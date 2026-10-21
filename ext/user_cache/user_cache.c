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

#include "user_cache_internal.h"
#include "user_cache_arginfo.h"

#include "Zend/zend_closures.h"
#include "Zend/zend_fibers.h"

#include "ext/standard/info.h"

#define UCACHE_KEY_RECORD_LIMIT_MIN		16384U
#define UCACHE_KEY_RECORD_TABLE_BYTES	(sizeof(Bucket) + 2 * sizeof(uint32_t))

#define UCACHE_DEFINE_SAFE_DIRECT_HANDLER_GETTER(member) \
	php_ucache_safe_direct_##member##_func_t ucache_safe_direct_##member##_func( \
			zend_class_entry *ce) \
	{ \
		const php_ucache_safe_direct_handlers *handlers = \
			ucache_safe_direct_find_handlers(ce, NULL) \
		; \
		return handlers != NULL ? handlers->member : NULL; \
	}

#define UCACHE_METHOD_BODY(name) \
	static void ZEND_FASTCALL ucache_method_##name(INTERNAL_FUNCTION_PARAMETERS)

#define UCACHE_METHOD_ENTRY(name) \
	ZEND_METHOD(UserCache_Cache, name) \
	{ \
		bool tracked = UC_G(persistent_exec); \
		if (UNEXPECTED(tracked)) { \
			UC_G(op_depth)++; \
		} \
		ucache_method_##name(INTERNAL_FUNCTION_PARAM_PASSTHRU); \
		if (UNEXPECTED(tracked) && UC_G(op_depth) != 0) { \
			UC_G(op_depth)--; \
		} \
	}

