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

#include "user_cache_entries.h"

#include "Zend/zend_closures.h"
#include "Zend/zend_gc.h"

#define UCACHE_DIRECT_STR_MIN_LEN		4096U

#define UCACHE_EXPUNGE_WRITE_OP_INTERVAL	64U
#define UCACHE_REHASH_TOMBSTONE_DIVISOR		4U
#define UCACHE_REHASH_EMPTY_SLOT_DIVISOR	16U
#define UCACHE_REHASH_RETRY_SKIPS			64U

#define UCACHE_ACCESS_NOW_REFRESH_INTERVAL	1024U
#define UCACHE_ACCESS_SAMPLE_INTERVAL		64U

#define UCACHE_RECORD_STORE_STR_MIN_LEN	256U

#define UCACHE_MAX_REQ_LOCAL_SLOTS	4096U

#define UCACHE_REQ_PIN_BUDGET_DIVISOR	4U

#define UCACHE_TRY_UNLOCK_ON_BAILOUT(stmt) \
	do { \
		zend_try { \
			stmt \
		} zend_catch { \
			ucache_unlock_if_held(); \
			zend_bailout(); \
		} zend_end_try(); \
	} while (0)

typedef enum {
	UCACHE_FIND_SLOT_IGNORE_EXPIRY = 0,
	UCACHE_FIND_SLOT_SKIP_EXPIRED,
	UCACHE_FIND_SLOT_DELETE_EXPIRED
} ucache_find_slot_expiry_mode;

enum {
	UCACHE_FETCH_FINISH_USE_REQ_LOCAL_SLOT = 1 << 0,
	UCACHE_FETCH_FINISH_NO_ALIASES = 1 << 1,
	UCACHE_FETCH_FINISH_RECORD_COPY = 1 << 2,
	UCACHE_FETCH_FINISH_RESTORE_HOOKS_RAN = 1 << 3
};

typedef enum {
	UCACHE_FETCH_LOCATE_MISS,
	UCACHE_FETCH_LOCATE_VAL_HIT,
	UCACHE_FETCH_LOCATE_SLOT,
	UCACHE_FETCH_LOCATE_UNCACHED
} ucache_fetch_locate_result;

typedef enum {
	UCACHE_STORE_ATTEMPT_STORED,
	UCACHE_STORE_ATTEMPT_RETRY,
	UCACHE_STORE_ATTEMPT_FAILED
} ucache_store_attempt_result;

static zend_never_inline uint64_t ucache_clock_now_noinline(void);
static zend_never_inline void ucache_key_record_release_refcounted_val(ucache_key_record *record, zval *detached);
static zend_never_inline ucache_optimistic_result ucache_optimistic_locate_fallback(
		ucache_hdr *hdr,
		uint32_t capacity,
		ucache_key_record *record,
		uint64_t seq,
		uint64_t epoch,
		bool stamp_access,
		ucache_entry *snapshot);
static bool ucache_rehash_locked(ucache_hdr *hdr);

static bool ucache_find_slot_in_hdr_locked(
		ucache_hdr *hdr,
		zend_string *key,
		zend_ulong hash,
		ucache_find_slot_expiry_mode expiry_mode,
		uint32_t *slot_idx,
		bool *found);

static zend_always_inline uint8_t ucache_entry_state(const ucache_entry *entry)
{
	return (uint8_t) MIN(ucache_entry_kind(entry), UCACHE_ENTRY_USED);
}

static zend_always_inline bool ucache_entry_is_empty(const ucache_entry *entry)
{
	return ucache_entry_kind(entry) == UCACHE_ENTRY_EMPTY;
}

static zend_always_inline void ucache_entry_set_val_type(ucache_entry *entry, uint8_t val_type)
{
	ucache_entry_set_kind(entry, UCACHE_ENTRY_USED + val_type);
}

static zend_always_inline uint32_t ucache_pool_bucket_for_key(const char *key, size_t key_len)
{
	const char *delim = memchr(key, UCACHE_KEY_DELIM_CHAR, key_len);
	size_t prefix_len = delim != NULL ? (size_t) (delim - key) + 1 : key_len;

	return (uint32_t) (zend_inline_hash_func(key, prefix_len) & (UCACHE_POOL_BUCKETS - 1));
}

static zend_always_inline uint32_t ucache_pool_link_ref(uint32_t slot)
{
	return slot + 1;
}

static zend_always_inline void ucache_pool_idx_link_locked(
		ucache_hdr *hdr,
		ucache_entry *entry,
		uint32_t slot)
{
	ucache_pool_links *links = ucache_pool_links_ptr(hdr);
	ucache_pool_links *link = &links[slot];
	uint32_t bucket = ucache_entry_pool_bucket(entry), ref = ucache_pool_link_ref(slot);

	link->prev = 0;
	link->next = hdr->pool_bucket_heads[bucket];
	if (link->next != 0) {
		links[ucache_pool_link_ref_slot(link->next)].prev = ref;
	}

	hdr->pool_bucket_heads[bucket] = ref;
}

static zend_always_inline uint64_t ucache_load_seq(const ucache_hdr *hdr, zend_string *key)
{
	uint64_t seq = ucache_atomic_load_64(&hdr->write_seq);
#ifdef UCACHE_HAVE_SCALAR_WRITE
	uint64_t stripe_seq;

	/* Relaxed: stripes are enabled in a write section, ordered by the write_seq acquire load above. */
	if (UNEXPECTED(UCACHE_ATOMIC_LOAD_32_RELAXED(&hdr->scalar_write_enabled) != 0)) {
		stripe_seq = ucache_atomic_load_64(
			&hdr->scalar_write_stripes[ucache_scalar_write_idx(zend_string_hash_val(key))].seq
		);

		if (((seq | stripe_seq) & 1) != 0) {
			return UINT64_MAX;
		}

		seq += stripe_seq;
	}
#else
	(void) key;
#endif

	return seq;
}

static zend_always_inline uint64_t ucache_read_seq(const ucache_hdr *hdr, zend_string *key)
{
	ucache_atomic_fence_acquire();

	return ucache_load_seq(hdr, key);
}

static zend_always_inline bool ucache_val_uses_offset(uint8_t val_type)
{
	return
		val_type == UCACHE_VAL_STR ||
		val_type == UCACHE_VAL_SGRAPH
	;
}

static zend_always_inline void ucache_entry_set_long_zero_filled(ucache_entry *entry, zend_long lval)
{
	entry->double_val = 0;
	entry->long_val = lval;
}

static zend_always_inline uint8_t *ucache_ptr_in_hdr(
		const ucache_hdr *hdr,
		uint32_t offset)
{
	return (uint8_t *) hdr + ucache_offset_bytes(offset);
}

static zend_always_inline const char *ucache_entry_key_in_hdr(
		const ucache_hdr *hdr,
		const ucache_entry *entry)
{
	return (const char *) hdr + ucache_entry_key_pos(entry);
}

static zend_always_inline uint32_t ucache_combined_key_offset(uint32_t val_offset, size_t payload_size)
{
	return val_offset + (uint32_t) (payload_size >> UCACHE_OFFSET_SHIFT);
}

static zend_always_inline uint16_t ucache_combined_key_flags(size_t payload_size)
{
	return UCACHE_ENTRY_FLAG_COMBINED_VAL_KEY |
		(uint16_t) ((payload_size & (UCACHE_OFFSET_UNIT - 1)) << UCACHE_ENTRY_KEY_BYTE_SHIFT)
	;
}

static zend_always_inline bool ucache_key_equals(
		const ucache_hdr *hdr,
		const ucache_entry *entry,
		zend_string *key,
		zend_ulong hash)
{
	if (
		!ucache_entry_is_used(entry) ||
		entry->hash != ucache_table_hash(hash) ||
		entry->key_len != ZSTR_LEN(key)
	) {
		return false;
	}

	return memcmp(
		ucache_entry_key_in_hdr(hdr, entry),
		ZSTR_VAL(key),
		ZSTR_LEN(key)
	) == 0;
}

static zend_always_inline bool ucache_optimistic_key_matches(
		const ucache_hdr *hdr,
		size_t key_pos,
		const zend_string *key)
{
	return ucache_bytes_in_bounds(hdr, key_pos, ZSTR_LEN(key)) &&
		memcmp((const char *) hdr + key_pos, ZSTR_VAL(key), ZSTR_LEN(key)) == 0
	;
}

static zend_always_inline bool ucache_reuse_block_locked(
		ucache_hdr *hdr,
		uint32_t reusable_offset,
		size_t size,
		uint32_t owner)
{
	uint32_t capacity;

	if (reusable_offset == 0) {
		return false;
	}

	capacity = ucache_block_payload_capacity(hdr, reusable_offset);
	if (capacity < size) {
		return false;
	}

	if (capacity - size >= UCACHE_BLOCK_MIN_SIZE) {
		ucache_shrink_locked(reusable_offset, size);

		ucache_pool_bucket_changed_locked(
			hdr,
			ucache_entry_pool_bucket(&ucache_entries_ptr(hdr)[owner])
		);
	}

	return true;
}

static zend_always_inline uint32_t ucache_write_payload_locked(
		ucache_hdr *hdr,
		uint32_t reusable_offset,
		size_t size,
		const void *src,
		uint32_t owner)
{
	if (ucache_reuse_block_locked(hdr, reusable_offset, size, owner)) {
		memcpy(ucache_ptr_in_hdr(hdr, reusable_offset), src, size);

		return reusable_offset;
	}

	return ucache_alloc_locked(size, src, owner);
}

static zend_always_inline void ucache_entry_set_block_owner(const ucache_entry *entry, uint32_t slot)
{
	uint32_t val_offset = ucache_entry_val_offset(entry);

	if (val_offset != 0) {
		ucache_block_ptr(ucache_payload_block_offset(val_offset))->owner = slot;
	}

	if ((entry->flags & UCACHE_ENTRY_FLAG_COMBINED_VAL_KEY) == 0 &&
		entry->key_offset != 0
	) {
		ucache_block_ptr(ucache_payload_block_offset(entry->key_offset))->owner = slot;
	}
}

static zend_always_inline void ucache_expiry_floor_lower_locked(ucache_hdr *hdr, uint32_t expires_at)
{
	if (expires_at != 0 && expires_at < hdr->expiry_floor) {
		hdr->expiry_floor = expires_at;
	}
}

static zend_always_inline uint32_t ucache_access_now(void)
{
	uint32_t now = UC_G(access_now);

	if (now == 0 || ++UC_G(access_now_touches) >= UCACHE_ACCESS_NOW_REFRESH_INTERVAL) {
		now = (uint32_t) (ucache_clock_now_noinline() / UCACHE_CLOCK_TICKS_PER_SEC);
		if (now == 0) {
			now = 1;
		}

		UC_G(access_now) = now;
		UC_G(access_now_touches) = 0;
	}

	return now;
}

static zend_always_inline void ucache_record_entry_access(
		ucache_hdr *hdr,
		uint32_t capacity,
		uint32_t slot_idx,
		uint32_t now)
{
	uint32_t *stamp, prev;

	stamp = &((uint32_t *) (ucache_entries_ptr(hdr) + capacity))[slot_idx];

	prev = UCACHE_ATOMIC_LOAD_32_RELAXED(stamp);
	while (prev < now) {
		if (ucache_atomic_cas_32(stamp, prev, now)) {
			break;
		}

		prev = UCACHE_ATOMIC_LOAD_32_RELAXED(stamp);
	}
}

static zend_always_inline void ucache_touch_entry_access(
		ucache_hdr *hdr,
		uint32_t capacity,
		uint32_t slot_idx)
{
	ucache_record_entry_access(hdr, capacity, slot_idx, ucache_access_now());
}

static zend_always_inline void ucache_touch_cached_entry_access(
		ucache_hdr *hdr,
		uint32_t slot_idx,
		uint16_t *touches)
{
	const ucache_storage *storage;
	uint64_t now;

	if (EXPECTED(++*touches < UCACHE_ACCESS_SAMPLE_INTERVAL)) {
		return;
	}

	*touches = 0;

	now = ucache_clock_now_noinline();

	ucache_access_note_time(now);

	storage = &ucache_active_ctx()->storage;
	if (storage->layout_memo_valid && slot_idx < storage->capacity_memo) {
		ucache_record_entry_access(
			hdr,
			storage->capacity_memo,
			slot_idx,
			UC_G(access_now)
		);
	}
}

static zend_always_inline bool ucache_is_expired_now(
		const ucache_hdr *hdr,
		const ucache_entry *entry,
		uint64_t *now)
{
	if (!ucache_entry_is_used(entry) || entry->expires_at == 0) {
		return false;
	}

	if (*now == 0) {
		*now = ucache_clock_now_noinline();

		ucache_access_note_time(*now);
	}

	return ucache_is_expired(entry, ucache_time_rel(hdr, *now));
}

static zend_always_inline void ucache_note_expired_read(void)
{
	UC_G(expired_read_observations)++;
}

