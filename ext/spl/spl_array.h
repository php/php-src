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
   | Authors: Marcus Boerger <helly@php.net>                              |
   +----------------------------------------------------------------------+
 */

#ifndef SPL_ARRAY_H
#define SPL_ARRAY_H

#include "php.h"
#include "php_spl.h"
#include "spl_iterators.h"

#define SPL_ARRAY_STD_PROP_LIST      0x00000001
#define SPL_ARRAY_ARRAY_AS_PROPS     0x00000002
#define SPL_ARRAY_CHILD_ARRAYS_ONLY  0x00000004
#define SPL_ARRAY_IS_SELF            0x01000000
#define SPL_ARRAY_USE_OTHER          0x02000000
#define SPL_ARRAY_INT_MASK           0xFFFF0000
#define SPL_ARRAY_CLONE_MASK         0x0100FFFF

#define SPL_ARRAY_METHOD_NO_ARG				0
#define SPL_ARRAY_METHOD_CALLBACK_ARG  		1
#define SPL_ARRAY_METHOD_SORT_FLAGS_ARG 	2

extern PHPAPI zend_class_entry *spl_ce_ArrayObject;
extern PHPAPI zend_class_entry *spl_ce_ArrayIterator;
extern PHPAPI zend_class_entry *spl_ce_RecursiveArrayIterator;

PHP_MINIT_FUNCTION(spl_array);

extern void spl_array_iterator_append(zval *object, zval *append_value);
extern void spl_array_iterator_key(zval *object, zval *return_value);

#endif /* SPL_ARRAY_H */
