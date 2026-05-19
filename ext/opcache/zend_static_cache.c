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

#include "php.h"

#include "Zend/zend_attributes.h"
#include "Zend/zend_atomic.h"
#include "Zend/zend_closures.h"
#include "Zend/zend_exceptions.h"

#include "ZendAccelerator.h"
#include "zend_accelerator_module.h"
#include "zend_shared_alloc.h"
#include "zend_static_cache.h"
#include "zend_smart_str.h"

#include "ext/date/php_date.h"
#include "ext/spl/spl_array.h"
#include "ext/spl/spl_fixedarray.h"
#include "ext/standard/php_var.h"

#include "SAPI.h"

#include "zend_static_cache_internal.h"
#include "zend_opcache_serializer.h"

#include "opcache_arginfo.h"

#define ZEND_OPCACHE_STATIC_CACHE_API_VALUE_TYPE "object|array|string|int|float|bool|null"

zend_class_entry *zend_opcache_static_cache_exception_ce;
zend_class_entry *zend_opcache_static_cache_strategy_ce;
zend_class_entry *zend_opcache_static_cache_info_ce;

static zend_class_entry *zend_opcache_static_cache_pinned_attribute_ce;
static zend_class_entry *zend_opcache_static_cache_volatile_static_attribute_ce;
static zend_class_entry *zend_opcache_static_cache_safe_direct_attribute_ce;

ZEND_METHOD(OPcache_StaticCacheInfo, __construct)
{
}

zend_opcache_static_cache_context zend_opcache_static_cache_volatile_context_state = {
	{0}, {0}, "volatile cache", "opcache_volatile_cache_lock",
#ifndef ZEND_WIN32
	ZEND_OPCACHE_STATIC_CACHE_VOLATILE_SEM_FILENAME_PREFIX,
#endif
	true, false
};

zend_opcache_static_cache_context zend_opcache_static_cache_pinned_context_state = {
	{0}, {0}, "pinned cache", "opcache_pinned_cache_lock",
#ifndef ZEND_WIN32
	ZEND_OPCACHE_STATIC_CACHE_PINNED_SEM_FILENAME_PREFIX,
#endif
	false, true
};

bool zend_opcache_static_cache_safe_direct_classes_marked = false;
bool zend_opcache_static_cache_subsystem_disabled = false;
const char *zend_opcache_static_cache_subsystem_failure_reason = NULL;

static HashTable zend_opcache_static_cache_safe_direct_handler_table;
static bool zend_opcache_static_cache_safe_direct_handlers_initialized = false;

#ifdef ZTS
ZEND_EXT_TLS bool zend_opcache_static_cache_zts_lock_is_write = false;
#endif
ZEND_EXT_TLS HashTable zend_opcache_static_cache_attribute_classes;
ZEND_EXT_TLS bool zend_opcache_static_cache_attribute_classes_initialized = false;
ZEND_EXT_TLS HashTable zend_opcache_static_cache_ignored_classes;
ZEND_EXT_TLS bool zend_opcache_static_cache_ignored_classes_initialized = false;
ZEND_EXT_TLS HashTable zend_opcache_static_cache_function_statics;
ZEND_EXT_TLS bool zend_opcache_static_cache_function_statics_initialized = false;
ZEND_EXT_TLS HashTable zend_opcache_static_cache_class_blob_handles;
ZEND_EXT_TLS bool zend_opcache_static_cache_class_blob_handles_initialized = false;
ZEND_EXT_TLS zend_opcache_static_cache_runtime zend_opcache_static_cache_volatile_runtime_state = {0};
ZEND_EXT_TLS zend_opcache_static_cache_runtime zend_opcache_static_cache_pinned_runtime_state = {0};
ZEND_EXT_TLS zend_opcache_static_cache_context *zend_opcache_static_cache_active_context_ptr = NULL;
ZEND_EXT_TLS HashTable *zend_opcache_static_cache_tracked_roots = NULL;
ZEND_EXT_TLS HashTable *zend_opcache_static_cache_tracked_references = NULL;
ZEND_EXT_TLS HashTable *zend_opcache_static_cache_tracked_arrays = NULL;
ZEND_EXT_TLS HashTable *zend_opcache_static_cache_tracked_objects = NULL;
ZEND_EXT_TLS bool zend_opcache_static_cache_has_tracked_arrays = false;
ZEND_EXT_TLS bool zend_opcache_static_cache_has_tracked_objects = false;
ZEND_EXT_TLS bool zend_opcache_static_cache_volatile_skip_publish = false;
ZEND_EXT_TLS bool zend_opcache_static_cache_pinned_skip_publish = false;
ZEND_EXT_TLS zend_opcache_static_cache_shared_graph_ref *zend_opcache_static_cache_shared_graph_refs = NULL;
ZEND_EXT_TLS uint32_t zend_opcache_static_cache_shared_graph_ref_count = 0;
ZEND_EXT_TLS uint32_t zend_opcache_static_cache_shared_graph_ref_capacity = 0;
ZEND_EXT_TLS zend_opcache_static_cache_shared_graph_ref *zend_opcache_static_cache_retired_shared_graphs = NULL;
ZEND_EXT_TLS uint32_t zend_opcache_static_cache_retired_shared_graph_count = 0;
ZEND_EXT_TLS uint32_t zend_opcache_static_cache_retired_shared_graph_capacity = 0;
/* Single-slot pointer caches that short-circuit the hash lookup performed by
 * the mutation hooks. The caches are invalidated whenever the tracked-pointer
 * map mutates. */
ZEND_EXT_TLS HashTable *zend_opcache_static_cache_last_array_ht = NULL;
ZEND_EXT_TLS zend_opcache_static_cache_tracked_dependency *zend_opcache_static_cache_last_array_dependency = NULL;
ZEND_EXT_TLS zend_object *zend_opcache_static_cache_last_object_obj = NULL;
ZEND_EXT_TLS zend_opcache_static_cache_tracked_dependency *zend_opcache_static_cache_last_object_dependency = NULL;
ZEND_EXT_TLS zend_opcache_static_cache_pending_mutation zend_opcache_static_cache_pending_mutation_state;
ZEND_EXT_TLS bool zend_opcache_static_cache_capture_active = false;
ZEND_EXT_TLS bool zend_opcache_static_cache_capture_available = false;
ZEND_EXT_TLS zend_opcache_static_cache_static_slot_handle *zend_opcache_static_cache_capture_handle = NULL;
ZEND_EXT_TLS HashTable *zend_opcache_static_cache_capture_arrays = NULL;
ZEND_EXT_TLS HashTable *zend_opcache_static_cache_capture_objects = NULL;
ZEND_EXT_TLS zend_opcache_static_cache_lookup_entry zend_opcache_static_cache_volatile_lookup_entry_storage[ZEND_OPCACHE_STATIC_CACHE_LOOKUP_BUCKETS];
ZEND_EXT_TLS zend_opcache_static_cache_lookup_entry zend_opcache_static_cache_pinned_lookup_entry_storage[ZEND_OPCACHE_STATIC_CACHE_LOOKUP_BUCKETS];
ZEND_EXT_TLS HashTable *zend_opcache_static_cache_volatile_request_local_slots = NULL;
ZEND_EXT_TLS HashTable *zend_opcache_static_cache_pinned_request_local_slots = NULL;
ZEND_EXT_TLS HashTable *zend_opcache_static_cache_volatile_entry_locks = NULL;
ZEND_EXT_TLS HashTable *zend_opcache_static_cache_pinned_entry_locks = NULL;
ZEND_EXT_TLS uint32_t zend_opcache_static_cache_volatile_entry_lock_counts[ZEND_OPCACHE_STATIC_CACHE_ENTRY_LOCK_STRIPES];
ZEND_EXT_TLS uint32_t zend_opcache_static_cache_pinned_entry_lock_counts[ZEND_OPCACHE_STATIC_CACHE_ENTRY_LOCK_STRIPES];

static zend_always_inline bool zend_opcache_static_cache_key_has_reserved_class_prefix(zend_string *key)
{
	return zend_string_starts_with_literal(key, "pinned_static_class:") ||
		zend_string_starts_with_literal(key, "volatile_static_class:")
	;
}

static zend_always_inline zend_class_entry *zend_opcache_static_cache_lookup_loaded_class_key(zend_string *key)
{
	return zend_lookup_class_ex(key, NULL, ZEND_FETCH_CLASS_NO_AUTOLOAD);
}

static zend_always_inline bool zend_opcache_static_cache_validate_key_ex(
		zend_string *key,
		uint32_t arg_num,
		bool allow_class_selector)
{
	if (ZSTR_LEN(key) == 0) {
		zend_argument_value_error(arg_num, "must be a non-empty string");

		return false;
	}

	if (zend_opcache_static_cache_key_has_reserved_class_prefix(key)) {
		zend_argument_value_error(arg_num, "must not start with a reserved static-cache class key prefix");

		return false;
	}

	if (!allow_class_selector && zend_opcache_static_cache_lookup_loaded_class_key(key) != NULL) {
		zend_argument_value_error(arg_num, "must not be a loaded class name");

		return false;
	}

	return true;
}

static zend_always_inline bool zend_opcache_static_cache_validate_key(zend_string *key, uint32_t arg_num)
{
	return zend_opcache_static_cache_validate_key_ex(key, arg_num, false);
}