#define UCACHE_REQ_HANDLER(key, method) \
	{ key, sizeof(key) - 1, ucache_method_##method }

typedef struct _ucache_held_records {
	struct _ucache_held_records *prev;
	struct _ucache_held_records *next;
	ucache_key_record **records;
	uint32_t count;
} ucache_held_records;

typedef struct {
	zval val;
	ucache_key_record *record;
	ucache_prepared_val prepared;
	ucache_store_result store_result;
} ucache_bulk_store_item;

static zend_never_inline void ucache_key_records_trim(void);
static zend_never_inline ucache_obj *ucache_obj_uninitialized(zval *this_ptr);
static zend_result ucache_post_deactivate(void);
static ZEND_INI_MH(OnUpdateUserCacheShmSize);
static ZEND_INI_MH(OnUpdateUserCacheEvictionPolicy);
static ZEND_INI_MH(OnUpdateUserCacheLockfilePath);
static ZEND_INI_MH(OnUpdateUserCachePreferredMemoryModel);
static ZEND_INI_MH(OnUpdateUserCacheEntriesHint);
static ZEND_INI_MH(OnUpdateUserCacheLockLeaseMax);
static PHP_MINIT_FUNCTION(user_cache);
static PHP_MSHUTDOWN_FUNCTION(user_cache);
static PHP_RINIT_FUNCTION(user_cache);
static PHP_RSHUTDOWN_FUNCTION(user_cache);
static PHP_MINFO_FUNCTION(user_cache);

#ifdef ZTS
int user_cache_globals_id;
#endif
static HashTable ucache_safe_direct_handler_table;
static bool ucache_safe_direct_handlers_initialized = false;
static zend_class_entry *ucache_ce;
static zend_class_entry *ucache_status_ce;
static zend_class_entry *ucache_pool_status_ce;
static zend_object_handlers ucache_obj_handlers;
#ifndef ZEND_WIN32
static bool ucache_self_pid_uncached = false;
static bool ucache_pid_atfork_registered = false;
#endif
#if defined(ZTS) && !defined(ZEND_WIN32)
static bool ucache_zts_atfork_registered = false;
#endif

PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY("user_cache.enable",                 "1",    PHP_INI_SYSTEM, OnUpdateBool,                          enable,          ucache_globals, user_cache_globals)
	STD_PHP_INI_ENTRY("user_cache.enable_cli",             "0",    PHP_INI_SYSTEM, OnUpdateBool,                          enable_cli,      ucache_globals, user_cache_globals)
	STD_PHP_INI_ENTRY("user_cache.shm_size",               "16M",  PHP_INI_SYSTEM, OnUpdateUserCacheShmSize,              shm_size,        ucache_globals, user_cache_globals)
	STD_PHP_INI_ENTRY("user_cache.entries_hint",           "0",    PHP_INI_SYSTEM, OnUpdateUserCacheEntriesHint,          entries_hint,    ucache_globals, user_cache_globals)
	STD_PHP_INI_ENTRY("user_cache.eviction_policy",        "lru",  PHP_INI_SYSTEM, OnUpdateUserCacheEvictionPolicy,       eviction_policy, ucache_globals, user_cache_globals)
	STD_PHP_INI_ENTRY("user_cache.lockfile_path",          "/tmp", PHP_INI_SYSTEM, OnUpdateUserCacheLockfilePath,         lockfile_path,   ucache_globals, user_cache_globals)
	STD_PHP_INI_ENTRY("user_cache.preferred_memory_model", "",     PHP_INI_SYSTEM, OnUpdateUserCachePreferredMemoryModel, mem_model,    ucache_globals, user_cache_globals)
	STD_PHP_INI_ENTRY("user_cache.lock_lease_max",         "3600", PHP_INI_SYSTEM, OnUpdateUserCacheLockLeaseMax,         lock_lease_max,  ucache_globals, user_cache_globals)
PHP_INI_END()

#ifndef ZTS
ucache_globals user_cache_globals;
#else
size_t user_cache_globals_offset;
#endif
uint64_t ucache_self_pid = 0;

ucache_ctx ucache_ctx_state = {
	.storage = { .lock_file = -1 },
	.lock_name = "php_user_cache_lock",
};

bool ucache_runtime_opted_in = false;
zend_class_entry *ucache_availability_ce;
zend_object_handlers ucache_status_obj_handlers;
zend_object_handlers ucache_pool_status_obj_handlers;
php_ucache_mode ucache_registered_mode = PHP_UCACHE_MODE_REQ;
atomic_bool ucache_registration_closed = false;

zend_module_entry user_cache_module_entry = {
	STANDARD_MODULE_HEADER,
	"user_cache",
	NULL,
	PHP_MINIT(user_cache),
	PHP_MSHUTDOWN(user_cache),
	PHP_RINIT(user_cache),
	PHP_RSHUTDOWN(user_cache),
	PHP_MINFO(user_cache),
	PHP_VERSION,
	NO_MODULE_GLOBALS,
	ucache_post_deactivate,
	STANDARD_MODULE_PROPERTIES_EX
};

UCACHE_DEFINE_OBJ_FROM_STD(ucache_obj, obj)

static zend_always_inline ucache_obj *ucache_obj_from_this(zval *this_ptr)
{
	ucache_obj *cache = ucache_obj_from_obj(Z_OBJ_P(this_ptr));

	ZEND_ASSERT(cache->ctx == ucache_active_ctx());

	if (UNEXPECTED(cache->scope_prefix == NULL)) {
		return ucache_obj_uninitialized(this_ptr);
	}

	if (UNEXPECTED(UC_G(key_records_trim_due))) {
		ucache_key_records_trim();

		if (UNEXPECTED(EG(exception))) {
			return NULL;
		}
	}

	return cache;
}

static zend_always_inline bool ucache_runtime_is_resolved(void)
{
	return UC_G(runtime_resolved) &&
		UC_G(runtime_resolved_ctx) == ucache_active_ctx() &&
		UC_G(runtime_resolved_enabled) == UC_G(enable)
	;
}

static zend_always_inline void ucache_ensure_ready(void)
{
	if (ucache_runtime_is_resolved()) {
		return;
	}

	ucache_ensure_ready_impl();
}

static zend_always_inline uint32_t ucache_key_record_limit(void)
{
	const ucache_storage *storage = &ucache_active_ctx()->storage;

	if (!storage->layout_memo_valid) {
		return UCACHE_KEY_RECORD_LIMIT_MIN;
	}

	return MAX(UCACHE_KEY_RECORD_LIMIT_MIN, storage->capacity_memo);
}

static zend_always_inline size_t ucache_key_record_footprint(const ucache_key_record *record)
{
	size_t storage_key_bytes = _ZSTR_STRUCT_SIZE(ZSTR_LEN(record->storage_key));
	size_t lookup_key_bytes_max = storage_key_bytes;

	return sizeof(*record) + UCACHE_KEY_RECORD_TABLE_BYTES + storage_key_bytes + lookup_key_bytes_max;
}

static zend_always_inline bool ucache_key_records_within(uint32_t limit, size_t budget)
{
	return UC_G(key_record_count) < limit && UC_G(key_record_bytes) < budget;
}

static zend_always_inline ucache_obj *ucache_frame_pool(const zend_execute_data *ex)
{
	if (ex->func == NULL || ex->func->common.scope != ucache_ce || Z_TYPE(ex->This) != IS_OBJECT) {
		return NULL;
	}

	return ucache_obj_from_obj(Z_OBJ(ex->This));
}

static zend_always_inline uint64_t ucache_cur_pid(void)
{
#ifdef ZEND_WIN32
	return (uint64_t) GetCurrentProcessId();
#else
	return (uint64_t) getpid();
#endif
}

static zend_always_inline size_t ucache_key_len_max(const ucache_obj *cache)
{
	return cache->key_len_max;
}

static zend_always_inline bool ucache_validate_root_val(zval *val)
{
	ZVAL_DEREF(val);

	if (Z_TYPE_P(val) == IS_RESOURCE) {
		ucache_throw_unstorable_res();

		return false;
	}

	if (Z_TYPE_P(val) == IS_OBJECT && Z_OBJCE_P(val) == zend_ce_closure) {
		ucache_throw_unstorable_obj(Z_OBJCE_P(val));

		return false;
	}

	return true;
}

static zend_always_inline zend_long ucache_clamp_lock_lease(zend_long lease)
{
	zend_long lease_max = UC_G(lock_lease_max);

	return lease_max > 0 && lease > lease_max
		? lease_max
		: lease
	;
}

#ifndef ZEND_WIN32
static void ucache_pid_atfork_child(void)
{
	ucache_self_pid = 0;

#ifdef UCACHE_HAVE_BOUNDARY_SHM
	ucache_shared_boundary_segs_after_fork();
#endif
}
#endif

#if defined(ZTS) && !defined(ZEND_WIN32)
static void ucache_zts_atfork_prepare(void)
{
	ucache_boundary_partitions_lock();
	ucache_lock_storage_startup_before_fork();
}

static void ucache_zts_atfork_release(void)
{
	ucache_unlock_storage_startup_after_fork();
	ucache_boundary_partitions_unlock();
}

static void ucache_zts_atfork_child(void)
{
	ucache_zts_atfork_release();

	ucache_reinit_storage_locks_after_fork();
}
#endif

static bool ucache_user_key_is_valid(zend_string *key)
{
	return ZSTR_LEN(key) != 0 &&
		memchr(
			ZSTR_VAL(key),
			UCACHE_KEY_DELIM_CHAR,
			ZSTR_LEN(key)
		) == NULL
	;
}

static bool ucache_validate_delim_free(zend_string *str, const char *empty_err)
{
	if (ZSTR_LEN(str) == 0) {
		zend_argument_value_error(1, "%s", empty_err);

		return false;
	}

	if (memchr(ZSTR_VAL(str), UCACHE_KEY_DELIM_CHAR, ZSTR_LEN(str)) != NULL) {
		zend_argument_value_error(1, "must not contain the user-cache key delimiter " UCACHE_KEY_DELIM_NAME);

		return false;
	}

	return true;
}

static bool ucache_validate_pool_name(zend_string *pool)
{
	if (!ucache_validate_delim_free(pool, "must not be empty")) {
		return false;
	}

	if (ZSTR_LEN(pool) > UCACHE_POOL_NAME_MAX) {
		zend_argument_value_error(1, "must not be longer than %d bytes", (int) UCACHE_POOL_NAME_MAX);

		return false;
	}

	return true;
}

static bool ucache_validate_store_arr(HashTable *vals, size_t key_len_max)
{
	zend_string *key;
	zval *val;

	ZEND_HASH_FOREACH_STR_KEY_VAL(vals, key, val) {
		if (key != NULL && !ucache_user_key_is_valid(key)) {
			zend_argument_value_error(1, "must be an array with non-empty string or int keys that do not contain " UCACHE_KEY_DELIM_NAME);

			return false;
		}

		if (key != NULL && ZSTR_LEN(key) > key_len_max) {
			zend_argument_value_error(1, "must be an array whose keys are not longer than %zu bytes", key_len_max);

			return false;
		}

		if (!ucache_validate_root_val(val)) {
			return false;
		}
	} ZEND_HASH_FOREACH_END();

	return true;
}

static zend_string *ucache_build_scope_prefix(zend_string *scope)
{
	return zend_string_concat2(
		ZSTR_VAL(scope),
		ZSTR_LEN(scope),
		ZEND_STRL(UCACHE_KEY_DELIM)
	);
}

static void ucache_reset_class_entries(void)
{
	ucache_availability_ce = NULL;
	ucache_status_ce = NULL;
	ucache_pool_status_ce = NULL;
	ucache_ce = NULL;
}

static bool ucache_validate_non_negative(zend_long val, uint32_t arg_num)
{
	if (val < 0) {
		zend_argument_value_error(arg_num, "must be greater than or equal to 0");

		return false;
	}

	return true;
}

static zend_never_inline bool ucache_available_after_resolving(void)
{
	ucache_ensure_ready_impl();

	return ucache_active_runtime()->available && !EG(exception);
}

static PHP_UCACHE_HOT bool ucache_available(void)
{
	if (EXPECTED(ucache_runtime_is_resolved())) {
		return ucache_active_runtime()->available;
	}

	return ucache_available_after_resolving();
}

static bool ucache_begin_read(void)
{
	if (!ucache_available()) {
		return false;
	}

	if (!ucache_rlock()) {
		return false;
	}

	if (!ucache_hdr_is_initialized_locked()) {
		ucache_unlock();

		return false;
	}

	return true;
}

static bool ucache_begin_write(void)
{
	if (!ucache_wlock()) {
		return false;
	}

	if (!ucache_hdr_init_locked()) {
		ucache_unlock();

		return false;
	}

	return true;
}

static void ucache_forget_deleted_record(ucache_key_record *record, uint64_t epoch)
{
	ucache_key_record_deleted(record, epoch);

	ucache_release_req_local_slot(record->storage_key);
}

static bool ucache_delete_api(ucache_key_record *record)
{
	uint64_t epoch;

	if (!ucache_wlock_for_entry_mutation(record->storage_key)) {
		return false;
	}

	epoch = ucache_delete_locked(record->storage_key);

	ucache_unlock();

	ucache_forget_deleted_record(record, epoch);

	return true;
}

static void ucache_delete_unrestorable_entry(
		ucache_key_record *record,
		uint64_t gen)
{
	uint64_t epoch;

	if (EG(exception) || gen == 0 || !ucache_try_wlock_for_entry_mutation(record->storage_key)) {
		return;
	}

	epoch = ucache_delete_gen_locked(record->storage_key, gen);

	ucache_unlock();

	if (epoch != 0) {
		ucache_forget_deleted_record(record, epoch);
	}
}

static void ucache_drop_unrestorable_gen(
		ucache_key_record *record,
		uint64_t gen)
{
	bool overflowed = UC_G(stack_overflowed);

	UC_G(stack_overflowed) = false;

	if (!overflowed) {
		ucache_delete_unrestorable_entry(record, gen);
	}
}

static zend_never_inline void ucache_drop_unrestorable_entry(ucache_key_record *record)
{
	ucache_drop_unrestorable_gen(record, UC_G(unrestorable_gen));
}

static bool ucache_clear_api(
		zend_string *scope_prefix)
{
	if (!ucache_begin_write()) {
		return false;
	}

	if (!ucache_entry_locks_allow_clear_locked(scope_prefix)) {
		ucache_unlock();

		return false;
	}

	ucache_delete_by_prefix_locked(scope_prefix);

	ucache_unlock();

	ucache_release_active_req_local_slots_by_prefix(scope_prefix);

	return true;
}

static zend_never_inline bool ucache_fetch_if_present_fallback(
		ucache_key_record *record,
		zval *return_value)
{
	ucache_fetch_pending_seed pending_seed;
	bool fetched, entry_found = false, lock_held = true;

	if (EG(exception)) {
		UC_G(stack_overflowed) = false;

		return false;
	}

	if (!ucache_begin_read()) {
		return false;
	}

	UC_G(stack_overflowed) = false;

	fetched = ucache_fetch_locked(
		record,
		return_value,
		&entry_found,
		&pending_seed,
		&lock_held
	);

	if (lock_held) {
		ucache_unlock();
	}

	ucache_fetch_finish(record, &pending_seed, return_value);

	if (fetched) {
		return true;
	}

	if (!entry_found) {
		return false;
	}

	ucache_drop_unrestorable_gen(record, pending_seed.gen);

	return false;
}

static PHP_UCACHE_HOT bool ucache_fetch_if_present_api(
		ucache_key_record *record,
		zval *return_value)
{
	ucache_optimistic_result result;

	if (!ucache_available()) {
		return false;
	}

	UC_G(stack_overflowed) = false;

	result = ucache_fetch_optimistic(record, return_value, true);
	if (EXPECTED(result == UCACHE_OPTIMISTIC_FOUND)) {
		return true;
	}

	if (result == UCACHE_OPTIMISTIC_MISS) {
		return false;
	}

	if (result == UCACHE_OPTIMISTIC_UNRESTORABLE) {
		ucache_drop_unrestorable_entry(record);

		return false;
	}

	return ucache_fetch_if_present_fallback(record, return_value);
}

static void ucache_fetch_api(
		ucache_key_record *record,
		zval *default_val,
		zval *return_value)
{
	if (!ucache_fetch_if_present_api(record, return_value)) {
		if (default_val != NULL) {
			ZVAL_COPY(return_value, default_val);
		} else {
			ZVAL_NULL(return_value);
		}
	}
}

static void ucache_fetch_multiple_fetch_one(
		ucache_key_record *record,
		zval *default_val,
		bool *rlock_held,
		zval *out,
		ucache_fetch_pending_seed *pending_seed)
{
	bool fetched, found, lock_held = true;

	pending_seed->should_seed = false;

	if (!*rlock_held) {
		ucache_fetch_api(record, default_val, out);

		return;
	}

	UC_G(stack_overflowed) = false;

	fetched = ucache_fetch_locked(record, out, &found, pending_seed, &lock_held);

	*rlock_held = lock_held;

	if (fetched) {
		return;
	}

	if (!found) {
		ZVAL_COPY(out, default_val);

		return;
	}

	if (lock_held) {
		ucache_unlock();

		*rlock_held = false;
	}

	ucache_drop_unrestorable_gen(record, pending_seed->gen);

	if (EG(exception)) {
		ZVAL_UNDEF(out);
	} else {
		ZVAL_COPY(out, default_val);
	}
}

static void ucache_key_records_schedule_trim(uint32_t limit, size_t budget)
{
	if (ucache_key_records_within(limit, budget)) {
		UC_G(key_record_trim_at) = limit;
		UC_G(key_record_bytes_trim_at) = budget;

		return;
	}

	UC_G(key_record_trim_at) = UC_G(key_record_count) + limit / 2;
	UC_G(key_record_bytes_trim_at) = UC_G(key_record_bytes) + budget / 2;
}

static void ucache_key_record_free(ucache_key_record *record)
{
	UC_G(key_record_bytes) -= ucache_key_record_footprint(record);

	ucache_key_record_reset(record, NULL);
	zend_string_release(record->storage_key);

	efree(record);

	UC_G(key_record_count)--;
}

static void ucache_key_record_dtor(zval *zv)
{
	ucache_key_record_free(Z_PTR_P(zv));
}

static PHP_UCACHE_HOT ucache_key_record *ucache_key_record_find(
		ucache_obj *cache,
		zend_string *key)
{
	uint32_t i;

	if (cache->key_records != NULL) {
		return zend_hash_find_ptr(cache->key_records, key);
	}

	for (i = 0; i < cache->inline_count; i++) {
		if (zend_string_equals(cache->inline_keys[i], key)) {
			return cache->inline_records[i];
		}
	}

	return NULL;
}

static ucache_key_record *ucache_key_record_create(
		ucache_obj *cache,
		zend_string *key)
{
	ucache_key_record *record = emalloc(sizeof(*record));
	uint32_t i;

	record->storage_key = zend_string_concat2(
		ZSTR_VAL(cache->scope_prefix),
		ZSTR_LEN(cache->scope_prefix),
		ZSTR_VAL(key),
		ZSTR_LEN(key)
	);
	record->mutation_epoch = 0;
	record->slot_idx = 0;
	record->pinned_payload_offset = 0;
	record->expires_at = 0;
	record->state = UCACHE_RECORD_EMPTY;
	record->val_kind = UCACHE_RECORD_VAL_NONE;
	record->access_touches = 0;

	ZVAL_UNDEF(&record->val);

	zend_string_hash_val(record->storage_key);

	UC_G(key_record_count)++;
	UC_G(key_record_bytes) += ucache_key_record_footprint(record);

	if (UNEXPECTED(UC_G(key_record_trim_at) == 0)) {
		ucache_key_records_schedule_trim(ucache_key_record_limit(), ucache_req_cache_budget());
	}

	if (UNEXPECTED(UC_G(key_record_count) >= UC_G(key_record_trim_at)) ||
		UNEXPECTED(UC_G(key_record_bytes) >= UC_G(key_record_bytes_trim_at))
	) {
		UC_G(key_records_trim_due) = true;
	}

	if (cache->key_records == NULL) {
		if (cache->inline_count < UCACHE_INLINE_KEY_RECORDS) {
			cache->inline_keys[cache->inline_count] = zend_string_copy(key);
			cache->inline_records[cache->inline_count++] = record;

			return record;
		}

		ALLOC_HASHTABLE(cache->key_records);

		zend_hash_init(cache->key_records, UCACHE_INLINE_KEY_RECORDS * 2, NULL, ucache_key_record_dtor, 0);

		for (i = 0; i < cache->inline_count; i++) {
			zend_hash_add_new_ptr(cache->key_records, cache->inline_keys[i], cache->inline_records[i]);
			zend_string_release(cache->inline_keys[i]);
		}

		cache->inline_count = 0;
	}

	zend_hash_add_new_ptr(cache->key_records, key, record);

	return record;
}

static ucache_key_record *ucache_key_record_lookup(
		ucache_obj *cache,
		zend_string *key)
{
	ucache_key_record *record = ucache_key_record_find(cache, key);

	return record != NULL ? record : ucache_key_record_create(cache, key);
}

static void ucache_key_record_reset_deferred(ucache_key_record *record, zval *released)
{
	zval detached;

	ZVAL_UNDEF(&detached);

	ucache_key_record_reset(record, &detached);

	if (Z_ISUNDEF(detached)) {
		return;
	}

	if (Z_ISUNDEF_P(released)) {
		array_init(released);
	}

	add_next_index_zval(released, &detached);
}

static void ucache_key_records_reset(ucache_obj *cache)
{
	ucache_key_record *record;
	zval released;
	uint32_t i;

	ZVAL_UNDEF(&released);

	if (cache->key_records == NULL) {
		for (i = 0; i < cache->inline_count; i++) {
			ucache_key_record_reset_deferred(cache->inline_records[i], &released);
		}
	} else {
		ZEND_HASH_FOREACH_PTR(cache->key_records, record) {
			ucache_key_record_reset_deferred(record, &released);
		} ZEND_HASH_FOREACH_END();
	}

	zval_ptr_dtor(&released);
}

static void ucache_key_records_free(ucache_obj *cache)
{
	ucache_key_record *inline_records[UCACHE_INLINE_KEY_RECORDS];
	zend_string *inline_keys[UCACHE_INLINE_KEY_RECORDS];
	HashTable *records = cache->key_records;
	uint32_t i, inline_count = cache->inline_count;

	memcpy(inline_records, cache->inline_records, sizeof(inline_records));
	memcpy(inline_keys, cache->inline_keys, sizeof(inline_keys));

	cache->key_records = NULL;
	cache->inline_count = 0;

	if (records != NULL) {
		zend_hash_destroy(records);

		FREE_HASHTABLE(records);
	}

	for (i = 0; i < inline_count; i++) {
		zend_string_release(inline_keys[i]);
		ucache_key_record_free(inline_records[i]);
	}
}

static bool ucache_key_record_is_held(const HashTable *held, const ucache_key_record *record)
{
	return zend_hash_index_exists(held, (zend_ulong) (uintptr_t) record);
}

static void ucache_key_records_drop_oldest_inline(ucache_obj *cache, const HashTable *held, zval *released)
{
	ucache_key_record *dropped[UCACHE_INLINE_KEY_RECORDS], *record;
	zend_string *dropped_keys[UCACHE_INLINE_KEY_RECORDS];
	uint32_t i, kept_count = 0, dropped_count = 0;

	for (i = 0; i < cache->inline_count; i++) {
		if (!ucache_key_record_is_held(held, cache->inline_records[i])) {
			ucache_key_record_reset_deferred(cache->inline_records[i], released);
		}
	}

	if (UCACHE_DEBUG_FAULT("FORCE_KEY_RECORD_TRIM_BAILOUT")) {
		zend_bailout();
	}

	for (i = 0; i < cache->inline_count; i++) {
		record = cache->inline_records[i];
		if (ucache_key_record_is_held(held, record)) {
			cache->inline_keys[kept_count] = cache->inline_keys[i];
			cache->inline_records[kept_count++] = record;

			continue;
		}

		dropped_keys[dropped_count] = cache->inline_keys[i];
		dropped[dropped_count++] = record;
	}

	cache->inline_count = kept_count;

	for (i = 0; i < dropped_count; i++) {
		zend_string_release(dropped_keys[i]);
		ucache_key_record_free(dropped[i]);
	}
}

static void ucache_key_records_drop_oldest_half(ucache_obj *cache, const HashTable *held, zval *released)
{
	ucache_key_record **dropped, *record;
	HashTable *kept;
	Bucket *bucket;
	uint32_t i, count, drop, dropped_count = 0;

	if (cache->key_records == NULL) {
		ucache_key_records_drop_oldest_inline(cache, held, released);

		return;
	}

	count = zend_hash_num_elements(cache->key_records);
	drop = count - count / 2;
	if (drop == 0) {
		return;
	}

	dropped = safe_emalloc(drop, sizeof(*dropped), 0);

	ALLOC_HASHTABLE(kept);

	zend_hash_init(kept, count - drop, NULL, ucache_key_record_dtor, 0);

	ZEND_HASH_MAP_FOREACH_BUCKET(cache->key_records, bucket) {
		record = Z_PTR(bucket->val);
		if (dropped_count < drop && !ucache_key_record_is_held(held, record)) {
			ucache_key_record_reset_deferred(record, released);

			dropped[dropped_count++] = record;

			continue;
		}

		zend_hash_add_new_ptr(kept, bucket->key, record);
	} ZEND_HASH_FOREACH_END();

	cache->key_records->pDestructor = NULL;

	zend_hash_destroy(cache->key_records);

	FREE_HASHTABLE(cache->key_records);

	cache->key_records = kept;

	for (i = 0; i < dropped_count; i++) {
		ucache_key_record_free(dropped[i]);
	}

	efree(dropped);
}

static ucache_held_records *ucache_held_records_begin(ucache_key_record **records, uint32_t count)
{
	ucache_held_records *held_records;

	if (count == 0) {
		return NULL;
	}

	held_records = emalloc(sizeof(*held_records));
	held_records->prev = NULL;
	held_records->next = UC_G(held_records);
	held_records->records = records;
	held_records->count = count;

	if (held_records->next != NULL) {
		held_records->next->prev = held_records;
	}

	UC_G(held_records) = held_records;

	return held_records;
}

static void ucache_held_records_end(ucache_held_records *held_records)
{
	if (held_records == NULL) {
		return;
	}

	if (held_records->prev != NULL) {
		held_records->prev->next = held_records->next;
	} else {
		UC_G(held_records) = held_records->next;
	}

	if (held_records->next != NULL) {
		held_records->next->prev = held_records->prev;
	}

	efree(held_records);
}

static void ucache_key_records_hold_key(ucache_obj *cache, zend_string *key, HashTable *held)
{
	ucache_key_record *record = ucache_key_record_find(cache, key);

	if (record != NULL) {
		zend_hash_index_add_empty_element(held, (zend_ulong) (uintptr_t) record);
	}
}

static void ucache_key_records_hold_idx_key(ucache_obj *cache, zend_ulong idx, HashTable *held)
{
	zend_string *key = zend_long_to_str((zend_long) idx);

	ucache_key_records_hold_key(cache, key, held);

	zend_string_release(key);
}

static void ucache_key_records_hold_frames(zend_execute_data *ex, HashTable *held)
{
	ucache_obj *cache;
	zend_string *key;
	zend_ulong idx;
	zval *arg;

	for (; ex != NULL; ex = ex->prev_execute_data) {
		cache = ucache_frame_pool(ex);
		if (cache == NULL || ZEND_CALL_NUM_ARGS(ex) == 0) {
			continue;
		}

		arg = ZEND_CALL_ARG(ex, 1);

		if (Z_TYPE_P(arg) == IS_STRING) {
			ucache_key_records_hold_key(cache, Z_STR_P(arg), held);
		} else if (Z_TYPE_P(arg) == IS_ARRAY &&
			zend_string_equals_literal_ci(ex->func->common.function_name, "storeMultiple")
		) {
			ZEND_HASH_FOREACH_KEY(Z_ARRVAL_P(arg), idx, key) {
				if (key != NULL) {
					ucache_key_records_hold_key(cache, key, held);
				} else {
					ucache_key_records_hold_idx_key(cache, idx, held);
				}
			} ZEND_HASH_FOREACH_END();
		}
	}
}

static void ucache_key_records_collect_held(HashTable *held)
{
	ucache_held_records *held_records;
	zend_object **bucket, **end;
	zend_fiber *fiber;
	uint32_t i;

	for (held_records = UC_G(held_records); held_records != NULL; held_records = held_records->next) {
		for (i = 0; i < held_records->count; i++) {
			zend_hash_index_add_empty_element(held, (zend_ulong) (uintptr_t) held_records->records[i]);
		}
	}

	ucache_key_records_hold_frames(EG(current_execute_data)->prev_execute_data, held);

	end = EG(objects_store).object_buckets + EG(objects_store).top;
	for (bucket = EG(objects_store).object_buckets + 1; bucket < end; bucket++) {
		if (!IS_OBJ_VALID(*bucket) || (*bucket)->ce != zend_ce_fiber) {
			continue;
		}

		fiber = (zend_fiber *) *bucket;
		if (fiber->context.status == ZEND_FIBER_STATUS_SUSPENDED) {
			ucache_key_records_hold_frames(fiber->execute_data, held);
		}
	}
}

static zend_never_inline void ucache_key_records_trim_pools(uint32_t limit, size_t budget)
{
	ucache_obj *cache;
	HashTable held;
	zval released;
	uint32_t before;

	ZVAL_UNDEF(&released);

	zend_hash_init(&held, 8, NULL, NULL, 0);

	ucache_key_records_collect_held(&held);

	do {
		before = UC_G(key_record_count);

		for (cache = UC_G(live_pools); cache != NULL; cache = cache->live_next) {
			ucache_key_records_drop_oldest_half(cache, &held, &released);
		}
	} while (UC_G(key_record_count) < before &&
		(UC_G(key_record_count) > limit / 2 || UC_G(key_record_bytes) > budget / 2)
	);

	zend_hash_destroy(&held);

	zval_ptr_dtor(&released);
}

static zend_never_inline void ucache_key_records_trim(void)
{
	uint32_t limit = ucache_key_record_limit();
	size_t budget = ucache_req_cache_budget();

	UC_G(key_records_trim_due) = false;

	if (!ucache_key_records_within(limit, budget)) {
		ucache_key_records_trim_pools(limit, budget);
	}

	ucache_key_records_schedule_trim(limit, budget);
}

static zend_never_inline ucache_obj *ucache_obj_uninitialized(zval *this_ptr)
{
	zend_throw_error(NULL, "%s instance was not initialized", ZSTR_VAL(Z_OBJCE_P(this_ptr)->name));

	return NULL;
}

static zend_never_inline ucache_key_record *ucache_key_too_long(ucache_obj *cache)
{
	zend_argument_value_error(1, "must not be longer than %zu bytes", ucache_key_len_max(cache));

	return NULL;
}

static PHP_UCACHE_HOT ucache_key_record *ucache_validated_key_record(
		ucache_obj *cache,
		zend_string *key)
{
	ucache_key_record *record = ucache_key_record_find(cache, key);

	if (record != NULL) {
		return record;
	}

	if (!ucache_validate_delim_free(key, "must be a non-empty string")) {
		return NULL;
	}

	if (UNEXPECTED(ZSTR_LEN(key) > ucache_key_len_max(cache))) {
		return ucache_key_too_long(cache);
	}

	return ucache_key_record_create(cache, key);
}

static bool ucache_key_record_list(
		ucache_obj *cache,
		HashTable *keys,
		ucache_key_record ***records_ptr,
		zval **key_copies_ptr)
{
	ucache_key_record **records;
	zend_string *key;
	zval *val, *key_copies = NULL;
	uint32_t i = 0, count = zend_hash_num_elements(keys);
	bool too_long = false;

	*records_ptr = NULL;

	if (key_copies_ptr != NULL) {
		*key_copies_ptr = NULL;
	}

	if (count == 0) {
		return true;
	}

	records = safe_emalloc(count, sizeof(*records), 0);

	if (key_copies_ptr != NULL) {
		key_copies = safe_emalloc(count, sizeof(zval), 0);
	}

	ZEND_HASH_FOREACH_VAL(keys, val) {
		ZVAL_DEREF(val);

		if (Z_TYPE_P(val) == IS_STRING) {
			records[i] = ucache_key_record_find(cache, Z_STR_P(val));
			if (records[i] == NULL) {
				if (!ucache_user_key_is_valid(Z_STR_P(val))) {
					break;
				}

				if (Z_STRLEN_P(val) > ucache_key_len_max(cache)) {
					too_long = true;

					break;
				}

				records[i] = ucache_key_record_create(cache, Z_STR_P(val));
			}
		} else if (Z_TYPE_P(val) == IS_LONG) {
			key = zend_long_to_str(Z_LVAL_P(val));
			records[i] = ucache_key_record_lookup(cache, key);

			zend_string_release(key);
		} else {
			break;
		}

		if (key_copies != NULL) {
			ZVAL_COPY(&key_copies[i], val);
		}

		i++;
	} ZEND_HASH_FOREACH_END();

	if (i != count) {
		if (too_long) {
			zend_argument_value_error(
				1,
				"must contain only cache keys that are not longer than %zu bytes",
				ucache_key_len_max(cache)
			);
		} else {
			zend_argument_value_error(
				1,
				"must contain only non-empty string or int cache keys that do not contain " UCACHE_KEY_DELIM_NAME
			);
		}

		if (key_copies != NULL) {
			while (i > 0) {
				zval_ptr_dtor_nogc(&key_copies[--i]);
			}

			efree(key_copies);
		}

		efree(records);

		return false;
	}

	*records_ptr = records;

	if (key_copies_ptr != NULL) {
		*key_copies_ptr = key_copies;
	}

	return true;
}

static zend_result ucache_fetch_multiple_api(
		ucache_obj *cache,
		HashTable *keys,
		zval *default_val,
		zval *return_value)
{
	ucache_fetch_pending_seed *pending_seeds;
	ucache_held_records *held_records;
	ucache_key_record **records;
	zval *vals, *key_copies;
	uint32_t count = zend_hash_num_elements(keys), i, p, pending_count = 0, *pending_idx;
	bool rlock_held, backend_readable;

	if (!ucache_key_record_list(cache, keys, &records, &key_copies)) {
		return FAILURE;
	}

	held_records = ucache_held_records_begin(records, count);

	backend_readable = ucache_available();

	pending_seeds = NULL;
	pending_idx = NULL;
	vals = NULL;

	if (count != 0) {
		pending_seeds = safe_emalloc(count, sizeof(ucache_fetch_pending_seed), 0);
		pending_idx = safe_emalloc(count, sizeof(uint32_t), 0);
		vals = safe_emalloc(count, sizeof(zval), 0);

		for (i = 0; i < count; i++) {
			pending_seeds[i].should_seed = false;

			ZVAL_UNDEF(&pending_seeds[i].detached_val);
			ZVAL_UNDEF(&vals[i]);
		}
	}

	for (i = 0; backend_readable && i < count && !EG(exception); i++) {
		UC_G(stack_overflowed) = false;

		switch (ucache_fetch_optimistic(records[i], &vals[i], false)) {
			case UCACHE_OPTIMISTIC_FOUND:
			case UCACHE_OPTIMISTIC_MISS:
				continue;
			case UCACHE_OPTIMISTIC_UNRESTORABLE:
				ucache_drop_unrestorable_entry(records[i]);

				continue;
			case UCACHE_OPTIMISTIC_FALLBACK:
				break;
		}

		if (EG(exception)) {
			UC_G(stack_overflowed) = false;

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
						records[i],
						default_val,
						&rlock_held,
						&vals[i],
						&pending_seeds[i]
					);

					if (EG(exception)) {
						break;
					}
				}
			} zend_catch {
				ucache_unlock_if_held();

				zend_bailout();
			} zend_end_try();

			if (rlock_held) {
				ucache_unlock();
			}
		}
	}

	if (pending_idx != NULL) {
		efree(pending_idx);
	}

	if (EG(exception)) {
		for (i = 0; i < count; i++) {
			zval_ptr_dtor(&vals[i]);
			zval_ptr_dtor_nogc(&key_copies[i]);
			ucache_fetch_finish(records[i], &pending_seeds[i], NULL);
		}

		ucache_held_records_end(held_records);

		if (vals != NULL) {
			efree(vals);
			efree(key_copies);
			efree(pending_seeds);
			efree(records);
		}

		return FAILURE;
	}

	array_init_size(return_value, count);

	for (i = 0; i < count; i++) {
		if (Z_ISUNDEF(vals[i])) {
			ZVAL_COPY(&vals[i], default_val);
		}

		ucache_fetch_finish(records[i], &pending_seeds[i], &vals[i]);

		if (Z_TYPE(key_copies[i]) == IS_LONG) {
			zend_hash_index_update(Z_ARRVAL_P(return_value), Z_LVAL(key_copies[i]), &vals[i]);
		} else {
			zend_symtable_update(Z_ARRVAL_P(return_value), Z_STR(key_copies[i]), &vals[i]);
			zend_string_release(Z_STR(key_copies[i]));
		}
	}

	ucache_held_records_end(held_records);

	if (vals != NULL) {
		efree(vals);
		efree(key_copies);
		efree(pending_seeds);
		efree(records);
	}

	return SUCCESS;
}

