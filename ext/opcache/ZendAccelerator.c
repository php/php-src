/*
   +----------------------------------------------------------------------+
   | Zend OPcache                                                         |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2018 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <zeev@php.net>                                 |
   |          Stanislav Malyshev <stas@zend.com>                          |
   |          Dmitry Stogov <dmitry@php.net>                              |
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
#include "ext/pcre/php_pcre.h"
#include "ext/standard/md5.h"

#ifdef HAVE_OPCACHE_FILE_CACHE
# include "zend_file_cache.h"
#endif

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

#ifdef __AVX__
#include <immintrin.h>
#endif

#define SHM_PROTECT() \
	do { \
		if (ZCG(accel_directives).protect_memory) { \
			zend_accel_shared_protect(1); \
		} \
	} while (0)
#define SHM_UNPROTECT() \
	do { \
		if (ZCG(accel_directives).protect_memory) { \
			zend_accel_shared_protect(0); \
		} \
	} while (0)

ZEND_EXTENSION();

#ifndef ZTS
zend_accel_globals accel_globals;
#else
int accel_globals_id;
#if defined(COMPILE_DL_OPCACHE)
ZEND_TSRMLS_CACHE_DEFINE()
#endif
#endif

/* Points to the structure shared across all PHP processes */
zend_accel_shared_globals *accel_shared_globals = NULL;

/* true globals, no need for thread safety */
zend_bool accel_startup_ok = 0;
static char *zps_failure_reason = NULL;
char *zps_api_failure_reason = NULL;
#ifdef HAVE_OPCACHE_FILE_CACHE
zend_bool file_cache_only = 0;  /* process uses file cache only */
#endif
#if ENABLE_FILE_CACHE_FALLBACK
zend_bool fallback_process = 0; /* process uses file cache fallback */
#endif

static zend_op_array *(*accelerator_orig_compile_file)(zend_file_handle *file_handle, int type);
static int (*accelerator_orig_zend_stream_open_function)(const char *filename, zend_file_handle *handle );
static zend_string *(*accelerator_orig_zend_resolve_path)(const char *filename, size_t filename_len);
static zif_handler orig_chdir = NULL;
static ZEND_INI_MH((*orig_include_path_on_modify)) = NULL;
static int (*orig_post_startup_cb)(void);

static void accel_gen_system_id(void);
static int accel_post_startup(void);

#ifdef ZEND_WIN32
# define INCREMENT(v) InterlockedIncrement64(&ZCSG(v))
# define DECREMENT(v) InterlockedDecrement64(&ZCSG(v))
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

	for (p = filename;
	     (*p >= 'a' && *p <= 'z') ||
	     (*p >= 'A' && *p <= 'Z') ||
	     (*p >= '0' && *p <= '9') ||
	     *p == '+' || *p == '-' || *p == '.';
	     p++);
	return ((p != filename) && (p[0] == ':') && (p[1] == '/') && (p[2] == '/'));
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
			zend_string_release_ex(ZCG(cwd), 0);
		}
		ZCG(cwd) = zend_string_init(cwd, strlen(cwd), 0);
	} else {
		if (ZCG(cwd)) {
			zend_string_release_ex(ZCG(cwd), 0);
			ZCG(cwd) = NULL;
		}
	}
	ZCG(cwd_key_len) = 0;
	ZCG(cwd_check) = 1;
}

static inline zend_string* accel_getcwd(void)
{
	if (ZCG(cwd)) {
		return ZCG(cwd);
	} else {
		char cwd[MAXPATHLEN + 1];

		if (!VCWD_GETCWD(cwd, MAXPATHLEN)) {
			return NULL;
		}
		ZCG(cwd) = zend_string_init(cwd, strlen(cwd), 0);
		ZCG(cwd_key_len) = 0;
		ZCG(cwd_check) = 1;
		return ZCG(cwd);
	}
}

void zend_accel_schedule_restart_if_necessary(zend_accel_restart_reason reason)
{
	if ((((double) ZSMMG(wasted_shared_memory)) / ZCG(accel_directives).memory_consumption) >= ZCG(accel_directives).max_wasted_percentage) {
 		zend_accel_schedule_restart(reason);
	}
}

/* O+ tracks changes of "include_path" directive. It stores all the requested
 * values in ZCG(include_paths) shared hash table, current value in
 * ZCG(include_path)/ZCG(include_path_len) and one letter "path key" in
 * ZCG(include_path_key).
 */
static ZEND_INI_MH(accel_include_path_on_modify)
{
	int ret = orig_include_path_on_modify(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage);

	if (ret == SUCCESS) {
		ZCG(include_path) = new_value;
		ZCG(include_path_key_len) = 0;
		ZCG(include_path_check) = 1;
	}
	return ret;
}

static inline void accel_restart_enter(void)
{
#ifdef ZEND_WIN32
	INCREMENT(restart_in);
#else
	struct flock restart_in_progress;

	restart_in_progress.l_type = F_WRLCK;
	restart_in_progress.l_whence = SEEK_SET;
	restart_in_progress.l_start = 2;
	restart_in_progress.l_len = 1;

	if (fcntl(lock_file, F_SETLK, &restart_in_progress) == -1) {
		zend_accel_error(ACCEL_LOG_DEBUG, "RestartC(+1):  %s (%d)", strerror(errno), errno);
	}
#endif
	ZCSG(restart_in_progress) = 1;
}

static inline void accel_restart_leave(void)
{
#ifdef ZEND_WIN32
	ZCSG(restart_in_progress) = 0;
	DECREMENT(restart_in);
#else
	struct flock restart_finished;

	restart_finished.l_type = F_UNLCK;
	restart_finished.l_whence = SEEK_SET;
	restart_finished.l_start = 2;
	restart_finished.l_len = 1;

	ZCSG(restart_in_progress) = 0;
	if (fcntl(lock_file, F_SETLK, &restart_finished) == -1) {
		zend_accel_error(ACCEL_LOG_DEBUG, "RestartC(-1):  %s (%d)", strerror(errno), errno);
	}
#endif
}

