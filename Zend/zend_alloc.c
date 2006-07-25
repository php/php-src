/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2006 Zend Technologies Ltd. (http://www.zend.com) |
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
   |          Dmitry Sogov <dmitry@zend.com>                              |
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

#ifndef ZEND_USE_MALLOC_MM
# define ZEND_USE_MALLOC_MM ZEND_DEBUG
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

static void zend_mm_panic(const char *message)
{
	fprintf(stderr, "%s\n", message);
#if ZEND_DEBUG && defined(HAVE_KILL) && defined(HAVE_GETPID)
	kill(getpid(), SIGSEGV);
#else
	exit(1);
#endif
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

#if defined(HAVE_MEM_MMAP_ANON) || defined(HAVE_MEM_MMAP_ZERO)

static zend_mm_segment* zend_mm_mem_mmap_realloc(zend_mm_storage *storage, zend_mm_segment* segment, size_t size)
{
	zend_mm_segment *ret;
#ifdef HAVE_MREMAP
	ret = (zend_mm_segment*)mremap(segment, segment->size, size, MREMAP_MAYMOVE);
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
	munmap(segment, segment->size);
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

# define ZEND_MM_MEM_MMAP_ANON_DSC {"mmap_anon", zend_mm_mem_dummy_init, zend_mm_mem_dummy_dtor, zend_mm_mem_mmap_anon_alloc, zend_mm_mem_mmap_realloc, zend_mm_mem_mmap_free}

#endif

#ifdef HAVE_MEM_MMAP_ZERO

static int zend_mm_dev_zero_fd = -1;

static zend_mm_storage* zend_mm_mem_mmap_zero_init(void *params)
{
	if (zend_mm_dev_zero_fd != -1) {
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

# define ZEND_MM_MEM_MMAP_ZERO_DSC {"mmap_zero", zend_mm_mem_mmap_zero_init, zend_mm_mem_mmap_zero_dtor, zend_mm_mem_mmap_zero_alloc, zend_mm_mem_mmap_realloc, zend_mm_mem_mmap_free}

#endif

#ifdef HAVE_MEM_WIN32

static zend_mm_segment* zend_mm_mem_win32_alloc(zend_mm_storage *storage, size_t size)
{
	if (VirtualAlloc(0, size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE)) {
		return malloc(sizeof(zend_mm_storage));
	} else {
		return NULL;
	}
}

static void zend_mm_mem_win32_free(zend_mm_storage *storage, zend_mm_segment* segment)
{
	VirtualFree(segment, 0, MEM_RELEASE);
	free(storage);
}

static zend_mm_segment* zend_mm_mem_win32_realloc(zend_mm_storage *storage, zend_mm_segment* segment, size_t size)
{
	zend_mm_segment *ret = zend_mm_mem_win32_alloc(storage, size);
	if (ret) {
		memcpy(ret, segment, size > segment->size ? segment->size : size);
		zend_mm_mem_win32_free(storage, segment);
	}
	return ret;
}

# define ZEND_MM_MEM_WIN32_DSC {"win32", zend_mm_mem_dummy_init, zend_mm_mem_dummy_dtor, zend_mm_mem_win32_alloc, zend_mm_mem_win32_realloc, zend_mm_mem_win32_free}

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

# define ZEND_MM_MEM_MALLOC_DSC {"malloc", zend_mm_mem_dummy_init, zend_mm_mem_dummy_dtor, zend_mm_mem_malloc_alloc, zend_mm_mem_malloc_realloc, zend_mm_mem_malloc_free}

#endif

static const zend_mm_mem_handlers mem_handlers[] = {
#ifdef HAVE_MEM_MALLOC
	ZEND_MM_MEM_MALLOC_DSC,
#endif
#ifdef HAVE_MEM_MMAP_ANON
	ZEND_MM_MEM_MMAP_ANON_DSC,
#endif
#ifdef HAVE_MEM_MMAP_ZERO
	ZEND_MM_MEM_MMAP_ZERO_DSC,
#endif
#ifdef HAVE_MEM_WIN32
	ZEND_MM_MEM_WIN32_DSC,
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

typedef enum _mem_magic {
	MEM_BLOCK_VALID  = 0x7312F8DC,
	MEM_BLOCK_FREED  = 0x99954317,
	MEM_BLOCK_CACHED = 0xFB8277DC,
	MEM_BLOCK_GUARD  = 0x2A8FCC84,
	MEM_BLOCK_LEAK   = 0x6C5E8F2D
} mem_magic;

/* mm block type */
typedef struct _zend_mm_block_info {
	size_t _size;
	size_t _prev;
} zend_mm_block_info;

typedef struct _zend_mm_debug_info {
	char *filename;
	uint lineno;
	char *orig_filename;
	uint orig_lineno;
	size_t size;
} zend_mm_debug_info;

typedef struct _zend_mm_block {
	zend_mm_block_info info;
#if ZEND_DEBUG
	mem_magic magic;
	zend_mm_debug_info debug;
# ifdef ZTS
	THREAD_T thread_id;
# endif
#endif
} zend_mm_block;

typedef struct _zend_mm_free_block {
	zend_mm_block_info info;
#if ZEND_DEBUG
	mem_magic magic;
#endif
	struct _zend_mm_free_block *prev_free_block;
	struct _zend_mm_free_block *next_free_block;
} zend_mm_free_block;

#define ZEND_MM_NUM_BUCKETS 32

#define ZEND_MM_CACHE 1
#define ZEND_MM_CACHE_SIZE (32*1024)

struct _zend_mm_heap {
	unsigned int        free_bitmap;
	size_t              block_size;
	zend_mm_segment    *segments_list;
	zend_mm_storage    *storage;
	size_t				real_size;
#if MEMORY_LIMIT
	size_t              real_peak;
	size_t				limit;
	size_t              size;
	size_t              peak;
#endif
#if ZEND_USE_MALLOC_MM
	int					use_zend_alloc;
#endif
	int					overflow;
#if ZEND_MM_CACHE
	unsigned int        cached;
	zend_mm_free_block *cache[ZEND_MM_NUM_BUCKETS];
#endif
	zend_mm_free_block  free_buckets[ZEND_MM_NUM_BUCKETS];
};

#define ZEND_MM_TYPE_MASK				0x3L

#define ZEND_MM_FREE_BLOCK				0x0L
#define ZEND_MM_USED_BLOCK				0x1L
#define ZEND_MM_GUARD_BLOCK				0x3L

#define ZEND_MM_BLOCK(b, type, size)	do { \
											size_t _size = (size); \
											(b)->info._size = (type) | _size; \
											ZEND_MM_BLOCK_AT(b, _size)->info._prev = (type) | _size; \
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
#define ZEND_MM_PREV_BLOCK(b)			ZEND_MM_BLOCK_AT(b, -(int)((b)->info._prev & ~ZEND_MM_TYPE_MASK))

#define ZEND_MM_PREV_BLOCK_IS_FREE(b)	(!((b)->info._prev & ZEND_MM_USED_BLOCK))

#define ZEND_MM_MARK_FIRST_BLOCK(b)		((b)->info._prev = ZEND_MM_GUARD_BLOCK)
#define ZEND_MM_IS_FIRST_BLOCK(b)		((b)->info._prev == ZEND_MM_GUARD_BLOCK)

/* optimized access */
#define ZEND_MM_FREE_BLOCK_SIZE(b)		(b)->info._size

#ifndef ZEND_MM_ALIGNMENT
# define ZEND_MM_ALIGNMENT 8
# define ZEND_MM_ALIGNMENT_LOG2 3
#endif

#define ZEND_MM_ALIGNMENT_MASK ~(ZEND_MM_ALIGNMENT-1)

/* Aligned header size */
#define ZEND_MM_ALIGNED_SIZE(size)			((size + ZEND_MM_ALIGNMENT - 1) & ZEND_MM_ALIGNMENT_MASK)
#define ZEND_MM_ALIGNED_HEADER_SIZE			ZEND_MM_ALIGNED_SIZE(sizeof(zend_mm_block))
#define ZEND_MM_ALIGNED_FREE_HEADER_SIZE	ZEND_MM_ALIGNED_SIZE(sizeof(zend_mm_free_block))
#define ZEND_MM_ALIGNED_MIN_HEADER_SIZE		(sizeof(zend_mm_block)+END_MAGIC_SIZE>sizeof(zend_mm_free_block)?ZEND_MM_ALIGNED_SIZE(sizeof(zend_mm_block)+END_MAGIC_SIZE):ZEND_MM_ALIGNED_SIZE(sizeof(zend_mm_free_block)))
#define ZEND_MM_ALIGNED_SEGMENT_SIZE		ZEND_MM_ALIGNED_SIZE(sizeof(zend_mm_segment))

#define ZEND_MM_MIN_SIZE					(ZEND_MM_ALIGNED_MIN_HEADER_SIZE-(ZEND_MM_ALIGNED_HEADER_SIZE+END_MAGIC_SIZE))

#define ZEND_MM_MAX_SMALL_SIZE				(((ZEND_MM_NUM_BUCKETS-1)<<ZEND_MM_ALIGNMENT_LOG2)+ZEND_MM_ALIGNED_MIN_HEADER_SIZE)

#define ZEND_MM_TRUE_SIZE(size)				(((long)size<(long)ZEND_MM_MIN_SIZE)?(ZEND_MM_ALIGNED_MIN_HEADER_SIZE):(ZEND_MM_ALIGNED_SIZE(size+ZEND_MM_ALIGNED_HEADER_SIZE+END_MAGIC_SIZE)))

#define ZEND_MM_BUCKET_INDEX(true_size)		((true_size>>ZEND_MM_ALIGNMENT_LOG2)-(ZEND_MM_ALIGNED_MIN_HEADER_SIZE>>ZEND_MM_ALIGNMENT_LOG2)+1)

#define ZEND_MM_SMALL_SIZE(true_size)		(true_size < ZEND_MM_MAX_SMALL_SIZE)

/* Memory calculations */
#define ZEND_MM_BLOCK_AT(blk, offset)	((zend_mm_block *) (((char *) (blk))+(offset)))
#define ZEND_MM_DATA_OF(p)				((void *) (((char *) (p))+ZEND_MM_ALIGNED_HEADER_SIZE))
#define ZEND_MM_HEADER_OF(blk)			ZEND_MM_BLOCK_AT(blk, -(int)ZEND_MM_ALIGNED_HEADER_SIZE)

/* Debug output */
#if ZEND_DEBUG

# ifdef ZTS
#  define ZEND_MM_BAD_THREAD_ID(block) ((block)->thread_id != tsrm_thread_id())
# else
#  define ZEND_MM_BAD_THREAD_ID(block) 0
# endif

# define ZEND_MM_VALID_PTR(block) \
	zend_mm_check_ptr(heap, block, 1 ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC)

# define ZEND_MM_SET_MAGIC(block, val) do { \
		(block)->magic = (val); \
	} while (0)

# define ZEND_MM_CHECK_MAGIC(block, val) do { \
		if ((block)->magic != (val)) { \
			zend_mm_panic("Heap corrupted"); \
		} \
	} while (0)

# define ZEND_MM_END_MAGIC_PTR(block) \
	(long*)(((char*)(ZEND_MM_DATA_OF(block))) + ((zend_mm_block*)(block))->debug.size)

# define ZEND_MM_SET_END_MAGIC(block) do { \
		long *p = ZEND_MM_END_MAGIC_PTR(block); \
		*p = MEM_BLOCK_END_MAGIC; \
	} while (0)

# define MEM_BLOCK_END_MAGIC 0x2A8FCC84L

# define END_MAGIC_SIZE sizeof(long)

#else

# define ZEND_MM_VALID_PTR(ptr) 1

# define ZEND_MM_SET_MAGIC(block, val)

# define ZEND_MM_SET_END_MAGIC(block)

# define ZEND_MM_CHECK_MAGIC(block, val)

# define END_MAGIC_SIZE 0

#endif

static inline void zend_mm_add_to_free_list(zend_mm_heap *heap, zend_mm_free_block *mm_block)
{
	zend_mm_free_block *prev, *next;
	size_t size;

	ZEND_MM_SET_MAGIC(mm_block, MEM_BLOCK_FREED);

	size = ZEND_MM_FREE_BLOCK_SIZE(mm_block);
	if (ZEND_MM_SMALL_SIZE(size)) {
		size_t index = ZEND_MM_BUCKET_INDEX(size);
		prev = &heap->free_buckets[index];

		if (prev->prev_free_block == prev) {
			heap->free_bitmap |= (1U << index);
		}
	} else {
		prev = &heap->free_buckets[0];
	}
	next = prev->next_free_block;
	mm_block->prev_free_block = prev;
	mm_block->next_free_block = next;
	prev->next_free_block = mm_block;
	next->prev_free_block = mm_block;
}


static inline void zend_mm_remove_from_free_list(zend_mm_heap *heap, zend_mm_free_block *mm_block)
{
	zend_mm_free_block *prev, *next;

	ZEND_MM_CHECK_MAGIC(mm_block, MEM_BLOCK_FREED);

	prev = mm_block->prev_free_block;
	next = mm_block->next_free_block;
	prev->next_free_block = next;
	next->prev_free_block = prev;

	if (prev == next) {
		size_t size = ZEND_MM_FREE_BLOCK_SIZE(mm_block);

		if (ZEND_MM_SMALL_SIZE(size)) {
			size_t index = ZEND_MM_BUCKET_INDEX(size);

			heap->free_bitmap &= ~(1U << index);
		}
	}
}

static inline void zend_mm_init(zend_mm_heap *heap)
{
	int i;

	heap->free_bitmap = 0;
#if ZEND_MM_CACHE
	heap->cached = 0;
	memset(heap->cache, 0, sizeof(heap->cache));
#endif
	for (i = 0; i < ZEND_MM_NUM_BUCKETS; i++) {
		heap->free_buckets[i].next_free_block =
		heap->free_buckets[i].prev_free_block = &heap->free_buckets[i];
	}
}

static void zend_mm_del_segment(zend_mm_heap *heap, zend_mm_segment *segment)
{
	if (heap->segments_list == segment) {
		heap->segments_list = segment->next_segment;
	} else {
		zend_mm_segment *p = heap->segments_list;

		while (p) {
			if (p->next_segment == segment) {
				p->next_segment = segment->next_segment;
				break;
			}
			p = p->next_segment;
		}
	}
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
				zend_mm_block *prev_block = ZEND_MM_PREV_BLOCK(mm_block);
				zend_mm_block *next_block = ZEND_MM_NEXT_BLOCK(mm_block);

				heap->cached -= size;

				if (ZEND_MM_IS_FREE_BLOCK(prev_block)) {
					size += ZEND_MM_FREE_BLOCK_SIZE(prev_block);
					mm_block = (zend_mm_free_block*)prev_block;
					zend_mm_remove_from_free_list(heap, (zend_mm_free_block *) prev_block);
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
		}
	}
}
#endif

/* Notes:
 * - This function may alter the block_sizes values to match platform alignment
 * - This function does *not* perform sanity checks on the arguments
 */
ZEND_API zend_mm_heap *zend_mm_startup_ex(const zend_mm_mem_handlers *handlers, size_t block_size, void *params)
{
	zend_mm_storage *storage;
	zend_mm_heap    *heap;

#if 0
	for (i = 0; i < ZEND_MM_MAX_SMALL_SIZE; i++) {
		printf("%3d%c: %3ld %d %2ld\n", i, (i == ZEND_MM_MIN_SIZE?'*':' '), (long)ZEND_MM_TRUE_SIZE(i), ZEND_MM_SMALL_SIZE(ZEND_MM_TRUE_SIZE(i)), (long)ZEND_MM_BUCKET_INDEX(ZEND_MM_TRUE_SIZE(i)));
	}
	exit(0);
#endif

	storage = handlers->init(params);
	if (!storage) {
		fprintf(stderr, "Cannot initialize zend_mm storage\n");
		exit(255);
	}
	storage->handlers = handlers;

	heap = malloc(sizeof(struct _zend_mm_heap));

	heap->storage = storage;
	heap->block_size = block_size = ZEND_MM_ALIGNED_SIZE(block_size);
	heap->segments_list = NULL;
	zend_mm_init(heap);

#if ZEND_USE_MALLOC_MM
	heap->use_zend_alloc = 1;
#endif

	heap->real_size = 0;
#if MEMORY_LIMIT
	heap->real_peak = 0;
	heap->limit = 1<<30;
	heap->size = 0;
	heap->peak = 0;
#endif

	heap->overflow = 0;

	return heap;
}

ZEND_API zend_mm_heap *zend_mm_startup(void)
{
	int i;
	size_t seg_size;
	char *mem_type = getenv("ZEND_MM_MEM_TYPE");
	char *tmp;
	const zend_mm_mem_handlers *handlers;

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
			for (i = 0; mem_handlers[i].name; i++) {
				fprintf(stderr, "    '%s'\n", mem_handlers[i].name);
			}
			exit(255);
		}
	}
	handlers = &mem_handlers[i];

	tmp = getenv("ZEND_MM_SEG_SIZE");
	if (tmp) {
		seg_size = zend_atoi(tmp, 0);
	} else {
		seg_size = 256 * 1024;
	}

	return zend_mm_startup_ex(handlers, seg_size, NULL);
}

