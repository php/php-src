/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2016 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   |          Dmitry Stogov <dmitry@zend.com>                             |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "zend.h"
#include "zend_alloc.h"
#include "zend_globals.h"
#include "zend_operators.h"

#ifdef HAVE_SIGNAL_H
# include <signal.h>
#endif
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#ifdef ZEND_WIN32
# include <wincrypt.h>
# include <process.h>
#endif

#ifndef ZEND_MM_HEAP_PROTECTION
# define ZEND_MM_HEAP_PROTECTION ZEND_DEBUG
#endif

#ifndef ZEND_MM_SAFE_UNLINKING
# define ZEND_MM_SAFE_UNLINKING 1
#endif

#ifndef ZEND_MM_COOKIES
# define ZEND_MM_COOKIES ZEND_DEBUG
#endif

#ifdef _WIN64
# define PTR_FMT "0x%0.16I64x"
/*
#elif sizeof(long) == 8
# define PTR_FMT "0x%0.16lx"
*/
#else
# define PTR_FMT "0x%0.8lx"
#endif

#if ZEND_DEBUG
void zend_debug_alloc_output(char *format, ...)
{
	char output_buf[256];
	va_list args;

	va_start(args, format);
	vsprintf(output_buf, format, args);
	va_end(args);

#ifdef ZEND_WIN32
	OutputDebugString(output_buf);
#else
	fprintf(stderr, "%s", output_buf);
#endif
}
#endif

#if (defined (__GNUC__) && __GNUC__ > 2 ) && !defined(__INTEL_COMPILER) && !defined(DARWIN) && !defined(__hpux) && !defined(_AIX)
static void zend_mm_panic(const char *message) __attribute__ ((noreturn));
#endif

static void zend_mm_panic(const char *message)
{
	fprintf(stderr, "%s\n", message);
/* See http://support.microsoft.com/kb/190351 */
#ifdef PHP_WIN32
	fflush(stderr);
#endif
#if ZEND_DEBUG && defined(HAVE_KILL) && defined(HAVE_GETPID)
	kill(getpid(), SIGSEGV);
#endif
	exit(1);
}

/*******************/
/* Storage Manager */
/*******************/

#ifdef ZEND_WIN32
#  define HAVE_MEM_WIN32    /* use VirtualAlloc() to allocate memory     */
#endif
#define HAVE_MEM_MALLOC     /* use malloc() to allocate segments         */

#include <sys/types.h>
#include <sys/stat.h>
#if HAVE_LIMITS_H
#include <limits.h>
#endif
#include <fcntl.h>
#include <errno.h>

#if defined(HAVE_MEM_MMAP_ANON) || defined(HAVE_MEM_MMAP_ZERO)
# ifdef HAVE_MREMAP
#  ifndef _GNU_SOURCE
#   define _GNU_SOURCE
#  endif
#  ifndef __USE_GNU
#   define __USE_GNU
#  endif
# endif
# include <sys/mman.h>
# ifndef MAP_ANON
#  ifdef MAP_ANONYMOUS
#   define MAP_ANON MAP_ANONYMOUS
#  endif
# endif
# ifndef MREMAP_MAYMOVE
#  define MREMAP_MAYMOVE 0
# endif
# ifndef MAP_FAILED
#  define MAP_FAILED ((void*)-1)
# endif
#endif

static zend_mm_storage* zend_mm_mem_dummy_init(void *params)
{
	return malloc(sizeof(zend_mm_storage));
}

static void zend_mm_mem_dummy_dtor(zend_mm_storage *storage)
{
	free(storage);
}

static void zend_mm_mem_dummy_compact(zend_mm_storage *storage)
{
}

#if defined(HAVE_MEM_MMAP_ANON) || defined(HAVE_MEM_MMAP_ZERO)

static zend_mm_segment* zend_mm_mem_mmap_realloc(zend_mm_storage *storage, zend_mm_segment* segment, size_t size)
{
	zend_mm_segment *ret;
#ifdef HAVE_MREMAP
#if defined(__NetBSD__)
	/* NetBSD 5 supports mremap but takes an extra newp argument */
	ret = (zend_mm_segment*)mremap(segment, segment->size, segment, size, MREMAP_MAYMOVE);
#else
	ret = (zend_mm_segment*)mremap(segment, segment->size, size, MREMAP_MAYMOVE);
#endif
	if (ret == MAP_FAILED) {
#endif
		ret = storage->handlers->_alloc(storage, size);
		if (ret) {
			memcpy(ret, segment, size > segment->size ? segment->size : size);
			storage->handlers->_free(storage, segment);
		}
#ifdef HAVE_MREMAP
	}
#endif
	return ret;
}

static void zend_mm_mem_mmap_free(zend_mm_storage *storage, zend_mm_segment* segment)
{
	munmap((void*)segment, segment->size);
}

#endif

#ifdef HAVE_MEM_MMAP_ANON

static zend_mm_segment* zend_mm_mem_mmap_anon_alloc(zend_mm_storage *storage, size_t size)
{
	zend_mm_segment *ret = (zend_mm_segment*)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
	if (ret == MAP_FAILED) {
		ret = NULL;
	}
	return ret;
}

# define ZEND_MM_MEM_MMAP_ANON_DSC {"mmap_anon", zend_mm_mem_dummy_init, zend_mm_mem_dummy_dtor, zend_mm_mem_dummy_compact, zend_mm_mem_mmap_anon_alloc, zend_mm_mem_mmap_realloc, zend_mm_mem_mmap_free}

#endif

#ifdef HAVE_MEM_MMAP_ZERO

static int zend_mm_dev_zero_fd = -1;

static zend_mm_storage* zend_mm_mem_mmap_zero_init(void *params)
{
	if (zend_mm_dev_zero_fd == -1) {
		zend_mm_dev_zero_fd = open("/dev/zero", O_RDWR, S_IRUSR | S_IWUSR);
	}
	if (zend_mm_dev_zero_fd >= 0) {
		return malloc(sizeof(zend_mm_storage));
	} else {
		return NULL;
	}
}

static void zend_mm_mem_mmap_zero_dtor(zend_mm_storage *storage)
{
	close(zend_mm_dev_zero_fd);
	free(storage);
}

static zend_mm_segment* zend_mm_mem_mmap_zero_alloc(zend_mm_storage *storage, size_t size)
{
	zend_mm_segment *ret = (zend_mm_segment*)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE, zend_mm_dev_zero_fd, 0);
	if (ret == MAP_FAILED) {
		ret = NULL;
	}
	return ret;
}

# define ZEND_MM_MEM_MMAP_ZERO_DSC {"mmap_zero", zend_mm_mem_mmap_zero_init, zend_mm_mem_mmap_zero_dtor, zend_mm_mem_dummy_compact, zend_mm_mem_mmap_zero_alloc, zend_mm_mem_mmap_realloc, zend_mm_mem_mmap_free}

#endif

#ifdef HAVE_MEM_WIN32

static zend_mm_storage* zend_mm_mem_win32_init(void *params)
{
	HANDLE heap = HeapCreate(HEAP_NO_SERIALIZE, 0, 0);
	zend_mm_storage* storage;

	if (heap == NULL) {
		return NULL;
	}
	storage = (zend_mm_storage*)malloc(sizeof(zend_mm_storage));
	if (storage == NULL) {
		HeapDestroy(heap);
		return NULL;
	}
	storage->data = (void*) heap;
	return storage;
}

static void zend_mm_mem_win32_dtor(zend_mm_storage *storage)
{
	HeapDestroy((HANDLE)storage->data);
	free(storage);
}

static void zend_mm_mem_win32_compact(zend_mm_storage *storage)
{
    HeapDestroy((HANDLE)storage->data);
    storage->data = (void*)HeapCreate(HEAP_NO_SERIALIZE, 0, 0);
}

static zend_mm_segment* zend_mm_mem_win32_alloc(zend_mm_storage *storage, size_t size)
{
	return (zend_mm_segment*) HeapAlloc((HANDLE)storage->data, HEAP_NO_SERIALIZE, size);
}

static void zend_mm_mem_win32_free(zend_mm_storage *storage, zend_mm_segment* segment)
{
	HeapFree((HANDLE)storage->data, HEAP_NO_SERIALIZE, segment);
}

static zend_mm_segment* zend_mm_mem_win32_realloc(zend_mm_storage *storage, zend_mm_segment* segment, size_t size)
{
	return (zend_mm_segment*) HeapReAlloc((HANDLE)storage->data, HEAP_NO_SERIALIZE, segment, size);
}

# define ZEND_MM_MEM_WIN32_DSC {"win32", zend_mm_mem_win32_init, zend_mm_mem_win32_dtor, zend_mm_mem_win32_compact, zend_mm_mem_win32_alloc, zend_mm_mem_win32_realloc, zend_mm_mem_win32_free}

#endif

#ifdef HAVE_MEM_MALLOC

static zend_mm_segment* zend_mm_mem_malloc_alloc(zend_mm_storage *storage, size_t size)
{
	return (zend_mm_segment*)malloc(size);
}

static zend_mm_segment* zend_mm_mem_malloc_realloc(zend_mm_storage *storage, zend_mm_segment *ptr, size_t size)
{
	return (zend_mm_segment*)realloc(ptr, size);
}

static void zend_mm_mem_malloc_free(zend_mm_storage *storage, zend_mm_segment *ptr)
{
	free(ptr);
}

# define ZEND_MM_MEM_MALLOC_DSC {"malloc", zend_mm_mem_dummy_init, zend_mm_mem_dummy_dtor, zend_mm_mem_dummy_compact, zend_mm_mem_malloc_alloc, zend_mm_mem_malloc_realloc, zend_mm_mem_malloc_free}

#endif

static const zend_mm_mem_handlers mem_handlers[] = {
#ifdef HAVE_MEM_WIN32
	ZEND_MM_MEM_WIN32_DSC,
#endif
#ifdef HAVE_MEM_MALLOC
	ZEND_MM_MEM_MALLOC_DSC,
#endif
#ifdef HAVE_MEM_MMAP_ANON
	ZEND_MM_MEM_MMAP_ANON_DSC,
#endif
#ifdef HAVE_MEM_MMAP_ZERO
	ZEND_MM_MEM_MMAP_ZERO_DSC,
#endif
	{NULL, NULL, NULL, NULL, NULL, NULL}
};

# define ZEND_MM_STORAGE_DTOR()						heap->storage->handlers->dtor(heap->storage)
# define ZEND_MM_STORAGE_ALLOC(size)				heap->storage->handlers->_alloc(heap->storage, size)
# define ZEND_MM_STORAGE_REALLOC(ptr, size)			heap->storage->handlers->_realloc(heap->storage, ptr, size)
# define ZEND_MM_STORAGE_FREE(ptr)					heap->storage->handlers->_free(heap->storage, ptr)

/****************/
/* Heap Manager */
/****************/

#define MEM_BLOCK_VALID  0x7312F8DC
#define	MEM_BLOCK_FREED  0x99954317
#define	MEM_BLOCK_CACHED 0xFB8277DC
#define	MEM_BLOCK_GUARD  0x2A8FCC84
#define	MEM_BLOCK_LEAK   0x6C5E8F2D

/* mm block type */
typedef struct _zend_mm_block_info {
#if ZEND_MM_COOKIES
	size_t _cookie;
#endif
	size_t _size;
	size_t _prev;
} zend_mm_block_info;

#if ZEND_DEBUG

typedef struct _zend_mm_debug_info {
	const char *filename;
	uint lineno;
	const char *orig_filename;
	uint orig_lineno;
	size_t size;
#if ZEND_MM_HEAP_PROTECTION
	unsigned int start_magic;
#endif
} zend_mm_debug_info;

#elif ZEND_MM_HEAP_PROTECTION

typedef struct _zend_mm_debug_info {
	size_t size;
	unsigned int start_magic;
} zend_mm_debug_info;

#endif

typedef struct _zend_mm_block {
	zend_mm_block_info info;
#if ZEND_DEBUG
	unsigned int magic;
# ifdef ZTS
	THREAD_T thread_id;
# endif
	zend_mm_debug_info debug;
#elif ZEND_MM_HEAP_PROTECTION
	zend_mm_debug_info debug;
#endif
} zend_mm_block;

typedef struct _zend_mm_small_free_block {
	zend_mm_block_info info;
#if ZEND_DEBUG
	unsigned int magic;
# ifdef ZTS
	THREAD_T thread_id;
# endif
#endif
	struct _zend_mm_free_block *prev_free_block;
	struct _zend_mm_free_block *next_free_block;
} zend_mm_small_free_block;

typedef struct _zend_mm_free_block {
	zend_mm_block_info info;
#if ZEND_DEBUG
	unsigned int magic;
# ifdef ZTS
	THREAD_T thread_id;
# endif
#endif
	struct _zend_mm_free_block *prev_free_block;
	struct _zend_mm_free_block *next_free_block;

	struct _zend_mm_free_block **parent;
	struct _zend_mm_free_block *child[2];
} zend_mm_free_block;

#define ZEND_MM_NUM_BUCKETS (sizeof(size_t) << 3)

#define ZEND_MM_CACHE 1
#define ZEND_MM_CACHE_SIZE (ZEND_MM_NUM_BUCKETS * 4 * 1024)

#ifndef ZEND_MM_CACHE_STAT
# define ZEND_MM_CACHE_STAT 0
#endif

