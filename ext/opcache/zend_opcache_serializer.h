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

#ifndef ZEND_OPCACHE_SERIALIZER_H
#define ZEND_OPCACHE_SERIALIZER_H

#include <string.h>

#include "Zend/zend_attributes.h"
#include "Zend/zend_closures.h"

#define ZEND_OPCACHE_SERIALIZER_ALIGN8(size) (((size) + 7UL) & ~7UL)

#if defined(_MSC_VER)
# define ZEND_OPCACHE_SERIALIZER_PREFETCH_R(ptr) ((void) 0)
#else
# define ZEND_OPCACHE_SERIALIZER_PREFETCH_R(ptr) __builtin_prefetch((ptr), 0, 3)
#endif

#define ZEND_OPCACHE_SERIALIZER_MAX_DEPTH 128

#define ZEND_OPCACHE_SERIALIZER_TYPE_UNDEF 0
#define ZEND_OPCACHE_SERIALIZER_TYPE_NULL 1
#define ZEND_OPCACHE_SERIALIZER_TYPE_FALSE 2
#define ZEND_OPCACHE_SERIALIZER_TYPE_TRUE 3
#define ZEND_OPCACHE_SERIALIZER_TYPE_LONG 4
#define ZEND_OPCACHE_SERIALIZER_TYPE_DOUBLE 5
#define ZEND_OPCACHE_SERIALIZER_TYPE_STRING 6
#define ZEND_OPCACHE_SERIALIZER_TYPE_ARRAY 7
#define ZEND_OPCACHE_SERIALIZER_TYPE_OBJECT 8

#define ZEND_OPCACHE_SERIALIZER_ARRAY_MAP 0x00
#define ZEND_OPCACHE_SERIALIZER_ARRAY_PACKED 0x01

#define ZEND_OPCACHE_STATIC_CACHE_DIRECT_CACHE_SAFE_ATTRIBUTE "opcache\\__directcachesafe"

#define ZEND_OPCACHE_SERIALIZER_OBJ_SERIALIZE 0x01
#define ZEND_OPCACHE_SERIALIZER_OBJ_PROPS 0x02
#define ZEND_OPCACHE_SERIALIZER_OBJ_PHP_SER 0x04
#define ZEND_OPCACHE_SERIALIZER_OBJ_SAFE_DIRECT 0x08

typedef struct _zend_opcache_serializer_hdr_t {
	uint8_t type;
	uint8_t flags;
	uint16_t _pad;
	uint32_t data_size;
} zend_opcache_serializer_hdr_t;

typedef struct _zend_opcache_serializer_wbuf_t {
	unsigned char *data;
	size_t len;
	size_t cap;
	uint32_t depth;
	bool failed_unstorable;
	HashTable visited_arrays;
	HashTable seen_objects;
} zend_opcache_serializer_wbuf_t;

typedef void (*zend_opcache_serializer_capture_func_t)(zval *value);

typedef struct _zend_opcache_serializer_rbuf_t {
	const unsigned char *data;
	zend_opcache_serializer_capture_func_t capture_decoded_value;
	zend_opcache_serializer_capture_func_t capture_decoded_reachable_value;
	size_t len;
	size_t pos;
	uint32_t depth;
	uint32_t capture_suppression;
	bool skip_decoded_value_capture;
} zend_opcache_serializer_rbuf_t;

static zend_always_inline void zend_opcache_serializer_capture_decoded_value(zend_opcache_serializer_rbuf_t *rb, zval *value)
{
	if (rb->capture_decoded_value != NULL) {
		rb->capture_decoded_value(value);
	}
}

static zend_always_inline void zend_opcache_serializer_capture_decoded_reachable_value(zend_opcache_serializer_rbuf_t *rb, zval *value)
{
	if (rb->capture_decoded_reachable_value != NULL) {
		rb->capture_decoded_reachable_value(value);
	}
}

static zend_always_inline bool zend_opcache_serializer_try_u32(size_t value, uint32_t *out)
{
	if (value > UINT32_MAX) {
		return false;
	}

	*out = (uint32_t) value;

	return true;
}

static zend_always_inline void zend_opcache_serializer_wbuf_init(zend_opcache_serializer_wbuf_t *wb, size_t initial_cap)
{
	wb->data = emalloc(initial_cap);
	wb->len = 0;
	wb->cap = initial_cap;
	wb->depth = 0;
	wb->failed_unstorable = false;

	zend_hash_init(&wb->visited_arrays, 8, NULL, NULL, 0);
	zend_hash_init(&wb->seen_objects, 8, NULL, NULL, 0);
}

static zend_always_inline void zend_opcache_serializer_wbuf_destroy(zend_opcache_serializer_wbuf_t *wb)
{
	zend_hash_destroy(&wb->visited_arrays);
	zend_hash_destroy(&wb->seen_objects);
}

static zend_always_inline void zend_opcache_serializer_wbuf_grow(zend_opcache_serializer_wbuf_t *wb, size_t need)
{
	size_t new_cap;

	if (wb->len + need <= wb->cap) {
		return;
	}

	new_cap = wb->cap;
	if (new_cap == 0) {
		new_cap = need;
	}

	while (new_cap < wb->len + need) {
		new_cap = new_cap + (new_cap >> 1);
	}

	wb->data = erealloc(wb->data, new_cap);
	wb->cap = new_cap;
}

static zend_always_inline void *zend_opcache_serializer_wbuf_reserve(zend_opcache_serializer_wbuf_t *wb, size_t size)
{
	size_t aligned_size;
	void *ptr;

	aligned_size = ZEND_OPCACHE_SERIALIZER_ALIGN8(size);
	zend_opcache_serializer_wbuf_grow(wb, aligned_size);
	ptr = wb->data + wb->len;
	wb->len += aligned_size;

	return ptr;
}

static zend_always_inline void zend_opcache_serializer_wbuf_zero_padding(void *ptr, size_t size)
{
	size_t aligned_size = ZEND_OPCACHE_SERIALIZER_ALIGN8(size);

	if (aligned_size > size) {
		memset((unsigned char *) ptr + size, 0, aligned_size - size);
	}
}

static zend_always_inline void zend_opcache_serializer_wbuf_write(zend_opcache_serializer_wbuf_t *wb, const void *src, size_t size)
{
	void *dst;

	dst = zend_opcache_serializer_wbuf_reserve(wb, size);
	memcpy(dst, src, size);
	zend_opcache_serializer_wbuf_zero_padding(dst, size);
}

static zend_always_inline zend_opcache_serializer_hdr_t *zend_opcache_serializer_wbuf_write_hdr(
		zend_opcache_serializer_wbuf_t *wb, uint8_t type, uint8_t flags, uint32_t data_size)
{
	zend_opcache_serializer_hdr_t *hdr;

	hdr = (zend_opcache_serializer_hdr_t *) zend_opcache_serializer_wbuf_reserve(wb, sizeof(zend_opcache_serializer_hdr_t));
	hdr->type = type;
	hdr->flags = flags;
	hdr->_pad = 0;
	hdr->data_size = data_size;

	return hdr;
}

static zend_always_inline bool zend_opcache_serializer_patch_hdr(zend_opcache_serializer_wbuf_t *wb, size_t hdr_offset)
{
	zend_opcache_serializer_hdr_t *hdr;
	uint32_t data_size;
	size_t payload_size;

	if (wb->len < hdr_offset + ZEND_OPCACHE_SERIALIZER_ALIGN8(sizeof(zend_opcache_serializer_hdr_t))) {
		return false;
	}

	payload_size = wb->len - hdr_offset - ZEND_OPCACHE_SERIALIZER_ALIGN8(sizeof(zend_opcache_serializer_hdr_t));
	if (!zend_opcache_serializer_try_u32(payload_size, &data_size)) {
		return false;
	}

	hdr = (zend_opcache_serializer_hdr_t *) (wb->data + hdr_offset);
	hdr->data_size = data_size;

	return true;
}

static zend_always_inline bool zend_opcache_serializer_write_bytes_entry(zend_opcache_serializer_wbuf_t *wb,
		const char *bytes, uint32_t byte_len)
{
	uint32_t reserved = 0;
	size_t entry_size;
	unsigned char *dst;

	entry_size = sizeof(uint32_t) + sizeof(uint32_t) + (size_t) byte_len + 1;
	if (entry_size > UINT32_MAX) {
		return false;
	}

	dst = (unsigned char *) zend_opcache_serializer_wbuf_reserve(wb, entry_size);
	memcpy(dst, &byte_len, sizeof(uint32_t));
	memcpy(dst + sizeof(uint32_t), &reserved, sizeof(uint32_t));
	memcpy(dst + sizeof(uint32_t) + sizeof(uint32_t), bytes, byte_len);
	dst[sizeof(uint32_t) + sizeof(uint32_t) + byte_len] = '\0';
	zend_opcache_serializer_wbuf_zero_padding(dst, entry_size);

	return true;
}

