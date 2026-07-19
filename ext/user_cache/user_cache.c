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
   | Authors: Go Kudo <zeriyoshi@php.net>                                 |
   +----------------------------------------------------------------------+
 */

#include "php.h"
#include "php_user_cache.h"

#include "user_cache_internal.h"
#include "user_cache_arginfo.h"
#include "user_cache_decl.h"
#include "user_cache_shm.h"

#include "Zend/zend_atomic.h"
#include "Zend/zend_closures.h"
#include "Zend/zend_exceptions.h"
#include "Zend/zend_smart_str.h"

#include "ext/standard/php_var.h"
#include "ext/standard/info.h"

#include "SAPI.h"

#define PHP_USER_CACHE_API_VALUE_TYPE			"object|array|string|int|float|bool|null"
#define PHP_USER_CACHE_STORAGE_KEY_CACHE_MAX	4096U
#define PHP_USER_CACHE_MAX_BOUNDARY_PARTITIONS	32U

#define PHP_USER_CACHE_DEFINE_OBJ_FROM_STD(type) \
	static type *type##_from_obj(zend_object *obj) \
	{ \
		return (type *) ((char *) obj - offsetof(type, std)); \
	}

#define PHP_USER_CACHE_DEFINE_SAFE_DIRECT_HANDLER_GETTER(member) \
	php_user_cache_safe_direct_##member##_func_t php_user_cache_safe_direct_##member##_func( \
			zend_class_entry *ce) \
	{ \
		const php_user_cache_safe_direct_handlers *handlers = \
			php_user_cache_safe_direct_find_handlers(ce, NULL) \
		; \
		return handlers != NULL ? handlers->member : NULL; \
	}

#define PHP_USER_CACHE_DEFINE_STATUS_LONG_GETTER(method, field) \
	ZEND_METHOD(UserCache_CacheStatus, method) \
	{ \
		php_user_cache_status_object *status; \
		\
		ZEND_PARSE_PARAMETERS_NONE(); \
		\
		status = user_cache_status_from_this(ZEND_THIS); \
		if (status == NULL) { \
			RETURN_THROWS(); \
		} \
		\
		RETURN_LONG(status->stats.field); \
	}

typedef struct {
	zend_string *scope;
	zend_string *scope_prefix;
	php_user_cache_context *context;
	HashTable *storage_key_cache;
	zend_object std;
} php_user_cache_object;

typedef struct {
	zend_string *scope;
	zend_string *scope_prefix;
	php_user_cache_context *context;
	/* Immutable snapshot. */
	zend_long entry_count;
	zend_long used_memory;
	zval entry_keys;
	zend_object std;
} php_user_cache_pool_status_object;

typedef struct {
	zend_ulong hash;
	zend_string *key;
	uint32_t index;
} php_user_cache_bulk_order;

typedef struct {
	zval *value;
	php_user_cache_prepared_value prepared;
	php_user_cache_store_result store_result;
	/* True while this item's store has committed but its captured replaced
	 * entry has not been discarded or rolled back; consulted on bailout. */
	bool committed;
} php_user_cache_bulk_store_item;

typedef struct {
	zend_long configured_memory;
	zend_long shared_memory_size;
	zend_long used_memory;
	zend_long free_memory;
	zend_long wasted_memory;
	zend_long entry_count;
	zend_long entry_capacity;
	zend_long tombstone_count;
	zend_long expunge_count;
	zend_long store_failure_count;
	zend_long graph_pin_slots_in_use;
	zend_long graph_pinned_references;
	zend_long dead_pin_owners_reclaimed;
	zend_long dead_pins_stripped;
} php_user_cache_info_stats;

typedef struct {
	/* Immutable snapshot. */
	php_user_cache_info_stats stats;
	php_user_cache_reason availability_reason;
	bool initialized;
	zend_object std;
} php_user_cache_status_object;

typedef struct _php_user_cache_boundary_partition {
	char *boundary;
	size_t boundary_len;
	zend_ulong boundary_hash;
	php_user_cache_partition *partition;
	struct _php_user_cache_boundary_partition *next;
} php_user_cache_boundary_partition;

#ifndef ZTS
php_user_cache_globals user_cache_globals;
#else
int user_cache_globals_id;
size_t user_cache_globals_offset;
#endif

static HashTable php_user_cache_safe_direct_handler_table;
static bool user_cache_safe_direct_handlers_initialized = false;
static php_user_cache_boundary_partition *php_user_cache_boundary_partitions = NULL;
static zend_class_entry *php_user_cache_availability_ce;
static zend_class_entry *php_user_cache_ce;
static zend_object_handlers php_user_cache_object_handlers;
static zend_object_handlers php_user_cache_status_object_handlers;
static zend_object_handlers php_user_cache_pool_status_object_handlers;
static uint32_t php_user_cache_boundary_partition_count = 0;
static bool php_user_cache_boundary_limit_logged = false;
static bool php_user_cache_boundary_startup_failed_logged = false;
static uint64_t php_user_cache_self_pid = 0;
#ifndef ZEND_WIN32
static bool php_user_cache_self_pid_uncached = false;
static bool php_user_cache_pid_atfork_registered = false;
#endif

zend_class_entry *php_user_cache_exception_ce;
zend_class_entry *php_user_cache_status_ce;
zend_class_entry *php_user_cache_pool_status_ce;
php_user_cache_context php_user_cache_context_state = {
	.storage = { .lock_file = -1 },
	.name = "user cache",
	.lock_name = "php_user_cache_lock",
#ifndef ZEND_WIN32
	.sem_filename_prefix = PHP_USER_CACHE_SEM_FILENAME_PREFIX,
#endif
	.clear_on_pressure = true,
};
bool php_user_cache_runtime_opted_in = false;
/* Append-only; cgi/lsapi boundary partitions are added lazily at request
 * activation, and those SAPIs are single-threaded, so no locking is needed. */
php_user_cache_partition *php_user_cache_partitions = NULL;

static void user_cache_object_free(zend_object *obj);
static zend_object *user_cache_object_create(zend_class_entry *ce);
static void user_cache_pool_status_object_free(zend_object *obj);
static zend_object *user_cache_pool_status_object_create(zend_class_entry *ce);
static zend_object *user_cache_status_object_create(zend_class_entry *ce);

#ifndef ZEND_WIN32
static void user_cache_pid_atfork_child(void)
{
	php_user_cache_self_pid = 0;
}
#endif

static bool user_cache_user_key_is_valid(zend_string *key)
{
	return ZSTR_LEN(key) != 0 &&
		memchr(ZSTR_VAL(key), PHP_USER_CACHE_KEY_DELIMITER_CHAR, ZSTR_LEN(key)) == NULL
	;
}

static bool user_cache_validate_delimiter_free(zend_string *str, uint32_t arg_num, const char *empty_error)
{
	if (ZSTR_LEN(str) == 0) {
		zend_argument_value_error(arg_num, "%s", empty_error);

		return false;
	}

	if (memchr(ZSTR_VAL(str), PHP_USER_CACHE_KEY_DELIMITER_CHAR, ZSTR_LEN(str)) != NULL) {
		zend_argument_value_error(arg_num, "must not contain the user-cache key delimiter " PHP_USER_CACHE_KEY_DELIMITER_NAME);

		return false;
	}

	return true;
}

static bool user_cache_validate_key(zend_string *key, uint32_t arg_num)
{
	return user_cache_validate_delimiter_free(key, arg_num, "must be a non-empty string");
}

static bool user_cache_validate_arg_value_kind(
		zval *value,
		uint32_t arg_num,
		const char *resource_error,
		const char *closure_error)
{
	ZVAL_DEREF(value);

	if (Z_TYPE_P(value) == IS_RESOURCE) {
		zend_argument_type_error(arg_num, "%s", resource_error);

		return false;
	}

	if (Z_TYPE_P(value) == IS_OBJECT && Z_OBJCE_P(value) == zend_ce_closure) {
		zend_argument_type_error(arg_num, "%s", closure_error);

		return false;
	}

	return true;
}

static bool user_cache_validate_store_array_value(zval *value, uint32_t arg_num)
{
	return user_cache_validate_arg_value_kind(
		value,
		arg_num,
		"must contain only values of type " PHP_USER_CACHE_API_VALUE_TYPE ", resource given",
		"must not contain Closure objects"
	);
}

static bool user_cache_validate_store_array(HashTable *values, uint32_t arg_num)
{
	zend_string *key;
	zval *value;

	ZEND_HASH_FOREACH_STR_KEY_VAL(values, key, value) {
		if (key != NULL && !user_cache_user_key_is_valid(key)) {
			zend_argument_value_error(arg_num, "must be an array with non-empty string or int keys that do not contain " PHP_USER_CACHE_KEY_DELIMITER_NAME);

			return false;
		}

		if (!user_cache_validate_store_array_value(value, arg_num)) {
			return false;
		}
	} ZEND_HASH_FOREACH_END();

	return true;
}

static void user_cache_release_key_list(zend_string **keys, uint32_t count)
{
	uint32_t i;

	if (keys == NULL) {
		return;
	}

	for (i = 0; i < count; i++) {
		zend_string_release(keys[i]);
	}

	efree(keys);
}

static bool user_cache_prepare_key_list(
		HashTable *keys,
		uint32_t arg_num,
		zend_string ***prepared_keys,
		uint32_t *prepared_count)
{
	zend_string **prepared;
	zval *value;
	uint32_t count, i = 0;

	ZEND_ASSERT(prepared_keys != NULL);
	ZEND_ASSERT(prepared_count != NULL);

	count = zend_hash_num_elements(keys);

	*prepared_keys = NULL;
	*prepared_count = 0;

	if (count == 0) {
		return true;
	}

	prepared = safe_emalloc(count, sizeof(zend_string *), 0);

	ZEND_HASH_FOREACH_VAL(keys, value) {
		ZVAL_DEREF(value);

		if (Z_TYPE_P(value) == IS_STRING) {
			if (!user_cache_user_key_is_valid(Z_STR_P(value))) {
				zend_argument_value_error(arg_num, "must contain only non-empty string or int cache keys that do not contain " PHP_USER_CACHE_KEY_DELIMITER_NAME);
				user_cache_release_key_list(prepared, i);

				return false;
			}

			prepared[i++] = zend_string_copy(Z_STR_P(value));
		} else if (Z_TYPE_P(value) == IS_LONG) {
			prepared[i++] = zend_long_to_str(Z_LVAL_P(value));
		} else {
			zend_argument_value_error(arg_num, "must contain only non-empty string or int cache keys that do not contain " PHP_USER_CACHE_KEY_DELIMITER_NAME);
			user_cache_release_key_list(prepared, i);

			return false;
		}
	} ZEND_HASH_FOREACH_END();

	*prepared_keys = prepared;
	*prepared_count = i;

	return true;
}

static bool user_cache_validate_api_value(zval *value, uint32_t arg_num)
{
	return user_cache_validate_arg_value_kind(
		value,
		arg_num,
		"must be of type " PHP_USER_CACHE_API_VALUE_TYPE ", resource given",
		"must not be a Closure object"
	);
}

static bool user_cache_validate_remember_value(zval *value)
{
	ZVAL_DEREF(value);

	if (Z_TYPE_P(value) == IS_RESOURCE) {
		zend_type_error(PHP_USER_CACHE_MSG_RESOURCE_UNSTORABLE);

		return false;
	}

	if (Z_TYPE_P(value) == IS_OBJECT && Z_OBJCE_P(value) == zend_ce_closure) {
		zend_type_error(PHP_USER_CACHE_MSG_CLOSURE_UNSTORABLE);

		return false;
	}

	return true;
}

static zend_string *user_cache_build_scope_prefix(zend_string *scope)
{
	const char *prefix = "user_cache" PHP_USER_CACHE_KEY_DELIMITER;
	size_t prefix_len = sizeof("user_cache" PHP_USER_CACHE_KEY_DELIMITER) - 1;

	return zend_string_concat3(
		prefix,
		prefix_len,
		ZSTR_VAL(scope),
		ZSTR_LEN(scope),
		ZEND_STRL(PHP_USER_CACHE_KEY_DELIMITER)
	);
}

