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

#define UCACHE_EXPUNGE_SCAN_MAX				4096U

#define UCACHE_EVICTION_WINDOW				32U
#define UCACHE_EVICTION_MAX_VICTIMS			64U
#define UCACHE_EVICTION_AGE_ORDERED_VICTIMS	4U
#define UCACHE_EVICTION_REMAINDER_PROBES	4U
#define UCACHE_EVICTION_SCAN_MAX			4096U

typedef struct {
	uint32_t stamps[UCACHE_EVICTION_WINDOW];
	uint32_t slots[UCACHE_EVICTION_WINDOW];
	uint32_t count;
} ucache_eviction_window;

static zend_always_inline bool ucache_expiry_floor_not_reached_locked(
		const ucache_hdr *hdr,
		uint64_t now_rel)
{
	return now_rel < (uint64_t) hdr->expiry_floor;
}

static bool ucache_expunge_expired_locked(void)
{
	ucache_hdr *hdr = ucache_hdr_ptr();
	ucache_entry *entries, *entry;
	uint64_t now;
	uint32_t i, floor;
	bool removed = false;

	if (hdr == NULL || !ucache_hdr_init_locked()) {
		return false;
	}

	if (hdr->expiring_count == 0) {
		hdr->expiry_floor = UCACHE_EXPIRY_FLOOR_NONE;

		ucache_sgraph_reclaim_orphaned_locked();

		return false;
	}

	now = ucache_time_rel(hdr, ucache_clock_now());
	if (ucache_expiry_floor_not_reached_locked(hdr, now)) {
		ucache_sgraph_reclaim_orphaned_locked();

		return false;
	}

	floor = UCACHE_EXPIRY_FLOOR_NONE;
	entries = ucache_entries_ptr(hdr);
	for (i = 0; i < hdr->capacity; i++) {
		entry = &entries[i];

		if (ucache_is_expired(entry, now)) {
			ucache_delete_entry_locked(hdr, entry, i);

			removed = true;
		} else if (ucache_entry_is_used(entry) && entry->expires_at != 0 && entry->expires_at < floor) {
			floor = entry->expires_at;
		}
	}

	hdr->expiry_floor = floor;

	ucache_sgraph_reclaim_orphaned_locked();

	return removed;
}

static bool ucache_entry_evictable_locked(
		ucache_hdr *hdr,
		const ucache_entry *entry,
		int8_t *gquiescent,
		uint64_t lock_now_rel)
{
	if (!ucache_entry_is_used(entry)) {
		return false;
	}

	if (ucache_entry_val_type(entry) == UCACHE_VAL_SGRAPH && entry->val_offset != 0) {
		if (ucache_sgraph_payload_has_refs_locked(entry->val_offset)) {
			return false;
		}

		if (*gquiescent < 0) {
			*gquiescent = ucache_quiesce_graph_payloads_locked() ? 1 : 0;
		}

		if (*gquiescent == 0) {
			return false;
		}
	}

	return !ucache_entry_key_lock_active_locked(
		hdr,
		entry->hash,
		ucache_entry_key_pos(entry),
		entry->key_len,
		lock_now_rel
	);
}

static bool ucache_eviction_window_accepts(const ucache_eviction_window *window, uint32_t stamp)
{
	uint32_t newer = 0, i;

	for (i = 0; i < window->count; i++) {
		newer += window->stamps[i] >= stamp;
	}

	return newer * 4 >= window->count * 3;
}

