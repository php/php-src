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

#include "zend_interfaces.h"
#include "zend_enum.h"
#include "zend_operators.h"
#include "zend_user_cache_internal.h"
#include "ext/spl/php_spl.h"

/* Per-decode maps, allocated lazily only when the graph has aliases. */
static ZEND_EXT_TLS HashTable *zend_opcache_user_cache_decode_identity_map = NULL;
static ZEND_EXT_TLS HashTable *zend_opcache_user_cache_decode_reference_map = NULL;
static ZEND_EXT_TLS HashTable *zend_opcache_user_cache_decode_array_map = NULL;

/* Request-lifetime class/enum lookup cache keyed by pinned payload addresses. */
ZEND_EXT_TLS HashTable *zend_opcache_user_cache_decode_resolve_cache = NULL;

static void zend_opcache_user_cache_decode_identity_object_dtor(zval *zv);
static void zend_opcache_user_cache_decode_reference_dtor(zval *zv);
static void zend_opcache_user_cache_decode_array_dtor(zval *zv);
static bool zend_opcache_user_cache_shared_graph_rebase_direct_array(
	zend_array *array,
	const unsigned char *old_base,
	const unsigned char *new_base,
	size_t len,
	ptrdiff_t delta,
	HashTable *seen_arrays
);

static zend_always_inline void *zend_opcache_user_cache_decode_resolve_cache_find(const void *address)
{
	if (zend_opcache_user_cache_decode_resolve_cache == NULL) {
		return NULL;
	}

	return zend_hash_index_find_ptr(
		zend_opcache_user_cache_decode_resolve_cache,
		(zend_ulong) (uintptr_t) address
	);
}

static zend_always_inline void zend_opcache_user_cache_decode_resolve_cache_store(const void *address, void *value)
{
	if (zend_opcache_user_cache_decode_resolve_cache == NULL) {
		zend_opcache_user_cache_decode_resolve_cache = emalloc(sizeof(HashTable));
		zend_hash_init(zend_opcache_user_cache_decode_resolve_cache, 8, NULL, NULL, 0);
	}

	zend_hash_index_add_ptr(
		zend_opcache_user_cache_decode_resolve_cache,
		(zend_ulong) (uintptr_t) address,
		value
	);
}

static zend_always_inline void zend_opcache_user_cache_decode_identity_map_teardown(void)
{
	if (zend_opcache_user_cache_decode_identity_map != NULL) {
		zend_hash_destroy(zend_opcache_user_cache_decode_identity_map);
		efree(zend_opcache_user_cache_decode_identity_map);
		zend_opcache_user_cache_decode_identity_map = NULL;
	}
}

static zend_always_inline bool zend_opcache_user_cache_decode_identity_map_insert(uint32_t offset, zend_object *object)
{
	if (zend_opcache_user_cache_decode_identity_map == NULL) {
		zend_opcache_user_cache_decode_identity_map = emalloc(sizeof(HashTable));
		zend_hash_init(zend_opcache_user_cache_decode_identity_map, 8, NULL,
			zend_opcache_user_cache_decode_identity_object_dtor, 0
		);
	}

	GC_ADDREF(object);
	if (zend_hash_index_add_ptr(zend_opcache_user_cache_decode_identity_map, offset, object) == NULL) {
		OBJ_RELEASE(object);

		return false;
	}

	return true;
}

static zend_always_inline zend_object *zend_opcache_user_cache_decode_identity_map_find(uint32_t offset)
{
	if (zend_opcache_user_cache_decode_identity_map == NULL) {
		return NULL;
	}

	return zend_hash_index_find_ptr(zend_opcache_user_cache_decode_identity_map, offset);
}

static zend_always_inline void zend_opcache_user_cache_decode_reference_map_teardown(void)
{
	if (zend_opcache_user_cache_decode_reference_map != NULL) {
		zend_hash_destroy(zend_opcache_user_cache_decode_reference_map);
		efree(zend_opcache_user_cache_decode_reference_map);
		zend_opcache_user_cache_decode_reference_map = NULL;
	}
}

static zend_always_inline bool zend_opcache_user_cache_decode_reference_map_insert(uint32_t offset, zend_reference *reference)
{
	if (zend_opcache_user_cache_decode_reference_map == NULL) {
		zend_opcache_user_cache_decode_reference_map = emalloc(sizeof(HashTable));
		zend_hash_init(zend_opcache_user_cache_decode_reference_map,
			8,
			NULL,
			zend_opcache_user_cache_decode_reference_dtor, 0
		);
	}

	GC_ADDREF(reference);

	if (zend_hash_index_add_ptr(zend_opcache_user_cache_decode_reference_map, offset, reference) == NULL) {
		if (GC_DELREF(reference) == 0) {
			efree_size(reference, sizeof(zend_reference));
		}

		return false;
	}

	return true;
}

static zend_always_inline zend_reference *zend_opcache_user_cache_decode_reference_map_find(uint32_t offset)
{
	if (zend_opcache_user_cache_decode_reference_map == NULL) {
		return NULL;
	}

	return zend_hash_index_find_ptr(zend_opcache_user_cache_decode_reference_map, offset);
}

static zend_always_inline void zend_opcache_user_cache_decode_array_map_teardown(void)
{
	if (zend_opcache_user_cache_decode_array_map != NULL) {
		zend_hash_destroy(zend_opcache_user_cache_decode_array_map);

		efree(zend_opcache_user_cache_decode_array_map);

		zend_opcache_user_cache_decode_array_map = NULL;
	}
}

static zend_always_inline bool zend_opcache_user_cache_decode_array_map_insert(uint32_t offset, zend_array *array)
{
	if (zend_opcache_user_cache_decode_array_map == NULL) {
		zend_opcache_user_cache_decode_array_map = emalloc(sizeof(HashTable));

		zend_hash_init(zend_opcache_user_cache_decode_array_map, 8, NULL,
			zend_opcache_user_cache_decode_array_dtor, 0
		);
	}

	GC_ADDREF(array);
	if (zend_hash_index_add_ptr(zend_opcache_user_cache_decode_array_map, offset, array) == NULL) {
		if (GC_DELREF(array) == 0) {
			zend_array_destroy(array);
		}

		return false;
	}

	return true;
}

static zend_always_inline zend_array *zend_opcache_user_cache_decode_array_map_find(uint32_t offset)
{
	if (zend_opcache_user_cache_decode_array_map == NULL) {
		return NULL;
	}

	return zend_hash_index_find_ptr(zend_opcache_user_cache_decode_array_map, offset);
}

/* Graphs are aligned within generic SHM blocks. */
static zend_always_inline size_t zend_opcache_user_cache_shared_graph_alignment_padding(const void *buffer)
{
	uintptr_t raw_address, aligned_address;

	raw_address = (uintptr_t) buffer;
	aligned_address = (uintptr_t) ZEND_MM_ALIGNED_SIZE(raw_address);

	return (size_t) (aligned_address - raw_address);
}

static zend_always_inline bool zend_opcache_user_cache_shared_graph_offset_in_block(
	uint32_t offset,
	uint32_t block_offset,
	uint32_t block_size)
{
	return offset >= block_offset + sizeof(zend_opcache_user_cache_block) && offset < block_offset + block_size;
}

static zend_always_inline const unsigned char *zend_opcache_user_cache_shared_graph_locate_buffer(
	const unsigned char *buffer,
	size_t buffer_len,
	size_t *graph_len)
{
	const zend_opcache_user_cache_shared_graph_header *header;
	size_t padding;

	padding = zend_opcache_user_cache_shared_graph_alignment_padding(buffer);
	if (padding > buffer_len || buffer_len - padding < sizeof(zend_opcache_user_cache_shared_graph_header)) {
		return NULL;
	}

	buffer += padding;
	buffer_len -= padding;
	header = (const zend_opcache_user_cache_shared_graph_header *) buffer;
	if (header->magic != ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_MAGIC ||
		header->version != ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VERSION
	) {
		return NULL;
	}

	if (graph_len != NULL) {
		*graph_len = buffer_len;
	}

	return buffer;
}

static zend_always_inline void zend_opcache_user_cache_shared_graph_calc_init(zend_opcache_user_cache_shared_graph_calc_context *context)
{
	context->size = 0;

	zend_hash_init(&context->seen_arrays, 8, NULL, NULL, 0);
	zend_hash_init(&context->seen_objects, 8, NULL, NULL, 0);
	zend_hash_init(&context->seen_references, 8, NULL, NULL, 0);
	zend_hash_init(&context->string_dedup, 8, NULL, NULL, 0);
}

static zend_always_inline void zend_opcache_user_cache_shared_graph_calc_destroy(zend_opcache_user_cache_shared_graph_calc_context *context)
{
	zend_hash_destroy(&context->string_dedup);
	zend_hash_destroy(&context->seen_references);
	zend_hash_destroy(&context->seen_objects);
	zend_hash_destroy(&context->seen_arrays);
}

static zend_always_inline bool zend_opcache_user_cache_shared_graph_reserve_size(size_t *size, size_t amount)
{
	size_t aligned_amount;

	aligned_amount = ZEND_ALIGNED_SIZE(amount);
	if (*size > SIZE_MAX - aligned_amount) {
		return false;
	}

	*size += aligned_amount;

	return *size <= UINT32_MAX;
}

/* Match the copy pass's string deduplication. */
static zend_always_inline bool zend_opcache_user_cache_shared_graph_calc_reserve_string(
	zend_opcache_user_cache_shared_graph_calc_context *context,
	const zend_string *string)
{
	if (zend_hash_exists(&context->string_dedup, (zend_string *) string)) {
		return true;
	}

	if (!zend_opcache_user_cache_shared_graph_reserve_size(&context->size, _ZSTR_STRUCT_SIZE(ZSTR_LEN(string)))) {
		return false;
	}

	return zend_hash_add_empty_element(&context->string_dedup, (zend_string *) string) != NULL;
}

static zend_always_inline bool zend_opcache_user_cache_shared_graph_mark_seen(HashTable *seen_objects, zend_object *object)
{
	zend_ulong object_key;

	object_key = (zend_ulong) (uintptr_t) object;
	if (zend_hash_index_exists(seen_objects, object_key)) {
		return false;
	}

	return zend_hash_index_add_empty_element(seen_objects, object_key) != NULL;
}

static zend_always_inline bool zend_opcache_user_cache_shared_graph_mark_seen_array(HashTable *seen_arrays, const HashTable *array)
{
	zend_ulong array_key;

	array_key = (zend_ulong) (uintptr_t) array;
	if (zend_hash_index_exists(seen_arrays, array_key)) {
		return false;
	}

	return zend_hash_index_add_empty_element(seen_arrays, array_key) != NULL;
}

static zend_always_inline bool zend_opcache_user_cache_shared_graph_can_restore_direct(zend_class_entry *ce)
{
	/* Non-serializable classes expose no usable value state. */
	if (ce->ce_flags & ZEND_ACC_NOT_SERIALIZABLE) {
		return false;
	}

	/* Custom internal objects need a registered safe-direct handler. */
	if (ce->type != ZEND_USER_CLASS && ce->create_object != NULL) {
		return false;
	}

	return true;
}

static zend_always_inline bool zend_opcache_user_cache_shared_graph_property_is_default(
		zend_object *object,
		uint32_t property_index,
		const zval *property_value)
{
	zend_property_info *property_info;
	zval *default_properties, *default_value;

	if (object->ce->type != ZEND_USER_CLASS ||
		object->ce->properties_info_table == NULL ||
		property_index >= object->ce->default_properties_count
	) {
		return false;
	}

	property_info = object->ce->properties_info_table[property_index];
	default_properties = CE_DEFAULT_PROPERTIES_TABLE(object->ce);
	default_value = &default_properties[OBJ_PROP_TO_NUM(property_info->offset)];

	return Z_TYPE_P(default_value) != IS_UNDEF && zend_is_identical(property_value, default_value);
}

static zend_always_inline bool zend_opcache_user_cache_shared_graph_is_unmangled_property_name(zend_string *property_name)
{
	return ZSTR_LEN(property_name) != 0 && ZSTR_VAL(property_name)[0] != '\0';
}

static zend_always_inline bool zend_opcache_user_cache_shared_graph_safe_direct_property_shadows_state(
		zval *state, zend_string *property_name)
{
	return zend_opcache_user_cache_shared_graph_is_unmangled_property_name(property_name) &&
		Z_TYPE_P(state) == IS_ARRAY &&
		zend_hash_exists(Z_ARRVAL_P(state), property_name)
	;
}

static zend_always_inline bool zend_opcache_user_cache_shared_graph_should_skip_safe_direct_property(
		zend_object *object,
		zval *state,
		zend_string *property_name,
		uint32_t property_index,
		const zval *property_value)
{
	return zend_opcache_user_cache_shared_graph_safe_direct_property_shadows_state(state, property_name) ||
		zend_opcache_user_cache_shared_graph_property_is_default(object, property_index, property_value)
	;
}

static zend_always_inline bool zend_opcache_user_cache_shared_graph_can_use_safe_direct(zend_class_entry *ce)
{
	zend_class_entry *base_ce = zend_opcache_user_cache_safe_direct_find_base(ce);

	return base_ce != NULL &&
		zend_opcache_user_cache_safe_direct_state_serialize_func(ce) != NULL &&
		zend_opcache_user_cache_safe_direct_state_unserialize_func(ce) != NULL
	;
}

static zend_always_inline bool zend_opcache_user_cache_shared_graph_pointer_in_range(
		const void *pointer,
		const unsigned char *base,
		size_t len)
{
	uintptr_t address, start;

	if (pointer == NULL || base == NULL || len == 0) {
		return false;
	}

	address = (uintptr_t) pointer;
	start = (uintptr_t) base;

	return address >= start && address - start < len;
}

