/* 
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2001 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Sascha Schumann <sascha@schumann.cx>                        |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#include "php.h"

#ifdef HAVE_LIBMM

#include <mm.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include "php_session.h"
#include "mod_mm.h"

#define PS_MM_PATH "/tmp/session_mm"

/* For php_uint32 */
#include "ext/standard/basic_functions.h"

/*
 * this list holds all data associated with one session 
 */

typedef struct ps_sd {
	struct ps_sd *next;
	php_uint32 hv;		/* hash value of key */
	time_t ctime;		/* time of last change */
	char *key;
	void *data;
	size_t datalen;		/* amount of valid data */
	size_t alloclen;	/* amount of allocated memory for data */
} ps_sd;

typedef struct {
	MM *mm;
	ps_sd **hash;
} ps_mm;

static ps_mm *ps_mm_instance = NULL;

/* should be a prime */
#define HASH_SIZE 577

#if 0
#define ps_mm_debug(a) printf a
#else
#define ps_mm_debug(a)
#endif

static inline php_uint32 ps_sd_hash(const char *data)
{
	php_uint32 h;
	char c;
	
	for (h = 2166136261U; (c = *data++); ) {
		h *= 16777619;
		h ^= c;
	}
	
	return h;
}
	

static ps_sd *ps_sd_new(ps_mm *data, const char *key, const void *sdata, size_t sdatalen)
{
	php_uint32 hv, slot;
	ps_sd *sd;

	hv = ps_sd_hash(key);
	slot = hv % HASH_SIZE;
	
	sd = mm_malloc(data->mm, sizeof(*sd));
	if (!sd)
		return NULL;
	sd->ctime = 0;
	sd->hv = hv;
	
	sd->data = mm_malloc(data->mm, sdatalen);
	if (!sd->data) {
		mm_free(data->mm, sd);
		return NULL;
	}

	sd->alloclen = sd->datalen = sdatalen;
	
	sd->key = mm_strdup(data->mm, key);
	if (!sd->key) {
		mm_free(data->mm, sd->data);
		mm_free(data->mm, sd);
		return NULL;
	}
	
	memcpy(sd->data, sdata, sdatalen);
	
	sd->next = data->hash[slot];
	data->hash[slot] = sd;
	
	ps_mm_debug(("inserting %s(%p) into slot %d\n", key, sd, slot));

	return sd;
}

static void ps_sd_destroy(ps_mm *data, ps_sd *sd)
{
	php_uint32 slot;

	slot = ps_sd_hash(sd->key) % HASH_SIZE;

	if (data->hash[slot] == sd)
		data->hash[slot] = sd->next;
	else {
		ps_sd *prev;

		/* There must be some entry before the one we want to delete */
		for (prev = data->hash[slot]; prev->next != sd; prev = prev->next);
		prev->next = sd->next;
	}
		
	mm_free(data->mm, sd->key);
	if (sd->data) 
		mm_free(data->mm, sd->data);
	mm_free(data->mm, sd);
}

static ps_sd *ps_sd_lookup(ps_mm *data, const char *key, int rw)
{
	php_uint32 hv, slot;
	ps_sd *ret;

	hv = ps_sd_hash(key);
	slot = hv % HASH_SIZE;
	
	for (ret = data->hash[slot]; ret; ret = ret->next)
		if (ret->hv == hv && !strcmp(ret->key, key)) 
			break;

	if (ret && rw && ret != data->hash[slot]) {
		/* Move the entry to the top of the linked list */
		
		ret->next = data->hash[slot];
		data->hash[slot] = ret;
	}

	ps_mm_debug(("lookup(%s): ret=%p,hv=%u,slot=%d\n", key, ret, hv, slot));
	
	return ret;
}

ps_module ps_mod_mm = {
	PS_MOD(mm)
};

#define PS_MM_DATA ps_mm *data = PS_GET_MOD_DATA()

static int ps_mm_initialize(ps_mm *data, const char *path)
{
	data->mm = mm_create(0, path);
	if (!data->mm) {
		return FAILURE;
	}

	data->hash = mm_calloc(data->mm, HASH_SIZE, sizeof(*data->hash));
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

	for (h = 0; h < HASH_SIZE; h++)
		for (sd = data->hash[h]; sd; sd = next) {
			next = sd->next;
			ps_sd_destroy(data, sd);
		}
	
	mm_free(data->mm, data->hash);
	mm_destroy(data->mm);
}

PHP_MINIT_FUNCTION(ps_mm)
{
	ps_mm_instance = calloc(sizeof(*ps_mm_instance), 1);
	if (ps_mm_initialize(ps_mm_instance, PS_MM_PATH) != SUCCESS) {
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
		free(ps_mm_instance);
		return SUCCESS;
	}
	return FAILURE;
}

PS_OPEN_FUNC(mm)
{
	ps_mm_debug(("open: ps_mm_instance=%p\n", ps_mm_instance));
	
	if (!ps_mm_instance)
		return FAILURE;
	
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
	
	sd = ps_sd_lookup(data, key, 0);
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
		sd = ps_sd_new(data, key, val, vallen);
		ps_mm_debug(("new entry for %s\n", key));
	} else {
		ps_mm_debug(("found existing entry for %s\n", key));

		if (vallen >= sd->alloclen) {
			mm_free(data->mm, sd->data);
			sd->alloclen = vallen + 1;
			sd->data = mm_malloc(data->mm, sd->alloclen);

			if (!sd->data) {
				ps_sd_destroy(data, sd);
				sd = NULL;
			}
		}
		if (sd) {
			sd->datalen = vallen;
			memcpy(sd->data, val, vallen);
		}
	}

	if (sd)
		time(&sd->ctime);

	mm_unlock(data->mm);
	
	return sd ? SUCCESS : FAILURE;
}

PS_DESTROY_FUNC(mm)
{
	PS_MM_DATA;
	ps_sd *sd;
	
	mm_lock(data->mm, MM_LOCK_RW);
	
	sd = ps_sd_lookup(data, key, 0);
	if (sd)
		ps_sd_destroy(data, sd);
	
	mm_unlock(data->mm);
	
	return SUCCESS;
}

PS_GC_FUNC(mm) 
{
	PS_MM_DATA;
	int h;
	time_t limit;
	ps_sd *sd, *next;
	
	*nrdels = 0;
	ps_mm_debug(("gc\n"));
		
	time(&limit);

	limit -= maxlifetime;

	mm_lock(data->mm, MM_LOCK_RW);

	for (h = 0; h < HASH_SIZE; h++)
		for (sd = data->hash[h]; sd; sd = next) {
			next = sd->next;
			if (sd->ctime < limit) {
				ps_mm_debug(("purging %s\n", sd->key));
				ps_sd_destroy(data, sd);
				(*nrdels)++;
			}
		}

	mm_unlock(data->mm);
	
	return SUCCESS;
}

zend_module_entry php_session_mm_module = {
	"session mm",
	NULL,
	PHP_MINIT(ps_mm), PHP_MSHUTDOWN(ps_mm),
	NULL, NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES
};

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 tw=78 fdm=marker
 * vim<600: sw=4 ts=4 tw=78
 */
