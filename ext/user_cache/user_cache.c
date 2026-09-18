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

#define PHP_UCACHE_API_VALUE_TYPE			"object|array|string|int|float|bool|null"
#define PHP_UCACHE_STORAGE_KEY_CACHE_SLOTS	256U
#define PHP_UCACHE_MAX_BOUNDARY_PARTITIONS	32U

#define PHP_UCACHE_DEFINE_OBJ_FROM_STD(type, name) \
	static type *ucache_##name##_from_obj(zend_object *obj) \
	{ \
		return (type *) ((char *) obj - offsetof(type, std)); \
	}

#define PHP_UCACHE_DEFINE_SAFE_DIRECT_HANDLER_GETTER(member) \
	php_ucache_safe_direct_##member##_func_t php_ucache_safe_direct_##member##_func( \
			zend_class_entry *ce) \
	{ \
		const php_ucache_safe_direct_handlers_t *handlers = \
			php_ucache_safe_direct_find_handlers(ce, NULL) \
		; \
		return handlers != NULL ? handlers->member : NULL; \
	}

#define PHP_UCACHE_DEFINE_STATUS_LONG_GETTER(method, field) \
	ZEND_METHOD(UserCache_CacheStatus, method) \
	{ \
		php_ucache_status_object *status; \
		\
		ZEND_PARSE_PARAMETERS_NONE(); \
		\
		status = ucache_status_from_this(ZEND_THIS); \
		if (status == NULL) { \
			RETURN_THROWS(); \
		} \
		\
		RETURN_LONG(status->stats.field); \
	}

#define PHP_UCACHE_SHM_SIZE_MAX \
	((uint64_t) UINT32_MAX * PHP_UCACHE_SHM_UNIT - ZEND_MM_ALIGNMENT)
#define PHP_UCACHE_ENTRIES_HINT_MAX		16777213

/* Direct-mapped, evict-on-collision: bounds the per-pool key cache to a
 * few KiB. */
typedef struct {
	zend_string *key;
	zend_string *storage_key;
} php_ucache_storage_key_slot_t;

typedef struct {
	zend_string *scope;
	zend_string *scope_prefix;
	php_ucache_ctx_t *ctx;
	/* Lazily allocated PHP_UCACHE_STORAGE_KEY_CACHE_SLOTS slots. */
	php_ucache_storage_key_slot_t *storage_key_cache;
	/* Lookups not yet folded into the shared counters. */
	uint64_t hit_count;
	uint64_t miss_count;
	zend_object std;
} php_ucache_object;

typedef struct {
	zend_string *scope;
	zend_string *scope_prefix;
	php_ucache_ctx_t *ctx;
	/* Immutable snapshot. */
	zend_long entry_count;
	zend_long used_memory;
	zend_long hit_count;
	zend_long miss_count;
	zval entry_keys;
	zend_object std;
} php_ucache_pool_status_object;

typedef struct {
	zend_ulong hash;
	zend_string *key;
	uint32_t index;
} php_ucache_bulk_order_t;

typedef struct {
	zval *value;
	php_ucache_prepared_value_t prepared;
	php_ucache_store_result_t store_result;
	/* True while this item's store has committed but its captured replaced
	 * entry has not been discarded or rolled back; consulted on bailout. */
	bool committed;
} php_ucache_bulk_store_item_t;

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
	zend_long eviction_count;
	zend_long graph_pin_slots_in_use;
	zend_long graph_pinned_references;
	zend_long dead_pin_owners_reclaimed;
	zend_long dead_pins_stripped;
	zend_long hit_count;
	zend_long miss_count;
	zend_long interned_key_count;
} php_ucache_info_stats_t;

typedef struct {
	/* Immutable snapshot. */
	php_ucache_info_stats_t stats;
	php_ucache_reason_t availability_reason;
	bool initialized;
	zend_object std;
} php_ucache_status_object;

typedef struct _php_ucache_boundary_partition {
	char *boundary;
	size_t boundary_len;
	zend_ulong boundary_hash;
	php_ucache_partition_t *partition;
	struct _php_ucache_boundary_partition *next;
} php_ucache_boundary_partition_t;

#ifdef ZTS
static int user_cache_globals_id;
#endif
static HashTable ucache_safe_direct_handler_table;
static bool ucache_safe_direct_handlers_initialized = false;
static php_ucache_boundary_partition_t *ucache_boundary_partitions = NULL;
static zend_class_entry *ucache_availability_ce;
static zend_class_entry *ucache_ce;
static zend_class_entry *ucache_status_ce;
static zend_class_entry *ucache_pool_status_ce;
static zend_object_handlers ucache_object_handlers;
static zend_object_handlers ucache_status_object_handlers;
static zend_object_handlers ucache_pool_status_object_handlers;
static uint64_t ucache_self_pid = 0;
static uint32_t ucache_boundary_partition_count = 0;
static bool ucache_boundary_creation_disabled = false;
static bool ucache_boundary_startup_failed_logged = false;
#ifdef ZTS
/* Serializes boundary partition lookup/creation and, transitively, all
 * request-time mutation of php_ucache_partitions. */
static MUTEX_T ucache_boundary_partitions_mutex = NULL;
#endif
#ifndef ZEND_WIN32
static bool ucache_self_pid_uncached = false;
static bool ucache_pid_atfork_registered = false;
#endif

#ifndef ZTS
php_ucache_globals user_cache_globals;
#else
size_t user_cache_globals_offset;
#endif
php_ucache_ctx_t php_ucache_ctx_state = {
	.storage = { .lock_file = -1 },
	.lock_name = "php_user_cache_lock",
#ifndef ZEND_WIN32
	.sem_filename_prefix = PHP_UCACHE_SEM_FILENAME_PREFIX,
#endif
};
bool php_ucache_runtime_opted_in = false;
/* Append-only. Partitions are added eagerly before workers exist
 * (apache2handler config stage, FPM master) or lazily through boundary
 * partition creation, which holds the boundary partitions lock so threaded
 * SAPIs cannot race the thread-shutdown traversal in
 * ucache_claim_header_is_attached(). */
php_ucache_partition_t *php_ucache_partitions = NULL;

static zend_always_inline void ucache_ensure_ready(void)
{
	php_ucache_ctx_t *ctx = php_ucache_active_context();

	if (UC_G(runtime_resolved) &&
		UC_G(runtime_resolved_ctx) == ctx &&
		UC_G(runtime_resolved_enabled) == UC_G(enable)
	) {
		return;
	}

	php_ucache_ensure_ready_impl();
}

#ifndef ZEND_WIN32
static void ucache_pid_atfork_child(void)
{
	ucache_self_pid = 0;
}
#endif

static bool ucache_user_key_is_valid(zend_string *key)
{
	return ZSTR_LEN(key) != 0 &&
		memchr(ZSTR_VAL(key), PHP_UCACHE_KEY_DELIMITER_CHAR, ZSTR_LEN(key)) == NULL
	;
}

static bool ucache_validate_delimiter_free(zend_string *str, uint32_t arg_num, const char *empty_error)
{
	if (ZSTR_LEN(str) == 0) {
		zend_argument_value_error(arg_num, "%s", empty_error);

		return false;
	}

	if (memchr(ZSTR_VAL(str), PHP_UCACHE_KEY_DELIMITER_CHAR, ZSTR_LEN(str)) != NULL) {
		zend_argument_value_error(arg_num, "must not contain the user-cache key delimiter " PHP_UCACHE_KEY_DELIMITER_NAME);

		return false;
	}

	return true;
}

static bool ucache_validate_key(zend_string *key, uint32_t arg_num)
{
	return ucache_validate_delimiter_free(key, arg_num, "must be a non-empty string");
}

static bool ucache_validate_arg_value_kind(
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

static bool ucache_validate_store_array_value(zval *value, uint32_t arg_num)
{
	return ucache_validate_arg_value_kind(
		value,
		arg_num,
		"must contain only values of type " PHP_UCACHE_API_VALUE_TYPE ", resource given",
		"must not contain Closure objects"
	);
}

static bool ucache_validate_store_array(HashTable *values, uint32_t arg_num)
{
	zend_string *key;
	zval *value;

	ZEND_HASH_FOREACH_STR_KEY_VAL(values, key, value) {
		if (key != NULL && !ucache_user_key_is_valid(key)) {
			zend_argument_value_error(arg_num, "must be an array with non-empty string or int keys that do not contain " PHP_UCACHE_KEY_DELIMITER_NAME);

			return false;
		}

		if (!ucache_validate_store_array_value(value, arg_num)) {
			return false;
		}
	} ZEND_HASH_FOREACH_END();

	return true;
}

static void ucache_release_key_list(zend_string **keys, uint32_t count)
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

static bool ucache_prepare_key_list(
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
			if (!ucache_user_key_is_valid(Z_STR_P(value))) {
				zend_argument_value_error(arg_num, "must contain only non-empty string or int cache keys that do not contain " PHP_UCACHE_KEY_DELIMITER_NAME);
				ucache_release_key_list(prepared, i);

				return false;
			}

			prepared[i++] = zend_string_copy(Z_STR_P(value));
		} else if (Z_TYPE_P(value) == IS_LONG) {
			prepared[i++] = zend_long_to_str(Z_LVAL_P(value));
		} else {
			zend_argument_value_error(arg_num, "must contain only non-empty string or int cache keys that do not contain " PHP_UCACHE_KEY_DELIMITER_NAME);
			ucache_release_key_list(prepared, i);

			return false;
		}
	} ZEND_HASH_FOREACH_END();

	*prepared_keys = prepared;
	*prepared_count = i;

	return true;
}

static bool ucache_validate_api_value(zval *value, uint32_t arg_num)
{
	return ucache_validate_arg_value_kind(
		value,
		arg_num,
		"must be of type " PHP_UCACHE_API_VALUE_TYPE ", resource given",
		"must not be a Closure object"
	);
}

static bool ucache_validate_remember_value(zval *value)
{
	ZVAL_DEREF(value);

	if (Z_TYPE_P(value) == IS_RESOURCE) {
		zend_type_error(PHP_UCACHE_MSG_RESOURCE_UNSTORABLE);

		return false;
	}

	if (Z_TYPE_P(value) == IS_OBJECT && Z_OBJCE_P(value) == zend_ce_closure) {
		zend_type_error(PHP_UCACHE_MSG_CLOSURE_UNSTORABLE);

		return false;
	}

	return true;
}

static zend_string *ucache_build_scope_prefix(zend_string *scope)
{
	return zend_string_concat3(
		ZEND_STRL("user_cache" PHP_UCACHE_KEY_DELIMITER),
		ZSTR_VAL(scope),
		ZSTR_LEN(scope),
		ZEND_STRL(PHP_UCACHE_KEY_DELIMITER)
	);
}

PHP_UCACHE_DEFINE_OBJ_FROM_STD(php_ucache_object, object)

PHP_UCACHE_DEFINE_OBJ_FROM_STD(php_ucache_pool_status_object, pool_status)

PHP_UCACHE_DEFINE_OBJ_FROM_STD(php_ucache_status_object, status)

static php_ucache_object *ucache_object_from_this(zval *this_ptr)
{
	php_ucache_object *cache = ucache_object_from_obj(Z_OBJ_P(this_ptr));

	ZEND_ASSERT(cache->ctx == php_ucache_active_context());

	if (cache->scope_prefix == NULL) {
		zend_throw_error(NULL, "UserCache\\Cache instance was not initialized");

		return NULL;
	}

	return cache;
}

static php_ucache_pool_status_object *ucache_pool_status_from_this(zval *this_ptr)
{
	php_ucache_pool_status_object *status =
		ucache_pool_status_from_obj(Z_OBJ_P(this_ptr))
	;

	if (status->scope == NULL || status->scope_prefix == NULL || status->ctx == NULL) {
		zend_throw_error(NULL, "UserCache\\CachePoolStatus instance was not initialized");

		return NULL;
	}

	return status;
}

static php_ucache_status_object *ucache_status_from_this(zval *this_ptr)
{
	php_ucache_status_object *status =
		ucache_status_from_obj(Z_OBJ_P(this_ptr))
	;

	if (!status->initialized) {
		zend_throw_error(NULL, "UserCache\\CacheStatus instance was not initialized");

		return NULL;
	}

	return status;
}

static bool ucache_validate_non_negative(zend_long value, uint32_t arg_num)
{
	if (value < 0) {
		zend_argument_value_error(arg_num, "must be greater than or equal to 0");

		return false;
	}

	return true;
}

