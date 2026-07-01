/*
   +----------------------------------------------------------------------+
   | Zend OPcache                                                         |
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Author: Go Kudo <zeriyoshi@php.net>                                  |
   +----------------------------------------------------------------------+
*/

#include "zend_user_cache_internal.h"

#ifdef ZEND_WIN32
# include "zend_execute.h"
# include "zend_system_id.h"
# include "win32/ioutil.h"
# include <fcntl.h>
# include <io.h>
# include <stdio.h>
# include <winbase.h>
#else
# include <errno.h>
# include <fcntl.h>
# include <stdio.h>
# include <sys/stat.h>
# include <pthread.h>
# ifdef HAVE_UNISTD_H
#  include <unistd.h>
# endif
# if defined(USE_MMAP) || (defined(__linux__) && defined(HAVE_MEMFD_CREATE))
#  include <sys/mman.h>
# endif
#endif

#if defined(USE_MMAP) && !defined(ZEND_WIN32)
# if defined(MAP_ANON) && !defined(MAP_ANONYMOUS)
#  define MAP_ANONYMOUS MAP_ANON
# endif
#endif

#ifdef ZEND_WIN32
# define ZEND_OPCACHE_USER_CACHE_WIN32_MAPPING_PREFIX_SIZE (2 * sizeof(void *))
# define ZEND_OPCACHE_USER_CACHE_WIN32_MAPPING_NAME "ZendOPcache.UserCache.SharedMemoryArea"
# define ZEND_OPCACHE_USER_CACHE_WIN32_MAPPING_MUTEX_NAME "ZendOPcache.UserCache.SharedMemoryMutex"
# define ZEND_OPCACHE_USER_CACHE_WIN32_LOCK_FILE_NAME "ZendOPcache.UserCache.LockFile"
#endif

#ifdef ZEND_OPCACHE_USER_CACHE_HAVE_OPTIMISTIC
# define ZEND_OPCACHE_USER_CACHE_READER_CLAIM_MAX 4U
#endif

#ifdef ZEND_WIN32
typedef struct _zend_opcache_user_cache_win32_segment {
	zend_shared_segment segment;
	HANDLE memfile;
	void *mapping_base;
	size_t mapping_size;
} zend_opcache_user_cache_win32_segment;
#endif

#ifdef ZEND_OPCACHE_USER_CACHE_HAVE_OPTIMISTIC
typedef struct {
	zend_opcache_user_cache_header *header;
	uint32_t slot_index;
} zend_opcache_user_cache_reader_claim;
#endif

typedef enum _zend_opcache_user_cache_entry_lock_release_mode {
	ZEND_OPCACHE_USER_CACHE_ENTRY_LOCK_RELEASE_DROP,
	ZEND_OPCACHE_USER_CACHE_ENTRY_LOCK_RELEASE_CLEAR,
	ZEND_OPCACHE_USER_CACHE_ENTRY_LOCK_RELEASE_PRESERVE_LEASES
} zend_opcache_user_cache_entry_lock_release_mode;

#if defined(USE_MMAP) && !defined(ZEND_WIN32)
static int zend_opcache_user_cache_mmap_create_segments(
	size_t requested_size,
	zend_shared_segment ***shared_segments_p,
	int *shared_segments_count,
	const char **error_in
);
static int zend_opcache_user_cache_mmap_detach_segment(zend_shared_segment *shared_segment);
static size_t zend_opcache_user_cache_mmap_segment_type_size(void);
#endif

#ifdef ZEND_WIN32
static int zend_opcache_user_cache_win32_create_segments(
	size_t requested_size,
	zend_shared_segment ***shared_segments_p,
	int *shared_segments_count,
	const char **error_in
);
static int zend_opcache_user_cache_win32_detach_segment(zend_shared_segment *shared_segment);
static size_t zend_opcache_user_cache_win32_segment_type_size(void);
#endif

#if defined(USE_MMAP) && !defined(ZEND_WIN32)
static const zend_shared_memory_handlers zend_opcache_user_cache_mmap_handlers = {
	zend_opcache_user_cache_mmap_create_segments,
	zend_opcache_user_cache_mmap_detach_segment,
	zend_opcache_user_cache_mmap_segment_type_size
};
#endif

#ifdef ZEND_WIN32
static const zend_shared_memory_handlers zend_opcache_user_cache_win32_handlers = {
	zend_opcache_user_cache_win32_create_segments,
	zend_opcache_user_cache_win32_detach_segment,
	zend_opcache_user_cache_win32_segment_type_size
};
#endif

static const zend_shared_memory_handler_entry zend_opcache_user_cache_handler_table[] = {
#if defined(USE_MMAP) && !defined(ZEND_WIN32)
	{ "mmap", &zend_opcache_user_cache_mmap_handlers },
#endif
#ifdef USE_SHM
	{ "shm", &zend_alloc_shm_handlers },
#endif
#ifdef USE_SHM_OPEN
	{ "posix", &zend_alloc_posix_handlers },
#endif
#ifdef ZEND_WIN32
	{ "win32", &zend_opcache_user_cache_win32_handlers },
#endif
	{ NULL, NULL }
};

#ifndef ZEND_WIN32
static ZEND_EXT_TLS zend_ulong zend_opcache_user_cache_entry_lock_owner_pid = 0;
#endif

#ifdef ZEND_OPCACHE_USER_CACHE_HAVE_OPTIMISTIC
/* Current write-lock section has announced itself in write_seq. */
static ZEND_EXT_TLS bool zend_opcache_user_cache_write_seq_bumped = false;
/* 0 = drain not attempted in this write section, 1 = drained, -1 = timed out
 * (graph payloads must not be freed, moved, or overwritten in place). */
static ZEND_EXT_TLS int8_t zend_opcache_user_cache_reader_drain_state = 0;

static ZEND_EXT_TLS zend_opcache_user_cache_reader_claim zend_opcache_user_cache_reader_claims[ZEND_OPCACHE_USER_CACHE_READER_CLAIM_MAX];
static ZEND_EXT_TLS uint32_t zend_opcache_user_cache_reader_claim_count = 0;
static uint64_t zend_opcache_user_cache_reader_incarnation = 0;
#endif

static zend_always_inline bool zend_opcache_user_cache_force_startup_failure(void)
{
	const char *value = getenv("OPCACHE_USER_CACHE_FORCE_STARTUP_FAILURE");

	return value != NULL && value[0] != '\0' && value[0] != '0';
}

static zend_always_inline bool zend_opcache_user_cache_requires_pre_request_storage(void)
{
	if (sapi_module.name == NULL) {
		return false;
	}

	return strcmp(sapi_module.name, "fpm-fcgi") == 0 ||
		strcmp(sapi_module.name, "apache2handler") == 0 ||
		strcmp(sapi_module.name, "cli-server") == 0;
}

static zend_always_inline bool zend_opcache_user_cache_environment_is_allowed(void)
{
	return zend_opcache_user_cache_runtime_opted_in;
}

static zend_always_inline bool zend_opcache_user_cache_opcache_is_disabled_for_sapi(void)
{
	if (!ZCG(enabled)) {
		return true;
	}

	return !ZCG(accel_directives).enable_cli &&
		sapi_module.name != NULL &&
		(strcmp(sapi_module.name, "cli") == 0 || strcmp(sapi_module.name, "phpdbg") == 0);
}

static zend_always_inline void zend_opcache_user_cache_set_unavailable(const char *failure_reason, bool startup_failed)
{
	zend_opcache_user_cache_context *context = zend_opcache_user_cache_active_context();
	zend_opcache_user_cache_runtime *runtime = zend_opcache_user_cache_active_runtime();

	runtime->available = false;
	runtime->startup_failed = startup_failed;
	runtime->backend_initialized = context->storage.initialized;
	runtime->failure_reason = failure_reason;
}

static zend_always_inline void zend_opcache_user_cache_set_available(void)
{
	zend_opcache_user_cache_context *context = zend_opcache_user_cache_active_context();
	zend_opcache_user_cache_runtime *runtime = zend_opcache_user_cache_active_runtime();

	runtime->available = true;
	runtime->startup_failed = false;
	runtime->backend_initialized = context->storage.initialized;
	runtime->failure_reason = NULL;
}

static zend_always_inline HashTable **zend_opcache_user_cache_entry_lock_table_ptr(void)
{
	return &zend_opcache_user_cache_entry_lock_table;
}

static zend_always_inline uint32_t zend_opcache_user_cache_entry_lock_table_index(zend_ulong hash)
{
	return (uint32_t) (hash % ZEND_OPCACHE_USER_CACHE_ENTRY_LOCK_TABLE_SIZE);
}

#ifdef ZEND_OPCACHE_USER_CACHE_HAVE_OPTIMISTIC
static zend_always_inline uint64_t zend_opcache_user_cache_current_pid(void)
{
#ifdef ZEND_WIN32
	return (uint64_t) GetCurrentProcessId();
#else
	return (uint64_t) getpid();
#endif
}
#endif

static zend_always_inline uint32_t zend_opcache_user_cache_used_end_offset_locked(const zend_opcache_user_cache_header *header)
{
	return header->data_offset + header->next_free;
}

static zend_always_inline bool zend_opcache_user_cache_payload_size_to_block_size(size_t size, uint32_t *block_size)
{
	size_t aligned_size;

	if (size == 0 || size > UINT32_MAX - sizeof(zend_opcache_user_cache_block)) {
		return false;
	}

	aligned_size = ZEND_ALIGNED_SIZE(sizeof(zend_opcache_user_cache_block) + size);
	if (aligned_size > UINT32_MAX) {
		return false;
	}

	*block_size = (uint32_t) aligned_size;

	return true;
}

static zend_always_inline bool zend_opcache_user_cache_offset_in_block(uint32_t offset, uint32_t block_offset, uint32_t block_size)
{
	return offset >= block_offset + sizeof(zend_opcache_user_cache_block) && offset < block_offset + block_size;
}

static zend_always_inline bool zend_opcache_user_cache_entry_lock_record_key_matches(
		const zend_opcache_user_cache_entry_lock_record *record,
		zend_string *key,
		zend_ulong hash)
{
	return record->state == ZEND_OPCACHE_USER_CACHE_ENTRY_LOCK_USED &&
		record->hash == hash &&
		record->key_len == ZSTR_LEN(key) &&
		memcmp(zend_opcache_user_cache_ptr(record->key_offset), ZSTR_VAL(key), ZSTR_LEN(key)) == 0
	;
}

#ifdef ZEND_WIN32
static inline bool zend_opcache_user_cache_win32_set_segment(
	zend_opcache_user_cache_win32_segment *segment,
	HANDLE memfile,
	void *mapping_base,
	size_t mapping_size,
	size_t requested_size
)
{
	segment->memfile = memfile;
	segment->mapping_base = mapping_base;
	segment->mapping_size = mapping_size;
	segment->segment.p = (char *) mapping_base + ZEND_OPCACHE_USER_CACHE_WIN32_MAPPING_PREFIX_SIZE;
	segment->segment.pos = 0;
	segment->segment.size = requested_size;

	return true;
}
#endif

