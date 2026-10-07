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

#include "user_cache_storage.h"

#define UCACHE_ENTRY_LOCK_SWEEP_OP_INTERVAL		64U

#define UCACHE_NSEC_PER_SEC					1000000000L
#define UCACHE_ROBUST_MUTEX_RECHECK_NSEC	(50L * 1000L * 1000L)
#define UCACHE_FCNTL_DEADLOCK_RETRY_US		1000U
#ifdef HAVE_PTHREAD_MUTEX_CLOCKLOCK
# define UCACHE_ROBUST_MUTEX_CLOCK			CLOCK_MONOTONIC
# define UCACHE_ROBUST_MUTEX_LOCK_UNTIL(mutex, deadline) \
	pthread_mutex_clocklock((mutex), CLOCK_MONOTONIC, (deadline))
#else
# define UCACHE_ROBUST_MUTEX_CLOCK			CLOCK_REALTIME
# define UCACHE_ROBUST_MUTEX_LOCK_UNTIL(mutex, deadline) \
	pthread_mutex_timedlock((mutex), (deadline))
#endif

#define UCACHE_SEM_FILENAME_PREFIX	".PhpUserCacheSem."

struct _ucache_lock_ops {
	const char *name;
	bool (*rlock)(ucache_storage *storage);
	bool (*wlock)(ucache_storage *storage);
	void (*unlock)(ucache_storage *storage);
};

#ifdef ZEND_WIN32
static bool ucache_win32_rlock(ucache_storage *storage);
static bool ucache_win32_wlock(ucache_storage *storage);
static void ucache_win32_unlock(ucache_storage *storage);
#else
#ifdef UCACHE_HAVE_SHARED_MUTEX
static bool ucache_shared_mutex_lock(ucache_storage *storage);
static void ucache_shared_mutex_unlock(ucache_storage *storage);
#endif /* UCACHE_HAVE_SHARED_MUTEX */
static bool ucache_fcntl_rlock(ucache_storage *storage);
static bool ucache_fcntl_wlock(ucache_storage *storage);
static void ucache_fcntl_unlock(ucache_storage *storage);
#endif /* ZEND_WIN32 */

#ifdef ZEND_WIN32
static const ucache_lock_ops ucache_win32_lock_ops = {
	"win32",
	ucache_win32_rlock,
	ucache_win32_wlock,
	ucache_win32_unlock
};
#else
#ifdef UCACHE_HAVE_SHARED_MUTEX
static const ucache_lock_ops ucache_shared_mutex_lock_ops = {
	"mutex",
	ucache_shared_mutex_lock,
	ucache_shared_mutex_lock,
	ucache_shared_mutex_unlock
};
#endif /* UCACHE_HAVE_SHARED_MUTEX */

static const ucache_lock_ops ucache_fcntl_lock_ops = {
	"fcntl",
	ucache_fcntl_rlock,
	ucache_fcntl_wlock,
	ucache_fcntl_unlock
};
#endif /* ZEND_WIN32 */

#ifdef ZEND_WIN32
const ucache_platform_ops ucache_platform = {
	ucache_win32_sleep_us,
	ucache_win32_proc_start_time_token,
	ucache_win32_proc_has_exited,
	ucache_win32_alloc_err_code,
	ucache_win32_log_alloc_failure
};
#else
const ucache_platform_ops ucache_platform = {
	ucache_posix_sleep_us,
	ucache_posix_proc_start_time_token,
	ucache_posix_proc_has_exited,
	ucache_posix_alloc_err_code,
	ucache_posix_log_alloc_failure
};
#endif /* ZEND_WIN32 */

#ifdef UCACHE_HAVE_SCALAR_WRITE
static zend_always_inline void ucache_scalar_write_back_off(ucache_scalar_write_stripe *stripe)
{
	ucache_atomic_store_32(&stripe->active, 0);

	pthread_mutex_unlock(&stripe->mutex.mutex);
}
#endif /* UCACHE_HAVE_SCALAR_WRITE */

static zend_always_inline bool ucache_hdr_is_initialized_acquire(void)
{
	ucache_hdr *hdr = ucache_hdr_ptr();

	if (hdr == NULL) {
		return false;
	}

	if (ucache_atomic_load_32(&hdr->magic) != UCACHE_MAGIC) {
		return false;
	}

	ucache_atomic_fence_acquire();

	return true;
}

