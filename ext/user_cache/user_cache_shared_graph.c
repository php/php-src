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

#include "user_cache_shared_graph.h"

#include "ext/random/php_random.h"

#define UCACHE_GRAPH_PIN_PROBE_INTERVAL_TICKS (8U * UCACHE_CLOCK_TICKS_PER_SEC)

#define UCACHE_BLOCK_OWNER_RETIRED_GRAPH (UINT32_MAX - 1)

#define UCACHE_RETIRED_SCAN_INLINE_BLOCKS		64U

typedef struct {
	uint32_t *block_offsets;
	bool *referenced;
	uint32_t count;
	uint32_t capacity;
	uint32_t inline_block_offsets[UCACHE_RETIRED_SCAN_INLINE_BLOCKS];
	bool inline_referenced[UCACHE_RETIRED_SCAN_INLINE_BLOCKS];
} ucache_retired_block_list;

static zend_always_inline bool ucache_graph_pin_slot_in_bitmap(
		const ucache_sgraph_hdr *hdr,
		uint32_t slot_idx)
{
	return slot_idx / 32U < MIN(hdr->pin_word_count, UCACHE_GRAPH_PIN_WORDS_MAX);
}

static zend_always_inline bool ucache_graph_pin_claim_is_stale(
		const ucache_graph_pin_claim *claim,
		int my_pid32)
{
	return atomic_load(&claim->hdr->graph_pin_slots[claim->slot_idx].owner_pid) != my_pid32;
}

static zend_always_inline uint32_t ucache_sgraph_ref_slot_mask(void)
{
	return UC_G(sgraph_ref_capacity) * 2 - 1;
}

static PHP_UCACHE_HOT ucache_sgraph_hdr *ucache_sgraph_payload_hdr(uint32_t payload_offset)
{
	const uint8_t *gbuf;
	ucache_sgraph_hdr *hdr;
	size_t buf_len;

	if (payload_offset == 0) {
		return NULL;
	}

	buf_len = ucache_block_payload_capacity(ucache_hdr_ptr(), payload_offset);
	if (buf_len == 0) {
		return NULL;
	}

	gbuf = ucache_sgraph_locate(
		ucache_ptr(payload_offset),
		buf_len,
		NULL
	);
	if (gbuf == NULL) {
		return NULL;
	}

	hdr = (ucache_sgraph_hdr *) gbuf;

	return hdr;
}

static uint32_t ucache_graph_pin_popcount(uint32_t v)
{
	v = v - ((v >> 1) & 0x55555555U);
	v = (v & 0x33333333U) + ((v >> 2) & 0x33333333U);

	return (((v + (v >> 4)) & 0x0F0F0F0FU) * 0x01010101U) >> 24;
}

static void ucache_graph_pin_count_add(ucache_graph_pin_slot *slot, int delta)
{
	int expected = atomic_load(&slot->pin_count), desired;

	for (;;) {
		desired = expected + delta;
		ZEND_ASSERT(desired >= 0 && "graph pin count underflow");
		if (UNEXPECTED(desired < 0)) {
			desired = 0;
		}

		if (atomic_compare_exchange_strong(&slot->pin_count, &expected, desired)) {
			return;
		}
	}
}

static bool ucache_graph_pin_probe_due(void)
{
	return UC_G(graph_pin_probe_last_at) == 0 ||
		ucache_clock_now() - UC_G(graph_pin_probe_last_at) >= UCACHE_GRAPH_PIN_PROBE_INTERVAL_TICKS
	;
}

static void ucache_graph_pin_bit_set(ucache_sgraph_hdr *hdr, uint32_t slot_idx)
{
	atomic_int *word = &hdr->pin_owners[slot_idx / 32U];
	int mask = (int) (1U << (slot_idx % 32U)),
		expected
	;

	ZEND_ASSERT(ucache_graph_pin_slot_in_bitmap(hdr, slot_idx));

	expected = atomic_load(word);

	while ((expected & mask) == 0) {
		if (atomic_compare_exchange_strong(word, &expected, expected | mask)) {
			return;
		}
	}
}

static bool ucache_graph_pin_bit_clear(ucache_sgraph_hdr *hdr, uint32_t slot_idx)
{
	atomic_int *word = &hdr->pin_owners[slot_idx / 32U];
	int mask = (int) (1U << (slot_idx % 32U)),
		expected
	;

	if (!ucache_graph_pin_slot_in_bitmap(hdr, slot_idx)) {
		return false;
	}

	expected = atomic_load(word);

	while ((expected & mask) != 0) {
		if (atomic_compare_exchange_strong(word, &expected, expected & ~mask)) {
			return true;
		}
	}

	return false;
}

static void ucache_graph_pin_claims_recycle_idle(int my_pid32)
{
	ucache_graph_pin_claim *claims = UC_G(graph_pin_claims);
	ucache_graph_pin_slot *slot;
	uint32_t i;
	int expected;

	for (i = 0; i < UC_G(graph_pin_claim_count); i++) {
		slot = &claims[i].hdr->graph_pin_slots[claims[i].slot_idx];
		if (atomic_load(&slot->pin_count) != 0) {
			continue;
		}

		claims[i] = claims[--UC_G(graph_pin_claim_count)];
		expected = my_pid32;

		ucache_atomic_store_64(&slot->owner_start_time, 0);

		atomic_compare_exchange_strong(&slot->owner_pid, &expected, 0);

		break;
	}
}

