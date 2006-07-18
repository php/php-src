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
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#if 0
#ifndef _ZEND_MM_H
#define _ZEND_MM_H

#include <sys/types.h>

#include "zend.h"
#include "zend_types.h"


#ifdef ZEND_WIN32
#undef ZEND_MM
#else
/* #define ZEND_MM */
#undef ZEND_MM
#endif

/* mm block type */
typedef struct _zend_mm_block {
	unsigned int size : 31;
	unsigned int type : 1;
	size_t prev_size;
	unsigned int guard_block : 1;
} zend_mm_block;

typedef struct _zend_mm_free_block {
	unsigned int size : 31;
	unsigned int type : 1;
	size_t prev_size;
	struct _zend_mm_free_block *prev_free_block;
	struct _zend_mm_free_block *next_free_block;
} zend_mm_free_block;

typedef struct _zend_mm_segment {
	struct _zend_mm_segment *next_segment;
} zend_mm_segment;

#define ZEND_MM_NUM_BUCKETS 16

#define ZEND_HEAP_MAX_BUCKETS ZEND_MM_NUM_BUCKETS

typedef int zend_heap[2*ZEND_HEAP_MAX_BUCKETS-1];

typedef struct _zend_mm_heap {
	zend_mm_segment *segments_list;
	size_t block_size;
	zend_mm_free_block *free_buckets[ZEND_MM_NUM_BUCKETS];
	zend_heap heap;
} zend_mm_heap;

zend_bool zend_mm_startup(zend_mm_heap *heap, size_t block_size);
void zend_mm_shutdown(zend_mm_heap *heap);
void *zend_mm_alloc(zend_mm_heap *heap, size_t size);
void zend_mm_free(zend_mm_heap *heap, void *p);
void *zend_mm_realloc(zend_mm_heap *heap, void *p, size_t size);

#endif /* _ZEND_MM_H */
#endif
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
