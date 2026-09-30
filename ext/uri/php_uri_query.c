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
   | Authors: Máté Kocsis <kocsismate@php.net>                            |
   +----------------------------------------------------------------------+
*/
#include "zend_enum.h"
#include "zend_smart_str.h"
#include "zend_API.h"
#include "zend_hash.h"
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "php.h"
#include "Zend/zend_exceptions.h"
#include "Zend/zend_interfaces.h"

#include "php_uri_common.h"
#include "php_uri_query.h"
#include "uri_parser_rfc3986.h"
#include "uri_parser_whatwg.h"
#include "lexbor/encoding/decode.h"

#define MAX_TOMBSTONES 100

ZEND_ATTRIBUTE_NONNULL_ARGS(1) static zend_string *php_uri_query_params_convert_value_to_str(
	const zval *value, const zend_string *array_key
) {
	ZVAL_DEREF(value);

	switch (Z_TYPE_P(value)) {
		case IS_NULL:
			return NULL;
		case IS_DOUBLE: {
			if (UNEXPECTED(!zend_finite(Z_DVAL_P(value)))) {
				if (array_key != NULL) {
					zend_argument_value_error(1,
						"contains a non-finite float for query parameter \"%pS\"", array_key);
				} else {
					zend_argument_value_error(2, "must be finite");
				}
				return NULL;
			}

			char buffer[ZEND_DOUBLE_MAX_LENGTH];
			zend_gcvt(Z_DVAL_P(value), (int) PG(serialize_precision), '.', 'e', buffer);
			return zend_string_init(buffer, strlen(buffer), false);
		}
		case IS_OBJECT:
			if (instanceof_function(Z_OBJCE_P(value), zend_ce_backed_enum)) {
				value = zend_enum_fetch_case_value(Z_OBJ_P(value));
			}
			break;
		default:
			break;
	}

	if (array_key != NULL) {
		if (Z_TYPE_P(value) == IS_STRING || Z_TYPE_P(value) == IS_LONG || Z_TYPE_P(value) == IS_DOUBLE) {
			return zval_get_string(value);
		}
		zend_argument_type_error(1,
			"contains an invalid value for query parameter \"%pS\": expected BackedEnum|string|int|float|null, %s given",
			array_key, zend_zval_value_name(value));
		return NULL;
	}

	zval tmp;
	ZVAL_COPY(&tmp, value);
	zval *normalized;
	zend_string *result = NULL;

	if (zend_parse_arg_number_or_str(&tmp, &normalized, false, 2) && !EG(exception)) {
		result = zval_get_string(&tmp);
	} else if (!EG(exception)) {
		zend_argument_type_error(2, "must be of type BackedEnum|string|int|float|null, %s given", zend_zval_value_name(&tmp));
	}
	zval_ptr_dtor(&tmp);

	return result;
}

ZEND_ATTRIBUTE_NONNULL_ARGS(1,2) static bool php_uri_query_params_has_entry(
	const php_uri_query_params_object *query_params_object, zend_string *name
) {
	const php_uri_query_param_lookup_entry *lookup_entry = zend_hash_find_ptr(&query_params_object->entry_lookup_table, name);

	return lookup_entry != NULL;
}

ZEND_ATTRIBUTE_NONNULL_ARGS(1,2) static bool php_uri_query_params_has_entry_with_value(
	const php_uri_query_params_object *query_params_object, zend_string *name, const zend_string *value
) {
	const php_uri_query_param_lookup_entry *lookup_entry = zend_hash_find_ptr(&query_params_object->entry_lookup_table, name);
	if (lookup_entry == NULL) {
		return false;
	}

	uint32_t index = lookup_entry->first_index;
	while (index != UINT32_MAX) {
		const php_uri_query_param_list_entry *list_entry = zend_hash_index_find_ptr(&query_params_object->entry_list, index);
		ZEND_ASSERT(list_entry != NULL);

		if (
			(value == NULL && list_entry->value == NULL) ||
			(value != NULL && list_entry->value != NULL && zend_string_equal_content(value, list_entry->value))
		) {
			return true;
		}

		index = list_entry->next_index;
	}

	return false;
}

static void php_uri_query_params_find_value(
	const php_uri_query_params_object *query_params_object, const uint32_t index, zval *zv
) {
	const php_uri_query_param_list_entry *list_entry = zend_hash_index_find_ptr(&query_params_object->entry_list, index);
	ZEND_ASSERT(list_entry != NULL);

	if (list_entry->value == NULL) {
		ZVAL_NULL(zv);
	} else {
		ZVAL_STR_COPY(zv, list_entry->value);
	}
}

ZEND_ATTRIBUTE_NONNULL_ARGS(1, 2) static void php_uri_query_params_find_first_value(
	const php_uri_query_params_object *query_params_object, zend_string *name, zval *zv
) {
	const php_uri_query_param_lookup_entry *lookup_entry = zend_hash_find_ptr(&query_params_object->entry_lookup_table, name);
	if (lookup_entry == NULL) {
		ZVAL_NULL(zv);
		return;
	}

	php_uri_query_params_find_value(query_params_object, lookup_entry->first_index, zv);
}

ZEND_ATTRIBUTE_NONNULL_ARGS(1,2) static void php_uri_query_params_find_last_value(
	const php_uri_query_params_object *query_params_object, zend_string *name, zval *zv
) {
	const php_uri_query_param_lookup_entry *lookup_entry = zend_hash_find_ptr(&query_params_object->entry_lookup_table, name);
	if (lookup_entry == NULL) {
		ZVAL_NULL(zv);
		return;
	}

	php_uri_query_params_find_value(query_params_object, lookup_entry->last_index, zv);
}

ZEND_ATTRIBUTE_NONNULL_ARGS(1,2) static void php_uri_query_params_find_all_values(
	const php_uri_query_params_object *query_params_object, zend_string *name, zval *zv
) {
	const php_uri_query_param_lookup_entry *lookup_entry = zend_hash_find_ptr(&query_params_object->entry_lookup_table, name);
	if (lookup_entry == NULL) {
		ZVAL_EMPTY_ARRAY(zv);
		return;
	}

	array_init(zv);

	uint32_t index = lookup_entry->first_index;
	while (index != UINT32_MAX) {
		const php_uri_query_param_list_entry *list_entry = zend_hash_index_find_ptr(&query_params_object->entry_list, index);
		ZEND_ASSERT(list_entry != NULL);

		if (list_entry->key != NULL) {
			zval value;
			if (list_entry->value == NULL) {
				ZVAL_NULL(&value);
			} else {
				ZVAL_STR_COPY(&value, list_entry->value);
			}
			zend_hash_next_index_insert(Z_ARR_P(zv), &value);
		}

		index = list_entry->next_index;
	}
}

ZEND_ATTRIBUTE_NONNULL static void php_uri_query_params_list_entries(php_uri_query_params_object *object, zval *params)
{
	array_init_size(params, zend_array_count(&object->entry_list));

	ZEND_HASH_FOREACH_PTR(&object->entry_list, php_uri_query_param_list_entry *list_entry) {
		if (list_entry->key != NULL) {
			zval key;
			ZVAL_STR_COPY(&key, list_entry->key);

			zval value;
			if (list_entry->value == NULL) {
				ZVAL_NULL(&value);
			} else {
				ZVAL_STR_COPY(&value, list_entry->value);
			}

			zval zv;
			array_init_size(&zv, 2);
			zend_hash_index_add(Z_ARR(zv), 0, &key);
			zend_hash_index_add(Z_ARR(zv), 1, &value);

			zend_hash_next_index_insert(Z_ARR_P(params), &zv);
		}
	} ZEND_HASH_FOREACH_END();
}

