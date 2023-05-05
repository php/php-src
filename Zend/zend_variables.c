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
   |          Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

#include <stdio.h>
#include "zend.h"
#include "zend_API.h"
#include "zend_ast.h"
#include "zend_globals.h"
#include "zend_constants.h"
#include "zend_list.h"

#if ZEND_DEBUG
static void ZEND_FASTCALL zend_string_destroy(zend_string *str);
#else
# define zend_string_destroy _efree
#endif
static void ZEND_FASTCALL zend_reference_destroy(zend_reference *ref);
static void ZEND_FASTCALL zend_empty_destroy(zend_reference *ref);

typedef void (ZEND_FASTCALL *zend_rc_dtor_func_t)(zend_refcounted *p);

static const zend_rc_dtor_func_t zend_rc_dtor_func[] = {
	[IS_UNDEF] =        (zend_rc_dtor_func_t)zend_empty_destroy,
	[IS_NULL] =         (zend_rc_dtor_func_t)zend_empty_destroy,
	[IS_FALSE] =        (zend_rc_dtor_func_t)zend_empty_destroy,
	[IS_TRUE] =         (zend_rc_dtor_func_t)zend_empty_destroy,
	[IS_LONG] =         (zend_rc_dtor_func_t)zend_empty_destroy,
	[IS_DOUBLE] =       (zend_rc_dtor_func_t)zend_empty_destroy,
	[IS_STRING] =       (zend_rc_dtor_func_t)zend_string_destroy,
	[IS_ARRAY] =        (zend_rc_dtor_func_t)zend_array_destroy,
	[IS_OBJECT] =       (zend_rc_dtor_func_t)zend_objects_store_del,
	[IS_RESOURCE] =     (zend_rc_dtor_func_t)zend_list_free,
	[IS_REFERENCE] =    (zend_rc_dtor_func_t)zend_reference_destroy,
	[IS_CONSTANT_AST] = (zend_rc_dtor_func_t)zend_ast_ref_destroy
};

ZEND_API void ZEND_FASTCALL rc_dtor_func(zend_refcounted *p)
{
	ZEND_ASSERT(GC_TYPE(p) <= IS_CONSTANT_AST);
	zend_rc_dtor_func[GC_TYPE(p)](p);
}

#if ZEND_DEBUG
static void ZEND_FASTCALL zend_string_destroy(zend_string *str)
{
	CHECK_ZVAL_STRING(str);
	ZEND_ASSERT(!ZSTR_IS_INTERNED(str));
	ZEND_ASSERT(GC_REFCOUNT(str) == 0);
	ZEND_ASSERT(!(GC_FLAGS(str) & IS_STR_PERSISTENT));
	efree(str);
}
#endif

static void ZEND_FASTCALL zend_reference_destroy(zend_reference *ref)
{
	ZEND_ASSERT(!ZEND_REF_HAS_TYPE_SOURCES(ref));
	i_zval_ptr_dtor(&ref->val);
	efree_size(ref, sizeof(zend_reference));
}

static void ZEND_FASTCALL zend_empty_destroy(zend_reference *ref)
{
}

ZEND_API void zval_ptr_dtor(zval *zval_ptr) /* {{{ */
{
	i_zval_ptr_dtor(zval_ptr);
}
/* }}} */

ZEND_API void zval_internal_ptr_dtor(zval *zval_ptr) /* {{{ */
{
	if (Z_REFCOUNTED_P(zval_ptr)) {
		zend_refcounted *ref = Z_COUNTED_P(zval_ptr);

		if (GC_DELREF(ref) == 0) {
			if (Z_TYPE_P(zval_ptr) == IS_STRING) {
				zend_string *str = (zend_string*)ref;

				CHECK_ZVAL_STRING(str);
				ZEND_ASSERT(!ZSTR_IS_INTERNED(str));
				ZEND_ASSERT((GC_FLAGS(str) & IS_STR_PERSISTENT));
				free(str);
			} else {
				zend_error_noreturn(E_CORE_ERROR, "Internal zval's can't be arrays, objects, resources or reference");
			}
		}
	}
}
/* }}} */

/* This function should only be used as a copy constructor, i.e. it
 * should only be called AFTER a zval has been copied to another
 * location using ZVAL_COPY_VALUE. Do not call it before copying,
 * otherwise a reference may be leaked. */
ZEND_API void zval_add_ref(zval *p)
{
	if (Z_REFCOUNTED_P(p)) {
		if (Z_ISREF_P(p) && Z_REFCOUNT_P(p) == 1) {
			ZVAL_COPY(p, Z_REFVAL_P(p));
		} else {
			Z_ADDREF_P(p);
		}
	}
}

ZEND_API void ZEND_FASTCALL zval_copy_ctor_func(zval *zvalue)
{
	if (EXPECTED(Z_TYPE_P(zvalue) == IS_ARRAY)) {
		ZVAL_ARR(zvalue, zend_array_dup(Z_ARRVAL_P(zvalue)));
	} else if (EXPECTED(Z_TYPE_P(zvalue) == IS_STRING)) {
		ZEND_ASSERT(!ZSTR_IS_INTERNED(Z_STR_P(zvalue)));
		CHECK_ZVAL_STRING(Z_STR_P(zvalue));
		ZVAL_NEW_STR(zvalue, zend_string_dup(Z_STR_P(zvalue), 0));
	} else {
		ZEND_UNREACHABLE();
	}
}
