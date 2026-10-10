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

#define UCACHE_READER_DRAIN_SPIN		1024U
#define UCACHE_READER_DRAIN_TIMEOUT_US	10000U

#define UCACHE_SAPI_CLI					1U
#define UCACHE_SAPI_PRE_REQ_STORAGE		2U

#define UCACHE_MEM_MODEL_SHM_ALIAS	"cgi"

typedef struct {
	const char *alloc_err_in;
	int alloc_err_code;
	const char *preferred_failed_model;
	const char *preferred_err_in;
	int preferred_err_code;
	const char *fallback_model;
	uint32_t clamped_capacity;
	bool alloc_failed;
} ucache_startup_warnings;

static bool ucache_capacity_clamp_warned = false;

static const char *ucache_classified_sapi_name = NULL;
static uint32_t ucache_classified_sapi_flags = 0;

#ifdef ZTS
static ucache_startup_lock ucache_startup_storage_lock_state =
	UCACHE_STARTUP_LOCK_INITIALIZER
;
#endif

static zend_always_inline uint32_t ucache_free_bin_count_for_seg(size_t seg_size)
{
	return ucache_size_class((uint32_t) MIN(seg_size, (size_t) UCACHE_BLOCK_SIZE_MAX)) + 1U;
}

static zend_always_inline size_t ucache_shm_size_floor(size_t seg_size)
{
	return UCACHE_ALIGNED_SIZE(
		UCACHE_ALIGNED_SIZE(
			UCACHE_ALIGNED_SIZE(
				sizeof(ucache_hdr) + UCACHE_MIN_CAPACITY * UCACHE_TABLE_SLOT_SIZE
			) +
			UCACHE_ENTRY_LOCK_MIN_CAPACITY * sizeof(ucache_entry_lock_record)
		) +
		ucache_free_bins_bytes(ucache_free_bin_count_for_seg(seg_size))
	);
}

static zend_always_inline uint32_t ucache_sapi_flags(void)
{
	if (UNEXPECTED(sapi_module.name != ucache_classified_sapi_name)) {
		ucache_classify_sapi();
	}

	return ucache_classified_sapi_flags;
}

static zend_always_inline bool ucache_requires_pre_req_storage(void)
{
	return (ucache_sapi_flags() & UCACHE_SAPI_PRE_REQ_STORAGE) != 0;
}

static zend_always_inline bool ucache_is_disabled_for_sapi(void)
{
	if (!UC_G(enable)) {
		return true;
	}

	return !UC_G(enable_cli) && (ucache_sapi_flags() & UCACHE_SAPI_CLI) != 0;
}

static zend_always_inline void ucache_set_unavailable(php_ucache_reason reason)
{
	ucache_runtime *runtime = ucache_active_runtime();

	runtime->available = false;
	runtime->failure_reason = reason;
}

static zend_always_inline void ucache_set_available(void)
{
	ucache_runtime *runtime = ucache_active_runtime();

	runtime->available = true;
	runtime->failure_reason = PHP_UCACHE_REASON_NONE;
}

#ifdef ZTS
# ifndef ZEND_WIN32
static inline void ucache_zts_lock_reinit_after_fork(ucache_storage *storage)
{
	if (storage->zts_lock != NULL) {
		pthread_mutex_init(storage->zts_lock, NULL);
	}
}
# endif

static inline void ucache_startup_storage_lock(void)
{
# ifdef ZEND_WIN32
	AcquireSRWLockExclusive(&ucache_startup_storage_lock_state);
# else
	pthread_mutex_lock(&ucache_startup_storage_lock_state);
# endif
}

static inline void ucache_startup_storage_unlock(void)
{
# ifdef ZEND_WIN32
	ReleaseSRWLockExclusive(&ucache_startup_storage_lock_state);
# else
	pthread_mutex_unlock(&ucache_startup_storage_lock_state);
# endif
}
#else
static inline void ucache_startup_storage_lock(void)
{
}

static inline void ucache_startup_storage_unlock(void)
{
}
#endif /* ZTS */

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
	while (candidate > UCACHE_MIN_CAPACITY && !ucache_capacity_is_prime(candidate)) {
		candidate--;
	}

	return candidate;
}

static bool ucache_hdr_boundary_identity_matches_locked(
		const ucache_hdr *hdr,
		size_t requested_size)
{
#ifdef UCACHE_HAVE_BOUNDARY_SHM
	ucache_ctx *ctx = ucache_active_ctx();

	if (!ucache_ctx_is_boundary(ctx)) {
		return hdr->boundary_identity_digest_set == 0;
	}

	if (hdr->boundary_identity_digest_set == 0) {
		return false;
	}

	return memcmp(
		hdr->boundary_identity_digest,
		ucache_shared_boundary_digest_memo(ctx, requested_size),
		sizeof(ctx->storage.boundary_digest_memo)
	) == 0;
#else
	(void) hdr;
	(void) requested_size;

	return true;
#endif
}

