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

#include "TSRM.h"

#ifdef ZTS

#include <stdio.h>
#include <stdarg.h>

#if ZEND_DEBUG
# include <assert.h>
# define TSRM_ASSERT(c) assert(c)
#else
# define TSRM_ASSERT(c)
#endif

typedef struct _tsrm_tls_entry tsrm_tls_entry;

/* TSRMLS_CACHE_DEFINE; is already done in Zend, this is being always compiled statically. */
TSRMLS_CACHE_EXTERN();

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
	size_t fast_offset;
	int done;
} tsrm_resource_type;


/* The memory manager table */
static tsrm_tls_entry	**tsrm_tls_table=NULL;
static int				tsrm_tls_table_size;
static ts_rsrc_id		id_count;

/* The resource sizes table */
static tsrm_resource_type	*resource_types_table=NULL;
static int					resource_types_table_size;

/* Reserved space for fast globals access */
static size_t tsrm_reserved_pos  = 0;
static size_t tsrm_reserved_size = 0;

static MUTEX_T tsmm_mutex;	  /* thread-safe memory manager mutex */
static MUTEX_T tsrm_env_mutex; /* tsrm environ mutex */

/* New thread handlers */
static tsrm_thread_begin_func_t tsrm_new_thread_begin_handler = NULL;
static tsrm_thread_end_func_t tsrm_new_thread_end_handler = NULL;
static tsrm_shutdown_func_t tsrm_shutdown_handler = NULL;

/* Debug support */
int tsrm_error(int level, const char *format, ...);

/* Read a resource from a thread's resource storage */
static int tsrm_error_level;
static FILE *tsrm_error_file;

#ifdef TSRM_DEBUG
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

#ifdef TSRM_WIN32
static DWORD tls_key;
# define tsrm_tls_set(what)		TlsSetValue(tls_key, (void*)(what))
# define tsrm_tls_get()			TlsGetValue(tls_key)
#else
static pthread_key_t tls_key;
# define tsrm_tls_set(what)		pthread_setspecific(tls_key, (void*)(what))
# define tsrm_tls_get()			pthread_getspecific(tls_key)
#endif

TSRM_TLS bool in_main_thread = false;
TSRM_TLS bool is_thread_shutdown = false;

/* Startup TSRM (call once for the entire process) */
TSRM_API bool tsrm_startup(int expected_threads, int expected_resources, int debug_level, const char *debug_filename)
{/*{{{*/
#ifdef TSRM_WIN32
	tls_key = TlsAlloc();
#else
	pthread_key_create(&tls_key, 0);
#endif

	/* ensure singleton */
	in_main_thread = true;
	is_thread_shutdown = false;

	tsrm_error_file = stderr;
	tsrm_error_set(debug_level, debug_filename);
	tsrm_tls_table_size = expected_threads;

	tsrm_tls_table = (tsrm_tls_entry **) calloc(tsrm_tls_table_size, sizeof(tsrm_tls_entry *));
	if (!tsrm_tls_table) {
		TSRM_ERROR((TSRM_ERROR_LEVEL_ERROR, "Unable to allocate TLS table"));
		is_thread_shutdown = true;
		return 0;
	}
	id_count=0;

	resource_types_table_size = expected_resources;
	resource_types_table = (tsrm_resource_type *) calloc(resource_types_table_size, sizeof(tsrm_resource_type));
	if (!resource_types_table) {
		TSRM_ERROR((TSRM_ERROR_LEVEL_ERROR, "Unable to allocate resource types table"));
		is_thread_shutdown = true;
		free(tsrm_tls_table);
		return 0;
	}

	tsmm_mutex = tsrm_mutex_alloc();

	TSRM_ERROR((TSRM_ERROR_LEVEL_CORE, "Started up TSRM, %d expected threads, %d expected resources", expected_threads, expected_resources));

	tsrm_reserved_pos  = 0;
	tsrm_reserved_size = 0;

	tsrm_env_mutex = tsrm_mutex_alloc();

	return 1;
}/*}}}*/

static void ts_free_resources(tsrm_tls_entry *thread_resources)
{
	/* Need to destroy in reverse order to respect dependencies. */
	for (int i = thread_resources->count - 1; i >= 0; i--) {
		if (!resource_types_table[i].done) {
			if (resource_types_table[i].dtor) {
				resource_types_table[i].dtor(thread_resources->storage[i]);
			}

			if (!resource_types_table[i].fast_offset) {
				free(thread_resources->storage[i]);
			}
		}
	}

	free(thread_resources->storage);
}

