/*
   +----------------------------------------------------------------------+
   | Thread Safe Resource Manager                                         |
   +----------------------------------------------------------------------+
   | Copyright (c) 1999-2011, Andi Gutmans, Sascha Schumann, Zeev Suraski |
   | This source file is subject to the TSRM license, that is bundled     |
   | with this package in the file LICENSE                                |
   +----------------------------------------------------------------------+
   | Authors:  Zeev Suraski <zeev@php.net>                                |
   +----------------------------------------------------------------------+
*/

#ifndef TSRM_H
#define TSRM_H

#if !defined(__CYGWIN__) && defined(WIN32)
# define TSRM_WIN32
# include "Zend/zend_config.w32.h"
#else
# include "main/php_config.h"
#endif

#include "main/php_stdint.h"

#ifdef TSRM_WIN32
#	ifdef TSRM_EXPORTS
#		define TSRM_API __declspec(dllexport)
#	else
#		define TSRM_API __declspec(dllimport)
#	endif
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define TSRM_API __attribute__ ((visibility("default")))
#else
#	define TSRM_API
#endif

typedef intptr_t tsrm_intptr_t;
typedef uintptr_t tsrm_uintptr_t;

/* Only compile multi-threading functions if we're in ZTS mode */
#ifdef ZTS

#ifdef TSRM_WIN32
# ifndef TSRM_INCLUDE_FULL_WINDOWS_HEADERS
#  define WIN32_LEAN_AND_MEAN
# endif
# include <windows.h>
# include <shellapi.h>
#elif defined(GNUPTH)
# include <pth.h>
#elif defined(PTHREADS)
# include <pthread.h>
#elif defined(TSRM_ST)
# include <st.h>
#endif

#if SIZEOF_SIZE_T == 4
# define TSRM_ALIGNED_SIZE(size) \
	(((size) + INT32_C(15)) & ~INT32_C(15))
#else
# define TSRM_ALIGNED_SIZE(size) \
	(((size) + INT64_C(15)) & ~INT64_C(15))
#endif

typedef int ts_rsrc_id;

/* Define THREAD_T and MUTEX_T */
#ifdef TSRM_WIN32
# define THREAD_T DWORD
# define MUTEX_T CRITICAL_SECTION *
#elif defined(GNUPTH)
# define THREAD_T pth_t
# define MUTEX_T pth_mutex_t *
#elif defined(PTHREADS)
# define THREAD_T pthread_t
# define MUTEX_T pthread_mutex_t *
#elif defined(TSRM_ST)
# define THREAD_T st_thread_t
# define MUTEX_T st_mutex_t
#endif

#include <signal.h>

typedef void (*ts_allocate_ctor)(void *);
typedef void (*ts_allocate_dtor)(void *);

#define THREAD_HASH_OF(thr,ts)  (unsigned long)thr%(unsigned long)ts

