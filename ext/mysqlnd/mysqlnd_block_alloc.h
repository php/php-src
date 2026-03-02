/*
  +----------------------------------------------------------------------+
  | Copyright © The PHP Group and Contributors.                          |
  +----------------------------------------------------------------------+
  | This source file is subject to the Modified BSD License that is      |
  | bundled with this package in the file LICENSE, and is available      |
  | through the World Wide Web at <https://www.php.net/license/>.        |
  |                                                                      |
  | SPDX-License-Identifier: BSD-3-Clause                                |
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