struct _zend_mm_heap {
	int                 use_zend_alloc;
	void               *(*_malloc)(size_t);
	void                (*_free)(void*);
	void               *(*_realloc)(void*, size_t);
	size_t              free_bitmap;
	size_t              large_free_bitmap;
	size_t              block_size;
	size_t              compact_size;
	zend_mm_segment    *segments_list;
	zend_mm_storage    *storage;
	size_t              real_size;
	size_t              real_peak;
	size_t              limit;
	size_t              size;
	size_t              peak;
	size_t              reserve_size;
	void               *reserve;
	int                 overflow;
	int                 internal;
#if ZEND_MM_CACHE
	unsigned int        cached;
	zend_mm_free_block *cache[ZEND_MM_NUM_BUCKETS];
#endif
	zend_mm_free_block *free_buckets[ZEND_MM_NUM_BUCKETS*2];
	zend_mm_free_block *large_free_buckets[ZEND_MM_NUM_BUCKETS];
	zend_mm_free_block *rest_buckets[2];
	int                 rest_count;
#if ZEND_MM_CACHE_STAT
	struct {
		int count;
		int max_count;
		int hit;
		int miss;
	} cache_stat[ZEND_MM_NUM_BUCKETS+1];
#endif
};

#define ZEND_MM_SMALL_FREE_BUCKET(heap, index) \
	(zend_mm_free_block*) ((char*)&heap->free_buckets[index * 2] + \
		sizeof(zend_mm_free_block*) * 2 - \
		sizeof(zend_mm_small_free_block))

#define ZEND_MM_REST_BUCKET(heap) \
	(zend_mm_free_block*)((char*)&heap->rest_buckets[0] + \
		sizeof(zend_mm_free_block*) * 2 - \
		sizeof(zend_mm_small_free_block))

#define ZEND_MM_REST_BLOCK ((zend_mm_free_block**)(zend_uintptr_t)(1))

#define ZEND_MM_MAX_REST_BLOCKS 16

#if ZEND_MM_COOKIES

static unsigned int _zend_mm_cookie = 0;

# define ZEND_MM_COOKIE(block) \
	(((size_t)(block)) ^ _zend_mm_cookie)
# define ZEND_MM_SET_COOKIE(block) \
	(block)->info._cookie = ZEND_MM_COOKIE(block)
# define ZEND_MM_CHECK_COOKIE(block) \
	if (UNEXPECTED((block)->info._cookie != ZEND_MM_COOKIE(block))) { \
		zend_mm_panic("zend_mm_heap corrupted"); \
	}
#else
# define ZEND_MM_SET_COOKIE(block)
# define ZEND_MM_CHECK_COOKIE(block)
#endif

/* Default memory segment size */
#define ZEND_MM_SEG_SIZE   (256 * 1024)

/* Reserved space for error reporting in case of memory overflow */
#define ZEND_MM_RESERVE_SIZE            (8*1024)

#ifdef _WIN64
# define ZEND_MM_LONG_CONST(x)	(x##i64)
#else
# define ZEND_MM_LONG_CONST(x)	(x##L)
#endif

#define ZEND_MM_TYPE_MASK		ZEND_MM_LONG_CONST(0x3)

#define ZEND_MM_FREE_BLOCK		ZEND_MM_LONG_CONST(0x0)
#define ZEND_MM_USED_BLOCK		ZEND_MM_LONG_CONST(0x1)
#define ZEND_MM_GUARD_BLOCK		ZEND_MM_LONG_CONST(0x3)

#define ZEND_MM_BLOCK(b, type, size)	do { \
											size_t _size = (size); \
											(b)->info._size = (type) | _size; \
											ZEND_MM_BLOCK_AT(b, _size)->info._prev = (type) | _size; \
											ZEND_MM_SET_COOKIE(b); \
										} while (0);
#define ZEND_MM_LAST_BLOCK(b)			do { \
		(b)->info._size = ZEND_MM_GUARD_BLOCK | ZEND_MM_ALIGNED_HEADER_SIZE; \
		ZEND_MM_SET_MAGIC(b, MEM_BLOCK_GUARD); \
 	} while (0);
#define ZEND_MM_BLOCK_SIZE(b)			((b)->info._size & ~ZEND_MM_TYPE_MASK)
#define ZEND_MM_IS_FREE_BLOCK(b)		(!((b)->info._size & ZEND_MM_USED_BLOCK))
#define ZEND_MM_IS_USED_BLOCK(b)		((b)->info._size & ZEND_MM_USED_BLOCK)
#define ZEND_MM_IS_GUARD_BLOCK(b)		(((b)->info._size & ZEND_MM_TYPE_MASK) == ZEND_MM_GUARD_BLOCK)

#define ZEND_MM_NEXT_BLOCK(b)			ZEND_MM_BLOCK_AT(b, ZEND_MM_BLOCK_SIZE(b))
#define ZEND_MM_PREV_BLOCK(b)			ZEND_MM_BLOCK_AT(b, -(ssize_t)((b)->info._prev & ~ZEND_MM_TYPE_MASK))

#define ZEND_MM_PREV_BLOCK_IS_FREE(b)	(!((b)->info._prev & ZEND_MM_USED_BLOCK))

#define ZEND_MM_MARK_FIRST_BLOCK(b)		((b)->info._prev = ZEND_MM_GUARD_BLOCK)
#define ZEND_MM_IS_FIRST_BLOCK(b)		((b)->info._prev == ZEND_MM_GUARD_BLOCK)

/* optimized access */
#define ZEND_MM_FREE_BLOCK_SIZE(b)		(b)->info._size

/* Aligned header size */
#define ZEND_MM_ALIGNED_HEADER_SIZE			ZEND_MM_ALIGNED_SIZE(sizeof(zend_mm_block))
#define ZEND_MM_ALIGNED_FREE_HEADER_SIZE	ZEND_MM_ALIGNED_SIZE(sizeof(zend_mm_small_free_block))
#define ZEND_MM_MIN_ALLOC_BLOCK_SIZE		ZEND_MM_ALIGNED_SIZE(ZEND_MM_ALIGNED_HEADER_SIZE + END_MAGIC_SIZE)
#define ZEND_MM_ALIGNED_MIN_HEADER_SIZE		(ZEND_MM_MIN_ALLOC_BLOCK_SIZE>ZEND_MM_ALIGNED_FREE_HEADER_SIZE?ZEND_MM_MIN_ALLOC_BLOCK_SIZE:ZEND_MM_ALIGNED_FREE_HEADER_SIZE)
#define ZEND_MM_ALIGNED_SEGMENT_SIZE		ZEND_MM_ALIGNED_SIZE(sizeof(zend_mm_segment))

#define ZEND_MM_MIN_SIZE					((ZEND_MM_ALIGNED_MIN_HEADER_SIZE>(ZEND_MM_ALIGNED_HEADER_SIZE+END_MAGIC_SIZE))?(ZEND_MM_ALIGNED_MIN_HEADER_SIZE-(ZEND_MM_ALIGNED_HEADER_SIZE+END_MAGIC_SIZE)):0)

#define ZEND_MM_MAX_SMALL_SIZE				((ZEND_MM_NUM_BUCKETS<<ZEND_MM_ALIGNMENT_LOG2)+ZEND_MM_ALIGNED_MIN_HEADER_SIZE)

#define ZEND_MM_TRUE_SIZE(size)				((size<ZEND_MM_MIN_SIZE)?(ZEND_MM_ALIGNED_MIN_HEADER_SIZE):(ZEND_MM_ALIGNED_SIZE(size+ZEND_MM_ALIGNED_HEADER_SIZE+END_MAGIC_SIZE)))

#define ZEND_MM_BUCKET_INDEX(true_size)		((true_size>>ZEND_MM_ALIGNMENT_LOG2)-(ZEND_MM_ALIGNED_MIN_HEADER_SIZE>>ZEND_MM_ALIGNMENT_LOG2))

#define ZEND_MM_SMALL_SIZE(true_size)		(true_size < ZEND_MM_MAX_SMALL_SIZE)

/* Memory calculations */
#define ZEND_MM_BLOCK_AT(blk, offset)	((zend_mm_block *) (((char *) (blk))+(offset)))
#define ZEND_MM_DATA_OF(p)				((void *) (((char *) (p))+ZEND_MM_ALIGNED_HEADER_SIZE))
#define ZEND_MM_HEADER_OF(blk)			ZEND_MM_BLOCK_AT(blk, -(int)ZEND_MM_ALIGNED_HEADER_SIZE)

/* Debug output */
#if ZEND_DEBUG

# ifdef ZTS
#  define ZEND_MM_SET_THREAD_ID(block) \
	((zend_mm_block*)(block))->thread_id = tsrm_thread_id()
#  define ZEND_MM_BAD_THREAD_ID(block) ((block)->thread_id != tsrm_thread_id())
# else
#  define ZEND_MM_SET_THREAD_ID(block)
#  define ZEND_MM_BAD_THREAD_ID(block) 0
# endif

# define ZEND_MM_VALID_PTR(block) \
	zend_mm_check_ptr(heap, block, 1 ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC)

# define ZEND_MM_SET_MAGIC(block, val) do { \
		(block)->magic = (val); \
	} while (0)

# define ZEND_MM_CHECK_MAGIC(block, val) do { \
		if ((block)->magic != (val)) { \
			zend_mm_panic("zend_mm_heap corrupted"); \
		} \
	} while (0)

# define ZEND_MM_SET_DEBUG_INFO(block, __size, set_valid, set_thread) do { \
		((zend_mm_block*)(block))->debug.filename = __zend_filename; \
		((zend_mm_block*)(block))->debug.lineno = __zend_lineno; \
		((zend_mm_block*)(block))->debug.orig_filename = __zend_orig_filename; \
		((zend_mm_block*)(block))->debug.orig_lineno = __zend_orig_lineno; \
		ZEND_MM_SET_BLOCK_SIZE(block, __size); \
		if (set_valid) { \
			ZEND_MM_SET_MAGIC(block, MEM_BLOCK_VALID); \
		} \
		if (set_thread) { \
			ZEND_MM_SET_THREAD_ID(block); \
		} \
	} while (0)

#else

# define ZEND_MM_VALID_PTR(ptr) EXPECTED(ptr != NULL)

# define ZEND_MM_SET_MAGIC(block, val)

# define ZEND_MM_CHECK_MAGIC(block, val)

# define ZEND_MM_SET_DEBUG_INFO(block, __size, set_valid, set_thread) ZEND_MM_SET_BLOCK_SIZE(block, __size)

#endif


#if ZEND_MM_HEAP_PROTECTION

# define ZEND_MM_CHECK_PROTECTION(block) \
	do { \
		if ((block)->debug.start_magic != _mem_block_start_magic || \
		    memcmp(ZEND_MM_END_MAGIC_PTR(block), &_mem_block_end_magic, END_MAGIC_SIZE) != 0) { \
		    zend_mm_panic("zend_mm_heap corrupted"); \
		} \
	} while (0)

# define ZEND_MM_END_MAGIC_PTR(block) \
	(((char*)(ZEND_MM_DATA_OF(block))) + ((zend_mm_block*)(block))->debug.size)

# define END_MAGIC_SIZE sizeof(unsigned int)

# define ZEND_MM_SET_BLOCK_SIZE(block, __size) do { \
		char *p; \
		((zend_mm_block*)(block))->debug.size = (__size); \
		p = ZEND_MM_END_MAGIC_PTR(block); \
		((zend_mm_block*)(block))->debug.start_magic = _mem_block_start_magic; \
		memcpy(p, &_mem_block_end_magic, END_MAGIC_SIZE); \
	} while (0)

static unsigned int _mem_block_start_magic = 0;
static unsigned int _mem_block_end_magic   = 0;

#else

# if ZEND_DEBUG
#  define ZEND_MM_SET_BLOCK_SIZE(block, _size) \
	((zend_mm_block*)(block))->debug.size = (_size)
# else
#  define ZEND_MM_SET_BLOCK_SIZE(block, _size)
# endif

# define ZEND_MM_CHECK_PROTECTION(block)

# define END_MAGIC_SIZE 0

#endif

#if ZEND_MM_SAFE_UNLINKING
# define ZEND_MM_CHECK_BLOCK_LINKAGE(block) \
	if (UNEXPECTED((block)->info._size != ZEND_MM_BLOCK_AT(block, ZEND_MM_FREE_BLOCK_SIZE(block))->info._prev) || \
		UNEXPECTED(!UNEXPECTED(ZEND_MM_IS_FIRST_BLOCK(block)) && \
	    UNEXPECTED(ZEND_MM_PREV_BLOCK(block)->info._size != (block)->info._prev))) { \
	    zend_mm_panic("zend_mm_heap corrupted"); \
	}
#define ZEND_MM_CHECK_TREE(block) \
	if (UNEXPECTED(*((block)->parent) != (block))) { \
		zend_mm_panic("zend_mm_heap corrupted"); \
	}
#else
# define ZEND_MM_CHECK_BLOCK_LINKAGE(block)
# define ZEND_MM_CHECK_TREE(block)
#endif

#define ZEND_MM_LARGE_BUCKET_INDEX(S) zend_mm_high_bit(S)

static void *_zend_mm_alloc_int(zend_mm_heap *heap, size_t size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC) ZEND_ATTRIBUTE_MALLOC ZEND_ATTRIBUTE_ALLOC_SIZE(2);
static void _zend_mm_free_int(zend_mm_heap *heap, void *p ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC);
static void *_zend_mm_realloc_int(zend_mm_heap *heap, void *p, size_t size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC) ZEND_ATTRIBUTE_ALLOC_SIZE(3);

