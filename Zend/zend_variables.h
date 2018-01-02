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
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   |          Dmitry Stogov <dmitry@zend.com>                             |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef ZEND_VARIABLES_H
#define ZEND_VARIABLES_H

#include "zend_types.h"
#include "zend_gc.h"

BEGIN_EXTERN_C()

ZEND_API void ZEND_FASTCALL _zval_dtor_func(zend_refcounted *p ZEND_FILE_LINE_DC);
ZEND_API void ZEND_FASTCALL _zval_copy_ctor_func(zval *zvalue ZEND_FILE_LINE_DC);

#define zval_dtor_func(zv)         _zval_dtor_func(zv ZEND_FILE_LINE_CC)
#define zval_copy_ctor_func(zv)    _zval_copy_ctor_func(zv ZEND_FILE_LINE_CC)

static zend_always_inline void _zval_ptr_dtor_nogc(zval *zval_ptr ZEND_FILE_LINE_DC)
{
	if (Z_REFCOUNTED_P(zval_ptr) && !Z_DELREF_P(zval_ptr)) {
		_zval_dtor_func(Z_COUNTED_P(zval_ptr) ZEND_FILE_LINE_RELAY_CC);
	}
}

static zend_always_inline void i_zval_ptr_dtor(zval *zval_ptr ZEND_FILE_LINE_DC)
{
	if (Z_REFCOUNTED_P(zval_ptr)) {
		zend_refcounted *ref = Z_COUNTED_P(zval_ptr);
		if (!--GC_REFCOUNT(ref)) {
			_zval_dtor_func(ref ZEND_FILE_LINE_RELAY_CC);
		} else {
			gc_check_possible_root(ref);
		}
	}
}

static zend_always_inline void _zval_copy_ctor(zval *zvalue ZEND_FILE_LINE_DC)
{
	if (Z_REFCOUNTED_P(zvalue) || Z_COPYABLE_P(zvalue)) {
		if (Z_COPYABLE_P(zvalue)) {
			_zval_copy_ctor_func(zvalue ZEND_FILE_LINE_RELAY_CC);
		} else {
			Z_ADDREF_P(zvalue);
		}
	}
}

static zend_always_inline void _zval_opt_copy_ctor(zval *zvalue ZEND_FILE_LINE_DC)
{
	if (Z_OPT_REFCOUNTED_P(zvalue) || Z_OPT_COPYABLE_P(zvalue)) {
		if (Z_OPT_COPYABLE_P(zvalue)) {
			_zval_copy_ctor_func(zvalue ZEND_FILE_LINE_RELAY_CC);
		} else {
			Z_ADDREF_P(zvalue);
		}
	}
}

ZEND_API size_t zend_print_variable(zval *var);
ZEND_API void _zval_ptr_dtor(zval *zval_ptr ZEND_FILE_LINE_DC);
ZEND_API void _zval_internal_dtor_for_ptr(zval *zvalue ZEND_FILE_LINE_DC);
ZEND_API void _zval_internal_dtor(zval *zvalue ZEND_FILE_LINE_DC);
ZEND_API void _zval_internal_ptr_dtor(zval *zvalue ZEND_FILE_LINE_DC);
ZEND_API void _zval_dtor_wrapper(zval *zvalue);
#define zval_copy_ctor(zvalue) _zval_copy_ctor((zvalue) ZEND_FILE_LINE_CC)
#define zval_opt_copy_ctor(zvalue) _zval_opt_copy_ctor((zvalue) ZEND_FILE_LINE_CC)
#define zval_dtor(zvalue) zval_ptr_dtor_nogc(zvalue)
#define zval_ptr_dtor(zval_ptr) _zval_ptr_dtor((zval_ptr) ZEND_FILE_LINE_CC)
#define zval_ptr_dtor_nogc(zval_ptr) _zval_ptr_dtor_nogc((zval_ptr) ZEND_FILE_LINE_CC)
#define zval_internal_dtor(zvalue) _zval_internal_dtor((zvalue) ZEND_FILE_LINE_CC)
#define zval_internal_ptr_dtor(zvalue) _zval_internal_ptr_dtor((zvalue) ZEND_FILE_LINE_CC)
#define zval_dtor_wrapper _zval_dtor_wrapper

#if ZEND_DEBUG
ZEND_API void _zval_ptr_dtor_wrapper(zval *zval_ptr);
ZEND_API void _zval_internal_dtor_wrapper(zval *zvalue);
ZEND_API void _zval_internal_ptr_dtor_wrapper(zval *zvalue);
#define zval_ptr_dtor_wrapper _zval_ptr_dtor_wrapper
#define zval_internal_dtor_wrapper _zval_internal_dtor_wrapper
#define zval_internal_ptr_dtor_wrapper _zval_internal_ptr_dtor_wrapper
#else
#define zval_ptr_dtor_wrapper _zval_ptr_dtor
#define zval_internal_dtor_wrapper _zval_internal_dtor
#define zval_internal_ptr_dtor_wrapper _zval_internal_ptr_dtor
#endif

ZEND_API void zval_add_ref(zval *p);
ZEND_API void zval_add_ref_unref(zval *p);

END_EXTERN_C()

#define ZVAL_DESTRUCTOR zval_dtor_wrapper
#define ZVAL_PTR_DTOR zval_ptr_dtor_wrapper
#define ZVAL_INTERNAL_DTOR zval_internal_dtor_wrapper
#define ZVAL_INTERNAL_PTR_DTOR zval_internal_ptr_dtor_wrapper

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
