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
   | Authors: Christian Seiler <chris_se@gmx.net>                         |
   |          Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_CLOSURES_H
#define ZEND_CLOSURES_H

#include "zend_types.h"

BEGIN_EXTERN_C()

/* This macro depends on zend_closure structure layout */
#define ZEND_CLOSURE_OBJECT(op_array) \
	((zend_object*)((char*)(op_array) - sizeof(zend_object)))

void zend_register_closure_ce(void);
void zend_closure_bind_var(zval *closure_zv, zend_string *var_name, zval *var);
void zend_closure_bind_var_ex(zval *closure_zv, uint32_t offset, zval *val);
void zend_closure_from_frame(zval *closure_zv, const zend_execute_data *frame);

extern ZEND_API zend_class_entry *zend_ce_closure;

ZEND_API void zend_create_closure(zval *res, zend_function *op_array, zend_class_entry *scope, zend_class_entry *called_scope, zval *this_ptr);
ZEND_API void zend_create_fake_closure(zval *res, zend_function *op_array, zend_class_entry *scope, zend_class_entry *called_scope, zval *this_ptr);
ZEND_API zend_function *zend_get_closure_invoke_method(zend_object *obj);
ZEND_API const zend_function *zend_get_closure_method_def(zend_object *obj);
ZEND_API zval* zend_get_closure_this_ptr(zval *obj);

/* Closures declared in constant expressions (anonymous declarations as well
 * as first-class callable references) are addressable by an element-scoped
 * reference: an opaque "<site>@<key>[#<hash>]" string where <site> names the
 * declaring reflection element and <key> is the closure's rank within that
 * element (for an anonymous closure, then suffixed with a hash of the
 * closure's code as a staleness check) or the referenced callable's name (for
 * a first-class callable; names cannot drift and carry no hash).
 * zend_constexpr_closure_ref() returns the reference (a fresh zend_string the
 * caller owns) for a Closure object, when it has one. */
ZEND_API zend_result zend_constexpr_closure_ref(zend_object *closure_obj, zend_class_entry **ce, zend_string **id);
ZEND_API void zend_closure_mark_as_constexpr_fcc(zval *closure_zv, zend_class_entry *site_class);

END_EXTERN_C()

#endif