#if defined(USE_MMAP) && !defined(ZEND_WIN32)
static int zend_opcache_user_cache_mmap_create_segments(
	size_t requested_size,
	zend_shared_segment ***shared_segments_p,
	int *shared_segments_count,
	const char **error_in
)
{
	zend_shared_segment *segment;
	void *mapping;

	mapping = mmap(NULL, requested_size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (mapping == MAP_FAILED) {
		*error_in = "mmap";
		return ALLOC_FAILURE;
	}

	*shared_segments_count = 1;
	*shared_segments_p = (zend_shared_segment **) calloc(1, sizeof(zend_shared_segment *) + sizeof(zend_shared_segment));
	if (*shared_segments_p == NULL) {
		munmap(mapping, requested_size);
		*error_in = "calloc";
		return ALLOC_FAILURE;
	}

	segment = (zend_shared_segment *) ((char *) *shared_segments_p + sizeof(zend_shared_segment *));
	(*shared_segments_p)[0] = segment;

	segment->p = mapping;
	segment->pos = 0;
	segment->size = requested_size;
	segment->end = requested_size;

	return ALLOC_SUCCESS;
}

static int zend_opcache_user_cache_mmap_detach_segment(zend_shared_segment *shared_segment)
{
	munmap(shared_segment->p, shared_segment->size);

	return 0;
}

static size_t zend_opcache_user_cache_mmap_segment_type_size(void)
{
	return sizeof(zend_shared_segment);
}
#endif

#ifdef ZEND_WIN32
static void zend_opcache_user_cache_win32_create_name(char *buffer, size_t buffer_size, const char *name, size_t unique_id)
{
	zend_opcache_user_cache_context *context = zend_opcache_user_cache_active_context();
	const char *sapi_name = sapi_module.name != NULL ? sapi_module.name : "";

	snprintf(
		buffer,
		buffer_size,
		"%s@%.32s@%.20s@%.32s@%s@%zx",
		name,
		accel_uname_id,
		sapi_name,
		zend_system_id,
		context->lock_name,
		unique_id
	);
}

static int zend_opcache_user_cache_win32_reattach_segment(
	zend_opcache_user_cache_win32_segment *segment,
	HANDLE memfile,
	size_t requested_size,
	const char **error_in
)
{
	void *metadata_view, *wanted_mapping_base, *execute_ex_base, *mapping_base;
	MEMORY_BASIC_INFORMATION info;
	size_t mapping_size;

	if (requested_size > SIZE_MAX - ZEND_OPCACHE_USER_CACHE_WIN32_MAPPING_PREFIX_SIZE) {
		*error_in = "size overflow";
		return ALLOC_FAILURE;
	}

	mapping_size = requested_size + ZEND_OPCACHE_USER_CACHE_WIN32_MAPPING_PREFIX_SIZE;
	metadata_view = MapViewOfFileEx(memfile, FILE_MAP_READ, 0, 0, ZEND_OPCACHE_USER_CACHE_WIN32_MAPPING_PREFIX_SIZE, NULL);
	if (metadata_view == NULL) {
		*error_in = "MapViewOfFileEx";
		return ALLOC_FAILURE;
	}

	wanted_mapping_base = ((void **) metadata_view)[0];
	execute_ex_base = ((void **) metadata_view)[1];
	UnmapViewOfFile(metadata_view);

	if ((void *) execute_ex != execute_ex_base) {
		*error_in = "execute_ex";
		return ALLOC_FAILURE;
	}

	if (VirtualQuery(wanted_mapping_base, &info, sizeof(info)) == 0 ||
		info.State != MEM_FREE ||
		info.RegionSize < mapping_size
	) {
		*error_in = "VirtualQuery";
		return ALLOC_FAILURE;
	}

	mapping_base = MapViewOfFileEx(memfile, FILE_MAP_ALL_ACCESS, 0, 0, 0, wanted_mapping_base);
	if (mapping_base == NULL) {
		*error_in = "MapViewOfFileEx";
		return ALLOC_FAILURE;
	}

	return zend_opcache_user_cache_win32_set_segment(segment, memfile, mapping_base, mapping_size, requested_size)
		? ALLOC_SUCCESS
		: ALLOC_FAILURE;
}

static int zend_opcache_user_cache_win32_create_segment(
	zend_opcache_user_cache_win32_segment *segment,
	const char *mapping_name,
	size_t requested_size,
	const char **error_in
)
{
	HANDLE memfile;
	void *mapping_base = NULL;
	size_t mapping_size;
	DWORD size_high, size_low;
	uint32_t index;
	void *configured_mapping_base = (void *) -1;
#if defined(_WIN64)
	void *mapping_base_set[] = {
		(void *) 0x0000100000000000,
		(void *) 0x0000200000000000,
		(void *) 0x0000300000000000,
		(void *) 0x0000400000000000,
		(void *) 0x0000500000000000,
		(void *) 0x0000600000000000,
		(void *) 0x0000700000000000,
		NULL,
		(void *) -1
	};
#else
	void *mapping_base_set[] = {
		(void *) 0x20000000,
		(void *) 0x21000000,
		(void *) 0x30000000,
		(void *) 0x31000000,
		(void *) 0x50000000,
		NULL,
		(void *) -1
	};
#endif

	if (requested_size > SIZE_MAX - ZEND_OPCACHE_USER_CACHE_WIN32_MAPPING_PREFIX_SIZE) {
		*error_in = "size overflow";
		return ALLOC_FAILURE;
	}

	mapping_size = requested_size + ZEND_OPCACHE_USER_CACHE_WIN32_MAPPING_PREFIX_SIZE;
#if defined(_WIN64)
	size_high = (DWORD) (mapping_size >> 32);
	size_low = (DWORD) (mapping_size & 0xffffffff);
#else
	if (mapping_size > UINT32_MAX) {
		*error_in = "size overflow";
		return ALLOC_FAILURE;
	}
	size_high = 0;
	size_low = (DWORD) mapping_size;
#endif

	memfile = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE | SEC_COMMIT, size_high, size_low, mapping_name);
	if (memfile == NULL) {
		*error_in = "CreateFileMappingA";
		return ALLOC_FAILURE;
	}

	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		int result = zend_opcache_user_cache_win32_reattach_segment(segment, memfile, requested_size, error_in);
		if (result != ALLOC_SUCCESS) {
			CloseHandle(memfile);
		}

		return result;
	}

	if (ZCG(accel_directives).mmap_base && *ZCG(accel_directives).mmap_base) {
		char *mmap_base = ZCG(accel_directives).mmap_base;

		if (mmap_base[0] == '0' && mmap_base[1] == 'x') {
			mmap_base += 2;
		}
		if (sscanf(mmap_base, "%p", &configured_mapping_base) == 1) {
			mapping_base = MapViewOfFileEx(memfile, FILE_MAP_ALL_ACCESS, 0, 0, 0, configured_mapping_base);
		}
	}

	for (index = 0; mapping_base == NULL && mapping_base_set[index] != (void *) -1; index++) {
		mapping_base = MapViewOfFileEx(memfile, FILE_MAP_ALL_ACCESS, 0, 0, 0, mapping_base_set[index]);
	}

	if (mapping_base == NULL) {
		CloseHandle(memfile);
		*error_in = "MapViewOfFileEx";
		return ALLOC_FAILURE;
	}

	((void **) mapping_base)[0] = mapping_base;
	((void **) mapping_base)[1] = (void *) execute_ex;

	return zend_opcache_user_cache_win32_set_segment(segment, memfile, mapping_base, mapping_size, requested_size)
		? ALLOC_SUCCESS
		: ALLOC_FAILURE;
}

static int zend_opcache_user_cache_win32_create_segments(
	size_t requested_size,
	zend_shared_segment ***shared_segments_p,
	int *shared_segments_count,
	const char **error_in
)
{
	zend_opcache_user_cache_win32_segment *segment;
	HANDLE mutex = NULL, memfile = NULL;
	DWORD wait_result;
	char mapping_name[MAXPATHLEN], mutex_name[MAXPATHLEN];
	bool mutex_acquired = false;
	int result = ALLOC_FAILURE;

	*shared_segments_count = 1;
	*shared_segments_p = (zend_shared_segment **) calloc(
		1,
		sizeof(zend_shared_segment *) + sizeof(zend_opcache_user_cache_win32_segment)
	);
	if (*shared_segments_p == NULL) {
		*error_in = "calloc";
		return ALLOC_FAILURE;
	}

	segment = (zend_opcache_user_cache_win32_segment *) ((char *) *shared_segments_p + sizeof(zend_shared_segment *));
	(*shared_segments_p)[0] = (zend_shared_segment *) segment;

	zend_opcache_user_cache_win32_create_name(
		mapping_name,
		sizeof(mapping_name),
		ZEND_OPCACHE_USER_CACHE_WIN32_MAPPING_NAME,
		requested_size
	);
	zend_opcache_user_cache_win32_create_name(
		mutex_name,
		sizeof(mutex_name),
		ZEND_OPCACHE_USER_CACHE_WIN32_MAPPING_MUTEX_NAME,
		requested_size
	);

	mutex = CreateMutexA(NULL, FALSE, mutex_name);
	if (mutex == NULL) {
		*error_in = "CreateMutexA";
		goto failure;
	}

	wait_result = WaitForSingleObject(mutex, INFINITE);
	if (wait_result != WAIT_OBJECT_0 && wait_result != WAIT_ABANDONED) {
		*error_in = "WaitForSingleObject";
		goto failure;
	}
	mutex_acquired = true;

	memfile = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, mapping_name);
	if (memfile != NULL) {
		result = zend_opcache_user_cache_win32_reattach_segment(segment, memfile, requested_size, error_in);
		if (result != ALLOC_SUCCESS) {
			CloseHandle(memfile);
		}
	} else {
		result = zend_opcache_user_cache_win32_create_segment(segment, mapping_name, requested_size, error_in);
	}

	if (mutex_acquired) {
		ReleaseMutex(mutex);
		mutex_acquired = false;
	}
	CloseHandle(mutex);
	mutex = NULL;

	if (result == ALLOC_SUCCESS) {
		return ALLOC_SUCCESS;
	}

failure:
	if (mutex_acquired) {
		ReleaseMutex(mutex);
	}
	if (mutex != NULL) {
		CloseHandle(mutex);
	}
	free(*shared_segments_p);
	*shared_segments_p = NULL;
	*shared_segments_count = 0;

	return ALLOC_FAILURE;
}

static int zend_opcache_user_cache_win32_detach_segment(zend_shared_segment *shared_segment)
{
	zend_opcache_user_cache_win32_segment *segment = (zend_opcache_user_cache_win32_segment *) shared_segment;

	if (segment->mapping_base != NULL) {
		UnmapViewOfFile(segment->mapping_base);
		segment->mapping_base = NULL;
	}

	if (segment->memfile != NULL) {
		CloseHandle(segment->memfile);
		segment->memfile = NULL;
	}

	return 0;
}

static size_t zend_opcache_user_cache_win32_segment_type_size(void)
{
	return sizeof(zend_opcache_user_cache_win32_segment);
}
#endif

static void zend_opcache_user_cache_cleanup_segments(const zend_shared_memory_handlers *handler, zend_shared_segment **segments, int segment_count)
{
	int index;

	if (!handler || !segments) {
		return;
	}

	for (index = 0; index < segment_count; index++) {
		if (segments[index]->p && segments[index]->p != (void *) -1) {
			handler->detach_segment(segments[index]);
		}
	}

	free(segments);
}

#ifdef ZEND_OPCACHE_USER_CACHE_HAVE_SHARED_MUTEX
static bool zend_opcache_user_cache_shared_mutexes_init(zend_opcache_user_cache_header *header)
{
	pthread_mutexattr_t attr;
	bool ok;

	if (pthread_mutexattr_init(&attr) != 0) {
		return false;
	}

	ok = pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED) == 0 &&
		pthread_mutexattr_setrobust(&attr, PTHREAD_MUTEX_ROBUST) == 0 &&
		pthread_mutex_init(&header->global_shared_mutex.mutex, &attr) == 0
	;

	pthread_mutexattr_destroy(&attr);

	return ok;
}

/* Discard mutable contents if a mutex owner died during a write section. */
static void zend_opcache_user_cache_recover_after_owner_death(void)
{
	zend_opcache_user_cache_header *header = zend_opcache_user_cache_header_ptr();

	if (header == NULL || !zend_opcache_user_cache_header_is_initialized_locked()) {
		return;
	}

	if ((header->write_seq & 1) == 0) {
		return;
	}

	memset(
		zend_opcache_user_cache_entries(header),
		0,
		(size_t) header->capacity * sizeof(zend_opcache_user_cache_entry)
	);
	memset(header->entry_lock_records, 0, sizeof(header->entry_lock_records));
	memset(header->reader_slots, 0, sizeof(header->reader_slots));
	memset(header->orphaned_graphs, 0, sizeof(header->orphaned_graphs));

	header->active_optimistic_readers = 0;
	header->count = 0;
	header->tombstone_count = 0;
	header->next_free = 0;
	header->free_list = 0;
	header->last_block_offset = 0;

	zend_opcache_user_cache_bump_mutation_epoch_locked(header);

#ifdef ZEND_OPCACHE_USER_CACHE_HAVE_OPTIMISTIC
	zend_opcache_user_cache_seq_publish(&header->write_seq, header->write_seq + 1);
#else
	header->write_seq++;
#endif
}

static bool zend_opcache_user_cache_shared_mutex_lock(void)
{
	zend_opcache_user_cache_header *header = zend_opcache_user_cache_header_ptr();
	int result;

	if (header == NULL) {
		return false;
	}

	result = pthread_mutex_lock(&header->global_shared_mutex.mutex);
	if (result == EOWNERDEAD) {
		pthread_mutex_consistent(&header->global_shared_mutex.mutex);
		zend_opcache_user_cache_recover_after_owner_death();
		result = 0;
	}

	if (result != 0) {
		return false;
	}

	zend_opcache_user_cache_lock_held = true;

	return true;
}
#endif