PHP_USER_CACHE_DEFINE_OBJ_FROM_STD(php_user_cache_object)

PHP_USER_CACHE_DEFINE_OBJ_FROM_STD(php_user_cache_pool_status_object)

PHP_USER_CACHE_DEFINE_OBJ_FROM_STD(php_user_cache_status_object)

static php_user_cache_object *user_cache_object_from_this(zval *this_ptr)
{
	php_user_cache_object *cache = php_user_cache_object_from_obj(Z_OBJ_P(this_ptr));

	ZEND_ASSERT(cache->context == php_user_cache_active_context());

	if (cache->scope_prefix == NULL) {
		zend_throw_error(NULL, "UserCache\\Cache instance was not initialized");

		return NULL;
	}

	return cache;
}

static php_user_cache_pool_status_object *user_cache_pool_status_from_this(zval *this_ptr)
{
	php_user_cache_pool_status_object *status =
		php_user_cache_pool_status_object_from_obj(Z_OBJ_P(this_ptr))
	;

	if (status->scope == NULL || status->scope_prefix == NULL || status->context == NULL) {
		zend_throw_error(NULL, "UserCache\\CachePoolStatus instance was not initialized");

		return NULL;
	}

	return status;
}

static php_user_cache_status_object *user_cache_status_from_this(zval *this_ptr)
{
	php_user_cache_status_object *status =
		php_user_cache_status_object_from_obj(Z_OBJ_P(this_ptr))
	;

	if (!status->initialized) {
		zend_throw_error(NULL, "UserCache\\CacheStatus instance was not initialized");

		return NULL;
	}

	return status;
}

static void user_cache_register_classes(void)
{
	if (php_user_cache_ce != NULL) {
		return;
	}

	php_user_cache_exception_ce = zend_ce_exception;
	php_user_cache_availability_ce = register_class_UserCache_CacheAvailability();
	php_user_cache_status_ce = register_class_UserCache_CacheStatus();
	php_user_cache_pool_status_ce = register_class_UserCache_CachePoolStatus();
	php_user_cache_ce = register_class_UserCache_Cache();

	php_user_cache_ce->create_object = user_cache_object_create;
	php_user_cache_status_ce->create_object = user_cache_status_object_create;
	php_user_cache_pool_status_ce->create_object = user_cache_pool_status_object_create;

	memcpy(
		&php_user_cache_object_handlers,
		zend_get_std_object_handlers(),
		sizeof(zend_object_handlers)
	);

	php_user_cache_object_handlers.offset = offsetof(php_user_cache_object, std);
	php_user_cache_object_handlers.free_obj = user_cache_object_free;
	php_user_cache_object_handlers.clone_obj = NULL;

	memcpy(
		&php_user_cache_status_object_handlers,
		zend_get_std_object_handlers(),
		sizeof(zend_object_handlers)
	);

	php_user_cache_status_object_handlers.offset = offsetof(php_user_cache_status_object, std);
	php_user_cache_status_object_handlers.clone_obj = NULL;

	memcpy(
		&php_user_cache_pool_status_object_handlers,
		zend_get_std_object_handlers(),
		sizeof(zend_object_handlers)
	);

	php_user_cache_pool_status_object_handlers.offset = offsetof(php_user_cache_pool_status_object, std);
	php_user_cache_pool_status_object_handlers.free_obj = user_cache_pool_status_object_free;
	php_user_cache_pool_status_object_handlers.clone_obj = NULL;
}

static void user_cache_reset_class_entries(void)
{
	php_user_cache_exception_ce = NULL;
	php_user_cache_availability_ce = NULL;
	php_user_cache_status_ce = NULL;
	php_user_cache_pool_status_ce = NULL;
	php_user_cache_ce = NULL;
}

static bool user_cache_validate_non_negative(zend_long value, uint32_t arg_num)
{
	if (value < 0) {
		zend_argument_value_error(arg_num, "must be greater than or equal to 0");

		return false;
	}

	return true;
}

static bool user_cache_can_read(void)
{
	php_user_cache_ensure_ready();

	return php_user_cache_active_runtime()->available;
}

/* On success the read lock is held and the header is initialized; on any
 * failure no lock is held. */
static bool user_cache_begin_read(void)
{
	if (!user_cache_can_read()) {
		return false;
	}

	if (!php_user_cache_rlock()) {
		return false;
	}

	if (!php_user_cache_header_is_initialized_locked()) {
		php_user_cache_unlock();

		return false;
	}

	return true;
}

/* Store readiness matches read readiness; per-entry and global write locks
 * are acquired separately. */
static bool user_cache_can_write(void)
{
	return user_cache_can_read();
}

/* On success the write lock is held and the header is initialized; on failure
 * no lock is held. */
static bool user_cache_begin_write(void)
{
	if (!php_user_cache_wlock()) {
		return false;
	}

	if (!php_user_cache_header_init_locked()) {
		php_user_cache_unlock();

		return false;
	}

	return true;
}

static bool user_cache_delete_storage_key_prevalidated(zend_string *key)
{
	if (!php_user_cache_wlock_for_entry_mutation(key)) {
		return false;
	}

	php_user_cache_delete_locked(key);

	php_user_cache_unlock();

	return true;
}

static void user_cache_delete_corrupt_entry_prevalidated(zend_string *key)
{
	if (EG(exception)) {
		return;
	}

	(void) user_cache_delete_storage_key_prevalidated(key);
}

static bool user_cache_clear_scope_prevalidated(
		zend_string *scope_prefix)
{
	if (!user_cache_begin_write()) {
		return false;
	}

	php_user_cache_delete_by_prefix_locked(scope_prefix);

	php_user_cache_unlock();
	php_user_cache_release_active_request_local_slots_by_prefix(scope_prefix);

	return true;
}

static bool user_cache_fetch_if_present_api(
		zend_string *key,
		zval *return_value)
{
	php_user_cache_fetch_pending_seed pending_seed;
	bool fetched, entry_found = false;

	if (!user_cache_can_read()) {
		return false;
	}

	UC_G(stack_overflowed) = false;
	switch (php_user_cache_fetch_optimistic(key, return_value)) {
		case PHP_USER_CACHE_OPTIMISTIC_FOUND:
			return true;
		case PHP_USER_CACHE_OPTIMISTIC_MISS:
			return false;
		case PHP_USER_CACHE_OPTIMISTIC_FALLBACK:
			break;
	}

	if (EG(exception)) {
		/* A stack overflow in the lock-free path is not a real error and would
		 * recur under the read lock, so surface a miss and drop the exception. */
		if (UC_G(stack_overflowed)) {
			UC_G(stack_overflowed) = false;
			zend_clear_exception();

			return false;
		}

		return false;
	}

	if (!user_cache_begin_read()) {
		return false;
	}

	UC_G(stack_overflowed) = false;
	fetched = php_user_cache_fetch_locked(key, false, true, return_value, &entry_found, &pending_seed);

	php_user_cache_unlock();

	if (fetched) {
		/* Clone outside the shared-memory lock. */
		if (pending_seed.should_seed_request_local_slot) {
			php_user_cache_fetch_finish(key, pending_seed.generation, return_value, pending_seed.flags);
		}

		return true;
	}

	if (!entry_found) {
		return false;
	}

	if (UC_G(stack_overflowed)) {
		UC_G(stack_overflowed) = false;
		if (EG(exception)) {
			zend_clear_exception();
		}

		return false;
	}

	if (EG(exception)) {
		return false;
	}

	/* Decode failed without an exception: the entry is corrupt, drop it. */
	user_cache_delete_corrupt_entry_prevalidated(key);

	return false;
}

static void user_cache_fetch_api(
		zend_string *key,
		zval *default_value,
		zval *return_value)
{
	if (!user_cache_fetch_if_present_api(key, return_value)) {
		ZVAL_COPY(return_value, default_value);
	}
}

static void user_cache_fetch_multiple_fill_defaults(
		zend_string **prepared_keys,
		zend_string **storage_keys,
		uint32_t count,
		zval *default_value,
		zval *return_value)
{
	zval default_copy;
	uint32_t i;

	for (i = 0; i < count; i++) {
		ZVAL_COPY(&default_copy, default_value);
		zend_symtable_update(Z_ARRVAL_P(return_value), prepared_keys[i], &default_copy);
		zend_string_release(storage_keys[i]);
	}
}

/* A corrupt entry drops the batch lock and switches subsequent reads to the
 * self-locking path. */
static void user_cache_fetch_multiple_fetch_one(
		zend_string *storage_key,
		zval *default_value,
		bool *rlock_held,
		zval *out,
		php_user_cache_fetch_pending_seed *pending_seed)
{
	bool fetched, found;

	pending_seed->should_seed_request_local_slot = false;

	if (!*rlock_held) {
		user_cache_fetch_api(storage_key, default_value, out);

		return;
	}

	UC_G(stack_overflowed) = false;
	fetched = php_user_cache_fetch_locked(storage_key, false, true, out, &found, pending_seed);
	if (fetched) {
		return;
	}

	if (!found) {
		ZVAL_COPY(out, default_value);
	} else if (UC_G(stack_overflowed)) {
		UC_G(stack_overflowed) = false;

		if (EG(exception)) {
			zend_clear_exception();
		}

		ZVAL_COPY(out, default_value);
	} else if (EG(exception)) {
		/* A restore hook or autoloader threw: propagate the userland exception
		 * and keep the entry. Drop the batch lock so the caller can unwind. */
		php_user_cache_unlock();
		*rlock_held = false;

		ZVAL_UNDEF(out);
	} else {
		php_user_cache_unlock();
		*rlock_held = false;
		user_cache_delete_corrupt_entry_prevalidated(storage_key);

		ZVAL_COPY(out, default_value);
	}
}

static void user_cache_finish_fetch_multiple(
		zend_string **prepared_keys,
		zend_string **storage_keys,
		const php_user_cache_fetch_pending_seed *pending_seeds,
		uint32_t count,
		zval *return_value)
{
	zval *val;
	uint32_t i;

	for (i = 0; i < count; i++) {
		if (pending_seeds[i].should_seed_request_local_slot) {
			val = zend_symtable_find(Z_ARRVAL_P(return_value), prepared_keys[i]);
			if (val != NULL) {
				php_user_cache_fetch_finish(
					storage_keys[i],
					pending_seeds[i].generation,
					val,
					pending_seeds[i].flags
				);
			}
		}

		zend_string_release(storage_keys[i]);
	}
}

static zend_string *user_cache_storage_key_cache_lookup(
		php_user_cache_object *cache,
		zend_string *key)
{
	zval *cached;

	if (cache->storage_key_cache == NULL) {
		return NULL;
	}

	cached = zend_hash_find(cache->storage_key_cache, key);
	if (cached == NULL) {
		return NULL;
	}

	return zend_string_copy(Z_STR_P(cached));
}

static zend_string *user_cache_storage_key_build(
		php_user_cache_object *cache,
		zend_string *key)
{
	zend_string *storage_key;
	zval cache_zv;

	storage_key = zend_string_concat2(
		ZSTR_VAL(cache->scope_prefix),
		ZSTR_LEN(cache->scope_prefix),
		ZSTR_VAL(key),
		ZSTR_LEN(key)
	);

	zend_string_hash_val(storage_key);

	if (cache->storage_key_cache == NULL) {
		ALLOC_HASHTABLE(cache->storage_key_cache);
		zend_hash_init(cache->storage_key_cache, 8, NULL, ZVAL_PTR_DTOR, 0);
	}

	if (zend_hash_num_elements(cache->storage_key_cache) < PHP_USER_CACHE_STORAGE_KEY_CACHE_MAX) {
		ZVAL_STR_COPY(&cache_zv, storage_key);
		zend_hash_add(cache->storage_key_cache, key, &cache_zv);
	}

	return storage_key;
}