static zend_always_inline void *zend_opcache_user_cache_shared_graph_rebase_pointer(
		void *pointer,
		const unsigned char *old_base,
		size_t len,
		ptrdiff_t delta)
{
	if (!zend_opcache_user_cache_shared_graph_pointer_in_range(pointer, old_base, len)) {
		return pointer;
	}

	return (void *) ((char *) pointer - delta);
}

static bool zend_opcache_user_cache_shared_graph_can_copy_direct_value(HashTable *seen_arrays, const zval *value)
{
	const HashTable *array;
	const Bucket *bucket;
	const zval *packed_value;
	zend_ulong array_key;
	uint32_t index;
	bool result = true;

	switch (Z_TYPE_P(value)) {
		case IS_UNDEF:
		case IS_NULL:
		case IS_FALSE:
		case IS_TRUE:
		case IS_LONG:
		case IS_DOUBLE:
		case IS_STRING:
			return true;
		case IS_ARRAY:
			array = Z_ARRVAL_P(value);
			if (array->nNumOfElements == 0) {
				return true;
			}

			if (!zend_opcache_user_cache_shared_graph_mark_seen_array(seen_arrays, array)) {
				return false;
			}

			array_key = (zend_ulong) (uintptr_t) array;
			if (HT_IS_PACKED(array)) {
				for (index = 0; index < array->nNumUsed; index++) {
					packed_value = &array->arPacked[index];
					if (!zend_opcache_user_cache_shared_graph_can_copy_direct_value(seen_arrays, packed_value)) {
						result = false;
						break;
					}
				}
			} else {
				bucket = array->arData;
				for (index = 0; index < array->nNumUsed; index++) {
					if (Z_TYPE(bucket[index].val) != IS_UNDEF &&
						!zend_opcache_user_cache_shared_graph_can_copy_direct_value(seen_arrays, &bucket[index].val)
					) {
						result = false;
						break;
					}
				}
			}

			zend_hash_index_del(seen_arrays, array_key);

			return result;
		default:
			return false;
	}
}

static bool zend_opcache_user_cache_shared_graph_calc_direct_value(
		zend_opcache_user_cache_shared_graph_calc_context *context,
		const zval *value)
{
	const HashTable *array;
	const Bucket *bucket;
	const zval *packed_value;
	zend_ulong array_key;
	uint32_t index;
	size_t data_size;
	bool result = true;

	switch (Z_TYPE_P(value)) {
		case IS_UNDEF:
		case IS_NULL:
		case IS_FALSE:
		case IS_TRUE:
		case IS_LONG:
		case IS_DOUBLE:
			return true;
		case IS_STRING:
			return zend_opcache_user_cache_shared_graph_calc_reserve_string(context, Z_STR_P(value));
		case IS_ARRAY:
			array = Z_ARRVAL_P(value);
			if (array->nNumOfElements == 0) {
				return true;
			}

			if (!zend_opcache_user_cache_shared_graph_mark_seen_array(&context->seen_arrays, array)) {
				return false;
			}

			array_key = (zend_ulong) (uintptr_t) array;
			data_size = HT_IS_PACKED(array) ? HT_PACKED_USED_SIZE(array) : HT_USED_SIZE(array);
			if (!zend_opcache_user_cache_shared_graph_reserve_size(&context->size, sizeof(zend_array)) ||
				!zend_opcache_user_cache_shared_graph_reserve_size(&context->size, data_size)
			) {
				result = false;
				goto direct_array_done;
			}

			if (HT_IS_PACKED(array)) {
				for (index = 0; index < array->nNumUsed; index++) {
					packed_value = &array->arPacked[index];
					if (!zend_opcache_user_cache_shared_graph_calc_direct_value(context, packed_value)) {
						result = false;
						break;
					}
				}
			} else {
				bucket = array->arData;
				for (index = 0; index < array->nNumUsed; index++) {
					if (bucket[index].key != NULL &&
						!zend_opcache_user_cache_shared_graph_calc_reserve_string(context, bucket[index].key)
					) {
						result = false;
						break;
					}

					if (Z_TYPE(bucket[index].val) != IS_UNDEF &&
						!zend_opcache_user_cache_shared_graph_calc_direct_value(context, &bucket[index].val)
					) {
						result = false;
						break;
					}
				}
			}

direct_array_done:
			zend_hash_index_del(&context->seen_arrays, array_key);

			return result;
		default:
			return false;
	}
}

static bool zend_opcache_user_cache_shared_graph_property_is_self_object_hash(
		zend_object *object,
		uint32_t property_index,
		const zval *property_value,
		zend_string **object_hash_ptr)
{
	if (Z_TYPE_P(property_value) != IS_STRING ||
		!zend_opcache_user_cache_is_userland_declared_non_public_property(object, property_index)
	) {
		return false;
	}

	if (*object_hash_ptr == NULL) {
		*object_hash_ptr = php_spl_object_hash(object);
	}

	return zend_string_equals(Z_STR_P(property_value), *object_hash_ptr);
}

static zend_class_entry *zend_opcache_user_cache_decode_lookup_class(zend_string *class_name)
{
	zend_class_entry *ce;

	ce = zend_opcache_user_cache_decode_resolve_cache_find(class_name);
	if (ce != NULL) {
		return ce;
	}

	ce = zend_lookup_class(class_name);
	if (ce != NULL) {
		zend_opcache_user_cache_decode_resolve_cache_store(class_name, ce);
	}

	return ce;
}

static void zend_opcache_user_cache_decode_identity_object_dtor(zval *zv)
{
	OBJ_RELEASE((zend_object *) Z_PTR_P(zv));
}

static void zend_opcache_user_cache_decode_reference_dtor(zval *zv)
{
	zval tmp;

	/* Release the transient ref taken on insert (frees the reference at rc 0). */
	ZVAL_REF(&tmp, (zend_reference *) Z_PTR_P(zv));
	zval_ptr_dtor(&tmp);
}

static void zend_opcache_user_cache_decode_array_dtor(zval *zv)
{
	zval tmp;

	/* Release the transient ref taken on insert (frees the array at rc 0). */
	ZVAL_ARR(&tmp, (zend_array *) Z_PTR_P(zv));

	zval_ptr_dtor(&tmp);
}

static bool zend_opcache_user_cache_shared_graph_get_safe_direct_state(
		HashTable *state_memo,
		const zval *value,
		zval **state_ptr,
		zval *owned_state)
{
	zend_object *object = Z_OBJ_P(value);
	zend_opcache_user_cache_safe_direct_state_serialize_func_t serialize_func;
	zend_ulong memo_key;
	zval *memo_state;

	ZVAL_UNDEF(owned_state);
	*state_ptr = NULL;

	memo_key = (zend_ulong) (uintptr_t) object;
	if (state_memo != NULL) {
		memo_state = zend_hash_index_find(state_memo, memo_key);
		if (memo_state != NULL) {
			if (Z_TYPE_P(memo_state) != IS_ARRAY) {
				return false;
			}

			*state_ptr = memo_state;

			return true;
		}
	}

	serialize_func = zend_opcache_user_cache_safe_direct_state_serialize_func(object->ce);
	if (serialize_func == NULL ||
		!serialize_func(value, owned_state) ||
		Z_TYPE_P(owned_state) != IS_ARRAY
	) {
		if (!Z_ISUNDEF_P(owned_state)) {
			zval_ptr_dtor(owned_state);
			ZVAL_UNDEF(owned_state);
		}

		return false;
	}

	if (state_memo != NULL) {
		memo_state = zend_hash_index_add(state_memo, memo_key, owned_state);
		if (memo_state == NULL) {
			zval_ptr_dtor(owned_state);
			ZVAL_UNDEF(owned_state);

			return false;
		}

		ZVAL_UNDEF(owned_state);
		*state_ptr = memo_state;

		return true;
	}

	*state_ptr = owned_state;

	return true;
}

static bool zend_opcache_user_cache_shared_graph_calc_value(
	zend_opcache_user_cache_shared_graph_calc_context *context,
	const zval *value
)
{
	const HashTable *array;
	zend_ulong array_key;
	zend_string *key, *property_name, *case_name;
	zend_class_entry *ce;
	zend_reference *ref;
	zend_object *object;
	zval *element, *property_value, *source_value, *state_ptr, state;
	HashTable *properties;
	uint32_t property_count, property_index;
	bool result;

	switch (Z_TYPE_P(value)) {
		case IS_UNDEF:
		case IS_NULL:
		case IS_FALSE:
		case IS_TRUE:
		case IS_LONG:
		case IS_DOUBLE:
			return true;
		case IS_STRING:
			return zend_opcache_user_cache_shared_graph_calc_reserve_string(context, Z_STR_P(value));
		case IS_ARRAY:
			array = Z_ARRVAL_P(value);

			if (array->nNumOfElements == 0) {
				/* Preserve nNextFreeElement for empty dynamic arrays. */
				if (array->nNextFreeElement != 0) {
					return zend_opcache_user_cache_shared_graph_reserve_size(&context->size,
						sizeof(zend_opcache_user_cache_shared_graph_array))
					;
				}

				return true;
			}

			/* Immutable arrays cannot contain objects or references. */
			if ((GC_FLAGS(Z_ARRVAL_P(value)) & IS_ARRAY_IMMUTABLE) ||
				zend_opcache_user_cache_shared_graph_can_copy_direct_value(&context->seen_arrays, value)
			) {
				return zend_opcache_user_cache_shared_graph_calc_direct_value(context, value);
			}

			if (!zend_opcache_user_cache_shared_graph_mark_seen_array(&context->seen_arrays, array)) {
				/* Cyclic arrays are emitted as back-references. */
				return true;
			}

			array_key = (zend_ulong) (uintptr_t) array;
			result = true;

			if (!zend_opcache_user_cache_shared_graph_reserve_size(&context->size, sizeof(zend_opcache_user_cache_shared_graph_array)) ||
				!zend_opcache_user_cache_shared_graph_reserve_size(
					&context->size,
					(size_t) array->nNumOfElements * sizeof(zend_opcache_user_cache_shared_graph_array_element)
				)
			) {
				result = false;

				goto array_done;
			}

			ZEND_HASH_FOREACH_STR_KEY_VAL((HashTable *) array, key, element) {
				if (key != NULL && !zend_opcache_user_cache_shared_graph_calc_reserve_string(context, key)) {
					result = false;

					break;
				}

				if (!zend_opcache_user_cache_shared_graph_calc_value(context, element)) {
					result = false;

					break;
				}
			} ZEND_HASH_FOREACH_END();

			goto array_done;
		case IS_OBJECT:
			object = Z_OBJ_P(value);
			ce = object->ce;

		if (ce->ce_flags & ZEND_ACC_ENUM) {
			/* Enum cases are restored by class name + case name. */
			case_name = Z_STR_P(zend_enum_fetch_case_name(object));

			return zend_opcache_user_cache_shared_graph_reserve_size(
					&context->size,
					sizeof(zend_opcache_user_cache_shared_graph_enum)
				) &&
				zend_opcache_user_cache_shared_graph_calc_reserve_string(context, ce->name) &&
				zend_opcache_user_cache_shared_graph_calc_reserve_string(context, case_name)
			;
		}

		if (zend_opcache_user_cache_shared_graph_can_use_safe_direct(ce) &&
			zend_opcache_user_cache_shared_graph_get_safe_direct_state(
				context->state_memo,
				value,
				&state_ptr,
				&state
			)
		) {
			if (!zend_opcache_user_cache_shared_graph_reserve_size(&context->size,
				sizeof(zend_opcache_user_cache_shared_graph_safe_direct_object)) ||
				!zend_opcache_user_cache_shared_graph_calc_reserve_string(context, ce->name) ||
				!zend_opcache_user_cache_shared_graph_calc_value(context, state_ptr)
			) {
				if (!Z_ISUNDEF(state)) {
					zval_ptr_dtor(&state);
				}

				return false;
			}

			properties = zend_std_get_properties(object);
			property_count = properties != NULL ? properties->nNumOfElements : 0;
			if (property_count != 0 &&
				!zend_opcache_user_cache_shared_graph_reserve_size(
					&context->size,
					(size_t) property_count * sizeof(zend_opcache_user_cache_shared_graph_property)
				)
			) {
				if (!Z_ISUNDEF(state)) {
					zval_ptr_dtor(&state);
				}

				return false;
			}

			property_index = 0;
			result = true;
			if (properties != NULL) {
				ZEND_HASH_FOREACH_STR_KEY_VAL(properties, property_name, property_value) {
					if (property_name == NULL) {
						result = false;

						break;
					}

					source_value = Z_TYPE_P(property_value) == IS_INDIRECT
						? Z_INDIRECT_P(property_value)
						: property_value
					;

					if (zend_opcache_user_cache_shared_graph_should_skip_safe_direct_property(
							object,
							state_ptr,
							property_name,
							property_index,
							source_value
						)
					) {
						++property_index;

						continue;
					}

					if (!zend_opcache_user_cache_shared_graph_calc_reserve_string(context, property_name) ||
						!zend_opcache_user_cache_shared_graph_calc_value(context, source_value)
					) {
						result = false;

						break;
					}

					++property_index;
				} ZEND_HASH_FOREACH_END();
			}

			if (!Z_ISUNDEF(state)) {
				zval_ptr_dtor(&state);
			}

			return result;
		}

		if (!zend_opcache_user_cache_shared_graph_can_restore_direct(ce)) {
			return false;
		}

		if (!zend_opcache_user_cache_shared_graph_mark_seen(&context->seen_objects, object)) {
			/* Repeat or cyclic objects are emitted as back-references. */
			return true;
		}

		if (!zend_opcache_user_cache_shared_graph_reserve_size(&context->size, sizeof(zend_opcache_user_cache_shared_graph_object))) {
			return false;
		}

		if (!zend_opcache_user_cache_shared_graph_calc_reserve_string(context, ce->name)) {
			return false;
		}

		properties = zend_get_properties_for((zval *) value, ZEND_PROP_PURPOSE_SERIALIZE);
		property_count = properties != NULL ? properties->nNumOfElements : 0;
		if (property_count != 0 &&
			!zend_opcache_user_cache_shared_graph_reserve_size(
				&context->size,
				(size_t) property_count * sizeof(zend_opcache_user_cache_shared_graph_property)
			)
		) {
			if (properties != NULL) {
				zend_release_properties(properties);
			}

			return false;
		}

		if (properties != NULL) {
			property_index = 0;
			ZEND_HASH_FOREACH_STR_KEY_VAL(properties, property_name, property_value) {
				if (property_name == NULL) {
					zend_release_properties(properties);

					return false;
				}

				source_value = Z_TYPE_P(property_value) == IS_INDIRECT
					? Z_INDIRECT_P(property_value)
					: property_value
				;

				if (zend_opcache_user_cache_shared_graph_property_is_default(
						object,
						property_index,
						source_value
					)
				) {
					++property_index;
					continue;
				}

				if (!zend_opcache_user_cache_shared_graph_calc_reserve_string(context, property_name)) {
					zend_release_properties(properties);

					return false;
				}

				if (!zend_opcache_user_cache_shared_graph_calc_value(context, source_value)) {
					zend_release_properties(properties);

					return false;
				}

				++property_index;
			} ZEND_HASH_FOREACH_END();

			zend_release_properties(properties);
		}

		return true;
	case IS_REFERENCE: {
		ref = Z_REF_P(value);

		/* Repeat (shared or cyclic) reference -> back-ref node, no extra payload. */
		if (!zend_opcache_user_cache_shared_graph_mark_seen(&context->seen_references, (zend_object *) ref)) {
			return true;
		}

		if (!zend_opcache_user_cache_shared_graph_reserve_size(&context->size,
			sizeof(zend_opcache_user_cache_shared_graph_reference))
		) {
			return false;
		}

			return zend_opcache_user_cache_shared_graph_calc_value(context, &ref->val);
		}
		default:
			return false;
	}

array_done:
	zend_hash_index_del(&context->seen_arrays, array_key);

	return result;
}

