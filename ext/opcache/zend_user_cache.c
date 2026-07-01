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

#include "Zend/zend_atomic.h"
#include "Zend/zend_closures.h"
#include "Zend/zend_exceptions.h"

#include "ZendAccelerator.h"
#include "zend_accelerator_module.h"
#include "zend_shared_alloc.h"
#include "zend_user_cache.h"
#include "zend_smart_str.h"

#include "ext/standard/php_var.h"

#include "SAPI.h"

#include "zend_user_cache_internal.h"

#include "opcache_user_cache_arginfo.h"

#define ZEND_OPCACHE_USER_CACHE_API_VALUE_TYPE "object|array|string|int|float|bool|null"
#define ZEND_OPCACHE_USER_CACHE_STORAGE_KEY_CACHE_MAX 4096U

typedef struct _zend_opcache_user_cache_object {
	zend_string *scope;
	zend_string *scope_prefix;
	zend_opcache_user_cache_context *context;
	HashTable *storage_key_cache;
	zend_object std;
} zend_opcache_user_cache_object;

typedef struct {
	zend_ulong hash;
	zend_string *key;
	uint32_t index;
} zend_opcache_user_cache_bulk_order;

typedef struct {
	zend_long configured_memory;
	zend_long shared_memory_size;
	zend_long used_memory;
	zend_long free_memory;
	zend_long wasted_memory;
	zend_long entry_count;
	zend_long entry_capacity;
	zend_long tombstone_count;
	bool backend_initialized;
} zend_opcache_user_cache_info_stats;

zend_class_entry *zend_opcache_user_cache_exception_ce;
zend_class_entry *zend_opcache_user_cache_info_ce;
static zend_class_entry *zend_opcache_user_cache_cache_ce;
static zend_object_handlers zend_opcache_user_cache_object_handlers;
zend_opcache_user_cache_context zend_opcache_user_cache_context_state = {
	{0}, "user cache", "opcache_user_cache_lock",
#ifndef ZEND_WIN32
	ZEND_OPCACHE_USER_CACHE_SEM_FILENAME_PREFIX,
#endif
	true, false
};
bool zend_opcache_user_cache_runtime_opted_in = false;
zend_opcache_user_cache_partition *zend_opcache_user_cache_partitions = NULL;
static HashTable zend_opcache_user_cache_safe_direct_handler_table;
static bool zend_opcache_user_cache_safe_direct_handlers_initialized = false;
ZEND_EXT_TLS const char *zend_opcache_user_cache_request_unavailable_reason = NULL;
ZEND_EXT_TLS zend_opcache_user_cache_partition *zend_opcache_user_cache_active_partition = NULL;
ZEND_EXT_TLS bool zend_opcache_user_cache_lock_held = false;
ZEND_EXT_TLS bool zend_opcache_user_cache_lock_held_is_write = false;
ZEND_EXT_TLS zend_opcache_user_cache_runtime zend_opcache_user_cache_runtime_state = {0};
ZEND_EXT_TLS zend_opcache_user_cache_context *zend_opcache_user_cache_active_context_ptr = NULL;
ZEND_EXT_TLS zend_opcache_user_cache_shared_graph_ref *zend_opcache_user_cache_shared_graph_refs = NULL;
ZEND_EXT_TLS uint32_t zend_opcache_user_cache_shared_graph_ref_count = 0;
ZEND_EXT_TLS uint32_t zend_opcache_user_cache_shared_graph_ref_capacity = 0;
ZEND_EXT_TLS HashTable *zend_opcache_user_cache_shared_graph_ref_index = NULL;
ZEND_EXT_TLS zend_opcache_user_cache_shared_graph_ref *zend_opcache_user_cache_retired_shared_graphs = NULL;
ZEND_EXT_TLS uint32_t zend_opcache_user_cache_retired_shared_graph_count = 0;
ZEND_EXT_TLS uint32_t zend_opcache_user_cache_retired_shared_graph_capacity = 0;
ZEND_EXT_TLS zend_opcache_user_cache_lookup_entry zend_opcache_user_cache_lookup_entry_storage[ZEND_OPCACHE_USER_CACHE_LOOKUP_BUCKETS];
ZEND_EXT_TLS HashTable *zend_opcache_user_cache_request_local_slot_table = NULL;
ZEND_EXT_TLS HashTable *zend_opcache_user_cache_entry_lock_table = NULL;

static zend_string *zend_opcache_user_cache_storage_key(
	zend_opcache_user_cache_object *cache,
	zend_string *key
);
static void zend_opcache_user_cache_object_free(zend_object *object);
static zend_object *zend_opcache_user_cache_object_create(zend_class_entry *ce);
static bool zend_opcache_user_cache_store_storage_key_prevalidated(zend_string *key, zval *value, zend_long ttl);

static bool zend_opcache_user_cache_key_is_valid_user_key(zend_string *key)
{
	return ZSTR_LEN(key) != 0 &&
		memchr(ZSTR_VAL(key), ZEND_OPCACHE_USER_CACHE_KEY_DELIMITER_CHAR, ZSTR_LEN(key)) == NULL
	;
}

static bool zend_opcache_user_cache_validate_key(zend_string *key, uint32_t arg_num)
{
	if (ZSTR_LEN(key) == 0) {
		zend_argument_value_error(arg_num, "must be a non-empty string");

		return false;
	}

	if (memchr(ZSTR_VAL(key), ZEND_OPCACHE_USER_CACHE_KEY_DELIMITER_CHAR, ZSTR_LEN(key)) != NULL) {
		zend_argument_value_error(arg_num, "must not contain the user-cache key delimiter " ZEND_OPCACHE_USER_CACHE_KEY_DELIMITER_NAME);

		return false;
	}

	return true;
}

static bool zend_opcache_user_cache_validate_store_array_value(zval *value, uint32_t arg_num)
{
	ZVAL_DEREF(value);

	if (Z_TYPE_P(value) == IS_RESOURCE) {
		zend_argument_type_error(arg_num, "must contain only values of type " ZEND_OPCACHE_USER_CACHE_API_VALUE_TYPE ", resource given");

		return false;
	}

	if (Z_TYPE_P(value) == IS_OBJECT && Z_OBJCE_P(value) == zend_ce_closure) {
		zend_argument_type_error(arg_num, "must not contain Closure objects");

		return false;
	}

	return true;
}

static bool zend_opcache_user_cache_validate_store_array(HashTable *values, uint32_t arg_num)
{
	zend_string *key;
	zval *value;

	ZEND_HASH_FOREACH_STR_KEY_VAL(values, key, value) {
		if (key == NULL || !zend_opcache_user_cache_key_is_valid_user_key(key)) {
			zend_argument_value_error(arg_num, "must be an array with non-empty string keys that do not contain " ZEND_OPCACHE_USER_CACHE_KEY_DELIMITER_NAME);

			return false;
		}

		if (!zend_opcache_user_cache_validate_store_array_value(value, arg_num)) {
			return false;
		}
	} ZEND_HASH_FOREACH_END();

	return true;
}

static void zend_opcache_user_cache_release_key_list(zend_string **keys, uint32_t key_count)
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

static bool zend_opcache_user_cache_prepare_key_list(
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
			if (!zend_opcache_user_cache_key_is_valid_user_key(Z_STR_P(value))) {
				zend_argument_value_error(arg_num, "must contain only non-empty string or int cache keys that do not contain " ZEND_OPCACHE_USER_CACHE_KEY_DELIMITER_NAME);
				zend_opcache_user_cache_release_key_list(prepared, index);

				return false;
			}

			prepared[index++] = zend_string_copy(Z_STR_P(value));
		} else if (Z_TYPE_P(value) == IS_LONG) {
			prepared[index++] = zend_long_to_str(Z_LVAL_P(value));
		} else {
			zend_argument_value_error(arg_num, "must contain only non-empty string or int cache keys that do not contain " ZEND_OPCACHE_USER_CACHE_KEY_DELIMITER_NAME);
			zend_opcache_user_cache_release_key_list(prepared, index);

			return false;
		}
	} ZEND_HASH_FOREACH_END();

	*prepared_keys = prepared;
	*prepared_key_count = index;

	return true;
}

static bool zend_opcache_user_cache_validate_api_value(zval *value, uint32_t arg_num)
{
	ZVAL_DEREF(value);

	if (Z_TYPE_P(value) == IS_RESOURCE) {
		zend_argument_type_error(arg_num, "must be of type " ZEND_OPCACHE_USER_CACHE_API_VALUE_TYPE ", resource given");

		return false;
	}

	if (Z_TYPE_P(value) == IS_OBJECT && Z_OBJCE_P(value) == zend_ce_closure) {
		zend_argument_type_error(arg_num, "must not be a Closure object");

		return false;
	}

	return true;
}

