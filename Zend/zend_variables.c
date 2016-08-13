/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2016 Zend Technologies Ltd. (http://www.zend.com) |
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

ZEND_API void ZEND_FASTCALL _zval_dtor_func(zend_refcounted *p ZEND_FILE_LINE_DC)
{
	switch (GC_TYPE(p)) {
		case IS_STRING:
		case IS_CONSTANT: {
				zend_string *str = (zend_string*)p;
				CHECK_ZVAL_STRING_REL(str);
				zend_string_free(str);
				break;
			}
		case IS_ARRAY: {
				zend_array *arr = (zend_array*)p;
				zend_array_destroy(arr);
				break;
			}
		case IS_CONSTANT_AST: {
				zend_ast_ref *ast = (zend_ast_ref*)p;

				zend_ast_destroy_and_free(ast->ast);
				efree_size(ast, sizeof(zend_ast_ref));
				break;
			}
		case IS_OBJECT: {
				zend_object *obj = (zend_object*)p;

				zend_objects_store_del(obj);
				break;
			}
		case IS_RESOURCE: {
				zend_resource *res = (zend_resource*)p;

				/* destroy resource */
				zend_list_free(res);
				break;
			}
		case IS_REFERENCE: {
				zend_reference *ref = (zend_reference*)p;

				i_zval_ptr_dtor(&ref->val ZEND_FILE_LINE_RELAY_CC);
				efree_size(ref, sizeof(zend_reference));
				break;
			}
		default:
			break;
	}
}

ZEND_API void _zval_internal_dtor(zval *zvalue ZEND_FILE_LINE_DC)
{
	switch (Z_TYPE_P(zvalue)) {
		case IS_STRING:
		case IS_CONSTANT:
			CHECK_ZVAL_STRING_REL(Z_STR_P(zvalue));
			zend_string_release(Z_STR_P(zvalue));
			break;
		case IS_ARRAY:
		case IS_CONSTANT_AST:
		case IS_OBJECT:
		case IS_RESOURCE:
			zend_error_noreturn(E_CORE_ERROR, "Internal zval's can't be arrays, objects or resources");
			break;
		case IS_REFERENCE: {
				zend_reference *ref = (zend_reference*)Z_REF_P(zvalue);

				zval_internal_ptr_dtor(&ref->val);
				free(ref);
				break;
			}
		case IS_LONG:
		case IS_DOUBLE:
		case IS_FALSE:
		case IS_TRUE:
		case IS_NULL:
		default:
			break;
	}
}

ZEND_API void _zval_internal_dtor_for_ptr(zval *zvalue ZEND_FILE_LINE_DC)
{
	switch (Z_TYPE_P(zvalue)) {
		case IS_STRING:
		case IS_CONSTANT:
			CHECK_ZVAL_STRING_REL(Z_STR_P(zvalue));
			zend_string_free(Z_STR_P(zvalue));
			break;
		case IS_ARRAY:
		case IS_CONSTANT_AST:
		case IS_OBJECT:
		case IS_RESOURCE:
			zend_error_noreturn(E_CORE_ERROR, "Internal zval's can't be arrays, objects or resources");
			break;
		case IS_REFERENCE: {
				zend_reference *ref = (zend_reference*)Z_REF_P(zvalue);

				zval_internal_ptr_dtor(&ref->val);
				free(ref);
				break;
			}
		case IS_LONG:
		case IS_DOUBLE:
		case IS_FALSE:
		case IS_TRUE:
		case IS_NULL:
		default:
			break;
	}
}

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

ZEND_API void zval_add_ref_unref(zval *p)
{
	if (Z_REFCOUNTED_P(p)) {
		if (Z_ISREF_P(p)) {
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
	} else if (EXPECTED(Z_TYPE_P(zvalue) == IS_STRING) ||
	           EXPECTED(Z_TYPE_P(zvalue) == IS_CONSTANT)) {
		CHECK_ZVAL_STRING_REL(Z_STR_P(zvalue));
		Z_STR_P(zvalue) = zend_string_dup(Z_STR_P(zvalue), 0);
	} else if (EXPECTED(Z_TYPE_P(zvalue) == IS_CONSTANT_AST)) {
		zend_ast *copy = zend_ast_copy(Z_ASTVAL_P(zvalue));
		ZVAL_NEW_AST(zvalue, copy);
	}
}


ZEND_API size_t zend_print_variable(zval *var)
{
	return zend_print_zval(var, 0);
}


ZEND_API void _zval_dtor_wrapper(zval *zvalue)
{
	zval_dtor(zvalue);
}


#if ZEND_DEBUG
ZEND_API void _zval_internal_dtor_wrapper(zval *zvalue)
{
	zval_internal_dtor(zvalue);
}


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
