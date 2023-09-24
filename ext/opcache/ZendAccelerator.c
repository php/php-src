/*
   +----------------------------------------------------------------------+
   | Zend OPcache                                                         |
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
#include "zend_vm.h"
#include "zend_inheritance.h"
#include "zend_exceptions.h"
#include "zend_mmap.h"
#include "zend_observer.h"
#include "main/php_main.h"
#include "main/SAPI.h"
#include "main/php_streams.h"
#include "main/php_open_temporary_file.h"
#include "zend_API.h"
#include "zend_ini.h"
#include "zend_virtual_cwd.h"
#include "zend_accelerator_util_funcs.h"
#include "zend_accelerator_hash.h"
#include "zend_file_cache.h"
#include "ext/pcre/php_pcre.h"
#include "ext/standard/md5.h"
#include "ext/hash/php_hash.h"

#ifdef HAVE_JIT
# include "jit/zend_jit.h"
#endif

#ifndef ZEND_WIN32
#include  <netdb.h>
#endif

#ifdef ZEND_WIN32
typedef int uid_t;
typedef int gid_t;
#include <io.h>
#include <lmcons.h>
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
# include <sys/wait.h>
# include <sys/ipc.h>
# include <pwd.h>
# include <grp.h>
#endif

#include <sys/stat.h>
#include <errno.h>

#ifdef __AVX__
#include <immintrin.h>
#endif

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
#ifdef ZEND_WIN32
char accel_uname_id[32];
#endif
bool accel_startup_ok = false;
static const char *zps_failure_reason = NULL;
const char *zps_api_failure_reason = NULL;
bool file_cache_only = false;  /* process uses file cache only */
#if ENABLE_FILE_CACHE_FALLBACK
bool fallback_process = false; /* process uses file cache fallback */
#endif

static zend_op_array *(*accelerator_orig_compile_file)(zend_file_handle *file_handle, int type);
static zend_class_entry* (*accelerator_orig_inheritance_cache_get)(zend_class_entry *ce, zend_class_entry *parent, zend_class_entry **traits_and_interfaces);
static zend_class_entry* (*accelerator_orig_inheritance_cache_add)(zend_class_entry *ce, zend_class_entry *proto, zend_class_entry *parent, zend_class_entry **traits_and_interfaces, HashTable *dependencies);
static zend_result (*accelerator_orig_zend_stream_open_function)(zend_file_handle *handle );
static zend_string *(*accelerator_orig_zend_resolve_path)(zend_string *filename);
static zif_handler orig_chdir = NULL;
static ZEND_INI_MH((*orig_include_path_on_modify)) = NULL;
static zend_result (*orig_post_startup_cb)(void);

static zend_result accel_post_startup(void);
static zend_result accel_finish_startup(void);

static void preload_shutdown(void);
static void preload_activate(void);
static void preload_restart(void);

#ifdef ZEND_WIN32
# define INCREMENT(v) InterlockedIncrement64(&ZCSG(v))
# define DECREMENT(v) InterlockedDecrement64(&ZCSG(v))
# define LOCKVAL(v)   (ZCSG(v))
#endif

/**
 * Clear AVX/SSE2-aligned memory.
 */
static void bzero_aligned(void *mem, size_t size)
{
#if defined(__x86_64__)
	memset(mem, 0, size);
#elif defined(__AVX__)
	char *p = (char*)mem;
	char *end = p + size;
	__m256i ymm0 = _mm256_setzero_si256();

	while (p < end) {
		_mm256_store_si256((__m256i*)p, ymm0);
		_mm256_store_si256((__m256i*)(p+32), ymm0);
		p += 64;
	}
#elif defined(__SSE2__)
	char *p = (char*)mem;
	char *end = p + size;
	__m128i xmm0 = _mm_setzero_si128();

	while (p < end) {
		_mm_store_si128((__m128i*)p, xmm0);
		_mm_store_si128((__m128i*)(p+16), xmm0);
		_mm_store_si128((__m128i*)(p+32), xmm0);
		_mm_store_si128((__m128i*)(p+48), xmm0);
		p += 64;
	}
#else
	memset(mem, 0, size);
#endif
}

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

static inline bool is_cacheable_stream_path(const char *filename)
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
	ZCG(cwd_check) = true;
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
		ZCG(cwd_check) = true;
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
		ZCG(include_path_check) = true;
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
	ZCSG(restart_in_progress) = true;
}

static inline void accel_restart_leave(void)
{
#ifdef ZEND_WIN32
	ZCSG(restart_in_progress) = false;
	DECREMENT(restart_in);
#else
	struct flock restart_finished;

	restart_finished.l_type = F_UNLCK;
	restart_finished.l_whence = SEEK_SET;
	restart_finished.l_start = 2;
	restart_finished.l_len = 1;

	ZCSG(restart_in_progress) = false;
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
			ZCSG(restart_in_progress) = false;
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
static inline zend_result accel_activate_add(void)
{
#ifdef ZEND_WIN32
	SHM_UNPROTECT();
	INCREMENT(mem_usage);
	SHM_PROTECT();
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
		SHM_UNPROTECT();
		DECREMENT(mem_usage);
		ZCG(counted) = false;
		SHM_PROTECT();
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
	if (lock_file == -1) {
		return;
	}

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
			if (ZSTR_HAS_CE_CACHE(s)) {
				/* Discard non-global CE_CACHE slots on reset. */
				uintptr_t idx = (GC_REFCOUNT(s) - 1) / sizeof(void *);
				if (idx >= ZCSG(map_ptr_last)) {
					GC_SET_REFCOUNT(s, 2);
					GC_DEL_FLAGS(s, IS_STR_CLASS_NAME_MAP_PTR);
				}
			}

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
	zend_ulong   h;
	uint32_t     pos;
	zend_string *s;

	if (IS_ACCEL_INTERNED(str)) {
		/* this is already an interned string */
		return str;
	}

	if (!ZCG(counted)) {
		if (!ZCG(accelerator_enabled) || accel_activate_add() == FAILURE) {
			return NULL;
		}
		ZCG(counted) = true;
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

	if (UNEXPECTED(file_cache_only)) {
		return str;
	}

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
				goto finish;
			}
			pos = STRTAB_COLLISION(s);
		} while (pos != STRTAB_INVALID_POS);
	}

	if (UNEXPECTED((char*)ZCSG(interned_strings).end - (char*)ZCSG(interned_strings).top < STRTAB_STR_SIZE(str))) {
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
	GC_SET_REFCOUNT(s, 2);
	GC_TYPE_INFO(s) = GC_STRING | ((IS_STR_INTERNED | IS_STR_PERMANENT) << GC_FLAGS_SHIFT)| (ZSTR_IS_VALID_UTF8(str) ? IS_STR_VALID_UTF8 : 0);
	ZSTR_H(s) = h;
	ZSTR_LEN(s) = ZSTR_LEN(str);
	memcpy(ZSTR_VAL(s), ZSTR_VAL(str), ZSTR_LEN(s) + 1);
	ZCSG(interned_strings).top = STRTAB_NEXT(s);

finish:
	/* Transfer CE_CACHE map ptr slot to new interned string.
	 * Should only happen for permanent interned strings with permanent map_ptr slot. */
	if (ZSTR_HAS_CE_CACHE(str) && !ZSTR_HAS_CE_CACHE(s)) {
		ZEND_ASSERT(GC_FLAGS(str) & IS_STR_PERMANENT);
		GC_SET_REFCOUNT(s, GC_REFCOUNT(str));
		GC_ADD_FLAGS(s, IS_STR_CLASS_NAME_MAP_PTR);
	}

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
			if (EXPECTED(ZSTR_H(s) == h) && zend_string_equals_cstr(s, str, size)) {
				return s;
			}
			pos = STRTAB_COLLISION(s);
		} while (pos != STRTAB_INVALID_POS);
	}
	return NULL;
}

static zend_string* ZEND_FASTCALL accel_init_interned_string_for_php(const char *str, size_t size, bool permanent)
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

