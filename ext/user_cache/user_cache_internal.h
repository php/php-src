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

#ifndef PHP_USER_CACHE_INTERNAL_H
#define PHP_USER_CACHE_INTERNAL_H

#include "php.h"

#include <time.h>
#ifdef ZTS
# include "TSRM/TSRM.h"
#endif

#include "Zend/zend_atomic.h"
#include "Zend/zend_bitset.h"
#include "Zend/zend_call_stack.h"
#include "Zend/zend_enum.h"
#include "Zend/zend_exceptions.h"
#include "Zend/zend_smart_str.h"

#include "php_user_cache.h"
#include "user_cache_shm.h"

#ifdef ZEND_WIN32
# include "zend_execute.h"
# include "win32/ioutil.h"

# include <fcntl.h>
# include <io.h>
# include <winbase.h>
# ifdef _MSC_VER
#  include <intrin.h>
# endif
#else /* !ZEND_WIN32 */
# include <errno.h>
# include <fcntl.h>
# include <pthread.h>
# include <sys/types.h>
# include <sys/stat.h>
# ifdef HAVE_UNISTD_H
#  include <unistd.h>
# endif
# if defined(PHP_UCACHE_USE_MMAP) || (defined(__linux__) && defined(HAVE_MEMFD_CREATE))
#  include <sys/mman.h>
# endif
#endif /* ZEND_WIN32 */
#define PHP_UCACHE_MAGIC		0xCAC17E01U
#define PHP_UCACHE_VERSION		1U
#define PHP_UCACHE_MIN_CAPACITY 127U
#define PHP_UCACHE_OCCUPANCY_WORD_BITS	(sizeof(zend_ulong) * 8)
#define PHP_UCACHE_OCCUPANCY_WORDS(capacity) \
	(((size_t) (capacity) + PHP_UCACHE_OCCUPANCY_WORD_BITS - 1) / PHP_UCACHE_OCCUPANCY_WORD_BITS)
#define PHP_UCACHE_OCCUPANCY_BYTES(capacity) \
	(PHP_UCACHE_OCCUPANCY_WORDS(capacity) * sizeof(zend_ulong))
/* Segment-wide interning of array string keys, property, class and enum
 * case names up to PHP_UCACHE_INTERN_MAX_LEN bytes. */
#define PHP_UCACHE_INTERN_MIN_CAPACITY		256U
#define PHP_UCACHE_INTERN_MAX_CAPACITY		(1U << 24)
#define PHP_UCACHE_INTERN_MAX_LEN			64U
/* Distinct keys one payload may intern: bounds the lock-free probes and
 * the candidate bookkeeping for key-heavy values whose keys are unique. */
#define PHP_UCACHE_INTERN_MAX_KEYS_PER_PAYLOAD	256U
#define PHP_UCACHE_INTERN_BYTES(capacity) \
	((size_t) (capacity) * sizeof(php_ucache_intern_slot_t))
/* String offset fields in graph payloads: bit 31 marks a segment unit
 * offset of an interned string instead of a payload-relative byte offset. */
#define PHP_UCACHE_GRAPH_SEGMENT_STRING_FLAG	0x80000000U
/* At or below this the header plus the minimum entry, occupancy and lock
 * tables cannot even be formatted, so the cache silently stays unavailable.
 * Mirrors the alignment roundings of ucache_header_layout_memo(). */
#define PHP_UCACHE_SHM_SIZE_FLOOR \
	PHP_UCACHE_ALIGNED_SIZE( \
		PHP_UCACHE_ALIGNED_SIZE( \
			PHP_UCACHE_ALIGNED_SIZE( \
				PHP_UCACHE_ALIGNED_SIZE( \
					sizeof(php_ucache_header_t) \
					+ PHP_UCACHE_MIN_CAPACITY * (sizeof(php_ucache_entry_t) + sizeof(uint32_t)) \
				) \
				+ PHP_UCACHE_OCCUPANCY_BYTES(PHP_UCACHE_MIN_CAPACITY) \
			) \
			+ PHP_UCACHE_INTERN_BYTES(PHP_UCACHE_INTERN_MIN_CAPACITY) \
		) \
		+ PHP_UCACHE_ENTRY_LOCK_MIN_CAPACITY * sizeof(php_ucache_entry_lock_record_t) \
	)
#define PHP_UCACHE_BLOCK_HEADER_UNITS \
	((uint32_t) (sizeof(php_ucache_block_t) / PHP_UCACHE_SHM_UNIT))

#define PHP_UCACHE_KEY_DELIMITER		"\x1f"
#define PHP_UCACHE_KEY_DELIMITER_CHAR	'\x1f'
#define PHP_UCACHE_KEY_DELIMITER_NAME	"0x1F"

#define PHP_UCACHE_MSG_RESOURCE_UNSTORABLE			"resources cannot be stored in the user cache"
#define PHP_UCACHE_MSG_CLOSURE_UNSTORABLE			"Closure objects cannot be stored in the user cache"
#define PHP_UCACHE_MSG_LAZY_OBJECT_UNSTORABLE		"lazy objects cannot be stored in the user cache"
#define PHP_UCACHE_MSG_OPAQUE_OBJECT_UNSTORABLE		"objects with opaque internal state (e.g. Fiber, Generator, PDO) cannot be stored in the user cache"
#define PHP_UCACHE_MSG_NESTED_TOO_DEEP_UNSTORABLE	"value is nested too deeply to be stored in the user cache"

#define PHP_UCACHE_ENTRY_EMPTY		0
#define PHP_UCACHE_ENTRY_USED		1
#define PHP_UCACHE_ENTRY_TOMBSTONE	2

#define PHP_UCACHE_VALUE_NULL			0
#define PHP_UCACHE_VALUE_TRUE			1
#define PHP_UCACHE_VALUE_FALSE			2
#define PHP_UCACHE_VALUE_LONG			3
#define PHP_UCACHE_VALUE_DOUBLE			4
#define PHP_UCACHE_VALUE_STRING			5
#define PHP_UCACHE_VALUE_SHARED_GRAPH	8

#define PHP_UCACHE_SHARED_GRAPH_MAGIC						0xCAC17E02U
#define PHP_UCACHE_SHARED_GRAPH_VERSION						1U
#define PHP_UCACHE_SHARED_GRAPH_FLAG_HAS_SHARED_IDENTITY	0x2U
#define PHP_UCACHE_SHARED_GRAPH_FLAG_HAS_OBJECT				0x4U
#define PHP_UCACHE_SHARED_GRAPH_FLAG_PREFERS_PROTOTYPE		0x8U
#define PHP_UCACHE_SHARED_GRAPH_RETIRED						(1 << 30) /* ref_state packs the RETIRED flag (bit 30) with the live refcount (low bits). */
#define PHP_UCACHE_SHARED_GRAPH_REFCOUNT_MASK				(PHP_UCACHE_SHARED_GRAPH_RETIRED - 1)

#define PHP_UCACHE_SHARED_GRAPH_VALUE_UNDEF						0
#define PHP_UCACHE_SHARED_GRAPH_VALUE_NULL						1
#define PHP_UCACHE_SHARED_GRAPH_VALUE_TRUE						2
#define PHP_UCACHE_SHARED_GRAPH_VALUE_FALSE						3
#define PHP_UCACHE_SHARED_GRAPH_VALUE_LONG						4
#define PHP_UCACHE_SHARED_GRAPH_VALUE_DOUBLE					5
#define PHP_UCACHE_SHARED_GRAPH_VALUE_STRING					6
#define PHP_UCACHE_SHARED_GRAPH_VALUE_ARRAY						7
#define PHP_UCACHE_SHARED_GRAPH_VALUE_OBJECT					8
#define PHP_UCACHE_SHARED_GRAPH_VALUE_DYNAMIC_ARRAY				10
#define PHP_UCACHE_SHARED_GRAPH_VALUE_OBJECT_REF				11
#define PHP_UCACHE_SHARED_GRAPH_VALUE_REFERENCE					13
#define PHP_UCACHE_SHARED_GRAPH_VALUE_REFERENCE_REF				14
#define PHP_UCACHE_SHARED_GRAPH_VALUE_ARRAY_REF					15
#define PHP_UCACHE_SHARED_GRAPH_VALUE_ENUM						16
#define PHP_UCACHE_SHARED_GRAPH_VALUE_SAFE_DIRECT_OBJECT		18
#define PHP_UCACHE_SHARED_GRAPH_VALUE_SERIALIZED_OBJECT			19
#define PHP_UCACHE_SHARED_GRAPH_VALUE_SERDES_OBJECT				20
#define PHP_UCACHE_SHARED_GRAPH_VALUE_SLEEP_OBJECT				21
#define PHP_UCACHE_SHARED_GRAPH_VALUE_SHAPED_ARRAY				22
#define PHP_UCACHE_SHARED_GRAPH_VALUE_SERIALIZED_SHAPED_OBJECT	23
#define PHP_UCACHE_SHARED_GRAPH_VALUE_SLEEP_SHAPED_OBJECT		24