static zend_always_inline bool zend_opcache_static_cache_validate_key_or_class(zend_string *key_or_class, uint32_t arg_num)
{
	return zend_opcache_static_cache_validate_key_ex(key_or_class, arg_num, true);
}

static zend_always_inline bool zend_opcache_static_cache_validate_store_array_keys(HashTable *values, uint32_t arg_num)
{
	zend_string *key;

	ZEND_HASH_FOREACH_STR_KEY(values, key) {
		if (key == NULL || ZSTR_LEN(key) == 0 ||
			zend_opcache_static_cache_key_has_reserved_class_prefix(key) ||
			zend_opcache_static_cache_lookup_loaded_class_key(key) != NULL
		) {
			zend_argument_value_error(arg_num, "must be an array with non-empty string keys that are not reserved static-cache class keys or loaded class names");

			return false;
		}
	} ZEND_HASH_FOREACH_END();

	return true;
}

static zend_always_inline void zend_opcache_static_cache_release_key_list(zend_string **keys, uint32_t key_count)
{
	uint32_t index;

	if (keys == NULL) {
		return;
	}

	for (index = 0; index < key_count; index++) {
		zend_string_release(keys[index]);
	}

	efree(keys);
}

static zend_always_inline bool zend_opcache_static_cache_prepare_key_list(
		HashTable *keys,
		zend_string ***prepared_keys,
		uint32_t *prepared_key_count,
		uint32_t arg_num)
{
	zend_string **prepared;
	zval *value;
	uint32_t count, index = 0;

	ZEND_ASSERT(prepared_keys != NULL);
	ZEND_ASSERT(prepared_key_count != NULL);

	count = zend_hash_num_elements(keys);
	*prepared_keys = NULL;
	*prepared_key_count = 0;

	if (count == 0) {
		return true;
	}

	prepared = safe_emalloc(count, sizeof(zend_string *), 0);
	ZEND_HASH_FOREACH_VAL(keys, value) {
		ZVAL_DEREF(value);
		if (Z_TYPE_P(value) == IS_STRING) {
			if (Z_STRLEN_P(value) == 0 ||
				zend_opcache_static_cache_key_has_reserved_class_prefix(Z_STR_P(value)) ||
				zend_opcache_static_cache_lookup_loaded_class_key(Z_STR_P(value)) != NULL
			) {
				zend_argument_value_error(arg_num, "must contain only non-empty string or int cache keys that are not reserved static-cache class keys or loaded class names");
				zend_opcache_static_cache_release_key_list(prepared, index);

				return false;
			}

			prepared[index++] = zend_string_copy(Z_STR_P(value));
		} else if (Z_TYPE_P(value) == IS_LONG) {
			prepared[index++] = zend_long_to_str(Z_LVAL_P(value));
		} else {
			zend_argument_value_error(arg_num, "must contain only non-empty string or int cache keys that are not reserved static-cache class keys or loaded class names");
			zend_opcache_static_cache_release_key_list(prepared, index);

			return false;
		}
	} ZEND_HASH_FOREACH_END();

	*prepared_keys = prepared;
	*prepared_key_count = index;

	return true;
}

static zend_always_inline bool zend_opcache_static_cache_validate_api_value(zval *value, uint32_t arg_num)
{
	ZVAL_DEREF(value);
	if (Z_TYPE_P(value) == IS_RESOURCE) {
		zend_argument_type_error(arg_num, "must be of type " ZEND_OPCACHE_STATIC_CACHE_API_VALUE_TYPE ", resource given");

		return false;
	}

	if (Z_TYPE_P(value) == IS_OBJECT && Z_OBJCE_P(value) == zend_ce_closure) {
		zend_argument_type_error(arg_num, "must not be a Closure object");

		return false;
	}

	return true;
}

static void zend_opcache_static_cache_register_accelerator_handlers(void);

static void zend_opcache_static_cache_safe_direct_handlers_dtor(zval *zv)
{
	pefree(Z_PTR_P(zv), true);
}

void zend_opcache_static_cache_safe_direct_handlers_init(void)
{
	if (zend_opcache_static_cache_safe_direct_handlers_initialized) {
		return;
	}

	zend_hash_init(
		&zend_opcache_static_cache_safe_direct_handler_table,
		8,
		NULL,
		zend_opcache_static_cache_safe_direct_handlers_dtor,
		true
	);
	zend_opcache_static_cache_safe_direct_handlers_initialized = true;
}

void zend_opcache_static_cache_safe_direct_handlers_destroy(void)
{
	if (!zend_opcache_static_cache_safe_direct_handlers_initialized) {
		return;
	}

	zend_hash_destroy(&zend_opcache_static_cache_safe_direct_handler_table);
	zend_opcache_static_cache_safe_direct_handlers_initialized = false;
}

void zend_opcache_static_cache_safe_direct_register_class(
		zend_class_entry *ce,
		const zend_opcache_static_cache_safe_direct_handlers *handlers)
{
	zend_opcache_static_cache_safe_direct_handlers handlers_copy;

	if (ce == NULL ||
		zend_opcache_static_cache_safe_direct_attribute_ce == NULL ||
		handlers == NULL ||
		handlers->copy == NULL ||
		handlers->state_serialize == NULL ||
		handlers->state_unserialize == NULL
	) {
		return;
	}

	if (!zend_opcache_serializer_safe_direct_cache_has_attribute(ce->attributes)) {
		zend_add_class_attribute(ce, zend_opcache_static_cache_safe_direct_attribute_ce->name, 0);
	}

	zend_opcache_static_cache_safe_direct_handlers_init();
	handlers_copy = *handlers;
	zend_hash_index_update_mem(
		&zend_opcache_static_cache_safe_direct_handler_table,
		(zend_ulong) (uintptr_t) ce,
		&handlers_copy,
		sizeof(handlers_copy)
	);
}

static const zend_opcache_static_cache_safe_direct_handlers *zend_opcache_static_cache_safe_direct_find_handlers(
		zend_class_entry *ce,
		zend_class_entry **base_ce_ptr)
{
	const zend_opcache_static_cache_safe_direct_handlers *handlers;

	if (!zend_opcache_static_cache_safe_direct_handlers_initialized) {
		return NULL;
	}

	while (ce != NULL) {
		handlers = zend_hash_index_find_ptr(
			&zend_opcache_static_cache_safe_direct_handler_table,
			(zend_ulong) (uintptr_t) ce
		);
		if (handlers != NULL) {
			if (base_ce_ptr != NULL) {
				*base_ce_ptr = ce;
			}

			return handlers;
		}

		ce = ce->parent;
	}

	return NULL;
}

static bool zend_opcache_static_cache_safe_direct_serializer_path_serialize(
		const zval *object,
		zval *state)
{
	const zend_opcache_static_cache_safe_direct_handlers *handlers =
		zend_opcache_static_cache_safe_direct_find_handlers(Z_OBJCE_P(object), NULL)
	;

	ZVAL_UNDEF(state);
	if (handlers == NULL || handlers->serializer_path.serialize == NULL) {
		return false;
	}

	zend_call_known_instance_method_with_0_params(handlers->serializer_path.serialize, Z_OBJ_P(object), state);
	if (EG(exception) || Z_TYPE_P(state) != IS_ARRAY) {
		if (Z_TYPE_P(state) != IS_UNDEF) {
			zval_ptr_dtor(state);
			ZVAL_UNDEF(state);
		}

		return false;
	}

	return true;
}

static bool zend_opcache_static_cache_safe_direct_serializer_path_unserialize(
		zval *object,
		zval *state)
{
	const zend_opcache_static_cache_safe_direct_handlers *handlers =
		zend_opcache_static_cache_safe_direct_find_handlers(Z_OBJCE_P(object), NULL)
	;

	if (handlers == NULL || handlers->serializer_path.unserialize == NULL || Z_TYPE_P(state) != IS_ARRAY) {
		return false;
	}

	zend_call_known_instance_method_with_1_params(handlers->serializer_path.unserialize, Z_OBJ_P(object), NULL, state);

	return !EG(exception);
}

static bool zend_opcache_static_cache_safe_direct_serializer_path_copy(
		void *context,
		zend_object *old_object,
		zend_object *new_object,
		zend_opcache_static_cache_safe_direct_clone_value_func_t clone_value)
{
	zval old_zv, new_zv, state_zv, cloned_state_zv;
	bool result = false;

	if (clone_value == NULL) {
		return false;
	}

	ZVAL_OBJ(&old_zv, old_object);
	ZVAL_OBJ(&new_zv, new_object);
	ZVAL_UNDEF(&state_zv);
	ZVAL_UNDEF(&cloned_state_zv);

	if (!zend_opcache_static_cache_safe_direct_serializer_path_serialize(&old_zv, &state_zv)) {
		goto cleanup;
	}

	if (!clone_value(context, &cloned_state_zv, &state_zv) ||
			Z_TYPE(cloned_state_zv) != IS_ARRAY) {
		goto cleanup;
	}

	result = zend_opcache_static_cache_safe_direct_serializer_path_unserialize(
		&new_zv,
		&cloned_state_zv
	);

cleanup:
	if (Z_TYPE(cloned_state_zv) != IS_UNDEF) {
		zval_ptr_dtor(&cloned_state_zv);
	}
	if (Z_TYPE(state_zv) != IS_UNDEF) {
		zval_ptr_dtor(&state_zv);
	}

	return result && !EG(exception);
}

