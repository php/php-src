/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Sascha Schumann <sascha@schumann.cx>                         |
   +----------------------------------------------------------------------+
 */

#include "php.h"

#ifdef HAVE_LIBGLIB

#include <unistd.h>
#include <glib.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdint.h>

#include "php_session.h"
#include "mod_mm.h"
#include "SAPI.h"

#define PS_MM_FILE "session_mm_"

#ifdef ZTS
MUTEX_T session_mm_lock;
#endif

/* This list holds all data associated with one session. */

typedef struct ps_sd {
	time_t ctime;		/* time of last change */
	void *data;
	size_t datalen;		/* amount of valid data */
	size_t alloclen;	/* amount of allocated memory for data */
} ps_sd;

typedef struct {
	GHashTable *hash;
	pid_t owner;
} ps_mm;

typedef struct {
	ps_mm *data;
	zend_long maxlifetime;
	zend_long *nrdels;
} ps_timelimit;

static ps_mm *ps_mm_instance = NULL;

#if 0
# define ps_mm_debug(a) printf a
#else
# define ps_mm_debug(a)
#endif

static ps_sd *ps_sd_new(ps_mm *data, zend_string *key)
{
	ps_sd *sd;

	sd = g_try_malloc(sizeof(ps_sd) + ZSTR_LEN(key));
	if (!sd) {

		php_error_docref(NULL, E_WARNING, "g_malloc failed");
		return NULL;
	}

	sd->ctime = 0;
	sd->data = NULL;
	sd->alloclen = sd->datalen = 0;

	g_hash_table_insert(data->hash, key, sd);

	return sd;
}

static void ps_sd_destroy(ps_mm *data, zend_string *key, ps_sd *sd)
{
	if (sd->data) {
		g_free(sd->data);
	}

	g_hash_table_remove(data->hash, key);
}

const ps_module ps_mod_mm = {
	PS_MOD_SID(mm)
};

#define PS_MM_DATA ps_mm *data = PS_GET_MOD_DATA()

static guint ps_mm_hash(gconstpointer key) {
        return (guint)(zend_string_hash_val((zend_string *)key)); 
}

static gboolean ps_mm_key_equals(gconstpointer a, gconstpointer b) {
        return zend_string_equals((const zend_string *)a, (const zend_string *)b);
}

static zend_result ps_mm_initialize(ps_mm *data)
{
	data->owner = getpid();
	data->hash = g_hash_table_new(ps_mm_hash, ps_mm_key_equals);
	if (!data->hash) {
		php_error_docref(NULL, E_WARNING, "hash table created failed");
		return FAILURE;
	}

	return SUCCESS;
}

static gboolean ps_mm_destroy_entry(gpointer key, gpointer val, gpointer _priv)
{
	zend_string_release_ex((zend_string *)key, false);
	ps_sd *sd = (ps_sd *)val;
	g_free(sd);
	return TRUE;
}

static void ps_mm_timelimit(gpointer key, gpointer val, gpointer _priv)
{
	time_t limit;
	time(&limit);
	ps_timelimit *ps_tm = (ps_timelimit *)_priv;

	limit -= ps_tm->maxlifetime;

	ps_sd *sd = (ps_sd *)val;
	if (sd->ctime < limit) {
		ps_sd_destroy(ps_tm->data, (zend_string *)key, sd);
		(*ps_tm->nrdels)++;
	}
}

static void ps_mm_destroy(ps_mm *data)
{
	/* This function is called during each module shutdown,
	   but we must not release the shared memory pool, when
	   an Apache child dies! */
	if (data->owner != getpid()) {
		return;
	}

	g_hash_table_foreach_remove(data->hash, ps_mm_destroy_entry, NULL);

	g_hash_table_destroy(data->hash);
	pefree(data, true);
}