#define PHP_UCACHE_SHARED_GRAPH_OBJECT_FLAG_SHARED	0x1U

#define PHP_UCACHE_SHARED_GRAPH_ARRAY_FLAG_PACKED			0x2U
#define PHP_UCACHE_SHARED_GRAPH_ARRAY_FLAG_WIDE_NEXT_FREE	0x4U /* next_free holds the offset of an out-of-line int64 next-free index. */
/* Which key columns a dynamic array's column block carries. */
#define PHP_UCACHE_SHARED_GRAPH_ARRAY_FLAG_STRING_KEYS		0x8U
#define PHP_UCACHE_SHARED_GRAPH_ARRAY_FLAG_INT_KEYS			0x10U
#define PHP_UCACHE_SHARED_GRAPH_ARRAY_SHAPE_MAX_KEYS		32U

#define PHP_UCACHE_LOOKUP_BUCKETS		256U
#define PHP_UCACHE_LOOKUP_WAYS			2U
#define PHP_UCACHE_LOOKUP_SETS			(PHP_UCACHE_LOOKUP_BUCKETS / PHP_UCACHE_LOOKUP_WAYS)

#define PHP_UCACHE_DECODE_DIRECT_CACHE_SLOTS	4U

#define PHP_UCACHE_EVICTION_POLICY_LRU		0
#define PHP_UCACHE_EVICTION_POLICY_CLEAR	1
#define PHP_UCACHE_EVICTION_POLICY_NONE		2

/* Per-key lock records live in a capacity-sized power-of-two region behind
 * the access stamps (see ucache_calculate_entry_lock_capacity()); the
 * bounds below clamp that sizing. */
#define PHP_UCACHE_ENTRY_LOCK_MIN_CAPACITY		128U
#define PHP_UCACHE_ENTRY_LOCK_MAX_CAPACITY		1024U
#define PHP_UCACHE_ENTRY_LOCK_EMPTY				0
#define PHP_UCACHE_ENTRY_LOCK_USED				1
#define PHP_UCACHE_ENTRY_LOCK_TOMBSTONE			2

#if defined(__GNUC__) || defined(__clang__)
# define PHP_UCACHE_HAVE_OPTIMISTIC	1
#elif defined(ZEND_WIN32) && defined(_MSC_VER)
# define PHP_UCACHE_HAVE_OPTIMISTIC	1
# define PHP_UCACHE_OPTIMISTIC_MSVC	1
#endif

#if !defined(ZEND_WIN32) && defined(__linux__)
# define PHP_UCACHE_HAVE_SHARED_MUTEX	1
#endif

#if defined(PHP_UCACHE_USE_MMAP) && !defined(ZEND_WIN32)
# define PHP_UCACHE_HAVE_ANON_MMAP	1
/* Boundary partitions never touch POSIX shm (/dev/shm): independently
 * started processes rendezvous through a fully preallocated regular file
 * below user_cache.lockfile_path that is mapped with mmap(). */
# define PHP_UCACHE_HAVE_BOUNDARY_MMAP	1
# define PHP_UCACHE_BOUNDARY_SALT_SIZE	32
# if defined(MAP_ANON) && !defined(MAP_ANONYMOUS)
#  define MAP_ANONYMOUS MAP_ANON
# endif
#endif

#define PHP_UCACHE_LOCK_MODEL_FCNTL	0U
#define PHP_UCACHE_LOCK_MODEL_MUTEX	1U

#define PHP_UCACHE_READER_SLOTS			256U
#define PHP_UCACHE_POOL_STATS_SLOTS		128U
#define PHP_UCACHE_POOL_STATS_EMPTY		0U
#define PHP_UCACHE_POOL_STATS_USED		1U
#define PHP_UCACHE_POOL_STATS_TOMBSTONE	2U
#define PHP_UCACHE_READER_CLAIM_MAX		4U

#define PHP_UCACHE_ORPHANED_GRAPH_SLOTS	32U

#define PHP_UCACHE_GRAPH_PIN_SLOTS				256U
#define PHP_UCACHE_GRAPH_PIN_WORDS				(PHP_UCACHE_GRAPH_PIN_SLOTS / 32U)
#define PHP_UCACHE_GRAPH_PIN_CLAIM_MAX			4U

#define PHP_UCACHE_LOOKUP_EMPTY	0
#define PHP_UCACHE_LOOKUP_HIT	1
#define PHP_UCACHE_LOOKUP_MISS	2

#define PHP_UCACHE_ENTRY_FLAG_COMBINED_VALUE_KEY	0x0001U

#define PHP_UCACHE_BLOCK_FREE			1U

#ifdef ZEND_WIN32
# define PHP_UCACHE_WIN32_MAPPING_NAME "PhpUserCache.SharedMemoryArea"
# define PHP_UCACHE_WIN32_MAPPING_MUTEX_NAME "PhpUserCache.SharedMemoryMutex"
# define PHP_UCACHE_WIN32_LOCK_FILE_NAME "PhpUserCache.LockFile"
#else
# define PHP_UCACHE_SEM_FILENAME_PREFIX	".PhpUserCacheSem."
#endif

#ifdef ZTS
# ifdef ZEND_WIN32
#  define PHP_UCACHE_STARTUP_LOCK_INITIALIZER SRWLOCK_INIT
# else
#  define PHP_UCACHE_STARTUP_LOCK_INITIALIZER PTHREAD_MUTEX_INITIALIZER
# endif
#endif

/* Clear the debug-only flag before exposing a decoded table. */
#if ZEND_DEBUG
# define PHP_UCACHE_HT_DISALLOW_COW_VIOLATION(ht) HT_FLAGS(ht) &= ~HASH_FLAG_ALLOW_COW_VIOLATION
#else
# define PHP_UCACHE_HT_DISALLOW_COW_VIOLATION(ht)
#endif

#ifdef PHP_UCACHE_HAVE_OPTIMISTIC
# define PHP_UCACHE_OPTIMISTIC_ENABLED 1
#ifdef PHP_UCACHE_OPTIMISTIC_MSVC
/* The winnt.h wrappers, not the intrinsics: on x86 only the 64-bit
 * compare-exchange intrinsic exists, and winnt.h implements
 * InterlockedOr64 / InterlockedExchange64 there as inline CAS loops. */
# define PHP_UCACHE_ATOMIC_LOAD_64(target) \
	((uint64_t) InterlockedOr64((volatile LONG64 *) (target), 0))
# define PHP_UCACHE_ATOMIC_STORE_64(target, value) \
	((void) InterlockedExchange64((volatile LONG64 *) (target), (LONG64) (value)))
# define PHP_UCACHE_ATOMIC_CAS_64(target, expected, desired) \
	((uint64_t) InterlockedCompareExchange64( \
		(volatile LONG64 *) (target), \
		(LONG64) (desired), \
		(LONG64) (expected) \
	) == (expected))
# define PHP_UCACHE_ATOMIC_LOAD_32(target) \
	((uint32_t) _InterlockedOr((volatile long *) (target), 0))
# define PHP_UCACHE_ATOMIC_STORE_32(target, value) \
	((void) _InterlockedExchange((volatile long *) (target), (long) (value)))
# define PHP_UCACHE_ATOMIC_CAS_32(target, expected, desired) \
	((uint32_t) _InterlockedCompareExchange((volatile long *) (target), (long) (desired), (long) (expected)) == (expected))
# define PHP_UCACHE_ATOMIC_LOAD_32_RELAXED(target) \
	(*(volatile uint32_t *) (target))
# define PHP_UCACHE_ATOMIC_STORE_32_RELAXED(target, value) \
	((void) (*(volatile uint32_t *) (target) = (value)))