static PHP_UCACHE_HOT bool ucache_can_read(void)
{
	ucache_ensure_ready();

	return php_ucache_active_runtime()->available;
}

static bool ucache_begin_read(void)
{
	if (!ucache_can_read()) {
		return false;
	}

	if (!php_ucache_rlock()) {
		return false;
	}

	if (!php_ucache_header_is_initialized_locked()) {
		php_ucache_unlock();

		return false;
	}

	return true;
}

static bool ucache_can_write(void)
{
	return ucache_can_read();
}

static bool ucache_begin_write(void)
{
	if (!php_ucache_wlock()) {
		return false;
	}

	if (!php_ucache_header_init_locked()) {
		php_ucache_unlock();

		return false;
	}

	return true;
}

static void ucache_pool_note_lookup(php_ucache_object *cache, bool hit)
{
	if (hit) {
		cache->hit_count++;
	} else {
		cache->miss_count++;
	}
}

/* Needs the read lock, or the write lock when create is set. */
static php_ucache_pool_stats_t *ucache_pool_stats_slot_locked(
		php_ucache_header_t *header,
		zend_string *name,
		bool create)
{
	php_ucache_pool_stats_t *slot = NULL, *tombstone = NULL;
	zend_ulong hash = zend_string_hash_val(name);
	uint32_t i, slot_idx = (uint32_t) (hash % PHP_UCACHE_POOL_STATS_SLOTS), name_offset;

	for (i = 0; i < PHP_UCACHE_POOL_STATS_SLOTS; i++) {
		slot = &header->pool_stats[slot_idx];

		if (slot->state == PHP_UCACHE_POOL_STATS_EMPTY) {
			break;
		}

		if (slot->state == PHP_UCACHE_POOL_STATS_TOMBSTONE) {
			if (tombstone == NULL) {
				tombstone = slot;
			}
		} else if (slot->name_hash == hash &&
			slot->name_len == ZSTR_LEN(name) &&
			php_ucache_payload_in_bounds(header, slot->name_offset, slot->name_len) &&
			memcmp(php_ucache_ptr(slot->name_offset), ZSTR_VAL(name), ZSTR_LEN(name)) == 0
		) {
			return slot;
		}

		slot_idx = slot_idx + 1 == PHP_UCACHE_POOL_STATS_SLOTS ? 0 : slot_idx + 1;
	}

	if (!create) {
		return NULL;
	}

	if (tombstone != NULL) {
		slot = tombstone;
	} else if (i == PHP_UCACHE_POOL_STATS_SLOTS) {
		return NULL;
	}

	name_offset = php_ucache_alloc_locked(ZSTR_LEN(name), ZSTR_VAL(name));
	if (name_offset == 0) {
		return NULL;
	}

	slot->name_hash = hash;
	slot->name_offset = name_offset;
	slot->name_len = (uint32_t) ZSTR_LEN(name);
	slot->state = PHP_UCACHE_POOL_STATS_USED;
	slot->reserved = 0;
	slot->hit_count = 0;
	slot->miss_count = 0;

	return slot;
}

static void ucache_pool_stats_drop_locked(php_ucache_header_t *header, zend_string *name)
{
	php_ucache_pool_stats_t *slot = ucache_pool_stats_slot_locked(header, name, false);

	if (slot == NULL) {
		return;
	}

	php_ucache_free_locked(slot->name_offset);

	memset(slot, 0, sizeof(*slot));

	slot->state = PHP_UCACHE_POOL_STATS_TOMBSTONE;
}

/* The segment totals always take the counts; the pool slot may be missing
 * when the table is full or its name block cannot be allocated. */
static void ucache_pool_stats_fold_locked(php_ucache_header_t *header, php_ucache_object *cache)
{
	php_ucache_pool_stats_t *slot;

	if (cache->hit_count == 0 && cache->miss_count == 0) {
		return;
	}

	header->hit_count += cache->hit_count;
	header->miss_count += cache->miss_count;

	slot = ucache_pool_stats_slot_locked(header, cache->scope, true);
	if (slot != NULL) {
		slot->hit_count += cache->hit_count;
		slot->miss_count += cache->miss_count;
	}

	cache->hit_count = 0;
	cache->miss_count = 0;
}

/* Folding bypasses the request availability gate: the counts were taken
 * while the segment was available, and request shutdown (where most
 * folds happen) reports the cache unavailable to user code. */
static bool ucache_pool_stats_begin_write(void)
{
	php_ucache_storage_t *storage = &php_ucache_active_context()->storage;

	if (!storage->initialized ||
		!storage->lock_initialized ||
		UC_G(lock_held) ||
		!php_ucache_wlock()
	) {
		return false;
	}

	if (!php_ucache_header_adoptable_locked()) {
		php_ucache_unlock();

		return false;
	}

	return true;
}

static void ucache_flush_pool_stats(php_ucache_object *cache)
{
	php_ucache_ctx_t *prev_ctx;

	if (cache->hit_count == 0 && cache->miss_count == 0) {
		return;
	}

	prev_ctx = php_ucache_activate_context(cache->ctx);

	if (ucache_pool_stats_begin_write()) {
		ucache_pool_stats_fold_locked(php_ucache_header_ptr(), cache);

		php_ucache_unlock();
	}

	php_ucache_restore_context(prev_ctx);
}

static void ucache_flush_pool_stats_all(void)
{
	php_ucache_object *cache;
	zval *zv;
	bool pending = false;

	if (UC_G(pool_table) == NULL) {
		return;
	}

	ZEND_HASH_FOREACH_VAL(UC_G(pool_table), zv) {
		cache = ucache_object_from_obj(Z_OBJ_P(zv));

		if (cache->ctx == php_ucache_active_context() &&
			(cache->hit_count != 0 || cache->miss_count != 0)
		) {
			pending = true;

			break;
		}
	} ZEND_HASH_FOREACH_END();

	if (!pending || !ucache_pool_stats_begin_write()) {
		return;
	}

	ZEND_HASH_FOREACH_VAL(UC_G(pool_table), zv) {
		cache = ucache_object_from_obj(Z_OBJ_P(zv));

		if (cache->ctx == php_ucache_active_context()) {
			ucache_pool_stats_fold_locked(php_ucache_header_ptr(), cache);
		}
	} ZEND_HASH_FOREACH_END();

	php_ucache_unlock();
}

static bool ucache_delete_storage_key_prevalidated(zend_string *key)
{
	if (!php_ucache_wlock_for_entry_mutation(key)) {
		return false;
	}

	php_ucache_delete_locked(key);

	php_ucache_unlock();

	return true;
}

/* drop_stats_scope names a pool whose lookup counters go with its entries
 * (deletePool); NULL keeps them (clear). */
static bool ucache_clear_scope_prevalidated(
		zend_string *scope_prefix,
		zend_string *drop_stats_scope)
{
	if (!ucache_begin_write()) {
		return false;
	}

	if (!php_ucache_entry_locks_allow_clear_locked()) {
		php_ucache_unlock();

		return false;
	}

	php_ucache_delete_by_prefix_locked(scope_prefix);

	if (drop_stats_scope != NULL) {
		ucache_pool_stats_drop_locked(php_ucache_header_ptr(), drop_stats_scope);
	}

	php_ucache_unlock();
	php_ucache_release_active_request_local_slots_by_prefix(scope_prefix);

	return true;
}