ZEND_ATTRIBUTE_NONNULL_ARGS(1,2) static zend_result php_uri_query_params_append_entry(
	php_uri_query_params_object *query_params_object, zend_string *name, zend_string *value
) {
	php_uri_query_param_list_entry new_list_entry = {
		.key = zend_string_copy(name),
		.value = value == NULL ? NULL : zend_string_copy(value),
		.next_index = UINT32_MAX,
	};

	const void *result = zend_hash_next_index_insert_mem(&query_params_object->entry_list, &new_list_entry, sizeof(new_list_entry));
	if (result == NULL) {
		return FAILURE;
	}

	const zend_long new_index = query_params_object->entry_list.nNextFreeElement - 1;

	php_uri_query_param_lookup_entry *lookup_entry = zend_hash_find_ptr(&query_params_object->entry_lookup_table, name);

	if (lookup_entry == NULL) {
		/* Insert a new lookup entry */

		php_uri_query_param_lookup_entry new_lookup_entry = {
			.first_index = new_index,
			.last_index = new_index,
		};
		if (zend_hash_add_mem(&query_params_object->entry_lookup_table, name, &new_lookup_entry, sizeof(new_lookup_entry)) == NULL) {
			zend_hash_index_del(&query_params_object->entry_list, new_index);
			return FAILURE;
		}
	} else {
		/* Update the existing lookup entry + the same key list */

		php_uri_query_param_list_entry *last_list_entry = zend_hash_index_find_ptr(&query_params_object->entry_list, lookup_entry->last_index);
		ZEND_ASSERT(last_list_entry != NULL);

		last_list_entry->next_index = new_index;
		lookup_entry->last_index = new_index;
	}

	return SUCCESS;
}

ZEND_ATTRIBUTE_NONNULL static zend_result php_uri_query_params_append_array_value(
	php_uri_query_params_object *object, zend_string *name, const zval *value
) {
	zend_result result = FAILURE;

	zend_string *value_str = php_uri_query_params_convert_value_to_str(value, name);
	if (!EG(exception)) {
		result = php_uri_query_params_append_entry(object, name, value_str);
		if (UNEXPECTED(result == FAILURE)) {
			/* This should be unreachable in practice. */
			zend_throw_exception(php_uri_ce_error, "Failed to append the query parameter", 0);
		}
	}

	if (value_str != NULL) {
		zend_string_release(value_str);
	}

	return result;
}

ZEND_ATTRIBUTE_NONNULL_ARGS(1, 2) static zend_result php_uri_query_params_append_array(
	php_uri_query_params_object *object, HashTable *array, const zend_string *prefix
) {
	if (GC_IS_RECURSIVE(array)) {
		zend_argument_value_error(1, "must not contain recursive arrays");
		return FAILURE;
	}

#ifdef ZEND_CHECK_STACK_LIMIT
	if (UNEXPECTED(zend_call_stack_overflowed(EG(stack_limit)))) {
		zend_throw_error(NULL, "Maximum call stack size reached.");
		return FAILURE;
	}
#endif

	bool use_empty_brackets = prefix != NULL && zend_array_is_list(array);
	if (use_empty_brackets) {
		ZEND_HASH_FOREACH_VAL(array, zval *value) {
			ZVAL_DEREF(value);
			if (Z_TYPE_P(value) == IS_ARRAY) {
				use_empty_brackets = false;
				break;
			}
		} ZEND_HASH_FOREACH_END();
	}

	zend_result result = SUCCESS;
	zend_ulong index;
	zend_string *key;
	zval *value;

	GC_TRY_PROTECT_RECURSION(array);

	ZEND_HASH_FOREACH_KEY_VAL(array, index, key, value) {
		ZVAL_DEREF(value);

		if (Z_TYPE_P(value) == IS_ARRAY && zend_hash_num_elements(Z_ARRVAL_P(value)) == 0) {
			continue;
		}

		zend_string *name;
		if (prefix == NULL) {
			name = key != NULL ? zend_string_copy(key) : zend_long_to_str((zend_long) index);
		} else if (use_empty_brackets) {
			name = zend_string_concat2(ZSTR_VAL(prefix), ZSTR_LEN(prefix), "[]", 2);
		} else {
			smart_str buf = {0};
			smart_str_append(&buf, prefix);
			smart_str_appendc(&buf, '[');
			if (key != NULL) {
				smart_str_append(&buf, key);
			} else {
				smart_str_append_long(&buf, (zend_long) index);
			}
			smart_str_appendc(&buf, ']');
			name = smart_str_extract(&buf);
		}

		if (Z_TYPE_P(value) == IS_ARRAY) {
			result = php_uri_query_params_append_array(object, Z_ARRVAL_P(value), name);
		} else {
			result = php_uri_query_params_append_array_value(object, name, value);
		}

		zend_string_release(name);
		if (result == FAILURE) {
			break;
		}
	} ZEND_HASH_FOREACH_END();

	GC_TRY_UNPROTECT_RECURSION(array);

	return result;
}

ZEND_ATTRIBUTE_NONNULL static void php_uri_query_params_rebuild_lookup(const HashTable *entry_list, const HashTable *entry_lookup_table)
{
	ZEND_HASH_FOREACH_PTR(entry_lookup_table, php_uri_query_param_lookup_entry *lookup_entry) {
		lookup_entry->first_index = UINT32_MAX;
		lookup_entry->last_index = UINT32_MAX;
	} ZEND_HASH_FOREACH_END();

	zend_ulong new_index;
	ZEND_HASH_FOREACH_NUM_KEY_PTR(entry_list, new_index, php_uri_query_param_list_entry *new_entry) {
		new_entry->next_index = UINT32_MAX;
		php_uri_query_param_lookup_entry *lookup_entry = zend_hash_find_ptr(entry_lookup_table, new_entry->key);
		ZEND_ASSERT(lookup_entry != NULL);
		if (lookup_entry->first_index == UINT32_MAX) {
			lookup_entry->first_index = new_index;
		} else {
			php_uri_query_param_list_entry *prev_entry = zend_hash_index_find_ptr(entry_list, lookup_entry->last_index);
			ZEND_ASSERT(prev_entry != NULL);
			prev_entry->next_index = new_index;
		}

		lookup_entry->last_index = new_index;
	} ZEND_HASH_FOREACH_END();
}

static int php_uri_query_params_remove_tombstone(zval *zv)
{
	const php_uri_query_param_list_entry *entry = Z_PTR_P(zv);

	return entry->key == NULL ? ZEND_HASH_APPLY_REMOVE : ZEND_HASH_APPLY_KEEP;
}

ZEND_ATTRIBUTE_NONNULL static void php_uri_query_params_remove_tombstones(php_uri_query_params_object *object)
{
	if (object->tombstone_count == zend_hash_num_elements(&object->entry_list)) {
		/* Reset the next insertion index when no live entries remain. */
		zend_hash_clean(&object->entry_list);
	} else {
		zend_hash_apply(&object->entry_list, php_uri_query_params_remove_tombstone);
	}
	object->tombstone_count = 0;
}

ZEND_ATTRIBUTE_NONNULL static void php_uri_query_params_cleanup_tombstones(
php_uri_query_params_object *query_params_object
) {
	if (query_params_object->tombstone_count <= MAX_TOMBSTONES) {
		return;
	}

	HashTable *entry_list = &query_params_object->entry_list;
	ZEND_ASSERT(HT_IS_PACKED(entry_list));
	ZEND_ASSERT(!HT_HAS_ITERATORS(entry_list));

	php_uri_query_params_remove_tombstones(query_params_object);

	/* Get rid of any holes - ZEND_HASH_PACKED_FOREACH_VAL skips UNDEF items */
	uint32_t new_index = 0;
	ZEND_HASH_PACKED_FOREACH_VAL(entry_list, zval *entry_zv) {
		zval *new_entry_zv = &entry_list->arPacked[new_index];
		if (new_entry_zv != entry_zv) {
			ZVAL_COPY_VALUE(new_entry_zv, entry_zv);
			ZVAL_UNDEF(entry_zv);
		}
		new_index++;
	} ZEND_HASH_FOREACH_END();

	entry_list->nNumUsed = new_index;
	entry_list->nNextFreeElement = new_index;
	entry_list->nInternalPointer = 0;

	php_uri_query_params_rebuild_lookup(entry_list, &query_params_object->entry_lookup_table);
}

