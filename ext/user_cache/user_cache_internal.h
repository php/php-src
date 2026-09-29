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

#ifndef UCACHE_INTERNAL_H
#define UCACHE_INTERNAL_H

#include "php.h"

#include <stdatomic.h>
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

#include "Zend/zend_bitset.h"
#include "Zend/zend_call_stack.h"
#include "Zend/zend_enum.h"
#include "Zend/zend_exceptions.h"
#include "Zend/zend_hrtime.h"
#include "Zend/zend_smart_str.h"
#include "Zend/zend_system_id.h"

#include "php_user_cache.h"
#include "user_cache_shm.h"

#include "ext/standard/php_var.h"

#include "SAPI.h"

#if ZEND_DEBUG
# define UCACHE_DEBUG_FAULT(name) ucache_debug_fault("USER_CACHE_DEBUG_" name)
# define UCACHE_DEBUG_SIMULATE_KILL(name) \
	do { \
		if (UCACHE_DEBUG_FAULT(name)) { \
			_Exit(0); \
		} \
	} while (0)
#else
# define UCACHE_DEBUG_FAULT(name) false
# define UCACHE_DEBUG_SIMULATE_KILL(name) ((void) 0)
#endif

#define UCACHE_MAGIC						0xCAC17E01U
#define UCACHE_MIN_CAPACITY					127U
#define UCACHE_TABLE_SLOT_SIZE \
	(sizeof(ucache_entry) + sizeof(uint32_t) + sizeof(ucache_pool_links))
#define UCACHE_AUTO_SEG_BYTES_PER_ENTRY		1024U
#define UCACHE_ENTRIES_HINT_MAX				16777213

#define UCACHE_REQ_CACHE_BUDGET_MIN			(8U * 1024U * 1024U)
#define UCACHE_REQ_CACHE_BUDGET_DIVISOR		8U

#define UCACHE_STORAGE_KEY_MAX		UINT16_MAX
#define UCACHE_POOL_NAME_MAX		(UCACHE_STORAGE_KEY_MAX - 1 - MAX_LENGTH_OF_LONG)

#define UCACHE_KEY_DELIM		"\x1f"
#define UCACHE_KEY_DELIM_CHAR	'\x1f'
#define UCACHE_KEY_DELIM_NAME	"0x1F"

#define UCACHE_MSG_RESOURCE_UNSTORABLE		"Resources cannot be stored in the user cache"
#define UCACHE_MSG_OBJ_UNSTORABLE			"%s objects cannot be stored in the user cache"
#define UCACHE_MSG_LAZY_OBJ_UNSTORABLE		"Uninitialized lazy objects cannot be stored in the user cache"
#define UCACHE_MSG_NESTED_TOO_DEEPLY		"Value is nested too deeply to be stored in the user cache"

#define UCACHE_ENTRY_EMPTY		0
#define UCACHE_ENTRY_TOMBSTONE	1
#define UCACHE_ENTRY_USED		2

#define UCACHE_VAL_NULL		0
#define UCACHE_VAL_TRUE		1
#define UCACHE_VAL_FALSE	2
#define UCACHE_VAL_LONG		3
#define UCACHE_VAL_DOUBLE	4
#define UCACHE_VAL_STR		5
#define UCACHE_VAL_SGRAPH	6

#define UCACHE_PREPARED_OWNER_BUF	1
#define UCACHE_PREPARED_OWNER_STR	2

#define UCACHE_SGRAPH_FLAG_EMPTY_ROOT			0x1U
#define UCACHE_SGRAPH_FLAG_HAS_SHARED_IDENTITY	0x2U
#define UCACHE_SGRAPH_FLAG_HAS_OBJ				0x4U
#define UCACHE_SGRAPH_FLAG_PREFERS_PROTO		0x8U
#define UCACHE_SGRAPH_REF_STATE_RETIRED			(1 << 30)
#define UCACHE_SGRAPH_REF_STATE_REFCOUNT_MASK	(UCACHE_SGRAPH_REF_STATE_RETIRED - 1)

#define UCACHE_SGRAPH_VAL_UNDEF					0
#define UCACHE_SGRAPH_VAL_NULL					1
#define UCACHE_SGRAPH_VAL_TRUE					2
#define UCACHE_SGRAPH_VAL_FALSE					3
#define UCACHE_SGRAPH_VAL_LONG					4
#define UCACHE_SGRAPH_VAL_LONG_WIDE				5
#define UCACHE_SGRAPH_VAL_DOUBLE				6
#define UCACHE_SGRAPH_VAL_STR					7
#define UCACHE_SGRAPH_VAL_ARR					8
#define UCACHE_SGRAPH_VAL_OBJ					9
#define UCACHE_SGRAPH_VAL_DYNAMIC_ARR			10
#define UCACHE_SGRAPH_VAL_OBJ_REF				11
#define UCACHE_SGRAPH_VAL_REF					12
#define UCACHE_SGRAPH_VAL_REF_REF				13
#define UCACHE_SGRAPH_VAL_ENUM					14
#define UCACHE_SGRAPH_VAL_SAFE_DIRECT_OBJ		15
#define UCACHE_SGRAPH_VAL_SERIALIZED_OBJ		16
#define UCACHE_SGRAPH_VAL_SERDES_OBJ			17
#define UCACHE_SGRAPH_VAL_SLEEP_OBJ				18
#define UCACHE_SGRAPH_VAL_SHAPED_ARR			19
#define UCACHE_SGRAPH_VAL_SERIALIZED_SHAPED_OBJ	20
#define UCACHE_SGRAPH_VAL_SLEEP_SHAPED_OBJ		21

#define UCACHE_SGRAPH_OBJ_FLAG_SHARED	0x1U

#define UCACHE_SGRAPH_ELEM_STR_KEY	0x1U

#define UCACHE_SGRAPH_ARR_FLAG_PACKED			0x1U
#define UCACHE_SGRAPH_ARR_FLAG_WIDE_NEXT_FREE	0x2U
#define UCACHE_SGRAPH_ARR_FLAG_PACKED_VALS		0x4U
#define UCACHE_SGRAPH_ARR_SHAPE_MAX_KEYS		8U

#define UCACHE_DECODE_DIRECT_CACHE_SLOTS	4U

#define UCACHE_CLOCK_TICKS_PER_SEC	16U
#define UCACHE_CLOCK_TICK_NS		(ZEND_NANO_IN_SEC / UCACHE_CLOCK_TICKS_PER_SEC)
#define UCACHE_CLOCK_SPAN_SEC		(UINT32_MAX / UCACHE_CLOCK_TICKS_PER_SEC)

#define UCACHE_EXPIRY_FLOOR_NONE	UINT32_MAX

#define UCACHE_EVICTION_POLICY_LRU		0
#define UCACHE_EVICTION_POLICY_CLEAR	1
#define UCACHE_EVICTION_POLICY_NONE		2

#define UCACHE_ENTRY_LOCK_MIN_CAPACITY		128U
#define UCACHE_ENTRY_LOCK_EMPTY				0
#define UCACHE_ENTRY_LOCK_USED				1
#define UCACHE_ENTRY_LOCK_TOMBSTONE			2

/* libatomic's fallback locks are per process; shared memory needs lock-free atomics. */
#if (defined(__GNUC__) || defined(__clang__)) && \
	defined(__GCC_ATOMIC_LLONG_LOCK_FREE) && __GCC_ATOMIC_LLONG_LOCK_FREE == 2
# define UCACHE_HAVE_OPTIMISTIC	1
#elif defined(ZEND_WIN32) && defined(_MSC_VER)
# define UCACHE_HAVE_OPTIMISTIC	1
# define UCACHE_OPTIMISTIC_MSVC	1
#endif

#if !defined(ZEND_WIN32) && defined(HAVE_PTHREAD_MUTEXATTR_SETPSHARED) && \
	defined(HAVE_PTHREAD_MUTEXATTR_SETROBUST) && defined(HAVE_PTHREAD_MUTEX_CONSISTENT) && \
	(defined(HAVE_PTHREAD_MUTEX_CLOCKLOCK) || defined(HAVE_PTHREAD_MUTEX_TIMEDLOCK))
# define UCACHE_HAVE_SHARED_MUTEX	1
#endif

#if defined(UCACHE_HAVE_SHARED_MUTEX) && defined(UCACHE_HAVE_OPTIMISTIC)
# define UCACHE_HAVE_SCALAR_WRITE	1
#endif

