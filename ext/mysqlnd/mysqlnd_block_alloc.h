/*
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
  +----------------------------------------------------------------------+
*/

#ifndef MYSQLND_BLOCK_ALLOC_H
#define MYSQLND_BLOCK_ALLOC_H

PHPAPI MYSQLND_MEMORY_POOL *	mysqlnd_mempool_create(size_t arena_size);
PHPAPI void 					mysqlnd_mempool_destroy(MYSQLND_MEMORY_POOL * pool);
PHPAPI void						mysqlnd_mempool_save_state(MYSQLND_MEMORY_POOL * pool);
PHPAPI void						mysqlnd_mempool_restore_state(MYSQLND_MEMORY_POOL * pool);

#endif	/* MYSQLND_BLOCK_ALLOC_H */
