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

/* This really should not be needed... */
#ifndef ZEND_DIMENSION_HANDLERS_H
#define ZEND_DIMENSION_HANDLERS_H

//BEGIN_EXTERN_C()

#include "zend_types.h"

typedef struct _zend_internal_class_dimensions_functions {
	/* rv is a slot provided by the callee that is returned */
	zval *(*/* const */ read_dimension)(zend_object *object, zval *offset, zval *rv);
	bool  (*/* const */ has_dimension)(zend_object *object, zval *offset);
	zval *(*/* const */ fetch_dimension)(zend_object *object, zval *offset, zval *rv);
	void  (*/* const */ write_dimension)(zend_object *object, zval *offset, zval *value);
	/* rv is a slot provided by the callee that is returned */
	void  (*/* const */ append)(zend_object *object, zval *value);
	zval *(*/* const */ fetch_append)(zend_object *object, zval *rv);
	void  (*/* const */ unset_dimension)(zend_object *object, zval *offset);
} zend_internal_class_dimensions_functions;

typedef struct _zend_user_class_dimensions_functions {
	zend_function *read_dimension;
	zend_function *has_dimension;
	zend_function *fetch_dimension;
	zend_function *write_dimension;
	zend_function *append;
	zend_function *fetch_append;
	zend_function *unset_dimension;
} zend_user_class_dimensions_functions;

ZEND_API zval* zend_class_read_dimension(zend_object *object, zval *offset, zval *rv);
ZEND_API bool  zend_class_has_dimension(zend_object *object, zval *offset);
ZEND_API zval* zend_class_fetch_dimension(zend_object *object, zval *offset, zval *rv);
ZEND_API void  zend_class_write_dimension(zend_object *object, zval *offset, zval *value);
ZEND_API void  zend_class_append(zend_object *object, zval *value);
ZEND_API zval *zend_class_fetch_append(zend_object *object, zval *rv);
ZEND_API void  zend_class_unset_dimension(zend_object *object, zval *offset);

/* VM and JIT Helper */
ZEND_API bool zend_class_isset_empty_dimension(zend_object *object, zval *offset, bool is_empty);

#endif /* ZEND_DIMENSION_HANDLERS_H */