static bool zend_opcache_static_cache_safe_direct_serializer_path_has_unstorable(
		void *context,
		const zval *value,
		zend_opcache_static_cache_safe_direct_value_has_unstorable_func_t value_has_unstorable)
{
	zval state_zv;
	bool result;

	if (value_has_unstorable == NULL) {
		return false;
	}

	ZVAL_UNDEF(&state_zv);
	if (!zend_opcache_static_cache_safe_direct_serializer_path_serialize(value, &state_zv)) {
		return true;
	}

	result = value_has_unstorable(context, &state_zv);
	zval_ptr_dtor(&state_zv);

	return result;
}

zend_opcache_static_cache_safe_direct_state_copy_func_t zend_opcache_static_cache_safe_direct_copy_func(
		zend_class_entry *ce,
		zend_class_entry **base_ce_ptr)
{
	const zend_opcache_static_cache_safe_direct_handlers *handlers =
		zend_opcache_static_cache_safe_direct_find_handlers(ce, base_ce_ptr);

	return handlers != NULL ? handlers->copy : NULL;
}

zend_opcache_static_cache_safe_direct_state_has_unstorable_func_t zend_opcache_static_cache_safe_direct_state_has_unstorable_func(
		zend_class_entry *ce)
{
	const zend_opcache_static_cache_safe_direct_handlers *handlers =
		zend_opcache_static_cache_safe_direct_find_handlers(ce, NULL);

	return handlers != NULL ? handlers->state_has_unstorable : NULL;
}

zend_opcache_static_cache_safe_direct_state_serialize_func_t zend_opcache_static_cache_safe_direct_state_serialize_func(
		zend_class_entry *ce)
{
	const zend_opcache_static_cache_safe_direct_handlers *handlers =
		zend_opcache_static_cache_safe_direct_find_handlers(ce, NULL);

	return handlers != NULL ? handlers->state_serialize : NULL;
}

zend_opcache_static_cache_safe_direct_state_unserialize_func_t zend_opcache_static_cache_safe_direct_state_unserialize_func(
		zend_class_entry *ce)
{
	const zend_opcache_static_cache_safe_direct_handlers *handlers =
		zend_opcache_static_cache_safe_direct_find_handlers(ce, NULL);

	return handlers != NULL ? handlers->state_unserialize : NULL;
}

bool zend_opcache_static_cache_safe_direct_allows_custom_serializers(zend_class_entry *ce)
{
	const zend_opcache_static_cache_safe_direct_handlers *handlers =
		zend_opcache_static_cache_safe_direct_find_handlers(ce, NULL);

	return handlers != NULL && handlers->allows_custom_serializers;
}

bool zend_opcache_static_cache_safe_direct_state_includes_properties(zend_class_entry *ce)
{
	const zend_opcache_static_cache_safe_direct_handlers *handlers =
		zend_opcache_static_cache_safe_direct_find_handlers(ce, NULL);

	return handlers != NULL && handlers->serializer_path.state_includes_properties;
}

static zend_always_inline zend_string *zend_opcache_static_cache_validate_volatile_static_attribute(
		zend_attribute *attr,
		uint32_t target ZEND_ATTRIBUTE_UNUSED,
		zend_class_entry *scope)
{
	zend_opcache_static_cache_volatile_static_attribute_config config;
	zend_string *error = NULL;

	if (!zend_opcache_static_cache_volatile_static_attribute_config_from_attribute(attr, scope, &config, &error)) {
		return error;
	}

	return NULL;
}

static zend_always_inline zend_string *zend_opcache_static_cache_validate_direct_cache_safe_attribute(
		zend_attribute *attr ZEND_ATTRIBUTE_UNUSED,
		uint32_t target,
		zend_class_entry *scope)
{
	if (target != ZEND_ATTRIBUTE_TARGET_CLASS) {
		return ZSTR_INIT_LITERAL("Only classes can be marked with #[OPcache\\__DirectCacheSafe]", 0);
	}

	if (scope == NULL || scope->type != ZEND_INTERNAL_CLASS) {
		return ZSTR_INIT_LITERAL("Only internal classes can be marked with #[OPcache\\__DirectCacheSafe]", 0);
	}

	return NULL;
}

static zend_always_inline void zend_opcache_static_cache_register_classes(void)
{
	zend_internal_attribute *attribute;

	if (zend_opcache_static_cache_pinned_attribute_ce != NULL) {
		return;
	}

	zend_opcache_static_cache_info_ce = register_class_OPcache_StaticCacheInfo();
	zend_opcache_static_cache_pinned_attribute_ce = register_class_OPcache_PinnedStatic();
	zend_mark_internal_attribute(zend_opcache_static_cache_pinned_attribute_ce);
	zend_opcache_static_cache_strategy_ce = register_class_OPcache_CacheStrategy();
	zend_opcache_static_cache_volatile_static_attribute_ce = register_class_OPcache_VolatileStatic();
	attribute = zend_mark_internal_attribute(zend_opcache_static_cache_volatile_static_attribute_ce);
	attribute->validator = zend_opcache_static_cache_validate_volatile_static_attribute;
	zend_opcache_static_cache_safe_direct_attribute_ce = register_class_OPcache___DirectCacheSafe();
	attribute = zend_mark_internal_attribute(zend_opcache_static_cache_safe_direct_attribute_ce);
	attribute->validator = zend_opcache_static_cache_validate_direct_cache_safe_attribute;
	zend_opcache_static_cache_exception_ce = register_class_OPcache_StaticCacheException(zend_ce_exception);
}

static zend_always_inline void zend_opcache_static_cache_safe_direct_register_serializer_class(
		zend_class_entry *ce,
		bool allows_custom_serializers)
{
	zend_opcache_static_cache_safe_direct_handlers handlers;

	if (ce == NULL || ce->__serialize == NULL || ce->__unserialize == NULL) {
		return;
	}

	handlers.allows_custom_serializers = allows_custom_serializers;
	handlers.serializer_path.state_includes_properties = true;
	handlers.serializer_path.serialize = ce->__serialize;
	handlers.serializer_path.unserialize = ce->__unserialize;
	handlers.copy = zend_opcache_static_cache_safe_direct_serializer_path_copy;
	handlers.state_has_unstorable = zend_opcache_static_cache_safe_direct_serializer_path_has_unstorable;
	handlers.state_serialize = zend_opcache_static_cache_safe_direct_serializer_path_serialize;
	handlers.state_unserialize = zend_opcache_static_cache_safe_direct_serializer_path_unserialize;

	zend_opcache_static_cache_safe_direct_register_class(ce, &handlers);
}

static zend_always_inline void zend_opcache_static_cache_safe_direct_register_internal_classes(void)
{
	zend_class_entry *date_ce, *immutable_ce, *timezone_ce, *interval_ce, *fixedarray_ce, *arrayobject_ce, *arrayiterator_ce, *recursive_arrayiterator_ce;
	const zend_opcache_static_cache_safe_direct_handlers *date_handlers, *spl_array_handlers, *spl_fixedarray_handlers;

	if (zend_opcache_static_cache_safe_direct_classes_marked) {
		return;
	}

	date_ce = php_date_get_date_ce();
	immutable_ce = php_date_get_immutable_ce();
	timezone_ce = php_date_get_timezone_ce();
	interval_ce = php_date_get_interval_ce();
	fixedarray_ce = spl_ce_SplFixedArray;
	arrayobject_ce = spl_ce_ArrayObject;
	arrayiterator_ce = spl_ce_ArrayIterator;
	recursive_arrayiterator_ce = spl_ce_RecursiveArrayIterator;
	if (date_ce == NULL || immutable_ce == NULL || timezone_ce == NULL || interval_ce == NULL ||
		fixedarray_ce == NULL || arrayobject_ce == NULL || arrayiterator_ce == NULL ||
		recursive_arrayiterator_ce == NULL
	) {
		return;
	}

	date_handlers = php_date_get_direct_cache_handlers();
	spl_fixedarray_handlers = spl_fixedarray_object_get_direct_cache_handlers();
	spl_array_handlers = spl_array_object_get_direct_cache_handlers();
	if (date_handlers != NULL) {
		zend_opcache_static_cache_safe_direct_register_class(date_ce, date_handlers);
		zend_opcache_static_cache_safe_direct_register_class(immutable_ce, date_handlers);
		zend_opcache_static_cache_safe_direct_register_class(timezone_ce, date_handlers);
		zend_opcache_static_cache_safe_direct_register_class(interval_ce, date_handlers);
	} else {
		zend_opcache_static_cache_safe_direct_register_serializer_class(date_ce, true);
		zend_opcache_static_cache_safe_direct_register_serializer_class(immutable_ce, true);
		zend_opcache_static_cache_safe_direct_register_serializer_class(timezone_ce, true);
		zend_opcache_static_cache_safe_direct_register_serializer_class(interval_ce, true);
	}

	if (spl_fixedarray_handlers != NULL) {
		zend_opcache_static_cache_safe_direct_register_class(fixedarray_ce, spl_fixedarray_handlers);
	} else {
		zend_opcache_static_cache_safe_direct_register_serializer_class(fixedarray_ce, false);
	}

	if (spl_array_handlers != NULL) {
		zend_opcache_static_cache_safe_direct_register_class(arrayobject_ce, spl_array_handlers);
		zend_opcache_static_cache_safe_direct_register_class(arrayiterator_ce, spl_array_handlers);
		zend_opcache_static_cache_safe_direct_register_class(recursive_arrayiterator_ce, spl_array_handlers);
	} else {
		zend_opcache_static_cache_safe_direct_register_serializer_class(arrayobject_ce, false);
		zend_opcache_static_cache_safe_direct_register_serializer_class(arrayiterator_ce, false);
		zend_opcache_static_cache_safe_direct_register_serializer_class(recursive_arrayiterator_ce, false);
	}

	zend_opcache_static_cache_safe_direct_classes_marked = true;
}

