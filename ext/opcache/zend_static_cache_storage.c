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

#include "zend_static_cache_internal.h"

#ifndef ZEND_WIN32
# include <errno.h>
# include <fcntl.h>
# include <stdio.h>
# include <sys/stat.h>
# ifdef ZTS
#  include <pthread.h>
# endif
# ifdef HAVE_UNISTD_H
#  include <unistd.h>
# endif
# if defined(__linux__) && defined(HAVE_MEMFD_CREATE)
#  include <sys/mman.h>
# endif
#endif

static const zend_shared_memory_handler_entry zend_opcache_static_cache_handler_table[] = {
#ifdef USE_MMAP
	{ "mmap", &zend_alloc_mmap_handlers },
#endif
#ifdef USE_SHM
	{ "shm", &zend_alloc_shm_handlers },
#endif
#ifdef USE_SHM_OPEN
	{ "posix", &zend_alloc_posix_handlers },
#endif
#ifdef ZEND_WIN32
	{ "win32", &zend_alloc_win32_handlers },
#endif
	{ NULL, NULL }
};

#ifndef ZEND_WIN32
static ZEND_EXT_TLS zend_ulong zend_opcache_static_cache_entry_lock_owner_pid = 0;
#ifdef ZTS
static ZEND_EXT_TLS bool zend_opcache_static_cache_entry_locks_process_is_fork_child = false;
#endif
#endif

static zend_always_inline bool zend_opcache_static_cache_force_startup_failure(void)
{
	const char *value = getenv("OPCACHE_STATIC_CACHE_FORCE_STARTUP_FAILURE");

	return value != NULL && value[0] != '\0' && value[0] != '0';
}

static zend_always_inline bool zend_opcache_static_cache_requires_pre_request_storage(void)
{
	return sapi_module.name != NULL && strcmp(sapi_module.name, "fpm-fcgi") == 0;
}

static zend_always_inline void zend_opcache_static_cache_set_unavailable(const char *failure_reason, bool startup_failed)
{
	zend_opcache_static_cache_context *context = zend_opcache_static_cache_active_context();
	zend_opcache_static_cache_runtime *runtime = zend_opcache_static_cache_context_runtime(context);

	runtime->available = false;
	runtime->startup_failed = startup_failed;
	runtime->backend_initialized = context->storage.initialized;
	runtime->failure_reason = failure_reason;
}

static zend_always_inline void zend_opcache_static_cache_set_available(void)
{
	zend_opcache_static_cache_context *context = zend_opcache_static_cache_active_context();
	zend_opcache_static_cache_runtime *runtime = zend_opcache_static_cache_context_runtime(context);

	runtime->available = true;
	runtime->startup_failed = false;
	runtime->backend_initialized = context->storage.initialized;
	runtime->failure_reason = NULL;
}

static zend_always_inline void zend_opcache_static_cache_cleanup_segments(const zend_shared_memory_handlers *handler, zend_shared_segment **segments, int segment_count)
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

#ifndef ZEND_WIN32
#ifdef ZTS
static void zend_opcache_static_cache_entry_locks_shutdown(zend_opcache_static_cache_storage *storage)
{
	uint32_t index;

	if (!storage->entry_locks_initialized) {
		return;
	}

	for (index = 0; index < ZEND_OPCACHE_STATIC_CACHE_ENTRY_LOCK_STRIPES; index++) {
		if (storage->entry_locks[index] != NULL) {
			tsrm_mutex_free(storage->entry_locks[index]);
			storage->entry_locks[index] = NULL;
		}
	}

	storage->entry_locks_initialized = false;
}

static bool zend_opcache_static_cache_entry_locks_startup(zend_opcache_static_cache_storage *storage)
{
	uint32_t index;

	if (storage->entry_locks_initialized) {
		return true;
	}

	for (index = 0; index < ZEND_OPCACHE_STATIC_CACHE_ENTRY_LOCK_STRIPES; index++) {
		storage->entry_locks[index] = tsrm_mutex_alloc();
		if (storage->entry_locks[index] == NULL) {
			zend_opcache_static_cache_entry_locks_shutdown(storage);

			return false;
		}
	}
	storage->entry_locks_initialized = true;

	return true;
}
#endif

static bool zend_opcache_static_cache_lock_internal(short lock_type)
{
	zend_opcache_static_cache_storage *storage = &zend_opcache_static_cache_active_context()->storage;
	struct flock mem_lock;

	if (!storage->lock_initialized) {
		return false;
	}

#ifdef ZTS
	zend_opcache_static_cache_zts_lock_is_write = lock_type == F_WRLCK;
	if (!(lock_type == F_RDLCK
			? zend_thread_rwlock_rdlock(&storage->zts_lock)
			: zend_thread_rwlock_wrlock(&storage->zts_lock)
		)
	) {
		zend_opcache_static_cache_zts_lock_is_write = false;

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
			if (lock_type == F_RDLCK) {
				zend_thread_rwlock_unlock_rd(&storage->zts_lock);
			} else {
				zend_thread_rwlock_unlock_wr(&storage->zts_lock);
			}
			zend_opcache_static_cache_zts_lock_is_write = false;
#endif
			return false;
		}
	}

	return true;
}

