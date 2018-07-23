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

#include <stdio.h>
#include "zend.h"
#include "zend_API.h"
#include "zend_ast.h"
#include "zend_globals.h"
#include "zend_constants.h"
#include "zend_list.h"

static void ZEND_FASTCALL zend_string_destroy(zend_string *str ZEND_FILE_LINE_DC);
static void ZEND_FASTCALL zend_reference_destroy(zend_reference *ref ZEND_FILE_LINE_DC);
static void ZEND_FASTCALL zend_empty_destroy(zend_reference *ref ZEND_FILE_LINE_DC);

#if ZEND_DEBUG
static void ZEND_FASTCALL zend_array_destroy_wrapper(zend_array *arr ZEND_FILE_LINE_DC);
static void ZEND_FASTCALL zend_object_destroy_wrapper(zend_object *obj ZEND_FILE_LINE_DC);
static void ZEND_FASTCALL zend_resource_destroy_wrapper(zend_resource *res ZEND_FILE_LINE_DC);
static void ZEND_FASTCALL zend_ast_ref_destroy_wrapper(zend_ast_ref *ast ZEND_FILE_LINE_DC);
#else
# define zend_array_destroy_wrapper    zend_array_destroy
# define zend_object_destroy_wrapper   zend_objects_store_del
# define zend_resource_destroy_wrapper zend_list_free
# define zend_ast_ref_destroy_wrapper  zend_ast_ref_destroy
#endif

typedef void (ZEND_FASTCALL *zend_rc_dtor_func_t)(zend_refcounted *p ZEND_FILE_LINE_DC);

static const zend_rc_dtor_func_t zend_rc_dtor_func[] = {
	/* IS_UNDEF        */ (zend_rc_dtor_func_t)zend_empty_destroy,
	/* IS_NULL         */ (zend_rc_dtor_func_t)zend_empty_destroy,
	/* IS_FALSE        */ (zend_rc_dtor_func_t)zend_empty_destroy,
	/* IS_TRUE         */ (zend_rc_dtor_func_t)zend_empty_destroy,
	/* IS_LONG         */ (zend_rc_dtor_func_t)zend_empty_destroy,
	/* IS_DOUBLE       */ (zend_rc_dtor_func_t)zend_empty_destroy,
	/* IS_STRING       */ (zend_rc_dtor_func_t)zend_string_destroy,
	/* IS_ARRAY        */ (zend_rc_dtor_func_t)zend_array_destroy_wrapper,
	/* IS_OBJECT       */ (zend_rc_dtor_func_t)zend_object_destroy_wrapper,
	/* IS_RESOURCE     */ (zend_rc_dtor_func_t)zend_resource_destroy_wrapper,
	/* IS_REFERENCE    */ (zend_rc_dtor_func_t)zend_reference_destroy,
	/* IS_CONSTANT_AST */ (zend_rc_dtor_func_t)zend_ast_ref_destroy_wrapper
};

ZEND_API void ZEND_FASTCALL _rc_dtor_func(zend_refcounted *p ZEND_FILE_LINE_DC)
{
	ZEND_ASSERT(GC_TYPE(p) <= IS_CONSTANT_AST);
	zend_rc_dtor_func[GC_TYPE(p)](p ZEND_FILE_LINE_RELAY_CC);
}

static void ZEND_FASTCALL zend_string_destroy(zend_string *str ZEND_FILE_LINE_DC)
{
	CHECK_ZVAL_STRING_REL(str);
	ZEND_ASSERT(!ZSTR_IS_INTERNED(str));
	ZEND_ASSERT(GC_REFCOUNT(str) == 0);
	ZEND_ASSERT(!(GC_FLAGS(str) & IS_STR_PERSISTENT));
	efree(str);
}

static void ZEND_FASTCALL zend_reference_destroy(zend_reference *ref ZEND_FILE_LINE_DC)
{
	i_zval_ptr_dtor(&ref->val ZEND_FILE_LINE_RELAY_CC);
	efree_size(ref, sizeof(zend_reference));
}

static void ZEND_FASTCALL zend_empty_destroy(zend_reference *ref ZEND_FILE_LINE_DC)
{
}

#if ZEND_DEBUG
static void ZEND_FASTCALL zend_array_destroy_wrapper(zend_array *arr ZEND_FILE_LINE_DC)
{
	zend_array_destroy(arr);
}

static void ZEND_FASTCALL zend_object_destroy_wrapper(zend_object *obj ZEND_FILE_LINE_DC)
{
	zend_objects_store_del(obj);
}

static void ZEND_FASTCALL zend_resource_destroy_wrapper(zend_resource *res ZEND_FILE_LINE_DC)
{
	zend_list_free(res);
}

static void ZEND_FASTCALL zend_ast_ref_destroy_wrapper(zend_ast_ref *ast ZEND_FILE_LINE_DC)
{
	zend_ast_ref_destroy(ast);
}
#endif

ZEND_API void _zval_ptr_dtor(zval *zval_ptr ZEND_FILE_LINE_DC) /* {{{ */
{
	i_zval_ptr_dtor(zval_ptr ZEND_FILE_LINE_RELAY_CC);
}
/* }}} */

ZEND_API void _zval_internal_ptr_dtor(zval *zval_ptr ZEND_FILE_LINE_DC) /* {{{ */
{
	if (Z_REFCOUNTED_P(zval_ptr)) {
		zend_refcounted *ref = Z_COUNTED_P(zval_ptr);

		if (GC_DELREF(ref) == 0) {
			if (Z_TYPE_P(zval_ptr) == IS_STRING) {
				zend_string *str = (zend_string*)ref;

				CHECK_ZVAL_STRING_REL(str);
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

ZEND_API void ZEND_FASTCALL _zval_copy_ctor_func(zval *zvalue ZEND_FILE_LINE_DC)
{
	if (EXPECTED(Z_TYPE_P(zvalue) == IS_ARRAY)) {
		ZVAL_ARR(zvalue, zend_array_dup(Z_ARRVAL_P(zvalue)));
	} else if (EXPECTED(Z_TYPE_P(zvalue) == IS_STRING)) {
		ZEND_ASSERT(!ZSTR_IS_INTERNED(Z_STR_P(zvalue)));
		CHECK_ZVAL_STRING_REL(Z_STR_P(zvalue));
		ZVAL_NEW_STR(zvalue, zend_string_dup(Z_STR_P(zvalue), 0));
	}
}


#if ZEND_DEBUG
ZEND_API void _zval_ptr_dtor_wrapper(zval *zval_ptr)
{

	i_zval_ptr_dtor(zval_ptr ZEND_FILE_LINE_CC);
}


ZEND_API void _zval_internal_ptr_dtor_wrapper(zval *zval_ptr)
{
	zval_internal_ptr_dtor(zval_ptr);
}
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