static bool ucache_atomic_update_api(
		ucache_key_record *record,
		zend_long step,
		zend_long ttl,
		bool decrement,
		ucache_atomic_update_result *result)
{
	bool updated;

	if (!ucache_available()) {
		memset(result, 0, sizeof(*result));

		return false;
	}

	if (ucache_try_atomic_update(record, step, ttl, decrement, result, &updated)) {
		return updated;
	}

	if (!ucache_wlock_for_entry_mutation(record->storage_key)) {
		memset(result, 0, sizeof(*result));

		return false;
	}

	updated = ucache_atomic_update_locked(
		record->storage_key,
		step,
		ttl,
		decrement,
		result
	);

	ucache_unlock();

	ucache_key_record_atomic_updated(record, result);

	if (result->is_overflow || result->is_type_err) {
		updated = false;
	}

	return updated;
}

static bool ucache_exists_api(ucache_key_record *record)
{
	bool exists;

	if (!ucache_available()) {
		return false;
	}

	switch (ucache_exists_optimistic(record)) {
		case UCACHE_OPTIMISTIC_FOUND:
			return true;
		case UCACHE_OPTIMISTIC_MISS:
			return false;
		case UCACHE_OPTIMISTIC_UNRESTORABLE:
		case UCACHE_OPTIMISTIC_FALLBACK:
			break;
	}

	if (!ucache_begin_read()) {
		return false;
	}

	exists = ucache_exists_locked(record->storage_key);

	ucache_unlock();

	return exists;
}

