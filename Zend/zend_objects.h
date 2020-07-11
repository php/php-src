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
END_EXTERN_C()

#endif /* ZEND_OBJECTS_H */
