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

#define UCACHE_ENTRY_LOCK_SWEEP_INTERVAL_TICKS	UCACHE_CLOCK_TICKS_PER_SEC
#define UCACHE_DEFERRED_RELEASE_RETRY_CTXS		16U

#ifdef ZTS
static ucache_entry_lock *ucache_orphaned_entry_lock_releases = NULL;
static atomic_bool ucache_orphaned_entry_lock_releases_pending = false;
#endif

static zend_always_inline bool ucache_entry_lock_record_key_matches(
		const ucache_entry_lock_record *record,
		const char *key,
		size_t key_len,
		zend_ulong hash)
{
	return record->state == UCACHE_ENTRY_LOCK_USED &&
		record->hash == hash &&
		record->key_len == key_len &&
		memcmp(ucache_ptr(record->key_offset), key, key_len) == 0
	;
}

static zend_always_inline void ucache_entry_lock_record_disown_locked(ucache_entry_lock_record *record)
{
	record->owner_pid = 0;
	record->owner_start_time = 0;
	record->owner_token = 0;
}

static bool ucache_entry_lock_record_is_stale(const ucache_entry_lock_record *record, uint64_t now_rel)
{
	if (record->expires_at != 0) {
		return (uint64_t) record->expires_at <= now_rel;
	}

	return record->owner_pid == 0;
}

static uint32_t ucache_entry_lock_expires_at(
		const ucache_hdr *hdr,
		zend_long lease)
{
	ZEND_ASSERT(lease > 0);

	return ucache_expiry_deadline(hdr, ucache_clock_now(), lease);
}

static void ucache_remove_entry_lock_record_locked(
		ucache_entry_lock_record *record)
{
	ucache_hdr *hdr = ucache_hdr_ptr();

	ZEND_ASSERT(record->state == UCACHE_ENTRY_LOCK_USED);
	ZEND_ASSERT(hdr->entry_lock_count > 0);

	if (record->key_offset != 0) {
		ucache_free_locked(record->key_offset);
	}

	hdr->entry_lock_count--;
	hdr->entry_lock_tombstone_count++;

	memset(record, 0, sizeof(*record));

	record->state = UCACHE_ENTRY_LOCK_TOMBSTONE;
}

static void ucache_rehash_entry_locks_locked(ucache_hdr *hdr)
{
	ucache_entry_lock_record *records, copy;
	uint32_t mask = hdr->entry_lock_capacity - 1, idx, slot;
	bool moved;

	if (hdr->entry_lock_tombstone_count < hdr->entry_lock_capacity / 4) {
		return;
	}

	records = ucache_entry_lock_records_ptr(hdr);
	if (hdr->entry_lock_count == 0) {
		memset(records, 0, (size_t) hdr->entry_lock_capacity * sizeof(*records));

		hdr->entry_lock_tombstone_count = 0;

		return;
	}

	for (idx = 0; idx < hdr->entry_lock_capacity; idx++) {
		if (records[idx].state == UCACHE_ENTRY_LOCK_TOMBSTONE) {
			memset(&records[idx], 0, sizeof(records[idx]));
		}
	}

	hdr->entry_lock_tombstone_count = 0;

	do {
		moved = false;

		for (idx = 0; idx < hdr->entry_lock_capacity; idx++) {
			if (records[idx].state != UCACHE_ENTRY_LOCK_USED) {
				continue;
			}

			for (slot = ucache_entry_lock_table_idx(hdr, records[idx].hash); slot != idx; slot = (slot + 1) & mask) {
				if (records[slot].state != UCACHE_ENTRY_LOCK_EMPTY) {
					continue;
				}

				copy = records[idx];
				copy.state = UCACHE_ENTRY_LOCK_EMPTY;
				records[slot] = copy;

				UCACHE_ATOMIC_FENCE_SEQ_CST();
				records[slot].state = UCACHE_ENTRY_LOCK_USED;
				UCACHE_ATOMIC_FENCE_SEQ_CST();

				UCACHE_DEBUG_SIMULATE_KILL("EXIT_IN_ENTRY_LOCK_REHASH");

				memset(&records[idx], 0, sizeof(records[idx]));

				moved = true;

				break;
			}
		}
	} while (moved);
}

