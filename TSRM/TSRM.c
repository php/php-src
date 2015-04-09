/*
   +----------------------------------------------------------------------+
   | Thread Safe Resource Manager                                         |
   +----------------------------------------------------------------------+
   | Copyright (c) 1999-2011, Andi Gutmans, Sascha Schumann, Zeev Suraski |
   | This source file is subject to the TSRM license, that is bundled     |
   | with this package in the file LICENSE                                |
   +----------------------------------------------------------------------+
   | Authors:  Zeev Suraski <zeev@zend.com>                               |
   +----------------------------------------------------------------------+
*/

#include "TSRM.h"

#ifdef ZTS

#include <stdio.h>

#if HAVE_STDARG_H
#include <stdarg.h>
#endif

typedef struct _tsrm_tls_entry tsrm_tls_entry;

#if defined(TSRM_WIN32)
/* TSRMLS_CACHE_DEFINE; is already done in Zend, this is being always compiled statically. */
#endif

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
	int done;
} tsrm_resource_type;


/* The memory manager table */
static tsrm_tls_entry	**tsrm_tls_table=NULL;
static int				tsrm_tls_table_size;
static ts_rsrc_id		id_count;

/* The resource sizes table */
static tsrm_resource_type	*resource_types_table=NULL;
static int					resource_types_table_size;


static MUTEX_T tsmm_mutex;	/* thread-safe memory manager mutex */

/* New thread handlers */
static tsrm_thread_begin_func_t tsrm_new_thread_begin_handler;
static tsrm_thread_end_func_t tsrm_new_thread_end_handler;

/* Debug support */
int tsrm_error(int level, const char *format, ...);

/* Read a resource from a thread's resource storage */
static int tsrm_error_level;
static FILE *tsrm_error_file;

#if TSRM_DEBUG
#define TSRM_ERROR(args) tsrm_error args
#define TSRM_SAFE_RETURN_RSRC(array, offset, range)																		\
	{																													\
		int unshuffled_offset = TSRM_UNSHUFFLE_RSRC_ID(offset);															\
																														\
		if (offset==0) {																								\
			return &array;																								\
		} else if ((unshuffled_offset)>=0 && (unshuffled_offset)<(range)) {												\
			TSRM_ERROR((TSRM_ERROR_LEVEL_INFO, "Successfully fetched resource id %d for thread id %ld - 0x%0.8X",		\
						unshuffled_offset, (long) thread_resources->thread_id, array[unshuffled_offset]));				\
			return array[unshuffled_offset];																			\
		} else {																										\
			TSRM_ERROR((TSRM_ERROR_LEVEL_ERROR, "Resource id %d is out of range (%d..%d)",								\
						unshuffled_offset, TSRM_SHUFFLE_RSRC_ID(0), TSRM_SHUFFLE_RSRC_ID(thread_resources->count-1)));	\
			return NULL;																								\
		}																												\
	}
#else
#define TSRM_ERROR(args)
#define TSRM_SAFE_RETURN_RSRC(array, offset, range)		\
	if (offset==0) {									\
		return &array;									\
	} else {											\
		return array[TSRM_UNSHUFFLE_RSRC_ID(offset)];	\
	}
#endif

#if defined(PTHREADS)
/* Thread local storage */
static pthread_key_t tls_key;
# define tsrm_tls_set(what)		pthread_setspecific(tls_key, (void*)(what))
# define tsrm_tls_get()			pthread_getspecific(tls_key)

#elif defined(TSRM_ST)
static int tls_key;
# define tsrm_tls_set(what)		st_thread_setspecific(tls_key, (void*)(what))
# define tsrm_tls_get()			st_thread_getspecific(tls_key)

#elif defined(TSRM_WIN32)
static DWORD tls_key;
# define tsrm_tls_set(what)		TlsSetValue(tls_key, (void*)(what))
# define tsrm_tls_get()			TlsGetValue(tls_key)