static zend_always_inline bool ucache_scalar_to_zval(
		uint8_t val_type,
		const zend_long *lval,
		const double *dval,
		zval *return_value)
{
	switch (val_type) {
		case UCACHE_VAL_NULL:
			ZVAL_NULL(return_value);

			return true;
		case UCACHE_VAL_TRUE:
			ZVAL_TRUE(return_value);

			return true;
		case UCACHE_VAL_FALSE:
			ZVAL_FALSE(return_value);

			return true;
		case UCACHE_VAL_LONG:
			ZVAL_LONG(return_value, *lval);

			return true;
		case UCACHE_VAL_DOUBLE:
			ZVAL_DOUBLE(return_value, *dval);

			return true;
		default:
			return false;
	}
}

static zend_always_inline bool ucache_can_run_userland(void)
{
	return !UC_G(lock_held) && UC_G(scalar_write_hdr) == NULL;
}

static zend_always_inline void ucache_key_record_release_val(ucache_key_record *record, zval *detached)
{
	uint8_t kind = record->val_kind;

	if (kind == UCACHE_RECORD_VAL_NONE) {
		return;
	}

	record->val_kind = UCACHE_RECORD_VAL_NONE;

	if (Z_REFCOUNTED(record->val)) {
		ZEND_ASSERT(kind == UCACHE_RECORD_VAL_COPY || kind == UCACHE_RECORD_VAL_PINNED_COPY);

		ucache_key_record_release_refcounted_val(record, detached);

		return;
	}

	ZVAL_UNDEF(&record->val);
}

static zend_always_inline bool ucache_entry_written_after(const ucache_entry *entry, uint64_t epoch)
{
	return entry->gen > epoch;
}

static zend_always_inline void ucache_key_record_mark_miss(
		ucache_key_record *record,
		uint64_t epoch,
		zval *detached)
{
	ucache_key_record_release_val(record, detached);

	record->mutation_epoch = epoch;
	record->state = UCACHE_RECORD_MISS;
	record->access_touches = 0;
}

static zend_always_inline void ucache_key_record_mark_hit(
		ucache_key_record *record,
		uint64_t epoch,
		uint32_t slot_idx,
		const ucache_entry *entry,
		zval *detached)
{
	if (record->state != UCACHE_RECORD_HIT || ucache_entry_written_after(entry, record->mutation_epoch)) {
		ucache_key_record_release_val(record, detached);
	}

	record->mutation_epoch = epoch;
	record->slot_idx = slot_idx;
	record->expires_at = entry->expires_at;
	record->state = UCACHE_RECORD_HIT;
	record->access_touches = 0;

	if (record->val_kind == UCACHE_RECORD_VAL_NONE &&
		ucache_scalar_to_zval(
			ucache_entry_val_type(entry),
			&entry->long_val,
			&entry->double_val,
			&record->val
		)
	) {
		record->val_kind = UCACHE_RECORD_VAL_COPY;
	}
}

static zend_always_inline void ucache_key_record_mark_stored(
		ucache_key_record *record,
		uint64_t gen,
		uint32_t slot_idx,
		uint32_t expires_at)
{
	ucache_key_record_release_val(record, NULL);

	record->mutation_epoch = gen;
	record->slot_idx = slot_idx;
	record->expires_at = expires_at;
	record->state = UCACHE_RECORD_HIT;
	record->access_touches = 0;
}

static zend_always_inline void ucache_key_record_reset_impl(ucache_key_record *record, zval *detached)
{
	ucache_key_record_release_val(record, detached);

	record->mutation_epoch = 0;
	record->state = UCACHE_RECORD_EMPTY;
}

static zend_always_inline bool ucache_key_record_charge_copy(ucache_key_record *record, size_t bytes)
{
	if (!ucache_record_val_fits(bytes)) {
		return false;
	}

	UC_G(record_val_bytes) += bytes;

	record->copy_charged_bytes = (uint32_t) bytes;

	return true;
}

static zend_always_inline void ucache_key_record_cache_copy(
		ucache_key_record *record,
		zval *val,
		size_t bytes)
{
	if (record->val_kind != UCACHE_RECORD_VAL_NONE) {
		return;
	}

	if (Z_REFCOUNTED_P(val) && !ucache_key_record_charge_copy(record, bytes)) {
		return;
	}

	ZVAL_COPY(&record->val, val);

	record->val_kind = UCACHE_RECORD_VAL_COPY;
}

static zend_always_inline void ucache_key_record_cache_pinned_copy(
		ucache_key_record *record,
		zval *val,
		uint32_t payload_offset,
		uint32_t val_len)
{
	ZEND_ASSERT(Z_REFCOUNTED_P(val));

	if (record->val_kind != UCACHE_RECORD_VAL_NONE ||
		!ucache_key_record_charge_copy(record, val_len)
	) {
		return;
	}

	ZVAL_COPY(&record->val, val);

	Z_EXTRA(record->val) = payload_offset;

	record->val_kind = UCACHE_RECORD_VAL_PINNED_COPY;
}

static zend_always_inline void ucache_key_record_cache_pinned(
		ucache_key_record *record,
		zval *val,
		uint32_t payload_offset,
		uint32_t ref_idx)
{
	ZEND_ASSERT(!Z_REFCOUNTED_P(val));

	if (record->val_kind != UCACHE_RECORD_VAL_NONE) {
		return;
	}

	ZVAL_COPY_VALUE(&record->val, val);

	Z_EXTRA(record->val) = ref_idx;

	record->pinned_payload_offset = payload_offset;
	record->val_kind = UCACHE_RECORD_VAL_PINNED;
}

static zend_always_inline bool ucache_req_holds_sgraph_ref(uint32_t idx, uint32_t payload_offset)
{
	return idx < UC_G(sgraph_ref_count) &&
		UC_G(sgraph_ref_owner_pid) == ucache_cached_pid() &&
		UC_G(sgraph_refs)[idx].payload_offset == payload_offset &&
		UC_G(sgraph_refs)[idx].ctx == ucache_active_ctx()
	;
}

static zend_always_inline bool ucache_key_record_val_retained(const ucache_key_record *record)
{
	ZEND_ASSERT(record->val_kind != UCACHE_RECORD_VAL_NONE);

	if (!(record->val_kind & UCACHE_RECORD_VAL_PINNED)) {
		return true;
	}

	if (!(record->val_kind & UCACHE_RECORD_VAL_COPY)) {
		return ucache_req_holds_sgraph_ref(
			Z_EXTRA(record->val),
			record->pinned_payload_offset
		);
	}

	return ucache_req_sgraph_ref_idx(Z_EXTRA(record->val)) != UINT32_MAX;
}

static zend_always_inline bool ucache_key_record_val_usable(
		const ucache_hdr *hdr,
		const ucache_key_record *record)
{
	uint64_t now;

	if (record->expires_at != 0) {
		now = ucache_clock_now_noinline();

		ucache_access_note_time(now);

		if ((uint64_t) record->expires_at <= ucache_time_rel(hdr, now)) {
			return false;
		}
	}

	return ucache_key_record_val_retained(record);
}

static zend_always_inline void ucache_init_prepared_val(ucache_prepared_val *prepared)
{
	memset(prepared, 0, sizeof(*prepared));

	prepared->val_type = UCACHE_VAL_NULL;
}

static zend_always_inline bool ucache_long_add_overflow(
		zend_long lhs,
		zend_long rhs,
		zend_long *result)
{
	*result = (zend_long) ((zend_ulong) lhs + (zend_ulong) rhs);

	return (rhs > 0 && lhs > ZEND_LONG_MAX - rhs) ||
		(rhs < 0 && lhs < ZEND_LONG_MIN - rhs)
	;
}

static zend_always_inline bool ucache_long_sub_overflow(
		zend_long lhs,
		zend_long rhs,
		zend_long *result)
{
	*result = (zend_long) ((zend_ulong) lhs - (zend_ulong) rhs);

	return (rhs > 0 && lhs < ZEND_LONG_MIN + rhs) ||
		(rhs < 0 && lhs > ZEND_LONG_MAX + rhs)
	;
}

static zend_always_inline bool ucache_rehash_due(const ucache_hdr *hdr)
{
	uint32_t empty_slots = hdr->capacity - hdr->count - hdr->tombstone_count;

	return hdr->tombstone_count > hdr->capacity / UCACHE_REHASH_TOMBSTONE_DIVISOR || (
		hdr->tombstone_count != 0 &&
		empty_slots < hdr->capacity / UCACHE_REHASH_EMPTY_SLOT_DIVISOR
	);
}

static zend_always_inline void ucache_maybe_rehash_locked(void)
{
	ucache_hdr *hdr = ucache_hdr_ptr();

	if (hdr == NULL ||
		!ucache_hdr_is_initialized_locked() ||
		!ucache_rehash_due(hdr)
	) {
		return;
	}

	if (UC_G(rehash_retry_skips) != 0) {
		UC_G(rehash_retry_skips)--;

		return;
	}

	if (!ucache_rehash_locked(hdr)) {
		UC_G(rehash_retry_skips) = UCACHE_REHASH_RETRY_SKIPS;
	}
}

static zend_always_inline bool ucache_expunge_due_on_this_write(void)
{
	return UC_G(expunge_write_ops) >= UCACHE_EXPUNGE_WRITE_OP_INTERVAL - 1 ||
		UC_G(expired_read_observations) >= UCACHE_EXPIRED_READ_EXPUNGE_THRESHOLD
	;
}

static zend_always_inline void ucache_maybe_expunge_expired_locked(void)
{
	if (EXPECTED(!ucache_expunge_due_on_this_write())) {
		UC_G(expunge_write_ops)++;

		return;
	}

	UC_G(expired_read_observations) = 0;
	UC_G(expunge_write_ops) = 0;

	ucache_expunge_expired_bounded_locked();
}

static zend_always_inline uint32_t ucache_optimistic_capacity(const ucache_hdr *hdr)
{
	const ucache_storage *storage = &ucache_active_ctx()->storage;

	if (hdr->magic != UCACHE_MAGIC ||
		!storage->layout_memo_valid ||
		hdr->capacity != storage->capacity_memo ||
		hdr->data_offset != storage->data_offset_memo
	) {
		return 0;
	}

	return storage->capacity_memo;
}

static zend_always_inline bool ucache_req_pin_fits_budget(const ucache_hdr *hdr, uint32_t payload_len)
{
	return (uint64_t) UC_G(sgraph_ref_bytes) + payload_len <= hdr->data_size / UCACHE_REQ_PIN_BUDGET_DIVISOR;
}

static zend_always_inline uint32_t ucache_fetch_finish_flags_after_decode(uint32_t flags, uint32_t hook_calls_before)
{
	if (UNEXPECTED(UC_G(restore_hook_calls) != hook_calls_before)) {
		flags |= UCACHE_FETCH_FINISH_RESTORE_HOOKS_RAN;
	}

	return flags;
}

static zend_always_inline ucache_optimistic_result ucache_optimistic_unrestorable(
		const ucache_entry *snapshot)
{
	UC_G(unrestorable_gen) = snapshot->gen;

	return UCACHE_OPTIMISTIC_UNRESTORABLE;
}

static zend_always_inline uint32_t ucache_insert_cap(uint32_t capacity)
{
	return capacity - capacity / 8;
}

static zend_always_inline bool ucache_scalar_write_locate(
		ucache_hdr *hdr,
		const ucache_key_record *record,
		uint32_t *slot_idx)
{
	zend_string *key = record->storage_key;
	bool found;

	if (record->state == UCACHE_RECORD_HIT &&
		record->slot_idx < hdr->capacity &&
		(
			record->mutation_epoch == ucache_atomic_load_64(&hdr->mutation_epoch) ||
			ucache_key_equals(
				hdr,
				&ucache_entries_ptr(hdr)[record->slot_idx],
				key,
				ZSTR_H(key)
			)
		)
	) {
		*slot_idx = record->slot_idx;

		return true;
	}

	return ucache_find_slot_in_hdr_locked(
			hdr,
			key,
			ZSTR_H(key),
			UCACHE_FIND_SLOT_IGNORE_EXPIRY,
			slot_idx,
			&found
		) && found
	;
}

static zend_always_inline bool ucache_key_record_is_cur_miss(const ucache_key_record *record)
{
	ucache_hdr *hdr;

	if (record->state != UCACHE_RECORD_MISS) {
		return false;
	}

	hdr = ucache_hdr_ptr();

	return hdr != NULL && record->mutation_epoch == ucache_atomic_load_64(&hdr->mutation_epoch);
}

static zend_always_inline void ucache_scalar_write_release_record_val(ucache_key_record *record)
{
	while (UNEXPECTED(Z_REFCOUNTED(record->val))) {
		ucache_key_record_release_val(record, NULL);
	}
}

static zend_always_inline bool ucache_scalar_write_can_skip_sweep(void)
{
	if (ucache_expunge_due_on_this_write()) {
		return false;
	}

	UC_G(expunge_write_ops)++;

	return true;
}

static zend_always_inline bool ucache_optimistic_key_record_slot(
		ucache_hdr *hdr,
		uint32_t capacity,
		const ucache_key_record *record,
		bool cur,
		ucache_entry *snapshot)
{
	const ucache_entry *entry;
	zend_string *key = record->storage_key;
	uint64_t now;

	if (record->slot_idx >= capacity) {
		return false;
	}

	entry = &ucache_entries_ptr(hdr)[record->slot_idx];

	if (!ucache_entry_is_used(entry) ||
		(
			!cur &&
			(
				entry->hash != ucache_table_hash(ZSTR_H(key)) ||
				entry->key_len != ZSTR_LEN(key) ||
				!ucache_optimistic_key_matches(hdr, ucache_entry_key_pos(entry), key)
			)
		)
	) {
		return false;
	}

	if (entry->expires_at != 0) {
		now = ucache_clock_now_noinline();

		ucache_access_note_time(now);

		if ((uint64_t) entry->expires_at <= ucache_time_rel(hdr, now)) {
			return false;
		}
	}

	*snapshot = *entry;

	return true;
}