#ifdef ZTS
static inline bool ucache_zts_lock_init(ucache_storage *storage)
{
	storage->zts_lock = tsrm_mutex_alloc();

	return storage->zts_lock != NULL;
}

static inline void ucache_zts_lock_destroy(ucache_storage *storage)
{
	tsrm_mutex_free(storage->zts_lock);
	storage->zts_lock = NULL;
}

static inline bool ucache_zts_lock(ucache_storage *storage)
{
	return tsrm_mutex_lock(storage->zts_lock) == 0;
}

static inline void ucache_zts_unlock(ucache_storage *storage)
{
	tsrm_mutex_unlock(storage->zts_lock);
}
#else
static inline bool ucache_zts_lock_init(ucache_storage *storage)
{
	(void) storage;

	return true;
}

static inline void ucache_zts_lock_destroy(ucache_storage *storage)
{
	(void) storage;
}

static inline bool ucache_zts_lock(ucache_storage *storage)
{
	(void) storage;

	return true;
}

static inline void ucache_zts_unlock(ucache_storage *storage)
{
	(void) storage;
}
#endif /* ZTS */

#ifndef ZEND_WIN32
#ifdef UCACHE_HAVE_SHARED_MUTEX
/* Waiters recheck periodically: a robust mutex handoff can lose its wakeup when processes are killed during it. */
static int ucache_robust_mutex_lock(pthread_mutex_t *mutex)
{
	struct timespec deadline;
	int result = pthread_mutex_trylock(mutex);

	while (result == EBUSY || result == ETIMEDOUT) {
		clock_gettime(UCACHE_ROBUST_MUTEX_CLOCK, &deadline);

		deadline.tv_nsec += UCACHE_ROBUST_MUTEX_RECHECK_NSEC;
		if (deadline.tv_nsec >= UCACHE_NSEC_PER_SEC) {
			deadline.tv_sec++;
			deadline.tv_nsec -= UCACHE_NSEC_PER_SEC;
		}

		result = UCACHE_ROBUST_MUTEX_LOCK_UNTIL(mutex, &deadline);
	}

	return result;
}

static uint64_t ucache_scalar_next_gen(ucache_hdr *hdr)
{
	uint64_t expected, desired;

	for (;;) {
		expected = ucache_atomic_load_64(&hdr->mutation_epoch);
		desired = expected + 1;

		if (desired == 0) {
			desired = 1;
		}

		if (ucache_atomic_cas_64(&hdr->mutation_epoch, expected, desired)) {
			return desired;
		}
	}
}

static void ucache_scalar_write_undo_if_pending(ucache_hdr *hdr, ucache_scalar_write_stripe *stripe)
{
	ucache_entry *entry;
	uint64_t seq = ucache_atomic_load_64(&stripe->seq);

	if ((seq & 1) == 0) {
		return;
	}

	entry = &ucache_entries_ptr(hdr)[stripe->slot_idx];

	ZEND_ASSERT(ucache_entry_holds_scalar(entry));

	memcpy(&entry->long_val, &stripe->old_val, sizeof(stripe->old_val));

	entry->flags = stripe->old_flags;
	entry->gen = stripe->old_gen;

	ZEND_ASSERT(ucache_entry_holds_scalar(entry));

	(void) ucache_scalar_next_gen(hdr);
	ucache_seq_publish(&stripe->seq, seq + 1);
}

#ifdef UCACHE_HAVE_SCALAR_WRITE
static bool ucache_entry_lock_hash_present(ucache_hdr *hdr, zend_ulong hash)
{
	ucache_entry_lock_record *records = ucache_entry_lock_records_ptr(hdr);
	uint32_t i, probe;

	for (probe = 0; probe < hdr->entry_lock_capacity; probe++) {
		i = (ucache_entry_lock_table_idx(hdr, hash) + probe) & (hdr->entry_lock_capacity - 1);

		if (records[i].state == UCACHE_ENTRY_LOCK_EMPTY) {
			return false;
		}

		if (records[i].state == UCACHE_ENTRY_LOCK_USED && records[i].hash == hash) {
			return true;
		}
	}

	return false;
}
#endif /* UCACHE_HAVE_SCALAR_WRITE */

