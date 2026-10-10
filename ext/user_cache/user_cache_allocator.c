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

#if ZEND_DEBUG
# define UCACHE_DEBUG_RESERVED_DATA_END	(((uint64_t) 1 << 32) - 64 * 1024)
#endif

#define UCACHE_BLOCK_MIN_UNITS	(UCACHE_BLOCK_MIN_SIZE / UCACHE_OFFSET_UNIT)

#ifdef ZEND_WIN32
static bool ucache_win32_ensure_committed_locked(ucache_hdr *hdr, uint64_t end_bytes);
static void ucache_win32_reset_released_tail_locked(ucache_hdr *hdr);
#endif /* ZEND_WIN32 */

static zend_always_inline void *ucache_block_payload(ucache_block *block)
{
	return block + 1;
}

static zend_always_inline ucache_free_links *ucache_block_links(ucache_block *block)
{
	return (ucache_free_links *) ucache_block_payload(block);
}

static zend_always_inline ucache_free_links *ucache_free_links_at(
		const ucache_hdr *hdr,
		uint32_t block_offset)
{
	return ucache_block_links(ucache_block_ptr_in_hdr(hdr, block_offset));
}

static zend_always_inline uint32_t ucache_prev_block_size(const ucache_block *block)
{
	uint32_t prev_size;

	memcpy(&prev_size, (const uint8_t *) block - sizeof(prev_size), sizeof(prev_size));

	return prev_size;
}

static zend_always_inline uint32_t ucache_data_start_offset(const ucache_hdr *hdr)
{
	return ucache_offset_from_bytes(hdr->data_offset);
}

static zend_always_inline bool ucache_block_offset_in_used(
		uint32_t block_offset,
		uint32_t data_start,
		uint32_t used_end)
{
	return block_offset >= data_start &&
		(uint64_t) block_offset + UCACHE_BLOCK_MIN_UNITS <= used_end
	;
}

static zend_always_inline bool ucache_block_size_fits_used(
		uint32_t block_offset,
		uint32_t block_size,
		uint32_t used_end)
{
	return block_size >= UCACHE_BLOCK_MIN_SIZE &&
		block_size % UCACHE_OFFSET_UNIT == 0 &&
		(uint64_t) block_offset + block_size / UCACHE_OFFSET_UNIT <= used_end
	;
}

static zend_always_inline bool ucache_prev_block_reachable(
		uint32_t block_offset,
		uint32_t prev_size,
		uint32_t data_start)
{
	return prev_size >= UCACHE_BLOCK_MIN_SIZE &&
		prev_size % UCACHE_OFFSET_UNIT == 0 &&
		block_offset >= data_start &&
		block_offset - data_start >= prev_size / UCACHE_OFFSET_UNIT
	;
}

static zend_always_inline void ucache_block_set_prev_free_locked(
		ucache_hdr *hdr,
		uint32_t block_offset,
		bool prev_free)
{
	ucache_block *block;

	if (block_offset >= ucache_used_end_offset_locked(hdr)) {
		return;
	}

	block = ucache_block_ptr_in_hdr(hdr, block_offset);

	if (prev_free) {
		block->size |= UCACHE_BLOCK_PREV_FREE;
	} else {
		block->size &= ~UCACHE_BLOCK_PREV_FREE;
	}
}

static zend_always_inline bool ucache_ensure_committed_locked(ucache_hdr *hdr, uint64_t end_bytes)
{
#ifdef ZEND_WIN32
	return ucache_win32_ensure_committed_locked(hdr, end_bytes);
#else
	(void) hdr;
	(void) end_bytes;

	return true;
#endif
}

static zend_always_inline void ucache_tail_grown_locked(ucache_hdr *hdr)
{
#ifdef ZEND_WIN32
	hdr->stale_tail_end = MAX(hdr->stale_tail_end, (uint64_t) hdr->data_offset + hdr->next_free);
#else
	(void) hdr;
#endif
}

static zend_always_inline void ucache_tail_released_locked(ucache_hdr *hdr)
{
#ifdef ZEND_WIN32
	ucache_win32_reset_released_tail_locked(hdr);
#else
	(void) hdr;
#endif
}

#ifdef ZEND_WIN32
static bool ucache_win32_ensure_committed_locked(ucache_hdr *hdr, uint64_t end_bytes)
{
	ucache_win32_seg *seg =
		(ucache_win32_seg *) ucache_active_ctx()->storage.seg
	;

	if (end_bytes <= seg->committed_bytes) {
		return true;
	}

	if (!ucache_win32_commit_to(seg, (size_t) end_bytes)) {
		hdr->commit_failure_count++;
		hdr->commit_ceiling = seg->committed_bytes;

		return false;
	}

	ucache_atomic_store_64(&hdr->committed_end, MAX(hdr->committed_end, seg->committed_bytes));

	hdr->commit_ceiling = 0;

	return true;
}