static zend_string *zend_opcache_user_cache_build_scope_prefix(zend_string *scope)
{
	const char *prefix = "user_cache" ZEND_OPCACHE_USER_CACHE_KEY_DELIMITER;
	size_t prefix_len = sizeof("user_cache" ZEND_OPCACHE_USER_CACHE_KEY_DELIMITER) - 1;

	return zend_string_concat3(
		prefix,
		prefix_len,
		ZSTR_VAL(scope),
		ZSTR_LEN(scope),
		ZEND_STRL(ZEND_OPCACHE_USER_CACHE_KEY_DELIMITER)
	);
}

static zend_string *zend_opcache_user_cache_build_storage_key(
		const zend_opcache_user_cache_object *cache,
		zend_string *key)
{
	return zend_string_concat2(
		ZSTR_VAL(cache->scope_prefix),
		ZSTR_LEN(cache->scope_prefix),
		ZSTR_VAL(key),
		ZSTR_LEN(key)
	);
}

static zend_opcache_user_cache_object *zend_opcache_user_cache_object_from_obj(
	zend_object *object)
{
	return (zend_opcache_user_cache_object *) ((char *) object - offsetof(zend_opcache_user_cache_object, std));
}

static zend_opcache_user_cache_object *zend_opcache_user_cache_from_this(zval *this_ptr)
{
	zend_opcache_user_cache_object *cache = zend_opcache_user_cache_object_from_obj(Z_OBJ_P(this_ptr));

	if (cache->scope_prefix == NULL) {
		zend_throw_error(NULL, "Opcache\\UserCache instance was not initialized");

		return NULL;
	}

	return cache;
}

static void zend_opcache_user_cache_register_classes(void)
{
	if (zend_opcache_user_cache_cache_ce != NULL) {
		return;
	}

	zend_opcache_user_cache_exception_ce = zend_ce_exception;
	zend_opcache_user_cache_info_ce = register_class_Opcache_UserCacheInfo();
	zend_opcache_user_cache_cache_ce = register_class_Opcache_UserCache();

	zend_opcache_user_cache_cache_ce->create_object = zend_opcache_user_cache_object_create;

	memcpy(
		&zend_opcache_user_cache_object_handlers,
		zend_get_std_object_handlers(),
		sizeof(zend_object_handlers)
	);

	zend_opcache_user_cache_object_handlers.offset = offsetof(zend_opcache_user_cache_object, std);
	zend_opcache_user_cache_object_handlers.free_obj = zend_opcache_user_cache_object_free;
	zend_opcache_user_cache_object_handlers.clone_obj = NULL;
}

static void zend_opcache_user_cache_reset_class_entries(void)
{
	zend_opcache_user_cache_exception_ce = NULL;
	zend_opcache_user_cache_info_ce = NULL;
	zend_opcache_user_cache_cache_ce = NULL;
}

static void zend_opcache_user_cache_invalidate_all_context(zend_opcache_user_cache_context *context)
{
	zend_opcache_user_cache_context *previous_context;

	previous_context = zend_opcache_user_cache_activate_context(context);

	if (ZCG(accel_directives).user_cache_shm_size == 0 || !context->storage.initialized) {
		zend_opcache_user_cache_restore_context(previous_context);

		return;
	}

	if (!zend_opcache_user_cache_wlock()) {
		zend_opcache_user_cache_restore_context(previous_context);

		return;
	}

	if (zend_opcache_user_cache_header_is_initialized_locked()) {
		zend_opcache_user_cache_clear_locked();
	}

	zend_opcache_user_cache_unlock();
	zend_opcache_user_cache_restore_context(previous_context);
}

static bool zend_opcache_user_cache_parse_ttl(zend_long ttl, uint32_t arg_num)
{
	if (ttl < 0) {
		zend_argument_value_error(arg_num, "must be greater than or equal to 0");

		return false;
	}

	return true;
}

static bool zend_opcache_user_cache_parse_step(zend_long step, uint32_t arg_num)
{
	if (step < 0) {
		zend_argument_value_error(arg_num, "must be greater than or equal to 0");

		return false;
	}

	return true;
}

static bool zend_opcache_user_cache_parse_lease(zend_long lease, uint32_t arg_num)
{
	if (lease < 0) {
		zend_argument_value_error(arg_num, "must be greater than or equal to 0");

		return false;
	}

	return true;
}

/* The optimistic path performs its own consistency validation. */
static bool zend_opcache_user_cache_can_read(void)
{
	zend_opcache_user_cache_ensure_ready();

	return zend_opcache_user_cache_active_runtime()->available;
}

static bool zend_opcache_user_cache_begin_read(void)
{
	if (!zend_opcache_user_cache_can_read()) {
		return false;
	}

	if (!zend_opcache_user_cache_rlock()) {
		return false;
	}

	if (!zend_opcache_user_cache_header_is_initialized_locked()) {
		zend_opcache_user_cache_unlock();

		return false;
	}

	return true;
}

static bool zend_opcache_user_cache_can_write(void)
{
	zend_opcache_user_cache_ensure_ready();

	if (!zend_opcache_user_cache_active_runtime()->available) {
		return false;
	}

	return true;
}

static bool zend_opcache_user_cache_acquire_write_lock(void)
{
	if (!zend_opcache_user_cache_wlock()) {
		return false;
	}

	if (!zend_opcache_user_cache_header_init_locked()) {
		zend_opcache_user_cache_unlock();

		return false;
	}

	return true;
}

static bool zend_opcache_user_cache_begin_entry_mutation(zend_string *key, bool *release_entry_lock)
{
	ZEND_ASSERT(release_entry_lock != NULL);
	*release_entry_lock = false;

	if (!zend_opcache_user_cache_has_entry_lock(key)) {
		if (!zend_opcache_user_cache_acquire_entry_lock(key)) {
			return false;
		}

		*release_entry_lock = true;
	}

	return true;
}

static void zend_opcache_user_cache_finish_entry_mutation(zend_string *key, bool release_entry_lock)
{
	if (release_entry_lock) {
		zend_opcache_user_cache_release_entry_lock(key);
	}
}

static bool zend_opcache_user_cache_delete_storage_key_prevalidated(zend_string *key)
{
	if (!zend_opcache_user_cache_wlock_for_entry_mutation(key)) {
		return false;
	}

	zend_opcache_user_cache_delete_locked(key);

	zend_opcache_user_cache_unlock();

	return true;
}

static void zend_opcache_user_cache_delete_corrupt_entry_prevalidated(zend_string *key)
{
	if (EG(exception)) {
		return;
	}

	(void) zend_opcache_user_cache_delete_storage_key_prevalidated(key);
}

static bool zend_opcache_user_cache_clear_scope_prevalidated(
		zend_string *scope_prefix)
{
	if (!zend_opcache_user_cache_acquire_write_lock()) {
		return false;
	}

	zend_opcache_user_cache_delete_entries_by_prefix_locked(scope_prefix);

	zend_opcache_user_cache_unlock();
	zend_opcache_user_cache_release_active_request_local_slots_by_prefix(scope_prefix);

	return true;
}

static void zend_opcache_user_cache_fetch_api(
		zend_opcache_user_cache_context *context,
		zend_string *key,
		zval *default_value,
		zval *return_value)
{
	zend_opcache_user_cache_context *previous_context;
	bool fetched, found = false;

	previous_context = zend_opcache_user_cache_activate_context(context);

	if (!zend_opcache_user_cache_can_read()) {
		zend_opcache_user_cache_restore_context(previous_context);
		ZVAL_COPY(return_value, default_value);

		return;
	}

	switch (zend_opcache_user_cache_fetch_optimistic(key, return_value)) {
		case ZEND_OPCACHE_USER_CACHE_OPTIMISTIC_FOUND:
			zend_opcache_user_cache_restore_context(previous_context);

			return;
		case ZEND_OPCACHE_USER_CACHE_OPTIMISTIC_MISS:
			zend_opcache_user_cache_restore_context(previous_context);
			ZVAL_COPY(return_value, default_value);

			return;
		case ZEND_OPCACHE_USER_CACHE_OPTIMISTIC_FALLBACK:
			break;
	}

	if (EG(exception)) {
		zend_clear_exception();
		zend_opcache_user_cache_delete_corrupt_entry_prevalidated(key);
		zend_opcache_user_cache_restore_context(previous_context);
		ZVAL_COPY(return_value, default_value);

		return;
	}

	if (!zend_opcache_user_cache_begin_read()) {
		zend_opcache_user_cache_restore_context(previous_context);

		ZVAL_COPY(return_value, default_value);

		return;
	}

	fetched = zend_opcache_user_cache_fetch_locked(key, return_value, false, &found, true);

	zend_opcache_user_cache_unlock();

	if (!fetched) {
		if (!found) {
			zend_opcache_user_cache_restore_context(previous_context);
			ZVAL_COPY(return_value, default_value);

			return;
		}

		if (EG(exception)) {
			zend_clear_exception();
		}

		zend_opcache_user_cache_delete_corrupt_entry_prevalidated(key);
		zend_opcache_user_cache_restore_context(previous_context);
		ZVAL_COPY(return_value, default_value);

		return;
	}

	zend_opcache_user_cache_restore_context(previous_context);
}

