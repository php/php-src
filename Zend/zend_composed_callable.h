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
   | Authors: Sara Golemon <pollita@php.net>                              |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_COMPOSED_CALLABLE_H
#define ZEND_COMPOSED_CALLABLE_H

#include "zend.h"
#include "zend_API.h"

BEGIN_EXTERN_C()

extern ZEND_API zend_class_entry *zend_ce_composed_callable;

ZEND_API zend_result zend_composed_callable_new_from_pair(zval *result, zval *callable1, zval *callable2);

void zend_register_composed_callable(zend_class_entry *ce);

END_EXTERN_C()

#endif /* ZEND_COMPOSED_CALLABLE_H */