static void ucache_win32_reset_released_tail_locked(ucache_hdr *hdr)
{
	ucache_win32_seg *seg =
		(ucache_win32_seg *) ucache_active_ctx()->storage.seg
	;
	uint64_t reset_start, reset_end;

	reset_start = ZEND_MM_ALIGNED_SIZE_EX((uint64_t) hdr->data_offset + hdr->next_free, UCACHE_WIN32_COMMIT_CHUNK);
	reset_end = MIN(ZEND_MM_ALIGNED_SIZE_EX(hdr->stale_tail_end, UCACHE_WIN32_COMMIT_CHUNK), seg->seg.size);

	if (reset_end < reset_start + UCACHE_WIN32_COMMIT_CHUNK) {
		return;
	}

	ucache_win32_reset_range(seg, (size_t) reset_start, (size_t) reset_end);

	hdr->stale_tail_end = reset_start;
}
#endif /* ZEND_WIN32 */

static uint32_t ucache_alloc_from_tail_locked(
		ucache_hdr *hdr,
		size_t size,
		uint32_t total_size,
		const void *src,
		uint32_t owner)
{
	ucache_block *block;
	uint32_t block_offset;

	if (hdr->next_free > hdr->data_size || total_size > hdr->data_size - hdr->next_free) {
		return 0;
	}

	if (!ucache_ensure_committed_locked(hdr, (uint64_t) hdr->data_offset + hdr->next_free + total_size)) {
		return 0;
	}

	block_offset = ucache_used_end_offset_locked(hdr);
	block = ucache_block_ptr_in_hdr(hdr, block_offset);
	block->size = total_size;
	block->owner = owner;

	if (src != NULL) {
		memcpy(ucache_block_payload(block), src, size);
	}

	hdr->next_free += total_size;

	ucache_tail_grown_locked(hdr);

	return ucache_block_payload_offset(block_offset);
}

static void ucache_free_list_remove_locked(ucache_hdr *hdr, uint32_t block_offset)
{
	ucache_block *block = ucache_block_ptr_in_hdr(hdr, block_offset);
	ucache_free_links *links = ucache_block_links(block);
	uint32_t size = ucache_block_size(block), bin = ucache_size_class(size),
		prev_free = links->prev_free, next_free = links->next_free,
		data_start = ucache_data_start_offset(hdr), used_end = ucache_used_end_offset_locked(hdr),
		*bins = ucache_free_bins_ptr(hdr)
	;

	ZEND_ASSERT(hdr->free_list_bytes >= size && bin < hdr->free_bin_count);
	ZEND_ASSERT(prev_free == 0 || ucache_block_offset_in_used(prev_free, data_start, used_end));
	ZEND_ASSERT(next_free == 0 || ucache_block_offset_in_used(next_free, data_start, used_end));

	hdr->free_list_bytes = hdr->free_list_bytes >= size ? hdr->free_list_bytes - size : 0;

	if (prev_free != 0) {
		if (ucache_block_offset_in_used(prev_free, data_start, used_end)) {
			ucache_free_links_at(hdr, prev_free)->next_free = next_free;
		}
	} else if (bin < hdr->free_bin_count) {
		bins[bin] = next_free;
	}

	if (next_free != 0 && ucache_block_offset_in_used(next_free, data_start, used_end)) {
		ucache_free_links_at(hdr, next_free)->prev_free = prev_free;
	}

	if (bin < hdr->free_bin_count && bins[bin] == 0) {
		ucache_free_bin_mask_ptr(hdr)[bin / 32U] &= ~(1U << (bin % 32U));
	}

	block->size &= ~UCACHE_BLOCK_FREE;
}