#ifndef ZEND_WIN32
static bool zend_opcache_user_cache_lock_internal(short lock_type)
{
	zend_opcache_user_cache_storage *storage = &zend_opcache_user_cache_active_context()->storage;
	struct flock mem_lock;

	if (!storage->lock_initialized) {
		return false;
	}

#ifdef ZEND_OPCACHE_USER_CACHE_HAVE_SHARED_MUTEX
	/* Use the lock backend selected by the segment header. */
	if (!storage->use_shared_mutex &&
		zend_opcache_user_cache_header_ptr() != NULL &&
		zend_opcache_user_cache_header_is_initialized_locked() &&
		zend_opcache_user_cache_header_ptr()->lock_model == ZEND_OPCACHE_USER_CACHE_LOCK_MODEL_MUTEX
	) {
		storage->use_shared_mutex = true;
	}

	if (storage->use_shared_mutex) {
		return zend_opcache_user_cache_shared_mutex_lock();
	}
#endif

#ifdef ZTS
	if (tsrm_mutex_lock(storage->zts_lock) != 0) {
		return false;
	}
#endif

	mem_lock.l_type = lock_type;
	mem_lock.l_whence = SEEK_SET;
	mem_lock.l_start = 0;
	mem_lock.l_len = 1;

	while (fcntl(storage->lock_file, F_SETLKW, &mem_lock) == -1) {
		if (errno != EINTR) {
#ifdef ZTS
			tsrm_mutex_unlock(storage->zts_lock);
#endif
			return false;
		}
	}

	zend_opcache_user_cache_lock_held = true;

	return true;
}

static bool zend_opcache_user_cache_lock_startup(void)
{
	zend_opcache_user_cache_context *context = zend_opcache_user_cache_active_context();
	zend_opcache_user_cache_storage *storage = &context->storage;
	int val;

	if (storage->lock_initialized) {
		return true;
	}

#ifdef ZTS
	storage->zts_lock = tsrm_mutex_alloc();
	if (storage->zts_lock == NULL) {
		return false;
	}
#endif

#if defined(__linux__) && defined(HAVE_MEMFD_CREATE) && defined(MFD_CLOEXEC)
	storage->lock_file = memfd_create(context->lock_name, MFD_CLOEXEC);
	if (storage->lock_file >= 0) {
		storage->lock_initialized = true;
		return true;
	}
#endif

#ifdef O_TMPFILE
	storage->lock_file = open(ZCG(accel_directives).lockfile_path, O_RDWR | O_TMPFILE | O_EXCL | O_CLOEXEC, 0666);
	if (storage->lock_file >= 0) {
		storage->lock_initialized = true;
		return true;
	}
#endif

	snprintf(
		storage->lockfile_name,
		sizeof(storage->lockfile_name),
		"%s/%sXXXXXX",
		ZCG(accel_directives).lockfile_path,
		context->sem_filename_prefix
	);

	storage->lock_file = mkstemp(storage->lockfile_name);
	if (storage->lock_file == -1) {
#ifdef ZTS
		tsrm_mutex_free(storage->zts_lock);
		storage->zts_lock = NULL;
#endif

		return false;
	}

	if (fchmod(storage->lock_file, 0666) == -1) {
		close(storage->lock_file);
		storage->lock_file = -1;
#ifdef ZTS
		tsrm_mutex_free(storage->zts_lock);
		storage->zts_lock = NULL;
#endif
		return false;
	}

	val = fcntl(storage->lock_file, F_GETFD, 0);
	val |= FD_CLOEXEC;
	fcntl(storage->lock_file, F_SETFD, val);
	unlink(storage->lockfile_name);

	storage->lock_initialized = true;

	return true;
}

static void zend_opcache_user_cache_lock_shutdown(void)
{
	zend_opcache_user_cache_storage *storage = &zend_opcache_user_cache_active_context()->storage;

	if (!storage->lock_initialized) {
		return;
	}

	if (storage->lock_file >= 0) {
		close(storage->lock_file);
		storage->lock_file = -1;
	}
#ifdef ZTS
	tsrm_mutex_free(storage->zts_lock);
	storage->zts_lock = NULL;
#endif
	storage->lock_initialized = false;
}

static bool zend_opcache_user_cache_rlock_impl(void)
{
	return zend_opcache_user_cache_lock_internal(F_RDLCK);
}

static bool zend_opcache_user_cache_wlock_impl(void)
{
	return zend_opcache_user_cache_lock_internal(F_WRLCK);
}

static void zend_opcache_user_cache_unlock_impl(void)
{
	zend_opcache_user_cache_storage *storage = &zend_opcache_user_cache_active_context()->storage;
	struct flock mem_unlock;

	if (!storage->lock_initialized) {
		return;
	}

#ifdef ZEND_OPCACHE_USER_CACHE_HAVE_SHARED_MUTEX
	if (storage->use_shared_mutex) {
		zend_opcache_user_cache_header *header = zend_opcache_user_cache_header_ptr();

		if (header != NULL) {
			pthread_mutex_unlock(&header->global_shared_mutex.mutex);
		}

		zend_opcache_user_cache_lock_held = false;

		return;
	}
#endif

	mem_unlock.l_type = F_UNLCK;
	mem_unlock.l_whence = SEEK_SET;
	mem_unlock.l_start = 0;
	mem_unlock.l_len = 1;
	fcntl(storage->lock_file, F_SETLK, &mem_unlock);

#ifdef ZTS
	tsrm_mutex_unlock(storage->zts_lock);
#endif
	zend_opcache_user_cache_lock_held = false;
}
#else
static bool zend_opcache_user_cache_win32_open_lock_file_at(zend_opcache_user_cache_storage *storage, const char *directory, const char *base_name)
{
	size_t directory_len;
	const char *separator;

	if (directory == NULL || directory[0] == '\0') {
		return false;
	}

	directory_len = strlen(directory);
	separator = directory[directory_len - 1] == '/' || directory[directory_len - 1] == '\\'
		? ""
		: "/"
	;

	snprintf(
		storage->lockfile_name,
		sizeof(storage->lockfile_name),
		"%s%s%s.lock",
		directory,
		separator,
		base_name
	);

	storage->lock_file = php_win32_ioutil_open(storage->lockfile_name, O_RDWR | O_CREAT | O_BINARY, 0666);

	return storage->lock_file >= 0;
}

static bool zend_opcache_user_cache_win32_open_lock_file(zend_opcache_user_cache_context *context)
{
	zend_opcache_user_cache_storage *storage = &context->storage;
	char base_name[MAXPATHLEN], temp_path[MAXPATHLEN], *cursor;
	DWORD temp_path_len;

	zend_opcache_user_cache_win32_create_name(
		base_name,
		sizeof(base_name),
		ZEND_OPCACHE_USER_CACHE_WIN32_LOCK_FILE_NAME,
		storage->size
	);

	/* Partition names may contain SAPI-specific delimiters that are invalid in
	 * Win32 file names. The mapping and mutex names are kernel object names,
	 * but the lock backend stores this name on disk. */
	for (cursor = base_name; *cursor != '\0'; cursor++) {
		if ((unsigned char) *cursor < 32 ||
			*cursor == '<' ||
			*cursor == '>' ||
			*cursor == ':' ||
			*cursor == '"' ||
			*cursor == '/' ||
			*cursor == '\\' ||
			*cursor == '|' ||
			*cursor == '?' ||
			*cursor == '*'
		) {
			*cursor = '_';
		}
	}

	temp_path_len = GetTempPathA(sizeof(temp_path), temp_path);
	if (temp_path_len == 0 || temp_path_len >= sizeof(temp_path)) {
		return false;
	}

	return zend_opcache_user_cache_win32_open_lock_file_at(storage, temp_path, base_name);
}

static bool zend_opcache_user_cache_win32_lock_range(zend_opcache_user_cache_storage *storage, DWORD offset, bool exclusive, bool blocking)
{
	OVERLAPPED overlapped;
	HANDLE file_handle;
	DWORD flags = 0;

	if (!storage->lock_initialized || storage->lock_file < 0) {
		return false;
	}

	file_handle = (HANDLE) _get_osfhandle(storage->lock_file);
	if (file_handle == INVALID_HANDLE_VALUE) {
		return false;
	}

	memset(&overlapped, 0, sizeof(overlapped));
	overlapped.Offset = offset;

	if (exclusive) {
		flags |= LOCKFILE_EXCLUSIVE_LOCK;
	}
	if (!blocking) {
		flags |= LOCKFILE_FAIL_IMMEDIATELY;
	}

	return LockFileEx(file_handle, flags, 0, 1, 0, &overlapped) == TRUE;
}

static void zend_opcache_user_cache_win32_unlock_range(zend_opcache_user_cache_storage *storage, DWORD offset)
{
	OVERLAPPED overlapped;
	HANDLE file_handle;

	if (!storage->lock_initialized || storage->lock_file < 0) {
		return;
	}

	file_handle = (HANDLE) _get_osfhandle(storage->lock_file);
	if (file_handle == INVALID_HANDLE_VALUE) {
		return;
	}

	memset(&overlapped, 0, sizeof(overlapped));
	overlapped.Offset = offset;
	UnlockFileEx(file_handle, 0, 1, 0, &overlapped);
}

static bool zend_opcache_user_cache_lock_internal(bool exclusive)
{
	zend_opcache_user_cache_storage *storage = &zend_opcache_user_cache_active_context()->storage;

	if (!storage->lock_initialized) {
		return false;
	}

#ifdef ZTS
	if (tsrm_mutex_lock(storage->zts_lock) != 0) {
		return false;
	}
#endif

	if (!zend_opcache_user_cache_win32_lock_range(storage, 0, exclusive, true)) {
#ifdef ZTS
		tsrm_mutex_unlock(storage->zts_lock);
#endif
		return false;
	}

	zend_opcache_user_cache_lock_held = true;

	return true;
}

static bool zend_opcache_user_cache_lock_startup(void)
{
	zend_opcache_user_cache_context *context = zend_opcache_user_cache_active_context();
	zend_opcache_user_cache_storage *storage = &context->storage;

	if (storage->lock_initialized) {
		return true;
	}

#ifdef ZTS
	storage->zts_lock = tsrm_mutex_alloc();
	if (storage->zts_lock == NULL) {
		return false;
	}
#endif

	if (!zend_opcache_user_cache_win32_open_lock_file(context)) {
#ifdef ZTS
		tsrm_mutex_free(storage->zts_lock);
		storage->zts_lock = NULL;
#endif
		return false;
	}

	storage->lock_initialized = true;

	return true;
}

static void zend_opcache_user_cache_lock_shutdown(void)
{
	zend_opcache_user_cache_storage *storage = &zend_opcache_user_cache_active_context()->storage;

	if (!storage->lock_initialized) {
		return;
	}

	if (storage->lock_file >= 0) {
		php_win32_ioutil_close(storage->lock_file);
		storage->lock_file = -1;
	}
#ifdef ZTS
	tsrm_mutex_free(storage->zts_lock);
	storage->zts_lock = NULL;
#endif
	storage->lock_initialized = false;
}

static bool zend_opcache_user_cache_rlock_impl(void)
{
	return zend_opcache_user_cache_lock_internal(false);
}

static bool zend_opcache_user_cache_wlock_impl(void)
{
	return zend_opcache_user_cache_lock_internal(true);
}

static void zend_opcache_user_cache_unlock_impl(void)
{
	zend_opcache_user_cache_storage *storage = &zend_opcache_user_cache_active_context()->storage;

	if (!storage->lock_initialized) {
		return;
	}

	zend_opcache_user_cache_win32_unlock_range(storage, 0);

#ifdef ZTS
	tsrm_mutex_unlock(storage->zts_lock);
#endif
	zend_opcache_user_cache_lock_held = false;
}
#endif

static void zend_opcache_user_cache_entry_lock_lease_wait(void)
{
#ifdef ZEND_WIN32
	Sleep(10);
#elif defined(HAVE_UNISTD_H)
	usleep(10000);
#endif
}

static uint64_t zend_opcache_user_cache_entry_lock_expires_at(zend_long lease)
{
	uint64_t now, lease_seconds;

	ZEND_ASSERT(lease > 0);

	now = (uint64_t) time(NULL);
	lease_seconds = (uint64_t) lease;
	if (lease_seconds > UINT64_MAX - now) {
		return UINT64_MAX;
	}

	return now + lease_seconds;
}

static uint64_t zend_opcache_user_cache_entry_lock_current_pid(void)
{
#ifdef ZEND_WIN32
	return (uint64_t) GetCurrentProcessId();
#else
	return (uint64_t) getpid();
#endif
}

/* Best-effort process start time, used to tell a reader-slot owner apart from
 * an unrelated process that recycled its PID. 0 means unknown. */
static uint64_t zend_opcache_user_cache_process_start_time(uint64_t pid)
{
#ifdef ZEND_WIN32
	FILETIME creation, exit_time, kernel_time, user_time;
	HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, (DWORD) pid);
	uint64_t start_time = 0;

	if (process == NULL) {
		return 0;
	}

	if (GetProcessTimes(process, &creation, &exit_time, &kernel_time, &user_time)) {
		start_time = ((uint64_t) creation.dwHighDateTime << 32) | creation.dwLowDateTime;
	}

	CloseHandle(process);

	return start_time;
