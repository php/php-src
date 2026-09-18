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
   | Author: Go Kudo <zeriyoshi@php.net>                                  |
   +----------------------------------------------------------------------+
*/

#include "user_cache_internal.h"

#include "ext/hash/php_hash.h"
#include "ext/hash/php_hash_sha.h"
#include "ext/random/php_random_csprng.h"

#include "SAPI.h"

#ifdef ZEND_WIN32
# include "Zend/zend_system_id.h"
#else
# include <signal.h>
#endif

#if defined(__APPLE__) || defined(__FreeBSD__)
# include <sys/sysctl.h>
# ifdef __FreeBSD__
#  include <sys/param.h>
#  include <sys/user.h>
# endif
#endif

#define PHP_UCACHE_AUTO_ENTRY_BYTES		4096U

#define PHP_UCACHE_ENTRY_LOCK_WAIT_TIMEOUT_US	(10U * 1000U * 1000U)
#define PHP_UCACHE_ENTRY_LOCK_RETRY_INTERVAL_US	10000U

#define PHP_UCACHE_READER_OWNER_RECLAIMING UINT64_MAX

#define PHP_UCACHE_READER_DRAIN_SPIN		1024U
#define PHP_UCACHE_READER_DRAIN_TIMEOUT_US	10000U

#ifdef PHP_UCACHE_HAVE_BOUNDARY_MMAP
# define PHP_UCACHE_BOUNDARY_DIR_PREFIX		".PhpUserCacheBnd."
# define PHP_UCACHE_BOUNDARY_SALT_NAME		"salt"
# define PHP_UCACHE_BOUNDARY_SEGMENT_SUFFIX	".seg"
# define PHP_UCACHE_BOUNDARY_LOCK_SUFFIX	".lock"
#endif

#ifndef ZEND_WIN32
# define PHP_UCACHE_PREALLOCATE_CHUNK	65536U
#endif

typedef enum {
	PHP_UCACHE_ENTRY_LOCK_RELEASE_DROP,
	PHP_UCACHE_ENTRY_LOCK_RELEASE_PRESERVE_LEASES
} php_ucache_entry_lock_release_mode_t;

typedef struct {
	char *key;
	uint32_t key_len;
	zend_ulong hash;
	uint64_t owner_pid;
	uint64_t owner_start_time;
	uint64_t owner_token;
	uint64_t expires_at;
} php_ucache_recovered_entry_lock_t;

typedef struct {
	php_ucache_ctx_t *ctx;
	uint64_t owner_pid;
	uint64_t owner_token;
	zend_long lease;
	bool preserve_lease;
} php_ucache_entry_lock_t;

typedef struct {
	zend_string *key;
	php_ucache_entry_lock_t *lock;
} php_ucache_entry_lock_release_pair_t;

typedef struct {
	uint32_t (*sleep_us)(uint32_t interval_us);
	uint64_t (*process_start_time_token)(uint64_t pid);
	bool (*process_has_exited)(uint64_t pid);
	int (*alloc_error_code)(void);
	void (*log_alloc_failure)(const char *error_in, int error_code);
} php_ucache_platform_ops_t;

/* Callers must hold no lock; each op leaves UC_G(lock_held) consistent.
 * Dispatch happens only while storage->lock_initialized is true. */
struct _php_ucache_lock_ops {
	bool (*rlock)(php_ucache_storage_t *storage);
	bool (*wlock)(php_ucache_storage_t *storage);
	void (*unlock)(php_ucache_storage_t *storage);
};

static bool ucache_capacity_clamp_warned = false;

#ifdef PHP_UCACHE_HAVE_BOUNDARY_MMAP
static bool ucache_boundary_dir_failure_logged = false;
static bool ucache_boundary_boot_token_loaded = false;
static uint64_t ucache_boundary_boot_token_memo = 0;
#endif

static bool ucache_reader_slots_enabled = false;

#ifdef ZTS
static php_ucache_startup_lock ucache_startup_storage_lock_state =
	PHP_UCACHE_STARTUP_LOCK_INITIALIZER
;
#endif

static const php_ucache_platform_ops_t *ucache_platform_ops(void);

static zend_always_inline bool ucache_requires_pre_request_storage(void)
{
	if (sapi_module.name == NULL) {
		return false;
	}

	return strcmp(sapi_module.name, "fpm-fcgi") == 0 ||
		strcmp(sapi_module.name, "apache2handler") == 0 ||
		strcmp(sapi_module.name, "cli-server") == 0
	;
}

static zend_always_inline bool ucache_is_opted_in(void)
{
	return php_ucache_runtime_opted_in;
}

static zend_always_inline bool ucache_is_disabled_for_sapi(void)
{
	if (!UC_G(enable)) {
		return true;
	}

	return !UC_G(enable_cli) &&
		sapi_module.name != NULL &&
		(strcmp(sapi_module.name, "cli") == 0 || strcmp(sapi_module.name, "phpdbg") == 0)
	;
}

static zend_always_inline void ucache_set_unavailable(php_ucache_reason_t reason)
{
	php_ucache_runtime_t *runtime = php_ucache_active_runtime();

	runtime->available = false;
	runtime->failure_reason = reason;
}

static zend_always_inline void ucache_set_available(void)
{
	php_ucache_runtime_t *runtime = php_ucache_active_runtime();

	runtime->available = true;
	runtime->failure_reason = PHP_UCACHE_REASON_NONE;
}

static zend_always_inline HashTable **ucache_entry_lock_table_ptr(void)
{
	return &UC_G(entry_lock_table);
}

static zend_always_inline uint32_t ucache_entry_lock_table_index(
		const php_ucache_header_t *header,
		zend_ulong hash)
{
	/* entry_lock_capacity is a power of two. */
	return (uint32_t) (hash & (header->entry_lock_capacity - 1));
}

static zend_always_inline uint32_t ucache_used_end_offset_locked(const php_ucache_header_t *header)
{
	return header->data_offset + header->next_free;
}

static zend_always_inline void ucache_block_mark_free(php_ucache_block_t *block)
{
	block->flags |= PHP_UCACHE_BLOCK_FREE;
}

static zend_always_inline bool ucache_entry_lock_record_key_matches(
		const php_ucache_entry_lock_record_t *record,
		zend_string *key,
		zend_ulong hash)
{
	return record->state == PHP_UCACHE_ENTRY_LOCK_USED &&
		record->hash == hash &&
		record->key_len == ZSTR_LEN(key) &&
		memcmp(php_ucache_ptr(record->key_offset), ZSTR_VAL(key), ZSTR_LEN(key)) == 0
	;
}

static zend_always_inline php_ucache_entry_lock_t *ucache_find_local_entry_lock(
		const php_ucache_ctx_t *ctx,
		zend_string *key)
{
	HashTable **locks_ptr = ucache_entry_lock_table_ptr();
	php_ucache_entry_lock_t *lock;

	if (*locks_ptr == NULL) {
		return NULL;
	}

	lock = zend_hash_find_ptr(*locks_ptr, key);
	if (lock == NULL || lock->ctx != ctx) {
		return NULL;
	}

	return lock;
}

/* Do not cache failures or values inherited across fork. */
static zend_always_inline uint64_t ucache_cached_self_start_time_token(uint64_t self_pid)
{
	if (UC_G(self_start_time_pid) != self_pid || UC_G(self_start_time_token) == 0) {
		UC_G(self_start_time_pid) = self_pid;
		UC_G(self_start_time_token) = ucache_platform_ops()->process_start_time_token(self_pid);
	}

	return UC_G(self_start_time_token);
}

#ifdef ZTS
static zend_always_inline bool ucache_storage_holds_header(
		const php_ucache_storage_t *storage,
		const php_ucache_header_t *header)
{
	return storage->initialized &&
		storage->segment_count == 1 &&
		storage->segments[0]->p == (const void *) header
	;
}
#endif

static zend_always_inline bool ucache_atomic_cas_64(uint64_t *target, uint64_t expected, uint64_t desired)
{
	return PHP_UCACHE_ATOMIC_CAS_64(target, expected, desired);
}

static zend_always_inline uint32_t ucache_atomic_load_32(const uint32_t *target)
{
	return PHP_UCACHE_ATOMIC_LOAD_32(target);
}

static zend_always_inline void ucache_atomic_store_32(uint32_t *target, uint32_t value)
{
	PHP_UCACHE_ATOMIC_STORE_32(target, value);
}

static zend_always_inline bool ucache_atomic_cas_32(uint32_t *target, uint32_t expected, uint32_t desired)
{
	return PHP_UCACHE_ATOMIC_CAS_32(target, expected, desired);
}

static zend_always_inline void ucache_atomic_fence_seq_cst(void)
{
	PHP_UCACHE_ATOMIC_FENCE_SEQ_CST();
}

/* Announce fences so the odd (write-in-progress) sequence is visible before the
 * payload writes; publish needs no fence because readers re-check the sequence
 * after copying. */
static zend_always_inline void ucache_seq_announce(uint64_t *seq, uint64_t value)
{
	php_ucache_atomic_store_64(seq, value);
	ucache_atomic_fence_seq_cst();
}

static zend_always_inline void ucache_seq_publish(uint64_t *seq, uint64_t value)
{
	php_ucache_atomic_store_64(seq, value);
}

static zend_always_inline bool ucache_header_is_initialized_acquire(void)
{
	php_ucache_header_t *header = php_ucache_header_ptr();

	if (header == NULL) {
		return false;
	}

	if (ucache_atomic_load_32(&header->magic) != PHP_UCACHE_MAGIC) {
		return false;
	}

	php_ucache_atomic_fence_acquire();

	return ucache_atomic_load_32(&header->version) == PHP_UCACHE_VERSION;
}

static inline bool ucache_zts_lock_init(php_ucache_storage_t *storage)
{
#ifdef ZTS
	storage->zts_lock = tsrm_mutex_alloc();

	return storage->zts_lock != NULL;
#else
	(void) storage;

	return true;
#endif
}

static inline void ucache_zts_lock_destroy(php_ucache_storage_t *storage)
{
#ifdef ZTS
	tsrm_mutex_free(storage->zts_lock);
	storage->zts_lock = NULL;
#else
	(void) storage;
#endif
}

static inline bool ucache_zts_lock(php_ucache_storage_t *storage)
{
#ifdef ZTS
	return tsrm_mutex_lock(storage->zts_lock) == 0;
#else
	(void) storage;

	return true;
#endif
}

static inline void ucache_zts_unlock(php_ucache_storage_t *storage)
{
#ifdef ZTS
	tsrm_mutex_unlock(storage->zts_lock);
#else
	(void) storage;
#endif
}

static inline uint32_t ucache_sleep_entry_lock_retry_interval(void)
{
	return ucache_platform_ops()->sleep_us(PHP_UCACHE_ENTRY_LOCK_RETRY_INTERVAL_US);
}

static inline bool ucache_startup_storage_is_complete(void)
{
#ifdef ZTS
	return ucache_atomic_load_32(&php_ucache_active_context()->storage.startup_complete) != 0;
#else
	return false;
#endif
}

static inline void ucache_startup_storage_lock(void)
{
#ifdef ZTS
# ifdef ZEND_WIN32
	AcquireSRWLockExclusive(&ucache_startup_storage_lock_state);
# else
	pthread_mutex_lock(&ucache_startup_storage_lock_state);
# endif /* ZEND_WIN32 */
#endif
}

static inline void ucache_startup_storage_unlock(void)
{
#ifdef ZTS
# ifdef ZEND_WIN32
	ReleaseSRWLockExclusive(&ucache_startup_storage_lock_state);
# else
	pthread_mutex_unlock(&ucache_startup_storage_lock_state);
# endif /* ZEND_WIN32 */
#endif
}

#ifdef ZEND_WIN32
static inline void ucache_win32_set_segment(
		php_ucache_win32_segment_t *segment,
		HANDLE memfile,
		void *mapping_base,
		size_t requested_size)
{
	segment->memfile = memfile;
	segment->mapping_base = mapping_base;
	segment->segment.p = mapping_base;
	segment->segment.size = requested_size;
}
#endif /* ZEND_WIN32 */

#ifdef ZEND_WIN32
static uint32_t ucache_win32_sleep_us(uint32_t interval_us)
{
	DWORD interval_ms = interval_us / 1000U;

	if (interval_ms == 0) {
		interval_ms = 1;
	}

	Sleep(interval_ms);

	return (uint32_t) interval_ms * 1000U;
}

static uint64_t ucache_win32_process_start_time_token(uint64_t pid)
{
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
}

static bool ucache_win32_process_has_exited(uint64_t pid)
{
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
}

static int ucache_win32_alloc_error_code(void)
{
	return (int) GetLastError();
}

static void ucache_win32_log_alloc_failure(const char *error_in, int error_code)
{
	char *msg = php_win32_error_to_msg(error_code);

	php_error_docref(
		NULL, E_WARNING,
		"Cache: shared memory initialization failed: %s: %s (%d)",
		error_in != NULL ? error_in : "unknown",
		msg,
		error_code
	);

	php_win32_error_msg_free(msg);
}

static const php_ucache_platform_ops_t ucache_platform_ops_table = {
	ucache_win32_sleep_us,
	ucache_win32_process_start_time_token,
	ucache_win32_process_has_exited,
	ucache_win32_alloc_error_code,
	ucache_win32_log_alloc_failure
};
#else /* !ZEND_WIN32 */
static uint32_t ucache_posix_sleep_us(uint32_t interval_us)
{
#ifdef HAVE_USLEEP
	usleep(interval_us);
#endif

	return interval_us;
}

static uint64_t ucache_posix_process_start_time_token(uint64_t pid)
{
#if defined(__linux__)
	const char *p;
	ssize_t stat_len;
	uint64_t start_time = 0;
	uint32_t field;
	int fd;
	char path[64], stat_buf[1024];

	snprintf(path, sizeof(path), "/proc/%" PRIu64 "/stat", pid);
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

	if (sscanf(p, " %" SCNu64, &start_time) != 1) {
		return 0;
	}

	return start_time;
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
#endif /* defined(__linux__) */
}

static bool ucache_posix_process_has_exited(uint64_t pid)
{
	return kill((pid_t) pid, 0) == -1 && errno == ESRCH;
}

static int ucache_posix_alloc_error_code(void)
{
	return errno;
}

static void ucache_posix_log_alloc_failure(const char *error_in, int error_code)
{
	php_error_docref(
		NULL, E_WARNING,
		"Cache: shared memory initialization failed: %s: %s (%d)",
		error_in != NULL ? error_in : "unknown",
		strerror(error_code),
		error_code
	);
}

/* Filesystems without native preallocation commit blocks only when written. */
static bool ucache_posix_zero_fill_fd(int fd, size_t size)
{
	static const uint8_t zeros[PHP_UCACHE_PREALLOCATE_CHUNK];
	size_t offset = 0, chunk;
	ssize_t written;

	if (ftruncate(fd, (off_t) size) != 0) {
		return false;
	}

	while (offset < size) {
		chunk = size - offset < sizeof(zeros) ? size - offset : sizeof(zeros);
		written = pwrite(fd, zeros, chunk, (off_t) offset);
		if (written < 0) {
			if (errno == EINTR) {
				continue;
			}

			return false;
		}

		if (written == 0) {
			errno = EIO;

			return false;
		}

		offset += (size_t) written;
	}

	return true;
}

static const php_ucache_platform_ops_t ucache_platform_ops_table = {
	ucache_posix_sleep_us,
	ucache_posix_process_start_time_token,
	ucache_posix_process_has_exited,
	ucache_posix_alloc_error_code,
	ucache_posix_log_alloc_failure
};
#endif /* ZEND_WIN32 */

static const php_ucache_platform_ops_t *ucache_platform_ops(void)
{
	return &ucache_platform_ops_table;
}

static bool ucache_capacity_is_prime(uint32_t candidate)
{
	uint32_t i;

	if (candidate < 4) {
		return candidate > 1;
	}

	if (candidate % 2 == 0 || candidate % 3 == 0) {
		return false;
	}

	for (i = 5; (uint64_t) i * i <= candidate; i += 6) {
		if (candidate % i == 0 || candidate % (i + 2) == 0) {
			return false;
		}
	}

	return true;
}

static uint32_t ucache_next_prime(uint32_t candidate)
{
	while (!ucache_capacity_is_prime(candidate)) {
		candidate++;
	}

	return candidate;
}

static uint32_t ucache_prev_prime(uint32_t candidate)
{
	while (candidate > PHP_UCACHE_MIN_CAPACITY && !ucache_capacity_is_prime(candidate)) {
		candidate--;
	}

	return candidate;
}

static uint32_t ucache_calculate_entry_lock_capacity(uint32_t capacity)
{
	uint32_t want = capacity / 16,
		lock_capacity = PHP_UCACHE_ENTRY_LOCK_MAX_CAPACITY
	;

	while (lock_capacity > PHP_UCACHE_ENTRY_LOCK_MIN_CAPACITY && lock_capacity > want) {
		lock_capacity >>= 1;
	}

	return lock_capacity;
}

static uint32_t ucache_calculate_intern_capacity(uint32_t capacity)
{
	uint32_t want = capacity / 4, intern_capacity = PHP_UCACHE_INTERN_MIN_CAPACITY;

	while (intern_capacity < want && intern_capacity < PHP_UCACHE_INTERN_MAX_CAPACITY) {
		intern_capacity <<= 1;
	}

	return intern_capacity;
}

