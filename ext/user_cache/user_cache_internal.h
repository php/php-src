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
   | Author: Go Kudo <zeriyoshi@php.net>.                                 |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_USER_CACHE_INTERNAL_H
#define PHP_USER_CACHE_INTERNAL_H

#include "php.h"

#include <time.h>
#ifdef ZTS
# include "TSRM/TSRM.h"
#endif
#if defined(ZEND_WIN32) && defined(_MSC_VER)
# include <intrin.h>
#endif
#ifndef ZEND_WIN32
# include <pthread.h>
#endif

#include "Zend/zend_attributes.h"
#include "Zend/zend_ast.h"
#include "Zend/zend_atomic.h"
#include "Zend/zend_call_stack.h"
#include "Zend/zend_enum.h"
#include "Zend/zend_exceptions.h"
#include "Zend/zend_smart_str.h"
#include "Zend/zend_system_id.h"

#include "php_user_cache.h"
#include "user_cache_shm.h"

#include "ext/standard/php_var.h"

#include "SAPI.h"

#define PHP_USER_CACHE_MAGIC 		0xCAC17E01U
#define PHP_USER_CACHE_VERSION 		1U
#define PHP_USER_CACHE_MIN_CAPACITY 127U
#define PHP_USER_CACHE_SLOT_BYTES	256U

#define PHP_USER_CACHE_KEY_DELIMITER		"\x1f"
#define PHP_USER_CACHE_KEY_DELIMITER_CHAR	'\x1f'
#define PHP_USER_CACHE_KEY_DELIMITER_NAME	"0x1F"

#define PHP_USER_CACHE_MSG_RESOURCE_UNSTORABLE		"resources cannot be stored in the user cache"
#define PHP_USER_CACHE_MSG_CLOSURE_UNSTORABLE		"Closure objects cannot be stored in the user cache"
#define PHP_USER_CACHE_MSG_LAZY_OBJECT_UNSTORABLE	"lazy objects cannot be stored in the user cache"
#define PHP_USER_CACHE_MSG_OPAQUE_OBJECT_UNSTORABLE	"objects with opaque internal state (e.g. Fiber, Generator, PDO) cannot be stored in the user cache"

#define PHP_USER_CACHE_ENTRY_EMPTY		0
#define PHP_USER_CACHE_ENTRY_USED		1
#define PHP_USER_CACHE_ENTRY_TOMBSTONE	2

#define PHP_USER_CACHE_VALUE_NULL			0
#define PHP_USER_CACHE_VALUE_TRUE 			1
#define PHP_USER_CACHE_VALUE_FALSE 			2
#define PHP_USER_CACHE_VALUE_LONG 			3
#define PHP_USER_CACHE_VALUE_DOUBLE 		4
#define PHP_USER_CACHE_VALUE_STRING 		5
#define PHP_USER_CACHE_VALUE_SHARED_GRAPH	8

#define PHP_USER_CACHE_SHARED_GRAPH_MAGIC 						0xCAC17E02U
#define PHP_USER_CACHE_SHARED_GRAPH_VERSION 					1U
#define PHP_USER_CACHE_SHARED_GRAPH_FLAG_HAS_SHARED_IDENTITY	0x2U
#define PHP_USER_CACHE_SHARED_GRAPH_FLAG_HAS_OBJECT 			0x4U
#define PHP_USER_CACHE_SHARED_GRAPH_FLAG_PREFERS_PROTOTYPE		0x8U
/* ref_state packs the RETIRED flag (bit 30) with the live refcount (low bits). */
#define PHP_USER_CACHE_SHARED_GRAPH_RETIRED						(1 << 30)
#define PHP_USER_CACHE_SHARED_GRAPH_REFCOUNT_MASK				(PHP_USER_CACHE_SHARED_GRAPH_RETIRED - 1)

#define PHP_USER_CACHE_SHARED_GRAPH_VALUE_UNDEF						0
#define PHP_USER_CACHE_SHARED_GRAPH_VALUE_NULL						1
#define PHP_USER_CACHE_SHARED_GRAPH_VALUE_TRUE						2
#define PHP_USER_CACHE_SHARED_GRAPH_VALUE_FALSE						3
#define PHP_USER_CACHE_SHARED_GRAPH_VALUE_LONG						4
#define PHP_USER_CACHE_SHARED_GRAPH_VALUE_DOUBLE					5
#define PHP_USER_CACHE_SHARED_GRAPH_VALUE_STRING					6
#define PHP_USER_CACHE_SHARED_GRAPH_VALUE_ARRAY						7
#define PHP_USER_CACHE_SHARED_GRAPH_VALUE_OBJECT					8
#define PHP_USER_CACHE_SHARED_GRAPH_VALUE_DYNAMIC_ARRAY				10
#define PHP_USER_CACHE_SHARED_GRAPH_VALUE_OBJECT_REF				11
#define PHP_USER_CACHE_SHARED_GRAPH_VALUE_REFERENCE					13
#define PHP_USER_CACHE_SHARED_GRAPH_VALUE_REFERENCE_REF				14
#define PHP_USER_CACHE_SHARED_GRAPH_VALUE_ARRAY_REF					15
#define PHP_USER_CACHE_SHARED_GRAPH_VALUE_ENUM						16
#define PHP_USER_CACHE_SHARED_GRAPH_VALUE_SAFE_DIRECT_OBJECT		18
#define PHP_USER_CACHE_SHARED_GRAPH_VALUE_SERIALIZED_OBJECT			19
#define PHP_USER_CACHE_SHARED_GRAPH_VALUE_SERDES_OBJECT				20
#define PHP_USER_CACHE_SHARED_GRAPH_VALUE_SLEEP_OBJECT				21
#define PHP_USER_CACHE_SHARED_GRAPH_VALUE_SHAPED_ARRAY				22
#define PHP_USER_CACHE_SHARED_GRAPH_VALUE_SERIALIZED_SHAPED_OBJECT	23
#define PHP_USER_CACHE_SHARED_GRAPH_VALUE_SLEEP_SHAPED_OBJECT		24

