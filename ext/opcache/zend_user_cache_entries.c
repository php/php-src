/*
   +----------------------------------------------------------------------+
   | Zend OPcache                                                         |
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

#include "Zend/zend_closures.h"
#include "Zend/zend_objects.h"
#include "ext/spl/php_spl.h"

#include "zend_user_cache_internal.h"

#define ZEND_OPCACHE_USER_CACHE_REQUEST_LOCAL_ARRAY_NO_CLONE ((void *) 1)
#define ZEND_OPCACHE_USER_CACHE_REQUEST_LOCAL_ARRAY_NEEDS_CLONE ((void *) 2)

typedef struct {
	HashTable arrays;
	HashTable objects;
	HashTable references;
	HashTable *array_clone_flags;
	/* False when identity maps are not needed. */
	bool track_identity;
} zend_opcache_user_cache_request_local_clone_context;

typedef enum {
	ZEND_OPCACHE_USER_CACHE_REQUEST_LOCAL_SLOT_MISS,
	ZEND_OPCACHE_USER_CACHE_REQUEST_LOCAL_SLOT_HIT
} zend_opcache_user_cache_request_local_slot_result;

typedef struct _zend_opcache_user_cache_unstorable_context {
	HashTable *seen_arrays;
	HashTable *seen_objects;
	const char **failure_message;
} zend_opcache_user_cache_unstorable_context;

static bool zend_opcache_user_cache_clone_request_local_value(
	zend_opcache_user_cache_request_local_clone_context *context,
	zval *dst,
	zval *src
);

static bool zend_opcache_user_cache_value_needs_request_local_clone_cached(
	zend_opcache_user_cache_request_local_clone_context *context,
	zval *value
);

static bool zend_opcache_user_cache_find_slot_for_read_locked(
	zend_string *key,
	zend_ulong hash,
	zend_opcache_user_cache_header **header_ptr,
	uint32_t *slot_index,
	bool *found
);

static bool zend_opcache_user_cache_find_unstorable_value(
	zval *value,
	HashTable *seen_arrays,
	HashTable *seen_objects,
	const char **failure_message
);

static void zend_opcache_user_cache_rehash_locked(zend_opcache_user_cache_header *header);

static zend_always_inline void zend_opcache_user_cache_release_value_storage_locked(uint8_t value_type, uint32_t value_offset)
{
	bool graph_quiescent;

	if (value_offset == 0) {
		return;
	}

	if (value_type == ZEND_OPCACHE_USER_CACHE_VALUE_SHARED_GRAPH) {
		/* An optimistic reader may still be about to pin this graph. */
		graph_quiescent = zend_opcache_user_cache_graph_payloads_quiescent_locked();

		if (zend_opcache_user_cache_shared_graph_retire_payload_locked(value_offset)) {
			if (graph_quiescent) {
				zend_opcache_user_cache_free_locked(value_offset);
			} else {
				zend_opcache_user_cache_orphan_graph_payload_locked(value_offset);
			}
		}
	} else if (zend_opcache_user_cache_value_uses_offset(value_type)) {
		zend_opcache_user_cache_free_locked(value_offset);
	}
}

static zend_always_inline void zend_opcache_user_cache_release_entry_storage_locked(zend_opcache_user_cache_entry *entry)
{
	bool uses_combined_value_key;

	uses_combined_value_key = (entry->reserved & ZEND_OPCACHE_USER_CACHE_ENTRY_RESERVED_COMBINED_VALUE_KEY) != 0;
	if (entry->key_offset != 0 && !uses_combined_value_key) {
		zend_opcache_user_cache_free_locked(entry->key_offset);
	}

	zend_opcache_user_cache_release_value_storage_locked(entry->value_type, entry->value_offset);
}

static zend_always_inline void zend_opcache_user_cache_release_entry_storage_except_locked(
		zend_opcache_user_cache_entry *entry,
		const zend_opcache_user_cache_entry *kept_entry)
{
	bool uses_combined_value_key, kept_uses_combined_value_key;

	uses_combined_value_key = (entry->reserved & ZEND_OPCACHE_USER_CACHE_ENTRY_RESERVED_COMBINED_VALUE_KEY) != 0;
	kept_uses_combined_value_key = kept_entry != NULL &&
		(kept_entry->reserved & ZEND_OPCACHE_USER_CACHE_ENTRY_RESERVED_COMBINED_VALUE_KEY) != 0
	;
	if (entry->key_offset != 0 && !uses_combined_value_key &&
		(kept_entry == NULL || kept_uses_combined_value_key || entry->key_offset != kept_entry->key_offset)
	) {
		zend_opcache_user_cache_free_locked(entry->key_offset);
	}

	if (entry->value_offset != 0 &&
		(kept_entry == NULL ||
		 entry->value_offset != kept_entry->value_offset ||
		 entry->value_type != kept_entry->value_type)
	) {
		zend_opcache_user_cache_release_value_storage_locked(entry->value_type, entry->value_offset);
	}
}

static zend_always_inline void zend_opcache_user_cache_delete_entry_locked(zend_opcache_user_cache_entry *entry, zend_opcache_user_cache_header *header)
{
	if (entry->state == ZEND_OPCACHE_USER_CACHE_ENTRY_USED && header->count != 0) {
		header->count--;
	}

	if (entry->state != ZEND_OPCACHE_USER_CACHE_ENTRY_TOMBSTONE) {
		header->tombstone_count++;
	}

	zend_opcache_user_cache_release_entry_storage_locked(entry);
	entry->hash = 0;
	entry->key_offset = 0;
	entry->key_len = 0;
	entry->state = ZEND_OPCACHE_USER_CACHE_ENTRY_TOMBSTONE;
	entry->value_type = ZEND_OPCACHE_USER_CACHE_VALUE_NULL;
	entry->value_offset = 0;
	entry->value_len = 0;
	entry->reserved = 0;
	entry->expires_at = 0;
	entry->generation = 0;
	entry->long_value = 0;
	entry->double_value = 0;
	zend_opcache_user_cache_bump_mutation_epoch_locked(header);
}

static zend_always_inline void zend_opcache_user_cache_release_request_local_slot_context(HashTable **slots_ptr)
{
	if (*slots_ptr == NULL) {
		return;
	}

	zend_hash_destroy(*slots_ptr);

	FREE_HASHTABLE(*slots_ptr);

	*slots_ptr = NULL;
}

static zend_always_inline bool zend_opcache_user_cache_is_expired(const zend_opcache_user_cache_entry *entry, uint64_t now)
{
	return entry->state == ZEND_OPCACHE_USER_CACHE_ENTRY_USED && entry->expires_at != 0 && entry->expires_at <= now;
}

static zend_always_inline bool zend_opcache_user_cache_maybe_expired(const zend_opcache_user_cache_entry *entry, uint64_t *now)
{
	if (entry->state != ZEND_OPCACHE_USER_CACHE_ENTRY_USED || entry->expires_at == 0) {
		return false;
	}

	if (*now == 0) {
		*now = (uint64_t) time(NULL);
	}

	return zend_opcache_user_cache_is_expired(entry, *now);
}

static zend_always_inline bool zend_opcache_user_cache_payload_can_fit_locked(size_t size)
{
	zend_opcache_user_cache_header *header = zend_opcache_user_cache_header_ptr();
	size_t total_size;

	if (!header || size == 0 || size > UINT32_MAX - sizeof(zend_opcache_user_cache_block)) {
		return false;
	}

	total_size = ZEND_ALIGNED_SIZE(sizeof(zend_opcache_user_cache_block) + size);

	return total_size <= UINT32_MAX && total_size <= header->data_size;
}

static zend_always_inline void zend_opcache_user_cache_init_prepared_value(zend_opcache_user_cache_prepared_value *prepared)
{
	memset(prepared, 0, sizeof(*prepared));
	prepared->value_type = ZEND_OPCACHE_USER_CACHE_VALUE_NULL;
}

static zend_always_inline bool zend_opcache_user_cache_prepared_value_can_seed_request_local_slot(
		const zend_opcache_user_cache_prepared_value *prepared)
{
	return prepared->value_type == ZEND_OPCACHE_USER_CACHE_VALUE_STRING &&
		prepared->value_len >= ZEND_OPCACHE_USER_CACHE_REQUEST_LOCAL_STRING_MIN_LEN
	;
}

static zend_always_inline bool zend_opcache_user_cache_long_add_wrapped(
		zend_long lhs,
		zend_long rhs,
		zend_long *result)
{
	*result = (zend_long) ((zend_ulong) lhs + (zend_ulong) rhs);

	return (rhs > 0 && lhs > ZEND_LONG_MAX - rhs) ||
		(rhs < 0 && lhs < ZEND_LONG_MIN - rhs)
	;
}

static zend_always_inline bool zend_opcache_user_cache_long_sub_wrapped(
		zend_long lhs,
		zend_long rhs,
		zend_long *result)
{
	*result = (zend_long) ((zend_ulong) lhs - (zend_ulong) rhs);

	return (rhs > 0 && lhs < ZEND_LONG_MIN + rhs) ||
		(rhs < 0 && lhs > ZEND_LONG_MAX + rhs)
	;
}

static zend_always_inline void zend_opcache_user_cache_maybe_rehash_locked(void)
{
	zend_opcache_user_cache_header *header = zend_opcache_user_cache_header_ptr();

	if (header != NULL &&
		zend_opcache_user_cache_header_is_initialized_locked() &&
		header->tombstone_count > header->capacity / 4
	) {
		zend_opcache_user_cache_rehash_locked(header);
	}
}

#ifdef ZEND_OPCACHE_USER_CACHE_HAVE_OPTIMISTIC
static zend_always_inline bool zend_opcache_user_cache_optimistic_payload_in_bounds(
		const zend_opcache_user_cache_header *header,
		uint32_t offset,
		uint64_t len)
{
	uint64_t limit = (uint64_t) header->data_offset + header->data_size;

	return offset >= header->data_offset + sizeof(zend_opcache_user_cache_block) &&
		(uint64_t) offset + len <= limit
	;
}

static zend_always_inline bool zend_opcache_user_cache_optimistic_header(
		zend_opcache_user_cache_header **header_ptr,
		uint64_t *seq_ptr)
{
	zend_opcache_user_cache_header *header = zend_opcache_user_cache_header_ptr();
	uint64_t seq;

	if (header == NULL) {
		return false;
	}

	seq = zend_opcache_user_cache_seq_load(&header->write_seq);
	if (seq < 2 || (seq & 1) != 0) {
		return false;
	}

	if (header->magic != ZEND_OPCACHE_USER_CACHE_MAGIC ||
		header->version != ZEND_OPCACHE_USER_CACHE_VERSION ||
		header->capacity < ZEND_OPCACHE_USER_CACHE_MIN_CAPACITY ||
		header->capacity > ZEND_OPCACHE_USER_CACHE_MAX_CAPACITY
	) {
		return false;
	}

	if (zend_opcache_user_cache_seq_reload(&header->write_seq) != seq) {
		return false;
	}

	*header_ptr = header;
	*seq_ptr = seq;

	return true;
}

/* Lock-free probe; the caller revalidates the sequence before using snapshot. */
static bool zend_opcache_user_cache_optimistic_probe(
		const zend_opcache_user_cache_header *header,
		const zend_opcache_user_cache_entry *entries,
		zend_string *key,
		zend_ulong hash,
		zend_opcache_user_cache_entry *snapshot,
		uint32_t *slot_index_ptr,
		bool *found)
{
	const zend_opcache_user_cache_entry *entry;
	uint64_t now = 0;
	uint32_t index, step;

	index = (uint32_t) (hash % header->capacity);

	for (step = 0; step < header->capacity; step++) {
		entry = &entries[index];

		switch (entry->state) {
			case ZEND_OPCACHE_USER_CACHE_ENTRY_EMPTY:
				*found = false;

				return true;
			case ZEND_OPCACHE_USER_CACHE_ENTRY_USED:
				if (entry->hash == hash && entry->key_len == ZSTR_LEN(key)) {
					if (!zend_opcache_user_cache_optimistic_payload_in_bounds(header, entry->key_offset, entry->key_len)) {
						return false;
					}

					if (memcmp(zend_opcache_user_cache_ptr(entry->key_offset), ZSTR_VAL(key), ZSTR_LEN(key)) == 0) {
						if (entry->expires_at != 0) {
							if (now == 0) {
								now = (uint64_t) time(NULL);
							}

							if (entry->expires_at <= now) {
								*found = false;

								return true;
							}
						}

						*snapshot = *entry;
						*slot_index_ptr = index;
						*found = true;

						return true;
					}
				}
				break;
			case ZEND_OPCACHE_USER_CACHE_ENTRY_TOMBSTONE:
				break;
			default:
				/* Torn read: resolved by the caller's sequence revalidation. */
				return false;
		}

		++index;
		if (index == header->capacity) {
			index = 0;
		}
	}

	*found = false;

	return true;
}
#endif

static void zend_opcache_user_cache_request_local_slot_dtor(zval *slot_zv)
{
	zend_opcache_user_cache_request_local_slot *slot = Z_PTR_P(slot_zv);

	if (slot->has_array_clone_flags) {
		zend_hash_destroy(&slot->array_clone_flags);
	}

	if (!Z_ISUNDEF(slot->value)) {
		zval_ptr_dtor(&slot->value);
	}

	efree(slot);
}

static bool zend_opcache_user_cache_value_needs_request_local_clone_inner(
		zval *value,
		HashTable *visited_arrays)
{
	zend_ulong array_key;
	zval *element;

	if (Z_ISREF_P(value)) {
		return true;
	}

	switch (Z_TYPE_P(value)) {
		case IS_OBJECT:
			return true;
		case IS_ARRAY:
			array_key = (zend_ulong) (uintptr_t) Z_ARRVAL_P(value);
			if (zend_hash_index_exists(visited_arrays, array_key)) {
				return false;
			}

			zend_hash_index_add_empty_element(visited_arrays, array_key);

			ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(value), element) {
				if (zend_opcache_user_cache_value_needs_request_local_clone_inner(element, visited_arrays)) {
					zend_hash_index_del(visited_arrays, array_key);

					return true;
				}
			} ZEND_HASH_FOREACH_END();

			zend_hash_index_del(visited_arrays, array_key);

			return false;
		default:
			return false;
	}
}

