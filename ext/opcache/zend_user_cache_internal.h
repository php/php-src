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

#ifndef ZEND_USER_CACHE_INTERNAL_H
#define ZEND_USER_CACHE_INTERNAL_H

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
#include "zend_user_cache.h"

#include "ext/standard/php_var.h"

#include "SAPI.h"

#define ZEND_OPCACHE_USER_CACHE_MAGIC 0xCAC17E01U
#define ZEND_OPCACHE_USER_CACHE_VERSION 1U
#define ZEND_OPCACHE_USER_CACHE_MIN_CAPACITY 127U
#define ZEND_OPCACHE_USER_CACHE_MAX_CAPACITY 65521U
#define ZEND_OPCACHE_USER_CACHE_SLOT_BYTES 256U
/* Storage-key component separator. */
#define ZEND_OPCACHE_USER_CACHE_KEY_DELIMITER "\x1f"
#define ZEND_OPCACHE_USER_CACHE_KEY_DELIMITER_CHAR '\x1f'
#define ZEND_OPCACHE_USER_CACHE_KEY_DELIMITER_NAME "0x1F"

#define ZEND_OPCACHE_USER_CACHE_ENTRY_EMPTY 0
#define ZEND_OPCACHE_USER_CACHE_ENTRY_USED 1
#define ZEND_OPCACHE_USER_CACHE_ENTRY_TOMBSTONE 2

#define ZEND_OPCACHE_USER_CACHE_VALUE_NULL 0
#define ZEND_OPCACHE_USER_CACHE_VALUE_TRUE 1
#define ZEND_OPCACHE_USER_CACHE_VALUE_FALSE 2
#define ZEND_OPCACHE_USER_CACHE_VALUE_LONG 3
#define ZEND_OPCACHE_USER_CACHE_VALUE_DOUBLE 4
#define ZEND_OPCACHE_USER_CACHE_VALUE_STRING 5
#define ZEND_OPCACHE_USER_CACHE_VALUE_SHARED_GRAPH 8

#define ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_MAGIC 0xCAC17E02U
#define ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VERSION 9U
/* Graph needs per-decode identity maps. */
#define ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_FLAG_HAS_SHARED_IDENTITY 0x2U
/* Decode may resolve a class and must run off-lock. */
#define ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_FLAG_HAS_OBJECT 0x4U
/* Prefer request-local prototype cloning on repeated fetches. */
#define ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_FLAG_PREFERS_PROTOTYPE 0x8U
#define ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_RETIRED (1 << 30)
#define ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_REFCOUNT_MASK (ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_RETIRED - 1)

#define ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_UNDEF 0
#define ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_NULL 1
#define ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_TRUE 2
#define ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_FALSE 3
#define ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_LONG 4
#define ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_DOUBLE 5
#define ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_STRING 6
#define ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_ARRAY 7
#define ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_OBJECT 8
#define ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_DYNAMIC_ARRAY 10
/* Back-reference to an OBJECT node. */
#define ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_OBJECT_REF 11
/* PHP reference and back-reference to one. */
#define ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_REFERENCE 13
#define ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_REFERENCE_REF 14

/* Back-reference to a DYNAMIC_ARRAY node. */
#define ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_ARRAY_REF 15

/* Enum case encoded as class name + case name. */
#define ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_ENUM 16

/* Internal object restored through a registered C state handler. */
#define ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_VALUE_SAFE_DIRECT_OBJECT 18

/* Node may be targeted by a back-reference. */
#define ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_OBJECT_FLAG_SHARED 0x1U

/* Rebind selected spl_object_hash() property values on fetch. */
#define ZEND_OPCACHE_USER_CACHE_SHARED_GRAPH_PROPERTY_FLAG_SELF_OBJECT_HASH 0x1U

#define ZEND_OPCACHE_USER_CACHE_LOOKUP_BUCKETS 256U
#define ZEND_OPCACHE_USER_CACHE_LOOKUP_WAYS 2U
#define ZEND_OPCACHE_USER_CACHE_LOOKUP_SETS (ZEND_OPCACHE_USER_CACHE_LOOKUP_BUCKETS / ZEND_OPCACHE_USER_CACHE_LOOKUP_WAYS)
#define ZEND_OPCACHE_USER_CACHE_LOOKUP_INVALID_SLOT UINT32_MAX
#define ZEND_OPCACHE_USER_CACHE_REQUEST_LOCAL_STRING_MIN_LEN 256U
/* Large strings are stored as STRING-rooted shared graphs. */
#define ZEND_OPCACHE_USER_CACHE_DIRECT_STRING_MIN_LEN 4096U

#define ZEND_OPCACHE_USER_CACHE_ENTRY_LOCK_TABLE_SIZE 1024U
#define ZEND_OPCACHE_USER_CACHE_ENTRY_LOCK_EMPTY 0
#define ZEND_OPCACHE_USER_CACHE_ENTRY_LOCK_USED 1
#define ZEND_OPCACHE_USER_CACHE_ENTRY_LOCK_TOMBSTONE 2
#define ZEND_OPCACHE_USER_CACHE_LOW_MEMORY_COMPACT_THRESHOLD (3U * 1024U * 1024U)

/* Optimistic fetch needs 64-bit atomics and explicit fences. */
#if defined(__GNUC__) || defined(__clang__)
# define ZEND_OPCACHE_USER_CACHE_HAVE_OPTIMISTIC 1
#elif defined(ZEND_WIN32) && defined(_MSC_VER)
# include <intrin.h>
# define ZEND_OPCACHE_USER_CACHE_HAVE_OPTIMISTIC 1
# define ZEND_OPCACHE_USER_CACHE_OPTIMISTIC_MSVC 1
#endif

