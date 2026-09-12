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

#ifdef ZEND_WIN_TSRM_TEB_SLOT
/* Holds &_tsrm_ls_cache in a TEB TLS slot so EG()/CG() reach it
 * with a single gs:[] load rather than the 3-load __declspec(thread) lookup. */
#define ZEND_WIN_TEB_TLS_SLOTS 0x1480

static DWORD zend_win_tsrm_cache_slot = TLS_OUT_OF_INDEXES;
unsigned long zend_win_tsrm_cache_offset = 0;

ZEND_API zend_tsrm_ls_cache *zend_win_tsrm_cache_fallback(void)
{
	return &_tsrm_ls_cache;
}

ZEND_API void zend_win_tsrm_cache_shutdown(void)
{
	zend_win_tsrm_cache_offset = 0;
	if (zend_win_tsrm_cache_slot != TLS_OUT_OF_INDEXES) {
		TlsFree(zend_win_tsrm_cache_slot);
		zend_win_tsrm_cache_slot = TLS_OUT_OF_INDEXES;
	}
}

static void zend_win_tsrm_cache_publish(void)
{
	if (zend_win_tsrm_cache_slot == TLS_OUT_OF_INDEXES) {
		return;
	}
	TlsSetValue(zend_win_tsrm_cache_slot, &_tsrm_ls_cache);
	/* Verify our layout assumptions work */
	if ((zend_tsrm_ls_cache *) __readgsqword(zend_win_tsrm_cache_offset) != &_tsrm_ls_cache) {
		fprintf(stderr, "PHP Startup: the ZTS globals cache is not reachable through "
			"TEB offset %lu, falling back to __declspec(thread)\n",
			zend_win_tsrm_cache_offset);
		zend_win_tsrm_cache_shutdown();
	}
}

ZEND_API void zend_win_tsrm_cache_init(bool alloc)
{
	if (alloc) {
		DWORD slot = TlsAlloc();
		if (slot == TLS_OUT_OF_INDEXES) {
			return;
		}
		if (slot >= TLS_MINIMUM_AVAILABLE) {
			TlsFree(slot);
			return;
		}
		zend_win_tsrm_cache_slot = slot;
		zend_win_tsrm_cache_offset = ZEND_WIN_TEB_TLS_SLOTS
			+ slot * (unsigned long) sizeof(void*);
	}
	zend_win_tsrm_cache_publish();
}
#else
# define zend_win_tsrm_cache_publish() do {} while (0)
#endif

struct _tsrm_tls_entry {
	void **storage;
	int count;
	THREAD_T thread_id;
	tsrm_tls_entry *next;
#ifdef TSRM_WIN32
	void *thread_exit_data;
#endif
};