static void ucache_hdr_layout_memo(
		ucache_storage *storage,
		uint32_t *capacity,
		uint32_t *data_offset)
{
	if (!storage->layout_memo_valid) {
		storage->capacity_memo = ucache_calc_capacity(storage->size, &storage->capacity_clamped);
		storage->entry_lock_capacity_memo =
			ucache_calc_entry_lock_capacity(storage->capacity_memo)
		;
		storage->entry_lock_offset_memo = (uint32_t) UCACHE_ALIGNED_SIZE(
			sizeof(ucache_hdr)
			+ storage->capacity_memo * UCACHE_TABLE_SLOT_SIZE
		);
		storage->free_bins_offset_memo = (uint32_t) UCACHE_ALIGNED_SIZE(
			storage->entry_lock_offset_memo
			+ storage->entry_lock_capacity_memo * sizeof(ucache_entry_lock_record)
		);
		storage->free_bin_count_memo = ucache_free_bin_count_for_seg(storage->size);
		storage->data_offset_memo = (uint32_t) UCACHE_ALIGNED_SIZE(
			storage->free_bins_offset_memo + ucache_free_bins_bytes(storage->free_bin_count_memo)
		);
		storage->layout_memo_valid = true;
	}

	*capacity = storage->capacity_memo;
	*data_offset = storage->data_offset_memo;
}

static bool ucache_hdr_layout_matches_memo_locked(
		const ucache_storage *storage,
		const ucache_hdr *hdr,
		uint32_t capacity,
		uint32_t data_offset)
{
	return hdr->capacity == capacity &&
		hdr->data_offset == data_offset &&
		hdr->entry_lock_capacity == storage->entry_lock_capacity_memo &&
		hdr->entry_lock_offset == storage->entry_lock_offset_memo &&
		hdr->free_bins_offset == storage->free_bins_offset_memo &&
		hdr->free_bin_count == storage->free_bin_count_memo &&
		hdr->graph_pin_slot_count == ucache_ctx_graph_pin_slot_count(ucache_active_ctx()) &&
		ucache_hdr_data_bounds_match(storage, hdr)
	;
}

static bool ucache_commit_seg_prefix(ucache_startup_warnings *warnings)
{
#ifdef ZEND_WIN32
	ucache_storage *storage = &ucache_active_ctx()->storage;
	uint32_t capacity, data_offset;

	ucache_hdr_layout_memo(storage, &capacity, &data_offset);

	if (ucache_win32_commit_to((ucache_win32_seg *) storage->seg, data_offset)) {
		return true;
	}

	warnings->alloc_err_in = "VirtualAlloc";
	warnings->alloc_err_code = ucache_platform.alloc_err_code();
	warnings->alloc_failed = true;

	return false;
#else
	(void) warnings;

	return true;
#endif
}

static zend_never_inline bool ucache_hdr_format_fresh_locked(
		ucache_hdr *hdr,
		uint32_t capacity,
		uint32_t data_offset)
{
	ucache_storage *storage = &ucache_active_ctx()->storage;
	uint64_t epoch;

	if ((size_t) data_offset + UCACHE_BLOCK_MIN_SIZE > storage->size) {
		return false;
	}

	epoch = MAX((uint64_t) time(NULL) << 32, hdr->mutation_epoch + 1);

	ucache_atomic_store_64(&hdr->mutation_epoch, epoch != 0 ? epoch : 1);

	if (!storage->seg->zero_filled) {
		memset(hdr, 0, offsetof(ucache_hdr, mutation_epoch));
		memset(
			&hdr->mutation_epoch + 1,
			0,
			data_offset - offsetof(ucache_hdr, mutation_epoch) - sizeof(hdr->mutation_epoch)
		);
	}

	storage->seg->zero_filled = false;

	hdr->capacity = capacity;
	hdr->data_offset = data_offset;
	hdr->data_size = storage->size - data_offset;
	hdr->entry_lock_capacity = storage->entry_lock_capacity_memo;
	hdr->entry_lock_offset = storage->entry_lock_offset_memo;
	hdr->free_bins_offset = storage->free_bins_offset_memo;
	hdr->free_bin_count = storage->free_bin_count_memo;
	hdr->graph_pin_slot_count = ucache_ctx_graph_pin_slot_count(ucache_active_ctx());
	hdr->committed_end = ucache_view_committed_bytes(storage);
	hdr->stale_tail_end = hdr->committed_end;
	hdr->next_free = 0;

	hdr->count = 0;
	hdr->expiring_count = 0;
	hdr->expiry_floor = UCACHE_EXPIRY_FLOOR_NONE;
	hdr->time_base = ucache_clock_now();

	if (hdr->time_base == 0) {
		hdr->time_base = 1;
	}

	hdr->write_seq = UC_G(write_seq_bumped) ? 1u : 2u;
#ifdef UCACHE_HAVE_SHARED_MUTEX
	hdr->lock_model = ucache_shared_mutex_init(hdr)
		? UCACHE_LOCK_MODEL_MUTEX
		: UCACHE_LOCK_MODEL_FCNTL
	;
#else
	hdr->lock_model = UCACHE_LOCK_MODEL_FCNTL;
#endif
#ifdef UCACHE_HAVE_BOUNDARY_SHM
	if (ucache_ctx_is_boundary(ucache_active_ctx())) {
		memcpy(
			hdr->boundary_identity_digest,
			ucache_shared_boundary_digest_memo(ucache_active_ctx(), storage->size),
			sizeof(hdr->boundary_identity_digest)
		);

		hdr->boundary_identity_digest_set = 1;
		hdr->lock_file_dev = ucache_boundary_seg_of(storage)->lock_dev;
		hdr->lock_file_ino = ucache_boundary_seg_of(storage)->lock_ino;
	}
#endif
#if ZEND_DEBUG
	ucache_debug_reserve_data_below_4g_locked(hdr);
#endif

	ucache_atomic_fence_seq_cst();

	hdr->magic = UCACHE_MAGIC;

	ucache_write_section_announce(hdr);

	return true;
}

