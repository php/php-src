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

#include "zend_static_cache_internal.h"
#include "zend_opcache_serializer.h"

#include "Zend/zend_closures.h"
#include "Zend/zend_objects.h"

typedef struct _zend_opcache_static_cache_request_local_clone_context {
	HashTable arrays;
	HashTable objects;
	HashTable references;
} zend_opcache_static_cache_request_local_clone_context;

/* Value graph cloning is mutually recursive across arrays, references, and objects. */
static bool zend_opcache_static_cache_clone_request_local_value(
	zend_opcache_static_cache_request_local_clone_context *context,
	zval *dst,
	zval *src
);

static zend_always_inline void zend_opcache_static_cache_release_value_storage_locked(uint8_t value_type, uint32_t value_offset)
{
	if (value_offset == 0) {
		return;
	}

	if (value_type == ZEND_OPCACHE_STATIC_CACHE_VALUE_SHARED_GRAPH) {
		if (zend_opcache_static_cache_shared_graph_retire_payload_locked(value_offset)) {
			zend_opcache_static_cache_free_locked(value_offset);
		}
	} else if (zend_opcache_static_cache_value_uses_offset(value_type)) {
		zend_opcache_static_cache_free_locked(value_offset);
	}
}

static zend_always_inline void zend_opcache_static_cache_release_entry_storage_locked(zend_opcache_static_cache_entry *entry)
{
	bool uses_combined_value_key = (entry->reserved & ZEND_OPCACHE_STATIC_CACHE_ENTRY_RESERVED_COMBINED_VALUE_KEY) != 0;

	if (entry->key_offset != 0 && !uses_combined_value_key) {
		zend_opcache_static_cache_free_locked(entry->key_offset);
	}

	zend_opcache_static_cache_release_value_storage_locked(entry->value_type, entry->value_offset);
}

static zend_always_inline void zend_opcache_static_cache_delete_entry_locked(zend_opcache_static_cache_entry *entry, zend_opcache_static_cache_header *header)
{
	if (entry->state == ZEND_OPCACHE_STATIC_CACHE_ENTRY_USED && header->count != 0) {
		header->count--;
	}

	zend_opcache_static_cache_release_entry_storage_locked(entry);
	entry->hash = 0;
	entry->key_offset = 0;
	entry->key_len = 0;
	entry->state = ZEND_OPCACHE_STATIC_CACHE_ENTRY_TOMBSTONE;
	entry->value_type = ZEND_OPCACHE_STATIC_CACHE_VALUE_NULL;
	entry->value_offset = 0;
	entry->value_len = 0;
	entry->reserved = 0;
	entry->expires_at = 0;
	entry->long_value = 0;
	entry->double_value = 0;
	zend_opcache_static_cache_bump_mutation_epoch_locked(header);
}

static zend_always_inline void zend_opcache_static_cache_release_request_local_slot_context(HashTable **slots_ptr)
{
	if (*slots_ptr == NULL) {
		return;
	}

	zend_hash_destroy(*slots_ptr);
	FREE_HASHTABLE(*slots_ptr);
	*slots_ptr = NULL;
}

static zend_always_inline bool zend_opcache_static_cache_is_expired(const zend_opcache_static_cache_entry *entry, uint64_t now)
{
	return entry->state == ZEND_OPCACHE_STATIC_CACHE_ENTRY_USED && entry->expires_at != 0 && entry->expires_at <= now;
}

static zend_always_inline bool zend_opcache_static_cache_maybe_expired(const zend_opcache_static_cache_entry *entry, uint64_t *now)
{
	if (entry->state != ZEND_OPCACHE_STATIC_CACHE_ENTRY_USED || entry->expires_at == 0) {
		return false;
	}

	if (*now == 0) {
		*now = (uint64_t) time(NULL);
	}

	return zend_opcache_static_cache_is_expired(entry, *now);
}

static zend_always_inline bool zend_opcache_static_cache_payload_can_fit_locked(size_t size)
{
	zend_opcache_static_cache_header *header = zend_opcache_static_cache_header_ptr();
	size_t total_size;

	if (!header || size == 0 || size > UINT32_MAX - sizeof(zend_opcache_static_cache_block)) {
		return false;
	}

	total_size = ZEND_ALIGNED_SIZE(sizeof(zend_opcache_static_cache_block) + size);

	return total_size <= UINT32_MAX && total_size <= header->data_size;
}

static zend_always_inline void zend_opcache_static_cache_init_prepared_value(zend_opcache_static_cache_prepared_value *prepared)
{
	memset(prepared, 0, sizeof(*prepared));
	prepared->value_type = ZEND_OPCACHE_STATIC_CACHE_VALUE_NULL;
}

static void zend_opcache_static_cache_request_local_slot_dtor(zval *slot_zv)
{
	zend_opcache_static_cache_request_local_slot *slot = Z_PTR_P(slot_zv);

	if (!Z_ISUNDEF(slot->value)) {
		zval_ptr_dtor(&slot->value);
	}

	efree(slot);
}

static bool zend_opcache_static_cache_value_needs_request_local_clone_inner(
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
				if (zend_opcache_static_cache_value_needs_request_local_clone_inner(element, visited_arrays)) {
					return true;
				}
			} ZEND_HASH_FOREACH_END();

			return false;
		default:
			return false;
	}
}

static bool zend_opcache_static_cache_value_needs_request_local_clone(zval *value)
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
	result = zend_opcache_static_cache_value_needs_request_local_clone_inner(value, &visited_arrays);
	zend_hash_destroy(&visited_arrays);

	return result;
}

static void zend_opcache_static_cache_request_local_clone_object_dtor(zval *object_zv)
{
	zend_object *object = Z_PTR_P(object_zv);

	OBJ_RELEASE(object);
}

static void zend_opcache_static_cache_request_local_clone_array_dtor(zval *array_zv)
{
	zend_array *array = Z_PTR_P(array_zv);

	zend_array_release(array);
}

static void zend_opcache_static_cache_request_local_clone_reference_dtor(zval *reference_zv)
{
	zval ref_zv;

	ZVAL_REF(&ref_zv, (zend_reference *) Z_PTR_P(reference_zv));
	zval_ptr_dtor(&ref_zv);
}

static void zend_opcache_static_cache_request_local_clone_context_init(
		zend_opcache_static_cache_request_local_clone_context *context)
{
	zend_hash_init(&context->arrays, 8, NULL, zend_opcache_static_cache_request_local_clone_array_dtor, 0);
	zend_hash_init(&context->objects, 8, NULL, zend_opcache_static_cache_request_local_clone_object_dtor, 0);
	zend_hash_init(&context->references, 8, NULL, zend_opcache_static_cache_request_local_clone_reference_dtor, 0);
}

static void zend_opcache_static_cache_request_local_clone_context_destroy(
		zend_opcache_static_cache_request_local_clone_context *context)
{
	zend_hash_destroy(&context->references);
	zend_hash_destroy(&context->objects);
	zend_hash_destroy(&context->arrays);
}

static bool zend_opcache_static_cache_clone_request_local_array_ex(
		zend_opcache_static_cache_request_local_clone_context *context,
		zval *dst,
		zval *src,
		bool known_needs_clone)
{
	zend_ulong key = (zend_ulong) (uintptr_t) Z_ARRVAL_P(src);
	zend_array *array = zend_hash_index_find_ptr(&context->arrays, key);
	zval *element, cloned_element;

	if (!known_needs_clone && !zend_opcache_static_cache_value_needs_request_local_clone(src)) {
		ZVAL_COPY(dst, src);

		return true;
	}

	if (array != NULL) {
		GC_ADDREF(array);
		ZVAL_ARR(dst, array);

		return true;
	}

	array = zend_array_dup(Z_ARRVAL_P(src));
	zend_hash_index_update_ptr(&context->arrays, key, array);
	GC_ADDREF(array);
	ZVAL_ARR(dst, array);
	ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(dst), element) {
		if (Z_TYPE_P(element) == IS_INDIRECT ||
			!zend_opcache_static_cache_value_needs_request_local_clone(element)
		) {
			continue;
		}

		if (!zend_opcache_static_cache_clone_request_local_value(context, &cloned_element, element)) {
			zval_ptr_dtor(dst);
			ZVAL_UNDEF(dst);

			return false;
		}

		zval_ptr_dtor(element);
		ZVAL_COPY_VALUE(element, &cloned_element);
	} ZEND_HASH_FOREACH_END();

	return true;
}

