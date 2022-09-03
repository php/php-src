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

#ifndef ZEND_ACCELERATOR_H
#define ZEND_ACCELERATOR_H

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#define ACCELERATOR_PRODUCT_NAME	"Zend OPcache"
/* 2 - added Profiler support, on 20010712 */
/* 3 - added support for Optimizer's encoded-only-files mode */
/* 4 - works with the new Optimizer, that supports the file format with licenses */
/* 5 - API 4 didn't really work with the license-enabled file format.  v5 does. */
/* 6 - Monitor was removed from ZendPlatform.so, to a module of its own */
/* 7 - Optimizer was embedded into Accelerator */
/* 8 - Standalone Open Source Zend OPcache */
#define ACCELERATOR_API_NO 8

#if ZEND_WIN32
# include "zend_config.w32.h"
#else
#include "zend_config.h"
# include <sys/time.h>
# include <sys/resource.h>
#endif

#if HAVE_UNISTD_H
# include "unistd.h"
#endif

#include "zend_extensions.h"
#include "zend_compile.h"

#include "Optimizer/zend_optimizer.h"
#include "zend_accelerator_hash.h"
#include "zend_accelerator_debug.h"

#ifndef PHPAPI
# ifdef ZEND_WIN32
#  define PHPAPI __declspec(dllimport)
# else
#  define PHPAPI
# endif
#endif

#ifndef ZEND_EXT_API
# ifdef ZEND_WIN32
#  define ZEND_EXT_API __declspec(dllexport)
# elif defined(__GNUC__) && __GNUC__ >= 4
#  define ZEND_EXT_API __attribute__ ((visibility("default")))
# else
#  define ZEND_EXT_API
# endif
#endif

#ifdef ZEND_WIN32
# ifndef MAXPATHLEN
#  include "win32/ioutil.h"
#  define MAXPATHLEN PHP_WIN32_IOUTIL_MAXPATHLEN
# endif
# include <direct.h>
#else
# ifndef MAXPATHLEN
#  define MAXPATHLEN     4096
# endif
# include <sys/param.h>
#endif

/*** file locking ***/
#ifndef ZEND_WIN32
extern int lock_file;
#endif

#if defined(ZEND_WIN32)
# define ENABLE_FILE_CACHE_FALLBACK 1
#else
# define ENABLE_FILE_CACHE_FALLBACK 0
#endif

#if ZEND_WIN32
typedef unsigned __int64 accel_time_t;
#else
typedef time_t accel_time_t;
#endif

typedef enum _zend_accel_restart_reason {
	ACCEL_RESTART_OOM,    /* restart because of out of memory */
	ACCEL_RESTART_HASH,   /* restart because of hash overflow */
	ACCEL_RESTART_USER    /* restart scheduled by opcache_reset() */
} zend_accel_restart_reason;

typedef struct _zend_early_binding {
	zend_string *lcname;
	zend_string *rtd_key;
	zend_string *lc_parent_name;
	uint32_t cache_slot;
} zend_early_binding;

typedef struct _zend_persistent_script {
	zend_script    script;
	zend_long      compiler_halt_offset;   /* position of __HALT_COMPILER or -1 */
	int            ping_auto_globals_mask; /* which autoglobals are used by the script */
	accel_time_t   timestamp;              /* the script modification time */
	bool      corrupted;
	bool      is_phar;
	bool      empty;
	uint32_t       num_warnings;
	uint32_t       num_early_bindings;
	zend_error_info **warnings;
	zend_early_binding *early_bindings;

	void          *mem;                    /* shared memory area used by script structures */
	size_t         size;                   /* size of used shared memory */

	/* All entries that shouldn't be counted in the ADLER32
	 * checksum must be declared in this struct
	 */
	struct zend_persistent_script_dynamic_members {
		time_t       last_used;
		zend_ulong   hits;
		unsigned int memory_consumption;
		unsigned int checksum;
		time_t       revalidate;
	} dynamic_members;
} zend_persistent_script;

typedef struct _zend_accel_directives {
	zend_long           memory_consumption;
	zend_long           max_accelerated_files;
	double         max_wasted_percentage;
	char          *user_blacklist_filename;
	zend_long           consistency_checks;
	zend_long           force_restart_timeout;
	bool      use_cwd;
	bool      ignore_dups;
	bool      validate_timestamps;
	bool      revalidate_path;
	bool      save_comments;
	bool      record_warnings;
	bool      protect_memory;
	bool      file_override_enabled;
	bool      enable_cli;
	bool      validate_permission;
#ifndef ZEND_WIN32
	bool      validate_root;
#endif
	zend_ulong     revalidate_freq;
	zend_ulong     file_update_protection;
	char          *error_log;
#ifdef ZEND_WIN32
	char          *mmap_base;
#endif
	char          *memory_model;
	zend_long           log_verbosity_level;

	zend_long           optimization_level;
	zend_long           opt_debug_level;
	zend_long           max_file_size;
	zend_long           interned_strings_buffer;
	char          *restrict_api;
#ifndef ZEND_WIN32
	char          *lockfile_path;
#endif
	char          *file_cache;
	bool      file_cache_only;
	bool      file_cache_consistency_checks;
#if ENABLE_FILE_CACHE_FALLBACK
	bool      file_cache_fallback;
#endif
#ifdef HAVE_HUGE_CODE_PAGES
	bool      huge_code_pages;
#endif
	char *preload;
#ifndef ZEND_WIN32
	char *preload_user;
#endif
#ifdef ZEND_WIN32
	char *cache_id;
#endif
} zend_accel_directives;