static void zend_opcache_user_cache_shared_graph_copy_init(
	zend_opcache_user_cache_shared_graph_copy_context *context,
	unsigned char *buffer,
	size_t size
)
{
	context->buffer = buffer;
	context->size = size;
	context->position = 0;

	zend_hash_init(&context->seen_arrays, 8, NULL, NULL, 0);
	zend_hash_init(&context->seen_objects, 8, NULL, NULL, 0);
	zend_hash_init(&context->seen_references, 8, NULL, NULL, 0);
	zend_hash_init(&context->string_dedup, 8, NULL, NULL, 0);

	context->has_shared_identity = false;
	context->has_object = false;
	context->prefers_prototype = false;
}

static void zend_opcache_user_cache_shared_graph_copy_destroy(zend_opcache_user_cache_shared_graph_copy_context *context)
{
	zend_hash_destroy(&context->string_dedup);
	zend_hash_destroy(&context->seen_references);
	zend_hash_destroy(&context->seen_objects);
	zend_hash_destroy(&context->seen_arrays);
}

static bool zend_opcache_user_cache_shared_graph_copy_alloc(
	zend_opcache_user_cache_shared_graph_copy_context *context,
	size_t amount,
	uint32_t *offset
)
{
	size_t aligned_amount;

	aligned_amount = ZEND_ALIGNED_SIZE(amount);
	if (context->position > context->size || aligned_amount > context->size - context->position) {
		return false;
	}

	*offset = (uint32_t) context->position;
	memset(context->buffer + context->position, 0, aligned_amount);
	context->position += aligned_amount;

	return true;
}

static bool zend_opcache_user_cache_shared_graph_copy_string(
	zend_opcache_user_cache_shared_graph_copy_context *context,
	const zend_string *string,
	uint32_t *offset
)
{
	zend_string *new_string;
	uint32_t string_offset;
	size_t string_size;
	zval *cached, cached_offset;

	/* Payload-local string deduplication. */
	cached = zend_hash_find(&context->string_dedup, (zend_string *) string);
	if (cached != NULL) {
		*offset = (uint32_t) Z_LVAL_P(cached);

		return true;
	}

	string_size = _ZSTR_STRUCT_SIZE(ZSTR_LEN(string));
	if (!zend_opcache_user_cache_shared_graph_copy_alloc(context, string_size, &string_offset)) {
		return false;
	}

	new_string = (zend_string *) (context->buffer + string_offset);
	memcpy(new_string, string, string_size);
	GC_SET_REFCOUNT(new_string, 2);
	GC_TYPE_INFO(new_string) = GC_STRING | ((IS_STR_INTERNED | IS_STR_PERMANENT) << GC_FLAGS_SHIFT);
	*offset = string_offset;

	ZVAL_LONG(&cached_offset, (zend_long) string_offset);
	zend_hash_add(&context->string_dedup, (zend_string *) string, &cached_offset);

	return true;
}

static bool zend_opcache_user_cache_shared_graph_copy_direct_value(
		zend_opcache_user_cache_shared_graph_copy_context *context,
		const zval *source,
		zval *destination)
{
	const HashTable *source_array;
	const Bucket *source_bucket;
	const zval *source_packed;
	zend_array *target;
	zend_ulong array_key;
	zval *target_packed;
	Bucket *target_bucket;
	uint32_t string_offset, array_offset, data_offset, key_offset, index;
	size_t data_size;
	bool result = true;

	switch (Z_TYPE_P(source)) {
		case IS_UNDEF:
			ZVAL_UNDEF(destination);
			return true;
		case IS_NULL:
			ZVAL_NULL(destination);
			return true;
		case IS_TRUE:
			ZVAL_TRUE(destination);
			return true;
		case IS_FALSE:
			ZVAL_FALSE(destination);
			return true;
		case IS_LONG:
			ZVAL_LONG(destination, Z_LVAL_P(source));
			return true;
		case IS_DOUBLE:
			ZVAL_DOUBLE(destination, Z_DVAL_P(source));
			return true;
		case IS_STRING:
			if (!zend_opcache_user_cache_shared_graph_copy_string(context, Z_STR_P(source), &string_offset)) {
				return false;
			}

			ZVAL_INTERNED_STR(destination, (zend_string *) (void *) (context->buffer + string_offset));

			return true;
		case IS_ARRAY:
			source_array = Z_ARRVAL_P(source);
			if (source_array->nNumOfElements == 0) {
				ZVAL_EMPTY_ARRAY(destination);

				return true;
			}

			if (!zend_opcache_user_cache_shared_graph_mark_seen_array(&context->seen_arrays, source_array)) {
				return false;
			}

			array_key = (zend_ulong) (uintptr_t) source_array;
			data_size = HT_IS_PACKED(source_array) ? HT_PACKED_USED_SIZE(source_array) : HT_USED_SIZE(source_array);
			if (!zend_opcache_user_cache_shared_graph_copy_alloc(context, sizeof(zend_array), &array_offset) ||
				!zend_opcache_user_cache_shared_graph_copy_alloc(context, data_size, &data_offset)
			) {
				result = false;
				goto copy_direct_array_done;
			}

			target = (zend_array *) (context->buffer + array_offset);

			memcpy(target, source_array, sizeof(zend_array));
			memcpy(context->buffer + data_offset, HT_GET_DATA_ADDR(source_array), data_size);

			GC_SET_REFCOUNT(target, 2);
			GC_TYPE_INFO(target) = GC_ARRAY | ((IS_ARRAY_IMMUTABLE | GC_NOT_COLLECTABLE) << GC_FLAGS_SHIFT);

			HT_FLAGS(target) |= HASH_FLAG_STATIC_KEYS;

			target->pDestructor = NULL;
			target->nInternalPointer = 0;

			HT_SET_DATA_ADDR(target, context->buffer + data_offset);

			if (HT_IS_PACKED(source_array)) {
				target_packed = target->arPacked;
				for (index = 0; index < source_array->nNumUsed; index++) {
					source_packed = &source_array->arPacked[index];
					if (!zend_opcache_user_cache_shared_graph_copy_direct_value(context, source_packed, &target_packed[index])) {
						result = false;

						break;
					}
				}
			} else {
				source_bucket = source_array->arData;
				target_bucket = target->arData;
				for (index = 0; index < source_array->nNumUsed; index++) {
					if (source_bucket[index].key != NULL) {
						if (!zend_opcache_user_cache_shared_graph_copy_string(context, source_bucket[index].key, &key_offset)) {
							result = false;

							break;
						}

						target_bucket[index].key = (zend_string *) (void *) (context->buffer + key_offset);
					} else {
						target_bucket[index].key = NULL;
					}

					if (!zend_opcache_user_cache_shared_graph_copy_direct_value(context, &source_bucket[index].val, &target_bucket[index].val)) {
						result = false;

						break;
					}
				}
			}

copy_direct_array_done:
			zend_hash_index_del(&context->seen_arrays, array_key);

			if (!result) {
				return false;
			}

			ZVAL_ARR(destination, (zend_array *) (void *) (context->buffer + array_offset));
			Z_TYPE_FLAGS_P(destination) = 0;

			return true;
		default:
			return false;
	}
}

