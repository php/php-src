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

#include "ext/standard/php_var.h"

#include "SAPI.h"

#include "zend_static_cache_internal.h"

#include "opcache_arginfo.h"

#define ZEND_OPCACHE_STATIC_CACHE_API_VALUE_TYPE "object|array|string|int|float|bool|null"

typedef struct _zend_opcache_static_cache_object {
	zend_string *pool_name;
	zend_string *pool_prefix;
	zend_opcache_static_cache_context *context;
	zend_object std;
} zend_opcache_static_cache_object;

zend_class_entry *zend_opcache_static_cache_exception_ce;
zend_class_entry *zend_opcache_static_cache_strategy_ce;
zend_class_entry *zend_opcache_static_cache_info_ce;

static zend_class_entry *zend_opcache_static_cache_interface_ce;
static zend_class_entry *zend_opcache_static_cache_volatile_cache_ce;
static zend_class_entry *zend_opcache_static_cache_stable_cache_ce;
static zend_class_entry *zend_opcache_static_cache_stable_attribute_ce;
static zend_class_entry *zend_opcache_static_cache_volatile_static_attribute_ce;
static zend_object_handlers zend_opcache_static_cache_object_handlers;
ZEND_EXT_TLS HashTable *zend_opcache_static_cache_pool_instances = NULL;

static void zend_opcache_static_cache_disable_subsystem(const char *failure_reason);

ZEND_METHOD(OPcache_StaticCacheInfo, __construct)
{
}

zend_opcache_static_cache_context zend_opcache_static_cache_volatile_context_state = {
	{0}, {0}, "volatile cache", "opcache_volatile_cache_lock",
#ifndef ZEND_WIN32
	ZEND_OPCACHE_STATIC_CACHE_VOLATILE_SEM_FILENAME_PREFIX,
#endif
	true, false, false
};

zend_opcache_static_cache_context zend_opcache_static_cache_stable_context_state = {
	{0}, {0}, "stable cache", "opcache_stable_cache_lock",
#ifndef ZEND_WIN32
	ZEND_OPCACHE_STATIC_CACHE_STABLE_SEM_FILENAME_PREFIX,
#endif
	false, true, true
};

bool zend_opcache_static_cache_subsystem_disabled = false;
const char *zend_opcache_static_cache_subsystem_failure_reason = NULL;
/* Static Cache is available only after a SAPI opts in before request handling.
 * The opt-in declares a trust/storage boundary that holds for the lifetime of
 * the shared-memory owner, such as an FPM pool partition or a single-runtime
 * CLI/phpdbg execution. */
bool zend_opcache_static_cache_runtime_opted_in = false;
zend_opcache_static_cache_partition *zend_opcache_static_cache_partitions = NULL;
ZEND_EXT_TLS zend_opcache_static_cache_partition *zend_opcache_static_cache_active_partition = NULL;

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
ZEND_EXT_TLS zend_opcache_static_cache_runtime zend_opcache_static_cache_stable_runtime_state = {0};
ZEND_EXT_TLS zend_opcache_static_cache_context *zend_opcache_static_cache_active_context_ptr = NULL;
ZEND_EXT_TLS HashTable *zend_opcache_static_cache_tracked_roots = NULL;
ZEND_EXT_TLS HashTable *zend_opcache_static_cache_tracked_references = NULL;
ZEND_EXT_TLS HashTable *zend_opcache_static_cache_tracked_arrays = NULL;
ZEND_EXT_TLS HashTable *zend_opcache_static_cache_tracked_objects = NULL;
ZEND_EXT_TLS bool zend_opcache_static_cache_has_tracked_arrays = false;
ZEND_EXT_TLS bool zend_opcache_static_cache_has_tracked_objects = false;
ZEND_EXT_TLS bool zend_opcache_static_cache_volatile_skip_publish = false;
ZEND_EXT_TLS bool zend_opcache_static_cache_stable_skip_publish = false;
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
ZEND_EXT_TLS zend_opcache_static_cache_lookup_entry zend_opcache_static_cache_stable_lookup_entry_storage[ZEND_OPCACHE_STATIC_CACHE_LOOKUP_BUCKETS];
ZEND_EXT_TLS HashTable *zend_opcache_static_cache_volatile_request_local_slots = NULL;
ZEND_EXT_TLS HashTable *zend_opcache_static_cache_stable_request_local_slots = NULL;
ZEND_EXT_TLS HashTable *zend_opcache_static_cache_volatile_entry_locks = NULL;
ZEND_EXT_TLS HashTable *zend_opcache_static_cache_stable_entry_locks = NULL;
ZEND_EXT_TLS uint32_t zend_opcache_static_cache_volatile_entry_lock_counts[ZEND_OPCACHE_STATIC_CACHE_ENTRY_LOCK_STRIPES];
ZEND_EXT_TLS uint32_t zend_opcache_static_cache_stable_entry_lock_counts[ZEND_OPCACHE_STATIC_CACHE_ENTRY_LOCK_STRIPES];

static zend_always_inline bool zend_opcache_static_cache_key_has_reserved_class_prefix(zend_string *key)
{
	return zend_string_starts_with_literal(key, "stable_static_class:") ||
		zend_string_starts_with_literal(key, "volatile_static_class:")
	;
}

static zend_always_inline bool zend_opcache_static_cache_is_fpm(void)
{
	return sapi_module.name != NULL && strcmp(sapi_module.name, "fpm-fcgi") == 0;
}

static zend_always_inline bool zend_opcache_static_cache_key_is_valid_user_key(zend_string *key)
{
	return ZSTR_LEN(key) != 0 &&
		memchr(ZSTR_VAL(key), ':', ZSTR_LEN(key)) == NULL &&
		!zend_opcache_static_cache_key_has_reserved_class_prefix(key) &&
		zend_lookup_class_ex(key, NULL, ZEND_FETCH_CLASS_NO_AUTOLOAD) == NULL
	;
}

static zend_always_inline bool zend_opcache_static_cache_validate_key(zend_string *key, uint32_t arg_num)
{
	if (ZSTR_LEN(key) == 0) {
		zend_argument_value_error(arg_num, "must be a non-empty string");

		return false;
	}

	if (zend_opcache_static_cache_key_has_reserved_class_prefix(key)) {
		zend_argument_value_error(arg_num, "must not start with a reserved static-cache class key prefix");

		return false;
	}

	if (memchr(ZSTR_VAL(key), ':', ZSTR_LEN(key)) != NULL) {
		zend_argument_value_error(arg_num, "must not contain the static-cache key delimiter \":\"");

		return false;
	}

	if (zend_lookup_class_ex(key, NULL, ZEND_FETCH_CLASS_NO_AUTOLOAD) != NULL) {
		zend_argument_value_error(arg_num, "must not be a loaded class name");

		return false;
	}

	return true;
}

static zend_always_inline bool zend_opcache_static_cache_validate_store_array_value(zval *value, uint32_t arg_num)
{
	ZVAL_DEREF(value);

	if (Z_TYPE_P(value) == IS_RESOURCE) {
		zend_argument_type_error(arg_num, "must contain only values of type " ZEND_OPCACHE_STATIC_CACHE_API_VALUE_TYPE ", resource given");

		return false;
	}

	if (Z_TYPE_P(value) == IS_OBJECT && Z_OBJCE_P(value) == zend_ce_closure) {
		zend_argument_type_error(arg_num, "must not contain Closure objects");

		return false;
	}

	return true;
}