typedef struct {
	size_t size;
	ts_allocate_ctor ctor;
	ts_allocate_dtor dtor;
	ptrdiff_t fast_offset;
	/* When set, storage comes from __thread memory instead of being allocated by TSRM. */
	void *(*tls_addr)(void);
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
static tsrm_shutdown_func_t tsrm_thread_free_handler = NULL;

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

static bool tsrm_thread_exit_armed = false;

#ifdef TSRM_WIN32
static DWORD tls_key = TLS_OUT_OF_INDEXES;
static DWORD tsrm_exit_key = FLS_OUT_OF_INDEXES;
static BOOLEAN (NTAPI *tsrm_dll_shutdown_in_progress)(void);

typedef struct {
	tsrm_tls_entry *thread_resources;
} tsrm_thread_exit_data;

# define TSRM_THREAD_EXIT_CC	WINAPI
# define tsrm_tls_set(what)		TlsSetValue(tls_key, (void*)(what))
# define tsrm_tls_get()			TlsGetValue(tls_key)

static void tsrm_exit_key_set(tsrm_tls_entry *thread_resources)
{
	tsrm_thread_exit_data *data;

	if (!tsrm_thread_exit_armed) {
		return;
	}

	data = FlsGetValue(tsrm_exit_key);
	if (!thread_resources) {
		if (data && FlsSetValue(tsrm_exit_key, NULL)) {
			free(data);
		}
		return;
	}

	if (!data) {
		data = malloc(sizeof(tsrm_thread_exit_data));
		if (!data) {
			return;
		}
		data->thread_resources = thread_resources;
		if (!FlsSetValue(tsrm_exit_key, data)) {
			free(data);
			return;
		}
		thread_resources->thread_exit_data = data;
		return;
	}
	data->thread_resources = thread_resources;
	thread_resources->thread_exit_data = data;
}
#else
static pthread_key_t tls_key;
static pthread_key_t tsrm_exit_key;
# define TSRM_THREAD_EXIT_CC
# define tsrm_tls_set(what)		pthread_setspecific(tls_key, (void*)(what))
# define tsrm_tls_get()			pthread_getspecific(tls_key)
# define tsrm_exit_key_set(what)	do { if (tsrm_thread_exit_armed) { pthread_setspecific(tsrm_exit_key, (void*)(what)); } } while (0)
#endif

TSRM_TLS bool in_main_thread = false;
TSRM_TLS bool is_thread_shutdown = false;

static void TSRM_THREAD_EXIT_CC tsrm_thread_exit_handler(void *arg)
{
#ifdef TSRM_WIN32
	tsrm_thread_exit_data *data = arg;
	tsrm_tls_entry *thread_resources;

	if (tsrm_dll_shutdown_in_progress()) {
		return;
	}
	/* FLS callbacks also run when another fiber is deleted. TSRM storage is
	 * thread-wide, so only tear it down for the exiting execution context. */
	if (!tsrm_thread_exit_armed) {
		free(data);
		return;
	}
	if (FlsGetValue(tsrm_exit_key) != data) {
		free(data);
		return;
	}
	FlsSetValue(tsrm_exit_key, NULL);
	thread_resources = data->thread_resources;
	if (thread_resources != TSRMLS_CACHE || thread_resources->thread_exit_data != data
			|| in_main_thread || !tsrm_tls_table) {
		free(data);
		return;
	}
#else
	tsrm_tls_entry *thread_resources = (tsrm_tls_entry *) arg;

	if (!tsrm_thread_exit_armed || arg != TSRMLS_CACHE || in_main_thread || !tsrm_tls_table) {
		return;
	}
#endif

	tsrm_tls_set(thread_resources);
	ts_free_thread();
#ifdef TSRM_WIN32
	free(data);
#endif
}

TSRM_API void tsrm_thread_exit_disarm(void)
{
	if (!tsrm_thread_exit_armed) {
		return;
	}
	tsrm_thread_exit_armed = false;
#ifdef TSRM_WIN32
	DWORD key = tsrm_exit_key;
	tsrm_exit_key = FLS_OUT_OF_INDEXES;
	FlsFree(key);
#else
	pthread_key_delete(tsrm_exit_key);
#endif
}

#if !defined(TSRM_WIN32) && defined(__GNUC__)
static void __attribute__((destructor)) tsrm_thread_exit_unload(void)
{
	tsrm_thread_exit_disarm();
}
#endif

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

#ifdef TSRM_WIN32
	tsrm_dll_shutdown_in_progress = (BOOLEAN (NTAPI *)(void)) (void *) GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "RtlDllShutdownInProgress");
	tsrm_exit_key = tsrm_dll_shutdown_in_progress ? FlsAlloc(tsrm_thread_exit_handler) : FLS_OUT_OF_INDEXES;
	tsrm_thread_exit_armed = tsrm_exit_key != FLS_OUT_OF_INDEXES;
#else
	tsrm_thread_exit_armed = pthread_key_create(&tsrm_exit_key, tsrm_thread_exit_handler) == 0;
#endif

	return 1;
}/*}}}*/

static void ts_free_resources(
	tsrm_tls_entry *thread_resources, bool destroy_tls_resources, bool run_dtors)
{
	/* Need to destroy in reverse order to respect dependencies. */
	for (int i = thread_resources->count - 1; i >= 0; i--) {
		if (!resource_types_table[i].done) {
			/* Native TLS may only be accessed by its owning thread. */
			if (resource_types_table[i].tls_addr && !destroy_tls_resources) {
				continue;
			}
			if (run_dtors && resource_types_table[i].dtor) {
				resource_types_table[i].dtor(thread_resources->storage[i]);
			}

			if (!resource_types_table[i].fast_offset && !resource_types_table[i].tls_addr) {
				free(thread_resources->storage[i]);
			}
		}
	}

	free(thread_resources->storage);
}

