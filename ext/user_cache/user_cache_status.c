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
   | Author: Go Kudo <zeriyoshi@php.net>                                  |
   +----------------------------------------------------------------------+
*/

#include "user_cache_internal.h"
#include "user_cache_decl.h"

#define UCACHE_DEFINE_STATUS_LONG_GETTER(method, field) \
	ZEND_METHOD(UserCache_CacheStatus, method) \
	{ \
		ucache_status_obj *status; \
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

typedef struct {
	const ucache_ctx *ctx;
	const ucache_hdr *hdr;
	uint64_t owner_pid;
	uint64_t time_base;
	uint64_t bucket_epoch;
	zend_long entry_count;
	zend_long used_mem;
	zval entry_keys;
} ucache_pool_status_snapshot;

static zend_always_inline const char *ucache_entry_key_ptr(const ucache_entry *entry)
{
	return (const char *) ucache_base() + ucache_entry_key_pos(entry);
}

static ucache_pool_status_obj *ucache_pool_status_from_this(zval *this_ptr)
{
	ucache_pool_status_obj *status =
		ucache_pool_status_obj_from_obj(Z_OBJ_P(this_ptr))
	;

	if (status->scope == NULL) {
		zend_throw_error(NULL, "%s instance was not initialized", ZSTR_VAL(Z_OBJCE_P(this_ptr)->name));

		return NULL;
	}

	return status;
}

static ucache_status_obj *ucache_status_from_this(zval *this_ptr)
{
	ucache_status_obj *status =
		ucache_status_obj_from_obj(Z_OBJ_P(this_ptr))
	;

	if (!status->initialized) {
		zend_throw_error(NULL, "%s instance was not initialized", ZSTR_VAL(Z_OBJCE_P(this_ptr)->name));

		return NULL;
	}

	return status;
}

static zend_long ucache_clamp_zend_long(uint64_t count)
{
	return count > (uint64_t) ZEND_LONG_MAX ? ZEND_LONG_MAX : (zend_long) count;
}

static zend_object *ucache_availability_enum_case(php_ucache_reason reason)
{
	zend_enum_UserCache_CacheAvailability case_id
		= ZEND_ENUM_UserCache_CacheAvailability_UnavailableByUnknownReason
	;

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
		case PHP_UCACHE_REASON_REQ_SHUTDOWN:
			case_id = ZEND_ENUM_UserCache_CacheAvailability_UnavailableByUnknownReason;

			break;
	}

	return zend_enum_get_case_by_id(ucache_availability_ce, case_id);
}

static void ucache_add_pool_mem(size_t *used_mem, size_t size)
{
	if (*used_mem >= (size_t) ZEND_LONG_MAX ||
		size > (size_t) ZEND_LONG_MAX - *used_mem
	) {
		*used_mem = (size_t) ZEND_LONG_MAX;

		return;
	}

	*used_mem += size;
}

static size_t ucache_payload_block_size(
		const ucache_hdr *hdr,
		uint32_t payload_offset)
{
	uint32_t block_size;

	if (!ucache_payload_in_bounds(hdr, payload_offset, 0)) {
		return 0;
	}

	block_size = ucache_block_size(ucache_block_ptr(ucache_payload_block_offset(payload_offset)));
	if (block_size < sizeof(ucache_block) ||
		!ucache_payload_in_bounds(hdr, payload_offset, block_size - sizeof(ucache_block))
	) {
		return 0;
	}

	return block_size;
}

static void ucache_account_pool_entry(
		const ucache_hdr *hdr,
		const ucache_entry *entry,
		zend_string *prefix,
		zend_long *entry_count,
		size_t *used_mem,
		zval *entry_keys)
{
	const char *key;
	uint32_t val_offset;
	size_t prefix_len;
	bool combined_val_key;

	combined_val_key = (entry->flags & UCACHE_ENTRY_FLAG_COMBINED_VAL_KEY) != 0;
	val_offset = ucache_entry_val_offset(entry);
	key = ucache_entry_key_ptr(entry);
	prefix_len = ZSTR_LEN(prefix);

	(*entry_count)++;

	add_next_index_stringl(entry_keys, key + prefix_len, entry->key_len - prefix_len);

	ucache_add_pool_mem(used_mem, sizeof(ucache_entry) + sizeof(ucache_pool_links));

	if (combined_val_key) {
		ucache_add_pool_mem(
			used_mem,
			ucache_payload_block_size(hdr, val_offset)
		);
	} else {
		ucache_add_pool_mem(
			used_mem,
			ucache_payload_block_size(hdr, entry->key_offset)
		);

		if (val_offset != 0) {
			ucache_add_pool_mem(
				used_mem,
				ucache_payload_block_size(hdr, val_offset)
			);
		}
	}
}