static zend_always_inline ucache_optimistic_result ucache_optimistic_locate(
		ucache_hdr *hdr,
		uint32_t capacity,
		ucache_key_record *record,
		uint64_t seq,
		uint64_t epoch,
		bool stamp_access,
		ucache_entry *snapshot)
{
	if (record->state != UCACHE_RECORD_HIT ||
		record->mutation_epoch != epoch ||
		!ucache_optimistic_key_record_slot(hdr, capacity, record, true, snapshot)
	) {
		return ucache_optimistic_locate_fallback(hdr, capacity, record, seq, epoch, stamp_access, snapshot);
	}

	if (ucache_read_seq(hdr, record->storage_key) != seq) {
		return UCACHE_OPTIMISTIC_FALLBACK;
	}

	if (stamp_access) {
		ucache_touch_cached_entry_access(hdr, record->slot_idx, &record->access_touches);
	}

	return UCACHE_OPTIMISTIC_FOUND;
}

/* Out of line: clock_gettime()'s &local would add a stack protector to every hot caller. */
static zend_never_inline uint64_t ucache_clock_now_noinline(void)
{
	return ucache_clock_now();
}

static zend_never_inline void ucache_key_record_release_refcounted_val(ucache_key_record *record, zval *detached)
{
	zend_refcounted *counted;

	UC_G(record_val_bytes) -= record->copy_charged_bytes;

	if (detached != NULL && Z_COLLECTABLE(record->val)) {
		ZEND_ASSERT(Z_ISUNDEF_P(detached));

		ZVAL_COPY_VALUE(detached, &record->val);
		ZVAL_UNDEF(&record->val);

		return;
	}

	ZEND_ASSERT(!Z_COLLECTABLE(record->val) || ucache_can_run_userland());

	counted = Z_COUNTED(record->val);

	ZVAL_UNDEF(&record->val);

	GC_DTOR(counted);
}

static bool ucache_optimistic_probe(
		const ucache_hdr *hdr,
		uint32_t capacity,
		zend_string *key,
		zend_ulong hash,
		const ucache_entry *entries,
		ucache_entry *snapshot,
		uint32_t *slot_idx,
		bool *found)
{
	const ucache_entry *entry;
	uint64_t now = 0;
	uint32_t i, step;
	size_t key_pos;

	i = ucache_table_hash(hash) % capacity;

	for (step = 0; step < capacity; step++) {
		entry = &entries[i];

		switch (ucache_entry_state(entry)) {
			case UCACHE_ENTRY_EMPTY:
				*found = false;

				return true;
			case UCACHE_ENTRY_USED:
				if (entry->hash == ucache_table_hash(hash) && entry->key_len == ZSTR_LEN(key)) {
					key_pos = ucache_entry_key_pos(entry);
					if (!ucache_bytes_in_bounds(hdr, key_pos, ZSTR_LEN(key))) {
						return false;
					}

					if (memcmp((const char *) hdr + key_pos, ZSTR_VAL(key), ZSTR_LEN(key)) == 0) {
						if (ucache_is_expired_now(hdr, entry, &now)) {
							ucache_note_expired_read();

							*found = false;

							return true;
						}

						*snapshot = *entry;
						*slot_idx = i;
						*found = true;

						return true;
					}
				}

				break;
			case UCACHE_ENTRY_TOMBSTONE:
				break;
		}

		++i;

		if (i == capacity) {
			i = 0;
		}
	}

	*found = false;

	return true;
}

static ucache_op_lease *ucache_op_lease_reserve(uint32_t payload_offset)
{
	ucache_op_lease *lease;
	ucache_ctx *ctx = ucache_active_ctx();
	uint64_t pid = ucache_cached_pid();

	for (lease = UC_G(op_leases); lease != NULL; lease = lease->next) {
		if (lease->ctx == ctx && lease->payload_offset == payload_offset && lease->owner_pid == pid) {
			lease->users++;

			return lease;
		}
	}

	if (UC_G(op_lease_free) != NULL) {
		lease = UC_G(op_lease_free);

		UC_G(op_lease_free) = lease->next;
	} else if (UC_G(op_lease_inline).ctx == NULL) {
		lease = &UC_G(op_lease_inline);
	} else {
		lease = emalloc(sizeof(*lease));
	}

	lease->ctx = ctx;
	lease->payload_offset = payload_offset;
	lease->owner_pid = pid;
	lease->users = 1;
	lease->acquired = false;
	lease->next = UC_G(op_leases);

	UC_G(op_leases) = lease;

	return lease;
}

static bool ucache_op_lease_acquire(
		ucache_op_lease *lease,
		bool *resource_limited)
{
	*resource_limited = false;

	if (!lease->acquired) {
		lease->acquired = ucache_sgraph_acquire_ref(lease->payload_offset, resource_limited);
	}

	return lease->acquired;
}

static void ucache_op_lease_release(ucache_op_lease *lease)
{
	ucache_op_lease **link;
	ucache_ctx *prev;
	bool released;

	ZEND_ASSERT(lease->users == 0);

	if (lease->acquired && lease->owner_pid == ucache_cached_pid()) {
		prev = ucache_activate_ctx(lease->ctx);
		released = ucache_sgraph_release_op_ref(lease->payload_offset);

		ucache_restore_ctx(prev);

		if (!released) {
			return;
		}
	}

	link = &UC_G(op_leases);
	while (*link != lease) {
		link = &(*link)->next;
	}

	*link = lease->next;
	lease->ctx = NULL;

	if (lease != &UC_G(op_lease_inline)) {
		lease->next = UC_G(op_lease_free);

		UC_G(op_lease_free) = lease;
	}
}

static void ucache_op_lease_end(ucache_op_lease *lease)
{
	ZEND_ASSERT(lease->users > 0);

	if (--lease->users == 0) {
		ucache_op_lease_release(lease);
	}
}

static bool ucache_materialize_sgraph_locked(
		const ucache_hdr *hdr,
		uint32_t val_offset,
		uint32_t val_len,
		zval *return_value,
		bool *lock_held,
		bool *private_copy)
{
	ucache_sgraph_snapshot *snapshot = NULL;
	ucache_op_lease *lease = NULL;
	bool result, pinned = true, resource_limited = false;

	ZEND_ASSERT(*lock_held && UC_G(lock_held));

	if (UNEXPECTED(UC_G(persistent_exec))) {
		UCACHE_TRY_UNLOCK_ON_BAILOUT(
			lease = ucache_op_lease_reserve(val_offset);
		);

		if (!ucache_op_lease_acquire(lease, &resource_limited)) {
			ucache_op_lease_end(lease);

			lease = NULL;
			pinned = false;
		}
	} else if (ucache_req_sgraph_ref_idx(val_offset) == UINT32_MAX) {
		if (ucache_req_pin_fits_budget(hdr, val_len)) {
			UCACHE_TRY_UNLOCK_ON_BAILOUT(
				ucache_sgraph_ref_reserve();
			);

			pinned = ucache_sgraph_acquire_ref(val_offset, &resource_limited);
			if (pinned) {
				ucache_register_sgraph_ref(val_offset, val_len);
			}
		} else {
			pinned = false;
			resource_limited = true;
		}
	}

	if (!pinned) {
		if (!resource_limited) {
			return false;
		}

		UCACHE_TRY_UNLOCK_ON_BAILOUT(
			snapshot = ucache_sgraph_snapshot_create(
				ucache_ptr_in_hdr(hdr, val_offset),
				val_len
			);
		);

		if (snapshot == NULL) {
			return false;
		}
	}

	ZVAL_UNDEF(return_value);

	ucache_unlock();

	*lock_held = false;
	*private_copy = snapshot != NULL;

	result = snapshot != NULL
		? ucache_sgraph_decode_snapshot(snapshot, return_value)
		: ucache_sgraph_decode(
			ucache_ptr_in_hdr(hdr, val_offset),
			val_len,
			return_value
		)
	;

	if (!result && Z_TYPE_P(return_value) != IS_UNDEF) {
		zval_ptr_dtor(return_value);

		ZVAL_UNDEF(return_value);
	}

	if (lease != NULL) {
		ucache_op_lease_end(lease);
	}

	return result;
}

static bool ucache_fetch_req_local_slot(
		zend_string *key,
		uint64_t gen,
		zval *return_value)
{
	ucache_req_local_slot *slot = ucache_find_req_local_slot(key, gen);
	bool cloned;

	if (slot == NULL || !slot->has_val) {
		return false;
	}

	slot->clone_depth++;

	cloned = ucache_clone_req_local_slot_val_known(
		return_value,
		&slot->val,
		slot->has_clone_verdicts ? ucache_req_local_slot_verdicts(slot) : NULL,
		slot->no_aliases,
		NULL
	);

	slot->clone_depth--;

	if (slot->released_while_cloning) {
		ucache_req_local_slot_free(slot);

		return cloned;
	}

	if (!cloned) {
		ucache_release_req_local_slot(key);

		return false;
	}

	return true;
}

static bool ucache_find_slot_in_hdr_locked(
		ucache_hdr *hdr,
		zend_string *key,
		zend_ulong hash,
		ucache_find_slot_expiry_mode expiry_mode,
		uint32_t *slot_idx,
		bool *found)
{
	ucache_entry *entries, *entry;
	uint64_t now = 0;
	uint32_t i, first_tombstone = UINT32_MAX, step;

	ZEND_ASSERT(
		UC_G(lock_held) ||
		(UC_G(scalar_write_hdr) != NULL && expiry_mode == UCACHE_FIND_SLOT_IGNORE_EXPIRY)
	);

	if (hdr == NULL) {
		return false;
	}

	entries = ucache_entries_ptr(hdr);
	i = ucache_table_hash(hash) % hdr->capacity;

	for (step = 0; step < hdr->capacity; step++) {
		entry = &entries[i];

		if (ucache_entry_is_empty(entry)) {
			*slot_idx = first_tombstone != UINT32_MAX ? first_tombstone : i;
			*found = false;

			return true;
		}

		if (ucache_entry_is_tombstone(entry)) {
			if (first_tombstone == UINT32_MAX) {
				first_tombstone = i;
			}
		} else if (expiry_mode != UCACHE_FIND_SLOT_IGNORE_EXPIRY &&
			ucache_is_expired_now(hdr, entry, &now)
		) {
			if (expiry_mode == UCACHE_FIND_SLOT_DELETE_EXPIRED) {
				ucache_delete_entry_locked(hdr, entry, i);
			} else {
				ucache_note_expired_read();
			}

			if (first_tombstone == UINT32_MAX) {
				first_tombstone = i;
			}
		} else if (ucache_key_equals(hdr, entry, key, hash)) {
			*slot_idx = i;
			*found = true;

			return true;
		}

		++i;

		if (i == hdr->capacity) {
			i = 0;
		}
	}

	if (first_tombstone != UINT32_MAX) {
		*slot_idx = first_tombstone;
		*found = false;

		return true;
	}

	return false;
}

static bool ucache_find_slot_locked(
		zend_string *key,
		zend_ulong hash,
		ucache_find_slot_expiry_mode expiry_mode,
		ucache_hdr **hdr_ptr,
		uint32_t *slot_idx,
		bool *found)
{
	ucache_hdr *hdr = ucache_hdr_ptr();

	if (hdr == NULL || !ucache_hdr_adoptable_locked()) {
		return false;
	}

	if (hdr_ptr != NULL) {
		*hdr_ptr = hdr;
	}

	return ucache_find_slot_in_hdr_locked(
		hdr,
		key,
		hash,
		expiry_mode,
		slot_idx,
		found
	);
}

static bool ucache_find_slot_for_write_locked(
		zend_string *key,
		zend_ulong hash,
		ucache_hdr **hdr_ptr,
		uint32_t *slot_idx,
		bool *found)
{
	ucache_hdr *hdr = ucache_hdr_ptr();

	ZEND_ASSERT(UC_G(lock_held_is_write));

	if (!ucache_hdr_is_initialized_locked() ||
		!ucache_find_slot_in_hdr_locked(
			hdr,
			key,
			hash,
			UCACHE_FIND_SLOT_DELETE_EXPIRED,
			slot_idx,
			found
		)
	) {
		return false;
	}

	*hdr_ptr = hdr;

	if (!*found && hdr->count >= ucache_insert_cap(hdr->capacity)) {
		return false;
	}

	return true;
}