#if ZEND_DEBUG
static long zend_mm_find_leaks(zend_mm_segment *segment, zend_mm_block *b)
{
	long leaks = 0;
	zend_mm_block *p, *q;

	p = ZEND_MM_NEXT_BLOCK(b);
	while (1) {
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
		if (ZEND_MM_IS_GUARD_BLOCK(q)) {
			ZEND_MM_CHECK_MAGIC(q, MEM_BLOCK_GUARD);
			segment = segment->next_segment;
			if (!segment) {
				break;
			}
			q = (zend_mm_block *) ((char *) segment + ZEND_MM_ALIGNED_SEGMENT_SIZE);
		}
		p = q;
	}
	return leaks;
}

static void zend_mm_check_leaks(zend_mm_heap *heap)
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

				zend_message_dispatcher(ZMSG_LOG_SCRIPT_NAME, NULL);
				zend_message_dispatcher(ZMSG_MEMORY_LEAK_DETECTED, &leak);
				repeated = zend_mm_find_leaks(segment, p);
				total += 1 + repeated;
				if (repeated) {
					zend_message_dispatcher(ZMSG_MEMORY_LEAK_REPEATED, (void *)repeated);
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
		zend_message_dispatcher(ZMSG_MEMORY_LEAKS_GRAND_TOTAL, &total);
	}
}