static uint32_t ucache_calculate_capacity(size_t size, bool *clamped)
{
	uint64_t hint, want, max_capacity;
	uint32_t capacity, lock_capacity, next_lock_capacity;
	size_t reserve, lock_bytes;

	*clamped = false;

	hint = UC_G(entries_hint) > 0
		? (uint64_t) UC_G(entries_hint)
		: (uint64_t) (size / PHP_UCACHE_AUTO_ENTRY_BYTES)
	;

	if (hint < PHP_UCACHE_MIN_CAPACITY) {
		hint = PHP_UCACHE_MIN_CAPACITY;
	}

	want = hint + (hint + 2) / 3;
	capacity = ucache_next_prime((uint32_t) want);

	reserve = size / 2;
	lock_capacity = ucache_calculate_entry_lock_capacity(capacity);

	for (;;) {
		lock_bytes = (size_t) lock_capacity * sizeof(php_ucache_entry_lock_record_t);
		if (reserve <= sizeof(php_ucache_header_t) + lock_bytes) {
			return PHP_UCACHE_MIN_CAPACITY;
		}

		/* Three extra bytes per slot over-reserve the occupancy bitmap
		 * (1/8 byte) and the intern slot table (at most 2 bytes). */
		max_capacity =
			(reserve - sizeof(php_ucache_header_t) - lock_bytes)
			/ (sizeof(php_ucache_entry_t) + sizeof(uint32_t) + 3)
		;
		if (capacity > max_capacity) {
			capacity = ucache_prev_prime(
				max_capacity > PHP_UCACHE_MIN_CAPACITY
					? (uint32_t) max_capacity
					: PHP_UCACHE_MIN_CAPACITY
			);

			*clamped = UC_G(entries_hint) > 0;
		}

		next_lock_capacity = ucache_calculate_entry_lock_capacity(capacity);
		if (next_lock_capacity == lock_capacity) {
			break;
		}

		lock_capacity = next_lock_capacity;
	}

	return capacity;
}

#if defined(PHP_UCACHE_HAVE_ANON_MMAP) || defined(PHP_UCACHE_HAVE_BOUNDARY_MMAP)
static bool ucache_wrap_mapped_segment(
		void *mapping,
		size_t requested_size,
		php_ucache_shm_segment_t ***shared_segments_p,
		uint32_t *shared_segments_count,
		const char **error_in)
{
	php_ucache_shm_segment_t *segment;

	*shared_segments_count = 1;
	*shared_segments_p = (php_ucache_shm_segment_t **) pecalloc(1, sizeof(php_ucache_shm_segment_t *) + sizeof(php_ucache_shm_segment_t), true);

	segment = (php_ucache_shm_segment_t *) ((char *) *shared_segments_p + sizeof(php_ucache_shm_segment_t *));
	(*shared_segments_p)[0] = segment;

	segment->p = mapping;
	segment->size = requested_size;

	return true;
}

static void ucache_munmap_detach_segment(php_ucache_shm_segment_t *shared_segment)
{
	munmap(shared_segment->p, shared_segment->size);
}

#endif /* defined(PHP_UCACHE_HAVE_ANON_MMAP) || defined(PHP_UCACHE_HAVE_BOUNDARY_MMAP) */

#ifdef PHP_UCACHE_HAVE_ANON_MMAP
static bool ucache_mmap_create_segments(
		size_t requested_size,
		php_ucache_shm_segment_t ***shared_segments_p,
		uint32_t *shared_segments_count,
		const char **error_in)
{
	void *mapping;

	mapping = mmap(NULL, requested_size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (mapping == MAP_FAILED) {
		*error_in = "mmap";

		return false;
	}

	if (!ucache_wrap_mapped_segment(mapping, requested_size, shared_segments_p, shared_segments_count, error_in)) {
		munmap(mapping, requested_size);

		return false;
	}

	return true;
}
#endif /* PHP_UCACHE_HAVE_ANON_MMAP */

#ifdef PHP_UCACHE_HAVE_BOUNDARY_MMAP
static void ucache_shared_boundary_digest(
		const php_ucache_ctx_t *ctx,
		size_t requested_size,
		uint8_t digest[32])
{
	const char *identity;
	PHP_SHA256_CTX sha_ctx;
	uint32_t lock_capacity;
	char prefix[128];
	size_t identity_len, update_len = 0;
	int prefix_len;
	bool clamped;

	ZEND_ASSERT(ctx->boundary_identity != NULL);

	identity = ctx->boundary_identity;
	identity_len = ctx->boundary_identity_len;
	/* The entries hint and the derived lock capacity participate in the
	 * digest because they determine the table layout: processes with
	 * mismatched layouts must fail identity instead of reformatting each
	 * other's segment. */
	lock_capacity = ucache_calculate_entry_lock_capacity(
		ucache_calculate_capacity(requested_size, &clamped)
	);

	prefix_len = snprintf(
		prefix,
		sizeof(prefix),
		"PhpUserCache.boundary-mmap|%u|%zu|" ZEND_LONG_FMT "|%u|%zu|",
		PHP_UCACHE_VERSION,
		requested_size,
		UC_G(entries_hint),
		lock_capacity,
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

	ZEND_ASSERT(ctx->storage.boundary_salt_loaded);

	PHP_SHA256Update(&sha_ctx, ctx->storage.boundary_salt, sizeof(ctx->storage.boundary_salt));
	PHP_SHA256Final(digest, &sha_ctx);
}

static void ucache_shared_boundary_digest_to_identity(
		const uint8_t digest[32],
		uint64_t *identity_high,
		uint32_t *identity_low)
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

static void ucache_shared_boundary_identity(
		const php_ucache_ctx_t *ctx,
		size_t requested_size,
		uint64_t *identity_high,
		uint32_t *identity_low)
{
	uint8_t digest[32];

	ucache_shared_boundary_digest(ctx, requested_size, digest);
	ucache_shared_boundary_digest_to_identity(digest, identity_high, identity_low);
}

static void ucache_shared_boundary_object_name(char *buf, size_t buf_size, size_t requested_size, const char *suffix)
{
	uint64_t identity_high;
	uint32_t identity_low;

	ucache_shared_boundary_identity(
		php_ucache_active_context(),
		requested_size,
		&identity_high,
		&identity_low
	);

	snprintf(buf, buf_size, "%016" PRIx64 "%08" PRIx32 "%s", identity_high, identity_low, suffix);
}

static bool ucache_shared_boundary_fd_is_trusted(int fd, struct stat *st)
{
	if (fstat(fd, st) != 0) {
		return false;
	}

	return st->st_uid == geteuid() && (st->st_mode & 0077) == 0;
}

/* Zero when the platform offers no boot identity (stale images then go
 * undetected). */
static uint64_t ucache_shared_boundary_boot_token(void)
{
#if defined(__linux__)
	ssize_t len;
	uint32_t digits = 0;
	int fd;
	const char *p;
	char buf[64], hex[17];
#elif defined(__APPLE__) || defined(__FreeBSD__)
	struct timeval boottime;
	size_t size = sizeof(boottime);
	int mib[2] = { CTL_KERN, KERN_BOOTTIME };
#endif

	if (ucache_boundary_boot_token_loaded) {
		return ucache_boundary_boot_token_memo;
	}

#if defined(__linux__)
	/* boot_id stays fixed for the whole boot, unlike /proc/stat btime which
	 * follows wall-clock steps. */
	fd = open("/proc/sys/kernel/random/boot_id", O_RDONLY | O_CLOEXEC);
	if (fd >= 0) {
		len = read(fd, buf, sizeof(buf) - 1);
		close(fd);
		if (len > 0) {
			buf[len] = '\0';
			for (p = buf; *p != '\0' && digits < 16; p++) {
				if (*p == '-') {
					continue;
				}

				if (!isxdigit((unsigned char) *p)) {
					break;
				}

				hex[digits++] = *p;
			}

			hex[digits] = '\0';

			if (digits == 16) {
				ucache_boundary_boot_token_memo = (uint64_t) strtoull(hex, NULL, 16);
			}
		}
	}
#elif defined(__APPLE__) || defined(__FreeBSD__)
	if (sysctl(mib, 2, &boottime, &size, NULL, 0) == 0 && size == sizeof(boottime)) {
		ucache_boundary_boot_token_memo = (uint64_t) boottime.tv_sec;
	}
#endif /* defined(__linux__) */

	ucache_boundary_boot_token_loaded = true;

	return ucache_boundary_boot_token_memo;
}

/* Caller holds the fcntl bootstrap lock and has not yet switched to the
 * header's own lock: an image from an earlier boot may be torn or hold a
 * robust mutex whose owner the kernel never saw die. Clearing the magic
 * makes php_ucache_header_init_locked() format it afresh instead. */
static bool ucache_shared_boundary_discard_stale_image_locked(void)
{
	php_ucache_header_t *header = php_ucache_header_ptr();

	if (header->boot_token == ucache_shared_boundary_boot_token()) {
		return false;
	}

	ucache_atomic_store_32(&header->magic, 0);
	ucache_atomic_fence_seq_cst();

	return true;
}

static void ucache_shared_boundary_log_dir_failure(const char *dir_path, const char *reason)
{
	char message[MAXPATHLEN + 192];

	if (ucache_boundary_dir_failure_logged) {
		return;
	}

	ucache_boundary_dir_failure_logged = true;

	snprintf(
		message,
		sizeof(message),
		"UserCache boundary directory %s is unusable (%s); it must be a directory owned by uid %lu "
		"with mode 0700 (see user_cache.lockfile_path)",
		dir_path,
		reason,
		(unsigned long) geteuid()
	);

	php_log_err(message);
}

static int ucache_shared_boundary_open_private_dir(char *dir_path, size_t dir_path_size, const char **error_in)
{
	struct stat st;
	int fd, saved_errno, len;

	len = snprintf(
		dir_path,
		dir_path_size,
		"%s/" PHP_UCACHE_BOUNDARY_DIR_PREFIX "%lu",
		UC_G(lockfile_path),
		(unsigned long) geteuid()
	);
	if (len < 0 || (size_t) len >= dir_path_size) {
		errno = ENAMETOOLONG;
		*error_in = "lockfile_path";

		return -1;
	}

	if (mkdir(dir_path, 0700) != 0 && errno != EEXIST) {
		saved_errno = errno;
		ucache_shared_boundary_log_dir_failure(dir_path, strerror(saved_errno));
		errno = saved_errno;

		*error_in = "mkdir";

		return -1;
	}

	fd = open(dir_path, O_RDONLY | O_DIRECTORY | O_NOFOLLOW | O_CLOEXEC);
	if (fd < 0) {
		saved_errno = errno;
		ucache_shared_boundary_log_dir_failure(dir_path, strerror(saved_errno));
		errno = saved_errno;

		*error_in = "open directory";

		return -1;
	}

	if (!ucache_shared_boundary_fd_is_trusted(fd, &st) || !S_ISDIR(st.st_mode)) {
		close(fd);
		ucache_shared_boundary_log_dir_failure(dir_path, "not a private directory owned by this uid");
		errno = EACCES;

		*error_in = "directory ownership";

		return -1;
	}

	return fd;
}

static bool ucache_shared_boundary_read_salt(
		int dir_fd,
		const char *dir_path,
		uint8_t *salt,
		bool *missing,
		const char **error_in)
{
	struct stat st;
	size_t got = 0;
	ssize_t n;
	int fd;

	*missing = false;

	fd = openat(dir_fd, PHP_UCACHE_BOUNDARY_SALT_NAME, O_RDONLY | O_NOFOLLOW | O_CLOEXEC);
	if (fd < 0) {
		*missing = errno == ENOENT;
		*error_in = "open salt";

		return false;
	}

	if (!ucache_shared_boundary_fd_is_trusted(fd, &st) ||
		!S_ISREG(st.st_mode) ||
		st.st_size != PHP_UCACHE_BOUNDARY_SALT_SIZE
	) {
		close(fd);
		ucache_shared_boundary_log_dir_failure(
			dir_path,
			"its " PHP_UCACHE_BOUNDARY_SALT_NAME " file is not a private regular file of the expected size owned by this uid"
		);
		errno = EACCES;

		*error_in = "salt ownership";

		return false;
	}

	while (got < PHP_UCACHE_BOUNDARY_SALT_SIZE) {
		n = read(fd, salt + got, PHP_UCACHE_BOUNDARY_SALT_SIZE - got);
		if (n < 0 && errno == EINTR) {
			continue;
		}

		if (n <= 0) {
			close(fd);
			if (n == 0) {
				errno = EIO;
			}

			*error_in = "read salt";

			return false;
		}

		got += (size_t) n;
	}

	close(fd);

	return true;
}

static bool ucache_shared_boundary_create_salt(int dir_fd, const char **error_in)
{
	uint8_t salt[PHP_UCACHE_BOUNDARY_SALT_SIZE];
	size_t written = 0;
	ssize_t n;
	char tmp_name[64];
	int fd, saved_errno;

	snprintf(tmp_name, sizeof(tmp_name), PHP_UCACHE_BOUNDARY_SALT_NAME ".%lu", (unsigned long) getpid());

	fd = openat(dir_fd, tmp_name, O_WRONLY | O_CREAT | O_EXCL | O_NOFOLLOW | O_CLOEXEC, 0600);
	if (fd < 0 && errno == EEXIST) {
		unlinkat(dir_fd, tmp_name, 0);
		fd = openat(dir_fd, tmp_name, O_WRONLY | O_CREAT | O_EXCL | O_NOFOLLOW | O_CLOEXEC, 0600);
	}

	if (fd < 0) {
		*error_in = "create salt";

		return false;
	}

	if (php_random_bytes_silent(salt, sizeof(salt)) == FAILURE) {
		close(fd);
		unlinkat(dir_fd, tmp_name, 0);
		errno = EIO;

		*error_in = "php_random_bytes";

		return false;
	}

	while (written < sizeof(salt)) {
		n = write(fd, salt + written, sizeof(salt) - written);
		if (n < 0 && errno == EINTR) {
			continue;
		}

		if (n <= 0) {
			saved_errno = n == 0 ? EIO : errno;
			close(fd);
			unlinkat(dir_fd, tmp_name, 0);
			errno = saved_errno;

			*error_in = "write salt";

			return false;
		}

		written += (size_t) n;
	}

	close(fd);

	if (linkat(dir_fd, tmp_name, dir_fd, PHP_UCACHE_BOUNDARY_SALT_NAME, 0) != 0 && errno != EEXIST) {
		saved_errno = errno;
		unlinkat(dir_fd, tmp_name, 0);
		errno = saved_errno;

		*error_in = "linkat";

		return false;
	}

	unlinkat(dir_fd, tmp_name, 0);

	return true;
}

static bool ucache_shared_boundary_load_salt(int dir_fd, const char *dir_path, const char **error_in)
{
	php_ucache_storage_t *storage = &php_ucache_active_context()->storage;
	uint32_t attempt;
	bool missing, loaded = false;

	if (storage->boundary_salt_loaded) {
		return true;
	}

	for (attempt = 0; attempt < 2 && !loaded; attempt++) {
		loaded = ucache_shared_boundary_read_salt(dir_fd, dir_path, storage->boundary_salt, &missing, error_in);
		if (loaded || !missing || !ucache_shared_boundary_create_salt(dir_fd, error_in)) {
			break;
		}
	}

	storage->boundary_salt_loaded = loaded;

	return loaded;
}

/* Published (linkat, no replace) only once fully preallocated: attachers never
 * see a partial file, and a full filesystem fails here with ENOSPC instead of
 * SIGBUS on first touch. */
static int ucache_shared_boundary_open_segment_fd(
		int dir_fd,
		const char *segment_name,
		size_t requested_size,
		const char **error_in)
{
	struct stat st;
	uint32_t attempt;
	int fd, saved_errno;
	char tmp_name[96];

	for (attempt = 0; attempt < 2; attempt++) {
		fd = openat(dir_fd, segment_name, O_RDWR | O_NOFOLLOW | O_CLOEXEC);
		if (fd >= 0) {
			if (!ucache_shared_boundary_fd_is_trusted(fd, &st) ||
				!S_ISREG(st.st_mode) ||
				st.st_size < 0 ||
				(uint64_t) st.st_size != (uint64_t) requested_size
			) {
				close(fd);
				errno = EACCES;
				*error_in = "segment file";

				return -1;
			}

			return fd;
		}

		if (errno != ENOENT) {
			*error_in = "open segment";

			return -1;
		}

		snprintf(tmp_name, sizeof(tmp_name), "%s.%lu.tmp", segment_name, (unsigned long) getpid());

		fd = openat(dir_fd, tmp_name, O_RDWR | O_CREAT | O_EXCL | O_NOFOLLOW | O_CLOEXEC, 0600);
		if (fd < 0 && errno == EEXIST) {
			unlinkat(dir_fd, tmp_name, 0);
			fd = openat(dir_fd, tmp_name, O_RDWR | O_CREAT | O_EXCL | O_NOFOLLOW | O_CLOEXEC, 0600);
		}

		if (fd < 0) {
			*error_in = "create segment";

			return -1;
		}

		if (!php_ucache_preallocate_fd(fd, requested_size)) {
			saved_errno = errno;
			close(fd);
			unlinkat(dir_fd, tmp_name, 0);
			errno = saved_errno;
			*error_in = "preallocate segment";

			return -1;
		}

		if (linkat(dir_fd, tmp_name, dir_fd, segment_name, 0) == 0) {
			unlinkat(dir_fd, tmp_name, 0);

			return fd;
		}

		saved_errno = errno;
		close(fd);
		unlinkat(dir_fd, tmp_name, 0);
		if (saved_errno != EEXIST) {
			errno = saved_errno;
			*error_in = "publish segment";

			return -1;
		}
	}

	errno = EEXIST;
	*error_in = "publish segment";

	return -1;
}

static bool ucache_shared_boundary_create_segments(
		size_t requested_size,
		php_ucache_shm_segment_t ***shared_segments_p,
		uint32_t *shared_segments_count,
		const char **error_in)
{
	int dir_fd, fd, saved_errno;
	char segment_name[64], dir_path[MAXPATHLEN];
	void *mapping;

	if (requested_size > (size_t) SSIZE_MAX) {
		*error_in = "size overflow";

		return false;
	}

	dir_fd = ucache_shared_boundary_open_private_dir(dir_path, sizeof(dir_path), error_in);
	if (dir_fd < 0) {
		return false;
	}

	fd = -1;
	if (ucache_shared_boundary_load_salt(dir_fd, dir_path, error_in)) {
		ucache_shared_boundary_object_name(
			segment_name,
			sizeof(segment_name),
			requested_size,
			PHP_UCACHE_BOUNDARY_SEGMENT_SUFFIX
		);

		fd = ucache_shared_boundary_open_segment_fd(dir_fd, segment_name, requested_size, error_in);
	}

	saved_errno = errno;
	close(dir_fd);
	errno = saved_errno;

	if (fd < 0) {
		return false;
	}

	mapping = mmap(NULL, requested_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	close(fd);
	if (mapping == MAP_FAILED) {
		*error_in = "mmap";

		return false;
	}

	if (!ucache_wrap_mapped_segment(mapping, requested_size, shared_segments_p, shared_segments_count, error_in)) {
		munmap(mapping, requested_size);

		return false;
	}

	return true;
}

static const php_ucache_shm_handler_entry_t *ucache_shared_boundary_handler_entry(void)
{
	static const php_ucache_shm_handlers_t handlers = {
		ucache_shared_boundary_create_segments,
		ucache_munmap_detach_segment
	};
	static const php_ucache_shm_handler_entry_t entry = {
		"boundary-mmap", &handlers
	};

	return &entry;
}
#endif /* PHP_UCACHE_HAVE_BOUNDARY_MMAP */

#ifdef ZEND_WIN32
static void ucache_win32_format_name(char *buf, size_t buf_size, const char *name, size_t unique_id)
{
	const char *sapi_name = sapi_module.name != NULL ? sapi_module.name : "";
	php_ucache_ctx_t *ctx = php_ucache_active_context();

	snprintf(
		buf,
		buf_size,
		"%s@%.32s@%.20s@%s@%zx",
		name,
		zend_system_id,
		sapi_name,
		ctx->lock_name,
		unique_id
	);
}

static bool ucache_win32_reattach_segment(
		php_ucache_win32_segment_t *segment,
		HANDLE memfile,
		size_t requested_size,
		const char **error_in)
{
	MEMORY_BASIC_INFORMATION info;
	void *mapping_base;

	mapping_base = MapViewOfFileEx(memfile, FILE_MAP_ALL_ACCESS, 0, 0, 0, NULL);
	if (mapping_base == NULL) {
		*error_in = "MapViewOfFileEx";

		return false;
	}

	if (VirtualQuery(mapping_base, &info, sizeof(info)) == 0 ||
		info.RegionSize < requested_size
	) {
		UnmapViewOfFile(mapping_base);
		*error_in = "VirtualQuery";

		return false;
	}

	ucache_win32_set_segment(segment, memfile, mapping_base, requested_size);

	return true;
}

static bool ucache_win32_create_segment(
		php_ucache_win32_segment_t *segment,
		const char *mapping_name,
		size_t requested_size,
		const char **error_in)
{
	HANDLE memfile;
	DWORD size_high, size_low;
	bool result;
	void *mapping_base;

#if defined(_WIN64)
	size_high = (DWORD) (requested_size >> 32);
	size_low = (DWORD) (requested_size & 0xffffffff);
#else /* !defined(_WIN64) */
	if (requested_size > UINT32_MAX) {
		*error_in = "size overflow";

		return false;
	}

	size_high = 0;
	size_low = (DWORD) requested_size;
#endif /* defined(_WIN64) */

	memfile = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE | SEC_COMMIT, size_high, size_low, mapping_name);
	if (memfile == NULL) {
		*error_in = "CreateFileMappingA";

		return false;
	}

	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		result = ucache_win32_reattach_segment(segment, memfile, requested_size, error_in);
		if (!result) {
			CloseHandle(memfile);
		}

		return result;
	}

	mapping_base = MapViewOfFileEx(memfile, FILE_MAP_ALL_ACCESS, 0, 0, 0, NULL);
	if (mapping_base == NULL) {
		CloseHandle(memfile);
		*error_in = "MapViewOfFileEx";

		return false;
	}

	ucache_win32_set_segment(segment, memfile, mapping_base, requested_size);

	return true;
}

static bool ucache_win32_create_segments(
		size_t requested_size,
		php_ucache_shm_segment_t ***shared_segments_p,
		uint32_t *shared_segments_count,
		const char **error_in)
{
	php_ucache_win32_segment_t *segment;
	HANDLE mutex = NULL, memfile = NULL;
	DWORD wait_result;
	bool result = false;
	char mapping_name[MAXPATHLEN], mutex_name[MAXPATHLEN];

	*shared_segments_count = 1;
	*shared_segments_p = (php_ucache_shm_segment_t **) pecalloc(
		1,
		sizeof(php_ucache_shm_segment_t *) + sizeof(php_ucache_win32_segment_t),
		true
	);

	segment = (php_ucache_win32_segment_t *) ((char *) *shared_segments_p + sizeof(php_ucache_shm_segment_t *));
	(*shared_segments_p)[0] = (php_ucache_shm_segment_t *) segment;

	ucache_win32_format_name(
		mapping_name,
		sizeof(mapping_name),
		PHP_UCACHE_WIN32_MAPPING_NAME,
		requested_size
	);
	ucache_win32_format_name(
		mutex_name,
		sizeof(mutex_name),
		PHP_UCACHE_WIN32_MAPPING_MUTEX_NAME,
		requested_size
	);

	mutex = CreateMutexA(NULL, FALSE, mutex_name);
	if (mutex == NULL) {
		*error_in = "CreateMutexA";

		goto bailout;
	}

	wait_result = WaitForSingleObject(mutex, INFINITE);
	if (wait_result != WAIT_OBJECT_0 && wait_result != WAIT_ABANDONED) {
		*error_in = "WaitForSingleObject";

		goto bailout;
	}

	memfile = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, mapping_name);
	if (memfile != NULL) {
		result = ucache_win32_reattach_segment(segment, memfile, requested_size, error_in);
		if (!result) {
			CloseHandle(memfile);
		}
	} else {
		result = ucache_win32_create_segment(segment, mapping_name, requested_size, error_in);
	}

	ReleaseMutex(mutex);
	CloseHandle(mutex);
	mutex = NULL;

	if (result) {
		return true;
	}

bailout:
	if (mutex != NULL) {
		CloseHandle(mutex);
	}

	pefree(*shared_segments_p, true);

	*shared_segments_p = NULL;
	*shared_segments_count = 0;

	return false;
}