#elif defined(BETHREADS)
static int32 tls_key;
# define tsrm_tls_set(what)		tls_set(tls_key, (void*)(what))
# define tsrm_tls_get()			(tsrm_tls_entry*)tls_get(tls_key)

#else
# define tsrm_tls_set(what)
# define tsrm_tls_get()			NULL
# warning tsrm_set_interpreter_context is probably broken on this platform
#endif

/* Startup TSRM (call once for the entire process) */
TSRM_API int tsrm_startup(int expected_threads, int expected_resources, int debug_level, char *debug_filename)
{
#if defined(GNUPTH)
	pth_init();
#elif defined(PTHREADS)
	pthread_key_create( &tls_key, 0 );
#elif defined(TSRM_ST)
	st_init();
	st_key_create(&tls_key, 0);
#elif defined(TSRM_WIN32)
	tls_key = TlsAlloc();
#elif defined(BETHREADS)
	tls_key = tls_allocate();
#endif

	tsrm_error_file = stderr;
	tsrm_error_set(debug_level, debug_filename);
	tsrm_tls_table_size = expected_threads;

	tsrm_tls_table = (tsrm_tls_entry **) calloc(tsrm_tls_table_size, sizeof(tsrm_tls_entry *));
	if (!tsrm_tls_table) {
		TSRM_ERROR((TSRM_ERROR_LEVEL_ERROR, "Unable to allocate TLS table"));
		return 0;
	}
	id_count=0;

	resource_types_table_size = expected_resources;
	resource_types_table = (tsrm_resource_type *) calloc(resource_types_table_size, sizeof(tsrm_resource_type));
	if (!resource_types_table) {
		TSRM_ERROR((TSRM_ERROR_LEVEL_ERROR, "Unable to allocate resource types table"));
		free(tsrm_tls_table);
		tsrm_tls_table = NULL;
		return 0;
	}

	tsmm_mutex = tsrm_mutex_alloc();

	tsrm_new_thread_begin_handler = tsrm_new_thread_end_handler = NULL;

	TSRM_ERROR((TSRM_ERROR_LEVEL_CORE, "Started up TSRM, %d expected threads, %d expected resources", expected_threads, expected_resources));
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
				for (j=0; j<p->count; j++) {
					if (p->storage[j]) {
						if (resource_types_table && !resource_types_table[j].done && resource_types_table[j].dtor) {
							resource_types_table[j].dtor(p->storage[j]);
						}
						free(p->storage[j]);
					}
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
	TSRM_ERROR((TSRM_ERROR_LEVEL_CORE, "Shutdown TSRM"));
	if (tsrm_error_file!=stderr) {
		fclose(tsrm_error_file);
	}
#if defined(GNUPTH)
	pth_kill();
#elif defined(PTHREADS)
	pthread_setspecific(tls_key, 0);
	pthread_key_delete(tls_key);
#elif defined(TSRM_WIN32)
	TlsFree(tls_key);
#endif
}


/* allocates a new thread-safe-resource id */
TSRM_API ts_rsrc_id ts_allocate_id(ts_rsrc_id *rsrc_id, size_t size, ts_allocate_ctor ctor, ts_allocate_dtor dtor)
{
	int i;

	TSRM_ERROR((TSRM_ERROR_LEVEL_CORE, "Obtaining a new resource id, %d bytes", size));

	tsrm_mutex_lock(tsmm_mutex);

	/* obtain a resource id */
	*rsrc_id = TSRM_SHUFFLE_RSRC_ID(id_count++);
	TSRM_ERROR((TSRM_ERROR_LEVEL_CORE, "Obtained resource id %d", *rsrc_id));

	/* store the new resource type in the resource sizes table */
	if (resource_types_table_size < id_count) {
		resource_types_table = (tsrm_resource_type *) realloc(resource_types_table, sizeof(tsrm_resource_type)*id_count);
		if (!resource_types_table) {
			tsrm_mutex_unlock(tsmm_mutex);
			TSRM_ERROR((TSRM_ERROR_LEVEL_ERROR, "Unable to allocate storage for resource"));
			*rsrc_id = 0;
			return 0;
		}
		resource_types_table_size = id_count;
	}
	resource_types_table[TSRM_UNSHUFFLE_RSRC_ID(*rsrc_id)].size = size;
	resource_types_table[TSRM_UNSHUFFLE_RSRC_ID(*rsrc_id)].ctor = ctor;
	resource_types_table[TSRM_UNSHUFFLE_RSRC_ID(*rsrc_id)].dtor = dtor;
	resource_types_table[TSRM_UNSHUFFLE_RSRC_ID(*rsrc_id)].done = 0;

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

	TSRM_ERROR((TSRM_ERROR_LEVEL_CORE, "Successfully allocated new resource id %d", *rsrc_id));
	return *rsrc_id;
}


static void allocate_new_resource(tsrm_tls_entry **thread_resources_ptr, THREAD_T thread_id)
{
	int i;

	TSRM_ERROR((TSRM_ERROR_LEVEL_CORE, "Creating data structures for thread %x", thread_id));
	(*thread_resources_ptr) = (tsrm_tls_entry *) malloc(sizeof(tsrm_tls_entry));
	(*thread_resources_ptr)->storage = (void **) malloc(sizeof(void *)*id_count);
	(*thread_resources_ptr)->count = id_count;
	(*thread_resources_ptr)->thread_id = thread_id;
	(*thread_resources_ptr)->next = NULL;

	/* Set thread local storage to this new thread resources structure */
	tsrm_tls_set(*thread_resources_ptr);

	if (tsrm_new_thread_begin_handler) {
		tsrm_new_thread_begin_handler(thread_id);
	}
	for (i=0; i<id_count; i++) {
		if (resource_types_table[i].done) {
			(*thread_resources_ptr)->storage[i] = NULL;
		} else
		{
			(*thread_resources_ptr)->storage[i] = (void *) malloc(resource_types_table[i].size);
			if (resource_types_table[i].ctor) {
				resource_types_table[i].ctor((*thread_resources_ptr)->storage[i]);
			}
		}
	}

	if (tsrm_new_thread_end_handler) {
		tsrm_new_thread_end_handler(thread_id);
	}

	tsrm_mutex_unlock(tsmm_mutex);
}


/* fetches the requested resource for the current thread */
TSRM_API void *ts_resource_ex(ts_rsrc_id id, THREAD_T *th_id)
{
	THREAD_T thread_id;
	int hash_value;
	tsrm_tls_entry *thread_resources;

#ifdef NETWARE
	/* The below if loop is added for NetWare to fix an abend while unloading PHP
	 * when an Apache unload command is issued on the system console.
	 * While exiting from PHP, at the end for some reason, this function is called
	 * with tsrm_tls_table = NULL. When this happened, the server abends when
	 * tsrm_tls_table is accessed since it is NULL.
	 */
	if(tsrm_tls_table) {
#endif
	if (!th_id) {
		/* Fast path for looking up the resources for the current
		 * thread. Its used by just about every call to
		 * ts_resource_ex(). This avoids the need for a mutex lock
		 * and our hashtable lookup.
		 */
		thread_resources = tsrm_tls_get();

		if (thread_resources) {
			TSRM_ERROR((TSRM_ERROR_LEVEL_INFO, "Fetching resource id %d for current thread %d", id, (long) thread_resources->thread_id));
			/* Read a specific resource from the thread's resources.
			 * This is called outside of a mutex, so have to be aware about external
			 * changes to the structure as we read it.
			 */
			TSRM_SAFE_RETURN_RSRC(thread_resources->storage, id, thread_resources->count);
		}
		thread_id = tsrm_thread_id();
	} else {
		thread_id = *th_id;
	}

	TSRM_ERROR((TSRM_ERROR_LEVEL_INFO, "Fetching resource id %d for thread %ld", id, (long) thread_id));
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
	/* Read a specific resource from the thread's resources.
	 * This is called outside of a mutex, so have to be aware about external
	 * changes to the structure as we read it.
	 */
	TSRM_SAFE_RETURN_RSRC(thread_resources->storage, id, thread_resources->count);
#ifdef NETWARE
	}	/* if(tsrm_tls_table) */
#endif
}

/* frees an interpreter context.  You are responsible for making sure that
 * it is not linked into the TSRM hash, and not marked as the current interpreter */
void tsrm_free_interpreter_context(void *context)
{
	tsrm_tls_entry *next, *thread_resources = (tsrm_tls_entry*)context;
	int i;

	while (thread_resources) {
		next = thread_resources->next;

		for (i=0; i<thread_resources->count; i++) {
			if (resource_types_table[i].dtor) {
				resource_types_table[i].dtor(thread_resources->storage[i]);
			}
		}
		for (i=0; i<thread_resources->count; i++) {
			free(thread_resources->storage[i]);
		}
		free(thread_resources->storage);
		free(thread_resources);
		thread_resources = next;
	}
}

void *tsrm_set_interpreter_context(void *new_ctx)
{
	tsrm_tls_entry *current;

	current = tsrm_tls_get();

	/* TODO: unlink current from the global linked list, and replace it
	 * it with the new context, protected by mutex where/if appropriate */

	/* Set thread local storage to this new thread resources structure */
	tsrm_tls_set(new_ctx);

	/* return old context, so caller can restore it when they're done */
	return current;
}


/* allocates a new interpreter context */
void *tsrm_new_interpreter_context(void)
{
	tsrm_tls_entry *new_ctx, *current;
	THREAD_T thread_id;

	thread_id = tsrm_thread_id();
	tsrm_mutex_lock(tsmm_mutex);

	current = tsrm_tls_get();

	allocate_new_resource(&new_ctx, thread_id);

	/* switch back to the context that was in use prior to our creation
	 * of the new one */
	return tsrm_set_interpreter_context(current);
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
			tsrm_tls_set(0);
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


/* frees all resources allocated for all threads except current */
void ts_free_worker_threads(void)
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
		if (thread_resources->thread_id != thread_id) {
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
			free(thread_resources);
			if (last) {
				thread_resources = last->next;
			} else {
				thread_resources = tsrm_tls_table[hash_value];
			}
		} else {
			if (thread_resources->next) {
				last = thread_resources;
			}
			thread_resources = thread_resources->next;
		}
	}
	tsrm_mutex_unlock(tsmm_mutex);
}


