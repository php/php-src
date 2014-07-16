/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2014 Zend Technologies Ltd. (http://www.zend.com) |
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

/*
 * zend_alloc is designed to be a modern CPU cache friendly memory manager
 * for PHP. Most ideas are taken from jemalloc and tcmalloc implementations.
 *
 * All allocations are split into 3 categories:
 *
 * Huge  - the size is greater than CHUNK size (~2M by default), allocation is
 *         performed using mmap().
 *
 * Large - a number of pages (4096K by default) inside a CHUNK. Large blocks
 *         are always alligned on page boundary.
 *
 * Small - less than half of page size. Small sizes are aligned to nearest
 *         greater predefined small size (there are 30 predefined sizes:
 *         2, 4, 8, 16, 24, 32, ... 2048). Small blocks are allocated from
 *         RUNs. Each RUN is allocated as a single or few following pages.
 *         Allocation inside RUNs implemented as fetching first element from
 *         a list of free elements or first bit from bitset of free elements.
 *
 * zend_alloc allocates memory from OS by CHUNKs, these CHUNKs and huge memory
 * blocks are always aligned to CHUNK boundary. So it's very easy to determine
 * the CHUNK owning the certain pointer. Regular CHUNKs reserve a single
 * page at start for special purpose. It contains bitset of free pages,
 * few bitset for available runs of predefined small sizes, map of pages that
 * keeps information about usage of each page in this CHUNK, etc.
 *
 * zend_alloc provides familiar emalloc/efree/erealloc API, but in addition it
 * provides specialized and optimized routines to allocate blocks of predefined
 * sizes (e.g. emalloc_2(), emallc_4(), ..., emalloc_large(), etc)
 * The library uses C preprocessor tricks that substitute calls to emalloc()
 * with more specialized routines when the requested size is known.
 */

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

static ZEND_NORETURN void zend_mm_panic(const char *message)
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

#include <sys/types.h>
#include <sys/stat.h>
#if HAVE_LIMITS_H
#include <limits.h>
#endif
#include <fcntl.h>
#include <errno.h>

#ifndef _WIN32
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
# ifndef MAP_POPULATE
#  define MAP_POPULATE 0
#endif
#endif

/****************/
/* Heap Manager */
/****************/

#ifndef ZEND_MM_STAT
# define ZEND_MM_STAT 1    /* track current and peak memory usage            */
#endif
#ifndef ZEND_MM_LIMIT
# define ZEND_MM_LIMIT 1   /* support for user-defined memory limit          */
#endif
#ifndef ZEND_MM_CUSTOM
# define ZEND_MM_CUSTOM 1  /* support for custom memory allocator            */
                           /* USE_ZEND_ALLOC=0 may switch to system malloc() */
#endif
#ifndef ZEND_MM_SPEC_CT
# define ZEND_MM_SPEC_CT 0 /* use compile-time specializer                   */
#endif
#ifndef ZEND_MM_SPEC_RT
# define ZEND_MM_SPEC_RT 0 /* use run-time specializer                       */
#endif
#ifndef ZEND_MM_ERROR
# define ZEND_MM_ERROR 1   /* report system errors                           */
#endif

#ifndef ZEND_MM_CHECK
# define ZEND_MM_CHECK(condition, message)  do { \
		if (UNEXPECTED(!(condition))) { \
			zend_mm_panic(message); \
		} \
	} while (0)
#endif

typedef unsigned int  zend_mm_page_info; /* 4-byte integer */
//???typedef unsigned int zend_mm_bitset;    /* 4-byte or 8-byte integer */
typedef unsigned long zend_mm_bitset;    /* 4-byte or 8-byte integer */

#define ZEND_MM_ALIGNED_OFFSET(size, alignment) \
	(((size_t)(size)) & ((alignment) - 1))
#define ZEND_MM_ALIGNED_BASE(size, alignment) \
	(((size_t)(size)) & ~((alignment) - 1))
#define ZEND_MM_ALIGNED_SIZE_EX(size, alignment) \
	(((size_t)(size) + ((alignment) - 1)) & ~((alignment) - 1))
#define ZEND_MM_SIZE_TO_NUM(size, alignment) \
	(((size_t)(size) + ((alignment) - 1)) / (alignment))

#define ZEND_MM_BITSET_LEN		(sizeof(zend_mm_bitset) * 8)       /* 32 or 64 */
#define ZEND_MM_PAGE_MAP_LEN	(ZEND_MM_PAGES / ZEND_MM_BITSET_LEN) /* 16 or 8 */
#define ZEND_MM_ELEMENTS_LEN	(ZEND_MM_PAGE_SIZE / ZEND_MM_MIN_SMALL_SIZE / ZEND_MM_BITSET_LEN)

typedef zend_mm_bitset zend_mm_page_map[ZEND_MM_PAGE_MAP_LEN];     /* 64B */

#define ZEND_MM_FREE_LIST_END            0

#define ZEND_MM_IS_FRUM                  0x00000000
#define ZEND_MM_IS_LRUN                  0x00000400
#define ZEND_MM_IS_SRUN                  0x00000800
#define ZEND_MM_IS_NRUN                  0x00000c00

#define ZEND_MM_LRUN_PAGES_MASK          0x000003ff
#define ZEND_MM_LRUN_PAGES_OFFSET        0

#define ZEND_MM_NRUN_PAGES_MASK          0x000003ff
#define ZEND_MM_NRUN_PAGES_OFFSET        0

#define ZEND_MM_SRUN_BIN_NUM_MASK        0x0000001f
#define ZEND_MM_SRUN_BITSET_MASK         0xffff0000
#define ZEND_MM_SRUN_BIN_NUM_OFFSET      0
#define ZEND_MM_SRUN_BITSET_OFFSET       16

#define ZEND_MM_LRUN_PAGES(info)         (((info) & ZEND_MM_LRUN_PAGES_MASK) >> ZEND_MM_LRUN_PAGES_OFFSET)
#define ZEND_MM_NRUN_PAGES(info)         (((info) & ZEND_MM_NRUN_PAGES_MASK) >> ZEND_MM_NRUN_PAGES_OFFSET)
#define ZEND_MM_SRUN_BIN_NUM(info)       (((info) & ZEND_MM_SRUN_BIN_NUM_MASK) >> ZEND_MM_SRUN_BIN_NUM_OFFSET)
#define ZEND_MM_SRUN_BITSET(info)        (((info) & ZEND_MM_SRUN_BITSET_MASK) >> ZEND_MM_SRUN_BITSET_OFFSET)

#define ZEND_MM_FRUN()                   ZEND_MM_IS_FRUN
#define ZEND_MM_LRUN(count)              (ZEND_MM_IS_LRUN | ((count) << ZEND_MM_LRUN_PAGES_OFFSET))
#define ZEND_MM_NRUN(num)                (ZEND_MM_IS_NRUN | ((num) << ZEND_MM_NRUN_PAGES_OFFSET))
#define ZEND_MM_SRUN(bin_num)            (ZEND_MM_IS_SRUN | ((bin_num) << ZEND_MM_SRUN_BIN_NUM_OFFSET))

#define ZEND_MM_SRUN_BITSET_SET(num, set) \
	(ZEND_MM_SRUN(num) | ((set) << ZEND_MM_SRUN_BITSET_OFFSET))

#define ZEND_MM_SRUN_BITSET_UPDATE(info, set) \
	((info) | ((set) << ZEND_MM_SRUN_BITSET_OFFSET))

#define ZEND_MM_SRUN_BITSET_INCL(info, num) \
	((info) | (1 << ((num) + ZEND_MM_SRUN_BITSET_OFFSET)))

#define ZEND_MM_SRUN_BITSET_EXCL(info, num) \
	((info) & ~(1 << ((num) + ZEND_MM_SRUN_BITSET_OFFSET)))

#define ZEND_MM_BINS 30

typedef struct  _zend_mm_page      zend_mm_page;
typedef struct  _zend_mm_bin       zend_mm_bin;
typedef struct  _zend_mm_free_list zend_mm_free_list;
typedef struct  _zend_mm_chunk     zend_mm_chunk;
typedef struct  _zend_mm_huge_list zend_mm_huge_list;

struct _zend_mm_page {
	char               bytes[ZEND_MM_PAGE_SIZE];
};

struct _zend_mm_chunk {
	zend_mm_heap      *heap;
	zend_mm_chunk     *next;
	zend_mm_chunk     *prev;
	int                free_pages;
	int                free_tail;
	int                num;
#if ZEND_DEBUG && defined(ZTS)
	THREAD_T           thread_id;
	char               reserve[64 - (sizeof(void*) * 3 + sizeof(int) * 3 + sizeof(THREAD_T))];
#else
	char               reserve[64 - (sizeof(void*) * 3 + sizeof(int) * 3)];
#endif
	zend_mm_page_map   free_map;                 /* 64 B */
	zend_mm_page_map   small_map[ZEND_MM_BINS];  /* 1920 B (ZEND_MM_BINS <= 30) */
	zend_mm_page_info  map[ZEND_MM_PAGES];       /* 2 KB = 512 * 4 */
};

#if ZEND_DEBUG && defined(ZTS)
# define ZEND_MM_CHECK_THREAD_ID(chunk) do { \
		ZEND_MM_CHECK((chunk)->thread_id == tsrm_thread_id(), "zend_mm_heap corrupted"); \
	} while (0)
#else
# define ZEND_MM_CHECK_THREAD_ID(chunk) do { \
	} while (0);
#endif

#if ZEND_DEBUG
typedef struct _zend_mm_debug_info {
	size_t             size;
	const char        *filename;
	const char        *orig_filename;
	uint               lineno;
	uint               orig_lineno;
} zend_mm_debug_info;
#endif

struct _zend_mm_bin {
	int                next_free;
	int                num_used;
};

struct _zend_mm_free_list {
	short              next_free;
};

struct _zend_mm_huge_list {
	void              *ptr;
	size_t             size;
	zend_mm_huge_list *next;
#if ZEND_DEBUG
	zend_mm_debug_info dbg;
#endif
};

struct _zend_mm_heap {
	zend_mm_chunk     *main_chunk;
	zend_mm_chunk     *cached_chunks;
	int                chunks_count;
	int                peak_chunks_count;
	int                cached_chunks_count;
	double             avg_chunks_count;
#if ZEND_MM_STAT || ZEND_MM_LIMIT
	size_t             real_size;
#endif
#if ZEND_MM_STAT
	size_t             real_peak;
#endif
#if ZEND_MM_LIMIT
	size_t             limit;
	int                overflow;
#endif
#if ZEND_MM_STAT
	size_t             size;
	size_t             peak;
#endif
#if ZEND_MM_CUSTOM
	int                use_custom_heap;
	void              *(*_malloc)(size_t);
	void               (*_free)(void*);
	void              *(*_realloc)(void*, size_t);
#endif
	zend_mm_huge_list *huge_list;
	zend_mm_bin       *cache[ZEND_MM_BINS];
};

static ZEND_NORETURN void zend_mm_safe_error(zend_mm_heap *heap,
	const char *format,
	size_t limit,
#if ZEND_DEBUG
	const char *filename,
	uint lineno,
#endif
	size_t size)
{
	TSRMLS_FETCH();

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
	}  zend_end_try();
	heap->overflow = 0;
	zend_bailout();
	exit(1);
}

#define _BIN_DATA_SIZE(num, size, offset, elements, pages, x, y) size,
static const unsigned int bin_data_size[] = {
  ZEND_MM_BINS_INFO(_BIN_DATA_SIZE, x, y)
};

#define _BIN_DATA_OFFSET(num, size, offset, elements, pages, x, y) offset,
static const unsigned int bin_data_offset[] = {
  ZEND_MM_BINS_INFO(_BIN_DATA_OFFSET, x, y)
};

#define _BIN_DATA_ELEMENTS(num, size, offset, elements, pages, x, y) elements,
static const int bin_elements[] = {
  ZEND_MM_BINS_INFO(_BIN_DATA_ELEMENTS, x, y)
};

#define _BIN_DATA_PAGES(num, size, offset, elements, pages, x, y) pages,
static const int bin_pages[] = {
  ZEND_MM_BINS_INFO(_BIN_DATA_PAGES, x, y)
};

static zend_always_inline int zend_mm_small_size_to_bit(size_t size)
{
#if defined(__GNUC__)
	if (UNEXPECTED(size == 0)) {
		return 0;
	} else {
		return sizeof(size) * 8 -  __builtin_clzl(size);
	}
#else
	int n = 16;
	if (size == 0) return 0;
	if (size <= 0x00ff) {n -= 8; size = size << 8;}
	if (size <= 0x0fff) {n -= 4; size = size << 4;}
	if (size <= 0x3fff) {n -= 2; size = size << 2;}
	if (size <= 0x7fff) {n -= 1;}
	return n;
#endif
}

#ifndef MAX
# define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
# define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

