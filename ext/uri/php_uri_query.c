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
#include "../../Zend/zend_hash.h"
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

#define MAX_TOMBSTONES 1000

#define PARSING_MAX_QUERY_STRING_LENGTH_DEFAULT 10000
#define PARSING_MAX_QUERY_PARAM_COUNT_DEFAULT 1000
#define USE_NULL_AS_EMPTY_STRING_DEFAULT false

typedef enum php_uri_query_params_spec {
	PHP_URI_QUERY_PARAMS_SPEC_RFC1866,
	PHP_URI_QUERY_PARAMS_SPEC_RFC3986,
	PHP_URI_QUERY_PARAMS_SPEC_WHATWG,
} php_uri_query_params_spec;

ZEND_ATTRIBUTE_NONNULL_ARGS(1) static zend_result php_uri_query_param_options_update_properties(
	zend_object *query_param_options_object, const zend_long parsing_max_query_string_length, const zend_long parsing_max_param_count,
	zend_string *true_value, zend_string *false_value, const bool use_null_as_empty_string
) {
	php_uri_query_param_options_object *options = php_uri_query_param_options_object_from_obj(query_param_options_object);
	if (options->is_initialized) {
		zend_throw_error(NULL, "Cannot modify readonly object of class %s", ZSTR_VAL(query_param_options_object->ce->name));
		return FAILURE;
	}

	options->parsing_max_query_string_length = parsing_max_query_string_length;
	options->parsing_max_param_count = parsing_max_param_count;

	if (true_value == NULL) {
		options->true_value = zend_string_init(ZEND_STRL("1"), false);
	} else {
		options->true_value = true_value;
	}

	if (false_value == NULL) {
		options->false_value = zend_string_init(ZEND_STRL("0"), false);
	} else {
		options->false_value = false_value;
	}

	options->use_null_as_string = use_null_as_empty_string;
	options->is_initialized = true;

	return SUCCESS;
}

ZEND_ATTRIBUTE_NONNULL_ARGS(1,2) static zend_string *php_uri_query_params_convert_value_to_str(
	const php_uri_query_params_object *query_params_object, const zval *zv
) {
	switch (Z_TYPE_P(zv)) {
		case IS_STRING: {
			return zend_string_copy(Z_STR_P(zv));
		}
		case IS_FALSE:
			ZEND_FALLTHROUGH;
		case IS_TRUE: {
			return zend_string_init(Z_TYPE_P(zv) == IS_FALSE ? "0" : "1", 1, false);
		}
		case IS_LONG: {
			return zend_long_to_str(Z_LVAL_P(zv));
		}
		case IS_DOUBLE: {
			return zend_double_to_str(Z_LVAL_P(zv));
		}
		case IS_NULL: {
			return NULL;
		}
		case IS_RESOURCE:
			zend_argument_value_error(2, "must not contain a resource");
			return NULL;
		case IS_ARRAY: {
			zend_argument_value_error(2, "must not contain an arra");
			return NULL;
		}
		case IS_OBJECT:
			if (Z_OBJCE_P(zv)->ce_flags & ZEND_ACC_ENUM && Z_OBJCE_P(zv)->enum_backing_type != IS_UNDEF) {
				const zval *tmp = zend_enum_fetch_case_value(Z_OBJ_P(zv));
				return php_uri_query_params_convert_value_to_str(query_params_object, tmp);
			}

			zend_argument_value_error(2, "must not contain an object");
			return NULL;
		default: ZEND_UNREACHABLE();
	}
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
		) { // TODO properly handle search for NULL values according to options
			return true;
		}

		index = list_entry->next_same_key;
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
			zend_hash_next_index_insert(Z_ARR_P(zv), &value);
		}

		index = list_entry->next_same_key;
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
		.next_same_key = UINT32_MAX,
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

		last_list_entry->next_same_key = new_index;
		lookup_entry->last_index = new_index;
	}

	return SUCCESS;
}