static bool ucache_find_entry_lock_record_slot_raw_locked(
		ucache_hdr *hdr,
		const char *key,
		size_t key_len,
		zend_ulong hash,
		bool inserting,
		uint32_t *slot_idx,
		bool *found)
{
	ucache_entry_lock_record *record;
	uint64_t now = 0;
	uint32_t first_avail = UINT32_MAX, i, probe;
	bool swept = false;

	*found = false;

	ucache_rehash_entry_locks_locked(hdr);

	if (hdr->entry_lock_count == 0) {
		*slot_idx = ucache_entry_lock_table_idx(hdr, hash);

		return true;
	}

	for (;;) {
		for (probe = 0; probe < hdr->entry_lock_capacity; probe++) {
			i = (ucache_entry_lock_table_idx(hdr, hash) + probe) & (hdr->entry_lock_capacity - 1);
			record = &ucache_entry_lock_records_ptr(hdr)[i];

			if (record->state == UCACHE_ENTRY_LOCK_EMPTY) {
				*slot_idx = first_avail != UINT32_MAX ? first_avail : i;

				return true;
			}

			if (record->state == UCACHE_ENTRY_LOCK_TOMBSTONE) {
				if (first_avail == UINT32_MAX) {
					first_avail = i;
				}

				continue;
			}

			if (now == 0) {
				now = ucache_time_rel(hdr, ucache_clock_now());
			}

			if (ucache_entry_lock_record_key_matches(record, key, key_len, hash)) {
				if (ucache_entry_lock_record_is_active_locked(record, now)) {
					*slot_idx = i;
					*found = true;

					return true;
				}
			} else if (!ucache_entry_lock_record_is_stale(record, now)) {
				continue;
			}

			ucache_remove_entry_lock_record_locked(record);

			if (first_avail == UINT32_MAX) {
				first_avail = i;
			}
		}

		if (first_avail != UINT32_MAX) {
			*slot_idx = first_avail;

			return true;
		}

		if (!inserting || swept) {
			return false;
		}

		swept = true;

		ucache_sweep_entry_locks_locked(hdr, now);
	}
}

static bool ucache_find_entry_lock_record_insert_slot_locked(
		ucache_hdr *hdr,
		zend_string *key,
		zend_ulong hash,
		uint32_t *slot_idx,
		bool *found)
{
	return ucache_find_entry_lock_record_slot_raw_locked(
		hdr, ZSTR_VAL(key), ZSTR_LEN(key), hash, true, slot_idx, found
	);
}

static bool ucache_insert_entry_lock_record_locked(
		ucache_hdr *hdr,
		uint32_t slot_idx,
		zend_string *key,
		zend_ulong hash,
		ucache_entry_lock *lock)
{
	ucache_entry_lock_record *record = &ucache_entry_lock_records_ptr(hdr)[slot_idx];
	uint64_t owner_pid;
	uint32_t key_offset;

	if (ZSTR_LEN(key) > UINT32_MAX) {
		return false;
	}

	key_offset = ucache_alloc_locked(ZSTR_LEN(key), ZSTR_VAL(key), UCACHE_BLOCK_OWNER_NONE);
	if (key_offset == 0 && hdr->entry_lock_count != 0) {
		ucache_sweep_entry_locks_locked(hdr, ucache_time_rel(hdr, ucache_clock_now()));

		key_offset = ucache_alloc_locked(ZSTR_LEN(key), ZSTR_VAL(key), UCACHE_BLOCK_OWNER_NONE);
	}

	if (key_offset == 0) {
		ucache_write_section_announce(hdr);

		UCACHE_DEBUG_SIMULATE_KILL("EXIT_IN_ENTRY_LOCK_EVICTION");

		if (ucache_reclaim_space_for_entry_lock_locked(ZSTR_LEN(key))) {
			key_offset = ucache_alloc_locked(
				ZSTR_LEN(key),
				ZSTR_VAL(key),
				UCACHE_BLOCK_OWNER_NONE
			);
		}
	}

	if (key_offset == 0) {
		return false;
	}

	owner_pid = ucache_cached_pid();

	if (record->state == UCACHE_ENTRY_LOCK_TOMBSTONE) {
		hdr->entry_lock_tombstone_count--;
	}

	hdr->entry_lock_count++;

	memset(record, 0, sizeof(*record));

	record->hash = hash;
	record->owner_pid = owner_pid;
	record->owner_start_time = ucache_cached_self_start_time_token(owner_pid);
	record->owner_token = ++hdr->entry_lock_acquire_seq;
	record->expires_at = lock->lease > 0 ? ucache_entry_lock_expires_at(hdr, lock->lease) : 0;
	record->key_offset = key_offset;
	record->key_len = (uint32_t) ZSTR_LEN(key);
	record->state = UCACHE_ENTRY_LOCK_USED;

	lock->owner_token = record->owner_token;
	lock->owner_start_time = record->owner_start_time;

	return true;
}