static zend_never_inline bool ucache_quiesce_scalar_writers(ucache_hdr *hdr)
{
	ucache_scalar_write_stripe *stripe;
	uint32_t i;
	int result;

	ucache_atomic_store_32(&hdr->scalar_write_gate, 1);
	ucache_atomic_fence_seq_cst();
	for (i = 0; i < UCACHE_SCALAR_WRITE_STRIPES; i++) {
		stripe = &hdr->scalar_write_stripes[i];
		if (ucache_atomic_load_32(&stripe->active) == 0 &&
			(ucache_atomic_load_64(&stripe->seq) & 1) == 0
		) {
			continue;
		}

		result = ucache_robust_mutex_lock(&stripe->mutex.mutex);
		if (result == EOWNERDEAD) {
			result = pthread_mutex_consistent(&stripe->mutex.mutex);
			if (result != 0) {
				pthread_mutex_unlock(&stripe->mutex.mutex);
			}
		}

		if (result != 0) {
			return false;
		}

		ucache_scalar_write_undo_if_pending(hdr, stripe);
		ucache_atomic_store_32(&stripe->active, 0);

		pthread_mutex_unlock(&stripe->mutex.mutex);
	}

	return true;
}

static bool ucache_shared_mutex_lock(ucache_storage *storage)
{
	ucache_hdr *hdr = ucache_hdr_ptr();
	int result;

	(void) storage;

	if (hdr == NULL) {
		return false;
	}

	result = ucache_robust_mutex_lock(&hdr->global_shared_mutex.mutex);
	if (result == EOWNERDEAD) {
		if (pthread_mutex_consistent(&hdr->global_shared_mutex.mutex) != 0) {
			pthread_mutex_unlock(&hdr->global_shared_mutex.mutex);

			return false;
		}

		result = 0;
	}

	if (result != 0) {
		return false;
	}

	if (UCACHE_OPTIMISTIC_ENABLED &&
		UNEXPECTED(ucache_atomic_load_32(&hdr->scalar_write_enabled) != 0) &&
		!ucache_quiesce_scalar_writers(hdr)
	) {
		ucache_atomic_store_32(&hdr->scalar_write_gate, 0);
		pthread_mutex_unlock(&hdr->global_shared_mutex.mutex);

		return false;
	}

	UC_G(lock_held) = true;

	return true;
}

static void ucache_shared_mutex_unlock(ucache_storage *storage)
{
	ucache_hdr *hdr = ucache_hdr_ptr();

	(void) storage;

	if (hdr != NULL) {
		if (ucache_atomic_load_32(&hdr->scalar_write_enabled) != 0) {
			ucache_atomic_store_32(&hdr->scalar_write_gate, 0);
		}

		pthread_mutex_unlock(&hdr->global_shared_mutex.mutex);
	}

	UC_G(lock_held) = false;
}
#endif /* UCACHE_HAVE_SHARED_MUTEX */

static bool ucache_fcntl_lock(ucache_storage *storage, short lock_type)
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
		if (errno == EDEADLK) {
			ucache_platform.sleep_us(UCACHE_FCNTL_DEADLOCK_RETRY_US);
		} else if (errno != EINTR) {
			ucache_zts_unlock(storage);

			return false;
		}
	}

	UC_G(lock_held) = true;

	return true;
}

static bool ucache_fcntl_rlock(ucache_storage *storage)
{
	return ucache_fcntl_lock(storage, F_RDLCK);
}

static bool ucache_fcntl_wlock(ucache_storage *storage)
{
	return ucache_fcntl_lock(storage, F_WRLCK);
}

static void ucache_fcntl_unlock(ucache_storage *storage)
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
#else
static bool ucache_win32_open_lock_file_at(
		ucache_storage *storage,
		const char *dir,
		const char *base_name,
		bool delete_on_close)
{
	size_t dir_len;
	const char *sep;
	char lockfile_name[MAXPATHLEN];

	if (dir == NULL || dir[0] == '\0') {
		return false;
	}

	dir_len = strlen(dir);
	sep = dir[dir_len - 1] == '/' || dir[dir_len - 1] == '\\'
		? ""
		: "/"
	;

	snprintf(
		lockfile_name,
		sizeof(lockfile_name),
		"%s%s%s.lock",
		dir,
		sep,
		base_name
	);

	storage->lock_file = php_win32_ioutil_open(
		lockfile_name,
		O_RDWR | O_CREAT | O_BINARY | (delete_on_close ? _O_TEMPORARY | O_EXCL : 0),
		0666
	);
	if (storage->lock_file < 0) {
		return false;
	}

	if (!ucache_win32_owned_by_cur_user((HANDLE) _get_osfhandle(storage->lock_file), SE_FILE_OBJECT)) {
		php_win32_ioutil_close(storage->lock_file);
		storage->lock_file = -1;

		return false;
	}

	return true;
}