static int php_uri_query_params_compare_original_order(const Bucket *a, const Bucket *b)
{
	/* zend_hash_sort() stores the original position in Z_EXTRA() for stable sorting. */
	return (Z_EXTRA(a->val) > Z_EXTRA(b->val)) - (Z_EXTRA(a->val) < Z_EXTRA(b->val));
}

static int php_uri_query_params_compare_bytes(Bucket *a, Bucket *b)
{
	const php_uri_query_param_list_entry *entry_a = Z_PTR(a->val);
	const php_uri_query_param_list_entry *entry_b = Z_PTR(b->val);
	const int result = zend_binary_strcmp(
		ZSTR_VAL(entry_a->key), ZSTR_LEN(entry_a->key), ZSTR_VAL(entry_b->key), ZSTR_LEN(entry_b->key));

	return result != 0 ? result : php_uri_query_params_compare_original_order(a, b);
}

static lxb_codepoint_t php_uri_query_params_next_codepoint(const lxb_char_t **p, const lxb_char_t *end)
{
	if (**p < 0x80) {
		return *(*p)++;
	}

	lxb_encoding_decode_t decoder = {0};
	const lxb_codepoint_t cp = lxb_encoding_decode_utf_8_single(&decoder, p, end);

	if (cp == LXB_ENCODING_DECODE_ERROR || cp == LXB_ENCODING_DECODE_CONTINUE) {
		return LXB_ENCODING_REPLACEMENT_CODEPOINT;
	}

	return cp;
}

static int php_uri_query_params_compare_utf16(Bucket *a, Bucket *b)
{
	const php_uri_query_param_list_entry *entry_a = Z_PTR(a->val);
	const php_uri_query_param_list_entry *entry_b = Z_PTR(b->val);

	const lxb_char_t *p = (const lxb_char_t *) ZSTR_VAL(entry_a->key);
	const lxb_char_t *q = (const lxb_char_t *) ZSTR_VAL(entry_b->key);

	const lxb_char_t *end_a = p + ZSTR_LEN(entry_a->key);
	const lxb_char_t *end_b = q + ZSTR_LEN(entry_b->key);

	while (p < end_a && q < end_b) {
		const lxb_codepoint_t cp_a = php_uri_query_params_next_codepoint(&p, end_a);
		const lxb_codepoint_t cp_b = php_uri_query_params_next_codepoint(&q, end_b);

		if (cp_a == cp_b) {
			continue;
		}

		/* UTF-16 follows code point order except that surrogate pairs precede U+E000..U+FFFF. */
		if (cp_a >= 0xE000 && cp_b >= 0xE000 && (cp_a > 0xFFFF) != (cp_b > 0xFFFF)) {
			return cp_a > 0xFFFF ? -1 : 1;
		}

		return (int) cp_a - (int) cp_b;
	}

	if (p < end_a || q < end_b) {
		return p < end_a ? 1 : -1;
	}

	return php_uri_query_params_compare_original_order(a, b);
}

ZEND_ATTRIBUTE_NONNULL static void php_uri_query_params_sort_entries(
	php_uri_query_params_object *object, const bucket_compare_func_t compare
) {
	if (object->tombstone_count > 0) {
		php_uri_query_params_remove_tombstones(object);
	}

	zend_hash_sort(&object->entry_list, compare, true);

	php_uri_query_params_rebuild_lookup(&object->entry_list, &object->entry_lookup_table);
}

static void php_uri_query_params_tombstone_entry(
	php_uri_query_params_object *query_params_object, php_uri_query_param_list_entry *list_entry
) {
	if (list_entry->key == NULL) {
		return;
	}

	zend_string_release(list_entry->key);
	list_entry->key = NULL;
	if (list_entry->value != NULL) {
		zend_string_release(list_entry->value);
		list_entry->value = NULL;
	}

	list_entry->next_index = UINT32_MAX;
	query_params_object->tombstone_count++;
}

ZEND_ATTRIBUTE_NONNULL_ARGS(1, 2) static void php_uri_query_params_delete_entries(
	php_uri_query_params_object *query_params_object, zend_string *name
) {
	const php_uri_query_param_lookup_entry *lookup_entry = zend_hash_find_ptr(&query_params_object->entry_lookup_table, name);
	if (lookup_entry == NULL) {
		return;
	}

	uint32_t index = lookup_entry->first_index;
	while (index != UINT32_MAX) {
		php_uri_query_param_list_entry *list_entry = zend_hash_index_find_ptr(&query_params_object->entry_list, index);
		ZEND_ASSERT(list_entry != NULL);

		const uint32_t next = list_entry->next_index;
		php_uri_query_params_tombstone_entry(query_params_object, list_entry);
		index = next;
	}

	zend_hash_del(&query_params_object->entry_lookup_table, name);

	php_uri_query_params_cleanup_tombstones(query_params_object);
}

ZEND_ATTRIBUTE_NONNULL_ARGS(1,2) static void php_uri_query_params_delete_entries_with_value(
	php_uri_query_params_object *query_params_object, zend_string *name, const zend_string *value
) {
	php_uri_query_param_lookup_entry *lookup_entry = zend_hash_find_ptr(&query_params_object->entry_lookup_table, name);
	if (lookup_entry == NULL) {
		return;
	}

	uint32_t index = lookup_entry->first_index;
	uint32_t new_first = UINT32_MAX;
	uint32_t new_last = UINT32_MAX;
	php_uri_query_param_list_entry *prev_list_entry = NULL;

	while (index != UINT32_MAX) {
		php_uri_query_param_list_entry *list_entry = zend_hash_index_find_ptr(&query_params_object->entry_list, index);
		ZEND_ASSERT(list_entry != NULL);

		const uint32_t next = list_entry->next_index;

		const bool should_delete = (value == NULL && list_entry->value == NULL) ||
			(value != NULL && list_entry->value != NULL && zend_string_equal_content(value, list_entry->value));

		if (should_delete) {
			php_uri_query_params_tombstone_entry(query_params_object, list_entry);

			/* Rebuild the same key list */
			if (prev_list_entry != NULL) {
				prev_list_entry->next_index = next;
			}
		} else {
			if (new_first == UINT32_MAX) {
				new_first = index;
			}
			new_last = index;
			prev_list_entry = list_entry;
		}

		index = next;
	}

	/* Either delete the lookup entry if no list entries remain, or update it with the new data. */
	if (new_first == UINT32_MAX) {
		zend_hash_del(&query_params_object->entry_lookup_table, name);
	} else {
		php_uri_query_param_list_entry *last_entry = zend_hash_index_find_ptr(&query_params_object->entry_list, new_last);
		ZEND_ASSERT(last_entry != NULL);
		last_entry->next_index = UINT32_MAX;

		lookup_entry->first_index = new_first;
		lookup_entry->last_index = new_last;
	}
}

ZEND_ATTRIBUTE_NONNULL_ARGS(1, 2) static zend_result php_uri_query_params_set_entry(
	php_uri_query_params_object *query_params_object, zend_string *name, zend_string *value
) {
	php_uri_query_param_lookup_entry *lookup_entry = zend_hash_find_ptr(&query_params_object->entry_lookup_table, name);
	if (lookup_entry == NULL) {
		return php_uri_query_params_append_entry(query_params_object, name, value);
	}

	/* Overwrite first occurrence */
	php_uri_query_param_list_entry *first_entry = zend_hash_index_find_ptr(&query_params_object->entry_list, lookup_entry->first_index);
	ZEND_ASSERT(first_entry != NULL);

	if (first_entry->value != NULL) {
		zend_string_release(first_entry->value);
	}
	first_entry->value = value != NULL ? zend_string_copy(value) : NULL;

	/* Tombstone the rest of the occurrences */
	uint32_t index = first_entry->next_index;
	first_entry->next_index = UINT32_MAX;

	while (index != UINT32_MAX) {
		php_uri_query_param_list_entry *list_entry = zend_hash_index_find_ptr(&query_params_object->entry_list, index);
		ZEND_ASSERT(list_entry != NULL);

		const uint32_t next = list_entry->next_index;
		php_uri_query_params_tombstone_entry(query_params_object, list_entry);
		index = next;
	}

	/* Update the lookup entry */
	lookup_entry->last_index = lookup_entry->first_index;

	php_uri_query_params_cleanup_tombstones(query_params_object);

	return SUCCESS;
}