static zend_string *user_cache_storage_key(
		php_user_cache_object *cache,
		zend_string *key)
{
	zend_string *storage_key = user_cache_storage_key_cache_lookup(cache, key);

	if (storage_key != NULL) {
		return storage_key;
	}

	return user_cache_storage_key_build(cache, key);
}

static zend_string *user_cache_validated_storage_key(
		php_user_cache_object *cache,
		zend_string *key,
		uint32_t arg_num)
{
	zend_string *storage_key = user_cache_storage_key_cache_lookup(cache, key);

	if (storage_key != NULL) {
		return storage_key;
	}

	if (!user_cache_validate_key(key, arg_num)) {
		return NULL;
	}

	return user_cache_storage_key_build(cache, key);
}

static zend_result user_cache_fetch_multiple_api(
		php_user_cache_object *cache,
		HashTable *keys,
		zval *default_value,
		zval *return_value)
{
	php_user_cache_fetch_pending_seed *pending_seeds;
	zend_string **prepared_keys, **storage_keys, *key, *storage_key;
	zval val;
	uint32_t count, i;
	bool rlock_held;

	if (!user_cache_prepare_key_list(keys, 1, &prepared_keys, &count)) {
		return FAILURE;
	}

	/* Do not allocate while holding the read lock. */
	storage_keys = NULL;
	pending_seeds = NULL;
	if (count != 0) {
		storage_keys = safe_emalloc(count, sizeof(zend_string *), 0);
		pending_seeds = safe_emalloc(count, sizeof(php_user_cache_fetch_pending_seed), 0);

		for (i = 0; i < count; i++) {
			storage_keys[i] = user_cache_storage_key(cache, prepared_keys[i]);
		}
	}

	array_init_size(return_value, count);

	if (!user_cache_begin_read()) {
		user_cache_fetch_multiple_fill_defaults(
			prepared_keys,
			storage_keys,
			count,
			default_value,
			return_value
		);

		if (storage_keys != NULL) {
			efree(storage_keys);
		}

		if (pending_seeds != NULL) {
			efree(pending_seeds);
		}

		user_cache_release_key_list(prepared_keys, count);

		return SUCCESS;
	}

	rlock_held = true;

	zend_try {
		for (i = 0; i < count; i++) {
			key = prepared_keys[i];
			storage_key = storage_keys[i];

			user_cache_fetch_multiple_fetch_one(
				storage_key,
				default_value,
				&rlock_held,
				&val,
				&pending_seeds[i]
			);

			/* A restore hook or autoloader threw: abort the batch and let the
			 * userland exception propagate, matching native unserialize(). */
			if (EG(exception)) {
				zval_ptr_dtor(&val);

				break;
			}

			zend_symtable_update(Z_ARRVAL_P(return_value), key, &val);
		}
	} zend_catch {
		php_user_cache_unlock_if_held();
		zend_bailout();
	} zend_end_try();

	if (rlock_held) {
		php_user_cache_unlock();
	}

	if (EG(exception)) {
		zval_ptr_dtor(return_value);
		ZVAL_UNDEF(return_value);

		for (i = 0; i < count; i++) {
			zend_string_release(storage_keys[i]);
		}

		if (storage_keys != NULL) {
			efree(storage_keys);
		}

		if (pending_seeds != NULL) {
			efree(pending_seeds);
		}

		user_cache_release_key_list(prepared_keys, count);

		return FAILURE;
	}

	/* Clone outside the shared-memory lock. */
	user_cache_finish_fetch_multiple(prepared_keys, storage_keys, pending_seeds, count, return_value);

	if (storage_keys != NULL) {
		efree(storage_keys);
	}

	if (pending_seeds != NULL) {
		efree(pending_seeds);
	}

	user_cache_release_key_list(prepared_keys, count);

	return SUCCESS;
}

static bool user_cache_atomic_update_api(
		zend_string *key,
		zend_long step,
		zend_long ttl,
		bool decrement,
		php_user_cache_atomic_update_result *result)
{
	bool updated;

	if (!user_cache_can_write() ||
		!php_user_cache_wlock_for_entry_mutation(key)
	) {
		memset(result, 0, sizeof(*result));

		return false;
	}

	updated = php_user_cache_atomic_update_locked(key, step, ttl, decrement, true, result);

	php_user_cache_unlock();

	if (result->is_overflow || result->is_type_error) {
		updated = false;
	}

	return updated;
}

static bool user_cache_exists_api(zend_string *key)
{
	bool exists;

	if (!user_cache_can_read()) {
		return false;
	}

	switch (php_user_cache_exists_optimistic(key)) {
		case PHP_USER_CACHE_OPTIMISTIC_FOUND:
			return true;
		case PHP_USER_CACHE_OPTIMISTIC_MISS:
			return false;
		case PHP_USER_CACHE_OPTIMISTIC_FALLBACK:
			break;
	}

	if (!user_cache_begin_read()) {
		return false;
	}

	exists = php_user_cache_exists_locked(key);

	php_user_cache_unlock();

	return exists;
}

static bool user_cache_lock_api(
		zend_string *key,
		zend_long lease)
{
	if (!user_cache_can_write()) {
		return false;
	}

	return php_user_cache_try_acquire_entry_lock(key, lease);
}

static bool user_cache_unlock_api(zend_string *key)
{
	if (!user_cache_can_write()) {
		return false;
	}

	return php_user_cache_release_entry_lock(key);
}

static zend_long user_cache_count_to_zend_long(uint64_t count)
{
	return count > (uint64_t) ZEND_LONG_MAX ? ZEND_LONG_MAX : (zend_long) count;
}

static zend_long user_cache_size_to_zend_long(size_t size)
{
	return user_cache_count_to_zend_long(size);
}

/* Caller must hold the read lock. */
static size_t user_cache_sum_wasted_memory_locked(php_user_cache_header *header)
{
	php_user_cache_block *block;
	uint32_t block_offset, block_limit, next_offset, iter_limit, iters;
	size_t wasted_memory = 0;

	block_limit = header->data_offset + header->next_free;
	iter_limit = (header->data_size / ((uint32_t) sizeof(php_user_cache_block)));
	for (block_offset = header->free_list, iters = 0;
		block_offset != 0 && iters < iter_limit;
		iters++
	) {
		if (block_offset < header->data_offset || block_offset > block_limit - sizeof(php_user_cache_block)) {
			break;
		}

		block = php_user_cache_block_ptr(block_offset);
		next_offset = block_offset + block->size;
		if (block->size < sizeof(php_user_cache_block) || next_offset > block_limit) {
			break;
		}

		wasted_memory += block->size;
		block_offset = block->next_free;
	}

	return wasted_memory;
}

static void user_cache_collect_info_stats(php_user_cache_info_stats *stats)
{
	php_user_cache_runtime *runtime;
	php_user_cache_storage *storage;
	php_user_cache_header *header;
	php_user_cache_graph_pin_slot *pin_slot;
	size_t free_memory = 0, wasted_memory = 0, tail_memory = 0;
	uint32_t i;

	memset(stats, 0, sizeof(*stats));

	runtime = php_user_cache_active_runtime();
	storage = &php_user_cache_active_context()->storage;

	stats->configured_memory = user_cache_size_to_zend_long(runtime->configured_memory);
	stats->shared_memory_size = user_cache_size_to_zend_long(storage->size);

	if (!storage->initialized ||
		!storage->lock_initialized ||
		!php_user_cache_rlock()
	) {
		return;
	}

	header = php_user_cache_header_ptr();
	if (header == NULL || !php_user_cache_header_is_initialized_locked()) {
		stats->free_memory = stats->shared_memory_size;
		php_user_cache_unlock();

		return;
	}

	if (header->next_free <= header->data_size) {
		tail_memory = header->data_size - header->next_free;
	}

	wasted_memory = user_cache_sum_wasted_memory_locked(header);
	free_memory = tail_memory + wasted_memory;

	stats->entry_count = (zend_long) header->count;
	stats->entry_capacity = (zend_long) header->capacity;
	stats->tombstone_count = (zend_long) header->tombstone_count;
	stats->expunge_count = user_cache_count_to_zend_long(header->expunge_count);
	stats->store_failure_count = user_cache_count_to_zend_long(header->store_failure_count);
	stats->dead_pin_owners_reclaimed = user_cache_count_to_zend_long(header->graph_dead_pin_owners_reclaimed);
	stats->dead_pins_stripped = user_cache_count_to_zend_long(header->graph_dead_pins_stripped);

	for (i = 0; i < PHP_USER_CACHE_GRAPH_PIN_SLOTS; i++) {
		pin_slot = &header->graph_pin_slots[i];

		if (zend_atomic_int_load_ex(&pin_slot->owner_pid) != 0) {
			stats->graph_pin_slots_in_use++;
		}

		stats->graph_pinned_references +=
			(zend_long) zend_atomic_int_load_ex(&pin_slot->pin_count);
	}

	stats->free_memory = user_cache_size_to_zend_long(free_memory);
	stats->wasted_memory = user_cache_size_to_zend_long(wasted_memory);
	stats->used_memory = storage->size > free_memory
		? user_cache_size_to_zend_long(storage->size - free_memory)
		: 0
	;

	php_user_cache_unlock();
}

/* No default: the compiler should diagnose a missing enum case. */
static zend_object *user_cache_availability_enum_case(php_user_cache_reason reason)
{
	int case_id = ZEND_ENUM_UserCache_CacheAvailability_UnavailableByUnknownReason;

	switch (reason) {
		case PHP_USER_CACHE_REASON_NONE:
			case_id = ZEND_ENUM_UserCache_CacheAvailability_Available;
			break;
		case PHP_USER_CACHE_REASON_DISABLED_BY_INI:
			case_id = ZEND_ENUM_UserCache_CacheAvailability_DisabledByIni;
			break;
		case PHP_USER_CACHE_REASON_SHM_INIT_FAILED:
			case_id = ZEND_ENUM_UserCache_CacheAvailability_UnavailableBySharedMemoryInitializationFailed;
			break;
		case PHP_USER_CACHE_REASON_SAPI_NOT_ENABLED:
			case_id = ZEND_ENUM_UserCache_CacheAvailability_DisabledBySapi;
			break;
		case PHP_USER_CACHE_REASON_BACKEND_NOT_INITIALIZED_BEFORE_WORKER:
			case_id = ZEND_ENUM_UserCache_CacheAvailability_UnavailableByBackendNotInitializedBeforeWorkerStartup;
			break;
		case PHP_USER_CACHE_REASON_BACKEND_INITIALIZED_AFTER_WORKER:
			case_id = ZEND_ENUM_UserCache_CacheAvailability_UnavailableByBackendInitializedAfterWorkerStartup;
			break;
		case PHP_USER_CACHE_REASON_CGI_BOUNDARY_UNAVAILABLE:
			case_id = ZEND_ENUM_UserCache_CacheAvailability_UnavailableByCgiFastCgiBoundary;
			break;
		case PHP_USER_CACHE_REASON_LSAPI_BOUNDARY_UNAVAILABLE:
			case_id = ZEND_ENUM_UserCache_CacheAvailability_UnavailableByLsapiBoundary;
			break;
	}

	return zend_enum_get_case_by_id(php_user_cache_availability_ce, case_id);
}

static void user_cache_return_status(zval *return_value)
{
	php_user_cache_status_object *status;

	php_user_cache_ensure_ready();

	object_init_ex(return_value, php_user_cache_status_ce);
	status = php_user_cache_status_object_from_obj(Z_OBJ_P(return_value));

	status->availability_reason = php_user_cache_active_runtime()->failure_reason;
	user_cache_collect_info_stats(&status->stats);

	status->initialized = true;
}

static void user_cache_add_pool_memory(size_t *used_memory, size_t size)
{
	if (*used_memory >= (size_t) ZEND_LONG_MAX ||
		size > (size_t) ZEND_LONG_MAX - *used_memory
	) {
		*used_memory = (size_t) ZEND_LONG_MAX;

		return;
	}

	*used_memory += size;
}