static zend_always_inline bool zend_opcache_serializer_encode_string(zend_opcache_serializer_wbuf_t *wb,
		const zend_string *str)
{
	uint32_t string_len;
	size_t payload_size;

	if (!zend_opcache_serializer_try_u32(ZSTR_LEN(str), &string_len)) {
		return false;
	}

	payload_size = sizeof(uint32_t) + sizeof(uint32_t) + (size_t) string_len + 1;
	if (payload_size > UINT32_MAX) {
		return false;
	}

	zend_opcache_serializer_wbuf_write_hdr(wb, ZEND_OPCACHE_SERIALIZER_TYPE_STRING, 0, (uint32_t) payload_size);

	return zend_opcache_serializer_write_bytes_entry(wb, ZSTR_VAL(str), string_len);
}

static zend_always_inline bool zend_opcache_serializer_write_string_key_entry(zend_opcache_serializer_wbuf_t *wb,
		const char *key_str, uint32_t key_len)
{
	uint32_t is_string = 1;
	size_t entry_size;
	unsigned char *dst;

	entry_size = 2 * sizeof(uint32_t) + (size_t) key_len + 1;
	if (entry_size > UINT32_MAX) {
		return false;
	}

	dst = (unsigned char *) zend_opcache_serializer_wbuf_reserve(wb, entry_size);
	memcpy(dst, &is_string, sizeof(uint32_t));
	memcpy(dst + sizeof(uint32_t), &key_len, sizeof(uint32_t));
	memcpy(dst + 2 * sizeof(uint32_t), key_str, key_len);
	dst[2 * sizeof(uint32_t) + key_len] = '\0';
	zend_opcache_serializer_wbuf_zero_padding(dst, entry_size);

	return true;
}

static zend_always_inline bool zend_opcache_serializer_enter_array(zend_opcache_serializer_wbuf_t *wb, const HashTable *ht)
{
	zend_ulong key;
	zval tmp;

	key = (zend_ulong) (uintptr_t) ht;
	if (zend_hash_index_exists(&wb->visited_arrays, key)) {
		return false;
	}

	ZVAL_NULL(&tmp);
	zend_hash_index_add_new(&wb->visited_arrays, key, &tmp);

	return true;
}

static zend_always_inline void zend_opcache_serializer_leave_array(zend_opcache_serializer_wbuf_t *wb, const HashTable *ht)
{
	zend_hash_index_del(&wb->visited_arrays, (zend_ulong) (uintptr_t) ht);
}

static zend_always_inline bool zend_opcache_serializer_mark_object(zend_opcache_serializer_wbuf_t *wb, const zval *zv)
{
	zend_ulong key;
	zval tmp;

	key = (zend_ulong) (uintptr_t) Z_OBJ_P(zv);
	if (zend_hash_index_exists(&wb->seen_objects, key)) {
		return false;
	}

	ZVAL_NULL(&tmp);
	zend_hash_index_add_new(&wb->seen_objects, key, &tmp);

	return true;
}

static zend_always_inline size_t zend_opcache_serializer_sleep_property_count(HashTable *sleep_ht)
{
	zval *sleep_entry;
	size_t count = 0;

	ZEND_HASH_FOREACH_VAL(sleep_ht, sleep_entry) {
		if (Z_TYPE_P(sleep_entry) == IS_STRING) {
			count++;
		}
	} ZEND_HASH_FOREACH_END();

	return count;
}

static zend_always_inline zval *zend_opcache_serializer_find_sleep_property(HashTable *props,
		zend_class_entry *ce, zend_string *sleep_name, zend_string **resolved_name_out,
		bool *resolved_name_owned_out)
{
	zend_property_info *prop_info;
	zend_string *mangled_name;
	zval *found_val;

	found_val = zend_hash_find(props, sleep_name);
	if (found_val != NULL) {
		*resolved_name_out = sleep_name;
		*resolved_name_owned_out = false;

		return found_val;
	}

	prop_info = zend_hash_find_ptr(&ce->properties_info, sleep_name);
	if (prop_info != NULL) {
		if ((prop_info->flags & ZEND_ACC_PRIVATE) != 0) {
			mangled_name = zend_mangle_property_name(ZSTR_VAL(prop_info->ce->name),
				ZSTR_LEN(prop_info->ce->name), ZSTR_VAL(sleep_name), ZSTR_LEN(sleep_name), 0)
			;
		} else if ((prop_info->flags & ZEND_ACC_PROTECTED) != 0) {
			mangled_name = zend_mangle_property_name("*", 1,
				ZSTR_VAL(sleep_name), ZSTR_LEN(sleep_name), 0)
			;
		} else {
			mangled_name = NULL;
		}

		if (mangled_name != NULL) {
			found_val = zend_hash_find(props, mangled_name);
			if (found_val != NULL) {
				*resolved_name_out = mangled_name;
				*resolved_name_owned_out = true;

				return found_val;
			}

			zend_string_release(mangled_name);
		}
	}

	mangled_name = zend_mangle_property_name(ZSTR_VAL(ce->name), ZSTR_LEN(ce->name),
		ZSTR_VAL(sleep_name), ZSTR_LEN(sleep_name), 0)
	;

	found_val = zend_hash_find(props, mangled_name);
	if (found_val != NULL) {
		*resolved_name_out = mangled_name;
		*resolved_name_owned_out = true;

		return found_val;
	}

	zend_string_release(mangled_name);

	mangled_name = zend_mangle_property_name("*", 1,
		ZSTR_VAL(sleep_name), ZSTR_LEN(sleep_name), 0)
	;

	found_val = zend_hash_find(props, mangled_name);
	if (found_val != NULL) {
		*resolved_name_out = mangled_name;
		*resolved_name_owned_out = true;

		return found_val;
	}

	zend_string_release(mangled_name);

	*resolved_name_out = sleep_name;
	*resolved_name_owned_out = false;

	return NULL;
}

static zend_always_inline bool zend_opcache_serializer_safe_direct_cache_has_attribute(const HashTable *attributes)
{
	return attributes != NULL &&
		zend_get_attribute_str(attributes, ZEND_STRL(ZEND_OPCACHE_STATIC_CACHE_DIRECT_CACHE_SAFE_ATTRIBUTE)) != NULL
	;
}

static zend_always_inline zend_class_entry *zend_opcache_serializer_find_safe_direct_cache_base(zend_class_entry *ce)
{
	zend_class_entry *base_ce = NULL;

	if (zend_opcache_static_cache_safe_direct_copy_func(ce, &base_ce) == NULL) {
		return NULL;
	}

	return base_ce;
}

static zend_always_inline bool zend_opcache_serializer_class_magic_method_changed(zend_class_entry *ce,
		zend_class_entry *base_ce, const char *name, size_t name_len)
{
	zend_function *func = zend_hash_str_find_ptr(&ce->function_table, name, name_len);
	zend_function *base_func = zend_hash_str_find_ptr(&base_ce->function_table, name, name_len);

	if (func == NULL) {
		return base_func != NULL;
	}

	if (func == base_func || func->common.scope == base_ce) {
		return false;
	}

	return true;
}

static zend_always_inline bool zend_opcache_serializer_safe_direct_cache_allows_custom_serializers(
		zend_class_entry *base_ce)
{
	return zend_opcache_static_cache_safe_direct_allows_custom_serializers(base_ce);
}

static zend_always_inline bool zend_opcache_serializer_has_safe_direct_cache_overrides(zend_class_entry *ce,
		zend_class_entry *base_ce)
{
	if (ce == base_ce) {
		return false;
	}

	if ((ce->__serialize != base_ce->__serialize || ce->__unserialize != base_ce->__unserialize) &&
		!zend_opcache_serializer_safe_direct_cache_allows_custom_serializers(base_ce)
	) {
		return true;
	}

	return zend_opcache_serializer_class_magic_method_changed(ce, base_ce, ZEND_STRL("__sleep")) ||
		zend_opcache_serializer_class_magic_method_changed(ce, base_ce, ZEND_STRL("__wakeup"))
	;
}

static bool zend_opcache_serializer_value_has_unstorable_ex(
		const zval *value,
		HashTable *seen_arrays,
		HashTable *seen_objects);

typedef struct _zend_opcache_serializer_has_unstorable_context {
	HashTable *seen_arrays;
	HashTable *seen_objects;
} zend_opcache_serializer_has_unstorable_context;