ZEND_ATTRIBUTE_NONNULL zend_object *php_uri_object_query_params_create_object(zend_class_entry *ce)
{
	php_uri_query_params_object *uri_query_params_object = zend_object_alloc(sizeof(*uri_query_params_object), ce);

	zend_object_std_init(&uri_query_params_object->std, ce);
	object_properties_init(&uri_query_params_object->std, ce);

	uri_query_params_object->is_initialized = false;
	uri_query_params_object->tombstone_count = 0;

	return &uri_query_params_object->std;
}

ZEND_ATTRIBUTE_NONNULL void php_uri_query_params_object_free(php_uri_query_params_object *uri_query_params_object)
{
	if (uri_query_params_object->is_initialized) {
		zend_hash_destroy(&uri_query_params_object->entry_list);
		zend_hash_destroy(&uri_query_params_object->entry_lookup_table);
		uri_query_params_object->tombstone_count = 0;
	}
}

ZEND_ATTRIBUTE_NONNULL void php_uri_query_params_object_handler_free(zend_object *object)
{
	php_uri_query_params_object *uri_query_params_object = php_uri_query_params_object_from_obj(object);

	php_uri_query_params_object_free(uri_query_params_object);

	zend_object_std_dtor(&uri_query_params_object->std);
}

static void php_uri_query_param_list_entry_copy(zval *zv)
{
	const php_uri_query_param_list_entry *original = Z_PTR_P(zv);
	php_uri_query_param_list_entry *copy = emalloc(sizeof(*copy));

	copy->key = original->key != NULL ? zend_string_copy(original->key) : NULL;
	copy->value = original->value != NULL ? zend_string_copy(original->value) : NULL;
	copy->next_index = original->next_index;

	Z_PTR_P(zv) = copy;
}

static void php_uri_query_param_lookup_entry_copy(zval *zv)
{
	const php_uri_query_param_lookup_entry *original = Z_PTR_P(zv);
	php_uri_query_param_lookup_entry *copy = emalloc(sizeof(*copy));

	copy->first_index = original->first_index;
	copy->last_index = original->last_index;

	Z_PTR_P(zv) = copy;
}

ZEND_ATTRIBUTE_NONNULL zend_object *php_uri_query_params_object_handler_clone(zend_object *object)
{
	const php_uri_query_params_object *uri_query_params_object = php_uri_query_params_object_from_obj(object);

	php_uri_query_params_object *new_uri_query_params_object = php_uri_query_params_object_from_obj(
		object->ce->create_object(object->ce)
	);

	if (uri_query_params_object->is_initialized) {
		zend_hash_init(&new_uri_query_params_object->entry_list, zend_hash_num_elements(&uri_query_params_object->entry_list),
			NULL, uri_query_params_object->entry_list.pDestructor, 0);
		zend_hash_init(&new_uri_query_params_object->entry_lookup_table, zend_hash_num_elements(&uri_query_params_object->entry_lookup_table),
			NULL, uri_query_params_object->entry_lookup_table.pDestructor, 0);
		zend_hash_copy(&new_uri_query_params_object->entry_list, &uri_query_params_object->entry_list, php_uri_query_param_list_entry_copy);
		zend_hash_copy(&new_uri_query_params_object->entry_lookup_table, &uri_query_params_object->entry_lookup_table, php_uri_query_param_lookup_entry_copy);
		new_uri_query_params_object->tombstone_count = uri_query_params_object->tombstone_count;
		new_uri_query_params_object->is_initialized = true;
	}
	zend_objects_clone_members(&new_uri_query_params_object->std, &uri_query_params_object->std);

	return &new_uri_query_params_object->std;
}

ZEND_ATTRIBUTE_NONNULL static bool php_uri_query_params_object_can_be_reused(const zend_object *object)
{
	/* Only reuse an object when neither a strong nor a weak reference can observe the change. */
	return GC_REFCOUNT(object) == 1 && !(GC_FLAGS(object) & IS_OBJ_WEAKLY_REFERENCED);
}

ZEND_ATTRIBUTE_NONNULL static php_uri_query_params_object *php_uri_query_params_object_clone_for_modification(
	zend_object *object, zval *return_value
) {
	if (php_uri_query_params_object_can_be_reused(object)) {
		ZVAL_OBJ_COPY(return_value, object);
	} else {
		zend_object *new_object = object->handlers->clone_obj(object);
		if (new_object == NULL) {
			return NULL;
		}
		ZVAL_OBJ(return_value, new_object);
	}

	return Z_URI_QUERY_PARAMS_OBJECT_P(return_value);
}

typedef struct php_uri_query_params_iterator {
	zend_object_iterator intern;
	php_uri_query_params_object *query_params_object;
	uint32_t current_index;
	zval current_value;
} php_uri_query_params_iterator;

static void php_uri_query_params_iterator_dtor(zend_object_iterator *iter)
{
	php_uri_query_params_iterator *iterator = (php_uri_query_params_iterator *) iter;
	zval_ptr_dtor(&iterator->current_value);
	zval_ptr_dtor(&iterator->intern.data);
}

static php_uri_query_param_list_entry *php_uri_query_params_iterator_current_entry(
	php_uri_query_params_iterator *iterator
) {
	const php_uri_query_params_object *obj = iterator->query_params_object;

	if (!obj->is_initialized) {
		return NULL;
	}

	while (iterator->current_index < zend_hash_num_elements(&obj->entry_list)) {
		php_uri_query_param_list_entry *entry = zend_hash_index_find_ptr(&obj->entry_list, iterator->current_index);

		if (entry != NULL && entry->key != NULL) {
			return entry;
		}

		iterator->current_index++;
	}

	return NULL;
}

static zend_result php_uri_query_params_iterator_valid(zend_object_iterator *iter)
{
	php_uri_query_params_iterator *iterator = (php_uri_query_params_iterator *) iter;

	return php_uri_query_params_iterator_current_entry(iterator) != NULL ? SUCCESS : FAILURE;
}

static zval *php_uri_query_params_iterator_current_data(zend_object_iterator *iter)
{
	php_uri_query_params_iterator *iterator = (php_uri_query_params_iterator *) iter;
	const php_uri_query_param_list_entry *entry = php_uri_query_params_iterator_current_entry(iterator);

	if (entry == NULL) {
		return NULL;
	}

	zval_ptr_dtor(&iterator->current_value);
	array_init_size(&iterator->current_value, 2);

	zval key_zv, value_zv;
	ZVAL_STR_COPY(&key_zv, entry->key);
	if (entry->value != NULL) {
		ZVAL_STR_COPY(&value_zv, entry->value);
	} else {
		ZVAL_NULL(&value_zv);
	}

	zend_hash_next_index_insert(Z_ARRVAL(iterator->current_value), &key_zv);
	zend_hash_next_index_insert(Z_ARRVAL(iterator->current_value), &value_zv);

	return &iterator->current_value;
}

static void php_uri_query_params_iterator_move_forward(zend_object_iterator *iter)
{
	php_uri_query_params_iterator *iterator = (php_uri_query_params_iterator *) iter;
	if (php_uri_query_params_iterator_current_entry(iterator) != NULL) {
		iterator->current_index++;
	}
}

static void php_uri_query_params_iterator_rewind(zend_object_iterator *iter)
{
	php_uri_query_params_iterator *iterator = (php_uri_query_params_iterator *) iter;
	iterator->current_index = 0;
}