/* Other platforms use fcntl locks. */
#if !defined(ZEND_WIN32) && defined(__linux__)
# include <pthread.h>
# define ZEND_OPCACHE_USER_CACHE_HAVE_SHARED_MUTEX 1
#endif

#define ZEND_OPCACHE_USER_CACHE_LOCK_MODEL_FCNTL 0U
#define ZEND_OPCACHE_USER_CACHE_LOCK_MODEL_MUTEX 1U

#define ZEND_OPCACHE_USER_CACHE_READER_SLOTS 64U
#define ZEND_OPCACHE_USER_CACHE_READER_DRAIN_SPIN 1024U
#define ZEND_OPCACHE_USER_CACHE_READER_DRAIN_TIMEOUT_US 10000U
/* Force-retired graph payloads pending a later free. */
#define ZEND_OPCACHE_USER_CACHE_ORPHANED_GRAPH_SLOTS 32U

#define ZEND_OPCACHE_USER_CACHE_LOOKUP_EMPTY 0
#define ZEND_OPCACHE_USER_CACHE_LOOKUP_HIT 1
#define ZEND_OPCACHE_USER_CACHE_LOOKUP_MISS 2

#define ZEND_OPCACHE_USER_CACHE_ENTRY_RESERVED_COMBINED_VALUE_KEY 0x0001U

#define ZEND_OPCACHE_USER_CACHE_BLOCK_FREE 1U
#define ZEND_OPCACHE_USER_CACHE_LOOKUP_VALUE_NONE 0xFFU

#ifndef ZEND_WIN32
# define ZEND_OPCACHE_USER_CACHE_SEM_FILENAME_PREFIX ".ZendUserCacheSem."
#endif

#ifdef ZEND_OPCACHE_USER_CACHE_HAVE_SHARED_MUTEX
typedef union {
	pthread_mutex_t mutex;
	char padding[64];
} zend_opcache_user_cache_shared_mutex;
#endif

typedef struct {
	bool enabled;
	bool available;
	bool startup_failed;
	bool backend_initialized;
	size_t configured_memory;
	const char *failure_reason;
} zend_opcache_user_cache_runtime;

typedef struct {
	const zend_shared_memory_handlers *handler;
	zend_shared_segment **segments;
	int segment_count;
	size_t size;
	const char *model;
	bool initialized;
	bool initialized_before_request;
	bool lock_initialized;
	/* Resolved from header->lock_model at storage startup. */
	bool use_shared_mutex;
	int lock_file;
	char lockfile_name[MAXPATHLEN];
#ifdef ZTS
	MUTEX_T zts_lock;
#endif
} zend_opcache_user_cache_storage;

typedef struct {
	zend_opcache_user_cache_storage storage;
	const char *name;
	const char *lock_name;
#ifndef ZEND_WIN32
	const char *sem_filename_prefix;
#endif
	bool clear_on_pressure;
	bool strict_store_failure;
} zend_opcache_user_cache_context;

struct _zend_opcache_user_cache_partition {
	zend_opcache_user_cache_context context;
	char *name;
	struct _zend_opcache_user_cache_partition *next;
};

typedef struct {
	zend_ulong hash;
	uint64_t owner_pid;
	uint64_t owner_start_time;
	uint64_t expires_at;
	uint32_t key_offset;
	uint32_t key_len;
	uint8_t state;
	uint8_t reserved[7];
} zend_opcache_user_cache_entry_lock_record;

/* Identifies an optimistic reader window; start time distinguishes recycled PIDs. */
typedef struct {
	uint64_t owner_pid;
	uint64_t owner_incarnation;
	uint64_t owner_start_time;
	uint32_t active;
	uint32_t reserved;
} zend_opcache_user_cache_reader_slot;

/* Active SHM context for storage helpers. */
typedef struct {
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
	/* Odd while a writer is active, even otherwise. */
	uint64_t write_seq;
	/* Optimistic readers currently inside pin windows. */
	uint32_t active_optimistic_readers;
	uint32_t tombstone_count;
	/* Selected once at header initialization. */
	uint32_t lock_model;
	uint32_t lock_model_reserved;
	uint32_t orphaned_graphs_saturated;
	uint32_t orphaned_graphs_reserved;
	uint32_t orphaned_graphs[ZEND_OPCACHE_USER_CACHE_ORPHANED_GRAPH_SLOTS];
	zend_opcache_user_cache_reader_slot reader_slots[ZEND_OPCACHE_USER_CACHE_READER_SLOTS];
#ifdef ZEND_OPCACHE_USER_CACHE_HAVE_SHARED_MUTEX
	/* Protects exact-key entry lock records in mutex mode. */
	zend_opcache_user_cache_shared_mutex global_shared_mutex;
#endif
	zend_opcache_user_cache_entry_lock_record entry_lock_records[ZEND_OPCACHE_USER_CACHE_ENTRY_LOCK_TABLE_SIZE];
#ifndef ZEND_WIN32
	uint32_t reserved_lock;
#endif
} zend_opcache_user_cache_header;

typedef struct {
	uint32_t size;
	uint32_t prev_size;
	uint32_t next_free;
	uint32_t prev_free;
	uint32_t flags;
} zend_opcache_user_cache_block;

typedef struct {
	zend_ulong hash;
	uint32_t key_offset;
	uint32_t key_len;
	uint32_t value_offset;
	uint32_t value_len;
	uint64_t expires_at;
	uint64_t generation;
	uint8_t state;
	uint8_t value_type;
	uint16_t reserved;
	zend_long long_value;
	double double_value;
} zend_opcache_user_cache_entry;

