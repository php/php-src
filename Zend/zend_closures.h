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
   | Authors: Christian Seiler <chris_se@gmx.net>                         |
   |          Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_CLOSURES_H
#define ZEND_CLOSURES_H

BEGIN_EXTERN_C()

/* This macro depends on zend_closure structure layout */
#define ZEND_CLOSURE_OBJECT(op_array) \
	((zend_object*)((char*)(op_array) - sizeof(zend_object)))

void zend_register_closure_ce(void);
void zend_closure_bind_var(zval *closure_zv, zend_string *var_name, zval *var);
void zend_closure_bind_var_ex(zval *closure_zv, uint32_t offset, zval *val);
void zend_closure_from_frame(zval *closure_zv, zend_execute_data *frame);

extern ZEND_API zend_class_entry *zend_ce_closure;

ZEND_API void zend_create_closure(zval *res, zend_function *op_array, zend_class_entry *scope, zend_class_entry *called_scope, zval *this_ptr);
ZEND_API void zend_create_fake_closure(zval *res, zend_function *op_array, zend_class_entry *scope, zend_class_entry *called_scope, zval *this_ptr);
ZEND_API zend_function *zend_get_closure_invoke_method(zend_object *obj);
ZEND_API const zend_function *zend_get_closure_method_def(zend_object *obj);
ZEND_API zval* zend_get_closure_this_ptr(zval *obj);

END_EXTERN_C()

#endif
