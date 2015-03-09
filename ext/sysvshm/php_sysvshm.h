/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2015 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Christian Cartus <cartus@atrior.de>                          |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_SYSVSHM_H
#define PHP_SYSVSHM_H

#if HAVE_SYSVSHM

extern zend_module_entry sysvshm_module_entry;
#define sysvshm_module_ptr &sysvshm_module_entry

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define PHP_SHM_RSRC_NAME "sysvshm"

typedef struct {
	int le_shm;
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
} sysvshm_shm;

PHP_MINIT_FUNCTION(sysvshm);
PHP_FUNCTION(shm_attach);
PHP_FUNCTION(shm_detach);
PHP_FUNCTION(shm_remove);
PHP_FUNCTION(shm_put_var);
PHP_FUNCTION(shm_get_var);
PHP_FUNCTION(shm_has_var);
PHP_FUNCTION(shm_remove_var);

extern sysvshm_module php_sysvshm;

#else

#define sysvshm_module_ptr NULL

#endif

#define phpext_sysvshm_ptr sysvshm_module_ptr

#endif /* PHP_SYSVSHM_H */