static const zend_object_iterator_funcs php_uri_query_params_iterator_funcs = {
	.dtor = php_uri_query_params_iterator_dtor,
	.valid = php_uri_query_params_iterator_valid,
	.get_current_data = php_uri_query_params_iterator_current_data,
	.get_current_key = NULL,
	.move_forward = php_uri_query_params_iterator_move_forward,
	.rewind = php_uri_query_params_iterator_rewind,
	.invalidate_current = NULL,
	.get_gc = NULL,
};

ZEND_ATTRIBUTE_NONNULL zend_object_iterator *php_uri_object_query_params_get_iterator(
	zend_class_entry *ce, zval *object, const int by_ref)
{
	if (by_ref) {
		zend_throw_error(NULL, "An iterator cannot be used with foreach by reference");
		return NULL;
	}

	php_uri_query_params_iterator *iterator = emalloc(sizeof(php_uri_query_params_iterator));

	zend_iterator_init(&iterator->intern);
	iterator->intern.funcs = &php_uri_query_params_iterator_funcs;

	ZVAL_OBJ_COPY(&iterator->intern.data, Z_OBJ_P(object));
	iterator->query_params_object = Z_URI_QUERY_PARAMS_OBJECT_P(object);
	iterator->current_index = 0;
	ZVAL_UNDEF(&iterator->current_value);

	return &iterator->intern;
}

static void php_uri_query_param_list_entry_dtor(zval *zv)
{
	php_uri_query_param_list_entry *list_entry = Z_PTR_P(zv);

	if (list_entry->key != NULL) {
		zend_string_release(list_entry->key);
	}

	if (list_entry->value != NULL) {
		zend_string_release(list_entry->value);
	}

	efree(list_entry);
}

static void php_uri_query_param_lookup_entry_dtor(zval *zv)
{
	efree(Z_PTR_P(zv));
}

ZEND_ATTRIBUTE_NONNULL static void php_uri_query_params_object_init(php_uri_query_params_object *object)
{
	ZEND_ASSERT(!object->is_initialized);
	zend_hash_init(&object->entry_list, 8, NULL, php_uri_query_param_list_entry_dtor, 0);
	zend_hash_init(&object->entry_lookup_table, 8, NULL, php_uri_query_param_lookup_entry_dtor, 0);
	object->is_initialized = true;
}

ZEND_ATTRIBUTE_NONNULL_ARGS(1, 3) zend_result php_uri_query_params_parse(
	const char *query_str_first, const size_t query_str_length, php_uri_query_params_object *query_params_object,
	const php_uri_query_params_parsing_method parsing_method, const bool silent, const zend_long max_param_count
) {
	php_uri_query_params_object_init(query_params_object);

	/* Validate the encoded query component, not the decoded parameter bytes. */
	if (parsing_method == PHP_URI_QUERY_PARAMS_PARSING_METHOD_RFC3986
		&& !php_uri_parser_rfc3986_query_is_well_formed(query_str_first, query_str_length)
	) {
		if (!silent) {
			zend_throw_exception(php_uri_ce_invalid_query_params_exception, "The specified query string is malformed", 0);
		}
		return FAILURE;
	}

	const char *p = query_str_first;
	const char *end = query_str_first + query_str_length;

	/* The URLSearchParams constructor removes exactly one leading question mark. */
	if (parsing_method == PHP_URI_QUERY_PARAMS_PARSING_METHOD_WHATWG_URL_SEARCH_PARAMS && p < end && *p == '?') {
		p++;
	}

	if (p == end) {
		return SUCCESS;
	}

	const char *ampersand;
	do {
		const char *pair_start = p;
		ampersand = memchr(p, '&', (size_t)(end - p));
		const char *pair_end = ampersand != NULL ? ampersand : end;
		const size_t pair_len = (size_t)(pair_end - pair_start);
		p = ampersand != NULL ? ampersand + 1 : end;

		if (pair_len == 0
			&& (parsing_method == PHP_URI_QUERY_PARAMS_PARSING_METHOD_WHATWG_URL
				|| parsing_method == PHP_URI_QUERY_PARAMS_PARSING_METHOD_WHATWG_URL_SEARCH_PARAMS)
		) {
			continue;
		}

		if (UNEXPECTED(max_param_count > 0 && zend_hash_num_elements(&query_params_object->entry_list) >= max_param_count)) {
			if (!silent) {
				zend_throw_exception(php_uri_ce_invalid_query_params_exception, "The specified query string contains too many parameters", 0);
			}
			return FAILURE;
		}

		const char *eq = memchr(pair_start, '=', pair_len);

		const char *name_start;
		size_t name_len;
		const char *value_start;
		size_t value_len;

		if (eq == NULL) {
			name_start = pair_start;
			name_len = pair_len;
			value_start = NULL;
			value_len = 0;
		} else {
			name_start = pair_start;
			name_len = (size_t)(eq - pair_start);
			value_start = eq + 1;
			value_len = (size_t) (pair_end - value_start);
		}

		zend_string *name;
		zend_string *value;

		switch (parsing_method) {
			case PHP_URI_QUERY_PARAMS_PARSING_METHOD_RFC1866:
				name = php_uri_parser_rfc3986_query_component_percent_decode(name_start, name_len, true);
				value = php_uri_parser_rfc3986_query_component_percent_decode(value_start, value_len, true);
				break;
			case PHP_URI_QUERY_PARAMS_PARSING_METHOD_RFC3986:
				name = php_uri_parser_rfc3986_query_component_percent_decode(name_start, name_len, false);
				value = php_uri_parser_rfc3986_query_component_percent_decode(value_start, value_len, false);
				break;
			case PHP_URI_QUERY_PARAMS_PARSING_METHOD_WHATWG_URL:
			case PHP_URI_QUERY_PARAMS_PARSING_METHOD_WHATWG_URL_SEARCH_PARAMS:
				name = php_uri_parser_whatwg_query_component_percent_decode(name_start, name_len);
				if (UNEXPECTED(name == NULL)) {
					ZEND_ASSERT(ZEND_HAS_OOM());
					zend_handle_delayed_oom();
					return FAILURE;
				}
				if (value_start == NULL) {
					value = ZSTR_EMPTY_ALLOC();
				} else {
					value = php_uri_parser_whatwg_query_component_percent_decode(value_start, value_len);
					if (UNEXPECTED(value == NULL)) {
						zend_string_release(name);
						ZEND_ASSERT(ZEND_HAS_OOM());
						zend_handle_delayed_oom();
						return FAILURE;
					}
				}
				break;
			default: ZEND_UNREACHABLE();
		}

		const zend_result result = php_uri_query_params_append_entry(query_params_object, name, value);

		zend_string_release(name);
		if (value != NULL) {
			zend_string_release(value);
		}

		if (UNEXPECTED(result == FAILURE)) {
			if (!silent) {
				zend_throw_exception(php_uri_ce_invalid_query_params_exception, "Failed to parse the specified query string", 0);
			}

			return FAILURE;
		}
	} while (ampersand != NULL);

	return SUCCESS;
}

ZEND_ATTRIBUTE_NONNULL static void throw_cannot_recompose_uri_to_string(const zend_object *object)
{
	zend_throw_exception_ex(php_uri_ce_error, 0, "Cannot recompose %pS to a string", object->ce->name);
}

ZEND_ATTRIBUTE_NONNULL static zend_string *php_uri_query_params_to_rfc1866_string(const php_uri_query_params_object *uri_query_params_object)
{
	return php_uri_parser_rfc3986_query_params_to_string(uri_query_params_object, true);
}

ZEND_ATTRIBUTE_NONNULL zend_string *php_uri_query_params_to_rfc3986_string(const php_uri_query_params_object *uri_query_params_object)
{
	return php_uri_parser_rfc3986_query_params_to_string(uri_query_params_object, false);
}

ZEND_ATTRIBUTE_NONNULL zend_string *php_uri_query_params_to_whatwg_string(const php_uri_query_params_object *uri_query_params_object)
{
	return php_uri_parser_whatwg_query_params_to_string(uri_query_params_object);
}