static uint32_t ucache_entry_coalesced_size_locked(const ucache_hdr *hdr, const ucache_entry *entry)
{
	const ucache_block *block;
	uint32_t payload_offset = ucache_entry_val_offset(entry),
		block_offset, next_offset, size, neighbour_size, merge_limit = ucache_block_merge_limit()
	;

	if (payload_offset == 0) {
		if ((entry->flags & UCACHE_ENTRY_FLAG_COMBINED_VAL_KEY) != 0 || entry->key_offset == 0) {
			return 0;
		}

		payload_offset = entry->key_offset;
	}

	block_offset = ucache_payload_block_offset(payload_offset);
	block = ucache_block_ptr_in_hdr(hdr, block_offset);
	size = ucache_block_size(block);

	next_offset = ucache_offset_after(block_offset, size);
	if (next_offset < ucache_used_end_offset_locked(hdr) &&
		ucache_block_is_free(ucache_block_ptr_in_hdr(hdr, next_offset))
	) {
		neighbour_size = ucache_block_size(ucache_block_ptr_in_hdr(hdr, next_offset));
		if ((uint64_t) size + neighbour_size <= merge_limit) {
			size += neighbour_size;
		}
	}

	if (ucache_block_prev_is_free(block)) {
		memcpy(&neighbour_size, (const uint8_t *) block - sizeof(neighbour_size), sizeof(neighbour_size));
		if ((uint64_t) size + neighbour_size <= merge_limit) {
			size += neighbour_size;
		}
	}

	return size;
}

static ucache_entry *ucache_block_owner_entry_locked(ucache_hdr *hdr, uint32_t block_offset)
{
	ucache_block *block = ucache_block_ptr(block_offset);
	ucache_entry *entry;
	uint32_t payload_offset = ucache_block_payload_offset(block_offset);

	if (ucache_block_is_free(block) || block->owner >= hdr->capacity) {
		return NULL;
	}

	entry = &ucache_entries_ptr(hdr)[block->owner];
	if (!ucache_entry_is_used(entry)) {
		return NULL;
	}

	if (ucache_entry_val_offset(entry) == payload_offset ||
		((entry->flags & UCACHE_ENTRY_FLAG_COMBINED_VAL_KEY) == 0 && entry->key_offset == payload_offset)
	) {
		return entry;
	}

	return NULL;
}

static bool ucache_grow_free_block_by_eviction_locked(
		ucache_hdr *hdr,
		uint32_t region,
		size_t needed_size,
		uint32_t max_victims,
		const ucache_eviction_window *window,
		uint32_t *victims,
		int8_t *gquiescent,
		uint64_t lock_now_rel)
{
	ucache_entry *entry, *entries = ucache_entries_ptr(hdr);
	uint32_t *stamps = ucache_access_stamps_ptr(hdr), next_offset, evicted = 0;

	while (region != 0 && evicted < max_victims && *victims < UCACHE_EVICTION_MAX_VICTIMS) {
		next_offset = ucache_offset_after(region, ucache_block_size(ucache_block_ptr(region)));
		if (next_offset >= ucache_used_end_offset_locked(hdr)) {
			return false;
		}

		entry = ucache_block_owner_entry_locked(hdr, next_offset);
		if (entry == NULL ||
			!ucache_eviction_window_accepts(window, UCACHE_ATOMIC_LOAD_32_RELAXED(&stamps[entry - entries])) ||
			!ucache_entry_evictable_locked(hdr, entry, gquiescent, lock_now_rel)
		) {
			return false;
		}

		ucache_delete_entry_locked(hdr, entry, (uint32_t) (entry - entries));

		hdr->eviction_count++;
		(*victims)++;
		evicted++;

		if (ucache_alloc_can_satisfy_locked(needed_size)) {
			return true;
		}

		if (region >= ucache_used_end_offset_locked(hdr)) {
			return false;
		}
	}

	return false;
}