typedef struct {
	bool found;
	zend_opcache_user_cache_entry entry;
} zend_opcache_user_cache_replaced_entry;

typedef struct {
	zend_ulong hash;
	uint64_t mutation_epoch;
	uint32_t slot_index;
	uint8_t state;
	/* Inline scalar for exact-key, same-epoch hits. */
	uint8_t value_type;
	uint8_t reserved[2];
	zend_string *key;
	zend_long long_value;
	double double_value;
} zend_opcache_user_cache_lookup_entry;

typedef struct {
	uint64_t generation;
	bool needs_clone;
	bool has_array_clone_flags;
	/* Allows clone without identity maps. */
	bool no_aliases;
	zval value;
	HashTable array_clone_flags;
} zend_opcache_user_cache_request_local_slot;

typedef struct {
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
	/* Safe-direct state arrays computed during prepare. */
	HashTable *state_memo;
} zend_opcache_user_cache_prepared_value;

typedef struct {
	uint32_t magic;
	uint32_t version;
	uint32_t root_offset;
	uint32_t root_type;
	uint32_t flags;
	zend_atomic_int ref_state;
} zend_opcache_user_cache_shared_graph_header;

typedef struct {
	zend_opcache_user_cache_context *context;
	uint32_t payload_offset;
} zend_opcache_user_cache_shared_graph_ref;

typedef struct {
	zend_opcache_user_cache_context *context;
	uint64_t owner_pid;
	zend_long lease;
	bool preserve_lease;
} zend_opcache_user_cache_entry_lock;

typedef struct {
	uint8_t type;
	uint8_t reserved[7];
	union {
		zend_long long_value;
		double double_value;
		uint64_t offset;
	} payload;
} zend_opcache_user_cache_shared_graph_value;

typedef struct {
	uint32_t name_offset;
	uint32_t reserved;
	zend_opcache_user_cache_shared_graph_value value;
} zend_opcache_user_cache_shared_graph_property;

typedef struct {
	zend_ulong h;
	uint32_t key_offset;
	uint32_t reserved;
	zend_opcache_user_cache_shared_graph_value value;
} zend_opcache_user_cache_shared_graph_array_element;

typedef struct {
	uint32_t count;
	uint32_t next_free;
	uint32_t elements_offset;
	uint32_t reserved;
} zend_opcache_user_cache_shared_graph_array;

typedef struct {
	uint32_t class_name_offset;
	uint32_t property_count;
	uint32_t properties_offset;
	uint32_t reserved;
} zend_opcache_user_cache_shared_graph_object;

typedef struct {
	uint32_t class_name_offset;
	uint32_t property_count;
	uint32_t properties_offset;
	uint32_t reserved;
	zend_opcache_user_cache_shared_graph_value state;
} zend_opcache_user_cache_shared_graph_safe_direct_object;

typedef struct {
	uint32_t flags;     /* OBJECT_FLAG_SHARED if the reference is shared/cyclic */
	uint32_t reserved;
	zend_opcache_user_cache_shared_graph_value inner;
} zend_opcache_user_cache_shared_graph_reference;

typedef struct _zend_opcache_user_cache_shared_graph_enum {
	uint32_t class_name_offset;
	uint32_t case_name_offset;
} zend_opcache_user_cache_shared_graph_enum;

typedef struct {
	size_t size;
	HashTable seen_arrays;
	HashTable seen_objects;
	HashTable seen_references;
	/* Matches the copy pass's string deduplication. */
	HashTable string_dedup;
	/* Safe-direct state arrays shared with the copy pass. */
	HashTable *state_memo;
} zend_opcache_user_cache_shared_graph_calc_context;

typedef struct {
	unsigned char *buffer;
	size_t size;
	size_t position;
	HashTable seen_arrays;
	HashTable seen_objects;
	HashTable seen_references;
	HashTable string_dedup;
	bool has_shared_identity;
	bool has_object;
	bool prefers_prototype;
	/* Safe-direct state arrays shared with the calc pass. */
	HashTable *state_memo;
} zend_opcache_user_cache_shared_graph_copy_context;

typedef enum {
	ZEND_OPCACHE_USER_CACHE_OPTIMISTIC_FALLBACK = 0,
	ZEND_OPCACHE_USER_CACHE_OPTIMISTIC_FOUND,
	ZEND_OPCACHE_USER_CACHE_OPTIMISTIC_MISS
} zend_opcache_user_cache_optimistic_result;

extern zend_class_entry *zend_opcache_user_cache_exception_ce;
extern zend_class_entry *zend_opcache_user_cache_info_ce;
extern zend_opcache_user_cache_context zend_opcache_user_cache_context_state;
extern bool zend_opcache_user_cache_runtime_opted_in;
extern zend_opcache_user_cache_partition *zend_opcache_user_cache_partitions;
extern ZEND_EXT_TLS zend_opcache_user_cache_partition *zend_opcache_user_cache_active_partition;
extern ZEND_EXT_TLS zend_opcache_user_cache_runtime zend_opcache_user_cache_runtime_state;
extern ZEND_EXT_TLS zend_opcache_user_cache_context *zend_opcache_user_cache_active_context_ptr;
extern ZEND_EXT_TLS const char *zend_opcache_user_cache_request_unavailable_reason;
extern ZEND_EXT_TLS bool zend_opcache_user_cache_lock_held;
extern ZEND_EXT_TLS bool zend_opcache_user_cache_lock_held_is_write;
extern ZEND_EXT_TLS zend_opcache_user_cache_shared_graph_ref *zend_opcache_user_cache_shared_graph_refs;
extern ZEND_EXT_TLS uint32_t zend_opcache_user_cache_shared_graph_ref_count;
extern ZEND_EXT_TLS uint32_t zend_opcache_user_cache_shared_graph_ref_capacity;
extern ZEND_EXT_TLS HashTable *zend_opcache_user_cache_shared_graph_ref_index;
extern ZEND_EXT_TLS HashTable *zend_opcache_user_cache_decode_resolve_cache;
extern ZEND_EXT_TLS zend_opcache_user_cache_shared_graph_ref *zend_opcache_user_cache_retired_shared_graphs;
extern ZEND_EXT_TLS uint32_t zend_opcache_user_cache_retired_shared_graph_count;
extern ZEND_EXT_TLS uint32_t zend_opcache_user_cache_retired_shared_graph_capacity;
extern ZEND_EXT_TLS zend_opcache_user_cache_lookup_entry zend_opcache_user_cache_lookup_entry_storage[ZEND_OPCACHE_USER_CACHE_LOOKUP_BUCKETS];
extern ZEND_EXT_TLS HashTable *zend_opcache_user_cache_request_local_slot_table;
extern ZEND_EXT_TLS HashTable *zend_opcache_user_cache_entry_lock_table;