ZEND_ATTRIBUTE_NONNULL static HashTable *php_uri_query_params_get_debug_properties(php_uri_query_params_object *object)
{
	HashTable *result = zend_new_array(1);

	zval params;
	php_uri_query_params_list_entries(object, &params);

	zend_hash_str_add(result, ZEND_STRL("params"), &params);

	return result;
}

static void uri_query_params_parse(INTERNAL_FUNCTION_PARAMETERS, const php_uri_query_params_parsing_method parsing_method)
{
	zend_string *query_str;
	zend_long max_param_count = PHP_URI_QUERY_PARAMS_DEFAULT_MAX_PARAM_COUNT;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(query_str)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(max_param_count)
	ZEND_PARSE_PARAMETERS_END();

	if (max_param_count < 0) {
		zend_argument_value_error(2, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	object_init_ex(return_value, zend_get_called_scope(execute_data));
	php_uri_query_params_object *uri_query_params_object = Z_URI_QUERY_PARAMS_OBJECT_P(return_value);

	if (php_uri_query_params_parse(ZSTR_VAL(query_str), ZSTR_LEN(query_str), uri_query_params_object,
		parsing_method, false, max_param_count
	) == FAILURE) {
		RETURN_THROWS();
	}
}

PHP_METHOD(Uri_QueryParams, parseRfc1866)
{
	uri_query_params_parse(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_URI_QUERY_PARAMS_PARSING_METHOD_RFC1866);
}

PHP_METHOD(Uri_QueryParams, parseRfc3986)
{
	uri_query_params_parse(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_URI_QUERY_PARAMS_PARSING_METHOD_RFC3986);
}

PHP_METHOD(Uri_QueryParams, parseWhatWgUrlSearchParams)
{
	uri_query_params_parse(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_URI_QUERY_PARAMS_PARSING_METHOD_WHATWG_URL_SEARCH_PARAMS);
}

PHP_METHOD(Uri_QueryParams, fromArray)
{
	HashTable *query_params_arr;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(query_params_arr)
	ZEND_PARSE_PARAMETERS_END();

	object_init_ex(return_value, zend_get_called_scope(execute_data));
	php_uri_query_params_object *object = Z_URI_QUERY_PARAMS_OBJECT_P(return_value);

	php_uri_query_params_object_init(object);

	if (php_uri_query_params_append_array(object, query_params_arr, NULL) == FAILURE) {
		RETURN_THROWS();
	}
}

PHP_METHOD(Uri_QueryParams, fromList)
{
	HashTable *list;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(list)
	ZEND_PARSE_PARAMETERS_END();

	if (!zend_array_is_list(list)) {
		zend_argument_value_error(1, "must be a list of [name, value] pairs");
		RETURN_THROWS();
	}

	object_init_ex(return_value, zend_get_called_scope(execute_data));
	php_uri_query_params_object *object = Z_URI_QUERY_PARAMS_OBJECT_P(return_value);
	php_uri_query_params_object_init(object);

	ZEND_HASH_FOREACH_NUM_KEY_VAL(list, const zend_ulong index, const zval *pair) {
		ZVAL_DEREF(pair);

		if (Z_TYPE_P(pair) != IS_ARRAY) {
			zend_argument_type_error(1, "must contain an array at index " ZEND_ULONG_FMT ", %s given",
				index, zend_zval_value_name(pair));
			RETURN_THROWS();
		}

		const HashTable *fields = Z_ARRVAL_P(pair);
		if (zend_hash_num_elements(fields) != 2 || !zend_array_is_list(fields)) {
			zend_argument_value_error(1, "must contain a [name, value] pair at index " ZEND_ULONG_FMT, index);
			RETURN_THROWS();
		}

		const zval *name = zend_hash_index_find(fields, 0);
		ZVAL_DEREF(name);
		if (Z_TYPE_P(name) != IS_STRING) {
			zend_argument_type_error(1, "must contain a string parameter name at index " ZEND_ULONG_FMT ", %s given",
				index, zend_zval_value_name(name));
			RETURN_THROWS();
		}

		zend_string *name_str = zend_string_copy(Z_STR_P(name));
		const zend_result result = php_uri_query_params_append_array_value(object, name_str, zend_hash_index_find(fields, 1));
		zend_string_release(name_str);
		if (UNEXPECTED(result == FAILURE)) {
			RETURN_THROWS();
		}
	} ZEND_HASH_FOREACH_END();
}

PHP_METHOD(Uri_QueryParams, __construct)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_uri_query_params_object *uri_query_params_object = Z_URI_QUERY_PARAMS_OBJECT_P(ZEND_THIS);
	if (uri_query_params_object->is_initialized) {
		zend_throw_error(NULL, "Cannot modify readonly object of class %pS", uri_query_params_object->std.ce->name);
		RETURN_THROWS();
	}

	php_uri_query_params_object_init(uri_query_params_object);
}

PHP_METHOD(Uri_QueryParams, has)
{
	zend_string *name;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	const php_uri_query_params_object *uri_query_params_object = Z_URI_QUERY_PARAMS_OBJECT_P(ZEND_THIS);

	RETVAL_BOOL(php_uri_query_params_has_entry(uri_query_params_object, name));
}

PHP_METHOD(Uri_QueryParams, hasValue)
{
	zend_string *name;
	zval *value;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_STR(name)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	const php_uri_query_params_object *uri_query_params_object = Z_URI_QUERY_PARAMS_OBJECT_P(ZEND_THIS);

	zend_string *value_str = php_uri_query_params_convert_value_to_str(value, NULL);
	if (EG(exception)) {
		RETURN_THROWS();
	}

	RETVAL_BOOL(php_uri_query_params_has_entry_with_value(uri_query_params_object, name, value_str));

	if (value_str != NULL) {
		zend_string_release(value_str);
	}
}

PHP_METHOD(Uri_QueryParams, getFirst)
{
	zend_string *name;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	const php_uri_query_params_object *uri_query_params_object = Z_URI_QUERY_PARAMS_OBJECT_P(ZEND_THIS);

	php_uri_query_params_find_first_value(uri_query_params_object, name, return_value);
}

PHP_METHOD(Uri_QueryParams, getLast)
{
	zend_string *name;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	const php_uri_query_params_object *uri_query_params_object = Z_URI_QUERY_PARAMS_OBJECT_P(ZEND_THIS);

	php_uri_query_params_find_last_value(uri_query_params_object, name, return_value);
}

PHP_METHOD(Uri_QueryParams, getAll)
{
	zend_string *name;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	const php_uri_query_params_object *uri_query_params_object = Z_URI_QUERY_PARAMS_OBJECT_P(ZEND_THIS);

	php_uri_query_params_find_all_values(uri_query_params_object, name, return_value);
}

PHP_METHOD(Uri_QueryParams, toList)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_uri_query_params_object *uri_query_params_object = Z_URI_QUERY_PARAMS_OBJECT_P(ZEND_THIS);

	php_uri_query_params_list_entries(uri_query_params_object, return_value);
}

PHP_METHOD(Uri_QueryParams, getIterator)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_create_internal_iterator_zval(return_value, ZEND_THIS);
}

PHP_METHOD(Uri_QueryParams, count)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_uri_query_params_object *uri_query_params_object = Z_URI_QUERY_PARAMS_OBJECT_P(ZEND_THIS);

	RETVAL_LONG(zend_array_count(&uri_query_params_object->entry_list) - uri_query_params_object->tombstone_count);
}

PHP_METHOD(Uri_QueryParams, append)
{
	zend_string *name;
	zval *value;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_STR(name)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	zend_string *value_str = php_uri_query_params_convert_value_to_str(value, NULL);
	if (EG(exception)) {
		RETURN_THROWS();
	}

	php_uri_query_params_object *new_object = php_uri_query_params_object_clone_for_modification(Z_OBJ_P(ZEND_THIS), return_value);
	if (new_object == NULL) {
		if (value_str != NULL) {
			zend_string_release(value_str);
		}
		RETURN_THROWS();
	}

	const zend_result result = php_uri_query_params_append_entry(new_object, name, value_str);
	if (value_str != NULL) {
		zend_string_release(value_str);
	}
	if (UNEXPECTED(result == FAILURE)) {
		/* This should be unreachable in practice. */
		zend_throw_exception(php_uri_ce_error, "Failed to append the query parameter", 0);
		RETURN_THROWS();
	}
}