static bool ucache_graph_pin_slot_is_idle_with_dead_owner(
		ucache_graph_pin_slot *slot,
		int owner,
		uint64_t owner_start_time,
		int my_pid32)
{
	if (owner == 0 ||
		owner == my_pid32 ||
		atomic_load(&slot->pin_count) != 0
	) {
		return false;
	}

	return owner == UCACHE_GRAPH_PIN_OWNER_ABANDONED ||
		ucache_owner_is_dead((uint64_t) (uint32_t) owner, owner_start_time)
	;
}

static int32_t ucache_graph_pin_slot_find(ucache_hdr *hdr, bool claim)
{
	ucache_graph_pin_claim *claims = UC_G(graph_pin_claims);
	ucache_graph_pin_slot *slot;
	uint64_t my_pid = ucache_cached_pid(), owner_start_time;
	uint32_t i = 0;
	int32_t found = -1;
	int my_pid32 = (int) (uint32_t) my_pid, expected;

	while (i < UC_G(graph_pin_claim_count)) {
		if (ucache_graph_pin_claim_is_stale(&claims[i], my_pid32)) {
			claims[i] = claims[--UC_G(graph_pin_claim_count)];

			continue;
		}

		if (claims[i].hdr == hdr) {
			return (int32_t) claims[i].slot_idx;
		}

		i++;
	}

	if (claim && UC_G(graph_pin_claim_count) == UCACHE_GRAPH_PIN_CLAIM_MAX) {
		ucache_graph_pin_claims_recycle_idle(my_pid32);
	}

	if (!claim || UC_G(graph_pin_claim_count) == UCACHE_GRAPH_PIN_CLAIM_MAX) {
		return -1;
	}

	for (i = 0; i < ucache_graph_pin_slot_count(hdr); i++) {
		slot = &hdr->graph_pin_slots[i];
		expected = 0;

		if (atomic_load(&slot->owner_pid) == 0 &&
			atomic_compare_exchange_strong(&slot->owner_pid, &expected, my_pid32)
		) {
			found = (int32_t) i;

			break;
		}
	}

	for (i = 0; found < 0 && i < ucache_graph_pin_slot_count(hdr); i++) {
		slot = &hdr->graph_pin_slots[i];
		owner_start_time = ucache_atomic_load_64(&slot->owner_start_time);
		expected = atomic_load(&slot->owner_pid);

		if (!ucache_graph_pin_slot_is_idle_with_dead_owner(slot, expected, owner_start_time, my_pid32) ||
			!ucache_atomic_cas_64(&slot->owner_start_time, owner_start_time, 0) ||
			!atomic_compare_exchange_strong(&slot->owner_pid, &expected, my_pid32)
		) {
			continue;
		}

		found = (int32_t) i;

		UCACHE_DEBUG_SIMULATE_KILL("EXIT_IN_GRAPH_PIN_SLOT_RECLAIM");
	}

	if (found >= 0) {
		ucache_atomic_store_64(&slot->owner_start_time, ucache_self_start_time_token());

		claims[UC_G(graph_pin_claim_count)].hdr = hdr;
		claims[UC_G(graph_pin_claim_count)].slot_idx = (uint32_t) found;

		UC_G(graph_pin_claim_count)++;
	}

	return found;
}

static int32_t ucache_graph_pin_slot_claim(ucache_hdr *hdr)
{
	int32_t slot;

	if (UNEXPECTED(UC_G(graph_pin_claim_failed_hdr) == hdr) &&
		UC_G(graph_pin_claim_failed_pid) == ucache_cached_pid()
	) {
		return -1;
	}

	slot = ucache_graph_pin_slot_find(hdr, true);
	if (UNEXPECTED(slot < 0) && UC_G(graph_pin_claim_count) < UCACHE_GRAPH_PIN_CLAIM_MAX) {
		UC_G(graph_pin_claim_failed_hdr) = hdr;
		UC_G(graph_pin_claim_failed_pid) = ucache_cached_pid();
	}

	return slot;
}

static void ucache_graph_pin_strip_payload_locked(
		ucache_sgraph_hdr *ghdr,
		const uint32_t *dead_pin_mask)
{
	uint32_t w, drop, cleared = 0;
	int expected, desired, refcount;

	for (w = 0; w < MIN(ghdr->pin_word_count, UCACHE_GRAPH_PIN_WORDS_MAX); w++) {
		if (dead_pin_mask[w] == 0) {
			continue;
		}

		expected = atomic_load(&ghdr->pin_owners[w]);

		for (;;) {
			desired = expected & ~(int) dead_pin_mask[w];
			if (desired == expected) {
				break;
			}

			if (atomic_compare_exchange_strong(&ghdr->pin_owners[w], &expected, desired)) {
				cleared += ucache_graph_pin_popcount((uint32_t) expected & dead_pin_mask[w]);

				break;
			}
		}
	}

	if (cleared == 0) {
		return;
	}

	expected = atomic_load(&ghdr->ref_state);

	for (;;) {
		refcount = expected & UCACHE_SGRAPH_REF_STATE_REFCOUNT_MASK;
		drop = (uint32_t) refcount < cleared
			? (uint32_t) refcount
			: cleared
		;
		desired = (expected & UCACHE_SGRAPH_REF_STATE_RETIRED) | (refcount - (int) drop);

		if (atomic_compare_exchange_strong(&ghdr->ref_state, &expected, desired)) {
			break;
		}
	}

	ucache_hdr_ptr()->graph_dead_pins_stripped += cleared;
}