#define PHP_USER_CACHE_SHARED_GRAPH_OBJECT_FLAG_SHARED	0x1U

#define PHP_USER_CACHE_SHARED_GRAPH_ARRAY_FLAG_PACKED		0x2U
#define PHP_USER_CACHE_SHARED_GRAPH_ARRAY_SHAPE_MAX_KEYS	8U

#define PHP_USER_CACHE_LOOKUP_BUCKETS		256U
#define PHP_USER_CACHE_LOOKUP_WAYS			2U
#define PHP_USER_CACHE_LOOKUP_SETS			(PHP_USER_CACHE_LOOKUP_BUCKETS / PHP_USER_CACHE_LOOKUP_WAYS)
#define PHP_USER_CACHE_LOOKUP_INVALID_SLOT	UINT32_MAX

#define PHP_USER_CACHE_REQUEST_LOCAL_STRING_MIN_LEN	256U

#define PHP_USER_CACHE_DIRECT_STRING_MIN_LEN		4096U
#define PHP_USER_CACHE_DECODE_DIRECT_CACHE_SLOTS	4U

#define PHP_USER_CACHE_EXPIRED_READ_EXPUNGE_THRESHOLD	64U

/* Advance the bounded expiry scan on write traffic alone: expired entries
 * that are never read again would otherwise wait for allocation pressure. */
#define PHP_USER_CACHE_EXPUNGE_WRITE_OP_INTERVAL		64U

#define PHP_USER_CACHE_EXPUNGE_SCAN_MAX	4096U

#define PHP_USER_CACHE_ENTRY_LOCK_TABLE_SIZE		1024U
#define PHP_USER_CACHE_ENTRY_LOCK_WAIT_TIMEOUT_US	(10U * 1000U * 1000U)
#define PHP_USER_CACHE_ENTRY_LOCK_EMPTY				0
#define PHP_USER_CACHE_ENTRY_LOCK_USED				1
#define PHP_USER_CACHE_ENTRY_LOCK_TOMBSTONE			2

#if defined(__GNUC__) || defined(__clang__)
# define PHP_USER_CACHE_HAVE_OPTIMISTIC	1
#elif defined(ZEND_WIN32) && defined(_MSC_VER)
# define PHP_USER_CACHE_HAVE_OPTIMISTIC	1
# define PHP_USER_CACHE_OPTIMISTIC_MSVC	1
#endif

#if !defined(ZEND_WIN32) && defined(__linux__)
# define PHP_USER_CACHE_HAVE_SHARED_MUTEX	1
#endif

/* Boundary partitions require named shared memory. */
#if !defined(ZEND_WIN32) && defined(PHP_USER_CACHE_USE_SHM_OPEN)
# define PHP_USER_CACHE_HAVE_BOUNDARY_SHM	1
#endif

#if defined(PHP_USER_CACHE_USE_MMAP) && !defined(ZEND_WIN32)
# define PHP_USER_CACHE_HAVE_ANON_MMAP	1
#endif

#define PHP_USER_CACHE_LOCK_MODEL_FCNTL	0U
#define PHP_USER_CACHE_LOCK_MODEL_MUTEX	1U

#define PHP_USER_CACHE_READER_SLOTS				256U
#define PHP_USER_CACHE_READER_DRAIN_SPIN		1024U
#define PHP_USER_CACHE_READER_DRAIN_TIMEOUT_US	10000U
#ifdef PHP_USER_CACHE_HAVE_OPTIMISTIC
# define PHP_USER_CACHE_READER_CLAIM_MAX	4U
#endif

#define PHP_USER_CACHE_ORPHANED_GRAPH_SLOTS	32U

#define PHP_USER_CACHE_GRAPH_PIN_SLOTS				256U
#define PHP_USER_CACHE_GRAPH_PIN_WORDS				(PHP_USER_CACHE_GRAPH_PIN_SLOTS / 32U)
#define PHP_USER_CACHE_GRAPH_PIN_CLAIM_MAX			4U
/* Rate limit for the request-end dead-pin-owner probes. */
#define PHP_USER_CACHE_GRAPH_PIN_PROBE_INTERVAL_SEC	8U

#define PHP_USER_CACHE_LOOKUP_EMPTY	0
#define PHP_USER_CACHE_LOOKUP_HIT	1
#define PHP_USER_CACHE_LOOKUP_MISS	2

#define PHP_USER_CACHE_ENTRY_RESERVED_COMBINED_VALUE_KEY	0x0001U

#define PHP_USER_CACHE_BLOCK_FREE			1U
#define PHP_USER_CACHE_LOOKUP_VALUE_NONE	0xFFU

#ifndef ZEND_WIN32
# define PHP_USER_CACHE_SEM_FILENAME_PREFIX	".PhpUserCacheSem."
#endif

/* Clear the debug-only flag before exposing a decoded table. */
#if ZEND_DEBUG
# define PHP_USER_CACHE_HT_DISALLOW_COW_VIOLATION(ht) HT_FLAGS(ht) &= ~HASH_FLAG_ALLOW_COW_VIOLATION
#else
# define PHP_USER_CACHE_HT_DISALLOW_COW_VIOLATION(ht)
#endif

#ifdef PHP_USER_CACHE_HAVE_OPTIMISTIC
#ifdef PHP_USER_CACHE_OPTIMISTIC_MSVC
# define PHP_USER_CACHE_ATOMIC_LOAD_64(target) \
	((uint64_t) _InterlockedOr64((volatile __int64 *) (target), 0))
# define PHP_USER_CACHE_ATOMIC_STORE_64(target, value) \
	((void) _InterlockedExchange64((volatile __int64 *) (target), (__int64) (value)))
