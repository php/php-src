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

#ifndef ZEND_STATIC_CACHE_INTERNAL_H
#define ZEND_STATIC_CACHE_INTERNAL_H

#include "php.h"

#include <time.h>
#ifdef ZTS
# include "TSRM/TSRM.h"
#endif

#include "Zend/zend_attributes.h"
#include "Zend/zend_ast.h"
#include "Zend/zend_atomic.h"
#include "Zend/zend_enum.h"
#include "Zend/zend_exceptions.h"

#include "ZendAccelerator.h"
#include "zend_accelerator_module.h"
#include "zend_shared_alloc.h"
#include "zend_smart_str.h"
#include "zend_static_cache.h"

#include "ext/date/php_date.h"
#include "ext/spl/spl_array.h"
#include "ext/spl/spl_fixedarray.h"
#include "ext/standard/php_var.h"

#include "SAPI.h"

#define ZEND_OPCACHE_STATIC_CACHE_MAGIC 0xCAC17E01U
#define ZEND_OPCACHE_STATIC_CACHE_VERSION 2U
#define ZEND_OPCACHE_STATIC_CACHE_MIN_CAPACITY 127U
#define ZEND_OPCACHE_STATIC_CACHE_MAX_CAPACITY 65521U
#define ZEND_OPCACHE_STATIC_CACHE_SLOT_BYTES 256U

#define ZEND_OPCACHE_STATIC_CACHE_ENTRY_EMPTY 0
#define ZEND_OPCACHE_STATIC_CACHE_ENTRY_USED 1
#define ZEND_OPCACHE_STATIC_CACHE_ENTRY_TOMBSTONE 2

#define ZEND_OPCACHE_STATIC_CACHE_VALUE_NULL 0
#define ZEND_OPCACHE_STATIC_CACHE_VALUE_TRUE 1
#define ZEND_OPCACHE_STATIC_CACHE_VALUE_FALSE 2
#define ZEND_OPCACHE_STATIC_CACHE_VALUE_LONG 3
#define ZEND_OPCACHE_STATIC_CACHE_VALUE_DOUBLE 4
#define ZEND_OPCACHE_STATIC_CACHE_VALUE_STRING 5
#define ZEND_OPCACHE_STATIC_CACHE_VALUE_SERIALIZED 6
#define ZEND_OPCACHE_STATIC_CACHE_VALUE_OPCACHE_SERIALIZED 7
#define ZEND_OPCACHE_STATIC_CACHE_VALUE_SHARED_GRAPH 8

#define ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_MAGIC 0xCAC17E02U
#define ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_VERSION 1U
#define ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_RETIRED (1 << 30)
#define ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_REFCOUNT_MASK (ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_RETIRED - 1)

#define ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_VALUE_UNDEF 0
#define ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_VALUE_NULL 1
#define ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_VALUE_TRUE 2
#define ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_VALUE_FALSE 3
#define ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_VALUE_LONG 4
#define ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_VALUE_DOUBLE 5
#define ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_VALUE_STRING 6
#define ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_VALUE_ARRAY 7
#define ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_VALUE_OBJECT 8
#define ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_VALUE_SERIALIZED_OBJECT 9
#define ZEND_OPCACHE_STATIC_CACHE_SHARED_GRAPH_VALUE_DYNAMIC_ARRAY 10

#define ZEND_OPCACHE_STATIC_CACHE_LOOKUP_BUCKETS 256U
#define ZEND_OPCACHE_STATIC_CACHE_LOOKUP_WAYS 2U
#define ZEND_OPCACHE_STATIC_CACHE_LOOKUP_SETS (ZEND_OPCACHE_STATIC_CACHE_LOOKUP_BUCKETS / ZEND_OPCACHE_STATIC_CACHE_LOOKUP_WAYS)
#define ZEND_OPCACHE_STATIC_CACHE_LOOKUP_INVALID_SLOT UINT32_MAX

#define ZEND_OPCACHE_STATIC_CACHE_ENTRY_LOCK_STRIPES 256U
#define ZEND_OPCACHE_STATIC_CACHE_LOW_MEMORY_COMPACT_THRESHOLD (3U * 1024U * 1024U)

#define ZEND_OPCACHE_STATIC_CACHE_LOOKUP_EMPTY 0
#define ZEND_OPCACHE_STATIC_CACHE_LOOKUP_HIT 1
#define ZEND_OPCACHE_STATIC_CACHE_LOOKUP_MISS 2

#define ZEND_OPCACHE_STATIC_CACHE_ENTRY_RESERVED_COMBINED_VALUE_KEY 0x0001U

#define ZEND_OPCACHE_STATIC_CACHE_PINNED_ATTRIBUTE "opcache\\pinnedstatic"
#define ZEND_OPCACHE_STATIC_CACHE_VOLATILE_STATIC_ATTRIBUTE "opcache\\volatilestatic"

#define ZEND_OPCACHE_STATIC_CACHE_STRATEGY_IMMEDIATE 0
#define ZEND_OPCACHE_STATIC_CACHE_STRATEGY_TRACKING 1

#define ZEND_OPCACHE_STATIC_CACHE_BLOCK_FREE 1U

#ifndef ZEND_WIN32
# define ZEND_OPCACHE_STATIC_CACHE_VOLATILE_SEM_FILENAME_PREFIX ".ZendVolatileCacheSem."
# define ZEND_OPCACHE_STATIC_CACHE_PINNED_SEM_FILENAME_PREFIX ".ZendPinnedCacheSem."
#endif

typedef enum _zend_opcache_static_cache_kind {
	ZEND_OPCACHE_STATIC_CACHE_NONE,
	ZEND_OPCACHE_STATIC_CACHE_PINNED,
	ZEND_OPCACHE_STATIC_CACHE_VOLATILE_STATIC,
	ZEND_OPCACHE_STATIC_CACHE_VOLATILE_STATIC_TRACKING,
	ZEND_OPCACHE_STATIC_CACHE_CONFLICT
} zend_opcache_static_cache_kind;

typedef struct _zend_opcache_static_cache_volatile_static_attribute_config {
	zend_long ttl;
	zend_long strategy;
} zend_opcache_static_cache_volatile_static_attribute_config;