static bool zend_opcache_static_cache_clone_request_local_reference(
		zend_opcache_static_cache_request_local_clone_context *context,
		zval *dst,
		zend_reference *src_ref)
{
	zend_ulong key = (zend_ulong) (uintptr_t) src_ref;
	zend_reference *new_ref;
	zval inner;

	new_ref = zend_hash_index_find_ptr(&context->references, key);
	if (new_ref != NULL) {
		GC_ADDREF(new_ref);
		ZVAL_REF(dst, new_ref);

		return true;
	}

	ZVAL_NEW_EMPTY_REF(dst);
	new_ref = Z_REF_P(dst);
	ZVAL_UNDEF(&new_ref->val);
	zend_hash_index_update_ptr(&context->references, key, new_ref);

	if (!zend_opcache_static_cache_clone_request_local_value(context, &inner, &src_ref->val)) {
		ZVAL_UNDEF(dst);

		return false;
	}

	ZVAL_COPY_VALUE(&new_ref->val, &inner);
	GC_ADDREF(new_ref);
	ZVAL_REF(dst, new_ref);

	return true;
}

static bool zend_opcache_static_cache_clone_request_local_object_members(
		zend_opcache_static_cache_request_local_clone_context *context,
		zend_object *old_object,
		zend_object *new_object)
{
	zend_ulong num_key;
	zend_string *key;
	zval *src, *dst, *end, *prop, new_prop;

	if (old_object->ce->default_properties_count) {
		src = old_object->properties_table;
		dst = new_object->properties_table;
		end = src + old_object->ce->default_properties_count;
		do {
			if (!zend_opcache_static_cache_clone_request_local_value(context, &new_prop, src)) {
				return false;
			}

			zval_ptr_dtor(dst);
			ZVAL_COPY_VALUE(dst, &new_prop);
			Z_PROP_FLAG_P(dst) = Z_PROP_FLAG_P(src);

			src++;
			dst++;
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
			} else if (!zend_opcache_static_cache_clone_request_local_value(context, &new_prop, prop)) {
				return false;
			}

			if (key != NULL) {
				_zend_hash_append(new_object->properties, key, &new_prop);
			} else {
				zend_hash_index_add_new(new_object->properties, num_key, &new_prop);
			}
		} ZEND_HASH_FOREACH_END();
	}

	return true;
}

static bool zend_opcache_static_cache_clone_request_local_std_object(
		zend_opcache_static_cache_request_local_clone_context *context,
		zend_object *old_object,
		zend_object **new_object_ptr)
{
	zend_object *new_object;
	zval *dst, *end;

	new_object = zend_objects_new(old_object->ce);

	if (new_object->ce->default_properties_count) {
		dst = new_object->properties_table;
		end = dst + new_object->ce->default_properties_count;
		do {
			ZVAL_UNDEF(dst);
			dst++;
		} while (dst != end);
	}

	zend_hash_index_update_ptr(
		&context->objects,
		(zend_ulong) (uintptr_t) old_object,
		new_object
	);

	if (!zend_opcache_static_cache_clone_request_local_object_members(context, old_object, new_object)) {
		return false;
	}

	*new_object_ptr = new_object;

	return true;
}

static bool zend_opcache_static_cache_clone_request_local_value_callback(
		void *context,
		zval *dst,
		zval *src)
{
	return zend_opcache_static_cache_clone_request_local_value(
		(zend_opcache_static_cache_request_local_clone_context *) context,
		dst,
		src
	);
}

static bool zend_opcache_static_cache_clone_request_local_safe_direct_object(
		zend_opcache_static_cache_request_local_clone_context *context,
		zend_object *old_object,
		zend_object **new_object_ptr)
{
	zend_opcache_static_cache_safe_direct_state_copy_func_t copy_func;
	zend_class_entry *ce, *base_ce = NULL;
	zend_object *new_object;
	zend_ulong key;
	zval new_zv;

	ce = old_object->ce;

	copy_func = zend_opcache_static_cache_safe_direct_copy_func(ce, &base_ce);
	if (copy_func == NULL || zend_opcache_serializer_has_safe_direct_cache_overrides(ce, base_ce)) {
		return false;
	}

	ZVAL_UNDEF(&new_zv);
	if (object_init_ex(&new_zv, ce) != SUCCESS) {
		return false;
	}

	new_object = Z_OBJ(new_zv);
	key = (zend_ulong) (uintptr_t) old_object;
	zend_hash_index_update_ptr(&context->objects, key, new_object);

	if (!copy_func(
		context,
		old_object,
		new_object,
		zend_opcache_static_cache_clone_request_local_value_callback
	)) {
		return false;
	}

	if (!zend_opcache_static_cache_clone_request_local_object_members(context, old_object, new_object)) {
		return false;
	}

	*new_object_ptr = new_object;

	return true;
}

static bool zend_opcache_static_cache_clone_request_local_object(
		zend_opcache_static_cache_request_local_clone_context *context,
		zend_object *old_object,
		zend_object **new_object_ptr)
{
	zend_object *new_object;
	zend_ulong key;

	if (old_object == NULL || zend_object_is_lazy(old_object)) {
		return false;
	}

	key = (zend_ulong) (uintptr_t) old_object;
	new_object = zend_hash_index_find_ptr(&context->objects, key);

	if (new_object != NULL) {
		*new_object_ptr = new_object;

		return true;
	}

	if (old_object->handlers == zend_get_std_object_handlers()) {
		return zend_opcache_static_cache_clone_request_local_std_object(context, old_object, new_object_ptr);
	}

	return zend_opcache_static_cache_clone_request_local_safe_direct_object(context, old_object, new_object_ptr);
}

static bool zend_opcache_static_cache_clone_request_local_value(
		zend_opcache_static_cache_request_local_clone_context *context,
		zval *dst,
		zval *src)
{
	zend_object *object;

	if (Z_ISREF_P(src)) {
		return zend_opcache_static_cache_clone_request_local_reference(context, dst, Z_REF_P(src));
	}

	switch (Z_TYPE_P(src)) {
		case IS_ARRAY:
			return zend_opcache_static_cache_clone_request_local_array_ex(context, dst, src, false);
		case IS_OBJECT:
			if (!zend_opcache_static_cache_clone_request_local_object(context, Z_OBJ_P(src), &object)) {
				return false;
			}

			GC_ADDREF(object);
			ZVAL_OBJ(dst, object);

			return true;
		default:
			ZVAL_COPY(dst, src);

			return true;
	}
}

static bool zend_opcache_static_cache_clone_request_local_slot_value(zval *dst, zval *src)
{
	zend_opcache_static_cache_request_local_clone_context context;
	bool result;

	if (Z_TYPE_P(src) == IS_ARRAY) {
		if (!zend_opcache_static_cache_value_needs_request_local_clone(src)) {
			ZVAL_COPY(dst, src);

			return true;
		}

		zend_opcache_static_cache_request_local_clone_context_init(&context);
		result = zend_opcache_static_cache_clone_request_local_array_ex(&context, dst, src, true);
		zend_opcache_static_cache_request_local_clone_context_destroy(&context);

		return result;
	}

	if (!zend_opcache_static_cache_value_needs_request_local_clone(src)) {
		ZVAL_COPY(dst, src);

		return true;
	}

	zend_opcache_static_cache_request_local_clone_context_init(&context);
	result = zend_opcache_static_cache_clone_request_local_value(&context, dst, src);
	zend_opcache_static_cache_request_local_clone_context_destroy(&context);

	return result;
}

static HashTable *zend_opcache_static_cache_request_local_slots(void)
{
	HashTable **slots_ptr = zend_opcache_static_cache_active_request_local_slots_ptr();

	if (*slots_ptr == NULL) {
		ALLOC_HASHTABLE(*slots_ptr);
		zend_hash_init(*slots_ptr, 0, NULL, zend_opcache_static_cache_request_local_slot_dtor, 0);
	}

	return *slots_ptr;
}

static zend_never_inline void zend_opcache_static_cache_reacquire_read_lock_or_fail(const char *cache_name)
{
	if (!zend_opcache_static_cache_rlock()) {
		zend_error_noreturn(E_ERROR, "Unable to reacquire the %s read lock after userland execution", cache_name);
	}
}

