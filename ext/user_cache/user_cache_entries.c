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
   | Author: Go Kudo <zeriyoshi@php.net>.                                 |
   +----------------------------------------------------------------------+
*/

#include "user_cache_internal.h"

#include "Zend/zend_closures.h"
#include "Zend/zend_gc.h"
#include "Zend/zend_objects.h"

#define PHP_USER_CACHE_REQUEST_LOCAL_NO_DEEP_CLONE ((void *) 1)
#define PHP_USER_CACHE_REQUEST_LOCAL_NEEDS_DEEP_CLONE ((void *) 2)

/* Release the lock before propagating a bailout, unless a bulk store asked to
 * keep it held so its rollback runs without a lock gap. */
#define PHP_USER_CACHE_TRY_UNLOCK_ON_BAILOUT(stmt) \
	do { \
		zend_try { \
			stmt \
		} zend_catch { \
			if (!UC_G(store_defer_unlock)) { \
				php_user_cache_unlock_if_held(); \
			} \
			zend_bailout(); \
		} zend_end_try(); \
	} while (0)

typedef enum {
	PHP_USER_CACHE_REQUEST_LOCAL_SLOT_MISS,
	PHP_USER_CACHE_REQUEST_LOCAL_SLOT_HIT
} php_user_cache_request_local_slot_result;

typedef enum {
	PHP_USER_CACHE_FIND_SLOT_IGNORE_EXPIRY = 0,
	PHP_USER_CACHE_FIND_SLOT_SKIP_EXPIRED,
	PHP_USER_CACHE_FIND_SLOT_DELETE_EXPIRED
} php_user_cache_find_slot_expiry_mode;

typedef enum {
	PHP_USER_CACHE_FETCH_FINISH_USE_REQUEST_LOCAL_SLOT = 1 << 0,
	PHP_USER_CACHE_FETCH_FINISH_NO_ALIASES = 1 << 1,
	PHP_USER_CACHE_FETCH_FINISH_DEFER_REQUEST_LOCAL_SLOT = 1 << 2
} php_user_cache_fetch_finish_flags;

typedef enum {
	PHP_USER_CACHE_FETCH_LOCATE_MISS,
	PHP_USER_CACHE_FETCH_LOCATE_SCALAR_HIT,
	PHP_USER_CACHE_FETCH_LOCATE_SLOT,
	PHP_USER_CACHE_FETCH_LOCATE_UNCACHED
} php_user_cache_fetch_locate_result;

typedef enum {
	PHP_USER_CACHE_OBJECT_STORABLE_VIA_HOOKS,
	PHP_USER_CACHE_OBJECT_SERDES,
	PHP_USER_CACHE_OBJECT_OPAQUE,
	PHP_USER_CACHE_OBJECT_SCAN_MEMBERS
} php_user_cache_object_storability;

typedef enum {
	PHP_USER_CACHE_STORE_ATTEMPT_STORED,
	PHP_USER_CACHE_STORE_ATTEMPT_RETRY,
	PHP_USER_CACHE_STORE_ATTEMPT_FAILED
} php_user_cache_store_attempt_result;

typedef struct {
	HashTable arrays;
	HashTable objects;
	HashTable references;
	HashTable *clone_verdicts;
	bool track_identity;
} php_user_cache_request_local_clone_context;

typedef struct {
	HashTable *seen_arrays;
	HashTable *seen_objects;
	const char **failure_message;
} php_user_cache_unstorable_context;

typedef struct {
	uint64_t generation;
	const void *context;
	bool needs_deep_clone;
	bool has_clone_verdicts;
	bool no_aliases;
	bool has_value;
	zval value;
	HashTable clone_verdicts;
} php_user_cache_request_local_slot;

static bool user_cache_clone_request_local_value(
	php_user_cache_request_local_clone_context *ctx,
	zval *dst,
	zval *src
);

static bool user_cache_collect_request_local_clone_verdicts_impl(
	zval *value,
	HashTable *seen_arrs,
	HashTable *seen_objs,
	HashTable *verdicts,
	bool record_array_result
);

static bool user_cache_find_unstorable_value(
	zval *value,
	HashTable *seen_arrs,
	HashTable *seen_objs,
	const char **msg
);

static void user_cache_rehash_locked(php_user_cache_header *header);
static bool user_cache_expunge_expired_bounded_locked(void);

static zend_always_inline bool user_cache_value_uses_offset(uint8_t value_type)
{
	return
		value_type == PHP_USER_CACHE_VALUE_STRING ||
		value_type == PHP_USER_CACHE_VALUE_SHARED_GRAPH
	;
}

static zend_always_inline uint32_t user_cache_max_capacity_for_size(size_t size)
{
	size_t usable;

	if (size <= sizeof(php_user_cache_header)) {
		return 0;
	}

	usable = (size - sizeof(php_user_cache_header)) / sizeof(php_user_cache_entry);

	return usable > UINT32_MAX
		? UINT32_MAX
		: (uint32_t) usable
	;
}

static zend_always_inline uint8_t *user_cache_ptr_in_header(
		const php_user_cache_header *header,
		uint32_t offset)
{
	return (uint8_t *) header + offset;
}

static zend_always_inline bool user_cache_key_equals(
		const php_user_cache_header *header,
		const php_user_cache_entry *entry,
		zend_string *key,
		zend_ulong hash)
{
	if (entry->state != PHP_USER_CACHE_ENTRY_USED || entry->hash != hash || entry->key_len != ZSTR_LEN(key)) {
		return false;
	}

	return memcmp(
		user_cache_ptr_in_header(header, entry->key_offset),
		ZSTR_VAL(key),
		ZSTR_LEN(key)
	) == 0;
}

static zend_always_inline uint32_t user_cache_write_payload_locked(
		const php_user_cache_header *header,
		uint32_t reusable_offset,
		size_t size,
		const void *src)
{
	if (reusable_offset != 0 && php_user_cache_block_payload_capacity(reusable_offset) >= size) {
		memcpy(user_cache_ptr_in_header(header, reusable_offset), src, size);

		return reusable_offset;
	}

	return php_user_cache_alloc_locked(size, src);
}

static zend_always_inline php_user_cache_lookup_entry *user_cache_lookup_cache_set(zend_ulong hash)
{
	uint32_t set_idx = (uint32_t) (hash & (PHP_USER_CACHE_LOOKUP_SETS - 1));

	return &UC_G(lookup_entry_storage)[set_idx * PHP_USER_CACHE_LOOKUP_WAYS];
}

static zend_always_inline void user_cache_lookup_entry_release_key(
		php_user_cache_lookup_entry *lookup_entry)
{
	if (lookup_entry->key != NULL) {
		zend_string_release(lookup_entry->key);
		lookup_entry->key = NULL;
	}
}

static zend_always_inline void user_cache_lookup_cache_store(
		php_user_cache_lookup_entry *lookup_entry,
		zend_ulong hash,
		uint64_t epoch,
		uint32_t slot_idx,
		uint8_t state)
{
	if (lookup_entry == NULL) {
		return;
	}

	user_cache_lookup_entry_release_key(lookup_entry);

	lookup_entry->hash = hash;
	lookup_entry->mutation_epoch = epoch;
	lookup_entry->context = php_user_cache_active_context();
	lookup_entry->slot_index = slot_idx;
	lookup_entry->state = state;
	lookup_entry->value_type = PHP_USER_CACHE_LOOKUP_VALUE_NONE;
}

/* Prefer the current context, then empty, stale and miss slots. */
static zend_always_inline php_user_cache_lookup_entry *user_cache_lookup_cache_select_slot(
		php_user_cache_lookup_entry *lookup_entries,
		zend_ulong hash,
		uint64_t epoch,
		bool allow_hit_eviction)
{
	const void *ctx = php_user_cache_active_context();
	php_user_cache_lookup_entry *preferred, *alternate;
	uint32_t way = (uint32_t) ((((uint64_t) hash >> 32) ^ hash) & (PHP_USER_CACHE_LOOKUP_WAYS - 1));

	if (lookup_entries == NULL) {
		return NULL;
	}

	preferred = &lookup_entries[way];
	alternate = preferred == &lookup_entries[0] ? &lookup_entries[1] : &lookup_entries[0];

	if (preferred->state != PHP_USER_CACHE_LOOKUP_EMPTY &&
		preferred->hash == hash &&
		preferred->mutation_epoch == epoch &&
		preferred->context == ctx
	) {
		return preferred;
	}
	if (alternate->state != PHP_USER_CACHE_LOOKUP_EMPTY &&
		alternate->hash == hash &&
		alternate->mutation_epoch == epoch &&
		alternate->context == ctx
	) {
		return alternate;
	}

	if (preferred->state == PHP_USER_CACHE_LOOKUP_EMPTY ||
		preferred->mutation_epoch != epoch ||
		preferred->context != ctx
	) {
		return preferred;
	}
	if (alternate->state == PHP_USER_CACHE_LOOKUP_EMPTY ||
		alternate->mutation_epoch != epoch ||
		alternate->context != ctx
	) {
		return alternate;
	}

	if (preferred->state == PHP_USER_CACHE_LOOKUP_MISS) {
		return preferred;
	}
	if (alternate->state == PHP_USER_CACHE_LOOKUP_MISS) {
		return alternate;
	}

	return allow_hit_eviction ? preferred : NULL;
}

static zend_always_inline void user_cache_lookup_cache_store_miss(
		php_user_cache_lookup_entry *lookup_entries,
		zend_ulong hash,
		uint64_t epoch,
		zend_string *key)
{
	php_user_cache_lookup_entry *victim =
		user_cache_lookup_cache_select_slot(lookup_entries, hash, epoch, false)
	;

	if (victim == NULL) {
		return;
	}

	user_cache_lookup_cache_store(
		victim,
		hash,
		epoch,
		PHP_USER_CACHE_LOOKUP_INVALID_SLOT,
		PHP_USER_CACHE_LOOKUP_MISS
	);

	/* MISS entries must still distinguish hash collisions. */
	victim->key = zend_string_copy(key);
}

static zend_always_inline void user_cache_lookup_cache_store_hit(
		php_user_cache_lookup_entry *lookup_entry,
		zend_ulong hash,
		uint64_t epoch,
		uint32_t slot_idx,
		zend_string *key,
		const php_user_cache_entry *entry)
{
	if (lookup_entry == NULL) {
		return;
	}

	user_cache_lookup_cache_store(
		lookup_entry,
		hash,
		epoch,
		slot_idx,
		PHP_USER_CACHE_LOOKUP_HIT
	);

	lookup_entry->key = zend_string_copy(key);

	/* Only non-expiring scalars may be promoted; the lookup fast path returns
	 * them without an expiry recheck. */
	if (entry->expires_at != 0) {
		return;
	}

	switch (entry->value_type) {
		case PHP_USER_CACHE_VALUE_NULL:
		case PHP_USER_CACHE_VALUE_TRUE:
		case PHP_USER_CACHE_VALUE_FALSE:
		case PHP_USER_CACHE_VALUE_LONG:
		case PHP_USER_CACHE_VALUE_DOUBLE:
			lookup_entry->value_type = entry->value_type;
			lookup_entry->long_value = entry->long_value;
			lookup_entry->double_value = entry->double_value;

			break;
		default:
			break;
	}
}

static zend_always_inline void user_cache_lookup_cache_reset_entry(php_user_cache_lookup_entry *lookup_entry)
{
	if (lookup_entry == NULL) {
		return;
	}

	user_cache_lookup_entry_release_key(lookup_entry);

	memset(lookup_entry, 0, sizeof(*lookup_entry));
}

static zend_always_inline void user_cache_release_value_storage_locked(uint8_t value_type, uint32_t value_offset)
{
	bool graph_quiescent;

	if (value_offset == 0) {
		return;
	}

	if (value_type == PHP_USER_CACHE_VALUE_SHARED_GRAPH) {
		graph_quiescent = php_user_cache_quiesce_graph_payloads_locked();

		if (php_user_cache_shared_graph_retire_payload_locked(value_offset)) {
			if (graph_quiescent) {
				php_user_cache_free_locked(value_offset);
			} else {
				php_user_cache_shared_graph_orphan_payload_locked(value_offset);
			}
		}
	} else if (user_cache_value_uses_offset(value_type)) {
		php_user_cache_free_locked(value_offset);
	}
}

static zend_always_inline void user_cache_release_entry_storage_except_locked(
		php_user_cache_entry *entry,
		const php_user_cache_entry *kept_entry)
{
	bool combined, kept_combined;

	combined = (entry->reserved & PHP_USER_CACHE_ENTRY_RESERVED_COMBINED_VALUE_KEY) != 0;
	kept_combined = kept_entry != NULL &&
		(kept_entry->reserved & PHP_USER_CACHE_ENTRY_RESERVED_COMBINED_VALUE_KEY) != 0
	;
	if (entry->key_offset != 0 && !combined &&
		(kept_entry == NULL || kept_combined || entry->key_offset != kept_entry->key_offset)
	) {
		php_user_cache_free_locked(entry->key_offset);
	}

	if (entry->value_offset != 0 &&
		(
			kept_entry == NULL ||
			entry->value_offset != kept_entry->value_offset ||
			entry->value_type != kept_entry->value_type
		)
	) {
		user_cache_release_value_storage_locked(entry->value_type, entry->value_offset);
	}
}

static zend_always_inline void user_cache_release_entry_storage_locked(php_user_cache_entry *entry)
{
	user_cache_release_entry_storage_except_locked(entry, NULL);
}

static zend_always_inline void user_cache_delete_entry_locked(php_user_cache_header *header, php_user_cache_entry *entry)
{
	if (entry->state == PHP_USER_CACHE_ENTRY_USED && header->count != 0) {
		header->count--;
	}

	if (entry->state != PHP_USER_CACHE_ENTRY_TOMBSTONE) {
		header->tombstone_count++;
	}

	user_cache_release_entry_storage_locked(entry);

	entry->hash = 0;
	entry->key_offset = 0;
	entry->key_len = 0;
	entry->state = PHP_USER_CACHE_ENTRY_TOMBSTONE;
	entry->value_type = PHP_USER_CACHE_VALUE_NULL;
	entry->value_offset = 0;
	entry->value_len = 0;
	entry->reserved = 0;
	entry->expires_at = 0;
	entry->generation = 0;
	entry->long_value = 0;
	entry->double_value = 0;

	php_user_cache_bump_mutation_epoch_locked(header);
}

static zend_always_inline void user_cache_release_request_local_slot_table(HashTable **slots_ptr)
{
	if (*slots_ptr == NULL) {
		return;
	}

	zend_hash_destroy(*slots_ptr);

	FREE_HASHTABLE(*slots_ptr);

	*slots_ptr = NULL;
}

static zend_always_inline bool user_cache_is_expired(const php_user_cache_entry *entry, uint64_t now)
{
	return entry->state == PHP_USER_CACHE_ENTRY_USED &&
		entry->expires_at != 0 &&
		entry->expires_at <= now
	;
}

static zend_always_inline bool user_cache_is_expired_now(const php_user_cache_entry *entry, uint64_t *now)
{
	if (entry->state != PHP_USER_CACHE_ENTRY_USED || entry->expires_at == 0) {
		return false;
	}

	if (*now == 0) {
		*now = (uint64_t) time(NULL);
	}

	return user_cache_is_expired(entry, *now);
}

static zend_always_inline void user_cache_note_expired_read(void)
{
	UC_G(expired_read_observations)++;
}