#ifdef __cplusplus
extern "C" {
#endif

/* startup/shutdown */
TSRM_API int tsrm_startup(int expected_threads, int expected_resources, int debug_level, char *debug_filename);
TSRM_API void tsrm_shutdown(void);

/* environ lock API */
TSRM_API void tsrm_env_lock();
TSRM_API void tsrm_env_unlock();

/* allocates a new thread-safe-resource id */
TSRM_API ts_rsrc_id ts_allocate_id(ts_rsrc_id *rsrc_id, size_t size, ts_allocate_ctor ctor, ts_allocate_dtor dtor);

/* Fast resource in reserved (pre-allocated) space */
TSRM_API void tsrm_reserve(size_t size);
TSRM_API ts_rsrc_id ts_allocate_fast_id(ts_rsrc_id *rsrc_id, size_t *offset, size_t size, ts_allocate_ctor ctor, ts_allocate_dtor dtor);

/* fetches the requested resource for the current thread */
TSRM_API void *ts_resource_ex(ts_rsrc_id id, THREAD_T *th_id);
#define ts_resource(id)			ts_resource_ex(id, NULL)

/* frees all resources allocated for the current thread */
TSRM_API void ts_free_thread(void);

/* deallocates all occurrences of a given id */
TSRM_API void ts_free_id(ts_rsrc_id id);


/* Debug support */
#define TSRM_ERROR_LEVEL_ERROR	1
#define TSRM_ERROR_LEVEL_CORE	2
#define TSRM_ERROR_LEVEL_INFO	3

typedef void (*tsrm_thread_begin_func_t)(THREAD_T thread_id);
typedef void (*tsrm_thread_end_func_t)(THREAD_T thread_id);
typedef void (*tsrm_shutdown_func_t)(void);


TSRM_API int tsrm_error(int level, const char *format, ...);
TSRM_API void tsrm_error_set(int level, char *debug_filename);

/* utility functions */
TSRM_API THREAD_T tsrm_thread_id(void);
TSRM_API MUTEX_T tsrm_mutex_alloc(void);
TSRM_API void tsrm_mutex_free(MUTEX_T mutexp);
TSRM_API int tsrm_mutex_lock(MUTEX_T mutexp);
TSRM_API int tsrm_mutex_unlock(MUTEX_T mutexp);
#ifdef HAVE_SIGPROCMASK
TSRM_API int tsrm_sigmask(int how, const sigset_t *set, sigset_t *oldset);
#endif

TSRM_API void *tsrm_set_new_thread_begin_handler(tsrm_thread_begin_func_t new_thread_begin_handler);
TSRM_API void *tsrm_set_new_thread_end_handler(tsrm_thread_end_func_t new_thread_end_handler);
TSRM_API void *tsrm_set_shutdown_handler(tsrm_shutdown_func_t shutdown_handler);

/* these 3 APIs should only be used by people that fully understand the threading model
 * used by PHP/Zend and the selected SAPI. */
TSRM_API void *tsrm_new_interpreter_context(void);
TSRM_API void *tsrm_set_interpreter_context(void *new_ctx);
TSRM_API void tsrm_free_interpreter_context(void *context);

TSRM_API void *tsrm_get_ls_cache(void);
TSRM_API uint8_t tsrm_is_main_thread(void);
TSRM_API uint8_t tsrm_is_shutdown(void);
TSRM_API const char *tsrm_api_name(void);

#ifdef TSRM_WIN32
# define TSRM_TLS __declspec(thread)
#else
# define TSRM_TLS __thread
#endif

#define TSRM_SHUFFLE_RSRC_ID(rsrc_id)		((rsrc_id)+1)
#define TSRM_UNSHUFFLE_RSRC_ID(rsrc_id)		((rsrc_id)-1)

#define TSRMLS_FETCH_FROM_CTX(ctx)	void ***tsrm_ls = (void ***) ctx
#define TSRMLS_SET_CTX(ctx)		ctx = (void ***) tsrm_get_ls_cache()
#define TSRMG(id, type, element)	(TSRMG_BULK(id, type)->element)
#define TSRMG_BULK(id, type)	((type) (*((void ***) tsrm_get_ls_cache()))[TSRM_UNSHUFFLE_RSRC_ID(id)])
#define TSRMG_FAST(offset, type, element)	(TSRMG_FAST_BULK(offset, type)->element)
#define TSRMG_FAST_BULK(offset, type)	((type) (((char*) tsrm_get_ls_cache())+(offset)))

#define TSRMG_STATIC(id, type, element)	(TSRMG_BULK_STATIC(id, type)->element)
#define TSRMG_BULK_STATIC(id, type)	((type) (*((void ***) TSRMLS_CACHE))[TSRM_UNSHUFFLE_RSRC_ID(id)])
#define TSRMG_FAST_STATIC(offset, type, element)	(TSRMG_FAST_BULK_STATIC(offset, type)->element)
#define TSRMG_FAST_BULK_STATIC(offset, type)	((type) (((char*) TSRMLS_CACHE)+(offset)))
#define TSRMLS_CACHE_EXTERN() extern TSRM_TLS void *TSRMLS_CACHE;
#define TSRMLS_CACHE_DEFINE() TSRM_TLS void *TSRMLS_CACHE = NULL;
#define TSRMLS_CACHE_UPDATE() TSRMLS_CACHE = tsrm_get_ls_cache()
#define TSRMLS_CACHE _tsrm_ls_cache

/* BC only */
#define TSRMLS_D void
#define TSRMLS_DC
#define TSRMLS_C
#define TSRMLS_CC
#define TSRMLS_FETCH()

#ifdef __cplusplus
}
#endif

#else /* non ZTS */

#define tsrm_env_lock()
#define tsrm_env_unlock()

#define TSRMLS_FETCH()
#define TSRMLS_FETCH_FROM_CTX(ctx)
#define TSRMLS_SET_CTX(ctx)

#define TSRMG_STATIC(id, type, element)
#define TSRMLS_CACHE_EXTERN()
#define TSRMLS_CACHE_DEFINE()
#define TSRMLS_CACHE_UPDATE()
#define TSRMLS_CACHE

#define TSRM_TLS

/* BC only */
#define TSRMLS_D	void
#define TSRMLS_DC
#define TSRMLS_C
#define TSRMLS_CC

#endif /* ZTS */

#endif /* TSRM_H */