# define PHP_UCACHE_ATOMIC_FENCE_SEQ_CST()	MemoryBarrier()
# define PHP_UCACHE_ATOMIC_FENCE_ACQUIRE()	MemoryBarrier()
#else /* !PHP_UCACHE_OPTIMISTIC_MSVC */
# define PHP_UCACHE_ATOMIC_LOAD_64(target) \
	__atomic_load_n((target), __ATOMIC_ACQUIRE)
# define PHP_UCACHE_ATOMIC_STORE_64(target, value) \
	__atomic_store_n((target), (value), __ATOMIC_RELEASE)
# define PHP_UCACHE_ATOMIC_CAS_64(target, expected, desired) \
	__atomic_compare_exchange_n((target), &(expected), (desired), false, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)
# define PHP_UCACHE_ATOMIC_LOAD_32(target) \
	__atomic_load_n((target), __ATOMIC_ACQUIRE)
# define PHP_UCACHE_ATOMIC_STORE_32(target, value) \
	__atomic_store_n((target), (value), __ATOMIC_RELEASE)
# define PHP_UCACHE_ATOMIC_CAS_32(target, expected, desired) \
	__atomic_compare_exchange_n((target), &(expected), (desired), false, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)
# define PHP_UCACHE_ATOMIC_LOAD_32_RELAXED(target) \
	__atomic_load_n((target), __ATOMIC_RELAXED)
# define PHP_UCACHE_ATOMIC_STORE_32_RELAXED(target, value) \
	__atomic_store_n((target), (value), __ATOMIC_RELAXED)
# define PHP_UCACHE_ATOMIC_FENCE_SEQ_CST()	__atomic_thread_fence(__ATOMIC_SEQ_CST)
# define PHP_UCACHE_ATOMIC_FENCE_ACQUIRE()	__atomic_thread_fence(__ATOMIC_ACQUIRE)
#endif /* PHP_UCACHE_OPTIMISTIC_MSVC */
#else /* !PHP_UCACHE_HAVE_OPTIMISTIC */
/* Single-threaded fallbacks: PHP_UCACHE_OPTIMISTIC_ENABLED gates off the
 * lock-free reader path, so every remaining consumer runs under the exclusive
 * global lock. */
# define PHP_UCACHE_OPTIMISTIC_ENABLED 0
# define PHP_UCACHE_ATOMIC_LOAD_64(target) (*(target))
# define PHP_UCACHE_ATOMIC_STORE_64(target, value) ((void) (*(target) = (value)))
# define PHP_UCACHE_ATOMIC_CAS_64(target, expected, desired) \
	(*(target) == (expected) ? ((*(target) = (desired)), true) : false)
# define PHP_UCACHE_ATOMIC_LOAD_32(target) (*(target))
# define PHP_UCACHE_ATOMIC_STORE_32(target, value) ((void) (*(target) = (value)))
# define PHP_UCACHE_ATOMIC_CAS_32(target, expected, desired) \
	(*(target) == (expected) ? ((*(target) = (desired)), true) : false)
# define PHP_UCACHE_ATOMIC_LOAD_32_RELAXED(target) (*(target))
# define PHP_UCACHE_ATOMIC_STORE_32_RELAXED(target, value) ((void) (*(target) = (value)))
# define PHP_UCACHE_ATOMIC_FENCE_SEQ_CST()	((void) 0)
# define PHP_UCACHE_ATOMIC_FENCE_ACQUIRE()	((void) 0)
#endif /* PHP_UCACHE_HAVE_OPTIMISTIC */

#ifdef PHP_UCACHE_HAVE_SHARED_MUTEX
typedef union {
	pthread_mutex_t mutex;
	char padding[64];
} php_ucache_shared_mutex_t;
#endif

#ifdef ZTS
# ifdef ZEND_WIN32
typedef SRWLOCK php_ucache_startup_lock;
# else
typedef pthread_mutex_t php_ucache_startup_lock;
# endif
#endif

#ifdef ZEND_WIN32
typedef struct _php_ucache_win32_segment {
	php_ucache_shm_segment_t segment;
	HANDLE memfile;
	void *mapping_base;
} php_ucache_win32_segment_t;
#endif

typedef enum {
	PHP_UCACHE_OPTIMISTIC_FALLBACK = 0,
	PHP_UCACHE_OPTIMISTIC_FOUND,
	PHP_UCACHE_OPTIMISTIC_MISS
} php_ucache_optimistic_result_t;

typedef enum {
	PHP_UCACHE_VERBATIM_ROOT_UNDECIDED = 0,
	PHP_UCACHE_VERBATIM_ROOT_SIZED,
	PHP_UCACHE_VERBATIM_ROOT_ELIGIBLE_UNSIZED,
	PHP_UCACHE_VERBATIM_ROOT_INELIGIBLE
} php_ucache_verbatim_root_result_t;

/* Array address to verbatim eligibility, content hash and the address of
 * the equal-content array whose copy it shares (itself when first); all
 * stay valid across CALC and COPY only while no state hook has run. */
typedef struct {
	HashTable verdicts;
	HashTable content_hashes;
	HashTable canonicals;
} php_ucache_verbatim_memo_t;

typedef struct {
	size_t configured_memory;
	php_ucache_reason_t failure_reason;
	bool enabled;
	bool available;
} php_ucache_runtime_t;

/* Global lock model dispatch table (fcntl / robust shared mutex / win32),
 * defined in user_cache_storage.c and selected at lock negotiation time. */
typedef struct _php_ucache_lock_ops php_ucache_lock_ops_t;

typedef struct {
	const php_ucache_shm_handlers_t *handler;
	const char *handler_name;
	php_ucache_shm_segment_t **segments;
	const php_ucache_lock_ops_t *lock_ops;
	size_t size;
	uint32_t segment_count;
	int lock_file;
	uint32_t startup_complete;
	uint32_t capacity_memo;
	uint32_t data_offset_memo;
	uint32_t entry_lock_capacity_memo;
	uint32_t entry_lock_offset_memo;
	uint32_t occupancy_offset_memo;
	uint32_t intern_capacity_memo;
	uint32_t intern_offset_memo;
	bool initialized;
	bool initialized_before_request;
	bool lock_initialized;
	bool layout_memo_valid;
	bool capacity_clamped;
#ifdef PHP_UCACHE_HAVE_BOUNDARY_MMAP
	bool boundary_digest_memoized;
	uint8_t boundary_digest_memo[32];
	bool boundary_salt_loaded;
	uint8_t boundary_salt[PHP_UCACHE_BOUNDARY_SALT_SIZE];
#endif
	char lockfile_name[MAXPATHLEN];
#ifdef ZTS
	MUTEX_T zts_lock;
#endif
} php_ucache_storage_t;

typedef struct {
	php_ucache_storage_t storage;
	const char *lock_name;
#ifndef ZEND_WIN32
	const char *sem_filename_prefix;
#endif
	bool boundary_shared;
	const char *boundary_identity;
	size_t boundary_identity_len;
} php_ucache_ctx_t;

struct _php_ucache_partition {
	php_ucache_ctx_t ctx;
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
} php_ucache_entry_lock_record_t;

typedef struct {
	php_ucache_ctx_t *ctx;
	char *key;
	uint64_t owner_pid;
	uint64_t owner_start_time;
	uint64_t owner_token;
	zend_long lease;
	uint32_t key_len;
	bool preserve_lease;
} php_ucache_deferred_entry_lock_release_t;

typedef struct {
	uint64_t owner_pid;
	uint64_t owner_start_time;
	uint32_t active;
	uint32_t reserved;
} php_ucache_reader_slot_t;

/* Open-addressed table over interned strings: the tag holds the high hash
 * bits (the whole hash on ILP32), str_offset 0 is an empty slot. Written
 * only under the write lock and rebuilt by the mark-sweep; encoders probe
 * it lock-free and revalidate intern_generation under the write lock before
 * a payload that references an interned string is published. */
typedef struct {
	uint32_t str_offset;
	uint32_t tag;
} php_ucache_intern_slot_t;

/* Per-pool lookup counters in an open-addressed table keyed by pool name;
 * the name bytes live in a data-region block. Requests accumulate counts
 * locally and fold them in under the write lock, so the hot fetch path
 * never touches this table. */
typedef struct {
	zend_ulong name_hash;
	uint32_t name_offset;
	uint32_t name_len;
	uint32_t state;
	uint32_t reserved;
	uint64_t hit_count;
	uint64_t miss_count;
} php_ucache_pool_stats_t;

