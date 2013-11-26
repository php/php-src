/*
   +----------------------------------------------------------------------+
   | Zend OPcache                                                         |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2013 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   |          Stanislav Malyshev <stas@zend.com>                          |
   |          Dmitry Stogov <dmitry@zend.com>                             |
   +----------------------------------------------------------------------+
*/

#include "main/php.h"
#include "main/php_globals.h"
#include "zend.h"
#include "zend_extensions.h"
#include "zend_compile.h"
#include "ZendAccelerator.h"
#include "zend_persist.h"
#include "zend_shared_alloc.h"
#include "zend_accelerator_module.h"
#include "zend_accelerator_blacklist.h"
#include "zend_list.h"
#include "zend_execute.h"
#include "main/SAPI.h"
#include "main/php_streams.h"
#include "main/php_open_temporary_file.h"
#include "zend_API.h"
#include "zend_ini.h"
#include "zend_virtual_cwd.h"
#include "zend_accelerator_util_funcs.h"
#include "zend_accelerator_hash.h"

#ifndef ZEND_WIN32
#include  <netdb.h>
#endif

#ifdef ZEND_WIN32
typedef int uid_t;
typedef int gid_t;
#include <io.h>
#endif

#ifndef ZEND_WIN32
# include <sys/time.h>
#else
# include <process.h>
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#include <fcntl.h>
#include <signal.h>
#include <time.h>

#ifndef ZEND_WIN32
# include <sys/types.h>
# include <sys/ipc.h>
#endif

#include <sys/stat.h>
#include <errno.h>

#define SHM_PROTECT() \
	do { \
		if (ZCG(accel_directives).protect_memory) { \
			zend_accel_shared_protect(1 TSRMLS_CC); \
		} \
	} while (0)
#define SHM_UNPROTECT() \
	do { \
		if (ZCG(accel_directives).protect_memory) { \
			zend_accel_shared_protect(0 TSRMLS_CC); \
		} \
	} while (0)

ZEND_EXTENSION();

#ifndef ZTS
zend_accel_globals accel_globals;
#else
int accel_globals_id;
#endif

/* Points to the structure shared across all PHP processes */
zend_accel_shared_globals *accel_shared_globals = NULL;

/* true globals, no need for thread safety */
zend_bool accel_startup_ok = 0;
static char *zps_failure_reason = NULL;
char *zps_api_failure_reason = NULL;

static zend_op_array *(*accelerator_orig_compile_file)(zend_file_handle *file_handle, int type TSRMLS_DC);
static int (*accelerator_orig_zend_stream_open_function)(const char *filename, zend_file_handle *handle  TSRMLS_DC);
#if ZEND_EXTENSION_API_NO >= PHP_5_3_X_API_NO
static char *(*accelerator_orig_zend_resolve_path)(const char *filename, int filename_len TSRMLS_DC);
#endif
static void (*orig_chdir)(INTERNAL_FUNCTION_PARAMETERS) = NULL;
static ZEND_INI_MH((*orig_include_path_on_modify)) = NULL;

#if ZEND_EXTENSION_API_NO < PHP_5_3_X_API_NO
static char *accel_php_resolve_path(const char *filename, int filename_length, const char *path TSRMLS_DC);
#endif

#ifdef ZEND_WIN32
# define INCREMENT(v) InterlockedIncrement(&ZCSG(v))
# define DECREMENT(v) InterlockedDecrement(&ZCSG(v))
# define LOCKVAL(v)   (ZCSG(v))
#endif

#ifdef ZEND_WIN32
static time_t zend_accel_get_time(void)
{
	FILETIME now;
	GetSystemTimeAsFileTime(&now);

	return (time_t) ((((((__int64)now.dwHighDateTime) << 32)|now.dwLowDateTime) - 116444736000000000L)/10000000);
}
#else
# define zend_accel_get_time() time(NULL)
#endif

static inline int is_stream_path(const char *filename)
{
	const char *p;

	for (p = filename; isalnum((int)*p) || *p == '+' || *p == '-' || *p == '.'; p++);
	return ((*p == ':') && (p - filename > 1) && (p[1] == '/') && (p[2] == '/'));
}

static inline int is_cacheable_stream_path(const char *filename)
{
	return memcmp(filename, "file://", sizeof("file://") - 1) == 0 ||
	       memcmp(filename, "phar://", sizeof("phar://") - 1) == 0;
}

/* O+ overrides PHP chdir() function and remembers the current working directory
 * in ZCG(cwd) and ZCG(cwd_len). Later accel_getcwd() can use stored value and
 * avoid getcwd() call.
 */
static ZEND_FUNCTION(accel_chdir)
{
	char cwd[MAXPATHLEN];

	orig_chdir(INTERNAL_FUNCTION_PARAM_PASSTHRU);
	if (VCWD_GETCWD(cwd, MAXPATHLEN)) {
		if (ZCG(cwd)) {
			efree(ZCG(cwd));
		}
		ZCG(cwd_len) = strlen(cwd);
		ZCG(cwd) = estrndup(cwd, ZCG(cwd_len));
	} else {
		if (ZCG(cwd)) {
			efree(ZCG(cwd));
			ZCG(cwd) = NULL;
		}
	}
}

static inline char* accel_getcwd(int *cwd_len TSRMLS_DC)
{
	if (ZCG(cwd)) {
		*cwd_len = ZCG(cwd_len);
		return ZCG(cwd);
	} else {
		char cwd[MAXPATHLEN + 1];

		if (!VCWD_GETCWD(cwd, MAXPATHLEN)) {
			return NULL;
		}
		*cwd_len = ZCG(cwd_len) = strlen(cwd);
		ZCG(cwd) = estrndup(cwd, ZCG(cwd_len));
		return ZCG(cwd);
	}
}

void zend_accel_schedule_restart_if_necessary(zend_accel_restart_reason reason TSRMLS_DC)
{
	if ((((double) ZSMMG(wasted_shared_memory)) / ZCG(accel_directives).memory_consumption) >= ZCG(accel_directives).max_wasted_percentage) {
 		zend_accel_schedule_restart(reason TSRMLS_CC);
	}
}

/* O+ tracks changes of "include_path" directive. It stores all the requested
 * values in ZCG(include_paths) shared hash table, current value in
 * ZCG(include_path)/ZCG(include_path_len) and one letter "path key" in
 * ZCG(include_path_key).
 */
static ZEND_INI_MH(accel_include_path_on_modify)
{
	int ret = orig_include_path_on_modify(entry, new_value, new_value_length, mh_arg1, mh_arg2, mh_arg3, stage TSRMLS_CC);

	ZCG(include_path_key) = NULL;
	if (ret == SUCCESS) {
		ZCG(include_path) = new_value;
		if (ZCG(include_path) && *ZCG(include_path)) {
			ZCG(include_path_len) = new_value_length;

			if (ZCG(enabled) && accel_startup_ok &&
			    (ZCG(counted) || ZCSG(accelerator_enabled))) {

				ZCG(include_path_key) = zend_accel_hash_find(&ZCSG(include_paths), ZCG(include_path), ZCG(include_path_len) + 1);
			    if (!ZCG(include_path_key) &&
			        !zend_accel_hash_is_full(&ZCSG(include_paths))) {
					SHM_UNPROTECT();
					zend_shared_alloc_lock(TSRMLS_C);

					ZCG(include_path_key) = zend_accel_hash_find(&ZCSG(include_paths), ZCG(include_path), ZCG(include_path_len) + 1);
				    if (!ZCG(include_path_key) &&
					    !zend_accel_hash_is_full(&ZCSG(include_paths))) {
						char *key;

						key = zend_shared_alloc(ZCG(include_path_len) + 2);
						if (key) {
							memcpy(key, ZCG(include_path), ZCG(include_path_len) + 1);
							key[ZCG(include_path_len) + 1] = 'A' + ZCSG(include_paths).num_entries;
							ZCG(include_path_key) = key + ZCG(include_path_len) + 1;
							zend_accel_hash_update(&ZCSG(include_paths), key, ZCG(include_path_len) + 1, 0, ZCG(include_path_key));
						} else {
							zend_accel_schedule_restart_if_necessary(ACCEL_RESTART_OOM TSRMLS_CC);
						}
					}

					zend_shared_alloc_unlock(TSRMLS_C);
					SHM_PROTECT();
				}
			} else {
				ZCG(include_path_check) = 1;
			}
		} else {
			ZCG(include_path) = "";
			ZCG(include_path_len) = 0;
		}
	}
	return ret;
}

#if ZEND_EXTENSION_API_NO > PHP_5_3_X_API_NO
/* Interned strings support */
static char *orig_interned_strings_start;
static char *orig_interned_strings_end;
static const char *(*orig_new_interned_string)(const char *str, int len, int free_src TSRMLS_DC);
static void (*orig_interned_strings_snapshot)(TSRMLS_D);
static void (*orig_interned_strings_restore)(TSRMLS_D);

/* O+ disables creation of interned strings by regular PHP compiler, instead,
 * it creates interned strings in shared memory when saves a script.
 * Such interned strings are shared across all PHP processes
 */
static const char *accel_new_interned_string_for_php(const char *str, int len, int free_src TSRMLS_DC)
{
	return str;
}

static void accel_interned_strings_snapshot_for_php(TSRMLS_D)
{
}

static void accel_interned_strings_restore_for_php(TSRMLS_D)
{
}

#ifndef ZTS
static void accel_interned_strings_restore_state(TSRMLS_D)
{
	unsigned int i;

	for (i = 0; i < ZCSG(interned_strings).nTableSize; i++) {
		ZCSG(interned_strings).arBuckets[i] = ZCSG(interned_strings_saved_state).arBuckets[i];
		if (ZCSG(interned_strings).arBuckets[i]) {
			ZCSG(interned_strings).arBuckets[i]->pLast = NULL;
		}
	}
	ZCSG(interned_strings).pListHead = ZCSG(interned_strings_saved_state).pListHead;
	ZCSG(interned_strings).pListTail = ZCSG(interned_strings_saved_state).pListTail;
	if (ZCSG(interned_strings).pListHead) {
		ZCSG(interned_strings).pListHead->pListLast = NULL;
	}
	if (ZCSG(interned_strings).pListTail) {
		ZCSG(interned_strings).pListTail->pListNext = NULL;
	}
	ZCSG(interned_strings_top) = ZCSG(interned_strings_saved_state).top;
}

static void accel_interned_strings_save_state(TSRMLS_D)
{
	ZCSG(interned_strings_saved_state).arBuckets = (Bucket**)zend_shared_alloc(ZCSG(interned_strings).nTableSize * sizeof(Bucket *));
	if (!ZCSG(interned_strings_saved_state).arBuckets) {
		zend_accel_error(ACCEL_LOG_FATAL, "Insufficient shared memory!");
	}
	memcpy(ZCSG(interned_strings_saved_state).arBuckets, ZCSG(interned_strings).arBuckets, ZCSG(interned_strings).nTableSize * sizeof(Bucket *));
	ZCSG(interned_strings_saved_state).pListHead = ZCSG(interned_strings).pListHead;
	ZCSG(interned_strings_saved_state).pListTail = ZCSG(interned_strings).pListTail;
	ZCSG(interned_strings_saved_state).top = ZCSG(interned_strings_top);
}
#endif

const char *accel_new_interned_string(const char *arKey, int nKeyLength, int free_src TSRMLS_DC)
{
/* for now interned strings are supported only for non-ZTS build */
#ifndef ZTS
	ulong h;
	uint nIndex;
	Bucket *p;

	if (arKey >= ZCSG(interned_strings_start) && arKey < ZCSG(interned_strings_end)) {
		/* this is already an interned string */
		return arKey;
	}

	h = zend_inline_hash_func(arKey, nKeyLength);
	nIndex = h & ZCSG(interned_strings).nTableMask;

	/* check for existing interned string */
	p = ZCSG(interned_strings).arBuckets[nIndex];
	while (p != NULL) {
		if ((p->h == h) && (p->nKeyLength == (uint)nKeyLength)) {
			if (!memcmp(p->arKey, arKey, nKeyLength)) {
				if (free_src) {
					efree((char*)arKey);
				}
				return p->arKey;
			}
		}
		p = p->pNext;
	}

	if (ZCSG(interned_strings_top) + ZEND_MM_ALIGNED_SIZE(sizeof(Bucket) + nKeyLength) >=
	    ZCSG(interned_strings_end)) {
	    /* no memory, return the same non-interned string */
		return arKey;
	}

	/* create new interning string in shared interned strings buffer */
	p = (Bucket *) ZCSG(interned_strings_top);
	ZCSG(interned_strings_top) += ZEND_MM_ALIGNED_SIZE(sizeof(Bucket) + nKeyLength);

	p->arKey = (char*)(p + 1);
	memcpy((char*)p->arKey, arKey, nKeyLength);
	p->nKeyLength = nKeyLength;
	p->h = h;
	p->pData = &p->pDataPtr;
	p->pDataPtr = p;

	p->pNext = ZCSG(interned_strings).arBuckets[nIndex];
	p->pLast = NULL;
	if (p->pNext) {
		p->pNext->pLast = p;
	}
	ZCSG(interned_strings).arBuckets[nIndex] = p;

	p->pListLast = ZCSG(interned_strings).pListTail;
	ZCSG(interned_strings).pListTail = p;
	p->pListNext = NULL;
	if (p->pListLast != NULL) {
		p->pListLast->pListNext = p;
	}
	if (!ZCSG(interned_strings).pListHead) {
		ZCSG(interned_strings).pListHead = p;
	}

	ZCSG(interned_strings).nNumOfElements++;

	if (free_src) {
		efree((char*)arKey);
	}

	return p->arKey;
#else
	return arKey;
#endif
}