#define MAX_EXPECTED_1(a, b) (EXPECTED((a) > (b)) ? (a) : (b))
#define MAX_EXPECTED_2(a, b) (UNEXPECTED((a) > (b)) ? (a) : (b))
#define MIN_EXPECTED_1(a, b) (EXPECTED((a) < (b)) ? (a) : (b))
#define MIN_EXPECTED_2(a, b) (UNEXPECTED((a) < (b)) ? (a) : (b))

static zend_always_inline int zend_mm_small_size_to_bin(size_t size)
{
	int n;

	if (UNEXPECTED(size <= 4)) {
		return (size > 2);
	} else {
		n = zend_mm_small_size_to_bit(size - 1);
		n = 2 + 4 * MAX_EXPECTED_2(n - 6, 0) + ((size - 1) >> MAX(n - 3, 3));
	}
	return n;
}

#define ZEND_MM_SMALL_SIZE_TO_BIN(size)  zend_mm_small_size_to_bin(size)

#define ZEND_MM_PAGE_ADDR(chunk, page_num) \
	((void*)(((zend_mm_page*)(chunk)) + (page_num)))

/*****************/
/* OS Allocation */
/*****************/

static void *zend_mm_mmap_fixed(void *addr, size_t size)
{

#ifdef _WIN32
	return VirtualAlloc(addr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
#else		
	/* MAP_FIXED leads to discarding of the old mapping, so it can't be used. */
	void *ptr = mmap(addr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON /*| MAP_POPULATE | MAP_HUGETLB*/, -1, 0);

	if (ptr == MAP_FAILED) {
#if ZEND_MM_ERROR
		fprintf(stderr, "\nmmap() failed: [%d] %s\n", errno, strerror(errno));
#endif
		return NULL;
	} else if (ptr != addr) {
		if (munmap(ptr, size) != 0) {
#if ZEND_MM_ERROR
			fprintf(stderr, "\nmunmap() failed: [%d] %s\n", errno, strerror(errno));
#endif
		}
		return NULL;
	}
	return ptr;
#endif
}

static void *zend_mm_mmap(size_t size)
{
#ifdef _WIN32
	void *ptr = VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	
	if (ptr == NULL) {
#if ZEND_MM_ERROR
		fprintf(stderr, "\nVirtualAlloc() failed: [%d]\n", GetLastError());
#endif
		return NULL;
	}
	return ptr;
#else		
	void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON /*| MAP_POPULATE | MAP_HUGETLB*/, -1, 0);

	if (ptr == MAP_FAILED) {
#if ZEND_MM_ERROR
		fprintf(stderr, "\nmmap() failed: [%d] %s\n", errno, strerror(errno));
#endif
		return NULL;
	}
	return ptr;
#endif
}

static void zend_mm_munmap(void *addr, size_t size)
{
#ifdef _WIN32
	if (VirtualFree(addr, 0, MEM_RELEASE) == 0) {
#if ZEND_MM_ERROR
		fprintf(stderr, "\nVirtualFree() failed: [%d]\n", GetLastError());
#endif
	}
#else		
	if (munmap(addr, size) != 0) {
#if ZEND_MM_ERROR
		fprintf(stderr, "\nmunmap() failed: [%d] %s\n", errno, strerror(errno));
#endif
	}
#endif
}

/***********/
/* Bitmask */
/***********/

/* number of trailing set (1) bits */
static zend_always_inline int zend_mm_bitset_nts(zend_mm_bitset bitset)
{
#if defined(__GNUC__)
	return __builtin_ctzl(~bitset);
#else
	int n;

	if (bitset == (zend_mm_bitset)-1) return ZEND_MM_BITSET_LEN;

	n = 0;
#if SIZEOF_LONG == 8
	if (sizeof(zend_mm_bitset) == 8) {
		if ((bitset & 0xffffffff) == 0xffffffff) {n += 32; bitset = bitset >> 32;}
	}
#endif
	if ((bitset & 0x0000ffff) == 0x0000ffff) {n += 16; bitset = bitset >> 16;}
	if ((bitset & 0x000000ff) == 0x000000ff) {n +=  8; bitset = bitset >>  8;}
	if ((bitset & 0x0000000f) == 0x0000000f) {n +=  4; bitset = bitset >>  4;}
	if ((bitset & 0x00000003) == 0x00000003) {n +=  2; bitset = bitset >>  2;}
	return n + (bitset & 1);
#endif
}

/* number of trailing zero bits (0x01 -> 1; 0x40 -> 6; 0x00 -> LEN) */
static zend_always_inline int zend_mm_bitset_ntz(zend_mm_bitset bitset)
{
#if defined(__GNUC__)
	return __builtin_ctzl(bitset);
#else
	int n;

	if (bitset == (zend_mm_bitset)0) return ZEND_MM_BITSET_LEN;

	n = 1;
#if SIZEOF_LONG == 8
	if (sizeof(zend_mm_bitset) == 8) {
		if ((bitset & 0xffffffff) == 0) {n += 32; bitset = bitset >> 32;}
	}
#endif
	if ((bitset & 0x0000ffff) == 0) {n += 16; bitset = bitset >> 16;}
	if ((bitset & 0x000000ff) == 0) {n +=  8; bitset = bitset >>  8;}
	if ((bitset & 0x0000000f) == 0) {n +=  4; bitset = bitset >>  4;}
	if ((bitset & 0x00000003) == 0) {n +=  2; bitset = bitset >>  2;}
	return n - (bitset & 1);
#endif
}

static zend_always_inline int zend_mm_bitset_find_zero(zend_mm_bitset *bitset, int size)
{
	int i = 0;

	do {
		zend_mm_bitset tmp = bitset[i];
		if (tmp != (zend_mm_bitset)-1) {
			return i * ZEND_MM_BITSET_LEN + zend_mm_bitset_nts(tmp);
		}
		i++;
	} while (i < size);
	return -1;
}

static zend_always_inline int zend_mm_bitset_find_one(zend_mm_bitset *bitset, int size)
{
	int i = 0;

	do {
		zend_mm_bitset tmp = bitset[i];
		if (tmp != 0) {
			return i * ZEND_MM_BITSET_LEN + zend_mm_bitset_ntz(tmp);
		}
		i++;
	} while (i < size);
	return -1;
}

static zend_always_inline int zend_mm_bitset_find_zero_and_set(zend_mm_bitset *bitset, int size)
{
	int i = 0;

	do {
		zend_mm_bitset tmp = bitset[i];
		if (tmp != (zend_mm_bitset)-1) {
			int n = zend_mm_bitset_nts(tmp);
			bitset[i] |= 1 << n;
			return i * ZEND_MM_BITSET_LEN + n;
		}
		i++;
	} while (i < size);
	return -1;
}

static zend_always_inline int zend_mm_bitset_is_set(zend_mm_bitset *bitset, int bit)
{
	return (bitset[bit / ZEND_MM_BITSET_LEN] & (1L << (bit & (ZEND_MM_BITSET_LEN-1)))) != 0;
}

static zend_always_inline void zend_mm_bitset_set_bit(zend_mm_bitset *bitset, int bit)
{
	bitset[bit / ZEND_MM_BITSET_LEN] |= (1L << (bit & (ZEND_MM_BITSET_LEN-1)));
}

static zend_always_inline void zend_mm_bitset_reset_bit(zend_mm_bitset *bitset, int bit)
{
	bitset[bit / ZEND_MM_BITSET_LEN] &= ~(1L << (bit & (ZEND_MM_BITSET_LEN-1)));
}

static zend_always_inline void zend_mm_bitset_set_range(zend_mm_bitset *bitset, int start, int len)
{
	if (len == 1) {
		zend_mm_bitset_set_bit(bitset, start);
	} else {
		int pos = start / ZEND_MM_BITSET_LEN;
		int end = (start + len - 1) / ZEND_MM_BITSET_LEN;
		int bit = start & (ZEND_MM_BITSET_LEN - 1);
		zend_mm_bitset tmp;

		if (pos != end) {
			/* set bits from "bit" to ZEND_MM_BITSET_LEN-1 */
			tmp = (zend_mm_bitset)-1 << bit;
			bitset[pos++] |= tmp;
			while (pos != end) {
				/* set all bits */
				bitset[pos++] = (zend_mm_bitset)-1;
			}
			end = (start + len - 1) & (ZEND_MM_BITSET_LEN - 1);
			/* set bits from "0" to "end" */
			tmp = (zend_mm_bitset)-1 >> ((ZEND_MM_BITSET_LEN - 1) - end);
			bitset[pos] |= tmp;
		} else {
			end = (start + len - 1) & (ZEND_MM_BITSET_LEN - 1);
			/* set bits from "bit" to "end" */
			tmp = (zend_mm_bitset)-1 << bit;
			tmp &= (zend_mm_bitset)-1 >> ((ZEND_MM_BITSET_LEN - 1) - end);
			bitset[pos] |= tmp;
		}
	}
}

static zend_always_inline void zend_mm_bitset_reset_range(zend_mm_bitset *bitset, int start, int len)
{
	if (len == 1) {
		zend_mm_bitset_reset_bit(bitset, start);
	} else {
		int pos = start / ZEND_MM_BITSET_LEN;
		int end = (start + len - 1) / ZEND_MM_BITSET_LEN;
		int bit = start & (ZEND_MM_BITSET_LEN - 1);
		zend_mm_bitset tmp;

		if (pos != end) {
			/* reset bits from "bit" to ZEND_MM_BITSET_LEN-1 */
			tmp = ~((1L << bit) - 1);
			bitset[pos++] &= ~tmp;
			while (pos != end) {
				/* set all bits */
				bitset[pos++] = 0;
			}
			end = (start + len - 1) & (ZEND_MM_BITSET_LEN - 1);
			/* reset bits from "0" to "end" */
			tmp = (zend_mm_bitset)-1 >> ((ZEND_MM_BITSET_LEN - 1) - end);
			bitset[pos] &= ~tmp;
		} else {
			end = (start + len - 1) & (ZEND_MM_BITSET_LEN - 1);
			/* reset bits from "bit" to "end" */
			tmp = (zend_mm_bitset)-1 << bit;
			tmp &= (zend_mm_bitset)-1 >> ((ZEND_MM_BITSET_LEN - 1) - end);
			bitset[pos] &= ~tmp;
		}
	}
}

static zend_always_inline int zend_mm_bitset_is_free_range(zend_mm_bitset *bitset, int start, int len)
{
	if (len == 1) {
		return !zend_mm_bitset_is_set(bitset, start);
	} else {
		int pos = start / ZEND_MM_BITSET_LEN;
		int end = (start + len - 1) / ZEND_MM_BITSET_LEN;
		int bit = start & (ZEND_MM_BITSET_LEN - 1);
		zend_mm_bitset tmp;

		if (pos != end) {
			/* set bits from "bit" to ZEND_MM_BITSET_LEN-1 */
			tmp = (zend_mm_bitset)-1 << bit;
			if ((bitset[pos++] & tmp) != 0) {
				return 0;
			}
			while (pos != end) {
				/* set all bits */
				if (bitset[pos++] != 0) {
					return 0;
				}
			}
			end = (start + len - 1) & (ZEND_MM_BITSET_LEN - 1);
			/* set bits from "0" to "end" */
			tmp = (zend_mm_bitset)-1 >> ((ZEND_MM_BITSET_LEN - 1) - end);
			return (bitset[pos] & tmp) == 0;
		} else {
			end = (start + len - 1) & (ZEND_MM_BITSET_LEN - 1);
			/* set bits from "bit" to "end" */
			tmp = (zend_mm_bitset)-1 << bit;
			tmp &= (zend_mm_bitset)-1 >> ((ZEND_MM_BITSET_LEN - 1) - end);
			return (bitset[pos] & tmp) == 0;
		}
	}
}

/**********/
/* Chunks */
/**********/

static void *zend_mm_chunk_alloc(size_t size, size_t alignment)
{
	void *ptr = zend_mm_mmap(size);

	if (ptr == NULL) {
		return NULL;
	} else if (ZEND_MM_ALIGNED_OFFSET(ptr, alignment) == 0) {
#ifdef MADV_HUGEPAGE
//	    madvise(ptr, size, MADV_HUGEPAGE);
#endif
		return ptr;
	} else {
		size_t offset;

		/* chunk has to be aligned */
		zend_mm_munmap(ptr, size);
		ptr = zend_mm_mmap(size + alignment - ZEND_MM_PAGE_SIZE);
#ifdef _WIN32
		offset = ZEND_MM_ALIGNED_OFFSET(ptr, alignment);		
		zend_mm_munmap(ptr, size + alignment - ZEND_MM_PAGE_SIZE);
		ptr = zend_mm_mmap_fixed((void*)((char*)ptr + (alignment - offset)), size);
		offset = ZEND_MM_ALIGNED_OFFSET(ptr, alignment);		
		if (offset != 0) {
			zend_mm_munmap(ptr, size);
			return NULL;
		}
		return ptr;
#else
		offset = ZEND_MM_ALIGNED_OFFSET(ptr, alignment);
		if (offset != 0) {
			offset = alignment - offset;
			zend_mm_munmap(ptr, offset);
			ptr = (char*)ptr + offset;
		} else {
			zend_mm_munmap((char*)ptr + size, alignment - ZEND_MM_PAGE_SIZE);
		}
# ifdef MADV_HUGEPAGE
//	    madvise(ptr, size, MADV_HUGEPAGE);
# endif
#endif
		return ptr;
	}
}

