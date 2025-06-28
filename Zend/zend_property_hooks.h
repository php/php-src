/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright © Zend by Perforce and Contributors.                       |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
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