static bool ucache_win32_open_lock_file(ucache_ctx *ctx)
{
	const char *err_in;
	ucache_storage *storage = &ctx->storage;
	DWORD tmp_path_w_len;
	wchar_t tmp_path_w[MAXPATHLEN];
	size_t tmp_path_len;
	char base_name[MAXPATHLEN], *tmp_path, *p;
	bool opened;

	if (!ucache_win32_ctx_is_proc_private(ctx) && !ucache_win32_load_salt(&err_in)) {
		return false;
	}

	ucache_win32_format_name(
		base_name,
		sizeof(base_name),
		UCACHE_WIN32_LOCK_FILE_NAME,
		storage->size
	);

	for (p = base_name; *p != '\0'; p++) {
		if ((uint8_t) *p < 32 || strchr("<>:\"/\\|?*", *p) != NULL) {
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

	opened = ucache_win32_open_lock_file_at(
		storage,
		tmp_path,
		base_name,
		ucache_win32_ctx_is_proc_private(ctx)
	);

	free(tmp_path);

	return opened;
}

static bool ucache_win32_lock_range(ucache_storage *storage, bool exclusive)
{
	OVERLAPPED overlapped;
	HANDLE handle;

	if (!storage->lock_initialized || storage->lock_file < 0) {
		return false;
	}

	handle = (HANDLE) _get_osfhandle(storage->lock_file);
	if (handle == INVALID_HANDLE_VALUE) {
		return false;
	}

	memset(&overlapped, 0, sizeof(overlapped));

	return LockFileEx(handle, exclusive ? LOCKFILE_EXCLUSIVE_LOCK : 0, 0, 1, 0, &overlapped) == TRUE;
}

static void ucache_win32_unlock_range(ucache_storage *storage)
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

static bool ucache_win32_lock(ucache_storage *storage, bool exclusive)
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

static bool ucache_win32_rlock(ucache_storage *storage)
{
	return ucache_win32_lock(storage, false);
}

static bool ucache_win32_wlock(ucache_storage *storage)
{
	return ucache_win32_lock(storage, true);
}

static void ucache_win32_unlock(ucache_storage *storage)
{
	ucache_win32_unlock_range(storage);

	ucache_zts_unlock(storage);

	UC_G(lock_held) = false;
}
#endif /* ZEND_WIN32 */

static bool ucache_rlock_impl(void)
{
	ucache_storage *storage = &ucache_active_ctx()->storage;

	ZEND_ASSERT(!UC_G(lock_held) && !UC_G(write_seq_bumped));

	return !UC_G(lock_held) && storage->lock_initialized && storage->lock_ops->rlock(storage);
}

static bool ucache_wlock_impl(void)
{
	ucache_storage *storage = &ucache_active_ctx()->storage;

	ZEND_ASSERT(!UC_G(lock_held) && !UC_G(write_seq_bumped));

	return !UC_G(lock_held) && storage->lock_initialized && storage->lock_ops->wlock(storage);
}

static void ucache_unlock_impl(void)
{
	ucache_storage *storage = &ucache_active_ctx()->storage;

	if (storage->lock_initialized) {
		storage->lock_ops->unlock(storage);
	}
}

static bool ucache_wlock_recovered(void)
{
	if (!ucache_wlock_impl()) {
		return false;
	}

	UC_G(lock_held_is_write) = true;

	if (!ucache_recover_after_owner_death_locked()) {
		UC_G(lock_held_is_write) = false;

		ucache_unlock_impl();

		return false;
	}

	return true;
}

static void ucache_write_section_enter(void)
{
	ucache_hdr *hdr = ucache_hdr_ptr();

	ZEND_ASSERT(UC_G(lock_held_is_write) && !UC_G(write_seq_bumped));

	if (hdr == NULL ||
		!ucache_hdr_is_initialized_locked()
	) {
		return;
	}

	ucache_seq_announce(&hdr->write_seq, hdr->write_seq + 1);

	UC_G(write_seq_bumped) = true;
	UC_G(reader_drain_state) = 0;

	UCACHE_DEBUG_SIMULATE_KILL("EXIT_IN_WRITE_SECTION");
}

static void ucache_write_section_leave(void)
{
	ucache_hdr *hdr = ucache_hdr_ptr();

	if (UC_G(entry_lock_table_section_open)) {
		UC_G(entry_lock_table_section_open) = false;

		if (hdr != NULL) {
			hdr->entry_lock_table_section_open = 0;
		}
	}

	if (!UC_G(write_seq_bumped)) {
		return;
	}

	UC_G(write_seq_bumped) = false;

	if (hdr == NULL) {
		return;
	}

	ucache_seq_publish(&hdr->write_seq, hdr->write_seq + 1);
}

static bool ucache_wlock_for_entry_mutations_within(
		zend_string **keys,
		uint32_t count,
		uint64_t wait_budget_us)
{
	ucache_entry_lock *local_lock;
	ucache_entry_lock_record *record;
	ucache_entry_lock_holder holder;
	ucache_hdr *hdr;
	uint64_t waited_us = 0, now;
	uint32_t i, slot_idx;
	bool found;

	ucache_ensure_entry_lock_owner();
	ucache_drain_deferred_entry_lock_releases();

	for (;;) {
		if (!ucache_wlock()) {
			return false;
		}

		if (!ucache_hdr_init_locked()) {
			ucache_unlock();

			return false;
		}

		hdr = ucache_hdr_ptr();
		if (hdr->entry_lock_count != 0 &&
			++UC_G(entry_lock_sweep_ops) >= UCACHE_ENTRY_LOCK_SWEEP_OP_INTERVAL
		) {
			UC_G(entry_lock_sweep_ops) = 0;

			now = ucache_time_rel(hdr, ucache_clock_now());
			if (now >= hdr->entry_lock_sweep_at) {
				ucache_sweep_entry_locks_locked(hdr, now);
			}
		}

		if (hdr->entry_lock_count == 0) {
			return true;
		}

		for (i = 0; i < count; i++) {
			if (
				!ucache_find_entry_lock_record_slot_locked(
					hdr,
					keys[i],
					zend_string_hash_val(keys[i]),
					&slot_idx,
					&found
				) || !found
			) {
				continue;
			}

			record = &ucache_entry_lock_records_ptr(hdr)[slot_idx];
			local_lock = ucache_find_local_entry_lock(
				ucache_active_ctx(),
				keys[i]
			);

			if (local_lock == NULL ||
				record->owner_pid != local_lock->owner_pid ||
				record->owner_token != local_lock->owner_token
			) {
				ucache_entry_lock_holder_capture(&holder, hdr, slot_idx);

				break;
			}
		}

		if (i == count) {
			return true;
		}

		ucache_unlock();

		if (waited_us >= wait_budget_us) {
			return false;
		}

		do {
			waited_us += ucache_sleep_entry_lock_retry_interval(waited_us);
		} while (waited_us < wait_budget_us &&
			ucache_entry_lock_holder_unchanged(&ucache_active_ctx()->storage, &holder)
		);
	}
}

#ifndef ZEND_WIN32
#ifdef UCACHE_HAVE_SHARED_MUTEX
bool ucache_shared_mutex_init(ucache_hdr *hdr)
{
	pthread_mutexattr_t attr;
	uint32_t i;
	bool result;

	if (UCACHE_DEBUG_FAULT("FORCE_FCNTL_LOCK_MODEL")) {
		return false;
	}

	if (pthread_mutexattr_init(&attr) != 0) {
		return false;
	}

	result = pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED) == 0 &&
		pthread_mutexattr_setrobust(&attr, PTHREAD_MUTEX_ROBUST) == 0 &&
		pthread_mutex_init(&hdr->global_shared_mutex.mutex, &attr) == 0
	;
	if (result) {
		for (i = 0; i < UCACHE_SCALAR_WRITE_STRIPES; i++) {
			if (pthread_mutex_init(&hdr->scalar_write_stripes[i].mutex.mutex, &attr) != 0) {
				break;
			}
		}

		if (i == UCACHE_SCALAR_WRITE_STRIPES) {
			hdr->scalar_write_ready = 1;
		} else {
			while (i != 0) {
				pthread_mutex_destroy(&hdr->scalar_write_stripes[--i].mutex.mutex);
			}
		}
	}

	pthread_mutexattr_destroy(&attr);

	return result;
}
#endif /* UCACHE_HAVE_SHARED_MUTEX */

const ucache_lock_ops *ucache_lock_ops_for_model(uint32_t lock_model)
{
#ifdef UCACHE_HAVE_SHARED_MUTEX
	if (lock_model == UCACHE_LOCK_MODEL_MUTEX) {
		return &ucache_shared_mutex_lock_ops;
	}
#else
	(void) lock_model;
#endif

	return &ucache_fcntl_lock_ops;
}

bool ucache_create_lock(void)
{
	ucache_ctx *ctx = ucache_active_ctx();
	ucache_storage *storage = &ctx->storage;
	int val;
	char lockfile_name[MAXPATHLEN];

	if (storage->lock_initialized) {
		return true;
	}

	if (!ucache_zts_lock_init(storage)) {
		return false;
	}

	storage->lock_ops = &ucache_fcntl_lock_ops;

#ifdef UCACHE_HAVE_BOUNDARY_SHM
	if (ucache_ctx_is_boundary(ctx)) {
		storage->lock_file = fcntl(ucache_boundary_seg_of(storage)->lock_fd, F_DUPFD_CLOEXEC, 0);
		if (storage->lock_file < 0) {
			ucache_zts_lock_destroy(storage);

			return false;
		}

		storage->lock_initialized = true;

		return true;
	}
#endif /* UCACHE_HAVE_BOUNDARY_SHM */

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
		lockfile_name,
		sizeof(lockfile_name),
		"%s/" UCACHE_SEM_FILENAME_PREFIX "XXXXXX",
		UC_G(lockfile_path)
	);

	storage->lock_file = mkstemp(lockfile_name);
	if (storage->lock_file == -1) {
		ucache_zts_lock_destroy(storage);

		return false;
	}

	val = fcntl(storage->lock_file, F_GETFD, 0);
	val |= FD_CLOEXEC;

	fcntl(storage->lock_file, F_SETFD, val);
	unlink(lockfile_name);

	storage->lock_initialized = true;

	return true;
}