#ifndef ZTS
/* Copy PHP interned strings from PHP process memory into the shared memory */
static void accel_use_shm_interned_strings(TSRMLS_D)
{
	Bucket *p, *q;

	/* function table hash keys */
	p = CG(function_table)->pListHead;
	while (p) {
		if (p->nKeyLength) {
			p->arKey = accel_new_interned_string(p->arKey, p->nKeyLength, 0 TSRMLS_CC);
		}
		p = p->pListNext;
	}

	/* class table hash keys, class names, properties, methods, constants, etc */
	p = CG(class_table)->pListHead;
	while (p) {
		zend_class_entry *ce = (zend_class_entry*)(p->pDataPtr);

		if (p->nKeyLength) {
			p->arKey = accel_new_interned_string(p->arKey, p->nKeyLength, 0 TSRMLS_CC);
		}

		if (ce->name) {
			ce->name = accel_new_interned_string(ce->name, ce->name_length + 1, 0 TSRMLS_CC);
		}

		q = ce->properties_info.pListHead;
		while (q) {
			zend_property_info *info = (zend_property_info*)(q->pData);

			if (q->nKeyLength) {
				q->arKey = accel_new_interned_string(q->arKey, q->nKeyLength, 0 TSRMLS_CC);
			}

			if (info->name) {
				info->name = accel_new_interned_string(info->name, info->name_length + 1, 0 TSRMLS_CC);
			}

			q = q->pListNext;
		}

		q =	ce->function_table.pListHead;
		while (q) {
			if (q->nKeyLength) {
				q->arKey = accel_new_interned_string(q->arKey, q->nKeyLength, 0 TSRMLS_CC);
			}
			q = q->pListNext;
		}

		q =	ce->constants_table.pListHead;
		while (q) {
			if (q->nKeyLength) {
				q->arKey = accel_new_interned_string(q->arKey, q->nKeyLength, 0 TSRMLS_CC);
			}
			q = q->pListNext;
		}

		p = p->pListNext;
	}

	/* constant hash keys */
	p = EG(zend_constants)->pListHead;
	while (p) {
		if (p->nKeyLength) {
			p->arKey = accel_new_interned_string(p->arKey, p->nKeyLength, 0 TSRMLS_CC);
		}
		p = p->pListNext;
	}

	/* auto globals hash keys and names */
	p = CG(auto_globals)->pListHead;
	while (p) {
		zend_auto_global *auto_global = (zend_auto_global*)p->pData;

		auto_global->name = accel_new_interned_string(auto_global->name, auto_global->name_len + 1, 0 TSRMLS_CC);
		if (p->nKeyLength) {
			p->arKey = accel_new_interned_string(p->arKey, p->nKeyLength, 0 TSRMLS_CC);
		}
		p = p->pListNext;
	}
}
#endif
#endif

static inline void accel_restart_enter(TSRMLS_D)
{
#ifdef ZEND_WIN32
	INCREMENT(restart_in);
#else
	static const FLOCK_STRUCTURE(restart_in_progress, F_WRLCK, SEEK_SET, 2, 1);

	if (fcntl(lock_file, F_SETLK, &restart_in_progress) == -1) {
		zend_accel_error(ACCEL_LOG_DEBUG, "RestartC(+1):  %s (%d)", strerror(errno), errno);
	}
#endif
	ZCSG(restart_in_progress) = 1;
}

static inline void accel_restart_leave(TSRMLS_D)
{
#ifdef ZEND_WIN32
	ZCSG(restart_in_progress) = 0;
	DECREMENT(restart_in);
#else
	static const FLOCK_STRUCTURE(restart_finished, F_UNLCK, SEEK_SET, 2, 1);

	ZCSG(restart_in_progress) = 0;
	if (fcntl(lock_file, F_SETLK, &restart_finished) == -1) {
		zend_accel_error(ACCEL_LOG_DEBUG, "RestartC(-1):  %s (%d)", strerror(errno), errno);
	}
#endif
}

static inline int accel_restart_is_active(TSRMLS_D)
{
	if (ZCSG(restart_in_progress)) {
#ifndef ZEND_WIN32
		FLOCK_STRUCTURE(restart_check, F_WRLCK, SEEK_SET, 2, 1);

		if (fcntl(lock_file, F_GETLK, &restart_check) == -1) {
			zend_accel_error(ACCEL_LOG_DEBUG, "RestartC:  %s (%d)", strerror(errno), errno);
			return FAILURE;
		}
		if (restart_check.l_type == F_UNLCK) {
			ZCSG(restart_in_progress) = 0;
			return 0;
		} else {
			return 1;
		}
#else
		return LOCKVAL(restart_in) != 0;
#endif
	}
	return 0;
}

/* Creates a read lock for SHM access */
static inline void accel_activate_add(TSRMLS_D)
{
#ifdef ZEND_WIN32
	INCREMENT(mem_usage);
#else
	static const FLOCK_STRUCTURE(mem_usage_lock, F_RDLCK, SEEK_SET, 1, 1);

	if (fcntl(lock_file, F_SETLK, &mem_usage_lock) == -1) {
		zend_accel_error(ACCEL_LOG_DEBUG, "UpdateC(+1):  %s (%d)", strerror(errno), errno);
	}
#endif
}

/* Releases a lock for SHM access */
static inline void accel_deactivate_sub(TSRMLS_D)
{
#ifdef ZEND_WIN32
	if (ZCG(counted)) {
		DECREMENT(mem_usage);
		ZCG(counted) = 0;
	}
#else
	static const FLOCK_STRUCTURE(mem_usage_unlock, F_UNLCK, SEEK_SET, 1, 1);

	if (fcntl(lock_file, F_SETLK, &mem_usage_unlock) == -1) {
		zend_accel_error(ACCEL_LOG_DEBUG, "UpdateC(-1):  %s (%d)", strerror(errno), errno);
	}
#endif
}

static inline void accel_unlock_all(TSRMLS_D)
{
#ifdef ZEND_WIN32
	accel_deactivate_sub(TSRMLS_C);
#else
	static const FLOCK_STRUCTURE(mem_usage_unlock_all, F_UNLCK, SEEK_SET, 0, 0);

	if (fcntl(lock_file, F_SETLK, &mem_usage_unlock_all) == -1) {
		zend_accel_error(ACCEL_LOG_DEBUG, "UnlockAll:  %s (%d)", strerror(errno), errno);
	}
#endif
}

#ifndef ZEND_WIN32
static inline void kill_all_lockers(struct flock *mem_usage_check)
{
	int tries = 10;

	/* so that other process won't try to force while we are busy cleaning up */
	ZCSG(force_restart_time) = 0;
	while (mem_usage_check->l_pid > 0) {
		while (tries--) {
			zend_accel_error(ACCEL_LOG_INFO, "Killed locker %d", mem_usage_check->l_pid);
			if (kill(mem_usage_check->l_pid, SIGKILL)) {
				break;
			}
			/* give it a chance to die */
			usleep(20000);
			if (kill(mem_usage_check->l_pid, 0)) {
				/* can't kill it */
				break;
			}
			usleep(10000);
		}
		if (!tries) {
			zend_accel_error(ACCEL_LOG_INFO, "Can't kill %d after 20 tries!", mem_usage_check->l_pid);
			ZCSG(force_restart_time) = time(NULL); /* restore forced restart request */
		}

		mem_usage_check->l_type = F_WRLCK;
		mem_usage_check->l_whence = SEEK_SET;
		mem_usage_check->l_start = 1;
		mem_usage_check->l_len = 1;
		mem_usage_check->l_pid = -1;
		if (fcntl(lock_file, F_GETLK, mem_usage_check) == -1) {
			zend_accel_error(ACCEL_LOG_DEBUG, "KLockers:  %s (%d)", strerror(errno), errno);
			break;
		}

		if (mem_usage_check->l_type == F_UNLCK || mem_usage_check->l_pid <= 0) {
			break;
		}
	}
}
#endif

static inline int accel_is_inactive(TSRMLS_D)
{
#ifdef ZEND_WIN32
	if (LOCKVAL(mem_usage) == 0) {
		return SUCCESS;
	}
#else
	FLOCK_STRUCTURE(mem_usage_check, F_WRLCK, SEEK_SET, 1, 1);

	mem_usage_check.l_pid = -1;
	if (fcntl(lock_file, F_GETLK, &mem_usage_check) == -1) {
		zend_accel_error(ACCEL_LOG_DEBUG, "UpdateC:  %s (%d)", strerror(errno), errno);
		return FAILURE;
	}
	if (mem_usage_check.l_type == F_UNLCK) {
		return SUCCESS;
	}

	if (ZCG(accel_directives).force_restart_timeout
		&& ZCSG(force_restart_time)
		&& time(NULL) >= ZCSG(force_restart_time)) {
		zend_accel_error(ACCEL_LOG_WARNING, "Forced restart at %d (after %d seconds), locked by %d", time(NULL), ZCG(accel_directives).force_restart_timeout, mem_usage_check.l_pid);
		kill_all_lockers(&mem_usage_check);

		return FAILURE; /* next request should be able to restart it */
	}
#endif

	return FAILURE;
}

static int zend_get_stream_timestamp(const char *filename, struct stat *statbuf TSRMLS_DC)
{
	php_stream_wrapper *wrapper;
	php_stream_statbuf stream_statbuf;
	int ret, er;

	if (!filename) {
		return FAILURE;
	}

	wrapper = php_stream_locate_url_wrapper(filename, NULL, STREAM_LOCATE_WRAPPERS_ONLY TSRMLS_CC);
	if (!wrapper) {
		return FAILURE;
	}
	if (!wrapper->wops || !wrapper->wops->url_stat) {
		statbuf->st_mtime = 1;
		return SUCCESS; /* anything other than 0 is considered to be a valid timestamp */
	}

	er = EG(error_reporting);
	EG(error_reporting) = 0;
	zend_try {
		ret = wrapper->wops->url_stat(wrapper, (char*)filename, PHP_STREAM_URL_STAT_QUIET, &stream_statbuf, NULL TSRMLS_CC);
	} zend_catch {
		ret = -1;
	} zend_end_try();
	EG(error_reporting) = er;

	if (ret != 0) {
		return FAILURE;
	}

	*statbuf = stream_statbuf.sb;
	return SUCCESS;
}

#if ZEND_WIN32
static accel_time_t zend_get_file_handle_timestamp_win(zend_file_handle *file_handle, size_t *size)
{
	static unsigned __int64 utc_base = 0;
	static FILETIME utc_base_ft;
	WIN32_FILE_ATTRIBUTE_DATA fdata;

	if (!file_handle->opened_path) {
		return 0;
	}

	if (!utc_base) {
		SYSTEMTIME st;

		st.wYear = 1970;
		st.wMonth = 1;
		st.wDay = 1;
		st.wHour = 0;
		st.wMinute = 0;
		st.wSecond = 0;
		st.wMilliseconds = 0;

		SystemTimeToFileTime (&st, &utc_base_ft);
		utc_base = (((unsigned __int64)utc_base_ft.dwHighDateTime) << 32) + utc_base_ft.dwLowDateTime;
    }

	if (GetFileAttributesEx(file_handle->opened_path, GetFileExInfoStandard, &fdata) != 0) {
		unsigned __int64 ftime;

		if (CompareFileTime (&fdata.ftLastWriteTime, &utc_base_ft) < 0) {
			return 0;
		}

		ftime = (((unsigned __int64)fdata.ftLastWriteTime.dwHighDateTime) << 32) + fdata.ftLastWriteTime.dwLowDateTime - utc_base;
		ftime /= 10000000L;

		if (size) {
			*size = (size_t)(((unsigned __int64)fdata.nFileSizeHigh) << 32 + (unsigned __int64)fdata.nFileSizeLow);
		}
		return (accel_time_t)ftime;
	}
	return 0;
}
#endif

static accel_time_t zend_get_file_handle_timestamp(zend_file_handle *file_handle, size_t *size TSRMLS_DC)
{
	struct stat statbuf;
#ifdef ZEND_WIN32
	accel_time_t res;
#endif

	if (sapi_module.get_stat &&
	    !EG(opline_ptr) &&
	    file_handle->filename == SG(request_info).path_translated) {

		struct stat *tmpbuf = sapi_module.get_stat(TSRMLS_C);

		if (tmpbuf) {
			if (size) {
				*size = tmpbuf->st_size;
			}
			return tmpbuf->st_mtime;
		}
	}

#ifdef ZEND_WIN32
	res = zend_get_file_handle_timestamp_win(file_handle, size);
	if (res) {
		return res;
	}
#endif

	switch (file_handle->type) {
		case ZEND_HANDLE_FD:
			if (fstat(file_handle->handle.fd, &statbuf) == -1) {
				return 0;
			}
			break;
		case ZEND_HANDLE_FP:
			if (fstat(fileno(file_handle->handle.fp), &statbuf) == -1) {
				if (zend_get_stream_timestamp(file_handle->filename, &statbuf TSRMLS_CC) != SUCCESS) {
					return 0;
				}
			}
			break;
		case ZEND_HANDLE_FILENAME:
#if ZEND_EXTENSION_API_NO >= PHP_5_3_X_API_NO
		case ZEND_HANDLE_MAPPED:
#endif
			{
				char *file_path = file_handle->opened_path;

				if (file_path) {
					if (is_stream_path(file_path)) {
						if (zend_get_stream_timestamp(file_path, &statbuf TSRMLS_CC) == SUCCESS) {
							break;
						}
					}
					if (VCWD_STAT(file_path, &statbuf) != -1) {
						break;
					}
				}

				if (zend_get_stream_timestamp(file_handle->filename, &statbuf TSRMLS_CC) != SUCCESS) {
					return 0;
				}
				break;
			}
		case ZEND_HANDLE_STREAM:
			{
				php_stream *stream = (php_stream *)file_handle->handle.stream.handle;
				php_stream_statbuf sb;
				int ret, er;

				if (!stream ||
				    !stream->ops ||
				    !stream->ops->stat) {
					return 0;
				}

				er = EG(error_reporting);
				EG(error_reporting) = 0;
				zend_try {
					ret = stream->ops->stat(stream, &sb TSRMLS_CC);
				} zend_catch {
					ret = -1;
				} zend_end_try();
				EG(error_reporting) = er;
				if (ret != 0) {
					return 0;
				}

				statbuf = sb.sb;
			}
			break;

		default:
			return 0;
	}

	if (size) {
		*size = statbuf.st_size;
	}
	return statbuf.st_mtime;
}