/* Shutdown TSRM (call once for the entire process). Tears down every thread left
 * in the table. Native TLS dtors only run for the calling thread. */
TSRM_API void tsrm_shutdown(void)
{/*{{{*/
	tsrm_tls_entry *current_thread_resources;

	if (is_thread_shutdown) {
		/* shutdown must only occur once */
		return;
	}
	current_thread_resources = tsrm_tls_get();

	is_thread_shutdown = true;

	if (!in_main_thread) {
		/* only the main thread may shutdown tsrm */
		return;
	}

	tsrm_thread_exit_disarm();

	for (int i=0; i<tsrm_tls_table_size; i++) {
		tsrm_tls_entry *p = tsrm_tls_table[i], *next_p;

		while (p) {
			next_p = p->next;
			if (resource_types_table) {
				/* This call will already free p->storage for us */
				ts_free_resources(p, p == current_thread_resources, true);
			} else {
				free(p->storage);
			}
			free(p);
			p = next_p;
		}
	}
	free(tsrm_tls_table);
	tsrm_tls_table = NULL;
	free(resource_types_table);
	resource_types_table = NULL;
	tsrm_mutex_free(tsmm_mutex);
	tsrm_mutex_free(tsrm_env_mutex);
	TSRM_ERROR((TSRM_ERROR_LEVEL_CORE, "Shutdown TSRM"));
	if (tsrm_error_file!=stderr) {
		fclose(tsrm_error_file);
	}
#ifdef TSRM_WIN32
	TlsFree(tls_key);
	tls_key = TLS_OUT_OF_INDEXES;
#else
	pthread_setspecific(tls_key, 0);
	pthread_key_delete(tls_key);
#endif
	TSRMLS_CACHE = NULL;
	if (tsrm_shutdown_handler) {
		tsrm_shutdown_handler();
	}
	tsrm_new_thread_begin_handler = NULL;
	tsrm_new_thread_end_handler = NULL;
	tsrm_shutdown_handler = NULL;
	tsrm_thread_free_handler = NULL;

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
					if (resource_types_table[j].tls_addr) {
						TSRM_ASSERT(p->thread_id == tsrm_thread_id());
						p->storage[j] = resource_types_table[j].tls_addr();
					} else if (resource_types_table[j].fast_offset) {
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
	resource_types_table[TSRM_UNSHUFFLE_RSRC_ID(*rsrc_id)].tls_addr = NULL;
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
	resource_types_table[TSRM_UNSHUFFLE_RSRC_ID(*rsrc_id)].fast_offset = *offset;
	resource_types_table[TSRM_UNSHUFFLE_RSRC_ID(*rsrc_id)].tls_addr = NULL;
	resource_types_table[TSRM_UNSHUFFLE_RSRC_ID(*rsrc_id)].done = 0;

	tsrm_update_active_threads();
	tsrm_mutex_unlock(tsmm_mutex);

	TSRM_ERROR((TSRM_ERROR_LEVEL_CORE, "Successfully allocated new resource id %d", *rsrc_id));
	return *rsrc_id;
}/*}}}*/

/* allocates a resource id whose per-thread storage is a native __thread block */
TSRM_API ts_rsrc_id ts_allocate_tls_id(ts_rsrc_id *rsrc_id, void *(*tls_addr)(void), size_t size, ts_allocate_ctor ctor, ts_allocate_dtor dtor)
{
	TSRM_ERROR((TSRM_ERROR_LEVEL_CORE, "Obtaining a new TLS resource id, %d bytes", size));

	tsrm_mutex_lock(tsmm_mutex);

	*rsrc_id = TSRM_SHUFFLE_RSRC_ID(id_count++);

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
	resource_types_table[TSRM_UNSHUFFLE_RSRC_ID(*rsrc_id)].tls_addr = tls_addr;
	resource_types_table[TSRM_UNSHUFFLE_RSRC_ID(*rsrc_id)].done = 0;

	tsrm_update_active_threads();
	tsrm_mutex_unlock(tsmm_mutex);

	TSRM_ERROR((TSRM_ERROR_LEVEL_CORE, "Successfully allocated new TLS resource id %d", *rsrc_id));
	return *rsrc_id;
}

static void set_thread_local_storage_resource_to(tsrm_tls_entry *thread_resource)
{
	tsrm_tls_set(thread_resource);
	TSRMLS_CACHE = thread_resource;
	zend_win_tsrm_cache_publish();
}

/* Must be called with tsmm_mutex held */
static void allocate_new_resource(tsrm_tls_entry **thread_resources_ptr, THREAD_T thread_id)
{/*{{{*/
	TSRM_ERROR((TSRM_ERROR_LEVEL_CORE, "Creating data structures for thread %x", thread_id));
	/* Fast resources live in the reserved space right behind the entry. */
	(*thread_resources_ptr) = (tsrm_tls_entry *) malloc(TSRM_ALIGNED_SIZE(sizeof(tsrm_tls_entry)) + tsrm_reserved_size);
	(*thread_resources_ptr)->storage = NULL;
	if (id_count > 0) {
		(*thread_resources_ptr)->storage = (void **) malloc(sizeof(void *)*id_count);
	}
	(*thread_resources_ptr)->count = id_count;
	(*thread_resources_ptr)->thread_id = thread_id;
	(*thread_resources_ptr)->next = NULL;
#ifdef TSRM_WIN32
	(*thread_resources_ptr)->thread_exit_data = NULL;
#endif

	/* Set thread local storage to this new thread resources structure */
	set_thread_local_storage_resource_to(*thread_resources_ptr);

	if (tsrm_new_thread_begin_handler) {
		tsrm_new_thread_begin_handler(thread_id);
	}
	for (int i=0; i<id_count; i++) {
		if (resource_types_table[i].done) {
			(*thread_resources_ptr)->storage[i] = NULL;
		} else {
			if (resource_types_table[i].tls_addr) {
				(*thread_resources_ptr)->storage[i] = resource_types_table[i].tls_addr();
			} else if (resource_types_table[i].fast_offset) {
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

	/* A thread exiting from a resource constructor cannot safely clean up an
	 * entry that is still being built while tsmm_mutex is held. */
	tsrm_exit_key_set(*thread_resources_ptr);
	is_thread_shutdown = false;
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
	 * The old native TLS is no longer accessible, so its destructors cannot run safely.
	 * Release the directly owned allocations and create fresh resources for the new thread.
	 * The fact that this situation happens isn't that bad because a child process containing
	 * threads will eventually be respawned anyway by the SAPI, so the stale threads won't last
	 * forever. */
	TSRM_ASSERT(thread_resources->thread_id == thread_id);
	if (thread_id == tsrm_thread_id() && !tsrm_tls_get()) {
		tsrm_tls_entry *next = thread_resources->next;
		/* Keep signal handlers away from both the stale entry and the replacement
		 * until all of the replacement's resources have been constructed. */
		is_thread_shutdown = true;
		/* The dead thread's native TLS is gone, and its remaining resource dtors
		 * may depend on those globals. Only release directly owned allocations. */
		ts_free_resources(thread_resources, false, false);
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
	tsrm_tls_entry *thread_resources = tsrm_tls_get();
	tsrm_tls_entry *p;
	int hash_value;
	tsrm_tls_entry *last=NULL;

	TSRM_ASSERT(!in_main_thread);
	if (!thread_resources) {
		return;
	}

	is_thread_shutdown = true;
	/* Release resources that depend on TSRM before taking its lock. */
	if (tsrm_thread_free_handler) {
		tsrm_thread_free_handler();
	}

	tsrm_mutex_lock(tsmm_mutex);
	hash_value = THREAD_HASH_OF(thread_resources->thread_id, tsrm_tls_table_size);
	p = tsrm_tls_table[hash_value];

	while (p) {
		if (p == thread_resources) {
			ts_free_resources(thread_resources, true, true);
			if (last) {
				last->next = p->next;
			} else {
				tsrm_tls_table[hash_value] = p->next;
			}
			tsrm_tls_set(0);
			tsrm_exit_key_set(0);
			TSRMLS_CACHE = NULL;
			free(thread_resources);
			tsrm_mutex_unlock(tsmm_mutex);
			return;
		}
		last = p;
		p = p->next;
	}
	tsrm_tls_set(0);
	tsrm_exit_key_set(0);
	TSRMLS_CACHE = NULL;
	tsrm_mutex_unlock(tsmm_mutex);
}/*}}}*/

/* deallocates all occurrences of a given id */
void ts_free_id(ts_rsrc_id id)
{/*{{{*/
	int rsrc_id = TSRM_UNSHUFFLE_RSRC_ID(id);
	tsrm_tls_entry *current_thread_resources = tsrm_tls_get();

	tsrm_mutex_lock(tsmm_mutex);

	TSRM_ERROR((TSRM_ERROR_LEVEL_CORE, "Freeing resource id %d", id));

	if (tsrm_tls_table) {
		for (int i=0; i<tsrm_tls_table_size; i++) {
			tsrm_tls_entry *p = tsrm_tls_table[i];

			while (p) {
				if (p->count > rsrc_id && p->storage[rsrc_id]) {
					/* Native TLS may only be accessed by its owning thread. */
					if (resource_types_table
					 && (!resource_types_table[rsrc_id].tls_addr || p == current_thread_resources)) {
						if (resource_types_table[rsrc_id].dtor) {
							resource_types_table[rsrc_id].dtor(p->storage[rsrc_id]);
						}
						if (!resource_types_table[rsrc_id].fast_offset && !resource_types_table[rsrc_id].tls_addr) {
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

TSRM_API void ts_apply_for_id(ts_rsrc_id id, void (*cb)(void *))
{
	int rsrc_id = TSRM_UNSHUFFLE_RSRC_ID(id);
	tsrm_tls_entry *current_thread_resources = tsrm_tls_get();

	tsrm_mutex_lock(tsmm_mutex);

	if (tsrm_tls_table && resource_types_table) {
		bool tls_backed = resource_types_table[rsrc_id].tls_addr != NULL;

		for (int i = 0; i < tsrm_tls_table_size; i++) {
			tsrm_tls_entry *p = tsrm_tls_table[i];

			while (p) {
				/* Native TLS may only be accessed by its owning thread. */
				if (p->count > rsrc_id && p->storage[rsrc_id]
				 && (!tls_backed || p == current_thread_resources)) {
					cb(p->storage[rsrc_id]);
				}
				p = p->next;
			}
		}
	}

	tsrm_mutex_unlock(tsmm_mutex);
}

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


void tsrm_set_thread_free_handler(tsrm_shutdown_func_t thread_free_handler)
{
	tsrm_thread_free_handler = thread_free_handler;
}


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
#if defined(TSRM_TLS_MODEL_GLOBAL_DYNAMIC)
	/* No constant TCB offset under global-dynamic, can't use fast path */
	return 0;
#elif defined(__APPLE__) && defined(__x86_64__)
    // TODO: Implement support for fast JIT ZTS code ???
	return 0;
#elif defined(__x86_64__) && defined(__GNUC__) && !defined(__FreeBSD__) && \
	!defined(__NetBSD__) && !defined(__OpenBSD__) && !defined(__MUSL__) && \
	!defined(__HAIKU__) && !defined(__CYGWIN__)
	size_t ret;

	asm ("movq _tsrm_ls_cache@gottpoff(%%rip),%0"
          : "=r" (ret));
	return ret;
#elif defined(__i386__) && defined(__GNUC__) && !defined(__FreeBSD__) && \
	!defined(__NetBSD__) && !defined(__OpenBSD__) && !defined(__MUSL__) && \
	!defined(__HAIKU__) && !defined(__CYGWIN__)
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
# elif defined(TSRM_TLS_MODEL_DEFAULT)
	/* Surplus Static TLS space isn't guaranteed. */
	ret = 0;
# elif defined(TSRM_TLS_MODEL_INITIAL_EXEC)
	asm("adrp %0, :gottprel:_tsrm_ls_cache\n\t"
		"ldr %0, [%0, #:gottprel_lo12:_tsrm_ls_cache]"
		: "=r" (ret));
# elif defined(TSRM_TLS_MODEL_LOCAL_EXEC)
	asm("mov %0, xzr\n\t"
	    "add %0, %0, #:tprel_hi12:_tsrm_ls_cache, lsl #12\n\t"
	    "add %0, %0, #:tprel_lo12_nc:_tsrm_ls_cache"
	     : "=r" (ret));
# else
#  error "TSRM TLS model not set"
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

TSRM_API bool tsrm_is_managed_thread(void)
{/*{{{*/
	return tsrm_tls_get() ? true : false;
}/*}}}*/

#endif /* ZTS */