/* Per-process record of shared-graph payload pins, claimed lock-free under
 * the read lock or an optimistic reader section. The pid lives in a
 * zend_atomic_int because, unlike reader slots, pin claims must also work on
 * builds without the 64-bit atomic macros; pids fit in 32 bits on every
 * supported platform. Pins of an abnormally terminated owner are stripped by
 * php_ucache_shared_graph_strip_dead_pins_locked() via pid + start-time
 * liveness, which is what makes retired-but-pinned payloads reclaimable. */
typedef struct {
	zend_atomic_int owner_pid;
	zend_atomic_int pin_count;
	uint64_t owner_start_time;
} php_ucache_graph_pin_slot_t;

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
	uint32_t boundary_identity_digest_set;
	uint64_t mutation_epoch;
	uint64_t write_seq;
	/* Epoch for relative second stamps (entry TTLs, lock leases): set once at
	 * format time so 32-bit deadlines stay valid for ~136 years of segment
	 * lifetime instead of breaking at an absolute wall-clock horizon. */
	uint64_t time_base;
	uint64_t expunge_count;
	uint64_t store_failure_count;
	uint64_t eviction_count;
	/* Cumulative dead-pin recovery statistics: owners whose slots the sweep
	 * reclaimed, and payload references stripped from them. */
	uint64_t graph_dead_pin_owners_reclaimed;
	uint64_t graph_dead_pins_stripped;
	/* Segment-lifetime fetch lookups, folded in at request end. */
	uint64_t hit_count;
	uint64_t miss_count;
	uint32_t tombstone_count;
	uint32_t expiring_count;
	uint32_t lock_model;
	uint32_t orphaned_graphs_saturated;
	/* Shared clock hand for LRU victim scans; fairness only, never trusted. */
	uint32_t eviction_hand;
	/* Power-of-two record count and segment offset of the per-key lock
	 * region (header -> entries -> stamps -> occupancy -> intern slots ->
	 * lock records -> data), and the regions before it. */
	uint32_t entry_lock_capacity;
	uint32_t entry_lock_offset;
	uint32_t occupancy_offset;
	uint32_t intern_capacity;
	uint32_t intern_offset;
	uint32_t intern_count;
	/* Set once an insert found the table at its load limit; cleared by a
	 * sweep that frees room. */
	uint32_t intern_saturated;
	/* Bumped by every sweep or reset: a payload prepared against an older
	 * generation may reference strings that no longer exist. */
	uint64_t intern_generation;
	uint64_t intern_sweep_count;
	/* Bumped under the write lock for every inserted lock record; the new
	 * value becomes that record's owner_token. */
	uint64_t entry_lock_acquire_seq;
	/* A file-backed boundary image left by an earlier boot may be torn or
	 * hold a robust mutex nobody can release; startup reformats it instead
	 * of adopting it. */
	uint64_t boot_token;
	uint8_t boundary_identity_digest[32];
	uint32_t orphaned_graphs[PHP_UCACHE_ORPHANED_GRAPH_SLOTS];
	php_ucache_pool_stats_t pool_stats[PHP_UCACHE_POOL_STATS_SLOTS];
	php_ucache_graph_pin_slot_t graph_pin_slots[PHP_UCACHE_GRAPH_PIN_SLOTS];
	php_ucache_reader_slot_t reader_slots[PHP_UCACHE_READER_SLOTS];
#ifdef PHP_UCACHE_HAVE_SHARED_MUTEX
	php_ucache_shared_mutex_t global_shared_mutex;
#endif
} php_ucache_header_t;

/* Sizes and offsets count PHP_UCACHE_SHM_UNIT bytes. */
typedef struct {
	uint32_t size;
	uint32_t prev_size;
	uint32_t next_free;
	uint32_t prev_free;
	uint32_t flags;
} php_ucache_block_t;

ZEND_STATIC_ASSERT(sizeof(php_ucache_block_t) % PHP_UCACHE_SHM_UNIT == 0, "block header must be unit aligned");

/* 48 bytes, hole-free on LP64. Probe-order layout: the fields a miss
 * rejection reads (hash, key, state, expiry) fill the first 24 bytes;
 * hit-only fields (scalar union, generation, value block) trail. */
typedef struct {
	zend_ulong hash;
	uint32_t key_offset;
	uint32_t key_len;
	/* Seconds relative to header time_base; 0 = never expires. */
	uint32_t expires_at;
	uint8_t state;
	uint8_t value_type;
	uint16_t flags;
	union {
		zend_long long_value;
		double double_value;
	};
	uint64_t generation;
	uint32_t value_offset;
	uint32_t value_len;
} php_ucache_entry_t;

typedef struct {
	bool found;
	php_ucache_entry_t entry;
} php_ucache_replaced_entry_t;

typedef struct {
	zend_ulong hash;
	uint64_t mutation_epoch;
	const void *ctx;
	uint32_t slot_index;
	uint8_t state;
	uint8_t value_type;
	zend_string *key;
	union {
		zend_long long_value;
		double double_value;
	};
} php_ucache_lookup_entry_t;

ZEND_STATIC_ASSERT(PHP_UCACHE_LOOKUP_WAYS == 2, "lookup cache is two-way");

/* A payload field referring to a candidate's in-payload copy; patched at
 * publish once the candidate is interned. */
typedef struct {
	uint32_t candidate;
	uint32_t site_offset;
	bool pointer;
} php_ucache_graph_intern_site_t;

/* Interning state carried from CALC/COPY (outside the lock) to publish. */
typedef struct {
	uint64_t generation;
	uint32_t list_capacity;
	uint32_t list_offset;
	uint32_t list_count;
	/* In-payload string offsets; publish overwrites each with its segment
	 * offset (0 when it stayed in the payload). */
	uint32_t *candidates;
	uint32_t candidate_count;
	uint32_t candidate_capacity;
	php_ucache_graph_intern_site_t *sites;
	uint32_t site_count;
	uint32_t site_capacity;
	bool enabled;
	/* Two lookups saw different generations: publish must re-prepare. */
	bool generation_conflict;
} php_ucache_graph_intern_plan_t;

typedef enum {
	PHP_UCACHE_PUBLISH_DONE = 0,
	PHP_UCACHE_PUBLISH_FAILED,
	/* An intern sweep ran since the payload was prepared. */
	PHP_UCACHE_PUBLISH_STALE_INTERNS
} php_ucache_publish_result_t;

typedef struct {
	zend_ulong hash;
	size_t payload_size;
	size_t payload_used_size;
	const uint8_t *payload_source;
	uint8_t *owned_buffer;
	zend_string *owned_string;
	/* Owned by the prepared value. */
	uint32_t *fixup_offsets;
	union {
		zend_long long_value;
		double double_value;
	};
	HashTable *state_memo;
	php_ucache_graph_intern_plan_t intern;
	uint32_t value_len;
	uint32_t fixup_count;
	uint8_t value_type;
	bool has_verbatim_array;
} php_ucache_prepared_value_t;

typedef struct {
	bool caller_holds_write_lock;
	bool disable_interning;
	/* A memo from an earlier prepare of the same value: its state hooks
	 * are not run again, so the re-prepare may hold the write lock. */
	HashTable *state_memo;
} php_ucache_prepare_options_t;

typedef struct {
	bool retry_after_memory_pressure;
	bool capture_replaced_entry;
} php_ucache_store_options_t;

typedef struct {
	php_ucache_replaced_entry_t replaced_entry;
	uint64_t stored_generation;
	bool should_seed_request_local_slot;
} php_ucache_store_result_t;

/* Deferred until after the read lock is released. */
typedef struct {
	uint64_t generation;
	uint32_t flags;
	bool should_seed_request_local_slot;
} php_ucache_fetch_pending_seed_t;

typedef struct {
	zend_long new_value;
	bool is_overflow;
	bool is_type_error;
} php_ucache_atomic_update_result_t;

typedef struct {
	uint32_t magic;
	uint32_t version;
	uint32_t root_offset;
	uint32_t root_type;
	uint32_t flags;
	/* Segment unit offsets of the interned strings this payload references
	 * (a uint32_t list inside the payload); the intern sweep marks them. */
	uint32_t intern_list_offset;
	uint32_t intern_count;
	zend_atomic_int ref_state;
	/* Bit per graph_pin_slots index: which owners hold ref_state references.
	 * A reference taken without a claimable slot sets no bit and stays
	 * unreclaimable if its owner crashes (the pre-record behavior). */
	zend_atomic_int pin_owners[PHP_UCACHE_GRAPH_PIN_WORDS];
} php_ucache_shared_graph_header_t;

