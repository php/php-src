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

#ifndef UCACHE_ENTRIES_H
#define UCACHE_ENTRIES_H

#include "user_cache_internal.h"

#define UCACHE_EXPIRED_READ_EXPUNGE_THRESHOLD	64U

typedef struct {
	uint64_t gen;
	const ucache_ctx *ctx;
	bool needs_deep_clone;
	bool has_clone_verdicts;
	bool no_aliases;
	bool has_val;
	bool proto_rejected;
	bool released_while_cloning;
	uint16_t clone_depth;
	zval val;
} ucache_req_local_slot;

typedef struct {
	ucache_req_local_slot slot;
	HashTable clone_verdicts;
} ucache_req_local_slot_with_verdicts;

typedef struct {
	bool expired;
	bool slot_pressure_evict;
	bool mem_pressure_evict;
	bool clear;
	bool room_checked;
	bool room_after_clear;
	uint32_t room_entry_lock_count;
} ucache_store_retries;

ucache_req_local_slot *ucache_find_req_local_slot(
		zend_string *key,
		uint64_t gen);
void ucache_req_local_slot_free(ucache_req_local_slot *slot);
bool ucache_clone_req_local_slot_val_known(
		zval *dst,
		zval *src,
		HashTable *verdicts,
		bool no_aliases,
		bool *proto_rejected);
void ucache_req_local_slots_drop_oldest_half(void);
void ucache_store_req_local_slot(
		zend_string *key,
		uint64_t gen,
		zval *val,
		bool no_aliases,
		uint32_t charged_bytes);
void ucache_mark_req_local_slot(zend_string *key, uint64_t gen);
void ucache_expunge_expired_bounded_locked(void);
bool ucache_reclaim_space_for_store_locked(
		bool fits_in_empty_cache,
		size_t needed_size,
		ucache_store_retries *retries);

static zend_always_inline bool ucache_entry_is_tombstone(const ucache_entry *entry)
{
	return ucache_entry_kind(entry) == UCACHE_ENTRY_TOMBSTONE;
}

static zend_always_inline void ucache_entry_set_kind(ucache_entry *entry, uint32_t kind)
{
	entry->flags = (uint16_t) ((entry->flags & ~UCACHE_ENTRY_KIND_MASK) | (kind << UCACHE_ENTRY_KIND_SHIFT));
}

static zend_always_inline void ucache_entry_set_tombstone(ucache_entry *entry)
{
	ucache_entry_set_kind(entry, UCACHE_ENTRY_TOMBSTONE);
}

static zend_always_inline bool ucache_record_val_fits(size_t bytes)
{
	size_t budget = ucache_req_cache_budget();

	return bytes <= UINT32_MAX &&
		UC_G(record_val_bytes) <= budget &&
		bytes <= budget - UC_G(record_val_bytes)
	;
}

static zend_always_inline uint32_t ucache_entry_pool_bucket(const ucache_entry *entry)
{
	return (entry->flags & UCACHE_ENTRY_POOL_BUCKET_MASK)
		>> UCACHE_ENTRY_POOL_BUCKET_SHIFT
	;
}

static zend_always_inline uint32_t ucache_pool_link_ref_slot(uint32_t ref)
{
	ZEND_ASSERT(ref != 0);

	return ref - 1;
}

static zend_always_inline void ucache_pool_idx_unlink_locked(
		ucache_hdr *hdr,
		ucache_entry *entry,
		uint32_t slot)
{
	ucache_pool_links *links = ucache_pool_links_ptr(hdr), *link = &links[slot];
	uint32_t bucket = ucache_entry_pool_bucket(entry);

	if (link->prev != 0) {
		links[ucache_pool_link_ref_slot(link->prev)].next = link->next;
	} else {
		hdr->pool_bucket_heads[bucket] = link->next;
	}

	if (link->next != 0) {
		links[ucache_pool_link_ref_slot(link->next)].prev = link->prev;
	}

	link->prev = 0;
	link->next = 0;

	ucache_pool_bucket_changed_locked(hdr, bucket);
}