static int zend_mm_check_ptr(zend_mm_heap *heap, void *ptr, int silent ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	zend_mm_block *p;
	int no_cache_notice = 0;
	int had_problems = 0;
	int valid_beginning = 1;
	long *end_magic;

	if (silent==2) {
		silent = 1;
		no_cache_notice = 1;
	} else if (silent==3) {
		silent = 0;
		no_cache_notice = 1;
	}
	if (!silent) {
		zend_message_dispatcher(ZMSG_LOG_SCRIPT_NAME, NULL);
		zend_debug_alloc_output("---------------------------------------\n");
		zend_debug_alloc_output("%s(%d) : Block 0x%0.8lX status:\n" ZEND_FILE_LINE_RELAY_CC, (long) ptr);
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
			zend_debug_alloc_output("Invalid pinter: ((thread_id=0x%0.8X) != (expected=0x%0.8X))\n", (long)p->thread_id, (long)tsrm_thread_id());
			had_problems = 1;
		} else {
			return zend_mm_check_ptr(heap, ptr, 0 ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
		}
	}
#endif

	if (p->info._size != ZEND_MM_NEXT_BLOCK(p)->info._prev) {
		if (!silent) {
			zend_debug_alloc_output("Invalid pointer: ((size=0x%0.8X) != (next.prev=0x%0.8X))\n", p->info._size, ZEND_MM_NEXT_BLOCK(p)->info._prev);
			had_problems = 1;
		} else {
			return zend_mm_check_ptr(heap, ptr, 0 ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
		}
	}
	if (p->info._prev != ZEND_MM_GUARD_BLOCK &&
	    ZEND_MM_PREV_BLOCK(p)->info._size != p->info._prev) {
		if (!silent) {
			zend_debug_alloc_output("Invalid pointer: ((prev=0x%0.8X) != (prev.size=0x%0.8X))\n", p->info._prev, ZEND_MM_PREV_BLOCK(p)->info._size);
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

	end_magic = ZEND_MM_END_MAGIC_PTR(p);
	if (!valid_beginning) {
		if (!silent) {
			zend_debug_alloc_output("%10s\t", "End:");
			zend_debug_alloc_output("Unknown\n");
		}
	} else if (*end_magic == MEM_BLOCK_END_MAGIC) {
		if (!silent) {
			zend_debug_alloc_output("%10s\t", "End:");
			zend_debug_alloc_output("OK\n");
		}
	} else {
		static long mem_block_end_magic = MEM_BLOCK_END_MAGIC;
		char *overflow_ptr, *magic_ptr=(char *) &mem_block_end_magic;
		int overflows=0;
		long i;

		if (silent) {
			return _mem_block_check(ptr, 0 ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
		}
		had_problems = 1;
		overflow_ptr = (char *) end_magic;

		for (i=0; i<(int)sizeof(long); i++) {
			if (overflow_ptr[i]!=magic_ptr[i]) {
				overflows++;
			}
		}

		zend_debug_alloc_output("%10s\t", "End:");
		zend_debug_alloc_output("Overflown (magic=0x%0.8lX instead of 0x%0.8lX)\n", *end_magic, MEM_BLOCK_END_MAGIC);
		zend_debug_alloc_output("%10s\t","");
		if (overflows>=(int)sizeof(long)) {
			zend_debug_alloc_output("At least %d bytes overflown\n", sizeof(long));
		} else {
			zend_debug_alloc_output("%d byte(s) overflown\n", overflows);
		}
	}

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

ZEND_API void zend_mm_shutdown(zend_mm_heap *heap, int full_shutdown, int silent)
{
	zend_mm_segment *segment;
	zend_mm_segment *prev;

#if ZEND_DEBUG
	if (!silent) {
		zend_mm_check_leaks(heap);
	}
#endif

	segment = heap->segments_list;
	while (segment) {
		prev = segment;
		segment = segment->next_segment;
		ZEND_MM_STORAGE_FREE(prev);
	}
	if (full_shutdown) {
		ZEND_MM_STORAGE_DTOR();
		free(heap);
	} else {
		heap->segments_list = NULL;
		zend_mm_init(heap);
		heap->real_size = 0;
#if MEMORY_LIMIT
		heap->real_peak = 0;
		heap->size = 0;
		heap->peak = 0;
#endif
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
	if (heap->overflow == 0) {
		char *error_filename;
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
			zend_error(E_ERROR,
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
		} zend_end_try();
	} else {
		heap->overflow = 2;
	}
	zend_bailout();
}

static void *_zend_mm_alloc_int(zend_mm_heap *heap, size_t size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC) ZEND_ATTRIBUTE_MALLOC;

static void *_zend_mm_alloc_int(zend_mm_heap *heap, size_t size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	size_t true_size, best_size = 0x7fffffff;
	zend_mm_free_block *p, *end, *best_fit = NULL;

	true_size = ZEND_MM_TRUE_SIZE(size);

	if (ZEND_MM_SMALL_SIZE(true_size)) {
		size_t index = ZEND_MM_BUCKET_INDEX(true_size);
		unsigned int bitmap;

#if ZEND_MM_CACHE
		if (heap->cache[index]) {
			/* Get block from cache */
			best_fit = heap->cache[index];
			heap->cache[index] = best_fit->prev_free_block;
			heap->cached -= true_size;
#if ZEND_DEBUG
			ZEND_MM_CHECK_MAGIC(best_fit, MEM_BLOCK_CACHED);
			ZEND_MM_SET_MAGIC(best_fit, MEM_BLOCK_VALID);
			((zend_mm_block*)best_fit)->debug.size = size;
			((zend_mm_block*)best_fit)->debug.filename = __zend_filename;
			((zend_mm_block*)best_fit)->debug.lineno = __zend_lineno;
			((zend_mm_block*)best_fit)->debug.orig_filename = __zend_orig_filename;
			((zend_mm_block*)best_fit)->debug.orig_lineno = __zend_orig_lineno;
			ZEND_MM_SET_END_MAGIC(best_fit);
#endif
			return ZEND_MM_DATA_OF(best_fit);
		}
#endif

		bitmap = heap->free_bitmap >> index;
		if (bitmap) {
			/* Found some "small" free block that can be used */
			if (bitmap & 1) {
				/* Found "small" free block of exactly the same size */
				best_fit = heap->free_buckets[index].next_free_block;
				goto zend_mm_finished_searching_for_block;
			} else {
				/* Search for bigger "small" block */
				static const int offset[16] = {4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0};
				int n;

				/* Unrolled loop that search for best "small" block */
				do {
					n = offset[bitmap & 15];
					bitmap >>= 4;
					index += n;
				} while (n == 4);

				best_fit = heap->free_buckets[index].next_free_block;
				goto zend_mm_finished_searching_for_block;
			}
		}
	}

	end = &heap->free_buckets[0];
	for (p = end->next_free_block; p != end; p = p->next_free_block) {
		size_t s = ZEND_MM_FREE_BLOCK_SIZE(p);
		if (s > true_size) {
			if (s < best_size) {	/* better fit */
				best_fit = p;
				best_size = s;
			}
		} else if (s == true_size) {
			/* Found "big" free block of exactly the same size */
			best_fit = p;
			goto zend_mm_finished_searching_for_block;
		}
	}

	if (best_fit) {
zend_mm_finished_searching_for_block:
		/* remove from free list */
		HANDLE_BLOCK_INTERRUPTIONS();
#if ZEND_DEBUG
		ZEND_MM_CHECK_MAGIC(best_fit, MEM_BLOCK_FREED);
#endif
		zend_mm_remove_from_free_list(heap, best_fit);

		{
			size_t block_size = ZEND_MM_FREE_BLOCK_SIZE(best_fit);
			size_t remaining_size = block_size - true_size;

			if (remaining_size < ZEND_MM_ALIGNED_MIN_HEADER_SIZE) {
				ZEND_MM_BLOCK(best_fit, ZEND_MM_USED_BLOCK, block_size);
			} else {
				zend_mm_free_block *new_free_block;

				/* prepare new free block */
				ZEND_MM_BLOCK(best_fit, ZEND_MM_USED_BLOCK, true_size);
				new_free_block = (zend_mm_free_block *) ZEND_MM_BLOCK_AT(best_fit, true_size);
				ZEND_MM_BLOCK(new_free_block, ZEND_MM_FREE_BLOCK, remaining_size);

				/* add the new free block to the free list */
				zend_mm_add_to_free_list(heap, new_free_block);
			}
		}
	} else {
		size_t segment_size;
		zend_mm_segment *segment;
		zend_mm_block *next_block;

		if (true_size + ZEND_MM_ALIGNED_SEGMENT_SIZE + ZEND_MM_ALIGNED_HEADER_SIZE > heap->block_size) {
			/* Make sure we add a memory block which is big enough */
			segment_size = true_size + ZEND_MM_ALIGNED_SEGMENT_SIZE + ZEND_MM_ALIGNED_HEADER_SIZE;
			segment_size = ((segment_size + (heap->block_size-1)) / heap->block_size) * heap->block_size;
		} else {
			segment_size = heap->block_size;
		}


#if MEMORY_LIMIT
		if (heap->real_size + segment_size > heap->limit) {
			/* Memory limit overflow */
#if ZEND_DEBUG
			zend_mm_safe_error(heap, "Allowed memory size of %d bytes exhausted at %s:%d (tried to allocate %d bytes)", heap->limit, __zend_filename, __zend_lineno, size);
#else
			zend_mm_safe_error(heap, "Allowed memory size of %d bytes exhausted (tried to allocate %d bytes)", heap->limit, size);
#endif
		}
#endif

		HANDLE_BLOCK_INTERRUPTIONS();

		segment = (zend_mm_segment *) ZEND_MM_STORAGE_ALLOC(segment_size);

		if (!segment) {
			/* Storage manager cannot allocate memory */
#if ZEND_MM_CACHE
			zend_mm_free_cache(heap);
#endif
			HANDLE_UNBLOCK_INTERRUPTIONS();
#if ZEND_DEBUG
			zend_mm_safe_error(heap, "Out of memory (allocated %d) at %s:%d (tried to allocate %d bytes)", heap->real_size, __zend_filename, __zend_lineno, size);
#else
			zend_mm_safe_error(heap, "Out of memory (allocated %d) (tried to allocate %d bytes)", heap->real_size, size);
#endif
			return NULL;
		}

		heap->real_size += segment_size;
#if MEMORY_LIMIT
		if (heap->real_size > heap->real_peak) {
			heap->real_peak = heap->real_size;
		}
#endif

		segment->size = segment_size;
		segment->next_segment = heap->segments_list;
		heap->segments_list = segment;

		best_fit = (zend_mm_free_block *) ((char *) segment + ZEND_MM_ALIGNED_SEGMENT_SIZE);

		ZEND_MM_BLOCK(best_fit, ZEND_MM_USED_BLOCK, true_size);
		ZEND_MM_MARK_FIRST_BLOCK(best_fit);

		next_block = ZEND_MM_NEXT_BLOCK(best_fit);
		if (segment_size > (true_size+ZEND_MM_ALIGNED_SEGMENT_SIZE+ZEND_MM_ALIGNED_HEADER_SIZE)) {
			/* setup free block */
			ZEND_MM_BLOCK(next_block, ZEND_MM_FREE_BLOCK, segment_size - (true_size+ZEND_MM_ALIGNED_SEGMENT_SIZE+ZEND_MM_ALIGNED_HEADER_SIZE));
			zend_mm_add_to_free_list(heap, (zend_mm_free_block *) next_block);
			next_block = ZEND_MM_NEXT_BLOCK(next_block);
		}
		/* setup guard block */
		ZEND_MM_LAST_BLOCK(next_block);
	}

#if ZEND_DEBUG
	ZEND_MM_SET_MAGIC(best_fit, MEM_BLOCK_VALID);
	((zend_mm_block*)best_fit)->debug.size = size;
	((zend_mm_block*)best_fit)->debug.filename = __zend_filename;
	((zend_mm_block*)best_fit)->debug.lineno = __zend_lineno;
	((zend_mm_block*)best_fit)->debug.orig_filename = __zend_orig_filename;
	((zend_mm_block*)best_fit)->debug.orig_lineno = __zend_orig_lineno;
# ifdef ZTS
	((zend_mm_block*)best_fit)->thread_id = tsrm_thread_id();
# endif
	ZEND_MM_SET_END_MAGIC(best_fit);
#endif

#if MEMORY_LIMIT
	heap->size += true_size;
	if (heap->peak < heap->size) {
		heap->peak = heap->size;
	}
#endif

	HANDLE_UNBLOCK_INTERRUPTIONS();

	return ZEND_MM_DATA_OF(best_fit);
}


static void _zend_mm_free_int(zend_mm_heap *heap, void *p ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	zend_mm_block *mm_block;
	zend_mm_block *prev_block, *next_block;
	size_t size;

	if (!ZEND_MM_VALID_PTR(p)) {
		return;
	}
	mm_block = ZEND_MM_HEADER_OF(p);
	size = ZEND_MM_BLOCK_SIZE(mm_block);

#if ZEND_DEBUG
	memset(ZEND_MM_DATA_OF(mm_block), 0x5a, mm_block->debug.size);
#endif

#if ZEND_MM_CACHE
	if (ZEND_MM_SMALL_SIZE(size) && heap->cached < ZEND_MM_CACHE_SIZE) {
		size_t index = ZEND_MM_BUCKET_INDEX(size);
		zend_mm_free_block **cache = &heap->cache[index];

		((zend_mm_free_block*)mm_block)->prev_free_block = *cache;
		*cache = (zend_mm_free_block*)mm_block;
		heap->cached += size;
		ZEND_MM_SET_MAGIC(mm_block, MEM_BLOCK_CACHED);
		return;
	}
#endif

	HANDLE_BLOCK_INTERRUPTIONS();

#if MEMORY_LIMIT
	heap->size -= size;
#endif

	if (ZEND_MM_PREV_BLOCK_IS_FREE(mm_block)) {
		next_block = ZEND_MM_NEXT_BLOCK(mm_block);
		if (ZEND_MM_IS_FREE_BLOCK(next_block)) {
		    /* merge with previous and next block */
			zend_mm_remove_from_free_list(heap, (zend_mm_free_block *) next_block);
			prev_block=ZEND_MM_PREV_BLOCK(mm_block);
			if (ZEND_MM_SMALL_SIZE(ZEND_MM_FREE_BLOCK_SIZE(prev_block))) {
				zend_mm_remove_from_free_list(heap, (zend_mm_free_block *) prev_block);
				size += ZEND_MM_FREE_BLOCK_SIZE(prev_block) + ZEND_MM_FREE_BLOCK_SIZE(next_block);
				mm_block = prev_block;
			} else {
				ZEND_MM_BLOCK(prev_block, ZEND_MM_FREE_BLOCK, size + ZEND_MM_FREE_BLOCK_SIZE(prev_block) + ZEND_MM_FREE_BLOCK_SIZE(next_block));
				if (ZEND_MM_IS_FIRST_BLOCK(prev_block) &&
				    ZEND_MM_IS_GUARD_BLOCK(ZEND_MM_NEXT_BLOCK(prev_block))) {
					mm_block = prev_block;
					zend_mm_remove_from_free_list(heap, (zend_mm_free_block *) mm_block);
					goto free_segment;
				}
				HANDLE_UNBLOCK_INTERRUPTIONS();
				return;
			}
		} else {
		    /* merge with previous block */
			prev_block=ZEND_MM_PREV_BLOCK(mm_block);
			if (ZEND_MM_SMALL_SIZE(ZEND_MM_FREE_BLOCK_SIZE(prev_block))) {
				size += ZEND_MM_FREE_BLOCK_SIZE(prev_block);
				zend_mm_remove_from_free_list(heap, (zend_mm_free_block *) prev_block);
				mm_block = prev_block;
		   	} else {
				ZEND_MM_BLOCK(prev_block, ZEND_MM_FREE_BLOCK, size + ZEND_MM_FREE_BLOCK_SIZE(prev_block));
				if (ZEND_MM_IS_FIRST_BLOCK(prev_block) &&
				    ZEND_MM_IS_GUARD_BLOCK(ZEND_MM_NEXT_BLOCK(prev_block))) {
					mm_block = prev_block;
					zend_mm_remove_from_free_list(heap, (zend_mm_free_block *) mm_block);
					goto free_segment;
				}
				HANDLE_UNBLOCK_INTERRUPTIONS();
				return;
		   	}
		}
	} else {
		next_block = ZEND_MM_NEXT_BLOCK(mm_block);
		if (ZEND_MM_IS_FREE_BLOCK(next_block)) {
			/* merge with the next block */
			zend_mm_remove_from_free_list(heap, (zend_mm_free_block *) next_block);
			size += ZEND_MM_FREE_BLOCK_SIZE(next_block);
		}
	}
	ZEND_MM_BLOCK(mm_block, ZEND_MM_FREE_BLOCK, size);
	if (ZEND_MM_IS_FIRST_BLOCK(mm_block) &&
	    ZEND_MM_IS_GUARD_BLOCK(ZEND_MM_NEXT_BLOCK(mm_block))) {
free_segment:
		zend_mm_del_segment(heap, (zend_mm_segment *) ((char *)mm_block - ZEND_MM_ALIGNED_SEGMENT_SIZE));
	} else {
		zend_mm_add_to_free_list(heap, (zend_mm_free_block *) mm_block);
	}
	HANDLE_UNBLOCK_INTERRUPTIONS();
}

static void *_zend_mm_realloc_int(zend_mm_heap *heap, void *p, size_t size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	zend_mm_block *mm_block = ZEND_MM_HEADER_OF(p);
	zend_mm_block *next_block;
	size_t true_size;
	void *ptr;

	if (!p || !ZEND_MM_VALID_PTR(p)) {
		return _zend_mm_alloc_int(heap, size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
	}
	mm_block = ZEND_MM_HEADER_OF(p);
	true_size = ZEND_MM_TRUE_SIZE(size);

#if MEMORY_LIMIT
	heap->size = heap->size + true_size - ZEND_MM_BLOCK_SIZE(mm_block);
	if (heap->peak < heap->size) {
		heap->peak = heap->size;
	}
#endif
	
	if (true_size <= ZEND_MM_BLOCK_SIZE(mm_block)) {
		size_t remaining_size = ZEND_MM_BLOCK_SIZE(mm_block) - true_size;

		if (remaining_size >= ZEND_MM_ALIGNED_MIN_HEADER_SIZE) {
			zend_mm_free_block *new_free_block;

			HANDLE_BLOCK_INTERRUPTIONS();
			next_block = ZEND_MM_NEXT_BLOCK(mm_block);
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
			HANDLE_UNBLOCK_INTERRUPTIONS();
		}
#if ZEND_DEBUG
		mm_block->debug.size = size;
		mm_block->debug.filename = __zend_filename;
		mm_block->debug.lineno = __zend_lineno;
		mm_block->debug.orig_filename = __zend_orig_filename;
		mm_block->debug.orig_lineno = __zend_orig_lineno;
		ZEND_MM_SET_END_MAGIC(mm_block);
#endif
		return p;
	}

	next_block = ZEND_MM_NEXT_BLOCK(mm_block);

	if (ZEND_MM_IS_FREE_BLOCK(next_block)) {
		if (ZEND_MM_BLOCK_SIZE(mm_block) + ZEND_MM_FREE_BLOCK_SIZE(next_block) >= true_size) {
			size_t block_size = ZEND_MM_BLOCK_SIZE(mm_block) + ZEND_MM_FREE_BLOCK_SIZE(next_block);
			size_t remaining_size = block_size - true_size;

			HANDLE_BLOCK_INTERRUPTIONS();
			zend_mm_remove_from_free_list(heap, (zend_mm_free_block *) next_block);

			if (remaining_size < ZEND_MM_ALIGNED_MIN_HEADER_SIZE) {
				ZEND_MM_BLOCK(mm_block, ZEND_MM_USED_BLOCK, block_size);
			} else {
				zend_mm_free_block *new_free_block;

				/* prepare new free block */
				ZEND_MM_BLOCK(mm_block, ZEND_MM_USED_BLOCK, true_size);
				new_free_block = (zend_mm_free_block *) ZEND_MM_BLOCK_AT(mm_block, true_size);
				ZEND_MM_BLOCK(new_free_block, ZEND_MM_FREE_BLOCK, remaining_size);

				/* add the new free block to the free list */
				zend_mm_add_to_free_list(heap, new_free_block);
			}
#if ZEND_DEBUG
			mm_block->debug.size = size;
			mm_block->debug.filename = __zend_filename;
			mm_block->debug.lineno = __zend_lineno;
			mm_block->debug.orig_filename = __zend_orig_filename;
			mm_block->debug.orig_lineno = __zend_orig_lineno;
			ZEND_MM_SET_END_MAGIC(mm_block);
#endif
			HANDLE_UNBLOCK_INTERRUPTIONS();
			return p;
		} else if (ZEND_MM_IS_FIRST_BLOCK(mm_block) &&
				   ZEND_MM_IS_GUARD_BLOCK(ZEND_MM_NEXT_BLOCK(next_block))) {
			HANDLE_BLOCK_INTERRUPTIONS();
			zend_mm_remove_from_free_list(heap, (zend_mm_free_block *) next_block);
			goto realloc_segment;
		}
	} else if (ZEND_MM_IS_FIRST_BLOCK(mm_block) && ZEND_MM_IS_GUARD_BLOCK(next_block)) {
		zend_mm_segment *segment;
		zend_mm_segment *segment_copy;
		size_t segment_size;

		HANDLE_BLOCK_INTERRUPTIONS();
realloc_segment:
		/* segment size, size of block and size of guard block */
		if (true_size+ZEND_MM_ALIGNED_SEGMENT_SIZE+ZEND_MM_ALIGNED_HEADER_SIZE > heap->block_size) {
			segment_size = true_size+ZEND_MM_ALIGNED_SEGMENT_SIZE+ZEND_MM_ALIGNED_HEADER_SIZE;
			segment_size = ((segment_size + (heap->block_size-1)) / heap->block_size) * heap->block_size;
		} else {
			segment_size = heap->block_size;
		}

		segment_copy = (zend_mm_segment *) ((char *)mm_block - ZEND_MM_ALIGNED_SEGMENT_SIZE);
#if MEMORY_LIMIT
		if (heap->real_size + segment_size - segment_copy->size > heap->limit) {
			HANDLE_UNBLOCK_INTERRUPTIONS();
#if ZEND_DEBUG
			zend_mm_safe_error(heap, "Allowed memory size of %d bytes exhausted at %s:%d (tried to allocate %d bytes)", heap->limit, __zend_filename, __zend_lineno, size);
#else
			zend_mm_safe_error(heap, "Allowed memory size of %d bytes exhausted (tried to allocate %d bytes)", heap->limit, size);
#endif
			return NULL;
		}
#endif
		segment = ZEND_MM_STORAGE_REALLOC(segment_copy, segment_size);
		if (!segment) {
			HANDLE_UNBLOCK_INTERRUPTIONS();
#if ZEND_DEBUG
			zend_mm_safe_error(heap, "Out of memory (allocated %d) at %s:%d (tried to allocate %d bytes)", heap->real_size, __zend_filename, __zend_lineno, size);
#else
			zend_mm_safe_error(heap, "Out of memory (allocated %d) (tried to allocate %d bytes)", heap->real_size, size);
#endif
			return NULL;
		}
		heap->real_size += segment_size - segment->size;
#if MEMORY_LIMIT
		if (heap->real_size > heap->real_peak) {
			heap->real_peak = heap->real_size;
		}
#endif
		segment->size = segment_size;

		if (segment != segment_copy) {
			if (heap->segments_list == segment_copy) {
				heap->segments_list = segment;
			} else {
				zend_mm_segment *seg = heap->segments_list;

				while (seg) {
					if (seg->next_segment == segment_copy) {
						seg->next_segment = segment;
						break;
					}
					seg = seg->next_segment;
				}
			}
			mm_block = (zend_mm_block *) ((char *) segment + ZEND_MM_ALIGNED_SEGMENT_SIZE);
		}

		ZEND_MM_BLOCK(mm_block, ZEND_MM_USED_BLOCK, true_size);
		ZEND_MM_MARK_FIRST_BLOCK(mm_block);

		next_block = ZEND_MM_NEXT_BLOCK(mm_block);
		if (segment_size > (true_size+ZEND_MM_ALIGNED_SEGMENT_SIZE+ZEND_MM_ALIGNED_HEADER_SIZE)) {
			/* setup free block */
			ZEND_MM_BLOCK(next_block, ZEND_MM_FREE_BLOCK, segment_size - (ZEND_MM_ALIGNED_SEGMENT_SIZE+true_size+ZEND_MM_ALIGNED_HEADER_SIZE));
			zend_mm_add_to_free_list(heap, (zend_mm_free_block *) next_block);
			next_block = ZEND_MM_NEXT_BLOCK(next_block);
		}
		ZEND_MM_LAST_BLOCK(next_block);

#if ZEND_DEBUG
		ZEND_MM_SET_MAGIC(mm_block, MEM_BLOCK_VALID);
		mm_block->debug.size = size;
		mm_block->debug.filename = __zend_filename;
		mm_block->debug.lineno = __zend_lineno;
		mm_block->debug.orig_filename = __zend_orig_filename;
		mm_block->debug.orig_lineno = __zend_orig_lineno;
# ifdef ZTS
		mm_block->thread_id = tsrm_thread_id();
# endif
		ZEND_MM_SET_END_MAGIC(mm_block);
#endif
		HANDLE_UNBLOCK_INTERRUPTIONS();
		return ZEND_MM_DATA_OF(mm_block);
	}

	ptr = _zend_mm_alloc_int(heap, size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
#if ZEND_DEBUG
	memcpy(ptr, p, mm_block->debug.size);
#else
	memcpy(ptr, p, ZEND_MM_BLOCK_SIZE(mm_block) - ZEND_MM_ALIGNED_HEADER_SIZE);
#endif
	_zend_mm_free_int(heap, p ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
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
#if ZEND_DEBUG
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

ZEND_API void *_emalloc(size_t size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	TSRMLS_FETCH();

#if ZEND_USE_MALLOC_MM
	if (!AG(mm_heap)->use_zend_alloc) {
		return malloc(size);
	}
#endif
	return _zend_mm_alloc_int(AG(mm_heap), size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
}

ZEND_API void _efree(void *ptr ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	TSRMLS_FETCH();

#if ZEND_USE_MALLOC_MM
	if (!AG(mm_heap)->use_zend_alloc) {
		free(ptr);
		return;
	}
#endif
	_zend_mm_free_int(AG(mm_heap), ptr ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
}

ZEND_API void *_erealloc(void *ptr, size_t size, int allow_failure ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	TSRMLS_FETCH();

#if ZEND_USE_MALLOC_MM
	if (!AG(mm_heap)->use_zend_alloc) {
		return realloc(ptr, size);
	}
#endif
	return _zend_mm_realloc_int(AG(mm_heap), ptr, size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
}

ZEND_API size_t _zend_mem_block_size(void *ptr TSRMLS_DC ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
#if ZEND_USE_MALLOC_MM
	if (!AG(mm_heap)->use_zend_alloc) {
		return 0;
	}
#endif
	return _zend_mm_block_size(AG(mm_heap), ptr ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
}

#include "zend_multiply.h"

ZEND_API void *_safe_emalloc(size_t nmemb, size_t size, size_t offset ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{

	if (nmemb < LONG_MAX
			&& size < LONG_MAX
			&& offset < LONG_MAX
			&& nmemb >= 0
			&& size >= 0
			&& offset >= 0) {
		long lval;
		double dval;
		int use_dval;

		ZEND_SIGNED_MULTIPLY_LONG(nmemb, size, lval, dval, use_dval);

		if (!use_dval
				&& lval < (long) (LONG_MAX - offset)) {
			return emalloc_rel(lval + offset);
		}
	}

	zend_error(E_ERROR, "Possible integer overflow in memory allocation (%zd * %zd + %zd)", nmemb, size, offset);
	return 0;
}

ZEND_API void *_safe_malloc(size_t nmemb, size_t size, size_t offset)
{

	if (nmemb < LONG_MAX
			&& size < LONG_MAX
			&& offset < LONG_MAX
			&& nmemb >= 0
			&& size >= 0
			&& offset >= 0) {
		long lval;
		double dval;
		int use_dval;

		ZEND_SIGNED_MULTIPLY_LONG(nmemb, size, lval, dval, use_dval);

		if (!use_dval
				&& lval < (long) (LONG_MAX - offset)) {
			return pemalloc(lval + offset, 1);
		}
	}

	zend_error(E_ERROR, "Possible integer overflow in memory allocation (%zd * %zd + %zd)", nmemb, size, offset);
	return 0;
}

ZEND_API void *_ecalloc(size_t nmemb, size_t size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	void *p;
	int final_size = size*nmemb;

	p = _emalloc(final_size ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
	if (!p) {
		return (void *) p;
	}
	memset(p, 0, final_size);
	return p;
}

ZEND_API char *_estrdup(const char *s ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	int length;
	char *p;

	length = strlen(s)+1;
	p = (char *) _emalloc(length ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
	if (!p) {
		return (char *)NULL;
	}
	memcpy(p, s, length);
	return p;
}

ZEND_API char *_estrndup(const char *s, uint length ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	char *p;

	p = (char *) _emalloc(length+1 ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
	if (!p) {
		return (char *)NULL;
	}
	memcpy(p, s, length);
	p[length] = 0;
	return p;
}


ZEND_API char *zend_strndup(const char *s, uint length)
{
	char *p;

	p = (char *) malloc(length+1);
	if (!p) {
		return (char *)NULL;
	}
	if (length) {
		memcpy(p, s, length);
	}
	p[length] = 0;
	return p;
}


ZEND_API int zend_set_memory_limit(unsigned int memory_limit)
{
#if MEMORY_LIMIT
	TSRMLS_FETCH();

	AG(mm_heap)->limit = memory_limit;
	return SUCCESS;
#else
	return FAILURE;
#endif
}

ZEND_API size_t zend_memory_usage(int real_usage TSRMLS_DC)
{
	if (real_usage) {
		return AG(mm_heap)->real_size;
	} else {
#if MEMORY_LIMIT
		return AG(mm_heap)->size;
#else
		return AG(mm_heap)->real_size;
#endif
	}
}

#if MEMORY_LIMIT
ZEND_API size_t zend_memory_peak_usage(int real_usage TSRMLS_DC)
{
	if (real_usage) {
		return AG(mm_heap)->real_peak;
	} else {
		return AG(mm_heap)->peak;
	}
}
#endif

ZEND_API void shutdown_memory_manager(int silent, int full_shutdown TSRMLS_DC)
{
	zend_mm_shutdown(AG(mm_heap), full_shutdown, silent);
}

static void alloc_globals_ctor(zend_alloc_globals *alloc_globals TSRMLS_DC)
{
	alloc_globals->mm_heap = zend_mm_startup();
#if ZEND_USE_MALLOC_MM
	{
		char *tmp = getenv("USE_ZEND_ALLOC");
		if (tmp) {
			alloc_globals->mm_heap->use_zend_alloc = zend_atoi(tmp, 0);
		}
	}
#endif
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


#if ZEND_DEBUG
ZEND_API int _mem_block_check(void *ptr, int silent ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	TSRMLS_FETCH();

#if ZEND_USE_MALLOC_MM
	if (!AG(mm_heap)->use_zend_alloc) {
		return 1;
	}
#endif
	return zend_mm_check_ptr(AG(mm_heap), ptr, silent ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_ORIG_RELAY_CC);
}


ZEND_API void _full_mem_check(int silent ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	int errors;
	TSRMLS_FETCH();

#if ZEND_USE_MALLOC_MM
	if (!AG(mm_heap)->use_zend_alloc) {
		return;
	}
#endif

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