#ifdef UCACHE_USE_SHM_OPEN
# define UCACHE_HAVE_BOUNDARY_SHM	1
# define UCACHE_BOUNDARY_SALT_SIZE	32
#endif

#define UCACHE_MAX_BOUNDARY_PARTITIONS	32U

#define UCACHE_LOCK_MODEL_FCNTL	0U
#define UCACHE_LOCK_MODEL_MUTEX	1U

#define UCACHE_CPU_CACHE_LINE_SIZE	64U

#define UCACHE_SCALAR_WRITE_STRIPES	8U

#define UCACHE_READER_SLOTS				256U
#define UCACHE_READER_CLAIM_MAX			4U

#define UCACHE_OWNER_PROBES				16U

#define UCACHE_ORPHANED_GRAPH_SLOTS	32U

#define UCACHE_GRAPH_PIN_SLOTS_MAX			256U
#define UCACHE_GRAPH_PIN_WORDS_MAX			(UCACHE_GRAPH_PIN_SLOTS_MAX / 32U)
#define UCACHE_GRAPH_PIN_CLAIM_MAX			4U
#define UCACHE_GRAPH_PIN_OWNER_ABANDONED	(-2)

#define UCACHE_RECORD_EMPTY	0
#define UCACHE_RECORD_HIT	1
#define UCACHE_RECORD_MISS	2

#define UCACHE_RECORD_VAL_NONE			0
#define UCACHE_RECORD_VAL_COPY			1
#define UCACHE_RECORD_VAL_PINNED		2
#define UCACHE_RECORD_VAL_PINNED_COPY	(UCACHE_RECORD_VAL_PINNED | UCACHE_RECORD_VAL_COPY)

#define UCACHE_INLINE_KEY_RECORDS		4U

#define UCACHE_ENTRY_FLAG_COMBINED_VAL_KEY		0x0001U
#define UCACHE_ENTRY_KIND_SHIFT					4U
#define UCACHE_ENTRY_KIND_MASK					0x00f0U
#define UCACHE_ENTRY_KIND_USED_BITS				0x00e0U
#define UCACHE_ENTRY_POOL_BUCKET_SHIFT			8U
#define UCACHE_ENTRY_POOL_BUCKET_MASK			0xff00U

#define UCACHE_POOL_BUCKETS	256U

#define UCACHE_BLOCK_FREE			1U
#define UCACHE_BLOCK_PREV_FREE		2U
#define UCACHE_BLOCK_FLAGS			(UCACHE_BLOCK_FREE | UCACHE_BLOCK_PREV_FREE)
#define UCACHE_BLOCK_MIN_SIZE		24U
#define UCACHE_BLOCK_OWNER_NONE		UINT32_MAX
#define UCACHE_BLOCK_PAYLOAD_ALIGNMENT	MIN(UCACHE_PLATFORM_ALIGNMENT, sizeof(ucache_block))
#define UCACHE_BLOCK_SIZE_MAX		0xFE000000U
#if ZEND_DEBUG
# define UCACHE_DEBUG_BLOCK_MERGE_LIMIT	(64U * 1024U)
#endif
#define UCACHE_BLOCK_HDR_UNITS		1U

#define UCACHE_OFFSET_SHIFT			3U
#define UCACHE_OFFSET_UNIT			(1U << UCACHE_OFFSET_SHIFT)
#define UCACHE_SEG_SIZE_MAX			((uint64_t) UINT32_MAX << UCACHE_OFFSET_SHIFT)
#define UCACHE_SHM_SIZE_MAX \
	MIN(UCACHE_SEG_SIZE_MAX, (uint64_t) (MIN((uint64_t) SIZE_MAX, (uint64_t) ZEND_LONG_MAX) & ~(uint64_t) (UCACHE_OFFSET_UNIT - 1)))

#define UCACHE_ENTRY_KEY_BYTE_SHIFT	1U
#define UCACHE_ENTRY_KEY_BYTE_MASK	((UCACHE_OFFSET_UNIT - 1U) << UCACHE_ENTRY_KEY_BYTE_SHIFT)

#define UCACHE_SGRAPH_ALIGNMENT_SLACK \
	(ZEND_MM_ALIGNMENT > UCACHE_BLOCK_PAYLOAD_ALIGNMENT ? ZEND_MM_ALIGNMENT - 1 : 0)
#define UCACHE_SGRAPH_HDR_SIZE(pin_word_count) \
	(offsetof(ucache_sgraph_hdr, pin_owners) + (size_t) (pin_word_count) * sizeof(atomic_int))

#define UCACHE_SIZE_CLASS_EXACT_LIMIT	1024U
#define UCACHE_SIZE_CLASS_EXACT_BINS	(UCACHE_SIZE_CLASS_EXACT_LIMIT / 8U - 2U)
#define UCACHE_SIZE_CLASS_MIN_SHIFT		10U
#define UCACHE_SIZE_CLASS_STEP_SHIFT	6U

#define UCACHE_FREE_BINS \
	(UCACHE_SIZE_CLASS_EXACT_BINS + ((32U - UCACHE_SIZE_CLASS_MIN_SHIFT) << UCACHE_SIZE_CLASS_STEP_SHIFT))

#ifdef UCACHE_HAVE_OPTIMISTIC
# define UCACHE_OPTIMISTIC_ENABLED	1
#ifdef UCACHE_OPTIMISTIC_MSVC
# if defined(_M_X64) || defined(_M_ARM64)
/* Aligned loads are single-copy atomic here, so a load only needs the acquire barrier, not a locked read-modify-write. */
#  define UCACHE_MSVC_PLAIN_ATOMIC_LOADS	1
#  ifdef _M_ARM64
#   define UCACHE_MSVC_LOAD_ACQUIRE_BARRIER()	__dmb(_ARM64_BARRIER_ISH)
#  else
#   define UCACHE_MSVC_LOAD_ACQUIRE_BARRIER()	_ReadWriteBarrier()
#  endif
#  define UCACHE_ATOMIC_LOAD_64(target) \
	ucache_msvc_load_acquire_64((const volatile __int64 *) (target))
#  define UCACHE_ATOMIC_LOAD_32(target) \
	ucache_msvc_load_acquire_32((const volatile int *) (target))
# else
/* winnt.h maps these to intrinsics, or to CAS loops on x86 which lacks them. */
#  define UCACHE_ATOMIC_LOAD_64(target) \
	((uint64_t) InterlockedOr64((volatile LONG64 *) (target), 0))
#  define UCACHE_ATOMIC_LOAD_32(target) \
	((uint32_t) _InterlockedOr((volatile long *) (target), 0))
# endif
# define UCACHE_ATOMIC_STORE_64(target, val) \
	((void) InterlockedExchange64((volatile LONG64 *) (target), (LONG64) (val)))
# define UCACHE_ATOMIC_CAS_64(target, expected, desired) \
	((uint64_t) _InterlockedCompareExchange64( \
		(volatile __int64 *) (target), \
		(__int64) (desired), \
		(__int64) (expected) \
	) == (expected))
# define UCACHE_ATOMIC_STORE_32(target, val) \
	((void) _InterlockedExchange((volatile long *) (target), (long) (val)))
# define UCACHE_ATOMIC_CAS_32(target, expected, desired) \
	((uint32_t) _InterlockedCompareExchange((volatile long *) (target), (long) (desired), (long) (expected)) == (expected))
# define UCACHE_ATOMIC_LOAD_32_RELAXED(target) \
	(*(volatile uint32_t *) (target))
# define UCACHE_ATOMIC_STORE_32_RELAXED(target, val) \
	((void) (*(volatile uint32_t *) (target) = (val)))
# define UCACHE_ATOMIC_FENCE_SEQ_CST()	MemoryBarrier()
# define UCACHE_ATOMIC_FENCE_ACQUIRE()	MemoryBarrier()
#else
/* i386 aligns uint64_t to 4, making clang call libatomic; all targets are 8-aligned. */
# define UCACHE_ATOMIC_LOAD_64(target) \
	__atomic_load_n((const ucache_atomic_u64 *) (const void *) (target), __ATOMIC_ACQUIRE)
# define UCACHE_ATOMIC_STORE_64(target, val) \
	__atomic_store_n((ucache_atomic_u64 *) (void *) (target), (val), __ATOMIC_RELEASE)
# define UCACHE_ATOMIC_CAS_64(target, expected, desired) \
	__atomic_compare_exchange_n( \
		(ucache_atomic_u64 *) (void *) (target), \
		(ucache_atomic_u64 *) (void *) &(expected), \
		(desired), \
		false, \
		__ATOMIC_ACQ_REL, \
		__ATOMIC_ACQUIRE \
	)
