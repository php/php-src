/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2015 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Dmitry Stogov <dmitry@zend.com>                             |
   +----------------------------------------------------------------------+
*/

/* $Id:$ */

#ifndef _ZEND_ARENA_H_
#define _ZEND_ARENA_H_

#include "zend.h"

typedef struct _zend_arena zend_arena;

struct _zend_arena {
	char		*ptr;
	char		*end;
	zend_arena  *prev;
};

static zend_always_inline zend_arena* zend_arena_create(size_t size)
{
	zend_arena *arena = (zend_arena*)emalloc(size);

	arena->ptr = (char*) arena + ZEND_MM_ALIGNED_SIZE(sizeof(zend_arena));
	arena->end = (char*) arena + size;
	arena->prev = NULL;
	return arena;
}

static zend_always_inline void zend_arena_destroy(zend_arena *arena)
{
	do {
		zend_arena *prev = arena->prev;
		efree(arena);
		arena = prev;
	} while (arena);
}

#define ZEND_ARENA_ALIGNMENT 8U

static zend_always_inline void* zend_arena_alloc(zend_arena **arena_ptr, size_t size)
{
	zend_arena *arena = *arena_ptr;
	char *ptr = arena->ptr;

	size = ZEND_MM_ALIGNED_SIZE(size);

	if (EXPECTED(size <= (size_t)(arena->end - ptr))) {
		arena->ptr = ptr + size;
	} else {
		size_t arena_size =
			UNEXPECTED((size + ZEND_MM_ALIGNED_SIZE(sizeof(zend_arena))) > (size_t)(arena->end - (char*) arena)) ?
				(size + ZEND_MM_ALIGNED_SIZE(sizeof(zend_arena))) :
				(size_t)(arena->end - (char*) arena);
		zend_arena *new_arena = (zend_arena*)emalloc(arena_size);

		ptr = (char*) new_arena + ZEND_MM_ALIGNED_SIZE(sizeof(zend_arena));
		new_arena->ptr = (char*) new_arena + ZEND_MM_ALIGNED_SIZE(sizeof(zend_arena)) + size;
		new_arena->end = (char*) new_arena + arena_size;
		new_arena->prev = arena;
		*arena_ptr = new_arena;
	}

	return (void*) ptr;
}

static zend_always_inline void* zend_arena_calloc(zend_arena **arena_ptr, size_t count, size_t unit_size)
{
	int overflow;
	size_t size;
	void *ret;

	size = zend_safe_address(unit_size, count, 0, &overflow);
	if (UNEXPECTED(overflow)) {
		zend_error(E_ERROR, "Possible integer overflow in zend_arena_calloc() (%zu * %zu)", unit_size, count);
	}
	ret = zend_arena_alloc(arena_ptr, size);
	memset(ret, 0, size);
	return ret;
}

static zend_always_inline void* zend_arena_checkpoint(zend_arena *arena)
{
	return arena->ptr;
}

static zend_always_inline void zend_arena_release(zend_arena **arena_ptr, void *checkpoint)
{
	zend_arena *arena = *arena_ptr;

	while (UNEXPECTED((char*)checkpoint > arena->end) ||
	       UNEXPECTED((char*)checkpoint < (char*)arena)) {
		zend_arena *prev = arena->prev;
		efree(arena);
		*arena_ptr = arena = prev;
	}
	arena->ptr = (char*)checkpoint;
}

#endif /* _ZEND_ARENA_H_ */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