static bool ucache_evict_remainder_neighbor_locked(
		ucache_hdr *hdr,
		size_t needed_size,
		const ucache_eviction_window *window,
		uint32_t *victims,
		int8_t *gquiescent,
		uint64_t lock_now_rel)
{
	const uint32_t *bins = ucache_free_bins_ptr(hdr), *mask = ucache_free_bin_mask_ptr(hdr);
	uint32_t word = ucache_free_bin_words(hdr->free_bin_count), bits, bin, block_offset, probes = 0;

	while (word-- > 0) {
		bits = mask[word];

		while (bits != 0) {
			bin = word * 32U + ucache_floor_log2(bits);
			bits &= ~(1U << (bin % 32U));

			block_offset = bins[bin];
			if (block_offset == 0) {
				continue;
			}

			if (probes++ == UCACHE_EVICTION_REMAINDER_PROBES) {
				return false;
			}

			if (ucache_grow_free_block_by_eviction_locked(
					hdr,
					block_offset,
					needed_size,
					1,
					window,
					victims,
					gquiescent,
					lock_now_rel
				)
			) {
				return true;
			}
		}
	}

	return false;
}

static void ucache_eviction_window_collect(
		ucache_hdr *hdr,
		ucache_eviction_window *window,
		uint32_t *hand,
		uint32_t *visited,
		int8_t *gquiescent,
		uint64_t lock_now_rel)
{
	ucache_entry *entries = ucache_entries_ptr(hdr), *entry;
	uint32_t *stamps = ucache_access_stamps_ptr(hdr), slot, scanned;

	window->count = 0;

	for (scanned = 0;
		*visited < hdr->capacity &&
		scanned < UCACHE_EVICTION_SCAN_MAX &&
		window->count < UCACHE_EVICTION_WINDOW;
		(*visited)++
	) {
		slot = *hand;
		*hand = slot + 1 == hdr->capacity ? 0 : slot + 1;
		entry = &entries[slot];

		if (window->count != 0 || ucache_entry_is_used(entry)) {
			scanned++;
		}

		if (ucache_entry_evictable_locked(hdr, entry, gquiescent, lock_now_rel)) {
			window->stamps[window->count] = UCACHE_ATOMIC_LOAD_32_RELAXED(&stamps[slot]);
			window->slots[window->count++] = slot;
		}
	}
}

static uint32_t ucache_eviction_window_pick(
		ucache_hdr *hdr,
		ucache_eviction_window *window,
		size_t needed_total,
		uint32_t victims,
		int8_t *gquiescent,
		uint64_t lock_now_rel,
		bool *fits_ptr)
{
	ucache_entry *entries = ucache_entries_ptr(hdr), *entry;
	uint32_t i, best = UINT32_MAX;
	bool fits, best_fits = false;

	for (i = 0; i < window->count; i++) {
		if (window->slots[i] == UINT32_MAX) {
			continue;
		}

		entry = &entries[window->slots[i]];
		if (!ucache_entry_evictable_locked(hdr, entry, gquiescent, lock_now_rel)) {
			window->slots[i] = UINT32_MAX;

			continue;
		}

		fits = needed_total == 0 ||
			(
				ucache_entry_coalesced_size_locked(hdr, entry) >= needed_total &&
				(
					victims >= UCACHE_EVICTION_AGE_ORDERED_VICTIMS ||
					ucache_eviction_window_accepts(window, window->stamps[i])
				)
			)
		;
		if (best == UINT32_MAX ||
			(fits && !best_fits) ||
			(fits == best_fits && window->stamps[i] < window->stamps[best])
		) {
			best = i;
			best_fits = fits;
		}
	}

	*fits_ptr = best_fits;

	return best;
}