static bool ucache_try_storage_handler(
		const ucache_shm_handler_entry *handler_entry,
		const char **err_in_ptr,
		int *err_code_ptr)
{
	const char *err_in = NULL;
	ucache_ctx *ctx = ucache_active_ctx();
	ucache_runtime *runtime = ucache_active_runtime();
	ucache_storage *storage = &ctx->storage;
	ucache_shm_seg *seg = NULL;

	if (handler_entry->handler->create_seg(
			runtime->configured_mem,
			&seg,
			&err_in
		) != UCACHE_ALLOC_SUCCESS
	) {
		*err_code_ptr = ucache_platform.alloc_err_code();
		*err_in_ptr = err_in;

		return false;
	}

	storage->handler = handler_entry->handler;
	storage->handler_name = handler_entry->name;
	storage->seg = seg;
	storage->base = seg->p;
	storage->size = runtime->configured_mem;
	storage->initialized = true;

	return true;
}

static const ucache_shm_handler_entry *ucache_find_storage_handler(const char *model)
{
	const ucache_shm_handler_entry *handler_entry;

	if (strcmp(model, UCACHE_MEM_MODEL_SHM_ALIAS) == 0) {
		model = "shm";
	}

	for (handler_entry = ucache_handler_table(); handler_entry->name; handler_entry++) {
		if (strcmp(model, handler_entry->name) == 0) {
			return handler_entry;
		}
	}

	return NULL;
}

static bool ucache_try_preferred_storage_handler(
		const ucache_shm_handler_entry *handler_entry,
		ucache_startup_warnings *warnings)
{
	if (UCACHE_DEBUG_FAULT("FAIL_PREFERRED_MEMORY_MODEL")) {
		warnings->alloc_err_in = "debug fault";
		warnings->alloc_err_code = 0;
	} else if (ucache_try_storage_handler(handler_entry, &warnings->alloc_err_in, &warnings->alloc_err_code)) {
		return true;
	}

	warnings->preferred_failed_model = handler_entry->name;
	warnings->preferred_err_in = warnings->alloc_err_in;
	warnings->preferred_err_code = warnings->alloc_err_code;

	return false;
}

static void ucache_log_preferred_model_failure(const ucache_startup_warnings *warnings, bool started)
{
	char failure[128];

	if (started && warnings->fallback_model != NULL) {
		snprintf(
			failure,
			sizeof(failure),
			"preferred memory model \"%s\" failed, using \"%s\"",
			warnings->preferred_failed_model,
			warnings->fallback_model
		);
	} else {
		snprintf(failure, sizeof(failure), "preferred memory model \"%s\" failed", warnings->preferred_failed_model);
	}

	ucache_platform.log_alloc_failure(failure, warnings->preferred_err_in, warnings->preferred_err_code);
}