PHP_METHOD(Uri_QueryParams, set)
{
	zend_string *name;
	zval *value;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_STR(name)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	zend_string *value_str = php_uri_query_params_convert_value_to_str(value, NULL);
	if (EG(exception)) {
		RETURN_THROWS();
	}

	php_uri_query_params_object *new_object = php_uri_query_params_object_clone_for_modification(Z_OBJ_P(ZEND_THIS), return_value);
	if (new_object == NULL) {
		if (value_str != NULL) {
			zend_string_release(value_str);
		}
		RETURN_THROWS();
	}

	const zend_result result = php_uri_query_params_set_entry(new_object, name, value_str);
	if (value_str != NULL) {
		zend_string_release(value_str);
	}
	if (UNEXPECTED(result == FAILURE)) {
		/* This should be unreachable in practice. */
		zend_throw_exception(php_uri_ce_error, "Failed to set the query parameter", 0);
		RETURN_THROWS();
	}
}

PHP_METHOD(Uri_QueryParams, delete)
{
	zend_string *name;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	php_uri_query_params_object *new_object = php_uri_query_params_object_clone_for_modification(Z_OBJ_P(ZEND_THIS), return_value);
	if (new_object == NULL) {
		RETURN_THROWS();
	}

	php_uri_query_params_delete_entries(new_object, name);
}

PHP_METHOD(Uri_QueryParams, deleteValue)
{
	zend_string *name;
	zval *value;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_STR(name)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	zend_string *value_str = php_uri_query_params_convert_value_to_str(value, NULL);
	if (EG(exception)) {
		RETURN_THROWS();
	}

	php_uri_query_params_object *new_object = php_uri_query_params_object_clone_for_modification(Z_OBJ_P(ZEND_THIS), return_value);
	if (new_object == NULL) {
		if (value_str != NULL) {
			zend_string_release(value_str);
		}
		RETURN_THROWS();
	}

	php_uri_query_params_delete_entries_with_value(new_object, name, value_str);
	if (value_str != NULL) {
		zend_string_release(value_str);
	}
}

static void php_uri_query_params_sort(INTERNAL_FUNCTION_PARAMETERS, const bucket_compare_func_t compare)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_uri_query_params_object *new_object = php_uri_query_params_object_clone_for_modification(Z_OBJ_P(ZEND_THIS), return_value);
	if (new_object == NULL) {
		RETURN_THROWS();
	}

	php_uri_query_params_sort_entries(new_object, compare);
}

PHP_METHOD(Uri_QueryParams, sortByBytes)
{
	php_uri_query_params_sort(INTERNAL_FUNCTION_PARAM_PASSTHRU, php_uri_query_params_compare_bytes);
}

PHP_METHOD(Uri_QueryParams, sortByUtf16)
{
	php_uri_query_params_sort(INTERNAL_FUNCTION_PARAM_PASSTHRU, php_uri_query_params_compare_utf16);
}

/* Return zero for literal names, including names with malformed bracket notation. */
ZEND_ATTRIBUTE_NONNULL static size_t php_uri_query_params_array_key_depth(
	const zend_string *name, size_t *root_len
) {
	const char *key = ZSTR_VAL(name);
	const char *opening_bracket = memchr(key, '[', ZSTR_LEN(name));
	*root_len = ZSTR_LEN(name);

	/* Either no opening bracket was found, or a closing bracket precedes it */
	if (opening_bracket == NULL || memchr(key, ']', opening_bracket - key) != NULL) {
		return 0;
	}

	size_t depth = 0;
	const char *p = opening_bracket;

	const char *end = key + ZSTR_LEN(name);
	while (p < end) {
		if (*p++ != '[') {
			return 0;
		}

		while (p < end && *p != ']') {
			/* Check if there is an opening bracket before the previous one was closed */
			if (*p++ == '[') {
				return 0;
			}
		}

		/* Check whether we reached the end of the input before finding the closing bracket */
		if (p == end) {
			return 0;
		}

		p++;
		depth++;
	}

	*root_len = opening_bracket - key;

	return depth;
}

ZEND_ATTRIBUTE_NONNULL static zend_result php_uri_query_params_add_entry_to_array(
	HashTable *result, const php_uri_query_params_object *object,
	const php_uri_query_param_list_entry *entry, zend_long max_nesting_level
) {
	size_t root_len;
	size_t depth = php_uri_query_params_array_key_depth(entry->key, &root_len);
	const char *key = ZSTR_VAL(entry->key);
	const char *end = key + ZSTR_LEN(entry->key);

	if (depth == 0) {
		const php_uri_query_param_lookup_entry *lookup_entry = zend_hash_find_ptr(&object->entry_lookup_table, entry->key);
		bool is_list = lookup_entry->first_index != lookup_entry->last_index;
		if (is_list && max_nesting_level == 0) {
			goto nesting_error;
		}

		zval value;
		if (entry->value == NULL) {
			ZVAL_NULL(&value);
		} else {
			ZVAL_STR_COPY(&value, entry->value);
		}

		if (is_list) {
			zval *list = zend_symtable_str_find(result, key, root_len);
			if (list == NULL) {
				zval new_list;
				array_init(&new_list);
				list = zend_symtable_str_update(result, key, root_len, &new_list);
			}
			zend_hash_next_index_insert(Z_ARRVAL_P(list), &value);
		} else {
			zend_symtable_str_update(result, key, root_len, &value);
		}

		return SUCCESS;
	}

	if (depth > (zend_ulong) max_nesting_level) {
		goto nesting_error;
	}

	/* Reject bracket notation if the root also appears as a plain parameter name, e.g. "foo=1&foo[bar]=2". */
	if (zend_hash_str_exists(&object->entry_lookup_table, key, root_len)) {
		goto conflict_error;
	}

	HashTable *current = result;
	const char *segment = key;
	size_t segment_len = root_len;
	const char *next = key + root_len;
	bool append = false;

	while (true) {
		bool is_leaf = next == end;
		zval *existing = append ? NULL : zend_symtable_str_find(current, segment, segment_len);
		if (existing == NULL) {
			zval value;

			if (!is_leaf) {
				array_init(&value);
			} else if (entry->value == NULL) {
				ZVAL_NULL(&value);
			} else {
				ZVAL_STR_COPY(&value, entry->value);
			}

			if (append) {
				existing = zend_hash_next_index_insert(current, &value);
			} else {
				existing = zend_symtable_str_update(current, segment, segment_len, &value);
			}

			if (UNEXPECTED(existing == NULL)) {
				zval_ptr_dtor(&value);
				zend_throw_exception_ex(php_uri_ce_invalid_query_params_exception, 0,
					"Cannot append an array element for query parameter \"%pS\"", entry->key);
				return FAILURE;
			}
		} else {
			if (is_leaf || Z_TYPE_P(existing) != IS_ARRAY) {
				goto conflict_error;
			}
		}

		if (is_leaf) {
			return SUCCESS;
		}

		current = Z_ARRVAL_P(existing);
		segment = next + 1;
		const char *close = memchr(segment, ']', end - segment);
		ZEND_ASSERT(close != NULL);
		segment_len = close - segment;
		append = segment_len == 0;
		next = close + 1;
	}

nesting_error:
	zend_throw_exception_ex(php_uri_ce_invalid_query_params_exception, 0,
		"The query parameter \"%pS\" exceeds the maximum nesting level of " ZEND_LONG_FMT,
		entry->key, max_nesting_level);
	return FAILURE;

conflict_error:
	zend_throw_exception_ex(php_uri_ce_invalid_query_params_exception, 0,
		"The query parameter \"%pS\" has conflicting array paths", entry->key);
	return FAILURE;
}