# define PHP_USER_CACHE_ATOMIC_CAS_64(target, expected, desired) \
	((uint64_t) _InterlockedCompareExchange64( \
		(volatile __int64 *) (target), \
		(__int64) (desired), \
		(__int64) (expected) \
	) == (expected))
# define PHP_USER_CACHE_ATOMIC_LOAD_32(target) \
	((uint32_t) _InterlockedOr((volatile long *) (target), 0))
# define PHP_USER_CACHE_ATOMIC_STORE_32(target, value) \
	((void) _InterlockedExchange((volatile long *) (target), (long) (value)))
# define PHP_USER_CACHE_ATOMIC_CAS_32(target, expected, desired) \
	((uint32_t) _InterlockedCompareExchange((volatile long *) (target), (long) (desired), (long) (expected)) == (expected))
# define PHP_USER_CACHE_ATOMIC_FENCE_SEQ_CST()	MemoryBarrier()
# define PHP_USER_CACHE_ATOMIC_FENCE_ACQUIRE()	MemoryBarrier()
#else
# define PHP_USER_CACHE_ATOMIC_LOAD_64(target) \
	__atomic_load_n((target), __ATOMIC_ACQUIRE)
# define PHP_USER_CACHE_ATOMIC_STORE_64(target, value) \
	__atomic_store_n((target), (value), __ATOMIC_RELEASE)
# define PHP_USER_CACHE_ATOMIC_CAS_64(target, expected, desired) \
	__atomic_compare_exchange_n((target), &(expected), (desired), false, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)
# define PHP_USER_CACHE_ATOMIC_LOAD_32(target) \
	__atomic_load_n((target), __ATOMIC_ACQUIRE)
# define PHP_USER_CACHE_ATOMIC_STORE_32(target, value) \
	__atomic_store_n((target), (value), __ATOMIC_RELEASE)
# define PHP_USER_CACHE_ATOMIC_CAS_32(target, expected, desired) \
	__atomic_compare_exchange_n((target), &(expected), (desired), false, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)
# define PHP_USER_CACHE_ATOMIC_FENCE_SEQ_CST()	__atomic_thread_fence(__ATOMIC_SEQ_CST)
# define PHP_USER_CACHE_ATOMIC_FENCE_ACQUIRE()	__atomic_thread_fence(__ATOMIC_ACQUIRE)
#endif
#endif

#ifdef PHP_USER_CACHE_HAVE_SHARED_MUTEX
typedef union {
	pthread_mutex_t mutex;
	char padding[64];
} php_user_cache_shared_mutex;
#endif

typedef struct {
	size_t configured_memory;
	php_user_cache_reason failure_reason;
	bool enabled;
	bool available;
} php_user_cache_runtime;

typedef struct {
	const php_user_cache_shm_handlers *handler;
	php_user_cache_shm_segment **segments;
	int segment_count;
	size_t size;
	bool initialized;
	uint32_t startup_complete;
	bool initialized_before_request;
	bool lock_initialized;
	bool use_shared_mutex;
	int lock_file;
	char lockfile_name[MAXPATHLEN];
	/* Cached for the lifetime of the attached segment. */
	bool layout_memo_valid;
	uint32_t capacity_memo;
	uint32_t data_offset_memo;
#ifdef PHP_USER_CACHE_HAVE_BOUNDARY_SHM
	/* Cached for the lifetime of the partition. */
	bool boundary_digest_memoized;
	uint8_t boundary_digest_memo[32];
#endif
#ifdef ZTS
	MUTEX_T zts_lock;
#endif
} php_user_cache_storage;

typedef struct {
	php_user_cache_storage storage;
	const char *name;
	const char *lock_name;
#ifndef ZEND_WIN32
	const char *sem_filename_prefix;
#endif
	bool clear_on_pressure;
	bool boundary_shared;
	const char *boundary_identity;
	size_t boundary_identity_len;
} php_user_cache_context;

struct _php_user_cache_partition {
	php_user_cache_context context;
	char *name;
	struct _php_user_cache_partition *next;
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
} php_user_cache_entry_lock_record;

typedef struct {
	uint64_t owner_pid;
	uint64_t owner_start_time;
	uint32_t active;
	uint32_t reserved;
} php_user_cache_reader_slot;

/* Per-process record of shared-graph payload pins, claimed lock-free under
 * the read lock or an optimistic reader section. The pid lives in a
 * zend_atomic_int because, unlike reader slots, pin claims must also work on
 * builds without the 64-bit atomic macros; pids fit in 32 bits on every
 * supported platform. Pins of an abnormally terminated owner are stripped by
 * php_user_cache_shared_graph_strip_dead_pins_locked() via pid + start-time
 * liveness, which is what makes retired-but-pinned payloads reclaimable. */
typedef struct {
	zend_atomic_int owner_pid;
	/* Live pins held by this owner. Advisory: a crash window can leave it
	 * higher than the surviving bits, so it only gates liveness probes. */
	zend_atomic_int pin_count;
	uint64_t owner_start_time;
} php_user_cache_graph_pin_slot;

/* Cross-references are offsets from the segment base. */
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
	uint64_t write_seq;
	uint64_t expunge_count;
	uint64_t store_failure_count;
	/* Cumulative dead-pin recovery statistics: owners whose slots the sweep
	 * reclaimed, and individual payload references stripped from them. A
	 * rising value means workers died (SIGKILL/OOM) while holding
	 * shared-graph references. */
	uint64_t graph_dead_pin_owners_reclaimed;
	uint64_t graph_dead_pins_stripped;
	uint32_t tombstone_count;
	uint32_t lock_model;
	uint32_t orphaned_graphs_saturated;
	uint8_t boundary_identity_digest[32];
	uint32_t boundary_identity_digest_set;
	uint32_t orphaned_graphs[PHP_USER_CACHE_ORPHANED_GRAPH_SLOTS];
	php_user_cache_graph_pin_slot graph_pin_slots[PHP_USER_CACHE_GRAPH_PIN_SLOTS];
	php_user_cache_reader_slot reader_slots[PHP_USER_CACHE_READER_SLOTS];