/* deallocates all occurrences of a given id */
void ts_free_id(ts_rsrc_id id)
{
	int i;
	int j = TSRM_UNSHUFFLE_RSRC_ID(id);

	tsrm_mutex_lock(tsmm_mutex);

	TSRM_ERROR((TSRM_ERROR_LEVEL_CORE, "Freeing resource id %d", id));

	if (tsrm_tls_table) {
		for (i=0; i<tsrm_tls_table_size; i++) {
			tsrm_tls_entry *p = tsrm_tls_table[i];

			while (p) {
				if (p->count > j && p->storage[j]) {
					if (resource_types_table && resource_types_table[j].dtor) {
						resource_types_table[j].dtor(p->storage[j]);
					}
					free(p->storage[j]);
					p->storage[j] = NULL;
				}
				p = p->next;
			}
		}
	}
	resource_types_table[j].done = 1;

	tsrm_mutex_unlock(tsmm_mutex);

	TSRM_ERROR((TSRM_ERROR_LEVEL_CORE, "Successfully freed resource id %d", id));
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
#elif defined(TSRM_ST)
	return st_thread_self();
#elif defined(BETHREADS)
	return find_thread(NULL);
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
#elif defined(TSRM_ST)
	mutexp = st_mutex_new();
#elif defined(BETHREADS)
	mutexp = (beos_ben*)malloc(sizeof(beos_ben));
	mutexp->ben = 0;
	mutexp->sem = create_sem(1, "PHP sempahore");
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
		free(mutexp);
#elif defined(GNUPTH)
		free(mutexp);
#elif defined(PTHREADS)
		pthread_mutex_destroy(mutexp);
		free(mutexp);
#elif defined(NSAPI)
		crit_terminate(mutexp);
#elif defined(PI3WEB)
		PISync_delete(mutexp);
#elif defined(TSRM_ST)
		st_mutex_destroy(mutexp);
#elif defined(BETHREADS)
		delete_sem(mutexp->sem);
		free(mutexp);
#endif
	}