typedef struct _zend_opcache_static_cache_runtime {
	bool enabled;
	bool available;
	bool startup_failed;
	bool backend_initialized;
	size_t configured_memory;
	const char *failure_reason;
} zend_opcache_static_cache_runtime;

typedef struct _zend_opcache_static_cache_storage {
	const zend_shared_memory_handlers *handler;
	zend_shared_segment **segments;
	int segment_count;
	size_t size;
	const char *model;
	bool initialized;
	bool initialized_before_request;
	bool lock_initialized;
	int lock_file;
	char lockfile_name[MAXPATHLEN];
#ifdef ZTS
	zend_thread_rwlock_t zts_lock;
	MUTEX_T entry_locks[ZEND_OPCACHE_STATIC_CACHE_ENTRY_LOCK_STRIPES];
	bool entry_locks_initialized;
#endif
} zend_opcache_static_cache_storage;

typedef struct _zend_opcache_static_cache_context {
	zend_opcache_static_cache_runtime runtime;
	zend_opcache_static_cache_storage storage;
	const char *name;
	const char *lock_name;
#ifndef ZEND_WIN32
	const char *sem_filename_prefix;
#endif
	bool clear_on_pressure;
	bool strict_store_failure;
} zend_opcache_static_cache_context;

typedef struct _zend_opcache_static_cache_entry_lock_lease {
	uint64_t expires_at;
} zend_opcache_static_cache_entry_lock_lease;

/* The same storage primitives serve the explicit volatile cache, VolatileStatic, and
 * PinnedStatic. Callers switch this TLS context around short critical sections
 * so allocator, lookup-cache, and lock helpers always operate on the right SHM
 * segment without threading a context argument through every hot helper. */
typedef struct _zend_opcache_static_cache_header {
	uint32_t magic;
	uint32_t version;
	uint32_t capacity;
	uint32_t count;
	uint32_t data_offset;
	uint32_t data_size;
	uint32_t next_free;
	uint32_t free_list;
	uint32_t last_block_offset;
	uint64_t mutation_epoch;
	zend_opcache_static_cache_entry_lock_lease entry_lock_leases[ZEND_OPCACHE_STATIC_CACHE_ENTRY_LOCK_STRIPES];
#ifndef ZEND_WIN32
	uint32_t reserved_lock;
#endif
} zend_opcache_static_cache_header;

typedef struct _zend_opcache_static_cache_block {
	uint32_t size;
	uint32_t prev_size;
	uint32_t next_free;
	uint32_t prev_free;
	uint32_t flags;
} zend_opcache_static_cache_block;

typedef struct _zend_opcache_static_cache_entry {
	zend_ulong hash;
	uint32_t key_offset;
	uint32_t key_len;
	uint32_t value_offset;
	uint32_t value_len;
	uint64_t expires_at;
	uint8_t state;
	uint8_t value_type;
	uint16_t reserved;
	zend_long long_value;
	double double_value;
} zend_opcache_static_cache_entry;

typedef struct _zend_opcache_static_cache_lookup_entry {
	zend_ulong hash;
	uint64_t mutation_epoch;
	uint32_t slot_index;
	uint8_t state;
	uint8_t reserved[3];
} zend_opcache_static_cache_lookup_entry;

typedef struct _zend_opcache_static_cache_request_local_slot {
	uint64_t mutation_epoch;
	zval value;
} zend_opcache_static_cache_request_local_slot;

typedef struct _zend_opcache_static_cache_prepared_value {
	zend_ulong hash;
	uint8_t value_type;
	uint8_t reserved[3];
	uint32_t value_len;
	size_t payload_size;
	size_t payload_used_size;
	const unsigned char *payload_source;
	unsigned char *owned_buffer;
	zend_string *owned_string;
	zend_long long_value;
	double double_value;
} zend_opcache_static_cache_prepared_value;

typedef struct _zend_opcache_static_cache_class_blob_handle {
	zend_class_entry *ce;
	zend_string *cache_key;
	zend_opcache_static_cache_context *context;
	zend_opcache_static_cache_kind kind;
	zend_long ttl;
	zval root_state;
	bool initialized;
	bool dirty;
	bool request_deferred_publish;
} zend_opcache_static_cache_class_blob_handle;

typedef struct _zend_opcache_static_cache_function_static_entry {
	zval *slot;
	zend_string *cache_key;
	zend_opcache_static_cache_context *context;
	zend_opcache_static_cache_kind kind;
	zend_long ttl;
	zend_opcache_static_cache_class_blob_handle *class_blob_handle;
	zend_string *method_key;
	zend_string *var_name;
} zend_opcache_static_cache_function_static_entry;

typedef struct _zend_opcache_static_cache_static_slot_handle {
	zval *slot;
	zend_string *cache_key;
	zend_opcache_static_cache_context *context;
	zend_opcache_static_cache_kind kind;
	zend_long ttl;
	zend_opcache_static_cache_class_blob_handle *class_blob_handle;
	zval original_value;
	zend_refcounted *original_root;
	uint8_t original_root_type;
	bool has_original_value;
	bool snapshot_root_published;
	bool request_deferred_publish;
	bool mutation_dirty;
} zend_opcache_static_cache_static_slot_handle;

typedef struct _zend_opcache_static_cache_tracked_dependency {
	zend_opcache_static_cache_static_slot_handle *single_handle;
	HashTable *handles;
} zend_opcache_static_cache_tracked_dependency;

typedef struct _zend_opcache_static_cache_pending_mutation {
	zend_opcache_static_cache_static_slot_handle *handle;
	uint32_t depth;
	bool dirty;
	bool root_array;
} zend_opcache_static_cache_pending_mutation;

typedef struct _zend_opcache_static_cache_shared_graph_header {
	uint32_t magic;
	uint32_t version;
	uint32_t root_offset;
	uint32_t root_type;
	zend_atomic_int ref_state;
} zend_opcache_static_cache_shared_graph_header;

typedef struct _zend_opcache_static_cache_shared_graph_ref {
	zend_opcache_static_cache_context *context;
	uint32_t payload_offset;
} zend_opcache_static_cache_shared_graph_ref;