static size_t user_cache_payload_block_size(uint32_t payload_offset)
{
	php_user_cache_block *block;

	if (payload_offset < sizeof(php_user_cache_block)) {
		return 0;
	}

	block = php_user_cache_block_ptr(payload_offset - sizeof(php_user_cache_block));
	if (block->size < sizeof(php_user_cache_block)) {
		return 0;
	}

	return block->size;
}

static void user_cache_account_pool_entry(
		const php_user_cache_entry *entry,
		zend_string *prefix,
		zend_long *entry_count,
		size_t *used_memory,
		zval *entry_keys)
{
	const char *key;
	size_t prefix_len;
	bool combined_value_key;

	combined_value_key = (entry->reserved & PHP_USER_CACHE_ENTRY_RESERVED_COMBINED_VALUE_KEY) != 0;
	key = (const char *) php_user_cache_ptr(entry->key_offset);
	prefix_len = ZSTR_LEN(prefix);

	(*entry_count)++;

	add_next_index_stringl(entry_keys, key + prefix_len, entry->key_len - prefix_len);

	user_cache_add_pool_memory(used_memory, sizeof(php_user_cache_entry));
	if (combined_value_key) {
		user_cache_add_pool_memory(
			used_memory,
			user_cache_payload_block_size(entry->value_offset)
		);
	} else {
		user_cache_add_pool_memory(
			used_memory,
			user_cache_payload_block_size(entry->key_offset)
		);
		user_cache_add_pool_memory(
			used_memory,
			user_cache_payload_block_size(entry->value_offset)
		);
	}
}

static void user_cache_collect_pool_status(
		php_user_cache_context *ctx,
		zend_string *prefix,
		zend_long *entry_count,
		zend_long *used_memory,
		zval *entry_keys)
{
	php_user_cache_context *prev_ctx;
	php_user_cache_storage *storage;
	php_user_cache_header *header;
	php_user_cache_entry *entries, *entry;
	uint32_t i;
	size_t used_size = 0;

	*entry_count = 0;
	*used_memory = 0;

	prev_ctx = php_user_cache_activate_context(ctx);

	storage = &php_user_cache_active_context()->storage;
	if (!storage->initialized || !storage->lock_initialized || !php_user_cache_rlock()) {
		php_user_cache_restore_context(prev_ctx);

		return;
	}

	header = php_user_cache_header_ptr();
	if (header == NULL ||
		!php_user_cache_header_is_initialized_locked()
	) {
		php_user_cache_unlock();
		php_user_cache_restore_context(prev_ctx);

		return;
	}

	entries = php_user_cache_entries_ptr(header);

	/* Release the lock before propagating an allocation bailout. */
	zend_try {
		for (i = 0; i < header->capacity; i++) {
			entry = &entries[i];
			if (entry->state == PHP_USER_CACHE_ENTRY_USED &&
				entry->key_len >= ZSTR_LEN(prefix) &&
				memcmp(php_user_cache_ptr(entry->key_offset), ZSTR_VAL(prefix), ZSTR_LEN(prefix)) == 0
			) {
				user_cache_account_pool_entry(
					entry,
					prefix,
					entry_count,
					&used_size,
					entry_keys
				);
			}
		}
	} zend_catch {
		php_user_cache_unlock_if_held();
		php_user_cache_restore_context(prev_ctx);
		zend_bailout();
	} zend_end_try();

	php_user_cache_unlock();
	php_user_cache_restore_context(prev_ctx);

	*used_memory = user_cache_size_to_zend_long(used_size);
}

static void user_cache_return_pool_status(
		php_user_cache_object *cache,
		zval *return_value)
{
	php_user_cache_pool_status_object *status;

	php_user_cache_ensure_ready();

	object_init_ex(return_value, php_user_cache_pool_status_ce);

	status = php_user_cache_pool_status_object_from_obj(Z_OBJ_P(return_value));
	status->scope = zend_string_copy(cache->scope);
	status->scope_prefix = zend_string_copy(cache->scope_prefix);
	status->context = cache->context;

	array_init(&status->entry_keys);

	user_cache_collect_pool_status(
		status->context,
		status->scope_prefix,
		&status->entry_count,
		&status->used_memory,
		&status->entry_keys
	);
}