PHP_METHOD(Uri_QueryParams, withArray)
{
	HashTable *query_params_arr;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(query_params_arr)
	ZEND_PARSE_PARAMETERS_END();

	php_uri_query_params_object *object;

	if (php_uri_query_params_object_can_be_reused(Z_OBJ_P(ZEND_THIS))) {
		ZVAL_OBJ_COPY(return_value, Z_OBJ_P(ZEND_THIS));
		object = Z_URI_QUERY_PARAMS_OBJECT_P(return_value);
		zend_hash_clean(&object->entry_list);
		zend_hash_clean(&object->entry_lookup_table);
		object->tombstone_count = 0;
	} else {
		object_init_ex(return_value, Z_OBJCE_P(ZEND_THIS));
		object = Z_URI_QUERY_PARAMS_OBJECT_P(return_value);
		php_uri_query_params_object_init(object);
	}

	if (php_uri_query_params_append_array(object, query_params_arr, NULL) == FAILURE) {
		RETURN_THROWS();
	}
}

PHP_METHOD(Uri_QueryParams, toArray)
{
	zend_long max_nesting_level = 64;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(max_nesting_level)
	ZEND_PARSE_PARAMETERS_END();

	if (max_nesting_level < 0) {
		zend_argument_value_error(1, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	const php_uri_query_params_object *object = Z_URI_QUERY_PARAMS_OBJECT_P(ZEND_THIS);

	zend_array *result = zend_new_array(0);
	ZEND_HASH_FOREACH_PTR(&object->entry_list, php_uri_query_param_list_entry *entry) {
		if (entry->key == NULL) {
			continue;
		}

		if (php_uri_query_params_add_entry_to_array(result, object, entry, max_nesting_level) == FAILURE) {
			zend_array_destroy(result);
			RETURN_THROWS();
		}
	} ZEND_HASH_FOREACH_END();

	RETURN_ARR(result);
}

PHP_METHOD(Uri_QueryParams, toRfc1866String)
{
	ZEND_PARSE_PARAMETERS_NONE();

	const php_uri_query_params_object *uri_query_params_object = Z_URI_QUERY_PARAMS_OBJECT_P(ZEND_THIS);

	zend_string *result = php_uri_query_params_to_rfc1866_string(uri_query_params_object);
	if (result == NULL) {
		throw_cannot_recompose_uri_to_string(Z_OBJ_P(ZEND_THIS));
		RETURN_THROWS();
	}

	ZVAL_STR(return_value, result);
}

PHP_METHOD(Uri_QueryParams, toRfc3986String)
{
	ZEND_PARSE_PARAMETERS_NONE();

	const php_uri_query_params_object *uri_query_params_object = Z_URI_QUERY_PARAMS_OBJECT_P(ZEND_THIS);

	zend_string *result = php_uri_query_params_to_rfc3986_string(uri_query_params_object);
	if (result == NULL) {
		throw_cannot_recompose_uri_to_string(Z_OBJ_P(ZEND_THIS));
		RETURN_THROWS();
	}

	ZVAL_STR(return_value, result);
}

PHP_METHOD(Uri_QueryParams, toWhatWgUrlString)
{
	ZEND_PARSE_PARAMETERS_NONE();

	const php_uri_query_params_object *uri_query_params_object = Z_URI_QUERY_PARAMS_OBJECT_P(ZEND_THIS);

	zend_string *result = php_uri_query_params_to_whatwg_string(uri_query_params_object);
	if (result == NULL) {
		throw_cannot_recompose_uri_to_string(Z_OBJ_P(ZEND_THIS));
		RETURN_THROWS();
	}

	ZVAL_STR(return_value, result);
}

PHP_METHOD(Uri_QueryParams, __serialize)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_uri_query_params_object *uri_query_params_object = Z_URI_QUERY_PARAMS_OBJECT_P(ZEND_THIS);

	/* Serialize the parameter list without recomposing or converting its values. */
	zval params;
	php_uri_query_params_list_entries(uri_query_params_object, &params);

	array_init(return_value);

	zval arr;
	array_init(&arr);
	zend_hash_str_add_new(Z_ARRVAL(arr), ZEND_STRL(PHP_URI_SERIALIZE_QUERY_PARAMS_FIELD_NAME), &params);
	zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &arr);

	/* Serialize regular properties: second array */
	ZVAL_EMPTY_ARRAY(&arr);
	zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &arr);
}

static void uri_query_params_unserialize(INTERNAL_FUNCTION_PARAMETERS)
{
	HashTable *data;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(data)
	ZEND_PARSE_PARAMETERS_END();

	php_uri_query_params_object *uri_query_params_object = Z_URI_QUERY_PARAMS_OBJECT_P(ZEND_THIS);
	if (uri_query_params_object->is_initialized) {
		/* Intentionally throw two exceptions for proper chaining. */
		zend_throw_error(NULL, "Cannot modify readonly object of class %pS", uri_query_params_object->std.ce->name);
		zend_throw_exception_ex(NULL, 0, "Invalid serialization data for %pS object", uri_query_params_object->std.ce->name);
		RETURN_THROWS();
	}

	/* Verify the expected number of elements, this implicitly ensures that no additional elements are present. */
	if (zend_hash_num_elements(data) != 2) {
		goto invalid_data;
	}

	/* Validate the complete state before initializing the readonly object. */
	const zval *arr = zend_hash_index_find(data, 0);
	if (arr == NULL || Z_TYPE_P(arr) != IS_ARRAY || zend_hash_num_elements(Z_ARRVAL_P(arr)) != 1) {
		goto invalid_data;
	}

	const zval *params = zend_hash_str_find(Z_ARRVAL_P(arr), ZEND_STRL(PHP_URI_SERIALIZE_QUERY_PARAMS_FIELD_NAME));
	if (params == NULL || Z_TYPE_P(params) != IS_ARRAY || !zend_array_is_list(Z_ARRVAL_P(params))) {
		goto invalid_data;
	}

	ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(params), zval *param) {
		if (Z_TYPE_P(param) != IS_ARRAY || zend_hash_num_elements(Z_ARRVAL_P(param)) != 2) {
			goto invalid_data;
		}
		const zval *name = zend_hash_index_find(Z_ARRVAL_P(param), 0);
		const zval *value = zend_hash_index_find(Z_ARRVAL_P(param), 1);
		if (name == NULL || Z_TYPE_P(name) != IS_STRING
			|| value == NULL || (Z_TYPE_P(value) != IS_STRING && Z_TYPE_P(value) != IS_NULL)) {
			goto invalid_data;
		}
	} ZEND_HASH_FOREACH_END();

	arr = zend_hash_index_find(data, 1);
	if (arr == NULL || Z_TYPE_P(arr) != IS_ARRAY || zend_hash_num_elements(Z_ARRVAL_P(arr)) != 0) {
		goto invalid_data;
	}

	php_uri_query_params_object_init(uri_query_params_object);

	ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(params), zval *param) {
		const zval *name = zend_hash_index_find(Z_ARRVAL_P(param), 0);
		const zval *value = zend_hash_index_find(Z_ARRVAL_P(param), 1);
		if (php_uri_query_params_append_entry(uri_query_params_object, Z_STR_P(name),
			Z_TYPE_P(value) == IS_NULL ? NULL : Z_STR_P(value)) == FAILURE) {
			/* This should be unreachable in practice. */
			zend_throw_exception(php_uri_ce_error, "Failed to restore the query parameters", 0);
			RETURN_THROWS();
		}
	} ZEND_HASH_FOREACH_END();

	return;

invalid_data:
	zend_throw_exception_ex(NULL, 0, "Invalid serialization data for %pS object", uri_query_params_object->std.ce->name);
	RETURN_THROWS();
}

PHP_METHOD(Uri_QueryParams, __unserialize)
{
	uri_query_params_unserialize(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

PHP_METHOD(Uri_QueryParams, __debugInfo)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_uri_query_params_object *uri_query_params_object = Z_URI_QUERY_PARAMS_OBJECT_P(ZEND_THIS);

	RETURN_ARR(php_uri_query_params_get_debug_properties(uri_query_params_object));
}