static bool zend_opcache_serializer_safe_direct_value_has_unstorable_callback(
		void *context,
		const zval *value)
{
	zend_opcache_serializer_has_unstorable_context *has_unstorable_context =
		(zend_opcache_serializer_has_unstorable_context *) context;

	return zend_opcache_serializer_value_has_unstorable_ex(
		value,
		has_unstorable_context->seen_arrays,
		has_unstorable_context->seen_objects
	);
}

static bool zend_opcache_serializer_hash_has_unstorable_ex(
		const HashTable *hash,
		HashTable *seen_arrays,
		HashTable *seen_objects)
{
	zval *element;

	if (hash == NULL) {
		return false;
	}

	ZEND_HASH_FOREACH_VAL((HashTable *) hash, element) {
		if (Z_TYPE_P(element) == IS_INDIRECT) {
			element = Z_INDIRECT_P(element);
			if (Z_TYPE_P(element) == IS_UNDEF) {
				continue;
			}
		}

		if (zend_opcache_serializer_value_has_unstorable_ex(element, seen_arrays, seen_objects)) {
			return true;
		}
	} ZEND_HASH_FOREACH_END();

	return false;
}

static bool zend_opcache_serializer_safe_direct_state_has_unstorable(
		const zval *value,
		HashTable *seen_arrays,
		HashTable *seen_objects)
{
	zend_class_entry *base_ce = NULL;
	zend_opcache_static_cache_safe_direct_state_copy_func_t copy_func;
	zend_opcache_static_cache_safe_direct_state_has_unstorable_func_t state_has_unstorable_func;
	zend_opcache_serializer_has_unstorable_context context;

	copy_func = zend_opcache_static_cache_safe_direct_copy_func(Z_OBJCE_P(value), &base_ce);
	if (copy_func == NULL ||
		zend_opcache_serializer_has_safe_direct_cache_overrides(Z_OBJCE_P(value), base_ce)
	) {
		return false;
	}

	state_has_unstorable_func =
		zend_opcache_static_cache_safe_direct_state_has_unstorable_func(Z_OBJCE_P(value));
	if (state_has_unstorable_func == NULL) {
		return false;
	}

	context.seen_arrays = seen_arrays;
	context.seen_objects = seen_objects;

	return state_has_unstorable_func(
		&context,
		value,
		zend_opcache_serializer_safe_direct_value_has_unstorable_callback
	);
}

static bool zend_opcache_serializer_value_has_unstorable_ex(
		const zval *value,
		HashTable *seen_arrays,
		HashTable *seen_objects)
{
	zend_object *object;
	zend_ulong key;
	zval *property, *end;

	ZVAL_DEREF(value);
	switch (Z_TYPE_P(value)) {
		case IS_RESOURCE:
			return true;
		case IS_OBJECT:
			if (Z_OBJCE_P(value) == zend_ce_closure) {
				return true;
			}

			object = Z_OBJ_P(value);
			key = (zend_ulong) (uintptr_t) object;
			if (zend_hash_index_exists(seen_objects, key)) {
				return false;
			}
			zend_hash_index_add_empty_element(seen_objects, key);

			if (zend_opcache_serializer_safe_direct_state_has_unstorable(value, seen_arrays, seen_objects)) {
				return true;
			}

			if (object->ce->default_properties_count != 0) {
				property = object->properties_table;
				end = property + object->ce->default_properties_count;
				do {
					if (Z_TYPE_P(property) != IS_UNDEF &&
						zend_opcache_serializer_value_has_unstorable_ex(property, seen_arrays, seen_objects)
					) {
						return true;
					}
					property++;
				} while (property != end);
			}

			return object->properties != NULL &&
				zend_opcache_serializer_hash_has_unstorable_ex(object->properties, seen_arrays, seen_objects);
		case IS_ARRAY:
			key = (zend_ulong) (uintptr_t) Z_ARR_P(value);
			if (zend_hash_index_exists(seen_arrays, key)) {
				return false;
			}
			zend_hash_index_add_empty_element(seen_arrays, key);

			return zend_opcache_serializer_hash_has_unstorable_ex(Z_ARRVAL_P(value), seen_arrays, seen_objects);
		default:
			return false;
	}
}

static zend_always_inline bool zend_opcache_serializer_value_has_unstorable(const zval *value)
{
	HashTable seen_arrays, seen_objects;
	bool result;

	zend_hash_init(&seen_arrays, 8, NULL, NULL, 0);
	zend_hash_init(&seen_objects, 8, NULL, NULL, 0);
	result = zend_opcache_serializer_value_has_unstorable_ex(value, &seen_arrays, &seen_objects);
	zend_hash_destroy(&seen_objects);
	zend_hash_destroy(&seen_arrays);

	return result;
}

static zend_always_inline bool zend_opcache_serializer_hash_has_unstorable(const HashTable *hash)
{
	HashTable seen_arrays, seen_objects;
	bool result;

	if (hash == NULL) {
		return false;
	}

	zend_hash_init(&seen_arrays, 8, NULL, NULL, 0);
	zend_hash_init(&seen_objects, 8, NULL, NULL, 0);
	result = zend_opcache_serializer_hash_has_unstorable_ex(hash, &seen_arrays, &seen_objects);
	zend_hash_destroy(&seen_objects);
	zend_hash_destroy(&seen_arrays);

	return result;
}

static zend_always_inline bool zend_opcache_serializer_encode_zval(zend_opcache_serializer_wbuf_t *wb,
		const zval *zv);

static zend_always_inline bool zend_opcache_serializer_encode_property_table(zend_opcache_serializer_wbuf_t *wb,
		const HashTable *props)
{
	zend_string *prop_key;
	zval *prop_val;
	uint32_t prop_count, arr_meta[2] = {0, 0}, key_len;
	size_t prop_count_tmp = 0, arr_hdr_offset;

	ZEND_HASH_FOREACH_STR_KEY_VAL(props, prop_key, prop_val) {
		if (prop_key != NULL) {
			prop_count_tmp++;
		}
	} ZEND_HASH_FOREACH_END();

	if (!zend_opcache_serializer_try_u32(prop_count_tmp, &prop_count)) {
		return false;
	}

	arr_hdr_offset = wb->len;
	zend_opcache_serializer_wbuf_write_hdr(wb, ZEND_OPCACHE_SERIALIZER_TYPE_ARRAY,
		ZEND_OPCACHE_SERIALIZER_ARRAY_MAP, 0);
	arr_meta[0] = prop_count;
	zend_opcache_serializer_wbuf_write(wb, arr_meta, sizeof(arr_meta));

	ZEND_HASH_FOREACH_STR_KEY_VAL(props, prop_key, prop_val) {
		if (prop_key == NULL) {
			continue;
		}

		if (Z_TYPE_P(prop_val) == IS_INDIRECT) {
			prop_val = Z_INDIRECT_P(prop_val);
		}

		if (!zend_opcache_serializer_try_u32(ZSTR_LEN(prop_key), &key_len) ||
			!zend_opcache_serializer_write_string_key_entry(wb, ZSTR_VAL(prop_key), key_len)
		) {
			return false;
		}

		if (Z_TYPE_P(prop_val) == IS_UNDEF) {
			zend_opcache_serializer_wbuf_write_hdr(wb, ZEND_OPCACHE_SERIALIZER_TYPE_UNDEF, 0, 0);
		} else if (!zend_opcache_serializer_encode_zval(wb, prop_val)) {
			return false;
		}
	} ZEND_HASH_FOREACH_END();

	return zend_opcache_serializer_patch_hdr(wb, arr_hdr_offset);
}

static zend_always_inline bool zend_opcache_serializer_update_object_property(zend_object *object,
		zend_string *key, zval *prop_val)
{
	const char *class_name, *prop_name;
	zend_string *cname;
	zend_class_entry *ce;
	size_t prop_name_len;

	if (ZSTR_LEN(key) > 0 && ZSTR_VAL(key)[0] == '\0') {
		if (zend_unmangle_property_name_ex(key, &class_name, &prop_name, &prop_name_len) == SUCCESS) {
			if (class_name[0] != '*') {
				cname = zend_string_init(class_name, strlen(class_name), 0);
				ce = zend_lookup_class(cname);
				if (ce != NULL) {
					zend_update_property(ce, object, prop_name, prop_name_len, prop_val);
				}

				zend_string_release_ex(cname, 0);
			} else {
				zend_update_property(object->ce, object, prop_name, prop_name_len, prop_val);
			}
		}
	} else {
		zend_update_property(object->ce, object, ZSTR_VAL(key), ZSTR_LEN(key), prop_val);
	}

	return !EG(exception);
}