#elif defined(__linux__)
	char path[64], stat_buffer[1024];
	const char *cursor;
	unsigned long long start_time = 0;
	ssize_t stat_len;
	int fd, field;

	snprintf(path, sizeof(path), "/proc/%llu/stat", (unsigned long long) pid);
	fd = open(path, O_RDONLY);
	if (fd < 0) {
		return 0;
	}

	stat_len = read(fd, stat_buffer, sizeof(stat_buffer) - 1);
	close(fd);
	if (stat_len <= 0) {
		return 0;
	}

	stat_buffer[stat_len] = '\0';

	/* The comm field may contain spaces and parentheses; fields are counted
	 * from after its closing parenthesis. starttime is overall field 22, the
	 * 20th space-separated token after ") ". */
	cursor = strrchr(stat_buffer, ')');
	if (cursor == NULL) {
		return 0;
	}

	cursor++;
	for (field = 0; field < 19 && *cursor != '\0'; field++) {
		while (*cursor == ' ') {
			cursor++;
		}
		while (*cursor != '\0' && *cursor != ' ') {
			cursor++;
		}
	}

	if (sscanf(cursor, " %llu", &start_time) != 1) {
		return 0;
	}

	return (uint64_t) start_time;
#else
	(void) pid;

	return 0;
#endif
}

/* Unknown owner state is treated as alive. */
static bool zend_opcache_user_cache_process_owner_is_dead(uint64_t owner_pid, uint64_t owner_start_time)
{
	uint64_t start_time;

#ifdef ZEND_WIN32
	HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, (DWORD) owner_pid);
	DWORD exit_code = 0;

	if (process == NULL) {
		return GetLastError() == ERROR_INVALID_PARAMETER;
	}

	if (GetExitCodeProcess(process, &exit_code) && exit_code != STILL_ACTIVE) {
		CloseHandle(process);

		return true;
	}

	CloseHandle(process);
#else
	if (kill((pid_t) owner_pid, 0) == -1 && errno == ESRCH) {
		return true;
	}
#endif

	if (owner_start_time != 0) {
		start_time = zend_opcache_user_cache_process_start_time(owner_pid);
		if (start_time != 0 && start_time != owner_start_time) {
			return true;
		}
	}

	return false;
}

static void zend_opcache_user_cache_remove_entry_lock_record_locked(
		zend_opcache_user_cache_entry_lock_record *record)
{
	if (record->state == ZEND_OPCACHE_USER_CACHE_ENTRY_LOCK_USED && record->key_offset != 0) {
		zend_opcache_user_cache_free_locked(record->key_offset);
	}

	memset(record, 0, sizeof(*record));
	record->state = ZEND_OPCACHE_USER_CACHE_ENTRY_LOCK_TOMBSTONE;
}

static bool zend_opcache_user_cache_entry_lock_record_is_active_locked(
		zend_opcache_user_cache_entry_lock_record *record,
		uint64_t now)
{
	if (record->state != ZEND_OPCACHE_USER_CACHE_ENTRY_LOCK_USED) {
		return false;
	}

	if (record->expires_at != 0 && record->expires_at <= now) {
		return false;
	}

	if (record->owner_pid == 0) {
		return record->expires_at != 0;
	}

	if (zend_opcache_user_cache_process_owner_is_dead(record->owner_pid, record->owner_start_time)) {
		record->owner_pid = 0;
		record->owner_start_time = 0;

		return record->expires_at != 0;
	}

	return true;
}

static bool zend_opcache_user_cache_find_entry_lock_record_slot_locked(
		zend_opcache_user_cache_header *header,
		zend_string *key,
		zend_ulong hash,
		uint32_t *slot_index,
		bool *found)
{
	uint32_t first_available = UINT32_MAX, index, probe;
	uint64_t now = (uint64_t) time(NULL);
	zend_opcache_user_cache_entry_lock_record *record;

	*found = false;

	for (probe = 0; probe < ZEND_OPCACHE_USER_CACHE_ENTRY_LOCK_TABLE_SIZE; probe++) {
		index = (zend_opcache_user_cache_entry_lock_table_index(hash) + probe) % ZEND_OPCACHE_USER_CACHE_ENTRY_LOCK_TABLE_SIZE;
		record = &header->entry_lock_records[index];

		if (record->state == ZEND_OPCACHE_USER_CACHE_ENTRY_LOCK_EMPTY) {
			*slot_index = first_available != UINT32_MAX ? first_available : index;

			return true;
		}

		if (record->state == ZEND_OPCACHE_USER_CACHE_ENTRY_LOCK_TOMBSTONE) {
			if (first_available == UINT32_MAX) {
				first_available = index;
			}
			continue;
		}

		if (!zend_opcache_user_cache_entry_lock_record_is_active_locked(record, now)) {
			zend_opcache_user_cache_remove_entry_lock_record_locked(record);
			if (first_available == UINT32_MAX) {
				first_available = index;
			}
			continue;
		}

		if (zend_opcache_user_cache_entry_lock_record_key_matches(record, key, hash)) {
			*slot_index = index;
			*found = true;

			return true;
		}
	}

	if (first_available != UINT32_MAX) {
		*slot_index = first_available;

		return true;
	}

	return false;
}

static bool zend_opcache_user_cache_insert_entry_lock_record_locked(
		zend_opcache_user_cache_header *header,
		uint32_t slot_index,
		zend_string *key,
		zend_ulong hash,
		zend_long lease)
{
	zend_opcache_user_cache_entry_lock_record *record = &header->entry_lock_records[slot_index];
	uint32_t key_offset;
	uint64_t owner_pid;

	if (ZSTR_LEN(key) > UINT32_MAX) {
		return false;
	}

	key_offset = zend_opcache_user_cache_alloc_locked(ZSTR_LEN(key), ZSTR_VAL(key));
	if (key_offset == 0) {
		return false;
	}

	owner_pid = zend_opcache_user_cache_entry_lock_current_pid();

	memset(record, 0, sizeof(*record));
	record->hash = hash;
	record->owner_pid = owner_pid;
	record->owner_start_time = zend_opcache_user_cache_process_start_time(owner_pid);
	record->expires_at = lease > 0 ? zend_opcache_user_cache_entry_lock_expires_at(lease) : 0;
	record->key_offset = key_offset;
	record->key_len = (uint32_t) ZSTR_LEN(key);
	record->state = ZEND_OPCACHE_USER_CACHE_ENTRY_LOCK_USED;

	return true;
}

static bool zend_opcache_user_cache_update_entry_lock_record_lease_locked(
		zend_opcache_user_cache_header *header,
		zend_string *key,
		zend_long lease)
{
	zend_opcache_user_cache_entry_lock_record *record;
	zend_ulong hash;
	uint32_t slot_index;
	uint64_t expires_at;
	bool found;

	if (lease <= 0) {
		return true;
	}

	hash = zend_string_hash_val(key);
	if (!zend_opcache_user_cache_find_entry_lock_record_slot_locked(header, key, hash, &slot_index, &found) || !found) {
		return false;
	}

	record = &header->entry_lock_records[slot_index];
	expires_at = zend_opcache_user_cache_entry_lock_expires_at(lease);
	if (record->expires_at < expires_at) {
		record->expires_at = expires_at;
	}

	return true;
}

static zend_opcache_user_cache_entry_lock *zend_opcache_user_cache_create_local_entry_lock(
		zend_opcache_user_cache_context *context,
		zend_long lease,
		bool preserve_lease)
{
	zend_opcache_user_cache_entry_lock *lock;

	lock = emalloc(sizeof(zend_opcache_user_cache_entry_lock));
	lock->context = context;
	lock->owner_pid = zend_opcache_user_cache_entry_lock_current_pid();
	lock->lease = lease;
	lock->preserve_lease = preserve_lease;

	return lock;
}

static bool zend_opcache_user_cache_add_local_entry_lock(
		HashTable *locks,
		zend_string *key,
		zend_opcache_user_cache_entry_lock *lock)
{
	bool added = false;

	zend_try {
		added = zend_hash_add_ptr(locks, key, lock) != NULL;
	} zend_catch {
		efree(lock);
		zend_bailout();
	} zend_end_try();

	return added;
}

static void zend_opcache_user_cache_release_entry_lock_context(
		zend_opcache_user_cache_context *context,
		HashTable **locks_ptr,
		zend_opcache_user_cache_entry_lock_release_mode mode)
{
	zend_opcache_user_cache_context *previous_context;
	zend_opcache_user_cache_entry_lock *lock;
	zend_opcache_user_cache_header *header;
	zend_string *key, **released_keys;
	zend_ulong hash;
	uint32_t lock_count, released_key_count = 0, index, slot_index;
	bool found;

	if (*locks_ptr == NULL) {
		return;
	}

	lock_count = zend_hash_num_elements(*locks_ptr);
	if (lock_count == 0 || mode == ZEND_OPCACHE_USER_CACHE_ENTRY_LOCK_RELEASE_DROP) {
		zend_hash_destroy(*locks_ptr);
		FREE_HASHTABLE(*locks_ptr);
		*locks_ptr = NULL;

		return;
	}

	released_keys = safe_emalloc(lock_count, sizeof(zend_string *), 0);
	ZEND_HASH_FOREACH_STR_KEY_PTR(*locks_ptr, key, lock) {
		if (key == NULL || lock == NULL || lock->context != context) {
			continue;
		}

		released_keys[released_key_count++] = zend_string_copy(key);
	} ZEND_HASH_FOREACH_END();

	if (released_key_count == 0) {
		efree(released_keys);

		return;
	}

	previous_context = zend_opcache_user_cache_activate_context(context);
	if (zend_opcache_user_cache_wlock()) {
		header = zend_opcache_user_cache_header_ptr();
		if (zend_opcache_user_cache_header_init_locked()) {
			for (index = 0; index < released_key_count; index++) {
				key = released_keys[index];
				lock = zend_hash_find_ptr(*locks_ptr, key);
				if (lock == NULL) {
					continue;
				}

				hash = zend_string_hash_val(key);
				if (!zend_opcache_user_cache_find_entry_lock_record_slot_locked(header, key, hash, &slot_index, &found) || !found) {
					continue;
				}

				if (header->entry_lock_records[slot_index].owner_pid != lock->owner_pid) {
					continue;
				}

				if (mode == ZEND_OPCACHE_USER_CACHE_ENTRY_LOCK_RELEASE_PRESERVE_LEASES &&
						lock->preserve_lease &&
						lock->lease > 0) {
					header->entry_lock_records[slot_index].owner_pid = 0;
					header->entry_lock_records[slot_index].owner_start_time = 0;
					header->entry_lock_records[slot_index].expires_at =
						zend_opcache_user_cache_entry_lock_expires_at(lock->lease);
				} else {
					zend_opcache_user_cache_remove_entry_lock_record_locked(&header->entry_lock_records[slot_index]);
				}
			}
		}
		zend_opcache_user_cache_unlock();
	}
	zend_opcache_user_cache_restore_context(previous_context);

	for (index = 0; index < released_key_count; index++) {
		zend_hash_del(*locks_ptr, released_keys[index]);
		zend_string_release(released_keys[index]);
	}
	efree(released_keys);

	if (zend_hash_num_elements(*locks_ptr) == 0) {
		zend_hash_destroy(*locks_ptr);
		FREE_HASHTABLE(*locks_ptr);
		*locks_ptr = NULL;
	}
}

static void zend_opcache_user_cache_release_entry_lock_all_contexts(
		HashTable **locks_ptr,
		zend_opcache_user_cache_entry_lock_release_mode mode)
{
	zend_opcache_user_cache_context *context = NULL;
	zend_opcache_user_cache_entry_lock *lock;

	while (*locks_ptr != NULL) {
		context = NULL;

		ZEND_HASH_FOREACH_PTR(*locks_ptr, lock) {
			if (lock != NULL && lock->context != NULL) {
				context = lock->context;

				break;
			}
		} ZEND_HASH_FOREACH_END();

		if (context == NULL) {
			zend_opcache_user_cache_release_entry_lock_context(
				zend_opcache_user_cache_default_context(),
				locks_ptr,
				ZEND_OPCACHE_USER_CACHE_ENTRY_LOCK_RELEASE_DROP
			);

			return;
		}

		zend_opcache_user_cache_release_entry_lock_context(context, locks_ptr, mode);
	}
}