static bool ucache_select_storage_handler(ucache_startup_warnings *warnings)
{
	const ucache_shm_handler_entry *handler_entry, *preferred = NULL;
	const char *model = UC_G(mem_model);

#ifdef UCACHE_HAVE_BOUNDARY_SHM
	if (ucache_ctx_is_boundary(ucache_active_ctx())) {
		if (ucache_try_storage_handler(
				ucache_shared_boundary_handler_entry(),
				&warnings->alloc_err_in,
				&warnings->alloc_err_code
			)
		) {
			return true;
		}

		warnings->alloc_failed = true;

		return false;
	}
#endif

	if (model != NULL && model[0] != '\0') {
		preferred = ucache_find_storage_handler(model);
		if (preferred != NULL && ucache_try_preferred_storage_handler(preferred, warnings)) {
			return true;
		}
	}

	for (handler_entry = ucache_handler_table(); handler_entry->name; handler_entry++) {
		if (handler_entry == preferred) {
			continue;
		}

		if (ucache_try_storage_handler(handler_entry, &warnings->alloc_err_in, &warnings->alloc_err_code)) {
			warnings->fallback_model = handler_entry->name;

			return true;
		}
	}

	warnings->alloc_failed = true;

	return false;
}

static bool ucache_startup_storage_impl(ucache_startup_warnings *warnings)
{
	ucache_storage *storage = &ucache_active_ctx()->storage;

	if (storage->initialized) {
		return true;
	}

	if (!ucache_select_storage_handler(warnings)) {
		ucache_reset_storage();

		return false;
	}

	if (!ucache_commit_seg_prefix(warnings) || !ucache_create_lock()) {
		goto bailout;
	}

	if (!ucache_wlock_negotiated_lock_model()) {
		ucache_destroy_lock();

		goto bailout;
	}

	if (!ucache_hdr_init_locked()) {
		ucache_unlock();

		ucache_destroy_lock();

		goto bailout;
	}

	ucache_unlock();

	if (storage->capacity_clamped && !ucache_capacity_clamp_warned) {
		ucache_capacity_clamp_warned = true;

		warnings->clamped_capacity = storage->capacity_memo;
	}

	if (ucache_hdr_ptr() != NULL) {
		storage->lock_ops = ucache_lock_ops_for_model(ucache_hdr_ptr()->lock_model);
	}

#ifdef ZTS
	ucache_atomic_store_32(&storage->startup_complete, 1);
#endif

	return true;

bailout:
	ucache_cleanup_seg(storage->handler, storage->seg);

	ucache_reset_storage();

	return false;
}

static bool ucache_startup_storage(void)
{
	ucache_startup_warnings warnings = {0};
	bool result;

	if (ucache_storage_startup_is_complete(&ucache_active_ctx()->storage)) {
		return true;
	}

	ucache_startup_storage_lock();

	result = ucache_startup_storage_impl(&warnings);

	ucache_startup_storage_unlock();

#ifdef UCACHE_HAVE_BOUNDARY_SHM
	ucache_shared_boundary_flush_dir_failure_log();
#endif

	if (warnings.preferred_failed_model != NULL) {
		ucache_log_preferred_model_failure(&warnings, result);
	}

	if (warnings.alloc_failed) {
		ucache_platform.log_alloc_failure(
			"shared memory initialization failed",
			warnings.alloc_err_in,
			warnings.alloc_err_code
		);
	}

	if (warnings.clamped_capacity != 0) {
		ucache_warn(
			"user_cache.entries_hint (" ZEND_LONG_FMT ") exceeds what user_cache.shm_size can index; clamping capacity to %u",
			UC_G(entries_hint),
			warnings.clamped_capacity
		);
	}

	return result;
}

static void ucache_resolve_runtime(void)
{
	ucache_ctx *ctx = ucache_active_ctx();
	ucache_runtime *runtime;
	ucache_storage *storage = &ctx->storage;

	ucache_reset_runtime();

	runtime = ucache_active_runtime();
	if (!runtime->enabled) {
		return;
	}

	if (UC_G(in_req_shutdown)) {
		ucache_set_unavailable(PHP_UCACHE_REASON_REQ_SHUTDOWN);

		return;
	}

	if (UC_G(req_unavailable_reason) != PHP_UCACHE_REASON_NONE) {
		ucache_set_unavailable(UC_G(req_unavailable_reason));

		return;
	}

	if (!ucache_runtime_opted_in ||
		!ucache_exec_available()
	) {
		ucache_set_unavailable(PHP_UCACHE_REASON_SAPI_NOT_ENABLED);

		return;
	}

	if (ucache_is_disabled_for_sapi()) {
		ucache_set_unavailable(PHP_UCACHE_REASON_DISABLED_BY_INI);

		return;
	}

	if (!storage->initialized &&
		ucache_requires_pre_req_storage()
	) {
		ucache_set_unavailable(
			ucache_ctx_is_boundary(ctx)
				? PHP_UCACHE_REASON_SHM_INIT_FAILED
				: PHP_UCACHE_REASON_BACKEND_NOT_INITIALIZED_BEFORE_WORKER
		);

		return;
	}

	if (!ucache_startup_storage()) {
		ucache_set_unavailable(PHP_UCACHE_REASON_SHM_INIT_FAILED);

		return;
	}

	if (ucache_requires_pre_req_storage() &&
		!storage->initialized_before_req
	) {
		ucache_set_unavailable(PHP_UCACHE_REASON_BACKEND_INITIALIZED_AFTER_WORKER);

		return;
	}

	ucache_set_available();
}