void zend_opcache_user_cache_reset_runtime(void);
void zend_opcache_user_cache_reset_storage(void);
bool zend_opcache_user_cache_header_init_locked(void);
void zend_opcache_user_cache_free_locked(uint32_t payload_offset);
uint32_t zend_opcache_user_cache_alloc_locked(size_t size, const void *source);
bool zend_opcache_user_cache_compact_to_fit_locked(size_t size);
bool zend_opcache_user_cache_startup_storage_before_request(void);
void zend_opcache_user_cache_shutdown_storage(void);
void zend_opcache_user_cache_ensure_ready(void);
bool zend_opcache_user_cache_rlock(void);
bool zend_opcache_user_cache_wlock(void);
bool zend_opcache_user_cache_wlock_for_entry_mutation(zend_string *key);
void zend_opcache_user_cache_unlock(void);
void zend_opcache_user_cache_unlock_if_held(void);
bool zend_opcache_user_cache_acquire_entry_lock(zend_string *key);
bool zend_opcache_user_cache_try_acquire_entry_lock(zend_string *key, zend_long lease);
bool zend_opcache_user_cache_has_entry_lock(zend_string *key);
bool zend_opcache_user_cache_release_entry_lock(zend_string *key);
bool zend_opcache_user_cache_entry_locks_allow_clear_locked(void);
void zend_opcache_user_cache_release_active_entry_locks(void);
void zend_opcache_user_cache_release_request_entry_locks(void);
void zend_opcache_user_cache_safe_direct_handlers_init(void);
void zend_opcache_user_cache_safe_direct_handlers_destroy(void);
zend_opcache_user_cache_safe_direct_state_copy_func_t zend_opcache_user_cache_safe_direct_copy_func(
	zend_class_entry *ce,
	zend_class_entry **base_ce_ptr
);
zend_class_entry *zend_opcache_user_cache_safe_direct_find_base(zend_class_entry *ce);
zend_opcache_user_cache_safe_direct_state_has_unstorable_func_t zend_opcache_user_cache_safe_direct_state_has_unstorable_func(
	zend_class_entry *ce
);
zend_opcache_user_cache_safe_direct_state_serialize_func_t zend_opcache_user_cache_safe_direct_state_serialize_func(
	zend_class_entry *ce
);
zend_opcache_user_cache_safe_direct_state_unserialize_func_t zend_opcache_user_cache_safe_direct_state_unserialize_func(
	zend_class_entry *ce
);
bool zend_opcache_user_cache_safe_direct_prefers_request_local_prototype(zend_class_entry *ce);
bool zend_opcache_user_cache_calculate_shared_graph_size(const zval *value, size_t *buffer_len, HashTable *state_memo);
bool zend_opcache_user_cache_build_shared_graph_in_place(const zval *value, unsigned char *buffer, size_t buffer_len, size_t *graph_len, HashTable *state_memo);
bool zend_opcache_user_cache_shared_graph_copy_fits_buffer(
	const unsigned char *source_buffer,
	size_t source_buffer_len,
	size_t source_graph_len,
	const unsigned char *target_buffer,
	size_t target_buffer_len
);
bool zend_opcache_user_cache_fetch_shared_graph(const unsigned char *buffer, size_t buffer_len, zval *destination);
bool zend_opcache_user_cache_shared_graph_requires_prototype(uint32_t payload_offset);
bool zend_opcache_user_cache_shared_graph_decode_is_lock_safe(uint32_t payload_offset);
bool zend_opcache_user_cache_shared_graph_payload_has_aliases(uint32_t payload_offset);
void zend_opcache_user_cache_decode_resolve_cache_release(void);
bool zend_opcache_user_cache_shared_graph_can_overwrite_payload_locked(uint32_t payload_offset);
bool zend_opcache_user_cache_shared_graph_can_move_payload_locked(uint32_t payload_offset);
bool zend_opcache_user_cache_shared_graph_rebase_moved_payload_locked(uint32_t payload_offset, ptrdiff_t delta);
bool zend_opcache_user_cache_shared_graph_acquire_locked(uint32_t payload_offset);
bool zend_opcache_user_cache_shared_graph_retire_payload_locked(uint32_t payload_offset);
bool zend_opcache_user_cache_shared_graph_release_ref_locked(uint32_t payload_offset);
bool zend_opcache_user_cache_has_request_shared_graph_ref(uint32_t payload_offset);
void zend_opcache_user_cache_register_shared_graph_ref(uint32_t payload_offset);
void zend_opcache_user_cache_defer_retired_shared_graph_free(uint32_t payload_offset);
bool zend_opcache_user_cache_release_request_shared_graph_refs(void);
bool zend_opcache_user_cache_clear_locked(void);
bool zend_opcache_user_cache_prepare_value(
	zend_opcache_user_cache_prepared_value *prepared,
	zend_string *key,
	zval *value,
	bool throw_on_failure,
	bool honor_strict_store_failure,
	bool lock_held
);
void zend_opcache_user_cache_destroy_prepared_value(zend_opcache_user_cache_prepared_value *prepared);
bool zend_opcache_user_cache_store_prepared_locked(
	zend_string *key,
	zval *value,
	const zend_opcache_user_cache_prepared_value *prepared,
	zend_long ttl,
	bool throw_on_failure,
	bool honor_strict_store_failure,
	uint64_t *generation_ptr,
	bool *seed_request_local_slot_ptr,
	zend_opcache_user_cache_replaced_entry *replaced_entry_ptr,
	bool allow_pressure_retries
);
bool zend_opcache_user_cache_store_locked(zend_string *key, zval *value, zend_long ttl, bool throw_on_failure, bool honor_strict_store_failure);
bool zend_opcache_user_cache_fetch_locked(zend_string *key, zval *return_value, bool throw_if_missing, bool *found_ptr, bool use_request_local_slot);
bool zend_opcache_user_cache_exists_locked(zend_string *key);
bool zend_opcache_user_cache_delete_locked(zend_string *key);
void zend_opcache_user_cache_discard_replaced_entry_locked(zend_string *key, zend_opcache_user_cache_replaced_entry *replaced_entry);
void zend_opcache_user_cache_rollback_replaced_entry_locked(zend_string *key, zend_opcache_user_cache_replaced_entry *replaced_entry);
void zend_opcache_user_cache_delete_entries_by_prefix_locked(zend_string *prefix);
bool zend_opcache_user_cache_atomic_update_locked(
	zend_string *key,
	zend_long step,
	zend_long ttl,
	bool decrement,
	bool insert_if_missing,
	zend_long *new_value,
	bool *is_overflow
);
void zend_opcache_user_cache_release_request_local_slots(void);
void zend_opcache_user_cache_release_active_request_local_slots(void);
void zend_opcache_user_cache_release_active_request_local_slots_by_prefix(zend_string *prefix);
void zend_opcache_user_cache_store_request_local_value_slot(zend_string *key, uint64_t generation, zval *value, bool no_aliases);
zend_opcache_user_cache_optimistic_result zend_opcache_user_cache_fetch_optimistic(
	zend_string *key,
	zval *return_value
);
zend_opcache_user_cache_optimistic_result zend_opcache_user_cache_exists_optimistic(zend_string *key);
bool zend_opcache_user_cache_optimistic_reader_begin(zend_opcache_user_cache_header *header, uint32_t *slot_index_ptr);
void zend_opcache_user_cache_optimistic_reader_end(zend_opcache_user_cache_header *header, uint32_t slot_index);
void zend_opcache_user_cache_optimistic_fork_setup(void);
bool zend_opcache_user_cache_graph_payloads_quiescent_locked(void);
void zend_opcache_user_cache_shared_graph_ref_reserve(void);
void zend_opcache_user_cache_orphan_graph_payload_locked(uint32_t payload_offset);
void zend_opcache_user_cache_reclaim_orphaned_graphs_locked(void);