static zend_always_inline void zend_mm_chunk_init(zend_mm_heap *heap, zend_mm_chunk *chunk)
{
	chunk->heap = heap;
	chunk->next = heap->main_chunk;
	chunk->prev = heap->main_chunk->prev;
	chunk->prev->next = chunk;
	chunk->next->prev = chunk;
	/* mark first pages as allocated */
	chunk->free_pages = ZEND_MM_PAGES - ZEND_MM_FIRST_PAGE;
	chunk->free_tail = ZEND_MM_FIRST_PAGE;
	/* the younger chunks have bigger number */
	chunk->num = chunk->prev->num + 1;
#if ZEND_DEBUG && defined(ZTS)
	chunk->thread_id = tsrm_thread_id();
#endif
	/* mark first pages as allocated */
	chunk->free_map[0] = (1L << ZEND_MM_FIRST_PAGE) - 1;
	chunk->map[0] = ZEND_MM_LRUN(ZEND_MM_FIRST_PAGE);
}

/***********************/
/* Huge Runs (forward) */
/***********************/

static size_t zend_mm_get_huge_block_size(zend_mm_heap *heap, void *ptr ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC);
static void *zend_mm_alloc_huge(zend_mm_heap *heap, size_t size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC);
static void zend_mm_free_huge(zend_mm_heap *heap, void *ptr ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC);

#if ZEND_DEBUG
static void zend_mm_change_huge_block_size(zend_mm_heap *heap, void *ptr, size_t size, size_t dbg_size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC);
#else
static void zend_mm_change_huge_block_size(zend_mm_heap *heap, void *ptr, size_t size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC);
#endif

/**************/
/* Large Runs */
/**************/