static zend_always_inline void zend_opcache_static_cache_invalidate_script_context(zend_opcache_static_cache_context *context, zend_persistent_script *persistent_script)
{
	zend_opcache_static_cache_context *previous_context;

	if (!zend_opcache_static_cache_context_runtime(context)->available || persistent_script == NULL) {
		return;
	}

	previous_context = zend_opcache_static_cache_activate_context(context);
	if (!zend_opcache_static_cache_wlock()) {
		zend_opcache_static_cache_restore_context(previous_context);
		return;
	}

	if (zend_opcache_static_cache_header_init_locked()) {
		zend_opcache_static_cache_delete_script_keys_locked(persistent_script);
	}

	zend_opcache_static_cache_unlock();
	zend_opcache_static_cache_mark_publish_skipped(context);
	zend_opcache_static_cache_restore_context(previous_context);
}

static zend_always_inline void zend_opcache_static_cache_invalidate_all_context(zend_opcache_static_cache_context *context)
{
	zend_opcache_static_cache_context *previous_context;

	if (!zend_opcache_static_cache_context_runtime(context)->available) {
		return;
	}

	previous_context = zend_opcache_static_cache_activate_context(context);
	if (!zend_opcache_static_cache_wlock()) {
		zend_opcache_static_cache_restore_context(previous_context);

		return;
	}

	if (!zend_opcache_static_cache_header_init_locked()) {
		zend_opcache_static_cache_unlock();
		zend_opcache_static_cache_restore_context(previous_context);

		return;
	}

	if (!zend_opcache_static_cache_clear_locked()) {
		zend_opcache_static_cache_unlock();
		zend_opcache_static_cache_restore_context(previous_context);

		return;
	}

	zend_opcache_static_cache_unlock();
	zend_opcache_static_cache_release_active_entry_locks();
	zend_opcache_static_cache_mark_publish_skipped(context);
	zend_opcache_static_cache_restore_context(previous_context);
}

static zend_always_inline bool zend_opcache_static_cache_parse_ttl(zend_long ttl, uint32_t arg_num)
{
	if (ttl < 0) {
		zend_argument_value_error(arg_num, "must be greater than or equal to 0");

		return false;
	}

	return true;
}

static zend_always_inline bool zend_opcache_static_cache_parse_lease(zend_long lease, uint32_t arg_num)
{
	if (lease < 0) {
		zend_argument_value_error(arg_num, "must be greater than or equal to 0");

		return false;
	}

	return true;
}

static zend_always_inline bool zend_opcache_static_cache_require_available_read(void)
{
	zend_opcache_static_cache_context *context = zend_opcache_static_cache_active_context();

	if (!zend_opcache_validate_api_restriction()) {
		zend_throw_exception_ex(zend_opcache_static_cache_exception_ce, 0, ACCELERATOR_PRODUCT_NAME " API access is restricted");

		return false;
	}

	if (!zend_opcache_static_cache_active_runtime()->available) {
		if (zend_opcache_static_cache_active_runtime()->failure_reason) {
			zend_throw_exception_ex(zend_opcache_static_cache_exception_ce, 0, "%s", zend_opcache_static_cache_active_runtime()->failure_reason);
		} else {
			zend_throw_exception_ex(zend_opcache_static_cache_exception_ce, 0, "OPcache %s is disabled", context->name);
		}

		return false;
	}

	if (!zend_opcache_static_cache_rlock()) {
		zend_throw_exception_ex(zend_opcache_static_cache_exception_ce, 0, "Unable to acquire the %s read lock", context->name);

		return false;
	}

	if (!zend_opcache_static_cache_header_is_initialized_locked()) {
		zend_opcache_static_cache_unlock();
		zend_throw_exception_ex(zend_opcache_static_cache_exception_ce, 0, "Unable to access the %s header", context->name);

		return false;
	}

	return true;
}

static zend_always_inline bool zend_opcache_static_cache_validate_available_write(void)
{
	zend_opcache_static_cache_context *context = zend_opcache_static_cache_active_context();

	if (!zend_opcache_validate_api_restriction()) {
		zend_throw_exception_ex(zend_opcache_static_cache_exception_ce, 0, ACCELERATOR_PRODUCT_NAME " API access is restricted");

		return false;
	}

	if (!zend_opcache_static_cache_active_runtime()->available) {
		if (zend_opcache_static_cache_active_runtime()->failure_reason) {
			zend_throw_exception_ex(zend_opcache_static_cache_exception_ce, 0, "%s", zend_opcache_static_cache_active_runtime()->failure_reason);
		} else {
			zend_throw_exception_ex(zend_opcache_static_cache_exception_ce, 0, "OPcache %s is disabled", context->name);
		}

		return false;
	}

	return true;
}

static zend_always_inline bool zend_opcache_static_cache_acquire_write_lock(void)
{
	zend_opcache_static_cache_context *context = zend_opcache_static_cache_active_context();

	if (!zend_opcache_static_cache_wlock()) {
		zend_throw_exception_ex(zend_opcache_static_cache_exception_ce, 0, "Unable to acquire the %s lock", context->name);

		return false;
	}

	if (!zend_opcache_static_cache_header_init_locked()) {
		zend_opcache_static_cache_unlock();
		zend_throw_exception_ex(zend_opcache_static_cache_exception_ce, 0, "Unable to initialize the %s header", context->name);

		return false;
	}

	return true;
}

static zend_always_inline bool zend_opcache_static_cache_require_available_write(void)
{
	if (!zend_opcache_static_cache_validate_available_write()) {
		return false;
	}

	return zend_opcache_static_cache_acquire_write_lock();
}

static zend_always_inline bool zend_opcache_static_cache_begin_entry_mutation(zend_string *key, bool *release_entry_lock)
{
	ZEND_ASSERT(release_entry_lock != NULL);
	*release_entry_lock = false;

	if (!zend_opcache_static_cache_has_entry_lock(key)) {
		if (!zend_opcache_static_cache_acquire_entry_lock(key)) {
			return false;
		}

		*release_entry_lock = true;
	}

	return true;
}

static zend_always_inline void zend_opcache_static_cache_finish_entry_mutation(zend_string *key, bool release_entry_lock, bool successful_mutation)
{
	if (successful_mutation || release_entry_lock) {
		zend_opcache_static_cache_release_entry_lock(key);
	}
}

static zend_always_inline bool zend_opcache_static_cache_explicit_delete_prevalidated(zend_string *key)
{
	bool deleted, release_entry_lock;

	release_entry_lock = zend_opcache_static_cache_has_entry_lock(key);

	if (!zend_opcache_static_cache_acquire_write_lock()) {
		return false;
	}

	deleted = zend_opcache_static_cache_delete_locked(key);
	zend_opcache_static_cache_unlock();
	if (deleted && release_entry_lock) {
		zend_opcache_static_cache_release_entry_lock(key);
	}

	return deleted;
}

static zend_always_inline bool zend_opcache_static_cache_explicit_delete_or_class_prevalidated(zend_string *key_or_class)
{
	zend_class_entry *ce;

	ce = zend_opcache_static_cache_lookup_loaded_class_key(key_or_class);
	if (ce != NULL) {
		if (zend_opcache_static_cache_class_has_keys(ce)) {
			if (!zend_opcache_static_cache_acquire_write_lock()) {
				return false;
			}

			zend_opcache_static_cache_delete_class_keys_locked(ce);
			zend_opcache_static_cache_unlock();
		}

		return true;
	}

	return zend_opcache_static_cache_explicit_delete_prevalidated(key_or_class);
}

static zend_always_inline bool zend_opcache_static_cache_explicit_clear_prevalidated(void)
{
	bool cleared;

	if (!zend_opcache_static_cache_acquire_write_lock()) {
		return false;
	}

	cleared = zend_opcache_static_cache_clear_locked();
	zend_opcache_static_cache_unlock();
	if (cleared) {
		zend_opcache_static_cache_release_active_entry_locks();
	}

	return cleared;
}

static zend_always_inline bool zend_opcache_static_cache_explicit_store_prevalidated(zend_string *key, zval *value, zend_long ttl, bool throw_on_failure)
{
	zend_opcache_static_cache_prepared_value prepared;
	bool stored, release_entry_lock = false;

	if (!zend_opcache_static_cache_prepare_value(&prepared, key, value, throw_on_failure, false)) {
		return false;
	}

	if (!zend_opcache_static_cache_begin_entry_mutation(key, &release_entry_lock)) {
		zend_opcache_static_cache_destroy_prepared_value(&prepared);

		return false;
	}

	/* Callers validate availability before staging the value so large shared-graph
	 * builds stay outside the write lock. Once that preflight passed, the commit
	 * path only needs to acquire the lock and publish the prepared payload. */
	if (!zend_opcache_static_cache_acquire_write_lock()) {
		zend_opcache_static_cache_destroy_prepared_value(&prepared);
		if (release_entry_lock) {
			zend_opcache_static_cache_release_entry_lock(key);
		}

		return false;
	}

	stored = zend_opcache_static_cache_store_prepared_locked(key, value, &prepared, ttl, throw_on_failure);
	zend_opcache_static_cache_unlock();
	zend_opcache_static_cache_destroy_prepared_value(&prepared);
	zend_opcache_static_cache_finish_entry_mutation(key, release_entry_lock, stored);

	return stored;
}