static void zend_opcache_user_cache_ensure_entry_lock_process(void)
{
#ifndef ZEND_WIN32
	zend_ulong current_pid = (zend_ulong) getpid();

	if (zend_opcache_user_cache_entry_lock_owner_pid == 0) {
		zend_opcache_user_cache_entry_lock_owner_pid = current_pid;

		return;
	}

	if (zend_opcache_user_cache_entry_lock_owner_pid == current_pid) {
		return;
	}

	/* Drop inherited lock tables without releasing parent-owned records. */
	zend_opcache_user_cache_release_entry_lock_context(
		zend_opcache_user_cache_default_context(),
		&zend_opcache_user_cache_entry_lock_table,
		ZEND_OPCACHE_USER_CACHE_ENTRY_LOCK_RELEASE_DROP
	);
	zend_opcache_user_cache_entry_lock_owner_pid = current_pid;
#endif
}

static void zend_opcache_user_cache_entry_lock_dtor(zval *lock_zv)
{
	zend_opcache_user_cache_entry_lock *lock = Z_PTR_P(lock_zv);

	if (lock != NULL) {
		efree(lock);
	}
}

static HashTable *zend_opcache_user_cache_entry_locks(void)
{
	HashTable **locks_ptr = zend_opcache_user_cache_entry_lock_table_ptr();

	if (*locks_ptr == NULL) {
		ALLOC_HASHTABLE(*locks_ptr);
		zend_hash_init(*locks_ptr, 0, NULL, zend_opcache_user_cache_entry_lock_dtor, 0);
	}

	return *locks_ptr;
}

static HashTable *zend_opcache_user_cache_prepare_entry_locks_for_insert(void)
{
	HashTable *locks = zend_opcache_user_cache_entry_locks();

	zend_hash_extend(locks, zend_hash_num_elements(locks) + 1, 0);

	return locks;
}

static void zend_opcache_user_cache_destroy_empty_entry_locks(HashTable **locks_ptr)
{
	if (*locks_ptr != NULL && zend_hash_num_elements(*locks_ptr) == 0) {
		zend_hash_destroy(*locks_ptr);
		FREE_HASHTABLE(*locks_ptr);
		*locks_ptr = NULL;
	}
}

static bool zend_opcache_user_cache_acquire_entry_lock_record(
		zend_opcache_user_cache_context *context,
		zend_string *key,
		zend_long lease,
		bool preserve_lease,
		bool blocking)
{
	zend_opcache_user_cache_context *previous_context;
	zend_opcache_user_cache_entry_lock *lock;
	HashTable **locks_ptr = zend_opcache_user_cache_entry_lock_table_ptr();
	HashTable *locks;
	zend_opcache_user_cache_header *header;
	zend_ulong hash = zend_string_hash_val(key);
	uint32_t slot_index;
	bool found, inserted, insert_failed;

	zend_opcache_user_cache_ensure_entry_lock_process();

	if (*locks_ptr != NULL && (lock = zend_hash_find_ptr(*locks_ptr, key)) != NULL) {
		if (lease > lock->lease) {
			previous_context = zend_opcache_user_cache_activate_context(context);
			if (!zend_opcache_user_cache_wlock()) {
				zend_opcache_user_cache_restore_context(previous_context);

				return false;
			}

			header = zend_opcache_user_cache_header_ptr();
			inserted = zend_opcache_user_cache_header_init_locked() &&
				zend_opcache_user_cache_update_entry_lock_record_lease_locked(header, key, lease)
			;
			zend_opcache_user_cache_unlock();
			zend_opcache_user_cache_restore_context(previous_context);

			if (!inserted) {
				return false;
			}

			lock->lease = lease;
		}
		lock->preserve_lease = lock->preserve_lease || preserve_lease;

		return true;
	}

	locks = zend_opcache_user_cache_prepare_entry_locks_for_insert();
	lock = zend_opcache_user_cache_create_local_entry_lock(context, lease, preserve_lease);

	for (;;) {
		inserted = false;
		found = false;
		insert_failed = false;

		previous_context = zend_opcache_user_cache_activate_context(context);
		if (!zend_opcache_user_cache_wlock()) {
			zend_opcache_user_cache_restore_context(previous_context);
			efree(lock);
			zend_opcache_user_cache_destroy_empty_entry_locks(locks_ptr);

			return false;
		}

		header = zend_opcache_user_cache_header_ptr();
		if (!zend_opcache_user_cache_header_init_locked()) {
			zend_opcache_user_cache_unlock();
			zend_opcache_user_cache_restore_context(previous_context);
			efree(lock);
			zend_opcache_user_cache_destroy_empty_entry_locks(locks_ptr);

			return false;
		}
		if (zend_opcache_user_cache_find_entry_lock_record_slot_locked(header, key, hash, &slot_index, &found)) {
			if (!found &&
				!(inserted = zend_opcache_user_cache_insert_entry_lock_record_locked(header, slot_index, key, hash, lease))
			) {
				insert_failed = true;
			}
		}
		zend_opcache_user_cache_unlock();
		zend_opcache_user_cache_restore_context(previous_context);

		if (inserted) {
			if (zend_opcache_user_cache_add_local_entry_lock(locks, key, lock)) {
				return true;
			}

			previous_context = zend_opcache_user_cache_activate_context(context);
			if (zend_opcache_user_cache_wlock()) {
				header = zend_opcache_user_cache_header_ptr();
				if (zend_opcache_user_cache_header_is_initialized_locked() &&
					zend_opcache_user_cache_find_entry_lock_record_slot_locked(header, key, hash, &slot_index, &found) &&
					found &&
					header->entry_lock_records[slot_index].owner_pid == lock->owner_pid
				) {
					zend_opcache_user_cache_remove_entry_lock_record_locked(&header->entry_lock_records[slot_index]);
				}
				zend_opcache_user_cache_unlock();
			}
			zend_opcache_user_cache_restore_context(previous_context);
			efree(lock);
			zend_opcache_user_cache_destroy_empty_entry_locks(locks_ptr);

			return false;
		}

		if (insert_failed) {
			efree(lock);
			zend_opcache_user_cache_destroy_empty_entry_locks(locks_ptr);

			return false;
		}

		if (!blocking) {
			efree(lock);
			zend_opcache_user_cache_destroy_empty_entry_locks(locks_ptr);

			return false;
		}

		zend_opcache_user_cache_entry_lock_lease_wait();
	}
}

static uint32_t zend_opcache_user_cache_calculate_capacity(size_t size)
{
	size_t capacity = size / ZEND_OPCACHE_USER_CACHE_SLOT_BYTES, data_offset;

	if (capacity < ZEND_OPCACHE_USER_CACHE_MIN_CAPACITY) {
		capacity = ZEND_OPCACHE_USER_CACHE_MIN_CAPACITY;
	} else if (capacity > ZEND_OPCACHE_USER_CACHE_MAX_CAPACITY) {
		capacity = ZEND_OPCACHE_USER_CACHE_MAX_CAPACITY;
	}

	if ((capacity & 1) == 0) {
		capacity--;
	}

	for (;;) {
		data_offset = ZEND_ALIGNED_SIZE(sizeof(zend_opcache_user_cache_header) + capacity * sizeof(zend_opcache_user_cache_entry));
		if (data_offset < size || capacity == ZEND_OPCACHE_USER_CACHE_MIN_CAPACITY) {
			break;
		}

		capacity >>= 1;
		if (capacity < ZEND_OPCACHE_USER_CACHE_MIN_CAPACITY) {
			capacity = ZEND_OPCACHE_USER_CACHE_MIN_CAPACITY;
		}

		if ((capacity & 1) == 0) {
			capacity--;
		}
	}

	return (uint32_t) capacity;
}

static void zend_opcache_user_cache_free_list_remove_locked(zend_opcache_user_cache_header *header, uint32_t block_offset)
{
	zend_opcache_user_cache_block *block = zend_opcache_user_cache_block_ptr(block_offset);

	if (block->prev_free != 0) {
		zend_opcache_user_cache_block_ptr(block->prev_free)->next_free = block->next_free;
	} else {
		header->free_list = block->next_free;
	}

	if (block->next_free != 0) {
		zend_opcache_user_cache_block_ptr(block->next_free)->prev_free = block->prev_free;
	}

	block->next_free = 0;
	block->prev_free = 0;
	block->flags &= ~ZEND_OPCACHE_USER_CACHE_BLOCK_FREE;
}

static void zend_opcache_user_cache_free_list_insert_locked(zend_opcache_user_cache_header *header, uint32_t block_offset)
{
	zend_opcache_user_cache_block *block = zend_opcache_user_cache_block_ptr(block_offset);

	block->prev_free = 0;
	block->next_free = header->free_list;

	if (header->free_list != 0) {
		zend_opcache_user_cache_block_ptr(header->free_list)->prev_free = block_offset;
	}

	zend_opcache_user_cache_block_mark_free(block);
	header->free_list = block_offset;
}

static void zend_opcache_user_cache_update_following_prev_size_locked(
	zend_opcache_user_cache_header *header,
	uint32_t block_offset,
	const zend_opcache_user_cache_block *block
)
{
	uint32_t next_offset = block_offset + block->size;

	if (next_offset < zend_opcache_user_cache_used_end_offset_locked(header)) {
		zend_opcache_user_cache_block_ptr(next_offset)->prev_size = block->size;
	}
}

static void zend_opcache_user_cache_trim_tail_free_blocks_locked(
	zend_opcache_user_cache_header *header,
	uint32_t block_offset
)
{
	zend_opcache_user_cache_block *block = zend_opcache_user_cache_block_ptr(block_offset);
	uint32_t prev_offset;

	while (block_offset >= header->data_offset &&
		header->last_block_offset == block_offset &&
		zend_opcache_user_cache_block_is_free(block) &&
		block_offset + block->size == zend_opcache_user_cache_used_end_offset_locked(header)
	) {
		prev_offset = 0;
		zend_opcache_user_cache_free_list_remove_locked(header, block_offset);
		header->next_free -= block->size;
		if (block->prev_size != 0 && block_offset > header->data_offset) {
			prev_offset = block_offset - block->prev_size;
		}

		header->last_block_offset = prev_offset;
		if (prev_offset == 0) {
			break;
		}

		block_offset = prev_offset;
		block = zend_opcache_user_cache_block_ptr(block_offset);
	}
}

static bool zend_opcache_user_cache_try_handler(const zend_shared_memory_handler_entry *handler_entry)
{
	const char *error_in = NULL;
	zend_opcache_user_cache_context *context = zend_opcache_user_cache_active_context();
	zend_opcache_user_cache_runtime *runtime = zend_opcache_user_cache_active_runtime();
	zend_opcache_user_cache_storage *storage = &context->storage;
	zend_shared_segment **segments = NULL;
	int segment_count = 0, result;

	result = handler_entry->handler->create_segments(
		runtime->configured_memory,
		&segments,
		&segment_count,
		&error_in
	);
	if (result != ALLOC_SUCCESS) {
		zend_opcache_user_cache_cleanup_segments(handler_entry->handler, segments, segment_count);
		return false;
	}

	storage->handler = handler_entry->handler;
	storage->segments = segments;
	storage->segment_count = segment_count;
	storage->size = runtime->configured_memory;
	storage->model = handler_entry->name;
	storage->initialized = true;

	return true;
}

static bool zend_opcache_user_cache_startup_storage(void)
{
	const zend_shared_memory_handler_entry *handler_entry;
	const char *requested_model = ZCG(accel_directives).memory_model;
	zend_opcache_user_cache_storage *storage = &zend_opcache_user_cache_active_context()->storage;

	if (storage->initialized) {
		return true;
	}

	if (requested_model && requested_model[0]) {
		if (strcmp(requested_model, "cgi") == 0) {
			requested_model = "shm";
		}

		for (handler_entry = zend_opcache_user_cache_handler_table; handler_entry->name; handler_entry++) {
			if (strcmp(requested_model, handler_entry->name) == 0 && zend_opcache_user_cache_try_handler(handler_entry)) {
				goto storage_ready;
			}
		}
	}

	for (handler_entry = zend_opcache_user_cache_handler_table; handler_entry->name; handler_entry++) {
		if (requested_model && requested_model[0] && strcmp(requested_model, handler_entry->name) == 0) {
			continue;
		}

		if (zend_opcache_user_cache_try_handler(handler_entry)) {
			goto storage_ready;
		}
	}

	return false;

storage_ready:
	if (storage->segment_count != 1) {
		zend_opcache_user_cache_cleanup_segments(
			storage->handler,
			storage->segments,
			storage->segment_count
		);
		zend_opcache_user_cache_reset_storage();

		return false;
	}

	if (!zend_opcache_user_cache_lock_startup()) {
		zend_opcache_user_cache_cleanup_segments(
			storage->handler,
			storage->segments,
			storage->segment_count
		);
		zend_opcache_user_cache_reset_storage();

		return false;
	}

	if (!zend_opcache_user_cache_wlock()) {
		zend_opcache_user_cache_lock_shutdown();
		zend_opcache_user_cache_cleanup_segments(
			storage->handler,
			storage->segments,
			storage->segment_count
		);
		zend_opcache_user_cache_reset_storage();

		return false;
	}

	if (!zend_opcache_user_cache_header_init_locked()) {
		zend_opcache_user_cache_unlock();
		zend_opcache_user_cache_lock_shutdown();
		zend_opcache_user_cache_cleanup_segments(
			storage->handler,
			storage->segments,
			storage->segment_count
		);
		zend_opcache_user_cache_reset_storage();

		return false;
	}

	zend_opcache_user_cache_unlock();

#ifdef ZEND_OPCACHE_USER_CACHE_HAVE_SHARED_MUTEX
	storage->use_shared_mutex =
		zend_opcache_user_cache_header_ptr() != NULL &&
		zend_opcache_user_cache_header_ptr()->lock_model == ZEND_OPCACHE_USER_CACHE_LOCK_MODEL_MUTEX
	;
#endif

	return true;
}