#ifdef PHP_USER_CACHE_HAVE_SHARED_MUTEX
	php_user_cache_shared_mutex global_shared_mutex;
#endif
	php_user_cache_entry_lock_record entry_lock_records[PHP_USER_CACHE_ENTRY_LOCK_TABLE_SIZE];
} php_user_cache_header;

typedef struct {
	uint32_t size;
	uint32_t prev_size;
	uint32_t next_free;
	uint32_t prev_free;
	uint32_t flags;
} php_user_cache_block;

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
} php_user_cache_entry;

typedef struct {
	bool found;
	php_user_cache_entry entry;
} php_user_cache_replaced_entry;

typedef struct {
	zend_ulong hash;
	uint64_t mutation_epoch;
	const void *context;
	uint32_t slot_index;
	uint8_t state;
	uint8_t value_type;
	uint8_t reserved[2];
	zend_string *key;
	zend_long long_value;
	double double_value;
} php_user_cache_lookup_entry;

typedef struct {
	zend_ulong hash;
	size_t payload_size;
	size_t payload_used_size;
	const uint8_t *payload_source;
	uint8_t *owned_buffer;
	zend_string *owned_string;
	/* Owned by the prepared value. */
	uint32_t *fixup_offsets;
	zend_long long_value;
	double double_value;
	HashTable *state_memo;
	uint32_t value_len;
	uint32_t fixup_count;
	uint8_t value_type;
	bool has_verbatim_array;
	uint8_t reserved[2];
} php_user_cache_prepared_value;

typedef struct {
	bool caller_holds_write_lock;
	bool throw_on_failure;
} php_user_cache_prepare_options;

typedef struct {
	bool retry_after_memory_pressure;
	bool throw_on_failure;
	bool capture_replaced_entry;
} php_user_cache_store_options;

typedef struct {
	php_user_cache_replaced_entry replaced_entry;
	uint64_t stored_generation;
	bool should_seed_request_local_slot;
} php_user_cache_store_result;

/* Deferred until after the read lock is released. */
typedef struct {
	uint64_t generation;
	uint32_t flags;
	bool should_seed_request_local_slot;
} php_user_cache_fetch_pending_seed;

typedef struct {
	zend_long new_value;
	bool is_overflow;
	bool is_type_error;
} php_user_cache_atomic_update_result;

typedef struct {
	uint32_t magic;
	uint32_t version;
	uint32_t root_offset;
	uint32_t root_type;
	uint32_t flags;
	zend_atomic_int ref_state;
	/* Bit per graph_pin_slots index: which owners hold ref_state references.
	 * A reference taken without a claimable slot sets no bit and stays
	 * unreclaimable if its owner crashes (the pre-record behavior). */
	zend_atomic_int pin_owners[PHP_USER_CACHE_GRAPH_PIN_WORDS];
} php_user_cache_shared_graph_header;

typedef struct {
	php_user_cache_context *context;
	uint32_t payload_offset;
} php_user_cache_shared_graph_ref;

typedef struct {
	uint8_t type;
	uint8_t reserved[7];
	union {
		zend_long long_value;
		double double_value;
		uint64_t offset;
	} payload;
} php_user_cache_shared_graph_value;

typedef struct {
	uint32_t name_offset;
	/* Declared property index plus one for __sleep state, otherwise zero. */
	uint32_t sleep_state_index;
	php_user_cache_shared_graph_value value;
} php_user_cache_shared_graph_property;

typedef struct {
	zend_ulong h;
	uint32_t key_offset;
	uint32_t reserved;
	php_user_cache_shared_graph_value value;
} php_user_cache_shared_graph_array_element;

typedef struct {
	uint32_t count;
	uint32_t next_free;
	uint32_t elements_offset;
	uint32_t reserved;
} php_user_cache_shared_graph_array;

typedef struct {
	uint32_t key_offset;
} php_user_cache_shared_graph_array_shape_element;

typedef struct {
	uint32_t count;
	uint32_t elements_offset;
} php_user_cache_shared_graph_array_shape;

typedef struct {
	uint32_t count;
	uint32_t next_free;
	uint32_t shape_offset;
	uint32_t reserved;
	uint32_t values_offset;
} php_user_cache_shared_graph_shaped_array;

typedef struct {
	uint32_t class_name_offset;
	uint32_t property_count;
	uint32_t properties_offset;
	uint32_t reserved;
} php_user_cache_shared_graph_object;

typedef struct {
	uint32_t class_name_offset;
	uint32_t shape_offset;
	uint32_t count;
} php_user_cache_shared_graph_state_schema;

typedef struct {
	uint32_t state_schema_offset;
	uint32_t reserved;
	uint32_t state_values_offset;
	uint32_t state_next_free;
} php_user_cache_shared_graph_shaped_state_object;

/* Shared layout for safe-direct and serialized object state. */
typedef struct {
	uint32_t class_name_offset;
	uint32_t property_count;
	uint32_t properties_offset;
	uint32_t reserved;
	php_user_cache_shared_graph_value state;
} php_user_cache_shared_graph_safe_direct_object;

typedef struct {
	uint32_t blob_len;
	uint32_t flags;
} php_user_cache_shared_graph_serdes_object;

typedef struct {
	uint32_t flags;
	uint32_t reserved;
	php_user_cache_shared_graph_value inner;
} php_user_cache_shared_graph_reference;

typedef struct {
	uint32_t class_name_offset;
	uint32_t case_name_offset;
} php_user_cache_shared_graph_enum;