static zend_always_inline zend_result zend_opcache_static_cache_fetch_api(
		zend_opcache_static_cache_context *context,
		zend_string *key,
		zval *default_value,
		zval *return_value)
{
	zend_opcache_static_cache_context *previous_context;
	bool fetched, found = false;

	previous_context = zend_opcache_static_cache_activate_context(context);
	if (!zend_opcache_static_cache_require_available_read()) {
		zend_opcache_static_cache_restore_context(previous_context);

		return FAILURE;
	}

	fetched = zend_opcache_static_cache_fetch_locked(key, return_value, false, &found, true);
	zend_opcache_static_cache_unlock();
	zend_opcache_static_cache_restore_context(previous_context);

	if (!fetched) {
		if (!found) {
			ZVAL_COPY(return_value, default_value);

			return SUCCESS;
		}

		if (!EG(exception)) {
			zend_throw_exception_ex(zend_opcache_static_cache_exception_ce, 0, "Stored %s value for key \"%s\" could not be decoded", context->name, ZSTR_VAL(key));
		}

		return FAILURE;
	}

	return SUCCESS;
}

static zend_always_inline zend_result zend_opcache_static_cache_fetch_array_api(
		zend_opcache_static_cache_context *context,
		HashTable *keys,
		zval *default_value,
		zval *return_value)
{
	zend_opcache_static_cache_context *previous_context;
	zend_string **prepared_keys, *key;
	zval fetched_value;
	uint32_t key_count, index;
	bool fetched, found;

	if (!zend_opcache_static_cache_prepare_key_list(keys, &prepared_keys, &key_count, 1)) {
		return FAILURE;
	}

	previous_context = zend_opcache_static_cache_activate_context(context);
	if (!zend_opcache_static_cache_require_available_read()) {
		zend_opcache_static_cache_restore_context(previous_context);
		zend_opcache_static_cache_release_key_list(prepared_keys, key_count);

		return FAILURE;
	}

	array_init_size(return_value, key_count);

	for (index = 0; index < key_count; index++) {
		key = prepared_keys[index];
		fetched = zend_opcache_static_cache_fetch_locked(key, &fetched_value, false, &found, false);
		if (!fetched) {
			if (!found) {
				ZVAL_COPY(&fetched_value, default_value);
			} else {
				if (!EG(exception)) {
					zend_throw_exception_ex(zend_opcache_static_cache_exception_ce, 0, "Stored %s value for key \"%s\" could not be decoded", context->name, ZSTR_VAL(key));
				}
				zval_ptr_dtor(return_value);
				ZVAL_UNDEF(return_value);
				zend_opcache_static_cache_unlock();
				zend_opcache_static_cache_restore_context(previous_context);
				zend_opcache_static_cache_release_key_list(prepared_keys, key_count);

				return FAILURE;
			}
		}

		zend_hash_update(Z_ARRVAL_P(return_value), key, &fetched_value);
	}

	zend_opcache_static_cache_unlock();
	zend_opcache_static_cache_restore_context(previous_context);
	zend_opcache_static_cache_release_key_list(prepared_keys, key_count);

	return SUCCESS;
}

static zend_always_inline zend_result zend_opcache_static_cache_exists_api(zend_opcache_static_cache_context *context, zend_string *key, bool *exists)
{
	zend_opcache_static_cache_context *previous_context;

	previous_context = zend_opcache_static_cache_activate_context(context);
	if (!zend_opcache_static_cache_require_available_read()) {
		zend_opcache_static_cache_restore_context(previous_context);

		return FAILURE;
	}

	*exists = zend_opcache_static_cache_exists_locked(key);
	zend_opcache_static_cache_unlock();
	zend_opcache_static_cache_restore_context(previous_context);

	return SUCCESS;
}

static zend_always_inline zend_result zend_opcache_static_cache_lock_api(
		zend_opcache_static_cache_context *context,
		zend_string *key,
		zend_long lease,
		bool *locked)
{
	zend_opcache_static_cache_context *previous_context;

	previous_context = zend_opcache_static_cache_activate_context(context);
	if (!zend_opcache_static_cache_validate_available_write()) {
		zend_opcache_static_cache_restore_context(previous_context);

		return FAILURE;
	}

	*locked = zend_opcache_static_cache_try_acquire_entry_lock(key, lease);
	zend_opcache_static_cache_restore_context(previous_context);

	return SUCCESS;
}

static zend_always_inline zend_result zend_opcache_static_cache_unlock_api(zend_opcache_static_cache_context *context, zend_string *key, bool *unlocked)
{
	zend_opcache_static_cache_context *previous_context;

	previous_context = zend_opcache_static_cache_activate_context(context);
	if (!zend_opcache_static_cache_validate_available_write()) {
		zend_opcache_static_cache_restore_context(previous_context);

		return FAILURE;
	}

	*unlocked = zend_opcache_static_cache_release_entry_lock(key);
	zend_opcache_static_cache_restore_context(previous_context);

	return SUCCESS;
}

static zend_always_inline void zend_opcache_static_cache_disable_subsystem(const char *failure_reason)
{
	zend_opcache_static_cache_context *previous_context;

	zend_opcache_static_cache_subsystem_disabled = true;
	zend_opcache_static_cache_subsystem_failure_reason = failure_reason != NULL
		? failure_reason
		: "OPcache static cache startup failed"
	;

	zend_opcache_static_cache_unregister_hooks();

	previous_context = zend_opcache_static_cache_activate_context(&zend_opcache_static_cache_volatile_context_state);
	zend_opcache_static_cache_shutdown_storage();
	zend_opcache_static_cache_reset_runtime();
	zend_opcache_static_cache_activate_context(&zend_opcache_static_cache_pinned_context_state);
	zend_opcache_static_cache_shutdown_storage();
	zend_opcache_static_cache_reset_runtime();
	zend_opcache_static_cache_restore_context(previous_context);
}

zend_result zend_opcache_register_functions(int module_type)
{
	return zend_register_functions(NULL, ext_functions, NULL, module_type);
}

zend_result zend_opcache_static_cache_minit(void)
{
	zend_opcache_static_cache_context *previous_context;

	zend_opcache_static_cache_subsystem_disabled = false;
	zend_opcache_static_cache_subsystem_failure_reason = NULL;
	zend_opcache_static_cache_safe_direct_classes_marked = false;

	zend_opcache_static_cache_register_classes();
	zend_opcache_static_cache_safe_direct_handlers_init();
	zend_opcache_static_cache_register_accelerator_handlers();

	previous_context = zend_opcache_static_cache_activate_context(&zend_opcache_static_cache_volatile_context_state);
	zend_opcache_static_cache_reset_storage();

	zend_opcache_static_cache_activate_context(&zend_opcache_static_cache_pinned_context_state);
	zend_opcache_static_cache_reset_storage();

	zend_opcache_static_cache_restore_context(previous_context);

	return SUCCESS;
}

static void zend_opcache_static_cache_startup(void)
{
	const char *failure_reason;
	zend_opcache_static_cache_context *previous_context;

	previous_context = zend_opcache_static_cache_activate_context(&zend_opcache_static_cache_volatile_context_state);
	zend_opcache_static_cache_reset_runtime();
	if (zend_opcache_static_cache_active_runtime()->enabled && ZCG(enabled) && accel_startup_ok && !file_cache_only) {
		if (!zend_opcache_static_cache_startup_storage_before_request()) {
			failure_reason = zend_opcache_static_cache_active_runtime()->failure_reason;
			zend_opcache_static_cache_restore_context(previous_context);
			zend_opcache_static_cache_disable_subsystem(failure_reason);

			return;
		}
	}

	zend_opcache_static_cache_activate_context(&zend_opcache_static_cache_pinned_context_state);
	zend_opcache_static_cache_reset_runtime();
	if (zend_opcache_static_cache_active_runtime()->enabled && ZCG(enabled) && accel_startup_ok && !file_cache_only) {
		if (!zend_opcache_static_cache_startup_storage_before_request()) {
			failure_reason = zend_opcache_static_cache_active_runtime()->failure_reason;
			zend_opcache_static_cache_restore_context(previous_context);
			zend_opcache_static_cache_disable_subsystem(failure_reason);

			return;
		}
	}

	zend_opcache_static_cache_restore_context(previous_context);

	if (!zend_opcache_static_cache_subsystem_disabled &&
		(
			ZCG(accel_directives).static_cache_volatile_size_mb != 0 ||
			ZCG(accel_directives).static_cache_pinned_size_mb != 0
		)
	) {
		zend_opcache_static_cache_register_hooks();
	}
}