static bool zend_opcache_user_cache_block_is_movable_locked(
	zend_opcache_user_cache_header *header,
	uint32_t block_offset,
	uint32_t block_size
)
{
	zend_opcache_user_cache_entry *entries, *entry;
	zend_opcache_user_cache_entry_lock_record *record;
	uint32_t index;
	bool referenced = false;

	entries = zend_opcache_user_cache_entries(header);
	for (index = 0; index < header->capacity; index++) {
		entry = &entries[index];
		if (entry->state != ZEND_OPCACHE_USER_CACHE_ENTRY_USED) {
			continue;
		}

		if (entry->key_offset != 0 &&
			zend_opcache_user_cache_offset_in_block(entry->key_offset, block_offset, block_size)
		) {
			referenced = true;
		}

		if (entry->value_offset != 0 &&
			zend_opcache_user_cache_offset_in_block(entry->value_offset, block_offset, block_size)
		) {
			referenced = true;
			if (entry->value_type == ZEND_OPCACHE_USER_CACHE_VALUE_SHARED_GRAPH) {
				if (!zend_opcache_user_cache_shared_graph_can_move_payload_locked(entry->value_offset)) {
					return false;
				}
			}
		}
	}

	for (index = 0; index < ZEND_OPCACHE_USER_CACHE_ENTRY_LOCK_TABLE_SIZE; index++) {
		record = &header->entry_lock_records[index];
		if (record->state == ZEND_OPCACHE_USER_CACHE_ENTRY_LOCK_USED &&
			record->key_offset != 0 &&
			zend_opcache_user_cache_offset_in_block(record->key_offset, block_offset, block_size)
		) {
			referenced = true;
		}
	}

	return referenced;
}

static void zend_opcache_user_cache_update_moved_block_entries_locked(
	zend_opcache_user_cache_header *header,
	uint32_t old_block_offset,
	uint32_t new_block_offset,
	uint32_t block_size
)
{
	zend_opcache_user_cache_entry *entries, *entry;
	zend_opcache_user_cache_entry_lock_record *record;
	uint32_t index, delta, new_value_offset;

	ZEND_ASSERT(new_block_offset <= old_block_offset);
	delta = old_block_offset - new_block_offset;
	entries = zend_opcache_user_cache_entries(header);
	for (index = 0; index < header->capacity; index++) {
		entry = &entries[index];
		if (entry->state != ZEND_OPCACHE_USER_CACHE_ENTRY_USED) {
			continue;
		}

		if (entry->key_offset != 0 &&
			zend_opcache_user_cache_offset_in_block(entry->key_offset, old_block_offset, block_size)
		) {
			entry->key_offset -= delta;
		}

		if (entry->value_offset != 0 &&
			zend_opcache_user_cache_offset_in_block(entry->value_offset, old_block_offset, block_size)
		) {
			new_value_offset = entry->value_offset - delta;

			if (entry->value_type == ZEND_OPCACHE_USER_CACHE_VALUE_SHARED_GRAPH) {
				if (!zend_opcache_user_cache_shared_graph_rebase_moved_payload_locked(new_value_offset, delta)) {
					ZEND_ASSERT(0);
				}
			}

			entry->value_offset = new_value_offset;
		}
	}

	for (index = 0; index < ZEND_OPCACHE_USER_CACHE_ENTRY_LOCK_TABLE_SIZE; index++) {
		record = &header->entry_lock_records[index];
		if (record->state == ZEND_OPCACHE_USER_CACHE_ENTRY_LOCK_USED &&
			record->key_offset != 0 &&
			zend_opcache_user_cache_offset_in_block(record->key_offset, old_block_offset, block_size)
		) {
			record->key_offset -= delta;
		}
	}
}

static bool zend_opcache_user_cache_compaction_can_fit_locked(
	zend_opcache_user_cache_header *header,
	uint32_t required_block_size
)
{
	zend_opcache_user_cache_block *block;
	uint32_t data_end, used_end, offset, next_offset, block_size, region_start, region_used_size, write_offset, max_free_size = 0, region_free_size;
	bool movable, would_move = false;

	data_end = header->data_offset + header->data_size;
	used_end = header->data_offset + header->next_free;
	offset = header->data_offset;
	region_start = header->data_offset;
	region_used_size = 0;
	write_offset = header->data_offset;

	while (offset < used_end) {
		block = zend_opcache_user_cache_block_ptr(offset);
		block_size = block->size;
		if (block_size < ZEND_ALIGNED_SIZE(sizeof(zend_opcache_user_cache_block) + 1) ||
			block_size > used_end - offset
		) {
			return false;
		}

		next_offset = offset + block_size;
		if (!zend_opcache_user_cache_block_is_free(block)) {
			movable = zend_opcache_user_cache_block_is_movable_locked(header, offset, block_size);
			if (movable) {
				if (offset != write_offset) {
					would_move = true;
				}
				region_used_size += block_size;
				write_offset += block_size;
			} else {
				if (offset - region_start < region_used_size) {
					return false;
				}

				region_free_size = offset - region_start - region_used_size;

				if (region_free_size > max_free_size) {
					max_free_size = region_free_size;
				}

				region_start = next_offset;
				region_used_size = 0;
				write_offset = next_offset;
			}
		}

		offset = next_offset;
	}

	if (data_end - region_start < region_used_size) {
		return false;
	}

	if (data_end - region_start - region_used_size > max_free_size) {
		max_free_size = data_end - region_start - region_used_size;
	}

	return would_move && max_free_size >= required_block_size;
}

static bool zend_opcache_user_cache_compact_movable_blocks_locked(zend_opcache_user_cache_header *header)
{
	zend_opcache_user_cache_block *block, *free_block;
	uint32_t used_end, offset, next_offset, block_size, write_offset,
			previous_block_size = 0, last_block_offset = 0, free_size;
	bool moved = false, movable;

	used_end = header->data_offset + header->next_free;
	offset = header->data_offset;
	write_offset = header->data_offset;
	header->free_list = 0;

	while (offset < used_end) {
		block = zend_opcache_user_cache_block_ptr(offset);
		block_size = block->size;
		if (block_size < ZEND_ALIGNED_SIZE(sizeof(zend_opcache_user_cache_block) + 1) ||
			block_size > used_end - offset
		) {
			return false;
		}

		next_offset = offset + block_size;
		if (zend_opcache_user_cache_block_is_free(block)) {
			offset = next_offset;
			continue;
		}

		movable = zend_opcache_user_cache_block_is_movable_locked(header, offset, block_size);
		if (!movable) {
			if (write_offset < offset) {
				free_block = zend_opcache_user_cache_block_ptr(write_offset);
				free_size = offset - write_offset;

				free_block->size = free_size;
				free_block->prev_size = previous_block_size;
				free_block->next_free = 0;
				free_block->prev_free = 0;
				free_block->flags = ZEND_OPCACHE_USER_CACHE_BLOCK_FREE;
				zend_opcache_user_cache_free_list_insert_locked(header, write_offset);
				previous_block_size = free_size;
				last_block_offset = write_offset;
			}

			block->prev_size = previous_block_size;
			block->next_free = 0;
			block->prev_free = 0;
			zend_opcache_user_cache_block_mark_used(block);
			previous_block_size = block_size;
			last_block_offset = offset;
			write_offset = next_offset;
			offset = next_offset;
			continue;
		}

		if (write_offset != offset) {
			memmove(zend_opcache_user_cache_ptr(write_offset), block, block_size);
			zend_opcache_user_cache_update_moved_block_entries_locked(header, offset, write_offset, block_size);
			block = zend_opcache_user_cache_block_ptr(write_offset);
			moved = true;
		}

		block->prev_size = previous_block_size;
		block->next_free = 0;
		block->prev_free = 0;
		zend_opcache_user_cache_block_mark_used(block);
		previous_block_size = block_size;
		last_block_offset = write_offset;
		write_offset += block_size;
		offset = next_offset;
	}

	header->next_free = write_offset - header->data_offset;
	header->last_block_offset = last_block_offset;

	if (moved) {
		zend_opcache_user_cache_bump_mutation_epoch_locked(header);
	}

	return moved;
}

static bool zend_opcache_user_cache_compact_if_low_memory_locked(
	zend_opcache_user_cache_header *header,
	uint32_t allocating_block_size
)
{
	uint32_t tail_remaining;

	if (!zend_opcache_user_cache_active_context()->clear_on_pressure ||
		header->free_list == 0 ||
		header->next_free > header->data_size
	) {
		return false;
	}

	tail_remaining = header->data_size - header->next_free;
	if (tail_remaining >= ZEND_OPCACHE_USER_CACHE_LOW_MEMORY_COMPACT_THRESHOLD &&
		allocating_block_size <= tail_remaining - ZEND_OPCACHE_USER_CACHE_LOW_MEMORY_COMPACT_THRESHOLD
	) {
		return false;
	}

	if (!zend_opcache_user_cache_compaction_can_fit_locked(header, 0)) {
		return false;
	}

	return zend_opcache_user_cache_compact_movable_blocks_locked(header);
}

#ifdef ZEND_OPCACHE_USER_CACHE_HAVE_OPTIMISTIC
static void zend_opcache_user_cache_write_section_enter(void)
{
	zend_opcache_user_cache_header *header = zend_opcache_user_cache_header_ptr();

	zend_opcache_user_cache_write_seq_bumped = false;
	zend_opcache_user_cache_reader_drain_state = 0;

	if (header == NULL || !zend_opcache_user_cache_header_is_initialized_locked()) {
		return;
	}

	zend_opcache_user_cache_seq_announce(&header->write_seq, header->write_seq + 1);
	zend_opcache_user_cache_write_seq_bumped = true;
}

static void zend_opcache_user_cache_write_section_leave(void)
{
	zend_opcache_user_cache_header *header;

	if (!zend_opcache_user_cache_write_seq_bumped) {
		return;
	}

	zend_opcache_user_cache_write_seq_bumped = false;
	zend_opcache_user_cache_reader_drain_state = 0;

	header = zend_opcache_user_cache_header_ptr();
	if (header == NULL) {
		return;
	}

	zend_opcache_user_cache_seq_publish(&header->write_seq, header->write_seq + 1);
}

/* A freshly initialized header becomes visible before the write section ends. */
static void zend_opcache_user_cache_write_section_note_header_initialized(zend_opcache_user_cache_header *header)
{
	if (!zend_opcache_user_cache_lock_held_is_write || zend_opcache_user_cache_write_seq_bumped) {
		return;
	}

	zend_opcache_user_cache_seq_announce(&header->write_seq, header->write_seq + 1);
	zend_opcache_user_cache_write_seq_bumped = true;
	zend_opcache_user_cache_reader_drain_state = 0;
}

static bool zend_opcache_user_cache_reader_slot_owner_is_dead(const zend_opcache_user_cache_reader_slot *slot)
{
	return zend_opcache_user_cache_process_owner_is_dead(slot->owner_pid, slot->owner_start_time);
}

#ifndef ZEND_WIN32
static void zend_opcache_user_cache_optimistic_atfork_child(void)
{
	zend_opcache_user_cache_reader_claim_count = 0;
}
#endif