static void ucache_sgraph_mark_retired_block_locked(uint32_t payload_offset)
{
	ucache_block_ptr(ucache_payload_block_offset(payload_offset))->owner =
		UCACHE_BLOCK_OWNER_RETIRED_GRAPH
	;
}

static void ucache_sgraph_forget_orphan_locked(ucache_hdr *hdr, uint32_t payload_offset)
{
	uint32_t i;

	for (i = 0; i < UCACHE_ORPHANED_GRAPH_SLOTS; i++) {
		if (hdr->orphaned_graphs[i] == payload_offset) {
			hdr->orphaned_graphs[i] = 0;
		}
	}
}

static uint32_t ucache_sgraph_free_retired_block_locked(uint32_t payload_offset)
{
	ucache_sgraph_forget_orphan_locked(ucache_hdr_ptr(), payload_offset);

	ucache_block_ptr(ucache_payload_block_offset(payload_offset))->owner =
		UCACHE_BLOCK_OWNER_NONE
	;

	return ucache_free_locked(payload_offset);
}

static ucache_sgraph_hdr *ucache_sgraph_retired_block_hdr_locked(uint32_t payload_offset)
{
	ucache_block *block = ucache_block_ptr(ucache_payload_block_offset(payload_offset));

	if (ucache_block_is_free(block) || block->owner != UCACHE_BLOCK_OWNER_RETIRED_GRAPH) {
		return NULL;
	}

	return ucache_sgraph_payload_hdr(payload_offset);
}

static void ucache_sgraph_force_retire_locked(uint32_t payload_offset)
{
	ucache_sgraph_hdr *hdr = ucache_sgraph_payload_hdr(payload_offset);
	int state, expected;

	if (hdr == NULL) {
		return;
	}

	ucache_sgraph_mark_retired_block_locked(payload_offset);

	for (;;) {
		state = atomic_load(&hdr->ref_state);

		if ((state & UCACHE_SGRAPH_REF_STATE_RETIRED) != 0) {
			return;
		}

		expected = state;
		if (atomic_compare_exchange_strong(
			&hdr->ref_state, &expected, state | UCACHE_SGRAPH_REF_STATE_RETIRED
		)) {
			return;
		}
	}
}

static void ucache_retired_block_list_init(ucache_retired_block_list *list)
{
	list->block_offsets = list->inline_block_offsets;
	list->referenced = list->inline_referenced;
	list->count = 0;
	list->capacity = UCACHE_RETIRED_SCAN_INLINE_BLOCKS;
}

static void ucache_retired_block_list_free(ucache_retired_block_list *list)
{
	if (list->block_offsets != list->inline_block_offsets) {
		free(list->block_offsets);
		free(list->referenced);
	}
}

static bool ucache_retired_block_list_grow(ucache_retired_block_list *list)
{
	uint32_t *block_offsets, capacity;
	bool *referenced;

	if (list->capacity > UINT32_MAX / 2) {
		return false;
	}

	capacity = list->capacity * 2;
	block_offsets = malloc((size_t) capacity * sizeof(*block_offsets));
	referenced = malloc((size_t) capacity * sizeof(*referenced));
	if (block_offsets == NULL || referenced == NULL) {
		free(block_offsets);
		free(referenced);

		return false;
	}

	memcpy(block_offsets, list->block_offsets, (size_t) list->count * sizeof(*block_offsets));
	memcpy(referenced, list->referenced, (size_t) list->count * sizeof(*referenced));

	ucache_retired_block_list_free(list);

	list->block_offsets = block_offsets;
	list->referenced = referenced;
	list->capacity = capacity;

	return true;
}

static bool ucache_retired_block_list_add(ucache_retired_block_list *list, uint32_t block_offset)
{
	ZEND_ASSERT(list->count == 0 || list->block_offsets[list->count - 1] < block_offset);

	if (list->count == list->capacity && !ucache_retired_block_list_grow(list)) {
		return false;
	}

	list->block_offsets[list->count] = block_offset;
	list->referenced[list->count] = false;
	list->count++;

	return true;
}

static void ucache_retired_block_list_mark_locked(ucache_retired_block_list *list, uint32_t live_offset)
{
	uint32_t lo = 0, hi = list->count, mid, block_offset;

	while (lo < hi) {
		mid = lo + (hi - lo) / 2;

		if (ucache_block_payload_offset(list->block_offsets[mid]) <= live_offset) {
			lo = mid + 1;
		} else {
			hi = mid;
		}
	}

	if (lo == 0) {
		return;
	}

	block_offset = list->block_offsets[lo - 1];
	if (live_offset < ucache_offset_after(block_offset, ucache_block_size(ucache_block_ptr(block_offset)))) {
		list->referenced[lo - 1] = true;
	}
}

static void ucache_retired_block_list_mark_live_locked(
		ucache_retired_block_list *list,
		ucache_hdr *hdr)
{
	const ucache_entry_lock_record *lock_record;
	const ucache_entry *entry;
	uint32_t i, val_offset;

	for (i = 0; i < hdr->capacity; i++) {
		entry = &ucache_entries_ptr(hdr)[i];
		if (!ucache_entry_is_used(entry)) {
			continue;
		}

		if (entry->key_offset != 0) {
			ucache_retired_block_list_mark_locked(list, entry->key_offset);
		}

		val_offset = ucache_entry_val_offset(entry);
		if (val_offset != 0) {
			ucache_retired_block_list_mark_locked(list, val_offset);
		}
	}

	for (i = 0; i < hdr->entry_lock_capacity; i++) {
		lock_record = &ucache_entry_lock_records_ptr(hdr)[i];
		if (lock_record->state == UCACHE_ENTRY_LOCK_USED && lock_record->key_offset != 0) {
			ucache_retired_block_list_mark_locked(list, lock_record->key_offset);
		}
	}
}