static inline int do_validate_timestamps(zend_persistent_script *persistent_script, zend_file_handle *file_handle TSRMLS_DC)
{
	zend_file_handle ps_handle;
	char *full_path_ptr = NULL;

	/** check that the persistant script is indeed the same file we cached
	 * (if part of the path is a symlink than it possible that the user will change it)
	 * See bug #15140
	 */
	if (file_handle->opened_path) {
		if (strcmp(persistent_script->full_path, file_handle->opened_path) != 0) {
			return FAILURE;
		}
	} else {		
#if ZEND_EXTENSION_API_NO < PHP_5_3_X_API_NO
		full_path_ptr = accel_php_resolve_path(file_handle->filename, strlen(file_handle->filename), ZCG(include_path) TSRMLS_CC);
#else
		full_path_ptr = accelerator_orig_zend_resolve_path(file_handle->filename, strlen(file_handle->filename) TSRMLS_CC);
#endif
		if (full_path_ptr && strcmp(persistent_script->full_path, full_path_ptr) != 0) {
			efree(full_path_ptr);
			return FAILURE;
		}
		file_handle->opened_path = full_path_ptr;
	}

	if (persistent_script->timestamp == 0) {
		if (full_path_ptr) {
			efree(full_path_ptr);
			file_handle->opened_path = NULL;
		}
		return FAILURE;
	}

	if (zend_get_file_handle_timestamp(file_handle, NULL TSRMLS_CC) == persistent_script->timestamp) {
		if (full_path_ptr) {
			efree(full_path_ptr);
			file_handle->opened_path = NULL;
		}
		return SUCCESS;
	}
	if (full_path_ptr) {
		efree(full_path_ptr);
		file_handle->opened_path = NULL;
	}

	ps_handle.type = ZEND_HANDLE_FILENAME;
	ps_handle.filename = persistent_script->full_path;
	ps_handle.opened_path = persistent_script->full_path;

	if (zend_get_file_handle_timestamp(&ps_handle, NULL TSRMLS_CC) == persistent_script->timestamp) {
		return SUCCESS;
	}

	return FAILURE;
}

static inline int validate_timestamp_and_record(zend_persistent_script *persistent_script, zend_file_handle *file_handle TSRMLS_DC)
{
	if (ZCG(accel_directives).revalidate_freq &&
	    (persistent_script->dynamic_members.revalidate >= ZCSG(revalidate_at))) {
		return SUCCESS;
	} else if (do_validate_timestamps(persistent_script, file_handle TSRMLS_CC) == FAILURE) {
		return FAILURE;
	} else {
		persistent_script->dynamic_members.revalidate = ZCSG(revalidate_at);
		return SUCCESS;
	}
}

static unsigned int zend_accel_script_checksum(zend_persistent_script *persistent_script)
{
	signed char *mem = (signed char*)persistent_script->mem;
	size_t size = persistent_script->size;
	size_t persistent_script_check_block_size = ((char *)&(persistent_script->dynamic_members)) - (char *)persistent_script;
	unsigned int checksum = ADLER32_INIT;

	if (mem < (signed char*)persistent_script) {
		checksum = zend_adler32(checksum, mem, (signed char*)persistent_script - mem);
		size -= (signed char*)persistent_script - mem;
		mem  += (signed char*)persistent_script - mem;
	}

	zend_adler32(checksum, mem, persistent_script_check_block_size);
	mem  += sizeof(*persistent_script);
	size -= sizeof(*persistent_script);

	if (size > 0) {
		checksum = zend_adler32(checksum, mem, size);
	}
	return checksum;
}

/* Instead of resolving full real path name each time we need to identify file,
 * we create a key that consist from requested file name, current working
 * directory, current include_path, etc */
char *accel_make_persistent_key_ex(zend_file_handle *file_handle, int path_length, int *key_len TSRMLS_DC)
{
    int key_length;

    /* CWD and include_path don't matter for absolute file names and streams */
    if (ZCG(accel_directives).use_cwd &&
        !IS_ABSOLUTE_PATH(file_handle->filename, path_length) &&
        !is_stream_path(file_handle->filename)) {
        char *include_path = NULL;
        int include_path_len = 0;
        const char *parent_script = NULL;
        int parent_script_len = 0;
        int cur_len = 0;
        int cwd_len;
        char *cwd;

        if ((cwd = accel_getcwd(&cwd_len TSRMLS_CC)) == NULL) {
            /* we don't handle this well for now. */
            zend_accel_error(ACCEL_LOG_INFO, "getcwd() failed for '%s' (%d), please try to set opcache.use_cwd to 0 in ini file", file_handle->filename, errno);
            if (file_handle->opened_path) {
                cwd = file_handle->opened_path;
		        cwd_len = strlen(cwd);
            } else {
				ZCG(key_len) = 0;
                return NULL;
            }
        }

		if (ZCG(include_path_key)) {
			include_path = ZCG(include_path_key);
			include_path_len = 1;
		} else {
	        include_path = ZCG(include_path);
    	    include_path_len = ZCG(include_path_len);
			if (ZCG(include_path_check) &&
			    ZCG(enabled) && accel_startup_ok &&
			    (ZCG(counted) || ZCSG(accelerator_enabled)) &&
			    !zend_accel_hash_find(&ZCSG(include_paths), ZCG(include_path), ZCG(include_path_len) + 1) &&
			    !zend_accel_hash_is_full(&ZCSG(include_paths))) {

				SHM_UNPROTECT();
				zend_shared_alloc_lock(TSRMLS_C);

				ZCG(include_path_key) = zend_accel_hash_find(&ZCSG(include_paths), ZCG(include_path), ZCG(include_path_len) + 1);
				if (ZCG(include_path_key)) {
					include_path = ZCG(include_path_key);
					include_path_len = 1;
				} else if (!zend_accel_hash_is_full(&ZCSG(include_paths))) {
					char *key;

					key = zend_shared_alloc(ZCG(include_path_len) + 2);
					if (key) {
						memcpy(key, ZCG(include_path), ZCG(include_path_len) + 1);
						key[ZCG(include_path_len) + 1] = 'A' + ZCSG(include_paths).num_entries;
						ZCG(include_path_key) = key + ZCG(include_path_len) + 1;
						zend_accel_hash_update(&ZCSG(include_paths), key, ZCG(include_path_len) + 1, 0, ZCG(include_path_key));
						include_path = ZCG(include_path_key);
						include_path_len = 1;
					} else {
						zend_accel_schedule_restart_if_necessary(ACCEL_RESTART_OOM TSRMLS_CC);
					}
    	    	}

				zend_shared_alloc_unlock(TSRMLS_C);
				SHM_PROTECT();
			}
		}

        /* Here we add to the key the parent script directory,
           since fopen_wrappers from version 4.0.7 use current script's path
           in include path too.
        */
        if (EG(in_execution) &&
            (parent_script = zend_get_executed_filename(TSRMLS_C)) != NULL &&
	        parent_script[0] != '[') {

            parent_script_len = strlen(parent_script);
            while ((--parent_script_len > 0) && !IS_SLASH(parent_script[parent_script_len]));
        }

        /* Calculate key length */
        key_length = cwd_len + path_length + include_path_len + 2;
        if (parent_script_len) {
            key_length += parent_script_len + 1;
        }

        /* Generate key
         * Note - the include_path must be the last element in the key,
         * since in itself, it may include colons (which we use to separate
         * different components of the key)
         */
		if ((size_t)key_length >= sizeof(ZCG(key))) {
			ZCG(key_len) = 0;
			return NULL;
		}
		memcpy(ZCG(key), cwd, cwd_len);
		ZCG(key)[cwd_len] = ':';

		memcpy(ZCG(key) + cwd_len + 1, file_handle->filename, path_length);

		ZCG(key)[cwd_len + 1 + path_length] = ':';

        cur_len = cwd_len + 1 + path_length + 1;

        if (parent_script_len) {
			memcpy(ZCG(key) + cur_len, parent_script, parent_script_len);
            cur_len += parent_script_len;
			ZCG(key)[cur_len] = ':';
            cur_len++;
        }
		memcpy(ZCG(key) + cur_len, include_path, include_path_len);
		ZCG(key)[key_length] = '\0';
    } else {
        /* not use_cwd */
        key_length = path_length;
		if ((size_t)key_length >= sizeof(ZCG(key))) {
			ZCG(key_len) = 0;
			return NULL;
		}
		memcpy(ZCG(key), file_handle->filename, key_length + 1);
    }

	*key_len = ZCG(key_len) = key_length;
	return ZCG(key);
}

static inline char *accel_make_persistent_key(zend_file_handle *file_handle, int *key_len TSRMLS_DC)
{
	return accel_make_persistent_key_ex(file_handle, strlen(file_handle->filename), key_len TSRMLS_CC);
}

int zend_accel_invalidate(const char *filename, int filename_len, zend_bool force TSRMLS_DC)
{
	char *realpath;
	zend_persistent_script *persistent_script;

	if (!ZCG(enabled) || !accel_startup_ok || !ZCSG(accelerator_enabled) || accelerator_shm_read_lock(TSRMLS_C) != SUCCESS) {
		return FAILURE;
	}

#if ZEND_EXTENSION_API_NO < PHP_5_3_X_API_NO
	realpath = accel_php_resolve_path(filename, filename_len, ZCG(include_path) TSRMLS_CC);
#else
	realpath = accelerator_orig_zend_resolve_path(filename, filename_len TSRMLS_CC);
#endif

	if (!realpath) {
		return FAILURE;
	}

	persistent_script = zend_accel_hash_find(&ZCSG(hash), realpath, strlen(realpath) + 1);
	if (persistent_script && !persistent_script->corrupted) {
		zend_file_handle file_handle;

		file_handle.type = ZEND_HANDLE_FILENAME;
		file_handle.filename = realpath;
		file_handle.opened_path = realpath;

		if (force ||
			!ZCG(accel_directives).validate_timestamps ||
			do_validate_timestamps(persistent_script, &file_handle TSRMLS_CC) == FAILURE) {
			SHM_UNPROTECT();
			zend_shared_alloc_lock(TSRMLS_C);
			if (!persistent_script->corrupted) {
				persistent_script->corrupted = 1;
				persistent_script->timestamp = 0;
				ZSMMG(wasted_shared_memory) += persistent_script->dynamic_members.memory_consumption;
				if (ZSMMG(memory_exhausted)) {
					zend_accel_restart_reason reason =
						zend_accel_hash_is_full(&ZCSG(hash)) ? ACCEL_RESTART_HASH : ACCEL_RESTART_OOM;
					zend_accel_schedule_restart_if_necessary(reason TSRMLS_CC);
				}
			}
			zend_shared_alloc_unlock(TSRMLS_C);
			SHM_PROTECT();
		}
	}

	accelerator_shm_read_unlock(TSRMLS_C);
	efree(realpath);
	
	return SUCCESS;
}

/* Adds another key for existing cached script */
static void zend_accel_add_key(char *key, unsigned int key_length, zend_accel_hash_entry *bucket TSRMLS_DC)
{
	if (!zend_accel_hash_find(&ZCSG(hash), key, key_length + 1)) {
		if (zend_accel_hash_is_full(&ZCSG(hash))) {
			zend_accel_error(ACCEL_LOG_DEBUG, "No more entries in hash table!");
			ZSMMG(memory_exhausted) = 1;
			zend_accel_schedule_restart_if_necessary(ACCEL_RESTART_HASH TSRMLS_CC);
		} else {
			char *new_key = zend_shared_alloc(key_length + 1);
			if (new_key) {
				memcpy(new_key, key, key_length + 1);
				zend_accel_hash_update(&ZCSG(hash), new_key, key_length + 1, 1, bucket);
			} else {
				zend_accel_schedule_restart_if_necessary(ACCEL_RESTART_OOM TSRMLS_CC);
			}
		}
	}
}