static bool ucache_evict_lru_locked(size_t needed_size)
{
	ucache_hdr *hdr = ucache_hdr_ptr();
	ucache_eviction_window window;
	uint64_t lock_now, lock_now_rel;
	uint32_t hand, visited = 0, best, best_slot, region, victims = 0;
	int8_t gquiescent = -1;
	size_t needed_total = needed_size != 0
		? ucache_block_total_size(needed_size)
		: 0
	;
	bool evicted_any = false, best_fits;

	if (hdr == NULL || hdr->count == 0) {
		return false;
	}

	hand = hdr->eviction_hand % hdr->capacity;
	lock_now = ucache_clock_now();
	lock_now_rel = ucache_time_rel(hdr, lock_now);

	ucache_access_note_time(lock_now);

	window.count = 0;

	while (victims < UCACHE_EVICTION_MAX_VICTIMS) {
		best = ucache_eviction_window_pick(
			hdr,
			&window,
			needed_total,
			victims,
			&gquiescent,
			lock_now_rel,
			&best_fits
		);
		if (best == UINT32_MAX) {
			if (visited >= hdr->capacity) {
				break;
			}

			ucache_eviction_window_collect(hdr, &window, &hand, &visited, &gquiescent, lock_now_rel);
			if (window.count == 0) {
				break;
			}

			continue;
		}

		best_slot = window.slots[best];
		window.slots[best] = UINT32_MAX;

		if (needed_size != 0 &&
			!best_fits &&
			ucache_evict_remainder_neighbor_locked(
				hdr,
				needed_size,
				&window,
				&victims,
				&gquiescent,
				lock_now_rel
			)
		) {
			evicted_any = true;

			break;
		}

		if (!ucache_entry_is_used(&ucache_entries_ptr(hdr)[best_slot])) {
			evicted_any = true;

			continue;
		}

		region = ucache_delete_entry_locked(hdr, &ucache_entries_ptr(hdr)[best_slot], best_slot);

		hdr->eviction_count++;
		victims++;
		evicted_any = true;

		if (needed_size == 0 ||
			ucache_alloc_can_satisfy_locked(needed_size) ||
			ucache_grow_free_block_by_eviction_locked(
				hdr,
				region,
				needed_size,
				UCACHE_EVICTION_MAX_VICTIMS,
				&window,
				&victims,
				&gquiescent,
				lock_now_rel
			)
		) {
			break;
		}
	}

	hdr->eviction_hand = hand;

	return evicted_any;
}

static bool ucache_clear_locked(void)
{
	ucache_hdr *hdr = ucache_hdr_ptr();
	ucache_entry *entries;
	uint32_t i;

	if (hdr == NULL || !ucache_hdr_init_locked()) {
		return false;
	}

	entries = ucache_entries_ptr(hdr);
	for (i = 0; i < hdr->capacity; i++) {
		if (ucache_entry_is_used(&entries[i])) {
			ucache_release_entry_storage_locked(&entries[i]);
		}
	}

	memset(entries, 0, sizeof(ucache_entry) * hdr->capacity);

	ucache_pool_idx_reset_locked(hdr);
	ucache_access_stamps_reset(hdr);

	ucache_sgraph_reclaim_orphaned_locked();

	(void) ucache_sgraph_strip_dead_pins_locked(true);

	hdr->count = 0;
	hdr->expiring_count = 0;
	hdr->expiry_floor = UCACHE_EXPIRY_FLOOR_NONE;
	hdr->tombstone_count = 0;

	ucache_bump_mutation_epoch_locked(hdr);

	return true;
}

static bool ucache_val_takes_most_of_data_area(const ucache_hdr *hdr, size_t needed_size)
{
	return ucache_block_total_size(needed_size) > hdr->data_size / 2;
}

static bool ucache_eviction_can_make_room_locked(
		size_t needed_size,
		bool clearing,
		ucache_store_retries *retries)
{
	ucache_hdr *hdr = ucache_hdr_ptr();

	if (hdr == NULL ||
		needed_size == 0 ||
		(!clearing && !ucache_val_takes_most_of_data_area(hdr, needed_size))
	) {
		return true;
	}

	if (!retries->room_checked || retries->room_entry_lock_count != hdr->entry_lock_count) {
		retries->room_checked = true;
		retries->room_entry_lock_count = hdr->entry_lock_count;
		retries->room_after_clear = ucache_block_total_size(needed_size) <=
			ucache_sgraph_largest_space_after_clear_locked(hdr)
		;
	}

	return retries->room_after_clear;
}