static bool ucache_retired_block_list_collect_locked(
		ucache_retired_block_list *list,
		ucache_hdr *hdr)
{
	ucache_block *block;
	uint32_t used_end, offset, block_size;

	used_end = ucache_used_end_offset_locked(hdr);
	offset = ucache_offset_from_bytes(hdr->data_offset);
	while (offset < used_end) {
		block = ucache_block_ptr(offset);
		block_size = ucache_block_size(block);

		if (block_size < UCACHE_BLOCK_MIN_SIZE ||
			(block_size >> UCACHE_OFFSET_SHIFT) > used_end - offset
		) {
			return false;
		}

		if (ucache_sgraph_retired_block_hdr_locked(ucache_block_payload_offset(offset)) != NULL &&
			!ucache_retired_block_list_add(list, offset)
		) {
			return false;
		}

		offset = ucache_offset_after(offset, block_size);
	}

	return true;
}

static bool ucache_sgraph_payload_is_referenced_locked(uint32_t payload_offset)
{
	ucache_sgraph_hdr *hdr = ucache_sgraph_payload_hdr(payload_offset);

	return hdr != NULL &&
		(atomic_load(&hdr->ref_state) & UCACHE_SGRAPH_REF_STATE_REFCOUNT_MASK) != 0
	;
}

static bool ucache_sgraph_block_survives_clear_locked(
		const ucache_hdr *hdr,
		const ucache_entry *entries,
		const ucache_block *block,
		uint32_t block_offset)
{
	const ucache_entry *entry;
	uint32_t payload_offset = ucache_block_payload_offset(block_offset);

	if (block->owner == UCACHE_BLOCK_OWNER_RETIRED_GRAPH) {
		return ucache_sgraph_payload_is_referenced_locked(payload_offset);
	}

	if (block->owner >= hdr->capacity) {
		return true;
	}

	entry = &entries[block->owner];

	return ucache_entry_is_used(entry) &&
		ucache_entry_val_type(entry) == UCACHE_VAL_SGRAPH &&
		ucache_entry_val_offset(entry) == payload_offset &&
		ucache_sgraph_payload_is_referenced_locked(payload_offset)
	;
}

static bool ucache_sgraph_reclaim_orphaned_by_scan_locked(
		ucache_hdr *hdr,
		const uint32_t *dead_pin_mask,
		bool *completed)
{
	ucache_retired_block_list retired;
	ucache_sgraph_hdr *ghdr;
	uint32_t i, payload_offset;
	bool reclaimed = false;

	*completed = false;

	ucache_retired_block_list_init(&retired);

	if (!ucache_retired_block_list_collect_locked(&retired, hdr)) {
		ucache_retired_block_list_free(&retired);

		return false;
	}

	if (retired.count != 0) {
		ucache_retired_block_list_mark_live_locked(&retired, hdr);
	}

	for (i = 0; i < retired.count; i++) {
		if (retired.referenced[i]) {
			continue;
		}

		payload_offset = ucache_block_payload_offset(retired.block_offsets[i]);
		ghdr = ucache_sgraph_retired_block_hdr_locked(payload_offset);
		if (ghdr == NULL) {
			continue;
		}

		if (dead_pin_mask != NULL) {
			ucache_graph_pin_strip_payload_locked(ghdr, dead_pin_mask);
		}

		if (atomic_load(&ghdr->ref_state) == UCACHE_SGRAPH_REF_STATE_RETIRED) {
			ucache_sgraph_free_retired_block_locked(payload_offset);

			reclaimed = true;
		}
	}

	ucache_retired_block_list_free(&retired);

	*completed = true;

	return reclaimed;
}

static void ucache_destroy_sgraph_ref_slots(void)
{
	if (UC_G(sgraph_ref_slots) != NULL) {
		efree(UC_G(sgraph_ref_slots));

		UC_G(sgraph_ref_slots) = NULL;
	}
}

static void ucache_sgraph_refs_free(void)
{
	if (UC_G(sgraph_refs) != NULL) {
		efree(UC_G(sgraph_refs));

		UC_G(sgraph_refs) = NULL;
	}

	UC_G(sgraph_ref_count) = 0;
	UC_G(sgraph_ref_capacity) = 0;
	UC_G(sgraph_ref_bytes) = 0;

	ucache_destroy_sgraph_ref_slots();
}

static void ucache_sgraph_refs_check_fork(void)
{
	uint64_t pid = ucache_cached_pid();

	if (UC_G(sgraph_ref_owner_pid) == pid) {
		return;
	}

	if (UC_G(sgraph_ref_owner_pid) != 0) {
		ucache_sgraph_refs_free();

		ucache_decode_payload_addr_caches_release();
	}

	UC_G(sgraph_ref_owner_pid) = pid;
}

static uint32_t ucache_sgraph_ref_first_slot(
		const ucache_ctx *ctx,
		uint32_t payload_offset)
{
	return (uint32_t) php_random_splitmix64_mix((uint64_t) (uintptr_t) ctx ^ payload_offset) &
		ucache_sgraph_ref_slot_mask()
	;
}