static zend_persistent_script *cache_script_in_shared_memory(zend_persistent_script *new_persistent_script, char *key, unsigned int key_length, int *from_shared_memory TSRMLS_DC)
{
	zend_accel_hash_entry *bucket;
	uint memory_used;

	/* Check if script may be stored in shared memory */
	if (!zend_accel_script_persistable(new_persistent_script)) {
		return new_persistent_script;
	}

	if (!zend_accel_script_optimize(new_persistent_script TSRMLS_CC)) {
		return new_persistent_script;
	}

	if (!compact_persistent_script(new_persistent_script)) {
		return new_persistent_script;
	}

	/* exclusive lock */
	zend_shared_alloc_lock(TSRMLS_C);

	if (zend_accel_hash_is_full(&ZCSG(hash))) {
		zend_accel_error(ACCEL_LOG_DEBUG, "No more entries in hash table!");
		ZSMMG(memory_exhausted) = 1;
		zend_accel_schedule_restart_if_necessary(ACCEL_RESTART_HASH TSRMLS_CC);
		zend_shared_alloc_unlock(TSRMLS_C);
		return new_persistent_script;
	}

	/* Check if we still need to put the file into the cache (may be it was
	 * already stored by another process. This final check is done under
	 * exclusive lock) */
	bucket = zend_accel_hash_find_entry(&ZCSG(hash), new_persistent_script->full_path, new_persistent_script->full_path_len + 1);
	if (bucket) {
		zend_persistent_script *existing_persistent_script = (zend_persistent_script *)bucket->data;

		if (!existing_persistent_script->corrupted) {
			if (!ZCG(accel_directives).validate_timestamps ||
			    (new_persistent_script->timestamp == existing_persistent_script->timestamp)) {
				zend_accel_add_key(key, key_length, bucket TSRMLS_CC);
			}
			zend_shared_alloc_unlock(TSRMLS_C);
			return new_persistent_script;
		}
	}

	/* Calculate the required memory size */
	memory_used = zend_accel_script_persist_calc(new_persistent_script, key, key_length TSRMLS_CC);

	/* Allocate shared memory */
	ZCG(mem) = zend_shared_alloc(memory_used);
	if (!ZCG(mem)) {
		zend_accel_schedule_restart_if_necessary(ACCEL_RESTART_OOM TSRMLS_CC);
		zend_shared_alloc_unlock(TSRMLS_C);
		return new_persistent_script;
	}

	/* cleanup after calculation */
	new_persistent_script->mem  = ZCG(mem);
	new_persistent_script->size = memory_used;

	/* Copy into shared memory */
	new_persistent_script = zend_accel_script_persist(new_persistent_script, &key, key_length TSRMLS_CC);

	/* Consistency check */
	if ((char*)new_persistent_script->mem + new_persistent_script->size != (char*)ZCG(mem)) {
		zend_accel_error(
			((char*)new_persistent_script->mem + new_persistent_script->size < (char*)ZCG(mem)) ? ACCEL_LOG_ERROR : ACCEL_LOG_WARNING,
			"Internal error: wrong size calculation: %s start=0x%08x, end=0x%08x, real=0x%08x\n",
			new_persistent_script->full_path,
			new_persistent_script->mem,
			(char *)new_persistent_script->mem + new_persistent_script->size,
			ZCG(mem));
	}

	new_persistent_script->dynamic_members.checksum = zend_accel_script_checksum(new_persistent_script);

	/* store script structure in the hash table */
	bucket = zend_accel_hash_update(&ZCSG(hash), new_persistent_script->full_path, new_persistent_script->full_path_len + 1, 0, new_persistent_script);
	if (bucket &&
	    /* key may contain non-persistent PHAR aliases (see issues #115 and #149) */
	    memcmp(key, "phar://", sizeof("phar://") - 1) != 0 &&
	    (new_persistent_script->full_path_len != key_length ||
	     memcmp(new_persistent_script->full_path, key, key_length) != 0)) {
		/* link key to the same persistent script in hash table */
		if (!zend_accel_hash_update(&ZCSG(hash), key, key_length + 1, 1, bucket)) {
			zend_accel_error(ACCEL_LOG_DEBUG, "No more entries in hash table!");
			ZSMMG(memory_exhausted) = 1;
			zend_accel_schedule_restart_if_necessary(ACCEL_RESTART_HASH TSRMLS_CC);
		}
	}

	new_persistent_script->dynamic_members.memory_consumption = ZEND_ALIGNED_SIZE(new_persistent_script->size);

	zend_shared_alloc_unlock(TSRMLS_C);

	*from_shared_memory = 1;
	return new_persistent_script;
}

static const struct jit_auto_global_info
{
    const char *name;
    size_t len;
} jit_auto_globals_info[] = {
    { "_SERVER",  sizeof("_SERVER")},
    { "_ENV",     sizeof("_ENV")},
    { "_REQUEST", sizeof("_REQUEST")},
#if ZEND_EXTENSION_API_NO > PHP_5_3_X_API_NO
    { "GLOBALS",  sizeof("GLOBALS")},
#endif
};

static int zend_accel_get_auto_globals(TSRMLS_D)
{
	int i, ag_size = (sizeof(jit_auto_globals_info) / sizeof(jit_auto_globals_info[0]));
	int n = 1;
	int mask = 0;

	for (i = 0; i < ag_size ; i++) {
		if (zend_hash_exists(&EG(symbol_table), jit_auto_globals_info[i].name, jit_auto_globals_info[i].len)) {
			mask |= n;
		}
		n += n;
	}
	return mask;
}

#if ZEND_EXTENSION_API_NO > PHP_5_3_X_API_NO
static int zend_accel_get_auto_globals_no_jit(TSRMLS_D)
{
	if (zend_hash_exists(&EG(symbol_table), jit_auto_globals_info[3].name, jit_auto_globals_info[3].len)) {
		return 8;
	}
	return 0;
}
#endif

static void zend_accel_set_auto_globals(int mask TSRMLS_DC)
{
	int i, ag_size = (sizeof(jit_auto_globals_info) / sizeof(jit_auto_globals_info[0]));
	int n = 1;

	for (i = 0; i < ag_size ; i++) {
		if (mask & n) {
			zend_is_auto_global(jit_auto_globals_info[i].name, jit_auto_globals_info[i].len - 1 TSRMLS_CC);
		}
		n += n;
	}
}

static zend_persistent_script *compile_and_cache_file(zend_file_handle *file_handle, int type, char *key, unsigned int key_length, zend_op_array **op_array_p, int *from_shared_memory TSRMLS_DC)
{
	zend_persistent_script *new_persistent_script;
	zend_op_array *orig_active_op_array;
	HashTable *orig_function_table, *orig_class_table;
	zval *orig_user_error_handler;
	zend_op_array *op_array;
	int do_bailout = 0;
	accel_time_t timestamp = 0;
#if ZEND_EXTENSION_API_NO >= PHP_5_3_X_API_NO
	zend_uint orig_compiler_options = 0;
#endif

    /* Try to open file */
    if (file_handle->type == ZEND_HANDLE_FILENAME) {
        if (accelerator_orig_zend_stream_open_function(file_handle->filename, file_handle TSRMLS_CC) == SUCCESS) {
        	/* key may be changed by zend_stream_open_function() */
        	if (key == ZCG(key)) {
        		key_length = ZCG(key_len);
        	}
        } else {
			*op_array_p = NULL;
			if (type == ZEND_REQUIRE) {
#if ZEND_EXTENSION_API_NO < PHP_5_3_X_API_NO
				zend_message_dispatcher(ZMSG_FAILED_REQUIRE_FOPEN, file_handle->filename);
#else
				zend_message_dispatcher(ZMSG_FAILED_REQUIRE_FOPEN, file_handle->filename TSRMLS_CC);
#endif
				zend_bailout();
			} else {
#if ZEND_EXTENSION_API_NO < PHP_5_3_X_API_NO
				zend_message_dispatcher(ZMSG_FAILED_INCLUDE_FOPEN, file_handle->filename);
#else
				zend_message_dispatcher(ZMSG_FAILED_INCLUDE_FOPEN, file_handle->filename TSRMLS_CC);
#endif
			}
			return NULL;
    	}
    }

	/* check blacklist right after ensuring that file was opened */
	if (file_handle->opened_path && zend_accel_blacklist_is_blacklisted(&accel_blacklist, file_handle->opened_path)) {
		ZCSG(blacklist_misses)++;
		*op_array_p = accelerator_orig_compile_file(file_handle, type TSRMLS_CC);
		return NULL;
	}

#if ZEND_EXTENSION_API_NO >= PHP_5_3_X_API_NO
	if (file_handle->type == ZEND_HANDLE_STREAM &&
	    (!strstr(file_handle->filename, ".phar") ||
	     strstr(file_handle->filename, "://"))) {
		char *buf;
		size_t size;

		/* Stream callbacks needs to be called in context of original
		 * function and class tables (see: https://bugs.php.net/bug.php?id=64353)
		 */
		if (zend_stream_fixup(file_handle, &buf, &size TSRMLS_CC) == FAILURE) {
			*op_array_p = NULL;
			return NULL;
		}
	}
#endif

	if (ZCG(accel_directives).validate_timestamps ||
	    ZCG(accel_directives).file_update_protection ||
	    ZCG(accel_directives).max_file_size > 0) {
		size_t size = 0;

		/* Obtain the file timestamps, *before* actually compiling them,
		 * otherwise we have a race-condition.
		 */
		timestamp = zend_get_file_handle_timestamp(file_handle, ZCG(accel_directives).max_file_size > 0 ? &size : NULL TSRMLS_CC);

		/* If we can't obtain a timestamp (that means file is possibly socket)
		 *  we won't cache it
		 */
		if (timestamp == 0) {
			*op_array_p = accelerator_orig_compile_file(file_handle, type TSRMLS_CC);
			return NULL;
		}

		/* check if file is too new (may be it's not written completely yet) */
		if (ZCG(accel_directives).file_update_protection &&
		    (ZCG(request_time) - ZCG(accel_directives).file_update_protection < timestamp)) {
			*op_array_p = accelerator_orig_compile_file(file_handle, type TSRMLS_CC);
			return NULL;
		}

		if (ZCG(accel_directives).max_file_size > 0 && size > (size_t)ZCG(accel_directives).max_file_size) {
			ZCSG(blacklist_misses)++;
			*op_array_p = accelerator_orig_compile_file(file_handle, type TSRMLS_CC);
			return NULL;
		}
	}

	new_persistent_script = create_persistent_script();

	/* Save the original values for the op_array, function table and class table */
	orig_active_op_array = CG(active_op_array);
	orig_function_table = CG(function_table);
	orig_class_table = CG(class_table);
	orig_user_error_handler = EG(user_error_handler);

	/* Override them with ours */
	CG(function_table) = &ZCG(function_table);
	EG(class_table) = CG(class_table) = &new_persistent_script->class_table;
	EG(user_error_handler) = NULL;

	zend_try {
#if ZEND_EXTENSION_API_NO >= PHP_5_3_X_API_NO
		orig_compiler_options = CG(compiler_options);
		CG(compiler_options) |= ZEND_COMPILE_HANDLE_OP_ARRAY;
		CG(compiler_options) |= ZEND_COMPILE_IGNORE_INTERNAL_CLASSES;
		CG(compiler_options) |= ZEND_COMPILE_DELAYED_BINDING;
		CG(compiler_options) |= ZEND_COMPILE_NO_CONSTANT_SUBSTITUTION;
#endif
		op_array = *op_array_p = accelerator_orig_compile_file(file_handle, type TSRMLS_CC);
#if ZEND_EXTENSION_API_NO >= PHP_5_3_X_API_NO
		CG(compiler_options) = orig_compiler_options;
#endif
	} zend_catch {
		op_array = NULL;
		do_bailout = 1;
#if ZEND_EXTENSION_API_NO >= PHP_5_3_X_API_NO
		CG(compiler_options) = orig_compiler_options;
#endif
	} zend_end_try();

	/* Restore originals */
	CG(active_op_array) = orig_active_op_array;
	CG(function_table) = orig_function_table;
	EG(class_table) = CG(class_table) = orig_class_table;
	EG(user_error_handler) = orig_user_error_handler;

	if (!op_array) {
		/* compilation failed */
		free_persistent_script(new_persistent_script, 1);
		zend_accel_free_user_functions(&ZCG(function_table) TSRMLS_CC);
		if (do_bailout) {
			zend_bailout();
		}
		return NULL;
	}

	/* Build the persistent_script structure.
	   Here we aren't sure we would store it, but we will need it
	   further anyway.
	*/
	zend_accel_move_user_functions(&ZCG(function_table), &new_persistent_script->function_table TSRMLS_CC);
	new_persistent_script->main_op_array = *op_array;

	efree(op_array); /* we have valid persistent_script, so it's safe to free op_array */

    /* Fill in the ping_auto_globals_mask for the new script. If jit for auto globals is enabled we
       will have to ping the used auto global variables before execution */
#if ZEND_EXTENSION_API_NO > PHP_5_3_X_API_NO
	if (PG(auto_globals_jit)) {
		new_persistent_script->ping_auto_globals_mask = zend_accel_get_auto_globals(TSRMLS_C);
	} else {
		new_persistent_script->ping_auto_globals_mask = zend_accel_get_auto_globals_no_jit(TSRMLS_C);
	}
#else
	if ((PG(auto_globals_jit) && !PG(register_globals) && !PG(register_long_arrays))) {
		new_persistent_script->ping_auto_globals_mask = zend_accel_get_auto_globals(TSRMLS_C);
	}
#endif

	if (ZCG(accel_directives).validate_timestamps) {
		/* Obtain the file timestamps, *before* actually compiling them,
		 * otherwise we have a race-condition.
		 */
		new_persistent_script->timestamp = timestamp;
		new_persistent_script->dynamic_members.revalidate = ZCSG(revalidate_at);
	}

	if (file_handle->opened_path) {
		new_persistent_script->full_path_len = strlen(file_handle->opened_path);
		new_persistent_script->full_path = estrndup(file_handle->opened_path, new_persistent_script->full_path_len);
	} else {
		new_persistent_script->full_path_len = strlen(file_handle->filename);
		new_persistent_script->full_path = estrndup(file_handle->filename, new_persistent_script->full_path_len);
	}
	new_persistent_script->hash_value = zend_hash_func(new_persistent_script->full_path, new_persistent_script->full_path_len + 1);

	/* Now persistent_script structure is ready in process memory */
	return cache_script_in_shared_memory(new_persistent_script, key, key_length, from_shared_memory TSRMLS_CC);
}