static zend_never_inline bool zend_opcache_static_cache_reacquire_write_lock_or_fail(const char *cache_name)
{
	if (!zend_opcache_static_cache_wlock()) {
		zend_error_noreturn(E_ERROR, "Unable to reacquire the %s write lock after userland execution", cache_name);
	}

	if (!zend_opcache_static_cache_header_init_locked()) {
		zend_throw_exception_ex(zend_opcache_static_cache_exception_ce, 0, "Unable to initialize the %s header", cache_name);

		return false;
	}

	return true;
}

static bool zend_opcache_static_cache_materialize_payload_locked(
		zend_string *key,
		uint8_t value_type,
		uint32_t value_offset,
		uint32_t value_len,
		zval *return_value,
		bool throw_if_missing,
		const char *cache_name)
{
	const unsigned char *position, *end;
	php_unserialize_data_t var_hash;
	uint32_t copied_value_len;
	bool ref_registered;
	unsigned char *payload_copy;
	int result;

	switch (value_type) {
		case ZEND_OPCACHE_STATIC_CACHE_VALUE_SERIALIZED:
			copied_value_len = value_len;
			payload_copy = emalloc(copied_value_len);
			memcpy(payload_copy, zend_opcache_static_cache_ptr(value_offset), copied_value_len);
			zend_opcache_static_cache_unlock();

			ZVAL_UNDEF(return_value);
			position = payload_copy;
			end = position + copied_value_len;
			PHP_VAR_UNSERIALIZE_INIT(var_hash);
			result = php_var_unserialize(return_value, &position, end, &var_hash);
			PHP_VAR_UNSERIALIZE_DESTROY(var_hash);
			efree(payload_copy);

			zend_opcache_static_cache_reacquire_read_lock_or_fail(cache_name);

			if (result != 1 || position != end) {
				if (Z_TYPE_P(return_value) != IS_UNDEF) {
					zval_ptr_dtor(return_value);
					ZVAL_UNDEF(return_value);
				}

				if (!EG(exception) && throw_if_missing) {
					zend_throw_exception_ex(zend_opcache_static_cache_exception_ce, 0, "Stored %s value for key \"%s\" is corrupted", cache_name, ZSTR_VAL(key));
				}

				return false;
			}

			return true;
		case ZEND_OPCACHE_STATIC_CACHE_VALUE_OPCACHE_SERIALIZED:
			copied_value_len = value_len;
			payload_copy = emalloc(copied_value_len);
			memcpy(payload_copy, zend_opcache_static_cache_ptr(value_offset), copied_value_len);
			zend_opcache_static_cache_unlock();

			ZVAL_UNDEF(return_value);
			result = (zend_opcache_static_cache_capture_active
					? zend_opcache_unserialize_ex(
						return_value,
						payload_copy,
						copied_value_len,
						zend_opcache_static_cache_capture_decoded_value,
						zend_opcache_static_cache_capture_decoded_reachable_value
					)
					: zend_opcache_unserialize(
						return_value,
						payload_copy,
						copied_value_len
					)
				);
			efree(payload_copy);

			zend_opcache_static_cache_reacquire_read_lock_or_fail(cache_name);

			if (!result) {
				if (Z_TYPE_P(return_value) != IS_UNDEF) {
					zval_ptr_dtor(return_value);
					ZVAL_UNDEF(return_value);
				}

				if (!EG(exception) && throw_if_missing) {
					zend_throw_exception_ex(zend_opcache_static_cache_exception_ce, 0, "Stored %s value for key \"%s\" is corrupted", cache_name, ZSTR_VAL(key));
				}

				return false;
			}

			return true;
		case ZEND_OPCACHE_STATIC_CACHE_VALUE_SHARED_GRAPH:
			ref_registered = zend_opcache_static_cache_has_request_shared_graph_ref(value_offset);
			if (!ref_registered && !zend_opcache_static_cache_shared_graph_acquire_locked(value_offset)) {
				if (throw_if_missing) {
					zend_throw_exception_ex(zend_opcache_static_cache_exception_ce, 0, "Stored %s value for key \"%s\" is corrupted", cache_name, ZSTR_VAL(key));
				}

				return false;
			}

			zend_opcache_static_cache_unlock();
			ZVAL_UNDEF(return_value);
			result = zend_opcache_static_cache_fetch_shared_graph(
				(const unsigned char *) zend_opcache_static_cache_ptr(value_offset),
				value_len,
				return_value
			);
			zend_opcache_static_cache_reacquire_read_lock_or_fail(cache_name);

			if (!result) {
				if (Z_TYPE_P(return_value) != IS_UNDEF) {
					zval_ptr_dtor(return_value);
					ZVAL_UNDEF(return_value);
				}

				if (!ref_registered && zend_opcache_static_cache_shared_graph_release_ref_locked(value_offset)) {
					zend_opcache_static_cache_defer_retired_shared_graph_free(value_offset);
				}

				if (!EG(exception) && throw_if_missing) {
					zend_throw_exception_ex(zend_opcache_static_cache_exception_ce, 0, "Stored %s value for key \"%s\" is corrupted", cache_name, ZSTR_VAL(key));
				}

				return false;
			}

			if (!ref_registered) {
				zend_opcache_static_cache_register_shared_graph_ref(value_offset);
			}

			return true;
		default:
			return false;
	}
}

static bool zend_opcache_static_cache_fetch_request_local_slot(zend_string *key, uint64_t mutation_epoch, zval *return_value)
{
	zend_opcache_static_cache_request_local_slot *slot;
	HashTable **slots_ptr = zend_opcache_static_cache_active_request_local_slots_ptr();

	if (*slots_ptr == NULL) {
		return false;
	}

	slot = zend_hash_find_ptr(*slots_ptr, key);
	if (slot == NULL) {
		return false;
	}

	if (slot->mutation_epoch != mutation_epoch) {
		zend_hash_del(*slots_ptr, key);

		return false;
	}

	if (!zend_opcache_static_cache_clone_request_local_slot_value(return_value, &slot->value)) {
		zend_hash_del(*slots_ptr, key);

		return false;
	}

	return true;
}

static void zend_opcache_static_cache_store_request_local_value_slot(zend_string *key, uint64_t mutation_epoch, zval *value)
{
	zend_opcache_static_cache_request_local_slot *slot;
	zval slot_zv;

	slot = emalloc(sizeof(zend_opcache_static_cache_request_local_slot));
	slot->mutation_epoch = mutation_epoch;
	ZVAL_UNDEF(&slot->value);
	if (!zend_opcache_static_cache_clone_request_local_slot_value(&slot->value, value)) {
		ZVAL_PTR(&slot_zv, slot);
		zend_opcache_static_cache_request_local_slot_dtor(&slot_zv);

		return;
	}
	zend_hash_update_ptr(zend_opcache_static_cache_request_local_slots(), key, slot);
}

static bool zend_opcache_static_cache_fetch_finish(
		zend_string *key,
		uint64_t mutation_epoch,
		zval *return_value,
		bool use_request_local_slot)
{
	if (use_request_local_slot) {
		/* Keep a request-local prototype. Later fetches clone object handles out
		 * of this prototype instead of rematerializing from SHM. */
		zend_opcache_static_cache_store_request_local_value_slot(key, mutation_epoch, return_value);
	}

	return true;
}

