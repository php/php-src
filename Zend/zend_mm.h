#ifndef _ZEND_MM_H
#define _ZEND_MM_H

#include <sys/types.h>

#include "zend.h"

#define ZEND_MM 0

/* mm block type */
typedef struct _zend_mm_block {
	size_t size;
	unsigned int type;
	size_t prev_size;
	struct _zend_mm_block *prev_free_block;
	struct _zend_mm_block *next_free_block;
} zend_mm_block;

typedef struct _zend_mm_heap {
	/* Head of free list */
	zend_mm_block *free_list;
	size_t block_size;
} zend_mm_heap;

zend_bool zend_mm_startup(zend_mm_heap *heap, size_t block_size);
void zend_mm_shutdown(zend_mm_heap *heap);
void *zend_mm_alloc(zend_mm_heap *heap, size_t size);
void zend_mm_free(zend_mm_heap *heap, void *p);
void *zend_mm_realloc(zend_mm_heap *heap, void *p, size_t size);

#endif /* _ZEND_MM_H */
