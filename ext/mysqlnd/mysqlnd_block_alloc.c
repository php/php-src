/*
  +----------------------------------------------------------------------+
  | PHP Version 6                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2008 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Georg Richter <georg@mysql.com>                             |
  |          Andrey Hristov <andrey@mysql.com>                           |
  |          Ulf Wendel <uwendel@mysql.com>                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "php.h"
#include "mysqlnd.h"
#include "mysqlnd_block_alloc.h"
#include "mysqlnd_debug.h"
#include "mysqlnd_priv.h"


/* {{{ mysqlnd_mempool_free_chunk */
static void
mysqlnd_mempool_free_contents(MYSQLND_MEMORY_POOL * pool TSRMLS_DC)
{
	uint i;
	DBG_ENTER("mysqlnd_mempool_dtor");
	for (i = 0; i < pool->free_chunk_list_elements; i++) {
		MYSQLND_MEMORY_POOL_CHUNK * chunk = pool->free_chunk_list[i];
		chunk->free_chunk(chunk, FALSE TSRMLS_CC);
	}
	
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_mempool_free_chunk */
static void
mysqlnd_mempool_free_chunk(MYSQLND_MEMORY_POOL_CHUNK * chunk, zend_bool cache_it TSRMLS_DC)
{
	MYSQLND_MEMORY_POOL * pool = chunk->pool;
	DBG_ENTER("mysqlnd_mempool_free_chunk");
	if (chunk->from_pool) {
		/* Try to back-off and guess if this is the last block allocated */
		if (chunk->ptr == (pool->arena + (pool->arena_size - pool->free_size - chunk->size))) {
			/*
				This was the last allocation. Lucky us, we can free
				a bit of memory from the pool. Next time we will return from the same ptr.
			*/
			pool->free_size += chunk->size;
		}
		pool->refcount--;
	} else {
		mnd_free(chunk->ptr);
	}
	if (cache_it && pool->free_chunk_list_elements < MYSQLND_MEMORY_POOL_CHUNK_LIST_SIZE) {
		chunk->ptr = NULL;
		pool->free_chunk_list[pool->free_chunk_list_elements++] = chunk;
	} else {
		/* We did not cache it -> free it */
		mnd_free(chunk);
	}
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_mempool_resize_chunk */
static void
mysqlnd_mempool_resize_chunk(MYSQLND_MEMORY_POOL_CHUNK * chunk, uint size TSRMLS_DC)
{
	DBG_ENTER("mysqlnd_mempool_resize_chunk");
	if (chunk->from_pool) {
		MYSQLND_MEMORY_POOL * pool = chunk->pool;
		/* Try to back-off and guess if this is the last block allocated */
		if (chunk->ptr == (pool->arena + (pool->arena_size - pool->free_size - chunk->size))) {
			/*
				This was the last allocation. Lucky us, we can free
				a bit of memory from the pool. Next time we will return from the same ptr.
			*/
			if ((chunk->size + pool->free_size) < size) {
				zend_uchar *new_ptr;
				new_ptr = mnd_malloc(size);
				memcpy(new_ptr, chunk->ptr, chunk->size);
				chunk->ptr = new_ptr;
				pool->free_size += chunk->size;
				chunk->size = size;
				chunk->pool = NULL; /* now we have no pool memory */
				pool->refcount--;
			} else {
				/* If the chunk is > than asked size then free_memory increases, otherwise decreases*/
				pool->free_size += (chunk->size - size);
			}
		} else {
			/* Not last chunk, if the user asks for less, give it to him */
			if (chunk->size >= size) {
				; /* nop */
			} else {
				zend_uchar *new_ptr;
				new_ptr = mnd_malloc(size);
				memcpy(new_ptr, chunk->ptr, chunk->size);
				chunk->ptr = new_ptr;
				chunk->size = size;
				chunk->pool = NULL; /* now we have no pool memory */
				pool->refcount--;				
			}
		}
	} else {
		chunk->ptr = mnd_realloc(chunk->ptr, size);
	}
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_mempool_get_chunk */
static
MYSQLND_MEMORY_POOL_CHUNK * mysqlnd_mempool_get_chunk(MYSQLND_MEMORY_POOL * pool, uint size TSRMLS_DC)
{
	MYSQLND_MEMORY_POOL_CHUNK *chunk = NULL;
	DBG_ENTER("mysqlnd_mempool_get_chunk");

	if (pool->free_chunk_list_elements) {
		chunk = pool->free_chunk_list[--pool->free_chunk_list_elements];
	} else {
		chunk = mnd_malloc(sizeof(MYSQLND_MEMORY_POOL_CHUNK));
	}

	chunk->free_chunk = mysqlnd_mempool_free_chunk;
	chunk->resize_chunk = mysqlnd_mempool_resize_chunk;
	chunk->size = size;
	/*
	  Should not go over MYSQLND_MAX_PACKET_SIZE, since we
	  expect non-arena memory in mysqlnd_wireprotocol.c . We
	  realloc the non-arena memory.
	*/
	chunk->pool = pool;
	if (size > pool->free_size) {
		chunk->ptr = mnd_malloc(size);
		chunk->from_pool = FALSE;
	} else {
		chunk->from_pool = TRUE;
		++pool->refcount;
		chunk->ptr = pool->arena + (pool->arena_size - pool->free_size);
		/* Last step, update free_size */
		pool->free_size -= size;
	}
	DBG_RETURN(chunk);
}
/* }}} */


/* {{{ mysqlnd_mempool_create */
MYSQLND_MEMORY_POOL *
mysqlnd_mempool_create(size_t arena_size TSRMLS_DC)
{
	/* We calloc, because we free(). We don't mnd_calloc()  for a reason. */
	MYSQLND_MEMORY_POOL * ret = mnd_calloc(1, sizeof(MYSQLND_MEMORY_POOL));
	DBG_ENTER("mysqlnd_mempool_create");

	ret->free_size = ret->arena_size = arena_size;
	ret->refcount = 0;
	/* OOM ? */
	ret->arena = mnd_malloc(ret->arena_size);
	ret->get_chunk = mysqlnd_mempool_get_chunk;

	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_mempool_destroy */
void
mysqlnd_mempool_destroy(MYSQLND_MEMORY_POOL * pool TSRMLS_DC)
{
	DBG_ENTER("mysqlnd_mempool_destroy");
	/* mnd_free will reference LOCK_access and might crash, depending on the caller...*/
	mysqlnd_mempool_free_contents(pool TSRMLS_CC);
	mnd_free(pool->arena);
	DBG_VOID_RETURN;
}
/* }}} */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