static bool ucache_rehash_locked(ucache_hdr *hdr)
{
	ucache_entry *entries, *snapshot, *entry, *target;
	uint32_t i, slot, step, *stamps, *stamp_snapshot;

	entries = ucache_entries_ptr(hdr);
	stamps = ucache_access_stamps_ptr(hdr);

	snapshot = UCACHE_DEBUG_FAULT("FAIL_REHASH_ALLOC")
		? NULL
		: malloc((size_t) hdr->capacity * (sizeof(*snapshot) + sizeof(*stamp_snapshot)))
	;
	if (snapshot == NULL) {
		return false;
	}

	stamp_snapshot = (uint32_t *) (snapshot + hdr->capacity);

	memcpy(snapshot, entries, (size_t) hdr->capacity * sizeof(*snapshot));

	for (i = 0; i < hdr->capacity; i++) {
		stamp_snapshot[i] = UCACHE_ATOMIC_LOAD_32_RELAXED(&stamps[i]);
	}

	memset(entries, 0, (size_t) hdr->capacity * sizeof(*entries));

	ucache_pool_idx_reset_locked(hdr);
	ucache_access_stamps_reset(hdr);

	hdr->count = 0;
	hdr->tombstone_count = 0;

	for (i = 0; i < hdr->capacity; i++) {
		entry = &snapshot[i];

		if (!ucache_entry_is_used(entry)) {
			continue;
		}

		slot = entry->hash % hdr->capacity;
		for (step = 0; step < hdr->capacity; step++) {
			target = &entries[slot];

			if (ucache_entry_is_empty(target)) {
				*target = *entry;

				ucache_pool_idx_link_locked(hdr, target, slot);
				ucache_entry_set_block_owner(target, slot);

				UCACHE_ATOMIC_STORE_32_RELAXED(&stamps[slot], stamp_snapshot[i]);

				hdr->count++;

				break;
			}

			++slot;

			if (slot == hdr->capacity) {
				slot = 0;
			}
		}
	}

	free(snapshot);

	ucache_bump_mutation_epoch_locked(hdr);

	return true;
}

static uint8_t *ucache_reserve_combined_val_key_locked(
		ucache_hdr *hdr,
		uint32_t reusable_offset,
		zend_string *key,
		size_t payload_size,
		uint32_t owner,
		uint32_t *val_offset)
{
	uint32_t base_offset;
	size_t key_size, total_size;

	key_size = ZSTR_LEN(key) + 1;
	total_size = payload_size + key_size;
	if (ucache_reuse_block_locked(hdr, reusable_offset, total_size, owner)) {
		base_offset = reusable_offset;
	} else {
		base_offset = ucache_alloc_locked(total_size, NULL, owner);
		if (base_offset == 0) {
			return NULL;
		}
	}

	*val_offset = base_offset;

	return ucache_ptr_in_hdr(hdr, base_offset);
}

static bool ucache_publish_combined_val_key_locked(
		ucache_hdr *hdr,
		uint32_t reusable_offset,
		zend_string *key,
		size_t payload_size,
		const void *src,
		uint32_t owner,
		uint32_t *val_offset)
{
	uint8_t *payload;

	ZEND_ASSERT(src != NULL);

	payload = ucache_reserve_combined_val_key_locked(
		hdr,
		reusable_offset,
		key,
		payload_size,
		owner,
		val_offset
	);
	if (payload == NULL) {
		return false;
	}

	memcpy(payload, src, payload_size);
	memcpy(payload + payload_size, ZSTR_VAL(key), ZSTR_LEN(key) + 1);

	return true;
}

static bool ucache_publish_prepared_sgraph_locked(
		const ucache_prepared_val *prepared,
		zval *val,
		uint8_t *payload)
{
	zval pinned_root;

	if (prepared->payload_src != NULL &&
		ucache_sgraph_publish_copied_payload_locked(
			payload,
			prepared->payload_src,
			prepared->payload_size,
			prepared->payload_used_size,
			prepared->has_verbatim_arr,
			prepared->fixup_offsets,
			prepared->fixup_count
		)
	) {
		return true;
	}

	if (prepared->owner_type == UCACHE_PREPARED_OWNER_STR) {
		ZVAL_STR(&pinned_root, prepared->owned_str);
		val = &pinned_root;
	}

	return ucache_build_sgraph_in_place(
		val,
		NULL,
		NULL,
		prepared->packed_vals_allowed,
		payload,
		prepared->payload_size,
		NULL,
		NULL,
		NULL,
		NULL
	);
}

static void ucache_rollback_partial_store_allocs_locked(
		uint32_t new_val_offset,
		uint32_t old_val_offset,
		uint32_t new_key_offset,
		uint32_t old_key_offset)
{
	if (new_val_offset != 0 && new_val_offset != old_val_offset) {
		ucache_free_locked(new_val_offset);
	}

	if (new_key_offset != 0 && new_key_offset != old_key_offset) {
		ucache_free_locked(new_key_offset);
	}
}

static void ucache_delete_overwritten_combined_entry_locked(
		ucache_hdr *hdr,
		ucache_entry *entry,
		uint32_t slot_idx)
{
	uint32_t block_offset = entry->val_offset;

	ucache_entry_set_val_type(entry, UCACHE_VAL_NULL);
	entry->val_offset = 0;

	ucache_delete_entry_locked(hdr, entry, slot_idx);

	ucache_free_locked(block_offset);
}

static ucache_store_attempt_result ucache_store_attempt_locked(
		zend_string *key,
		zval *val,
		const ucache_prepared_val *prepared,
		zend_long ttl,
		bool bulk,
		ucache_store_result *result,
		size_t key_size,
		ucache_store_retries *retries)
{
	const bool retry_after_mem_pressure = !bulk;
	ucache_hdr *hdr;
	ucache_entry *entries, *entry, replaced_snapshot;
	zend_long new_lval = 0;
	double new_dval = 0;
	uint64_t store_now;
	uint32_t expires_at, slot_idx, offset = 0, goffset = 0, reusable_offset,
			old_key_offset = 0, old_val_offset = 0,
			new_key_offset = 0, new_val_offset = 0, new_val_len = 0,
			combined_reuse_offset = 0
	;
	uint16_t old_flags = 0, new_flags = 0;
	uint8_t *combined_payload, old_val_type = UCACHE_VAL_NULL, new_val_type = prepared->val_type;
	size_t reclaim_size;
	bool found, old_combined, use_combined_publish, allocates_key, fits_in_empty_cache, capture_replaced = false,
			published = false, blocks_changed
	;

	if (!ucache_find_slot_for_write_locked(
			key,
			prepared->hash,
			&hdr,
			&slot_idx,
			&found
		)
	) {
		if (retry_after_mem_pressure &&
			ucache_reclaim_space_for_store_locked(
				true,
				0,
				retries
			)
		) {
			return UCACHE_STORE_ATTEMPT_RETRY;
		}

		ucache_hdr_ptr()->store_failure_count++;

		return UCACHE_STORE_ATTEMPT_FAILED;
	}

	entries = ucache_entries_ptr(hdr);
	entry = &entries[slot_idx];

	if (ttl == 0) {
		expires_at = 0;
		if (!found) {
			ucache_access_note_time(ucache_clock_now());
		}
	} else {
		store_now = ucache_clock_now();

		ucache_access_note_time(store_now);

		expires_at = ucache_expiry_deadline(hdr, store_now, ttl);
	}

	if (found) {
		capture_replaced = bulk;

		replaced_snapshot = *entry;

		old_key_offset = entry->key_offset;
		old_val_type = ucache_entry_val_type(entry);
		old_val_offset = ucache_entry_val_offset(entry);
		old_flags = entry->flags;
	}

	old_combined = found && (old_flags & UCACHE_ENTRY_FLAG_COMBINED_VAL_KEY) != 0;
	reusable_offset = 0;

	if (found && !capture_replaced && old_val_type != UCACHE_VAL_SGRAPH && !old_combined) {
		reusable_offset = old_val_offset;
	}

	new_key_offset = found && !capture_replaced ? old_key_offset : 0;

	use_combined_publish = ucache_val_uses_offset(prepared->val_type) &&
		(!found || old_combined)
	;

	if (old_combined && old_val_offset != 0 && !capture_replaced) {
		if (old_val_type == UCACHE_VAL_SGRAPH) {
			if (ucache_val_uses_offset(prepared->val_type) &&
				ucache_sgraph_quiesce_for_overwrite_locked(old_val_offset) &&
				(
					prepared->val_type != UCACHE_VAL_SGRAPH ||
					prepared->payload_src == NULL ||
					ucache_sgraph_copy_fits_buf(
						ucache_ptr_in_hdr(hdr, old_val_offset),
						prepared->payload_src,
						prepared->payload_size,
						prepared->payload_used_size
					)
				)
			) {
				combined_reuse_offset = old_val_offset;
			}
		} else if (hdr->count == 1) {
			combined_reuse_offset = old_val_offset;
		}
	}

	allocates_key = !use_combined_publish && (!found || old_combined || capture_replaced);
	if (allocates_key) {
		new_key_offset = ucache_alloc_locked(key_size, ZSTR_VAL(key), slot_idx);
		if (new_key_offset == 0) {
			fits_in_empty_cache = ucache_payload_can_fit_locked(key_size);

			goto bailout;
		}
	}

	switch (prepared->val_type) {
		case UCACHE_VAL_NULL:
		case UCACHE_VAL_TRUE:
		case UCACHE_VAL_FALSE:
			break;
		case UCACHE_VAL_LONG:
			new_lval = prepared->long_val;

			break;
		case UCACHE_VAL_DOUBLE:
			new_dval = prepared->double_val;

			break;
		case UCACHE_VAL_STR:
			if (use_combined_publish) {
				if (!ucache_publish_combined_val_key_locked(
						hdr,
						combined_reuse_offset,
						key,
						prepared->payload_size,
						prepared->payload_src,
						slot_idx,
						&new_val_offset
					)
				) {
					fits_in_empty_cache = ucache_payload_can_fit_locked(prepared->payload_size + key_size);

					goto bailout;
				}

				new_key_offset = ucache_combined_key_offset(new_val_offset, prepared->payload_size);
				new_flags = ucache_combined_key_flags(prepared->payload_size);
			} else {
				offset = ucache_write_payload_locked(
					hdr,
					reusable_offset,
					prepared->payload_size,
					prepared->payload_src,
					slot_idx
				);
				if (offset == 0) {
					fits_in_empty_cache = ucache_payload_can_fit_locked(prepared->payload_size);

					goto bailout;
				}

				new_val_offset = offset;
			}

			new_val_len = (uint32_t) prepared->payload_size - 1;

			break;
		case UCACHE_VAL_SGRAPH:
			if (use_combined_publish) {
				combined_payload = ucache_reserve_combined_val_key_locked(
					hdr,
					combined_reuse_offset,
					key,
					prepared->payload_size,
					slot_idx,
					&new_val_offset
				);

				goffset = new_val_offset;

				if (combined_payload != NULL) {
					zend_try {
						published = ucache_publish_prepared_sgraph_locked(
							prepared,
							val,
							combined_payload
						);
					} zend_catch {
						if (goffset != combined_reuse_offset) {
							ucache_free_locked(goffset);
						} else {
							ucache_delete_overwritten_combined_entry_locked(hdr, entry, slot_idx);
						}

						if (!UC_G(store_defer_unlock)) {
							ucache_unlock_if_held();
						}

						zend_bailout();
					} zend_end_try();

					if (published) {
						memcpy(combined_payload + prepared->payload_size, ZSTR_VAL(key), key_size);

						new_key_offset = ucache_combined_key_offset(goffset, prepared->payload_size);
						new_flags = ucache_combined_key_flags(prepared->payload_size);
						new_val_offset = goffset;
						new_val_len = (uint32_t) prepared->payload_size;

						break;
					}

					if (goffset != combined_reuse_offset) {
						ucache_free_locked(goffset);
					} else {
						ucache_delete_overwritten_combined_entry_locked(hdr, entry, slot_idx);

						old_val_offset = 0;
						old_key_offset = 0;
						found = false;
					}

					new_val_offset = 0;
					new_key_offset = found && !capture_replaced ? old_key_offset : 0;

					if (EG(exception)) {
						return UCACHE_STORE_ATTEMPT_FAILED;
					}
				} else if (retry_after_mem_pressure &&
					ucache_payload_can_fit_locked(prepared->payload_size + key_size) &&
					ucache_reclaim_space_for_store_locked(
						true,
						prepared->payload_size + key_size,
						retries
					)
				) {
					return UCACHE_STORE_ATTEMPT_RETRY;
				}
			} else {
				goffset = ucache_alloc_locked(prepared->payload_size, NULL, slot_idx);

				if (goffset != 0) {
					zend_try {
						published = ucache_publish_prepared_sgraph_locked(
							prepared,
							val,
							ucache_ptr_in_hdr(hdr, goffset)
						);
					} zend_catch {
						ZEND_ASSERT((new_flags & UCACHE_ENTRY_FLAG_COMBINED_VAL_KEY) == 0);

						ucache_rollback_partial_store_allocs_locked(
							goffset,
							old_val_offset,
							new_key_offset,
							old_key_offset
						);

						if (!UC_G(store_defer_unlock)) {
							ucache_unlock_if_held();
						}

						zend_bailout();
					} zend_end_try();

					if (published) {
						new_val_offset = goffset;
						new_val_len = (uint32_t) prepared->payload_size;

						break;
					}

					ucache_free_locked(goffset);

					if (EG(exception)) {
						ZEND_ASSERT((new_flags & UCACHE_ENTRY_FLAG_COMBINED_VAL_KEY) == 0);

						ucache_rollback_partial_store_allocs_locked(
							0,
							old_val_offset,
							new_key_offset,
							old_key_offset
						);

						return UCACHE_STORE_ATTEMPT_FAILED;
					}
				} else if (retry_after_mem_pressure &&
					ucache_payload_can_fit_locked(prepared->payload_size) &&
					ucache_reclaim_space_for_store_locked(
						true,
						prepared->payload_size,
						retries
					)
				) {
					return UCACHE_STORE_ATTEMPT_RETRY;
				}
			}

			fits_in_empty_cache = ucache_payload_can_fit_locked(prepared->payload_size);

			goto bailout;
		default:
			ZEND_UNREACHABLE();
	}

	if (!found && ucache_entry_is_tombstone(entry) && hdr->tombstone_count != 0) {
		hdr->tombstone_count--;
	}

	if (found && entry->expires_at != 0) {
		hdr->expiring_count--;
	}

	if (expires_at != 0) {
		hdr->expiring_count++;
	}

	ZEND_ASSERT(ZSTR_LEN(key) <= UCACHE_STORAGE_KEY_MAX);

	entry->hash = ucache_table_hash(prepared->hash);
	entry->key_offset = new_key_offset;
	entry->key_len = (uint16_t) ZSTR_LEN(key);
	entry->expires_at = expires_at;

	ucache_expiry_floor_lower_locked(hdr, expires_at);
	entry->flags = new_flags | (found
		? old_flags & UCACHE_ENTRY_POOL_BUCKET_MASK
		: ucache_pool_bucket_for_key(ZSTR_VAL(key), ZSTR_LEN(key))
			<< UCACHE_ENTRY_POOL_BUCKET_SHIFT
	);
	ucache_entry_set_val_type(entry, new_val_type);

	if (ucache_val_uses_offset(new_val_type)) {
		entry->val_offset = new_val_offset;
		entry->val_len = new_val_len;
	} else if (new_val_type == UCACHE_VAL_DOUBLE) {
		entry->double_val = new_dval;
	} else {
		ucache_entry_set_long_zero_filled(entry, new_lval);
	}

	if (found) {
		ucache_touch_entry_access(hdr, hdr->capacity, slot_idx);
	} else {
		UCACHE_ATOMIC_STORE_32_RELAXED(
			&ucache_access_stamps_ptr(hdr)[slot_idx], ucache_access_now()
		);
	}

	if (capture_replaced) {
		result->replaced_entry = replaced_snapshot;
	} else {
		if (found &&
			old_key_offset != 0 &&
			old_key_offset != new_key_offset &&
			!old_combined
		) {
			ucache_free_locked(old_key_offset);
		}

		if (found &&
			old_val_offset != 0 &&
			old_val_offset != new_val_offset
		) {
			ucache_release_val_storage_locked(old_val_type, old_val_offset);
		}
	}

	if (!found) {
		hdr->count++;
		ucache_pool_idx_link_locked(hdr, entry, slot_idx);
	}

	blocks_changed = !found || old_val_offset != new_val_offset ||
		old_combined != ((new_flags & UCACHE_ENTRY_FLAG_COMBINED_VAL_KEY) != 0) ||
		(!old_combined && old_key_offset != new_key_offset)
	;
	if (blocks_changed) {
		ucache_pool_bucket_changed_locked(hdr, ucache_entry_pool_bucket(entry));
	}

	ucache_bump_mutation_epoch_locked(hdr);

	entry->gen = hdr->mutation_epoch;

	if (found && !bulk && expires_at == 0 && replaced_snapshot.expires_at == 0 &&
		old_val_type <= UCACHE_VAL_DOUBLE && new_val_type <= UCACHE_VAL_DOUBLE
	) {
		ucache_scalar_write_enable_locked(hdr);
	}

	result->stored_gen = entry->gen;
	result->stored_slot = slot_idx;
	result->stored_expires_at = expires_at;
	result->stored_val_type = prepared->val_type;

	return UCACHE_STORE_ATTEMPT_STORED;

bailout:
	ZEND_ASSERT((new_flags & UCACHE_ENTRY_FLAG_COMBINED_VAL_KEY) == 0);

	ucache_rollback_partial_store_allocs_locked(
		new_val_offset,
		old_val_offset,
		new_key_offset,
		old_key_offset
	);

	ZEND_ASSERT(!retry_after_mem_pressure || !allocates_key || !ucache_val_uses_offset(prepared->val_type));

	reclaim_size = use_combined_publish ? prepared->payload_size + key_size : prepared->payload_size;
	if (reclaim_size == 0) {
		reclaim_size = key_size;
	}

	if (retry_after_mem_pressure &&
		ucache_reclaim_space_for_store_locked(
			fits_in_empty_cache,
			reclaim_size,
			retries
		)
	) {
		return UCACHE_STORE_ATTEMPT_RETRY;
	}

	hdr->store_failure_count++;

	return UCACHE_STORE_ATTEMPT_FAILED;
}