static bool ucache_update_entry_lock_record_lease_locked(
		ucache_hdr *hdr,
		zend_string *key,
		const ucache_entry_lock *lock,
		zend_long lease)
{
	ucache_entry_lock_record *record;
	zend_ulong hash;
	uint32_t expires_at, slot_idx;
	bool found;

	hash = zend_string_hash_val(key);
	if (!ucache_find_entry_lock_record_slot_locked(
			hdr,
			key,
			hash,
			&slot_idx,
			&found
		) || !found
	) {
		return false;
	}

	record = &ucache_entry_lock_records_ptr(hdr)[slot_idx];
	if (record->owner_pid != lock->owner_pid || record->owner_token != lock->owner_token) {
		return false;
	}

	if (lease > 0 && record->expires_at != 0) {
		expires_at = ucache_entry_lock_expires_at(hdr, lease);
		if (record->expires_at < expires_at) {
			record->expires_at = expires_at;
		}
	}

	return true;
}

static ucache_entry_lock *ucache_create_local_entry_lock(
		ucache_ctx *ctx,
		zend_string *key,
		zend_long lease)
{
	ucache_entry_lock *lock;

	if (ZSTR_LEN(key) > SIZE_MAX - sizeof(*lock) || ZSTR_LEN(key) > UINT32_MAX) {
		return NULL;
	}

	lock = UC_G(entry_lock_spare);
	if (lock != NULL) {
		UC_G(entry_lock_spare) = NULL;

		if (lock->key_len < ZSTR_LEN(key)) {
			free(lock);

			lock = NULL;
		}
	}

	if (lock == NULL) {
		lock = malloc(sizeof(*lock) + ZSTR_LEN(key));
		if (lock == NULL) {
			return NULL;
		}
	}

	lock->ctx = ctx;
	lock->next = NULL;
	lock->owner_pid = ucache_cached_pid();
	lock->owner_start_time = 0;
	lock->owner_token = 0;
	lock->lease = lease;
	lock->key_len = (uint32_t) ZSTR_LEN(key);
	lock->requested_by_lock = false;

	memcpy(lock->key, ZSTR_VAL(key), ZSTR_LEN(key) + 1);

	return lock;
}

static void ucache_defer_entry_lock_release(ucache_entry_lock *lock)
{
	lock->next = UC_G(deferred_entry_lock_releases);

	UC_G(deferred_entry_lock_releases) = lock;
}

static void ucache_free_entry_lock_list(ucache_entry_lock *entry)
{
	ucache_entry_lock *next;

	while (entry != NULL) {
		next = entry->next;
		free(entry);
		entry = next;
	}
}

#ifdef ZTS
static void ucache_adopt_orphaned_entry_lock_releases(void)
{
	ucache_entry_lock *entry, *next;

	if (!atomic_load(&ucache_orphaned_entry_lock_releases_pending)) {
		return;
	}

	ucache_boundary_partitions_lock();

	entry = ucache_orphaned_entry_lock_releases;
	ucache_orphaned_entry_lock_releases = NULL;

	atomic_store(&ucache_orphaned_entry_lock_releases_pending, false);

	ucache_boundary_partitions_unlock();

	while (entry != NULL) {
		next = entry->next;

		ucache_defer_entry_lock_release(entry);

		entry = next;
	}
}
#endif

static bool ucache_add_local_entry_lock(
		HashTable *locks,
		zend_string *key,
		ucache_entry_lock *lock)
{
	bool added = false;

	zend_try {
		added = zend_hash_add_ptr(locks, key, lock) != NULL;
	} zend_catch {
		lock->lease = 0;

		ucache_defer_entry_lock_release(lock);

		zend_bailout();
	} zend_end_try();

	return added;
}

static void ucache_release_entry_lock_record_locked(
		ucache_hdr *hdr,
		const ucache_entry_lock *lock,
		zend_ulong hash)
{
	ucache_entry_lock_record *record;
	uint32_t slot_idx;
	bool found;

	if (!ucache_find_entry_lock_record_slot_raw_locked(
			hdr, lock->key, lock->key_len, hash, false, &slot_idx, &found
		) ||
		!found
	) {
		return;
	}

	record = &ucache_entry_lock_records_ptr(hdr)[slot_idx];
	if (record->owner_pid != lock->owner_pid ||
		record->owner_start_time != lock->owner_start_time ||
		record->owner_token != lock->owner_token
	) {
		return;
	}

	if (lock->lease > 0 && record->expires_at != 0) {
		ucache_entry_lock_record_disown_locked(record);
	} else {
		ucache_remove_entry_lock_record_locked(record);
	}
}