static bool zend_opcache_user_cache_value_needs_request_local_clone(zval *value)
{
	HashTable visited_arrays;
	bool result;

	if (Z_ISREF_P(value)) {
		return true;
	} else if (Z_TYPE_P(value) == IS_OBJECT) {
		return true;
	} else if (Z_TYPE_P(value) != IS_ARRAY) {
		return false;
	}

	zend_hash_init(&visited_arrays, 8, NULL, NULL, 0);

	result = zend_opcache_user_cache_value_needs_request_local_clone_inner(value, &visited_arrays);
	zend_hash_destroy(&visited_arrays);

	return result;
}

static bool zend_opcache_user_cache_collect_request_local_array_clone_flags_inner(
		zval *value,
		HashTable *visited_arrays,
		HashTable *visited_objects,
		HashTable *array_clone_flags,
		bool record_array_result);

static bool zend_opcache_user_cache_collect_request_local_object_clone_flags(
		zend_object *object,
		HashTable *visited_arrays,
		HashTable *visited_objects,
		HashTable *array_clone_flags)
{
	zend_ulong object_key;
	zval *prop, *src, *end;
	void *flag;
	bool members_need_clone = false;

	if (object == NULL) {
		return true;
	}

	object_key = (zend_ulong) (uintptr_t) object;

	flag = zend_hash_index_find_ptr(array_clone_flags, object_key);
	if (flag != NULL) {
		return flag == ZEND_OPCACHE_USER_CACHE_REQUEST_LOCAL_ARRAY_NEEDS_CLONE;
	}

	if (zend_hash_index_exists(visited_objects, object_key)) {
		return true;
	}

	zend_hash_index_add_empty_element(visited_objects, object_key);

	if (object->ce->default_properties_count) {
		src = object->properties_table;
		end = src + object->ce->default_properties_count;
		do {
			if (zend_opcache_user_cache_collect_request_local_array_clone_flags_inner(
					src,
					visited_arrays,
					visited_objects,
					array_clone_flags,
					true
				)
			) {
				members_need_clone = true;
			}
			src++;
		} while (src != end);
	}

	if (object->properties != NULL && zend_hash_num_elements(object->properties) != 0) {
		/* Dynamic properties require the deep path. */
		members_need_clone = true;

		ZEND_HASH_MAP_FOREACH_VAL(object->properties, prop) {
			if (Z_TYPE_P(prop) != IS_INDIRECT) {
				zend_opcache_user_cache_collect_request_local_array_clone_flags_inner(
					prop,
					visited_arrays,
					visited_objects,
					array_clone_flags,
					true
				);
			}
		} ZEND_HASH_FOREACH_END();
	}

	/* NO_CLONE allows a property-table memcpy. */
	zend_hash_index_update_ptr(
		array_clone_flags,
		object_key,
		members_need_clone ?
			ZEND_OPCACHE_USER_CACHE_REQUEST_LOCAL_ARRAY_NEEDS_CLONE :
			ZEND_OPCACHE_USER_CACHE_REQUEST_LOCAL_ARRAY_NO_CLONE
	);

	return members_need_clone;
}

static bool zend_opcache_user_cache_collect_request_local_array_clone_flags_inner(
		zval *value,
		HashTable *visited_arrays,
		HashTable *visited_objects,
		HashTable *array_clone_flags,
		bool record_array_result)
{
	zend_ulong array_key;
	zval *element;
	bool needs_clone = false;
	void *flag;

	if (Z_ISREF_P(value)) {
		zend_opcache_user_cache_collect_request_local_array_clone_flags_inner(
			&Z_REF_P(value)->val,
			visited_arrays,
			visited_objects,
			array_clone_flags,
			true
		);

		return true;
	}

	switch (Z_TYPE_P(value)) {
		case IS_OBJECT:
			zend_opcache_user_cache_collect_request_local_object_clone_flags(
				Z_OBJ_P(value),
				visited_arrays,
				visited_objects,
				array_clone_flags
			);

			return true;
		case IS_ARRAY:
			array_key = (zend_ulong) (uintptr_t) Z_ARRVAL_P(value);
			flag = zend_hash_index_find_ptr(array_clone_flags, array_key);
			if (flag != NULL) {
				return flag == ZEND_OPCACHE_USER_CACHE_REQUEST_LOCAL_ARRAY_NEEDS_CLONE;
			}

			if (zend_hash_index_exists(visited_arrays, array_key)) {
				return false;
			}

			zend_hash_index_add_empty_element(visited_arrays, array_key);

			ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(value), element) {
				if (zend_opcache_user_cache_collect_request_local_array_clone_flags_inner(
						element,
						visited_arrays,
						visited_objects,
						array_clone_flags,
						false
					)
				) {
					needs_clone = true;
				}
			} ZEND_HASH_FOREACH_END();

			zend_hash_index_del(visited_arrays, array_key);

			if (needs_clone || record_array_result) {
				zend_hash_index_update_ptr(
					array_clone_flags,
					array_key,
					needs_clone ?
						ZEND_OPCACHE_USER_CACHE_REQUEST_LOCAL_ARRAY_NEEDS_CLONE :
						ZEND_OPCACHE_USER_CACHE_REQUEST_LOCAL_ARRAY_NO_CLONE
				);
			}

			return needs_clone;
		default:
			return false;
	}
}

static bool zend_opcache_user_cache_collect_request_local_array_clone_flags(
		zval *value,
		HashTable *array_clone_flags)
{
	HashTable visited_arrays, visited_objects;
	bool result;

	zend_hash_init(&visited_arrays, 8, NULL, NULL, 0);
	zend_hash_init(&visited_objects, 8, NULL, NULL, 0);

	result = zend_opcache_user_cache_collect_request_local_array_clone_flags_inner(
		value,
		&visited_arrays,
		&visited_objects,
		array_clone_flags,
		true
	);

	zend_hash_destroy(&visited_objects);
	zend_hash_destroy(&visited_arrays);

	return result;
}

static void zend_opcache_user_cache_request_local_clone_object_dtor(zval *object_zv)
{
	zend_object *object = Z_PTR_P(object_zv);

	OBJ_RELEASE(object);
}

static void zend_opcache_user_cache_request_local_clone_array_dtor(zval *array_zv)
{
	zend_array *array = Z_PTR_P(array_zv);

	zend_array_release(array);
}

static void zend_opcache_user_cache_request_local_clone_reference_dtor(zval *reference_zv)
{
	zval ref_zv;

	ZVAL_REF(&ref_zv, (zend_reference *) Z_PTR_P(reference_zv));
	zval_ptr_dtor(&ref_zv);
}

static void zend_opcache_user_cache_request_local_clone_context_init(
		zend_opcache_user_cache_request_local_clone_context *context,
		HashTable *array_clone_flags,
		bool track_identity)
{
	zend_hash_init(&context->arrays, 8, NULL, zend_opcache_user_cache_request_local_clone_array_dtor, 0);
	zend_hash_init(&context->objects, 8, NULL, zend_opcache_user_cache_request_local_clone_object_dtor, 0);
	zend_hash_init(&context->references, 8, NULL, zend_opcache_user_cache_request_local_clone_reference_dtor, 0);
	context->array_clone_flags = array_clone_flags;
	context->track_identity = track_identity;
}

static void zend_opcache_user_cache_request_local_clone_context_destroy(
		zend_opcache_user_cache_request_local_clone_context *context)
{
	zend_hash_destroy(&context->references);
	zend_hash_destroy(&context->objects);
	zend_hash_destroy(&context->arrays);
}

static bool zend_opcache_user_cache_value_needs_request_local_clone_cached(
		zend_opcache_user_cache_request_local_clone_context *context,
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

	if (context->array_clone_flags != NULL) {
		flag = zend_hash_index_find_ptr(
			context->array_clone_flags,
			(zend_ulong) (uintptr_t) Z_ARRVAL_P(value)
		);
		if (flag != NULL) {
			return flag == ZEND_OPCACHE_USER_CACHE_REQUEST_LOCAL_ARRAY_NEEDS_CLONE;
		}
	}

	return zend_opcache_user_cache_value_needs_request_local_clone(value);
}

static bool zend_opcache_user_cache_clone_request_local_array_ex(
		zend_opcache_user_cache_request_local_clone_context *context,
		zval *dst,
		zval *src,
		bool known_needs_clone)
{
	zend_ulong key = (zend_ulong) (uintptr_t) Z_ARRVAL_P(src);
	zend_array *array;
	zval *element, cloned_element;
	void *flag;

	if (!known_needs_clone) {
		if (context->array_clone_flags != NULL) {
			flag = zend_hash_index_find_ptr(context->array_clone_flags, key);
			if (flag == ZEND_OPCACHE_USER_CACHE_REQUEST_LOCAL_ARRAY_NO_CLONE) {
				ZVAL_COPY(dst, src);

				return true;
			} else if (flag == ZEND_OPCACHE_USER_CACHE_REQUEST_LOCAL_ARRAY_NEEDS_CLONE) {
				known_needs_clone = true;
			}
		}

		if (!known_needs_clone && !zend_opcache_user_cache_value_needs_request_local_clone(src)) {
			ZVAL_COPY(dst, src);

			return true;
		}
	}

	if (context->track_identity) {
		array = zend_hash_index_find_ptr(&context->arrays, key);
		if (array != NULL) {
			GC_ADDREF(array);
			ZVAL_ARR(dst, array);

			return true;
		}
	}

	array = zend_array_dup(Z_ARRVAL_P(src));
	if (context->track_identity) {
		zend_hash_index_update_ptr(&context->arrays, key, array);
	}

	ZEND_HASH_FOREACH_VAL(array, element) {
		if (Z_TYPE_P(element) == IS_INDIRECT ||
			!zend_opcache_user_cache_value_needs_request_local_clone_cached(context, element)
		) {
			continue;
		}

		if (!zend_opcache_user_cache_clone_request_local_value(context, &cloned_element, element)) {
			if (!context->track_identity) {
				zend_array_release(array);
			}

			ZVAL_UNDEF(dst);

			return false;
		}

		zval_ptr_dtor(element);
		ZVAL_COPY_VALUE(element, &cloned_element);
	} ZEND_HASH_FOREACH_END();

	if (context->track_identity) {
		GC_ADDREF(array);
	}

	ZVAL_ARR(dst, array);

	return true;
}

static bool zend_opcache_user_cache_clone_request_local_reference(
		zend_opcache_user_cache_request_local_clone_context *context,
		zval *dst,
		zend_reference *src_ref)
{
	zend_ulong key = (zend_ulong) (uintptr_t) src_ref;
	zend_reference *new_ref;
	zval inner;

	if (context->track_identity) {
		new_ref = zend_hash_index_find_ptr(&context->references, key);
		if (new_ref != NULL) {
			GC_ADDREF(new_ref);
			ZVAL_REF(dst, new_ref);

			return true;
		}
	}

	ZVAL_NEW_EMPTY_REF(dst);
	new_ref = Z_REF_P(dst);
	ZVAL_UNDEF(&new_ref->val);

	if (context->track_identity) {
		zend_hash_index_update_ptr(&context->references, key, new_ref);
	}

	if (!zend_opcache_user_cache_clone_request_local_value(context, &inner, &src_ref->val)) {
		if (!context->track_identity) {
			zval_ptr_dtor(dst);
		}

		ZVAL_UNDEF(dst);

		return false;
	}

	ZVAL_COPY_VALUE(&new_ref->val, &inner);
	if (context->track_identity) {
		GC_ADDREF(new_ref);
		ZVAL_REF(dst, new_ref);
	}

	return true;
}

static bool zend_opcache_user_cache_clone_request_local_object_members(
		zend_opcache_user_cache_request_local_clone_context *context,
		zend_object *old_object,
		zend_object *new_object,
		bool rebind_self_hash_properties)
{
	zend_ulong num_key;
	zend_string *key, *old_object_hash = NULL, *new_object_hash = NULL;
	zend_property_info *property_info;
	zval *src, *dst, *end, *prop, new_prop;
	uint32_t property_index = 0;

	if (old_object->ce->default_properties_count) {
		src = old_object->properties_table;
		dst = new_object->properties_table;
		end = src + old_object->ce->default_properties_count;
		do {
			if (rebind_self_hash_properties &&
				Z_TYPE_P(src) == IS_STRING &&
				zend_opcache_user_cache_is_userland_declared_non_public_property(old_object, property_index)
			) {
				if (old_object_hash == NULL) {
					old_object_hash = php_spl_object_hash(old_object);
				}

				if (zend_string_equals(Z_STR_P(src), old_object_hash)) {
					if (new_object_hash == NULL) {
						new_object_hash = php_spl_object_hash(new_object);
					}

					ZVAL_STR_COPY(&new_prop, new_object_hash);
				} else {
					if (!zend_opcache_user_cache_clone_request_local_value(context, &new_prop, src)) {
						if (old_object_hash != NULL) {
							zend_string_release(old_object_hash);
						}

						if (new_object_hash != NULL) {
							zend_string_release(new_object_hash);
						}

						return false;
					}
				}
			} else if (!zend_opcache_user_cache_clone_request_local_value(context, &new_prop, src)) {
				if (old_object_hash != NULL) {
					zend_string_release(old_object_hash);
				}

				if (new_object_hash != NULL) {
					zend_string_release(new_object_hash);
				}

				return false;
			}

			zval_ptr_dtor(dst);

			ZVAL_COPY_VALUE(dst, &new_prop);
			Z_PROP_FLAG_P(dst) = Z_PROP_FLAG_P(src);

			if (Z_ISREF_P(dst) && new_object->ce->properties_info_table != NULL) {
				property_info = new_object->ce->properties_info_table[property_index];
				if (property_info != NULL &&
					property_info != ZEND_WRONG_PROPERTY_INFO &&
					ZEND_TYPE_IS_SET(property_info->type)
				) {
					ZEND_REF_ADD_TYPE_SOURCE(Z_REF_P(dst), property_info);
				}
			}

			src++;
			dst++;
			property_index++;
		} while (src != end);
	}

	if (old_object->properties != NULL && zend_hash_num_elements(old_object->properties) != 0) {
		if (new_object->properties != NULL) {
			zend_hash_clean(new_object->properties);
			zend_hash_extend(new_object->properties, zend_hash_num_elements(old_object->properties), 0);
		} else {
			new_object->properties = zend_new_array(zend_hash_num_elements(old_object->properties));
			zend_hash_real_init_mixed(new_object->properties);
		}

		HT_FLAGS(new_object->properties) |=
			HT_FLAGS(old_object->properties) & HASH_FLAG_HAS_EMPTY_IND
		;

		ZEND_HASH_MAP_FOREACH_KEY_VAL(old_object->properties, num_key, key, prop) {
			if (Z_TYPE_P(prop) == IS_INDIRECT) {
				ZVAL_INDIRECT(
					&new_prop,
					new_object->properties_table + (Z_INDIRECT_P(prop) - old_object->properties_table)
				);
			} else if (!zend_opcache_user_cache_clone_request_local_value(context, &new_prop, prop)) {
				if (old_object_hash != NULL) {
					zend_string_release(old_object_hash);
				}

				if (new_object_hash != NULL) {
					zend_string_release(new_object_hash);
				}

				return false;
			}

			if (key != NULL) {
				_zend_hash_append(new_object->properties, key, &new_prop);
			} else {
				zend_hash_index_add_new(new_object->properties, num_key, &new_prop);
			}
		} ZEND_HASH_FOREACH_END();
	}

	if (old_object_hash != NULL) {
		zend_string_release(old_object_hash);
	}

	if (new_object_hash != NULL) {
		zend_string_release(new_object_hash);
	}

	return true;
}

