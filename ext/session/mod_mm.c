/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2014 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Sascha Schumann <sascha@schumann.cx>                         |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#include "php.h"

#ifdef HAVE_LIBMM

#include <unistd.h>
#include <mm.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include "php_session.h"
#include "mod_mm.h"
#include "SAPI.h"

#ifdef ZTS
# error mm is not thread-safe
#endif

#define PS_MM_FILE "session_mm_"

/* For php_uint32 */
#include "ext/standard/basic_functions.h"

/* This list holds all data associated with one session. */

typedef struct ps_sd {
	struct ps_sd *next;
	php_uint32 hv;		/* hash value of key */
	time_t ctime;		/* time of last change */
	void *data;
	size_t datalen;		/* amount of valid data */
	size_t alloclen;	/* amount of allocated memory for data */
	char key[1];		/* inline key */
} ps_sd;

typedef struct {
	MM *mm;
	ps_sd **hash;
	php_uint32 hash_max;
	php_uint32 hash_cnt;
	pid_t owner;
} ps_mm;

static ps_mm *ps_mm_instance = NULL;

#if 0
# define ps_mm_debug(a) printf a
#else
# define ps_mm_debug(a)
#endif

static inline php_uint32 ps_sd_hash(const char *data, int len)
{
	php_uint32 h;
	const char *e = data + len;

	for (h = 2166136261U; data < e; ) {
		h *= 16777619;
		h ^= *data++;
	}

	return h;
}

static void hash_split(ps_mm *data)
{
	php_uint32 nmax;
	ps_sd **nhash;
	ps_sd **ohash, **ehash;
	ps_sd *ps, *next;

	nmax = ((data->hash_max + 1) << 1) - 1;
	nhash = mm_calloc(data->mm, nmax + 1, sizeof(*data->hash));

	if (!nhash) {
		/* no further memory to expand hash table */
		return;
	}

	ehash = data->hash + data->hash_max + 1;
	for (ohash = data->hash; ohash < ehash; ohash++) {
		for (ps = *ohash; ps; ps = next) {
			next = ps->next;
			ps->next = nhash[ps->hv & nmax];
			nhash[ps->hv & nmax] = ps;
		}
	}
	mm_free(data->mm, data->hash);

	data->hash = nhash;
	data->hash_max = nmax;
}

static ps_sd *ps_sd_new(ps_mm *data, const char *key)
{
	php_uint32 hv, slot;
	ps_sd *sd;
	int keylen;

	keylen = strlen(key);

	sd = mm_malloc(data->mm, sizeof(ps_sd) + keylen);
	if (!sd) {
		TSRMLS_FETCH();

		php_error_docref(NULL TSRMLS_CC, E_WARNING, "mm_malloc failed, avail %ld, err %s", mm_available(data->mm), mm_error());
		return NULL;
	}

	hv = ps_sd_hash(key, keylen);
	slot = hv & data->hash_max;

	sd->ctime = 0;
	sd->hv = hv;
	sd->data = NULL;
	sd->alloclen = sd->datalen = 0;

	memcpy(sd->key, key, keylen + 1);

	sd->next = data->hash[slot];
	data->hash[slot] = sd;

	data->hash_cnt++;

	if (!sd->next) {
		if (data->hash_cnt >= data->hash_max) {
			hash_split(data);
		}
	}

	ps_mm_debug(("inserting %s(%p) into slot %d\n", key, sd, slot));

	return sd;
}

static void ps_sd_destroy(ps_mm *data, ps_sd *sd)
{
	php_uint32 slot;

	slot = ps_sd_hash(sd->key, strlen(sd->key)) & data->hash_max;

	if (data->hash[slot] == sd) {
		data->hash[slot] = sd->next;
	} else {
		ps_sd *prev;

		/* There must be some entry before the one we want to delete */
		for (prev = data->hash[slot]; prev->next != sd; prev = prev->next);
		prev->next = sd->next;
	}

	data->hash_cnt--;

	if (sd->data) {
		mm_free(data->mm, sd->data);
	}

	mm_free(data->mm, sd);
}