static inline unsigned int zend_mm_high_bit(size_t _size)
{
#if defined(__GNUC__) && (defined(__native_client__) || defined(i386))
	unsigned int n;

	__asm__("bsrl %1,%0\n\t" : "=r" (n) : "rm"  (_size) : "cc");
	return n;
#elif defined(__GNUC__) && defined(__x86_64__)
	unsigned long n;

        __asm__("bsr %1,%0\n\t" : "=r" (n) : "rm"  (_size) : "cc");
        return (unsigned int)n;
#elif defined(_MSC_VER) && defined(_M_IX86)
	__asm {
		bsr eax, _size
	}
#elif defined(__GNUC__) && (defined(__arm__) || defined(__aarch64__) || defined(__powerpc__))
	return (8 * SIZEOF_SIZE_T - 1) - __builtin_clzl(_size);
#else
	unsigned int n = 0;
	while (_size != 0) {
		_size = _size >> 1;
		n++;
	}
	return n-1;
#endif
}

static inline unsigned int zend_mm_low_bit(size_t _size)
{
#if defined(__GNUC__) && (defined(__native_client__) || defined(i386))
	unsigned int n;

	__asm__("bsfl %1,%0\n\t" : "=r" (n) : "rm"  (_size) : "cc");
	return n;
#elif defined(__GNUC__) && defined(__x86_64__)
        unsigned long n;

        __asm__("bsf %1,%0\n\t" : "=r" (n) : "rm"  (_size) : "cc");
        return (unsigned int)n;
#elif defined(_MSC_VER) && defined(_M_IX86)
	__asm {
		bsf eax, _size
   }
#elif defined(__GNUC__) && (defined(__arm__) || defined(__aarch64__) || defined(__powerpc__))
	return __builtin_ctzl(_size);
#else
	static const int offset[16] = {4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0};
	unsigned int n;
	unsigned int index = 0;

	n = offset[_size & 15];
	while (n == 4) {
		_size >>= 4;
		index += n;
		n = offset[_size & 15];
	}

	return index + n;
#endif
}

static inline void zend_mm_add_to_free_list(zend_mm_heap *heap, zend_mm_free_block *mm_block)
{
	size_t size;
	size_t index;

	ZEND_MM_SET_MAGIC(mm_block, MEM_BLOCK_FREED);

	size = ZEND_MM_FREE_BLOCK_SIZE(mm_block);
	if (EXPECTED(!ZEND_MM_SMALL_SIZE(size))) {
		zend_mm_free_block **p;

		index = ZEND_MM_LARGE_BUCKET_INDEX(size);
		p = &heap->large_free_buckets[index];
		mm_block->child[0] = mm_block->child[1] = NULL;
		if (!*p) {
			*p = mm_block;
			mm_block->parent = p;
			mm_block->prev_free_block = mm_block->next_free_block = mm_block;
			heap->large_free_bitmap |= (ZEND_MM_LONG_CONST(1) << index);
		} else {
			size_t m;

			for (m = size << (ZEND_MM_NUM_BUCKETS - index); ; m <<= 1) {
				zend_mm_free_block *prev = *p;

				if (ZEND_MM_FREE_BLOCK_SIZE(prev) != size) {
					p = &prev->child[(m >> (ZEND_MM_NUM_BUCKETS-1)) & 1];
					if (!*p) {
						*p = mm_block;
						mm_block->parent = p;
						mm_block->prev_free_block = mm_block->next_free_block = mm_block;
						break;
					}
				} else {
					zend_mm_free_block *next = prev->next_free_block;

					prev->next_free_block = next->prev_free_block = mm_block;
					mm_block->next_free_block = next;
					mm_block->prev_free_block = prev;
					mm_block->parent = NULL;
					break;
				}
			}
		}
	} else {
		zend_mm_free_block *prev, *next;

		index = ZEND_MM_BUCKET_INDEX(size);

		prev = ZEND_MM_SMALL_FREE_BUCKET(heap, index);
		if (prev->prev_free_block == prev) {
			heap->free_bitmap |= (ZEND_MM_LONG_CONST(1) << index);
		}
		next = prev->next_free_block;

		mm_block->prev_free_block = prev;
		mm_block->next_free_block = next;
		prev->next_free_block = next->prev_free_block = mm_block;
	}
}

static inline void zend_mm_remove_from_free_list(zend_mm_heap *heap, zend_mm_free_block *mm_block)
{
	zend_mm_free_block *prev = mm_block->prev_free_block;
	zend_mm_free_block *next = mm_block->next_free_block;

	ZEND_MM_CHECK_MAGIC(mm_block, MEM_BLOCK_FREED);

	if (EXPECTED(prev == mm_block)) {
		zend_mm_free_block **rp, **cp;

#if ZEND_MM_SAFE_UNLINKING
		if (UNEXPECTED(next != mm_block)) {
			zend_mm_panic("zend_mm_heap corrupted");
		}
#endif

		rp = &mm_block->child[mm_block->child[1] != NULL];
		prev = *rp;
		if (EXPECTED(prev == NULL)) {
			size_t index = ZEND_MM_LARGE_BUCKET_INDEX(ZEND_MM_FREE_BLOCK_SIZE(mm_block));

			ZEND_MM_CHECK_TREE(mm_block);
			*mm_block->parent = NULL;
			if (mm_block->parent == &heap->large_free_buckets[index]) {
				heap->large_free_bitmap &= ~(ZEND_MM_LONG_CONST(1) << index);
		    }
		} else {
			while (*(cp = &(prev->child[prev->child[1] != NULL])) != NULL) {
				prev = *cp;
				rp = cp;
			}
			*rp = NULL;

subst_block:
			ZEND_MM_CHECK_TREE(mm_block);
			*mm_block->parent = prev;
			prev->parent = mm_block->parent;
			if ((prev->child[0] = mm_block->child[0])) {
				ZEND_MM_CHECK_TREE(prev->child[0]);
				prev->child[0]->parent = &prev->child[0];
			}
			if ((prev->child[1] = mm_block->child[1])) {
				ZEND_MM_CHECK_TREE(prev->child[1]);
				prev->child[1]->parent = &prev->child[1];
			}
		}
	} else {

#if ZEND_MM_SAFE_UNLINKING
		if (UNEXPECTED(prev->next_free_block != mm_block) || UNEXPECTED(next->prev_free_block != mm_block)) {
			zend_mm_panic("zend_mm_heap corrupted");
		}
#endif

		prev->next_free_block = next;
		next->prev_free_block = prev;

		if (EXPECTED(ZEND_MM_SMALL_SIZE(ZEND_MM_FREE_BLOCK_SIZE(mm_block)))) {
			if (EXPECTED(prev == next)) {
				size_t index = ZEND_MM_BUCKET_INDEX(ZEND_MM_FREE_BLOCK_SIZE(mm_block));

				if (EXPECTED(heap->free_buckets[index*2] == heap->free_buckets[index*2+1])) {
					heap->free_bitmap &= ~(ZEND_MM_LONG_CONST(1) << index);
				}
			}
		} else if (UNEXPECTED(mm_block->parent == ZEND_MM_REST_BLOCK)) {
			heap->rest_count--;
		} else if (UNEXPECTED(mm_block->parent != NULL)) {
			goto subst_block;
		}
	}
}

static inline void zend_mm_add_to_rest_list(zend_mm_heap *heap, zend_mm_free_block *mm_block)
{
	zend_mm_free_block *prev, *next;

	while (heap->rest_count >= ZEND_MM_MAX_REST_BLOCKS) {
		zend_mm_free_block *p = heap->rest_buckets[1];

		if (!ZEND_MM_SMALL_SIZE(ZEND_MM_FREE_BLOCK_SIZE(p))) {
			heap->rest_count--;
		}
		prev = p->prev_free_block;
		next = p->next_free_block;
		prev->next_free_block = next;
		next->prev_free_block = prev;
		zend_mm_add_to_free_list(heap, p);
	}

	if (!ZEND_MM_SMALL_SIZE(ZEND_MM_FREE_BLOCK_SIZE(mm_block))) {
		mm_block->parent = ZEND_MM_REST_BLOCK;
		heap->rest_count++;
	}

	ZEND_MM_SET_MAGIC(mm_block, MEM_BLOCK_FREED);

	prev = heap->rest_buckets[0];
	next = prev->next_free_block;
	mm_block->prev_free_block = prev;
	mm_block->next_free_block = next;
	prev->next_free_block = next->prev_free_block = mm_block;
}

static inline void zend_mm_init(zend_mm_heap *heap)
{
	zend_mm_free_block* p;
	int i;

	heap->free_bitmap = 0;
	heap->large_free_bitmap = 0;
#if ZEND_MM_CACHE
	heap->cached = 0;
	memset(heap->cache, 0, sizeof(heap->cache));
#endif
#if ZEND_MM_CACHE_STAT
	for (i = 0; i < ZEND_MM_NUM_BUCKETS; i++) {
		heap->cache_stat[i].count = 0;
	}
#endif
	p = ZEND_MM_SMALL_FREE_BUCKET(heap, 0);
	for (i = 0; i < ZEND_MM_NUM_BUCKETS; i++) {
		p->next_free_block = p;
		p->prev_free_block = p;
		p = (zend_mm_free_block*)((char*)p + sizeof(zend_mm_free_block*) * 2);
		heap->large_free_buckets[i] = NULL;
	}
	heap->rest_buckets[0] = heap->rest_buckets[1] = ZEND_MM_REST_BUCKET(heap);
	heap->rest_count = 0;
}

static void zend_mm_del_segment(zend_mm_heap *heap, zend_mm_segment *segment)
{
	zend_mm_segment **p = &heap->segments_list;

	while (*p != segment) {
		p = &(*p)->next_segment;
	}
	*p = segment->next_segment;
	heap->real_size -= segment->size;
	ZEND_MM_STORAGE_FREE(segment);
}

#if ZEND_MM_CACHE
static void zend_mm_free_cache(zend_mm_heap *heap)
{
	int i;

	for (i = 0; i < ZEND_MM_NUM_BUCKETS; i++) {
		if (heap->cache[i]) {
			zend_mm_free_block *mm_block = heap->cache[i];

			while (mm_block) {
				size_t size = ZEND_MM_BLOCK_SIZE(mm_block);
				zend_mm_free_block *q = mm_block->prev_free_block;
				zend_mm_block *next_block = ZEND_MM_NEXT_BLOCK(mm_block);

				heap->cached -= size;

				if (ZEND_MM_PREV_BLOCK_IS_FREE(mm_block)) {
					mm_block = (zend_mm_free_block*)ZEND_MM_PREV_BLOCK(mm_block);
					size += ZEND_MM_FREE_BLOCK_SIZE(mm_block);
					zend_mm_remove_from_free_list(heap, (zend_mm_free_block *) mm_block);
				}
				if (ZEND_MM_IS_FREE_BLOCK(next_block)) {
					size += ZEND_MM_FREE_BLOCK_SIZE(next_block);
					zend_mm_remove_from_free_list(heap, (zend_mm_free_block *) next_block);
				}
				ZEND_MM_BLOCK(mm_block, ZEND_MM_FREE_BLOCK, size);

				if (ZEND_MM_IS_FIRST_BLOCK(mm_block) &&
				    ZEND_MM_IS_GUARD_BLOCK(ZEND_MM_NEXT_BLOCK(mm_block))) {
					zend_mm_del_segment(heap, (zend_mm_segment *) ((char *)mm_block - ZEND_MM_ALIGNED_SEGMENT_SIZE));
				} else {
					zend_mm_add_to_free_list(heap, (zend_mm_free_block *) mm_block);
				}

				mm_block = q;
			}
			heap->cache[i] = NULL;
#if ZEND_MM_CACHE_STAT
			heap->cache_stat[i].count = 0;
#endif
		}
	}
}
#endif

#if ZEND_MM_HEAP_PROTECTION || ZEND_MM_COOKIES
static void zend_mm_random(unsigned char *buf, size_t size) /* {{{ */
{
	size_t i = 0;
	unsigned char t;

#ifdef ZEND_WIN32
	HCRYPTPROV   hCryptProv;
	int has_context = 0;

	if (!CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, 0)) {
		/* Could mean that the key container does not exist, let try
		   again by asking for a new one */
		if (GetLastError() == NTE_BAD_KEYSET) {
			if (CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET)) {
				has_context = 1;
			}
		}
	} else {
		has_context = 1;
	}
	if (has_context) {
		do {
			BOOL ret = CryptGenRandom(hCryptProv, size, buf);
			CryptReleaseContext(hCryptProv, 0);
			if (ret) {
				while (i < size && buf[i] != 0) {
					i++;
				}
				if (i == size) {
					return;
				}
		   }
		} while (0);
	}
#elif defined(HAVE_DEV_URANDOM)
	int fd = open("/dev/urandom", 0);

	if (fd >= 0) {
		if (read(fd, buf, size) == size) {
			while (i < size && buf[i] != 0) {
				i++;
			}
			if (i == size) {
				close(fd);
			    return;
			}
		}
		close(fd);
	}
#endif
	t = (unsigned char)getpid();
	while (i < size) {
		do {
			buf[i] = ((unsigned char)rand()) ^ t;
		} while (buf[i] == 0);
		t = buf[i++] << 1;
    }
}
/* }}} */
#endif