/* Shutdown TSRM (call once for the entire process) */
TSRM_API void tsrm_shutdown(void)
{/*{{{*/
	if (is_thread_shutdown) {
		/* shutdown must only occur once */
		return;
	}

	is_thread_shutdown = true;

	if (!in_main_thread) {
		/* only the main thread may shutdown tsrm */
		return;
	}

	for (int i=0; i<tsrm_tls_table_size; i++) {
		tsrm_tls_entry *p = tsrm_tls_table[i], *next_p;

		while (p) {
			next_p = p->next;
			if (resource_types_table) {
				/* This call will already free p->storage for us */
				ts_free_resources(p);
			} else {
				free(p->storage);
			}
			free(p);
			p = next_p;
		}
	}
	free(tsrm_tls_table);
	free(resource_types_table);
	tsrm_mutex_free(tsmm_mutex);
	tsrm_mutex_free(tsrm_env_mutex);
	TSRM_ERROR((TSRM_ERROR_LEVEL_CORE, "Shutdown TSRM"));
	if (tsrm_error_file!=stderr) {
		fclose(tsrm_error_file);
	}
#ifdef TSRM_WIN32
	TlsFree(tls_key);
#else
	pthread_setspecific(tls_key, 0);
	pthread_key_delete(tls_key);
#endif
	if (tsrm_shutdown_handler) {
		tsrm_shutdown_handler();
	}
	tsrm_new_thread_begin_handler = NULL;
	tsrm_new_thread_end_handler = NULL;
	tsrm_shutdown_handler = NULL;

	tsrm_reserved_pos  = 0;
	tsrm_reserved_size = 0;
}/*}}}*/

/* {{{ */
/* environ lock api */
TSRM_API void tsrm_env_lock(void) {
	tsrm_mutex_lock(tsrm_env_mutex);
}

TSRM_API void tsrm_env_unlock(void) {
	tsrm_mutex_unlock(tsrm_env_mutex);
} /* }}} */

/* enlarge the arrays for the already active threads */
static void tsrm_update_active_threads(void)
{/*{{{*/
	for (int i=0; i<tsrm_tls_table_size; i++) {
		tsrm_tls_entry *p = tsrm_tls_table[i];

		while (p) {
			if (p->count < id_count) {
				int j;

				p->storage = (void *) realloc(p->storage, sizeof(void *)*id_count);
				for (j=p->count; j<id_count; j++) {
					if (resource_types_table[j].fast_offset) {
						p->storage[j] = (void *) (((char*)p) + resource_types_table[j].fast_offset);
					} else {
						p->storage[j] = (void *) malloc(resource_types_table[j].size);
					}
					if (resource_types_table[j].ctor) {
						resource_types_table[j].ctor(p->storage[j]);
					}
				}
				p->count = id_count;
			}
			p = p->next;
		}
	}
}/*}}}*/


/* allocates a new thread-safe-resource id */
TSRM_API ts_rsrc_id ts_allocate_id(ts_rsrc_id *rsrc_id, size_t size, ts_allocate_ctor ctor, ts_allocate_dtor dtor)
{/*{{{*/
	TSRM_ERROR((TSRM_ERROR_LEVEL_CORE, "Obtaining a new resource id, %d bytes", size));

	tsrm_mutex_lock(tsmm_mutex);

	/* obtain a resource id */
	*rsrc_id = TSRM_SHUFFLE_RSRC_ID(id_count++);
	TSRM_ERROR((TSRM_ERROR_LEVEL_CORE, "Obtained resource id %d", *rsrc_id));

	/* store the new resource type in the resource sizes table */
	if (resource_types_table_size < id_count) {
		tsrm_resource_type *_tmp;
		_tmp = (tsrm_resource_type *) realloc(resource_types_table, sizeof(tsrm_resource_type)*id_count);
		if (!_tmp) {
			TSRM_ERROR((TSRM_ERROR_LEVEL_ERROR, "Unable to allocate storage for resource"));
			*rsrc_id = 0;
			tsrm_mutex_unlock(tsmm_mutex);
			return 0;
		}
		resource_types_table = _tmp;
		resource_types_table_size = id_count;
	}
	resource_types_table[TSRM_UNSHUFFLE_RSRC_ID(*rsrc_id)].size = size;
	resource_types_table[TSRM_UNSHUFFLE_RSRC_ID(*rsrc_id)].ctor = ctor;
	resource_types_table[TSRM_UNSHUFFLE_RSRC_ID(*rsrc_id)].dtor = dtor;
	resource_types_table[TSRM_UNSHUFFLE_RSRC_ID(*rsrc_id)].fast_offset = 0;
	resource_types_table[TSRM_UNSHUFFLE_RSRC_ID(*rsrc_id)].done = 0;

	tsrm_update_active_threads();
	tsrm_mutex_unlock(tsmm_mutex);

	TSRM_ERROR((TSRM_ERROR_LEVEL_CORE, "Successfully allocated new resource id %d", *rsrc_id));
	return *rsrc_id;
}/*}}}*/