typedef struct _zend_opcache_static_cache_entry_lock {
	zend_opcache_static_cache_context *context;
	zend_ulong owner_pid;
	zend_long lease;
	uint32_t stripe;
} zend_opcache_static_cache_entry_lock;

typedef struct _zend_opcache_static_cache_shared_graph_value {
	uint8_t type;
	uint8_t reserved[7];
	union {
		zend_long long_value;
		double double_value;
		uint64_t offset;
	} payload;
} zend_opcache_static_cache_shared_graph_value;

typedef struct _zend_opcache_static_cache_shared_graph_property {
	uint32_t name_offset;
	uint32_t reserved;
	zend_opcache_static_cache_shared_graph_value value;
} zend_opcache_static_cache_shared_graph_property;

typedef struct _zend_opcache_static_cache_shared_graph_array_element {
	zend_ulong h;
	uint32_t key_offset;
	uint32_t reserved;
	zend_opcache_static_cache_shared_graph_value value;
} zend_opcache_static_cache_shared_graph_array_element;

typedef struct _zend_opcache_static_cache_shared_graph_array {
	uint32_t count;
	uint32_t next_free;
	uint32_t elements_offset;
	uint32_t reserved;
} zend_opcache_static_cache_shared_graph_array;

typedef struct _zend_opcache_static_cache_shared_graph_object {
	uint32_t class_name_offset;
	uint32_t property_count;
	uint32_t properties_offset;
	uint32_t reserved;
} zend_opcache_static_cache_shared_graph_object;

typedef struct _zend_opcache_static_cache_shared_graph_serialized_object {
	uint32_t data_len;
	uint32_t reserved;
	unsigned char data[1];
} zend_opcache_static_cache_shared_graph_serialized_object;

typedef struct _zend_opcache_static_cache_shared_graph_calc_context {
	size_t size;
	HashTable seen_arrays;
	HashTable seen_objects;
} zend_opcache_static_cache_shared_graph_calc_context;

typedef struct _zend_opcache_static_cache_shared_graph_copy_context {
	unsigned char *buffer;
	size_t size;
	size_t position;
	HashTable seen_arrays;
	HashTable seen_objects;
} zend_opcache_static_cache_shared_graph_copy_context;

extern zend_class_entry *zend_opcache_static_cache_exception_ce;
extern zend_class_entry *zend_opcache_static_cache_strategy_ce;
extern zend_class_entry *zend_opcache_static_cache_info_ce;
extern zend_opcache_static_cache_context zend_opcache_static_cache_volatile_context_state;
extern zend_opcache_static_cache_context zend_opcache_static_cache_pinned_context_state;
extern bool zend_opcache_static_cache_subsystem_disabled;
extern const char *zend_opcache_static_cache_subsystem_failure_reason;
extern ZEND_EXT_TLS zend_opcache_static_cache_runtime zend_opcache_static_cache_volatile_runtime_state;
extern ZEND_EXT_TLS zend_opcache_static_cache_runtime zend_opcache_static_cache_pinned_runtime_state;
extern ZEND_EXT_TLS HashTable zend_opcache_static_cache_attribute_classes;
extern ZEND_EXT_TLS bool zend_opcache_static_cache_attribute_classes_initialized;
extern ZEND_EXT_TLS HashTable zend_opcache_static_cache_ignored_classes;
extern ZEND_EXT_TLS bool zend_opcache_static_cache_ignored_classes_initialized;
extern ZEND_EXT_TLS HashTable zend_opcache_static_cache_function_statics;
extern ZEND_EXT_TLS bool zend_opcache_static_cache_function_statics_initialized;
extern ZEND_EXT_TLS HashTable zend_opcache_static_cache_class_blob_handles;
extern ZEND_EXT_TLS bool zend_opcache_static_cache_class_blob_handles_initialized;
extern bool zend_opcache_static_cache_safe_direct_classes_marked;
extern ZEND_EXT_TLS zend_opcache_static_cache_context *zend_opcache_static_cache_active_context_ptr;
#ifdef ZTS
extern ZEND_EXT_TLS bool zend_opcache_static_cache_zts_lock_is_write;
#endif
extern ZEND_EXT_TLS HashTable *zend_opcache_static_cache_tracked_roots;
extern ZEND_EXT_TLS HashTable *zend_opcache_static_cache_tracked_references;
extern ZEND_EXT_TLS HashTable *zend_opcache_static_cache_tracked_arrays;
extern ZEND_EXT_TLS HashTable *zend_opcache_static_cache_tracked_objects;
extern ZEND_EXT_TLS bool zend_opcache_static_cache_has_tracked_arrays;
extern ZEND_EXT_TLS bool zend_opcache_static_cache_has_tracked_objects;
extern ZEND_EXT_TLS bool zend_opcache_static_cache_volatile_skip_publish;
extern ZEND_EXT_TLS bool zend_opcache_static_cache_pinned_skip_publish;
extern ZEND_EXT_TLS zend_opcache_static_cache_shared_graph_ref *zend_opcache_static_cache_shared_graph_refs;
extern ZEND_EXT_TLS uint32_t zend_opcache_static_cache_shared_graph_ref_count;
extern ZEND_EXT_TLS uint32_t zend_opcache_static_cache_shared_graph_ref_capacity;
extern ZEND_EXT_TLS zend_opcache_static_cache_shared_graph_ref *zend_opcache_static_cache_retired_shared_graphs;
extern ZEND_EXT_TLS uint32_t zend_opcache_static_cache_retired_shared_graph_count;
extern ZEND_EXT_TLS uint32_t zend_opcache_static_cache_retired_shared_graph_capacity;
extern ZEND_EXT_TLS HashTable *zend_opcache_static_cache_last_array_ht;
extern ZEND_EXT_TLS zend_opcache_static_cache_tracked_dependency *zend_opcache_static_cache_last_array_dependency;
extern ZEND_EXT_TLS zend_object *zend_opcache_static_cache_last_object_obj;
extern ZEND_EXT_TLS zend_opcache_static_cache_tracked_dependency *zend_opcache_static_cache_last_object_dependency;
extern ZEND_EXT_TLS zend_opcache_static_cache_pending_mutation zend_opcache_static_cache_pending_mutation_state;
extern ZEND_EXT_TLS bool zend_opcache_static_cache_capture_active;
extern ZEND_EXT_TLS bool zend_opcache_static_cache_capture_available;
extern ZEND_EXT_TLS zend_opcache_static_cache_static_slot_handle *zend_opcache_static_cache_capture_handle;
extern ZEND_EXT_TLS HashTable *zend_opcache_static_cache_capture_arrays;
extern ZEND_EXT_TLS HashTable *zend_opcache_static_cache_capture_objects;
extern ZEND_EXT_TLS zend_opcache_static_cache_lookup_entry zend_opcache_static_cache_volatile_lookup_entry_storage[ZEND_OPCACHE_STATIC_CACHE_LOOKUP_BUCKETS];
extern ZEND_EXT_TLS zend_opcache_static_cache_lookup_entry zend_opcache_static_cache_pinned_lookup_entry_storage[ZEND_OPCACHE_STATIC_CACHE_LOOKUP_BUCKETS];
extern ZEND_EXT_TLS HashTable *zend_opcache_static_cache_volatile_request_local_slots;
extern ZEND_EXT_TLS HashTable *zend_opcache_static_cache_pinned_request_local_slots;
extern ZEND_EXT_TLS HashTable *zend_opcache_static_cache_volatile_entry_locks;
extern ZEND_EXT_TLS HashTable *zend_opcache_static_cache_pinned_entry_locks;
extern ZEND_EXT_TLS uint32_t zend_opcache_static_cache_volatile_entry_lock_counts[ZEND_OPCACHE_STATIC_CACHE_ENTRY_LOCK_STRIPES];
extern ZEND_EXT_TLS uint32_t zend_opcache_static_cache_pinned_entry_lock_counts[ZEND_OPCACHE_STATIC_CACHE_ENTRY_LOCK_STRIPES];