static bool zend_opcache_user_cache_shared_graph_copy_property_value(
	zend_opcache_user_cache_shared_graph_copy_context *context,
	const zval *source,
	zend_opcache_user_cache_shared_graph_value *destination
)
{
	zend_opcache_user_cache_shared_graph_object *graph_object;
	zend_opcache_user_cache_shared_graph_safe_direct_object *graph_safe_direct;
	zend_opcache_user_cache_shared_graph_property *graph_properties;
	zend_opcache_user_cache_shared_graph_array *graph_array;
	zend_opcache_user_cache_shared_graph_array_element *graph_elements, *graph_element;
	zend_opcache_user_cache_shared_graph_enum *graph_enum;
	zend_opcache_user_cache_shared_graph_reference *graph_reference;
	zend_reference *ref;
	zend_object *object;
	zend_class_entry *ce;
	zend_string *property_name, *key, *case_name, *self_hash;
	zend_ulong array_key, h;
	zval *property_value, *source_value, *element, array_value, *safe_direct_state_ptr, safe_direct_state;
	HashTable *properties;
	uint32_t string_offset, object_offset, class_name_offset, properties_offset,
		property_index, property_count,
		array_offset, elements_offset, key_offset,
		shared_offset,
		enum_offset, enum_class_offset, enum_case_offset,
		reference_offset,
		safe_direct_offset, safe_direct_class_offset, safe_direct_properties_offset
	;
	bool result;
	void *seen_offset;

	memset(destination, 0, sizeof(*destination));

	switch (Z_TYPE_P(source)) {
		case IS_UNDEF:
			destination->type = ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_UNDEF;

			return true;
		case IS_NULL:
			destination->type = ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_NULL;

			return true;
		case IS_TRUE:
			destination->type = ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_TRUE;

			return true;
		case IS_FALSE:
			destination->type = ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_FALSE;

			return true;
		case IS_LONG:
			destination->type = ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_LONG;
			destination->payload.long_value = Z_LVAL_P(source);

			return true;
		case IS_DOUBLE:
			destination->type = ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_DOUBLE;
			destination->payload.double_value = Z_DVAL_P(source);

			return true;
		case IS_STRING:
			if (!zend_opcache_user_cache_shared_graph_copy_string(context, Z_STR_P(source), &string_offset)) {
				return false;
			}

			destination->type = ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_STRING;
			destination->payload.offset = string_offset;

			return true;
		case IS_ARRAY: {
			result = true;

			if (Z_ARRVAL_P(source)->nNumOfElements == 0) {
				if (Z_ARRVAL_P(source)->nNextFreeElement == 0) {
					destination->type = ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_ARRAY;
					destination->payload.offset = 0;

					return true;
				}

				/* Preserve nNextFreeElement for empty dynamic arrays. */
				if (!zend_opcache_user_cache_shared_graph_copy_alloc(context, sizeof(*graph_array), &array_offset)) {
					return false;
				}

				graph_array = (zend_opcache_user_cache_shared_graph_array *) (context->buffer + array_offset);
				graph_array->count = 0;
				graph_array->next_free = (uint32_t) Z_ARRVAL_P(source)->nNextFreeElement;
				graph_array->elements_offset = 0;
				graph_array->reserved = 0;
				destination->type = ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_DYNAMIC_ARRAY;
				destination->payload.offset = array_offset;

				return true;
			}

			/* calc_value() already accepted immutable arrays. */
			if ((GC_FLAGS(Z_ARRVAL_P(source)) & IS_ARRAY_IMMUTABLE) ||
				zend_opcache_user_cache_shared_graph_can_copy_direct_value(&context->seen_arrays, source)
			) {
				if (!zend_opcache_user_cache_shared_graph_copy_direct_value(context, source, &array_value)) {
					return false;
				}

				destination->type = ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_ARRAY;
				destination->payload.offset = (uint32_t) ((unsigned char *) Z_ARRVAL(array_value) - context->buffer);

				return true;
			}

			array_key = (zend_ulong) (uintptr_t) Z_ARRVAL_P(source);

			seen_offset = zend_hash_index_find_ptr(&context->seen_arrays, array_key);
			if (seen_offset != NULL) {
				/* Back-reference to an array ancestor. */
				shared_offset = (uint32_t) (uintptr_t) seen_offset;
				((zend_opcache_user_cache_shared_graph_array *) (context->buffer + shared_offset))->reserved |=
					ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_OBJECT_FLAG_SHARED
				;

				destination->type = ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_ARRAY_REF;
				destination->payload.offset = shared_offset;
				context->has_shared_identity = true;

				return true;
			}

			if (!zend_opcache_user_cache_shared_graph_copy_alloc(context, sizeof(*graph_array), &array_offset) ||
				!zend_opcache_user_cache_shared_graph_copy_alloc(
					context,
					(size_t) Z_ARRVAL_P(source)->nNumOfElements * sizeof(*graph_elements),
					&elements_offset
				)
			) {
				result = false;

				goto array_done;
			}

			graph_array = (zend_opcache_user_cache_shared_graph_array *) (context->buffer + array_offset);
			graph_array->count = Z_ARRVAL_P(source)->nNumOfElements;
			graph_array->next_free = (uint32_t) Z_ARRVAL_P(source)->nNextFreeElement;
			graph_array->elements_offset = elements_offset;
			graph_array->reserved = 0;

			/* Record the offset before recursively encoding elements. */
			if (zend_hash_index_add_ptr(&context->seen_arrays, array_key, (void *) (uintptr_t) array_offset) == NULL) {
				result = false;

				goto array_done;
			}

			graph_elements = (zend_opcache_user_cache_shared_graph_array_element *) (context->buffer + elements_offset);
			graph_element = graph_elements;

			ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(source), h, key, element) {
				memset(graph_element, 0, sizeof(*graph_element));
				graph_element->h = h;
				if (key != NULL) {
					if (!zend_opcache_user_cache_shared_graph_copy_string(context, key, &key_offset)) {
						result = false;
						break;
					}

					graph_element->key_offset = key_offset;
				}

				if (!zend_opcache_user_cache_shared_graph_copy_property_value(context, element, &graph_element->value)) {
					result = false;

					break;
				}

				++graph_element;
			} ZEND_HASH_FOREACH_END();

			if (result) {
				destination->type = ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_DYNAMIC_ARRAY;
				destination->payload.offset = array_offset;
			}

			goto array_done;
		}
		case IS_OBJECT:
			/* Objects and enums may resolve classes during decode. */
			context->has_object = true;
			ce = Z_OBJCE_P(source);

			if (ce->ce_flags & ZEND_ACC_ENUM) {
				case_name = Z_STR_P(zend_enum_fetch_case_name(Z_OBJ_P(source)));

				if (!zend_opcache_user_cache_shared_graph_copy_alloc(context,
					sizeof(zend_opcache_user_cache_shared_graph_enum), &enum_offset) ||
					!zend_opcache_user_cache_shared_graph_copy_string(context, ce->name, &enum_class_offset) ||
					!zend_opcache_user_cache_shared_graph_copy_string(context, case_name, &enum_case_offset)
				) {
					return false;
				}

				graph_enum = (zend_opcache_user_cache_shared_graph_enum *) (context->buffer + enum_offset);
				graph_enum->class_name_offset = enum_class_offset;
				graph_enum->case_name_offset = enum_case_offset;

				destination->type = ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_ENUM;
				destination->payload.offset = enum_offset;

				return true;
			}

			object = Z_OBJ_P(source);
			if (zend_opcache_user_cache_shared_graph_can_use_safe_direct(object->ce)) {
				if (zend_opcache_user_cache_safe_direct_prefers_request_local_prototype(object->ce)) {
					context->prefers_prototype = true;
				}

				seen_offset = zend_hash_index_find_ptr(&context->seen_objects, (zend_ulong) (uintptr_t) object);
				if (seen_offset != NULL) {
					shared_offset = (uint32_t) (uintptr_t) seen_offset;
					((zend_opcache_user_cache_shared_graph_safe_direct_object *) (context->buffer + shared_offset))->reserved |=
						ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_OBJECT_FLAG_SHARED
					;

					destination->type = ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_OBJECT_REF;
					destination->payload.offset = shared_offset;
					context->has_shared_identity = true;

					return true;
				}

				if (zend_opcache_user_cache_shared_graph_get_safe_direct_state(
						context->state_memo,
						source,
						&safe_direct_state_ptr,
						&safe_direct_state
					)
				) {
					if (!zend_opcache_user_cache_shared_graph_copy_alloc(context,
							sizeof(zend_opcache_user_cache_shared_graph_safe_direct_object), &safe_direct_offset) ||
						!zend_opcache_user_cache_shared_graph_copy_string(context, object->ce->name, &safe_direct_class_offset)
					) {
						if (!Z_ISUNDEF(safe_direct_state)) {
							zval_ptr_dtor(&safe_direct_state);
						}

						return false;
					}

					if (zend_hash_index_add_ptr(&context->seen_objects, (zend_ulong) (uintptr_t) object,
						(void *) (uintptr_t) safe_direct_offset) == NULL
					) {
						if (!Z_ISUNDEF(safe_direct_state)) {
							zval_ptr_dtor(&safe_direct_state);
						}

						return false;
					}

					graph_safe_direct = (zend_opcache_user_cache_shared_graph_safe_direct_object *) (context->buffer + safe_direct_offset);
					graph_safe_direct->class_name_offset = safe_direct_class_offset;
					graph_safe_direct->reserved = 0;

					if (!zend_opcache_user_cache_shared_graph_copy_property_value(context, safe_direct_state_ptr, &graph_safe_direct->state)) {
						if (!Z_ISUNDEF(safe_direct_state)) {
							zval_ptr_dtor(&safe_direct_state);
						}

						return false;
					}

					properties = zend_std_get_properties(object);
					property_count = properties != NULL ? properties->nNumOfElements : 0;
					graph_safe_direct->property_count = property_count;
					graph_safe_direct->properties_offset = 0;

					if (property_count != 0) {
						if (!zend_opcache_user_cache_shared_graph_copy_alloc(
								context,
								(size_t) property_count * sizeof(zend_opcache_user_cache_shared_graph_property),
								&safe_direct_properties_offset
							)
						) {
							if (!Z_ISUNDEF(safe_direct_state)) {
								zval_ptr_dtor(&safe_direct_state);
							}

							return false;
						}

						graph_safe_direct->properties_offset = safe_direct_properties_offset;
						graph_properties = (zend_opcache_user_cache_shared_graph_property *) (context->buffer + safe_direct_properties_offset);
						property_index = 0;
						result = true;

						self_hash = NULL;
						ZEND_HASH_FOREACH_STR_KEY_VAL(properties, property_name, property_value) {
							graph_properties[property_index].name_offset = 0;
							graph_properties[property_index].reserved = 0;
							graph_properties[property_index].value.type = ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_UNDEF;

							if (property_name == NULL) {
								result = false;

								break;
							}

							source_value = Z_TYPE_P(property_value) == IS_INDIRECT
								? Z_INDIRECT_P(property_value)
								: property_value
							;

							if (zend_opcache_user_cache_shared_graph_should_skip_safe_direct_property(
									object,
									safe_direct_state_ptr,
									property_name,
									property_index,
									source_value
								)
							) {
								++property_index;
								continue;
							}

							if (!zend_opcache_user_cache_shared_graph_copy_string(context, property_name, &string_offset)) {
								result = false;

								break;
							}
							graph_properties[property_index].name_offset = string_offset;

							if (zend_opcache_user_cache_shared_graph_property_is_self_object_hash(
									object,
									property_index,
									source_value,
									&self_hash
								)
							) {
								graph_properties[property_index].reserved |=
									ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_PROPERTY_FLAG_SELF_OBJECT_HASH
								;
							}

							if (!zend_opcache_user_cache_shared_graph_copy_property_value(
									context,
									source_value,
									&graph_properties[property_index].value
								)
							) {
								result = false;

								break;
							}

							++property_index;
						} ZEND_HASH_FOREACH_END();

						if (self_hash != NULL) {
							zend_string_release(self_hash);
						}

						if (!result) {
							if (!Z_ISUNDEF(safe_direct_state)) {
								zval_ptr_dtor(&safe_direct_state);
							}

							return false;
						}
					}

					if (!Z_ISUNDEF(safe_direct_state)) {
						zval_ptr_dtor(&safe_direct_state);
					}

					destination->type = ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_SAFE_DIRECT_OBJECT;
					destination->payload.offset = safe_direct_offset;

					return true;
				}
			}

			if (!zend_opcache_user_cache_shared_graph_can_restore_direct(object->ce)) {
				return false;
			}

			context->prefers_prototype = true;

			seen_offset = zend_hash_index_find_ptr(&context->seen_objects, (zend_ulong) (uintptr_t) object);
			if (seen_offset != NULL) {
				/* Back-reference to an object already emitted. */
				shared_offset = (uint32_t) (uintptr_t) seen_offset;
				((zend_opcache_user_cache_shared_graph_object *) (context->buffer + shared_offset))->reserved |=
					ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_OBJECT_FLAG_SHARED
				;

				destination->type = ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_OBJECT_REF;
				destination->payload.offset = shared_offset;
				context->has_shared_identity = true;

				return true;
			}

			if (!zend_opcache_user_cache_shared_graph_copy_alloc(context, sizeof(*graph_object), &object_offset)) {
				return false;
			}

			/* Record the node offset before encoding properties so a cyclic or
			 * shared reference reached while encoding them resolves to a back-ref. */
			if (zend_hash_index_add_ptr(&context->seen_objects, (zend_ulong) (uintptr_t) object,
				(void *) (uintptr_t) object_offset) == NULL
			) {
				return false;
			}

			if (!zend_opcache_user_cache_shared_graph_copy_string(context, object->ce->name, &class_name_offset)) {
				return false;
			}

			properties = zend_get_properties_for((zval *) source, ZEND_PROP_PURPOSE_SERIALIZE);
			property_count = properties != NULL ? properties->nNumOfElements : 0;
			properties_offset = 0;
			if (property_count != 0 &&
				!zend_opcache_user_cache_shared_graph_copy_alloc(
					context,
					((size_t) property_count * sizeof(zend_opcache_user_cache_shared_graph_property)),
					&properties_offset
				)
			) {
				if (properties != NULL) {
					zend_release_properties(properties);
				}

				return false;
			}

			graph_object = (zend_opcache_user_cache_shared_graph_object *) (context->buffer + object_offset);
			graph_object->class_name_offset = class_name_offset;
			graph_object->property_count = property_count;
			graph_object->properties_offset = properties_offset;

			/* A later back-reference sets FLAG_SHARED. */
			graph_object->reserved = 0;

			if (property_count == 0) {
				if (properties != NULL) {
					zend_release_properties(properties);
				}

				destination->type = ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_OBJECT;
				destination->payload.offset = object_offset;

				return true;
			}

			graph_properties = (zend_opcache_user_cache_shared_graph_property *) (context->buffer + properties_offset);
			property_index = 0;

			ZEND_HASH_FOREACH_STR_KEY_VAL(properties, property_name, property_value) {
				if (property_name == NULL) {
					zend_release_properties(properties);

					return false;
				}

				graph_properties[property_index].name_offset = 0;
				graph_properties[property_index].reserved = 0;
				graph_properties[property_index].value.type = ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_UNDEF;

				source_value = Z_TYPE_P(property_value) == IS_INDIRECT
					? Z_INDIRECT_P(property_value)
					: property_value
				;

				if (zend_opcache_user_cache_shared_graph_property_is_default(
						object,
						property_index,
						source_value
					)
				) {
					++property_index;
					continue;
				}

				if (!zend_opcache_user_cache_shared_graph_copy_string(context, property_name, &graph_properties[property_index].name_offset)) {
					zend_release_properties(properties);

					return false;
				}

				if (!zend_opcache_user_cache_shared_graph_copy_property_value(
						context,
						source_value,
						&graph_properties[property_index].value
					)
				) {
					zend_release_properties(properties);

					return false;
				}

				++property_index;
			} ZEND_HASH_FOREACH_END();

			zend_release_properties(properties);

			destination->type = ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_OBJECT;
			destination->payload.offset = object_offset;

			return true;
		case IS_REFERENCE: {
			ref = Z_REF_P(source);
			seen_offset = zend_hash_index_find_ptr(&context->seen_references, (zend_ulong) (uintptr_t) ref);

			if (seen_offset != NULL) {
				/* Shared/cyclic reference already emitted: flag it, write a back-ref. */
				shared_offset = (uint32_t) (uintptr_t) seen_offset;
				((zend_opcache_user_cache_shared_graph_reference *) (context->buffer + shared_offset))->flags |=
					ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_OBJECT_FLAG_SHARED
				;

				destination->type = ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_REFERENCE_REF;
				destination->payload.offset = shared_offset;
				context->has_shared_identity = true;

				return true;
			}

			if (!zend_opcache_user_cache_shared_graph_copy_alloc(context,
				sizeof(zend_opcache_user_cache_shared_graph_reference), &reference_offset)
			) {
				return false;
			}

			/* Record the offset before encoding the inner value so a cycle through
			 * this reference resolves to a back-ref. */
			if (zend_hash_index_add_ptr(&context->seen_references, (zend_ulong) (uintptr_t) ref,
				(void *) (uintptr_t) reference_offset) == NULL
			) {
				return false;
			}

			graph_reference = (zend_opcache_user_cache_shared_graph_reference *) (context->buffer + reference_offset);
			graph_reference->flags = 0;
			graph_reference->reserved = 0;

			if (!zend_opcache_user_cache_shared_graph_copy_property_value(context, &ref->val, &graph_reference->inner)) {
				return false;
			}

			destination->type = ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_REFERENCE;
			destination->payload.offset = reference_offset;
			context->has_shared_identity = true;

			return true;
		}
		default:
			return false;
	}

array_done:
	zend_hash_index_del(&context->seen_arrays, array_key);

	return result;
}