static ps_sd *ps_sd_lookup(ps_mm *data, const char *key, int rw)
{
	php_uint32 hv, slot;
	ps_sd *ret, *prev;

	hv = ps_sd_hash(key, strlen(key));
	slot = hv & data->hash_max;

	for (prev = NULL, ret = data->hash[slot]; ret; prev = ret, ret = ret->next) {
		if (ret->hv == hv && !strcmp(ret->key, key)) {
			break;
		}
	}

	if (ret && rw && ret != data->hash[slot]) {
		/* Move the entry to the top of the linked list */
		if (prev) {
			prev->next = ret->next;
		}

		ret->next = data->hash[slot];
		data->hash[slot] = ret;
	}

	ps_mm_debug(("lookup(%s): ret=%p,hv=%u,slot=%d\n", key, ret, hv, slot));

	return ret;
}

static int ps_mm_key_exists(ps_mm *data, const char *key TSRMLS_DC)
{
	ps_sd *sd;

	if (!key) {
		return FAILURE;
	}
	sd = ps_sd_lookup(data, key, 0);
	if (sd) {
		return SUCCESS;
	}
	return FAILURE;
}

ps_module ps_mod_mm = {
	PS_MOD_SID(mm)
};

#define PS_MM_DATA ps_mm *data = PS_GET_MOD_DATA()

static int ps_mm_initialize(ps_mm *data, const char *path)
{
	data->owner = getpid();
	data->mm = mm_create(0, path);
	if (!data->mm) {
		return FAILURE;
	}

	data->hash_cnt = 0;
	data->hash_max = 511;
	data->hash = mm_calloc(data->mm, data->hash_max + 1, sizeof(ps_sd *));
	if (!data->hash) {
		mm_destroy(data->mm);
		return FAILURE;
	}

	return SUCCESS;
}

static void ps_mm_destroy(ps_mm *data)
{
	int h;
	ps_sd *sd, *next;

	/* This function is called during each module shutdown,
	   but we must not release the shared memory pool, when
	   an Apache child dies! */
	if (data->owner != getpid()) {
		return;
	}

	for (h = 0; h < data->hash_max + 1; h++) {
		for (sd = data->hash[h]; sd; sd = next) {
			next = sd->next;
			ps_sd_destroy(data, sd);
		}
	}

	mm_free(data->mm, data->hash);
	mm_destroy(data->mm);
	free(data);
}