static zend_always_inline bool zend_opcache_serializer_restore_object_properties(zval *object, const HashTable *props)
{
	zend_string *prop_name;
	zval *prop_val;

	ZEND_HASH_FOREACH_STR_KEY_VAL(props, prop_name, prop_val) {
		if (prop_name == NULL || Z_TYPE_P(prop_val) == IS_REFERENCE) {
			continue;
		}

		if (!zend_opcache_serializer_update_object_property(Z_OBJ_P(object), prop_name, prop_val)) {
			return false;
		}
	} ZEND_HASH_FOREACH_END();

	return true;
}

static zend_always_inline bool zend_opcache_serializer_encode_safe_direct_state_payload(
		zend_opcache_serializer_wbuf_t *wb, zval *state_zv, const HashTable *props)
{
	bool has_unstorable_state, has_unstorable_props, ok;

	has_unstorable_state = zend_opcache_serializer_value_has_unstorable(state_zv);
	has_unstorable_props = zend_opcache_serializer_hash_has_unstorable(props);
	if (has_unstorable_state || has_unstorable_props) {
		wb->failed_unstorable = true;
		ok = false;
	} else {
		ok = zend_opcache_serializer_encode_zval(wb, state_zv) &&
			zend_opcache_serializer_encode_property_table(wb, props);
	}

	zval_ptr_dtor(state_zv);
	ZVAL_UNDEF(state_zv);

	return ok;
}

static zend_always_inline bool zend_opcache_serializer_encode_safe_direct_registered_payload(
		zend_opcache_serializer_wbuf_t *wb, const zval *zv)
{
	zend_opcache_static_cache_safe_direct_state_serialize_func_t serialize_func;
	zval state_zv;
	HashTable *props;

	serialize_func = zend_opcache_static_cache_safe_direct_state_serialize_func(Z_OBJCE_P(zv));
	if (serialize_func == NULL) {
		return false;
	}

	ZVAL_UNDEF(&state_zv);
	if (!serialize_func(zv, &state_zv) || Z_TYPE(state_zv) != IS_ARRAY) {
		if (Z_TYPE(state_zv) != IS_UNDEF) {
			zval_ptr_dtor(&state_zv);
		}

		return false;
	}

	props = zend_std_get_properties(Z_OBJ_P(zv));

	return zend_opcache_serializer_encode_safe_direct_state_payload(wb, &state_zv, props);
}

static zend_always_inline bool zend_opcache_serializer_try_encode_safe_direct_object(
		zend_opcache_serializer_wbuf_t *wb, const zval *zv, size_t hdr_offset,
		zend_class_entry *ce, zend_string *class_name, uint32_t name_len)
{
	zend_class_entry *base_ce = NULL;
	zend_opcache_static_cache_safe_direct_state_copy_func_t copy_func;
	bool ok;

	copy_func = zend_opcache_static_cache_safe_direct_copy_func(ce, &base_ce);
	if (copy_func == NULL ||
		zend_opcache_serializer_has_safe_direct_cache_overrides(ce, base_ce)
	) {
		return false;
	}

	zend_opcache_serializer_wbuf_write_hdr(wb, ZEND_OPCACHE_SERIALIZER_TYPE_OBJECT,
		ZEND_OPCACHE_SERIALIZER_OBJ_SAFE_DIRECT, 0);

	ok = zend_opcache_serializer_write_bytes_entry(wb, ZSTR_VAL(class_name), name_len);
	if (ok) {
		ok = zend_opcache_serializer_encode_safe_direct_registered_payload(wb, zv);
	}

	if (!ok) {
		if (EG(exception)) {
			return false;
		}

		wb->len = hdr_offset;

		return false;
	}

	return zend_opcache_serializer_patch_hdr(wb, hdr_offset);
}

static zend_always_inline bool zend_opcache_serializer_array_can_use_packed(const HashTable *ht)
{
	zend_string *key;
	zend_ulong expected_idx = 0, h;
	zval *val;

	if (!HT_IS_PACKED(ht)) {
		return false;
	}

	ZEND_HASH_FOREACH_KEY_VAL((HashTable *) ht, h, key, val) {
		(void) val;

		if (key != NULL || h != expected_idx++) {
			return false;
		}
	} ZEND_HASH_FOREACH_END();

	return true;
}

static bool zend_opcache_serializer_encode_array(zend_opcache_serializer_wbuf_t *wb,
		const HashTable *ht)
{
	zend_opcache_serializer_hdr_t *hdr;
	zend_ulong h;
	zend_string *key;
	zval *val;
	uint64_t idx_value;
	uint32_t count, arr_meta[2], key_len;
	uint8_t flags;
	size_t hdr_offset;
	bool use_packed;
	unsigned char key_buf[16];

	if (!zend_opcache_serializer_try_u32(zend_hash_num_elements(ht), &count)) {
		return false;
	}

	if (ht->nNextFreeElement > UINT32_MAX) {
		return false;
	}

	if (!zend_opcache_serializer_enter_array(wb, ht)) {
		return false;
	}

	use_packed = zend_opcache_serializer_array_can_use_packed(ht);
	flags = use_packed ? ZEND_OPCACHE_SERIALIZER_ARRAY_PACKED : ZEND_OPCACHE_SERIALIZER_ARRAY_MAP;
	hdr_offset = wb->len;
	hdr = zend_opcache_serializer_wbuf_write_hdr(wb, ZEND_OPCACHE_SERIALIZER_TYPE_ARRAY, flags, 0);
	ZEND_ASSERT(hdr != NULL);

	arr_meta[0] = count;
	arr_meta[1] = (uint32_t) ht->nNextFreeElement;
	zend_opcache_serializer_wbuf_write(wb, arr_meta, sizeof(arr_meta));

	if (use_packed) {
		ZEND_HASH_PACKED_FOREACH_VAL((HashTable *) ht, val) {
			if (!zend_opcache_serializer_encode_zval(wb, val)) {
				zend_opcache_serializer_leave_array(wb, ht);

				return false;
			}
		} ZEND_HASH_FOREACH_END();
	} else {
		ZEND_HASH_FOREACH_KEY_VAL((HashTable *) ht, h, key, val) {
			if (key != NULL) {
				if (!zend_opcache_serializer_try_u32(ZSTR_LEN(key), &key_len) ||
					!zend_opcache_serializer_write_string_key_entry(wb, ZSTR_VAL(key), key_len)
				) {
					zend_opcache_serializer_leave_array(wb, ht);

					return false;
				}
			} else {
				memset(key_buf, 0, sizeof(key_buf));
				idx_value = (uint64_t) h;
				memcpy(key_buf + 8, &idx_value, sizeof(uint64_t));

				zend_opcache_serializer_wbuf_write(wb, key_buf, sizeof(key_buf));
			}

			if (!zend_opcache_serializer_encode_zval(wb, val)) {
				zend_opcache_serializer_leave_array(wb, ht);

				return false;
			}
		} ZEND_HASH_FOREACH_END();
	}

	zend_opcache_serializer_leave_array(wb, ht);

	return zend_opcache_serializer_patch_hdr(wb, hdr_offset);
}