static bool zend_opcache_user_cache_shared_graph_try_update_declared_object_property(
	zend_object *object,
	zend_string *property_name,
	zend_property_info *property_info,
	zval *property_value,
	bool *failed
)
{
	zval *slot, tmp, indirect;

	*failed = false;

	if (property_info == NULL ||
		property_info == ZEND_WRONG_PROPERTY_INFO ||
		!zend_string_equals(property_info->name, property_name) ||
		(property_info->flags & (ZEND_ACC_STATIC|ZEND_ACC_VIRTUAL)) != 0 ||
		property_info->offset == ZEND_VIRTUAL_PROPERTY_OFFSET
	) {
		return false;
	}

	slot = OBJ_PROP(object, property_info->offset);

	if (Z_ISREF_P(property_value)) {
		/* Preserve aliased properties by installing the reference itself. */
		if (ZEND_TYPE_IS_SET(property_info->type)) {
			if (!zend_verify_prop_assignable_by_ref(property_info, property_value, true)) {
				*failed = true;

				return false;
			}

			ZEND_REF_ADD_TYPE_SOURCE(Z_REF_P(property_value), property_info);
		}

		zval_ptr_dtor(slot);
		ZVAL_COPY(slot, property_value);

		if (object->properties != NULL) {
			ZVAL_INDIRECT(&indirect, slot);
			zend_hash_update(object->properties, property_name, &indirect);
		}

		return true;
	}

	/* Other property kinds use the standard update path. */
	if ((property_info->flags & (ZEND_ACC_READONLY|ZEND_ACC_PPP_SET_MASK)) != 0 ||
		(property_info->flags & ZEND_ACC_PPP_MASK) != ZEND_ACC_PUBLIC
	) {
		return false;
	}

	ZVAL_COPY_DEREF(&tmp, property_value);

	if (ZEND_TYPE_IS_SET(property_info->type) &&
		!zend_verify_property_type(property_info, &tmp, true)
	) {
		zval_ptr_dtor(&tmp);

		*failed = true;

		return false;
	}

	zval_ptr_dtor(slot);
	ZVAL_COPY_VALUE(slot, &tmp);

	if (object->properties != NULL) {
		ZVAL_INDIRECT(&indirect, slot);
		zend_hash_update(object->properties, property_name, &indirect);
	}

	return true;
}

static bool zend_opcache_user_cache_shared_graph_update_object_property(
	zval *object_zv,
	zend_string *property_name,
	zval *property_value
)
{
	const char *class_name, *prop_name;
	zend_string *cname;
	zend_class_entry *scope;
	zend_object *object;
	zend_property_info *property_info;
	HashTable *properties;
	size_t prop_name_len;
	bool failed;

	object = Z_OBJ_P(object_zv);
	if (ZSTR_LEN(property_name) != 0 && ZSTR_VAL(property_name)[0] != '\0') {
		property_info = zend_get_property_info(object->ce, property_name, true);
		if (zend_opcache_user_cache_shared_graph_try_update_declared_object_property(
				object,
				property_name,
				property_info,
				property_value,
				&failed)
		) {
			return true;
		}

		if (failed) {
			return false;
		}

		/* Preserve aliased dynamic properties. */
		if (Z_ISREF_P(property_value)) {
			properties = zend_std_get_properties(object);

			if (properties != NULL) {
				Z_TRY_ADDREF_P(property_value);

				zend_hash_update(properties, property_name, property_value);

				return true;
			}
		}

		zend_update_property(object->ce, object, ZSTR_VAL(property_name), ZSTR_LEN(property_name), property_value);

		return !EG(exception);
	}

	/* Private/protected property name. */
	if (zend_unmangle_property_name_ex(property_name, &class_name, &prop_name, &prop_name_len) == SUCCESS) {
		if (class_name[0] != '*') {
			cname = zend_string_init(class_name, strlen(class_name), 0);
			scope = zend_lookup_class(cname);

			if (scope != NULL) {
				zend_update_property(scope, object, prop_name, prop_name_len, property_value);
			}

			zend_string_release_ex(cname, 0);
		} else {
			zend_update_property(object->ce, object, prop_name, prop_name_len, property_value);
		}
	}

	return !EG(exception);
}

static bool zend_opcache_user_cache_shared_graph_update_object_property_at(
		zval *object_zv,
		zend_string *property_name,
		uint32_t property_index,
		zval *property_value
)
{
	zend_object *object;
	zend_property_info *property_info;
	bool failed;

	object = Z_OBJ_P(object_zv);
	if (ZSTR_LEN(property_name) != 0 && ZSTR_VAL(property_name)[0] != '\0' &&
		object->ce->type == ZEND_USER_CLASS &&
		object->ce->properties_info_table != NULL &&
		property_index < object->ce->default_properties_count
	) {
		property_info = object->ce->properties_info_table[property_index];

		if (zend_opcache_user_cache_shared_graph_try_update_declared_object_property(
				object,
				property_name,
				property_info,
				property_value,
				&failed
			)
		) {
			return true;
		}

		if (failed) {
			return false;
		}
	}

	return zend_opcache_user_cache_shared_graph_update_object_property(object_zv, property_name, property_value);
}

static bool zend_opcache_user_cache_fetch_shared_graph_value(
	const unsigned char *buffer,
	const zend_opcache_user_cache_shared_graph_value *value,
	zval *destination
)
{
	const zend_opcache_user_cache_shared_graph_object *graph_object;
	const zend_opcache_user_cache_shared_graph_array *graph_array;
	const zend_opcache_user_cache_shared_graph_array_element *graph_elements, *graph_element;
	const zend_opcache_user_cache_shared_graph_property *properties, *property;
	const zend_opcache_user_cache_shared_graph_safe_direct_object *graph_safe_direct;
	const zend_opcache_user_cache_shared_graph_enum *graph_enum;
	const zend_opcache_user_cache_shared_graph_reference *graph_reference;
	zend_class_entry *ce, *enum_ce;
	zend_string *class_name, *property_name, *enum_class_name, *enum_case_name;
	zend_reference *ref, *shared_reference;
	zend_array *shared_array;
	zend_object *shared_object, *case_obj;
	zval property_value, safe_direct_state;
	uint32_t index;

	switch (value->type) {
		case ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_UNDEF:
			ZVAL_UNDEF(destination);
			return true;
		case ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_NULL:
			ZVAL_NULL(destination);
			return true;
		case ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_TRUE:
			ZVAL_TRUE(destination);
			return true;
		case ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_FALSE:
			ZVAL_FALSE(destination);
			return true;
		case ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_LONG:
			ZVAL_LONG(destination, value->payload.long_value);
			return true;
		case ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_DOUBLE:
			ZVAL_DOUBLE(destination, value->payload.double_value);
			return true;
		case ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_STRING:
			ZVAL_INTERNED_STR(destination, (zend_string *) (void *) (buffer + (uint32_t) value->payload.offset));
			return true;
		case ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_ARRAY:
			if ((uint32_t) value->payload.offset == 0) {
				ZVAL_EMPTY_ARRAY(destination);
			} else {
				ZVAL_ARR(destination, (zend_array *) (void *) (buffer + (uint32_t) value->payload.offset));
				Z_TYPE_FLAGS_P(destination) = 0;
			}

			return true;
		case ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_DYNAMIC_ARRAY:
			graph_array = (const zend_opcache_user_cache_shared_graph_array *) (buffer + (uint32_t) value->payload.offset);
			array_init_size(destination, graph_array->count);

			/* Register before filling to resolve cycles. */
			if (graph_array->reserved & ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_OBJECT_FLAG_SHARED) {
				if (!zend_opcache_user_cache_decode_array_map_insert((uint32_t) value->payload.offset, Z_ARRVAL_P(destination))) {
					zval_ptr_dtor(destination);

					ZVAL_UNDEF(destination);

					return false;
				}

				HT_ALLOW_COW_VIOLATION(Z_ARRVAL_P(destination));
			}

			graph_elements = (const zend_opcache_user_cache_shared_graph_array_element *) (buffer + graph_array->elements_offset);

			for (index = 0; index < graph_array->count; index++) {
				graph_element = &graph_elements[index];

				ZVAL_UNDEF(&property_value);

				if (!zend_opcache_user_cache_fetch_shared_graph_value(buffer, &graph_element->value, &property_value)) {
					zval_ptr_dtor(destination);

					ZVAL_UNDEF(destination);

					return false;
				}

				if (graph_element->key_offset != 0) {
					property_name = (zend_string *) (void *) (buffer + graph_element->key_offset);

					HT_ALLOW_COW_VIOLATION(Z_ARRVAL_P(destination));

					if (zend_hash_add_new(Z_ARRVAL_P(destination), property_name, &property_value) == NULL) {
						zval_ptr_dtor(&property_value);
						zval_ptr_dtor(destination);

						ZVAL_UNDEF(destination);

						return false;
					}
				} else {
					HT_ALLOW_COW_VIOLATION(Z_ARRVAL_P(destination));

					if (zend_hash_index_add_new(Z_ARRVAL_P(destination), graph_element->h, &property_value) == NULL) {
						zval_ptr_dtor(&property_value);
						zval_ptr_dtor(destination);

						ZVAL_UNDEF(destination);

						return false;
					}
				}
			}

			Z_ARRVAL_P(destination)->nNextFreeElement = graph_array->next_free;
#if ZEND_DEBUG
			HT_FLAGS(Z_ARRVAL_P(destination)) &= ~HASH_FLAG_ALLOW_COW_VIOLATION;
#endif
			return true;
		case ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_ARRAY_REF: {
			shared_array = zend_opcache_user_cache_decode_array_map_find((uint32_t) value->payload.offset);
			if (shared_array == NULL) {
				return false;
			}

			ZVAL_ARR(destination, shared_array);
			GC_ADDREF(shared_array);

			return true;
		}
		case ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_OBJECT:
			graph_object = (const zend_opcache_user_cache_shared_graph_object *) (buffer + (uint32_t) value->payload.offset);
			class_name = (zend_string *) (void *) (buffer + graph_object->class_name_offset);
			ce = zend_opcache_user_cache_decode_lookup_class(class_name);

			if (ce == NULL || object_init_ex(destination, ce) != SUCCESS) {
				return false;
			}

			/* Register before decoding properties to resolve back-references. */
			if ((graph_object->reserved & ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_OBJECT_FLAG_SHARED) &&
				!zend_opcache_user_cache_decode_identity_map_insert((uint32_t) value->payload.offset, Z_OBJ_P(destination))
			) {
				zval_ptr_dtor(destination);

				ZVAL_UNDEF(destination);

				return false;
			}

			if (graph_object->property_count == 0) {
				return true;
			}

			properties = (const zend_opcache_user_cache_shared_graph_property *) (buffer + graph_object->properties_offset);
			for (index = 0; index < graph_object->property_count; index++) {
				property = &properties[index];
				if (property->value.type == ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_UNDEF) {
					continue;
				}

				property_name = (zend_string *) (void *) (buffer + property->name_offset);

				ZVAL_UNDEF(&property_value);

				if (!zend_opcache_user_cache_fetch_shared_graph_value(buffer, &property->value, &property_value) ||
					!zend_opcache_user_cache_shared_graph_update_object_property_at(destination, property_name, index, &property_value)
				) {
					zval_ptr_dtor(&property_value);
					zval_ptr_dtor(destination);

					ZVAL_UNDEF(destination);

					return false;
				}

				zval_ptr_dtor(&property_value);
			}

			return true;
		case ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_OBJECT_REF: {
			shared_object = zend_opcache_user_cache_decode_identity_map_find((uint32_t) value->payload.offset);
			if (shared_object == NULL) {
				return false;
			}

			ZVAL_OBJ(destination, shared_object);
			GC_ADDREF(shared_object);

			return true;
		}
		case ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_SAFE_DIRECT_OBJECT:
			graph_safe_direct = (const zend_opcache_user_cache_shared_graph_safe_direct_object *) (buffer + (uint32_t) value->payload.offset);

			class_name = (zend_string *) (void *) (buffer + graph_safe_direct->class_name_offset);
			ce = zend_opcache_user_cache_decode_lookup_class(class_name);
			if (ce == NULL ||
				zend_opcache_user_cache_safe_direct_state_unserialize_func(ce) == NULL ||
				object_init_ex(destination, ce) != SUCCESS
			) {
				return false;
			}

			if ((graph_safe_direct->reserved & ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_OBJECT_FLAG_SHARED) &&
				!zend_opcache_user_cache_decode_identity_map_insert((uint32_t) value->payload.offset, Z_OBJ_P(destination))
			) {
				zval_ptr_dtor(destination);

				ZVAL_UNDEF(destination);

				return false;
			}

			ZVAL_UNDEF(&safe_direct_state);
			if (!zend_opcache_user_cache_fetch_shared_graph_value(buffer, &graph_safe_direct->state, &safe_direct_state) ||
				Z_TYPE(safe_direct_state) != IS_ARRAY ||
				!zend_opcache_user_cache_safe_direct_state_unserialize_func(ce)(destination, &safe_direct_state)
			) {
				zval_ptr_dtor(&safe_direct_state);
				zval_ptr_dtor(destination);

				ZVAL_UNDEF(destination);

				return false;
			}
			zval_ptr_dtor(&safe_direct_state);

			if (graph_safe_direct->property_count != 0) {
				properties = (const zend_opcache_user_cache_shared_graph_property *) (buffer + graph_safe_direct->properties_offset);
				for (index = 0; index < graph_safe_direct->property_count; index++) {
					property = &properties[index];
					if (property->value.type == ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_UNDEF) {
						continue;
					}

					property_name = (zend_string *) (void *) (buffer + property->name_offset);
					ZVAL_UNDEF(&property_value);

					if (property->reserved & ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_PROPERTY_FLAG_SELF_OBJECT_HASH) {
						ZVAL_STR(&property_value, php_spl_object_hash(Z_OBJ_P(destination)));
					} else if (!zend_opcache_user_cache_fetch_shared_graph_value(buffer, &property->value, &property_value)) {
						zval_ptr_dtor(destination);

						ZVAL_UNDEF(destination);

						return false;
					}

					if (!zend_opcache_user_cache_shared_graph_update_object_property_at(destination, property_name, index, &property_value)) {
						zval_ptr_dtor(&property_value);
						zval_ptr_dtor(destination);

						ZVAL_UNDEF(destination);

						return false;
					}

					zval_ptr_dtor(&property_value);
				}
			}

			return true;
		case ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_ENUM: {
			graph_enum = (const zend_opcache_user_cache_shared_graph_enum *) (buffer + (uint32_t) value->payload.offset);

			/* Cache by pinned enum node address. */
			case_obj = zend_opcache_user_cache_decode_resolve_cache_find(graph_enum);
			if (case_obj == NULL) {
				enum_class_name = (zend_string *) (void *) (buffer + graph_enum->class_name_offset);
				enum_case_name = (zend_string *) (void *) (buffer + graph_enum->case_name_offset);
				enum_ce = zend_opcache_user_cache_decode_lookup_class(enum_class_name);

				if (enum_ce == NULL || !(enum_ce->ce_flags & ZEND_ACC_ENUM)) {
					return false;
				}

				case_obj = zend_enum_get_case(enum_ce, enum_case_name);
				if (case_obj == NULL) {
					return false;
				}

				zend_opcache_user_cache_decode_resolve_cache_store(graph_enum, case_obj);
			}

			ZVAL_OBJ(destination, case_obj);
			GC_ADDREF(case_obj);

			return true;
		}
		case ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_REFERENCE: {
			graph_reference = (const zend_opcache_user_cache_shared_graph_reference *) (buffer + (uint32_t) value->payload.offset);

			ZVAL_NEW_EMPTY_REF(destination);
			ref = Z_REF_P(destination);

			ZVAL_UNDEF(&ref->val);

			/* Register before decoding the inner value so a cycle or sibling that
			 * aliases this reference resolves to the same zend_reference. Only
			 * shared/cyclic references carry back-refs, so others need no entry. */
			if ((graph_reference->flags & ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_OBJECT_FLAG_SHARED) &&
				!zend_opcache_user_cache_decode_reference_map_insert((uint32_t) value->payload.offset, ref)
			) {
				zval_ptr_dtor(destination);

				ZVAL_UNDEF(destination);

				return false;
			}

			if (!zend_opcache_user_cache_fetch_shared_graph_value(buffer, &graph_reference->inner, &ref->val)) {
				zval_ptr_dtor(destination);

				ZVAL_UNDEF(destination);

				return false;
			}

			return true;
		}
		case ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_REFERENCE_REF: {
			shared_reference = zend_opcache_user_cache_decode_reference_map_find((uint32_t) value->payload.offset);

			if (shared_reference == NULL) {
				return false;
			}

			ZVAL_REF(destination, shared_reference);
			GC_ADDREF(shared_reference);

			return true;
		}
		default:
			return false;
	}
}