void zend_opcache_static_cache_reset_runtime(void);
void zend_opcache_static_cache_reset_storage(void);
bool zend_opcache_static_cache_header_init_locked(void);
void zend_opcache_static_cache_free_locked(uint32_t payload_offset);
uint32_t zend_opcache_static_cache_alloc_locked(size_t size, const void *source);
bool zend_opcache_static_cache_compact_to_fit_locked(size_t size);
bool zend_opcache_static_cache_startup_storage_before_request(void);
void zend_opcache_static_cache_shutdown_storage(void);
void zend_opcache_static_cache_ensure_ready(void);
void zend_opcache_static_cache_populate_info(zval *return_value);
bool zend_opcache_static_cache_rlock(void);
bool zend_opcache_static_cache_wlock(void);
void zend_opcache_static_cache_unlock(void);
bool zend_opcache_static_cache_acquire_entry_lock(zend_string *key);
bool zend_opcache_static_cache_try_acquire_entry_lock(zend_string *key, zend_long lease);
bool zend_opcache_static_cache_has_entry_lock(zend_string *key);
bool zend_opcache_static_cache_release_entry_lock(zend_string *key);
bool zend_opcache_static_cache_has_all_entry_locks(void);
void zend_opcache_static_cache_release_active_entry_locks(void);
void zend_opcache_static_cache_release_request_entry_locks(void);
void zend_opcache_static_cache_safe_direct_handlers_init(void);
void zend_opcache_static_cache_safe_direct_handlers_destroy(void);
void zend_opcache_static_cache_safe_direct_register_class(
	zend_class_entry *ce,
	const zend_opcache_static_cache_safe_direct_handlers *handlers);
zend_opcache_static_cache_safe_direct_state_copy_func_t zend_opcache_static_cache_safe_direct_copy_func(
	zend_class_entry *ce,
	zend_class_entry **base_ce_ptr);
zend_opcache_static_cache_safe_direct_state_has_unstorable_func_t zend_opcache_static_cache_safe_direct_state_has_unstorable_func(
	zend_class_entry *ce);
zend_opcache_static_cache_safe_direct_state_serialize_func_t zend_opcache_static_cache_safe_direct_state_serialize_func(
	zend_class_entry *ce);
zend_opcache_static_cache_safe_direct_state_unserialize_func_t zend_opcache_static_cache_safe_direct_state_unserialize_func(
	zend_class_entry *ce);
bool zend_opcache_static_cache_safe_direct_allows_custom_serializers(zend_class_entry *ce);
bool zend_opcache_static_cache_safe_direct_state_includes_properties(zend_class_entry *ce);
bool zend_opcache_static_cache_calculate_shared_graph_size(const zval *value, size_t *buffer_len);
bool zend_opcache_static_cache_build_shared_graph_in_place(const zval *value, unsigned char *buffer, size_t buffer_len, size_t *graph_len);
bool zend_opcache_static_cache_shared_graph_copy_fits_buffer(
		const unsigned char *source_buffer,
		size_t source_buffer_len,
		size_t source_graph_len,
		const unsigned char *target_buffer,
		size_t target_buffer_len);
bool zend_opcache_static_cache_fetch_shared_graph(const unsigned char *buffer, size_t buffer_len, zval *destination);
bool zend_opcache_static_cache_shared_graph_can_overwrite_payload_locked(uint32_t payload_offset);
bool zend_opcache_static_cache_shared_graph_can_move_payload_locked(uint32_t payload_offset);
bool zend_opcache_static_cache_shared_graph_rebase_moved_payload_locked(uint32_t payload_offset, ptrdiff_t delta);
bool zend_opcache_static_cache_shared_graph_acquire_locked(uint32_t payload_offset);
bool zend_opcache_static_cache_shared_graph_retire_payload_locked(uint32_t payload_offset);
bool zend_opcache_static_cache_shared_graph_release_ref_locked(uint32_t payload_offset);
bool zend_opcache_static_cache_has_request_shared_graph_ref(uint32_t payload_offset);
void zend_opcache_static_cache_register_shared_graph_ref(uint32_t payload_offset);
void zend_opcache_static_cache_defer_retired_shared_graph_free(uint32_t payload_offset);
bool zend_opcache_static_cache_release_request_shared_graph_refs(void);
bool zend_opcache_static_cache_clear_locked(void);
bool zend_opcache_static_cache_prepare_value(
		zend_opcache_static_cache_prepared_value *prepared,
		zend_string *key,
		zval *value,
		bool throw_on_failure,
		bool lock_held);