typedef enum {
	PHP_USER_CACHE_OPTIMISTIC_FALLBACK = 0,
	PHP_USER_CACHE_OPTIMISTIC_FOUND,
	PHP_USER_CACHE_OPTIMISTIC_MISS
} php_user_cache_optimistic_result;

/* Result of the root-array verbatim sizing attempt. */
typedef enum {
	PHP_USER_CACHE_VERBATIM_ROOT_UNDECIDED = 0,
	PHP_USER_CACHE_VERBATIM_ROOT_SIZED,
	PHP_USER_CACHE_VERBATIM_ROOT_ELIGIBLE_UNSIZED,
	PHP_USER_CACHE_VERBATIM_ROOT_INELIGIBLE
} php_user_cache_verbatim_root_result;

#ifdef PHP_USER_CACHE_HAVE_OPTIMISTIC
typedef struct {
	php_user_cache_header *header;
	uint32_t slot_index;
} php_user_cache_reader_claim;
#endif

typedef struct {
	php_user_cache_header *header;
	uint32_t slot_index;
} php_user_cache_graph_pin_claim;

typedef struct {
	php_user_cache_partition *active_partition;
	php_user_cache_runtime runtime_state;
	php_user_cache_context *active_context_ptr;
	php_user_cache_reason request_unavailable_reason;
	bool lock_held;
	bool lock_held_is_write;
	bool runtime_resolved;
	bool runtime_resolved_enabled;
	const php_user_cache_context *runtime_resolved_context;
	php_user_cache_shared_graph_ref *shared_graph_refs;
	uint32_t shared_graph_ref_count;
	uint32_t shared_graph_ref_capacity;
	uint64_t shared_graph_ref_owner_pid;
	HashTable *shared_graph_ref_index;
	php_user_cache_lookup_entry lookup_entry_storage[PHP_USER_CACHE_LOOKUP_BUCKETS];
	HashTable *request_local_slot_table;
	HashTable *entry_lock_table;
	HashTable *pool_table;
	HashTable *decode_identity_map;
	HashTable *decode_reference_map;
	HashTable *decode_array_map;
	HashTable *decode_resolve_cache;
	HashTable *decode_shape_prototype_cache;
	/* Request-local class route cache. */
	HashTable *object_route_memo;
	const void *decode_resolve_direct_keys[PHP_USER_CACHE_DECODE_DIRECT_CACHE_SLOTS];
	void *decode_resolve_direct_values[PHP_USER_CACHE_DECODE_DIRECT_CACHE_SLOTS];
	const void *decode_shape_prototype_direct_keys[PHP_USER_CACHE_DECODE_DIRECT_CACHE_SLOTS];
	zend_array *decode_shape_prototype_direct_values[PHP_USER_CACHE_DECODE_DIRECT_CACHE_SLOTS];
	uint8_t decode_resolve_direct_next;
	uint8_t decode_shape_prototype_direct_next;
#ifndef ZEND_WIN32
	zend_ulong entry_lock_owner_pid;
#endif
	bool write_seq_bumped;
	bool stack_overflowed;
	/* Set while a bulk store commits its items so a per-item bailout keeps the
	 * write lock held: the bulk rollback then runs without a lock gap another
	 * process could mutate through. */
	bool store_defer_unlock;
	/* Request shutdown must collect cyclic slot clones. */
	bool request_local_slot_may_cycle;
#ifdef PHP_USER_CACHE_HAVE_OPTIMISTIC
	int8_t reader_drain_state;
	php_user_cache_reader_claim reader_claims[PHP_USER_CACHE_READER_CLAIM_MAX];
	uint32_t reader_claim_count;
#endif
	php_user_cache_graph_pin_claim graph_pin_claims[PHP_USER_CACHE_GRAPH_PIN_CLAIM_MAX];
	uint32_t graph_pin_claim_count;
	/* Rate limit for request-end dead-pin sweeps. */
	uint64_t graph_pin_probe_last_at;
	uint32_t expired_read_observations;
	/* Write operations since the last bounded expiry scan. */
	uint32_t expunge_write_ops;
	/* Process-local resume point for bounded expiry scans. */
	uint32_t expired_expunge_cursor;
	uint64_t entry_lock_owner_probe_pid;
	uint64_t entry_lock_owner_probe_start_time;
	uint64_t entry_lock_owner_probe_at;
	bool entry_lock_owner_probe_dead;
	/* Recomputed after fork. */
	uint64_t self_start_time_pid;
	uint64_t self_start_time_token;
	bool enable;
	bool enable_cli;
	zend_long shm_size;
	char *lockfile_path;
	char *memory_model;
} php_user_cache_globals;

#ifdef ZTS
# define UC_G(v) ZEND_TSRMG_FAST(user_cache_globals_offset, php_user_cache_globals *, v)
extern int user_cache_globals_id;
extern size_t user_cache_globals_offset;
#else
# define UC_G(v) (user_cache_globals.v)
extern php_user_cache_globals user_cache_globals;
#endif

extern zend_class_entry *php_user_cache_exception_ce;
extern zend_class_entry *php_user_cache_status_ce;
extern zend_class_entry *php_user_cache_pool_status_ce;
extern php_user_cache_context php_user_cache_context_state;
extern bool php_user_cache_runtime_opted_in;
extern php_user_cache_partition *php_user_cache_partitions;

