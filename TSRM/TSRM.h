/*
   +----------------------------------------------------------------------+
   | Thread Safe Resource Manager                                         |
   +----------------------------------------------------------------------+
   | Copyright (c) 1999, 2000, Andi Gutmans, Sascha Schumann, Zeev Suraski|
   | This source file is subject to the TSRM license, that is bundled     |
   | with this package in the file LICENSE                                |
   +----------------------------------------------------------------------+
   | Authors:  Zeev Suraski <zeev@zend.com>                               |
   +----------------------------------------------------------------------+
*/

#ifndef TSRM_H
#define TSRM_H

/* #ifndef WIN32 */
#if !defined(WIN32) && !defined(NETWARE)
# include "tsrm_config.h"
#endif

#ifdef WIN32
# define TSRM_WIN32
# include "tsrm_config.w32.h"
#endif

#ifdef TSRM_WIN32
#	ifdef TSRM_EXPORTS
#	define TSRM_API __declspec(dllexport)
#	else
#	define TSRM_API __declspec(dllimport)
#	endif
#else
#	define TSRM_API
#endif

/* Only compile multi-threading functions if we're in ZTS mode */
#ifdef ZTS

#ifdef TSRM_WIN32
# include <windows.h>
#elif defined(NETWARE)
# include <nks/thread.h>
#ifdef USE_MPK
# include <mpklib4php.h>
#else
# include <nks/synch.h>
#endif
#elif defined(GNUPTH)
# include <pth.h>
#elif defined(PTHREADS)
# include <pthread.h>
#elif defined(TSRM_ST)
# include <st.h>
#elif defined(BETHREADS)
#include <kernel/OS.h> 
#include <TLS.h>
#endif

typedef int ts_rsrc_id;

/* Define THREAD_T and MUTEX_T */
#ifdef TSRM_WIN32
# define THREAD_T DWORD
# define MUTEX_T CRITICAL_SECTION *
#elif defined(NETWARE)
# define THREAD_T NXThreadId_t
#ifdef USE_MPK
# define MUTEX_T MUTEX
#else
# define MUTEX_T NXMutex_t *
#endif
#elif defined(GNUPTH)
# define THREAD_T pth_t
# define MUTEX_T pth_mutex_t *
#elif defined(PTHREADS)
# define THREAD_T pthread_t
# define MUTEX_T pthread_mutex_t *
#elif defined(NSAPI)
# define THREAD_T SYS_THREAD
# define MUTEX_T CRITICAL
#elif defined(PI3WEB)
# define THREAD_T PIThread *
# define MUTEX_T PISync *
#elif defined(TSRM_ST)
# define THREAD_T st_thread_t
# define MUTEX_T st_mutex_t
#elif defined(BETHREADS)
# define THREAD_T thread_id
typedef struct {
  sem_id sem;
  int32 ben;
} beos_ben;
# define MUTEX_T beos_ben * 
#endif

typedef void (*ts_allocate_ctor)(void *, void ***);
typedef void (*ts_allocate_dtor)(void *, void ***);

#define THREAD_HASH_OF(thr,ts)  (unsigned long)thr%(unsigned long)ts

#ifdef __cplusplus
extern "C" {
#endif

/* startup/shutdown */
TSRM_API int tsrm_startup(int expected_threads, int expected_resources, int debug_level, char *debug_filename);
TSRM_API void tsrm_shutdown(void);

/* allocates a new thread-safe-resource id */
TSRM_API ts_rsrc_id ts_allocate_id(ts_rsrc_id *rsrc_id, size_t size, ts_allocate_ctor ctor, ts_allocate_dtor dtor);

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

typedef void (*tsrm_thread_begin_func_t)(THREAD_T thread_id, void ***tsrm_ls);
typedef void (*tsrm_thread_end_func_t)(THREAD_T thread_id, void ***tsrm_ls);


TSRM_API int tsrm_error(int level, const char *format, ...);
TSRM_API void tsrm_error_set(int level, char *debug_filename);

/* utility functions */
TSRM_API THREAD_T tsrm_thread_id(void);
TSRM_API MUTEX_T tsrm_mutex_alloc(void);
TSRM_API void tsrm_mutex_free(MUTEX_T mutexp);
TSRM_API int tsrm_mutex_lock(MUTEX_T mutexp);
TSRM_API int tsrm_mutex_unlock(MUTEX_T mutexp);

TSRM_API void *tsrm_set_new_thread_begin_handler(tsrm_thread_begin_func_t new_thread_begin_handler);
TSRM_API void *tsrm_set_new_thread_end_handler(tsrm_thread_end_func_t new_thread_end_handler);

#define TSRM_SHUFFLE_RSRC_ID(rsrc_id)		((rsrc_id)+1)
#define TSRM_UNSHUFFLE_RSRC_ID(rsrc_id)		((rsrc_id)-1)

#define TSRMLS_FETCH()			void ***tsrm_ls = (void ***) ts_resource_ex(0, NULL)
#define TSRMG(id, type, element)	(((type) (*((void ***) tsrm_ls))[TSRM_UNSHUFFLE_RSRC_ID(id)])->element)
#define TSRMLS_D	void ***tsrm_ls
#define TSRMLS_DC	, TSRMLS_D
#define TSRMLS_C	tsrm_ls
#define TSRMLS_CC	, TSRMLS_C

#ifdef __cplusplus
}
#endif

#else /* non ZTS */

#define TSRMLS_FETCH()
#define TSRMLS_D	void
#define TSRMLS_DC
#define TSRMLS_C
#define TSRMLS_CC

#endif /* ZTS */

#endif /* TSRM_H */