static void ucache_sgraph_ref_slots_insert(uint32_t idx)
{
	const ucache_req_graph_ref *ref = &UC_G(sgraph_refs)[idx];
	uint32_t slot = ucache_sgraph_ref_first_slot(ref->ctx, ref->payload_offset);

	while (UC_G(sgraph_ref_slots)[slot] != 0) {
		slot = (slot + 1) & ucache_sgraph_ref_slot_mask();
	}

	UC_G(sgraph_ref_slots)[slot] = idx + 1;
}

static void ucache_grow_sgraph_refs(void)
{
	uint32_t capacity = UC_G(sgraph_ref_capacity), i, *slots;

	if (UNEXPECTED(capacity > UINT32_MAX / 4)) {
		zend_error_noreturn(E_ERROR, "UserCache: shared graph reference capacity exceeded");
	}

	capacity = capacity == 0 ? 8 : capacity * 2;

	UC_G(sgraph_refs) = safe_erealloc(
		UC_G(sgraph_refs),
		capacity,
		sizeof(ucache_req_graph_ref),
		0
	);

	slots = ecalloc((size_t) capacity * 2, sizeof(uint32_t));

	ucache_destroy_sgraph_ref_slots();

	UC_G(sgraph_ref_slots) = slots;
	UC_G(sgraph_ref_capacity) = capacity;

	for (i = 0; i < UC_G(sgraph_ref_count); i++) {
		ucache_sgraph_ref_slots_insert(i);
	}
}

static bool ucache_sgraph_release_ref_locked(uint32_t payload_offset)
{
	ucache_sgraph_hdr *hdr = ucache_sgraph_payload_hdr(payload_offset);
	ucache_hdr *cache_hdr = ucache_hdr_ptr();
	int32_t pin_slot = -1;
	bool released = false, bit_cleared = false;
	int state, refcount, expected, desired = 0;

	if (hdr == NULL) {
		return false;
	}

	if (cache_hdr != NULL) {
		pin_slot = ucache_graph_pin_slot_find(cache_hdr, false);
		if (pin_slot >= 0) {
			bit_cleared = ucache_graph_pin_bit_clear(hdr, (uint32_t) pin_slot);
		}
	}

	for (;;) {
		state = atomic_load(&hdr->ref_state);
		refcount = state & UCACHE_SGRAPH_REF_STATE_REFCOUNT_MASK;
		expected = state;

		if (refcount == 0) {
			break;
		}

		desired = (state & UCACHE_SGRAPH_REF_STATE_RETIRED) | (refcount - 1);
		if (atomic_compare_exchange_strong(&hdr->ref_state, &expected, desired)) {
			released = true;

			break;
		}
	}

	if (bit_cleared) {
		ucache_graph_pin_count_add(&cache_hdr->graph_pin_slots[pin_slot], -1);
	}

	return released &&
		(desired & UCACHE_SGRAPH_REF_STATE_RETIRED) != 0 &&
		(desired & UCACHE_SGRAPH_REF_STATE_REFCOUNT_MASK) == 0
	;
}

static bool ucache_sgraph_release_ref_unlocked(uint32_t payload_offset)
{
	ucache_sgraph_hdr *hdr = ucache_sgraph_payload_hdr(payload_offset);
	ucache_hdr *cache_hdr = ucache_hdr_ptr();
	int32_t pin_slot;
	int state, expected;
	bool bit_cleared = false;

	if (hdr == NULL || cache_hdr == NULL) {
		return false;
	}

	state = atomic_load(&hdr->ref_state);
	if (state & UCACHE_SGRAPH_REF_STATE_RETIRED) {
		return false;
	}

	pin_slot = ucache_graph_pin_slot_find(cache_hdr, false);
	if (pin_slot >= 0) {
		bit_cleared = ucache_graph_pin_bit_clear(hdr, (uint32_t) pin_slot);
	}

	while (!(state & UCACHE_SGRAPH_REF_STATE_RETIRED)) {
		ZEND_ASSERT((state & UCACHE_SGRAPH_REF_STATE_REFCOUNT_MASK) != 0);

		expected = state;

		if (atomic_compare_exchange_strong(&hdr->ref_state, &expected, state - 1)) {
			if (bit_cleared) {
				ucache_graph_pin_count_add(&cache_hdr->graph_pin_slots[pin_slot], -1);
			}

			return true;
		}

		state = expected;
	}

	if (bit_cleared) {
		ucache_graph_pin_bit_set(hdr, (uint32_t) pin_slot);
	}

	return false;
}

bool ucache_sgraph_release_op_ref(uint32_t payload_offset)
{
	bool recovered;

	if (ucache_sgraph_payload_hdr(payload_offset) == NULL || ucache_hdr_ptr() == NULL) {
		return false;
	}

	if (ucache_sgraph_release_ref_unlocked(payload_offset)) {
		return true;
	}

	if (!ucache_wlock_for_ref_release(&recovered)) {
		return false;
	}

	if (ucache_sgraph_release_ref_locked(payload_offset) &&
		recovered &&
		ucache_hdr_is_initialized_locked()
	) {
		if (ucache_quiesce_graph_payloads_locked()) {
			ucache_sgraph_free_retired_block_locked(payload_offset);
		} else {
			ucache_sgraph_orphan_payload_locked(payload_offset);
		}
	}

	ucache_unlock();

	return true;
}

