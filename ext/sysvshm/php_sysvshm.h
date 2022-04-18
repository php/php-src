/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
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
	zend_long length;
	zend_long next;
	char mem;
} sysvshm_chunk;

typedef struct {
	char magic[8];
	zend_long start;
	zend_long end;
	zend_long free;
	zend_long total;
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