static bool ucache_claim_hdr_is_attached(const ucache_hdr *hdr)
{
	const php_ucache_partition *partition;
	bool attached = false;

	if (ucache_ctx_state.storage.base == hdr) {
		return true;
	}

	ucache_boundary_partitions_lock();

	for (partition = ucache_partitions; partition != NULL; partition = partition->next) {
		if (partition->ctx.storage.base == hdr) {
			attached = true;

			break;
		}
	}

	ucache_boundary_partitions_unlock();

	return attached;
}

static void ucache_release_oldest_reader_claim(uint64_t my_pid)
{
	ucache_reader_claim *claims = UC_G(reader_claims);
	ucache_reader_slot *slot = &claims[0].hdr->reader_slots[claims[0].slot_idx];

	ucache_atomic_store_64(&slot->owner_start_time, 0);
	ucache_atomic_cas_64(&slot->owner_pid, my_pid, 0);

	UC_G(reader_claim_count)--;

	memmove(claims, claims + 1, UC_G(reader_claim_count) * sizeof(*claims));
}

static int32_t ucache_claim_reader_slot(ucache_hdr *hdr)
{
	ucache_reader_slot *slot;
	uint64_t my_pid, expected, owner_start_time;
	uint32_t i, used;

	my_pid = ucache_cached_pid();

	if (UNEXPECTED(UC_G(reader_claim_pid) != my_pid)) {
		UC_G(reader_claim_count) = 0;
		UC_G(reader_claim_pid) = my_pid;
	}

	for (i = 0; i < UC_G(reader_claim_count); i++) {
		if (UC_G(reader_claims)[i].hdr == hdr) {
			return (int32_t) UC_G(reader_claims)[i].slot_idx;
		}
	}

	if (UNEXPECTED(UC_G(reader_claim_failed_hdr) == hdr) && UC_G(reader_claim_failed_pid) == my_pid) {
		return -1;
	}

	if (UC_G(reader_claim_count) == UCACHE_READER_CLAIM_MAX) {
		ucache_release_oldest_reader_claim(my_pid);
	}

	for (i = 0; i < UCACHE_READER_SLOTS; i++) {
		slot = &hdr->reader_slots[i];

		if (ucache_atomic_load_64(&slot->owner_pid) == 0 &&
			ucache_atomic_cas_64(&slot->owner_pid, 0, my_pid)
		) {
			goto done;
		}
	}

	for (i = 0; i < UCACHE_READER_SLOTS; i++) {
		slot = &hdr->reader_slots[i];

		owner_start_time = ucache_atomic_load_64(&slot->owner_start_time);
		expected = ucache_atomic_load_64(&slot->owner_pid);

		if (expected == 0 ||
			ucache_atomic_load_32(&slot->active) != 0 ||
			!ucache_owner_is_dead(expected, owner_start_time) ||
			!ucache_atomic_cas_64(&slot->owner_start_time, owner_start_time, 0) ||
			!ucache_atomic_cas_64(&slot->owner_pid, expected, my_pid)
		) {
			continue;
		}

		goto done;
	}

	UC_G(reader_claim_failed_hdr) = hdr;
	UC_G(reader_claim_failed_pid) = my_pid;

	return -1;

done:
	ucache_atomic_store_64(&slot->owner_start_time, ucache_cached_self_start_time_token(my_pid));

	for (;;) {
		used = ucache_atomic_load_32(&hdr->reader_slots_used);
		if (used > i || ucache_atomic_cas_32(&hdr->reader_slots_used, used, i + 1)) {
			break;
		}
	}

	UC_G(reader_claims)[UC_G(reader_claim_count)].hdr = hdr;
	UC_G(reader_claims)[UC_G(reader_claim_count)].slot_idx = i;
	UC_G(reader_claim_count)++;

	return (int32_t) i;
}

uint32_t ucache_calc_entry_lock_capacity(uint32_t capacity)
{
	uint32_t want = capacity / 16,
			lock_capacity = UCACHE_ENTRY_LOCK_MAX_CAPACITY
	;

	while (lock_capacity > UCACHE_ENTRY_LOCK_MIN_CAPACITY && lock_capacity > want) {
		lock_capacity >>= 1;
	}

	return lock_capacity;
}