void zend_opcache_static_cache_destroy_prepared_value(zend_opcache_static_cache_prepared_value *prepared);
bool zend_opcache_static_cache_store_prepared_locked(
		zend_string *key,
		zval *value,
		const zend_opcache_static_cache_prepared_value *prepared,
		zend_long ttl,
		bool throw_on_failure);
bool zend_opcache_static_cache_store_locked(zend_string *key, zval *value, zend_long ttl, bool throw_on_failure);
bool zend_opcache_static_cache_fetch_locked(zend_string *key, zval *return_value, bool throw_if_missing, bool *found_ptr, bool use_request_local_slot);
bool zend_opcache_static_cache_exists_locked(zend_string *key);
bool zend_opcache_static_cache_delete_locked(zend_string *key);
bool zend_opcache_static_cache_atomic_update_locked(zend_string *key, zend_long step, bool decrement, bool insert_if_missing, zend_long *new_value, const char *type_error_message);
void zend_opcache_static_cache_release_request_local_slots(void);
void zend_opcache_static_cache_update_mutation_hook_state(void);
bool zend_opcache_static_cache_prepare_memo_fetch(zval *value, zend_opcache_static_cache_prepared_value *prepared);
void zend_opcache_static_cache_prepare_memo_store(zval *value, zend_opcache_static_cache_prepared_value *prepared);
bool zend_opcache_static_cache_class_has_keys(zend_class_entry *ce);
void zend_opcache_static_cache_delete_class_keys_locked(zend_class_entry *ce);
void zend_opcache_static_cache_delete_script_keys_locked(zend_persistent_script *persistent_script);
void zend_opcache_static_cache_register_hooks(void);
void zend_opcache_static_cache_unregister_hooks(void);
void zend_opcache_static_cache_request_init(void);
void zend_opcache_static_cache_capture_begin_for_handle(zend_opcache_static_cache_static_slot_handle *handle);
void zend_opcache_static_cache_capture_discard(void);
void zend_opcache_static_cache_capture_decoded_reachable_value(zval *value);
void zend_opcache_static_cache_capture_decoded_value(zval *value);
void zend_opcache_static_cache_request_shutdown(void);

static zend_always_inline zend_opcache_static_cache_context *zend_opcache_static_cache_active_context(void)
{
	return zend_opcache_static_cache_active_context_ptr != NULL
		? zend_opcache_static_cache_active_context_ptr
		: &zend_opcache_static_cache_volatile_context_state
	;
}

static zend_always_inline void *zend_opcache_static_cache_base(void)
{
	zend_opcache_static_cache_storage *storage = &zend_opcache_static_cache_active_context()->storage;

	if (!storage->initialized || storage->segment_count != 1) {
		return NULL;
	}

	return storage->segments[0]->p;
}

static zend_always_inline zend_opcache_static_cache_header *zend_opcache_static_cache_header_ptr(void)
{
	return (zend_opcache_static_cache_header *) zend_opcache_static_cache_base();
}

static zend_always_inline char *zend_opcache_static_cache_ptr(uint32_t offset)
{
	return (char *) zend_opcache_static_cache_base() + offset;
}

static zend_always_inline zend_opcache_static_cache_block *zend_opcache_static_cache_block_ptr(uint32_t offset)
{
	return (zend_opcache_static_cache_block *) zend_opcache_static_cache_ptr(offset);
}

static zend_always_inline uint32_t zend_opcache_static_cache_block_payload_capacity(uint32_t payload_offset)
{
	zend_opcache_static_cache_block *block;

	if (payload_offset < sizeof(zend_opcache_static_cache_block)) {
		return 0;
	}

	block = zend_opcache_static_cache_block_ptr(payload_offset - sizeof(zend_opcache_static_cache_block));
	if (block->size < sizeof(zend_opcache_static_cache_block)) {
		return 0;
	}

	return block->size - (uint32_t) sizeof(zend_opcache_static_cache_block);
}

static zend_always_inline uint32_t zend_opcache_static_cache_write_payload_locked(uint32_t reusable_offset, size_t size, const void *source)
{
	if (reusable_offset != 0 && zend_opcache_static_cache_block_payload_capacity(reusable_offset) >= size) {
		memcpy(zend_opcache_static_cache_ptr(reusable_offset), source, size);

		return reusable_offset;
	}

	return zend_opcache_static_cache_alloc_locked(size, source);
}

static zend_always_inline bool zend_opcache_static_cache_strategy_is_valid(zend_long strategy)
{
	return strategy == ZEND_OPCACHE_STATIC_CACHE_STRATEGY_IMMEDIATE ||
		strategy == ZEND_OPCACHE_STATIC_CACHE_STRATEGY_TRACKING
	;
}

static zend_always_inline bool zend_opcache_static_cache_strategy_from_zval(zval *value, zend_long *strategy)
{
	zval *backing_value;

	if (value == NULL ||
		Z_TYPE_P(value) != IS_OBJECT ||
		Z_OBJCE_P(value) != zend_opcache_static_cache_strategy_ce
	) {
		return false;
	}

	backing_value = zend_enum_fetch_case_value(Z_OBJ_P(value));
	if (Z_TYPE_P(backing_value) != IS_LONG) {
		return false;
	}

	*strategy = Z_LVAL_P(backing_value);

	return zend_opcache_static_cache_strategy_is_valid(*strategy);
}

