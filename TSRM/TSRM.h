/*
   +----------------------------------------------------------------------+
   | Thread Safe Resource Manager                                         |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998, 1999 Zeev Suraski                                |
   +----------------------------------------------------------------------+
   | This source file is subject to the Zend license, that is bundled     |
   | with this package in the file LICENSE.  If you did not receive a     |
   | copy of the Zend license, please mail us at zend@zend.com so we can  |
   | send you a copy immediately.                                         |
   +----------------------------------------------------------------------+
   | Author:  Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/


#ifndef _TSRM_H
#define _TSRM_H

#include <windows.h>

typedef int ts_rsrc_id;


/* Define TSRM_FUNC */
#ifdef __cplusplus
#define TSRM_FUNC	extern "C"
#else
#define TSRM_FUNC
#endif


/* Define THREAD_T and MUTEX_T */
#if defined(WIN32)
# define THREAD_T DWORD
# define MUTEX_T void *
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


#define THREAD_HASH_OF(thr,ts)  thr%ts


/* startup/shutdown */
TSRM_FUNC int tsrm_startup(int expected_threads, int expected_resources, int debug_status);
TSRM_FUNC void tsrm_shutdown();

/* allocates a new thread-safe-resource id */
TSRM_FUNC ts_rsrc_id ts_allocate_id(size_t size, void (*ctor)(void *resource), void (*dtor)(void *resource));

/* fetches the requested resource for the current thread */
TSRM_FUNC void *ts_resource(ts_rsrc_id id);

/* frees all resources allocated for the current thread */
TSRM_FUNC void ts_free_thread();

/* deallocates all occurrences of a given id */
TSRM_FUNC void ts_free_id(ts_rsrc_id id);


/* Debug support */
TSRM_FUNC void tsrm_debug_set(int status);

/* utility functions */
TSRM_FUNC THREAD_T tsrm_thread_id(void);
TSRM_FUNC MUTEX_T tsrm_mutex_alloc(void);
TSRM_FUNC void tsrm_mutex_free(MUTEX_T mutexp);
TSRM_FUNC int tsrm_mutex_lock(MUTEX_T mutexp);
TSRM_FUNC int tsrm_mutex_unlock(MUTEX_T mutexp);

#endif /* _TSRM_H */