static void ucache_pool_status_snapshot_dtor(zval *val)
{
	ucache_pool_status_snapshot *snapshot = Z_PTR_P(val);

	if (snapshot != NULL) {
		zval_ptr_dtor_nogc(&snapshot->entry_keys);

		efree(snapshot);
	}
}

static ucache_pool_status_snapshot *ucache_pool_status_snapshot_get_or_create(zend_ulong handle)
{
	ucache_pool_status_snapshot *snapshot;
	HashTable *snapshots = UC_G(pool_status_snapshots);
	zval *entry, empty;

	if (snapshots == NULL) {
		ALLOC_HASHTABLE(snapshots);
		zend_hash_init(snapshots, 8, NULL, ucache_pool_status_snapshot_dtor, 0);

		UC_G(pool_status_snapshots) = snapshots;
	}

	entry = zend_hash_index_find(snapshots, handle);
	if (entry == NULL) {
		ZVAL_PTR(&empty, NULL);

		entry = zend_hash_index_add_new(snapshots, handle, &empty);
	}

	snapshot = Z_PTR_P(entry);
	if (snapshot == NULL) {
		snapshot = ecalloc(1, sizeof(*snapshot));

		ZVAL_UNDEF(&snapshot->entry_keys);
		ZVAL_PTR(entry, snapshot);
	}

	return snapshot;
}

void ucache_collect_info_stats(ucache_info_stats *stats)
{
	ucache_runtime *runtime;
	ucache_storage *storage;
	ucache_hdr *hdr;
	ucache_graph_pin_slot *pin_slot;
	uint32_t i;
	size_t free_mem = 0, wasted_mem = 0, tail_mem = 0;

	memset(stats, 0, sizeof(*stats));

	runtime = ucache_active_runtime();
	storage = &ucache_active_ctx()->storage;

	stats->configured_mem = ucache_clamp_zend_long(runtime->configured_mem);

	if (!runtime->available) {
		return;
	}

	stats->shared_mem_size = ucache_clamp_zend_long(storage->size);

	if (!ucache_storage_startup_is_complete(storage) || !ucache_rlock()) {
		return;
	}

	hdr = ucache_hdr_ptr();
	if (hdr == NULL || !ucache_hdr_adoptable_locked()) {
		stats->free_mem = stats->shared_mem_size;

		ucache_unlock();

		return;
	}

	if (hdr->next_free <= hdr->data_size) {
		tail_mem = hdr->data_size - hdr->next_free;
	}

	wasted_mem = hdr->free_list_bytes;
	free_mem = tail_mem + wasted_mem;

	stats->entry_count = (zend_long) hdr->count;
	stats->entry_capacity = (zend_long) hdr->capacity;
	stats->tombstone_count = (zend_long) hdr->tombstone_count;
	stats->expunge_count = ucache_clamp_zend_long(hdr->expunge_count);
	stats->store_failure_count = ucache_clamp_zend_long(hdr->store_failure_count);
	stats->eviction_count = ucache_clamp_zend_long(hdr->eviction_count);
	stats->dead_pin_owners_reclaimed = ucache_clamp_zend_long(hdr->graph_dead_pin_owners_reclaimed);
	stats->dead_pins_stripped = ucache_clamp_zend_long(hdr->graph_dead_pins_stripped);
	stats->committed_mem = ucache_clamp_zend_long(ucache_committed_bytes_locked(hdr));
	stats->commit_failure_count = ucache_clamp_zend_long(hdr->commit_failure_count);

	for (i = 0; i < ucache_graph_pin_slot_count(hdr); i++) {
		pin_slot = &hdr->graph_pin_slots[i];

		if (atomic_load(&pin_slot->owner_pid) != 0) {
			stats->graph_pin_slots_in_use++;
		}

		stats->graph_pinned_refs +=
			(zend_long) atomic_load(&pin_slot->pin_count)
		;
	}

	stats->free_mem = ucache_clamp_zend_long(free_mem);
	stats->wasted_mem = ucache_clamp_zend_long(wasted_mem);
	stats->used_mem = storage->size > free_mem
		? ucache_clamp_zend_long(storage->size - free_mem)
		: 0
	;

	ucache_unlock();
}

void ucache_release_pool_status_snapshots(void)
{
	HashTable *snapshots = UC_G(pool_status_snapshots);

	UC_G(pool_status_snapshots) = NULL;

	if (snapshots != NULL) {
		zend_hash_destroy(snapshots);

		FREE_HASHTABLE(snapshots);
	}
}

