/*
   +----------------------------------------------------------------------+
   | Zend JIT                                                             |
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
   | Authors: Dmitry Stogov <dmitry@php.net>                              |
   |          Xinchen Hui <laruence@php.net>                              |
   +----------------------------------------------------------------------+
*/

#ifndef HAVE_ZEND_JIT_GDB_H
#define HAVE_ZEND_JIT_GDB_H

#include "zend_compile.h"

#define HAVE_GDB

int zend_jit_gdb_register(const char    *name,
                          const zend_op_array *op_array,
                          const void    *start,
                          size_t         size,
                          uint32_t       sp_offset,
                          uint32_t       sp_adjustment);

int zend_jit_gdb_unregister(void);
void zend_jit_gdb_init(void);

#endif /* HAVE_ZEND_JIT_GDB_H */