static zend_always_inline bool zend_opcache_static_cache_strategy_from_attribute_zval(zval *value, zend_long *strategy)
{
	zend_ast *ast, *class_ast, *case_ast;
	zend_string *class_name, *case_name;

	if (zend_opcache_static_cache_strategy_from_zval(value, strategy)) {
		return true;
	}

	if (value == NULL || Z_TYPE_P(value) != IS_CONSTANT_AST) {
		return false;
	}

	ast = Z_ASTVAL_P(value);
	if (ast->kind != ZEND_AST_CLASS_CONST) {
		return false;
	}

	class_ast = ast->child[0];
	case_ast = ast->child[1];
	if (class_ast == NULL ||
		case_ast == NULL ||
		class_ast->kind != ZEND_AST_ZVAL ||
		case_ast->kind != ZEND_AST_ZVAL ||
		Z_TYPE_P(zend_ast_get_zval(class_ast)) != IS_STRING ||
		Z_TYPE_P(zend_ast_get_zval(case_ast)) != IS_STRING
	) {
		return false;
	}

	class_name = zend_ast_get_str(class_ast);
	if (!zend_string_equals_literal_ci(class_name, "OPcache\\CacheStrategy")) {
		return false;
	}

	case_name = zend_ast_get_str(case_ast);
	if (zend_string_equals_literal(case_name, "Immediate")) {
		*strategy = ZEND_OPCACHE_STATIC_CACHE_STRATEGY_IMMEDIATE;

		return true;
	}

	if (zend_string_equals_literal(case_name, "Tracking")) {
		*strategy = ZEND_OPCACHE_STATIC_CACHE_STRATEGY_TRACKING;

		return true;
	}

	return false;
}

static zend_always_inline void zend_opcache_static_cache_volatile_static_attribute_config_init(
		zend_opcache_static_cache_volatile_static_attribute_config *config)
{
	config->ttl = 0;
	config->strategy = ZEND_OPCACHE_STATIC_CACHE_STRATEGY_IMMEDIATE;
}

static zend_always_inline bool zend_opcache_static_cache_volatile_static_attribute_parse_ttl(
		zend_attribute *attr,
		uint32_t arg_num,
		zend_class_entry *scope,
		zend_long *ttl,
		zend_string **error)
{
	zval ttl_zv;

	ZVAL_COPY_OR_DUP(&ttl_zv, &attr->args[arg_num].value);
	if (Z_TYPE(ttl_zv) != IS_LONG) {
		zval_ptr_dtor(&ttl_zv);
		if (zend_get_attribute_value(&ttl_zv, attr, arg_num, scope) == FAILURE) {
			*error = ZSTR_INIT_LITERAL("OPcache\\VolatileStatic ttl must be resolvable", 0);

			return false;
		}
	}

	if (Z_TYPE(ttl_zv) != IS_LONG) {
		*error = zend_strpprintf(0,
			"OPcache\\VolatileStatic ttl must be of type int, %s given",
			zend_zval_value_name(&ttl_zv)
		);
		zval_ptr_dtor(&ttl_zv);

		return false;
	}

	if (Z_LVAL(ttl_zv) < 0) {
		*error = ZSTR_INIT_LITERAL("OPcache\\VolatileStatic ttl must be greater than or equal to 0", 0);
		zval_ptr_dtor(&ttl_zv);

		return false;
	}

	*ttl = Z_LVAL(ttl_zv);
	zval_ptr_dtor(&ttl_zv);

	return true;
}

static zend_always_inline bool zend_opcache_static_cache_volatile_static_attribute_parse_strategy(
		zend_attribute *attr,
		uint32_t arg_num,
		zend_class_entry *scope,
		zend_long *strategy,
		zend_string **error)
{
	zval strategy_zv;

	ZVAL_COPY_OR_DUP(&strategy_zv, &attr->args[arg_num].value);
	if (!zend_opcache_static_cache_strategy_from_attribute_zval(&strategy_zv, strategy)) {
		zval_ptr_dtor(&strategy_zv);
		if (zend_get_attribute_value(&strategy_zv, attr, arg_num, scope) == FAILURE) {
			*error = ZSTR_INIT_LITERAL("OPcache\\VolatileStatic strategy must be resolvable", 0);

			return false;
		}
	}

	if (!zend_opcache_static_cache_strategy_from_attribute_zval(&strategy_zv, strategy)) {
		*error = zend_strpprintf(0,
			"OPcache\\VolatileStatic strategy must be of type OPcache\\CacheStrategy, %s given",
			zend_zval_value_name(&strategy_zv)
		);
		zval_ptr_dtor(&strategy_zv);

		return false;
	}

	zval_ptr_dtor(&strategy_zv);

	return true;
}

static zend_always_inline bool zend_opcache_static_cache_volatile_static_attribute_config_from_attribute(
		zend_attribute *attr,
		zend_class_entry *scope,
		zend_opcache_static_cache_volatile_static_attribute_config *config,
		zend_string **error)
{
	uint32_t i;
	bool ttl_seen = false, strategy_seen = false;

	zend_opcache_static_cache_volatile_static_attribute_config_init(config);
	*error = NULL;

	if (attr == NULL || attr->argc == 0) {
		return true;
	}

	if (attr->argc > 2) {
		*error = ZSTR_INIT_LITERAL("OPcache\\VolatileStatic expects at most 2 arguments", 0);

		return false;
	}

	for (i = 0; i < attr->argc; i++) {
		if (attr->args[i].name != NULL) {
			if (zend_string_equals_literal(attr->args[i].name, "ttl")) {
				if (ttl_seen) {
					*error = ZSTR_INIT_LITERAL("Named parameter $ttl overwrites previous argument", 0);

					return false;
				}

				ttl_seen = true;
				if (!zend_opcache_static_cache_volatile_static_attribute_parse_ttl(attr, i, scope, &config->ttl, error)) {
					return false;
				}

				continue;
			}

			if (zend_string_equals_literal(attr->args[i].name, "strategy")) {
				if (strategy_seen) {
					*error = ZSTR_INIT_LITERAL("Named parameter $strategy overwrites previous argument", 0);

					return false;
				}

				strategy_seen = true;
				if (!zend_opcache_static_cache_volatile_static_attribute_parse_strategy(attr, i, scope, &config->strategy, error)) {
					return false;
				}

				continue;
			}

			*error = zend_strpprintf(0, "Unknown named parameter $%s", ZSTR_VAL(attr->args[i].name));

			return false;
		}

		if (i == 0) {
			if (ttl_seen) {
				*error = ZSTR_INIT_LITERAL("Named parameter $ttl overwrites previous argument", 0);

				return false;
			}

			ttl_seen = true;
			if (!zend_opcache_static_cache_volatile_static_attribute_parse_ttl(attr, i, scope, &config->ttl, error)) {
				return false;
			}

			continue;
		}

		if (strategy_seen) {
			*error = ZSTR_INIT_LITERAL("Named parameter $strategy overwrites previous argument", 0);

			return false;
		}

		strategy_seen = true;
		if (!zend_opcache_static_cache_volatile_static_attribute_parse_strategy(attr, i, scope, &config->strategy, error)) {
			return false;
		}
	}

	return true;
}