static zend_always_inline uint32_t ucache_release_val_storage_locked(uint8_t val_type, uint32_t val_offset)
{
	bool gquiescent;

	if (val_offset == 0) {
		return 0;
	}

	switch (val_type) {
		case UCACHE_VAL_SGRAPH:
			gquiescent = ucache_quiesce_graph_payloads_locked();

			if (!ucache_sgraph_retire_payload_locked(val_offset)) {
				return 0;
			}

			if (!gquiescent) {
				ucache_sgraph_orphan_payload_locked(val_offset);

				return 0;
			}

			return ucache_free_locked(val_offset);
		case UCACHE_VAL_STR:
			return ucache_free_locked(val_offset);
		default:
			return 0;
	}
}

static zend_always_inline uint32_t ucache_release_entry_storage_locked(ucache_entry *entry)
{
	uint32_t val_offset, key_region = 0, val_region = 0;
	bool combined;

	combined = (entry->flags & UCACHE_ENTRY_FLAG_COMBINED_VAL_KEY) != 0;

	if (entry->key_offset != 0 && !combined) {
		key_region = ucache_free_locked(entry->key_offset);
	}

	val_offset = ucache_entry_val_offset(entry);
	if (val_offset != 0) {
		val_region = ucache_release_val_storage_locked(
			ucache_entry_val_type(entry),
			val_offset
		);
	}

	return val_region != 0
		? val_region
		: key_region
	;
}

static zend_always_inline uint32_t ucache_delete_entry_locked(
		ucache_hdr *hdr,
		ucache_entry *entry,
		uint32_t slot)
{
	uint32_t region;

	if (ucache_entry_is_used(entry)) {
		ucache_pool_idx_unlink_locked(hdr, entry, slot);

		if (hdr->count != 0) {
			hdr->count--;

			if (entry->expires_at != 0) {
				hdr->expiring_count--;
			}
		}
	}

	if (!ucache_entry_is_tombstone(entry)) {
		hdr->tombstone_count++;
	}

	region = ucache_release_entry_storage_locked(entry);

	entry->hash = 0;
	entry->key_offset = 0;
	entry->key_len = 0;
	entry->flags = 0;

	ucache_entry_set_tombstone(entry);

	entry->expires_at = 0;
	entry->gen = 0;
	entry->double_val = 0;

	ucache_bump_mutation_epoch_locked(hdr);

	return region;
}

static zend_always_inline void ucache_release_req_local_slot_table(HashTable **slots_ptr)
{
	HashTable *slots = *slots_ptr;

	if (slots == NULL) {
		return;
	}

	*slots_ptr = NULL;

	UC_G(req_local_slot_owner_pid) = 0;

	zend_hash_destroy(slots);

	FREE_HASHTABLE(slots);
}

static zend_always_inline void ucache_req_local_slots_check_fork(void)
{
	if (UC_G(req_local_slot_table) != NULL &&
		UNEXPECTED(UC_G(req_local_slot_owner_pid) != ucache_cached_pid())
	) {
		ucache_release_req_local_slot_table(&UC_G(req_local_slot_table));
	}
}

static zend_always_inline bool ucache_is_expired(const ucache_entry *entry, uint64_t now_rel)
{
	return ucache_entry_is_used(entry) &&
		entry->expires_at != 0 &&
		(uint64_t) entry->expires_at <= now_rel
	;
}

static zend_always_inline void ucache_access_note_time(uint64_t now)
{
	uint32_t seconds = (uint32_t) (now / UCACHE_CLOCK_TICKS_PER_SEC);

	if (seconds != 0) {
		UC_G(access_now) = seconds;
		UC_G(access_now_touches) = 0;
	}
}

static zend_always_inline bool ucache_payload_can_fit_locked(size_t size)
{
	ucache_hdr *hdr = ucache_hdr_ptr();
	size_t total_size;

	if (
		!hdr ||
		size == 0 ||
		size > UCACHE_BLOCK_SIZE_MAX - sizeof(ucache_block)
	) {
		return false;
	}

	total_size = ucache_block_total_size(size);

	return total_size <= UCACHE_BLOCK_SIZE_MAX && total_size <= hdr->data_size;
}

static zend_always_inline HashTable *ucache_req_local_slot_verdicts(ucache_req_local_slot *slot)
{
	ZEND_ASSERT(slot->needs_deep_clone);

	return &((ucache_req_local_slot_with_verdicts *) slot)->clone_verdicts;
}

#endif /* UCACHE_ENTRIES_H */