static void zend_opcache_static_cache_post_startup(void)
{
	const char *failure_reason;
	zend_opcache_static_cache_context *previous_context;

	previous_context = zend_opcache_static_cache_activate_context(&zend_opcache_static_cache_volatile_context_state);
	zend_opcache_static_cache_reset_runtime();
	if (!(zend_opcache_static_cache_active_context()->storage).initialized &&
		zend_opcache_static_cache_active_runtime()->enabled &&
		ZCG(enabled) &&
		accel_startup_ok &&
		!file_cache_only
	) {
		if (!zend_opcache_static_cache_startup_storage_before_request()) {
			failure_reason = zend_opcache_static_cache_active_runtime()->failure_reason;
			zend_opcache_static_cache_restore_context(previous_context);
			zend_opcache_static_cache_disable_subsystem(failure_reason);

			return;
		}
	}

	zend_opcache_static_cache_activate_context(&zend_opcache_static_cache_pinned_context_state);
	zend_opcache_static_cache_reset_runtime();
	if (!(zend_opcache_static_cache_active_context()->storage).initialized &&
		zend_opcache_static_cache_active_runtime()->enabled &&
		ZCG(enabled) &&
		accel_startup_ok &&
		!file_cache_only
	) {
		if (!zend_opcache_static_cache_startup_storage_before_request()) {
			failure_reason = zend_opcache_static_cache_active_runtime()->failure_reason;
			zend_opcache_static_cache_restore_context(previous_context);
			zend_opcache_static_cache_disable_subsystem(failure_reason);

			return;
		}
	}

	zend_opcache_static_cache_restore_context(previous_context);

	if (!zend_opcache_static_cache_subsystem_disabled &&
		(
			ZCG(accel_directives).static_cache_volatile_size_mb != 0 ||
			ZCG(accel_directives).static_cache_pinned_size_mb != 0
		)
	) {
		zend_opcache_static_cache_safe_direct_register_internal_classes();
	}
}

void zend_opcache_static_cache_mshutdown(void)
{
	zend_opcache_static_cache_context *previous_context;

	zend_opcache_static_cache_unregister_hooks();

	previous_context = zend_opcache_static_cache_activate_context(&zend_opcache_static_cache_volatile_context_state);
	zend_opcache_static_cache_shutdown_storage();
	zend_opcache_static_cache_reset_runtime();
	zend_opcache_static_cache_activate_context(&zend_opcache_static_cache_pinned_context_state);
	zend_opcache_static_cache_shutdown_storage();
	zend_opcache_static_cache_reset_runtime();
	zend_opcache_static_cache_restore_context(previous_context);

	zend_opcache_static_cache_subsystem_disabled = false;
	zend_opcache_static_cache_subsystem_failure_reason = NULL;
	zend_opcache_static_cache_safe_direct_classes_marked = false;
	zend_opcache_static_cache_safe_direct_handlers_destroy();
	zend_accel_register_static_cache_handlers(NULL);
}

static zend_result zend_opcache_static_cache_rinit(void)
{
	zend_opcache_static_cache_context *previous_context;
	bool static_cache_available;

	previous_context = zend_opcache_static_cache_activate_context(&zend_opcache_static_cache_volatile_context_state);
	zend_opcache_static_cache_ensure_ready();
	zend_opcache_static_cache_activate_context(&zend_opcache_static_cache_pinned_context_state);
	zend_opcache_static_cache_ensure_ready();
	zend_opcache_static_cache_restore_context(previous_context);
	zend_opcache_static_cache_clear_lookup_caches();
	zend_opcache_static_cache_reset_publish_skips();

	EG(static_cache_class_access_active) = false;
	EG(tracked_mutation_hooks_active) = false;

	static_cache_available = zend_opcache_static_cache_context_runtime(&zend_opcache_static_cache_volatile_context_state)->available ||
		zend_opcache_static_cache_context_runtime(&zend_opcache_static_cache_pinned_context_state)->available
	;
	if (!static_cache_available) {
		return SUCCESS;
	}

	zend_opcache_static_cache_register_hooks();

	EG(static_cache_class_access_active) = true;
	zend_opcache_static_cache_request_init();

	return SUCCESS;
}

zend_result zend_opcache_static_cache_rshutdown(void)
{
	bool shared_graph_refs_released;

	zend_opcache_static_cache_clear_lookup_caches();
	zend_opcache_static_cache_request_shutdown();
	zend_opcache_static_cache_release_request_entry_locks();
	zend_opcache_static_cache_release_request_local_slots();

	shared_graph_refs_released = zend_opcache_static_cache_release_request_shared_graph_refs();
	if (shared_graph_refs_released) {
		zend_opcache_static_cache_compact_after_request_shutdown();
	}

	EG(static_cache_class_access_active) = false;
	EG(tracked_mutation_hooks_active) = false;

	return SUCCESS;
}

static void zend_opcache_static_cache_invalidate_script(zend_persistent_script *persistent_script)
{
	zend_opcache_static_cache_invalidate_script_context(&zend_opcache_static_cache_pinned_context_state, persistent_script);
	zend_opcache_static_cache_invalidate_script_context(&zend_opcache_static_cache_volatile_context_state, persistent_script);
}

static void zend_opcache_static_cache_register_accelerator_handlers(void)
{
	static const zend_opcache_static_cache_accelerator_handlers handlers = {
		zend_opcache_static_cache_startup,
		zend_opcache_static_cache_post_startup,
		zend_opcache_static_cache_rinit,
		zend_opcache_static_cache_invalidate_script
	};

	zend_accel_register_static_cache_handlers(&handlers);
}

void zend_opcache_static_cache_invalidate_all(void)
{
	zend_opcache_static_cache_invalidate_all_context(&zend_opcache_static_cache_pinned_context_state);
	zend_opcache_static_cache_invalidate_all_context(&zend_opcache_static_cache_volatile_context_state);
}

void zend_opcache_static_cache_volatile_get_status(zval *return_value)
{
	zend_opcache_static_cache_context *previous_context = zend_opcache_static_cache_activate_context(&zend_opcache_static_cache_volatile_context_state);

	zend_opcache_static_cache_populate_info(return_value);
	zend_opcache_static_cache_restore_context(previous_context);
}

void zend_opcache_static_cache_pinned_get_status(zval *return_value)
{
	zend_opcache_static_cache_context *previous_context = zend_opcache_static_cache_activate_context(&zend_opcache_static_cache_pinned_context_state);

	zend_opcache_static_cache_populate_info(return_value);
	zend_opcache_static_cache_restore_context(previous_context);
}

bool zend_opcache_static_cache_volatile_is_enabled(void)
{
	return zend_opcache_static_cache_context_runtime(&zend_opcache_static_cache_volatile_context_state)->enabled;
}

bool zend_opcache_static_cache_volatile_is_available(void)
{
	return zend_opcache_static_cache_context_runtime(&zend_opcache_static_cache_volatile_context_state)->available;
}

const char *zend_opcache_static_cache_volatile_failure_reason(void)
{
	return zend_opcache_static_cache_context_runtime(&zend_opcache_static_cache_volatile_context_state)->failure_reason;
}

bool zend_opcache_static_cache_pinned_is_enabled(void)
{
	return zend_opcache_static_cache_context_runtime(&zend_opcache_static_cache_pinned_context_state)->enabled;
}

bool zend_opcache_static_cache_pinned_is_available(void)
{
	return zend_opcache_static_cache_context_runtime(&zend_opcache_static_cache_pinned_context_state)->available;
}

const char *zend_opcache_static_cache_pinned_failure_reason(void)
{
	return zend_opcache_static_cache_context_runtime(&zend_opcache_static_cache_pinned_context_state)->failure_reason;
}

ZEND_METHOD(OPcache_VolatileStatic, __construct)
{
	zend_long ttl = 0;
	zval default_strategy, *strategy = NULL;

	ZVAL_UNDEF(&default_strategy);

	ZEND_PARSE_PARAMETERS_START(0, 2)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(ttl)
		Z_PARAM_OBJECT_OF_CLASS(strategy, zend_opcache_static_cache_strategy_ce)
	ZEND_PARSE_PARAMETERS_END();

	if (!zend_opcache_static_cache_parse_ttl(ttl, 1)) {
		RETURN_THROWS();
	}

	if (strategy == NULL) {
		ZVAL_OBJ_COPY(&default_strategy, zend_enum_get_case_cstr(zend_opcache_static_cache_strategy_ce, "Immediate"));
		strategy = &default_strategy;
	}

	zend_update_property_long(zend_opcache_static_cache_volatile_static_attribute_ce, Z_OBJ_P(ZEND_THIS), ZEND_STRL("ttl"), ttl);
	zend_update_property(zend_opcache_static_cache_volatile_static_attribute_ce, Z_OBJ_P(ZEND_THIS), ZEND_STRL("strategy"), strategy);
	zval_ptr_dtor(&default_strategy);
}

ZEND_FUNCTION(OPcache_volatile_store)
{
	zend_string *key;
	zend_long ttl = 0;
	zval *value;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(key)
		Z_PARAM_ZVAL(value)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(ttl)
	ZEND_PARSE_PARAMETERS_END();

	if (!zend_opcache_static_cache_validate_key(key, 1)) {
		RETURN_THROWS();
	}

	if (!zend_opcache_static_cache_validate_api_value(value, 2)) {
		RETURN_THROWS();
	}

	if (!zend_opcache_static_cache_parse_ttl(ttl, 3) || !zend_opcache_static_cache_validate_available_write()) {
		RETURN_THROWS();
	}

	if (!zend_opcache_static_cache_explicit_store_prevalidated(key, value, ttl, false)) {
		if (EG(exception)) {
			RETURN_THROWS();
		}

		RETURN_FALSE;
	}

	RETURN_TRUE;
}