# define UCACHE_ATOMIC_LOAD_32(target) \
	__atomic_load_n((target), __ATOMIC_ACQUIRE)
# define UCACHE_ATOMIC_STORE_32(target, val) \
	__atomic_store_n((target), (val), __ATOMIC_RELEASE)
# define UCACHE_ATOMIC_CAS_32(target, expected, desired) \
	__atomic_compare_exchange_n((target), &(expected), (desired), false, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)
# define UCACHE_ATOMIC_LOAD_32_RELAXED(target) \
	__atomic_load_n((target), __ATOMIC_RELAXED)
# define UCACHE_ATOMIC_STORE_32_RELAXED(target, val) \
	__atomic_store_n((target), (val), __ATOMIC_RELAXED)
# define UCACHE_ATOMIC_FENCE_SEQ_CST()	__atomic_thread_fence(__ATOMIC_SEQ_CST)
# define UCACHE_ATOMIC_FENCE_ACQUIRE()	__atomic_thread_fence(__ATOMIC_ACQUIRE)
#endif /* UCACHE_OPTIMISTIC_MSVC */
#else
# define UCACHE_OPTIMISTIC_ENABLED 0
# define UCACHE_ATOMIC_LOAD_64(target) (*(target))
# define UCACHE_ATOMIC_STORE_64(target, val) ((void) (*(target) = (val)))
# define UCACHE_ATOMIC_CAS_64(target, expected, desired) \
	(*(target) == (expected) ? ((*(target) = (desired)), true) : false)
# define UCACHE_ATOMIC_LOAD_32(target) (*(target))
# define UCACHE_ATOMIC_STORE_32(target, val) ((void) (*(target) = (val)))
# define UCACHE_ATOMIC_CAS_32(target, expected, desired) \
	(*(target) == (expected) ? ((*(target) = (desired)), true) : false)
# define UCACHE_ATOMIC_LOAD_32_RELAXED(target) (*(target))
# define UCACHE_ATOMIC_STORE_32_RELAXED(target, val) ((void) (*(target) = (val)))
# define UCACHE_ATOMIC_FENCE_SEQ_CST()	((void) 0)
# define UCACHE_ATOMIC_FENCE_ACQUIRE()	((void) 0)
#endif /* UCACHE_HAVE_OPTIMISTIC */

#ifdef ZTS
# define UC_G(v) ZEND_TSRMG_FAST(user_cache_globals_offset, ucache_globals *, v)
# ifdef ZEND_ENABLE_STATIC_TSRMLS_CACHE
#  define UCACHE_GLOBALS_PTR() TSRMG_FAST_BULK_STATIC(user_cache_globals_offset, ucache_globals *)
# else
#  define UCACHE_GLOBALS_PTR() TSRMG_FAST_BULK(user_cache_globals_offset, ucache_globals *)
# endif
#else
# define UC_G(v) (user_cache_globals.v)
# define UCACHE_GLOBALS_PTR() (&user_cache_globals)
#endif

#define UCACHE_DEFINE_OBJ_FROM_STD(type, name) \
	static zend_always_inline type *ucache_##name##_from_obj(zend_object *obj) \
	{ \
		return (type *) ((char *) obj - offsetof(type, std)); \
	}

#if defined(UCACHE_HAVE_OPTIMISTIC) && !defined(UCACHE_OPTIMISTIC_MSVC)
typedef uint64_t ucache_atomic_u64 __attribute__((aligned(8)));
#endif

#ifdef UCACHE_HAVE_SHARED_MUTEX
typedef union {
	pthread_mutex_t mutex;
	char padding[64];
} ucache_shared_mutex;
#endif

typedef enum {
	UCACHE_OPTIMISTIC_FALLBACK = 0,
	UCACHE_OPTIMISTIC_FOUND,
	UCACHE_OPTIMISTIC_MISS,
	UCACHE_OPTIMISTIC_UNRESTORABLE
} ucache_optimistic_result;

typedef struct {
	size_t configured_mem;
	php_ucache_reason failure_reason;
	bool enabled;
	bool available;
} ucache_runtime;

typedef struct _ucache_lock_ops ucache_lock_ops;

typedef struct {
	const ucache_shm_handlers *handler;
	const char *handler_name;
	ucache_shm_seg *seg;
	void *base;
	const ucache_lock_ops *lock_ops;
	size_t size;
	int lock_file;
#ifdef ZTS
	uint32_t startup_complete;
#endif
	uint32_t capacity_memo;
	uint32_t data_offset_memo;
	uint32_t entry_lock_capacity_memo;
	uint32_t entry_lock_offset_memo;
	uint32_t free_bins_offset_memo;
	uint32_t free_bin_count_memo;
	bool initialized;
	bool initialized_before_req;
	bool lock_initialized;
	bool layout_memo_valid;
	bool capacity_clamped;
#ifdef UCACHE_HAVE_BOUNDARY_SHM
	bool boundary_digest_memoized;
	uint8_t boundary_digest_memo[32];
	bool boundary_salt_loaded;
	uint8_t boundary_salt[UCACHE_BOUNDARY_SALT_SIZE];
#endif
#ifdef ZTS
	MUTEX_T zts_lock;
#endif
} ucache_storage;

typedef struct {
	ucache_storage storage;
	const char *lock_name;
	const char *boundary_identity;
	size_t boundary_identity_len;
	uint32_t graph_pin_slot_count;
} ucache_ctx;

struct _php_ucache_partition {
	ucache_ctx ctx;
	char *name;
	struct _php_ucache_partition *next;
};

typedef struct {
	zend_ulong hash;
	uint32_t key_offset;
	uint32_t key_len;
	uint32_t expires_at;
	uint8_t state;
	uint8_t reserved[3];
	uint64_t owner_pid;
	uint64_t owner_start_time;
	uint64_t owner_token;
} ucache_entry_lock_record;

typedef struct _ucache_entry_lock {
	ucache_ctx *ctx;
	struct _ucache_entry_lock *next;
	uint64_t owner_pid;
	uint64_t owner_start_time;
	uint64_t owner_token;
	zend_long lease;
	uint32_t key_len;
	bool requested_by_lock;
	char key[1];
} ucache_entry_lock;

typedef struct {
	uint64_t owner_pid;
	uint64_t owner_start_time;
	uint32_t active;
	uint32_t reserved;
} ucache_reader_slot;

typedef struct {
	atomic_int owner_pid;
	atomic_int pin_count;
	uint64_t owner_start_time;
} ucache_graph_pin_slot;

#ifdef UCACHE_HAVE_SHARED_MUTEX
typedef struct {
	ucache_shared_mutex mutex;
	uint64_t seq;
	uint64_t old_val;
	uint64_t old_gen;
	uint32_t slot_idx;
	uint32_t active;
	uint16_t old_flags;
	uint8_t padding_to_128_bytes[30];
} ucache_scalar_write_stripe;
#endif

typedef struct {
	uint32_t magic;
	uint32_t capacity;
	uint32_t count;
	uint32_t data_offset;
	uint32_t boundary_identity_digest_set;
	uint32_t scalar_write_enabled;
	uint64_t data_size;
	uint64_t next_free;
	uint64_t free_list_bytes;
	uint64_t mutation_epoch;
	uint64_t write_seq;
	uint64_t time_base;
	uint64_t expunge_count;
	uint64_t store_failure_count;
	uint64_t eviction_count;
	uint64_t graph_dead_pin_owners_reclaimed;
	uint64_t graph_dead_pins_stripped;
	uint32_t tombstone_count;
	uint32_t expiring_count;
	uint32_t lock_model;
	uint32_t orphaned_graphs_saturated;
	uint32_t eviction_hand;
	uint32_t reader_slots_used;
	uint32_t entry_lock_capacity;
	uint32_t entry_lock_offset;
	uint32_t entry_lock_count;
	uint32_t entry_lock_tombstone_count;
	uint32_t entry_lock_table_section_open;
	uint32_t entry_lock_sweep_at;
	uint32_t free_bins_offset;
	uint32_t free_bin_count;
	uint32_t graph_pin_slot_count;
	uint32_t expiry_floor;
	uint64_t entry_lock_acquire_seq;
	uint32_t pool_bucket_heads[UCACHE_POOL_BUCKETS];
	uint64_t pool_bucket_epochs[UCACHE_POOL_BUCKETS];
	uint8_t boundary_identity_digest[32];
	uint32_t orphaned_graphs[UCACHE_ORPHANED_GRAPH_SLOTS];
	ucache_graph_pin_slot graph_pin_slots[UCACHE_GRAPH_PIN_SLOTS_MAX];
	ucache_reader_slot reader_slots[UCACHE_READER_SLOTS];
	uint64_t lock_file_dev;
	uint64_t lock_file_ino;
	uint64_t committed_end;
	uint64_t commit_ceiling;
	uint64_t commit_failure_count;
	uint64_t stale_tail_end;
#ifdef UCACHE_HAVE_SHARED_MUTEX
	ucache_shared_mutex global_shared_mutex;
	ZEND_SET_ALIGNED(128, uint32_t scalar_write_ready);
	uint32_t scalar_write_gate;
	ZEND_SET_ALIGNED(128, ucache_scalar_write_stripe scalar_write_stripes[UCACHE_SCALAR_WRITE_STRIPES]);
#endif
} ucache_hdr;