static zend_always_inline bool zend_opcache_static_cache_validate_store_array(HashTable *values, uint32_t arg_num)
{
	zend_string *key;
	zval *value;

	ZEND_HASH_FOREACH_STR_KEY_VAL(values, key, value) {
		if (key == NULL || !zend_opcache_static_cache_key_is_valid_user_key(key)) {
			zend_argument_value_error(arg_num, "must be an array with non-empty string keys that do not contain \":\" and are not reserved static-cache class keys or loaded class names");

			return false;
		}

		if (!zend_opcache_static_cache_validate_store_array_value(value, arg_num)) {
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
			if (!zend_opcache_static_cache_key_is_valid_user_key(Z_STR_P(value))) {
				zend_argument_value_error(arg_num, "must contain only non-empty string or int cache keys that do not contain \":\" and are not reserved static-cache class keys or loaded class names");
				zend_opcache_static_cache_release_key_list(prepared, index);

				return false;
			}

			prepared[index++] = zend_string_copy(Z_STR_P(value));
		} else if (Z_TYPE_P(value) == IS_LONG) {
			prepared[index++] = zend_long_to_str(Z_LVAL_P(value));
		} else {
			zend_argument_value_error(arg_num, "must contain only non-empty string or int cache keys that do not contain \":\" and are not reserved static-cache class keys or loaded class names");
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

static zend_always_inline zend_string *zend_opcache_static_cache_build_pool_prefix(
		bool is_stable,
		zend_string *pool_name)
{
	const char *prefix;
	size_t prefix_len;

	if (is_stable) {
		prefix = "stable_cache:";
		prefix_len = sizeof("stable_cache:") - 1;
	} else {
		prefix = "volatile_cache:";
		prefix_len = sizeof("volatile_cache:") - 1;
	}

	return zend_string_concat3(
		prefix,
		prefix_len,
		ZSTR_VAL(pool_name),
		ZSTR_LEN(pool_name),
		ZEND_STRL(":")
	);
}

static zend_always_inline zend_string *zend_opcache_static_cache_build_storage_key(
		const zend_opcache_static_cache_object *cache,
		zend_string *key)
{
	return zend_string_concat2(
		ZSTR_VAL(cache->pool_prefix),
		ZSTR_LEN(cache->pool_prefix),
		ZSTR_VAL(key),
		ZSTR_LEN(key)
	);
}

static zend_always_inline zend_opcache_static_cache_object *zend_opcache_static_cache_object_from_obj(
	zend_object *object)
{
	return (zend_opcache_static_cache_object *) ((char *) object - offsetof(zend_opcache_static_cache_object, std));
}

static void zend_opcache_static_cache_object_free(zend_object *object)
{
	zend_opcache_static_cache_object *cache = zend_opcache_static_cache_object_from_obj(object);

	if (cache->pool_name != NULL) {
		zend_string_release(cache->pool_name);
	}

	if (cache->pool_prefix != NULL) {
		zend_string_release(cache->pool_prefix);
	}

	zend_object_std_dtor(&cache->std);
}

static zend_object *zend_opcache_static_cache_object_create(zend_class_entry *ce)
{
	zend_opcache_static_cache_object *cache;

	cache = zend_object_alloc(sizeof(zend_opcache_static_cache_object), ce);

	zend_object_std_init(&cache->std, ce);
	object_properties_init(&cache->std, ce);

	cache->pool_name = NULL;
	cache->pool_prefix = NULL;
	cache->context = ce == zend_opcache_static_cache_stable_cache_ce
		? zend_opcache_static_cache_active_stable_context()
		: zend_opcache_static_cache_active_volatile_context()
	;
	cache->std.handlers = &zend_opcache_static_cache_object_handlers;

	return &cache->std;
}

static zend_always_inline zend_opcache_static_cache_object *zend_opcache_static_cache_from_this(zval *this_ptr)
{
	zend_opcache_static_cache_object *cache = zend_opcache_static_cache_object_from_obj(Z_OBJ_P(this_ptr));

	if (cache->pool_prefix == NULL) {
		zend_throw_error(NULL, "OPcache static cache instances must be obtained through getInstance()");

		return NULL;
	}

	return cache;
}

static zend_always_inline zend_string *zend_opcache_static_cache_build_instance_key(
		bool is_stable,
		zend_string *pool_name)
{
	const char *prefix;
	size_t prefix_len;

	if (is_stable) {
		prefix = "s:";
		prefix_len = sizeof("s:") - 1;
	} else {
		prefix = "v:";
		prefix_len = sizeof("v:") - 1;
	}

	return zend_string_concat2(
		prefix,
		prefix_len,
		ZSTR_VAL(pool_name),
		ZSTR_LEN(pool_name)
	);
}

static HashTable *zend_opcache_static_cache_instances(void)
{
	if (zend_opcache_static_cache_pool_instances == NULL) {
		ALLOC_HASHTABLE(zend_opcache_static_cache_pool_instances);
		zend_hash_init(zend_opcache_static_cache_pool_instances, 0, NULL, ZVAL_PTR_DTOR, 0);
	}

	return zend_opcache_static_cache_pool_instances;
}

static void zend_opcache_static_cache_release_instances(void)
{
	if (zend_opcache_static_cache_pool_instances == NULL) {
		return;
	}

	zend_hash_destroy(zend_opcache_static_cache_pool_instances);
	FREE_HASHTABLE(zend_opcache_static_cache_pool_instances);

	zend_opcache_static_cache_pool_instances = NULL;
}

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
		handlers == NULL ||
		handlers->copy == NULL ||
		handlers->state_serialize == NULL ||
		handlers->state_unserialize == NULL
	) {
		return;
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

zend_opcache_static_cache_safe_direct_state_copy_func_t zend_opcache_static_cache_safe_direct_copy_func(
		zend_class_entry *ce,
		zend_class_entry **base_ce_ptr)
{
	const zend_opcache_static_cache_safe_direct_handlers *handlers =
		zend_opcache_static_cache_safe_direct_find_handlers(ce, base_ce_ptr)
	;

	return handlers != NULL ? handlers->copy : NULL;
}

zend_opcache_static_cache_safe_direct_state_has_unstorable_func_t zend_opcache_static_cache_safe_direct_state_has_unstorable_func(
		zend_class_entry *ce)
{
	const zend_opcache_static_cache_safe_direct_handlers *handlers =
		zend_opcache_static_cache_safe_direct_find_handlers(ce, NULL)
	;

	return handlers != NULL ? handlers->state_has_unstorable : NULL;
}

zend_opcache_static_cache_safe_direct_state_serialize_func_t zend_opcache_static_cache_safe_direct_state_serialize_func(
		zend_class_entry *ce)
{
	const zend_opcache_static_cache_safe_direct_handlers *handlers =
		zend_opcache_static_cache_safe_direct_find_handlers(ce, NULL)
	;

	return handlers != NULL ? handlers->state_serialize : NULL;
}

zend_opcache_static_cache_safe_direct_state_unserialize_func_t zend_opcache_static_cache_safe_direct_state_unserialize_func(
		zend_class_entry *ce)
{
	const zend_opcache_static_cache_safe_direct_handlers *handlers =
		zend_opcache_static_cache_safe_direct_find_handlers(ce, NULL)
	;

	return handlers != NULL ? handlers->state_unserialize : NULL;
}

bool zend_opcache_static_cache_safe_direct_allows_custom_serializers(zend_class_entry *ce)
{
	const zend_opcache_static_cache_safe_direct_handlers *handlers =
		zend_opcache_static_cache_safe_direct_find_handlers(ce, NULL)
	;

	return handlers != NULL && handlers->allows_custom_serializers;
}

bool zend_opcache_static_cache_safe_direct_state_includes_properties(zend_class_entry *ce)
{
	const zend_opcache_static_cache_safe_direct_handlers *handlers =
		zend_opcache_static_cache_safe_direct_find_handlers(ce, NULL)
	;

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

static zend_always_inline void zend_opcache_static_cache_register_classes(void)
{
	zend_internal_attribute *attribute;

	if (zend_opcache_static_cache_stable_attribute_ce != NULL) {
		return;
	}

	zend_opcache_static_cache_info_ce = register_class_OPcache_StaticCacheInfo();
	zend_opcache_static_cache_stable_attribute_ce = register_class_OPcache_StableStatic();

	zend_mark_internal_attribute(zend_opcache_static_cache_stable_attribute_ce);

	zend_opcache_static_cache_strategy_ce = register_class_OPcache_CacheStrategy();
	zend_opcache_static_cache_volatile_static_attribute_ce = register_class_OPcache_VolatileStatic();
	attribute = zend_mark_internal_attribute(zend_opcache_static_cache_volatile_static_attribute_ce);
	attribute->validator = zend_opcache_static_cache_validate_volatile_static_attribute;
	zend_opcache_static_cache_exception_ce = register_class_OPcache_StaticCacheException(zend_ce_exception);

	zend_opcache_static_cache_interface_ce = register_class_OPcache_StaticCacheInterface();
	zend_opcache_static_cache_volatile_cache_ce = register_class_OPcache_VolatileCache(
		zend_opcache_static_cache_interface_ce
	);
	zend_opcache_static_cache_stable_cache_ce = register_class_OPcache_StableCache(
		zend_opcache_static_cache_interface_ce
	);

	zend_opcache_static_cache_volatile_cache_ce->create_object = zend_opcache_static_cache_object_create;
	zend_opcache_static_cache_stable_cache_ce->create_object = zend_opcache_static_cache_object_create;

	memcpy(
		&zend_opcache_static_cache_object_handlers,
		zend_get_std_object_handlers(),
		sizeof(zend_object_handlers)
	);

	zend_opcache_static_cache_object_handlers.offset = offsetof(zend_opcache_static_cache_object, std);
	zend_opcache_static_cache_object_handlers.free_obj = zend_opcache_static_cache_object_free;
	zend_opcache_static_cache_object_handlers.clone_obj = NULL;
}

static zend_always_inline void zend_opcache_static_cache_reset_class_entries(void)
{
	zend_opcache_static_cache_exception_ce = NULL;
	zend_opcache_static_cache_strategy_ce = NULL;
	zend_opcache_static_cache_info_ce = NULL;
	zend_opcache_static_cache_interface_ce = NULL;
	zend_opcache_static_cache_volatile_cache_ce = NULL;
	zend_opcache_static_cache_stable_cache_ce = NULL;
	zend_opcache_static_cache_stable_attribute_ce = NULL;
	zend_opcache_static_cache_volatile_static_attribute_ce = NULL;
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
	if (!zend_opcache_validate_api_restriction()) {
		return false;
	}

	if (!zend_opcache_static_cache_active_runtime()->available) {
		return false;
	}

	if (!zend_opcache_static_cache_rlock()) {
		return false;
	}

	if (!zend_opcache_static_cache_header_is_initialized_locked()) {
		zend_opcache_static_cache_unlock();

		return false;
	}

	return true;
}

static zend_always_inline bool zend_opcache_static_cache_validate_available_write(void)
{
	if (!zend_opcache_validate_api_restriction()) {
		return false;
	}

	if (!zend_opcache_static_cache_active_runtime()->available) {
		return false;
	}

	return true;
}

static zend_always_inline bool zend_opcache_static_cache_acquire_write_lock(void)
{
	if (!zend_opcache_static_cache_wlock()) {
		return false;
	}

	if (!zend_opcache_static_cache_header_init_locked()) {
		zend_opcache_static_cache_unlock();

		return false;
	}

	return true;
}

static zend_always_inline void zend_opcache_static_cache_warn_atomic_overflow(const char *function_name)
{
	zend_error(E_WARNING, "%s(): Integer overflow occurred; result wrapped around", function_name);
}

static zend_always_inline void zend_opcache_static_cache_throw_atomic_failure(const char *function_name, zend_string *key)
{
	zend_throw_exception_ex(
		zend_opcache_static_cache_exception_ce,
		0,
		"%s(): Unable to update stable cache integer value for key \"%s\"",
		function_name,
		ZSTR_VAL(key)
	);
}

static void zend_opcache_static_cache_init_partition_contexts(zend_opcache_static_cache_partition *partition, const char *name)
{
	partition->volatile_context = zend_opcache_static_cache_volatile_context_state;
	partition->stable_context = zend_opcache_static_cache_stable_context_state;
	partition->volatile_context.storage.lock_file = -1;
	partition->stable_context.storage.lock_file = -1;
	partition->name = name != NULL ? strdup(name) : NULL;
}

ZEND_API zend_opcache_static_cache_partition *zend_opcache_static_cache_partition_create(const char *name)
{
	zend_opcache_static_cache_partition *partition = calloc(1, sizeof(zend_opcache_static_cache_partition));

	if (partition == NULL) {
		return NULL;
	}

	zend_opcache_static_cache_init_partition_contexts(partition, name);

	partition->next = zend_opcache_static_cache_partitions;
	zend_opcache_static_cache_partitions = partition;

	return partition;
}

ZEND_API bool zend_opcache_static_cache_partition_startup_before_request(zend_opcache_static_cache_partition *partition)
{
	const char *failure_reason;
	zend_opcache_static_cache_context *previous_context;
	zend_opcache_static_cache_partition *previous_partition;

	if (partition == NULL) {
		return true;
	}

	previous_partition = zend_opcache_static_cache_active_partition;
	zend_opcache_static_cache_active_partition = partition;
	previous_context = zend_opcache_static_cache_activate_context(&partition->volatile_context);

	zend_opcache_static_cache_reset_runtime();
	if (zend_opcache_static_cache_active_runtime()->enabled && ZCG(enabled) && accel_startup_ok && !file_cache_only) {
		if (!zend_opcache_static_cache_startup_storage_before_request()) {
			failure_reason = zend_opcache_static_cache_active_runtime()->failure_reason;

			zend_opcache_static_cache_disable_subsystem(failure_reason);
			zend_opcache_static_cache_restore_context(previous_context);

			zend_opcache_static_cache_active_partition = previous_partition;

			return false;
		}
	}

	zend_opcache_static_cache_activate_context(&partition->stable_context);
	zend_opcache_static_cache_reset_runtime();
	if (zend_opcache_static_cache_active_runtime()->enabled && ZCG(enabled) && accel_startup_ok && !file_cache_only) {
		if (!zend_opcache_static_cache_startup_storage_before_request()) {
			failure_reason = zend_opcache_static_cache_active_runtime()->failure_reason;

			zend_opcache_static_cache_disable_subsystem(failure_reason);
			zend_opcache_static_cache_restore_context(previous_context);

			zend_opcache_static_cache_active_partition = previous_partition;

			return false;
		}
	}

	zend_opcache_static_cache_restore_context(previous_context);

	zend_opcache_static_cache_active_partition = previous_partition;

	return true;
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

	return true;
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
		zend_opcache_static_cache_release_active_request_local_slots();
	}

	return cleared;
}

static zend_always_inline bool zend_opcache_static_cache_explicit_clear_pool_prevalidated(
		zend_string *pool_prefix)
{
	zend_opcache_static_cache_header *header;
	zend_opcache_static_cache_entry *entries, *entry;
	zend_string *storage_key;
	uint32_t index;

	if (!zend_opcache_static_cache_acquire_write_lock()) {
		return false;
	}

	header = zend_opcache_static_cache_header_ptr();
	entries = zend_opcache_static_cache_entries(header);
	for (index = 0; index < header->capacity; index++) {
		entry = &entries[index];
		if (entry->state != ZEND_OPCACHE_STATIC_CACHE_ENTRY_USED ||
			entry->key_len < ZSTR_LEN(pool_prefix) ||
			memcmp(zend_opcache_static_cache_ptr(entry->key_offset), ZSTR_VAL(pool_prefix), ZSTR_LEN(pool_prefix)) != 0
		) {
			continue;
		}

		storage_key = zend_string_init(
			zend_opcache_static_cache_ptr(entry->key_offset),
			entry->key_len,
			0
		);

		zend_opcache_static_cache_delete_locked(storage_key);
		zend_string_release(storage_key);
	}

	zend_opcache_static_cache_unlock();
	zend_opcache_static_cache_release_active_entry_locks_by_prefix(pool_prefix);
	zend_opcache_static_cache_release_active_request_local_slots_by_prefix(pool_prefix);
	zend_opcache_static_cache_mark_publish_skipped(zend_opcache_static_cache_active_context());

	return true;
}

static zend_always_inline bool zend_opcache_static_cache_explicit_store_prevalidated(zend_string *key, zval *value, zend_long ttl)
{
	zend_opcache_static_cache_prepared_value prepared;
	uint64_t generation = 0;
	bool stored, release_entry_lock = false, seed_request_local_slot = false;

	if (!zend_opcache_static_cache_prepare_value(&prepared, key, value, false, false, false)) {
		zend_opcache_static_cache_destroy_prepared_value(&prepared);

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

	stored = zend_opcache_static_cache_store_prepared_locked(
		key,
		value,
		&prepared,
		ttl,
		false,
		false,
		&generation,
		&seed_request_local_slot
	);

	zend_opcache_static_cache_unlock();
	zend_opcache_static_cache_destroy_prepared_value(&prepared);
	zend_opcache_static_cache_finish_entry_mutation(key, release_entry_lock, stored);

	if (stored && seed_request_local_slot) {
		zend_opcache_static_cache_store_request_local_value_slot(key, generation, value);
	}

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

		return FAILURE;
	}

	return SUCCESS;
}

static zend_always_inline zend_result zend_opcache_static_cache_fetch_array_pool_api(
		const zend_opcache_static_cache_object *cache,
		HashTable *keys,
		zval *default_value,
		zval *return_value)
{
	zend_opcache_static_cache_context *previous_context;
	zend_string **prepared_keys, *key, *storage_key;
	zval fetched_value;
	uint32_t key_count, index;
	bool fetched, found;

	if (!zend_opcache_static_cache_prepare_key_list(keys, &prepared_keys, &key_count, 1)) {
		return FAILURE;
	}

	previous_context = zend_opcache_static_cache_activate_context(cache->context);
	if (!zend_opcache_static_cache_require_available_read()) {
		zend_opcache_static_cache_restore_context(previous_context);
		zend_opcache_static_cache_release_key_list(prepared_keys, key_count);

		return FAILURE;
	}

	array_init_size(return_value, key_count);

	for (index = 0; index < key_count; index++) {
		key = prepared_keys[index];
		storage_key = zend_opcache_static_cache_build_storage_key(cache, key);
		fetched = zend_opcache_static_cache_fetch_locked(storage_key, &fetched_value, false, &found, true);
		if (!fetched) {
			if (!found) {
				ZVAL_COPY(&fetched_value, default_value);
			} else {
				zval_ptr_dtor(return_value);

				ZVAL_UNDEF(return_value);

				zend_string_release(storage_key);

				zend_opcache_static_cache_unlock();
				zend_opcache_static_cache_restore_context(previous_context);
				zend_opcache_static_cache_release_key_list(prepared_keys, key_count);

				return FAILURE;
			}
		}

		zend_hash_update(Z_ARRVAL_P(return_value), key, &fetched_value);

		zend_string_release(storage_key);
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

	previous_context = zend_opcache_static_cache_activate_context(zend_opcache_static_cache_active_volatile_context());

	zend_opcache_static_cache_shutdown_storage();
	zend_opcache_static_cache_reset_runtime();
	zend_opcache_static_cache_activate_context(zend_opcache_static_cache_active_stable_context());
	zend_opcache_static_cache_shutdown_storage();
	zend_opcache_static_cache_reset_runtime();
	zend_opcache_static_cache_restore_context(previous_context);
}

static void zend_opcache_static_cache_startup(void)
{
	const char *failure_reason;
	zend_opcache_static_cache_context *previous_context;

	if (zend_opcache_static_cache_is_fpm()) {
		return;
	}

	previous_context = zend_opcache_static_cache_activate_context(zend_opcache_static_cache_active_volatile_context());

	zend_opcache_static_cache_reset_runtime();

	if (zend_opcache_static_cache_active_runtime()->enabled && ZCG(enabled) && accel_startup_ok && !file_cache_only) {
		if (!zend_opcache_static_cache_startup_storage_before_request()) {
			failure_reason = zend_opcache_static_cache_active_runtime()->failure_reason;
			zend_opcache_static_cache_restore_context(previous_context);
			zend_opcache_static_cache_disable_subsystem(failure_reason);

			return;
		}
	}

	zend_opcache_static_cache_activate_context(zend_opcache_static_cache_active_stable_context());
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
			ZCG(accel_directives).static_cache_stable_size_mb != 0
		)
	) {
		zend_opcache_static_cache_register_hooks();
	}
}

static void zend_opcache_static_cache_post_startup(void)
{
	const char *failure_reason;
	zend_opcache_static_cache_context *previous_context;

	if (zend_opcache_static_cache_is_fpm()) {
		return;
	}

	previous_context = zend_opcache_static_cache_activate_context(zend_opcache_static_cache_active_volatile_context());

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

	zend_opcache_static_cache_activate_context(zend_opcache_static_cache_active_stable_context());
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
}

static void zend_opcache_static_cache_partition_shutdown_all(void)
{
	zend_opcache_static_cache_partition *partition = zend_opcache_static_cache_partitions, *next;
	zend_opcache_static_cache_context *previous_context;

	previous_context = zend_opcache_static_cache_active_context_ptr;
	while (partition != NULL) {
		next = partition->next;

		zend_opcache_static_cache_activate_context(&partition->volatile_context);
		zend_opcache_static_cache_shutdown_storage();
		zend_opcache_static_cache_reset_runtime();
		zend_opcache_static_cache_activate_context(&partition->stable_context);
		zend_opcache_static_cache_shutdown_storage();
		zend_opcache_static_cache_reset_runtime();

		free(partition->name);
		free(partition);

		partition = next;
	}

	zend_opcache_static_cache_partitions = NULL;

	zend_opcache_static_cache_restore_context(previous_context);
}

static zend_result zend_opcache_static_cache_rinit(void)
{
	zend_opcache_static_cache_context *previous_context;
	bool static_cache_available;

	previous_context = zend_opcache_static_cache_activate_context(zend_opcache_static_cache_active_volatile_context());

	zend_opcache_static_cache_ensure_ready();
	zend_opcache_static_cache_activate_context(zend_opcache_static_cache_active_stable_context());
	zend_opcache_static_cache_ensure_ready();
	zend_opcache_static_cache_restore_context(previous_context);
	zend_opcache_static_cache_clear_lookup_caches();
	zend_opcache_static_cache_reset_publish_skips();

	EG(static_cache_class_access_active) = false;
	EG(tracked_mutation_hooks_active) = false;

	static_cache_available = zend_opcache_static_cache_context_runtime(zend_opcache_static_cache_active_volatile_context())->available ||
		zend_opcache_static_cache_context_runtime(zend_opcache_static_cache_active_stable_context())->available
	;
	if (!static_cache_available) {
		return SUCCESS;
	}

	zend_opcache_static_cache_register_hooks();

	EG(static_cache_class_access_active) = true;
	zend_opcache_static_cache_request_init();

	return SUCCESS;
}

static void zend_opcache_static_cache_invalidate_script(zend_persistent_script *persistent_script)
{
	zend_opcache_static_cache_invalidate_script_context(zend_opcache_static_cache_active_stable_context(), persistent_script);
	zend_opcache_static_cache_invalidate_script_context(zend_opcache_static_cache_active_volatile_context(), persistent_script);
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

ZEND_API void zend_opcache_static_cache_partition_activate(zend_opcache_static_cache_partition *partition)
{
	zend_opcache_static_cache_active_partition = partition;
	zend_opcache_static_cache_active_context_ptr = NULL;
}

ZEND_API void zend_opcache_static_cache_opt_in(void)
{
	zend_opcache_static_cache_runtime_opted_in = true;
}

zend_result zend_opcache_static_cache_minit(void)
{
	zend_opcache_static_cache_context *previous_context;

	zend_opcache_static_cache_subsystem_disabled = false;
	zend_opcache_static_cache_subsystem_failure_reason = NULL;

	zend_opcache_static_cache_register_classes();
	zend_opcache_static_cache_safe_direct_handlers_init();
	zend_opcache_static_cache_register_accelerator_handlers();

	previous_context = zend_opcache_static_cache_activate_context(zend_opcache_static_cache_active_volatile_context());

	zend_opcache_static_cache_reset_storage();
	zend_opcache_static_cache_activate_context(zend_opcache_static_cache_active_stable_context());
	zend_opcache_static_cache_reset_storage();

	zend_opcache_static_cache_restore_context(previous_context);

	return SUCCESS;
}

void zend_opcache_static_cache_mshutdown(void)
{
	zend_opcache_static_cache_context *previous_context;

	zend_opcache_static_cache_unregister_hooks();
	zend_opcache_static_cache_partition_shutdown_all();

	zend_opcache_static_cache_active_partition = NULL;
	previous_context = zend_opcache_static_cache_activate_context(zend_opcache_static_cache_active_volatile_context());

	zend_opcache_static_cache_shutdown_storage();
	zend_opcache_static_cache_reset_runtime();
	zend_opcache_static_cache_activate_context(&zend_opcache_static_cache_stable_context_state);
	zend_opcache_static_cache_shutdown_storage();
	zend_opcache_static_cache_reset_runtime();
	zend_opcache_static_cache_restore_context(previous_context);

	zend_opcache_static_cache_subsystem_disabled = false;
	zend_opcache_static_cache_subsystem_failure_reason = NULL;
	zend_opcache_static_cache_active_partition = NULL;

	zend_opcache_static_cache_safe_direct_handlers_destroy();
	zend_opcache_static_cache_reset_class_entries();
	zend_accel_register_static_cache_handlers(NULL);
}

void zend_opcache_static_cache_invalidate_all(void)
{
	zend_opcache_static_cache_invalidate_all_context(zend_opcache_static_cache_active_stable_context());
	zend_opcache_static_cache_invalidate_all_context(zend_opcache_static_cache_active_volatile_context());
}

void zend_opcache_static_cache_volatile_get_status(zval *return_value)
{
	zend_opcache_static_cache_context *previous_context = zend_opcache_static_cache_activate_context(zend_opcache_static_cache_active_volatile_context());

	zend_opcache_static_cache_populate_info(return_value);
	zend_opcache_static_cache_restore_context(previous_context);
}

void zend_opcache_static_cache_stable_get_status(zval *return_value)
{
	zend_opcache_static_cache_context *previous_context = zend_opcache_static_cache_activate_context(zend_opcache_static_cache_active_stable_context());

	zend_opcache_static_cache_populate_info(return_value);
	zend_opcache_static_cache_restore_context(previous_context);
}

bool zend_opcache_static_cache_volatile_is_enabled(void)
{
	return zend_opcache_static_cache_context_runtime(zend_opcache_static_cache_active_volatile_context())->enabled;
}

bool zend_opcache_static_cache_volatile_is_available(void)
{
	return zend_opcache_static_cache_context_runtime(zend_opcache_static_cache_active_volatile_context())->available;
}

const char *zend_opcache_static_cache_volatile_failure_reason(void)
{
	return zend_opcache_static_cache_context_runtime(zend_opcache_static_cache_active_volatile_context())->failure_reason;
}

bool zend_opcache_static_cache_stable_is_enabled(void)
{
	return zend_opcache_static_cache_context_runtime(zend_opcache_static_cache_active_stable_context())->enabled;
}

bool zend_opcache_static_cache_stable_is_available(void)
{
	return zend_opcache_static_cache_context_runtime(zend_opcache_static_cache_active_stable_context())->available;
}

const char *zend_opcache_static_cache_stable_failure_reason(void)
{
	return zend_opcache_static_cache_context_runtime(zend_opcache_static_cache_active_stable_context())->failure_reason;
}

zend_result zend_opcache_static_cache_rshutdown(void)
{
	zend_opcache_static_cache_clear_lookup_caches();

	EG(static_cache_class_access_active) = false;
	EG(tracked_mutation_hooks_active) = false;

	zend_opcache_static_cache_request_shutdown();
	zend_opcache_static_cache_release_request_entry_locks();
	zend_opcache_static_cache_release_request_local_slots();
	zend_opcache_static_cache_release_request_shared_graph_refs();
	zend_opcache_static_cache_release_instances();

	EG(static_cache_class_access_active) = false;
	EG(tracked_mutation_hooks_active) = false;

	return SUCCESS;
}

zend_result zend_opcache_register_functions(int module_type)
{
	return zend_register_functions(NULL, ext_functions, NULL, module_type);
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

static zend_always_inline void zend_opcache_static_cache_get_instance(INTERNAL_FUNCTION_PARAMETERS, bool is_stable)
{
	zend_opcache_static_cache_object *cache;
	zend_class_entry *ce = is_stable
		? zend_opcache_static_cache_stable_cache_ce
		: zend_opcache_static_cache_volatile_cache_ce
	;
	zend_string *pool_name, *instance_key;
	zval instance, *cached_instance;
	HashTable *instances;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(pool_name)
	ZEND_PARSE_PARAMETERS_END();

	if (memchr(ZSTR_VAL(pool_name), ':', ZSTR_LEN(pool_name)) != NULL) {
		zend_argument_value_error(1, "must not contain the static-cache key delimiter \":\"");

		RETURN_THROWS();
	}

	instance_key = zend_opcache_static_cache_build_instance_key(is_stable, pool_name);
	instances = zend_opcache_static_cache_instances();
	cached_instance = zend_hash_find(instances, instance_key);
	if (cached_instance != NULL) {
		zend_string_release(instance_key);

		RETURN_OBJ_COPY(Z_OBJ_P(cached_instance));
	}

	object_init_ex(&instance, ce);
	cache = zend_opcache_static_cache_object_from_obj(Z_OBJ(instance));
	cache->pool_name = zend_string_copy(pool_name);
	cache->pool_prefix = zend_opcache_static_cache_build_pool_prefix(is_stable, pool_name);
	cache->context = is_stable
		? zend_opcache_static_cache_active_stable_context()
		: zend_opcache_static_cache_active_volatile_context()
	;

	cached_instance = zend_hash_update(instances, instance_key, &instance);

	zend_string_release(instance_key);

	if (cached_instance == NULL) {
		zval_ptr_dtor(&instance);

		zend_throw_error(NULL, "Unable to register OPcache static cache instance");

		RETURN_THROWS();
	}

	RETURN_OBJ_COPY(Z_OBJ_P(cached_instance));
}

static zend_always_inline bool zend_opcache_static_cache_instance_store(
		zend_opcache_static_cache_object *cache,
		zend_string *key,
		zval *value,
		zend_long ttl)
{
	zend_opcache_static_cache_context *previous_context;
	zend_string *storage_key;
	bool stored;

	previous_context = zend_opcache_static_cache_activate_context(cache->context);
	if (!zend_opcache_static_cache_validate_available_write()) {
		zend_opcache_static_cache_restore_context(previous_context);

		return false;
	}

	storage_key = zend_opcache_static_cache_build_storage_key(cache, key);
	stored = zend_opcache_static_cache_explicit_store_prevalidated(storage_key, value, ttl);

	zend_string_release(storage_key);

	zend_opcache_static_cache_restore_context(previous_context);

	return stored;
}

static zend_always_inline bool zend_opcache_static_cache_instance_store_multiple(
		zend_opcache_static_cache_object *cache,
		HashTable *values,
		zend_long ttl)
{
	zend_opcache_static_cache_context *previous_context;
	zend_string *key, *storage_key;
	zval *value;
	bool stored;

	previous_context = zend_opcache_static_cache_activate_context(cache->context);
	if (!zend_opcache_static_cache_validate_available_write()) {
		zend_opcache_static_cache_restore_context(previous_context);

		return false;
	}

	ZEND_HASH_FOREACH_STR_KEY_VAL(values, key, value) {
		ZEND_ASSERT(key != NULL);

		storage_key = zend_opcache_static_cache_build_storage_key(cache, key);
		stored = zend_opcache_static_cache_explicit_store_prevalidated(storage_key, value, ttl);

		zend_string_release(storage_key);

		if (!stored) {
			zend_opcache_static_cache_restore_context(previous_context);

			return false;
		}
	} ZEND_HASH_FOREACH_END();

	zend_opcache_static_cache_restore_context(previous_context);

	return true;
}

static zend_always_inline void zend_opcache_static_cache_return_info(
		zval *return_value,
		zend_opcache_static_cache_context *context)
{
	zend_opcache_static_cache_context *previous_context;

	if (!zend_opcache_validate_api_restriction()) {
		zend_throw_exception_ex(zend_opcache_static_cache_exception_ce, 0, ACCELERATOR_PRODUCT_NAME " API access is restricted");

		return;
	}

	previous_context = zend_opcache_static_cache_activate_context(context);

	zend_opcache_static_cache_populate_info(return_value);
	zend_opcache_static_cache_restore_context(previous_context);
}

PHP_FUNCTION(opcache_static_cache_volatile_reset)
{
	zend_opcache_static_cache_context *previous_context;

	ZEND_PARSE_PARAMETERS_NONE();

	previous_context = zend_opcache_static_cache_activate_context(zend_opcache_static_cache_active_volatile_context());
	if (!zend_opcache_static_cache_validate_available_write()) {
		zend_opcache_static_cache_restore_context(previous_context);

		if (EG(exception)) {
			RETURN_THROWS();
		}

		RETURN_FALSE;
	}

	if (!zend_opcache_static_cache_explicit_clear_prevalidated()) {
		zend_opcache_static_cache_restore_context(previous_context);

		if (EG(exception)) {
			RETURN_THROWS();
		}

		RETURN_FALSE;
	}

	zend_opcache_static_cache_restore_context(previous_context);

	RETURN_TRUE;
}

ZEND_METHOD(OPcache_VolatileCache, __construct)
{
	ZEND_PARSE_PARAMETERS_NONE();
}

ZEND_METHOD(OPcache_VolatileCache, getInstance)
{
	zend_opcache_static_cache_get_instance(INTERNAL_FUNCTION_PARAM_PASSTHRU, false);
}

ZEND_METHOD(OPcache_VolatileCache, store)
{
	zend_opcache_static_cache_object *cache;
	zend_string *key;
	zval *value;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_STR(key)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	cache = zend_opcache_static_cache_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	if (!zend_opcache_static_cache_validate_key(key, 1)) {
		RETURN_THROWS();
	}

	if (!zend_opcache_static_cache_validate_api_value(value, 2)) {
		RETURN_THROWS();
	}

	if (!zend_opcache_static_cache_instance_store(cache, key, value, 0)) {
		if (EG(exception)) {
			RETURN_THROWS();
		}

		RETURN_FALSE;
	}

	RETURN_TRUE;
}

ZEND_METHOD(OPcache_VolatileCache, storeMultiple)
{
	zend_opcache_static_cache_object *cache;
	HashTable *values;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(values)
	ZEND_PARSE_PARAMETERS_END();

	cache = zend_opcache_static_cache_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	if (!zend_opcache_static_cache_validate_store_array(values, 1)) {
		RETURN_THROWS();
	}

	if (!zend_opcache_static_cache_instance_store_multiple(cache, values, 0)) {
		if (EG(exception)) {
			RETURN_THROWS();
		}

		RETURN_FALSE;
	}

	RETURN_TRUE;
}

ZEND_METHOD(OPcache_VolatileCache, fetch)
{
	zend_opcache_static_cache_object *cache;
	zend_string *key, *storage_key;
	zval *default_value = NULL, default_null;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(key)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(default_value)
	ZEND_PARSE_PARAMETERS_END();

	cache = zend_opcache_static_cache_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

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

	storage_key = zend_opcache_static_cache_build_storage_key(cache, key);
	if (zend_opcache_static_cache_fetch_api(cache->context, storage_key, default_value, return_value) == FAILURE) {
		zend_string_release(storage_key);

		if (EG(exception)) {
			RETURN_THROWS();
		}

		RETURN_NULL();
	}

	zend_string_release(storage_key);
}

ZEND_METHOD(OPcache_VolatileCache, fetchMultiple)
{
	zend_opcache_static_cache_object *cache;
	zval *default_value = NULL, default_null;
	HashTable *keys;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ARRAY_HT(keys)
		Z_PARAM_OPTIONAL
		Z_PARAM_ARRAY_OR_NULL(default_value)
	ZEND_PARSE_PARAMETERS_END();

	cache = zend_opcache_static_cache_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	if (default_value == NULL) {
		ZVAL_NULL(&default_null);
		default_value = &default_null;
	}

	if (zend_opcache_static_cache_fetch_array_pool_api(cache, keys, default_value, return_value) == FAILURE) {
		if (EG(exception)) {
			RETURN_THROWS();
		}

		RETURN_NULL();
	}
}

ZEND_METHOD(OPcache_VolatileCache, has)
{
	zend_opcache_static_cache_object *cache;
	zend_string *key, *storage_key;
	bool exists;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(key)
	ZEND_PARSE_PARAMETERS_END();

	cache = zend_opcache_static_cache_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	if (!zend_opcache_static_cache_validate_key(key, 1)) {
		RETURN_THROWS();
	}

	storage_key = zend_opcache_static_cache_build_storage_key(cache, key);
	if (zend_opcache_static_cache_exists_api(cache->context, storage_key, &exists) == FAILURE) {
		zend_string_release(storage_key);

		if (EG(exception)) {
			RETURN_THROWS();
		}

		RETURN_FALSE;
	}

	zend_string_release(storage_key);

	RETURN_BOOL(exists);
}

ZEND_METHOD(OPcache_VolatileCache, delete)
{
	zend_opcache_static_cache_context *previous_context;
	zend_opcache_static_cache_object *cache;
	zend_string *key, *storage_key;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(key)
	ZEND_PARSE_PARAMETERS_END();

	cache = zend_opcache_static_cache_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	if (!zend_opcache_static_cache_validate_key(key, 1)) {
		RETURN_THROWS();
	}

	previous_context = zend_opcache_static_cache_activate_context(cache->context);
	if (!zend_opcache_static_cache_validate_available_write()) {
		zend_opcache_static_cache_restore_context(previous_context);

		if (EG(exception)) {
			RETURN_THROWS();
		}

		RETURN_FALSE;
	}

	storage_key = zend_opcache_static_cache_build_storage_key(cache, key);
	if (!zend_opcache_static_cache_explicit_delete_prevalidated(storage_key)) {
		zend_string_release(storage_key);
		zend_opcache_static_cache_restore_context(previous_context);

		if (EG(exception)) {
			RETURN_THROWS();
		}

		RETURN_FALSE;
	}

	zend_string_release(storage_key);

	zend_opcache_static_cache_restore_context(previous_context);

	RETURN_TRUE;
}

ZEND_METHOD(OPcache_VolatileCache, deleteMultiple)
{
	zend_opcache_static_cache_context *previous_context;
	zend_opcache_static_cache_object *cache;
	zend_string **prepared_keys, *storage_key;
	HashTable *keys;
	uint32_t key_count, index;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(keys)
	ZEND_PARSE_PARAMETERS_END();

	cache = zend_opcache_static_cache_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	if (!zend_opcache_static_cache_prepare_key_list(keys, &prepared_keys, &key_count, 1)) {
		RETURN_THROWS();
	}

	previous_context = zend_opcache_static_cache_activate_context(cache->context);
	if (!zend_opcache_static_cache_validate_available_write()) {
		zend_opcache_static_cache_restore_context(previous_context);
		zend_opcache_static_cache_release_key_list(prepared_keys, key_count);

		if (EG(exception)) {
			RETURN_THROWS();
		}

		RETURN_FALSE;
	}

	for (index = 0; index < key_count; index++) {
		storage_key = zend_opcache_static_cache_build_storage_key(cache, prepared_keys[index]);
		if (!zend_opcache_static_cache_explicit_delete_prevalidated(storage_key)) {
			zend_string_release(storage_key);

			zend_opcache_static_cache_restore_context(previous_context);
			zend_opcache_static_cache_release_key_list(prepared_keys, key_count);

			if (EG(exception)) {
				RETURN_THROWS();
			}

			RETURN_FALSE;
		}

		zend_string_release(storage_key);
	}

	zend_opcache_static_cache_restore_context(previous_context);
	zend_opcache_static_cache_release_key_list(prepared_keys, key_count);

	RETURN_TRUE;
}

ZEND_METHOD(OPcache_VolatileCache, clear)
{
	zend_opcache_static_cache_context *previous_context;
	zend_opcache_static_cache_object *cache;

	ZEND_PARSE_PARAMETERS_NONE();

	cache = zend_opcache_static_cache_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	previous_context = zend_opcache_static_cache_activate_context(cache->context);
	if (!zend_opcache_static_cache_validate_available_write()) {
		zend_opcache_static_cache_restore_context(previous_context);

		if (EG(exception)) {
			RETURN_THROWS();
		}

		RETURN_FALSE;
	}

	if (!zend_opcache_static_cache_explicit_clear_pool_prevalidated(cache->pool_prefix)) {
		zend_opcache_static_cache_restore_context(previous_context);

		if (EG(exception)) {
			RETURN_THROWS();
		}

		RETURN_FALSE;
	}

	zend_opcache_static_cache_restore_context(previous_context);

	RETURN_TRUE;
}

ZEND_METHOD(OPcache_VolatileCache, lock)
{
	zend_opcache_static_cache_object *cache;
	zend_long lease = 0;
	zend_string *key, *storage_key;
	bool locked;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(key)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(lease)
	ZEND_PARSE_PARAMETERS_END();

	cache = zend_opcache_static_cache_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	if (!zend_opcache_static_cache_validate_key(key, 1)) {
		RETURN_THROWS();
	}

	if (!zend_opcache_static_cache_parse_lease(lease, 2)) {
		RETURN_THROWS();
	}

	storage_key = zend_opcache_static_cache_build_storage_key(cache, key);
	if (zend_opcache_static_cache_lock_api(cache->context, storage_key, lease, &locked) == FAILURE) {
		zend_string_release(storage_key);

		if (EG(exception)) {
			RETURN_THROWS();
		}

		RETURN_FALSE;
	}

	zend_string_release(storage_key);

	RETURN_BOOL(locked);
}

ZEND_METHOD(OPcache_VolatileCache, unlock)
{
	zend_opcache_static_cache_object *cache;
	zend_string *key, *storage_key;
	bool unlocked;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(key)
	ZEND_PARSE_PARAMETERS_END();

	cache = zend_opcache_static_cache_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	if (!zend_opcache_static_cache_validate_key(key, 1)) {
		RETURN_THROWS();
	}

	storage_key = zend_opcache_static_cache_build_storage_key(cache, key);
	if (zend_opcache_static_cache_unlock_api(cache->context, storage_key, &unlocked) == FAILURE) {
		zend_string_release(storage_key);

		if (EG(exception)) {
			RETURN_THROWS();
		}

		RETURN_FALSE;
	}

	zend_string_release(storage_key);

	RETURN_BOOL(unlocked);
}

ZEND_METHOD(OPcache_VolatileCache, info)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_opcache_static_cache_return_info(return_value, zend_opcache_static_cache_active_volatile_context());

	if (EG(exception)) {
		RETURN_THROWS();
	}
}

ZEND_METHOD(OPcache_StableCache, __construct)
{
	ZEND_PARSE_PARAMETERS_NONE();
}

ZEND_METHOD(OPcache_StableCache, getInstance)
{
	zend_opcache_static_cache_get_instance(INTERNAL_FUNCTION_PARAM_PASSTHRU, true);
}

ZEND_METHOD(OPcache_StableCache, store)
{
	zend_opcache_static_cache_object *cache;
	zend_string *key;
	zval *value;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_STR(key)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	cache = zend_opcache_static_cache_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	if (!zend_opcache_static_cache_validate_key(key, 1)) {
		RETURN_THROWS();
	}

	if (!zend_opcache_static_cache_validate_api_value(value, 2)) {
		RETURN_THROWS();
	}

	if (!zend_opcache_static_cache_instance_store(cache, key, value, 0)) {
		if (EG(exception)) {
			RETURN_THROWS();
		}

		RETURN_FALSE;
	}

	RETURN_TRUE;
}

ZEND_METHOD(OPcache_StableCache, storeMultiple)
{
	zend_opcache_static_cache_object *cache;
	HashTable *values;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(values)
	ZEND_PARSE_PARAMETERS_END();

	cache = zend_opcache_static_cache_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	if (!zend_opcache_static_cache_validate_store_array(values, 1)) {
		RETURN_THROWS();
	}

	if (!zend_opcache_static_cache_instance_store_multiple(cache, values, 0)) {
		if (EG(exception)) {
			RETURN_THROWS();
		}

		RETURN_FALSE;
	}

	RETURN_TRUE;
}

ZEND_METHOD(OPcache_StableCache, storeWithTtl)
{
	zend_opcache_static_cache_object *cache;
	zend_long ttl;
	zend_string *key;
	zval *value;

	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_STR(key)
		Z_PARAM_ZVAL(value)
		Z_PARAM_LONG(ttl)
	ZEND_PARSE_PARAMETERS_END();

	cache = zend_opcache_static_cache_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	if (!zend_opcache_static_cache_validate_key(key, 1)) {
		RETURN_THROWS();
	}

	if (!zend_opcache_static_cache_validate_api_value(value, 2)) {
		RETURN_THROWS();
	}

	if (!zend_opcache_static_cache_parse_ttl(ttl, 3)) {
		RETURN_THROWS();
	}

	if (!zend_opcache_static_cache_instance_store(cache, key, value, ttl)) {
		if (EG(exception)) {
			RETURN_THROWS();
		}

		RETURN_FALSE;
	}

	RETURN_TRUE;
}

ZEND_METHOD(OPcache_StableCache, storeMultipleWithTtl)
{
	zend_opcache_static_cache_object *cache;
	zend_long ttl;
	HashTable *values;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_ARRAY_HT(values)
		Z_PARAM_LONG(ttl)
	ZEND_PARSE_PARAMETERS_END();

	cache = zend_opcache_static_cache_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	if (!zend_opcache_static_cache_validate_store_array(values, 1)) {
		RETURN_THROWS();
	}

	if (!zend_opcache_static_cache_parse_ttl(ttl, 2)) {
		RETURN_THROWS();
	}

	if (!zend_opcache_static_cache_instance_store_multiple(cache, values, ttl)) {
		if (EG(exception)) {
			RETURN_THROWS();
		}

		RETURN_FALSE;
	}

	RETURN_TRUE;
}

ZEND_METHOD(OPcache_StableCache, fetch)
{
	zend_opcache_static_cache_object *cache;
	zend_string *key, *storage_key;
	zval *default_value = NULL, default_null;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(key)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(default_value)
	ZEND_PARSE_PARAMETERS_END();

	cache = zend_opcache_static_cache_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

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

	storage_key = zend_opcache_static_cache_build_storage_key(cache, key);
	if (zend_opcache_static_cache_fetch_api(cache->context, storage_key, default_value, return_value) == FAILURE) {
		zend_string_release(storage_key);

		if (EG(exception)) {
			RETURN_THROWS();
		}

		RETURN_NULL();
	}

	zend_string_release(storage_key);
}

ZEND_METHOD(OPcache_StableCache, fetchMultiple)
{
	zend_opcache_static_cache_object *cache;
	zval *default_value = NULL, default_null;
	HashTable *keys;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ARRAY_HT(keys)
		Z_PARAM_OPTIONAL
		Z_PARAM_ARRAY_OR_NULL(default_value)
	ZEND_PARSE_PARAMETERS_END();

	cache = zend_opcache_static_cache_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	if (default_value == NULL) {
		ZVAL_NULL(&default_null);
		default_value = &default_null;
	}

	if (zend_opcache_static_cache_fetch_array_pool_api(cache, keys, default_value, return_value) == FAILURE) {
		if (EG(exception)) {
			RETURN_THROWS();
		}

		RETURN_NULL();
	}
}

ZEND_METHOD(OPcache_StableCache, has)
{
	zend_opcache_static_cache_object *cache;
	zend_string *key, *storage_key;
	bool exists;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(key)
	ZEND_PARSE_PARAMETERS_END();

	cache = zend_opcache_static_cache_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	if (!zend_opcache_static_cache_validate_key(key, 1)) {
		RETURN_THROWS();
	}

	storage_key = zend_opcache_static_cache_build_storage_key(cache, key);
	if (zend_opcache_static_cache_exists_api(cache->context, storage_key, &exists) == FAILURE) {
		zend_string_release(storage_key);

		if (EG(exception)) {
			RETURN_THROWS();
		}

		RETURN_FALSE;
	}

	zend_string_release(storage_key);

	RETURN_BOOL(exists);
}

ZEND_METHOD(OPcache_StableCache, delete)
{
	zend_opcache_static_cache_context *previous_context;
	zend_opcache_static_cache_object *cache;
	zend_string *key, *storage_key;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(key)
	ZEND_PARSE_PARAMETERS_END();

	cache = zend_opcache_static_cache_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	if (!zend_opcache_static_cache_validate_key(key, 1)) {
		RETURN_THROWS();
	}

	previous_context = zend_opcache_static_cache_activate_context(cache->context);
	if (!zend_opcache_static_cache_validate_available_write()) {
		zend_opcache_static_cache_restore_context(previous_context);

		if (EG(exception)) {
			RETURN_THROWS();
		}

		RETURN_FALSE;
	}

	storage_key = zend_opcache_static_cache_build_storage_key(cache, key);
	if (!zend_opcache_static_cache_explicit_delete_prevalidated(storage_key)) {
		zend_string_release(storage_key);

		zend_opcache_static_cache_restore_context(previous_context);

		if (EG(exception)) {
			RETURN_THROWS();
		}

		RETURN_FALSE;
	}

	zend_string_release(storage_key);

	zend_opcache_static_cache_restore_context(previous_context);

	RETURN_TRUE;
}

ZEND_METHOD(OPcache_StableCache, deleteMultiple)
{
	zend_opcache_static_cache_context *previous_context;
	zend_opcache_static_cache_object *cache;
	zend_string **prepared_keys, *storage_key;
	HashTable *keys;
	uint32_t key_count, index;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(keys)
	ZEND_PARSE_PARAMETERS_END();

	cache = zend_opcache_static_cache_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	if (!zend_opcache_static_cache_prepare_key_list(keys, &prepared_keys, &key_count, 1)) {
		RETURN_THROWS();
	}

	previous_context = zend_opcache_static_cache_activate_context(cache->context);
	if (!zend_opcache_static_cache_validate_available_write()) {
		zend_opcache_static_cache_restore_context(previous_context);
		zend_opcache_static_cache_release_key_list(prepared_keys, key_count);

		if (EG(exception)) {
			RETURN_THROWS();
		}

		RETURN_FALSE;
	}

	for (index = 0; index < key_count; index++) {
		storage_key = zend_opcache_static_cache_build_storage_key(cache, prepared_keys[index]);
		if (!zend_opcache_static_cache_explicit_delete_prevalidated(storage_key)) {
			zend_string_release(storage_key);

			zend_opcache_static_cache_restore_context(previous_context);
			zend_opcache_static_cache_release_key_list(prepared_keys, key_count);

			if (EG(exception)) {
				RETURN_THROWS();
			}

			RETURN_FALSE;
		}
		zend_string_release(storage_key);
	}

	zend_opcache_static_cache_restore_context(previous_context);
	zend_opcache_static_cache_release_key_list(prepared_keys, key_count);

	RETURN_TRUE;
}

ZEND_METHOD(OPcache_StableCache, clear)
{
	zend_opcache_static_cache_context *previous_context;
	zend_opcache_static_cache_object *cache;

	ZEND_PARSE_PARAMETERS_NONE();

	cache = zend_opcache_static_cache_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	previous_context = zend_opcache_static_cache_activate_context(cache->context);
	if (!zend_opcache_static_cache_validate_available_write()) {
		zend_opcache_static_cache_restore_context(previous_context);

		if (EG(exception)) {
			RETURN_THROWS();
		}

		RETURN_FALSE;
	}

	if (!zend_opcache_static_cache_explicit_clear_pool_prevalidated(cache->pool_prefix)) {
		zend_opcache_static_cache_restore_context(previous_context);

		if (EG(exception)) {
			RETURN_THROWS();
		}

		RETURN_FALSE;
	}

	zend_opcache_static_cache_restore_context(previous_context);

	RETURN_TRUE;
}

ZEND_METHOD(OPcache_StableCache, lock)
{
	zend_opcache_static_cache_object *cache;
	zend_long lease = 0;
	zend_string *key, *storage_key;
	bool locked;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(key)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(lease)
	ZEND_PARSE_PARAMETERS_END();

	cache = zend_opcache_static_cache_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	if (!zend_opcache_static_cache_validate_key(key, 1)) {
		RETURN_THROWS();
	}

	if (!zend_opcache_static_cache_parse_lease(lease, 2)) {
		RETURN_THROWS();
	}

	storage_key = zend_opcache_static_cache_build_storage_key(cache, key);
	if (zend_opcache_static_cache_lock_api(cache->context, storage_key, lease, &locked) == FAILURE) {
		zend_string_release(storage_key);

		if (EG(exception)) {
			RETURN_THROWS();
		}

		RETURN_FALSE;
	}

	zend_string_release(storage_key);

	RETURN_BOOL(locked);
}

ZEND_METHOD(OPcache_StableCache, unlock)
{
	zend_opcache_static_cache_object *cache;
	zend_string *key, *storage_key;
	bool unlocked;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(key)
	ZEND_PARSE_PARAMETERS_END();

	cache = zend_opcache_static_cache_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	if (!zend_opcache_static_cache_validate_key(key, 1)) {
		RETURN_THROWS();
	}

	storage_key = zend_opcache_static_cache_build_storage_key(cache, key);
	if (zend_opcache_static_cache_unlock_api(cache->context, storage_key, &unlocked) == FAILURE) {
		zend_string_release(storage_key);

		if (EG(exception)) {
			RETURN_THROWS();
		}

		RETURN_FALSE;
	}

	zend_string_release(storage_key);

	RETURN_BOOL(unlocked);
}

ZEND_METHOD(OPcache_StableCache, increment)
{
	zend_opcache_static_cache_context *previous_context;
	zend_opcache_static_cache_object *cache;
	zend_long step = 1, new_value;
	zend_string *key, *storage_key;
	bool release_entry_lock = false, is_overflow = false;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(key)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(step)
	ZEND_PARSE_PARAMETERS_END();

	cache = zend_opcache_static_cache_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	if (!zend_opcache_static_cache_validate_key(key, 1)) {
		RETURN_THROWS();
	}

	previous_context = zend_opcache_static_cache_activate_context(cache->context);
	if (!zend_opcache_static_cache_validate_available_write()) {
		zend_opcache_static_cache_restore_context(previous_context);

		if (EG(exception)) {
			RETURN_THROWS();
		}

		zend_opcache_static_cache_throw_atomic_failure("OPcache\\StableCache::increment", key);
		RETURN_THROWS();
	}

	storage_key = zend_opcache_static_cache_build_storage_key(cache, key);
	if (!zend_opcache_static_cache_begin_entry_mutation(storage_key, &release_entry_lock)) {
		zend_string_release(storage_key);

		zend_opcache_static_cache_restore_context(previous_context);

		if (EG(exception)) {
			RETURN_THROWS();
		}

		zend_opcache_static_cache_throw_atomic_failure("OPcache\\StableCache::increment", key);
		RETURN_THROWS();
	}

	if (!zend_opcache_static_cache_acquire_write_lock()) {
		if (release_entry_lock) {
			zend_opcache_static_cache_release_entry_lock(storage_key);
		}

		zend_string_release(storage_key);

		zend_opcache_static_cache_restore_context(previous_context);

		if (EG(exception)) {
			RETURN_THROWS();
		}

		zend_opcache_static_cache_throw_atomic_failure("OPcache\\StableCache::increment", key);
		RETURN_THROWS();
	}

	if (!zend_opcache_static_cache_atomic_update_locked(storage_key, step, false, true, &new_value, &is_overflow)) {
		zend_opcache_static_cache_unlock();

		if (release_entry_lock) {
			zend_opcache_static_cache_release_entry_lock(storage_key);
		}

		zend_string_release(storage_key);

		zend_opcache_static_cache_restore_context(previous_context);

		if (EG(exception)) {
			RETURN_THROWS();
		}

		zend_opcache_static_cache_throw_atomic_failure("OPcache\\StableCache::increment", key);
		RETURN_THROWS();
	}

	RETVAL_LONG(new_value);

	zend_opcache_static_cache_unlock();
	zend_opcache_static_cache_finish_entry_mutation(storage_key, release_entry_lock, true);

	zend_string_release(storage_key);

	zend_opcache_static_cache_restore_context(previous_context);

	if (is_overflow) {
		zend_opcache_static_cache_warn_atomic_overflow("OPcache\\StableCache::increment");
	}
}

ZEND_METHOD(OPcache_StableCache, decrement)
{
	zend_opcache_static_cache_context *previous_context;
	zend_opcache_static_cache_object *cache;
	zend_long step = 1, new_value;
	zend_string *key, *storage_key;
	bool release_entry_lock = false, is_overflow = false;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(key)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(step)
	ZEND_PARSE_PARAMETERS_END();

	cache = zend_opcache_static_cache_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	if (!zend_opcache_static_cache_validate_key(key, 1)) {
		RETURN_THROWS();
	}

	previous_context = zend_opcache_static_cache_activate_context(cache->context);
	if (!zend_opcache_static_cache_validate_available_write()) {
		zend_opcache_static_cache_restore_context(previous_context);

		if (EG(exception)) {
			RETURN_THROWS();
		}

		zend_opcache_static_cache_throw_atomic_failure("OPcache\\StableCache::decrement", key);
		RETURN_THROWS();
	}

	storage_key = zend_opcache_static_cache_build_storage_key(cache, key);
	if (!zend_opcache_static_cache_begin_entry_mutation(storage_key, &release_entry_lock)) {
		zend_string_release(storage_key);

		zend_opcache_static_cache_restore_context(previous_context);

		if (EG(exception)) {
			RETURN_THROWS();
		}

		zend_opcache_static_cache_throw_atomic_failure("OPcache\\StableCache::decrement", key);
		RETURN_THROWS();
	}

	if (!zend_opcache_static_cache_acquire_write_lock()) {
		if (release_entry_lock) {
			zend_opcache_static_cache_release_entry_lock(storage_key);
		}

		zend_string_release(storage_key);

		zend_opcache_static_cache_restore_context(previous_context);

		if (EG(exception)) {
			RETURN_THROWS();
		}

		zend_opcache_static_cache_throw_atomic_failure("OPcache\\StableCache::decrement", key);
		RETURN_THROWS();
	}

	if (!zend_opcache_static_cache_atomic_update_locked(storage_key, step, true, true, &new_value, &is_overflow)) {
		zend_opcache_static_cache_unlock();

		if (release_entry_lock) {
			zend_opcache_static_cache_release_entry_lock(storage_key);
		}

		zend_string_release(storage_key);

		zend_opcache_static_cache_restore_context(previous_context);

		if (EG(exception)) {
			RETURN_THROWS();
		}

		zend_opcache_static_cache_throw_atomic_failure("OPcache\\StableCache::decrement", key);
		RETURN_THROWS();
	}

	RETVAL_LONG(new_value);

	zend_opcache_static_cache_unlock();
	zend_opcache_static_cache_finish_entry_mutation(storage_key, release_entry_lock, true);
	zend_string_release(storage_key);
	zend_opcache_static_cache_restore_context(previous_context);

	if (is_overflow) {
		zend_opcache_static_cache_warn_atomic_overflow("OPcache\\StableCache::decrement");
	}
}

ZEND_METHOD(OPcache_StableCache, info)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_opcache_static_cache_return_info(return_value, zend_opcache_static_cache_active_stable_context());

	if (EG(exception)) {
		RETURN_THROWS();
	}
}