static bool ucache_lock_api(
		zend_string *key,
		zend_long lease)
{
	if (!ucache_available()) {
		return false;
	}

	return ucache_try_acquire_entry_lock(key, ucache_clamp_lock_lease(lease));
}

static bool ucache_unlock_api(zend_string *key)
{
	if (!ucache_available()) {
		return false;
	}

	return ucache_release_entry_lock(key);
}

static void ucache_release_remember_lock(zend_string *key)
{
	if (ucache_available()) {
		(void) ucache_release_entry_lock_unless_requested(key);
	}
}

static void ucache_return_status(zval *return_value)
{
	ucache_status_obj *status;

	ucache_ensure_ready();

	object_init_ex(return_value, ucache_status_ce);

	status = ucache_status_obj_from_obj(Z_OBJ_P(return_value));
	status->availability_reason = ucache_active_runtime()->failure_reason;

	ucache_collect_info_stats(&status->stats);

	status->initialized = true;
}

static void ucache_return_pool_status(
		ucache_obj *cache,
		zval *return_value)
{
	ucache_pool_status_obj *status;

	ucache_ensure_ready();

	object_init_ex(return_value, ucache_pool_status_ce);

	status = ucache_pool_status_obj_from_obj(Z_OBJ_P(return_value));
	status->scope = zend_string_copy(cache->scope);

	ZVAL_EMPTY_ARRAY(&status->entry_keys);

	if (!ucache_available()) {
		return;
	}

	ucache_collect_pool_status(
		cache,
		&status->entry_count,
		&status->used_mem,
		&status->entry_keys
	);
}

static void ucache_obj_free(zend_object *obj)
{
	ucache_obj *cache = ucache_obj_from_obj(obj);

	if (cache->live_prev != NULL) {
		cache->live_prev->live_next = cache->live_next;
	} else {
		UC_G(live_pools) = cache->live_next;
	}

	if (cache->live_next != NULL) {
		cache->live_next->live_prev = cache->live_prev;
	}

	zend_object_std_dtor(&cache->std);

	if (UC_G(pool_status_snapshots) != NULL) {
		zend_hash_index_del(UC_G(pool_status_snapshots), obj->handle);
	}

	if (cache->scope != NULL) {
		zend_string_release(cache->scope);
	}

	if (cache->scope_prefix != NULL) {
		zend_string_release(cache->scope_prefix);
	}

	ucache_key_records_free(cache);
}