static bool ucache_store_prepared_locked_impl(
		zend_string *key,
		zval *val,
		const ucache_prepared_val *prepared,
		zend_long ttl,
		bool bulk,
		ucache_store_result *result)
{
	ucache_store_attempt_result attempt_result;
	ucache_store_retries retries = {0};
	size_t key_size, min_payload_size;

	ZVAL_DEREF(val);

	memset(result, 0, sizeof(*result));

	key_size = ZSTR_LEN(key) + 1;
	min_payload_size = ucache_val_uses_offset(prepared->val_type)
		? prepared->payload_size
		: 0
	;

	if (min_payload_size > SIZE_MAX - key_size ||
		!ucache_payload_can_fit_locked(min_payload_size + key_size)
	) {
		ucache_hdr_ptr()->store_failure_count++;

		return false;
	}

	ucache_maybe_rehash_locked();

	for (;;) {
		attempt_result = ucache_store_attempt_locked(
			key,
			val,
			prepared,
			ttl,
			bulk,
			result,
			key_size,
			&retries
		);

		if (attempt_result == UCACHE_STORE_ATTEMPT_RETRY) {
			continue;
		}

		return attempt_result == UCACHE_STORE_ATTEMPT_STORED;
	}
}

static bool ucache_prepare_direct_val(
		zval *val,
		ucache_prepared_val *prepared)
{
	switch (Z_TYPE_P(val)) {
		case IS_NULL:
			prepared->val_type = UCACHE_VAL_NULL;

			return true;
		case IS_TRUE:
			prepared->val_type = UCACHE_VAL_TRUE;

			return true;
		case IS_FALSE:
			prepared->val_type = UCACHE_VAL_FALSE;

			return true;
		case IS_LONG:
			prepared->val_type = UCACHE_VAL_LONG;
			prepared->long_val = Z_LVAL_P(val);

			return true;
		case IS_DOUBLE:
			prepared->val_type = UCACHE_VAL_DOUBLE;
			prepared->double_val = Z_DVAL_P(val);

			return true;
		case IS_STRING:
			if (Z_STRLEN_P(val) < UCACHE_DIRECT_STR_MIN_LEN) {
				prepared->val_type = UCACHE_VAL_STR;
				prepared->payload_size = Z_STRLEN_P(val) + 1;
				prepared->payload_used_size = prepared->payload_size;
				prepared->payload_src = (const uint8_t *) Z_STRVAL_P(val);

				return true;
			}

			return false;
		default:
			return false;
	}
}

static void ucache_throw_if_pass_overflowed_stack(void)
{
	if (!UC_G(stack_overflowed)) {
		return;
	}

	UC_G(stack_overflowed) = false;

	if (!EG(exception)) {
		zend_type_error(UCACHE_MSG_NESTED_TOO_DEEPLY);
	}
}

static bool ucache_build_prepared_sgraph(
		zval *val,
		HashTable *verbatim_verdicts,
		size_t glen,
		ucache_prepared_val *prepared)
{
	return ucache_build_sgraph_in_place(
		val,
		prepared->state_memo,
		zend_hash_num_elements(prepared->state_memo) == 0 ? verbatim_verdicts : NULL,
		prepared->packed_vals_allowed,
		prepared->owned_buf,
		glen,
		&prepared->payload_used_size,
		&prepared->has_verbatim_arr,
		&prepared->fixup_offsets,
		&prepared->fixup_count
	);
}

static bool ucache_rebuild_prepared_sgraph_after_hooks(
		zval *val,
		ucache_prepared_val *prepared,
		uint32_t calc_memo_count)
{
	size_t glen;

	if (EG(exception) ||
		UC_G(stack_overflowed) ||
		calc_memo_count == 0 ||
		zend_hash_num_elements(prepared->state_memo) != calc_memo_count ||
		!ucache_calc_sgraph_size(
			val,
			prepared->state_memo,
			NULL,
			&glen,
			&prepared->packed_vals_allowed
		) ||
		glen > UINT32_MAX
	) {
		return false;
	}

	prepared->owned_buf = erealloc(prepared->owned_buf, glen);
	prepared->payload_size = glen;

	return ucache_build_prepared_sgraph(val, NULL, glen, prepared);
}

static bool ucache_prepare_sgraph_val(
		zval *val,
		HashTable *verbatim_verdicts,
		size_t verbatim_glen,
		ucache_prepared_val *prepared)
{
	size_t glen = 0;
	uint32_t calc_memo_count;
	bool build_at_publish = Z_TYPE_P(val) == IS_STRING;

	if (!build_at_publish && prepared->state_memo == NULL) {
		prepared->state_memo = emalloc(sizeof(HashTable));

		zend_hash_init(prepared->state_memo, 8, NULL, ZVAL_PTR_DTOR, 0);
	}

	UC_G(stack_overflowed) = false;

	if (verbatim_glen != 0) {
		glen = verbatim_glen;
	} else if (!ucache_calc_sgraph_size(
			val,
			prepared->state_memo,
			verbatim_verdicts,
			&glen,
			&prepared->packed_vals_allowed
		)
	) {
		ucache_throw_if_pass_overflowed_stack();

		return false;
	}

	if (glen > UINT32_MAX) {
		return false;
	}

	prepared->val_type = UCACHE_VAL_SGRAPH;
	prepared->payload_size = glen;

	if (build_at_publish) {
		prepared->owner_type = UCACHE_PREPARED_OWNER_STR;
		prepared->owned_str = zend_string_copy(Z_STR_P(val));

		return true;
	}

	prepared->owner_type = UCACHE_PREPARED_OWNER_BUF;
	prepared->owned_buf = emalloc(glen);

	calc_memo_count = zend_hash_num_elements(prepared->state_memo);

	if (!ucache_build_prepared_sgraph(val, verbatim_verdicts, glen, prepared) &&
		!ucache_rebuild_prepared_sgraph_after_hooks(val, prepared, calc_memo_count)
	) {
		ucache_throw_if_pass_overflowed_stack();

		return false;
	}

	prepared->payload_src = prepared->owned_buf;
	if (prepared->packed_vals_allowed) {
		if (UNEXPECTED(prepared->payload_used_size > prepared->payload_size - UCACHE_SGRAPH_ALIGNMENT_SLACK)) {
			return false;
		}

		prepared->payload_size = prepared->payload_used_size + UCACHE_SGRAPH_ALIGNMENT_SLACK;
	}

	return true;
}

static uint32_t ucache_fetch_finish_flags(uint32_t gflags, bool owned_copy)
{
	uint32_t flags = 0;

	if ((UC_G(persistent_exec) || owned_copy) &&
		!(gflags & (
			UCACHE_SGRAPH_FLAG_HAS_OBJ |
			UCACHE_SGRAPH_FLAG_HAS_SHARED_IDENTITY
		))
	) {
		return UCACHE_FETCH_FINISH_RECORD_COPY;
	}

	if (UCACHE_OPTIMISTIC_ENABLED &&
		(
			(gflags & UCACHE_SGRAPH_FLAG_PREFERS_PROTO) ||
			(
				UC_G(persistent_exec) &&
				!(gflags & UCACHE_SGRAPH_FLAG_HAS_OBJ)
			)
		)
	) {
		flags |= UCACHE_FETCH_FINISH_USE_REQ_LOCAL_SLOT;
	}

	if (!(gflags & UCACHE_SGRAPH_FLAG_HAS_SHARED_IDENTITY)) {
		flags |= UCACHE_FETCH_FINISH_NO_ALIASES;
	}

	return flags;
}

static PHP_UCACHE_HOT void ucache_fetch_finish_impl(
		ucache_key_record *record,
		uint64_t gen,
		zval *return_value,
		uint32_t flags,
		uint32_t val_len)
{
	ucache_req_local_slot *slot;

	if (flags & UCACHE_FETCH_FINISH_RECORD_COPY) {
		if (record->state == UCACHE_RECORD_HIT &&
			record->val_kind == UCACHE_RECORD_VAL_NONE
		) {
			ucache_key_record_cache_copy(record, return_value, val_len);
		}

		return;
	}

	if (!(flags & UCACHE_FETCH_FINISH_USE_REQ_LOCAL_SLOT)) {
		return;
	}

	slot = ucache_find_req_local_slot(record->storage_key, gen);
	if (slot == NULL) {
		if (UC_G(req_local_slot_table) != NULL &&
			zend_hash_num_elements(UC_G(req_local_slot_table)) >= UCACHE_MAX_REQ_LOCAL_SLOTS
		) {
			ucache_req_local_slots_drop_oldest_half();
		}

		ucache_mark_req_local_slot(record->storage_key, gen);
	} else if (!slot->has_val && !slot->proto_rejected) {
		if (UNEXPECTED(flags & UCACHE_FETCH_FINISH_RESTORE_HOOKS_RAN)) {
			slot->proto_rejected = true;

			return;
		}

		ucache_store_req_local_slot(
			record->storage_key,
			gen,
			return_value,
			(flags & UCACHE_FETCH_FINISH_NO_ALIASES) != 0,
			val_len
		);
	}
}