static zend_always_inline bool zend_opcache_user_cache_is_userland_declared_non_public_property(
		zend_object *object,
		uint32_t property_index)
{
	zend_property_info *property_info;

	if (object->ce->properties_info_table == NULL ||
		property_index >= object->ce->default_properties_count
	) {
		return false;
	}

	property_info = object->ce->properties_info_table[property_index];

	return property_info != NULL &&
		property_info != ZEND_WRONG_PROPERTY_INFO &&
		property_info->ce != NULL &&
		property_info->ce->type == ZEND_USER_CLASS &&
		(property_info->flags & (ZEND_ACC_STATIC|ZEND_ACC_VIRTUAL)) == 0 &&
		property_info->offset != ZEND_VIRTUAL_PROPERTY_OFFSET &&
		(property_info->flags & ZEND_ACC_PPP_MASK) != ZEND_ACC_PUBLIC
	;
}

#ifdef ZEND_OPCACHE_USER_CACHE_HAVE_OPTIMISTIC
/* MSVC Interlocked operations provide full barriers. */
#ifdef ZEND_OPCACHE_USER_CACHE_OPTIMISTIC_MSVC
static zend_always_inline uint64_t zend_opcache_user_cache_atomic_load_64(const uint64_t *target)
{
	return (uint64_t) _InterlockedOr64((volatile __int64 *) target, 0);
}

static zend_always_inline void zend_opcache_user_cache_atomic_store_64(uint64_t *target, uint64_t value)
{
	(void) _InterlockedExchange64((volatile __int64 *) target, (__int64) value);
}

static zend_always_inline bool zend_opcache_user_cache_atomic_cas_64(uint64_t *target, uint64_t expected, uint64_t desired)
{
	return (uint64_t) _InterlockedCompareExchange64(
		(volatile __int64 *) target,
		(__int64) desired,
		(__int64) expected
	) == expected;
}

static zend_always_inline uint32_t zend_opcache_user_cache_atomic_load_32(const uint32_t *target)
{
	return (uint32_t) _InterlockedOr((volatile long *) target, 0);
}

static zend_always_inline void zend_opcache_user_cache_atomic_store_32(uint32_t *target, uint32_t value)
{
	(void) _InterlockedExchange((volatile long *) target, (long) value);
}