static zend_result zend_opcache_user_cache_fetch_multiple_api(
		zend_opcache_user_cache_object *cache,
		HashTable *keys,
		zval *default_value,
		zval *return_value)
{
	zend_opcache_user_cache_context *previous_context;
	zend_string **prepared_keys, **storage_keys, *key, *storage_key;
	zval fetched_value, default_copy;
	uint32_t key_count, index;
	bool fetched, found, locked_fetch;

	if (!zend_opcache_user_cache_prepare_key_list(keys, &prepared_keys, &key_count, 1)) {
		return FAILURE;
	}

	storage_keys = NULL;
	if (key_count != 0) {
		storage_keys = safe_emalloc(key_count, sizeof(zend_string *), 0);
		for (index = 0; index < key_count; index++) {
			storage_keys[index] = zend_opcache_user_cache_storage_key(cache, prepared_keys[index]);
		}
	}

	array_init_size(return_value, key_count);

	previous_context = zend_opcache_user_cache_activate_context(cache->context);
	if (!zend_opcache_user_cache_begin_read()) {
		zend_opcache_user_cache_restore_context(previous_context);
		for (index = 0; index < key_count; index++) {
			ZVAL_COPY(&default_copy, default_value);
			zend_hash_update(Z_ARRVAL_P(return_value), prepared_keys[index], &default_copy);
			zend_string_release(storage_keys[index]);
		}

		if (storage_keys != NULL) {
			efree(storage_keys);
		}

		zend_opcache_user_cache_release_key_list(prepared_keys, key_count);

		return SUCCESS;
	}

	locked_fetch = true;
	for (index = 0; index < key_count; index++) {
		key = prepared_keys[index];
		storage_key = storage_keys[index];

		if (locked_fetch) {
			fetched = zend_opcache_user_cache_fetch_locked(storage_key, &fetched_value, false, &found, true);

			if (!fetched) {
				if (!found) {
					ZVAL_COPY(&fetched_value, default_value);
				} else {
					if (EG(exception)) {
						zend_clear_exception();
					}

					zend_opcache_user_cache_unlock();
					locked_fetch = false;
					zend_opcache_user_cache_delete_corrupt_entry_prevalidated(storage_key);
					ZVAL_COPY(&fetched_value, default_value);
				}
			}
		} else {
			zend_opcache_user_cache_fetch_api(cache->context, storage_key, default_value, &fetched_value);
		}

		zend_hash_update(Z_ARRVAL_P(return_value), key, &fetched_value);

		zend_string_release(storage_key);
	}

	if (locked_fetch) {
		zend_opcache_user_cache_unlock();
	}
	zend_opcache_user_cache_restore_context(previous_context);
	if (storage_keys != NULL) {
		efree(storage_keys);
	}
	zend_opcache_user_cache_release_key_list(prepared_keys, key_count);

	return SUCCESS;
}

static void zend_opcache_user_cache_atomic_update_api(
		zend_opcache_user_cache_context *context,
		zend_string *key,
		zend_long step,
		zend_long ttl,
		bool decrement,
		zend_long *new_value,
		bool *updated)
{
	zend_opcache_user_cache_context *previous_context;
	bool is_overflow = false;

	previous_context = zend_opcache_user_cache_activate_context(context);

	if (!zend_opcache_user_cache_can_write()) {
		*updated = false;
		zend_opcache_user_cache_restore_context(previous_context);

		return;
	}

	if (!zend_opcache_user_cache_wlock_for_entry_mutation(key)) {
		*updated = false;
		zend_opcache_user_cache_restore_context(previous_context);

		return;
	}

	*updated = zend_opcache_user_cache_atomic_update_locked(key, step, ttl, decrement, true, new_value, &is_overflow);

	zend_opcache_user_cache_unlock();
	zend_opcache_user_cache_restore_context(previous_context);

	if (is_overflow) {
		*updated = false;
	}
}

static void zend_opcache_user_cache_exists_api(zend_opcache_user_cache_context *context, zend_string *key, bool *exists)
{
	zend_opcache_user_cache_context *previous_context;

	previous_context = zend_opcache_user_cache_activate_context(context);

	if (!zend_opcache_user_cache_can_read()) {
		*exists = false;
		zend_opcache_user_cache_restore_context(previous_context);

		return;
	}

	switch (zend_opcache_user_cache_exists_optimistic(key)) {
		case ZEND_OPCACHE_USER_CACHE_OPTIMISTIC_FOUND:
			*exists = true;
			zend_opcache_user_cache_restore_context(previous_context);

			return;
		case ZEND_OPCACHE_USER_CACHE_OPTIMISTIC_MISS:
			*exists = false;
			zend_opcache_user_cache_restore_context(previous_context);

			return;
		case ZEND_OPCACHE_USER_CACHE_OPTIMISTIC_FALLBACK:
			break;
	}

	if (!zend_opcache_user_cache_begin_read()) {
		*exists = false;
		zend_opcache_user_cache_restore_context(previous_context);

		return;
	}

	*exists = zend_opcache_user_cache_exists_locked(key);

	zend_opcache_user_cache_unlock();
	zend_opcache_user_cache_restore_context(previous_context);
}

static void zend_opcache_user_cache_lock_api(
		zend_opcache_user_cache_context *context,
		zend_string *key,
		zend_long lease,
		bool *locked)
{
	zend_opcache_user_cache_context *previous_context;

	previous_context = zend_opcache_user_cache_activate_context(context);

	if (!zend_opcache_user_cache_can_write()) {
		*locked = false;
		zend_opcache_user_cache_restore_context(previous_context);

		return;
	}

	*locked = zend_opcache_user_cache_try_acquire_entry_lock(key, lease);

	zend_opcache_user_cache_restore_context(previous_context);
}

static void zend_opcache_user_cache_unlock_api(zend_opcache_user_cache_context *context, zend_string *key, bool *unlocked)
{
	zend_opcache_user_cache_context *previous_context;

	previous_context = zend_opcache_user_cache_activate_context(context);

	if (!zend_opcache_user_cache_can_write()) {
		*unlocked = false;
		zend_opcache_user_cache_restore_context(previous_context);

		return;
	}

	*unlocked = zend_opcache_user_cache_release_entry_lock(key);

	zend_opcache_user_cache_restore_context(previous_context);
}

static bool zend_opcache_user_cache_instance_store(
		zend_opcache_user_cache_object *cache,
		zend_string *key,
		zval *value,
		zend_long ttl)
{
	zend_opcache_user_cache_context *previous_context;
	zend_string *storage_key;
	bool stored;

	previous_context = zend_opcache_user_cache_activate_context(cache->context);
	if (!zend_opcache_user_cache_can_write()) {
		zend_opcache_user_cache_restore_context(previous_context);

		return false;
	}

	storage_key = zend_opcache_user_cache_storage_key(cache, key);
	stored = zend_opcache_user_cache_store_storage_key_prevalidated(storage_key, value, ttl);

	zend_string_release(storage_key);

	zend_opcache_user_cache_restore_context(previous_context);

	return stored;
}

static zend_long zend_opcache_user_cache_size_to_zend_long(size_t size)
{
	return size > (size_t) ZEND_LONG_MAX ? ZEND_LONG_MAX : (zend_long) size;
}