void ucache_collect_pool_status(
		ucache_obj *cache,
		zend_long *entry_count,
		zend_long *used_mem,
		zval *entry_keys)
{
	ucache_ctx *ctx = cache->ctx;
	zend_string *prefix = cache->scope_prefix;
	ucache_ctx *prev_ctx;
	ucache_storage *storage;
	ucache_hdr *hdr;
	ucache_entry *entries, *entry;
	ucache_pool_links *links;
	ucache_pool_status_snapshot *snapshot;
	zval stale_keys;
	uint64_t time_base, bucket_epoch;
	uint32_t bucket, idx;
	size_t used_size = 0;

	*entry_count = 0;
	*used_mem = 0;

	prev_ctx = ucache_activate_ctx(ctx);

	storage = &ctx->storage;
	if (!ucache_storage_startup_is_complete(storage) || !ucache_rlock()) {
		ucache_restore_ctx(prev_ctx);

		return;
	}

	hdr = ucache_hdr_ptr();
	if (hdr == NULL ||
		!ucache_hdr_adoptable_locked()
	) {
		ucache_unlock();
		ucache_restore_ctx(prev_ctx);

		return;
	}

	bucket = (uint32_t) (zend_string_hash_val(prefix) & (UCACHE_POOL_BUCKETS - 1));
	snapshot = UC_G(pool_status_snapshots) != NULL
		? zend_hash_index_find_ptr(UC_G(pool_status_snapshots), cache->std.handle)
		: NULL
	;
	if (snapshot != NULL &&
		snapshot->ctx == ctx &&
		snapshot->hdr == hdr &&
		snapshot->owner_pid == ucache_cached_pid() &&
		snapshot->time_base == hdr->time_base &&
		snapshot->bucket_epoch == hdr->pool_bucket_epochs[bucket]
	) {
		*entry_count = snapshot->entry_count;
		*used_mem = snapshot->used_mem;

		ZVAL_COPY(entry_keys, &snapshot->entry_keys);

		ucache_unlock();
		ucache_restore_ctx(prev_ctx);

		return;
	}

	entries = ucache_entries_ptr(hdr);
	links = ucache_pool_links_ptr(hdr);

	zend_try {
		array_init(entry_keys);

		for (idx = hdr->pool_bucket_heads[bucket]; idx != 0; idx = links[idx - 1].next) {
			ZEND_ASSERT(idx <= hdr->capacity);
			entry = &entries[idx - 1];
			if (ucache_entry_is_used(entry) &&
				ucache_bytes_in_bounds(hdr, ucache_entry_key_pos(entry), entry->key_len) &&
				entry->key_len >= ZSTR_LEN(prefix) &&
				memcmp(ucache_entry_key_ptr(entry), ZSTR_VAL(prefix), ZSTR_LEN(prefix)) == 0
			) {
				ucache_account_pool_entry(
					hdr,
					entry,
					prefix,
					entry_count,
					&used_size,
					entry_keys
				);
			}
		}
	} zend_catch {
		ucache_unlock_if_held();
		ucache_restore_ctx(prev_ctx);

		zend_bailout();
	} zend_end_try();

	time_base = hdr->time_base;
	bucket_epoch = hdr->pool_bucket_epochs[bucket];

	ucache_unlock();
	ucache_restore_ctx(prev_ctx);

	*used_mem = ucache_clamp_zend_long(used_size);

	if (UC_G(in_req_shutdown)) {
		return;
	}

	snapshot = ucache_pool_status_snapshot_get_or_create(cache->std.handle);

	ZVAL_COPY_VALUE(&stale_keys, &snapshot->entry_keys);
	ZVAL_COPY(&snapshot->entry_keys, entry_keys);

	snapshot->ctx = ctx;
	snapshot->hdr = hdr;
	snapshot->owner_pid = ucache_cached_pid();
	snapshot->time_base = time_base;
	snapshot->bucket_epoch = bucket_epoch;
	snapshot->entry_count = *entry_count;
	snapshot->used_mem = *used_mem;

	zval_ptr_dtor(&stale_keys);
}

void ucache_pool_status_obj_free(zend_object *obj)
{
	ucache_pool_status_obj *status =
		ucache_pool_status_obj_from_obj(obj)
	;

	zend_object_std_dtor(&status->std);

	if (status->scope != NULL) {
		zend_string_release(status->scope);
	}

	zval_ptr_dtor(&status->entry_keys);
}