static zend_always_inline void zend_opcache_user_cache_atomic_inc_32(uint32_t *target)
{
	(void) _InterlockedIncrement((volatile long *) target);
}

static zend_always_inline void zend_opcache_user_cache_atomic_dec_32(uint32_t *target)
{
	(void) _InterlockedDecrement((volatile long *) target);
}

static zend_always_inline bool zend_opcache_user_cache_atomic_cas_32(uint32_t *target, uint32_t expected, uint32_t desired)
{
	return (uint32_t) _InterlockedCompareExchange((volatile long *) target, (long) desired, (long) expected) == expected;
}

static zend_always_inline void zend_opcache_user_cache_atomic_fence_seq_cst(void)
{
	MemoryBarrier();
}

static zend_always_inline void zend_opcache_user_cache_atomic_fence_acquire(void)
{
	MemoryBarrier();
}
#else
static zend_always_inline uint64_t zend_opcache_user_cache_atomic_load_64(const uint64_t *target)
{
	return __atomic_load_n(target, __ATOMIC_ACQUIRE);
}

static zend_always_inline void zend_opcache_user_cache_atomic_store_64(uint64_t *target, uint64_t value)
{
	__atomic_store_n(target, value, __ATOMIC_RELEASE);
}

static zend_always_inline bool zend_opcache_user_cache_atomic_cas_64(uint64_t *target, uint64_t expected, uint64_t desired)
{
	return __atomic_compare_exchange_n(target, &expected, desired, false, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE);
}

static zend_always_inline uint32_t zend_opcache_user_cache_atomic_load_32(const uint32_t *target)
{
	return __atomic_load_n(target, __ATOMIC_ACQUIRE);
}

static zend_always_inline void zend_opcache_user_cache_atomic_store_32(uint32_t *target, uint32_t value)
{
	__atomic_store_n(target, value, __ATOMIC_RELEASE);
}

static zend_always_inline void zend_opcache_user_cache_atomic_inc_32(uint32_t *target)
{
	(void) __atomic_fetch_add(target, 1, __ATOMIC_SEQ_CST);
}

static zend_always_inline void zend_opcache_user_cache_atomic_dec_32(uint32_t *target)
{
	(void) __atomic_fetch_sub(target, 1, __ATOMIC_SEQ_CST);
}

static zend_always_inline bool zend_opcache_user_cache_atomic_cas_32(uint32_t *target, uint32_t expected, uint32_t desired)
{
	return __atomic_compare_exchange_n(target, &expected, desired, false, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE);
}

static zend_always_inline void zend_opcache_user_cache_atomic_fence_seq_cst(void)
{
	__atomic_thread_fence(__ATOMIC_SEQ_CST);
}

static zend_always_inline void zend_opcache_user_cache_atomic_fence_acquire(void)
{
	__atomic_thread_fence(__ATOMIC_ACQUIRE);
}
#endif
#endif

static zend_always_inline zend_opcache_user_cache_context *zend_opcache_user_cache_active_context(void)
{
	if (zend_opcache_user_cache_active_context_ptr != NULL) {
		return zend_opcache_user_cache_active_context_ptr;
	}

	return zend_opcache_user_cache_active_partition != NULL
		? &zend_opcache_user_cache_active_partition->context
		: &zend_opcache_user_cache_context_state
	;
}

static zend_always_inline zend_opcache_user_cache_context *zend_opcache_user_cache_default_context(void)
{
	return zend_opcache_user_cache_active_partition != NULL
		? &zend_opcache_user_cache_active_partition->context
		: &zend_opcache_user_cache_context_state
	;
}

static zend_always_inline void *zend_opcache_user_cache_base(void)
{
	zend_opcache_user_cache_storage *storage = &zend_opcache_user_cache_active_context()->storage;

	if (!storage->initialized ||
		storage->segment_count != 1
	) {
		return NULL;
	}

	return storage->segments[0]->p;
}

static zend_always_inline zend_opcache_user_cache_header *zend_opcache_user_cache_header_ptr(void)
{
	return (zend_opcache_user_cache_header *) zend_opcache_user_cache_base();
}

static zend_always_inline char *zend_opcache_user_cache_ptr(uint32_t offset)
{
	return (char *) zend_opcache_user_cache_base() + offset;
}

static zend_always_inline zend_opcache_user_cache_block *zend_opcache_user_cache_block_ptr(uint32_t offset)
{
	return (zend_opcache_user_cache_block *) zend_opcache_user_cache_ptr(offset);
}

static zend_always_inline uint32_t zend_opcache_user_cache_block_payload_capacity(uint32_t payload_offset)
{
	zend_opcache_user_cache_block *block;

	if (payload_offset < sizeof(zend_opcache_user_cache_block)) {
		return 0;
	}

	block = zend_opcache_user_cache_block_ptr(payload_offset - sizeof(zend_opcache_user_cache_block));
	if (block->size < sizeof(zend_opcache_user_cache_block)) {
		return 0;
	}

	return block->size - (uint32_t) sizeof(zend_opcache_user_cache_block);
}

static zend_always_inline uint32_t zend_opcache_user_cache_write_payload_locked(uint32_t reusable_offset, size_t size, const void *source)
{
	if (reusable_offset != 0 && zend_opcache_user_cache_block_payload_capacity(reusable_offset) >= size) {
		memcpy(zend_opcache_user_cache_ptr(reusable_offset), source, size);

		return reusable_offset;
	}

	return zend_opcache_user_cache_alloc_locked(size, source);
}

static zend_always_inline zend_opcache_user_cache_context *zend_opcache_user_cache_activate_context(zend_opcache_user_cache_context *context)
{
	zend_opcache_user_cache_context *previous = zend_opcache_user_cache_active_context_ptr;

	zend_opcache_user_cache_active_context_ptr = context;

	return previous;
}