static zend_always_inline zend_opcache_static_cache_context *zend_opcache_static_cache_activate_context(zend_opcache_static_cache_context *context)
{
	zend_opcache_static_cache_context *previous = zend_opcache_static_cache_active_context_ptr;

	zend_opcache_static_cache_active_context_ptr = context;

	return previous;
}

static zend_always_inline void zend_opcache_static_cache_restore_context(zend_opcache_static_cache_context *context)
{
	zend_opcache_static_cache_active_context_ptr = context;
}

static zend_always_inline zend_opcache_static_cache_runtime *zend_opcache_static_cache_context_runtime(zend_opcache_static_cache_context *context)
{
	/* Runtime readiness is request-local under ZTS, while storage remains shared. */
	return context == &zend_opcache_static_cache_pinned_context_state
		? &zend_opcache_static_cache_pinned_runtime_state
		: &zend_opcache_static_cache_volatile_runtime_state
	;
}

static zend_always_inline zend_opcache_static_cache_runtime *zend_opcache_static_cache_active_runtime(void)
{
	return zend_opcache_static_cache_context_runtime(zend_opcache_static_cache_active_context());
}

static zend_always_inline zend_opcache_static_cache_lookup_entry *zend_opcache_static_cache_active_lookup_entries(void)
{
	return zend_opcache_static_cache_active_context() == &zend_opcache_static_cache_pinned_context_state
		? zend_opcache_static_cache_pinned_lookup_entry_storage
		: zend_opcache_static_cache_volatile_lookup_entry_storage
	;
}

static zend_always_inline HashTable **zend_opcache_static_cache_active_request_local_slots_ptr(void)
{
	return zend_opcache_static_cache_active_context() == &zend_opcache_static_cache_pinned_context_state
		? &zend_opcache_static_cache_pinned_request_local_slots
		: &zend_opcache_static_cache_volatile_request_local_slots
	;
}

static zend_always_inline zend_opcache_static_cache_lookup_entry *zend_opcache_static_cache_lookup_cache_set(zend_ulong hash)
{
	uint32_t set_index = (uint32_t) (hash & (ZEND_OPCACHE_STATIC_CACHE_LOOKUP_SETS - 1));

	return &zend_opcache_static_cache_active_lookup_entries()[set_index * ZEND_OPCACHE_STATIC_CACHE_LOOKUP_WAYS];
}

static zend_always_inline void zend_opcache_static_cache_lookup_cache_store(
		zend_opcache_static_cache_lookup_entry *lookup_entry,
		zend_ulong hash,
		uint64_t mutation_epoch,
		uint32_t slot_index,
		uint8_t state)
{
	if (lookup_entry == NULL) {
		return;
	}

	lookup_entry->hash = hash;
	lookup_entry->mutation_epoch = mutation_epoch;
	lookup_entry->slot_index = slot_index;
	lookup_entry->state = state;
}

static zend_always_inline zend_opcache_static_cache_lookup_entry *zend_opcache_static_cache_lookup_cache_preferred_way(
		zend_opcache_static_cache_lookup_entry *lookup_entries,
		zend_ulong hash)
{
	uint32_t way = (uint32_t) ((((uint64_t) hash >> 32) ^ hash) & (ZEND_OPCACHE_STATIC_CACHE_LOOKUP_WAYS - 1));

	return &lookup_entries[way];
}

static zend_always_inline zend_opcache_static_cache_lookup_entry *zend_opcache_static_cache_lookup_cache_select_slot(
		zend_opcache_static_cache_lookup_entry *lookup_entries,
		zend_ulong hash,
		uint64_t mutation_epoch,
		bool allow_hit_eviction)
{
	zend_opcache_static_cache_lookup_entry *preferred, *alternate;

	if (lookup_entries == NULL) {
		return NULL;
	}

	preferred = zend_opcache_static_cache_lookup_cache_preferred_way(lookup_entries, hash);
	alternate = preferred == &lookup_entries[0] ? &lookup_entries[1] : &lookup_entries[0];

	/* Keep each set two-way associative: the hash chooses a stable preferred
	 * way, while the alternate way gives short collision chains a request-local
	 * fast path without touching the shared entry table. Mutation epochs make
	 * stale hit/miss entries self-invalidating after any writer updates SHM. */
	if (preferred->state != ZEND_OPCACHE_STATIC_CACHE_LOOKUP_EMPTY && preferred->hash == hash && preferred->mutation_epoch == mutation_epoch) {
		return preferred;
	}
	if (alternate->state != ZEND_OPCACHE_STATIC_CACHE_LOOKUP_EMPTY && alternate->hash == hash && alternate->mutation_epoch == mutation_epoch) {
		return alternate;
	}

	if (preferred->state == ZEND_OPCACHE_STATIC_CACHE_LOOKUP_EMPTY || preferred->mutation_epoch != mutation_epoch) {
		return preferred;
	}
	if (alternate->state == ZEND_OPCACHE_STATIC_CACHE_LOOKUP_EMPTY || alternate->mutation_epoch != mutation_epoch) {
		return alternate;
	}

	if (preferred->state == ZEND_OPCACHE_STATIC_CACHE_LOOKUP_MISS) {
		return preferred;
	}
	if (alternate->state == ZEND_OPCACHE_STATIC_CACHE_LOOKUP_MISS) {
		return alternate;
	}

	return allow_hit_eviction ? preferred : NULL;
}

