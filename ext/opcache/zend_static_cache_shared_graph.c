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

/* Within one decode the class for a given (buffer, offset) is fixed, and after
 * string dedup every object of a class shares one class-name offset. Memoize the
 * last resolved class so a homogeneous graph (an array or tree of one class)
 * resolves its class once instead of once per node. The slot is reset at the top
 * of each decode so a recycled buffer address cannot alias a stale class. */
static ZEND_EXT_TLS struct {
	const unsigned char *buffer;
	uint32_t class_name_offset;
	zend_class_entry *ce;
} zend_opcache_static_cache_decode_class_memo;

/* Shared-graph payloads start inside generic SHM blocks whose payload base may
 * be less aligned than zend managed objects require. Align the graph start
 * within the payload and treat that aligned layout as the only valid format. */
static zend_always_inline size_t zend_opcache_static_cache_shared_graph_alignment_padding(const void *buffer)
{
	uintptr_t raw_address, aligned_address;

	raw_address = (uintptr_t) buffer;
	aligned_address = (uintptr_t) ZEND_MM_ALIGNED_SIZE(raw_address);

	return (size_t) (aligned_address - raw_address);
}

static zend_always_inline const unsigned char *zend_opcache_static_cache_shared_graph_locate_buffer(
	const unsigned char *buffer,
	size_t buffer_len,
	size_t *graph_len
)
{
	const zend_opcache_static_cache_shared_graph_header *header;
	size_t padding;

	padding = zend_opcache_static_cache_shared_graph_alignment_padding(buffer);
	if (padding > buffer_len || buffer_len - padding < sizeof(zend_opcache_static_cache_shared_graph_header)) {
		return NULL;
	}

	buffer += padding;
	buffer_len -= padding;
	header = (const zend_opcache_static_cache_shared_graph_header *) buffer;
	if (header->magic != ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_MAGIC ||
		header->version != ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_VERSION
	) {
		return NULL;
	}

	if (graph_len != NULL) {
		*graph_len = buffer_len;
	}

	return buffer;
}

static zend_always_inline void zend_opcache_static_cache_shared_graph_calc_init(zend_opcache_static_cache_shared_graph_calc_context *context)
{
	context->size = 0;

	zend_hash_init(&context->seen_arrays, 8, NULL, NULL, 0);
	zend_hash_init(&context->seen_objects, 8, NULL, NULL, 0);
}

static zend_always_inline void zend_opcache_static_cache_shared_graph_calc_destroy(zend_opcache_static_cache_shared_graph_calc_context *context)
{
	zend_hash_destroy(&context->seen_objects);
	zend_hash_destroy(&context->seen_arrays);
}

static zend_always_inline bool zend_opcache_static_cache_shared_graph_reserve_size(size_t *size, size_t amount)
{
	size_t aligned_amount;

	aligned_amount = ZEND_ALIGNED_SIZE(amount);
	if (*size > SIZE_MAX - aligned_amount) {
		return false;
	}

	*size += aligned_amount;

	return *size <= UINT32_MAX;
}

static zend_always_inline bool zend_opcache_static_cache_shared_graph_mark_seen(HashTable *seen_objects, zend_object *object)
{
	zend_ulong object_key;

	object_key = (zend_ulong) (uintptr_t) object;
	if (zend_hash_index_exists(seen_objects, object_key)) {
		return false;
	}

	return zend_hash_index_add_empty_element(seen_objects, object_key) != NULL;
}

static zend_always_inline bool zend_opcache_static_cache_shared_graph_mark_seen_array(HashTable *seen_arrays, const HashTable *array)
{
	zend_ulong array_key;

	array_key = (zend_ulong) (uintptr_t) array;
	if (zend_hash_index_exists(seen_arrays, array_key)) {
		return false;
	}

	return zend_hash_index_add_empty_element(seen_arrays, array_key) != NULL;
}

static zend_always_inline bool zend_opcache_static_cache_shared_graph_has_custom_serialization(zend_class_entry *ce)
{
	return ce->create_object != NULL ||
		ce->__serialize != NULL ||
		ce->__unserialize != NULL ||
		zend_hash_str_exists(&ce->function_table, ZEND_STRL("__sleep")) ||
		zend_hash_str_exists(&ce->function_table, ZEND_STRL("__wakeup"))
	;
}

static zend_always_inline bool zend_opcache_static_cache_shared_graph_can_restore_direct(zend_class_entry *ce)
{
	if (ce->type != ZEND_USER_CLASS) {
		return false;
	}

	if (zend_opcache_serializer_find_safe_direct_cache_base(ce) != NULL) {
		return false;
	}

	return !zend_opcache_static_cache_shared_graph_has_custom_serialization(ce);
}