static zend_always_inline void zend_opcache_user_cache_restore_context(zend_opcache_user_cache_context *context)
{
	zend_opcache_user_cache_active_context_ptr = context;
}

static zend_always_inline zend_opcache_user_cache_runtime *zend_opcache_user_cache_active_runtime(void)
{
	return &zend_opcache_user_cache_runtime_state;
}

static zend_always_inline HashTable **zend_opcache_user_cache_active_request_local_slots_ptr(void)
{
	return &zend_opcache_user_cache_request_local_slot_table;
}

static zend_always_inline zend_opcache_user_cache_lookup_entry *zend_opcache_user_cache_lookup_cache_set(zend_ulong hash)
{
	uint32_t set_index = (uint32_t) (hash & (ZEND_OPCACHE_USER_CACHE_LOOKUP_SETS - 1));

	return &zend_opcache_user_cache_lookup_entry_storage[set_index * ZEND_OPCACHE_USER_CACHE_LOOKUP_WAYS];
}

static zend_always_inline void zend_opcache_user_cache_lookup_entry_release_key(
		zend_opcache_user_cache_lookup_entry *lookup_entry)
{
	if (lookup_entry->key != NULL) {
		zend_string_release(lookup_entry->key);
		lookup_entry->key = NULL;
	}
}

static zend_always_inline void zend_opcache_user_cache_lookup_cache_store(
		zend_opcache_user_cache_lookup_entry *lookup_entry,
		zend_ulong hash,
		uint64_t mutation_epoch,
		uint32_t slot_index,
		uint8_t state)
{
	if (lookup_entry == NULL) {
		return;
	}

	zend_opcache_user_cache_lookup_entry_release_key(lookup_entry);

	lookup_entry->hash = hash;
	lookup_entry->mutation_epoch = mutation_epoch;
	lookup_entry->slot_index = slot_index;
	lookup_entry->state = state;
	lookup_entry->value_type = ZEND_OPCACHE_USER_CACHE_LOOKUP_VALUE_NONE;
}

/* Store exact-key fast-path data for non-expiring scalars. */
static zend_always_inline void zend_opcache_user_cache_lookup_cache_fill_hit_value(
		zend_opcache_user_cache_lookup_entry *lookup_entry,
		zend_string *key,
		const zend_opcache_user_cache_entry *entry)
{
	if (lookup_entry == NULL) {
		return;
	}

	lookup_entry->key = zend_string_copy(key);

	if (entry->expires_at != 0) {
		return;
	}

	switch (entry->value_type) {
		case ZEND_OPCACHE_USER_CACHE_VALUE_NULL:
		case ZEND_OPCACHE_USER_CACHE_VALUE_TRUE:
		case ZEND_OPCACHE_USER_CACHE_VALUE_FALSE:
		case ZEND_OPCACHE_USER_CACHE_VALUE_LONG:
		case ZEND_OPCACHE_USER_CACHE_VALUE_DOUBLE:
			lookup_entry->value_type = entry->value_type;
			lookup_entry->long_value = entry->long_value;
			lookup_entry->double_value = entry->double_value;
			break;
		default:
			break;
	}
}

static zend_always_inline zend_opcache_user_cache_lookup_entry *zend_opcache_user_cache_lookup_cache_preferred_way(
		zend_opcache_user_cache_lookup_entry *lookup_entries,
		zend_ulong hash)
{
	uint32_t way = (uint32_t) ((((uint64_t) hash >> 32) ^ hash) & (ZEND_OPCACHE_USER_CACHE_LOOKUP_WAYS - 1));

	return &lookup_entries[way];
}

static zend_always_inline zend_opcache_user_cache_lookup_entry *zend_opcache_user_cache_lookup_cache_select_slot(
		zend_opcache_user_cache_lookup_entry *lookup_entries,
		zend_ulong hash,
		uint64_t mutation_epoch,
		bool allow_hit_eviction)
{
	zend_opcache_user_cache_lookup_entry *preferred, *alternate;

	if (lookup_entries == NULL) {
		return NULL;
	}

	preferred = zend_opcache_user_cache_lookup_cache_preferred_way(lookup_entries, hash);
	alternate = preferred == &lookup_entries[0] ? &lookup_entries[1] : &lookup_entries[0];

	/* Two-way set associative lookup cache, invalidated by mutation_epoch. */
	if (preferred->state != ZEND_OPCACHE_USER_CACHE_LOOKUP_EMPTY && preferred->hash == hash && preferred->mutation_epoch == mutation_epoch) {
		return preferred;
	}
	if (alternate->state != ZEND_OPCACHE_USER_CACHE_LOOKUP_EMPTY && alternate->hash == hash && alternate->mutation_epoch == mutation_epoch) {
		return alternate;
	}

	if (preferred->state == ZEND_OPCACHE_USER_CACHE_LOOKUP_EMPTY || preferred->mutation_epoch != mutation_epoch) {
		return preferred;
	}
	if (alternate->state == ZEND_OPCACHE_USER_CACHE_LOOKUP_EMPTY || alternate->mutation_epoch != mutation_epoch) {
		return alternate;
	}

	if (preferred->state == ZEND_OPCACHE_USER_CACHE_LOOKUP_MISS) {
		return preferred;
	}
	if (alternate->state == ZEND_OPCACHE_USER_CACHE_LOOKUP_MISS) {
		return alternate;
	}

	return allow_hit_eviction ? preferred : NULL;
}