static bool ucache_fetch_emit_val_locked(
		const ucache_hdr *hdr,
		ucache_entry *entry,
		uint64_t epoch,
		zval *return_value,
		ucache_fetch_pending_seed *pending_seed,
		bool *lock_held)
{
	uint64_t gen = entry->gen;
	uint32_t gflags, hook_calls_before, val_len = entry->val_len;
	bool private_copy = false;

	pending_seed->should_seed = false;
	pending_seed->mutation_epoch = epoch;
	pending_seed->gen = gen;

	if (ucache_scalar_to_zval(
			ucache_entry_val_type(entry),
			&entry->long_val,
			&entry->double_val,
			return_value
		)
	) {
		return true;
	}

	switch (ucache_entry_val_type(entry)) {
		case UCACHE_VAL_STR:
			UCACHE_TRY_UNLOCK_ON_BAILOUT(
				ZVAL_STRINGL(
					return_value,
					(const char *) ucache_ptr_in_hdr(
						hdr,
						entry->val_offset
					),
					val_len
				);
			);

			pending_seed->flags = UCACHE_FETCH_FINISH_RECORD_COPY;
			pending_seed->val_len = val_len;
			pending_seed->should_seed = true;

			return true;
		case UCACHE_VAL_SGRAPH:
			gflags = ucache_sgraph_payload_flags(entry->val_offset);
			hook_calls_before = UC_G(restore_hook_calls);

			if (!ucache_materialize_sgraph_locked(
					hdr,
					entry->val_offset,
					val_len,
					return_value,
					lock_held,
					&private_copy
				)
			) {
				return false;
			}

			pending_seed->flags = ucache_fetch_finish_flags_after_decode(
				ucache_fetch_finish_flags(gflags, private_copy),
				hook_calls_before
			);
			pending_seed->val_len = val_len;
			pending_seed->should_seed = true;

			return true;
		default:
			return false;
	}
}

static ucache_fetch_locate_result ucache_fetch_probe_key_record_locked(
		ucache_hdr *hdr,
		ucache_key_record *record,
		ucache_entry *entries,
		uint64_t epoch,
		zval *return_value,
		uint32_t *slot_idx,
		zval *detached)
{
	ucache_entry *entry;
	uint64_t now = 0;
	bool cur = record->mutation_epoch == epoch;

	if (record->state == UCACHE_RECORD_EMPTY) {
		return UCACHE_FETCH_LOCATE_UNCACHED;
	}

	if (cur) {
		if (record->state == UCACHE_RECORD_MISS) {
			return UCACHE_FETCH_LOCATE_MISS;
		}

		if (record->val_kind != UCACHE_RECORD_VAL_NONE &&
			ucache_key_record_val_usable(hdr, record)
		) {
			ZVAL_COPY(return_value, &record->val);

			ucache_touch_cached_entry_access(hdr, record->slot_idx, &record->access_touches);

			return UCACHE_FETCH_LOCATE_VAL_HIT;
		}
	} else if (record->state != UCACHE_RECORD_HIT) {
		ucache_key_record_reset_impl(record, detached);

		return UCACHE_FETCH_LOCATE_UNCACHED;
	}

	if (record->slot_idx >= hdr->capacity ||
		(
			!cur &&
			!ucache_key_equals(
				hdr,
				&entries[record->slot_idx],
				record->storage_key,
				ZSTR_H(record->storage_key)
			)
		)
	) {
		ucache_key_record_reset_impl(record, detached);

		return UCACHE_FETCH_LOCATE_UNCACHED;
	}

	entry = &entries[record->slot_idx];
	if (ucache_is_expired_now(hdr, entry, &now)) {
		ucache_note_expired_read();

		ucache_key_record_mark_miss(record, epoch, detached);

		return UCACHE_FETCH_LOCATE_MISS;
	}

	*slot_idx = record->slot_idx;

	if (!cur) {
		ucache_key_record_mark_hit(record, epoch, *slot_idx, entry, detached);
	}

	return UCACHE_FETCH_LOCATE_SLOT;
}

static ucache_fetch_locate_result ucache_fetch_probe_entry_table_locked(
		ucache_hdr *hdr,
		ucache_key_record *record,
		ucache_entry *entries,
		uint64_t epoch,
		uint32_t *slot_idx,
		zval *detached)
{
	bool found;

	if (!ucache_find_slot_in_hdr_locked(
			hdr,
			record->storage_key,
			ZSTR_H(record->storage_key),
			UCACHE_FIND_SLOT_SKIP_EXPIRED,
			slot_idx,
			&found
		) ||
		!found
	) {
		ucache_key_record_mark_miss(record, epoch, detached);

		return UCACHE_FETCH_LOCATE_MISS;
	}

	ucache_key_record_mark_hit(record, epoch, *slot_idx, &entries[*slot_idx], detached);

	return UCACHE_FETCH_LOCATE_SLOT;
}

static bool ucache_atomic_insert_missing_locked(
		zend_string *key,
		zend_long step,
		zend_long ttl,
		bool decrement,
		ucache_atomic_update_result *result)
{
	ucache_prepared_val prepared;
	ucache_store_result store_result;
	zend_long updated;
	zval init_val = {0};
	bool is_overflow, stored;

	is_overflow = decrement
		? ucache_long_sub_overflow(0, step, &updated)
		: ucache_long_add_overflow(0, step, &updated)
	;
	if (is_overflow) {
		result->is_overflow = true;

		return false;
	}

	ZVAL_LONG(&init_val, updated);

	if (!ucache_prepare_val(key, &init_val, &prepared)) {
		ucache_destroy_prepared_val(&prepared);

		return false;
	}

	UCACHE_TRY_UNLOCK_ON_BAILOUT(
		stored = ucache_store_prepared_locked_impl(key, &init_val, &prepared, ttl, false, &store_result);
	);

	ucache_destroy_prepared_val(&prepared);

	if (stored) {
		result->new_val = Z_LVAL(init_val);

		result->stored_gen = store_result.stored_gen;
		result->stored_slot = store_result.stored_slot;
		result->stored_expires_at = store_result.stored_expires_at;

		return true;
	}

	return false;
}

static zend_never_inline ucache_optimistic_result ucache_optimistic_locate_fallback(
		ucache_hdr *hdr,
		uint32_t capacity,
		ucache_key_record *record,
		uint64_t seq,
		uint64_t epoch,
		bool stamp_access,
		ucache_entry *snapshot)
{
	zend_string *key = record->storage_key;
	uint32_t slot_idx = 0;
	bool cur = record->mutation_epoch == epoch, found = true;

	if (record->state == UCACHE_RECORD_HIT &&
		ucache_optimistic_key_record_slot(hdr, capacity, record, cur, snapshot)
	) {
		slot_idx = record->slot_idx;
	} else {
		cur = false;

		if (!ucache_optimistic_probe(
				hdr,
				capacity,
				key,
				ZSTR_H(key),
				ucache_entries_ptr(hdr),
				snapshot,
				&slot_idx,
				&found
			)
		) {
			return UCACHE_OPTIMISTIC_FALLBACK;
		}
	}

	if (ucache_read_seq(hdr, key) != seq) {
		return UCACHE_OPTIMISTIC_FALLBACK;
	}

	if (!found) {
		ucache_key_record_mark_miss(record, epoch, NULL);

		return UCACHE_OPTIMISTIC_MISS;
	}

	if (cur) {
		if (stamp_access) {
			ucache_touch_cached_entry_access(hdr, slot_idx, &record->access_touches);
		}

		return UCACHE_OPTIMISTIC_FOUND;
	}

	if (stamp_access) {
		ucache_touch_entry_access(hdr, capacity, slot_idx);
	}

	ucache_key_record_mark_hit(record, epoch, slot_idx, snapshot, NULL);
	if (!stamp_access) {
		record->access_touches = UCACHE_ACCESS_SAMPLE_INTERVAL - 1;
	}

	return UCACHE_OPTIMISTIC_FOUND;
}

static ucache_optimistic_result ucache_optimistic_emit_str(
		ucache_hdr *hdr,
		ucache_key_record *record,
		uint64_t seq,
		const ucache_entry *snapshot,
		zval *return_value)
{
	zend_string *key = record->storage_key, *str;

	if (!ucache_payload_in_bounds(hdr, snapshot->val_offset, snapshot->val_len)) {
		return UCACHE_OPTIMISTIC_FALLBACK;
	}

	str = zend_string_init(
		(const char *) ucache_ptr_in_hdr(hdr, snapshot->val_offset),
		snapshot->val_len,
		0
	);

	if (ucache_read_seq(hdr, key) != seq) {
		zend_string_release(str);

		return UCACHE_OPTIMISTIC_FALLBACK;
	}

	ZVAL_STR(return_value, str);

	ucache_key_record_cache_copy(record, return_value, snapshot->val_len);

	return UCACHE_OPTIMISTIC_FOUND;
}

static zend_never_inline ucache_optimistic_result ucache_optimistic_decode_owned_graph(
		ucache_hdr *hdr,
		ucache_key_record *record,
		uint64_t seq,
		const ucache_entry *snapshot,
		zval *return_value,
		uint32_t gflags)
{
	zend_string *key = record->storage_key;
	ucache_op_lease *lease = ucache_op_lease_reserve(snapshot->val_offset);
	uint32_t reader_slot, hook_calls_before;
	bool decoded, resource_limited;

	if (!lease->acquired) {
		if (!ucache_optimistic_reader_begin(hdr, &reader_slot)) {
			ucache_op_lease_end(lease);

			return UCACHE_OPTIMISTIC_FALLBACK;
		}

		if (ucache_read_seq(hdr, key) != seq) {
			ucache_optimistic_reader_end(hdr, reader_slot);
			ucache_op_lease_end(lease);

			return UCACHE_OPTIMISTIC_FALLBACK;
		}

		decoded = ucache_op_lease_acquire(lease, &resource_limited);

		ucache_optimistic_reader_end(hdr, reader_slot);

		if (!decoded) {
			ucache_op_lease_end(lease);

			return UCACHE_OPTIMISTIC_FALLBACK;
		}
	}

	ZVAL_UNDEF(return_value);

	hook_calls_before = UC_G(restore_hook_calls);
	decoded = ucache_sgraph_decode(
		ucache_ptr_in_hdr(hdr, snapshot->val_offset),
		snapshot->val_len,
		return_value
	);
	if (!decoded && Z_TYPE_P(return_value) != IS_UNDEF) {
		zval_ptr_dtor(return_value);

		ZVAL_UNDEF(return_value);
	}

	ucache_op_lease_end(lease);

	if (!decoded) {
		return ucache_optimistic_unrestorable(snapshot);
	}

	ucache_fetch_finish_impl(
		record,
		snapshot->gen,
		return_value,
		ucache_fetch_finish_flags_after_decode(ucache_fetch_finish_flags(gflags, true), hook_calls_before),
		snapshot->val_len
	);

	return UCACHE_OPTIMISTIC_FOUND;
}