static bool zend_opcache_static_cache_find_slot_in_header_locked(
		zend_opcache_static_cache_header *header,
		zend_string *key,
		zend_ulong hash,
		uint32_t *slot_index,
		bool *found,
		bool delete_expired)
{
	zend_opcache_static_cache_entry *entries, *entry;
	uint64_t now = 0;
	uint32_t index, first_tombstone = UINT32_MAX, step;

	if (header == NULL) {
		return false;
	}

	entries = zend_opcache_static_cache_entries(header);
	index = (uint32_t) (hash % header->capacity);

	for (step = 0; step < header->capacity; step++) {
		entry = &entries[index];

		if (entry->state == ZEND_OPCACHE_STATIC_CACHE_ENTRY_EMPTY) {
			*slot_index = first_tombstone != UINT32_MAX ? first_tombstone : index;
			*found = false;

			return true;
		}

		if (entry->state == ZEND_OPCACHE_STATIC_CACHE_ENTRY_TOMBSTONE) {
			if (first_tombstone == UINT32_MAX) {
				first_tombstone = index;
			}
		} else if (zend_opcache_static_cache_maybe_expired(entry, &now)) {
			if (delete_expired) {
				zend_opcache_static_cache_delete_entry_locked(entry, header);
			}

			if (first_tombstone == UINT32_MAX) {
				first_tombstone = index;
			}
		} else if (zend_opcache_static_cache_key_equals(entry, key, hash)) {
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

static bool zend_opcache_static_cache_find_slot_locked(
		zend_string *key,
		zend_ulong hash,
		zend_opcache_static_cache_header **header_ptr,
		uint32_t *slot_index,
		bool *found,
		bool delete_expired)
{
	zend_opcache_static_cache_header *header = zend_opcache_static_cache_header_ptr();

	if (!header || !zend_opcache_static_cache_header_init_locked()) {
		return false;
	}

	if (header_ptr != NULL) {
		*header_ptr = header;
	}

	return zend_opcache_static_cache_find_slot_in_header_locked(header, key, hash, slot_index, found, delete_expired);
}

static bool zend_opcache_static_cache_find_slot_for_read_locked(
		zend_string *key,
		zend_ulong hash,
		zend_opcache_static_cache_header **header_ptr,
		uint32_t *slot_index,
		bool *found)
{
	return zend_opcache_static_cache_find_slot_locked(key, hash, header_ptr, slot_index, found, false);
}

static bool zend_opcache_static_cache_find_slot_for_write_locked(
		zend_string *key,
		zend_ulong hash,
		zend_opcache_static_cache_header **header_ptr,
		uint32_t *slot_index,
		bool *found)
{
	return zend_opcache_static_cache_find_slot_locked(key, hash, header_ptr, slot_index, found, true);
}

static bool zend_opcache_static_cache_expunge_expired_locked(void)
{
	zend_opcache_static_cache_header *header = zend_opcache_static_cache_header_ptr();
	zend_opcache_static_cache_entry *entries;
	uint64_t now;
	uint32_t index;
	bool removed = false;

	if (!header || !zend_opcache_static_cache_header_init_locked()) {
		return false;
	}

	now = (uint64_t) time(NULL);
	entries = zend_opcache_static_cache_entries(header);
	for (index = 0; index < header->capacity; index++) {
		if (zend_opcache_static_cache_is_expired(&entries[index], now)) {
			zend_opcache_static_cache_delete_entry_locked(&entries[index], header);
			removed = true;
		}
	}

	return removed;
}

static void zend_opcache_static_cache_handle_store_failure(const char *failure_message, bool throw_on_failure, bool honor_strict_store_failure)
{
	zend_opcache_static_cache_context *context = zend_opcache_static_cache_active_context();

	if (honor_strict_store_failure && context->strict_store_failure && !throw_on_failure) {
		zend_opcache_static_cache_mark_publish_skipped(context);
		zend_throw_exception_ex(zend_opcache_static_cache_exception_ce, 0, "%s", failure_message);

		return;
	}

	if (throw_on_failure) {
		zend_throw_exception_ex(zend_opcache_static_cache_exception_ce, 0, "%s", failure_message);
	}
}

static bool zend_opcache_static_cache_find_unstorable_value(
		zval *value,
		HashTable *seen_arrays,
		HashTable *seen_objects,
		const char **failure_message)
{
	zend_object *object;
	zend_ulong key;
	zval *element, *property, *end;

	ZVAL_DEREF(value);
	if (Z_TYPE_P(value) == IS_RESOURCE) {
		*failure_message = "resources cannot be stored in the static cache";

		return true;
	}

	if (Z_TYPE_P(value) == IS_OBJECT && Z_OBJCE_P(value) == zend_ce_closure) {
		*failure_message = "Closure objects cannot be stored in the static cache";

		return true;
	}

	if (Z_TYPE_P(value) == IS_ARRAY) {
		key = (zend_ulong) (uintptr_t) Z_ARR_P(value);
		if (zend_hash_index_exists(seen_arrays, key)) {
			return false;
		}

		zend_hash_index_add_empty_element(seen_arrays, key);

		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(value), element) {
			if (zend_opcache_static_cache_find_unstorable_value(element, seen_arrays, seen_objects, failure_message)) {
				return true;
			}
		} ZEND_HASH_FOREACH_END();

		return false;
	}

	if (Z_TYPE_P(value) == IS_OBJECT) {
		object = Z_OBJ_P(value);
		key = (zend_ulong) (uintptr_t) object;

		if (zend_hash_index_exists(seen_objects, key)) {
			return false;
		}

		zend_hash_index_add_empty_element(seen_objects, key);

		if (object->ce->default_properties_count != 0) {
			property = object->properties_table;
			end = property + object->ce->default_properties_count;
			do {
				if (Z_TYPE_P(property) != IS_UNDEF &&
					zend_opcache_static_cache_find_unstorable_value(property, seen_arrays, seen_objects, failure_message)
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

				if (zend_opcache_static_cache_find_unstorable_value(element, seen_arrays, seen_objects, failure_message)) {
					return true;
				}
			} ZEND_HASH_FOREACH_END();
		}
	}

	return false;
}

static bool zend_opcache_static_cache_validate_storable_value(zval *value, bool throw_on_failure, bool honor_strict_store_failure)
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
	found = zend_opcache_static_cache_find_unstorable_value(value, &seen_arrays, &seen_objects, &failure_message);
	zend_hash_destroy(&seen_objects);
	zend_hash_destroy(&seen_arrays);
	if (EG(exception)) {
		return false;
	}

	if (!found) {
		return true;
	}

	if (failure_message != NULL) {
		zend_opcache_static_cache_handle_store_failure(failure_message, throw_on_failure, honor_strict_store_failure);
	}

	return false;
}

static unsigned char *zend_opcache_static_cache_reserve_combined_offset_value_locked(
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
	if (reusable_offset != 0 && zend_opcache_static_cache_block_payload_capacity(reusable_offset) >= total_size) {
		base_offset = reusable_offset;
	} else {
		base_offset = zend_opcache_static_cache_alloc_locked(total_size, NULL);
		if (base_offset == 0) {
			return NULL;
		}
	}

	*value_offset = base_offset;
	*key_offset = base_offset + (uint32_t) payload_size;

	return (unsigned char *) zend_opcache_static_cache_ptr(base_offset);
}

static bool zend_opcache_static_cache_publish_combined_offset_value_locked(
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

	payload = zend_opcache_static_cache_reserve_combined_offset_value_locked(
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

static bool zend_opcache_static_cache_retry_store_after_pressure_locked(
		bool *retried_expired,
		bool *retried_compact,
		bool *retried_clear,
		size_t required_payload_size,
		bool allow_clear)
{
	if (!*retried_expired) {
		*retried_expired = true;

		if (zend_opcache_static_cache_expunge_expired_locked()) {
			return true;
		}
	}

	if (required_payload_size != 0 && !*retried_compact) {
		if (zend_opcache_static_cache_compact_to_fit_locked(required_payload_size)) {
			*retried_compact = true;

			return true;
		}
	}

	if (allow_clear && zend_opcache_static_cache_active_context()->clear_on_pressure && !*retried_clear) {
		*retried_clear = true;

		return zend_opcache_static_cache_has_all_entry_locks() && zend_opcache_static_cache_clear_locked();
	}

	return false;
}

bool zend_opcache_static_cache_clear_locked(void)
{
	zend_opcache_static_cache_header *header = zend_opcache_static_cache_header_ptr();
	zend_opcache_static_cache_entry *entries;
	uint64_t mutation_epoch;
	uint32_t index;

	if (!header || !zend_opcache_static_cache_header_init_locked()) {
		return false;
	}

	mutation_epoch = header->mutation_epoch;
	entries = zend_opcache_static_cache_entries(header);
	for (index = 0; index < header->capacity; index++) {
		if (entries[index].state == ZEND_OPCACHE_STATIC_CACHE_ENTRY_USED) {
			zend_opcache_static_cache_release_entry_storage_locked(&entries[index]);
		}
	}

	memset(entries, 0, sizeof(zend_opcache_static_cache_entry) * header->capacity);
	memset(header->entry_lock_leases, 0, sizeof(header->entry_lock_leases));

	header->count = 0;
	header->mutation_epoch = mutation_epoch + 1;
	if (header->mutation_epoch == 0) {
		header->mutation_epoch = 1;
	}

	zend_opcache_static_cache_mark_publish_skipped(zend_opcache_static_cache_active_context());

	return true;
}

bool zend_opcache_static_cache_prepare_value(
		zend_opcache_static_cache_prepared_value *prepared,
		zend_string *key,
		zval *value,
		bool throw_on_failure,
		bool honor_strict_store_failure,
		bool lock_held)
{
	php_serialize_data_t var_hash;
	smart_str serialized = {0};
	size_t shared_graph_len, encoded_len;
	unsigned char *encoded;
	bool failed_unstorable;

	if (prepared == NULL) {
		return false;
	}

	zend_opcache_static_cache_init_prepared_value(prepared);
	ZVAL_DEREF(value);
	prepared->hash = zend_string_hash_val(key);

	if (Z_TYPE_P(value) == IS_RESOURCE) {
		zend_opcache_static_cache_handle_store_failure(
			"resources cannot be stored in the static cache",
			throw_on_failure,
			honor_strict_store_failure
		);

		return false;
	}

	if (Z_TYPE_P(value) == IS_OBJECT && Z_OBJCE_P(value) == zend_ce_closure) {
		zend_opcache_static_cache_handle_store_failure(
			"Closure objects cannot be stored in the static cache",
			throw_on_failure,
			honor_strict_store_failure
		);

		return false;
	}

	if (!zend_opcache_static_cache_validate_storable_value(value, throw_on_failure, honor_strict_store_failure)) {
		return false;
	}

	switch (Z_TYPE_P(value)) {
		case IS_NULL:
			prepared->value_type = ZEND_OPCACHE_STATIC_CACHE_VALUE_NULL;
			return true;
		case IS_TRUE:
			prepared->value_type = ZEND_OPCACHE_STATIC_CACHE_VALUE_TRUE;
			return true;
		case IS_FALSE:
			prepared->value_type = ZEND_OPCACHE_STATIC_CACHE_VALUE_FALSE;
			return true;
		case IS_LONG:
			prepared->value_type = ZEND_OPCACHE_STATIC_CACHE_VALUE_LONG;
			prepared->long_value = Z_LVAL_P(value);
			return true;
		case IS_DOUBLE:
			prepared->value_type = ZEND_OPCACHE_STATIC_CACHE_VALUE_DOUBLE;
			prepared->double_value = Z_DVAL_P(value);
			return true;
		case IS_STRING:
			prepared->value_type = ZEND_OPCACHE_STATIC_CACHE_VALUE_STRING;
			prepared->value_len = (uint32_t) Z_STRLEN_P(value);
			prepared->payload_size = Z_STRLEN_P(value) + 1;
			prepared->payload_used_size = prepared->payload_size;
			prepared->payload_source = (const unsigned char *) Z_STRVAL_P(value);
			return true;
		default:
			/* The distinct-key write benchmark stores the same source graph many times
			 * in one request. Reusing a request-local prepared shared graph is only
			 * safe when we can prove the source graph stayed clean, so the memo layer
			 * tracks reachable array/object mutations and excludes internal safe-direct
			 * objects whose state may change outside those hooks. */
			if (zend_opcache_static_cache_prepare_memo_fetch(value, prepared)) {
				return true;
			}

			shared_graph_len = 0;
			if (zend_opcache_static_cache_calculate_shared_graph_size(value, &shared_graph_len)) {
				prepared->value_type = ZEND_OPCACHE_STATIC_CACHE_VALUE_SHARED_GRAPH;
				prepared->value_len = (uint32_t) shared_graph_len;
				prepared->payload_size = shared_graph_len;

				/* Explicit stores keep shared-graph construction out of the write lock
				 * regardless of payload size. Even small direct builds lengthen the
				 * critical section enough to regress contended writers. */
				if (lock_held) {
					return true;
				}

				prepared->owned_buffer = emalloc(shared_graph_len);
				if (zend_opcache_static_cache_build_shared_graph_in_place(
						value,
						prepared->owned_buffer,
						shared_graph_len,
						&prepared->payload_used_size
					)
				) {
					prepared->payload_source = prepared->owned_buffer;
					zend_opcache_static_cache_prepare_memo_store(value, prepared);

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

			encoded = NULL;
			encoded_len = 0;
			failed_unstorable = false;
			if (zend_opcache_serialize_ex(&encoded, &encoded_len, value, &failed_unstorable)) {
				prepared->value_type = ZEND_OPCACHE_STATIC_CACHE_VALUE_OPCACHE_SERIALIZED;
				prepared->value_len = (uint32_t) encoded_len;
				prepared->payload_size = encoded_len;
				prepared->payload_used_size = encoded_len;
				prepared->owned_buffer = encoded;
				prepared->payload_source = encoded;

				return true;
			}

			if (failed_unstorable) {
				zend_opcache_static_cache_handle_store_failure(
					"resources and Closure objects cannot be stored in the static cache",
					throw_on_failure,
					honor_strict_store_failure
				);

				return false;
			}

			if (EG(exception)) {
				return false;
			}

			PHP_VAR_SERIALIZE_INIT(var_hash);
			php_var_serialize(&serialized, value, &var_hash);
			PHP_VAR_SERIALIZE_DESTROY(var_hash);
			if (serialized.s == NULL) {
				if (EG(exception)) {
					return false;
				}

				zend_opcache_static_cache_handle_store_failure(
					"failed to serialize value for cache storage",
					throw_on_failure,
					honor_strict_store_failure
				);

				return false;
			}

			prepared->value_type = ZEND_OPCACHE_STATIC_CACHE_VALUE_SERIALIZED;
			prepared->value_len = (uint32_t) ZSTR_LEN(serialized.s);
			prepared->payload_size = ZSTR_LEN(serialized.s);
			prepared->payload_used_size = prepared->payload_size;
			prepared->owned_string = serialized.s;
			prepared->payload_source = (const unsigned char *) ZSTR_VAL(serialized.s);
			serialized.s = NULL;

			return true;
	}
}

void zend_opcache_static_cache_destroy_prepared_value(zend_opcache_static_cache_prepared_value *prepared)
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

	zend_opcache_static_cache_init_prepared_value(prepared);
}

bool zend_opcache_static_cache_store_prepared_locked(
		zend_string *key,
		zval *value,
		const zend_opcache_static_cache_prepared_value *prepared,
		zend_long ttl,
		bool throw_on_failure,
		bool honor_strict_store_failure)
{
	const char *failure_message;
	zend_opcache_static_cache_header *header;
	zend_opcache_static_cache_entry *entries, *entry;
	zend_long new_long_value = 0;
	php_serialize_data_t var_hash;
	smart_str serialized = {0};
	uint64_t expires_at;
	uint32_t slot_index, offset = 0, graph_offset = 0, reusable_offset, old_key_offset = 0, old_value_offset = 0,
			new_key_offset = 0, new_value_offset = 0, new_value_len = 0, combined_reusable_offset = 0;
	uint16_t old_reserved = 0, new_reserved = 0;
	uint8_t old_value_type = ZEND_OPCACHE_STATIC_CACHE_VALUE_NULL, new_value_type;
	size_t encoded_len, serialized_len, key_payload_size, failed_payload_size;
	bool found, retried_expired = false, retried_compact = false, retried_clear = false, allow_clear, old_combined, use_combined_publish, failed_unstorable;
	unsigned char *encoded, *combined_payload;
	double new_double_value = 0;

	ZVAL_DEREF(value);

	if (prepared == NULL) {
		return false;
	}

	key_payload_size = ZSTR_LEN(key) + 1;

retry_store:
	old_key_offset = 0;
	old_value_type = ZEND_OPCACHE_STATIC_CACHE_VALUE_NULL;
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

	if (!zend_opcache_static_cache_find_slot_for_write_locked(key, prepared->hash, &header, &slot_index, &found)) {
		if (zend_opcache_static_cache_retry_store_after_pressure_locked(&retried_expired, &retried_compact, &retried_clear, 0, true)) {
			goto retry_store;
		}

		zend_opcache_static_cache_handle_store_failure("cache hash table is full", throw_on_failure, honor_strict_store_failure);

		return false;
	}

	entries = zend_opcache_static_cache_entries(header);
	entry = &entries[slot_index];

	if (found) {
		old_key_offset = entry->key_offset;
		old_value_type = entry->value_type;
		old_value_offset = entry->value_offset;
		old_reserved = entry->reserved;
	}

	old_combined = found && (old_reserved & ZEND_OPCACHE_STATIC_CACHE_ENTRY_RESERVED_COMBINED_VALUE_KEY) != 0;
	reusable_offset = found && old_value_type != ZEND_OPCACHE_STATIC_CACHE_VALUE_SHARED_GRAPH && !old_combined ? old_value_offset : 0;
	new_key_offset = found ? old_key_offset : 0;

	/* Offset-backed payloads prefer the combined publish path so a new entry, and
	 * a slot that was already stored in combined form, can replace value+key with
	 * one allocator decision. That keeps overwrite bookkeeping local to this slot
	 * instead of bouncing between separate key and value lifetimes. */
	use_combined_publish = zend_opcache_static_cache_value_uses_offset(prepared->value_type) && (!found || old_combined);

	/* In-place reuse is intentionally narrower than combined publish itself.
	 * Generic combined payloads only reuse the hot-key case, but prepared shared
	 * graphs are already built outside the lock. For those explicit-store writes
	 * we can afford a cheap overwrite check on every matching key to avoid the
	 * allocator churn that still shows up in distinct-key contention. */
	if (old_combined && old_value_offset != 0) {
		if (old_value_type == ZEND_OPCACHE_STATIC_CACHE_VALUE_SHARED_GRAPH) {
			if (prepared->payload_source != NULL &&
				zend_opcache_static_cache_shared_graph_can_overwrite_payload_locked(old_value_offset) &&
				(prepared->value_type != ZEND_OPCACHE_STATIC_CACHE_VALUE_SHARED_GRAPH ||
					(prepared->payload_source != NULL && zend_opcache_static_cache_shared_graph_copy_fits_buffer(
							prepared->payload_source,
							prepared->payload_size,
							prepared->payload_used_size,
							(const unsigned char *) zend_opcache_static_cache_ptr(old_value_offset),
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
		new_key_offset = zend_opcache_static_cache_alloc_locked(key_payload_size, ZSTR_VAL(key));
		if (new_key_offset == 0) {
			failure_message = "not enough shared memory left";
			failed_payload_size = key_payload_size;
			allow_clear = zend_opcache_static_cache_payload_can_fit_locked(key_payload_size);

			goto store_failed;
		}
	}

	switch (prepared->value_type) {
		case ZEND_OPCACHE_STATIC_CACHE_VALUE_NULL:
			new_value_type = ZEND_OPCACHE_STATIC_CACHE_VALUE_NULL;
			break;
		case ZEND_OPCACHE_STATIC_CACHE_VALUE_TRUE:
			new_value_type = ZEND_OPCACHE_STATIC_CACHE_VALUE_TRUE;
			break;
		case ZEND_OPCACHE_STATIC_CACHE_VALUE_FALSE:
			new_value_type = ZEND_OPCACHE_STATIC_CACHE_VALUE_FALSE;
			break;
		case ZEND_OPCACHE_STATIC_CACHE_VALUE_LONG:
			new_value_type = ZEND_OPCACHE_STATIC_CACHE_VALUE_LONG;
			new_long_value = prepared->long_value;
			break;
		case ZEND_OPCACHE_STATIC_CACHE_VALUE_DOUBLE:
			new_value_type = ZEND_OPCACHE_STATIC_CACHE_VALUE_DOUBLE;
			new_double_value = prepared->double_value;
			break;
		case ZEND_OPCACHE_STATIC_CACHE_VALUE_STRING:
		case ZEND_OPCACHE_STATIC_CACHE_VALUE_OPCACHE_SERIALIZED:
		case ZEND_OPCACHE_STATIC_CACHE_VALUE_SERIALIZED:
			if (use_combined_publish) {
				if (!zend_opcache_static_cache_publish_combined_offset_value_locked(
						combined_reusable_offset,
						key,
						prepared->payload_size,
						prepared->payload_source,
						&new_value_offset,
						&new_key_offset)
				) {
					failure_message = "not enough shared memory left";
					failed_payload_size = prepared->payload_size + key_payload_size;
					allow_clear = zend_opcache_static_cache_payload_can_fit_locked(prepared->payload_size + key_payload_size);

					goto store_failed;
				}

				new_reserved = ZEND_OPCACHE_STATIC_CACHE_ENTRY_RESERVED_COMBINED_VALUE_KEY;
			} else {
				offset = zend_opcache_static_cache_write_payload_locked(
					reusable_offset,
					prepared->payload_size,
					prepared->payload_source);
				if (offset == 0) {
					failure_message = "not enough shared memory left";
					failed_payload_size = prepared->payload_size;
					allow_clear = zend_opcache_static_cache_payload_can_fit_locked(prepared->payload_size);

					goto store_failed;
				}

				new_value_offset = offset;
			}

			new_value_type = prepared->value_type;
			new_value_len = prepared->value_len;
			break;
		case ZEND_OPCACHE_STATIC_CACHE_VALUE_SHARED_GRAPH:
			if (use_combined_publish) {
				/* Shared-graph fetch helpers treat value_offset as the start of the SHM
				 * payload, so the graph must stay at the block base. Reserve the whole
				 * combined block first, publish the graph into the base payload, then
				 * append the key bytes after the graph payload. */
				combined_payload = zend_opcache_static_cache_reserve_combined_offset_value_locked(
					prepared->payload_source != NULL ? combined_reusable_offset : 0,
					key,
					prepared->payload_size,
					&new_value_offset,
					&new_key_offset
				);
				graph_offset = new_value_offset;

				if (combined_payload != NULL) {
					/* Prepared shared-graph buffers may contain direct array payloads with
					 * final-buffer pointers, so the SHM destination must be the build site. */
					if (zend_opcache_static_cache_build_shared_graph_in_place(
							value,
							combined_payload,
							prepared->payload_size,
							NULL)
					) {
						memcpy(combined_payload + prepared->payload_size, ZSTR_VAL(key), key_payload_size);
						new_reserved = ZEND_OPCACHE_STATIC_CACHE_ENTRY_RESERVED_COMBINED_VALUE_KEY;
						new_value_type = ZEND_OPCACHE_STATIC_CACHE_VALUE_SHARED_GRAPH;
						new_value_offset = graph_offset;
						new_value_len = prepared->value_len;
						break;
					}

					if (graph_offset != combined_reusable_offset) {
						zend_opcache_static_cache_free_locked(graph_offset);
					}
					graph_offset = 0;
					new_value_offset = 0;
					new_key_offset = found ? old_key_offset : 0;

					if (EG(exception)) {
						return false;
					}
				} else if (
					zend_opcache_static_cache_payload_can_fit_locked(prepared->payload_size + key_payload_size) &&
					zend_opcache_static_cache_retry_store_after_pressure_locked(&retried_expired, &retried_compact, &retried_clear, prepared->payload_size + key_payload_size, true)
				) {
					goto retry_store;
				}
			} else {
				graph_offset = zend_opcache_static_cache_alloc_locked(prepared->payload_size, NULL);

				if (graph_offset != 0) {
					/* Prepared shared-graph buffers may contain direct array payloads with
					 * final-buffer pointers, so the SHM destination must be the build site. */
					if (zend_opcache_static_cache_build_shared_graph_in_place(
							value,
							(unsigned char *) zend_opcache_static_cache_ptr(graph_offset),
							prepared->payload_size,
							NULL)
					) {
						new_value_type = ZEND_OPCACHE_STATIC_CACHE_VALUE_SHARED_GRAPH;
						new_value_offset = graph_offset;
						new_value_len = prepared->value_len;
						break;
					}

					zend_opcache_static_cache_free_locked(graph_offset);

					if (EG(exception)) {
						return false;
					}
				} else if (
					zend_opcache_static_cache_payload_can_fit_locked(prepared->payload_size) &&
					zend_opcache_static_cache_retry_store_after_pressure_locked(&retried_expired, &retried_compact, &retried_clear, prepared->payload_size, true)
				) {
					goto retry_store;
				}
			}

			offset = reusable_offset;
			encoded = NULL;
			encoded_len = 0;
			failed_unstorable = false;
			if (zend_opcache_serialize_ex(&encoded, &encoded_len, value, &failed_unstorable)) {
				if (use_combined_publish) {
					if (!zend_opcache_static_cache_publish_combined_offset_value_locked(
							combined_reusable_offset,
							key,
							encoded_len,
							encoded,
							&new_value_offset,
							&new_key_offset)
					) {
						efree(encoded);
						failure_message = "not enough shared memory left";
						failed_payload_size = encoded_len + key_payload_size;
						allow_clear = zend_opcache_static_cache_payload_can_fit_locked(encoded_len + key_payload_size);

						goto store_failed;
					}

					new_reserved = ZEND_OPCACHE_STATIC_CACHE_ENTRY_RESERVED_COMBINED_VALUE_KEY;
				} else {
					offset = zend_opcache_static_cache_write_payload_locked(offset, encoded_len, encoded);
					if (offset == 0) {
						efree(encoded);
						failure_message = "not enough shared memory left";
						failed_payload_size = encoded_len;
						allow_clear = zend_opcache_static_cache_payload_can_fit_locked(encoded_len);

						goto store_failed;
					}

					new_value_offset = offset;
				}
				efree(encoded);
				new_value_type = ZEND_OPCACHE_STATIC_CACHE_VALUE_OPCACHE_SERIALIZED;
				new_value_len = (uint32_t) encoded_len;
				break;
			}

			if (failed_unstorable) {
				failure_message = "resources and Closure objects cannot be stored in the static cache";
				allow_clear = false;

				goto store_failed;
			}

			if (EG(exception)) {
				return false;
			}

			PHP_VAR_SERIALIZE_INIT(var_hash);
			php_var_serialize(&serialized, value, &var_hash);
			PHP_VAR_SERIALIZE_DESTROY(var_hash);
			if (serialized.s == NULL) {
				if (EG(exception)) {
					return false;
				}

				failure_message = "failed to serialize value for cache storage";
				allow_clear = false;

				goto store_failed;
			}

			serialized_len = ZSTR_LEN(serialized.s);
			if (use_combined_publish) {
				if (!zend_opcache_static_cache_publish_combined_offset_value_locked(
						combined_reusable_offset,
						key,
						serialized_len,
						ZSTR_VAL(serialized.s),
						&new_value_offset,
						&new_key_offset)
				) {
					smart_str_free(&serialized);
					failure_message = "not enough shared memory left";
					failed_payload_size = serialized_len + key_payload_size;
					allow_clear = zend_opcache_static_cache_payload_can_fit_locked(serialized_len + key_payload_size);

					goto store_failed;
				}

				new_reserved = ZEND_OPCACHE_STATIC_CACHE_ENTRY_RESERVED_COMBINED_VALUE_KEY;
			} else {
				offset = zend_opcache_static_cache_write_payload_locked(reusable_offset, serialized_len, ZSTR_VAL(serialized.s));
				if (offset == 0) {
					smart_str_free(&serialized);
					failure_message = "not enough shared memory left";
					failed_payload_size = serialized_len;
					allow_clear = zend_opcache_static_cache_payload_can_fit_locked(serialized_len);

					goto store_failed;
				}

				new_value_offset = offset;
			}
			smart_str_free(&serialized);
			new_value_type = ZEND_OPCACHE_STATIC_CACHE_VALUE_SERIALIZED;
			new_value_len = (uint32_t) serialized_len;
			break;
		default:
			ZEND_UNREACHABLE();
	}

	entry->hash = prepared->hash;
	entry->key_offset = new_key_offset;
	entry->key_len = (uint32_t) ZSTR_LEN(key);
	entry->value_offset = new_value_offset;
	entry->value_len = new_value_len;
	entry->expires_at = expires_at;
	entry->state = ZEND_OPCACHE_STATIC_CACHE_ENTRY_USED;
	entry->value_type = new_value_type;
	entry->reserved = new_reserved;
	entry->long_value = new_long_value;
	entry->double_value = new_double_value;

	if (found && old_key_offset != 0 && old_key_offset != new_key_offset && !old_combined) {
		zend_opcache_static_cache_free_locked(old_key_offset);
	}

	if (found && old_value_offset != 0 && old_value_offset != new_value_offset) {
		zend_opcache_static_cache_release_value_storage_locked(old_value_type, old_value_offset);
	}

	if (!found) {
		header->count++;
	}

	zend_opcache_static_cache_bump_mutation_epoch_locked(header);

	return true;

store_failed:
	if (new_value_offset != 0 && new_value_offset != old_value_offset) {
		zend_opcache_static_cache_free_locked(new_value_offset);
	}

	if (new_key_offset != 0 && new_key_offset != old_key_offset &&
		(new_reserved & ZEND_OPCACHE_STATIC_CACHE_ENTRY_RESERVED_COMBINED_VALUE_KEY) == 0
	) {
		zend_opcache_static_cache_free_locked(new_key_offset);
	}

	if (zend_opcache_static_cache_retry_store_after_pressure_locked(&retried_expired, &retried_compact, &retried_clear, failed_payload_size, allow_clear)) {
		goto retry_store;
	}

	zend_opcache_static_cache_handle_store_failure(failure_message, throw_on_failure, honor_strict_store_failure);

	return false;
}

bool zend_opcache_static_cache_store_locked(zend_string *key, zval *value, zend_long ttl, bool throw_on_failure, bool honor_strict_store_failure)
{
	const char *cache_name = zend_opcache_static_cache_active_context()->name;
	zend_opcache_static_cache_prepared_value prepared;
	bool stored;

	/* Value preparation may invoke userland serialization hooks. Keep the caller's
	 * lock contract by dropping the write lock only for preparation and
	 * reacquiring it before returning. */
	zend_opcache_static_cache_unlock();
	if (!zend_opcache_static_cache_prepare_value(&prepared, key, value, throw_on_failure, honor_strict_store_failure, false)) {
		zend_opcache_static_cache_reacquire_write_lock_or_fail(cache_name);

		return false;
	}

	if (!zend_opcache_static_cache_reacquire_write_lock_or_fail(cache_name)) {
		zend_opcache_static_cache_destroy_prepared_value(&prepared);

		return false;
	}

	stored = zend_opcache_static_cache_store_prepared_locked(key, value, &prepared, ttl, throw_on_failure, honor_strict_store_failure);
	zend_opcache_static_cache_destroy_prepared_value(&prepared);

	return stored;
}

bool zend_opcache_static_cache_fetch_locked(zend_string *key, zval *return_value, bool throw_if_missing, bool *found_ptr, bool use_request_local_slot)
{
	const char *cache_name = zend_opcache_static_cache_active_context()->name;
	zend_opcache_static_cache_header *header;
	zend_opcache_static_cache_entry *entries, *entry;
	zend_opcache_static_cache_lookup_entry *lookup_entries, *lookup_entry;
	zend_ulong hash;
	uint64_t mutation_epoch, now;
	uint32_t way, slot_index;
	bool found;

	if (found_ptr != NULL) {
		*found_ptr = false;
	}

	hash = zend_string_hash_val(key);

	header = zend_opcache_static_cache_header_ptr();
	if (!header || !zend_opcache_static_cache_header_is_initialized_locked()) {
		if (throw_if_missing) {
			zend_throw_exception_ex(zend_opcache_static_cache_exception_ce, 0, "Cache key \"%s\" was not found", ZSTR_VAL(key));
		}

		return false;
	}
	entries = zend_opcache_static_cache_entries(header);
	mutation_epoch = header->mutation_epoch;
	lookup_entries = zend_opcache_static_cache_lookup_cache_set(hash);

	/* Request-local lookup cache: repeated fetches of the same key avoid the
	 * shared hash-table probe. A cached miss is just as valuable for userland
	 * exists/fetch probes, and the mutation epoch keeps both hit and miss records
	 * coherent after another writer updates the segment. */
	for (way = 0; way < ZEND_OPCACHE_STATIC_CACHE_LOOKUP_WAYS; way++) {
		lookup_entry = &lookup_entries[way];
		if (lookup_entry->state == ZEND_OPCACHE_STATIC_CACHE_LOOKUP_EMPTY || lookup_entry->hash != hash) {
			continue;
		}

		if (lookup_entry->mutation_epoch != mutation_epoch) {
			zend_opcache_static_cache_lookup_cache_reset_entry(lookup_entry);
			continue;
		}

		if (lookup_entry->state == ZEND_OPCACHE_STATIC_CACHE_LOOKUP_MISS) {
			if (throw_if_missing) {
				zend_throw_exception_ex(zend_opcache_static_cache_exception_ce, 0, "Cache key \"%s\" was not found", ZSTR_VAL(key));
			}

			return false;
		}

		if (lookup_entry->slot_index >= header->capacity) {
			zend_opcache_static_cache_lookup_cache_reset_entry(lookup_entry);
			continue;
		}

		entry = &entries[lookup_entry->slot_index];
		if (!zend_opcache_static_cache_key_equals(entry, key, hash)) {
			zend_opcache_static_cache_lookup_cache_reset_entry(lookup_entry);
			continue;
		}

		now = 0;
		if (zend_opcache_static_cache_maybe_expired(entry, &now)) {
			zend_opcache_static_cache_lookup_cache_reset_entry(lookup_entry);
			zend_opcache_static_cache_lookup_cache_store_miss(lookup_entries, hash, mutation_epoch);
			if (throw_if_missing) {
				zend_throw_exception_ex(zend_opcache_static_cache_exception_ce, 0, "Cache key \"%s\" was not found", ZSTR_VAL(key));
			}

			return false;
		}

		slot_index = lookup_entry->slot_index;
		found = true;

		goto value_found;
	}

	if (!zend_opcache_static_cache_find_slot_in_header_locked(header, key, hash, &slot_index, &found, false) || !found) {
		zend_opcache_static_cache_lookup_cache_store_miss(lookup_entries, hash, mutation_epoch);
		if (throw_if_missing) {
			zend_throw_exception_ex(zend_opcache_static_cache_exception_ce, 0, "Cache key \"%s\" was not found", ZSTR_VAL(key));
		}

		return false;
	}

	lookup_entry = zend_opcache_static_cache_lookup_cache_select_slot(lookup_entries, hash, mutation_epoch, true);
	if (lookup_entry != NULL) {
		zend_opcache_static_cache_lookup_cache_store_hit(lookup_entry, hash, mutation_epoch, slot_index);
	}

value_found:
	if (found_ptr != NULL) {
		*found_ptr = true;
	}

	entry = &entries[slot_index];
	if (use_request_local_slot) {
		if (zend_opcache_static_cache_fetch_request_local_slot(key, mutation_epoch, return_value)) {
			return true;
		}
		if (EG(exception)) {
			return false;
		}
	}

	switch (entry->value_type) {
		case ZEND_OPCACHE_STATIC_CACHE_VALUE_NULL:
			ZVAL_NULL(return_value);
			return zend_opcache_static_cache_fetch_finish(key, mutation_epoch, return_value, use_request_local_slot);
		case ZEND_OPCACHE_STATIC_CACHE_VALUE_TRUE:
			ZVAL_TRUE(return_value);
			return zend_opcache_static_cache_fetch_finish(key, mutation_epoch, return_value, use_request_local_slot);
		case ZEND_OPCACHE_STATIC_CACHE_VALUE_FALSE:
			ZVAL_FALSE(return_value);
			return zend_opcache_static_cache_fetch_finish(key, mutation_epoch, return_value, use_request_local_slot);
		case ZEND_OPCACHE_STATIC_CACHE_VALUE_LONG:
			ZVAL_LONG(return_value, entry->long_value);
			return zend_opcache_static_cache_fetch_finish(key, mutation_epoch, return_value, use_request_local_slot);
		case ZEND_OPCACHE_STATIC_CACHE_VALUE_DOUBLE:
			ZVAL_DOUBLE(return_value, entry->double_value);
			return zend_opcache_static_cache_fetch_finish(key, mutation_epoch, return_value, use_request_local_slot);
		case ZEND_OPCACHE_STATIC_CACHE_VALUE_STRING:
			ZVAL_STRINGL(return_value, zend_opcache_static_cache_ptr(entry->value_offset), entry->value_len);
			return zend_opcache_static_cache_fetch_finish(key, mutation_epoch, return_value, use_request_local_slot);
		case ZEND_OPCACHE_STATIC_CACHE_VALUE_SERIALIZED:
		case ZEND_OPCACHE_STATIC_CACHE_VALUE_OPCACHE_SERIALIZED:
		case ZEND_OPCACHE_STATIC_CACHE_VALUE_SHARED_GRAPH:
			if (!zend_opcache_static_cache_materialize_payload_locked(
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

			return zend_opcache_static_cache_fetch_finish(key, mutation_epoch, return_value, use_request_local_slot);
		default:
			if (throw_if_missing) {
				zend_throw_exception_ex(zend_opcache_static_cache_exception_ce, 0, "Stored %s value for key \"%s\" has an unknown type", cache_name, ZSTR_VAL(key));
			}

			return false;
	}
}

bool zend_opcache_static_cache_exists_locked(zend_string *key)
{
	zend_ulong hash = zend_string_hash_val(key);
	uint32_t slot_index;
	bool found;

	if (!zend_opcache_static_cache_find_slot_for_read_locked(key, hash, NULL, &slot_index, &found)) {
		return false;
	}

	return found;
}

bool zend_opcache_static_cache_delete_locked(zend_string *key)
{
	zend_opcache_static_cache_header *header;
	zend_opcache_static_cache_entry *entries;
	zend_ulong hash = zend_string_hash_val(key);
	uint32_t slot_index;
	bool found;

	if (!zend_opcache_static_cache_find_slot_for_write_locked(key, hash, &header, &slot_index, &found) || !found) {
		return true;
	}

	entries = zend_opcache_static_cache_entries(header);
	zend_opcache_static_cache_delete_entry_locked(&entries[slot_index], header);

	return true;
}

static zend_always_inline bool zend_opcache_static_cache_long_add_wrapped(
		zend_long lhs,
		zend_long rhs,
		zend_long *result)
{
	*result = (zend_long) ((zend_ulong) lhs + (zend_ulong) rhs);

	return (rhs > 0 && lhs > ZEND_LONG_MAX - rhs) ||
		(rhs < 0 && lhs < ZEND_LONG_MIN - rhs);
}

static zend_always_inline bool zend_opcache_static_cache_long_sub_wrapped(
		zend_long lhs,
		zend_long rhs,
		zend_long *result)
{
	*result = (zend_long) ((zend_ulong) lhs - (zend_ulong) rhs);

	return (rhs > 0 && lhs < ZEND_LONG_MIN + rhs) ||
		(rhs < 0 && lhs > ZEND_LONG_MAX + rhs);
}

bool zend_opcache_static_cache_atomic_update_locked(
		zend_string *key,
		zend_long step,
		bool decrement,
		bool insert_if_missing,
		zend_long *new_value,
		bool *is_overflow,
		const char *type_error_message,
		bool throw_on_error)
{
	zend_opcache_static_cache_header *header;
	zend_opcache_static_cache_entry *entries, *entry;
	zend_ulong hash = zend_string_hash_val(key);
	zval initial_value = {0};
	zend_long result;
	uint32_t slot_index;
	bool found, result_is_overflow;

	*is_overflow = false;

	if (!zend_opcache_static_cache_find_slot_for_write_locked(key, hash, &header, &slot_index, &found) || !found) {
		if (insert_if_missing) {
			result_is_overflow = decrement
				? zend_opcache_static_cache_long_sub_wrapped(0, step, &result)
				: zend_opcache_static_cache_long_add_wrapped(0, step, &result);
			ZVAL_LONG(&initial_value, result);
			if (zend_opcache_static_cache_store_locked(key, &initial_value, 0, throw_on_error, false)) {
				*is_overflow = result_is_overflow;
				*new_value = Z_LVAL(initial_value);

				return true;
			}

			return false;
		}

		if (throw_on_error) {
			zend_throw_exception_ex(zend_opcache_static_cache_exception_ce, 0, "Cache key \"%s\" was not found", ZSTR_VAL(key));
		}

		return false;
	}

	entries = zend_opcache_static_cache_entries(header);
	entry = &entries[slot_index];
	if (entry->value_type != ZEND_OPCACHE_STATIC_CACHE_VALUE_LONG) {
		if (throw_on_error) {
			if (entry->value_type > ZEND_OPCACHE_STATIC_CACHE_VALUE_SHARED_GRAPH) {
				zend_throw_exception_ex(zend_opcache_static_cache_exception_ce, 0, "Stored %s value for key \"%s\" has an unknown type", zend_opcache_static_cache_active_context()->name, ZSTR_VAL(key));
			} else {
				zend_throw_exception_ex(zend_opcache_static_cache_exception_ce, 0, "%s", type_error_message);
			}
		}

		return false;
	}

	result_is_overflow = decrement
		? zend_opcache_static_cache_long_sub_wrapped(entry->long_value, step, &result)
		: zend_opcache_static_cache_long_add_wrapped(entry->long_value, step, &result);
	entry->long_value = result;
	*is_overflow = result_is_overflow;

	zend_opcache_static_cache_bump_mutation_epoch_locked(header);
	*new_value = entry->long_value;

	return true;
}

void zend_opcache_static_cache_release_request_local_slots(void)
{
	zend_opcache_static_cache_release_request_local_slot_context(&zend_opcache_static_cache_volatile_request_local_slots);
	zend_opcache_static_cache_release_request_local_slot_context(&zend_opcache_static_cache_pinned_request_local_slots);
}