typedef struct {
	php_ucache_ctx_t *ctx;
	uint32_t payload_offset;
} php_ucache_shared_graph_ref_t;

typedef union {
	zend_long long_value;
	double double_value;
	uint64_t offset;
} php_ucache_shared_graph_payload_t;

/* A single value (root, reference inner, safe-direct state). Sequences of
 * values live in column blocks instead: a type column and an 8-byte
 * payload column, plus key or property-name columns, each padded to 8
 * bytes (ucache_graph_*_columns_* in user_cache_shared_graph.c). */
typedef struct {
	uint8_t type;
	uint8_t reserved[7];
	php_ucache_shared_graph_payload_t payload;
} php_ucache_shared_graph_value_t;

typedef struct {
	uint32_t count;
	uint32_t next_free;
	uint32_t elements_offset;
	uint32_t flags;
} php_ucache_shared_graph_array_t;

typedef struct {
	uint32_t key_offset;
} php_ucache_shared_graph_array_shape_element_t;

typedef struct {
	uint32_t count;
	uint32_t elements_offset;
} php_ucache_shared_graph_array_shape_t;

typedef struct {
	uint32_t count;
	uint32_t next_free;
	uint32_t shape_offset;
	uint32_t flags;
	uint32_t values_offset;
} php_ucache_shared_graph_shaped_array_t;

typedef struct {
	uint32_t class_name_offset;
	uint32_t property_count;
	uint32_t properties_offset;
	uint32_t flags;
} php_ucache_shared_graph_object_t;

typedef struct {
	uint32_t class_name_offset;
	uint32_t shape_offset;
	uint32_t count;
} php_ucache_shared_graph_state_schema_t;

typedef struct {
	uint32_t state_schema_offset;
	uint32_t flags;
	uint32_t state_values_offset;
	uint32_t state_next_free;
} php_ucache_shared_graph_shaped_state_object_t;

/* Shared layout for safe-direct and serialized object state. */
typedef struct {
	uint32_t class_name_offset;
	uint32_t property_count;
	uint32_t properties_offset;
	uint32_t flags;
	php_ucache_shared_graph_value_t state;
} php_ucache_shared_graph_safe_direct_object_t;

typedef struct {
	uint32_t blob_len;
	uint32_t flags;
} php_ucache_shared_graph_serdes_object_t;

typedef struct {
	uint32_t flags;
	uint32_t reserved;
	php_ucache_shared_graph_value_t inner;
} php_ucache_shared_graph_reference_t;

typedef struct {
	uint32_t class_name_offset;
	uint32_t case_name_offset;
} php_ucache_shared_graph_enum_t;

typedef struct {
	php_ucache_header_t *header;
	uint32_t slot_index;
} php_ucache_reader_claim_t;

typedef struct {
	php_ucache_header_t *header;
	uint32_t slot_index;
} php_ucache_graph_pin_claim_t;

typedef struct {
	php_ucache_partition_t *active_partition;
	php_ucache_runtime_t runtime_state;
	php_ucache_ctx_t *active_context_ptr;
	php_ucache_reason_t request_unavailable_reason;
	bool lock_held;
	bool lock_held_is_write;
	bool runtime_resolved;
	bool runtime_resolved_enabled;
	const php_ucache_ctx_t *runtime_resolved_ctx;
	php_ucache_shared_graph_ref_t *shared_graph_refs;
	uint32_t shared_graph_ref_count;
	uint32_t shared_graph_ref_capacity;
	uint64_t shared_graph_ref_owner_pid;
	HashTable *shared_graph_ref_index;
	php_ucache_lookup_entry_t lookup_entry_storage[PHP_UCACHE_LOOKUP_BUCKETS];
	HashTable *request_local_slot_table;
	HashTable *entry_lock_table;
	php_ucache_deferred_entry_lock_release_t *deferred_entry_lock_releases;
	uint32_t deferred_entry_lock_release_count;
	uint32_t deferred_entry_lock_release_capacity;
	HashTable *pool_table;
	HashTable *decode_identity_map;
	HashTable *decode_reference_map;
	HashTable *decode_array_map;
	HashTable *decode_resolve_cache;
	HashTable *decode_shape_prototype_cache;
	HashTable *object_route_memo;
	/* Segment bounds of the payload being decoded, for interned strings
	 * referenced by segment offset (set and restored per decode). */
	const uint8_t *decode_segment_base;
	size_t decode_segment_len;
	const void *decode_resolve_direct_keys[PHP_UCACHE_DECODE_DIRECT_CACHE_SLOTS];
	void *decode_resolve_direct_values[PHP_UCACHE_DECODE_DIRECT_CACHE_SLOTS];
	const void *decode_shape_prototype_direct_keys[PHP_UCACHE_DECODE_DIRECT_CACHE_SLOTS];
	zend_array *decode_shape_prototype_direct_values[PHP_UCACHE_DECODE_DIRECT_CACHE_SLOTS];
	php_ucache_reader_claim_t reader_claims[PHP_UCACHE_READER_CLAIM_MAX];
	php_ucache_graph_pin_claim_t graph_pin_claims[PHP_UCACHE_GRAPH_PIN_CLAIM_MAX];
#ifndef ZEND_WIN32
	uint64_t entry_lock_owner_pid;
#endif /* !ZEND_WIN32 */
	uint64_t graph_pin_probe_last_at;
	uint64_t entry_lock_owner_probe_pid;
	uint64_t entry_lock_owner_probe_start_time;
	uint64_t entry_lock_owner_probe_at;
	uint64_t self_start_time_pid; /* Recomputed after fork. */
	uint64_t self_start_time_token;
	zend_long shm_size;
	zend_long entries_hint;
	zend_long eviction_policy;
	char *lockfile_path;
	char *memory_model;
	uint32_t reader_claim_count;
	uint32_t graph_pin_claim_count;
	uint32_t expired_read_observations;
	uint32_t expunge_write_ops; /* Write operations since the last bounded expiry scan. */
	uint32_t intern_sweep_ticks; /* Allocation-pressure reclaims since the last intern sweep. */
	uint32_t expired_expunge_cursor;
	/* Request-lazy coarse clock for access stamps; refreshed opportunistically
	 * and after a bounded number of touches (entries.c). */
	uint32_t access_now;
	uint32_t access_now_touches;
	uint8_t decode_resolve_direct_next;
	uint8_t decode_shape_prototype_direct_next;
	bool write_seq_bumped;
	bool stack_overflowed;
	/* Set while a bulk store commits its items so a per-item bailout keeps the
	 * write lock held: the bulk rollback then runs without a lock gap another
	 * process could mutate through. */
	bool store_defer_unlock;
	bool request_local_slot_may_cycle; /* Request shutdown must collect cyclic slot clones. */
	int8_t reader_drain_state;
	bool entry_lock_owner_probe_dead;
	bool in_request_shutdown;
	bool enable;
	bool enable_cli;
} php_ucache_globals;

#ifdef ZTS
# define UC_G(v) ZEND_TSRMG_FAST(user_cache_globals_offset, php_ucache_globals *, v)
extern size_t user_cache_globals_offset;
#else
# define UC_G(v) (user_cache_globals.v)
extern php_ucache_globals user_cache_globals;
#endif

extern php_ucache_ctx_t php_ucache_ctx_state;
extern bool php_ucache_runtime_opted_in;
extern php_ucache_partition_t *php_ucache_partitions;

/* Guards boundary partition lookup/creation and request-time additions to
 * php_ucache_partitions; no-ops on non-ZTS builds. */