static bool ucache_ctx_listed(
		ucache_ctx *const *ctxs,
		uint32_t count,
		const ucache_ctx *ctx)
{
	uint32_t i;

	for (i = 0; i < count; i++) {
		if (ctxs[i] == ctx) {
			return true;
		}
	}

	return false;
}

static void ucache_destroy_entry_locks_if_empty(HashTable **locks_ptr)
{
	if (*locks_ptr != NULL && zend_hash_num_elements(*locks_ptr) == 0) {
		zend_hash_destroy(*locks_ptr);

		FREE_HASHTABLE(*locks_ptr);

		*locks_ptr = NULL;
	}
}

static void ucache_release_entry_locks_for_ctx(ucache_ctx *ctx)
{
	ucache_ctx *prev_ctx;
	ucache_entry_lock *lock;
	ucache_hdr *hdr;
	HashTable **locks_ptr = &UC_G(entry_lock_table);
	zend_string *key;
	zval *entry;
	bool released_to_shm = false;

	if (*locks_ptr == NULL) {
		return;
	}

	if (zend_hash_num_elements(*locks_ptr) == 0) {
		zend_hash_destroy(*locks_ptr);

		FREE_HASHTABLE(*locks_ptr);

		*locks_ptr = NULL;

		return;
	}

	prev_ctx = ucache_activate_ctx(ctx);
	if (ucache_wlock_entry_lock_table()) {
		hdr = ucache_hdr_ptr();
		if (ucache_hdr_init_locked()) {
			ZEND_HASH_FOREACH_STR_KEY_PTR(*locks_ptr, key, lock) {
				if (key != NULL && lock != NULL && lock->ctx == ctx) {
					ucache_release_entry_lock_record_locked(hdr, lock, zend_string_hash_val(key));
				}
			} ZEND_HASH_FOREACH_END();

			released_to_shm = true;
		}

		ucache_unlock();
	}

	ucache_restore_ctx(prev_ctx);

	ZEND_HASH_FOREACH_STR_KEY_VAL(*locks_ptr, key, entry) {
		lock = Z_PTR_P(entry);

		if (key == NULL || lock == NULL || lock->ctx != ctx) {
			continue;
		}

		if (!released_to_shm) {
			ucache_defer_entry_lock_release(lock);

			ZVAL_PTR(entry, NULL);
		}

		zend_hash_del(*locks_ptr, key);
	} ZEND_HASH_FOREACH_END();

	ucache_destroy_entry_locks_if_empty(locks_ptr);
}

static void ucache_release_entry_locks_all_ctxs(void)
{
	ucache_ctx *ctx;
	ucache_entry_lock *lock;

	while (UC_G(entry_lock_table) != NULL) {
		ctx = ucache_owning_ctx();

		ZEND_HASH_FOREACH_PTR(UC_G(entry_lock_table), lock) {
			ctx = lock->ctx;

			break;
		} ZEND_HASH_FOREACH_END();

		ucache_release_entry_locks_for_ctx(ctx);
	}
}

static void ucache_entry_lock_dtor(zval *lock_zv)
{
	ucache_entry_lock *lock = Z_PTR_P(lock_zv);

	if (lock != NULL) {
		if (UC_G(entry_lock_spare) == NULL && lock->key_len <= 256) {
			UC_G(entry_lock_spare) = lock;
		} else {
			free(lock);
		}
	}
}

static HashTable *ucache_prepare_entry_locks_for_insert(void)
{
	HashTable *locks = UC_G(entry_lock_table);

	if (locks == NULL) {
		ALLOC_HASHTABLE(locks);

		zend_hash_init(locks, 0, NULL, ucache_entry_lock_dtor, 0);

		UC_G(entry_lock_table) = locks;
	}

	zend_hash_extend(locks, zend_hash_num_elements(locks) + 1, 0);

	return locks;
}

static bool ucache_upgrade_held_entry_lock_or_drop_if_lost(
		zend_string *key,
		zend_long lease,
		ucache_entry_lock *lock)
{
	bool updated;

	if (!ucache_wlock_entry_lock_table()) {
		return false;
	}

	updated = ucache_hdr_init_locked() &&
		ucache_update_entry_lock_record_lease_locked(ucache_hdr_ptr(), key, lock, lease)
	;

	ucache_unlock();

	if (!updated) {
		zend_hash_del(UC_G(entry_lock_table), key);
		ucache_destroy_entry_locks_if_empty(&UC_G(entry_lock_table));

		return false;
	}

	if (lease > lock->lease) {
		lock->lease = lease;
	}

	return true;
}