uint32_t ucache_calc_capacity(size_t size, bool *clamped)
{
	uint64_t hint, want, max_capacity;
	uint32_t capacity, lock_capacity, next_lock_capacity;
	size_t reserve, lock_bytes, fixed_bytes;

	*clamped = false;

	hint = UC_G(entries_hint) > 0
		? (uint64_t) UC_G(entries_hint)
		: (uint64_t) (size / UCACHE_AUTO_SEG_BYTES_PER_ENTRY)
	;

	if (hint < UCACHE_MIN_CAPACITY) {
		hint = UCACHE_MIN_CAPACITY;
	}

	want = hint + (hint + 2) / 3;
	capacity = ucache_next_prime((uint32_t) want);

	reserve = size / 2;
	lock_capacity = ucache_calc_entry_lock_capacity(capacity);

	for (;;) {
		lock_bytes = (size_t) lock_capacity * sizeof(ucache_entry_lock_record);
		fixed_bytes = sizeof(ucache_hdr) + lock_bytes +
			ucache_free_bins_bytes(ucache_free_bin_count_for_seg(size))
		;

		if (reserve <= fixed_bytes) {
			*clamped = UC_G(entries_hint) > 0;

			return UCACHE_MIN_CAPACITY;
		}

		max_capacity = (reserve - fixed_bytes) / UCACHE_TABLE_SLOT_SIZE;
		if (capacity > max_capacity) {
			capacity = ucache_prev_prime(
				max_capacity > UCACHE_MIN_CAPACITY
					? (uint32_t) max_capacity
					: UCACHE_MIN_CAPACITY
			);

			*clamped = UC_G(entries_hint) > 0;
		}

		next_lock_capacity = ucache_calc_entry_lock_capacity(capacity);
		if (next_lock_capacity == lock_capacity) {
			break;
		}

		lock_capacity = next_lock_capacity;
	}

	return capacity;
}

#ifdef UCACHE_HAVE_BOUNDARY_SHM
const uint8_t *ucache_shared_boundary_digest_memo(ucache_ctx *ctx, size_t requested_size)
{
	ucache_storage *storage = &ctx->storage;

	if (!storage->boundary_digest_memoized) {
		ucache_shared_boundary_digest(ctx, requested_size, storage->boundary_digest_memo);

		storage->boundary_digest_memoized = true;
	}

	return storage->boundary_digest_memo;
}

bool ucache_hdr_boundary_lock_file_replaced_locked(const ucache_hdr *hdr)
{
	ucache_ctx *ctx = ucache_active_ctx();

	return ucache_ctx_is_boundary(ctx) &&
		hdr->lock_model == UCACHE_LOCK_MODEL_FCNTL &&
		(
			hdr->lock_file_dev != ucache_boundary_seg_of(&ctx->storage)->lock_dev ||
			hdr->lock_file_ino != ucache_boundary_seg_of(&ctx->storage)->lock_ino
		)
	;
}
#endif /* UCACHE_HAVE_BOUNDARY_SHM */

bool ucache_quiesce_graph_payloads_locked(void)
{
	ucache_hdr *hdr;
	ucache_reader_slot *slot;
	uint64_t waited_us = 0, owner_pid, owner_start_time;
	uint32_t i, used, count, spin = 0;

	if (UCACHE_DEBUG_FAULT("FORCE_GRAPH_NOT_QUIESCENT")) {
		return false;
	}

	if (!UC_G(write_seq_bumped)) {
		return true;
	}

	if (UC_G(reader_drain_state) != 0) {
		return UC_G(reader_drain_state) > 0;
	}

	hdr = ucache_hdr_ptr();
	if (hdr == NULL) {
		return true;
	}

	ucache_atomic_fence_seq_cst();

	for (;;) {
		used = MIN(ucache_atomic_load_32(&hdr->reader_slots_used), UCACHE_READER_SLOTS);

		count = 0;
		for (i = 0; i < used; i++) {
			if (ucache_atomic_load_32(&hdr->reader_slots[i].active) != 0) {
				count++;
			}
		}

		if (count == 0) {
			UC_G(reader_drain_state) = 1;

			return true;
		}

		spin++;

		if ((spin & 0xFFU) == 0) {
			for (i = 0; i < used; i++) {
				slot = &hdr->reader_slots[i];

				owner_start_time = ucache_atomic_load_64(&slot->owner_start_time);
				owner_pid = ucache_atomic_load_64(&slot->owner_pid);

				if (ucache_atomic_load_32(&slot->active) == 0 ||
					owner_pid == 0 ||
					!ucache_owner_is_dead(owner_pid, owner_start_time)
				) {
					continue;
				}

				if (ucache_atomic_load_64(&slot->owner_pid) == owner_pid &&
					ucache_atomic_load_64(&slot->owner_start_time) == owner_start_time
				) {
					ucache_atomic_cas_32(&slot->active, 1, 0);
				}
			}
		}

		if (spin > UCACHE_READER_DRAIN_SPIN) {
			waited_us += ucache_platform.sleep_us(50);

			if (waited_us > UCACHE_READER_DRAIN_TIMEOUT_US) {
				UC_G(reader_drain_state) = -1;

				return false;
			}
		}
	}
}