static void user_cache_object_free(zend_object *obj)
{
	php_user_cache_object *cache = php_user_cache_object_from_obj(obj);

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

static void user_cache_pool_status_object_free(zend_object *obj)
{
	php_user_cache_pool_status_object *status =
		php_user_cache_pool_status_object_from_obj(obj)
	;

	if (status->scope != NULL) {
		zend_string_release(status->scope);
	}

	if (status->scope_prefix != NULL) {
		zend_string_release(status->scope_prefix);
	}

	zval_ptr_dtor(&status->entry_keys);

	zend_object_std_dtor(&status->std);
}

static zend_object *user_cache_object_create(zend_class_entry *ce)
{
	php_user_cache_object *cache;

	cache = zend_object_alloc(sizeof(php_user_cache_object), ce);

	zend_object_std_init(&cache->std, ce);
	object_properties_init(&cache->std, ce);

	cache->scope = NULL;
	cache->scope_prefix = NULL;
	cache->storage_key_cache = NULL;
	cache->context = php_user_cache_owning_context();
	cache->std.handlers = &php_user_cache_object_handlers;

	return &cache->std;
}

static zend_object *user_cache_pool_status_object_create(zend_class_entry *ce)
{
	php_user_cache_pool_status_object *status;

	status = zend_object_alloc(sizeof(php_user_cache_pool_status_object), ce);

	zend_object_std_init(&status->std, ce);
	object_properties_init(&status->std, ce);

	status->scope = NULL;
	status->scope_prefix = NULL;
	status->context = NULL;
	status->entry_count = 0;
	status->used_memory = 0;

	ZVAL_UNDEF(&status->entry_keys);

	status->std.handlers = &php_user_cache_pool_status_object_handlers;

	return &status->std;
}

static zend_object *user_cache_status_object_create(zend_class_entry *ce)
{
	php_user_cache_status_object *status;

	status = zend_object_alloc(sizeof(php_user_cache_status_object), ce);

	zend_object_std_init(&status->std, ce);
	object_properties_init(&status->std, ce);

	memset(&status->stats, 0, sizeof(status->stats));

	status->availability_reason = PHP_USER_CACHE_REASON_NONE;
	status->initialized = false;
	status->std.handlers = &php_user_cache_status_object_handlers;

	return &status->std;
}

static HashTable *user_cache_pools(void)
{
	if (UC_G(pool_table) == NULL) {
		ALLOC_HASHTABLE(UC_G(pool_table));
		zend_hash_init(UC_G(pool_table), 8, NULL, ZVAL_PTR_DTOR, 0);
	}

	return UC_G(pool_table);
}

static void user_cache_release_pools(void)
{
	if (UC_G(pool_table) == NULL) {
		return;
	}

	zend_hash_destroy(UC_G(pool_table));
	FREE_HASHTABLE(UC_G(pool_table));

	UC_G(pool_table) = NULL;
}

static bool user_cache_validate_pool_name(zend_string *pool, uint32_t arg_num)
{
	return user_cache_validate_delimiter_free(pool, arg_num, "must not be empty");
}

static zend_object *user_cache_get_or_create_pool(zend_string *pool)
{
	php_user_cache_object *cache;
	zend_object *obj;
	zval *existing, pool_zv;
	HashTable *pools = user_cache_pools();

	existing = zend_hash_find(pools, pool);
	if (existing != NULL) {
		return Z_OBJ_P(existing);
	}

	obj = user_cache_object_create(php_user_cache_ce);

	cache = php_user_cache_object_from_obj(obj);
	cache->scope = zend_string_copy(pool);
	cache->scope_prefix = user_cache_build_scope_prefix(pool);

	ZVAL_OBJ(&pool_zv, obj);

	if (zend_hash_add(pools, pool, &pool_zv) == NULL) {
		OBJ_RELEASE(obj);

		return NULL;
	}

	return obj;
}

static void user_cache_safe_direct_handlers_dtor(zval *zv)
{
	pefree(Z_PTR_P(zv), true);
}

const php_user_cache_safe_direct_handlers *php_user_cache_safe_direct_find_handlers(
		zend_class_entry *ce,
		zend_class_entry **base_ce_ptr)
{
	const php_user_cache_safe_direct_handlers *handlers;

	if (!user_cache_safe_direct_handlers_initialized) {
		return NULL;
	}

	while (ce != NULL) {
		handlers = zend_hash_index_find_ptr(
			&php_user_cache_safe_direct_handler_table,
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

static bool user_cache_store_storage_key_prevalidated(zend_string *key, zval *value, zend_long ttl, bool add_only)
{
	php_user_cache_prepare_options prep_opts = {
		.caller_holds_write_lock = false,
		.throw_on_failure = false,
	};
	php_user_cache_store_options store_opts = {
		.retry_after_memory_pressure = true,
		.throw_on_failure = false,
		.capture_replaced_entry = false,
	};
	php_user_cache_prepared_value prepared;
	php_user_cache_store_result result;
	bool stored;

	if (!php_user_cache_prepare_value(key, value, &prep_opts, &prepared)) {
		php_user_cache_destroy_prepared_value(&prepared);

		return false;
	}

	if (!php_user_cache_wlock_for_entry_mutation(key)) {
		php_user_cache_destroy_prepared_value(&prepared);

		return false;
	}

	if (add_only && php_user_cache_exists_locked(key)) {
		php_user_cache_unlock();
		php_user_cache_destroy_prepared_value(&prepared);

		return false;
	}

	zend_try {
		stored = php_user_cache_store_prepared_locked(
			key,
			value,
			&prepared,
			ttl,
			&store_opts,
			&result
		);
	} zend_catch {
		php_user_cache_unlock_if_held();
		php_user_cache_destroy_prepared_value(&prepared);

		zend_bailout();
	} zend_end_try();

	php_user_cache_unlock();
	php_user_cache_destroy_prepared_value(&prepared);

	if (stored && result.should_seed_request_local_slot) {
		php_user_cache_store_request_local_slot(key, result.stored_generation, value, true);
	}

	return stored;
}

/* Entry locks must be acquired in this order. */
static int user_cache_bulk_order_compare(const void *lhs_ptr, const void *rhs_ptr)
{
	const php_user_cache_bulk_order *lhs, *rhs;
	size_t lhs_len, rhs_len, cmp_len;
	int result;

	lhs = lhs_ptr;
	rhs = rhs_ptr;

	if (lhs->hash != rhs->hash) {
		return lhs->hash < rhs->hash ? -1 : 1;
	}

	lhs_len = ZSTR_LEN(lhs->key);
	rhs_len = ZSTR_LEN(rhs->key);
	cmp_len = lhs_len < rhs_len ? lhs_len : rhs_len;

	result = memcmp(ZSTR_VAL(lhs->key), ZSTR_VAL(rhs->key), cmp_len);
	if (result != 0) {
		return result < 0 ? -1 : 1;
	}

	if (lhs_len != rhs_len) {
		return lhs_len < rhs_len ? -1 : 1;
	}

	return lhs->index < rhs->index ? -1 : (lhs->index > rhs->index ? 1 : 0);
}

/* Acquires locks in bulk order and records them by original index. */
static bool user_cache_bulk_lock_keys_ordered(
		zend_string **storage_keys,
		php_user_cache_bulk_order *order,
		bool *acquired,
		uint32_t count)
{
	zend_string **sorted_keys;
	uint32_t i;
	bool *sorted_acquired, result;

	for (i = 0; i < count; i++) {
		order[i].hash = zend_string_hash_val(storage_keys[i]);
		order[i].key = storage_keys[i];
		order[i].index = i;
	}

	qsort(order, count, sizeof(*order), user_cache_bulk_order_compare);

	sorted_keys = safe_emalloc(count, sizeof(*sorted_keys), 0);
	sorted_acquired = safe_emalloc(count, sizeof(*sorted_acquired), 0);

	for (i = 0; i < count; i++) {
		sorted_keys[i] = storage_keys[order[i].index];
	}

	result = php_user_cache_acquire_entry_locks(sorted_keys, sorted_acquired, count);

	for (i = 0; i < count; i++) {
		acquired[order[i].index] = sorted_acquired[i];
	}

	efree(sorted_acquired);
	efree(sorted_keys);

	return result;
}

static uint32_t user_cache_prepare_bulk_store_items(
		php_user_cache_object *cache,
		HashTable *values,
		php_user_cache_bulk_store_item *items,
		zend_string **storage_keys,
		const php_user_cache_prepare_options *prep_opts,
		bool *result)
{
	zend_ulong num_key;
	zend_string *key, *storage_key;
	zval *value;
	uint32_t i = 0;

	*result = true;

	ZEND_HASH_FOREACH_KEY_VAL(values, num_key, key, value) {
		if (key != NULL) {
			storage_key = user_cache_storage_key(cache, key);
		} else {
			key = zend_long_to_str(num_key);
			storage_key = user_cache_storage_key(cache, key);

			zend_string_release(key);
		}

		storage_keys[i] = storage_key;
		items[i].value = value;

		if (!php_user_cache_prepare_value(storage_key, value, prep_opts, &items[i].prepared)) {
			php_user_cache_destroy_prepared_value(&items[i].prepared);

			zend_string_release(storage_key);

			*result = false;

			break;
		}

		i++;
	} ZEND_HASH_FOREACH_END();

	return i;
}

#if ZEND_DEBUG
/* Debug-only fault injection: force a bailout escaping the commit loop after
 * the first item commits, exercising the replaced-entry rollback path that is
 * otherwise unreachable from userland. */
static bool user_cache_debug_force_bulk_commit_bailout(void)
{
	const char *value = getenv("USER_CACHE_DEBUG_FORCE_BULK_COMMIT_BAILOUT");

	return value != NULL && value[0] != '\0' && value[0] != '0';
}
#endif

/* Rolls back the entire batch if any store fails. */
static uint32_t user_cache_commit_bulk_store_locked(
		php_user_cache_bulk_store_item *items,
		zend_string **storage_keys,
		uint32_t prepared_count,
		zend_long ttl,
		const php_user_cache_store_options *store_opts,
		bool *result)
{
	uint32_t i, j, stored_count = 0;

	*result = true;

	UC_G(store_defer_unlock) = true;

	for (i = 0; i < prepared_count; i++) {
		if (!php_user_cache_store_prepared_locked(
				storage_keys[i],
				items[i].value,
				&items[i].prepared,
				ttl,
				store_opts,
				&items[i].store_result)
		) {
			*result = false;

			break;
		}

		items[i].committed = true;
		stored_count = i + 1;

#if ZEND_DEBUG
		if (i == 0 && prepared_count > 1 && user_cache_debug_force_bulk_commit_bailout()) {
			zend_bailout();
		}
#endif
	}

	UC_G(store_defer_unlock) = false;

	if (*result) {
		for (i = 0; i < stored_count; i++) {
			php_user_cache_discard_replaced_entry_locked(
				storage_keys[i],
				&items[i].store_result.replaced_entry
			);
			items[i].committed = false;
		}
	} else {
		for (j = stored_count; j > 0; j--) {
			php_user_cache_rollback_replaced_entry_locked(
				storage_keys[j - 1],
				&items[j - 1].store_result.replaced_entry
			);
			items[j - 1].committed = false;
		}

		stored_count = 0;
	}

	return stored_count;
}

/* A bailout escaping the bulk commit would strand every captured replaced
 * entry: the new entries are published, so the snapshots hold the only
 * references to the old key/value blocks. The commit kept the write lock held
 * across the bailout (store_defer_unlock), so this restores the pre-batch
 * state on the same lock hold before the bailout continues. */
static void user_cache_abort_bulk_store_on_bailout(
		php_user_cache_bulk_store_item *items,
		zend_string **storage_keys,
		uint32_t prepared_count)
{
	uint32_t j;

	UC_G(store_defer_unlock) = false;

	/* The lock is expected to still be held; re-acquire defensively otherwise. */
	if (!UC_G(lock_held) && !php_user_cache_wlock()) {
		return;
	}

	for (j = prepared_count; j > 0; j--) {
		if (items[j - 1].committed) {
			php_user_cache_rollback_replaced_entry_locked(
				storage_keys[j - 1],
				&items[j - 1].store_result.replaced_entry
			);
			items[j - 1].committed = false;
		}
	}

	php_user_cache_unlock_if_held();
}

static void user_cache_finish_bulk_store(
		php_user_cache_bulk_store_item *items,
		zend_string **storage_keys,
		uint32_t stored_count)
{
	uint32_t i;

	for (i = 0; i < stored_count; i++) {
		if (items[i].store_result.should_seed_request_local_slot) {
			php_user_cache_store_request_local_slot(
				storage_keys[i],
				items[i].store_result.stored_generation,
				items[i].value,
				true
			);
		}
	}
}

static bool user_cache_instance_store_multiple(
		php_user_cache_object *cache,
		HashTable *values,
		zend_long ttl)
{
	php_user_cache_bulk_store_item *items;
	php_user_cache_bulk_order *order;
	zend_string **storage_keys;
	php_user_cache_prepare_options prep_opts = {
		.caller_holds_write_lock = false,
		.throw_on_failure = false,
	};
	php_user_cache_store_options store_opts = {
		.retry_after_memory_pressure = false,
		.throw_on_failure = false,
		.capture_replaced_entry = true,
	};
	uint32_t i, count, prepared_count, stored_count = 0;
	bool result, *acquired;

	if (!user_cache_can_write()) {
		return false;
	}

	count = zend_hash_num_elements(values);
	if (count == 0) {
		return true;
	}

	items = ecalloc(count, sizeof(*items));
	order = safe_emalloc(count, sizeof(*order), 0);
	storage_keys = safe_emalloc(count, sizeof(zend_string *), 0);
	acquired = ecalloc(count, sizeof(bool));

	prepared_count = user_cache_prepare_bulk_store_items(
		cache,
		values,
		items,
		storage_keys,
		&prep_opts,
		&result
	);

	if (result) {
		result = user_cache_bulk_lock_keys_ordered(storage_keys, order, acquired, prepared_count);
	}

	if (result && user_cache_begin_write()) {
		zend_try {
			stored_count = user_cache_commit_bulk_store_locked(
				items,
				storage_keys,
				prepared_count,
				ttl,
				&store_opts,
				&result
			);
		} zend_catch {
			user_cache_abort_bulk_store_on_bailout(items, storage_keys, prepared_count);

			zend_bailout();
		} zend_end_try();

		php_user_cache_unlock();
	} else {
		result = false;
	}

	php_user_cache_release_entry_locks(storage_keys, acquired, prepared_count);

	user_cache_finish_bulk_store(items, storage_keys, stored_count);

	for (i = 0; i < prepared_count; i++) {
		php_user_cache_destroy_prepared_value(&items[i].prepared);

		zend_string_release(storage_keys[i]);
	}

	efree(acquired);
	efree(storage_keys);
	efree(order);
	efree(items);

	return result;
}

static bool user_cache_instance_delete_multiple(
		php_user_cache_object *cache,
		HashTable *keys)
{
	php_user_cache_bulk_order *order;
	zend_string **prepared_keys, **storage_keys;
	uint32_t i, count;
	bool *acquired, result = true;

	if (!user_cache_prepare_key_list(keys, 1, &prepared_keys, &count)) {
		return false;
	}

	if (!user_cache_can_write()) {
		user_cache_release_key_list(prepared_keys, count);

		return !EG(exception);
	}

	if (count == 0) {
		user_cache_release_key_list(prepared_keys, count);

		return true;
	}

	storage_keys = safe_emalloc(count, sizeof(zend_string *), 0);
	order = safe_emalloc(count, sizeof(php_user_cache_bulk_order), 0);
	acquired = ecalloc(count, sizeof(bool));

	for (i = 0; i < count; i++) {
		storage_keys[i] = user_cache_storage_key(cache, prepared_keys[i]);
	}

	result = user_cache_bulk_lock_keys_ordered(storage_keys, order, acquired, count);

	if (result && user_cache_begin_write()) {
		zend_try {
			for (i = 0; i < count; i++) {
				php_user_cache_delete_locked(storage_keys[i]);
			}
		} zend_catch {
			php_user_cache_unlock_if_held();

			zend_bailout();
		} zend_end_try();

		php_user_cache_unlock();
	} else {
		result = false;
	}

	php_user_cache_release_entry_locks(storage_keys, acquired, count);

	for (i = 0; i < count; i++) {
		zend_string_release(storage_keys[i]);
	}

	efree(acquired);
	efree(order);
	efree(storage_keys);

	user_cache_release_key_list(prepared_keys, count);

	return result;
}

static bool user_cache_init_partition_context(php_user_cache_partition *partition, const char *name)
{
	partition->context = php_user_cache_context_state;
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

static bool user_cache_startup_storage_for_context(php_user_cache_context *ctx)
{
	php_user_cache_context *prev_ctx;

	prev_ctx = php_user_cache_activate_context(ctx);

	php_user_cache_reset_runtime();
	if (php_user_cache_active_runtime()->enabled && UC_G(enable)) {
		if (!php_user_cache_startup_storage_before_request()) {
			php_user_cache_restore_context(prev_ctx);

			return false;
		}
	}

	php_user_cache_restore_context(prev_ctx);

	return true;
}

static void user_cache_partitions_shutdown(void)
{
	php_user_cache_partition *partition, *next;
	php_user_cache_context *prev_ctx;

	prev_ctx = UC_G(active_context_ptr);

	partition = php_user_cache_partitions;
	while (partition != NULL) {
		next = partition->next;

		php_user_cache_activate_context(&partition->context);
		php_user_cache_shutdown_storage();
		php_user_cache_reset_runtime();

		free(partition->name);
		free(partition);

		partition = next;
	}

	php_user_cache_partitions = NULL;

	php_user_cache_restore_context(prev_ctx);
}

/* Separate identical boundary names owned by different Unix users. */
static bool user_cache_format_boundary_key_prefix(char *prefix, size_t prefix_size, size_t *prefix_len)
{
#if !defined(ZEND_WIN32) && defined(HAVE_UNISTD_H)
	int n;

	n = snprintf(
		prefix,
		prefix_size,
		"uid:%ld:gid:%ld:",
		(long) geteuid(),
		(long) getegid()
	);

	if (n < 0 || (size_t) n >= prefix_size) {
		return false;
	}

	*prefix_len = (size_t) n;
#else
	prefix[0] = '\0';
	*prefix_len = 0;
	(void) prefix_size;
#endif

	return true;
}

static char *user_cache_build_boundary_key(const char *boundary, size_t *key_len)
{
	size_t boundary_len, prefix_len;
	char *boundary_key, prefix[64];

	boundary_len = strlen(boundary);

	if (!user_cache_format_boundary_key_prefix(prefix, sizeof(prefix), &prefix_len)) {
		return NULL;
	}

	if (boundary_len > SIZE_MAX - prefix_len - 1) {
		return NULL;
	}

	boundary_key = malloc(prefix_len + boundary_len + 1);
	if (boundary_key == NULL) {
		return NULL;
	}

	memcpy(boundary_key, prefix, prefix_len);
	memcpy(boundary_key + prefix_len, boundary, boundary_len + 1);

	*key_len = prefix_len + boundary_len;

	return boundary_key;
}

static php_user_cache_boundary_partition *user_cache_find_boundary_partition(
		const char *boundary,
		size_t boundary_len,
		zend_ulong boundary_hash)
{
	php_user_cache_boundary_partition *entry;

	for (entry = php_user_cache_boundary_partitions; entry != NULL; entry = entry->next) {
		if (entry->boundary_hash == boundary_hash &&
			entry->boundary_len == boundary_len &&
			memcmp(entry->boundary, boundary, boundary_len) == 0
		) {
			return entry;
		}
	}

	return NULL;
}

static php_user_cache_boundary_partition *user_cache_create_boundary_partition(
		const char *sapi_prefix,
		const char *boundary,
		size_t boundary_len,
		zend_ulong boundary_hash,
		void (*log_message)(const char *message))
{
	php_user_cache_boundary_partition *entry;
	char partition_name[128];

	if (php_user_cache_boundary_partition_count >= PHP_USER_CACHE_MAX_BOUNDARY_PARTITIONS) {
		if (!php_user_cache_boundary_limit_logged) {
			log_message("UserCache disabled for this request because the cache boundary partition limit was reached");
			php_user_cache_boundary_limit_logged = true;
		}

		return NULL;
	}

	entry = calloc(1, sizeof(*entry));
	if (entry == NULL) {
		return NULL;
	}

	entry->boundary = malloc(boundary_len + 1);
	if (entry->boundary == NULL) {
		free(entry);

		return NULL;
	}

	memcpy(entry->boundary, boundary, boundary_len);

	entry->boundary[boundary_len] = '\0';
	entry->boundary_len = boundary_len;
	entry->boundary_hash = boundary_hash;

	snprintf(
		partition_name,
		sizeof(partition_name),
		"%s:boundary:" ZEND_ULONG_FMT ":" ZEND_ULONG_FMT ":%zx",
		sapi_prefix,
		boundary_hash,
		boundary_len > 1 ? zend_inline_hash_func(boundary + 1, boundary_len - 1) : boundary_hash,
		boundary_len
	);

	entry->partition = php_user_cache_partition_create(partition_name);
	if (entry->partition == NULL) {
		free(entry->boundary);
		free(entry);

		return NULL;
	}

	entry->partition->context.boundary_identity = entry->boundary;
	entry->partition->context.boundary_identity_len = entry->boundary_len;
	entry->partition->context.boundary_shared = true;

	entry->next = php_user_cache_boundary_partitions;

	php_user_cache_boundary_partitions = entry;
	php_user_cache_boundary_partition_count++;

	return entry;
}

uint64_t php_user_cache_cached_pid(void)
{
#ifndef ZEND_WIN32
	if (UNEXPECTED(php_user_cache_self_pid_uncached)) {
		return php_user_cache_current_pid();
	}
#endif

	if (UNEXPECTED(php_user_cache_self_pid == 0)) {
		php_user_cache_self_pid = php_user_cache_current_pid();
	}

	return php_user_cache_self_pid;
}

static void user_cache_safe_direct_handlers_init(void)
{
	if (user_cache_safe_direct_handlers_initialized) {
		return;
	}

	zend_hash_init(
		&php_user_cache_safe_direct_handler_table,
		8,
		NULL,
		user_cache_safe_direct_handlers_dtor,
		true
	);

	user_cache_safe_direct_handlers_initialized = true;
}

static void user_cache_safe_direct_handlers_destroy(void)
{
	if (!user_cache_safe_direct_handlers_initialized) {
		return;
	}

	zend_hash_destroy(&php_user_cache_safe_direct_handler_table);

	user_cache_safe_direct_handlers_initialized = false;
}

ZEND_API void php_user_cache_safe_direct_register_class(
		zend_class_entry *ce,
		const php_user_cache_safe_direct_handlers *handlers)
{
	php_user_cache_safe_direct_handlers handlers_copy;

	if (ce == NULL ||
		handlers == NULL ||
		handlers->copy == NULL ||
		handlers->state_serialize == NULL ||
		handlers->state_unserialize == NULL
	) {
		return;
	}

	user_cache_safe_direct_handlers_init();

	handlers_copy = *handlers;

	zend_hash_index_update_mem(
		&php_user_cache_safe_direct_handler_table,
		(zend_ulong) (uintptr_t) ce,
		&handlers_copy,
		sizeof(handlers_copy)
	);
}

php_user_cache_safe_direct_state_copy_func_t php_user_cache_safe_direct_state_copy_func(
		zend_class_entry *ce,
		zend_class_entry **base_ce_ptr)
{
	const php_user_cache_safe_direct_handlers *handlers =
		php_user_cache_safe_direct_find_handlers(ce, base_ce_ptr)
	;

	return handlers != NULL ? handlers->copy : NULL;
}

zend_class_entry *php_user_cache_safe_direct_find_base(zend_class_entry *ce)
{
	zend_class_entry *base_ce = NULL;

	if (php_user_cache_safe_direct_state_copy_func(ce, &base_ce) == NULL) {
		return NULL;
	}

	return base_ce;
}

PHP_USER_CACHE_DEFINE_SAFE_DIRECT_HANDLER_GETTER(state_has_unstorable)

PHP_USER_CACHE_DEFINE_SAFE_DIRECT_HANDLER_GETTER(state_serialize)

PHP_USER_CACHE_DEFINE_SAFE_DIRECT_HANDLER_GETTER(state_unserialize)

bool php_user_cache_safe_direct_prefers_request_local_prototype(zend_class_entry *ce)
{
	const php_user_cache_safe_direct_handlers *handlers =
		php_user_cache_safe_direct_find_handlers(ce, NULL)
	;

	return handlers != NULL && handlers->prefer_request_local_prototype;
}

ZEND_API php_user_cache_partition *php_user_cache_partition_create(const char *name)
{
	php_user_cache_partition *partition;

	partition = calloc(1, sizeof(php_user_cache_partition));
	if (partition == NULL) {
		return NULL;
	}

	if (!user_cache_init_partition_context(partition, name)) {
		free(partition);

		return NULL;
	}

	partition->next = php_user_cache_partitions;
	php_user_cache_partitions = partition;

	return partition;
}

ZEND_API bool php_user_cache_partition_startup_storage(php_user_cache_partition *partition)
{
	php_user_cache_partition *prev_partition;
	bool result;

	if (partition == NULL) {
		return true;
	}

	prev_partition = UC_G(active_partition);
	UC_G(active_partition) = partition;

	result = user_cache_startup_storage_for_context(&partition->context);

	UC_G(active_partition) = prev_partition;

	return result;
}

ZEND_API bool php_user_cache_startup_default_context_storage(void)
{
	return user_cache_startup_storage_for_context(&php_user_cache_context_state);
}

ZEND_API void php_user_cache_partition_activate(php_user_cache_partition *partition)
{
	UC_G(active_partition) = partition;
	UC_G(active_context_ptr) = NULL;
	UC_G(request_unavailable_reason) = PHP_USER_CACHE_REASON_NONE;
	UC_G(runtime_resolved) = false;
}

ZEND_API void php_user_cache_activate_request_unavailable(php_user_cache_reason reason)
{
	UC_G(active_partition) = NULL;
	UC_G(active_context_ptr) = NULL;
	UC_G(request_unavailable_reason) = reason;
	UC_G(runtime_resolved) = false;
}

ZEND_API php_user_cache_partition *php_user_cache_boundary_partition_get(
		const char *sapi_prefix,
		const char *boundary,
		void (*log_message)(const char *message))
{
	php_user_cache_boundary_partition *entry;
	zend_ulong boundary_hash;
	size_t boundary_len;
	char *boundary_key;

	boundary_key = user_cache_build_boundary_key(boundary, &boundary_len);
	if (boundary_key == NULL) {
		return NULL;
	}

	boundary_hash = zend_inline_hash_func(boundary_key, boundary_len);
	entry = user_cache_find_boundary_partition(boundary_key, boundary_len, boundary_hash);
	if (entry == NULL) {
		entry = user_cache_create_boundary_partition(sapi_prefix, boundary_key, boundary_len, boundary_hash, log_message);
		if (entry == NULL) {
			free(boundary_key);

			return NULL;
		}
	}

	free(boundary_key);

	if (!php_user_cache_partition_startup_storage(entry->partition) &&
		!php_user_cache_boundary_startup_failed_logged
	) {
		log_message("UserCache partition startup failed; UserCache will be unavailable");
		php_user_cache_boundary_startup_failed_logged = true;
	}

	return entry->partition;
}

ZEND_API void php_user_cache_activate_boundary_partition(
		const char *sapi_prefix,
		const char *(*get_env)(const char *name),
		void (*log_message)(const char *message),
		php_user_cache_reason failure_reason)
{
	const char *boundary;
	php_user_cache_partition *partition = NULL;

	boundary = get_env("DOCUMENT_ROOT");
	if (boundary == NULL || boundary[0] == '\0') {
		boundary = get_env("SERVER_NAME");
	}

	if (boundary != NULL && boundary[0] != '\0') {
		partition = php_user_cache_boundary_partition_get(sapi_prefix, boundary, log_message);
	}

	if (partition == NULL) {
		php_user_cache_activate_request_unavailable(failure_reason);

		return;
	}

	php_user_cache_partition_activate(partition);
}

ZEND_API void php_user_cache_boundary_partitions_shutdown(void)
{
	php_user_cache_boundary_partition *entry, *next;

	entry = php_user_cache_boundary_partitions;
	while (entry != NULL) {
		next = entry->next;
		free(entry->boundary);
		free(entry);
		entry = next;
	}

	php_user_cache_boundary_partitions = NULL;
	php_user_cache_boundary_partition_count = 0;
	php_user_cache_boundary_limit_logged = false;
	php_user_cache_boundary_startup_failed_logged = false;
}

ZEND_API void php_user_cache_opt_in(void)
{
	php_user_cache_runtime_opted_in = true;
}

#ifdef ZTS
static void user_cache_globals_ctor(php_user_cache_globals *user_cache_globals)
{
	memset(user_cache_globals, 0, sizeof(php_user_cache_globals));
}

static void user_cache_globals_dtor(php_user_cache_globals *user_cache_globals)
{
#ifdef PHP_USER_CACHE_HAVE_OPTIMISTIC
	/* Release reader slots owned by the terminating thread. */
	php_user_cache_release_thread_reader_claims(user_cache_globals);
#endif
	php_user_cache_release_thread_graph_pin_claims(user_cache_globals);
}

size_t php_user_cache_globals_size(void)
{
	return sizeof(php_user_cache_globals);
}

/* SAPI activation may access these globals before MINIT and after MSHUTDOWN. */
void php_user_cache_globals_startup(void)
{
	user_cache_globals_id = ts_allocate_fast_id(
		&user_cache_globals_id,
		&user_cache_globals_offset,
		sizeof(php_user_cache_globals),
		(ts_allocate_ctor) user_cache_globals_ctor,
		(ts_allocate_dtor) user_cache_globals_dtor
	);
}
#endif

void php_user_cache_minit(void)
{
	php_user_cache_context *prev_ctx;

#ifndef ZEND_WIN32
	php_user_cache_self_pid = php_user_cache_current_pid();
	if (!php_user_cache_pid_atfork_registered &&
		!php_user_cache_self_pid_uncached
	) {
		if (pthread_atfork(NULL, NULL, user_cache_pid_atfork_child) == 0) {
			php_user_cache_pid_atfork_registered = true;
		} else {
			php_user_cache_self_pid = 0;
			php_user_cache_self_pid_uncached = true;
		}
	}
#endif

	php_user_cache_runtime_opted_in = false;

	user_cache_register_classes();

	user_cache_safe_direct_handlers_init();
	php_user_cache_optimistic_fork_setup();

	prev_ctx = php_user_cache_activate_context(php_user_cache_owning_context());

	php_user_cache_reset_storage();

	php_user_cache_restore_context(prev_ctx);
}

void php_user_cache_mshutdown(void)
{
	php_user_cache_context *prev_ctx;

	user_cache_partitions_shutdown();

	UC_G(active_partition) = NULL;

	prev_ctx = php_user_cache_activate_context(php_user_cache_owning_context());

	php_user_cache_shutdown_storage();
	php_user_cache_reset_runtime();
	php_user_cache_restore_context(prev_ctx);

	UC_G(active_partition) = NULL;

	php_user_cache_runtime_opted_in = false;

	user_cache_safe_direct_handlers_destroy();
	user_cache_reset_class_entries();
}

zend_result php_user_cache_rshutdown(void)
{
	php_user_cache_unlock_if_held();

	UC_G(request_unavailable_reason) = PHP_USER_CACHE_REASON_NONE;
	UC_G(runtime_resolved) = false;

	user_cache_release_pools();
	php_user_cache_lookup_cache_clear();

	php_user_cache_release_request_entry_locks();
	php_user_cache_release_request_local_slots();

	if (UC_G(request_local_slot_may_cycle)) {
		UC_G(request_local_slot_may_cycle) = false;

		/* No later GC run is guaranteed during shutdown. */
		if (!CG(unclean_shutdown)) {
			gc_collect_cycles();
		}
	}

	php_user_cache_decode_resolve_cache_release();
	php_user_cache_decode_shape_prototype_cache_release();
	php_user_cache_decode_maps_teardown();

	return SUCCESS;
}

zend_result php_user_cache_post_deactivate(void)
{
	php_user_cache_release_request_shared_graph_refs();
	php_user_cache_expunge_expired_at_request_end();

	return SUCCESS;
}

static void user_cache_store_method(INTERNAL_FUNCTION_PARAMETERS, bool add_only)
{
	php_user_cache_object *cache;
	zend_long ttl = 0;
	zend_string *key, *storage_key;
	zval *value;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(key)
		Z_PARAM_ZVAL(value)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(ttl)
	ZEND_PARSE_PARAMETERS_END();

	cache = user_cache_object_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	storage_key = user_cache_validated_storage_key(cache, key, 1);
	if (storage_key == NULL) {
		RETURN_THROWS();
	}

	if (!user_cache_validate_api_value(value, 2)) {
		zend_string_release(storage_key);
		RETURN_THROWS();
	}

	if (!user_cache_validate_non_negative(ttl, 3)) {
		zend_string_release(storage_key);
		RETURN_THROWS();
	}

	if (!(user_cache_can_write() &&
		user_cache_store_storage_key_prevalidated(storage_key, value, ttl, add_only))
	) {
		zend_string_release(storage_key);

		if (EG(exception)) {
			RETURN_THROWS();
		}

		RETURN_FALSE;
	}

	zend_string_release(storage_key);

	RETURN_TRUE;
}

static void user_cache_atomic_update_method(INTERNAL_FUNCTION_PARAMETERS, bool decrement)
{
	php_user_cache_object *cache;
	php_user_cache_atomic_update_result result;
	zend_long step = 1, ttl = 0;
	zend_string *key, *storage_key;
	bool updated;

	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_STR(key)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(step)
		Z_PARAM_LONG(ttl)
	ZEND_PARSE_PARAMETERS_END();

	cache = user_cache_object_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	storage_key = user_cache_validated_storage_key(cache, key, 1);
	if (storage_key == NULL) {
		RETURN_THROWS();
	}

	if (!user_cache_validate_non_negative(step, 2)) {
		zend_string_release(storage_key);

		RETURN_THROWS();
	}

	if (!user_cache_validate_non_negative(ttl, 3)) {
		zend_string_release(storage_key);

		RETURN_THROWS();
	}

	updated = user_cache_atomic_update_api(storage_key, step, ttl, decrement, &result);

	zend_string_release(storage_key);

	if (result.is_type_error) {
		zend_value_error(
			"%s of user cache key \"%s\" requires the stored value to be an integer",
			decrement ? "Decrement" : "Increment",
			ZSTR_VAL(key)
		);

		RETURN_THROWS();
	}

	if (result.is_overflow) {
		zend_throw_error(
			zend_ce_arithmetic_error,
			"%s of user cache key \"%s\" would exceed the range of a PHP integer",
			decrement ? "Decrement" : "Increment",
			ZSTR_VAL(key)
		);

		RETURN_THROWS();
	}

	if (!updated) {
		RETURN_NULL();
	}

	RETURN_LONG(result.new_value);
}

ZEND_METHOD(UserCache_CacheStatus, __construct)
{
}

ZEND_METHOD(UserCache_CacheStatus, getAvailability)
{
	php_user_cache_status_object *status;
	zend_object *availability;

	ZEND_PARSE_PARAMETERS_NONE();

	status = user_cache_status_from_this(ZEND_THIS);
	if (status == NULL) {
		RETURN_THROWS();
	}

	availability = user_cache_availability_enum_case(status->availability_reason);

	RETURN_OBJ_COPY(availability);
}

PHP_USER_CACHE_DEFINE_STATUS_LONG_GETTER(getConfiguredMemory, configured_memory)

PHP_USER_CACHE_DEFINE_STATUS_LONG_GETTER(getSharedMemorySize, shared_memory_size)

PHP_USER_CACHE_DEFINE_STATUS_LONG_GETTER(getUsedMemory, used_memory)

PHP_USER_CACHE_DEFINE_STATUS_LONG_GETTER(getFreeMemory, free_memory)

PHP_USER_CACHE_DEFINE_STATUS_LONG_GETTER(getWastedMemory, wasted_memory)

PHP_USER_CACHE_DEFINE_STATUS_LONG_GETTER(getEntryCount, entry_count)

PHP_USER_CACHE_DEFINE_STATUS_LONG_GETTER(getEntryCapacity, entry_capacity)

PHP_USER_CACHE_DEFINE_STATUS_LONG_GETTER(getTombstoneCount, tombstone_count)

PHP_USER_CACHE_DEFINE_STATUS_LONG_GETTER(getExpungeCount, expunge_count)

PHP_USER_CACHE_DEFINE_STATUS_LONG_GETTER(getStoreFailureCount, store_failure_count)

PHP_USER_CACHE_DEFINE_STATUS_LONG_GETTER(getGraphPinSlotsInUse, graph_pin_slots_in_use)

PHP_USER_CACHE_DEFINE_STATUS_LONG_GETTER(getGraphPinnedReferences, graph_pinned_references)

PHP_USER_CACHE_DEFINE_STATUS_LONG_GETTER(getDeadPinOwnersReclaimed, dead_pin_owners_reclaimed)

PHP_USER_CACHE_DEFINE_STATUS_LONG_GETTER(getDeadPinsStripped, dead_pins_stripped)

ZEND_METHOD(UserCache_CachePoolStatus, __construct)
{
}

ZEND_METHOD(UserCache_CachePoolStatus, getPoolName)
{
	php_user_cache_pool_status_object *status;

	ZEND_PARSE_PARAMETERS_NONE();

	status = user_cache_pool_status_from_this(ZEND_THIS);
	if (status == NULL) {
		RETURN_THROWS();
	}

	RETURN_STR_COPY(status->scope);
}

ZEND_METHOD(UserCache_CachePoolStatus, getEntryCount)
{
	php_user_cache_pool_status_object *status;

	ZEND_PARSE_PARAMETERS_NONE();

	status = user_cache_pool_status_from_this(ZEND_THIS);
	if (status == NULL) {
		RETURN_THROWS();
	}

	RETURN_LONG(status->entry_count);
}

ZEND_METHOD(UserCache_CachePoolStatus, getEntryKeys)
{
	php_user_cache_pool_status_object *status;

	ZEND_PARSE_PARAMETERS_NONE();

	status = user_cache_pool_status_from_this(ZEND_THIS);
	if (status == NULL) {
		RETURN_THROWS();
	}

	if (Z_TYPE(status->entry_keys) != IS_ARRAY) {
		RETURN_EMPTY_ARRAY();
	}

	RETURN_COPY(&status->entry_keys);
}

ZEND_METHOD(UserCache_CachePoolStatus, getUsedMemory)
{
	php_user_cache_pool_status_object *status;

	ZEND_PARSE_PARAMETERS_NONE();

	status = user_cache_pool_status_from_this(ZEND_THIS);
	if (status == NULL) {
		RETURN_THROWS();
	}

	RETURN_LONG(status->used_memory);
}

ZEND_METHOD(UserCache_Cache, __construct)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_throw_error(NULL, "UserCache\\Cache instances must be obtained via UserCache\\Cache::getPool()");
}

ZEND_METHOD(UserCache_Cache, hasPool)
{
	zend_string *pool;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(pool)
	ZEND_PARSE_PARAMETERS_END();

	if (!user_cache_validate_pool_name(pool, 1)) {
		RETURN_THROWS();
	}

	RETURN_BOOL(UC_G(pool_table) != NULL &&
		zend_hash_exists(UC_G(pool_table), pool)
	);
}

ZEND_METHOD(UserCache_Cache, getPool)
{
	zend_string *pool;
	zend_object *obj;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(pool)
	ZEND_PARSE_PARAMETERS_END();

	if (!user_cache_validate_pool_name(pool, 1)) {
		RETURN_THROWS();
	}

	obj = user_cache_get_or_create_pool(pool);
	if (obj == NULL) {
		zend_throw_error(NULL, "Failed to register UserCache\\Cache pool");

		RETURN_THROWS();
	}

	GC_ADDREF(obj);

	RETURN_OBJ(obj);
}

ZEND_METHOD(UserCache_Cache, getPools)
{
	zend_string *pool;
	zval *instance;

	ZEND_PARSE_PARAMETERS_NONE();

	array_init(return_value);

	if (UC_G(pool_table) == NULL) {
		return;
	}

	ZEND_HASH_FOREACH_STR_KEY_VAL(UC_G(pool_table), pool, instance) {
		if (pool == NULL) {
			continue;
		}

		Z_ADDREF_P(instance);

		/* Preserve numeric-looking pool names as string keys. */
		zend_hash_add(Z_ARRVAL_P(return_value), pool, instance);
	} ZEND_HASH_FOREACH_END();
}

ZEND_METHOD(UserCache_Cache, deletePool)
{
	zend_string *pool, *scope_prefix;
	bool cleared = true;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(pool)
	ZEND_PARSE_PARAMETERS_END();

	if (!user_cache_validate_pool_name(pool, 1)) {
		RETURN_THROWS();
	}

	if (user_cache_can_write()) {
		scope_prefix = user_cache_build_scope_prefix(pool);
		cleared = user_cache_clear_scope_prevalidated(scope_prefix);
		zend_string_release(scope_prefix);
	} else if (EG(exception)) {
		RETURN_THROWS();
	}

	if (!cleared) {
		if (EG(exception)) {
			RETURN_THROWS();
		}

		RETURN_FALSE;
	}

	/* Drop the request-local pool object; a later getPool() recreates it empty. */
	if (UC_G(pool_table) != NULL) {
		zend_hash_del(UC_G(pool_table), pool);
	}

	RETURN_TRUE;
}

ZEND_METHOD(UserCache_Cache, getStatus)
{
	ZEND_PARSE_PARAMETERS_NONE();

	user_cache_return_status(return_value);

	if (EG(exception)) {
		RETURN_THROWS();
	}
}

ZEND_METHOD(UserCache_Cache, store)
{
	user_cache_store_method(INTERNAL_FUNCTION_PARAM_PASSTHRU, false);
}

ZEND_METHOD(UserCache_Cache, add)
{
	user_cache_store_method(INTERNAL_FUNCTION_PARAM_PASSTHRU, true);
}

ZEND_METHOD(UserCache_Cache, storeMultiple)
{
	php_user_cache_object *cache;
	zend_long ttl = 0;
	HashTable *values;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ARRAY_HT(values)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(ttl)
	ZEND_PARSE_PARAMETERS_END();

	cache = user_cache_object_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	if (!user_cache_validate_store_array(values, 1)) {
		RETURN_THROWS();
	}

	if (!user_cache_validate_non_negative(ttl, 2)) {
		RETURN_THROWS();
	}

	if (!user_cache_instance_store_multiple(cache, values, ttl)) {
		if (EG(exception)) {
			RETURN_THROWS();
		}

		RETURN_FALSE;
	}

	RETURN_TRUE;
}

ZEND_METHOD(UserCache_Cache, increment)
{
	user_cache_atomic_update_method(INTERNAL_FUNCTION_PARAM_PASSTHRU, false);
}

ZEND_METHOD(UserCache_Cache, decrement)
{
	user_cache_atomic_update_method(INTERNAL_FUNCTION_PARAM_PASSTHRU, true);
}

ZEND_METHOD(UserCache_Cache, fetch)
{
	php_user_cache_object *cache;
	zend_string *key, *storage_key;
	zval *default_value = NULL, default_null;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(key)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(default_value)
	ZEND_PARSE_PARAMETERS_END();

	cache = user_cache_object_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	storage_key = user_cache_validated_storage_key(cache, key, 1);
	if (storage_key == NULL) {
		RETURN_THROWS();
	}

	if (default_value == NULL) {
		ZVAL_NULL(&default_null);
		default_value = &default_null;
	}

	user_cache_fetch_api(storage_key, default_value, return_value);

	zend_string_release(storage_key);
}

ZEND_METHOD(UserCache_Cache, fetchMultiple)
{
	php_user_cache_object *cache;
	zval *default_value = NULL, default_null;
	HashTable *keys;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ARRAY_HT(keys)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(default_value)
	ZEND_PARSE_PARAMETERS_END();

	cache = user_cache_object_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	if (default_value == NULL) {
		ZVAL_NULL(&default_null);
		default_value = &default_null;
	}

	if (user_cache_fetch_multiple_api(cache, keys, default_value, return_value) == FAILURE) {
		if (EG(exception)) {
			RETURN_THROWS();
		}

		RETURN_EMPTY_ARRAY();
	}
}

ZEND_METHOD(UserCache_Cache, has)
{
	php_user_cache_object *cache;
	zend_string *key, *storage_key;
	bool exists;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(key)
	ZEND_PARSE_PARAMETERS_END();

	cache = user_cache_object_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	storage_key = user_cache_validated_storage_key(cache, key, 1);
	if (storage_key == NULL) {
		RETURN_THROWS();
	}

	exists = user_cache_exists_api(storage_key);

	zend_string_release(storage_key);

	RETURN_BOOL(exists);
}

ZEND_METHOD(UserCache_Cache, delete)
{
	php_user_cache_object *cache;
	zend_string *key, *storage_key;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(key)
	ZEND_PARSE_PARAMETERS_END();

	cache = user_cache_object_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	storage_key = user_cache_validated_storage_key(cache, key, 1);
	if (storage_key == NULL) {
		RETURN_THROWS();
	}

	if (!user_cache_can_write()) {
		zend_string_release(storage_key);

		if (EG(exception)) {
			RETURN_THROWS();
		}

		RETURN_TRUE;
	}

	if (!user_cache_delete_storage_key_prevalidated(storage_key)) {
		zend_string_release(storage_key);

		if (EG(exception)) {
			RETURN_THROWS();
		}

		RETURN_FALSE;
	}

	zend_string_release(storage_key);

	RETURN_TRUE;
}

ZEND_METHOD(UserCache_Cache, deleteMultiple)
{
	php_user_cache_object *cache;
	HashTable *keys;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(keys)
	ZEND_PARSE_PARAMETERS_END();

	cache = user_cache_object_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	if (!user_cache_instance_delete_multiple(cache, keys)) {
		if (EG(exception)) {
			RETURN_THROWS();
		}

		RETURN_FALSE;
	}

	RETURN_TRUE;
}

ZEND_METHOD(UserCache_Cache, clear)
{
	php_user_cache_object *cache;

	ZEND_PARSE_PARAMETERS_NONE();

	cache = user_cache_object_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	if (!user_cache_can_write()) {
		if (EG(exception)) {
			RETURN_THROWS();
		}

		RETURN_TRUE;
	}

	if (!user_cache_clear_scope_prevalidated(cache->scope_prefix)) {
		if (EG(exception)) {
			RETURN_THROWS();
		}

		RETURN_FALSE;
	}

	RETURN_TRUE;
}

ZEND_METHOD(UserCache_Cache, lock)
{
	php_user_cache_object *cache;
	zend_long lease = 0;
	zend_string *key, *storage_key;
	bool locked;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(key)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(lease)
	ZEND_PARSE_PARAMETERS_END();

	cache = user_cache_object_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	storage_key = user_cache_validated_storage_key(cache, key, 1);
	if (storage_key == NULL) {
		RETURN_THROWS();
	}

	if (!user_cache_validate_non_negative(lease, 2)) {
		zend_string_release(storage_key);

		RETURN_THROWS();
	}

	locked = user_cache_lock_api(storage_key, lease);
	zend_string_release(storage_key);

	RETURN_BOOL(locked);
}

ZEND_METHOD(UserCache_Cache, unlock)
{
	php_user_cache_object *cache;
	zend_string *key, *storage_key;
	bool unlocked;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(key)
	ZEND_PARSE_PARAMETERS_END();

	cache = user_cache_object_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	storage_key = user_cache_validated_storage_key(cache, key, 1);
	if (storage_key == NULL) {
		RETURN_THROWS();
	}

	unlocked = user_cache_unlock_api(storage_key);
	zend_string_release(storage_key);

	RETURN_BOOL(unlocked);
}

static bool user_cache_invoke_remember_callback(
		zend_string *key,
		zend_string *storage_key,
		zend_fcall_info *fci,
		zend_fcall_info_cache *fcc,
		zend_long ttl,
		zval *result)
{
	zval key_zv;

	ZVAL_UNDEF(result);
	fci->retval = result;

	ZVAL_STR(&key_zv, key);
	fci->param_count = 1;
	fci->params = &key_zv;
	fci->named_params = NULL;

	if (zend_call_function(fci, fcc) != SUCCESS || EG(exception)) {
		return true;
	}

	if (Z_TYPE_P(result) == IS_UNDEF) {
		return true;
	}

	if (!user_cache_validate_remember_value(result)) {
		return false;
	}

	if (user_cache_can_write() && user_cache_store_storage_key_prevalidated(storage_key, result, ttl, false)) {
		return true;
	}

	/* Failing to store the memoized value is not itself an error unless it raised an exception. */
	return !EG(exception);
}

ZEND_METHOD(UserCache_Cache, remember)
{
	php_user_cache_object *cache;
	zend_long ttl = 0;
	zend_string *key, *storage_key;
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;
	zval result;
	bool found = false, locked = false, callback_failed = false;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(key)
		Z_PARAM_FUNC(fci, fcc)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(ttl)
	ZEND_PARSE_PARAMETERS_END();

	cache = user_cache_object_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	storage_key = user_cache_validated_storage_key(cache, key, 1);
	if (storage_key == NULL) {
		RETURN_THROWS();
	}

	if (!user_cache_validate_non_negative(ttl, 3)) {
		zend_string_release(storage_key);

		RETURN_THROWS();
	}

	found = user_cache_fetch_if_present_api(storage_key, return_value);
	if (found) {
		zend_string_release(storage_key);

		return;
	}

	/* A restore hook or autoloader threw while materializing the entry: the
	 * entry exists, so propagate the exception instead of recomputing. */
	if (EG(exception)) {
		zend_string_release(storage_key);

		RETURN_THROWS();
	}

	locked = user_cache_lock_api(storage_key, 0);

	/* Another request may have populated the entry while the lock was acquired. */
	found = user_cache_fetch_if_present_api(storage_key, return_value);
	if (found) {
		if (locked) {
			locked = user_cache_unlock_api(storage_key);
		}

		zend_string_release(storage_key);

		return;
	}

	if (EG(exception)) {
		if (locked) {
			locked = user_cache_unlock_api(storage_key);
		}

		zend_string_release(storage_key);

		RETURN_THROWS();
	}

	/* Do not return before zend_end_try() restores the bailout chain. */
	zend_try {
		callback_failed =
			!user_cache_invoke_remember_callback(key, storage_key, &fci, &fcc, ttl, &result);
	} zend_catch {
		if (locked) {
			locked = user_cache_unlock_api(storage_key);
		}

		zend_string_release(storage_key);

		zend_bailout();
	} zend_end_try();

	if (callback_failed) {
		zval_ptr_dtor(&result);

		locked = user_cache_unlock_api(storage_key);

		zend_string_release(storage_key);

		RETURN_THROWS();
	}

	if (locked) {
		locked = user_cache_unlock_api(storage_key);
	}

	zend_string_release(storage_key);

	if (EG(exception)) {
		if (Z_TYPE(result) != IS_UNDEF) {
			zval_ptr_dtor(&result);
		}

		RETURN_THROWS();
	}

	if (Z_TYPE(result) == IS_UNDEF) {
		RETURN_NULL();
	}

	RETURN_COPY_VALUE(&result);
}

ZEND_METHOD(UserCache_Cache, getPoolStatus)
{
	php_user_cache_object *cache;

	ZEND_PARSE_PARAMETERS_NONE();

	cache = user_cache_object_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	user_cache_return_pool_status(cache, return_value);

	if (EG(exception)) {
		RETURN_THROWS();
	}
}

static ZEND_INI_MH(OnUpdateUserCacheShmSize)
{
	zend_long *p, size;

	p = (zend_long *) ZEND_INI_GET_ADDR();
	size = zend_ini_parse_quantity_warn(new_value, entry->name);

	if (size < 0) {
		zend_error(E_WARNING, "user_cache.shm_size must be greater than or equal to 0, " ZEND_LONG_FMT " given", size);

		return FAILURE;
	}

	if ((zend_ulong) size > UINT32_MAX - ZEND_MM_ALIGNMENT) {
		size = (zend_long) (UINT32_MAX - ZEND_MM_ALIGNMENT);
		zend_error(E_WARNING, "user_cache.shm_size is limited to slightly under 4096M; clamping");
	}

	*p = size;

	return SUCCESS;
}

PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY("user_cache.enable",                 "1",    PHP_INI_SYSTEM, OnUpdateBool,             enable,        php_user_cache_globals, user_cache_globals)
	STD_PHP_INI_ENTRY("user_cache.enable_cli",             "0",    PHP_INI_SYSTEM, OnUpdateBool,             enable_cli,    php_user_cache_globals, user_cache_globals)
	STD_PHP_INI_ENTRY("user_cache.shm_size",               "16M",  PHP_INI_SYSTEM, OnUpdateUserCacheShmSize, shm_size,      php_user_cache_globals, user_cache_globals)
	STD_PHP_INI_ENTRY("user_cache.lockfile_path",          "/tmp", PHP_INI_SYSTEM, OnUpdateString,           lockfile_path, php_user_cache_globals, user_cache_globals)
	STD_PHP_INI_ENTRY("user_cache.preferred_memory_model", "",     PHP_INI_SYSTEM, OnUpdateStringUnempty,    memory_model,  php_user_cache_globals, user_cache_globals)
PHP_INI_END()

static PHP_MINIT_FUNCTION(cache)
{
	REGISTER_INI_ENTRIES();

	php_user_cache_minit();

	return SUCCESS;
}

static PHP_MSHUTDOWN_FUNCTION(cache)
{
	php_user_cache_mshutdown();

	UNREGISTER_INI_ENTRIES();

	return SUCCESS;
}

static PHP_RSHUTDOWN_FUNCTION(cache)
{
	return php_user_cache_rshutdown();
}

static PHP_MINFO_FUNCTION(cache)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "UserCache support", "enabled");
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}

zend_module_entry user_cache_module_entry = {
	STANDARD_MODULE_HEADER,
	"user_cache",
	NULL,
	PHP_MINIT(cache),
	PHP_MSHUTDOWN(cache),
	NULL,
	PHP_RSHUTDOWN(cache),
	PHP_MINFO(cache),
	PHP_VERSION,
	NO_MODULE_GLOBALS,
	php_user_cache_post_deactivate,
	STANDARD_MODULE_PROPERTIES_EX
};