PHP_MINIT_FUNCTION(ps_mm)
{
	int save_path_len = strlen(PS(save_path));
	int mod_name_len = strlen(sapi_module.name);
	int euid_len;
	char *ps_mm_path, euid[30];
	int ret;

	ps_mm_instance = calloc(sizeof(*ps_mm_instance), 1);
	if (!ps_mm_instance) {
		return FAILURE;
	}

	if (!(euid_len = slprintf(euid, sizeof(euid), "%d", geteuid()))) {
		free(ps_mm_instance);
		ps_mm_instance = NULL;
		return FAILURE;
	}

	/* Directory + '/' + File + Module Name + Effective UID + \0 */
	ps_mm_path = emalloc(save_path_len + 1 + (sizeof(PS_MM_FILE) - 1) + mod_name_len + euid_len + 1);

	memcpy(ps_mm_path, PS(save_path), save_path_len);
	if (save_path_len && PS(save_path)[save_path_len - 1] != DEFAULT_SLASH) {
		ps_mm_path[save_path_len] = DEFAULT_SLASH;
		save_path_len++;
	}
	memcpy(ps_mm_path + save_path_len, PS_MM_FILE, sizeof(PS_MM_FILE) - 1);
	save_path_len += sizeof(PS_MM_FILE) - 1;
	memcpy(ps_mm_path + save_path_len, sapi_module.name, mod_name_len);
	save_path_len += mod_name_len;
	memcpy(ps_mm_path + save_path_len, euid, euid_len);
	ps_mm_path[save_path_len + euid_len] = '\0';

	ret = ps_mm_initialize(ps_mm_instance, ps_mm_path);

	efree(ps_mm_path);

	if (ret != SUCCESS) {
		free(ps_mm_instance);
		ps_mm_instance = NULL;
		return FAILURE;
	}

	php_session_register_module(&ps_mod_mm);
	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(ps_mm)
{
	if (ps_mm_instance) {
		ps_mm_destroy(ps_mm_instance);
		return SUCCESS;
	}
	return FAILURE;
}

PS_OPEN_FUNC(mm)
{
	ps_mm_debug(("open: ps_mm_instance=%p\n", ps_mm_instance));

	if (!ps_mm_instance) {
		return FAILURE;
	}
	PS_SET_MOD_DATA(ps_mm_instance);

	return SUCCESS;
}

PS_CLOSE_FUNC(mm)
{
	PS_SET_MOD_DATA(NULL);

	return SUCCESS;
}

PS_READ_FUNC(mm)
{
	PS_MM_DATA;
	ps_sd *sd;
	int ret = FAILURE;

	mm_lock(data->mm, MM_LOCK_RD);

	/* If there is an ID and strict mode, verify existence */
	if (PS(use_strict_mode)
		&& ps_mm_key_exists(data, key TSRMLS_CC) == FAILURE) {
		/* key points to PS(id), but cannot change here. */
		if (key) {
			efree(PS(id));
			PS(id) = NULL;
		}
		PS(id) = PS(mod)->s_create_sid((void **)&data, NULL TSRMLS_CC);
		if (!PS(id)) {
			return FAILURE;
		}
		if (PS(use_cookies)) {
			PS(send_cookie) = 1;
		}
		php_session_reset_id(TSRMLS_C);
	}

	sd = ps_sd_lookup(data, PS(id), 0);
	if (sd) {
		*vallen = sd->datalen;
		*val = emalloc(sd->datalen + 1);
		memcpy(*val, sd->data, sd->datalen);
		(*val)[sd->datalen] = '\0';
		ret = SUCCESS;
	}

	mm_unlock(data->mm);

	return ret;
}

PS_WRITE_FUNC(mm)
{
	PS_MM_DATA;
	ps_sd *sd;

	mm_lock(data->mm, MM_LOCK_RW);

	sd = ps_sd_lookup(data, key, 1);
	if (!sd) {
		sd = ps_sd_new(data, key);
		ps_mm_debug(("new entry for %s\n", key));
	}

	if (sd) {
		if (vallen >= sd->alloclen) {
			if (data->mm) {
				mm_free(data->mm, sd->data);
			}
			sd->alloclen = vallen + 1;
			sd->data = mm_malloc(data->mm, sd->alloclen);

			if (!sd->data) {
				ps_sd_destroy(data, sd);
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "cannot allocate new data segment");
				sd = NULL;
			}
		}
		if (sd) {
			sd->datalen = vallen;
			memcpy(sd->data, val, vallen);
			time(&sd->ctime);
		}
	}

	mm_unlock(data->mm);

	return sd ? SUCCESS : FAILURE;
}

PS_DESTROY_FUNC(mm)
{
	PS_MM_DATA;
	ps_sd *sd;

	mm_lock(data->mm, MM_LOCK_RW);

	sd = ps_sd_lookup(data, key, 0);
	if (sd) {
		ps_sd_destroy(data, sd);
	}

	mm_unlock(data->mm);

	return SUCCESS;
}

PS_GC_FUNC(mm)
{
	PS_MM_DATA;
	time_t limit;
	ps_sd **ohash, **ehash;
	ps_sd *sd, *next;

	*nrdels = 0;
	ps_mm_debug(("gc\n"));

	time(&limit);

	limit -= maxlifetime;

	mm_lock(data->mm, MM_LOCK_RW);

	ehash = data->hash + data->hash_max + 1;
	for (ohash = data->hash; ohash < ehash; ohash++) {
		for (sd = *ohash; sd; sd = next) {
			next = sd->next;
			if (sd->ctime < limit) {
				ps_mm_debug(("purging %s\n", sd->key));
				ps_sd_destroy(data, sd);
				(*nrdels)++;
			}
		}
	}

	mm_unlock(data->mm);

	return SUCCESS;
}

PS_CREATE_SID_FUNC(mm)
{
	char *sid;
	int maxfail = 3;
	PS_MM_DATA;

	do {
		sid = php_session_create_id((void **)&data, newlen TSRMLS_CC);
		/* Check collision */
		if (ps_mm_key_exists(data, sid TSRMLS_CC) == SUCCESS) {
			if (sid) {
				efree(sid);
				sid = NULL;
			}
			if (!(maxfail--)) {
				return NULL;
			}
		}
	} while(!sid);

	return sid;
}

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