zend_object *ucache_pool_status_obj_create(zend_class_entry *ce)
{
	ucache_pool_status_obj *status;

	status = zend_object_alloc(sizeof(ucache_pool_status_obj), ce);

	zend_object_std_init(&status->std, ce);
	object_properties_init(&status->std, ce);

	status->scope = NULL;
	status->entry_count = 0;
	status->used_mem = 0;

	ZVAL_UNDEF(&status->entry_keys);

	status->std.handlers = &ucache_pool_status_obj_handlers;

	return &status->std;
}

zend_object *ucache_status_obj_create(zend_class_entry *ce)
{
	ucache_status_obj *status;

	status = zend_object_alloc(sizeof(ucache_status_obj), ce);

	zend_object_std_init(&status->std, ce);
	object_properties_init(&status->std, ce);

	memset(&status->stats, 0, sizeof(status->stats));

	status->availability_reason = PHP_UCACHE_REASON_NONE;
	status->initialized = false;
	status->std.handlers = &ucache_status_obj_handlers;

	return &status->std;
}

ZEND_METHOD(UserCache_CacheStatus, __construct)
{
}

ZEND_METHOD(UserCache_CacheStatus, getAvailability)
{
	ucache_status_obj *status;
	zend_object *availability;

	ZEND_PARSE_PARAMETERS_NONE();

	status = ucache_status_from_this(ZEND_THIS);
	if (status == NULL) {
		RETURN_THROWS();
	}

	availability = ucache_availability_enum_case(
		status->availability_reason
	);

	RETURN_OBJ_COPY(availability);
}

UCACHE_DEFINE_STATUS_LONG_GETTER(getConfiguredMemory, configured_mem)

UCACHE_DEFINE_STATUS_LONG_GETTER(getSharedMemorySize, shared_mem_size)

UCACHE_DEFINE_STATUS_LONG_GETTER(getUsedMemory, used_mem)

UCACHE_DEFINE_STATUS_LONG_GETTER(getFreeMemory, free_mem)

UCACHE_DEFINE_STATUS_LONG_GETTER(getWastedMemory, wasted_mem)

UCACHE_DEFINE_STATUS_LONG_GETTER(getEntryCount, entry_count)

UCACHE_DEFINE_STATUS_LONG_GETTER(getEntryCapacity, entry_capacity)

UCACHE_DEFINE_STATUS_LONG_GETTER(getTombstoneCount, tombstone_count)

UCACHE_DEFINE_STATUS_LONG_GETTER(getExpungeCount, expunge_count)

UCACHE_DEFINE_STATUS_LONG_GETTER(getEvictionCount, eviction_count)

UCACHE_DEFINE_STATUS_LONG_GETTER(getStoreFailureCount, store_failure_count)

UCACHE_DEFINE_STATUS_LONG_GETTER(getGraphPinSlotsInUse, graph_pin_slots_in_use)

UCACHE_DEFINE_STATUS_LONG_GETTER(getGraphPinnedReferences, graph_pinned_refs)

UCACHE_DEFINE_STATUS_LONG_GETTER(getDeadPinOwnersReclaimed, dead_pin_owners_reclaimed)

UCACHE_DEFINE_STATUS_LONG_GETTER(getDeadPinsStripped, dead_pins_stripped)

UCACHE_DEFINE_STATUS_LONG_GETTER(getCommittedMemory, committed_mem)

UCACHE_DEFINE_STATUS_LONG_GETTER(getCommitFailureCount, commit_failure_count)

ZEND_METHOD(UserCache_CachePoolStatus, __construct)
{
}

ZEND_METHOD(UserCache_CachePoolStatus, getPoolName)
{
	ucache_pool_status_obj *status;

	ZEND_PARSE_PARAMETERS_NONE();

	status = ucache_pool_status_from_this(ZEND_THIS);
	if (status == NULL) {
		RETURN_THROWS();
	}

	RETURN_STR_COPY(status->scope);
}

ZEND_METHOD(UserCache_CachePoolStatus, getEntryCount)
{
	ucache_pool_status_obj *status;

	ZEND_PARSE_PARAMETERS_NONE();

	status = ucache_pool_status_from_this(ZEND_THIS);
	if (status == NULL) {
		RETURN_THROWS();
	}

	RETURN_LONG(status->entry_count);
}

ZEND_METHOD(UserCache_CachePoolStatus, getEntryKeys)
{
	ucache_pool_status_obj *status;

	ZEND_PARSE_PARAMETERS_NONE();

	status = ucache_pool_status_from_this(ZEND_THIS);
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
	ucache_pool_status_obj *status;

	ZEND_PARSE_PARAMETERS_NONE();

	status = ucache_pool_status_from_this(ZEND_THIS);
	if (status == NULL) {
		RETURN_THROWS();
	}

	RETURN_LONG(status->used_mem);
}