static bool ucache_remove_owned_entry_lock_record(
		zend_string *key,
		zend_ulong hash,
		uint64_t owner_pid,
		uint64_t owner_token,
		bool *removed)
{
	ucache_hdr *hdr;
	ucache_entry_lock_record *record;
	uint32_t slot_idx;
	bool found;

	if (!ucache_wlock_entry_lock_table()) {
		return false;
	}

	hdr = ucache_hdr_ptr();
	*removed = false;

	if (ucache_hdr_is_initialized_locked() &&
		ucache_find_entry_lock_record_slot_locked(
			hdr,
			key,
			hash,
			&slot_idx,
			&found
		) &&
		found
	) {
		record = &ucache_entry_lock_records_ptr(hdr)[slot_idx];

		if (record->owner_pid == owner_pid && record->owner_token == owner_token) {
			ucache_remove_entry_lock_record_locked(record);

			*removed = true;
		}
	}

	ucache_unlock();

	return true;
}

static bool ucache_acquire_entry_lock_record(
		zend_string *key,
		zend_long lease,
		bool blocking)
{
	ucache_ctx *ctx = ucache_active_ctx();
	ucache_entry_lock *lock;
	ucache_entry_lock_holder holder;
	ucache_hdr *hdr;
	zend_ulong hash = zend_string_hash_val(key);
	HashTable *locks, **locks_ptr = &UC_G(entry_lock_table);
	uint64_t waited_us = 0;
	uint32_t slot_idx;
	bool found, inserted, insert_failed, removed;

	ucache_ensure_entry_lock_owner();

	ucache_drain_deferred_entry_lock_releases();

	lock = ucache_find_local_entry_lock(ctx, key);
	if (lock != NULL) {
		if (ucache_upgrade_held_entry_lock_or_drop_if_lost(key, lease, lock)) {
			return true;
		}

		if (ucache_find_local_entry_lock(ctx, key) != NULL) {
			return false;
		}
	}

	locks = ucache_prepare_entry_locks_for_insert();
	lock = ucache_create_local_entry_lock(ctx, key, lease);
	if (lock == NULL) {
		ucache_destroy_entry_locks_if_empty(locks_ptr);

		return false;
	}

	for (;;) {
		inserted = false;
		found = false;
		insert_failed = false;

		if (!ucache_wlock_entry_lock_table()) {
			free(lock);

			ucache_destroy_entry_locks_if_empty(locks_ptr);

			return false;
		}

		hdr = ucache_hdr_ptr();

		if (!ucache_hdr_init_locked()) {
			ucache_unlock();

			free(lock);

			ucache_destroy_entry_locks_if_empty(locks_ptr);

			return false;
		}

		if (ucache_find_entry_lock_record_insert_slot_locked(hdr, key, hash, &slot_idx, &found)) {
			if (found) {
				ucache_entry_lock_holder_capture(&holder, hdr, slot_idx);
			} else if (!(inserted = ucache_insert_entry_lock_record_locked(
					hdr,
					slot_idx,
					key,
					hash,
					lock
				))
			) {
				insert_failed = true;
			}
		} else {
			insert_failed = true;
		}

		ucache_unlock();

		if (inserted) {
			if (ucache_add_local_entry_lock(locks, key, lock)) {
				return true;
			}

			if (!ucache_remove_owned_entry_lock_record(
					key,
					hash,
					lock->owner_pid,
					lock->owner_token,
					&removed
				)
			) {
				lock->lease = 0;

				ucache_defer_entry_lock_release(lock);
			} else {
				free(lock);
			}

			ucache_destroy_entry_locks_if_empty(locks_ptr);

			return false;
		}

		if (insert_failed) {
			free(lock);

			ucache_destroy_entry_locks_if_empty(locks_ptr);

			return false;
		}

		if (!blocking || waited_us >= ucache_entry_lock_wait_timeout_us()) {
			free(lock);

			ucache_destroy_entry_locks_if_empty(locks_ptr);

			return false;
		}

		do {
			waited_us += ucache_sleep_entry_lock_retry_interval(waited_us);
		} while (found &&
			waited_us < ucache_entry_lock_wait_timeout_us() &&
			ucache_entry_lock_holder_unchanged(&ctx->storage, &holder)
		);
	}
}