typedef struct {
	uint32_t size;
	uint32_t owner;
} ucache_block;

typedef struct {
	uint32_t next_free;
	uint32_t prev_free;
} ucache_free_links;

typedef struct {
	uint32_t hash;
	uint32_t key_offset;
	uint32_t expires_at;
	uint16_t key_len;
	uint16_t flags;
	union {
		zend_long long_val;
		double double_val;
		struct {
			uint32_t val_offset;
			uint32_t val_len;
		};
	};
	uint64_t gen;
} ucache_entry;

typedef struct {
	uint32_t prev;
	uint32_t next;
} ucache_pool_links;

typedef struct {
	zend_string *storage_key;
	uint64_t mutation_epoch;
	zval val;
	uint32_t slot_idx;
	union {
		uint32_t pinned_payload_offset;
		uint32_t copy_charged_bytes;
	};
	uint32_t expires_at;
	uint8_t state;
	uint8_t val_kind;
	uint16_t access_touches;
} ucache_key_record;

typedef struct {
	zend_ulong hash;
	size_t payload_size;
	size_t payload_used_size;
	union {
		const uint8_t *payload_src;
		zend_long long_val;
		double double_val;
	};
	union {
		uint8_t *owned_buf;
		zend_string *owned_str;
	};
	uint32_t *fixup_offsets;
	HashTable *state_memo;
	uint32_t fixup_count;
	uint8_t val_type;
	uint8_t owner_type;
	bool has_verbatim_arr;
	bool packed_vals_allowed;
} ucache_prepared_val;

typedef struct {
	ucache_entry replaced_entry;
	uint64_t stored_gen;
	uint32_t stored_slot;
	uint32_t stored_expires_at;
	uint8_t stored_val_type;
	bool committed;
} ucache_store_result;

typedef struct {
	uint64_t gen;
	uint64_t mutation_epoch;
	zval detached_val;
	uint32_t flags;
	uint32_t val_len;
	bool should_seed;
} ucache_fetch_pending_seed;

typedef struct {
	zend_long new_val;
	uint64_t stored_gen;
	uint32_t stored_slot;
	uint32_t stored_expires_at;
	bool is_overflow;
	bool is_type_err;
} ucache_atomic_update_result;

typedef struct {
	uint8_t root_type;
	uint8_t flags;
	uint16_t pin_word_count;
	atomic_int ref_state;
	atomic_int pin_owners[1];
} ucache_sgraph_hdr;

typedef struct {
	ucache_ctx *ctx;
	uint32_t payload_offset;
} ucache_req_graph_ref;

typedef struct {
	ucache_hdr *hdr;
	uint32_t slot_idx;
} ucache_reader_claim;

typedef struct {
	ucache_hdr *hdr;
	uint32_t slot_idx;
} ucache_graph_pin_claim;

typedef struct {
	uint64_t pid;
	uint64_t start_time;
	uint64_t probed_at;
	bool dead;
} ucache_owner_probe;

typedef struct _ucache_restore_queue ucache_restore_queue;
typedef struct _ucache_owned_decode_frame ucache_owned_decode_frame;
typedef struct _ucache_sgraph_snapshot ucache_sgraph_snapshot;

typedef struct _ucache_op_lease {
	struct _ucache_op_lease *next;
	ucache_ctx *ctx;
	uint64_t owner_pid;
	uint32_t payload_offset;
	uint32_t users;
	bool acquired;
} ucache_op_lease;

typedef struct _ucache_obj {
	zend_string *scope;
	zend_string *scope_prefix;
	ucache_ctx *ctx;
	struct _ucache_obj *live_prev;
	struct _ucache_obj *live_next;
	HashTable *key_records;
	zend_string *inline_keys[UCACHE_INLINE_KEY_RECORDS];
	ucache_key_record *inline_records[UCACHE_INLINE_KEY_RECORDS];
	uint32_t inline_count;
	uint32_t key_len_max;
	zend_object std;
} ucache_obj;

typedef struct {
	zend_string *scope;
	zend_long entry_count;
	zend_long used_mem;
	zval entry_keys;
	zend_object std;
} ucache_pool_status_obj;

typedef struct {
	zend_long configured_mem;
	zend_long shared_mem_size;
	zend_long used_mem;
	zend_long free_mem;
	zend_long wasted_mem;
	zend_long entry_count;
	zend_long entry_capacity;
	zend_long tombstone_count;
	zend_long expunge_count;
	zend_long store_failure_count;
	zend_long eviction_count;
	zend_long graph_pin_slots_in_use;
	zend_long graph_pinned_refs;
	zend_long dead_pin_owners_reclaimed;
	zend_long dead_pins_stripped;
	zend_long committed_mem;
	zend_long commit_failure_count;
} ucache_info_stats;

typedef struct {
	ucache_info_stats stats;
	php_ucache_reason availability_reason;
	bool initialized;
	zend_object std;
} ucache_status_obj;

typedef struct {
	php_ucache_partition *active_partition;
	ucache_ctx *active_ctx_ptr;
	const ucache_ctx *runtime_resolved_ctx;
	ucache_runtime runtime_state;
	uint32_t op_depth;
	uint32_t access_now;
	uint32_t access_now_touches;
	bool runtime_resolved;
	bool runtime_resolved_enabled;
	bool enable;
	bool enable_cli;
	bool persistent_exec;
	bool stack_overflowed;
	bool in_req_shutdown;
	bool key_records_trim_due;
	uint32_t key_record_count;
	bool lock_held;
	bool lock_held_is_write;
	ucache_hdr *scalar_write_hdr;
	uint32_t scalar_write_idx;
	php_ucache_reason req_unavailable_reason;
	ucache_req_graph_ref *sgraph_refs;
	uint32_t sgraph_ref_count;
	uint32_t sgraph_ref_capacity;
	uint64_t sgraph_ref_owner_pid;
	uint32_t *sgraph_ref_slots;
	size_t sgraph_ref_bytes;
	HashTable *req_local_slot_table;
	uint64_t req_local_slot_owner_pid;
	HashTable *entry_lock_table;
	ucache_entry_lock *deferred_entry_lock_releases;
	ucache_entry_lock *entry_lock_spare;
	HashTable *pool_table;
	uint32_t pool_trim_at;
	struct _ucache_obj *live_pools;
	struct _ucache_held_records *held_records;
	HashTable *pool_status_snapshots;
	HashTable *decode_identity_map;
	HashTable *decode_ref_map;
	ucache_restore_queue *decode_restore_queue;
	ucache_owned_decode_frame *owned_decode_frame;
	uint64_t unrestorable_gen;
	ucache_op_lease *op_leases;
	ucache_op_lease *op_lease_free;
	ucache_op_lease op_lease_inline;
	HashTable *decode_resolve_cache;
	HashTable *decode_shape_proto_cache;
	HashTable *obj_route_memo;
	const void *decode_resolve_direct_keys[UCACHE_DECODE_DIRECT_CACHE_SLOTS];
	void *decode_resolve_direct_vals[UCACHE_DECODE_DIRECT_CACHE_SLOTS];
	const void *decode_shape_proto_direct_keys[UCACHE_DECODE_DIRECT_CACHE_SLOTS];
	zend_array *decode_shape_proto_direct_vals[UCACHE_DECODE_DIRECT_CACHE_SLOTS];
	ucache_reader_claim reader_claims[UCACHE_READER_CLAIM_MAX];
	ucache_graph_pin_claim graph_pin_claims[UCACHE_GRAPH_PIN_CLAIM_MAX];
#ifndef ZEND_WIN32
	zend_ulong entry_lock_owner_pid;
#endif /* ZEND_WIN32 */
	uint64_t graph_pin_probe_last_at;
	uint64_t reader_claim_pid;
	uint64_t reader_claim_failed_pid;
	ucache_hdr *reader_claim_failed_hdr;
	uint64_t graph_pin_claim_failed_pid;
	ucache_hdr *graph_pin_claim_failed_hdr;
	ucache_owner_probe owner_probes[UCACHE_OWNER_PROBES];
	uint64_t self_start_time_pid;
	uint64_t self_start_time_token;
	zend_long shm_size;
	zend_long entries_hint;
	zend_long eviction_policy;
	char *lockfile_path;
	char *mem_model;
	uint32_t reader_claim_count;
	uint32_t graph_pin_claim_count;
	uint32_t decode_depth;
	uint32_t restore_hook_calls;
	uint32_t expired_read_observations;
	uint32_t expunge_write_ops;
	uint32_t rehash_retry_skips;
	uint32_t entry_lock_sweep_ops;
	uint32_t expired_expunge_cursor;
	size_t record_val_bytes;
	size_t req_local_slot_bytes;
	size_t key_record_bytes;
	size_t key_record_bytes_trim_at;
	uint32_t key_record_trim_at;
	uint8_t decode_resolve_direct_next;
	uint8_t decode_shape_proto_direct_next;
	bool write_seq_bumped;
	bool entry_lock_table_section_open;
	bool store_defer_unlock;
	bool req_local_slot_may_cycle;
	int8_t reader_drain_state;
	bool exec_prepared;
	bool exec_active;
	bool exec_poisoned;
	bool logical_req_ending;
	php_ucache_partition *exec_prev_partition;
	ucache_ctx *exec_prev_ctx;
	php_ucache_reason exec_prev_unavailable_reason;
	php_ucache_partition *exec_partition;
	uint64_t logical_scope_token;
} ucache_globals;