/* zend_compile() replacement */
zend_op_array *persistent_compile_file(zend_file_handle *file_handle, int type TSRMLS_DC)
{
	zend_persistent_script *persistent_script = NULL;
	char *key = NULL;
	int key_length;
	int from_shared_memory; /* if the script we've got is stored in SHM */

	if (!file_handle->filename ||
		!ZCG(enabled) || !accel_startup_ok ||
		(!ZCG(counted) && !ZCSG(accelerator_enabled)) ||
	    CG(interactive) ||
	    (ZCSG(restart_in_progress) && accel_restart_is_active(TSRMLS_C)) ||
	    (is_stream_path(file_handle->filename) && 
	     !is_cacheable_stream_path(file_handle->filename))) {
		/* The Accelerator is disabled, act as if without the Accelerator */
		return accelerator_orig_compile_file(file_handle, type TSRMLS_CC);
	}

	/* Make sure we only increase the currently running processes semaphore
     * once each execution (this function can be called more than once on
     * each execution)
     */
	if (!ZCG(counted)) {
		ZCG(counted) = 1;
		accel_activate_add(TSRMLS_C);
	}

	/* In case this callback is called from include_once, require_once or it's
	 * a main FastCGI request, the key must be already calculated, and cached
	 * persistent script already found */
	if ((EG(opline_ptr) == NULL &&
	     ZCG(cache_opline) == NULL &&
	     file_handle->filename == SG(request_info).path_translated &&
	     ZCG(cache_persistent_script)) ||
	    (EG(opline_ptr) && *EG(opline_ptr) &&
	     *EG(opline_ptr) == ZCG(cache_opline) &&
	     (*EG(opline_ptr))->opcode == ZEND_INCLUDE_OR_EVAL &&
#if ZEND_EXTENSION_API_NO > PHP_5_3_X_API_NO
	     ((*EG(opline_ptr))->extended_value == ZEND_INCLUDE_ONCE ||
	      (*EG(opline_ptr))->extended_value == ZEND_REQUIRE_ONCE))) {
#else
 	     ((*EG(opline_ptr))->op2.u.constant.value.lval == ZEND_INCLUDE_ONCE ||
 	      (*EG(opline_ptr))->op2.u.constant.value.lval == ZEND_REQUIRE_ONCE))) {
#endif
		if (!ZCG(key_len)) {
			return accelerator_orig_compile_file(file_handle, type TSRMLS_CC);
		}
		/* persistent script was already found by overridden open() or
		 * resolve_path() callbacks */
		persistent_script = ZCG(cache_persistent_script);
		key = ZCG(key);
		key_length = ZCG(key_len);
	} else {
		/* try to find cached script by key */
		if ((key = accel_make_persistent_key(file_handle, &key_length TSRMLS_CC)) == NULL) {
			return accelerator_orig_compile_file(file_handle, type TSRMLS_CC);
		}
		persistent_script = zend_accel_hash_find(&ZCSG(hash), key, key_length + 1);
		if (!persistent_script) {
			/* try to find cached script by full real path */
			zend_accel_hash_entry *bucket;

			/* open file to resolve the path */
		    if (file_handle->type == ZEND_HANDLE_FILENAME &&
#if ZEND_EXTENSION_API_NO >= PHP_5_3_X_API_NO
        		accelerator_orig_zend_stream_open_function(file_handle->filename, file_handle TSRMLS_CC) == FAILURE) {
#else
		        zend_stream_open(file_handle->filename, file_handle TSRMLS_CC) == FAILURE) {
#endif
				if (type == ZEND_REQUIRE) {
#if ZEND_EXTENSION_API_NO < PHP_5_3_X_API_NO
					zend_message_dispatcher(ZMSG_FAILED_REQUIRE_FOPEN, file_handle->filename);
#else
					zend_message_dispatcher(ZMSG_FAILED_REQUIRE_FOPEN, file_handle->filename TSRMLS_CC);
#endif
					zend_bailout();
				} else {
#if ZEND_EXTENSION_API_NO < PHP_5_3_X_API_NO
					zend_message_dispatcher(ZMSG_FAILED_INCLUDE_FOPEN, file_handle->filename);
#else
					zend_message_dispatcher(ZMSG_FAILED_INCLUDE_FOPEN, file_handle->filename TSRMLS_CC);
#endif
				}
				return NULL;
		    }

			if (file_handle->opened_path &&
			    (bucket = zend_accel_hash_find_entry(&ZCSG(hash), file_handle->opened_path, strlen(file_handle->opened_path) + 1)) != NULL) {

				persistent_script = (zend_persistent_script *)bucket->data;
				if (!ZCG(accel_directives).revalidate_path &&
				    !persistent_script->corrupted) {
			    	SHM_UNPROTECT();
					zend_shared_alloc_lock(TSRMLS_C);
					zend_accel_add_key(key, key_length, bucket TSRMLS_CC);
					zend_shared_alloc_unlock(TSRMLS_C);
			    	SHM_PROTECT();
				}
			}
		}
	}

	/* clear cache */
	ZCG(cache_opline) = NULL;
	ZCG(cache_persistent_script) = NULL;

	if (persistent_script && persistent_script->corrupted) {
		persistent_script = NULL;
	}

	SHM_UNPROTECT();

	/* If script is found then validate_timestamps if option is enabled */
	if (persistent_script && ZCG(accel_directives).validate_timestamps) {
		if (validate_timestamp_and_record(persistent_script, file_handle TSRMLS_CC) == FAILURE) {
			zend_shared_alloc_lock(TSRMLS_C);
			if (!persistent_script->corrupted) {
				persistent_script->corrupted = 1;
				persistent_script->timestamp = 0;
				ZSMMG(wasted_shared_memory) += persistent_script->dynamic_members.memory_consumption;
				if (ZSMMG(memory_exhausted)) {
					zend_accel_restart_reason reason =
						zend_accel_hash_is_full(&ZCSG(hash)) ? ACCEL_RESTART_HASH : ACCEL_RESTART_OOM;
					zend_accel_schedule_restart_if_necessary(reason TSRMLS_CC);
				}
			}
			zend_shared_alloc_unlock(TSRMLS_C);
			persistent_script = NULL;
		}
	}

	/* if turned on - check the compiled script ADLER32 checksum */
	if (persistent_script && ZCG(accel_directives).consistency_checks
		&& persistent_script->dynamic_members.hits % ZCG(accel_directives).consistency_checks == 0) {

		unsigned int checksum = zend_accel_script_checksum(persistent_script);
		if (checksum != persistent_script->dynamic_members.checksum ) {
			/* The checksum is wrong */
			zend_accel_error(ACCEL_LOG_INFO, "Checksum failed for '%s':  expected=0x%0.8X, found=0x%0.8X",
							 persistent_script->full_path, persistent_script->dynamic_members.checksum, checksum);
			zend_shared_alloc_lock(TSRMLS_C);
			if (!persistent_script->corrupted) {
				persistent_script->corrupted = 1;
				persistent_script->timestamp = 0;
				ZSMMG(wasted_shared_memory) += persistent_script->dynamic_members.memory_consumption;
				if (ZSMMG(memory_exhausted)) {
					zend_accel_restart_reason reason =
						zend_accel_hash_is_full(&ZCSG(hash)) ? ACCEL_RESTART_HASH : ACCEL_RESTART_OOM;
					zend_accel_schedule_restart_if_necessary(reason TSRMLS_CC);
				}
			}
			zend_shared_alloc_unlock(TSRMLS_C);
			persistent_script = NULL;
		}
	}

	/* If script was not found or invalidated by validate_timestamps */
	if (!persistent_script) {
		zend_op_array *op_array;

		/* Cache miss.. */
		ZCSG(misses)++;

		/* No memory left. Behave like without the Accelerator */
		if (ZSMMG(memory_exhausted) || ZCSG(restart_pending)) {
			SHM_PROTECT();
			return accelerator_orig_compile_file(file_handle, type TSRMLS_CC);
		}

		/* Try and cache the script and assume that it is returned from_shared_memory.
         * If it isn't compile_and_cache_file() changes the flag to 0
         */
       	from_shared_memory = 0;
		persistent_script = compile_and_cache_file(file_handle, type, key, key_length, &op_array, &from_shared_memory TSRMLS_CC);

		/* Caching is disabled, returning op_array;
		 * or something went wrong during compilation, returning NULL
		 */
		if (!persistent_script) {
			SHM_PROTECT();
			return op_array;
		}
	} else {

#if !ZEND_WIN32
		ZCSG(hits)++; /* TBFixed: may lose one hit */
		persistent_script->dynamic_members.hits++; /* see above */
#else
		InterlockedIncrement(&ZCSG(hits));
		InterlockedIncrement(&persistent_script->dynamic_members.hits);
#endif

		/* see bug #15471 (old BTS) */
		if (persistent_script->full_path) {
			if (!EG(opline_ptr) || !*EG(opline_ptr) ||
			    (*EG(opline_ptr))->opcode != ZEND_INCLUDE_OR_EVAL ||
#if ZEND_EXTENSION_API_NO > PHP_5_3_X_API_NO
			    ((*EG(opline_ptr))->extended_value != ZEND_INCLUDE_ONCE &&
			     (*EG(opline_ptr))->extended_value != ZEND_REQUIRE_ONCE)) {
#else
 			    ((*EG(opline_ptr))->op2.u.constant.value.lval != ZEND_INCLUDE_ONCE &&
 			     (*EG(opline_ptr))->op2.u.constant.value.lval != ZEND_REQUIRE_ONCE)) {
#endif
				void *dummy = (void *) 1;

				if (zend_hash_quick_add(&EG(included_files), persistent_script->full_path, persistent_script->full_path_len + 1, persistent_script->hash_value, &dummy, sizeof(void *), NULL) == SUCCESS) {
					/* ext/phar has to load phar's metadata into memory */
					if (strstr(persistent_script->full_path, ".phar") && !strstr(persistent_script->full_path, "://")) {
						php_stream_statbuf ssb;
						char *fname = emalloc(sizeof("phar://") + persistent_script->full_path_len);

						memcpy(fname, "phar://", sizeof("phar://") - 1);
						memcpy(fname + sizeof("phar://") - 1, persistent_script->full_path, persistent_script->full_path_len + 1);
						php_stream_stat_path(fname, &ssb);
						efree(fname);
					}
				}
			}
		}
#if ZEND_EXTENSION_API_NO < PHP_5_3_X_API_NO
		zend_file_handle_dtor(file_handle);
#else
		zend_file_handle_dtor(file_handle TSRMLS_CC);
#endif
		from_shared_memory = 1;
	}

	persistent_script->dynamic_members.last_used = ZCG(request_time);

	SHM_PROTECT();

    /* Fetch jit auto globals used in the script before execution */
    if (persistent_script->ping_auto_globals_mask) {
		zend_accel_set_auto_globals(persistent_script->ping_auto_globals_mask TSRMLS_CC);
	}

	return zend_accel_load_script(persistent_script, from_shared_memory TSRMLS_CC);
}

#if ZEND_EXTENSION_API_NO < PHP_5_3_X_API_NO

/* Taken from php-5.2.5 because early versions don't have correct version */
static char *accel_tsrm_realpath(const char *path, int path_len TSRMLS_DC)
{
	cwd_state new_state;
	char *real_path;
	char *cwd;
	int cwd_len;

	/* realpath("") returns CWD */
	if (!*path) {
		new_state.cwd = (char*)malloc(1);
		if (!new_state.cwd) {
			zend_accel_error(ACCEL_LOG_ERROR, "malloc() failed");
			return NULL;
		}
		new_state.cwd[0] = '\0';
		new_state.cwd_length = 0;
	    if ((cwd = accel_getcwd(&cwd_len TSRMLS_CC)) != NULL) {
		    path = cwd;
		}
	} else if (!IS_ABSOLUTE_PATH(path, path_len) &&
	    (cwd = accel_getcwd(&cwd_len TSRMLS_CC)) != NULL) {
		new_state.cwd = zend_strndup(cwd, cwd_len);
		if (!new_state.cwd) {
			zend_accel_error(ACCEL_LOG_ERROR, "malloc() failed");
			return NULL;
		}
		new_state.cwd_length = cwd_len;
	} else {
		new_state.cwd = (char*)malloc(1);
		if (!new_state.cwd) {
			zend_accel_error(ACCEL_LOG_ERROR, "malloc() failed");
			return NULL;
		}
		new_state.cwd[0] = '\0';
		new_state.cwd_length = 0;
	}

#ifndef CWD_REALPATH
# define CWD_REALPATH 2
#endif
	if (virtual_file_ex(&new_state, path, NULL, CWD_REALPATH)) {
		free(new_state.cwd);
		return NULL;
	}

	real_path = emalloc(new_state.cwd_length + 1);
	memcpy(real_path, new_state.cwd, new_state.cwd_length + 1);
	free(new_state.cwd);
	return real_path;
}

static char *accel_php_resolve_path(const char *filename, int filename_length, const char *path TSRMLS_DC)
{
	char *resolved_path;
	char trypath[MAXPATHLEN];
	const char *ptr, *end;
	int len;

	if (!filename) {
		return NULL;
	}

	if (*filename == '.' ||
	    IS_ABSOLUTE_PATH(filename, filename_length) ||
	    !path ||
	    !*path) {
		return accel_tsrm_realpath(filename, filename_length TSRMLS_CC);
	}

	ptr = path;
	while (*ptr) {
		for (end = ptr; *end && *end != DEFAULT_DIR_SEPARATOR; end++);
		len = end - ptr;
		if (*end) end++;
		if (len + 1 + filename_length + 1 >= MAXPATHLEN) {
			ptr = end;
			continue;
		}
		memcpy(trypath, ptr, len);
		trypath[len] = '/';
		memcpy(trypath + len + 1, filename, filename_length + 1);
		ptr = end;
		if ((resolved_path = accel_tsrm_realpath(trypath, len + 1 + filename_length TSRMLS_CC)) != NULL) {
			return resolved_path;
		}
	} /* end provided path */

	/* check in calling scripts' current working directory as a fall back case
	 */
	if (EG(in_execution)) {
		char *exec_fname = zend_get_executed_filename(TSRMLS_C);
		int exec_fname_length = strlen(exec_fname);

		while ((--exec_fname_length >= 0) && !IS_SLASH(exec_fname[exec_fname_length]));
		if (exec_fname && exec_fname[0] != '[' &&
		    exec_fname_length > 0 &&
		    exec_fname_length + 1 + filename_length + 1 < MAXPATHLEN) {
			memcpy(trypath, exec_fname, exec_fname_length + 1);
			memcpy(trypath + exec_fname_length + 1, filename, filename_length + 1);
			return accel_tsrm_realpath(trypath, exec_fname_length + 1 + filename_length TSRMLS_CC);
		}
	}

	return NULL;
}