bool ucache_entry_lock_record_is_active_locked(
		ucache_entry_lock_record *record,
		uint64_t now_rel)
{
	if (record->state != UCACHE_ENTRY_LOCK_USED) {
		return false;
	}

	if (record->expires_at != 0 && (uint64_t) record->expires_at <= now_rel) {
		return false;
	}

	if (record->owner_pid == 0) {
		return record->expires_at != 0;
	}

	if (ucache_owner_is_dead(record->owner_pid, record->owner_start_time)) {
		ucache_entry_lock_record_disown_locked(record);

		return record->expires_at != 0;
	}

	return true;
}

void ucache_sweep_entry_locks_locked(ucache_hdr *hdr, uint64_t now)
{
	ucache_entry_lock_record *record;
	uint32_t i;

	hdr->entry_lock_sweep_at = (uint32_t) MIN(now + UCACHE_ENTRY_LOCK_SWEEP_INTERVAL_TICKS, UINT32_MAX);

	for (i = 0; i < hdr->entry_lock_capacity && hdr->entry_lock_count != 0; i++) {
		record = &ucache_entry_lock_records_ptr(hdr)[i];
		if (record->state == UCACHE_ENTRY_LOCK_USED &&
			!ucache_entry_lock_record_is_active_locked(record, now)
		) {
			ucache_remove_entry_lock_record_locked(record);
		}
	}
}

bool ucache_find_entry_lock_record_slot_locked(
		ucache_hdr *hdr,
		zend_string *key,
		zend_ulong hash,
		uint32_t *slot_idx,
		bool *found)
{
	return ucache_find_entry_lock_record_slot_raw_locked(
		hdr, ZSTR_VAL(key), ZSTR_LEN(key), hash, false, slot_idx, found
	);
}

void ucache_entry_lock_holder_capture(
		ucache_entry_lock_holder *holder,
		ucache_hdr *hdr,
		uint32_t slot_idx)
{
	const ucache_entry_lock_record *record = &ucache_entry_lock_records_ptr(hdr)[slot_idx];

	holder->owner_pid = record->owner_pid;
	holder->owner_start_time = record->owner_start_time;
	holder->owner_token = record->owner_token;
	holder->hash = record->hash;
	holder->slot_idx = slot_idx;
	holder->expires_at = record->expires_at;
}

bool ucache_entry_lock_holder_unchanged(
		const ucache_storage *storage,
		const ucache_entry_lock_holder *holder)
{
	const ucache_hdr *hdr = (const ucache_hdr *) storage->base;
	const ucache_entry_lock_record *record;

	if (hdr == NULL ||
		!storage->layout_memo_valid ||
		holder->slot_idx >= storage->entry_lock_capacity_memo
	) {
		return false;
	}

	record = (const ucache_entry_lock_record *) (
		(const char *) hdr + storage->entry_lock_offset_memo
	) + holder->slot_idx;

	if (record->state != UCACHE_ENTRY_LOCK_USED ||
		record->hash != holder->hash ||
		record->owner_token != holder->owner_token ||
		record->owner_pid != holder->owner_pid ||
		record->expires_at != holder->expires_at
	) {
		return false;
	}

	if (holder->expires_at != 0 &&
		(uint64_t) holder->expires_at <= ucache_time_rel(hdr, ucache_clock_now())
	) {
		return false;
	}

	if (holder->owner_pid == 0) {
		return holder->expires_at != 0;
	}

	return !ucache_owner_is_dead(holder->owner_pid, holder->owner_start_time);
}

void ucache_drain_deferred_entry_lock_releases(void)
{
	ucache_ctx *ctx;
	ucache_entry_lock **link = &UC_G(deferred_entry_lock_releases), *entry, *local_lock;
	ucache_hdr *hdr = NULL;
	uint64_t self_pid;
	bool drainable = false;

	if (*link == NULL) {
		return;
	}

	ctx = ucache_active_ctx();
	self_pid = ucache_cached_pid();

	while ((entry = *link) != NULL) {
		if (entry->owner_pid != self_pid) {
			*link = entry->next;

			free(entry);

			continue;
		}

		drainable = drainable || entry->ctx == ctx;
		link = &entry->next;
	}

	if (!drainable || !ucache_wlock_entry_lock_table()) {
		return;
	}

	if (ucache_hdr_is_initialized_locked()) {
		hdr = ucache_hdr_ptr();
	}

	link = &UC_G(deferred_entry_lock_releases);
	while ((entry = *link) != NULL) {
		if (entry->ctx != ctx) {
			link = &entry->next;

			continue;
		}

		local_lock = UC_G(entry_lock_table) != NULL
			? zend_hash_str_find_ptr(UC_G(entry_lock_table), entry->key, entry->key_len)
			: NULL
		;

		if (hdr != NULL && (local_lock == NULL || local_lock->ctx != ctx)) {
			ucache_release_entry_lock_record_locked(
				hdr, entry, zend_hash_func(entry->key, entry->key_len)
			);
		}

		*link = entry->next;

		free(entry);
	}

	ucache_unlock();
}