#ifdef ZTS
extern size_t user_cache_globals_offset;
#else
extern ucache_globals user_cache_globals;
#endif

extern ucache_ctx ucache_ctx_state;
extern uint64_t ucache_self_pid;
extern bool ucache_runtime_opted_in;
extern php_ucache_partition *ucache_partitions;
extern zend_class_entry *ucache_availability_ce;
extern zend_object_handlers ucache_status_obj_handlers;
extern zend_object_handlers ucache_pool_status_obj_handlers;
extern php_ucache_mode ucache_registered_mode;
extern atomic_bool ucache_registration_closed;
#ifdef ZTS
extern MUTEX_T ucache_boundary_partitions_mutex;
#endif

void ucache_boundary_partitions_lock(void);
void ucache_boundary_partitions_unlock(void);
uint64_t ucache_resolve_pid(void);
void ucache_use_req_method_handlers(void);
void ucache_partitions_shutdown(void);
void ucache_boundary_partitions_shutdown(void);
void ucache_restore_exec_preparation(void);
ZEND_COLD void ucache_warn(const char *format, ...) ZEND_ATTRIBUTE_FORMAT(printf, 1, 2);
ZEND_COLD void ucache_warn_docref(const char *format, ...) ZEND_ATTRIBUTE_FORMAT(printf, 1, 2);
ZEND_COLD void ucache_log_err(const char *msg);
void ucache_collect_info_stats(ucache_info_stats *stats);
void ucache_release_pool_status_snapshots(void);
void ucache_collect_pool_status(
		ucache_obj *cache,
		zend_long *entry_count,
		zend_long *used_mem,
		zval *entry_keys);
void ucache_pool_status_obj_free(zend_object *obj);
zend_object *ucache_pool_status_obj_create(zend_class_entry *ce);
zend_object *ucache_status_obj_create(zend_class_entry *ce);
void ucache_reset_runtime(void);
bool ucache_exec_available(void);
bool ucache_storage_startup_is_complete(const ucache_storage *storage);
void ucache_reset_storage(void);
size_t ucache_shm_size_min(void);
bool ucache_mem_model_is_available(const char *model);
uint64_t ucache_committed_bytes_locked(const ucache_hdr *hdr);
bool ucache_hdr_init_locked(void);
bool ucache_hdr_adoptable_locked(void);
const char *ucache_lock_model_name(const ucache_storage *storage);
uint32_t ucache_free_locked(uint32_t payload_offset);
void ucache_shrink_locked(uint32_t payload_offset, size_t payload_size);
uint32_t ucache_alloc_locked(size_t size, const void *src, uint32_t owner);
bool ucache_alloc_can_satisfy_locked(size_t size);
bool ucache_reclaim_space_for_entry_lock_locked(size_t key_size);
bool ucache_startup_storage_before_req(void);
void ucache_shutdown_storage(void);
void ucache_classify_sapi(void);
void ucache_ensure_ready_impl(void);
bool ucache_rlock(void);
bool ucache_wlock(void);
bool ucache_wlock_for_entry_mutation(zend_string *key);
bool ucache_wlock_for_entry_mutations(zend_string **keys, uint32_t count);
bool ucache_try_wlock_for_entry_mutation(zend_string *key);
bool ucache_wlock_for_ref_release(bool *recovered);
void ucache_unlock(void);
void ucache_unlock_if_held(void);
bool ucache_scalar_write_begin(zend_ulong hash, ucache_hdr **hdr_ptr, uint32_t *stripe_idx);
void ucache_scalar_write_enable_locked(ucache_hdr *hdr);
void ucache_scalar_write_prepare(ucache_hdr *hdr, uint32_t stripe_idx, uint32_t slot_idx);
void ucache_scalar_write_commit(ucache_hdr *hdr, uint32_t stripe_idx, ucache_entry *entry);
void ucache_scalar_write_end(void);
bool ucache_try_acquire_entry_lock(zend_string *key, zend_long lease);
bool ucache_acquire_entry_lock(zend_string *key);
bool ucache_release_entry_lock(zend_string *key);
bool ucache_release_entry_lock_unless_requested(zend_string *key);
bool ucache_req_owns_entry_lock(zend_string *key);
bool ucache_entry_locks_allow_clear_locked(zend_string *prefix);
bool ucache_entry_key_lock_active_locked(
		ucache_hdr *hdr,
		uint32_t hash,
		size_t key_pos,
		uint32_t key_len,
		uint64_t now);
void ucache_release_req_entry_locks(void);
void ucache_retry_deferred_entry_lock_releases(void);
bool ucache_active_ctx_has_deferred_entry_lock_releases(void);
void ucache_free_thread_deferred_entry_lock_releases(ucache_globals *globals);
#ifdef ZTS
void ucache_orphan_thread_deferred_entry_lock_releases(ucache_globals *globals);
void ucache_free_orphaned_entry_lock_releases(void);
#endif
const php_ucache_safe_direct_handlers *ucache_safe_direct_find_handlers(
		zend_class_entry *ce,
		zend_class_entry **base_ce_ptr);
php_ucache_safe_direct_copy_func_t ucache_safe_direct_copy_func(zend_class_entry *ce);
bool ucache_serdes_encode(zval *val, smart_str *buf);
bool ucache_serdes_decode(const uint8_t *data, size_t len, zval *dst);
void ucache_sgraph_calc_verbatim_root(
		const zval *val,
		HashTable *verbatim_verdicts,
		size_t *buf_len);
bool ucache_calc_sgraph_size(
		const zval *val,
		HashTable *state_memo,
		HashTable *verbatim_verdicts,
		size_t *buf_len,
		bool *packed_vals_allowed);
bool ucache_build_sgraph_in_place(
		const zval *val,
		HashTable *state_memo,
		const HashTable *verbatim_verdicts,
		bool packed_vals_allowed,
		uint8_t *buf,
		size_t buf_len,
		size_t *glen,
		bool *has_verbatim_arr,
		uint32_t **fixup_offsets,
		uint32_t *fixup_count);
bool ucache_sgraph_copy_fits_buf(
		const uint8_t *dst_buf,
		const uint8_t *src_buf,
		size_t buf_len,
		size_t src_glen);