void ucache_expunge_expired_bounded_locked(void)
{
	ucache_hdr *hdr = ucache_hdr_ptr();
	ucache_entry *entries;
	uint64_t now;
	uint32_t cursor, scan_len, i;

	if (hdr == NULL || !ucache_hdr_init_locked()) {
		return;
	}

	if (hdr->expiring_count == 0) {
		hdr->expiry_floor = UCACHE_EXPIRY_FLOOR_NONE;

		ucache_sgraph_reclaim_orphaned_locked();

		return;
	}

	now = ucache_time_rel(hdr, ucache_clock_now());
	if (ucache_expiry_floor_not_reached_locked(hdr, now)) {
		ucache_sgraph_reclaim_orphaned_locked();

		return;
	}

	entries = ucache_entries_ptr(hdr);

	cursor = UC_G(expired_expunge_cursor);
	if (cursor >= hdr->capacity) {
		cursor = 0;
	}

	scan_len = hdr->capacity < UCACHE_EXPUNGE_SCAN_MAX
		? hdr->capacity
		: UCACHE_EXPUNGE_SCAN_MAX
	;
	for (i = 0; i < scan_len; i++) {
		if (ucache_is_expired(&entries[cursor], now)) {
			ucache_delete_entry_locked(hdr, &entries[cursor], cursor);
		}

		++cursor;

		if (cursor == hdr->capacity) {
			cursor = 0;
		}
	}

	UC_G(expired_expunge_cursor) = cursor;

	ucache_sgraph_reclaim_orphaned_locked();
}

bool ucache_reclaim_space_for_store_locked(
		bool fits_in_empty_cache,
		size_t needed_size,
		ucache_store_retries *retries)
{
	bool reclaimed, *evict_used = needed_size == 0 ? &retries->slot_pressure_evict : &retries->mem_pressure_evict;

	if (!retries->expired) {
		retries->expired = true;

		reclaimed = ucache_expunge_expired_locked();

		if (ucache_sgraph_strip_dead_pins_locked(true)) {
			reclaimed = true;
		}

		if (reclaimed || (needed_size != 0 && ucache_alloc_can_satisfy_locked(needed_size))) {
			return true;
		}
	}

	if (fits_in_empty_cache &&
		UC_G(eviction_policy) == UCACHE_EVICTION_POLICY_LRU &&
		!*evict_used &&
		ucache_eviction_can_make_room_locked(needed_size, false, retries)
	) {
		*evict_used = true;

		if (ucache_evict_lru_locked(needed_size)) {
			return true;
		}
	}

	if (fits_in_empty_cache &&
		UC_G(eviction_policy) != UCACHE_EVICTION_POLICY_NONE &&
		!retries->clear
	) {
		retries->clear = true;

		if (ucache_entry_locks_allow_clear_locked(NULL) &&
			ucache_eviction_can_make_room_locked(needed_size, true, retries) &&
			ucache_clear_locked()
		) {
			ucache_hdr_ptr()->expunge_count++;

			return true;
		}
	}

	return false;
}

void ucache_expunge_expired_at_req_end(void)
{
	if (EXPECTED(UC_G(expired_read_observations) < UCACHE_EXPIRED_READ_EXPUNGE_THRESHOLD)) {
		return;
	}

	if (!ucache_wlock()) {
		return;
	}

	UC_G(expired_read_observations) = 0;

	ucache_expunge_expired_bounded_locked();

	ucache_unlock();
}

bool ucache_reclaim_space_for_entry_lock_locked(size_t key_size)
{
	ucache_store_retries retries = {0};

	while (!ucache_alloc_can_satisfy_locked(key_size)) {
		if (!ucache_reclaim_space_for_store_locked(
				ucache_payload_can_fit_locked(key_size),
				key_size,
				&retries
			)
		) {
			return false;
		}
	}

	return true;
}