static void zend_opcache_user_cache_collect_info_stats(zend_opcache_user_cache_info_stats *stats)
{
	zend_opcache_user_cache_runtime *runtime;
	zend_opcache_user_cache_storage *storage;
	zend_opcache_user_cache_header *header;
	zend_opcache_user_cache_block *block;
	size_t free_memory = 0, wasted_memory = 0, tail_memory = 0;
	uint32_t block_offset, block_limit, next_offset, iteration_limit, iterations;

	memset(stats, 0, sizeof(*stats));

	runtime = zend_opcache_user_cache_active_runtime();
	storage = &zend_opcache_user_cache_active_context()->storage;

	stats->configured_memory = zend_opcache_user_cache_size_to_zend_long(runtime->configured_memory);
	stats->shared_memory_size = zend_opcache_user_cache_size_to_zend_long(storage->size);
	stats->backend_initialized = storage->initialized;

	if (!storage->initialized || !storage->lock_initialized || !zend_opcache_user_cache_rlock()) {
		return;
	}

	header = zend_opcache_user_cache_header_ptr();
	if (header == NULL || !zend_opcache_user_cache_header_is_initialized_locked()) {
		stats->free_memory = stats->shared_memory_size;
		zend_opcache_user_cache_unlock();

		return;
	}

	if (header->next_free <= header->data_size) {
		tail_memory = header->data_size - header->next_free;
	}

	block_limit = header->data_offset + header->next_free;
	iteration_limit = header->data_size / (uint32_t) sizeof(zend_opcache_user_cache_block);
	for (block_offset = header->free_list, iterations = 0;
			block_offset != 0 && iterations < iteration_limit;
			iterations++) {
		if (block_offset < header->data_offset || block_offset > block_limit - sizeof(zend_opcache_user_cache_block)) {
			break;
		}

		block = zend_opcache_user_cache_block_ptr(block_offset);
		next_offset = block_offset + block->size;
		if (block->size < sizeof(zend_opcache_user_cache_block) || next_offset > block_limit) {
			break;
		}

		wasted_memory += block->size;
		block_offset = block->next_free;
	}

	free_memory = tail_memory + wasted_memory;

	stats->entry_count = (zend_long) header->count;
	stats->entry_capacity = (zend_long) header->capacity;
	stats->tombstone_count = (zend_long) header->tombstone_count;
	stats->free_memory = zend_opcache_user_cache_size_to_zend_long(free_memory);
	stats->wasted_memory = zend_opcache_user_cache_size_to_zend_long(wasted_memory);
	stats->used_memory = storage->size > free_memory
		? zend_opcache_user_cache_size_to_zend_long(storage->size - free_memory)
		: 0;

	zend_opcache_user_cache_unlock();
}

static void zend_opcache_user_cache_return_user_info(
		zval *return_value,
		zend_opcache_user_cache_object *cache)
{
	zend_opcache_user_cache_context *previous_context;
	zend_opcache_user_cache_runtime *runtime;
	zend_opcache_user_cache_info_stats stats;

	previous_context = zend_opcache_user_cache_activate_context(cache->context);
	zend_opcache_user_cache_ensure_ready();
	runtime = zend_opcache_user_cache_active_runtime();
	zend_opcache_user_cache_collect_info_stats(&stats);

	object_init_ex(return_value, zend_opcache_user_cache_info_ce);
	zend_update_property_str(zend_opcache_user_cache_info_ce, Z_OBJ_P(return_value), ZEND_STRL("scope"), cache->scope);
	zend_update_property_bool(zend_opcache_user_cache_info_ce, Z_OBJ_P(return_value), ZEND_STRL("available"), runtime->available);
	if (runtime->failure_reason != NULL) {
		zend_update_property_string(zend_opcache_user_cache_info_ce, Z_OBJ_P(return_value), ZEND_STRL("unavailableReason"), (char *) runtime->failure_reason);
	} else {
		zend_update_property_null(zend_opcache_user_cache_info_ce, Z_OBJ_P(return_value), ZEND_STRL("unavailableReason"));
	}
	zend_update_property_bool(zend_opcache_user_cache_info_ce, Z_OBJ_P(return_value), ZEND_STRL("enabled"), runtime->enabled);
	zend_update_property_bool(zend_opcache_user_cache_info_ce, Z_OBJ_P(return_value), ZEND_STRL("startupFailed"), runtime->startup_failed);
	zend_update_property_bool(zend_opcache_user_cache_info_ce, Z_OBJ_P(return_value), ZEND_STRL("backendInitialized"), stats.backend_initialized);
	zend_update_property_long(zend_opcache_user_cache_info_ce, Z_OBJ_P(return_value), ZEND_STRL("configuredMemory"), stats.configured_memory);
	zend_update_property_long(zend_opcache_user_cache_info_ce, Z_OBJ_P(return_value), ZEND_STRL("sharedMemorySize"), stats.shared_memory_size);
	zend_update_property_long(zend_opcache_user_cache_info_ce, Z_OBJ_P(return_value), ZEND_STRL("usedMemory"), stats.used_memory);
	zend_update_property_long(zend_opcache_user_cache_info_ce, Z_OBJ_P(return_value), ZEND_STRL("freeMemory"), stats.free_memory);
	zend_update_property_long(zend_opcache_user_cache_info_ce, Z_OBJ_P(return_value), ZEND_STRL("wastedMemory"), stats.wasted_memory);
	zend_update_property_long(zend_opcache_user_cache_info_ce, Z_OBJ_P(return_value), ZEND_STRL("entryCount"), stats.entry_count);
	zend_update_property_long(zend_opcache_user_cache_info_ce, Z_OBJ_P(return_value), ZEND_STRL("entryCapacity"), stats.entry_capacity);
	zend_update_property_long(zend_opcache_user_cache_info_ce, Z_OBJ_P(return_value), ZEND_STRL("tombstoneCount"), stats.tombstone_count);

	zend_opcache_user_cache_restore_context(previous_context);
}

static void zend_opcache_user_cache_object_free(zend_object *object)
{
	zend_opcache_user_cache_object *cache = zend_opcache_user_cache_object_from_obj(object);

	if (cache->scope != NULL) {
		zend_string_release(cache->scope);
	}

	if (cache->scope_prefix != NULL) {
		zend_string_release(cache->scope_prefix);
	}

	if (cache->storage_key_cache != NULL) {
		zend_hash_destroy(cache->storage_key_cache);
		FREE_HASHTABLE(cache->storage_key_cache);
	}

	zend_object_std_dtor(&cache->std);
}

static zend_object *zend_opcache_user_cache_object_create(zend_class_entry *ce)
{
	zend_opcache_user_cache_object *cache;

	cache = zend_object_alloc(sizeof(zend_opcache_user_cache_object), ce);

	zend_object_std_init(&cache->std, ce);
	object_properties_init(&cache->std, ce);

	cache->scope = NULL;
	cache->scope_prefix = NULL;
	cache->storage_key_cache = NULL;
	cache->context = zend_opcache_user_cache_default_context();
	cache->std.handlers = &zend_opcache_user_cache_object_handlers;

	return &cache->std;
}

static zend_string *zend_opcache_user_cache_storage_key(
		zend_opcache_user_cache_object *cache,
		zend_string *key)
{
	zend_string *storage_key;
	zval *cached, cache_zv;

	if (cache->storage_key_cache != NULL) {
		cached = zend_hash_find(cache->storage_key_cache, key);
		if (cached != NULL) {
			return zend_string_copy(Z_STR_P(cached));
		}
	}

	storage_key = zend_opcache_user_cache_build_storage_key(cache, key);
	zend_string_hash_val(storage_key);

	if (cache->storage_key_cache == NULL) {
		ALLOC_HASHTABLE(cache->storage_key_cache);
		zend_hash_init(cache->storage_key_cache, 8, NULL, ZVAL_PTR_DTOR, 0);
	}

	if (zend_hash_num_elements(cache->storage_key_cache) < ZEND_OPCACHE_USER_CACHE_STORAGE_KEY_CACHE_MAX) {
		ZVAL_STR_COPY(&cache_zv, storage_key);
		zend_hash_add(cache->storage_key_cache, key, &cache_zv);
	}

	return storage_key;
}

static void zend_opcache_user_cache_safe_direct_handlers_dtor(zval *zv)
{
	pefree(Z_PTR_P(zv), true);
}