void php_ucache_boundary_partitions_lock(void);
void php_ucache_boundary_partitions_unlock(void);
uint64_t php_ucache_cached_pid(void);
void php_ucache_reset_runtime(void);
void php_ucache_reset_storage(void);
bool php_ucache_header_init_locked(void);
bool php_ucache_header_adoptable_locked(void);
void php_ucache_free_locked(uint32_t payload_offset);
uint32_t php_ucache_alloc_locked(size_t size, const void *src);
bool php_ucache_alloc_can_satisfy_locked(size_t size, size_t key_size);
/* Lock-free; *generation is the value to confirm under the write lock. */
uint32_t php_ucache_intern_find(zend_string *str, uint64_t *generation);
uint32_t php_ucache_intern_add_locked(zend_string *str);
void php_ucache_intern_table_reset_locked(void);
void php_ucache_intern_table_insert_locked(uint32_t str_offset);
bool php_ucache_shared_graph_intern_sweep_locked(void);
bool php_ucache_startup_storage_before_request(void);
void php_ucache_shutdown_storage(void);
void php_ucache_ensure_ready_impl(void);
bool php_ucache_rlock(void);
bool php_ucache_wlock(void);
bool php_ucache_wlock_for_entry_mutation(zend_string *key);
bool php_ucache_wlock_for_ref_release(bool *write_section_entered);
void php_ucache_unlock(void);
void php_ucache_unlock_if_held(void);
bool php_ucache_try_acquire_entry_lock(zend_string *key, zend_long lease);
bool php_ucache_release_entry_lock(zend_string *key);
bool php_ucache_request_owns_entry_lock(zend_string *key);
/* Keys must be sorted with duplicates adjacent. */
bool php_ucache_acquire_entry_locks(zend_string **keys, bool *acquired, uint32_t count);
void php_ucache_release_entry_locks(zend_string **keys, const bool *acquired, uint32_t count);
bool php_ucache_entry_locks_allow_clear_locked(void);
/* *now is time_base-relative seconds, lazily filled when passed as 0. */
bool php_ucache_entry_key_lock_active_locked(
		php_ucache_header_t *header,
		zend_ulong hash,
		uint32_t key_offset,
		uint32_t key_len,
		uint64_t *now);
void php_ucache_release_request_entry_locks(void);
#ifdef ZTS
void php_ucache_free_thread_deferred_entry_lock_releases(php_ucache_globals *globals);
#endif /* ZTS */
const php_ucache_safe_direct_handlers_t *php_ucache_safe_direct_find_handlers(
		zend_class_entry *ce,
		zend_class_entry **base_ce_ptr);
php_ucache_safe_direct_state_copy_func_t php_ucache_safe_direct_state_copy_func(
		zend_class_entry *ce,
		zend_class_entry **base_ce_ptr);
php_ucache_safe_direct_state_has_unstorable_func_t php_ucache_safe_direct_state_has_unstorable_func(
		zend_class_entry *ce);
php_ucache_safe_direct_state_serialize_func_t php_ucache_safe_direct_state_serialize_func(
		zend_class_entry *ce);
php_ucache_safe_direct_state_unserialize_func_t php_ucache_safe_direct_state_unserialize_func(
		zend_class_entry *ce);
bool php_ucache_safe_direct_prefers_request_local_prototype(zend_class_entry *ce);
bool php_ucache_serdes_encode(zval *value, smart_str *buf, const char **failure_msg);
bool php_ucache_serdes_decode(const uint8_t *data, size_t len, zval *dst);
uint32_t php_ucache_serdes_declared_property_index_plus_one(
		zend_class_entry *ce,
		zend_string *name);
bool php_ucache_serdes_get_sleep_state(
		zval *obj_zv,
		zval *state,
		const char **failure_msg);
bool php_ucache_serdes_call_magic_serialize(zend_object *obj, zval *state);
bool php_ucache_shared_graph_update_object_property(
		zval *obj_zv,
		zend_string *prop_name,
		zval *prop_val);
bool php_ucache_shared_graph_update_object_property_at(
		zval *obj_zv,
		zend_string *prop_name,
		uint32_t prop_idx,
		zval *prop_val);
bool php_ucache_shared_graph_can_copy_verbatim_root(const zval *value, php_ucache_verbatim_memo_t *verbatim_memo);
php_ucache_verbatim_root_result_t php_ucache_shared_graph_calc_verbatim_root(
		const zval *value,
		php_ucache_verbatim_memo_t *verbatim_memo,
		size_t *buf_len,
		uint32_t *intern_key_count);
bool php_ucache_calculate_shared_graph_size(
		const zval *value,
		HashTable *state_memo,
		php_ucache_verbatim_memo_t *verbatim_memo,
		size_t *buf_len,
		uint32_t *intern_key_count);
bool php_ucache_build_shared_graph_in_place(
		const zval *value,
		HashTable *state_memo,
		php_ucache_verbatim_memo_t *verbatim_memo,
		uint8_t *buf,
		size_t buf_len,
		size_t *graph_len,
		bool *has_verbatim_array,
		uint32_t **fixup_offsets,
		uint32_t *fixup_count,
		php_ucache_graph_intern_plan_t *intern_plan);
void php_ucache_graph_intern_plan_destroy(php_ucache_graph_intern_plan_t *plan);
bool php_ucache_shared_graph_copy_fits_buffer(
		const uint8_t *dst_buf,
		const uint8_t *src_buf,
		size_t buf_len,
		size_t src_graph_len);
bool php_ucache_shared_graph_decode(const uint8_t *buf, size_t buf_len, zval *dst);
bool php_ucache_shared_graph_prefers_prototype(uint32_t payload_offset);
bool php_ucache_shared_graph_decode_is_lock_safe(uint32_t payload_offset);
bool php_ucache_shared_graph_payload_has_aliases(uint32_t payload_offset);
void php_ucache_decode_resolve_cache_release(void);
void php_ucache_decode_shape_prototype_cache_release(void);
void php_ucache_decode_maps_teardown(void);
bool php_ucache_shared_graph_can_overwrite_payload_locked(uint32_t payload_offset);
bool php_ucache_shared_graph_payload_has_refs_locked(uint32_t payload_offset);
php_ucache_publish_result_t php_ucache_shared_graph_publish_copied_payload_locked(
		uint8_t *dst_buf,
		const uint8_t *src_buf,
		size_t buf_len,
		size_t src_graph_len,
		bool has_verbatim_array,
		const uint32_t *fixup_offsets,
		uint32_t fixup_count,
		php_ucache_graph_intern_plan_t *intern_plan);
bool php_ucache_shared_graph_acquire_ref(uint32_t payload_offset);
bool php_ucache_shared_graph_retire_payload_locked(uint32_t payload_offset);
bool php_ucache_has_request_shared_graph_ref(uint32_t payload_offset);
void php_ucache_register_shared_graph_ref(uint32_t payload_offset);
bool php_ucache_release_request_shared_graph_refs(void);
void php_ucache_expunge_expired_at_request_end(void);
void php_ucache_lookup_cache_clear(void);
bool php_ucache_prepare_value(
		zend_string *key,
		zval *value,
		const php_ucache_prepare_options_t *options,
		php_ucache_prepared_value_t *prepared);
void php_ucache_destroy_prepared_value(php_ucache_prepared_value_t *prepared);
bool php_ucache_store_prepared_locked(
		zend_string *key,
		zval *value,
		php_ucache_prepared_value_t *prepared,
		zend_long ttl,
		const php_ucache_store_options_t *options,
		php_ucache_store_result_t *result);
/* May release the global lock while materializing a value: when *lock_held
 * comes back false the caller no longer holds the lock and must neither
 * unlock nor touch SHM state afterwards. */
bool php_ucache_fetch_locked(
		zend_string *key,
		bool use_request_local_slot,
		zval *return_value,
		bool *found,
		php_ucache_fetch_pending_seed_t *pending_seed,
		bool *lock_held);
void php_ucache_fetch_finish(zend_string *key, uint64_t gen, zval *return_value, uint32_t flags);
bool php_ucache_exists_locked(zend_string *key);
void php_ucache_delete_locked(zend_string *key);
void php_ucache_discard_replaced_entry_locked(zend_string *key, php_ucache_replaced_entry_t *replaced_entry);
void php_ucache_rollback_replaced_entry_locked(zend_string *key, php_ucache_replaced_entry_t *replaced_entry);
void php_ucache_delete_by_prefix_locked(zend_string *prefix);
bool php_ucache_atomic_update_locked(
		zend_string *key,
		zend_long step,
		zend_long ttl,
		bool decrement,
		php_ucache_atomic_update_result_t *result);
void php_ucache_release_request_local_slots(void);
void php_ucache_release_active_request_local_slots_by_prefix(zend_string *prefix);
void php_ucache_store_request_local_slot(zend_string *key, uint64_t gen, zval *value, bool no_aliases);
void php_ucache_object_table_dtor(zval *zv);
void php_ucache_reference_table_dtor(zval *zv);
php_ucache_optimistic_result_t php_ucache_fetch_optimistic(
		zend_string *key,
		zval *return_value,
		bool allow_decode);