#ifdef THR_DEBUG
	printf("Mutex freed thread: %d\n",mythreadid());
#endif
}


/*
  Lock a mutex.
  A return value of 0 indicates success
*/
TSRM_API int tsrm_mutex_lock(MUTEX_T mutexp)
{
	TSRM_ERROR((TSRM_ERROR_LEVEL_INFO, "Mutex locked thread: %ld", tsrm_thread_id()));
#ifdef TSRM_WIN32
	EnterCriticalSection(mutexp);
	return 0;
#elif defined(GNUPTH)
	if (pth_mutex_acquire(mutexp, 0, NULL)) {
		return 0;
	}
	return -1;
#elif defined(PTHREADS)
	return pthread_mutex_lock(mutexp);
#elif defined(NSAPI)
	crit_enter(mutexp);
	return 0;
#elif defined(PI3WEB)
	return PISync_lock(mutexp);
#elif defined(TSRM_ST)
	return st_mutex_lock(mutexp);
#elif defined(BETHREADS)
	if (atomic_add(&mutexp->ben, 1) != 0)
		return acquire_sem(mutexp->sem);
	return 0;
#endif
}


/*
  Unlock a mutex.
  A return value of 0 indicates success
*/
TSRM_API int tsrm_mutex_unlock(MUTEX_T mutexp)
{
	TSRM_ERROR((TSRM_ERROR_LEVEL_INFO, "Mutex unlocked thread: %ld", tsrm_thread_id()));
#ifdef TSRM_WIN32
	LeaveCriticalSection(mutexp);
	return 0;
#elif defined(GNUPTH)
	if (pth_mutex_release(mutexp)) {
		return 0;
	}
	return -1;
#elif defined(PTHREADS)
	return pthread_mutex_unlock(mutexp);
#elif defined(NSAPI)
	crit_exit(mutexp);
	return 0;
#elif defined(PI3WEB)
	return PISync_unlock(mutexp);
#elif defined(TSRM_ST)
	return st_mutex_unlock(mutexp);
#elif defined(BETHREADS)
	if (atomic_add(&mutexp->ben, -1) != 1)
		return release_sem(mutexp->sem);
	return 0;
#endif
}

/*
  Changes the signal mask of the calling thread
*/
#ifdef HAVE_SIGPROCMASK
TSRM_API int tsrm_sigmask(int how, const sigset_t *set, sigset_t *oldset)
{
	TSRM_ERROR((TSRM_ERROR_LEVEL_INFO, "Changed sigmask in thread: %ld", tsrm_thread_id()));
	/* TODO: add support for other APIs */
#ifdef PTHREADS
	return pthread_sigmask(how, set, oldset);
#else
	return sigprocmask(how, set, oldset);
#endif
}
#endif


TSRM_API void *tsrm_set_new_thread_begin_handler(tsrm_thread_begin_func_t new_thread_begin_handler)
{
	void *retval = (void *) tsrm_new_thread_begin_handler;

	tsrm_new_thread_begin_handler = new_thread_begin_handler;
	return retval;
}


TSRM_API void *tsrm_set_new_thread_end_handler(tsrm_thread_end_func_t new_thread_end_handler)
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

TSRM_API void *tsrm_get_ls_cache(void)
{
	return tsrm_tls_get();
}

#endif /* ZTS */