static bool zend_opcache_static_cache_shared_graph_can_copy_direct_value(HashTable *seen_arrays, const zval *value)
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

			if (!zend_opcache_static_cache_shared_graph_mark_seen_array(seen_arrays, array)) {
				return false;
			}

			array_key = (zend_ulong) (uintptr_t) array;
			if (HT_IS_PACKED(array)) {
				for (index = 0; index < array->nNumUsed; index++) {
					packed_value = &array->arPacked[index];
					if (!zend_opcache_static_cache_shared_graph_can_copy_direct_value(seen_arrays, packed_value)) {
						result = false;
						break;
					}
				}
			} else {
				bucket = array->arData;
				for (index = 0; index < array->nNumUsed; index++) {
					if (Z_TYPE(bucket[index].val) != IS_UNDEF &&
						!zend_opcache_static_cache_shared_graph_can_copy_direct_value(seen_arrays, &bucket[index].val)
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

static bool zend_opcache_static_cache_shared_graph_calc_direct_value(
		zend_opcache_static_cache_shared_graph_calc_context *context,
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
			return zend_opcache_static_cache_shared_graph_reserve_size(
				&context->size,
				_ZSTR_STRUCT_SIZE(ZSTR_LEN(Z_STR_P(value)))
			);
		case IS_ARRAY:
			array = Z_ARRVAL_P(value);
			if (array->nNumOfElements == 0) {
				return true;
			}

			if (!zend_opcache_static_cache_shared_graph_mark_seen_array(&context->seen_arrays, array)) {
				return false;
			}

			array_key = (zend_ulong) (uintptr_t) array;
			data_size = HT_IS_PACKED(array) ? HT_PACKED_USED_SIZE(array) : HT_USED_SIZE(array);
			if (!zend_opcache_static_cache_shared_graph_reserve_size(&context->size, sizeof(zend_array)) ||
				!zend_opcache_static_cache_shared_graph_reserve_size(&context->size, data_size)
			) {
				result = false;
				goto direct_array_done;
			}

			if (HT_IS_PACKED(array)) {
				for (index = 0; index < array->nNumUsed; index++) {
					packed_value = &array->arPacked[index];
					if (!zend_opcache_static_cache_shared_graph_calc_direct_value(context, packed_value)) {
						result = false;
						break;
					}
				}
			} else {
				bucket = array->arData;
				for (index = 0; index < array->nNumUsed; index++) {
					if (bucket[index].key != NULL &&
						!zend_opcache_static_cache_shared_graph_reserve_size(
							&context->size,
							_ZSTR_STRUCT_SIZE(ZSTR_LEN(bucket[index].key))
						)
					) {
						result = false;
						break;
					}

					if (Z_TYPE(bucket[index].val) != IS_UNDEF &&
						!zend_opcache_static_cache_shared_graph_calc_direct_value(context, &bucket[index].val)
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

static bool zend_opcache_static_cache_shared_graph_calc_value(
	zend_opcache_static_cache_shared_graph_calc_context *context,
	const zval *value
)
{
	const HashTable *array;
	zend_object *object;
	zend_class_entry *ce, *safe_direct_base;
	zend_string *key, *property_name;
	zend_ulong array_key;
	zval *element, *property_value, *source_value;
	HashTable *properties;
	uint32_t property_count;
	size_t encoded_len;
	bool result;
	unsigned char *encoded;

	switch (Z_TYPE_P(value)) {
		case IS_UNDEF:
		case IS_NULL:
		case IS_FALSE:
		case IS_TRUE:
		case IS_LONG:
		case IS_DOUBLE:
			return true;
		case IS_STRING:
			return zend_opcache_static_cache_shared_graph_reserve_size(&context->size, _ZSTR_STRUCT_SIZE(ZSTR_LEN(Z_STR_P(value))));
		case IS_ARRAY:
			array = Z_ARRVAL_P(value);

			if (array->nNumOfElements == 0) {
				return true;
			}

			if (zend_opcache_static_cache_shared_graph_can_copy_direct_value(&context->seen_arrays, value)) {
				return zend_opcache_static_cache_shared_graph_calc_direct_value(context, value);
			}

			if (!zend_opcache_static_cache_shared_graph_mark_seen_array(&context->seen_arrays, array)) {
				return false;
			}

			array_key = (zend_ulong) (uintptr_t) array;
			result = true;

			if (!zend_opcache_static_cache_shared_graph_reserve_size(&context->size, sizeof(zend_opcache_static_cache_shared_graph_array)) ||
				!zend_opcache_static_cache_shared_graph_reserve_size(
					&context->size,
					(size_t) array->nNumOfElements * sizeof(zend_opcache_static_cache_shared_graph_array_element)
				)
			) {
				result = false;

				goto array_done;
			}

			ZEND_HASH_FOREACH_STR_KEY_VAL((HashTable *) array, key, element) {
				if (key != NULL && !zend_opcache_static_cache_shared_graph_reserve_size(&context->size, _ZSTR_STRUCT_SIZE(ZSTR_LEN(key)))) {
					result = false;
					break;
				}

				if (!zend_opcache_static_cache_shared_graph_calc_value(context, element)) {
					result = false;
					break;
				}
			} ZEND_HASH_FOREACH_END();

			goto array_done;
		case IS_OBJECT:
			object = Z_OBJ_P(value);
			ce = object->ce;

			safe_direct_base = zend_opcache_serializer_find_safe_direct_cache_base(ce);
			if (safe_direct_base != NULL) {
				if (zend_opcache_serializer_has_safe_direct_cache_overrides(ce, safe_direct_base)) {
					return false;
				}

				encoded = NULL;
				encoded_len = 0;
				if (!zend_opcache_serialize(&encoded, &encoded_len, value)) {
					return false;
				}

				efree(encoded);

				return zend_opcache_static_cache_shared_graph_reserve_size(
					&context->size,
					sizeof(zend_opcache_static_cache_shared_graph_serialized_object) + encoded_len - 1
				);
			}

			if (!zend_opcache_static_cache_shared_graph_can_restore_direct(ce)) {
				return false;
			}

			if (!zend_opcache_static_cache_shared_graph_mark_seen(&context->seen_objects, object)) {
				return false;
			}

			if (!zend_opcache_static_cache_shared_graph_reserve_size(&context->size, sizeof(zend_opcache_static_cache_shared_graph_object))) {
				return false;
			}

			if (!zend_opcache_static_cache_shared_graph_reserve_size(&context->size, _ZSTR_STRUCT_SIZE(ZSTR_LEN(ce->name)))) {
				return false;
			}

			properties = zend_get_properties_for((zval *) value, ZEND_PROP_PURPOSE_SERIALIZE);
			property_count = properties != NULL ? properties->nNumOfElements : 0;
			if (property_count != 0 &&
				!zend_opcache_static_cache_shared_graph_reserve_size(
					&context->size,
					(size_t) property_count * sizeof(zend_opcache_static_cache_shared_graph_property)
				)
			) {
				if (properties != NULL) {
					zend_release_properties(properties);
				}
				return false;
			}

			if (properties != NULL) {
				ZEND_HASH_FOREACH_STR_KEY_VAL(properties, property_name, property_value) {
					if (property_name == NULL || !zend_opcache_static_cache_shared_graph_reserve_size(&context->size, _ZSTR_STRUCT_SIZE(ZSTR_LEN(property_name)))) {
						zend_release_properties(properties);
						return false;
					}

					source_value =
						Z_TYPE_P(property_value) == IS_INDIRECT
							? Z_INDIRECT_P(property_value)
							: property_value
					;

					if (!zend_opcache_static_cache_shared_graph_calc_value(context, source_value)) {
						zend_release_properties(properties);
						return false;
					}
				} ZEND_HASH_FOREACH_END();

				zend_release_properties(properties);
			}

			return true;
		default:
			return false;
	}

array_done:
	zend_hash_index_del(&context->seen_arrays, array_key);

	return result;
}

static void zend_opcache_static_cache_shared_graph_copy_init(
	zend_opcache_static_cache_shared_graph_copy_context *context,
	unsigned char *buffer,
	size_t size
)
{
	context->buffer = buffer;
	context->size = size;
	context->position = 0;

	zend_hash_init(&context->seen_arrays, 8, NULL, NULL, 0);
	zend_hash_init(&context->seen_objects, 8, NULL, NULL, 0);
	zend_hash_init(&context->string_dedup, 8, NULL, NULL, 0);
	context->has_serialized_object = false;
}

static void zend_opcache_static_cache_shared_graph_copy_destroy(zend_opcache_static_cache_shared_graph_copy_context *context)
{
	zend_hash_destroy(&context->string_dedup);
	zend_hash_destroy(&context->seen_objects);
	zend_hash_destroy(&context->seen_arrays);
}

static bool zend_opcache_static_cache_shared_graph_copy_alloc(
	zend_opcache_static_cache_shared_graph_copy_context *context,
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

static bool zend_opcache_static_cache_shared_graph_copy_string(
	zend_opcache_static_cache_shared_graph_copy_context *context,
	const zend_string *string,
	uint32_t *offset
)
{
	zend_string *new_string;
	uint32_t string_offset;
	size_t string_size;
	zval *cached, cached_offset;

	/* Deduplicate equal strings within a payload. Class names and property
	 * names repeat across every object, so interning them to a single buffer
	 * copy shrinks the payload and lets the decoder memoize the class per
	 * offset. The strings are immutable (IS_STR_INTERNED | IS_STR_PERMANENT),
	 * so sharing one copy is safe. */
	cached = zend_hash_find(&context->string_dedup, (zend_string *) string);
	if (cached != NULL) {
		*offset = (uint32_t) Z_LVAL_P(cached);

		return true;
	}

	string_size = _ZSTR_STRUCT_SIZE(ZSTR_LEN(string));
	if (!zend_opcache_static_cache_shared_graph_copy_alloc(context, string_size, &string_offset)) {
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

static bool zend_opcache_static_cache_shared_graph_copy_direct_value(
		zend_opcache_static_cache_shared_graph_copy_context *context,
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
			if (!zend_opcache_static_cache_shared_graph_copy_string(context, Z_STR_P(source), &string_offset)) {
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

			if (!zend_opcache_static_cache_shared_graph_mark_seen_array(&context->seen_arrays, source_array)) {
				return false;
			}

			array_key = (zend_ulong) (uintptr_t) source_array;
			data_size = HT_IS_PACKED(source_array) ? HT_PACKED_USED_SIZE(source_array) : HT_USED_SIZE(source_array);
			if (!zend_opcache_static_cache_shared_graph_copy_alloc(context, sizeof(zend_array), &array_offset) ||
				!zend_opcache_static_cache_shared_graph_copy_alloc(context, data_size, &data_offset)
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
					if (!zend_opcache_static_cache_shared_graph_copy_direct_value(context, source_packed, &target_packed[index])) {
						result = false;
						break;
					}
				}
			} else {
				source_bucket = source_array->arData;
				target_bucket = target->arData;
				for (index = 0; index < source_array->nNumUsed; index++) {
					if (source_bucket[index].key != NULL) {
						if (!zend_opcache_static_cache_shared_graph_copy_string(context, source_bucket[index].key, &key_offset)) {
							result = false;
							break;
						}

						target_bucket[index].key = (zend_string *) (void *) (context->buffer + key_offset);
					} else {
						target_bucket[index].key = NULL;
					}

					if (!zend_opcache_static_cache_shared_graph_copy_direct_value(context, &source_bucket[index].val, &target_bucket[index].val)) {
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

static bool zend_opcache_static_cache_shared_graph_copy_property_value(
	zend_opcache_static_cache_shared_graph_copy_context *context,
	const zval *source,
	zend_opcache_static_cache_shared_graph_value *destination
)
{
	zend_opcache_static_cache_shared_graph_serialized_object *serialized_object;
	zend_opcache_static_cache_shared_graph_object *graph_object;
	zend_opcache_static_cache_shared_graph_property *graph_properties;
	zend_opcache_static_cache_shared_graph_array *graph_array;
	zend_opcache_static_cache_shared_graph_array_element *graph_elements, *graph_element;
	zend_object *object;
	zend_class_entry *ce, *safe_direct_base;
	zend_string *property_name, *key;
	zend_ulong array_key, h;
	zval *property_value, *source_value, *element, array_value;
	HashTable *properties;
	uint32_t payload_offset, string_offset, object_offset, class_name_offset, properties_offset, property_index, property_count, array_offset, elements_offset, key_offset;
	size_t encoded_len, serialized_size;
	bool result;
	unsigned char *encoded;

	memset(destination, 0, sizeof(*destination));

	switch (Z_TYPE_P(source)) {
		case IS_UNDEF:
			destination->type = ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_VALUE_UNDEF;

			return true;
		case IS_NULL:
			destination->type = ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_VALUE_NULL;

			return true;
		case IS_TRUE:
			destination->type = ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_VALUE_TRUE;

			return true;
		case IS_FALSE:
			destination->type = ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_VALUE_FALSE;

			return true;
		case IS_LONG:
			destination->type = ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_VALUE_LONG;
			destination->payload.long_value = Z_LVAL_P(source);

			return true;
		case IS_DOUBLE:
			destination->type = ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_VALUE_DOUBLE;
			destination->payload.double_value = Z_DVAL_P(source);

			return true;
		case IS_STRING:
			if (!zend_opcache_static_cache_shared_graph_copy_string(context, Z_STR_P(source), &string_offset)) {
				return false;
			}

			destination->type = ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_VALUE_STRING;
			destination->payload.offset = string_offset;

			return true;
		case IS_ARRAY: {
			result = true;

			if (Z_ARRVAL_P(source)->nNumOfElements == 0) {
				destination->type = ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_VALUE_ARRAY;
				destination->payload.offset = 0;

				return true;
			}

			if (zend_opcache_static_cache_shared_graph_can_copy_direct_value(&context->seen_arrays, source)) {
				if (!zend_opcache_static_cache_shared_graph_copy_direct_value(context, source, &array_value)) {
					return false;
				}

				destination->type = ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_VALUE_ARRAY;
				destination->payload.offset = (uint32_t) ((unsigned char *) Z_ARRVAL(array_value) - context->buffer);

				return true;
			}

			if (!zend_opcache_static_cache_shared_graph_mark_seen_array(&context->seen_arrays, Z_ARRVAL_P(source))) {
				return false;
			}

			array_key = (zend_ulong) (uintptr_t) Z_ARRVAL_P(source);
			if (!zend_opcache_static_cache_shared_graph_copy_alloc(context, sizeof(*graph_array), &array_offset) ||
				!zend_opcache_static_cache_shared_graph_copy_alloc(
					context,
					(size_t) Z_ARRVAL_P(source)->nNumOfElements * sizeof(*graph_elements),
					&elements_offset
				)
			) {
				result = false;

				goto array_done;
			}

			graph_array = (zend_opcache_static_cache_shared_graph_array *) (context->buffer + array_offset);
			graph_array->count = Z_ARRVAL_P(source)->nNumOfElements;
			graph_array->next_free = (uint32_t) Z_ARRVAL_P(source)->nNextFreeElement;
			graph_array->elements_offset = elements_offset;
			graph_elements = (zend_opcache_static_cache_shared_graph_array_element *) (context->buffer + elements_offset);
			graph_element = graph_elements;

			ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(source), h, key, element) {
				memset(graph_element, 0, sizeof(*graph_element));
				graph_element->h = h;
				if (key != NULL) {
					if (!zend_opcache_static_cache_shared_graph_copy_string(context, key, &key_offset)) {
						result = false;
						break;
					}

					graph_element->key_offset = key_offset;
				}

				if (!zend_opcache_static_cache_shared_graph_copy_property_value(context, element, &graph_element->value)) {
					result = false;
					break;
				}

				++graph_element;
			} ZEND_HASH_FOREACH_END();

			if (result) {
				destination->type = ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_VALUE_DYNAMIC_ARRAY;
				destination->payload.offset = array_offset;
			}

			goto array_done;
		}
		case IS_OBJECT:
			ce = Z_OBJCE_P(source);
			safe_direct_base = zend_opcache_serializer_find_safe_direct_cache_base(ce);
			if (safe_direct_base != NULL) {
				if (zend_opcache_serializer_has_safe_direct_cache_overrides(ce, safe_direct_base)) {
					return false;
				}

				encoded = NULL;
				encoded_len = 0;
				if (!zend_opcache_serialize(&encoded, &encoded_len, source)) {
					return false;
				}

				serialized_size = sizeof(zend_opcache_static_cache_shared_graph_serialized_object) + encoded_len - 1;
				if (!zend_opcache_static_cache_shared_graph_copy_alloc(context, serialized_size, &payload_offset)) {
					efree(encoded);

					return false;
				}

				serialized_object = (zend_opcache_static_cache_shared_graph_serialized_object *) (context->buffer + payload_offset);
				serialized_object->data_len = (uint32_t) encoded_len;
				memcpy(serialized_object->data, encoded, encoded_len);
				efree(encoded);

				destination->type = ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_VALUE_SERIALIZED_OBJECT;
				destination->payload.offset = payload_offset;

				/* This node is decoded by re-running unserialize, so a graph
				 * containing it keeps its request-local prototype. */
				context->has_serialized_object = true;

				return true;
			}

			object = Z_OBJ_P(source);
			if (!zend_opcache_static_cache_shared_graph_can_restore_direct(object->ce)) {
				return false;
			}

			if (!zend_opcache_static_cache_shared_graph_mark_seen(&context->seen_objects, object)) {
				return false;
			}

			if (!zend_opcache_static_cache_shared_graph_copy_alloc(context, sizeof(*graph_object), &object_offset)) {
				return false;
			}

			if (!zend_opcache_static_cache_shared_graph_copy_string(context, object->ce->name, &class_name_offset)) {
				return false;
			}

			properties = zend_get_properties_for((zval *) source, ZEND_PROP_PURPOSE_SERIALIZE);
			property_count = properties != NULL ? properties->nNumOfElements : 0;
			properties_offset = 0;
			if (property_count != 0 &&
				!zend_opcache_static_cache_shared_graph_copy_alloc(
					context,
					((size_t) property_count * sizeof(zend_opcache_static_cache_shared_graph_property)),
					&properties_offset
				)
			) {
				if (properties != NULL) {
					zend_release_properties(properties);
				}
				return false;
			}

			graph_object = (zend_opcache_static_cache_shared_graph_object *) (context->buffer + object_offset);
			graph_object->class_name_offset = class_name_offset;
			graph_object->property_count = property_count;
			graph_object->properties_offset = properties_offset;

			if (property_count == 0) {
				if (properties != NULL) {
					zend_release_properties(properties);
				}

				destination->type = ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_VALUE_OBJECT;
				destination->payload.offset = object_offset;

				return true;
			}

			graph_properties = (zend_opcache_static_cache_shared_graph_property *) (context->buffer + properties_offset);
			property_index = 0;
			ZEND_HASH_FOREACH_STR_KEY_VAL(properties, property_name, property_value) {
				if (property_name == NULL ||
					!zend_opcache_static_cache_shared_graph_copy_string(context, property_name, &graph_properties[property_index].name_offset)
				) {
					zend_release_properties(properties);

					return false;
				}

				source_value =
					Z_TYPE_P(property_value) == IS_INDIRECT
						? Z_INDIRECT_P(property_value)
						: property_value
				;

				if (!zend_opcache_static_cache_shared_graph_copy_property_value(
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
			destination->type = ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_VALUE_OBJECT;
			destination->payload.offset = object_offset;

			return true;
		default:
			return false;
	}

array_done:
	zend_hash_index_del(&context->seen_arrays, array_key);

	return result;
}

static bool zend_opcache_static_cache_shared_graph_try_update_declared_object_property(
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
		(property_info->flags & (ZEND_ACC_STATIC|ZEND_ACC_READONLY|ZEND_ACC_PPP_SET_MASK|ZEND_ACC_VIRTUAL)) != 0 ||
		(property_info->flags & ZEND_ACC_PPP_MASK) != ZEND_ACC_PUBLIC ||
		property_info->offset == ZEND_VIRTUAL_PROPERTY_OFFSET
	) {
		return false;
	}

	slot = OBJ_PROP(object, property_info->offset);
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

static bool zend_opcache_static_cache_shared_graph_update_object_property(
	zval *object_zv,
	zend_string *property_name,
	zval *property_value
)
{
	zend_object *object;
	zend_property_info *property_info;
	bool failed;

	object = Z_OBJ_P(object_zv);
	if (ZSTR_LEN(property_name) != 0 && ZSTR_VAL(property_name)[0] != '\0') {
		property_info = zend_get_property_info(object->ce, property_name, true);
		if (zend_opcache_static_cache_shared_graph_try_update_declared_object_property(
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
	}

	return zend_opcache_serializer_update_object_property(object, property_name, property_value);
}

static bool zend_opcache_static_cache_shared_graph_update_object_property_at(
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

		if (zend_opcache_static_cache_shared_graph_try_update_declared_object_property(
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
	}

	return zend_opcache_static_cache_shared_graph_update_object_property(object_zv, property_name, property_value);
}

static bool zend_opcache_static_cache_fetch_shared_graph_value(
	const unsigned char *buffer,
	const zend_opcache_static_cache_shared_graph_value *value,
	zval *destination
)
{
	const zend_opcache_static_cache_shared_graph_object *graph_object;
	const zend_opcache_static_cache_shared_graph_array *graph_array;
	const zend_opcache_static_cache_shared_graph_array_element *graph_elements, *graph_element;
	const zend_opcache_static_cache_shared_graph_property *properties, *property;
	const zend_opcache_static_cache_shared_graph_serialized_object *serialized_object;
	zend_class_entry *ce;
	zend_string *class_name, *property_name;
	zval property_value;
	uint32_t index;

	switch (value->type) {
		case ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_VALUE_UNDEF:
			ZVAL_UNDEF(destination);
			return true;
		case ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_VALUE_NULL:
			ZVAL_NULL(destination);
			return true;
		case ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_VALUE_TRUE:
			ZVAL_TRUE(destination);
			return true;
		case ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_VALUE_FALSE:
			ZVAL_FALSE(destination);
			return true;
		case ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_VALUE_LONG:
			ZVAL_LONG(destination, value->payload.long_value);
			return true;
		case ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_VALUE_DOUBLE:
			ZVAL_DOUBLE(destination, value->payload.double_value);
			return true;
		case ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_VALUE_STRING:
			ZVAL_INTERNED_STR(destination, (zend_string *) (void *) (buffer + (uint32_t) value->payload.offset));
			return true;
		case ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_VALUE_ARRAY:
			if ((uint32_t) value->payload.offset == 0) {
				ZVAL_EMPTY_ARRAY(destination);
			} else {
				ZVAL_ARR(destination, (zend_array *) (void *) (buffer + (uint32_t) value->payload.offset));
				Z_TYPE_FLAGS_P(destination) = 0;
			}

			zend_opcache_static_cache_capture_decoded_value(destination);

			return true;
		case ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_VALUE_DYNAMIC_ARRAY:
			graph_array = (const zend_opcache_static_cache_shared_graph_array *) (buffer + (uint32_t) value->payload.offset);
			array_init_size(destination, graph_array->count);
			graph_elements = (const zend_opcache_static_cache_shared_graph_array_element *) (buffer + graph_array->elements_offset);

			for (index = 0; index < graph_array->count; index++) {
				graph_element = &graph_elements[index];
				ZVAL_UNDEF(&property_value);
				if (!zend_opcache_static_cache_fetch_shared_graph_value(buffer, &graph_element->value, &property_value)) {
					zval_ptr_dtor(destination);
					ZVAL_UNDEF(destination);

					return false;
				}

				if (graph_element->key_offset != 0) {
					property_name = (zend_string *) (void *) (buffer + graph_element->key_offset);
					if (zend_hash_add_new(Z_ARRVAL_P(destination), property_name, &property_value) == NULL) {
						zval_ptr_dtor(&property_value);
						zval_ptr_dtor(destination);
						ZVAL_UNDEF(destination);

						return false;
					}
				} else if (zend_hash_index_add_new(Z_ARRVAL_P(destination), graph_element->h, &property_value) == NULL) {
					zval_ptr_dtor(&property_value);
					zval_ptr_dtor(destination);
					ZVAL_UNDEF(destination);

					return false;
				}
			}

			Z_ARRVAL_P(destination)->nNextFreeElement = graph_array->next_free;
			zend_opcache_static_cache_capture_decoded_value(destination);

			return true;
		case ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_VALUE_OBJECT:
			graph_object = (const zend_opcache_static_cache_shared_graph_object *) (buffer + (uint32_t) value->payload.offset);
			if (zend_opcache_static_cache_decode_class_memo.buffer == buffer &&
				zend_opcache_static_cache_decode_class_memo.class_name_offset == graph_object->class_name_offset
			) {
				ce = zend_opcache_static_cache_decode_class_memo.ce;
			} else {
				class_name = (zend_string *) (void *) (buffer + graph_object->class_name_offset);
				ce = zend_lookup_class(class_name);
				if (ce != NULL) {
					zend_opcache_static_cache_decode_class_memo.buffer = buffer;
					zend_opcache_static_cache_decode_class_memo.class_name_offset = graph_object->class_name_offset;
					zend_opcache_static_cache_decode_class_memo.ce = ce;
				}
			}

			if (ce == NULL || object_init_ex(destination, ce) != SUCCESS) {
				return false;
			}

			if (graph_object->property_count == 0) {
				zend_opcache_static_cache_capture_decoded_value(destination);
				return true;
			}

			properties = (const zend_opcache_static_cache_shared_graph_property *) (buffer + graph_object->properties_offset);
			for (index = 0; index < graph_object->property_count; index++) {
				property = &properties[index];
				if (property->value.type == ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_VALUE_UNDEF) {
					continue;
				}

				property_name = (zend_string *) (void *) (buffer + property->name_offset);
				ZVAL_UNDEF(&property_value);
				if (!zend_opcache_static_cache_fetch_shared_graph_value(buffer, &property->value, &property_value) ||
					!zend_opcache_static_cache_shared_graph_update_object_property_at(destination, property_name, index, &property_value)
				) {
					zval_ptr_dtor(&property_value);
					zval_ptr_dtor(destination);
					ZVAL_UNDEF(destination);

					return false;
				}

				zval_ptr_dtor(&property_value);
			}

			zend_opcache_static_cache_capture_decoded_value(destination);

			return true;
		case ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_VALUE_SERIALIZED_OBJECT:
			serialized_object = (const zend_opcache_static_cache_shared_graph_serialized_object *) (buffer + (uint32_t) value->payload.offset);

			return zend_opcache_static_cache_capture_active
				? zend_opcache_unserialize_ex(
					destination,
					serialized_object->data,
					serialized_object->data_len,
					zend_opcache_static_cache_capture_decoded_value,
					zend_opcache_static_cache_capture_decoded_reachable_value)
				: zend_opcache_unserialize(destination, serialized_object->data, serialized_object->data_len)
			;
		default:
			return false;
	}
}

static zend_opcache_static_cache_shared_graph_header *zend_opcache_static_cache_shared_graph_payload_header(uint32_t payload_offset)
{
	const unsigned char *graph_buffer;
	zend_opcache_static_cache_shared_graph_header *header;
	size_t buffer_len;

	if (payload_offset == 0) {
		return NULL;
	}

	buffer_len = zend_opcache_static_cache_block_payload_capacity(payload_offset);
	if (buffer_len == 0) {
		return NULL;
	}

	graph_buffer = zend_opcache_static_cache_shared_graph_locate_buffer(
		(const unsigned char *) zend_opcache_static_cache_ptr(payload_offset),
		buffer_len,
		NULL
	);
	if (graph_buffer == NULL) {
		return NULL;
	}

	header = (zend_opcache_static_cache_shared_graph_header *) graph_buffer;
	if (header->magic != ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_MAGIC ||
		header->version != ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_VERSION
	) {
		return NULL;
	}

	return header;
}

/* A shared graph that embeds serialized object nodes is expensive to re-decode
 * (each fetch would re-run unserialize), so such a graph keeps its request-local
 * prototype. A pure-direct graph decodes cheaply from SHM and skips it. */
bool zend_opcache_static_cache_shared_graph_requires_prototype(uint32_t payload_offset)
{
	zend_opcache_static_cache_shared_graph_header *header =
		zend_opcache_static_cache_shared_graph_payload_header(payload_offset)
	;

	if (header == NULL) {
		return true;
	}

	return (header->flags & ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_FLAG_HAS_SERIALIZED_OBJECT) != 0;
}

static bool zend_opcache_static_cache_free_retired_shared_graphs(void)
{
	zend_opcache_static_cache_shared_graph_ref *ref;
	zend_opcache_static_cache_context *previous_context;
	uint32_t index;
	bool freed = false;

	if (zend_opcache_static_cache_retired_shared_graph_count == 0) {
		return false;
	}

	for (index = 0; index < zend_opcache_static_cache_retired_shared_graph_count; index++) {
		ref = &zend_opcache_static_cache_retired_shared_graphs[index];

		if (ref->context == NULL) {
			continue;
		}

		previous_context = zend_opcache_static_cache_activate_context(ref->context);

		if (zend_opcache_static_cache_wlock()) {
			if (zend_opcache_static_cache_header_is_initialized_locked()) {
				zend_opcache_static_cache_free_locked(ref->payload_offset);
				freed = true;
			}

			zend_opcache_static_cache_unlock();
		}

		zend_opcache_static_cache_restore_context(previous_context);
	}

	efree(zend_opcache_static_cache_retired_shared_graphs);

	zend_opcache_static_cache_retired_shared_graphs = NULL;
	zend_opcache_static_cache_retired_shared_graph_count = 0;
	zend_opcache_static_cache_retired_shared_graph_capacity = 0;

	return freed;
}

bool zend_opcache_static_cache_calculate_shared_graph_size(
	const zval *value,
	size_t *buffer_len
)
{
	zend_opcache_static_cache_shared_graph_calc_context calc_context;
	bool result;

	if (!buffer_len) {
		return false;
	}

	*buffer_len = 0;
	if (Z_TYPE_P(value) == IS_OBJECT) {
		if (!zend_opcache_static_cache_shared_graph_can_restore_direct(Z_OBJCE_P(value))) {
			return false;
		}
	} else if (Z_TYPE_P(value) != IS_ARRAY) {
		return false;
	}

	zend_opcache_static_cache_shared_graph_calc_init(&calc_context);

	result = zend_opcache_static_cache_shared_graph_reserve_size(&calc_context.size, sizeof(zend_opcache_static_cache_shared_graph_header));
	if (result) {
		result = zend_opcache_static_cache_shared_graph_calc_value(&calc_context, value);
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

	zend_opcache_static_cache_shared_graph_calc_destroy(&calc_context);

	return result;
}

bool zend_opcache_static_cache_build_shared_graph_in_place(
	const zval *value,
	unsigned char *buffer,
	size_t buffer_len,
	size_t *graph_len
)
{
	zend_opcache_static_cache_shared_graph_copy_context copy_context;
	zend_opcache_static_cache_shared_graph_header *header;
	zend_opcache_static_cache_shared_graph_value root_value;
	uint32_t header_offset, root_offset, root_type;
	size_t padding;
	bool result;

	if (buffer == NULL) {
		return false;
	}

	padding = zend_opcache_static_cache_shared_graph_alignment_padding(buffer);
	if (padding > buffer_len || buffer_len - padding < sizeof(zend_opcache_static_cache_shared_graph_header)) {
		return false;
	}
	if (padding != 0) {
		memset(buffer, 0, padding);
	}

	buffer += padding;
	buffer_len -= padding;

	zend_opcache_static_cache_shared_graph_copy_init(&copy_context, buffer, buffer_len);
	root_offset = 0;
	root_type = 0;
	result = zend_opcache_static_cache_shared_graph_copy_alloc(&copy_context, sizeof(*header), &header_offset) && header_offset == 0;
	if (result) {
		if (Z_TYPE_P(value) == IS_OBJECT) {
			result = zend_opcache_static_cache_shared_graph_copy_property_value(&copy_context, value, &root_value) &&
						root_value.type == ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_VALUE_OBJECT
			;
		} else if (Z_TYPE_P(value) == IS_ARRAY) {
			result = zend_opcache_static_cache_shared_graph_copy_property_value(&copy_context, value, &root_value) &&
					(root_value.type == ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_VALUE_ARRAY ||
					root_value.type == ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_VALUE_DYNAMIC_ARRAY)
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
		zend_opcache_static_cache_shared_graph_copy_destroy(&copy_context);

		return false;
	}

	header = (zend_opcache_static_cache_shared_graph_header *) buffer;
	header->magic = ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_MAGIC;
	header->version = ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_VERSION;
	header->root_offset = root_offset;
	header->root_type = root_type;
	header->flags = copy_context.has_serialized_object
		? ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_FLAG_HAS_SERIALIZED_OBJECT
		: 0
	;

	ZEND_ATOMIC_INT_INIT(&header->ref_state, 0);

	if (graph_len != NULL) {
		*graph_len = copy_context.position;
	}

	zend_opcache_static_cache_shared_graph_copy_destroy(&copy_context);

	return true;
}

bool zend_opcache_static_cache_shared_graph_copy_fits_buffer(
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

	target_padding = zend_opcache_static_cache_shared_graph_alignment_padding(target_buffer);

	return target_padding <= target_buffer_len && source_graph_len <= target_buffer_len - target_padding;
}

bool zend_opcache_static_cache_fetch_shared_graph(
	const unsigned char *buffer,
	size_t buffer_len,
	zval *destination
)
{
	const zend_opcache_static_cache_shared_graph_header *header;
	const unsigned char *graph_buffer;
	zend_opcache_static_cache_shared_graph_value root_value;
	uint32_t root_type;
	bool capture_active, result;

	graph_buffer = zend_opcache_static_cache_shared_graph_locate_buffer(buffer, buffer_len, &buffer_len);
	if (graph_buffer == NULL) {
		return false;
	}

	buffer = graph_buffer;

	/* Invalidate the per-decode class memo: a buffer address can be recycled
	 * across payloads, so a stale (buffer, offset) entry must not be trusted. */
	zend_opcache_static_cache_decode_class_memo.buffer = NULL;

	header = (const zend_opcache_static_cache_shared_graph_header *) buffer;
	if (header->magic != ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_MAGIC ||
		header->version != ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_VERSION ||
		(header->root_offset != 0 && header->root_offset >= buffer_len)
	) {
		return false;
	}

	root_type = header->root_type != 0 ? header->root_type : ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_VALUE_OBJECT;
	memset(&root_value, 0, sizeof(root_value));
	root_value.type = (uint8_t) root_type;
	root_value.payload.offset = header->root_offset;
	capture_active = zend_opcache_static_cache_capture_active;
	if (capture_active) {
		zend_opcache_static_cache_capture_active = false;
	}

	switch (root_type) {
		case ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_VALUE_OBJECT:
			if (header->root_offset == 0) {
				if (capture_active) {
					zend_opcache_static_cache_capture_active = true;
				}

				return false;
			}

			result = zend_opcache_static_cache_fetch_shared_graph_value(buffer, &root_value, destination);
			break;
		case ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_VALUE_ARRAY:
		case ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_VALUE_DYNAMIC_ARRAY:
			result = zend_opcache_static_cache_fetch_shared_graph_value(buffer, &root_value, destination);
			break;
		default:
			if (capture_active) {
				zend_opcache_static_cache_capture_active = true;
			}

			return false;
	}

	if (capture_active) {
		zend_opcache_static_cache_capture_active = true;
		if (result) {
			zend_opcache_static_cache_capture_decoded_reachable_value(destination);
		}
	}

	return result;
}

static zend_always_inline bool zend_opcache_static_cache_shared_graph_pointer_in_range(
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

static zend_always_inline void *zend_opcache_static_cache_shared_graph_rebase_pointer(
		void *pointer,
		const unsigned char *old_base,
		size_t len,
		ptrdiff_t delta)
{
	if (!zend_opcache_static_cache_shared_graph_pointer_in_range(pointer, old_base, len)) {
		return pointer;
	}

	return (void *) ((char *) pointer - delta);
}

static bool zend_opcache_static_cache_shared_graph_rebase_direct_array(
	zend_array *array,
	const unsigned char *old_base,
	const unsigned char *new_base,
	size_t len,
	ptrdiff_t delta,
	HashTable *seen_arrays
);

static bool zend_opcache_static_cache_shared_graph_rebase_direct_zval(
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
			Z_STR_P(value) = (zend_string *) zend_opcache_static_cache_shared_graph_rebase_pointer(
				Z_STR_P(value),
				old_base,
				len,
				delta
			);
			return true;
		case IS_ARRAY:
			array = (zend_array *) zend_opcache_static_cache_shared_graph_rebase_pointer(
				Z_ARR_P(value),
				old_base,
				len,
				delta
			);

			Z_ARR_P(value) = array;

			if (!zend_opcache_static_cache_shared_graph_pointer_in_range(array, new_base, len)) {
				return true;
			}

			return zend_opcache_static_cache_shared_graph_rebase_direct_array(
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

static bool zend_opcache_static_cache_shared_graph_rebase_direct_array(
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

	if (!zend_opcache_static_cache_shared_graph_pointer_in_range(array, new_base, len)) {
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
	data = zend_opcache_static_cache_shared_graph_rebase_pointer(data, old_base, len, delta);
	HT_SET_DATA_ADDR(array, data);

	if (!zend_opcache_static_cache_shared_graph_pointer_in_range(data, new_base, len)) {
		return false;
	}

	if (HT_IS_PACKED(array)) {
		packed = array->arPacked;
		for (index = 0; index < array->nNumUsed; index++) {
			if (!zend_opcache_static_cache_shared_graph_rebase_direct_zval(
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
				bucket[index].key = (zend_string *) zend_opcache_static_cache_shared_graph_rebase_pointer(
					bucket[index].key,
					old_base,
					len,
					delta
				);

				if (!zend_opcache_static_cache_shared_graph_pointer_in_range(bucket[index].key, new_base, len)) {
					return false;
				}
			}

			if (!zend_opcache_static_cache_shared_graph_rebase_direct_zval(
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

static bool zend_opcache_static_cache_shared_graph_rebase_graph_value(
		const unsigned char *buffer,
		const zend_opcache_static_cache_shared_graph_value *value,
		const unsigned char *old_base,
		const unsigned char *new_base,
		size_t len,
		ptrdiff_t delta,
		HashTable *seen_arrays)
{
	const zend_opcache_static_cache_shared_graph_array *graph_array;
	const zend_opcache_static_cache_shared_graph_array_element *graph_elements;
	const zend_opcache_static_cache_shared_graph_object *graph_object;
	const zend_opcache_static_cache_shared_graph_property *properties;
	zend_array *array;
	uint32_t index;

	switch (value->type) {
		case ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_VALUE_ARRAY:
			if ((uint32_t) value->payload.offset == 0) {
				return true;
			}

			array = (zend_array *) (void *) (buffer + (uint32_t) value->payload.offset);

			return zend_opcache_static_cache_shared_graph_rebase_direct_array(
				array,
				old_base,
				new_base,
				len,
				delta,
				seen_arrays
			);
		case ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_VALUE_DYNAMIC_ARRAY:
			graph_array = (const zend_opcache_static_cache_shared_graph_array *) (buffer + (uint32_t) value->payload.offset);
			graph_elements = (const zend_opcache_static_cache_shared_graph_array_element *) (buffer + graph_array->elements_offset);

			for (index = 0; index < graph_array->count; index++) {
				if (!zend_opcache_static_cache_shared_graph_rebase_graph_value(
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
		case ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_VALUE_OBJECT:
			graph_object = (const zend_opcache_static_cache_shared_graph_object *) (buffer + (uint32_t) value->payload.offset);
			properties = (const zend_opcache_static_cache_shared_graph_property *) (buffer + graph_object->properties_offset);
			for (index = 0; index < graph_object->property_count; index++) {
				if (!zend_opcache_static_cache_shared_graph_rebase_graph_value(
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
		default:
			return true;
	}
}

bool zend_opcache_static_cache_shared_graph_can_overwrite_payload_locked(uint32_t payload_offset)
{
	return zend_opcache_static_cache_shared_graph_can_move_payload_locked(payload_offset);
}

bool zend_opcache_static_cache_shared_graph_can_move_payload_locked(uint32_t payload_offset)
{
	zend_opcache_static_cache_shared_graph_header *header = zend_opcache_static_cache_shared_graph_payload_header(payload_offset);

	if (header == NULL) {
		return false;
	}

	return zend_atomic_int_load_ex(&header->ref_state) == 0;
}

bool zend_opcache_static_cache_shared_graph_rebase_moved_payload_locked(uint32_t payload_offset, ptrdiff_t delta)
{
	const unsigned char *buffer, *old_buffer, *new_base, *old_base;
	zend_opcache_static_cache_shared_graph_header *header;
	zend_opcache_static_cache_shared_graph_value root_value;
	HashTable seen_arrays;
	uint32_t root_type;
	size_t buffer_len, graph_len, old_padding, new_padding;
	bool result;

	if (payload_offset == 0 || delta == 0) {
		return true;
	}

	buffer_len = zend_opcache_static_cache_block_payload_capacity(payload_offset);
	if (buffer_len == 0) {
		return false;
	}

	buffer = (const unsigned char *) zend_opcache_static_cache_ptr(payload_offset);
	old_buffer = buffer + delta;
	old_padding = zend_opcache_static_cache_shared_graph_alignment_padding(old_buffer);
	new_padding = zend_opcache_static_cache_shared_graph_alignment_padding(buffer);
	if (old_padding != new_padding) {
		return false;
	}

	buffer = zend_opcache_static_cache_shared_graph_locate_buffer(buffer, buffer_len, &graph_len);
	if (buffer == NULL) {
		return false;
	}

	header = (zend_opcache_static_cache_shared_graph_header *) buffer;
	if (header->magic != ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_MAGIC ||
		header->version != ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_VERSION ||
		(header->root_offset != 0 && header->root_offset >= graph_len)
	) {
		return false;
	}

	new_base = buffer;
	old_base = old_buffer + old_padding;
	root_type = header->root_type != 0 ? header->root_type : ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_VALUE_OBJECT;
	memset(&root_value, 0, sizeof(root_value));
	root_value.type = (uint8_t) root_type;
	root_value.payload.offset = header->root_offset;

	zend_hash_init(&seen_arrays, 8, NULL, NULL, 0);
	switch (root_type) {
		case ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_VALUE_ARRAY:
		case ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_VALUE_DYNAMIC_ARRAY:
			result = zend_opcache_static_cache_shared_graph_rebase_graph_value(
				buffer,
				&root_value,
				old_base,
				new_base,
				graph_len,
				delta,
				&seen_arrays
			);
			break;
		case ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_VALUE_OBJECT:
			result = header->root_offset != 0 &&
				zend_opcache_static_cache_shared_graph_rebase_graph_value(
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

bool zend_opcache_static_cache_shared_graph_acquire_locked(uint32_t payload_offset)
{
	zend_opcache_static_cache_shared_graph_header *header = zend_opcache_static_cache_shared_graph_payload_header(payload_offset);
	int state, refcount, expected;

	if (header == NULL) {
		return false;
	}

	for (;;) {
		state = zend_atomic_int_load_ex(&header->ref_state);
		refcount = state & ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_REFCOUNT_MASK;
		expected = state;

		if ((state & ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_RETIRED) != 0 ||
			refcount == ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_REFCOUNT_MASK
		) {
			return false;
		}

		if (zend_atomic_int_compare_exchange_ex(&header->ref_state, &expected, state + 1)) {
			return true;
		}
	}
}

bool zend_opcache_static_cache_shared_graph_retire_payload_locked(uint32_t payload_offset)
{
	zend_opcache_static_cache_shared_graph_header *header = zend_opcache_static_cache_shared_graph_payload_header(payload_offset);
	int state, refcount, expected;

	if (header == NULL) {
		return true;
	}

	for (;;) {
		state = zend_atomic_int_load_ex(&header->ref_state);
		refcount = state & ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_REFCOUNT_MASK;
		expected = state;

		if (refcount == 0) {
			return true;
		}

		if ((state & ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_RETIRED) != 0) {
			return false;
		}

		if (zend_atomic_int_compare_exchange_ex(&header->ref_state, &expected, state | ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_RETIRED)) {
			return false;
		}
	}
}

bool zend_opcache_static_cache_shared_graph_release_ref_locked(uint32_t payload_offset)
{
	zend_opcache_static_cache_shared_graph_header *header = zend_opcache_static_cache_shared_graph_payload_header(payload_offset);
	int state, refcount, expected, desired;

	if (header == NULL) {
		return false;
	}

	for (;;) {
		state = zend_atomic_int_load_ex(&header->ref_state);
		refcount = state & ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_REFCOUNT_MASK;
		expected = state;

		if (refcount == 0) {
			return false;
		}

		desired = (state & ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_RETIRED) | (refcount - 1);
		if (zend_atomic_int_compare_exchange_ex(&header->ref_state, &expected, desired)) {
			return (desired & ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_RETIRED) != 0 &&
					(desired & ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_REFCOUNT_MASK) == 0
			;
		}
	}
}

bool zend_opcache_static_cache_has_request_shared_graph_ref(uint32_t payload_offset)
{
	zend_opcache_static_cache_context *context;
	uint32_t index;

	context = zend_opcache_static_cache_active_context();
	for (index = 0; index < zend_opcache_static_cache_shared_graph_ref_count; index++) {
		if (zend_opcache_static_cache_shared_graph_refs[index].context == context &&
			zend_opcache_static_cache_shared_graph_refs[index].payload_offset == payload_offset
		) {
			return true;
		}
	}

	return false;
}

void zend_opcache_static_cache_register_shared_graph_ref(uint32_t payload_offset)
{
	if (zend_opcache_static_cache_shared_graph_ref_count == zend_opcache_static_cache_shared_graph_ref_capacity) {
		zend_opcache_static_cache_shared_graph_ref_capacity = zend_opcache_static_cache_shared_graph_ref_capacity == 0
			? 8
			: zend_opcache_static_cache_shared_graph_ref_capacity * 2
		;

		zend_opcache_static_cache_shared_graph_refs = erealloc(
			zend_opcache_static_cache_shared_graph_refs,
			sizeof(zend_opcache_static_cache_shared_graph_ref) * zend_opcache_static_cache_shared_graph_ref_capacity
		);
	}

	zend_opcache_static_cache_shared_graph_refs[zend_opcache_static_cache_shared_graph_ref_count].context = zend_opcache_static_cache_active_context();
	zend_opcache_static_cache_shared_graph_refs[zend_opcache_static_cache_shared_graph_ref_count].payload_offset = payload_offset;
	zend_opcache_static_cache_shared_graph_ref_count++;
}

void zend_opcache_static_cache_defer_retired_shared_graph_free(uint32_t payload_offset)
{
	zend_opcache_static_cache_shared_graph_ref *ref;

	if (zend_opcache_static_cache_retired_shared_graph_count == zend_opcache_static_cache_retired_shared_graph_capacity) {
		zend_opcache_static_cache_retired_shared_graph_capacity = zend_opcache_static_cache_retired_shared_graph_capacity == 0
			? 4
			: zend_opcache_static_cache_retired_shared_graph_capacity * 2
		;

		zend_opcache_static_cache_retired_shared_graphs = erealloc(
			zend_opcache_static_cache_retired_shared_graphs,
			sizeof(zend_opcache_static_cache_shared_graph_ref) * zend_opcache_static_cache_retired_shared_graph_capacity
		);
	}

	ref = &zend_opcache_static_cache_retired_shared_graphs[zend_opcache_static_cache_retired_shared_graph_count++];
	ref->context = zend_opcache_static_cache_active_context();
	ref->payload_offset = payload_offset;
}

bool zend_opcache_static_cache_release_request_shared_graph_refs(void)
{
	zend_opcache_static_cache_shared_graph_ref *ref;
	zend_opcache_static_cache_context *previous_context;
	uint32_t index;
	bool released = false;

	if (zend_opcache_static_cache_shared_graph_ref_count == 0) {
		return zend_opcache_static_cache_free_retired_shared_graphs();
	}

	for (index = 0; index < zend_opcache_static_cache_shared_graph_ref_count; index++) {
		ref = &zend_opcache_static_cache_shared_graph_refs[index];

		if (ref->context == NULL || !zend_opcache_static_cache_context_runtime(ref->context)->available) {
			continue;
		}

		previous_context = zend_opcache_static_cache_activate_context(ref->context);

		if (zend_opcache_static_cache_wlock()) {
			if (zend_opcache_static_cache_header_is_initialized_locked() &&
				ref->payload_offset != 0
			) {
				released = true;
				if (zend_opcache_static_cache_shared_graph_release_ref_locked(ref->payload_offset)) {
					zend_opcache_static_cache_free_locked(ref->payload_offset);
				}
			}

			zend_opcache_static_cache_unlock();
		}

		zend_opcache_static_cache_restore_context(previous_context);
	}

	efree(zend_opcache_static_cache_shared_graph_refs);

	zend_opcache_static_cache_shared_graph_refs = NULL;
	zend_opcache_static_cache_shared_graph_ref_count = 0;
	zend_opcache_static_cache_shared_graph_ref_capacity = 0;

	return zend_opcache_static_cache_free_retired_shared_graphs() || released;
}