static zend_always_inline void zend_opcache_user_cache_lookup_cache_store_miss(
		zend_opcache_user_cache_lookup_entry *lookup_entries,
		zend_ulong hash,
		uint64_t mutation_epoch)
{
	zend_opcache_user_cache_lookup_entry *victim = zend_opcache_user_cache_lookup_cache_select_slot(lookup_entries, hash, mutation_epoch, false);

	if (victim == NULL) {
		return;
	}

	zend_opcache_user_cache_lookup_cache_store(
		victim,
		hash,
		mutation_epoch,
		ZEND_OPCACHE_USER_CACHE_LOOKUP_INVALID_SLOT,
		ZEND_OPCACHE_USER_CACHE_LOOKUP_MISS
	);
}

static zend_always_inline void zend_opcache_user_cache_lookup_cache_store_hit(
		zend_opcache_user_cache_lookup_entry *lookup_entry,
		zend_ulong hash,
		uint64_t mutation_epoch,
		uint32_t slot_index)
{
	zend_opcache_user_cache_lookup_cache_store(
		lookup_entry,
		hash,
		mutation_epoch,
		slot_index,
		ZEND_OPCACHE_USER_CACHE_LOOKUP_HIT
	);
}

#ifdef ZEND_OPCACHE_USER_CACHE_HAVE_OPTIMISTIC
/* Odd sequence values denote an active writer. */
static zend_always_inline uint64_t zend_opcache_user_cache_seq_load(const uint64_t *seq)
{
	return zend_opcache_user_cache_atomic_load_64(seq);
}

static zend_always_inline uint64_t zend_opcache_user_cache_seq_reload(const uint64_t *seq)
{
	zend_opcache_user_cache_atomic_fence_acquire();

	return zend_opcache_user_cache_atomic_load_64(seq);
}

static zend_always_inline void zend_opcache_user_cache_seq_announce(uint64_t *seq, uint64_t value)
{
	zend_opcache_user_cache_atomic_store_64(seq, value);
	zend_opcache_user_cache_atomic_fence_seq_cst();
}

static zend_always_inline void zend_opcache_user_cache_seq_publish(uint64_t *seq, uint64_t value)
{
	zend_opcache_user_cache_atomic_store_64(seq, value);
}
#endif

static zend_always_inline bool zend_opcache_user_cache_header_is_initialized_locked(void)
{
	zend_opcache_user_cache_header *header = zend_opcache_user_cache_header_ptr();

	return header != NULL &&
		header->magic == ZEND_OPCACHE_USER_CACHE_MAGIC &&
		header->version == ZEND_OPCACHE_USER_CACHE_VERSION
	;
}

static zend_always_inline void zend_opcache_user_cache_bump_mutation_epoch_locked(zend_opcache_user_cache_header *header)
{
	if (header == NULL) {
		return;
	}

	header->mutation_epoch++;
	if (header->mutation_epoch == 0) {
		header->mutation_epoch = 1;
	}
}

static zend_always_inline zend_opcache_user_cache_entry *zend_opcache_user_cache_entries(zend_opcache_user_cache_header *header)
{
	return (zend_opcache_user_cache_entry *) ((char *) header + sizeof(zend_opcache_user_cache_header));
}

static zend_always_inline bool zend_opcache_user_cache_key_equals(
		const zend_opcache_user_cache_entry *entry,
		zend_string *key,
		zend_ulong hash)
{
	if (entry->state != ZEND_OPCACHE_USER_CACHE_ENTRY_USED || entry->hash != hash || entry->key_len != ZSTR_LEN(key)) {
		return false;
	}

	return memcmp(zend_opcache_user_cache_ptr(entry->key_offset), ZSTR_VAL(key), ZSTR_LEN(key)) == 0;
}

static zend_always_inline bool zend_opcache_user_cache_value_uses_offset(uint8_t value_type)
{
	return
		value_type == ZEND_OPCACHE_USER_CACHE_VALUE_STRING ||
		value_type == ZEND_OPCACHE_USER_CACHE_VALUE_SHARED_GRAPH
	;
}

static zend_always_inline bool zend_opcache_user_cache_block_is_free(const zend_opcache_user_cache_block *block)
{
	return (block->flags & ZEND_OPCACHE_USER_CACHE_BLOCK_FREE) != 0;
}

static zend_always_inline void zend_opcache_user_cache_block_mark_free(zend_opcache_user_cache_block *block)
{
	block->flags |= ZEND_OPCACHE_USER_CACHE_BLOCK_FREE;
}

static zend_always_inline void zend_opcache_user_cache_block_mark_used(zend_opcache_user_cache_block *block)
{
	block->flags &= ~ZEND_OPCACHE_USER_CACHE_BLOCK_FREE;
	block->next_free = 0;
	block->prev_free = 0;
}

static zend_always_inline void zend_opcache_user_cache_clear_lookup_cache(void)
{
	uint32_t index;

	for (index = 0; index < ZEND_OPCACHE_USER_CACHE_LOOKUP_BUCKETS; index++) {
		zend_opcache_user_cache_lookup_entry_release_key(&zend_opcache_user_cache_lookup_entry_storage[index]);
	}

	memset(zend_opcache_user_cache_lookup_entry_storage, 0, sizeof(zend_opcache_user_cache_lookup_entry) * ZEND_OPCACHE_USER_CACHE_LOOKUP_BUCKETS);
}

static zend_always_inline void zend_opcache_user_cache_lookup_cache_reset_entry(zend_opcache_user_cache_lookup_entry *lookup_entry)
{
	if (lookup_entry == NULL) {
		return;
	}

	zend_opcache_user_cache_lookup_entry_release_key(lookup_entry);

	memset(lookup_entry, 0, sizeof(*lookup_entry));
}

#endif /* ZEND_USER_CACHE_INTERNAL_H */