static zend_object *ucache_obj_create(zend_class_entry *ce)
{
	ucache_obj *cache;

	cache = zend_object_alloc(sizeof(ucache_obj), ce);

	zend_object_std_init(&cache->std, ce);
	object_properties_init(&cache->std, ce);

	cache->scope = NULL;
	cache->scope_prefix = NULL;
	cache->live_prev = NULL;
	cache->live_next = UC_G(live_pools);
	cache->key_records = NULL;
	cache->inline_count = 0;
	cache->key_len_max = 0;
	cache->ctx = ucache_owning_ctx();
	cache->std.handlers = &ucache_obj_handlers;

	if (cache->live_next != NULL) {
		cache->live_next->live_prev = cache;
	}

	UC_G(live_pools) = cache;

	return &cache->std;
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

	ucache_ce->create_object = ucache_obj_create;
	ucache_status_ce->create_object = ucache_status_obj_create;
	ucache_pool_status_ce->create_object = ucache_pool_status_obj_create;

	memcpy(
		&ucache_obj_handlers,
		zend_get_std_object_handlers(),
		sizeof(zend_object_handlers)
	);

	ucache_obj_handlers.offset = offsetof(ucache_obj, std);
	ucache_obj_handlers.free_obj = ucache_obj_free;
	ucache_obj_handlers.clone_obj = NULL;
	ucache_obj_handlers.compare = zend_objects_not_comparable;

	memcpy(
		&ucache_status_obj_handlers,
		zend_get_std_object_handlers(),
		sizeof(zend_object_handlers)
	);

	ucache_status_obj_handlers.offset = offsetof(ucache_status_obj, std);
	ucache_status_obj_handlers.clone_obj = NULL;
	ucache_status_obj_handlers.compare = zend_objects_not_comparable;

	memcpy(
		&ucache_pool_status_obj_handlers,
		zend_get_std_object_handlers(),
		sizeof(zend_object_handlers)
	);

	ucache_pool_status_obj_handlers.offset = offsetof(ucache_pool_status_obj, std);
	ucache_pool_status_obj_handlers.free_obj = ucache_pool_status_obj_free;
	ucache_pool_status_obj_handlers.clone_obj = NULL;
	ucache_pool_status_obj_handlers.compare = zend_objects_not_comparable;
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
	HashTable *pools = UC_G(pool_table);

	UC_G(pool_table) = NULL;

	if (pools != NULL) {
		zend_hash_destroy(pools);

		FREE_HASHTABLE(pools);
	}
}

static zend_object *ucache_create_pool_obj(zend_string *pool)
{
	zend_object *obj = ucache_obj_create(ucache_ce);
	ucache_obj *cache = ucache_obj_from_obj(obj);

	cache->scope = zend_string_copy(pool);
	cache->scope_prefix = ucache_build_scope_prefix(pool);
	cache->key_len_max = (uint32_t) (UCACHE_STORAGE_KEY_MAX - ZSTR_LEN(cache->scope_prefix));

	return obj;
}

static zend_never_inline void ucache_pools_trim(HashTable *pools, uint32_t limit)
{
	zend_string **names, *name;
	zval *pool_zv;
	uint32_t i,
		count = 0,
		drop = zend_hash_num_elements(pools) - MIN(zend_hash_num_elements(pools), limit / 2)
	;

	names = safe_emalloc(drop, sizeof(zend_string *), 0);
	ZEND_HASH_FOREACH_STR_KEY_VAL(pools, name, pool_zv) {
		if (count == drop) {
			break;
		}

		if (Z_REFCOUNT_P(pool_zv) == 1) {
			names[count++] = zend_string_copy(name);
		}
	} ZEND_HASH_FOREACH_END();

	for (i = 0; i < count; i++) {
		pool_zv = UC_G(pool_table) == pools ? zend_hash_find(pools, names[i]) : NULL;
		if (pool_zv != NULL && Z_REFCOUNT_P(pool_zv) == 1) {
			zend_hash_del(pools, names[i]);
		}

		zend_string_release(names[i]);
	}

	efree(names);

	if (UC_G(pool_table) == pools) {
		UC_G(pool_trim_at) = MAX(limit, zend_hash_num_elements(pools) + limit / 2);
	}
}

static zend_object *ucache_get_or_create_pool(zend_string *pool)
{
	zend_object *obj;
	zval *existing, pool_zv;
	HashTable *pools = ucache_pools();
	uint32_t limit;

	existing = zend_hash_find(pools, pool);
	if (existing != NULL) {
		return Z_OBJ_P(existing);
	}

	limit = ucache_key_record_limit();
	if (zend_hash_num_elements(pools) >= MAX(limit, UC_G(pool_trim_at))) {
		ucache_pools_trim(pools, limit);

		pools = ucache_pools();

		existing = zend_hash_find(pools, pool);
		if (existing != NULL) {
			return Z_OBJ_P(existing);
		}
	}

	obj = ucache_create_pool_obj(pool);

	ZVAL_OBJ(&pool_zv, obj);

	zend_hash_add_new(pools, pool, &pool_zv);

	return obj;
}

static void ucache_safe_direct_handlers_dtor(zval *zv)
{
	pefree(Z_PTR_P(zv), true);
}

static bool ucache_store_api(ucache_key_record *record, zval *val, zend_long ttl, bool add_only)
{
	zend_string *key = record->storage_key;
	ucache_prepared_val prepared;
	ucache_store_result result;
	bool stored;

	if (!ucache_prepare_val(key, val, &prepared)) {
		ucache_destroy_prepared_val(&prepared);

		return false;
	}

	if (!add_only && ttl == 0 && prepared.val_type <= UCACHE_VAL_DOUBLE &&
		ucache_try_store_scalar(record, &prepared)
	) {
		ucache_destroy_prepared_val(&prepared);
		ucache_release_req_local_slot(key);

		return true;
	}

	if (!ucache_wlock_for_entry_mutation(key)) {
		ucache_destroy_prepared_val(&prepared);

		return false;
	}

	if (add_only && ucache_exists_locked(key)) {
		ucache_unlock();
		ucache_destroy_prepared_val(&prepared);

		return false;
	}

	zend_try {
		stored = ucache_store_prepared_locked(key, val, &prepared, ttl, false, &result);
	} zend_catch {
		ucache_unlock_if_held();
		ucache_destroy_prepared_val(&prepared);

		zend_bailout();
	} zend_end_try();

	ucache_unlock();
	ucache_destroy_prepared_val(&prepared);

	if (stored) {
		ucache_key_record_stored(record, &result, val);
		ucache_release_req_local_slot(key);
	}

	return stored;
}

static uint32_t ucache_prepare_bulk_store_items(
		ucache_obj *cache,
		HashTable *vals,
		ucache_bulk_store_item *items,
		zend_string **storage_keys,
		bool *result)
{
	zend_ulong num_key;
	zend_string *key;
	zval *val;
	uint32_t i = 0;

	*result = true;

	ZEND_HASH_FOREACH_KEY_VAL(vals, num_key, key, val) {
		if (key != NULL) {
			items[i].record = ucache_key_record_lookup(cache, key);
		} else {
			key = zend_long_to_str(num_key);
			items[i].record = ucache_key_record_lookup(cache, key);

			zend_string_release(key);
		}

		storage_keys[i] = items[i].record->storage_key;

		ZVAL_COPY_DEREF(&items[i].val, val);

		if (!ucache_validate_root_val(&items[i].val) ||
			!ucache_prepare_val(storage_keys[i], &items[i].val, &items[i].prepared)
		) {
			ucache_destroy_prepared_val(&items[i].prepared);

			*result = false;

			break;
		}

		i++;
	} ZEND_HASH_FOREACH_END();

	return i;
}

static uint32_t ucache_commit_bulk_store_locked(
		ucache_bulk_store_item *items,
		uint32_t prepared_count,
		zend_long ttl,
		bool *result)
{
	uint32_t i, j, stored_count = 0;

	*result = true;

	UC_G(store_defer_unlock) = true;

	for (i = 0; i < prepared_count; i++) {
		if (!ucache_store_prepared_locked(
				items[i].record->storage_key,
				&items[i].val,
				&items[i].prepared,
				ttl,
				true,
				&items[i].store_result
			)
		) {
			*result = false;

			break;
		}

		items[i].store_result.committed = true;
		stored_count = i + 1;

		if (i == 0 && prepared_count > 1 && UCACHE_DEBUG_FAULT("FORCE_BULK_COMMIT_BAILOUT")) {
			zend_bailout();
		}
	}

	UC_G(store_defer_unlock) = false;

	if (*result) {
		for (i = 0; i < stored_count; i++) {
			ucache_discard_replaced_entry_locked(&items[i].store_result.replaced_entry);

			items[i].store_result.committed = false;
		}
	} else {
		for (j = stored_count; j > 0; j--) {
			ucache_rollback_replaced_entry_locked(
				items[j - 1].record->storage_key,
				&items[j - 1].store_result.replaced_entry
			);

			items[j - 1].store_result.committed = false;
		}

		stored_count = 0;
	}

	return stored_count;
}

static void ucache_abort_bulk_store_on_bailout(
		ucache_bulk_store_item *items,
		uint32_t prepared_count)
{
	uint32_t j;

	UC_G(store_defer_unlock) = false;

	ZEND_ASSERT(UC_G(lock_held));

	for (j = prepared_count; j > 0; j--) {
		if (items[j - 1].store_result.committed) {
			ucache_rollback_replaced_entry_locked(
				items[j - 1].record->storage_key,
				&items[j - 1].store_result.replaced_entry
			);

			items[j - 1].store_result.committed = false;
		}
	}

	ucache_unlock();
}

static void ucache_finish_bulk_store(ucache_bulk_store_item *items, uint32_t stored_count)
{
	uint32_t i;

	for (i = 0; i < stored_count; i++) {
		ucache_key_record_stored(items[i].record, &items[i].store_result, &items[i].val);
		ucache_release_req_local_slot(items[i].record->storage_key);
	}
}

static bool ucache_instance_store_multiple(
		ucache_obj *cache,
		HashTable *vals,
		zend_long ttl)
{
	ucache_bulk_store_item *items;
	zend_string **storage_keys;
	uint32_t i, count, prepared_count, stored_count = 0;
	bool result;

	if (!ucache_available()) {
		return false;
	}

	count = zend_hash_num_elements(vals);
	if (count == 0) {
		return true;
	}

	items = ecalloc(count, sizeof(*items));
	storage_keys = safe_emalloc(count, sizeof(zend_string *), 0);

	prepared_count = ucache_prepare_bulk_store_items(cache, vals, items, storage_keys, &result);

	if (result && ucache_wlock_for_entry_mutations(storage_keys, prepared_count)) {
		zend_try {
			stored_count = ucache_commit_bulk_store_locked(items, prepared_count, ttl, &result);
		} zend_catch {
			ucache_abort_bulk_store_on_bailout(items, prepared_count);

			zend_bailout();
		} zend_end_try();

		ucache_unlock();
	} else {
		result = false;
	}

	ucache_finish_bulk_store(items, stored_count);

	for (i = 0; i < prepared_count; i++) {
		ucache_destroy_prepared_val(&items[i].prepared);
	}

	for (i = 0; i < count; i++) {
		zval_ptr_dtor(&items[i].val);
	}

	efree(storage_keys);
	efree(items);

	return result;
}