ZEND_FUNCTION(OPcache_volatile_store_array)
{
	zend_string *key;
	zend_long ttl = 0;
	zval *value;
	HashTable *values;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ARRAY_HT(values)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(ttl)
	ZEND_PARSE_PARAMETERS_END();

	if (!zend_opcache_static_cache_parse_ttl(ttl, 2) || !zend_opcache_static_cache_validate_available_write()) {
		RETURN_THROWS();
	}

	if (!zend_opcache_static_cache_validate_store_array_keys(values, 1)) {
		RETURN_THROWS();
	}

	ZEND_HASH_FOREACH_STR_KEY_VAL(values, key, value) {
		ZEND_ASSERT(key != NULL);

		if (!zend_opcache_static_cache_explicit_store_prevalidated(key, value, ttl, false)) {
			if (EG(exception)) {
				RETURN_THROWS();
			}

			RETURN_FALSE;
		}
	} ZEND_HASH_FOREACH_END();

	RETURN_TRUE;
}

ZEND_FUNCTION(OPcache_volatile_fetch)
{
	zend_string *key;
	zval *default_value = NULL, default_null;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(key)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(default_value)
	ZEND_PARSE_PARAMETERS_END();

	if (!zend_opcache_static_cache_validate_key(key, 1)) {
		RETURN_THROWS();
	}

	if (default_value == NULL) {
		ZVAL_NULL(&default_null);
		default_value = &default_null;
	}

	if (!zend_opcache_static_cache_validate_api_value(default_value, 2)) {
		RETURN_THROWS();
	}

	if (zend_opcache_static_cache_fetch_api(&zend_opcache_static_cache_volatile_context_state, key, default_value, return_value) == FAILURE) {
		RETURN_THROWS();
	}
}

ZEND_FUNCTION(OPcache_volatile_fetch_array)
{
	HashTable *keys;
	zval *default_value = NULL, default_null;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ARRAY_HT(keys)
		Z_PARAM_OPTIONAL
		Z_PARAM_ARRAY_OR_NULL(default_value)
	ZEND_PARSE_PARAMETERS_END();

	if (default_value == NULL) {
		ZVAL_NULL(&default_null);
		default_value = &default_null;
	}

	if (zend_opcache_static_cache_fetch_array_api(&zend_opcache_static_cache_volatile_context_state, keys, default_value, return_value) == FAILURE) {
		RETURN_THROWS();
	}
}