static ucache_optimistic_result ucache_optimistic_emit_sgraph(
		ucache_hdr *hdr,
		ucache_key_record *record,
		uint64_t seq,
		const ucache_entry *snapshot,
		zval *return_value,
		bool allow_decode)
{
	zend_string *key = record->storage_key;
	uint32_t reader_slot = 0, gflags, ref_idx, hook_calls_before;

	if (!ucache_payload_in_bounds(
			hdr,
			snapshot->val_offset,
			UCACHE_SGRAPH_HDR_SIZE(0) + UCACHE_SGRAPH_ALIGNMENT_SLACK
		) ||
		!ucache_payload_in_bounds(
			hdr,
			snapshot->val_offset,
			snapshot->val_len
		)
	) {
		return UCACHE_OPTIMISTIC_FALLBACK;
	}

	gflags = ucache_sgraph_payload_flags(snapshot->val_offset);

	if (ucache_read_seq(hdr, key) != seq) {
		return UCACHE_OPTIMISTIC_FALLBACK;
	}

	if (((gflags & UCACHE_SGRAPH_FLAG_PREFERS_PROTO) || UC_G(persistent_exec)) &&
		ucache_fetch_req_local_slot(key, snapshot->gen, return_value)
	) {
		return UCACHE_OPTIMISTIC_FOUND;
	}

	if (!allow_decode && (gflags & UCACHE_SGRAPH_FLAG_HAS_OBJ)) {
		return UCACHE_OPTIMISTIC_FALLBACK;
	}

	if (UNEXPECTED(UC_G(persistent_exec))) {
		return ucache_optimistic_decode_owned_graph(
			hdr,
			record,
			seq,
			snapshot,
			return_value,
			gflags
		);
	}

	ref_idx = ucache_req_sgraph_ref_idx(snapshot->val_offset);
	if (ref_idx == UINT32_MAX) {
		if (!ucache_req_pin_fits_budget(hdr, snapshot->val_len)) {
			return UCACHE_OPTIMISTIC_FALLBACK;
		}

		ucache_sgraph_ref_reserve();

		if (!ucache_optimistic_reader_begin(hdr, &reader_slot)) {
			return UCACHE_OPTIMISTIC_FALLBACK;
		}

		if (ucache_read_seq(hdr, key) != seq) {
			ucache_optimistic_reader_end(hdr, reader_slot);

			return UCACHE_OPTIMISTIC_FALLBACK;
		}

		if (!ucache_sgraph_acquire_ref(snapshot->val_offset, NULL)) {
			ucache_optimistic_reader_end(hdr, reader_slot);

			return UCACHE_OPTIMISTIC_FALLBACK;
		}

		ref_idx = ucache_register_sgraph_ref(snapshot->val_offset, snapshot->val_len);

		ucache_optimistic_reader_end(hdr, reader_slot);
	}

	ZVAL_UNDEF(return_value);

	hook_calls_before = UC_G(restore_hook_calls);

	if (!ucache_sgraph_decode(
			ucache_ptr_in_hdr(hdr, snapshot->val_offset),
			snapshot->val_len,
			return_value
		)
	) {
		if (Z_TYPE_P(return_value) != IS_UNDEF) {
			zval_ptr_dtor(return_value);

			ZVAL_UNDEF(return_value);
		}

		return ucache_optimistic_unrestorable(snapshot);
	}

	if (gflags & UCACHE_SGRAPH_FLAG_PREFERS_PROTO) {
		ucache_fetch_finish_impl(
			record,
			snapshot->gen,
			return_value,
			ucache_fetch_finish_flags_after_decode(ucache_fetch_finish_flags(gflags, false), hook_calls_before),
			snapshot->val_len
		);
	} else if (!Z_REFCOUNTED_P(return_value)) {
		ucache_key_record_cache_pinned(record, return_value, snapshot->val_offset, ref_idx);
	} else if (!(gflags & (
			UCACHE_SGRAPH_FLAG_HAS_OBJ |
			UCACHE_SGRAPH_FLAG_HAS_SHARED_IDENTITY
		))
	) {
		ucache_key_record_cache_pinned_copy(record, return_value, snapshot->val_offset, snapshot->val_len);
	}

	return UCACHE_OPTIMISTIC_FOUND;
}

static zend_never_inline ucache_optimistic_result ucache_exists_optimistic_entry(
		ucache_hdr *hdr,
		ucache_key_record *record,
		uint64_t seq,
		uint64_t epoch)
{
	ucache_entry snapshot;
	uint32_t capacity;

	capacity = ucache_optimistic_capacity(hdr);
	if (capacity == 0) {
		return UCACHE_OPTIMISTIC_FALLBACK;
	}

	return ucache_optimistic_locate(hdr, capacity, record, seq, epoch, false, &snapshot);
}

static zend_never_inline ucache_optimistic_result ucache_fetch_optimistic_entry(
		ucache_hdr *hdr,
		ucache_key_record *record,
		uint64_t seq,
		uint64_t epoch,
		zval *return_value,
		bool allow_decode)
{
	ucache_entry snapshot;
	ucache_optimistic_result result;
	uint32_t capacity;

	capacity = ucache_optimistic_capacity(hdr);
	if (capacity == 0) {
		return UCACHE_OPTIMISTIC_FALLBACK;
	}

	result = ucache_optimistic_locate(hdr, capacity, record, seq, epoch, true, &snapshot);
	if (result != UCACHE_OPTIMISTIC_FOUND) {
		return result;
	}

	if (record->val_kind != UCACHE_RECORD_VAL_NONE) {
		if (ucache_key_record_val_retained(record)) {
			ZVAL_COPY(return_value, &record->val);

			return UCACHE_OPTIMISTIC_FOUND;
		}

		ucache_key_record_release_val(record, NULL);
	}

	if (ucache_scalar_to_zval(
			ucache_entry_val_type(&snapshot),
			&snapshot.long_val,
			&snapshot.double_val,
			return_value
		)
	) {
		return UCACHE_OPTIMISTIC_FOUND;
	}

	switch (ucache_entry_val_type(&snapshot)) {
		case UCACHE_VAL_STR:
			return ucache_optimistic_emit_str(hdr, record, seq, &snapshot, return_value);
		case UCACHE_VAL_SGRAPH:
			return ucache_optimistic_emit_sgraph(hdr, record, seq, &snapshot, return_value, allow_decode);
		default:
			return UCACHE_OPTIMISTIC_FALLBACK;
	}
}

ucache_req_local_slot *ucache_find_req_local_slot(
		zend_string *key,
		uint64_t gen)
{
	ucache_req_local_slot *slot;
	HashTable **slots_ptr = &UC_G(req_local_slot_table);

	ucache_req_local_slots_check_fork();

	if (*slots_ptr == NULL) {
		return NULL;
	}

	slot = zend_hash_find_ptr(*slots_ptr, key);
	if (slot == NULL) {
		return NULL;
	}

	if (slot->gen != gen || slot->ctx != ucache_active_ctx()) {
		ucache_release_req_local_slot(key);

		return NULL;
	}

	return slot;
}

void ucache_retry_op_lease_releases(void)
{
	ucache_op_lease *lease, *next;

	for (lease = UC_G(op_leases); lease != NULL; lease = next) {
		next = lease->next;

		if (lease->users == 0) {
			ucache_op_lease_release(lease);
		}
	}
}

void ucache_release_op_leases(void)
{
	ucache_op_lease *lease, *next;
	ucache_ctx *prev;
	uint64_t pid = ucache_cached_pid();

	lease = UC_G(op_leases);

	UC_G(op_leases) = NULL;

	while (lease != NULL) {
		next = lease->next;
		if (lease->acquired && lease->owner_pid == pid) {
			prev = ucache_activate_ctx(lease->ctx);
			(void) ucache_sgraph_release_op_ref(lease->payload_offset);
			ucache_restore_ctx(prev);
		}

		if (lease != &UC_G(op_lease_inline)) {
			efree(lease);
		}

		lease = next;
	}

	lease = UC_G(op_lease_free);

	UC_G(op_lease_free) = NULL;

	while (lease != NULL) {
		next = lease->next;

		efree(lease);

		lease = next;
	}

	memset(&UC_G(op_lease_inline), 0, sizeof(UC_G(op_lease_inline)));
}

bool ucache_prepare_val(
		zend_string *key,
		zval *val,
		ucache_prepared_val *prepared)
{
	HashTable verbatim_verdicts;
	size_t verbatim_glen = 0;
	bool has_verbatim_verdicts = false, result;

	ucache_init_prepared_val(prepared);

	ZVAL_DEREF(val);

	prepared->hash = zend_string_hash_val(key);

	switch (Z_TYPE_P(val)) {
		case IS_RESOURCE:
			return false;
		case IS_OBJECT:
			if (Z_OBJCE_P(val) == zend_ce_closure) {
				return false;
			}

			break;
		case IS_ARRAY:
			if (EXPECTED(!EG(exception))) {
				zend_hash_init(&verbatim_verdicts, 8, NULL, NULL, 0);

				has_verbatim_verdicts = true;

				ucache_sgraph_calc_verbatim_root(val, &verbatim_verdicts, &verbatim_glen);
			}

			break;
		default:
			break;
	}

	if (ucache_prepare_direct_val(val, prepared)) {
		result = true;

		goto done;
	}

	if (!has_verbatim_verdicts) {
		zend_hash_init(&verbatim_verdicts, 8, NULL, NULL, 0);

		has_verbatim_verdicts = true;
	}

	result = ucache_prepare_sgraph_val(val, &verbatim_verdicts, verbatim_glen, prepared);

done:
	if (has_verbatim_verdicts) {
		zend_hash_destroy(&verbatim_verdicts);
	}

	return result;
}

void ucache_destroy_prepared_val(ucache_prepared_val *prepared)
{
	zend_ulong memo_key;
	zval *memo_val;

	if (prepared->owner_type == UCACHE_PREPARED_OWNER_BUF) {
		efree(prepared->owned_buf);
	} else if (prepared->owner_type == UCACHE_PREPARED_OWNER_STR) {
		zend_string_release(prepared->owned_str);
	}

	if (prepared->fixup_offsets != NULL) {
		efree(prepared->fixup_offsets);
	}

	if (prepared->state_memo != NULL) {
		ZEND_HASH_FOREACH_NUM_KEY_VAL(prepared->state_memo, memo_key, memo_val) {
			if (Z_TYPE_P(memo_val) == IS_ARRAY || Z_TYPE_P(memo_val) == IS_STRING) {
				OBJ_RELEASE((zend_object *) (uintptr_t) memo_key);
			}
		} ZEND_HASH_FOREACH_END();

		zend_hash_destroy(prepared->state_memo);

		efree(prepared->state_memo);
	}
}

bool ucache_store_prepared_locked(
		zend_string *key,
		zval *val,
		const ucache_prepared_val *prepared,
		zend_long ttl,
		bool bulk,
		ucache_store_result *result)
{
	ucache_maybe_expunge_expired_locked();

	return ucache_store_prepared_locked_impl(key, val, prepared, ttl, bulk, result);
}

void ucache_key_record_reset(ucache_key_record *record, zval *detached)
{
	ucache_key_record_reset_impl(record, detached);
}

void ucache_key_record_stored(
		ucache_key_record *record,
		const ucache_store_result *result,
		zval *val)
{
	ucache_key_record_mark_stored(record, result->stored_gen, result->stored_slot, result->stored_expires_at);

	ZVAL_DEREF(val);

	if (result->stored_val_type < UCACHE_VAL_STR) {
		ucache_key_record_cache_copy(record, val, 0);
	} else if (result->stored_val_type == UCACHE_VAL_STR &&
		Z_REFCOUNTED_P(val) &&
		Z_STRLEN_P(val) >= UCACHE_RECORD_STORE_STR_MIN_LEN
	) {
		ucache_key_record_cache_copy(record, val, Z_STRLEN_P(val));
	}
}

void ucache_key_record_deleted(ucache_key_record *record, uint64_t epoch)
{
	if (epoch == 0) {
		ucache_key_record_reset_impl(record, NULL);

		return;
	}

	ucache_key_record_mark_miss(record, epoch, NULL);
}

void ucache_key_record_atomic_updated(
		ucache_key_record *record,
		const ucache_atomic_update_result *result)
{
	if (result->stored_gen == 0) {
		return;
	}

	ucache_key_record_mark_stored(record, result->stored_gen, result->stored_slot, result->stored_expires_at);

	if (record->val_kind != UCACHE_RECORD_VAL_NONE) {
		return;
	}

	ZVAL_LONG(&record->val, result->new_val);

	record->val_kind = UCACHE_RECORD_VAL_COPY;
}

void ucache_fetch_finish(
		ucache_key_record *record,
		ucache_fetch_pending_seed *pending_seed,
		zval *return_value)
{
	if (return_value != NULL &&
		pending_seed->should_seed &&
		(
			!(pending_seed->flags & UCACHE_FETCH_FINISH_RECORD_COPY) ||
			record->mutation_epoch == pending_seed->mutation_epoch
		)
	) {
		ucache_fetch_finish_impl(
			record,
			pending_seed->gen,
			return_value,
			pending_seed->flags,
			pending_seed->val_len
		);
	}

	zval_ptr_dtor(&pending_seed->detached_val);
}

bool ucache_fetch_locked(
		ucache_key_record *record,
		zval *return_value,
		bool *found,
		ucache_fetch_pending_seed *pending_seed,
		bool *lock_held)
{
	ucache_hdr *hdr;
	ucache_entry *entries, *entry;
	ucache_fetch_locate_result locate;
	uint64_t epoch;
	uint32_t slot_idx = 0;

	ZEND_ASSERT(*lock_held && UC_G(lock_held));

	*found = false;

	pending_seed->should_seed = false;
	pending_seed->gen = 0;

	ZVAL_UNDEF(&pending_seed->detached_val);

	hdr = ucache_hdr_ptr();
	if (hdr == NULL || !ucache_hdr_adoptable_locked()) {
		return false;
	}

	entries = ucache_entries_ptr(hdr);
	epoch = hdr->mutation_epoch;

	locate = ucache_fetch_probe_key_record_locked(
		hdr,
		record,
		entries,
		epoch,
		return_value,
		&slot_idx,
		&pending_seed->detached_val
	);
	if (locate == UCACHE_FETCH_LOCATE_UNCACHED) {
		locate = ucache_fetch_probe_entry_table_locked(
			hdr,
			record,
			entries,
			epoch,
			&slot_idx,
			&pending_seed->detached_val
		);
	}

	if (locate == UCACHE_FETCH_LOCATE_VAL_HIT) {
		*found = true;

		return true;
	}

	if (locate == UCACHE_FETCH_LOCATE_MISS) {
		return false;
	}

	entry = &entries[slot_idx];

	ucache_touch_entry_access(hdr, hdr->capacity, slot_idx);

	*found = true;

	if (record->val_kind != UCACHE_RECORD_VAL_NONE) {
		if (ucache_key_record_val_retained(record)) {
			ZVAL_COPY(return_value, &record->val);

			return true;
		}

		ucache_key_record_release_val(record, &pending_seed->detached_val);
	}

	return ucache_fetch_emit_val_locked(hdr, entry, epoch, return_value, pending_seed, lock_held);
}