static void ucache_free_list_insert_locked(
		ucache_hdr *hdr,
		uint32_t block_offset,
		uint32_t size,
		bool prev_free)
{
	ucache_block *block = ucache_block_ptr_in_hdr(hdr, block_offset);
	ucache_free_links *links = ucache_block_links(block);
	uint32_t bin = ucache_size_class(size), next_free,
		data_start = ucache_data_start_offset(hdr), used_end = ucache_used_end_offset_locked(hdr),
		*bins = ucache_free_bins_ptr(hdr)
	;

	ZEND_ASSERT((uint64_t) hdr->free_list_bytes + size <= hdr->next_free && bin < hdr->free_bin_count);
	ZEND_ASSERT(ucache_block_size_fits_used(block_offset, size, used_end));

	if (bin >= hdr->free_bin_count || !ucache_block_size_fits_used(block_offset, size, used_end)) {
		return;
	}

	next_free = bins[bin];

	ZEND_ASSERT(next_free == 0 || ucache_block_offset_in_used(next_free, data_start, used_end));

	if (next_free != 0 && !ucache_block_offset_in_used(next_free, data_start, used_end)) {
		next_free = 0;
	}

	hdr->free_list_bytes += size;

	block->size = size | UCACHE_BLOCK_FREE | (prev_free ? UCACHE_BLOCK_PREV_FREE : 0);
	block->owner = UCACHE_BLOCK_OWNER_NONE;

	memcpy((uint8_t *) block + size - sizeof(uint32_t), &size, sizeof(size));

	links->prev_free = 0;
	links->next_free = next_free;
	if (next_free != 0) {
		ucache_free_links_at(hdr, next_free)->prev_free = block_offset;
	}

	bins[bin] = block_offset;
	ucache_free_bin_mask_ptr(hdr)[bin / 32U] |= 1U << (bin % 32U);

	ucache_block_set_prev_free_locked(hdr, ucache_offset_after(block_offset, size), true);
}

static void ucache_trim_tail_locked(ucache_hdr *hdr, uint32_t block_offset)
{
	ucache_block *block = ucache_block_ptr_in_hdr(hdr, block_offset), *prev;
	uint32_t data_start = ucache_data_start_offset(hdr), prev_size, prev_offset;

	while (ucache_block_prev_is_free(block)) {
		prev_size = ucache_prev_block_size(block);

		ZEND_ASSERT(ucache_prev_block_reachable(block_offset, prev_size, data_start));

		if (!ucache_prev_block_reachable(block_offset, prev_size, data_start)) {
			break;
		}

		prev_offset = block_offset - ucache_offset_from_bytes(prev_size);
		prev = ucache_block_ptr_in_hdr(hdr, prev_offset);

		ZEND_ASSERT(ucache_block_is_free(prev) && ucache_block_size(prev) == prev_size);

		if (!ucache_block_is_free(prev) || ucache_block_size(prev) != prev_size) {
			break;
		}

		block_offset = prev_offset;
		block = prev;

		ucache_free_list_remove_locked(hdr, block_offset);
	}

	hdr->next_free = ucache_offset_bytes(block_offset) - hdr->data_offset;

	ucache_tail_released_locked(hdr);
}

static uint32_t ucache_free_bin_from(const ucache_hdr *hdr, uint32_t bin)
{
	const uint32_t *mask = ucache_free_bin_mask_ptr(hdr);
	uint32_t word = bin / 32U, words = ucache_free_bin_words(hdr->free_bin_count), bits;

	if (bin >= hdr->free_bin_count) {
		return UINT32_MAX;
	}

	bits = mask[word] & (~0U << (bin % 32U));

	while (bits == 0) {
		if (++word == words) {
			return UINT32_MAX;
		}

		bits = mask[word];
	}

	return word * 32U + (uint32_t) zend_ulong_ntz((zend_ulong) bits);
}

static uint32_t ucache_free_block_at_least_locked(
		const ucache_hdr *hdr,
		uint32_t total_size)
{
	uint32_t bin;

	ZEND_ASSERT(total_size == ucache_size_class_round_up(total_size));

	if (hdr->free_list_bytes < total_size) {
		return 0;
	}

	bin = ucache_free_bin_from(hdr, ucache_size_class(total_size));
	if (bin == UINT32_MAX) {
		return 0;
	}

	ZEND_ASSERT(ucache_free_bins_ptr(hdr)[bin] != 0);

	return ucache_free_bins_ptr(hdr)[bin];
}