ZEND_ATTRIBUTE_NONNULL static void php_uri_query_params_cleanup_tombstones(
	php_uri_query_params_object *query_params_object
) {
	if (query_params_object->tombstone_count <= MAX_TOMBSTONES) {
		return;
	}

	HashTable new_entry_list;
	zend_hash_init(&new_entry_list, query_params_object->tombstone_count, NULL, query_params_object->entry_list.pDestructor, 0);

	HashTable last_seen_index;
	zend_hash_init(&last_seen_index, 8, NULL, NULL, 0);

	php_uri_query_param_list_entry *old_entry;
	ZEND_HASH_FOREACH_PTR(&query_params_object->entry_list, old_entry) {
		if (old_entry->key == NULL) {
			continue;
		}

		uint32_t new_index = zend_hash_num_elements(&new_entry_list);

		php_uri_query_param_list_entry new_entry = {
			.key = old_entry->key,
			.value = old_entry->value,
			.next_same_key = UINT32_MAX,
		};
		zend_hash_next_index_insert_mem(&new_entry_list, &new_entry, sizeof(new_entry));

		php_uri_query_param_lookup_entry *lookup_entry = zend_hash_find_ptr(&query_params_object->entry_lookup_table, old_entry->key);
		ZEND_ASSERT(lookup_entry != NULL);

		zval *last_index_zv = zend_hash_find(&last_seen_index, old_entry->key);

		if (last_index_zv == NULL) {
			lookup_entry->first_index = new_index;
		} else {
			uint32_t prev_index = (uint32_t) Z_LVAL_P(last_index_zv);
			php_uri_query_param_list_entry *prev_entry = zend_hash_index_find_ptr(&new_entry_list, prev_index);
			ZEND_ASSERT(prev_entry != NULL);
			prev_entry->next_same_key = new_index;
		}

		lookup_entry->last_index = new_index;

		zval new_last_zv;
		ZVAL_LONG(&new_last_zv, (zend_long) new_index);
		zend_hash_update(&last_seen_index, old_entry->key, &new_last_zv);

	} ZEND_HASH_FOREACH_END();

	zend_hash_destroy(&last_seen_index);
	zend_hash_destroy(&query_params_object->entry_list);
	query_params_object->entry_list = new_entry_list;
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
	list_entry->next_same_key = UINT32_MAX;
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

		const uint32_t next = list_entry->next_same_key;
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

		const uint32_t next = list_entry->next_same_key;

		const bool should_delete = (value == NULL && list_entry->value == NULL) ||
			(value != NULL && list_entry->value != NULL && zend_string_equal_content(value, list_entry->value));

		if (should_delete) {
			php_uri_query_params_tombstone_entry(query_params_object, list_entry);

			/* Rebuild the same key list */
			if (prev_list_entry != NULL) {
				prev_list_entry->next_same_key = next;
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
		last_entry->next_same_key = UINT32_MAX;

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
	uint32_t index = first_entry->next_same_key;
	first_entry->next_same_key = UINT32_MAX;

	while (index != UINT32_MAX) {
		php_uri_query_param_list_entry *list_entry = zend_hash_index_find_ptr(&query_params_object->entry_list, index);
		ZEND_ASSERT(list_entry != NULL);

		const uint32_t next = list_entry->next_same_key;
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

ZEND_ATTRIBUTE_NONNULL zend_object *php_uri_query_params_object_handler_clone(zend_object *object)
{
	const php_uri_query_params_object *uri_query_params_object = php_uri_query_params_object_from_obj(object);

	php_uri_query_params_object *new_uri_query_params_object = php_uri_query_params_object_from_obj(
		object->ce->create_object(object->ce)
	);

	zend_hash_copy(&new_uri_query_params_object->entry_list, &uri_query_params_object->entry_list, NULL);
	zend_hash_copy(&new_uri_query_params_object->entry_lookup_table, &uri_query_params_object->entry_lookup_table, NULL);
	new_uri_query_params_object->tombstone_count = uri_query_params_object->tombstone_count;
	new_uri_query_params_object->is_initialized = uri_query_params_object->is_initialized;
	zend_objects_clone_members(&new_uri_query_params_object->std, &uri_query_params_object->std);

	return &new_uri_query_params_object->std;
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

static zend_result php_uri_query_params_iterator_valid(zend_object_iterator *iter)
{
	php_uri_query_params_iterator *iterator = (php_uri_query_params_iterator *) iter;
	php_uri_query_params_object *obj = iterator->query_params_object;

	if (!obj->is_initialized) {
		return FAILURE;
	}

	while (iterator->current_index < zend_hash_num_elements(&obj->entry_list)) {
		php_uri_query_param_list_entry *entry = zend_hash_index_find_ptr(
			&obj->entry_list, iterator->current_index);
		if (entry != NULL && entry->key != NULL) {
			return SUCCESS;
		}
		iterator->current_index++;
	}

	return FAILURE;
}

static zval *php_uri_query_params_iterator_current_data(zend_object_iterator *iter)
{
	php_uri_query_params_iterator *iterator = (php_uri_query_params_iterator *) iter;
	php_uri_query_params_object *obj = iterator->query_params_object;

	php_uri_query_param_list_entry *entry = zend_hash_index_find_ptr(
		&obj->entry_list, iterator->current_index);
	ZEND_ASSERT(entry != NULL && entry->key != NULL);

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

static void php_uri_query_params_iterator_current_key(
	zend_object_iterator *iter, zval *key)
{
	php_uri_query_params_iterator *iterator = (php_uri_query_params_iterator *) iter;
	ZVAL_LONG(key, (zend_long) iterator->current_index);
}

static void php_uri_query_params_iterator_move_forward(zend_object_iterator *iter)
{
	php_uri_query_params_iterator *iterator = (php_uri_query_params_iterator *) iter;
	iterator->current_index++;
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
	.get_current_key = php_uri_query_params_iterator_current_key,
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

ZEND_ATTRIBUTE_NONNULL_ARGS(1) static php_uri_query_param_options_object *php_uri_query_params_object_init(
	php_uri_query_params_object *uri_query_params_object, zval *options
) {
	if (!uri_query_params_object->is_initialized) {
		zend_hash_init(&uri_query_params_object->entry_list, 8, NULL, php_uri_query_param_list_entry_dtor, 0);
		zend_hash_init(&uri_query_params_object->entry_lookup_table, 8, NULL, php_uri_query_param_lookup_entry_dtor, 0);
		uri_query_params_object->is_initialized = true;
	}

	if (options == NULL) {
		zval default_options;
		object_init_ex(&default_options, php_uri_ce_query_param_options);

		if (php_uri_query_param_options_update_properties(
			Z_OBJ(default_options), PARSING_MAX_QUERY_STRING_LENGTH_DEFAULT,
			PARSING_MAX_QUERY_PARAM_COUNT_DEFAULT, NULL, NULL, USE_NULL_AS_EMPTY_STRING_DEFAULT
		)) {
			return NULL;
		}

		/* There is no need to update the properties as the default values are already set by the create handler */
		zend_update_property(uri_query_params_object->std.ce, &uri_query_params_object->std, ZEND_STRL("options"), &default_options);
		if (EG(exception)) {
			zval_ptr_dtor(&default_options);
			return NULL;
		}

		return Z_URI_QUERY_PARAM_OPTIONS_OBJECT_P(&default_options);
	}

	zend_update_property(uri_query_params_object->std.ce, &uri_query_params_object->std, ZEND_STRL("options"), options);
	if (EG(exception)) {
		return NULL;
	}
	GC_ADDREF(&uri_query_params_object->std);

	return Z_URI_QUERY_PARAM_OPTIONS_OBJECT_P(options);
}

ZEND_ATTRIBUTE_NONNULL void php_uri_query_param_options_object_free(php_uri_query_param_options_object *uri_query_param_options_object)
{
	if (uri_query_param_options_object->is_initialized) {
		if (uri_query_param_options_object->true_value != NULL) {
			zend_string_release(uri_query_param_options_object->true_value);
			uri_query_param_options_object->true_value = NULL;
		}
		if (uri_query_param_options_object->false_value != NULL) {
			zend_string_release(uri_query_param_options_object->false_value);
			uri_query_param_options_object->false_value = NULL;
		}
	}
}

ZEND_ATTRIBUTE_NONNULL zend_object *php_uri_object_create_query_param_options(zend_class_entry *ce)
{
	php_uri_query_param_options_object *uri_query_param_options_object = zend_object_alloc(sizeof(*uri_query_param_options_object), ce);

	zend_object_std_init(&uri_query_param_options_object->std, ce);
	object_properties_init(&uri_query_param_options_object->std, ce);

	uri_query_param_options_object->is_initialized = false;

	return &uri_query_param_options_object->std;
}

ZEND_ATTRIBUTE_NONNULL void php_uri_query_param_options_object_handler_free(zend_object *object)
{
	php_uri_query_param_options_object *uri_query_param_options_object = php_uri_query_param_options_object_from_obj(object);

	php_uri_query_param_options_object_free(uri_query_param_options_object);

	zend_object_std_dtor(&uri_query_param_options_object->std);
}

ZEND_ATTRIBUTE_NONNULL zend_object *php_uri_query_param_options_object_handler_clone(zend_object *object)
{
	const php_uri_query_param_options_object *uri_query_param_options_object = php_uri_query_param_options_object_from_obj(object);

	php_uri_query_param_options_object *new_uri_query_param_options_object = php_uri_query_param_options_object_from_obj(
		object->ce->create_object(object->ce)
	);

	new_uri_query_param_options_object->parsing_max_query_string_length = uri_query_param_options_object->parsing_max_query_string_length;
	new_uri_query_param_options_object->parsing_max_param_count = uri_query_param_options_object->parsing_max_param_count;
	if (uri_query_param_options_object->true_value != NULL) {
		new_uri_query_param_options_object->true_value = zend_string_copy(uri_query_param_options_object->true_value);
	}
	if (uri_query_param_options_object->false_value != NULL) {
		new_uri_query_param_options_object->false_value = zend_string_copy(uri_query_param_options_object->false_value);
	}
	new_uri_query_param_options_object->use_null_as_string = uri_query_param_options_object->use_null_as_string;
	new_uri_query_param_options_object->is_initialized = uri_query_param_options_object->is_initialized;
	zend_objects_clone_members(&new_uri_query_param_options_object->std, &uri_query_param_options_object->std);

	return &new_uri_query_param_options_object->std;
}

ZEND_ATTRIBUTE_NONNULL int php_uri_query_param_options_object_handler_has_property(zend_object *obj, zend_string *name, int check_empty, void **cache_slot)
{
	if (check_empty == ZEND_PROPERTY_NOT_EMPTY) {
		const php_uri_query_param_options_object *options = php_uri_query_param_options_object_from_obj(obj);

		if (zend_string_equals_literal(name, "parsingMaxQueryStringLength")) {
			return options->parsing_max_query_string_length != 0;
		}
	}

	return zend_string_equals_literal(name, "trueValue") || zend_string_equals_literal(name, "falseValue");
}

ZEND_ATTRIBUTE_NONNULL zval *php_uri_query_param_options_object_handler_read_property(zend_object *obj, zend_string *name, int type, void **cache_slot, zval *rv)
{
	const php_uri_query_param_options_object *options = php_uri_query_param_options_object_from_obj(obj);

	if (zend_string_equals_literal(name, "parsingMaxQueryStringLength")) {
		ZVAL_LONG(rv, options->parsing_max_query_string_length);
		return rv;
	}

	if (zend_string_equals_literal(name, "parsingMaxParamCount")) {
		ZVAL_LONG(rv, options->parsing_max_param_count);
		return rv;
	}

	if (zend_string_equals_literal(name, "trueValue")) {
		ZVAL_STR_COPY(rv, options->true_value);
		return rv;
	}

	if (zend_string_equals_literal(name, "falseValue")) {
		ZVAL_STR_COPY(rv, options->false_value);
		return rv;
	}

	if (zend_string_equals_literal(name, "useNullAsEmptyString")) {
		ZVAL_BOOL(rv, options->use_null_as_string);
		return rv;
	}

	ZEND_UNREACHABLE();
}

ZEND_ATTRIBUTE_NONNULL zval *php_uri_query_param_options_object_handler_write_property(zend_object *obj, zend_string *name, zval *value, void **cache_slot)
{
	zend_readonly_property_modification_error_ex(ZSTR_VAL(obj->ce->name), ZSTR_VAL(name));
	return &EG(error_zval);
}

ZEND_ATTRIBUTE_NONNULL void php_uri_query_param_options_object_handler_unset_property(zend_object *obj, zend_string *name, void **cache_slot)
{
	zend_throw_error(NULL, "Cannot unset readonly property %s::$%s", ZSTR_VAL(obj->ce->name), ZSTR_VAL(name));
}

ZEND_ATTRIBUTE_NONNULL zval *php_uri_query_param_options_object_handler_get_property_ptr_ptr(zend_object *object, zend_string *member, int type, void **cache_slot)
{
	/* Must always go through read property because all properties are virtual, and no dynamic properties are allowed. */
	return NULL;
}

ZEND_ATTRIBUTE_NONNULL_ARGS(1, 3) static zend_result php_uri_query_params_parse(
	const char *query_str_first, const size_t query_str_length, php_uri_query_params_object *query_params_object,
	const php_uri_query_params_spec spec, const bool silent, zval *options
) {
	const php_uri_query_param_options_object *query_param_options_object = php_uri_query_params_object_init(query_params_object, options);
	if (query_param_options_object == NULL) {
		return FAILURE;
	}

	if (UNEXPECTED(query_str_length > query_param_options_object->parsing_max_query_string_length)) {
		if (!silent) {
			zend_throw_exception(php_uri_ce_exception, "The specified query string is too long", 0);
		}
		return FAILURE;
	}

	const char *p = query_str_first;
	const char *end = query_str_first + query_str_length;

	if (spec == PHP_URI_QUERY_PARAMS_SPEC_WHATWG && *p == '?') { // TODO check other skippable characters ("/t" "/n" etc.)
		p++;
	}

	while (p < end) {
		const char *ampersand = memchr(p, '&', (size_t)(end - p));
		const char *pair_end = ampersand != NULL ? ampersand : end;
		const size_t pair_len = (size_t)(pair_end - p);

		if (pair_len == 0) {
			p = pair_end + 1;
			continue;
		}

		const char *eq = memchr(p, '=', pair_len);

		const char *name_start;
		size_t name_len;
		const char *value_start;
		size_t value_len;

		if (eq == NULL) {
			name_start = p;
			name_len = pair_len;
			value_start = NULL;
			value_len = 0;
		} else {
			name_start = p;
			name_len = (size_t)(eq - p);
			value_start = eq + 1;
			value_len = (size_t) (pair_end - value_start);
		}

		zend_string *name;
		zend_string *value;

		switch (spec) {
			case PHP_URI_QUERY_PARAMS_SPEC_RFC1866:
				name = php_uri_parser_rfc3986_query_component_percent_decode(name_start, name_len, true);
				value = php_uri_parser_rfc3986_query_component_percent_decode(value_start, value_len, true);
				break;
			case PHP_URI_QUERY_PARAMS_SPEC_RFC3986:
				name = php_uri_parser_rfc3986_query_component_percent_decode(name_start, name_len, false);
				value = php_uri_parser_rfc3986_query_component_percent_decode(value_start, value_len, false);
				break;
			case PHP_URI_QUERY_PARAMS_SPEC_WHATWG:
				name = php_uri_parser_whatwg_query_component_percent_decode(name_start, name_len);
				value = php_uri_parser_whatwg_query_component_percent_decode(value_start, value_len);
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
				zend_throw_exception(php_uri_ce_exception, "Failed to parse the specified query string", 0);
			}

			return FAILURE;
		}

		if (UNEXPECTED(zend_array_count(&query_params_object->entry_list) > query_param_options_object->parsing_max_param_count)) {
			if (!silent) {
				zend_throw_exception(php_uri_ce_exception, "The specified query string contains too many parameters", 0);
			}
			return FAILURE;
		}

		p = pair_end + 1;
	}

	return SUCCESS;
}

ZEND_ATTRIBUTE_NONNULL static void throw_cannot_recompose_uri_to_string(const zend_object *object)
{
	zend_throw_exception_ex(php_uri_ce_error, 0, "Cannot recompose %s to a string", ZSTR_VAL(object->ce->name));
}

ZEND_ATTRIBUTE_NONNULL static zend_string *php_uri_query_params_to_rfc1866_string(const php_uri_query_params_object *uri_query_params_object)
{
	return php_uri_parser_rfc3986_query_params_to_string(uri_query_params_object, true);
}

ZEND_ATTRIBUTE_NONNULL static zend_string *php_uri_query_params_to_rfc3986_string(const php_uri_query_params_object *uri_query_params_object)
{
	return php_uri_parser_rfc3986_query_params_to_string(uri_query_params_object, false);
}

ZEND_ATTRIBUTE_NONNULL static zend_string *php_uri_query_params_to_whatwg_string(const php_uri_query_params_object *uri_query_params_object)
{
	return php_uri_parser_rfc3986_query_params_to_string(uri_query_params_object, false);
}

ZEND_ATTRIBUTE_NONNULL static HashTable *php_uri_query_params_get_debug_properties(php_uri_query_params_object *object)
{
	const HashTable *std_properties = zend_std_get_properties(&object->std);
	HashTable *result = zend_array_dup(std_properties);

	zval params;
	php_uri_query_params_list_entries(object, &params);

	zend_hash_str_add(result, ZEND_STRL("params"), &params);

	return result;
}

PHP_METHOD(Uri_QueryParams, parseRfc1866)
{
	zend_string *query_str;
	zval *options;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(query_str)
		Z_PARAM_OPTIONAL
		Z_PARAM_OBJECT_OF_CLASS_OR_NULL(options, php_uri_ce_query_param_options)
	ZEND_PARSE_PARAMETERS_END();

	object_init_ex(return_value, Z_CE_P(ZEND_THIS));
	php_uri_query_params_object *uri_query_params_object = Z_URI_QUERY_PARAMS_OBJECT_P(return_value);

	if (php_uri_query_params_parse(ZSTR_VAL(query_str), ZSTR_LEN(query_str), uri_query_params_object,
		PHP_URI_QUERY_PARAMS_SPEC_RFC1866, false, options
	) == FAILURE) {
		php_uri_query_params_object_free(Z_URI_QUERY_PARAMS_OBJECT_P(return_value));
		RETURN_NULL();
	}
}

PHP_METHOD(Uri_QueryParams, parseRfc3986)
{
	zend_string *query_str;
	zval *options;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(query_str)
		Z_PARAM_OPTIONAL
		Z_PARAM_OBJECT_OF_CLASS_OR_NULL(options, php_uri_ce_query_param_options)
	ZEND_PARSE_PARAMETERS_END();

	object_init_ex(return_value, Z_CE_P(ZEND_THIS));
	php_uri_query_params_object *uri_query_params_object = Z_URI_QUERY_PARAMS_OBJECT_P(return_value);

	if (php_uri_query_params_parse(ZSTR_VAL(query_str), ZSTR_LEN(query_str), uri_query_params_object,
		PHP_URI_QUERY_PARAMS_SPEC_RFC3986, false, options
	) == FAILURE) {
		php_uri_query_params_object_free(Z_URI_QUERY_PARAMS_OBJECT_P(return_value));
		RETURN_NULL();
	}
}

PHP_METHOD(Uri_QueryParams, parseWhatWg)
{
	zend_string *query_str;
	zval *options;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(query_str)
		Z_PARAM_OPTIONAL
		Z_PARAM_OBJECT_OF_CLASS_OR_NULL(options, php_uri_ce_query_param_options)
	ZEND_PARSE_PARAMETERS_END();

	object_init_ex(return_value, Z_CE_P(ZEND_THIS));
	php_uri_query_params_object *uri_query_params_object = Z_URI_QUERY_PARAMS_OBJECT_P(return_value);

	if (php_uri_query_params_parse(ZSTR_VAL(query_str), ZSTR_LEN(query_str), uri_query_params_object,
		PHP_URI_QUERY_PARAMS_SPEC_WHATWG, false, options
	) == FAILURE) {
		php_uri_query_params_object_free(Z_URI_QUERY_PARAMS_OBJECT_P(return_value));
		RETURN_NULL();
	}
}

PHP_METHOD(Uri_QueryParams, fromArray)
{
	HashTable *query_params_arr;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(query_params_arr)
	ZEND_PARSE_PARAMETERS_END();

	zend_throw_exception(NULL, "Not implemented", 0);

	RETVAL_COPY(ZEND_THIS);
}

PHP_METHOD(Uri_QueryParams, __construct)
{
	zval *options = NULL;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_OBJECT_OF_CLASS_OR_NULL(options, php_uri_ce_query_param_options)
	ZEND_PARSE_PARAMETERS_END();

	php_uri_query_params_object *uri_query_params_object = Z_URI_QUERY_PARAMS_OBJECT_P(ZEND_THIS);

	if (php_uri_query_params_object_init(uri_query_params_object, options) == NULL) {
		RETURN_THROWS();
	}
}

PHP_METHOD(Uri_QueryParams, append)
{
	zend_string *name;
	zval *value;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_STR(name)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	php_uri_query_params_object *uri_query_params_object = Z_URI_QUERY_PARAMS_OBJECT_P(ZEND_THIS);

	zend_string *value_str = php_uri_query_params_convert_value_to_str(uri_query_params_object, value);
	if (EG(exception)) {
		RETURN_THROWS();
	}

	php_uri_query_params_append_entry(uri_query_params_object, name, value_str);

	RETVAL_COPY(ZEND_THIS);
}

PHP_METHOD(Uri_QueryParams, delete)
{
	zend_string *name;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	php_uri_query_params_object *uri_query_params_object = Z_URI_QUERY_PARAMS_OBJECT_P(ZEND_THIS);

	php_uri_query_params_delete_entries(uri_query_params_object, name);

	RETVAL_COPY(ZEND_THIS);
}

PHP_METHOD(Uri_QueryParams, deleteValue)
{
	zend_string *name, *value;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_STR(name)
		Z_PARAM_STR_OR_NULL(value)
	ZEND_PARSE_PARAMETERS_END();

	php_uri_query_params_object *uri_query_params_object = Z_URI_QUERY_PARAMS_OBJECT_P(ZEND_THIS);

	php_uri_query_params_delete_entries_with_value(uri_query_params_object, name, value);

	RETVAL_COPY(ZEND_THIS);
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

	zend_string *value_str = php_uri_query_params_convert_value_to_str(uri_query_params_object, value);
	if (EG(exception)) {
		RETURN_THROWS();
	}

	RETVAL_BOOL(php_uri_query_params_has_entry_with_value(uri_query_params_object, name, value_str));
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

PHP_METHOD(Uri_QueryParams, list)
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

PHP_METHOD(Uri_QueryParams, set)
{
	zend_string *name;
	zval *value;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_STR(name)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	php_uri_query_params_object *uri_query_params_object = Z_URI_QUERY_PARAMS_OBJECT_P(ZEND_THIS);

	zend_string *value_str = php_uri_query_params_convert_value_to_str(uri_query_params_object, value);
	if (EG(exception)) {
		RETURN_THROWS();
	}

	php_uri_query_params_set_entry(uri_query_params_object, name, value_str);

	RETVAL_COPY(ZEND_THIS);
}

PHP_METHOD(Uri_QueryParams, sort)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_throw_exception(NULL, "Not implemented", 0);

	RETVAL_COPY(ZEND_THIS);
}

static void php_uri_query_params_navigate_and_insert(zval *current, const char *suffix, size_t suffix_len, zend_string *value)
{
	ZEND_ASSERT(Z_TYPE_P(current) == IS_ARRAY);

	if (suffix_len == 0) {
		return;
	}

	ZEND_ASSERT(suffix[0] == '[');

	const char *close = memchr(suffix + 1, ']', suffix_len - 1);
	if (UNEXPECTED(close == NULL)) {
		return; /* TODO malformed key */
	}

	const char *seg = suffix + 1;
	size_t seg_len = (size_t) (close - seg);
	const char *rest = close + 1;
	size_t rest_len = suffix_len - (size_t) (rest - suffix);

	if (seg_len == 0) {
		if (rest_len == 0) {
			zval val;
			ZVAL_STR_COPY(&val, value);
			zend_hash_next_index_insert(Z_ARRVAL_P(current), &val);
		} else {
			zval new_arr;
			array_init(&new_arr);
			zval *inserted = zend_hash_next_index_insert(Z_ARRVAL_P(current), &new_arr);
			if (EXPECTED(inserted != NULL)) {
				php_uri_query_params_navigate_and_insert(inserted, rest, rest_len, value);
			}
		}
		return;
	}

	zend_long lval;
	zval *nested;

	if (is_numeric_string(seg, seg_len, &lval, NULL, 0) == IS_LONG) {
		/* Numeric key: foo[0] → integer index */
		nested = zend_hash_index_find(Z_ARRVAL_P(current), (zend_ulong) lval);
		if (rest_len == 0) {
			zval val;
			ZVAL_STR_COPY(&val, value);
			zend_hash_index_update(Z_ARRVAL_P(current), (zend_ulong) lval, &val);
		} else {
			if (nested == NULL || Z_TYPE_P(nested) != IS_ARRAY) {
				zval new_arr;
				array_init(&new_arr);
				nested = zend_hash_index_update(Z_ARRVAL_P(current), (zend_ulong) lval, &new_arr);
			}
			if (EXPECTED(nested != NULL)) {
				php_uri_query_params_navigate_and_insert(nested, rest, rest_len, value);
			}
		}
	} else {
		/* String key: foo[a] */
		zend_string *seg_str = zend_string_init(seg, seg_len, 0);
		if (rest_len == 0) {
			zval val;
			ZVAL_STR_COPY(&val, value);
			zend_hash_update(Z_ARRVAL_P(current), seg_str, &val);
		} else {
			nested = zend_hash_find(Z_ARRVAL_P(current), seg_str);
			if (nested == NULL || Z_TYPE_P(nested) != IS_ARRAY) {
				zval new_arr;
				array_init(&new_arr);
				nested = zend_hash_update(Z_ARRVAL_P(current), seg_str, &new_arr);
			}
			if (EXPECTED(nested != NULL)) {
				php_uri_query_params_navigate_and_insert(nested, rest, rest_len, value);
			}
		}
		zend_string_release(seg_str);
	}
}

static void php_uri_query_params_insert_entry(
	zval *result, const php_uri_query_param_list_entry *list_entry, bool is_list
) {
	const char *key = ZSTR_VAL(list_entry->key);
	size_t key_len = ZSTR_LEN(list_entry->key);

	const char *bracket = memchr(key, '[', key_len);
	size_t root_len = bracket != NULL ? (size_t) (bracket - key) : key_len;
	const char *suffix = (bracket != NULL) ? bracket : NULL;
	size_t suffix_len = (bracket != NULL) ? (key_len - root_len) : 0;

	if (suffix == NULL) {
		/* Bare key: scalar or list */
		if (is_list) {
			zval *existing = zend_hash_str_find(Z_ARRVAL_P(result), key, key_len);
			if (existing == NULL) {
				zval list;
				array_init(&list);
				zval val;
				ZVAL_STR_COPY(&val, list_entry->value);
				zend_hash_next_index_insert(Z_ARRVAL(list), &val);
				zend_hash_str_add(Z_ARRVAL_P(result), key, key_len, &list);
			} else {
				ZEND_ASSERT(Z_TYPE_P(existing) == IS_ARRAY);
				zval val;
				ZVAL_STR_COPY(&val, list_entry->value);
				zend_hash_next_index_insert(Z_ARRVAL_P(existing), &val);
			}
		} else {
			zval val;
			ZVAL_STR_COPY(&val, list_entry->value);
			zend_hash_str_update(Z_ARRVAL_P(result), key, key_len, &val);
		}
		return;
	}

	/* key with brackets: navigate */
	zval *container = zend_hash_str_find(Z_ARRVAL_P(result), key, root_len);
	if (container == NULL || Z_TYPE_P(container) != IS_ARRAY) {
		zval new_arr;
		array_init(&new_arr);
		container = zend_hash_str_update(Z_ARRVAL_P(result), key, root_len, &new_arr);
	}

	if (EXPECTED(container != NULL)) {
		php_uri_query_params_navigate_and_insert(container, suffix, suffix_len, list_entry->value);
	}
}

PHP_METHOD(Uri_QueryParams, toArray)
{
	ZEND_PARSE_PARAMETERS_NONE();

	const php_uri_query_params_object *obj = php_uri_query_params_object_from_obj(Z_OBJ_P(ZEND_THIS));

	array_init(return_value);

	php_uri_query_param_list_entry *list_entry;

	ZEND_HASH_FOREACH_PTR(&obj->entry_list, list_entry) {
		if (list_entry->key == NULL) {
			continue; /* tombstone */
		}

		bool is_list = false;
		if (memchr(ZSTR_VAL(list_entry->key), '[', ZSTR_LEN(list_entry->key)) == NULL) {
			const php_uri_query_param_lookup_entry *lookup_entry = zend_hash_find_ptr(&obj->entry_lookup_table, list_entry->key);
			is_list = lookup_entry != NULL && lookup_entry->first_index != lookup_entry->last_index;
		}

		php_uri_query_params_insert_entry(return_value, list_entry, is_list);

	} ZEND_HASH_FOREACH_END();
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

PHP_METHOD(Uri_QueryParams, toWhatWgString)
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

	/* Serialize state: "query" key in the first array */
	zend_string *uri_query_param_str = php_uri_query_params_to_rfc3986_string(uri_query_params_object);
	if (uri_query_param_str == NULL) {
		throw_cannot_recompose_uri_to_string(Z_OBJ_P(ZEND_THIS));
		RETURN_THROWS();
	}
	zval tmp;
	ZVAL_STR(&tmp, uri_query_param_str);

	array_init(return_value);

	zval arr;
	array_init(&arr);
	zend_hash_str_add_new(Z_ARRVAL(arr), PHP_URI_SERIALIZE_URI_QUERY_PARAM_FIELD_NAME, sizeof(PHP_URI_SERIALIZE_URI_QUERY_PARAM_FIELD_NAME) - 1, &tmp);
	zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &arr);

	/* Serialize regular properties: second array */
	ZVAL_ARR(&arr, uri_query_params_object->std.handlers->get_properties(&uri_query_params_object->std));
	Z_TRY_ADDREF(arr);
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
		zend_throw_error(NULL, "Cannot modify readonly object of class %s", ZSTR_VAL(uri_query_params_object->std.ce->name));
		zend_throw_exception_ex(NULL, 0, "Invalid serialization data for %s object", ZSTR_VAL(uri_query_params_object->std.ce->name));
		RETURN_THROWS();
	}

	/* Verify the expected number of elements, this implicitly ensures that no additional elements are present. */
	if (zend_hash_num_elements(data) != 2) {
		zend_throw_exception_ex(NULL, 0, "Invalid serialization data for %s object", ZSTR_VAL(uri_query_params_object->std.ce->name));
		RETURN_THROWS();
	}

	/* Unserialize state: "query" key in the first array */
	const zval *arr = zend_hash_index_find(data, 0);
	if (arr == NULL || Z_TYPE_P(arr) != IS_ARRAY) {
		zend_throw_exception_ex(NULL, 0, "Invalid serialization data for %s object", ZSTR_VAL(uri_query_params_object->std.ce->name));
		RETURN_THROWS();
	}

	/* Verify the expected number of elements inside the first array, this implicitly ensures that no additional elements are present. */
	if (zend_hash_num_elements(Z_ARRVAL_P(arr)) != 1) {
		zend_throw_exception_ex(NULL, 0, "Invalid serialization data for %s object", ZSTR_VAL(uri_query_params_object->std.ce->name));
		RETURN_THROWS();
	}

	const zval *uri_query_param_zv = zend_hash_str_find_ind(Z_ARRVAL_P(arr), ZEND_STRL(PHP_URI_SERIALIZE_URI_QUERY_PARAM_FIELD_NAME));
	if (uri_query_param_zv == NULL || Z_TYPE_P(uri_query_param_zv) != IS_STRING) {
		zend_throw_exception_ex(NULL, 0, "Invalid serialization data for %s object", ZSTR_VAL(uri_query_params_object->std.ce->name));
		RETURN_THROWS();
	}

	const zend_result result = SUCCESS; // TODO

	if (result == FAILURE) {
		RETURN_THROWS();
	}

	uri_query_params_object->is_initialized = true;

	/* Unserialize regular properties: second array */
	arr = zend_hash_index_find(data, 1);
	if (arr == NULL || Z_TYPE_P(arr) != IS_ARRAY) {
		zend_throw_exception_ex(NULL, 0, "Invalid serialization data for %s object", ZSTR_VAL(uri_query_params_object->std.ce->name));
		RETURN_THROWS();
	}

	/* Verify that there is no regular property in the second array, because the query param classes have no properties and they are final. */
	if (zend_hash_num_elements(Z_ARRVAL_P(arr)) > 0) {
		zend_throw_exception_ex(NULL, 0, "Invalid serialization data for %s object", ZSTR_VAL(uri_query_params_object->std.ce->name));
		RETURN_THROWS();
	}
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

PHP_METHOD(Uri_QueryParamOptions, __construct)
{
	zend_long parsing_max_query_string_length = PARSING_MAX_QUERY_STRING_LENGTH_DEFAULT;
	zend_long parsing_max_param_count = PARSING_MAX_QUERY_PARAM_COUNT_DEFAULT;
	zend_string *true_value = NULL;
	zend_string *false_value = NULL;
	bool use_null_as_empty_string = USE_NULL_AS_EMPTY_STRING_DEFAULT;

	ZEND_PARSE_PARAMETERS_START(0, 5)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(parsing_max_query_string_length)
		Z_PARAM_LONG(parsing_max_param_count)
		Z_PARAM_STR(true_value)
		Z_PARAM_STR(false_value)
		Z_PARAM_BOOL(use_null_as_empty_string)
	ZEND_PARSE_PARAMETERS_END();

	if (parsing_max_query_string_length < 0) {
		zend_argument_value_error(1, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	if (parsing_max_param_count < 0) {
		zend_argument_value_error(2, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	if (php_uri_query_param_options_update_properties(
		Z_OBJ_P(ZEND_THIS), parsing_max_query_string_length, parsing_max_param_count, true_value, false_value,
		use_null_as_empty_string
	) == FAILURE) {
		RETURN_THROWS();
	}
}

PHP_METHOD(Uri_QueryParamOptions, __debugInfo)
{
	ZEND_PARSE_PARAMETERS_NONE();

	const php_uri_query_param_options_object *object = Z_URI_QUERY_PARAM_OPTIONS_OBJECT_P(ZEND_THIS);

	array_init(return_value);

	add_assoc_long(return_value, "parsingMaxQueryStringLength", object->parsing_max_query_string_length);
	add_assoc_long(return_value, "parsingMaxParamCount", object->parsing_max_param_count);
	add_assoc_str(return_value, "trueValue", zend_string_copy(object->true_value));
	add_assoc_str(return_value, "falseValue", zend_string_copy(object->false_value));
	add_assoc_bool(return_value, "useNullAsLong", object->use_null_as_string);
}