/* Reserve space for fast thread-safe-resources */
TSRM_API void tsrm_reserve(size_t size)
{/*{{{*/
	tsrm_reserved_pos  = 0;
	tsrm_reserved_size = TSRM_ALIGNED_SIZE(size);
}/*}}}*/


/* allocates a new fast thread-safe-resource id */
TSRM_API ts_rsrc_id ts_allocate_fast_id(ts_rsrc_id *rsrc_id, size_t *offset, size_t size, ts_allocate_ctor ctor, ts_allocate_dtor dtor)
{/*{{{*/
	TSRM_ERROR((TSRM_ERROR_LEVEL_CORE, "Obtaining a new fast resource id, %d bytes", size));

	tsrm_mutex_lock(tsmm_mutex);

	/* obtain a resource id */
	*rsrc_id = TSRM_SHUFFLE_RSRC_ID(id_count++);
	TSRM_ERROR((TSRM_ERROR_LEVEL_CORE, "Obtained resource id %d", *rsrc_id));

	size = TSRM_ALIGNED_SIZE(size);
	if (tsrm_reserved_size - tsrm_reserved_pos < size) {
		TSRM_ERROR((TSRM_ERROR_LEVEL_ERROR, "Unable to allocate space for fast resource"));
		*rsrc_id = 0;
		*offset = 0;
		tsrm_mutex_unlock(tsmm_mutex);
		return 0;
	}

	*offset = TSRM_ALIGNED_SIZE(sizeof(tsrm_tls_entry)) + tsrm_reserved_pos;
	tsrm_reserved_pos += size;

	/* store the new resource type in the resource sizes table */
	if (resource_types_table_size < id_count) {
		tsrm_resource_type *_tmp;
		_tmp = (tsrm_resource_type *) realloc(resource_types_table, sizeof(tsrm_resource_type)*id_count);
		if (!_tmp) {
			TSRM_ERROR((TSRM_ERROR_LEVEL_ERROR, "Unable to allocate storage for resource"));
			*rsrc_id = 0;
			tsrm_mutex_unlock(tsmm_mutex);
			return 0;
		}
		resource_types_table = _tmp;
		resource_types_table_size = id_count;
	}
	resource_types_table[TSRM_UNSHUFFLE_RSRC_ID(*rsrc_id)].size = size;
	resource_types_table[TSRM_UNSHUFFLE_RSRC_ID(*rsrc_id)].ctor = ctor;
	resource_types_table[TSRM_UNSHUFFLE_RSRC_ID(*rsrc_id)].dtor = dtor;
	resource_types_table[TSRM_UNSHUFFLE_RSRC_ID(*rsrc_id)].fast_offset = *offset;
	resource_types_table[TSRM_UNSHUFFLE_RSRC_ID(*rsrc_id)].done = 0;

	tsrm_update_active_threads();
	tsrm_mutex_unlock(tsmm_mutex);

	TSRM_ERROR((TSRM_ERROR_LEVEL_CORE, "Successfully allocated new resource id %d", *rsrc_id));
	return *rsrc_id;
}/*}}}*/

static void set_thread_local_storage_resource_to(tsrm_tls_entry *thread_resource)
{
	tsrm_tls_set(thread_resource);
	TSRMLS_CACHE = thread_resource;
}