void ucache_destroy_lock(void)
{
	ucache_storage *storage = &ucache_active_ctx()->storage;

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
#else
const ucache_lock_ops *ucache_lock_ops_for_model(uint32_t lock_model)
{
	(void) lock_model;

	return &ucache_win32_lock_ops;
}

bool ucache_create_lock(void)
{
	ucache_ctx *ctx = ucache_active_ctx();
	ucache_storage *storage = &ctx->storage;

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

void ucache_destroy_lock(void)
{
	ucache_storage *storage = &ucache_active_ctx()->storage;

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
#endif /* ZEND_WIN32 */

bool ucache_wlock_entry_lock_table(void)
{
	ucache_hdr *hdr;

	if (!ucache_wlock_recovered()) {
		return false;
	}

	hdr = ucache_hdr_ptr();
	if (hdr != NULL && ucache_hdr_is_initialized_locked()) {
		hdr->entry_lock_table_section_open = 1;

		UC_G(entry_lock_table_section_open) = true;
	}

	return true;
}

void ucache_write_section_announce(ucache_hdr *hdr)
{
	if (!UC_G(lock_held_is_write) || UC_G(write_seq_bumped)) {
		return;
	}

	ucache_seq_announce(&hdr->write_seq, hdr->write_seq + 1);

	UC_G(write_seq_bumped) = true;
	UC_G(reader_drain_state) = 0;
}

bool ucache_wlock_negotiated_lock_model(void)
{
	const ucache_lock_ops *negotiated_ops;
	ucache_storage *storage = &ucache_active_ctx()->storage;

	if (!ucache_wlock_impl()) {
		return false;
	}

	negotiated_ops = ucache_hdr_is_initialized_acquire()
		? ucache_lock_ops_for_model(ucache_hdr_ptr()->lock_model)
		: storage->lock_ops
	;

	if (negotiated_ops != storage->lock_ops) {
		ucache_unlock_impl();

		storage->lock_ops = negotiated_ops;

		if (!ucache_wlock_impl()) {
			return false;
		}
	}

#ifdef UCACHE_HAVE_BOUNDARY_SHM
	if (ucache_hdr_is_initialized_acquire() &&
		ucache_hdr_boundary_lock_file_replaced_locked(ucache_hdr_ptr())
	) {
		ucache_unlock_impl();

		ucache_shared_boundary_retire_seg_name();

		return false;
	}
#endif /* UCACHE_HAVE_BOUNDARY_SHM */

	UC_G(lock_held_is_write) = true;

	if (!ucache_recover_after_owner_death_locked()) {
		UC_G(lock_held_is_write) = false;

		ucache_unlock_impl();

		return false;
	}

	ucache_write_section_enter();

	return true;
}

const char *ucache_lock_model_name(const ucache_storage *storage)
{
	return storage->lock_initialized && storage->lock_ops != NULL ? storage->lock_ops->name : "none";
}

bool ucache_rlock(void)
{
	ucache_hdr *hdr;
	uint32_t recovery_attempts = 0;

	for (;;) {
		if (!ucache_rlock_impl()) {
			return false;
		}

		UC_G(lock_held_is_write) = false;

		hdr = ucache_hdr_ptr();
		if (!UNEXPECTED(
				hdr != NULL &&
				ucache_hdr_is_initialized_locked() &&
				(
					(ucache_atomic_load_64(&hdr->write_seq) & 1) != 0 ||
					hdr->entry_lock_table_section_open != 0
				)
			)
		) {
			return true;
		}

		ucache_unlock();

		if (++recovery_attempts > 8) {
			return false;
		}

		UCACHE_DEBUG_SIMULATE_KILL("EXIT_BEFORE_READ_LOCK_RECOVERY");

		if (!ucache_wlock_recovered()) {
			return false;
		}

		ucache_unlock();
	}
}

bool ucache_wlock(void)
{
	if (!ucache_wlock_recovered()) {
		return false;
	}

	ucache_write_section_enter();

	return true;
}

bool ucache_wlock_for_ref_release(bool *recovered)
{
	if (!ucache_wlock_impl()) {
		return false;
	}

	UC_G(lock_held_is_write) = true;

	if (ucache_recover_after_owner_death_locked()) {
		*recovered = true;

		ucache_write_section_enter();

		return true;
	}

	*recovered = false;

	return true;
}

bool ucache_wlock_for_entry_mutations(zend_string **keys, uint32_t count)
{
	return ucache_wlock_for_entry_mutations_within(keys, count, ucache_entry_lock_wait_timeout_us());
}

bool ucache_wlock_for_entry_mutation(zend_string *key)
{
	return ucache_wlock_for_entry_mutations_within(&key, 1, ucache_entry_lock_wait_timeout_us());
}

bool ucache_try_wlock_for_entry_mutation(zend_string *key)
{
	return ucache_wlock_for_entry_mutations_within(&key, 1, 0);
}

#ifdef UCACHE_HAVE_SCALAR_WRITE
void ucache_scalar_write_enable_locked(ucache_hdr *hdr)
{
	ZEND_ASSERT(UC_G(lock_held));

	if (hdr->lock_model == UCACHE_LOCK_MODEL_MUTEX && hdr->scalar_write_ready &&
		ucache_atomic_load_32(&hdr->scalar_write_enabled) == 0
	) {
		ucache_atomic_store_32(&hdr->scalar_write_gate, 1);
		ucache_atomic_store_32(&hdr->scalar_write_enabled, 1);
	}
}

bool ucache_scalar_write_begin(zend_ulong hash, ucache_hdr **hdr_ptr, uint32_t *stripe_idx)
{
	ucache_hdr *hdr = ucache_hdr_ptr();
	ucache_scalar_write_stripe *stripe;
	uint32_t idx;
	int result;

	if (UC_G(lock_held) || UC_G(scalar_write_hdr) != NULL ||
		!ucache_hdr_is_initialized_acquire() ||
		hdr->lock_model != UCACHE_LOCK_MODEL_MUTEX ||
		ucache_atomic_load_32(&hdr->scalar_write_enabled) == 0
	) {
		return false;
	}

	idx = ucache_scalar_write_idx(hash);
	stripe = &hdr->scalar_write_stripes[idx];
	result = ucache_robust_mutex_lock(&stripe->mutex.mutex);
	if (result == EOWNERDEAD) {
		result = pthread_mutex_consistent(&stripe->mutex.mutex);
		if (result != 0) {
			pthread_mutex_unlock(&stripe->mutex.mutex);
		}
	}

	if (result != 0) {
		return false;
	}

	ucache_atomic_store_32(&stripe->active, 1);
	ucache_atomic_fence_seq_cst();

	if (ucache_atomic_load_32(&hdr->scalar_write_gate) != 0 ||
		(ucache_atomic_load_64(&hdr->write_seq) & 1) != 0 ||
		ucache_atomic_load_32(&hdr->entry_lock_table_section_open) != 0
	) {
		ucache_scalar_write_back_off(stripe);

		return false;
	}

	ucache_scalar_write_undo_if_pending(hdr, stripe);

	if (hdr->entry_lock_count != 0 && ucache_entry_lock_hash_present(hdr, hash)) {
		ucache_scalar_write_back_off(stripe);

		return false;
	}

	UC_G(scalar_write_hdr) = hdr;
	UC_G(scalar_write_idx) = idx;

	*hdr_ptr = hdr;
	*stripe_idx = idx;

	return true;
}

void ucache_scalar_write_prepare(ucache_hdr *hdr, uint32_t stripe_idx, uint32_t slot_idx)
{
	ucache_scalar_write_stripe *stripe = &hdr->scalar_write_stripes[stripe_idx];
	ucache_entry *entry = &ucache_entries_ptr(hdr)[slot_idx];

	stripe->slot_idx = slot_idx;

	memcpy(&stripe->old_val, &entry->long_val, sizeof(stripe->old_val));

	stripe->old_flags = entry->flags;
	stripe->old_gen = entry->gen;

	ucache_seq_announce(&stripe->seq, stripe->seq + 1);
}

void ucache_scalar_write_commit(ucache_hdr *hdr, uint32_t stripe_idx, ucache_entry *entry)
{
	ucache_scalar_write_stripe *stripe = &hdr->scalar_write_stripes[stripe_idx];

	entry->gen = ucache_scalar_next_gen(hdr);

	ucache_seq_publish(&stripe->seq, stripe->seq + 1);
}

void ucache_scalar_write_end(void)
{
	ucache_hdr *hdr = UC_G(scalar_write_hdr);
	ucache_scalar_write_stripe *stripe;

	if (hdr == NULL) {
		return;
	}

	stripe = &hdr->scalar_write_stripes[UC_G(scalar_write_idx)];

	ucache_scalar_write_undo_if_pending(hdr, stripe);

	UC_G(scalar_write_hdr) = NULL;

	ucache_atomic_store_32(&stripe->active, 0);

	pthread_mutex_unlock(&stripe->mutex.mutex);
}
#else
void ucache_scalar_write_enable_locked(ucache_hdr *hdr)
{
	(void) hdr;
}

bool ucache_scalar_write_begin(zend_ulong hash, ucache_hdr **hdr_ptr, uint32_t *stripe_idx)
{
	(void) hash;
	(void) hdr_ptr;
	(void) stripe_idx;

	return false;
}

void ucache_scalar_write_prepare(ucache_hdr *hdr, uint32_t stripe_idx, uint32_t slot_idx)
{
	(void) hdr;
	(void) stripe_idx;
	(void) slot_idx;
}

void ucache_scalar_write_commit(ucache_hdr *hdr, uint32_t stripe_idx, ucache_entry *entry)
{
	(void) hdr;
	(void) stripe_idx;
	(void) entry;
}

void ucache_scalar_write_end(void)
{
}
#endif /* UCACHE_HAVE_SCALAR_WRITE */

void ucache_unlock(void)
{
	if (UC_G(lock_held_is_write)) {
		ucache_write_section_leave();
	}

	UC_G(lock_held_is_write) = false;

	ucache_unlock_impl();
}

void ucache_unlock_if_held(void)
{
	if (UC_G(scalar_write_hdr) != NULL) {
		ucache_scalar_write_end();
	}

	if (UC_G(lock_held)) {
		ucache_unlock();
	}
}

uint32_t ucache_sleep_us(uint32_t interval_us)
{
	return ucache_platform.sleep_us(interval_us);
}