#if ZEND_DEBUG
static void *zend_mm_alloc_pages(zend_mm_heap *heap, int pages_count, size_t size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
#else
static void *zend_mm_alloc_pages(zend_mm_heap *heap, int pages_count ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
#endif
{
	zend_mm_chunk *chunk = heap->main_chunk;
	int page_num, len;

	while (1) {
		if (UNEXPECTED(chunk->free_pages < pages_count)) {
			goto not_found;
#if 0
		} else if (UNEXPECTED(chunk->free_pages + chunk->free_tail == ZEND_MM_PAGES)) {
			if (UNEXPECTED(ZEND_MM_PAGES - chunk->free_tail < pages_count)) {
				goto not_found;
			} else {
				page_num = chunk->free_tail;
				goto found;
			}
		} else if (0) {
			/* First-Fit Search */
			int free_tail = chunk->free_tail;
			zend_mm_bitset *bitset = chunk->free_map;
			zend_mm_bitset tmp = *(bitset++);
			int i = 0;

			while (1) {
				/* skip allocated blocks */
				while (tmp == (zend_mm_bitset)-1) {
					i += ZEND_MM_BITSET_LEN;
					if (i == ZEND_MM_PAGES) {
						goto not_found;
					}
					tmp = *(bitset++);
				}
				/* find first 0 bit */
				page_num = i + zend_mm_bitset_nts(tmp);
				/* reset bits from 0 to "bit" */
				tmp &= tmp + 1;
				/* skip free blocks */
				while (tmp == 0) {
					i += ZEND_MM_BITSET_LEN;
					len = i - page_num;
					if (len >= pages_count) {
						goto found;
					} else if (i >= free_tail) {
						goto not_found;
					}
					tmp = *(bitset++);
				}
				/* find first 1 bit */
				len = (i + zend_mm_bitset_ntz(tmp)) - page_num;
				if (len >= pages_count) {
					goto found;
				}
				/* set bits from 0 to "bit" */
				tmp |= tmp - 1;
			}
#endif
		} else {
			/* Best-Fit Search */
			int best = -1;
			int best_len = ZEND_MM_PAGES;
			int free_tail = chunk->free_tail;
			zend_mm_bitset *bitset = chunk->free_map;
			zend_mm_bitset tmp = *(bitset++);
			int i = 0;

			while (1) {
				/* skip allocated blocks */
				while (tmp == (zend_mm_bitset)-1) {
					i += ZEND_MM_BITSET_LEN;
					if (i == ZEND_MM_PAGES) {
						if (best > 0) {
							page_num = best;
							goto found;
						} else {
							goto not_found;
						}
					}
					tmp = *(bitset++);
				}
				/* find first 0 bit */
				page_num = i + zend_mm_bitset_nts(tmp);
				/* reset bits from 0 to "bit" */
				tmp &= tmp + 1;
				/* skip free blocks */
				while (tmp == 0) {
					i += ZEND_MM_BITSET_LEN;
					if (i >= free_tail) {
						len = ZEND_MM_PAGES - page_num;
						if (len >= pages_count && len < best_len) {
							chunk->free_tail = page_num + pages_count;
							goto found;
						} else {
							/* set accurate value */
							chunk->free_tail = page_num;
							if (best > 0) {
								page_num = best;
								goto found;
							} else {
								goto not_found;
							}
						}
					}
					tmp = *(bitset++);
				}
				/* find first 1 bit */
				len = i + zend_mm_bitset_ntz(tmp) - page_num;
				if (len >= pages_count) {
					if (len == pages_count) {
						goto found;
					} else if (len < best_len) {
						best_len = len;
						best = page_num;
					}
				}
				/* set bits from 0 to "bit" */
				tmp |= tmp - 1;
			}
		}

not_found:
		if (chunk->next == heap->main_chunk) {
			if (heap->cached_chunks) {
				heap->cached_chunks_count--;
				chunk = heap->cached_chunks;
				heap->cached_chunks = chunk->next;
			} else {
#if ZEND_MM_LIMIT
				if (heap->real_size + ZEND_MM_CHUNK_SIZE > heap->limit) {
					if (heap->overflow == 0) {
#if ZEND_DEBUG
						zend_mm_safe_error(heap, "Allowed memory size of %ld bytes exhausted at %s:%d (tried to allocate %lu bytes)", heap->limit, __zend_filename, __zend_lineno, size);
#else
						zend_mm_safe_error(heap, "Allowed memory size of %ld bytes exhausted (tried to allocate %lu bytes)", heap->limit, ZEND_MM_PAGE_SIZE * pages_count);
#endif
						return NULL;
					}
				}
#endif
				chunk = (zend_mm_chunk*)zend_mm_chunk_alloc(ZEND_MM_CHUNK_SIZE, ZEND_MM_CHUNK_SIZE);
				if (UNEXPECTED(chunk == NULL)) {
					/* insufficient memory */
					return NULL;
				}
#if ZEND_MM_STAT || ZEND_MM_LIMIT
				heap->real_size += ZEND_MM_CHUNK_SIZE;
#endif
#if ZEND_MM_STAT
				if (heap->real_size > heap->real_peak) {
					heap->real_peak = heap->real_size;
				}
#endif
			}
			heap->chunks_count++;
			if (heap->chunks_count > heap->peak_chunks_count) {
				heap->peak_chunks_count = heap->chunks_count;
			}
			zend_mm_chunk_init(heap, chunk);
			page_num = ZEND_MM_FIRST_PAGE;
			len = ZEND_MM_PAGES - ZEND_MM_FIRST_PAGE;
			goto found;
		} else {
			chunk = chunk->next;
		}
	}

found:
	/* mark run as allocated */
	chunk->free_pages -= pages_count;
	zend_mm_bitset_set_range(chunk->free_map, page_num, pages_count);
	chunk->map[page_num] = ZEND_MM_LRUN(pages_count);
	if (page_num == chunk->free_tail) {
		chunk->free_tail = page_num + pages_count;
	}
	return ZEND_MM_PAGE_ADDR(chunk, page_num);
}

static zend_always_inline void *zend_mm_alloc_large(zend_mm_heap *heap, size_t size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	int pages_count = ZEND_MM_SIZE_TO_NUM(size, ZEND_MM_PAGE_SIZE);
#if ZEND_DEBUG
	void *ptr = zend_mm_alloc_pages(heap, pages_count, size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
#else
	void *ptr = zend_mm_alloc_pages(heap, pages_count ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
#endif
#if ZEND_MM_STAT
	heap->size += pages_count * ZEND_MM_PAGE_SIZE;
	if (UNEXPECTED(heap->size > heap->peak)) {
		heap->peak = heap->size;
	}
#endif
	return ptr;
}

static void zend_mm_free_pages(zend_mm_heap *heap, zend_mm_chunk *chunk, int page_num, int pages_count)
{
	chunk->free_pages += pages_count;
	if (chunk->free_pages == ZEND_MM_PAGES - ZEND_MM_FIRST_PAGE) {
		/* delete chunk */
		chunk->next->prev = chunk->prev;
		chunk->prev->next = chunk->next;
		heap->chunks_count--;
		if (heap->chunks_count + heap->cached_chunks_count < heap->avg_chunks_count + 0.1) {
			/* delay deletion */
			heap->cached_chunks_count++;
			chunk->next = heap->cached_chunks;
			heap->cached_chunks = chunk;
		} else {
#if ZEND_MM_STAT || ZEND_MM_LIMIT
			heap->real_size -= ZEND_MM_CHUNK_SIZE;
#endif
			if (!heap->cached_chunks || chunk->num > heap->cached_chunks->num) {
				zend_mm_munmap(chunk, ZEND_MM_CHUNK_SIZE);
			} else {
//TODO: select the best chunk to delete???
				chunk->next = heap->cached_chunks->next;
				zend_mm_munmap(heap->cached_chunks, ZEND_MM_CHUNK_SIZE);
				heap->cached_chunks = chunk;
			}
		}
	} else {
		zend_mm_bitset_reset_range(chunk->free_map, page_num, pages_count);
		chunk->map[page_num] = 0;
		if (chunk->free_tail == page_num + pages_count) {
			/* this setting may be not accurate */
			chunk->free_tail = page_num;
		}
	}
}

static zend_always_inline void zend_mm_free_large(zend_mm_heap *heap, zend_mm_chunk *chunk, int page_num, int pages_count)
{
#if ZEND_MM_STAT
	heap->size -= pages_count * ZEND_MM_PAGE_SIZE;
#endif
	zend_mm_free_pages(heap, chunk, page_num, pages_count);
}

/**************/
/* Small Runs */
/**************/

/* separating the following function allows saving 2 instructions on x86
 * on the fast patch - register save/restore in prologue/epilogue
 */
static zend_never_inline void zend_mm_small_run_got_free(zend_mm_heap *heap, int bin_num, zend_mm_bin *bin)
{
	zend_mm_chunk *chunk = (zend_mm_chunk*)ZEND_MM_ALIGNED_BASE(bin, ZEND_MM_CHUNK_SIZE);
	int page_num = ZEND_MM_ALIGNED_OFFSET(bin, ZEND_MM_CHUNK_SIZE) / ZEND_MM_PAGE_SIZE;
	zend_mm_bin *cached_bin = heap->cache[bin_num];

	/* mark this run as available for allocation of small blocks */
	zend_mm_bitset_set_bit(chunk->small_map[bin_num], page_num);

	/* put this run into cache if it's lower then the currently cached one */
	if (EXPECTED(cached_bin)) {
		zend_mm_chunk *cached_chunk = (zend_mm_chunk*)ZEND_MM_ALIGNED_BASE(cached_bin, ZEND_MM_CHUNK_SIZE);
		if (EXPECTED(chunk == cached_chunk)) {
			if (EXPECTED(bin < cached_bin)) {
				goto cache_this_bin;
			}
		} else if (EXPECTED(chunk->num < cached_chunk->num)) {
			goto cache_this_bin;
		}
	} else {
cache_this_bin:
		heap->cache[bin_num] = bin;
	}
}

static zend_always_inline void zend_mm_small_run_became_full(zend_mm_heap *heap, int bin_num, zend_mm_bin *bin)
{
	zend_mm_chunk *chunk = (zend_mm_chunk*)ZEND_MM_ALIGNED_BASE(bin, ZEND_MM_CHUNK_SIZE);
	int page_num = ZEND_MM_ALIGNED_OFFSET(bin, ZEND_MM_CHUNK_SIZE) / ZEND_MM_PAGE_SIZE;

	zend_mm_bitset_reset_bit(chunk->small_map[bin_num], page_num);
	heap->cache[bin_num] = NULL;
}

static zend_always_inline void *zend_mm_alloc_small_list_init(zend_mm_chunk *chunk, int bin_num, int page_num, zend_mm_bin *bin)
{
	int i, n;
	zend_mm_free_list *p;

	chunk->map[page_num] = ZEND_MM_SRUN(bin_num);

	/* create a linked list of elements from 1 to last */
	i = bin_data_offset[bin_num] + bin_data_size[bin_num];
	n = bin_data_offset[bin_num] + (bin_elements[bin_num] - 1) * bin_data_size[bin_num];
	bin->next_free = i;
	p = (zend_mm_free_list*)((char*)bin + i);
	do {
		i += bin_data_size[bin_num];
		p->next_free = i;
#if ZEND_DEBUG
		do {
			zend_mm_debug_info *dbg = (zend_mm_debug_info*)((char*)p + bin_data_size[bin_num] - ZEND_MM_ALIGNED_SIZE(sizeof(zend_mm_debug_info)));
			dbg->size = 0;
		} while (0);
#endif
		p = (zend_mm_free_list*)((char*)p + bin_data_size[bin_num]);
	} while (i != n);

	p->next_free = ZEND_MM_FREE_LIST_END;
	bin->num_used = 1;

	/* return first element */
	return (char*)bin + bin_data_offset[bin_num];
}

static zend_always_inline void *zend_mm_alloc_small_list(zend_mm_heap *heap, int bin_num, zend_mm_bin *bin)
{
	void *ptr = (char*)bin + bin->next_free;

	bin->next_free = ((zend_mm_free_list*)ptr)->next_free;
	bin->num_used++;
	if (UNEXPECTED(bin->next_free == ZEND_MM_FREE_LIST_END)) {
		/* run became full */
		zend_mm_small_run_became_full(heap, bin_num, bin);
	}
	return ptr;
}

static zend_always_inline int zend_mm_free_small_list(zend_mm_heap *heap, int bin_num, zend_mm_bin *bin, void *ptr)
{
	int old_next_free = bin->next_free;

#if ZEND_DEBUG
	do {
		zend_mm_debug_info *dbg = (zend_mm_debug_info*)((char*)ptr + bin_data_size[bin_num] - ZEND_MM_ALIGNED_SIZE(sizeof(zend_mm_debug_info)));
		dbg->size = 0;
	} while (0);
#endif

	((zend_mm_free_list*)ptr)->next_free = old_next_free;
	bin->next_free = (char*)ptr - (char*)bin;

	bin->num_used--;
	if (UNEXPECTED(bin->num_used == 0)) {
		/* run became free */
		return 1;
	}
	if (UNEXPECTED(old_next_free == ZEND_MM_FREE_LIST_END)) {
		/* run was completely full */
		zend_mm_small_run_got_free(heap, bin_num, bin);
	}
	return 0;
}

static zend_always_inline void *zend_mm_alloc_small_subpage_init(zend_mm_chunk *chunk, int bin_num, int page_num, zend_mm_bin *bin)
{
	chunk->map[page_num] = ZEND_MM_SRUN_BITSET_SET(bin_num, 1);
	return (void*)bin;
}

static zend_always_inline void *zend_mm_alloc_small_subpage(zend_mm_heap *heap, int bin_num, zend_mm_bin *bin)
{
	zend_mm_chunk *chunk = (zend_mm_chunk*)ZEND_MM_ALIGNED_BASE(bin, ZEND_MM_CHUNK_SIZE);
	int page_num = ZEND_MM_ALIGNED_OFFSET(bin, ZEND_MM_CHUNK_SIZE) / ZEND_MM_PAGE_SIZE;
	zend_mm_page_info info = chunk->map[page_num];
	zend_mm_page_info bitset = ZEND_MM_SRUN_BITSET(info);
	int element_num = zend_mm_bitset_nts(bitset);

	/* set the right zero bit */
	bitset = bitset | (bitset + 1);
	chunk->map[page_num] = ZEND_MM_SRUN_BITSET_UPDATE(info, bitset);
	if (UNEXPECTED((1 << (bin_elements[bin_num])) - 1 == bitset)) {
		/* run became full */
		zend_mm_small_run_became_full(heap, bin_num, bin);
	}
	return (char*)bin + element_num * bin_data_size[bin_num];
}

static zend_always_inline int zend_mm_free_small_subpage(zend_mm_heap *heap, int bin_num, zend_mm_bin *bin, int element_num)
{
	zend_mm_chunk *chunk = (zend_mm_chunk*)ZEND_MM_ALIGNED_BASE(bin, ZEND_MM_CHUNK_SIZE);
	int page_num = ZEND_MM_ALIGNED_OFFSET(bin, ZEND_MM_CHUNK_SIZE) / ZEND_MM_PAGE_SIZE;

	if (UNEXPECTED((1 << bin_elements[bin_num]) - 1 == ZEND_MM_SRUN_BITSET(chunk->map[page_num]))) {
		/* run was completely full */
		zend_mm_small_run_got_free(heap, bin_num, bin);
	}
	chunk->map[page_num] = ZEND_MM_SRUN_BITSET_EXCL(chunk->map[page_num], element_num);
	return (ZEND_MM_SRUN_BITSET(chunk->map[page_num]) == 0);
}

static zend_never_inline void *zend_mm_alloc_small_slow(zend_mm_heap *heap, int bin_num ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
    zend_mm_chunk *chunk;
    int page_num;
	zend_mm_bin *bin;
	
#if ZEND_DEBUG
	bin = (zend_mm_bin*)zend_mm_alloc_pages(heap, bin_pages[bin_num], bin_data_size[bin_num] ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
#else
	bin = (zend_mm_bin*)zend_mm_alloc_pages(heap, bin_pages[bin_num] ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
#endif
	if (UNEXPECTED(bin == NULL)) {
		/* insufficient memory */
		return NULL;
	}

	heap->cache[bin_num] = bin;
	chunk = (zend_mm_chunk*)ZEND_MM_ALIGNED_BASE(bin, ZEND_MM_CHUNK_SIZE);
	page_num = ZEND_MM_ALIGNED_OFFSET(bin, ZEND_MM_CHUNK_SIZE) / ZEND_MM_PAGE_SIZE;
	zend_mm_bitset_set_bit(chunk->small_map[bin_num], page_num);
	if (bin_pages[bin_num] > 1) {
		int i = 1;
		do {
			chunk->map[page_num+i] = ZEND_MM_NRUN(i);
			i++;
		} while (i < bin_pages[bin_num]);
	}
	if (UNEXPECTED(bin_data_offset[bin_num] == 0)) {
		return zend_mm_alloc_small_subpage_init(chunk, bin_num, page_num, bin);
	} else {
		return zend_mm_alloc_small_list_init(chunk, bin_num, page_num, bin);
	}
}

static zend_always_inline void *zend_mm_alloc_small(zend_mm_heap *heap, size_t size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	int bin_num = ZEND_MM_SMALL_SIZE_TO_BIN(size);
	zend_mm_bin *bin;

#if ZEND_MM_STAT
	heap->size += bin_data_size[bin_num];
	if (UNEXPECTED(heap->size > heap->peak)) {
		heap->peak = heap->size;
	}
#endif
	if (EXPECTED(heap->cache[bin_num])) {
		bin = heap->cache[bin_num];
	} else {
		zend_mm_chunk *chunk = heap->main_chunk;
		int page_num;

		do {
			page_num = zend_mm_bitset_find_one(chunk->small_map[bin_num], ZEND_MM_PAGE_MAP_LEN);
			if (EXPECTED(page_num > 0)) {
				bin = (zend_mm_bin*)ZEND_MM_PAGE_ADDR(chunk, page_num);
				heap->cache[bin_num] = bin;
				goto found;
			}
			chunk = chunk->next;
		} while (chunk != heap->main_chunk);
		return zend_mm_alloc_small_slow(heap, bin_num ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
	}

found:
	if (UNEXPECTED(bin_data_offset[bin_num] == 0)) {
		return zend_mm_alloc_small_subpage(heap, bin_num, bin);
	} else {
		return zend_mm_alloc_small_list(heap, bin_num, bin);
	}
}

static zend_always_inline void zend_mm_free_small(zend_mm_heap *heap, void *ptr, zend_mm_bin *bin, int bin_num)
{
	zend_mm_bin *old_bin;

#if ZEND_MM_STAT
	heap->size -= bin_data_size[bin_num];
#endif

	if (UNEXPECTED(bin_data_offset[bin_num] == 0)) {
		int element_num = ((char*)ptr - ((char*)bin + bin_data_offset[bin_num])) / bin_data_size[bin_num];
		if (EXPECTED(!zend_mm_free_small_subpage(heap, bin_num, bin, element_num))) {
			return;
		}
	} else {
		if (EXPECTED(!zend_mm_free_small_list(heap, bin_num, bin, ptr))) {
			return;
		}
	}

	old_bin = heap->cache[bin_num];
	if (old_bin != bin) {
		if (UNEXPECTED(!old_bin)) {
			/* cache empty run, it may be deleted later by GC */
			heap->cache[bin_num] = bin;
		} else {
			zend_mm_chunk *chunk = (zend_mm_chunk*)ZEND_MM_ALIGNED_BASE(bin, ZEND_MM_CHUNK_SIZE);
			int page_num;

#if 0
			if (UNEXPECTED(old_bin->num_used == 0)) {
				zend_mm_chunk *old_chunk = (zend_mm_chunk*)ZEND_MM_ALIGNED_BASE(old_bin, ZEND_MM_CHUNK_SIZE);
				if ((chunk == old_chunk &&
				     ZEND_MM_ALIGNED_OFFSET(old_bin, ZEND_MM_CHUNK_SIZE) > ZEND_MM_ALIGNED_OFFSET(bin, ZEND_MM_CHUNK_SIZE)) ||
				    old_chunk->num > chunk->num) {
					heap->cache[bin_num] = bin;
					bin = old_bin;
					chunk = old_chunk;
				} 
			}
#endif
			page_num = ZEND_MM_ALIGNED_OFFSET(bin, ZEND_MM_CHUNK_SIZE) / ZEND_MM_PAGE_SIZE;
			zend_mm_bitset_reset_bit(chunk->small_map[bin_num], page_num);
			zend_mm_free_pages(heap, chunk, page_num, bin_pages[bin_num]);
		}
	}
}

/******************/
/* Specialization */
/******************/
#if ZEND_MM_SPEC_CT
# define _BIN_ALLOCATOR(_num, _size, _offset, _elements, _pages, x, y) \
	void* ZEND_FASTCALL zend_mm_alloc_small_ ## _size(zend_mm_heap *heap) { \
		return zend_mm_alloc_small(heap, _size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC); \
	}

ZEND_MM_BINS_INFO(_BIN_ALLOCATOR, x, y)
#endif

#if ZEND_MM_SPEC_RT
# define _BIN_ALLOCATOR_I(_num, _size, _offset, _elements, _pages, x, y) \
	static zend_always_inline void *zend_mm_alloc_small_ ## _size ## _i(zend_mm_heap *heap) { \
		return zend_mm_alloc_small(heap, _size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC); \
	}

ZEND_MM_BINS_INFO(_BIN_ALLOCATOR_I, x, y)

# define _BIN_DEALLOCATOR_I(_num, _size, _offset, _elements, _pages, x, y) \
	static zend_always_inline void zend_mm_free_small_ ## _size ## _i(zend_mm_heap *heap, void *ptr, zend_mm_bin *bin) { \
		zend_mm_free_small(heap, ptr, bin, _num); \
	}

ZEND_MM_BINS_INFO(_BIN_DEALLOCATOR_I, x, y)
#endif

/********/
/* Heap */
/********/

#if ZEND_DEBUG
static zend_always_inline zend_mm_debug_info *zend_mm_get_debug_info(zend_mm_heap *heap, void *ptr)
{
	size_t page_offset = ZEND_MM_ALIGNED_OFFSET(ptr, ZEND_MM_CHUNK_SIZE);
	zend_mm_chunk *chunk;
	int page_num;
	zend_mm_page_info info;

	ZEND_MM_CHECK(page_offset != 0, "zend_mm_heap corrupted");
	chunk = (zend_mm_chunk*)ZEND_MM_ALIGNED_BASE(ptr, ZEND_MM_CHUNK_SIZE);
	page_num = page_offset / ZEND_MM_PAGE_SIZE;
	info = chunk->map[page_num];
	ZEND_MM_CHECK(chunk->heap == heap, "zend_mm_heap corrupted");
	ZEND_MM_CHECK_THREAD_ID(chunk);
	if (EXPECTED(info & ZEND_MM_IS_SRUN)) {
		int bin_num;

		if (UNEXPECTED(info & ZEND_MM_IS_LRUN)) {
			/* this is non-first page of a small run */
			page_num -= ZEND_MM_NRUN_PAGES(info);
			info = chunk->map[page_num];
		}
		bin_num = ZEND_MM_SRUN_BIN_NUM(info);
		return (zend_mm_debug_info*)((char*)ptr + bin_data_size[bin_num] - ZEND_MM_ALIGNED_SIZE(sizeof(zend_mm_debug_info)));
	} else /* if (info & ZEND_MM_IS_LRUN) */ {
		int pages_count = ZEND_MM_LRUN_PAGES(info);

		return (zend_mm_debug_info*)((char*)ptr + ZEND_MM_PAGE_SIZE * pages_count - ZEND_MM_ALIGNED_SIZE(sizeof(zend_mm_debug_info)));
	}
}
#endif

static zend_always_inline void *zend_mm_alloc_heap(zend_mm_heap *heap, size_t size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	void *ptr;
#if ZEND_DEBUG
	size_t real_size = size;
	zend_mm_debug_info *dbg;

	size = ZEND_MM_ALIGNED_SIZE(size) + ZEND_MM_ALIGNED_SIZE(sizeof(zend_mm_debug_info));
#endif
	if (size <= ZEND_MM_MAX_SMALL_SIZE) {
#if ZEND_MM_SPEC_RT
# if defined(__GNUC__) && __GNUC__ >= 3 && 0
#  define _BIN_ALLOCATOR_CASE(_num, _size, _offset, _elements, _pages, heap, ptr) \
	label_ ## _size: return zend_mm_alloc_small_ ## _size ## _i(heap);
#  define _BIN_ALLOCATOR_LABEL(_num, _size, _offset, _elements, _pages, heap, ptr) \
		&&label_ ## _size,
		static const void **labels[] = {
			ZEND_MM_BINS_INFO(_BIN_ALLOCATOR_LABEL, x, y)
		};
		goto *labels[ZEND_MM_SMALL_SIZE_TO_BIN(size)];
		ZEND_MM_BINS_INFO(_BIN_ALLOCATOR_CASE, heap, ptr);
# else
#  define _BIN_ALLOCATOR_CASE(_num, _size, _offset, _elements, _pages, heap, size) \
	case _num: return zend_mm_alloc_small_ ## _size ## _i(heap);
		switch (ZEND_MM_SMALL_SIZE_TO_BIN(size)) {
			ZEND_MM_BINS_INFO(_BIN_ALLOCATOR_CASE, heap, size);
			default: return NULL;
		}
# endif
#else
		ptr = zend_mm_alloc_small(heap, size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
#if ZEND_DEBUG
		dbg = zend_mm_get_debug_info(heap, ptr);
		dbg->size = real_size;
		dbg->filename = __zend_filename;
		dbg->orig_filename = __zend_orig_filename;
		dbg->lineno = __zend_lineno;
		dbg->orig_lineno = __zend_orig_lineno;
#endif
		return ptr;
#endif
	} else if (size <= ZEND_MM_MAX_LARGE_SIZE) {
		ptr = zend_mm_alloc_large(heap, size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
#if ZEND_DEBUG
		dbg = zend_mm_get_debug_info(heap, ptr);
		dbg->size = real_size;
		dbg->filename = __zend_filename;
		dbg->orig_filename = __zend_orig_filename;
		dbg->lineno = __zend_lineno;
		dbg->orig_lineno = __zend_orig_lineno;
#endif
		return ptr;
	} else {
#if ZEND_DEBUG
		size = real_size;
#endif
		return zend_mm_alloc_huge(heap, size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
	}
}

static zend_always_inline void zend_mm_free_heap(zend_mm_heap *heap, void *ptr ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	size_t page_offset = ZEND_MM_ALIGNED_OFFSET(ptr, ZEND_MM_CHUNK_SIZE);

	if (UNEXPECTED(page_offset == 0)) {
		if (ptr != NULL) {
			zend_mm_free_huge(heap, ptr ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
		}
	} else {
		zend_mm_chunk *chunk = (zend_mm_chunk*)ZEND_MM_ALIGNED_BASE(ptr, ZEND_MM_CHUNK_SIZE);
		int page_num = page_offset / ZEND_MM_PAGE_SIZE;
		zend_mm_page_info info = chunk->map[page_num];

		ZEND_MM_CHECK(chunk->heap == heap, "zend_mm_heap corrupted");
		ZEND_MM_CHECK_THREAD_ID(chunk);
		if (EXPECTED(info & ZEND_MM_IS_SRUN)) {
			if (UNEXPECTED(info & ZEND_MM_IS_LRUN)) {
				/* this is non-first page of a small run */
				page_num -= ZEND_MM_NRUN_PAGES(info);
				info = chunk->map[page_num];
			}
#if ZEND_MM_SPEC_RT
# if defined(__GNUC__) && __GNUC__ >= 3 && 0
#  define _BIN_DEALLOCATOR_CASE(_num, _size, _offset, _elements, _pages, heap, ptr) \
	label_ ## _size: \
		zend_mm_free_small_ ## _size ## _i(heap, ptr, (zend_mm_bin*)ZEND_MM_PAGE_ADDR(chunk, page_num)); \
		return;
#  define _BIN_DEALLOCATOR_LABEL(_num, _size, _offset, _elements, _pages, heap, ptr) \
		&&label_ ## _size,
			{
				static const void **labels[] = {
					ZEND_MM_BINS_INFO(_BIN_DEALLOCATOR_LABEL, x, y)
				};
				goto *labels[ZEND_MM_SRUN_BIN_NUM(info)];
				ZEND_MM_BINS_INFO(_BIN_DEALLOCATOR_CASE, heap, ptr);
			}
# else
#  define _BIN_DEALLOCATOR_CASE(_num, _size, _offset, _elements, _pages, heap, ptr) \
	case _num: \
		zend_mm_free_small_ ## _size ## _i(heap, ptr, (zend_mm_bin*)ZEND_MM_PAGE_ADDR(chunk, page_num)); \
		break;
			switch (ZEND_MM_SRUN_BIN_NUM(info)) {
				ZEND_MM_BINS_INFO(_BIN_DEALLOCATOR_CASE, heap, ptr);
				default:
					break;
			}
# endif
#else
			zend_mm_free_small(heap, ptr, (zend_mm_bin*)ZEND_MM_PAGE_ADDR(chunk, page_num), ZEND_MM_SRUN_BIN_NUM(info));
#endif
		} else {
			int pages_count = ZEND_MM_LRUN_PAGES(info);

			ZEND_MM_CHECK(ZEND_MM_ALIGNED_OFFSET(page_offset, ZEND_MM_PAGE_SIZE) == 0, "zend_mm_heap corrupted");
			zend_mm_free_large(heap, chunk, page_num, pages_count);
		}
	}
}

static size_t zend_mm_size(zend_mm_heap *heap, void *ptr ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	size_t page_offset = ZEND_MM_ALIGNED_OFFSET(ptr, ZEND_MM_CHUNK_SIZE);

	if (UNEXPECTED(page_offset == 0)) {
		return zend_mm_get_huge_block_size(heap, ptr ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
	} else {
		zend_mm_chunk *chunk;
#if 0 && ZEND_DEBUG
		zend_mm_debug_info *dbg = zend_mm_get_debug_info(heap, ptr);
		return dbg->size;
#else
		int page_num;
		zend_mm_page_info info;

		chunk = (zend_mm_chunk*)ZEND_MM_ALIGNED_BASE(ptr, ZEND_MM_CHUNK_SIZE);
		page_num = page_offset / ZEND_MM_PAGE_SIZE;
		info = chunk->map[page_num];
		ZEND_MM_CHECK(chunk->heap == heap, "zend_mm_heap corrupted");		
		ZEND_MM_CHECK_THREAD_ID(chunk);
		if (EXPECTED(info & ZEND_MM_IS_SRUN)) {
			if (UNEXPECTED(info & ZEND_MM_IS_LRUN)) {
				/* this is non-first page of a small run */
				page_num -= ZEND_MM_NRUN_PAGES(info);
				info = chunk->map[page_num];
			}
			return bin_data_size[ZEND_MM_SRUN_BIN_NUM(info)];
		} else /* if (info & ZEND_MM_IS_LARGE_RUN) */ {
			return ZEND_MM_LRUN_PAGES(info) * ZEND_MM_PAGE_SIZE;
		}
#endif
	}
}

static void *zend_mm_realloc_heap(zend_mm_heap *heap, void *ptr, size_t size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	size_t page_offset;
	size_t old_size;
	size_t new_size;
	void *ret;
#if ZEND_DEBUG
	size_t real_size;
	zend_mm_debug_info *dbg;
#endif

	page_offset = ZEND_MM_ALIGNED_OFFSET(ptr, ZEND_MM_CHUNK_SIZE);
	if (UNEXPECTED(page_offset == 0)) {
		if (UNEXPECTED(ptr == NULL)) {
			return zend_mm_alloc_heap(heap, size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
		}
		old_size = zend_mm_get_huge_block_size(heap, ptr ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
#if ZEND_DEBUG
		real_size = size;
		size = ZEND_MM_ALIGNED_SIZE(size) + ZEND_MM_ALIGNED_SIZE(sizeof(zend_mm_debug_info));
#endif
		if (size > ZEND_MM_MAX_LARGE_SIZE) {
#if ZEND_DEBUG
			size = real_size;
#endif
			new_size = ZEND_MM_ALIGNED_SIZE_EX(size, ZEND_MM_PAGE_SIZE);
			if (new_size == old_size) {
#if ZEND_DEBUG
				zend_mm_change_huge_block_size(heap, ptr, new_size, real_size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
#else
				zend_mm_change_huge_block_size(heap, ptr, new_size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
#endif
				return ptr;
#ifndef _WIN32
			} else if (new_size < old_size) {
				/* unmup tail */
				zend_mm_munmap((char*)ptr + new_size, old_size - new_size);
#if ZEND_MM_STAT || ZEND_MM_LIMIT
				heap->real_size -= old_size - new_size;
#endif
#if ZEND_MM_STAT
				heap->size -= old_size - new_size;
#endif
#if ZEND_DEBUG
				zend_mm_change_huge_block_size(heap, ptr, new_size, real_size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
#else
				zend_mm_change_huge_block_size(heap, ptr, new_size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
#endif
				return ptr;
			} else /* if (new_size > old_size) */ {
#if ZEND_MM_LIMIT
				if (heap->real_size + (new_size - old_size) > heap->limit) {
					if (heap->overflow == 0) {
#if ZEND_DEBUG
						zend_mm_safe_error(heap, "Allowed memory size of %ld bytes exhausted at %s:%d (tried to allocate %lu bytes)", heap->limit, __zend_filename, __zend_lineno, size);
#else
						zend_mm_safe_error(heap, "Allowed memory size of %ld bytes exhausted (tried to allocate %lu bytes)", heap->limit, size);
#endif
						return NULL;
					}
				}
#endif
				/* try to map tail right after this block */
				if (zend_mm_mmap_fixed((char*)ptr + old_size, new_size - old_size)) {
#if ZEND_MM_STAT || ZEND_MM_LIMIT
					heap->real_size += new_size - old_size;
#endif
#if ZEND_MM_STAT
					heap->size += new_size - old_size;
#endif
#if ZEND_DEBUG
					zend_mm_change_huge_block_size(heap, ptr, new_size, real_size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
#else
					zend_mm_change_huge_block_size(heap, ptr, new_size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
#endif
					return ptr;
				}
#endif
			}
		}
	} else {
		zend_mm_chunk *chunk = (zend_mm_chunk*)ZEND_MM_ALIGNED_BASE(ptr, ZEND_MM_CHUNK_SIZE);
		int page_num = page_offset / ZEND_MM_PAGE_SIZE;
		zend_mm_page_info info = chunk->map[page_num];
#if ZEND_DEBUG
		size_t real_size = size;

		size = ZEND_MM_ALIGNED_SIZE(size) + ZEND_MM_ALIGNED_SIZE(sizeof(zend_mm_debug_info));
#endif

		ZEND_MM_CHECK(chunk->heap == heap, "zend_mm_heap corrupted");		
		ZEND_MM_CHECK_THREAD_ID(chunk);
		if (info & ZEND_MM_IS_SRUN) {
			int old_bin_num, bin_num;

			if (info & ZEND_MM_IS_LRUN) {
				/* this is non-first page of a small run */
				page_num -= ZEND_MM_NRUN_PAGES(info);
				info = chunk->map[page_num];
			}
			old_bin_num = ZEND_MM_SRUN_BIN_NUM(info);
			old_size = bin_data_size[old_bin_num];
			bin_num = ZEND_MM_SMALL_SIZE_TO_BIN(size);
			if (old_bin_num == bin_num) {
#if ZEND_DEBUG
				dbg = zend_mm_get_debug_info(heap, ptr);
				dbg->size = real_size;
				dbg->filename = __zend_filename;
				dbg->orig_filename = __zend_orig_filename;
				dbg->lineno = __zend_lineno;
				dbg->orig_lineno = __zend_orig_lineno;
#endif
				return ptr;
			}
		} else /* if (info & ZEND_MM_IS_LARGE_RUN) */ {
			ZEND_MM_CHECK(ZEND_MM_ALIGNED_OFFSET(page_offset, ZEND_MM_PAGE_SIZE) == 0, "zend_mm_heap corrupted");
			old_size = ZEND_MM_LRUN_PAGES(info) * ZEND_MM_PAGE_SIZE;
			if (size > ZEND_MM_MAX_SMALL_SIZE && size <= ZEND_MM_MAX_LARGE_SIZE) {
				new_size = ZEND_MM_ALIGNED_SIZE_EX(size, ZEND_MM_PAGE_SIZE);
				if (new_size == old_size) {
#if ZEND_DEBUG
					dbg = zend_mm_get_debug_info(heap, ptr);
					dbg->size = real_size;
					dbg->filename = __zend_filename;
					dbg->orig_filename = __zend_orig_filename;
					dbg->lineno = __zend_lineno;
					dbg->orig_lineno = __zend_orig_lineno;
#endif
					return ptr;
				} else if (new_size < old_size) {
					/* free tail pages */
					int new_pages_count = new_size / ZEND_MM_PAGE_SIZE;
					int rest_pages_count = (old_size - new_size) / ZEND_MM_PAGE_SIZE;

#if ZEND_MM_STAT
					heap->size -= rest_pages_count * ZEND_MM_PAGE_SIZE;
#endif
					chunk->map[page_num] = ZEND_MM_LRUN(new_pages_count);
					chunk->free_pages += rest_pages_count;
					zend_mm_bitset_reset_range(chunk->free_map, page_num + new_pages_count, rest_pages_count);
#if ZEND_DEBUG
					dbg = zend_mm_get_debug_info(heap, ptr);
					dbg->size = real_size;
					dbg->filename = __zend_filename;
					dbg->orig_filename = __zend_orig_filename;
					dbg->lineno = __zend_lineno;
					dbg->orig_lineno = __zend_orig_lineno;
#endif
					return ptr;
				} else /* if (new_size > old_size) */ {
					int new_pages_count = new_size / ZEND_MM_PAGE_SIZE;
					int old_pages_count = old_size / ZEND_MM_PAGE_SIZE;

					/* try to allocate tail pages after this block */
					if (page_num + new_pages_count <= ZEND_MM_PAGES &&
					    zend_mm_bitset_is_free_range(chunk->free_map, page_num + old_pages_count, new_pages_count - old_pages_count)) {
#if ZEND_MM_STAT
						heap->size += (new_size - old_size);
						if (heap->size > heap->peak) {
							heap->peak = heap->size;
						}
#endif
						chunk->free_pages -= new_pages_count - old_pages_count;
						zend_mm_bitset_set_range(chunk->free_map, page_num + old_pages_count, new_pages_count - old_pages_count);
						chunk->map[page_num] = ZEND_MM_LRUN(new_pages_count);
#if ZEND_DEBUG
						dbg = zend_mm_get_debug_info(heap, ptr);
						dbg->size = real_size;
						dbg->filename = __zend_filename;
						dbg->orig_filename = __zend_orig_filename;
						dbg->lineno = __zend_lineno;
						dbg->orig_lineno = __zend_orig_lineno;
#endif
						return ptr;
					}
				}
			}
		}
#if ZEND_DEBUG
		size = real_size;
#endif
	}

	/* Naive reallocation */
	old_size = zend_mm_size(heap, ptr ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
	ret = zend_mm_alloc_heap(heap, size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
	memcpy(ret, ptr, MIN(old_size, size));
	zend_mm_free_heap(heap, ptr ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
	return ret;
}

/*********************/
/* Huge Runs (again) */
/*********************/

#if ZEND_DEBUG
static void zend_mm_add_huge_block(zend_mm_heap *heap, void *ptr, size_t size, size_t dbg_size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
#else
static void zend_mm_add_huge_block(zend_mm_heap *heap, void *ptr, size_t size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
#endif
{
	zend_mm_huge_list *list = (zend_mm_huge_list*)zend_mm_alloc_heap(heap, sizeof(zend_mm_huge_list) ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
	list->ptr = ptr;
	list->size = size;
	list->next = heap->huge_list;
#if ZEND_DEBUG
	list->dbg.size = dbg_size;
	list->dbg.filename = __zend_filename;
	list->dbg.orig_filename = __zend_orig_filename;
	list->dbg.lineno = __zend_lineno;
	list->dbg.orig_lineno = __zend_orig_lineno;
#endif
	heap->huge_list = list;
}

static size_t zend_mm_del_huge_block(zend_mm_heap *heap, void *ptr ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	zend_mm_huge_list *prev = NULL;
	zend_mm_huge_list *list = heap->huge_list;
	while (list != NULL) {
		if (list->ptr == ptr) {
			size_t size;

			if (prev) {
				prev->next = list->next;
			} else {
				heap->huge_list = list->next;
			}
			size = list->size;
			zend_mm_free_heap(heap, list ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
			return size;
		}
		prev = list;
		list = list->next;
	}
	ZEND_MM_CHECK(0, "zend_mm_heap corrupted");
	return 0;
}

static size_t zend_mm_get_huge_block_size(zend_mm_heap *heap, void *ptr ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	zend_mm_huge_list *list = heap->huge_list;
	while (list != NULL) {
		if (list->ptr == ptr) {
			return list->size;
		}
		list = list->next;
	}
	ZEND_MM_CHECK(0, "zend_mm_heap corrupted");
	return 0;
}

#if ZEND_DEBUG
static void zend_mm_change_huge_block_size(zend_mm_heap *heap, void *ptr, size_t size, size_t dbg_size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
#else
static void zend_mm_change_huge_block_size(zend_mm_heap *heap, void *ptr, size_t size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
#endif
{
	zend_mm_huge_list *list = heap->huge_list;
	while (list != NULL) {
		if (list->ptr == ptr) {
			list->size = size;
#if ZEND_DEBUG
			list->dbg.size = dbg_size;
			list->dbg.filename = __zend_filename;
			list->dbg.orig_filename = __zend_orig_filename;
			list->dbg.lineno = __zend_lineno;
			list->dbg.orig_lineno = __zend_orig_lineno;
#endif
			return;
		}
		list = list->next;
	}
}

static void *zend_mm_alloc_huge(zend_mm_heap *heap, size_t size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	size_t new_size = ZEND_MM_ALIGNED_SIZE_EX(size, ZEND_MM_PAGE_SIZE);
	void *ptr;

#if ZEND_MM_LIMIT
	if (heap->real_size + new_size > heap->limit) {
		if (heap->overflow == 0) {
#if ZEND_DEBUG
			zend_mm_safe_error(heap, "Allowed memory size of %ld bytes exhausted at %s:%d (tried to allocate %lu bytes)", heap->limit, __zend_filename, __zend_lineno, size);
#else
			zend_mm_safe_error(heap, "Allowed memory size of %ld bytes exhausted (tried to allocate %lu bytes)", heap->limit, size);
#endif
			return NULL;
		}
	}
#endif
	ptr = zend_mm_chunk_alloc(new_size, ZEND_MM_CHUNK_SIZE);
	if (UNEXPECTED(ptr == NULL)) {
		/* insufficient memory */
		return NULL;
	}
#if ZEND_DEBUG
	zend_mm_add_huge_block(heap, ptr, new_size, size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
#else
	zend_mm_add_huge_block(heap, ptr, new_size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
#endif
#if ZEND_MM_STAT || ZEND_MM_LIMIT
	heap->real_size += new_size;
#endif
#if ZEND_MM_STAT
	if (heap->real_size > heap->real_peak) {
		heap->real_peak = heap->real_size;
	}
	heap->size += new_size;
	if (heap->size > heap->peak) {
		heap->peak = heap->size;
	}
#endif
	return ptr;
}

static void zend_mm_free_huge(zend_mm_heap *heap, void *ptr ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	size_t size;

	ZEND_MM_CHECK(ZEND_MM_ALIGNED_OFFSET(ptr, ZEND_MM_CHUNK_SIZE) == 0, "zend_mm_heap corrupted");
	size = zend_mm_del_huge_block(heap, ptr ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
	zend_mm_munmap(ptr, size);
#if ZEND_MM_STAT || ZEND_MM_LIMIT
	heap->real_size -= size;
#endif
#if ZEND_MM_STAT
	heap->size -= size;
#endif
}

/******************/
/* Initialization */
/******************/

zend_mm_heap *zend_mm_init(void)
{
	zend_mm_chunk *chunk = (zend_mm_chunk*)zend_mm_chunk_alloc(ZEND_MM_CHUNK_SIZE, ZEND_MM_CHUNK_SIZE);
	zend_mm_heap *heap;

	if (UNEXPECTED(chunk == NULL)) {
#if ZEND_MM_ERROR
		fprintf(stderr, "\nCan't initialize heap: [%d] %s\n", errno, strerror(errno));
#endif
		return NULL;
	}
	heap = (zend_mm_heap*)ZEND_MM_PAGE_ADDR(chunk, ZEND_MM_FIRST_PAGE);
	chunk->heap = heap;
	chunk->next = chunk;
	chunk->prev = chunk;
	chunk->free_pages = ZEND_MM_PAGES - (ZEND_MM_FIRST_PAGE + 1);
	chunk->free_tail = ZEND_MM_FIRST_PAGE + 1;
	chunk->num = 0;
#if ZEND_DEBUG && defined(ZTS)
	chunk->thread_id = tsrm_thread_id();
#endif
	chunk->free_map[0] = (1L << (ZEND_MM_FIRST_PAGE + 1)) - 1;
	chunk->map[0] = ZEND_MM_LRUN(ZEND_MM_FIRST_PAGE);
	chunk->map[ZEND_MM_FIRST_PAGE] = ZEND_MM_LRUN(1);
	heap->main_chunk = chunk;
	heap->cached_chunks = NULL;
	heap->chunks_count = 1;
	heap->peak_chunks_count = 1;
	heap->cached_chunks_count = 0;
	heap->avg_chunks_count = 1.0;
#if ZEND_MM_STAT || ZEND_MM_LIMIT
	heap->real_size = ZEND_MM_CHUNK_SIZE;
#endif
#if ZEND_MM_STAT
	heap->real_peak = ZEND_MM_CHUNK_SIZE;
	heap->size = 0;
	heap->peak = 0;
#endif
#if ZEND_MM_LIMIT
	heap->limit = (-1L >> 1);
	heap->overflow = 0;
#endif
#if ZEND_MM_CUSTOM
	heap->use_custom_heap = 0;
#endif
	heap->huge_list = NULL;
	return heap;
}

#if ZEND_DEBUG
/******************/
/* Leak detection */
/******************/

static long zend_mm_find_leaks_small(zend_mm_chunk *p, int i, int j, zend_leak_info *leak)
{
	int bin_num = ZEND_MM_SRUN_BIN_NUM(p->map[i]);
	long count = 0;

	if (bin_data_offset[bin_num] == 0) {
		if (ZEND_MM_SRUN_BITSET(p->map[i]) != 0) {
			for (; j < bin_elements[bin_num]; j++) {
				if (ZEND_MM_SRUN_BITSET(p->map[i]) & (1 << j)) {
					zend_mm_debug_info *dbg = (zend_mm_debug_info*)((char*)p + ZEND_MM_PAGE_SIZE * i + bin_data_size[bin_num] * (j + 1) - ZEND_MM_ALIGNED_SIZE(sizeof(zend_mm_debug_info)));

					if (dbg->filename == leak->filename &&
					    dbg->lineno == leak->lineno) {

						count++;
						p->map[i] = ZEND_MM_SRUN_BITSET_EXCL(p->map[i], j);
						if (ZEND_MM_SRUN_BITSET(p->map[i]) == 0) {
							break;
						}
					}
				 }
			}
		}
	} else {
		zend_mm_bin *bin = (zend_mm_bin*)((char*)p + ZEND_MM_PAGE_SIZE * i);

		if (bin->num_used > 0) {
			zend_mm_debug_info *dbg = (zend_mm_debug_info*)((char*)p + ZEND_MM_PAGE_SIZE * i + bin_data_offset[bin_num] + bin_data_size[bin_num] * (j + 1) - ZEND_MM_ALIGNED_SIZE(sizeof(zend_mm_debug_info)));

			while (j < bin_elements[bin_num]) {
				if (dbg->size != 0 && 
				    dbg->filename == leak->filename &&
				    dbg->lineno == leak->lineno) {

					count++;
					dbg->size = 0;
					dbg->filename = NULL;
					dbg->lineno = 0;
					bin->num_used--;
					if (bin->num_used == 0) {
						break;
					}
				}
				j++;
				dbg = (zend_mm_debug_info*)((char*)dbg + bin_data_size[bin_num]);
			}
    	}
	}
	return count;
}

static long zend_mm_find_leaks(zend_mm_heap *heap, zend_mm_chunk *p, int i, zend_leak_info *leak)
{
	long count = 0;

	do {
		while (i < p->free_tail) {
			if (zend_mm_bitset_is_set(p->free_map, i)) {
				if (p->map[i] & ZEND_MM_IS_SRUN) {
					int bin_num = ZEND_MM_SRUN_BIN_NUM(p->map[i]);
					zend_mm_find_leaks_small(p, i, 0, leak);
					i += bin_pages[bin_num];
				} else /* if (p->map[i] & ZEND_MM_IS_LRUN) */ {
					int pages_count = ZEND_MM_LRUN_PAGES(p->map[i]);
					zend_mm_debug_info *dbg = (zend_mm_debug_info*)((char*)p + ZEND_MM_PAGE_SIZE * (i + pages_count) - ZEND_MM_ALIGNED_SIZE(sizeof(zend_mm_debug_info)));

					if (dbg->filename == leak->filename && dbg->lineno == leak->lineno) {
						count++;
					}
					zend_mm_bitset_reset_range(p->free_map, i, i + pages_count);
					i += pages_count;
				}
			} else {
				i++;
			}		
		}
		p = p->next;
	} while (p != heap->main_chunk);
	return count;
}

static void zend_mm_check_leaks(zend_mm_heap *heap TSRMLS_DC)
{
	zend_mm_huge_list *list;
	zend_mm_chunk *p;
	zend_leak_info leak;
	long repeated = 0;
	zend_uint total = 0;
	int i, j;

	/* find leaked huge blocks and free them */
	list = heap->huge_list;
	while (list) {
		zend_mm_huge_list *q = list;

		heap->huge_list = list->next;

		leak.addr = list->ptr;
		leak.size = list->dbg.size;
		leak.filename = list->dbg.filename;
		leak.orig_filename = list->dbg.orig_filename;
		leak.lineno = list->dbg.lineno;
		leak.orig_lineno = list->dbg.orig_lineno;
		
		zend_message_dispatcher(ZMSG_LOG_SCRIPT_NAME, NULL TSRMLS_CC);
		zend_message_dispatcher(ZMSG_MEMORY_LEAK_DETECTED, &leak TSRMLS_CC);
//???		repeated = zend_mm_find_leaks(segment, p);
		total += 1 + repeated;
		if (repeated) {
			zend_message_dispatcher(ZMSG_MEMORY_LEAK_REPEATED, (void *)(zend_uintptr_t)repeated TSRMLS_CC);
		}

		list = list->next;
		zend_mm_munmap(q->ptr, q->size);
		zend_mm_free_heap(heap, q, NULL, 0, NULL, 0);
	}

	/* for each chunk */
	p = heap->main_chunk;
	do {
		i = ZEND_MM_FIRST_PAGE;
		if (p == heap->main_chunk) {
			/* first block in main chunk contains the heap itself */
			i++;
		}
		while (i < p->free_tail) {
			if (zend_mm_bitset_is_set(p->free_map, i)) {
				if (p->map[i] & ZEND_MM_IS_SRUN) {
					int bin_num = ZEND_MM_SRUN_BIN_NUM(p->map[i]);

					if (bin_data_offset[bin_num] == 0) {
						if (ZEND_MM_SRUN_BITSET(p->map[i]) != 0) {
							for (j = 0; j < bin_elements[bin_num]; j++) {
								if (ZEND_MM_SRUN_BITSET(p->map[i]) & (1 << j)) {
									zend_mm_debug_info *dbg = (zend_mm_debug_info*)((char*)p + ZEND_MM_PAGE_SIZE * i + bin_data_size[bin_num] * (j + 1) - ZEND_MM_ALIGNED_SIZE(sizeof(zend_mm_debug_info)));

									leak.addr = (zend_mm_debug_info*)((char*)p + ZEND_MM_PAGE_SIZE * i + bin_data_size[bin_num] * j);
									leak.size = dbg->size;
									leak.filename = dbg->filename;
									leak.orig_filename = dbg->orig_filename;
									leak.lineno = dbg->lineno;
									leak.orig_lineno = dbg->orig_lineno;
		
									zend_message_dispatcher(ZMSG_LOG_SCRIPT_NAME, NULL TSRMLS_CC);
									zend_message_dispatcher(ZMSG_MEMORY_LEAK_DETECTED, &leak TSRMLS_CC);

									p->map[i] = ZEND_MM_SRUN_BITSET_EXCL(p->map[i], j);

									repeated = zend_mm_find_leaks_small(p, i, j + 1, &leak) +
									           zend_mm_find_leaks(heap, p, i + bin_pages[bin_num], &leak);
									total += 1 + repeated;
									if (repeated) {
										zend_message_dispatcher(ZMSG_MEMORY_LEAK_REPEATED, (void *)(zend_uintptr_t)repeated TSRMLS_CC);
									}
									if (ZEND_MM_SRUN_BITSET(p->map[i]) == 0) {
										break;
									}
								}
							}
						}
					} else {
						zend_mm_bin *bin = (zend_mm_bin*)((char*)p + ZEND_MM_PAGE_SIZE * i);

						if (bin->num_used > 0) {
							zend_mm_debug_info *dbg = (zend_mm_debug_info*)((char*)p + ZEND_MM_PAGE_SIZE * i + bin_data_offset[bin_num] + bin_data_size[bin_num] - ZEND_MM_ALIGNED_SIZE(sizeof(zend_mm_debug_info)));

							j = 0;
							while (j < bin_elements[bin_num]) {
								if (dbg->size != 0) {
									leak.addr = (zend_mm_debug_info*)((char*)p + ZEND_MM_PAGE_SIZE * i + bin_data_offset[bin_num] + bin_data_size[bin_num] * j);
									leak.size = dbg->size;
									leak.filename = dbg->filename;
									leak.orig_filename = dbg->orig_filename;
									leak.lineno = dbg->lineno;
									leak.orig_lineno = dbg->orig_lineno;
		
									zend_message_dispatcher(ZMSG_LOG_SCRIPT_NAME, NULL TSRMLS_CC);
									zend_message_dispatcher(ZMSG_MEMORY_LEAK_DETECTED, &leak TSRMLS_CC);

									dbg->size = 0;
									dbg->filename = NULL;
									dbg->lineno = 0;
									bin->num_used--;

									repeated = zend_mm_find_leaks_small(p, i, j + 1, &leak) +
									           zend_mm_find_leaks(heap, p, i + bin_pages[bin_num], &leak);
									total += 1 + repeated;
									if (repeated) {
										zend_message_dispatcher(ZMSG_MEMORY_LEAK_REPEATED, (void *)(zend_uintptr_t)repeated TSRMLS_CC);
									}

									if (bin->num_used == 0) {
										break;
									}
								}
								dbg = (zend_mm_debug_info*)((char*)dbg + bin_data_size[bin_num]);
								j++;
							}
						}
					}
					i += bin_pages[bin_num];
				} else /* if (p->map[i] & ZEND_MM_IS_LRUN) */ {
					int pages_count = ZEND_MM_LRUN_PAGES(p->map[i]);
					zend_mm_debug_info *dbg = (zend_mm_debug_info*)((char*)p + ZEND_MM_PAGE_SIZE * (i + pages_count) - ZEND_MM_ALIGNED_SIZE(sizeof(zend_mm_debug_info)));

					leak.addr = (void*)((char*)p + ZEND_MM_PAGE_SIZE * i);
					leak.size = dbg->size;
					leak.filename = dbg->filename;
					leak.orig_filename = dbg->orig_filename;
					leak.lineno = dbg->lineno;
					leak.orig_lineno = dbg->orig_lineno;
		
					zend_message_dispatcher(ZMSG_LOG_SCRIPT_NAME, NULL TSRMLS_CC);
					zend_message_dispatcher(ZMSG_MEMORY_LEAK_DETECTED, &leak TSRMLS_CC);

					zend_mm_bitset_reset_range(p->free_map, i, i + pages_count);
					i += pages_count;

					repeated = zend_mm_find_leaks(heap, p, i + pages_count, &leak);
					total += 1 + repeated;
					if (repeated) {
						zend_message_dispatcher(ZMSG_MEMORY_LEAK_REPEATED, (void *)(zend_uintptr_t)repeated TSRMLS_CC);
					}
				}
			} else {
				i++;
			}
		}
		p = p->next;
	} while (p != heap->main_chunk);
	if (total) {
		zend_message_dispatcher(ZMSG_MEMORY_LEAKS_GRAND_TOTAL, &total TSRMLS_CC);
	}
}
#endif

void zend_mm_shutdown(zend_mm_heap *heap, int full, int silent TSRMLS_DC)
{
	zend_mm_chunk *p;
	zend_mm_huge_list *list;

#if ZEND_MM_CUSTOM
	if (heap->use_custom_heap) {
		return;
	}
#endif

#if ZEND_DEBUG
	if (!silent) {
		zend_mm_check_leaks(heap TSRMLS_CC);
	}
#endif

	/* free huge blocks */
	list = heap->huge_list;
	while (list) {
		zend_mm_huge_list *q = list;
		list = list->next;
		zend_mm_munmap(q->ptr, q->size);
	}
	
	/* move all chunks except of the first one into the cache */
	p = heap->main_chunk->next;
	while (p != heap->main_chunk) {
		zend_mm_chunk *q = p->next;
		p->next = heap->cached_chunks;
		heap->cached_chunks = p;
		p = q;
		heap->chunks_count--;
		heap->cached_chunks_count++;
	}
	
	if (full) {
		/* free all cached chunks */
		while (heap->cached_chunks) {
			p = heap->cached_chunks;
			heap->cached_chunks = p->next;
			zend_mm_munmap(p, ZEND_MM_CHUNK_SIZE);
		}
		/* free the first chunk */
		zend_mm_munmap(heap->main_chunk, ZEND_MM_CHUNK_SIZE);
	} else {
		/* free some cached chunks to keep average count */
		heap->avg_chunks_count = (heap->avg_chunks_count + (double)heap->peak_chunks_count) / 2.0;
		while ((double)heap->cached_chunks_count + 0.9 > heap->avg_chunks_count &&
		       heap->cached_chunks) {
			p = heap->cached_chunks;
			heap->cached_chunks = p->next;
			zend_mm_munmap(p, ZEND_MM_CHUNK_SIZE);
			heap->cached_chunks_count--;
		}
		/* clear cached chunks */
		p = heap->cached_chunks;
		while (p != NULL) {
			zend_mm_chunk *q = p->next;
			memset(p, 0, sizeof(zend_mm_chunk));
			p->next = q;
			p = q;
		}

		/* reinitialize the first chunk and heap */
		heap->chunks_count = 1;
		heap->peak_chunks_count = 1;
#if ZEND_MM_STAT || ZEND_MM_LIMIT
		heap->real_size = ZEND_MM_CHUNK_SIZE;
#endif		
#if ZEND_MM_STAT
		heap->real_peak = ZEND_MM_CHUNK_SIZE;
		heap->size = 0;
		heap->peak = 0;
#endif
		heap->huge_list = NULL;
		memset(heap->cache, 0, sizeof(heap->cache));
		p = heap->main_chunk;
		memset(p, 0, ZEND_MM_FIRST_PAGE * ZEND_MM_PAGE_SIZE);
		p->heap = heap;
		p->next = p;
		p->prev = p;
#if ZEND_DEBUG && defined(ZTS)
		p->thread_id = tsrm_thread_id();
#endif
		p->free_pages = ZEND_MM_PAGES - (ZEND_MM_FIRST_PAGE + 1);
		p->free_tail = ZEND_MM_FIRST_PAGE + 1;
		p->free_map[0] = (1L << (ZEND_MM_FIRST_PAGE + 1)) - 1;
		p->map[0] = ZEND_MM_LRUN(ZEND_MM_FIRST_PAGE);
		p->map[ZEND_MM_FIRST_PAGE] = ZEND_MM_LRUN(1);
	}
}

/**************/
/* PUBLIC API */
/**************/

ZEND_API void *_zend_mm_alloc(zend_mm_heap *heap, size_t size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	return zend_mm_alloc_heap(heap, size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
}

ZEND_API void _zend_mm_free(zend_mm_heap *heap, void *ptr ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	zend_mm_free_heap(heap, ptr ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
}

void *_zend_mm_realloc(zend_mm_heap *heap, void *ptr, size_t size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	return zend_mm_realloc_heap(heap, ptr, size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
}

ZEND_API size_t _zend_mm_block_size(zend_mm_heap *heap, void *ptr ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	return zend_mm_size(heap, ptr ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
}

#if ZEND_MM_SPEC_CT
# define _BIN_ALLOCATOR_SELECTOR_START(_num, _size, _offset, _elements, _pages, heap, size) \
	((size <= _size) ? zend_mm_alloc_small_ ## _size(heap) :
# define _BIN_ALLOCATOR_SELECTOR_END(_num, _size, _offset, _elements, _pages, heap, size) \
	)

# define ZEND_MM_ALLOCATOR(heap, size) \
	ZEND_MM_BINS_INFO(_BIN_ALLOCATOR_SELECTOR_START, heap, size) \
	((size <= ZEND_MM_MAX_LARGE_SIZE) ? zend_mm_alloc_large(heap, size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC) : zend_mm_alloc_huge(heap, size)) \
	ZEND_MM_BINS_INFO(_BIN_ALLOCATOR_SELECTOR_END, heap, size)

# define zend_mm_alloc(size) \
	(__builtin_constant_p(size) ? ZEND_MM_ALLOCATOR(_heap, size) : zend_mm_alloc_heap(_heap, size))
#endif

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
#if ZEND_MM_CUSTOM
	return !AG(mm_heap)->use_custom_heap;
#else
	return 1;
#endif
}

#if !ZEND_DEBUG
#undef _emalloc

#if ZEND_MM_CUSTOM
# define ZEND_MM_CUSTOM_ALLOCATOR(size) do { \
		if (UNEXPECTED(AG(mm_heap)->use_custom_heap)) { \
			return AG(mm_heap)->_malloc(size); \
		} \
	} while (0)
# define ZEND_MM_CUSTOM_DEALLOCATOR(ptr) do { \
		if (UNEXPECTED(AG(mm_heap)->use_custom_heap)) { \
			AG(mm_heap)->_free(ptr); \
			return; \
		} \
	} while (0)
#else
# define ZEND_MM_CUSTOM_ALLOCATOR(size)
# define ZEND_MM_CUSTOM_DEALLOCATOR(ptr)
#endif

# define _ZEND_BIN_ALLOCATOR(_num, _size, _offset, _elements, _pages, x, y) \
	ZEND_API void* ZEND_FASTCALL _emalloc_ ## _size(void) { \
		TSRMLS_FETCH(); \
		ZEND_MM_CUSTOM_ALLOCATOR(_size); \
		return zend_mm_alloc_small(AG(mm_heap), _size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC); \
	}

ZEND_MM_BINS_INFO(_ZEND_BIN_ALLOCATOR, x, y)

ZEND_API void* ZEND_FASTCALL _emalloc_large(size_t size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	TSRMLS_FETCH();

	ZEND_MM_CUSTOM_ALLOCATOR(size);
	return zend_mm_alloc_large(AG(mm_heap), size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
}

ZEND_API void* ZEND_FASTCALL _emalloc_huge(size_t size)
{
	TSRMLS_FETCH();

	ZEND_MM_CUSTOM_ALLOCATOR(size);
	return zend_mm_alloc_huge(AG(mm_heap), size);
}

# define _ZEND_BIN_FREE(_num, _size, _offset, _elements, _pages, x, y) \
	ZEND_API void ZEND_FASTCALL _efree_ ## _size(void *ptr) { \
		TSRMLS_FETCH(); \
		ZEND_MM_CUSTOM_DEALLOCATOR(ptr); \
		{ \
			size_t page_offset = ZEND_MM_ALIGNED_OFFSET(ptr, ZEND_MM_CHUNK_SIZE); \
			zend_mm_chunk *chunk = (zend_mm_chunk*)ZEND_MM_ALIGNED_BASE(ptr, ZEND_MM_CHUNK_SIZE); \
			int page_num = page_offset / ZEND_MM_PAGE_SIZE; \
			ZEND_MM_CHECK(chunk->heap == AG(mm_heap), "zend_mm_heap corrupted"); \
			ZEND_MM_CHECK_THREAD_ID(chunk); \
			if (bin_pages[_num] > 1) { \
				zend_mm_page_info info = chunk->map[page_num]; \
				if (info & ZEND_MM_IS_LRUN) { \
					page_num -= ZEND_MM_NRUN_PAGES(info); \
				} \
			} \
			ZEND_ASSERT(chunk->map[page_num] & ZEND_MM_IS_SRUN); \
			ZEND_ASSERT(ZEND_MM_SRUN_BIN_NUM(chunk->map[page_num]) == _num); \
			zend_mm_free_small(AG(mm_heap), ptr, (zend_mm_bin*)ZEND_MM_PAGE_ADDR(chunk, page_num), _num); \
		} \
	}

ZEND_MM_BINS_INFO(_ZEND_BIN_FREE, x, y)

ZEND_API void ZEND_FASTCALL _efree_large(void *ptr, size_t size)
{
	TSRMLS_FETCH();

	ZEND_MM_CUSTOM_DEALLOCATOR(ptr);
	{
		size_t page_offset = ZEND_MM_ALIGNED_OFFSET(ptr, ZEND_MM_CHUNK_SIZE);
		zend_mm_chunk *chunk = (zend_mm_chunk*)ZEND_MM_ALIGNED_BASE(ptr, ZEND_MM_CHUNK_SIZE);
		int page_num = page_offset / ZEND_MM_PAGE_SIZE;
		int pages_count = ZEND_MM_ALIGNED_SIZE_EX(size, ZEND_MM_PAGE_SIZE) / ZEND_MM_PAGE_SIZE;

		ZEND_MM_CHECK(chunk->heap == AG(mm_heap) && ZEND_MM_ALIGNED_OFFSET(page_offset, ZEND_MM_PAGE_SIZE) == 0, "zend_mm_heap corrupted");
		ZEND_MM_CHECK_THREAD_ID(chunk);
		ZEND_ASSERT(chunk->map[page_num] & ZEND_MM_IS_LRUN);
		ZEND_ASSERT(ZEND_MM_LRUN_PAGES(chunk->map[page_num]) == pages_count);
		zend_mm_free_large(AG(mm_heap), chunk, page_num, pages_count);
	}
}

ZEND_API void ZEND_FASTCALL _efree_huge(void *ptr, size_t size)
{
	TSRMLS_FETCH();

	ZEND_MM_CUSTOM_DEALLOCATOR(ptr);
	// TODO: use size???
	zend_mm_free_huge(AG(mm_heap), ptr);
}
#endif

ZEND_API void* ZEND_FASTCALL _emalloc(size_t size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	TSRMLS_FETCH();

#if ZEND_MM_CUSTOM
	if (UNEXPECTED(AG(mm_heap)->use_custom_heap)) {
		return AG(mm_heap)->_malloc(size);
	}
#endif
	return zend_mm_alloc_heap(AG(mm_heap), size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
}

ZEND_API void ZEND_FASTCALL _efree(void *ptr ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	TSRMLS_FETCH();

#if ZEND_MM_CUSTOM
	if (UNEXPECTED(AG(mm_heap)->use_custom_heap)) {
		AG(mm_heap)->_free(ptr);
		return;
	}
#endif
	zend_mm_free_heap(AG(mm_heap), ptr ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
}

ZEND_API void* ZEND_FASTCALL _erealloc(void *ptr, size_t size, int allow_failure ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	TSRMLS_FETCH();

	if (UNEXPECTED(AG(mm_heap)->use_custom_heap)) {
		return AG(mm_heap)->_realloc(ptr, size);
	}
	return zend_mm_realloc_heap(AG(mm_heap), ptr, size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
}

ZEND_API size_t ZEND_FASTCALL _zend_mem_block_size(void *ptr TSRMLS_DC ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	if (UNEXPECTED(AG(mm_heap)->use_custom_heap)) {
		return 0;
	}
	return zend_mm_size(AG(mm_heap), ptr ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
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


ZEND_API void* ZEND_FASTCALL _safe_emalloc(size_t nmemb, size_t size, size_t offset ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	return emalloc_rel(safe_address(nmemb, size, offset));
}

ZEND_API void* ZEND_FASTCALL _safe_malloc(size_t nmemb, size_t size, size_t offset)
{
	return pemalloc(safe_address(nmemb, size, offset), 1);
}

ZEND_API void* ZEND_FASTCALL _safe_erealloc(void *ptr, size_t nmemb, size_t size, size_t offset ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	return erealloc_rel(ptr, safe_address(nmemb, size, offset));
}

ZEND_API void* ZEND_FASTCALL _safe_realloc(void *ptr, size_t nmemb, size_t size, size_t offset)
{
	return perealloc(ptr, safe_address(nmemb, size, offset), 1);
}


ZEND_API void* ZEND_FASTCALL _ecalloc(size_t nmemb, size_t size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
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

ZEND_API char* ZEND_FASTCALL _estrdup(const char *s ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	int length;
	char *p;
#ifdef ZEND_SIGNALS
	TSRMLS_FETCH();
#endif

	HANDLE_BLOCK_INTERRUPTIONS();

	length = strlen(s)+1;
	p = (char *) _emalloc(length ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
	if (UNEXPECTED(p == NULL)) {
		HANDLE_UNBLOCK_INTERRUPTIONS();
		return p;
	}
	memcpy(p, s, length);
	HANDLE_UNBLOCK_INTERRUPTIONS();
	return p;
}

ZEND_API char* ZEND_FASTCALL _estrndup(const char *s, uint length ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	char *p;
#ifdef ZEND_SIGNALS
	TSRMLS_FETCH();
#endif

	HANDLE_BLOCK_INTERRUPTIONS();

	p = (char *) _emalloc(length+1 ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
	if (UNEXPECTED(p == NULL)) {
		HANDLE_UNBLOCK_INTERRUPTIONS();
		return p;
	}
	memcpy(p, s, length);
	p[length] = 0;
	HANDLE_UNBLOCK_INTERRUPTIONS();
	return p;
}


ZEND_API char* ZEND_FASTCALL zend_strndup(const char *s, uint length)
{
	char *p;
#ifdef ZEND_SIGNALS
	TSRMLS_FETCH();
#endif

	HANDLE_BLOCK_INTERRUPTIONS();

	p = (char *) malloc(length+1);
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


ZEND_API int zend_set_memory_limit(size_t memory_limit TSRMLS_DC)
{
#if ZEND_MM_LIMIT
	AG(mm_heap)->limit = (memory_limit >= ZEND_MM_CHUNK_SIZE) ? memory_limit : ZEND_MM_CHUNK_SIZE;
#endif
	return SUCCESS;
}

ZEND_API size_t zend_memory_usage(int real_usage TSRMLS_DC)
{
#if ZEND_MM_STAT
	if (real_usage) {
		return AG(mm_heap)->real_size;
	} else {
		size_t usage = AG(mm_heap)->size;
		return usage;
	}
#endif
	return 0;
}

ZEND_API size_t zend_memory_peak_usage(int real_usage TSRMLS_DC)
{
#if ZEND_MM_STAT
	if (real_usage) {
		return AG(mm_heap)->real_peak;
	} else {
		return AG(mm_heap)->peak;
	}
#endif
	return 0;
}

ZEND_API void shutdown_memory_manager(int silent, int full_shutdown TSRMLS_DC)
{
	zend_mm_shutdown(AG(mm_heap), full_shutdown, silent TSRMLS_CC);
}

static void alloc_globals_ctor(zend_alloc_globals *alloc_globals TSRMLS_DC)
{
#if ZEND_MM_CUSTOM
	char *tmp = getenv("USE_ZEND_ALLOC");

	if (tmp && !zend_atoi(tmp, 0)) {
		alloc_globals->mm_heap = malloc(sizeof(zend_mm_heap));
		memset(alloc_globals->mm_heap, 0, sizeof(zend_mm_heap));
		alloc_globals->mm_heap->use_custom_heap = 1;
		alloc_globals->mm_heap->_malloc = malloc;
		alloc_globals->mm_heap->_free = free;
		alloc_globals->mm_heap->_realloc = realloc;
		return;
	}
#endif
	alloc_globals->mm_heap = zend_mm_init();
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
	AG(mm_heap) = (zend_mm_heap*)new_heap;
	return (zend_mm_heap*)old_heap;
}

ZEND_API zend_mm_storage *zend_mm_get_storage(zend_mm_heap *heap)
{
//	return heap->storage;???
	return NULL;
}

ZEND_API void zend_mm_set_custom_handlers(zend_mm_heap *heap,
                                          void* (*_malloc)(size_t),
                                          void  (*_free)(void*),
                                          void* (*_realloc)(void*, size_t))
{
#if ZEND_MM_CUSTOM
	zend_mm_heap *_heap = (zend_mm_heap*)heap;
	
	_heap->use_custom_heap = 1;
	_heap->_malloc = _malloc;
	_heap->_free = _free;
	_heap->_realloc = _realloc;
#endif
}

#if ZEND_DEBUG
ZEND_API int _mem_block_check(void *ptr, int silent ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
//	TSRMLS_FETCH();
//
//	if (!AG(mm_heap)->use_zend_alloc) {
		return 1;
//	}
//	return zend_mm_check_ptr(AG(mm_heap), ptr, silent ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
}


ZEND_API void _full_mem_check(int silent ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
//	int errors;
//	TSRMLS_FETCH();
//
//	if (!AG(mm_heap)->use_zend_alloc) {
//		return;
//	}
//
//	zend_debug_alloc_output("------------------------------------------------\n");
//	zend_debug_alloc_output("Full Memory Check at %s:%d\n" ZEND_FILE_LINE_RELAY_CC);
//
//	errors = zend_mm_check_heap(AG(mm_heap), silent ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
//
//	zend_debug_alloc_output("End of full memory check %s:%d (%d errors)\n" ZEND_FILE_LINE_RELAY_CC, errors);
//	zend_debug_alloc_output("------------------------------------------------\n");
}
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
