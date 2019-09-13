/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Andrey Hristov <andrey@php.net>                             |
  |          Ulf Wendel <uw@php.net>                                     |
  |          Dmitry Stogov <dmitry@php.net>                              |
  +----------------------------------------------------------------------+
*/

#include "php.h"
#include "mysqlnd.h"
#include "mysqlnd_block_alloc.h"
#include "mysqlnd_debug.h"
#include "mysqlnd_priv.h"

/* {{{ mysqlnd_mempool_free_chunk */
static void
mysqlnd_mempool_free_chunk(MYSQLND_MEMORY_POOL * pool, void * ptr)
{
	DBG_ENTER("mysqlnd_mempool_free_chunk");
	/* Try to back-off and guess if this is the last block allocated */
#ifndef ZEND_TRACK_ARENA_ALLOC
	if (ptr == pool->last) {
		/*
			This was the last allocation. Lucky us, we can free
			a bit of memory from the pool. Next time we will return from the same ptr.
		*/
		pool->arena->ptr = (char*)ptr;
		pool->last = NULL;
	}
#endif
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_mempool_resize_chunk */
static void *
mysqlnd_mempool_resize_chunk(MYSQLND_MEMORY_POOL * pool, void * ptr, size_t old_size, size_t size)
{
	DBG_ENTER("mysqlnd_mempool_resize_chunk");

#ifndef ZEND_TRACK_ARENA_ALLOC
	/* Try to back-off and guess if this is the last block allocated */
	if (ptr == pool->last
	  && (ZEND_MM_ALIGNED_SIZE(size) <= ((char*)pool->arena->end - (char*)ptr))) {
		/*
			This was the last allocation. Lucky us, we can free
			a bit of memory from the pool. Next time we will return from the same ptr.
		*/
		pool->arena->ptr = (char*)ptr + ZEND_MM_ALIGNED_SIZE(size);
		DBG_RETURN(ptr);
	}
#endif

	void *new_ptr = zend_arena_alloc(&pool->arena, size);
	memcpy(new_ptr, ptr, MIN(old_size, size));
	pool->last = ptr = new_ptr;
	DBG_RETURN(ptr);
}
/* }}} */


/* {{{ mysqlnd_mempool_get_chunk */
static void *
mysqlnd_mempool_get_chunk(MYSQLND_MEMORY_POOL * pool, size_t size)
{
	void *ptr = NULL;
	DBG_ENTER("mysqlnd_mempool_get_chunk");

	ptr = zend_arena_alloc(&pool->arena, size);
	pool->last = ptr;

	DBG_RETURN(ptr);
}
/* }}} */


/* {{{ mysqlnd_mempool_create */
PHPAPI MYSQLND_MEMORY_POOL *
mysqlnd_mempool_create(size_t arena_size)
{
	zend_arena * arena;
	MYSQLND_MEMORY_POOL * ret;

	DBG_ENTER("mysqlnd_mempool_create");
	arena = zend_arena_create(MAX(arena_size, ZEND_MM_ALIGNED_SIZE(sizeof(zend_arena))));
	ret = zend_arena_alloc(&arena, sizeof(MYSQLND_MEMORY_POOL));
	ret->arena = arena;
	ret->last = NULL;
	ret->checkpoint = NULL;
	ret->get_chunk = mysqlnd_mempool_get_chunk;
	ret->free_chunk = mysqlnd_mempool_free_chunk;
	ret->resize_chunk = mysqlnd_mempool_resize_chunk;
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_mempool_destroy */
PHPAPI void
mysqlnd_mempool_destroy(MYSQLND_MEMORY_POOL * pool)
{
	DBG_ENTER("mysqlnd_mempool_destroy");
	/* mnd_free will reference LOCK_access and might crash, depending on the caller...*/
	zend_arena_destroy(pool->arena);
	DBG_VOID_RETURN;
}
/* }}} */

/* {{{ mysqlnd_mempool_save_state */
PHPAPI void
mysqlnd_mempool_save_state(MYSQLND_MEMORY_POOL * pool)
{
	DBG_ENTER("mysqlnd_mempool_save_state");
	pool->checkpoint = zend_arena_checkpoint(pool->arena);
	DBG_VOID_RETURN;
}
/* }}} */

/* {{{ mysqlnd_mempool_restore_state */
PHPAPI void
mysqlnd_mempool_restore_state(MYSQLND_MEMORY_POOL * pool)
{
	DBG_ENTER("mysqlnd_mempool_restore_state");
#if ZEND_DEBUG
	ZEND_ASSERT(pool->checkpoint);
#endif
	if (pool->checkpoint) {
		zend_arena_release(&pool->arena, pool->checkpoint);
		pool->last = NULL;
		pool->checkpoint = NULL;
	}
	DBG_VOID_RETURN;
}
/* }}} */