/* zend_stream_open_function() replacement for PHP 5.2 */
static int persistent_stream_open_function(const char *filename, zend_file_handle *handle TSRMLS_DC)
{
	if (ZCG(enabled) && accel_startup_ok &&
	    (ZCG(counted) || ZCSG(accelerator_enabled)) &&
	    !CG(interactive) &&
	    !ZCSG(restart_in_progress)) {

		if (EG(opline_ptr) && *EG(opline_ptr)) {
			zend_op *opline = *EG(opline_ptr);

            if (opline->opcode == ZEND_INCLUDE_OR_EVAL &&
                (opline->op2.u.constant.value.lval == ZEND_INCLUDE_ONCE ||
                 opline->op2.u.constant.value.lval == ZEND_REQUIRE_ONCE)) {
				/* we are in include_once */
				char *key = NULL;
				int key_length;
				char *resolved_path;
				zend_accel_hash_entry *bucket;
				zend_persistent_script *persistent_script;
				int filename_len;

				if (opline->op1.op_type == IS_CONST) {
					filename_len = Z_STRLEN(opline->op1.u.constant);
				} else {
					filename_len = strlen(filename);
				}
				handle->filename = (char*)filename;
				handle->free_filename = 0;
				handle->opened_path = NULL;

			    /* Check if requested file already cached (by full name) */
			    if (IS_ABSOLUTE_PATH(filename, filename_len) &&
				    (persistent_script = zend_accel_hash_find(&ZCSG(hash), (char*)filename, filename_len + 1)) != NULL &&
				    !persistent_script->corrupted) {

					handle->opened_path = estrndup(persistent_script->full_path, persistent_script->full_path_len);
					handle->type = ZEND_HANDLE_FILENAME;
					memcpy(ZCG(key), persistent_script->full_path, persistent_script->full_path_len + 1);
					ZCG(key_len) = persistent_script->full_path_len;
					ZCG(cache_opline) = opline;
					ZCG(cache_persistent_script) = persistent_script;
					return SUCCESS;
			    }

			    /* Check if requested file already cached (by key) */
				key = accel_make_persistent_key_ex(handle, filename_len, &key_length TSRMLS_CC);
				if (!ZCG(accel_directives).revalidate_path &&
				    key &&
					(persistent_script = zend_accel_hash_find(&ZCSG(hash), key, key_length + 1)) != NULL &&
				    !persistent_script->corrupted) {

					handle->opened_path = estrndup(persistent_script->full_path, persistent_script->full_path_len);
					handle->type = ZEND_HANDLE_FILENAME;
					ZCG(cache_opline) = opline;
					ZCG(cache_persistent_script) = persistent_script;
					return SUCCESS;
				}

				/* find the full real path */
				resolved_path = accel_php_resolve_path(filename, filename_len, ZCG(include_path) TSRMLS_CC);

			    /* Check if requested file already cached (by real name) */
				if (resolved_path &&
				    (bucket = zend_accel_hash_find_entry(&ZCSG(hash), resolved_path, strlen(resolved_path) + 1)) != NULL) {

					persistent_script = (zend_persistent_script *)bucket->data;
					if (persistent_script && !persistent_script->corrupted) {
						handle->opened_path = resolved_path;
						handle->type = ZEND_HANDLE_FILENAME;
						if (key && !ZCG(accel_directives).revalidate_path) {
							/* add another "key" for the same bucket */
					    	SHM_UNPROTECT();
							zend_shared_alloc_lock(TSRMLS_C);
							zend_accel_add_key(key, key_length, bucket TSRMLS_CC);
							zend_shared_alloc_unlock(TSRMLS_C);
					    	SHM_PROTECT();
						}
						ZCG(cache_opline) = opline;
						ZCG(cache_persistent_script) = persistent_script;
						return SUCCESS;
					}
				}
				if (resolved_path) {
					efree(resolved_path);
				}
			}
		}
	}
	ZCG(cache_opline) = NULL;
	ZCG(cache_persistent_script) = NULL;
	return accelerator_orig_zend_stream_open_function(filename, handle TSRMLS_CC);
}

#else

/* zend_stream_open_function() replacement for PHP 5.3 and above */
static int persistent_stream_open_function(const char *filename, zend_file_handle *handle TSRMLS_DC)
{
	if (ZCG(enabled) && accel_startup_ok &&
	    (ZCG(counted) || ZCSG(accelerator_enabled)) &&
	    !CG(interactive) &&
	    !ZCSG(restart_in_progress)) {

		/* check if callback is called from include_once or it's a main request */
		if ((!EG(opline_ptr) &&
		     filename == SG(request_info).path_translated) ||
		    (EG(opline_ptr) &&
		     *EG(opline_ptr) &&
             (*EG(opline_ptr))->opcode == ZEND_INCLUDE_OR_EVAL &&
#if ZEND_EXTENSION_API_NO > PHP_5_3_X_API_NO
             ((*EG(opline_ptr))->extended_value == ZEND_INCLUDE_ONCE ||
              (*EG(opline_ptr))->extended_value == ZEND_REQUIRE_ONCE))) {
#else
              ((*EG(opline_ptr))->op2.u.constant.value.lval == ZEND_INCLUDE_ONCE ||
               (*EG(opline_ptr))->op2.u.constant.value.lval == ZEND_REQUIRE_ONCE))) {
#endif

			/* we are in include_once or FastCGI request */
			zend_persistent_script *persistent_script;

			handle->filename = (char*)filename;
			handle->free_filename = 0;

			/* check if cached script was already found by resolve_path() */
			if ((EG(opline_ptr) == NULL &&
			     ZCG(cache_opline) == NULL &&
			     ZCG(cache_persistent_script) != NULL) ||
			    (EG(opline_ptr) &&
			     (ZCG(cache_opline) == *EG(opline_ptr)))) {
				persistent_script = ZCG(cache_persistent_script);
				handle->opened_path = estrndup(persistent_script->full_path, persistent_script->full_path_len);
				handle->type = ZEND_HANDLE_FILENAME;
				return SUCCESS;
#if 0
			} else {
				/* FIXME: It looks like this part is not needed any more */
				int filename_len = strlen(filename);

			    if ((IS_ABSOLUTE_PATH(filename, filename_len) ||
			         is_stream_path(filename)) &&
				    (persistent_script = zend_accel_hash_find(&ZCSG(hash), (char*)filename, filename_len + 1)) != NULL &&
				    !persistent_script->corrupted) {

					handle->opened_path = estrndup(persistent_script->full_path, persistent_script->full_path_len);
					handle->type = ZEND_HANDLE_FILENAME;
					memcpy(ZCG(key), persistent_script->full_path, persistent_script->full_path_len + 1);
					ZCG(key_len) = persistent_script->full_path_len;
					ZCG(cache_opline) = EG(opline_ptr) ? *EG(opline_ptr) : NULL;
					ZCG(cache_persistent_script) = EG(opline_ptr) ? persistent_script : NULL;
					return SUCCESS;
			    }
#endif
			}
		}
	}
	ZCG(cache_opline) = NULL;
	ZCG(cache_persistent_script) = NULL;
	return accelerator_orig_zend_stream_open_function(filename, handle TSRMLS_CC);
}

/* zend_resolve_path() replacement for PHP 5.3 and above */
static char* persistent_zend_resolve_path(const char *filename, int filename_len TSRMLS_DC)
{
	if (ZCG(enabled) && accel_startup_ok &&
	    (ZCG(counted) || ZCSG(accelerator_enabled)) &&
	    !CG(interactive) &&
	    !ZCSG(restart_in_progress)) {

		/* check if callback is called from include_once or it's a main request */
		if ((!EG(opline_ptr) &&
		     filename == SG(request_info).path_translated) ||
		    (EG(opline_ptr) &&
		     *EG(opline_ptr) &&
             (*EG(opline_ptr))->opcode == ZEND_INCLUDE_OR_EVAL &&
#if ZEND_EXTENSION_API_NO > PHP_5_3_X_API_NO
             ((*EG(opline_ptr))->extended_value == ZEND_INCLUDE_ONCE ||
              (*EG(opline_ptr))->extended_value == ZEND_REQUIRE_ONCE))) {
#else
              ((*EG(opline_ptr))->op2.u.constant.value.lval == ZEND_INCLUDE_ONCE ||
               (*EG(opline_ptr))->op2.u.constant.value.lval == ZEND_REQUIRE_ONCE))) {
#endif

			/* we are in include_once or FastCGI request */
			zend_file_handle handle;
			char *key = NULL;
			int key_length;
			char *resolved_path;
			zend_accel_hash_entry *bucket;
			zend_persistent_script *persistent_script;

		    /* Check if requested file already cached (by full name) */
		    if ((IS_ABSOLUTE_PATH(filename, filename_len) ||
		         is_stream_path(filename)) &&
			    (bucket = zend_accel_hash_find_entry(&ZCSG(hash), (char*)filename, filename_len + 1)) != NULL) {
				persistent_script = (zend_persistent_script *)bucket->data;
				if (persistent_script && !persistent_script->corrupted) {
					memcpy(ZCG(key), persistent_script->full_path, persistent_script->full_path_len + 1);
					ZCG(key_len) = persistent_script->full_path_len;
					ZCG(cache_opline) = EG(opline_ptr) ? *EG(opline_ptr) : NULL;
					ZCG(cache_persistent_script) = persistent_script;
					return estrndup(persistent_script->full_path, persistent_script->full_path_len);
				}
		    }

		    /* Check if requested file already cached (by key) */
			handle.filename = (char*)filename;
			handle.free_filename = 0;
			handle.opened_path = NULL;
			key = accel_make_persistent_key_ex(&handle, filename_len, &key_length TSRMLS_CC);
			if (!ZCG(accel_directives).revalidate_path &&
			    key &&
				(persistent_script = zend_accel_hash_find(&ZCSG(hash), key, key_length + 1)) != NULL &&
			    !persistent_script->corrupted) {

				/* we have persistent script */
				ZCG(cache_opline) = EG(opline_ptr) ? *EG(opline_ptr) : NULL;
				ZCG(cache_persistent_script) = persistent_script;
				return estrndup(persistent_script->full_path, persistent_script->full_path_len);
			}

			/* find the full real path */
			resolved_path = accelerator_orig_zend_resolve_path(filename, filename_len TSRMLS_CC);

		    /* Check if requested file already cached (by real path) */
			if (resolved_path &&
			    (bucket = zend_accel_hash_find_entry(&ZCSG(hash), resolved_path, strlen(resolved_path) + 1)) != NULL) {
				persistent_script = (zend_persistent_script *)bucket->data;

				if (persistent_script && !persistent_script->corrupted) {
					if (key && !ZCG(accel_directives).revalidate_path) {
						/* add another "key" for the same bucket */
				    	SHM_UNPROTECT();
						zend_shared_alloc_lock(TSRMLS_C);
						zend_accel_add_key(key, key_length, bucket TSRMLS_CC);
						zend_shared_alloc_unlock(TSRMLS_C);
			    		SHM_PROTECT();
					}
					ZCG(cache_opline) = (EG(opline_ptr) && key) ? *EG(opline_ptr): NULL;
					ZCG(cache_persistent_script) = key ? persistent_script : NULL;
					return resolved_path;
				}
			}
			ZCG(cache_opline) = NULL;
			ZCG(cache_persistent_script) = NULL;
			return resolved_path;
		}
	}
	ZCG(cache_opline) = NULL;
	ZCG(cache_persistent_script) = NULL;
	return accelerator_orig_zend_resolve_path(filename, filename_len TSRMLS_CC);
}

#endif

static void zend_reset_cache_vars(TSRMLS_D)
{
	ZSMMG(memory_exhausted) = 0;
	ZCSG(hits) = 0;
	ZCSG(misses) = 0;
	ZCSG(blacklist_misses) = 0;
	ZSMMG(wasted_shared_memory) = 0;
	ZCSG(restart_pending) = 0;
	ZCSG(force_restart_time) = 0;
}