static bool zend_opcache_serializer_encode_object(zend_opcache_serializer_wbuf_t *wb,
		const zval *zv)
{
	zend_class_entry *ce;
	zend_string *class_name, *resolved_name, *prop_key;
	zval retval, sleep_rv, func_name, *sleep_entry, *found_val, *prop_val;
	php_serialize_data_t var_hash;
	HashTable *props, *sleep_ht;
	smart_str ser = {0};
	uint32_t name_len, prop_count, arr_meta[2] = {0, 0}, key_len, ser_len;
	size_t hdr_offset, arr_hdr_offset, property_count;
	bool has_sleep, resolved_name_owned;
	int call_ret;

	ce = Z_OBJCE_P(zv);

	class_name = ce->name;
	if (!zend_opcache_serializer_try_u32(ZSTR_LEN(class_name), &name_len)) {
		return false;
	}

	if (!zend_opcache_serializer_mark_object(wb, zv)) {
		return false;
	}

	hdr_offset = wb->len;
	ZVAL_UNDEF(&retval);
	if (zend_opcache_serializer_try_encode_safe_direct_object(wb, zv, hdr_offset, ce, class_name, name_len)) {
		return true;
	}

	if (EG(exception)) {
		return false;
	}

	if (wb->failed_unstorable) {
		wb->len = hdr_offset;

		return false;
	}

	if (ce->__serialize != NULL) {
		zend_call_known_instance_method_with_0_params(ce->__serialize, Z_OBJ_P(zv), &retval);
		if (EG(exception)) {
			if (Z_TYPE(retval) != IS_UNDEF) {
				zval_ptr_dtor(&retval);
			}

			return false;
		}

		if (Z_TYPE(retval) == IS_ARRAY) {
			if (zend_opcache_serializer_value_has_unstorable(&retval)) {
				wb->failed_unstorable = true;
				zval_ptr_dtor(&retval);
				wb->len = hdr_offset;

				return false;
			}

			zend_opcache_serializer_wbuf_write_hdr(wb, ZEND_OPCACHE_SERIALIZER_TYPE_OBJECT,
				ZEND_OPCACHE_SERIALIZER_OBJ_SERIALIZE, 0);
			if (!zend_opcache_serializer_write_bytes_entry(wb, ZSTR_VAL(class_name), name_len) ||
				!zend_opcache_serializer_encode_array(wb, Z_ARRVAL(retval))
			) {
				zval_ptr_dtor(&retval);

				if (EG(exception)) {
					return false;
				}

				if (wb->failed_unstorable) {
					wb->len = hdr_offset;

					return false;
				}

				wb->len = hdr_offset;

				goto fallback_php;
			}

			zval_ptr_dtor(&retval);

			return zend_opcache_serializer_patch_hdr(wb, hdr_offset);
		}

		if (Z_TYPE(retval) != IS_UNDEF) {
			zval_ptr_dtor(&retval);
		}

		wb->len = hdr_offset;

		goto fallback_php;
	}

	props = NULL;
	sleep_ht = NULL;
	has_sleep = false;
	ZVAL_UNDEF(&sleep_rv);

	if (zend_hash_str_exists(&ce->function_table, "__sleep", sizeof("__sleep") - 1)) {
		ZVAL_STRINGL(&func_name, "__sleep", sizeof("__sleep") - 1);
		call_ret = call_user_function(CG(function_table), (zval *) zv, &func_name, &sleep_rv, 0, NULL);
		zval_ptr_dtor_str(&func_name);

		if (EG(exception)) {
			if (Z_TYPE(sleep_rv) != IS_UNDEF) {
				zval_ptr_dtor(&sleep_rv);
			}

			return false;
		}

		if (call_ret == SUCCESS && Z_TYPE(sleep_rv) == IS_ARRAY) {
			has_sleep = true;
			sleep_ht = Z_ARRVAL(sleep_rv);
		} else if (Z_TYPE(sleep_rv) != IS_UNDEF) {
			zval_ptr_dtor(&sleep_rv);
			ZVAL_UNDEF(&sleep_rv);
		}
	}

	props = zend_get_properties_for((zval *) zv, ZEND_PROP_PURPOSE_SERIALIZE);
	if (props != NULL) {
		property_count = has_sleep
			? zend_opcache_serializer_sleep_property_count(sleep_ht)
			: zend_hash_num_elements(props)
		;
		if (!zend_opcache_serializer_try_u32(property_count, &prop_count)) {
			zend_release_properties(props);

			if (has_sleep) {
				zval_ptr_dtor(&sleep_rv);
			}

			return false;
		}

		if (!has_sleep && zend_opcache_serializer_hash_has_unstorable(props)) {
			wb->failed_unstorable = true;
			zend_release_properties(props);
			wb->len = hdr_offset;

			return false;
		}

		zend_opcache_serializer_wbuf_write_hdr(wb, ZEND_OPCACHE_SERIALIZER_TYPE_OBJECT,
			ZEND_OPCACHE_SERIALIZER_OBJ_PROPS, 0);
		if (!zend_opcache_serializer_write_bytes_entry(wb, ZSTR_VAL(class_name), name_len)) {
			zend_release_properties(props);

			if (has_sleep) {
				zval_ptr_dtor(&sleep_rv);
			}

			return false;
		}

		arr_hdr_offset = wb->len;
		zend_opcache_serializer_wbuf_write_hdr(wb, ZEND_OPCACHE_SERIALIZER_TYPE_ARRAY,
			ZEND_OPCACHE_SERIALIZER_ARRAY_MAP, 0
		);
		arr_meta[0] = prop_count;
		zend_opcache_serializer_wbuf_write(wb, arr_meta, sizeof(arr_meta));

		if (has_sleep) {
			ZEND_HASH_FOREACH_VAL(sleep_ht, sleep_entry) {
				if (Z_TYPE_P(sleep_entry) != IS_STRING) {
					continue;
				}

				resolved_name = Z_STR_P(sleep_entry);
				resolved_name_owned = false;
				found_val = zend_opcache_serializer_find_sleep_property(props, ce,
					Z_STR_P(sleep_entry), &resolved_name, &resolved_name_owned)
				;

				if (!zend_opcache_serializer_try_u32(ZSTR_LEN(resolved_name), &key_len) ||
					!zend_opcache_serializer_write_string_key_entry(wb, ZSTR_VAL(resolved_name), key_len)
				) {
					if (resolved_name_owned) {
						zend_string_release(resolved_name);
					}

					zend_release_properties(props);
					zval_ptr_dtor(&sleep_rv);

					return false;
				}

				if (resolved_name_owned) {
					zend_string_release(resolved_name);
				}

				if (found_val != NULL && Z_TYPE_P(found_val) == IS_INDIRECT) {
					found_val = Z_INDIRECT_P(found_val);
				}

				if (found_val != NULL &&
					Z_TYPE_P(found_val) != IS_UNDEF &&
					zend_opcache_serializer_value_has_unstorable(found_val)
				) {
					wb->failed_unstorable = true;
					zend_release_properties(props);
					zval_ptr_dtor(&sleep_rv);
					wb->len = hdr_offset;

					return false;
				}

				if (found_val == NULL || Z_TYPE_P(found_val) == IS_UNDEF) {
					zend_opcache_serializer_wbuf_write_hdr(wb, ZEND_OPCACHE_SERIALIZER_TYPE_UNDEF, 0, 0);
				} else if (!zend_opcache_serializer_encode_zval(wb, found_val)) {
					zend_release_properties(props);
					zval_ptr_dtor(&sleep_rv);

					if (EG(exception)) {
						return false;
					}

					if (wb->failed_unstorable) {
						wb->len = hdr_offset;

						return false;
					}

					wb->len = hdr_offset;

					goto fallback_php;
				}
			} ZEND_HASH_FOREACH_END();
		} else {
			ZEND_HASH_FOREACH_STR_KEY_VAL(props, prop_key, prop_val) {
				if (prop_key == NULL) {
					continue;
				}

				if (Z_TYPE_P(prop_val) == IS_INDIRECT) {
					prop_val = Z_INDIRECT_P(prop_val);
				}

				if (!zend_opcache_serializer_try_u32(ZSTR_LEN(prop_key), &key_len) ||
					!zend_opcache_serializer_write_string_key_entry(wb, ZSTR_VAL(prop_key), key_len)) {
					zend_release_properties(props);

					return false;
				}

				if (Z_TYPE_P(prop_val) == IS_UNDEF) {
					zend_opcache_serializer_wbuf_write_hdr(wb, ZEND_OPCACHE_SERIALIZER_TYPE_UNDEF, 0, 0);
				} else if (!zend_opcache_serializer_encode_zval(wb, prop_val)) {
					zend_release_properties(props);

					if (EG(exception)) {
						return false;
					}

					if (wb->failed_unstorable) {
						wb->len = hdr_offset;

						return false;
					}

					wb->len = hdr_offset;

					goto fallback_php;
				}
			} ZEND_HASH_FOREACH_END();

			zend_release_properties(props);

			if (!zend_opcache_serializer_patch_hdr(wb, arr_hdr_offset)) {
				return false;
			}

			return zend_opcache_serializer_patch_hdr(wb, hdr_offset);
		}

		if (has_sleep) {
			zval_ptr_dtor(&sleep_rv);
		}

		zend_release_properties(props);

		if (!zend_opcache_serializer_patch_hdr(wb, arr_hdr_offset)) {
			return false;
		}

		return zend_opcache_serializer_patch_hdr(wb, hdr_offset);
	}

fallback_php:
	if (zend_opcache_serializer_value_has_unstorable(zv)) {
		wb->failed_unstorable = true;

		return false;
	}

	PHP_VAR_SERIALIZE_INIT(var_hash);
	php_var_serialize(&ser, (zval *) zv, &var_hash);
	PHP_VAR_SERIALIZE_DESTROY(var_hash);
	if (ser.s == NULL) {
		return false;
	}

	if (!zend_opcache_serializer_try_u32(ZSTR_LEN(ser.s), &ser_len)) {
		smart_str_free(&ser);

		return false;
	}

	zend_opcache_serializer_wbuf_write_hdr(wb, ZEND_OPCACHE_SERIALIZER_TYPE_OBJECT,
		ZEND_OPCACHE_SERIALIZER_OBJ_PHP_SER, 0);

	if (!zend_opcache_serializer_write_bytes_entry(wb, ZSTR_VAL(class_name), name_len) ||
		!zend_opcache_serializer_write_bytes_entry(wb, ZSTR_VAL(ser.s), ser_len)
	) {
		smart_str_free(&ser);

		return false;
	}

	smart_str_free(&ser);

	return zend_opcache_serializer_patch_hdr(wb, hdr_offset);
}

