/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Author: Go Kudo <zeriyoshi@php.net>.                                 |
   +----------------------------------------------------------------------+
*/

#include "user_cache_internal.h"
#include "user_cache_storage_portability.h"

#include "ext/hash/php_hash.h"
#include "ext/hash/php_hash_sha.h"

typedef enum {
	PHP_USER_CACHE_ENTRY_LOCK_RELEASE_DROP,
	PHP_USER_CACHE_ENTRY_LOCK_RELEASE_CLEAR,
	PHP_USER_CACHE_ENTRY_LOCK_RELEASE_PRESERVE_LEASES
} php_user_cache_entry_lock_release_mode;

typedef struct {
	char *key;
	uint32_t key_len;
	zend_ulong hash;
	uint64_t owner_pid;
	uint64_t owner_start_time;
	uint64_t expires_at;
} php_user_cache_recovered_entry_lock;

typedef struct {
	php_user_cache_context *context;
	uint64_t owner_pid;
	zend_long lease;
	bool preserve_lease;
} php_user_cache_entry_lock;

typedef struct {
	zend_string *key;
	php_user_cache_entry_lock *lock;
} php_user_cache_entry_lock_release_pair;

static const uint32_t php_user_cache_capacity_primes[] = {
	127U,
	251U,
	509U,
	1021U,
	2039U,
	4093U,
	8191U,
	16381U,
	32749U,
	65521U,
	131071U,
	262139U,
	524287U,
	1048573U,
	2097143U,
	4194301U,
	8388593U,
	16777213U,
};

#ifdef PHP_USER_CACHE_HAVE_OPTIMISTIC
static uint64_t php_user_cache_reader_incarnation = 0;
#endif

static php_user_cache_startup_lock php_user_cache_startup_storage_lock_state =
	PHP_USER_CACHE_STARTUP_LOCK_INITIALIZER
;

static inline uint64_t user_cache_process_start_time_token(uint64_t pid);

static zend_always_inline bool user_cache_requires_pre_request_storage(void)
{
	if (sapi_module.name == NULL) {
		return false;
	}

	return strcmp(sapi_module.name, "fpm-fcgi") == 0 ||
		strcmp(sapi_module.name, "apache2handler") == 0 ||
		strcmp(sapi_module.name, "cli-server") == 0
	;
}

static zend_always_inline bool user_cache_is_opted_in(void)
{
	return php_user_cache_runtime_opted_in;
}

static zend_always_inline bool user_cache_is_disabled_for_sapi(void)
{
	if (!UC_G(enable)) {
		return true;
	}

	return !UC_G(enable_cli) &&
		sapi_module.name != NULL &&
		(strcmp(sapi_module.name, "cli") == 0 || strcmp(sapi_module.name, "phpdbg") == 0)
	;
}

static zend_always_inline void user_cache_set_unavailable(php_user_cache_reason reason)
{
	php_user_cache_runtime *runtime = php_user_cache_active_runtime();

	runtime->available = false;
	runtime->failure_reason = reason;
}

static zend_always_inline void user_cache_set_available(void)
{
	php_user_cache_runtime *runtime = php_user_cache_active_runtime();

	runtime->available = true;
	runtime->failure_reason = PHP_USER_CACHE_REASON_NONE;
}

static zend_always_inline HashTable **user_cache_entry_lock_table_ptr(void)
{
	return &UC_G(entry_lock_table);
}

static zend_always_inline uint32_t user_cache_entry_lock_table_index(zend_ulong hash)
{
	return (uint32_t) (hash % PHP_USER_CACHE_ENTRY_LOCK_TABLE_SIZE);
}

static zend_always_inline uint32_t user_cache_used_end_offset_locked(const php_user_cache_header *header)
{
	return header->data_offset + header->next_free;
}

static zend_always_inline void user_cache_block_mark_free(php_user_cache_block *block)
{
	block->flags |= PHP_USER_CACHE_BLOCK_FREE;
}

static zend_always_inline void user_cache_block_mark_used(php_user_cache_block *block)
{
	block->flags &= ~PHP_USER_CACHE_BLOCK_FREE;
	block->next_free = 0;
	block->prev_free = 0;
}

static zend_always_inline bool user_cache_entry_lock_record_key_matches(
		const php_user_cache_entry_lock_record *record,
		zend_string *key,
		zend_ulong hash)
{
	return record->state == PHP_USER_CACHE_ENTRY_LOCK_USED &&
		record->hash == hash &&
		record->key_len == ZSTR_LEN(key) &&
		memcmp(php_user_cache_ptr(record->key_offset), ZSTR_VAL(key), ZSTR_LEN(key)) == 0
	;
}

static zend_always_inline php_user_cache_entry_lock *user_cache_find_local_entry_lock(
		const php_user_cache_context *ctx,
		zend_string *key)
{
	HashTable **locks_ptr = user_cache_entry_lock_table_ptr();
	php_user_cache_entry_lock *lock;

	if (*locks_ptr == NULL) {
		return NULL;
	}

	lock = zend_hash_find_ptr(*locks_ptr, key);
	if (lock == NULL || lock->context != ctx) {
		return NULL;
	}

	return lock;
}

/* Do not cache failures or values inherited across fork. */
static zend_always_inline uint64_t user_cache_cached_self_start_time_token(uint64_t self_pid)
{
	if (UC_G(self_start_time_pid) != self_pid || UC_G(self_start_time_token) == 0) {
		UC_G(self_start_time_pid) = self_pid;
		UC_G(self_start_time_token) = user_cache_process_start_time_token(self_pid);
	}

	return UC_G(self_start_time_token);
}

#if defined(ZTS) && defined(PHP_USER_CACHE_HAVE_OPTIMISTIC)
static zend_always_inline bool user_cache_storage_holds_header(
		const php_user_cache_storage *storage,
		const php_user_cache_header *header)
{
	return storage->initialized &&
		storage->segment_count == 1 &&
		storage->segments[0]->p == (const void *) header
	;
}
#endif

#ifdef PHP_USER_CACHE_HAVE_OPTIMISTIC
static zend_always_inline void user_cache_atomic_store_64(uint64_t *target, uint64_t value)
{
	PHP_USER_CACHE_ATOMIC_STORE_64(target, value);
}

static zend_always_inline bool user_cache_atomic_cas_64(uint64_t *target, uint64_t expected, uint64_t desired)
{
	return PHP_USER_CACHE_ATOMIC_CAS_64(target, expected, desired);
}

static zend_always_inline uint32_t user_cache_atomic_load_32(const uint32_t *target)
{
	return PHP_USER_CACHE_ATOMIC_LOAD_32(target);
}

static zend_always_inline void user_cache_atomic_store_32(uint32_t *target, uint32_t value)
{
	PHP_USER_CACHE_ATOMIC_STORE_32(target, value);
}

static zend_always_inline bool user_cache_atomic_cas_32(uint32_t *target, uint32_t expected, uint32_t desired)
{
	return PHP_USER_CACHE_ATOMIC_CAS_32(target, expected, desired);
}

static zend_always_inline void user_cache_atomic_fence_seq_cst(void)
{
	PHP_USER_CACHE_ATOMIC_FENCE_SEQ_CST();
}
#endif

/* Announce fences so the odd (write-in-progress) sequence is visible before the
 * payload writes; publish needs no fence because readers re-check the sequence
 * after copying. */
static zend_always_inline void user_cache_seq_announce(uint64_t *seq, uint64_t value)
{
#ifdef PHP_USER_CACHE_HAVE_OPTIMISTIC
	user_cache_atomic_store_64(seq, value);
	user_cache_atomic_fence_seq_cst();
#else
	*seq = value;
#endif
}

static zend_always_inline void user_cache_seq_publish(uint64_t *seq, uint64_t value)
{
#ifdef PHP_USER_CACHE_HAVE_OPTIMISTIC
	user_cache_atomic_store_64(seq, value);
#else
	*seq = value;
#endif
}

#ifdef PHP_USER_CACHE_HAVE_SHARED_MUTEX
static zend_always_inline bool user_cache_header_is_initialized_acquire(void)
{
	php_user_cache_header *header = php_user_cache_header_ptr();

	if (header == NULL) {
		return false;
	}

#ifdef PHP_USER_CACHE_HAVE_OPTIMISTIC
	if (user_cache_atomic_load_32(&header->magic) != PHP_USER_CACHE_MAGIC) {
		return false;
	}

	php_user_cache_atomic_fence_acquire();

	return user_cache_atomic_load_32(&header->version) == PHP_USER_CACHE_VERSION;
#else
	return php_user_cache_header_is_initialized_locked();
#endif
}
#endif

static inline bool user_cache_zts_lock_init(php_user_cache_storage *storage)
{
#ifdef ZTS
	storage->zts_lock = tsrm_mutex_alloc();

	return storage->zts_lock != NULL;
#else
	(void) storage;

	return true;
#endif
}

static inline void user_cache_zts_lock_destroy(php_user_cache_storage *storage)
{
#ifdef ZTS
	tsrm_mutex_free(storage->zts_lock);
	storage->zts_lock = NULL;
#else
	(void) storage;
#endif
}

static inline bool user_cache_zts_lock(php_user_cache_storage *storage)
{
#ifdef ZTS
	return tsrm_mutex_lock(storage->zts_lock) == 0;
#else
	(void) storage;

	return true;
#endif
}

static inline void user_cache_zts_unlock(php_user_cache_storage *storage)
{
#ifdef ZTS
	tsrm_mutex_unlock(storage->zts_lock);
#else
	(void) storage;
#endif
}

static inline void user_cache_startup_lock_acquire(php_user_cache_startup_lock *lock)
{
#ifdef ZTS
# ifdef ZEND_WIN32
	AcquireSRWLockExclusive(lock);
# else
	pthread_mutex_lock(lock);
# endif
#else
	(void) lock;
#endif
}

static inline void user_cache_startup_lock_release(php_user_cache_startup_lock *lock)
{
#ifdef ZTS
# ifdef ZEND_WIN32
	ReleaseSRWLockExclusive(lock);
# else
	pthread_mutex_unlock(lock);
# endif
#else
	(void) lock;
#endif
}

static inline uint32_t user_cache_sleep_entry_lock_retry_interval(void)
{
#ifdef ZEND_WIN32
	Sleep(PHP_USER_CACHE_ENTRY_LOCK_RETRY_INTERVAL_US / 1000);
#elif defined(HAVE_UNISTD_H)
	usleep(PHP_USER_CACHE_ENTRY_LOCK_RETRY_INTERVAL_US);
#endif

	return PHP_USER_CACHE_ENTRY_LOCK_RETRY_INTERVAL_US;
}

static inline uint64_t user_cache_process_start_time_token(uint64_t pid)
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
	const char *p;
	ssize_t stat_len;
	int fd, field;
	unsigned long long start_time = 0;
	char path[64], stat_buf[1024];

	snprintf(path, sizeof(path), "/proc/%llu/stat", (unsigned long long) pid);
	fd = open(path, O_RDONLY);
	if (fd < 0) {
		return 0;
	}

	stat_len = read(fd, stat_buf, sizeof(stat_buf) - 1);
	close(fd);
	if (stat_len <= 0) {
		return 0;
	}

	stat_buf[stat_len] = '\0';

	/* State (field 3) follows comm's closing parenthesis; skip 19 fields
	 * from there to reach starttime (field 22). */
	p = strrchr(stat_buf, ')');
	if (p == NULL) {
		return 0;
	}

	p++;
	for (field = 0; field < 19 && *p != '\0'; field++) {
		while (*p == ' ') {
			p++;
		}
		while (*p != '\0' && *p != ' ') {
			p++;
		}
	}

	if (sscanf(p, " %llu", &start_time) != 1) {
		return 0;
	}

	return (uint64_t) start_time;
#elif defined(__APPLE__) || defined(__FreeBSD__)
	struct kinfo_proc info;
	size_t size = sizeof(info);
	int mib[4] = { CTL_KERN, KERN_PROC, KERN_PROC_PID, (int) pid };

	if (sysctl(mib, 4, &info, &size, NULL, 0) != 0 || size < sizeof(info)) {
		return 0;
	}

	/* Only stable, non-zero equality is required. */
# ifdef __APPLE__
	return (((uint64_t) info.kp_proc.p_starttime.tv_sec << 20)
		| (uint64_t) info.kp_proc.p_starttime.tv_usec) + 1
	;
# else
	return (((uint64_t) info.ki_start.tv_sec << 20)
		| (uint64_t) info.ki_start.tv_usec) + 1
	;
# endif
#else
	(void) pid;

	return 0;
#endif
}

static inline bool user_cache_process_has_exited(uint64_t pid)
{
#ifdef ZEND_WIN32
	HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, (DWORD) pid);
	DWORD exit_code = 0;

	if (process == NULL) {
		return GetLastError() == ERROR_INVALID_PARAMETER;
	}

	if (GetExitCodeProcess(process, &exit_code) && exit_code != STILL_ACTIVE) {
		CloseHandle(process);

		return true;
	}

	CloseHandle(process);

	return false;
#else
	return kill((pid_t) pid, 0) == -1 && errno == ESRCH;
#endif
}

static inline bool user_cache_startup_storage_is_complete(void)
{
#if defined(ZTS) && defined(PHP_USER_CACHE_HAVE_OPTIMISTIC)
	return user_cache_atomic_load_32(&php_user_cache_active_context()->storage.startup_complete) != 0;
#else
	return false;
#endif
}

static inline void user_cache_startup_storage_lock(void)
{
	user_cache_startup_lock_acquire(&php_user_cache_startup_storage_lock_state);
}

static inline void user_cache_startup_storage_unlock(void)
{
	user_cache_startup_lock_release(&php_user_cache_startup_storage_lock_state);
}

#ifdef ZEND_WIN32
static inline void user_cache_win32_set_segment(
	php_user_cache_win32_segment *segment,
	HANDLE memfile,
	void *mapping_base,
	size_t mapping_size,
	size_t requested_size
)
{
	segment->memfile = memfile;
	segment->mapping_base = mapping_base;
	segment->mapping_size = mapping_size;
	segment->segment.p = mapping_base;
	segment->segment.pos = 0;
	segment->segment.size = requested_size;
}
#endif