static int32_t zend_opcache_user_cache_claim_reader_slot(zend_opcache_user_cache_header *header)
{
	zend_opcache_user_cache_reader_slot *slot;
	uint64_t my_pid, expected;
	uint32_t index;

	for (index = 0; index < zend_opcache_user_cache_reader_claim_count; index++) {
		if (zend_opcache_user_cache_reader_claims[index].header == header) {
			return (int32_t) zend_opcache_user_cache_reader_claims[index].slot_index;
		}
	}

	if (zend_opcache_user_cache_reader_claim_count == ZEND_OPCACHE_USER_CACHE_READER_CLAIM_MAX ||
		zend_opcache_user_cache_reader_incarnation == 0
	) {
		return -1;
	}

	my_pid = zend_opcache_user_cache_current_pid();

	for (index = 0; index < ZEND_OPCACHE_USER_CACHE_READER_SLOTS; index++) {
		slot = &header->reader_slots[index];

		expected = zend_opcache_user_cache_atomic_load_64(&slot->owner_pid);

		if (expected == my_pid &&
			slot->owner_incarnation != zend_opcache_user_cache_reader_incarnation
		) {
			/* Stale claim from a previous incarnation of this PID. */
			zend_opcache_user_cache_atomic_store_32(&slot->active, 0);
			slot->owner_incarnation = zend_opcache_user_cache_reader_incarnation;
			slot->owner_start_time = zend_opcache_user_cache_process_start_time(my_pid);

			goto claimed;
		}

		if (expected != 0) {
			if (zend_opcache_user_cache_reader_slot_owner_is_dead(slot) &&
				zend_opcache_user_cache_atomic_load_32(&slot->active) == 0 &&
				zend_opcache_user_cache_atomic_cas_64(&slot->owner_pid, expected, my_pid)
			) {
				slot->owner_incarnation = zend_opcache_user_cache_reader_incarnation;
				slot->owner_start_time = zend_opcache_user_cache_process_start_time(my_pid);

				goto claimed;
			}

			continue;
		}

		if (zend_opcache_user_cache_atomic_cas_64(&slot->owner_pid, 0, my_pid)) {
			slot->owner_incarnation = zend_opcache_user_cache_reader_incarnation;
			slot->owner_start_time = zend_opcache_user_cache_process_start_time(my_pid);

			goto claimed;
		}
	}

	return -1;

claimed:
	zend_opcache_user_cache_reader_claims[zend_opcache_user_cache_reader_claim_count].header = header;
	zend_opcache_user_cache_reader_claims[zend_opcache_user_cache_reader_claim_count].slot_index = index;
	zend_opcache_user_cache_reader_claim_count++;

	return (int32_t) index;
}

bool zend_opcache_user_cache_graph_payloads_quiescent_locked(void)
{
	zend_opcache_user_cache_header *header;
	zend_opcache_user_cache_reader_slot *slot;
	uint64_t waited_us = 0;
	uint32_t count, index, spin = 0;

	if (!zend_opcache_user_cache_write_seq_bumped) {
		return true;
	}

	if (zend_opcache_user_cache_reader_drain_state != 0) {
		return zend_opcache_user_cache_reader_drain_state > 0;
	}

	header = zend_opcache_user_cache_header_ptr();
	if (header == NULL) {
		return true;
	}

	/* Pair with the reader's publish fence: after this fence, either the
	 * reader's counter increment is visible here, or the reader observes the
	 * odd sequence and backs off without pinning. */
	zend_opcache_user_cache_atomic_fence_seq_cst();

	for (;;) {
		count = zend_opcache_user_cache_atomic_load_32(&header->active_optimistic_readers);
		if (count == 0) {
			zend_opcache_user_cache_reader_drain_state = 1;

			return true;
		}

		spin++;

		if ((spin & 0xFFU) == 0) {
			/* Recover readers whose owner is known dead. */
			for (index = 0; index < ZEND_OPCACHE_USER_CACHE_READER_SLOTS; index++) {
				slot = &header->reader_slots[index];

				if (zend_opcache_user_cache_atomic_load_32(&slot->active) == 0 || slot->owner_pid == 0) {
					continue;
				}

				if (zend_opcache_user_cache_reader_slot_owner_is_dead(slot) &&
					zend_opcache_user_cache_atomic_cas_32(&slot->active, 1, 0)
				) {
					zend_opcache_user_cache_atomic_dec_32(&header->active_optimistic_readers);
				}
			}
		}

		if (spin > ZEND_OPCACHE_USER_CACHE_READER_DRAIN_SPIN) {
#ifdef ZEND_WIN32
			Sleep(1);
			waited_us += 1000;
#else
			usleep(50);
			waited_us += 50;
#endif

			if (waited_us > ZEND_OPCACHE_USER_CACHE_READER_DRAIN_TIMEOUT_US) {
				zend_opcache_user_cache_reader_drain_state = -1;

				return false;
			}
		}
	}
}

void zend_opcache_user_cache_optimistic_fork_setup(void)
{
	static bool registered = false;

	if (zend_opcache_user_cache_reader_incarnation == 0) {
		zend_opcache_user_cache_reader_incarnation =
			((uint64_t) time(NULL) << 20) ^
			(uint64_t) (uintptr_t) &zend_opcache_user_cache_reader_incarnation ^
			zend_opcache_user_cache_current_pid()
		;
		if (zend_opcache_user_cache_reader_incarnation == 0) {
			zend_opcache_user_cache_reader_incarnation = 1;
		}
	}

	if (registered) {
		return;
	}

	registered = true;

#ifndef ZEND_WIN32
	pthread_atfork(NULL, NULL, zend_opcache_user_cache_optimistic_atfork_child);
#endif
}

bool zend_opcache_user_cache_optimistic_reader_begin(zend_opcache_user_cache_header *header, uint32_t *slot_index_ptr)
{
	int32_t slot_index = zend_opcache_user_cache_claim_reader_slot(header);

	if (slot_index < 0) {
		return false;
	}

	*slot_index_ptr = (uint32_t) slot_index;

	zend_opcache_user_cache_atomic_store_32(&header->reader_slots[slot_index].active, 1);
	/* Pairs with the draining writer's fence. */
	zend_opcache_user_cache_atomic_inc_32(&header->active_optimistic_readers);

	return true;
}

void zend_opcache_user_cache_optimistic_reader_end(zend_opcache_user_cache_header *header, uint32_t slot_index)
{
	zend_opcache_user_cache_atomic_dec_32(&header->active_optimistic_readers);
	zend_opcache_user_cache_atomic_store_32(&header->reader_slots[slot_index].active, 0);
}
#else
bool zend_opcache_user_cache_graph_payloads_quiescent_locked(void)
{
	return true;
}

void zend_opcache_user_cache_optimistic_fork_setup(void)
{
}

bool zend_opcache_user_cache_optimistic_reader_begin(zend_opcache_user_cache_header *header, uint32_t *slot_index_ptr)
{
	(void) header;
	(void) slot_index_ptr;

	return false;
}

void zend_opcache_user_cache_optimistic_reader_end(zend_opcache_user_cache_header *header, uint32_t slot_index)
{
	(void) header;
	(void) slot_index;
}
#endif

void zend_opcache_user_cache_reset_runtime(void)
{
	zend_opcache_user_cache_context *context = zend_opcache_user_cache_active_context();
	zend_opcache_user_cache_runtime *runtime = zend_opcache_user_cache_active_runtime();

	memset(runtime, 0, sizeof(*runtime));

	runtime->configured_memory = ZCG(accel_directives).user_cache_shm_size;

	runtime->enabled = runtime->configured_memory != 0;
	if (!runtime->enabled) {
		runtime->available = false;
		runtime->startup_failed = false;
		runtime->backend_initialized = context->storage.initialized;
		runtime->failure_reason = "OPcache User Cache is disabled by opcache.user_cache_shm_size=0";
	}

}

void zend_opcache_user_cache_reset_storage(void)
{
	zend_opcache_user_cache_storage *storage = &zend_opcache_user_cache_active_context()->storage;

	memset(storage, 0, sizeof(*storage));
	storage->lock_file = -1;
}

bool zend_opcache_user_cache_header_init_locked(void)
{
	zend_opcache_user_cache_storage *storage = &zend_opcache_user_cache_active_context()->storage;
	zend_opcache_user_cache_header *header = zend_opcache_user_cache_header_ptr();
	uint32_t capacity, data_offset;

	if (!header) {
		return false;
	}

	if (header->magic == ZEND_OPCACHE_USER_CACHE_MAGIC && header->version == ZEND_OPCACHE_USER_CACHE_VERSION) {
		return true;
	}

	capacity = zend_opcache_user_cache_calculate_capacity(storage->size);
	data_offset = (uint32_t) ZEND_ALIGNED_SIZE(sizeof(zend_opcache_user_cache_header) + capacity * sizeof(zend_opcache_user_cache_entry));
	if (data_offset >= storage->size) {
		return false;
	}

	memset(header, 0, data_offset);
	header->capacity = capacity;
	header->data_offset = data_offset;
	header->data_size = (uint32_t) (storage->size - data_offset);
	header->next_free = 0;
	header->free_list = 0;
	header->last_block_offset = 0;
	header->count = 0;
	header->mutation_epoch = 1;
	header->write_seq = 2;
#ifdef ZEND_OPCACHE_USER_CACHE_HAVE_SHARED_MUTEX
	header->lock_model = zend_opcache_user_cache_shared_mutexes_init(header)
		? ZEND_OPCACHE_USER_CACHE_LOCK_MODEL_MUTEX
		: ZEND_OPCACHE_USER_CACHE_LOCK_MODEL_FCNTL
	;
#else
	header->lock_model = ZEND_OPCACHE_USER_CACHE_LOCK_MODEL_FCNTL;
#endif
	header->magic = ZEND_OPCACHE_USER_CACHE_MAGIC;
	header->version = ZEND_OPCACHE_USER_CACHE_VERSION;

#ifdef ZEND_OPCACHE_USER_CACHE_HAVE_OPTIMISTIC
	zend_opcache_user_cache_write_section_note_header_initialized(header);
#endif

	return true;
}

void zend_opcache_user_cache_free_locked(uint32_t payload_offset)
{
	zend_opcache_user_cache_header *header = zend_opcache_user_cache_header_ptr();
	zend_opcache_user_cache_block *block, *adjacent;
	uint32_t block_offset, original_block_offset, next_offset, prev_offset;

	if (!header || payload_offset < sizeof(zend_opcache_user_cache_block)) {
		return;
	}

	block_offset = payload_offset - (uint32_t) sizeof(zend_opcache_user_cache_block);
	original_block_offset = block_offset;
	block = zend_opcache_user_cache_block_ptr(block_offset);
	if (zend_opcache_user_cache_block_is_free(block)) {
		return;
	}

	zend_opcache_user_cache_block_mark_free(block);

	next_offset = block_offset + block->size;
	if (next_offset < zend_opcache_user_cache_used_end_offset_locked(header)) {
		adjacent = zend_opcache_user_cache_block_ptr(next_offset);

		if (zend_opcache_user_cache_block_is_free(adjacent)) {
			zend_opcache_user_cache_free_list_remove_locked(header, next_offset);
			block->size += adjacent->size;
			if (header->last_block_offset == next_offset) {
				header->last_block_offset = block_offset;
			}
		}
	}

	if (block->prev_size != 0 && block_offset > header->data_offset) {
		prev_offset = block_offset - block->prev_size;

		adjacent = zend_opcache_user_cache_block_ptr(prev_offset);
		if (zend_opcache_user_cache_block_is_free(adjacent)) {
			zend_opcache_user_cache_free_list_remove_locked(header, prev_offset);
			block->size += adjacent->size;
			adjacent->size = block->size;
			block = adjacent;
			block_offset = prev_offset;
			if (header->last_block_offset == original_block_offset) {
				header->last_block_offset = block_offset;
			}
		}
	}

	zend_opcache_user_cache_update_following_prev_size_locked(header, block_offset, block);
	zend_opcache_user_cache_free_list_insert_locked(header, block_offset);
	zend_opcache_user_cache_trim_tail_free_blocks_locked(header, block_offset);
}

