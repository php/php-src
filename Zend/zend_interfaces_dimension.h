/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) Zend Technologies Ltd. (http://www.zend.com)           |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Author: Gina Peter Banyard <girgias@php.net>                         |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_DIMENSION_INTERFACES_H
#define ZEND_DIMENSION_INTERFACES_H

#include "zend_types.h"
#include "zend_compile.h"

BEGIN_EXTERN_C()

extern ZEND_API zend_class_entry *zend_ce_arrayaccess;

extern ZEND_API zend_class_entry *zend_ce_dimension_read;
extern ZEND_API zend_class_entry *zend_ce_dimension_fetch;
extern ZEND_API zend_class_entry *zend_ce_dimension_write;
extern ZEND_API zend_class_entry *zend_ce_dimension_unset;
extern ZEND_API zend_class_entry *zend_ce_appendable;
extern ZEND_API zend_class_entry *zend_ce_dimension_fetch_append;

ZEND_API void zend_register_dimension_interfaces(void);

END_EXTERN_C()

#endif /* ZEND_DIMENSION_INTERFACES_H */