ZEND_FUNCTION(OPcache_volatile_exists)
{
	zend_string *key;
	bool exists;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(key)
	ZEND_PARSE_PARAMETERS_END();

	if (!zend_opcache_static_cache_validate_key(key, 1)) {
		RETURN_THROWS();
	}

	if (zend_opcache_static_cache_exists_api(&zend_opcache_static_cache_volatile_context_state, key, &exists) == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_BOOL(exists);
}

ZEND_FUNCTION(OPcache_volatile_lock)
{
	zend_string *key;
	zend_long lease = 0;
	bool locked;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(key)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(lease)
	ZEND_PARSE_PARAMETERS_END();

	if (!zend_opcache_static_cache_validate_key(key, 1)) {
		RETURN_THROWS();
	}

	if (!zend_opcache_static_cache_parse_lease(lease, 2)) {
		RETURN_THROWS();
	}

	if (zend_opcache_static_cache_lock_api(&zend_opcache_static_cache_volatile_context_state, key, lease, &locked) == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_BOOL(locked);
}

ZEND_FUNCTION(OPcache_volatile_unlock)
{
	zend_string *key;
	bool unlocked;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(key)
	ZEND_PARSE_PARAMETERS_END();

	if (!zend_opcache_static_cache_validate_key(key, 1)) {
		RETURN_THROWS();
	}

	if (zend_opcache_static_cache_unlock_api(&zend_opcache_static_cache_volatile_context_state, key, &unlocked) == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_BOOL(unlocked);
}

ZEND_FUNCTION(OPcache_volatile_delete)
{
	zend_string *key_or_class;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(key_or_class)
	ZEND_PARSE_PARAMETERS_END();

	if (!zend_opcache_static_cache_validate_key_or_class(key_or_class, 1)) {
		RETURN_THROWS();
	}

	if (!zend_opcache_static_cache_validate_available_write()) {
		RETURN_THROWS();
	}

	if (!zend_opcache_static_cache_explicit_delete_or_class_prevalidated(key_or_class)) {
		RETURN_THROWS();
	}
}

ZEND_FUNCTION(OPcache_volatile_delete_array)
{
	zend_string **prepared_keys;
	HashTable *keys;
	uint32_t key_count, index;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(keys)
	ZEND_PARSE_PARAMETERS_END();

	if (!zend_opcache_static_cache_prepare_key_list(keys, &prepared_keys, &key_count, 1)) {
		RETURN_THROWS();
	}

	if (!zend_opcache_static_cache_validate_available_write()) {
		zend_opcache_static_cache_release_key_list(prepared_keys, key_count);

		RETURN_THROWS();
	}

	for (index = 0; index < key_count; index++) {
		if (!zend_opcache_static_cache_explicit_delete_prevalidated(prepared_keys[index])) {
			zend_opcache_static_cache_release_key_list(prepared_keys, key_count);

			RETURN_THROWS();
		}
	}

	zend_opcache_static_cache_release_key_list(prepared_keys, key_count);
}

ZEND_FUNCTION(OPcache_volatile_clear)
{
	ZEND_PARSE_PARAMETERS_NONE();

	if (!zend_opcache_static_cache_validate_available_write()) {
		RETURN_THROWS();
	}

	if (!zend_opcache_static_cache_explicit_clear_prevalidated()) {
		zend_throw_exception_ex(zend_opcache_static_cache_exception_ce, 0, "Unable to clear the volatile cache");

		RETURN_THROWS();
	}

	zend_opcache_static_cache_mark_publish_skipped(&zend_opcache_static_cache_volatile_context_state);
}

ZEND_FUNCTION(OPcache_volatile_cache_info)
{
	zend_opcache_static_cache_context *previous_context;

	ZEND_PARSE_PARAMETERS_NONE();

	if (!zend_opcache_validate_api_restriction()) {
		zend_throw_exception_ex(zend_opcache_static_cache_exception_ce, 0, ACCELERATOR_PRODUCT_NAME " API access is restricted");

		RETURN_THROWS();
	}

	previous_context = zend_opcache_static_cache_activate_context(&zend_opcache_static_cache_volatile_context_state);
	zend_opcache_static_cache_populate_info(return_value);
	zend_opcache_static_cache_restore_context(previous_context);
}

ZEND_FUNCTION(OPcache_pinned_store)
{
	zend_opcache_static_cache_context *previous_context;
	zend_string *key;
	zval *value;
	bool stored;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_STR(key)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	if (!zend_opcache_static_cache_validate_key(key, 1)) {
		RETURN_THROWS();
	}

	if (!zend_opcache_static_cache_validate_api_value(value, 2)) {
		RETURN_THROWS();
	}

	previous_context = zend_opcache_static_cache_activate_context(&zend_opcache_static_cache_pinned_context_state);
	if (!zend_opcache_static_cache_validate_available_write()) {
		zend_opcache_static_cache_restore_context(previous_context);

		RETURN_THROWS();
	}

	stored = zend_opcache_static_cache_explicit_store_prevalidated(key, value, 0, true);
	zend_opcache_static_cache_restore_context(previous_context);

	if (!stored) {
		RETURN_THROWS();
	}
}

ZEND_FUNCTION(OPcache_pinned_store_array)
{
	zend_opcache_static_cache_context *previous_context;
	zend_string *key;
	zval *value;
	HashTable *values;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(values)
	ZEND_PARSE_PARAMETERS_END();

	previous_context = zend_opcache_static_cache_activate_context(&zend_opcache_static_cache_pinned_context_state);
	if (!zend_opcache_static_cache_validate_available_write()) {
		zend_opcache_static_cache_restore_context(previous_context);

		RETURN_THROWS();
	}

	if (!zend_opcache_static_cache_validate_store_array_keys(values, 1)) {
		zend_opcache_static_cache_restore_context(previous_context);

		RETURN_THROWS();
	}

	ZEND_HASH_FOREACH_STR_KEY_VAL(values, key, value) {
		ZEND_ASSERT(key != NULL);

		if (!zend_opcache_static_cache_explicit_store_prevalidated(key, value, 0, true)) {
			zend_opcache_static_cache_restore_context(previous_context);

			RETURN_THROWS();
		}
	} ZEND_HASH_FOREACH_END();

	zend_opcache_static_cache_restore_context(previous_context);
}

ZEND_FUNCTION(OPcache_pinned_fetch)
{
	zend_string *key;
	zval *default_value = NULL, default_null;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(key)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(default_value)
	ZEND_PARSE_PARAMETERS_END();

	if (!zend_opcache_static_cache_validate_key(key, 1)) {
		RETURN_THROWS();
	}

	if (default_value == NULL) {
		ZVAL_NULL(&default_null);
		default_value = &default_null;
	}

	if (!zend_opcache_static_cache_validate_api_value(default_value, 2)) {
		RETURN_THROWS();
	}

	if (zend_opcache_static_cache_fetch_api(&zend_opcache_static_cache_pinned_context_state, key, default_value, return_value) == FAILURE) {
		RETURN_THROWS();
	}
}

ZEND_FUNCTION(OPcache_pinned_fetch_array)
{
	HashTable *keys;
	zval *default_value = NULL, default_null;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ARRAY_HT(keys)
		Z_PARAM_OPTIONAL
		Z_PARAM_ARRAY_OR_NULL(default_value)
	ZEND_PARSE_PARAMETERS_END();

	if (default_value == NULL) {
		ZVAL_NULL(&default_null);
		default_value = &default_null;
	}

	if (zend_opcache_static_cache_fetch_array_api(&zend_opcache_static_cache_pinned_context_state, keys, default_value, return_value) == FAILURE) {
		RETURN_THROWS();
	}
}

ZEND_FUNCTION(OPcache_pinned_exists)
{
	zend_string *key;
	bool exists;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(key)
	ZEND_PARSE_PARAMETERS_END();

	if (!zend_opcache_static_cache_validate_key(key, 1)) {
		RETURN_THROWS();
	}

	if (zend_opcache_static_cache_exists_api(&zend_opcache_static_cache_pinned_context_state, key, &exists) == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_BOOL(exists);
}

ZEND_FUNCTION(OPcache_pinned_lock)
{
	zend_string *key;
	zend_long lease = 0;
	bool locked;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(key)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(lease)
	ZEND_PARSE_PARAMETERS_END();

	if (!zend_opcache_static_cache_validate_key(key, 1)) {
		RETURN_THROWS();
	}

	if (!zend_opcache_static_cache_parse_lease(lease, 2)) {
		RETURN_THROWS();
	}

	if (zend_opcache_static_cache_lock_api(&zend_opcache_static_cache_pinned_context_state, key, lease, &locked) == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_BOOL(locked);
}

ZEND_FUNCTION(OPcache_pinned_unlock)
{
	zend_string *key;
	bool unlocked;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(key)
	ZEND_PARSE_PARAMETERS_END();

	if (!zend_opcache_static_cache_validate_key(key, 1)) {
		RETURN_THROWS();
	}

	if (zend_opcache_static_cache_unlock_api(&zend_opcache_static_cache_pinned_context_state, key, &unlocked) == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_BOOL(unlocked);
}

ZEND_FUNCTION(OPcache_pinned_delete)
{
	zend_opcache_static_cache_context *previous_context;
	zend_string *key_or_class;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(key_or_class)
	ZEND_PARSE_PARAMETERS_END();

	if (!zend_opcache_static_cache_validate_key_or_class(key_or_class, 1)) {
		RETURN_THROWS();
	}

	previous_context = zend_opcache_static_cache_activate_context(&zend_opcache_static_cache_pinned_context_state);
	if (!zend_opcache_static_cache_validate_available_write()) {
		zend_opcache_static_cache_restore_context(previous_context);

		RETURN_THROWS();
	}

	if (!zend_opcache_static_cache_explicit_delete_or_class_prevalidated(key_or_class)) {
		zend_opcache_static_cache_restore_context(previous_context);

		RETURN_THROWS();
	}
	zend_opcache_static_cache_restore_context(previous_context);
}

ZEND_FUNCTION(OPcache_pinned_delete_array)
{
	zend_opcache_static_cache_context *previous_context;
	zend_string **prepared_keys;
	HashTable *keys;
	uint32_t key_count, index;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(keys)
	ZEND_PARSE_PARAMETERS_END();

	if (!zend_opcache_static_cache_prepare_key_list(keys, &prepared_keys, &key_count, 1)) {
		RETURN_THROWS();
	}

	previous_context = zend_opcache_static_cache_activate_context(&zend_opcache_static_cache_pinned_context_state);
	if (!zend_opcache_static_cache_validate_available_write()) {
		zend_opcache_static_cache_restore_context(previous_context);
		zend_opcache_static_cache_release_key_list(prepared_keys, key_count);

		RETURN_THROWS();
	}

	for (index = 0; index < key_count; index++) {
		if (!zend_opcache_static_cache_explicit_delete_prevalidated(prepared_keys[index])) {
			zend_opcache_static_cache_restore_context(previous_context);
			zend_opcache_static_cache_release_key_list(prepared_keys, key_count);

			RETURN_THROWS();
		}
	}

	zend_opcache_static_cache_restore_context(previous_context);
	zend_opcache_static_cache_release_key_list(prepared_keys, key_count);
}

ZEND_FUNCTION(OPcache_pinned_clear)
{
	zend_opcache_static_cache_context *previous_context;

	ZEND_PARSE_PARAMETERS_NONE();

	previous_context = zend_opcache_static_cache_activate_context(&zend_opcache_static_cache_pinned_context_state);
	if (!zend_opcache_static_cache_validate_available_write()) {
		zend_opcache_static_cache_restore_context(previous_context);

		RETURN_THROWS();
	}

	if (!zend_opcache_static_cache_explicit_clear_prevalidated()) {
		zend_opcache_static_cache_restore_context(previous_context);
		zend_throw_exception_ex(zend_opcache_static_cache_exception_ce, 0, "Unable to clear the pinned cache");

		RETURN_THROWS();
	}

	zend_opcache_static_cache_mark_publish_skipped(&zend_opcache_static_cache_pinned_context_state);
	zend_opcache_static_cache_restore_context(previous_context);
}

ZEND_FUNCTION(OPcache_pinned_atomic_increment)
{
	zend_opcache_static_cache_context *previous_context;
	zend_long step = 1, new_value;
	zend_string *key;
	bool release_entry_lock = false;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(key)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(step)
	ZEND_PARSE_PARAMETERS_END();

	if (!zend_opcache_static_cache_validate_key(key, 1)) {
		RETURN_THROWS();
	}

	previous_context = zend_opcache_static_cache_activate_context(&zend_opcache_static_cache_pinned_context_state);
	if (!zend_opcache_static_cache_begin_entry_mutation(key, &release_entry_lock)) {
		zend_opcache_static_cache_restore_context(previous_context);

		RETURN_THROWS();
	}

	if (!zend_opcache_static_cache_require_available_write()) {
		if (release_entry_lock) {
			zend_opcache_static_cache_release_entry_lock(key);
		}
		zend_opcache_static_cache_restore_context(previous_context);

		RETURN_THROWS();
	}

	if (!zend_opcache_static_cache_atomic_update_locked(key, step, false, true, &new_value, "Atomic increment requires an integer value")) {
		zend_opcache_static_cache_unlock();
		if (release_entry_lock) {
			zend_opcache_static_cache_release_entry_lock(key);
		}
		zend_opcache_static_cache_restore_context(previous_context);

		RETURN_THROWS();
	}

	RETVAL_LONG(new_value);
	zend_opcache_static_cache_unlock();
	zend_opcache_static_cache_finish_entry_mutation(key, release_entry_lock, true);
	zend_opcache_static_cache_restore_context(previous_context);
}

ZEND_FUNCTION(OPcache_pinned_atomic_decrement)
{
	zend_opcache_static_cache_context *previous_context;
	zend_string *key;
	zend_long step = 1, new_value;
	bool release_entry_lock = false;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(key)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(step)
	ZEND_PARSE_PARAMETERS_END();

	if (!zend_opcache_static_cache_validate_key(key, 1)) {
		RETURN_THROWS();
	}

	previous_context = zend_opcache_static_cache_activate_context(&zend_opcache_static_cache_pinned_context_state);
	if (!zend_opcache_static_cache_begin_entry_mutation(key, &release_entry_lock)) {
		zend_opcache_static_cache_restore_context(previous_context);

		RETURN_THROWS();
	}

	if (!zend_opcache_static_cache_require_available_write()) {
		if (release_entry_lock) {
			zend_opcache_static_cache_release_entry_lock(key);
		}
		zend_opcache_static_cache_restore_context(previous_context);

		RETURN_THROWS();
	}

	if (!zend_opcache_static_cache_atomic_update_locked(key, step, true, true, &new_value, "Atomic decrement requires an integer value")) {
		zend_opcache_static_cache_unlock();
		if (release_entry_lock) {
			zend_opcache_static_cache_release_entry_lock(key);
		}
		zend_opcache_static_cache_restore_context(previous_context);

		RETURN_THROWS();
	}

	RETVAL_LONG(new_value);
	zend_opcache_static_cache_unlock();
	zend_opcache_static_cache_finish_entry_mutation(key, release_entry_lock, true);
	zend_opcache_static_cache_restore_context(previous_context);
}

ZEND_FUNCTION(OPcache_pinned_cache_info)
{
	zend_opcache_static_cache_context *previous_context;

	ZEND_PARSE_PARAMETERS_NONE();

	if (!zend_opcache_validate_api_restriction()) {
		zend_throw_exception_ex(zend_opcache_static_cache_exception_ce, 0, ACCELERATOR_PRODUCT_NAME " API access is restricted");

		RETURN_THROWS();
	}

	previous_context = zend_opcache_static_cache_activate_context(&zend_opcache_static_cache_pinned_context_state);
	zend_opcache_static_cache_populate_info(return_value);
	zend_opcache_static_cache_restore_context(previous_context);
}