static void accel_activate(void)
{
	TSRMLS_FETCH();

	if (!ZCG(enabled) || !accel_startup_ok) {
		return;
	}

	SHM_UNPROTECT();
	/* PHP-5.4 and above return "double", but we use 1 sec precision */
	ZCG(request_time) = (time_t)sapi_get_request_time(TSRMLS_C);
	ZCG(cache_opline) = NULL;
	ZCG(cache_persistent_script) = NULL;
	ZCG(include_path_check) = !ZCG(include_path_key);

	if (ZCG(counted)) {
#ifdef ZTS
		zend_accel_error(ACCEL_LOG_WARNING, "Stuck count for thread id %d", tsrm_thread_id());
#else
		zend_accel_error(ACCEL_LOG_WARNING, "Stuck count for pid %d", getpid());
#endif
		accel_unlock_all(TSRMLS_C);
		ZCG(counted) = 0;
	}

	if (ZCSG(restart_pending)) {
		zend_shared_alloc_lock(TSRMLS_C);
		if (ZCSG(restart_pending) != 0) { /* check again, to ensure that the cache wasn't already cleaned by another process */
			if (accel_is_inactive(TSRMLS_C) == SUCCESS) {
				zend_accel_error(ACCEL_LOG_DEBUG, "Restarting!");
				ZCSG(restart_pending) = 0;
				switch ZCSG(restart_reason) {
					case ACCEL_RESTART_OOM:
						ZCSG(oom_restarts)++;
						break;
					case ACCEL_RESTART_HASH:
						ZCSG(hash_restarts)++;
						break;
					case ACCEL_RESTART_USER:
						ZCSG(manual_restarts)++;
						break;
				}
				accel_restart_enter(TSRMLS_C);

				zend_reset_cache_vars(TSRMLS_C);
				zend_accel_hash_clean(&ZCSG(hash));

				/* include_paths keeps only the first path */
				if (ZCSG(include_paths).num_entries > 1) {
					ZCSG(include_paths).num_entries = 1;
					ZCSG(include_paths).num_direct_entries = 1;
					memset(ZCSG(include_paths).hash_table, 0, sizeof(zend_accel_hash_entry*) * ZCSG(include_paths).max_num_entries);
					ZCSG(include_paths).hash_table[zend_inline_hash_func(ZCSG(include_paths).hash_entries[0].key, ZCSG(include_paths).hash_entries[0].key_length) % ZCSG(include_paths).max_num_entries] = &ZCSG(include_paths).hash_entries[0];
				}

#if (ZEND_EXTENSION_API_NO > PHP_5_3_X_API_NO) && !defined(ZTS)
				accel_interned_strings_restore_state(TSRMLS_C);
#endif

				zend_shared_alloc_restore_state();
				ZCSG(accelerator_enabled) = ZCSG(cache_status_before_restart);
				ZCSG(last_restart_time) = ZCG(request_time);
				accel_restart_leave(TSRMLS_C);
			}
		}
		zend_shared_alloc_unlock(TSRMLS_C);
	}

	/* check if ZCG(function_table) wasn't somehow polluted on the way */
	if (ZCG(internal_functions_count) != zend_hash_num_elements(&ZCG(function_table))) {
		zend_accel_error(ACCEL_LOG_WARNING, "Internal functions count changed - was %d, now %d", ZCG(internal_functions_count), zend_hash_num_elements(&ZCG(function_table)));
	}

	if (ZCG(accel_directives).validate_timestamps) {
		time_t now = ZCG(request_time);
		if (now > ZCSG(revalidate_at) + (time_t)ZCG(accel_directives).revalidate_freq) {
			ZCSG(revalidate_at) = now;
		}
	}

	ZCG(cwd) = NULL;

	SHM_PROTECT();
}

#if !ZEND_DEBUG

/* Fast Request Shutdown
 * =====================
 * Zend Memory Manager frees memory by its own. We don't have to free each
 * allocated block separately, but we like to call all the destructors and
 * callbacks in exactly the same order.
 */

static void accel_fast_hash_destroy(HashTable *ht)
{
	Bucket *p = ht->pListHead;

	while (p != NULL) {
		ht->pDestructor(p->pData);
		p = p->pListNext;
	}
}