uint64_t php_user_cache_cached_pid(void);
void php_user_cache_reset_runtime(void);
void php_user_cache_reset_storage(void);
bool php_user_cache_header_init_locked(void);
void php_user_cache_free_locked(uint32_t payload_offset);
uint32_t php_user_cache_alloc_locked(size_t size, const void *src);
bool php_user_cache_startup_storage_before_request(void);
void php_user_cache_shutdown_storage(void);
void php_user_cache_ensure_ready_impl(void);
bool php_user_cache_rlock(void);
bool php_user_cache_wlock(void);
bool php_user_cache_wlock_for_entry_mutation(zend_string *key);
void php_user_cache_unlock(void);
void php_user_cache_unlock_if_held(void);
bool php_user_cache_try_acquire_entry_lock(zend_string *key, zend_long lease);
bool php_user_cache_release_entry_lock(zend_string *key);
/* Keys must be sorted with duplicates adjacent. */
bool php_user_cache_acquire_entry_locks(zend_string **keys, bool *acquired, uint32_t count);
void php_user_cache_release_entry_locks(zend_string **keys, const bool *acquired, uint32_t count);
bool php_user_cache_entry_locks_allow_clear_locked(void);
void php_user_cache_release_request_entry_locks(void);
const php_user_cache_safe_direct_handlers *php_user_cache_safe_direct_find_handlers(
	zend_class_entry *ce,
	zend_class_entry **base_ce_ptr
);
php_user_cache_safe_direct_state_copy_func_t php_user_cache_safe_direct_state_copy_func(
	zend_class_entry *ce,
	zend_class_entry **base_ce_ptr
);
zend_class_entry *php_user_cache_safe_direct_find_base(zend_class_entry *ce);
php_user_cache_safe_direct_state_has_unstorable_func_t php_user_cache_safe_direct_state_has_unstorable_func(
	zend_class_entry *ce
);
php_user_cache_safe_direct_state_serialize_func_t php_user_cache_safe_direct_state_serialize_func(
	zend_class_entry *ce
);
php_user_cache_safe_direct_state_unserialize_func_t php_user_cache_safe_direct_state_unserialize_func(
	zend_class_entry *ce
);
bool php_user_cache_safe_direct_prefers_request_local_prototype(zend_class_entry *ce);
bool php_user_cache_serdes_encode(zval *value, smart_str *buf, const char **failure_msg);
bool php_user_cache_serdes_decode(const uint8_t *data, size_t len, zval *dst);
uint32_t php_user_cache_serdes_declared_property_index_plus_one(
	zend_class_entry *ce,
	zend_string *name
);
bool php_user_cache_serdes_get_sleep_state(
	zval *obj_zv,
	zval *state,
	const char **failure_msg
);
bool php_user_cache_serdes_call_magic_serialize(zend_object *obj, zval *state);
bool php_user_cache_shared_graph_update_object_property(
	zval *obj_zv,
	zend_string *prop_name,
	zval *prop_val
);
bool php_user_cache_shared_graph_update_object_property_at(
	zval *obj_zv,
	zend_string *prop_name,
	uint32_t prop_idx,
	zval *prop_val
);
bool php_user_cache_shared_graph_can_copy_verbatim_root(const zval *value, HashTable *verbatim_verdicts);
php_user_cache_verbatim_root_result php_user_cache_shared_graph_calc_verbatim_root(const zval *value, HashTable *verbatim_verdicts, size_t *buf_len);
bool php_user_cache_calculate_shared_graph_size(const zval *value, HashTable *state_memo, HashTable *verbatim_verdicts, size_t *buf_len);
bool php_user_cache_build_shared_graph_in_place(const zval *value, HashTable *state_memo, const HashTable *verbatim_verdicts, uint8_t *buf, size_t buf_len, size_t *graph_len, bool *has_verbatim_array, uint32_t **fixup_offsets, uint32_t *fixup_count);
bool php_user_cache_shared_graph_copy_fits_buffer(
	const uint8_t *dst_buf,
	size_t dst_buf_len,
	const uint8_t *src_buf,
	size_t src_buf_len,
	size_t src_graph_len
);
bool php_user_cache_shared_graph_decode(const uint8_t *buf, size_t buf_len, zval *dst);
bool php_user_cache_shared_graph_prefers_prototype(uint32_t payload_offset);
bool php_user_cache_shared_graph_decode_is_lock_safe(uint32_t payload_offset);
bool php_user_cache_shared_graph_payload_has_aliases(uint32_t payload_offset);
void php_user_cache_decode_resolve_cache_release(void);
void php_user_cache_decode_shape_prototype_cache_release(void);
void php_user_cache_decode_maps_teardown(void);
bool php_user_cache_shared_graph_can_overwrite_payload_locked(uint32_t payload_offset);
bool php_user_cache_shared_graph_publish_copied_payload_locked(
	uint8_t *dst_buf,
	size_t dst_buf_len,
	const uint8_t *src_buf,
	size_t src_buf_len,
	size_t src_graph_len,
	bool has_verbatim_array,
	const uint32_t *fixup_offsets,
	uint32_t fixup_count
);
bool php_user_cache_shared_graph_acquire_ref(uint32_t payload_offset);
bool php_user_cache_shared_graph_retire_payload_locked(uint32_t payload_offset);
bool php_user_cache_shared_graph_release_ref_locked(uint32_t payload_offset);
bool php_user_cache_has_request_shared_graph_ref(uint32_t payload_offset);
void php_user_cache_register_shared_graph_ref(uint32_t payload_offset);
bool php_user_cache_release_request_shared_graph_refs(void);
void php_user_cache_expunge_expired_at_request_end(void);
bool php_user_cache_clear_locked(void);
void php_user_cache_lookup_cache_clear(void);
bool php_user_cache_prepare_value(
	zend_string *key,
	zval *value,
	const php_user_cache_prepare_options *options,
	php_user_cache_prepared_value *prepared
);
void php_user_cache_destroy_prepared_value(php_user_cache_prepared_value *prepared);
bool php_user_cache_store_prepared_locked(
	zend_string *key,
	zval *value,
	const php_user_cache_prepared_value *prepared,
	zend_long ttl,
	const php_user_cache_store_options *options,
	php_user_cache_store_result *result
);
bool php_user_cache_fetch_locked(
	zend_string *key,
	bool throw_if_missing,
	bool use_request_local_slot,
	zval *return_value,
	bool *found,
	php_user_cache_fetch_pending_seed *pending_seed
);
void php_user_cache_fetch_finish(zend_string *key, uint64_t gen, zval *return_value, uint32_t flags);
bool php_user_cache_exists_locked(zend_string *key);
void php_user_cache_delete_locked(zend_string *key);
void php_user_cache_discard_replaced_entry_locked(zend_string *key, php_user_cache_replaced_entry *replaced_entry);
void php_user_cache_rollback_replaced_entry_locked(zend_string *key, php_user_cache_replaced_entry *replaced_entry);
void php_user_cache_delete_by_prefix_locked(zend_string *prefix);
bool php_user_cache_atomic_update_locked(
	zend_string *key,
	zend_long step,
	zend_long ttl,
	bool decrement,
	bool insert_if_missing,
	php_user_cache_atomic_update_result *result
);
void php_user_cache_release_request_local_slots(void);
void php_user_cache_release_active_request_local_slots(void);
void php_user_cache_release_active_request_local_slots_by_prefix(zend_string *prefix);
void php_user_cache_store_request_local_slot(zend_string *key, uint64_t gen, zval *value, bool no_aliases);
void php_user_cache_object_table_dtor(zval *zv);
void php_user_cache_reference_table_dtor(zval *zv);
php_user_cache_optimistic_result php_user_cache_fetch_optimistic(
	zend_string *key,
	zval *return_value
);
php_user_cache_optimistic_result php_user_cache_exists_optimistic(zend_string *key);
bool php_user_cache_optimistic_reader_begin(php_user_cache_header *header, uint32_t *slot_idx_ptr);
void php_user_cache_optimistic_reader_end(php_user_cache_header *header, uint32_t slot_idx);
void php_user_cache_optimistic_fork_setup(void);
#if defined(ZTS) && defined(PHP_USER_CACHE_HAVE_OPTIMISTIC)
void php_user_cache_release_thread_reader_claims(php_user_cache_globals *globals);
#endif
bool php_user_cache_quiesce_graph_payloads_locked(void);
void php_user_cache_shared_graph_ref_reserve(void);
void php_user_cache_shared_graph_orphan_payload_locked(uint32_t payload_offset);
void php_user_cache_shared_graph_reclaim_orphaned_locked(void);
bool php_user_cache_shared_graph_strip_dead_pins_locked(bool force);
bool php_user_cache_graph_pin_owner_is_dead(uint64_t owner_pid, uint64_t owner_start_time);
uint64_t php_user_cache_self_start_time_token(void);
#ifdef ZTS
void php_user_cache_release_thread_graph_pin_claims(php_user_cache_globals *globals);
#endif

