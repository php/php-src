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

#include "TSRM.h"

#ifdef ZTS

#include <stdio.h>
#include <stdlib.h>

#if HAVE_STDARG_H
#include <stdarg.h>
#endif

typedef struct _tsrm_tls_entry tsrm_tls_entry;

struct _tsrm_tls_entry {
	void **storage;
	int count;
	THREAD_T thread_id;
	tsrm_tls_entry *next;
};


typedef struct {
	size_t size;
	ts_allocate_ctor ctor;
	ts_allocate_dtor dtor;
} tsrm_resource_type;


#define TSRM_SHUFFLE_RSRC_ID(rsrc_id)		((rsrc_id)+1)
#define TSRM_UNSHUFFLE_RSRC_ID(rsrc_id)		((rsrc_id)-1)

/* The memory manager table */
static tsrm_tls_entry	**tsrm_tls_table=NULL;
static int				tsrm_tls_table_size;
static ts_rsrc_id		id_count;

/* The resource sizes table */
static tsrm_resource_type	*resource_types_table=NULL;
static int					resource_types_table_size;


static MUTEX_T tsmm_mutex;	/* thread-safe memory manager mutex */

/* New thread handlers */
static void (*tsrm_new_thread_begin_handler)();
static void (*tsrm_new_thread_end_handler)();

/* Debug support */
int tsrm_error(int level, const char *format, ...);

/* Read a resource from a thread's resource storage */
void *ts_resource_read( tsrm_tls_entry *thread_resources, ts_rsrc_id id );
static int tsrm_error_level;
static FILE *tsrm_error_file;
#if TSRM_DEBUG
#define TSRM_ERROR tsrm_error
#define TSRM_SAFE_ARRAY_OFFSET(array, offset, range)	(((offset)>=0 && (offset)<(range)) ? array[offset] : NULL)
#else
#define TSRM_ERROR
#define TSRM_SAFE_ARRAY_OFFSET(array, offset, range)	array[offset]
#endif

#if defined(PTHREADS)
/* Thread local storage */
static pthread_key_t tls_key;
#endif


/* Startup TSRM (call once for the entire process) */
TSRM_API int tsrm_startup(int expected_threads, int expected_resources, int debug_level, char *debug_filename)
{
#if defined(GNUPTH)
	pth_init();
#elif defined(PTHREADS)
	pthread_key_create( &tls_key, 0 );
#endif

	tsrm_error_file = stderr;
	tsrm_error_set(debug_level, debug_filename);
	tsrm_tls_table_size = expected_threads;

	tsrm_tls_table = (tsrm_tls_entry **) calloc(tsrm_tls_table_size, sizeof(tsrm_tls_entry *));
	if (!tsrm_tls_table) {
		TSRM_ERROR(TSRM_ERROR_LEVEL_ERROR, "Unable to allocate TLS table");
		return 0;
	}
	id_count=0;

	resource_types_table_size = expected_resources;
	resource_types_table = (tsrm_resource_type *) calloc(resource_types_table_size, sizeof(tsrm_resource_type));
	if (!resource_types_table) {
		TSRM_ERROR(TSRM_ERROR_LEVEL_ERROR, "Unable to allocate resource types table");
		free(tsrm_tls_table);
		tsrm_tls_table = NULL;
		return 0;
	}

	tsmm_mutex = tsrm_mutex_alloc();

	tsrm_new_thread_begin_handler = tsrm_new_thread_end_handler = NULL;

	TSRM_ERROR(TSRM_ERROR_LEVEL_CORE, "Started up TSRM, %d expected threads, %d expected resources", expected_threads, expected_resources);
	return 1;
}


/* Shutdown TSRM (call once for the entire process) */
TSRM_API void tsrm_shutdown(void)
{
	int i;

	if (tsrm_tls_table) {
		for (i=0; i<tsrm_tls_table_size; i++) {
			tsrm_tls_entry *p = tsrm_tls_table[i], *next_p;

			while (p) {
				int j;

				next_p = p->next;
				for (j=0; j<id_count; j++) {
					free(p->storage[j]);
				}
				free(p->storage);
				free(p);
				p = next_p;
			}
		}
		free(tsrm_tls_table);
		tsrm_tls_table = NULL;
	}
	if (resource_types_table) {
		free(resource_types_table);
		resource_types_table=NULL;
	}
	tsrm_mutex_free(tsmm_mutex);
	tsmm_mutex = NULL;
	TSRM_ERROR(TSRM_ERROR_LEVEL_CORE, "Shutdown TSRM");
	if (tsrm_error_file!=stderr) {
		fclose(tsrm_error_file);
	}
#if defined(GNUPTH)
	pth_kill();
#elif defined(PTHREADS)
	pthread_key_delete( tls_key );
#endif
}


