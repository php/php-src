#ifndef _STATIC_ALLOCATOR_H
#define _STATIC_ALLOCATOR_H

#define ALLOCATOR_BLOCK_SIZE 400000

typedef unsigned int zend_uint;

typedef struct _Block {
	char *bp;
	char *pos;
	char *end;
} Block;

typedef struct _StaticAllocator {
	Block *Blocks;
	zend_uint num_blocks;
	zend_uint current_block;
} StaticAllocator;

int static_allocator_init(StaticAllocator *sa);
char *static_allocator_allocate(StaticAllocator *sa, zend_uint size);
void static_allocator_destroy(StaticAllocator *sa);

/* Temporary */
#define emalloc(s) malloc(s)
#define efree(p) free(p)
#define SUCCESS 0
#define FAILURE -1

#endif /* _STATIC_ALLOCATOR_H */
