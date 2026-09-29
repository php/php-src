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

#ifndef UCACHE_STORAGE_H
#define UCACHE_STORAGE_H

#include "user_cache_internal.h"
#include "user_cache_storage_portability.h"

#define UCACHE_START_TIME_EXITED UINT64_MAX
#define UCACHE_OWNER_TOKEN_PID_MIX UINT64_C(0x9E3779B97F4A7C15)

#define UCACHE_ENTRY_LOCK_MAX_CAPACITY			1024U
#define UCACHE_ENTRY_LOCK_WAIT_TIMEOUT_US		(10U * 1000U * 1000U)
#define UCACHE_ENTRY_LOCK_WAIT_DEBUG_TIMEOUT_US	(300U * 1000U)

typedef struct {
	uint64_t owner_pid;
	uint64_t owner_start_time;
	uint64_t owner_token;
	zend_ulong hash;
	uint32_t slot_idx;
	uint32_t expires_at;
} ucache_entry_lock_holder;

typedef struct {
	uint32_t (*sleep_us)(uint32_t interval_us);
	uint64_t (*proc_start_time_token)(uint64_t pid);
	bool (*proc_has_exited)(uint64_t pid);
	int (*alloc_err_code)(void);
	void (*log_alloc_failure)(const char *failure, const char *err_in, int err_code);
} ucache_platform_ops;

extern const ucache_platform_ops ucache_platform;

uint32_t ucache_calc_entry_lock_capacity(uint32_t capacity);
uint32_t ucache_calc_capacity(size_t size, bool *clamped);
#ifdef UCACHE_HAVE_BOUNDARY_SHM
const uint8_t *ucache_shared_boundary_digest_memo(ucache_ctx *ctx, size_t requested_size);
bool ucache_hdr_boundary_lock_file_replaced_locked(const ucache_hdr *hdr);
void ucache_shared_boundary_digest(
		const ucache_ctx *ctx,
		size_t requested_size,
		uint8_t digest[32]);
void ucache_shared_boundary_retire_seg_name(void);
void ucache_shared_boundary_flush_dir_failure_log(void);
const ucache_shm_handler_entry *ucache_shared_boundary_handler_entry(void);
#endif /* UCACHE_HAVE_BOUNDARY_SHM */
#ifdef ZEND_WIN32
bool ucache_win32_ctx_is_proc_private(const ucache_ctx *ctx);
bool ucache_win32_owned_by_cur_user(HANDLE handle, SE_OBJECT_TYPE type);
bool ucache_win32_load_salt(const char **err_in);
void ucache_win32_format_name(char *buf, size_t buf_size, const char *name, size_t unique_id);
bool ucache_win32_commit_to(ucache_win32_seg *seg, size_t end_bytes);
void ucache_win32_reset_range(ucache_win32_seg *seg, size_t start_bytes, size_t end_bytes);
#endif /* ZEND_WIN32 */
const ucache_shm_handler_entry *ucache_handler_table(void);
void ucache_cleanup_seg(const ucache_shm_handlers *handler, ucache_shm_seg *seg);
#ifdef UCACHE_HAVE_SHARED_MUTEX
bool ucache_shared_mutex_init(ucache_hdr *hdr);
#endif /* UCACHE_HAVE_SHARED_MUTEX */
const ucache_lock_ops *ucache_lock_ops_for_model(uint32_t lock_model);
bool ucache_create_lock(void);
void ucache_destroy_lock(void);
bool ucache_wlock_entry_lock_table(void);
void ucache_write_section_announce(ucache_hdr *hdr);
bool ucache_wlock_negotiated_lock_model(void);
bool ucache_entry_lock_record_is_active_locked(
		ucache_entry_lock_record *record,
		uint64_t now_rel);
void ucache_sweep_entry_locks_locked(ucache_hdr *hdr, uint64_t now);
bool ucache_find_entry_lock_record_slot_locked(
		ucache_hdr *hdr,
		zend_string *key,
		zend_ulong hash,
		uint32_t *slot_idx,
		bool *found);
void ucache_entry_lock_holder_capture(
		ucache_entry_lock_holder *holder,
		ucache_hdr *hdr,
		uint32_t slot_idx);
bool ucache_entry_lock_holder_unchanged(
		const ucache_storage *storage,
		const ucache_entry_lock_holder *holder);
void ucache_drain_deferred_entry_lock_releases(void);
void ucache_ensure_entry_lock_owner(void);
#ifdef ZEND_WIN32
uint32_t ucache_win32_sleep_us(uint32_t interval_us);
uint64_t ucache_win32_proc_start_time_token(uint64_t pid);
bool ucache_win32_proc_has_exited(uint64_t pid);
int ucache_win32_alloc_err_code(void);
void ucache_win32_log_alloc_failure(const char *failure, const char *err_in, int err_code);
#else
uint32_t ucache_posix_sleep_us(uint32_t interval_us);
uint64_t ucache_posix_proc_start_time_token(uint64_t pid);
bool ucache_posix_proc_has_exited(uint64_t pid);
int ucache_posix_alloc_err_code(void);
void ucache_posix_log_alloc_failure(const char *failure, const char *err_in, int err_code);
#endif /* ZEND_WIN32 */
bool ucache_recover_after_owner_death_locked(void);
#if ZEND_DEBUG
void ucache_debug_reserve_data_below_4g_locked(ucache_hdr *hdr);
#endif

