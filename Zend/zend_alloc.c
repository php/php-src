/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) Zend Technologies Ltd. (http://www.zend.com)           |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <zeev@php.net>                                 |
   |          Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

/*
 * zend_alloc is designed to be a modern CPU cache friendly memory manager
 * for PHP. Most ideas are taken from jemalloc and tcmalloc implementations.
 *
 * All allocations are split into 3 categories:
 *
 * Huge  - the size is greater than CHUNK size (~2M by default), allocation is
 *         performed using mmap(). The result is aligned on 2M boundary.
 *
 * Large - a number of 4096K pages inside a CHUNK. Large blocks
 *         are always aligned on page boundary.
 *
 * Small - less than 3/4 of page size. Small sizes are rounded up to nearest
 *         greater predefined small size (there are 30 predefined sizes:
 *         8, 16, 24, 32, ... 3072). Small blocks are allocated from
 *         RUNs. Each RUN is allocated as a single or few following pages.
 *         Allocation inside RUNs implemented using linked list of free
 *         elements. The result is aligned to 8 bytes.
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
#include "zend_multiply.h"
#include "zend_bitset.h"
#include "zend_mmap.h"
#include <signal.h>

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#ifdef ZEND_WIN32
# include <wincrypt.h>
# include <process.h>
# include "win32/winutil.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <fcntl.h>
#include <errno.h>
#ifdef __SANITIZE_ADDRESS__
# include <sanitizer/asan_interface.h>
#endif

#ifndef _WIN32
# include <sys/mman.h>
# ifndef MAP_ANON
#  ifdef MAP_ANONYMOUS
#   define MAP_ANON MAP_ANONYMOUS
#  endif
# endif
# ifndef MAP_FAILED
#  define MAP_FAILED ((void*)-1)
# endif
# ifndef MAP_POPULATE
#  define MAP_POPULATE 0
# endif
#  if defined(_SC_PAGESIZE) || (_SC_PAGE_SIZE)
#    define REAL_PAGE_SIZE _real_page_size
static size_t _real_page_size = ZEND_MM_PAGE_SIZE;
#  endif
# ifdef MAP_ALIGNED_SUPER
#    define MAP_HUGETLB MAP_ALIGNED_SUPER
# endif
#endif

#ifndef REAL_PAGE_SIZE
# define REAL_PAGE_SIZE ZEND_MM_PAGE_SIZE
#endif

/* NetBSD has an mremap() function with a signature that is incompatible with Linux (WTF?),
 * so pretend it doesn't exist. */
#ifndef __linux__
# undef HAVE_MREMAP
#endif

#ifndef __APPLE__
# define ZEND_MM_FD -1
#else
# include <mach/vm_statistics.h>
/* Mac allows to track anonymous page via vmmap per TAG id.
 * user land applications are allowed to take from 240 to 255.
 */
# define ZEND_MM_FD VM_MAKE_TAG(250U)
#endif

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
#ifndef ZEND_MM_STORAGE
# define ZEND_MM_STORAGE 1 /* support for custom memory storage              */
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

typedef uint32_t   zend_mm_page_info; /* 4-byte integer */
typedef zend_ulong zend_mm_bitset;    /* 4-byte or 8-byte integer */

#define ZEND_MM_ALIGNED_OFFSET(size, alignment) \
	(((size_t)(size)) & ((alignment) - 1))
#define ZEND_MM_ALIGNED_BASE(size, alignment) \
	(((size_t)(size)) & ~((alignment) - 1))
#define ZEND_MM_SIZE_TO_NUM(size, alignment) \
	(((size_t)(size) + ((alignment) - 1)) / (alignment))

#define ZEND_MM_BITSET_LEN		(sizeof(zend_mm_bitset) * 8)       /* 32 or 64 */
#define ZEND_MM_PAGE_MAP_LEN	(ZEND_MM_PAGES / ZEND_MM_BITSET_LEN) /* 16 or 8 */

typedef zend_mm_bitset zend_mm_page_map[ZEND_MM_PAGE_MAP_LEN];     /* 64B */

#define ZEND_MM_IS_FRUN                  0x00000000
#define ZEND_MM_IS_LRUN                  0x40000000
#define ZEND_MM_IS_SRUN                  0x80000000

#define ZEND_MM_LRUN_PAGES_MASK          0x000003ff
#define ZEND_MM_LRUN_PAGES_OFFSET        0

#define ZEND_MM_SRUN_BIN_NUM_MASK        0x0000001f
#define ZEND_MM_SRUN_BIN_NUM_OFFSET      0

#define ZEND_MM_SRUN_FREE_COUNTER_MASK   0x01ff0000
#define ZEND_MM_SRUN_FREE_COUNTER_OFFSET 16

#define ZEND_MM_NRUN_OFFSET_MASK         0x01ff0000
#define ZEND_MM_NRUN_OFFSET_OFFSET       16

#define ZEND_MM_LRUN_PAGES(info)         (((info) & ZEND_MM_LRUN_PAGES_MASK) >> ZEND_MM_LRUN_PAGES_OFFSET)
#define ZEND_MM_SRUN_BIN_NUM(info)       (((info) & ZEND_MM_SRUN_BIN_NUM_MASK) >> ZEND_MM_SRUN_BIN_NUM_OFFSET)
#define ZEND_MM_SRUN_FREE_COUNTER(info)  (((info) & ZEND_MM_SRUN_FREE_COUNTER_MASK) >> ZEND_MM_SRUN_FREE_COUNTER_OFFSET)
#define ZEND_MM_NRUN_OFFSET(info)        (((info) & ZEND_MM_NRUN_OFFSET_MASK) >> ZEND_MM_NRUN_OFFSET_OFFSET)

#define ZEND_MM_FRUN()                   ZEND_MM_IS_FRUN
#define ZEND_MM_LRUN(count)              (ZEND_MM_IS_LRUN | ((count) << ZEND_MM_LRUN_PAGES_OFFSET))
#define ZEND_MM_SRUN(bin_num)            (ZEND_MM_IS_SRUN | ((bin_num) << ZEND_MM_SRUN_BIN_NUM_OFFSET))
#define ZEND_MM_SRUN_EX(bin_num, count)  (ZEND_MM_IS_SRUN | ((bin_num) << ZEND_MM_SRUN_BIN_NUM_OFFSET) | ((count) << ZEND_MM_SRUN_FREE_COUNTER_OFFSET))
#define ZEND_MM_NRUN(bin_num, offset)    (ZEND_MM_IS_SRUN | ZEND_MM_IS_LRUN | ((bin_num) << ZEND_MM_SRUN_BIN_NUM_OFFSET) | ((offset) << ZEND_MM_NRUN_OFFSET_OFFSET))

#define ZEND_MM_BINS 30

typedef struct  _zend_mm_page      zend_mm_page;
typedef struct  _zend_mm_bin       zend_mm_bin;
typedef struct  _zend_mm_free_slot zend_mm_free_slot;
typedef struct  _zend_mm_chunk     zend_mm_chunk;
typedef struct  _zend_mm_huge_list zend_mm_huge_list;

static bool zend_mm_use_huge_pages = false;

/*
 * Memory is retrieved from OS by chunks of fixed size 2MB.
 * Inside chunk it's managed by pages of fixed size 4096B.
 * So each chunk consists from 512 pages.
 * The first page of each chunk is reserved for chunk header.
 * It contains service information about all pages.
 *
 * free_pages - current number of free pages in this chunk
 *
 * free_tail  - number of continuous free pages at the end of chunk
 *
 * free_map   - bitset (a bit for each page). The bit is set if the corresponding
 *              page is allocated. Allocator for "large sizes" may easily find a
 *              free page (or a continuous number of pages) searching for zero
 *              bits.
 *
 * map        - contains service information for each page. (32-bits for each
 *              page).
 *    usage:
 *				(2 bits)
 * 				FRUN - free page,
 *              LRUN - first page of "large" allocation
 *              SRUN - first page of a bin used for "small" allocation
 *
 *    lrun_pages:
 *              (10 bits) number of allocated pages
 *
 *    srun_bin_num:
 *              (5 bits) bin number (e.g. 0 for sizes 0-2, 1 for 3-4,
 *               2 for 5-8, 3 for 9-16 etc) see zend_alloc_sizes.h
 */

struct _zend_mm_heap {
#if ZEND_MM_CUSTOM
	int                use_custom_heap;
#endif
#if ZEND_MM_STORAGE
	zend_mm_storage   *storage;
#endif
#if ZEND_MM_STAT
	size_t             size;                    /* current memory usage */
	size_t             peak;                    /* peak memory usage */
#endif
	zend_mm_free_slot *free_slot[ZEND_MM_BINS]; /* free lists for small sizes */
#if ZEND_MM_STAT || ZEND_MM_LIMIT
	size_t             real_size;               /* current size of allocated pages */
#endif
#if ZEND_MM_STAT
	size_t             real_peak;               /* peak size of allocated pages */
#endif
#if ZEND_MM_LIMIT
	size_t             limit;                   /* memory limit */
	int                overflow;                /* memory overflow flag */
#endif

	zend_mm_huge_list *huge_list;               /* list of huge allocated blocks */

	zend_mm_chunk     *main_chunk;
	zend_mm_chunk     *cached_chunks;			/* list of unused chunks */
	int                chunks_count;			/* number of allocated chunks */
	int                peak_chunks_count;		/* peak number of allocated chunks for current request */
	int                cached_chunks_count;		/* number of cached chunks */
	double             avg_chunks_count;		/* average number of chunks allocated per request */
	int                last_chunks_delete_boundary; /* number of chunks after last deletion */
	int                last_chunks_delete_count;    /* number of deletion over the last boundary */
#if ZEND_MM_CUSTOM
	union {
		struct {
			void      *(*_malloc)(size_t);
			void       (*_free)(void*);
			void      *(*_realloc)(void*, size_t);
		} std;
		struct {
			void      *(*_malloc)(size_t ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC);
			void       (*_free)(void*  ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC);
			void      *(*_realloc)(void*, size_t  ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC);
		} debug;
	} custom_heap;
	HashTable *tracked_allocs;
#endif
};

struct _zend_mm_chunk {
	zend_mm_heap      *heap;
	zend_mm_chunk     *next;
	zend_mm_chunk     *prev;
	uint32_t           free_pages;				/* number of free pages */
	uint32_t           free_tail;               /* number of free pages at the end of chunk */
	uint32_t           num;
	char               reserve[64 - (sizeof(void*) * 3 + sizeof(uint32_t) * 3)];
	zend_mm_heap       heap_slot;               /* used only in main chunk */
	zend_mm_page_map   free_map;                /* 512 bits or 64 bytes */
	zend_mm_page_info  map[ZEND_MM_PAGES];      /* 2 KB = 512 * 4 */
};

struct _zend_mm_page {
	char               bytes[ZEND_MM_PAGE_SIZE];
};

/*
 * bin - is one or few continuous pages (up to 8) used for allocation of
 * a particular "small size".
 */
struct _zend_mm_bin {
	char               bytes[ZEND_MM_PAGE_SIZE * 8];
};

struct _zend_mm_free_slot {
	zend_mm_free_slot *next_free_slot;
};

struct _zend_mm_huge_list {
	void              *ptr;
	size_t             size;
	zend_mm_huge_list *next;
#if ZEND_DEBUG
	zend_mm_debug_info dbg;
#endif
};

#define ZEND_MM_PAGE_ADDR(chunk, page_num) \
	((void*)(((zend_mm_page*)(chunk)) + (page_num)))

#define _BIN_DATA_SIZE(num, size, elements, pages, x, y) size,
static const uint32_t bin_data_size[] = {
	ZEND_MM_BINS_INFO(_BIN_DATA_SIZE, x, y)
};

#define _BIN_DATA_ELEMENTS(num, size, elements, pages, x, y) elements,
static const uint32_t bin_elements[] = {
	ZEND_MM_BINS_INFO(_BIN_DATA_ELEMENTS, x, y)
};

#define _BIN_DATA_PAGES(num, size, elements, pages, x, y) pages,
static const uint32_t bin_pages[] = {
	ZEND_MM_BINS_INFO(_BIN_DATA_PAGES, x, y)
};

#if ZEND_DEBUG
ZEND_COLD void zend_debug_alloc_output(char *format, ...)
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

static ZEND_COLD ZEND_NORETURN void zend_mm_panic(const char *message)
{
	fprintf(stderr, "%s\n", message);
/* See http://support.microsoft.com/kb/190351 */
#ifdef ZEND_WIN32
	fflush(stderr);
#endif
#if ZEND_DEBUG && defined(HAVE_KILL) && defined(HAVE_GETPID)
	kill(getpid(), SIGSEGV);
#endif
	abort();
}

static ZEND_COLD ZEND_NORETURN void zend_mm_safe_error(zend_mm_heap *heap,
	const char *format,
	size_t limit,
#if ZEND_DEBUG
	const char *filename,
	uint32_t lineno,
#endif
	size_t size)
{

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

#ifdef _WIN32
static void stderr_last_error(char *msg)
{
	DWORD err = GetLastError();
	char *buf = php_win32_error_to_msg(err);

	if (!buf[0]) {
		fprintf(stderr, "\n%s: [0x%08lx]\n", msg, err);
	}
	else {
		fprintf(stderr, "\n%s: [0x%08lx] %s\n", msg, err, buf);
	}

	php_win32_error_msg_free(buf);
}
#endif

/*****************/
/* OS Allocation */
/*****************/

static void zend_mm_munmap(void *addr, size_t size)
{
#ifdef _WIN32
	if (VirtualFree(addr, 0, MEM_RELEASE) == 0) {
		/** ERROR_INVALID_ADDRESS is expected when addr is not range start address */
		if (GetLastError() != ERROR_INVALID_ADDRESS) {
#if ZEND_MM_ERROR
			stderr_last_error("VirtualFree() failed");
#endif
			return;
		}
		SetLastError(0);

		MEMORY_BASIC_INFORMATION mbi;
		if (VirtualQuery(addr, &mbi, sizeof(mbi)) == 0) {
#if ZEND_MM_ERROR
			stderr_last_error("VirtualQuery() failed");
#endif
			return;
		}
		addr = mbi.AllocationBase;

		if (VirtualFree(addr, 0, MEM_RELEASE) == 0) {
#if ZEND_MM_ERROR
			stderr_last_error("VirtualFree() failed");
#endif
		}
	}
#else
	if (munmap(addr, size) != 0) {
#if ZEND_MM_ERROR
		fprintf(stderr, "\nmunmap() failed: [%d] %s\n", errno, strerror(errno));
#endif
	}
#endif
}

#ifndef HAVE_MREMAP
static void *zend_mm_mmap_fixed(void *addr, size_t size)
{
#ifdef _WIN32
	void *ptr = VirtualAlloc(addr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	if (ptr == NULL) {
		/** ERROR_INVALID_ADDRESS is expected when fixed addr range is not free */
		if (GetLastError() != ERROR_INVALID_ADDRESS) {
#if ZEND_MM_ERROR
			stderr_last_error("VirtualAlloc() fixed failed");
#endif
		}
		SetLastError(0);
		return NULL;
	}
	ZEND_ASSERT(ptr == addr);
	return ptr;
#else
	int flags = MAP_PRIVATE | MAP_ANON;
#if defined(MAP_EXCL)
	flags |= MAP_FIXED | MAP_EXCL;
#elif defined(MAP_TRYFIXED)
	flags |= MAP_TRYFIXED;
#endif
	/* MAP_FIXED leads to discarding of the old mapping, so it can't be used. */
	void *ptr = mmap(addr, size, PROT_READ | PROT_WRITE, flags /*| MAP_POPULATE | MAP_HUGETLB*/, ZEND_MM_FD, 0);

	if (ptr == MAP_FAILED) {
#if ZEND_MM_ERROR && !defined(MAP_EXCL) && !defined(MAP_TRYFIXED)
		fprintf(stderr, "\nmmap() fixed failed: [%d] %s\n", errno, strerror(errno));
#endif
		return NULL;
	} else if (ptr != addr) {
		zend_mm_munmap(ptr, size);
		return NULL;
	}
	return ptr;
#endif
}
#endif

static void *zend_mm_mmap(size_t size)
{
#ifdef _WIN32
	void *ptr = VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	if (ptr == NULL) {
#if ZEND_MM_ERROR
		stderr_last_error("VirtualAlloc() failed");
#endif
		return NULL;
	}
	return ptr;
#else
	void *ptr;

#if defined(MAP_HUGETLB) || defined(VM_FLAGS_SUPERPAGE_SIZE_2MB)
	if (zend_mm_use_huge_pages && size == ZEND_MM_CHUNK_SIZE) {
		int fd = -1;
		int mflags = MAP_PRIVATE | MAP_ANON;
#if defined(MAP_HUGETLB)
		mflags |= MAP_HUGETLB;
#else
		fd = VM_FLAGS_SUPERPAGE_SIZE_2MB;
#endif
		ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, mflags, fd, 0);
		if (ptr != MAP_FAILED) {
			zend_mmap_set_name(ptr, size, "zend_alloc");
			return ptr;
		}
	}
#endif

	ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, ZEND_MM_FD, 0);

	if (ptr == MAP_FAILED) {
#if ZEND_MM_ERROR
		fprintf(stderr, "\nmmap() failed: [%d] %s\n", errno, strerror(errno));
#endif
		return NULL;
	}
	zend_mmap_set_name(ptr, size, "zend_alloc");
	return ptr;
#endif
}

/***********/
/* Bitmask */
/***********/

/* number of trailing set (1) bits */
static zend_always_inline int zend_mm_bitset_nts(zend_mm_bitset bitset)
{
#if (defined(__GNUC__) || __has_builtin(__builtin_ctzl)) && SIZEOF_ZEND_LONG == SIZEOF_LONG && defined(PHP_HAVE_BUILTIN_CTZL)
	return __builtin_ctzl(~bitset);
#elif (defined(__GNUC__) || __has_builtin(__builtin_ctzll)) && defined(PHP_HAVE_BUILTIN_CTZLL)
	return __builtin_ctzll(~bitset);
#elif defined(_WIN32)
	unsigned long index;

#if defined(_WIN64)
	if (!BitScanForward64(&index, ~bitset)) {
#else
	if (!BitScanForward(&index, ~bitset)) {
#endif
		/* undefined behavior */
		return 32;
	}

	return (int)index;
#else
	int n;

	if (bitset == (zend_mm_bitset)-1) return ZEND_MM_BITSET_LEN;

	n = 0;
#if SIZEOF_ZEND_LONG == 8
	if (sizeof(zend_mm_bitset) == 8) {
		if ((bitset & 0xffffffff) == 0xffffffff) {n += 32; bitset = bitset >> Z_UL(32);}
	}
#endif
	if ((bitset & 0x0000ffff) == 0x0000ffff) {n += 16; bitset = bitset >> 16;}
	if ((bitset & 0x000000ff) == 0x000000ff) {n +=  8; bitset = bitset >>  8;}
	if ((bitset & 0x0000000f) == 0x0000000f) {n +=  4; bitset = bitset >>  4;}
	if ((bitset & 0x00000003) == 0x00000003) {n +=  2; bitset = bitset >>  2;}
	return n + (bitset & 1);
#endif
}

static zend_always_inline int zend_mm_bitset_is_set(zend_mm_bitset *bitset, int bit)
{
	return ZEND_BIT_TEST(bitset, bit);
}

static zend_always_inline void zend_mm_bitset_set_bit(zend_mm_bitset *bitset, int bit)
{
	bitset[bit / ZEND_MM_BITSET_LEN] |= (Z_UL(1) << (bit & (ZEND_MM_BITSET_LEN-1)));
}

static zend_always_inline void zend_mm_bitset_reset_bit(zend_mm_bitset *bitset, int bit)
{
	bitset[bit / ZEND_MM_BITSET_LEN] &= ~(Z_UL(1) << (bit & (ZEND_MM_BITSET_LEN-1)));
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
			tmp = ~((Z_UL(1) << bit) - 1);
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

static zend_always_inline void zend_mm_hugepage(void* ptr, size_t size)
{
#if defined(MADV_HUGEPAGE)
	(void)madvise(ptr, size, MADV_HUGEPAGE);
#elif defined(HAVE_MEMCNTL)
	struct memcntl_mha m = {.mha_cmd = MHA_MAPSIZE_VA, .mha_pagesize = ZEND_MM_CHUNK_SIZE, .mha_flags = 0};
	(void)memcntl(ptr, size, MC_HAT_ADVISE, (char *)&m, 0, 0);
#elif !defined(VM_FLAGS_SUPERPAGE_SIZE_2MB) && !defined(MAP_ALIGNED_SUPER)
	zend_error_noreturn(E_ERROR, "huge_pages: thp unsupported on this platform");
#endif
}

static void *zend_mm_chunk_alloc_int(size_t size, size_t alignment)
{
	void *ptr = zend_mm_mmap(size);

	if (ptr == NULL) {
		return NULL;
	} else if (ZEND_MM_ALIGNED_OFFSET(ptr, alignment) == 0) {
		if (zend_mm_use_huge_pages) {
			zend_mm_hugepage(ptr, size);
		}
#ifdef __SANITIZE_ADDRESS__
		ASAN_UNPOISON_MEMORY_REGION(ptr, size);
#endif
		return ptr;
	} else {
		size_t offset;

		/* chunk has to be aligned */
		zend_mm_munmap(ptr, size);
		ptr = zend_mm_mmap(size + alignment - REAL_PAGE_SIZE);
#ifdef _WIN32
		offset = ZEND_MM_ALIGNED_OFFSET(ptr, alignment);
		if (offset != 0) {
			offset = alignment - offset;
		}
		zend_mm_munmap(ptr, size + alignment - REAL_PAGE_SIZE);
		ptr = zend_mm_mmap_fixed((void*)((char*)ptr + offset), size);
		if (ptr == NULL) { // fix GH-9650, fixed addr range is not free
			ptr = zend_mm_mmap(size + alignment - REAL_PAGE_SIZE);
			if (ptr == NULL) {
				return NULL;
			}
			offset = ZEND_MM_ALIGNED_OFFSET(ptr, alignment);
			if (offset != 0) {
				ptr = (void*)((char*)ptr + alignment - offset);
			}
		}
		return ptr;
#else
		offset = ZEND_MM_ALIGNED_OFFSET(ptr, alignment);
		if (offset != 0) {
			offset = alignment - offset;
			zend_mm_munmap(ptr, offset);
			ptr = (char*)ptr + offset;
			alignment -= offset;
		}
		if (alignment > REAL_PAGE_SIZE) {
			zend_mm_munmap((char*)ptr + size, alignment - REAL_PAGE_SIZE);
		}
		if (zend_mm_use_huge_pages) {
			zend_mm_hugepage(ptr, size);
		}
# ifdef __SANITIZE_ADDRESS__
		ASAN_UNPOISON_MEMORY_REGION(ptr, size);
# endif
#endif
		return ptr;
	}
}

static void *zend_mm_chunk_alloc(zend_mm_heap *heap, size_t size, size_t alignment)
{
#if ZEND_MM_STORAGE
	if (UNEXPECTED(heap->storage)) {
		void *ptr = heap->storage->handlers.chunk_alloc(heap->storage, size, alignment);
		ZEND_ASSERT(((uintptr_t)((char*)ptr + (alignment-1)) & (alignment-1)) == (uintptr_t)ptr);
		return ptr;
	}
#endif
	return zend_mm_chunk_alloc_int(size, alignment);
}

static void zend_mm_chunk_free(zend_mm_heap *heap, void *addr, size_t size)
{
#if ZEND_MM_STORAGE
	if (UNEXPECTED(heap->storage)) {
		heap->storage->handlers.chunk_free(heap->storage, addr, size);
		return;
	}
#endif
	zend_mm_munmap(addr, size);
}

static int zend_mm_chunk_truncate(zend_mm_heap *heap, void *addr, size_t old_size, size_t new_size)
{
#if ZEND_MM_STORAGE
	if (UNEXPECTED(heap->storage)) {
		if (heap->storage->handlers.chunk_truncate) {
			return heap->storage->handlers.chunk_truncate(heap->storage, addr, old_size, new_size);
		} else {
			return 0;
		}
	}
#endif
#ifndef _WIN32
	zend_mm_munmap((char*)addr + new_size, old_size - new_size);
	return 1;
#else
	return 0;
#endif
}

static int zend_mm_chunk_extend(zend_mm_heap *heap, void *addr, size_t old_size, size_t new_size)
{
#if ZEND_MM_STORAGE
	if (UNEXPECTED(heap->storage)) {
		if (heap->storage->handlers.chunk_extend) {
			return heap->storage->handlers.chunk_extend(heap->storage, addr, old_size, new_size);
		} else {
			return 0;
		}
	}
#endif
#ifdef HAVE_MREMAP
	/* We don't use MREMAP_MAYMOVE due to alignment requirements. */
	void *ptr = mremap(addr, old_size, new_size, 0);
	if (ptr == MAP_FAILED) {
		return 0;
	}
	/* Sanity check: The mapping shouldn't have moved. */
	ZEND_ASSERT(ptr == addr);
	return 1;
#elif !defined(_WIN32)
	return (zend_mm_mmap_fixed((char*)addr + old_size, new_size - old_size) != NULL);
#else
	return 0;
#endif
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
static void *zend_mm_alloc_pages(zend_mm_heap *heap, uint32_t pages_count, size_t size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
#else
static void *zend_mm_alloc_pages(zend_mm_heap *heap, uint32_t pages_count ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
#endif
{
	zend_mm_chunk *chunk = heap->main_chunk;
	uint32_t page_num, len;
	int steps = 0;

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
				len = (i + zend_ulong_ntz(tmp)) - page_num;
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
			uint32_t best_len = ZEND_MM_PAGES;
			uint32_t free_tail = chunk->free_tail;
			zend_mm_bitset *bitset = chunk->free_map;
			zend_mm_bitset tmp = *(bitset++);
			uint32_t i = 0;

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
					if (i >= free_tail || i == ZEND_MM_PAGES) {
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
				len = i + zend_ulong_ntz(tmp) - page_num;
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
get_chunk:
			if (heap->cached_chunks) {
				heap->cached_chunks_count--;
				chunk = heap->cached_chunks;
				heap->cached_chunks = chunk->next;
			} else {
#if ZEND_MM_LIMIT
				if (UNEXPECTED(ZEND_MM_CHUNK_SIZE > heap->limit - heap->real_size)) {
					if (zend_mm_gc(heap)) {
						goto get_chunk;
					} else if (heap->overflow == 0) {
#if ZEND_DEBUG
						zend_mm_safe_error(heap, "Allowed memory size of %zu bytes exhausted at %s:%d (tried to allocate %zu bytes)", heap->limit, __zend_filename, __zend_lineno, size);
#else
						zend_mm_safe_error(heap, "Allowed memory size of %zu bytes exhausted (tried to allocate %zu bytes)", heap->limit, ZEND_MM_PAGE_SIZE * pages_count);
#endif
						return NULL;
					}
				}
#endif
				chunk = (zend_mm_chunk*)zend_mm_chunk_alloc(heap, ZEND_MM_CHUNK_SIZE, ZEND_MM_CHUNK_SIZE);
				if (UNEXPECTED(chunk == NULL)) {
					/* insufficient memory */
					if (zend_mm_gc(heap) &&
					    (chunk = (zend_mm_chunk*)zend_mm_chunk_alloc(heap, ZEND_MM_CHUNK_SIZE, ZEND_MM_CHUNK_SIZE)) != NULL) {
						/* pass */
					} else {
#if !ZEND_MM_LIMIT
						zend_mm_safe_error(heap, "Out of memory");
#elif ZEND_DEBUG
						zend_mm_safe_error(heap, "Out of memory (allocated %zu bytes) at %s:%d (tried to allocate %zu bytes)", heap->real_size, __zend_filename, __zend_lineno, size);
#else
						zend_mm_safe_error(heap, "Out of memory (allocated %zu bytes) (tried to allocate %zu bytes)", heap->real_size, ZEND_MM_PAGE_SIZE * pages_count);
#endif
						return NULL;
					}
				}
#if ZEND_MM_STAT
				do {
					size_t size = heap->real_size + ZEND_MM_CHUNK_SIZE;
					size_t peak = MAX(heap->real_peak, size);
					heap->real_size = size;
					heap->real_peak = peak;
				} while (0);
#elif ZEND_MM_LIMIT
				heap->real_size += ZEND_MM_CHUNK_SIZE;

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
			steps++;
		}
	}

found:
	if (steps > 2 && pages_count < 8) {
		/* move chunk into the head of the linked-list */
		chunk->prev->next = chunk->next;
		chunk->next->prev = chunk->prev;
		chunk->next = heap->main_chunk->next;
		chunk->prev = heap->main_chunk;
		chunk->prev->next = chunk;
		chunk->next->prev = chunk;
	}
	/* mark run as allocated */
	chunk->free_pages -= pages_count;
	zend_mm_bitset_set_range(chunk->free_map, page_num, pages_count);
	chunk->map[page_num] = ZEND_MM_LRUN(pages_count);
	if (page_num == chunk->free_tail) {
		chunk->free_tail = page_num + pages_count;
	}
	return ZEND_MM_PAGE_ADDR(chunk, page_num);
}

static zend_always_inline void *zend_mm_alloc_large_ex(zend_mm_heap *heap, size_t size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	int pages_count = (int)ZEND_MM_SIZE_TO_NUM(size, ZEND_MM_PAGE_SIZE);
#if ZEND_DEBUG
	void *ptr = zend_mm_alloc_pages(heap, pages_count, size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
#else
	void *ptr = zend_mm_alloc_pages(heap, pages_count ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
#endif
#if ZEND_MM_STAT
	do {
		size_t size = heap->size + pages_count * ZEND_MM_PAGE_SIZE;
		size_t peak = MAX(heap->peak, size);
		heap->size = size;
		heap->peak = peak;
	} while (0);
#endif
	return ptr;
}

#if ZEND_DEBUG
static zend_never_inline void *zend_mm_alloc_large(zend_mm_heap *heap, size_t size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	return zend_mm_alloc_large_ex(heap, size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
}
#else
static zend_never_inline void *zend_mm_alloc_large(zend_mm_heap *heap, size_t size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	return zend_mm_alloc_large_ex(heap, size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
}
#endif

static zend_always_inline void zend_mm_delete_chunk(zend_mm_heap *heap, zend_mm_chunk *chunk)
{
	chunk->next->prev = chunk->prev;
	chunk->prev->next = chunk->next;
	heap->chunks_count--;
	if (heap->chunks_count + heap->cached_chunks_count < heap->avg_chunks_count + 0.1
	 || (heap->chunks_count == heap->last_chunks_delete_boundary
	  && heap->last_chunks_delete_count >= 4)) {
		/* delay deletion */
		heap->cached_chunks_count++;
		chunk->next = heap->cached_chunks;
		heap->cached_chunks = chunk;
	} else {
#if ZEND_MM_STAT || ZEND_MM_LIMIT
		heap->real_size -= ZEND_MM_CHUNK_SIZE;
#endif
		if (!heap->cached_chunks) {
			if (heap->chunks_count != heap->last_chunks_delete_boundary) {
				heap->last_chunks_delete_boundary = heap->chunks_count;
				heap->last_chunks_delete_count = 0;
			} else {
				heap->last_chunks_delete_count++;
			}
		}
		if (!heap->cached_chunks || chunk->num > heap->cached_chunks->num) {
			zend_mm_chunk_free(heap, chunk, ZEND_MM_CHUNK_SIZE);
		} else {
//TODO: select the best chunk to delete???
			chunk->next = heap->cached_chunks->next;
			zend_mm_chunk_free(heap, heap->cached_chunks, ZEND_MM_CHUNK_SIZE);
			heap->cached_chunks = chunk;
		}
	}
}

static zend_always_inline void zend_mm_free_pages_ex(zend_mm_heap *heap, zend_mm_chunk *chunk, uint32_t page_num, uint32_t pages_count, int free_chunk)
{
	chunk->free_pages += pages_count;
	zend_mm_bitset_reset_range(chunk->free_map, page_num, pages_count);
	chunk->map[page_num] = 0;
	if (chunk->free_tail == page_num + pages_count) {
		/* this setting may be not accurate */
		chunk->free_tail = page_num;
	}
	if (free_chunk && chunk != heap->main_chunk && chunk->free_pages == ZEND_MM_PAGES - ZEND_MM_FIRST_PAGE) {
		zend_mm_delete_chunk(heap, chunk);
	}
}

static zend_never_inline void zend_mm_free_pages(zend_mm_heap *heap, zend_mm_chunk *chunk, int page_num, int pages_count)
{
	zend_mm_free_pages_ex(heap, chunk, page_num, pages_count, 1);
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

/* higher set bit number (0->N/A, 1->1, 2->2, 4->3, 8->4, 127->7, 128->8 etc) */
static zend_always_inline int zend_mm_small_size_to_bit(int size)
{
#if (defined(__GNUC__) || __has_builtin(__builtin_clz))  && defined(PHP_HAVE_BUILTIN_CLZ)
	return (__builtin_clz(size) ^ 0x1f) + 1;
#elif defined(_WIN32)
	unsigned long index;

	if (!BitScanReverse(&index, (unsigned long)size)) {
		/* undefined behavior */
		return 64;
	}

	return (((31 - (int)index) ^ 0x1f) + 1);
#else
	int n = 16;
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

static zend_always_inline int zend_mm_small_size_to_bin(size_t size)
{
#if 0
	int n;
                            /*0,  1,  2,  3,  4,  5,  6,  7,  8,  9  10, 11, 12*/
	static const int f1[] = { 3,  3,  3,  3,  3,  3,  3,  4,  5,  6,  7,  8,  9};
	static const int f2[] = { 0,  0,  0,  0,  0,  0,  0,  4,  8, 12, 16, 20, 24};

	if (UNEXPECTED(size <= 2)) return 0;
	n = zend_mm_small_size_to_bit(size - 1);
	return ((size-1) >> f1[n]) + f2[n];
#else
	unsigned int t1, t2;

	if (size <= 64) {
		/* we need to support size == 0 ... */
		return (size - !!size) >> 3;
	} else {
		t1 = size - 1;
		t2 = zend_mm_small_size_to_bit(t1) - 3;
		t1 = t1 >> t2;
		t2 = t2 - 3;
		t2 = t2 << 2;
		return (int)(t1 + t2);
	}
#endif
}

#define ZEND_MM_SMALL_SIZE_TO_BIN(size)  zend_mm_small_size_to_bin(size)

static zend_never_inline void *zend_mm_alloc_small_slow(zend_mm_heap *heap, uint32_t bin_num ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	zend_mm_chunk *chunk;
	int page_num;
	zend_mm_bin *bin;
	zend_mm_free_slot *p, *end;

#if ZEND_DEBUG
	bin = (zend_mm_bin*)zend_mm_alloc_pages(heap, bin_pages[bin_num], bin_data_size[bin_num] ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
#else
	bin = (zend_mm_bin*)zend_mm_alloc_pages(heap, bin_pages[bin_num] ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
#endif
	if (UNEXPECTED(bin == NULL)) {
		/* insufficient memory */
		return NULL;
	}

	chunk = (zend_mm_chunk*)ZEND_MM_ALIGNED_BASE(bin, ZEND_MM_CHUNK_SIZE);
	page_num = ZEND_MM_ALIGNED_OFFSET(bin, ZEND_MM_CHUNK_SIZE) / ZEND_MM_PAGE_SIZE;
	chunk->map[page_num] = ZEND_MM_SRUN(bin_num);
	if (bin_pages[bin_num] > 1) {
		uint32_t i = 1;

		do {
			chunk->map[page_num+i] = ZEND_MM_NRUN(bin_num, i);
			i++;
		} while (i < bin_pages[bin_num]);
	}

	/* create a linked list of elements from 1 to last */
	end = (zend_mm_free_slot*)((char*)bin + (bin_data_size[bin_num] * (bin_elements[bin_num] - 1)));
	heap->free_slot[bin_num] = p = (zend_mm_free_slot*)((char*)bin + bin_data_size[bin_num]);
	do {
		p->next_free_slot = (zend_mm_free_slot*)((char*)p + bin_data_size[bin_num]);
#if ZEND_DEBUG
		do {
			zend_mm_debug_info *dbg = (zend_mm_debug_info*)((char*)p + bin_data_size[bin_num] - ZEND_MM_ALIGNED_SIZE(sizeof(zend_mm_debug_info)));
			dbg->size = 0;
		} while (0);
#endif
		p = (zend_mm_free_slot*)((char*)p + bin_data_size[bin_num]);
	} while (p != end);

	/* terminate list using NULL */
	p->next_free_slot = NULL;
#if ZEND_DEBUG
		do {
			zend_mm_debug_info *dbg = (zend_mm_debug_info*)((char*)p + bin_data_size[bin_num] - ZEND_MM_ALIGNED_SIZE(sizeof(zend_mm_debug_info)));
			dbg->size = 0;
		} while (0);
#endif

	/* return first element */
	return bin;
}

static zend_always_inline void *zend_mm_alloc_small(zend_mm_heap *heap, int bin_num ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
#if ZEND_MM_STAT
	do {
		size_t size = heap->size + bin_data_size[bin_num];
		size_t peak = MAX(heap->peak, size);
		heap->size = size;
		heap->peak = peak;
	} while (0);
#endif

	if (EXPECTED(heap->free_slot[bin_num] != NULL)) {
		zend_mm_free_slot *p = heap->free_slot[bin_num];
		heap->free_slot[bin_num] = p->next_free_slot;
		return p;
	} else {
		return zend_mm_alloc_small_slow(heap, bin_num ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
	}
}

static zend_always_inline void zend_mm_free_small(zend_mm_heap *heap, void *ptr, int bin_num)
{
	zend_mm_free_slot *p;

#if ZEND_MM_STAT
	heap->size -= bin_data_size[bin_num];
#endif

#if ZEND_DEBUG
	do {
		zend_mm_debug_info *dbg = (zend_mm_debug_info*)((char*)ptr + bin_data_size[bin_num] - ZEND_MM_ALIGNED_SIZE(sizeof(zend_mm_debug_info)));
		dbg->size = 0;
	} while (0);
#endif

	p = (zend_mm_free_slot*)ptr;
	p->next_free_slot = heap->free_slot[bin_num];
	heap->free_slot[bin_num] = p;
}

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
	page_num = (int)(page_offset / ZEND_MM_PAGE_SIZE);
	info = chunk->map[page_num];
	ZEND_MM_CHECK(chunk->heap == heap, "zend_mm_heap corrupted");
	if (EXPECTED(info & ZEND_MM_IS_SRUN)) {
		int bin_num = ZEND_MM_SRUN_BIN_NUM(info);
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

	/* special handling for zero-size allocation */
	size = MAX(size, 1);
	size = ZEND_MM_ALIGNED_SIZE(size) + ZEND_MM_ALIGNED_SIZE(sizeof(zend_mm_debug_info));
	if (UNEXPECTED(size < real_size)) {
		zend_error_noreturn(E_ERROR, "Possible integer overflow in memory allocation (%zu + %zu)", ZEND_MM_ALIGNED_SIZE(real_size), ZEND_MM_ALIGNED_SIZE(sizeof(zend_mm_debug_info)));
		return NULL;
	}
#endif
	if (EXPECTED(size <= ZEND_MM_MAX_SMALL_SIZE)) {
		ptr = zend_mm_alloc_small(heap, ZEND_MM_SMALL_SIZE_TO_BIN(size) ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
#if ZEND_DEBUG
		dbg = zend_mm_get_debug_info(heap, ptr);
		dbg->size = real_size;
		dbg->filename = __zend_filename;
		dbg->orig_filename = __zend_orig_filename;
		dbg->lineno = __zend_lineno;
		dbg->orig_lineno = __zend_orig_lineno;
#endif
		return ptr;
	} else if (EXPECTED(size <= ZEND_MM_MAX_LARGE_SIZE)) {
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
		int page_num = (int)(page_offset / ZEND_MM_PAGE_SIZE);
		zend_mm_page_info info = chunk->map[page_num];

		ZEND_MM_CHECK(chunk->heap == heap, "zend_mm_heap corrupted");
		if (EXPECTED(info & ZEND_MM_IS_SRUN)) {
			zend_mm_free_small(heap, ptr, ZEND_MM_SRUN_BIN_NUM(info));
		} else /* if (info & ZEND_MM_IS_LRUN) */ {
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
		page_num = (int)(page_offset / ZEND_MM_PAGE_SIZE);
		info = chunk->map[page_num];
		ZEND_MM_CHECK(chunk->heap == heap, "zend_mm_heap corrupted");
		if (EXPECTED(info & ZEND_MM_IS_SRUN)) {
			return bin_data_size[ZEND_MM_SRUN_BIN_NUM(info)];
		} else /* if (info & ZEND_MM_IS_LARGE_RUN) */ {
			return ZEND_MM_LRUN_PAGES(info) * ZEND_MM_PAGE_SIZE;
		}
#endif
	}
}

static zend_never_inline void *zend_mm_realloc_slow(zend_mm_heap *heap, void *ptr, size_t size, size_t copy_size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	void *ret;

#if ZEND_MM_STAT
	do {
		size_t orig_peak = heap->peak;
#endif
		ret = zend_mm_alloc_heap(heap, size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
		memcpy(ret, ptr, copy_size);
		zend_mm_free_heap(heap, ptr ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
#if ZEND_MM_STAT
		heap->peak = MAX(orig_peak, heap->size);
	} while (0);
#endif
	return ret;
}

static zend_never_inline void *zend_mm_realloc_huge(zend_mm_heap *heap, void *ptr, size_t size, size_t copy_size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	size_t old_size;
	size_t new_size;
#if ZEND_DEBUG
	size_t real_size;
#endif

	old_size = zend_mm_get_huge_block_size(heap, ptr ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
#if ZEND_DEBUG
	real_size = size;
	size = ZEND_MM_ALIGNED_SIZE(size) + ZEND_MM_ALIGNED_SIZE(sizeof(zend_mm_debug_info));
#endif
	if (size > ZEND_MM_MAX_LARGE_SIZE) {
#if ZEND_DEBUG
		size = real_size;
#endif
#ifdef ZEND_WIN32
		/* On Windows we don't have ability to extend huge blocks in-place.
		 * We allocate them with 2MB size granularity, to avoid many
		 * reallocations when they are extended by small pieces
		 */
		new_size = ZEND_MM_ALIGNED_SIZE_EX(size, MAX(REAL_PAGE_SIZE, ZEND_MM_CHUNK_SIZE));
#else
		new_size = ZEND_MM_ALIGNED_SIZE_EX(size, REAL_PAGE_SIZE);
#endif
		if (new_size == old_size) {
#if ZEND_DEBUG
			zend_mm_change_huge_block_size(heap, ptr, new_size, real_size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
#else
			zend_mm_change_huge_block_size(heap, ptr, new_size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
#endif
			return ptr;
		} else if (new_size < old_size) {
			/* unmup tail */
			if (zend_mm_chunk_truncate(heap, ptr, old_size, new_size)) {
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
			}
		} else /* if (new_size > old_size) */ {
#if ZEND_MM_LIMIT
			if (UNEXPECTED(new_size - old_size > heap->limit - heap->real_size)) {
				if (zend_mm_gc(heap) && new_size - old_size <= heap->limit - heap->real_size) {
					/* pass */
				} else if (heap->overflow == 0) {
#if ZEND_DEBUG
					zend_mm_safe_error(heap, "Allowed memory size of %zu bytes exhausted at %s:%d (tried to allocate %zu bytes)", heap->limit, __zend_filename, __zend_lineno, size);
#else
					zend_mm_safe_error(heap, "Allowed memory size of %zu bytes exhausted (tried to allocate %zu bytes)", heap->limit, size);
#endif
					return NULL;
				}
			}
#endif
			/* try to map tail right after this block */
			if (zend_mm_chunk_extend(heap, ptr, old_size, new_size)) {
#if ZEND_MM_STAT || ZEND_MM_LIMIT
				heap->real_size += new_size - old_size;
#endif
#if ZEND_MM_STAT
				heap->real_peak = MAX(heap->real_peak, heap->real_size);
				heap->size += new_size - old_size;
				heap->peak = MAX(heap->peak, heap->size);
#endif
#if ZEND_DEBUG
				zend_mm_change_huge_block_size(heap, ptr, new_size, real_size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
#else
				zend_mm_change_huge_block_size(heap, ptr, new_size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
#endif
				return ptr;
			}
		}
	}

	return zend_mm_realloc_slow(heap, ptr, size, MIN(old_size, copy_size) ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
}

static zend_always_inline void *zend_mm_realloc_heap(zend_mm_heap *heap, void *ptr, size_t size, bool use_copy_size, size_t copy_size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	size_t page_offset;
	size_t old_size;
	size_t new_size;
	void *ret;
#if ZEND_DEBUG
	zend_mm_debug_info *dbg;
#endif

	page_offset = ZEND_MM_ALIGNED_OFFSET(ptr, ZEND_MM_CHUNK_SIZE);
	if (UNEXPECTED(page_offset == 0)) {
		if (EXPECTED(ptr == NULL)) {
			return _zend_mm_alloc(heap, size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
		} else {
			return zend_mm_realloc_huge(heap, ptr, size, copy_size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
		}
	} else {
		zend_mm_chunk *chunk = (zend_mm_chunk*)ZEND_MM_ALIGNED_BASE(ptr, ZEND_MM_CHUNK_SIZE);
		int page_num = (int)(page_offset / ZEND_MM_PAGE_SIZE);
		zend_mm_page_info info = chunk->map[page_num];
#if ZEND_DEBUG
		size_t real_size = size;

		size = ZEND_MM_ALIGNED_SIZE(size) + ZEND_MM_ALIGNED_SIZE(sizeof(zend_mm_debug_info));
#endif

		ZEND_MM_CHECK(chunk->heap == heap, "zend_mm_heap corrupted");
		if (info & ZEND_MM_IS_SRUN) {
			int old_bin_num = ZEND_MM_SRUN_BIN_NUM(info);

			do {
				old_size = bin_data_size[old_bin_num];

				/* Check if requested size fits into current bin */
				if (size <= old_size) {
					/* Check if truncation is necessary */
					if (old_bin_num > 0 && size < bin_data_size[old_bin_num - 1]) {
						/* truncation */
						ret = zend_mm_alloc_small(heap, ZEND_MM_SMALL_SIZE_TO_BIN(size) ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
						copy_size = use_copy_size ? MIN(size, copy_size) : size;
						memcpy(ret, ptr, copy_size);
						zend_mm_free_small(heap, ptr, old_bin_num);
					} else {
						/* reallocation in-place */
						ret = ptr;
					}
				} else if (size <= ZEND_MM_MAX_SMALL_SIZE) {
					/* small extension */

#if ZEND_MM_STAT
					do {
						size_t orig_peak = heap->peak;
#endif
						ret = zend_mm_alloc_small(heap, ZEND_MM_SMALL_SIZE_TO_BIN(size) ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
						copy_size = use_copy_size ? MIN(old_size, copy_size) : old_size;
						memcpy(ret, ptr, copy_size);
						zend_mm_free_small(heap, ptr, old_bin_num);
#if ZEND_MM_STAT
						heap->peak = MAX(orig_peak, heap->size);
					} while (0);
#endif
				} else {
					/* slow reallocation */
					break;
				}

#if ZEND_DEBUG
				dbg = zend_mm_get_debug_info(heap, ret);
				dbg->size = real_size;
				dbg->filename = __zend_filename;
				dbg->orig_filename = __zend_orig_filename;
				dbg->lineno = __zend_lineno;
				dbg->orig_lineno = __zend_orig_lineno;
#endif
				return ret;
			}  while (0);

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
					int new_pages_count = (int)(new_size / ZEND_MM_PAGE_SIZE);
					int rest_pages_count = (int)((old_size - new_size) / ZEND_MM_PAGE_SIZE);

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
					int new_pages_count = (int)(new_size / ZEND_MM_PAGE_SIZE);
					int old_pages_count = (int)(old_size / ZEND_MM_PAGE_SIZE);

					/* try to allocate tail pages after this block */
					if (page_num + new_pages_count <= ZEND_MM_PAGES &&
					    zend_mm_bitset_is_free_range(chunk->free_map, page_num + old_pages_count, new_pages_count - old_pages_count)) {
#if ZEND_MM_STAT
						do {
							size_t size = heap->size + (new_size - old_size);
							size_t peak = MAX(heap->peak, size);
							heap->size = size;
							heap->peak = peak;
						} while (0);
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

	copy_size = MIN(old_size, copy_size);
	return zend_mm_realloc_slow(heap, ptr, size, copy_size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
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
#ifdef ZEND_WIN32
	/* On Windows we don't have ability to extend huge blocks in-place.
	 * We allocate them with 2MB size granularity, to avoid many
	 * reallocations when they are extended by small pieces
	 */
	size_t alignment = MAX(REAL_PAGE_SIZE, ZEND_MM_CHUNK_SIZE);
#else
	size_t alignment = REAL_PAGE_SIZE;
#endif
	size_t new_size = ZEND_MM_ALIGNED_SIZE_EX(size, alignment);
	void *ptr;

	if (UNEXPECTED(new_size < size)) {
		zend_error_noreturn(E_ERROR, "Possible integer overflow in memory allocation (%zu + %zu)", size, alignment);
	}

#if ZEND_MM_LIMIT
	if (UNEXPECTED(new_size > heap->limit - heap->real_size)) {
		if (zend_mm_gc(heap) && new_size <= heap->limit - heap->real_size) {
			/* pass */
		} else if (heap->overflow == 0) {
#if ZEND_DEBUG
			zend_mm_safe_error(heap, "Allowed memory size of %zu bytes exhausted at %s:%d (tried to allocate %zu bytes)", heap->limit, __zend_filename, __zend_lineno, size);
#else
			zend_mm_safe_error(heap, "Allowed memory size of %zu bytes exhausted (tried to allocate %zu bytes)", heap->limit, size);
#endif
			return NULL;
		}
	}
#endif
	ptr = zend_mm_chunk_alloc(heap, new_size, ZEND_MM_CHUNK_SIZE);
	if (UNEXPECTED(ptr == NULL)) {
		/* insufficient memory */
		if (zend_mm_gc(heap) &&
		    (ptr = zend_mm_chunk_alloc(heap, new_size, ZEND_MM_CHUNK_SIZE)) != NULL) {
			/* pass */
		} else {
#if !ZEND_MM_LIMIT
			zend_mm_safe_error(heap, "Out of memory");
#elif ZEND_DEBUG
			zend_mm_safe_error(heap, "Out of memory (allocated %zu bytes) at %s:%d (tried to allocate %zu bytes)", heap->real_size, __zend_filename, __zend_lineno, size);
#else
			zend_mm_safe_error(heap, "Out of memory (allocated %zu bytes) (tried to allocate %zu bytes)", heap->real_size, size);
#endif
			return NULL;
		}
	}
#if ZEND_DEBUG
	zend_mm_add_huge_block(heap, ptr, new_size, size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
#else
	zend_mm_add_huge_block(heap, ptr, new_size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
#endif
#if ZEND_MM_STAT
	do {
		size_t size = heap->real_size + new_size;
		size_t peak = MAX(heap->real_peak, size);
		heap->real_size = size;
		heap->real_peak = peak;
	} while (0);
	do {
		size_t size = heap->size + new_size;
		size_t peak = MAX(heap->peak, size);
		heap->size = size;
		heap->peak = peak;
	} while (0);
#elif ZEND_MM_LIMIT
	heap->real_size += new_size;
#endif
	return ptr;
}

static void zend_mm_free_huge(zend_mm_heap *heap, void *ptr ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	size_t size;

	ZEND_MM_CHECK(ZEND_MM_ALIGNED_OFFSET(ptr, ZEND_MM_CHUNK_SIZE) == 0, "zend_mm_heap corrupted");
	size = zend_mm_del_huge_block(heap, ptr ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
	zend_mm_chunk_free(heap, ptr, size);
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

static zend_mm_heap *zend_mm_init(void)
{
	zend_mm_chunk *chunk = (zend_mm_chunk*)zend_mm_chunk_alloc_int(ZEND_MM_CHUNK_SIZE, ZEND_MM_CHUNK_SIZE);
	zend_mm_heap *heap;

	if (UNEXPECTED(chunk == NULL)) {
#if ZEND_MM_ERROR
		fprintf(stderr, "Can't initialize heap\n");
#endif
		return NULL;
	}
	heap = &chunk->heap_slot;
	chunk->heap = heap;
	chunk->next = chunk;
	chunk->prev = chunk;
	chunk->free_pages = ZEND_MM_PAGES - ZEND_MM_FIRST_PAGE;
	chunk->free_tail = ZEND_MM_FIRST_PAGE;
	chunk->num = 0;
	chunk->free_map[0] = (Z_L(1) << ZEND_MM_FIRST_PAGE) - 1;
	chunk->map[0] = ZEND_MM_LRUN(ZEND_MM_FIRST_PAGE);
	heap->main_chunk = chunk;
	heap->cached_chunks = NULL;
	heap->chunks_count = 1;
	heap->peak_chunks_count = 1;
	heap->cached_chunks_count = 0;
	heap->avg_chunks_count = 1.0;
	heap->last_chunks_delete_boundary = 0;
	heap->last_chunks_delete_count = 0;
#if ZEND_MM_STAT || ZEND_MM_LIMIT
	heap->real_size = ZEND_MM_CHUNK_SIZE;
#endif
#if ZEND_MM_STAT
	heap->real_peak = ZEND_MM_CHUNK_SIZE;
	heap->size = 0;
	heap->peak = 0;
#endif
#if ZEND_MM_LIMIT
	heap->limit = (size_t)Z_L(-1) >> 1;
	heap->overflow = 0;
#endif
#if ZEND_MM_CUSTOM
	heap->use_custom_heap = ZEND_MM_CUSTOM_HEAP_NONE;
#endif
#if ZEND_MM_STORAGE
	heap->storage = NULL;
#endif
	heap->huge_list = NULL;
	return heap;
}

ZEND_API size_t zend_mm_gc(zend_mm_heap *heap)
{
	zend_mm_free_slot *p, **q;
	zend_mm_chunk *chunk;
	size_t page_offset;
	int page_num;
	zend_mm_page_info info;
	uint32_t i, free_counter;
	bool has_free_pages;
	size_t collected = 0;

#if ZEND_MM_CUSTOM
	if (heap->use_custom_heap) {
		return 0;
	}
#endif

	for (i = 0; i < ZEND_MM_BINS; i++) {
		has_free_pages = false;
		p = heap->free_slot[i];
		while (p != NULL) {
			chunk = (zend_mm_chunk*)ZEND_MM_ALIGNED_BASE(p, ZEND_MM_CHUNK_SIZE);
			ZEND_MM_CHECK(chunk->heap == heap, "zend_mm_heap corrupted");
			page_offset = ZEND_MM_ALIGNED_OFFSET(p, ZEND_MM_CHUNK_SIZE);
			ZEND_ASSERT(page_offset != 0);
			page_num = (int)(page_offset / ZEND_MM_PAGE_SIZE);
			info = chunk->map[page_num];
			ZEND_ASSERT(info & ZEND_MM_IS_SRUN);
			if (info & ZEND_MM_IS_LRUN) {
				page_num -= ZEND_MM_NRUN_OFFSET(info);
				info = chunk->map[page_num];
				ZEND_ASSERT(info & ZEND_MM_IS_SRUN);
				ZEND_ASSERT(!(info & ZEND_MM_IS_LRUN));
			}
			ZEND_ASSERT(ZEND_MM_SRUN_BIN_NUM(info) == i);
			free_counter = ZEND_MM_SRUN_FREE_COUNTER(info) + 1;
			if (free_counter == bin_elements[i]) {
				has_free_pages = true;
			}
			chunk->map[page_num] = ZEND_MM_SRUN_EX(i, free_counter);
			p = p->next_free_slot;
		}

		if (!has_free_pages) {
			continue;
		}

		q = &heap->free_slot[i];
		p = *q;
		while (p != NULL) {
			chunk = (zend_mm_chunk*)ZEND_MM_ALIGNED_BASE(p, ZEND_MM_CHUNK_SIZE);
			ZEND_MM_CHECK(chunk->heap == heap, "zend_mm_heap corrupted");
			page_offset = ZEND_MM_ALIGNED_OFFSET(p, ZEND_MM_CHUNK_SIZE);
			ZEND_ASSERT(page_offset != 0);
			page_num = (int)(page_offset / ZEND_MM_PAGE_SIZE);
			info = chunk->map[page_num];
			ZEND_ASSERT(info & ZEND_MM_IS_SRUN);
			if (info & ZEND_MM_IS_LRUN) {
				page_num -= ZEND_MM_NRUN_OFFSET(info);
				info = chunk->map[page_num];
				ZEND_ASSERT(info & ZEND_MM_IS_SRUN);
				ZEND_ASSERT(!(info & ZEND_MM_IS_LRUN));
			}
			ZEND_ASSERT(ZEND_MM_SRUN_BIN_NUM(info) == i);
			if (ZEND_MM_SRUN_FREE_COUNTER(info) == bin_elements[i]) {
				/* remove from cache */
				p = p->next_free_slot;
				*q = p;
			} else {
				q = &p->next_free_slot;
				p = *q;
			}
		}
	}

	chunk = heap->main_chunk;
	do {
		i = ZEND_MM_FIRST_PAGE;
		while (i < chunk->free_tail) {
			if (zend_mm_bitset_is_set(chunk->free_map, i)) {
				info = chunk->map[i];
				if (info & ZEND_MM_IS_SRUN) {
					int bin_num = ZEND_MM_SRUN_BIN_NUM(info);
					int pages_count = bin_pages[bin_num];

					if (ZEND_MM_SRUN_FREE_COUNTER(info) == bin_elements[bin_num]) {
						/* all elements are free */
						zend_mm_free_pages_ex(heap, chunk, i, pages_count, 0);
						collected += pages_count;
					} else {
						/* reset counter */
						chunk->map[i] = ZEND_MM_SRUN(bin_num);
					}
					i += bin_pages[bin_num];
				} else /* if (info & ZEND_MM_IS_LRUN) */ {
					i += ZEND_MM_LRUN_PAGES(info);
				}
			} else {
				i++;
			}
		}
		if (chunk->free_pages == ZEND_MM_PAGES - ZEND_MM_FIRST_PAGE) {
			zend_mm_chunk *next_chunk = chunk->next;

			zend_mm_delete_chunk(heap, chunk);
			chunk = next_chunk;
		} else {
			chunk = chunk->next;
		}
	} while (chunk != heap->main_chunk);

	return collected * ZEND_MM_PAGE_SIZE;
}

#if ZEND_DEBUG
/******************/
/* Leak detection */
/******************/

static zend_long zend_mm_find_leaks_small(zend_mm_chunk *p, uint32_t i, uint32_t j, zend_leak_info *leak)
{
	bool empty = true;
	zend_long count = 0;
	int bin_num = ZEND_MM_SRUN_BIN_NUM(p->map[i]);
	zend_mm_debug_info *dbg = (zend_mm_debug_info*)((char*)p + ZEND_MM_PAGE_SIZE * i + bin_data_size[bin_num] * (j + 1) - ZEND_MM_ALIGNED_SIZE(sizeof(zend_mm_debug_info)));

	while (j < bin_elements[bin_num]) {
		if (dbg->size != 0) {
			if (dbg->filename == leak->filename && dbg->lineno == leak->lineno) {
				count++;
				dbg->size = 0;
				dbg->filename = NULL;
				dbg->lineno = 0;
			} else {
				empty = false;
			}
		}
		j++;
		dbg = (zend_mm_debug_info*)((char*)dbg + bin_data_size[bin_num]);
	}
	if (empty) {
		zend_mm_bitset_reset_range(p->free_map, i, bin_pages[bin_num]);
	}
	return count;
}

static zend_long zend_mm_find_leaks(zend_mm_heap *heap, zend_mm_chunk *p, uint32_t i, zend_leak_info *leak)
{
	zend_long count = 0;

	do {
		while (i < p->free_tail) {
			if (zend_mm_bitset_is_set(p->free_map, i)) {
				if (p->map[i] & ZEND_MM_IS_SRUN) {
					int bin_num = ZEND_MM_SRUN_BIN_NUM(p->map[i]);
					count += zend_mm_find_leaks_small(p, i, 0, leak);
					i += bin_pages[bin_num];
				} else /* if (p->map[i] & ZEND_MM_IS_LRUN) */ {
					int pages_count = ZEND_MM_LRUN_PAGES(p->map[i]);
					zend_mm_debug_info *dbg = (zend_mm_debug_info*)((char*)p + ZEND_MM_PAGE_SIZE * (i + pages_count) - ZEND_MM_ALIGNED_SIZE(sizeof(zend_mm_debug_info)));

					if (dbg->filename == leak->filename && dbg->lineno == leak->lineno) {
						count++;
					}
					zend_mm_bitset_reset_range(p->free_map, i, pages_count);
					i += pages_count;
				}
			} else {
				i++;
			}
		}
		p = p->next;
		i = ZEND_MM_FIRST_PAGE;
	} while (p != heap->main_chunk);
	return count;
}

static zend_long zend_mm_find_leaks_huge(zend_mm_heap *heap, zend_mm_huge_list *list)
{
	zend_long count = 0;
	zend_mm_huge_list *prev = list;
	zend_mm_huge_list *p = list->next;

	while (p) {
		if (p->dbg.filename == list->dbg.filename && p->dbg.lineno == list->dbg.lineno) {
			prev->next = p->next;
			zend_mm_chunk_free(heap, p->ptr, p->size);
			zend_mm_free_heap(heap, p, NULL, 0, NULL, 0);
			count++;
		} else {
			prev = p;
		}
		p = prev->next;
	}

	return count;
}

static void zend_mm_check_leaks(zend_mm_heap *heap)
{
	zend_mm_huge_list *list;
	zend_mm_chunk *p;
	zend_leak_info leak;
	zend_long repeated = 0;
	uint32_t total = 0;
	uint32_t i, j;

	/* find leaked huge blocks and free them */
	list = heap->huge_list;
	while (list) {
		zend_mm_huge_list *q = list;

		leak.addr = list->ptr;
		leak.size = list->dbg.size;
		leak.filename = list->dbg.filename;
		leak.orig_filename = list->dbg.orig_filename;
		leak.lineno = list->dbg.lineno;
		leak.orig_lineno = list->dbg.orig_lineno;

		zend_message_dispatcher(ZMSG_LOG_SCRIPT_NAME, NULL);
		zend_message_dispatcher(ZMSG_MEMORY_LEAK_DETECTED, &leak);
		repeated = zend_mm_find_leaks_huge(heap, list);
		total += 1 + repeated;
		if (repeated) {
			zend_message_dispatcher(ZMSG_MEMORY_LEAK_REPEATED, (void *)(uintptr_t)repeated);
		}

		heap->huge_list = list = list->next;
		zend_mm_chunk_free(heap, q->ptr, q->size);
		zend_mm_free_heap(heap, q, NULL, 0, NULL, 0);
	}

	/* for each chunk */
	p = heap->main_chunk;
	do {
		i = ZEND_MM_FIRST_PAGE;
		while (i < p->free_tail) {
			if (zend_mm_bitset_is_set(p->free_map, i)) {
				if (p->map[i] & ZEND_MM_IS_SRUN) {
					int bin_num = ZEND_MM_SRUN_BIN_NUM(p->map[i]);
					zend_mm_debug_info *dbg = (zend_mm_debug_info*)((char*)p + ZEND_MM_PAGE_SIZE * i + bin_data_size[bin_num] - ZEND_MM_ALIGNED_SIZE(sizeof(zend_mm_debug_info)));

					j = 0;
					while (j < bin_elements[bin_num]) {
						if (dbg->size != 0) {
							leak.addr = (zend_mm_debug_info*)((char*)p + ZEND_MM_PAGE_SIZE * i + bin_data_size[bin_num] * j);
							leak.size = dbg->size;
							leak.filename = dbg->filename;
							leak.orig_filename = dbg->orig_filename;
							leak.lineno = dbg->lineno;
							leak.orig_lineno = dbg->orig_lineno;

							zend_message_dispatcher(ZMSG_LOG_SCRIPT_NAME, NULL);
							zend_message_dispatcher(ZMSG_MEMORY_LEAK_DETECTED, &leak);

							dbg->size = 0;
							dbg->filename = NULL;
							dbg->lineno = 0;

							repeated = zend_mm_find_leaks_small(p, i, j + 1, &leak) +
							           zend_mm_find_leaks(heap, p, i + bin_pages[bin_num], &leak);
							total += 1 + repeated;
							if (repeated) {
								zend_message_dispatcher(ZMSG_MEMORY_LEAK_REPEATED, (void *)(uintptr_t)repeated);
							}
						}
						dbg = (zend_mm_debug_info*)((char*)dbg + bin_data_size[bin_num]);
						j++;
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

					zend_message_dispatcher(ZMSG_LOG_SCRIPT_NAME, NULL);
					zend_message_dispatcher(ZMSG_MEMORY_LEAK_DETECTED, &leak);

					zend_mm_bitset_reset_range(p->free_map, i, pages_count);

					repeated = zend_mm_find_leaks(heap, p, i + pages_count, &leak);
					total += 1 + repeated;
					if (repeated) {
						zend_message_dispatcher(ZMSG_MEMORY_LEAK_REPEATED, (void *)(uintptr_t)repeated);
					}
					i += pages_count;
				}
			} else {
				i++;
			}
		}
		p = p->next;
	} while (p != heap->main_chunk);
	if (total) {
		zend_message_dispatcher(ZMSG_MEMORY_LEAKS_GRAND_TOTAL, &total);
	}
}
#endif

#if ZEND_MM_CUSTOM
static void *tracked_malloc(size_t size);
static void tracked_free_all(void);
#endif

void zend_mm_shutdown(zend_mm_heap *heap, bool full, bool silent)
{
	zend_mm_chunk *p;
	zend_mm_huge_list *list;

#if ZEND_MM_CUSTOM
	if (heap->use_custom_heap) {
		if (heap->custom_heap.std._malloc == tracked_malloc) {
			if (silent) {
				tracked_free_all();
			}
			zend_hash_clean(heap->tracked_allocs);
			if (full) {
				zend_hash_destroy(heap->tracked_allocs);
				free(heap->tracked_allocs);
				/* Make sure the heap free below does not use tracked_free(). */
				heap->custom_heap.std._free = free;
			}
			heap->size = 0;
		}

		if (full) {
			if (ZEND_DEBUG && heap->use_custom_heap == ZEND_MM_CUSTOM_HEAP_DEBUG) {
				heap->custom_heap.debug._free(heap ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC);
			} else {
				heap->custom_heap.std._free(heap);
			}
		}
		return;
	}
#endif

#if ZEND_DEBUG
	if (!silent) {
		char *tmp = getenv("ZEND_ALLOC_PRINT_LEAKS");
		if (!tmp || ZEND_ATOL(tmp)) {
			zend_mm_check_leaks(heap);
		}
	}
#endif

	/* free huge blocks */
	list = heap->huge_list;
	heap->huge_list = NULL;
	while (list) {
		zend_mm_huge_list *q = list;
		list = list->next;
		zend_mm_chunk_free(heap, q->ptr, q->size);
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
			zend_mm_chunk_free(heap, p, ZEND_MM_CHUNK_SIZE);
		}
		/* free the first chunk */
		zend_mm_chunk_free(heap, heap->main_chunk, ZEND_MM_CHUNK_SIZE);
	} else {
		/* free some cached chunks to keep average count */
		heap->avg_chunks_count = (heap->avg_chunks_count + (double)heap->peak_chunks_count) / 2.0;
		while ((double)heap->cached_chunks_count + 0.9 > heap->avg_chunks_count &&
		       heap->cached_chunks) {
			p = heap->cached_chunks;
			heap->cached_chunks = p->next;
			zend_mm_chunk_free(heap, p, ZEND_MM_CHUNK_SIZE);
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
		p = heap->main_chunk;
		p->heap = &p->heap_slot;
		p->next = p;
		p->prev = p;
		p->free_pages = ZEND_MM_PAGES - ZEND_MM_FIRST_PAGE;
		p->free_tail = ZEND_MM_FIRST_PAGE;
		p->num = 0;

#if ZEND_MM_STAT
		heap->size = heap->peak = 0;
#endif
		memset(heap->free_slot, 0, sizeof(heap->free_slot));
#if ZEND_MM_STAT || ZEND_MM_LIMIT
		heap->real_size = (heap->cached_chunks_count + 1) * ZEND_MM_CHUNK_SIZE;
#endif
#if ZEND_MM_STAT
		heap->real_peak = (heap->cached_chunks_count + 1) * ZEND_MM_CHUNK_SIZE;
#endif
		heap->chunks_count = 1;
		heap->peak_chunks_count = 1;
		heap->last_chunks_delete_boundary = 0;
		heap->last_chunks_delete_count = 0;

		memset(p->free_map, 0, sizeof(p->free_map) + sizeof(p->map));
		p->free_map[0] = (1L << ZEND_MM_FIRST_PAGE) - 1;
		p->map[0] = ZEND_MM_LRUN(ZEND_MM_FIRST_PAGE);
	}
}

/**************/
/* PUBLIC API */
/**************/

ZEND_API void* ZEND_FASTCALL _zend_mm_alloc(zend_mm_heap *heap, size_t size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	return zend_mm_alloc_heap(heap, size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
}

ZEND_API void ZEND_FASTCALL _zend_mm_free(zend_mm_heap *heap, void *ptr ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	zend_mm_free_heap(heap, ptr ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
}

void* ZEND_FASTCALL _zend_mm_realloc(zend_mm_heap *heap, void *ptr, size_t size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	return zend_mm_realloc_heap(heap, ptr, size, 0, size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
}

void* ZEND_FASTCALL _zend_mm_realloc2(zend_mm_heap *heap, void *ptr, size_t size, size_t copy_size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	return zend_mm_realloc_heap(heap, ptr, size, 1, copy_size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
}

ZEND_API size_t ZEND_FASTCALL _zend_mm_block_size(zend_mm_heap *heap, void *ptr ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
#if ZEND_MM_CUSTOM
	if (UNEXPECTED(heap->use_custom_heap)) {
		if (heap->custom_heap.std._malloc == tracked_malloc) {
			zend_ulong h = ((uintptr_t) ptr) >> ZEND_MM_ALIGNMENT_LOG2;
			zval *size_zv = zend_hash_index_find(heap->tracked_allocs, h);
			if  (size_zv) {
				return Z_LVAL_P(size_zv);
			}
		}
		return 0;
	}
#endif
	return zend_mm_size(heap, ptr ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
}

/**********************/
/* Allocation Manager */
/**********************/

typedef struct _zend_alloc_globals {
	zend_mm_heap *mm_heap;
} zend_alloc_globals;

#ifdef ZTS
static int alloc_globals_id;
static size_t alloc_globals_offset;
# define AG(v) ZEND_TSRMG_FAST(alloc_globals_offset, zend_alloc_globals *, v)
#else
# define AG(v) (alloc_globals.v)
static zend_alloc_globals alloc_globals;
#endif

ZEND_API bool is_zend_mm(void)
{
#if ZEND_MM_CUSTOM
	return !AG(mm_heap)->use_custom_heap;
#else
	return 1;
#endif
}

ZEND_API bool is_zend_ptr(const void *ptr)
{
#if ZEND_MM_CUSTOM
	if (AG(mm_heap)->use_custom_heap) {
		if (AG(mm_heap)->custom_heap.std._malloc == tracked_malloc) {
			zend_ulong h = ((uintptr_t) ptr) >> ZEND_MM_ALIGNMENT_LOG2;
			zval *size_zv = zend_hash_index_find(AG(mm_heap)->tracked_allocs, h);
			if  (size_zv) {
				return 1;
			}
		}
		return 0;
	}
#endif

	if (AG(mm_heap)->main_chunk) {
		zend_mm_chunk *chunk = AG(mm_heap)->main_chunk;

		do {
			if (ptr >= (void*)chunk
			 && ptr < (void*)((char*)chunk + ZEND_MM_CHUNK_SIZE)) {
				return 1;
			}
			chunk = chunk->next;
		} while (chunk != AG(mm_heap)->main_chunk);
	}

	if (AG(mm_heap)->huge_list) {
		zend_mm_huge_list *block = AG(mm_heap)->huge_list;

		do {
			if (ptr >= (void*)block
			 && ptr < (void*)((char*)block + block->size)) {
				return 1;
			}
			block = block->next;
		} while (block != AG(mm_heap)->huge_list);
	}
	return 0;
}

#if ZEND_MM_CUSTOM

static ZEND_COLD void* ZEND_FASTCALL _malloc_custom(size_t size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	if (ZEND_DEBUG && AG(mm_heap)->use_custom_heap == ZEND_MM_CUSTOM_HEAP_DEBUG) {
		return AG(mm_heap)->custom_heap.debug._malloc(size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
	} else {
		return AG(mm_heap)->custom_heap.std._malloc(size);
	}
}

static ZEND_COLD void ZEND_FASTCALL _efree_custom(void *ptr ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	if (ZEND_DEBUG && AG(mm_heap)->use_custom_heap == ZEND_MM_CUSTOM_HEAP_DEBUG) {
		AG(mm_heap)->custom_heap.debug._free(ptr ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
	} else {
		AG(mm_heap)->custom_heap.std._free(ptr);
	}
}

static ZEND_COLD void* ZEND_FASTCALL _realloc_custom(void *ptr, size_t size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	if (ZEND_DEBUG && AG(mm_heap)->use_custom_heap == ZEND_MM_CUSTOM_HEAP_DEBUG) {
		return AG(mm_heap)->custom_heap.debug._realloc(ptr, size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
	} else {
		return AG(mm_heap)->custom_heap.std._realloc(ptr, size);
	}
}
#endif

#if !ZEND_DEBUG && defined(HAVE_BUILTIN_CONSTANT_P)
#undef _emalloc

#if ZEND_MM_CUSTOM
# define ZEND_MM_CUSTOM_ALLOCATOR(size) do { \
		if (UNEXPECTED(AG(mm_heap)->use_custom_heap)) { \
			return _malloc_custom(size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC); \
		} \
	} while (0)
# define ZEND_MM_CUSTOM_DEALLOCATOR(ptr) do { \
		if (UNEXPECTED(AG(mm_heap)->use_custom_heap)) { \
			_efree_custom(ptr ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC); \
			return; \
		} \
	} while (0)
#else
# define ZEND_MM_CUSTOM_ALLOCATOR(size)
# define ZEND_MM_CUSTOM_DEALLOCATOR(ptr)
#endif

# define _ZEND_BIN_ALLOCATOR(_num, _size, _elements, _pages, x, y) \
	ZEND_API void* ZEND_FASTCALL _emalloc_ ## _size(void) { \
		ZEND_MM_CUSTOM_ALLOCATOR(_size); \
		return zend_mm_alloc_small(AG(mm_heap), _num ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC); \
	}

ZEND_MM_BINS_INFO(_ZEND_BIN_ALLOCATOR, x, y)

ZEND_API void* ZEND_FASTCALL _emalloc_large(size_t size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	ZEND_MM_CUSTOM_ALLOCATOR(size);
	return zend_mm_alloc_large_ex(AG(mm_heap), size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
}

ZEND_API void* ZEND_FASTCALL _emalloc_huge(size_t size)
{
	ZEND_MM_CUSTOM_ALLOCATOR(size);
	return zend_mm_alloc_huge(AG(mm_heap), size);
}

#if ZEND_DEBUG
# define _ZEND_BIN_FREE(_num, _size, _elements, _pages, x, y) \
	ZEND_API void ZEND_FASTCALL _efree_ ## _size(void *ptr) { \
		ZEND_MM_CUSTOM_DEALLOCATOR(ptr); \
		{ \
			size_t page_offset = ZEND_MM_ALIGNED_OFFSET(ptr, ZEND_MM_CHUNK_SIZE); \
			zend_mm_chunk *chunk = (zend_mm_chunk*)ZEND_MM_ALIGNED_BASE(ptr, ZEND_MM_CHUNK_SIZE); \
			int page_num = page_offset / ZEND_MM_PAGE_SIZE; \
			ZEND_MM_CHECK(chunk->heap == AG(mm_heap), "zend_mm_heap corrupted"); \
			ZEND_ASSERT(chunk->map[page_num] & ZEND_MM_IS_SRUN); \
			ZEND_ASSERT(ZEND_MM_SRUN_BIN_NUM(chunk->map[page_num]) == _num); \
			zend_mm_free_small(AG(mm_heap), ptr, _num); \
		} \
	}
#else
# define _ZEND_BIN_FREE(_num, _size, _elements, _pages, x, y) \
	ZEND_API void ZEND_FASTCALL _efree_ ## _size(void *ptr) { \
		ZEND_MM_CUSTOM_DEALLOCATOR(ptr); \
		{ \
			zend_mm_chunk *chunk = (zend_mm_chunk*)ZEND_MM_ALIGNED_BASE(ptr, ZEND_MM_CHUNK_SIZE); \
			ZEND_MM_CHECK(chunk->heap == AG(mm_heap), "zend_mm_heap corrupted"); \
			zend_mm_free_small(AG(mm_heap), ptr, _num); \
		} \
	}
#endif

ZEND_MM_BINS_INFO(_ZEND_BIN_FREE, x, y)

ZEND_API void ZEND_FASTCALL _efree_large(void *ptr, size_t size)
{
	ZEND_MM_CUSTOM_DEALLOCATOR(ptr);
	{
		size_t page_offset = ZEND_MM_ALIGNED_OFFSET(ptr, ZEND_MM_CHUNK_SIZE);
		zend_mm_chunk *chunk = (zend_mm_chunk*)ZEND_MM_ALIGNED_BASE(ptr, ZEND_MM_CHUNK_SIZE);
		int page_num = page_offset / ZEND_MM_PAGE_SIZE;
		uint32_t pages_count = ZEND_MM_ALIGNED_SIZE_EX(size, ZEND_MM_PAGE_SIZE) / ZEND_MM_PAGE_SIZE;

		ZEND_MM_CHECK(chunk->heap == AG(mm_heap) && ZEND_MM_ALIGNED_OFFSET(page_offset, ZEND_MM_PAGE_SIZE) == 0, "zend_mm_heap corrupted");
		ZEND_ASSERT(chunk->map[page_num] & ZEND_MM_IS_LRUN);
		ZEND_ASSERT(ZEND_MM_LRUN_PAGES(chunk->map[page_num]) == pages_count);
		zend_mm_free_large(AG(mm_heap), chunk, page_num, pages_count);
	}
}

ZEND_API void ZEND_FASTCALL _efree_huge(void *ptr, size_t size)
{

	ZEND_MM_CUSTOM_DEALLOCATOR(ptr);
	zend_mm_free_huge(AG(mm_heap), ptr);
}
#endif

ZEND_API void* ZEND_FASTCALL _emalloc(size_t size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
#if ZEND_MM_CUSTOM
	if (UNEXPECTED(AG(mm_heap)->use_custom_heap)) {
		return _malloc_custom(size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
	}
#endif
	return zend_mm_alloc_heap(AG(mm_heap), size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
}

ZEND_API void ZEND_FASTCALL _efree(void *ptr ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
#if ZEND_MM_CUSTOM
	if (UNEXPECTED(AG(mm_heap)->use_custom_heap)) {
		_efree_custom(ptr ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
		return;
	}
#endif
	zend_mm_free_heap(AG(mm_heap), ptr ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
}

ZEND_API void* ZEND_FASTCALL _erealloc(void *ptr, size_t size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
#if ZEND_MM_CUSTOM
	if (UNEXPECTED(AG(mm_heap)->use_custom_heap)) {
		return _realloc_custom(ptr, size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
	}
#endif
	return zend_mm_realloc_heap(AG(mm_heap), ptr, size, 0, size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
}

ZEND_API void* ZEND_FASTCALL _erealloc2(void *ptr, size_t size, size_t copy_size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
#if ZEND_MM_CUSTOM
	if (UNEXPECTED(AG(mm_heap)->use_custom_heap)) {
		return _realloc_custom(ptr, size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
	}
#endif
	return zend_mm_realloc_heap(AG(mm_heap), ptr, size, 1, copy_size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
}

ZEND_API size_t ZEND_FASTCALL _zend_mem_block_size(void *ptr ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	return _zend_mm_block_size(AG(mm_heap), ptr ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
}

ZEND_API void* ZEND_FASTCALL _safe_emalloc(size_t nmemb, size_t size, size_t offset ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	return _emalloc(zend_safe_address_guarded(nmemb, size, offset) ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
}

ZEND_API void* ZEND_FASTCALL _safe_malloc(size_t nmemb, size_t size, size_t offset)
{
	return pemalloc(zend_safe_address_guarded(nmemb, size, offset), 1);
}

ZEND_API void* ZEND_FASTCALL _safe_erealloc(void *ptr, size_t nmemb, size_t size, size_t offset ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	return _erealloc(ptr, zend_safe_address_guarded(nmemb, size, offset) ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
}

ZEND_API void* ZEND_FASTCALL _safe_realloc(void *ptr, size_t nmemb, size_t size, size_t offset)
{
	return perealloc(ptr, zend_safe_address_guarded(nmemb, size, offset), 1);
}

ZEND_API void* ZEND_FASTCALL _ecalloc(size_t nmemb, size_t size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	void *p;

	size = zend_safe_address_guarded(nmemb, size, 0);
	p = _emalloc(size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
	memset(p, 0, size);
	return p;
}

ZEND_API char* ZEND_FASTCALL _estrdup(const char *s ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	size_t length;
	char *p;

	length = strlen(s);
	if (UNEXPECTED(length + 1 == 0)) {
		zend_error_noreturn(E_ERROR, "Possible integer overflow in memory allocation (1 * %zu + 1)", length);
	}
	p = (char *) _emalloc(length + 1 ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
	memcpy(p, s, length+1);
	return p;
}

ZEND_API char* ZEND_FASTCALL _estrndup(const char *s, size_t length ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	char *p;

	if (UNEXPECTED(length + 1 == 0)) {
		zend_error_noreturn(E_ERROR, "Possible integer overflow in memory allocation (1 * %zu + 1)", length);
	}
	p = (char *) _emalloc(length + 1 ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
	memcpy(p, s, length);
	p[length] = 0;
	return p;
}

static ZEND_COLD ZEND_NORETURN void zend_out_of_memory(void);

ZEND_API char* ZEND_FASTCALL zend_strndup(const char *s, size_t length)
{
	char *p;

	if (UNEXPECTED(length + 1 == 0)) {
		zend_error_noreturn(E_ERROR, "Possible integer overflow in memory allocation (1 * %zu + 1)", length);
	}
	p = (char *) malloc(length + 1);
	if (UNEXPECTED(p == NULL)) {
		zend_out_of_memory();
	}
	if (EXPECTED(length)) {
		memcpy(p, s, length);
	}
	p[length] = 0;
	return p;
}

ZEND_API zend_result zend_set_memory_limit(size_t memory_limit)
{
#if ZEND_MM_LIMIT
	zend_mm_heap *heap = AG(mm_heap);

	if (UNEXPECTED(memory_limit < heap->real_size)) {
		if (memory_limit >= heap->real_size - heap->cached_chunks_count * ZEND_MM_CHUNK_SIZE) {
			/* free some cached chunks to fit into new memory limit */
			do {
				zend_mm_chunk *p = heap->cached_chunks;
				heap->cached_chunks = p->next;
				zend_mm_chunk_free(heap, p, ZEND_MM_CHUNK_SIZE);
				heap->cached_chunks_count--;
				heap->real_size -= ZEND_MM_CHUNK_SIZE;
			} while (memory_limit < heap->real_size);
			return SUCCESS;
		}
		return FAILURE;
	}
	AG(mm_heap)->limit = memory_limit;
#endif
	return SUCCESS;
}

ZEND_API bool zend_alloc_in_memory_limit_error_reporting(void)
{
#if ZEND_MM_LIMIT
	return AG(mm_heap)->overflow;
#else
	return false;
#endif
}

ZEND_API size_t zend_memory_usage(bool real_usage)
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

ZEND_API size_t zend_memory_peak_usage(bool real_usage)
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

ZEND_API void zend_memory_reset_peak_usage(void)
{
#if ZEND_MM_STAT
	AG(mm_heap)->real_peak = AG(mm_heap)->real_size;
	AG(mm_heap)->peak = AG(mm_heap)->size;
#endif
}

ZEND_API void shutdown_memory_manager(bool silent, bool full_shutdown)
{
	zend_mm_shutdown(AG(mm_heap), full_shutdown, silent);
}

static ZEND_COLD ZEND_NORETURN void zend_out_of_memory(void)
{
	fprintf(stderr, "Out of memory\n");
	exit(1);
}

#if ZEND_MM_CUSTOM
static zend_always_inline void tracked_add(zend_mm_heap *heap, void *ptr, size_t size) {
	zval size_zv;
	zend_ulong h = ((uintptr_t) ptr) >> ZEND_MM_ALIGNMENT_LOG2;
	ZEND_ASSERT((void *) (uintptr_t) (h << ZEND_MM_ALIGNMENT_LOG2) == ptr);
	ZVAL_LONG(&size_zv, size);
	zend_hash_index_add_new(heap->tracked_allocs, h, &size_zv);
}

static zend_always_inline zval *tracked_get_size_zv(zend_mm_heap *heap, void *ptr) {
	zend_ulong h = ((uintptr_t) ptr) >> ZEND_MM_ALIGNMENT_LOG2;
	zval *size_zv = zend_hash_index_find(heap->tracked_allocs, h);
	ZEND_ASSERT(size_zv && "Trying to free pointer not allocated through ZendMM");
	return size_zv;
}

static zend_always_inline void tracked_check_limit(zend_mm_heap *heap, size_t add_size) {
	if (add_size > heap->limit - heap->size && !heap->overflow) {
#if ZEND_DEBUG
		zend_mm_safe_error(heap,
			"Allowed memory size of %zu bytes exhausted at %s:%d (tried to allocate %zu bytes)",
			heap->limit, "file", 0, add_size);
#else
		zend_mm_safe_error(heap,
			"Allowed memory size of %zu bytes exhausted (tried to allocate %zu bytes)",
			heap->limit, add_size);
#endif
	}
}

static void *tracked_malloc(size_t size)
{
	zend_mm_heap *heap = AG(mm_heap);
	tracked_check_limit(heap, size);

	void *ptr = malloc(size);
	if (!ptr) {
		zend_out_of_memory();
	}

	tracked_add(heap, ptr, size);
	heap->size += size;
	return ptr;
}

static void tracked_free(void *ptr) {
	if (!ptr) {
		return;
	}

	zend_mm_heap *heap = AG(mm_heap);
	zval *size_zv = tracked_get_size_zv(heap, ptr);
	heap->size -= Z_LVAL_P(size_zv);
	zend_hash_del_bucket(heap->tracked_allocs, (Bucket *) size_zv);
	free(ptr);
}

static void *tracked_realloc(void *ptr, size_t new_size) {
	zend_mm_heap *heap = AG(mm_heap);
	zval *old_size_zv = NULL;
	size_t old_size = 0;
	if (ptr) {
		old_size_zv = tracked_get_size_zv(heap, ptr);
		old_size = Z_LVAL_P(old_size_zv);
	}

	if (new_size > old_size) {
		tracked_check_limit(heap, new_size - old_size);
	}

	/* Delete information about old allocation only after checking the memory limit. */
	if (old_size_zv) {
		zend_hash_del_bucket(heap->tracked_allocs, (Bucket *) old_size_zv);
	}

	ptr = __zend_realloc(ptr, new_size);
	tracked_add(heap, ptr, new_size);
	heap->size += new_size - old_size;
	return ptr;
}

static void tracked_free_all(void) {
	HashTable *tracked_allocs = AG(mm_heap)->tracked_allocs;
	zend_ulong h;
	ZEND_HASH_FOREACH_NUM_KEY(tracked_allocs, h) {
		void *ptr = (void *) (uintptr_t) (h << ZEND_MM_ALIGNMENT_LOG2);
		free(ptr);
	} ZEND_HASH_FOREACH_END();
}
#endif

static void alloc_globals_ctor(zend_alloc_globals *alloc_globals)
{
	char *tmp;

#if ZEND_MM_CUSTOM
	tmp = getenv("USE_ZEND_ALLOC");
	if (tmp && !ZEND_ATOL(tmp)) {
		bool tracked = (tmp = getenv("USE_TRACKED_ALLOC")) && ZEND_ATOL(tmp);
		zend_mm_heap *mm_heap = alloc_globals->mm_heap = malloc(sizeof(zend_mm_heap));
		memset(mm_heap, 0, sizeof(zend_mm_heap));
		mm_heap->use_custom_heap = ZEND_MM_CUSTOM_HEAP_STD;
		mm_heap->limit = (size_t)Z_L(-1) >> 1;
		mm_heap->overflow = 0;

		if (!tracked) {
			/* Use system allocator. */
			mm_heap->custom_heap.std._malloc = __zend_malloc;
			mm_heap->custom_heap.std._free = free;
			mm_heap->custom_heap.std._realloc = __zend_realloc;
		} else {
			/* Use system allocator and track allocations for auto-free. */
			mm_heap->custom_heap.std._malloc = tracked_malloc;
			mm_heap->custom_heap.std._free = tracked_free;
			mm_heap->custom_heap.std._realloc = tracked_realloc;
			mm_heap->tracked_allocs = malloc(sizeof(HashTable));
			zend_hash_init(mm_heap->tracked_allocs, 1024, NULL, NULL, 1);
		}
		return;
	}
#endif

	tmp = getenv("USE_ZEND_ALLOC_HUGE_PAGES");
	if (tmp && ZEND_ATOL(tmp)) {
		zend_mm_use_huge_pages = true;
	}
	alloc_globals->mm_heap = zend_mm_init();
}

#ifdef ZTS
static void alloc_globals_dtor(zend_alloc_globals *alloc_globals)
{
	zend_mm_shutdown(alloc_globals->mm_heap, 1, 1);
}
#endif

ZEND_API void start_memory_manager(void)
{
#ifdef ZTS
	ts_allocate_fast_id(&alloc_globals_id, &alloc_globals_offset, sizeof(zend_alloc_globals), (ts_allocate_ctor) alloc_globals_ctor, (ts_allocate_dtor) alloc_globals_dtor);
#else
	alloc_globals_ctor(&alloc_globals);
#endif
#ifndef _WIN32
#  if defined(_SC_PAGESIZE)
	REAL_PAGE_SIZE = sysconf(_SC_PAGESIZE);
#  elif defined(_SC_PAGE_SIZE)
	REAL_PAGE_SIZE = sysconf(_SC_PAGE_SIZE);
#  endif
#endif
}

ZEND_API zend_mm_heap *zend_mm_set_heap(zend_mm_heap *new_heap)
{
	zend_mm_heap *old_heap;

	old_heap = AG(mm_heap);
	AG(mm_heap) = (zend_mm_heap*)new_heap;
	return (zend_mm_heap*)old_heap;
}

ZEND_API zend_mm_heap *zend_mm_get_heap(void)
{
	return AG(mm_heap);
}

ZEND_API bool zend_mm_is_custom_heap(zend_mm_heap *new_heap)
{
#if ZEND_MM_CUSTOM
	return AG(mm_heap)->use_custom_heap;
#else
	return 0;
#endif
}

ZEND_API void zend_mm_set_custom_handlers(zend_mm_heap *heap,
                                          void* (*_malloc)(size_t),
                                          void  (*_free)(void*),
                                          void* (*_realloc)(void*, size_t))
{
#if ZEND_MM_CUSTOM
	zend_mm_heap *_heap = (zend_mm_heap*)heap;

	if (!_malloc && !_free && !_realloc) {
		_heap->use_custom_heap = ZEND_MM_CUSTOM_HEAP_NONE;
	} else {
		_heap->use_custom_heap = ZEND_MM_CUSTOM_HEAP_STD;
		_heap->custom_heap.std._malloc = _malloc;
		_heap->custom_heap.std._free = _free;
		_heap->custom_heap.std._realloc = _realloc;
	}
#endif
}

ZEND_API void zend_mm_get_custom_handlers(zend_mm_heap *heap,
                                          void* (**_malloc)(size_t),
                                          void  (**_free)(void*),
                                          void* (**_realloc)(void*, size_t))
{
#if ZEND_MM_CUSTOM
	zend_mm_heap *_heap = (zend_mm_heap*)heap;

	if (heap->use_custom_heap) {
		*_malloc = _heap->custom_heap.std._malloc;
		*_free = _heap->custom_heap.std._free;
		*_realloc = _heap->custom_heap.std._realloc;
	} else {
		*_malloc = NULL;
		*_free = NULL;
		*_realloc = NULL;
	}
#else
	*_malloc = NULL;
	*_free = NULL;
	*_realloc = NULL;
#endif
}

#if ZEND_DEBUG
ZEND_API void zend_mm_set_custom_debug_handlers(zend_mm_heap *heap,
                                                void* (*_malloc)(size_t ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC),
                                                void  (*_free)(void* ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC),
                                                void* (*_realloc)(void*, size_t ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC))
{
#if ZEND_MM_CUSTOM
	zend_mm_heap *_heap = (zend_mm_heap*)heap;

	_heap->use_custom_heap = ZEND_MM_CUSTOM_HEAP_DEBUG;
	_heap->custom_heap.debug._malloc = _malloc;
	_heap->custom_heap.debug._free = _free;
	_heap->custom_heap.debug._realloc = _realloc;
#endif
}
#endif

ZEND_API zend_mm_storage *zend_mm_get_storage(zend_mm_heap *heap)
{
#if ZEND_MM_STORAGE
	return heap->storage;
#else
	return NULL
#endif
}

ZEND_API zend_mm_heap *zend_mm_startup(void)
{
	return zend_mm_init();
}

ZEND_API zend_mm_heap *zend_mm_startup_ex(const zend_mm_handlers *handlers, void *data, size_t data_size)
{
#if ZEND_MM_STORAGE
	zend_mm_storage tmp_storage, *storage;
	zend_mm_chunk *chunk;
	zend_mm_heap *heap;

	memcpy((zend_mm_handlers*)&tmp_storage.handlers, handlers, sizeof(zend_mm_handlers));
	tmp_storage.data = data;
	chunk = (zend_mm_chunk*)handlers->chunk_alloc(&tmp_storage, ZEND_MM_CHUNK_SIZE, ZEND_MM_CHUNK_SIZE);
	if (UNEXPECTED(chunk == NULL)) {
#if ZEND_MM_ERROR
		fprintf(stderr, "Can't initialize heap\n");
#endif
		return NULL;
	}
	heap = &chunk->heap_slot;
	chunk->heap = heap;
	chunk->next = chunk;
	chunk->prev = chunk;
	chunk->free_pages = ZEND_MM_PAGES - ZEND_MM_FIRST_PAGE;
	chunk->free_tail = ZEND_MM_FIRST_PAGE;
	chunk->num = 0;
	chunk->free_map[0] = (Z_L(1) << ZEND_MM_FIRST_PAGE) - 1;
	chunk->map[0] = ZEND_MM_LRUN(ZEND_MM_FIRST_PAGE);
	heap->main_chunk = chunk;
	heap->cached_chunks = NULL;
	heap->chunks_count = 1;
	heap->peak_chunks_count = 1;
	heap->cached_chunks_count = 0;
	heap->avg_chunks_count = 1.0;
	heap->last_chunks_delete_boundary = 0;
	heap->last_chunks_delete_count = 0;
#if ZEND_MM_STAT || ZEND_MM_LIMIT
	heap->real_size = ZEND_MM_CHUNK_SIZE;
#endif
#if ZEND_MM_STAT
	heap->real_peak = ZEND_MM_CHUNK_SIZE;
	heap->size = 0;
	heap->peak = 0;
#endif
#if ZEND_MM_LIMIT
	heap->limit = (size_t)Z_L(-1) >> 1;
	heap->overflow = 0;
#endif
#if ZEND_MM_CUSTOM
	heap->use_custom_heap = 0;
#endif
	heap->storage = &tmp_storage;
	heap->huge_list = NULL;
	memset(heap->free_slot, 0, sizeof(heap->free_slot));
	storage = _zend_mm_alloc(heap, sizeof(zend_mm_storage) + data_size ZEND_FILE_LINE_CC ZEND_FILE_LINE_CC);
	if (!storage) {
		handlers->chunk_free(&tmp_storage, chunk, ZEND_MM_CHUNK_SIZE);
#if ZEND_MM_ERROR
		fprintf(stderr, "Can't initialize heap\n");
#endif
		return NULL;
	}
	memcpy(storage, &tmp_storage, sizeof(zend_mm_storage));
	if (data) {
		storage->data = (void*)(((char*)storage + sizeof(zend_mm_storage)));
		memcpy(storage->data, data, data_size);
	}
	heap->storage = storage;
	return heap;
#else
	return NULL;
#endif
}

ZEND_API void * __zend_malloc(size_t len)
{
	void *tmp = malloc(len);
	if (EXPECTED(tmp || !len)) {
		return tmp;
	}
	zend_out_of_memory();
}

ZEND_API void * __zend_calloc(size_t nmemb, size_t len)
{
	void *tmp;

	len = zend_safe_address_guarded(nmemb, len, 0);
	tmp = __zend_malloc(len);
	memset(tmp, 0, len);
	return tmp;
}

ZEND_API void * __zend_realloc(void *p, size_t len)
{
	p = realloc(p, len);
	if (EXPECTED(p || !len)) {
		return p;
	}
	zend_out_of_memory();
}

ZEND_API char * __zend_strdup(const char *s)
{
	char *tmp = strdup(s);
	if (EXPECTED(tmp)) {
		return tmp;
	}
	zend_out_of_memory();
}

#ifdef ZTS
size_t zend_mm_globals_size(void)
{
	return sizeof(zend_alloc_globals);
}
#endif
