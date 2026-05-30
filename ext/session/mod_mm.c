/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Author: Sascha Schumann <sascha@schumann.cx>                         |
   +----------------------------------------------------------------------+
 */

/* GH-14019: backend switched from libmm (shared memory, prefork-only, not
 * thread-safe, unmaintained upstream) to an in-process GLib GHashTable
 * guarded by a TSRM mutex. Trade-off: no more cross-process sharing, but
 * ZTS and Windows now work. Prefork users who relied on shared sessions
 * must switch to files/memcached/redis. */

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

#ifdef ZTS
static MUTEX_T session_mm_lock;
# define PS_MM_LOCK()   tsrm_mutex_lock(session_mm_lock)
# define PS_MM_UNLOCK() tsrm_mutex_unlock(session_mm_lock)
#else
# define PS_MM_LOCK()   ((void) 0)
# define PS_MM_UNLOCK() ((void) 0)
#endif

/* Per-session entry stored as the GHashTable value. */
typedef struct ps_sd {
	time_t ctime;       /* time of last change */
	void *data;
	size_t datalen;     /* amount of valid data */
	size_t alloclen;    /* amount of allocated memory for data */
} ps_sd;

typedef struct {
	GHashTable *hash;
} ps_mm;

typedef struct {
	time_t limit;
	zend_long nrdels;
} ps_mm_gc_ctx;

static ps_mm *ps_mm_instance = NULL;

#if 0
# define ps_mm_debug(a) printf a
#else
# define ps_mm_debug(a)
#endif

static guint ps_mm_hash(gconstpointer key)
{
	zend_ulong h = zend_string_hash_val((zend_string *) key);
	return (guint) (h ^ (h >> 32));
}

static gboolean ps_mm_key_equals(gconstpointer a, gconstpointer b)
{
	return zend_string_equals((const zend_string *) a, (const zend_string *) b);
}

static void ps_mm_key_free(gpointer key)
{
	zend_string_release_ex((zend_string *) key, false);
}

static void ps_mm_value_free(gpointer val)
{
	ps_sd *sd = (ps_sd *) val;
	if (sd->data) {
		g_free(sd->data);
	}
	g_free(sd);
}

static ps_sd *ps_sd_new(ps_mm *data, zend_string *key)
{
	ps_sd *sd = g_try_malloc0(sizeof(ps_sd));
	if (!sd) {
		php_error_docref(NULL, E_WARNING, "g_try_malloc failed");
		return NULL;
	}

	g_hash_table_insert(data->hash, zend_string_copy(key), sd);
	ps_mm_debug(("inserting %s(%p)\n", ZSTR_VAL(key), sd));

	return sd;
}

/* TODO: migrate to PS_MOD_UPDATE_TIMESTAMP(mm) + PS_UPDATE_TIMESTAMP_FUNC(mm)
 * so lazy_write can skip rewriting unchanged sessions. mod_files and mod_user
 * are already on the new API; mm is the last legacy holdout. */
const ps_module ps_mod_mm = {
	PS_MOD(mm)
};

#define PS_MM_DATA ps_mm *data = PS_GET_MOD_DATA()

static zend_result ps_mm_initialize(ps_mm *data)
{
	data->hash = g_hash_table_new_full(ps_mm_hash, ps_mm_key_equals,
	                                   ps_mm_key_free, ps_mm_value_free);
	if (!data->hash) {
		php_error_docref(NULL, E_WARNING, "hash table creation failed");
		return FAILURE;
	}

	return SUCCESS;
}

static void ps_mm_destroy(ps_mm *data)
{
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
		ps_mm_destroy(ps_mm_instance);
		ps_mm_instance = NULL;
#ifdef ZTS
		tsrm_mutex_free(session_mm_lock);
		session_mm_lock = NULL;
#endif
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

	PS_MM_LOCK();

	sd = g_hash_table_lookup(data->hash, key);
	if (sd) {
		*val = zend_string_init(sd->data, sd->datalen, false);
		ret = SUCCESS;
	}

	PS_MM_UNLOCK();

	return ret;
}

PS_WRITE_FUNC(mm)
{
	PS_MM_DATA;
	ps_sd *sd;
	zend_result ret = FAILURE;

	PS_MM_LOCK();

	sd = g_hash_table_lookup(data->hash, key);
	if (!sd) {
		sd = ps_sd_new(data, key);
		ps_mm_debug(("new entry for %s\n", ZSTR_VAL(key)));
	}

	if (sd) {
		if (val->len >= sd->alloclen) {
			size_t new_alloc = val->len + 1;
			void *new_data = g_try_realloc(sd->data, new_alloc);

			if (!new_data) {
				php_error_docref(NULL, E_WARNING, "Cannot allocate new data segment");
				/* sd and its old data are freed by the hash table's value destroy notifier. */
				g_hash_table_remove(data->hash, key);
				sd = NULL;
			} else {
				sd->data = new_data;
				sd->alloclen = new_alloc;
			}
		}
		if (sd) {
			sd->datalen = val->len;
			memcpy(sd->data, val->val, val->len);
			time(&sd->ctime);
			ret = SUCCESS;
		}
	}

	PS_MM_UNLOCK();
	return ret;
}

PS_DESTROY_FUNC(mm)
{
	PS_MM_DATA;

	PS_MM_LOCK();
	g_hash_table_remove(data->hash, key);
	PS_MM_UNLOCK();

	return SUCCESS;
}

static gboolean ps_mm_gc_check(gpointer key, gpointer val, gpointer priv)
{
	ps_mm_gc_ctx *ctx = (ps_mm_gc_ctx *) priv;
	ps_sd *sd = (ps_sd *) val;
	(void) key;

	if (sd->ctime < ctx->limit) {
		ctx->nrdels++;
		return TRUE;
	}
	return FALSE;
}

PS_GC_FUNC(mm)
{
	PS_MM_DATA;
	ps_mm_gc_ctx ctx;

	ps_mm_debug(("gc\n"));

	ctx.nrdels = 0;
	time(&ctx.limit);
	ctx.limit -= maxlifetime;

	PS_MM_LOCK();
	g_hash_table_foreach_remove(data->hash, ps_mm_gc_check, &ctx);
	PS_MM_UNLOCK();

	*nrdels = ctx.nrdels;
	return ctx.nrdels;
}

PS_CREATE_SID_FUNC(mm)
{
	zend_string *sid;
	int maxfail = 3;
	PS_MM_DATA;

	do {
		sid = php_session_create_id((void **) &data);
		if (!sid) {
			if (maxfail-- <= 0) {
				return NULL;
			}
			continue;
		}
		/* Check collision */
		PS_MM_LOCK();
		gboolean exists = g_hash_table_contains(data->hash, sid);
		PS_MM_UNLOCK();
		if (exists) {
			zend_string_release_ex(sid, false);
			sid = NULL;
			if (maxfail-- <= 0) {
				return NULL;
			}
		}
	} while (!sid);

	return sid;
}

/*
 * Check session ID existence for use_strict_mode support.
 * PARAMETERS: PS_VALIDATE_SID_ARGS in php_session.h
 * RETURN VALUE: SUCCESS or FAILURE.
 *
 * Return SUCCESS for valid key (already existing session).
 * Return FAILURE for invalid key (non-existing session).
 * *mod_data, *key are guaranteed to have non-NULL values.
 */
PS_VALIDATE_SID_FUNC(mm)
{
	PS_MM_DATA;
	gboolean found;

	PS_MM_LOCK();
	found = g_hash_table_contains(data->hash, key);
	PS_MM_UNLOCK();

	return found ? SUCCESS : FAILURE;
}

#endif