static bool ucache_instance_delete_multiple(
		ucache_obj *cache,
		HashTable *keys)
{
	ucache_held_records *held_records;
	ucache_key_record **records;
	zend_string **storage_keys;
	uint64_t epoch = 0;
	uint32_t i, count = zend_hash_num_elements(keys);
	bool result = true;

	if (!ucache_key_record_list(cache, keys, &records, NULL)) {
		return false;
	}

	held_records = ucache_held_records_begin(records, count);

	if (!ucache_available() || count == 0) {
		ucache_held_records_end(held_records);

		if (records != NULL) {
			efree(records);
		}

		return !EG(exception);
	}

	storage_keys = safe_emalloc(count, sizeof(zend_string *), 0);

	for (i = 0; i < count; i++) {
		storage_keys[i] = records[i]->storage_key;
	}

	if (ucache_wlock_for_entry_mutations(storage_keys, count)) {
		zend_try {
			for (i = 0; i < count; i++) {
				epoch = ucache_delete_locked(storage_keys[i]);
			}
		} zend_catch {
			ucache_unlock_if_held();

			zend_bailout();
		} zend_end_try();

		ucache_unlock();
	} else {
		result = false;
	}

	if (result) {
		for (i = 0; i < count; i++) {
			ucache_key_record_deleted(records[i], epoch);
			ucache_release_req_local_slot(storage_keys[i]);
		}
	}

	ucache_held_records_end(held_records);

	efree(records);
	efree(storage_keys);

	return result;
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

#ifdef ZTS
static void ucache_globals_ctor(void *storage)
{
	ucache_globals *user_cache_globals = storage;

	memset(user_cache_globals, 0, sizeof(ucache_globals));

	user_cache_globals->persistent_exec = ucache_runtime_opted_in &&
		ucache_registered_mode == PHP_UCACHE_MODE_PERSISTENT
	;
}

static void ucache_globals_dtor(void *storage)
{
	ucache_globals *user_cache_globals = storage;

	ucache_release_thread_reader_claims(user_cache_globals);
	ucache_release_thread_graph_pin_claims(user_cache_globals);

	if (ucache_boundary_partitions_mutex != NULL) {
		ucache_orphan_thread_deferred_entry_lock_releases(user_cache_globals);
	} else {
		ucache_free_thread_deferred_entry_lock_releases(user_cache_globals);
	}
}
#endif /* ZTS */

static void ucache_minit(void)
{
	ucache_ctx *prev_ctx;

#ifdef ZTS
	if (ucache_boundary_partitions_mutex == NULL) {
		ucache_boundary_partitions_mutex = tsrm_mutex_alloc();
	}
#endif

#if defined(ZTS) && !defined(ZEND_WIN32)
	if (!ucache_zts_atfork_registered &&
		pthread_atfork(ucache_zts_atfork_prepare, ucache_zts_atfork_release, ucache_zts_atfork_child) == 0
	) {
		ucache_zts_atfork_registered = true;
	}
#endif

#ifndef ZEND_WIN32
	if (!ucache_pid_atfork_registered &&
		!ucache_self_pid_uncached &&
		pthread_atfork(NULL, NULL, ucache_pid_atfork_child) != 0
	) {
		ucache_self_pid_uncached = true;
	}

	ucache_pid_atfork_registered = !ucache_self_pid_uncached;
	ucache_self_pid = ucache_self_pid_uncached ? 0 : ucache_cur_pid();
#endif /* ZEND_WIN32 */
	ucache_runtime_opted_in = false;
	ucache_registered_mode = PHP_UCACHE_MODE_REQ;

	atomic_store(&ucache_registration_closed, false);

	ucache_register_classes();

	ucache_safe_direct_handlers_init();

	ucache_classify_sapi();

	prev_ctx = ucache_activate_ctx(ucache_owning_ctx());

	ucache_reset_storage();

	ucache_restore_ctx(prev_ctx);
}

static void ucache_mshutdown(void)
{
	ucache_ctx *prev_ctx;

	UCACHE_DEBUG_SIMULATE_KILL("EXIT_BEFORE_CLAIM_RELEASE");

	ucache_release_thread_reader_claims(UCACHE_GLOBALS_PTR());
	ucache_release_thread_graph_pin_claims(UCACHE_GLOBALS_PTR());
	ucache_free_thread_deferred_entry_lock_releases(UCACHE_GLOBALS_PTR());
#ifdef ZTS
	ucache_free_orphaned_entry_lock_releases();
#endif

	ucache_partitions_shutdown();
	ucache_boundary_partitions_shutdown();

#ifdef ZTS
	if (ucache_boundary_partitions_mutex != NULL) {
		tsrm_mutex_free(ucache_boundary_partitions_mutex);
		ucache_boundary_partitions_mutex = NULL;
	}
#endif

	UC_G(active_partition) = NULL;
	UC_G(active_ctx_ptr) = NULL;
	UC_G(exec_prepared) = false;
	UC_G(exec_prev_partition) = NULL;
	UC_G(exec_prev_ctx) = NULL;
	UC_G(exec_partition) = NULL;

	prev_ctx = ucache_activate_ctx(ucache_owning_ctx());

	ucache_shutdown_storage();
	ucache_reset_runtime();
	ucache_restore_ctx(prev_ctx);

	ucache_runtime_opted_in = false;
	ucache_registered_mode = PHP_UCACHE_MODE_REQ;

	ucache_safe_direct_handlers_destroy();
	ucache_reset_class_entries();
}

static zend_result ucache_rinit(void)
{
	if (!php_during_module_startup()) {
		atomic_store(&ucache_registration_closed, true);
	}

	if (!UC_G(exec_prepared)) {
		UC_G(persistent_exec) = ucache_runtime_opted_in &&
			ucache_registered_mode == PHP_UCACHE_MODE_PERSISTENT
		;
	}

	UC_G(exec_active) = true;
	UC_G(exec_partition) = UC_G(active_partition);
	UC_G(runtime_resolved) = false;

	return SUCCESS;
}

static zend_result ucache_rshutdown(void)
{
	UC_G(in_req_shutdown) = true;

	ucache_unlock_if_held();

	UC_G(req_unavailable_reason) = PHP_UCACHE_REASON_NONE;
	UC_G(runtime_resolved) = false;

	ucache_release_pool_status_snapshots();
	ucache_release_pools();

	ucache_release_req_entry_locks();
	ucache_release_req_local_slots();

	if (UC_G(req_local_slot_may_cycle)) {
		UC_G(req_local_slot_may_cycle) = false;

		if (!CG(unclean_shutdown)) {
			gc_collect_cycles();
		}
	}

	ucache_decode_payload_addr_caches_release();
	ucache_decode_maps_teardown();
	ucache_sgraph_obj_route_memo_release();

	return SUCCESS;
}

static zend_result ucache_post_deactivate(void)
{
	ucache_release_op_leases();
	ucache_release_req_sgraph_refs();
	ucache_abandon_graph_pin_claims();
	ucache_retry_deferred_entry_lock_releases();
	ucache_expunge_expired_at_req_end();

	UC_G(in_req_shutdown) = false;
	UC_G(runtime_resolved) = false;
	UC_G(access_now) = 0;
	UC_G(access_now_touches) = 0;
	UC_G(reader_claim_failed_hdr) = NULL;
	UC_G(key_records_trim_due) = false;
	UC_G(key_record_count) = 0;
	UC_G(key_record_trim_at) = 0;
	UC_G(key_record_bytes) = 0;
	UC_G(key_record_bytes_trim_at) = 0;
	UC_G(record_val_bytes) = 0;
	UC_G(req_local_slot_bytes) = 0;
	UC_G(pool_trim_at) = 0;
	UC_G(held_records) = NULL;
	UC_G(live_pools) = NULL;
	UC_G(exec_active) = false;
	UC_G(exec_poisoned) = false;
	UC_G(logical_req_ending) = false;
	UC_G(logical_scope_token) = 0;
	UC_G(op_depth) = 0;

	ucache_restore_exec_preparation();

	return SUCCESS;
}

static void ucache_store_method(INTERNAL_FUNCTION_PARAMETERS, bool add_only)
{
	ucache_obj *cache;
	ucache_key_record *record;
	zend_long ttl = 0;
	zend_string *key;
	zval *val;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(key)
		Z_PARAM_ZVAL(val)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(ttl)
	ZEND_PARSE_PARAMETERS_END();

	cache = ucache_obj_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	record = ucache_validated_key_record(cache, key);
	if (record == NULL) {
		RETURN_THROWS();
	}

	if (!ucache_validate_root_val(val)) {
		RETURN_THROWS();
	}

	if (!ucache_validate_non_negative(ttl, 3)) {
		RETURN_THROWS();
	}

	if (!(
			ucache_available() &&
			ucache_store_api(
				record,
				val,
				ttl,
				add_only
			)
		)
	) {
		if (EG(exception)) {
			RETURN_THROWS();
		}

		RETURN_FALSE;
	}

	RETURN_TRUE;
}

static void ucache_atomic_update_method(INTERNAL_FUNCTION_PARAMETERS, bool decrement)
{
	ucache_obj *cache;
	ucache_atomic_update_result result;
	ucache_key_record *record;
	zend_long step = 1, ttl = 0;
	zend_string *key;
	bool updated;

	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_STR(key)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(step)
		Z_PARAM_LONG(ttl)
	ZEND_PARSE_PARAMETERS_END();

	cache = ucache_obj_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	record = ucache_validated_key_record(cache, key);
	if (record == NULL) {
		RETURN_THROWS();
	}

	if (!ucache_validate_non_negative(step, 2)) {
		RETURN_THROWS();
	}

	if (!ucache_validate_non_negative(ttl, 3)) {
		RETURN_THROWS();
	}

	updated = ucache_atomic_update_api(record, step, ttl, decrement, &result);

	if (result.is_type_err) {
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

	RETURN_LONG(result.new_val);
}

static bool ucache_invoke_remember_cb(
		zend_string *key,
		ucache_key_record *record,
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

	if (Z_ISREF_P(result)) {
		zend_unwrap_reference(result);
	}

	if (!ucache_validate_root_val(result)) {
		return false;
	}

	if (ucache_available() &&
		ucache_store_api(
			record,
			result,
			ttl,
			false
		)
	) {
		return true;
	}

	return !EG(exception);
}

UCACHE_METHOD_BODY(__construct)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_throw_error(
		NULL,
		"%s instances must be obtained via %s::getPool()",
		ZSTR_VAL(ucache_ce->name),
		ZSTR_VAL(ucache_ce->name)
	);
}

UCACHE_METHOD_BODY(hasPool)
{
	zend_string *pool;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(pool)
	ZEND_PARSE_PARAMETERS_END();

	if (!ucache_validate_pool_name(pool)) {
		RETURN_THROWS();
	}

	RETURN_BOOL(UC_G(pool_table) != NULL &&
		zend_hash_exists(UC_G(pool_table), pool)
	);
}

UCACHE_METHOD_BODY(getPool)
{
	zend_string *pool;
	zend_object *obj;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(pool)
	ZEND_PARSE_PARAMETERS_END();

	if (!ucache_validate_pool_name(pool)) {
		RETURN_THROWS();
	}

	if (UC_G(in_req_shutdown)) {
		RETURN_OBJ(ucache_create_pool_obj(pool));
	}

	obj = ucache_get_or_create_pool(pool);

	GC_ADDREF(obj);

	RETURN_OBJ(obj);
}

UCACHE_METHOD_BODY(getPools)
{
	zval *instance;
	zend_string *pool;

	ZEND_PARSE_PARAMETERS_NONE();

	array_init(return_value);

	if (UC_G(pool_table) == NULL) {
		return;
	}

	ZEND_HASH_FOREACH_STR_KEY_VAL(UC_G(pool_table), pool, instance) {
		ZEND_ASSERT(pool != NULL);

		Z_ADDREF_P(instance);

		zend_symtable_update(Z_ARRVAL_P(return_value), pool, instance);
	} ZEND_HASH_FOREACH_END();
}

UCACHE_METHOD_BODY(deletePool)
{
	zend_string *pool, *scope_prefix;
	zend_object *pool_obj;
	zval *instance;
	bool cleared = true;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(pool)
	ZEND_PARSE_PARAMETERS_END();

	if (!ucache_validate_pool_name(pool)) {
		RETURN_THROWS();
	}

	if (ucache_available()) {
		scope_prefix = ucache_build_scope_prefix(pool);
		cleared = ucache_clear_api(scope_prefix);

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
		instance = zend_hash_find(UC_G(pool_table), pool);
		if (instance != NULL) {
			pool_obj = Z_OBJ_P(instance);

			GC_ADDREF(pool_obj);

			ucache_key_records_reset(ucache_obj_from_obj(pool_obj));

			instance = zend_hash_find(UC_G(pool_table), pool);
			if (instance != NULL && Z_OBJ_P(instance) == pool_obj) {
				zend_hash_del(UC_G(pool_table), pool);
			}

			OBJ_RELEASE(pool_obj);
		}
	}

	RETURN_TRUE;
}

UCACHE_METHOD_BODY(getStatus)
{
	ZEND_PARSE_PARAMETERS_NONE();

	ucache_return_status(return_value);

	if (EG(exception)) {
		RETURN_THROWS();
	}
}

UCACHE_METHOD_BODY(store)
{
	ucache_store_method(INTERNAL_FUNCTION_PARAM_PASSTHRU, false);
}

UCACHE_METHOD_BODY(add)
{
	ucache_store_method(INTERNAL_FUNCTION_PARAM_PASSTHRU, true);
}

UCACHE_METHOD_BODY(storeMultiple)
{
	ucache_obj *cache;
	zend_long ttl = 0;
	HashTable *vals;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ARRAY_HT(vals)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(ttl)
	ZEND_PARSE_PARAMETERS_END();

	cache = ucache_obj_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	if (!ucache_validate_store_arr(vals, ucache_key_len_max(cache))) {
		RETURN_THROWS();
	}

	if (!ucache_validate_non_negative(ttl, 2)) {
		RETURN_THROWS();
	}

	if (!ucache_instance_store_multiple(cache, vals, ttl)) {
		if (EG(exception)) {
			RETURN_THROWS();
		}

		RETURN_FALSE;
	}

	RETURN_TRUE;
}

UCACHE_METHOD_BODY(increment)
{
	ucache_atomic_update_method(INTERNAL_FUNCTION_PARAM_PASSTHRU, false);
}

UCACHE_METHOD_BODY(decrement)
{
	ucache_atomic_update_method(INTERNAL_FUNCTION_PARAM_PASSTHRU, true);
}

UCACHE_METHOD_BODY(fetch)
{
	ucache_obj *cache;
	ucache_key_record *record;
	zend_string *key;
	zval *default_val = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(key)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(default_val)
	ZEND_PARSE_PARAMETERS_END();

	cache = ucache_obj_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	record = ucache_validated_key_record(cache, key);
	if (record == NULL) {
		RETURN_THROWS();
	}

	ucache_fetch_api(record, default_val, return_value);
}

UCACHE_METHOD_BODY(fetchMultiple)
{
	ucache_obj *cache;
	zval *default_val = NULL, default_null;
	HashTable *keys;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ARRAY_HT(keys)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(default_val)
	ZEND_PARSE_PARAMETERS_END();

	cache = ucache_obj_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	if (default_val == NULL) {
		ZVAL_NULL(&default_null);

		default_val = &default_null;
	}

	if (ucache_fetch_multiple_api(cache, keys, default_val, return_value) == FAILURE) {
		if (EG(exception)) {
			RETURN_THROWS();
		}

		RETURN_EMPTY_ARRAY();
	}
}

UCACHE_METHOD_BODY(has)
{
	ucache_obj *cache;
	ucache_key_record *record;
	zend_string *key;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(key)
	ZEND_PARSE_PARAMETERS_END();

	cache = ucache_obj_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	record = ucache_validated_key_record(cache, key);
	if (record == NULL) {
		RETURN_THROWS();
	}

	RETURN_BOOL(ucache_exists_api(record));
}

UCACHE_METHOD_BODY(delete)
{
	ucache_obj *cache;
	ucache_key_record *record;
	zend_string *key;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(key)
	ZEND_PARSE_PARAMETERS_END();

	cache = ucache_obj_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	record = ucache_validated_key_record(cache, key);
	if (record == NULL) {
		RETURN_THROWS();
	}

	if (!ucache_available()) {
		if (EG(exception)) {
			RETURN_THROWS();
		}

		RETURN_TRUE;
	}

	if (!ucache_delete_api(record)) {
		if (EG(exception)) {
			RETURN_THROWS();
		}

		RETURN_FALSE;
	}

	RETURN_TRUE;
}

UCACHE_METHOD_BODY(deleteMultiple)
{
	ucache_obj *cache;
	HashTable *keys;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(keys)
	ZEND_PARSE_PARAMETERS_END();

	cache = ucache_obj_from_this(ZEND_THIS);
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

UCACHE_METHOD_BODY(clear)
{
	ucache_obj *cache;

	ZEND_PARSE_PARAMETERS_NONE();

	cache = ucache_obj_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	if (!ucache_available()) {
		if (EG(exception)) {
			RETURN_THROWS();
		}

		RETURN_TRUE;
	}

	if (!ucache_clear_api(cache->scope_prefix)) {
		if (EG(exception)) {
			RETURN_THROWS();
		}

		RETURN_FALSE;
	}

	ucache_key_records_reset(cache);

	RETURN_TRUE;
}

UCACHE_METHOD_BODY(lock)
{
	ucache_obj *cache;
	ucache_key_record *record;
	zend_long lease = 0;
	zend_string *key;
	bool locked;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(key)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(lease)
	ZEND_PARSE_PARAMETERS_END();

	cache = ucache_obj_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	record = ucache_validated_key_record(cache, key);
	if (record == NULL) {
		RETURN_THROWS();
	}

	if (!ucache_validate_non_negative(lease, 2)) {
		RETURN_THROWS();
	}

	locked = ucache_lock_api(record->storage_key, lease);

	RETURN_BOOL(locked);
}

UCACHE_METHOD_BODY(unlock)
{
	ucache_obj *cache;
	ucache_key_record *record;
	zend_string *key;
	bool unlocked;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(key)
	ZEND_PARSE_PARAMETERS_END();

	cache = ucache_obj_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	record = ucache_validated_key_record(cache, key);
	if (record == NULL) {
		RETURN_THROWS();
	}

	unlocked = ucache_unlock_api(record->storage_key);

	RETURN_BOOL(unlocked);
}

UCACHE_METHOD_BODY(remember)
{
	ucache_obj *cache;
	ucache_key_record *record;
	zend_long ttl = 0;
	zend_string *key, *storage_key;
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;
	zval result;
	bool found = false, preheld = false,
		locked = false, cb_failed = false
	;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(key)
		Z_PARAM_FUNC(fci, fcc)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(ttl)
	ZEND_PARSE_PARAMETERS_END();

	cache = ucache_obj_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	record = ucache_validated_key_record(cache, key);
	if (record == NULL) {
		RETURN_THROWS();
	}

	storage_key = record->storage_key;

	if (!ucache_validate_non_negative(ttl, 3)) {
		RETURN_THROWS();
	}

	found = ucache_fetch_if_present_api(record, return_value);
	if (found) {
		return;
	}

	if (EG(exception)) {
		RETURN_THROWS();
	}

	preheld = ucache_available() &&
		ucache_req_owns_entry_lock(storage_key)
	;
	locked = !preheld &&
		ucache_available() &&
		ucache_acquire_entry_lock(storage_key)
	;

	found = ucache_fetch_if_present_api(record, return_value);
	if (found) {
		if (locked) {
			ucache_release_remember_lock(storage_key);
		}

		return;
	}

	if (EG(exception)) {
		if (locked) {
			ucache_release_remember_lock(storage_key);
		}

		RETURN_THROWS();
	}

	zend_try {
		cb_failed = !ucache_invoke_remember_cb(
			key, record, &fci, &fcc, ttl, &result
		);
	} zend_catch {
		if (locked) {
			ucache_release_remember_lock(storage_key);
		}

		zend_bailout();
	} zend_end_try();

	if (cb_failed) {
		zval_ptr_dtor(&result);

		if (locked) {
			ucache_release_remember_lock(storage_key);
		}

		RETURN_THROWS();
	}

	if (locked) {
		ucache_release_remember_lock(storage_key);
	}

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

UCACHE_METHOD_BODY(getPoolStatus)
{
	ucache_obj *cache;

	ZEND_PARSE_PARAMETERS_NONE();

	cache = ucache_obj_from_this(ZEND_THIS);
	if (cache == NULL) {
		RETURN_THROWS();
	}

	ucache_return_pool_status(cache, return_value);

	if (EG(exception)) {
		RETURN_THROWS();
	}
}

static zend_long ucache_ini_parse_quantity(const zend_ini_entry *entry, const zend_string *new_value, int stage)
{
	zend_string *errstr;
	zend_long val = zend_ini_parse_quantity(new_value, &errstr);

	if (errstr != NULL) {
		if (stage != ZEND_INI_STAGE_DEACTIVATE) {
			ucache_warn("Invalid \"%s\" setting. %s", ZSTR_VAL(entry->name), ZSTR_VAL(errstr));
		}

		zend_string_release(errstr);
	}

	return val;
}

static ZEND_INI_MH(OnUpdateUserCacheShmSize)
{
	zend_long *p, size;

	p = (zend_long *) ZEND_INI_GET_ADDR();
	size = ucache_ini_parse_quantity(entry, new_value, stage);

	if (size < 0) {
		if (stage != ZEND_INI_STAGE_DEACTIVATE) {
			ucache_warn("user_cache.shm_size must be greater than or equal to 0, " ZEND_LONG_FMT " given", size);
		}

		return FAILURE;
	}

	if ((uint64_t) size > UCACHE_SHM_SIZE_MAX) {
		size = (zend_long) UCACHE_SHM_SIZE_MAX;

		if (stage != ZEND_INI_STAGE_DEACTIVATE) {
			ucache_warn("user_cache.shm_size is limited to " ZEND_LONG_FMT " bytes; clamping", size);
		}
	}

	if (size != 0 && (size_t) size < ucache_shm_size_min() && stage != ZEND_INI_STAGE_DEACTIVATE) {
		ucache_warn(
			"user_cache.shm_size (" ZEND_LONG_FMT ") is below the minimum cache layout (%zu bytes); the cache will be unavailable",
			size,
			ucache_shm_size_min()
		);
	}

	*p = size;

	return SUCCESS;
}

static ZEND_INI_MH(OnUpdateUserCacheEvictionPolicy)
{
	zend_long *p = (zend_long *) ZEND_INI_GET_ADDR();

	if (zend_string_equals_literal_ci(new_value, "lru")) {
		*p = UCACHE_EVICTION_POLICY_LRU;
	} else if (zend_string_equals_literal_ci(new_value, "clear")) {
		*p = UCACHE_EVICTION_POLICY_CLEAR;
	} else if (ZSTR_LEN(new_value) == 0 || zend_string_equals_literal_ci(new_value, "none")) {
		*p = UCACHE_EVICTION_POLICY_NONE;
	} else {
		if (stage != ZEND_INI_STAGE_DEACTIVATE) {
			ucache_warn("user_cache.eviction_policy must be one of \"lru\", \"clear\" or \"none\"");
		}

		return FAILURE;
	}

	return SUCCESS;
}

static ZEND_INI_MH(OnUpdateUserCacheLockfilePath)
{
	if (new_value == NULL) {
		return FAILURE;
	}

#ifndef ZEND_WIN32
	if (!IS_ABSOLUTE_PATH(ZSTR_VAL(new_value), ZSTR_LEN(new_value))) {
		if (stage != ZEND_INI_STAGE_DEACTIVATE) {
			ucache_warn("user_cache.lockfile_path must be an absolute path, \"%s\" given", ZSTR_VAL(new_value));
		}

		return FAILURE;
	}
#endif

	return OnUpdateString(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage);
}

static ZEND_INI_MH(OnUpdateUserCachePreferredMemoryModel)
{
	char **p = (char **) ZEND_INI_GET_ADDR();

	if (new_value == NULL || ZSTR_LEN(new_value) == 0) {
		*p = NULL;

		return SUCCESS;
	}

	if (!ucache_mem_model_is_available(ZSTR_VAL(new_value))) {
		if (stage != ZEND_INI_STAGE_DEACTIVATE) {
			ucache_warn(
				"user_cache.preferred_memory_model \"%s\" is not a memory model available on this platform",
				ZSTR_VAL(new_value)
			);
		}

		return FAILURE;
	}

	*p = ZSTR_VAL(new_value);

	return SUCCESS;
}

static ZEND_INI_MH(OnUpdateUserCacheEntriesHint)
{
	zend_long *p, hint;

	p = (zend_long *) ZEND_INI_GET_ADDR();
	hint = ucache_ini_parse_quantity(entry, new_value, stage);

	if (hint < 0) {
		if (stage != ZEND_INI_STAGE_DEACTIVATE) {
			ucache_warn("user_cache.entries_hint must be greater than or equal to 0, " ZEND_LONG_FMT " given", hint);
		}

		return FAILURE;
	}

	if (hint > UCACHE_ENTRIES_HINT_MAX) {
		hint = UCACHE_ENTRIES_HINT_MAX;

		if (stage != ZEND_INI_STAGE_DEACTIVATE) {
			ucache_warn("user_cache.entries_hint is limited to %d; clamping", UCACHE_ENTRIES_HINT_MAX);
		}
	}

	*p = hint;

	return SUCCESS;
}

static ZEND_INI_MH(OnUpdateUserCacheLockLeaseMax)
{
	zend_long *p, lease_max;

	p = (zend_long *) ZEND_INI_GET_ADDR();
	lease_max = ucache_ini_parse_quantity(entry, new_value, stage);

	if (lease_max < 0) {
		if (stage != ZEND_INI_STAGE_DEACTIVATE) {
			ucache_warn("user_cache.lock_lease_max must be greater than or equal to 0, " ZEND_LONG_FMT " given", lease_max);
		}

		return FAILURE;
	}

	*p = lease_max;

	return SUCCESS;
}

static PHP_MINIT_FUNCTION(user_cache)
{
#ifdef ZTS
	if (!ucache_globals_allocated()) {
		php_ucache_globals_startup();
	}
#endif

	REGISTER_INI_ENTRIES();

	ucache_minit();

	return SUCCESS;
}

static PHP_MSHUTDOWN_FUNCTION(user_cache)
{
	ucache_mshutdown();

	UNREGISTER_INI_ENTRIES();

	return SUCCESS;
}

static PHP_RSHUTDOWN_FUNCTION(user_cache)
{
	return ucache_rshutdown();
}

static PHP_RINIT_FUNCTION(user_cache)
{
	return ucache_rinit();
}

static PHP_MINFO_FUNCTION(user_cache)
{
	const ucache_storage *storage = &ucache_active_ctx()->storage;
	bool started = ucache_storage_startup_is_complete(storage);

	php_info_print_table_start();
	php_info_print_table_row(2, "UserCache support", "enabled");
	php_info_print_table_row(
		2,
		"Active memory model",
		started && storage->handler_name != NULL
			? storage->handler_name
			: "none"
	);
	php_info_print_table_row(2, "Active lock model", started ? ucache_lock_model_name(storage) : "none");
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}

void ucache_use_req_method_handlers(void)
{
	static const struct {
		const char *name;
		size_t name_len;
		zif_handler handler;
	} methods[] = {
		UCACHE_REQ_HANDLER("__construct", __construct),
		UCACHE_REQ_HANDLER("haspool", hasPool),
		UCACHE_REQ_HANDLER("getpool", getPool),
		UCACHE_REQ_HANDLER("getpools", getPools),
		UCACHE_REQ_HANDLER("deletepool", deletePool),
		UCACHE_REQ_HANDLER("getstatus", getStatus),
		UCACHE_REQ_HANDLER("store", store),
		UCACHE_REQ_HANDLER("add", add),
		UCACHE_REQ_HANDLER("storemultiple", storeMultiple),
		UCACHE_REQ_HANDLER("increment", increment),
		UCACHE_REQ_HANDLER("decrement", decrement),
		UCACHE_REQ_HANDLER("fetch", fetch),
		UCACHE_REQ_HANDLER("fetchmultiple", fetchMultiple),
		UCACHE_REQ_HANDLER("has", has),
		UCACHE_REQ_HANDLER("delete", delete),
		UCACHE_REQ_HANDLER("deletemultiple", deleteMultiple),
		UCACHE_REQ_HANDLER("clear", clear),
		UCACHE_REQ_HANDLER("lock", lock),
		UCACHE_REQ_HANDLER("unlock", unlock),
		UCACHE_REQ_HANDLER("remember", remember),
		UCACHE_REQ_HANDLER("getpoolstatus", getPoolStatus),
	};
	zend_function *func;
	uint32_t i;

	for (i = 0; i < sizeof(methods) / sizeof(methods[0]); i++) {
		func = zend_hash_str_find_ptr(&ucache_ce->function_table, methods[i].name, methods[i].name_len);

		ZEND_ASSERT(func != NULL && func->type == ZEND_INTERNAL_FUNCTION);

		func->internal_function.handler = methods[i].handler;
	}
}

const php_ucache_safe_direct_handlers *ucache_safe_direct_find_handlers(
		zend_class_entry *ce,
		zend_class_entry **base_ce_ptr)
{
	const php_ucache_safe_direct_handlers *handlers;

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

uint64_t ucache_resolve_pid(void)
{
	uint64_t pid = ucache_cur_pid();

#ifndef ZEND_WIN32
	if (UNEXPECTED(ucache_self_pid_uncached)) {
		return pid;
	}
#endif

	ucache_self_pid = pid;

	return pid;
}

ZEND_API void php_ucache_safe_direct_register_class(
		zend_class_entry *ce,
		const php_ucache_safe_direct_handlers *handlers)
{
	php_ucache_safe_direct_handlers handlers_copy;

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

	zend_hash_index_add_mem(
		&ucache_safe_direct_handler_table,
		(zend_ulong) (uintptr_t) ce,
		&handlers_copy,
		sizeof(handlers_copy)
	);
}

UCACHE_DEFINE_SAFE_DIRECT_HANDLER_GETTER(copy)

#ifdef ZTS
size_t php_ucache_globals_size(void)
{
	return sizeof(ucache_globals);
}

void php_ucache_globals_startup(void)
{
	user_cache_globals_id = ts_allocate_fast_id(
		&user_cache_globals_id,
		&user_cache_globals_offset,
		sizeof(ucache_globals),
		ucache_globals_ctor,
		ucache_globals_dtor
	);
}
#endif /* ZTS */

UCACHE_METHOD_ENTRY(__construct)

UCACHE_METHOD_ENTRY(hasPool)

UCACHE_METHOD_ENTRY(getPool)

UCACHE_METHOD_ENTRY(getPools)

UCACHE_METHOD_ENTRY(deletePool)

UCACHE_METHOD_ENTRY(getStatus)

UCACHE_METHOD_ENTRY(store)

UCACHE_METHOD_ENTRY(add)

UCACHE_METHOD_ENTRY(storeMultiple)

UCACHE_METHOD_ENTRY(increment)

UCACHE_METHOD_ENTRY(decrement)

UCACHE_METHOD_ENTRY(fetch)

UCACHE_METHOD_ENTRY(fetchMultiple)

UCACHE_METHOD_ENTRY(has)

UCACHE_METHOD_ENTRY(delete)

UCACHE_METHOD_ENTRY(deleteMultiple)

UCACHE_METHOD_ENTRY(clear)

UCACHE_METHOD_ENTRY(lock)

UCACHE_METHOD_ENTRY(unlock)

UCACHE_METHOD_ENTRY(remember)

UCACHE_METHOD_ENTRY(getPoolStatus)