static zend_always_inline uint64_t php_user_cache_current_pid(void)
{
#ifdef ZEND_WIN32
	return (uint64_t) GetCurrentProcessId();
#else
	return (uint64_t) getpid();
#endif
}

static zend_always_inline bool php_user_cache_stack_overflowed(void)
{
#ifdef ZEND_CHECK_STACK_LIMIT
	bool overflowed = UNEXPECTED(zend_call_stack_overflowed(EG(stack_limit)));

	/* Latch the failure for the rest of the request. */
	if (overflowed) {
		UC_G(stack_overflowed) = true;
	}

	return overflowed;
#else
	return false;
#endif
}

#ifdef PHP_USER_CACHE_HAVE_OPTIMISTIC
static zend_always_inline uint64_t php_user_cache_atomic_load_64(const uint64_t *target)
{
	return PHP_USER_CACHE_ATOMIC_LOAD_64(target);
}

static zend_always_inline void php_user_cache_atomic_fence_acquire(void)
{
	PHP_USER_CACHE_ATOMIC_FENCE_ACQUIRE();
}
#endif

static zend_always_inline php_user_cache_context *php_user_cache_owning_context(void)
{
	return UC_G(active_partition) != NULL
		? &UC_G(active_partition)->context
		: &php_user_cache_context_state
	;
}

static zend_always_inline php_user_cache_context *php_user_cache_active_context(void)
{
	if (UC_G(active_context_ptr) != NULL) {
		return UC_G(active_context_ptr);
	}

	return php_user_cache_owning_context();
}

static zend_always_inline void *php_user_cache_base(void)
{
	php_user_cache_storage *storage = &php_user_cache_active_context()->storage;

	if (!storage->initialized ||
		storage->segment_count != 1
	) {
		return NULL;
	}

	return storage->segments[0]->p;
}

static zend_always_inline php_user_cache_header *php_user_cache_header_ptr(void)
{
	return (php_user_cache_header *) php_user_cache_base();
}

static zend_always_inline uint8_t *php_user_cache_ptr(uint32_t offset)
{
	return (uint8_t *) php_user_cache_base() + offset;
}

static zend_always_inline php_user_cache_block *php_user_cache_block_ptr(uint32_t offset)
{
	return (php_user_cache_block *) php_user_cache_ptr(offset);
}

static zend_always_inline uint32_t php_user_cache_block_payload_capacity(uint32_t payload_offset)
{
	php_user_cache_block *block;

	if (payload_offset < sizeof(php_user_cache_block)) {
		return 0;
	}

	block = php_user_cache_block_ptr(payload_offset - sizeof(php_user_cache_block));
	if (block->size < sizeof(php_user_cache_block)) {
		return 0;
	}

	return block->size - (uint32_t) sizeof(php_user_cache_block);
}

static zend_always_inline bool php_user_cache_block_is_free(const php_user_cache_block *block)
{
	return (block->flags & PHP_USER_CACHE_BLOCK_FREE) != 0;
}

static zend_always_inline php_user_cache_context *php_user_cache_activate_context(php_user_cache_context *context)
{
	php_user_cache_context *previous = UC_G(active_context_ptr);

	UC_G(active_context_ptr) = context;

	return previous;
}

static zend_always_inline void php_user_cache_restore_context(php_user_cache_context *context)
{
	UC_G(active_context_ptr) = context;
}