/* Must be called with tsmm_mutex held */
static void allocate_new_resource(tsrm_tls_entry **thread_resources_ptr, THREAD_T thread_id)
{/*{{{*/
	TSRM_ERROR((TSRM_ERROR_LEVEL_CORE, "Creating data structures for thread %x", thread_id));
	(*thread_resources_ptr) = (tsrm_tls_entry *) malloc(TSRM_ALIGNED_SIZE(sizeof(tsrm_tls_entry)) + tsrm_reserved_size);
	(*thread_resources_ptr)->storage = NULL;
	if (id_count > 0) {
		(*thread_resources_ptr)->storage = (void **) malloc(sizeof(void *)*id_count);
	}
	(*thread_resources_ptr)->count = id_count;
	(*thread_resources_ptr)->thread_id = thread_id;
	(*thread_resources_ptr)->next = NULL;

	/* Set thread local storage to this new thread resources structure */
	set_thread_local_storage_resource_to(*thread_resources_ptr);

	if (tsrm_new_thread_begin_handler) {
		tsrm_new_thread_begin_handler(thread_id);
	}
	for (int i=0; i<id_count; i++) {
		if (resource_types_table[i].done) {
			(*thread_resources_ptr)->storage[i] = NULL;
		} else {
			if (resource_types_table[i].fast_offset) {
				(*thread_resources_ptr)->storage[i] = (void *) (((char*)(*thread_resources_ptr)) + resource_types_table[i].fast_offset);
			} else {
				(*thread_resources_ptr)->storage[i] = (void *) malloc(resource_types_table[i].size);
			}
			if (resource_types_table[i].ctor) {
				resource_types_table[i].ctor((*thread_resources_ptr)->storage[i]);
			}
		}
	}

	if (tsrm_new_thread_end_handler) {
		tsrm_new_thread_end_handler(thread_id);
	}
}/*}}}*/

/* fetches the requested resource for the current thread */
TSRM_API void *ts_resource_ex(ts_rsrc_id id, THREAD_T *th_id)
{/*{{{*/
	THREAD_T thread_id;
	int hash_value;
	tsrm_tls_entry *thread_resources, **last_thread_resources;

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
		tsrm_mutex_unlock(tsmm_mutex);
		return ts_resource_ex(id, &thread_id);
	} else {
		 last_thread_resources = &tsrm_tls_table[hash_value];
		 while (thread_resources->thread_id != thread_id) {
			last_thread_resources = &thread_resources->next;
			if (thread_resources->next) {
				thread_resources = thread_resources->next;
			} else {
				allocate_new_resource(&thread_resources->next, thread_id);
				tsrm_mutex_unlock(tsmm_mutex);
				return ts_resource_ex(id, &thread_id);
			}
		 }
	}

	/* It's possible that the current thread resources are requested, and that we get here.
	 * This means that the TSRM key pointer and cached pointer are NULL, but there is still
	 * a thread resource associated with this ID in the hashtable. This can occur if a thread
	 * goes away, but its resources are never cleaned up, and then that thread ID is reused.
	 * Since we don't always have a way to know when a thread goes away, we can't clean up
	 * the thread's resources before the new thread spawns.
	 * To solve this issue, we'll free up the old thread resources gracefully (gracefully
	 * because there might still be resources open like database connection which need to
	 * be shut down cleanly). After freeing up, we'll create the new resources for this thread
	 * as if the stale resources never existed in the first place. From that point forward,
	 * it is as if that situation never occurred.
	 * The fact that this situation happens isn't that bad because a child process containing
	 * threads will eventually be respawned anyway by the SAPI, so the stale threads won't last
	 * forever. */
	TSRM_ASSERT(thread_resources->thread_id == thread_id);
	if (thread_id == tsrm_thread_id() && !tsrm_tls_get()) {
		tsrm_tls_entry *next = thread_resources->next;
		/* In case that extensions don't use the pointer passed from the dtor, but incorrectly
		 * use the global pointer, we need to setup the global pointer temporarily here. */
		set_thread_local_storage_resource_to(thread_resources);
		/* Free up the old resource from the old thread instance */
		ts_free_resources(thread_resources);
		free(thread_resources);
		/* Allocate a new resource at the same point in the linked list, and relink the next pointer */
		allocate_new_resource(last_thread_resources, thread_id);
		thread_resources = *last_thread_resources;
		thread_resources->next = next;
		/* We don't have to tail-call ts_resource_ex, we can take the fast path to the return
		 * because we already have the correct pointer. */
	}

	tsrm_mutex_unlock(tsmm_mutex);

	/* Read a specific resource from the thread's resources.
	 * This is called outside of a mutex, so have to be aware about external
	 * changes to the structure as we read it.
	 */
	TSRM_SAFE_RETURN_RSRC(thread_resources->storage, id, thread_resources->count);
}/*}}}*/