void ucache_ensure_entry_lock_owner(void)
{
#ifndef ZEND_WIN32
	zend_ulong cur_pid = (zend_ulong) ucache_cached_pid();

	if (UC_G(entry_lock_owner_pid) == 0) {
		UC_G(entry_lock_owner_pid) = cur_pid;

		return;
	}

	if (UC_G(entry_lock_owner_pid) == cur_pid) {
		return;
	}

	if (UC_G(entry_lock_table) != NULL) {
		zend_hash_destroy(UC_G(entry_lock_table));

		FREE_HASHTABLE(UC_G(entry_lock_table));

		UC_G(entry_lock_table) = NULL;
	}

	UC_G(entry_lock_owner_pid) = cur_pid;
#endif
}

bool ucache_try_acquire_entry_lock(zend_string *key, zend_long lease)
{
	ucache_entry_lock *lock;

	if (!ucache_acquire_entry_lock_record(key, lease, false)) {
		return false;
	}

	lock = ucache_find_local_entry_lock(ucache_active_ctx(), key);
	if (lock != NULL) {
		lock->requested_by_lock = true;
	}

	return true;
}

bool ucache_acquire_entry_lock(zend_string *key)
{
	return ucache_acquire_entry_lock_record(key, 0, true);
}

bool ucache_release_entry_lock(zend_string *key)
{
	ucache_ctx *ctx = ucache_active_ctx();
	ucache_entry_lock *lock;
	bool still_owned;

	ucache_ensure_entry_lock_owner();

	lock = ucache_find_local_entry_lock(ctx, key);
	if (lock == NULL) {
		return false;
	}

	if (!ucache_remove_owned_entry_lock_record(
			key,
			zend_string_hash_val(key),
			lock->owner_pid,
			lock->owner_token,
			&still_owned
		)
	) {
		return false;
	}

	zend_hash_del(UC_G(entry_lock_table), key);

	ucache_destroy_entry_locks_if_empty(&UC_G(entry_lock_table));

	return still_owned;
}

bool ucache_release_entry_lock_unless_requested(zend_string *key)
{
	ucache_entry_lock *lock;

	ucache_ensure_entry_lock_owner();

	lock = ucache_find_local_entry_lock(ucache_active_ctx(), key);
	if (lock != NULL && lock->requested_by_lock) {
		return true;
	}

	return ucache_release_entry_lock(key);
}

bool ucache_req_owns_entry_lock(zend_string *key)
{
	ucache_ctx *ctx = ucache_active_ctx();
	ucache_entry_lock *lock;

	ucache_ensure_entry_lock_owner();

	lock = ucache_find_local_entry_lock(ctx, key);
	if (lock == NULL) {
		return false;
	}

	return ucache_upgrade_held_entry_lock_or_drop_if_lost(key, 0, lock) ||
		ucache_find_local_entry_lock(ctx, key) != NULL
	;
}

bool ucache_entry_key_lock_active_locked(
		ucache_hdr *hdr,
		uint32_t hash,
		size_t key_pos,
		uint32_t key_len,
		uint64_t now)
{
	ucache_entry_lock_record *record;
	uint32_t i, probe;

	for (probe = 0; probe < hdr->entry_lock_capacity; probe++) {
		i = (ucache_entry_lock_table_idx(hdr, hash) + probe)
			& (hdr->entry_lock_capacity - 1)
		;
		record = &ucache_entry_lock_records_ptr(hdr)[i];

		if (record->state == UCACHE_ENTRY_LOCK_EMPTY) {
			return false;
		}

		if (record->state != UCACHE_ENTRY_LOCK_USED ||
			ucache_table_hash(record->hash) != hash ||
			record->key_len != key_len
		) {
			continue;
		}

		if (memcmp(
				(const char *) hdr + ucache_offset_bytes(record->key_offset),
				(const char *) hdr + key_pos,
				key_len
			) != 0
		) {
			continue;
		}

		return ucache_entry_lock_record_is_active_locked(record, now);
	}

	return false;
}