static zend_always_inline bool zend_opcache_serializer_encode_zval(zend_opcache_serializer_wbuf_t *wb, const zval *zv)
{
	int64_t lval;
	double dval;

	if (wb->depth >= ZEND_OPCACHE_SERIALIZER_MAX_DEPTH) {
		return false;
	}

	wb->depth++;

	switch (Z_TYPE_P(zv)) {
		case IS_UNDEF:
			zend_opcache_serializer_wbuf_write_hdr(wb, ZEND_OPCACHE_SERIALIZER_TYPE_UNDEF, 0, 0);
			break;
		case IS_NULL:
			zend_opcache_serializer_wbuf_write_hdr(wb, ZEND_OPCACHE_SERIALIZER_TYPE_NULL, 0, 0);
			break;
		case IS_FALSE:
			zend_opcache_serializer_wbuf_write_hdr(wb, ZEND_OPCACHE_SERIALIZER_TYPE_FALSE, 0, 0);
			break;
		case IS_TRUE:
			zend_opcache_serializer_wbuf_write_hdr(wb, ZEND_OPCACHE_SERIALIZER_TYPE_TRUE, 0, 0);
			break;
		case IS_LONG:
			zend_opcache_serializer_wbuf_write_hdr(wb, ZEND_OPCACHE_SERIALIZER_TYPE_LONG, 0, sizeof(int64_t));
			lval = (int64_t) Z_LVAL_P(zv);
			zend_opcache_serializer_wbuf_write(wb, &lval, sizeof(int64_t));

			break;
		case IS_DOUBLE:
			zend_opcache_serializer_wbuf_write_hdr(wb, ZEND_OPCACHE_SERIALIZER_TYPE_DOUBLE, 0, sizeof(double));
			dval = Z_DVAL_P(zv);
			zend_opcache_serializer_wbuf_write(wb, &dval, sizeof(double));

			break;
		case IS_STRING:
			if (!zend_opcache_serializer_encode_string(wb, Z_STR_P(zv))) {
				wb->depth--;

				return false;
			}

			break;
		case IS_ARRAY:
			if (!zend_opcache_serializer_encode_array(wb, Z_ARRVAL_P(zv))) {
				wb->depth--;

				return false;
			}

			break;
		case IS_OBJECT:
			if (!zend_opcache_serializer_encode_object(wb, zv)) {
				wb->depth--;

				return false;
			}

			break;
		case IS_RESOURCE:
			wb->failed_unstorable = true;
			wb->depth--;

			return false;
		default:
			wb->depth--;

			return false;
	}

	wb->depth--;

	return true;
}

static zend_always_inline bool zend_opcache_serialize_ex(
		unsigned char **buf,
		size_t *buf_len,
		const zval *value,
		bool *failed_unstorable)
{
	zend_opcache_serializer_wbuf_t wb;
	bool ok;

	zend_opcache_serializer_wbuf_init(&wb, 256);
	ok = zend_opcache_serializer_encode_zval(&wb, value);
	if (failed_unstorable != NULL) {
		*failed_unstorable = wb.failed_unstorable;
	}
	zend_opcache_serializer_wbuf_destroy(&wb);

	if (!ok) {
		efree(wb.data);
		*buf = NULL;
		*buf_len = 0;

		return false;
	}

	*buf = wb.data;
	*buf_len = wb.len;

	return true;
}

static zend_always_inline bool zend_opcache_serialize(unsigned char **buf, size_t *buf_len,
		const zval *value)
{
	return zend_opcache_serialize_ex(buf, buf_len, value, NULL);
}

static zend_always_inline bool zend_opcache_serializer_rbuf_check(const zend_opcache_serializer_rbuf_t *rb, size_t need)
{
	return rb->pos + need <= rb->len;
}

static zend_always_inline const void *zend_opcache_serializer_rbuf_read(zend_opcache_serializer_rbuf_t *rb, size_t size)
{
	const void *ptr;
	size_t aligned_size;

	aligned_size = ZEND_OPCACHE_SERIALIZER_ALIGN8(size);
	if (!zend_opcache_serializer_rbuf_check(rb, aligned_size)) {
		return NULL;
	}

	ptr = rb->data + rb->pos;
	rb->pos += aligned_size;

	return ptr;
}

static zend_always_inline bool zend_opcache_serializer_rbuf_read_copy(
		zend_opcache_serializer_rbuf_t *rb, void *dst, size_t size)
{
	const void *src;

	src = zend_opcache_serializer_rbuf_read(rb, size);
	if (src == NULL) {
		return false;
	}

	memcpy(dst, src, size);

	return true;
}

static zend_always_inline bool zend_opcache_serializer_rbuf_read_hdr(
		zend_opcache_serializer_rbuf_t *rb, zend_opcache_serializer_hdr_t *hdr)
{
	return zend_opcache_serializer_rbuf_read_copy(rb, hdr, sizeof(*hdr));
}

static zend_always_inline bool zend_opcache_serializer_rbuf_skip(zend_opcache_serializer_rbuf_t *rb, size_t size)
{
	size_t aligned_size;

	aligned_size = ZEND_OPCACHE_SERIALIZER_ALIGN8(size);
	if (!zend_opcache_serializer_rbuf_check(rb, aligned_size)) {
		return false;
	}

	rb->pos += aligned_size;

	return true;
}

static zend_always_inline bool zend_opcache_serializer_decode_zval(zend_opcache_serializer_rbuf_t *rb,
		zval *dst);

static inline bool zend_opcache_serializer_decode_zval_suppressed(zend_opcache_serializer_rbuf_t *rb, zval *dst)
{
	bool result;

	rb->capture_suppression++;
	result = zend_opcache_serializer_decode_zval(rb, dst);
	rb->capture_suppression--;

	return result;
}

static zend_always_inline bool zend_opcache_serializer_decode_string_to_zval(zend_opcache_serializer_rbuf_t *rb,
		zval *dst, const zend_opcache_serializer_hdr_t *hdr)
{
	const unsigned char *payload;
	uint32_t string_len;

	if (hdr->data_size < 9) {
		return false;
	}

	payload = (const unsigned char *) zend_opcache_serializer_rbuf_read(rb, hdr->data_size);
	if (payload == NULL) {
		return false;
	}

	memcpy(&string_len, payload, sizeof(uint32_t));
	if ((size_t) string_len + 9 > hdr->data_size) {
		return false;
	}

	ZVAL_STRINGL(dst, (const char *) (payload + 8), string_len);

	return true;
}

static zend_always_inline bool zend_opcache_serializer_read_object_class_name(zend_opcache_serializer_rbuf_t *rb,
		const char **name_str_out, uint32_t *name_len_out)
{
	const unsigned char *name_data;
	const char *name_str;
	uint32_t name_len;

	name_data = (const unsigned char *) zend_opcache_serializer_rbuf_read(rb, 8);
	if (name_data == NULL) {
		return false;
	}

	memcpy(&name_len, name_data, sizeof(uint32_t));
	name_str = (const char *) zend_opcache_serializer_rbuf_read(rb, (size_t) name_len + 1);
	if (name_str == NULL) {
		return false;
	}

	*name_str_out = name_str;
	*name_len_out = name_len;

	return true;
}

