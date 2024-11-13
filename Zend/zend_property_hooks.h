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
   | Authors: Ilija Tovilo <ilutov@php.net>                               |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_PROPERTY_HOOKS_H
#define ZEND_PROPERTY_HOOKS_H

#include "zend_portability.h"

BEGIN_EXTERN_C()

typedef struct _zend_array zend_array;
typedef struct _zend_class_entry zend_class_entry;
typedef struct _zend_object zend_object;
typedef struct _zend_object_iterator zend_object_iterator;
typedef struct _zval_struct zval;

typedef enum {
	ZEND_PROPERTY_HOOK_GET = 0,
	ZEND_PROPERTY_HOOK_SET = 1,
} zend_property_hook_kind;

ZEND_API zend_object_iterator *zend_hooked_object_get_iterator(zend_class_entry *ce, zval *object, int by_ref);
ZEND_API zend_array *zend_hooked_object_build_properties(zend_object *zobj);

END_EXTERN_C()

#endif /* ZEND_PROPERTY_HOOKS_H */