static zend_opcache_user_cache_shared_graph_header *zend_opcache_user_cache_shared_graph_payload_header(uint32_t payload_offset)
{
	const unsigned char *graph_buffer;
	zend_opcache_user_cache_shared_graph_header *header;
	size_t buffer_len;

	if (payload_offset == 0) {
		return NULL;
	}

	buffer_len = zend_opcache_user_cache_block_payload_capacity(payload_offset);
	if (buffer_len == 0) {
		return NULL;
	}

	graph_buffer = zend_opcache_user_cache_shared_graph_locate_buffer(
		(const unsigned char *) zend_opcache_user_cache_ptr(payload_offset),
		buffer_len,
		NULL
	);
	if (graph_buffer == NULL) {
		return NULL;
	}

	header = (zend_opcache_user_cache_shared_graph_header *) graph_buffer;
	if (header->magic != ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_MAGIC ||
		header->version != ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VERSION
	) {
		return NULL;
	}

	return header;
}

/* Caller holds the context write lock with an initialized header. */
static bool zend_opcache_user_cache_free_retired_for_context_locked(zend_opcache_user_cache_context *context)
{
	zend_opcache_user_cache_shared_graph_ref *ref;
	uint32_t index;
	bool freed = false;
	bool graph_quiescent = zend_opcache_user_cache_graph_payloads_quiescent_locked();

	for (index = 0; index < zend_opcache_user_cache_retired_shared_graph_count; index++) {
		ref = &zend_opcache_user_cache_retired_shared_graphs[index];

		if (ref->context != context) {
			continue;
		}

		if (graph_quiescent) {
			zend_opcache_user_cache_free_locked(ref->payload_offset);
			freed = true;
		} else {
			zend_opcache_user_cache_orphan_graph_payload_locked(ref->payload_offset);
		}

		ref->context = NULL;
	}

	zend_opcache_user_cache_reclaim_orphaned_graphs_locked();

	return freed;
}

static bool zend_opcache_user_cache_free_retired_shared_graphs(void)
{
	zend_opcache_user_cache_context *context, *previous_context;
	uint32_t index;
	bool freed = false;

	if (zend_opcache_user_cache_retired_shared_graph_count == 0) {
		return false;
	}

	/* Group pending frees by context. */
	for (index = 0; index < zend_opcache_user_cache_retired_shared_graph_count; index++) {
		context = zend_opcache_user_cache_retired_shared_graphs[index].context;

		if (context == NULL) {
			continue;
		}

		previous_context = zend_opcache_user_cache_activate_context(context);

		if (zend_opcache_user_cache_wlock()) {
			if (zend_opcache_user_cache_header_is_initialized_locked()) {
				freed = zend_opcache_user_cache_free_retired_for_context_locked(context) || freed;
			}

			zend_opcache_user_cache_unlock();
		}

		zend_opcache_user_cache_restore_context(previous_context);
	}

	efree(zend_opcache_user_cache_retired_shared_graphs);

	zend_opcache_user_cache_retired_shared_graphs = NULL;
	zend_opcache_user_cache_retired_shared_graph_count = 0;
	zend_opcache_user_cache_retired_shared_graph_capacity = 0;

	return freed;
}

static bool zend_opcache_user_cache_shared_graph_rebase_direct_zval(
		zval *value,
		const unsigned char *old_base,
		const unsigned char *new_base,
		size_t len,
		ptrdiff_t delta,
		HashTable *seen_arrays)
{
	zend_array *array;

	switch (Z_TYPE_P(value)) {
		case IS_STRING:
			Z_STR_P(value) = (zend_string *) zend_opcache_user_cache_shared_graph_rebase_pointer(
				Z_STR_P(value),
				old_base,
				len,
				delta
			);
			return true;
		case IS_ARRAY:
			array = (zend_array *) zend_opcache_user_cache_shared_graph_rebase_pointer(
				Z_ARR_P(value),
				old_base,
				len,
				delta
			);

			Z_ARR_P(value) = array;

			if (!zend_opcache_user_cache_shared_graph_pointer_in_range(array, new_base, len)) {
				return true;
			}

			return zend_opcache_user_cache_shared_graph_rebase_direct_array(
				array,
				old_base,
				new_base,
				len,
				delta,
				seen_arrays
			);
		default:
			return true;
	}
}

static bool zend_opcache_user_cache_shared_graph_rebase_direct_array(
		zend_array *array,
		const unsigned char *old_base,
		const unsigned char *new_base,
		size_t len,
		ptrdiff_t delta,
		HashTable *seen_arrays)
{
	zend_ulong key;
	zval *packed;
	Bucket *bucket;
	uint32_t index;
	void *data;

	if (!zend_opcache_user_cache_shared_graph_pointer_in_range(array, new_base, len)) {
		return true;
	}

	key = (zend_ulong) (uintptr_t) array;
	if (zend_hash_index_exists(seen_arrays, key)) {
		return true;
	}

	if (zend_hash_index_add_empty_element(seen_arrays, key) == NULL) {
		return false;
	}

	data = HT_GET_DATA_ADDR(array);
	data = zend_opcache_user_cache_shared_graph_rebase_pointer(data, old_base, len, delta);

	HT_SET_DATA_ADDR(array, data);

	if (!zend_opcache_user_cache_shared_graph_pointer_in_range(data, new_base, len)) {
		return false;
	}

	if (HT_IS_PACKED(array)) {
		packed = array->arPacked;
		for (index = 0; index < array->nNumUsed; index++) {
			if (!zend_opcache_user_cache_shared_graph_rebase_direct_zval(
					&packed[index],
					old_base,
					new_base,
					len,
					delta,
					seen_arrays
				)
			) {
				return false;
			}
		}
	} else {
		bucket = array->arData;
		for (index = 0; index < array->nNumUsed; index++) {
			if (bucket[index].key != NULL) {
				bucket[index].key = (zend_string *) zend_opcache_user_cache_shared_graph_rebase_pointer(
					bucket[index].key,
					old_base,
					len,
					delta
				);

				if (!zend_opcache_user_cache_shared_graph_pointer_in_range(bucket[index].key, new_base, len)) {
					return false;
				}
			}

			if (!zend_opcache_user_cache_shared_graph_rebase_direct_zval(
					&bucket[index].val,
					old_base,
					new_base,
					len,
					delta,
					seen_arrays
				)
			) {
				return false;
			}
		}
	}

	return true;
}

static bool zend_opcache_user_cache_shared_graph_rebase_graph_value(
		const unsigned char *buffer,
		const zend_opcache_user_cache_shared_graph_value *value,
		const unsigned char *old_base,
		const unsigned char *new_base,
		size_t len,
		ptrdiff_t delta,
		HashTable *seen_arrays)
{
	const zend_opcache_user_cache_shared_graph_array *graph_array;
	const zend_opcache_user_cache_shared_graph_array_element *graph_elements;
	const zend_opcache_user_cache_shared_graph_object *graph_object;
	const zend_opcache_user_cache_shared_graph_property *properties;
	const zend_opcache_user_cache_shared_graph_safe_direct_object *graph_safe_direct;
	zend_array *array;
	uint32_t index;

	switch (value->type) {
		case ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_ARRAY:
			if ((uint32_t) value->payload.offset == 0) {
				return true;
			}

			array = (zend_array *) (void *) (buffer + (uint32_t) value->payload.offset);

			return zend_opcache_user_cache_shared_graph_rebase_direct_array(
				array,
				old_base,
				new_base,
				len,
				delta,
				seen_arrays
			);
		case ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_DYNAMIC_ARRAY:
			graph_array = (const zend_opcache_user_cache_shared_graph_array *) (buffer + (uint32_t) value->payload.offset);
			graph_elements = (const zend_opcache_user_cache_shared_graph_array_element *) (buffer + graph_array->elements_offset);

			for (index = 0; index < graph_array->count; index++) {
				if (!zend_opcache_user_cache_shared_graph_rebase_graph_value(
						buffer,
						&graph_elements[index].value,
						old_base,
						new_base,
						len,
						delta,
						seen_arrays
					)
				) {
					return false;
				}
			}
			return true;
		case ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_OBJECT:
			graph_object = (const zend_opcache_user_cache_shared_graph_object *) (buffer + (uint32_t) value->payload.offset);
			properties = (const zend_opcache_user_cache_shared_graph_property *) (buffer + graph_object->properties_offset);
			for (index = 0; index < graph_object->property_count; index++) {
				if (!zend_opcache_user_cache_shared_graph_rebase_graph_value(
						buffer,
						&properties[index].value,
						old_base,
						new_base,
						len,
						delta,
						seen_arrays
					)
				) {
					return false;
				}
			}

			return true;
		case ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_REFERENCE:
			/* References can hold direct arrays with in-buffer pointers. */
			return zend_opcache_user_cache_shared_graph_rebase_graph_value(
				buffer,
				&((const zend_opcache_user_cache_shared_graph_reference *) (buffer + (uint32_t) value->payload.offset))->inner,
				old_base,
				new_base,
				len,
				delta,
				seen_arrays
			);
		case ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_SAFE_DIRECT_OBJECT: {
			 graph_safe_direct =
				(const zend_opcache_user_cache_shared_graph_safe_direct_object *) (buffer + (uint32_t) value->payload.offset)
			;

			if (!zend_opcache_user_cache_shared_graph_rebase_graph_value(
					buffer,
					&graph_safe_direct->state,
					old_base,
					new_base,
					len,
					delta,
					seen_arrays
				)
			) {
				return false;
			}

			properties = (const zend_opcache_user_cache_shared_graph_property *) (buffer + graph_safe_direct->properties_offset);
			for (index = 0; index < graph_safe_direct->property_count; index++) {
				if (!zend_opcache_user_cache_shared_graph_rebase_graph_value(
						buffer,
						&properties[index].value,
						old_base,
						new_base,
						len,
						delta,
						seen_arrays
					)
				) {
					return false;
				}
			}

			return true;
		}
		default:
			return true;
	}
}

/* Prevent new pins after a reader-drain timeout. */
static void zend_opcache_user_cache_shared_graph_force_retire_locked(uint32_t payload_offset)
{
	zend_opcache_user_cache_shared_graph_header *header = zend_opcache_user_cache_shared_graph_payload_header(payload_offset);
	int state, expected;

	if (header == NULL) {
		return;
	}

	for (;;) {
		state = zend_atomic_int_load_ex(&header->ref_state);

		if ((state & ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_RETIRED) != 0) {
			return;
		}

		expected = state;
		if (zend_atomic_int_compare_exchange_ex(&header->ref_state, &expected, state | ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_RETIRED)) {
			return;
		}
	}
}

static bool zend_opcache_user_cache_orphaned_graph_block_is_referenced_locked(
	zend_opcache_user_cache_header *header,
	uint32_t block_offset,
	uint32_t block_size
)
{
	zend_opcache_user_cache_entry *entries, *entry;
	uint32_t index;

	entries = zend_opcache_user_cache_entries(header);
	for (index = 0; index < header->capacity; index++) {
		entry = &entries[index];
		if (entry->state != ZEND_OPCACHE_USER_CACHE_ENTRY_USED) {
			continue;
		}

		if (entry->key_offset != 0 &&
			zend_opcache_user_cache_shared_graph_offset_in_block(entry->key_offset, block_offset, block_size)
		) {
			return true;
		}

		if (entry->value_offset != 0 &&
			zend_opcache_user_cache_shared_graph_offset_in_block(entry->value_offset, block_offset, block_size)
		) {
			return true;
		}
	}

	return false;
}