uint32_t zend_opcache_user_cache_alloc_locked(size_t size, const void *source)
{
	zend_opcache_user_cache_header *header = zend_opcache_user_cache_header_ptr();
	zend_opcache_user_cache_block *block, *remainder;
	uint32_t total_size, min_split_size, best_offset = 0, best_size = UINT32_MAX, block_offset, *free_offset_ptr;
	size_t aligned_size;

	min_split_size = (uint32_t) ZEND_ALIGNED_SIZE(sizeof(zend_opcache_user_cache_block) + 1);

	if (!header || size == 0 || size > UINT32_MAX - sizeof(zend_opcache_user_cache_block)) {
		return 0;
	}

	aligned_size = ZEND_ALIGNED_SIZE(sizeof(zend_opcache_user_cache_block) + size);
	if (aligned_size > UINT32_MAX) {
		return 0;
	}

	total_size = (uint32_t) aligned_size;
	zend_opcache_user_cache_compact_if_low_memory_locked(header, total_size);

	free_offset_ptr = &header->free_list;
	while (*free_offset_ptr != 0) {
		block = zend_opcache_user_cache_block_ptr(*free_offset_ptr);
		if (block->size >= total_size && block->size < best_size) {
			best_offset = *free_offset_ptr;
			best_size = block->size;
			if (best_size == total_size) {
				break;
			}
		}
		free_offset_ptr = &block->next_free;
	}

	if (best_offset != 0) {
		block = zend_opcache_user_cache_block_ptr(best_offset);
		zend_opcache_user_cache_free_list_remove_locked(header, best_offset);
		if (block->size >= total_size + min_split_size) {
			remainder = zend_opcache_user_cache_block_ptr(best_offset + total_size);
			remainder->size = block->size - total_size;
			remainder->prev_size = total_size;
			remainder->next_free = 0;
			remainder->prev_free = 0;
			remainder->flags = ZEND_OPCACHE_USER_CACHE_BLOCK_FREE;
			block->size = total_size;
			zend_opcache_user_cache_update_following_prev_size_locked(header, best_offset + total_size, remainder);
			zend_opcache_user_cache_free_list_insert_locked(header, best_offset + total_size);
			if (header->last_block_offset == best_offset) {
				header->last_block_offset = best_offset + total_size;
			}
		} else {
			zend_opcache_user_cache_update_following_prev_size_locked(header, best_offset, block);
		}
		zend_opcache_user_cache_block_mark_used(block);
		if (source != NULL) {
			memcpy(zend_opcache_user_cache_ptr(best_offset + sizeof(zend_opcache_user_cache_block)), source, size);
		}

		return best_offset + (uint32_t) sizeof(zend_opcache_user_cache_block);
	}

	if (header->next_free > header->data_size || total_size > header->data_size - header->next_free) {
		return 0;
	}

	block_offset = header->data_offset + header->next_free;
	block = zend_opcache_user_cache_block_ptr(block_offset);
	block->size = total_size;
	block->prev_size = header->last_block_offset != 0 ? zend_opcache_user_cache_block_ptr(header->last_block_offset)->size : 0;
	block->next_free = 0;
	block->prev_free = 0;
	block->flags = 0;
	if (source != NULL) {
		memcpy(zend_opcache_user_cache_ptr(block_offset + sizeof(zend_opcache_user_cache_block)), source, size);
	}
	header->next_free += total_size;
	header->last_block_offset = block_offset;

	return block_offset + (uint32_t) sizeof(zend_opcache_user_cache_block);
}

bool zend_opcache_user_cache_compact_to_fit_locked(size_t size)
{
	zend_opcache_user_cache_header *header = zend_opcache_user_cache_header_ptr();
	uint32_t required_block_size;

	if (!header || !zend_opcache_user_cache_header_init_locked()) {
		return false;
	}

	if (!zend_opcache_user_cache_payload_size_to_block_size(size, &required_block_size) ||
		required_block_size > header->data_size ||
		!zend_opcache_user_cache_compaction_can_fit_locked(header, required_block_size)
	) {
		return false;
	}

	return zend_opcache_user_cache_compact_movable_blocks_locked(header);
}

bool zend_opcache_user_cache_startup_storage_before_request(void)
{
	zend_opcache_user_cache_storage *storage = &zend_opcache_user_cache_active_context()->storage;

	if (zend_opcache_user_cache_force_startup_failure()) {
		zend_opcache_user_cache_set_unavailable("Unable to initialize shared memory backend", true);

		return false;
	}

	if (!zend_opcache_user_cache_environment_is_allowed()) {
		zend_opcache_user_cache_set_unavailable("OPcache User Cache is not enabled for this SAPI", false);

		return true;
	}

	if (!zend_opcache_user_cache_startup_storage()) {
		zend_opcache_user_cache_set_unavailable("Unable to initialize shared memory backend", true);

		return false;
	}

	storage->initialized_before_request = true;

	return true;
}

void zend_opcache_user_cache_shutdown_storage(void)
{
	zend_opcache_user_cache_storage *storage = &zend_opcache_user_cache_active_context()->storage;

	zend_opcache_user_cache_lock_shutdown();
	zend_opcache_user_cache_cleanup_segments(
		storage->handler,
		storage->segments,
		storage->segment_count
	);
	zend_opcache_user_cache_reset_storage();
}

void zend_opcache_user_cache_ensure_ready(void)
{
	zend_opcache_user_cache_context *context = zend_opcache_user_cache_active_context();
	zend_opcache_user_cache_runtime *runtime;
	zend_opcache_user_cache_storage *storage = &context->storage;

	zend_opcache_user_cache_reset_runtime();
	runtime = zend_opcache_user_cache_active_runtime();
	if (!runtime->enabled) {
		return;
	}

	if (zend_opcache_user_cache_request_unavailable_reason != NULL) {
		zend_opcache_user_cache_set_unavailable(zend_opcache_user_cache_request_unavailable_reason, false);

		return;
	}

	if (!zend_opcache_user_cache_environment_is_allowed()) {
		zend_opcache_user_cache_set_unavailable("OPcache User Cache is not enabled for this SAPI", false);

		return;
	}

	if (zend_opcache_user_cache_opcache_is_disabled_for_sapi()) {
		zend_opcache_user_cache_set_unavailable("OPcache is disabled", false);

		return;
	}

	if (!accel_startup_ok) {
		zend_opcache_user_cache_set_unavailable("OPcache startup failed", true);

		return;
	}

	if (file_cache_only) {
		zend_opcache_user_cache_set_unavailable("Cache is unavailable in file_cache_only mode", false);

		return;
	}

	if (!storage->initialized &&
		zend_opcache_user_cache_requires_pre_request_storage()
	) {
		zend_opcache_user_cache_set_unavailable("Shared memory backend was not initialized before worker startup", true);

		return;
	}

	if (!zend_opcache_user_cache_startup_storage()) {
		zend_opcache_user_cache_set_unavailable("Unable to initialize shared memory backend", true);

		return;
	}

	if (zend_opcache_user_cache_requires_pre_request_storage() &&
		!storage->initialized_before_request
	) {
		zend_opcache_user_cache_set_unavailable("Shared memory backend was initialized after worker startup", true);

		return;
	}

	zend_opcache_user_cache_set_available();
}

bool zend_opcache_user_cache_rlock(void)
{
	if (!zend_opcache_user_cache_rlock_impl()) {
		return false;
	}

	zend_opcache_user_cache_lock_held_is_write = false;

	return true;
}

bool zend_opcache_user_cache_wlock(void)
{
	if (!zend_opcache_user_cache_wlock_impl()) {
		return false;
	}

	zend_opcache_user_cache_lock_held_is_write = true;
#ifdef ZEND_OPCACHE_USER_CACHE_HAVE_OPTIMISTIC
	zend_opcache_user_cache_write_section_enter();
#endif

	return true;
}

bool zend_opcache_user_cache_wlock_for_entry_mutation(zend_string *key)
{
	HashTable **locks_ptr = zend_opcache_user_cache_entry_lock_table_ptr();
	zend_opcache_user_cache_entry_lock *local_lock;
	zend_opcache_user_cache_header *header;
	zend_ulong hash = zend_string_hash_val(key);
	uint32_t slot_index;
	bool found;

	zend_opcache_user_cache_ensure_entry_lock_process();

	for (;;) {
		if (!zend_opcache_user_cache_wlock()) {
			return false;
		}

		if (!zend_opcache_user_cache_header_init_locked()) {
			zend_opcache_user_cache_unlock();

			return false;
		}

		header = zend_opcache_user_cache_header_ptr();
		if (!zend_opcache_user_cache_find_entry_lock_record_slot_locked(header, key, hash, &slot_index, &found) || !found) {
			return true;
		}

		if (*locks_ptr != NULL &&
			(local_lock = zend_hash_find_ptr(*locks_ptr, key)) != NULL &&
			header->entry_lock_records[slot_index].owner_pid == local_lock->owner_pid
		) {
			return true;
		}

		zend_opcache_user_cache_unlock();
		zend_opcache_user_cache_entry_lock_lease_wait();
	}
}

void zend_opcache_user_cache_unlock(void)
{
#ifdef ZEND_OPCACHE_USER_CACHE_HAVE_OPTIMISTIC
	if (zend_opcache_user_cache_lock_held_is_write) {
		zend_opcache_user_cache_write_section_leave();
	}
#endif
	zend_opcache_user_cache_lock_held_is_write = false;
	zend_opcache_user_cache_unlock_impl();
}

void zend_opcache_user_cache_unlock_if_held(void)
{
	if (zend_opcache_user_cache_lock_held) {
		zend_opcache_user_cache_unlock();
	}
}

bool zend_opcache_user_cache_acquire_entry_lock(zend_string *key)
{
	return zend_opcache_user_cache_acquire_entry_lock_record(
		zend_opcache_user_cache_active_context(),
		key,
		0,
		false,
		true
	);
}

bool zend_opcache_user_cache_try_acquire_entry_lock(zend_string *key, zend_long lease)
{
	return zend_opcache_user_cache_acquire_entry_lock_record(
		zend_opcache_user_cache_active_context(),
		key,
		lease,
		true,
		false
	);
}

bool zend_opcache_user_cache_has_entry_lock(zend_string *key)
{
	HashTable **locks_ptr = zend_opcache_user_cache_entry_lock_table_ptr();

	zend_opcache_user_cache_ensure_entry_lock_process();

	return *locks_ptr != NULL && zend_hash_exists(*locks_ptr, key);
}

bool zend_opcache_user_cache_release_entry_lock(zend_string *key)
{
	zend_opcache_user_cache_context *context = zend_opcache_user_cache_active_context();
	zend_opcache_user_cache_context *previous_context;
	zend_opcache_user_cache_entry_lock *lock;
	HashTable **locks_ptr = zend_opcache_user_cache_entry_lock_table_ptr();
	zend_opcache_user_cache_header *header;
	zend_ulong hash;
	uint32_t slot_index;
	bool found;

	zend_opcache_user_cache_ensure_entry_lock_process();

	if (*locks_ptr == NULL) {
		return false;
	}

	lock = zend_hash_find_ptr(*locks_ptr, key);
	if (lock == NULL) {
		return false;
	}

	hash = zend_string_hash_val(key);
	previous_context = zend_opcache_user_cache_activate_context(context);
	if (!zend_opcache_user_cache_wlock()) {
		zend_opcache_user_cache_restore_context(previous_context);

		return false;
	}

	header = zend_opcache_user_cache_header_ptr();
	if (zend_opcache_user_cache_header_is_initialized_locked() &&
		zend_opcache_user_cache_find_entry_lock_record_slot_locked(header, key, hash, &slot_index, &found) &&
		found &&
		header->entry_lock_records[slot_index].owner_pid == lock->owner_pid
	) {
		zend_opcache_user_cache_remove_entry_lock_record_locked(&header->entry_lock_records[slot_index]);
	}
	zend_opcache_user_cache_unlock();
	zend_opcache_user_cache_restore_context(previous_context);

	zend_hash_del(*locks_ptr, key);
	if (zend_hash_num_elements(*locks_ptr) == 0) {
		zend_hash_destroy(*locks_ptr);
		FREE_HASHTABLE(*locks_ptr);
		*locks_ptr = NULL;
	}

	return true;
}

bool zend_opcache_user_cache_entry_locks_allow_clear_locked(void)
{
	zend_opcache_user_cache_header *header = zend_opcache_user_cache_header_ptr();
	zend_opcache_user_cache_entry_lock_record *record;
	uint64_t owner_pid;
	uint64_t now;
	uint32_t index;

	zend_opcache_user_cache_ensure_entry_lock_process();

	if (header == NULL || !zend_opcache_user_cache_header_is_initialized_locked()) {
		return true;
	}

	owner_pid = zend_opcache_user_cache_entry_lock_current_pid();
	now = (uint64_t) time(NULL);

	for (index = 0; index < ZEND_OPCACHE_USER_CACHE_ENTRY_LOCK_TABLE_SIZE; index++) {
		record = &header->entry_lock_records[index];
		if (record->state != ZEND_OPCACHE_USER_CACHE_ENTRY_LOCK_USED) {
			continue;
		}

		if (!zend_opcache_user_cache_entry_lock_record_is_active_locked(record, now)) {
			zend_opcache_user_cache_remove_entry_lock_record_locked(record);
			continue;
		}

		if (record->owner_pid != owner_pid) {
			return false;
		}
	}

	return true;
}

void zend_opcache_user_cache_release_active_entry_locks(void)
{
	zend_opcache_user_cache_context *context = zend_opcache_user_cache_active_context();

	zend_opcache_user_cache_release_entry_lock_context(
		context,
		zend_opcache_user_cache_entry_lock_table_ptr(),
		ZEND_OPCACHE_USER_CACHE_ENTRY_LOCK_RELEASE_CLEAR
	);
}

void zend_opcache_user_cache_release_request_entry_locks(void)
{
	zend_opcache_user_cache_release_entry_lock_all_contexts(
		&zend_opcache_user_cache_entry_lock_table,
		ZEND_OPCACHE_USER_CACHE_ENTRY_LOCK_RELEASE_PRESERVE_LEASES
	);
#ifndef ZEND_WIN32
	zend_opcache_user_cache_entry_lock_owner_pid = 0;
#endif
}