static uint32_t ucache_alloc_from_free_list_locked(
		ucache_hdr *hdr,
		size_t size,
		uint32_t total_size,
		const void *src,
		uint32_t owner)
{
	ucache_block *block, *next;
	uint32_t block_offset, block_size, remainder, prev_flag, next_offset, next_size,
		data_start = ucache_data_start_offset(hdr), used_end = ucache_used_end_offset_locked(hdr)
	;

	block_offset = ucache_free_block_at_least_locked(hdr, total_size);
	if (block_offset == 0) {
		return 0;
	}

	ZEND_ASSERT(ucache_block_offset_in_used(block_offset, data_start, used_end));

	if (!ucache_block_offset_in_used(block_offset, data_start, used_end)) {
		return 0;
	}

	block = ucache_block_ptr_in_hdr(hdr, block_offset);
	block_size = ucache_block_size(block);
	prev_flag = block->size & UCACHE_BLOCK_PREV_FREE;

	ZEND_ASSERT(ucache_block_is_free(block) && block_size >= total_size);
	ZEND_ASSERT(ucache_block_size_fits_used(block_offset, block_size, used_end));

	if (!ucache_block_is_free(block) ||
		block_size < total_size ||
		!ucache_block_size_fits_used(block_offset, block_size, used_end)
	) {
		return 0;
	}

	ucache_free_list_remove_locked(hdr, block_offset);

	remainder = block_size - total_size;
	if (remainder >= UCACHE_BLOCK_MIN_SIZE) {
		next_offset = ucache_offset_after(block_offset, block_size);

		ZEND_ASSERT(next_offset != used_end);

		if (next_offset < used_end) {
			next = ucache_block_ptr_in_hdr(hdr, next_offset);
			next_size = ucache_block_size(next);

			if (ucache_block_is_free(next) &&
				(uint64_t) remainder + next_size <= ucache_block_merge_limit() &&
				ucache_block_size_fits_used(next_offset, next_size, used_end)
			) {
				remainder += next_size;

				ucache_free_list_remove_locked(hdr, next_offset);
			}
		}

		ucache_free_list_insert_locked(hdr, ucache_offset_after(block_offset, total_size), remainder, false);

		block_size = total_size;
	} else {
		ucache_block_set_prev_free_locked(hdr, ucache_offset_after(block_offset, block_size), false);
	}

	block->size = block_size | prev_flag;
	block->owner = owner;

	if (src != NULL) {
		memcpy(ucache_block_payload(block), src, size);
	}

	return ucache_block_payload_offset(block_offset);
}

#if ZEND_DEBUG
void ucache_debug_reserve_data_below_4g_locked(ucache_hdr *hdr)
{
	uint64_t remaining;
	uint32_t block_size;

	if (!UCACHE_DEBUG_FAULT("RESERVE_DATA_BELOW_4G") ||
		UCACHE_DEBUG_RESERVED_DATA_END - hdr->data_offset >= hdr->data_size
	) {
		return;
	}

	remaining = UCACHE_DEBUG_RESERVED_DATA_END - hdr->data_offset - hdr->next_free;
	while (remaining >= UCACHE_BLOCK_MIN_SIZE) {
		block_size = (uint32_t) MIN(remaining, UCACHE_BLOCK_SIZE_MAX) & ~(UCACHE_PLATFORM_ALIGNMENT - 1);

		ucache_alloc_from_tail_locked(hdr, 0, block_size, NULL, UCACHE_BLOCK_OWNER_NONE);

		remaining -= block_size;
	}
}
#endif

uint64_t ucache_committed_bytes_locked(const ucache_hdr *hdr)
{
	return MAX(hdr->committed_end, ucache_view_committed_bytes(&ucache_active_ctx()->storage));
}

uint32_t ucache_free_locked(uint32_t payload_offset)
{
	ucache_hdr *hdr = ucache_hdr_ptr();
	ucache_block *block, *next, *prev;
	uint32_t block_offset, size, next_offset, next_size, prev_offset, prev_size, data_start, used_end,
		merge_limit = ucache_block_merge_limit()
	;

	if (hdr == NULL || payload_offset < UCACHE_BLOCK_HDR_UNITS) {
		return 0;
	}

	block_offset = ucache_payload_block_offset(payload_offset);
	data_start = ucache_data_start_offset(hdr);
	used_end = ucache_used_end_offset_locked(hdr);

	ZEND_ASSERT(ucache_block_offset_in_used(block_offset, data_start, used_end));

	if (!ucache_block_offset_in_used(block_offset, data_start, used_end)) {
		return 0;
	}

	block = ucache_block_ptr_in_hdr(hdr, block_offset);
	if (ucache_block_is_free(block)) {
		return 0;
	}

	size = ucache_block_size(block);

	ZEND_ASSERT(ucache_block_size_fits_used(block_offset, size, used_end));

	if (!ucache_block_size_fits_used(block_offset, size, used_end)) {
		return 0;
	}

	next_offset = ucache_offset_after(block_offset, size);
	next = (ucache_block *) ((uint8_t *) block + size);
	if (next_offset < used_end && ucache_block_is_free(next)) {
		next_size = ucache_block_size(next);

		ZEND_ASSERT(ucache_block_size_fits_used(next_offset, next_size, used_end));

		if ((uint64_t) size + next_size <= merge_limit &&
			ucache_block_size_fits_used(next_offset, next_size, used_end)
		) {
			size += next_size;

			ucache_free_list_remove_locked(hdr, next_offset);
		}
	}

	if (ucache_offset_after(block_offset, size) == used_end) {
		ucache_trim_tail_locked(hdr, block_offset);

		return 0;
	}

	if (ucache_block_prev_is_free(block)) {
		prev_size = ucache_prev_block_size(block);

		ZEND_ASSERT(ucache_prev_block_reachable(block_offset, prev_size, data_start));

		if ((uint64_t) size + prev_size <= merge_limit &&
			ucache_prev_block_reachable(block_offset, prev_size, data_start)
		) {
			prev_offset = block_offset - ucache_offset_from_bytes(prev_size);
			prev = ucache_block_ptr_in_hdr(hdr, prev_offset);

			ZEND_ASSERT(ucache_block_is_free(prev) && ucache_block_size(prev) == prev_size);

			if (ucache_block_is_free(prev) && ucache_block_size(prev) == prev_size) {
				block_offset = prev_offset;
				block = prev;
				size += prev_size;

				ucache_free_list_remove_locked(hdr, block_offset);
			}
		}
	}

	ucache_free_list_insert_locked(hdr, block_offset, size, ucache_block_prev_is_free(block));

	return block_offset;
}