#if defined(ZTS) && !defined(ZEND_WIN32)
void ucache_lock_storage_startup_before_fork(void)
{
	ucache_startup_storage_lock();
}

void ucache_unlock_storage_startup_after_fork(void)
{
	ucache_startup_storage_unlock();
}

void ucache_reinit_storage_locks_after_fork(void)
{
	php_ucache_partition *partition;

	ucache_zts_lock_reinit_after_fork(&ucache_ctx_state.storage);

	for (partition = ucache_partitions; partition != NULL; partition = partition->next) {
		ucache_zts_lock_reinit_after_fork(&partition->ctx.storage);
	}
}
#endif

bool ucache_optimistic_reader_begin(ucache_hdr *hdr, uint32_t *slot_idx_ptr)
{
	int32_t slot_idx;

	if (!UCACHE_OPTIMISTIC_ENABLED) {
		return false;
	}

	slot_idx = ucache_claim_reader_slot(hdr);
	if (slot_idx < 0) {
		return false;
	}

	*slot_idx_ptr = (uint32_t) slot_idx;

	ucache_atomic_store_32(&hdr->reader_slots[slot_idx].active, 1);

	ucache_atomic_fence_seq_cst();

	return true;
}

void ucache_optimistic_reader_end(ucache_hdr *hdr, uint32_t slot_idx)
{
	ucache_atomic_store_32(&hdr->reader_slots[slot_idx].active, 0);
}

void ucache_abandon_graph_pin_claims(void)
{
	ucache_graph_pin_claim *claims = UC_G(graph_pin_claims);
	ucache_graph_pin_slot *slot;
	uint32_t i = 0;
	int expected, my_pid = (int) (uint32_t) ucache_cached_pid();

	while (i < UC_G(graph_pin_claim_count)) {
		slot = &claims[i].hdr->graph_pin_slots[claims[i].slot_idx];
		expected = atomic_load(&slot->owner_pid);
		if (expected == my_pid && atomic_load(&slot->pin_count) == 0) {
			i++;

			continue;
		}

		claims[i] = claims[--UC_G(graph_pin_claim_count)];
		if (expected == my_pid) {
			ucache_atomic_store_64(&slot->owner_start_time, 0);

			atomic_compare_exchange_strong(
				&slot->owner_pid,
				&expected,
				UCACHE_GRAPH_PIN_OWNER_ABANDONED
			);
		}
	}
}

void ucache_release_thread_graph_pin_claims(ucache_globals *globals)
{
	ucache_graph_pin_slot *slot;
	uint32_t i;
	int expected;

	for (i = 0; i < globals->graph_pin_claim_count; i++) {
		if (!ucache_claim_hdr_is_attached(globals->graph_pin_claims[i].hdr)) {
			continue;
		}

		slot = &globals->graph_pin_claims[i].hdr->graph_pin_slots[
			globals->graph_pin_claims[i].slot_idx
		];

		expected = atomic_load(&slot->owner_pid);
		if (expected != (int) (uint32_t) ucache_cached_pid()) {
			continue;
		}

		ucache_atomic_store_64(&slot->owner_start_time, 0);

		if (atomic_load(&slot->pin_count) != 0) {
			atomic_compare_exchange_strong(
				&slot->owner_pid,
				&expected,
				UCACHE_GRAPH_PIN_OWNER_ABANDONED
			);

			continue;
		}

		atomic_compare_exchange_strong(&slot->owner_pid, &expected, 0);
	}

	globals->graph_pin_claim_count = 0;
}

void ucache_release_thread_reader_claims(ucache_globals *globals)
{
	ucache_reader_slot *slot;
	uint64_t my_pid;
	uint32_t i;

	if (globals->reader_claim_count == 0) {
		return;
	}

	my_pid = ucache_cached_pid();

	for (i = 0; i < globals->reader_claim_count; i++) {
		if (!ucache_claim_hdr_is_attached(globals->reader_claims[i].hdr)) {
			continue;
		}

		slot = &globals->reader_claims[i].hdr->reader_slots[globals->reader_claims[i].slot_idx];
		if (ucache_atomic_load_64(&slot->owner_pid) != my_pid) {
			continue;
		}

		ucache_atomic_store_32(&slot->active, 0);

		ucache_atomic_store_64(&slot->owner_start_time, 0);
		ucache_atomic_cas_64(&slot->owner_pid, my_pid, 0);
	}

	globals->reader_claim_count = 0;
}

ZEND_API bool php_ucache_is_enabled_by_ini(void)
{
	return ucache_globals_allocated() && !ucache_is_disabled_for_sapi() && UC_G(shm_size) != 0;
}