#if defined(PHP_USER_CACHE_HAVE_ANON_MMAP) || defined(PHP_USER_CACHE_HAVE_BOUNDARY_SHM)
static int user_cache_wrap_mapped_segment(
	void *mapping,
	size_t requested_size,
	php_user_cache_shm_segment ***shared_segments_p,
	int *shared_segments_count,
	const char **error_in
)
{
	php_user_cache_shm_segment *segment;

	*shared_segments_count = 1;
	*shared_segments_p = (php_user_cache_shm_segment **) calloc(1, sizeof(php_user_cache_shm_segment *) + sizeof(php_user_cache_shm_segment));
	if (*shared_segments_p == NULL) {
		munmap(mapping, requested_size);
		*error_in = "calloc";

		return PHP_USER_CACHE_ALLOC_FAILURE;
	}

	segment = (php_user_cache_shm_segment *) ((char *) *shared_segments_p + sizeof(php_user_cache_shm_segment *));
	(*shared_segments_p)[0] = segment;

	segment->p = mapping;
	segment->pos = 0;
	segment->size = requested_size;
	segment->end = requested_size;

	return PHP_USER_CACHE_ALLOC_SUCCESS;
}

static int user_cache_munmap_detach_segment(php_user_cache_shm_segment *shared_segment)
{
	munmap(shared_segment->p, shared_segment->size);

	return 0;
}

static size_t user_cache_mmap_segment_type_size(void)
{
	return sizeof(php_user_cache_shm_segment);
}
#endif

#ifdef PHP_USER_CACHE_HAVE_ANON_MMAP
static int user_cache_mmap_create_segments(
	size_t requested_size,
	php_user_cache_shm_segment ***shared_segments_p,
	int *shared_segments_count,
	const char **error_in
)
{
	void *mapping;

	mapping = mmap(NULL, requested_size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (mapping == MAP_FAILED) {
		*error_in = "mmap";

		return PHP_USER_CACHE_ALLOC_FAILURE;
	}

	return user_cache_wrap_mapped_segment(mapping, requested_size, shared_segments_p, shared_segments_count, error_in);
}
#endif

#ifdef PHP_USER_CACHE_HAVE_BOUNDARY_SHM
static void user_cache_shared_boundary_digest(
	const php_user_cache_context *ctx,
	size_t requested_size,
	uint8_t digest[32]
)
{
	const char *identity;
	PHP_SHA256_CTX sha_ctx;
	size_t identity_len, update_len = 0;
	int prefix_len;
	char prefix[64];

	identity = ctx->boundary_identity != NULL
		? ctx->boundary_identity
		: (ctx->lock_name != NULL ? ctx->lock_name : "")
	;
	identity_len = ctx->boundary_identity != NULL
		? ctx->boundary_identity_len
		: strlen(identity)
	;
	prefix_len = snprintf(
		prefix,
		sizeof(prefix),
		"PhpUserCache.boundary-shm|%u|%zu|%zu|",
		PHP_USER_CACHE_VERSION,
		requested_size,
		identity_len
	);

	PHP_SHA256Init(&sha_ctx);
	if (prefix_len > 0) {
		update_len = (size_t) prefix_len;
		if (update_len >= sizeof(prefix)) {
			update_len = sizeof(prefix) - 1;
		}

		PHP_SHA256Update(&sha_ctx, (const uint8_t *) prefix, update_len);
	}

	PHP_SHA256Update(&sha_ctx, (const uint8_t *) identity, identity_len);
	PHP_SHA256Final(digest, &sha_ctx);
}

static void user_cache_shared_boundary_digest_to_identity(
	const uint8_t digest[32],
	uint64_t *identity_high,
	uint32_t *identity_low
)
{
	*identity_high =
		((uint64_t) digest[0] << 56) |
		((uint64_t) digest[1] << 48) |
		((uint64_t) digest[2] << 40) |
		((uint64_t) digest[3] << 32) |
		((uint64_t) digest[4] << 24) |
		((uint64_t) digest[5] << 16) |
		((uint64_t) digest[6] << 8) |
		(uint64_t) digest[7]
	;
	*identity_low =
		((uint32_t) digest[8] << 24) |
		((uint32_t) digest[9] << 16) |
		((uint32_t) digest[10] << 8) |
		(uint32_t) digest[11]
	;
}

static void user_cache_shared_boundary_identity(
	const php_user_cache_context *ctx,
	size_t requested_size,
	uint64_t *identity_high,
	uint32_t *identity_low
)
{
	uint8_t digest[32];

	user_cache_shared_boundary_digest(ctx, requested_size, digest);
	user_cache_shared_boundary_digest_to_identity(digest, identity_high, identity_low);
}

static void user_cache_shared_boundary_shm_name(char *buf, size_t buf_size, size_t requested_size)
{
	uint64_t identity_high;
	uint32_t identity_low;

	user_cache_shared_boundary_identity(
		php_user_cache_active_context(),
		requested_size,
		&identity_high,
		&identity_low
	);

	snprintf(buf, buf_size, "/ZUC.%016" PRIx64 "%08" PRIx32, identity_high, identity_low);
}

static void user_cache_shared_boundary_lock_name(char *buf, size_t buf_size, size_t requested_size)
{
	uint64_t identity_high;
	uint32_t identity_low;

	user_cache_shared_boundary_identity(
		php_user_cache_active_context(),
		requested_size,
		&identity_high,
		&identity_low
	);

	snprintf(
		buf,
		buf_size,
		"%s/.ZendUserCacheBnd.%016" PRIx64 "%08" PRIx32 ".lock",
		UC_G(lockfile_path),
		identity_high,
		identity_low
	);
}

static bool user_cache_shared_boundary_fd_is_trusted(int fd, struct stat *st)
{
	if (fstat(fd, st) != 0) {
		return false;
	}

	return st->st_uid == geteuid() && (st->st_mode & 0077) == 0;
}

/* Recreate a zero-length shm object left by a failed creator. */
static int user_cache_shared_boundary_open_shm_fd(
	const char *shm_name,
	size_t requested_size,
	const char **error_in
)
{
	struct stat st;
	uint32_t attempts;
	int fd, create_attempts;

	for (create_attempts = 0; create_attempts < 2; create_attempts++) {
		fd = shm_open(shm_name, O_RDWR | O_CREAT | O_EXCL, 0600);
		if (fd >= 0) {
			if (ftruncate(fd, (off_t) requested_size) != 0) {
				close(fd);
				shm_unlink(shm_name);
				*error_in = "ftruncate";

				return -1;
			}

			return fd;
		}

		if (errno != EEXIST) {
			*error_in = "shm_open";

			return -1;
		}

		fd = shm_open(shm_name, O_RDWR, 0600);
		if (fd < 0) {
			*error_in = "shm_open";

			return -1;
		}

		if (!user_cache_shared_boundary_fd_is_trusted(fd, &st)) {
			close(fd);
			*error_in = "shm ownership";

			return -1;
		}

		for (attempts = 0; (size_t) st.st_size != requested_size && attempts < 1000; attempts++) {
			if (st.st_size != 0) {
				break;
			}

			usleep(1000);

			if (fstat(fd, &st) != 0) {
				break;
			}
		}

		if ((size_t) st.st_size == requested_size) {
			return fd;
		}

		close(fd);

		if (st.st_size == 0 && create_attempts == 0) {
			shm_unlink(shm_name);

			continue;
		}

		*error_in = "shm size";

		return -1;
	}

	*error_in = "shm size";

	return -1;
}

static int user_cache_shared_boundary_create_segments(
	size_t requested_size,
	php_user_cache_shm_segment ***shared_segments_p,
	int *shared_segments_count,
	const char **error_in
)
{
	int fd;
	char shm_name[64];
	void *mapping;

	if (requested_size > (size_t) SSIZE_MAX) {
		*error_in = "size overflow";

		return PHP_USER_CACHE_ALLOC_FAILURE;
	}

	user_cache_shared_boundary_shm_name(shm_name, sizeof(shm_name), requested_size);

	fd = user_cache_shared_boundary_open_shm_fd(shm_name, requested_size, error_in);
	if (fd < 0) {
		return PHP_USER_CACHE_ALLOC_FAILURE;
	}

	mapping = mmap(NULL, requested_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	close(fd);
	if (mapping == MAP_FAILED) {
		*error_in = "mmap";

		return PHP_USER_CACHE_ALLOC_FAILURE;
	}

	return user_cache_wrap_mapped_segment(mapping, requested_size, shared_segments_p, shared_segments_count, error_in);
}

static const php_user_cache_shm_handler_entry *user_cache_shared_boundary_handler_entry(void)
{
	static const php_user_cache_shm_handlers handlers = {
		user_cache_shared_boundary_create_segments,
		user_cache_munmap_detach_segment,
		user_cache_mmap_segment_type_size
	};
	static const php_user_cache_shm_handler_entry entry = {
		"boundary-shm", &handlers
	};

	return &entry;
}
#endif

#ifdef ZEND_WIN32
static void user_cache_win32_format_name(char *buf, size_t buf_size, const char *name, size_t unique_id)
{
	const char *sapi_name = sapi_module.name != NULL ? sapi_module.name : "";
	php_user_cache_context *ctx = php_user_cache_active_context();

	snprintf(
		buf,
		buf_size,
		"%s@%.32s@%.20s@%.32s@%s@%zx",
		name,
		zend_system_id,
		sapi_name,
		zend_system_id,
		ctx->lock_name,
		unique_id
	);
}

static int user_cache_win32_reattach_segment(
	php_user_cache_win32_segment *segment,
	HANDLE memfile,
	size_t requested_size,
	const char **error_in
)
{
	MEMORY_BASIC_INFORMATION info;
	void *mapping_base;

	mapping_base = MapViewOfFileEx(memfile, FILE_MAP_ALL_ACCESS, 0, 0, 0, NULL);
	if (mapping_base == NULL) {
		*error_in = "MapViewOfFileEx";

		return PHP_USER_CACHE_ALLOC_FAILURE;
	}

	/* The mapped object must cover the requested range. */
	if (VirtualQuery(mapping_base, &info, sizeof(info)) == 0 ||
		info.RegionSize < requested_size
	) {
		UnmapViewOfFile(mapping_base);
		*error_in = "VirtualQuery";

		return PHP_USER_CACHE_ALLOC_FAILURE;
	}

	user_cache_win32_set_segment(segment, memfile, mapping_base, requested_size, requested_size);

	return PHP_USER_CACHE_ALLOC_SUCCESS;
}

static int user_cache_win32_create_segment(
	php_user_cache_win32_segment *segment,
	const char *mapping_name,
	size_t requested_size,
	const char **error_in
)
{
	HANDLE memfile;
	DWORD size_high, size_low;
	int result;
	void *mapping_base;

#if defined(_WIN64)
	size_high = (DWORD) (requested_size >> 32);
	size_low = (DWORD) (requested_size & 0xffffffff);
#else
	if (requested_size > UINT32_MAX) {
		*error_in = "size overflow";
		return PHP_USER_CACHE_ALLOC_FAILURE;
	}

	size_high = 0;
	size_low = (DWORD) requested_size;
#endif

	memfile = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE | SEC_COMMIT, size_high, size_low, mapping_name);
	if (memfile == NULL) {
		*error_in = "CreateFileMappingA";
		return PHP_USER_CACHE_ALLOC_FAILURE;
	}

	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		result = user_cache_win32_reattach_segment(segment, memfile, requested_size, error_in);
		if (result != PHP_USER_CACHE_ALLOC_SUCCESS) {
			CloseHandle(memfile);
		}

		return result;
	}

	mapping_base = MapViewOfFileEx(memfile, FILE_MAP_ALL_ACCESS, 0, 0, 0, NULL);
	if (mapping_base == NULL) {
		CloseHandle(memfile);
		*error_in = "MapViewOfFileEx";

		return PHP_USER_CACHE_ALLOC_FAILURE;
	}

	user_cache_win32_set_segment(segment, memfile, mapping_base, requested_size, requested_size);

	return PHP_USER_CACHE_ALLOC_SUCCESS;
}

