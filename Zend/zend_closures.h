/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2015 Zend Technologies Ltd. (http://www.zend.com) |
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
   |          Dmitry Stogov <dmitry@zend.com>                             |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef ZEND_CLOSURES_H
#define ZEND_CLOSURES_H

BEGIN_EXTERN_C()

void zend_register_closure_ce(void);

extern ZEND_API zend_class_entry *zend_ce_closure;

ZEND_API void zend_create_closure(zval *res, zend_function *op_array, zend_class_entry *scope, zval *this_ptr);
ZEND_API zend_function *zend_get_closure_invoke_method(zend_object *obj);
ZEND_API const zend_function *zend_get_closure_method_def(zval *obj);
ZEND_API zval* zend_get_closure_this_ptr(zval *obj);

END_EXTERN_C()

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