static bool zend_opcache_user_cache_reclaim_saturated_orphaned_graphs_locked(zend_opcache_user_cache_header *header)
{
	zend_opcache_user_cache_block *block;
	zend_opcache_user_cache_shared_graph_header *graph_header;
	uint32_t used_end, offset, block_size, payload_offset;
	bool reclaimed = false, restart;

	do {
		restart = false;
		used_end = header->data_offset + header->next_free;
		offset = header->data_offset;

		while (offset < used_end) {
			block = zend_opcache_user_cache_block_ptr(offset);
			block_size = block->size;

			if (block_size < ZEND_ALIGNED_SIZE(sizeof(zend_opcache_user_cache_block) + 1) ||
				block_size > used_end - offset
			) {
				return reclaimed;
			}

			if (!zend_opcache_user_cache_block_is_free(block) &&
				!zend_opcache_user_cache_orphaned_graph_block_is_referenced_locked(header, offset, block_size)
			) {
				payload_offset = offset + (uint32_t) sizeof(zend_opcache_user_cache_block);
				graph_header = zend_opcache_user_cache_shared_graph_payload_header(payload_offset);

				if (graph_header != NULL &&
					zend_atomic_int_load_ex(&graph_header->ref_state) == (int) ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_RETIRED
				) {
					zend_opcache_user_cache_free_locked(payload_offset);
					reclaimed = true;
					restart = true;

					break;
				}
			}

			offset += block_size;
		}
	} while (restart);

	return reclaimed;
}

void zend_opcache_user_cache_decode_resolve_cache_release(void)
{
	if (zend_opcache_user_cache_decode_resolve_cache == NULL) {
		return;
	}

	zend_hash_destroy(zend_opcache_user_cache_decode_resolve_cache);
	efree(zend_opcache_user_cache_decode_resolve_cache);

	zend_opcache_user_cache_decode_resolve_cache = NULL;
}

bool zend_opcache_user_cache_shared_graph_requires_prototype(uint32_t payload_offset)
{
	zend_opcache_user_cache_shared_graph_header *header =
		zend_opcache_user_cache_shared_graph_payload_header(payload_offset)
	;

	if (header == NULL) {
		return true;
	}

	return (header->flags & ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_FLAG_PREFERS_PROTOTYPE) != 0;
}

bool zend_opcache_user_cache_shared_graph_payload_has_aliases(uint32_t payload_offset)
{
	zend_opcache_user_cache_shared_graph_header *header =
		zend_opcache_user_cache_shared_graph_payload_header(payload_offset)
	;

	if (header == NULL) {
		return true;
	}

	return (header->flags & ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_FLAG_HAS_SHARED_IDENTITY) != 0;
}

bool zend_opcache_user_cache_shared_graph_decode_is_lock_safe(uint32_t payload_offset)
{
	zend_opcache_user_cache_shared_graph_header *header =
		zend_opcache_user_cache_shared_graph_payload_header(payload_offset)
	;

	return header != NULL &&
		(header->flags & ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_FLAG_HAS_OBJECT) == 0
	;
}

bool zend_opcache_user_cache_calculate_shared_graph_size(
	const zval *value,
	size_t *buffer_len,
	HashTable *state_memo
)
{
	zend_opcache_user_cache_shared_graph_calc_context calc_context;
	zend_class_entry *root_ce;
	bool result;

	if (!buffer_len) {
		return false;
	}

	*buffer_len = 0;

	if (Z_TYPE_P(value) == IS_OBJECT) {
		root_ce = Z_OBJCE_P(value);
		/* A top-level enum case becomes an ENUM root; otherwise only a directly
		 * restorable object is a valid graph root. */
		if (!zend_opcache_user_cache_shared_graph_can_restore_direct(root_ce) &&
			!zend_opcache_user_cache_shared_graph_can_use_safe_direct(root_ce) &&
			!(root_ce->ce_flags & ZEND_ACC_ENUM)
		) {
			return false;
		}
	} else if (Z_TYPE_P(value) != IS_ARRAY && Z_TYPE_P(value) != IS_STRING) {
		return false;
	}

	zend_opcache_user_cache_shared_graph_calc_init(&calc_context);

	calc_context.state_memo = state_memo;
	result = zend_opcache_user_cache_shared_graph_reserve_size(&calc_context.size, sizeof(zend_opcache_user_cache_shared_graph_header));

	if (result) {
		result = zend_opcache_user_cache_shared_graph_calc_value(&calc_context, value);
	}

	if (result) {
		if (calc_context.size > UINT32_MAX - (ZEND_MM_ALIGNMENT - 1)) {
			result = false;
		} else {
			calc_context.size += ZEND_MM_ALIGNMENT - 1;
		}
	}
	if (result) {
		*buffer_len = calc_context.size;
	}

	zend_opcache_user_cache_shared_graph_calc_destroy(&calc_context);

	return result;
}

bool zend_opcache_user_cache_build_shared_graph_in_place(
	const zval *value,
	unsigned char *buffer,
	size_t buffer_len,
	size_t *graph_len,
	HashTable *state_memo
)
{
	zend_opcache_user_cache_shared_graph_copy_context copy_context;
	zend_opcache_user_cache_shared_graph_header *header;
	zend_opcache_user_cache_shared_graph_value root_value;
	uint32_t header_offset, root_offset, root_type;
	size_t padding;
	bool result;

	if (buffer == NULL) {
		return false;
	}

	padding = zend_opcache_user_cache_shared_graph_alignment_padding(buffer);
	if (padding > buffer_len || buffer_len - padding < sizeof(zend_opcache_user_cache_shared_graph_header)) {
		return false;
	}

	if (padding != 0) {
		memset(buffer, 0, padding);
	}

	buffer += padding;
	buffer_len -= padding;

	zend_opcache_user_cache_shared_graph_copy_init(&copy_context, buffer, buffer_len);

	copy_context.state_memo = state_memo;
	root_offset = 0;
	root_type = 0;

	result = zend_opcache_user_cache_shared_graph_copy_alloc(&copy_context, sizeof(*header), &header_offset) && header_offset == 0;
	if (result) {
		if (Z_TYPE_P(value) == IS_OBJECT) {
			result = zend_opcache_user_cache_shared_graph_copy_property_value(&copy_context, value, &root_value) &&
					(root_value.type == ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_OBJECT ||
					root_value.type == ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_SAFE_DIRECT_OBJECT ||
					root_value.type == ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_ENUM)
			;
		} else if (Z_TYPE_P(value) == IS_ARRAY) {
			result = zend_opcache_user_cache_shared_graph_copy_property_value(&copy_context, value, &root_value) &&
					(root_value.type == ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_ARRAY ||
					root_value.type == ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_DYNAMIC_ARRAY)
			;
		} else if (Z_TYPE_P(value) == IS_STRING) {
			result = zend_opcache_user_cache_shared_graph_copy_property_value(&copy_context, value, &root_value) &&
					root_value.type == ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_STRING
			;
		} else {
			result = false;
		}

		if (result) {
			root_type = root_value.type;
			root_offset = (uint32_t) root_value.payload.offset;
		}
	}

	if (!result) {
		zend_opcache_user_cache_shared_graph_copy_destroy(&copy_context);

		return false;
	}

	header = (zend_opcache_user_cache_shared_graph_header *) buffer;
	header->magic = ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_MAGIC;
	header->version = ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VERSION;
	header->root_offset = root_offset;
	header->root_type = root_type;
	header->flags =
		(copy_context.has_shared_identity ? ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_FLAG_HAS_SHARED_IDENTITY : 0) |
		(copy_context.has_object ? ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_FLAG_HAS_OBJECT : 0) |
		(copy_context.prefers_prototype ? ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_FLAG_PREFERS_PROTOTYPE : 0)
	;

	ZEND_ATOMIC_INT_INIT(&header->ref_state, 0);

	if (graph_len != NULL) {
		*graph_len = copy_context.position;
	}

	zend_opcache_user_cache_shared_graph_copy_destroy(&copy_context);

	return true;
}

bool zend_opcache_user_cache_shared_graph_copy_fits_buffer(
	const unsigned char *source_buffer,
	size_t source_buffer_len,
	size_t source_graph_len,
	const unsigned char *target_buffer,
	size_t target_buffer_len
)
{
	size_t target_padding;

	if (source_buffer == NULL || target_buffer == NULL || source_graph_len == 0 || source_graph_len > source_buffer_len) {
		return false;
	}

	target_padding = zend_opcache_user_cache_shared_graph_alignment_padding(target_buffer);

	return target_padding <= target_buffer_len && source_graph_len <= target_buffer_len - target_padding;
}

bool zend_opcache_user_cache_fetch_shared_graph(
	const unsigned char *buffer,
	size_t buffer_len,
	zval *destination
)
{
	const zend_opcache_user_cache_shared_graph_header *header;
	const unsigned char *graph_buffer;
	zend_opcache_user_cache_shared_graph_value root_value;
	HashTable *saved_identity_map, *saved_reference_map, *saved_array_map;
	uint32_t root_type;
	bool result;

	graph_buffer = zend_opcache_user_cache_shared_graph_locate_buffer(buffer, buffer_len, &buffer_len);
	if (graph_buffer == NULL) {
		return false;
	}

	buffer = graph_buffer;

	header = (const zend_opcache_user_cache_shared_graph_header *) buffer;
	if (header->magic != ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_MAGIC ||
		header->version != ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VERSION ||
		(header->root_offset != 0 && header->root_offset >= buffer_len)
	) {
		return false;
	}

	/* Decoding can re-enter through autoload. */
	saved_identity_map = zend_opcache_user_cache_decode_identity_map;
	zend_opcache_user_cache_decode_identity_map = NULL;
	saved_reference_map = zend_opcache_user_cache_decode_reference_map;
	zend_opcache_user_cache_decode_reference_map = NULL;
	saved_array_map = zend_opcache_user_cache_decode_array_map;
	zend_opcache_user_cache_decode_array_map = NULL;

	root_type = header->root_type != 0 ? header->root_type : ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_OBJECT;

	memset(&root_value, 0, sizeof(root_value));

	root_value.type = (uint8_t) root_type;
	root_value.payload.offset = header->root_offset;

	switch (root_type) {
		case ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_SAFE_DIRECT_OBJECT:
		case ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_OBJECT:
			if (header->root_offset == 0) {
				zend_opcache_user_cache_decode_identity_map = saved_identity_map;
				zend_opcache_user_cache_decode_reference_map = saved_reference_map;
				zend_opcache_user_cache_decode_array_map = saved_array_map;

				return false;
			}

			result = zend_opcache_user_cache_fetch_shared_graph_value(buffer, &root_value, destination);

			break;
		case ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_ARRAY:
		case ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_DYNAMIC_ARRAY:
		case ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_ENUM:
		case ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_STRING:
			result = zend_opcache_user_cache_fetch_shared_graph_value(buffer, &root_value, destination);

			break;
		default:
			zend_opcache_user_cache_decode_identity_map = saved_identity_map;
			zend_opcache_user_cache_decode_reference_map = saved_reference_map;
			zend_opcache_user_cache_decode_array_map = saved_array_map;

			return false;
	}

	zend_opcache_user_cache_decode_identity_map_teardown();
	zend_opcache_user_cache_decode_reference_map_teardown();
	zend_opcache_user_cache_decode_array_map_teardown();

	zend_opcache_user_cache_decode_identity_map = saved_identity_map;
	zend_opcache_user_cache_decode_reference_map = saved_reference_map;
	zend_opcache_user_cache_decode_array_map = saved_array_map;

	return result;
}

bool zend_opcache_user_cache_shared_graph_can_overwrite_payload_locked(uint32_t payload_offset)
{
	return zend_opcache_user_cache_shared_graph_can_move_payload_locked(payload_offset);
}

bool zend_opcache_user_cache_shared_graph_can_move_payload_locked(uint32_t payload_offset)
{
	zend_opcache_user_cache_shared_graph_header *header = zend_opcache_user_cache_shared_graph_payload_header(payload_offset);

	if (header == NULL) {
		return false;
	}

	/* Refcounts are reliable only after optimistic readers have drained. */
	if (!zend_opcache_user_cache_graph_payloads_quiescent_locked()) {
		return false;
	}

	return zend_atomic_int_load_ex(&header->ref_state) == 0;
}

bool zend_opcache_user_cache_shared_graph_rebase_moved_payload_locked(uint32_t payload_offset, ptrdiff_t delta)
{
	const unsigned char *buffer, *old_buffer, *new_base, *old_base;
	zend_opcache_user_cache_shared_graph_header *header;
	zend_opcache_user_cache_shared_graph_value root_value;
	HashTable seen_arrays;
	uint32_t root_type;
	size_t buffer_len, graph_len, old_padding, new_padding;
	bool result;

	if (payload_offset == 0 || delta == 0) {
		return true;
	}

	buffer_len = zend_opcache_user_cache_block_payload_capacity(payload_offset);
	if (buffer_len == 0) {
		return false;
	}

	buffer = (const unsigned char *) zend_opcache_user_cache_ptr(payload_offset);
	old_buffer = buffer + delta;
	old_padding = zend_opcache_user_cache_shared_graph_alignment_padding(old_buffer);
	new_padding = zend_opcache_user_cache_shared_graph_alignment_padding(buffer);

	if (old_padding != new_padding) {
		return false;
	}

	buffer = zend_opcache_user_cache_shared_graph_locate_buffer(buffer, buffer_len, &graph_len);
	if (buffer == NULL) {
		return false;
	}

	header = (zend_opcache_user_cache_shared_graph_header *) buffer;
	if (header->magic != ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_MAGIC ||
		header->version != ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VERSION ||
		(header->root_offset != 0 && header->root_offset >= graph_len)
	) {
		return false;
	}

	new_base = buffer;
	old_base = old_buffer + old_padding;
	root_type = header->root_type != 0 ? header->root_type : ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_OBJECT;

	memset(&root_value, 0, sizeof(root_value));

	root_value.type = (uint8_t) root_type;
	root_value.payload.offset = header->root_offset;

	zend_hash_init(&seen_arrays, 8, NULL, NULL, 0);
	switch (root_type) {
		case ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_ARRAY:
		case ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_DYNAMIC_ARRAY:
		case ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_ENUM:
		case ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_STRING:
			/* ENUM and STRING hold only buffer-relative offsets, so
			 * rebase_graph_value is a no-op for them (its default), but route
			 * them here so a moved payload does not hit the failing default. */
			result = zend_opcache_user_cache_shared_graph_rebase_graph_value(
				buffer,
				&root_value,
				old_base,
				new_base,
				graph_len,
				delta,
				&seen_arrays
			);
			break;
		case ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_OBJECT:
		case ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_SAFE_DIRECT_OBJECT:
			result = header->root_offset != 0 &&
				zend_opcache_user_cache_shared_graph_rebase_graph_value(
					buffer,
					&root_value,
					old_base,
					new_base,
					graph_len,
					delta,
					&seen_arrays
				);
			break;
		default:
			result = false;
			break;
	}

	zend_hash_destroy(&seen_arrays);

	return result;
}