typedef struct _zend_accel_globals {
	bool               counted;   /* the process uses shared memory */
	bool               enabled;
	bool               locked;    /* thread obtained exclusive lock */
	bool               accelerator_enabled; /* accelerator enabled for current request */
	bool               pcre_reseted;
	zend_accel_directives   accel_directives;
	zend_string            *cwd;                  /* current working directory or NULL */
	zend_string            *include_path;         /* current value of "include_path" directive */
	char                    include_path_key[32]; /* key of current "include_path" */
	char                    cwd_key[32];          /* key of current working directory */
	int                     include_path_key_len;
	bool                    include_path_check;
	int                     cwd_key_len;
	bool                    cwd_check;
	int                     auto_globals_mask;
	time_t                  request_time;
	time_t                  last_restart_time; /* used to synchronize SHM and in-process caches */
	HashTable               xlat_table;
#ifndef ZEND_WIN32
	zend_ulong              root_hash;
#endif
	/* preallocated shared-memory block to save current script */
	void                   *mem;
	zend_persistent_script *current_persistent_script;
	/* cache to save hash lookup on the same INCLUDE opcode */
	const zend_op          *cache_opline;
	zend_persistent_script *cache_persistent_script;
	/* preallocated buffer for keys */
	zend_string             key;
	char                    _key[MAXPATHLEN * 8];
} zend_accel_globals;

typedef struct _zend_string_table {
	uint32_t     nTableMask;
	uint32_t     nNumOfElements;
	zend_string *start;
	zend_string *top;
	zend_string *end;
	zend_string *saved_top;
} zend_string_table;

typedef struct _zend_accel_shared_globals {
	/* Cache Data Structures */
	zend_ulong   hits;
	zend_ulong   misses;
	zend_ulong   blacklist_misses;
	zend_ulong   oom_restarts;     /* number of restarts because of out of memory */
	zend_ulong   hash_restarts;    /* number of restarts because of hash overflow */
	zend_ulong   manual_restarts;  /* number of restarts scheduled by opcache_reset() */
	zend_accel_hash hash;             /* hash table for cached scripts */

	size_t map_ptr_last;

	/* Directives & Maintenance */
	time_t          start_time;
	time_t          last_restart_time;
	time_t          force_restart_time;
	bool       accelerator_enabled;
	bool       restart_pending;
	zend_accel_restart_reason restart_reason;
	bool       cache_status_before_restart;
#ifdef ZEND_WIN32
	LONGLONG   mem_usage;
	LONGLONG   restart_in;
#endif
	bool       restart_in_progress;

	/* Preloading */
	zend_persistent_script *preload_script;
	zend_persistent_script **saved_scripts;

	/* uninitialized HashTable Support */
	uint32_t uninitialized_bucket[-HT_MIN_MASK];

	/* Tracing JIT */
	void *jit_traces;
	const void **jit_exit_groups;

	/* Interned Strings Support (must be the last element) */
	zend_string_table interned_strings;
} zend_accel_shared_globals;

#ifdef ZEND_WIN32
extern char accel_uname_id[32];
#endif
extern bool accel_startup_ok;
extern bool file_cache_only;
#if ENABLE_FILE_CACHE_FALLBACK
extern bool fallback_process;
#endif

extern zend_accel_shared_globals *accel_shared_globals;
#define ZCSG(element)   (accel_shared_globals->element)

#ifdef ZTS
# define ZCG(v)	ZEND_TSRMG(accel_globals_id, zend_accel_globals *, v)
extern int accel_globals_id;
# ifdef COMPILE_DL_OPCACHE
ZEND_TSRMLS_CACHE_EXTERN()
# endif
#else
# define ZCG(v) (accel_globals.v)
extern zend_accel_globals accel_globals;
#endif

extern char *zps_api_failure_reason;

BEGIN_EXTERN_C()

void accel_shutdown(void);
zend_result  accel_activate(INIT_FUNC_ARGS);
zend_result accel_post_deactivate(void);
void zend_accel_schedule_restart(zend_accel_restart_reason reason);
void zend_accel_schedule_restart_if_necessary(zend_accel_restart_reason reason);
accel_time_t zend_get_file_handle_timestamp(zend_file_handle *file_handle, size_t *size);
int  validate_timestamp_and_record(zend_persistent_script *persistent_script, zend_file_handle *file_handle);
int  validate_timestamp_and_record_ex(zend_persistent_script *persistent_script, zend_file_handle *file_handle);
int  zend_accel_invalidate(zend_string *filename, bool force);
int  accelerator_shm_read_lock(void);
void accelerator_shm_read_unlock(void);

zend_string *accel_make_persistent_key(zend_string *path);
zend_op_array *persistent_compile_file(zend_file_handle *file_handle, int type);

#define IS_ACCEL_INTERNED(str) \
	((char*)(str) >= (char*)ZCSG(interned_strings).start && (char*)(str) < (char*)ZCSG(interned_strings).top)

zend_string* ZEND_FASTCALL accel_new_interned_string(zend_string *str);

uint32_t zend_accel_get_class_name_map_ptr(zend_string *type_name);

END_EXTERN_C()

/* memory write protection */
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

#endif /* ZEND_ACCELERATOR_H */
