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
   | Authors: krakjoe <krakjoe@php.net>                                   |
   +----------------------------------------------------------------------+
*/
#ifndef ZEND_PARTIAL_H
#define ZEND_PARTIAL_H

BEGIN_EXTERN_C()

void zend_partial_startup(void);

#define ZEND_APPLY_NORMAL   (1<<16)
#define ZEND_APPLY_FACTORY  (1<<17)
#define ZEND_APPLY_PASS     (1<<18)
#define ZEND_APPLY_VARIADIC (1<<19)

void zend_partial_create(zval *result, uint32_t info, zval *this_ptr, zend_function *function, uint32_t argc, zval *argv, zend_array *extra_named_params);

void zend_partial_args_check(zend_execute_data *call);

ZEND_NAMED_FUNCTION(zend_partial_call_magic);

END_EXTERN_C()

#endif