static zend_always_inline php_user_cache_runtime *php_user_cache_active_runtime(void)
{
	return &UC_G(runtime_state);
}

static zend_always_inline uint64_t php_user_cache_seq_load(const uint64_t *seq)
{
#ifdef PHP_USER_CACHE_HAVE_OPTIMISTIC
	return php_user_cache_atomic_load_64(seq);
#else
	return *seq;
#endif
}

static zend_always_inline uint64_t php_user_cache_seq_reload(const uint64_t *seq)
{
#ifdef PHP_USER_CACHE_HAVE_OPTIMISTIC
	php_user_cache_atomic_fence_acquire();

	return php_user_cache_atomic_load_64(seq);
#else
	return *seq;
#endif
}

static zend_always_inline bool php_user_cache_header_is_initialized_locked(void)
{
	php_user_cache_header *header = php_user_cache_header_ptr();

	return header != NULL &&
		header->magic == PHP_USER_CACHE_MAGIC &&
		header->version == PHP_USER_CACHE_VERSION
	;
}

static zend_always_inline void php_user_cache_bump_mutation_epoch_locked(php_user_cache_header *header)
{
	if (header == NULL) {
		return;
	}

	header->mutation_epoch++;
	if (header->mutation_epoch == 0) {
		header->mutation_epoch = 1;
	}
}

static zend_always_inline php_user_cache_entry *php_user_cache_entries_ptr(php_user_cache_header *header)
{
	return (php_user_cache_entry *) ((char *) header + sizeof(php_user_cache_header));
}

static zend_always_inline bool php_user_cache_seen_test_and_add(HashTable *seen, const void *ptr)
{
	zend_ulong key = (zend_ulong) (uintptr_t) ptr;

	if (zend_hash_index_exists(seen, key)) {
		return false;
	}

	return zend_hash_index_add_empty_element(seen, key) != NULL;
}

static zend_always_inline void php_user_cache_ensure_ready(void)
{
	php_user_cache_context *ctx = php_user_cache_active_context();

	if (UC_G(runtime_resolved) &&
		UC_G(runtime_resolved_context) == ctx &&
		UC_G(runtime_resolved_enabled) == UC_G(enable)
	) {
		return;
	}

	php_user_cache_ensure_ready_impl();
}

static inline bool php_user_cache_class_overrides_safe_direct_magic_serialize_ex(
		zend_class_entry *ce,
		const php_user_cache_safe_direct_handlers *handlers,
		zend_class_entry *base_ce)
{
	if (ce->type != ZEND_USER_CLASS ||
		ce->__serialize == NULL ||
		ce->__unserialize == NULL ||
		(ce->ce_flags & (ZEND_ACC_NOT_SERIALIZABLE|ZEND_ACC_ENUM)) != 0 ||
		handlers == NULL ||
		base_ce == ce
	) {
		return false;
	}

	return
		(
			ce->__serialize->type == ZEND_USER_FUNCTION ||
			ce->__unserialize->type == ZEND_USER_FUNCTION
		) &&
		(
			ce->__serialize->common.scope != base_ce ||
			ce->__unserialize->common.scope != base_ce
		)
	;
}

static inline bool php_user_cache_class_has_safe_direct_state_funcs(zend_class_entry *ce)
{
	/* Registered as an all-or-nothing set. */
	return php_user_cache_safe_direct_find_handlers(ce, NULL) != NULL;
}

static inline bool php_user_cache_class_magic_route_active(zend_class_entry *ce)
{
	const php_user_cache_safe_direct_handlers *handlers;
	zend_class_entry *base_ce = NULL;

	if (ce->ce_flags & (ZEND_ACC_NOT_SERIALIZABLE|ZEND_ACC_ENUM)) {
		return false;
	}

	handlers = php_user_cache_safe_direct_find_handlers(ce, &base_ce);

	if (php_user_cache_class_overrides_safe_direct_magic_serialize_ex(ce, handlers, base_ce)) {
		return true;
	}

	return handlers == NULL;
}

static inline bool php_user_cache_class_uses_magic_serialize(zend_class_entry *ce)
{
	return ce->__serialize != NULL && ce->__unserialize != NULL &&
		php_user_cache_class_magic_route_active(ce)
	;
}

static inline bool php_user_cache_class_uses_serialize_props(zend_class_entry *ce)
{
	/* Native serialization restores this state as properties. */
	return ce->__serialize != NULL && ce->__unserialize == NULL &&
		php_user_cache_class_magic_route_active(ce)
	;
}

static inline bool php_user_cache_class_uses_magic_unserialize(zend_class_entry *ce)
{
	if (ce->__serialize != NULL || ce->__unserialize == NULL) {
		return false;
	}

	/* Match native serialization precedence. */
	if (ce->serialize != NULL && ce->unserialize != NULL) {
		return false;
	}

	return php_user_cache_class_magic_route_active(ce);
}

static inline bool php_user_cache_class_uses_serdes(zend_class_entry *ce)
{
	if (ce->ce_flags & (ZEND_ACC_NOT_SERIALIZABLE|ZEND_ACC_ENUM)) {
		return false;
	}

	if (php_user_cache_class_uses_magic_serialize(ce) ||
		php_user_cache_class_uses_magic_unserialize(ce) ||
		php_user_cache_class_has_safe_direct_state_funcs(ce)
	) {
		return false;
	}

	if (zend_hash_find_known_hash(&ce->function_table, ZSTR_KNOWN(ZEND_STR_SLEEP)) != NULL ||
		zend_hash_find_known_hash(&ce->function_table, ZSTR_KNOWN(ZEND_STR_WAKEUP)) != NULL
	) {
		return true;
	}

	/* Honor native and Serializable handlers. */
	return ce->serialize != NULL &&
		ce->unserialize != NULL
	;
}

#endif /* PHP_USER_CACHE_INTERNAL_H */