void ucache_shrink_locked(uint32_t payload_offset, size_t payload_size)
{
	ucache_hdr *hdr = ucache_hdr_ptr();
	ucache_block *block, *tail;
	uint32_t block_offset, tail_offset, block_size, used_end;
	size_t total_size = ucache_block_total_size(payload_size);

	if (hdr == NULL || payload_offset < UCACHE_BLOCK_HDR_UNITS) {
		return;
	}

	block_offset = ucache_payload_block_offset(payload_offset);
	used_end = ucache_used_end_offset_locked(hdr);

	ZEND_ASSERT(ucache_block_offset_in_used(block_offset, ucache_data_start_offset(hdr), used_end));

	if (!ucache_block_offset_in_used(block_offset, ucache_data_start_offset(hdr), used_end)) {
		return;
	}

	block = ucache_block_ptr_in_hdr(hdr, block_offset);
	block_size = ucache_block_size(block);

	ZEND_ASSERT(!ucache_block_is_free(block) && ucache_block_size_fits_used(block_offset, block_size, used_end));

	if (ucache_block_is_free(block) ||
		!ucache_block_size_fits_used(block_offset, block_size, used_end) ||
		total_size >= block_size ||
		block_size - total_size < UCACHE_BLOCK_MIN_SIZE
	) {
		return;
	}

	block->size = (uint32_t) total_size | (block->size & UCACHE_BLOCK_PREV_FREE);

	tail_offset = ucache_offset_after(block_offset, total_size);
	tail = ucache_block_ptr_in_hdr(hdr, tail_offset);
	tail->size = block_size - (uint32_t) total_size;
	tail->owner = UCACHE_BLOCK_OWNER_NONE;

	(void) ucache_free_locked(ucache_block_payload_offset(tail_offset));
}

uint32_t ucache_alloc_locked(size_t size, const void *src, uint32_t owner)
{
	ucache_hdr *hdr = ucache_hdr_ptr();
	uint32_t total_size, offset;
	size_t aligned_size;

	if (hdr == NULL || size == 0 || size > UCACHE_BLOCK_SIZE_MAX - sizeof(ucache_block)) {
		return 0;
	}

	aligned_size = ucache_block_total_size(size);
	if (aligned_size > UCACHE_BLOCK_SIZE_MAX) {
		return 0;
	}

	total_size = (uint32_t) aligned_size;

	offset = ucache_alloc_from_free_list_locked(
		hdr,
		size,
		total_size,
		src,
		owner
	);
	if (offset != 0) {
		return offset;
	}

	return ucache_alloc_from_tail_locked(
		hdr,
		size,
		total_size,
		src,
		owner
	);
}

bool ucache_alloc_can_satisfy_locked(size_t size)
{
	ucache_hdr *hdr = ucache_hdr_ptr();
	size_t total, tail;

	if (hdr == NULL || size == 0 || size > UCACHE_BLOCK_SIZE_MAX - sizeof(ucache_block)) {
		return false;
	}

	total = ucache_block_total_size(size);
	if (total > UCACHE_BLOCK_SIZE_MAX) {
		return false;
	}

	tail = hdr->next_free <= ucache_data_tail_limit_locked(hdr)
		? ucache_data_tail_limit_locked(hdr) - hdr->next_free
		: 0
	;

	return tail >= total || ucache_free_block_at_least_locked(hdr, (uint32_t) total) != 0;
}