/* Notes:
 * - This function may alter the block_sizes values to match platform alignment
 * - This function does *not* perform sanity checks on the arguments
 */
ZEND_API zend_mm_heap *zend_mm_startup_ex(const zend_mm_mem_handlers *handlers, size_t block_size, size_t reserve_size, int internal, void *params)
{
	zend_mm_storage *storage;
	zend_mm_heap    *heap;

#if 0
	int i;

	printf("ZEND_MM_ALIGNMENT=%d\n", ZEND_MM_ALIGNMENT);
	printf("ZEND_MM_ALIGNMENT_LOG2=%d\n", ZEND_MM_ALIGNMENT_LOG2);
	printf("ZEND_MM_MIN_SIZE=%d\n", ZEND_MM_MIN_SIZE);
	printf("ZEND_MM_MAX_SMALL_SIZE=%d\n", ZEND_MM_MAX_SMALL_SIZE);
	printf("ZEND_MM_ALIGNED_HEADER_SIZE=%d\n", ZEND_MM_ALIGNED_HEADER_SIZE);
	printf("ZEND_MM_ALIGNED_FREE_HEADER_SIZE=%d\n", ZEND_MM_ALIGNED_FREE_HEADER_SIZE);
	printf("ZEND_MM_MIN_ALLOC_BLOCK_SIZE=%d\n", ZEND_MM_MIN_ALLOC_BLOCK_SIZE);
	printf("ZEND_MM_ALIGNED_MIN_HEADER_SIZE=%d\n", ZEND_MM_ALIGNED_MIN_HEADER_SIZE);
	printf("ZEND_MM_ALIGNED_SEGMENT_SIZE=%d\n", ZEND_MM_ALIGNED_SEGMENT_SIZE);
	for (i = 0; i < ZEND_MM_MAX_SMALL_SIZE; i++) {
		printf("%3d%c: %3ld %d %2ld\n", i, (i == ZEND_MM_MIN_SIZE?'*':' '), (long)ZEND_MM_TRUE_SIZE(i), ZEND_MM_SMALL_SIZE(ZEND_MM_TRUE_SIZE(i)), (long)ZEND_MM_BUCKET_INDEX(ZEND_MM_TRUE_SIZE(i)));
	}
	exit(0);
#endif

#if ZEND_MM_HEAP_PROTECTION
	if (_mem_block_start_magic == 0) {
		zend_mm_random((unsigned char*)&_mem_block_start_magic, sizeof(_mem_block_start_magic));
	}
	if (_mem_block_end_magic == 0) {
		zend_mm_random((unsigned char*)&_mem_block_end_magic, sizeof(_mem_block_end_magic));
	}
#endif
#if ZEND_MM_COOKIES
	if (_zend_mm_cookie == 0) {
		zend_mm_random((unsigned char*)&_zend_mm_cookie, sizeof(_zend_mm_cookie));
	}
#endif

	if (zend_mm_low_bit(block_size) != zend_mm_high_bit(block_size)) {
		fprintf(stderr, "'block_size' must be a power of two\n");
/* See http://support.microsoft.com/kb/190351 */
#ifdef PHP_WIN32
		fflush(stderr);
#endif
		exit(255);
	}
	storage = handlers->init(params);
	if (!storage) {
		fprintf(stderr, "Cannot initialize zend_mm storage [%s]\n", handlers->name);
/* See http://support.microsoft.com/kb/190351 */
#ifdef PHP_WIN32
		fflush(stderr);
#endif
		exit(255);
	}
	storage->handlers = handlers;

	heap = malloc(sizeof(struct _zend_mm_heap));
	if (heap == NULL) {
		fprintf(stderr, "Cannot allocate heap for zend_mm storage [%s]\n", handlers->name);
#ifdef PHP_WIN32
		fflush(stderr);
#endif
		exit(255);
	}
	heap->storage = storage;
	heap->block_size = block_size;
	heap->compact_size = 0;
	heap->segments_list = NULL;
	zend_mm_init(heap);
# if ZEND_MM_CACHE_STAT
	memset(heap->cache_stat, 0, sizeof(heap->cache_stat));
# endif

	heap->use_zend_alloc = 1;
	heap->real_size = 0;
	heap->overflow = 0;
	heap->real_peak = 0;
	heap->limit = ZEND_MM_LONG_CONST(1)<<(ZEND_MM_NUM_BUCKETS-2);
	heap->size = 0;
	heap->peak = 0;
	heap->internal = internal;
	heap->reserve = NULL;
	heap->reserve_size = reserve_size;
	if (reserve_size > 0) {
		heap->reserve = _zend_mm_alloc_int(heap, reserve_size ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC);
	}
	if (internal) {
		int i;
		zend_mm_free_block *p, *q, *orig;
		zend_mm_heap *mm_heap = _zend_mm_alloc_int(heap, sizeof(zend_mm_heap)  ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC);

		*mm_heap = *heap;

		p = ZEND_MM_SMALL_FREE_BUCKET(mm_heap, 0);
		orig = ZEND_MM_SMALL_FREE_BUCKET(heap, 0);
		for (i = 0; i < ZEND_MM_NUM_BUCKETS; i++) {
			q = p;
			while (q->prev_free_block != orig) {
				q = q->prev_free_block;
			}
			q->prev_free_block = p;
			q = p;
			while (q->next_free_block != orig) {
				q = q->next_free_block;
			}
			q->next_free_block = p;
			p = (zend_mm_free_block*)((char*)p + sizeof(zend_mm_free_block*) * 2);
			orig = (zend_mm_free_block*)((char*)orig + sizeof(zend_mm_free_block*) * 2);
			if (mm_heap->large_free_buckets[i]) {
				mm_heap->large_free_buckets[i]->parent = &mm_heap->large_free_buckets[i];
			}
		}
		mm_heap->rest_buckets[0] = mm_heap->rest_buckets[1] = ZEND_MM_REST_BUCKET(mm_heap);
		mm_heap->rest_count = 0;

		free(heap);
		heap = mm_heap;
	}
	return heap;
}

ZEND_API zend_mm_heap *zend_mm_startup(void)
{
	int i;
	size_t seg_size;
	char *mem_type = getenv("ZEND_MM_MEM_TYPE");
	char *tmp;
	const zend_mm_mem_handlers *handlers;
	zend_mm_heap *heap;

	if (mem_type == NULL) {
		i = 0;
	} else {
		for (i = 0; mem_handlers[i].name; i++) {
			if (strcmp(mem_handlers[i].name, mem_type) == 0) {
				break;
			}
		}
		if (!mem_handlers[i].name) {
			fprintf(stderr, "Wrong or unsupported zend_mm storage type '%s'\n", mem_type);
			fprintf(stderr, "  supported types:\n");
/* See http://support.microsoft.com/kb/190351 */
#ifdef PHP_WIN32
			fflush(stderr);
#endif
			for (i = 0; mem_handlers[i].name; i++) {
				fprintf(stderr, "    '%s'\n", mem_handlers[i].name);
			}
/* See http://support.microsoft.com/kb/190351 */
#ifdef PHP_WIN32
			fflush(stderr);
#endif
			exit(255);
		}
	}
	handlers = &mem_handlers[i];

	tmp = getenv("ZEND_MM_SEG_SIZE");
	if (tmp) {
		seg_size = zend_atoi(tmp, 0);
		if (zend_mm_low_bit(seg_size) != zend_mm_high_bit(seg_size)) {
			fprintf(stderr, "ZEND_MM_SEG_SIZE must be a power of two\n");
/* See http://support.microsoft.com/kb/190351 */
#ifdef PHP_WIN32
			fflush(stderr);
#endif
			exit(255);
		} else if (seg_size < ZEND_MM_ALIGNED_SEGMENT_SIZE + ZEND_MM_ALIGNED_HEADER_SIZE) {
			fprintf(stderr, "ZEND_MM_SEG_SIZE is too small\n");
/* See http://support.microsoft.com/kb/190351 */
#ifdef PHP_WIN32
			fflush(stderr);
#endif
			exit(255);
		}
	} else {
		seg_size = ZEND_MM_SEG_SIZE;
	}

	heap = zend_mm_startup_ex(handlers, seg_size, ZEND_MM_RESERVE_SIZE, 0, NULL);
	if (heap) {
		tmp = getenv("ZEND_MM_COMPACT");
		if (tmp) {
			heap->compact_size = zend_atoi(tmp, 0);
		} else {
			heap->compact_size = 2 * 1024 * 1024;
		}
	}
	return heap;
}

#if ZEND_DEBUG
static long zend_mm_find_leaks(zend_mm_segment *segment, zend_mm_block *b)
{
	long leaks = 0;
	zend_mm_block *p, *q;

	p = ZEND_MM_NEXT_BLOCK(b);
	while (1) {
		if (ZEND_MM_IS_GUARD_BLOCK(p)) {
			ZEND_MM_CHECK_MAGIC(p, MEM_BLOCK_GUARD);
			segment = segment->next_segment;
			if (!segment) {
				break;
			}
			p = (zend_mm_block *) ((char *) segment + ZEND_MM_ALIGNED_SEGMENT_SIZE);
			continue;
		}
		q = ZEND_MM_NEXT_BLOCK(p);
		if (q <= p ||
		    (char*)q > (char*)segment + segment->size ||
		    p->info._size != q->info._prev) {
		    zend_mm_panic("zend_mm_heap corrupted");
		}
		if (!ZEND_MM_IS_FREE_BLOCK(p)) {
			if (p->magic == MEM_BLOCK_VALID) {
				if (p->debug.filename==b->debug.filename && p->debug.lineno==b->debug.lineno) {
					ZEND_MM_SET_MAGIC(p, MEM_BLOCK_LEAK);
					leaks++;
				}
#if ZEND_MM_CACHE
			} else if (p->magic == MEM_BLOCK_CACHED) {
				/* skip it */
#endif
			} else if (p->magic != MEM_BLOCK_LEAK) {
			    zend_mm_panic("zend_mm_heap corrupted");
			}
		}
		p = q;
	}
	return leaks;
}

static void zend_mm_check_leaks(zend_mm_heap *heap TSRMLS_DC)
{
	zend_mm_segment *segment = heap->segments_list;
	zend_mm_block *p, *q;
	zend_uint total = 0;

	if (!segment) {
		return;
	}
	p = (zend_mm_block *) ((char *) segment + ZEND_MM_ALIGNED_SEGMENT_SIZE);
	while (1) {
		q = ZEND_MM_NEXT_BLOCK(p);
		if (q <= p ||
		    (char*)q > (char*)segment + segment->size ||
		    p->info._size != q->info._prev) {
			zend_mm_panic("zend_mm_heap corrupted");
		}
		if (!ZEND_MM_IS_FREE_BLOCK(p)) {
			if (p->magic == MEM_BLOCK_VALID) {
				long repeated;
				zend_leak_info leak;

				ZEND_MM_SET_MAGIC(p, MEM_BLOCK_LEAK);

				leak.addr = ZEND_MM_DATA_OF(p);
				leak.size = p->debug.size;
				leak.filename = p->debug.filename;
				leak.lineno = p->debug.lineno;
				leak.orig_filename = p->debug.orig_filename;
				leak.orig_lineno = p->debug.orig_lineno;

				zend_message_dispatcher(ZMSG_LOG_SCRIPT_NAME, NULL TSRMLS_CC);
				zend_message_dispatcher(ZMSG_MEMORY_LEAK_DETECTED, &leak TSRMLS_CC);
				repeated = zend_mm_find_leaks(segment, p);
				total += 1 + repeated;
				if (repeated) {
					zend_message_dispatcher(ZMSG_MEMORY_LEAK_REPEATED, (void *)(zend_uintptr_t)repeated TSRMLS_CC);
				}
#if ZEND_MM_CACHE
			} else if (p->magic == MEM_BLOCK_CACHED) {
				/* skip it */
#endif
			} else if (p->magic != MEM_BLOCK_LEAK) {
				zend_mm_panic("zend_mm_heap corrupted");
			}
		}
		if (ZEND_MM_IS_GUARD_BLOCK(q)) {
			segment = segment->next_segment;
			if (!segment) {
				break;
			}
			q = (zend_mm_block *) ((char *) segment + ZEND_MM_ALIGNED_SEGMENT_SIZE);
		}
		p = q;
	}
	if (total) {
		zend_message_dispatcher(ZMSG_MEMORY_LEAKS_GRAND_TOTAL, &total TSRMLS_CC);
	}
}