static zend_always_inline size_t ucache_free_bins_bytes(uint32_t bin_count)
{
	return ((size_t) bin_count + ucache_free_bin_words(bin_count)) * sizeof(uint32_t);
}

static zend_always_inline uint32_t ucache_atomic_load_32(const uint32_t *target)
{
	return UCACHE_ATOMIC_LOAD_32(target);
}

static zend_always_inline void ucache_atomic_store_32(uint32_t *target, uint32_t val)
{
	UCACHE_ATOMIC_STORE_32(target, val);
}

static zend_always_inline void ucache_atomic_fence_seq_cst(void)
{
	UCACHE_ATOMIC_FENCE_SEQ_CST();
}

static zend_always_inline uint32_t ucache_size_class(uint32_t size)
{
	uint32_t shift;

	if (size < UCACHE_SIZE_CLASS_EXACT_LIMIT) {
		return size < 16U ? 0U : size / 8U - 2U;
	}

	shift = ucache_floor_log2(size);

	return UCACHE_SIZE_CLASS_EXACT_BINS
		+ ((shift - UCACHE_SIZE_CLASS_MIN_SHIFT) << UCACHE_SIZE_CLASS_STEP_SHIFT)
		+ ((size - (1U << shift)) >> (shift - UCACHE_SIZE_CLASS_STEP_SHIFT))
	;
}

static zend_always_inline uint32_t ucache_entry_lock_table_idx(
		const ucache_hdr *hdr,
		zend_ulong hash)
{
	return (uint32_t) (hash & (hdr->entry_lock_capacity - 1));
}

static zend_always_inline ucache_entry_lock *ucache_find_local_entry_lock(
		const ucache_ctx *ctx,
		zend_string *key)
{
	ucache_entry_lock *lock;

	if (UC_G(entry_lock_table) == NULL) {
		return NULL;
	}

	lock = zend_hash_find_ptr(UC_G(entry_lock_table), key);
	if (lock == NULL || lock->ctx != ctx) {
		return NULL;
	}

	return lock;
}

static zend_always_inline uint64_t ucache_proc_owner_token(uint64_t pid)
{
	uint64_t start_time = ucache_platform.proc_start_time_token(pid), token;

	if (start_time == 0 || start_time == UCACHE_START_TIME_EXITED) {
		return start_time;
	}

	token = start_time ^ (pid * UCACHE_OWNER_TOKEN_PID_MIX);

	return token == 0 || token == UCACHE_START_TIME_EXITED ? 1 : token;
}

static zend_always_inline uint64_t ucache_cached_self_start_time_token(uint64_t self_pid)
{
	if (UC_G(self_start_time_pid) != self_pid ||
		UC_G(self_start_time_token) == 0
	) {
		UC_G(self_start_time_pid) = self_pid;
		UC_G(self_start_time_token) = ucache_proc_owner_token(self_pid);
	}

	return UC_G(self_start_time_token);
}

static zend_always_inline void ucache_seq_announce(uint64_t *seq, uint64_t val)
{
	ucache_atomic_store_64(seq, val);

	ucache_atomic_fence_seq_cst();
}

static zend_always_inline void ucache_seq_publish(uint64_t *seq, uint64_t val)
{
	ucache_atomic_store_64(seq, val);
}

static zend_always_inline uint64_t ucache_view_committed_bytes(const ucache_storage *storage)
{
#ifdef ZEND_WIN32
	return ((const ucache_win32_seg *) storage->seg)->committed_bytes;
#else
	return storage->size;
#endif
}

#ifdef UCACHE_HAVE_BOUNDARY_SHM
static zend_always_inline ucache_boundary_seg *ucache_boundary_seg_of(const ucache_storage *storage)
{
	ZEND_ASSERT(storage->seg != NULL);

	return (ucache_boundary_seg *) storage->seg;
}
#endif

static zend_always_inline uint32_t ucache_entry_lock_wait_timeout_us(void)
{
	return UCACHE_DEBUG_FAULT("SHORT_ENTRY_LOCK_WAIT")
		? UCACHE_ENTRY_LOCK_WAIT_DEBUG_TIMEOUT_US
		: UCACHE_ENTRY_LOCK_WAIT_TIMEOUT_US
	;
}

static inline uint32_t ucache_sleep_entry_lock_retry_interval(uint64_t waited_us)
{
	uint32_t interval_us = UCACHE_ENTRY_LOCK_RETRY_INTERVAL_US;

	if (waited_us < UCACHE_ENTRY_LOCK_RETRY_INTERVAL_US) {
		interval_us = MIN(
			(uint32_t) waited_us + UCACHE_ENTRY_LOCK_RETRY_INITIAL_US,
			UCACHE_ENTRY_LOCK_RETRY_INTERVAL_US - (uint32_t) waited_us
		);
	}

	return ucache_platform.sleep_us(interval_us);
}

#endif /* UCACHE_STORAGE_H */