/* allocates a new thread-safe-resource id */
TSRM_API ts_rsrc_id ts_allocate_id(size_t size, ts_allocate_ctor ctor, ts_allocate_dtor dtor)
{
	ts_rsrc_id new_id;
	int i;

	TSRM_ERROR(TSRM_ERROR_LEVEL_CORE, "Obtaining a new resource id, %d bytes", size);

	tsrm_mutex_lock(tsmm_mutex);

	/* obtain a resource id */
	new_id = id_count++;
	TSRM_ERROR(TSRM_ERROR_LEVEL_CORE, "Obtained resource id %d", TSRM_SHUFFLE_RSRC_ID(new_id));

	/* store the new resource type in the resource sizes table */
	if (resource_types_table_size < id_count) {
		resource_types_table = (tsrm_resource_type *) realloc(resource_types_table, sizeof(tsrm_resource_type)*id_count);
		if (!resource_types_table) {
			tsrm_mutex_unlock(tsmm_mutex);
			TSRM_ERROR(TSRM_ERROR_LEVEL_ERROR, "Unable to allocate storage for resource");
			return 0;
		}
		resource_types_table_size = id_count;
	}
	resource_types_table[new_id].size = size;
	resource_types_table[new_id].ctor = ctor;
	resource_types_table[new_id].dtor = dtor;

	/* enlarge the arrays for the already active threads */
	for (i=0; i<tsrm_tls_table_size; i++) {
		tsrm_tls_entry *p = tsrm_tls_table[i];

		while (p) {
			if (p->count < id_count) {
				int j;

				p->storage = (void *) realloc(p->storage, sizeof(void *)*id_count);
				for (j=p->count; j<id_count; j++) {
					p->storage[j] = (void *) malloc(resource_types_table[j].size);
					if (resource_types_table[j].ctor) {
						resource_types_table[j].ctor(p->storage[j]);
					}
				}
				p->count = id_count;
			}
			p = p->next;
		}
	}
	tsrm_mutex_unlock(tsmm_mutex);

	TSRM_ERROR(TSRM_ERROR_LEVEL_CORE, "Successfully allocated new resource id %d", TSRM_SHUFFLE_RSRC_ID(new_id));
	return TSRM_SHUFFLE_RSRC_ID(new_id);
}


static void allocate_new_resource(tsrm_tls_entry **thread_resources_ptr, THREAD_T thread_id)
{
	int i;

	TSRM_ERROR(TSRM_ERROR_LEVEL_CORE, "Creating data structures for thread %x", thread_id);
	(*thread_resources_ptr) = (tsrm_tls_entry *) malloc(sizeof(tsrm_tls_entry));
	(*thread_resources_ptr)->storage = (void **) malloc(sizeof(void *)*id_count);
	(*thread_resources_ptr)->count = id_count;
	(*thread_resources_ptr)->thread_id = thread_id;
	(*thread_resources_ptr)->next = NULL;

#if defined(PTHREADS)
	/* Set thread local storage to this new thread resources structure */
	pthread_setspecific( tls_key, (void *)*thread_resources_ptr );
#endif

	if (tsrm_new_thread_begin_handler) {
		tsrm_new_thread_begin_handler(thread_id);
	}
	for (i=0; i<id_count; i++) {
		(*thread_resources_ptr)->storage[i] = (void *) malloc(resource_types_table[i].size);
		if (resource_types_table[i].ctor) {
			resource_types_table[i].ctor((*thread_resources_ptr)->storage[i]);
		}
	}

	tsrm_mutex_unlock(tsmm_mutex);

	if (tsrm_new_thread_end_handler) {
		tsrm_new_thread_end_handler(thread_id);
	}
}


