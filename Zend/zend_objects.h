/* 
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2013 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef ZEND_OBJECTS_H
#define ZEND_OBJECTS_H

#include "zend.h"

BEGIN_EXTERN_C()
ZEND_API void zend_object_std_init(zend_object *object, zend_class_entry *ce TSRMLS_DC);
ZEND_API void zend_object_std_dtor(zend_object *object TSRMLS_DC);
ZEND_API zend_object_value zend_objects_new(zend_object **object, zend_class_entry *class_type TSRMLS_DC);
ZEND_API void zend_objects_destroy_object(zend_object *object, zend_object_handle handle TSRMLS_DC);
ZEND_API zend_object *zend_objects_get_address(const zval *object TSRMLS_DC);
ZEND_API void zend_objects_clone_members(zend_object *new_object, zend_object_value new_obj_val, zend_object *old_object, zend_object_handle handle TSRMLS_DC);
ZEND_API zend_object_value zend_objects_clone_obj(zval *object TSRMLS_DC);
ZEND_API void zend_objects_free_object_storage(zend_object *object TSRMLS_DC);
END_EXTERN_C()

#endif /* ZEND_OBJECTS_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