static void accel_fast_zval_ptr_dtor(zval **zval_ptr)
{
	zval *zvalue = *zval_ptr;

	if (Z_DELREF_P(zvalue) == 0) {
#if ZEND_EXTENSION_API_NO >= PHP_5_3_X_API_NO
		switch (Z_TYPE_P(zvalue) & IS_CONSTANT_TYPE_MASK) {
#else
		switch (Z_TYPE_P(zvalue) & ~IS_CONSTANT_INDEX) {
#endif
			case IS_ARRAY:
			case IS_CONSTANT_ARRAY: {
					TSRMLS_FETCH();

#if ZEND_EXTENSION_API_NO >= PHP_5_3_X_API_NO
					GC_REMOVE_ZVAL_FROM_BUFFER(zvalue);
#endif
					if (zvalue->value.ht && (zvalue->value.ht != &EG(symbol_table))) {
						/* break possible cycles */
						Z_TYPE_P(zvalue) = IS_NULL;
						zvalue->value.ht->pDestructor = (dtor_func_t)accel_fast_zval_ptr_dtor;
						accel_fast_hash_destroy(zvalue->value.ht);
					}
				}
				break;
			case IS_OBJECT:
				{
					TSRMLS_FETCH();

#if ZEND_EXTENSION_API_NO >= PHP_5_3_X_API_NO
					GC_REMOVE_ZVAL_FROM_BUFFER(zvalue);
#endif
					Z_OBJ_HT_P(zvalue)->del_ref(zvalue TSRMLS_CC);
				}
				break;
			case IS_RESOURCE:
				{
					TSRMLS_FETCH();

					/* destroy resource */
					zend_list_delete(zvalue->value.lval);
				}
				break;
			case IS_LONG:
			case IS_DOUBLE:
			case IS_BOOL:
			case IS_NULL:
			case IS_STRING:
			case IS_CONSTANT:
			default:
				return;
				break;
		}
	}
}

static int accel_clean_non_persistent_function(zend_function *function TSRMLS_DC)
{
	if (function->type == ZEND_INTERNAL_FUNCTION) {
		return ZEND_HASH_APPLY_STOP;
	} else {
		if (function->op_array.static_variables) {
			function->op_array.static_variables->pDestructor = (dtor_func_t)accel_fast_zval_ptr_dtor;
			accel_fast_hash_destroy(function->op_array.static_variables);
			function->op_array.static_variables = NULL;
		}
		return (--(*function->op_array.refcount) <= 0) ?
			ZEND_HASH_APPLY_REMOVE :
			ZEND_HASH_APPLY_KEEP;
	}
}

static int accel_cleanup_function_data(zend_function *function TSRMLS_DC)
{
	if (function->type == ZEND_USER_FUNCTION) {
		if (function->op_array.static_variables) {
			function->op_array.static_variables->pDestructor = (dtor_func_t)accel_fast_zval_ptr_dtor;
			accel_fast_hash_destroy(function->op_array.static_variables);
			function->op_array.static_variables = NULL;
		}
	}
	return 0;
}

static int accel_clean_non_persistent_class(zend_class_entry **pce TSRMLS_DC)
{
	zend_class_entry *ce = *pce;

	if (ce->type == ZEND_INTERNAL_CLASS) {
		return ZEND_HASH_APPLY_STOP;
	} else {
#if ZEND_EXTENSION_API_NO > PHP_5_3_X_API_NO
		if (ce->ce_flags & ZEND_HAS_STATIC_IN_METHODS) {
			zend_hash_apply(&ce->function_table, (apply_func_t) accel_cleanup_function_data TSRMLS_CC);
		}
		if (ce->static_members_table) {
			int i;

			for (i = 0; i < ce->default_static_members_count; i++) {
				if (ce->static_members_table[i]) {
					accel_fast_zval_ptr_dtor(&ce->static_members_table[i]);
					ce->static_members_table[i] = NULL;
				}
			}
			ce->static_members_table = NULL;
		}
#else
		zend_hash_apply(&ce->function_table, (apply_func_t) accel_cleanup_function_data TSRMLS_CC);
		if (ce->static_members) {
			ce->static_members->pDestructor = (dtor_func_t)accel_fast_zval_ptr_dtor;
			accel_fast_hash_destroy(ce->static_members);
			ce->static_members = NULL;
		}
#endif
		return ZEND_HASH_APPLY_REMOVE;
	}
}

static int accel_clean_non_persistent_constant(zend_constant *c TSRMLS_DC)
{
	if (c->flags & CONST_PERSISTENT) {
		return ZEND_HASH_APPLY_STOP;
	} else {
		interned_free(c->name);
		return ZEND_HASH_APPLY_REMOVE;
	}
}

static void zend_accel_fast_shutdown(TSRMLS_D)
{
	if (EG(full_tables_cleanup)) {
		EG(symbol_table).pDestructor = (dtor_func_t)accel_fast_zval_ptr_dtor;
	} else {
		dtor_func_t old_destructor;

		if (EG(objects_store).top > 1 || zend_hash_num_elements(&EG(regular_list)) > 0) {
			/* We don't have to destroy all zvals if they cannot call any destructors */

		    old_destructor = EG(symbol_table).pDestructor;
			EG(symbol_table).pDestructor = (dtor_func_t)accel_fast_zval_ptr_dtor;
			zend_try {
				zend_hash_graceful_reverse_destroy(&EG(symbol_table));
			} zend_end_try();
			EG(symbol_table).pDestructor = old_destructor;
		}
		zend_hash_init(&EG(symbol_table), 0, NULL, NULL, 0);
		old_destructor = EG(function_table)->pDestructor;
		EG(function_table)->pDestructor = NULL;
		zend_hash_reverse_apply(EG(function_table), (apply_func_t) accel_clean_non_persistent_function TSRMLS_CC);
		EG(function_table)->pDestructor = old_destructor;
		old_destructor = EG(class_table)->pDestructor;
		EG(class_table)->pDestructor = NULL;
		zend_hash_reverse_apply(EG(class_table), (apply_func_t) accel_clean_non_persistent_class TSRMLS_CC);
		EG(class_table)->pDestructor = old_destructor;
		old_destructor = EG(zend_constants)->pDestructor;
		EG(zend_constants)->pDestructor = NULL;
		zend_hash_reverse_apply(EG(zend_constants), (apply_func_t) accel_clean_non_persistent_constant TSRMLS_CC);
		EG(zend_constants)->pDestructor = old_destructor;
	}
	CG(unclean_shutdown) = 1;
}
#endif

static void accel_deactivate(void)
{
	/* ensure that we restore function_table and class_table
	 * In general, they're restored by persistent_compile_file(), but in case
	 * the script is aborted abnormally, they may become messed up.
	 */
	TSRMLS_FETCH();

	if (!ZCG(enabled) || !accel_startup_ok) {
		return;
	}

	zend_shared_alloc_safe_unlock(TSRMLS_C); /* be sure we didn't leave cache locked */
	accel_unlock_all(TSRMLS_C);
	ZCG(counted) = 0;

#if !ZEND_DEBUG
	if (ZCG(accel_directives).fast_shutdown) {
		zend_accel_fast_shutdown(TSRMLS_C);
	}
#endif

	if (ZCG(cwd)) {
		efree(ZCG(cwd));
		ZCG(cwd) = NULL;
	}

}

static int accelerator_remove_cb(zend_extension *element1, zend_extension *element2)
{
	(void)element2; /* keep the compiler happy */

	if (!strcmp(element1->name, ACCELERATOR_PRODUCT_NAME )) {
		element1->startup = NULL;
#if 0
		/* We have to call shutdown callback it to free TS resources */
		element1->shutdown = NULL;
#endif
		element1->activate = NULL;
		element1->deactivate = NULL;
		element1->op_array_handler = NULL;

#ifdef __DEBUG_MESSAGES__
        fprintf(stderr, ACCELERATOR_PRODUCT_NAME " is disabled: %s\n", (zps_failure_reason ? zps_failure_reason : "unknown error"));
        fflush(stderr);
#endif
	}

	return 0;
}

static void zps_startup_failure(char *reason, char *api_reason, int (*cb)(zend_extension *, zend_extension *) TSRMLS_DC)
{
	accel_startup_ok = 0;
	zps_failure_reason = reason;
	zps_api_failure_reason = api_reason?api_reason:reason;
	zend_llist_del_element(&zend_extensions, NULL, (int (*)(void *, void *))cb);
}

static inline int accel_find_sapi(TSRMLS_D)
{
	static const char *supported_sapis[] = {
		"apache",
		"fastcgi",
		"cli-server",
		"cgi-fcgi",
		"fpm-fcgi",
		"isapi",
		"apache2filter",
		"apache2handler",
		"litespeed",
		NULL
	};
	const char **sapi_name;

	if (sapi_module.name) {
		for (sapi_name = supported_sapis; *sapi_name; sapi_name++) {
			if (strcmp(sapi_module.name, *sapi_name) == 0) {
				return SUCCESS;
			}
		}
		if (ZCG(accel_directives).enable_cli &&
		    strcmp(sapi_module.name, "cli") == 0) {
			return SUCCESS;
		}
	}

	return FAILURE;
}

static int zend_accel_init_shm(TSRMLS_D)
{
	zend_shared_alloc_lock(TSRMLS_C);

	accel_shared_globals = zend_shared_alloc(sizeof(zend_accel_shared_globals));
	if (!accel_shared_globals) {
		zend_accel_error(ACCEL_LOG_FATAL, "Insufficient shared memory!");
		return FAILURE;
	}
	ZSMMG(app_shared_globals) = accel_shared_globals;

	zend_accel_hash_init(&ZCSG(hash), ZCG(accel_directives).max_accelerated_files);
	zend_accel_hash_init(&ZCSG(include_paths), 32);

#if ZEND_EXTENSION_API_NO > PHP_5_3_X_API_NO

# ifndef ZTS
	zend_hash_init(&ZCSG(interned_strings), (ZCG(accel_directives).interned_strings_buffer * 1024 * 1024) / (sizeof(Bucket) + sizeof(Bucket*) + 8 /* average string length */), NULL, NULL, 1);
	ZCSG(interned_strings).nTableMask = ZCSG(interned_strings).nTableSize - 1;
	ZCSG(interned_strings).arBuckets = zend_shared_alloc(ZCSG(interned_strings).nTableSize * sizeof(Bucket *));
	ZCSG(interned_strings_start) = zend_shared_alloc((ZCG(accel_directives).interned_strings_buffer * 1024 * 1024));
	if (!ZCSG(interned_strings).arBuckets || !ZCSG(interned_strings_start)) {
		zend_accel_error(ACCEL_LOG_FATAL, ACCELERATOR_PRODUCT_NAME " cannot allocate buffer for interned strings");
		return FAILURE;
	}
	ZCSG(interned_strings_end)   = ZCSG(interned_strings_start) + (ZCG(accel_directives).interned_strings_buffer * 1024 * 1024);
	ZCSG(interned_strings_top)   = ZCSG(interned_strings_start);
# else
	ZCSG(interned_strings_start) = ZCSG(interned_strings_end) = NULL;
# endif

	orig_interned_strings_start = CG(interned_strings_start);
	orig_interned_strings_end = CG(interned_strings_end);
	orig_new_interned_string = zend_new_interned_string;
	orig_interned_strings_snapshot = zend_interned_strings_snapshot;
	orig_interned_strings_restore = zend_interned_strings_restore;

	CG(interned_strings_start) = ZCSG(interned_strings_start);
	CG(interned_strings_end) = ZCSG(interned_strings_end);
	zend_new_interned_string = accel_new_interned_string_for_php;
	zend_interned_strings_snapshot = accel_interned_strings_snapshot_for_php;
	zend_interned_strings_restore = accel_interned_strings_restore_for_php;

# ifndef ZTS
	accel_use_shm_interned_strings(TSRMLS_C);
	accel_interned_strings_save_state(TSRMLS_C);
# endif

#endif

	zend_reset_cache_vars(TSRMLS_C);

	ZCSG(oom_restarts) = 0;
	ZCSG(hash_restarts) = 0;
	ZCSG(manual_restarts) = 0;

	ZCSG(accelerator_enabled) = 1;
	ZCSG(start_time) = zend_accel_get_time();
	ZCSG(last_restart_time) = 0;
	ZCSG(restart_in_progress) = 0;

	zend_shared_alloc_unlock(TSRMLS_C);

	return SUCCESS;
}

static void accel_globals_ctor(zend_accel_globals *accel_globals TSRMLS_DC)
{
	memset(accel_globals, 0, sizeof(zend_accel_globals));
	zend_hash_init(&accel_globals->function_table, zend_hash_num_elements(CG(function_table)), NULL, ZEND_FUNCTION_DTOR, 1);
	zend_accel_copy_internal_functions(TSRMLS_C);
}

static void accel_globals_dtor(zend_accel_globals *accel_globals TSRMLS_DC)
{
	accel_globals->function_table.pDestructor = NULL;
	zend_hash_destroy(&accel_globals->function_table);
}

static int accel_startup(zend_extension *extension)
{
	zend_function *func;
	zend_ini_entry *ini_entry;
	TSRMLS_FETCH();

#ifdef ZTS
	accel_globals_id = ts_allocate_id(&accel_globals_id, sizeof(zend_accel_globals), (ts_allocate_ctor) accel_globals_ctor, (ts_allocate_dtor) accel_globals_dtor);
#else
	accel_globals_ctor(&accel_globals);
#endif

#ifdef ZEND_WIN32
	_setmaxstdio(2048); /* The default configuration is limited to 512 stdio files */
#endif

	if (start_accel_module() == FAILURE) {
		accel_startup_ok = 0;
		zend_error(E_WARNING, ACCELERATOR_PRODUCT_NAME ": module registration failed!");
		return FAILURE;
	}

	/* no supported SAPI found - disable acceleration and stop initialization */
	if (accel_find_sapi(TSRMLS_C) == FAILURE) {
		accel_startup_ok = 0;
		if (!ZCG(accel_directives).enable_cli &&
		    strcmp(sapi_module.name, "cli") == 0) {
			zps_startup_failure("Opcode Caching is disabled for CLI", NULL, accelerator_remove_cb TSRMLS_CC);
		} else {
			zps_startup_failure("Opcode Caching is only supported in Apache, ISAPI, FPM, FastCGI and LiteSpeed SAPIs", NULL, accelerator_remove_cb TSRMLS_CC);
		}
		return SUCCESS;
	}

	if (ZCG(enabled) == 0) {
		return SUCCESS ;
	}
/********************************************/
/* End of non-SHM dependent initializations */
/********************************************/
	switch (zend_shared_alloc_startup(ZCG(accel_directives).memory_consumption)) {
		case ALLOC_SUCCESS:
			if (zend_accel_init_shm(TSRMLS_C) == FAILURE) {
				accel_startup_ok = 0;
				return FAILURE;
			}
			break;
		case ALLOC_FAILURE:
			accel_startup_ok = 0;
			zend_accel_error(ACCEL_LOG_FATAL, "Failure to initialize shared memory structures - probably not enough shared memory.");
			return SUCCESS;
		case SUCCESSFULLY_REATTACHED:
			accel_shared_globals = (zend_accel_shared_globals *) ZSMMG(app_shared_globals);
#if ZEND_EXTENSION_API_NO > PHP_5_3_X_API_NO
			zend_shared_alloc_lock(TSRMLS_C);
			orig_interned_strings_start = CG(interned_strings_start);
			orig_interned_strings_end = CG(interned_strings_end);
			orig_new_interned_string = zend_new_interned_string;
			orig_interned_strings_snapshot = zend_interned_strings_snapshot;
			orig_interned_strings_restore = zend_interned_strings_restore;

			CG(interned_strings_start) = ZCSG(interned_strings_start);
			CG(interned_strings_end) = ZCSG(interned_strings_end);
			zend_new_interned_string = accel_new_interned_string_for_php;
			zend_interned_strings_snapshot = accel_interned_strings_snapshot_for_php;
			zend_interned_strings_restore = accel_interned_strings_restore_for_php;

# ifndef ZTS
			accel_use_shm_interned_strings(TSRMLS_C);
# endif

			zend_shared_alloc_unlock(TSRMLS_C);
#endif

			break;
		case FAILED_REATTACHED:
			accel_startup_ok = 0;
			zend_accel_error(ACCEL_LOG_FATAL, "Failure to initialize shared memory structures - can not reattach to exiting shared memory.");
			return SUCCESS;
			break;
	}

	/* from this point further, shared memory is supposed to be OK */

	/* Override compiler */
	accelerator_orig_compile_file = zend_compile_file;
	zend_compile_file = persistent_compile_file;

	/* Override stream opener function (to eliminate open() call caused by
	 * include/require statements ) */
	accelerator_orig_zend_stream_open_function = zend_stream_open_function;
	zend_stream_open_function = persistent_stream_open_function;

#if ZEND_EXTENSION_API_NO >= PHP_5_3_X_API_NO
	/* Override path resolver function (to eliminate stat() calls caused by
	 * include_once/require_once statements */
	accelerator_orig_zend_resolve_path = zend_resolve_path;
	zend_resolve_path = persistent_zend_resolve_path;
#endif

	if (ZCG(accel_directives).validate_timestamps) {
		ZCSG(revalidate_at) = zend_accel_get_time() + ZCG(accel_directives).revalidate_freq;
	}

	/* Override chdir() function */
	if (zend_hash_find(CG(function_table), "chdir", sizeof("chdir"), (void**)&func) == SUCCESS &&
	    func->type == ZEND_INTERNAL_FUNCTION) {
		orig_chdir = func->internal_function.handler;
		func->internal_function.handler = ZEND_FN(accel_chdir);
	}
	ZCG(cwd) = NULL;

	/* Override "include_path" modifier callback */
	if (zend_hash_find(EG(ini_directives), "include_path", sizeof("include_path"), (void **) &ini_entry) == SUCCESS) {
		ZCG(include_path) = INI_STR("include_path");
		ZCG(include_path_key) = NULL;
		if (ZCG(include_path) && *ZCG(include_path)) {
			ZCG(include_path_len) = strlen(ZCG(include_path));
			ZCG(include_path_key) = zend_accel_hash_find(&ZCSG(include_paths), ZCG(include_path), ZCG(include_path_len) + 1);
			if (!ZCG(include_path_key) &&
			    !zend_accel_hash_is_full(&ZCSG(include_paths))) {
				char *key;

				zend_shared_alloc_lock(TSRMLS_C);
				key = zend_shared_alloc(ZCG(include_path_len) + 2);
				if (key) {
					memcpy(key, ZCG(include_path), ZCG(include_path_len) + 1);
					key[ZCG(include_path_len) + 1] = 'A' + ZCSG(include_paths).num_entries;
					ZCG(include_path_key) = key + ZCG(include_path_len) + 1;
					zend_accel_hash_update(&ZCSG(include_paths), key, ZCG(include_path_len) + 1, 0, ZCG(include_path_key));
				} else {
					zend_accel_schedule_restart_if_necessary(ACCEL_RESTART_OOM TSRMLS_CC);
				}
				zend_shared_alloc_unlock(TSRMLS_C);
			}
		} else {
			ZCG(include_path) = "";
			ZCG(include_path_len) = 0;
		}
		orig_include_path_on_modify = ini_entry->on_modify;
		ini_entry->on_modify = accel_include_path_on_modify;
	}

	zend_shared_alloc_lock(TSRMLS_C);
	zend_shared_alloc_save_state();
	zend_shared_alloc_unlock(TSRMLS_C);

	SHM_PROTECT();

	accel_startup_ok = 1;

	/* Override file_exists(), is_file() and is_readable() */
	zend_accel_override_file_functions(TSRMLS_C);

	/* Load black list */
	accel_blacklist.entries = NULL;
	if (ZCG(enabled) && accel_startup_ok &&
	    ZCG(accel_directives).user_blacklist_filename && 
	    *ZCG(accel_directives.user_blacklist_filename)) {
		zend_accel_blacklist_init(&accel_blacklist);
		zend_accel_blacklist_load(&accel_blacklist, ZCG(accel_directives.user_blacklist_filename));
	}
	
#if 0
	/* FIXME: We probably don't need it here */
	zend_accel_copy_internal_functions(TSRMLS_C);
#endif

	return SUCCESS;
}

static void accel_free_ts_resources()
{
#ifndef ZTS
	accel_globals_dtor(&accel_globals);
#else
	ts_free_id(accel_globals_id);
#endif
}

void accel_shutdown(TSRMLS_D)
{
	zend_ini_entry *ini_entry;

	zend_accel_blacklist_shutdown(&accel_blacklist);

	if (!ZCG(enabled) || !accel_startup_ok) {
		accel_free_ts_resources();
		return;
	}

	accel_free_ts_resources();
	zend_shared_alloc_shutdown();
	zend_compile_file = accelerator_orig_compile_file;

	if (zend_hash_find(EG(ini_directives), "include_path", sizeof("include_path"), (void **) &ini_entry) == SUCCESS) {
		ini_entry->on_modify = orig_include_path_on_modify;
	}

#if ZEND_EXTENSION_API_NO > PHP_5_3_X_API_NO
# ifndef ZTS
	zend_hash_clean(CG(function_table));
	zend_hash_clean(CG(class_table));
	zend_hash_clean(EG(zend_constants));
# endif
	CG(interned_strings_start) = orig_interned_strings_start;
	CG(interned_strings_end) = orig_interned_strings_end;
	zend_new_interned_string = orig_new_interned_string;
	zend_interned_strings_snapshot = orig_interned_strings_snapshot;
	zend_interned_strings_restore = orig_interned_strings_restore;
#endif

}

void zend_accel_schedule_restart(zend_accel_restart_reason reason TSRMLS_DC)
{
	if (ZCSG(restart_pending)) {
		/* don't schedule twice */
		return;
	}
	zend_accel_error(ACCEL_LOG_DEBUG, "Restart Scheduled!");

	SHM_UNPROTECT();
	ZCSG(restart_pending) = 1;
	ZCSG(restart_reason) = reason;
	ZCSG(cache_status_before_restart) = ZCSG(accelerator_enabled);
	ZCSG(accelerator_enabled) = 0;

	if (ZCG(accel_directives).force_restart_timeout) {
		ZCSG(force_restart_time) = zend_accel_get_time() + ZCG(accel_directives).force_restart_timeout;
	} else {
		ZCSG(force_restart_time) = 0;
	}
	SHM_PROTECT();
}

/* this is needed because on WIN32 lock is not decreased unless ZCG(counted) is set */
#ifdef ZEND_WIN32
#define accel_deactivate_now() ZCG(counted) = 1; accel_deactivate_sub(TSRMLS_C)
#else
#define accel_deactivate_now() accel_deactivate_sub(TSRMLS_C)
#endif

/* ensures it is OK to read SHM
	if it's not OK (restart in progress) returns FAILURE
	if OK returns SUCCESS
	MUST call accelerator_shm_read_unlock after done lock operations
*/
int accelerator_shm_read_lock(TSRMLS_D)
{
	if (ZCG(counted)) {
		/* counted means we are holding read lock for SHM, so that nothing bad can happen */
		return SUCCESS;
	} else {
		/* here accelerator is active but we do not hold SHM lock. This means restart was scheduled
			or is in progress now */
		accel_activate_add(TSRMLS_C); /* acquire usage lock */
		/* Now if we weren't inside restart, restart would not begin until we remove usage lock */
		if (ZCSG(restart_in_progress)) {
			/* we already were inside restart this means it's not safe to touch shm */
			accel_deactivate_now(); /* drop usage lock */
			return FAILURE;
		}
	}
	return SUCCESS;
}

/* must be called ONLY after SUCCESSFUL accelerator_shm_read_lock */
void accelerator_shm_read_unlock(TSRMLS_D)
{
	if (!ZCG(counted)) {
		/* counted is 0 - meaning we had to readlock manually, release readlock now */
		accel_deactivate_now();
	}
}

ZEND_EXT_API zend_extension zend_extension_entry = {
	ACCELERATOR_PRODUCT_NAME,               /* name */
	ACCELERATOR_VERSION,					/* version */
	"Zend Technologies",					/* author */
	"http://www.zend.com/",					/* URL */
	"Copyright (c) 1999-2013",				/* copyright */
	accel_startup,					   		/* startup */
	NULL,									/* shutdown */
	accel_activate,							/* per-script activation */
	accel_deactivate,						/* per-script deactivation */
	NULL,									/* message handler */
	NULL,									/* op_array handler */
	NULL,									/* extended statement handler */
	NULL,									/* extended fcall begin handler */
	NULL,									/* extended fcall end handler */
	NULL,									/* op_array ctor */
	NULL,									/* op_array dtor */
	STANDARD_ZEND_EXTENSION_PROPERTIES
};