php_ucache_optimistic_result_t php_ucache_exists_optimistic(zend_string *key);
bool php_ucache_optimistic_reader_begin(php_ucache_header_t *header, uint32_t *slot_idx_ptr);
void php_ucache_optimistic_reader_end(php_ucache_header_t *header, uint32_t slot_idx);
void php_ucache_optimistic_fork_setup(void);
#ifdef ZTS
void php_ucache_release_thread_reader_claims(php_ucache_globals *globals);
#endif
bool php_ucache_quiesce_graph_payloads_locked(void);
void php_ucache_shared_graph_ref_reserve(void);
void php_ucache_shared_graph_orphan_payload_locked(uint32_t payload_offset);
void php_ucache_shared_graph_reclaim_orphaned_locked(void);
bool php_ucache_shared_graph_strip_dead_pins_locked(bool force);
bool php_ucache_graph_pin_owner_is_dead(uint64_t owner_pid, uint64_t owner_start_time);
uint64_t php_ucache_self_start_time_token(void);
#ifdef ZTS
void php_ucache_release_thread_graph_pin_claims(php_ucache_globals *globals);
#endif /* ZTS */

static zend_always_inline uint64_t php_ucache_current_pid(void)
{
#ifdef ZEND_WIN32
	return (uint64_t) GetCurrentProcessId();
#else
	return (uint64_t) getpid();
#endif
}

static zend_always_inline bool php_ucache_stack_overflowed(void)
{
#ifdef ZEND_CHECK_STACK_LIMIT
	bool overflowed = UNEXPECTED(zend_call_stack_overflowed(EG(stack_limit)));

	if (overflowed) {
		UC_G(stack_overflowed) = true;
	}

	return overflowed;
#else
	return false;
#endif /* ZEND_CHECK_STACK_LIMIT */
}

static zend_always_inline uint64_t php_ucache_atomic_load_64(const uint64_t *target)
{
	return PHP_UCACHE_ATOMIC_LOAD_64(target);
}

static zend_always_inline void php_ucache_atomic_fence_acquire(void)
{
	PHP_UCACHE_ATOMIC_FENCE_ACQUIRE();
}

static zend_always_inline void php_ucache_atomic_store_64(uint64_t *target, uint64_t value)
{
	PHP_UCACHE_ATOMIC_STORE_64(target, value);
}

static zend_always_inline php_ucache_ctx_t *php_ucache_owning_context(void)
{
	return UC_G(active_partition) != NULL
		? &UC_G(active_partition)->ctx
		: &php_ucache_ctx_state
	;
}

static zend_always_inline php_ucache_ctx_t *php_ucache_active_context(void)
{
	if (UC_G(active_context_ptr) != NULL) {
		return UC_G(active_context_ptr);
	}

	return php_ucache_owning_context();
}

static zend_always_inline void *php_ucache_base(void)
{
	php_ucache_storage_t *storage = &php_ucache_active_context()->storage;

	if (!storage->initialized ||
		storage->segment_count != 1
	) {
		return NULL;
	}

	return storage->segments[0]->p;
}

static zend_always_inline php_ucache_header_t *php_ucache_header_ptr(void)
{
	return (php_ucache_header_t *) php_ucache_base();
}

static zend_always_inline size_t php_ucache_shm_bytes(uint32_t units)
{
	return (size_t) units * PHP_UCACHE_SHM_UNIT;
}

static zend_always_inline uint32_t php_ucache_shm_units(size_t bytes)
{
	ZEND_ASSERT(bytes % PHP_UCACHE_SHM_UNIT == 0);

	return (uint32_t) (bytes / PHP_UCACHE_SHM_UNIT);
}

static zend_always_inline uint8_t *php_ucache_ptr(uint32_t offset)
{
	return (uint8_t *) php_ucache_base() + php_ucache_shm_bytes(offset);
}

static zend_always_inline php_ucache_block_t *php_ucache_block_ptr(uint32_t offset)
{
	return (php_ucache_block_t *) php_ucache_ptr(offset);
}

static zend_always_inline size_t php_ucache_block_payload_capacity(uint32_t payload_offset)
{
	php_ucache_block_t *block;

	if (payload_offset < PHP_UCACHE_BLOCK_HEADER_UNITS) {
		return 0;
	}

	block = php_ucache_block_ptr(payload_offset - PHP_UCACHE_BLOCK_HEADER_UNITS);
	if (block->size < PHP_UCACHE_BLOCK_HEADER_UNITS) {
		return 0;
	}

	return php_ucache_shm_bytes(block->size - PHP_UCACHE_BLOCK_HEADER_UNITS);
}

/* A size is allocatable only if its aligned block still fits a 32-bit unit count. */
static zend_always_inline bool php_ucache_alloc_units(size_t size, uint32_t *units)
{
	if (size == 0 ||
		size > UINT32_MAX - sizeof(php_ucache_block_t) - (PHP_UCACHE_PLATFORM_ALIGNMENT - 1)
	) {
		return false;
	}

	*units = php_ucache_shm_units(PHP_UCACHE_ALIGNED_SIZE(sizeof(php_ucache_block_t) + size));

	return true;
}

static zend_always_inline bool php_ucache_payload_in_bounds(
		const php_ucache_header_t *header,
		uint32_t offset,
		uint64_t len)
{
	uint64_t limit = ((uint64_t) header->data_offset + header->data_size) * PHP_UCACHE_SHM_UNIT;

	return offset >= header->data_offset + PHP_UCACHE_BLOCK_HEADER_UNITS &&
		(uint64_t) php_ucache_shm_bytes(offset) + len <= limit
	;
}

static zend_always_inline bool php_ucache_block_is_free(const php_ucache_block_t *block)
{
	return (block->flags & PHP_UCACHE_BLOCK_FREE) != 0;
}

static zend_always_inline php_ucache_ctx_t *php_ucache_activate_context(php_ucache_ctx_t *ctx)
{
	php_ucache_ctx_t *previous = UC_G(active_context_ptr);

	UC_G(active_context_ptr) = ctx;

	return previous;
}

static zend_always_inline void php_ucache_restore_context(php_ucache_ctx_t *ctx)
{
	UC_G(active_context_ptr) = ctx;
}

static zend_always_inline php_ucache_runtime_t *php_ucache_active_runtime(void)
{
	return &UC_G(runtime_state);
}

static zend_always_inline uint64_t php_ucache_seq_load(const uint64_t *seq)
{
	return php_ucache_atomic_load_64(seq);
}

static zend_always_inline bool php_ucache_header_is_initialized_locked(void)
{
	php_ucache_header_t *header = php_ucache_header_ptr();

	return header != NULL &&
		header->magic == PHP_UCACHE_MAGIC &&
		header->version == PHP_UCACHE_VERSION
	;
}

static zend_always_inline uint64_t php_ucache_time_rel(const php_ucache_header_t *header, uint64_t now)
{
	return now > header->time_base ? now - header->time_base : 0;
}

/* 0 is reserved for "never expires"; the deadline saturates at UINT32_MAX,
 * i.e. ~136 years past the segment's format time. */
static zend_always_inline uint32_t php_ucache_expiry_deadline(
		const php_ucache_header_t *header,
		uint64_t now,
		zend_long ttl)
{
	uint64_t deadline = php_ucache_time_rel(header, now) + (uint64_t) ttl;

	if (deadline == 0) {
		deadline = 1;
	}

	return deadline > (uint64_t) UINT32_MAX
		? UINT32_MAX
		: (uint32_t) deadline
	;
}

static zend_always_inline void php_ucache_bump_mutation_epoch_locked(php_ucache_header_t *header)
{
	if (header == NULL) {
		return;
	}

	header->mutation_epoch++;
	if (header->mutation_epoch == 0) {
		header->mutation_epoch = 1;
	}
}

static zend_always_inline php_ucache_entry_t *php_ucache_entries_ptr(php_ucache_header_t *header)
{
	return (php_ucache_entry_t *) ((char *) header + sizeof(php_ucache_header_t));
}

/* Advisory LRU stamps live in a parallel array behind the entry table so the
 * optimistic readers' plain 64B entry snapshots never share an address with
 * their relaxed stamp stores (a formal ZTS/TSan race otherwise). */
