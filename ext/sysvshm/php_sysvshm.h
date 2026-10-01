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
   | Author: Christian Cartus <cartus@atrior.de>                          |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_SYSVSHM_H
#define PHP_SYSVSHM_H

#ifdef HAVE_SYSVSHM

extern zend_module_entry sysvshm_module_entry;
#define sysvshm_module_ptr &sysvshm_module_entry

#include "php_version.h"
#define PHP_SYSVSHM_VERSION PHP_VERSION

#include <sys/types.h>

#ifdef PHP_WIN32
# include <TSRM/tsrm_win32.h>
# include "win32/ipc.h"
#else
# include <sys/ipc.h>
# include <sys/shm.h>
#endif

typedef struct {
	zend_long init_mem;
} sysvshm_module;

typedef struct {
	zend_long key;
	size_t length;
	size_t next;
	char mem;
} sysvshm_chunk;

typedef struct {
	char magic[8];
	size_t start;
	size_t end;
	size_t free;
	size_t total;
} sysvshm_chunk_head;

typedef struct {
	key_t key;               /* key set by user */
	zend_long id;                 /* returned by shmget */
	sysvshm_chunk_head *ptr; /* memory address of shared memory */
	zend_object std;
} sysvshm_shm;

PHP_MINIT_FUNCTION(sysvshm);
PHP_MINFO_FUNCTION(sysvshm);

extern sysvshm_module php_sysvshm;

#else

#define sysvshm_module_ptr NULL

#endif

#define phpext_sysvshm_ptr sysvshm_module_ptr

#endif /* PHP_SYSVSHM_H */