static int zend_mm_check_ptr(zend_mm_heap *heap, void *ptr, int silent ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	zend_mm_block *p;
	int no_cache_notice = 0;
	int had_problems = 0;
	int valid_beginning = 1;

	if (silent==2) {
		silent = 1;
		no_cache_notice = 1;
	} else if (silent==3) {
		silent = 0;
		no_cache_notice = 1;
	}
	if (!silent) {
		TSRMLS_FETCH();

		zend_message_dispatcher(ZMSG_LOG_SCRIPT_NAME, NULL TSRMLS_CC);
		zend_debug_alloc_output("---------------------------------------\n");
		zend_debug_alloc_output("%s(%d) : Block "PTR_FMT" status:\n" ZEND_FILE_LINE_RELAY_CC, ptr);
		if (__zend_orig_filename) {
			zend_debug_alloc_output("%s(%d) : Actual location (location was relayed)\n" ZEND_FILE_LINE_ORIG_RELAY_CC);
		}
		if (!ptr) {
			zend_debug_alloc_output("NULL\n");
			zend_debug_alloc_output("---------------------------------------\n");
			return 0;
		}
	}

	if (!ptr) {
		if (silent) {
			return zend_mm_check_ptr(heap, ptr, 0 ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
		}
	}

	p = ZEND_MM_HEADER_OF(ptr);

#ifdef ZTS
	if (ZEND_MM_BAD_THREAD_ID(p)) {
		if (!silent) {
			zend_debug_alloc_output("Invalid pointer: ((thread_id=0x%0.8X) != (expected=0x%0.8X))\n", (long)p->thread_id, (long)tsrm_thread_id());
			had_problems = 1;
		} else {
			return zend_mm_check_ptr(heap, ptr, 0 ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
		}
	}
#endif

	if (p->info._size != ZEND_MM_NEXT_BLOCK(p)->info._prev) {
		if (!silent) {
			zend_debug_alloc_output("Invalid pointer: ((size="PTR_FMT") != (next.prev="PTR_FMT"))\n", p->info._size, ZEND_MM_NEXT_BLOCK(p)->info._prev);
			had_problems = 1;
		} else {
			return zend_mm_check_ptr(heap, ptr, 0 ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
		}
	}
	if (p->info._prev != ZEND_MM_GUARD_BLOCK &&
	    ZEND_MM_PREV_BLOCK(p)->info._size != p->info._prev) {
		if (!silent) {
			zend_debug_alloc_output("Invalid pointer: ((prev="PTR_FMT") != (prev.size="PTR_FMT"))\n", p->info._prev, ZEND_MM_PREV_BLOCK(p)->info._size);
			had_problems = 1;
		} else {
			return zend_mm_check_ptr(heap, ptr, 0 ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
		}
	}

	if (had_problems) {
		zend_debug_alloc_output("---------------------------------------\n");
		return 0;
	}

	if (!silent) {
		zend_debug_alloc_output("%10s\t","Beginning:  ");
	}

	if (!ZEND_MM_IS_USED_BLOCK(p)) {
		if (!silent) {
			if (p->magic != MEM_BLOCK_FREED) {
				zend_debug_alloc_output("Freed (magic=0x%0.8X, expected=0x%0.8X)\n", p->magic, MEM_BLOCK_FREED);
			} else {
				zend_debug_alloc_output("Freed\n");
			}
			had_problems = 1;
		} else {
			return zend_mm_check_ptr(heap, ptr, 0 ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
		}
	} else if (ZEND_MM_IS_GUARD_BLOCK(p)) {
		if (!silent) {
			if (p->magic != MEM_BLOCK_FREED) {
				zend_debug_alloc_output("Guard (magic=0x%0.8X, expected=0x%0.8X)\n", p->magic, MEM_BLOCK_FREED);
			} else {
				zend_debug_alloc_output("Guard\n");
			}
			had_problems = 1;
		} else {
			return zend_mm_check_ptr(heap, ptr, 0 ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
		}
	} else {
		switch (p->magic) {
			case MEM_BLOCK_VALID:
			case MEM_BLOCK_LEAK:
				if (!silent) {
					zend_debug_alloc_output("OK (allocated on %s:%d, %d bytes)\n", p->debug.filename, p->debug.lineno, (int)p->debug.size);
				}
				break; /* ok */
			case MEM_BLOCK_CACHED:
				if (!no_cache_notice) {
					if (!silent) {
						zend_debug_alloc_output("Cached\n");
						had_problems = 1;
					} else {
						return zend_mm_check_ptr(heap, ptr, 0 ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
					}
				}
			case MEM_BLOCK_FREED:
				if (!silent) {
					zend_debug_alloc_output("Freed (invalid)\n");
					had_problems = 1;
				} else {
					return zend_mm_check_ptr(heap, ptr, 0 ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
				}
				break;
			case MEM_BLOCK_GUARD:
				if (!silent) {
					zend_debug_alloc_output("Guard (invalid)\n");
					had_problems = 1;
				} else {
					return zend_mm_check_ptr(heap, ptr, 0 ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
				}
				break;
			default:
				if (!silent) {
					zend_debug_alloc_output("Unknown (magic=0x%0.8X, expected=0x%0.8X)\n", p->magic, MEM_BLOCK_VALID);
					had_problems = 1;
					valid_beginning = 0;
				} else {
					return zend_mm_check_ptr(heap, ptr, 0 ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
				}
				break;
		}
	}

#if ZEND_MM_HEAP_PROTECTION
	if (!valid_beginning) {
		if (!silent) {
			zend_debug_alloc_output("%10s\t", "Start:");
			zend_debug_alloc_output("Unknown\n");
			zend_debug_alloc_output("%10s\t", "End:");
			zend_debug_alloc_output("Unknown\n");
		}
	} else {
		char *end_magic = ZEND_MM_END_MAGIC_PTR(p);

		if (p->debug.start_magic == _mem_block_start_magic) {
			if (!silent) {
				zend_debug_alloc_output("%10s\t", "Start:");
				zend_debug_alloc_output("OK\n");
			}
		} else {
			char *overflow_ptr, *magic_ptr=(char *) &_mem_block_start_magic;
			int overflows=0;
			int i;

			if (silent) {
				return _mem_block_check(ptr, 0 ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
			}
			had_problems = 1;
			overflow_ptr = (char *) &p->debug.start_magic;
			i = END_MAGIC_SIZE;
			while (--i >= 0) {
				if (overflow_ptr[i]!=magic_ptr[i]) {
					overflows++;
				}
			}
			zend_debug_alloc_output("%10s\t", "Start:");
			zend_debug_alloc_output("Overflown (magic=0x%0.8X instead of 0x%0.8X)\n", p->debug.start_magic, _mem_block_start_magic);
			zend_debug_alloc_output("%10s\t","");
			if (overflows >= END_MAGIC_SIZE) {
				zend_debug_alloc_output("At least %d bytes overflown\n", END_MAGIC_SIZE);
			} else {
				zend_debug_alloc_output("%d byte(s) overflown\n", overflows);
			}
		}
		if (memcmp(end_magic, &_mem_block_end_magic, END_MAGIC_SIZE)==0) {
			if (!silent) {
				zend_debug_alloc_output("%10s\t", "End:");
				zend_debug_alloc_output("OK\n");
			}
		} else {
			char *overflow_ptr, *magic_ptr=(char *) &_mem_block_end_magic;
			int overflows=0;
			int i;

			if (silent) {
				return _mem_block_check(ptr, 0 ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
			}
			had_problems = 1;
			overflow_ptr = (char *) end_magic;

			for (i=0; i < END_MAGIC_SIZE; i++) {
				if (overflow_ptr[i]!=magic_ptr[i]) {
					overflows++;
				}
			}

			zend_debug_alloc_output("%10s\t", "End:");
			zend_debug_alloc_output("Overflown (magic=0x%0.8X instead of 0x%0.8X)\n", *end_magic, _mem_block_end_magic);
			zend_debug_alloc_output("%10s\t","");
			if (overflows >= END_MAGIC_SIZE) {
				zend_debug_alloc_output("At least %d bytes overflown\n", END_MAGIC_SIZE);
			} else {
				zend_debug_alloc_output("%d byte(s) overflown\n", overflows);
			}
		}
	}
#endif

	if (!silent) {
		zend_debug_alloc_output("---------------------------------------\n");
	}
	return ((!had_problems) ? 1 : 0);
}

static int zend_mm_check_heap(zend_mm_heap *heap, int silent ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	zend_mm_segment *segment = heap->segments_list;
	zend_mm_block *p, *q;
	int errors = 0;

	if (!segment) {
		return 0;
	}
	p = (zend_mm_block *) ((char *) segment + ZEND_MM_ALIGNED_SEGMENT_SIZE);
	while (1) {
		q = ZEND_MM_NEXT_BLOCK(p);
		if (q <= p ||
		    (char*)q > (char*)segment + segment->size ||
		    p->info._size != q->info._prev) {
			zend_mm_panic("zend_mm_heap corrupted");
		}
		if (!ZEND_MM_IS_FREE_BLOCK(p)) {
			if (p->magic == MEM_BLOCK_VALID || p->magic == MEM_BLOCK_LEAK) {
				if (!zend_mm_check_ptr(heap, ZEND_MM_DATA_OF(p), (silent?2:3) ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC)) {
					errors++;
				}
#if ZEND_MM_CACHE
			} else if (p->magic == MEM_BLOCK_CACHED) {
				/* skip it */
#endif
			} else if (p->magic != MEM_BLOCK_LEAK) {
				zend_mm_panic("zend_mm_heap corrupted");
			}
		}
		if (ZEND_MM_IS_GUARD_BLOCK(q)) {
			segment = segment->next_segment;
			if (!segment) {
				return errors;
			}
			q = (zend_mm_block *) ((char *) segment + ZEND_MM_ALIGNED_SEGMENT_SIZE);
		}
		p = q;
	}
}
#endif

ZEND_API void zend_mm_shutdown(zend_mm_heap *heap, int full_shutdown, int silent TSRMLS_DC)
{
	zend_mm_storage *storage;
	zend_mm_segment *segment;
	zend_mm_segment *prev;
	int internal;

	if (!heap->use_zend_alloc) {
		if (full_shutdown) {
			free(heap);
		}
		return;
	}

	if (heap->reserve) {
#if ZEND_DEBUG
		if (!silent) {
			_zend_mm_free_int(heap, heap->reserve ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC);
		}
#endif
		heap->reserve = NULL;
	}

#if ZEND_MM_CACHE_STAT
	if (full_shutdown) {
		FILE *f;

		f = fopen("zend_mm.log", "w");
		if (f) {
			int i,j;
			size_t size, true_size, min_size, max_size;
			int hit = 0, miss = 0;

			fprintf(f, "\nidx min_size max_size true_size  max_len     hits   misses\n");
			size = 0;
			while (1) {
				true_size = ZEND_MM_TRUE_SIZE(size);
				if (ZEND_MM_SMALL_SIZE(true_size)) {
					min_size = size;
					i = ZEND_MM_BUCKET_INDEX(true_size);
					size++;
					while (1) {
						true_size = ZEND_MM_TRUE_SIZE(size);
						if (ZEND_MM_SMALL_SIZE(true_size)) {
							j = ZEND_MM_BUCKET_INDEX(true_size);
							if (j > i) {
								max_size = size-1;
								break;
							}
						} else {
							max_size = size-1;
							break;
						}
						size++;
					}
					hit += heap->cache_stat[i].hit;
					miss += heap->cache_stat[i].miss;
					fprintf(f, "%2d %8d %8d %9d %8d %8d %8d\n", i, (int)min_size, (int)max_size, ZEND_MM_TRUE_SIZE(max_size), heap->cache_stat[i].max_count, heap->cache_stat[i].hit, heap->cache_stat[i].miss);
				} else {
					break;
				}
			}
			fprintf(f, "                                        %8d %8d\n", hit, miss);
			fprintf(f, "                                        %8d %8d\n", heap->cache_stat[ZEND_MM_NUM_BUCKETS].hit, heap->cache_stat[ZEND_MM_NUM_BUCKETS].miss);
			fclose(f);
		}
	}
#endif

#if ZEND_DEBUG
	if (!silent) {
		zend_mm_check_leaks(heap TSRMLS_CC);
	}
#endif

	internal = heap->internal;
	storage = heap->storage;
	segment = heap->segments_list;
	if (full_shutdown) {
		while (segment) {
			prev = segment;
			segment = segment->next_segment;
			ZEND_MM_STORAGE_FREE(prev);
		}
		heap->segments_list = NULL;
		storage->handlers->dtor(storage);
		if (!internal) {
			free(heap);
		}
	} else {
		if (segment) {
#ifndef ZEND_WIN32
			if (heap->reserve_size) {
				while (segment->next_segment) {
					prev = segment;
					segment = segment->next_segment;
					ZEND_MM_STORAGE_FREE(prev);
				}
				heap->segments_list = segment;
			} else {
#endif
				do {
					prev = segment;
					segment = segment->next_segment;
					ZEND_MM_STORAGE_FREE(prev);
				} while (segment);
				heap->segments_list = NULL;
#ifndef ZEND_WIN32
			}
#endif
		}
		if (heap->compact_size &&
		    heap->real_peak > heap->compact_size) {
			storage->handlers->compact(storage);
		}
		zend_mm_init(heap);
		if (heap->segments_list) {
			heap->real_size = heap->segments_list->size;
			heap->real_peak = heap->segments_list->size;
		} else {
			heap->real_size = 0;
			heap->real_peak = 0;
		}
		heap->size = 0;
		heap->peak = 0;
		if (heap->segments_list) {
			/* mark segment as a free block */
			zend_mm_free_block *b = (zend_mm_free_block*)((char*)heap->segments_list + ZEND_MM_ALIGNED_SEGMENT_SIZE);
			size_t block_size = heap->segments_list->size - ZEND_MM_ALIGNED_SEGMENT_SIZE - ZEND_MM_ALIGNED_HEADER_SIZE;

			ZEND_MM_MARK_FIRST_BLOCK(b);
			ZEND_MM_LAST_BLOCK(ZEND_MM_BLOCK_AT(b, block_size));
			ZEND_MM_BLOCK(b, ZEND_MM_FREE_BLOCK, block_size);
			zend_mm_add_to_free_list(heap, b);
		}
		if (heap->reserve_size) {
			heap->reserve = _zend_mm_alloc_int(heap, heap->reserve_size  ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC);
		}
		heap->overflow = 0;
	}
}

static void zend_mm_safe_error(zend_mm_heap *heap,
	const char *format,
	size_t limit,
#if ZEND_DEBUG
	const char *filename,
	uint lineno,
#endif
	size_t size)
{
	if (heap->reserve) {
		_zend_mm_free_int(heap, heap->reserve ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC);
		heap->reserve = NULL;
	}
	if (heap->overflow == 0) {
		const char *error_filename;
		uint error_lineno;
		TSRMLS_FETCH();
		if (zend_is_compiling(TSRMLS_C)) {
			error_filename = zend_get_compiled_filename(TSRMLS_C);
			error_lineno = zend_get_compiled_lineno(TSRMLS_C);
		} else if (EG(in_execution)) {
			error_filename = EG(active_op_array)?EG(active_op_array)->filename:NULL;
			error_lineno = EG(opline_ptr)?(*EG(opline_ptr))->lineno:0;
		} else {
			error_filename = NULL;
			error_lineno = 0;
		}
		if (!error_filename) {
			error_filename = "Unknown";
		}
		heap->overflow = 1;
		zend_try {
			zend_error_noreturn(E_ERROR,
				format,
				limit,
#if ZEND_DEBUG
				filename,
				lineno,
#endif
				size);
		} zend_catch {
			if (heap->overflow == 2) {
				fprintf(stderr, "\nFatal error: ");
				fprintf(stderr,
					format,
					limit,
#if ZEND_DEBUG
					filename,
					lineno,
#endif
					size);
				fprintf(stderr, " in %s on line %d\n", error_filename, error_lineno);
			}
/* See http://support.microsoft.com/kb/190351 */
#ifdef PHP_WIN32
			fflush(stderr);
#endif
		} zend_end_try();
	} else {
		heap->overflow = 2;
	}
	zend_bailout();
}

static zend_mm_free_block *zend_mm_search_large_block(zend_mm_heap *heap, size_t true_size)
{
	zend_mm_free_block *best_fit;
	size_t index = ZEND_MM_LARGE_BUCKET_INDEX(true_size);
	size_t bitmap = heap->large_free_bitmap >> index;
	zend_mm_free_block *p;

	if (bitmap == 0) {
		return NULL;
	}

	if (UNEXPECTED((bitmap & 1) != 0)) {
		/* Search for best "large" free block */
		zend_mm_free_block *rst = NULL;
		size_t m;
		size_t best_size = -1;

		best_fit = NULL;
		p = heap->large_free_buckets[index];
		for (m = true_size << (ZEND_MM_NUM_BUCKETS - index); ; m <<= 1) {
			if (UNEXPECTED(ZEND_MM_FREE_BLOCK_SIZE(p) == true_size)) {
				return p->next_free_block;
			} else if (ZEND_MM_FREE_BLOCK_SIZE(p) >= true_size &&
			           ZEND_MM_FREE_BLOCK_SIZE(p) < best_size) {
				best_size = ZEND_MM_FREE_BLOCK_SIZE(p);
				best_fit = p;
			}
			if ((m & (ZEND_MM_LONG_CONST(1) << (ZEND_MM_NUM_BUCKETS-1))) == 0) {
				if (p->child[1]) {
					rst = p->child[1];
				}
				if (p->child[0]) {
					p = p->child[0];
				} else {
					break;
				}
			} else if (p->child[1]) {
				p = p->child[1];
			} else {
				break;
			}
		}

		for (p = rst; p; p = p->child[p->child[0] != NULL]) {
			if (UNEXPECTED(ZEND_MM_FREE_BLOCK_SIZE(p) == true_size)) {
				return p->next_free_block;
			} else if (ZEND_MM_FREE_BLOCK_SIZE(p) > true_size &&
			           ZEND_MM_FREE_BLOCK_SIZE(p) < best_size) {
				best_size = ZEND_MM_FREE_BLOCK_SIZE(p);
				best_fit = p;
			}
		}

		if (best_fit) {
			return best_fit->next_free_block;
		}
		bitmap = bitmap >> 1;
		if (!bitmap) {
			return NULL;
		}
		index++;
	}

	/* Search for smallest "large" free block */
	best_fit = p = heap->large_free_buckets[index + zend_mm_low_bit(bitmap)];
	while ((p = p->child[p->child[0] != NULL])) {
		if (ZEND_MM_FREE_BLOCK_SIZE(p) < ZEND_MM_FREE_BLOCK_SIZE(best_fit)) {
			best_fit = p;
		}
	}
	return best_fit->next_free_block;
}

static void *_zend_mm_alloc_int(zend_mm_heap *heap, size_t size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	zend_mm_free_block *best_fit;
	size_t true_size = ZEND_MM_TRUE_SIZE(size);
	size_t block_size;
	size_t remaining_size;
	size_t segment_size;
	zend_mm_segment *segment;
	int keep_rest = 0;
#ifdef ZEND_SIGNALS
	TSRMLS_FETCH();
#endif

	HANDLE_BLOCK_INTERRUPTIONS();

	if (EXPECTED(ZEND_MM_SMALL_SIZE(true_size))) {
		size_t index = ZEND_MM_BUCKET_INDEX(true_size);
		size_t bitmap;

		if (UNEXPECTED(true_size < size)) {
			goto out_of_memory;
		}
#if ZEND_MM_CACHE
		if (EXPECTED(heap->cache[index] != NULL)) {
			/* Get block from cache */
#if ZEND_MM_CACHE_STAT
			heap->cache_stat[index].count--;
			heap->cache_stat[index].hit++;
#endif
			best_fit = heap->cache[index];
			heap->cache[index] = best_fit->prev_free_block;
			heap->cached -= true_size;
			ZEND_MM_CHECK_MAGIC(best_fit, MEM_BLOCK_CACHED);
			ZEND_MM_SET_DEBUG_INFO(best_fit, size, 1, 0);
			HANDLE_UNBLOCK_INTERRUPTIONS();
			return ZEND_MM_DATA_OF(best_fit);
 		}
#if ZEND_MM_CACHE_STAT
		heap->cache_stat[index].miss++;
#endif
#endif

		bitmap = heap->free_bitmap >> index;
		if (bitmap) {
			/* Found some "small" free block that can be used */
			index += zend_mm_low_bit(bitmap);
			best_fit = heap->free_buckets[index*2];
#if ZEND_MM_CACHE_STAT
			heap->cache_stat[ZEND_MM_NUM_BUCKETS].hit++;
#endif
			goto zend_mm_finished_searching_for_block;
		}
	}

#if ZEND_MM_CACHE_STAT
	heap->cache_stat[ZEND_MM_NUM_BUCKETS].miss++;
#endif

	best_fit = zend_mm_search_large_block(heap, true_size);

	if (!best_fit && heap->real_size >= heap->limit - heap->block_size) {
		zend_mm_free_block *p = heap->rest_buckets[0];
		size_t best_size = -1;

		while (p != ZEND_MM_REST_BUCKET(heap)) {
			if (UNEXPECTED(ZEND_MM_FREE_BLOCK_SIZE(p) == true_size)) {
				best_fit = p;
				goto zend_mm_finished_searching_for_block;
			} else if (ZEND_MM_FREE_BLOCK_SIZE(p) > true_size &&
			           ZEND_MM_FREE_BLOCK_SIZE(p) < best_size) {
				best_size = ZEND_MM_FREE_BLOCK_SIZE(p);
				best_fit = p;
			}
			p = p->prev_free_block;
		}
	}

	if (!best_fit) {
		if (true_size > heap->block_size - (ZEND_MM_ALIGNED_SEGMENT_SIZE + ZEND_MM_ALIGNED_HEADER_SIZE)) {
			/* Make sure we add a memory block which is big enough,
			   segment must have header "size" and trailer "guard" block */
			segment_size = true_size + ZEND_MM_ALIGNED_SEGMENT_SIZE + ZEND_MM_ALIGNED_HEADER_SIZE;
			segment_size = (segment_size + (heap->block_size-1)) & ~(heap->block_size-1);
			keep_rest = 1;
		} else {
			segment_size = heap->block_size;
		}

		if (segment_size < true_size ||
		    heap->real_size + segment_size > heap->limit) {
			/* Memory limit overflow */
#if ZEND_MM_CACHE
			zend_mm_free_cache(heap);
#endif
			HANDLE_UNBLOCK_INTERRUPTIONS();
#if ZEND_DEBUG
			zend_mm_safe_error(heap, "Allowed memory size of %ld bytes exhausted at %s:%d (tried to allocate %lu bytes)", heap->limit, __zend_filename, __zend_lineno, size);
#else
			zend_mm_safe_error(heap, "Allowed memory size of %ld bytes exhausted (tried to allocate %lu bytes)", heap->limit, size);
#endif
		}

		segment = (zend_mm_segment *) ZEND_MM_STORAGE_ALLOC(segment_size);

		if (!segment) {
			/* Storage manager cannot allocate memory */
#if ZEND_MM_CACHE
			zend_mm_free_cache(heap);
#endif
out_of_memory:
			HANDLE_UNBLOCK_INTERRUPTIONS();
#if ZEND_DEBUG
			zend_mm_safe_error(heap, "Out of memory (allocated %ld) at %s:%d (tried to allocate %lu bytes)", heap->real_size, __zend_filename, __zend_lineno, size);
#else
			zend_mm_safe_error(heap, "Out of memory (allocated %ld) (tried to allocate %lu bytes)", heap->real_size, size);
#endif
			return NULL;
		}

		heap->real_size += segment_size;
		if (heap->real_size > heap->real_peak) {
			heap->real_peak = heap->real_size;
		}

		segment->size = segment_size;
		segment->next_segment = heap->segments_list;
		heap->segments_list = segment;

		best_fit = (zend_mm_free_block *) ((char *) segment + ZEND_MM_ALIGNED_SEGMENT_SIZE);
		ZEND_MM_MARK_FIRST_BLOCK(best_fit);

		block_size = segment_size - ZEND_MM_ALIGNED_SEGMENT_SIZE - ZEND_MM_ALIGNED_HEADER_SIZE;

		ZEND_MM_LAST_BLOCK(ZEND_MM_BLOCK_AT(best_fit, block_size));

	} else {
zend_mm_finished_searching_for_block:
		/* remove from free list */
		ZEND_MM_CHECK_MAGIC(best_fit, MEM_BLOCK_FREED);
		ZEND_MM_CHECK_COOKIE(best_fit);
		ZEND_MM_CHECK_BLOCK_LINKAGE(best_fit);
		zend_mm_remove_from_free_list(heap, best_fit);

		block_size = ZEND_MM_FREE_BLOCK_SIZE(best_fit);
	}

	remaining_size = block_size - true_size;

	if (remaining_size < ZEND_MM_ALIGNED_MIN_HEADER_SIZE) {
		true_size = block_size;
		ZEND_MM_BLOCK(best_fit, ZEND_MM_USED_BLOCK, true_size);
	} else {
		zend_mm_free_block *new_free_block;

		/* prepare new free block */
		ZEND_MM_BLOCK(best_fit, ZEND_MM_USED_BLOCK, true_size);
		new_free_block = (zend_mm_free_block *) ZEND_MM_BLOCK_AT(best_fit, true_size);
		ZEND_MM_BLOCK(new_free_block, ZEND_MM_FREE_BLOCK, remaining_size);

		/* add the new free block to the free list */
		if (EXPECTED(!keep_rest)) {
			zend_mm_add_to_free_list(heap, new_free_block);
		} else {
			zend_mm_add_to_rest_list(heap, new_free_block);
		}
	}

	ZEND_MM_SET_DEBUG_INFO(best_fit, size, 1, 1);

	heap->size += true_size;
	if (heap->peak < heap->size) {
		heap->peak = heap->size;
	}

	HANDLE_UNBLOCK_INTERRUPTIONS();

	return ZEND_MM_DATA_OF(best_fit);
}


static void _zend_mm_free_int(zend_mm_heap *heap, void *p ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	zend_mm_block *mm_block;
	zend_mm_block *next_block;
	size_t size;
#ifdef ZEND_SIGNALS
	TSRMLS_FETCH();
#endif
	if (!ZEND_MM_VALID_PTR(p)) {
		return;
	}

	HANDLE_BLOCK_INTERRUPTIONS();

	mm_block = ZEND_MM_HEADER_OF(p);
	size = ZEND_MM_BLOCK_SIZE(mm_block);
	ZEND_MM_CHECK_PROTECTION(mm_block);

#if ZEND_DEBUG || ZEND_MM_HEAP_PROTECTION
	memset(ZEND_MM_DATA_OF(mm_block), 0x5a, mm_block->debug.size);
#endif

#if ZEND_MM_CACHE
	if (EXPECTED(ZEND_MM_SMALL_SIZE(size)) && EXPECTED(heap->cached < ZEND_MM_CACHE_SIZE)) {
		size_t index = ZEND_MM_BUCKET_INDEX(size);
		zend_mm_free_block **cache = &heap->cache[index];

		((zend_mm_free_block*)mm_block)->prev_free_block = *cache;
		*cache = (zend_mm_free_block*)mm_block;
		heap->cached += size;
		ZEND_MM_SET_MAGIC(mm_block, MEM_BLOCK_CACHED);
#if ZEND_MM_CACHE_STAT
		if (++heap->cache_stat[index].count > heap->cache_stat[index].max_count) {
			heap->cache_stat[index].max_count = heap->cache_stat[index].count;
		}
#endif
		HANDLE_UNBLOCK_INTERRUPTIONS();
		return;
	}
#endif

	heap->size -= size;

	next_block = ZEND_MM_BLOCK_AT(mm_block, size);
	if (ZEND_MM_IS_FREE_BLOCK(next_block)) {
		zend_mm_remove_from_free_list(heap, (zend_mm_free_block *) next_block);
		size += ZEND_MM_FREE_BLOCK_SIZE(next_block);
	}
	if (ZEND_MM_PREV_BLOCK_IS_FREE(mm_block)) {
		mm_block = ZEND_MM_PREV_BLOCK(mm_block);
		zend_mm_remove_from_free_list(heap, (zend_mm_free_block *) mm_block);
		size += ZEND_MM_FREE_BLOCK_SIZE(mm_block);
	}
	if (ZEND_MM_IS_FIRST_BLOCK(mm_block) &&
	    ZEND_MM_IS_GUARD_BLOCK(ZEND_MM_BLOCK_AT(mm_block, size))) {
		zend_mm_del_segment(heap, (zend_mm_segment *) ((char *)mm_block - ZEND_MM_ALIGNED_SEGMENT_SIZE));
	} else {
		ZEND_MM_BLOCK(mm_block, ZEND_MM_FREE_BLOCK, size);
		zend_mm_add_to_free_list(heap, (zend_mm_free_block *) mm_block);
	}
	HANDLE_UNBLOCK_INTERRUPTIONS();
}

static void *_zend_mm_realloc_int(zend_mm_heap *heap, void *p, size_t size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	zend_mm_block *mm_block = ZEND_MM_HEADER_OF(p);
	zend_mm_block *next_block;
	size_t true_size;
	size_t orig_size;
	void *ptr;
#ifdef ZEND_SIGNALS
	TSRMLS_FETCH();
#endif
	if (UNEXPECTED(!p) || !ZEND_MM_VALID_PTR(p)) {
		return _zend_mm_alloc_int(heap, size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
	}

	HANDLE_BLOCK_INTERRUPTIONS();

	mm_block = ZEND_MM_HEADER_OF(p);
	true_size = ZEND_MM_TRUE_SIZE(size);
	orig_size = ZEND_MM_BLOCK_SIZE(mm_block);
	ZEND_MM_CHECK_PROTECTION(mm_block);

	if (UNEXPECTED(true_size < size)) {
		goto out_of_memory;
	}

	if (true_size <= orig_size) {
		size_t remaining_size = orig_size - true_size;

		if (remaining_size >= ZEND_MM_ALIGNED_MIN_HEADER_SIZE) {
			zend_mm_free_block *new_free_block;

			next_block = ZEND_MM_BLOCK_AT(mm_block, orig_size);
			if (ZEND_MM_IS_FREE_BLOCK(next_block)) {
				remaining_size += ZEND_MM_FREE_BLOCK_SIZE(next_block);
				zend_mm_remove_from_free_list(heap, (zend_mm_free_block *) next_block);
			}

			/* prepare new free block */
			ZEND_MM_BLOCK(mm_block, ZEND_MM_USED_BLOCK, true_size);
			new_free_block = (zend_mm_free_block *) ZEND_MM_BLOCK_AT(mm_block, true_size);

			ZEND_MM_BLOCK(new_free_block, ZEND_MM_FREE_BLOCK, remaining_size);

			/* add the new free block to the free list */
			zend_mm_add_to_free_list(heap, new_free_block);
			heap->size += (true_size - orig_size);
		}
		ZEND_MM_SET_DEBUG_INFO(mm_block, size, 0, 0);
		HANDLE_UNBLOCK_INTERRUPTIONS();
		return p;
	}

#if ZEND_MM_CACHE
	if (ZEND_MM_SMALL_SIZE(true_size)) {
		size_t index = ZEND_MM_BUCKET_INDEX(true_size);

		if (heap->cache[index] != NULL) {
			zend_mm_free_block *best_fit;
			zend_mm_free_block **cache;

#if ZEND_MM_CACHE_STAT
			heap->cache_stat[index].count--;
			heap->cache_stat[index].hit++;
#endif
			best_fit = heap->cache[index];
			heap->cache[index] = best_fit->prev_free_block;
			ZEND_MM_CHECK_MAGIC(best_fit, MEM_BLOCK_CACHED);
			ZEND_MM_SET_DEBUG_INFO(best_fit, size, 1, 0);

			ptr = ZEND_MM_DATA_OF(best_fit);

#if ZEND_DEBUG || ZEND_MM_HEAP_PROTECTION
			memcpy(ptr, p, mm_block->debug.size);
#else
			memcpy(ptr, p, orig_size - ZEND_MM_ALIGNED_HEADER_SIZE);
#endif

			heap->cached -= true_size - orig_size;

			index = ZEND_MM_BUCKET_INDEX(orig_size);
			cache = &heap->cache[index];

			((zend_mm_free_block*)mm_block)->prev_free_block = *cache;
			*cache = (zend_mm_free_block*)mm_block;
			ZEND_MM_SET_MAGIC(mm_block, MEM_BLOCK_CACHED);
#if ZEND_MM_CACHE_STAT
			if (++heap->cache_stat[index].count > heap->cache_stat[index].max_count) {
				heap->cache_stat[index].max_count = heap->cache_stat[index].count;
			}
#endif

			HANDLE_UNBLOCK_INTERRUPTIONS();
			return ptr;
		}
	}
#endif

	next_block = ZEND_MM_BLOCK_AT(mm_block, orig_size);

	if (ZEND_MM_IS_FREE_BLOCK(next_block)) {
		ZEND_MM_CHECK_COOKIE(next_block);
		ZEND_MM_CHECK_BLOCK_LINKAGE(next_block);
		if (orig_size + ZEND_MM_FREE_BLOCK_SIZE(next_block) >= true_size) {
			size_t block_size = orig_size + ZEND_MM_FREE_BLOCK_SIZE(next_block);
			size_t remaining_size = block_size - true_size;

			zend_mm_remove_from_free_list(heap, (zend_mm_free_block *) next_block);

			if (remaining_size < ZEND_MM_ALIGNED_MIN_HEADER_SIZE) {
				true_size = block_size;
				ZEND_MM_BLOCK(mm_block, ZEND_MM_USED_BLOCK, true_size);
			} else {
				zend_mm_free_block *new_free_block;

				/* prepare new free block */
				ZEND_MM_BLOCK(mm_block, ZEND_MM_USED_BLOCK, true_size);
				new_free_block = (zend_mm_free_block *) ZEND_MM_BLOCK_AT(mm_block, true_size);
				ZEND_MM_BLOCK(new_free_block, ZEND_MM_FREE_BLOCK, remaining_size);

				/* add the new free block to the free list */
				if (ZEND_MM_IS_FIRST_BLOCK(mm_block) &&
				    ZEND_MM_IS_GUARD_BLOCK(ZEND_MM_BLOCK_AT(new_free_block, remaining_size))) {
					zend_mm_add_to_rest_list(heap, new_free_block);
				} else {
					zend_mm_add_to_free_list(heap, new_free_block);
				}
			}
			ZEND_MM_SET_DEBUG_INFO(mm_block, size, 0, 0);
			heap->size = heap->size + true_size - orig_size;
			if (heap->peak < heap->size) {
				heap->peak = heap->size;
			}
			HANDLE_UNBLOCK_INTERRUPTIONS();
			return p;
		} else if (ZEND_MM_IS_FIRST_BLOCK(mm_block) &&
				   ZEND_MM_IS_GUARD_BLOCK(ZEND_MM_BLOCK_AT(next_block, ZEND_MM_FREE_BLOCK_SIZE(next_block)))) {
			zend_mm_remove_from_free_list(heap, (zend_mm_free_block *) next_block);
			goto realloc_segment;
		}
	} else if (ZEND_MM_IS_FIRST_BLOCK(mm_block) && ZEND_MM_IS_GUARD_BLOCK(next_block)) {
		zend_mm_segment *segment;
		zend_mm_segment *segment_copy;
		size_t segment_size;
		size_t block_size;
		size_t remaining_size;

realloc_segment:
		/* segment size, size of block and size of guard block */
		if (true_size > heap->block_size - (ZEND_MM_ALIGNED_SEGMENT_SIZE + ZEND_MM_ALIGNED_HEADER_SIZE)) {
			segment_size = true_size+ZEND_MM_ALIGNED_SEGMENT_SIZE+ZEND_MM_ALIGNED_HEADER_SIZE;
			segment_size = (segment_size + (heap->block_size-1)) & ~(heap->block_size-1);
		} else {
			segment_size = heap->block_size;
		}

		segment_copy = (zend_mm_segment *) ((char *)mm_block - ZEND_MM_ALIGNED_SEGMENT_SIZE);
		if (segment_size < true_size ||
		    heap->real_size + segment_size - segment_copy->size > heap->limit) {
			if (ZEND_MM_IS_FREE_BLOCK(next_block)) {
				zend_mm_add_to_free_list(heap, (zend_mm_free_block *) next_block);
			}
#if ZEND_MM_CACHE
			zend_mm_free_cache(heap);
#endif
			HANDLE_UNBLOCK_INTERRUPTIONS();
#if ZEND_DEBUG
			zend_mm_safe_error(heap, "Allowed memory size of %ld bytes exhausted at %s:%d (tried to allocate %ld bytes)", heap->limit, __zend_filename, __zend_lineno, size);
#else
			zend_mm_safe_error(heap, "Allowed memory size of %ld bytes exhausted (tried to allocate %ld bytes)", heap->limit, size);
#endif
			return NULL;
		}

		segment = ZEND_MM_STORAGE_REALLOC(segment_copy, segment_size);
		if (!segment) {
#if ZEND_MM_CACHE
			zend_mm_free_cache(heap);
#endif
out_of_memory:
			HANDLE_UNBLOCK_INTERRUPTIONS();
#if ZEND_DEBUG
			zend_mm_safe_error(heap, "Out of memory (allocated %ld) at %s:%d (tried to allocate %ld bytes)", heap->real_size, __zend_filename, __zend_lineno, size);
#else
			zend_mm_safe_error(heap, "Out of memory (allocated %ld) (tried to allocate %ld bytes)", heap->real_size, size);
#endif
			return NULL;
		}
		heap->real_size += segment_size - segment->size;
		if (heap->real_size > heap->real_peak) {
			heap->real_peak = heap->real_size;
		}

		segment->size = segment_size;

		if (segment != segment_copy) {
			zend_mm_segment **seg = &heap->segments_list;
			while (*seg != segment_copy) {
				seg = &(*seg)->next_segment;
			}
			*seg = segment;
			mm_block = (zend_mm_block *) ((char *) segment + ZEND_MM_ALIGNED_SEGMENT_SIZE);
			ZEND_MM_MARK_FIRST_BLOCK(mm_block);
		}

		block_size = segment_size - ZEND_MM_ALIGNED_SEGMENT_SIZE - ZEND_MM_ALIGNED_HEADER_SIZE;
		remaining_size = block_size - true_size;

		/* setup guard block */
		ZEND_MM_LAST_BLOCK(ZEND_MM_BLOCK_AT(mm_block, block_size));

		if (remaining_size < ZEND_MM_ALIGNED_MIN_HEADER_SIZE) {
			true_size = block_size;
			ZEND_MM_BLOCK(mm_block, ZEND_MM_USED_BLOCK, true_size);
		} else {
			zend_mm_free_block *new_free_block;

			/* prepare new free block */
			ZEND_MM_BLOCK(mm_block, ZEND_MM_USED_BLOCK, true_size);
			new_free_block = (zend_mm_free_block *) ZEND_MM_BLOCK_AT(mm_block, true_size);
			ZEND_MM_BLOCK(new_free_block, ZEND_MM_FREE_BLOCK, remaining_size);

			/* add the new free block to the free list */
			zend_mm_add_to_rest_list(heap, new_free_block);
		}

		ZEND_MM_SET_DEBUG_INFO(mm_block, size, 1, 1);

		heap->size = heap->size + true_size - orig_size;
		if (heap->peak < heap->size) {
			heap->peak = heap->size;
		}

		HANDLE_UNBLOCK_INTERRUPTIONS();
		return ZEND_MM_DATA_OF(mm_block);
	}

	ptr = _zend_mm_alloc_int(heap, size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
#if ZEND_DEBUG || ZEND_MM_HEAP_PROTECTION
	memcpy(ptr, p, mm_block->debug.size);
#else
	memcpy(ptr, p, orig_size - ZEND_MM_ALIGNED_HEADER_SIZE);
#endif
	_zend_mm_free_int(heap, p ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
	HANDLE_UNBLOCK_INTERRUPTIONS();
	return ptr;
}

ZEND_API void *_zend_mm_alloc(zend_mm_heap *heap, size_t size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	return _zend_mm_alloc_int(heap, size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
}

ZEND_API void _zend_mm_free(zend_mm_heap *heap, void *p ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	_zend_mm_free_int(heap, p ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
}

ZEND_API void *_zend_mm_realloc(zend_mm_heap *heap, void *ptr, size_t size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	return _zend_mm_realloc_int(heap, ptr, size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
}

ZEND_API size_t _zend_mm_block_size(zend_mm_heap *heap, void *p ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	zend_mm_block *mm_block;

	if (!ZEND_MM_VALID_PTR(p)) {
		return 0;
	}
	mm_block = ZEND_MM_HEADER_OF(p);
	ZEND_MM_CHECK_PROTECTION(mm_block);
#if ZEND_DEBUG || ZEND_MM_HEAP_PROTECTION
	return mm_block->debug.size;
#else
	return ZEND_MM_BLOCK_SIZE(mm_block);
#endif
}

/**********************/
/* Allocation Manager */
/**********************/

typedef struct _zend_alloc_globals {
	zend_mm_heap *mm_heap;
} zend_alloc_globals;

#ifdef ZTS
static int alloc_globals_id;
# define AG(v) TSRMG(alloc_globals_id, zend_alloc_globals *, v)
#else
# define AG(v) (alloc_globals.v)
static zend_alloc_globals alloc_globals;
#endif

ZEND_API int is_zend_mm(TSRMLS_D)
{
	return AG(mm_heap)->use_zend_alloc;
}

ZEND_API void *_emalloc(size_t size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	TSRMLS_FETCH();

	if (UNEXPECTED(!AG(mm_heap)->use_zend_alloc)) {
		return AG(mm_heap)->_malloc(size);
	}
	return _zend_mm_alloc_int(AG(mm_heap), size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
}

ZEND_API void _efree(void *ptr ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	TSRMLS_FETCH();

	if (UNEXPECTED(!AG(mm_heap)->use_zend_alloc)) {
		AG(mm_heap)->_free(ptr);
		return;
	}
	_zend_mm_free_int(AG(mm_heap), ptr ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
}

ZEND_API void *_erealloc(void *ptr, size_t size, int allow_failure ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	TSRMLS_FETCH();

	if (UNEXPECTED(!AG(mm_heap)->use_zend_alloc)) {
		return AG(mm_heap)->_realloc(ptr, size);
	}
	return _zend_mm_realloc_int(AG(mm_heap), ptr, size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
}

ZEND_API size_t _zend_mem_block_size(void *ptr TSRMLS_DC ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	if (UNEXPECTED(!AG(mm_heap)->use_zend_alloc)) {
		return 0;
	}
	return _zend_mm_block_size(AG(mm_heap), ptr ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
}

#if defined(__GNUC__) && (defined(__native_client__) || defined(i386))

static inline size_t safe_address(size_t nmemb, size_t size, size_t offset)
{
	size_t res = nmemb;
	unsigned long overflow = 0;

	__asm__ ("mull %3\n\taddl %4,%0\n\tadcl $0,%1"
	     : "=&a"(res), "=&d" (overflow)
	     : "%0"(res),
	       "rm"(size),
	       "rm"(offset));

	if (UNEXPECTED(overflow)) {
		zend_error_noreturn(E_ERROR, "Possible integer overflow in memory allocation (%zu * %zu + %zu)", nmemb, size, offset);
		return 0;
	}
	return res;
}

#elif defined(__GNUC__) && defined(__x86_64__)

static inline size_t safe_address(size_t nmemb, size_t size, size_t offset)
{
        size_t res = nmemb;
        unsigned long overflow = 0;

#ifdef __ILP32__ /* x32 */
# define LP_SUFF "l"
#else /* amd64 */
# define LP_SUFF "q"
#endif

        __asm__ ("mul" LP_SUFF  " %3\n\t"
                 "add %4,%0\n\t"
                 "adc $0,%1"
             : "=&a"(res), "=&d" (overflow)
             : "%0"(res),
               "rm"(size),
               "rm"(offset));

#undef LP_SUFF
        if (UNEXPECTED(overflow)) {
                zend_error_noreturn(E_ERROR, "Possible integer overflow in memory allocation (%zu * %zu + %zu)", nmemb, size, offset);
                return 0;
        }
        return res;
}

#elif defined(__GNUC__) && defined(__arm__)

static inline size_t safe_address(size_t nmemb, size_t size, size_t offset)
{
        size_t res;
        unsigned long overflow;

        __asm__ ("umlal %0,%1,%2,%3"
             : "=r"(res), "=r"(overflow)
             : "r"(nmemb),
               "r"(size),
               "0"(offset),
               "1"(0));

        if (UNEXPECTED(overflow)) {
                zend_error_noreturn(E_ERROR, "Possible integer overflow in memory allocation (%zu * %zu + %zu)", nmemb, size, offset);
                return 0;
        }
        return res;
}

#elif defined(__GNUC__) && defined(__aarch64__)

static inline size_t safe_address(size_t nmemb, size_t size, size_t offset)
{
        size_t res;
        unsigned long overflow;

        __asm__ ("mul %0,%2,%3\n\tumulh %1,%2,%3\n\tadds %0,%0,%4\n\tadc %1,%1,xzr"
             : "=&r"(res), "=&r"(overflow)
             : "r"(nmemb),
               "r"(size),
               "r"(offset));

        if (UNEXPECTED(overflow)) {
                zend_error_noreturn(E_ERROR, "Possible integer overflow in memory allocation (%zu * %zu + %zu)", nmemb, size, offset);
                return 0;
        }
        return res;
}

#elif SIZEOF_SIZE_T == 4 && defined(HAVE_ZEND_LONG64)

static inline size_t safe_address(size_t nmemb, size_t size, size_t offset)
{
	zend_ulong64 res = (zend_ulong64)nmemb * (zend_ulong64)size + (zend_ulong64)offset;

	if (UNEXPECTED(res > (zend_ulong64)0xFFFFFFFFL)) {
		zend_error_noreturn(E_ERROR, "Possible integer overflow in memory allocation (%zu * %zu + %zu)", nmemb, size, offset);
		return 0;
	}
	return (size_t) res;
}

#else

static inline size_t safe_address(size_t nmemb, size_t size, size_t offset)
{
	size_t res = nmemb * size + offset;
	double _d  = (double)nmemb * (double)size + (double)offset;
	double _delta = (double)res - _d;

	if (UNEXPECTED((_d + _delta ) != _d)) {
		zend_error_noreturn(E_ERROR, "Possible integer overflow in memory allocation (%zu * %zu + %zu)", nmemb, size, offset);
		return 0;
	}
	return res;
}
#endif


ZEND_API void *_safe_emalloc_string(size_t nmemb, size_t size, size_t offset ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	size_t str_size = safe_address(nmemb, size, offset);
	if (UNEXPECTED(str_size > INT_MAX)) {
		zend_error_noreturn(E_ERROR, "String allocation overflow, max size is %d", INT_MAX);
	}
	return emalloc_rel(str_size);
}

ZEND_API void *_safe_emalloc(size_t nmemb, size_t size, size_t offset ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	return emalloc_rel(safe_address(nmemb, size, offset));
}

ZEND_API void *_safe_malloc(size_t nmemb, size_t size, size_t offset)
{
	return pemalloc(safe_address(nmemb, size, offset), 1);
}

ZEND_API void *_safe_erealloc(void *ptr, size_t nmemb, size_t size, size_t offset ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	return erealloc_rel(ptr, safe_address(nmemb, size, offset));
}

ZEND_API void *_safe_realloc(void *ptr, size_t nmemb, size_t size, size_t offset)
{
	return perealloc(ptr, safe_address(nmemb, size, offset), 1);
}


ZEND_API void *_ecalloc(size_t nmemb, size_t size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	void *p;
#ifdef ZEND_SIGNALS
	TSRMLS_FETCH();
#endif
	HANDLE_BLOCK_INTERRUPTIONS();

	p = _safe_emalloc(nmemb, size, 0 ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
	if (UNEXPECTED(p == NULL)) {
		HANDLE_UNBLOCK_INTERRUPTIONS();
		return p;
	}
	memset(p, 0, size * nmemb);
	HANDLE_UNBLOCK_INTERRUPTIONS();
	return p;
}

ZEND_API char *_estrdup(const char *s ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	size_t length;
	char *p;
#ifdef ZEND_SIGNALS
	TSRMLS_FETCH();
#endif

	HANDLE_BLOCK_INTERRUPTIONS();

	length = strlen(s);
	p = (char *) _emalloc(safe_address(length, 1, 1) ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
	if (UNEXPECTED(p == NULL)) {
		HANDLE_UNBLOCK_INTERRUPTIONS();
		return p;
	}
	memcpy(p, s, length+1);
	HANDLE_UNBLOCK_INTERRUPTIONS();
	return p;
}

ZEND_API char *_estrndup(const char *s, uint length ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	char *p;
#ifdef ZEND_SIGNALS
	TSRMLS_FETCH();
#endif

	HANDLE_BLOCK_INTERRUPTIONS();

	p = (char *) _emalloc(safe_address(length, 1, 1) ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
	if (UNEXPECTED(p == NULL)) {
		HANDLE_UNBLOCK_INTERRUPTIONS();
		return p;
	}
	memcpy(p, s, length);
	p[length] = 0;
	HANDLE_UNBLOCK_INTERRUPTIONS();
	return p;
}


ZEND_API char *zend_strndup(const char *s, uint length)
{
	char *p;
#ifdef ZEND_SIGNALS
	TSRMLS_FETCH();
#endif

	HANDLE_BLOCK_INTERRUPTIONS();

	p = (char *) malloc(safe_address(length, 1, 1));
	if (UNEXPECTED(p == NULL)) {
		HANDLE_UNBLOCK_INTERRUPTIONS();
		return p;
	}
	if (length) {
		memcpy(p, s, length);
	}
	p[length] = 0;
	HANDLE_UNBLOCK_INTERRUPTIONS();
	return p;
}


ZEND_API int zend_set_memory_limit(size_t memory_limit)
{
	TSRMLS_FETCH();

	AG(mm_heap)->limit = (memory_limit >= AG(mm_heap)->block_size) ? memory_limit : AG(mm_heap)->block_size;

	return SUCCESS;
}

ZEND_API size_t zend_memory_usage(int real_usage TSRMLS_DC)
{
	if (real_usage) {
		return AG(mm_heap)->real_size;
	} else {
		size_t usage = AG(mm_heap)->size;
#if ZEND_MM_CACHE
		usage -= AG(mm_heap)->cached;
#endif
		return usage;
	}
}

ZEND_API size_t zend_memory_peak_usage(int real_usage TSRMLS_DC)
{
	if (real_usage) {
		return AG(mm_heap)->real_peak;
	} else {
		return AG(mm_heap)->peak;
	}
}

ZEND_API void shutdown_memory_manager(int silent, int full_shutdown TSRMLS_DC)
{
	zend_mm_shutdown(AG(mm_heap), full_shutdown, silent TSRMLS_CC);
}

static void alloc_globals_ctor(zend_alloc_globals *alloc_globals TSRMLS_DC)
{
	char *tmp = getenv("USE_ZEND_ALLOC");

	if (tmp && !zend_atoi(tmp, 0)) {
		alloc_globals->mm_heap = malloc(sizeof(struct _zend_mm_heap));
		memset(alloc_globals->mm_heap, 0, sizeof(struct _zend_mm_heap));
		alloc_globals->mm_heap->use_zend_alloc = 0;
		alloc_globals->mm_heap->_malloc = __zend_malloc;
		alloc_globals->mm_heap->_free = free;
		alloc_globals->mm_heap->_realloc = __zend_realloc;
	} else {
		alloc_globals->mm_heap = zend_mm_startup();
	}
}

#ifdef ZTS
static void alloc_globals_dtor(zend_alloc_globals *alloc_globals TSRMLS_DC)
{
	shutdown_memory_manager(1, 1 TSRMLS_CC);
}
#endif

ZEND_API void start_memory_manager(TSRMLS_D)
{
#ifdef ZTS
	ts_allocate_id(&alloc_globals_id, sizeof(zend_alloc_globals), (ts_allocate_ctor) alloc_globals_ctor, (ts_allocate_dtor) alloc_globals_dtor);
#else
	alloc_globals_ctor(&alloc_globals);
#endif
}

ZEND_API zend_mm_heap *zend_mm_set_heap(zend_mm_heap *new_heap TSRMLS_DC)
{
	zend_mm_heap *old_heap;

	old_heap = AG(mm_heap);
	AG(mm_heap) = new_heap;
	return old_heap;
}

ZEND_API zend_mm_storage *zend_mm_get_storage(zend_mm_heap *heap)
{
	return heap->storage;
}

ZEND_API void zend_mm_set_custom_handlers(zend_mm_heap *heap,
                                          void* (*_malloc)(size_t),
                                          void  (*_free)(void*),
                                          void* (*_realloc)(void*, size_t))
{
	heap->use_zend_alloc = 0;
	heap->_malloc = _malloc;
	heap->_free = _free;
	heap->_realloc = _realloc;
}

#if ZEND_DEBUG
ZEND_API int _mem_block_check(void *ptr, int silent ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	TSRMLS_FETCH();

	if (!AG(mm_heap)->use_zend_alloc) {
		return 1;
	}
	return zend_mm_check_ptr(AG(mm_heap), ptr, silent ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
}


ZEND_API void _full_mem_check(int silent ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	int errors;
	TSRMLS_FETCH();

	if (!AG(mm_heap)->use_zend_alloc) {
		return;
	}

	zend_debug_alloc_output("------------------------------------------------\n");
	zend_debug_alloc_output("Full Memory Check at %s:%d\n" ZEND_FILE_LINE_RELAY_CC);

	errors = zend_mm_check_heap(AG(mm_heap), silent ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);

	zend_debug_alloc_output("End of full memory check %s:%d (%d errors)\n" ZEND_FILE_LINE_RELAY_CC, errors);
	zend_debug_alloc_output("------------------------------------------------\n");
}
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