const uint32_t *ucache_sgraph_node_sizes(uint32_t *count)
{
	static const uint32_t node_sizes[] = {
		UCACHE_SGRAPH_HDR_SIZE(0),
		sizeof(ucache_sgraph_val),
		sizeof(ucache_sgraph_prop),
		sizeof(ucache_sgraph_arr_elem),
		sizeof(ucache_sgraph_arr),
		sizeof(ucache_sgraph_arr_shape_elem),
		sizeof(ucache_sgraph_arr_shape),
		sizeof(ucache_sgraph_shaped_arr),
		sizeof(ucache_sgraph_obj),
		sizeof(ucache_sgraph_state_schema),
		sizeof(ucache_sgraph_shaped_state_obj),
		sizeof(ucache_sgraph_safe_direct_obj),
		sizeof(ucache_sgraph_serialized_obj),
		sizeof(ucache_sgraph_serdes_obj),
		sizeof(ucache_sgraph_ref),
		sizeof(ucache_sgraph_enum),
	};

	*count = sizeof(node_sizes) / sizeof(node_sizes[0]);

	return node_sizes;
}

uint32_t ucache_sgraph_payload_flags(uint32_t payload_offset)
{
	ucache_sgraph_hdr *hdr =
		ucache_sgraph_payload_hdr(payload_offset)
	;

	if (hdr == NULL) {
		return UCACHE_SGRAPH_FLAG_HAS_SHARED_IDENTITY
			| UCACHE_SGRAPH_FLAG_HAS_OBJ
			| UCACHE_SGRAPH_FLAG_PREFERS_PROTO
		;
	}

	return hdr->flags;
}

bool ucache_sgraph_payload_has_refs_locked(uint32_t payload_offset)
{
	ucache_sgraph_hdr *hdr =
		ucache_sgraph_payload_hdr(payload_offset)
	;

	return hdr != NULL && atomic_load(&hdr->ref_state) != 0;
}

bool ucache_sgraph_quiesce_for_overwrite_locked(uint32_t payload_offset)
{
	ucache_sgraph_hdr *hdr =
		ucache_sgraph_payload_hdr(payload_offset)
	;

	if (hdr == NULL) {
		return false;
	}

	if (!ucache_quiesce_graph_payloads_locked()) {
		return false;
	}

	return atomic_load(&hdr->ref_state) == 0;
}

void ucache_sgraph_orphan_payload_locked(uint32_t payload_offset)
{
	ucache_hdr *hdr = ucache_hdr_ptr();
	uint32_t i;

	ZEND_ASSERT(payload_offset != 0);

	if (hdr == NULL) {
		return;
	}

	ucache_sgraph_force_retire_locked(payload_offset);

	for (i = 0; i < UCACHE_ORPHANED_GRAPH_SLOTS; i++) {
		if (hdr->orphaned_graphs[i] == payload_offset) {
			return;
		}

		if (hdr->orphaned_graphs[i] == 0) {
			hdr->orphaned_graphs[i] = payload_offset;

			return;
		}
	}

	hdr->orphaned_graphs_saturated = 1;
}

void ucache_sgraph_reclaim_orphaned_locked(void)
{
	ucache_hdr *hdr = ucache_hdr_ptr();
	ucache_sgraph_hdr *ghdr;
	uint32_t i, payload_offset;
	bool checked_quiescent = false, scan_completed;

	if (hdr == NULL) {
		return;
	}

	for (i = 0; i < UCACHE_ORPHANED_GRAPH_SLOTS; i++) {
		payload_offset = hdr->orphaned_graphs[i];
		if (payload_offset == 0) {
			continue;
		}

		if (!checked_quiescent) {
			if (!ucache_quiesce_graph_payloads_locked()) {
				return;
			}

			checked_quiescent = true;
		}

		ghdr = ucache_sgraph_retired_block_hdr_locked(
			payload_offset
		);

		if (ghdr == NULL) {
			hdr->orphaned_graphs[i] = 0;

			continue;
		}

		if (atomic_load(&ghdr->ref_state) !=
			UCACHE_SGRAPH_REF_STATE_RETIRED
		) {
			hdr->orphaned_graphs[i] = 0;

			continue;
		}

		hdr->orphaned_graphs[i] = 0;

		ucache_sgraph_free_retired_block_locked(payload_offset);
	}

	if (hdr->orphaned_graphs_saturated != 0) {
		if (!checked_quiescent && !ucache_quiesce_graph_payloads_locked()) {
			return;
		}

		ucache_sgraph_reclaim_orphaned_by_scan_locked(hdr, NULL, &scan_completed);
		if (scan_completed) {
			hdr->orphaned_graphs_saturated = 0;
		}
	}
}