void ucache_reset_runtime(void)
{
	ucache_runtime *runtime = ucache_active_runtime();

	UC_G(runtime_resolved) = false;

	memset(runtime, 0, sizeof(*runtime));

	runtime->configured_mem = UC_G(shm_size);

	runtime->enabled = runtime->configured_mem != 0;
	if (!runtime->enabled) {
		runtime->available = false;
		runtime->failure_reason = PHP_UCACHE_REASON_DISABLED_BY_INI;
	}
}

bool ucache_storage_startup_is_complete(const ucache_storage *storage)
{
#ifdef ZTS
	return ucache_atomic_load_32(&storage->startup_complete) != 0;
#else
	return storage->initialized && storage->lock_initialized;
#endif
}

void ucache_reset_storage(void)
{
	ucache_storage *storage = &ucache_active_ctx()->storage;

	memset(storage, 0, sizeof(*storage));

	storage->lock_file = -1;
}

bool ucache_hdr_init_locked(void)
{
	ucache_storage *storage = &ucache_active_ctx()->storage;
	ucache_hdr *hdr = ucache_hdr_ptr();
	uint32_t capacity, data_offset;

	if (hdr == NULL) {
		return false;
	}

	ucache_hdr_layout_memo(storage, &capacity, &data_offset);

	if (hdr->magic == UCACHE_MAGIC) {
		if (ucache_hdr_layout_matches_memo_locked(storage, hdr, capacity, data_offset)) {
#ifdef UCACHE_HAVE_BOUNDARY_SHM
			if (ucache_hdr_boundary_lock_file_replaced_locked(hdr)) {
				ucache_shared_boundary_retire_seg_name();

				return false;
			}
#endif /* UCACHE_HAVE_BOUNDARY_SHM */

			return ucache_hdr_boundary_identity_matches_locked(hdr, storage->size);
		}

		if (ucache_ctx_is_boundary(ucache_active_ctx())) {
			return false;
		}
	}

	return ucache_hdr_format_fresh_locked(hdr, capacity, data_offset);
}

bool ucache_mem_model_is_available(const char *model)
{
	return ucache_find_storage_handler(model) != NULL;
}

size_t ucache_shm_size_min(void)
{
	size_t size = 0, smallest;

	while ((smallest = ucache_shm_size_floor(size) + UCACHE_BLOCK_MIN_SIZE) > size) {
		size = smallest;
	}

	return size;
}

bool ucache_hdr_adoptable_locked(void)
{
	ucache_storage *storage = &ucache_active_ctx()->storage;
	ucache_hdr *hdr = ucache_hdr_ptr();
	uint32_t capacity, data_offset;

	if (hdr == NULL) {
		return false;
	}

	ucache_hdr_layout_memo(storage, &capacity, &data_offset);

	if (hdr->magic != UCACHE_MAGIC ||
		!ucache_hdr_layout_matches_memo_locked(storage, hdr, capacity, data_offset)
	) {
		return false;
	}

	return ucache_hdr_boundary_identity_matches_locked(hdr, storage->size);
}

bool ucache_startup_storage_before_req(void)
{
	ucache_storage *storage = &ucache_active_ctx()->storage;

	if (!ucache_runtime_opted_in) {
		ucache_set_unavailable(PHP_UCACHE_REASON_SAPI_NOT_ENABLED);

		return true;
	}

	if (!ucache_startup_storage()) {
		ucache_set_unavailable(PHP_UCACHE_REASON_SHM_INIT_FAILED);

		return false;
	}

	storage->initialized_before_req = true;

	return true;
}

void ucache_shutdown_storage(void)
{
	ucache_storage *storage = &ucache_active_ctx()->storage;

	ucache_startup_storage_lock();

	ucache_destroy_lock();

	ucache_cleanup_seg(storage->handler, storage->seg);

	ucache_reset_storage();

	ucache_startup_storage_unlock();
}

void ucache_classify_sapi(void)
{
	const char *name = sapi_module.name;
	uint32_t flags = 0;

	if (name != NULL) {
		if (strcmp(name, "cli") == 0 || strcmp(name, "phpdbg") == 0) {
			flags |= UCACHE_SAPI_CLI;
		}

		if (strcmp(name, "fpm-fcgi") == 0 || strcmp(name, "apache2handler") == 0 || strcmp(name, "cli-server") == 0) {
			flags |= UCACHE_SAPI_PRE_REQ_STORAGE;
		}
	}

	ucache_classified_sapi_flags = flags;
	ucache_classified_sapi_name = name;
}

void ucache_ensure_ready_impl(void)
{
	ucache_ctx *ctx = ucache_active_ctx();

	ucache_resolve_runtime();

	UC_G(runtime_resolved) = true;
	UC_G(runtime_resolved_ctx) = ctx;
	UC_G(runtime_resolved_enabled) = UC_G(enable);
}