bool ucache_sgraph_decode(const uint8_t *buf, size_t buf_len, zval *dst);
ucache_sgraph_snapshot *ucache_sgraph_snapshot_create(const uint8_t *buf, size_t buf_len);
bool ucache_sgraph_decode_snapshot(ucache_sgraph_snapshot *snapshot, zval *dst);
bool ucache_owned_decode_validate_proc_impl(void);
const uint32_t *ucache_sgraph_node_sizes(uint32_t *count);
uint32_t ucache_sgraph_payload_flags(uint32_t payload_offset);
void ucache_sgraph_obj_route_memo_release(void);
ZEND_COLD void ucache_throw_unstorable_res(void);
ZEND_COLD void ucache_throw_unstorable_obj(const zend_class_entry *ce);
void ucache_decode_payload_addr_caches_release(void);
void ucache_decode_maps_teardown(void);
bool ucache_sgraph_quiesce_for_overwrite_locked(uint32_t payload_offset);
bool ucache_sgraph_payload_has_refs_locked(uint32_t payload_offset);
bool ucache_sgraph_publish_copied_payload_locked(
		uint8_t *dst_buf,
		const uint8_t *src_buf,
		size_t buf_len,
		size_t src_glen,
		bool has_verbatim_arr,
		const uint32_t *fixup_offsets,
		uint32_t fixup_count);
bool ucache_sgraph_acquire_ref(uint32_t payload_offset, bool *resource_limited);
bool ucache_sgraph_release_op_ref(uint32_t payload_offset);
void ucache_retry_op_lease_releases(void);
void ucache_release_op_leases(void);
void ucache_abandon_graph_pin_claims(void);
size_t ucache_sgraph_largest_space_after_clear_locked(ucache_hdr *hdr);
bool ucache_sgraph_retire_payload_locked(uint32_t payload_offset);
uint32_t ucache_req_sgraph_ref_idx(uint32_t payload_offset);
uint32_t ucache_register_sgraph_ref(uint32_t payload_offset, uint32_t payload_len);
void ucache_release_req_sgraph_refs(void);
void ucache_expunge_expired_at_req_end(void);
bool ucache_prepare_val(
		zend_string *key,
		zval *val,
		ucache_prepared_val *prepared);
void ucache_destroy_prepared_val(ucache_prepared_val *prepared);
bool ucache_store_prepared_locked(
		zend_string *key,
		zval *val,
		const ucache_prepared_val *prepared,
		zend_long ttl,
		bool bulk,
		ucache_store_result *result);
bool ucache_fetch_locked(
		ucache_key_record *record,
		zval *return_value,
		bool *found,
		ucache_fetch_pending_seed *pending_seed,
		bool *lock_held);
void ucache_fetch_finish(
		ucache_key_record *record,
		ucache_fetch_pending_seed *pending_seed,
		zval *return_value);
void ucache_key_record_reset(ucache_key_record *record, zval *detached);
void ucache_key_record_stored(
		ucache_key_record *record,
		const ucache_store_result *result,
		zval *val);
void ucache_key_record_deleted(ucache_key_record *record, uint64_t epoch);
void ucache_key_record_atomic_updated(
		ucache_key_record *record,
		const ucache_atomic_update_result *result);
bool ucache_exists_locked(zend_string *key);
uint64_t ucache_delete_locked(zend_string *key);
uint64_t ucache_delete_gen_locked(zend_string *key, uint64_t gen);
void ucache_discard_replaced_entry_locked(ucache_entry *replaced_entry);
void ucache_rollback_replaced_entry_locked(zend_string *key, ucache_entry *replaced_entry);
void ucache_delete_by_prefix_locked(zend_string *prefix);
bool ucache_atomic_update_locked(
		zend_string *key,
		zend_long step,
		zend_long ttl,
		bool decrement,
		ucache_atomic_update_result *result);
bool ucache_try_store_scalar(
		ucache_key_record *record,
		const ucache_prepared_val *prepared);
bool ucache_try_atomic_update(
		ucache_key_record *record,
		zend_long step,
		zend_long ttl,
		bool decrement,
		ucache_atomic_update_result *result,
		bool *updated);
void ucache_release_req_local_slots(void);
void ucache_release_req_local_slot(zend_string *key);
void ucache_release_active_req_local_slots_by_prefix(zend_string *prefix);
void ucache_obj_table_dtor(zval *zv);
void ucache_ref_table_dtor(zval *zv);
ucache_optimistic_result ucache_fetch_optimistic(
		ucache_key_record *record,
		zval *return_value,
		bool allow_decode);
ucache_optimistic_result ucache_exists_optimistic(ucache_key_record *record);
bool ucache_optimistic_reader_begin(ucache_hdr *hdr, uint32_t *slot_idx_ptr);
void ucache_optimistic_reader_end(ucache_hdr *hdr, uint32_t slot_idx);
#if defined(ZTS) && !defined(ZEND_WIN32)
void ucache_lock_storage_startup_before_fork(void);
void ucache_unlock_storage_startup_after_fork(void);
void ucache_reinit_storage_locks_after_fork(void);
#endif
#ifdef UCACHE_HAVE_BOUNDARY_SHM
void ucache_shared_boundary_segs_after_fork(void);
#endif
void ucache_release_thread_reader_claims(ucache_globals *globals);
bool ucache_quiesce_graph_payloads_locked(void);
void ucache_sgraph_ref_reserve(void);
void ucache_sgraph_orphan_payload_locked(uint32_t payload_offset);
void ucache_sgraph_reclaim_orphaned_locked(void);
bool ucache_sgraph_strip_dead_pins_locked(bool force);
bool ucache_owner_is_dead(uint64_t owner_pid, uint64_t owner_start_time);
uint64_t ucache_self_start_time_token(void);
void ucache_release_thread_graph_pin_claims(ucache_globals *globals);
ZEND_METHOD(UserCache_CacheStatus, __construct);
ZEND_METHOD(UserCache_CacheStatus, getAvailability);
ZEND_METHOD(UserCache_CacheStatus, getConfiguredMemory);
ZEND_METHOD(UserCache_CacheStatus, getSharedMemorySize);
ZEND_METHOD(UserCache_CacheStatus, getUsedMemory);
ZEND_METHOD(UserCache_CacheStatus, getFreeMemory);
ZEND_METHOD(UserCache_CacheStatus, getWastedMemory);
ZEND_METHOD(UserCache_CacheStatus, getEntryCount);
ZEND_METHOD(UserCache_CacheStatus, getEntryCapacity);
ZEND_METHOD(UserCache_CacheStatus, getTombstoneCount);
ZEND_METHOD(UserCache_CacheStatus, getExpungeCount);
ZEND_METHOD(UserCache_CacheStatus, getEvictionCount);
ZEND_METHOD(UserCache_CacheStatus, getStoreFailureCount);
ZEND_METHOD(UserCache_CacheStatus, getGraphPinSlotsInUse);
ZEND_METHOD(UserCache_CacheStatus, getGraphPinnedReferences);
ZEND_METHOD(UserCache_CacheStatus, getDeadPinOwnersReclaimed);
ZEND_METHOD(UserCache_CacheStatus, getDeadPinsStripped);
ZEND_METHOD(UserCache_CacheStatus, getCommittedMemory);
ZEND_METHOD(UserCache_CacheStatus, getCommitFailureCount);
ZEND_METHOD(UserCache_CachePoolStatus, __construct);
ZEND_METHOD(UserCache_CachePoolStatus, getPoolName);
ZEND_METHOD(UserCache_CachePoolStatus, getEntryCount);
ZEND_METHOD(UserCache_CachePoolStatus, getEntryKeys);
ZEND_METHOD(UserCache_CachePoolStatus, getUsedMemory);

static_assert(
	offsetof(ucache_hdr, scalar_write_enabled) / UCACHE_CPU_CACHE_LINE_SIZE ==
		offsetof(ucache_hdr, write_seq) / UCACHE_CPU_CACHE_LINE_SIZE,
	"scalar_write_enabled must share the cache line of write_seq"
);
static_assert(
	sizeof(ucache_entry) == 32,
	"two entries must share a cache line"
);
static_assert(
	sizeof(ucache_block) == UCACHE_BLOCK_HDR_UNITS * UCACHE_OFFSET_UNIT,
	"the block header must span whole offset units"
);
static_assert(
	UCACHE_PLATFORM_ALIGNMENT % UCACHE_OFFSET_UNIT == 0,
	"blocks must start on offset units"
);
static_assert(
	UCACHE_BLOCK_SIZE_MAX % UCACHE_PLATFORM_ALIGNMENT == 0,
	"the largest block must keep block alignment"
);
static_assert(
	(uint64_t) MAX(
		UCACHE_ENTRIES_HINT_MAX,
		UCACHE_SEG_SIZE_MAX / UCACHE_AUTO_SEG_BYTES_PER_ENTRY
	) * 2 * UCACHE_TABLE_SLOT_SIZE < UINT32_MAX,
	"the largest entry table must keep the layout offsets within 32 bits"
);