bool ucache_entry_locks_allow_clear_locked(zend_string *prefix)
{
	const char *key;
	ucache_hdr *hdr = ucache_hdr_ptr();
	ucache_entry_lock_record *record;
	ucache_entry_lock *local_lock;
	uint64_t now;
	uint32_t i;

	ucache_ensure_entry_lock_owner();

	if (hdr == NULL || !ucache_hdr_adoptable_locked()) {
		return true;
	}

	now = ucache_time_rel(hdr, ucache_clock_now());

	for (i = 0; i < hdr->entry_lock_capacity; i++) {
		record = &ucache_entry_lock_records_ptr(hdr)[i];
		if (record->state != UCACHE_ENTRY_LOCK_USED) {
			continue;
		}

		key = (const char *) ucache_ptr(record->key_offset);
		if (prefix != NULL && (record->key_len < ZSTR_LEN(prefix) ||
			memcmp(key, ZSTR_VAL(prefix), ZSTR_LEN(prefix)) != 0)
		) {
			continue;
		}

		if (!ucache_entry_lock_record_is_active_locked(record, now)) {
			ucache_remove_entry_lock_record_locked(record);

			continue;
		}

		local_lock = UC_G(entry_lock_table) != NULL
			? zend_hash_str_find_ptr(UC_G(entry_lock_table), key, record->key_len)
			: NULL
		;
		if (local_lock == NULL || local_lock->ctx != ucache_active_ctx() ||
			record->owner_pid != local_lock->owner_pid ||
			record->owner_token != local_lock->owner_token
		) {
			return false;
		}
	}

	return true;
}

void ucache_retry_deferred_entry_lock_releases(void)
{
	ucache_ctx *attempted[UCACHE_DEFERRED_RELEASE_RETRY_CTXS], *prev_ctx;
	ucache_entry_lock *entry;
	uint32_t attempted_count = 0;

	ucache_ensure_entry_lock_owner();

#ifdef ZTS
	ucache_adopt_orphaned_entry_lock_releases();
#endif

	entry = UC_G(deferred_entry_lock_releases);
	while (entry != NULL && attempted_count < UCACHE_DEFERRED_RELEASE_RETRY_CTXS) {
		if (ucache_ctx_listed(attempted, attempted_count, entry->ctx)) {
			entry = entry->next;

			continue;
		}

		attempted[attempted_count++] = entry->ctx;

		prev_ctx = ucache_activate_ctx(entry->ctx);
		ucache_drain_deferred_entry_lock_releases();
		ucache_restore_ctx(prev_ctx);

		entry = UC_G(deferred_entry_lock_releases);
	}
}

bool ucache_active_ctx_has_deferred_entry_lock_releases(void)
{
	ucache_ctx *ctx = ucache_active_ctx();
	ucache_entry_lock *entry;

	for (entry = UC_G(deferred_entry_lock_releases); entry != NULL; entry = entry->next) {
		if (entry->ctx == ctx) {
			return true;
		}
	}

	return false;
}

void ucache_release_req_entry_locks(void)
{
	ucache_ensure_entry_lock_owner();
	ucache_drain_deferred_entry_lock_releases();
	ucache_release_entry_locks_all_ctxs();

	free(UC_G(entry_lock_spare));

	UC_G(entry_lock_spare) = NULL;
#ifndef ZEND_WIN32
	UC_G(entry_lock_owner_pid) = 0;
#endif
}

void ucache_free_thread_deferred_entry_lock_releases(ucache_globals *globals)
{
	ucache_entry_lock *entry = globals->deferred_entry_lock_releases;

	free(globals->entry_lock_spare);

	globals->entry_lock_spare = NULL;
	globals->deferred_entry_lock_releases = NULL;

	ucache_free_entry_lock_list(entry);
}

#ifdef ZTS
void ucache_orphan_thread_deferred_entry_lock_releases(ucache_globals *globals)
{
	ucache_entry_lock *entry = globals->deferred_entry_lock_releases, *next;
	uint64_t self_pid = ucache_cached_pid();

	free(globals->entry_lock_spare);

	globals->entry_lock_spare = NULL;
	globals->deferred_entry_lock_releases = NULL;

	ucache_boundary_partitions_lock();

	while (entry != NULL) {
		next = entry->next;

		if (entry->owner_pid == self_pid) {
			entry->next = ucache_orphaned_entry_lock_releases;
			ucache_orphaned_entry_lock_releases = entry;

			atomic_store(&ucache_orphaned_entry_lock_releases_pending, true);
		} else {
			free(entry);
		}

		entry = next;
	}

	ucache_boundary_partitions_unlock();
}

void ucache_free_orphaned_entry_lock_releases(void)
{
	ucache_entry_lock *entry = ucache_orphaned_entry_lock_releases;

	ucache_orphaned_entry_lock_releases = NULL;

	atomic_store(&ucache_orphaned_entry_lock_releases_pending, false);

	ucache_free_entry_lock_list(entry);
}
#endif