static zend_always_inline uint32_t *php_ucache_access_stamps_ptr(php_ucache_header_t *header)
{
	return (uint32_t *) (
		(char *) header
		+ sizeof(php_ucache_header_t)
		+ (size_t) header->capacity * sizeof(php_ucache_entry_t)
	);
}

/* Valid only on an adopted (layout-verified) or freshly formatted header:
 * the offset is trusted SHM state. */
static zend_always_inline php_ucache_entry_lock_record_t *php_ucache_entry_lock_records_ptr(php_ucache_header_t *header)
{
	return (php_ucache_entry_lock_record_t *) ((uint8_t *) header + php_ucache_shm_bytes(header->entry_lock_offset));
}

/* One bit per USED entry slot, so whole-table scans cost the live count
 * rather than the capacity. Mutated only under the write lock; readers
 * hold at least the read lock (optimistic readers never consult it). */
static zend_always_inline zend_ulong *php_ucache_occupancy_ptr(php_ucache_header_t *header)
{
	return (zend_ulong *) ((uint8_t *) header + php_ucache_shm_bytes(header->occupancy_offset));
}

static zend_always_inline void php_ucache_occupancy_set(php_ucache_header_t *header, uint32_t slot_idx)
{
	php_ucache_occupancy_ptr(header)[slot_idx / PHP_UCACHE_OCCUPANCY_WORD_BITS] |=
		(zend_ulong) 1 << (slot_idx % PHP_UCACHE_OCCUPANCY_WORD_BITS)
	;
}

static zend_always_inline void php_ucache_occupancy_clear(php_ucache_header_t *header, uint32_t slot_idx)
{
	php_ucache_occupancy_ptr(header)[slot_idx / PHP_UCACHE_OCCUPANCY_WORD_BITS] &=
		~((zend_ulong) 1 << (slot_idx % PHP_UCACHE_OCCUPANCY_WORD_BITS))
	;
}

static zend_always_inline void php_ucache_occupancy_reset(php_ucache_header_t *header)
{
	memset(php_ucache_occupancy_ptr(header), 0, PHP_UCACHE_OCCUPANCY_BYTES(header->capacity));
}

/* First USED slot at or after from, or UINT32_MAX. */
static zend_always_inline uint32_t php_ucache_occupancy_next_used(
		php_ucache_header_t *header,
		uint32_t from)
{
	const zend_ulong *words = php_ucache_occupancy_ptr(header);
	zend_ulong word;
	size_t word_idx, word_count;

	if (from >= header->capacity) {
		return UINT32_MAX;
	}

	word_count = PHP_UCACHE_OCCUPANCY_WORDS(header->capacity);
	word_idx = from / PHP_UCACHE_OCCUPANCY_WORD_BITS;
	word = words[word_idx] & (~(zend_ulong) 0 << (from % PHP_UCACHE_OCCUPANCY_WORD_BITS));

	for (;;) {
		if (word != 0) {
			return (uint32_t) (word_idx * PHP_UCACHE_OCCUPANCY_WORD_BITS + (uint32_t) zend_ulong_ntz(word));
		}

		if (++word_idx == word_count) {
			return UINT32_MAX;
		}

		word = words[word_idx];
	}
}

static zend_always_inline php_ucache_intern_slot_t *php_ucache_intern_slots_ptr(php_ucache_header_t *header)
{
	return (php_ucache_intern_slot_t *) ((uint8_t *) header + php_ucache_shm_bytes(header->intern_offset));
}

static zend_always_inline uint32_t php_ucache_intern_tag(zend_ulong hash)
{
#if SIZEOF_ZEND_LONG > 4
	return (uint32_t) (hash >> 32);
#else
	return (uint32_t) hash;
#endif
}

static zend_always_inline uint32_t php_ucache_intern_load_limit(uint32_t capacity)
{
	return capacity - capacity / 4;
}

static zend_always_inline bool php_ucache_intern_eligible(const zend_string *str)
{
	return ZSTR_LEN(str) != 0 && ZSTR_LEN(str) <= PHP_UCACHE_INTERN_MAX_LEN;
}

/* Optimistic readers may still issue relaxed stamp stores after their
 * sequence check passes, so locked writers must also keep every stamp
 * access on relaxed atomics instead of plain bulk stores. */
static zend_always_inline void php_ucache_access_stamps_reset(php_ucache_header_t *header)
{
	uint32_t i, *stamps = php_ucache_access_stamps_ptr(header);

	for (i = 0; i < header->capacity; i++) {
		PHP_UCACHE_ATOMIC_STORE_32_RELAXED(&stamps[i], 0);
	}
}

static zend_always_inline bool php_ucache_seen_test_and_add(HashTable *seen, const void *ptr)
{
	zend_ulong key = (zend_ulong) (uintptr_t) ptr;

	if (zend_hash_index_exists(seen, key)) {
		return false;
	}

	return zend_hash_index_add_empty_element(seen, key) != NULL;
}

static inline bool php_ucache_class_overrides_safe_direct_magic_serialize_ex(
		zend_class_entry *ce,
		const php_ucache_safe_direct_handlers_t *handlers,
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

static inline bool php_ucache_class_has_sleep(zend_class_entry *ce)
{
	return zend_hash_find_known_hash(&ce->function_table, ZSTR_KNOWN(ZEND_STR_SLEEP)) != NULL;
}

static inline bool php_ucache_class_has_wakeup(zend_class_entry *ce)
{
	return zend_hash_find_known_hash(&ce->function_table, ZSTR_KNOWN(ZEND_STR_WAKEUP)) != NULL;
}

static inline bool php_ucache_class_magic_route_active(zend_class_entry *ce)
{
	const php_ucache_safe_direct_handlers_t *handlers;
	zend_class_entry *base_ce = NULL;

	if (ce->ce_flags & (ZEND_ACC_NOT_SERIALIZABLE|ZEND_ACC_ENUM)) {
		return false;
	}

	handlers = php_ucache_safe_direct_find_handlers(ce, &base_ce);

	if (php_ucache_class_overrides_safe_direct_magic_serialize_ex(ce, handlers, base_ce)) {
		return true;
	}

	return handlers == NULL;
}

static inline bool php_ucache_class_uses_magic_serialize(zend_class_entry *ce)
{
	return ce->__serialize != NULL && ce->__unserialize != NULL &&
		php_ucache_class_magic_route_active(ce)
	;
}

static inline bool php_ucache_class_uses_serialize_props(zend_class_entry *ce)
{
	/* Native serialization restores this state as properties. */
	return ce->__serialize != NULL && ce->__unserialize == NULL &&
		php_ucache_class_magic_route_active(ce)
	;
}

static inline bool php_ucache_class_uses_magic_unserialize(zend_class_entry *ce)
{
	if (ce->__serialize != NULL || ce->__unserialize == NULL) {
		return false;
	}

	/* Match native serialization precedence. */
	if (ce->serialize != NULL && ce->unserialize != NULL) {
		return false;
	}

	return php_ucache_class_magic_route_active(ce);
}

static inline bool php_ucache_class_uses_serdes(zend_class_entry *ce)
{
	if (ce->ce_flags & (ZEND_ACC_NOT_SERIALIZABLE|ZEND_ACC_ENUM)) {
		return false;
	}

	if (php_ucache_class_uses_magic_serialize(ce) ||
		php_ucache_class_uses_magic_unserialize(ce) ||
		php_ucache_safe_direct_find_handlers(ce, NULL) != NULL
	) {
		return false;
	}

	if (php_ucache_class_has_sleep(ce) ||
		php_ucache_class_has_wakeup(ce)
	) {
		return true;
	}

	return ce->serialize != NULL &&
		ce->unserialize != NULL
	;
}

/* Resolves a cached case name without trusting it; mirrors unserialize(). */
static inline zend_object *php_ucache_enum_case_find(zend_class_entry *ce, zend_string *name)
{
	zend_class_constant *c;

	c = zend_hash_find_ptr(CE_CONSTANTS_TABLE(ce), name);
	if (c == NULL || !(ZEND_CLASS_CONST_FLAGS(c) & ZEND_CLASS_CONST_IS_CASE)) {
		return NULL;
	}

	if (Z_TYPE(c->value) == IS_CONSTANT_AST &&
		zval_update_constant_ex(&c->value, c->ce) == FAILURE
	) {
		return NULL;
	}

	if (Z_TYPE(c->value) != IS_OBJECT) {
		return NULL;
	}

	return Z_OBJ(c->value);
}

#endif /* PHP_USER_CACHE_INTERNAL_H */