#if ZEND_DEBUG
static zend_always_inline bool ucache_debug_fault(const char *env_name)
{
	const char *val = getenv(env_name);

	return val != NULL && val[0] != '\0' && val[0] != '0';
}
#endif

static zend_always_inline uint32_t ucache_table_hash(zend_ulong hash)
{
	return (uint32_t) hash;
}

static zend_always_inline uint32_t ucache_entry_kind(const ucache_entry *entry)
{
	return (entry->flags & UCACHE_ENTRY_KIND_MASK) >> UCACHE_ENTRY_KIND_SHIFT;
}

static zend_always_inline bool ucache_entry_is_used(const ucache_entry *entry)
{
	return (entry->flags & UCACHE_ENTRY_KIND_USED_BITS) != 0;
}

static zend_always_inline bool ucache_entry_holds_scalar(const ucache_entry *entry)
{
	return ucache_entry_kind(entry) - UCACHE_ENTRY_USED <= UCACHE_VAL_DOUBLE;
}

static zend_always_inline uint8_t ucache_entry_val_type(const ucache_entry *entry)
{
	ZEND_ASSERT(ucache_entry_is_used(entry));

	return (uint8_t) (ucache_entry_kind(entry) - UCACHE_ENTRY_USED);
}

static zend_always_inline uint32_t ucache_entry_val_offset(const ucache_entry *entry)
{
	uint32_t kind = ucache_entry_kind(entry);

	return kind == UCACHE_ENTRY_USED + UCACHE_VAL_STR ||
		kind == UCACHE_ENTRY_USED + UCACHE_VAL_SGRAPH
		? entry->val_offset : 0
	;
}

static zend_always_inline uint64_t ucache_cached_pid(void)
{
	uint64_t pid = ucache_self_pid;

	return EXPECTED(pid != 0) ? pid : ucache_resolve_pid();
}

static zend_always_inline bool ucache_stack_exhausted(void)
{
#ifdef ZEND_CHECK_STACK_LIMIT
	return UNEXPECTED(zend_call_stack_overflowed(EG(stack_limit)));
#else
	return false;
#endif /* ZEND_CHECK_STACK_LIMIT */
}

static zend_always_inline bool ucache_owned_decode_validate_proc(void)
{
	return EXPECTED(UC_G(owned_decode_frame) == NULL) || ucache_owned_decode_validate_proc_impl();
}

#ifdef UCACHE_MSVC_PLAIN_ATOMIC_LOADS
static zend_always_inline uint64_t ucache_msvc_load_acquire_64(const volatile __int64 *target)
{
	uint64_t val = (uint64_t) __iso_volatile_load64(target);

	UCACHE_MSVC_LOAD_ACQUIRE_BARRIER();

	return val;
}

static zend_always_inline uint32_t ucache_msvc_load_acquire_32(const volatile int *target)
{
	uint32_t val = (uint32_t) __iso_volatile_load32(target);

	UCACHE_MSVC_LOAD_ACQUIRE_BARRIER();

	return val;
}
#endif

static zend_always_inline uint64_t ucache_atomic_load_64(const uint64_t *target)
{
	return UCACHE_ATOMIC_LOAD_64(target);
}

static zend_always_inline void ucache_atomic_store_64(uint64_t *target, uint64_t val)
{
	UCACHE_ATOMIC_STORE_64(target, val);
}

static zend_always_inline bool ucache_atomic_cas_64(uint64_t *target, uint64_t expected, uint64_t desired)
{
	return UCACHE_ATOMIC_CAS_64(target, expected, desired);
}

static zend_always_inline bool ucache_atomic_cas_32(uint32_t *target, uint32_t expected, uint32_t desired)
{
	return UCACHE_ATOMIC_CAS_32(target, expected, desired);
}

static zend_always_inline uint32_t ucache_scalar_write_idx(zend_ulong hash)
{
	return (uint32_t) ((hash ^ (hash >> 16)) & (UCACHE_SCALAR_WRITE_STRIPES - 1));
}

static zend_always_inline void ucache_atomic_fence_acquire(void)
{
	UCACHE_ATOMIC_FENCE_ACQUIRE();
}

static zend_always_inline ucache_ctx *ucache_owning_ctx(void)
{
	return UC_G(active_partition) != NULL
		? &UC_G(active_partition)->ctx
		: &ucache_ctx_state
	;
}

static zend_always_inline ucache_ctx *ucache_active_ctx(void)
{
	if (UC_G(active_ctx_ptr) != NULL) {
		return UC_G(active_ctx_ptr);
	}

	return ucache_owning_ctx();
}

static zend_always_inline bool ucache_ctx_is_boundary(const ucache_ctx *ctx)
{
	return ctx->boundary_identity != NULL;
}

static zend_always_inline uint32_t ucache_ctx_graph_pin_slot_count(const ucache_ctx *ctx)
{
	return ctx->graph_pin_slot_count != 0 ? ctx->graph_pin_slot_count : UCACHE_GRAPH_PIN_SLOTS_MAX;
}

static zend_always_inline uint32_t ucache_graph_pin_slot_count(const ucache_hdr *hdr)
{
	return MIN(hdr->graph_pin_slot_count, UCACHE_GRAPH_PIN_SLOTS_MAX);
}

static zend_always_inline void *ucache_base(void)
{
	return ucache_active_ctx()->storage.base;
}

static zend_always_inline size_t ucache_req_cache_budget(void)
{
	size_t mem = PG(memory_limit) > 0
		? (size_t) PG(memory_limit)
		: ucache_active_ctx()->storage.size
	;

	return MAX(UCACHE_REQ_CACHE_BUDGET_MIN, mem / UCACHE_REQ_CACHE_BUDGET_DIVISOR);
}

static zend_always_inline ucache_hdr *ucache_hdr_ptr(void)
{
	return (ucache_hdr *) ucache_base();
}

static zend_always_inline size_t ucache_offset_bytes(uint32_t offset)
{
	return (size_t) offset << UCACHE_OFFSET_SHIFT;
}

static zend_always_inline uint32_t ucache_offset_from_bytes(size_t bytes)
{
	ZEND_ASSERT((bytes & (UCACHE_OFFSET_UNIT - 1)) == 0);
#if SIZEOF_SIZE_T > 4
	ZEND_ASSERT((uint64_t) bytes <= UCACHE_SEG_SIZE_MAX);
#endif

	return (uint32_t) (bytes >> UCACHE_OFFSET_SHIFT);
}

static zend_always_inline uint32_t ucache_offset_after(uint32_t offset, size_t bytes)
{
	return offset + ucache_offset_from_bytes(bytes);
}

static zend_always_inline uint32_t ucache_payload_block_offset(uint32_t payload_offset)
{
	return payload_offset - UCACHE_BLOCK_HDR_UNITS;
}

static zend_always_inline uint32_t ucache_block_payload_offset(uint32_t block_offset)
{
	return block_offset + UCACHE_BLOCK_HDR_UNITS;
}

static zend_always_inline uint8_t *ucache_ptr(uint32_t offset)
{
	return (uint8_t *) ucache_base() + ucache_offset_bytes(offset);
}

static zend_always_inline size_t ucache_entry_key_pos(const ucache_entry *entry)
{
	return ucache_offset_bytes(entry->key_offset) |
		((entry->flags & UCACHE_ENTRY_KEY_BYTE_MASK) >> UCACHE_ENTRY_KEY_BYTE_SHIFT)
	;
}

static zend_always_inline uint32_t ucache_used_end_offset_locked(const ucache_hdr *hdr)
{
	return ucache_offset_from_bytes((size_t) hdr->data_offset + hdr->next_free);
}

static zend_always_inline ucache_block *ucache_block_ptr(uint32_t offset)
{
	return (ucache_block *) ucache_ptr(offset);
}

static zend_always_inline ucache_block *ucache_block_ptr_in_hdr(
		const ucache_hdr *hdr,
		uint32_t offset)
{
	return (ucache_block *) ((uint8_t *) hdr + ucache_offset_bytes(offset));
}

static zend_always_inline uint32_t ucache_block_merge_limit(void)
{
#if ZEND_DEBUG
	if (UCACHE_DEBUG_FAULT("SMALL_BLOCK_MERGE_LIMIT")) {
		return UCACHE_DEBUG_BLOCK_MERGE_LIMIT;
	}
#endif

	return UCACHE_BLOCK_SIZE_MAX;
}

static zend_always_inline uint32_t ucache_block_size(const ucache_block *block)
{
	return block->size & ~UCACHE_BLOCK_FLAGS;
}