static bool zend_opcache_user_cache_clone_request_local_std_object(
		zend_opcache_user_cache_request_local_clone_context *context,
		zend_object *old_object,
		zend_object **new_object_ptr)
{
	zend_object *new_object;
	zval *src, *dst, *end;
	void *member_flag = NULL;

	new_object = zend_objects_new(old_object->ce);

	if (context->track_identity) {
		zend_hash_index_update_ptr(
			&context->objects,
			(zend_ulong) (uintptr_t) old_object,
			new_object
		);
	}

	if (context->array_clone_flags != NULL) {
		member_flag = zend_hash_index_find_ptr(
			context->array_clone_flags,
			(zend_ulong) (uintptr_t) old_object
		);
	}

	if (member_flag == ZEND_OPCACHE_USER_CACHE_REQUEST_LOCAL_ARRAY_NO_CLONE &&
		old_object->properties == NULL
	) {
		/* All declared properties are shareable by plain zval copy. */
		if (old_object->ce->default_properties_count) {
			src = old_object->properties_table;
			dst = new_object->properties_table;
			end = src + old_object->ce->default_properties_count;
			memcpy(dst, src, sizeof(zval) * old_object->ce->default_properties_count);
			do {
				if (Z_REFCOUNTED_P(src)) {
					Z_ADDREF_P(src);
				}

				src++;
			} while (src != end);
		}
	} else {
		if (new_object->ce->default_properties_count) {
			dst = new_object->properties_table;
			end = dst + new_object->ce->default_properties_count;
			do {
				ZVAL_UNDEF(dst);

				dst++;
			} while (dst != end);
		}

		if (!zend_opcache_user_cache_clone_request_local_object_members(context, old_object, new_object, false)) {
			/* Without identity tracking the caller does not own new_object. */
			if (!context->track_identity) {
				OBJ_RELEASE(new_object);
			}

			return false;
		}
	}

	if (context->track_identity) {
		GC_ADDREF(new_object);
	}

	*new_object_ptr = new_object;

	return true;
}

static bool zend_opcache_user_cache_clone_request_local_value_callback(
		void *context,
		zval *dst,
		zval *src)
{
	return zend_opcache_user_cache_clone_request_local_value(
		(zend_opcache_user_cache_request_local_clone_context *) context,
		dst,
		src
	);
}

static bool zend_opcache_user_cache_clone_request_local_safe_direct_object(
		zend_opcache_user_cache_request_local_clone_context *context,
		zend_object *old_object,
		zend_object **new_object_ptr)
{
	zend_opcache_user_cache_safe_direct_state_copy_func_t copy_func;
	zend_ulong key;
	zend_class_entry *ce;
	zend_object *new_object;
	zval new_zv;

	ce = old_object->ce;

	copy_func = zend_opcache_user_cache_safe_direct_copy_func(ce, NULL);
	if (copy_func == NULL) {
		return false;
	}

	ZVAL_UNDEF(&new_zv);

	if (object_init_ex(&new_zv, ce) != SUCCESS) {
		return false;
	}

	new_object = Z_OBJ(new_zv);
	key = (zend_ulong) (uintptr_t) old_object;
	if (context->track_identity) {
		zend_hash_index_update_ptr(&context->objects, key, new_object);
	}

	if (!copy_func(
		context,
		old_object,
		new_object,
		zend_opcache_user_cache_clone_request_local_value_callback
	)) {
		if (!context->track_identity) {
			OBJ_RELEASE(new_object);
		}

		return false;
	}

	if (!zend_opcache_user_cache_clone_request_local_object_members(context, old_object, new_object, true)) {
		if (!context->track_identity) {
			OBJ_RELEASE(new_object);
		}

		return false;
	}

	if (context->track_identity) {
		GC_ADDREF(new_object);
	}

	*new_object_ptr = new_object;

	return true;
}

static bool zend_opcache_user_cache_clone_request_local_object(
		zend_opcache_user_cache_request_local_clone_context *context,
		zend_object *old_object,
		zend_object **new_object_ptr)
{
	zend_ulong key;
	zend_object *new_object;

	if (old_object == NULL || zend_object_is_lazy(old_object)) {
		return false;
	}

	if (context->track_identity) {
		key = (zend_ulong) (uintptr_t) old_object;
		new_object = zend_hash_index_find_ptr(&context->objects, key);

		if (new_object != NULL) {
			GC_ADDREF(new_object);
			*new_object_ptr = new_object;

			return true;
		}
	}

	if (old_object->handlers == zend_get_std_object_handlers()) {
		return zend_opcache_user_cache_clone_request_local_std_object(context, old_object, new_object_ptr);
	}

	return zend_opcache_user_cache_clone_request_local_safe_direct_object(context, old_object, new_object_ptr);
}

static bool zend_opcache_user_cache_clone_request_local_value(
		zend_opcache_user_cache_request_local_clone_context *context,
		zval *dst,
		zval *src)
{
	zend_object *object;

	if (Z_ISREF_P(src)) {
		return zend_opcache_user_cache_clone_request_local_reference(context, dst, Z_REF_P(src));
	}

	switch (Z_TYPE_P(src)) {
		case IS_ARRAY:
			return zend_opcache_user_cache_clone_request_local_array_ex(context, dst, src, false);
		case IS_OBJECT:
			/* The clone helper returns a reference owned by the caller, so the
			 * destination takes it without a further addref. */
			if (!zend_opcache_user_cache_clone_request_local_object(context, Z_OBJ_P(src), &object)) {
				return false;
			}

			ZVAL_OBJ(dst, object);

			return true;
		default:
			ZVAL_COPY(dst, src);

			return true;
	}
}

static bool zend_opcache_user_cache_clone_request_local_slot_value(zval *dst, zval *src, bool no_aliases)
{
	zend_opcache_user_cache_request_local_clone_context context;
	bool result;

	if (Z_TYPE_P(src) == IS_ARRAY) {
		if (!zend_opcache_user_cache_value_needs_request_local_clone(src)) {
			ZVAL_COPY(dst, src);

			return true;
		}

		zend_opcache_user_cache_request_local_clone_context_init(&context, NULL, !no_aliases);
		result = zend_opcache_user_cache_clone_request_local_array_ex(&context, dst, src, true);
		zend_opcache_user_cache_request_local_clone_context_destroy(&context);

		return result;
	}

	if (!zend_opcache_user_cache_value_needs_request_local_clone(src)) {
		ZVAL_COPY(dst, src);

		return true;
	}

	zend_opcache_user_cache_request_local_clone_context_init(&context, NULL, !no_aliases);
	result = zend_opcache_user_cache_clone_request_local_value(&context, dst, src);
	zend_opcache_user_cache_request_local_clone_context_destroy(&context);

	return result;
}

static bool zend_opcache_user_cache_clone_request_local_slot_value_known(
		zval *dst,
		zval *src,
		bool needs_clone,
		HashTable *array_clone_flags,
		bool no_aliases)
{
	zend_opcache_user_cache_request_local_clone_context context;
	bool result;

	if (!needs_clone) {
		ZVAL_COPY(dst, src);

		return true;
	}

	zend_opcache_user_cache_request_local_clone_context_init(&context, array_clone_flags, !no_aliases);

	if (Z_TYPE_P(src) == IS_ARRAY) {
		result = zend_opcache_user_cache_clone_request_local_array_ex(&context, dst, src, true);
	} else {
		result = zend_opcache_user_cache_clone_request_local_value(&context, dst, src);
	}

	zend_opcache_user_cache_request_local_clone_context_destroy(&context);

	return result;
}

static HashTable *zend_opcache_user_cache_request_local_slots(void)
{
	HashTable **slots_ptr = zend_opcache_user_cache_active_request_local_slots_ptr();

	if (*slots_ptr == NULL) {
		ALLOC_HASHTABLE(*slots_ptr);
		zend_hash_init(*slots_ptr, 0, NULL, zend_opcache_user_cache_request_local_slot_dtor, 0);
	}

	return *slots_ptr;
}

static zend_never_inline void zend_opcache_user_cache_reacquire_read_lock_or_fail(const char *cache_name)
{
	if (!zend_opcache_user_cache_rlock()) {
		zend_error_noreturn(E_ERROR, "Unable to reacquire the %s read lock after userland execution", cache_name);
	}
}

static zend_never_inline bool zend_opcache_user_cache_reacquire_write_lock_or_fail(const char *cache_name)
{
	if (!zend_opcache_user_cache_wlock()) {
		zend_error_noreturn(E_ERROR, "Unable to reacquire the %s write lock after userland execution", cache_name);
	}

	if (!zend_opcache_user_cache_header_init_locked()) {
		zend_try {
			zend_throw_exception_ex(zend_opcache_user_cache_exception_ce, 0, "Unable to initialize the %s header", cache_name);
		} zend_catch {
			zend_opcache_user_cache_unlock_if_held();
			zend_bailout();
		} zend_end_try();

		return false;
	}

	return true;
}

static bool zend_opcache_user_cache_materialize_payload_locked(
		zend_string *key,
		uint8_t value_type,
		uint32_t value_offset,
		uint32_t value_len,
		zval *return_value,
		bool throw_if_missing,
		const char *cache_name)
{
	bool ref_registered, lock_safe, defer_retired = false;
	int result;

	switch (value_type) {
		case ZEND_OPCACHE_USER_CACHE_VALUE_SHARED_GRAPH:
			ref_registered = zend_opcache_user_cache_has_request_shared_graph_ref(value_offset);
			if (!ref_registered && !zend_opcache_user_cache_shared_graph_acquire_locked(value_offset)) {
				if (throw_if_missing) {
					zend_try {
						zend_throw_exception_ex(zend_opcache_user_cache_exception_ce, 0, "Stored %s value for key \"%s\" is corrupted", cache_name, ZSTR_VAL(key));
					} zend_catch {
						zend_opcache_user_cache_unlock_if_held();
						zend_bailout();
					} zend_end_try();
				}

				return false;
			}

			lock_safe = zend_opcache_user_cache_shared_graph_decode_is_lock_safe(value_offset);
			if (!lock_safe) {
				zend_opcache_user_cache_unlock();
			}

			ZVAL_UNDEF(return_value);

			if (lock_safe) {
				zend_try {
					result = zend_opcache_user_cache_fetch_shared_graph(
						(const unsigned char *) zend_opcache_user_cache_ptr(value_offset),
						value_len,
						return_value
					);
				} zend_catch {
					zend_opcache_user_cache_unlock_if_held();
					zend_bailout();
				} zend_end_try();
			} else {
				result = zend_opcache_user_cache_fetch_shared_graph(
					(const unsigned char *) zend_opcache_user_cache_ptr(value_offset),
					value_len,
					return_value
				);

				if (result && !ref_registered) {
					zend_opcache_user_cache_register_shared_graph_ref(value_offset);
				}
			}
			if (!lock_safe && !result && Z_TYPE_P(return_value) != IS_UNDEF) {
				zval_ptr_dtor(return_value);
				ZVAL_UNDEF(return_value);
			}

			if (!lock_safe) {
				zend_opcache_user_cache_reacquire_read_lock_or_fail(cache_name);
			}

			if (!result) {
				if (lock_safe && Z_TYPE_P(return_value) != IS_UNDEF) {
					zval_ptr_dtor(return_value);
					ZVAL_UNDEF(return_value);
				}

				if (!ref_registered && zend_opcache_user_cache_shared_graph_release_ref_locked(value_offset)) {
					defer_retired = true;
				}

				if (defer_retired) {
					zend_opcache_user_cache_unlock();
					zend_opcache_user_cache_defer_retired_shared_graph_free(value_offset);
					zend_opcache_user_cache_reacquire_read_lock_or_fail(cache_name);
				}

				if (!EG(exception) && throw_if_missing) {
					zend_try {
						zend_throw_exception_ex(zend_opcache_user_cache_exception_ce, 0, "Stored %s value for key \"%s\" is corrupted", cache_name, ZSTR_VAL(key));
					} zend_catch {
						zend_opcache_user_cache_unlock_if_held();
						zend_bailout();
					} zend_end_try();
				}

				return false;
			}

			if (lock_safe && !ref_registered) {
				/* Registration is request-local bookkeeping; it may allocate, so
				 * a bailout must still release the held read lock. Registering
				 * under the lock avoids the unlock/relock round-trip the
				 * off-lock decode path pays anyway. */
				zend_try {
					zend_opcache_user_cache_register_shared_graph_ref(value_offset);
				} zend_catch {
					zend_opcache_user_cache_unlock_if_held();
					zend_bailout();
				} zend_end_try();
			}

			return true;
		default:
			return false;
	}
}