/* fetches the requested resource for the current thread */
TSRM_API void *ts_resource_ex(ts_rsrc_id id, THREAD_T *th_id)
{
	THREAD_T thread_id;
	int hash_value;
	tsrm_tls_entry *thread_resources;

	if (!th_id) {
#if defined(PTHREADS)
		/* Fast path for looking up the resources for the current
		* thread. Its used by just about every call to
		* ts_resource_ex(). This avoids the need for a mutex lock
		* and our hashtable lookup.
		*/
		thread_resources = pthread_getspecific( tls_key );
		if (thread_resources) {
			TSRM_ERROR(TSRM_ERROR_LEVEL_INFO, "Fetching resource id %d for current thread %d", id, (long) thread_resources->thread_id );
			return ts_resource_read( thread_resources, id );
		}
#endif
		thread_id = tsrm_thread_id();
	} else {
		thread_id = *th_id;
	}

	TSRM_ERROR(TSRM_ERROR_LEVEL_INFO, "Fetching resource id %d for thread %ld", id, (long) thread_id);
	tsrm_mutex_lock(tsmm_mutex);

	hash_value = THREAD_HASH_OF(thread_id, tsrm_tls_table_size);
	thread_resources = tsrm_tls_table[hash_value];

	if (!thread_resources) {
		allocate_new_resource(&tsrm_tls_table[hash_value], thread_id);
		return ts_resource_ex(id, &thread_id);
	} else {
		 do {
			if (thread_resources->thread_id == thread_id) {
				break;
			}
			if (thread_resources->next) {
				thread_resources = thread_resources->next;
			} else {
				allocate_new_resource(&thread_resources->next, thread_id);
				return ts_resource_ex(id, &thread_id);
				/*
				 * thread_resources = thread_resources->next;
				 * break;
				 */
			}
		 } while (thread_resources);
	}
	tsrm_mutex_unlock(tsmm_mutex);
	return ts_resource_read( thread_resources, id );
}


/* Read a specific resource from the thread's resources.
 * This is called outside of a mutex, so have to be aware about external
 * changes to the structure as we read it.
 */
void *ts_resource_read( tsrm_tls_entry *thread_resources, ts_rsrc_id id )
{
	void *resource;

	resource = TSRM_SAFE_ARRAY_OFFSET(thread_resources->storage, TSRM_UNSHUFFLE_RSRC_ID(id), thread_resources->count);
	if (resource) {
		TSRM_ERROR(TSRM_ERROR_LEVEL_INFO, "Successfully fetched resource id %d for thread id %ld - %x", id, (long) thread_resources->thread_id, (long) resource);
	} else {
		TSRM_ERROR(TSRM_ERROR_LEVEL_ERROR, "Resource id %d is out of range (%d..%d)", id, TSRM_SHUFFLE_RSRC_ID(0), TSRM_SHUFFLE_RSRC_ID(thread_resources->count-1));
		abort();
	}
	return resource;
}


/* frees all resources allocated for the current thread */
void ts_free_thread(void)
{
	tsrm_tls_entry *thread_resources;
	int i;
	THREAD_T thread_id = tsrm_thread_id();
	int hash_value;
	tsrm_tls_entry *last=NULL;

	tsrm_mutex_lock(tsmm_mutex);
	hash_value = THREAD_HASH_OF(thread_id, tsrm_tls_table_size);
	thread_resources = tsrm_tls_table[hash_value];

	while (thread_resources) {
		if (thread_resources->thread_id == thread_id) {
			for (i=0; i<thread_resources->count; i++) {
				if (resource_types_table[i].dtor) {
					resource_types_table[i].dtor(thread_resources->storage[i]);
				}
			}
			for (i=0; i<thread_resources->count; i++) {
				free(thread_resources->storage[i]);
			}
			free(thread_resources->storage);
			if (last) {
				last->next = thread_resources->next;
			} else {
				tsrm_tls_table[hash_value] = thread_resources->next;
			}
#if defined(PTHREADS)
			pthread_setspecific(tls_key, 0);
#endif
			free(thread_resources);
			break;
		}
		if (thread_resources->next) {
			last = thread_resources;
		}
		thread_resources = thread_resources->next;
	}
	tsrm_mutex_unlock(tsmm_mutex);
}


/* deallocates all occurrences of a given id */
void ts_free_id(ts_rsrc_id id)
{
}




/*
 * Utility Functions
 */

/* Obtain the current thread id */
TSRM_API THREAD_T tsrm_thread_id(void)
{
#ifdef TSRM_WIN32
	return GetCurrentThreadId();
#elif defined(GNUPTH)
	return pth_self();
#elif defined(PTHREADS)
	return pthread_self();
#elif defined(NSAPI)
	return systhread_current();
#elif defined(PI3WEB)
	return PIThread_getCurrent();
#endif
}