static inline void accel_copy_permanent_list_types(
	zend_new_interned_string_func_t new_interned_string, zend_type type)
{
	zend_type *single_type;
	ZEND_TYPE_FOREACH(type, single_type) {
		if (ZEND_TYPE_HAS_LIST(*single_type)) {
			ZEND_ASSERT(ZEND_TYPE_IS_INTERSECTION(*single_type));
			accel_copy_permanent_list_types(new_interned_string, *single_type);
		}
		if (ZEND_TYPE_HAS_NAME(*single_type)) {
			ZEND_TYPE_SET_PTR(*single_type, new_interned_string(ZEND_TYPE_NAME(*single_type)));
		}
	} ZEND_TYPE_FOREACH_END();
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
		zend_one_char_string[j] = new_interned_string(ZSTR_CHAR(j));
	}
	for (j = 0; j < ZEND_STR_LAST_KNOWN; j++) {
		zend_known_strings[j] = new_interned_string(zend_known_strings[j]);
	}

	/* function table hash keys */
	ZEND_HASH_MAP_FOREACH_BUCKET(CG(function_table), p) {
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
				accel_copy_permanent_list_types(new_interned_string, arg_info[i].type);
			}
		}
	} ZEND_HASH_FOREACH_END();

	/* class table hash keys, class names, properties, methods, constants, etc */
	ZEND_HASH_MAP_FOREACH_BUCKET(CG(class_table), p) {
		zend_class_entry *ce;

		ce = (zend_class_entry*)Z_PTR(p->val);

		if (p->key) {
			p->key = new_interned_string(p->key);
		}

		if (ce->name) {
			ce->name = new_interned_string(ce->name);
			ZEND_ASSERT(ZSTR_HAS_CE_CACHE(ce->name));
		}

		ZEND_HASH_MAP_FOREACH_BUCKET(&ce->properties_info, q) {
			zend_property_info *info;

			info = (zend_property_info*)Z_PTR(q->val);

			if (q->key) {
				q->key = new_interned_string(q->key);
			}

			if (info->name) {
				info->name = new_interned_string(info->name);
			}
		} ZEND_HASH_FOREACH_END();

		ZEND_HASH_MAP_FOREACH_BUCKET(&ce->function_table, q) {
			if (q->key) {
				q->key = new_interned_string(q->key);
			}
			if (Z_FUNC(q->val)->common.function_name) {
				Z_FUNC(q->val)->common.function_name = new_interned_string(Z_FUNC(q->val)->common.function_name);
			}
		} ZEND_HASH_FOREACH_END();

		ZEND_HASH_MAP_FOREACH_BUCKET(&ce->constants_table, q) {
			zend_class_constant* c;

			if (q->key) {
				q->key = new_interned_string(q->key);
			}
			c = (zend_class_constant*)Z_PTR(q->val);
			if (Z_TYPE(c->value) == IS_STRING) {
				ZVAL_STR(&c->value, new_interned_string(Z_STR(c->value)));
			}
		} ZEND_HASH_FOREACH_END();
	} ZEND_HASH_FOREACH_END();

	/* constant hash keys */
	ZEND_HASH_MAP_FOREACH_BUCKET(EG(zend_constants), p) {
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
	ZEND_HASH_MAP_FOREACH_BUCKET(CG(auto_globals), p) {
		zend_auto_global *auto_global;

		auto_global = (zend_auto_global*)Z_PTR(p->val);

		zend_string_addref(auto_global->name);
		auto_global->name = new_interned_string(auto_global->name);
		if (p->key) {
			p->key = new_interned_string(p->key);
		}
	} ZEND_HASH_FOREACH_END();

	ZEND_HASH_MAP_FOREACH_BUCKET(&module_registry, p) {
		if (p->key) {
			p->key = new_interned_string(p->key);
		}
	} ZEND_HASH_FOREACH_END();

	ZEND_HASH_MAP_FOREACH_BUCKET(EG(ini_directives), p) {
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
	ZEND_HASH_MAP_FOREACH_BUCKET(ht, p) {
		if (p->key) {
			p->key = new_interned_string(p->key);
		}
	} ZEND_HASH_FOREACH_END();

	ht = php_stream_get_url_stream_wrappers_hash_global();
	ZEND_HASH_MAP_FOREACH_BUCKET(ht, p) {
		if (p->key) {
			p->key = new_interned_string(p->key);
		}
	} ZEND_HASH_FOREACH_END();

	ht = php_stream_xport_get_hash();
	ZEND_HASH_MAP_FOREACH_BUCKET(ht, p) {
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

static void accel_use_shm_interned_strings(void)
{
	HANDLE_BLOCK_INTERRUPTIONS();
	SHM_UNPROTECT();
	zend_shared_alloc_lock();

	if (ZCSG(interned_strings).saved_top == NULL) {
		accel_copy_permanent_strings(accel_new_interned_string);
	} else {
		ZCG(counted) = true;
		accel_copy_permanent_strings(accel_replace_string_by_shm_permanent);
		ZCG(counted) = false;
	}
	accel_interned_strings_save_state();

	zend_shared_alloc_unlock();
	SHM_PROTECT();
	HANDLE_UNBLOCK_INTERRUPTIONS();
}

#ifndef ZEND_WIN32
static inline void kill_all_lockers(struct flock *mem_usage_check)
{
	int tries;
	/* so that other process won't try to force while we are busy cleaning up */
	ZCSG(force_restart_time) = 0;
	while (mem_usage_check->l_pid > 0) {
		/* Try SIGTERM first, switch to SIGKILL if not successful. */
		int signal = SIGTERM;
		errno = 0;
		bool success = false;
		tries = 10;

		while (tries--) {
			zend_accel_error(ACCEL_LOG_WARNING, "Attempting to kill locker %d", mem_usage_check->l_pid);
			if (kill(mem_usage_check->l_pid, signal)) {
				if (errno == ESRCH) {
					/* Process died before the signal was sent */
					success = true;
					zend_accel_error(ACCEL_LOG_WARNING, "Process %d died before SIGKILL was sent", mem_usage_check->l_pid);
				} else if (errno != 0) {
					zend_accel_error(ACCEL_LOG_WARNING, "Failed to send SIGKILL to locker %d: %s", mem_usage_check->l_pid, strerror(errno));
				}
				break;
			}
			/* give it a chance to die */
			usleep(20000);
			if (kill(mem_usage_check->l_pid, 0)) {
				if (errno == ESRCH) {
					/* successfully killed locker, process no longer exists  */
					success = true;
					zend_accel_error(ACCEL_LOG_WARNING, "Killed locker %d", mem_usage_check->l_pid);
				} else if (errno != 0) {
					zend_accel_error(ACCEL_LOG_WARNING, "Failed to check locker %d: %s", mem_usage_check->l_pid, strerror(errno));
				}
				break;
			}
			usleep(10000);
			/* If SIGTERM was not sufficient, use SIGKILL. */
			signal = SIGKILL;
		}
		if (!success) {
			/* errno is not ESRCH or we ran out of tries to kill the locker */
			ZCSG(force_restart_time) = time(NULL); /* restore forced restart request */
			/* cannot kill the locker, bail out with error */
			zend_accel_error_noreturn(ACCEL_LOG_ERROR, "Cannot kill process %d!", mem_usage_check->l_pid);
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

static inline bool accel_is_inactive(void)
{
#ifdef ZEND_WIN32
	/* on Windows, we don't need kill_all_lockers() because SAPIs
	   that work on Windows don't manage child processes (and we
	   can't do anything about hanging threads anyway); therefore
	   on Windows, we can simply manage this counter with atomics
	   instead of flocks (atomics are much faster but they don't
	   provide us with the PID of locker processes) */

	if (LOCKVAL(mem_usage) == 0) {
		return true;
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
		return false;
	}
	if (mem_usage_check.l_type == F_UNLCK) {
		return true;
	}

	if (ZCG(accel_directives).force_restart_timeout
		&& ZCSG(force_restart_time)
		&& time(NULL) >= ZCSG(force_restart_time)) {
		zend_accel_error(ACCEL_LOG_WARNING, "Forced restart at %ld (after " ZEND_LONG_FMT " seconds), locked by %d", (long)time(NULL), ZCG(accel_directives).force_restart_timeout, mem_usage_check.l_pid);
		kill_all_lockers(&mem_usage_check);

		return false; /* next request should be able to restart it */
	}
#endif

	return false;
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
	zend_stat_t statbuf = {0};
#ifdef ZEND_WIN32
	accel_time_t res;
#endif

	if (sapi_module.get_stat &&
	    !EG(current_execute_data) &&
	    file_handle->primary_script) {

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
		case ZEND_HANDLE_FP:
			if (zend_fstat(fileno(file_handle->handle.fp), &statbuf) == -1) {
				if (zend_get_stream_timestamp(ZSTR_VAL(file_handle->filename), &statbuf) != SUCCESS) {
					return 0;
				}
			}
			break;
		case ZEND_HANDLE_FILENAME:
			if (file_handle->opened_path) {
				char *file_path = ZSTR_VAL(file_handle->opened_path);

				if (php_is_stream_path(file_path)) {
					if (zend_get_stream_timestamp(file_path, &statbuf) == SUCCESS) {
						break;
					}
				}
				if (VCWD_STAT(file_path, &statbuf) != -1) {
					break;
				}
			}

			if (zend_get_stream_timestamp(ZSTR_VAL(file_handle->filename), &statbuf) != SUCCESS) {
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
	int ret;

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
		full_path_ptr = accelerator_orig_zend_resolve_path(file_handle->filename);
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

	zend_stream_init_filename_ex(&ps_handle, persistent_script->script.filename);
	ps_handle.opened_path = persistent_script->script.filename;

	ret = zend_get_file_handle_timestamp(&ps_handle, NULL) == persistent_script->timestamp
		? SUCCESS : FAILURE;

	zend_destroy_file_handle(&ps_handle);

	return ret;
}

zend_result validate_timestamp_and_record(zend_persistent_script *persistent_script, zend_file_handle *file_handle)
{
	if (persistent_script->timestamp == 0) {
		return SUCCESS; /* Don't check timestamps of preloaded scripts */
	} else if (ZCG(accel_directives).revalidate_freq &&
	    persistent_script->dynamic_members.revalidate >= ZCG(request_time)) {
		return SUCCESS;
	} else if (do_validate_timestamps(persistent_script, file_handle) == FAILURE) {
		return FAILURE;
	} else {
		persistent_script->dynamic_members.revalidate = ZCG(request_time) + ZCG(accel_directives).revalidate_freq;
		return SUCCESS;
	}
}

zend_result validate_timestamp_and_record_ex(zend_persistent_script *persistent_script, zend_file_handle *file_handle)
{
	SHM_UNPROTECT();
	const zend_result ret = validate_timestamp_and_record(persistent_script, file_handle);
	SHM_PROTECT();

	return ret;
}

/* Instead of resolving full real path name each time we need to identify file,
 * we create a key that consist from requested file name, current working
 * directory, current include_path, etc */
zend_string *accel_make_persistent_key(zend_string *str)
{
	const char *path = ZSTR_VAL(str);
	size_t path_length = ZSTR_LEN(str);
	char *key;
	int key_length;

	ZSTR_LEN(&ZCG(key)) = 0;

	/* CWD and include_path don't matter for absolute file names and streams */
	if (IS_ABSOLUTE_PATH(path, path_length)) {
		/* pass */
	} else if (UNEXPECTED(php_is_stream_path(path))) {
		if (!is_cacheable_stream_path(path)) {
			return NULL;
		}
		/* pass */
	} else if (UNEXPECTED(!ZCG(accel_directives).use_cwd)) {
		/* pass */
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
				ZCG(cwd_check) = false;
				if (ZCG(accelerator_enabled)) {

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
					} else {
						return NULL;
					}
				} else {
					return NULL;
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
				ZCG(include_path_check) = false;
				if (ZCG(accelerator_enabled)) {

					zend_string *str = accel_find_interned_string(ZCG(include_path));
					if (!str) {
						HANDLE_BLOCK_INTERRUPTIONS();
						SHM_UNPROTECT();
						zend_shared_alloc_lock();
						str = accel_new_interned_string(zend_string_copy(ZCG(include_path)));
						if (str == ZCG(include_path)) {
							zend_string_release(str);
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
					} else {
						return NULL;
					}
				} else {
					return NULL;
				}
			}
		}

		/* Calculate key length */
		if (UNEXPECTED((size_t)(cwd_len + path_length + include_path_len + 2) >= sizeof(ZCG(_key)))) {
			return NULL;
		}

		/* Generate key
		 * Note - the include_path must be the last element in the key,
		 * since in itself, it may include colons (which we use to separate
		 * different components of the key)
		 */
		key = ZSTR_VAL(&ZCG(key));
		memcpy(key, path, path_length);
		key[path_length] = ':';
		key_length = path_length + 1;
		memcpy(key + key_length, cwd, cwd_len);
		key_length += cwd_len;

		if (include_path_len) {
			key[key_length] = ':';
			key_length += 1;
			memcpy(key + key_length, include_path, include_path_len);
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

			if (UNEXPECTED((size_t)(key_length + parent_script_len + 1) >= sizeof(ZCG(_key)))) {
				return NULL;
			}
			key[key_length] = ':';
			key_length += 1;
			memcpy(key + key_length, ZSTR_VAL(parent_script), parent_script_len);
			key_length += parent_script_len;
		}
		key[key_length] = '\0';
		GC_SET_REFCOUNT(&ZCG(key), 1);
		GC_TYPE_INFO(&ZCG(key)) = GC_STRING;
		ZSTR_H(&ZCG(key)) = 0;
		ZSTR_LEN(&ZCG(key)) = key_length;
		return &ZCG(key);
	}

	/* not use_cwd */
	return str;
}

/**
 * Discard a #zend_persistent_script currently stored in shared
 * memory.
 *
 * Caller must lock shared memory via zend_shared_alloc_lock().
 */
static void zend_accel_discard_script(zend_persistent_script *persistent_script)
{
	if (persistent_script->corrupted) {
		/* already discarded */
		return;
	}

	persistent_script->corrupted = true;
	persistent_script->timestamp = 0;
	ZSMMG(wasted_shared_memory) += persistent_script->dynamic_members.memory_consumption;
	if (ZSMMG(memory_exhausted)) {
		zend_accel_restart_reason reason =
			zend_accel_hash_is_full(&ZCSG(hash)) ? ACCEL_RESTART_HASH : ACCEL_RESTART_OOM;
		zend_accel_schedule_restart_if_necessary(reason);
	}
}

/**
 * Wrapper for zend_accel_discard_script() which locks shared memory
 * via zend_shared_alloc_lock().
 */
static void zend_accel_lock_discard_script(zend_persistent_script *persistent_script)
{
	zend_shared_alloc_lock();
	zend_accel_discard_script(persistent_script);
	zend_shared_alloc_unlock();
}

zend_result zend_accel_invalidate(zend_string *filename, bool force)
{
	zend_string *realpath;
	zend_persistent_script *persistent_script;

	if (!ZCG(accelerator_enabled) || accelerator_shm_read_lock() != SUCCESS) {
		return FAILURE;
	}

	realpath = accelerator_orig_zend_resolve_path(filename);

	if (!realpath) {
		return FAILURE;
	}

	if (ZCG(accel_directives).file_cache) {
		zend_file_cache_invalidate(realpath);
	}

	persistent_script = zend_accel_hash_find(&ZCSG(hash), realpath);
	if (persistent_script && !persistent_script->corrupted) {
		zend_file_handle file_handle;
		zend_stream_init_filename_ex(&file_handle, realpath);
		file_handle.opened_path = realpath;

		if (force ||
			!ZCG(accel_directives).validate_timestamps ||
			do_validate_timestamps(persistent_script, &file_handle) == FAILURE) {
			HANDLE_BLOCK_INTERRUPTIONS();
			SHM_UNPROTECT();
			zend_accel_lock_discard_script(persistent_script);
			SHM_PROTECT();
			HANDLE_UNBLOCK_INTERRUPTIONS();
		}

		file_handle.opened_path = NULL;
		zend_destroy_file_handle(&file_handle);
	}

	accelerator_shm_read_unlock();
	zend_string_release_ex(realpath, 0);

	return SUCCESS;
}

static zend_string* accel_new_interned_key(zend_string *key)
{
	zend_string *new_key;

	if (zend_accel_in_shm(key)) {
		return key;
	}
	GC_ADDREF(key);
	new_key = accel_new_interned_string(key);
	if (UNEXPECTED(new_key == key)) {
		GC_DELREF(key);
		new_key = zend_shared_alloc(ZEND_MM_ALIGNED_SIZE_EX(_ZSTR_STRUCT_SIZE(ZSTR_LEN(key)), 8));
		if (EXPECTED(new_key)) {
			GC_SET_REFCOUNT(new_key, 2);
			GC_TYPE_INFO(new_key) = GC_STRING | (IS_STR_INTERNED << GC_FLAGS_SHIFT);
			ZSTR_H(new_key) = ZSTR_H(key);
			ZSTR_LEN(new_key) = ZSTR_LEN(key);
			memcpy(ZSTR_VAL(new_key), ZSTR_VAL(key), ZSTR_LEN(new_key) + 1);
		}
	}
	return new_key;
}

/* Adds another key for existing cached script */
static void zend_accel_add_key(zend_string *key, zend_accel_hash_entry *bucket)
{
	if (!zend_accel_hash_find(&ZCSG(hash), key)) {
		if (zend_accel_hash_is_full(&ZCSG(hash))) {
			zend_accel_error(ACCEL_LOG_DEBUG, "No more entries in hash table!");
			ZSMMG(memory_exhausted) = true;
			zend_accel_schedule_restart_if_necessary(ACCEL_RESTART_HASH);
		} else {
			zend_string *new_key = accel_new_interned_key(key);
			if (new_key) {
				if (zend_accel_hash_update(&ZCSG(hash), new_key, 1, bucket)) {
					zend_accel_error(ACCEL_LOG_INFO, "Added key '%s'", ZSTR_VAL(new_key));
				}
			} else {
				zend_accel_schedule_restart_if_necessary(ACCEL_RESTART_OOM);
			}
		}
	}
}

static zend_always_inline bool is_phar_file(zend_string *filename)
{
	return filename && ZSTR_LEN(filename) >= sizeof(".phar") &&
		!memcmp(ZSTR_VAL(filename) + ZSTR_LEN(filename) - (sizeof(".phar")-1), ".phar", sizeof(".phar")-1) &&
		!strstr(ZSTR_VAL(filename), "://");
}

static zend_persistent_script *store_script_in_file_cache(zend_persistent_script *new_persistent_script)
{
	uint32_t memory_used;

	zend_shared_alloc_init_xlat_table();

	/* Calculate the required memory size */
	memory_used = zend_accel_script_persist_calc(new_persistent_script, 0);

	/* Allocate memory block */
#if defined(__AVX__) || defined(__SSE2__)
	/* Align to 64-byte boundary */
	ZCG(mem) = zend_arena_alloc(&CG(arena), memory_used + 64);
	ZCG(mem) = (void*)(((uintptr_t)ZCG(mem) + 63L) & ~63L);
#elif ZEND_MM_NEED_EIGHT_BYTE_REALIGNMENT
	/* Align to 8-byte boundary */
	ZCG(mem) = zend_arena_alloc(&CG(arena), memory_used + 8);
	ZCG(mem) = (void*)(((uintptr_t)ZCG(mem) + 7L) & ~7L);
#else
	ZCG(mem) = zend_arena_alloc(&CG(arena), memory_used);
#endif

	zend_shared_alloc_clear_xlat_table();

	/* Copy into memory block */
	new_persistent_script = zend_accel_script_persist(new_persistent_script, 0);

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

	zend_file_cache_script_store(new_persistent_script, /* is_shm */ false);

	return new_persistent_script;
}

static zend_persistent_script *cache_script_in_file_cache(zend_persistent_script *new_persistent_script, bool *from_shared_memory)
{
	uint32_t orig_compiler_options;

	orig_compiler_options = CG(compiler_options);
	CG(compiler_options) |= ZEND_COMPILE_WITH_FILE_CACHE;
	zend_optimize_script(&new_persistent_script->script, ZCG(accel_directives).optimization_level, ZCG(accel_directives).opt_debug_level);
	zend_accel_finalize_delayed_early_binding_list(new_persistent_script);
	CG(compiler_options) = orig_compiler_options;

	*from_shared_memory = true;
	return store_script_in_file_cache(new_persistent_script);
}

static zend_persistent_script *cache_script_in_shared_memory(zend_persistent_script *new_persistent_script, zend_string *key, bool *from_shared_memory)
{
	zend_accel_hash_entry *bucket;
	uint32_t memory_used;
	uint32_t orig_compiler_options;

	orig_compiler_options = CG(compiler_options);
	if (ZCG(accel_directives).file_cache) {
		CG(compiler_options) |= ZEND_COMPILE_WITH_FILE_CACHE;
	}
	zend_optimize_script(&new_persistent_script->script, ZCG(accel_directives).optimization_level, ZCG(accel_directives).opt_debug_level);
	zend_accel_finalize_delayed_early_binding_list(new_persistent_script);
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
				zend_accel_add_key(key, bucket);
			}
			zend_shared_alloc_unlock();
#if 1
			/* prefer the script already stored in SHM */
			free_persistent_script(new_persistent_script, 1);
			*from_shared_memory = true;
			return existing_persistent_script;
#else
			return new_persistent_script;
#endif
		}
	}

	if (zend_accel_hash_is_full(&ZCSG(hash))) {
		zend_accel_error(ACCEL_LOG_DEBUG, "No more entries in hash table!");
		ZSMMG(memory_exhausted) = true;
		zend_accel_schedule_restart_if_necessary(ACCEL_RESTART_HASH);
		zend_shared_alloc_unlock();
		if (ZCG(accel_directives).file_cache) {
			new_persistent_script = store_script_in_file_cache(new_persistent_script);
			*from_shared_memory = true;
		}
		return new_persistent_script;
	}

	zend_shared_alloc_init_xlat_table();

	/* Calculate the required memory size */
	memory_used = zend_accel_script_persist_calc(new_persistent_script, 1);

	/* Allocate shared memory */
	ZCG(mem) = zend_shared_alloc_aligned(memory_used);
	if (!ZCG(mem)) {
		zend_shared_alloc_destroy_xlat_table();
		zend_accel_schedule_restart_if_necessary(ACCEL_RESTART_OOM);
		zend_shared_alloc_unlock();
		if (ZCG(accel_directives).file_cache) {
			new_persistent_script = store_script_in_file_cache(new_persistent_script);
			*from_shared_memory = true;
		}
		return new_persistent_script;
	}

	bzero_aligned(ZCG(mem), memory_used);

	zend_shared_alloc_clear_xlat_table();

	/* Copy into shared memory */
	new_persistent_script = zend_accel_script_persist(new_persistent_script, 1);

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

	/* store script structure in the hash table */
	bucket = zend_accel_hash_update(&ZCSG(hash), new_persistent_script->script.filename, 0, new_persistent_script);
	if (bucket) {
		zend_accel_error(ACCEL_LOG_INFO, "Cached script '%s'", ZSTR_VAL(new_persistent_script->script.filename));
		if (key &&
		    /* key may contain non-persistent PHAR aliases (see issues #115 and #149) */
		    !zend_string_starts_with_literal(key, "phar://") &&
		    !zend_string_equals(new_persistent_script->script.filename, key)) {
			/* link key to the same persistent script in hash table */
			zend_string *new_key = accel_new_interned_key(key);

			if (new_key) {
				if (zend_accel_hash_update(&ZCSG(hash), new_key, 1, bucket)) {
					zend_accel_error(ACCEL_LOG_INFO, "Added key '%s'", ZSTR_VAL(key));
				} else {
					zend_accel_error(ACCEL_LOG_DEBUG, "No more entries in hash table!");
					ZSMMG(memory_exhausted) = true;
					zend_accel_schedule_restart_if_necessary(ACCEL_RESTART_HASH);
				}
			} else {
				zend_accel_schedule_restart_if_necessary(ACCEL_RESTART_OOM);
			}
		}
	}

	new_persistent_script->dynamic_members.memory_consumption = ZEND_ALIGNED_SIZE(new_persistent_script->size);

	zend_shared_alloc_unlock();

	if (ZCG(accel_directives).file_cache) {
		SHM_PROTECT();
		zend_file_cache_script_store(new_persistent_script, /* is_shm */ true);
		SHM_UNPROTECT();
	}

	*from_shared_memory = true;
	return new_persistent_script;
}

#define ZEND_AUTOGLOBAL_MASK_SERVER  (1 << 0)
#define ZEND_AUTOGLOBAL_MASK_ENV     (1 << 1)
#define ZEND_AUTOGLOBAL_MASK_REQUEST (1 << 2)

static int zend_accel_get_auto_globals(void)
{
	int mask = 0;
	if (zend_hash_exists(&EG(symbol_table), ZSTR_KNOWN(ZEND_STR_AUTOGLOBAL_SERVER))) {
		mask |= ZEND_AUTOGLOBAL_MASK_SERVER;
	}
	if (zend_hash_exists(&EG(symbol_table), ZSTR_KNOWN(ZEND_STR_AUTOGLOBAL_ENV))) {
		mask |= ZEND_AUTOGLOBAL_MASK_ENV;
	}
	if (zend_hash_exists(&EG(symbol_table), ZSTR_KNOWN(ZEND_STR_AUTOGLOBAL_REQUEST))) {
		mask |= ZEND_AUTOGLOBAL_MASK_REQUEST;
	}
	return mask;
}

static void zend_accel_set_auto_globals(int mask)
{
	if (mask & ZEND_AUTOGLOBAL_MASK_SERVER) {
		zend_is_auto_global(ZSTR_KNOWN(ZEND_STR_AUTOGLOBAL_SERVER));
	}
	if (mask & ZEND_AUTOGLOBAL_MASK_ENV) {
		zend_is_auto_global(ZSTR_KNOWN(ZEND_STR_AUTOGLOBAL_ENV));
	}
	if (mask & ZEND_AUTOGLOBAL_MASK_REQUEST) {
		zend_is_auto_global(ZSTR_KNOWN(ZEND_STR_AUTOGLOBAL_REQUEST));
	}
	ZCG(auto_globals_mask) |= mask;
}

static void replay_warnings(uint32_t num_warnings, zend_error_info **warnings) {
	for (uint32_t i = 0; i < num_warnings; i++) {
		zend_error_info *warning = warnings[i];
		zend_error_zstr_at(warning->type, warning->filename, warning->lineno, warning->message);
	}
}

static zend_persistent_script *opcache_compile_file(zend_file_handle *file_handle, int type, zend_op_array **op_array_p)
{
	zend_persistent_script *new_persistent_script;
	uint32_t orig_functions_count, orig_class_count;
	zend_op_array *orig_active_op_array;
	zval orig_user_error_handler;
	zend_op_array *op_array;
	bool do_bailout = false;
	accel_time_t timestamp = 0;
	uint32_t orig_compiler_options = 0;

	/* Try to open file */
	if (file_handle->type == ZEND_HANDLE_FILENAME) {
		if (accelerator_orig_zend_stream_open_function(file_handle) != SUCCESS) {
			*op_array_p = NULL;
			if (!EG(exception)) {
				if (type == ZEND_REQUIRE) {
					zend_message_dispatcher(ZMSG_FAILED_REQUIRE_FOPEN, ZSTR_VAL(file_handle->filename));
				} else {
					zend_message_dispatcher(ZMSG_FAILED_INCLUDE_FOPEN, ZSTR_VAL(file_handle->filename));
				}
			}
			return NULL;
		}
	}

	/* check blacklist right after ensuring that file was opened */
	if (file_handle->opened_path && zend_accel_blacklist_is_blacklisted(&accel_blacklist, ZSTR_VAL(file_handle->opened_path), ZSTR_LEN(file_handle->opened_path))) {
		SHM_UNPROTECT();
		ZCSG(blacklist_misses)++;
		SHM_PROTECT();
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
			SHM_UNPROTECT();
			ZCSG(blacklist_misses)++;
			SHM_PROTECT();
			*op_array_p = accelerator_orig_compile_file(file_handle, type);
			return NULL;
		}
	}

	/* Save the original values for the op_array, function table and class table */
	orig_active_op_array = CG(active_op_array);
	orig_functions_count = EG(function_table)->nNumUsed;
	orig_class_count = EG(class_table)->nNumUsed;
	ZVAL_COPY_VALUE(&orig_user_error_handler, &EG(user_error_handler));

	/* Override them with ours */
	ZVAL_UNDEF(&EG(user_error_handler));
	if (ZCG(accel_directives).record_warnings) {
		zend_begin_record_errors();
	}

	zend_try {
		orig_compiler_options = CG(compiler_options);
		CG(compiler_options) |= ZEND_COMPILE_HANDLE_OP_ARRAY;
		CG(compiler_options) |= ZEND_COMPILE_IGNORE_INTERNAL_CLASSES;
		CG(compiler_options) |= ZEND_COMPILE_DELAYED_BINDING;
		CG(compiler_options) |= ZEND_COMPILE_NO_CONSTANT_SUBSTITUTION;
		CG(compiler_options) |= ZEND_COMPILE_IGNORE_OTHER_FILES;
		CG(compiler_options) |= ZEND_COMPILE_IGNORE_OBSERVER;
		if (ZCG(accel_directives).file_cache) {
			CG(compiler_options) |= ZEND_COMPILE_WITH_FILE_CACHE;
		}
		op_array = *op_array_p = accelerator_orig_compile_file(file_handle, type);
		CG(compiler_options) = orig_compiler_options;
	} zend_catch {
		op_array = NULL;
		do_bailout = true;
		CG(compiler_options) = orig_compiler_options;
	} zend_end_try();

	/* Restore originals */
	CG(active_op_array) = orig_active_op_array;
	EG(user_error_handler) = orig_user_error_handler;
	EG(record_errors) = 0;

	if (!op_array) {
		/* compilation failed */
		zend_free_recorded_errors();
		if (do_bailout) {
			zend_bailout();
		}
		return NULL;
	}

	/* Build the persistent_script structure.
	   Here we aren't sure we would store it, but we will need it
	   further anyway.
	*/
	new_persistent_script = create_persistent_script();
	new_persistent_script->script.main_op_array = *op_array;
	zend_accel_move_user_functions(CG(function_table), CG(function_table)->nNumUsed - orig_functions_count, &new_persistent_script->script);
	zend_accel_move_user_classes(CG(class_table), CG(class_table)->nNumUsed - orig_class_count, &new_persistent_script->script);
	zend_accel_build_delayed_early_binding_list(new_persistent_script);
	new_persistent_script->num_warnings = EG(num_errors);
	new_persistent_script->warnings = EG(errors);
	EG(num_errors) = 0;
	EG(errors) = NULL;

	efree(op_array); /* we have valid persistent_script, so it's safe to free op_array */

	/* Fill in the ping_auto_globals_mask for the new script. If jit for auto globals is enabled we
	   will have to ping the used auto global variables before execution */
	if (PG(auto_globals_jit)) {
		new_persistent_script->ping_auto_globals_mask = zend_accel_get_auto_globals();
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
		new_persistent_script->script.filename = zend_string_copy(file_handle->filename);
	}
	zend_string_hash_val(new_persistent_script->script.filename);

	/* Now persistent_script structure is ready in process memory */
	return new_persistent_script;
}

zend_op_array *file_cache_compile_file(zend_file_handle *file_handle, int type)
{
	zend_persistent_script *persistent_script;
	zend_op_array *op_array = NULL;
	bool from_memory; /* if the script we've got is stored in SHM */

	if (php_is_stream_path(ZSTR_VAL(file_handle->filename)) &&
	    !is_cacheable_stream_path(ZSTR_VAL(file_handle->filename))) {
		return accelerator_orig_compile_file(file_handle, type);
	}

	if (!file_handle->opened_path) {
		if (file_handle->type == ZEND_HANDLE_FILENAME &&
		    accelerator_orig_zend_stream_open_function(file_handle) == FAILURE) {
			if (!EG(exception)) {
				if (type == ZEND_REQUIRE) {
					zend_message_dispatcher(ZMSG_FAILED_REQUIRE_FOPEN, ZSTR_VAL(file_handle->filename));
				} else {
					zend_message_dispatcher(ZMSG_FAILED_INCLUDE_FOPEN, ZSTR_VAL(file_handle->filename));
				}
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
		replay_warnings(persistent_script->num_warnings, persistent_script->warnings);

	    if (persistent_script->ping_auto_globals_mask & ~ZCG(auto_globals_mask)) {
			zend_accel_set_auto_globals(persistent_script->ping_auto_globals_mask & ~ZCG(auto_globals_mask));
		}

		return zend_accel_load_script(persistent_script, 1);
	}

	persistent_script = opcache_compile_file(file_handle, type, &op_array);

	if (persistent_script) {
		from_memory = false;
		persistent_script = cache_script_in_file_cache(persistent_script, &from_memory);
		return zend_accel_load_script(persistent_script, from_memory);
	}

	return op_array;
}

static int check_persistent_script_access(zend_persistent_script *persistent_script)
{
	char *phar_path, *ptr;
	if ((ZSTR_LEN(persistent_script->script.filename)<sizeof("phar://.phar")) ||
	    memcmp(ZSTR_VAL(persistent_script->script.filename), "phar://", sizeof("phar://")-1)) {

		return access(ZSTR_VAL(persistent_script->script.filename), R_OK) != 0;

	} else {
		/* we got a cached file from .phar, so we have to strip prefix and path inside .phar to check access() */
		phar_path = estrdup(ZSTR_VAL(persistent_script->script.filename)+sizeof("phar://")-1);
		if ((ptr = strstr(phar_path, ".phar/")) != NULL)
		{
			*(ptr+sizeof(".phar/")-2) = 0; /* strip path inside .phar file */
		}
		bool ret = access(phar_path, R_OK) != 0;
		efree(phar_path);
		return ret;
	}
}

/* zend_compile() replacement */
zend_op_array *persistent_compile_file(zend_file_handle *file_handle, int type)
{
	zend_persistent_script *persistent_script = NULL;
	zend_string *key = NULL;
	bool from_shared_memory; /* if the script we've got is stored in SHM */

	if (!file_handle->filename || !ZCG(accelerator_enabled)) {
		/* The Accelerator is disabled, act as if without the Accelerator */
		ZCG(cache_opline) = NULL;
		ZCG(cache_persistent_script) = NULL;
		if (file_handle->filename
		 && ZCG(accel_directives).file_cache
		 && ZCG(enabled) && accel_startup_ok) {
			return file_cache_compile_file(file_handle, type);
		}
		return accelerator_orig_compile_file(file_handle, type);
	} else if (file_cache_only) {
		ZCG(cache_opline) = NULL;
		ZCG(cache_persistent_script) = NULL;
		return file_cache_compile_file(file_handle, type);
	} else if ((ZCSG(restart_in_progress) && accel_restart_is_active())) {
		if (ZCG(accel_directives).file_cache) {
			return file_cache_compile_file(file_handle, type);
		}
		ZCG(cache_opline) = NULL;
		ZCG(cache_persistent_script) = NULL;
		return accelerator_orig_compile_file(file_handle, type);
	}

	/* In case this callback is called from include_once, require_once or it's
	 * a main FastCGI request, the key must be already calculated, and cached
	 * persistent script already found */
	if (ZCG(cache_persistent_script) &&
	    ((!EG(current_execute_data) &&
	      file_handle->primary_script &&
	      ZCG(cache_opline) == NULL) ||
	     (EG(current_execute_data) &&
	      EG(current_execute_data)->func &&
	      ZEND_USER_CODE(EG(current_execute_data)->func->common.type) &&
	      ZCG(cache_opline) == EG(current_execute_data)->opline))) {

		persistent_script = ZCG(cache_persistent_script);
		if (ZSTR_LEN(&ZCG(key))) {
			key = &ZCG(key);
		}

	} else {
		if (!ZCG(accel_directives).revalidate_path) {
			/* try to find cached script by key */
			key = accel_make_persistent_key(file_handle->filename);
			if (!key) {
				ZCG(cache_opline) = NULL;
				ZCG(cache_persistent_script) = NULL;
				return accelerator_orig_compile_file(file_handle, type);
			}
			persistent_script = zend_accel_hash_find(&ZCSG(hash), key);
		} else if (UNEXPECTED(php_is_stream_path(ZSTR_VAL(file_handle->filename)) && !is_cacheable_stream_path(ZSTR_VAL(file_handle->filename)))) {
			ZCG(cache_opline) = NULL;
			ZCG(cache_persistent_script) = NULL;
			return accelerator_orig_compile_file(file_handle, type);
		}

		if (!persistent_script) {
			/* try to find cached script by full real path */
			zend_accel_hash_entry *bucket;

			/* open file to resolve the path */
		    if (file_handle->type == ZEND_HANDLE_FILENAME
		     && accelerator_orig_zend_stream_open_function(file_handle) == FAILURE) {
				if (!EG(exception)) {
					if (type == ZEND_REQUIRE) {
						zend_message_dispatcher(ZMSG_FAILED_REQUIRE_FOPEN, ZSTR_VAL(file_handle->filename));
					} else {
						zend_message_dispatcher(ZMSG_FAILED_INCLUDE_FOPEN, ZSTR_VAL(file_handle->filename));
					}
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
						zend_accel_add_key(key, bucket);
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
			if (ZCG(accel_directives).file_cache) {
				return file_cache_compile_file(file_handle, type);
			}
			return accelerator_orig_compile_file(file_handle, type);
		}
		ZCG(counted) = true;
	}

	/* Revalidate accessibility of cached file */
	if (EXPECTED(persistent_script != NULL) &&
	    UNEXPECTED(ZCG(accel_directives).validate_permission) &&
	    file_handle->type == ZEND_HANDLE_FILENAME &&
	    UNEXPECTED(check_persistent_script_access(persistent_script))) {
		if (!EG(exception)) {
			if (type == ZEND_REQUIRE) {
				zend_message_dispatcher(ZMSG_FAILED_REQUIRE_FOPEN, ZSTR_VAL(file_handle->filename));
			} else {
				zend_message_dispatcher(ZMSG_FAILED_INCLUDE_FOPEN, ZSTR_VAL(file_handle->filename));
			}
		}
		return NULL;
	}

	HANDLE_BLOCK_INTERRUPTIONS();
	SHM_UNPROTECT();

	/* If script is found then validate_timestamps if option is enabled */
	if (persistent_script && ZCG(accel_directives).validate_timestamps) {
		if (validate_timestamp_and_record(persistent_script, file_handle) == FAILURE) {
			zend_accel_lock_discard_script(persistent_script);
			persistent_script = NULL;
		}
	}

	/* Check the second level cache */
	if (!persistent_script && ZCG(accel_directives).file_cache) {
		persistent_script = zend_file_cache_script_load(file_handle);
	}

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
			if (ZCG(accel_directives).file_cache) {
				return file_cache_compile_file(file_handle, type);
			}
			return accelerator_orig_compile_file(file_handle, type);
		}

		SHM_PROTECT();
		HANDLE_UNBLOCK_INTERRUPTIONS();
		persistent_script = opcache_compile_file(file_handle, type, &op_array);
		HANDLE_BLOCK_INTERRUPTIONS();
		SHM_UNPROTECT();

		/* Try and cache the script and assume that it is returned from_shared_memory.
		 * If it isn't compile_and_cache_file() changes the flag to 0
		 */
		from_shared_memory = false;
		if (persistent_script) {
			persistent_script = cache_script_in_shared_memory(persistent_script, key, &from_shared_memory);
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
		persistent_script->dynamic_members.last_used = ZCG(request_time);
		SHM_PROTECT();
		HANDLE_UNBLOCK_INTERRUPTIONS();
	} else {

#if !ZEND_WIN32
		ZCSG(hits)++; /* TBFixed: may lose one hit */
		persistent_script->dynamic_members.hits++; /* see above */
#else
#if ZEND_ENABLE_ZVAL_LONG64
		InterlockedIncrement64(&ZCSG(hits));
		InterlockedIncrement64(&persistent_script->dynamic_members.hits);
#else
		InterlockedIncrement(&ZCSG(hits));
		InterlockedIncrement(&persistent_script->dynamic_members.hits);
#endif
#endif

		/* see bug #15471 (old BTS) */
		if (persistent_script->script.filename) {
			if (!EG(current_execute_data) ||
			    !EG(current_execute_data)->func ||
			    !ZEND_USER_CODE(EG(current_execute_data)->func->common.type) ||
			    !EG(current_execute_data)->opline ||
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
		persistent_script->dynamic_members.last_used = ZCG(request_time);
		SHM_PROTECT();
		HANDLE_UNBLOCK_INTERRUPTIONS();

		replay_warnings(persistent_script->num_warnings, persistent_script->warnings);
		from_shared_memory = true;
	}

	/* Fetch jit auto globals used in the script before execution */
	if (persistent_script->ping_auto_globals_mask & ~ZCG(auto_globals_mask)) {
		zend_accel_set_auto_globals(persistent_script->ping_auto_globals_mask & ~ZCG(auto_globals_mask));
	}

	return zend_accel_load_script(persistent_script, from_shared_memory);
}

static zend_always_inline zend_inheritance_cache_entry* zend_accel_inheritance_cache_find(zend_inheritance_cache_entry *entry, zend_class_entry *ce, zend_class_entry *parent, zend_class_entry **traits_and_interfaces, bool *needs_autoload_ptr)
{
	uint32_t i;

	ZEND_ASSERT(ce->ce_flags & ZEND_ACC_IMMUTABLE);
	ZEND_ASSERT(!(ce->ce_flags & ZEND_ACC_LINKED));

	while (entry) {
		bool found = true;
		bool needs_autoload = false;

		if (entry->parent != parent) {
			found = false;
		} else {
			for (i = 0; i < ce->num_traits + ce->num_interfaces; i++) {
				if (entry->traits_and_interfaces[i] != traits_and_interfaces[i]) {
					found = false;
					break;
				}
			}
			if (found && entry->dependencies) {
				for (i = 0; i < entry->dependencies_count; i++) {
					zend_class_entry *ce = zend_lookup_class_ex(entry->dependencies[i].name, NULL, ZEND_FETCH_CLASS_NO_AUTOLOAD);

					if (ce != entry->dependencies[i].ce) {
						if (!ce) {
							needs_autoload = true;
						} else {
							found = false;
							break;
						}
					}
				}
			}
		}
		if (found) {
			*needs_autoload_ptr = needs_autoload;
			return entry;
		}
		entry = entry->next;
	}

	return NULL;
}

static zend_class_entry* zend_accel_inheritance_cache_get(zend_class_entry *ce, zend_class_entry *parent, zend_class_entry **traits_and_interfaces)
{
	uint32_t i;
	bool needs_autoload;
	zend_inheritance_cache_entry *entry = ce->inheritance_cache;

	while (entry) {
		entry = zend_accel_inheritance_cache_find(entry, ce, parent, traits_and_interfaces, &needs_autoload);
		if (entry) {
			if (!needs_autoload) {
				replay_warnings(entry->num_warnings, entry->warnings);
				if (ZCSG(map_ptr_last) > CG(map_ptr_last)) {
					zend_map_ptr_extend(ZCSG(map_ptr_last));
				}
				ce = entry->ce;
				if (ZSTR_HAS_CE_CACHE(ce->name)) {
					ZSTR_SET_CE_CACHE_EX(ce->name, ce, 0);
				}
				return ce;
			}

			for (i = 0; i < entry->dependencies_count; i++) {
				zend_class_entry *ce = zend_lookup_class_ex(entry->dependencies[i].name, NULL, 0);

				if (ce == NULL) {
					return NULL;
				}
			}
		}
	}

	return NULL;
}

static zend_class_entry* zend_accel_inheritance_cache_add(zend_class_entry *ce, zend_class_entry *proto, zend_class_entry *parent, zend_class_entry **traits_and_interfaces, HashTable *dependencies)
{
	zend_persistent_script dummy;
	size_t size;
	uint32_t i;
	bool needs_autoload;
	zend_class_entry *new_ce;
	zend_inheritance_cache_entry *entry;

	ZEND_ASSERT(!(ce->ce_flags & ZEND_ACC_IMMUTABLE));
	ZEND_ASSERT(ce->ce_flags & ZEND_ACC_LINKED);

	if (!ZCG(accelerator_enabled) ||
	    (ZCSG(restart_in_progress) && accel_restart_is_active())) {
		return NULL;
	}

	if (traits_and_interfaces && dependencies) {
		for (i = 0; i < proto->num_traits + proto->num_interfaces; i++) {
			if (traits_and_interfaces[i]) {
				zend_hash_del(dependencies, traits_and_interfaces[i]->name);
			}
		}
	}

	SHM_UNPROTECT();
	zend_shared_alloc_lock();

	entry = proto->inheritance_cache;
	while (entry) {
		entry = zend_accel_inheritance_cache_find(entry, proto, parent, traits_and_interfaces, &needs_autoload);
		if (entry) {
			zend_shared_alloc_unlock();
			SHM_PROTECT();
			if (!needs_autoload) {
				zend_map_ptr_extend(ZCSG(map_ptr_last));
				return entry->ce;
			} else {
				return NULL;
			}
		}
	}

	zend_shared_alloc_init_xlat_table();

	memset(&dummy, 0, sizeof(dummy));
	dummy.size = ZEND_ALIGNED_SIZE(
		sizeof(zend_inheritance_cache_entry) -
		sizeof(void*) +
		(sizeof(void*) * (proto->num_traits + proto->num_interfaces)));
	if (dependencies) {
		dummy.size += ZEND_ALIGNED_SIZE(zend_hash_num_elements(dependencies) * sizeof(zend_class_dependency));
	}
	ZCG(current_persistent_script) = &dummy;
	zend_persist_class_entry_calc(ce);
	zend_persist_warnings_calc(EG(num_errors), EG(errors));
	size = dummy.size;

	zend_shared_alloc_clear_xlat_table();

#if ZEND_MM_NEED_EIGHT_BYTE_REALIGNMENT
	/* Align to 8-byte boundary */
	ZCG(mem) = zend_shared_alloc(size + 8);
#else
	ZCG(mem) = zend_shared_alloc(size);
#endif

	if (!ZCG(mem)) {
		zend_shared_alloc_destroy_xlat_table();
		zend_shared_alloc_unlock();
		SHM_PROTECT();
		return NULL;
	}

	zend_map_ptr_extend(ZCSG(map_ptr_last));

#if ZEND_MM_NEED_EIGHT_BYTE_REALIGNMENT
	/* Align to 8-byte boundary */
	ZCG(mem) = (void*)(((uintptr_t)ZCG(mem) + 7L) & ~7L);
#endif

	memset(ZCG(mem), 0, size);
	entry = (zend_inheritance_cache_entry*)ZCG(mem);
	ZCG(mem) = (char*)ZCG(mem) +
		ZEND_ALIGNED_SIZE(
			(sizeof(zend_inheritance_cache_entry) -
			 sizeof(void*) +
			 (sizeof(void*) * (proto->num_traits + proto->num_interfaces))));
	entry->parent = parent;
	for (i = 0; i < proto->num_traits + proto->num_interfaces; i++) {
		entry->traits_and_interfaces[i] = traits_and_interfaces[i];
	}
	if (dependencies && zend_hash_num_elements(dependencies)) {
		zend_string *dep_name;
		zend_class_entry *dep_ce;

		i = 0;
		entry->dependencies_count = zend_hash_num_elements(dependencies);
		entry->dependencies = (zend_class_dependency*)ZCG(mem);
		ZEND_HASH_MAP_FOREACH_STR_KEY_PTR(dependencies, dep_name, dep_ce) {
#if ZEND_DEBUG
			ZEND_ASSERT(zend_accel_in_shm(dep_name));
#endif
			entry->dependencies[i].name = dep_name;
			entry->dependencies[i].ce = dep_ce;
			i++;
		} ZEND_HASH_FOREACH_END();
		ZCG(mem) = (char*)ZCG(mem) + zend_hash_num_elements(dependencies) * sizeof(zend_class_dependency);
	}
	entry->ce = new_ce = zend_persist_class_entry(ce);
	zend_update_parent_ce(new_ce);

	entry->num_warnings = EG(num_errors);
	entry->warnings = zend_persist_warnings(EG(num_errors), EG(errors));
	entry->next = proto->inheritance_cache;
	proto->inheritance_cache = entry;

	EG(num_errors) = 0;
	EG(errors) = NULL;

	ZCSG(map_ptr_last) = CG(map_ptr_last);

	zend_shared_alloc_destroy_xlat_table();

	zend_shared_alloc_unlock();
	SHM_PROTECT();

	/* Consistency check */
	if ((char*)entry + size != (char*)ZCG(mem)) {
		zend_accel_error(
			((char*)entry + size < (char*)ZCG(mem)) ? ACCEL_LOG_ERROR : ACCEL_LOG_WARNING,
			"Internal error: wrong class size calculation: %s start=" ZEND_ADDR_FMT ", end=" ZEND_ADDR_FMT ", real=" ZEND_ADDR_FMT "\n",
			ZSTR_VAL(ce->name),
			(size_t)entry,
			(size_t)((char *)entry + size),
			(size_t)ZCG(mem));
	}

	zend_map_ptr_extend(ZCSG(map_ptr_last));

	return new_ce;
}

#ifdef ZEND_WIN32
static zend_result accel_gen_uname_id(void)
{
	PHP_MD5_CTX ctx;
	unsigned char digest[16];
	wchar_t uname[UNLEN + 1];
	DWORD unsize = UNLEN;

	if (!GetUserNameW(uname, &unsize)) {
		return FAILURE;
	}
	PHP_MD5Init(&ctx);
	PHP_MD5Update(&ctx, (void *) uname, (unsize - 1) * sizeof(wchar_t));
	PHP_MD5Update(&ctx, ZCG(accel_directives).cache_id, strlen(ZCG(accel_directives).cache_id));
	PHP_MD5Final(digest, &ctx);
	php_hash_bin2hex(accel_uname_id, digest, sizeof digest);
	return SUCCESS;
}
#endif

/* zend_stream_open_function() replacement for PHP 5.3 and above */
static zend_result persistent_stream_open_function(zend_file_handle *handle)
{
	if (ZCG(cache_persistent_script)) {
		/* check if callback is called from include_once or it's a main request */
		if ((!EG(current_execute_data) &&
		     handle->primary_script &&
		     ZCG(cache_opline) == NULL) ||
		    (EG(current_execute_data) &&
		     EG(current_execute_data)->func &&
		     ZEND_USER_CODE(EG(current_execute_data)->func->common.type) &&
		     ZCG(cache_opline) == EG(current_execute_data)->opline)) {

			/* we are in include_once or FastCGI request */
			handle->opened_path = zend_string_copy(ZCG(cache_persistent_script)->script.filename);
			return SUCCESS;
		}
		ZCG(cache_opline) = NULL;
		ZCG(cache_persistent_script) = NULL;
	}
	return accelerator_orig_zend_stream_open_function(handle);
}

/* zend_resolve_path() replacement for PHP 5.3 and above */
static zend_string* persistent_zend_resolve_path(zend_string *filename)
{
	if (!file_cache_only &&
	    ZCG(accelerator_enabled)) {

		/* check if callback is called from include_once or it's a main request */
		if ((!EG(current_execute_data)) ||
		    (EG(current_execute_data) &&
		     EG(current_execute_data)->func &&
		     ZEND_USER_CODE(EG(current_execute_data)->func->common.type) &&
		     EG(current_execute_data)->opline->opcode == ZEND_INCLUDE_OR_EVAL &&
		     (EG(current_execute_data)->opline->extended_value == ZEND_INCLUDE_ONCE ||
		      EG(current_execute_data)->opline->extended_value == ZEND_REQUIRE_ONCE))) {

			/* we are in include_once or FastCGI request */
			zend_string *resolved_path;
			zend_string *key = NULL;

			if (!ZCG(accel_directives).revalidate_path) {
				/* lookup by "not-real" path */
				key = accel_make_persistent_key(filename);
				if (key) {
					zend_accel_hash_entry *bucket = zend_accel_hash_find_entry(&ZCSG(hash), key);
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
					return accelerator_orig_zend_resolve_path(filename);
				}
			}

			/* find the full real path */
			resolved_path = accelerator_orig_zend_resolve_path(filename);

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
							zend_accel_add_key(key, bucket);
							zend_shared_alloc_unlock();
							SHM_PROTECT();
							HANDLE_UNBLOCK_INTERRUPTIONS();
						} else {
							ZSTR_LEN(&ZCG(key)) = 0;
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
	return accelerator_orig_zend_resolve_path(filename);
}

static void zend_reset_cache_vars(void)
{
	ZSMMG(memory_exhausted) = false;
	ZCSG(hits) = 0;
	ZCSG(misses) = 0;
	ZCSG(blacklist_misses) = 0;
	ZSMMG(wasted_shared_memory) = 0;
	ZCSG(restart_pending) = false;
	ZCSG(force_restart_time) = 0;
	ZCSG(map_ptr_last) = CG(map_ptr_last);
}

static void accel_reset_pcre_cache(void)
{
	Bucket *p;

	if (PCRE_G(per_request_cache)) {
		return;
	}

	ZEND_HASH_MAP_FOREACH_BUCKET(&PCRE_G(pcre_cache), p) {
		/* Remove PCRE cache entries with inconsistent keys */
		if (zend_accel_in_shm(p->key)) {
			p->key = NULL;
			zend_hash_del_bucket(&PCRE_G(pcre_cache), p);
		}
	} ZEND_HASH_FOREACH_END();
}

zend_result accel_activate(INIT_FUNC_ARGS)
{
	if (!ZCG(enabled) || !accel_startup_ok) {
		ZCG(accelerator_enabled) = false;
		return SUCCESS;
	}

	/* PHP-5.4 and above return "double", but we use 1 sec precision */
	ZCG(auto_globals_mask) = 0;
	ZCG(request_time) = (time_t)sapi_get_request_time();
	ZCG(cache_opline) = NULL;
	ZCG(cache_persistent_script) = NULL;
	ZCG(include_path_key_len) = 0;
	ZCG(include_path_check) = true;

	ZCG(cwd) = NULL;
	ZCG(cwd_key_len) = 0;
	ZCG(cwd_check) = true;

	if (file_cache_only) {
		ZCG(accelerator_enabled) = false;
		return SUCCESS;
	}

#ifndef ZEND_WIN32
	if (ZCG(accel_directives).validate_root) {
		struct stat buf;

		if (stat("/", &buf) != 0) {
			ZCG(root_hash) = 0;
		} else {
			ZCG(root_hash) = buf.st_ino;
			if (sizeof(buf.st_ino) > sizeof(ZCG(root_hash))) {
				if (ZCG(root_hash) != buf.st_ino) {
					zend_string *key = ZSTR_INIT_LITERAL("opcache.enable", 0);
					zend_alter_ini_entry_chars(key, "0", 1, ZEND_INI_SYSTEM, ZEND_INI_STAGE_RUNTIME);
					zend_string_release_ex(key, 0);
					zend_accel_error(ACCEL_LOG_WARNING, "Can't cache files in chroot() directory with too big inode");
					return SUCCESS;
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
		ZCG(counted) = false;
	}

	if (ZCSG(restart_pending)) {
		zend_shared_alloc_lock();
		if (ZCSG(restart_pending)) { /* check again, to ensure that the cache wasn't already cleaned by another process */
			if (accel_is_inactive()) {
				zend_accel_error(ACCEL_LOG_DEBUG, "Restarting!");
				ZCSG(restart_pending) = false;
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

				zend_map_ptr_reset();
				zend_reset_cache_vars();
				zend_accel_hash_clean(&ZCSG(hash));

				if (ZCG(accel_directives).interned_strings_buffer) {
					accel_interned_strings_restore_state();
				}

				zend_shared_alloc_restore_state();
				if (ZCSG(preload_script)) {
					preload_restart();
				}

#ifdef HAVE_JIT
				zend_jit_restart();
#endif

				ZCSG(accelerator_enabled) = ZCSG(cache_status_before_restart);
				if (ZCSG(last_restart_time) < ZCG(request_time)) {
					ZCSG(last_restart_time) = ZCG(request_time);
				} else {
					ZCSG(last_restart_time)++;
				}
				accel_restart_leave();
			}
		}
		zend_shared_alloc_unlock();
	}

	ZCG(accelerator_enabled) = ZCSG(accelerator_enabled);

	SHM_PROTECT();
	HANDLE_UNBLOCK_INTERRUPTIONS();

	if (ZCG(accelerator_enabled) && ZCSG(last_restart_time) != ZCG(last_restart_time)) {
		/* SHM was reinitialized. */
		ZCG(last_restart_time) = ZCSG(last_restart_time);

		/* Reset in-process realpath cache */
		realpath_cache_clean();

		accel_reset_pcre_cache();
		ZCG(pcre_reseted) = false;
	} else if (!ZCG(accelerator_enabled) && !ZCG(pcre_reseted)) {
		accel_reset_pcre_cache();
		ZCG(pcre_reseted) = true;
	}


#ifdef HAVE_JIT
	zend_jit_activate();
#endif

	if (ZCSG(preload_script)) {
		preload_activate();
	}

	return SUCCESS;
}

#ifdef HAVE_JIT
void accel_deactivate(void)
{
	zend_jit_deactivate();
}
#endif

zend_result accel_post_deactivate(void)
{
	if (ZCG(cwd)) {
		zend_string_release_ex(ZCG(cwd), 0);
		ZCG(cwd) = NULL;
	}

	if (!ZCG(enabled) || !accel_startup_ok) {
		return SUCCESS;
	}

	zend_shared_alloc_safe_unlock(); /* be sure we didn't leave cache locked */
	accel_unlock_all();
	ZCG(counted) = false;

	return SUCCESS;
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

static void zps_startup_failure(const char *reason, const char *api_reason, int (*cb)(zend_extension *, zend_extension *))
{
	accel_startup_ok = false;
	zps_failure_reason = reason;
	zps_api_failure_reason = api_reason?api_reason:reason;
	zend_llist_del_element(&zend_extensions, NULL, (int (*)(void *, void *))cb);
}

static inline zend_result accel_find_sapi(void)
{
	static const char *supported_sapis[] = {
		"apache",
		"fastcgi",
		"cli-server",
		"cgi-fcgi",
		"fpm-fcgi",
		"fpmi-fcgi",
		"apache2handler",
		"litespeed",
		"uwsgi",
		"fuzzer",
		"frankenphp",
		"ngx-php",
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

static zend_result zend_accel_init_shm(void)
{
	int i;
	size_t accel_shared_globals_size;

	zend_shared_alloc_lock();

	if (ZCG(accel_directives).interned_strings_buffer) {
		accel_shared_globals_size = sizeof(zend_accel_shared_globals) + ZCG(accel_directives).interned_strings_buffer * 1024 * 1024;
	} else {
		/* Make sure there is always at least one interned string hash slot,
		 * so the table can be queried unconditionally. */
		accel_shared_globals_size = sizeof(zend_accel_shared_globals) + sizeof(uint32_t);
	}

	accel_shared_globals = zend_shared_alloc(accel_shared_globals_size);
	if (!accel_shared_globals) {
		zend_shared_alloc_unlock();
		zend_accel_error_noreturn(ACCEL_LOG_FATAL,
				"Insufficient shared memory for interned strings buffer! (tried to allocate %zu bytes)",
				accel_shared_globals_size);
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
			(zend_string*)((char*)(accel_shared_globals + 1) + /* table data is stored after accel_shared_globals */
				ZCG(accel_directives).interned_strings_buffer * 1024 * 1024);
		ZCSG(interned_strings).saved_top = NULL;

		memset((char*)&ZCSG(interned_strings) + sizeof(zend_string_table),
			STRTAB_INVALID_POS,
			(char*)ZCSG(interned_strings).start -
				((char*)&ZCSG(interned_strings) + sizeof(zend_string_table)));
	} else {
		*STRTAB_HASH_TO_SLOT(&ZCSG(interned_strings), 0) = STRTAB_INVALID_POS;
	}

	/* We can reuse init_interned_string_for_php for the "init_existing_interned" case,
	 * because the function does not create new interned strings at runtime. */
	zend_interned_strings_set_request_storage_handlers(
		accel_new_interned_string_for_php,
		accel_init_interned_string_for_php,
		accel_init_interned_string_for_php);

	zend_reset_cache_vars();

	ZCSG(oom_restarts) = 0;
	ZCSG(hash_restarts) = 0;
	ZCSG(manual_restarts) = 0;

	ZCSG(accelerator_enabled) = true;
	ZCSG(start_time) = zend_accel_get_time();
	ZCSG(last_restart_time) = 0;
	ZCSG(restart_in_progress) = false;

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
#  ifdef MAP_ALIGNED_SUPER
#   include <sys/types.h>
#   include <sys/sysctl.h>
#   include <sys/user.h>
#   define MAP_HUGETLB MAP_ALIGNED_SUPER
#  endif
# endif

# if defined(MAP_HUGETLB) || defined(MADV_HUGEPAGE)
static zend_result accel_remap_huge_pages(void *start, size_t size, size_t real_size, const char *name, size_t offset)
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
		return FAILURE;
	}
	memcpy(mem, start, real_size);

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
			memcpy(start, mem, real_size);
			mprotect(start, size, PROT_READ | PROT_EXEC);
			munmap(mem, size);
			zend_error(E_WARNING,
				ACCELERATOR_PRODUCT_NAME " huge_code_pages: madvise(HUGEPAGE) failed: %s (%d)",
				strerror(errno), errno);
			return FAILURE;
		}
#  else
		memcpy(start, mem, real_size);
		mprotect(start, size, PROT_READ | PROT_EXEC);
		munmap(mem, size);
		zend_error(E_WARNING,
			ACCELERATOR_PRODUCT_NAME " huge_code_pages: mmap(HUGETLB) failed: %s (%d)",
			strerror(errno), errno);
		return FAILURE;
#  endif
	}

	// Given the MAP_FIXED flag the address can never diverge
	ZEND_ASSERT(ret == start);
	zend_mmap_set_name(start, size, "zend_huge_code_pages");
	memcpy(start, mem, real_size);
	mprotect(start, size, PROT_READ | PROT_EXEC);

	munmap(mem, size);

	return SUCCESS;
}

static void accel_move_code_to_huge_pages(void)
{
#if defined(__linux__)
	FILE *f;
	long unsigned int huge_page_size = 2 * 1024 * 1024;

	f = fopen("/proc/self/maps", "r");
	if (f) {
		long unsigned int  start, end, offset, inode;
		char perm[5], dev[10], name[MAXPATHLEN];
		int ret;

		while (1) {
			ret = fscanf(f, "%lx-%lx %4s %lx %9s %lu %s\n", &start, &end, perm, &offset, dev, &inode, name);
			if (ret == 7) {
				if (perm[0] == 'r' && perm[1] == '-' && perm[2] == 'x' && name[0] == '/') {
					long unsigned int  seg_start = ZEND_MM_ALIGNED_SIZE_EX(start, huge_page_size);
					long unsigned int  seg_end = (end & ~(huge_page_size-1L));
					long unsigned int  real_end;

					ret = fscanf(f, "%lx-", &start);
					if (ret == 1 && start == seg_end + huge_page_size) {
						real_end = end;
						seg_end = start;
					} else {
						real_end = seg_end;
					}

					if (seg_end > seg_start) {
						zend_accel_error(ACCEL_LOG_DEBUG, "remap to huge page %lx-%lx %s \n", seg_start, seg_end, name);
						accel_remap_huge_pages((void*)seg_start, seg_end - seg_start, real_end - seg_start, name, offset + seg_start - start);
					}
					break;
				}
			} else {
				break;
			}
		}
		fclose(f);
	}
#elif defined(__FreeBSD__)
	size_t s = 0;
	int mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_VMMAP, getpid()};
	long unsigned int huge_page_size = 2 * 1024 * 1024;
	if (sysctl(mib, 4, NULL, &s, NULL, 0) == 0) {
		s = s * 4 / 3;
		void *addr = mmap(NULL, s, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
		if (addr != MAP_FAILED) {
			if (sysctl(mib, 4, addr, &s, NULL, 0) == 0) {
				uintptr_t start = (uintptr_t)addr;
				uintptr_t end = start + s;
				while (start < end) {
					struct kinfo_vmentry *entry = (struct kinfo_vmentry *)start;
					size_t sz = entry->kve_structsize;
					if (sz == 0) {
						break;
					}
					int permflags = entry->kve_protection;
					if ((permflags & KVME_PROT_READ) && !(permflags & KVME_PROT_WRITE) &&
					    (permflags & KVME_PROT_EXEC) && entry->kve_path[0] != '\0') {
						long unsigned int seg_start = ZEND_MM_ALIGNED_SIZE_EX(start, huge_page_size);
						long unsigned int seg_end = (end & ~(huge_page_size-1L));
						if (seg_end > seg_start) {
							zend_accel_error(ACCEL_LOG_DEBUG, "remap to huge page %lx-%lx %s \n", seg_start, seg_end, entry->kve_path);
							accel_remap_huge_pages((void*)seg_start, seg_end - seg_start, seg_end - seg_start, entry->kve_path, entry->kve_offset + seg_start - start);
							// First relevant segment found is our binary
							break;
						}
					}
					start += sz;
				}
			}
			munmap(addr, s);
		}
	}
#endif
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
	accel_globals_id = ts_allocate_id(&accel_globals_id, sizeof(zend_accel_globals), (ts_allocate_ctor) accel_globals_ctor, NULL);
#else
	accel_globals_ctor(&accel_globals);
#endif

#ifdef HAVE_JIT
	zend_jit_init();
#endif

#ifdef ZEND_WIN32
# if !defined(__has_feature) || !__has_feature(address_sanitizer)
	_setmaxstdio(2048); /* The default configuration is limited to 512 stdio files */
# endif
#endif

	if (start_accel_module() == FAILURE) {
		accel_startup_ok = false;
		zend_error(E_WARNING, ACCELERATOR_PRODUCT_NAME ": module registration failed!");
		return FAILURE;
	}

#ifdef ZEND_WIN32
	if (UNEXPECTED(accel_gen_uname_id() == FAILURE)) {
		zps_startup_failure("Unable to get user name", NULL, accelerator_remove_cb);
		return SUCCESS;
	}
#endif

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
		accel_startup_ok = false;
		if (!ZCG(accel_directives).enable_cli &&
		    strcmp(sapi_module.name, "cli") == 0) {
			zps_startup_failure("Opcode Caching is disabled for CLI", NULL, accelerator_remove_cb);
		} else {
			zps_startup_failure("Opcode Caching is only supported in Apache, FPM, FastCGI, FrankenPHP, LiteSpeed and uWSGI SAPIs", NULL, accelerator_remove_cb);
		}
		return SUCCESS;
	}

	if (ZCG(enabled) == 0) {
		return SUCCESS ;
	}

	orig_post_startup_cb = zend_post_startup_cb;
	zend_post_startup_cb = accel_post_startup;

	/* Prevent unloading */
	extension->handle = 0;

	return SUCCESS;
}

static zend_result accel_post_startup(void)
{
	zend_function *func;
	zend_ini_entry *ini_entry;

	if (orig_post_startup_cb) {
		zend_result (*cb)(void) = orig_post_startup_cb;

		orig_post_startup_cb = NULL;
		if (cb() != SUCCESS) {
			return FAILURE;
		}
	}

/********************************************/
/* End of non-SHM dependent initializations */
/********************************************/
	file_cache_only = ZCG(accel_directives).file_cache_only;
	if (!file_cache_only) {
		size_t shm_size = ZCG(accel_directives).memory_consumption;
#ifdef HAVE_JIT
		size_t jit_size = 0;
		bool reattached = false;

		if (JIT_G(enabled) && JIT_G(buffer_size)
		 && zend_jit_check_support() == SUCCESS) {
			size_t page_size;

			page_size = zend_get_page_size();
			if (!page_size || (page_size & (page_size - 1))) {
				zend_accel_error_noreturn(ACCEL_LOG_FATAL, "Failure to initialize shared memory structures - can't get page size.");
				abort();
			}
			jit_size = JIT_G(buffer_size);
			jit_size = ZEND_MM_ALIGNED_SIZE_EX(jit_size, page_size);
			shm_size += jit_size;
		}

		switch (zend_shared_alloc_startup(shm_size, jit_size)) {
#else
		switch (zend_shared_alloc_startup(shm_size, 0)) {
#endif
			case ALLOC_SUCCESS:
				if (zend_accel_init_shm() == FAILURE) {
					accel_startup_ok = false;
					return FAILURE;
				}
				break;
			case ALLOC_FAILURE:
				accel_startup_ok = false;
				zend_accel_error_noreturn(ACCEL_LOG_FATAL, "Failure to initialize shared memory structures - probably not enough shared memory.");
				return SUCCESS;
			case SUCCESSFULLY_REATTACHED:
#ifdef HAVE_JIT
				reattached = true;
#endif
				zend_shared_alloc_lock();
				accel_shared_globals = (zend_accel_shared_globals *) ZSMMG(app_shared_globals);
				zend_interned_strings_set_request_storage_handlers(
					accel_new_interned_string_for_php,
					accel_init_interned_string_for_php,
					accel_init_interned_string_for_php);
				zend_shared_alloc_unlock();
				break;
			case FAILED_REATTACHED:
				accel_startup_ok = false;
				zend_accel_error_noreturn(ACCEL_LOG_FATAL, "Failure to initialize shared memory structures - cannot reattach to exiting shared memory.");
				return SUCCESS;
				break;
#if ENABLE_FILE_CACHE_FALLBACK
			case ALLOC_FALLBACK:
				zend_shared_alloc_lock();
				file_cache_only = true;
				fallback_process = true;
				zend_shared_alloc_unlock();
				goto file_cache_fallback;
				break;
#endif
		}

		/* from this point further, shared memory is supposed to be OK */

		/* remember the last restart time in the process memory */
		ZCG(last_restart_time) = ZCSG(last_restart_time);

		zend_shared_alloc_lock();
#ifdef HAVE_JIT
		if (JIT_G(enabled)) {
			if (JIT_G(buffer_size) == 0
		     || !ZSMMG(reserved)
			 || zend_jit_startup(ZSMMG(reserved), jit_size, reattached) != SUCCESS) {
				JIT_G(enabled) = false;
				JIT_G(on) = false;
			}
		}
#endif
		zend_shared_alloc_save_state();
		zend_shared_alloc_unlock();

		SHM_PROTECT();
	} else if (!ZCG(accel_directives).file_cache) {
		accel_startup_ok = false;
		zend_accel_error_noreturn(ACCEL_LOG_FATAL, "opcache.file_cache_only is set without a proper setting of opcache.file_cache");
		return SUCCESS;
	} else {
#ifdef HAVE_JIT
		JIT_G(enabled) = false;
		JIT_G(on) = false;
#endif
		accel_shared_globals = calloc(1, sizeof(zend_accel_shared_globals));
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

	accel_startup_ok = true;

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

	if (!file_cache_only && ZCG(accel_directives).interned_strings_buffer) {
		accel_use_shm_interned_strings();
	}

	if (accel_finish_startup() != SUCCESS) {
		return FAILURE;
	}

	if (ZCG(enabled) && accel_startup_ok) {
		/* Override inheritance cache callbaks */
		accelerator_orig_inheritance_cache_get = zend_inheritance_cache_get;
		accelerator_orig_inheritance_cache_add = zend_inheritance_cache_add;
		zend_inheritance_cache_get = zend_accel_inheritance_cache_get;
		zend_inheritance_cache_add = zend_accel_inheritance_cache_add;
	}

	return SUCCESS;
}

static void (*orig_post_shutdown_cb)(void);

static void accel_post_shutdown(void)
{
	zend_shared_alloc_shutdown();
}

void accel_shutdown(void)
{
	zend_ini_entry *ini_entry;
	bool _file_cache_only = false;

#ifdef HAVE_JIT
	zend_jit_shutdown();
#endif

	zend_accel_blacklist_shutdown(&accel_blacklist);

	if (!ZCG(enabled) || !accel_startup_ok) {
#ifdef ZTS
		ts_free_id(accel_globals_id);
#endif
		return;
	}

	if (ZCSG(preload_script)) {
		preload_shutdown();
	}

	_file_cache_only = file_cache_only;

	accel_reset_pcre_cache();

#ifdef ZTS
	ts_free_id(accel_globals_id);
#endif

	if (!_file_cache_only) {
		/* Delay SHM detach */
		orig_post_shutdown_cb = zend_post_shutdown_cb;
		zend_post_shutdown_cb = accel_post_shutdown;
	}

	zend_compile_file = accelerator_orig_compile_file;
	zend_inheritance_cache_get = accelerator_orig_inheritance_cache_get;
	zend_inheritance_cache_add = accelerator_orig_inheritance_cache_add;

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
	ZCSG(restart_pending) = true;
	ZCSG(restart_reason) = reason;
	ZCSG(cache_status_before_restart) = ZCSG(accelerator_enabled);
	ZCSG(accelerator_enabled) = false;

	if (ZCG(accel_directives).force_restart_timeout) {
		ZCSG(force_restart_time) = zend_accel_get_time() + ZCG(accel_directives).force_restart_timeout;
	} else {
		ZCSG(force_restart_time) = 0;
	}
	SHM_PROTECT();
	HANDLE_UNBLOCK_INTERRUPTIONS();
}

static void accel_deactivate_now(void)
{
	/* this is needed because on WIN32 lock is not decreased unless ZCG(counted) is set */
#ifdef ZEND_WIN32
	ZCG(counted) = true;
#endif
	accel_deactivate_sub();
}

/* ensures it is OK to read SHM
	if it's not OK (restart in progress) returns FAILURE
	if OK returns SUCCESS
	MUST call accelerator_shm_read_unlock after done lock operations
*/
zend_result accelerator_shm_read_lock(void)
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
		ZCG(counted) = true;
	}
	return SUCCESS;
}

/* must be called ONLY after SUCCESSFUL accelerator_shm_read_lock */
void accelerator_shm_read_unlock(void)
{
	if (!ZCG(counted)) {
		/* counted is false - meaning we had to readlock manually, release readlock now */
		accel_deactivate_now();
	}
}

/* Preloading */
static HashTable *preload_scripts = NULL;
static zend_op_array *(*preload_orig_compile_file)(zend_file_handle *file_handle, int type);

static void preload_shutdown(void)
{
	zval *zv;

#if 0
	if (EG(zend_constants)) {
		ZEND_HASH_MAP_REVERSE_FOREACH_VAL(EG(zend_constants), zv) {
			zend_constant *c = Z_PTR_P(zv);
			if (ZEND_CONSTANT_FLAGS(c) & CONST_PERSISTENT) {
				break;
			}
		} ZEND_HASH_MAP_FOREACH_END_DEL();
	}
#endif

	if (EG(function_table)) {
		ZEND_HASH_MAP_REVERSE_FOREACH_VAL(EG(function_table), zv) {
			zend_function *func = Z_PTR_P(zv);
			if (func->type == ZEND_INTERNAL_FUNCTION) {
				break;
			}
		} ZEND_HASH_MAP_FOREACH_END_DEL();
	}

	if (EG(class_table)) {
		ZEND_HASH_MAP_REVERSE_FOREACH_VAL(EG(class_table), zv) {
			zend_class_entry *ce = Z_PTR_P(zv);
			if (ce->type == ZEND_INTERNAL_CLASS) {
				break;
			}
		} ZEND_HASH_MAP_FOREACH_END_DEL();
	}
}

static void preload_activate(void)
{
	if (ZCSG(preload_script)->ping_auto_globals_mask & ~ZCG(auto_globals_mask)) {
		zend_accel_set_auto_globals(ZCSG(preload_script)->ping_auto_globals_mask & ~ZCG(auto_globals_mask));
	}
}

static void preload_restart(void)
{
	zend_accel_hash_update(&ZCSG(hash), ZCSG(preload_script)->script.filename, 0, ZCSG(preload_script));
	if (ZCSG(saved_scripts)) {
		zend_persistent_script **p = ZCSG(saved_scripts);
		while (*p) {
			zend_accel_hash_update(&ZCSG(hash), (*p)->script.filename, 0, *p);
			p++;
		}
	}
}

static size_t preload_try_strip_filename(zend_string *filename) {
	/*FIXME: better way to handle eval()'d code? see COMPILED_STRING_DESCRIPTION_FORMAT */
	if (ZSTR_LEN(filename) > sizeof(" eval()'d code")
		&& *(ZSTR_VAL(filename) + ZSTR_LEN(filename) - sizeof(" eval()'d code")) == ':') {
		const char *cfilename = ZSTR_VAL(filename);
		size_t cfilenamelen = ZSTR_LEN(filename) - sizeof(" eval()'d code") - 1 /*:*/;
		while (cfilenamelen && cfilename[--cfilenamelen] != '(');
		return cfilenamelen;
	}
	return 0;
}

static void preload_move_user_functions(HashTable *src, HashTable *dst)
{
	Bucket *p;
	dtor_func_t orig_dtor = src->pDestructor;
	zend_string *filename = NULL;
	bool copy = false;

	src->pDestructor = NULL;
	zend_hash_extend(dst, dst->nNumUsed + src->nNumUsed, 0);
	ZEND_HASH_MAP_REVERSE_FOREACH_BUCKET(src, p) {
		zend_function *function = Z_PTR(p->val);

		if (EXPECTED(function->type == ZEND_USER_FUNCTION)) {
			if (function->op_array.filename != filename) {
				filename = function->op_array.filename;
				if (filename) {
					if (!(copy = zend_hash_exists(preload_scripts, filename))) {
						size_t eval_len = preload_try_strip_filename(filename);
						if (eval_len) {
							copy = zend_hash_str_exists(preload_scripts, ZSTR_VAL(filename), eval_len);
						}
					}
				} else {
					copy = false;
				}
			}
			if (copy) {
				_zend_hash_append_ptr(dst, p->key, function);
			} else {
				orig_dtor(&p->val);
			}
			zend_hash_del_bucket(src, p);
		} else {
			break;
		}
	} ZEND_HASH_FOREACH_END();
	src->pDestructor = orig_dtor;
}

static void preload_move_user_classes(HashTable *src, HashTable *dst)
{
	Bucket *p;
	dtor_func_t orig_dtor = src->pDestructor;
	zend_string *filename = NULL;
	bool copy = false;

	src->pDestructor = NULL;
	zend_hash_extend(dst, dst->nNumUsed + src->nNumUsed, 0);
	ZEND_HASH_MAP_FOREACH_BUCKET_FROM(src, p, EG(persistent_classes_count)) {
		zend_class_entry *ce = Z_PTR(p->val);
		ZEND_ASSERT(ce->type == ZEND_USER_CLASS);
		if (ce->info.user.filename != filename) {
			filename = ce->info.user.filename;
			if (filename) {
				if (!(copy = zend_hash_exists(preload_scripts, filename))) {
					size_t eval_len = preload_try_strip_filename(filename);
					if (eval_len) {
						copy = zend_hash_str_exists(preload_scripts, ZSTR_VAL(filename), eval_len);
					}
				}
			} else {
				copy = false;
			}
		}
		if (copy) {
			_zend_hash_append(dst, p->key, &p->val);
		} else {
			orig_dtor(&p->val);
		}
		zend_hash_del_bucket(src, p);
	} ZEND_HASH_FOREACH_END();
	src->pDestructor = orig_dtor;
}

static zend_op_array *preload_compile_file(zend_file_handle *file_handle, int type)
{
	zend_op_array *op_array = preload_orig_compile_file(file_handle, type);

	if (op_array && op_array->refcount) {
		zend_persistent_script *script;

		script = create_persistent_script();
		script->script.filename = zend_string_copy(op_array->filename);
		zend_string_hash_val(script->script.filename);
		script->script.main_op_array = *op_array;

//???		efree(op_array->refcount);
		op_array->refcount = NULL;

		zend_hash_add_ptr(preload_scripts, script->script.filename, script);
	}

	return op_array;
}

static void preload_sort_classes(void *base, size_t count, size_t siz, compare_func_t compare, swap_func_t swp)
{
	Bucket *b1 = base;
	Bucket *b2;
	Bucket *end = b1 + count;
	Bucket tmp;
	zend_class_entry *ce, *p;

	while (b1 < end) {
try_again:
		ce = (zend_class_entry*)Z_PTR(b1->val);
		if (ce->parent && (ce->ce_flags & ZEND_ACC_LINKED)) {
			p = ce->parent;
			if (p->type == ZEND_USER_CLASS) {
				b2 = b1 + 1;
				while (b2 < end) {
					if (p ==  Z_PTR(b2->val)) {
						tmp = *b1;
						*b1 = *b2;
						*b2 = tmp;
						goto try_again;
					}
					b2++;
				}
			}
		}
		if (ce->num_interfaces && (ce->ce_flags & ZEND_ACC_LINKED)) {
			uint32_t i = 0;
			for (i = 0; i < ce->num_interfaces; i++) {
				p = ce->interfaces[i];
				if (p->type == ZEND_USER_CLASS) {
					b2 = b1 + 1;
					while (b2 < end) {
						if (p ==  Z_PTR(b2->val)) {
							tmp = *b1;
							*b1 = *b2;
							*b2 = tmp;
							goto try_again;
						}
						b2++;
					}
				}
			}
		}
		b1++;
	}
}

typedef struct {
	const char *kind;
	const char *name;
} preload_error;

static zend_result preload_resolve_deps(preload_error *error, const zend_class_entry *ce)
{
	memset(error, 0, sizeof(preload_error));

	if (ce->parent_name) {
		zend_string *key = zend_string_tolower(ce->parent_name);
		zend_class_entry *parent = zend_hash_find_ptr(EG(class_table), key);
		zend_string_release(key);
		if (!parent) {
			error->kind = "Unknown parent ";
			error->name = ZSTR_VAL(ce->parent_name);
			return FAILURE;
		}
	}

	if (ce->num_interfaces) {
		for (uint32_t i = 0; i < ce->num_interfaces; i++) {
			zend_class_entry *interface =
				zend_hash_find_ptr(EG(class_table), ce->interface_names[i].lc_name);
			if (!interface) {
				error->kind = "Unknown interface ";
				error->name = ZSTR_VAL(ce->interface_names[i].name);
				return FAILURE;
			}
		}
	}

	if (ce->num_traits) {
		for (uint32_t i = 0; i < ce->num_traits; i++) {
			zend_class_entry *trait =
				zend_hash_find_ptr(EG(class_table), ce->trait_names[i].lc_name);
			if (!trait) {
				error->kind = "Unknown trait ";
				error->name = ZSTR_VAL(ce->trait_names[i].name);
				return FAILURE;
			}
		}
	}

	return SUCCESS;
}

static bool preload_try_resolve_constants(zend_class_entry *ce)
{
	bool ok, changed, was_changed = false;
	zend_class_constant *c;
	zval *val;
	zend_string *key;

	EG(exception) = (void*)(uintptr_t)-1; /* prevent error reporting */
	do {
		ok = true;
		changed = false;
		ZEND_HASH_MAP_FOREACH_STR_KEY_PTR(&ce->constants_table, key, c) {
			val = &c->value;
			if (Z_TYPE_P(val) == IS_CONSTANT_AST) {
				if (EXPECTED(zend_update_class_constant(c, key, c->ce) == SUCCESS)) {
					was_changed = changed = true;
				} else {
					ok = false;
				}
			}
		} ZEND_HASH_FOREACH_END();
		if (ok) {
			ce->ce_flags &= ~ZEND_ACC_HAS_AST_CONSTANTS;
		}
		if (ce->default_properties_count) {
			uint32_t i;
			bool resolved = true;

			for (i = 0; i < ce->default_properties_count; i++) {
				val = &ce->default_properties_table[i];
				if (Z_TYPE_P(val) == IS_CONSTANT_AST) {
					zend_property_info *prop = ce->properties_info_table[i];
					if (UNEXPECTED(zval_update_constant_ex(val, prop->ce) != SUCCESS)) {
						resolved = ok = false;
					}
				}
			}
			if (resolved) {
				ce->ce_flags &= ~ZEND_ACC_HAS_AST_PROPERTIES;
			}
		}
		if (ce->default_static_members_count) {
			uint32_t count = ce->parent ? ce->default_static_members_count - ce->parent->default_static_members_count : ce->default_static_members_count;
			bool resolved = true;

			val = ce->default_static_members_table + ce->default_static_members_count - 1;
			while (count) {
				if (Z_TYPE_P(val) == IS_CONSTANT_AST) {
					if (UNEXPECTED(zval_update_constant_ex(val, ce) != SUCCESS)) {
						resolved = ok = false;
					}
				}
				val--;
				count--;
			}
			if (resolved) {
				ce->ce_flags &= ~ZEND_ACC_HAS_AST_STATICS;
			}
		}
	} while (changed && !ok);
	EG(exception) = NULL;
	CG(in_compilation) = false;

	if (ok) {
		ce->ce_flags |= ZEND_ACC_CONSTANTS_UPDATED;
	}

	return ok || was_changed;
}

static void (*orig_error_cb)(int type, zend_string *error_filename, const uint32_t error_lineno, zend_string *message);

static void preload_error_cb(int type, zend_string *error_filename, const uint32_t error_lineno, zend_string *message)
{
	/* Suppress printing of the error, only bail out for fatal errors. */
	if (type & E_FATAL_ERRORS) {
		zend_bailout();
	}
}

/* Remove DECLARE opcodes and dynamic defs. */
static void preload_remove_declares(zend_op_array *op_array)
{
	zend_op *opline = op_array->opcodes;
	zend_op *end = opline + op_array->last;
	uint32_t skip_dynamic_func_count = 0;
	zend_string *key;
	zend_op_array *func;

	while (opline != end) {
		switch (opline->opcode) {
			case ZEND_DECLARE_CLASS:
			case ZEND_DECLARE_CLASS_DELAYED:
				key = Z_STR_P(RT_CONSTANT(opline, opline->op1) + 1);
				if (!zend_hash_exists(CG(class_table), key)) {
					MAKE_NOP(opline);
				}
				break;
			case ZEND_DECLARE_FUNCTION:
				opline->op2.num -= skip_dynamic_func_count;
				key = Z_STR_P(RT_CONSTANT(opline, opline->op1));
				func = zend_hash_find_ptr(EG(function_table), key);
				if (func && func == op_array->dynamic_func_defs[opline->op2.num]) {
					zend_op_array **dynamic_func_defs;

					op_array->num_dynamic_func_defs--;
					if (op_array->num_dynamic_func_defs == 0) {
						dynamic_func_defs = NULL;
					} else {
						dynamic_func_defs = emalloc(sizeof(zend_op_array*) * op_array->num_dynamic_func_defs);
						if (opline->op2.num > 0) {
							memcpy(
								dynamic_func_defs,
								op_array->dynamic_func_defs,
								sizeof(zend_op_array*) * opline->op2.num);
						}
						if (op_array->num_dynamic_func_defs - opline->op2.num > 0) {
							memcpy(
								dynamic_func_defs + opline->op2.num,
								op_array->dynamic_func_defs + (opline->op2.num + 1),
								sizeof(zend_op_array*) * (op_array->num_dynamic_func_defs - opline->op2.num));
						}
					}
					efree(op_array->dynamic_func_defs);
					op_array->dynamic_func_defs = dynamic_func_defs;
					skip_dynamic_func_count++;
					MAKE_NOP(opline);
				}
				break;
			case ZEND_DECLARE_LAMBDA_FUNCTION:
				opline->op2.num -= skip_dynamic_func_count;
				break;
		}
		opline++;
	}
}

static void preload_link(void)
{
	zval *zv;
	zend_persistent_script *script;
	zend_class_entry *ce;
	zend_string *key;
	bool changed;

	HashTable errors;
	zend_hash_init(&errors, 0, NULL, NULL, 0);

	/* Resolve class dependencies */
	do {
		changed = false;

		ZEND_HASH_MAP_FOREACH_STR_KEY_VAL_FROM(EG(class_table), key, zv, EG(persistent_classes_count)) {
			ce = Z_PTR_P(zv);
			ZEND_ASSERT(ce->type != ZEND_INTERNAL_CLASS);

			if (!(ce->ce_flags & (ZEND_ACC_TOP_LEVEL|ZEND_ACC_ANON_CLASS))
					|| (ce->ce_flags & ZEND_ACC_LINKED)) {
				continue;
			}

			zend_string *lcname = zend_string_tolower(ce->name);
			if (!(ce->ce_flags & ZEND_ACC_ANON_CLASS)) {
				if (zend_hash_exists(EG(class_table), lcname)) {
					zend_string_release(lcname);
					continue;
				}
			}

			preload_error error_info;
			if (preload_resolve_deps(&error_info, ce) == FAILURE) {
				zend_string_release(lcname);
				continue;
			}

			zv = zend_hash_set_bucket_key(EG(class_table), (Bucket*)zv, lcname);
			ZEND_ASSERT(zv && "We already checked above that the class doesn't exist yet");

			/* Set the FILE_CACHED flag to force a lazy load, and the CACHED flag to
			 * prevent freeing of interface names. */
			void *checkpoint = zend_arena_checkpoint(CG(arena));
			zend_class_entry *orig_ce = ce;
			uint32_t temporary_flags = ZEND_ACC_FILE_CACHED|ZEND_ACC_CACHED;
			ce->ce_flags |= temporary_flags;
			if (ce->parent_name) {
				zend_string_addref(ce->parent_name);
			}

			/* Record and suppress errors during inheritance. */
			orig_error_cb = zend_error_cb;
			zend_error_cb = preload_error_cb;
			zend_begin_record_errors();

			/* Set filename & lineno information for inheritance errors */
			CG(in_compilation) = true;
			CG(compiled_filename) = ce->info.user.filename;
			CG(zend_lineno) = ce->info.user.line_start;
			zend_try {
				ce = zend_do_link_class(ce, NULL, lcname);
				if (!ce) {
					ZEND_ASSERT(0 && "Class linking failed?");
				}
				ce->ce_flags &= ~temporary_flags;
				changed = true;

				/* Inheritance successful, print out any warnings. */
				zend_error_cb = orig_error_cb;
				zend_emit_recorded_errors();
			} zend_catch {
				/* Clear variance obligations that were left behind on bailout. */
				if (CG(delayed_variance_obligations)) {
					zend_hash_index_del(
						CG(delayed_variance_obligations), (uintptr_t) Z_CE_P(zv));
				}

				/* Restore the original class. */
				zv = zend_hash_set_bucket_key(EG(class_table), (Bucket*)zv, key);
				Z_CE_P(zv) = orig_ce;
				orig_ce->ce_flags &= ~temporary_flags;
				zend_arena_release(&CG(arena), checkpoint);

				/* Remember the last error. */
				zend_error_cb = orig_error_cb;
				EG(record_errors) = false;
				ZEND_ASSERT(EG(num_errors) > 0);
				zend_hash_update_ptr(&errors, key, EG(errors)[EG(num_errors)-1]);
				EG(num_errors)--;
			} zend_end_try();
			CG(in_compilation) = false;
			CG(compiled_filename) = NULL;
			zend_free_recorded_errors();
			zend_string_release(lcname);
		} ZEND_HASH_FOREACH_END();
	} while (changed);

	do {
		changed = false;

		ZEND_HASH_MAP_REVERSE_FOREACH_VAL(EG(class_table), zv) {
			ce = Z_PTR_P(zv);
			if (ce->type == ZEND_INTERNAL_CLASS) {
				break;
			}
			if ((ce->ce_flags & ZEND_ACC_LINKED) && !(ce->ce_flags & ZEND_ACC_CONSTANTS_UPDATED)) {
				if (!(ce->ce_flags & ZEND_ACC_TRAIT)) { /* don't update traits */
					CG(in_compilation) = true; /* prevent autoloading */
					if (preload_try_resolve_constants(ce)) {
						changed = true;
					}
					CG(in_compilation) = false;
				}
			}
		} ZEND_HASH_FOREACH_END();
	} while (changed);

	/* Warn for classes that could not be linked. */
	ZEND_HASH_MAP_FOREACH_STR_KEY_VAL_FROM(
			EG(class_table), key, zv, EG(persistent_classes_count)) {
		ce = Z_PTR_P(zv);
		ZEND_ASSERT(ce->type != ZEND_INTERNAL_CLASS);
		if ((ce->ce_flags & (ZEND_ACC_TOP_LEVEL|ZEND_ACC_ANON_CLASS))
				&& !(ce->ce_flags & ZEND_ACC_LINKED)) {
			zend_string *lcname = zend_string_tolower(ce->name);
			preload_error error;
			if (!(ce->ce_flags & ZEND_ACC_ANON_CLASS)
			 && zend_hash_exists(EG(class_table), lcname)) {
				zend_error_at(
					E_WARNING, ce->info.user.filename, ce->info.user.line_start,
					"Can't preload already declared class %s", ZSTR_VAL(ce->name));
			} else if (preload_resolve_deps(&error, ce) == FAILURE) {
				zend_error_at(
					E_WARNING, ce->info.user.filename, ce->info.user.line_start,
					"Can't preload unlinked class %s: %s%s",
					ZSTR_VAL(ce->name), error.kind, error.name);
			} else {
				zend_error_info *error = zend_hash_find_ptr(&errors, key);
				zend_error_at(
					E_WARNING, error->filename, error->lineno,
					"Can't preload unlinked class %s: %s",
					ZSTR_VAL(ce->name), ZSTR_VAL(error->message));
			}
			zend_string_release(lcname);
		}
	} ZEND_HASH_FOREACH_END();

	zend_hash_destroy(&errors);

	ZEND_HASH_MAP_FOREACH_PTR(preload_scripts, script) {
		zend_op_array *op_array = &script->script.main_op_array;
		preload_remove_declares(op_array);

		if (op_array->fn_flags & ZEND_ACC_EARLY_BINDING) {
			zend_accel_free_delayed_early_binding_list(script);
			zend_accel_build_delayed_early_binding_list(script);
			if (!script->num_early_bindings) {
				op_array->fn_flags &= ~ZEND_ACC_EARLY_BINDING;
			}
		}
	} ZEND_HASH_FOREACH_END();

	/* Dynamic defs inside functions and methods need to be removed as well. */
	zend_op_array *op_array;
	ZEND_HASH_MAP_FOREACH_PTR_FROM(EG(function_table), op_array, EG(persistent_functions_count)) {
		ZEND_ASSERT(op_array->type == ZEND_USER_FUNCTION);
		preload_remove_declares(op_array);
	} ZEND_HASH_FOREACH_END();
	ZEND_HASH_MAP_FOREACH_PTR_FROM(EG(class_table), ce, EG(persistent_classes_count)) {
		ZEND_HASH_MAP_FOREACH_PTR(&ce->function_table, op_array) {
			if (op_array->type == ZEND_USER_FUNCTION) {
				preload_remove_declares(op_array);
			}
		} ZEND_HASH_FOREACH_END();
	} ZEND_HASH_FOREACH_END();
}

static zend_string *preload_resolve_path(zend_string *filename)
{
	if (php_is_stream_path(ZSTR_VAL(filename))) {
		return NULL;
	}
	return zend_resolve_path(filename);
}

static void preload_remove_empty_includes(void)
{
	zend_persistent_script *script;
	bool changed;

	/* mark all as empty */
	ZEND_HASH_MAP_FOREACH_PTR(preload_scripts, script) {
		script->empty = true;
	} ZEND_HASH_FOREACH_END();

	/* find non empty scripts */
	do {
		changed = false;
		ZEND_HASH_MAP_FOREACH_PTR(preload_scripts, script) {
			if (script->empty) {
				bool empty = true;
				zend_op *opline = script->script.main_op_array.opcodes;
				zend_op *end = opline + script->script.main_op_array.last;

				while (opline < end) {
					if (opline->opcode == ZEND_INCLUDE_OR_EVAL &&
					    opline->extended_value != ZEND_EVAL &&
					    opline->op1_type == IS_CONST &&
					    Z_TYPE_P(RT_CONSTANT(opline, opline->op1)) == IS_STRING &&
					    opline->result_type == IS_UNUSED) {

						zend_string *resolved_path = preload_resolve_path(Z_STR_P(RT_CONSTANT(opline, opline->op1)));

						if (resolved_path) {
							zend_persistent_script *incl = zend_hash_find_ptr(preload_scripts, resolved_path);
							zend_string_release(resolved_path);
							if (!incl || !incl->empty) {
								empty = false;
								break;
							}
						} else {
							empty = false;
							break;
						}
					} else if (opline->opcode != ZEND_NOP &&
					           opline->opcode != ZEND_RETURN &&
					           opline->opcode != ZEND_HANDLE_EXCEPTION) {
						empty = false;
						break;
					}
					opline++;
				}
				if (!empty) {
					script->empty = false;
					changed = true;
				}
			}
		} ZEND_HASH_FOREACH_END();
	} while (changed);

	/* remove empty includes */
	ZEND_HASH_MAP_FOREACH_PTR(preload_scripts, script) {
		zend_op *opline = script->script.main_op_array.opcodes;
		zend_op *end = opline + script->script.main_op_array.last;

		while (opline < end) {
			if (opline->opcode == ZEND_INCLUDE_OR_EVAL &&
			    opline->extended_value != ZEND_EVAL &&
			    opline->op1_type == IS_CONST &&
			    Z_TYPE_P(RT_CONSTANT(opline, opline->op1)) == IS_STRING) {

				zend_string *resolved_path = preload_resolve_path(Z_STR_P(RT_CONSTANT(opline, opline->op1)));

				if (resolved_path) {
					zend_persistent_script *incl = zend_hash_find_ptr(preload_scripts, resolved_path);
					if (incl && incl->empty && opline->result_type == IS_UNUSED) {
						MAKE_NOP(opline);
					} else {
						if (!IS_ABSOLUTE_PATH(Z_STRVAL_P(RT_CONSTANT(opline, opline->op1)), Z_STRLEN_P(RT_CONSTANT(opline, opline->op1)))) {
							/* replace relative patch with absolute one */
							zend_string_release(Z_STR_P(RT_CONSTANT(opline, opline->op1)));
							ZVAL_STR_COPY(RT_CONSTANT(opline, opline->op1), resolved_path);
						}
					}
					zend_string_release(resolved_path);
				}
			}
			opline++;
		}
	} ZEND_HASH_FOREACH_END();
}

static void preload_register_trait_methods(zend_class_entry *ce) {
	zend_op_array *op_array;
	ZEND_HASH_MAP_FOREACH_PTR(&ce->function_table, op_array) {
		if (!(op_array->fn_flags & ZEND_ACC_TRAIT_CLONE)) {
			ZEND_ASSERT(op_array->refcount && "Must have refcount pointer");
			zend_shared_alloc_register_xlat_entry(op_array->refcount, op_array);
		}
	} ZEND_HASH_FOREACH_END();
}

static void preload_fix_trait_methods(zend_class_entry *ce)
{
	zend_op_array *op_array;

	ZEND_HASH_MAP_FOREACH_PTR(&ce->function_table, op_array) {
		if (op_array->fn_flags & ZEND_ACC_TRAIT_CLONE) {
			zend_op_array *orig_op_array = zend_shared_alloc_get_xlat_entry(op_array->refcount);
			ZEND_ASSERT(orig_op_array && "Must be in xlat table");

			zend_string *function_name = op_array->function_name;
			zend_class_entry *scope = op_array->scope;
			uint32_t fn_flags = op_array->fn_flags;
			zend_function *prototype = op_array->prototype;
			HashTable *ht = op_array->static_variables;
			*op_array = *orig_op_array;
			op_array->function_name = function_name;
			op_array->scope = scope;
			op_array->fn_flags = fn_flags;
			op_array->prototype = prototype;
			op_array->static_variables = ht;
		}
	} ZEND_HASH_FOREACH_END();
}

static void preload_optimize(zend_persistent_script *script)
{
	zend_class_entry *ce;
	zend_persistent_script *tmp_script;

	zend_shared_alloc_init_xlat_table();

	ZEND_HASH_MAP_FOREACH_PTR(&script->script.class_table, ce) {
		if (ce->ce_flags & ZEND_ACC_TRAIT) {
			preload_register_trait_methods(ce);
		}
	} ZEND_HASH_FOREACH_END();

	ZEND_HASH_MAP_FOREACH_PTR(preload_scripts, tmp_script) {
		ZEND_HASH_MAP_FOREACH_PTR(&tmp_script->script.class_table, ce) {
			if (ce->ce_flags & ZEND_ACC_TRAIT) {
				preload_register_trait_methods(ce);
			}
		} ZEND_HASH_FOREACH_END();
	} ZEND_HASH_FOREACH_END();

	zend_optimize_script(&script->script, ZCG(accel_directives).optimization_level, ZCG(accel_directives).opt_debug_level);
	zend_accel_finalize_delayed_early_binding_list(script);

	ZEND_HASH_MAP_FOREACH_PTR(&script->script.class_table, ce) {
		preload_fix_trait_methods(ce);
	} ZEND_HASH_FOREACH_END();

	ZEND_HASH_MAP_FOREACH_PTR(preload_scripts, script) {
		ZEND_HASH_MAP_FOREACH_PTR(&script->script.class_table, ce) {
			preload_fix_trait_methods(ce);
		} ZEND_HASH_FOREACH_END();
	} ZEND_HASH_FOREACH_END();

	zend_shared_alloc_destroy_xlat_table();

	ZEND_HASH_MAP_FOREACH_PTR(preload_scripts, script) {
		zend_optimize_script(&script->script, ZCG(accel_directives).optimization_level, ZCG(accel_directives).opt_debug_level);
		zend_accel_finalize_delayed_early_binding_list(script);
	} ZEND_HASH_FOREACH_END();
}

static zend_persistent_script* preload_script_in_shared_memory(zend_persistent_script *new_persistent_script)
{
	zend_accel_hash_entry *bucket;
	uint32_t memory_used;
	uint32_t checkpoint;

	if (zend_accel_hash_is_full(&ZCSG(hash))) {
		zend_accel_error_noreturn(ACCEL_LOG_FATAL, "Not enough entries in hash table for preloading. Consider increasing the value for the opcache.max_accelerated_files directive in php.ini.");
		return NULL;
	}

	checkpoint = zend_shared_alloc_checkpoint_xlat_table();

	/* Calculate the required memory size */
	memory_used = zend_accel_script_persist_calc(new_persistent_script, 1);

	/* Allocate shared memory */
	ZCG(mem) = zend_shared_alloc_aligned(memory_used);
	if (!ZCG(mem)) {
		zend_accel_error_noreturn(ACCEL_LOG_FATAL, "Not enough shared memory for preloading. Consider increasing the value for the opcache.memory_consumption directive in php.ini.");
		return NULL;
	}

	bzero_aligned(ZCG(mem), memory_used);

	zend_shared_alloc_restore_xlat_table(checkpoint);

	/* Copy into shared memory */
	new_persistent_script = zend_accel_script_persist(new_persistent_script, 1);

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

	/* store script structure in the hash table */
	bucket = zend_accel_hash_update(&ZCSG(hash), new_persistent_script->script.filename, 0, new_persistent_script);
	if (bucket) {
		zend_accel_error(ACCEL_LOG_INFO, "Cached script '%s'", ZSTR_VAL(new_persistent_script->script.filename));
	}

	new_persistent_script->dynamic_members.memory_consumption = ZEND_ALIGNED_SIZE(new_persistent_script->size);

	return new_persistent_script;
}

static void preload_load(void)
{
	/* Load into process tables */
	zend_script *script = &ZCSG(preload_script)->script;
	if (zend_hash_num_elements(&script->function_table)) {
		Bucket *p = script->function_table.arData;
		Bucket *end = p + script->function_table.nNumUsed;

		zend_hash_extend(CG(function_table),
			CG(function_table)->nNumUsed + script->function_table.nNumUsed, 0);
		for (; p != end; p++) {
			_zend_hash_append_ptr_ex(CG(function_table), p->key, Z_PTR(p->val), 1);
		}
	}

	if (zend_hash_num_elements(&script->class_table)) {
		Bucket *p = script->class_table.arData;
		Bucket *end = p + script->class_table.nNumUsed;

		zend_hash_extend(CG(class_table),
			CG(class_table)->nNumUsed + script->class_table.nNumUsed, 0);
		for (; p != end; p++) {
			_zend_hash_append_ex(CG(class_table), p->key, &p->val, 1);
		}
	}

	if (EG(zend_constants)) {
		EG(persistent_constants_count) = EG(zend_constants)->nNumUsed;
	}
	if (EG(function_table)) {
		EG(persistent_functions_count) = EG(function_table)->nNumUsed;
	}
	if (EG(class_table)) {
		EG(persistent_classes_count)   = EG(class_table)->nNumUsed;
	}
	if (CG(map_ptr_last) != ZCSG(map_ptr_last)) {
		size_t old_map_ptr_last = CG(map_ptr_last);
		CG(map_ptr_last) = ZCSG(map_ptr_last);
		CG(map_ptr_size) = ZEND_MM_ALIGNED_SIZE_EX(CG(map_ptr_last) + 1, 4096);
		CG(map_ptr_real_base) = perealloc(CG(map_ptr_real_base), CG(map_ptr_size) * sizeof(void*), 1);
		CG(map_ptr_base) = ZEND_MAP_PTR_BIASED_BASE(CG(map_ptr_real_base));
		memset((void **) CG(map_ptr_real_base) + old_map_ptr_last, 0,
			(CG(map_ptr_last) - old_map_ptr_last) * sizeof(void *));
	}
}

static zend_result accel_preload(const char *config, bool in_child)
{
	zend_file_handle file_handle;
	zend_result ret;
	char *orig_open_basedir;
	size_t orig_map_ptr_last;
	uint32_t orig_compiler_options;

	ZCG(enabled) = false;
	ZCG(accelerator_enabled) = false;
	orig_open_basedir = PG(open_basedir);
	PG(open_basedir) = NULL;
	preload_orig_compile_file = accelerator_orig_compile_file;
	accelerator_orig_compile_file = preload_compile_file;

	orig_map_ptr_last = CG(map_ptr_last);

	/* Compile and execute preloading script */
	zend_stream_init_filename(&file_handle, (char *) config);

	preload_scripts = emalloc(sizeof(HashTable));
	zend_hash_init(preload_scripts, 0, NULL, NULL, 0);

	orig_compiler_options = CG(compiler_options);
	if (in_child) {
		CG(compiler_options) |= ZEND_COMPILE_PRELOAD_IN_CHILD;
	}
	CG(compiler_options) |= ZEND_COMPILE_PRELOAD;
	CG(compiler_options) |= ZEND_COMPILE_HANDLE_OP_ARRAY;
	CG(compiler_options) |= ZEND_COMPILE_DELAYED_BINDING;
	CG(compiler_options) |= ZEND_COMPILE_NO_CONSTANT_SUBSTITUTION;
	CG(compiler_options) |= ZEND_COMPILE_IGNORE_OTHER_FILES;
	CG(skip_shebang) = true;

	zend_try {
		zend_op_array *op_array;

		ret = SUCCESS;
		op_array = zend_compile_file(&file_handle, ZEND_REQUIRE);
		if (file_handle.opened_path) {
			zend_hash_add_empty_element(&EG(included_files), file_handle.opened_path);
		}
		zend_destroy_file_handle(&file_handle);
		if (op_array) {
			zend_execute(op_array, NULL);
			zend_exception_restore();
			if (UNEXPECTED(EG(exception))) {
				if (Z_TYPE(EG(user_exception_handler)) != IS_UNDEF) {
					zend_user_exception_handler();
				}
				if (EG(exception)) {
					ret = zend_exception_error(EG(exception), E_ERROR);
					if (ret == FAILURE) {
						CG(unclean_shutdown) = true;
					}
				}
			}
			destroy_op_array(op_array);
			efree_size(op_array, sizeof(zend_op_array));
		} else {
			if (EG(exception)) {
				zend_exception_error(EG(exception), E_ERROR);
			}

			CG(unclean_shutdown) = true;
			ret = FAILURE;
		}
	} zend_catch {
		ret = FAILURE;
	} zend_end_try();

	PG(open_basedir) = orig_open_basedir;
	accelerator_orig_compile_file = preload_orig_compile_file;
	ZCG(enabled) = true;

	zend_destroy_file_handle(&file_handle);

	if (ret == SUCCESS) {
		zend_persistent_script *script;
		int ping_auto_globals_mask;
		int i;

		if (PG(auto_globals_jit)) {
			ping_auto_globals_mask = zend_accel_get_auto_globals();
		} else {
			ping_auto_globals_mask = 0;
		}

		if (EG(zend_constants)) {
			/* Remember __COMPILER_HALT_OFFSET__(s). Do this early,
			 * as zend_shutdown_executor_values() destroys constants. */
			ZEND_HASH_MAP_FOREACH_PTR(preload_scripts, script) {
				zend_execute_data *orig_execute_data = EG(current_execute_data);
				zend_execute_data fake_execute_data;
				zval *offset;

				memset(&fake_execute_data, 0, sizeof(fake_execute_data));
				fake_execute_data.func = (zend_function*)&script->script.main_op_array;
				EG(current_execute_data) = &fake_execute_data;
				if ((offset = zend_get_constant_str("__COMPILER_HALT_OFFSET__", sizeof("__COMPILER_HALT_OFFSET__") - 1)) != NULL) {
					script->compiler_halt_offset = Z_LVAL_P(offset);
				}
				EG(current_execute_data) = orig_execute_data;
			} ZEND_HASH_FOREACH_END();
		}

		/* Cleanup executor */
		EG(flags) |= EG_FLAGS_IN_SHUTDOWN;

		php_call_shutdown_functions();
		zend_call_destructors();
		php_output_end_all();
		php_free_shutdown_functions();

		/* Release stored values to avoid dangling pointers */
		zend_shutdown_executor_values(/* fast_shutdown */ false);

		/* We don't want to preload constants.
		 * Check that  zend_shutdown_executor_values() also destroys constants. */
		ZEND_ASSERT(zend_hash_num_elements(EG(zend_constants)) == EG(persistent_constants_count));

		zend_hash_init(&EG(symbol_table), 0, NULL, ZVAL_PTR_DTOR, 0);

		CG(map_ptr_last) = orig_map_ptr_last;

		if (EG(full_tables_cleanup)) {
			zend_accel_error_noreturn(ACCEL_LOG_FATAL, "Preloading is not compatible with dl() function.");
			ret = FAILURE;
			goto finish;
		}

		/* Inheritance errors may be thrown during linking */
		zend_try {
			preload_link();
		} zend_catch {
			CG(map_ptr_last) = orig_map_ptr_last;
			ret = FAILURE;
			goto finish;
		} zend_end_try();

		preload_remove_empty_includes();

		script = create_persistent_script();
		script->ping_auto_globals_mask = ping_auto_globals_mask;

		/* Store all functions and classes in a single pseudo-file */
		CG(compiled_filename) = ZSTR_INIT_LITERAL("$PRELOAD$", 0);
#if ZEND_USE_ABS_CONST_ADDR
		init_op_array(&script->script.main_op_array, ZEND_USER_FUNCTION, 1);
#else
		init_op_array(&script->script.main_op_array, ZEND_USER_FUNCTION, 2);
#endif
		script->script.main_op_array.fn_flags |= ZEND_ACC_DONE_PASS_TWO;
		script->script.main_op_array.last = 1;
		script->script.main_op_array.last_literal = 1;
		script->script.main_op_array.T = ZEND_OBSERVER_ENABLED;
#if ZEND_USE_ABS_CONST_ADDR
		script->script.main_op_array.literals = (zval*)emalloc(sizeof(zval));
#else
		script->script.main_op_array.literals = (zval*)(script->script.main_op_array.opcodes + 1);
#endif
		ZVAL_NULL(script->script.main_op_array.literals);
		memset(script->script.main_op_array.opcodes, 0, sizeof(zend_op));
		script->script.main_op_array.opcodes[0].opcode = ZEND_RETURN;
		script->script.main_op_array.opcodes[0].op1_type = IS_CONST;
		script->script.main_op_array.opcodes[0].op1.constant = 0;
		ZEND_PASS_TWO_UPDATE_CONSTANT(&script->script.main_op_array, script->script.main_op_array.opcodes, script->script.main_op_array.opcodes[0].op1);
		zend_vm_set_opcode_handler(script->script.main_op_array.opcodes);

		script->script.filename = CG(compiled_filename);
		CG(compiled_filename) = NULL;

		preload_move_user_functions(CG(function_table), &script->script.function_table);
		preload_move_user_classes(CG(class_table), &script->script.class_table);

		zend_hash_sort_ex(&script->script.class_table, preload_sort_classes, NULL, 0);

		preload_optimize(script);

		zend_shared_alloc_init_xlat_table();

		HANDLE_BLOCK_INTERRUPTIONS();
		SHM_UNPROTECT();

		ZCSG(preload_script) = preload_script_in_shared_memory(script);

		SHM_PROTECT();
		HANDLE_UNBLOCK_INTERRUPTIONS();

		preload_load();

		/* Store individual scripts with unlinked classes */
		HANDLE_BLOCK_INTERRUPTIONS();
		SHM_UNPROTECT();

		i = 0;
		ZCSG(saved_scripts) = zend_shared_alloc((zend_hash_num_elements(preload_scripts) + 1) * sizeof(void*));
		ZEND_HASH_MAP_FOREACH_PTR(preload_scripts, script) {
			if (zend_hash_num_elements(&script->script.class_table) > 1) {
				zend_hash_sort_ex(&script->script.class_table, preload_sort_classes, NULL, 0);
			}
			ZCSG(saved_scripts)[i++] = preload_script_in_shared_memory(script);
		} ZEND_HASH_FOREACH_END();
		ZCSG(saved_scripts)[i] = NULL;

		zend_shared_alloc_save_state();
		accel_interned_strings_save_state();

		SHM_PROTECT();
		HANDLE_UNBLOCK_INTERRUPTIONS();

		zend_shared_alloc_destroy_xlat_table();
	} else {
		CG(map_ptr_last) = orig_map_ptr_last;
	}

finish:
	CG(compiler_options) = orig_compiler_options;
	zend_hash_destroy(preload_scripts);
	efree(preload_scripts);
	preload_scripts = NULL;

	return ret;
}

static size_t preload_ub_write(const char *str, size_t str_length)
{
	return fwrite(str, 1, str_length, stdout);
}

static void preload_flush(void *server_context)
{
	fflush(stdout);
}

static int preload_header_handler(sapi_header_struct *h, sapi_header_op_enum op, sapi_headers_struct *s)
{
	return 0;
}

static int preload_send_headers(sapi_headers_struct *sapi_headers)
{
	return SAPI_HEADER_SENT_SUCCESSFULLY;
}

static void preload_send_header(sapi_header_struct *sapi_header, void *server_context)
{
}

#ifndef ZEND_WIN32
static zend_result accel_finish_startup_preload(bool in_child)
{
	zend_result ret = SUCCESS;
	int orig_error_reporting;

	int (*orig_activate)(void) = sapi_module.activate;
	int (*orig_deactivate)(void) = sapi_module.deactivate;
	void (*orig_register_server_variables)(zval *track_vars_array) = sapi_module.register_server_variables;
	int (*orig_header_handler)(sapi_header_struct *sapi_header, sapi_header_op_enum op, sapi_headers_struct *sapi_headers) = sapi_module.header_handler;
	int (*orig_send_headers)(sapi_headers_struct *sapi_headers) = sapi_module.send_headers;
	void (*orig_send_header)(sapi_header_struct *sapi_header, void *server_context)= sapi_module.send_header;
	char *(*orig_getenv)(const char *name, size_t name_len) = sapi_module.getenv;
	size_t (*orig_ub_write)(const char *str, size_t str_length) = sapi_module.ub_write;
	void (*orig_flush)(void *server_context) = sapi_module.flush;
#ifdef ZEND_SIGNALS
	bool old_reset_signals = SIGG(reset);
#endif

	sapi_module.activate = NULL;
	sapi_module.deactivate = NULL;
	sapi_module.register_server_variables = NULL;
	sapi_module.header_handler = preload_header_handler;
	sapi_module.send_headers = preload_send_headers;
	sapi_module.send_header = preload_send_header;
	sapi_module.getenv = NULL;
	sapi_module.ub_write = preload_ub_write;
	sapi_module.flush = preload_flush;

	zend_interned_strings_switch_storage(1);

#ifdef ZEND_SIGNALS
	SIGG(reset) = false;
#endif

	orig_error_reporting = EG(error_reporting);
	EG(error_reporting) = 0;

	const zend_result rc = php_request_startup();

	EG(error_reporting) = orig_error_reporting;

	if (rc == SUCCESS) {
		bool orig_report_memleaks;

		/* don't send headers */
		SG(headers_sent) = true;
		SG(request_info).no_headers = true;
		php_output_set_status(0);

		ZCG(auto_globals_mask) = 0;
		ZCG(request_time) = (time_t)sapi_get_request_time();
		ZCG(cache_opline) = NULL;
		ZCG(cache_persistent_script) = NULL;
		ZCG(include_path_key_len) = 0;
		ZCG(include_path_check) = true;

		ZCG(cwd) = NULL;
		ZCG(cwd_key_len) = 0;
		ZCG(cwd_check) = true;

		if (accel_preload(ZCG(accel_directives).preload, in_child) != SUCCESS) {
			ret = FAILURE;
		}
		preload_flush(NULL);

		orig_report_memleaks = PG(report_memleaks);
		PG(report_memleaks) = false;
#ifdef ZEND_SIGNALS
		/* We may not have registered signal handlers due to SIGG(reset)=0, so
		 * also disable the check that they are registered. */
		SIGG(check) = false;
#endif
		php_request_shutdown(NULL); /* calls zend_shared_alloc_unlock(); */
		PG(report_memleaks) = orig_report_memleaks;
	} else {
		zend_shared_alloc_unlock();
		ret = FAILURE;
	}
#ifdef ZEND_SIGNALS
	SIGG(reset) = old_reset_signals;
#endif

	sapi_module.activate = orig_activate;
	sapi_module.deactivate = orig_deactivate;
	sapi_module.register_server_variables = orig_register_server_variables;
	sapi_module.header_handler = orig_header_handler;
	sapi_module.send_headers = orig_send_headers;
	sapi_module.send_header = orig_send_header;
	sapi_module.getenv = orig_getenv;
	sapi_module.ub_write = orig_ub_write;
	sapi_module.flush = orig_flush;

	sapi_activate();

	return ret;
}

static zend_result accel_finish_startup_preload_subprocess(pid_t *pid)
{
	uid_t euid = geteuid();
	if (euid != 0) {
		if (ZCG(accel_directives).preload_user
		 && *ZCG(accel_directives).preload_user) {
			zend_accel_error(ACCEL_LOG_WARNING, "\"opcache.preload_user\" is ignored because the current user is not \"root\"");
		}

		*pid = -1;
		return SUCCESS;
	}

	if (!ZCG(accel_directives).preload_user
	 || !*ZCG(accel_directives).preload_user) {

		bool sapi_requires_preload_user = !(strcmp(sapi_module.name, "cli") == 0
		  || strcmp(sapi_module.name, "phpdbg") == 0);

		if (!sapi_requires_preload_user) {
			*pid = -1;
			return SUCCESS;
		}

		zend_shared_alloc_unlock();
		zend_accel_error_noreturn(ACCEL_LOG_FATAL, "\"opcache.preload\" requires \"opcache.preload_user\" when running under uid 0");
		return FAILURE;
	}

	struct passwd *pw = getpwnam(ZCG(accel_directives).preload_user);
	if (pw == NULL) {
		zend_shared_alloc_unlock();
		zend_accel_error_noreturn(ACCEL_LOG_FATAL, "Preloading failed to getpwnam(\"%s\")", ZCG(accel_directives).preload_user);
		return FAILURE;
	}

	if (pw->pw_uid == euid) {
		*pid = -1;
		return SUCCESS;
	}

	*pid = fork();
	if (*pid == -1) {
		zend_shared_alloc_unlock();
		zend_accel_error_noreturn(ACCEL_LOG_FATAL, "Preloading failed to fork()");
		return FAILURE;
	}

	if (*pid == 0) { /* children */
		if (setgid(pw->pw_gid) < 0) {
			zend_accel_error(ACCEL_LOG_WARNING, "Preloading failed to setgid(%d)", pw->pw_gid);
			exit(1);
		}
		if (initgroups(pw->pw_name, pw->pw_gid) < 0) {
			zend_accel_error(ACCEL_LOG_WARNING, "Preloading failed to initgroups(\"%s\", %d)", pw->pw_name, pw->pw_uid);
			exit(1);
		}
		if (setuid(pw->pw_uid) < 0) {
			zend_accel_error(ACCEL_LOG_WARNING, "Preloading failed to setuid(%d)", pw->pw_uid);
			exit(1);
		}
	}

	return SUCCESS;
}
#endif /* ZEND_WIN32 */

static zend_result accel_finish_startup(void)
{
	if (!ZCG(enabled) || !accel_startup_ok) {
		return SUCCESS;
	}

	if (!(ZCG(accel_directives).preload && *ZCG(accel_directives).preload)) {
		return SUCCESS;
	}

#ifdef ZEND_WIN32
	zend_accel_error_noreturn(ACCEL_LOG_ERROR, "Preloading is not supported on Windows");
	return FAILURE;
#else /* ZEND_WIN32 */

	if (UNEXPECTED(file_cache_only)) {
		zend_accel_error(ACCEL_LOG_WARNING, "Preloading doesn't work in \"file_cache_only\" mode");
		return SUCCESS;
	}

	/* exclusive lock */
	zend_shared_alloc_lock();

	if (ZCSG(preload_script)) {
		/* Preloading was done in another process */
		preload_load();
		zend_shared_alloc_unlock();
		return SUCCESS;
	}


	pid_t pid;
	if (accel_finish_startup_preload_subprocess(&pid) == FAILURE) {
		zend_shared_alloc_unlock();
		return FAILURE;
	}

	if (pid == -1) { /* no subprocess was needed */
		/* The called function unlocks the shared alloc lock */
		return accel_finish_startup_preload(false);
	} else if (pid == 0) { /* subprocess */
		const zend_result ret = accel_finish_startup_preload(true);

		exit(ret == SUCCESS ? 0 : 1);
	} else { /* parent */
		int status;

		if (waitpid(pid, &status, 0) < 0) {
			zend_shared_alloc_unlock();
			zend_accel_error_noreturn(ACCEL_LOG_FATAL, "Preloading failed to waitpid(%d)", pid);
		}

		if (ZCSG(preload_script)) {
			preload_load();
		}

		zend_shared_alloc_unlock();

		if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
			return SUCCESS;
		} else {
			return FAILURE;
		}
	}
#endif /* ZEND_WIN32 */
}

ZEND_EXT_API zend_extension zend_extension_entry = {
	ACCELERATOR_PRODUCT_NAME,               /* name */
	PHP_VERSION,							/* version */
	"Zend Technologies",					/* author */
	"http://www.zend.com/",					/* URL */
	"Copyright (c)",						/* copyright */
	accel_startup,					   		/* startup */
	NULL,									/* shutdown */
	NULL,									/* per-script activation */
#ifdef HAVE_JIT
	accel_deactivate,                       /* per-script deactivation */
#else
	NULL,									/* per-script deactivation */
#endif
	NULL,									/* message handler */
	NULL,									/* op_array handler */
	NULL,									/* extended statement handler */
	NULL,									/* extended fcall begin handler */
	NULL,									/* extended fcall end handler */
	NULL,									/* op_array ctor */
	NULL,									/* op_array dtor */
	STANDARD_ZEND_EXTENSION_PROPERTIES
};
