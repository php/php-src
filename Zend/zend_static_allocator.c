/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2012 Zend Technologies Ltd. (http://www.zend.com) |
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

#include "zend_static_allocator.h"

/* Not checking emalloc() and erealloc() return values as they are supposed to bailout */

inline static void block_init(Block *block, zend_uint block_size)
{
	block->pos = block->bp = (char *) emalloc(block_size);
	block->end = block->bp + block_size;
}

inline static char *block_allocate(Block *block, zend_uint size)
{
	char *retval = block->pos;
	if ((block->pos += size) >= block->end) {
		return (char *)NULL;
	}
	return retval;
}

inline static void block_destroy(Block *block)
{
	efree(block->bp);
}

void static_allocator_init(StaticAllocator *sa)
{
	sa->Blocks = (Block *) emalloc(sizeof(Block));
	block_init(sa->Blocks, ALLOCATOR_BLOCK_SIZE);
	sa->num_blocks = 1;
	sa->current_block = 0;
}

char *static_allocator_allocate(StaticAllocator *sa, zend_uint size)
{
	char *retval;

	retval = block_allocate(&sa->Blocks[sa->current_block], size);
	if (retval) {
		return retval;
	}
	sa->Blocks = (Block *) erealloc(sa->Blocks, ++sa->num_blocks);
	sa->current_block++;
	block_init(&sa->Blocks[sa->current_block], (size > ALLOCATOR_BLOCK_SIZE) ? size : ALLOCATOR_BLOCK_SIZE);
	retval = block_allocate(&sa->Blocks[sa->current_block], size);
	return retval;
}

void static_allocator_destroy(StaticAllocator *sa)
{
	zend_uint i;

	for (i=0; i<sa->num_blocks; i++) {
		block_free(&sa->Blocks[i]);
	}
	efree(sa->Blocks);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
