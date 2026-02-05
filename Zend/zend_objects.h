/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright © Zend Technologies Ltd., a subsidiary company of          |
   |     Perforce Software, Inc., and Contributors.                       |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <zeev@php.net>                                 |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_OBJECTS_H
#define ZEND_OBJECTS_H

#include "zend.h"

BEGIN_EXTERN_C()
ZEND_API void ZEND_FASTCALL zend_object_std_init(zend_object *object, zend_class_entry *ce);
ZEND_API zend_object* ZEND_FASTCALL zend_objects_new(zend_class_entry *ce);
ZEND_API void ZEND_FASTCALL zend_objects_clone_members(zend_object *new_object, zend_object *old_object);

ZEND_API void zend_object_std_dtor(zend_object *object);
ZEND_API void zend_objects_destroy_object(zend_object *object);
ZEND_API zend_object *zend_objects_clone_obj(zend_object *object);
ZEND_API zend_object *zend_objects_clone_obj_with(zend_object *object, const zend_class_entry *scope, const HashTable *properties);

void zend_object_dtor_dynamic_properties(zend_object *object);
void zend_object_dtor_property(zend_object *object, zval *p);

END_EXTERN_C()

#endif /* ZEND_OBJECTS_H */
