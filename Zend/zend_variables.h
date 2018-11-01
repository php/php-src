/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2018 Zend Technologies Ltd. (http://www.zend.com) |
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
   |          Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_VARIABLES_H
#define ZEND_VARIABLES_H

#include "zend_types.h"
#include "zend_gc.h"

BEGIN_EXTERN_C()

ZEND_API void ZEND_FASTCALL rc_dtor_func(zend_refcounted *p);
ZEND_API void ZEND_FASTCALL zval_copy_ctor_func(zval *zvalue);

static zend_always_inline void zval_ptr_dtor_nogc(zval *zval_ptr)
{
	if (Z_REFCOUNTED_P(zval_ptr) && !Z_DELREF_P(zval_ptr)) {
		rc_dtor_func(Z_COUNTED_P(zval_ptr));
	}
}

static zend_always_inline void i_zval_ptr_dtor(zval *zval_ptr ZEND_FILE_LINE_DC)
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
ZEND_API void zval_internal_ptr_dtor(zval *zvalue);

/* Kept for compatibility */
#define zval_dtor(zvalue) zval_ptr_dtor_nogc(zvalue)
#define zval_internal_dtor(zvalue) zval_internal_ptr_dtor(zvalue)
#define zval_dtor_func rc_dtor_func
#define zval_ptr_dtor_wrapper zval_ptr_dtor
#define zval_internal_ptr_dtor_wrapper zval_internal_ptr_dtor

ZEND_API void zval_add_ref(zval *p);

END_EXTERN_C()

#define ZVAL_PTR_DTOR zval_ptr_dtor
#define ZVAL_INTERNAL_PTR_DTOR zval_internal_ptr_dtor

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
