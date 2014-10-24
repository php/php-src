#ifndef ZEND_MM_STRUCTS_H
#define ZEND_MM_STRUCTS_H

/* structs and macros defined in Zend/zend_alloc.c
   Needed for realizing watchpoints and sigsafe memory */

#include "zend.h"

#ifndef ZEND_MM_COOKIES
#	define ZEND_MM_COOKIES ZEND_DEBUG
#endif

#define ZEND_MM_CACHE 1
#ifndef ZEND_MM_CACHE_STAT
#	define ZEND_MM_CACHE_STAT 0
#endif

typedef struct _zend_mm_block_info {
#if ZEND_MM_COOKIES
	size_t _cookie;
#endif
	size_t _size;
	size_t _prev;
} zend_mm_block_info;

typedef struct _zend_mm_small_free_block {
	zend_mm_block_info info;
#if ZEND_DEBUG
	unsigned int magic;
#ifdef ZTS
	THREAD_T thread_id;
#endif
#endif
	struct _zend_mm_free_block *prev_free_block;
	struct _zend_mm_free_block *next_free_block;
} zend_mm_small_free_block;

typedef struct _zend_mm_free_block {
	zend_mm_block_info info;
#if ZEND_DEBUG
	unsigned int magic;
#ifdef ZTS
	THREAD_T thread_id;
#endif
#endif
	struct _zend_mm_free_block *prev_free_block;
	struct _zend_mm_free_block *next_free_block;

	struct _zend_mm_free_block **parent;
	struct _zend_mm_free_block *child[2];
} zend_mm_free_block;

#define ZEND_MM_SMALL_FREE_BUCKET(heap, index) \
	(zend_mm_free_block *) ((char *)&heap->free_buckets[index * 2] + \
		sizeof(zend_mm_free_block *) * 2 - \
		sizeof(zend_mm_small_free_block))

#define ZEND_MM_REST_BUCKET(heap) \
	(zend_mm_free_block *)((char *)&heap->rest_buckets[0] + \
		sizeof(zend_mm_free_block *) * 2 - \
		sizeof(zend_mm_small_free_block))

#define ZEND_MM_NUM_BUCKETS (sizeof(size_t) << 3)
struct _zend_mm_heap {
	int   use_zend_alloc;
	void *(*_malloc)(size_t);
	void  (*_free)(void *);
	void *(*_realloc)(void *, size_t);
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

#endif