PHP_MINIT_FUNCTION(ps_mm)
{
	ps_mm_instance = pecalloc(1, sizeof(*ps_mm_instance), true);
	if (!ps_mm_instance) {
		return FAILURE;
	}

	if (ps_mm_initialize(ps_mm_instance) == FAILURE) {
		pefree(ps_mm_instance, true);
		ps_mm_instance = NULL;
		return FAILURE;
	}

#ifdef ZTS
	session_mm_lock = tsrm_mutex_alloc();
#endif

	php_session_register_module(&ps_mod_mm);
	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(ps_mm)
{
	if (ps_mm_instance) {
#ifdef ZTS
		tsrm_mutex_free(session_mm_lock);
		session_mm_lock = NULL;
#endif
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
	zend_result ret = FAILURE;

	/* If there is an ID and strict mode, verify existence */
	if (PS(use_strict_mode)
		&& g_hash_table_contains(data->hash, key) == FAILURE) {
		/* key points to PS(id), but cannot change here. */
		if (key) {
			efree(PS(id));
			PS(id) = NULL;
		}
		PS(id) = PS(mod)->s_create_sid((void **)&data);
		if (!PS(id)) {
			return FAILURE;
		}
		if (PS(use_cookies)) {
			PS(send_cookie) = 1;
		}
		php_session_reset_id();
		PS(session_status) = php_session_active;
	}

	sd = g_hash_table_lookup(data->hash, PS(id));
	if (sd) {
		*val = zend_string_init(sd->data, sd->datalen, false);
		ret = SUCCESS;
	}

	return ret;
}

PS_WRITE_FUNC(mm)
{
	PS_MM_DATA;
	ps_sd *sd;

#ifdef ZTS
	tsrm_mutex_lock(session_mm_lock);
#endif

	sd = g_hash_table_lookup(data->hash, key);
	if (!sd) {
		sd = ps_sd_new(data, key);
		ps_mm_debug(("new entry for %s\n", ZSTR_VAL(key)));
	}

	if (sd) {
		if (val->len >= sd->alloclen) {
			sd->alloclen = val->len + 1;
			sd->data = g_realloc(sd->data, sd->alloclen);

			if (!sd->data) {
				ps_sd_destroy(data, key, sd);
				php_error_docref(NULL, E_WARNING, "Cannot allocate new data segment");
				sd = NULL;
			}
		}
		if (sd) {
			sd->datalen = val->len;
			memcpy(sd->data, val->val, val->len);
			time(&sd->ctime);
		}
	}

#ifdef ZTS
	tsrm_mutex_unlock(session_mm_lock);
#endif
	return sd ? SUCCESS : FAILURE;
}

PS_DESTROY_FUNC(mm)
{
	PS_MM_DATA;
	ps_sd *sd;

#ifdef ZTS
	tsrm_mutex_lock(session_mm_lock);
#endif

	sd = g_hash_table_lookup(data->hash, key);
	if (sd) {
		ps_sd_destroy(data, key, sd);
	}

#ifdef ZTS
	tsrm_mutex_unlock(session_mm_lock);
#endif
	return SUCCESS;
}

PS_GC_FUNC(mm)
{
	PS_MM_DATA;

	*nrdels = 0;
	ps_mm_debug(("gc\n"));

	ps_timelimit ps_tm;

	ps_tm.data = data;
	ps_tm.maxlifetime = maxlifetime;
	ps_tm.nrdels = nrdels;


	g_hash_table_foreach(data->hash, ps_mm_timelimit, &ps_tm);

	return *nrdels;
}

PS_CREATE_SID_FUNC(mm)
{
	zend_string *sid;
	int maxfail = 3;
	PS_MM_DATA;

	do {
		sid = php_session_create_id((void **)&data);
		if (!sid) {
			if (--maxfail < 0) {
				return NULL;
			} else {
				continue;
			}
		}
		/* Check collision */
		if (g_hash_table_contains(data->hash, sid) == SUCCESS) {
			if (sid) {
				zend_string_release_ex(sid, 0);
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