static zend_always_inline bool zend_opcache_serializer_decode_object_payload_with_ce(
		zend_opcache_serializer_rbuf_t *rb, zval *dst, uint8_t obj_flags, zend_class_entry *ce)
{
	const unsigned char *arr_meta, *key_data;
	const char *key_str;
	zend_opcache_serializer_hdr_t arr_hdr;
	zend_class_entry *base_ce = NULL;
	zend_property_info *prop_info;
	zend_opcache_static_cache_safe_direct_state_copy_func_t copy_func;
	zend_opcache_static_cache_safe_direct_state_unserialize_func_t unserialize_func;
	zval props_zv, state_zv, data_arr,
		val, *existing, *target, func_name, wakeup_rv;
	HashTable *obj_ht;
	uint32_t prop_count, index, key_len;
	uint8_t key_type;
	bool ok;

	if ((obj_flags & ZEND_OPCACHE_SERIALIZER_OBJ_SAFE_DIRECT) != 0) {
		copy_func = zend_opcache_static_cache_safe_direct_copy_func(ce, &base_ce);
		if (copy_func == NULL ||
			zend_opcache_serializer_has_safe_direct_cache_overrides(ce, base_ce)
		) {
			ZVAL_NULL(dst);

			return false;
		}

		unserialize_func = zend_opcache_static_cache_safe_direct_state_unserialize_func(ce);
		if (unserialize_func == NULL) {
			ZVAL_NULL(dst);

			return false;
		}

		ZVAL_UNDEF(&state_zv);
		ZVAL_UNDEF(&props_zv);
		if (!zend_opcache_serializer_decode_zval_suppressed(rb, &state_zv) ||
			!zend_opcache_serializer_decode_zval_suppressed(rb, &props_zv) ||
			Z_TYPE(state_zv) != IS_ARRAY ||
			Z_TYPE(props_zv) != IS_ARRAY
		) {
			if (Z_TYPE(state_zv) != IS_UNDEF) {
				zval_ptr_dtor(&state_zv);
			}

			if (Z_TYPE(props_zv) != IS_UNDEF) {
				zval_ptr_dtor(&props_zv);
			}

			ZVAL_NULL(dst);

			return false;
		}

		if (object_init_ex(dst, ce) != SUCCESS) {
			zval_ptr_dtor(&state_zv);
			zval_ptr_dtor(&props_zv);
			ZVAL_NULL(dst);

			return false;
		}

		ok = unserialize_func(dst, &state_zv);
		zval_ptr_dtor(&state_zv);
		if (!ok || EG(exception)) {
			zval_ptr_dtor(&props_zv);
			zval_ptr_dtor(dst);
			ZVAL_NULL(dst);

			return false;
		}

		ok = zend_opcache_serializer_restore_object_properties(dst, Z_ARRVAL(props_zv));
		zval_ptr_dtor(&props_zv);
		if (!ok) {
			zval_ptr_dtor(dst);
			ZVAL_NULL(dst);

			return false;
		}

		zend_opcache_serializer_capture_decoded_reachable_value(rb, dst);

		return true;
	}

	if ((obj_flags & ZEND_OPCACHE_SERIALIZER_OBJ_SERIALIZE) != 0) {
		ZVAL_UNDEF(&data_arr);
		if (!zend_opcache_serializer_decode_zval_suppressed(rb, &data_arr) || Z_TYPE(data_arr) != IS_ARRAY) {
			if (Z_TYPE(data_arr) != IS_UNDEF) {
				zval_ptr_dtor(&data_arr);
			}
			ZVAL_NULL(dst);

			return false;
		}

		if (object_init_ex(dst, ce) != SUCCESS) {
			zval_ptr_dtor(&data_arr);
			ZVAL_NULL(dst);

			return false;
		}

		if (ce->__unserialize != NULL) {
			zend_call_known_instance_method_with_1_params(ce->__unserialize, Z_OBJ_P(dst), NULL, &data_arr);
			if (EG(exception)) {
				zval_ptr_dtor(&data_arr);
				zval_ptr_dtor(dst);
				ZVAL_NULL(dst);

				return false;
			}
		}

		zval_ptr_dtor(&data_arr);
		zend_opcache_serializer_capture_decoded_reachable_value(rb, dst);

		return true;
	}

	if ((obj_flags & ZEND_OPCACHE_SERIALIZER_OBJ_PROPS) != 0) {
		if (object_init_ex(dst, ce) != SUCCESS) {
			ZVAL_NULL(dst);

			return false;
		}

		if (!zend_opcache_serializer_rbuf_read_hdr(rb, &arr_hdr) || arr_hdr.type != ZEND_OPCACHE_SERIALIZER_TYPE_ARRAY) {
			zval_ptr_dtor(dst);
			ZVAL_NULL(dst);

			return false;
		}

		arr_meta = (const unsigned char *) zend_opcache_serializer_rbuf_read(rb, 8);
		if (arr_meta == NULL) {
			zval_ptr_dtor(dst);
			ZVAL_NULL(dst);

			return false;
		}

		memcpy(&prop_count, arr_meta, sizeof(uint32_t));
		obj_ht = Z_OBJ_P(dst)->handlers->get_properties(Z_OBJ_P(dst));
		if (obj_ht == NULL) {
			obj_ht = zend_std_get_properties(Z_OBJ_P(dst));
		}

		for (index = 0; index < prop_count; index++) {
			if (!zend_opcache_serializer_rbuf_check(rb, 8)) {
				zval_ptr_dtor(dst);
				ZVAL_UNDEF(dst);

				return false;
			}

			key_type = rb->data[rb->pos];
			if (key_type != 1) {
				zval_ptr_dtor(dst);
				ZVAL_UNDEF(dst);

				return false;
			}

			key_data = (const unsigned char *) zend_opcache_serializer_rbuf_read(rb, 8);
			if (key_data == NULL) {
				zval_ptr_dtor(dst);
				ZVAL_UNDEF(dst);

				return false;
			}

			memcpy(&key_len, key_data + 4, sizeof(uint32_t));

			key_str = (const char *) zend_opcache_serializer_rbuf_read(rb, (size_t) key_len + 1);
			if (key_str == NULL) {
				zval_ptr_dtor(dst);
				ZVAL_UNDEF(dst);

				return false;
			}

			ZVAL_UNDEF(&val);
			if (!zend_opcache_serializer_decode_zval(rb, &val)) {
				zval_ptr_dtor(dst);
				ZVAL_UNDEF(dst);

				return false;
			}

			existing = zend_hash_str_find(obj_ht, key_str, key_len);
			if (existing != NULL) {
				target = existing;
				if (Z_TYPE_P(target) == IS_INDIRECT) {
					target = Z_INDIRECT_P(target);
				}

				if (Z_TYPE(val) != IS_UNDEF) {
					prop_info = zend_get_typed_property_info_for_slot(Z_OBJ_P(dst), target);
					if (prop_info != NULL && !zend_verify_prop_assignable_by_ref(prop_info, &val, true)) {
						zval_ptr_dtor(&val);
						zval_ptr_dtor(dst);
						ZVAL_UNDEF(dst);

						return false;
					}

					zval_ptr_dtor(target);
					ZVAL_COPY_VALUE(target, &val);
				} else {
					zval_ptr_dtor(&val);
				}
			} else if (Z_TYPE(val) != IS_UNDEF) {
				zend_hash_str_add_new(obj_ht, key_str, key_len, &val);
			} else {
				zval_ptr_dtor(&val);
			}
		}

		if (zend_hash_str_exists(&ce->function_table, "__wakeup", sizeof("__wakeup") - 1) &&
			!EG(exception)
		) {
			ZVAL_UNDEF(&wakeup_rv);
			ZVAL_STRINGL(&func_name, "__wakeup", sizeof("__wakeup") - 1);
			call_user_function(CG(function_table), dst, &func_name, &wakeup_rv, 0, NULL);
			zval_ptr_dtor_str(&func_name);

			if (Z_TYPE(wakeup_rv) != IS_UNDEF) {
				zval_ptr_dtor(&wakeup_rv);
			}

			if (EG(exception)) {
				zval_ptr_dtor(dst);
				ZVAL_NULL(dst);

				return false;
			}
		}

		return true;
	}

	ZVAL_NULL(dst);

	return false;
}

static bool zend_opcache_serializer_decode_object(zend_opcache_serializer_rbuf_t *rb,
		zval *dst, const zend_opcache_serializer_hdr_t *hdr)
{
	const unsigned char *ser_meta, *ser_data, *position, *end;
	const char *name_str;
	zend_string *class_name;
	zend_class_entry *ce;
	php_unserialize_data_t var_hash;
	uint32_t name_len, ser_len;
	uint8_t obj_flags;
	int result;

	name_str = NULL;
	name_len = 0;
	obj_flags = hdr->flags;

	if (!zend_opcache_serializer_read_object_class_name(rb, &name_str, &name_len)) {
		return false;
	}

	if ((obj_flags & ZEND_OPCACHE_SERIALIZER_OBJ_PHP_SER) != 0) {
		ser_meta = (const unsigned char *) zend_opcache_serializer_rbuf_read(rb, 8);
		if (ser_meta == NULL) {
			return false;
		}

		memcpy(&ser_len, ser_meta, sizeof(uint32_t));

		ser_data = (const unsigned char *) zend_opcache_serializer_rbuf_read(rb, (size_t) ser_len + 1);
		if (ser_data == NULL) {
			return false;
		}

		position = ser_data;
		end = position + ser_len;
		PHP_VAR_UNSERIALIZE_INIT(var_hash);
		result = php_var_unserialize(dst, &position, end, &var_hash);
		PHP_VAR_UNSERIALIZE_DESTROY(var_hash);
		if (result != 1 || position != end) {
			ZVAL_NULL(dst);

			return false;
		}

		zend_opcache_serializer_capture_decoded_reachable_value(rb, dst);

		return true;
	}

	class_name = zend_string_init(name_str, name_len, 0);
	ce = zend_lookup_class(class_name);
	zend_string_release(class_name);
	if (ce == NULL) {
		ZVAL_NULL(dst);

		return false;
	}

	return zend_opcache_serializer_decode_object_payload_with_ce(rb, dst, obj_flags, ce);
}