bool ucache_sgraph_strip_dead_pins_locked(bool force)
{
	ucache_hdr *hdr = ucache_hdr_ptr();
	ucache_graph_pin_slot *slot;
	ucache_entry *entries, *entry;
	ucache_sgraph_hdr *ghdr;
	uint64_t owner_pid, owner_start_time;
	uint32_t i, dead_mask[UCACHE_GRAPH_PIN_WORDS_MAX];
	bool any_dead = false, scan_completed;
	int val, owner;

	if (hdr == NULL || !ucache_hdr_is_initialized_locked()) {
		return false;
	}

	if (!force) {
		if (!ucache_graph_pin_probe_due()) {
			return false;
		}

		UC_G(graph_pin_probe_last_at) = ucache_clock_now();
	}

	memset(dead_mask, 0, sizeof(dead_mask));

	for (i = 0; i < ucache_graph_pin_slot_count(hdr); i++) {
		slot = &hdr->graph_pin_slots[i];

		val = atomic_load(&slot->owner_pid);
		if (val == 0 || atomic_load(&slot->pin_count) == 0) {
			continue;
		}

		if (val != UCACHE_GRAPH_PIN_OWNER_ABANDONED) {
			owner_pid = (uint64_t) (uint32_t) val;
			if (owner_pid == ucache_cached_pid() ||
				!ucache_owner_is_dead(
					owner_pid,
					ucache_atomic_load_64(
						&slot->owner_start_time
					)
				)
			) {
				continue;
			}
		}

		dead_mask[i / 32U] |= 1U << (i % 32U);

		any_dead = true;
	}

	if (!any_dead) {
		return false;
	}

	if (!ucache_quiesce_graph_payloads_locked()) {
		return false;
	}

	entries = ucache_entries_ptr(hdr);
	for (i = 0; i < hdr->capacity; i++) {
		entry = &entries[i];

		if (!ucache_entry_is_used(entry) ||
			ucache_entry_val_type(entry) != UCACHE_VAL_SGRAPH ||
			entry->val_offset == 0
		) {
			continue;
		}

		ghdr = ucache_sgraph_payload_hdr(entry->val_offset);
		if (ghdr != NULL) {
			ucache_graph_pin_strip_payload_locked(ghdr, dead_mask);
		}
	}

	any_dead = ucache_sgraph_reclaim_orphaned_by_scan_locked(hdr, dead_mask, &scan_completed);
	if (!scan_completed) {
		return any_dead;
	}

	for (i = 0; i < ucache_graph_pin_slot_count(hdr); i++) {
		if ((dead_mask[i / 32U] & (1U << (i % 32U))) == 0) {
			continue;
		}

		slot = &hdr->graph_pin_slots[i];
		owner_start_time = ucache_atomic_load_64(&slot->owner_start_time);
		owner = atomic_load(&slot->owner_pid);

		val = atomic_load(&slot->pin_count);
		while (val != 0) {
			if (atomic_compare_exchange_strong(&slot->pin_count, &val, 0)) {
				break;
			}
		}

		ucache_atomic_cas_64(&slot->owner_start_time, owner_start_time, 0);

		atomic_compare_exchange_strong(&slot->owner_pid, &owner, 0);

		hdr->graph_dead_pin_owners_reclaimed++;
	}

	return any_dead;
}

bool ucache_sgraph_acquire_ref(uint32_t payload_offset, bool *resource_limited)
{
	ucache_sgraph_hdr *hdr = ucache_sgraph_payload_hdr(payload_offset);
	ucache_hdr *cache_hdr = ucache_hdr_ptr();
	int32_t pin_slot = -1;
	int state, refcount, expected;

	if (resource_limited != NULL) {
		*resource_limited = false;
	}

	if (hdr == NULL) {
		return false;
	}

	pin_slot = cache_hdr != NULL ? ucache_graph_pin_slot_claim(cache_hdr) : -1;
	if (pin_slot < 0 || !ucache_graph_pin_slot_in_bitmap(hdr, (uint32_t) pin_slot)) {
		if (resource_limited != NULL) {
			*resource_limited = true;
		}

		return false;
	}

	ucache_graph_pin_count_add(&cache_hdr->graph_pin_slots[pin_slot], 1);

	for (;;) {
		state = atomic_load(&hdr->ref_state);
		refcount = state & UCACHE_SGRAPH_REF_STATE_REFCOUNT_MASK;
		expected = state;

		if ((state & UCACHE_SGRAPH_REF_STATE_RETIRED) != 0 ||
			refcount == UCACHE_SGRAPH_REF_STATE_REFCOUNT_MASK
		) {
			ucache_graph_pin_count_add(&cache_hdr->graph_pin_slots[pin_slot], -1);

			if (resource_limited != NULL &&
				refcount == UCACHE_SGRAPH_REF_STATE_REFCOUNT_MASK
			) {
				*resource_limited = true;
			}

			return false;
		}

		if (atomic_compare_exchange_strong(&hdr->ref_state, &expected, state + 1)) {
			break;
		}
	}

	ucache_graph_pin_bit_set(hdr, (uint32_t) pin_slot);

	return true;
}

size_t ucache_sgraph_largest_space_after_clear_locked(ucache_hdr *hdr)
{
	ucache_entry *entries = ucache_entries_ptr(hdr);
	ucache_block *block;
	uint32_t used_end, offset, block_size;
	size_t run = 0, largest = 0;

	used_end = ucache_used_end_offset_locked(hdr);
	offset = ucache_offset_from_bytes(hdr->data_offset);
	while (offset < used_end) {
		block = ucache_block_ptr(offset);
		block_size = ucache_block_size(block);

		if (block_size < UCACHE_BLOCK_MIN_SIZE ||
			(block_size >> UCACHE_OFFSET_SHIFT) > used_end - offset
		) {
			return SIZE_MAX;
		}

		if (!ucache_block_is_free(block) &&
			ucache_sgraph_block_survives_clear_locked(hdr, entries, block, offset)
		) {
			largest = MAX(largest, run);
			run = 0;
		} else {
			run += block_size;
		}

		offset = ucache_offset_after(offset, block_size);
	}

	if ((uint64_t) hdr->data_offset + ucache_data_tail_limit_locked(hdr) > ucache_offset_bytes(used_end)) {
		run += (size_t) ((uint64_t) hdr->data_offset + ucache_data_tail_limit_locked(hdr) - ucache_offset_bytes(used_end));
	}

	return MAX(largest, run);
}