static inline int accel_restart_is_active(void)
{
	if (ZCSG(restart_in_progress)) {
#ifndef ZEND_WIN32
		struct flock restart_check;

		restart_check.l_type = F_WRLCK;
		restart_check.l_whence = SEEK_SET;
		restart_check.l_start = 2;
		restart_check.l_len = 1;

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
static inline int accel_activate_add(void)
{
#ifdef ZEND_WIN32
	INCREMENT(mem_usage);
#else
	struct flock mem_usage_lock;

	mem_usage_lock.l_type = F_RDLCK;
	mem_usage_lock.l_whence = SEEK_SET;
	mem_usage_lock.l_start = 1;
	mem_usage_lock.l_len = 1;

	if (fcntl(lock_file, F_SETLK, &mem_usage_lock) == -1) {
		zend_accel_error(ACCEL_LOG_DEBUG, "UpdateC(+1):  %s (%d)", strerror(errno), errno);
		return FAILURE;
	}
#endif
	return SUCCESS;
}

/* Releases a lock for SHM access */
static inline void accel_deactivate_sub(void)
{
#ifdef ZEND_WIN32
	if (ZCG(counted)) {
		DECREMENT(mem_usage);
		ZCG(counted) = 0;
	}
#else
	struct flock mem_usage_unlock;

	mem_usage_unlock.l_type = F_UNLCK;
	mem_usage_unlock.l_whence = SEEK_SET;
	mem_usage_unlock.l_start = 1;
	mem_usage_unlock.l_len = 1;

	if (fcntl(lock_file, F_SETLK, &mem_usage_unlock) == -1) {
		zend_accel_error(ACCEL_LOG_DEBUG, "UpdateC(-1):  %s (%d)", strerror(errno), errno);
	}
#endif
}

static inline void accel_unlock_all(void)
{
#ifdef ZEND_WIN32
	accel_deactivate_sub();
#else
	struct flock mem_usage_unlock_all;

	mem_usage_unlock_all.l_type = F_UNLCK;
	mem_usage_unlock_all.l_whence = SEEK_SET;
	mem_usage_unlock_all.l_start = 0;
	mem_usage_unlock_all.l_len = 0;

	if (fcntl(lock_file, F_SETLK, &mem_usage_unlock_all) == -1) {
		zend_accel_error(ACCEL_LOG_DEBUG, "UnlockAll:  %s (%d)", strerror(errno), errno);
	}
#endif
}

/* Interned strings support */

/* O+ disables creation of interned strings by regular PHP compiler, instead,
 * it creates interned strings in shared memory when saves a script.
 * Such interned strings are shared across all PHP processes
 */

#define STRTAB_INVALID_POS 0

#define STRTAB_HASH_TO_SLOT(tab, h) \
	((uint32_t*)((char*)(tab) + sizeof(*(tab)) + ((h) & (tab)->nTableMask)))
#define STRTAB_STR_TO_POS(tab, s) \
	((uint32_t)((char*)s - (char*)(tab)))
#define STRTAB_POS_TO_STR(tab, pos) \
	((zend_string*)((char*)(tab) + (pos)))
#define STRTAB_COLLISION(s) \
	(*((uint32_t*)((char*)s - sizeof(uint32_t))))
#define STRTAB_STR_SIZE(s) \
	ZEND_MM_ALIGNED_SIZE_EX(_ZSTR_HEADER_SIZE + ZSTR_LEN(s) + 5, 8)
#define STRTAB_NEXT(s) \
	((zend_string*)((char*)(s) + STRTAB_STR_SIZE(s)))

static void accel_interned_strings_restore_state(void)
{
	zend_string *s, *top;
	uint32_t *hash_slot, n;

	/* clear removed content */
	memset(ZCSG(interned_strings).saved_top,
			0, (char*)ZCSG(interned_strings).top - (char*)ZCSG(interned_strings).saved_top);

	/* Reset "top" */
	ZCSG(interned_strings).top = ZCSG(interned_strings).saved_top;

	/* rehash */
	memset((char*)&ZCSG(interned_strings) + sizeof(zend_string_table),
		STRTAB_INVALID_POS,
		(char*)ZCSG(interned_strings).start -
			((char*)&ZCSG(interned_strings) + sizeof(zend_string_table)));
	s = ZCSG(interned_strings).start;
	top = ZCSG(interned_strings).top;
	n = 0;
	if (EXPECTED(s < top)) {
		do {
			hash_slot = STRTAB_HASH_TO_SLOT(&ZCSG(interned_strings), ZSTR_H(s));
			STRTAB_COLLISION(s) = *hash_slot;
			*hash_slot = STRTAB_STR_TO_POS(&ZCSG(interned_strings), s);
			s = STRTAB_NEXT(s);
			n++;
		} while (s < top);
	}
	ZCSG(interned_strings).nNumOfElements = n;
}

static void accel_interned_strings_save_state(void)
{
	ZCSG(interned_strings).saved_top = ZCSG(interned_strings).top;
}

static zend_always_inline zend_string *accel_find_interned_string(zend_string *str)
{
/* for now interned strings are supported only for non-ZTS build */
	zend_ulong   h;
	uint32_t     pos;
	zend_string *s;

	if (IS_ACCEL_INTERNED(str)) {
		/* this is already an interned string */
		return str;
	}

	if (!ZCG(counted)) {
		if (accel_activate_add() == FAILURE) {
			return str;
		}
		ZCG(counted) = 1;
	}

	h = zend_string_hash_val(str);

	/* check for existing interned string */
	pos = *STRTAB_HASH_TO_SLOT(&ZCSG(interned_strings), h);
	if (EXPECTED(pos != STRTAB_INVALID_POS)) {
		do {
			s = STRTAB_POS_TO_STR(&ZCSG(interned_strings), pos);
			if (EXPECTED(ZSTR_H(s) == h) && zend_string_equal_content(s, str)) {
				return s;
			}
			pos = STRTAB_COLLISION(s);
		} while (pos != STRTAB_INVALID_POS);
	}

	return NULL;
}

zend_string* ZEND_FASTCALL accel_new_interned_string(zend_string *str)
{
	zend_ulong   h;
	uint32_t     pos, *hash_slot;
	zend_string *s;

#ifdef HAVE_OPCACHE_FILE_CACHE
	if (UNEXPECTED(file_cache_only)) {
		return str;
	}
#endif

	if (IS_ACCEL_INTERNED(str)) {
		/* this is already an interned string */
		return str;
	}

	h = zend_string_hash_val(str);

	/* check for existing interned string */
	hash_slot = STRTAB_HASH_TO_SLOT(&ZCSG(interned_strings), h);
	pos = *hash_slot;
	if (EXPECTED(pos != STRTAB_INVALID_POS)) {
		do {
			s = STRTAB_POS_TO_STR(&ZCSG(interned_strings), pos);
			if (EXPECTED(ZSTR_H(s) == h) && zend_string_equal_content(s, str)) {
				zend_string_release(str);
				return s;
			}
			pos = STRTAB_COLLISION(s);
		} while (pos != STRTAB_INVALID_POS);
	}

	if (UNEXPECTED(ZCSG(interned_strings).end - ZCSG(interned_strings).top < STRTAB_STR_SIZE(str))) {
	    /* no memory, return the same non-interned string */
		zend_accel_error(ACCEL_LOG_WARNING, "Interned string buffer overflow");
		return str;
	}

	/* create new interning string in shared interned strings buffer */
	ZCSG(interned_strings).nNumOfElements++;
	s = ZCSG(interned_strings).top;
	hash_slot = STRTAB_HASH_TO_SLOT(&ZCSG(interned_strings), h);
	STRTAB_COLLISION(s) = *hash_slot;
	*hash_slot = STRTAB_STR_TO_POS(&ZCSG(interned_strings), s);
	GC_SET_REFCOUNT(s, 1);
	GC_TYPE_INFO(s) = IS_STRING | ((IS_STR_INTERNED | IS_STR_PERMANENT) << GC_FLAGS_SHIFT);
	ZSTR_H(s) = h;
	ZSTR_LEN(s) = ZSTR_LEN(str);
	memcpy(ZSTR_VAL(s), ZSTR_VAL(str), ZSTR_LEN(s) + 1);
	ZCSG(interned_strings).top = STRTAB_NEXT(s);

	zend_string_release(str);
	return s;
}

static zend_string* ZEND_FASTCALL accel_new_interned_string_for_php(zend_string *str)
{
	zend_string_hash_val(str);
	if (ZCG(counted)) {
		zend_string *ret = accel_find_interned_string(str);

		if (ret) {
			zend_string_release(str);
			return ret;
		}
	}
	return str;
}

static zend_always_inline zend_string *accel_find_interned_string_ex(zend_ulong h, const char *str, size_t size)
{
	uint32_t     pos;
	zend_string *s;

	/* check for existing interned string */
	pos = *STRTAB_HASH_TO_SLOT(&ZCSG(interned_strings), h);
	if (EXPECTED(pos != STRTAB_INVALID_POS)) {
		do {
			s = STRTAB_POS_TO_STR(&ZCSG(interned_strings), pos);
			if (EXPECTED(ZSTR_H(s) == h) && EXPECTED(ZSTR_LEN(s) == size)) {
				if (!memcmp(ZSTR_VAL(s), str, size)) {
					return s;
				}
			}
			pos = STRTAB_COLLISION(s);
		} while (pos != STRTAB_INVALID_POS);
	}
	return NULL;
}

static zend_string* ZEND_FASTCALL accel_init_interned_string_for_php(const char *str, size_t size, int permanent)
{
	if (ZCG(counted)) {
	    zend_ulong h = zend_inline_hash_func(str, size);
		zend_string *ret = accel_find_interned_string_ex(h, str, size);

		if (!ret) {
			ret = zend_string_init(str, size, permanent);
			ZSTR_H(ret) = h;
		}

		return ret;
	}

	return zend_string_init(str, size, permanent);
}

/* Copy PHP interned strings from PHP process memory into the shared memory */
static void accel_copy_permanent_strings(zend_new_interned_string_func_t new_interned_string)
{
	uint32_t j;
	Bucket *p, *q;
	HashTable *ht;

	/* empty string */
	zend_empty_string = new_interned_string(zend_empty_string);
	for (j = 0; j < 256; j++) {
		char s[2];
		s[0] = j;
		s[1] = 0;
		zend_one_char_string[j] = new_interned_string(zend_string_init(s, 1, 0));
	}
	for (j = 0; j < ZEND_STR_LAST_KNOWN; j++) {
		zend_known_strings[j] = new_interned_string(zend_known_strings[j]);
	}

	/* function table hash keys */
	ZEND_HASH_FOREACH_BUCKET(CG(function_table), p) {
		if (p->key) {
			p->key = new_interned_string(p->key);
		}
		if (Z_FUNC(p->val)->common.function_name) {
			Z_FUNC(p->val)->common.function_name = new_interned_string(Z_FUNC(p->val)->common.function_name);
		}
		if (Z_FUNC(p->val)->common.arg_info &&
		    (Z_FUNC(p->val)->common.fn_flags & (ZEND_ACC_HAS_RETURN_TYPE|ZEND_ACC_HAS_TYPE_HINTS))) {
			uint32_t i;
			uint32_t num_args = Z_FUNC(p->val)->common.num_args + 1;
			zend_arg_info *arg_info = Z_FUNC(p->val)->common.arg_info - 1;

			if (Z_FUNC(p->val)->common.fn_flags & ZEND_ACC_VARIADIC) {
				num_args++;
			}
			for (i = 0 ; i < num_args; i++) {
				if (ZEND_TYPE_IS_CLASS(arg_info[i].type)) {
					zend_bool allow_null = ZEND_TYPE_ALLOW_NULL(arg_info[i].type);
					arg_info[i].type = ZEND_TYPE_ENCODE_CLASS(new_interned_string(ZEND_TYPE_NAME(arg_info[i].type)), allow_null);
				}
			}
		}
	} ZEND_HASH_FOREACH_END();

	/* class table hash keys, class names, properties, methods, constants, etc */
	ZEND_HASH_FOREACH_BUCKET(CG(class_table), p) {
		zend_class_entry *ce;

		ce = (zend_class_entry*)Z_PTR(p->val);

		if (p->key) {
			p->key = new_interned_string(p->key);
		}

		if (ce->name) {
			ce->name = new_interned_string(ce->name);
		}

		ZEND_HASH_FOREACH_BUCKET(&ce->properties_info, q) {
			zend_property_info *info;

			info = (zend_property_info*)Z_PTR(q->val);

			if (q->key) {
				q->key = new_interned_string(q->key);
			}

			if (info->name) {
				info->name = new_interned_string(info->name);
			}
		} ZEND_HASH_FOREACH_END();

		ZEND_HASH_FOREACH_BUCKET(&ce->function_table, q) {
			if (q->key) {
				q->key = new_interned_string(q->key);
			}
			if (Z_FUNC(q->val)->common.function_name) {
				Z_FUNC(q->val)->common.function_name = new_interned_string(Z_FUNC(q->val)->common.function_name);
			}
		} ZEND_HASH_FOREACH_END();

		ZEND_HASH_FOREACH_BUCKET(&ce->constants_table, q) {
			if (q->key) {
				q->key = new_interned_string(q->key);
			}
		} ZEND_HASH_FOREACH_END();
	} ZEND_HASH_FOREACH_END();

	/* constant hash keys */
	ZEND_HASH_FOREACH_BUCKET(EG(zend_constants), p) {
		zend_constant *c;

		if (p->key) {
			p->key = new_interned_string(p->key);
		}
		c = (zend_constant*)Z_PTR(p->val);
		if (c->name) {
			c->name = new_interned_string(c->name);
		}
		if (Z_TYPE(c->value) == IS_STRING) {
			ZVAL_STR(&c->value, new_interned_string(Z_STR(c->value)));
		}
	} ZEND_HASH_FOREACH_END();

	/* auto globals hash keys and names */
	ZEND_HASH_FOREACH_BUCKET(CG(auto_globals), p) {
		zend_auto_global *auto_global;

		auto_global = (zend_auto_global*)Z_PTR(p->val);

		zend_string_addref(auto_global->name);
		auto_global->name = new_interned_string(auto_global->name);
		if (p->key) {
			p->key = new_interned_string(p->key);
		}
	} ZEND_HASH_FOREACH_END();

	ZEND_HASH_FOREACH_BUCKET(&module_registry, p) {
		if (p->key) {
			p->key = new_interned_string(p->key);
		}
	} ZEND_HASH_FOREACH_END();

	ZEND_HASH_FOREACH_BUCKET(EG(ini_directives), p) {
		zend_ini_entry *entry = (zend_ini_entry*)Z_PTR(p->val);

		if (p->key) {
			p->key = new_interned_string(p->key);
		}
		if (entry->name) {
			entry->name = new_interned_string(entry->name);
		}
		if (entry->value) {
			entry->value = new_interned_string(entry->value);
		}
		if (entry->orig_value) {
			entry->orig_value = new_interned_string(entry->orig_value);
		}
	} ZEND_HASH_FOREACH_END();

	ht = php_get_stream_filters_hash_global();
	ZEND_HASH_FOREACH_BUCKET(ht, p) {
		if (p->key) {
			p->key = new_interned_string(p->key);
		}
	} ZEND_HASH_FOREACH_END();

	ht = php_stream_get_url_stream_wrappers_hash_global();
	ZEND_HASH_FOREACH_BUCKET(ht, p) {
		if (p->key) {
			p->key = new_interned_string(p->key);
		}
	} ZEND_HASH_FOREACH_END();

	ht = php_stream_xport_get_hash();
	ZEND_HASH_FOREACH_BUCKET(ht, p) {
		if (p->key) {
			p->key = new_interned_string(p->key);
		}
	} ZEND_HASH_FOREACH_END();
}

static zend_string* ZEND_FASTCALL accel_replace_string_by_shm_permanent(zend_string *str)
{
	zend_string *ret = accel_find_interned_string(str);

	if (ret) {
		zend_string_release(str);
		return ret;
	}
	return str;
}

static zend_string* ZEND_FASTCALL accel_replace_string_by_process_permanent(zend_string *str)
{
	zend_string *ret = zend_interned_string_find_permanent(str);

	if (ret) {
		zend_string_release(str);
		return ret;
	}
	ZEND_ASSERT(0);
	return str;
}


static void accel_use_shm_interned_strings(void)
{
	HANDLE_BLOCK_INTERRUPTIONS();
	SHM_UNPROTECT();
	zend_shared_alloc_lock();

	if (ZCSG(interned_strings).saved_top == NULL) {
		accel_copy_permanent_strings(accel_new_interned_string);
	} else {
		accel_copy_permanent_strings(accel_replace_string_by_shm_permanent);
		if (ZCG(counted)) {
			accel_deactivate_sub();
		}
	}
	accel_interned_strings_save_state();

	zend_shared_alloc_unlock();
	SHM_PROTECT();
	HANDLE_UNBLOCK_INTERRUPTIONS();
}

static void accel_use_permanent_interned_strings(void)
{
	accel_copy_permanent_strings(accel_replace_string_by_process_permanent);
}

#ifndef ZEND_WIN32
static inline void kill_all_lockers(struct flock *mem_usage_check)
{
	int success, tries;
	/* so that other process won't try to force while we are busy cleaning up */
	ZCSG(force_restart_time) = 0;
	while (mem_usage_check->l_pid > 0) {
		/* Clear previous errno, reset success and tries */
		errno = 0;
		success = 0;
		tries = 10;

		while (tries--) {
			zend_accel_error(ACCEL_LOG_WARNING, "Attempting to kill locker %d", mem_usage_check->l_pid);
			if (kill(mem_usage_check->l_pid, SIGKILL)) {
				if (errno == ESRCH) {
					/* Process died before the signal was sent */
					success = 1;
					zend_accel_error(ACCEL_LOG_WARNING, "Process %d died before SIGKILL was sent", mem_usage_check->l_pid);
				}
				break;
			}
			/* give it a chance to die */
			usleep(20000);
			if (kill(mem_usage_check->l_pid, 0)) {
				if (errno == ESRCH) {
					/* successfully killed locker, process no longer exists  */
					success = 1;
					zend_accel_error(ACCEL_LOG_WARNING, "Killed locker %d", mem_usage_check->l_pid);
				}
				break;
			}
			usleep(10000);
		}
		if (!success) {
			/* errno is not ESRCH or we ran out of tries to kill the locker */
			ZCSG(force_restart_time) = time(NULL); /* restore forced restart request */
			/* cannot kill the locker, bail out with error */
			zend_accel_error(ACCEL_LOG_ERROR, "Cannot kill process %d: %s!", mem_usage_check->l_pid, strerror(errno));
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

static inline int accel_is_inactive(void)
{
#ifdef ZEND_WIN32
	if (LOCKVAL(mem_usage) == 0) {
		return SUCCESS;
	}
#else
	struct flock mem_usage_check;

	mem_usage_check.l_type = F_WRLCK;
	mem_usage_check.l_whence = SEEK_SET;
	mem_usage_check.l_start = 1;
	mem_usage_check.l_len = 1;
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
		zend_accel_error(ACCEL_LOG_WARNING, "Forced restart at %ld (after " ZEND_LONG_FMT " seconds), locked by %d", (long)time(NULL), ZCG(accel_directives).force_restart_timeout, mem_usage_check.l_pid);
		kill_all_lockers(&mem_usage_check);

		return FAILURE; /* next request should be able to restart it */
	}
#endif

	return FAILURE;
}

static int zend_get_stream_timestamp(const char *filename, zend_stat_t *statbuf)
{
	php_stream_wrapper *wrapper;
	php_stream_statbuf stream_statbuf;
	int ret, er;

	if (!filename) {
		return FAILURE;
	}

	wrapper = php_stream_locate_url_wrapper(filename, NULL, STREAM_LOCATE_WRAPPERS_ONLY);
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
		ret = wrapper->wops->url_stat(wrapper, (char*)filename, PHP_STREAM_URL_STAT_QUIET, &stream_statbuf, NULL);
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

	if (file_handle->opened_path && GetFileAttributesEx(file_handle->opened_path->val, GetFileExInfoStandard, &fdata) != 0) {
		unsigned __int64 ftime;

		if (CompareFileTime (&fdata.ftLastWriteTime, &utc_base_ft) < 0) {
			return 0;
		}

		ftime = (((unsigned __int64)fdata.ftLastWriteTime.dwHighDateTime) << 32) + fdata.ftLastWriteTime.dwLowDateTime - utc_base;
		ftime /= 10000000L;

		if (size) {
			*size = (size_t)((((unsigned __int64)fdata.nFileSizeHigh) << 32) + (unsigned __int64)fdata.nFileSizeLow);
		}
		return (accel_time_t)ftime;
	}
	return 0;
}
#endif

accel_time_t zend_get_file_handle_timestamp(zend_file_handle *file_handle, size_t *size)
{
	zend_stat_t statbuf;
#ifdef ZEND_WIN32
	accel_time_t res;
#endif

	if (sapi_module.get_stat &&
	    !EG(current_execute_data) &&
	    file_handle->filename == SG(request_info).path_translated) {

		zend_stat_t *tmpbuf = sapi_module.get_stat();

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
			if (zend_fstat(file_handle->handle.fd, &statbuf) == -1) {
				return 0;
			}
			break;
		case ZEND_HANDLE_FP:
			if (zend_fstat(fileno(file_handle->handle.fp), &statbuf) == -1) {
				if (zend_get_stream_timestamp(file_handle->filename, &statbuf) != SUCCESS) {
					return 0;
				}
			}
			break;
		case ZEND_HANDLE_FILENAME:
		case ZEND_HANDLE_MAPPED:
			if (file_handle->opened_path) {
				char *file_path = ZSTR_VAL(file_handle->opened_path);

				if (is_stream_path(file_path)) {
					if (zend_get_stream_timestamp(file_path, &statbuf) == SUCCESS) {
						break;
					}
				}
				if (VCWD_STAT(file_path, &statbuf) != -1) {
					break;
				}
			}

			if (zend_get_stream_timestamp(file_handle->filename, &statbuf) != SUCCESS) {
				return 0;
			}
			break;
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
					ret = stream->ops->stat(stream, &sb);
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

static inline int do_validate_timestamps(zend_persistent_script *persistent_script, zend_file_handle *file_handle)
{
	zend_file_handle ps_handle;
	zend_string *full_path_ptr = NULL;

	/** check that the persistent script is indeed the same file we cached
	 * (if part of the path is a symlink than it possible that the user will change it)
	 * See bug #15140
	 */
	if (file_handle->opened_path) {
		if (persistent_script->script.filename != file_handle->opened_path &&
		    !zend_string_equal_content(persistent_script->script.filename, file_handle->opened_path)) {
			return FAILURE;
		}
	} else {
		full_path_ptr = accelerator_orig_zend_resolve_path(file_handle->filename, strlen(file_handle->filename));
		if (full_path_ptr &&
		    persistent_script->script.filename != full_path_ptr &&
		    !zend_string_equal_content(persistent_script->script.filename, full_path_ptr)) {
			zend_string_release_ex(full_path_ptr, 0);
			return FAILURE;
		}
		file_handle->opened_path = full_path_ptr;
	}

	if (persistent_script->timestamp == 0) {
		if (full_path_ptr) {
			zend_string_release_ex(full_path_ptr, 0);
			file_handle->opened_path = NULL;
		}
		return FAILURE;
	}

	if (zend_get_file_handle_timestamp(file_handle, NULL) == persistent_script->timestamp) {
		if (full_path_ptr) {
			zend_string_release_ex(full_path_ptr, 0);
			file_handle->opened_path = NULL;
		}
		return SUCCESS;
	}
	if (full_path_ptr) {
		zend_string_release_ex(full_path_ptr, 0);
		file_handle->opened_path = NULL;
	}

	ps_handle.type = ZEND_HANDLE_FILENAME;
	ps_handle.filename = ZSTR_VAL(persistent_script->script.filename);
	ps_handle.opened_path = persistent_script->script.filename;

	if (zend_get_file_handle_timestamp(&ps_handle, NULL) == persistent_script->timestamp) {
		return SUCCESS;
	}

	return FAILURE;
}

int validate_timestamp_and_record(zend_persistent_script *persistent_script, zend_file_handle *file_handle)
{
	if (ZCG(accel_directives).revalidate_freq &&
	    persistent_script->dynamic_members.revalidate >= ZCG(request_time)) {
		return SUCCESS;
	} else if (do_validate_timestamps(persistent_script, file_handle) == FAILURE) {
		return FAILURE;
	} else {
		persistent_script->dynamic_members.revalidate = ZCG(request_time) + ZCG(accel_directives).revalidate_freq;
		return SUCCESS;
	}
}

int validate_timestamp_and_record_ex(zend_persistent_script *persistent_script, zend_file_handle *file_handle)
{
	int ret;

	SHM_UNPROTECT();
	ret = validate_timestamp_and_record(persistent_script, file_handle);
	SHM_PROTECT();

	return ret;
}

/* Instead of resolving full real path name each time we need to identify file,
 * we create a key that consist from requested file name, current working
 * directory, current include_path, etc */
char *accel_make_persistent_key(const char *path, size_t path_length, int *key_len)
{
	int key_length;

	/* CWD and include_path don't matter for absolute file names and streams */
    if (IS_ABSOLUTE_PATH(path, path_length)) {
		/* pass */
		ZCG(key_len) = 0;
    } else if (UNEXPECTED(is_stream_path(path))) {
		if (!is_cacheable_stream_path(path)) {
			return NULL;
		}
		/* pass */
		ZCG(key_len) = 0;
    } else if (UNEXPECTED(!ZCG(accel_directives).use_cwd)) {
		/* pass */
		ZCG(key_len) = 0;
    } else {
		const char *include_path = NULL, *cwd = NULL;
		int include_path_len = 0, cwd_len = 0;
		zend_string *parent_script = NULL;
		size_t parent_script_len = 0;

		if (EXPECTED(ZCG(cwd_key_len))) {
			cwd = ZCG(cwd_key);
			cwd_len = ZCG(cwd_key_len);
		} else {
			zend_string *cwd_str = accel_getcwd();

			if (UNEXPECTED(!cwd_str)) {
				/* we don't handle this well for now. */
				zend_accel_error(ACCEL_LOG_INFO, "getcwd() failed for '%s' (%d), please try to set opcache.use_cwd to 0 in ini file", path, errno);
				return NULL;
			}
			cwd = ZSTR_VAL(cwd_str);
			cwd_len = ZSTR_LEN(cwd_str);
			if (ZCG(cwd_check)) {
				ZCG(cwd_check) = 0;
				if ((ZCG(counted) || ZCSG(accelerator_enabled))) {

					zend_string *str = accel_find_interned_string(cwd_str);
					if (!str) {
						HANDLE_BLOCK_INTERRUPTIONS();
						SHM_UNPROTECT();
						zend_shared_alloc_lock();
						str = accel_new_interned_string(zend_string_copy(cwd_str));
						if (str == cwd_str) {
							zend_string_release_ex(str, 0);
							str = NULL;
						}
						zend_shared_alloc_unlock();
						SHM_PROTECT();
						HANDLE_UNBLOCK_INTERRUPTIONS();
					}
					if (str) {
						char buf[32];
						char *res = zend_print_long_to_buf(buf + sizeof(buf) - 1, STRTAB_STR_TO_POS(&ZCSG(interned_strings), str));

						cwd_len = ZCG(cwd_key_len) = buf + sizeof(buf) - 1 - res;
						cwd = ZCG(cwd_key);
						memcpy(ZCG(cwd_key), res, cwd_len + 1);
					}
				}
			}
		}

		if (EXPECTED(ZCG(include_path_key_len))) {
			include_path = ZCG(include_path_key);
			include_path_len = ZCG(include_path_key_len);
		} else if (!ZCG(include_path) || ZSTR_LEN(ZCG(include_path)) == 0) {
			include_path = "";
			include_path_len = 0;
		} else {
			include_path = ZSTR_VAL(ZCG(include_path));
			include_path_len = ZSTR_LEN(ZCG(include_path));

			if (ZCG(include_path_check)) {
				ZCG(include_path_check) = 0;
				if ((ZCG(counted) || ZCSG(accelerator_enabled))) {

					zend_string *str = accel_find_interned_string(ZCG(include_path));
					if (!str) {
						HANDLE_BLOCK_INTERRUPTIONS();
						SHM_UNPROTECT();
						zend_shared_alloc_lock();
						str = accel_new_interned_string(zend_string_copy(ZCG(include_path)));
						if (str == ZCG(include_path)) {
							str = NULL;
						}
						zend_shared_alloc_unlock();
						SHM_PROTECT();
						HANDLE_UNBLOCK_INTERRUPTIONS();
					}
					if (str) {
						char buf[32];
						char *res = zend_print_long_to_buf(buf + sizeof(buf) - 1, STRTAB_STR_TO_POS(&ZCSG(interned_strings), str));

						include_path_len = ZCG(include_path_key_len) = buf + sizeof(buf) - 1 - res;
						include_path = ZCG(include_path_key);
						memcpy(ZCG(include_path_key), res, include_path_len + 1);
					}
				}
			}
		}

		/* Calculate key length */
		if (UNEXPECTED((size_t)(cwd_len + path_length + include_path_len + 2) >= sizeof(ZCG(key)))) {
			return NULL;
		}

		/* Generate key
		 * Note - the include_path must be the last element in the key,
		 * since in itself, it may include colons (which we use to separate
		 * different components of the key)
		 */
		memcpy(ZCG(key), path, path_length);
		ZCG(key)[path_length] = ':';
		key_length = path_length + 1;
		memcpy(ZCG(key) + key_length, cwd, cwd_len);
		key_length += cwd_len;

		if (include_path_len) {
			ZCG(key)[key_length] = ':';
			key_length += 1;
			memcpy(ZCG(key) + key_length, include_path, include_path_len);
			key_length += include_path_len;
		}

		/* Here we add to the key the parent script directory,
		 * since fopen_wrappers from version 4.0.7 use current script's path
		 * in include path too.
		 */
		if (EXPECTED(EG(current_execute_data)) &&
		    EXPECTED((parent_script = zend_get_executed_filename_ex()) != NULL)) {

			parent_script_len = ZSTR_LEN(parent_script);
			while ((--parent_script_len > 0) && !IS_SLASH(ZSTR_VAL(parent_script)[parent_script_len]));

			if (UNEXPECTED((size_t)(key_length + parent_script_len + 1) >= sizeof(ZCG(key)))) {
				return NULL;
			}
			ZCG(key)[key_length] = ':';
			key_length += 1;
			memcpy(ZCG(key) + key_length, ZSTR_VAL(parent_script), parent_script_len);
			key_length += parent_script_len;
		}
		ZCG(key)[key_length] = '\0';
		*key_len = ZCG(key_len) = key_length;
		return ZCG(key);
	}

	/* not use_cwd */
	*key_len = path_length;
	return (char*)path;
}

int zend_accel_invalidate(const char *filename, size_t filename_len, zend_bool force)
{
	zend_string *realpath;
	zend_persistent_script *persistent_script;

	if (!ZCG(enabled) || !accel_startup_ok || !ZCSG(accelerator_enabled) || accelerator_shm_read_lock() != SUCCESS) {
		return FAILURE;
	}

	realpath = accelerator_orig_zend_resolve_path(filename, filename_len);

	if (!realpath) {
		return FAILURE;
	}

#ifdef HAVE_OPCACHE_FILE_CACHE
	if (ZCG(accel_directives).file_cache) {
		zend_file_cache_invalidate(realpath);
	}
#endif

	persistent_script = zend_accel_hash_find(&ZCSG(hash), realpath);
	if (persistent_script && !persistent_script->corrupted) {
		zend_file_handle file_handle;

		file_handle.type = ZEND_HANDLE_FILENAME;
		file_handle.filename = ZSTR_VAL(realpath);
		file_handle.opened_path = realpath;

		if (force ||
			!ZCG(accel_directives).validate_timestamps ||
			do_validate_timestamps(persistent_script, &file_handle) == FAILURE) {
			HANDLE_BLOCK_INTERRUPTIONS();
			SHM_UNPROTECT();
			zend_shared_alloc_lock();
			if (!persistent_script->corrupted) {
				persistent_script->corrupted = 1;
				persistent_script->timestamp = 0;
				ZSMMG(wasted_shared_memory) += persistent_script->dynamic_members.memory_consumption;
				if (ZSMMG(memory_exhausted)) {
					zend_accel_restart_reason reason =
						zend_accel_hash_is_full(&ZCSG(hash)) ? ACCEL_RESTART_HASH : ACCEL_RESTART_OOM;
					zend_accel_schedule_restart_if_necessary(reason);
				}
			}
			zend_shared_alloc_unlock();
			SHM_PROTECT();
			HANDLE_UNBLOCK_INTERRUPTIONS();
		}
	}

	accelerator_shm_read_unlock();
	zend_string_release_ex(realpath, 0);

	return SUCCESS;
}

/* Adds another key for existing cached script */
static void zend_accel_add_key(const char *key, unsigned int key_length, zend_accel_hash_entry *bucket)
{
	if (!zend_accel_hash_str_find(&ZCSG(hash), key, key_length)) {
		if (zend_accel_hash_is_full(&ZCSG(hash))) {
			zend_accel_error(ACCEL_LOG_DEBUG, "No more entries in hash table!");
			ZSMMG(memory_exhausted) = 1;
			zend_accel_schedule_restart_if_necessary(ACCEL_RESTART_HASH);
		} else {
			char *new_key = zend_shared_alloc(key_length + 1);
			if (new_key) {
				memcpy(new_key, key, key_length + 1);
				if (zend_accel_hash_update(&ZCSG(hash), new_key, key_length, 1, bucket)) {
					zend_accel_error(ACCEL_LOG_INFO, "Added key '%s'", new_key);
				}
			} else {
				zend_accel_schedule_restart_if_necessary(ACCEL_RESTART_OOM);
			}
		}
	}
}

static zend_always_inline zend_bool is_phar_file(zend_string *filename)
{
	return filename && ZSTR_LEN(filename) >= sizeof(".phar") &&
		!memcmp(ZSTR_VAL(filename) + ZSTR_LEN(filename) - (sizeof(".phar")-1), ".phar", sizeof(".phar")-1) &&
		!strstr(ZSTR_VAL(filename), "://");
}

#ifdef HAVE_OPCACHE_FILE_CACHE
static zend_persistent_script *store_script_in_file_cache(zend_persistent_script *new_persistent_script)
{
	uint32_t memory_used;

	zend_shared_alloc_init_xlat_table();

	/* Calculate the required memory size */
	memory_used = zend_accel_script_persist_calc(new_persistent_script, NULL, 0, 0);

	/* Allocate memory block */
#if defined(__AVX__) || defined(__SSE2__)
	/* Align to 64-byte boundary */
	ZCG(mem) = zend_arena_alloc(&CG(arena), memory_used + 64);
	ZCG(mem) = (void*)(((zend_uintptr_t)ZCG(mem) + 63L) & ~63L);
#else
	ZCG(mem) = zend_arena_alloc(&CG(arena), memory_used);
#endif

	/* Copy into memory block */
	new_persistent_script = zend_accel_script_persist(new_persistent_script, NULL, 0, 0);

	zend_shared_alloc_destroy_xlat_table();

	new_persistent_script->is_phar = is_phar_file(new_persistent_script->script.filename);

	/* Consistency check */
	if ((char*)new_persistent_script->mem + new_persistent_script->size != (char*)ZCG(mem)) {
		zend_accel_error(
			((char*)new_persistent_script->mem + new_persistent_script->size < (char*)ZCG(mem)) ? ACCEL_LOG_ERROR : ACCEL_LOG_WARNING,
			"Internal error: wrong size calculation: %s start=" ZEND_ADDR_FMT ", end=" ZEND_ADDR_FMT ", real=" ZEND_ADDR_FMT "\n",
			ZSTR_VAL(new_persistent_script->script.filename),
			(size_t)new_persistent_script->mem,
			(size_t)((char *)new_persistent_script->mem + new_persistent_script->size),
			(size_t)ZCG(mem));
	}

	new_persistent_script->dynamic_members.checksum = zend_accel_script_checksum(new_persistent_script);

	zend_file_cache_script_store(new_persistent_script, 0);

	return new_persistent_script;
}

static zend_persistent_script *cache_script_in_file_cache(zend_persistent_script *new_persistent_script, int *from_shared_memory)
{
	uint32_t orig_compiler_options;

	/* Check if script may be stored in shared memory */
	if (!zend_accel_script_persistable(new_persistent_script)) {
		return new_persistent_script;
	}

	orig_compiler_options = CG(compiler_options);
	CG(compiler_options) |= ZEND_COMPILE_WITH_FILE_CACHE;
	if (!zend_optimize_script(&new_persistent_script->script, ZCG(accel_directives).optimization_level, ZCG(accel_directives).opt_debug_level)) {
		CG(compiler_options) = orig_compiler_options;
		return new_persistent_script;
	}
	CG(compiler_options) = orig_compiler_options;

	*from_shared_memory = 1;
	return store_script_in_file_cache(new_persistent_script);
}
#endif

static zend_persistent_script *cache_script_in_shared_memory(zend_persistent_script *new_persistent_script, const char *key, unsigned int key_length, int *from_shared_memory)
{
	zend_accel_hash_entry *bucket;
	uint32_t memory_used;
	uint32_t orig_compiler_options;

	/* Check if script may be stored in shared memory */
	if (!zend_accel_script_persistable(new_persistent_script)) {
		return new_persistent_script;
	}

	orig_compiler_options = CG(compiler_options);
#ifdef HAVE_OPCACHE_FILE_CACHE
	if (ZCG(accel_directives).file_cache) {
		CG(compiler_options) |= ZEND_COMPILE_WITH_FILE_CACHE;
	}
#endif
	if (!zend_optimize_script(&new_persistent_script->script, ZCG(accel_directives).optimization_level, ZCG(accel_directives).opt_debug_level)) {
		CG(compiler_options) = orig_compiler_options;
		return new_persistent_script;
	}
	CG(compiler_options) = orig_compiler_options;

	/* exclusive lock */
	zend_shared_alloc_lock();

	/* Check if we still need to put the file into the cache (may be it was
	 * already stored by another process. This final check is done under
	 * exclusive lock) */
	bucket = zend_accel_hash_find_entry(&ZCSG(hash), new_persistent_script->script.filename);
	if (bucket) {
		zend_persistent_script *existing_persistent_script = (zend_persistent_script *)bucket->data;

		if (!existing_persistent_script->corrupted) {
			if (key &&
			    (!ZCG(accel_directives).validate_timestamps ||
			     (new_persistent_script->timestamp == existing_persistent_script->timestamp))) {
				zend_accel_add_key(key, key_length, bucket);
			}
			zend_shared_alloc_unlock();
			return new_persistent_script;
		}
	}

	if (zend_accel_hash_is_full(&ZCSG(hash))) {
		zend_accel_error(ACCEL_LOG_DEBUG, "No more entries in hash table!");
		ZSMMG(memory_exhausted) = 1;
		zend_accel_schedule_restart_if_necessary(ACCEL_RESTART_HASH);
		zend_shared_alloc_unlock();
#ifdef HAVE_OPCACHE_FILE_CACHE
		if (ZCG(accel_directives).file_cache) {
			new_persistent_script = store_script_in_file_cache(new_persistent_script);
			*from_shared_memory = 1;
		}
#endif
		return new_persistent_script;
	}

	zend_shared_alloc_init_xlat_table();

	/* Calculate the required memory size */
	memory_used = zend_accel_script_persist_calc(new_persistent_script, key, key_length, 1);

	/* Allocate shared memory */
#if defined(__AVX__) || defined(__SSE2__)
	/* Align to 64-byte boundary */
	ZCG(mem) = zend_shared_alloc(memory_used + 64);
	if (ZCG(mem)) {
		ZCG(mem) = (void*)(((zend_uintptr_t)ZCG(mem) + 63L) & ~63L);
#if defined(__x86_64__)
		memset(ZCG(mem), 0, memory_used);
#elif defined(__AVX__)
		{
			char *p = (char*)ZCG(mem);
			char *end = p + memory_used;
			__m256i ymm0 = _mm256_setzero_si256();

			while (p < end) {
				_mm256_store_si256((__m256i*)p, ymm0);
				_mm256_store_si256((__m256i*)(p+32), ymm0);
				p += 64;
			}
		}
#else
		{
			char *p = (char*)ZCG(mem);
			char *end = p + memory_used;
			__m128i xmm0 = _mm_setzero_si128();

			while (p < end) {
				_mm_store_si128((__m128i*)p, xmm0);
				_mm_store_si128((__m128i*)(p+16), xmm0);
				_mm_store_si128((__m128i*)(p+32), xmm0);
				_mm_store_si128((__m128i*)(p+48), xmm0);
				p += 64;
			}
		}
#endif
	}
#else
	ZCG(mem) = zend_shared_alloc(memory_used);
	if (ZCG(mem)) {
		memset(ZCG(mem), 0, memory_used);
	}
#endif
	if (!ZCG(mem)) {
		zend_shared_alloc_destroy_xlat_table();
		zend_accel_schedule_restart_if_necessary(ACCEL_RESTART_OOM);
		zend_shared_alloc_unlock();
#ifdef HAVE_OPCACHE_FILE_CACHE
		if (ZCG(accel_directives).file_cache) {
			new_persistent_script = store_script_in_file_cache(new_persistent_script);
			*from_shared_memory = 1;
		}
#endif
		return new_persistent_script;
	}

	/* Copy into shared memory */
	new_persistent_script = zend_accel_script_persist(new_persistent_script, &key, key_length, 1);

	zend_shared_alloc_destroy_xlat_table();

	new_persistent_script->is_phar = is_phar_file(new_persistent_script->script.filename);

	/* Consistency check */
	if ((char*)new_persistent_script->mem + new_persistent_script->size != (char*)ZCG(mem)) {
		zend_accel_error(
			((char*)new_persistent_script->mem + new_persistent_script->size < (char*)ZCG(mem)) ? ACCEL_LOG_ERROR : ACCEL_LOG_WARNING,
			"Internal error: wrong size calculation: %s start=" ZEND_ADDR_FMT ", end=" ZEND_ADDR_FMT ", real=" ZEND_ADDR_FMT "\n",
			ZSTR_VAL(new_persistent_script->script.filename),
			(size_t)new_persistent_script->mem,
			(size_t)((char *)new_persistent_script->mem + new_persistent_script->size),
			(size_t)ZCG(mem));
	}

	new_persistent_script->dynamic_members.checksum = zend_accel_script_checksum(new_persistent_script);

	/* store script structure in the hash table */
	bucket = zend_accel_hash_update(&ZCSG(hash), ZSTR_VAL(new_persistent_script->script.filename), ZSTR_LEN(new_persistent_script->script.filename), 0, new_persistent_script);
	if (bucket) {
		zend_accel_error(ACCEL_LOG_INFO, "Cached script '%s'", ZSTR_VAL(new_persistent_script->script.filename));
		if (key &&
		    /* key may contain non-persistent PHAR aliases (see issues #115 and #149) */
		    memcmp(key, "phar://", sizeof("phar://") - 1) != 0 &&
		    (ZSTR_LEN(new_persistent_script->script.filename) != key_length ||
		     memcmp(ZSTR_VAL(new_persistent_script->script.filename), key, key_length) != 0)) {
			/* link key to the same persistent script in hash table */
			if (zend_accel_hash_update(&ZCSG(hash), key, key_length, 1, bucket)) {
				zend_accel_error(ACCEL_LOG_INFO, "Added key '%s'", key);
			} else {
				zend_accel_error(ACCEL_LOG_DEBUG, "No more entries in hash table!");
				ZSMMG(memory_exhausted) = 1;
				zend_accel_schedule_restart_if_necessary(ACCEL_RESTART_HASH);
			}
		}
	}

	new_persistent_script->dynamic_members.memory_consumption = ZEND_ALIGNED_SIZE(new_persistent_script->size);

	zend_shared_alloc_unlock();

#ifdef HAVE_OPCACHE_FILE_CACHE
	if (ZCG(accel_directives).file_cache) {
		SHM_PROTECT();
		zend_file_cache_script_store(new_persistent_script, 1);
		SHM_UNPROTECT();
	}
#endif

	*from_shared_memory = 1;
	return new_persistent_script;
}

static const struct jit_auto_global_info
{
    const char *name;
    size_t len;
} jit_auto_globals_info[] = {
    { "_SERVER",  sizeof("_SERVER")-1},
    { "_ENV",     sizeof("_ENV")-1},
    { "_REQUEST", sizeof("_REQUEST")-1},
    { "GLOBALS",  sizeof("GLOBALS")-1},
};

static zend_string *jit_auto_globals_str[4];

static int zend_accel_get_auto_globals(void)
{
	int i, ag_size = (sizeof(jit_auto_globals_info) / sizeof(jit_auto_globals_info[0]));
	int n = 1;
	int mask = 0;

	for (i = 0; i < ag_size ; i++) {
		if (zend_hash_exists(&EG(symbol_table), jit_auto_globals_str[i])) {
			mask |= n;
		}
		n += n;
	}
	return mask;
}

static int zend_accel_get_auto_globals_no_jit(void)
{
	if (zend_hash_exists(&EG(symbol_table), jit_auto_globals_str[3])) {
		return 8;
	}
	return 0;
}

static void zend_accel_set_auto_globals(int mask)
{
	int i, ag_size = (sizeof(jit_auto_globals_info) / sizeof(jit_auto_globals_info[0]));
	int n = 1;

	for (i = 0; i < ag_size ; i++) {
		if ((mask & n) && !(ZCG(auto_globals_mask) & n)) {
			ZCG(auto_globals_mask) |= n;
			zend_is_auto_global(jit_auto_globals_str[i]);
		}
		n += n;
	}
}

static void zend_accel_init_auto_globals(void)
{
	int i, ag_size = (sizeof(jit_auto_globals_info) / sizeof(jit_auto_globals_info[0]));

	for (i = 0; i < ag_size ; i++) {
		jit_auto_globals_str[i] = zend_string_init(jit_auto_globals_info[i].name, jit_auto_globals_info[i].len, 1);
		zend_string_hash_val(jit_auto_globals_str[i]);
		jit_auto_globals_str[i] = accel_new_interned_string(jit_auto_globals_str[i]);
	}
}

static zend_persistent_script *opcache_compile_file(zend_file_handle *file_handle, int type, const char *key, zend_op_array **op_array_p)
{
	zend_persistent_script *new_persistent_script;
	zend_op_array *orig_active_op_array;
	HashTable *orig_function_table, *orig_class_table;
	zval orig_user_error_handler;
	zend_op_array *op_array;
	int do_bailout = 0;
	accel_time_t timestamp = 0;
	uint32_t orig_compiler_options = 0;

    /* Try to open file */
    if (file_handle->type == ZEND_HANDLE_FILENAME) {
        if (accelerator_orig_zend_stream_open_function(file_handle->filename, file_handle) != SUCCESS) {
			*op_array_p = NULL;
			if (type == ZEND_REQUIRE) {
				zend_message_dispatcher(ZMSG_FAILED_REQUIRE_FOPEN, file_handle->filename);
				zend_bailout();
			} else {
				zend_message_dispatcher(ZMSG_FAILED_INCLUDE_FOPEN, file_handle->filename);
			}
			return NULL;
    	}
    }

	/* check blacklist right after ensuring that file was opened */
	if (file_handle->opened_path && zend_accel_blacklist_is_blacklisted(&accel_blacklist, ZSTR_VAL(file_handle->opened_path), ZSTR_LEN(file_handle->opened_path))) {
		ZCSG(blacklist_misses)++;
		*op_array_p = accelerator_orig_compile_file(file_handle, type);
		return NULL;
	}

	if (ZCG(accel_directives).validate_timestamps ||
	    ZCG(accel_directives).file_update_protection ||
	    ZCG(accel_directives).max_file_size > 0) {
		size_t size = 0;

		/* Obtain the file timestamps, *before* actually compiling them,
		 * otherwise we have a race-condition.
		 */
		timestamp = zend_get_file_handle_timestamp(file_handle, ZCG(accel_directives).max_file_size > 0 ? &size : NULL);

		/* If we can't obtain a timestamp (that means file is possibly socket)
		 *  we won't cache it
		 */
		if (timestamp == 0) {
			*op_array_p = accelerator_orig_compile_file(file_handle, type);
			return NULL;
		}

		/* check if file is too new (may be it's not written completely yet) */
		if (ZCG(accel_directives).file_update_protection &&
		    ((accel_time_t)(ZCG(request_time) - ZCG(accel_directives).file_update_protection) < timestamp)) {
			*op_array_p = accelerator_orig_compile_file(file_handle, type);
			return NULL;
		}

		if (ZCG(accel_directives).max_file_size > 0 && size > (size_t)ZCG(accel_directives).max_file_size) {
			ZCSG(blacklist_misses)++;
			*op_array_p = accelerator_orig_compile_file(file_handle, type);
			return NULL;
		}
	}

	new_persistent_script = create_persistent_script();

	/* Save the original values for the op_array, function table and class table */
	orig_active_op_array = CG(active_op_array);
	orig_function_table = CG(function_table);
	orig_class_table = CG(class_table);
	ZVAL_COPY_VALUE(&orig_user_error_handler, &EG(user_error_handler));

	/* Override them with ours */
	CG(function_table) = &ZCG(function_table);
	EG(class_table) = CG(class_table) = &new_persistent_script->script.class_table;
	ZVAL_UNDEF(&EG(user_error_handler));

	zend_try {
		orig_compiler_options = CG(compiler_options);
		CG(compiler_options) |= ZEND_COMPILE_HANDLE_OP_ARRAY;
		CG(compiler_options) |= ZEND_COMPILE_IGNORE_INTERNAL_CLASSES;
		CG(compiler_options) |= ZEND_COMPILE_DELAYED_BINDING;
		CG(compiler_options) |= ZEND_COMPILE_NO_CONSTANT_SUBSTITUTION;
#ifdef HAVE_OPCACHE_FILE_CACHE
		if (ZCG(accel_directives).file_cache) {
			CG(compiler_options) |= ZEND_COMPILE_WITH_FILE_CACHE;
		}
#endif
		op_array = *op_array_p = accelerator_orig_compile_file(file_handle, type);
		CG(compiler_options) = orig_compiler_options;
	} zend_catch {
		op_array = NULL;
		do_bailout = 1;
		CG(compiler_options) = orig_compiler_options;
	} zend_end_try();

	/* Restore originals */
	CG(active_op_array) = orig_active_op_array;
	CG(function_table) = orig_function_table;
	EG(class_table) = CG(class_table) = orig_class_table;
	EG(user_error_handler) = orig_user_error_handler;

	if (!op_array) {
		/* compilation failed */
		free_persistent_script(new_persistent_script, 1);
		zend_accel_free_user_functions(&ZCG(function_table));
		if (do_bailout) {
			zend_bailout();
		}
		return NULL;
	}

	/* Build the persistent_script structure.
	   Here we aren't sure we would store it, but we will need it
	   further anyway.
	*/
	zend_accel_move_user_functions(&ZCG(function_table), &new_persistent_script->script.function_table);
	new_persistent_script->script.first_early_binding_opline =
		(op_array->fn_flags & ZEND_ACC_EARLY_BINDING) ?
			zend_build_delayed_early_binding_list(op_array) :
			(uint32_t)-1;
	new_persistent_script->script.main_op_array = *op_array;

	efree(op_array); /* we have valid persistent_script, so it's safe to free op_array */

    /* Fill in the ping_auto_globals_mask for the new script. If jit for auto globals is enabled we
       will have to ping the used auto global variables before execution */
	if (PG(auto_globals_jit)) {
		new_persistent_script->ping_auto_globals_mask = zend_accel_get_auto_globals();
	} else {
		new_persistent_script->ping_auto_globals_mask = zend_accel_get_auto_globals_no_jit();
	}

	if (ZCG(accel_directives).validate_timestamps) {
		/* Obtain the file timestamps, *before* actually compiling them,
		 * otherwise we have a race-condition.
		 */
		new_persistent_script->timestamp = timestamp;
		new_persistent_script->dynamic_members.revalidate = ZCG(request_time) + ZCG(accel_directives).revalidate_freq;
	}

	if (file_handle->opened_path) {
		new_persistent_script->script.filename = zend_string_copy(file_handle->opened_path);
	} else {
		new_persistent_script->script.filename = zend_string_init(file_handle->filename, strlen(file_handle->filename), 0);
	}
	zend_string_hash_val(new_persistent_script->script.filename);

	/* Now persistent_script structure is ready in process memory */
	return new_persistent_script;
}

#ifdef HAVE_OPCACHE_FILE_CACHE
zend_op_array *file_cache_compile_file(zend_file_handle *file_handle, int type)
{
	zend_persistent_script *persistent_script;
	zend_op_array *op_array = NULL;
	int from_memory; /* if the script we've got is stored in SHM */

	if (is_stream_path(file_handle->filename) &&
	    !is_cacheable_stream_path(file_handle->filename)) {
		return accelerator_orig_compile_file(file_handle, type);
	}

	if (!file_handle->opened_path) {
		if (file_handle->type == ZEND_HANDLE_FILENAME &&
		    accelerator_orig_zend_stream_open_function(file_handle->filename, file_handle) == FAILURE) {
			if (type == ZEND_REQUIRE) {
				zend_message_dispatcher(ZMSG_FAILED_REQUIRE_FOPEN, file_handle->filename);
				zend_bailout();
			} else {
				zend_message_dispatcher(ZMSG_FAILED_INCLUDE_FOPEN, file_handle->filename);
			}
			return NULL;
	    }
	}

	HANDLE_BLOCK_INTERRUPTIONS();
	SHM_UNPROTECT();
	persistent_script = zend_file_cache_script_load(file_handle);
	SHM_PROTECT();
	HANDLE_UNBLOCK_INTERRUPTIONS();
	if (persistent_script) {
		/* see bug #15471 (old BTS) */
		if (persistent_script->script.filename) {
			if (!EG(current_execute_data) || !EG(current_execute_data)->opline ||
			    !EG(current_execute_data)->func ||
			    !ZEND_USER_CODE(EG(current_execute_data)->func->common.type) ||
			    EG(current_execute_data)->opline->opcode != ZEND_INCLUDE_OR_EVAL ||
			    (EG(current_execute_data)->opline->extended_value != ZEND_INCLUDE_ONCE &&
			     EG(current_execute_data)->opline->extended_value != ZEND_REQUIRE_ONCE)) {
				if (zend_hash_add_empty_element(&EG(included_files), persistent_script->script.filename) != NULL) {
					/* ext/phar has to load phar's metadata into memory */
					if (persistent_script->is_phar) {
						php_stream_statbuf ssb;
						char *fname = emalloc(sizeof("phar://") + ZSTR_LEN(persistent_script->script.filename));

						memcpy(fname, "phar://", sizeof("phar://") - 1);
						memcpy(fname + sizeof("phar://") - 1, ZSTR_VAL(persistent_script->script.filename), ZSTR_LEN(persistent_script->script.filename) + 1);
						php_stream_stat_path(fname, &ssb);
						efree(fname);
					}
				}
			}
		}
		zend_file_handle_dtor(file_handle);

	    if (persistent_script->ping_auto_globals_mask) {
			zend_accel_set_auto_globals(persistent_script->ping_auto_globals_mask);
		}

		return zend_accel_load_script(persistent_script, 1);
	}

	persistent_script = opcache_compile_file(file_handle, type, NULL, &op_array);

	if (persistent_script) {
		from_memory = 0;
		persistent_script = cache_script_in_file_cache(persistent_script, &from_memory);
		return zend_accel_load_script(persistent_script, from_memory);
	}

	return op_array;
}
#endif

/* zend_compile() replacement */
zend_op_array *persistent_compile_file(zend_file_handle *file_handle, int type)
{
	zend_persistent_script *persistent_script = NULL;
	char *key = NULL;
	int key_length;
	int from_shared_memory; /* if the script we've got is stored in SHM */

	if (!file_handle->filename || !ZCG(enabled) || !accel_startup_ok) {
		/* The Accelerator is disabled, act as if without the Accelerator */
		return accelerator_orig_compile_file(file_handle, type);
#ifdef HAVE_OPCACHE_FILE_CACHE
	} else if (file_cache_only) {
		return file_cache_compile_file(file_handle, type);
#endif
	} else if ((!ZCG(counted) && !ZCSG(accelerator_enabled)) ||
	           (ZCSG(restart_in_progress) && accel_restart_is_active())) {
#ifdef HAVE_OPCACHE_FILE_CACHE
		if (ZCG(accel_directives).file_cache) {
			return file_cache_compile_file(file_handle, type);
		}
#endif
		return accelerator_orig_compile_file(file_handle, type);
	}

	/* In case this callback is called from include_once, require_once or it's
	 * a main FastCGI request, the key must be already calculated, and cached
	 * persistent script already found */
	if (ZCG(cache_persistent_script) &&
	    ((!EG(current_execute_data) &&
	      file_handle->filename == SG(request_info).path_translated &&
	      ZCG(cache_opline) == NULL) ||
	     (EG(current_execute_data) &&
	      EG(current_execute_data)->func &&
	      ZEND_USER_CODE(EG(current_execute_data)->func->common.type) &&
	      ZCG(cache_opline) == EG(current_execute_data)->opline))) {

		persistent_script = ZCG(cache_persistent_script);
		if (ZCG(key_len)) {
			key = ZCG(key);
			key_length = ZCG(key_len);
		}

	} else {
		if (!ZCG(accel_directives).revalidate_path) {
			/* try to find cached script by key */
			key = accel_make_persistent_key(file_handle->filename, strlen(file_handle->filename), &key_length);
			if (!key) {
				return accelerator_orig_compile_file(file_handle, type);
			}
			persistent_script = zend_accel_hash_str_find(&ZCSG(hash), key, key_length);
		} else if (UNEXPECTED(is_stream_path(file_handle->filename) && !is_cacheable_stream_path(file_handle->filename))) {
			return accelerator_orig_compile_file(file_handle, type);
		}

		if (!persistent_script) {
			/* try to find cached script by full real path */
			zend_accel_hash_entry *bucket;

			/* open file to resolve the path */
		    if (file_handle->type == ZEND_HANDLE_FILENAME &&
        		accelerator_orig_zend_stream_open_function(file_handle->filename, file_handle) == FAILURE) {
				if (type == ZEND_REQUIRE) {
					zend_message_dispatcher(ZMSG_FAILED_REQUIRE_FOPEN, file_handle->filename);
					zend_bailout();
				} else {
					zend_message_dispatcher(ZMSG_FAILED_INCLUDE_FOPEN, file_handle->filename);
				}
				return NULL;
		    }

			if (file_handle->opened_path) {
				bucket = zend_accel_hash_find_entry(&ZCSG(hash), file_handle->opened_path);

				if (bucket) {
					persistent_script = (zend_persistent_script *)bucket->data;

					if (key && !persistent_script->corrupted) {
						HANDLE_BLOCK_INTERRUPTIONS();
						SHM_UNPROTECT();
						zend_shared_alloc_lock();
						zend_accel_add_key(key, key_length, bucket);
						zend_shared_alloc_unlock();
						SHM_PROTECT();
						HANDLE_UNBLOCK_INTERRUPTIONS();
					}
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

	/* Make sure we only increase the currently running processes semaphore
     * once each execution (this function can be called more than once on
     * each execution)
     */
	if (!ZCG(counted)) {
		if (accel_activate_add() == FAILURE) {
#ifdef HAVE_OPCACHE_FILE_CACHE
			if (ZCG(accel_directives).file_cache) {
				return file_cache_compile_file(file_handle, type);
			}
#endif
			return accelerator_orig_compile_file(file_handle, type);
		}
		ZCG(counted) = 1;
	}

	/* Revalidate acessibility of cached file */
	if (EXPECTED(persistent_script != NULL) &&
	    UNEXPECTED(ZCG(accel_directives).validate_permission) &&
	    file_handle->type == ZEND_HANDLE_FILENAME &&
	    UNEXPECTED(access(ZSTR_VAL(persistent_script->script.filename), R_OK) != 0)) {
		if (type == ZEND_REQUIRE) {
			zend_message_dispatcher(ZMSG_FAILED_REQUIRE_FOPEN, file_handle->filename);
			zend_bailout();
		} else {
			zend_message_dispatcher(ZMSG_FAILED_INCLUDE_FOPEN, file_handle->filename);
		}
		return NULL;
	}

	HANDLE_BLOCK_INTERRUPTIONS();
	SHM_UNPROTECT();

	/* If script is found then validate_timestamps if option is enabled */
	if (persistent_script && ZCG(accel_directives).validate_timestamps) {
		if (validate_timestamp_and_record(persistent_script, file_handle) == FAILURE) {
			zend_shared_alloc_lock();
			if (!persistent_script->corrupted) {
				persistent_script->corrupted = 1;
				persistent_script->timestamp = 0;
				ZSMMG(wasted_shared_memory) += persistent_script->dynamic_members.memory_consumption;
				if (ZSMMG(memory_exhausted)) {
					zend_accel_restart_reason reason =
						zend_accel_hash_is_full(&ZCSG(hash)) ? ACCEL_RESTART_HASH : ACCEL_RESTART_OOM;
					zend_accel_schedule_restart_if_necessary(reason);
				}
			}
			zend_shared_alloc_unlock();
			persistent_script = NULL;
		}
	}

	/* if turned on - check the compiled script ADLER32 checksum */
	if (persistent_script && ZCG(accel_directives).consistency_checks
		&& persistent_script->dynamic_members.hits % ZCG(accel_directives).consistency_checks == 0) {

		unsigned int checksum = zend_accel_script_checksum(persistent_script);
		if (checksum != persistent_script->dynamic_members.checksum ) {
			/* The checksum is wrong */
			zend_accel_error(ACCEL_LOG_INFO, "Checksum failed for '%s':  expected=0x%08x, found=0x%08x",
							 ZSTR_VAL(persistent_script->script.filename), persistent_script->dynamic_members.checksum, checksum);
			zend_shared_alloc_lock();
			if (!persistent_script->corrupted) {
				persistent_script->corrupted = 1;
				persistent_script->timestamp = 0;
				ZSMMG(wasted_shared_memory) += persistent_script->dynamic_members.memory_consumption;
				if (ZSMMG(memory_exhausted)) {
					zend_accel_restart_reason reason =
						zend_accel_hash_is_full(&ZCSG(hash)) ? ACCEL_RESTART_HASH : ACCEL_RESTART_OOM;
					zend_accel_schedule_restart_if_necessary(reason);
				}
			}
			zend_shared_alloc_unlock();
			persistent_script = NULL;
		}
	}

#ifdef HAVE_OPCACHE_FILE_CACHE
	/* Check the second level cache */
	if (!persistent_script && ZCG(accel_directives).file_cache) {
		persistent_script = zend_file_cache_script_load(file_handle);
	}
#endif

	/* If script was not found or invalidated by validate_timestamps */
	if (!persistent_script) {
		uint32_t old_const_num = zend_hash_next_free_element(EG(zend_constants));
		zend_op_array *op_array;

		/* Cache miss.. */
		ZCSG(misses)++;

		/* No memory left. Behave like without the Accelerator */
		if (ZSMMG(memory_exhausted) || ZCSG(restart_pending)) {
			SHM_PROTECT();
			HANDLE_UNBLOCK_INTERRUPTIONS();
#ifdef HAVE_OPCACHE_FILE_CACHE
			if (ZCG(accel_directives).file_cache) {
				return file_cache_compile_file(file_handle, type);
			}
#endif
			return accelerator_orig_compile_file(file_handle, type);
		}

		/* Try and cache the script and assume that it is returned from_shared_memory.
         * If it isn't compile_and_cache_file() changes the flag to 0
         */
       	from_shared_memory = 0;
		persistent_script = opcache_compile_file(file_handle, type, key, &op_array);
		if (persistent_script) {
			persistent_script = cache_script_in_shared_memory(persistent_script, key, key ? key_length : 0, &from_shared_memory);
		}

		/* Caching is disabled, returning op_array;
		 * or something went wrong during compilation, returning NULL
		 */
		if (!persistent_script) {
			SHM_PROTECT();
			HANDLE_UNBLOCK_INTERRUPTIONS();
			return op_array;
		}
		if (from_shared_memory) {
			/* Delete immutable arrays moved into SHM */
			uint32_t new_const_num = zend_hash_next_free_element(EG(zend_constants));
			while (new_const_num > old_const_num) {
				new_const_num--;
				zend_hash_index_del(EG(zend_constants), new_const_num);
			}
		}
	} else {

#if !ZEND_WIN32
		ZCSG(hits)++; /* TBFixed: may lose one hit */
		persistent_script->dynamic_members.hits++; /* see above */
#else
#ifdef _M_X64
		InterlockedIncrement64(&ZCSG(hits));
#else
		InterlockedIncrement(&ZCSG(hits));
#endif
		InterlockedIncrement64(&persistent_script->dynamic_members.hits);
#endif

		/* see bug #15471 (old BTS) */
		if (persistent_script->script.filename) {
			if (!EG(current_execute_data) || !EG(current_execute_data)->opline ||
			    !EG(current_execute_data)->func ||
			    !ZEND_USER_CODE(EG(current_execute_data)->func->common.type) ||
			    EG(current_execute_data)->opline->opcode != ZEND_INCLUDE_OR_EVAL ||
			    (EG(current_execute_data)->opline->extended_value != ZEND_INCLUDE_ONCE &&
			     EG(current_execute_data)->opline->extended_value != ZEND_REQUIRE_ONCE)) {
				if (zend_hash_add_empty_element(&EG(included_files), persistent_script->script.filename) != NULL) {
					/* ext/phar has to load phar's metadata into memory */
					if (persistent_script->is_phar) {
						php_stream_statbuf ssb;
						char *fname = emalloc(sizeof("phar://") + ZSTR_LEN(persistent_script->script.filename));

						memcpy(fname, "phar://", sizeof("phar://") - 1);
						memcpy(fname + sizeof("phar://") - 1, ZSTR_VAL(persistent_script->script.filename), ZSTR_LEN(persistent_script->script.filename) + 1);
						php_stream_stat_path(fname, &ssb);
						efree(fname);
					}
				}
			}
		}
		zend_file_handle_dtor(file_handle);
		from_shared_memory = 1;
	}

	persistent_script->dynamic_members.last_used = ZCG(request_time);

	SHM_PROTECT();
	HANDLE_UNBLOCK_INTERRUPTIONS();

    /* Fetch jit auto globals used in the script before execution */
    if (persistent_script->ping_auto_globals_mask) {
		zend_accel_set_auto_globals(persistent_script->ping_auto_globals_mask);
	}

	return zend_accel_load_script(persistent_script, from_shared_memory);
}

/* zend_stream_open_function() replacement for PHP 5.3 and above */
static int persistent_stream_open_function(const char *filename, zend_file_handle *handle)
{
	if (ZCG(cache_persistent_script)) {
		/* check if callback is called from include_once or it's a main request */
		if ((!EG(current_execute_data) &&
		     filename == SG(request_info).path_translated &&
		     ZCG(cache_opline) == NULL) ||
		    (EG(current_execute_data) &&
		     EG(current_execute_data)->func &&
		     ZEND_USER_CODE(EG(current_execute_data)->func->common.type) &&
		     ZCG(cache_opline) == EG(current_execute_data)->opline)) {

			/* we are in include_once or FastCGI request */
			handle->filename = (char*)filename;
			handle->free_filename = 0;
			handle->opened_path = zend_string_copy(ZCG(cache_persistent_script)->script.filename);
			handle->type = ZEND_HANDLE_FILENAME;
			return SUCCESS;
		}
		ZCG(cache_opline) = NULL;
		ZCG(cache_persistent_script) = NULL;
	}
	return accelerator_orig_zend_stream_open_function(filename, handle);
}

/* zend_resolve_path() replacement for PHP 5.3 and above */
static zend_string* persistent_zend_resolve_path(const char *filename, size_t filename_len)
{
	if (ZCG(enabled) && accel_startup_ok &&
#ifdef HAVE_OPCACHE_FILE_CACHE
		!file_cache_only &&
#endif
	    (ZCG(counted) || ZCSG(accelerator_enabled)) &&
	    !ZCSG(restart_in_progress)) {

		/* check if callback is called from include_once or it's a main request */
		if ((!EG(current_execute_data) &&
		     filename == SG(request_info).path_translated) ||
		    (EG(current_execute_data) &&
		     EG(current_execute_data)->func &&
		     ZEND_USER_CODE(EG(current_execute_data)->func->common.type) &&
		     EG(current_execute_data)->opline->opcode == ZEND_INCLUDE_OR_EVAL &&
		     (EG(current_execute_data)->opline->extended_value == ZEND_INCLUDE_ONCE ||
		      EG(current_execute_data)->opline->extended_value == ZEND_REQUIRE_ONCE))) {

			/* we are in include_once or FastCGI request */
			zend_string *resolved_path;
			int key_length;
			char *key = NULL;

			if (!ZCG(accel_directives).revalidate_path) {
				/* lookup by "not-real" path */
				key = accel_make_persistent_key(filename, filename_len, &key_length);
				if (key) {
					zend_accel_hash_entry *bucket = zend_accel_hash_str_find_entry(&ZCSG(hash), key, key_length);
					if (bucket != NULL) {
						zend_persistent_script *persistent_script = (zend_persistent_script *)bucket->data;
						if (!persistent_script->corrupted) {
							ZCG(cache_opline) = EG(current_execute_data) ? EG(current_execute_data)->opline : NULL;
							ZCG(cache_persistent_script) = persistent_script;
							return zend_string_copy(persistent_script->script.filename);
						}
					}
				} else {
					ZCG(cache_opline) = NULL;
					ZCG(cache_persistent_script) = NULL;
					return accelerator_orig_zend_resolve_path(filename, filename_len);
				}
			}

			/* find the full real path */
			resolved_path = accelerator_orig_zend_resolve_path(filename, filename_len);

			if (resolved_path) {
				/* lookup by real path */
				zend_accel_hash_entry *bucket = zend_accel_hash_find_entry(&ZCSG(hash), resolved_path);
				if (bucket) {
					zend_persistent_script *persistent_script = (zend_persistent_script *)bucket->data;
					if (!persistent_script->corrupted) {
						if (key) {
							/* add another "key" for the same bucket */
							HANDLE_BLOCK_INTERRUPTIONS();
							SHM_UNPROTECT();
							zend_shared_alloc_lock();
							zend_accel_add_key(key, key_length, bucket);
							zend_shared_alloc_unlock();
							SHM_PROTECT();
							HANDLE_UNBLOCK_INTERRUPTIONS();
						} else {
							ZCG(key_len) = 0;
						}
						ZCG(cache_opline) = EG(current_execute_data) ? EG(current_execute_data)->opline : NULL;
						ZCG(cache_persistent_script) = persistent_script;
						return resolved_path;
					}
				}
			}

			ZCG(cache_opline) = NULL;
			ZCG(cache_persistent_script) = NULL;
			return resolved_path;
		}
	}
	ZCG(cache_opline) = NULL;
	ZCG(cache_persistent_script) = NULL;
	return accelerator_orig_zend_resolve_path(filename, filename_len);
}

static void zend_reset_cache_vars(void)
{
	ZSMMG(memory_exhausted) = 0;
	ZCSG(hits) = 0;
	ZCSG(misses) = 0;
	ZCSG(blacklist_misses) = 0;
	ZSMMG(wasted_shared_memory) = 0;
	ZCSG(restart_pending) = 0;
	ZCSG(force_restart_time) = 0;
}

static void accel_reset_pcre_cache(void)
{
	Bucket *p;

	ZEND_HASH_FOREACH_BUCKET(&PCRE_G(pcre_cache), p) {
		/* Remove PCRE cache entries with inconsistent keys */
		if (zend_accel_in_shm(p->key)) {
			p->key = NULL;
			zend_hash_del_bucket(&PCRE_G(pcre_cache), p);
		}
	} ZEND_HASH_FOREACH_END();
}

static void accel_activate(void)
{
	zend_bool reset_pcre = 0;

	if (!ZCG(enabled) || !accel_startup_ok) {
		return;
	}

	if (!ZCG(function_table).nTableSize) {
		zend_hash_init(&ZCG(function_table), zend_hash_num_elements(CG(function_table)), NULL, ZEND_FUNCTION_DTOR, 1);
		zend_accel_copy_internal_functions();
	}

	/* PHP-5.4 and above return "double", but we use 1 sec precision */
	ZCG(auto_globals_mask) = 0;
	ZCG(request_time) = (time_t)sapi_get_request_time();
	ZCG(cache_opline) = NULL;
	ZCG(cache_persistent_script) = NULL;
	ZCG(include_path_key_len) = 0;
	ZCG(include_path_check) = 1;

	/* check if ZCG(function_table) wasn't somehow polluted on the way */
	if (ZCG(internal_functions_count) != (zend_long)zend_hash_num_elements(&ZCG(function_table))) {
		zend_accel_error(ACCEL_LOG_WARNING, "Internal functions count changed - was %d, now %d", ZCG(internal_functions_count), zend_hash_num_elements(&ZCG(function_table)));
	}

	ZCG(cwd) = NULL;
	ZCG(cwd_key_len) = 0;
	ZCG(cwd_check) = 1;

#ifdef HAVE_OPCACHE_FILE_CACHE
	if (file_cache_only) {
		return;
	}
#endif

#ifndef ZEND_WIN32
	if (ZCG(accel_directives).validate_root) {
		struct stat buf;

		if (stat("/", &buf) != 0) {
			ZCG(root_hash) = 0;
		} else {
			ZCG(root_hash) = buf.st_ino;
			if (sizeof(buf.st_ino) > sizeof(ZCG(root_hash))) {
				if (ZCG(root_hash) != buf.st_ino) {
					zend_string *key = zend_string_init("opcache.enable", sizeof("opcache.enable")-1, 0);
					zend_alter_ini_entry_chars(key, "0", 1, ZEND_INI_SYSTEM, ZEND_INI_STAGE_RUNTIME);
					zend_string_release_ex(key, 0);
					zend_accel_error(ACCEL_LOG_WARNING, "Can't cache files in chroot() directory with too big inode");
					return;
				}
			}
		}
	} else {
		ZCG(root_hash) = 0;
	}
#endif

	HANDLE_BLOCK_INTERRUPTIONS();
	SHM_UNPROTECT();

	if (ZCG(counted)) {
#ifdef ZTS
		zend_accel_error(ACCEL_LOG_WARNING, "Stuck count for thread id %lu", (unsigned long) tsrm_thread_id());
#else
		zend_accel_error(ACCEL_LOG_WARNING, "Stuck count for pid %d", getpid());
#endif
		accel_unlock_all();
		ZCG(counted) = 0;
	}

	if (ZCSG(restart_pending)) {
		zend_shared_alloc_lock();
		if (ZCSG(restart_pending) != 0) { /* check again, to ensure that the cache wasn't already cleaned by another process */
			if (accel_is_inactive() == SUCCESS) {
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
				accel_restart_enter();

				zend_reset_cache_vars();
				zend_accel_hash_clean(&ZCSG(hash));

				if (ZCG(accel_directives).interned_strings_buffer) {
					accel_interned_strings_restore_state();
				}

				zend_shared_alloc_restore_state();
				ZCSG(accelerator_enabled) = ZCSG(cache_status_before_restart);
				if (ZCSG(last_restart_time) < ZCG(request_time)) {
					ZCSG(last_restart_time) = ZCG(request_time);
				} else {
					ZCSG(last_restart_time)++;
				}
				accel_restart_leave();
			}
		} else {
			reset_pcre = 1;
		}
		zend_shared_alloc_unlock();
	}

	SHM_PROTECT();
	HANDLE_UNBLOCK_INTERRUPTIONS();

	if (ZCSG(last_restart_time) != ZCG(last_restart_time)) {
		/* SHM was reinitialized. */
		ZCG(last_restart_time) = ZCSG(last_restart_time);

		/* Reset in-process realpath cache */
		realpath_cache_clean();

		accel_reset_pcre_cache();
	} else if (reset_pcre) {
		accel_reset_pcre_cache();
	}
}

int accel_post_deactivate(void)
{
	if (!ZCG(enabled) || !accel_startup_ok) {
		return SUCCESS;
	}

	zend_shared_alloc_safe_unlock(); /* be sure we didn't leave cache locked */
	accel_unlock_all();
	ZCG(counted) = 0;

	return SUCCESS;
}

static void accel_deactivate(void)
{
	/* ensure that we restore function_table and class_table
	 * In general, they're restored by persistent_compile_file(), but in case
	 * the script is aborted abnormally, they may become messed up.
	 */

	if (ZCG(cwd)) {
		zend_string_release_ex(ZCG(cwd), 0);
		ZCG(cwd) = NULL;
	}

	if (!ZCG(enabled) || !accel_startup_ok) {
		return;
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

static void zps_startup_failure(char *reason, char *api_reason, int (*cb)(zend_extension *, zend_extension *))
{
	accel_startup_ok = 0;
	zps_failure_reason = reason;
	zps_api_failure_reason = api_reason?api_reason:reason;
	zend_llist_del_element(&zend_extensions, NULL, (int (*)(void *, void *))cb);
}

static inline int accel_find_sapi(void)
{
	static const char *supported_sapis[] = {
		"apache",
		"fastcgi",
		"cli-server",
		"cgi-fcgi",
		"fpm-fcgi",
		"apache2handler",
		"litespeed",
		"uwsgi",
		NULL
	};
	const char **sapi_name;

	if (sapi_module.name) {
		for (sapi_name = supported_sapis; *sapi_name; sapi_name++) {
			if (strcmp(sapi_module.name, *sapi_name) == 0) {
				return SUCCESS;
			}
		}
		if (ZCG(accel_directives).enable_cli && (
		    strcmp(sapi_module.name, "cli") == 0
		  || strcmp(sapi_module.name, "phpdbg") == 0)) {
			return SUCCESS;
		}
	}

	return FAILURE;
}

static int zend_accel_init_shm(void)
{
	int i;

	zend_shared_alloc_lock();

	if (ZCG(accel_directives).interned_strings_buffer) {
		accel_shared_globals = zend_shared_alloc((ZCG(accel_directives).interned_strings_buffer * 1024 * 1024));
	} else {
		accel_shared_globals = zend_shared_alloc(sizeof(zend_accel_shared_globals));
	}
	if (!accel_shared_globals) {
		zend_accel_error(ACCEL_LOG_FATAL, "Insufficient shared memory!");
		zend_shared_alloc_unlock();
		return FAILURE;
	}
	memset(accel_shared_globals, 0, sizeof(zend_accel_shared_globals));
	ZSMMG(app_shared_globals) = accel_shared_globals;

	zend_accel_hash_init(&ZCSG(hash), ZCG(accel_directives).max_accelerated_files);

	if (ZCG(accel_directives).interned_strings_buffer) {
		uint32_t hash_size;

		/* must be a power of two */
		hash_size = ZCG(accel_directives).interned_strings_buffer * (32 * 1024);
		hash_size |= (hash_size >> 1);
		hash_size |= (hash_size >> 2);
		hash_size |= (hash_size >> 4);
		hash_size |= (hash_size >> 8);
		hash_size |= (hash_size >> 16);

		ZCSG(interned_strings).nTableMask = hash_size << 2;
		ZCSG(interned_strings).nNumOfElements = 0;
		ZCSG(interned_strings).start =
			(zend_string*)((char*)&ZCSG(interned_strings) +
				sizeof(zend_string_table) +
				((hash_size + 1) * sizeof(uint32_t))) +
				8;
		ZCSG(interned_strings).top =
			ZCSG(interned_strings).start;
		ZCSG(interned_strings).end =
			(zend_string*)((char*)accel_shared_globals +
				ZCG(accel_directives).interned_strings_buffer * 1024 * 1024);
		ZCSG(interned_strings).saved_top = NULL;

		memset((char*)&ZCSG(interned_strings) + sizeof(zend_string_table),
			STRTAB_INVALID_POS,
			(char*)ZCSG(interned_strings).start -
				((char*)&ZCSG(interned_strings) + sizeof(zend_string_table)));

		zend_interned_strings_set_permanent_storage_copy_handlers(accel_use_shm_interned_strings, accel_use_permanent_interned_strings);
	}

	zend_interned_strings_set_request_storage_handlers(accel_new_interned_string_for_php, accel_init_interned_string_for_php);

	zend_reset_cache_vars();

	ZCSG(oom_restarts) = 0;
	ZCSG(hash_restarts) = 0;
	ZCSG(manual_restarts) = 0;

	ZCSG(accelerator_enabled) = 1;
	ZCSG(start_time) = zend_accel_get_time();
	ZCSG(last_restart_time) = 0;
	ZCSG(restart_in_progress) = 0;


	for (i = 0; i < -HT_MIN_MASK; i++) {
		ZCSG(uninitialized_bucket)[i] = HT_INVALID_IDX;
	}

	zend_shared_alloc_unlock();

	return SUCCESS;
}

static void accel_globals_ctor(zend_accel_globals *accel_globals)
{
#if defined(COMPILE_DL_OPCACHE) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	memset(accel_globals, 0, sizeof(zend_accel_globals));

	/* TODO refactor to init this just once. */
	accel_gen_system_id();
}

static void accel_globals_dtor(zend_accel_globals *accel_globals)
{
	if (accel_globals->function_table.nTableSize) {
		accel_globals->function_table.pDestructor = NULL;
		zend_hash_destroy(&accel_globals->function_table);
	}
}

#define ZEND_BIN_ID "BIN_" ZEND_TOSTR(SIZEOF_CHAR) ZEND_TOSTR(SIZEOF_INT) ZEND_TOSTR(SIZEOF_LONG) ZEND_TOSTR(SIZEOF_SIZE_T) ZEND_TOSTR(SIZEOF_ZEND_LONG) ZEND_TOSTR(ZEND_MM_ALIGNMENT)

static void accel_gen_system_id(void)
{
	PHP_MD5_CTX context;
	unsigned char digest[16], c;
	char *md5str = ZCG(system_id);
	int i;

	PHP_MD5Init(&context);
	PHP_MD5Update(&context, PHP_VERSION, sizeof(PHP_VERSION)-1);
	PHP_MD5Update(&context, ZEND_EXTENSION_BUILD_ID, sizeof(ZEND_EXTENSION_BUILD_ID)-1);
	PHP_MD5Update(&context, ZEND_BIN_ID, sizeof(ZEND_BIN_ID)-1);
	if (strstr(PHP_VERSION, "-dev") != 0) {
		/* Development versions may be changed from build to build */
		PHP_MD5Update(&context, __DATE__, sizeof(__DATE__)-1);
		PHP_MD5Update(&context, __TIME__, sizeof(__TIME__)-1);
	}
	PHP_MD5Final(digest, &context);
	for (i = 0; i < 16; i++) {
		c = digest[i] >> 4;
		c = (c <= 9) ? c + '0' : c - 10 + 'a';
		md5str[i * 2] = c;
		c = digest[i] &  0x0f;
		c = (c <= 9) ? c + '0' : c - 10 + 'a';
		md5str[(i * 2) + 1] = c;
	}
}

#ifdef HAVE_HUGE_CODE_PAGES
# ifndef _WIN32
#  include <sys/mman.h>
#  ifndef MAP_ANON
#   ifdef MAP_ANONYMOUS
#    define MAP_ANON MAP_ANONYMOUS
#   endif
#  endif
#  ifndef MAP_FAILED
#   define MAP_FAILED ((void*)-1)
#  endif
# endif

# if defined(MAP_HUGETLB) || defined(MADV_HUGEPAGE)
static int accel_remap_huge_pages(void *start, size_t size, const char *name, size_t offset)
{
	void *ret = MAP_FAILED;
	void *mem;

	mem = mmap(NULL, size,
		PROT_READ | PROT_WRITE,
		MAP_PRIVATE | MAP_ANONYMOUS,
		-1, 0);
	if (mem == MAP_FAILED) {
		zend_error(E_WARNING,
			ACCELERATOR_PRODUCT_NAME " huge_code_pages: mmap failed: %s (%d)",
			strerror(errno), errno);
		return -1;
	}
	memcpy(mem, start, size);

#  ifdef MAP_HUGETLB
	ret = mmap(start, size,
		PROT_READ | PROT_WRITE | PROT_EXEC,
		MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED | MAP_HUGETLB,
		-1, 0);
#  endif
	if (ret == MAP_FAILED) {
		ret = mmap(start, size,
			PROT_READ | PROT_WRITE | PROT_EXEC,
			MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED,
			-1, 0);
		/* this should never happen? */
		ZEND_ASSERT(ret != MAP_FAILED);
#  ifdef MADV_HUGEPAGE
		if (-1 == madvise(start, size, MADV_HUGEPAGE)) {
			memcpy(start, mem, size);
			mprotect(start, size, PROT_READ | PROT_EXEC);
			munmap(mem, size);
			zend_error(E_WARNING,
				ACCELERATOR_PRODUCT_NAME " huge_code_pages: madvise(HUGEPAGE) failed: %s (%d)",
				strerror(errno), errno);
			return -1;
		}
#  else
		memcpy(start, mem, size);
		mprotect(start, size, PROT_READ | PROT_EXEC);
		munmap(mem, size);
		zend_error(E_WARNING,
			ACCELERATOR_PRODUCT_NAME " huge_code_pages: mmap(HUGETLB) failed: %s (%d)",
			strerror(errno), errno);
		return -1;
#  endif
	}

	if (ret == start) {
		memcpy(start, mem, size);
		mprotect(start, size, PROT_READ | PROT_EXEC);
	}
	munmap(mem, size);

	return (ret == start) ? 0 : -1;
}

static void accel_move_code_to_huge_pages(void)
{
	FILE *f;
	long unsigned int huge_page_size = 2 * 1024 * 1024;

	f = fopen("/proc/self/maps", "r");
	if (f) {
		long unsigned int  start, end, offset, inode;
		char perm[5], dev[6], name[MAXPATHLEN];
		int ret;

		ret = fscanf(f, "%lx-%lx %4s %lx %5s %ld %s\n", &start, &end, perm, &offset, dev, &inode, name);
		if (ret == 7 && perm[0] == 'r' && perm[1] == '-' && perm[2] == 'x' && name[0] == '/') {
			long unsigned int  seg_start = ZEND_MM_ALIGNED_SIZE_EX(start, huge_page_size);
			long unsigned int  seg_end = (end & ~(huge_page_size-1L));

			if (seg_end > seg_start) {
				zend_accel_error(ACCEL_LOG_DEBUG, "remap to huge page %lx-%lx %s \n", seg_start, seg_end, name);
				accel_remap_huge_pages((void*)seg_start, seg_end - seg_start, name, offset + seg_start - start);
			}
		}
		fclose(f);
	}
}
# else
static void accel_move_code_to_huge_pages(void)
{
	zend_error(E_WARNING, ACCELERATOR_PRODUCT_NAME ": opcache.huge_code_pages has no affect as huge page is not supported");
	return;
}
# endif /* defined(MAP_HUGETLB) || defined(MADV_HUGEPAGE) */
#endif /* HAVE_HUGE_CODE_PAGES */

static int accel_startup(zend_extension *extension)
{
#ifdef ZTS
	accel_globals_id = ts_allocate_id(&accel_globals_id, sizeof(zend_accel_globals), (ts_allocate_ctor) accel_globals_ctor, (ts_allocate_dtor) accel_globals_dtor);
#else
	accel_globals_ctor(&accel_globals);
#endif

#ifdef ZEND_WIN32
# if !defined(__has_feature) || !__has_feature(address_sanitizer)
	_setmaxstdio(2048); /* The default configuration is limited to 512 stdio files */
# endif
#endif

	if (start_accel_module() == FAILURE) {
		accel_startup_ok = 0;
		zend_error(E_WARNING, ACCELERATOR_PRODUCT_NAME ": module registration failed!");
		return FAILURE;
	}

	accel_gen_system_id();

#ifdef HAVE_HUGE_CODE_PAGES
	if (ZCG(accel_directives).huge_code_pages &&
	    (strcmp(sapi_module.name, "cli") == 0 ||
	     strcmp(sapi_module.name, "cli-server") == 0 ||
		 strcmp(sapi_module.name, "cgi-fcgi") == 0 ||
		 strcmp(sapi_module.name, "fpm-fcgi") == 0)) {
		accel_move_code_to_huge_pages();
	}
#endif

	/* no supported SAPI found - disable acceleration and stop initialization */
	if (accel_find_sapi() == FAILURE) {
		accel_startup_ok = 0;
		if (!ZCG(accel_directives).enable_cli &&
		    strcmp(sapi_module.name, "cli") == 0) {
			zps_startup_failure("Opcode Caching is disabled for CLI", NULL, accelerator_remove_cb);
		} else {
			zps_startup_failure("Opcode Caching is only supported in Apache, FPM, FastCGI and LiteSpeed SAPIs", NULL, accelerator_remove_cb);
		}
		return SUCCESS;
	}

	if (ZCG(enabled) == 0) {
		return SUCCESS ;
	}

	orig_post_startup_cb = zend_post_startup_cb;
	zend_post_startup_cb = accel_post_startup;

	return SUCCESS;
}

static int accel_post_startup(void)
{
	zend_function *func;
	zend_ini_entry *ini_entry;

	if (orig_post_startup_cb) {
		int (*cb)(void) = orig_post_startup_cb;

		orig_post_startup_cb = NULL;
		if (cb() != SUCCESS) {
			return FAILURE;
		}
	}

/********************************************/
/* End of non-SHM dependent initializations */
/********************************************/
#ifdef HAVE_OPCACHE_FILE_CACHE
	file_cache_only = ZCG(accel_directives).file_cache_only;
	if (!file_cache_only) {
#else
	if (1) {
#endif
		switch (zend_shared_alloc_startup(ZCG(accel_directives).memory_consumption)) {
			case ALLOC_SUCCESS:
				if (zend_accel_init_shm() == FAILURE) {
					accel_startup_ok = 0;
					return FAILURE;
				}
				break;
			case ALLOC_FAILURE:
				accel_startup_ok = 0;
				zend_accel_error(ACCEL_LOG_FATAL, "Failure to initialize shared memory structures - probably not enough shared memory.");
				return SUCCESS;
			case SUCCESSFULLY_REATTACHED:
				zend_shared_alloc_lock();
				accel_shared_globals = (zend_accel_shared_globals *) ZSMMG(app_shared_globals);
				if (ZCG(accel_directives).interned_strings_buffer) {
					zend_interned_strings_set_permanent_storage_copy_handlers(accel_use_shm_interned_strings, accel_use_permanent_interned_strings);
				}
				zend_interned_strings_set_request_storage_handlers(accel_new_interned_string_for_php, accel_init_interned_string_for_php);
				zend_shared_alloc_unlock();
				break;
			case FAILED_REATTACHED:
				accel_startup_ok = 0;
				zend_accel_error(ACCEL_LOG_FATAL, "Failure to initialize shared memory structures - can not reattach to exiting shared memory.");
				return SUCCESS;
				break;
#if ENABLE_FILE_CACHE_FALLBACK
			case ALLOC_FALLBACK:
				zend_shared_alloc_lock();
				file_cache_only = 1;
				fallback_process = 1;
				zend_accel_init_auto_globals();
				zend_shared_alloc_unlock();
				goto file_cache_fallback;
				break;
#endif
		}

		/* from this point further, shared memory is supposed to be OK */

		/* remember the last restart time in the process memory */
		ZCG(last_restart_time) = ZCSG(last_restart_time);

		/* Init auto-global strings */
		zend_accel_init_auto_globals();

		zend_shared_alloc_lock();
		zend_shared_alloc_save_state();
		zend_shared_alloc_unlock();

		SHM_PROTECT();
#ifdef HAVE_OPCACHE_FILE_CACHE
	} else if (!ZCG(accel_directives).file_cache) {
		accel_startup_ok = 0;
		zend_accel_error(ACCEL_LOG_FATAL, "opcache.file_cache_only is set without a proper setting of opcache.file_cache");
		return SUCCESS;
	} else {
		accel_shared_globals = calloc(1, sizeof(zend_accel_shared_globals));

		/* Init auto-global strings */
		zend_accel_init_auto_globals();
#endif
	}
#if ENABLE_FILE_CACHE_FALLBACK
file_cache_fallback:
#endif

	/* Override compiler */
	accelerator_orig_compile_file = zend_compile_file;
	zend_compile_file = persistent_compile_file;

	/* Override stream opener function (to eliminate open() call caused by
	 * include/require statements ) */
	accelerator_orig_zend_stream_open_function = zend_stream_open_function;
	zend_stream_open_function = persistent_stream_open_function;

	/* Override path resolver function (to eliminate stat() calls caused by
	 * include_once/require_once statements */
	accelerator_orig_zend_resolve_path = zend_resolve_path;
	zend_resolve_path = persistent_zend_resolve_path;

	/* Override chdir() function */
	if ((func = zend_hash_str_find_ptr(CG(function_table), "chdir", sizeof("chdir")-1)) != NULL &&
	    func->type == ZEND_INTERNAL_FUNCTION) {
		orig_chdir = func->internal_function.handler;
		func->internal_function.handler = ZEND_FN(accel_chdir);
	}
	ZCG(cwd) = NULL;
	ZCG(include_path) = NULL;

	/* Override "include_path" modifier callback */
	if ((ini_entry = zend_hash_str_find_ptr(EG(ini_directives), "include_path", sizeof("include_path")-1)) != NULL) {
		ZCG(include_path) = ini_entry->value;
		orig_include_path_on_modify = ini_entry->on_modify;
		ini_entry->on_modify = accel_include_path_on_modify;
	}

	accel_startup_ok = 1;

	/* Override file_exists(), is_file() and is_readable() */
	zend_accel_override_file_functions();

	/* Load black list */
	accel_blacklist.entries = NULL;
	if (ZCG(enabled) && accel_startup_ok &&
	    ZCG(accel_directives).user_blacklist_filename &&
	    *ZCG(accel_directives.user_blacklist_filename)) {
		zend_accel_blacklist_init(&accel_blacklist);
		zend_accel_blacklist_load(&accel_blacklist, ZCG(accel_directives.user_blacklist_filename));
	}

	zend_optimizer_startup();

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

void accel_shutdown(void)
{
	zend_ini_entry *ini_entry;
	zend_bool _file_cache_only = 0;

	zend_optimizer_shutdown();

	zend_accel_blacklist_shutdown(&accel_blacklist);

	if (!ZCG(enabled) || !accel_startup_ok) {
		accel_free_ts_resources();
		return;
	}

#ifdef HAVE_OPCACHE_FILE_CACHE
	_file_cache_only = file_cache_only;
#endif

	accel_reset_pcre_cache();

	accel_free_ts_resources();

	if (!_file_cache_only) {
		zend_shared_alloc_shutdown();
	}
	zend_compile_file = accelerator_orig_compile_file;

	if ((ini_entry = zend_hash_str_find_ptr(EG(ini_directives), "include_path", sizeof("include_path")-1)) != NULL) {
		ini_entry->on_modify = orig_include_path_on_modify;
	}
}

void zend_accel_schedule_restart(zend_accel_restart_reason reason)
{
	const char *zend_accel_restart_reason_text[ACCEL_RESTART_USER + 1] = {
		"out of memory",
		"hash overflow",
		"user",
	};

	if (ZCSG(restart_pending)) {
		/* don't schedule twice */
		return;
	}
	zend_accel_error(ACCEL_LOG_DEBUG, "Restart Scheduled! Reason: %s",
			zend_accel_restart_reason_text[reason]);

	HANDLE_BLOCK_INTERRUPTIONS();
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
	HANDLE_UNBLOCK_INTERRUPTIONS();
}

/* this is needed because on WIN32 lock is not decreased unless ZCG(counted) is set */
#ifdef ZEND_WIN32
#define accel_deactivate_now() ZCG(counted) = 1; accel_deactivate_sub()
#else
#define accel_deactivate_now() accel_deactivate_sub()
#endif

/* ensures it is OK to read SHM
	if it's not OK (restart in progress) returns FAILURE
	if OK returns SUCCESS
	MUST call accelerator_shm_read_unlock after done lock operations
*/
int accelerator_shm_read_lock(void)
{
	if (ZCG(counted)) {
		/* counted means we are holding read lock for SHM, so that nothing bad can happen */
		return SUCCESS;
	} else {
		/* here accelerator is active but we do not hold SHM lock. This means restart was scheduled
			or is in progress now */
		if (accel_activate_add() == FAILURE) { /* acquire usage lock */
			return FAILURE;
		}
		/* Now if we weren't inside restart, restart would not begin until we remove usage lock */
		if (ZCSG(restart_in_progress)) {
			/* we already were inside restart this means it's not safe to touch shm */
			accel_deactivate_now(); /* drop usage lock */
			return FAILURE;
		}
		ZCG(counted) = 1;
	}
	return SUCCESS;
}

/* must be called ONLY after SUCCESSFUL accelerator_shm_read_lock */
void accelerator_shm_read_unlock(void)
{
	if (!ZCG(counted)) {
		/* counted is 0 - meaning we had to readlock manually, release readlock now */
		accel_deactivate_now();
	}
}

ZEND_EXT_API zend_extension zend_extension_entry = {
	ACCELERATOR_PRODUCT_NAME,               /* name */
	PHP_VERSION,							/* version */
	"Zend Technologies",					/* author */
	"http://www.zend.com/",					/* URL */
	"Copyright (c) 1999-2018",				/* copyright */
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