static bool zend_opcache_serializer_decode_array(zend_opcache_serializer_rbuf_t *rb, zval *dst,
		const zend_opcache_serializer_hdr_t *hdr)
{
	const char *key_str;
	const unsigned char *meta, *key_data, *key_block;
	zval elem;
	uint64_t int_key;
	uint32_t count, next_free, index, key_len;
	uint8_t key_type;
	bool is_packed;

	if (hdr->data_size < 8) {
		return false;
	}

	meta = (const unsigned char *) zend_opcache_serializer_rbuf_read(rb, 8);
	if (meta == NULL) {
		return false;
	}

	memcpy(&count, meta, sizeof(uint32_t));
	memcpy(&next_free, meta + 4, sizeof(uint32_t));
	is_packed = (hdr->flags & ZEND_OPCACHE_SERIALIZER_ARRAY_PACKED) != 0;

	array_init_size(dst, count);

	if (is_packed) {
		for (index = 0; index < count; index++) {
			ZVAL_UNDEF(&elem);

			if (index + 1 < count && zend_opcache_serializer_rbuf_check(rb, 64)) {
				ZEND_OPCACHE_SERIALIZER_PREFETCH_R(rb->data + rb->pos + 64);
			}

			if (!zend_opcache_serializer_decode_zval(rb, &elem)) {
				zval_ptr_dtor(dst);
				ZVAL_UNDEF(dst);

				return false;
			}

			zend_hash_next_index_insert_new(Z_ARRVAL_P(dst), &elem);
		}
	} else {
		for (index = 0; index < count; index++) {
			if (zend_opcache_serializer_rbuf_check(rb, 64)) {
				ZEND_OPCACHE_SERIALIZER_PREFETCH_R(rb->data + rb->pos + 64);
			}

			if (!zend_opcache_serializer_rbuf_check(rb, 8)) {
				zval_ptr_dtor(dst);
				ZVAL_UNDEF(dst);

				return false;
			}

			key_type = rb->data[rb->pos];
			if (key_type == 0) {
				key_block = (const unsigned char *) zend_opcache_serializer_rbuf_read(rb, 16);
				if (key_block == NULL) {
					zval_ptr_dtor(dst);
					ZVAL_UNDEF(dst);

					return false;
				}

				memcpy(&int_key, key_block + 8, sizeof(uint64_t));

				ZVAL_UNDEF(&elem);
				if (!zend_opcache_serializer_decode_zval(rb, &elem)) {
					zval_ptr_dtor(dst);
					ZVAL_UNDEF(dst);

					return false;
				}

				zend_hash_index_add_new(Z_ARRVAL_P(dst), (zend_ulong) int_key, &elem);
			} else if (key_type == 1) {
				key_data = (const unsigned char *) zend_opcache_serializer_rbuf_read(rb, 8);
				if (key_data == NULL) {
					zval_ptr_dtor(dst);
					ZVAL_UNDEF(dst);

					return false;
				}

				memcpy(&key_len, key_data + 4, sizeof(uint32_t));

				key_str = (const char *) zend_opcache_serializer_rbuf_read(rb, (size_t) key_len + 1);
				if (key_str == NULL) {
					zval_ptr_dtor(dst);
					ZVAL_UNDEF(dst);

					return false;
				}

				ZVAL_UNDEF(&elem);
				if (!zend_opcache_serializer_decode_zval(rb, &elem)) {
					zval_ptr_dtor(dst);
					ZVAL_UNDEF(dst);

					return false;
				}

				zend_hash_str_add_new(Z_ARRVAL_P(dst), key_str, key_len, &elem);
			} else {
				zval_ptr_dtor(dst);
				ZVAL_UNDEF(dst);

				return false;
			}
		}
	}

	Z_ARRVAL_P(dst)->nNextFreeElement = next_free;

	return true;
}

static zend_always_inline bool zend_opcache_serializer_decode_zval(zend_opcache_serializer_rbuf_t *rb, zval *dst)
{
	zend_opcache_serializer_hdr_t hdr;
	const void *payload;
	int64_t lval;
	double dval;
	bool previous_skip_decoded_value_capture, skip_decoded_value_capture;

	if (rb->depth >= ZEND_OPCACHE_SERIALIZER_MAX_DEPTH) {
		ZVAL_NULL(dst);

		return false;
	}

	previous_skip_decoded_value_capture = rb->skip_decoded_value_capture;
	rb->skip_decoded_value_capture = false;
	rb->depth++;

	if (!zend_opcache_serializer_rbuf_read_hdr(rb, &hdr)) {
		ZVAL_NULL(dst);
		rb->depth--;

		return false;
	}

	switch (hdr.type) {
		case ZEND_OPCACHE_SERIALIZER_TYPE_UNDEF:
			ZVAL_UNDEF(dst);
			break;
		case ZEND_OPCACHE_SERIALIZER_TYPE_NULL:
			ZVAL_NULL(dst);
			break;
		case ZEND_OPCACHE_SERIALIZER_TYPE_FALSE:
			ZVAL_FALSE(dst);
			break;
		case ZEND_OPCACHE_SERIALIZER_TYPE_TRUE:
			ZVAL_TRUE(dst);
			break;
		case ZEND_OPCACHE_SERIALIZER_TYPE_LONG: {
			payload = zend_opcache_serializer_rbuf_read(rb, sizeof(int64_t));
			if (payload == NULL) {
				ZVAL_NULL(dst);
				rb->depth--;

				return false;
			}

			memcpy(&lval, payload, sizeof(int64_t));

			ZVAL_LONG(dst, (zend_long) lval);
			break;
		}
		case ZEND_OPCACHE_SERIALIZER_TYPE_DOUBLE: {
			payload = zend_opcache_serializer_rbuf_read(rb, sizeof(double));
			if (payload == NULL) {
				ZVAL_NULL(dst);
				rb->depth--;

				return false;
			}

			memcpy(&dval, payload, sizeof(double));

			ZVAL_DOUBLE(dst, dval);
			break;
		}
		case ZEND_OPCACHE_SERIALIZER_TYPE_STRING:
			if (!zend_opcache_serializer_decode_string_to_zval(rb, dst, &hdr)) {
				ZVAL_NULL(dst);
				rb->depth--;

				return false;
			}
			break;
		case ZEND_OPCACHE_SERIALIZER_TYPE_ARRAY:
			if (!zend_opcache_serializer_decode_array(rb, dst, &hdr)) {
				rb->depth--;

				return false;
			}
			break;
		case ZEND_OPCACHE_SERIALIZER_TYPE_OBJECT:
			if (!zend_opcache_serializer_decode_object(rb, dst, &hdr)) {
				rb->depth--;

				return false;
			}
			break;
		default:
			ZVAL_NULL(dst);
			rb->depth--;

			return false;
	}

	skip_decoded_value_capture = rb->skip_decoded_value_capture;
	rb->skip_decoded_value_capture = previous_skip_decoded_value_capture;
	if (rb->capture_suppression == 0 &&
		!skip_decoded_value_capture &&
		(Z_TYPE_P(dst) == IS_ARRAY || Z_TYPE_P(dst) == IS_OBJECT)
	) {
		zend_opcache_serializer_capture_decoded_value(rb, dst);
	}

	rb->depth--;

	return true;
}

static zend_always_inline bool zend_opcache_unserialize_ex(
	zval *dst,
	const unsigned char *buf,
	size_t buf_len,
	zend_opcache_serializer_capture_func_t capture_decoded_value,
	zend_opcache_serializer_capture_func_t capture_decoded_reachable_value
)
{
	zend_opcache_serializer_rbuf_t rb;

	rb.data = buf;
	rb.capture_decoded_value = capture_decoded_value;
	rb.capture_decoded_reachable_value = capture_decoded_reachable_value;
	rb.len = buf_len;
	rb.pos = 0;
	rb.depth = 0;
	rb.capture_suppression = 0;
	rb.skip_decoded_value_capture = false;

	if (!zend_opcache_serializer_decode_zval(&rb, dst)) {
		return false;
	}

	return rb.pos == rb.len;
}

static zend_always_inline bool zend_opcache_unserialize(zval *dst, const unsigned char *buf, size_t buf_len)
{
	return zend_opcache_unserialize_ex(dst, buf, buf_len, NULL, NULL);
}

#endif