bool ucache_sgraph_retire_payload_locked(uint32_t payload_offset)
{
	ucache_sgraph_hdr *hdr = ucache_sgraph_payload_hdr(payload_offset);
	int state, refcount, expected;

	if (hdr == NULL) {
		return true;
	}

	for (;;) {
		state = atomic_load(&hdr->ref_state);
		refcount = state & UCACHE_SGRAPH_REF_STATE_REFCOUNT_MASK;
		expected = state;

		if (refcount == 0) {
			return true;
		}

		if ((state & UCACHE_SGRAPH_REF_STATE_RETIRED) != 0 ||
			atomic_compare_exchange_strong(
				&hdr->ref_state,
				&expected,
				(state | UCACHE_SGRAPH_REF_STATE_RETIRED)
			)
		) {
			ucache_sgraph_mark_retired_block_locked(payload_offset);

			return false;
		}
	}
}

uint32_t ucache_req_sgraph_ref_idx(uint32_t payload_offset)
{
	const ucache_req_graph_ref *ref;
	ucache_ctx *ctx;
	uint32_t slot;

	ucache_sgraph_refs_check_fork();

	if (UC_G(sgraph_ref_count) == 0) {
		return UINT32_MAX;
	}

	ctx = ucache_active_ctx();

	for (slot = ucache_sgraph_ref_first_slot(ctx, payload_offset);
		UC_G(sgraph_ref_slots)[slot] != 0;
		slot = (slot + 1) & ucache_sgraph_ref_slot_mask()
	) {
		ref = &UC_G(sgraph_refs)[UC_G(sgraph_ref_slots)[slot] - 1];
		if (ref->ctx == ctx && ref->payload_offset == payload_offset) {
			return UC_G(sgraph_ref_slots)[slot] - 1;
		}
	}

	return UINT32_MAX;
}

void ucache_sgraph_ref_reserve(void)
{
	ucache_sgraph_refs_check_fork();

	if (UC_G(sgraph_ref_count) == UC_G(sgraph_ref_capacity)) {
		ucache_grow_sgraph_refs();
	}
}

uint32_t ucache_register_sgraph_ref(uint32_t payload_offset, uint32_t payload_len)
{
	uint32_t idx;

	ZEND_ASSERT(UC_G(sgraph_ref_owner_pid) == ucache_cached_pid());
	ZEND_ASSERT(UC_G(sgraph_ref_count) < UC_G(sgraph_ref_capacity));
	ZEND_ASSERT(payload_offset != 0);

	idx = UC_G(sgraph_ref_count)++;

	UC_G(sgraph_refs)[idx].ctx = ucache_active_ctx();
	UC_G(sgraph_refs)[idx].payload_offset = payload_offset;
	UC_G(sgraph_ref_bytes) += payload_len;

	ucache_sgraph_ref_slots_insert(idx);

	return idx;
}

void ucache_release_req_sgraph_refs(void)
{
	ucache_req_graph_ref *ref;
	ucache_ctx *ctx, *prev_ctx;
	uint32_t i, inner;
	bool recovered, maintenance_due, needs_lock;

	UC_G(graph_pin_claim_failed_hdr) = NULL;

	ucache_sgraph_refs_check_fork();

	if (UC_G(sgraph_ref_count) == 0) {
		ucache_sgraph_refs_free();

		return;
	}

	maintenance_due = ucache_graph_pin_probe_due();

	for (i = 0; i < UC_G(sgraph_ref_count); i++) {
		ctx = UC_G(sgraph_refs)[i].ctx;

		if (ctx == NULL) {
			continue;
		}

		prev_ctx = ucache_activate_ctx(ctx);
		needs_lock = maintenance_due;

		for (inner = i; inner < UC_G(sgraph_ref_count); inner++) {
			ref = &UC_G(sgraph_refs)[inner];

			if (ref->ctx != ctx) {
				continue;
			}

			if (ucache_sgraph_release_ref_unlocked(ref->payload_offset)) {
				ref->ctx = NULL;
			} else {
				needs_lock = true;
			}
		}

		if (needs_lock && ucache_wlock_for_ref_release(&recovered)) {
			if (!recovered) {
				for (inner = i; inner < UC_G(sgraph_ref_count); inner++) {
					ref = &UC_G(sgraph_refs)[inner];

					if (ref->ctx != ctx) {
						continue;
					}

					(void) ucache_sgraph_release_ref_locked(ref->payload_offset);

					ref->ctx = NULL;
				}
			} else if (ucache_hdr_is_initialized_locked()) {
				for (inner = i; inner < UC_G(sgraph_ref_count); inner++) {
					ref = &UC_G(sgraph_refs)[inner];

					if (ref->ctx != ctx) {
						continue;
					}

					if (ucache_sgraph_release_ref_locked(ref->payload_offset)) {
						if (ucache_quiesce_graph_payloads_locked()) {
							ucache_sgraph_free_retired_block_locked(ref->payload_offset);
						} else {
							ucache_sgraph_orphan_payload_locked(ref->payload_offset);
						}
					}

					ref->ctx = NULL;
				}

				ucache_sgraph_reclaim_orphaned_locked();
				(void) ucache_sgraph_strip_dead_pins_locked(false);
			}

			ucache_unlock();
		}

		for (inner = i; inner < UC_G(sgraph_ref_count); inner++) {
			if (UC_G(sgraph_refs)[inner].ctx == ctx) {
				UC_G(sgraph_refs)[inner].ctx = NULL;
			}
		}

		ucache_restore_ctx(prev_ctx);
	}

	if (maintenance_due) {
		UC_G(graph_pin_probe_last_at) = ucache_clock_now();
	}

	ucache_sgraph_refs_free();
}