static bool zend_opcache_static_cache_lock_startup(void)
{
	zend_opcache_static_cache_context *context = zend_opcache_static_cache_active_context();
	zend_opcache_static_cache_storage *storage = &context->storage;
	int val;

	if (storage->lock_initialized) {
		return true;
	}

#ifdef ZTS
	if (!zend_opcache_static_cache_entry_locks_startup(storage)) {
		return false;
	}

	if (!zend_thread_rwlock_init(&storage->zts_lock)) {
		zend_opcache_static_cache_entry_locks_shutdown(storage);

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
		zend_thread_rwlock_destroy(&storage->zts_lock);
		zend_opcache_static_cache_entry_locks_shutdown(storage);
#endif

		return false;
	}

	if (fchmod(storage->lock_file, 0666) == -1) {
		close(storage->lock_file);
		storage->lock_file = -1;
#ifdef ZTS
		zend_thread_rwlock_destroy(&storage->zts_lock);
		zend_opcache_static_cache_entry_locks_shutdown(storage);
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

static void zend_opcache_static_cache_lock_shutdown(void)
{
	zend_opcache_static_cache_storage *storage = &zend_opcache_static_cache_active_context()->storage;

	if (!storage->lock_initialized) {
		return;
	}

	if (storage->lock_file >= 0) {
		close(storage->lock_file);
		storage->lock_file = -1;
	}
#ifdef ZTS
	zend_thread_rwlock_destroy(&storage->zts_lock);
	zend_opcache_static_cache_entry_locks_shutdown(storage);
#endif
	storage->lock_initialized = false;
}

static bool zend_opcache_static_cache_rlock_impl(void)
{
	return zend_opcache_static_cache_lock_internal(F_RDLCK);
}

static bool zend_opcache_static_cache_wlock_impl(void)
{
	return zend_opcache_static_cache_lock_internal(F_WRLCK);
}

static void zend_opcache_static_cache_unlock_impl(void)
{
	zend_opcache_static_cache_storage *storage = &zend_opcache_static_cache_active_context()->storage;
	struct flock mem_unlock;
#ifdef ZTS
	bool zts_lock_is_write = zend_opcache_static_cache_zts_lock_is_write;
#endif

	if (!storage->lock_initialized) {
		return;
	}

	mem_unlock.l_type = F_UNLCK;
	mem_unlock.l_whence = SEEK_SET;
	mem_unlock.l_start = 0;
	mem_unlock.l_len = 1;
	fcntl(storage->lock_file, F_SETLK, &mem_unlock);

#ifdef ZTS
	if (zts_lock_is_write) {
		zend_thread_rwlock_unlock_wr(&storage->zts_lock);
	} else {
		zend_thread_rwlock_unlock_rd(&storage->zts_lock);
	}
	zend_opcache_static_cache_zts_lock_is_write = false;
#endif
}
#else
static bool zend_opcache_static_cache_lock_startup(void)
{
	return false;
}

static void zend_opcache_static_cache_lock_shutdown(void)
{
}

static bool zend_opcache_static_cache_rlock_impl(void)
{
	return false;
}

static bool zend_opcache_static_cache_wlock_impl(void)
{
	return false;
}

static void zend_opcache_static_cache_unlock_impl(void)
{
}
#endif

static HashTable **zend_opcache_static_cache_entry_locks_ptr_for_context(zend_opcache_static_cache_context *context)
{
	return context == &zend_opcache_static_cache_persistent_context_state
		? &zend_opcache_static_cache_persistent_entry_locks
		: &zend_opcache_static_cache_volatile_entry_locks
	;
}

static uint32_t *zend_opcache_static_cache_entry_lock_counts_for_context(zend_opcache_static_cache_context *context)
{
	return context == &zend_opcache_static_cache_persistent_context_state
		? zend_opcache_static_cache_persistent_entry_lock_counts
		: zend_opcache_static_cache_volatile_entry_lock_counts
	;
}

static uint32_t zend_opcache_static_cache_entry_lock_stripe(zend_string *key)
{
	return (uint32_t) (zend_string_hash_val(key) % ZEND_OPCACHE_STATIC_CACHE_ENTRY_LOCK_STRIPES);
}

#ifdef ZTS
static void zend_opcache_static_cache_entry_locks_reinit_after_fork(zend_opcache_static_cache_storage *storage)
{
	uint32_t index, allocated = 0;
	MUTEX_T old_locks[ZEND_OPCACHE_STATIC_CACHE_ENTRY_LOCK_STRIPES];
	MUTEX_T new_locks[ZEND_OPCACHE_STATIC_CACHE_ENTRY_LOCK_STRIPES];

	if (!storage->entry_locks_initialized) {
		return;
	}

	memcpy(old_locks, storage->entry_locks, sizeof(old_locks));

	/* A child process may inherit a process-local heap mutex whose copied
	 * state says it was locked by the parent. Replace the child-side copies
	 * instead of trying to unlock or reuse them. */
	for (index = 0; index < ZEND_OPCACHE_STATIC_CACHE_ENTRY_LOCK_STRIPES; index++) {
		new_locks[index] = tsrm_mutex_alloc();
		if (new_locks[index] == NULL) {
			while (allocated != 0) {
				tsrm_mutex_free(new_locks[--allocated]);
			}
			for (index = 0; index < ZEND_OPCACHE_STATIC_CACHE_ENTRY_LOCK_STRIPES; index++) {
				if (old_locks[index] != NULL) {
					tsrm_mutex_free(old_locks[index]);
				}
			}
			memset(storage->entry_locks, 0, sizeof(storage->entry_locks));
			storage->entry_locks_initialized = false;

			return;
		}
		allocated++;
	}

	for (index = 0; index < ZEND_OPCACHE_STATIC_CACHE_ENTRY_LOCK_STRIPES; index++) {
		if (old_locks[index] != NULL) {
			tsrm_mutex_free(old_locks[index]);
		}
	}

	memcpy(storage->entry_locks, new_locks, sizeof(storage->entry_locks));
}
#endif

static void zend_opcache_static_cache_release_entry_lock_context(HashTable **locks_ptr, uint32_t *counts)
{
	if (*locks_ptr == NULL) {
		return;
	}

	zend_hash_destroy(*locks_ptr);
	FREE_HASHTABLE(*locks_ptr);
	*locks_ptr = NULL;
	memset(counts, 0, sizeof(uint32_t) * ZEND_OPCACHE_STATIC_CACHE_ENTRY_LOCK_STRIPES);
}

static void zend_opcache_static_cache_ensure_entry_lock_process(void)
{
#ifndef ZEND_WIN32
	zend_ulong current_pid = (zend_ulong) getpid();

	if (zend_opcache_static_cache_entry_lock_owner_pid == 0) {
		zend_opcache_static_cache_entry_lock_owner_pid = current_pid;

		return;
	}

	if (zend_opcache_static_cache_entry_lock_owner_pid == current_pid) {
		return;
	}

	/* The request-local lock tables were inherited across fork. Destroy the
	 * child copies without unlocking stripes owned by the parent process. */
	zend_opcache_static_cache_release_entry_lock_context(
		&zend_opcache_static_cache_volatile_entry_locks,
		zend_opcache_static_cache_volatile_entry_lock_counts);
	zend_opcache_static_cache_release_entry_lock_context(
		&zend_opcache_static_cache_persistent_entry_locks,
		zend_opcache_static_cache_persistent_entry_lock_counts);
#ifdef ZTS
	zend_opcache_static_cache_entry_locks_reinit_after_fork(&zend_opcache_static_cache_volatile_context_state.storage);
	zend_opcache_static_cache_entry_locks_reinit_after_fork(&zend_opcache_static_cache_persistent_context_state.storage);
	zend_opcache_static_cache_entry_locks_process_is_fork_child = true;
#endif
	zend_opcache_static_cache_entry_lock_owner_pid = current_pid;
#endif
}

#ifndef ZEND_WIN32
static bool zend_opcache_static_cache_lock_entry_stripe_ex(zend_opcache_static_cache_context *context, uint32_t stripe, bool blocking)
{
	zend_opcache_static_cache_storage *storage = &context->storage;
	uint32_t *counts = zend_opcache_static_cache_entry_lock_counts_for_context(context);
	struct flock mem_lock;
#ifdef ZTS
	int result;
#endif

	if (counts[stripe] != 0) {
		counts[stripe]++;

		return true;
	}

#ifdef ZTS
	if (!storage->entry_locks_initialized && !zend_opcache_static_cache_entry_locks_startup(storage)) {
		return false;
	}

	if (storage->entry_locks[stripe] == NULL) {
		return false;
	}

	result = blocking
		? pthread_mutex_lock(storage->entry_locks[stripe])
		: pthread_mutex_trylock(storage->entry_locks[stripe])
	;
	if (result != 0) {
		return false;
	}
#endif

	if (!storage->lock_initialized || storage->lock_file < 0) {
#ifdef ZTS
		pthread_mutex_unlock(storage->entry_locks[stripe]);
#endif
		return false;
	}

	mem_lock.l_type = F_WRLCK;
	mem_lock.l_whence = SEEK_SET;
	mem_lock.l_start = (off_t) stripe + 1;
	mem_lock.l_len = 1;

	while (fcntl(storage->lock_file, blocking ? F_SETLKW : F_SETLK, &mem_lock) == -1) {
		if (errno != EINTR) {
#ifdef ZTS
			pthread_mutex_unlock(storage->entry_locks[stripe]);
#endif
			return false;
		}
	}

	counts[stripe] = 1;

	return true;
}

static bool zend_opcache_static_cache_lock_entry_stripe(zend_opcache_static_cache_context *context, uint32_t stripe)
{
	return zend_opcache_static_cache_lock_entry_stripe_ex(context, stripe, true);
}

static bool zend_opcache_static_cache_try_lock_entry_stripe(zend_opcache_static_cache_context *context, uint32_t stripe)
{
	return zend_opcache_static_cache_lock_entry_stripe_ex(context, stripe, false);
}

static void zend_opcache_static_cache_unlock_entry_stripe(zend_opcache_static_cache_context *context, uint32_t stripe)
{
	zend_opcache_static_cache_storage *storage = &context->storage;
	uint32_t *counts = zend_opcache_static_cache_entry_lock_counts_for_context(context);
	struct flock mem_unlock;

	ZEND_ASSERT(counts[stripe] != 0);
	if (--counts[stripe] != 0) {
		return;
	}

	if (storage->lock_initialized && storage->lock_file >= 0) {
		mem_unlock.l_type = F_UNLCK;
		mem_unlock.l_whence = SEEK_SET;
		mem_unlock.l_start = (off_t) stripe + 1;
		mem_unlock.l_len = 1;
		fcntl(storage->lock_file, F_SETLK, &mem_unlock);
	}

#ifdef ZTS
	if (storage->entry_locks_initialized && storage->entry_locks[stripe] != NULL) {
		tsrm_mutex_unlock(storage->entry_locks[stripe]);
	}
#endif
}
#endif

static void zend_opcache_static_cache_entry_lock_dtor(zval *lock_zv)
{
	zend_opcache_static_cache_entry_lock *lock = Z_PTR_P(lock_zv);

	if (lock == NULL) {
		return;
	}

#ifndef ZEND_WIN32
	if (lock->owner_pid == (zend_ulong) getpid()) {
		zend_opcache_static_cache_unlock_entry_stripe(lock->context, lock->stripe);
	}
#endif
	efree(lock);
}

static HashTable *zend_opcache_static_cache_entry_locks(zend_opcache_static_cache_context *context)
{
	HashTable **locks_ptr = zend_opcache_static_cache_entry_locks_ptr_for_context(context);

	if (*locks_ptr == NULL) {
		ALLOC_HASHTABLE(*locks_ptr);
		zend_hash_init(*locks_ptr, 0, NULL, zend_opcache_static_cache_entry_lock_dtor, 0);
	}

	return *locks_ptr;
}

static uint32_t zend_opcache_static_cache_calculate_capacity(size_t size)
{
	size_t capacity = size / ZEND_OPCACHE_STATIC_CACHE_SLOT_BYTES, data_offset;

	if (capacity < ZEND_OPCACHE_STATIC_CACHE_MIN_CAPACITY) {
		capacity = ZEND_OPCACHE_STATIC_CACHE_MIN_CAPACITY;
	} else if (capacity > ZEND_OPCACHE_STATIC_CACHE_MAX_CAPACITY) {
		capacity = ZEND_OPCACHE_STATIC_CACHE_MAX_CAPACITY;
	}

	if ((capacity & 1) == 0) {
		capacity--;
	}

	for (;;) {
		data_offset = ZEND_ALIGNED_SIZE(sizeof(zend_opcache_static_cache_header) + capacity * sizeof(zend_opcache_static_cache_entry));
		if (data_offset < size || capacity == ZEND_OPCACHE_STATIC_CACHE_MIN_CAPACITY) {
			break;
		}

		capacity >>= 1;
		if (capacity < ZEND_OPCACHE_STATIC_CACHE_MIN_CAPACITY) {
			capacity = ZEND_OPCACHE_STATIC_CACHE_MIN_CAPACITY;
		}

		if ((capacity & 1) == 0) {
			capacity--;
		}
	}

	return (uint32_t) capacity;
}

static uint32_t zend_opcache_static_cache_used_end_offset_locked(const zend_opcache_static_cache_header *header)
{
	return header->data_offset + header->next_free;
}

static void zend_opcache_static_cache_free_list_remove_locked(zend_opcache_static_cache_header *header, uint32_t block_offset)
{
	zend_opcache_static_cache_block *block = zend_opcache_static_cache_block_ptr(block_offset);

	if (block->prev_free != 0) {
		zend_opcache_static_cache_block_ptr(block->prev_free)->next_free = block->next_free;
	} else {
		header->free_list = block->next_free;
	}

	if (block->next_free != 0) {
		zend_opcache_static_cache_block_ptr(block->next_free)->prev_free = block->prev_free;
	}

	block->next_free = 0;
	block->prev_free = 0;
	block->flags &= ~ZEND_OPCACHE_STATIC_CACHE_BLOCK_FREE;
}

static void zend_opcache_static_cache_free_list_insert_locked(zend_opcache_static_cache_header *header, uint32_t block_offset)
{
	zend_opcache_static_cache_block *block = zend_opcache_static_cache_block_ptr(block_offset);

	block->prev_free = 0;
	block->next_free = header->free_list;

	if (header->free_list != 0) {
		zend_opcache_static_cache_block_ptr(header->free_list)->prev_free = block_offset;
	}

	zend_opcache_static_cache_block_mark_free(block);
	header->free_list = block_offset;
}

static void zend_opcache_static_cache_update_following_prev_size_locked(
	zend_opcache_static_cache_header *header,
	uint32_t block_offset,
	const zend_opcache_static_cache_block *block
)
{
	uint32_t next_offset = block_offset + block->size;

	if (next_offset < zend_opcache_static_cache_used_end_offset_locked(header)) {
		zend_opcache_static_cache_block_ptr(next_offset)->prev_size = block->size;
	}
}

static void zend_opcache_static_cache_trim_tail_free_blocks_locked(
	zend_opcache_static_cache_header *header,
	uint32_t block_offset
)
{
	zend_opcache_static_cache_block *block = zend_opcache_static_cache_block_ptr(block_offset);
	uint32_t prev_offset;

	while (block_offset >= header->data_offset &&
		header->last_block_offset == block_offset &&
		zend_opcache_static_cache_block_is_free(block) &&
		block_offset + block->size == zend_opcache_static_cache_used_end_offset_locked(header)
	) {
		prev_offset = 0;
		zend_opcache_static_cache_free_list_remove_locked(header, block_offset);
		header->next_free -= block->size;
		if (block->prev_size != 0 && block_offset > header->data_offset) {
			prev_offset = block_offset - block->prev_size;
		}

		header->last_block_offset = prev_offset;
		if (prev_offset == 0) {
			break;
		}

		block_offset = prev_offset;
		block = zend_opcache_static_cache_block_ptr(block_offset);
	}
}

static bool zend_opcache_static_cache_try_handler(const zend_shared_memory_handler_entry *handler_entry)
{
	const char *error_in = NULL;
	zend_opcache_static_cache_context *context = zend_opcache_static_cache_active_context();
	zend_opcache_static_cache_runtime *runtime = zend_opcache_static_cache_context_runtime(context);
	zend_opcache_static_cache_storage *storage = &context->storage;
	zend_shared_segment **segments = NULL;
	int segment_count = 0, result;

	result = handler_entry->handler->create_segments(
		runtime->configured_memory,
		&segments,
		&segment_count,
		&error_in
	);
	if (result != ALLOC_SUCCESS) {
		zend_opcache_static_cache_cleanup_segments(handler_entry->handler, segments, segment_count);
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

static bool zend_opcache_static_cache_startup_storage(void)
{
	const zend_shared_memory_handler_entry *handler_entry;
	const char *requested_model = ZCG(accel_directives).memory_model;
	zend_opcache_static_cache_storage *storage = &zend_opcache_static_cache_active_context()->storage;

	if (storage->initialized) {
		return true;
	}

	if (requested_model && requested_model[0]) {
		if (strcmp(requested_model, "cgi") == 0) {
			requested_model = "shm";
		}

		for (handler_entry = zend_opcache_static_cache_handler_table; handler_entry->name; handler_entry++) {
			if (strcmp(requested_model, handler_entry->name) == 0 && zend_opcache_static_cache_try_handler(handler_entry)) {
				goto storage_ready;
			}
		}
	}

	for (handler_entry = zend_opcache_static_cache_handler_table; handler_entry->name; handler_entry++) {
		if (requested_model && requested_model[0] && strcmp(requested_model, handler_entry->name) == 0) {
			continue;
		}

		if (zend_opcache_static_cache_try_handler(handler_entry)) {
			goto storage_ready;
		}
	}

	return false;

storage_ready:
	if (storage->segment_count != 1) {
		zend_opcache_static_cache_cleanup_segments(
			storage->handler,
			storage->segments,
			storage->segment_count
		);
		zend_opcache_static_cache_reset_storage();

		return false;
	}

	if (!zend_opcache_static_cache_lock_startup()) {
		zend_opcache_static_cache_cleanup_segments(
			storage->handler,
			storage->segments,
			storage->segment_count
		);
		zend_opcache_static_cache_reset_storage();

		return false;
	}

	if (!zend_opcache_static_cache_wlock()) {
		zend_opcache_static_cache_lock_shutdown();
		zend_opcache_static_cache_cleanup_segments(
			storage->handler,
			storage->segments,
			storage->segment_count
		);
		zend_opcache_static_cache_reset_storage();

		return false;
	}

	if (!zend_opcache_static_cache_header_init_locked()) {
		zend_opcache_static_cache_unlock();
		zend_opcache_static_cache_lock_shutdown();
		zend_opcache_static_cache_cleanup_segments(
			storage->handler,
			storage->segments,
			storage->segment_count
		);
		zend_opcache_static_cache_reset_storage();

		return false;
	}

	zend_opcache_static_cache_unlock();

	return true;
}

static bool zend_opcache_static_cache_payload_size_to_block_size(size_t size, uint32_t *block_size)
{
	size_t aligned_size;

	if (size == 0 || size > UINT32_MAX - sizeof(zend_opcache_static_cache_block)) {
		return false;
	}

	aligned_size = ZEND_ALIGNED_SIZE(sizeof(zend_opcache_static_cache_block) + size);
	if (aligned_size > UINT32_MAX) {
		return false;
	}

	*block_size = (uint32_t) aligned_size;

	return true;
}

static bool zend_opcache_static_cache_offset_in_block(uint32_t offset, uint32_t block_offset, uint32_t block_size)
{
	return offset >= block_offset + sizeof(zend_opcache_static_cache_block) && offset < block_offset + block_size;
}

static bool zend_opcache_static_cache_block_is_movable_locked(
	zend_opcache_static_cache_header *header,
	uint32_t block_offset,
	uint32_t block_size
)
{
	zend_opcache_static_cache_entry *entries, *entry;
	uint32_t index;
	bool referenced = false;

	entries = zend_opcache_static_cache_entries(header);
	for (index = 0; index < header->capacity; index++) {
		entry = &entries[index];
		if (entry->state != ZEND_OPCACHE_STATIC_CACHE_ENTRY_USED) {
			continue;
		}

		if (entry->key_offset != 0 &&
			zend_opcache_static_cache_offset_in_block(entry->key_offset, block_offset, block_size)
		) {
			referenced = true;
		}

		if (entry->value_offset != 0 &&
			zend_opcache_static_cache_offset_in_block(entry->value_offset, block_offset, block_size)
		) {
			referenced = true;
			if (entry->value_type == ZEND_OPCACHE_STATIC_CACHE_VALUE_SHARED_GRAPH) {
				/* Shared graphs may contain final-buffer pointers, so keep their block anchored. */
				return false;
			}
		}
	}

	return referenced;
}

static void zend_opcache_static_cache_update_moved_block_entries_locked(
	zend_opcache_static_cache_header *header,
	uint32_t old_block_offset,
	uint32_t new_block_offset,
	uint32_t block_size
)
{
	zend_opcache_static_cache_entry *entries, *entry;
	uint32_t index, delta;

	ZEND_ASSERT(new_block_offset <= old_block_offset);
	delta = old_block_offset - new_block_offset;
	entries = zend_opcache_static_cache_entries(header);
	for (index = 0; index < header->capacity; index++) {
		entry = &entries[index];
		if (entry->state != ZEND_OPCACHE_STATIC_CACHE_ENTRY_USED) {
			continue;
		}

		if (entry->key_offset != 0 &&
			zend_opcache_static_cache_offset_in_block(entry->key_offset, old_block_offset, block_size)
		) {
			entry->key_offset -= delta;
		}

		if (entry->value_offset != 0 &&
			zend_opcache_static_cache_offset_in_block(entry->value_offset, old_block_offset, block_size)
		) {
			entry->value_offset -= delta;
		}
	}
}

static bool zend_opcache_static_cache_compaction_can_fit_locked(
	zend_opcache_static_cache_header *header,
	uint32_t required_block_size
)
{
	zend_opcache_static_cache_block *block;
	uint32_t data_end, used_end, offset, next_offset, block_size, region_start, region_used_size, write_offset, max_free_size = 0, region_free_size;
	bool movable, would_move = false;

	data_end = header->data_offset + header->data_size;
	used_end = header->data_offset + header->next_free;
	offset = header->data_offset;
	region_start = header->data_offset;
	region_used_size = 0;
	write_offset = header->data_offset;

	while (offset < used_end) {
		block = zend_opcache_static_cache_block_ptr(offset);
		block_size = block->size;
		if (block_size < ZEND_ALIGNED_SIZE(sizeof(zend_opcache_static_cache_block) + 1) ||
			block_size > used_end - offset
		) {
			return false;
		}

		next_offset = offset + block_size;
		if (!zend_opcache_static_cache_block_is_free(block)) {
			movable = zend_opcache_static_cache_block_is_movable_locked(header, offset, block_size);
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

void zend_opcache_static_cache_reset_runtime(void)
{
	zend_opcache_static_cache_context *context = zend_opcache_static_cache_active_context();
	zend_opcache_static_cache_runtime *runtime = zend_opcache_static_cache_context_runtime(context);

	memset(runtime, 0, sizeof(*runtime));

	runtime->configured_memory = context == &zend_opcache_static_cache_persistent_context_state
		? ZCG(accel_directives).static_cache_persistent_size_mb
		: ZCG(accel_directives).static_cache_volatile_size_mb
	;
	runtime->enabled = runtime->configured_memory != 0;

	if (zend_opcache_static_cache_subsystem_disabled) {
		runtime->enabled = false;
		runtime->available = false;
		runtime->startup_failed = true;
		runtime->backend_initialized = context->storage.initialized;
		runtime->failure_reason = zend_opcache_static_cache_subsystem_failure_reason;
	}
}

void zend_opcache_static_cache_reset_storage(void)
{
	zend_opcache_static_cache_storage *storage = &zend_opcache_static_cache_active_context()->storage;

	memset(storage, 0, sizeof(*storage));
#ifndef ZEND_WIN32
	storage->lock_file = -1;
#endif
}

bool zend_opcache_static_cache_header_init_locked(void)
{
	zend_opcache_static_cache_storage *storage = &zend_opcache_static_cache_active_context()->storage;
	zend_opcache_static_cache_header *header = zend_opcache_static_cache_header_ptr();
	uint32_t capacity, data_offset;

	if (!header) {
		return false;
	}

	if (header->magic == ZEND_OPCACHE_STATIC_CACHE_MAGIC && header->version == ZEND_OPCACHE_STATIC_CACHE_VERSION) {
		return true;
	}

	capacity = zend_opcache_static_cache_calculate_capacity(storage->size);
	data_offset = (uint32_t) ZEND_ALIGNED_SIZE(sizeof(zend_opcache_static_cache_header) + capacity * sizeof(zend_opcache_static_cache_entry));
	if (data_offset >= storage->size) {
		return false;
	}

	/* Only the header and entry table need an eager zero state. Payload pages
	 * are touched on demand when allocator blocks are first created. */
	memset(header, 0, data_offset);
	header->capacity = capacity;
	header->data_offset = data_offset;
	header->data_size = (uint32_t) (storage->size - data_offset);
	header->next_free = 0;
	header->free_list = 0;
	header->last_block_offset = 0;
	header->count = 0;
	header->mutation_epoch = 1;
	header->magic = ZEND_OPCACHE_STATIC_CACHE_MAGIC;
	header->version = ZEND_OPCACHE_STATIC_CACHE_VERSION;

	return true;
}

void zend_opcache_static_cache_free_locked(uint32_t payload_offset)
{
	zend_opcache_static_cache_header *header = zend_opcache_static_cache_header_ptr();
	zend_opcache_static_cache_block *block, *adjacent;
	uint32_t block_offset, original_block_offset, next_offset, prev_offset;

	if (!header || payload_offset < sizeof(zend_opcache_static_cache_block)) {
		return;
	}

	block_offset = payload_offset - (uint32_t) sizeof(zend_opcache_static_cache_block);
	original_block_offset = block_offset;
	block = zend_opcache_static_cache_block_ptr(block_offset);
	if (zend_opcache_static_cache_block_is_free(block)) {
		return;
	}

	zend_opcache_static_cache_block_mark_free(block);

	next_offset = block_offset + block->size;
	if (next_offset < zend_opcache_static_cache_used_end_offset_locked(header)) {
		adjacent = zend_opcache_static_cache_block_ptr(next_offset);

		if (zend_opcache_static_cache_block_is_free(adjacent)) {
			zend_opcache_static_cache_free_list_remove_locked(header, next_offset);
			block->size += adjacent->size;
			if (header->last_block_offset == next_offset) {
				header->last_block_offset = block_offset;
			}
		}
	}

	if (block->prev_size != 0 && block_offset > header->data_offset) {
		prev_offset = block_offset - block->prev_size;

		adjacent = zend_opcache_static_cache_block_ptr(prev_offset);
		if (zend_opcache_static_cache_block_is_free(adjacent)) {
			zend_opcache_static_cache_free_list_remove_locked(header, prev_offset);
			block->size += adjacent->size;
			adjacent->size = block->size;
			block = adjacent;
			block_offset = prev_offset;
			if (header->last_block_offset == original_block_offset) {
				header->last_block_offset = block_offset;
			}
		}
	}

	zend_opcache_static_cache_update_following_prev_size_locked(header, block_offset, block);
	zend_opcache_static_cache_free_list_insert_locked(header, block_offset);
	zend_opcache_static_cache_trim_tail_free_blocks_locked(header, block_offset);
}

uint32_t zend_opcache_static_cache_alloc_locked(size_t size, const void *source)
{
	zend_opcache_static_cache_header *header = zend_opcache_static_cache_header_ptr();
	zend_opcache_static_cache_block *block, *remainder;
	uint32_t total_size, min_split_size, best_offset = 0, best_size = UINT32_MAX, block_offset, *free_offset_ptr;
	size_t aligned_size;

	min_split_size = (uint32_t) ZEND_ALIGNED_SIZE(sizeof(zend_opcache_static_cache_block) + 1);

	if (!header || size == 0 || size > UINT32_MAX - sizeof(zend_opcache_static_cache_block)) {
		return 0;
	}

	aligned_size = ZEND_ALIGNED_SIZE(sizeof(zend_opcache_static_cache_block) + size);
	if (aligned_size > UINT32_MAX) {
		return 0;
	}

	total_size = (uint32_t) aligned_size;

	free_offset_ptr = &header->free_list;
	while (*free_offset_ptr != 0) {
		block = zend_opcache_static_cache_block_ptr(*free_offset_ptr);
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
		block = zend_opcache_static_cache_block_ptr(best_offset);
		zend_opcache_static_cache_free_list_remove_locked(header, best_offset);
		if (block->size >= total_size + min_split_size) {
			remainder = zend_opcache_static_cache_block_ptr(best_offset + total_size);
			remainder->size = block->size - total_size;
			remainder->prev_size = total_size;
			remainder->next_free = 0;
			remainder->prev_free = 0;
			remainder->flags = ZEND_OPCACHE_STATIC_CACHE_BLOCK_FREE;
			block->size = total_size;
			zend_opcache_static_cache_update_following_prev_size_locked(header, best_offset + total_size, remainder);
			zend_opcache_static_cache_free_list_insert_locked(header, best_offset + total_size);
			if (header->last_block_offset == best_offset) {
				header->last_block_offset = best_offset + total_size;
			}
		} else {
			zend_opcache_static_cache_update_following_prev_size_locked(header, best_offset, block);
		}
		zend_opcache_static_cache_block_mark_used(block);
		if (source != NULL) {
			memcpy(zend_opcache_static_cache_ptr(best_offset + sizeof(zend_opcache_static_cache_block)), source, size);
		}

		return best_offset + (uint32_t) sizeof(zend_opcache_static_cache_block);
	}

	if (header->next_free > header->data_size || total_size > header->data_size - header->next_free) {
		return 0;
	}

	block_offset = header->data_offset + header->next_free;
	block = zend_opcache_static_cache_block_ptr(block_offset);
	block->size = total_size;
	block->prev_size = header->last_block_offset != 0 ? zend_opcache_static_cache_block_ptr(header->last_block_offset)->size : 0;
	block->next_free = 0;
	block->prev_free = 0;
	block->flags = 0;
	if (source != NULL) {
		memcpy(zend_opcache_static_cache_ptr(block_offset + sizeof(zend_opcache_static_cache_block)), source, size);
	}
	header->next_free += total_size;
	header->last_block_offset = block_offset;

	return block_offset + (uint32_t) sizeof(zend_opcache_static_cache_block);
}

bool zend_opcache_static_cache_compact_to_fit_locked(size_t size)
{
	zend_opcache_static_cache_header *header = zend_opcache_static_cache_header_ptr();
	zend_opcache_static_cache_block *block, *free_block;
	uint32_t required_block_size, used_end, offset, next_offset, block_size, write_offset,
			previous_block_size = 0, last_block_offset = 0, free_size;
	bool moved = false, movable;

	if (!header || !zend_opcache_static_cache_header_init_locked()) {
		return false;
	}

	if (!zend_opcache_static_cache_payload_size_to_block_size(size, &required_block_size) ||
		required_block_size > header->data_size ||
		!zend_opcache_static_cache_compaction_can_fit_locked(header, required_block_size)
	) {
		return false;
	}

	used_end = header->data_offset + header->next_free;
	offset = header->data_offset;
	write_offset = header->data_offset;
	header->free_list = 0;

	while (offset < used_end) {
		block = zend_opcache_static_cache_block_ptr(offset);
		block_size = block->size;
		if (block_size < ZEND_ALIGNED_SIZE(sizeof(zend_opcache_static_cache_block) + 1) ||
			block_size > used_end - offset
		) {
			return false;
		}

		next_offset = offset + block_size;
		if (zend_opcache_static_cache_block_is_free(block)) {
			offset = next_offset;
			continue;
		}

		movable = zend_opcache_static_cache_block_is_movable_locked(header, offset, block_size);
		if (!movable) {
			if (write_offset < offset) {
				free_block = zend_opcache_static_cache_block_ptr(write_offset);
				free_size = offset - write_offset;

				free_block->size = free_size;
				free_block->prev_size = previous_block_size;
				free_block->next_free = 0;
				free_block->prev_free = 0;
				free_block->flags = ZEND_OPCACHE_STATIC_CACHE_BLOCK_FREE;
				zend_opcache_static_cache_free_list_insert_locked(header, write_offset);
				previous_block_size = free_size;
				last_block_offset = write_offset;
			}

			block->prev_size = previous_block_size;
			block->next_free = 0;
			block->prev_free = 0;
			zend_opcache_static_cache_block_mark_used(block);
			previous_block_size = block_size;
			last_block_offset = offset;
			write_offset = next_offset;
			offset = next_offset;
			continue;
		}

		if (write_offset != offset) {
			memmove(zend_opcache_static_cache_ptr(write_offset), block, block_size);
			zend_opcache_static_cache_update_moved_block_entries_locked(header, offset, write_offset, block_size);
			block = zend_opcache_static_cache_block_ptr(write_offset);
			moved = true;
		}

		block->prev_size = previous_block_size;
		block->next_free = 0;
		block->prev_free = 0;
		zend_opcache_static_cache_block_mark_used(block);
		previous_block_size = block_size;
		last_block_offset = write_offset;
		write_offset += block_size;
		offset = next_offset;
	}

	header->next_free = write_offset - header->data_offset;
	header->last_block_offset = last_block_offset;

	if (moved) {
		zend_opcache_static_cache_bump_mutation_epoch_locked(header);
	}

	return moved;
}

bool zend_opcache_static_cache_startup_storage_before_request(void)
{
	zend_opcache_static_cache_storage *storage = &zend_opcache_static_cache_active_context()->storage;

	if (zend_opcache_static_cache_force_startup_failure()) {
		zend_opcache_static_cache_set_unavailable("Unable to initialize shared memory backend", true);

		return false;
	}

	if (!zend_opcache_static_cache_startup_storage()) {
		zend_opcache_static_cache_set_unavailable("Unable to initialize shared memory backend", true);

		return false;
	}

	storage->initialized_before_request = true;

	return true;
}

void zend_opcache_static_cache_shutdown_storage(void)
{
	zend_opcache_static_cache_storage *storage = &zend_opcache_static_cache_active_context()->storage;

	zend_opcache_static_cache_lock_shutdown();
	zend_opcache_static_cache_cleanup_segments(
		storage->handler,
		storage->segments,
		storage->segment_count
	);
	zend_opcache_static_cache_reset_storage();
}

void zend_opcache_static_cache_ensure_ready(void)
{
	zend_opcache_static_cache_context *context = zend_opcache_static_cache_active_context();
	zend_opcache_static_cache_runtime *runtime;
	zend_opcache_static_cache_storage *storage = &context->storage;

	zend_opcache_static_cache_reset_runtime();
	runtime = zend_opcache_static_cache_context_runtime(context);
	if (!runtime->enabled) {
		return;
	}

	if (!ZCG(enabled)) {
		zend_opcache_static_cache_set_unavailable("OPcache is disabled", false);

		return;
	}

	if (!accel_startup_ok) {
		zend_opcache_static_cache_set_unavailable("OPcache startup failed", true);

		return;
	}

	if (file_cache_only) {
		zend_opcache_static_cache_set_unavailable("Cache is unavailable in file_cache_only mode", false);

		return;
	}

	if (!storage->initialized &&
		zend_opcache_static_cache_requires_pre_request_storage()
	) {
		zend_opcache_static_cache_set_unavailable("Shared memory backend was not initialized before FPM worker startup", true);

		return;
	}

	if (!zend_opcache_static_cache_startup_storage()) {
		zend_opcache_static_cache_set_unavailable("Unable to initialize shared memory backend", true);

		return;
	}

	if (zend_opcache_static_cache_requires_pre_request_storage() &&
		!storage->initialized_before_request
	) {
		zend_opcache_static_cache_set_unavailable("Shared memory backend was initialized after FPM worker startup", true);

		return;
	}

	zend_opcache_static_cache_set_available();
}

void zend_opcache_static_cache_populate_array(zval *return_value)
{
	zend_opcache_static_cache_context *context = zend_opcache_static_cache_active_context();
	zend_opcache_static_cache_runtime *runtime = zend_opcache_static_cache_context_runtime(context);
	zend_opcache_static_cache_storage *storage = &context->storage;
	zend_opcache_static_cache_header *header;
	zend_long entry_count = 0;

	array_init(return_value);
	if (runtime->available && zend_opcache_static_cache_rlock()) {
		header = zend_opcache_static_cache_header_ptr();

		if (zend_opcache_static_cache_header_is_initialized_locked()) {
			entry_count = header->count;
		}

		zend_opcache_static_cache_unlock();
	}

	add_assoc_bool(return_value, "enabled", runtime->enabled);
	add_assoc_bool(return_value, "available", runtime->available);
	add_assoc_bool(return_value, "startup_failed", runtime->startup_failed);
	add_assoc_bool(return_value, "backend_initialized", runtime->backend_initialized);
	add_assoc_long(return_value, "configured_memory", (zend_long) runtime->configured_memory);
	add_assoc_long(return_value, "shared_memory", (zend_long) storage->size);
	add_assoc_long(return_value, "entry_count", entry_count);
	add_assoc_long(return_value, "segment_count", storage->segment_count);
	add_assoc_string(return_value, "shared_model", storage->model ? (char *) storage->model : "");

	if (runtime->failure_reason) {
		add_assoc_string(return_value, "failure_reason", (char *) runtime->failure_reason);
	}
}

bool zend_opcache_static_cache_rlock(void)
{
	return zend_opcache_static_cache_rlock_impl();
}

bool zend_opcache_static_cache_wlock(void)
{
	return zend_opcache_static_cache_wlock_impl();
}

void zend_opcache_static_cache_unlock(void)
{
	zend_opcache_static_cache_unlock_impl();
}

bool zend_opcache_static_cache_acquire_entry_lock(zend_string *key)
{
	zend_opcache_static_cache_context *context = zend_opcache_static_cache_active_context();
	zend_opcache_static_cache_entry_lock *lock;
	HashTable **locks_ptr = zend_opcache_static_cache_entry_locks_ptr_for_context(context);
	uint32_t stripe = zend_opcache_static_cache_entry_lock_stripe(key);

	zend_opcache_static_cache_ensure_entry_lock_process();

	if (*locks_ptr != NULL && zend_hash_exists(*locks_ptr, key)) {
		return true;
	}

#ifndef ZEND_WIN32
	if (!zend_opcache_static_cache_lock_entry_stripe(context, stripe)) {
		zend_throw_exception_ex(zend_opcache_static_cache_exception_ce, 0, "Unable to acquire the %s entry lock", context->name);

		return false;
	}
#else
	zend_throw_exception_ex(zend_opcache_static_cache_exception_ce, 0, "Unable to acquire the %s entry lock", context->name);

	return false;
#endif

	lock = emalloc(sizeof(zend_opcache_static_cache_entry_lock));
	lock->context = context;
	lock->owner_pid =
#ifndef ZEND_WIN32
		(zend_ulong) getpid();
#else
		0;
#endif
	lock->stripe = stripe;

	if (zend_hash_add_ptr(zend_opcache_static_cache_entry_locks(context), key, lock) == NULL) {
#ifndef ZEND_WIN32
		zend_opcache_static_cache_unlock_entry_stripe(context, stripe);
#endif
		efree(lock);

		return true;
	}

	return true;
}

bool zend_opcache_static_cache_try_acquire_entry_lock(zend_string *key)
{
	zend_opcache_static_cache_context *context = zend_opcache_static_cache_active_context();
	zend_opcache_static_cache_entry_lock *lock;
	HashTable **locks_ptr = zend_opcache_static_cache_entry_locks_ptr_for_context(context);
	uint32_t stripe = zend_opcache_static_cache_entry_lock_stripe(key);

	zend_opcache_static_cache_ensure_entry_lock_process();

	if (*locks_ptr != NULL && zend_hash_exists(*locks_ptr, key)) {
		return true;
	}

#ifndef ZEND_WIN32
	if (!zend_opcache_static_cache_try_lock_entry_stripe(context, stripe)) {
		return false;
	}
#else
	return false;
#endif

	lock = emalloc(sizeof(zend_opcache_static_cache_entry_lock));
	lock->context = context;
	lock->owner_pid =
#ifndef ZEND_WIN32
		(zend_ulong) getpid();
#else
		0;
#endif
	lock->stripe = stripe;

	if (zend_hash_add_ptr(zend_opcache_static_cache_entry_locks(context), key, lock) == NULL) {
#ifndef ZEND_WIN32
		zend_opcache_static_cache_unlock_entry_stripe(context, stripe);
#endif
		efree(lock);

		return true;
	}

	return true;
}

bool zend_opcache_static_cache_has_entry_lock(zend_string *key)
{
	HashTable **locks_ptr = zend_opcache_static_cache_entry_locks_ptr_for_context(zend_opcache_static_cache_active_context());

	zend_opcache_static_cache_ensure_entry_lock_process();

	return *locks_ptr != NULL && zend_hash_exists(*locks_ptr, key);
}

void zend_opcache_static_cache_release_entry_lock(zend_string *key)
{
	HashTable **locks_ptr = zend_opcache_static_cache_entry_locks_ptr_for_context(zend_opcache_static_cache_active_context());

	zend_opcache_static_cache_ensure_entry_lock_process();

	if (*locks_ptr == NULL) {
		return;
	}

	zend_hash_del(*locks_ptr, key);
}

bool zend_opcache_static_cache_has_all_entry_locks(void)
{
	uint32_t stripe, *counts = zend_opcache_static_cache_entry_lock_counts_for_context(zend_opcache_static_cache_active_context());

	zend_opcache_static_cache_ensure_entry_lock_process();

	for (stripe = 0; stripe < ZEND_OPCACHE_STATIC_CACHE_ENTRY_LOCK_STRIPES; stripe++) {
		if (counts[stripe] == 0) {
			return false;
		}
	}

	return true;
}

void zend_opcache_static_cache_release_active_entry_locks(void)
{
	zend_opcache_static_cache_context *context = zend_opcache_static_cache_active_context();

	zend_opcache_static_cache_release_entry_lock_context(
		zend_opcache_static_cache_entry_locks_ptr_for_context(context),
		zend_opcache_static_cache_entry_lock_counts_for_context(context));
}

void zend_opcache_static_cache_release_request_entry_locks(void)
{
	zend_opcache_static_cache_release_entry_lock_context(
		&zend_opcache_static_cache_volatile_entry_locks,
		zend_opcache_static_cache_volatile_entry_lock_counts
	);
	zend_opcache_static_cache_release_entry_lock_context(
		&zend_opcache_static_cache_persistent_entry_locks,
		zend_opcache_static_cache_persistent_entry_lock_counts
	);
#if !defined(ZEND_WIN32) && defined(ZTS)
	if (zend_opcache_static_cache_entry_locks_process_is_fork_child) {
		zend_opcache_static_cache_entry_locks_shutdown(&zend_opcache_static_cache_volatile_context_state.storage);
		zend_opcache_static_cache_entry_locks_shutdown(&zend_opcache_static_cache_persistent_context_state.storage);
	}
#endif
#ifndef ZEND_WIN32
	zend_opcache_static_cache_entry_lock_owner_pid = 0;
#endif
}