/* frees all resources allocated for the current thread */
void ts_free_thread(void)
{/*{{{*/
	tsrm_tls_entry *thread_resources;
	THREAD_T thread_id = tsrm_thread_id();
	int hash_value;
	tsrm_tls_entry *last=NULL;

	TSRM_ASSERT(!in_main_thread);

	tsrm_mutex_lock(tsmm_mutex);
	hash_value = THREAD_HASH_OF(thread_id, tsrm_tls_table_size);
	thread_resources = tsrm_tls_table[hash_value];

	while (thread_resources) {
		if (thread_resources->thread_id == thread_id) {
			ts_free_resources(thread_resources);
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
}/*}}}*/

/* deallocates all occurrences of a given id */
void ts_free_id(ts_rsrc_id id)
{/*{{{*/
	int rsrc_id = TSRM_UNSHUFFLE_RSRC_ID(id);

	tsrm_mutex_lock(tsmm_mutex);

	TSRM_ERROR((TSRM_ERROR_LEVEL_CORE, "Freeing resource id %d", id));

	if (tsrm_tls_table) {
		for (int i=0; i<tsrm_tls_table_size; i++) {
			tsrm_tls_entry *p = tsrm_tls_table[i];

			while (p) {
				if (p->count > rsrc_id && p->storage[rsrc_id]) {
					if (resource_types_table) {
						if (resource_types_table[rsrc_id].dtor) {
							resource_types_table[rsrc_id].dtor(p->storage[rsrc_id]);
						}
						if (!resource_types_table[rsrc_id].fast_offset) {
							free(p->storage[rsrc_id]);
						}
					}
					p->storage[rsrc_id] = NULL;
				}
				p = p->next;
			}
		}
	}
	resource_types_table[rsrc_id].done = 1;

	tsrm_mutex_unlock(tsmm_mutex);

	TSRM_ERROR((TSRM_ERROR_LEVEL_CORE, "Successfully freed resource id %d", id));
}/*}}}*/


/*
 * Utility Functions
 */

/* Obtain the current thread id */
TSRM_API THREAD_T tsrm_thread_id(void)
{/*{{{*/
#ifdef TSRM_WIN32
	return GetCurrentThreadId();
#else
	return pthread_self();
#endif
}/*}}}*/


/* Allocate a mutex */
TSRM_API MUTEX_T tsrm_mutex_alloc(void)
{/*{{{*/
	MUTEX_T mutexp;
#ifdef TSRM_WIN32
	mutexp = malloc(sizeof(CRITICAL_SECTION));
	InitializeCriticalSection(mutexp);
#else
	mutexp = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(mutexp,NULL);
#endif
#ifdef THR_DEBUG
	printf("Mutex created thread: %d\n",mythreadid());
#endif
	return( mutexp );
}/*}}}*/


/* Free a mutex */
TSRM_API void tsrm_mutex_free(MUTEX_T mutexp)
{/*{{{*/
	if (mutexp) {
#ifdef TSRM_WIN32
		DeleteCriticalSection(mutexp);
		free(mutexp);
#else
		pthread_mutex_destroy(mutexp);
		free(mutexp);
#endif
	}
#ifdef THR_DEBUG
	printf("Mutex freed thread: %d\n",mythreadid());
#endif
}/*}}}*/


/*
  Lock a mutex.
  A return value of 0 indicates success
*/
TSRM_API int tsrm_mutex_lock(MUTEX_T mutexp)
{/*{{{*/
	TSRM_ERROR((TSRM_ERROR_LEVEL_INFO, "Mutex locked thread: %ld", tsrm_thread_id()));
#ifdef TSRM_WIN32
	EnterCriticalSection(mutexp);
	return 0;
#else
	return pthread_mutex_lock(mutexp);
#endif
}/*}}}*/


/*
  Unlock a mutex.
  A return value of 0 indicates success
*/
TSRM_API int tsrm_mutex_unlock(MUTEX_T mutexp)
{/*{{{*/
	TSRM_ERROR((TSRM_ERROR_LEVEL_INFO, "Mutex unlocked thread: %ld", tsrm_thread_id()));
#ifdef TSRM_WIN32
	LeaveCriticalSection(mutexp);
	return 0;
#else
	return pthread_mutex_unlock(mutexp);
#endif
}/*}}}*/

/*
  Changes the signal mask of the calling thread
*/
#ifdef HAVE_SIGPROCMASK
TSRM_API int tsrm_sigmask(int how, const sigset_t *set, sigset_t *oldset)
{/*{{{*/
	TSRM_ERROR((TSRM_ERROR_LEVEL_INFO, "Changed sigmask in thread: %ld", tsrm_thread_id()));

    return pthread_sigmask(how, set, oldset);
}/*}}}*/
#endif


TSRM_API void *tsrm_set_new_thread_begin_handler(tsrm_thread_begin_func_t new_thread_begin_handler)
{/*{{{*/
	void *retval = (void *) tsrm_new_thread_begin_handler;

	tsrm_new_thread_begin_handler = new_thread_begin_handler;
	return retval;
}/*}}}*/


TSRM_API void *tsrm_set_new_thread_end_handler(tsrm_thread_end_func_t new_thread_end_handler)
{/*{{{*/
	void *retval = (void *) tsrm_new_thread_end_handler;

	tsrm_new_thread_end_handler = new_thread_end_handler;
	return retval;
}/*}}}*/


TSRM_API void *tsrm_set_shutdown_handler(tsrm_shutdown_func_t shutdown_handler)
{/*{{{*/
	void *retval = (void *) tsrm_shutdown_handler;

	tsrm_shutdown_handler = shutdown_handler;
	return retval;
}/*}}}*/


/*
 * Debug support
 */

#ifdef TSRM_DEBUG
int tsrm_error(int level, const char *format, ...)
{/*{{{*/
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
}/*}}}*/
#endif


void tsrm_error_set(int level, const char *debug_filename)
{/*{{{*/
	tsrm_error_level = level;

#ifdef TSRM_DEBUG
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
}/*}}}*/

TSRM_API void *tsrm_get_ls_cache(void)
{/*{{{*/
	return tsrm_tls_get();
}/*}}}*/

/* Returns offset of tsrm_ls_cache slot from Thread Control Block address */
TSRM_API size_t tsrm_get_ls_cache_tcb_offset(void)
{/*{{{*/
#if defined(__APPLE__) && defined(__x86_64__)
    // TODO: Implement support for fast JIT ZTS code ???
	return 0;
#elif defined(__x86_64__) && defined(__GNUC__) && !defined(__FreeBSD__) && \
	!defined(__OpenBSD__) && !defined(__MUSL__) && !defined(__HAIKU__)
	size_t ret;

	asm ("movq _tsrm_ls_cache@gottpoff(%%rip),%0"
          : "=r" (ret));
	return ret;
#elif defined(__i386__) && defined(__GNUC__) && !defined(__FreeBSD__) && \
	!defined(__OpenBSD__) && !defined(__MUSL__) && !defined(__HAIKU__)
	size_t ret;

	asm ("leal _tsrm_ls_cache@ntpoff,%0"
          : "=r" (ret));
	return ret;
#elif defined(__aarch64__)
	size_t ret;

# ifdef __APPLE__
	// Points to struct TLVDecriptor for _tsrm_ls_cache in macOS.
	asm("adrp %0, #__tsrm_ls_cache@TLVPPAGE\n\t"
	    "ldr %0, [%0, #__tsrm_ls_cache@TLVPPAGEOFF]"
	     : "=r" (ret));
# else
	asm("mov %0, xzr\n\t"
	    "add %0, %0, #:tprel_hi12:_tsrm_ls_cache, lsl #12\n\t"
	    "add %0, %0, #:tprel_lo12_nc:_tsrm_ls_cache"
	     : "=r" (ret));
# endif
	return ret;
#else
	return 0;
#endif
}/*}}}*/

TSRM_API bool tsrm_is_main_thread(void)
{/*{{{*/
	return in_main_thread;
}/*}}}*/

TSRM_API bool tsrm_is_shutdown(void)
{/*{{{*/
	return is_thread_shutdown;
}/*}}}*/

TSRM_API const char *tsrm_api_name(void)
{/*{{{*/
#ifdef TSRM_WIN32
	return "Windows Threads";
#else
	return "POSIX Threads";
#endif
}/*}}}*/

#endif /* ZTS */