static zend_always_inline bool user_cache_scalar_to_zval(
		uint8_t value_type,
		zend_long lval,
		double dval,
		zval *return_value)
{
	switch (value_type) {
		case PHP_USER_CACHE_VALUE_NULL:
			ZVAL_NULL(return_value);

			return true;
		case PHP_USER_CACHE_VALUE_TRUE:
			ZVAL_TRUE(return_value);

			return true;
		case PHP_USER_CACHE_VALUE_FALSE:
			ZVAL_FALSE(return_value);

			return true;
		case PHP_USER_CACHE_VALUE_LONG:
			ZVAL_LONG(return_value, lval);

			return true;
		case PHP_USER_CACHE_VALUE_DOUBLE:
			ZVAL_DOUBLE(return_value, dval);

			return true;
		default:
			return false;
	}
}

static zend_always_inline bool user_cache_payload_can_fit_locked(size_t size)
{
	php_user_cache_header *header = php_user_cache_header_ptr();
	size_t total_size;

	if (!header || size == 0 || size > UINT32_MAX - sizeof(php_user_cache_block)) {
		return false;
	}

	total_size = PHP_USER_CACHE_ALIGNED_SIZE(sizeof(php_user_cache_block) + size);

	return total_size <= UINT32_MAX && total_size <= header->data_size;
}

static zend_always_inline void user_cache_init_prepared_value(php_user_cache_prepared_value *prepared)
{
	memset(prepared, 0, sizeof(*prepared));

	prepared->value_type = PHP_USER_CACHE_VALUE_NULL;
}

static zend_always_inline bool user_cache_prepared_value_should_seed_request_local_slot(
		const php_user_cache_prepared_value *prepared)
{
	return prepared->value_type == PHP_USER_CACHE_VALUE_STRING &&
		prepared->value_len >= PHP_USER_CACHE_REQUEST_LOCAL_STRING_MIN_LEN
	;
}

static zend_always_inline bool user_cache_long_add_overflow(
		zend_long lhs,
		zend_long rhs,
		zend_long *result)
{
	*result = (zend_long) ((zend_ulong) lhs + (zend_ulong) rhs);

	return (rhs > 0 && lhs > ZEND_LONG_MAX - rhs) ||
		(rhs < 0 && lhs < ZEND_LONG_MIN - rhs)
	;
}

static zend_always_inline bool user_cache_long_sub_overflow(
		zend_long lhs,
		zend_long rhs,
		zend_long *result)
{
	*result = (zend_long) ((zend_ulong) lhs - (zend_ulong) rhs);

	return (rhs > 0 && lhs < ZEND_LONG_MIN + rhs) ||
		(rhs < 0 && lhs > ZEND_LONG_MAX + rhs)
	;
}

static zend_always_inline void user_cache_maybe_rehash_locked(void)
{
	php_user_cache_header *header = php_user_cache_header_ptr();

	if (header != NULL &&
		php_user_cache_header_is_initialized_locked() &&
		header->tombstone_count > header->capacity / 4
	) {
		user_cache_rehash_locked(header);
	}
}

static zend_always_inline void user_cache_maybe_expunge_expired_locked(void)
{
	UC_G(expunge_write_ops)++;

	if (EXPECTED(UC_G(expired_read_observations) < PHP_USER_CACHE_EXPIRED_READ_EXPUNGE_THRESHOLD &&
		UC_G(expunge_write_ops) < PHP_USER_CACHE_EXPUNGE_WRITE_OP_INTERVAL)
	) {
		return;
	}

	UC_G(expired_read_observations) = 0;
	UC_G(expunge_write_ops) = 0;

	(void) user_cache_expunge_expired_bounded_locked();
}

#ifdef PHP_USER_CACHE_HAVE_OPTIMISTIC
static zend_always_inline bool user_cache_optimistic_payload_in_bounds(
		const php_user_cache_header *header,
		uint32_t offset,
		uint64_t len)
{
	uint64_t limit = (uint64_t) header->data_offset + header->data_size;

	return offset >= header->data_offset + sizeof(php_user_cache_block) &&
		(uint64_t) offset + len <= limit
	;
}

static zend_always_inline bool user_cache_optimistic_header(
		php_user_cache_header **header_ptr,
		uint64_t *seq_ptr)
{
	php_user_cache_header *header = php_user_cache_header_ptr();
	uint64_t seq;

	if (header == NULL) {
		return false;
	}

	seq = php_user_cache_seq_load(&header->write_seq);
	if (seq < 2 || (seq & 1) != 0) {
		return false;
	}

	if (header->magic != PHP_USER_CACHE_MAGIC ||
		header->version != PHP_USER_CACHE_VERSION ||
		header->capacity < PHP_USER_CACHE_MIN_CAPACITY ||
		header->capacity > user_cache_max_capacity_for_size(
			php_user_cache_active_context()->storage.size
		)
	) {
		return false;
	}

	if (php_user_cache_seq_reload(&header->write_seq) != seq) {
		return false;
	}

	*header_ptr = header;
	*seq_ptr = seq;

	return true;
}

static zend_always_inline php_user_cache_request_local_slot *user_cache_alloc_request_local_slot(
		uint64_t gen,
		bool no_aliases,
		bool has_value)
{
	php_user_cache_request_local_slot *slot;

	slot = emalloc(sizeof(php_user_cache_request_local_slot));
	slot->generation = gen;
	slot->context = php_user_cache_active_context();
	slot->needs_deep_clone = false;
	slot->has_clone_verdicts = false;
	slot->no_aliases = no_aliases;
	slot->has_value = has_value;

	ZVAL_UNDEF(&slot->value);

	return slot;
}

/* The caller must recheck write_seq before using the snapshot. Returns false
 * when the snapshot is unusable (out-of-bounds offset or corrupt state); the
 * caller must fall back to the locked path. */