/* Allocate a mutex */
TSRM_API MUTEX_T tsrm_mutex_alloc(void)
{
    MUTEX_T mutexp;

#ifdef TSRM_WIN32
    mutexp = malloc(sizeof(CRITICAL_SECTION));
	InitializeCriticalSection(mutexp);
#elif defined(GNUPTH)
	mutexp = (MUTEX_T) malloc(sizeof(*mutexp));
	pth_mutex_init(mutexp);
#elif defined(PTHREADS)
	mutexp = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(mutexp,NULL);
#elif defined(NSAPI)
	mutexp = crit_init();
#elif defined(PI3WEB)
	mutexp = PIPlatform_allocLocalMutex();
#endif
#ifdef THR_DEBUG
		printf("Mutex created thread: %d\n",mythreadid());
#endif
    return( mutexp );
}


/* Free a mutex */
TSRM_API void tsrm_mutex_free(MUTEX_T mutexp)
{
    if (mutexp) {
#ifdef TSRM_WIN32
		DeleteCriticalSection(mutexp);
#elif defined(GNUPTH)
		free(mutexp);
#elif defined(PTHREADS)
		pthread_mutex_destroy(mutexp);
		free(mutexp);
#elif defined(NSAPI)
		crit_terminate(mutexp);
#elif defined(PI3WEB)
		PISync_delete(mutexp) 
#endif
    }
#ifdef THR_DEBUG
		printf("Mutex freed thread: %d\n",mythreadid());
#endif
}


/* Lock a mutex */
TSRM_API int tsrm_mutex_lock(MUTEX_T mutexp)
{
	TSRM_ERROR(TSRM_ERROR_LEVEL_INFO, "Mutex locked thread: %ld", tsrm_thread_id());
#ifdef TSRM_WIN32
	EnterCriticalSection(mutexp);
	return 1;
#elif defined(GNUPTH)
	return pth_mutex_acquire(mutexp, 0, NULL);
#elif defined(PTHREADS)
	return pthread_mutex_lock(mutexp);
#elif defined(NSAPI)
	return crit_enter(mutexp);
#elif defined(PI3WEB)
	return PISync_lock(mutexp);
#endif
}


/* Unlock a mutex */
TSRM_API int tsrm_mutex_unlock(MUTEX_T mutexp)
{
	TSRM_ERROR(TSRM_ERROR_LEVEL_INFO, "Mutex unlocked thread: %ld", tsrm_thread_id());
#ifdef TSRM_WIN32
	LeaveCriticalSection(mutexp);
	return 1;
#elif defined(GNUPTH)
	return pth_mutex_release(mutexp);
#elif defined(PTHREADS)
	return pthread_mutex_unlock(mutexp);
#elif defined(NSAPI)
	return crit_exit(mutexp);
#elif defined(PI3WEB)
	return PISync_unlock(mutexp);
#endif
}


TSRM_API void *tsrm_set_new_thread_begin_handler(void (*new_thread_begin_handler)(THREAD_T thread_id))
{
	void *retval = (void *) tsrm_new_thread_begin_handler;

	tsrm_new_thread_begin_handler = new_thread_begin_handler;
	return retval;
}


TSRM_API void *tsrm_set_new_thread_end_handler(void (*new_thread_end_handler)(THREAD_T thread_id))
{
	void *retval = (void *) tsrm_new_thread_end_handler;

	tsrm_new_thread_end_handler = new_thread_end_handler;
	return retval;
}



/*
 * Debug support
 */

#if TSRM_DEBUG
int tsrm_error(int level, const char *format, ...)
{
	if (level<=tsrm_error_level) {
		va_list args;
		int size;

		fprintf(tsrm_error_file, "TSRM:  ");
		va_start(args, format);
		size = vfprintf(tsrm_error_file, format, args);
		va_end(args);
		fprintf(tsrm_error_file, "\n");
		fflush(tsrm_error_file);
		return size;
	} else {
		return 0;
	}
}
#endif


void tsrm_error_set(int level, char *debug_filename)
{
	tsrm_error_level = level;

#if TSRM_DEBUG
	if (tsrm_error_file!=stderr) { /* close files opened earlier */
		fclose(tsrm_error_file);
	}

	if (debug_filename) {
		tsrm_error_file = fopen(debug_filename, "w");
		if (!tsrm_error_file) {
			tsrm_error_file = stderr;
		}
	} else {
		tsrm_error_file = stderr;
	}
#endif
}

#endif /* ZTS */
