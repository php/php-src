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
   |          Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_VARIABLES_H
#define ZEND_VARIABLES_H

#include "zend_types.h"
#include "zend_gc.h"
#include "zend_hash.h"

BEGIN_EXTERN_C()

ZEND_API void ZEND_FASTCALL rc_dtor_func(zend_refcounted *p);
ZEND_API void ZEND_FASTCALL zval_copy_ctor_func(zval *zvalue);

static zend_always_inline void zval_ptr_dtor_nogc(zval *zval_ptr)
{
	if (Z_REFCOUNTED_P(zval_ptr) && !Z_DELREF_P(zval_ptr)) {
		rc_dtor_func(Z_COUNTED_P(zval_ptr));
	}
}

static zend_always_inline void i_zval_ptr_dtor(zval *zval_ptr)
{
	if (Z_REFCOUNTED_P(zval_ptr)) {
		zend_refcounted *ref = Z_COUNTED_P(zval_ptr);
		if (!GC_DELREF(ref)) {
			rc_dtor_func(ref);
		} else {
			gc_check_possible_root(ref);
		}
	}
}

static zend_always_inline void zval_copy_ctor(zval *zvalue)
{
	if (Z_TYPE_P(zvalue) == IS_ARRAY) {
		ZVAL_ARR(zvalue, zend_array_dup(Z_ARR_P(zvalue)));
	} else if (Z_REFCOUNTED_P(zvalue)) {
		Z_ADDREF_P(zvalue);
	}
}

static zend_always_inline void zval_opt_copy_ctor(zval *zvalue)
{
	if (Z_OPT_TYPE_P(zvalue) == IS_ARRAY) {
		ZVAL_ARR(zvalue, zend_array_dup(Z_ARR_P(zvalue)));
	} else if (Z_OPT_REFCOUNTED_P(zvalue)) {
		Z_ADDREF_P(zvalue);
	}
}

static zend_always_inline void zval_ptr_dtor_str(zval *zval_ptr)
{
	if (Z_REFCOUNTED_P(zval_ptr) && !Z_DELREF_P(zval_ptr)) {
		ZEND_ASSERT(Z_TYPE_P(zval_ptr) == IS_STRING);
		ZEND_ASSERT(!ZSTR_IS_INTERNED(Z_STR_P(zval_ptr)));
		ZEND_ASSERT(!(GC_FLAGS(Z_STR_P(zval_ptr)) & IS_STR_PERSISTENT));
		efree(Z_STR_P(zval_ptr));
	}
}

ZEND_API void zval_ptr_dtor(zval *zval_ptr);
ZEND_API void zval_ptr_safe_dtor(zval *zval_ptr);
ZEND_API void zval_internal_ptr_dtor(zval *zvalue);

ZEND_API void zval_add_ref(zval *p);

END_EXTERN_C()

#define ZVAL_PTR_DTOR zval_ptr_dtor
#define ZVAL_INTERNAL_PTR_DTOR zval_internal_ptr_dtor

#endif