static zend_opcache_user_cache_request_local_slot_result zend_opcache_user_cache_fetch_request_local_slot(
				zend_string *key,
				uint64_t generation,
				zval *return_value)
{
	zend_opcache_user_cache_request_local_slot *slot;
	HashTable **slots_ptr = zend_opcache_user_cache_active_request_local_slots_ptr();

	if (*slots_ptr == NULL) {
		return ZEND_OPCACHE_USER_CACHE_REQUEST_LOCAL_SLOT_MISS;
	}

	slot = zend_hash_find_ptr(*slots_ptr, key);
	if (slot == NULL) {
		return ZEND_OPCACHE_USER_CACHE_REQUEST_LOCAL_SLOT_MISS;
	}

	if (slot->generation != generation) {
		zend_hash_del(*slots_ptr, key);

		return ZEND_OPCACHE_USER_CACHE_REQUEST_LOCAL_SLOT_MISS;
	}

	if (!zend_opcache_user_cache_clone_request_local_slot_value_known(
			return_value,
			&slot->value,
			slot->needs_clone,
			slot->has_array_clone_flags ? &slot->array_clone_flags : NULL,
			slot->no_aliases
		)
	) {
		zend_hash_del(*slots_ptr, key);

		return ZEND_OPCACHE_USER_CACHE_REQUEST_LOCAL_SLOT_MISS;
	}

	return ZEND_OPCACHE_USER_CACHE_REQUEST_LOCAL_SLOT_HIT;
}

static zend_opcache_user_cache_request_local_slot_result zend_opcache_user_cache_fetch_request_local_slot_locked(
				zend_string *key,
				uint64_t generation,
				zval *return_value)
{
	zend_opcache_user_cache_request_local_slot_result result = ZEND_OPCACHE_USER_CACHE_REQUEST_LOCAL_SLOT_MISS;

	zend_try {
		result = zend_opcache_user_cache_fetch_request_local_slot(key, generation, return_value);
	} zend_catch {
		zend_opcache_user_cache_unlock_if_held();
		zend_bailout();
	} zend_end_try();

	return result;
}

static bool zend_opcache_user_cache_fetch_finish(
				zend_string *key,
				uint64_t generation,
				zval *return_value,
			bool use_request_local_slot,
			bool no_aliases)
{
	if (use_request_local_slot) {
		zend_opcache_user_cache_store_request_local_value_slot(key, generation, return_value, no_aliases);
	}

	return true;
}

static bool zend_opcache_user_cache_fetch_finish_locked(
				zend_string *key,
				uint64_t generation,
				zval *return_value,
				bool use_request_local_slot,
				bool no_aliases)
{
	bool result = false;

	zend_try {
		result = zend_opcache_user_cache_fetch_finish(key, generation, return_value, use_request_local_slot, no_aliases);
	} zend_catch {
		zend_opcache_user_cache_unlock_if_held();
		zend_bailout();
	} zend_end_try();

	return result;
}

static void zend_opcache_user_cache_zval_stringl_locked(zval *return_value, const char *value, size_t value_len)
{
	zend_try {
		ZVAL_STRINGL(return_value, value, value_len);
	} zend_catch {
		zend_opcache_user_cache_unlock_if_held();
		zend_bailout();
	} zend_end_try();
}

static void zend_opcache_user_cache_throw_missing_key_locked(zend_string *key)
{
	zend_try {
		zend_throw_exception_ex(zend_opcache_user_cache_exception_ce, 0, "Cache key \"%s\" was not found", ZSTR_VAL(key));
	} zend_catch {
		zend_opcache_user_cache_unlock_if_held();
		zend_bailout();
	} zend_end_try();
}

static void zend_opcache_user_cache_throw_unknown_type_locked(const char *cache_name, zend_string *key)
{
	zend_try {
		zend_throw_exception_ex(zend_opcache_user_cache_exception_ce, 0, "Stored %s value for key \"%s\" has an unknown type", cache_name, ZSTR_VAL(key));
	} zend_catch {
		zend_opcache_user_cache_unlock_if_held();
		zend_bailout();
	} zend_end_try();
}

static bool zend_opcache_user_cache_find_slot_in_header_locked(
		zend_opcache_user_cache_header *header,
		zend_string *key,
		zend_ulong hash,
		uint32_t *slot_index,
		bool *found,
		bool delete_expired)
{
	zend_opcache_user_cache_entry *entries, *entry;
	uint64_t now = 0;
	uint32_t index, first_tombstone = UINT32_MAX, step;

	if (header == NULL) {
		return false;
	}

	entries = zend_opcache_user_cache_entries(header);
	index = (uint32_t) (hash % header->capacity);

	for (step = 0; step < header->capacity; step++) {
		entry = &entries[index];

		if (entry->state == ZEND_OPCACHE_USER_CACHE_ENTRY_EMPTY) {
			*slot_index = first_tombstone != UINT32_MAX ? first_tombstone : index;
			*found = false;

			return true;
		}

		if (entry->state == ZEND_OPCACHE_USER_CACHE_ENTRY_TOMBSTONE) {
			if (first_tombstone == UINT32_MAX) {
				first_tombstone = index;
			}
		} else if (zend_opcache_user_cache_maybe_expired(entry, &now)) {
			if (delete_expired) {
				zend_opcache_user_cache_delete_entry_locked(entry, header);
			}

			if (first_tombstone == UINT32_MAX) {
				first_tombstone = index;
			}
		} else if (zend_opcache_user_cache_key_equals(entry, key, hash)) {
			*slot_index = index;
			*found = true;

			return true;
		}

		++index;

		if (index == header->capacity) {
			index = 0;
		}
	}

	if (first_tombstone != UINT32_MAX) {
		*slot_index = first_tombstone;
		*found = false;

		return true;
	}

	return false;
}

static bool zend_opcache_user_cache_find_slot_exact_in_header_locked(
		zend_opcache_user_cache_header *header,
		zend_string *key,
		zend_ulong hash,
		uint32_t *slot_index,
		bool *found)
{
	zend_opcache_user_cache_entry *entries, *entry;
	uint32_t index, first_tombstone = UINT32_MAX, step;

	if (header == NULL) {
		return false;
	}

	entries = zend_opcache_user_cache_entries(header);
	index = (uint32_t) (hash % header->capacity);

	for (step = 0; step < header->capacity; step++) {
		entry = &entries[index];

		if (entry->state == ZEND_OPCACHE_USER_CACHE_ENTRY_EMPTY) {
			*slot_index = first_tombstone != UINT32_MAX ? first_tombstone : index;
			*found = false;

			return true;
		}

		if (entry->state == ZEND_OPCACHE_USER_CACHE_ENTRY_TOMBSTONE) {
			if (first_tombstone == UINT32_MAX) {
				first_tombstone = index;
			}
		} else if (zend_opcache_user_cache_key_equals(entry, key, hash)) {
			*slot_index = index;
			*found = true;

			return true;
		}

		++index;

		if (index == header->capacity) {
			index = 0;
		}
	}

	if (first_tombstone != UINT32_MAX) {
		*slot_index = first_tombstone;
		*found = false;

		return true;
	}

	return false;
}

static bool zend_opcache_user_cache_find_slot_locked(
		zend_string *key,
		zend_ulong hash,
		zend_opcache_user_cache_header **header_ptr,
		uint32_t *slot_index,
		bool *found,
		bool delete_expired)
{
	zend_opcache_user_cache_header *header = zend_opcache_user_cache_header_ptr();

	if (!header || !zend_opcache_user_cache_header_init_locked()) {
		return false;
	}

	if (header_ptr != NULL) {
		*header_ptr = header;
	}

	return zend_opcache_user_cache_find_slot_in_header_locked(header, key, hash, slot_index, found, delete_expired);
}

static bool zend_opcache_user_cache_find_slot_exact_locked(
		zend_string *key,
		zend_ulong hash,
		zend_opcache_user_cache_header **header_ptr,
		uint32_t *slot_index,
		bool *found)
{
	zend_opcache_user_cache_header *header = zend_opcache_user_cache_header_ptr();

	if (!header || !zend_opcache_user_cache_header_init_locked()) {
		return false;
	}

	if (header_ptr != NULL) {
		*header_ptr = header;
	}

	return zend_opcache_user_cache_find_slot_exact_in_header_locked(header, key, hash, slot_index, found);
}

static bool zend_opcache_user_cache_find_slot_for_read_locked(
		zend_string *key,
		zend_ulong hash,
		zend_opcache_user_cache_header **header_ptr,
		uint32_t *slot_index,
		bool *found)
{
	return zend_opcache_user_cache_find_slot_locked(key, hash, header_ptr, slot_index, found, false);
}

static bool zend_opcache_user_cache_find_slot_for_write_locked(
		zend_string *key,
		zend_ulong hash,
		zend_opcache_user_cache_header **header_ptr,
		uint32_t *slot_index,
		bool *found)
{
	return zend_opcache_user_cache_find_slot_locked(key, hash, header_ptr, slot_index, found, true);
}

