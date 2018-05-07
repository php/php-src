/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2018 The PHP Group                                |
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

#include "php_version.h"
#define PHP_SYSVSHM_VERSION PHP_VERSION

#include <sys/types.h>

#ifdef PHP_WIN32
# include <TSRM/tsrm_win32.h>
# include "win32/ipc.h"
# ifndef THREAD_LS
#  define THREAD_LS
# endif
#else
# include <sys/ipc.h>
# include <sys/shm.h>
#endif

#include "zend_exceptions.h"
#include "ext/spl/spl_exceptions.h"

zend_class_entry *shm_ce_ptr;

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

typedef struct {
	sysvshm_shm *shm_object_ptr;
	zend_object  std;
} shm_object;
static zend_object_handlers shm_object_handlers;

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_shm_object___construct, 0, 0, 1)
    ZEND_ARG_TYPE_INFO(0, key, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, size, IS_LONG, 1)
    ZEND_ARG_TYPE_INFO(0, perm, IS_LONG, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_shm_object_has, 0, 0, _IS_BOOL, NULL)
    ZEND_ARG_TYPE_INFO(0, variable_key, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_shm_object_free, 0, 0, IS_VOID, NULL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_shm_object_set, 0, 2, IS_VOID, NULL)
    ZEND_ARG_TYPE_INFO(0, variable_key, IS_LONG, 0)
	ZEND_ARG_INFO(0, variable)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_shm_object_get, 0, 0, 2)
    ZEND_ARG_TYPE_INFO(0, variable_key, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_shm_object_remove, 0, 1, IS_VOID, NULL)
    ZEND_ARG_TYPE_INFO(0, variable_key, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_shm_attach, 0, 1, SharedMemoryBlock, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, size, IS_LONG, 1)
	ZEND_ARG_TYPE_INFO(0, mode, IS_LONG, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_shm_detach, 0, 1, _IS_BOOL, NULL)
	ZEND_ARG_OBJ_INFO(0, object, SharedMemoryBlock, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_shm_has_var, 0, 2, _IS_BOOL, NULL)
	ZEND_ARG_OBJ_INFO(0, object, SharedMemoryBlock, 0)
	ZEND_ARG_TYPE_INFO(0, variable_key, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_shm_remove, 0, 1, IS_VOID, NULL)
	ZEND_ARG_OBJ_INFO(0, object, SharedMemoryBlock, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_shm_put_var, 0, 3, _IS_BOOL, NULL)
	ZEND_ARG_OBJ_INFO(0, object, SharedMemoryBlock, 0)
	ZEND_ARG_TYPE_INFO(0, variable_key, IS_LONG, 0)
	ZEND_ARG_INFO(0, variable)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_shm_get_var, 0, 0, 2)
	ZEND_ARG_OBJ_INFO(0, object, SharedMemoryBlock, 0)
	ZEND_ARG_TYPE_INFO(0, variable_key, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_shm_remove_var, 0, 0, 2)
	ZEND_ARG_OBJ_INFO(0, object, SharedMemoryBlock, 0)
	ZEND_ARG_TYPE_INFO(0, variable_key, IS_LONG, 0)
ZEND_END_ARG_INFO()
/* }}} */

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