static bool user_cache_optimistic_probe(
		const php_user_cache_header *header,
		zend_string *key,
		zend_ulong hash,
		const php_user_cache_entry *entries,
		php_user_cache_entry *snapshot,
		uint32_t *slot_idx,
		bool *found)
{
	const php_user_cache_entry *entry;
	uint64_t now = 0;
	uint32_t i, step;

	i = (uint32_t) (hash % header->capacity);

	for (step = 0; step < header->capacity; step++) {
		entry = &entries[i];

		switch (entry->state) {
			case PHP_USER_CACHE_ENTRY_EMPTY:
				*found = false;

				return true;
			case PHP_USER_CACHE_ENTRY_USED:
				if (entry->hash == hash && entry->key_len == ZSTR_LEN(key)) {
					if (!user_cache_optimistic_payload_in_bounds(header, entry->key_offset, entry->key_len)) {
						return false;
					}

					if (memcmp(user_cache_ptr_in_header(header, entry->key_offset), ZSTR_VAL(key), ZSTR_LEN(key)) == 0) {
						if (user_cache_is_expired_now(entry, &now)) {
							user_cache_note_expired_read();
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
			case PHP_USER_CACHE_ENTRY_TOMBSTONE:
				break;
			default:
				return false;
		}

		++i;
		if (i == header->capacity) {
			i = 0;
		}
	}

	*found = false;

	return true;
}
#endif

static void user_cache_request_local_slot_dtor(zval *slot_zv)
{
	php_user_cache_request_local_slot *slot = Z_PTR_P(slot_zv);

	if (slot->has_clone_verdicts) {
		zend_hash_destroy(&slot->clone_verdicts);
	}

	if (!Z_ISUNDEF(slot->value)) {
		zval_ptr_dtor(&slot->value);
	}

	efree(slot);
}

static bool user_cache_value_needs_request_local_deep_clone_impl(
		zval *value,
		HashTable *seen_arrs)
{
	zend_ulong arr_key;
	zval *elem;

	if (php_user_cache_stack_overflowed()) {
		return true;
	}

	if (Z_ISREF_P(value)) {
		return true;
	}

	switch (Z_TYPE_P(value)) {
		case IS_OBJECT:
			return true;
		case IS_ARRAY:
			arr_key = (zend_ulong) (uintptr_t) Z_ARRVAL_P(value);
			if (!php_user_cache_seen_test_and_add(seen_arrs, Z_ARRVAL_P(value))) {
				return false;
			}

			ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(value), elem) {
				if (user_cache_value_needs_request_local_deep_clone_impl(elem, seen_arrs)) {
					zend_hash_index_del(seen_arrs, arr_key);

					return true;
				}
			} ZEND_HASH_FOREACH_END();

			zend_hash_index_del(seen_arrs, arr_key);

			return false;
		default:
			return false;
	}
}

static bool user_cache_value_needs_request_local_deep_clone(zval *value)
{
	HashTable seen_arrs;
	bool result;

	if (Z_ISREF_P(value)) {
		return true;
	} else if (Z_TYPE_P(value) == IS_OBJECT) {
		return true;
	} else if (Z_TYPE_P(value) != IS_ARRAY) {
		return false;
	}

	zend_hash_init(&seen_arrs, 8, NULL, NULL, 0);

	result = user_cache_value_needs_request_local_deep_clone_impl(value, &seen_arrs);
	zend_hash_destroy(&seen_arrs);

	return result;
}

static bool user_cache_collect_request_local_object_clone_verdict(
		zend_object *obj,
		HashTable *seen_arrs,
		HashTable *seen_objs,
		HashTable *verdicts)
{
	zend_ulong obj_key;
	zval *prop, *src, *end;
	bool members_need_clone = false;
	void *flag;

	if (php_user_cache_stack_overflowed()) {
		/* A failed walk cannot prove that the value is acyclic. */
		UC_G(request_local_slot_may_cycle) = true;

		return true;
	}

	if (obj == NULL) {
		return true;
	}

	obj_key = (zend_ulong) (uintptr_t) obj;

	flag = zend_hash_index_find_ptr(verdicts, obj_key);
	if (flag != NULL) {
		return flag == PHP_USER_CACHE_REQUEST_LOCAL_NEEDS_DEEP_CLONE;
	}

	if (!php_user_cache_seen_test_and_add(seen_objs, obj)) {
		/* A seen unfinished object is a back edge. */
		UC_G(request_local_slot_may_cycle) = true;

		return true;
	}

	if (obj->ce->default_properties_count) {
		src = obj->properties_table;
		end = src + obj->ce->default_properties_count;

		do {
			if (user_cache_collect_request_local_clone_verdicts_impl(
					src,
					seen_arrs,
					seen_objs,
					verdicts,
					true
				)
			) {
				members_need_clone = true;
			}

			src++;
		} while (src != end);
	}

	if (obj->properties != NULL && zend_hash_num_elements(obj->properties) != 0) {
		members_need_clone = true;

		ZEND_HASH_MAP_FOREACH_VAL(obj->properties, prop) {
			if (Z_TYPE_P(prop) != IS_INDIRECT) {
				user_cache_collect_request_local_clone_verdicts_impl(
					prop,
					seen_arrs,
					seen_objs,
					verdicts,
					true
				);
			}
		} ZEND_HASH_FOREACH_END();
	}

	zend_hash_index_update_ptr(
		verdicts,
		obj_key,
		members_need_clone
			? PHP_USER_CACHE_REQUEST_LOCAL_NEEDS_DEEP_CLONE
			: PHP_USER_CACHE_REQUEST_LOCAL_NO_DEEP_CLONE
	);

	return members_need_clone;
}

static bool user_cache_collect_request_local_clone_verdicts_impl(
		zval *value,
		HashTable *seen_arrs,
		HashTable *seen_objs,
		HashTable *verdicts,
		bool record_array_result)
{
	zend_ulong arr_key;
	zval *elem;
	bool needs_deep_clone = false;
	void *flag;

	if (php_user_cache_stack_overflowed()) {
		UC_G(request_local_slot_may_cycle) = true;

		return true;
	}

	if (Z_ISREF_P(value)) {
		user_cache_collect_request_local_clone_verdicts_impl(
			&Z_REF_P(value)->val,
			seen_arrs,
			seen_objs,
			verdicts,
			true
		);

		return true;
	}

	switch (Z_TYPE_P(value)) {
		case IS_OBJECT:
			user_cache_collect_request_local_object_clone_verdict(
				Z_OBJ_P(value),
				seen_arrs,
				seen_objs,
				verdicts
			);

			return true;
		case IS_ARRAY:
			arr_key = (zend_ulong) (uintptr_t) Z_ARRVAL_P(value);
			flag = zend_hash_index_find_ptr(verdicts, arr_key);
			if (flag != NULL) {
				return flag == PHP_USER_CACHE_REQUEST_LOCAL_NEEDS_DEEP_CLONE;
			}

			if (!php_user_cache_seen_test_and_add(seen_arrs, Z_ARRVAL_P(value))) {
				/* A seen active array is a back edge. */
				UC_G(request_local_slot_may_cycle) = true;

				return false;
			}

			ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(value), elem) {
				if (user_cache_collect_request_local_clone_verdicts_impl(
						elem,
						seen_arrs,
						seen_objs,
						verdicts,
						false
					)
				) {
					needs_deep_clone = true;
				}
			} ZEND_HASH_FOREACH_END();

			zend_hash_index_del(seen_arrs, arr_key);

			if (needs_deep_clone || record_array_result) {
				zend_hash_index_update_ptr(
					verdicts,
					arr_key,
					needs_deep_clone
						? PHP_USER_CACHE_REQUEST_LOCAL_NEEDS_DEEP_CLONE
						: PHP_USER_CACHE_REQUEST_LOCAL_NO_DEEP_CLONE
				);
			}

			return needs_deep_clone;
		default:
			return false;
	}
}

static bool user_cache_collect_request_local_clone_verdicts(
		zval *value,
		HashTable *verdicts)
{
	HashTable seen_arrs, seen_objs;
	bool result;

	zend_hash_init(&seen_arrs, 8, NULL, NULL, 0);
	zend_hash_init(&seen_objs, 8, NULL, NULL, 0);

	result = user_cache_collect_request_local_clone_verdicts_impl(
		value,
		&seen_arrs,
		&seen_objs,
		verdicts,
		true
	);

	zend_hash_destroy(&seen_objs);
	zend_hash_destroy(&seen_arrs);

	return result;
}

static void user_cache_request_local_clone_array_dtor(zval *zv)
{
	zend_array *array = Z_PTR_P(zv);

	zend_array_release(array);
}

static void user_cache_request_local_clone_context_init(
		php_user_cache_request_local_clone_context *ctx,
		HashTable *verdicts,
		bool track_identity)
{
	zend_hash_init(&ctx->arrays, 8, NULL, user_cache_request_local_clone_array_dtor, 0);
	zend_hash_init(&ctx->objects, 8, NULL, php_user_cache_object_table_dtor, 0);
	zend_hash_init(&ctx->references, 8, NULL, php_user_cache_reference_table_dtor, 0);

	ctx->clone_verdicts = verdicts;
	ctx->track_identity = track_identity;
}

static void user_cache_request_local_clone_context_destroy(
		php_user_cache_request_local_clone_context *ctx)
{
	zend_hash_destroy(&ctx->references);
	zend_hash_destroy(&ctx->objects);
	zend_hash_destroy(&ctx->arrays);
}

static bool user_cache_value_needs_request_local_deep_clone_cached(
		php_user_cache_request_local_clone_context *ctx,
		zval *value)
{
	void *flag;

	if (Z_ISREF_P(value)) {
		return true;
	}

	if (Z_TYPE_P(value) == IS_OBJECT) {
		return true;
	}

	if (Z_TYPE_P(value) != IS_ARRAY) {
		return false;
	}

	if (ctx->clone_verdicts != NULL) {
		flag = zend_hash_index_find_ptr(ctx->clone_verdicts, (zend_ulong) (uintptr_t) Z_ARRVAL_P(value));
		if (flag != NULL) {
			return flag == PHP_USER_CACHE_REQUEST_LOCAL_NEEDS_DEEP_CLONE;
		}
	}

	return user_cache_value_needs_request_local_deep_clone(value);
}

static bool user_cache_clone_request_local_array(
		php_user_cache_request_local_clone_context *ctx,
		zval *dst,
		zval *src,
		bool known_needs_deep_clone)
{
	zend_ulong key = (zend_ulong) (uintptr_t) Z_ARRVAL_P(src);
	zend_array *array;
	zval *elem, cloned_elem;
	void *flag;

	if (!known_needs_deep_clone) {
		if (ctx->clone_verdicts != NULL) {
			flag = zend_hash_index_find_ptr(ctx->clone_verdicts, key);
			if (flag == PHP_USER_CACHE_REQUEST_LOCAL_NO_DEEP_CLONE) {
				ZVAL_COPY(dst, src);

				return true;
			} else if (flag == PHP_USER_CACHE_REQUEST_LOCAL_NEEDS_DEEP_CLONE) {
				known_needs_deep_clone = true;
			}
		}

		if (!known_needs_deep_clone && !user_cache_value_needs_request_local_deep_clone(src)) {
			ZVAL_COPY(dst, src);

			return true;
		}
	}

	if (ctx->track_identity) {
		array = zend_hash_index_find_ptr(&ctx->arrays, key);
		if (array != NULL) {
			GC_ADDREF(array);
			ZVAL_ARR(dst, array);

			return true;
		}
	}

	array = zend_array_dup(Z_ARRVAL_P(src));
	if (ctx->track_identity) {
		zend_hash_index_update_ptr(&ctx->arrays, key, array);
	}

	ZEND_HASH_FOREACH_VAL(array, elem) {
		if (Z_TYPE_P(elem) == IS_INDIRECT ||
			!user_cache_value_needs_request_local_deep_clone_cached(ctx, elem)
		) {
			continue;
		}

		if (!user_cache_clone_request_local_value(ctx, &cloned_elem, elem)) {
			if (!ctx->track_identity) {
				zend_array_release(array);
			}

			ZVAL_UNDEF(dst);

			return false;
		}

		zval_ptr_dtor(elem);

		ZVAL_COPY_VALUE(elem, &cloned_elem);
	} ZEND_HASH_FOREACH_END();

	if (ctx->track_identity) {
		GC_ADDREF(array);
	}

	ZVAL_ARR(dst, array);

	return true;
}

static bool user_cache_clone_request_local_reference(
		php_user_cache_request_local_clone_context *ctx,
		zval *dst,
		zend_reference *src_ref)
{
	zend_ulong key = (zend_ulong) (uintptr_t) src_ref;
	zend_reference *new_ref;
	zval inner;

	if (ctx->track_identity) {
		new_ref = zend_hash_index_find_ptr(&ctx->references, key);
		if (new_ref != NULL) {
			GC_ADDREF(new_ref);
			ZVAL_REF(dst, new_ref);

			return true;
		}
	}

	ZVAL_NEW_EMPTY_REF(dst);
	new_ref = Z_REF_P(dst);
	ZVAL_UNDEF(&new_ref->val);

	if (ctx->track_identity) {
		zend_hash_index_update_ptr(&ctx->references, key, new_ref);
	}

	if (!user_cache_clone_request_local_value(ctx, &inner, &src_ref->val)) {
		if (!ctx->track_identity) {
			zval_ptr_dtor(dst);
		}

		ZVAL_UNDEF(dst);

		return false;
	}

	ZVAL_COPY_VALUE(&new_ref->val, &inner);

	if (ctx->track_identity) {
		GC_ADDREF(new_ref);
		ZVAL_REF(dst, new_ref);
	}

	return true;
}

static bool user_cache_clone_request_local_object_members(
		php_user_cache_request_local_clone_context *ctx,
		zend_object *old_obj,
		zend_object *new_obj)
{
	zend_ulong num_key;
	zend_string *key;
	zend_property_info *prop_info;
	zval *src, *dst, *end, *prop, new_prop;
	uint32_t prop_idx = 0;

	if (old_obj->ce->default_properties_count) {
		src = old_obj->properties_table;
		dst = new_obj->properties_table;
		end = src + old_obj->ce->default_properties_count;

		do {
			if (!user_cache_clone_request_local_value(ctx, &new_prop, src)) {
				return false;
			}

			zval_ptr_dtor(dst);

			ZVAL_COPY_VALUE(dst, &new_prop);
			Z_PROP_FLAG_P(dst) = Z_PROP_FLAG_P(src);

			if (Z_ISREF_P(dst) && new_obj->ce->properties_info_table != NULL) {
				prop_info = new_obj->ce->properties_info_table[prop_idx];
				if (prop_info != NULL &&
					prop_info != ZEND_WRONG_PROPERTY_INFO &&
					ZEND_TYPE_IS_SET(prop_info->type)
				) {
					ZEND_REF_ADD_TYPE_SOURCE(Z_REF_P(dst), prop_info);
				}
			}

			src++;
			dst++;
			prop_idx++;
		} while (src != end);
	}

	if (old_obj->properties != NULL && zend_hash_num_elements(old_obj->properties) != 0) {
		if (new_obj->properties != NULL) {
			zend_hash_clean(new_obj->properties);
			zend_hash_extend(new_obj->properties, zend_hash_num_elements(old_obj->properties), 0);
		} else {
			new_obj->properties = zend_new_array(zend_hash_num_elements(old_obj->properties));
			zend_hash_real_init_mixed(new_obj->properties);
		}

		HT_FLAGS(new_obj->properties) |=
			HT_FLAGS(old_obj->properties) & HASH_FLAG_HAS_EMPTY_IND
		;

		ZEND_HASH_MAP_FOREACH_KEY_VAL(old_obj->properties, num_key, key, prop) {
			if (Z_TYPE_P(prop) == IS_INDIRECT) {
				ZVAL_INDIRECT(
					&new_prop,
					new_obj->properties_table + (Z_INDIRECT_P(prop) - old_obj->properties_table)
				);
			} else if (!user_cache_clone_request_local_value(ctx, &new_prop, prop)) {
				return false;
			}

			if (key != NULL) {
				_zend_hash_append(new_obj->properties, key, &new_prop);
			} else {
				zend_hash_index_add_new(new_obj->properties, num_key, &new_prop);
			}
		} ZEND_HASH_FOREACH_END();
	}

	return true;
}

static bool user_cache_clone_request_local_std_object(
		php_user_cache_request_local_clone_context *ctx,
		zend_object *old_obj,
		zend_object **new_obj_ptr)
{
	zend_object *new_obj;
	zval *src, *dst, *end;
	void *member_flag = NULL;

	new_obj = zend_objects_new(old_obj->ce);

	if (ctx->track_identity) {
		zend_hash_index_update_ptr(
			&ctx->objects,
			(zend_ulong) (uintptr_t) old_obj,
			new_obj
		);
	}

	if (ctx->clone_verdicts != NULL) {
		member_flag = zend_hash_index_find_ptr(
			ctx->clone_verdicts,
			(zend_ulong) (uintptr_t) old_obj
		);
	}

	if (member_flag == PHP_USER_CACHE_REQUEST_LOCAL_NO_DEEP_CLONE &&
		old_obj->properties == NULL
	) {
		if (old_obj->ce->default_properties_count) {
			src = old_obj->properties_table;
			dst = new_obj->properties_table;
			end = src + old_obj->ce->default_properties_count;

			memcpy(dst, src, sizeof(zval) * old_obj->ce->default_properties_count);

			do {
				if (Z_REFCOUNTED_P(src)) {
					Z_ADDREF_P(src);
				}

				src++;
			} while (src != end);
		}
	} else {
		if (new_obj->ce->default_properties_count) {
			dst = new_obj->properties_table;
			end = dst + new_obj->ce->default_properties_count;

			do {
				ZVAL_UNDEF(dst);

				dst++;
			} while (dst != end);
		}

		if (!user_cache_clone_request_local_object_members(ctx, old_obj, new_obj)) {
			if (!ctx->track_identity) {
				OBJ_RELEASE(new_obj);
			}

			return false;
		}
	}

	if (ctx->track_identity) {
		GC_ADDREF(new_obj);
	}

	*new_obj_ptr = new_obj;

	return true;
}

static bool user_cache_clone_request_local_value_callback(
		void *ctx,
		zval *dst,
		zval *src)
{
	return user_cache_clone_request_local_value(
		(php_user_cache_request_local_clone_context *) ctx,
		dst,
		src
	);
}

static bool user_cache_clone_request_local_safe_direct_object(
		php_user_cache_request_local_clone_context *ctx,
		zend_object *old_obj,
		zend_object **new_obj_ptr)
{
	php_user_cache_safe_direct_state_copy_func_t copy_func;
	zend_ulong key;
	zend_class_entry *ce;
	zend_object *new_obj;
	zval new_zv;

	ce = old_obj->ce;

	copy_func = php_user_cache_safe_direct_state_copy_func(ce, NULL);
	if (copy_func == NULL) {
		return false;
	}

	ZVAL_UNDEF(&new_zv);

	if (object_init_ex(&new_zv, ce) != SUCCESS) {
		return false;
	}

	new_obj = Z_OBJ(new_zv);
	key = (zend_ulong) (uintptr_t) old_obj;

	if (ctx->track_identity) {
		zend_hash_index_update_ptr(&ctx->objects, key, new_obj);
	}

	if (!copy_func(
			ctx,
			new_obj,
			old_obj,
			user_cache_clone_request_local_value_callback
		)
	) {
		if (!ctx->track_identity) {
			OBJ_RELEASE(new_obj);
		}

		return false;
	}

	if (!user_cache_clone_request_local_object_members(ctx, old_obj, new_obj)) {
		if (!ctx->track_identity) {
			OBJ_RELEASE(new_obj);
		}

		return false;
	}

	if (ctx->track_identity) {
		GC_ADDREF(new_obj);
	}

	*new_obj_ptr = new_obj;

	return true;
}

static bool user_cache_clone_request_local_object(
		php_user_cache_request_local_clone_context *ctx,
		zend_object *old_obj,
		zend_object **new_obj_ptr)
{
	zend_ulong key;
	zend_object *new_obj;

	if (old_obj == NULL || zend_object_is_lazy(old_obj)) {
		return false;
	}

	if (ctx->track_identity) {
		key = (zend_ulong) (uintptr_t) old_obj;
		new_obj = zend_hash_index_find_ptr(&ctx->objects, key);

		if (new_obj != NULL) {
			GC_ADDREF(new_obj);
			*new_obj_ptr = new_obj;

			return true;
		}
	}

	if (old_obj->handlers == zend_get_std_object_handlers()) {
		return user_cache_clone_request_local_std_object(ctx, old_obj, new_obj_ptr);
	}

	return user_cache_clone_request_local_safe_direct_object(ctx, old_obj, new_obj_ptr);
}

static bool user_cache_clone_request_local_value(
		php_user_cache_request_local_clone_context *ctx,
		zval *dst,
		zval *src)
{
	zend_object *obj;

	if (php_user_cache_stack_overflowed()) {
		ZVAL_UNDEF(dst);

		return false;
	}

	if (Z_ISREF_P(src)) {
		return user_cache_clone_request_local_reference(ctx, dst, Z_REF_P(src));
	}

	switch (Z_TYPE_P(src)) {
		case IS_ARRAY:
			return user_cache_clone_request_local_array(ctx, dst, src, false);
		case IS_OBJECT:
			if (!user_cache_clone_request_local_object(ctx, Z_OBJ_P(src), &obj)) {
				return false;
			}

			ZVAL_OBJ(dst, obj);

			return true;
		default:
			ZVAL_COPY(dst, src);

			return true;
	}
}

static bool user_cache_clone_request_local_slot_value_known(
		zval *dst,
		zval *src,
		bool needs_deep_clone,
		HashTable *verdicts,
		bool no_aliases)
{
	php_user_cache_request_local_clone_context ctx;
	bool result;

	if (!needs_deep_clone) {
		ZVAL_COPY(dst, src);

		return true;
	}

	user_cache_request_local_clone_context_init(&ctx, verdicts, !no_aliases);

	if (Z_TYPE_P(src) == IS_ARRAY) {
		result = user_cache_clone_request_local_array(&ctx, dst, src, true);
	} else {
		result = user_cache_clone_request_local_value(&ctx, dst, src);
	}

	user_cache_request_local_clone_context_destroy(&ctx);

	return result;
}

static HashTable *user_cache_request_local_slots(void)
{
	HashTable **slots_ptr = &UC_G(request_local_slot_table);

	if (*slots_ptr == NULL) {
		ALLOC_HASHTABLE(*slots_ptr);
		zend_hash_init(*slots_ptr, 0, NULL, user_cache_request_local_slot_dtor, 0);
	}

	return *slots_ptr;
}

static zend_never_inline void user_cache_reacquire_read_lock_or_fail(const char *cache_name)
{
	if (!php_user_cache_rlock()) {
		zend_error_noreturn(E_ERROR, "Unable to reacquire the %s read lock after userland execution", cache_name);
	}
}

static bool user_cache_materialize_shared_graph_locked(
		const php_user_cache_header *header,
		zend_string *key,
		const char *cache_name,
		uint8_t value_type,
		uint32_t value_offset,
		uint32_t value_len,
		bool throw_if_missing,
		zval *return_value)
{
	bool result, ref_registered, lock_safe;

	switch (value_type) {
		case PHP_USER_CACHE_VALUE_SHARED_GRAPH:
			ref_registered = php_user_cache_has_request_shared_graph_ref(value_offset);
			if (!ref_registered) {
				/* Register before decode can invoke user code and bail out. */
				php_user_cache_shared_graph_ref_reserve();

				if (!php_user_cache_shared_graph_acquire_ref(value_offset)) {
					if (throw_if_missing) {
						PHP_USER_CACHE_TRY_UNLOCK_ON_BAILOUT(
							zend_throw_exception_ex(php_user_cache_exception_ce, 0, "Stored %s value for key \"%s\" is corrupted", cache_name, ZSTR_VAL(key));
						);
					}

					return false;
				}

				php_user_cache_register_shared_graph_ref(value_offset);
			}

			lock_safe = php_user_cache_shared_graph_decode_is_lock_safe(value_offset);
			if (!lock_safe) {
				php_user_cache_unlock();
			}

			ZVAL_UNDEF(return_value);

			if (lock_safe) {
				PHP_USER_CACHE_TRY_UNLOCK_ON_BAILOUT(
					result = php_user_cache_shared_graph_decode(
						user_cache_ptr_in_header(header, value_offset),
						value_len,
						return_value
					);
				);
			} else {
				result = php_user_cache_shared_graph_decode(
					user_cache_ptr_in_header(header, value_offset),
					value_len,
					return_value
				);
			}

			if (!lock_safe && !result && Z_TYPE_P(return_value) != IS_UNDEF) {
				zval_ptr_dtor(return_value);
				ZVAL_UNDEF(return_value);
			}

			if (!lock_safe) {
				user_cache_reacquire_read_lock_or_fail(cache_name);
			}

			if (!result) {
				if (lock_safe && Z_TYPE_P(return_value) != IS_UNDEF) {
					zval_ptr_dtor(return_value);
					ZVAL_UNDEF(return_value);
				}

				/* Request cleanup releases the registered reference. */
				if (!EG(exception) && throw_if_missing) {
					PHP_USER_CACHE_TRY_UNLOCK_ON_BAILOUT(
						zend_throw_exception_ex(php_user_cache_exception_ce, 0, "Stored %s value for key \"%s\" is corrupted", cache_name, ZSTR_VAL(key));
					);
				}

				return false;
			}

			return true;
		default:
			return false;
	}
}

static php_user_cache_request_local_slot *user_cache_find_request_local_slot(
		zend_string *key,
		uint64_t gen,
		bool evict_stale)
{
	php_user_cache_request_local_slot *slot;
	HashTable **slots_ptr = &UC_G(request_local_slot_table);

	if (*slots_ptr == NULL) {
		return NULL;
	}

	slot = zend_hash_find_ptr(*slots_ptr, key);
	if (slot == NULL) {
		return NULL;
	}

	if (slot->generation != gen ||
		slot->context != (const void *) php_user_cache_active_context()
	) {
		if (evict_stale) {
			zend_hash_del(*slots_ptr, key);
		}

		return NULL;
	}

	return slot;
}

static php_user_cache_request_local_slot_result user_cache_fetch_request_local_slot(
		zend_string *key,
		uint64_t gen,
		zval *return_value)
{
	php_user_cache_request_local_slot *slot = user_cache_find_request_local_slot(key, gen, true);

	if (slot == NULL || !slot->has_value) {
		return PHP_USER_CACHE_REQUEST_LOCAL_SLOT_MISS;
	}

	if (!user_cache_clone_request_local_slot_value_known(
			return_value,
			&slot->value,
			slot->needs_deep_clone,
			slot->has_clone_verdicts ? &slot->clone_verdicts : NULL,
			slot->no_aliases
		)
	) {
		zend_hash_del(UC_G(request_local_slot_table), key);

		return PHP_USER_CACHE_REQUEST_LOCAL_SLOT_MISS;
	}

	return PHP_USER_CACHE_REQUEST_LOCAL_SLOT_HIT;
}

static void user_cache_mark_request_local_slot(zend_string *key, uint64_t gen)
{
	php_user_cache_request_local_slot *slot = user_cache_alloc_request_local_slot(gen, true, false);

	zend_hash_update_ptr(user_cache_request_local_slots(), key, slot);
}

static void user_cache_throw_key_not_found_guarded(zend_string *key)
{
	PHP_USER_CACHE_TRY_UNLOCK_ON_BAILOUT(
		zend_throw_exception_ex(php_user_cache_exception_ce, 0, "Cache key \"%s\" was not found", ZSTR_VAL(key));
	);
}

static void user_cache_throw_unknown_value_type_guarded(zend_string *key, const char *cache_name)
{
	PHP_USER_CACHE_TRY_UNLOCK_ON_BAILOUT(
		zend_throw_exception_ex(php_user_cache_exception_ce, 0, "Stored %s value for key \"%s\" has an unknown type", cache_name, ZSTR_VAL(key));
	);
}

static bool user_cache_find_slot_in_header_locked(
		php_user_cache_header *header,
		zend_string *key,
		zend_ulong hash,
		php_user_cache_find_slot_expiry_mode expiry_mode,
		uint32_t *slot_idx,
		bool *found)
{
	php_user_cache_entry *entries, *entry;
	uint64_t now = 0;
	uint32_t i, first_tombstone = UINT32_MAX, step;

	if (header == NULL) {
		return false;
	}

	entries = php_user_cache_entries_ptr(header);
	i = (uint32_t) (hash % header->capacity);

	for (step = 0; step < header->capacity; step++) {
		entry = &entries[i];

		if (entry->state == PHP_USER_CACHE_ENTRY_EMPTY) {
			*slot_idx = first_tombstone != UINT32_MAX ? first_tombstone : i;
			*found = false;

			return true;
		}

		if (entry->state == PHP_USER_CACHE_ENTRY_TOMBSTONE) {
			if (first_tombstone == UINT32_MAX) {
				first_tombstone = i;
			}
		} else if (expiry_mode != PHP_USER_CACHE_FIND_SLOT_IGNORE_EXPIRY &&
			user_cache_is_expired_now(entry, &now)
		) {
			if (expiry_mode == PHP_USER_CACHE_FIND_SLOT_DELETE_EXPIRED) {
				user_cache_delete_entry_locked(header, entry);
			} else {
				user_cache_note_expired_read();
			}

			if (first_tombstone == UINT32_MAX) {
				first_tombstone = i;
			}
		} else if (user_cache_key_equals(header, entry, key, hash)) {
			*slot_idx = i;
			*found = true;

			return true;
		}

		++i;

		if (i == header->capacity) {
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

static bool user_cache_find_slot_locked(
		zend_string *key,
		zend_ulong hash,
		php_user_cache_find_slot_expiry_mode expiry_mode,
		php_user_cache_header **header_ptr,
		uint32_t *slot_idx,
		bool *found)
{
	php_user_cache_header *header = php_user_cache_header_ptr();

	if (!header || !php_user_cache_header_init_locked()) {
		return false;
	}

	if (header_ptr != NULL) {
		*header_ptr = header;
	}

	return user_cache_find_slot_in_header_locked(
		header,
		key,
		hash,
		expiry_mode,
		slot_idx,
		found
	);
}

static bool user_cache_find_slot_ignore_expiry_locked(
		zend_string *key,
		zend_ulong hash,
		php_user_cache_header **header_ptr,
		uint32_t *slot_idx,
		bool *found)
{
	return user_cache_find_slot_locked(
		key,
		hash,
		PHP_USER_CACHE_FIND_SLOT_IGNORE_EXPIRY,
		header_ptr,
		slot_idx,
		found
	);
}

static bool user_cache_find_slot_for_read_locked(
		zend_string *key,
		zend_ulong hash,
		php_user_cache_header **header_ptr,
		uint32_t *slot_idx,
		bool *found)
{
	return user_cache_find_slot_locked(
		key,
		hash,
		PHP_USER_CACHE_FIND_SLOT_SKIP_EXPIRED,
		header_ptr,
		slot_idx,
		found
	);
}

static bool user_cache_find_slot_for_write_locked(
		zend_string *key,
		zend_ulong hash,
		php_user_cache_header **header_ptr,
		uint32_t *slot_idx,
		bool *found)
{
	php_user_cache_header *header;

	if (!user_cache_find_slot_locked(
			key,
			hash,
			PHP_USER_CACHE_FIND_SLOT_DELETE_EXPIRED,
			header_ptr,
			slot_idx,
			found
		)
	) {
		return false;
	}

	header = *header_ptr;
	/* Keep at least 1/8 of the table free so linear probing always terminates
	 * on an EMPTY slot. */
	if (!*found && header->count >= header->capacity - header->capacity / 8) {
		return false;
	}

	return true;
}

static void user_cache_rehash_locked(php_user_cache_header *header)
{
	php_user_cache_entry *entries, *snapshot, *entry, *target;
	uint32_t i, slot, step;

	entries = php_user_cache_entries_ptr(header);
	snapshot = emalloc((size_t) header->capacity * sizeof(*snapshot));

	memcpy(snapshot, entries, (size_t) header->capacity * sizeof(*snapshot));
	memset(entries, 0, (size_t) header->capacity * sizeof(*entries));

	header->count = 0;
	header->tombstone_count = 0;

	for (i = 0; i < header->capacity; i++) {
		entry = &snapshot[i];

		if (entry->state != PHP_USER_CACHE_ENTRY_USED) {
			continue;
		}

		slot = (uint32_t) (entry->hash % header->capacity);
		for (step = 0; step < header->capacity; step++) {
			target = &entries[slot];

			if (target->state == PHP_USER_CACHE_ENTRY_EMPTY) {
				*target = *entry;
				header->count++;

				break;
			}

			++slot;

			if (slot == header->capacity) {
				slot = 0;
			}
		}
	}

	efree(snapshot);

	php_user_cache_bump_mutation_epoch_locked(header);
}

static bool user_cache_expunge_expired_locked(void)
{
	php_user_cache_header *header = php_user_cache_header_ptr();
	php_user_cache_entry *entries;
	uint64_t now;
	uint32_t i;
	bool removed = false;

	if (!header || !php_user_cache_header_init_locked()) {
		return false;
	}

	now = (uint64_t) time(NULL);
	entries = php_user_cache_entries_ptr(header);
	for (i = 0; i < header->capacity; i++) {
		if (user_cache_is_expired(&entries[i], now)) {
			user_cache_delete_entry_locked(header, &entries[i]);
			removed = true;
		}
	}

	php_user_cache_shared_graph_reclaim_orphaned_locked();

	return removed;
}

/* Bound each expiry scan and resume from a process-local cursor. */
static bool user_cache_expunge_expired_bounded_locked(void)
{
	php_user_cache_header *header = php_user_cache_header_ptr();
	php_user_cache_entry *entries;
	uint64_t now;
	uint32_t cursor, scan_len, i;
	bool removed = false;

	if (!header || !php_user_cache_header_init_locked()) {
		return false;
	}

	now = (uint64_t) time(NULL);
	entries = php_user_cache_entries_ptr(header);

	cursor = UC_G(expired_expunge_cursor);
	if (cursor >= header->capacity) {
		cursor = 0;
	}

	scan_len = header->capacity < PHP_USER_CACHE_EXPUNGE_SCAN_MAX
		? header->capacity
		: PHP_USER_CACHE_EXPUNGE_SCAN_MAX
	;
	for (i = 0; i < scan_len; i++) {
		if (user_cache_is_expired(&entries[cursor], now)) {
			user_cache_delete_entry_locked(header, &entries[cursor]);
			removed = true;
		}

		++cursor;

		if (cursor == header->capacity) {
			cursor = 0;
		}
	}

	UC_G(expired_expunge_cursor) = cursor;

	php_user_cache_shared_graph_reclaim_orphaned_locked();

	return removed;
}

static void user_cache_handle_store_failure(const char *msg, bool throw_on_failure)
{
	if (throw_on_failure) {
		zend_throw_exception_ex(php_user_cache_exception_ce, 0, "%s", msg);
	}
}

static bool user_cache_safe_direct_value_has_unstorable(
		void *ctx_ptr,
		const zval *value)
{
	php_user_cache_unstorable_context *ctx = ctx_ptr;

	return user_cache_find_unstorable_value(
		(zval *) value,
		ctx->seen_arrays,
		ctx->seen_objects,
		ctx->failure_message
	);
}

static php_user_cache_object_storability user_cache_object_class_storability(
		zend_class_entry *ce)
{
	if (php_user_cache_class_uses_magic_serialize(ce) ||
		php_user_cache_class_uses_serialize_props(ce)
	) {
		return PHP_USER_CACHE_OBJECT_STORABLE_VIA_HOOKS;
	}

	if (php_user_cache_class_uses_magic_unserialize(ce)) {
		return PHP_USER_CACHE_OBJECT_STORABLE_VIA_HOOKS;
	}

	if (php_user_cache_class_uses_serdes(ce)) {
		return PHP_USER_CACHE_OBJECT_SERDES;
	}

	if ((ce->ce_flags & ZEND_ACC_ENUM) == 0 &&
		php_user_cache_safe_direct_state_serialize_func(ce) == NULL &&
		(
			(ce->ce_flags & ZEND_ACC_NOT_SERIALIZABLE) != 0 ||
			(ce->type != ZEND_USER_CLASS && ce->create_object != NULL)
		)
	) {
		return PHP_USER_CACHE_OBJECT_OPAQUE;
	}

	return PHP_USER_CACHE_OBJECT_SCAN_MEMBERS;
}

static bool user_cache_find_unstorable_in_array(
		zval *value,
		HashTable *seen_arrs,
		HashTable *seen_objs,
		const char **msg)
{
	zval *elem;

	if (!php_user_cache_seen_test_and_add(seen_arrs, Z_ARR_P(value))) {
		return false;
	}

	ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(value), elem) {
		if (user_cache_find_unstorable_value(elem, seen_arrs, seen_objs, msg)) {
			return true;
		}
	} ZEND_HASH_FOREACH_END();

	return false;
}

static bool user_cache_find_unstorable_in_object(
		zval *value,
		HashTable *seen_arrs,
		HashTable *seen_objs,
		const char **msg)
{
	php_user_cache_safe_direct_state_has_unstorable_func_t state_has_unstorable;
	php_user_cache_unstorable_context uctx;
	php_user_cache_object_storability storability;
	zend_object *obj = Z_OBJ_P(value);
	zval *elem, *prop, *end;

	if (zend_object_is_lazy(obj)) {
		*msg = PHP_USER_CACHE_MSG_LAZY_OBJECT_UNSTORABLE;

		return true;
	}

	if (!php_user_cache_seen_test_and_add(seen_objs, obj)) {
		return false;
	}

	storability = user_cache_object_class_storability(obj->ce);
	if (storability == PHP_USER_CACHE_OBJECT_STORABLE_VIA_HOOKS) {
		return false;
	}

	state_has_unstorable = php_user_cache_safe_direct_state_has_unstorable_func(obj->ce);
	if (state_has_unstorable != NULL) {
		uctx.seen_arrays = seen_arrs;
		uctx.seen_objects = seen_objs;
		uctx.failure_message = msg;

		if (state_has_unstorable(
				&uctx,
				value,
				user_cache_safe_direct_value_has_unstorable)
		) {
			if (*msg == NULL) {
				*msg = "objects of this class contain values that cannot be stored in the user cache";
			}

			return true;
		}
	}

	if (storability == PHP_USER_CACHE_OBJECT_SERDES) {
		return false;
	}

	if (storability == PHP_USER_CACHE_OBJECT_OPAQUE) {
		*msg = PHP_USER_CACHE_MSG_OPAQUE_OBJECT_UNSTORABLE;

		return true;
	}

	if (obj->ce->default_properties_count != 0) {
		prop = obj->properties_table;
		end = prop + obj->ce->default_properties_count;

		do {
			if (Z_TYPE_P(prop) != IS_UNDEF &&
				user_cache_find_unstorable_value(
					prop, seen_arrs, seen_objs, msg
				)
			) {
				return true;
			}

			prop++;
		} while (prop != end);
	}

	if (obj->properties != NULL) {
		ZEND_HASH_FOREACH_VAL(obj->properties, elem) {
			if (Z_TYPE_P(elem) == IS_INDIRECT) {
				elem = Z_INDIRECT_P(elem);
				if (Z_TYPE_P(elem) == IS_UNDEF) {
					continue;
				}
			}

			if (user_cache_find_unstorable_value(elem, seen_arrs, seen_objs, msg)) {
				return true;
			}
		} ZEND_HASH_FOREACH_END();
	}

	return false;
}

static bool user_cache_find_unstorable_value(
		zval *value,
		HashTable *seen_arrs,
		HashTable *seen_objs,
		const char **msg)
{
	if (php_user_cache_stack_overflowed()) {
		*msg = "value is nested too deeply to be stored in the user cache";

		return true;
	}

	ZVAL_DEREF(value);

	if (Z_TYPE_P(value) == IS_RESOURCE) {
		*msg = PHP_USER_CACHE_MSG_RESOURCE_UNSTORABLE;

		return true;
	}

	if (Z_TYPE_P(value) == IS_OBJECT && Z_OBJCE_P(value) == zend_ce_closure) {
		*msg = PHP_USER_CACHE_MSG_CLOSURE_UNSTORABLE;

		return true;
	}

	if (Z_TYPE_P(value) == IS_ARRAY) {
		return user_cache_find_unstorable_in_array(value, seen_arrs, seen_objs, msg);
	}

	if (Z_TYPE_P(value) == IS_OBJECT) {
		return user_cache_find_unstorable_in_object(value, seen_arrs, seen_objs, msg);
	}

	return false;
}

static bool user_cache_validate_storable_value(zval *value, bool throw_on_failure)
{
	const char *msg = NULL;
	HashTable seen_arrs, seen_objs;
	bool found;

	ZVAL_DEREF(value);

	if (Z_TYPE_P(value) != IS_ARRAY && Z_TYPE_P(value) != IS_OBJECT) {
		return true;
	}

	zend_hash_init(&seen_arrs, 8, NULL, NULL, 0);
	zend_hash_init(&seen_objs, 8, NULL, NULL, 0);

	found = user_cache_find_unstorable_value(value, &seen_arrs, &seen_objs, &msg);

	zend_hash_destroy(&seen_objs);
	zend_hash_destroy(&seen_arrs);

	if (EG(exception)) {
		return false;
	}

	if (!found) {
		return true;
	}

	if (msg != NULL) {
		zend_type_error("%s", msg);
	}

	return false;
}

static uint8_t *user_cache_reserve_combined_value_key_locked(
		const php_user_cache_header *header,
		uint32_t reusable_offset,
		zend_string *key,
		size_t payload_size,
		uint32_t *value_offset,
		uint32_t *key_offset)
{
	uint32_t base_offset;
	size_t key_size, total_size;

	key_size = ZSTR_LEN(key) + 1;
	if (payload_size > SIZE_MAX - key_size) {
		return NULL;
	}

	total_size = payload_size + key_size;
	if (reusable_offset != 0 &&
		php_user_cache_block_payload_capacity(reusable_offset) >= total_size
	) {
		base_offset = reusable_offset;
	} else {
		base_offset = php_user_cache_alloc_locked(total_size, NULL);
		if (base_offset == 0) {
			return NULL;
		}
	}

	*value_offset = base_offset;
	*key_offset = base_offset + (uint32_t) payload_size;

	return user_cache_ptr_in_header(header, base_offset);
}

static bool user_cache_publish_combined_value_key_locked(
		const php_user_cache_header *header,
		uint32_t reusable_offset,
		zend_string *key,
		size_t payload_size,
		const void *src,
		uint32_t *value_offset,
		uint32_t *key_offset)
{
	uint8_t *payload;

	if (src == NULL) {
		return false;
	}

	payload = user_cache_reserve_combined_value_key_locked(
		header,
		reusable_offset,
		key,
		payload_size,
		value_offset,
		key_offset
	);
	if (payload == NULL) {
		return false;
	}

	memcpy(payload, src, payload_size);
	memcpy(payload + payload_size, ZSTR_VAL(key), ZSTR_LEN(key) + 1);

	return true;
}

/* String roots are copied in place; other graphs normally use the staging
 * buffer prepared before taking the write lock. */
static bool user_cache_publish_prepared_shared_graph_locked(
		const php_user_cache_prepared_value *prepared,
		zval *value,
		uint8_t *payload)
{
	zval pinned_root;

	if (prepared->payload_source != NULL &&
		php_user_cache_shared_graph_publish_copied_payload_locked(
			payload,
			prepared->payload_size,
			prepared->payload_source,
			prepared->payload_size,
			prepared->payload_used_size,
			prepared->has_verbatim_array,
			prepared->fixup_offsets,
			prepared->fixup_count
		)
	) {
		return true;
	}

	/* Use the string pinned during the sizing pass. */
	if (prepared->owned_string != NULL) {
		ZVAL_STR(&pinned_root, prepared->owned_string);
		value = &pinned_root;
	}

	return php_user_cache_build_shared_graph_in_place(
		value,
		prepared->state_memo,
		NULL,
		payload,
		prepared->payload_size,
		NULL,
		NULL,
		NULL,
		NULL
	);
}

static bool user_cache_reclaim_space_for_store_locked(
		bool can_clear,
		bool *expired_retry_used,
		bool *clear_retry_used)
{
	bool reclaimed;

	if (!*expired_retry_used) {
		*expired_retry_used = true;

		reclaimed = user_cache_expunge_expired_locked();

		/* Pins left by killed workers hold payload blocks the expiry pass
		 * cannot see; strip them before falling through to a full clear. */
		if (php_user_cache_shared_graph_strip_dead_pins_locked(true)) {
			reclaimed = true;
		}

		if (reclaimed) {
			return true;
		}
	}

	if (can_clear && php_user_cache_active_context()->clear_on_pressure && !*clear_retry_used) {
		*clear_retry_used = true;

		if (php_user_cache_entry_locks_allow_clear_locked() && php_user_cache_clear_locked()) {
			php_user_cache_header_ptr()->expunge_count++;

			return true;
		}
	}

	return false;
}

static void user_cache_rollback_partial_store_allocs_locked(
		uint32_t new_value_offset,
		uint32_t old_value_offset,
		uint32_t new_key_offset,
		uint32_t old_key_offset,
		uint16_t new_reserved)
{
	if (new_value_offset != 0 && new_value_offset != old_value_offset) {
		php_user_cache_free_locked(new_value_offset);
	}

	if (new_key_offset != 0 && new_key_offset != old_key_offset &&
		(new_reserved & PHP_USER_CACHE_ENTRY_RESERVED_COMBINED_VALUE_KEY) == 0
	) {
		php_user_cache_free_locked(new_key_offset);
	}
}

static php_user_cache_store_attempt_result user_cache_store_attempt_locked(
		zend_string *key,
		zval *value,
		const php_user_cache_prepared_value *prepared,
		zend_long ttl,
		const php_user_cache_store_options *options,
		php_user_cache_store_result *result,
		size_t key_size,
		bool *expired_retry_used,
		bool *clear_retry_used)
{
	const char *msg;
	php_user_cache_header *header;
	php_user_cache_entry *entries, *entry, replaced_snapshot;
	zend_long new_lval = 0;
	uint64_t expires_at;
	uint32_t slot_idx, offset = 0, graph_offset = 0, reusable_offset, old_key_offset = 0, old_value_offset = 0,
			new_key_offset = 0, new_value_offset = 0, new_value_len = 0, combined_reuse_offset = 0;
	uint16_t old_reserved = 0, new_reserved = 0;
	uint8_t *combined_payload, old_value_type = PHP_USER_CACHE_VALUE_NULL, new_value_type = prepared->value_type;
	bool found, old_combined, use_combined_publish, can_clear, capture_replaced = false, published = false;
	double new_dval = 0;

	expires_at = ttl == 0
		? 0
		: (uint64_t) time(NULL) + (uint64_t) ttl
	;

	if (!user_cache_find_slot_for_write_locked(
			key,
			prepared->hash,
			&header,
			&slot_idx,
			&found
		)
	) {
		if (options->retry_after_memory_pressure &&
			user_cache_reclaim_space_for_store_locked(
				true,
				expired_retry_used,
				clear_retry_used
			)
		) {
			return PHP_USER_CACHE_STORE_ATTEMPT_RETRY;
		}

		php_user_cache_header_ptr()->store_failure_count++;
		user_cache_handle_store_failure(
			"cache hash table is full",
			options->throw_on_failure
		);

		return PHP_USER_CACHE_STORE_ATTEMPT_FAILED;
	}

	entries = php_user_cache_entries_ptr(header);
	entry = &entries[slot_idx];

	if (found) {
		capture_replaced = options->capture_replaced_entry && result != NULL;
		replaced_snapshot = *entry;
		old_key_offset = entry->key_offset;
		old_value_type = entry->value_type;
		old_value_offset = entry->value_offset;
		old_reserved = entry->reserved;
	}

	old_combined = found && (old_reserved & PHP_USER_CACHE_ENTRY_RESERVED_COMBINED_VALUE_KEY) != 0;
	reusable_offset = 0;

	if (found && !capture_replaced && old_value_type != PHP_USER_CACHE_VALUE_SHARED_GRAPH && !old_combined) {
		reusable_offset = old_value_offset;
	}

	new_key_offset = found ? old_key_offset : 0;

	use_combined_publish = user_cache_value_uses_offset(prepared->value_type) &&
		(!found || old_combined)
	;

	if (old_combined && old_value_offset != 0 && !capture_replaced) {
		if (old_value_type == PHP_USER_CACHE_VALUE_SHARED_GRAPH) {
			/* The sizing pass includes worst-case alignment padding. */
			if ((prepared->payload_source != NULL ||
					prepared->value_type == PHP_USER_CACHE_VALUE_SHARED_GRAPH) &&
				php_user_cache_shared_graph_can_overwrite_payload_locked(old_value_offset) &&
				(prepared->value_type != PHP_USER_CACHE_VALUE_SHARED_GRAPH ||
					prepared->payload_source == NULL ||
					php_user_cache_shared_graph_copy_fits_buffer(
						user_cache_ptr_in_header(header, old_value_offset),
						prepared->payload_size,
						prepared->payload_source,
						prepared->payload_size,
						prepared->payload_used_size
					)
				)
			) {
				combined_reuse_offset = old_value_offset;
			}
		} else if (header->count == 1) {
			combined_reuse_offset = old_value_offset;
		}
	}

	if (!use_combined_publish && (!found || old_combined)) {
		new_key_offset = php_user_cache_alloc_locked(key_size, ZSTR_VAL(key));
		if (new_key_offset == 0) {
			msg = "not enough shared memory left";
			can_clear = user_cache_payload_can_fit_locked(key_size);

			goto failure;
		}
	}

	switch (prepared->value_type) {
		case PHP_USER_CACHE_VALUE_NULL:
			new_value_type = PHP_USER_CACHE_VALUE_NULL;

			break;
		case PHP_USER_CACHE_VALUE_TRUE:
			new_value_type = PHP_USER_CACHE_VALUE_TRUE;

			break;
		case PHP_USER_CACHE_VALUE_FALSE:
			new_value_type = PHP_USER_CACHE_VALUE_FALSE;

			break;
		case PHP_USER_CACHE_VALUE_LONG:
			new_value_type = PHP_USER_CACHE_VALUE_LONG;
			new_lval = prepared->long_value;

			break;
		case PHP_USER_CACHE_VALUE_DOUBLE:
			new_value_type = PHP_USER_CACHE_VALUE_DOUBLE;
			new_dval = prepared->double_value;

			break;
		case PHP_USER_CACHE_VALUE_STRING:
			if (use_combined_publish) {
				if (!user_cache_publish_combined_value_key_locked(
						header,
						combined_reuse_offset,
						key,
						prepared->payload_size,
						prepared->payload_source,
						&new_value_offset,
						&new_key_offset)
				) {
					msg = "not enough shared memory left";
					can_clear = user_cache_payload_can_fit_locked(prepared->payload_size + key_size);

					goto failure;
				}

				new_reserved = PHP_USER_CACHE_ENTRY_RESERVED_COMBINED_VALUE_KEY;
			} else {
				offset = user_cache_write_payload_locked(
					header,
					reusable_offset,
					prepared->payload_size,
					prepared->payload_source
				);
				if (offset == 0) {
					msg = "not enough shared memory left";
					can_clear = user_cache_payload_can_fit_locked(prepared->payload_size);

					goto failure;
				}

				new_value_offset = offset;
			}

			new_value_type = prepared->value_type;
			new_value_len = prepared->value_len;

			break;
		case PHP_USER_CACHE_VALUE_SHARED_GRAPH:
			if (use_combined_publish) {
				combined_payload = user_cache_reserve_combined_value_key_locked(
					header,
					combined_reuse_offset,
					key,
					prepared->payload_size,
					&new_value_offset,
					&new_key_offset
				);

				graph_offset = new_value_offset;

				if (combined_payload != NULL) {
					/* A reused block remains attached so a later read can discard it. */
					zend_try {
						published = user_cache_publish_prepared_shared_graph_locked(
							prepared,
							value,
							combined_payload
						);
					} zend_catch {
						if (graph_offset != combined_reuse_offset) {
							php_user_cache_free_locked(graph_offset);
						}
						if (!UC_G(store_defer_unlock)) {
							php_user_cache_unlock_if_held();
						}
						zend_bailout();
					} zend_end_try();

					if (published) {
						memcpy(combined_payload + prepared->payload_size, ZSTR_VAL(key), key_size);
						new_reserved = PHP_USER_CACHE_ENTRY_RESERVED_COMBINED_VALUE_KEY;
						new_value_type = PHP_USER_CACHE_VALUE_SHARED_GRAPH;
						new_value_offset = graph_offset;
						new_value_len = prepared->value_len;

						break;
					}

					if (graph_offset != combined_reuse_offset) {
						php_user_cache_free_locked(graph_offset);
					} else {
						/* Detach the partially consumed combined block first. */
						entry->value_type = PHP_USER_CACHE_VALUE_NULL;
						entry->value_offset = 0;

						user_cache_delete_entry_locked(header, entry);
						php_user_cache_free_locked(combined_reuse_offset);

						combined_reuse_offset = 0;
						old_value_offset = 0;
						old_key_offset = 0;
						found = false;
					}

					graph_offset = 0;
					new_value_offset = 0;
					new_key_offset = found ? old_key_offset : 0;

					if (EG(exception)) {
						return PHP_USER_CACHE_STORE_ATTEMPT_FAILED;
					}
				} else if (options->retry_after_memory_pressure &&
					user_cache_payload_can_fit_locked(prepared->payload_size + key_size) &&
					user_cache_reclaim_space_for_store_locked(true, expired_retry_used, clear_retry_used)
				) {
					return PHP_USER_CACHE_STORE_ATTEMPT_RETRY;
				}
			} else {
				graph_offset = php_user_cache_alloc_locked(prepared->payload_size, NULL);

				if (graph_offset != 0) {
					/* Avoid leaking the block on an allocation bailout. */
					zend_try {
						published = user_cache_publish_prepared_shared_graph_locked(
							prepared,
							value,
							user_cache_ptr_in_header(header, graph_offset)
						);
					} zend_catch {
						php_user_cache_free_locked(graph_offset);
						if (!UC_G(store_defer_unlock)) {
							php_user_cache_unlock_if_held();
						}
						zend_bailout();
					} zend_end_try();

					if (published) {
						new_value_type = PHP_USER_CACHE_VALUE_SHARED_GRAPH;
						new_value_offset = graph_offset;
						new_value_len = prepared->value_len;

						break;
					}

					php_user_cache_free_locked(graph_offset);

					if (EG(exception)) {
						return PHP_USER_CACHE_STORE_ATTEMPT_FAILED;
					}
				} else if (options->retry_after_memory_pressure &&
					user_cache_payload_can_fit_locked(prepared->payload_size) &&
					user_cache_reclaim_space_for_store_locked(true, expired_retry_used, clear_retry_used)
				) {
					return PHP_USER_CACHE_STORE_ATTEMPT_RETRY;
				}
			}

			msg = "not enough shared memory left";
			can_clear = user_cache_payload_can_fit_locked(prepared->payload_size);

			goto failure;
		default:
			ZEND_UNREACHABLE();
	}

	if (!found && entry->state == PHP_USER_CACHE_ENTRY_TOMBSTONE && header->tombstone_count != 0) {
		header->tombstone_count--;
	}

	entry->hash = prepared->hash;
	entry->key_offset = new_key_offset;
	entry->key_len = (uint32_t) ZSTR_LEN(key);
	entry->value_offset = new_value_offset;
	entry->value_len = new_value_len;
	entry->expires_at = expires_at;
	entry->state = PHP_USER_CACHE_ENTRY_USED;
	entry->value_type = new_value_type;
	entry->reserved = new_reserved;
	entry->long_value = new_lval;
	entry->double_value = new_dval;

	if (capture_replaced) {
		result->replaced_entry.found = true;
		result->replaced_entry.entry = replaced_snapshot;
	} else {
		if (found &&
			old_key_offset != 0 &&
			old_key_offset != new_key_offset &&
			!old_combined
		) {
			php_user_cache_free_locked(old_key_offset);
		}

		if (found &&
			old_value_offset != 0 &&
			old_value_offset != new_value_offset
		) {
			user_cache_release_value_storage_locked(old_value_type, old_value_offset);
		}
	}

	if (!found) {
		header->count++;
	}

	php_user_cache_bump_mutation_epoch_locked(header);

	/* Per-entry generations avoid invalidating unrelated request-local slots. */
	entry->generation = header->mutation_epoch;

	if (result != NULL) {
		result->stored_generation = entry->generation;
		result->should_seed_request_local_slot = user_cache_prepared_value_should_seed_request_local_slot(prepared);
	}

	return PHP_USER_CACHE_STORE_ATTEMPT_STORED;

failure:
	user_cache_rollback_partial_store_allocs_locked(
		new_value_offset,
		old_value_offset,
		new_key_offset,
		old_key_offset,
		new_reserved
	);

	if (options->retry_after_memory_pressure &&
		user_cache_reclaim_space_for_store_locked(can_clear, expired_retry_used, clear_retry_used)
	) {
		return PHP_USER_CACHE_STORE_ATTEMPT_RETRY;
	}

	header->store_failure_count++;
	user_cache_handle_store_failure(
		msg,
		options->throw_on_failure
	);

	return PHP_USER_CACHE_STORE_ATTEMPT_FAILED;
}

static bool user_cache_store_prepared_locked_impl(
		zend_string *key,
		zval *value,
		const php_user_cache_prepared_value *prepared,
		zend_long ttl,
		const php_user_cache_store_options *options,
		php_user_cache_store_result *result)
{
	php_user_cache_store_attempt_result attempt_result;
	size_t key_size;
	bool expired_retry_used = false, clear_retry_used = false;

	ZVAL_DEREF(value);

	if (result != NULL) {
		result->stored_generation = 0;
		result->should_seed_request_local_slot = false;
		result->replaced_entry.found = false;

		memset(&result->replaced_entry.entry, 0, sizeof(result->replaced_entry.entry));
	}

	if (prepared == NULL || options == NULL) {
		return false;
	}

	key_size = ZSTR_LEN(key) + 1;

	user_cache_maybe_rehash_locked();

	for (;;) {
		attempt_result = user_cache_store_attempt_locked(
			key,
			value,
			prepared,
			ttl,
			options,
			result,
			key_size,
			&expired_retry_used,
			&clear_retry_used
		);

		if (attempt_result == PHP_USER_CACHE_STORE_ATTEMPT_RETRY) {
			continue;
		}

		return attempt_result == PHP_USER_CACHE_STORE_ATTEMPT_STORED;
	}
}

static bool user_cache_prepare_direct_value(
		zval *value,
		php_user_cache_prepared_value *prepared)
{
	switch (Z_TYPE_P(value)) {
		case IS_NULL:
			prepared->value_type = PHP_USER_CACHE_VALUE_NULL;

			return true;
		case IS_TRUE:
			prepared->value_type = PHP_USER_CACHE_VALUE_TRUE;

			return true;
		case IS_FALSE:
			prepared->value_type = PHP_USER_CACHE_VALUE_FALSE;

			return true;
		case IS_LONG:
			prepared->value_type = PHP_USER_CACHE_VALUE_LONG;
			prepared->long_value = Z_LVAL_P(value);

			return true;
		case IS_DOUBLE:
			prepared->value_type = PHP_USER_CACHE_VALUE_DOUBLE;
			prepared->double_value = Z_DVAL_P(value);

			return true;
		case IS_STRING:
			if (Z_STRLEN_P(value) < PHP_USER_CACHE_DIRECT_STRING_MIN_LEN) {
				prepared->value_type = PHP_USER_CACHE_VALUE_STRING;
				prepared->value_len = (uint32_t) Z_STRLEN_P(value);
				prepared->payload_size = Z_STRLEN_P(value) + 1;
				prepared->payload_used_size = prepared->payload_size;
				prepared->payload_source = (const uint8_t *) Z_STRVAL_P(value);

				return true;
			}

			return false;
		default:
			return false;
	}
}

static bool user_cache_prepare_shared_graph_value(
		zval *value,
		const php_user_cache_prepare_options *options,
		HashTable *verbatim_verdicts,
		size_t verbatim_graph_len,
		php_user_cache_prepared_value *prepared)
{
	size_t graph_len = 0;
	bool sized, build_at_publish;

	/* Copy string roots directly under the write lock; this path cannot invoke
	 * user code. */
	build_at_publish = options->caller_holds_write_lock || Z_TYPE_P(value) == IS_STRING;

	if (!build_at_publish && prepared->state_memo == NULL) {
		prepared->state_memo = emalloc(sizeof(HashTable));
		zend_hash_init(prepared->state_memo, 8, NULL, ZVAL_PTR_DTOR, 0);
	}

	if (verbatim_graph_len != 0) {
		/* The verbatim pass already computed the exact size. */
		graph_len = verbatim_graph_len;
		sized = true;
	} else {
		sized = php_user_cache_calculate_shared_graph_size(
			value,
			prepared->state_memo,
			verbatim_verdicts,
			&graph_len
		);
	}

	if (sized) {
		/* Do not truncate the graph length. */
		if (graph_len > UINT32_MAX) {
			return false;
		}

		prepared->value_type = PHP_USER_CACHE_VALUE_SHARED_GRAPH;
		prepared->value_len = (uint32_t) graph_len;
		prepared->payload_size = graph_len;

		if (build_at_publish) {
			if (Z_TYPE_P(value) == IS_STRING) {
				prepared->owned_string = zend_string_copy(Z_STR_P(value));
			}

			return true;
		}

		prepared->owned_buffer = emalloc(graph_len);
		/* Snapshot hooks may invalidate address-keyed verdicts. */
		if (php_user_cache_build_shared_graph_in_place(
				value,
				prepared->state_memo,
				zend_hash_num_elements(prepared->state_memo) == 0 ? verbatim_verdicts : NULL,
				prepared->owned_buffer,
				graph_len,
				&prepared->payload_used_size,
				&prepared->has_verbatim_array,
				&prepared->fixup_offsets,
				&prepared->fixup_count
			)
		) {
			prepared->payload_source = prepared->owned_buffer;

			return true;
		}

		if (EG(exception)) {
			return false;
		}

		efree(prepared->owned_buffer);
		prepared->owned_buffer = NULL;
	}

	if (EG(exception)) {
		return false;
	}

	user_cache_handle_store_failure(
		"value cannot be stored in the user cache: it contains a resource, a closure, or an object with opaque internal state (e.g. Fiber, Generator, PDO)",
		options->throw_on_failure
	);

	return false;
}

static bool user_cache_fetch_resolve_prototype_mode_locked(
		const php_user_cache_entry *entry,
		bool use_request_local_slot,
		bool *no_aliases)
{
	*no_aliases = true;

	if (!use_request_local_slot) {
		return false;
	}

	switch (entry->value_type) {
		case PHP_USER_CACHE_VALUE_SHARED_GRAPH:
			if (!php_user_cache_shared_graph_prefers_prototype(entry->value_offset)) {
				return false;
			}

			*no_aliases = !php_user_cache_shared_graph_payload_has_aliases(entry->value_offset);

			return true;
		case PHP_USER_CACHE_VALUE_STRING:
			/* Repeated string fetches use the request-local slot. */
			return true;
		default:
			/* Scalar hits are served by the lookup cache. */
			return false;
	}
}

/* Defer request-local cloning until after the read lock is released. */
static bool user_cache_fetch_emit_value_locked(
		const php_user_cache_header *header,
		zend_string *key,
		const char *cache_name,
		php_user_cache_entry *entry,
		uint64_t gen,
		bool throw_if_missing,
		bool use_request_local_slot,
		zval *return_value,
		php_user_cache_fetch_pending_seed *pending_seed)
{
	php_user_cache_request_local_slot_result slot_result;
	uint32_t flags;
	bool use_proto, no_aliases;

	pending_seed->should_seed_request_local_slot = false;

	use_proto = user_cache_fetch_resolve_prototype_mode_locked(entry, use_request_local_slot, &no_aliases);

	if (use_proto) {
		PHP_USER_CACHE_TRY_UNLOCK_ON_BAILOUT(
			slot_result = user_cache_fetch_request_local_slot(key, gen, return_value);
		);
		if (slot_result == PHP_USER_CACHE_REQUEST_LOCAL_SLOT_HIT) {
			return true;
		}

		if (EG(exception)) {
			return false;
		}
	}

	flags = 0;
	if (use_proto) {
		flags |= PHP_USER_CACHE_FETCH_FINISH_USE_REQUEST_LOCAL_SLOT;
	}
	if (no_aliases) {
		flags |= PHP_USER_CACHE_FETCH_FINISH_NO_ALIASES;
	}

	if (user_cache_scalar_to_zval(entry->value_type, entry->long_value, entry->double_value, return_value)) {
		return true;
	}

	switch (entry->value_type) {
		case PHP_USER_CACHE_VALUE_STRING:
			PHP_USER_CACHE_TRY_UNLOCK_ON_BAILOUT(
				ZVAL_STRINGL(return_value, (const char *) user_cache_ptr_in_header(header, entry->value_offset), entry->value_len);
			);
			pending_seed->generation = gen;
			pending_seed->flags = flags;
			pending_seed->should_seed_request_local_slot = true;

			return true;
		case PHP_USER_CACHE_VALUE_SHARED_GRAPH:
			if (!user_cache_materialize_shared_graph_locked(
					header,
					key,
					cache_name,
					entry->value_type,
					entry->value_offset,
					entry->value_len,
					throw_if_missing,
					return_value)
			) {
				return false;
			}

			pending_seed->generation = gen;
			pending_seed->flags = flags | PHP_USER_CACHE_FETCH_FINISH_DEFER_REQUEST_LOCAL_SLOT;
			pending_seed->should_seed_request_local_slot = true;

			return true;
		default:
			if (throw_if_missing) {
				user_cache_throw_unknown_value_type_guarded(key, cache_name);
			}

			return false;
	}
}

static php_user_cache_fetch_locate_result user_cache_fetch_probe_lookup_cache_locked(
		php_user_cache_header *header,
		zend_string *key,
		zend_ulong hash,
		php_user_cache_entry *entries,
		php_user_cache_lookup_entry *lookup_entries,
		uint64_t epoch,
		zval *return_value,
		uint32_t *slot_idx)
{
	const void *ctx = (const void *) php_user_cache_active_context();
	php_user_cache_lookup_entry *lookup_entry;
	php_user_cache_entry *entry;
	uint64_t now;
	uint32_t way;

	for (way = 0; way < PHP_USER_CACHE_LOOKUP_WAYS; way++) {
		lookup_entry = &lookup_entries[way];
		if (lookup_entry->state == PHP_USER_CACHE_LOOKUP_EMPTY ||
			lookup_entry->hash != hash ||
			lookup_entry->context != ctx
		) {
			continue;
		}

		if (lookup_entry->mutation_epoch != epoch) {
			/* Revalidate stale hits; discard stale misses. */
			if (lookup_entry->state != PHP_USER_CACHE_LOOKUP_HIT) {
				user_cache_lookup_cache_reset_entry(lookup_entry);
				continue;
			}
		} else {
			if (lookup_entry->state == PHP_USER_CACHE_LOOKUP_MISS) {
				/* A colliding MISS must not hide this key. */
				if (lookup_entry->key == NULL || !zend_string_equals(lookup_entry->key, key)) {
					continue;
				}

				return PHP_USER_CACHE_FETCH_LOCATE_MISS;
			}

			if (lookup_entry->key == key &&
				user_cache_scalar_to_zval(
					lookup_entry->value_type,
					lookup_entry->long_value,
					lookup_entry->double_value,
					return_value
				)
			) {
				return PHP_USER_CACHE_FETCH_LOCATE_SCALAR_HIT;
			}
		}

		if (lookup_entry->slot_index >= header->capacity) {
			user_cache_lookup_cache_reset_entry(lookup_entry);
			continue;
		}

		entry = &entries[lookup_entry->slot_index];
		if (!user_cache_key_equals(header, entry, key, hash)) {
			user_cache_lookup_cache_reset_entry(lookup_entry);
			continue;
		}

		now = 0;
		if (user_cache_is_expired_now(entry, &now)) {
			user_cache_note_expired_read();
			user_cache_lookup_cache_reset_entry(lookup_entry);
			user_cache_lookup_cache_store_miss(lookup_entries, hash, epoch, key);

			return PHP_USER_CACHE_FETCH_LOCATE_MISS;
		}

		*slot_idx = lookup_entry->slot_index;

		/* Refresh the revived cache entry. */
		if (lookup_entry->mutation_epoch != epoch || lookup_entry->key != key) {
			user_cache_lookup_cache_store_hit(lookup_entry, hash, epoch, *slot_idx, key, entry);
		}

		return PHP_USER_CACHE_FETCH_LOCATE_SLOT;
	}

	return PHP_USER_CACHE_FETCH_LOCATE_UNCACHED;
}

static php_user_cache_fetch_locate_result user_cache_fetch_probe_entry_table_locked(
		php_user_cache_header *header,
		zend_string *key,
		zend_ulong hash,
		php_user_cache_entry *entries,
		php_user_cache_lookup_entry *lookup_entries,
		uint64_t epoch,
		uint32_t *slot_idx)
{
	php_user_cache_lookup_entry *lookup_entry;
	bool found;

	if (!user_cache_find_slot_in_header_locked(
			header,
			key,
			hash,
			PHP_USER_CACHE_FIND_SLOT_SKIP_EXPIRED,
			slot_idx,
			&found
		) ||
		!found
	) {
		user_cache_lookup_cache_store_miss(lookup_entries, hash, epoch, key);

		return PHP_USER_CACHE_FETCH_LOCATE_MISS;
	}

	lookup_entry = user_cache_lookup_cache_select_slot(lookup_entries, hash, epoch, true);
	user_cache_lookup_cache_store_hit(lookup_entry, hash, epoch, *slot_idx, key, &entries[*slot_idx]);

	return PHP_USER_CACHE_FETCH_LOCATE_SLOT;
}

static bool user_cache_atomic_insert_missing_locked(
		zend_string *key,
		zend_long step,
		zend_long ttl,
		bool decrement,
		php_user_cache_atomic_update_result *result)
{
	php_user_cache_prepare_options prep_opts = {
		.caller_holds_write_lock = true,
		.throw_on_failure = false,
	};
	php_user_cache_store_options store_opts = {
		.retry_after_memory_pressure = true,
		.throw_on_failure = false,
		.capture_replaced_entry = false,
	};
	php_user_cache_prepared_value prepared;
	zend_long updated;
	zval init_val = {0};
	bool is_overflow, stored;

	is_overflow = decrement
		? user_cache_long_sub_overflow(0, step, &updated)
		: user_cache_long_add_overflow(0, step, &updated)
	;
	if (is_overflow) {
		result->is_overflow = true;

		return false;
	}

	ZVAL_LONG(&init_val, updated);
	if (!php_user_cache_prepare_value(key, &init_val, &prep_opts, &prepared)) {
		php_user_cache_destroy_prepared_value(&prepared);

		return false;
	}

	stored = php_user_cache_store_prepared_locked(
		key,
		&init_val,
		&prepared,
		ttl,
		&store_opts,
		NULL
	);

	php_user_cache_destroy_prepared_value(&prepared);

	if (stored) {
		result->new_value = Z_LVAL(init_val);

		return true;
	}

	return false;
}

#ifdef PHP_USER_CACHE_HAVE_OPTIMISTIC
/* Validate the final sequence before publishing the snapshot. */
static php_user_cache_optimistic_result user_cache_optimistic_locate(
		php_user_cache_header *header,
		zend_string *key,
		zend_ulong hash,
		uint64_t seq,
		uint64_t epoch,
		php_user_cache_lookup_entry *lookup_entries,
		bool have_snapshot,
		php_user_cache_entry *snapshot,
		uint32_t *slot_idx)
{
	php_user_cache_lookup_entry *lookup_entry;
	bool found = have_snapshot;

	if (!have_snapshot &&
		!user_cache_optimistic_probe(
			header,
			key,
			hash,
			php_user_cache_entries_ptr(header),
			snapshot,
			slot_idx,
			&found
		)
	) {
		return PHP_USER_CACHE_OPTIMISTIC_FALLBACK;
	}

	if (php_user_cache_seq_reload(&header->write_seq) != seq) {
		return PHP_USER_CACHE_OPTIMISTIC_FALLBACK;
	}

	if (!found) {
		user_cache_lookup_cache_store_miss(lookup_entries, hash, epoch, key);

		return PHP_USER_CACHE_OPTIMISTIC_MISS;
	}

	lookup_entry = user_cache_lookup_cache_select_slot(lookup_entries, hash, epoch, true);
	user_cache_lookup_cache_store_hit(lookup_entry, hash, epoch, *slot_idx, key, snapshot);

	return PHP_USER_CACHE_OPTIMISTIC_FOUND;
}

static bool user_cache_optimistic_scan_lookup_cache(
		php_user_cache_header *header,
		zend_string *key,
		zend_ulong hash,
		uint64_t seq,
		uint64_t epoch,
		php_user_cache_lookup_entry *lookup_entries,
		zval *return_value,
		uint32_t *hint_slot,
		php_user_cache_optimistic_result *result)
{
	const void *ctx = (const void *) php_user_cache_active_context();
	php_user_cache_lookup_entry *lookup_entry;
	uint32_t way;

	*hint_slot = UINT32_MAX;

	for (way = 0; way < PHP_USER_CACHE_LOOKUP_WAYS; way++) {
		lookup_entry = &lookup_entries[way];

		if (lookup_entry->state == PHP_USER_CACHE_LOOKUP_EMPTY ||
			lookup_entry->hash != hash ||
			lookup_entry->context != ctx
		) {
			continue;
		}

		if (lookup_entry->state == PHP_USER_CACHE_LOOKUP_MISS) {
			if (lookup_entry->mutation_epoch != epoch) {
				user_cache_lookup_cache_reset_entry(lookup_entry);

				continue;
			}

			if (lookup_entry->key == NULL || !zend_string_equals(lookup_entry->key, key)) {
				continue;
			}

			*result = php_user_cache_seq_reload(&header->write_seq) != seq
				? PHP_USER_CACHE_OPTIMISTIC_FALLBACK
				: PHP_USER_CACHE_OPTIMISTIC_MISS
			;

			return true;
		}

		if (lookup_entry->mutation_epoch == epoch &&
			lookup_entry->key == key &&
			user_cache_scalar_to_zval(
				lookup_entry->value_type,
				lookup_entry->long_value,
				lookup_entry->double_value,
				return_value
			)
		) {
			*result = php_user_cache_seq_reload(&header->write_seq) != seq
				? PHP_USER_CACHE_OPTIMISTIC_FALLBACK
				: PHP_USER_CACHE_OPTIMISTIC_FOUND
			;

			return true;
		}

		*hint_slot = lookup_entry->slot_index;

		break;
	}

	return false;
}

static bool user_cache_optimistic_try_hint_slot(
		php_user_cache_header *header,
		zend_string *key,
		zend_ulong hash,
		uint32_t hint_slot,
		php_user_cache_entry *snapshot,
		uint32_t *slot_idx)
{
	const php_user_cache_entry *hint_entry;

	if (hint_slot == UINT32_MAX || hint_slot >= header->capacity) {
		return false;
	}

	hint_entry = &php_user_cache_entries_ptr(header)[hint_slot];

	if (hint_entry->state == PHP_USER_CACHE_ENTRY_USED &&
		hint_entry->hash == hash &&
		hint_entry->key_len == ZSTR_LEN(key) &&
		user_cache_optimistic_payload_in_bounds(header, hint_entry->key_offset, hint_entry->key_len) &&
		memcmp(user_cache_ptr_in_header(header, hint_entry->key_offset), ZSTR_VAL(key), ZSTR_LEN(key)) == 0 &&
		(hint_entry->expires_at == 0 || hint_entry->expires_at > (uint64_t) time(NULL))
	) {
		*snapshot = *hint_entry;
		*slot_idx = hint_slot;

		return true;
	}

	return false;
}

static php_user_cache_optimistic_result user_cache_optimistic_emit_string(
		php_user_cache_header *header,
		zend_string *key,
		uint64_t seq,
		const php_user_cache_entry *snapshot,
		zval *return_value)
{
	zend_string *str;

	if (user_cache_fetch_request_local_slot(key, snapshot->generation, return_value) ==
		PHP_USER_CACHE_REQUEST_LOCAL_SLOT_HIT
	) {
		return PHP_USER_CACHE_OPTIMISTIC_FOUND;
	}

	if (!user_cache_optimistic_payload_in_bounds(header, snapshot->value_offset, snapshot->value_len)) {
		return PHP_USER_CACHE_OPTIMISTIC_FALLBACK;
	}

	str = zend_string_init(
		(const char *) user_cache_ptr_in_header(header, snapshot->value_offset),
		snapshot->value_len,
		0
	);

	if (php_user_cache_seq_reload(&header->write_seq) != seq) {
		zend_string_release(str);

		return PHP_USER_CACHE_OPTIMISTIC_FALLBACK;
	}

	ZVAL_STR(return_value, str);
	php_user_cache_fetch_finish(
		key,
		snapshot->generation,
		return_value,
		PHP_USER_CACHE_FETCH_FINISH_USE_REQUEST_LOCAL_SLOT | PHP_USER_CACHE_FETCH_FINISH_NO_ALIASES
	);

	return PHP_USER_CACHE_OPTIMISTIC_FOUND;
}

static php_user_cache_optimistic_result user_cache_optimistic_emit_shared_graph(
		php_user_cache_header *header,
		zend_string *key,
		uint64_t seq,
		const php_user_cache_entry *snapshot,
		zval *return_value)
{
	uint32_t reader_slot = 0, flags;
	bool use_proto, no_aliases, ref_registered;

	if (!user_cache_optimistic_payload_in_bounds(
			header,
			snapshot->value_offset,
			sizeof(php_user_cache_shared_graph_header) + ZEND_MM_ALIGNMENT) ||
		!user_cache_optimistic_payload_in_bounds(
			header,
			snapshot->value_offset,
			snapshot->value_len
		)
	) {
		return PHP_USER_CACHE_OPTIMISTIC_FALLBACK;
	}

	use_proto = php_user_cache_shared_graph_prefers_prototype(snapshot->value_offset);
	no_aliases = !php_user_cache_shared_graph_payload_has_aliases(snapshot->value_offset);

	if (php_user_cache_seq_reload(&header->write_seq) != seq) {
		return PHP_USER_CACHE_OPTIMISTIC_FALLBACK;
	}

	if (use_proto &&
		user_cache_fetch_request_local_slot(
			key, snapshot->generation, return_value
		) == PHP_USER_CACHE_REQUEST_LOCAL_SLOT_HIT
	) {
		return PHP_USER_CACHE_OPTIMISTIC_FOUND;
	}

	ref_registered = php_user_cache_has_request_shared_graph_ref(snapshot->value_offset);
	if (!ref_registered) {
		php_user_cache_shared_graph_ref_reserve();

		if (!php_user_cache_optimistic_reader_begin(header, &reader_slot)) {
			return PHP_USER_CACHE_OPTIMISTIC_FALLBACK;
		}

		if (php_user_cache_seq_reload(&header->write_seq) != seq) {
			php_user_cache_optimistic_reader_end(header, reader_slot);

			return PHP_USER_CACHE_OPTIMISTIC_FALLBACK;
		}

		if (!php_user_cache_shared_graph_acquire_ref(snapshot->value_offset)) {
			php_user_cache_optimistic_reader_end(header, reader_slot);

			return PHP_USER_CACHE_OPTIMISTIC_FALLBACK;
		}

		php_user_cache_register_shared_graph_ref(snapshot->value_offset);
		php_user_cache_optimistic_reader_end(header, reader_slot);
	}

	ZVAL_UNDEF(return_value);

	if (!php_user_cache_shared_graph_decode(
			user_cache_ptr_in_header(header, snapshot->value_offset),
			snapshot->value_len,
			return_value
		)
	) {
		if (Z_TYPE_P(return_value) != IS_UNDEF) {
			zval_ptr_dtor(return_value);
			ZVAL_UNDEF(return_value);
		}

		return PHP_USER_CACHE_OPTIMISTIC_FALLBACK;
	}

	flags = PHP_USER_CACHE_FETCH_FINISH_DEFER_REQUEST_LOCAL_SLOT;
	if (use_proto) {
		flags |= PHP_USER_CACHE_FETCH_FINISH_USE_REQUEST_LOCAL_SLOT;
	}
	if (no_aliases) {
		flags |= PHP_USER_CACHE_FETCH_FINISH_NO_ALIASES;
	}

	php_user_cache_fetch_finish(key, snapshot->generation, return_value, flags);

	return PHP_USER_CACHE_OPTIMISTIC_FOUND;
}
#endif

/* Read-only workloads observe expired entries but never take the write paths
 * that drive the bounded scan; run one scan at request end once enough
 * expired reads accumulated so read-heavy pools reclaim without a store. */
void php_user_cache_expunge_expired_at_request_end(void)
{
	if (EXPECTED(UC_G(expired_read_observations) < PHP_USER_CACHE_EXPIRED_READ_EXPUNGE_THRESHOLD)) {
		return;
	}

	if (!php_user_cache_wlock()) {
		return;
	}

	UC_G(expired_read_observations) = 0;

	(void) user_cache_expunge_expired_bounded_locked();

	php_user_cache_unlock();
}

bool php_user_cache_clear_locked(void)
{
	php_user_cache_header *header = php_user_cache_header_ptr();
	php_user_cache_entry *entries;
	uint64_t epoch;
	uint32_t i;

	if (!header || !php_user_cache_header_init_locked()) {
		return false;
	}

	epoch = header->mutation_epoch;
	entries = php_user_cache_entries_ptr(header);
	for (i = 0; i < header->capacity; i++) {
		if (entries[i].state == PHP_USER_CACHE_ENTRY_USED) {
			user_cache_release_entry_storage_locked(&entries[i]);
		}
	}

	memset(entries, 0, sizeof(php_user_cache_entry) * header->capacity);

	php_user_cache_shared_graph_reclaim_orphaned_locked();
	(void) php_user_cache_shared_graph_strip_dead_pins_locked(true);

	header->count = 0;
	header->tombstone_count = 0;
	header->mutation_epoch = epoch + 1;

	if (header->mutation_epoch == 0) {
		header->mutation_epoch = 1;
	}

	return true;
}

void php_user_cache_lookup_cache_clear(void)
{
	uint32_t i;

	for (i = 0; i < PHP_USER_CACHE_LOOKUP_BUCKETS; i++) {
		user_cache_lookup_entry_release_key(&UC_G(lookup_entry_storage)[i]);
	}

	memset(
		UC_G(lookup_entry_storage),
		0,
		sizeof(php_user_cache_lookup_entry) * PHP_USER_CACHE_LOOKUP_BUCKETS
	);
}

bool php_user_cache_prepare_value(
		zend_string *key,
		zval *value,
		const php_user_cache_prepare_options *options,
		php_user_cache_prepared_value *prepared)
{
	HashTable verbatim_verdicts;
	size_t verbatim_graph_len = 0;
	bool verbatim_eligible = false, has_verbatim_verdicts = false, result;

	if (prepared == NULL || options == NULL) {
		return false;
	}

	user_cache_init_prepared_value(prepared);
	ZVAL_DEREF(value);
	prepared->hash = zend_string_hash_val(key);

	if (Z_TYPE_P(value) == IS_RESOURCE) {
		user_cache_handle_store_failure(
			PHP_USER_CACHE_MSG_RESOURCE_UNSTORABLE,
			options->throw_on_failure
		);

		return false;
	}

	if (Z_TYPE_P(value) == IS_OBJECT && Z_OBJCE_P(value) == zend_ce_closure) {
		user_cache_handle_store_failure(
			PHP_USER_CACHE_MSG_CLOSURE_UNSTORABLE,
			options->throw_on_failure
		);

		return false;
	}

	if (Z_TYPE_P(value) == IS_ARRAY && EXPECTED(!EG(exception))) {
		zend_hash_init(&verbatim_verdicts, 8, NULL, NULL, 0);
		has_verbatim_verdicts = true;
		/* Prove eligibility and compute the size in one walk. */
		switch (php_user_cache_shared_graph_calc_verbatim_root(
			value,
			&verbatim_verdicts,
			&verbatim_graph_len
		)) {
			case PHP_USER_CACHE_VERBATIM_ROOT_SIZED:
			case PHP_USER_CACHE_VERBATIM_ROOT_ELIGIBLE_UNSIZED:
				verbatim_eligible = true;
				break;
			case PHP_USER_CACHE_VERBATIM_ROOT_INELIGIBLE:
				break;
			case PHP_USER_CACHE_VERBATIM_ROOT_UNDECIDED:
				verbatim_eligible = php_user_cache_shared_graph_can_copy_verbatim_root(
					value,
					&verbatim_verdicts
				);
				break;
		}
	}

	/* A verbatim root contains no values rejected by validation. */
	if (!verbatim_eligible &&
		!user_cache_validate_storable_value(value, options->throw_on_failure)
	) {
		result = false;

		goto done;
	}

	if (user_cache_prepare_direct_value(value, prepared)) {
		result = true;

		goto done;
	}

	if (!has_verbatim_verdicts) {
		zend_hash_init(&verbatim_verdicts, 8, NULL, NULL, 0);
		has_verbatim_verdicts = true;
	}

	result = user_cache_prepare_shared_graph_value(value, options, &verbatim_verdicts, verbatim_graph_len, prepared);

done:
	if (has_verbatim_verdicts) {
		zend_hash_destroy(&verbatim_verdicts);
	}

	return result;
}

void php_user_cache_destroy_prepared_value(php_user_cache_prepared_value *prepared)
{
	if (prepared == NULL) {
		return;
	}

	if (prepared->owned_buffer != NULL) {
		efree(prepared->owned_buffer);
	}

	if (prepared->fixup_offsets != NULL) {
		efree(prepared->fixup_offsets);
	}

	if (prepared->owned_string != NULL) {
		zend_string_release(prepared->owned_string);
	}

	if (prepared->state_memo != NULL) {
		zend_hash_destroy(prepared->state_memo);
		efree(prepared->state_memo);
	}

	user_cache_init_prepared_value(prepared);
}

void php_user_cache_object_table_dtor(zval *zv)
{
	zend_object *obj = Z_PTR_P(zv);

	OBJ_RELEASE(obj);
}

void php_user_cache_reference_table_dtor(zval *zv)
{
	zval ref_zv;

	ZVAL_REF(&ref_zv, (zend_reference *) Z_PTR_P(zv));
	zval_ptr_dtor(&ref_zv);
}

void php_user_cache_store_request_local_slot(zend_string *key, uint64_t gen, zval *value, bool no_aliases)
{
	php_user_cache_request_local_slot *slot;
	HashTable verdicts;
	zval slot_zv;
	bool needs_deep_clone = false, has_verdicts = false;

	ZVAL_DEREF(value);

	slot = user_cache_alloc_request_local_slot(gen, no_aliases, true);

	/* Collect once to avoid rescanning nested arrays during cloning. */
	if (Z_TYPE_P(value) == IS_ARRAY || Z_TYPE_P(value) == IS_OBJECT || Z_ISREF_P(value)) {
		zend_hash_init(&verdicts, 8, NULL, NULL, 0);
		has_verdicts = true;
		needs_deep_clone = user_cache_collect_request_local_clone_verdicts(value, &verdicts);
	}

	if (!user_cache_clone_request_local_slot_value_known(
			&slot->value,
			value,
			needs_deep_clone,
			has_verdicts ? &verdicts : NULL,
			no_aliases
		)
	) {
		if (has_verdicts) {
			zend_hash_destroy(&verdicts);
		}

		ZVAL_PTR(&slot_zv, slot);

		user_cache_request_local_slot_dtor(&slot_zv);

		return;
	}

	slot->needs_deep_clone = needs_deep_clone;

	if (has_verdicts) {
		if (needs_deep_clone) {
			/* Recollect verdicts for the cloned identities. */
			zend_hash_destroy(&verdicts);
			zend_hash_init(&slot->clone_verdicts, 8, NULL, NULL, 0);
			user_cache_collect_request_local_clone_verdicts(&slot->value, &slot->clone_verdicts);
		} else {
			/* Source verdicts remain valid without a deep clone. */
			slot->clone_verdicts = verdicts;
		}

		slot->has_clone_verdicts = true;
	}

	zend_hash_update_ptr(user_cache_request_local_slots(), key, slot);
}

bool php_user_cache_store_prepared_locked(
		zend_string *key,
		zval *value,
		const php_user_cache_prepared_value *prepared,
		zend_long ttl,
		const php_user_cache_store_options *options,
		php_user_cache_store_result *result)
{
	bool stored = false;

	user_cache_maybe_expunge_expired_locked();

	PHP_USER_CACHE_TRY_UNLOCK_ON_BAILOUT(
		stored = user_cache_store_prepared_locked_impl(
			key,
			value,
			prepared,
			ttl,
			options,
			result
		);
	);

	return stored;
}

void php_user_cache_fetch_finish(
		zend_string *key,
		uint64_t gen,
		zval *return_value,
		uint32_t flags)
{
	php_user_cache_request_local_slot *slot;
	bool should_store;

	if (flags & PHP_USER_CACHE_FETCH_FINISH_USE_REQUEST_LOCAL_SLOT) {
		if (!(flags & PHP_USER_CACHE_FETCH_FINISH_DEFER_REQUEST_LOCAL_SLOT)) {
			should_store = true;
		} else {
			/* Leave an empty marker only if no value was promoted. */
			slot = user_cache_find_request_local_slot(key, gen, true);

			if (slot == NULL) {
				user_cache_mark_request_local_slot(key, gen);
				should_store = false;
			} else if (!slot->has_value) {
				should_store = true;
			} else {
				/* Preserve a slot promoted by a nested fetch. */
				should_store = false;
			}
		}

		if (should_store) {
			php_user_cache_store_request_local_slot(
				key,
				gen,
				return_value,
				(flags & PHP_USER_CACHE_FETCH_FINISH_NO_ALIASES) != 0
			);
		}
	}
}

bool php_user_cache_fetch_locked(
		zend_string *key,
		bool throw_if_missing,
		bool use_request_local_slot,
		zval *return_value,
		bool *found,
		php_user_cache_fetch_pending_seed *pending_seed)
{
	const char *cache_name = php_user_cache_active_context()->name;
	php_user_cache_header *header;
	php_user_cache_entry *entries, *entry;
	php_user_cache_lookup_entry *lookup_entries;
	php_user_cache_fetch_locate_result locate;
	zend_ulong hash;
	uint64_t epoch;
	uint32_t slot_idx = 0;

	if (found != NULL) {
		*found = false;
	}

	pending_seed->should_seed_request_local_slot = false;

	hash = zend_string_hash_val(key);

	header = php_user_cache_header_ptr();
	if (!header || !php_user_cache_header_is_initialized_locked()) {
		if (throw_if_missing) {
			user_cache_throw_key_not_found_guarded(key);
		}

		return false;
	}

	entries = php_user_cache_entries_ptr(header);
	epoch = header->mutation_epoch;
	lookup_entries = user_cache_lookup_cache_set(hash);

	locate = user_cache_fetch_probe_lookup_cache_locked(
		header, key, hash, entries, lookup_entries, epoch, return_value, &slot_idx
	);

	if (locate == PHP_USER_CACHE_FETCH_LOCATE_UNCACHED) {
		locate = user_cache_fetch_probe_entry_table_locked(
			header, key, hash, entries, lookup_entries, epoch, &slot_idx
		);
	}

	if (locate == PHP_USER_CACHE_FETCH_LOCATE_SCALAR_HIT) {
		if (found != NULL) {
			*found = true;
		}

		return true;
	}

	if (locate == PHP_USER_CACHE_FETCH_LOCATE_MISS) {
		if (throw_if_missing) {
			user_cache_throw_key_not_found_guarded(key);
		}

		return false;
	}

	entry = &entries[slot_idx];

	if (found != NULL) {
		*found = true;
	}

	return user_cache_fetch_emit_value_locked(
		header,
		key,
		cache_name,
		entry,
		entry->generation,
		throw_if_missing,
		use_request_local_slot,
		return_value,
		pending_seed
	);
}

bool php_user_cache_exists_locked(zend_string *key)
{
	zend_ulong hash = zend_string_hash_val(key);
	uint32_t slot_idx;
	bool found;

	if (!user_cache_find_slot_for_read_locked(key, hash, NULL, &slot_idx, &found)) {
		return false;
	}

	return found;
}

void php_user_cache_discard_replaced_entry_locked(
		zend_string *key,
		php_user_cache_replaced_entry *replaced_entry)
{
	php_user_cache_header *header;
	php_user_cache_entry *entries, *current_entry = NULL;
	uint32_t slot_idx;
	bool found;

	if (replaced_entry == NULL || !replaced_entry->found) {
		return;
	}

	if (user_cache_find_slot_ignore_expiry_locked(
			key,
			replaced_entry->entry.hash,
			&header,
			&slot_idx,
			&found
		) &&
		found
	) {
		entries = php_user_cache_entries_ptr(header);
		current_entry = &entries[slot_idx];
	}

	user_cache_release_entry_storage_except_locked(&replaced_entry->entry, current_entry);
	replaced_entry->found = false;
	memset(&replaced_entry->entry, 0, sizeof(replaced_entry->entry));
}

void php_user_cache_rollback_replaced_entry_locked(
		zend_string *key,
		php_user_cache_replaced_entry *replaced_entry)
{
	php_user_cache_header *header;
	php_user_cache_entry *entries, *entry;
	uint32_t slot_idx;
	bool found;

	if (!user_cache_find_slot_ignore_expiry_locked(
			key,
			zend_string_hash_val(key),
			&header,
			&slot_idx,
			&found
		)
	) {
		return;
	}

	entries = php_user_cache_entries_ptr(header);
	entry = &entries[slot_idx];

	if (replaced_entry != NULL && replaced_entry->found) {
		if (found) {
			user_cache_release_entry_storage_except_locked(entry, &replaced_entry->entry);
		} else {
			if (entry->state == PHP_USER_CACHE_ENTRY_TOMBSTONE && header->tombstone_count != 0) {
				header->tombstone_count--;
			}

			header->count++;
		}

		*entry = replaced_entry->entry;
		replaced_entry->found = false;

		memset(&replaced_entry->entry, 0, sizeof(replaced_entry->entry));
	} else if (found) {
		user_cache_delete_entry_locked(header, entry);
	}
}

void php_user_cache_delete_locked(zend_string *key)
{
	php_user_cache_header *header;
	php_user_cache_entry *entries;
	zend_ulong hash = zend_string_hash_val(key);
	uint32_t slot_idx;
	bool found;

	user_cache_maybe_expunge_expired_locked();

	if (!user_cache_find_slot_for_write_locked(key, hash, &header, &slot_idx, &found) || !found) {
		return;
	}

	entries = php_user_cache_entries_ptr(header);
	user_cache_delete_entry_locked(header, &entries[slot_idx]);

	user_cache_maybe_rehash_locked();
}

void php_user_cache_delete_by_prefix_locked(zend_string *prefix)
{
	php_user_cache_header *header;
	php_user_cache_entry *entries, *entry;
	uint32_t i;

	header = php_user_cache_header_ptr();
	if (!header || !php_user_cache_header_is_initialized_locked()) {
		return;
	}

	entries = php_user_cache_entries_ptr(header);
	for (i = 0; i < header->capacity; i++) {
		entry = &entries[i];
		if (entry->state != PHP_USER_CACHE_ENTRY_USED ||
			entry->key_len < ZSTR_LEN(prefix) ||
			memcmp(
				user_cache_ptr_in_header(header, entry->key_offset),
				ZSTR_VAL(prefix),
				ZSTR_LEN(prefix)
			) != 0
		) {
			continue;
		}

		user_cache_delete_entry_locked(header, entry);
	}

	user_cache_maybe_rehash_locked();
}

bool php_user_cache_atomic_update_locked(
		zend_string *key,
		zend_long step,
		zend_long ttl,
		bool decrement,
		bool insert_if_missing,
		php_user_cache_atomic_update_result *result)
{
	php_user_cache_header *header;
	php_user_cache_entry *entries, *entry;
	zend_ulong hash = zend_string_hash_val(key);
	zend_long updated;
	uint32_t slot_idx;
	bool found, is_overflow;

	result->new_value = 0;
	result->is_overflow = false;
	result->is_type_error = false;

	user_cache_maybe_expunge_expired_locked();

	if (!user_cache_find_slot_for_write_locked(key, hash, &header, &slot_idx, &found) || !found) {
		return insert_if_missing
			? user_cache_atomic_insert_missing_locked(key, step, ttl, decrement, result)
			: false
		;
	}

	entries = php_user_cache_entries_ptr(header);
	entry = &entries[slot_idx];
	if (entry->value_type != PHP_USER_CACHE_VALUE_LONG) {
		result->is_type_error = true;

		return false;
	}

	is_overflow = decrement
		? user_cache_long_sub_overflow(entry->long_value, step, &updated)
		: user_cache_long_add_overflow(entry->long_value, step, &updated)
	;
	if (is_overflow) {
		result->is_overflow = true;

		return false;
	}

	entry->long_value = updated;

	php_user_cache_bump_mutation_epoch_locked(header);
	entry->generation = header->mutation_epoch;

	result->new_value = entry->long_value;

	return true;
}

void php_user_cache_release_request_local_slots(void)
{
	user_cache_release_request_local_slot_table(&UC_G(request_local_slot_table));
}

void php_user_cache_release_active_request_local_slots(void)
{
	/* Generation and context checks make over-invalidation safe. */
	php_user_cache_release_request_local_slots();
}

void php_user_cache_release_active_request_local_slots_by_prefix(zend_string *prefix)
{
	zend_string *key, **keys;
	HashTable **slots_ptr = &UC_G(request_local_slot_table);
	uint32_t i, slot_count, count = 0;

	if (*slots_ptr == NULL) {
		return;
	}

	slot_count = zend_hash_num_elements(*slots_ptr);
	if (slot_count == 0) {
		user_cache_release_request_local_slot_table(slots_ptr);

		return;
	}

	keys = safe_emalloc(slot_count, sizeof(zend_string *), 0);
	ZEND_HASH_FOREACH_STR_KEY(*slots_ptr, key) {
		if (key != NULL &&
			ZSTR_LEN(key) >= ZSTR_LEN(prefix) &&
			memcmp(
				ZSTR_VAL(key),
				ZSTR_VAL(prefix),
				ZSTR_LEN(prefix)
			) == 0
		) {
			keys[count++] = zend_string_copy(key);
		}
	} ZEND_HASH_FOREACH_END();

	for (i = 0; i < count; i++) {
		zend_hash_del(*slots_ptr, keys[i]);
		zend_string_release(keys[i]);
	}

	efree(keys);

	if (zend_hash_num_elements(*slots_ptr) == 0) {
		user_cache_release_request_local_slot_table(slots_ptr);
	}
}

#ifdef PHP_USER_CACHE_HAVE_OPTIMISTIC
php_user_cache_optimistic_result php_user_cache_fetch_optimistic(
		zend_string *key,
		zval *return_value)
{
	php_user_cache_header *header;
	php_user_cache_entry snapshot;
	php_user_cache_lookup_entry *lookup_entries;
	php_user_cache_optimistic_result result;
	zend_ulong hash;
	uint64_t seq, epoch;
	uint32_t slot_idx = 0, hint_slot;
	bool have_snapshot;

	if (!user_cache_optimistic_header(&header, &seq)) {
		return PHP_USER_CACHE_OPTIMISTIC_FALLBACK;
	}

	epoch = header->mutation_epoch;
	hash = zend_string_hash_val(key);
	lookup_entries = user_cache_lookup_cache_set(hash);

	if (user_cache_optimistic_scan_lookup_cache(
			header, key, hash, seq, epoch, lookup_entries, return_value, &hint_slot, &result
		)
	) {
		return result;
	}

	have_snapshot = user_cache_optimistic_try_hint_slot(
		header, key, hash, hint_slot, &snapshot, &slot_idx
	);

	result = user_cache_optimistic_locate(
		header, key, hash, seq, epoch, lookup_entries, have_snapshot, &snapshot, &slot_idx
	);
	if (result != PHP_USER_CACHE_OPTIMISTIC_FOUND) {
		return result;
	}

	if (user_cache_scalar_to_zval(snapshot.value_type, snapshot.long_value, snapshot.double_value, return_value)) {
		return PHP_USER_CACHE_OPTIMISTIC_FOUND;
	}

	switch (snapshot.value_type) {
		case PHP_USER_CACHE_VALUE_STRING:
			return user_cache_optimistic_emit_string(header, key, seq, &snapshot, return_value);
		case PHP_USER_CACHE_VALUE_SHARED_GRAPH:
			return user_cache_optimistic_emit_shared_graph(header, key, seq, &snapshot, return_value);
		default:
			return PHP_USER_CACHE_OPTIMISTIC_FALLBACK;
	}
}

php_user_cache_optimistic_result php_user_cache_exists_optimistic(zend_string *key)
{
	const void *ctx;
	php_user_cache_header *header;
	php_user_cache_entry snapshot;
	php_user_cache_lookup_entry *lookup_entries, *lookup_entry;
	zend_ulong hash;
	uint64_t seq, epoch;
	uint32_t way, slot_idx = 0;

	if (!user_cache_optimistic_header(&header, &seq)) {
		return PHP_USER_CACHE_OPTIMISTIC_FALLBACK;
	}

	epoch = header->mutation_epoch;
	hash = zend_string_hash_val(key);
	lookup_entries = user_cache_lookup_cache_set(hash);

	ctx = (const void *) php_user_cache_active_context();
	for (way = 0; way < PHP_USER_CACHE_LOOKUP_WAYS; way++) {
		lookup_entry = &lookup_entries[way];

		if (lookup_entry->state == PHP_USER_CACHE_LOOKUP_EMPTY ||
			lookup_entry->hash != hash ||
			lookup_entry->mutation_epoch != epoch ||
			lookup_entry->context != ctx
		) {
			continue;
		}

		if (lookup_entry->state == PHP_USER_CACHE_LOOKUP_MISS) {
			if (lookup_entry->key == NULL || !zend_string_equals(lookup_entry->key, key)) {
				continue;
			}

			if (php_user_cache_seq_reload(&header->write_seq) != seq) {
				return PHP_USER_CACHE_OPTIMISTIC_FALLBACK;
			}

			return PHP_USER_CACHE_OPTIMISTIC_MISS;
		}

		break;
	}

	return user_cache_optimistic_locate(
		header, key, hash, seq, epoch, lookup_entries, false, &snapshot, &slot_idx
	);
}
#else
php_user_cache_optimistic_result php_user_cache_fetch_optimistic(
		zend_string *key,
		zval *return_value)
{
	(void) key;
	(void) return_value;

	return PHP_USER_CACHE_OPTIMISTIC_FALLBACK;
}

php_user_cache_optimistic_result php_user_cache_exists_optimistic(zend_string *key)
{
	(void) key;

	return PHP_USER_CACHE_OPTIMISTIC_FALLBACK;
}
#endif