static zend_always_inline bool ucache_block_is_free(const ucache_block *block)
{
	return (block->size & UCACHE_BLOCK_FREE) != 0;
}

static zend_always_inline bool ucache_block_prev_is_free(const ucache_block *block)
{
	return (block->size & UCACHE_BLOCK_PREV_FREE) != 0;
}

static zend_always_inline uint32_t ucache_floor_log2(uint32_t val)
{
	return (uint32_t) (SIZEOF_ZEND_LONG * 8 - 1) - (uint32_t) zend_ulong_nlz((zend_ulong) val);
}

static zend_always_inline size_t ucache_size_class_round_up(size_t size)
{
	size_t step;

	if (size < UCACHE_SIZE_CLASS_EXACT_LIMIT || size > UINT32_MAX) {
		return size;
	}

	step = (size_t) 1 << (ucache_floor_log2((uint32_t) size) - UCACHE_SIZE_CLASS_STEP_SHIFT);

	return (size + step - 1) & ~(step - 1);
}

static zend_always_inline size_t ucache_block_total_size(size_t payload_size)
{
	size_t total = UCACHE_ALIGNED_SIZE(sizeof(ucache_block) + payload_size);

	return ucache_size_class_round_up(MAX(total, UCACHE_BLOCK_MIN_SIZE));
}

static zend_always_inline uint32_t ucache_block_payload_capacity(
		const ucache_hdr *hdr,
		uint32_t payload_offset)
{
	uint32_t size;

	if (payload_offset < UCACHE_BLOCK_HDR_UNITS) {
		return 0;
	}

	size = ucache_block_size(
		ucache_block_ptr_in_hdr(hdr, ucache_payload_block_offset(payload_offset))
	);

	return size < sizeof(ucache_block) ? 0 : size - (uint32_t) sizeof(ucache_block);
}

static zend_always_inline bool ucache_bytes_committed(const ucache_hdr *hdr, uint64_t end)
{
#ifdef ZEND_WIN32
	return end <= ucache_atomic_load_64(&hdr->committed_end);
#else
	(void) hdr;
	(void) end;

	return true;
#endif
}

static zend_always_inline bool ucache_bytes_in_bounds(
		const ucache_hdr *hdr,
		size_t pos,
		uint64_t len)
{
	return pos >= (size_t) hdr->data_offset + sizeof(ucache_block) &&
		(uint64_t) pos + len <= (uint64_t) hdr->data_offset + hdr->data_size &&
		ucache_bytes_committed(hdr, (uint64_t) pos + len)
	;
}

static zend_always_inline bool ucache_payload_in_bounds(
		const ucache_hdr *hdr,
		uint32_t offset,
		uint64_t len)
{
	return ucache_bytes_in_bounds(hdr, ucache_offset_bytes(offset), len);
}

static zend_always_inline ucache_ctx *ucache_activate_ctx(ucache_ctx *ctx)
{
	ucache_ctx *prev = UC_G(active_ctx_ptr);

	UC_G(active_ctx_ptr) = ctx;

	return prev;
}

static zend_always_inline void ucache_restore_ctx(ucache_ctx *ctx)
{
	UC_G(active_ctx_ptr) = ctx;
}

static zend_always_inline ucache_runtime *ucache_active_runtime(void)
{
	return &UC_G(runtime_state);
}

static zend_always_inline bool ucache_hdr_is_initialized_locked(void)
{
	ucache_hdr *hdr = ucache_hdr_ptr();

	return hdr != NULL && hdr->magic == UCACHE_MAGIC;
}

static zend_always_inline uint64_t ucache_clock_now(void)
{
#if ZEND_HRTIME_AVAILABLE
	return zend_hrtime() / UCACHE_CLOCK_TICK_NS;
#else
	return (uint64_t) time(NULL) * UCACHE_CLOCK_TICKS_PER_SEC;
#endif
}

static zend_always_inline uint64_t ucache_time_rel(const ucache_hdr *hdr, uint64_t now)
{
	return now > hdr->time_base
		? now - hdr->time_base
		: 0
	;
}

static zend_always_inline uint32_t ucache_expiry_deadline(
		const ucache_hdr *hdr,
		uint64_t now,
		zend_long ttl)
{
	uint64_t next_full_tick, deadline;

	if ((zend_ulong) ttl >= UCACHE_CLOCK_SPAN_SEC) {
		return UINT32_MAX;
	}

	next_full_tick = ucache_time_rel(hdr, now) + 1;
	deadline = next_full_tick + (uint64_t) ttl * UCACHE_CLOCK_TICKS_PER_SEC;

	return deadline > (uint64_t) UINT32_MAX
		? UINT32_MAX
		: (uint32_t) deadline
	;
}

static zend_always_inline uint64_t ucache_data_tail_limit_locked(const ucache_hdr *hdr)
{
	if (hdr->commit_ceiling > hdr->data_offset &&
		hdr->commit_ceiling - hdr->data_offset < hdr->data_size
	) {
		return hdr->commit_ceiling - hdr->data_offset;
	}

	return hdr->data_size;
}

static zend_always_inline void ucache_bump_mutation_epoch_locked(ucache_hdr *hdr)
{
	uint64_t epoch;

	if (hdr == NULL) {
		return;
	}

	epoch = hdr->mutation_epoch + 1;

	ucache_atomic_store_64(&hdr->mutation_epoch, epoch != 0 ? epoch : 1);
}

static zend_always_inline ucache_entry *ucache_entries_ptr(ucache_hdr *hdr)
{
	return (ucache_entry *) ((char *) hdr + sizeof(ucache_hdr));
}

static zend_always_inline ucache_pool_links *ucache_pool_links_ptr(ucache_hdr *hdr)
{
	return (ucache_pool_links *) (
		(char *) hdr
		+ sizeof(ucache_hdr)
		+ (size_t) hdr->capacity * (sizeof(ucache_entry) + sizeof(uint32_t))
	);
}

static zend_always_inline void ucache_pool_bucket_changed_locked(ucache_hdr *hdr, uint32_t bucket)
{
	hdr->pool_bucket_epochs[bucket]++;
	if (hdr->pool_bucket_epochs[bucket] == 0) {
		hdr->pool_bucket_epochs[bucket] = 1;
	}
}

static zend_always_inline void ucache_pool_idx_reset_locked(ucache_hdr *hdr)
{
	uint32_t bucket;

	memset(hdr->pool_bucket_heads, 0, sizeof(hdr->pool_bucket_heads));
	for (bucket = 0; bucket < UCACHE_POOL_BUCKETS; bucket++) {
		ucache_pool_bucket_changed_locked(hdr, bucket);
	}
}

static zend_always_inline uint32_t *ucache_access_stamps_ptr(ucache_hdr *hdr)
{
	return (uint32_t *) (
		(char *) hdr
		+ sizeof(ucache_hdr)
		+ (size_t) hdr->capacity * sizeof(ucache_entry)
	);
}

static zend_always_inline uint32_t ucache_free_bin_words(uint32_t bin_count)
{
	return (bin_count + 31U) / 32U;
}

static zend_always_inline uint32_t *ucache_free_bins_ptr(const ucache_hdr *hdr)
{
	return (uint32_t *) ((char *) hdr + hdr->free_bins_offset);
}

static zend_always_inline uint32_t *ucache_free_bin_mask_ptr(const ucache_hdr *hdr)
{
	return ucache_free_bins_ptr(hdr) + hdr->free_bin_count;
}

static zend_always_inline ucache_entry_lock_record *ucache_entry_lock_records_ptr(ucache_hdr *hdr)
{
	return (ucache_entry_lock_record *) ((char *) hdr + hdr->entry_lock_offset);
}

static zend_always_inline void ucache_access_stamps_reset(ucache_hdr *hdr)
{
	uint32_t i, *stamps = ucache_access_stamps_ptr(hdr);

	for (i = 0; i < hdr->capacity; i++) {
		UCACHE_ATOMIC_STORE_32_RELAXED(&stamps[i], 0);
	}
}

static zend_always_inline bool ucache_seen_test_and_add(HashTable *seen, const void *ptr)
{
	return zend_hash_index_add_empty_element(seen, (zend_ulong) (uintptr_t) ptr) != NULL;
}

UCACHE_DEFINE_OBJ_FROM_STD(ucache_pool_status_obj, pool_status_obj)

UCACHE_DEFINE_OBJ_FROM_STD(ucache_status_obj, status_obj)

#endif /* UCACHE_INTERNAL_H */