static zend_always_inline void zend_opcache_static_cache_lookup_cache_store_miss(
		zend_opcache_static_cache_lookup_entry *lookup_entries,
		zend_ulong hash,
		uint64_t mutation_epoch)
{
	zend_opcache_static_cache_lookup_entry *victim = zend_opcache_static_cache_lookup_cache_select_slot(lookup_entries, hash, mutation_epoch, false);

	if (victim == NULL) {
		return;
	}

	zend_opcache_static_cache_lookup_cache_store(
		victim,
		hash,
		mutation_epoch,
		ZEND_OPCACHE_STATIC_CACHE_LOOKUP_INVALID_SLOT,
		ZEND_OPCACHE_STATIC_CACHE_LOOKUP_MISS
	);
}

static zend_always_inline void zend_opcache_static_cache_lookup_cache_store_hit(
		zend_opcache_static_cache_lookup_entry *lookup_entry,
		zend_ulong hash,
		uint64_t mutation_epoch,
		uint32_t slot_index)
{
	zend_opcache_static_cache_lookup_cache_store(
		lookup_entry,
		hash,
		mutation_epoch,
		slot_index,
		ZEND_OPCACHE_STATIC_CACHE_LOOKUP_HIT
	);
}

static zend_always_inline bool zend_opcache_static_cache_header_is_initialized_locked(void)
{
	zend_opcache_static_cache_header *header = zend_opcache_static_cache_header_ptr();

	return header != NULL &&
		header->magic == ZEND_OPCACHE_STATIC_CACHE_MAGIC &&
		header->version == ZEND_OPCACHE_STATIC_CACHE_VERSION
	;
}

static zend_always_inline void zend_opcache_static_cache_bump_mutation_epoch_locked(zend_opcache_static_cache_header *header)
{
	if (header == NULL) {
		return;
	}

	/* uint64_t makes wrap-around essentially unreachable in practice, but on
 	 * wrap we skip 0 so a freshly bumped epoch can never coincide with the
	 * zero-initialized lookup_entry slot value. */
	header->mutation_epoch++;
	if (header->mutation_epoch == 0) {
		header->mutation_epoch = 1;
	}
}

static zend_always_inline zend_opcache_static_cache_entry *zend_opcache_static_cache_entries(zend_opcache_static_cache_header *header)
{
	return (zend_opcache_static_cache_entry *) ((char *) header + sizeof(zend_opcache_static_cache_header));
}

static zend_always_inline bool zend_opcache_static_cache_key_equals(
		const zend_opcache_static_cache_entry *entry,
		zend_string *key,
		zend_ulong hash)
{
	if (entry->state != ZEND_OPCACHE_STATIC_CACHE_ENTRY_USED || entry->hash != hash || entry->key_len != ZSTR_LEN(key)) {
		return false;
	}

	return memcmp(zend_opcache_static_cache_ptr(entry->key_offset), ZSTR_VAL(key), ZSTR_LEN(key)) == 0;
}

static zend_always_inline bool zend_opcache_static_cache_value_uses_offset(uint8_t value_type)
{
	return
		value_type == ZEND_OPCACHE_STATIC_CACHE_VALUE_STRING ||
		value_type == ZEND_OPCACHE_STATIC_CACHE_VALUE_SERIALIZED ||
		value_type == ZEND_OPCACHE_STATIC_CACHE_VALUE_OPCACHE_SERIALIZED ||
		value_type == ZEND_OPCACHE_STATIC_CACHE_VALUE_SHARED_GRAPH
	;
}

static zend_always_inline bool zend_opcache_static_cache_block_is_free(const zend_opcache_static_cache_block *block)
{
	return (block->flags & ZEND_OPCACHE_STATIC_CACHE_BLOCK_FREE) != 0;
}

static zend_always_inline void zend_opcache_static_cache_block_mark_free(zend_opcache_static_cache_block *block)
{
	block->flags |= ZEND_OPCACHE_STATIC_CACHE_BLOCK_FREE;
}

static zend_always_inline void zend_opcache_static_cache_block_mark_used(zend_opcache_static_cache_block *block)
{
	block->flags &= ~ZEND_OPCACHE_STATIC_CACHE_BLOCK_FREE;
	block->next_free = 0;
	block->prev_free = 0;
}

static zend_always_inline void zend_opcache_static_cache_clear_lookup_cache_context(zend_opcache_static_cache_context *context)
{
	zend_opcache_static_cache_lookup_entry *entries = context == &zend_opcache_static_cache_pinned_context_state
		? zend_opcache_static_cache_pinned_lookup_entry_storage
		: zend_opcache_static_cache_volatile_lookup_entry_storage
	;

	memset(entries, 0, sizeof(zend_opcache_static_cache_lookup_entry) * ZEND_OPCACHE_STATIC_CACHE_LOOKUP_BUCKETS);
}

static zend_always_inline void zend_opcache_static_cache_lookup_cache_reset_entry(zend_opcache_static_cache_lookup_entry *lookup_entry)
{
	if (lookup_entry == NULL) {
		return;
	}

	memset(lookup_entry, 0, sizeof(*lookup_entry));
}

static zend_always_inline void zend_opcache_static_cache_clear_lookup_caches(void)
{
	zend_opcache_static_cache_clear_lookup_cache_context(&zend_opcache_static_cache_volatile_context_state);
	zend_opcache_static_cache_clear_lookup_cache_context(&zend_opcache_static_cache_pinned_context_state);
}

static zend_always_inline bool *zend_opcache_static_cache_skip_publish_ptr(zend_opcache_static_cache_context *context)
{
	return context == &zend_opcache_static_cache_pinned_context_state
		? &zend_opcache_static_cache_pinned_skip_publish
		: &zend_opcache_static_cache_volatile_skip_publish
	;
}

static zend_always_inline void zend_opcache_static_cache_mark_publish_skipped(zend_opcache_static_cache_context *context)
{
	*zend_opcache_static_cache_skip_publish_ptr(context) = true;
}

static zend_always_inline bool zend_opcache_static_cache_publish_skipped(zend_opcache_static_cache_context *context)
{
	return *zend_opcache_static_cache_skip_publish_ptr(context);
}

static zend_always_inline void zend_opcache_static_cache_reset_publish_skips(void)
{
	zend_opcache_static_cache_volatile_skip_publish = false;
	zend_opcache_static_cache_pinned_skip_publish = false;
}

#endif /* ZEND_STATIC_CACHE_INTERNAL_H */