/* Rebuild the entry table after tombstone churn. */
static void zend_opcache_user_cache_rehash_locked(zend_opcache_user_cache_header *header)
{
	zend_opcache_user_cache_entry *entries, *snapshot, *entry, *target;
	uint32_t index, slot, step;

	entries = zend_opcache_user_cache_entries(header);
	snapshot = emalloc((size_t) header->capacity * sizeof(*snapshot));
	memcpy(snapshot, entries, (size_t) header->capacity * sizeof(*snapshot));
	memset(entries, 0, (size_t) header->capacity * sizeof(*entries));

	header->count = 0;
	header->tombstone_count = 0;

	for (index = 0; index < header->capacity; index++) {
		entry = &snapshot[index];

		if (entry->state != ZEND_OPCACHE_USER_CACHE_ENTRY_USED) {
			continue;
		}

		slot = (uint32_t) (entry->hash % header->capacity);
		for (step = 0; step < header->capacity; step++) {
			target = &entries[slot];

			if (target->state == ZEND_OPCACHE_USER_CACHE_ENTRY_EMPTY) {
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

	zend_opcache_user_cache_bump_mutation_epoch_locked(header);
}

static bool zend_opcache_user_cache_expunge_expired_locked(void)
{
	zend_opcache_user_cache_header *header = zend_opcache_user_cache_header_ptr();
	zend_opcache_user_cache_entry *entries;
	uint64_t now;
	uint32_t index;
	bool removed = false;

	if (!header || !zend_opcache_user_cache_header_init_locked()) {
		return false;
	}

	now = (uint64_t) time(NULL);
	entries = zend_opcache_user_cache_entries(header);
	for (index = 0; index < header->capacity; index++) {
		if (zend_opcache_user_cache_is_expired(&entries[index], now)) {
			zend_opcache_user_cache_delete_entry_locked(&entries[index], header);
			removed = true;
		}
	}

	/* Memory pressure is also the moment to collect payloads whose free was
	 * deferred past a reader-drain timeout. */
	zend_opcache_user_cache_reclaim_orphaned_graphs_locked();

	return removed;
}

static void zend_opcache_user_cache_handle_store_failure(const char *failure_message, bool throw_on_failure, bool honor_strict_store_failure)
{
	zend_opcache_user_cache_context *context = zend_opcache_user_cache_active_context();

	if (honor_strict_store_failure && context->strict_store_failure && !throw_on_failure) {
		zend_throw_exception_ex(zend_opcache_user_cache_exception_ce, 0, "%s", failure_message);

		return;
	}

	if (throw_on_failure) {
		zend_throw_exception_ex(zend_opcache_user_cache_exception_ce, 0, "%s", failure_message);
	}
}

static bool zend_opcache_user_cache_safe_direct_value_has_unstorable(
		void *context_ptr,
		const zval *value)
{
	zend_opcache_user_cache_unstorable_context *context = context_ptr;

	return zend_opcache_user_cache_find_unstorable_value(
		(zval *) value,
		context->seen_arrays,
		context->seen_objects,
		context->failure_message
	);
}

static bool zend_opcache_user_cache_find_unstorable_value(
		zval *value,
		HashTable *seen_arrays,
		HashTable *seen_objects,
		const char **failure_message)
{
	zend_ulong key;
	zend_object *object;
	zend_opcache_user_cache_safe_direct_state_has_unstorable_func_t state_has_unstorable;
	zend_opcache_user_cache_unstorable_context unstorable_context;
	zval *element, *property, *end;

	ZVAL_DEREF(value);

	if (Z_TYPE_P(value) == IS_RESOURCE) {
		*failure_message = "resources cannot be stored in the user cache";

		return true;
	}

	if (Z_TYPE_P(value) == IS_OBJECT && Z_OBJCE_P(value) == zend_ce_closure) {
		*failure_message = "Closure objects cannot be stored in the user cache";

		return true;
	}

	if (Z_TYPE_P(value) == IS_ARRAY) {
		key = (zend_ulong) (uintptr_t) Z_ARR_P(value);
		if (zend_hash_index_exists(seen_arrays, key)) {
			return false;
		}

		zend_hash_index_add_empty_element(seen_arrays, key);

		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(value), element) {
			if (zend_opcache_user_cache_find_unstorable_value(element, seen_arrays, seen_objects, failure_message)) {
				return true;
			}
		} ZEND_HASH_FOREACH_END();

		return false;
	}

	if (Z_TYPE_P(value) == IS_OBJECT) {
		object = Z_OBJ_P(value);
		key = (zend_ulong) (uintptr_t) object;

		if (zend_object_is_lazy(object)) {
			*failure_message = "lazy objects cannot be stored in the user cache";

			return true;
		}

		if (zend_hash_index_exists(seen_objects, key)) {
			return false;
		}

		zend_hash_index_add_empty_element(seen_objects, key);

		state_has_unstorable = zend_opcache_user_cache_safe_direct_state_has_unstorable_func(object->ce);
		if (state_has_unstorable != NULL) {
			unstorable_context.seen_arrays = seen_arrays;
			unstorable_context.seen_objects = seen_objects;
			unstorable_context.failure_message = failure_message;

			if (state_has_unstorable(
					&unstorable_context,
					value,
					zend_opcache_user_cache_safe_direct_value_has_unstorable)
			) {
				if (*failure_message == NULL) {
					*failure_message = "objects of this class contain values that cannot be stored in the user cache";
				}

				return true;
			}
		}

		if (object->ce->default_properties_count != 0) {
			property = object->properties_table;
			end = property + object->ce->default_properties_count;
			do {
				if (Z_TYPE_P(property) != IS_UNDEF &&
					zend_opcache_user_cache_find_unstorable_value(property, seen_arrays, seen_objects, failure_message)
				) {
					return true;
				}
				property++;
			} while (property != end);
		}

		if (object->properties != NULL) {
			ZEND_HASH_FOREACH_VAL(object->properties, element) {
				if (Z_TYPE_P(element) == IS_INDIRECT) {
					element = Z_INDIRECT_P(element);
					if (Z_TYPE_P(element) == IS_UNDEF) {
						continue;
					}
				}

				if (zend_opcache_user_cache_find_unstorable_value(element, seen_arrays, seen_objects, failure_message)) {
					return true;
				}
			} ZEND_HASH_FOREACH_END();
		}
	}

	return false;
}

static bool zend_opcache_user_cache_validate_storable_value(zval *value, bool throw_on_failure, bool honor_strict_store_failure)
{
	const char *failure_message = NULL;
	HashTable seen_arrays, seen_objects;
	bool found;

	ZVAL_DEREF(value);

	if (Z_TYPE_P(value) != IS_ARRAY && Z_TYPE_P(value) != IS_OBJECT) {
		return true;
	}

	zend_hash_init(&seen_arrays, 8, NULL, NULL, 0);
	zend_hash_init(&seen_objects, 8, NULL, NULL, 0);

	found = zend_opcache_user_cache_find_unstorable_value(value, &seen_arrays, &seen_objects, &failure_message);

	zend_hash_destroy(&seen_objects);
	zend_hash_destroy(&seen_arrays);

	if (EG(exception)) {
		return false;
	}

	if (!found) {
		return true;
	}

	if (failure_message != NULL) {
		zend_opcache_user_cache_handle_store_failure(failure_message, throw_on_failure, honor_strict_store_failure);
	}

	return false;
}

static unsigned char *zend_opcache_user_cache_reserve_combined_offset_value_locked(
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
		zend_opcache_user_cache_block_payload_capacity(reusable_offset) >= total_size
	) {
		base_offset = reusable_offset;
	} else {
		base_offset = zend_opcache_user_cache_alloc_locked(total_size, NULL);
		if (base_offset == 0) {
			return NULL;
		}
	}

	*value_offset = base_offset;
	*key_offset = base_offset + (uint32_t) payload_size;

	return (unsigned char *) zend_opcache_user_cache_ptr(base_offset);
}

static bool zend_opcache_user_cache_publish_combined_offset_value_locked(
		uint32_t reusable_offset,
		zend_string *key,
		size_t payload_size,
		const void *payload_source,
		uint32_t *value_offset,
		uint32_t *key_offset)
{
	unsigned char *payload;

	if (payload_source == NULL) {
		return false;
	}

	payload = zend_opcache_user_cache_reserve_combined_offset_value_locked(
		reusable_offset,
		key,
		payload_size,
		value_offset,
		key_offset
	);
	if (payload == NULL) {
		return false;
	}

	memcpy(payload, payload_source, payload_size);
	memcpy(payload + payload_size, ZSTR_VAL(key), ZSTR_LEN(key) + 1);

	return true;
}

static bool zend_opcache_user_cache_retry_store_after_pressure_locked(
		bool *retried_expired,
		bool *retried_compact,
		bool *retried_clear,
		size_t required_payload_size,
		bool allow_clear)
{
	if (!*retried_expired) {
		*retried_expired = true;

		if (zend_opcache_user_cache_expunge_expired_locked()) {
			return true;
		}
	}

	if (required_payload_size != 0 && !*retried_compact) {
		if (zend_opcache_user_cache_compact_to_fit_locked(required_payload_size)) {
			*retried_compact = true;

			return true;
		}
	}

	if (allow_clear && zend_opcache_user_cache_active_context()->clear_on_pressure && !*retried_clear) {
		*retried_clear = true;

		return zend_opcache_user_cache_entry_locks_allow_clear_locked() && zend_opcache_user_cache_clear_locked();
	}

	return false;
}

static bool zend_opcache_user_cache_store_prepared_locked_impl(
		zend_string *key,
		zval *value,
		const zend_opcache_user_cache_prepared_value *prepared,
		zend_long ttl,
		bool throw_on_failure,
		bool honor_strict_store_failure,
		uint64_t *generation_ptr,
		bool *seed_request_local_slot_ptr,
		zend_opcache_user_cache_replaced_entry *replaced_entry_ptr,
		bool allow_pressure_retries)
{
	const char *failure_message;
	zend_opcache_user_cache_header *header;
	zend_opcache_user_cache_entry *entries, *entry, old_entry;
	zend_long new_long_value = 0;
	uint64_t expires_at;
	uint32_t slot_index, offset = 0, graph_offset = 0, reusable_offset, old_key_offset = 0, old_value_offset = 0,
			new_key_offset = 0, new_value_offset = 0, new_value_len = 0, combined_reusable_offset = 0;
	uint16_t old_reserved = 0, new_reserved = 0;
	uint8_t old_value_type = ZEND_OPCACHE_USER_CACHE_VALUE_NULL, new_value_type;
	size_t key_payload_size, failed_payload_size;
	bool found, retried_expired = false, retried_compact = false, retried_clear = false, allow_clear, old_combined, use_combined_publish, preserve_old_entry = false;
	unsigned char *combined_payload;
	double new_double_value = 0;

	ZVAL_DEREF(value);

	if (generation_ptr != NULL) {
		*generation_ptr = 0;
	}

	if (seed_request_local_slot_ptr != NULL) {
		*seed_request_local_slot_ptr = false;
	}

	if (replaced_entry_ptr != NULL) {
		replaced_entry_ptr->found = false;
		memset(&replaced_entry_ptr->entry, 0, sizeof(replaced_entry_ptr->entry));
	}

	if (prepared == NULL) {
		return false;
	}

	key_payload_size = ZSTR_LEN(key) + 1;

	zend_opcache_user_cache_maybe_rehash_locked();

retry_store:
	old_key_offset = 0;
	old_value_type = ZEND_OPCACHE_USER_CACHE_VALUE_NULL;
	old_value_offset = 0;
	old_reserved = 0;
	combined_reusable_offset = 0;
	new_key_offset = 0;
	new_value_offset = 0;
	new_value_len = 0;
	new_reserved = 0;
	new_value_type = prepared->value_type;
	new_long_value = 0;
	new_double_value = 0;
	failed_payload_size = 0;
	expires_at = ttl == 0 ? 0 : (uint64_t) time(NULL) + (uint64_t) ttl;

	if (!zend_opcache_user_cache_find_slot_for_write_locked(key, prepared->hash, &header, &slot_index, &found)) {
		if (allow_pressure_retries && zend_opcache_user_cache_retry_store_after_pressure_locked(&retried_expired, &retried_compact, &retried_clear, 0, true)) {
			goto retry_store;
		}

		zend_opcache_user_cache_handle_store_failure("cache hash table is full", throw_on_failure, honor_strict_store_failure);

		return false;
	}

	entries = zend_opcache_user_cache_entries(header);
	entry = &entries[slot_index];

	if (found) {
		old_entry = *entry;
		preserve_old_entry = replaced_entry_ptr != NULL;
		old_key_offset = entry->key_offset;
		old_value_type = entry->value_type;
		old_value_offset = entry->value_offset;
		old_reserved = entry->reserved;
	}

	old_combined = found && (old_reserved & ZEND_OPCACHE_USER_CACHE_ENTRY_RESERVED_COMBINED_VALUE_KEY) != 0;
	reusable_offset = found && !preserve_old_entry && old_value_type != ZEND_OPCACHE_USER_CACHE_VALUE_SHARED_GRAPH && !old_combined ? old_value_offset : 0;
	new_key_offset = found ? old_key_offset : 0;

	use_combined_publish = zend_opcache_user_cache_value_uses_offset(prepared->value_type) && (!found || old_combined);

	/* Prepared shared graphs may overwrite a reusable combined block in place. */
	if (old_combined && old_value_offset != 0 && !preserve_old_entry) {
		if (old_value_type == ZEND_OPCACHE_USER_CACHE_VALUE_SHARED_GRAPH) {
			if (prepared->payload_source != NULL &&
				zend_opcache_user_cache_shared_graph_can_overwrite_payload_locked(old_value_offset) &&
				(prepared->value_type != ZEND_OPCACHE_USER_CACHE_VALUE_SHARED_GRAPH ||
					(prepared->payload_source != NULL && zend_opcache_user_cache_shared_graph_copy_fits_buffer(
							prepared->payload_source,
							prepared->payload_size,
							prepared->payload_used_size,
							(const unsigned char *) zend_opcache_user_cache_ptr(old_value_offset),
							prepared->payload_size
						)
					)
				)
			) {
				combined_reusable_offset = old_value_offset;
			}
		} else if (header->count == 1) {
			combined_reusable_offset = old_value_offset;
		}
	}

	if (!use_combined_publish && (!found || old_combined)) {
		new_key_offset = zend_opcache_user_cache_alloc_locked(key_payload_size, ZSTR_VAL(key));
		if (new_key_offset == 0) {
			failure_message = "not enough shared memory left";
			failed_payload_size = key_payload_size;
			allow_clear = zend_opcache_user_cache_payload_can_fit_locked(key_payload_size);

			goto store_failed;
		}
	}

	switch (prepared->value_type) {
		case ZEND_OPCACHE_USER_CACHE_VALUE_NULL:
			new_value_type = ZEND_OPCACHE_USER_CACHE_VALUE_NULL;

			break;
		case ZEND_OPCACHE_USER_CACHE_VALUE_TRUE:
			new_value_type = ZEND_OPCACHE_USER_CACHE_VALUE_TRUE;

			break;
		case ZEND_OPCACHE_USER_CACHE_VALUE_FALSE:
			new_value_type = ZEND_OPCACHE_USER_CACHE_VALUE_FALSE;

			break;
		case ZEND_OPCACHE_USER_CACHE_VALUE_LONG:
			new_value_type = ZEND_OPCACHE_USER_CACHE_VALUE_LONG;
			new_long_value = prepared->long_value;

			break;
		case ZEND_OPCACHE_USER_CACHE_VALUE_DOUBLE:
			new_value_type = ZEND_OPCACHE_USER_CACHE_VALUE_DOUBLE;
			new_double_value = prepared->double_value;

			break;
		case ZEND_OPCACHE_USER_CACHE_VALUE_STRING:
			if (use_combined_publish) {
				if (!zend_opcache_user_cache_publish_combined_offset_value_locked(
						combined_reusable_offset,
						key,
						prepared->payload_size,
						prepared->payload_source,
						&new_value_offset,
						&new_key_offset)
				) {
					failure_message = "not enough shared memory left";
					failed_payload_size = prepared->payload_size + key_payload_size;
					allow_clear = zend_opcache_user_cache_payload_can_fit_locked(prepared->payload_size + key_payload_size);

					goto store_failed;
				}

				new_reserved = ZEND_OPCACHE_USER_CACHE_ENTRY_RESERVED_COMBINED_VALUE_KEY;
			} else {
				offset = zend_opcache_user_cache_write_payload_locked(
					reusable_offset,
					prepared->payload_size,
					prepared->payload_source
				);
				if (offset == 0) {
					failure_message = "not enough shared memory left";
					failed_payload_size = prepared->payload_size;
					allow_clear = zend_opcache_user_cache_payload_can_fit_locked(prepared->payload_size);

					goto store_failed;
				}

				new_value_offset = offset;
			}

			new_value_type = prepared->value_type;
			new_value_len = prepared->value_len;
			break;
		case ZEND_OPCACHE_USER_CACHE_VALUE_SHARED_GRAPH:
			if (use_combined_publish) {
				/* value_offset must point at the graph payload base. */
				combined_payload = zend_opcache_user_cache_reserve_combined_offset_value_locked(
					prepared->payload_source != NULL ? combined_reusable_offset : 0,
					key,
					prepared->payload_size,
					&new_value_offset,
					&new_key_offset
				);

				graph_offset = new_value_offset;

				if (combined_payload != NULL) {
					if (zend_opcache_user_cache_build_shared_graph_in_place(
							value,
							combined_payload,
							prepared->payload_size,
							NULL,
								prepared->state_memo)
					) {
						memcpy(combined_payload + prepared->payload_size, ZSTR_VAL(key), key_payload_size);
						new_reserved = ZEND_OPCACHE_USER_CACHE_ENTRY_RESERVED_COMBINED_VALUE_KEY;
						new_value_type = ZEND_OPCACHE_USER_CACHE_VALUE_SHARED_GRAPH;
						new_value_offset = graph_offset;
						new_value_len = prepared->value_len;

						break;
					}

					if (graph_offset != combined_reusable_offset) {
						zend_opcache_user_cache_free_locked(graph_offset);
					}

					graph_offset = 0;
					new_value_offset = 0;
					new_key_offset = found ? old_key_offset : 0;

					if (EG(exception)) {
						return false;
					}
				} else if (allow_pressure_retries &&
					zend_opcache_user_cache_payload_can_fit_locked(prepared->payload_size + key_payload_size) &&
					zend_opcache_user_cache_retry_store_after_pressure_locked(&retried_expired, &retried_compact, &retried_clear, prepared->payload_size + key_payload_size, true)
				) {
					goto retry_store;
				}
			} else {
				graph_offset = zend_opcache_user_cache_alloc_locked(prepared->payload_size, NULL);

				if (graph_offset != 0) {
					if (zend_opcache_user_cache_build_shared_graph_in_place(
							value,
							(unsigned char *) zend_opcache_user_cache_ptr(graph_offset),
							prepared->payload_size,
							NULL,
								prepared->state_memo)
					) {
						new_value_type = ZEND_OPCACHE_USER_CACHE_VALUE_SHARED_GRAPH;
						new_value_offset = graph_offset;
						new_value_len = prepared->value_len;

						break;
					}

					zend_opcache_user_cache_free_locked(graph_offset);

					if (EG(exception)) {
						return false;
					}
				} else if (allow_pressure_retries &&
					zend_opcache_user_cache_payload_can_fit_locked(prepared->payload_size) &&
					zend_opcache_user_cache_retry_store_after_pressure_locked(&retried_expired, &retried_compact, &retried_clear, prepared->payload_size, true)
				) {
					goto retry_store;
				}
			}

			/* The shared-graph payload did not fit in shared memory. The build was
			 * already validated off-lock at prepare time, so a store-time failure is
			 * purely a memory shortfall, reached only after the expiry, compaction,
			 * and clear retries above. */
			failure_message = "not enough shared memory left";
			failed_payload_size = prepared->payload_size;
			allow_clear = zend_opcache_user_cache_payload_can_fit_locked(prepared->payload_size);

			goto store_failed;
		default:
			ZEND_UNREACHABLE();
	}

	if (!found && entry->state == ZEND_OPCACHE_USER_CACHE_ENTRY_TOMBSTONE && header->tombstone_count != 0) {
		header->tombstone_count--;
	}

	entry->hash = prepared->hash;
	entry->key_offset = new_key_offset;
	entry->key_len = (uint32_t) ZSTR_LEN(key);
	entry->value_offset = new_value_offset;
	entry->value_len = new_value_len;
	entry->expires_at = expires_at;
	entry->state = ZEND_OPCACHE_USER_CACHE_ENTRY_USED;
	entry->value_type = new_value_type;
	entry->reserved = new_reserved;
	entry->long_value = new_long_value;
	entry->double_value = new_double_value;

	if (preserve_old_entry) {
		replaced_entry_ptr->found = true;
		replaced_entry_ptr->entry = old_entry;
	} else {
		if (found && old_key_offset != 0 && old_key_offset != new_key_offset && !old_combined) {
			zend_opcache_user_cache_free_locked(old_key_offset);
		}

		if (found && old_value_offset != 0 && old_value_offset != new_value_offset) {
			zend_opcache_user_cache_release_value_storage_locked(old_value_type, old_value_offset);
		}
	}

	if (!found) {
		header->count++;
	}

	zend_opcache_user_cache_bump_mutation_epoch_locked(header);

	entry->generation = header->mutation_epoch;
	if (generation_ptr != NULL) {
		*generation_ptr = entry->generation;
	}

	if (seed_request_local_slot_ptr != NULL) {
		*seed_request_local_slot_ptr = zend_opcache_user_cache_prepared_value_can_seed_request_local_slot(prepared);
	}

	return true;

store_failed:
	if (new_value_offset != 0 && new_value_offset != old_value_offset) {
		zend_opcache_user_cache_free_locked(new_value_offset);
	}

	if (new_key_offset != 0 && new_key_offset != old_key_offset &&
		(new_reserved & ZEND_OPCACHE_USER_CACHE_ENTRY_RESERVED_COMBINED_VALUE_KEY) == 0
	) {
		zend_opcache_user_cache_free_locked(new_key_offset);
	}

	if (allow_pressure_retries && zend_opcache_user_cache_retry_store_after_pressure_locked(&retried_expired, &retried_compact, &retried_clear, failed_payload_size, allow_clear)) {
		goto retry_store;
	}

	zend_opcache_user_cache_handle_store_failure(failure_message, throw_on_failure, honor_strict_store_failure);

	return false;
}

bool zend_opcache_user_cache_clear_locked(void)
{
	zend_opcache_user_cache_header *header = zend_opcache_user_cache_header_ptr();
	zend_opcache_user_cache_entry *entries;
	uint64_t mutation_epoch;
	uint32_t index;

	if (!header || !zend_opcache_user_cache_header_init_locked()) {
		return false;
	}

	mutation_epoch = header->mutation_epoch;
	entries = zend_opcache_user_cache_entries(header);
	for (index = 0; index < header->capacity; index++) {
		if (entries[index].state == ZEND_OPCACHE_USER_CACHE_ENTRY_USED) {
			zend_opcache_user_cache_release_entry_storage_locked(&entries[index]);
		}
	}

	memset(entries, 0, sizeof(zend_opcache_user_cache_entry) * header->capacity);

	zend_opcache_user_cache_reclaim_orphaned_graphs_locked();

	header->count = 0;
	header->tombstone_count = 0;
	header->mutation_epoch = mutation_epoch + 1;

	/* Lookup-cache slots use 0 as their empty mutation epoch. */
	if (header->mutation_epoch == 0) {
		header->mutation_epoch = 1;
	}

	return true;
}

bool zend_opcache_user_cache_prepare_value(
		zend_opcache_user_cache_prepared_value *prepared,
		zend_string *key,
		zval *value,
		bool throw_on_failure,
		bool honor_strict_store_failure,
		bool lock_held)
{
	size_t shared_graph_len;

	if (prepared == NULL) {
		return false;
	}

	zend_opcache_user_cache_init_prepared_value(prepared);
	ZVAL_DEREF(value);
	prepared->hash = zend_string_hash_val(key);

	if (Z_TYPE_P(value) == IS_RESOURCE) {
		zend_opcache_user_cache_handle_store_failure(
			"resources cannot be stored in the user cache",
			throw_on_failure,
			honor_strict_store_failure
		);

		return false;
	}

	if (Z_TYPE_P(value) == IS_OBJECT && Z_OBJCE_P(value) == zend_ce_closure) {
		zend_opcache_user_cache_handle_store_failure(
			"Closure objects cannot be stored in the user cache",
			throw_on_failure,
			honor_strict_store_failure
		);

		return false;
	}

	if (!zend_opcache_user_cache_validate_storable_value(value, throw_on_failure, honor_strict_store_failure)) {
		return false;
	}

	switch (Z_TYPE_P(value)) {
		case IS_NULL:
			prepared->value_type = ZEND_OPCACHE_USER_CACHE_VALUE_NULL;

			return true;
		case IS_TRUE:
			prepared->value_type = ZEND_OPCACHE_USER_CACHE_VALUE_TRUE;

			return true;
		case IS_FALSE:
			prepared->value_type = ZEND_OPCACHE_USER_CACHE_VALUE_FALSE;

			return true;
		case IS_LONG:
			prepared->value_type = ZEND_OPCACHE_USER_CACHE_VALUE_LONG;
			prepared->long_value = Z_LVAL_P(value);

			return true;
		case IS_DOUBLE:
			prepared->value_type = ZEND_OPCACHE_USER_CACHE_VALUE_DOUBLE;
			prepared->double_value = Z_DVAL_P(value);

			return true;
		case IS_STRING:
			if (Z_STRLEN_P(value) < ZEND_OPCACHE_USER_CACHE_DIRECT_STRING_MIN_LEN) {
				prepared->value_type = ZEND_OPCACHE_USER_CACHE_VALUE_STRING;
				prepared->value_len = (uint32_t) Z_STRLEN_P(value);
				prepared->payload_size = Z_STRLEN_P(value) + 1;
				prepared->payload_used_size = prepared->payload_size;
				prepared->payload_source = (const unsigned char *) Z_STRVAL_P(value);

				return true;
			}

			ZEND_FALLTHROUGH;
		default:
			shared_graph_len = 0;

			if (!lock_held && prepared->state_memo == NULL) {
				prepared->state_memo = emalloc(sizeof(HashTable));
				zend_hash_init(prepared->state_memo, 8, NULL, ZVAL_PTR_DTOR, 0);
			}

			if (zend_opcache_user_cache_calculate_shared_graph_size(value, &shared_graph_len, prepared->state_memo)) {
				prepared->value_type = ZEND_OPCACHE_USER_CACHE_VALUE_SHARED_GRAPH;
				prepared->value_len = (uint32_t) shared_graph_len;
				prepared->payload_size = shared_graph_len;

				if (lock_held) {
					return true;
				}

				prepared->owned_buffer = emalloc(shared_graph_len);
				if (zend_opcache_user_cache_build_shared_graph_in_place(
						value,
						prepared->owned_buffer,
						shared_graph_len,
						&prepared->payload_used_size,
						prepared->state_memo
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

			zend_opcache_user_cache_handle_store_failure(
				"value cannot be stored in the user cache: it contains a resource, a closure, or an object with opaque internal state (e.g. Fiber, Generator, PDO)",
				throw_on_failure,
				honor_strict_store_failure
			);

			return false;
	}
}

void zend_opcache_user_cache_destroy_prepared_value(zend_opcache_user_cache_prepared_value *prepared)
{
	if (prepared == NULL) {
		return;
	}

	if (prepared->owned_buffer != NULL) {
		efree(prepared->owned_buffer);
	}

	if (prepared->owned_string != NULL) {
		zend_string_release(prepared->owned_string);
	}

	if (prepared->state_memo != NULL) {
		zend_hash_destroy(prepared->state_memo);
		efree(prepared->state_memo);
	}

	zend_opcache_user_cache_init_prepared_value(prepared);
}

void zend_opcache_user_cache_store_request_local_value_slot(zend_string *key, uint64_t generation, zval *value, bool no_aliases)
{
	zend_opcache_user_cache_request_local_slot *slot;
	zval slot_zv;

	ZVAL_DEREF(value);

	slot = emalloc(sizeof(zend_opcache_user_cache_request_local_slot));
	slot->generation = generation;
	slot->needs_clone = false;
	slot->has_array_clone_flags = false;
	slot->no_aliases = no_aliases;

	ZVAL_UNDEF(&slot->value);

	if (!zend_opcache_user_cache_clone_request_local_slot_value(&slot->value, value, no_aliases)) {
		ZVAL_PTR(&slot_zv, slot);

		zend_opcache_user_cache_request_local_slot_dtor(&slot_zv);

		return;
	}

	if (Z_TYPE_P(&slot->value) == IS_ARRAY || Z_TYPE_P(&slot->value) == IS_OBJECT || Z_ISREF_P(&slot->value)) {
		zend_hash_init(&slot->array_clone_flags, 8, NULL, NULL, 0);
		slot->has_array_clone_flags = true;
		slot->needs_clone = zend_opcache_user_cache_collect_request_local_array_clone_flags(
			&slot->value,
			&slot->array_clone_flags
		);
	} else {
		slot->needs_clone = false;
	}

	zend_hash_update_ptr(zend_opcache_user_cache_request_local_slots(), key, slot);
}

bool zend_opcache_user_cache_store_prepared_locked(
		zend_string *key,
		zval *value,
		const zend_opcache_user_cache_prepared_value *prepared,
		zend_long ttl,
		bool throw_on_failure,
		bool honor_strict_store_failure,
		uint64_t *generation_ptr,
		bool *seed_request_local_slot_ptr,
		zend_opcache_user_cache_replaced_entry *replaced_entry_ptr,
		bool allow_pressure_retries)
{
	bool stored = false;

	zend_try {
		stored = zend_opcache_user_cache_store_prepared_locked_impl(
			key,
			value,
			prepared,
			ttl,
			throw_on_failure,
			honor_strict_store_failure,
			generation_ptr,
			seed_request_local_slot_ptr,
			replaced_entry_ptr,
			allow_pressure_retries
		);
	} zend_catch {
		zend_opcache_user_cache_unlock_if_held();
		zend_bailout();
	} zend_end_try();

	return stored;
}

bool zend_opcache_user_cache_store_locked(zend_string *key, zval *value, zend_long ttl, bool throw_on_failure, bool honor_strict_store_failure)
{
	const char *cache_name = zend_opcache_user_cache_active_context()->name;
	zend_opcache_user_cache_prepared_value prepared;
	bool stored;

	/* Preserve the caller's lock contract around preparation. */
	zend_opcache_user_cache_unlock();
	if (!zend_opcache_user_cache_prepare_value(&prepared, key, value, throw_on_failure, honor_strict_store_failure, false)) {
		zend_opcache_user_cache_destroy_prepared_value(&prepared);
		zend_opcache_user_cache_reacquire_write_lock_or_fail(cache_name);

		return false;
	}

	if (!zend_opcache_user_cache_reacquire_write_lock_or_fail(cache_name)) {
		zend_opcache_user_cache_destroy_prepared_value(&prepared);

		return false;
	}

	stored = zend_opcache_user_cache_store_prepared_locked(
		key,
		value,
		&prepared,
		ttl,
		throw_on_failure,
		honor_strict_store_failure,
		NULL,
		NULL,
		NULL,
		true
	);

	zend_opcache_user_cache_destroy_prepared_value(&prepared);

	return stored;
}

bool zend_opcache_user_cache_fetch_locked(zend_string *key, zval *return_value, bool throw_if_missing, bool *found_ptr, bool use_request_local_slot)
{
	const char *cache_name = zend_opcache_user_cache_active_context()->name;
	zend_opcache_user_cache_header *header;
	zend_opcache_user_cache_entry *entries, *entry;
	zend_opcache_user_cache_lookup_entry *lookup_entries, *lookup_entry;
	zend_opcache_user_cache_request_local_slot_result slot_result;
	zend_ulong hash;
	uint64_t mutation_epoch, generation, now;
	uint32_t way, slot_index;
	bool found, use_proto, no_aliases;

	if (found_ptr != NULL) {
		*found_ptr = false;
	}

	hash = zend_string_hash_val(key);

	header = zend_opcache_user_cache_header_ptr();
	if (!header || !zend_opcache_user_cache_header_is_initialized_locked()) {
		if (throw_if_missing) {
			zend_opcache_user_cache_throw_missing_key_locked(key);
		}

		return false;
	}

	entries = zend_opcache_user_cache_entries(header);
	mutation_epoch = header->mutation_epoch;
	lookup_entries = zend_opcache_user_cache_lookup_cache_set(hash);

	for (way = 0; way < ZEND_OPCACHE_USER_CACHE_LOOKUP_WAYS; way++) {
		lookup_entry = &lookup_entries[way];
		if (lookup_entry->state == ZEND_OPCACHE_USER_CACHE_LOOKUP_EMPTY || lookup_entry->hash != hash) {
			continue;
		}

		if (lookup_entry->mutation_epoch != mutation_epoch) {
			/* Stale hits are revalidated below; stale misses are dropped. */
			if (lookup_entry->state != ZEND_OPCACHE_USER_CACHE_LOOKUP_HIT) {
				zend_opcache_user_cache_lookup_cache_reset_entry(lookup_entry);
				continue;
			}
		} else {
			if (lookup_entry->state == ZEND_OPCACHE_USER_CACHE_LOOKUP_MISS) {
				if (throw_if_missing) {
					zend_opcache_user_cache_throw_missing_key_locked(key);
				}

				return false;
			}

			if (lookup_entry->key == key &&
				lookup_entry->value_type != ZEND_OPCACHE_USER_CACHE_LOOKUP_VALUE_NONE
			) {
				switch (lookup_entry->value_type) {
					case ZEND_OPCACHE_USER_CACHE_VALUE_NULL:
						ZVAL_NULL(return_value);
						break;
					case ZEND_OPCACHE_USER_CACHE_VALUE_TRUE:
						ZVAL_TRUE(return_value);
						break;
					case ZEND_OPCACHE_USER_CACHE_VALUE_FALSE:
						ZVAL_FALSE(return_value);
						break;
					case ZEND_OPCACHE_USER_CACHE_VALUE_LONG:
						ZVAL_LONG(return_value, lookup_entry->long_value);
						break;
					default:
						ZVAL_DOUBLE(return_value, lookup_entry->double_value);
						break;
				}

				if (found_ptr != NULL) {
					*found_ptr = true;
				}

				return true;
			}
		}

		if (lookup_entry->slot_index >= header->capacity) {
			zend_opcache_user_cache_lookup_cache_reset_entry(lookup_entry);
			continue;
		}

		entry = &entries[lookup_entry->slot_index];
		if (!zend_opcache_user_cache_key_equals(entry, key, hash)) {
			zend_opcache_user_cache_lookup_cache_reset_entry(lookup_entry);
			continue;
		}

		now = 0;
		if (zend_opcache_user_cache_maybe_expired(entry, &now)) {
			zend_opcache_user_cache_lookup_cache_reset_entry(lookup_entry);
			zend_opcache_user_cache_lookup_cache_store_miss(lookup_entries, hash, mutation_epoch);
			if (throw_if_missing) {
				zend_opcache_user_cache_throw_missing_key_locked(key);
			}

			return false;
		}

		slot_index = lookup_entry->slot_index;

		/* Revived stale hit (or a hit recorded without this key instance):
		 * refresh the epoch and refresh the inline value. */
		if (lookup_entry->mutation_epoch != mutation_epoch || lookup_entry->key != key) {
			zend_opcache_user_cache_lookup_cache_store_hit(lookup_entry, hash, mutation_epoch, slot_index);
			zend_opcache_user_cache_lookup_cache_fill_hit_value(lookup_entry, key, entry);
		}

		found = true;

		goto value_found;
	}

	if (!zend_opcache_user_cache_find_slot_in_header_locked(header, key, hash, &slot_index, &found, false) || !found) {
		zend_opcache_user_cache_lookup_cache_store_miss(lookup_entries, hash, mutation_epoch);
		if (throw_if_missing) {
			zend_opcache_user_cache_throw_missing_key_locked(key);
		}

		return false;
	}

	lookup_entry = zend_opcache_user_cache_lookup_cache_select_slot(lookup_entries, hash, mutation_epoch, true);
	if (lookup_entry != NULL) {
		zend_opcache_user_cache_lookup_cache_store_hit(lookup_entry, hash, mutation_epoch, slot_index);
		zend_opcache_user_cache_lookup_cache_fill_hit_value(lookup_entry, key, &entries[slot_index]);
	}

value_found:
	entry = &entries[slot_index];
	generation = entry->generation;

	if (found_ptr != NULL) {
		*found_ptr = true;
	}

	use_proto = use_request_local_slot;
	no_aliases = true;
	if (entry->value_type == ZEND_OPCACHE_USER_CACHE_VALUE_SHARED_GRAPH) {
		if (use_proto) {
			use_proto = zend_opcache_user_cache_shared_graph_requires_prototype(entry->value_offset);
		}
		if (use_proto) {
			no_aliases = !zend_opcache_user_cache_shared_graph_payload_has_aliases(entry->value_offset);
		}
	}

	if (use_proto) {
		slot_result = zend_opcache_user_cache_fetch_request_local_slot_locked(key, generation, return_value);
		if (slot_result == ZEND_OPCACHE_USER_CACHE_REQUEST_LOCAL_SLOT_HIT) {
			return true;
		}

		if (EG(exception)) {
			return false;
		}
	}

	switch (entry->value_type) {
		case ZEND_OPCACHE_USER_CACHE_VALUE_NULL:
			ZVAL_NULL(return_value);
			return zend_opcache_user_cache_fetch_finish_locked(key, generation, return_value, use_proto, no_aliases);
		case ZEND_OPCACHE_USER_CACHE_VALUE_TRUE:
			ZVAL_TRUE(return_value);
			return zend_opcache_user_cache_fetch_finish_locked(key, generation, return_value, use_proto, no_aliases);
		case ZEND_OPCACHE_USER_CACHE_VALUE_FALSE:
			ZVAL_FALSE(return_value);
			return zend_opcache_user_cache_fetch_finish_locked(key, generation, return_value, use_proto, no_aliases);
		case ZEND_OPCACHE_USER_CACHE_VALUE_LONG:
			ZVAL_LONG(return_value, entry->long_value);
			return zend_opcache_user_cache_fetch_finish_locked(key, generation, return_value, use_proto, no_aliases);
		case ZEND_OPCACHE_USER_CACHE_VALUE_DOUBLE:
			ZVAL_DOUBLE(return_value, entry->double_value);
			return zend_opcache_user_cache_fetch_finish_locked(key, generation, return_value, use_proto, no_aliases);
		case ZEND_OPCACHE_USER_CACHE_VALUE_STRING:
			zend_opcache_user_cache_zval_stringl_locked(return_value, zend_opcache_user_cache_ptr(entry->value_offset), entry->value_len);
			return zend_opcache_user_cache_fetch_finish_locked(key, generation, return_value, use_proto, no_aliases);
		case ZEND_OPCACHE_USER_CACHE_VALUE_SHARED_GRAPH:
			if (!zend_opcache_user_cache_materialize_payload_locked(
					key,
					entry->value_type,
					entry->value_offset,
					entry->value_len,
					return_value,
					throw_if_missing,
					cache_name)
			) {
				return false;
			}

			return zend_opcache_user_cache_fetch_finish_locked(key, generation, return_value, use_proto, no_aliases);
		default:
			if (throw_if_missing) {
				zend_opcache_user_cache_throw_unknown_type_locked(cache_name, key);
			}

			return false;
	}
}

bool zend_opcache_user_cache_exists_locked(zend_string *key)
{
	zend_ulong hash = zend_string_hash_val(key);
	uint32_t slot_index;
	bool found;

	if (!zend_opcache_user_cache_find_slot_for_read_locked(key, hash, NULL, &slot_index, &found)) {
		return false;
	}

	return found;
}

void zend_opcache_user_cache_discard_replaced_entry_locked(
		zend_string *key,
		zend_opcache_user_cache_replaced_entry *replaced_entry)
{
	zend_opcache_user_cache_header *header;
	zend_opcache_user_cache_entry *entries, *current_entry = NULL;
	uint32_t slot_index;
	bool found;

	if (replaced_entry == NULL || !replaced_entry->found) {
		return;
	}

	if (zend_opcache_user_cache_find_slot_exact_locked(
			key,
			replaced_entry->entry.hash,
			&header,
			&slot_index,
			&found
		) &&
		found
	) {
		entries = zend_opcache_user_cache_entries(header);
		current_entry = &entries[slot_index];
	}

	zend_opcache_user_cache_release_entry_storage_except_locked(&replaced_entry->entry, current_entry);
	replaced_entry->found = false;
	memset(&replaced_entry->entry, 0, sizeof(replaced_entry->entry));
}

void zend_opcache_user_cache_rollback_replaced_entry_locked(
		zend_string *key,
		zend_opcache_user_cache_replaced_entry *replaced_entry)
{
	zend_opcache_user_cache_header *header;
	zend_opcache_user_cache_entry *entries, *entry;
	uint32_t slot_index;
	bool found;

	if (!zend_opcache_user_cache_find_slot_exact_locked(
			key,
			zend_string_hash_val(key),
			&header,
			&slot_index,
			&found)
	) {
		return;
	}

	entries = zend_opcache_user_cache_entries(header);
	entry = &entries[slot_index];

	if (replaced_entry != NULL && replaced_entry->found) {
		if (found) {
			zend_opcache_user_cache_release_entry_storage_except_locked(entry, &replaced_entry->entry);
		} else {
			if (entry->state == ZEND_OPCACHE_USER_CACHE_ENTRY_TOMBSTONE && header->tombstone_count != 0) {
				header->tombstone_count--;
			}
			header->count++;
		}

		*entry = replaced_entry->entry;
		replaced_entry->found = false;
		memset(&replaced_entry->entry, 0, sizeof(replaced_entry->entry));
	} else if (found) {
		zend_opcache_user_cache_delete_entry_locked(entry, header);
	}
}

bool zend_opcache_user_cache_delete_locked(zend_string *key)
{
	zend_opcache_user_cache_header *header;
	zend_opcache_user_cache_entry *entries;
	zend_ulong hash = zend_string_hash_val(key);
	uint32_t slot_index;
	bool found;

	if (!zend_opcache_user_cache_find_slot_for_write_locked(key, hash, &header, &slot_index, &found) || !found) {
		return true;
	}

	entries = zend_opcache_user_cache_entries(header);
	zend_opcache_user_cache_delete_entry_locked(&entries[slot_index], header);

	return true;
}

void zend_opcache_user_cache_delete_entries_by_prefix_locked(zend_string *prefix)
{
	zend_opcache_user_cache_header *header;
	zend_opcache_user_cache_entry *entries, *entry;
	uint32_t index;

	header = zend_opcache_user_cache_header_ptr();
	if (!header || !zend_opcache_user_cache_header_is_initialized_locked()) {
		return;
	}

	entries = zend_opcache_user_cache_entries(header);
	for (index = 0; index < header->capacity; index++) {
		entry = &entries[index];
		if (entry->state != ZEND_OPCACHE_USER_CACHE_ENTRY_USED ||
			entry->key_len < ZSTR_LEN(prefix) ||
			memcmp(zend_opcache_user_cache_ptr(entry->key_offset), ZSTR_VAL(prefix), ZSTR_LEN(prefix)) != 0
		) {
			continue;
		}

		zend_opcache_user_cache_delete_entry_locked(entry, header);
	}
}

bool zend_opcache_user_cache_atomic_update_locked(
		zend_string *key,
		zend_long step,
		zend_long ttl,
		bool decrement,
		bool insert_if_missing,
		zend_long *new_value,
		bool *is_overflow)
{
	zend_opcache_user_cache_header *header;
	zend_opcache_user_cache_entry *entries, *entry;
	zend_opcache_user_cache_prepared_value prepared;
	zend_ulong hash = zend_string_hash_val(key);
	zend_long result;
	zval initial_value = {0};
	uint32_t slot_index;
	bool found, result_is_overflow, stored;

	*is_overflow = false;

	if (!zend_opcache_user_cache_find_slot_for_write_locked(key, hash, &header, &slot_index, &found) || !found) {
		if (insert_if_missing) {
			result_is_overflow = decrement
				? zend_opcache_user_cache_long_sub_wrapped(0, step, &result)
				: zend_opcache_user_cache_long_add_wrapped(0, step, &result)
			;
			if (result_is_overflow) {
				*is_overflow = true;

				return false;
			}

			ZVAL_LONG(&initial_value, result);
			if (!zend_opcache_user_cache_prepare_value(&prepared, key, &initial_value, false, false, true)) {
				zend_opcache_user_cache_destroy_prepared_value(&prepared);

				return false;
			}

			stored = zend_opcache_user_cache_store_prepared_locked(
				key,
				&initial_value,
				&prepared,
				ttl,
				false,
				false,
				NULL,
				NULL,
				NULL,
				true
			);
			zend_opcache_user_cache_destroy_prepared_value(&prepared);
			if (stored) {
				*new_value = Z_LVAL(initial_value);

				return true;
			}

			return false;
		}

		return false;
	}

	entries = zend_opcache_user_cache_entries(header);
	entry = &entries[slot_index];
	if (entry->value_type != ZEND_OPCACHE_USER_CACHE_VALUE_LONG) {
		return false;
	}

	result_is_overflow = decrement
		? zend_opcache_user_cache_long_sub_wrapped(entry->long_value, step, &result)
		: zend_opcache_user_cache_long_add_wrapped(entry->long_value, step, &result)
	;
	if (result_is_overflow) {
		*is_overflow = true;

		return false;
	}

	entry->long_value = result;

	zend_opcache_user_cache_bump_mutation_epoch_locked(header);
	entry->generation = header->mutation_epoch;

	*new_value = entry->long_value;

	return true;
}

void zend_opcache_user_cache_release_request_local_slots(void)
{
	zend_opcache_user_cache_release_request_local_slot_context(&zend_opcache_user_cache_request_local_slot_table);
}

void zend_opcache_user_cache_release_active_request_local_slots(void)
{
	zend_opcache_user_cache_release_request_local_slot_context(zend_opcache_user_cache_active_request_local_slots_ptr());
}

void zend_opcache_user_cache_release_active_request_local_slots_by_prefix(zend_string *prefix)
{
	HashTable **slots_ptr = zend_opcache_user_cache_active_request_local_slots_ptr();
	zend_string **keys;
	zend_string *key;
	uint32_t slot_count, count = 0, index;

	if (*slots_ptr == NULL) {
		return;
	}

	slot_count = zend_hash_num_elements(*slots_ptr);
	if (slot_count == 0) {
		zend_hash_destroy(*slots_ptr);
		FREE_HASHTABLE(*slots_ptr);

		*slots_ptr = NULL;

		return;
	}

	keys = safe_emalloc(slot_count, sizeof(zend_string *), 0);
	ZEND_HASH_FOREACH_STR_KEY(*slots_ptr, key) {
		if (key != NULL &&
			ZSTR_LEN(key) >= ZSTR_LEN(prefix) &&
			memcmp(ZSTR_VAL(key), ZSTR_VAL(prefix), ZSTR_LEN(prefix)) == 0
		) {
			keys[count++] = zend_string_copy(key);
		}
	} ZEND_HASH_FOREACH_END();

	for (index = 0; index < count; index++) {
		zend_hash_del(*slots_ptr, keys[index]);
		zend_string_release(keys[index]);
	}
	efree(keys);

	if (zend_hash_num_elements(*slots_ptr) == 0) {
		zend_hash_destroy(*slots_ptr);
		FREE_HASHTABLE(*slots_ptr);

		*slots_ptr = NULL;
	}
}

#ifdef ZEND_OPCACHE_USER_CACHE_HAVE_OPTIMISTIC
/* Optimistic reads are bracketed by write_seq. Shared graphs are pinned inside
 * a reader window before decoding. */

zend_opcache_user_cache_optimistic_result zend_opcache_user_cache_fetch_optimistic(
		zend_string *key,
		zval *return_value)
{
	zend_opcache_user_cache_header *header;
	zend_opcache_user_cache_entry snapshot;
	zend_opcache_user_cache_lookup_entry *lookup_entries, *lookup_entry;
	zend_string *string_value;
	zend_ulong hash;
	uint64_t seq, mutation_epoch;
	uint32_t way, slot_index = 0, hint_slot = UINT32_MAX, reader_slot = 0;
	bool found = false, have_snapshot = false, use_proto, no_aliases, ref_registered;

	if (!zend_opcache_user_cache_optimistic_header(&header, &seq)) {
		return ZEND_OPCACHE_USER_CACHE_OPTIMISTIC_FALLBACK;
	}

	mutation_epoch = header->mutation_epoch;
	hash = zend_string_hash_val(key);
	lookup_entries = zend_opcache_user_cache_lookup_cache_set(hash);

	/* Request-local lookup cache: only epoch-current records are used here;
	 * stale-hint revival is left to the locked path. The epoch itself is part
	 * of the snapshot and is validated by the sequence reloads below. */
	for (way = 0; way < ZEND_OPCACHE_USER_CACHE_LOOKUP_WAYS; way++) {
		lookup_entry = &lookup_entries[way];

		if (lookup_entry->state == ZEND_OPCACHE_USER_CACHE_LOOKUP_EMPTY ||
			lookup_entry->hash != hash ||
			lookup_entry->mutation_epoch != mutation_epoch
		) {
			continue;
		}

		if (lookup_entry->state == ZEND_OPCACHE_USER_CACHE_LOOKUP_MISS) {
			if (zend_opcache_user_cache_seq_reload(&header->write_seq) != seq) {
				return ZEND_OPCACHE_USER_CACHE_OPTIMISTIC_FALLBACK;
			}

			return ZEND_OPCACHE_USER_CACHE_OPTIMISTIC_MISS;
		}

		if (lookup_entry->key == key &&
			lookup_entry->value_type != ZEND_OPCACHE_USER_CACHE_LOOKUP_VALUE_NONE
		) {
			switch (lookup_entry->value_type) {
				case ZEND_OPCACHE_USER_CACHE_VALUE_NULL:
					ZVAL_NULL(return_value);
					break;
				case ZEND_OPCACHE_USER_CACHE_VALUE_TRUE:
					ZVAL_TRUE(return_value);
					break;
				case ZEND_OPCACHE_USER_CACHE_VALUE_FALSE:
					ZVAL_FALSE(return_value);
					break;
				case ZEND_OPCACHE_USER_CACHE_VALUE_LONG:
					ZVAL_LONG(return_value, lookup_entry->long_value);
					break;
				default:
					ZVAL_DOUBLE(return_value, lookup_entry->double_value);
					break;
			}

			if (zend_opcache_user_cache_seq_reload(&header->write_seq) != seq) {
				return ZEND_OPCACHE_USER_CACHE_OPTIMISTIC_FALLBACK;
			}

			return ZEND_OPCACHE_USER_CACHE_OPTIMISTIC_FOUND;
		}

		hint_slot = lookup_entry->slot_index;

		break;
	}

	/* A current-epoch hit names the exact slot; validate it directly before
	 * resorting to the full probe. */
	if (hint_slot != UINT32_MAX && hint_slot < header->capacity) {
		const zend_opcache_user_cache_entry *hint_entry = &zend_opcache_user_cache_entries(header)[hint_slot];

		if (hint_entry->state == ZEND_OPCACHE_USER_CACHE_ENTRY_USED &&
			hint_entry->hash == hash &&
			hint_entry->key_len == ZSTR_LEN(key) &&
			zend_opcache_user_cache_optimistic_payload_in_bounds(header, hint_entry->key_offset, hint_entry->key_len) &&
			memcmp(zend_opcache_user_cache_ptr(hint_entry->key_offset), ZSTR_VAL(key), ZSTR_LEN(key)) == 0 &&
			(hint_entry->expires_at == 0 || hint_entry->expires_at > (uint64_t) time(NULL))
		) {
			snapshot = *hint_entry;
			slot_index = hint_slot;
			found = true;
			have_snapshot = true;
		}
	}

	if (!have_snapshot &&
		!zend_opcache_user_cache_optimistic_probe(
			header,
			zend_opcache_user_cache_entries(header),
			key,
			hash,
			&snapshot,
			&slot_index,
			&found)
	) {
		return ZEND_OPCACHE_USER_CACHE_OPTIMISTIC_FALLBACK;
	}

	if (zend_opcache_user_cache_seq_reload(&header->write_seq) != seq) {
		return ZEND_OPCACHE_USER_CACHE_OPTIMISTIC_FALLBACK;
	}

	/* The probe outcome is now a consistent snapshot. */

	if (!found) {
		zend_opcache_user_cache_lookup_cache_store_miss(lookup_entries, hash, mutation_epoch);

		return ZEND_OPCACHE_USER_CACHE_OPTIMISTIC_MISS;
	}

	lookup_entry = zend_opcache_user_cache_lookup_cache_select_slot(lookup_entries, hash, mutation_epoch, true);
	if (lookup_entry != NULL) {
		zend_opcache_user_cache_lookup_cache_store_hit(lookup_entry, hash, mutation_epoch, slot_index);
		zend_opcache_user_cache_lookup_cache_fill_hit_value(lookup_entry, key, &snapshot);
	}

	switch (snapshot.value_type) {
		case ZEND_OPCACHE_USER_CACHE_VALUE_NULL:
			ZVAL_NULL(return_value);

			return ZEND_OPCACHE_USER_CACHE_OPTIMISTIC_FOUND;
		case ZEND_OPCACHE_USER_CACHE_VALUE_TRUE:
			ZVAL_TRUE(return_value);

			return ZEND_OPCACHE_USER_CACHE_OPTIMISTIC_FOUND;
		case ZEND_OPCACHE_USER_CACHE_VALUE_FALSE:
			ZVAL_FALSE(return_value);

			return ZEND_OPCACHE_USER_CACHE_OPTIMISTIC_FOUND;
		case ZEND_OPCACHE_USER_CACHE_VALUE_LONG:
			ZVAL_LONG(return_value, snapshot.long_value);

			return ZEND_OPCACHE_USER_CACHE_OPTIMISTIC_FOUND;
		case ZEND_OPCACHE_USER_CACHE_VALUE_DOUBLE:
			ZVAL_DOUBLE(return_value, snapshot.double_value);

			return ZEND_OPCACHE_USER_CACHE_OPTIMISTIC_FOUND;
		case ZEND_OPCACHE_USER_CACHE_VALUE_STRING:
			if (zend_opcache_user_cache_fetch_request_local_slot(key, snapshot.generation, return_value) ==
				ZEND_OPCACHE_USER_CACHE_REQUEST_LOCAL_SLOT_HIT
			) {
				return ZEND_OPCACHE_USER_CACHE_OPTIMISTIC_FOUND;
			}

			if (!zend_opcache_user_cache_optimistic_payload_in_bounds(header, snapshot.value_offset, snapshot.value_len)) {
				return ZEND_OPCACHE_USER_CACHE_OPTIMISTIC_FALLBACK;
			}

			string_value = zend_string_init(
				zend_opcache_user_cache_ptr(snapshot.value_offset),
				snapshot.value_len,
				0
			);

			/* The payload may have been moved or freed mid-copy; the copy is
			 * private memory, so a moved sequence just discards it. */
			if (zend_opcache_user_cache_seq_reload(&header->write_seq) != seq) {
				zend_string_release(string_value);

				return ZEND_OPCACHE_USER_CACHE_OPTIMISTIC_FALLBACK;
			}

			ZVAL_STR(return_value, string_value);
			zend_opcache_user_cache_fetch_finish(key, snapshot.generation, return_value, true, true);

			return ZEND_OPCACHE_USER_CACHE_OPTIMISTIC_FOUND;
		case ZEND_OPCACHE_USER_CACHE_VALUE_SHARED_GRAPH:
			break;
		default:
			return ZEND_OPCACHE_USER_CACHE_OPTIMISTIC_FALLBACK;
	}

	/* Shared graph. Read the graph header flags tentatively, then revalidate. */
	if (!zend_opcache_user_cache_optimistic_payload_in_bounds(
			header,
			snapshot.value_offset,
			sizeof(zend_opcache_user_cache_shared_graph_header) + ZEND_MM_ALIGNMENT) ||
		!zend_opcache_user_cache_optimistic_payload_in_bounds(header, snapshot.value_offset, snapshot.value_len)
	) {
		return ZEND_OPCACHE_USER_CACHE_OPTIMISTIC_FALLBACK;
	}

	use_proto = zend_opcache_user_cache_shared_graph_requires_prototype(snapshot.value_offset);
	no_aliases = !zend_opcache_user_cache_shared_graph_payload_has_aliases(snapshot.value_offset);

	if (zend_opcache_user_cache_seq_reload(&header->write_seq) != seq) {
		return ZEND_OPCACHE_USER_CACHE_OPTIMISTIC_FALLBACK;
	}

	if (use_proto &&
		zend_opcache_user_cache_fetch_request_local_slot(key, snapshot.generation, return_value) ==
			ZEND_OPCACHE_USER_CACHE_REQUEST_LOCAL_SLOT_HIT
	) {
		return ZEND_OPCACHE_USER_CACHE_OPTIMISTIC_FOUND;
	}

	ref_registered = zend_opcache_user_cache_has_request_shared_graph_ref(snapshot.value_offset);

	if (!ref_registered) {
		/* Pin registration must not allocate inside the reader window. */
		zend_opcache_user_cache_shared_graph_ref_reserve();

		if (!zend_opcache_user_cache_optimistic_reader_begin(header, &reader_slot)) {
			return ZEND_OPCACHE_USER_CACHE_OPTIMISTIC_FALLBACK;
		}

		if (zend_opcache_user_cache_seq_reload(&header->write_seq) != seq) {
			zend_opcache_user_cache_optimistic_reader_end(header, reader_slot);

			return ZEND_OPCACHE_USER_CACHE_OPTIMISTIC_FALLBACK;
		}

		if (!zend_opcache_user_cache_shared_graph_acquire_locked(snapshot.value_offset)) {
			zend_opcache_user_cache_optimistic_reader_end(header, reader_slot);

			return ZEND_OPCACHE_USER_CACHE_OPTIMISTIC_FALLBACK;
		}

		zend_opcache_user_cache_register_shared_graph_ref(snapshot.value_offset);
		zend_opcache_user_cache_optimistic_reader_end(header, reader_slot);
	}

	ZVAL_UNDEF(return_value);

	if (!zend_opcache_user_cache_fetch_shared_graph(
			(const unsigned char *) zend_opcache_user_cache_ptr(snapshot.value_offset),
			snapshot.value_len,
			return_value)
	) {
		if (Z_TYPE_P(return_value) != IS_UNDEF) {
			zval_ptr_dtor(return_value);
			ZVAL_UNDEF(return_value);
		}

			/* Canonical error handling (corruption exceptions) is the locked
			 * path's job; the caller skips the retry when an exception is already
			 * pending. */
		return ZEND_OPCACHE_USER_CACHE_OPTIMISTIC_FALLBACK;
	}

	zend_opcache_user_cache_fetch_finish(key, snapshot.generation, return_value, use_proto, no_aliases);

	return ZEND_OPCACHE_USER_CACHE_OPTIMISTIC_FOUND;
}

zend_opcache_user_cache_optimistic_result zend_opcache_user_cache_exists_optimistic(zend_string *key)
{
	zend_opcache_user_cache_header *header;
	zend_opcache_user_cache_entry snapshot;
	zend_opcache_user_cache_lookup_entry *lookup_entries, *lookup_entry;
	zend_ulong hash;
	uint64_t seq, mutation_epoch;
	uint32_t way, slot_index = 0;
	bool found = false;

	if (!zend_opcache_user_cache_optimistic_header(&header, &seq)) {
		return ZEND_OPCACHE_USER_CACHE_OPTIMISTIC_FALLBACK;
	}

	mutation_epoch = header->mutation_epoch;
	hash = zend_string_hash_val(key);
	lookup_entries = zend_opcache_user_cache_lookup_cache_set(hash);

	for (way = 0; way < ZEND_OPCACHE_USER_CACHE_LOOKUP_WAYS; way++) {
		lookup_entry = &lookup_entries[way];

		if (lookup_entry->state == ZEND_OPCACHE_USER_CACHE_LOOKUP_EMPTY ||
			lookup_entry->hash != hash ||
			lookup_entry->mutation_epoch != mutation_epoch
		) {
			continue;
		}

		/* Hits may have expired without an epoch bump. */
		if (lookup_entry->state == ZEND_OPCACHE_USER_CACHE_LOOKUP_MISS) {
			if (zend_opcache_user_cache_seq_reload(&header->write_seq) != seq) {
				return ZEND_OPCACHE_USER_CACHE_OPTIMISTIC_FALLBACK;
			}

			return ZEND_OPCACHE_USER_CACHE_OPTIMISTIC_MISS;
		}

		break;
	}

	if (!zend_opcache_user_cache_optimistic_probe(
			header,
			zend_opcache_user_cache_entries(header),
			key,
			hash,
			&snapshot,
			&slot_index,
			&found)
	) {
		return ZEND_OPCACHE_USER_CACHE_OPTIMISTIC_FALLBACK;
	}

	if (zend_opcache_user_cache_seq_reload(&header->write_seq) != seq) {
		return ZEND_OPCACHE_USER_CACHE_OPTIMISTIC_FALLBACK;
	}

	if (!found) {
		zend_opcache_user_cache_lookup_cache_store_miss(lookup_entries, hash, mutation_epoch);

		return ZEND_OPCACHE_USER_CACHE_OPTIMISTIC_MISS;
	}

	lookup_entry = zend_opcache_user_cache_lookup_cache_select_slot(lookup_entries, hash, mutation_epoch, true);
	if (lookup_entry != NULL) {
		zend_opcache_user_cache_lookup_cache_store_hit(lookup_entry, hash, mutation_epoch, slot_index);
		zend_opcache_user_cache_lookup_cache_fill_hit_value(lookup_entry, key, &snapshot);
	}

	return ZEND_OPCACHE_USER_CACHE_OPTIMISTIC_FOUND;
}
#else
zend_opcache_user_cache_optimistic_result zend_opcache_user_cache_fetch_optimistic(
		zend_string *key,
		zval *return_value)
{
	(void) key;
	(void) return_value;

	return ZEND_OPCACHE_USER_CACHE_OPTIMISTIC_FALLBACK;
}

zend_opcache_user_cache_optimistic_result zend_opcache_user_cache_exists_optimistic(zend_string *key)
{
	(void) key;

	return ZEND_OPCACHE_USER_CACHE_OPTIMISTIC_FALLBACK;
}
#endif
