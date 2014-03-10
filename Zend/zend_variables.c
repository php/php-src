/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2014 Zend Technologies Ltd. (http://www.zend.com) |
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

ZEND_API void _zval_dtor_func(zval *zvalue ZEND_FILE_LINE_DC)
{
	switch (Z_TYPE_P(zvalue) & IS_CONSTANT_TYPE_MASK) {
		case IS_STRING:
		case IS_CONSTANT:
			CHECK_ZVAL_STRING_REL(zvalue);
			STR_RELEASE(Z_STR_P(zvalue));
			break;
		case IS_ARRAY:
		case IS_CONSTANT_ARRAY: {
				TSRMLS_FETCH();

				if (Z_ARRVAL_P(zvalue) != &EG(symbol_table).ht) {
					/* break possible cycles */
					Z_TYPE_P(zvalue) = IS_NULL;
					zend_hash_destroy(Z_ARRVAL_P(zvalue));
					efree(Z_ARR_P(zvalue));
				}
			}
			break;
		case IS_CONSTANT_AST:
			zend_ast_destroy(Z_AST_P(zvalue)->ast);
			efree(Z_AST_P(zvalue));
			break;
		case IS_OBJECT:
			{
				TSRMLS_FETCH();

				OBJ_RELEASE(Z_OBJ_P(zvalue));
			}
			break;
		case IS_RESOURCE:
			{
				TSRMLS_FETCH();

				if (Z_DELREF_P(zvalue) == 0) {
					/* destroy resource */
					zend_list_delete(Z_RES_P(zvalue));
				}
			}
			break;
		case IS_REFERENCE:
			if (Z_DELREF_P(zvalue) == 0) {
				zval_dtor(Z_REFVAL_P(zvalue));
				efree(Z_REF_P(zvalue));
			}
			break;
		case IS_LONG:
		case IS_DOUBLE:
		case IS_BOOL:
		case IS_NULL:
		default:
			return;
			break;
	}
}

ZEND_API void _zval_dtor_func_for_ptr(zval *zvalue ZEND_FILE_LINE_DC)
{
	switch (Z_TYPE_P(zvalue) & IS_CONSTANT_TYPE_MASK) {
		case IS_STRING:
		case IS_CONSTANT:
			CHECK_ZVAL_STRING_REL(zvalue);
			STR_FREE(Z_STR_P(zvalue));
			break;
		case IS_ARRAY:
		case IS_CONSTANT_ARRAY: {
				TSRMLS_FETCH();

				if (Z_ARRVAL_P(zvalue) != &EG(symbol_table).ht) {
					/* break possible cycles */
					Z_TYPE_P(zvalue) = IS_NULL;
					zend_hash_destroy(Z_ARRVAL_P(zvalue));
					efree(Z_ARR_P(zvalue));
				}
			}
			break;
		case IS_CONSTANT_AST:
			zend_ast_destroy(Z_AST_P(zvalue)->ast);
			efree(Z_AST_P(zvalue));
			break;
		case IS_OBJECT:
			{
				TSRMLS_FETCH();

				zend_objects_store_del(Z_OBJ_P(zvalue) TSRMLS_CC);
			}
			break;
		case IS_RESOURCE:
			{
				TSRMLS_FETCH();

				/* destroy resource */
				zend_list_delete(Z_RES_P(zvalue));
			}
			break;
		case IS_REFERENCE:
			zval_dtor(Z_REFVAL_P(zvalue));
			efree(Z_REF_P(zvalue));
			break;
		case IS_LONG:
		case IS_DOUBLE:
		case IS_BOOL:
		case IS_NULL:
		default:
			return;
			break;
	}
}

ZEND_API void _zval_internal_dtor(zval *zvalue ZEND_FILE_LINE_DC)
{
	switch (Z_TYPE_P(zvalue) & IS_CONSTANT_TYPE_MASK) {
		case IS_STRING:
		case IS_CONSTANT:
			CHECK_ZVAL_STRING_REL(zvalue);
			STR_RELEASE(Z_STR_P(zvalue));
			break;
		case IS_ARRAY:
		case IS_CONSTANT_ARRAY:
		case IS_CONSTANT_AST:
		case IS_OBJECT:
		case IS_RESOURCE:
			zend_error(E_CORE_ERROR, "Internal zval's can't be arrays, objects or resources");
			break;
		case IS_LONG:
		case IS_DOUBLE:
		case IS_BOOL:
		case IS_NULL:
		default:
			break;
	}
}

ZEND_API void _zval_internal_dtor_for_ptr(zval *zvalue ZEND_FILE_LINE_DC)
{
	switch (Z_TYPE_P(zvalue) & IS_CONSTANT_TYPE_MASK) {
		case IS_STRING:
		case IS_CONSTANT:
			CHECK_ZVAL_STRING_REL(zvalue);
			STR_FREE(Z_STR_P(zvalue));
			break;
		case IS_ARRAY:
		case IS_CONSTANT_ARRAY:
		case IS_CONSTANT_AST:
		case IS_OBJECT:
		case IS_RESOURCE:
			zend_error(E_CORE_ERROR, "Internal zval's can't be arrays, objects or resources");
			break;
		case IS_LONG:
		case IS_DOUBLE:
		case IS_BOOL:
		case IS_NULL:
		default:
			break;
	}
}

ZEND_API void zval_add_ref(zval *p)
{
	if (Z_REFCOUNTED_P(p)) {
//???: autoconversion from reverence to ordinal value
		if (Z_ISREF_P(p) && Z_REFCOUNT_P(p) == 1) {
			ZVAL_DUP(p, Z_REFVAL_P(p));
		} else {
			Z_ADDREF_P(p);
		}
	}
}

ZEND_API void zval_add_ref_unref(zval *p)
{
	if (Z_REFCOUNTED_P(p)) {
		if (Z_ISREF_P(p)) {
			if (Z_REFCOUNT_P(p) == 1) {
				zval *q = Z_REFVAL_P(p);
				ZVAL_DUP(p, q);
			} else {
				Z_ADDREF_P(p);
			}
		} else {
			Z_ADDREF_P(p);
		}
	}
}

ZEND_API void _zval_copy_ctor_func(zval *zvalue ZEND_FILE_LINE_DC)
{
	switch (Z_TYPE_P(zvalue) & IS_CONSTANT_TYPE_MASK) {
		case IS_CONSTANT:
		case IS_STRING:
			CHECK_ZVAL_STRING_REL(zvalue);
			if (!IS_INTERNED(Z_STR_P(zvalue))) {
				Z_STR_P(zvalue) = STR_DUP(Z_STR_P(zvalue), 0);
			}
			break;
		case IS_ARRAY:
		case IS_CONSTANT_ARRAY: {
				HashTable *ht = Z_ARRVAL_P(zvalue);
				TSRMLS_FETCH();

				if (ht == &EG(symbol_table).ht) {
					return; /* do nothing */
				}
				ZVAL_NEW_ARR(zvalue);
				zend_hash_init(Z_ARRVAL_P(zvalue), zend_hash_num_elements(ht), NULL, ZVAL_PTR_DTOR, 0);
				zend_hash_copy(Z_ARRVAL_P(zvalue), ht, zval_add_ref);
			}
			break;
		case IS_CONSTANT_AST: {
				zend_ast_ref *ast = emalloc(sizeof(zend_ast_ref));

				ast->gc.refcount = 1;
				ast->gc.u.v.type = IS_CONSTANT_AST;
				ast->ast = zend_ast_copy(Z_ASTVAL_P(zvalue));
				Z_AST_P(zvalue) = ast;
			}
			break;
		case IS_OBJECT:
		case IS_RESOURCE:
		case IS_REFERENCE:
			Z_ADDREF_P(zvalue);
			break;
	}
}


ZEND_API int zend_print_variable(zval *var) 
{
	return zend_print_zval(var, 0);
}


ZEND_API void _zval_dtor_wrapper(zval *zvalue)
{
	TSRMLS_FETCH();

	GC_REMOVE_ZVAL_FROM_BUFFER(zvalue);
	zval_dtor(zvalue);
}


#if ZEND_DEBUG
ZEND_API void _zval_copy_ctor_wrapper(zval *zvalue)
{
	zval_copy_ctor(zvalue);
}


ZEND_API void _zval_internal_dtor_wrapper(zval *zvalue)
{
	zval_internal_dtor(zvalue);
}


ZEND_API void _zval_ptr_dtor_wrapper(zval *zval_ptr)
{
	zval_ptr_dtor(zval_ptr);
}

ZEND_API void _zval_internal_ptr_dtor_wrapper(zval *zval_ptr)
{
	zval_internal_ptr_dtor(zval_ptr);
}
#endif

ZEND_API int zval_copy_static_var(zval *p TSRMLS_DC, int num_args, va_list args, zend_hash_key *key) /* {{{ */
{
	HashTable *target = va_arg(args, HashTable*);
	zend_bool is_ref;
	zval tmp;
  
	if (Z_TYPE_P(p) & (IS_LEXICAL_VAR|IS_LEXICAL_REF)) {
		is_ref = Z_TYPE_P(p) & IS_LEXICAL_REF;
    
		if (!EG(active_symbol_table)) {
			zend_rebuild_symbol_table(TSRMLS_C);
		}
		p = zend_hash_find(EG(active_symbol_table), key->key);
		if (!p) {
			p = &tmp;
			ZVAL_NULL(&tmp);
			if (is_ref) {
				ZVAL_NEW_REF(&tmp, &tmp);
				zend_hash_add(EG(active_symbol_table), key->key, &tmp);
			} else {
				zend_error(E_NOTICE,"Undefined variable: %s", key->key->val);
			}
		} else {
			if (is_ref) {
				SEPARATE_ZVAL_TO_MAKE_IS_REF(p);
/*
				if (!Z_ISREF_P(p)) {
					if (Z_REFCOUNTED_P(p) && Z_REFCOUNT_P(p) > 1) {
						Z_DELREF_P(p);
						ZVAL_NEW_REF(p, p);
						zval_copy_ctor(Z_REFVAL_P(p));
						Z_SET_REFCOUNT_P(Z_REFVAL_P(p), 1);
					} else {
						ZVAL_NEW_REF(p, p);
					}
				}
*/
			} else if (Z_ISREF_P(p)) {
				ZVAL_COPY_VALUE(&tmp, Z_REFVAL_P(p));
				if (Z_REFCOUNTED(tmp) && Z_REFCOUNT(tmp) > 1) {
					zval_copy_ctor(&tmp);
					Z_SET_REFCOUNT(tmp, 0);
				}
				p = &tmp;
			}
		}
	}
	if (zend_hash_add(target, key->key, p)) {
		if (Z_REFCOUNTED_P(p)) {
			Z_ADDREF_P(p);
		}
	}
	return ZEND_HASH_APPLY_KEEP;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