static const zend_opcache_user_cache_safe_direct_handlers *zend_opcache_user_cache_safe_direct_find_handlers(
		zend_class_entry *ce,
		zend_class_entry **base_ce_ptr)
{
	const zend_opcache_user_cache_safe_direct_handlers *handlers;

	if (!zend_opcache_user_cache_safe_direct_handlers_initialized) {
		return NULL;
	}

	while (ce != NULL) {
		handlers = zend_hash_index_find_ptr(
			&zend_opcache_user_cache_safe_direct_handler_table,
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

static bool zend_opcache_user_cache_store_storage_key_prevalidated(zend_string *key, zval *value, zend_long ttl)
{
	zend_opcache_user_cache_prepared_value prepared;
	uint64_t generation = 0;
	bool stored, seed_request_local_slot = false;

	if (!zend_opcache_user_cache_prepare_value(&prepared, key, value, false, false, false)) {
		zend_opcache_user_cache_destroy_prepared_value(&prepared);

		return false;
	}

	/* Prepare values before entering the write section. */
	if (!zend_opcache_user_cache_wlock_for_entry_mutation(key)) {
		zend_opcache_user_cache_destroy_prepared_value(&prepared);

		return false;
	}

	zend_try {
		stored = zend_opcache_user_cache_store_prepared_locked(
			key,
			value,
			&prepared,
			ttl,
			false,
			false,
			&generation,
			&seed_request_local_slot,
			NULL,
			true
		);
	} zend_catch {
		zend_opcache_user_cache_unlock_if_held();
		zend_opcache_user_cache_destroy_prepared_value(&prepared);
		zend_bailout();
	} zend_end_try();

	zend_opcache_user_cache_unlock();
	zend_opcache_user_cache_destroy_prepared_value(&prepared);

	if (stored && seed_request_local_slot) {
		zend_opcache_user_cache_store_request_local_value_slot(key, generation, value, true);
	}

	return stored;
}

static int zend_opcache_user_cache_bulk_order_compare(const void *lhs_ptr, const void *rhs_ptr)
{
	const zend_opcache_user_cache_bulk_order *lhs, *rhs;
	size_t lhs_len, rhs_len, compare_len;
	int result;

	lhs = lhs_ptr;
	rhs = rhs_ptr;

	if (lhs->hash != rhs->hash) {
		return lhs->hash < rhs->hash ? -1 : 1;
	}

	lhs_len = ZSTR_LEN(lhs->key);
	rhs_len = ZSTR_LEN(rhs->key);
	compare_len = lhs_len < rhs_len ? lhs_len : rhs_len;

	result = memcmp(ZSTR_VAL(lhs->key), ZSTR_VAL(rhs->key), compare_len);
	if (result != 0) {
		return result < 0 ? -1 : 1;
	}

	if (lhs_len != rhs_len) {
		return lhs_len < rhs_len ? -1 : 1;
	}

	return lhs->index < rhs->index ? -1 : (lhs->index > rhs->index ? 1 : 0);
}

/* Prepare first, reserve keys in order, then publish under one write lock. */
static bool zend_opcache_user_cache_instance_store_multiple(
		zend_opcache_user_cache_object *cache,
		HashTable *values,
		zend_long ttl)
{
	zend_opcache_user_cache_context *previous_context;
	zend_opcache_user_cache_prepared_value *prepared;
	zend_opcache_user_cache_replaced_entry *replaced;
	zend_opcache_user_cache_bulk_order *order;
	zend_string *key, **storage_keys;
	zval *value, **value_ptrs;
	uint64_t *generations;
	uint32_t count, index, rollback_index, prepared_count, stored_count;
	bool *reserved, *seed_slots, ok;

	previous_context = zend_opcache_user_cache_activate_context(cache->context);
	if (!zend_opcache_user_cache_can_write()) {
		zend_opcache_user_cache_restore_context(previous_context);

		return false;
	}

	count = zend_hash_num_elements(values);
	if (count == 0) {
		zend_opcache_user_cache_restore_context(previous_context);

		return true;
	}

	index = 0;
	prepared_count = 0;
	stored_count = 0;
	ok = true;

	storage_keys = safe_emalloc(count, sizeof(zend_string *), 0);
	value_ptrs = safe_emalloc(count, sizeof(zval *), 0);
	prepared = safe_emalloc(count, sizeof(zend_opcache_user_cache_prepared_value), 0);
	order = safe_emalloc(count, sizeof(zend_opcache_user_cache_bulk_order), 0);
	replaced = ecalloc(count, sizeof(zend_opcache_user_cache_replaced_entry));
	generations = safe_emalloc(count, sizeof(uint64_t), 0);
	reserved = ecalloc(count, sizeof(bool));
	seed_slots = ecalloc(count, sizeof(bool));

	ZEND_HASH_FOREACH_STR_KEY_VAL(values, key, value) {
		ZEND_ASSERT(key != NULL);

		storage_keys[index] = zend_opcache_user_cache_storage_key(cache, key);
		value_ptrs[index] = value;

		if (!zend_opcache_user_cache_prepare_value(&prepared[index], storage_keys[index], value, false, false, false)) {
			zend_opcache_user_cache_destroy_prepared_value(&prepared[index]);
			zend_string_release(storage_keys[index]);
			ok = false;

			break;
		}

		order[index].hash = zend_string_hash_val(storage_keys[index]);
		order[index].key = storage_keys[index];
		order[index].index = index;
		prepared_count = ++index;
	} ZEND_HASH_FOREACH_END();

	if (ok) {
		qsort(order, prepared_count, sizeof(*order), zend_opcache_user_cache_bulk_order_compare);

		for (index = 0; index < prepared_count; index++) {
			if (!zend_opcache_user_cache_begin_entry_mutation(
					storage_keys[order[index].index],
					&reserved[order[index].index]
				)
			) {
				ok = false;

				break;
			}
		}
	}

	if (ok && zend_opcache_user_cache_acquire_write_lock()) {
		zend_try {
			for (index = 0; index < prepared_count; index++) {
				if (!zend_opcache_user_cache_store_prepared_locked(
						storage_keys[index],
						value_ptrs[index],
						&prepared[index],
						ttl,
						false,
						false,
						&generations[index],
						&seed_slots[index],
						&replaced[index],
						false)
				) {
					ok = false;

					break;
				}

				stored_count = index + 1;
			}

			if (ok) {
				for (index = 0; index < stored_count; index++) {
					zend_opcache_user_cache_discard_replaced_entry_locked(storage_keys[index], &replaced[index]);
				}
			} else {
				for (rollback_index = stored_count; rollback_index > 0; rollback_index--) {
					zend_opcache_user_cache_rollback_replaced_entry_locked(
						storage_keys[rollback_index - 1],
						&replaced[rollback_index - 1]
					);
				}

				stored_count = 0;
			}
		} zend_catch {
			zend_opcache_user_cache_unlock_if_held();
			zend_bailout();
		} zend_end_try();

		zend_opcache_user_cache_unlock();
	} else {
		ok = false;
	}

	for (index = 0; index < prepared_count; index++) {
		zend_opcache_user_cache_finish_entry_mutation(storage_keys[index], reserved[index]);
	}

	for (index = 0; index < stored_count; index++) {
		if (seed_slots[index]) {
			zend_opcache_user_cache_store_request_local_value_slot(
				storage_keys[index],
				generations[index],
				value_ptrs[index],
				true
			);
		}
	}

	for (index = 0; index < prepared_count; index++) {
		zend_opcache_user_cache_destroy_prepared_value(&prepared[index]);
		zend_string_release(storage_keys[index]);
	}

	efree(seed_slots);
	efree(reserved);
	efree(generations);
	efree(replaced);
	efree(order);
	efree(prepared);
	efree(value_ptrs);
	efree(storage_keys);

	zend_opcache_user_cache_restore_context(previous_context);

	return ok;
}

static bool zend_opcache_user_cache_init_partition_contexts(zend_opcache_user_cache_partition *partition, const char *name)
{
	partition->context = zend_opcache_user_cache_context_state;
	partition->context.storage.lock_file = -1;
	if (name != NULL) {
		partition->name = strdup(name);
		if (partition->name == NULL) {
			return false;
		}

		partition->context.name = partition->name;
		partition->context.lock_name = partition->name;
	}

	return true;
}

static bool zend_opcache_user_cache_startup_storage_for_context(zend_opcache_user_cache_context *context)
{
	zend_opcache_user_cache_context *previous_context;

	previous_context = zend_opcache_user_cache_activate_context(context);

	zend_opcache_user_cache_reset_runtime();
	if (zend_opcache_user_cache_active_runtime()->enabled && ZCG(enabled) && accel_startup_ok && !file_cache_only) {
		if (!zend_opcache_user_cache_startup_storage_before_request()) {
			zend_opcache_user_cache_restore_context(previous_context);

			return false;
		}
	}

	zend_opcache_user_cache_restore_context(previous_context);

	return true;
}

static void zend_opcache_user_cache_partition_shutdown_all(void)
{
	zend_opcache_user_cache_partition *partition, *next;
	zend_opcache_user_cache_context *previous_context;

	previous_context = zend_opcache_user_cache_active_context_ptr;
	partition = zend_opcache_user_cache_partitions;
	while (partition != NULL) {
		next = partition->next;

		zend_opcache_user_cache_activate_context(&partition->context);
		zend_opcache_user_cache_shutdown_storage();
		zend_opcache_user_cache_reset_runtime();

		free(partition->name);
		free(partition);

		partition = next;
	}

	zend_opcache_user_cache_partitions = NULL;

	zend_opcache_user_cache_restore_context(previous_context);
}

void zend_opcache_user_cache_safe_direct_handlers_init(void)
{
	if (zend_opcache_user_cache_safe_direct_handlers_initialized) {
		return;
	}

	zend_hash_init(
		&zend_opcache_user_cache_safe_direct_handler_table,
		8,
		NULL,
		zend_opcache_user_cache_safe_direct_handlers_dtor,
		true
	);

	zend_opcache_user_cache_safe_direct_handlers_initialized = true;
}

void zend_opcache_user_cache_safe_direct_handlers_destroy(void)
{
	if (!zend_opcache_user_cache_safe_direct_handlers_initialized) {
		return;
	}

	zend_hash_destroy(&zend_opcache_user_cache_safe_direct_handler_table);

	zend_opcache_user_cache_safe_direct_handlers_initialized = false;
}

void zend_opcache_user_cache_safe_direct_register_class(
		zend_class_entry *ce,
		const zend_opcache_user_cache_safe_direct_handlers *handlers)
{
	zend_opcache_user_cache_safe_direct_handlers handlers_copy;

	if (ce == NULL ||
		handlers == NULL ||
		handlers->copy == NULL ||
		handlers->state_serialize == NULL ||
		handlers->state_unserialize == NULL
	) {
		return;
	}

	zend_opcache_user_cache_safe_direct_handlers_init();

	handlers_copy = *handlers;

	zend_hash_index_update_mem(
		&zend_opcache_user_cache_safe_direct_handler_table,
		(zend_ulong) (uintptr_t) ce,
		&handlers_copy,
		sizeof(handlers_copy)
	);
}

zend_opcache_user_cache_safe_direct_state_copy_func_t zend_opcache_user_cache_safe_direct_copy_func(
		zend_class_entry *ce,
		zend_class_entry **base_ce_ptr)
{
	const zend_opcache_user_cache_safe_direct_handlers *handlers =
		zend_opcache_user_cache_safe_direct_find_handlers(ce, base_ce_ptr)
	;

	return handlers != NULL ? handlers->copy : NULL;
}

zend_class_entry *zend_opcache_user_cache_safe_direct_find_base(zend_class_entry *ce)
{
	zend_class_entry *base_ce = NULL;

	if (zend_opcache_user_cache_safe_direct_copy_func(ce, &base_ce) == NULL) {
		return NULL;
	}

	return base_ce;
}

zend_opcache_user_cache_safe_direct_state_has_unstorable_func_t zend_opcache_user_cache_safe_direct_state_has_unstorable_func(
		zend_class_entry *ce)
{
	const zend_opcache_user_cache_safe_direct_handlers *handlers =
		zend_opcache_user_cache_safe_direct_find_handlers(ce, NULL)
	;

	return handlers != NULL ? handlers->state_has_unstorable : NULL;
}

zend_opcache_user_cache_safe_direct_state_serialize_func_t zend_opcache_user_cache_safe_direct_state_serialize_func(
		zend_class_entry *ce)
{
	const zend_opcache_user_cache_safe_direct_handlers *handlers =
		zend_opcache_user_cache_safe_direct_find_handlers(ce, NULL)
	;

	return handlers != NULL ? handlers->state_serialize : NULL;
}

zend_opcache_user_cache_safe_direct_state_unserialize_func_t zend_opcache_user_cache_safe_direct_state_unserialize_func(
		zend_class_entry *ce)
{
	const zend_opcache_user_cache_safe_direct_handlers *handlers =
		zend_opcache_user_cache_safe_direct_find_handlers(ce, NULL)
	;

	return handlers != NULL ? handlers->state_unserialize : NULL;
}

bool zend_opcache_user_cache_safe_direct_prefers_request_local_prototype(zend_class_entry *ce)
{
	const zend_opcache_user_cache_safe_direct_handlers *handlers =
		zend_opcache_user_cache_safe_direct_find_handlers(ce, NULL)
	;

	return handlers != NULL && handlers->prefer_request_local_prototype;
}

ZEND_API zend_opcache_user_cache_partition *zend_opcache_user_cache_partition_create(const char *name)
{
	zend_opcache_user_cache_partition *partition;

	partition = calloc(1, sizeof(zend_opcache_user_cache_partition));
	if (partition == NULL) {
		return NULL;
	}

	if (!zend_opcache_user_cache_init_partition_contexts(partition, name)) {
		free(partition);

		return NULL;
	}

	partition->next = zend_opcache_user_cache_partitions;
	zend_opcache_user_cache_partitions = partition;

	return partition;
}

ZEND_API bool zend_opcache_user_cache_partition_startup_storage(zend_opcache_user_cache_partition *partition)
{
	zend_opcache_user_cache_partition *previous_partition;
	bool ok;

	if (partition == NULL) {
		return true;
	}

	previous_partition = zend_opcache_user_cache_active_partition;
	zend_opcache_user_cache_active_partition = partition;
	ok = zend_opcache_user_cache_startup_storage_for_context(&partition->context);
	zend_opcache_user_cache_active_partition = previous_partition;

	return ok;
}

ZEND_API bool zend_opcache_user_cache_startup_default_context_storage(void)
{
	return zend_opcache_user_cache_startup_storage_for_context(&zend_opcache_user_cache_context_state);
}

ZEND_API void zend_opcache_user_cache_partition_activate(zend_opcache_user_cache_partition *partition)
{
	zend_opcache_user_cache_active_partition = partition;
	zend_opcache_user_cache_active_context_ptr = NULL;
	zend_opcache_user_cache_request_unavailable_reason = NULL;
}

ZEND_API void zend_opcache_user_cache_activate_request_unavailable(const char *failure_reason)
{
	zend_opcache_user_cache_active_partition = NULL;
	zend_opcache_user_cache_active_context_ptr = NULL;
	zend_opcache_user_cache_request_unavailable_reason = failure_reason;
}

ZEND_API void zend_opcache_user_cache_opt_in(void)
{
	zend_opcache_user_cache_runtime_opted_in = true;
}

void zend_opcache_user_cache_minit(void)
{
	zend_opcache_user_cache_context *previous_context;

	zend_opcache_user_cache_runtime_opted_in = false;

	zend_opcache_user_cache_register_classes();
	zend_opcache_user_cache_safe_direct_handlers_init();
	zend_opcache_user_cache_optimistic_fork_setup();

	previous_context = zend_opcache_user_cache_activate_context(zend_opcache_user_cache_default_context());

	zend_opcache_user_cache_reset_storage();

	zend_opcache_user_cache_restore_context(previous_context);
}

void zend_opcache_user_cache_mshutdown(void)
{
	zend_opcache_user_cache_context *previous_context;

	zend_opcache_user_cache_partition_shutdown_all();

	zend_opcache_user_cache_active_partition = NULL;
	previous_context = zend_opcache_user_cache_activate_context(zend_opcache_user_cache_default_context());

	zend_opcache_user_cache_shutdown_storage();
	zend_opcache_user_cache_reset_runtime();
	zend_opcache_user_cache_restore_context(previous_context);

	zend_opcache_user_cache_active_partition = NULL;
	zend_opcache_user_cache_runtime_opted_in = false;

	zend_opcache_user_cache_safe_direct_handlers_destroy();
	zend_opcache_user_cache_reset_class_entries();
}

void zend_opcache_user_cache_invalidate_all(void)
{
	zend_opcache_user_cache_partition *partition;
	zend_opcache_user_cache_partition *previous_partition;
	zend_opcache_user_cache_context *previous_context;

	previous_partition = zend_opcache_user_cache_active_partition;
	previous_context = zend_opcache_user_cache_active_context_ptr;

	zend_opcache_user_cache_invalidate_all_context(&zend_opcache_user_cache_context_state);
	for (partition = zend_opcache_user_cache_partitions; partition != NULL; partition = partition->next) {
		zend_opcache_user_cache_invalidate_all_context(&partition->context);
	}

	zend_opcache_user_cache_active_partition = previous_partition;
	zend_opcache_user_cache_active_context_ptr = previous_context;

	zend_opcache_user_cache_clear_lookup_cache();
	zend_opcache_user_cache_release_active_request_local_slots();
}

zend_result zend_opcache_user_cache_rshutdown(void)
{
	zend_opcache_user_cache_request_unavailable_reason = NULL;

	zend_opcache_user_cache_clear_lookup_cache();

	zend_opcache_user_cache_release_request_entry_locks();
	zend_opcache_user_cache_release_request_local_slots();
	zend_opcache_user_cache_decode_resolve_cache_release();

	return SUCCESS;
}

zend_result zend_opcache_user_cache_post_deactivate(void)
{
	zend_opcache_user_cache_release_request_shared_graph_refs();

	return SUCCESS;
}

ZEND_METHOD(Opcache_UserCacheInfo, __construct)
{
}

ZEND_METHOD(Opcache_UserCache, __construct)
{
	zend_opcache_user_cache_object *cache;
	zend_string *scope;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(scope)
	ZEND_PARSE_PARAMETERS_END();

	if (memchr(ZSTR_VAL(scope), ZEND_OPCACHE_USER_CACHE_KEY_DELIMITER_CHAR, ZSTR_LEN(scope)) != NULL) {
		zend_argument_value_error(1, "must not contain the user-cache key delimiter " ZEND_OPCACHE_USER_CACHE_KEY_DELIMITER_NAME);

		RETURN_THROWS();
	}

	cache = zend_opcache_user_cache_object_from_obj(Z_OBJ_P(ZEND_THIS));
	cache->scope = zend_string_copy(scope);
	cache->scope_prefix = zend_opcache_user_cache_build_scope_prefix(scope);
	cache->context = zend_opcache_user_cache_default_context();
}

ZEND_METHOD(Opcache_UserCache, store)
{
	zend_opcache_user_cache_object *cache;
	zend_long ttl = 0;
	zend_string *key;
	zval *value;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(key)
		Z_PARAM_ZVAL(value)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(ttl)
	ZEND_PARSE_PARAMETERS_END();

	cache = zend_opcache_user_cache_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	if (!zend_opcache_user_cache_validate_key(key, 1)) {
		RETURN_THROWS();
	}

	if (!zend_opcache_user_cache_validate_api_value(value, 2)) {
		RETURN_THROWS();
	}

	if (!zend_opcache_user_cache_parse_ttl(ttl, 3)) {
		RETURN_THROWS();
	}

	if (!zend_opcache_user_cache_instance_store(cache, key, value, ttl)) {
		if (EG(exception)) {
			RETURN_THROWS();
		}

		RETURN_FALSE;
	}

	RETURN_TRUE;
}

ZEND_METHOD(Opcache_UserCache, storeMultiple)
{
	zend_opcache_user_cache_object *cache;
	zend_long ttl = 0;
	HashTable *values;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ARRAY_HT(values)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(ttl)
	ZEND_PARSE_PARAMETERS_END();

	cache = zend_opcache_user_cache_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	if (!zend_opcache_user_cache_validate_store_array(values, 1)) {
		RETURN_THROWS();
	}

	if (!zend_opcache_user_cache_parse_ttl(ttl, 2)) {
		RETURN_THROWS();
	}

	if (!zend_opcache_user_cache_instance_store_multiple(cache, values, ttl)) {
		if (EG(exception)) {
			RETURN_THROWS();
		}

		RETURN_FALSE;
	}

	RETURN_TRUE;
}

ZEND_METHOD(Opcache_UserCache, increment)
{
	zend_opcache_user_cache_object *cache;
	zend_string *key, *storage_key;
	zend_long step = 1, ttl = 0, new_value = 0;
	bool updated;

	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_STR(key)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(step)
		Z_PARAM_LONG(ttl)
	ZEND_PARSE_PARAMETERS_END();

	cache = zend_opcache_user_cache_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	if (!zend_opcache_user_cache_validate_key(key, 1)) {
		RETURN_THROWS();
	}

	if (!zend_opcache_user_cache_parse_step(step, 2)) {
		RETURN_THROWS();
	}

	if (!zend_opcache_user_cache_parse_ttl(ttl, 3)) {
		RETURN_THROWS();
	}

	storage_key = zend_opcache_user_cache_storage_key(cache, key);
	zend_opcache_user_cache_atomic_update_api(cache->context, storage_key, step, ttl, false, &new_value, &updated);
	zend_string_release(storage_key);

	if (!updated) {
		RETURN_FALSE;
	}

	RETURN_LONG(new_value);
}

ZEND_METHOD(Opcache_UserCache, decrement)
{
	zend_opcache_user_cache_object *cache;
	zend_string *key, *storage_key;
	zend_long step = 1, ttl = 0, new_value = 0;
	bool updated;

	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_STR(key)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(step)
		Z_PARAM_LONG(ttl)
	ZEND_PARSE_PARAMETERS_END();

	cache = zend_opcache_user_cache_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	if (!zend_opcache_user_cache_validate_key(key, 1)) {
		RETURN_THROWS();
	}

	if (!zend_opcache_user_cache_parse_step(step, 2)) {
		RETURN_THROWS();
	}

	if (!zend_opcache_user_cache_parse_ttl(ttl, 3)) {
		RETURN_THROWS();
	}

	storage_key = zend_opcache_user_cache_storage_key(cache, key);
	zend_opcache_user_cache_atomic_update_api(cache->context, storage_key, step, ttl, true, &new_value, &updated);
	zend_string_release(storage_key);

	if (!updated) {
		RETURN_FALSE;
	}

	RETURN_LONG(new_value);
}

ZEND_METHOD(Opcache_UserCache, fetch)
{
	zend_opcache_user_cache_object *cache;
	zend_string *key, *storage_key;
	zval *default_value = NULL, default_null;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(key)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(default_value)
	ZEND_PARSE_PARAMETERS_END();

	cache = zend_opcache_user_cache_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	if (!zend_opcache_user_cache_validate_key(key, 1)) {
		RETURN_THROWS();
	}

	if (default_value == NULL) {
		ZVAL_NULL(&default_null);
		default_value = &default_null;
	}

	storage_key = zend_opcache_user_cache_storage_key(cache, key);
	zend_opcache_user_cache_fetch_api(cache->context, storage_key, default_value, return_value);
	zend_string_release(storage_key);
}

ZEND_METHOD(Opcache_UserCache, fetchMultiple)
{
	zend_opcache_user_cache_object *cache;
	zval *default_value = NULL, default_null;
	HashTable *keys;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ARRAY_HT(keys)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(default_value)
	ZEND_PARSE_PARAMETERS_END();

	cache = zend_opcache_user_cache_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	if (default_value == NULL) {
		ZVAL_NULL(&default_null);
		default_value = &default_null;
	}

	if (zend_opcache_user_cache_fetch_multiple_api(cache, keys, default_value, return_value) == FAILURE) {
		if (EG(exception)) {
			RETURN_THROWS();
		}

		RETURN_EMPTY_ARRAY();
	}
}

ZEND_METHOD(Opcache_UserCache, has)
{
	zend_opcache_user_cache_object *cache;
	zend_string *key, *storage_key;
	bool exists;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(key)
	ZEND_PARSE_PARAMETERS_END();

	cache = zend_opcache_user_cache_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	if (!zend_opcache_user_cache_validate_key(key, 1)) {
		RETURN_THROWS();
	}

	storage_key = zend_opcache_user_cache_storage_key(cache, key);
	zend_opcache_user_cache_exists_api(cache->context, storage_key, &exists);
	zend_string_release(storage_key);

	RETURN_BOOL(exists);
}

ZEND_METHOD(Opcache_UserCache, delete)
{
	zend_opcache_user_cache_context *previous_context;
	zend_opcache_user_cache_object *cache;
	zend_string *key, *storage_key;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(key)
	ZEND_PARSE_PARAMETERS_END();

	cache = zend_opcache_user_cache_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	if (!zend_opcache_user_cache_validate_key(key, 1)) {
		RETURN_THROWS();
	}

	previous_context = zend_opcache_user_cache_activate_context(cache->context);
	if (!zend_opcache_user_cache_can_write()) {
		zend_opcache_user_cache_restore_context(previous_context);

		if (EG(exception)) {
			RETURN_THROWS();
		}

		RETURN_TRUE;
	}

	storage_key = zend_opcache_user_cache_storage_key(cache, key);
	if (!zend_opcache_user_cache_delete_storage_key_prevalidated(storage_key)) {
		zend_string_release(storage_key);
		zend_opcache_user_cache_restore_context(previous_context);

		if (EG(exception)) {
			RETURN_THROWS();
		}

		RETURN_FALSE;
	}

	zend_string_release(storage_key);

	zend_opcache_user_cache_restore_context(previous_context);

	RETURN_TRUE;
}

ZEND_METHOD(Opcache_UserCache, deleteMultiple)
{
	zend_opcache_user_cache_context *previous_context;
	zend_opcache_user_cache_object *cache;
	zend_opcache_user_cache_bulk_order *order;
	zend_string **prepared_keys, **storage_keys;
	HashTable *keys;
	uint32_t key_count, index;
	bool *reserved, ok = true;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(keys)
	ZEND_PARSE_PARAMETERS_END();

	cache = zend_opcache_user_cache_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	if (!zend_opcache_user_cache_prepare_key_list(keys, &prepared_keys, &key_count, 1)) {
		RETURN_THROWS();
	}

	previous_context = zend_opcache_user_cache_activate_context(cache->context);
	if (!zend_opcache_user_cache_can_write()) {
		zend_opcache_user_cache_restore_context(previous_context);
		zend_opcache_user_cache_release_key_list(prepared_keys, key_count);

		if (EG(exception)) {
			RETURN_THROWS();
		}

		RETURN_TRUE;
	}

	if (key_count == 0) {
		zend_opcache_user_cache_restore_context(previous_context);
		zend_opcache_user_cache_release_key_list(prepared_keys, key_count);

		RETURN_TRUE;
	}

	storage_keys = safe_emalloc(key_count, sizeof(zend_string *), 0);
	order = safe_emalloc(key_count, sizeof(zend_opcache_user_cache_bulk_order), 0);
	reserved = ecalloc(key_count, sizeof(bool));

	for (index = 0; index < key_count; index++) {
		storage_keys[index] = zend_opcache_user_cache_storage_key(cache, prepared_keys[index]);
		order[index].hash = zend_string_hash_val(storage_keys[index]);
		order[index].key = storage_keys[index];
		order[index].index = index;
	}

	qsort(order, key_count, sizeof(*order), zend_opcache_user_cache_bulk_order_compare);

	for (index = 0; index < key_count; index++) {
		if (!zend_opcache_user_cache_begin_entry_mutation(
				storage_keys[order[index].index],
				&reserved[order[index].index])
		) {
			ok = false;

			break;
		}
	}

	if (ok && zend_opcache_user_cache_acquire_write_lock()) {
		zend_try {
			for (index = 0; index < key_count; index++) {
				if (!zend_opcache_user_cache_delete_locked(storage_keys[index])) {
					ok = false;

					break;
				}
			}
		} zend_catch {
			zend_opcache_user_cache_unlock_if_held();
			zend_bailout();
		} zend_end_try();

		zend_opcache_user_cache_unlock();
	} else {
		ok = false;
	}

	for (index = 0; index < key_count; index++) {
		zend_opcache_user_cache_finish_entry_mutation(storage_keys[index], reserved[index]);
		zend_string_release(storage_keys[index]);
	}

	efree(reserved);
	efree(order);
	efree(storage_keys);

	zend_opcache_user_cache_restore_context(previous_context);
	zend_opcache_user_cache_release_key_list(prepared_keys, key_count);

	if (!ok) {
		if (EG(exception)) {
			RETURN_THROWS();
		}

		RETURN_FALSE;
	}

	RETURN_TRUE;
}

ZEND_METHOD(Opcache_UserCache, clear)
{
	zend_opcache_user_cache_context *previous_context;
	zend_opcache_user_cache_object *cache;

	ZEND_PARSE_PARAMETERS_NONE();

	cache = zend_opcache_user_cache_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	previous_context = zend_opcache_user_cache_activate_context(cache->context);
	if (!zend_opcache_user_cache_can_write()) {
		zend_opcache_user_cache_restore_context(previous_context);

		if (EG(exception)) {
			RETURN_THROWS();
		}

		RETURN_TRUE;
	}

	if (!zend_opcache_user_cache_clear_scope_prevalidated(cache->scope_prefix)) {
		zend_opcache_user_cache_restore_context(previous_context);

		if (EG(exception)) {
			RETURN_THROWS();
		}

		RETURN_FALSE;
	}

	zend_opcache_user_cache_restore_context(previous_context);

	RETURN_TRUE;
}

ZEND_METHOD(Opcache_UserCache, lock)
{
	zend_opcache_user_cache_object *cache;
	zend_long lease = 0;
	zend_string *key, *storage_key;
	bool locked;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(key)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(lease)
	ZEND_PARSE_PARAMETERS_END();

	cache = zend_opcache_user_cache_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	if (!zend_opcache_user_cache_validate_key(key, 1)) {
		RETURN_THROWS();
	}

	if (!zend_opcache_user_cache_parse_lease(lease, 2)) {
		RETURN_THROWS();
	}

	storage_key = zend_opcache_user_cache_storage_key(cache, key);
	zend_opcache_user_cache_lock_api(cache->context, storage_key, lease, &locked);
	zend_string_release(storage_key);

	RETURN_BOOL(locked);
}

ZEND_METHOD(Opcache_UserCache, unlock)
{
	zend_opcache_user_cache_object *cache;
	zend_string *key, *storage_key;
	bool unlocked;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(key)
	ZEND_PARSE_PARAMETERS_END();

	cache = zend_opcache_user_cache_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	if (!zend_opcache_user_cache_validate_key(key, 1)) {
		RETURN_THROWS();
	}

	storage_key = zend_opcache_user_cache_storage_key(cache, key);
	zend_opcache_user_cache_unlock_api(cache->context, storage_key, &unlocked);
	zend_string_release(storage_key);

	RETURN_BOOL(unlocked);
}

ZEND_METHOD(Opcache_UserCache, remember)
{
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;
	zend_opcache_user_cache_object *cache;
	zend_long ttl = 0;
	zend_string *key, *storage_key;
	zval default_null = {0}, callback_result;
	bool exists = false, locked = false;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(key)
		Z_PARAM_FUNC(fci, fcc)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(ttl)
	ZEND_PARSE_PARAMETERS_END();

	cache = zend_opcache_user_cache_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	if (!zend_opcache_user_cache_validate_key(key, 1)) {
		RETURN_THROWS();
	}

	if (!zend_opcache_user_cache_parse_ttl(ttl, 3)) {
		RETURN_THROWS();
	}

	ZVAL_NULL(&default_null);
	storage_key = zend_opcache_user_cache_storage_key(cache, key);
	zend_opcache_user_cache_exists_api(cache->context, storage_key, &exists);
	if (exists) {
		zend_opcache_user_cache_fetch_api(cache->context, storage_key, &default_null, return_value);
		zend_string_release(storage_key);

		return;
	}

	zend_opcache_user_cache_lock_api(cache->context, storage_key, 0, &locked);

	zend_opcache_user_cache_exists_api(cache->context, storage_key, &exists);
	if (locked && exists) {
		zend_opcache_user_cache_fetch_api(cache->context, storage_key, &default_null, return_value);
		zend_opcache_user_cache_unlock_api(cache->context, storage_key, &locked);
		zend_string_release(storage_key);

		return;
	}

	ZVAL_UNDEF(&callback_result);
	fci.retval = &callback_result;

	zend_try {
		if (zend_call_function(&fci, &fcc) == SUCCESS && !EG(exception)) {
			if (Z_TYPE(callback_result) != IS_UNDEF) {
				if (!zend_opcache_user_cache_instance_store(cache, key, &callback_result, ttl) && EG(exception)) {
					zval_ptr_dtor(&callback_result);
					zend_opcache_user_cache_unlock_api(cache->context, storage_key, &locked);
					zend_string_release(storage_key);
					RETURN_THROWS();
				}
			}
		}
	} zend_catch {
		if (locked) {
			zend_opcache_user_cache_unlock_api(cache->context, storage_key, &locked);
		}
		zend_string_release(storage_key);
		zend_bailout();
	} zend_end_try();

	if (locked) {
		zend_opcache_user_cache_unlock_api(cache->context, storage_key, &locked);
	}
	zend_string_release(storage_key);

	if (EG(exception)) {
		if (Z_TYPE(callback_result) != IS_UNDEF) {
			zval_ptr_dtor(&callback_result);
		}
		RETURN_THROWS();
	}

	if (Z_TYPE(callback_result) == IS_UNDEF) {
		RETURN_NULL();
	}

	RETURN_COPY_VALUE(&callback_result);
}

ZEND_METHOD(Opcache_UserCache, info)
{
	zend_opcache_user_cache_object *cache;

	ZEND_PARSE_PARAMETERS_NONE();

	cache = zend_opcache_user_cache_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	zend_opcache_user_cache_return_user_info(return_value, cache);

	if (EG(exception)) {
		RETURN_THROWS();
	}
}