static PHP_UCACHE_HOT bool ucache_fetch_if_present_api(
		zend_string *key,
		zval *return_value)
{
	php_ucache_fetch_pending_seed_t pending_seed;
	bool fetched, entry_found = false, lock_held = true;

	if (!ucache_can_read()) {
		return false;
	}

	UC_G(stack_overflowed) = false;

	switch (php_ucache_fetch_optimistic(key, return_value, true)) {
		case PHP_UCACHE_OPTIMISTIC_FOUND:
			return true;
		case PHP_UCACHE_OPTIMISTIC_MISS:
			return false;
		case PHP_UCACHE_OPTIMISTIC_FALLBACK:
			break;
	}

	if (EG(exception)) {
		if (UC_G(stack_overflowed)) {
			UC_G(stack_overflowed) = false;

			zend_clear_exception();

			return false;
		}

		return false;
	}

	if (!ucache_begin_read()) {
		return false;
	}

	UC_G(stack_overflowed) = false;

	fetched = php_ucache_fetch_locked(key, true, return_value, &entry_found, &pending_seed, &lock_held);

	if (!lock_held) {
		return false;
	}

	php_ucache_unlock();

	if (fetched) {
		if (pending_seed.should_seed_request_local_slot) {
			php_ucache_fetch_finish(key, pending_seed.generation, return_value, pending_seed.flags);
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

	(void) ucache_delete_storage_key_prevalidated(key);

	return false;
}

static bool ucache_fetch_api(
		zend_string *key,
		zval *default_value,
		zval *return_value)
{
	if (ucache_fetch_if_present_api(key, return_value)) {
		return true;
	}

	ZVAL_COPY(return_value, default_value);

	return false;
}

static void ucache_fetch_multiple_fetch_one(
		zend_string *storage_key,
		zval *default_value,
		bool *rlock_held,
		zval *out,
		php_ucache_fetch_pending_seed_t *pending_seed,
		bool *hit)
{
	bool fetched, found, lock_held = true;

	if (!*rlock_held) {
		*hit = ucache_fetch_api(storage_key, default_value, out);

		return;
	}

	UC_G(stack_overflowed) = false;

	fetched = php_ucache_fetch_locked(storage_key, true, out, &found, pending_seed, &lock_held);
	*hit = fetched;

	if (!lock_held) {
		*rlock_held = false;

		if (EG(exception)) {
			ZVAL_UNDEF(out);
		} else {
			ZVAL_COPY(out, default_value);
		}

		return;
	}

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
		php_ucache_unlock();

		*rlock_held = false;

		ZVAL_UNDEF(out);
	} else {
		php_ucache_unlock();
		*rlock_held = false;

		(void) ucache_delete_storage_key_prevalidated(storage_key);

		ZVAL_COPY(out, default_value);
	}
}

static void ucache_finish_fetch_multiple(
		zend_string **prepared_keys,
		zend_string **storage_keys,
		const php_ucache_fetch_pending_seed_t *pending_seeds,
		uint32_t count,
		zval *return_value)
{
	zval *val;
	uint32_t i;

	for (i = 0; i < count; i++) {
		if (pending_seeds[i].should_seed_request_local_slot) {
			val = zend_symtable_find(Z_ARRVAL_P(return_value), prepared_keys[i]);
			if (val != NULL) {
				php_ucache_fetch_finish(
					storage_keys[i],
					pending_seeds[i].generation,
					val,
					pending_seeds[i].flags
				);
			}
		}
	}
}

static PHP_UCACHE_HOT zend_string *ucache_storage_key_cache_lookup(
		php_ucache_object *cache,
		zend_string *key)
{
	php_ucache_storage_key_slot_t *slot;

	if (cache->storage_key_cache == NULL) {
		return NULL;
	}

	slot = &cache->storage_key_cache[
		zend_string_hash_val(key) & (PHP_UCACHE_STORAGE_KEY_CACHE_SLOTS - 1)
	];
	if (slot->key == NULL || !zend_string_equals(slot->key, key)) {
		return NULL;
	}

	return zend_string_copy(slot->storage_key);
}

static zend_string *ucache_storage_key_build(
		php_ucache_object *cache,
		zend_string *key)
{
	php_ucache_storage_key_slot_t *slot;
	zend_string *storage_key;

	storage_key = zend_string_concat2(
		ZSTR_VAL(cache->scope_prefix),
		ZSTR_LEN(cache->scope_prefix),
		ZSTR_VAL(key),
		ZSTR_LEN(key)
	);

	zend_string_hash_val(storage_key);

	if (cache->storage_key_cache == NULL) {
		cache->storage_key_cache = ecalloc(
			PHP_UCACHE_STORAGE_KEY_CACHE_SLOTS,
			sizeof(*cache->storage_key_cache)
		);
	}

	slot = &cache->storage_key_cache[
		zend_string_hash_val(key) & (PHP_UCACHE_STORAGE_KEY_CACHE_SLOTS - 1)
	];
	if (slot->key != NULL) {
		zend_string_release(slot->key);
		zend_string_release(slot->storage_key);
	}

	slot->key = zend_string_copy(key);
	slot->storage_key = zend_string_copy(storage_key);

	return storage_key;
}

static zend_string *ucache_storage_key(
		php_ucache_object *cache,
		zend_string *key)
{
	zend_string *storage_key = ucache_storage_key_cache_lookup(cache, key);

	if (storage_key != NULL) {
		return storage_key;
	}

	return ucache_storage_key_build(cache, key);
}

static PHP_UCACHE_HOT zend_string *ucache_validated_storage_key(
		php_ucache_object *cache,
		zend_string *key,
		uint32_t arg_num)
{
	zend_string *storage_key = ucache_storage_key_cache_lookup(cache, key);

	if (storage_key != NULL) {
		return storage_key;
	}

	if (!ucache_validate_key(key, arg_num)) {
		return NULL;
	}

	return ucache_storage_key_build(cache, key);
}

static zend_result ucache_fetch_multiple_api(
		php_ucache_object *cache,
		HashTable *keys,
		zval *default_value,
		zval *return_value)
{
	php_ucache_fetch_pending_seed_t *pending_seeds;
	zend_string **prepared_keys, **storage_keys;
	zend_result result;
	zval *vals;
	uint32_t count, i, p, pending_count = 0, *pending_idx;
	bool rlock_held, backend_readable, *hits;

	if (!ucache_prepare_key_list(keys, 1, &prepared_keys, &count)) {
		return FAILURE;
	}

	backend_readable = ucache_can_read();

	storage_keys = NULL;
	pending_seeds = NULL;
	pending_idx = NULL;
	vals = NULL;
	hits = NULL;

	if (count != 0) {
		storage_keys = safe_emalloc(count, sizeof(zend_string *), 0);
		pending_seeds = safe_emalloc(count, sizeof(php_ucache_fetch_pending_seed_t), 0);
		pending_idx = safe_emalloc(count, sizeof(uint32_t), 0);
		vals = safe_emalloc(count, sizeof(zval), 0);
		hits = safe_emalloc(count, sizeof(bool), 0);

		memset(pending_seeds, 0, count * sizeof(php_ucache_fetch_pending_seed_t));
		memset(hits, 0, count * sizeof(bool));

		for (i = 0; i < count; i++) {
			storage_keys[i] = ucache_storage_key(cache, prepared_keys[i]);

			ZVAL_UNDEF(&vals[i]);
		}
	}

	for (i = 0; backend_readable && i < count && !EG(exception); i++) {
		UC_G(stack_overflowed) = false;

		switch (php_ucache_fetch_optimistic(storage_keys[i], &vals[i], false)) {
			case PHP_UCACHE_OPTIMISTIC_FOUND:
				hits[i] = true;

				continue;
			case PHP_UCACHE_OPTIMISTIC_MISS:
				continue;
			case PHP_UCACHE_OPTIMISTIC_FALLBACK:
				break;
		}

		if (EG(exception)) {
			if (UC_G(stack_overflowed)) {
				UC_G(stack_overflowed) = false;
				zend_clear_exception();

				continue;
			}

			break;
		}

		pending_idx[pending_count++] = i;
	}

	if (pending_count != 0 && !EG(exception)) {
		if (ucache_begin_read()) {
			rlock_held = true;

			zend_try {
				for (p = 0; p < pending_count; p++) {
					i = pending_idx[p];

					ucache_fetch_multiple_fetch_one(
						storage_keys[i],
						default_value,
						&rlock_held,
						&vals[i],
						&pending_seeds[i],
						&hits[i]
					);

					if (EG(exception)) {
						break;
					}
				}
			} zend_catch {
				php_ucache_unlock_if_held();

				zend_bailout();
			} zend_end_try();

			if (rlock_held) {
				php_ucache_unlock();
			}
		}
	}

	if (pending_idx != NULL) {
		efree(pending_idx);
	}

	if (EG(exception)) {
		for (i = 0; i < count; i++) {
			zval_ptr_dtor(&vals[i]);
		}

		result = FAILURE;
	} else {
		array_init_size(return_value, count);

		for (i = 0; i < count; i++) {
			if (Z_ISUNDEF(vals[i])) {
				ZVAL_COPY(&vals[i], default_value);
			}

			/* Ownership of vals[i] moves into return_value. */
			zend_symtable_update(Z_ARRVAL_P(return_value), prepared_keys[i], &vals[i]);
		}

		ucache_finish_fetch_multiple(prepared_keys, storage_keys, pending_seeds, count, return_value);

		for (i = 0; i < count; i++) {
			ucache_pool_note_lookup(cache, hits[i]);
		}

		result = SUCCESS;
	}

	if (hits != NULL) {
		efree(hits);
	}

	if (vals != NULL) {
		efree(vals);
	}

	ucache_release_key_list(storage_keys, count);

	if (pending_seeds != NULL) {
		efree(pending_seeds);
	}

	ucache_release_key_list(prepared_keys, count);

	return result;
}

static bool ucache_atomic_update_api(
		zend_string *key,
		zend_long step,
		zend_long ttl,
		bool decrement,
		php_ucache_atomic_update_result_t *result)
{
	bool updated;

	if (!ucache_can_write() ||
		!php_ucache_wlock_for_entry_mutation(key)
	) {
		memset(result, 0, sizeof(*result));

		return false;
	}

	updated = php_ucache_atomic_update_locked(key, step, ttl, decrement, result);

	php_ucache_unlock();

	return updated;
}

static bool ucache_exists_api(zend_string *key)
{
	bool exists;

	if (!ucache_can_read()) {
		return false;
	}

	switch (php_ucache_exists_optimistic(key)) {
		case PHP_UCACHE_OPTIMISTIC_FOUND:
			return true;
		case PHP_UCACHE_OPTIMISTIC_MISS:
			return false;
		case PHP_UCACHE_OPTIMISTIC_FALLBACK:
			break;
	}

	if (!ucache_begin_read()) {
		return false;
	}

	exists = php_ucache_exists_locked(key);

	php_ucache_unlock();

	return exists;
}

static bool ucache_lock_api(
		zend_string *key,
		zend_long lease)
{
	if (!ucache_can_write()) {
		return false;
	}

	return php_ucache_try_acquire_entry_lock(key, lease);
}

static bool ucache_unlock_api(zend_string *key)
{
	if (!ucache_can_write()) {
		return false;
	}

	return php_ucache_release_entry_lock(key);
}

static zend_long ucache_count_to_zend_long(uint64_t count)
{
	return count > (uint64_t) ZEND_LONG_MAX ? ZEND_LONG_MAX : (zend_long) count;
}

static zend_long ucache_size_to_zend_long(size_t size)
{
	return ucache_count_to_zend_long(size);
}

static size_t ucache_sum_wasted_memory_locked(php_ucache_header_t *header)
{
	php_ucache_block_t *block;
	uint64_t block_limit;
	uint32_t block_offset, iter_limit, iters;
	size_t wasted_memory = 0;

	if (header->next_free > header->data_size) {
		return 0;
	}

	block_limit = (uint64_t) header->data_offset + header->next_free;
	iter_limit = header->data_size / PHP_UCACHE_BLOCK_HEADER_UNITS;
	for (block_offset = header->free_list, iters = 0;
		block_offset != 0 && iters < iter_limit;
		iters++
	) {
		if (block_offset < header->data_offset ||
			(uint64_t) block_offset + PHP_UCACHE_BLOCK_HEADER_UNITS > block_limit
		) {
			break;
		}

		block = php_ucache_block_ptr(block_offset);
		if (block->size < PHP_UCACHE_BLOCK_HEADER_UNITS ||
			(uint64_t) block_offset + block->size > block_limit
		) {
			break;
		}

		wasted_memory += php_ucache_shm_bytes(block->size);
		block_offset = block->next_free;
	}

	return wasted_memory;
}

static void ucache_collect_info_stats(php_ucache_info_stats_t *stats)
{
	php_ucache_runtime_t *runtime;
	php_ucache_storage_t *storage;
	php_ucache_header_t *header;
	php_ucache_graph_pin_slot_t *pin_slot;
	size_t free_memory = 0, wasted_memory = 0, tail_memory = 0;
	uint32_t i;

	memset(stats, 0, sizeof(*stats));

	runtime = php_ucache_active_runtime();
	storage = &php_ucache_active_context()->storage;

	stats->configured_memory = ucache_size_to_zend_long(runtime->configured_memory);
	stats->shared_memory_size = ucache_size_to_zend_long(storage->size);

	if (!storage->initialized ||
		!storage->lock_initialized ||
		!php_ucache_rlock()
	) {
		return;
	}

	header = php_ucache_header_ptr();
	if (header == NULL || !php_ucache_header_adoptable_locked()) {
		stats->free_memory = stats->shared_memory_size;

		php_ucache_unlock();

		return;
	}

	if (header->next_free <= header->data_size) {
		tail_memory = php_ucache_shm_bytes(header->data_size - header->next_free);
	}

	wasted_memory = ucache_sum_wasted_memory_locked(header);
	free_memory = tail_memory + wasted_memory;

	stats->entry_count = (zend_long) header->count;
	stats->entry_capacity = (zend_long) header->capacity;
	stats->tombstone_count = (zend_long) header->tombstone_count;
	stats->expunge_count = ucache_count_to_zend_long(header->expunge_count);
	stats->store_failure_count = ucache_count_to_zend_long(header->store_failure_count);
	stats->eviction_count = ucache_count_to_zend_long(header->eviction_count);
	stats->dead_pin_owners_reclaimed = ucache_count_to_zend_long(header->graph_dead_pin_owners_reclaimed);
	stats->dead_pins_stripped = ucache_count_to_zend_long(header->graph_dead_pins_stripped);
	stats->hit_count = ucache_count_to_zend_long(header->hit_count);
	stats->miss_count = ucache_count_to_zend_long(header->miss_count);
	stats->interned_key_count = (zend_long) header->intern_count;

	for (i = 0; i < PHP_UCACHE_GRAPH_PIN_SLOTS; i++) {
		pin_slot = &header->graph_pin_slots[i];

		if (zend_atomic_int_load_ex(&pin_slot->owner_pid) != 0) {
			stats->graph_pin_slots_in_use++;
		}

		stats->graph_pinned_references +=
			(zend_long) zend_atomic_int_load_ex(&pin_slot->pin_count)
		;
	}

	stats->free_memory = ucache_size_to_zend_long(free_memory);
	stats->wasted_memory = ucache_size_to_zend_long(wasted_memory);
	stats->used_memory = storage->size > free_memory
		? ucache_size_to_zend_long(storage->size - free_memory)
		: 0
	;

	php_ucache_unlock();
}

static zend_object *ucache_availability_enum_case(php_ucache_reason_t reason)
{
	zend_enum_UserCache_CacheAvailability case_id = ZEND_ENUM_UserCache_CacheAvailability_UnavailableByUnknownReason;

	switch (reason) {
		case PHP_UCACHE_REASON_NONE:
			case_id = ZEND_ENUM_UserCache_CacheAvailability_Available;

			break;
		case PHP_UCACHE_REASON_DISABLED_BY_INI:
			case_id = ZEND_ENUM_UserCache_CacheAvailability_DisabledByIni;

			break;
		case PHP_UCACHE_REASON_SHM_INIT_FAILED:
			case_id = ZEND_ENUM_UserCache_CacheAvailability_UnavailableBySharedMemoryInitializationFailed;

			break;
		case PHP_UCACHE_REASON_SAPI_NOT_ENABLED:
			case_id = ZEND_ENUM_UserCache_CacheAvailability_DisabledBySapi;

			break;
		case PHP_UCACHE_REASON_BACKEND_NOT_INITIALIZED_BEFORE_WORKER:
			case_id = ZEND_ENUM_UserCache_CacheAvailability_UnavailableByBackendNotInitializedBeforeWorkerStartup;

			break;
		case PHP_UCACHE_REASON_BACKEND_INITIALIZED_AFTER_WORKER:
			case_id = ZEND_ENUM_UserCache_CacheAvailability_UnavailableByBackendInitializedAfterWorkerStartup;

			break;
		case PHP_UCACHE_REASON_CGI_BOUNDARY_UNAVAILABLE:
			case_id = ZEND_ENUM_UserCache_CacheAvailability_UnavailableByCgiFastCgiBoundary;

			break;
		case PHP_UCACHE_REASON_APACHE_BOUNDARY_UNAVAILABLE:
			case_id = ZEND_ENUM_UserCache_CacheAvailability_UnavailableByApacheBoundary;

			break;
		case PHP_UCACHE_REASON_LSAPI_BOUNDARY_UNAVAILABLE:
			case_id = ZEND_ENUM_UserCache_CacheAvailability_UnavailableByLsapiBoundary;

			break;
		case PHP_UCACHE_REASON_REQUEST_SHUTDOWN:
			case_id = ZEND_ENUM_UserCache_CacheAvailability_UnavailableByUnknownReason;

			break;
	}

	return zend_enum_get_case_by_id(ucache_availability_ce, case_id);
}

static void ucache_return_status(zval *return_value)
{
	php_ucache_status_object *status;

	ucache_ensure_ready();
	ucache_flush_pool_stats_all();

	object_init_ex(return_value, ucache_status_ce);
	status = ucache_status_from_obj(Z_OBJ_P(return_value));

	status->availability_reason = php_ucache_active_runtime()->failure_reason;

	ucache_collect_info_stats(&status->stats);

	status->initialized = true;
}

static void ucache_add_pool_memory(size_t *used_memory, size_t size)
{
	if (*used_memory >= (size_t) ZEND_LONG_MAX ||
		size > (size_t) ZEND_LONG_MAX - *used_memory
	) {
		*used_memory = (size_t) ZEND_LONG_MAX;

		return;
	}

	*used_memory += size;
}

static size_t ucache_payload_block_size(
		const php_ucache_header_t *header,
		uint32_t payload_offset)
{
	php_ucache_block_t *block;

	if (!php_ucache_payload_in_bounds(header, payload_offset, 0)) {
		return 0;
	}

	block = php_ucache_block_ptr(payload_offset - PHP_UCACHE_BLOCK_HEADER_UNITS);
	if (block->size < PHP_UCACHE_BLOCK_HEADER_UNITS ||
		!php_ucache_payload_in_bounds(
			header,
			payload_offset,
			php_ucache_shm_bytes(block->size - PHP_UCACHE_BLOCK_HEADER_UNITS)
		)
	) {
		return 0;
	}

	return php_ucache_shm_bytes(block->size);
}

static void ucache_account_pool_entry(
		const php_ucache_header_t *header,
		const php_ucache_entry_t *entry,
		zend_string *prefix,
		zend_long *entry_count,
		size_t *used_memory,
		zval *entry_keys)
{
	const char *key;
	size_t prefix_len;
	bool combined_value_key;

	combined_value_key = (entry->flags & PHP_UCACHE_ENTRY_FLAG_COMBINED_VALUE_KEY) != 0;
	key = (const char *) php_ucache_ptr(entry->key_offset);
	prefix_len = ZSTR_LEN(prefix);

	(*entry_count)++;

	add_next_index_stringl(entry_keys, key + prefix_len, entry->key_len - prefix_len);

	ucache_add_pool_memory(used_memory, sizeof(php_ucache_entry_t));

	if (combined_value_key) {
		ucache_add_pool_memory(
			used_memory,
			ucache_payload_block_size(header, entry->value_offset)
		);
	} else {
		ucache_add_pool_memory(
			used_memory,
			ucache_payload_block_size(header, entry->key_offset)
		);
		ucache_add_pool_memory(
			used_memory,
			ucache_payload_block_size(header, entry->value_offset)
		);
	}
}

static void ucache_collect_pool_status(
		php_ucache_ctx_t *ctx,
		zend_string *scope,
		zend_string *prefix,
		zend_long *entry_count,
		zend_long *used_memory,
		zend_long *hit_count,
		zend_long *miss_count,
		zval *entry_keys)
{
	php_ucache_ctx_t *prev_ctx;
	php_ucache_storage_t *storage;
	php_ucache_header_t *header;
	php_ucache_entry_t *entries, *entry;
	php_ucache_pool_stats_t *stats_slot;
	uint32_t i;
	size_t used_size = 0;

	*entry_count = 0;
	*used_memory = 0;
	*hit_count = 0;
	*miss_count = 0;

	prev_ctx = php_ucache_activate_context(ctx);

	storage = &php_ucache_active_context()->storage;
	if (!storage->initialized || !storage->lock_initialized || !php_ucache_rlock()) {
		php_ucache_restore_context(prev_ctx);

		return;
	}

	header = php_ucache_header_ptr();
	if (header == NULL ||
		!php_ucache_header_adoptable_locked()
	) {
		php_ucache_unlock();
		php_ucache_restore_context(prev_ctx);

		return;
	}

	entries = php_ucache_entries_ptr(header);

	stats_slot = ucache_pool_stats_slot_locked(header, scope, false);
	if (stats_slot != NULL) {
		*hit_count = ucache_count_to_zend_long(stats_slot->hit_count);
		*miss_count = ucache_count_to_zend_long(stats_slot->miss_count);
	}

	zend_try {
		for (i = php_ucache_occupancy_next_used(header, 0);
			i != UINT32_MAX;
			i = php_ucache_occupancy_next_used(header, i + 1)
		) {
			entry = &entries[i];
			if (php_ucache_payload_in_bounds(header, entry->key_offset, entry->key_len) &&
				entry->key_len >= ZSTR_LEN(prefix) &&
				memcmp(php_ucache_ptr(entry->key_offset), ZSTR_VAL(prefix), ZSTR_LEN(prefix)) == 0
			) {
				ucache_account_pool_entry(
					header,
					entry,
					prefix,
					entry_count,
					&used_size,
					entry_keys
				);
			}
		}
	} zend_catch {
		php_ucache_unlock_if_held();
		php_ucache_restore_context(prev_ctx);

		zend_bailout();
	} zend_end_try();

	php_ucache_unlock();
	php_ucache_restore_context(prev_ctx);

	*used_memory = ucache_size_to_zend_long(used_size);
}

static void ucache_return_pool_status(
		php_ucache_object *cache,
		zval *return_value)
{
	php_ucache_pool_status_object *status;

	ucache_ensure_ready();
	ucache_flush_pool_stats(cache);

	object_init_ex(return_value, ucache_pool_status_ce);

	status = ucache_pool_status_from_obj(Z_OBJ_P(return_value));
	status->scope = zend_string_copy(cache->scope);
	status->scope_prefix = zend_string_copy(cache->scope_prefix);
	status->ctx = cache->ctx;

	array_init(&status->entry_keys);

	ucache_collect_pool_status(
		status->ctx,
		status->scope,
		status->scope_prefix,
		&status->entry_count,
		&status->used_memory,
		&status->hit_count,
		&status->miss_count,
		&status->entry_keys
	);
}

static void ucache_object_free(zend_object *obj)
{
	php_ucache_object *cache = ucache_object_from_obj(obj);
	uint32_t i;

	/* Pools no longer in the request table fold their counts here. */
	if (cache->scope != NULL) {
		ucache_flush_pool_stats(cache);
		zend_string_release(cache->scope);
	}

	if (cache->scope_prefix != NULL) {
		zend_string_release(cache->scope_prefix);
	}

	if (cache->storage_key_cache != NULL) {
		for (i = 0; i < PHP_UCACHE_STORAGE_KEY_CACHE_SLOTS; i++) {
			if (cache->storage_key_cache[i].key != NULL) {
				zend_string_release(cache->storage_key_cache[i].key);
				zend_string_release(cache->storage_key_cache[i].storage_key);
			}
		}

		efree(cache->storage_key_cache);
	}

	zend_object_std_dtor(&cache->std);
}

static void ucache_pool_status_object_free(zend_object *obj)
{
	php_ucache_pool_status_object *status =
		ucache_pool_status_from_obj(obj)
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

static zend_object *ucache_object_create(zend_class_entry *ce)
{
	php_ucache_object *cache;

	cache = zend_object_alloc(sizeof(php_ucache_object), ce);

	zend_object_std_init(&cache->std, ce);
	object_properties_init(&cache->std, ce);

	cache->scope = NULL;
	cache->scope_prefix = NULL;
	cache->storage_key_cache = NULL;
	cache->hit_count = 0;
	cache->miss_count = 0;
	cache->ctx = php_ucache_owning_context();
	cache->std.handlers = &ucache_object_handlers;

	return &cache->std;
}

static zend_object *ucache_pool_status_object_create(zend_class_entry *ce)
{
	php_ucache_pool_status_object *status;

	status = zend_object_alloc(sizeof(php_ucache_pool_status_object), ce);

	zend_object_std_init(&status->std, ce);
	object_properties_init(&status->std, ce);

	status->scope = NULL;
	status->scope_prefix = NULL;
	status->ctx = NULL;
	status->entry_count = 0;
	status->used_memory = 0;
	status->hit_count = 0;
	status->miss_count = 0;

	ZVAL_UNDEF(&status->entry_keys);

	status->std.handlers = &ucache_pool_status_object_handlers;

	return &status->std;
}

static zend_object *ucache_status_object_create(zend_class_entry *ce)
{
	php_ucache_status_object *status;

	status = zend_object_alloc(sizeof(php_ucache_status_object), ce);

	zend_object_std_init(&status->std, ce);
	object_properties_init(&status->std, ce);

	memset(&status->stats, 0, sizeof(status->stats));

	status->availability_reason = PHP_UCACHE_REASON_NONE;
	status->initialized = false;
	status->std.handlers = &ucache_status_object_handlers;

	return &status->std;
}

static void ucache_register_classes(void)
{
	if (ucache_ce != NULL) {
		return;
	}

	ucache_availability_ce = register_class_UserCache_CacheAvailability();
	ucache_status_ce = register_class_UserCache_CacheStatus();
	ucache_pool_status_ce = register_class_UserCache_CachePoolStatus();
	ucache_ce = register_class_UserCache_Cache();

	ucache_ce->create_object = ucache_object_create;
	ucache_status_ce->create_object = ucache_status_object_create;
	ucache_pool_status_ce->create_object = ucache_pool_status_object_create;

	memcpy(
		&ucache_object_handlers,
		zend_get_std_object_handlers(),
		sizeof(zend_object_handlers)
	);

	ucache_object_handlers.offset = offsetof(php_ucache_object, std);
	ucache_object_handlers.free_obj = ucache_object_free;
	ucache_object_handlers.clone_obj = NULL;

	memcpy(
		&ucache_status_object_handlers,
		zend_get_std_object_handlers(),
		sizeof(zend_object_handlers)
	);

	ucache_status_object_handlers.offset = offsetof(php_ucache_status_object, std);
	ucache_status_object_handlers.clone_obj = NULL;

	memcpy(
		&ucache_pool_status_object_handlers,
		zend_get_std_object_handlers(),
		sizeof(zend_object_handlers)
	);

	ucache_pool_status_object_handlers.offset = offsetof(php_ucache_pool_status_object, std);
	ucache_pool_status_object_handlers.free_obj = ucache_pool_status_object_free;
	ucache_pool_status_object_handlers.clone_obj = NULL;
}

static void ucache_reset_class_entries(void)
{
	ucache_availability_ce = NULL;
	ucache_status_ce = NULL;
	ucache_pool_status_ce = NULL;
	ucache_ce = NULL;
}

static HashTable *ucache_pools(void)
{
	if (UC_G(pool_table) == NULL) {
		ALLOC_HASHTABLE(UC_G(pool_table));
		zend_hash_init(UC_G(pool_table), 8, NULL, ZVAL_PTR_DTOR, 0);
	}

	return UC_G(pool_table);
}

static void ucache_release_pools(void)
{
	if (UC_G(pool_table) == NULL) {
		return;
	}

	zend_hash_destroy(UC_G(pool_table));
	FREE_HASHTABLE(UC_G(pool_table));

	UC_G(pool_table) = NULL;
}

static bool ucache_validate_pool_name(zend_string *pool, uint32_t arg_num)
{
	return ucache_validate_delimiter_free(pool, arg_num, "must not be empty");
}

static zend_object *ucache_create_pool_object(zend_string *pool)
{
	zend_object *obj = ucache_object_create(ucache_ce);
	php_ucache_object *cache = ucache_object_from_obj(obj);

	cache->scope = zend_string_copy(pool);
	cache->scope_prefix = ucache_build_scope_prefix(pool);

	return obj;
}

static zend_object *ucache_get_or_create_pool(zend_string *pool)
{
	zend_object *obj;
	zval *zv;

	zv = zend_hash_lookup(ucache_pools(), pool);
	if (Z_ISNULL_P(zv)) {
		ZVAL_OBJ(zv, ucache_create_pool_object(pool));
	}

	obj = Z_OBJ_P(zv);

	GC_ADDREF(obj);

	return obj;
}

static void ucache_safe_direct_handlers_dtor(zval *zv)
{
	pefree(Z_PTR_P(zv), true);
}

static bool ucache_store_storage_key_prevalidated(zend_string *key, zval *value, zend_long ttl, bool add_only)
{
	php_ucache_prepare_options_t prep_opts = {
		.caller_holds_write_lock = false,
	};
	php_ucache_store_options_t store_opts = {
		.retry_after_memory_pressure = true,
		.capture_replaced_entry = false,
	};
	php_ucache_prepared_value_t prepared;
	php_ucache_store_result_t result;
	bool stored;

	if (!php_ucache_prepare_value(key, value, &prep_opts, &prepared)) {
		php_ucache_destroy_prepared_value(&prepared);

		return false;
	}

	if (!php_ucache_wlock_for_entry_mutation(key)) {
		php_ucache_destroy_prepared_value(&prepared);

		return false;
	}

	if (add_only && php_ucache_exists_locked(key)) {
		php_ucache_unlock();
		php_ucache_destroy_prepared_value(&prepared);

		return false;
	}

	zend_try {
		stored = php_ucache_store_prepared_locked(
			key,
			value,
			&prepared,
			ttl,
			&store_opts,
			&result
		);
	} zend_catch {
		php_ucache_unlock_if_held();
		php_ucache_destroy_prepared_value(&prepared);

		zend_bailout();
	} zend_end_try();

	php_ucache_unlock();
	php_ucache_destroy_prepared_value(&prepared);

	if (stored && result.should_seed_request_local_slot) {
		php_ucache_store_request_local_slot(key, result.stored_generation, value, true);
	}

	return stored;
}

static int ucache_bulk_order_compare(const void *lhs_ptr, const void *rhs_ptr)
{
	const php_ucache_bulk_order_t *lhs, *rhs;
	int result;

	lhs = lhs_ptr;
	rhs = rhs_ptr;

	if (lhs->hash != rhs->hash) {
		return ZEND_THREEWAY_COMPARE(lhs->hash, rhs->hash);
	}

	result = zend_binary_strcmp(
		ZSTR_VAL(lhs->key),
		ZSTR_LEN(lhs->key),
		ZSTR_VAL(rhs->key),
		ZSTR_LEN(rhs->key)
	);
	if (result != 0) {
		return result;
	}

	return ZEND_THREEWAY_COMPARE(lhs->index, rhs->index);
}

static bool ucache_bulk_lock_keys_ordered(
		zend_string **storage_keys,
		php_ucache_bulk_order_t *order,
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

	qsort(order, count, sizeof(*order), ucache_bulk_order_compare);

	sorted_keys = safe_emalloc(count, sizeof(*sorted_keys), 0);
	sorted_acquired = safe_emalloc(count, sizeof(*sorted_acquired), 0);

	for (i = 0; i < count; i++) {
		sorted_keys[i] = storage_keys[order[i].index];
	}

	result = php_ucache_acquire_entry_locks(sorted_keys, sorted_acquired, count);

	for (i = 0; i < count; i++) {
		acquired[order[i].index] = sorted_acquired[i];
	}

	efree(sorted_acquired);
	efree(sorted_keys);

	return result;
}

static uint32_t ucache_prepare_bulk_store_items(
		php_ucache_object *cache,
		HashTable *values,
		php_ucache_bulk_store_item_t *items,
		zend_string **storage_keys,
		const php_ucache_prepare_options_t *prep_opts,
		bool *result)
{
	zend_ulong num_key;
	zend_string *key, *storage_key;
	zval *value;
	uint32_t i = 0;

	*result = true;

	ZEND_HASH_FOREACH_KEY_VAL(values, num_key, key, value) {
		if (key != NULL) {
			storage_key = ucache_storage_key(cache, key);
		} else {
			key = zend_long_to_str(num_key);
			storage_key = ucache_storage_key(cache, key);

			zend_string_release(key);
		}

		if (!php_ucache_prepare_value(storage_key, value, prep_opts, &items[i].prepared)) {
			php_ucache_destroy_prepared_value(&items[i].prepared);

			zend_string_release(storage_key);

			*result = false;

			break;
		}

		storage_keys[i] = storage_key;
		items[i].value = value;

		i++;
	} ZEND_HASH_FOREACH_END();

	return i;
}

#if ZEND_DEBUG
/* Debug-only fault injection: force a bailout escaping the commit loop after
 * the first item commits, exercising the replaced-entry rollback path that is
 * otherwise unreachable from userland. */
static bool ucache_debug_force_bulk_commit_bailout(void)
{
	const char *value = getenv("USER_CACHE_DEBUG_FORCE_BULK_COMMIT_BAILOUT");

	return value != NULL && value[0] != '\0' && value[0] != '0';
}
#endif /* ZEND_DEBUG */

static uint32_t ucache_commit_bulk_store_locked(
		php_ucache_bulk_store_item_t *items,
		zend_string **storage_keys,
		uint32_t prepared_count,
		zend_long ttl,
		const php_ucache_store_options_t *store_opts,
		bool *result)
{
	uint32_t i, j, stored_count = 0;

	*result = true;

	UC_G(store_defer_unlock) = true;

	for (i = 0; i < prepared_count; i++) {
		if (!php_ucache_store_prepared_locked(
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
		if (i == 0 && prepared_count > 1 && ucache_debug_force_bulk_commit_bailout()) {
			zend_bailout();
		}
#endif /* ZEND_DEBUG */
	}

	UC_G(store_defer_unlock) = false;

	if (*result) {
		for (i = 0; i < stored_count; i++) {
			php_ucache_discard_replaced_entry_locked(
				storage_keys[i],
				&items[i].store_result.replaced_entry
			);

			items[i].committed = false;
		}
	} else {
		for (j = stored_count; j > 0; j--) {
			php_ucache_rollback_replaced_entry_locked(
				storage_keys[j - 1],
				&items[j - 1].store_result.replaced_entry
			);

			items[j - 1].committed = false;
		}

		stored_count = 0;
	}

	return stored_count;
}

static void ucache_abort_bulk_store_on_bailout(
		php_ucache_bulk_store_item_t *items,
		zend_string **storage_keys,
		uint32_t prepared_count)
{
	uint32_t j;

	UC_G(store_defer_unlock) = false;

	if (!UC_G(lock_held) && !php_ucache_wlock()) {
		return;
	}

	for (j = prepared_count; j > 0; j--) {
		if (items[j - 1].committed) {
			php_ucache_rollback_replaced_entry_locked(
				storage_keys[j - 1],
				&items[j - 1].store_result.replaced_entry
			);

			items[j - 1].committed = false;
		}
	}

	php_ucache_unlock_if_held();
}

static void ucache_finish_bulk_store(
		php_ucache_bulk_store_item_t *items,
		zend_string **storage_keys,
		uint32_t stored_count)
{
	uint32_t i;

	for (i = 0; i < stored_count; i++) {
		if (items[i].store_result.should_seed_request_local_slot) {
			php_ucache_store_request_local_slot(
				storage_keys[i],
				items[i].store_result.stored_generation,
				items[i].value,
				true
			);
		}
	}
}

static bool ucache_instance_store_multiple(
		php_ucache_object *cache,
		HashTable *values,
		zend_long ttl)
{
	php_ucache_bulk_store_item_t *items;
	php_ucache_bulk_order_t *order;
	php_ucache_prepare_options_t prep_opts = {
		.caller_holds_write_lock = false,
	};
	php_ucache_store_options_t store_opts = {
		.retry_after_memory_pressure = false,
		.capture_replaced_entry = true,
	};
	zend_string **storage_keys;
	uint32_t i, count, prepared_count, stored_count = 0;
	bool result, *acquired;

	if (!ucache_can_write()) {
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

	prepared_count = ucache_prepare_bulk_store_items(
		cache,
		values,
		items,
		storage_keys,
		&prep_opts,
		&result
	);

	if (result) {
		result = ucache_bulk_lock_keys_ordered(storage_keys, order, acquired, prepared_count);
	}

	if (result && ucache_begin_write()) {
		zend_try {
			stored_count = ucache_commit_bulk_store_locked(
				items,
				storage_keys,
				prepared_count,
				ttl,
				&store_opts,
				&result
			);
		} zend_catch {
			ucache_abort_bulk_store_on_bailout(items, storage_keys, prepared_count);

			zend_bailout();
		} zend_end_try();

		php_ucache_unlock();
	} else {
		result = false;
	}

	php_ucache_release_entry_locks(storage_keys, acquired, prepared_count);

	ucache_finish_bulk_store(items, storage_keys, stored_count);

	for (i = 0; i < prepared_count; i++) {
		php_ucache_destroy_prepared_value(&items[i].prepared);
	}

	ucache_release_key_list(storage_keys, prepared_count);

	efree(acquired);
	efree(order);
	efree(items);

	return result;
}

static bool ucache_instance_delete_multiple(
		php_ucache_object *cache,
		HashTable *keys)
{
	php_ucache_bulk_order_t *order;
	zend_string **prepared_keys, **storage_keys;
	uint32_t i, count;
	bool *acquired, result = true;

	if (!ucache_prepare_key_list(keys, 1, &prepared_keys, &count)) {
		return false;
	}

	if (!ucache_can_write()) {
		ucache_release_key_list(prepared_keys, count);

		return !EG(exception);
	}

	if (count == 0) {
		ucache_release_key_list(prepared_keys, count);

		return true;
	}

	storage_keys = safe_emalloc(count, sizeof(zend_string *), 0);
	order = safe_emalloc(count, sizeof(php_ucache_bulk_order_t), 0);
	acquired = ecalloc(count, sizeof(bool));

	for (i = 0; i < count; i++) {
		storage_keys[i] = ucache_storage_key(cache, prepared_keys[i]);
	}

	result = ucache_bulk_lock_keys_ordered(storage_keys, order, acquired, count);

	if (result && ucache_begin_write()) {
		zend_try {
			for (i = 0; i < count; i++) {
				php_ucache_delete_locked(storage_keys[i]);
			}
		} zend_catch {
			php_ucache_unlock_if_held();

			zend_bailout();
		} zend_end_try();

		php_ucache_unlock();
	} else {
		result = false;
	}

	php_ucache_release_entry_locks(storage_keys, acquired, count);

	ucache_release_key_list(storage_keys, count);

	efree(acquired);
	efree(order);

	ucache_release_key_list(prepared_keys, count);

	return result;
}

static void ucache_init_partition_context(php_ucache_partition_t *partition, const char *name)
{
	partition->ctx = php_ucache_ctx_state;
	partition->ctx.storage.lock_file = -1;

	if (name != NULL) {
		partition->name = pestrdup(name, true);
		partition->ctx.lock_name = partition->name;
	}
}

static bool ucache_startup_storage_for_context(php_ucache_ctx_t *ctx)
{
	php_ucache_ctx_t *prev_ctx;

	prev_ctx = php_ucache_activate_context(ctx);

	php_ucache_reset_runtime();
	if (php_ucache_active_runtime()->enabled && UC_G(enable)) {
		if (!php_ucache_startup_storage_before_request()) {
			php_ucache_restore_context(prev_ctx);

			return false;
		}
	}

	php_ucache_restore_context(prev_ctx);

	return true;
}

static void ucache_partitions_shutdown(void)
{
	php_ucache_partition_t *partition, *next;
	php_ucache_ctx_t *prev_ctx;

	prev_ctx = UC_G(active_context_ptr);

	partition = php_ucache_partitions;
	while (partition != NULL) {
		next = partition->next;

		php_ucache_activate_context(&partition->ctx);
		php_ucache_shutdown_storage();
		php_ucache_reset_runtime();

		pefree(partition->name, true);
		pefree(partition, true);

		partition = next;
	}

	php_ucache_partitions = NULL;

	php_ucache_restore_context(prev_ctx);
}

static bool ucache_format_boundary_key_prefix(char *prefix, size_t prefix_size, size_t *prefix_len)
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
#else /* !(!defined(ZEND_WIN32) && defined(HAVE_UNISTD_H)) */
	prefix[0] = '\0';
	*prefix_len = 0;
	(void) prefix_size;
#endif /* !defined(ZEND_WIN32) && defined(HAVE_UNISTD_H) */

	return true;
}

static char *ucache_build_boundary_key(
		const char *boundary,
		size_t boundary_len,
		size_t *key_len)
{
	size_t prefix_len;
	char *boundary_key, prefix[64];

	if (!ucache_format_boundary_key_prefix(prefix, sizeof(prefix), &prefix_len)) {
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
	memcpy(boundary_key + prefix_len, boundary, boundary_len);
	boundary_key[prefix_len + boundary_len] = '\0';

	*key_len = prefix_len + boundary_len;

	return boundary_key;
}

static php_ucache_boundary_partition_t *ucache_find_boundary_partition(
		const char *boundary,
		size_t boundary_len,
		zend_ulong boundary_hash)
{
	php_ucache_boundary_partition_t *entry;

	for (entry = ucache_boundary_partitions; entry != NULL; entry = entry->next) {
		if (entry->boundary_hash == boundary_hash &&
			entry->boundary_len == boundary_len &&
			memcmp(entry->boundary, boundary, boundary_len) == 0
		) {
			return entry;
		}
	}

	return NULL;
}

static php_ucache_boundary_partition_t *ucache_create_boundary_partition(
		const char *sapi_prefix,
		const char *boundary,
		size_t boundary_len,
		zend_ulong boundary_hash)
{
	php_ucache_boundary_partition_t *entry;
	char partition_name[128], limit_message[256];

	if (ucache_boundary_creation_disabled) {
		return NULL;
	}

	if (ucache_boundary_partition_count >= PHP_UCACHE_MAX_BOUNDARY_PARTITIONS) {
		ucache_boundary_creation_disabled = true;
		snprintf(
			limit_message,
			sizeof(limit_message),
			"UserCache boundary partition limit (%u) reached; creation of new partitions has been disabled "
			"for this process; existing partitions remain available",
			PHP_UCACHE_MAX_BOUNDARY_PARTITIONS
		);
		php_log_err(limit_message);

		return NULL;
	}

	/* libc allocation on purpose: the cgi boundary is resolved from
	 * sapi_activate() before zend_startup(), and a NULL here must degrade to
	 * an unavailable partition rather than abort. */
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

	entry->partition = php_ucache_partition_create(partition_name);
	if (entry->partition == NULL) {
		free(entry->boundary);
		free(entry);

		return NULL;
	}

	entry->partition->ctx.boundary_identity = entry->boundary;
	entry->partition->ctx.boundary_identity_len = entry->boundary_len;
	entry->partition->ctx.boundary_shared = true;

	entry->next = ucache_boundary_partitions;

	ucache_boundary_partitions = entry;
	ucache_boundary_partition_count++;

	return entry;
}

static void ucache_safe_direct_handlers_init(void)
{
	if (ucache_safe_direct_handlers_initialized) {
		return;
	}

	zend_hash_init(
		&ucache_safe_direct_handler_table,
		8,
		NULL,
		ucache_safe_direct_handlers_dtor,
		true
	);

	ucache_safe_direct_handlers_initialized = true;
}

static void ucache_safe_direct_handlers_destroy(void)
{
	if (!ucache_safe_direct_handlers_initialized) {
		return;
	}

	zend_hash_destroy(&ucache_safe_direct_handler_table);

	ucache_safe_direct_handlers_initialized = false;
}

static void ucache_activate_request_unavailable(php_ucache_reason_t reason)
{
	UC_G(active_partition) = NULL;
	UC_G(active_context_ptr) = NULL;
	UC_G(request_unavailable_reason) = reason;
	UC_G(runtime_resolved) = false;
}

static php_ucache_partition_t *ucache_boundary_partition_get(
		const char *sapi_prefix,
		const char *boundary,
		size_t supplied_boundary_len)
{
	php_ucache_boundary_partition_t *entry;
	zend_ulong boundary_hash;
	size_t boundary_len;
	char *boundary_key;

	boundary_key = ucache_build_boundary_key(boundary, supplied_boundary_len, &boundary_len);
	if (boundary_key == NULL) {
		return NULL;
	}

	boundary_hash = zend_inline_hash_func(boundary_key, boundary_len);

	php_ucache_boundary_partitions_lock();

	entry = ucache_find_boundary_partition(boundary_key, boundary_len, boundary_hash);
	if (entry == NULL) {
		entry = ucache_create_boundary_partition(sapi_prefix, boundary_key, boundary_len, boundary_hash);
		if (entry == NULL) {
			php_ucache_boundary_partitions_unlock();
			free(boundary_key);

			return NULL;
		}
	}

	free(boundary_key);

	if (!php_ucache_partition_startup_storage(entry->partition) &&
		!ucache_boundary_startup_failed_logged
	) {
		php_log_err("UserCache partition startup failed; UserCache will be unavailable");
		ucache_boundary_startup_failed_logged = true;
	}

	php_ucache_boundary_partitions_unlock();

	return entry->partition;
}

static void ucache_boundary_partitions_shutdown(void)
{
	php_ucache_boundary_partition_t *entry, *next;

	php_ucache_boundary_partitions_lock();

	entry = ucache_boundary_partitions;
	while (entry != NULL) {
		next = entry->next;
		free(entry->boundary);
		free(entry);
		entry = next;
	}

	ucache_boundary_partitions = NULL;
	ucache_boundary_partition_count = 0;
	ucache_boundary_creation_disabled = false;
	ucache_boundary_startup_failed_logged = false;

	php_ucache_boundary_partitions_unlock();
}

#ifdef ZTS
static void ucache_globals_ctor(php_ucache_globals *user_cache_globals)
{
	memset(user_cache_globals, 0, sizeof(php_ucache_globals));
}

static void ucache_globals_dtor(php_ucache_globals *user_cache_globals)
{
	php_ucache_release_thread_reader_claims(user_cache_globals);
	php_ucache_release_thread_graph_pin_claims(user_cache_globals);
	php_ucache_free_thread_deferred_entry_lock_releases(user_cache_globals);
}
#endif /* ZTS */

static zend_result ucache_post_deactivate(void)
{
	php_ucache_release_request_shared_graph_refs();
	php_ucache_expunge_expired_at_request_end();

	UC_G(in_request_shutdown) = false;
	UC_G(runtime_resolved) = false;
	UC_G(access_now) = 0;
	UC_G(access_now_touches) = 0;

	return SUCCESS;
}

static void ucache_store_method(INTERNAL_FUNCTION_PARAMETERS, bool add_only)
{
	php_ucache_object *cache;
	zend_long ttl = 0;
	zend_string *key, *storage_key;
	zval *value;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(key)
		Z_PARAM_ZVAL(value)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(ttl)
	ZEND_PARSE_PARAMETERS_END();

	cache = ucache_object_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	storage_key = ucache_validated_storage_key(cache, key, 1);
	if (storage_key == NULL) {
		RETURN_THROWS();
	}

	if (!ucache_validate_api_value(value, 2)) {
		zend_string_release(storage_key);

		RETURN_THROWS();
	}

	if (!ucache_validate_non_negative(ttl, 3)) {
		zend_string_release(storage_key);

		RETURN_THROWS();
	}

	if (!ucache_can_write() ||
		!ucache_store_storage_key_prevalidated(storage_key, value, ttl, add_only)
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

static void ucache_atomic_update_method(INTERNAL_FUNCTION_PARAMETERS, bool decrement)
{
	php_ucache_object *cache;
	php_ucache_atomic_update_result_t result;
	zend_long step = 1, ttl = 0;
	zend_string *key, *storage_key;
	bool updated;

	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_STR(key)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(step)
		Z_PARAM_LONG(ttl)
	ZEND_PARSE_PARAMETERS_END();

	cache = ucache_object_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	storage_key = ucache_validated_storage_key(cache, key, 1);
	if (storage_key == NULL) {
		RETURN_THROWS();
	}

	if (!ucache_validate_non_negative(step, 2)) {
		zend_string_release(storage_key);

		RETURN_THROWS();
	}

	if (!ucache_validate_non_negative(ttl, 3)) {
		zend_string_release(storage_key);

		RETURN_THROWS();
	}

	updated = ucache_atomic_update_api(storage_key, step, ttl, decrement, &result);

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

static void ucache_invoke_remember_callback(
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
		return;
	}

	if (Z_TYPE_P(result) == IS_UNDEF) {
		return;
	}

	/* A by-ref callback returns IS_REFERENCE; unwrap so validation, the
	 * store and the method return value all see the plain value. */
	if (Z_ISREF_P(result)) {
		zend_unwrap_reference(result);
	}

	if (!ucache_validate_remember_value(result)) {
		return;
	}

	if (ucache_can_write()) {
		(void) ucache_store_storage_key_prevalidated(storage_key, result, ttl, false);
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

	if ((uint64_t) size > PHP_UCACHE_SHM_SIZE_MAX) {
		size = (zend_long) PHP_UCACHE_SHM_SIZE_MAX;

		zend_error(E_WARNING, "user_cache.shm_size is limited to slightly under 16384M; clamping");
	}

	if (size != 0 && (size_t) size <= PHP_UCACHE_SHM_SIZE_FLOOR) {
		zend_error(
			E_WARNING,
			"user_cache.shm_size (" ZEND_LONG_FMT ") cannot hold the minimum cache layout (%zu bytes); the cache will be unavailable",
			size,
			PHP_UCACHE_SHM_SIZE_FLOOR
		);
	}

	*p = size;

	return SUCCESS;
}

static ZEND_INI_MH(OnUpdateUserCacheEvictionPolicy)
{
	zend_long *p = (zend_long *) ZEND_INI_GET_ADDR();

	if (zend_string_equals_literal_ci(new_value, "lru")) {
		*p = PHP_UCACHE_EVICTION_POLICY_LRU;
	} else if (zend_string_equals_literal_ci(new_value, "clear")) {
		*p = PHP_UCACHE_EVICTION_POLICY_CLEAR;
	} else if (ZSTR_LEN(new_value) == 0 || zend_string_equals_literal_ci(new_value, "none")) {
		*p = PHP_UCACHE_EVICTION_POLICY_NONE;
	} else {
		zend_error(E_WARNING, "user_cache.eviction_policy must be one of \"lru\", \"clear\" or \"none\"");

		return FAILURE;
	}

	return SUCCESS;
}

static ZEND_INI_MH(OnUpdateUserCacheEntriesHint)
{
	zend_long *p, hint;

	p = (zend_long *) ZEND_INI_GET_ADDR();
	hint = zend_ini_parse_quantity_warn(new_value, entry->name);

	if (hint < 0) {
		zend_error(E_WARNING, "user_cache.entries_hint must be greater than or equal to 0, " ZEND_LONG_FMT " given", hint);

		return FAILURE;
	}

	if (hint > PHP_UCACHE_ENTRIES_HINT_MAX) {
		hint = PHP_UCACHE_ENTRIES_HINT_MAX;

		zend_error(E_WARNING, "user_cache.entries_hint is limited to %d; clamping", PHP_UCACHE_ENTRIES_HINT_MAX);
	}

	*p = hint;

	return SUCCESS;
}

const php_ucache_safe_direct_handlers_t *php_ucache_safe_direct_find_handlers(
		zend_class_entry *ce,
		zend_class_entry **base_ce_ptr)
{
	const php_ucache_safe_direct_handlers_t *handlers;

	if (!ucache_safe_direct_handlers_initialized) {
		return NULL;
	}

	while (ce != NULL) {
		handlers = zend_hash_index_find_ptr(
			&ucache_safe_direct_handler_table,
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

void php_ucache_boundary_partitions_lock(void)
{
#ifdef ZTS
	if (ucache_boundary_partitions_mutex != NULL) {
		tsrm_mutex_lock(ucache_boundary_partitions_mutex);
	}
#endif
}

void php_ucache_boundary_partitions_unlock(void)
{
#ifdef ZTS
	if (ucache_boundary_partitions_mutex != NULL) {
		tsrm_mutex_unlock(ucache_boundary_partitions_mutex);
	}
#endif
}

uint64_t php_ucache_cached_pid(void)
{
#ifndef ZEND_WIN32
	if (UNEXPECTED(ucache_self_pid_uncached)) {
		return php_ucache_current_pid();
	}
#endif

	if (UNEXPECTED(ucache_self_pid == 0)) {
		ucache_self_pid = php_ucache_current_pid();
	}

	return ucache_self_pid;
}

ZEND_API void php_ucache_safe_direct_register_class(
		zend_class_entry *ce,
		const php_ucache_safe_direct_handlers_t *handlers)
{
	php_ucache_safe_direct_handlers_t handlers_copy;

	if (ce == NULL ||
		handlers == NULL ||
		handlers->copy == NULL ||
		handlers->state_serialize == NULL ||
		handlers->state_unserialize == NULL
	) {
		return;
	}

	ucache_safe_direct_handlers_init();

	handlers_copy = *handlers;

	zend_hash_index_update_mem(
		&ucache_safe_direct_handler_table,
		(zend_ulong) (uintptr_t) ce,
		&handlers_copy,
		sizeof(handlers_copy)
	);
}

php_ucache_safe_direct_state_copy_func_t php_ucache_safe_direct_state_copy_func(
		zend_class_entry *ce,
		zend_class_entry **base_ce_ptr)
{
	const php_ucache_safe_direct_handlers_t *handlers =
		php_ucache_safe_direct_find_handlers(ce, base_ce_ptr)
	;

	return handlers != NULL ? handlers->copy : NULL;
}

PHP_UCACHE_DEFINE_SAFE_DIRECT_HANDLER_GETTER(state_has_unstorable)

PHP_UCACHE_DEFINE_SAFE_DIRECT_HANDLER_GETTER(state_serialize)

PHP_UCACHE_DEFINE_SAFE_DIRECT_HANDLER_GETTER(state_unserialize)

bool php_ucache_safe_direct_prefers_request_local_prototype(zend_class_entry *ce)
{
	const php_ucache_safe_direct_handlers_t *handlers =
		php_ucache_safe_direct_find_handlers(ce, NULL)
	;

	return handlers != NULL && handlers->prefer_request_local_prototype;
}

ZEND_API php_ucache_partition_t *php_ucache_partition_create(const char *name)
{
	php_ucache_partition_t *partition;

	partition = pecalloc(1, sizeof(php_ucache_partition_t), true);
	ucache_init_partition_context(partition, name);

	partition->next = php_ucache_partitions;
	php_ucache_partitions = partition;

	return partition;
}

ZEND_API bool php_ucache_partition_startup_storage(php_ucache_partition_t *partition)
{
	php_ucache_partition_t *prev_partition;
	bool result;

	if (partition == NULL) {
		return true;
	}

	prev_partition = UC_G(active_partition);
	UC_G(active_partition) = partition;

	result = ucache_startup_storage_for_context(&partition->ctx);

	UC_G(active_partition) = prev_partition;

	return result;
}

ZEND_API bool php_ucache_startup_default_context_storage(void)
{
	return ucache_startup_storage_for_context(&php_ucache_ctx_state);
}

ZEND_API void php_ucache_partition_activate(php_ucache_partition_t *partition)
{
	UC_G(active_partition) = partition;
	UC_G(active_context_ptr) = NULL;
	UC_G(request_unavailable_reason) = PHP_UCACHE_REASON_NONE;
	UC_G(runtime_resolved) = false;
}

ZEND_API void php_ucache_activate_boundary_partition_by_id(
		const char *sapi_prefix,
		const char *boundary,
		size_t boundary_len,
		php_ucache_reason_t failure_reason)
{
	php_ucache_partition_t *partition = NULL;

	if (boundary != NULL && boundary_len != 0) {
		partition = ucache_boundary_partition_get(sapi_prefix, boundary, boundary_len);
	}

	if (partition == NULL) {
		ucache_activate_request_unavailable(failure_reason);

		return;
	}

	php_ucache_partition_activate(partition);
}

ZEND_API void php_ucache_activate_boundary_partition(
		const char *sapi_prefix,
		const char *(*get_env)(const char *name),
		php_ucache_reason_t failure_reason)
{
	const char *boundary;

	boundary = get_env("DOCUMENT_ROOT");
	if (boundary == NULL || boundary[0] == '\0') {
		boundary = get_env("SERVER_NAME");
	}

	php_ucache_activate_boundary_partition_by_id(
		sapi_prefix,
		boundary,
		boundary != NULL ? strlen(boundary) : 0,
		failure_reason
	);
}

ZEND_API void php_ucache_opt_in(void)
{
	php_ucache_runtime_opted_in = true;
}

#ifdef ZTS
size_t php_ucache_globals_size(void)
{
	return sizeof(php_ucache_globals);
}

void php_ucache_globals_startup(void)
{
	user_cache_globals_id = ts_allocate_fast_id(
		&user_cache_globals_id,
		&user_cache_globals_offset,
		sizeof(php_ucache_globals),
		(ts_allocate_ctor) ucache_globals_ctor,
		(ts_allocate_dtor) ucache_globals_dtor
	);
}
#endif /* ZTS */

ZEND_METHOD(UserCache_CacheStatus, __construct)
{
}

ZEND_METHOD(UserCache_CacheStatus, getAvailability)
{
	php_ucache_status_object *status;
	zend_object *availability;

	ZEND_PARSE_PARAMETERS_NONE();

	status = ucache_status_from_this(ZEND_THIS);
	if (status == NULL) {
		RETURN_THROWS();
	}

	availability = ucache_availability_enum_case(status->availability_reason);

	RETURN_OBJ_COPY(availability);
}

PHP_UCACHE_DEFINE_STATUS_LONG_GETTER(getConfiguredMemory, configured_memory)

PHP_UCACHE_DEFINE_STATUS_LONG_GETTER(getSharedMemorySize, shared_memory_size)

PHP_UCACHE_DEFINE_STATUS_LONG_GETTER(getUsedMemory, used_memory)

PHP_UCACHE_DEFINE_STATUS_LONG_GETTER(getFreeMemory, free_memory)

PHP_UCACHE_DEFINE_STATUS_LONG_GETTER(getWastedMemory, wasted_memory)

PHP_UCACHE_DEFINE_STATUS_LONG_GETTER(getEntryCount, entry_count)

PHP_UCACHE_DEFINE_STATUS_LONG_GETTER(getEntryCapacity, entry_capacity)

PHP_UCACHE_DEFINE_STATUS_LONG_GETTER(getTombstoneCount, tombstone_count)

PHP_UCACHE_DEFINE_STATUS_LONG_GETTER(getExpungeCount, expunge_count)

PHP_UCACHE_DEFINE_STATUS_LONG_GETTER(getEvictionCount, eviction_count)

PHP_UCACHE_DEFINE_STATUS_LONG_GETTER(getStoreFailureCount, store_failure_count)

PHP_UCACHE_DEFINE_STATUS_LONG_GETTER(getGraphPinSlotsInUse, graph_pin_slots_in_use)

PHP_UCACHE_DEFINE_STATUS_LONG_GETTER(getGraphPinnedReferences, graph_pinned_references)

PHP_UCACHE_DEFINE_STATUS_LONG_GETTER(getDeadPinOwnersReclaimed, dead_pin_owners_reclaimed)

PHP_UCACHE_DEFINE_STATUS_LONG_GETTER(getDeadPinsStripped, dead_pins_stripped)
PHP_UCACHE_DEFINE_STATUS_LONG_GETTER(getHitCount, hit_count)
PHP_UCACHE_DEFINE_STATUS_LONG_GETTER(getMissCount, miss_count)
PHP_UCACHE_DEFINE_STATUS_LONG_GETTER(getInternedKeyCount, interned_key_count)

ZEND_METHOD(UserCache_CachePoolStatus, __construct)
{
}

ZEND_METHOD(UserCache_CachePoolStatus, getPoolName)
{
	php_ucache_pool_status_object *status;

	ZEND_PARSE_PARAMETERS_NONE();

	status = ucache_pool_status_from_this(ZEND_THIS);
	if (status == NULL) {
		RETURN_THROWS();
	}

	RETURN_STR_COPY(status->scope);
}

ZEND_METHOD(UserCache_CachePoolStatus, getEntryCount)
{
	php_ucache_pool_status_object *status;

	ZEND_PARSE_PARAMETERS_NONE();

	status = ucache_pool_status_from_this(ZEND_THIS);
	if (status == NULL) {
		RETURN_THROWS();
	}

	RETURN_LONG(status->entry_count);
}

ZEND_METHOD(UserCache_CachePoolStatus, getEntryKeys)
{
	php_ucache_pool_status_object *status;

	ZEND_PARSE_PARAMETERS_NONE();

	status = ucache_pool_status_from_this(ZEND_THIS);
	if (status == NULL) {
		RETURN_THROWS();
	}

	RETURN_COPY(&status->entry_keys);
}

ZEND_METHOD(UserCache_CachePoolStatus, getUsedMemory)
{
	php_ucache_pool_status_object *status;

	ZEND_PARSE_PARAMETERS_NONE();

	status = ucache_pool_status_from_this(ZEND_THIS);
	if (status == NULL) {
		RETURN_THROWS();
	}

	RETURN_LONG(status->used_memory);
}

ZEND_METHOD(UserCache_CachePoolStatus, getHitCount)
{
	php_ucache_pool_status_object *status;

	ZEND_PARSE_PARAMETERS_NONE();

	status = ucache_pool_status_from_this(ZEND_THIS);
	if (status == NULL) {
		RETURN_THROWS();
	}

	RETURN_LONG(status->hit_count);
}

ZEND_METHOD(UserCache_CachePoolStatus, getMissCount)
{
	php_ucache_pool_status_object *status;

	ZEND_PARSE_PARAMETERS_NONE();

	status = ucache_pool_status_from_this(ZEND_THIS);
	if (status == NULL) {
		RETURN_THROWS();
	}

	RETURN_LONG(status->miss_count);
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

	if (!ucache_validate_pool_name(pool, 1)) {
		RETURN_THROWS();
	}

	RETURN_BOOL(UC_G(pool_table) != NULL &&
		zend_hash_exists(UC_G(pool_table), pool)
	);
}

ZEND_METHOD(UserCache_Cache, getPool)
{
	zend_string *pool;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(pool)
	ZEND_PARSE_PARAMETERS_END();

	if (!ucache_validate_pool_name(pool, 1)) {
		RETURN_THROWS();
	}

	if (UC_G(in_request_shutdown)) {
		RETURN_OBJ(ucache_create_pool_object(pool));
	}

	RETURN_OBJ(ucache_get_or_create_pool(pool));
}

ZEND_METHOD(UserCache_Cache, getPools)
{
	ZEND_PARSE_PARAMETERS_NONE();

	if (UC_G(pool_table) == NULL) {
		RETURN_EMPTY_ARRAY();
	}

	RETURN_ARR(zend_array_dup(UC_G(pool_table)));
}

ZEND_METHOD(UserCache_Cache, deletePool)
{
	php_ucache_object *cache;
	zend_string *pool, *scope_prefix;
	zval *zv;
	bool cleared = true;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(pool)
	ZEND_PARSE_PARAMETERS_END();

	if (!ucache_validate_pool_name(pool, 1)) {
		RETURN_THROWS();
	}

	if (ucache_can_write()) {
		scope_prefix = ucache_build_scope_prefix(pool);
		cleared = ucache_clear_scope_prevalidated(scope_prefix, pool);

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

	if (UC_G(pool_table) != NULL) {
		/* Counts of the deleted pool go with it, never into a fresh slot. */
		zv = zend_hash_find(UC_G(pool_table), pool);
		if (zv != NULL) {
			cache = ucache_object_from_obj(Z_OBJ_P(zv));
			cache->hit_count = 0;
			cache->miss_count = 0;
		}

		zend_hash_del(UC_G(pool_table), pool);
	}

	RETURN_TRUE;
}

ZEND_METHOD(UserCache_Cache, getStatus)
{
	ZEND_PARSE_PARAMETERS_NONE();

	ucache_return_status(return_value);

	if (EG(exception)) {
		RETURN_THROWS();
	}
}

ZEND_METHOD(UserCache_Cache, store)
{
	ucache_store_method(INTERNAL_FUNCTION_PARAM_PASSTHRU, false);
}

ZEND_METHOD(UserCache_Cache, add)
{
	ucache_store_method(INTERNAL_FUNCTION_PARAM_PASSTHRU, true);
}

ZEND_METHOD(UserCache_Cache, storeMultiple)
{
	php_ucache_object *cache;
	zend_long ttl = 0;
	HashTable *values;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ARRAY_HT(values)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(ttl)
	ZEND_PARSE_PARAMETERS_END();

	cache = ucache_object_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	if (!ucache_validate_store_array(values, 1)) {
		RETURN_THROWS();
	}

	if (!ucache_validate_non_negative(ttl, 2)) {
		RETURN_THROWS();
	}

	if (!ucache_instance_store_multiple(cache, values, ttl)) {
		if (EG(exception)) {
			RETURN_THROWS();
		}

		RETURN_FALSE;
	}

	RETURN_TRUE;
}

ZEND_METHOD(UserCache_Cache, increment)
{
	ucache_atomic_update_method(INTERNAL_FUNCTION_PARAM_PASSTHRU, false);
}

ZEND_METHOD(UserCache_Cache, decrement)
{
	ucache_atomic_update_method(INTERNAL_FUNCTION_PARAM_PASSTHRU, true);
}

ZEND_METHOD(UserCache_Cache, fetch)
{
	php_ucache_object *cache;
	zend_string *key, *storage_key;
	zval *default_value = NULL, default_null;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(key)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(default_value)
	ZEND_PARSE_PARAMETERS_END();

	cache = ucache_object_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	storage_key = ucache_validated_storage_key(cache, key, 1);
	if (storage_key == NULL) {
		RETURN_THROWS();
	}

	if (default_value == NULL) {
		ZVAL_NULL(&default_null);
		default_value = &default_null;
	}

	ucache_pool_note_lookup(cache, ucache_fetch_api(storage_key, default_value, return_value));

	zend_string_release(storage_key);
}

ZEND_METHOD(UserCache_Cache, fetchMultiple)
{
	php_ucache_object *cache;
	zval *default_value = NULL, default_null;
	HashTable *keys;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ARRAY_HT(keys)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(default_value)
	ZEND_PARSE_PARAMETERS_END();

	cache = ucache_object_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	if (default_value == NULL) {
		ZVAL_NULL(&default_null);
		default_value = &default_null;
	}

	if (ucache_fetch_multiple_api(cache, keys, default_value, return_value) == FAILURE) {
		RETURN_THROWS();
	}
}

ZEND_METHOD(UserCache_Cache, has)
{
	php_ucache_object *cache;
	zend_string *key, *storage_key;
	bool exists;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(key)
	ZEND_PARSE_PARAMETERS_END();

	cache = ucache_object_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	storage_key = ucache_validated_storage_key(cache, key, 1);
	if (storage_key == NULL) {
		RETURN_THROWS();
	}

	exists = ucache_exists_api(storage_key);

	zend_string_release(storage_key);

	RETURN_BOOL(exists);
}

ZEND_METHOD(UserCache_Cache, delete)
{
	php_ucache_object *cache;
	zend_string *key, *storage_key;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(key)
	ZEND_PARSE_PARAMETERS_END();

	cache = ucache_object_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	storage_key = ucache_validated_storage_key(cache, key, 1);
	if (storage_key == NULL) {
		RETURN_THROWS();
	}

	if (!ucache_can_write()) {
		zend_string_release(storage_key);

		if (EG(exception)) {
			RETURN_THROWS();
		}

		RETURN_TRUE;
	}

	if (!ucache_delete_storage_key_prevalidated(storage_key)) {
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
	php_ucache_object *cache;
	HashTable *keys;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(keys)
	ZEND_PARSE_PARAMETERS_END();

	cache = ucache_object_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	if (!ucache_instance_delete_multiple(cache, keys)) {
		if (EG(exception)) {
			RETURN_THROWS();
		}

		RETURN_FALSE;
	}

	RETURN_TRUE;
}

ZEND_METHOD(UserCache_Cache, clear)
{
	php_ucache_object *cache;

	ZEND_PARSE_PARAMETERS_NONE();

	cache = ucache_object_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	if (!ucache_can_write()) {
		if (EG(exception)) {
			RETURN_THROWS();
		}

		RETURN_TRUE;
	}

	if (!ucache_clear_scope_prevalidated(cache->scope_prefix, NULL)) {
		if (EG(exception)) {
			RETURN_THROWS();
		}

		RETURN_FALSE;
	}

	RETURN_TRUE;
}

ZEND_METHOD(UserCache_Cache, lock)
{
	php_ucache_object *cache;
	zend_long lease = 0;
	zend_string *key, *storage_key;
	bool locked;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(key)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(lease)
	ZEND_PARSE_PARAMETERS_END();

	cache = ucache_object_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	storage_key = ucache_validated_storage_key(cache, key, 1);
	if (storage_key == NULL) {
		RETURN_THROWS();
	}

	if (!ucache_validate_non_negative(lease, 2)) {
		zend_string_release(storage_key);

		RETURN_THROWS();
	}

	locked = ucache_lock_api(storage_key, lease);

	zend_string_release(storage_key);

	RETURN_BOOL(locked);
}

ZEND_METHOD(UserCache_Cache, unlock)
{
	php_ucache_object *cache;
	zend_string *key, *storage_key;
	bool unlocked;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(key)
	ZEND_PARSE_PARAMETERS_END();

	cache = ucache_object_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	storage_key = ucache_validated_storage_key(cache, key, 1);
	if (storage_key == NULL) {
		RETURN_THROWS();
	}

	unlocked = ucache_unlock_api(storage_key);

	zend_string_release(storage_key);

	RETURN_BOOL(unlocked);
}

ZEND_METHOD(UserCache_Cache, remember)
{
	php_ucache_object *cache;
	zend_long ttl = 0;
	zend_string *key, *storage_key;
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;
	zval result;
	bool found = false, preheld = false, locked = false;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(key)
		Z_PARAM_FUNC(fci, fcc)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(ttl)
	ZEND_PARSE_PARAMETERS_END();

	cache = ucache_object_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	storage_key = ucache_validated_storage_key(cache, key, 1);
	if (storage_key == NULL) {
		RETURN_THROWS();
	}

	if (!ucache_validate_non_negative(ttl, 3)) {
		zend_string_release(storage_key);

		RETURN_THROWS();
	}

	found = ucache_fetch_if_present_api(storage_key, return_value);
	ucache_pool_note_lookup(cache, found);
	if (found) {
		zend_string_release(storage_key);

		return;
	}

	if (EG(exception)) {
		zend_string_release(storage_key);

		RETURN_THROWS();
	}

	preheld = ucache_can_write() && php_ucache_request_owns_entry_lock(storage_key);
	locked = !preheld && ucache_lock_api(storage_key, 0);

	found = ucache_fetch_if_present_api(storage_key, return_value);
	if (found) {
		if (locked) {
			(void) ucache_unlock_api(storage_key);
		}

		zend_string_release(storage_key);

		return;
	}

	if (EG(exception)) {
		if (locked) {
			(void) ucache_unlock_api(storage_key);
		}

		zend_string_release(storage_key);

		RETURN_THROWS();
	}

	zend_try {
		ucache_invoke_remember_callback(key, storage_key, &fci, &fcc, ttl, &result);
	} zend_catch {
		if (locked) {
			(void) ucache_unlock_api(storage_key);
		}

		zend_string_release(storage_key);

		zend_bailout();
	} zend_end_try();

	if (EG(exception)) {
		zval_ptr_dtor(&result);
	}

	if (locked) {
		(void) ucache_unlock_api(storage_key);
	}

	zend_string_release(storage_key);

	if (EG(exception)) {
		RETURN_THROWS();
	}

	if (Z_TYPE(result) == IS_UNDEF) {
		RETURN_NULL();
	}

	RETURN_COPY_VALUE(&result);
}

ZEND_METHOD(UserCache_Cache, getPoolStatus)
{
	php_ucache_object *cache;

	ZEND_PARSE_PARAMETERS_NONE();

	cache = ucache_object_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	ucache_return_pool_status(cache, return_value);

	if (EG(exception)) {
		RETURN_THROWS();
	}
}

PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY("user_cache.enable",                 "1",    PHP_INI_SYSTEM, OnUpdateBool,                    enable,          php_ucache_globals, user_cache_globals)
	STD_PHP_INI_ENTRY("user_cache.enable_cli",             "0",    PHP_INI_SYSTEM, OnUpdateBool,                    enable_cli,      php_ucache_globals, user_cache_globals)
	STD_PHP_INI_ENTRY("user_cache.shm_size",               "16M",  PHP_INI_SYSTEM, OnUpdateUserCacheShmSize,        shm_size,        php_ucache_globals, user_cache_globals)
	STD_PHP_INI_ENTRY("user_cache.entries_hint",           "0",    PHP_INI_SYSTEM, OnUpdateUserCacheEntriesHint,    entries_hint,    php_ucache_globals, user_cache_globals)
	STD_PHP_INI_ENTRY("user_cache.eviction_policy",        "lru",  PHP_INI_SYSTEM, OnUpdateUserCacheEvictionPolicy, eviction_policy, php_ucache_globals, user_cache_globals)
	STD_PHP_INI_ENTRY("user_cache.lockfile_path",          "/tmp", PHP_INI_SYSTEM, OnUpdateString,                  lockfile_path,   php_ucache_globals, user_cache_globals)
	STD_PHP_INI_ENTRY("user_cache.preferred_memory_model", "",     PHP_INI_SYSTEM, OnUpdateStringUnempty,           memory_model,    php_ucache_globals, user_cache_globals)
PHP_INI_END()

static PHP_MINIT_FUNCTION(cache)
{
	php_ucache_ctx_t *prev_ctx;

	REGISTER_INI_ENTRIES();

#ifdef ZTS
	if (ucache_boundary_partitions_mutex == NULL) {
		ucache_boundary_partitions_mutex = tsrm_mutex_alloc();
	}
#endif

#ifndef ZEND_WIN32
	ucache_self_pid = php_ucache_current_pid();
	if (!ucache_pid_atfork_registered &&
		!ucache_self_pid_uncached
	) {
		if (pthread_atfork(NULL, NULL, ucache_pid_atfork_child) == 0) {
			ucache_pid_atfork_registered = true;
		} else {
			ucache_self_pid = 0;
			ucache_self_pid_uncached = true;
		}
	}
#endif /* !ZEND_WIN32 */

	php_ucache_runtime_opted_in = false;

	ucache_register_classes();

	ucache_safe_direct_handlers_init();
	php_ucache_optimistic_fork_setup();

	prev_ctx = php_ucache_activate_context(php_ucache_owning_context());

	php_ucache_reset_storage();

	php_ucache_restore_context(prev_ctx);

	return SUCCESS;
}

static PHP_MSHUTDOWN_FUNCTION(cache)
{
	php_ucache_ctx_t *prev_ctx;

	ucache_partitions_shutdown();
	ucache_boundary_partitions_shutdown();

#ifdef ZTS
	if (ucache_boundary_partitions_mutex != NULL) {
		tsrm_mutex_free(ucache_boundary_partitions_mutex);
		ucache_boundary_partitions_mutex = NULL;
	}
#endif

	UC_G(active_partition) = NULL;

	prev_ctx = php_ucache_activate_context(php_ucache_owning_context());

	php_ucache_shutdown_storage();
	php_ucache_reset_runtime();
	php_ucache_restore_context(prev_ctx);

	php_ucache_runtime_opted_in = false;

	ucache_safe_direct_handlers_destroy();
	ucache_reset_class_entries();

	UNREGISTER_INI_ENTRIES();

	return SUCCESS;
}

static PHP_RSHUTDOWN_FUNCTION(cache)
{
	UC_G(in_request_shutdown) = true;

	php_ucache_unlock_if_held();

	UC_G(request_unavailable_reason) = PHP_UCACHE_REASON_NONE;
	UC_G(runtime_resolved) = false;

	ucache_flush_pool_stats_all();
	ucache_release_pools();
	php_ucache_lookup_cache_clear();

	php_ucache_release_request_entry_locks();
	php_ucache_release_request_local_slots();

	if (UC_G(request_local_slot_may_cycle)) {
		UC_G(request_local_slot_may_cycle) = false;

		if (!CG(unclean_shutdown)) {
			gc_collect_cycles();
		}
	}

	php_ucache_decode_resolve_cache_release();
	php_ucache_decode_shape_prototype_cache_release();
	php_ucache_decode_maps_teardown();

	return SUCCESS;
}

static PHP_MINFO_FUNCTION(cache)
{
	const php_ucache_storage_t *storage = &php_ucache_active_context()->storage;

	php_info_print_table_start();
	php_info_print_table_row(2, "UserCache support", "enabled");
	php_info_print_table_row(
		2,
		"Active memory model",
		storage->initialized && storage->handler_name != NULL ? storage->handler_name : "none"
	);
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
	ucache_post_deactivate,
	STANDARD_MODULE_PROPERTIES_EX
};
