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

#ifdef HAVE_CONFIG_H
# include "tsrm_config.h"
#endif

/* Only compile multi-threading functions if we're in ZTS mode */
#ifdef ZTS

#ifdef WIN32
# define TSRM_WIN32
#endif

#ifdef TSRM_WIN32
# include <windows.h>
#elif defined(GNUPTH)
# include <pth.h>
#elif defined(PTHREADS)
# include <pthread.h>
#endif

typedef int ts_rsrc_id;

#ifdef TSRM_WIN32
#	ifdef TSRM_EXPORTS
#	define TSRM_API __declspec(dllexport)
#	else
#	define TSRM_API __declspec(dllimport)
#	endif
#else
#	define TSRM_API
#endif


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
#elif defined(NSAPI)
# define THREAD_T SYS_THREAD
# define MUTEX_T CRITICAL
#elif defined(PI3WEB)
# define THREAD_T PIThread *
# define MUTEX_T PISync *
#endif

typedef void (*ts_allocate_ctor)(void *);
typedef void (*ts_allocate_dtor)(void *);

#define THREAD_HASH_OF(thr,ts)  (unsigned long)thr%(unsigned long)ts

#ifdef __cplusplus
extern "C" {
#endif

/* startup/shutdown */
TSRM_API int tsrm_startup(int expected_threads, int expected_resources, int debug_level, char *debug_filename);
TSRM_API void tsrm_shutdown(void);

/* allocates a new thread-safe-resource id */
TSRM_API ts_rsrc_id ts_allocate_id(size_t size, ts_allocate_ctor ctor, ts_allocate_dtor dtor);

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
TSRM_API int tsrm_error(int level, const char *format, ...);
TSRM_API void tsrm_error_set(int level, char *debug_filename);

/* utility functions */
TSRM_API THREAD_T tsrm_thread_id(void);
TSRM_API MUTEX_T tsrm_mutex_alloc(void);
TSRM_API void tsrm_mutex_free(MUTEX_T mutexp);
TSRM_API int tsrm_mutex_lock(MUTEX_T mutexp);
TSRM_API int tsrm_mutex_unlock(MUTEX_T mutexp);

TSRM_API void *tsrm_set_new_thread_begin_handler(void (*new_thread_begin_handler)(THREAD_T thread_id));
TSRM_API void *tsrm_set_new_thread_end_handler(void (*new_thread_end_handler)(THREAD_T thread_id));

#ifdef __cplusplus
}
#endif

#endif /* ZTS */

#endif /* TSRM_H */
