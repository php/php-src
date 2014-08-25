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
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef ZEND_STATIC_ALLOCATOR_H
#define ZEND_STATIC_ALLOCATOR_H

#define ALLOCATOR_BLOCK_SIZE 400000

/* Temporary */
typedef unsigned int uint32_t;
#define emalloc(s) malloc(s)
#define efree(p) free(p)

typedef struct _Block {
	char *bp;
	char *pos;
	char *end;
} Block;

typedef struct _StaticAllocator {
	Block *Blocks;
	uint32_t num_blocks;
	uint32_t current_block;
} StaticAllocator;

void static_allocator_init(StaticAllocator *sa);
char *static_allocator_allocate(StaticAllocator *sa, uint32_t size);
void static_allocator_destroy(StaticAllocator *sa);

#endif /* ZEND_STATIC_ALLOCATOR_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