void zend_opcache_user_cache_orphan_graph_payload_locked(uint32_t payload_offset)
{
	zend_opcache_user_cache_header *header = zend_opcache_user_cache_header_ptr();
	uint32_t index;

	if (header == NULL || payload_offset == 0) {
		return;
	}

	zend_opcache_user_cache_shared_graph_force_retire_locked(payload_offset);

	for (index = 0; index < ZEND_OPCACHE_USER_CACHE_ORPHANED_GRAPH_SLOTS; index++) {
		if (header->orphaned_graphs[index] == payload_offset) {
			return;
		}

		if (header->orphaned_graphs[index] == 0) {
			header->orphaned_graphs[index] = payload_offset;

			return;
		}
	}

	header->orphaned_graphs_saturated = 1;
}

void zend_opcache_user_cache_reclaim_orphaned_graphs_locked(void)
{
	zend_opcache_user_cache_header *header = zend_opcache_user_cache_header_ptr();
	zend_opcache_user_cache_shared_graph_header *graph_header;
	uint32_t index, payload_offset;
	bool checked_quiescent = false;

	if (header == NULL) {
		return;
	}

	for (index = 0; index < ZEND_OPCACHE_USER_CACHE_ORPHANED_GRAPH_SLOTS; index++) {
		payload_offset = header->orphaned_graphs[index];
		if (payload_offset == 0) {
			continue;
		}

		if (!checked_quiescent) {
			if (!zend_opcache_user_cache_graph_payloads_quiescent_locked()) {
				return;
			}

			checked_quiescent = true;
		}

		graph_header = zend_opcache_user_cache_shared_graph_payload_header(payload_offset);

		if (graph_header == NULL) {
			header->orphaned_graphs[index] = 0;

			continue;
		}

		if (zend_atomic_int_load_ex(&graph_header->ref_state) !=
			(int) ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_RETIRED
		) {
			header->orphaned_graphs[index] = 0;

			continue;
		}

		header->orphaned_graphs[index] = 0;
		zend_opcache_user_cache_free_locked(payload_offset);
	}

	if (header->orphaned_graphs_saturated != 0) {
		if (!checked_quiescent) {
			if (!zend_opcache_user_cache_graph_payloads_quiescent_locked()) {
				return;
			}

			checked_quiescent = true;
		}

		zend_opcache_user_cache_reclaim_saturated_orphaned_graphs_locked(header);
		header->orphaned_graphs_saturated = 0;
	}
}

bool zend_opcache_user_cache_shared_graph_acquire_locked(uint32_t payload_offset)
{
	zend_opcache_user_cache_shared_graph_header *header = zend_opcache_user_cache_shared_graph_payload_header(payload_offset);
	int state, refcount, expected;

	if (header == NULL) {
		return false;
	}

	for (;;) {
		state = zend_atomic_int_load_ex(&header->ref_state);
		refcount = state & ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_REFCOUNT_MASK;
		expected = state;

		if ((state & ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_RETIRED) != 0 ||
			refcount == ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_REFCOUNT_MASK
		) {
			return false;
		}

		if (zend_atomic_int_compare_exchange_ex(&header->ref_state, &expected, state + 1)) {
			return true;
		}
	}
}

bool zend_opcache_user_cache_shared_graph_retire_payload_locked(uint32_t payload_offset)
{
	zend_opcache_user_cache_shared_graph_header *header = zend_opcache_user_cache_shared_graph_payload_header(payload_offset);
	int state, refcount, expected;

	if (header == NULL) {
		return true;
	}

	for (;;) {
		state = zend_atomic_int_load_ex(&header->ref_state);
		refcount = state & ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_REFCOUNT_MASK;
		expected = state;

		if (refcount == 0) {
			return true;
		}

		if ((state & ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_RETIRED) != 0) {
			return false;
		}

		if (zend_atomic_int_compare_exchange_ex(&header->ref_state, &expected, state | ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_RETIRED)) {
			return false;
		}
	}
}

bool zend_opcache_user_cache_shared_graph_release_ref_locked(uint32_t payload_offset)
{
	zend_opcache_user_cache_shared_graph_header *header = zend_opcache_user_cache_shared_graph_payload_header(payload_offset);
	int state, refcount, expected, desired;

	if (header == NULL) {
		return false;
	}

	for (;;) {
		state = zend_atomic_int_load_ex(&header->ref_state);
		refcount = state & ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_REFCOUNT_MASK;
		expected = state;

		if (refcount == 0) {
			return false;
		}

		desired = (state & ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_RETIRED) | (refcount - 1);
		if (zend_atomic_int_compare_exchange_ex(&header->ref_state, &expected, desired)) {
			return (desired & ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_RETIRED) != 0 &&
				(desired & ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_REFCOUNT_MASK) == 0
			;
		}
	}
}

bool zend_opcache_user_cache_has_request_shared_graph_ref(uint32_t payload_offset)
{
	zend_opcache_user_cache_context *context;
	zval *index_zv;
	uint32_t index;

	if (zend_opcache_user_cache_shared_graph_ref_count == 0) {
		return false;
	}

	context = zend_opcache_user_cache_active_context();

	/* The index maps payload offsets to the first matching request pin. */
	if (zend_opcache_user_cache_shared_graph_ref_index != NULL) {
		index_zv = zend_hash_index_find(zend_opcache_user_cache_shared_graph_ref_index, (zend_ulong) payload_offset);
		if (index_zv == NULL) {
			return false;
		}

		index = (uint32_t) Z_LVAL_P(index_zv);
		if (index < zend_opcache_user_cache_shared_graph_ref_count &&
			zend_opcache_user_cache_shared_graph_refs[index].context == context &&
			zend_opcache_user_cache_shared_graph_refs[index].payload_offset == payload_offset
		) {
			return true;
		}
	}

	for (index = 0; index < zend_opcache_user_cache_shared_graph_ref_count; index++) {
		if (zend_opcache_user_cache_shared_graph_refs[index].context == context &&
			zend_opcache_user_cache_shared_graph_refs[index].payload_offset == payload_offset
		) {
			return true;
		}
	}

	return false;
}

/* Avoid allocation inside the optimistic reader window. */
void zend_opcache_user_cache_shared_graph_ref_reserve(void)
{
	if (zend_opcache_user_cache_shared_graph_ref_count == zend_opcache_user_cache_shared_graph_ref_capacity) {
		zend_opcache_user_cache_shared_graph_ref_capacity = zend_opcache_user_cache_shared_graph_ref_capacity == 0
			? 8
			: zend_opcache_user_cache_shared_graph_ref_capacity * 2
		;

		zend_opcache_user_cache_shared_graph_refs = erealloc(
			zend_opcache_user_cache_shared_graph_refs,
			sizeof(zend_opcache_user_cache_shared_graph_ref) * zend_opcache_user_cache_shared_graph_ref_capacity
		);
	}

	if (zend_opcache_user_cache_shared_graph_ref_index == NULL) {
		zend_opcache_user_cache_shared_graph_ref_index = emalloc(sizeof(HashTable));
		zend_hash_init(zend_opcache_user_cache_shared_graph_ref_index, 8, NULL, NULL, 0);
	}

	zend_hash_extend(
		zend_opcache_user_cache_shared_graph_ref_index,
		zend_hash_num_elements(zend_opcache_user_cache_shared_graph_ref_index) + 1,
		0
	);
}

void zend_opcache_user_cache_register_shared_graph_ref(uint32_t payload_offset)
{
	zval index_zv;

	if (zend_opcache_user_cache_shared_graph_ref_count == zend_opcache_user_cache_shared_graph_ref_capacity) {
		zend_opcache_user_cache_shared_graph_ref_capacity = zend_opcache_user_cache_shared_graph_ref_capacity == 0
			? 8
			: zend_opcache_user_cache_shared_graph_ref_capacity * 2
		;

		zend_opcache_user_cache_shared_graph_refs = erealloc(
			zend_opcache_user_cache_shared_graph_refs,
			sizeof(zend_opcache_user_cache_shared_graph_ref) * zend_opcache_user_cache_shared_graph_ref_capacity
		);
	}

	zend_opcache_user_cache_shared_graph_refs[zend_opcache_user_cache_shared_graph_ref_count].context = zend_opcache_user_cache_active_context();
	zend_opcache_user_cache_shared_graph_refs[zend_opcache_user_cache_shared_graph_ref_count].payload_offset = payload_offset;

	if (zend_opcache_user_cache_shared_graph_ref_index == NULL) {
		zend_opcache_user_cache_shared_graph_ref_index = emalloc(sizeof(HashTable));
		zend_hash_init(zend_opcache_user_cache_shared_graph_ref_index, 8, NULL, NULL, 0);
	}

	ZVAL_LONG(&index_zv, (zend_long) zend_opcache_user_cache_shared_graph_ref_count);
	zend_hash_index_add(
		zend_opcache_user_cache_shared_graph_ref_index,
		(zend_ulong) payload_offset,
		&index_zv
	);

	zend_opcache_user_cache_shared_graph_ref_count++;
}

void zend_opcache_user_cache_defer_retired_shared_graph_free(uint32_t payload_offset)
{
	zend_opcache_user_cache_shared_graph_ref *ref;

	if (zend_opcache_user_cache_retired_shared_graph_count == zend_opcache_user_cache_retired_shared_graph_capacity) {
		zend_opcache_user_cache_retired_shared_graph_capacity = zend_opcache_user_cache_retired_shared_graph_capacity == 0
			? 4
			: zend_opcache_user_cache_retired_shared_graph_capacity * 2
		;

		zend_opcache_user_cache_retired_shared_graphs = erealloc(
			zend_opcache_user_cache_retired_shared_graphs,
			sizeof(zend_opcache_user_cache_shared_graph_ref) * zend_opcache_user_cache_retired_shared_graph_capacity
		);
	}

	ref = &zend_opcache_user_cache_retired_shared_graphs[zend_opcache_user_cache_retired_shared_graph_count++];
	ref->context = zend_opcache_user_cache_active_context();
	ref->payload_offset = payload_offset;
}

bool zend_opcache_user_cache_release_request_shared_graph_refs(void)
{
	zend_opcache_user_cache_shared_graph_ref *ref;
	zend_opcache_user_cache_context *context, *previous_context;
	uint32_t index, inner;
	bool released = false;

	if (zend_opcache_user_cache_shared_graph_ref_count == 0) {
		if (zend_opcache_user_cache_shared_graph_ref_index != NULL) {
			zend_hash_destroy(zend_opcache_user_cache_shared_graph_ref_index);
			efree(zend_opcache_user_cache_shared_graph_ref_index);
			zend_opcache_user_cache_shared_graph_ref_index = NULL;
		}

		return zend_opcache_user_cache_free_retired_shared_graphs();
	}

	/* Release request pins grouped by context. */
	for (index = 0; index < zend_opcache_user_cache_shared_graph_ref_count; index++) {
		context = zend_opcache_user_cache_shared_graph_refs[index].context;

		if (context == NULL) {
			continue;
		}

		previous_context = zend_opcache_user_cache_activate_context(context);

		if (zend_opcache_user_cache_wlock()) {
			if (zend_opcache_user_cache_header_is_initialized_locked()) {
				for (inner = index; inner < zend_opcache_user_cache_shared_graph_ref_count; inner++) {
					ref = &zend_opcache_user_cache_shared_graph_refs[inner];

					if (ref->context != context) {
						continue;
					}

					if (ref->payload_offset == 0) {
						ref->context = NULL;
						continue;
					}

					released = true;
					if (zend_opcache_user_cache_shared_graph_release_ref_locked(ref->payload_offset)) {
						if (zend_opcache_user_cache_graph_payloads_quiescent_locked()) {
							zend_opcache_user_cache_free_locked(ref->payload_offset);
						} else {
							zend_opcache_user_cache_orphan_graph_payload_locked(ref->payload_offset);
						}
					}

					ref->context = NULL;
				}

				zend_opcache_user_cache_free_retired_for_context_locked(context);
			}

			zend_opcache_user_cache_unlock();
		}

		zend_opcache_user_cache_restore_context(previous_context);
	}

	efree(zend_opcache_user_cache_shared_graph_refs);

	zend_opcache_user_cache_shared_graph_refs = NULL;
	zend_opcache_user_cache_shared_graph_ref_count = 0;
	zend_opcache_user_cache_shared_graph_ref_capacity = 0;

	if (zend_opcache_user_cache_shared_graph_ref_index != NULL) {
		zend_hash_destroy(zend_opcache_user_cache_shared_graph_ref_index);
		efree(zend_opcache_user_cache_shared_graph_ref_index);
		zend_opcache_user_cache_shared_graph_ref_index = NULL;
	}

	return zend_opcache_user_cache_free_retired_shared_graphs() || released;
}