static void ucache_win32_detach_segment(php_ucache_shm_segment_t *shared_segment)
{
	php_ucache_win32_segment_t *segment = (php_ucache_win32_segment_t *) shared_segment;

	if (segment->mapping_base != NULL) {
		UnmapViewOfFile(segment->mapping_base);
		segment->mapping_base = NULL;
	}

	if (segment->memfile != NULL) {
		CloseHandle(segment->memfile);
		segment->memfile = NULL;
	}
}

#endif /* ZEND_WIN32 */

#ifndef ZEND_WIN32
#ifdef PHP_UCACHE_HAVE_SHARED_MUTEX
static bool ucache_shared_mutex_init(php_ucache_header_t *header)
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

static bool ucache_shared_mutex_lock(php_ucache_storage_t *storage)
{
	php_ucache_header_t *header = php_ucache_header_ptr();
	int result;

	(void) storage;

	if (header == NULL) {
		return false;
	}

	result = pthread_mutex_lock(&header->global_shared_mutex.mutex);
	if (result == EOWNERDEAD) {
		if (pthread_mutex_consistent(&header->global_shared_mutex.mutex) != 0) {
			php_error_docref(NULL, E_WARNING, "Cache: unable to make the shared lock consistent after owner death");

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

static void ucache_shared_mutex_unlock(php_ucache_storage_t *storage)
{
	php_ucache_header_t *header = php_ucache_header_ptr();

	(void) storage;

	if (header != NULL) {
		pthread_mutex_unlock(&header->global_shared_mutex.mutex);
	}

	UC_G(lock_held) = false;
}

static const php_ucache_lock_ops_t ucache_shared_mutex_lock_ops = {
	ucache_shared_mutex_lock,
	ucache_shared_mutex_lock,
	ucache_shared_mutex_unlock
};
#endif /* PHP_UCACHE_HAVE_SHARED_MUTEX */

static bool ucache_fcntl_lock(php_ucache_storage_t *storage, short lock_type)
{
	struct flock mem_lock;

	if (!ucache_zts_lock(storage)) {
		return false;
	}

	mem_lock.l_type = lock_type;
	mem_lock.l_whence = SEEK_SET;
	mem_lock.l_start = 0;
	mem_lock.l_len = 1;

	while (fcntl(storage->lock_file, F_SETLKW, &mem_lock) == -1) {
		if (errno != EINTR) {
			ucache_zts_unlock(storage);

			return false;
		}
	}

	UC_G(lock_held) = true;

	return true;
}

static bool ucache_fcntl_rlock(php_ucache_storage_t *storage)
{
	return ucache_fcntl_lock(storage, F_RDLCK);
}

static bool ucache_fcntl_wlock(php_ucache_storage_t *storage)
{
	return ucache_fcntl_lock(storage, F_WRLCK);
}

static void ucache_fcntl_unlock(php_ucache_storage_t *storage)
{
	struct flock mem_unlock;

	mem_unlock.l_type = F_UNLCK;
	mem_unlock.l_whence = SEEK_SET;
	mem_unlock.l_start = 0;
	mem_unlock.l_len = 1;

	fcntl(storage->lock_file, F_SETLK, &mem_unlock);

	ucache_zts_unlock(storage);

	UC_G(lock_held) = false;
}

static const php_ucache_lock_ops_t ucache_fcntl_lock_ops = {
	ucache_fcntl_rlock,
	ucache_fcntl_wlock,
	ucache_fcntl_unlock
};

static const php_ucache_lock_ops_t *ucache_lock_ops_for_model(uint32_t lock_model)
{
#ifdef PHP_UCACHE_HAVE_SHARED_MUTEX
	if (lock_model == PHP_UCACHE_LOCK_MODEL_MUTEX) {
		return &ucache_shared_mutex_lock_ops;
	}
#else
	(void) lock_model;
#endif

	return &ucache_fcntl_lock_ops;
}

static bool ucache_create_lock(void)
{
	php_ucache_ctx_t *ctx = php_ucache_active_context();
	php_ucache_storage_t *storage = &ctx->storage;
#ifdef PHP_UCACHE_HAVE_BOUNDARY_MMAP
	const char *error_in;
	struct stat lock_st;
	int dir_fd;
	char dir_path[MAXPATHLEN];
#endif /* PHP_UCACHE_HAVE_BOUNDARY_MMAP */
	int val;

	if (storage->lock_initialized) {
		return true;
	}

	if (!ucache_zts_lock_init(storage)) {
		return false;
	}

	storage->lock_ops = &ucache_fcntl_lock_ops;

#ifdef PHP_UCACHE_HAVE_BOUNDARY_MMAP
	if (ctx->boundary_shared) {
		ucache_shared_boundary_object_name(
			storage->lockfile_name,
			sizeof(storage->lockfile_name),
			storage->size,
			PHP_UCACHE_BOUNDARY_LOCK_SUFFIX
		);

		dir_fd = ucache_shared_boundary_open_private_dir(dir_path, sizeof(dir_path), &error_in);
		if (dir_fd < 0) {
			goto bailout;
		}

		storage->lock_file = openat(dir_fd, storage->lockfile_name, O_RDWR | O_CREAT | O_CLOEXEC | O_NOFOLLOW, 0600);
		close(dir_fd);
		if (storage->lock_file < 0 ||
			!ucache_shared_boundary_fd_is_trusted(storage->lock_file, &lock_st)
		) {
			goto bailout;
		}

		storage->lock_initialized = true;

		return true;
	}
#endif /* PHP_UCACHE_HAVE_BOUNDARY_MMAP */

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
	if (storage->lock_file == -1 || fchmod(storage->lock_file, 0666) == -1) {
		goto bailout;
	}

	val = fcntl(storage->lock_file, F_GETFD, 0);
	val |= FD_CLOEXEC;

	fcntl(storage->lock_file, F_SETFD, val);
	unlink(storage->lockfile_name);

	storage->lock_initialized = true;

	return true;

bailout:
	if (storage->lock_file >= 0) {
		close(storage->lock_file);
		storage->lock_file = -1;
	}

	ucache_zts_lock_destroy(storage);

	return false;
}

static void ucache_destroy_lock(void)
{
	php_ucache_storage_t *storage = &php_ucache_active_context()->storage;

	if (!storage->lock_initialized) {
		return;
	}

	if (storage->lock_file >= 0) {
		close(storage->lock_file);
		storage->lock_file = -1;
	}

	ucache_zts_lock_destroy(storage);

	storage->lock_initialized = false;
}
#else /* ZEND_WIN32 */
static bool ucache_win32_open_lock_file_at(php_ucache_storage_t *storage, const char *dir, const char *base_name)
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

static bool ucache_win32_open_lock_file(php_ucache_ctx_t *ctx)
{
	php_ucache_storage_t *storage = &ctx->storage;
	DWORD tmp_path_w_len;
	wchar_t tmp_path_w[MAXPATHLEN];
	size_t tmp_path_len;
	char base_name[MAXPATHLEN], *tmp_path, *p;
	bool opened;

	ucache_win32_format_name(
		base_name,
		sizeof(base_name),
		PHP_UCACHE_WIN32_LOCK_FILE_NAME,
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

	tmp_path_w_len = GetTempPathW(MAXPATHLEN, tmp_path_w);
	if (tmp_path_w_len == 0 || tmp_path_w_len >= MAXPATHLEN) {
		return false;
	}

	tmp_path = php_win32_ioutil_conv_w_to_any(tmp_path_w, tmp_path_w_len, &tmp_path_len);
	if (tmp_path == NULL) {
		return false;
	}

	opened = ucache_win32_open_lock_file_at(storage, tmp_path, base_name);

	free(tmp_path);

	return opened;
}

static bool ucache_win32_lock_range(php_ucache_storage_t *storage, bool exclusive)
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

	if (exclusive) {
		flags |= LOCKFILE_EXCLUSIVE_LOCK;
	}

	return LockFileEx(handle, flags, 0, 1, 0, &overlapped) == TRUE;
}

static void ucache_win32_unlock_range(php_ucache_storage_t *storage)
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

	UnlockFileEx(handle, 0, 1, 0, &overlapped);
}

static bool ucache_win32_lock(php_ucache_storage_t *storage, bool exclusive)
{
	if (!ucache_zts_lock(storage)) {
		return false;
	}

	if (!ucache_win32_lock_range(storage, exclusive)) {
		ucache_zts_unlock(storage);

		return false;
	}

	UC_G(lock_held) = true;

	return true;
}

static bool ucache_win32_rlock(php_ucache_storage_t *storage)
{
	return ucache_win32_lock(storage, false);
}

static bool ucache_win32_wlock(php_ucache_storage_t *storage)
{
	return ucache_win32_lock(storage, true);
}

static void ucache_win32_unlock(php_ucache_storage_t *storage)
{
	ucache_win32_unlock_range(storage);

	ucache_zts_unlock(storage);

	UC_G(lock_held) = false;
}

static const php_ucache_lock_ops_t ucache_win32_lock_ops = {
	ucache_win32_rlock,
	ucache_win32_wlock,
	ucache_win32_unlock
};

static const php_ucache_lock_ops_t *ucache_lock_ops_for_model(uint32_t lock_model)
{
	(void) lock_model;

	return &ucache_win32_lock_ops;
}

static bool ucache_create_lock(void)
{
	php_ucache_ctx_t *ctx = php_ucache_active_context();
	php_ucache_storage_t *storage = &ctx->storage;

	if (storage->lock_initialized) {
		return true;
	}

	if (!ucache_zts_lock_init(storage)) {
		return false;
	}

	storage->lock_ops = &ucache_win32_lock_ops;

	if (!ucache_win32_open_lock_file(ctx)) {
		ucache_zts_lock_destroy(storage);

		return false;
	}

	storage->lock_initialized = true;

	return true;
}

static void ucache_destroy_lock(void)
{
	php_ucache_storage_t *storage = &php_ucache_active_context()->storage;

	if (!storage->lock_initialized) {
		return;
	}

	if (storage->lock_file >= 0) {
		php_win32_ioutil_close(storage->lock_file);
		storage->lock_file = -1;
	}

	ucache_zts_lock_destroy(storage);

	storage->lock_initialized = false;
}
#endif /* !ZEND_WIN32 */

static const php_ucache_shm_handler_entry_t *ucache_handler_table(void)
{
#ifdef PHP_UCACHE_HAVE_ANON_MMAP
	static const php_ucache_shm_handlers_t mmap_handlers = {
		ucache_mmap_create_segments,
		ucache_munmap_detach_segment
	};
#endif /* PHP_UCACHE_HAVE_ANON_MMAP */
#ifdef ZEND_WIN32
	static const php_ucache_shm_handlers_t win32_handlers = {
		ucache_win32_create_segments,
		ucache_win32_detach_segment
	};
#endif /* ZEND_WIN32 */
	static const php_ucache_shm_handler_entry_t handlers[] = {
#ifdef PHP_UCACHE_HAVE_ANON_MMAP
		{ "mmap", &mmap_handlers },
#endif /* PHP_UCACHE_HAVE_ANON_MMAP */
#ifdef PHP_UCACHE_USE_SHM
		{ "shm", &php_ucache_alloc_shm_handlers },
#endif /* PHP_UCACHE_USE_SHM */
#ifdef PHP_UCACHE_USE_SHM_OPEN
		{ "posix", &php_ucache_alloc_posix_handlers },
#endif /* PHP_UCACHE_USE_SHM_OPEN */
#ifdef ZEND_WIN32
		{ "win32", &win32_handlers },
#endif /* ZEND_WIN32 */
		{ NULL, NULL }
	};

	return handlers;
}

static void ucache_cleanup_segments(const php_ucache_shm_handlers_t *handler, php_ucache_shm_segment_t **segments, uint32_t segment_count)
{
	uint32_t i;

	if (!handler || !segments) {
		return;
	}

	for (i = 0; i < segment_count; i++) {
		if (segments[i]->p && segments[i]->p != (void *) -1) {
			handler->detach_segment(segments[i]);
		}
	}

	pefree(segments, true);
}

static bool ucache_entry_lock_owner_is_dead_impl(uint64_t owner_pid, uint64_t owner_start_time)
{
	const php_ucache_platform_ops_t *platform_ops = ucache_platform_ops();
	uint64_t current_start_time;

	if (platform_ops->process_has_exited(owner_pid)) {
		return true;
	}

	if (owner_start_time != 0) {
		current_start_time = platform_ops->process_start_time_token(owner_pid);
		if (current_start_time != 0 && current_start_time != owner_start_time) {
			return true;
		}
	}

	return false;
}

static bool ucache_entry_lock_owner_is_dead(uint64_t owner_pid, uint64_t owner_start_time)
{
	uint64_t now = (uint64_t) time(NULL);

	if (owner_pid == UC_G(entry_lock_owner_probe_pid) &&
		owner_start_time == UC_G(entry_lock_owner_probe_start_time) &&
		now == UC_G(entry_lock_owner_probe_at)
	) {
		return UC_G(entry_lock_owner_probe_dead);
	}

	UC_G(entry_lock_owner_probe_dead) = ucache_entry_lock_owner_is_dead_impl(owner_pid, owner_start_time);
	UC_G(entry_lock_owner_probe_pid) = owner_pid;
	UC_G(entry_lock_owner_probe_start_time) = owner_start_time;
	UC_G(entry_lock_owner_probe_at) = now;

	return UC_G(entry_lock_owner_probe_dead);
}

static bool ucache_recovery_lock_key_is_readable(
		const php_ucache_header_t *header,
		const php_ucache_entry_lock_record_t *record)
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

	return record->key_len <= php_ucache_shm_bytes(data_end - record->key_offset);
}

static bool ucache_entry_lock_record_is_active_locked(
		php_ucache_entry_lock_record_t *record,
		uint64_t now_rel,
		bool demote_dead_owner)
{
	if (record->state != PHP_UCACHE_ENTRY_LOCK_USED) {
		return false;
	}

	if (record->expires_at != 0 && (uint64_t) record->expires_at <= now_rel) {
		return false;
	}

	if (record->owner_pid == 0) {
		return record->expires_at != 0;
	}

	if (ucache_entry_lock_owner_is_dead(record->owner_pid, record->owner_start_time)) {
		if (!demote_dead_owner) {
			return false;
		}

		record->owner_pid = 0;
		record->owner_start_time = 0;
		record->owner_token = 0;

		return record->expires_at != 0;
	}

	return true;
}

static bool ucache_entry_lock_layout_sane(
		const php_ucache_storage_t *storage,
		const php_ucache_header_t *header)
{
	uint32_t lock_capacity = header->entry_lock_capacity;

	return lock_capacity >= PHP_UCACHE_ENTRY_LOCK_MIN_CAPACITY &&
		lock_capacity <= PHP_UCACHE_ENTRY_LOCK_MAX_CAPACITY &&
		(lock_capacity & (lock_capacity - 1)) == 0 &&
		php_ucache_shm_bytes(header->occupancy_offset) >= sizeof(php_ucache_header_t) &&
		(uint64_t) php_ucache_shm_bytes(header->intern_offset)
			>= (uint64_t) php_ucache_shm_bytes(header->occupancy_offset)
				+ PHP_UCACHE_OCCUPANCY_BYTES(header->capacity) &&
		header->intern_capacity >= PHP_UCACHE_INTERN_MIN_CAPACITY &&
		header->intern_capacity <= PHP_UCACHE_INTERN_MAX_CAPACITY &&
		(header->intern_capacity & (header->intern_capacity - 1)) == 0 &&
		(uint64_t) php_ucache_shm_bytes(header->entry_lock_offset)
			>= (uint64_t) php_ucache_shm_bytes(header->intern_offset)
				+ PHP_UCACHE_INTERN_BYTES(header->intern_capacity) &&
		php_ucache_shm_bytes(header->entry_lock_offset) <= storage->size &&
		(size_t) lock_capacity * sizeof(php_ucache_entry_lock_record_t)
			<= storage->size - php_ucache_shm_bytes(header->entry_lock_offset)
	;
}

static bool ucache_data_region_sane(
		const php_ucache_storage_t *storage,
		const php_ucache_header_t *header)
{
	uint64_t lock_table_end = (uint64_t) php_ucache_shm_bytes(header->entry_lock_offset)
		+ (uint64_t) header->entry_lock_capacity * sizeof(php_ucache_entry_lock_record_t)
	;
	size_t data_bytes = php_ucache_shm_bytes(header->data_offset);

	return (uint64_t) data_bytes >= lock_table_end &&
		data_bytes <= storage->size &&
		php_ucache_shm_bytes(header->data_size) <= storage->size - data_bytes
	;
}

static php_ucache_recovered_entry_lock_t *ucache_collect_recovery_entry_locks(
		php_ucache_header_t *header,
		uint32_t *count_ptr)
{
	php_ucache_recovered_entry_lock_t *locks;
	php_ucache_entry_lock_record_t *record;
	uint64_t now = php_ucache_time_rel(header, (uint64_t) time(NULL));
	uint32_t i, count = 0;
	char *key;

	for (i = 0; i < header->entry_lock_capacity; i++) {
		record = &php_ucache_entry_lock_records_ptr(header)[i];
		if (ucache_entry_lock_record_is_active_locked(record, now, false) &&
			ucache_recovery_lock_key_is_readable(header, record)
		) {
			count++;
		}
	}

	*count_ptr = 0;

	if (count == 0) {
		return NULL;
	}

	locks = (php_ucache_recovered_entry_lock_t *) pecalloc(count, sizeof(*locks), true);

	for (i = 0; i < header->entry_lock_capacity; i++) {
		record = &php_ucache_entry_lock_records_ptr(header)[i];
		if (!ucache_entry_lock_record_is_active_locked(record, now, false) ||
			!ucache_recovery_lock_key_is_readable(header, record)
		) {
			continue;
		}

		key = (char *) pemalloc(record->key_len, true);
		memcpy(key, php_ucache_ptr(record->key_offset), record->key_len);

		locks[*count_ptr].key = key;
		locks[*count_ptr].key_len = record->key_len;
		locks[*count_ptr].hash = record->hash;
		locks[*count_ptr].owner_pid = record->owner_pid;
		locks[*count_ptr].owner_start_time = record->owner_start_time;
		locks[*count_ptr].owner_token = record->owner_token;
		locks[*count_ptr].expires_at = record->expires_at;

		(*count_ptr)++;
	}

	return locks;
}

static void ucache_free_recovery_entry_locks(
		php_ucache_recovered_entry_lock_t *locks,
		uint32_t count)
{
	uint32_t i;

	if (locks == NULL) {
		return;
	}

	for (i = 0; i < count; i++) {
		pefree(locks[i].key, true);
	}

	pefree(locks, true);
}

static void ucache_restore_recovery_entry_locks(
		php_ucache_header_t *header,
		php_ucache_recovered_entry_lock_t *locks,
		uint32_t count)
{
	php_ucache_entry_lock_record_t *record;
	uint32_t i, probe, slot_idx, key_offset;

	for (i = 0; i < count; i++) {
		for (probe = 0; probe < header->entry_lock_capacity; probe++) {
			slot_idx = (ucache_entry_lock_table_index(header, locks[i].hash) + probe) &
				(header->entry_lock_capacity - 1)
			;

			record = &php_ucache_entry_lock_records_ptr(header)[slot_idx];
			if (record->state == PHP_UCACHE_ENTRY_LOCK_USED) {
				continue;
			}

			key_offset = php_ucache_alloc_locked(locks[i].key_len, locks[i].key);
			if (key_offset == 0) {
				break;
			}

			memset(record, 0, sizeof(*record));

			record->hash = locks[i].hash;
			record->owner_pid = locks[i].owner_pid;
			record->owner_start_time = locks[i].owner_start_time;
			record->owner_token = locks[i].owner_token;
			record->expires_at = (uint32_t) locks[i].expires_at;
			record->key_offset = key_offset;
			record->key_len = locks[i].key_len;
			record->state = PHP_UCACHE_ENTRY_LOCK_USED;

			break;
		}
	}
}

#ifdef PHP_UCACHE_HAVE_BOUNDARY_MMAP
static bool ucache_header_boundary_identity_matches_locked(const php_ucache_header_t *header)
{
	php_ucache_ctx_t *ctx = php_ucache_active_context();
	php_ucache_storage_t *storage = &ctx->storage;

	if (!ctx->boundary_shared) {
		return header->boundary_identity_digest_set == 0;
	}

	if (header->boundary_identity_digest_set == 0) {
		return false;
	}

	/* The boundary identity is immutable for the partition lifetime. */
	if (!storage->boundary_digest_memoized) {
		ucache_shared_boundary_digest(ctx, storage->size, storage->boundary_digest_memo);
		storage->boundary_digest_memoized = true;
	}

	return memcmp(header->boundary_identity_digest, storage->boundary_digest_memo, sizeof(storage->boundary_digest_memo)) == 0;
}
#endif /* PHP_UCACHE_HAVE_BOUNDARY_MMAP */

static bool ucache_recovery_blocked_by_live_reference(const php_ucache_header_t *header)
{
	const php_ucache_reader_slot_t *reader_slot;
	/* Not const: zend_atomic_int_load_ex() takes a non-const pointer on
	 * Windows because the Interlocked API has no read-only form. */
	php_ucache_graph_pin_slot_t *pin_slot;
	uint64_t owner_start_time, owner_pid;
	uint32_t i;

	ucache_atomic_fence_seq_cst();

	for (i = 0; i < PHP_UCACHE_READER_SLOTS; i++) {
		reader_slot = &header->reader_slots[i];
		if (ucache_atomic_load_32(&reader_slot->active) == 0) {
			continue;
		}

		owner_pid = php_ucache_atomic_load_64(&reader_slot->owner_pid);
		owner_start_time = php_ucache_atomic_load_64(&reader_slot->owner_start_time);
		if (owner_pid != 0 &&
			!ucache_entry_lock_owner_is_dead(owner_pid, owner_start_time)
		) {
			return true;
		}
	}

	for (i = 0; i < PHP_UCACHE_GRAPH_PIN_SLOTS; i++) {
		pin_slot = (php_ucache_graph_pin_slot_t *) &header->graph_pin_slots[i];
		if (zend_atomic_int_load_ex(&pin_slot->pin_count) == 0) {
			continue;
		}

		owner_pid = (uint64_t) (uint32_t) zend_atomic_int_load_ex(&pin_slot->owner_pid);
		if (owner_pid != 0 &&
			!ucache_entry_lock_owner_is_dead(owner_pid, php_ucache_atomic_load_64(&pin_slot->owner_start_time))
		) {
			return true;
		}
	}

	return false;
}

static bool ucache_recover_after_owner_death(void)
{
	php_ucache_storage_t *storage = &php_ucache_active_context()->storage;
	php_ucache_header_t *header = php_ucache_header_ptr();
	php_ucache_recovered_entry_lock_t *locks;
	php_ucache_reader_slot_t *slot;
	uint64_t owner_pid, owner_start_time;
	uint32_t lock_count = 0, i;
	size_t table_span;
	bool lock_layout_sane;

	if (!php_ucache_header_is_initialized_locked()) {
		return true;
	}

	if ((header->write_seq & 1) == 0) {
		return true;
	}

	if (ucache_recovery_blocked_by_live_reference(header)) {
		return false;
	}

	lock_layout_sane = ucache_entry_lock_layout_sane(storage, header);
	locks = lock_layout_sane &&
		ucache_data_region_sane(storage, header)
		? ucache_collect_recovery_entry_locks(header, &lock_count)
		: NULL
	;

	table_span = (size_t) header->capacity
		* (sizeof(php_ucache_entry_t) + sizeof(uint32_t))
	;
	if (storage->size > sizeof(php_ucache_header_t) &&
		table_span <= storage->size - sizeof(php_ucache_header_t)
	) {
		memset(
			php_ucache_entries_ptr(header),
			0,
			(size_t) header->capacity * sizeof(php_ucache_entry_t)
		);
		php_ucache_access_stamps_reset(header);
	}

	if (lock_layout_sane) {
		php_ucache_occupancy_reset(header);
		/* Interned strings lived in the data region that is reset below. */
		php_ucache_intern_table_reset_locked();

		memset(
			php_ucache_entry_lock_records_ptr(header),
			0,
			(size_t) header->entry_lock_capacity * sizeof(php_ucache_entry_lock_record_t)
		);
	}

	memset(header->orphaned_graphs, 0, sizeof(header->orphaned_graphs));
	/* Pool names lived in the data region that is reset below. */
	memset(header->pool_stats, 0, sizeof(header->pool_stats));
	header->hit_count = 0;
	header->miss_count = 0;

	for (i = 0; i < PHP_UCACHE_READER_SLOTS; i++) {
		slot = &header->reader_slots[i];

		owner_pid = php_ucache_atomic_load_64(&slot->owner_pid);
		owner_start_time = php_ucache_atomic_load_64(&slot->owner_start_time);

		if (owner_pid == PHP_UCACHE_READER_OWNER_RECLAIMING) {
			continue;
		}

		if (owner_pid != 0 &&
			ucache_entry_lock_owner_is_dead(owner_pid, owner_start_time)
		) {
			ucache_atomic_cas_32(&slot->active, 1, 0);
		}
	}

	header->count = 0;
	header->tombstone_count = 0;
	header->expiring_count = 0;
	header->next_free = 0;
	header->free_list = 0;
	header->last_block_offset = 0;
	header->orphaned_graphs_saturated = 0;

	ucache_restore_recovery_entry_locks(header, locks, lock_count);
	ucache_free_recovery_entry_locks(locks, lock_count);

	php_ucache_bump_mutation_epoch_locked(header);

	ucache_seq_publish(&header->write_seq, header->write_seq + 1);

	return true;
}

static bool ucache_rlock_impl(void)
{
	php_ucache_storage_t *storage = &php_ucache_active_context()->storage;

	return storage->lock_initialized && storage->lock_ops->rlock(storage);
}

static bool ucache_wlock_impl(void)
{
	php_ucache_storage_t *storage = &php_ucache_active_context()->storage;

	return storage->lock_initialized && storage->lock_ops->wlock(storage);
}

static void ucache_unlock_impl(void)
{
	php_ucache_storage_t *storage = &php_ucache_active_context()->storage;

	if (storage->lock_initialized) {
		storage->lock_ops->unlock(storage);
	}
}

static uint32_t ucache_entry_lock_expires_at(
		const php_ucache_header_t *header,
		zend_long lease)
{
	ZEND_ASSERT(lease > 0);

	return php_ucache_expiry_deadline(header, (uint64_t) time(NULL), lease);
}

static void ucache_remove_entry_lock_record_locked(
		php_ucache_entry_lock_record_t *record)
{
	if (record->state == PHP_UCACHE_ENTRY_LOCK_USED && record->key_offset != 0) {
		php_ucache_free_locked(record->key_offset);
	}

	memset(record, 0, sizeof(*record));

	record->state = PHP_UCACHE_ENTRY_LOCK_TOMBSTONE;
}

static bool ucache_find_entry_lock_record_slot_locked(
		php_ucache_header_t *header,
		zend_string *key,
		zend_ulong hash,
		uint32_t *slot_idx,
		bool *found)
{
	php_ucache_entry_lock_record_t *record;
	uint64_t now = 0;
	uint32_t first_avail = UINT32_MAX, i, probe;

	*found = false;

	for (probe = 0; probe < header->entry_lock_capacity; probe++) {
		i = (ucache_entry_lock_table_index(header, hash) + probe) & (header->entry_lock_capacity - 1);
		record = &php_ucache_entry_lock_records_ptr(header)[i];

		if (record->state == PHP_UCACHE_ENTRY_LOCK_EMPTY) {
			*slot_idx = first_avail != UINT32_MAX ? first_avail : i;

			return true;
		}

		if (record->state == PHP_UCACHE_ENTRY_LOCK_TOMBSTONE) {
			if (first_avail == UINT32_MAX) {
				first_avail = i;
			}

			continue;
		}

		if (now == 0) {
			now = php_ucache_time_rel(header, (uint64_t) time(NULL));
		}

		if (!ucache_entry_lock_record_is_active_locked(record, now, true)) {
			ucache_remove_entry_lock_record_locked(record);

			if (first_avail == UINT32_MAX) {
				first_avail = i;
			}

			continue;
		}

		if (ucache_entry_lock_record_key_matches(record, key, hash)) {
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

static bool ucache_insert_entry_lock_record_locked(
		php_ucache_header_t *header,
		uint32_t slot_idx,
		zend_string *key,
		zend_ulong hash,
		zend_long lease,
		uint64_t *owner_token)
{
	php_ucache_entry_lock_record_t *record = &php_ucache_entry_lock_records_ptr(header)[slot_idx];
	uint64_t owner_pid;
	uint32_t key_offset;

	if (ZSTR_LEN(key) > UINT32_MAX) {
		return false;
	}

	key_offset = php_ucache_alloc_locked(ZSTR_LEN(key), ZSTR_VAL(key));
	if (key_offset == 0) {
		return false;
	}

	owner_pid = php_ucache_cached_pid();

	memset(record, 0, sizeof(*record));

	record->hash = hash;
	record->owner_pid = owner_pid;
	record->owner_start_time = ucache_cached_self_start_time_token(owner_pid);
	record->owner_token = ++header->entry_lock_acquire_seq;
	record->expires_at = lease > 0 ? ucache_entry_lock_expires_at(header, lease) : 0;
	record->key_offset = key_offset;
	record->key_len = (uint32_t) ZSTR_LEN(key);
	record->state = PHP_UCACHE_ENTRY_LOCK_USED;

	*owner_token = record->owner_token;

	return true;
}

static bool ucache_update_entry_lock_record_lease_locked(
		php_ucache_header_t *header,
		zend_string *key,
		const php_ucache_entry_lock_t *lock,
		zend_long lease)
{
	php_ucache_entry_lock_record_t *record;
	zend_ulong hash;
	uint32_t expires_at, slot_idx;
	bool found;

	if (lease <= 0) {
		return true;
	}

	hash = zend_string_hash_val(key);
	if (!ucache_find_entry_lock_record_slot_locked(header, key, hash, &slot_idx, &found) || !found) {
		return false;
	}

	record = &php_ucache_entry_lock_records_ptr(header)[slot_idx];
	if (record->owner_pid != lock->owner_pid || record->owner_token != lock->owner_token) {
		return false;
	}

	expires_at = ucache_entry_lock_expires_at(header, lease);
	if (record->expires_at < expires_at) {
		record->expires_at = expires_at;
	}

	return true;
}

static php_ucache_entry_lock_t *ucache_create_local_entry_lock(
		php_ucache_ctx_t *ctx,
		zend_long lease,
		bool preserve_lease)
{
	php_ucache_entry_lock_t *lock;

	lock = emalloc(sizeof(php_ucache_entry_lock_t));
	lock->ctx = ctx;
	lock->owner_pid = php_ucache_cached_pid();
	lock->owner_token = 0;
	lock->lease = lease;
	lock->preserve_lease = preserve_lease;

	return lock;
}

static void ucache_defer_entry_lock_release(
		php_ucache_ctx_t *ctx,
		zend_string *key,
		const php_ucache_entry_lock_t *lock)
{
	php_ucache_deferred_entry_lock_release_t *entries = UC_G(deferred_entry_lock_releases), *entry;
	uint32_t count = UC_G(deferred_entry_lock_release_count),
		capacity = UC_G(deferred_entry_lock_release_capacity)
	;
	char *key_copy;

	if (count == capacity) {
		capacity = capacity == 0 ? 8U : capacity * 2U;
		entries = perealloc(entries, (size_t) capacity * sizeof(*entries), true);

		UC_G(deferred_entry_lock_releases) = entries;
		UC_G(deferred_entry_lock_release_capacity) = capacity;
	}

	key_copy = pemalloc(ZSTR_LEN(key), true);
	memcpy(key_copy, ZSTR_VAL(key), ZSTR_LEN(key));

	entry = &entries[count];
	entry->ctx = ctx;
	entry->key = key_copy;
	entry->key_len = (uint32_t) ZSTR_LEN(key);
	entry->owner_pid = lock->owner_pid;
	entry->owner_start_time = ucache_cached_self_start_time_token(lock->owner_pid);
	entry->owner_token = lock->owner_token;
	entry->lease = lock->lease;
	entry->preserve_lease = lock->preserve_lease;

	UC_G(deferred_entry_lock_release_count) = count + 1;
}

static bool ucache_add_local_entry_lock(
		HashTable *locks,
		zend_string *key,
		php_ucache_entry_lock_t *lock)
{
	bool added = false;

	zend_try {
		added = zend_hash_add_ptr(locks, key, lock) != NULL;
	} zend_catch {
		lock->preserve_lease = false;

		ucache_defer_entry_lock_release(lock->ctx, key, lock);

		efree(lock);
		zend_bailout();
	} zend_end_try();

	return added;
}

static void ucache_entry_lock_record_downgrade_to_lease_locked(
		php_ucache_header_t *header,
		php_ucache_entry_lock_record_t *record,
		zend_long lease)
{
	record->owner_pid = 0;
	record->owner_start_time = 0;
	record->owner_token = 0;
	record->expires_at = ucache_entry_lock_expires_at(header, lease);
}

static void ucache_drain_deferred_entry_lock_releases(void)
{
	php_ucache_ctx_t *ctx;
	php_ucache_deferred_entry_lock_release_t *entries = UC_G(deferred_entry_lock_releases);
	php_ucache_header_t *header = NULL;
	php_ucache_entry_lock_record_t *record;
	zend_string **keys, *key;
	uint64_t self_pid;
	uint32_t i, kept = 0, slot_idx, count = UC_G(deferred_entry_lock_release_count);
	bool locked, found;

	if (count == 0) {
		return;
	}

	ctx = php_ucache_active_context();
	self_pid = php_ucache_cached_pid();

	keys = safe_emalloc(count, sizeof(zend_string *), 0);
	for (i = 0; i < count; i++) {
		keys[i] = entries[i].owner_pid == self_pid && entries[i].ctx == ctx
			? zend_string_init(entries[i].key, entries[i].key_len, 0)
			: NULL
		;
	}

	locked = php_ucache_wlock();
	if (locked && php_ucache_header_is_initialized_locked()) {
		header = php_ucache_header_ptr();
	}

	for (i = 0; i < count; i++) {
		if (entries[i].owner_pid != self_pid) {
			pefree(entries[i].key, true);

			continue;
		}

		if (entries[i].ctx != ctx || !locked) {
			entries[kept++] = entries[i];

			continue;
		}

		if (header != NULL) {
			key = keys[i];

			if (ucache_find_local_entry_lock(ctx, key) == NULL &&
				ucache_find_entry_lock_record_slot_locked(header, key, zend_string_hash_val(key), &slot_idx, &found) &&
				found
			) {
				record = &php_ucache_entry_lock_records_ptr(header)[slot_idx];

				if (record->owner_pid == entries[i].owner_pid &&
					record->owner_start_time == entries[i].owner_start_time &&
					record->owner_token == entries[i].owner_token
				) {
					if (entries[i].preserve_lease && entries[i].lease > 0) {
						ucache_entry_lock_record_downgrade_to_lease_locked(header, record, entries[i].lease);
					} else {
						ucache_remove_entry_lock_record_locked(record);
					}
				}
			}
		}

		pefree(entries[i].key, true);
	}

	if (locked) {
		php_ucache_unlock();
	}

	for (i = 0; i < count; i++) {
		if (keys[i] != NULL) {
			zend_string_release(keys[i]);
		}
	}

	efree(keys);

	UC_G(deferred_entry_lock_release_count) = kept;

	if (kept == 0) {
		pefree(entries, true);
		UC_G(deferred_entry_lock_releases) = NULL;
		UC_G(deferred_entry_lock_release_capacity) = 0;
	}
}

static void ucache_release_entry_lock_records_locked(
		php_ucache_header_t *header,
		const php_ucache_entry_lock_release_pair_t *pairs,
		uint32_t pair_count,
		php_ucache_entry_lock_release_mode_t mode)
{
	php_ucache_entry_lock_t *lock;
	php_ucache_entry_lock_record_t *record;
	zend_ulong hash;
	zend_string *key;
	uint32_t i, slot_idx;
	bool found;

	for (i = 0; i < pair_count; i++) {
		key = pairs[i].key;
		lock = pairs[i].lock;

		hash = zend_string_hash_val(key);
		if (!ucache_find_entry_lock_record_slot_locked(header, key, hash, &slot_idx, &found) || !found) {
			continue;
		}

		record = &php_ucache_entry_lock_records_ptr(header)[slot_idx];

		if (record->owner_pid != lock->owner_pid || record->owner_token != lock->owner_token) {
			continue;
		}

		if (mode == PHP_UCACHE_ENTRY_LOCK_RELEASE_PRESERVE_LEASES &&
			lock->preserve_lease &&
			lock->lease > 0
		) {
			ucache_entry_lock_record_downgrade_to_lease_locked(header, record, lock->lease);
		} else {
			ucache_remove_entry_lock_record_locked(record);
		}
	}
}

static void ucache_destroy_entry_locks_if_empty(HashTable **locks_ptr)
{
	if (*locks_ptr != NULL && zend_hash_num_elements(*locks_ptr) == 0) {
		zend_hash_destroy(*locks_ptr);
		FREE_HASHTABLE(*locks_ptr);
		*locks_ptr = NULL;
	}
}

static void ucache_release_entry_locks_for_context(
		php_ucache_ctx_t *ctx,
		HashTable **locks_ptr,
		php_ucache_entry_lock_release_mode_t mode)
{
	php_ucache_ctx_t *prev_ctx;
	php_ucache_entry_lock_t *lock;
	php_ucache_header_t *header;
	php_ucache_entry_lock_release_pair_t *pairs;
	zend_string *key;
	uint32_t i, lock_count, pair_count = 0;
	bool released_to_shm = false;

	if (*locks_ptr == NULL) {
		return;
	}

	lock_count = zend_hash_num_elements(*locks_ptr);
	if (lock_count == 0 || mode == PHP_UCACHE_ENTRY_LOCK_RELEASE_DROP) {
		zend_hash_destroy(*locks_ptr);
		FREE_HASHTABLE(*locks_ptr);
		*locks_ptr = NULL;

		return;
	}

	pairs = safe_emalloc(lock_count, sizeof(php_ucache_entry_lock_release_pair_t), 0);

	ZEND_HASH_FOREACH_STR_KEY_PTR(*locks_ptr, key, lock) {
		if (key == NULL || lock == NULL || lock->ctx != ctx) {
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

	prev_ctx = php_ucache_activate_context(ctx);
	if (php_ucache_wlock()) {
		header = php_ucache_header_ptr();

		if (php_ucache_header_init_locked()) {
			ucache_release_entry_lock_records_locked(
				header,
				pairs,
				pair_count,
				mode
			);

			released_to_shm = true;
		}

		php_ucache_unlock();
	}

	php_ucache_restore_context(prev_ctx);

	if (!released_to_shm) {
		for (i = 0; i < pair_count; i++) {
			ucache_defer_entry_lock_release(ctx, pairs[i].key, pairs[i].lock);
		}
	}

	for (i = 0; i < pair_count; i++) {
		zend_hash_del(*locks_ptr, pairs[i].key);

		zend_string_release(pairs[i].key);
	}

	efree(pairs);

	ucache_destroy_entry_locks_if_empty(locks_ptr);
}

static void ucache_release_entry_locks_all_contexts(
		HashTable **locks_ptr,
		php_ucache_entry_lock_release_mode_t mode)
{
	php_ucache_ctx_t *ctx;
	php_ucache_entry_lock_t *lock;

	while (*locks_ptr != NULL) {
		ctx = NULL;

		ZEND_HASH_FOREACH_PTR(*locks_ptr, lock) {
			if (lock != NULL && lock->ctx != NULL) {
				ctx = lock->ctx;

				break;
			}
		} ZEND_HASH_FOREACH_END();

		ZEND_ASSERT(ctx != NULL);
		if (ctx == NULL) {
			break;
		}

		ucache_release_entry_locks_for_context(ctx, locks_ptr, mode);
	}
}

static void ucache_ensure_entry_lock_owner(void)
{
#ifndef ZEND_WIN32
	uint64_t cur_pid = php_ucache_cached_pid();

	if (UC_G(entry_lock_owner_pid) == 0) {
		UC_G(entry_lock_owner_pid) = cur_pid;

		return;
	}

	if (UC_G(entry_lock_owner_pid) == cur_pid) {
		return;
	}

	ucache_release_entry_locks_for_context(
		php_ucache_owning_context(),
		&UC_G(entry_lock_table),
		PHP_UCACHE_ENTRY_LOCK_RELEASE_DROP
	);

	UC_G(entry_lock_owner_pid) = cur_pid;
#endif /* !ZEND_WIN32 */
}

static void ucache_entry_lock_dtor(zval *lock_zv)
{
	php_ucache_entry_lock_t *lock = Z_PTR_P(lock_zv);

	if (lock != NULL) {
		efree(lock);
	}
}

static HashTable *ucache_prepare_entry_locks_for_insert(uint32_t reserve)
{
	HashTable **locks_ptr = ucache_entry_lock_table_ptr();

	if (*locks_ptr == NULL) {
		ALLOC_HASHTABLE(*locks_ptr);
		zend_hash_init(*locks_ptr, 0, NULL, ucache_entry_lock_dtor, 0);
	}

	zend_hash_extend(*locks_ptr, zend_hash_num_elements(*locks_ptr) + reserve, 0);

	return *locks_ptr;
}

static bool ucache_upgrade_held_entry_lock(
		php_ucache_ctx_t *ctx,
		zend_string *key,
		zend_long lease,
		php_ucache_entry_lock_t *lock)
{
	php_ucache_ctx_t *prev_ctx;
	php_ucache_header_t *header;
	bool updated;

	if (lease > lock->lease) {
		prev_ctx = php_ucache_activate_context(ctx);
		if (!php_ucache_wlock()) {
			php_ucache_restore_context(prev_ctx);

			return false;
		}

		header = php_ucache_header_ptr();
		updated = php_ucache_header_init_locked() &&
			ucache_update_entry_lock_record_lease_locked(header, key, lock, lease)
		;

		php_ucache_unlock();
		php_ucache_restore_context(prev_ctx);

		if (!updated) {
			return false;
		}

		lock->lease = lease;
	}

	lock->preserve_lease = true;

	return true;
}

static bool ucache_remove_owned_entry_lock_record(
		php_ucache_ctx_t *ctx,
		zend_string *key,
		zend_ulong hash,
		uint64_t owner_pid,
		uint64_t owner_token)
{
	php_ucache_ctx_t *prev_ctx;
	php_ucache_header_t *header;
	php_ucache_entry_lock_record_t *record;
	uint32_t slot_idx;
	bool found;

	prev_ctx = php_ucache_activate_context(ctx);
	if (!php_ucache_wlock()) {
		php_ucache_restore_context(prev_ctx);

		return false;
	}

	header = php_ucache_header_ptr();

	if (php_ucache_header_is_initialized_locked() &&
		ucache_find_entry_lock_record_slot_locked(header, key, hash, &slot_idx, &found) &&
		found
	) {
		record = &php_ucache_entry_lock_records_ptr(header)[slot_idx];

		if (record->owner_pid == owner_pid && record->owner_token == owner_token) {
			ucache_remove_entry_lock_record_locked(record);
		}
	}

	php_ucache_unlock();
	php_ucache_restore_context(prev_ctx);

	return true;
}

static void ucache_release_entry_locks_for_keys(
		php_ucache_ctx_t *ctx,
		zend_string **keys,
		const bool *acquired,
		uint32_t count)
{
	php_ucache_ctx_t *prev_ctx;
	php_ucache_entry_lock_t *lock;
	php_ucache_header_t *header;
	php_ucache_entry_lock_record_t *record;
	zend_ulong hash;
	HashTable **locks_ptr = ucache_entry_lock_table_ptr();
	uint32_t i, slot_idx;
	bool found, any_held = false;

	ucache_ensure_entry_lock_owner();

	for (i = 0; i < count; i++) {
		if (acquired[i] && ucache_find_local_entry_lock(ctx, keys[i]) != NULL) {
			any_held = true;

			break;
		}
	}

	if (!any_held) {
		return;
	}

	prev_ctx = php_ucache_activate_context(ctx);

	if (!php_ucache_wlock()) {
		php_ucache_restore_context(prev_ctx);

		return;
	}

	header = php_ucache_header_ptr();

	if (php_ucache_header_is_initialized_locked()) {
		for (i = 0; i < count; i++) {
			if (!acquired[i]) {
				continue;
			}

			lock = ucache_find_local_entry_lock(ctx, keys[i]);
			if (lock == NULL) {
				continue;
			}

			hash = zend_string_hash_val(keys[i]);
			if (ucache_find_entry_lock_record_slot_locked(header, keys[i], hash, &slot_idx, &found) &&
				found
			) {
				record = &php_ucache_entry_lock_records_ptr(header)[slot_idx];

				if (record->owner_pid == lock->owner_pid && record->owner_token == lock->owner_token) {
					ucache_remove_entry_lock_record_locked(record);
				}
			}
		}
	}

	php_ucache_unlock();
	php_ucache_restore_context(prev_ctx);

	for (i = 0; i < count; i++) {
		if (acquired[i] && ucache_find_local_entry_lock(ctx, keys[i]) != NULL) {
			zend_hash_del(*locks_ptr, keys[i]);
		}
	}

	ucache_destroy_entry_locks_if_empty(locks_ptr);
}

static bool ucache_acquire_entry_lock_record(
		php_ucache_ctx_t *ctx,
		zend_string *key,
		zend_long lease)
{
	php_ucache_ctx_t *prev_ctx;
	php_ucache_entry_lock_t *lock;
	php_ucache_header_t *header;
	zend_ulong hash = zend_string_hash_val(key);
	HashTable *locks, **locks_ptr = ucache_entry_lock_table_ptr();
	uint32_t slot_idx;
	bool found, inserted = false;

	ucache_ensure_entry_lock_owner();
	ucache_drain_deferred_entry_lock_releases();

	lock = ucache_find_local_entry_lock(ctx, key);
	if (lock != NULL) {
		return ucache_upgrade_held_entry_lock(ctx, key, lease, lock);
	}

	locks = ucache_prepare_entry_locks_for_insert(1);
	lock = ucache_create_local_entry_lock(ctx, lease, true);
	prev_ctx = php_ucache_activate_context(ctx);

	if (!php_ucache_wlock()) {
		php_ucache_restore_context(prev_ctx);

		goto bailout;
	}

	header = php_ucache_header_ptr();

	if (!php_ucache_header_init_locked()) {
		php_ucache_unlock();
		php_ucache_restore_context(prev_ctx);

		goto bailout;
	}

	if (ucache_find_entry_lock_record_slot_locked(header, key, hash, &slot_idx, &found) && !found) {
		inserted = ucache_insert_entry_lock_record_locked(
			header,
			slot_idx,
			key,
			hash,
			lease,
			&lock->owner_token
		);
	}

	php_ucache_unlock();
	php_ucache_restore_context(prev_ctx);

	if (inserted) {
		if (ucache_add_local_entry_lock(locks, key, lock)) {
			return true;
		}

		if (!ucache_remove_owned_entry_lock_record(ctx, key, hash, lock->owner_pid, lock->owner_token)) {
			lock->preserve_lease = false;
			ucache_defer_entry_lock_release(ctx, key, lock);
		}
	}

bailout:
	efree(lock);

	ucache_destroy_entry_locks_if_empty(locks_ptr);

	return false;
}

static bool ucache_acquire_entry_lock_records(
		php_ucache_ctx_t *ctx,
		zend_string **keys,
		bool *acquired,
		uint32_t count)
{
	php_ucache_ctx_t *prev_ctx;
	php_ucache_entry_lock_t *existing, **pending_locks;
	php_ucache_header_t *header;
	zend_ulong hash;
	HashTable *locks, **locks_ptr = ucache_entry_lock_table_ptr();
	uint64_t waited_us = 0;
	uint32_t i, slot_idx, start = 0, stop, insert_end, blocked_idx = UINT32_MAX, pending_count = 0;
	bool *collisions, found, blocked, insert_failed, add_failed;

	ucache_ensure_entry_lock_owner();
	ucache_drain_deferred_entry_lock_releases();

	pending_locks = safe_emalloc(count, sizeof(*pending_locks), 0);
	collisions = safe_emalloc(count, sizeof(*collisions), 0);

	for (i = 0; i < count; i++) {
		acquired[i] = false;
		pending_locks[i] = NULL;
		collisions[i] = false;

		if (i > 0 && zend_string_equals(keys[i], keys[i - 1])) {
			continue;
		}

		existing = *locks_ptr != NULL ? zend_hash_find_ptr(*locks_ptr, keys[i]) : NULL;
		if (existing != NULL && existing->ctx == ctx) {
			continue;
		}

		collisions[i] = existing != NULL;
		pending_locks[i] = ucache_create_local_entry_lock(ctx, 0, false);

		pending_count++;
	}

	if (pending_count == 0) {
		efree(collisions);
		efree(pending_locks);

		return true;
	}

	locks = ucache_prepare_entry_locks_for_insert(pending_count);

	for (;;) {
		stop = count;
		insert_end = count;
		blocked = false;
		insert_failed = false;
		add_failed = false;
		prev_ctx = php_ucache_activate_context(ctx);

		if (!php_ucache_wlock()) {
			php_ucache_restore_context(prev_ctx);

			goto bailout;
		}

		header = php_ucache_header_ptr();

		if (!php_ucache_header_init_locked()) {
			php_ucache_unlock();
			php_ucache_restore_context(prev_ctx);

			goto bailout;
		}

		for (i = start; i < count; i++) {
			if (pending_locks[i] == NULL) {
				continue;
			}

			hash = zend_string_hash_val(keys[i]);

			if (!ucache_find_entry_lock_record_slot_locked(header, keys[i], hash, &slot_idx, &found) || found) {
				blocked = true;
				stop = i;
				insert_end = i;

				break;
			}

			if (!ucache_insert_entry_lock_record_locked(
					header,
					slot_idx,
					keys[i],
					hash,
					0,
					&pending_locks[i]->owner_token
				)
			) {
				insert_failed = true;
				stop = i;
				insert_end = i;

				break;
			}

			if (collisions[i]) {
				stop = i;
				insert_end = i + 1;

				break;
			}
		}

		php_ucache_unlock();
		php_ucache_restore_context(prev_ctx);

		for (i = start; i < insert_end; i++) {
			if (pending_locks[i] == NULL) {
				continue;
			}

			if (add_failed || !ucache_add_local_entry_lock(locks, keys[i], pending_locks[i])) {
				if (!ucache_remove_owned_entry_lock_record(
						ctx,
						keys[i],
						zend_string_hash_val(keys[i]),
						pending_locks[i]->owner_pid,
						pending_locks[i]->owner_token
					)
				) {
					pending_locks[i]->preserve_lease = false;
					ucache_defer_entry_lock_release(ctx, keys[i], pending_locks[i]);
				}

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
			blocked_idx = stop;
			waited_us = 0;
		}

		if (waited_us >= PHP_UCACHE_ENTRY_LOCK_WAIT_TIMEOUT_US) {
			goto bailout;
		}

		waited_us += ucache_sleep_entry_lock_retry_interval();
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

	ucache_destroy_entry_locks_if_empty(locks_ptr);

	return false;
}

static void ucache_header_layout_memo(php_ucache_storage_t *storage)
{
	if (storage->layout_memo_valid) {
		return;
	}

	storage->capacity_memo = ucache_calculate_capacity(storage->size, &storage->capacity_clamped);
	storage->entry_lock_capacity_memo =
		ucache_calculate_entry_lock_capacity(storage->capacity_memo)
	;
	storage->occupancy_offset_memo = php_ucache_shm_units(PHP_UCACHE_ALIGNED_SIZE(
		sizeof(php_ucache_header_t)
		+ storage->capacity_memo * sizeof(php_ucache_entry_t)
		+ storage->capacity_memo * sizeof(uint32_t)
	));
	storage->intern_capacity_memo = ucache_calculate_intern_capacity(storage->capacity_memo);
	storage->intern_offset_memo = php_ucache_shm_units(PHP_UCACHE_ALIGNED_SIZE(
		php_ucache_shm_bytes(storage->occupancy_offset_memo)
		+ PHP_UCACHE_OCCUPANCY_BYTES(storage->capacity_memo)
	));
	storage->entry_lock_offset_memo = php_ucache_shm_units(PHP_UCACHE_ALIGNED_SIZE(
		php_ucache_shm_bytes(storage->intern_offset_memo)
		+ PHP_UCACHE_INTERN_BYTES(storage->intern_capacity_memo)
	));
	storage->data_offset_memo = php_ucache_shm_units(PHP_UCACHE_ALIGNED_SIZE(
		php_ucache_shm_bytes(storage->entry_lock_offset_memo)
		+ storage->entry_lock_capacity_memo * sizeof(php_ucache_entry_lock_record_t)
	));
	storage->layout_memo_valid = true;
}

static void ucache_free_list_remove_locked(php_ucache_header_t *header, uint32_t block_offset)
{
	php_ucache_block_t *block = php_ucache_block_ptr(block_offset);

	if (block->prev_free != 0) {
		php_ucache_block_ptr(block->prev_free)->next_free = block->next_free;
	} else {
		header->free_list = block->next_free;
	}

	if (block->next_free != 0) {
		php_ucache_block_ptr(block->next_free)->prev_free = block->prev_free;
	}

	block->next_free = 0;
	block->prev_free = 0;
	block->flags &= ~PHP_UCACHE_BLOCK_FREE;
}

static void ucache_free_list_insert_locked(php_ucache_header_t *header, uint32_t block_offset)
{
	php_ucache_block_t *block = php_ucache_block_ptr(block_offset);

	block->prev_free = 0;
	block->next_free = header->free_list;

	if (header->free_list != 0) {
		php_ucache_block_ptr(header->free_list)->prev_free = block_offset;
	}

	ucache_block_mark_free(block);

	header->free_list = block_offset;
}

static void ucache_update_following_prev_size_locked(
		php_ucache_header_t *header,
		uint32_t block_offset,
		const php_ucache_block_t *block)
{
	uint32_t next_offset = block_offset + block->size;

	if (next_offset < ucache_used_end_offset_locked(header)) {
		php_ucache_block_ptr(next_offset)->prev_size = block->size;
	}
}

static void ucache_trim_tail_free_blocks_locked(
		php_ucache_header_t *header,
		uint32_t block_offset)
{
	php_ucache_block_t *block = php_ucache_block_ptr(block_offset);
	uint32_t prev_offset;

	while (block_offset >= header->data_offset &&
		header->last_block_offset == block_offset &&
		php_ucache_block_is_free(block) &&
		block_offset + block->size == ucache_used_end_offset_locked(header)
	) {
		prev_offset = 0;
		ucache_free_list_remove_locked(header, block_offset);
		header->next_free -= block->size;

		if (block->prev_size != 0 && block_offset > header->data_offset) {
			prev_offset = block_offset - block->prev_size;
		}

		header->last_block_offset = prev_offset;

		if (prev_offset == 0) {
			break;
		}

		block_offset = prev_offset;
		block = php_ucache_block_ptr(block_offset);
	}
}

static void ucache_write_section_enter(void)
{
	php_ucache_header_t *header = php_ucache_header_ptr();

	UC_G(write_seq_bumped) = false;
	UC_G(reader_drain_state) = 0;

	if (header == NULL || !php_ucache_header_is_initialized_locked()) {
		return;
	}

	ucache_seq_announce(&header->write_seq, header->write_seq + 1);

	UC_G(write_seq_bumped) = true;
}

static void ucache_write_section_leave(void)
{
	php_ucache_header_t *header;

	if (!UC_G(write_seq_bumped)) {
		return;
	}

	UC_G(write_seq_bumped) = false;
	UC_G(reader_drain_state) = 0;

	header = php_ucache_header_ptr();
	if (header == NULL) {
		return;
	}

	ucache_seq_publish(&header->write_seq, header->write_seq + 1);
}

static void ucache_write_section_note_header_initialized(php_ucache_header_t *header)
{
	if (!UC_G(lock_held_is_write) || UC_G(write_seq_bumped)) {
		return;
	}

	ucache_seq_announce(&header->write_seq, header->write_seq + 1);

	UC_G(write_seq_bumped) = true;
	UC_G(reader_drain_state) = 0;
}

static bool ucache_header_format_fresh_locked(php_ucache_header_t *header)
{
	php_ucache_ctx_t *ctx = php_ucache_active_context();
	php_ucache_storage_t *storage = &ctx->storage;
	size_t data_bytes = php_ucache_shm_bytes(storage->data_offset_memo);

	if (data_bytes >= storage->size) {
		return false;
	}

	memset(header, 0, data_bytes);

	header->capacity = storage->capacity_memo;
	header->data_offset = storage->data_offset_memo;
	header->data_size = (uint32_t) ((storage->size - data_bytes) / PHP_UCACHE_SHM_UNIT);
	header->entry_lock_capacity = storage->entry_lock_capacity_memo;
	header->entry_lock_offset = storage->entry_lock_offset_memo;
	header->occupancy_offset = storage->occupancy_offset_memo;
	header->intern_capacity = storage->intern_capacity_memo;
	header->intern_offset = storage->intern_offset_memo;
	header->intern_generation = 1;
	header->mutation_epoch = 1;
	header->time_base = (uint64_t) time(NULL);
	if (header->time_base == 0) {
		header->time_base = 1;
	}

	header->write_seq = UC_G(write_seq_bumped) ? 1U : 2U;
#ifdef PHP_UCACHE_HAVE_SHARED_MUTEX
	header->lock_model = ucache_shared_mutex_init(header)
		? PHP_UCACHE_LOCK_MODEL_MUTEX
		: PHP_UCACHE_LOCK_MODEL_FCNTL
	;
#else
	header->lock_model = PHP_UCACHE_LOCK_MODEL_FCNTL;
#endif
#ifdef PHP_UCACHE_HAVE_BOUNDARY_MMAP
	if (ctx->boundary_shared) {
		ucache_shared_boundary_digest(
			ctx,
			storage->size,
			header->boundary_identity_digest
		);

		header->boundary_identity_digest_set = 1;
		header->boot_token = ucache_shared_boundary_boot_token();
	}
#endif /* PHP_UCACHE_HAVE_BOUNDARY_MMAP */

	header->version = PHP_UCACHE_VERSION;
	ucache_atomic_fence_seq_cst();
	header->magic = PHP_UCACHE_MAGIC;

	ucache_write_section_note_header_initialized(header);

	return true;
}

static uint32_t ucache_alloc_from_free_list_locked(
		php_ucache_header_t *header,
		size_t size,
		uint32_t total_size,
		const void *src)
{
	php_ucache_block_t *block, *remainder;
	uint32_t min_split_size, best_offset = 0, best_size = UINT32_MAX, *free_offset_ptr;

	min_split_size = php_ucache_shm_units(PHP_UCACHE_ALIGNED_SIZE(sizeof(php_ucache_block_t) + 1));

	free_offset_ptr = &header->free_list;
	while (*free_offset_ptr != 0) {
		block = php_ucache_block_ptr(*free_offset_ptr);

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

	block = php_ucache_block_ptr(best_offset);
	ucache_free_list_remove_locked(header, best_offset);

	if (block->size >= total_size + min_split_size) {
		remainder = php_ucache_block_ptr(best_offset + total_size);
		remainder->size = block->size - total_size;
		remainder->prev_size = total_size;
		remainder->flags = PHP_UCACHE_BLOCK_FREE;

		block->size = total_size;

		ucache_update_following_prev_size_locked(header, best_offset + total_size, remainder);
		ucache_free_list_insert_locked(header, best_offset + total_size);

		if (header->last_block_offset == best_offset) {
			header->last_block_offset = best_offset + total_size;
		}
	} else {
		ucache_update_following_prev_size_locked(header, best_offset, block);
	}

	if (src != NULL) {
		memcpy(php_ucache_ptr(best_offset + PHP_UCACHE_BLOCK_HEADER_UNITS), src, size);
	}

	return best_offset + PHP_UCACHE_BLOCK_HEADER_UNITS;
}

static uint32_t ucache_alloc_from_tail_locked(
		php_ucache_header_t *header,
		size_t size,
		uint32_t total_size,
		const void *src)
{
	php_ucache_block_t *block;
	uint32_t block_offset;

	if (header->next_free > header->data_size || total_size > header->data_size - header->next_free) {
		return 0;
	}

	block_offset = header->data_offset + header->next_free;
	block = php_ucache_block_ptr(block_offset);
	block->size = total_size;
	block->prev_size = header->last_block_offset != 0 ? php_ucache_block_ptr(header->last_block_offset)->size : 0;
	block->next_free = 0;
	block->prev_free = 0;
	block->flags = 0;

	if (src != NULL) {
		memcpy(php_ucache_ptr(block_offset + PHP_UCACHE_BLOCK_HEADER_UNITS), src, size);
	}

	header->next_free += total_size;
	header->last_block_offset = block_offset;

	return block_offset + PHP_UCACHE_BLOCK_HEADER_UNITS;
}

static bool ucache_try_storage_handler(
		const php_ucache_shm_handler_entry_t *handler_entry,
		const char **error_in_ptr,
		int *error_code_ptr)
{
	const char *error_in = NULL;
	php_ucache_ctx_t *ctx = php_ucache_active_context();
	php_ucache_runtime_t *runtime = php_ucache_active_runtime();
	php_ucache_storage_t *storage = &ctx->storage;
	php_ucache_shm_segment_t **segments = NULL;
	uint32_t segment_count = 0;

	if (!handler_entry->handler->create_segments(
			runtime->configured_memory,
			&segments,
			&segment_count,
			&error_in
		)
	) {
		*error_code_ptr = ucache_platform_ops()->alloc_error_code();
		*error_in_ptr = error_in;

		ucache_cleanup_segments(handler_entry->handler, segments, segment_count);

		return false;
	}

	storage->handler = handler_entry->handler;
	storage->handler_name = handler_entry->name;
	storage->segments = segments;
	storage->segment_count = segment_count;
	storage->size = runtime->configured_memory;
	storage->initialized = true;

	return true;
}

static bool ucache_select_storage_handler(void)
{
	const php_ucache_shm_handler_entry_t *handler_entry;
	const char *model = UC_G(memory_model), *error_in = NULL;
	int saved_error = 0;

#ifdef PHP_UCACHE_HAVE_BOUNDARY_MMAP
	if (php_ucache_active_context()->boundary_shared) {
		if (ucache_try_storage_handler(
				ucache_shared_boundary_handler_entry(),
				&error_in,
				&saved_error
			)
		) {
			return true;
		}

		ucache_platform_ops()->log_alloc_failure(error_in, saved_error);

		return false;
	}
#endif /* PHP_UCACHE_HAVE_BOUNDARY_MMAP */

	if (model && model[0]) {
		if (strcmp(model, "cgi") == 0) {
			model = "shm";
		}

		for (handler_entry = ucache_handler_table(); handler_entry->name; handler_entry++) {
			if (strcmp(model, handler_entry->name) == 0 &&
				ucache_try_storage_handler(handler_entry, &error_in, &saved_error)
			) {
				return true;
			}
		}
	}

	for (handler_entry = ucache_handler_table(); handler_entry->name; handler_entry++) {
		if (model && model[0] && strcmp(model, handler_entry->name) == 0) {
			continue;
		}

		if (ucache_try_storage_handler(handler_entry, &error_in, &saved_error)) {
			return true;
		}
	}

	ucache_platform_ops()->log_alloc_failure(error_in, saved_error);

	return false;
}

static bool ucache_enter_write_locked_section(void)
{
	UC_G(lock_held_is_write) = true;

	if (!ucache_recover_after_owner_death()) {
		UC_G(lock_held_is_write) = false;
		ucache_unlock_impl();

		return false;
	}

	ucache_write_section_enter();

	return true;
}

static bool ucache_negotiate_lock_model(void)
{
	const php_ucache_lock_ops_t *negotiated_ops;
	php_ucache_ctx_t *ctx = php_ucache_active_context();
	php_ucache_storage_t *storage = &ctx->storage;
	bool header_initialized;

	if (!ucache_wlock_impl()) {
		return false;
	}

	header_initialized = ucache_header_is_initialized_acquire();
#ifdef PHP_UCACHE_HAVE_BOUNDARY_MMAP
	if (header_initialized &&
		ctx->boundary_shared &&
		ucache_shared_boundary_discard_stale_image_locked()
	) {
		header_initialized = false;
	}
#endif

	negotiated_ops = header_initialized
		? ucache_lock_ops_for_model(php_ucache_header_ptr()->lock_model)
		: storage->lock_ops
	;

	if (negotiated_ops != storage->lock_ops) {
		ucache_unlock_impl();
		storage->lock_ops = negotiated_ops;

		return php_ucache_wlock();
	}

	return ucache_enter_write_locked_section();
}

static bool ucache_startup_storage_impl(void)
{
	php_ucache_storage_t *storage = &php_ucache_active_context()->storage;

	if (storage->initialized) {
		return true;
	}

	if (!ucache_select_storage_handler()) {
		return false;
	}

	ZEND_ASSERT(storage->segment_count == 1);

	if (!ucache_create_lock()) {
		goto bailout;
	}

	if (!ucache_negotiate_lock_model()) {
		ucache_destroy_lock();

		goto bailout;
	}

	if (!php_ucache_header_init_locked()) {
		php_ucache_unlock();
		ucache_destroy_lock();

		goto bailout;
	}

	php_ucache_unlock();

	if (storage->capacity_clamped && !ucache_capacity_clamp_warned) {
		ucache_capacity_clamp_warned = true;

		zend_error(
			E_WARNING,
			"user_cache.entries_hint (" ZEND_LONG_FMT ") exceeds what user_cache.shm_size can index; clamping capacity to %u",
			UC_G(entries_hint),
			storage->capacity_memo
		);
	}

	storage->lock_ops = ucache_lock_ops_for_model(php_ucache_header_ptr()->lock_model);

	ucache_atomic_store_32(&storage->startup_complete, 1);

	return true;

bailout:
	ucache_cleanup_segments(
		storage->handler,
		storage->segments,
		storage->segment_count
	);
	php_ucache_reset_storage();

	return false;
}

static bool ucache_startup_storage(void)
{
	bool result;

	if (ucache_startup_storage_is_complete()) {
		return true;
	}

	ucache_startup_storage_lock();
	result = ucache_startup_storage_impl();
	ucache_startup_storage_unlock();

	return result;
}

static void ucache_resolve_runtime(void)
{
	php_ucache_ctx_t *ctx = php_ucache_active_context();
	php_ucache_runtime_t *runtime;
	php_ucache_storage_t *storage = &ctx->storage;

	php_ucache_reset_runtime();

	runtime = php_ucache_active_runtime();
	if (!runtime->enabled) {
		return;
	}

	if (UC_G(in_request_shutdown)) {
		ucache_set_unavailable(PHP_UCACHE_REASON_REQUEST_SHUTDOWN);

		return;
	}

	if (UC_G(request_unavailable_reason) != PHP_UCACHE_REASON_NONE) {
		ucache_set_unavailable(UC_G(request_unavailable_reason));

		return;
	}

	if (!ucache_is_opted_in()) {
		ucache_set_unavailable(PHP_UCACHE_REASON_SAPI_NOT_ENABLED);

		return;
	}

	if (ucache_is_disabled_for_sapi()) {
		ucache_set_unavailable(PHP_UCACHE_REASON_DISABLED_BY_INI);

		return;
	}

	if (!storage->initialized &&
		ucache_requires_pre_request_storage()
	) {
		ucache_set_unavailable(PHP_UCACHE_REASON_BACKEND_NOT_INITIALIZED_BEFORE_WORKER);

		return;
	}

	if (!ucache_startup_storage()) {
		ucache_set_unavailable(PHP_UCACHE_REASON_SHM_INIT_FAILED);

		return;
	}

	if (ucache_requires_pre_request_storage() &&
		!storage->initialized_before_request
	) {
		ucache_set_unavailable(PHP_UCACHE_REASON_BACKEND_INITIALIZED_AFTER_WORKER);

		return;
	}

	ucache_set_available();
}

static bool ucache_reader_slot_owner_is_dead(const php_ucache_reader_slot_t *slot)
{
	uint64_t owner_pid = php_ucache_atomic_load_64(&slot->owner_pid),
		owner_start_time = php_ucache_atomic_load_64(&slot->owner_start_time)
	;

	return ucache_entry_lock_owner_is_dead(owner_pid, owner_start_time);
}

#ifndef ZEND_WIN32
static void ucache_optimistic_atfork_child(void)
{
	UC_G(reader_claim_count) = 0;
}
#endif

static int32_t ucache_claim_reader_slot(php_ucache_header_t *header)
{
	php_ucache_reader_slot_t *slot;
	uint64_t my_pid, expected;
	uint32_t i;

	for (i = 0; i < UC_G(reader_claim_count); i++) {
		if (UC_G(reader_claims)[i].header == header) {
			return (int32_t) UC_G(reader_claims)[i].slot_index;
		}
	}

	if (UC_G(reader_claim_count) == PHP_UCACHE_READER_CLAIM_MAX ||
		!ucache_reader_slots_enabled
	) {
		return -1;
	}

	my_pid = php_ucache_cached_pid();

	for (i = 0; i < PHP_UCACHE_READER_SLOTS; i++) {
		slot = &header->reader_slots[i];

		expected = php_ucache_atomic_load_64(&slot->owner_pid);

		if (expected == PHP_UCACHE_READER_OWNER_RECLAIMING) {
			continue;
		}

		if (expected != 0) {
			if (!ucache_reader_slot_owner_is_dead(slot) ||
				ucache_atomic_load_32(&slot->active) != 0
			) {
				continue;
			}

			if (!ucache_atomic_cas_64(&slot->owner_pid, expected, PHP_UCACHE_READER_OWNER_RECLAIMING)) {
				continue;
			}

			php_ucache_atomic_store_64(&slot->owner_start_time, 0);
			php_ucache_atomic_store_64(&slot->owner_pid, 0);
		}

		if (ucache_atomic_cas_64(&slot->owner_pid, 0, my_pid)) {
			php_ucache_atomic_store_64(&slot->owner_start_time, ucache_cached_self_start_time_token(my_pid));

			goto finish;
		}
	}

	return -1;

finish:
	UC_G(reader_claims)[UC_G(reader_claim_count)].header = header;
	UC_G(reader_claims)[UC_G(reader_claim_count)].slot_index = i;
	UC_G(reader_claim_count)++;

	return (int32_t) i;
}

#ifdef ZTS
static bool ucache_claim_header_is_attached(const php_ucache_header_t *header)
{
	const php_ucache_partition_t *partition;
	bool attached = false;

	if (ucache_storage_holds_header(&php_ucache_ctx_state.storage, header)) {
		return true;
	}

	php_ucache_boundary_partitions_lock();

	for (partition = php_ucache_partitions; partition != NULL; partition = partition->next) {
		if (ucache_storage_holds_header(&partition->ctx.storage, header)) {
			attached = true;

			break;
		}
	}

	php_ucache_boundary_partitions_unlock();

	return attached;
}
#endif /* ZTS */

static bool ucache_header_layout_matches_locked(
		const php_ucache_storage_t *storage,
		const php_ucache_header_t *header)
{
	return header->magic == PHP_UCACHE_MAGIC &&
		header->version == PHP_UCACHE_VERSION &&
		header->capacity == storage->capacity_memo &&
		header->data_offset == storage->data_offset_memo &&
		header->entry_lock_capacity == storage->entry_lock_capacity_memo &&
		header->entry_lock_offset == storage->entry_lock_offset_memo &&
		header->occupancy_offset == storage->occupancy_offset_memo &&
		header->intern_capacity == storage->intern_capacity_memo &&
		header->intern_offset == storage->intern_offset_memo
	;
}

static bool ucache_header_identity_matches_locked(const php_ucache_header_t *header)
{
#ifdef PHP_UCACHE_HAVE_BOUNDARY_MMAP
	return ucache_header_boundary_identity_matches_locked(header);
#else
	return true;
#endif
}

bool php_ucache_quiesce_graph_payloads_locked(void)
{
	php_ucache_header_t *header;
	php_ucache_reader_slot_t *slot;
	uint64_t waited_us = 0;
	uint32_t i, count, spin = 0;

	if (!UC_G(write_seq_bumped)) {
		return true;
	}

	if (UC_G(reader_drain_state) != 0) {
		return UC_G(reader_drain_state) > 0;
	}

	header = php_ucache_header_ptr();
	if (header == NULL) {
		return true;
	}

	ucache_atomic_fence_seq_cst();

	for (;;) {
		count = 0;
		for (i = 0; i < PHP_UCACHE_READER_SLOTS; i++) {
			if (ucache_atomic_load_32(&header->reader_slots[i].active) != 0) {
				count++;
			}
		}

		if (count == 0) {
			UC_G(reader_drain_state) = 1;

			return true;
		}

		spin++;

		if ((spin & 0xFFU) == 0) {
			for (i = 0; i < PHP_UCACHE_READER_SLOTS; i++) {
				slot = &header->reader_slots[i];

				if (ucache_atomic_load_32(&slot->active) == 0 ||
					php_ucache_atomic_load_64(&slot->owner_pid) == 0
				) {
					continue;
				}

				if (ucache_reader_slot_owner_is_dead(slot)) {
					ucache_atomic_cas_32(&slot->active, 1, 0);
				}
			}
		}

		if (spin > PHP_UCACHE_READER_DRAIN_SPIN) {
			waited_us += ucache_platform_ops()->sleep_us(50);

			if (waited_us > PHP_UCACHE_READER_DRAIN_TIMEOUT_US) {
				UC_G(reader_drain_state) = -1;

				return false;
			}
		}
	}
}

void php_ucache_optimistic_fork_setup(void)
{
	static bool registered = false;

	if (!PHP_UCACHE_OPTIMISTIC_ENABLED) {
		return;
	}

	ucache_reader_slots_enabled = true;

	if (registered) {
		return;
	}

#ifndef ZEND_WIN32
	if (pthread_atfork(NULL, NULL, ucache_optimistic_atfork_child) != 0) {
		return;
	}
#endif

	registered = true;
}

bool php_ucache_optimistic_reader_begin(php_ucache_header_t *header, uint32_t *slot_idx_ptr)
{
	int32_t slot_idx;

	if (!PHP_UCACHE_OPTIMISTIC_ENABLED) {
		return false;
	}

	slot_idx = ucache_claim_reader_slot(header);
	if (slot_idx < 0) {
		return false;
	}

	*slot_idx_ptr = (uint32_t) slot_idx;

	ucache_atomic_store_32(&header->reader_slots[slot_idx].active, 1);
	ucache_atomic_fence_seq_cst();

	return true;
}

void php_ucache_optimistic_reader_end(php_ucache_header_t *header, uint32_t slot_idx)
{
	ucache_atomic_store_32(&header->reader_slots[slot_idx].active, 0);
}

#ifdef ZTS
void php_ucache_release_thread_graph_pin_claims(php_ucache_globals *globals)
{
	php_ucache_graph_pin_slot_t *slot;
	uint32_t i;
	int expected;

	for (i = 0; i < globals->graph_pin_claim_count; i++) {
		if (!ucache_claim_header_is_attached(globals->graph_pin_claims[i].header)) {
			continue;
		}

		slot = &globals->graph_pin_claims[i].header->graph_pin_slots[
			globals->graph_pin_claims[i].slot_index
		];

		expected = zend_atomic_int_load_ex(&slot->owner_pid);
		if (expected != (int) (uint32_t) php_ucache_cached_pid() ||
			zend_atomic_int_load_ex(&slot->pin_count) != 0
		) {
			continue;
		}

		php_ucache_atomic_store_64(&slot->owner_start_time, 0);
		zend_atomic_int_compare_exchange_ex(&slot->owner_pid, &expected, 0);
	}

	globals->graph_pin_claim_count = 0;
}

void php_ucache_release_thread_reader_claims(php_ucache_globals *globals)
{
	php_ucache_reader_slot_t *slot;
	uint64_t my_pid;
	uint32_t i;

	if (globals->reader_claim_count == 0) {
		return;
	}

	my_pid = php_ucache_cached_pid();

	for (i = 0; i < globals->reader_claim_count; i++) {
		if (!ucache_claim_header_is_attached(globals->reader_claims[i].header)) {
			continue;
		}

		slot = &globals->reader_claims[i].header->reader_slots[globals->reader_claims[i].slot_index];

		ucache_atomic_store_32(&slot->active, 0);
		php_ucache_atomic_store_64(&slot->owner_start_time, 0);
		ucache_atomic_cas_64(&slot->owner_pid, my_pid, 0);
	}

	globals->reader_claim_count = 0;
}
#endif /* ZTS */

bool php_ucache_graph_pin_owner_is_dead(uint64_t owner_pid, uint64_t owner_start_time)
{
	return ucache_entry_lock_owner_is_dead(owner_pid, owner_start_time);
}

uint64_t php_ucache_self_start_time_token(void)
{
	return ucache_cached_self_start_time_token(php_ucache_cached_pid());
}

void php_ucache_reset_runtime(void)
{
	php_ucache_runtime_t *runtime = php_ucache_active_runtime();

	UC_G(runtime_resolved) = false;

	memset(runtime, 0, sizeof(*runtime));

	runtime->configured_memory = UC_G(shm_size);

	runtime->enabled = runtime->configured_memory != 0;
	if (!runtime->enabled) {
		runtime->failure_reason = PHP_UCACHE_REASON_DISABLED_BY_INI;
	}
}

void php_ucache_reset_storage(void)
{
	php_ucache_storage_t *storage = &php_ucache_active_context()->storage;

	memset(storage, 0, sizeof(*storage));

	storage->lock_file = -1;
}

bool php_ucache_header_init_locked(void)
{
	php_ucache_storage_t *storage = &php_ucache_active_context()->storage;
	php_ucache_header_t *header = php_ucache_header_ptr();

	if (!header) {
		return false;
	}

	ucache_header_layout_memo(storage);

	if (ucache_header_layout_matches_locked(storage, header)) {
		return ucache_header_identity_matches_locked(header);
	}

	return ucache_header_format_fresh_locked(header);
}

bool php_ucache_header_adoptable_locked(void)
{
	php_ucache_storage_t *storage = &php_ucache_active_context()->storage;
	php_ucache_header_t *header = php_ucache_header_ptr();

	if (!header) {
		return false;
	}

	ucache_header_layout_memo(storage);

	return ucache_header_layout_matches_locked(storage, header) &&
		ucache_header_identity_matches_locked(header)
	;
}

void php_ucache_free_locked(uint32_t payload_offset)
{
	php_ucache_header_t *header = php_ucache_header_ptr();
	php_ucache_block_t *block, *adjacent;
	uint32_t block_offset, original_block_offset, next_offset, prev_offset;

	if (!header || payload_offset < PHP_UCACHE_BLOCK_HEADER_UNITS) {
		return;
	}

	block_offset = payload_offset - PHP_UCACHE_BLOCK_HEADER_UNITS;
	original_block_offset = block_offset;
	block = php_ucache_block_ptr(block_offset);
	if (php_ucache_block_is_free(block)) {
		return;
	}

	next_offset = block_offset + block->size;
	if (next_offset < ucache_used_end_offset_locked(header)) {
		adjacent = php_ucache_block_ptr(next_offset);

		if (php_ucache_block_is_free(adjacent)) {
			ucache_free_list_remove_locked(header, next_offset);
			block->size += adjacent->size;

			if (header->last_block_offset == next_offset) {
				header->last_block_offset = block_offset;
			}
		}
	}

	if (block->prev_size != 0 && block_offset > header->data_offset) {
		prev_offset = block_offset - block->prev_size;

		adjacent = php_ucache_block_ptr(prev_offset);
		if (php_ucache_block_is_free(adjacent)) {
			ucache_free_list_remove_locked(header, prev_offset);

			block->size += adjacent->size;
			adjacent->size = block->size;
			block = adjacent;
			block_offset = prev_offset;

			if (header->last_block_offset == original_block_offset) {
				header->last_block_offset = block_offset;
			}
		}
	}

	ucache_update_following_prev_size_locked(header, block_offset, block);
	ucache_free_list_insert_locked(header, block_offset);
	ucache_trim_tail_free_blocks_locked(header, block_offset);
}

uint32_t php_ucache_alloc_locked(size_t size, const void *src)
{
	php_ucache_header_t *header = php_ucache_header_ptr();
	uint32_t total_size, offset;

	if (!header || !php_ucache_alloc_units(size, &total_size)) {
		return 0;
	}

	offset = ucache_alloc_from_free_list_locked(header, size, total_size, src);
	if (offset != 0) {
		return offset;
	}

	return ucache_alloc_from_tail_locked(header, size, total_size, src);
}

bool php_ucache_alloc_can_satisfy_locked(size_t size, size_t key_size)
{
	php_ucache_header_t *header = php_ucache_header_ptr();
	uint32_t free_offset, value_units, key_units = 0;
	size_t value_total, key_total, region, largest, second = 0;

	if (!header ||
		!php_ucache_alloc_units(size, &value_units) ||
		(key_size != 0 && !php_ucache_alloc_units(key_size, &key_units))
	) {
		return false;
	}

	value_total = value_units;
	key_total = key_units;

	region = header->next_free <= header->data_size
		? header->data_size - header->next_free
		: 0
	;
	largest = region;

	for (free_offset = header->free_list;
		free_offset != 0;
		free_offset = php_ucache_block_ptr(free_offset)->next_free
	) {
		region = php_ucache_block_ptr(free_offset)->size;

		if (region >= value_total + key_total) {
			return true;
		}

		if (region > largest) {
			second = largest;
			largest = region;
		} else if (region > second) {
			second = region;
		}
	}

	return largest >= value_total + key_total ||
		(largest >= value_total && (key_total == 0 || second >= key_total))
	;
}

static bool ucache_intern_string_matches(
		const php_ucache_header_t *header,
		uint32_t str_offset,
		const zend_string *str,
		zend_ulong hash)
{
	const zend_string *interned;
	size_t len = ZSTR_LEN(str);

	if (!php_ucache_payload_in_bounds(header, str_offset, _ZSTR_STRUCT_SIZE(len))) {
		return false;
	}

	interned = (const zend_string *) php_ucache_ptr(str_offset);

	return ZSTR_LEN(interned) == len &&
		ZSTR_H(interned) == hash &&
		memcmp(ZSTR_VAL(interned), ZSTR_VAL(str), len) == 0
	;
}

static void ucache_intern_table_insert_locked(
		php_ucache_header_t *header,
		uint32_t str_offset,
		zend_ulong hash)
{
	php_ucache_intern_slot_t *slots = php_ucache_intern_slots_ptr(header), *slot;
	uint32_t mask = header->intern_capacity - 1, i = (uint32_t) (hash & mask), probe;

	for (probe = 0; probe < header->intern_capacity; probe++) {
		slot = &slots[i];

		if (slot->str_offset == 0) {
			slot->tag = php_ucache_intern_tag(hash);
			PHP_UCACHE_ATOMIC_STORE_32_RELAXED(&slot->str_offset, str_offset);

			header->intern_count++;

			return;
		}

		i = (i + 1) & mask;
	}
}

/* Lock-free probe for a store being prepared outside the lock. Every read
 * is validated (bounds, length, hash, bytes) and bracketed by write_seq
 * like the optimistic readers; the caller must still confirm the returned
 * generation under the write lock before publishing the payload. */
uint32_t php_ucache_intern_find(zend_string *str, uint64_t *generation)
{
	php_ucache_storage_t *storage = &php_ucache_active_context()->storage;
	php_ucache_header_t *header = php_ucache_header_ptr();
	php_ucache_intern_slot_t *slots, *slot;
	zend_ulong hash;
	uint64_t seq;
	uint32_t i, mask, probe, str_offset, tag;

	*generation = 0;

	if (header == NULL ||
		!storage->layout_memo_valid ||
		header->magic != PHP_UCACHE_MAGIC ||
		header->version != PHP_UCACHE_VERSION ||
		header->intern_offset != storage->intern_offset_memo ||
		header->intern_capacity != storage->intern_capacity_memo ||
		header->data_offset != storage->data_offset_memo
	) {
		return 0;
	}

	seq = php_ucache_seq_load(&header->write_seq);
	if ((seq & 1) != 0) {
		return 0;
	}

	*generation = php_ucache_atomic_load_64(&header->intern_generation);

	hash = zend_string_hash_val(str);
	mask = header->intern_capacity - 1;
	tag = php_ucache_intern_tag(hash);
	slots = php_ucache_intern_slots_ptr(header);

	for (i = (uint32_t) (hash & mask), probe = 0; probe < header->intern_capacity; probe++, i = (i + 1) & mask) {
		slot = &slots[i];

		str_offset = PHP_UCACHE_ATOMIC_LOAD_32_RELAXED(&slot->str_offset);
		if (str_offset == 0) {
			break;
		}

		if (PHP_UCACHE_ATOMIC_LOAD_32_RELAXED(&slot->tag) != tag ||
			!ucache_intern_string_matches(header, str_offset, str, hash)
		) {
			continue;
		}

		php_ucache_atomic_fence_acquire();

		return php_ucache_seq_load(&header->write_seq) == seq ? str_offset : 0;
	}

	return 0;
}

/* Returns the unit offset of the new interned copy, or 0 when the table is
 * at its load limit, the block cannot be allocated or the offset would not
 * fit the flagged 31-bit string offset field. */
uint32_t php_ucache_intern_add_locked(zend_string *str)
{
	php_ucache_header_t *header = php_ucache_header_ptr();
	zend_string *interned;
	zend_ulong hash;
	uint32_t str_offset;
	size_t len, size;

	if (header == NULL || header->intern_saturated != 0) {
		return 0;
	}

	if (header->intern_count >= php_ucache_intern_load_limit(header->intern_capacity)) {
		header->intern_saturated = 1;

		return 0;
	}

	len = ZSTR_LEN(str);
	size = _ZSTR_STRUCT_SIZE(len);
	str_offset = php_ucache_alloc_locked(size, NULL);
	if (str_offset == 0) {
		return 0;
	}

	if ((str_offset & PHP_UCACHE_GRAPH_SEGMENT_STRING_FLAG) != 0) {
		php_ucache_free_locked(str_offset);

		return 0;
	}

	hash = zend_string_hash_val(str);
	interned = (zend_string *) php_ucache_ptr(str_offset);

	memcpy(interned, str, _ZSTR_HEADER_SIZE + len + 1);
	memset((uint8_t *) interned + _ZSTR_HEADER_SIZE + len + 1, 0, size - (_ZSTR_HEADER_SIZE + len + 1));

	/* Pinned as interned+permanent so the engine never refcounts or frees
	 * it; the sweep is the only thing that ever releases the block. */
	GC_SET_REFCOUNT(interned, 2);
	GC_TYPE_INFO(interned) = GC_STRING | ((IS_STR_INTERNED | IS_STR_PERMANENT) << GC_FLAGS_SHIFT);
	ZSTR_H(interned) = hash;

	ucache_intern_table_insert_locked(header, str_offset, hash);

	return str_offset;
}

/* Empties the slot table (the strings are the caller's to free) and starts
 * a new generation. */
void php_ucache_intern_table_reset_locked(void)
{
	php_ucache_header_t *header = php_ucache_header_ptr();

	if (header == NULL) {
		return;
	}

	memset(php_ucache_intern_slots_ptr(header), 0, PHP_UCACHE_INTERN_BYTES(header->intern_capacity));

	header->intern_count = 0;
	header->intern_saturated = 0;

	php_ucache_atomic_store_64(&header->intern_generation, header->intern_generation + 1);
}

void php_ucache_intern_table_insert_locked(uint32_t str_offset)
{
	php_ucache_header_t *header = php_ucache_header_ptr();

	if (header == NULL) {
		return;
	}

	ucache_intern_table_insert_locked(
		header,
		str_offset,
		ZSTR_H((const zend_string *) php_ucache_ptr(str_offset))
	);
}

bool php_ucache_startup_storage_before_request(void)
{
	php_ucache_storage_t *storage = &php_ucache_active_context()->storage;

	if (!ucache_is_opted_in()) {
		ucache_set_unavailable(PHP_UCACHE_REASON_SAPI_NOT_ENABLED);

		return true;
	}

	if (!ucache_startup_storage()) {
		ucache_set_unavailable(PHP_UCACHE_REASON_SHM_INIT_FAILED);

		return false;
	}

	storage->initialized_before_request = true;

	return true;
}

void php_ucache_shutdown_storage(void)
{
	php_ucache_storage_t *storage = &php_ucache_active_context()->storage;

	ucache_destroy_lock();
	ucache_cleanup_segments(
		storage->handler,
		storage->segments,
		storage->segment_count
	);
	php_ucache_reset_storage();
}

void php_ucache_ensure_ready_impl(void)
{
	php_ucache_ctx_t *ctx = php_ucache_active_context();

	ucache_resolve_runtime();

	UC_G(runtime_resolved) = true;
	UC_G(runtime_resolved_ctx) = ctx;
	UC_G(runtime_resolved_enabled) = UC_G(enable);
}

bool php_ucache_rlock(void)
{
	php_ucache_header_t *header;
	uint32_t recovery_attempts = 0;

	for (;;) {
		if (!ucache_rlock_impl()) {
			return false;
		}

		UC_G(lock_held_is_write) = false;

		header = php_ucache_header_ptr();
		if (EXPECTED(
				header == NULL ||
				!php_ucache_header_is_initialized_locked() ||
				(php_ucache_seq_load(&header->write_seq) & 1) == 0
			)
		) {
			return true;
		}

		php_ucache_unlock();

		if (++recovery_attempts > 8) {
			return false;
		}

		if (!php_ucache_wlock()) {
			return false;
		}

		php_ucache_unlock();
	}
}

bool php_ucache_wlock(void)
{
	if (!ucache_wlock_impl()) {
		return false;
	}

	return ucache_enter_write_locked_section();
}

bool php_ucache_wlock_for_ref_release(bool *write_section_entered)
{
	if (!ucache_wlock_impl()) {
		return false;
	}

	UC_G(lock_held_is_write) = true;

	*write_section_entered = ucache_recover_after_owner_death();

	if (*write_section_entered) {
		ucache_write_section_enter();
	} else {
		UC_G(write_seq_bumped) = false;
	}

	return true;
}

bool php_ucache_wlock_for_entry_mutation(zend_string *key)
{
	php_ucache_entry_lock_t *local_lock;
	php_ucache_header_t *header;
	php_ucache_entry_lock_record_t *record;
	zend_ulong hash = zend_string_hash_val(key);
	uint64_t waited_us = 0;
	uint32_t slot_idx;
	bool found;

	ucache_ensure_entry_lock_owner();
	ucache_drain_deferred_entry_lock_releases();

	for (;;) {
		if (!php_ucache_wlock()) {
			return false;
		}

		if (!php_ucache_header_init_locked()) {
			php_ucache_unlock();

			return false;
		}

		header = php_ucache_header_ptr();
		if (!ucache_find_entry_lock_record_slot_locked(header, key, hash, &slot_idx, &found) || !found) {
			return true;
		}

		local_lock = ucache_find_local_entry_lock(php_ucache_active_context(), key);
		record = &php_ucache_entry_lock_records_ptr(header)[slot_idx];
		if (local_lock != NULL &&
			record->owner_pid == local_lock->owner_pid &&
			record->owner_token == local_lock->owner_token
		) {
			return true;
		}

		php_ucache_unlock();

		if (waited_us >= PHP_UCACHE_ENTRY_LOCK_WAIT_TIMEOUT_US) {
			return false;
		}

		waited_us += ucache_sleep_entry_lock_retry_interval();
	}
}

void php_ucache_unlock(void)
{
	if (UC_G(lock_held_is_write)) {
		ucache_write_section_leave();
	}

	UC_G(lock_held_is_write) = false;

	ucache_unlock_impl();
}

void php_ucache_unlock_if_held(void)
{
	if (UC_G(lock_held)) {
		php_ucache_unlock();
	}
}

bool php_ucache_try_acquire_entry_lock(zend_string *key, zend_long lease)
{
	return ucache_acquire_entry_lock_record(
		php_ucache_active_context(),
		key,
		lease
	);
}

bool php_ucache_acquire_entry_locks(zend_string **keys, bool *acquired, uint32_t count)
{
	return ucache_acquire_entry_lock_records(
		php_ucache_active_context(),
		keys,
		acquired,
		count
	);
}

bool php_ucache_release_entry_lock(zend_string *key)
{
	php_ucache_ctx_t *ctx = php_ucache_active_context();
	php_ucache_entry_lock_t *lock;
	zend_ulong hash;
	HashTable **locks_ptr = ucache_entry_lock_table_ptr();

	ucache_ensure_entry_lock_owner();

	lock = ucache_find_local_entry_lock(ctx, key);
	if (lock == NULL) {
		return false;
	}

	hash = zend_string_hash_val(key);
	if (!ucache_remove_owned_entry_lock_record(ctx, key, hash, lock->owner_pid, lock->owner_token)) {
		return false;
	}

	zend_hash_del(*locks_ptr, key);
	ucache_destroy_entry_locks_if_empty(locks_ptr);

	return true;
}

bool php_ucache_request_owns_entry_lock(zend_string *key)
{
	ucache_ensure_entry_lock_owner();

	return ucache_find_local_entry_lock(php_ucache_active_context(), key) != NULL;
}

void php_ucache_release_entry_locks(zend_string **keys, const bool *acquired, uint32_t count)
{
	ucache_release_entry_locks_for_keys(
		php_ucache_active_context(),
		keys,
		acquired,
		count
	);
}

bool php_ucache_entry_key_lock_active_locked(
		php_ucache_header_t *header,
		zend_ulong hash,
		uint32_t key_offset,
		uint32_t key_len,
		uint64_t *now)
{
	php_ucache_entry_lock_record_t *record;
	uint32_t i, probe;

	for (probe = 0; probe < header->entry_lock_capacity; probe++) {
		i = (ucache_entry_lock_table_index(header, hash) + probe) & (header->entry_lock_capacity - 1);
		record = &php_ucache_entry_lock_records_ptr(header)[i];

		if (record->state == PHP_UCACHE_ENTRY_LOCK_EMPTY) {
			return false;
		}

		if (record->state != PHP_UCACHE_ENTRY_LOCK_USED ||
			record->hash != hash ||
			record->key_len != key_len
		) {
			continue;
		}

		if (memcmp(php_ucache_ptr(record->key_offset), php_ucache_ptr(key_offset), key_len) != 0) {
			continue;
		}

		if (*now == 0) {
			*now = php_ucache_time_rel(header, (uint64_t) time(NULL));
		}

		return ucache_entry_lock_record_is_active_locked(record, *now, false);
	}

	return false;
}

bool php_ucache_entry_locks_allow_clear_locked(void)
{
	php_ucache_header_t *header = php_ucache_header_ptr();
	php_ucache_entry_lock_record_t *record;
	uint64_t owner_pid, now;
	uint32_t i;

	ucache_ensure_entry_lock_owner();

	if (header == NULL || !php_ucache_header_adoptable_locked()) {
		return true;
	}

	owner_pid = php_ucache_cached_pid();
	now = php_ucache_time_rel(header, (uint64_t) time(NULL));

	for (i = 0; i < header->entry_lock_capacity; i++) {
		record = &php_ucache_entry_lock_records_ptr(header)[i];
		if (record->state != PHP_UCACHE_ENTRY_LOCK_USED) {
			continue;
		}

		if (!ucache_entry_lock_record_is_active_locked(record, now, true)) {
			ucache_remove_entry_lock_record_locked(record);

			continue;
		}

		if (record->owner_pid != owner_pid) {
			return false;
		}
	}

	return true;
}

void php_ucache_release_request_entry_locks(void)
{
	ucache_ensure_entry_lock_owner();
	ucache_drain_deferred_entry_lock_releases();
	ucache_release_entry_locks_all_contexts(
		&UC_G(entry_lock_table),
		PHP_UCACHE_ENTRY_LOCK_RELEASE_PRESERVE_LEASES
	);
#ifndef ZEND_WIN32
	UC_G(entry_lock_owner_pid) = 0;
#endif
}

#ifndef ZEND_WIN32
/* Commit every block up front so no later page fault can hit ENOSPC (SIGBUS
 * on a sparse file). Returns false with errno set. */
bool php_ucache_preallocate_fd(int fd, size_t size)
{
#if defined(HAVE_POSIX_FALLOCATE)
	int result;
#elif defined(__APPLE__)
	fstore_t store;
#endif

	if (size > (size_t) SSIZE_MAX) {
		errno = EOVERFLOW;

		return false;
	}

#if defined(HAVE_POSIX_FALLOCATE)
	do {
		result = posix_fallocate(fd, 0, (off_t) size);
	} while (result == EINTR);

	if (result == 0) {
		return true;
	}

	if (result != EOPNOTSUPP &&
#if defined(ENOTSUP) && ENOTSUP != EOPNOTSUPP
		result != ENOTSUP &&
#endif
		result != ENOSYS &&
		result != EINVAL &&
		result != ENODEV
	) {
		errno = result;

		return false;
	}
#elif defined(__APPLE__)
	memset(&store, 0, sizeof(store));
	store.fst_flags = F_ALLOCATECONTIG | F_ALLOCATEALL;
	store.fst_posmode = F_PEOFPOSMODE;
	store.fst_offset = 0;
	store.fst_length = (off_t) size;

	if (fcntl(fd, F_PREALLOCATE, &store) == 0) {
		return ftruncate(fd, (off_t) size) == 0;
	}

	store.fst_flags = F_ALLOCATEALL;
	if (fcntl(fd, F_PREALLOCATE, &store) == 0) {
		return ftruncate(fd, (off_t) size) == 0;
	}
#endif /* defined(HAVE_POSIX_FALLOCATE) */

	return ucache_posix_zero_fill_fd(fd, size);
}
#endif /* !ZEND_WIN32 */

#ifdef ZTS
void php_ucache_free_thread_deferred_entry_lock_releases(php_ucache_globals *globals)
{
	uint32_t i;

	for (i = 0; i < globals->deferred_entry_lock_release_count; i++) {
		pefree(globals->deferred_entry_lock_releases[i].key, true);
	}

	pefree(globals->deferred_entry_lock_releases, true);

	globals->deferred_entry_lock_releases = NULL;
	globals->deferred_entry_lock_release_count = 0;
	globals->deferred_entry_lock_release_capacity = 0;
}
#endif /* ZTS */