bool ucache_exists_locked(zend_string *key)
{
	zend_ulong hash = zend_string_hash_val(key);
	uint32_t slot_idx;
	bool found;

	if (!ucache_find_slot_locked(key, hash, UCACHE_FIND_SLOT_SKIP_EXPIRED, NULL, &slot_idx, &found)) {
		return false;
	}

	return found;
}

void ucache_discard_replaced_entry_locked(ucache_entry *replaced_entry)
{
	if (!ucache_entry_is_used(replaced_entry)) {
		return;
	}

	ucache_release_entry_storage_locked(replaced_entry);

	memset(replaced_entry, 0, sizeof(*replaced_entry));
}

void ucache_rollback_replaced_entry_locked(
		zend_string *key,
		ucache_entry *replaced_entry)
{
	ucache_hdr *hdr;
	ucache_entry *entry;
	uint32_t slot_idx;
	bool found;

	if (!ucache_find_slot_locked(
			key,
			zend_string_hash_val(key),
			UCACHE_FIND_SLOT_IGNORE_EXPIRY,
			&hdr,
			&slot_idx,
			&found
		)
	) {
		return;
	}

	entry = &ucache_entries_ptr(hdr)[slot_idx];

	if (ucache_entry_is_used(replaced_entry)) {
		if (found) {
			ucache_pool_idx_unlink_locked(hdr, entry, slot_idx);
			ucache_release_entry_storage_locked(entry);
		} else {
			if (ucache_entry_is_tombstone(entry) && hdr->tombstone_count != 0) {
				hdr->tombstone_count--;
			}

			hdr->count++;
		}

		if (found && entry->expires_at != 0) {
			hdr->expiring_count--;
		}

		if (replaced_entry->expires_at != 0) {
			hdr->expiring_count++;
		}

		*entry = *replaced_entry;

		ucache_expiry_floor_lower_locked(hdr, entry->expires_at);

		ucache_pool_idx_link_locked(hdr, entry, slot_idx);
		ucache_pool_bucket_changed_locked(hdr, ucache_entry_pool_bucket(entry));
		ucache_entry_set_block_owner(entry, slot_idx);

		ucache_bump_mutation_epoch_locked(hdr);

		entry->gen = hdr->mutation_epoch;

		memset(replaced_entry, 0, sizeof(*replaced_entry));
	} else if (found) {
		ucache_delete_entry_locked(hdr, entry, slot_idx);
	}
}

uint64_t ucache_delete_locked(zend_string *key)
{
	ucache_hdr *hdr;
	ucache_entry *entries;
	zend_ulong hash = zend_string_hash_val(key);
	uint32_t slot_idx;
	bool found;

	ucache_maybe_expunge_expired_locked();

	if (ucache_find_slot_for_write_locked(key, hash, &hdr, &slot_idx, &found) && found) {
		entries = ucache_entries_ptr(hdr);

		ucache_delete_entry_locked(hdr, &entries[slot_idx], slot_idx);

		ucache_maybe_rehash_locked();
	}

	return ucache_hdr_is_initialized_locked()
		? ucache_hdr_ptr()->mutation_epoch
		: 0
	;
}

uint64_t ucache_delete_gen_locked(zend_string *key, uint64_t gen)
{
	ucache_hdr *hdr;
	ucache_entry *entry;
	uint32_t slot_idx;
	bool found;

	if (!ucache_find_slot_for_write_locked(key, zend_string_hash_val(key), &hdr, &slot_idx, &found) || !found) {
		return 0;
	}

	entry = &ucache_entries_ptr(hdr)[slot_idx];
	if (entry->gen != gen) {
		return 0;
	}

	ucache_delete_entry_locked(hdr, entry, slot_idx);

	ucache_maybe_rehash_locked();

	return hdr->mutation_epoch;
}

void ucache_delete_by_prefix_locked(zend_string *prefix)
{
	ucache_hdr *hdr;
	ucache_entry *entries, *entry;
	ucache_pool_links *links;
	uint32_t bucket, ref, next, slot;

	hdr = ucache_hdr_ptr();
	if (hdr == NULL || !ucache_hdr_adoptable_locked()) {
		return;
	}

	entries = ucache_entries_ptr(hdr);
	links = ucache_pool_links_ptr(hdr);
	bucket = (uint32_t) (zend_string_hash_val(prefix) & (UCACHE_POOL_BUCKETS - 1));
	for (ref = hdr->pool_bucket_heads[bucket]; ref != 0; ref = next) {
		ZEND_ASSERT(ref <= hdr->capacity);
		slot = ucache_pool_link_ref_slot(ref);
		entry = &entries[slot];
		next = links[slot].next;
		if (!ucache_entry_is_used(entry) ||
			entry->key_len < ZSTR_LEN(prefix) ||
			memcmp(
				ucache_entry_key_in_hdr(hdr, entry),
				ZSTR_VAL(prefix),
				ZSTR_LEN(prefix)
			) != 0
		) {
			continue;
		}

		ucache_delete_entry_locked(hdr, entry, slot);
	}

	ucache_sgraph_reclaim_orphaned_locked();

	ucache_maybe_rehash_locked();
}

bool ucache_atomic_update_locked(
		zend_string *key,
		zend_long step,
		zend_long ttl,
		bool decrement,
		ucache_atomic_update_result *result)
{
	ucache_hdr *hdr;
	ucache_entry *entries, *entry;
	zend_ulong hash = zend_string_hash_val(key);
	zend_long updated;
	uint32_t slot_idx;
	bool found, is_overflow;

	result->new_val = 0;
	result->stored_gen = 0;
	result->is_overflow = false;
	result->is_type_err = false;

	ucache_maybe_expunge_expired_locked();

	if (!ucache_find_slot_for_write_locked(key, hash, &hdr, &slot_idx, &found) || !found) {
		return ucache_atomic_insert_missing_locked(
			key,
			step,
			ttl,
			decrement,
			result
		);
	}

	entries = ucache_entries_ptr(hdr);
	entry = &entries[slot_idx];
	if (ucache_entry_val_type(entry) != UCACHE_VAL_LONG) {
		result->is_type_err = true;

		return false;
	}

	is_overflow = decrement
		? ucache_long_sub_overflow(entry->long_val, step, &updated)
		: ucache_long_add_overflow(entry->long_val, step, &updated)
	;
	if (is_overflow) {
		result->is_overflow = true;

		return false;
	}

	entry->long_val = updated;

	ucache_touch_entry_access(hdr, hdr->capacity, slot_idx);

	ucache_bump_mutation_epoch_locked(hdr);

	entry->gen = hdr->mutation_epoch;

	result->new_val = entry->long_val;
	result->stored_gen = entry->gen;
	result->stored_slot = slot_idx;
	result->stored_expires_at = entry->expires_at;
	if (entry->expires_at == 0 && ttl == 0) {
		ucache_scalar_write_enable_locked(hdr);
	}

	return true;
}

bool ucache_try_store_scalar(
		ucache_key_record *record,
		const ucache_prepared_val *prepared)
{
	ucache_hdr *hdr;
	ucache_entry *entry;
	uint32_t stripe_idx, slot_idx;

	ZEND_ASSERT(prepared->val_type <= UCACHE_VAL_DOUBLE);

	if (ucache_key_record_is_cur_miss(record)) {
		return false;
	}

	ucache_scalar_write_release_record_val(record);

	if (!ucache_scalar_write_begin(prepared->hash, &hdr, &stripe_idx)) {
		return false;
	}

	if (!ucache_scalar_write_locate(hdr, record, &slot_idx)) {
		ucache_scalar_write_end();

		return false;
	}

	entry = &ucache_entries_ptr(hdr)[slot_idx];
	if (entry->expires_at != 0 || !ucache_entry_holds_scalar(entry) ||
		!ucache_scalar_write_can_skip_sweep()
	) {
		ucache_scalar_write_end();

		return false;
	}

	ucache_scalar_write_prepare(hdr, stripe_idx, slot_idx);

	ucache_entry_set_val_type(entry, prepared->val_type);
	if (prepared->val_type == UCACHE_VAL_DOUBLE) {
		entry->double_val = prepared->double_val;
	} else {
		ucache_entry_set_long_zero_filled(
			entry,
			prepared->val_type == UCACHE_VAL_LONG ? prepared->long_val : 0
		);
	}

	ucache_touch_entry_access(hdr, hdr->capacity, slot_idx);

	ucache_scalar_write_commit(hdr, stripe_idx, entry);

	ucache_key_record_mark_hit(record, entry->gen, slot_idx, entry, NULL);

	ucache_scalar_write_end();

	return true;
}

bool ucache_try_atomic_update(
		ucache_key_record *record,
		zend_long step,
		zend_long ttl,
		bool decrement,
		ucache_atomic_update_result *result,
		bool *updated)
{
	ucache_hdr *hdr;
	ucache_entry *entry;
	zend_long val;
	uint32_t stripe_idx, slot_idx;

	if (ttl != 0 || ucache_key_record_is_cur_miss(record)) {
		return false;
	}

	ucache_scalar_write_release_record_val(record);

	if (!ucache_scalar_write_begin(ZSTR_H(record->storage_key), &hdr, &stripe_idx)) {
		return false;
	}

	if (!ucache_scalar_write_locate(hdr, record, &slot_idx)) {
		ucache_scalar_write_end();

		return false;
	}

	entry = &ucache_entries_ptr(hdr)[slot_idx];
	if (entry->expires_at != 0 || !ucache_entry_holds_scalar(entry) ||
		!ucache_scalar_write_can_skip_sweep()
	) {
		ucache_scalar_write_end();

		return false;
	}

	memset(result, 0, sizeof(*result));

	*updated = false;

	if (ucache_entry_val_type(entry) != UCACHE_VAL_LONG) {
		result->is_type_err = true;
	} else if (decrement
			? ucache_long_sub_overflow(entry->long_val, step, &val)
			: ucache_long_add_overflow(entry->long_val, step, &val)
	) {
		result->is_overflow = true;
	} else {
		ucache_scalar_write_prepare(hdr, stripe_idx, slot_idx);
		entry->long_val = val;

		ucache_touch_entry_access(hdr, hdr->capacity, slot_idx);

		ucache_scalar_write_commit(hdr, stripe_idx, entry);

		ucache_key_record_mark_hit(record, entry->gen, slot_idx, entry, NULL);

		result->new_val = val;
		*updated = true;
	}

	ucache_scalar_write_end();

	return true;
}

PHP_UCACHE_HOT ucache_optimistic_result ucache_fetch_optimistic(
		ucache_key_record *record,
		zval *return_value,
		bool allow_decode)
{
	ucache_hdr *hdr = ucache_hdr_ptr();
	zend_string *key = record->storage_key;
	uint64_t seq, epoch;

	if (!UCACHE_OPTIMISTIC_ENABLED || hdr == NULL || UCACHE_DEBUG_FAULT("FORCE_LOCKED_FETCH")) {
		return UCACHE_OPTIMISTIC_FALLBACK;
	}

	seq = ucache_load_seq(hdr, key);
	if (seq < 2 || (seq & 1) != 0) {
		return UCACHE_OPTIMISTIC_FALLBACK;
	}

	epoch = ucache_atomic_load_64(&hdr->mutation_epoch);

	if (record->mutation_epoch == epoch) {
		if (record->state == UCACHE_RECORD_MISS) {
			return UCACHE_OPTIMISTIC_MISS;
		}

		if (record->val_kind != UCACHE_RECORD_VAL_NONE &&
			ucache_key_record_val_usable(hdr, record)
		) {
			if (record->expires_at != 0 && ucache_read_seq(hdr, key) != seq) {
				return UCACHE_OPTIMISTIC_FALLBACK;
			}

			ZVAL_COPY(return_value, &record->val);

			ucache_touch_cached_entry_access(hdr, record->slot_idx, &record->access_touches);

			return UCACHE_OPTIMISTIC_FOUND;
		}
	}

	return ucache_fetch_optimistic_entry(hdr, record, seq, epoch, return_value, allow_decode);
}

PHP_UCACHE_HOT ucache_optimistic_result ucache_exists_optimistic(ucache_key_record *record)
{
	ucache_hdr *hdr = ucache_hdr_ptr();
	zend_string *key = record->storage_key;
	uint64_t seq, epoch;

	if (!UCACHE_OPTIMISTIC_ENABLED || hdr == NULL) {
		return UCACHE_OPTIMISTIC_FALLBACK;
	}

	seq = ucache_load_seq(hdr, key);
	if (seq < 2 || (seq & 1) != 0) {
		return UCACHE_OPTIMISTIC_FALLBACK;
	}

	epoch = ucache_atomic_load_64(&hdr->mutation_epoch);

	if (record->mutation_epoch == epoch) {
		if (record->state == UCACHE_RECORD_MISS) {
			return UCACHE_OPTIMISTIC_MISS;
		}

		if (record->state == UCACHE_RECORD_HIT && record->expires_at == 0) {
			return UCACHE_OPTIMISTIC_FOUND;
		}
	}

	return ucache_exists_optimistic_entry(hdr, record, seq, epoch);
}