static int user_cache_win32_create_segments(
	size_t requested_size,
	php_user_cache_shm_segment ***shared_segments_p,
	int *shared_segments_count,
	const char **error_in
)
{
	php_user_cache_win32_segment *segment;
	HANDLE mutex = NULL, memfile = NULL;
	DWORD wait_result;
	bool mutex_acquired = false;
	int result = PHP_USER_CACHE_ALLOC_FAILURE;
	char mapping_name[MAXPATHLEN], mutex_name[MAXPATHLEN];

	*shared_segments_count = 1;
	*shared_segments_p = (php_user_cache_shm_segment **) calloc(
		1,
		sizeof(php_user_cache_shm_segment *) + sizeof(php_user_cache_win32_segment)
	);
	if (*shared_segments_p == NULL) {
		*error_in = "calloc";

		return PHP_USER_CACHE_ALLOC_FAILURE;
	}

	segment = (php_user_cache_win32_segment *) ((char *) *shared_segments_p + sizeof(php_user_cache_shm_segment *));
	(*shared_segments_p)[0] = (php_user_cache_shm_segment *) segment;

	user_cache_win32_format_name(
		mapping_name,
		sizeof(mapping_name),
		PHP_USER_CACHE_WIN32_MAPPING_NAME,
		requested_size
	);
	user_cache_win32_format_name(
		mutex_name,
		sizeof(mutex_name),
		PHP_USER_CACHE_WIN32_MAPPING_MUTEX_NAME,
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
		result = user_cache_win32_reattach_segment(segment, memfile, requested_size, error_in);
		if (result != PHP_USER_CACHE_ALLOC_SUCCESS) {
			CloseHandle(memfile);
		}
	} else {
		result = user_cache_win32_create_segment(segment, mapping_name, requested_size, error_in);
	}

	if (mutex_acquired) {
		ReleaseMutex(mutex);
		mutex_acquired = false;
	}

	CloseHandle(mutex);
	mutex = NULL;

	if (result == PHP_USER_CACHE_ALLOC_SUCCESS) {
		return PHP_USER_CACHE_ALLOC_SUCCESS;
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

	return PHP_USER_CACHE_ALLOC_FAILURE;
}

static int user_cache_win32_detach_segment(php_user_cache_shm_segment *shared_segment)
{
	php_user_cache_win32_segment *segment = (php_user_cache_win32_segment *) shared_segment;

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

static size_t user_cache_win32_segment_type_size(void)
{
	return sizeof(php_user_cache_win32_segment);
}
#endif

static const php_user_cache_shm_handler_entry *user_cache_handler_table(void)
{
#ifdef PHP_USER_CACHE_HAVE_ANON_MMAP
	static const php_user_cache_shm_handlers mmap_handlers = {
		user_cache_mmap_create_segments,
		user_cache_munmap_detach_segment,
		user_cache_mmap_segment_type_size
	};
#endif
#ifdef ZEND_WIN32
	static const php_user_cache_shm_handlers win32_handlers = {
		user_cache_win32_create_segments,
		user_cache_win32_detach_segment,
		user_cache_win32_segment_type_size
	};
#endif
	static const php_user_cache_shm_handler_entry handlers[] = {
#ifdef PHP_USER_CACHE_HAVE_ANON_MMAP
		{ "mmap", &mmap_handlers },
#endif
#ifdef PHP_USER_CACHE_USE_SHM
		{ "shm", &php_user_cache_alloc_shm_handlers },
#endif
#ifdef PHP_USER_CACHE_USE_SHM_OPEN
		{ "posix", &php_user_cache_alloc_posix_handlers },
#endif
#ifdef ZEND_WIN32
		{ "win32", &win32_handlers },
#endif
		{ NULL, NULL }
	};

	return handlers;
}

static void user_cache_cleanup_segments(const php_user_cache_shm_handlers *handler, php_user_cache_shm_segment **segments, int segment_count)
{
	int i;

	if (!handler || !segments) {
		return;
	}

	for (i = 0; i < segment_count; i++) {
		if (segments[i]->p && segments[i]->p != (void *) -1) {
			handler->detach_segment(segments[i]);
		}
	}

	free(segments);
}

static bool user_cache_entry_lock_owner_is_dead_impl(uint64_t owner_pid, uint64_t owner_start_time)
{
	uint64_t current_start_time;

	if (user_cache_process_has_exited(owner_pid)) {
		return true;
	}

	if (owner_start_time != 0) {
		current_start_time = user_cache_process_start_time_token(owner_pid);
		if (current_start_time != 0 && current_start_time != owner_start_time) {
			return true;
		}
	}

	return false;
}

static bool user_cache_entry_lock_owner_is_dead(uint64_t owner_pid, uint64_t owner_start_time)
{
	uint64_t now = (uint64_t) time(NULL);

	if (owner_pid == UC_G(entry_lock_owner_probe_pid) &&
		owner_start_time == UC_G(entry_lock_owner_probe_start_time) &&
		now == UC_G(entry_lock_owner_probe_at)
	) {
		return UC_G(entry_lock_owner_probe_dead);
	}

	UC_G(entry_lock_owner_probe_dead) = user_cache_entry_lock_owner_is_dead_impl(owner_pid, owner_start_time);
	UC_G(entry_lock_owner_probe_pid) = owner_pid;
	UC_G(entry_lock_owner_probe_start_time) = owner_start_time;
	UC_G(entry_lock_owner_probe_at) = now;

	return UC_G(entry_lock_owner_probe_dead);
}

/* Key offsets in a segment recovered after owner death are untrusted; bound
 * them before reading. */
static bool user_cache_recovery_lock_key_is_readable(
		const php_user_cache_header *header,
		const php_user_cache_entry_lock_record *record)
{
	uint32_t data_end;

	if (record->key_offset == 0 || record->key_len == 0 || record->key_offset < header->data_offset) {
		return false;
	}

	if (header->data_size > UINT32_MAX - header->data_offset) {
		return false;
	}

	data_end = header->data_offset + header->data_size;
	if (record->key_offset > data_end) {
		return false;
	}

	return record->key_len <= data_end - record->key_offset;
}

/* A lock whose owner has died stays active until its lease expires, then is
 * demoted to ownerless. */
static bool user_cache_entry_lock_record_is_active_locked(
		php_user_cache_entry_lock_record *record,
		uint64_t now,
		bool demote_dead_owner)
{
	if (record->state != PHP_USER_CACHE_ENTRY_LOCK_USED) {
		return false;
	}

	if (record->expires_at != 0 && record->expires_at <= now) {
		return false;
	}

	if (record->owner_pid == 0) {
		return record->expires_at != 0;
	}

	if (user_cache_entry_lock_owner_is_dead(record->owner_pid, record->owner_start_time)) {
		if (!demote_dead_owner) {
			return false;
		}

		record->owner_pid = 0;
		record->owner_start_time = 0;

		return record->expires_at != 0;
	}

	return true;
}

static php_user_cache_recovered_entry_lock *user_cache_collect_recovery_entry_locks(
		php_user_cache_header *header,
		uint32_t *count_ptr)
{
	php_user_cache_recovered_entry_lock *locks;
	php_user_cache_entry_lock_record *record;
	uint64_t now = (uint64_t) time(NULL);
	uint32_t i, count = 0;
	char *key;

	for (i = 0; i < PHP_USER_CACHE_ENTRY_LOCK_TABLE_SIZE; i++) {
		record = &header->entry_lock_records[i];
		if (user_cache_entry_lock_record_is_active_locked(record, now, false) &&
			user_cache_recovery_lock_key_is_readable(header, record)
		) {
			count++;
		}
	}

	*count_ptr = 0;
	if (count == 0) {
		return NULL;
	}

	locks = (php_user_cache_recovered_entry_lock *) calloc(count, sizeof(*locks));
	if (locks == NULL) {
		return NULL;
	}

	for (i = 0; i < PHP_USER_CACHE_ENTRY_LOCK_TABLE_SIZE; i++) {
		record = &header->entry_lock_records[i];
		if (!user_cache_entry_lock_record_is_active_locked(record, now, false) ||
			!user_cache_recovery_lock_key_is_readable(header, record)
		) {
			continue;
		}

		key = (char *) malloc(record->key_len);
		if (key == NULL) {
			continue;
		}

		memcpy(key, php_user_cache_ptr(record->key_offset), record->key_len);

		locks[*count_ptr].key = key;
		locks[*count_ptr].key_len = record->key_len;
		locks[*count_ptr].hash = record->hash;
		locks[*count_ptr].owner_pid = record->owner_pid;
		locks[*count_ptr].owner_start_time = record->owner_start_time;
		locks[*count_ptr].expires_at = record->expires_at;

		(*count_ptr)++;
	}

	return locks;
}

static void user_cache_free_recovery_entry_locks(
		php_user_cache_recovered_entry_lock *locks,
		uint32_t count)
{
	uint32_t i;

	if (locks == NULL) {
		return;
	}

	for (i = 0; i < count; i++) {
		free(locks[i].key);
	}

	free(locks);
}

static void user_cache_restore_recovery_entry_locks(
		php_user_cache_header *header,
		php_user_cache_recovered_entry_lock *locks,
		uint32_t count)
{
	php_user_cache_entry_lock_record *record;
	uint32_t i, probe, slot_idx, key_offset;

	for (i = 0; i < count; i++) {
		if (locks[i].key == NULL) {
			continue;
		}

		for (probe = 0; probe < PHP_USER_CACHE_ENTRY_LOCK_TABLE_SIZE; probe++) {
			slot_idx = (user_cache_entry_lock_table_index(locks[i].hash) + probe) %
				PHP_USER_CACHE_ENTRY_LOCK_TABLE_SIZE
			;

			record = &header->entry_lock_records[slot_idx];
			if (record->state == PHP_USER_CACHE_ENTRY_LOCK_USED) {
				continue;
			}

			key_offset = php_user_cache_alloc_locked(locks[i].key_len, locks[i].key);
			if (key_offset == 0) {
				break;
			}

			memset(record, 0, sizeof(*record));

			record->hash = locks[i].hash;
			record->owner_pid = locks[i].owner_pid;
			record->owner_start_time = locks[i].owner_start_time;
			record->expires_at = locks[i].expires_at;
			record->key_offset = key_offset;
			record->key_len = locks[i].key_len;
			record->state = PHP_USER_CACHE_ENTRY_LOCK_USED;

			break;
		}
	}
}

#ifdef PHP_USER_CACHE_HAVE_BOUNDARY_SHM
static bool user_cache_header_boundary_identity_matches_locked(
		const php_user_cache_header *header,
		size_t requested_size)
{
	php_user_cache_context *ctx = php_user_cache_active_context();
	php_user_cache_storage *storage = &ctx->storage;

	if (!ctx->boundary_shared) {
		return header->boundary_identity_digest_set == 0;
	}

	if (header->boundary_identity_digest_set == 0) {
		return false;
	}

	/* The boundary identity is immutable for the partition lifetime. */
	if (!storage->boundary_digest_memoized) {
		user_cache_shared_boundary_digest(ctx, requested_size, storage->boundary_digest_memo);
		storage->boundary_digest_memoized = true;
	}

	return memcmp(header->boundary_identity_digest, storage->boundary_digest_memo, sizeof(storage->boundary_digest_memo)) == 0;
}
#endif

/* A segment wipe would invalidate the zero-copy views (verbatim arrays, pinned
 * payloads) that live processes still reference from optimistic reads or graph
 * pins. Recovery defers while any live owner holds one; dead owners do not
 * block, since their state is stale and their memory is unreferenced. During
 * recovery write_seq is odd, so no new optimistic reader can start and this
 * snapshot of the reader/pin state is stable. */
static bool user_cache_recovery_blocked_by_live_reference(const php_user_cache_header *header)
{
	/* Not const: zend_atomic_int_load_ex() takes a non-const pointer on
	 * Windows because the Interlocked API has no read-only form. */
	php_user_cache_graph_pin_slot *pin_slot;
#ifdef PHP_USER_CACHE_HAVE_OPTIMISTIC
	const php_user_cache_reader_slot *reader_slot;
	uint64_t owner_start_time;
#endif
	uint64_t owner_pid;
	uint32_t i;

	user_cache_atomic_fence_seq_cst();

#ifdef PHP_USER_CACHE_HAVE_OPTIMISTIC
	for (i = 0; i < PHP_USER_CACHE_READER_SLOTS; i++) {
		reader_slot = &header->reader_slots[i];
		if (user_cache_atomic_load_32(&reader_slot->active) == 0) {
			continue;
		}

		owner_pid = php_user_cache_atomic_load_64(&reader_slot->owner_pid);
		owner_start_time = php_user_cache_atomic_load_64(&reader_slot->owner_start_time);
		if (owner_pid != 0 &&
			!user_cache_entry_lock_owner_is_dead(owner_pid, owner_start_time)
		) {
			return true;
		}
	}
#endif

	for (i = 0; i < PHP_USER_CACHE_GRAPH_PIN_SLOTS; i++) {
		pin_slot = (php_user_cache_graph_pin_slot *) &header->graph_pin_slots[i];
		if (zend_atomic_int_load_ex(&pin_slot->pin_count) == 0) {
			continue;
		}

		owner_pid = (uint64_t) (uint32_t) zend_atomic_int_load_ex(&pin_slot->owner_pid);
		if (owner_pid != 0 &&
			!user_cache_entry_lock_owner_is_dead(owner_pid, pin_slot->owner_start_time)
		) {
			return true;
		}
	}

	return false;
}

/* Recover an interrupted write while holding the exclusive lock. Returns false
 * when recovery must be deferred (see user_cache_recovery_blocked_by_live_reference);
 * the caller then leaves the segment as-is and fails until the reference drains. */
static bool user_cache_recover_after_owner_death(void)
{
	php_user_cache_header *header = php_user_cache_header_ptr();
	php_user_cache_recovered_entry_lock *locks;
	php_user_cache_reader_slot *slot;
	uint64_t owner_pid, owner_start_time;
	uint32_t lock_count = 0, i;

	if (header == NULL || !php_user_cache_header_is_initialized_locked()) {
		return true;
	}

	if ((header->write_seq & 1) == 0) {
		return true;
	}

	if (user_cache_recovery_blocked_by_live_reference(header)) {
		return false;
	}

	locks = user_cache_collect_recovery_entry_locks(header, &lock_count);

	memset(
		php_user_cache_entries_ptr(header),
		0,
		(size_t) header->capacity * sizeof(php_user_cache_entry)
	);
	memset(header->entry_lock_records, 0, sizeof(header->entry_lock_records));
	memset(header->orphaned_graphs, 0, sizeof(header->orphaned_graphs));

	for (i = 0; i < PHP_USER_CACHE_READER_SLOTS; i++) {
		slot = &header->reader_slots[i];

		/* Reader slots may be claimed lock-free. */
#ifdef PHP_USER_CACHE_HAVE_OPTIMISTIC
		owner_pid = php_user_cache_atomic_load_64(&slot->owner_pid);
		owner_start_time = php_user_cache_atomic_load_64(&slot->owner_start_time);
#else
		owner_pid = slot->owner_pid;
		owner_start_time = slot->owner_start_time;
#endif

		if (owner_pid != 0 &&
			user_cache_entry_lock_owner_is_dead(owner_pid, owner_start_time)
		) {
#ifdef PHP_USER_CACHE_HAVE_OPTIMISTIC
			/* Clear start time before pid so scanners cannot combine owners. */
			user_cache_atomic_cas_32(&slot->active, 1, 0);
			user_cache_atomic_store_64(&slot->owner_start_time, 0);
			user_cache_atomic_cas_64(&slot->owner_pid, owner_pid, 0);
#else
			memset(slot, 0, sizeof(*slot));
#endif
		}
	}

	header->count = 0;
	header->tombstone_count = 0;
	header->next_free = 0;
	header->free_list = 0;
	header->last_block_offset = 0;
	header->orphaned_graphs_saturated = 0;

	user_cache_restore_recovery_entry_locks(header, locks, lock_count);
	user_cache_free_recovery_entry_locks(locks, lock_count);

	php_user_cache_bump_mutation_epoch_locked(header);

	user_cache_seq_publish(&header->write_seq, header->write_seq + 1);

	return true;
}

#ifdef PHP_USER_CACHE_HAVE_SHARED_MUTEX
static bool user_cache_shared_mutex_init(php_user_cache_header *header)
{
	pthread_mutexattr_t attr;
	bool result;

	if (pthread_mutexattr_init(&attr) != 0) {
		return false;
	}

	result = pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED) == 0 &&
		pthread_mutexattr_setrobust(&attr, PTHREAD_MUTEX_ROBUST) == 0 &&
		pthread_mutex_init(&header->global_shared_mutex.mutex, &attr) == 0
	;

	pthread_mutexattr_destroy(&attr);

	return result;
}

static bool user_cache_shared_mutex_lock(void)
{
	php_user_cache_header *header = php_user_cache_header_ptr();
	int result;

	if (header == NULL) {
		return false;
	}

	result = pthread_mutex_lock(&header->global_shared_mutex.mutex);
	if (result == EOWNERDEAD) {
		/* A failed robust-mutex recovery makes the segment unusable. */
		if (pthread_mutex_consistent(&header->global_shared_mutex.mutex) != 0) {
			php_error_docref(NULL, E_WARNING, "Cache: unable to make the shared lock consistent after owner death");
			pthread_mutex_unlock(&header->global_shared_mutex.mutex);

			return false;
		}

		if (!user_cache_recover_after_owner_death()) {
			pthread_mutex_unlock(&header->global_shared_mutex.mutex);

			return false;
		}

		result = 0;
	}

	if (result != 0) {
		return false;
	}

	UC_G(lock_held) = true;

	return true;
}
#endif

#ifndef ZEND_WIN32
static bool user_cache_lock_impl(short lock_type)
{
	php_user_cache_storage *storage = &php_user_cache_active_context()->storage;
	struct flock mem_lock;

	if (!storage->lock_initialized) {
		return false;
	}

#ifdef PHP_USER_CACHE_HAVE_SHARED_MUTEX
	if (storage->use_shared_mutex) {
		return user_cache_shared_mutex_lock();
	}
#endif

	if (!user_cache_zts_lock(storage)) {
		return false;
	}

	mem_lock.l_type = lock_type;
	mem_lock.l_whence = SEEK_SET;
	mem_lock.l_start = 0;
	mem_lock.l_len = 1;

	while (fcntl(storage->lock_file, F_SETLKW, &mem_lock) == -1) {
		if (errno != EINTR) {
			user_cache_zts_unlock(storage);

			return false;
		}
	}

	UC_G(lock_held) = true;

	return true;
}

static bool user_cache_create_lock(void)
{
	php_user_cache_context *ctx = php_user_cache_active_context();
	php_user_cache_storage *storage = &ctx->storage;
	int val;
#ifdef PHP_USER_CACHE_USE_SHM_OPEN
	struct stat lock_st;
#endif

	if (storage->lock_initialized) {
		return true;
	}

	if (!user_cache_zts_lock_init(storage)) {
		return false;
	}

#ifdef PHP_USER_CACHE_USE_SHM_OPEN
	if (ctx->boundary_shared) {
		user_cache_shared_boundary_lock_name(
			storage->lockfile_name,
			sizeof(storage->lockfile_name),
			storage->size
		);

		/* Refuse symlinks in the predictable world-writable lock path. */
		storage->lock_file = open(storage->lockfile_name, O_RDWR | O_CREAT | O_CLOEXEC | O_NOFOLLOW, 0600);
		if (storage->lock_file >= 0 &&
			!user_cache_shared_boundary_fd_is_trusted(storage->lock_file, &lock_st)
		) {
			close(storage->lock_file);
			storage->lock_file = -1;
		}

		if (storage->lock_file < 0) {
			user_cache_zts_lock_destroy(storage);

			return false;
		}

		storage->lock_initialized = true;

		return true;
	}
#endif

#if defined(__linux__) && defined(HAVE_MEMFD_CREATE) && defined(MFD_CLOEXEC)
	storage->lock_file = memfd_create(ctx->lock_name, MFD_CLOEXEC);
	if (storage->lock_file >= 0) {
		storage->lock_initialized = true;

		return true;
	}
#endif

#ifdef O_TMPFILE
	storage->lock_file = open(UC_G(lockfile_path), O_RDWR | O_TMPFILE | O_EXCL | O_CLOEXEC, 0666);
	if (storage->lock_file >= 0) {
		storage->lock_initialized = true;

		return true;
	}
#endif

	snprintf(
		storage->lockfile_name,
		sizeof(storage->lockfile_name),
		"%s/%sXXXXXX",
		UC_G(lockfile_path),
		ctx->sem_filename_prefix
	);

	storage->lock_file = mkstemp(storage->lockfile_name);
	if (storage->lock_file == -1) {
		user_cache_zts_lock_destroy(storage);

		return false;
	}

	if (fchmod(storage->lock_file, 0666) == -1) {
		close(storage->lock_file);
		storage->lock_file = -1;
		user_cache_zts_lock_destroy(storage);

		return false;
	}

	val = fcntl(storage->lock_file, F_GETFD, 0);
	val |= FD_CLOEXEC;
	fcntl(storage->lock_file, F_SETFD, val);
	unlink(storage->lockfile_name);

	storage->lock_initialized = true;

	return true;
}

static void user_cache_destroy_lock(void)
{
	php_user_cache_storage *storage = &php_user_cache_active_context()->storage;

	if (!storage->lock_initialized) {
		return;
	}

	if (storage->lock_file >= 0) {
		close(storage->lock_file);
		storage->lock_file = -1;
	}

	user_cache_zts_lock_destroy(storage);

	storage->lock_initialized = false;
}

static bool user_cache_rlock_impl(void)
{
	return user_cache_lock_impl(F_RDLCK);
}

static bool user_cache_wlock_impl(void)
{
	return user_cache_lock_impl(F_WRLCK);
}

static void user_cache_unlock_impl(void)
{
	php_user_cache_storage *storage = &php_user_cache_active_context()->storage;
	struct flock mem_unlock;

	if (!storage->lock_initialized) {
		return;
	}

#ifdef PHP_USER_CACHE_HAVE_SHARED_MUTEX
	if (storage->use_shared_mutex) {
		php_user_cache_header *header = php_user_cache_header_ptr();

		if (header != NULL) {
			pthread_mutex_unlock(&header->global_shared_mutex.mutex);
		}

		UC_G(lock_held) = false;

		return;
	}
#endif

	mem_unlock.l_type = F_UNLCK;
	mem_unlock.l_whence = SEEK_SET;
	mem_unlock.l_start = 0;
	mem_unlock.l_len = 1;
	fcntl(storage->lock_file, F_SETLK, &mem_unlock);

	user_cache_zts_unlock(storage);
	UC_G(lock_held) = false;
}
#else
static bool user_cache_win32_open_lock_file_at(php_user_cache_storage *storage, const char *dir, const char *base_name)
{
	size_t dir_len;
	const char *sep;

	if (dir == NULL || dir[0] == '\0') {
		return false;
	}

	dir_len = strlen(dir);
	sep = dir[dir_len - 1] == '/' || dir[dir_len - 1] == '\\'
		? ""
		: "/"
	;

	snprintf(
		storage->lockfile_name,
		sizeof(storage->lockfile_name),
		"%s%s%s.lock",
		dir,
		sep,
		base_name
	);

	storage->lock_file = php_win32_ioutil_open(storage->lockfile_name, O_RDWR | O_CREAT | O_BINARY, 0666);

	return storage->lock_file >= 0;
}

static bool user_cache_win32_open_lock_file(php_user_cache_context *ctx)
{
	php_user_cache_storage *storage = &ctx->storage;
	DWORD tmp_path_w_len;
	wchar_t tmp_path_w[MAXPATHLEN];
	size_t tmp_path_len;
	bool opened;
	char base_name[MAXPATHLEN], *tmp_path, *p;

	user_cache_win32_format_name(
		base_name,
		sizeof(base_name),
		PHP_USER_CACHE_WIN32_LOCK_FILE_NAME,
		storage->size
	);

	for (p = base_name; *p != '\0'; p++) {
		if ((uint8_t) *p < 32 ||
			*p == '<' ||
			*p == '>' ||
			*p == ':' ||
			*p == '"' ||
			*p == '/' ||
			*p == '\\' ||
			*p == '|' ||
			*p == '?' ||
			*p == '*'
		) {
			*p = '_';
		}
	}

	/* Preserve temporary paths outside the ANSI codepage. */
	tmp_path_w_len = GetTempPathW(MAXPATHLEN, tmp_path_w);
	if (tmp_path_w_len == 0 || tmp_path_w_len >= MAXPATHLEN) {
		return false;
	}

	tmp_path = php_win32_ioutil_conv_w_to_any(tmp_path_w, tmp_path_w_len, &tmp_path_len);
	if (tmp_path == NULL) {
		return false;
	}

	opened = user_cache_win32_open_lock_file_at(storage, tmp_path, base_name);
	free(tmp_path);

	return opened;
}

static bool user_cache_win32_lock_range(php_user_cache_storage *storage, DWORD offset, bool exclusive, bool blocking)
{
	OVERLAPPED overlapped;
	HANDLE handle;
	DWORD flags = 0;

	if (!storage->lock_initialized || storage->lock_file < 0) {
		return false;
	}

	handle = (HANDLE) _get_osfhandle(storage->lock_file);
	if (handle == INVALID_HANDLE_VALUE) {
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

	return LockFileEx(handle, flags, 0, 1, 0, &overlapped) == TRUE;
}

static void user_cache_win32_unlock_range(php_user_cache_storage *storage, DWORD offset)
{
	OVERLAPPED overlapped;
	HANDLE handle;

	if (!storage->lock_initialized || storage->lock_file < 0) {
		return;
	}

	handle = (HANDLE) _get_osfhandle(storage->lock_file);
	if (handle == INVALID_HANDLE_VALUE) {
		return;
	}

	memset(&overlapped, 0, sizeof(overlapped));
	overlapped.Offset = offset;

	UnlockFileEx(handle, 0, 1, 0, &overlapped);
}

static bool user_cache_lock_impl(bool exclusive)
{
	php_user_cache_storage *storage = &php_user_cache_active_context()->storage;

	if (!storage->lock_initialized) {
		return false;
	}

	if (!user_cache_zts_lock(storage)) {
		return false;
	}

	if (!user_cache_win32_lock_range(storage, 0, exclusive, true)) {
		user_cache_zts_unlock(storage);

		return false;
	}

	UC_G(lock_held) = true;

	return true;
}

static bool user_cache_create_lock(void)
{
	php_user_cache_context *ctx = php_user_cache_active_context();
	php_user_cache_storage *storage = &ctx->storage;

	if (storage->lock_initialized) {
		return true;
	}

	if (!user_cache_zts_lock_init(storage)) {
		return false;
	}

	if (!user_cache_win32_open_lock_file(ctx)) {
		user_cache_zts_lock_destroy(storage);

		return false;
	}

	storage->lock_initialized = true;

	return true;
}

static void user_cache_destroy_lock(void)
{
	php_user_cache_storage *storage = &php_user_cache_active_context()->storage;

	if (!storage->lock_initialized) {
		return;
	}

	if (storage->lock_file >= 0) {
		php_win32_ioutil_close(storage->lock_file);
		storage->lock_file = -1;
	}

	user_cache_zts_lock_destroy(storage);

	storage->lock_initialized = false;
}

static bool user_cache_rlock_impl(void)
{
	return user_cache_lock_impl(false);
}

static bool user_cache_wlock_impl(void)
{
	return user_cache_lock_impl(true);
}

static void user_cache_unlock_impl(void)
{
	php_user_cache_storage *storage = &php_user_cache_active_context()->storage;

	if (!storage->lock_initialized) {
		return;
	}

	user_cache_win32_unlock_range(storage, 0);

	user_cache_zts_unlock(storage);

	UC_G(lock_held) = false;
}
#endif

static uint64_t user_cache_entry_lock_expires_at(zend_long lease)
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

static void user_cache_remove_entry_lock_record_locked(
		php_user_cache_entry_lock_record *record)
{
	if (record->state == PHP_USER_CACHE_ENTRY_LOCK_USED && record->key_offset != 0) {
		php_user_cache_free_locked(record->key_offset);
	}

	memset(record, 0, sizeof(*record));

	record->state = PHP_USER_CACHE_ENTRY_LOCK_TOMBSTONE;
}

static bool user_cache_find_entry_lock_record_slot_locked(
		php_user_cache_header *header,
		zend_string *key,
		zend_ulong hash,
		uint32_t *slot_idx,
		bool *found)
{
	php_user_cache_entry_lock_record *record;
	uint64_t now = 0;
	uint32_t first_avail = UINT32_MAX, i, probe;

	*found = false;

	for (probe = 0; probe < PHP_USER_CACHE_ENTRY_LOCK_TABLE_SIZE; probe++) {
		i = (user_cache_entry_lock_table_index(hash) + probe) % PHP_USER_CACHE_ENTRY_LOCK_TABLE_SIZE;
		record = &header->entry_lock_records[i];

		if (record->state == PHP_USER_CACHE_ENTRY_LOCK_EMPTY) {
			*slot_idx = first_avail != UINT32_MAX ? first_avail : i;

			return true;
		}

		if (record->state == PHP_USER_CACHE_ENTRY_LOCK_TOMBSTONE) {
			if (first_avail == UINT32_MAX) {
				first_avail = i;
			}

			continue;
		}

		/* Avoid a clock read for an empty table. */
		if (now == 0) {
			now = (uint64_t) time(NULL);
		}

		if (!user_cache_entry_lock_record_is_active_locked(record, now, true)) {
			user_cache_remove_entry_lock_record_locked(record);

			if (first_avail == UINT32_MAX) {
				first_avail = i;
			}

			continue;
		}

		if (user_cache_entry_lock_record_key_matches(record, key, hash)) {
			*slot_idx = i;
			*found = true;

			return true;
		}
	}

	if (first_avail != UINT32_MAX) {
		*slot_idx = first_avail;

		return true;
	}

	return false;
}

static bool user_cache_insert_entry_lock_record_locked(
		php_user_cache_header *header,
		uint32_t slot_idx,
		zend_string *key,
		zend_ulong hash,
		zend_long lease)
{
	php_user_cache_entry_lock_record *record = &header->entry_lock_records[slot_idx];
	uint64_t owner_pid;
	uint32_t key_offset;

	if (ZSTR_LEN(key) > UINT32_MAX) {
		return false;
	}

	key_offset = php_user_cache_alloc_locked(ZSTR_LEN(key), ZSTR_VAL(key));
	if (key_offset == 0) {
		return false;
	}

	owner_pid = php_user_cache_cached_pid();

	memset(record, 0, sizeof(*record));

	record->hash = hash;
	record->owner_pid = owner_pid;
	record->owner_start_time = user_cache_cached_self_start_time_token(owner_pid);
	record->expires_at = lease > 0 ? user_cache_entry_lock_expires_at(lease) : 0;
	record->key_offset = key_offset;
	record->key_len = (uint32_t) ZSTR_LEN(key);
	record->state = PHP_USER_CACHE_ENTRY_LOCK_USED;

	return true;
}

static bool user_cache_update_entry_lock_record_lease_locked(
		php_user_cache_header *header,
		zend_string *key,
		zend_long lease)
{
	php_user_cache_entry_lock_record *record;
	zend_ulong hash;
	uint64_t expires_at;
	uint32_t slot_idx;
	bool found;

	if (lease <= 0) {
		return true;
	}

	hash = zend_string_hash_val(key);
	if (!user_cache_find_entry_lock_record_slot_locked(header, key, hash, &slot_idx, &found) || !found) {
		return false;
	}

	record = &header->entry_lock_records[slot_idx];
	expires_at = user_cache_entry_lock_expires_at(lease);
	if (record->expires_at < expires_at) {
		record->expires_at = expires_at;
	}

	return true;
}

static php_user_cache_entry_lock *user_cache_create_local_entry_lock(
		php_user_cache_context *ctx,
		zend_long lease,
		bool preserve_lease)
{
	php_user_cache_entry_lock *lock;

	lock = emalloc(sizeof(php_user_cache_entry_lock));
	lock->context = ctx;
	lock->owner_pid = php_user_cache_cached_pid();
	lock->lease = lease;
	lock->preserve_lease = preserve_lease;

	return lock;
}

static bool user_cache_add_local_entry_lock(
		HashTable *locks,
		zend_string *key,
		php_user_cache_entry_lock *lock)
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

static void user_cache_release_entry_lock_records_locked(
		php_user_cache_header *header,
		const php_user_cache_entry_lock_release_pair *pairs,
		uint32_t pair_count,
		php_user_cache_entry_lock_release_mode mode)
{
	php_user_cache_entry_lock *lock;
	zend_ulong hash;
	zend_string *key;
	uint32_t i, slot_idx;
	bool found;

	for (i = 0; i < pair_count; i++) {
		key = pairs[i].key;
		lock = pairs[i].lock;

		hash = zend_string_hash_val(key);
		if (!user_cache_find_entry_lock_record_slot_locked(header, key, hash, &slot_idx, &found) || !found) {
			continue;
		}

		if (header->entry_lock_records[slot_idx].owner_pid != lock->owner_pid) {
			continue;
		}

		if (mode == PHP_USER_CACHE_ENTRY_LOCK_RELEASE_PRESERVE_LEASES &&
			lock->preserve_lease &&
			lock->lease > 0
		) {
			header->entry_lock_records[slot_idx].owner_pid = 0;
			header->entry_lock_records[slot_idx].owner_start_time = 0;
			header->entry_lock_records[slot_idx].expires_at =
				user_cache_entry_lock_expires_at(lock->lease)
			;
		} else {
			user_cache_remove_entry_lock_record_locked(&header->entry_lock_records[slot_idx]);
		}
	}
}

static void user_cache_destroy_entry_locks_if_empty(HashTable **locks_ptr)
{
	if (*locks_ptr != NULL && zend_hash_num_elements(*locks_ptr) == 0) {
		zend_hash_destroy(*locks_ptr);
		FREE_HASHTABLE(*locks_ptr);
		*locks_ptr = NULL;
	}
}

static void user_cache_release_entry_locks_for_context(
		php_user_cache_context *ctx,
		HashTable **locks_ptr,
		php_user_cache_entry_lock_release_mode mode)
{
	php_user_cache_context *prev_ctx;
	php_user_cache_entry_lock *lock;
	php_user_cache_header *header;
	php_user_cache_entry_lock_release_pair *pairs;
	zend_string *key;
	uint32_t i, lock_count, pair_count = 0;

	if (*locks_ptr == NULL) {
		return;
	}

	lock_count = zend_hash_num_elements(*locks_ptr);
	if (lock_count == 0 || mode == PHP_USER_CACHE_ENTRY_LOCK_RELEASE_DROP) {
		zend_hash_destroy(*locks_ptr);
		FREE_HASHTABLE(*locks_ptr);
		*locks_ptr = NULL;

		return;
	}

	pairs = safe_emalloc(lock_count, sizeof(php_user_cache_entry_lock_release_pair), 0);

	ZEND_HASH_FOREACH_STR_KEY_PTR(*locks_ptr, key, lock) {
		if (key == NULL || lock == NULL || lock->context != ctx) {
			continue;
		}

		pairs[pair_count].key = zend_string_copy(key);
		pairs[pair_count].lock = lock;

		pair_count++;
	} ZEND_HASH_FOREACH_END();

	if (pair_count == 0) {
		efree(pairs);

		return;
	}

	prev_ctx = php_user_cache_activate_context(ctx);
	if (php_user_cache_wlock()) {
		header = php_user_cache_header_ptr();

		if (php_user_cache_header_init_locked()) {
			user_cache_release_entry_lock_records_locked(
				header,
				pairs,
				pair_count,
				mode
			);
		}

		php_user_cache_unlock();
	}

	php_user_cache_restore_context(prev_ctx);

	for (i = 0; i < pair_count; i++) {
		zend_hash_del(*locks_ptr, pairs[i].key);
		zend_string_release(pairs[i].key);
	}

	efree(pairs);

	user_cache_destroy_entry_locks_if_empty(locks_ptr);
}

static void user_cache_release_entry_locks_all_contexts(
		HashTable **locks_ptr,
		php_user_cache_entry_lock_release_mode mode)
{
	php_user_cache_context *ctx = NULL;
	php_user_cache_entry_lock *lock;

	while (*locks_ptr != NULL) {
		ctx = NULL;

		ZEND_HASH_FOREACH_PTR(*locks_ptr, lock) {
			if (lock != NULL && lock->context != NULL) {
				ctx = lock->context;

				break;
			}
		} ZEND_HASH_FOREACH_END();

		if (ctx == NULL) {
			user_cache_release_entry_locks_for_context(
				php_user_cache_owning_context(),
				locks_ptr,
				PHP_USER_CACHE_ENTRY_LOCK_RELEASE_DROP
			);

			return;
		}

		user_cache_release_entry_locks_for_context(ctx, locks_ptr, mode);
	}
}

static void user_cache_ensure_entry_lock_owner(void)
{
#ifndef ZEND_WIN32
	zend_ulong cur_pid = (zend_ulong) php_user_cache_cached_pid();

	if (UC_G(entry_lock_owner_pid) == 0) {
		UC_G(entry_lock_owner_pid) = cur_pid;

		return;
	}

	if (UC_G(entry_lock_owner_pid) == cur_pid) {
		return;
	}

	user_cache_release_entry_locks_for_context(
		php_user_cache_owning_context(),
		&UC_G(entry_lock_table),
		PHP_USER_CACHE_ENTRY_LOCK_RELEASE_DROP
	);

	UC_G(entry_lock_owner_pid) = cur_pid;
#endif
}

static void user_cache_entry_lock_dtor(zval *lock_zv)
{
	php_user_cache_entry_lock *lock = Z_PTR_P(lock_zv);

	if (lock != NULL) {
		efree(lock);
	}
}

static HashTable *user_cache_prepare_entry_locks_for_insert(uint32_t reserve)
{
	HashTable **locks_ptr = user_cache_entry_lock_table_ptr();

	if (*locks_ptr == NULL) {
		ALLOC_HASHTABLE(*locks_ptr);
		zend_hash_init(*locks_ptr, 0, NULL, user_cache_entry_lock_dtor, 0);
	}

	zend_hash_extend(*locks_ptr, zend_hash_num_elements(*locks_ptr) + reserve, 0);

	return *locks_ptr;
}

static bool user_cache_upgrade_held_entry_lock(
		php_user_cache_context *ctx,
		zend_string *key,
		zend_long lease,
		bool preserve_lease,
		php_user_cache_entry_lock *lock)
{
	php_user_cache_context *prev_ctx;
	php_user_cache_header *header;
	bool updated;

	if (lease > lock->lease) {
		prev_ctx = php_user_cache_activate_context(ctx);
		if (!php_user_cache_wlock()) {
			php_user_cache_restore_context(prev_ctx);

			return false;
		}

		header = php_user_cache_header_ptr();
		updated = php_user_cache_header_init_locked() &&
			user_cache_update_entry_lock_record_lease_locked(header, key, lease)
		;

		php_user_cache_unlock();
		php_user_cache_restore_context(prev_ctx);

		if (!updated) {
			return false;
		}

		lock->lease = lease;
	}

	lock->preserve_lease = lock->preserve_lease || preserve_lease;

	return true;
}

static bool user_cache_remove_owned_entry_lock_record(
		php_user_cache_context *ctx,
		zend_string *key,
		zend_ulong hash,
		uint64_t owner_pid)
{
	php_user_cache_context *prev_ctx;
	php_user_cache_header *header;
	uint32_t slot_idx;
	bool found;

	prev_ctx = php_user_cache_activate_context(ctx);
	if (!php_user_cache_wlock()) {
		php_user_cache_restore_context(prev_ctx);

		return false;
	}

	header = php_user_cache_header_ptr();

	if (php_user_cache_header_is_initialized_locked() &&
		user_cache_find_entry_lock_record_slot_locked(
			header,
			key,
			hash,
			&slot_idx,
			&found
		) &&
		found &&
		header->entry_lock_records[slot_idx].owner_pid == owner_pid
	) {
		user_cache_remove_entry_lock_record_locked(
			&header->entry_lock_records[slot_idx]
		);
	}

	php_user_cache_unlock();
	php_user_cache_restore_context(prev_ctx);

	return true;
}

/* Release all flagged entry locks under one write lock. */
static void user_cache_release_entry_locks_for_keys(
		php_user_cache_context *ctx,
		zend_string **keys,
		const bool *acquired,
		uint32_t count)
{
	php_user_cache_context *prev_ctx;
	php_user_cache_entry_lock *lock;
	php_user_cache_header *header;
	zend_ulong hash;
	HashTable **locks_ptr = user_cache_entry_lock_table_ptr();
	uint32_t i, slot_idx;
	bool found, any_held = false;

	user_cache_ensure_entry_lock_owner();

	for (i = 0; i < count; i++) {
		if (acquired[i] && user_cache_find_local_entry_lock(ctx, keys[i]) != NULL) {
			any_held = true;

			break;
		}
	}

	if (!any_held) {
		return;
	}

	prev_ctx = php_user_cache_activate_context(ctx);

	if (!php_user_cache_wlock()) {
		php_user_cache_restore_context(prev_ctx);

		return;
	}

	header = php_user_cache_header_ptr();

	if (php_user_cache_header_is_initialized_locked()) {
		for (i = 0; i < count; i++) {
			if (!acquired[i]) {
				continue;
			}

			lock = user_cache_find_local_entry_lock(ctx, keys[i]);
			if (lock == NULL) {
				continue;
			}

			hash = zend_string_hash_val(keys[i]);
			if (user_cache_find_entry_lock_record_slot_locked(header, keys[i], hash, &slot_idx, &found) &&
				found &&
				header->entry_lock_records[slot_idx].owner_pid == lock->owner_pid
			) {
				user_cache_remove_entry_lock_record_locked(&header->entry_lock_records[slot_idx]);
			}
		}
	}

	php_user_cache_unlock();
	php_user_cache_restore_context(prev_ctx);

	for (i = 0; i < count; i++) {
		if (acquired[i] && user_cache_find_local_entry_lock(ctx, keys[i]) != NULL) {
			zend_hash_del(*locks_ptr, keys[i]);
		}
	}

	user_cache_destroy_entry_locks_if_empty(locks_ptr);
}

static bool user_cache_acquire_entry_lock_record(
		php_user_cache_context *ctx,
		zend_string *key,
		zend_long lease,
		bool preserve_lease,
		bool blocking)
{
	php_user_cache_context *prev_ctx;
	php_user_cache_entry_lock *lock;
	php_user_cache_header *header;
	zend_ulong hash = zend_string_hash_val(key);
	HashTable *locks, **locks_ptr = user_cache_entry_lock_table_ptr();
	uint64_t waited_us = 0;
	uint32_t slot_idx;
	bool found, inserted, insert_failed;

	user_cache_ensure_entry_lock_owner();

	lock = user_cache_find_local_entry_lock(ctx, key);
	if (lock != NULL) {
		return user_cache_upgrade_held_entry_lock(ctx, key, lease, preserve_lease, lock);
	}

	locks = user_cache_prepare_entry_locks_for_insert(1);
	lock = user_cache_create_local_entry_lock(ctx, lease, preserve_lease);

	for (;;) {
		inserted = false;
		found = false;
		insert_failed = false;
		prev_ctx = php_user_cache_activate_context(ctx);

		if (!php_user_cache_wlock()) {
			php_user_cache_restore_context(prev_ctx);
			efree(lock);
			user_cache_destroy_entry_locks_if_empty(locks_ptr);

			return false;
		}

		header = php_user_cache_header_ptr();

		if (!php_user_cache_header_init_locked()) {
			php_user_cache_unlock();
			php_user_cache_restore_context(prev_ctx);
			efree(lock);
			user_cache_destroy_entry_locks_if_empty(locks_ptr);

			return false;
		}

		if (user_cache_find_entry_lock_record_slot_locked(header, key, hash, &slot_idx, &found)) {
			if (!found &&
				!(inserted = user_cache_insert_entry_lock_record_locked(header, slot_idx, key, hash, lease))
			) {
				insert_failed = true;
			}
		}

		php_user_cache_unlock();
		php_user_cache_restore_context(prev_ctx);

		if (inserted) {
			if (user_cache_add_local_entry_lock(locks, key, lock)) {
				return true;
			}

			/* Roll back a record that cannot be tracked locally. */
			user_cache_remove_owned_entry_lock_record(ctx, key, hash, lock->owner_pid);
			efree(lock);
			user_cache_destroy_entry_locks_if_empty(locks_ptr);

			return false;
		}

		if (insert_failed) {
			efree(lock);
			user_cache_destroy_entry_locks_if_empty(locks_ptr);

			return false;
		}

		if (!blocking || waited_us >= PHP_USER_CACHE_ENTRY_LOCK_WAIT_TIMEOUT_US) {
			efree(lock);
			user_cache_destroy_entry_locks_if_empty(locks_ptr);

			return false;
		}

		waited_us += user_cache_sleep_entry_lock_retry_interval();
	}
}

/* Keys are sorted so concurrent bulk operations acquire locks in one order. */
static bool user_cache_acquire_entry_lock_records(
		php_user_cache_context *ctx,
		zend_string **keys,
		bool *acquired,
		uint32_t count)
{
	php_user_cache_context *prev_ctx;
	php_user_cache_entry_lock *existing, **pending_locks;
	php_user_cache_header *header;
	zend_ulong hash;
	HashTable *locks, **locks_ptr = user_cache_entry_lock_table_ptr();
	uint64_t waited_us = 0;
	uint32_t i, slot_idx, start = 0, stop, insert_end, blocked_idx = UINT32_MAX, pending_count = 0;
	bool *collisions, found, blocked, insert_failed, add_failed;

	user_cache_ensure_entry_lock_owner();

	pending_locks = safe_emalloc(count, sizeof(*pending_locks), 0);
	collisions = safe_emalloc(count, sizeof(*collisions), 0);

	for (i = 0; i < count; i++) {
		acquired[i] = false;
		pending_locks[i] = NULL;
		collisions[i] = false;

		/* Collapse adjacent duplicates. */
		if (i > 0 && zend_string_equals(keys[i], keys[i - 1])) {
			continue;
		}

		existing = *locks_ptr != NULL ? zend_hash_find_ptr(*locks_ptr, keys[i]) : NULL;
		if (existing != NULL && existing->context == ctx) {
			continue;
		}

		/* Local tracking will reject a record from another context. */
		collisions[i] = existing != NULL;
		pending_locks[i] = user_cache_create_local_entry_lock(ctx, 0, false);

		pending_count++;
	}

	if (pending_count == 0) {
		efree(collisions);
		efree(pending_locks);

		return true;
	}

	locks = user_cache_prepare_entry_locks_for_insert(pending_count);

	for (;;) {
		stop = count;
		insert_end = count;
		blocked = false;
		insert_failed = false;
		add_failed = false;
		prev_ctx = php_user_cache_activate_context(ctx);

		if (!php_user_cache_wlock()) {
			php_user_cache_restore_context(prev_ctx);

			goto bailout;
		}

		header = php_user_cache_header_ptr();

		if (!php_user_cache_header_init_locked()) {
			php_user_cache_unlock();
			php_user_cache_restore_context(prev_ctx);

			goto bailout;
		}

		/* Stop before waiting so the global lock is not held. */
		for (i = start; i < count; i++) {
			if (pending_locks[i] == NULL) {
				continue;
			}

			hash = zend_string_hash_val(keys[i]);

			if (!user_cache_find_entry_lock_record_slot_locked(header, keys[i], hash, &slot_idx, &found) || found) {
				blocked = true;
				stop = i;
				insert_end = i;

				break;
			}

			if (!user_cache_insert_entry_lock_record_locked(header, slot_idx, keys[i], hash, 0)) {
				insert_failed = true;
				stop = i;
				insert_end = i;

				break;
			}

			if (collisions[i]) {
				/* Do not insert records beyond the known failure. */
				stop = i;
				insert_end = i + 1;

				break;
			}
		}

		php_user_cache_unlock();
		php_user_cache_restore_context(prev_ctx);

		/* Roll back records that cannot be tracked locally. */
		for (i = start; i < insert_end; i++) {
			if (pending_locks[i] == NULL) {
				continue;
			}

			if (add_failed || !user_cache_add_local_entry_lock(locks, keys[i], pending_locks[i])) {
				user_cache_remove_owned_entry_lock_record(
					ctx,
					keys[i],
					zend_string_hash_val(keys[i]),
					pending_locks[i]->owner_pid
				);
				efree(pending_locks[i]);
				pending_locks[i] = NULL;
				add_failed = true;

				continue;
			}

			pending_locks[i] = NULL;
			acquired[i] = true;
		}

		if (add_failed || insert_failed) {
			goto bailout;
		}

		if (!blocked) {
			efree(collisions);
			efree(pending_locks);

			return true;
		}

		if (blocked_idx != stop) {
			/* Each blocked key gets a fresh wait budget. */
			blocked_idx = stop;
			waited_us = 0;
		}

		if (waited_us >= PHP_USER_CACHE_ENTRY_LOCK_WAIT_TIMEOUT_US) {
			goto bailout;
		}

		waited_us += user_cache_sleep_entry_lock_retry_interval();
		start = stop;
	}

bailout:
	for (i = 0; i < count; i++) {
		if (pending_locks[i] != NULL) {
			efree(pending_locks[i]);
		}
	}

	efree(collisions);
	efree(pending_locks);
	user_cache_destroy_entry_locks_if_empty(locks_ptr);

	return false;
}

static uint32_t user_cache_calculate_capacity(size_t size)
{
	uint32_t capacity = PHP_USER_CACHE_MIN_CAPACITY;
	size_t target = size / PHP_USER_CACHE_SLOT_BYTES, data_offset;
	unsigned int i;

	/* Leave room for value data after the entry table. */
	for (i = 0; i < sizeof(php_user_cache_capacity_primes) / sizeof(php_user_cache_capacity_primes[0]); i++) {
		if (php_user_cache_capacity_primes[i] > target) {
			break;
		}

		data_offset = PHP_USER_CACHE_ALIGNED_SIZE(
			sizeof(php_user_cache_header)
			+ (size_t) php_user_cache_capacity_primes[i] * sizeof(php_user_cache_entry)
		);
		if (data_offset >= size) {
			break;
		}

		capacity = php_user_cache_capacity_primes[i];
	}

	return capacity;
}

static void user_cache_free_list_remove_locked(php_user_cache_header *header, uint32_t block_offset)
{
	php_user_cache_block *block = php_user_cache_block_ptr(block_offset);

	if (block->prev_free != 0) {
		php_user_cache_block_ptr(block->prev_free)->next_free = block->next_free;
	} else {
		header->free_list = block->next_free;
	}

	if (block->next_free != 0) {
		php_user_cache_block_ptr(block->next_free)->prev_free = block->prev_free;
	}

	block->next_free = 0;
	block->prev_free = 0;
	block->flags &= ~PHP_USER_CACHE_BLOCK_FREE;
}

static void user_cache_free_list_insert_locked(php_user_cache_header *header, uint32_t block_offset)
{
	php_user_cache_block *block = php_user_cache_block_ptr(block_offset);

	block->prev_free = 0;
	block->next_free = header->free_list;

	if (header->free_list != 0) {
		php_user_cache_block_ptr(header->free_list)->prev_free = block_offset;
	}

	user_cache_block_mark_free(block);

	header->free_list = block_offset;
}

static void user_cache_update_following_prev_size_locked(
	php_user_cache_header *header,
	uint32_t block_offset,
	const php_user_cache_block *block
)
{
	uint32_t next_offset = block_offset + block->size;

	if (next_offset < user_cache_used_end_offset_locked(header)) {
		php_user_cache_block_ptr(next_offset)->prev_size = block->size;
	}
}

static void user_cache_trim_tail_free_blocks_locked(
	php_user_cache_header *header,
	uint32_t block_offset
)
{
	php_user_cache_block *block = php_user_cache_block_ptr(block_offset);
	uint32_t prev_offset;

	while (block_offset >= header->data_offset &&
		header->last_block_offset == block_offset &&
		php_user_cache_block_is_free(block) &&
		block_offset + block->size == user_cache_used_end_offset_locked(header)
	) {
		prev_offset = 0;
		user_cache_free_list_remove_locked(header, block_offset);
		header->next_free -= block->size;

		if (block->prev_size != 0 && block_offset > header->data_offset) {
			prev_offset = block_offset - block->prev_size;
		}

		header->last_block_offset = prev_offset;

		if (prev_offset == 0) {
			break;
		}

		block_offset = prev_offset;
		block = php_user_cache_block_ptr(block_offset);
	}
}

static void user_cache_write_section_enter(void)
{
	php_user_cache_header *header = php_user_cache_header_ptr();

	UC_G(write_seq_bumped) = false;

#ifdef PHP_USER_CACHE_HAVE_OPTIMISTIC
	UC_G(reader_drain_state) = 0;
#endif

	if (header == NULL || !php_user_cache_header_is_initialized_locked()) {
		return;
	}

	user_cache_seq_announce(&header->write_seq, header->write_seq + 1);

	UC_G(write_seq_bumped) = true;
}

static void user_cache_write_section_leave(void)
{
	php_user_cache_header *header;

	if (!UC_G(write_seq_bumped)) {
		return;
	}

	UC_G(write_seq_bumped) = false;

#ifdef PHP_USER_CACHE_HAVE_OPTIMISTIC
	UC_G(reader_drain_state) = 0;
#endif

	header = php_user_cache_header_ptr();
	if (header == NULL) {
		return;
	}

	user_cache_seq_publish(&header->write_seq, header->write_seq + 1);
}

static void user_cache_write_section_note_header_initialized(php_user_cache_header *header)
{
	if (!UC_G(lock_held_is_write) || UC_G(write_seq_bumped)) {
		return;
	}

	user_cache_seq_announce(&header->write_seq, header->write_seq + 1);
	UC_G(write_seq_bumped) = true;

#ifdef PHP_USER_CACHE_HAVE_OPTIMISTIC
	UC_G(reader_drain_state) = 0;
#endif
}

/* Publish a fresh segment by storing its magic last. */
static bool user_cache_header_format_fresh_locked(
		php_user_cache_header *header,
		uint32_t capacity,
		uint32_t data_offset)
{
	php_user_cache_storage *storage = &php_user_cache_active_context()->storage;

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
	/* Preserve an existing odd sequence until the write section ends. */
	header->write_seq = UC_G(write_seq_bumped) ? 1u : 2u;
#ifdef PHP_USER_CACHE_HAVE_SHARED_MUTEX
	header->lock_model = user_cache_shared_mutex_init(header)
		? PHP_USER_CACHE_LOCK_MODEL_MUTEX
		: PHP_USER_CACHE_LOCK_MODEL_FCNTL
	;
#else
	header->lock_model = PHP_USER_CACHE_LOCK_MODEL_FCNTL;
#endif
	if (php_user_cache_active_context()->boundary_shared) {
#ifdef PHP_USER_CACHE_HAVE_BOUNDARY_SHM
		user_cache_shared_boundary_digest(
			php_user_cache_active_context(),
			storage->size,
			header->boundary_identity_digest
		);

		header->boundary_identity_digest_set = 1;
#else
		header->boundary_identity_digest_set = 0;
#endif
	}

	header->version = PHP_USER_CACHE_VERSION;
#ifdef PHP_USER_CACHE_HAVE_OPTIMISTIC
	user_cache_atomic_fence_seq_cst();
#endif
	header->magic = PHP_USER_CACHE_MAGIC;

	user_cache_write_section_note_header_initialized(header);

	return true;
}

static uint32_t user_cache_alloc_from_free_list_locked(
		php_user_cache_header *header,
		size_t size,
		uint32_t total_size,
		const void *src)
{
	php_user_cache_block *block, *remainder;
	uint32_t min_split_size, best_offset = 0, best_size = UINT32_MAX, *free_offset_ptr;

	min_split_size = (uint32_t) PHP_USER_CACHE_ALIGNED_SIZE(sizeof(php_user_cache_block) + 1);

	free_offset_ptr = &header->free_list;
	while (*free_offset_ptr != 0) {
		block = php_user_cache_block_ptr(*free_offset_ptr);

		if (block->size >= total_size && block->size < best_size) {
			best_offset = *free_offset_ptr;
			best_size = block->size;
			if (best_size == total_size) {
				break;
			}
		}

		free_offset_ptr = &block->next_free;
	}

	if (best_offset == 0) {
		return 0;
	}

	block = php_user_cache_block_ptr(best_offset);
	user_cache_free_list_remove_locked(header, best_offset);

	if (block->size >= total_size + min_split_size) {
		remainder = php_user_cache_block_ptr(best_offset + total_size);
		remainder->size = block->size - total_size;
		remainder->prev_size = total_size;
		remainder->next_free = 0;
		remainder->prev_free = 0;
		remainder->flags = PHP_USER_CACHE_BLOCK_FREE;

		block->size = total_size;

		user_cache_update_following_prev_size_locked(header, best_offset + total_size, remainder);
		user_cache_free_list_insert_locked(header, best_offset + total_size);

		if (header->last_block_offset == best_offset) {
			header->last_block_offset = best_offset + total_size;
		}
	} else {
		user_cache_update_following_prev_size_locked(header, best_offset, block);
	}

	user_cache_block_mark_used(block);

	if (src != NULL) {
		memcpy(php_user_cache_ptr(best_offset + sizeof(php_user_cache_block)), src, size);
	}

	return best_offset + (uint32_t) sizeof(php_user_cache_block);
}

static uint32_t user_cache_alloc_from_tail_locked(
		php_user_cache_header *header,
		size_t size,
		uint32_t total_size,
		const void *src)
{
	php_user_cache_block *block;
	uint32_t block_offset;

	if (header->next_free > header->data_size || total_size > header->data_size - header->next_free) {
		return 0;
	}

	block_offset = header->data_offset + header->next_free;
	block = php_user_cache_block_ptr(block_offset);
	block->size = total_size;
	block->prev_size = header->last_block_offset != 0 ? php_user_cache_block_ptr(header->last_block_offset)->size : 0;
	block->next_free = 0;
	block->prev_free = 0;
	block->flags = 0;

	if (src != NULL) {
		memcpy(php_user_cache_ptr(block_offset + sizeof(php_user_cache_block)), src, size);
	}

	header->next_free += total_size;
	header->last_block_offset = block_offset;

	return block_offset + (uint32_t) sizeof(php_user_cache_block);
}

static void user_cache_log_storage_handler_failure(const char *error_in, int error_code)
{
#ifdef ZEND_WIN32
	char *msg = php_win32_error_to_msg(error_code);

	php_error_docref(
		NULL, E_WARNING,
		"Cache: shared memory initialization failed: %s: %s (%d)",
		error_in != NULL ? error_in : "unknown",
		msg,
		error_code
	);

	php_win32_error_msg_free(msg);
#else
	php_error_docref(
		NULL, E_WARNING,
		"Cache: shared memory initialization failed: %s: %s (%d)",
		error_in != NULL ? error_in : "unknown",
		strerror(error_code),
		error_code
	);
#endif
}

static bool user_cache_try_storage_handler(
	const php_user_cache_shm_handler_entry *handler_entry,
	const char **error_in_ptr,
	int *error_code_ptr
)
{
	const char *error_in = NULL;
	php_user_cache_context *ctx = php_user_cache_active_context();
	php_user_cache_runtime *runtime = php_user_cache_active_runtime();
	php_user_cache_storage *storage = &ctx->storage;
	php_user_cache_shm_segment **segments = NULL;
	int segment_count = 0;

	if (handler_entry->handler->create_segments(
			runtime->configured_memory,
			&segments,
			&segment_count,
			&error_in
		) != PHP_USER_CACHE_ALLOC_SUCCESS
	) {
		/* Preserve the error across cleanup. */
#ifdef ZEND_WIN32
		*error_code_ptr = (int) GetLastError();
#else
		*error_code_ptr = errno;
#endif
		*error_in_ptr = error_in;
		user_cache_cleanup_segments(handler_entry->handler, segments, segment_count);

		return false;
	}

	storage->handler = handler_entry->handler;
	storage->segments = segments;
	storage->segment_count = segment_count;
	storage->size = runtime->configured_memory;
	storage->initialized = true;

	return true;
}

/* Boundary contexts must not fall back to process-local storage. */
static bool user_cache_select_storage_handler(void)
{
	const php_user_cache_shm_handler_entry *handler_entry;
	const char *model = UC_G(memory_model);
	const char *error_in = NULL;
	int saved_error = 0;

#ifdef PHP_USER_CACHE_HAVE_BOUNDARY_SHM
	if (php_user_cache_active_context()->boundary_shared) {
		if (user_cache_try_storage_handler(
				user_cache_shared_boundary_handler_entry(),
				&error_in,
				&saved_error
			)
		) {
			return true;
		}

		user_cache_log_storage_handler_failure(error_in, saved_error);

		return false;
	}
#endif

	if (model && model[0]) {
		if (strcmp(model, "cgi") == 0) {
			model = "shm";
		}

		for (handler_entry = user_cache_handler_table(); handler_entry->name; handler_entry++) {
			if (strcmp(model, handler_entry->name) == 0 &&
				user_cache_try_storage_handler(handler_entry, &error_in, &saved_error)
			) {
				return true;
			}
		}
	}

	for (handler_entry = user_cache_handler_table(); handler_entry->name; handler_entry++) {
		if (model && model[0] && strcmp(model, handler_entry->name) == 0) {
			continue;
		}

		if (user_cache_try_storage_handler(handler_entry, &error_in, &saved_error)) {
			return true;
		}
	}

	user_cache_log_storage_handler_failure(error_in, saved_error);

	return false;
}

/* Adopt the segment's advertised lock model: re-acquire via the shared mutex
 * if an initialized segment already uses one, otherwise stay on the fcntl
 * lock and recover. Returns false when negotiation fails and the caller must
 * tear down. */
static bool user_cache_negotiate_lock_model(void)
{
#ifdef PHP_USER_CACHE_HAVE_SHARED_MUTEX
	php_user_cache_storage *storage = &php_user_cache_active_context()->storage;

	if (!user_cache_wlock_impl()) {
		return false;
	}

	if (user_cache_header_is_initialized_acquire() &&
		php_user_cache_header_ptr()->lock_model == PHP_USER_CACHE_LOCK_MODEL_MUTEX
	) {
		user_cache_unlock_impl();
		storage->use_shared_mutex = true;

		if (!php_user_cache_wlock()) {
			return false;
		}
	} else {
		UC_G(lock_held_is_write) = true;

		if (!user_cache_recover_after_owner_death()) {
			UC_G(lock_held_is_write) = false;
			user_cache_unlock_impl();

			return false;
		}

		user_cache_write_section_enter();
	}

	return true;
#else
	return php_user_cache_wlock();
#endif
}

static bool user_cache_startup_storage_impl(void)
{
	php_user_cache_storage *storage = &php_user_cache_active_context()->storage;

	if (storage->initialized) {
		return true;
	}

	if (!user_cache_select_storage_handler()) {
		return false;
	}

	if (storage->segment_count != 1) {
		goto bailout;
	}

	if (!user_cache_create_lock()) {
		goto bailout;
	}

	if (!user_cache_negotiate_lock_model()) {
		user_cache_destroy_lock();

		goto bailout;
	}

	if (!php_user_cache_header_init_locked()) {
		php_user_cache_unlock();
		user_cache_destroy_lock();

		goto bailout;
	}

	php_user_cache_unlock();

#ifdef PHP_USER_CACHE_HAVE_SHARED_MUTEX
	storage->use_shared_mutex =
		php_user_cache_header_ptr() != NULL &&
		php_user_cache_header_ptr()->lock_model == PHP_USER_CACHE_LOCK_MODEL_MUTEX
	;
#endif

#ifdef PHP_USER_CACHE_HAVE_OPTIMISTIC
	user_cache_atomic_store_32(&storage->startup_complete, 1);
#else
	storage->startup_complete = 1;
#endif

	return true;

bailout:
	user_cache_cleanup_segments(
		storage->handler,
		storage->segments,
		storage->segment_count
	);
	php_user_cache_reset_storage();

	return false;
}

static bool user_cache_startup_storage(void)
{
	bool result;

	if (user_cache_startup_storage_is_complete()) {
		return true;
	}

	user_cache_startup_storage_lock();
	result = user_cache_startup_storage_impl();
	user_cache_startup_storage_unlock();

	return result;
}

static void user_cache_resolve_runtime(void)
{
	php_user_cache_context *ctx = php_user_cache_active_context();
	php_user_cache_runtime *runtime;
	php_user_cache_storage *storage = &ctx->storage;

	php_user_cache_reset_runtime();

	runtime = php_user_cache_active_runtime();
	if (!runtime->enabled) {
		return;
	}

	if (UC_G(request_unavailable_reason) != PHP_USER_CACHE_REASON_NONE) {
		user_cache_set_unavailable(UC_G(request_unavailable_reason));

		return;
	}

	if (!user_cache_is_opted_in()) {
		user_cache_set_unavailable(PHP_USER_CACHE_REASON_SAPI_NOT_ENABLED);

		return;
	}

	if (user_cache_is_disabled_for_sapi()) {
		user_cache_set_unavailable(PHP_USER_CACHE_REASON_DISABLED_BY_INI);

		return;
	}

	if (!storage->initialized &&
		user_cache_requires_pre_request_storage()
	) {
		user_cache_set_unavailable(PHP_USER_CACHE_REASON_BACKEND_NOT_INITIALIZED_BEFORE_WORKER);

		return;
	}

	if (!user_cache_startup_storage()) {
		user_cache_set_unavailable(PHP_USER_CACHE_REASON_SHM_INIT_FAILED);

		return;
	}

	if (user_cache_requires_pre_request_storage() &&
		!storage->initialized_before_request
	) {
		user_cache_set_unavailable(PHP_USER_CACHE_REASON_BACKEND_INITIALIZED_AFTER_WORKER);

		return;
	}

	user_cache_set_available();
}

#ifdef ZTS
static bool user_cache_claim_header_is_attached(const php_user_cache_header *header)
{
	const php_user_cache_partition *partition;

	if (user_cache_storage_holds_header(&php_user_cache_context_state.storage, header)) {
		return true;
	}

	for (partition = php_user_cache_partitions; partition != NULL; partition = partition->next) {
		if (user_cache_storage_holds_header(&partition->context.storage, header)) {
			return true;
		}
	}

	return false;
}

/* Release this thread's graph pin slot claims: the process outlives the
 * thread, so its live pid would keep the slots unclaimable. Slots with pins
 * outstanding are left for the dead-owner sweep. */
void php_user_cache_release_thread_graph_pin_claims(php_user_cache_globals *globals)
{
	php_user_cache_graph_pin_slot *slot;
	int expected;
	uint32_t i;

	for (i = 0; i < globals->graph_pin_claim_count; i++) {
		if (!user_cache_claim_header_is_attached(globals->graph_pin_claims[i].header)) {
			continue;
		}

		slot = &globals->graph_pin_claims[i].header->graph_pin_slots[
			globals->graph_pin_claims[i].slot_index
		];

		expected = zend_atomic_int_load_ex(&slot->owner_pid);
		if (expected != (int) (uint32_t) php_user_cache_cached_pid() ||
			zend_atomic_int_load_ex(&slot->pin_count) != 0
		) {
			continue;
		}

		/* Clear start time before pid so scanners cannot combine owners. */
		slot->owner_start_time = 0;
		zend_atomic_int_compare_exchange_ex(&slot->owner_pid, &expected, 0);
	}

	globals->graph_pin_claim_count = 0;
}
#endif

#ifdef PHP_USER_CACHE_HAVE_OPTIMISTIC
static bool user_cache_reader_slot_owner_is_dead(const php_user_cache_reader_slot *slot)
{
	/* Reader slots are claimed lock-free. */
	uint64_t owner_pid = php_user_cache_atomic_load_64(&slot->owner_pid),
		owner_start_time = php_user_cache_atomic_load_64(&slot->owner_start_time)
	;

	return user_cache_entry_lock_owner_is_dead(owner_pid, owner_start_time);
}

#ifndef ZEND_WIN32
static void user_cache_optimistic_atfork_child(void)
{
	UC_G(reader_claim_count) = 0;
}
#endif

static int32_t user_cache_claim_reader_slot(php_user_cache_header *header)
{
	php_user_cache_reader_slot *slot;
	uint64_t my_pid, expected;
	uint32_t i;

	for (i = 0; i < UC_G(reader_claim_count); i++) {
		if (UC_G(reader_claims)[i].header == header) {
			return (int32_t) UC_G(reader_claims)[i].slot_index;
		}
	}

	if (UC_G(reader_claim_count) == PHP_USER_CACHE_READER_CLAIM_MAX ||
		php_user_cache_reader_incarnation == 0
	) {
		return -1;
	}

	my_pid = php_user_cache_cached_pid();

	for (i = 0; i < PHP_USER_CACHE_READER_SLOTS; i++) {
		slot = &header->reader_slots[i];

		expected = php_user_cache_atomic_load_64(&slot->owner_pid);

		if (expected != 0) {
			if (!user_cache_reader_slot_owner_is_dead(slot) ||
				user_cache_atomic_load_32(&slot->active) != 0
			) {
				continue;
			}

			/* Clear start time before pid so scanners cannot combine owners. */
			user_cache_atomic_store_64(&slot->owner_start_time, 0);
			if (!user_cache_atomic_cas_64(&slot->owner_pid, expected, 0)) {
				continue;
			}
		}

		if (user_cache_atomic_cas_64(&slot->owner_pid, 0, my_pid)) {
			user_cache_atomic_store_64(&slot->owner_start_time, user_cache_cached_self_start_time_token(my_pid));

			goto found;
		}
	}

	return -1;

found:
	UC_G(reader_claims)[UC_G(reader_claim_count)].header = header;
	UC_G(reader_claims)[UC_G(reader_claim_count)].slot_index = i;
	UC_G(reader_claim_count)++;

	return (int32_t) i;
}

bool php_user_cache_quiesce_graph_payloads_locked(void)
{
	php_user_cache_header *header;
	php_user_cache_reader_slot *slot;
	uint64_t waited_us = 0;
	uint32_t i, count, spin = 0;

	if (!UC_G(write_seq_bumped)) {
		return true;
	}

	if (UC_G(reader_drain_state) != 0) {
		return UC_G(reader_drain_state) > 0;
	}

	header = php_user_cache_header_ptr();
	if (header == NULL) {
		return true;
	}

	user_cache_atomic_fence_seq_cst();

	for (;;) {
		count = 0;
		for (i = 0; i < PHP_USER_CACHE_READER_SLOTS; i++) {
			if (user_cache_atomic_load_32(&header->reader_slots[i].active) != 0) {
				count++;
			}
		}

		if (count == 0) {
			UC_G(reader_drain_state) = 1;

			return true;
		}

		spin++;

		if ((spin & 0xFFU) == 0) {
			for (i = 0; i < PHP_USER_CACHE_READER_SLOTS; i++) {
				slot = &header->reader_slots[i];

				if (user_cache_atomic_load_32(&slot->active) == 0 ||
					php_user_cache_atomic_load_64(&slot->owner_pid) == 0
				) {
					continue;
				}

				if (user_cache_reader_slot_owner_is_dead(slot)) {
					user_cache_atomic_cas_32(&slot->active, 1, 0);
				}
			}
		}

		if (spin > PHP_USER_CACHE_READER_DRAIN_SPIN) {
#ifdef ZEND_WIN32
			Sleep(1);
			waited_us += 1000;
#else
			usleep(50);
			waited_us += 50;
#endif

			if (waited_us > PHP_USER_CACHE_READER_DRAIN_TIMEOUT_US) {
				UC_G(reader_drain_state) = -1;

				return false;
			}
		}
	}
}

void php_user_cache_optimistic_fork_setup(void)
{
	static bool registered = false;

	if (php_user_cache_reader_incarnation == 0) {
		php_user_cache_reader_incarnation =
			((uint64_t) time(NULL) << 20) ^
			(uint64_t) (uintptr_t) &php_user_cache_reader_incarnation ^
			php_user_cache_current_pid()
		;

		if (php_user_cache_reader_incarnation == 0) {
			php_user_cache_reader_incarnation = 1;
		}
	}

	if (registered) {
		return;
	}

#ifndef ZEND_WIN32
	if (pthread_atfork(NULL, NULL, user_cache_optimistic_atfork_child) != 0) {
		return;
	}
#endif

	registered = true;
}

bool php_user_cache_optimistic_reader_begin(php_user_cache_header *header, uint32_t *slot_idx_ptr)
{
	int32_t slot_idx = user_cache_claim_reader_slot(header);

	if (slot_idx < 0) {
		return false;
	}

	*slot_idx_ptr = (uint32_t) slot_idx;

	user_cache_atomic_store_32(&header->reader_slots[slot_idx].active, 1);
	user_cache_atomic_fence_seq_cst();

	return true;
}

void php_user_cache_optimistic_reader_end(php_user_cache_header *header, uint32_t slot_idx)
{
	user_cache_atomic_store_32(&header->reader_slots[slot_idx].active, 0);
}

#ifdef ZTS
/* Release per-thread reader slots before their live process pid makes them
 * permanently unclaimable. Clear active and start time before owner_pid. */
void php_user_cache_release_thread_reader_claims(php_user_cache_globals *globals)
{
	php_user_cache_reader_slot *slot;
	uint64_t my_pid;
	uint32_t i;

	if (globals->reader_claim_count == 0) {
		return;
	}

	my_pid = php_user_cache_cached_pid();

	for (i = 0; i < globals->reader_claim_count; i++) {
		if (!user_cache_claim_header_is_attached(globals->reader_claims[i].header)) {
			continue;
		}

		slot = &globals->reader_claims[i].header->reader_slots[globals->reader_claims[i].slot_index];

		user_cache_atomic_store_32(&slot->active, 0);
		user_cache_atomic_store_64(&slot->owner_start_time, 0);
		user_cache_atomic_cas_64(&slot->owner_pid, my_pid, 0);
	}

	globals->reader_claim_count = 0;
}
#endif
#else
bool php_user_cache_quiesce_graph_payloads_locked(void)
{
	return true;
}

void php_user_cache_optimistic_fork_setup(void)
{
}

bool php_user_cache_optimistic_reader_begin(php_user_cache_header *header, uint32_t *slot_idx_ptr)
{
	(void) header;
	(void) slot_idx_ptr;

	return false;
}

void php_user_cache_optimistic_reader_end(php_user_cache_header *header, uint32_t slot_idx)
{
	(void) header;
	(void) slot_idx;
}
#endif

/* Shared-graph pin owners reuse the entry-lock liveness probe and its
 * one-second cache. */
bool php_user_cache_graph_pin_owner_is_dead(uint64_t owner_pid, uint64_t owner_start_time)
{
	return user_cache_entry_lock_owner_is_dead(owner_pid, owner_start_time);
}

uint64_t php_user_cache_self_start_time_token(void)
{
	return user_cache_cached_self_start_time_token(php_user_cache_cached_pid());
}

void php_user_cache_reset_runtime(void)
{
	php_user_cache_runtime *runtime = php_user_cache_active_runtime();

	UC_G(runtime_resolved) = false;

	memset(runtime, 0, sizeof(*runtime));

	runtime->configured_memory = UC_G(shm_size);

	runtime->enabled = runtime->configured_memory != 0;
	if (!runtime->enabled) {
		runtime->available = false;
		runtime->failure_reason = PHP_USER_CACHE_REASON_DISABLED_BY_INI;
	}
}

void php_user_cache_reset_storage(void)
{
	php_user_cache_storage *storage = &php_user_cache_active_context()->storage;

	memset(storage, 0, sizeof(*storage));

	storage->lock_file = -1;
}

bool php_user_cache_header_init_locked(void)
{
	php_user_cache_storage *storage = &php_user_cache_active_context()->storage;
	php_user_cache_header *header = php_user_cache_header_ptr();
	uint32_t capacity, data_offset;

	if (!header) {
		return false;
	}

	/* Cache the layout for the lifetime of the attachment. */
	if (!storage->layout_memo_valid) {
		storage->capacity_memo = user_cache_calculate_capacity(storage->size);
		storage->data_offset_memo = (uint32_t) PHP_USER_CACHE_ALIGNED_SIZE(
			sizeof(php_user_cache_header) + storage->capacity_memo * sizeof(php_user_cache_entry)
		);
		storage->layout_memo_valid = true;
	}
	capacity = storage->capacity_memo;
	data_offset = storage->data_offset_memo;

	if (header->magic == PHP_USER_CACHE_MAGIC && header->version == PHP_USER_CACHE_VERSION) {
		/* Adopt an initialized segment only when its layout matches this
		 * build; otherwise fall through and reformat it. */
		if (header->capacity == capacity && header->data_offset == data_offset) {
#ifdef PHP_USER_CACHE_HAVE_BOUNDARY_SHM
			return user_cache_header_boundary_identity_matches_locked(header, storage->size);
#else
			return true;
#endif
		}
	}

	return user_cache_header_format_fresh_locked(header, capacity, data_offset);
}

void php_user_cache_free_locked(uint32_t payload_offset)
{
	php_user_cache_header *header = php_user_cache_header_ptr();
	php_user_cache_block *block, *adjacent;
	uint32_t block_offset, original_block_offset, next_offset, prev_offset;

	if (!header || payload_offset < sizeof(php_user_cache_block)) {
		return;
	}

	block_offset = payload_offset - (uint32_t) sizeof(php_user_cache_block);
	original_block_offset = block_offset;
	block = php_user_cache_block_ptr(block_offset);
	if (php_user_cache_block_is_free(block)) {
		return;
	}

	user_cache_block_mark_free(block);

	next_offset = block_offset + block->size;
	if (next_offset < user_cache_used_end_offset_locked(header)) {
		adjacent = php_user_cache_block_ptr(next_offset);

		if (php_user_cache_block_is_free(adjacent)) {
			user_cache_free_list_remove_locked(header, next_offset);
			block->size += adjacent->size;

			if (header->last_block_offset == next_offset) {
				header->last_block_offset = block_offset;
			}
		}
	}

	if (block->prev_size != 0 && block_offset > header->data_offset) {
		prev_offset = block_offset - block->prev_size;

		adjacent = php_user_cache_block_ptr(prev_offset);
		if (php_user_cache_block_is_free(adjacent)) {
			user_cache_free_list_remove_locked(header, prev_offset);

			block->size += adjacent->size;
			adjacent->size = block->size;
			block = adjacent;
			block_offset = prev_offset;

			if (header->last_block_offset == original_block_offset) {
				header->last_block_offset = block_offset;
			}
		}
	}

	user_cache_update_following_prev_size_locked(header, block_offset, block);
	user_cache_free_list_insert_locked(header, block_offset);
	user_cache_trim_tail_free_blocks_locked(header, block_offset);
}

uint32_t php_user_cache_alloc_locked(size_t size, const void *src)
{
	php_user_cache_header *header = php_user_cache_header_ptr();
	uint32_t total_size, offset;
	size_t aligned_size;

	if (!header || size == 0 || size > UINT32_MAX - sizeof(php_user_cache_block)) {
		return 0;
	}

	aligned_size = PHP_USER_CACHE_ALIGNED_SIZE(sizeof(php_user_cache_block) + size);
	if (aligned_size > UINT32_MAX) {
		return 0;
	}

	total_size = (uint32_t) aligned_size;

	offset = user_cache_alloc_from_free_list_locked(header, size, total_size, src);
	if (offset != 0) {
		return offset;
	}

	return user_cache_alloc_from_tail_locked(header, size, total_size, src);
}

bool php_user_cache_startup_storage_before_request(void)
{
	php_user_cache_storage *storage = &php_user_cache_active_context()->storage;

	if (!user_cache_is_opted_in()) {
		user_cache_set_unavailable(PHP_USER_CACHE_REASON_SAPI_NOT_ENABLED);

		return true;
	}

	if (!user_cache_startup_storage()) {
		user_cache_set_unavailable(PHP_USER_CACHE_REASON_SHM_INIT_FAILED);

		return false;
	}

	storage->initialized_before_request = true;

	return true;
}

void php_user_cache_shutdown_storage(void)
{
	php_user_cache_storage *storage = &php_user_cache_active_context()->storage;

	user_cache_destroy_lock();
	user_cache_cleanup_segments(
		storage->handler,
		storage->segments,
		storage->segment_count
	);
	php_user_cache_reset_storage();
}

void php_user_cache_ensure_ready_impl(void)
{
	php_user_cache_context *ctx = php_user_cache_active_context();

	user_cache_resolve_runtime();

	UC_G(runtime_resolved) = true;
	UC_G(runtime_resolved_context) = ctx;
	UC_G(runtime_resolved_enabled) = UC_G(enable);
}

bool php_user_cache_rlock(void)
{
	php_user_cache_header *header;
	uint32_t recovery_attempts = 0;

	for (;;) {
		if (!user_cache_rlock_impl()) {
			return false;
		}

		UC_G(lock_held_is_write) = false;

		header = php_user_cache_header_ptr();
		if (!UNEXPECTED(
				header != NULL &&
				php_user_cache_header_is_initialized_locked() &&
				(php_user_cache_seq_load(&header->write_seq) & 1) != 0
			)
		) {
			return true;
		}

		/* An odd write_seq means a writer died mid-update; take the write
		 * lock to drive recovery, then retry the read. Bail after a bounded
		 * number of attempts. */
		php_user_cache_unlock();

		if (++recovery_attempts > 8) {
			return false;
		}

		if (!php_user_cache_wlock()) {
			return false;
		}

		php_user_cache_unlock();
	}
}

bool php_user_cache_wlock(void)
{
	if (!user_cache_wlock_impl()) {
		return false;
	}

	UC_G(lock_held_is_write) = true;

	if (!user_cache_recover_after_owner_death()) {
		UC_G(lock_held_is_write) = false;
		user_cache_unlock_impl();

		return false;
	}

	user_cache_write_section_enter();

	return true;
}

bool php_user_cache_wlock_for_entry_mutation(zend_string *key)
{
	php_user_cache_entry_lock *local_lock;
	php_user_cache_header *header;
	zend_ulong hash = zend_string_hash_val(key);
	uint64_t waited_us = 0;
	uint32_t slot_idx;
	bool found;

	user_cache_ensure_entry_lock_owner();

	for (;;) {
		if (!php_user_cache_wlock()) {
			return false;
		}

		if (!php_user_cache_header_init_locked()) {
			php_user_cache_unlock();

			return false;
		}

		header = php_user_cache_header_ptr();
		if (!user_cache_find_entry_lock_record_slot_locked(header, key, hash, &slot_idx, &found) || !found) {
			return true;
		}

		local_lock = user_cache_find_local_entry_lock(php_user_cache_active_context(), key);
		if (local_lock != NULL &&
			header->entry_lock_records[slot_idx].owner_pid == local_lock->owner_pid
		) {
			return true;
		}

		php_user_cache_unlock();

		if (waited_us >= PHP_USER_CACHE_ENTRY_LOCK_WAIT_TIMEOUT_US) {
			return false;
		}

		waited_us += user_cache_sleep_entry_lock_retry_interval();
	}
}

void php_user_cache_unlock(void)
{
	if (UC_G(lock_held_is_write)) {
		user_cache_write_section_leave();
	}

	UC_G(lock_held_is_write) = false;

	user_cache_unlock_impl();
}

void php_user_cache_unlock_if_held(void)
{
	if (UC_G(lock_held)) {
		php_user_cache_unlock();
	}
}

bool php_user_cache_try_acquire_entry_lock(zend_string *key, zend_long lease)
{
	return user_cache_acquire_entry_lock_record(
		php_user_cache_active_context(),
		key,
		lease,
		true,
		false
	);
}

bool php_user_cache_acquire_entry_locks(zend_string **keys, bool *acquired, uint32_t count)
{
	return user_cache_acquire_entry_lock_records(
		php_user_cache_active_context(),
		keys,
		acquired,
		count
	);
}

bool php_user_cache_release_entry_lock(zend_string *key)
{
	php_user_cache_context *ctx = php_user_cache_active_context();
	php_user_cache_entry_lock *lock;
	zend_ulong hash;
	HashTable **locks_ptr = user_cache_entry_lock_table_ptr();

	user_cache_ensure_entry_lock_owner();

	lock = user_cache_find_local_entry_lock(ctx, key);
	if (lock == NULL) {
		return false;
	}

	hash = zend_string_hash_val(key);
	if (!user_cache_remove_owned_entry_lock_record(ctx, key, hash, lock->owner_pid)) {
		return false;
	}

	zend_hash_del(*locks_ptr, key);
	user_cache_destroy_entry_locks_if_empty(locks_ptr);

	return true;
}

void php_user_cache_release_entry_locks(zend_string **keys, const bool *acquired, uint32_t count)
{
	user_cache_release_entry_locks_for_keys(
		php_user_cache_active_context(),
		keys,
		acquired,
		count
	);
}

bool php_user_cache_entry_locks_allow_clear_locked(void)
{
	php_user_cache_header *header = php_user_cache_header_ptr();
	php_user_cache_entry_lock_record *record;
	uint64_t owner_pid, now;
	uint32_t i;

	user_cache_ensure_entry_lock_owner();

	if (header == NULL || !php_user_cache_header_is_initialized_locked()) {
		return true;
	}

	owner_pid = php_user_cache_cached_pid();
	now = (uint64_t) time(NULL);

	for (i = 0; i < PHP_USER_CACHE_ENTRY_LOCK_TABLE_SIZE; i++) {
		record = &header->entry_lock_records[i];
		if (record->state != PHP_USER_CACHE_ENTRY_LOCK_USED) {
			continue;
		}

		if (!user_cache_entry_lock_record_is_active_locked(record, now, true)) {
			user_cache_remove_entry_lock_record_locked(record);
			continue;
		}

		if (record->owner_pid != owner_pid) {
			return false;
		}
	}

	return true;
}

void php_user_cache_release_request_entry_locks(void)
{
	user_cache_release_entry_locks_all_contexts(
		&UC_G(entry_lock_table),
		PHP_USER_CACHE_ENTRY_LOCK_RELEASE_PRESERVE_LEASES
	);
#ifndef ZEND_WIN32
	UC_G(entry_lock_owner_pid) = 0;
#endif
}
