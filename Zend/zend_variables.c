/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2013 Zend Technologies Ltd. (http://www.zend.com) |
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
			str_efree_rel(zvalue->value.str.val);
			break;
		case IS_ARRAY:
		case IS_CONSTANT_ARRAY: {
				TSRMLS_FETCH();

				if (zvalue->value.ht && (zvalue->value.ht != &EG(symbol_table))) {
					/* break possible cycles */
					Z_TYPE_P(zvalue) = IS_NULL;
					zend_hash_destroy(zvalue->value.ht);
					FREE_HASHTABLE(zvalue->value.ht);
				}
			}
			break;
		case IS_CONSTANT_AST:
			zend_ast_destroy(Z_AST_P(zvalue));
			break;
		case IS_OBJECT:
			{
				TSRMLS_FETCH();

				Z_OBJ_HT_P(zvalue)->del_ref(zvalue TSRMLS_CC);
			}
			break;
		case IS_RESOURCE:
			{
				TSRMLS_FETCH();

				/* destroy resource */
				zend_list_delete(zvalue->value.lval);
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


ZEND_API void _zval_internal_dtor(zval *zvalue ZEND_FILE_LINE_DC)
{
	switch (Z_TYPE_P(zvalue) & IS_CONSTANT_TYPE_MASK) {
		case IS_STRING:
		case IS_CONSTANT:
			CHECK_ZVAL_STRING_REL(zvalue);
			str_free(zvalue->value.str.val);
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


ZEND_API void zval_add_ref(zval **p)
{
	Z_ADDREF_PP(p);
}


ZEND_API void _zval_copy_ctor_func(zval *zvalue ZEND_FILE_LINE_DC)
{
	switch (Z_TYPE_P(zvalue) & IS_CONSTANT_TYPE_MASK) {
		case IS_RESOURCE: {
				TSRMLS_FETCH();

				zend_list_addref(zvalue->value.lval);
			}
			break;
		case IS_BOOL:
		case IS_LONG:
		case IS_NULL:
			break;
		case IS_CONSTANT:
		case IS_STRING:
			CHECK_ZVAL_STRING_REL(zvalue);
			if (!IS_INTERNED(zvalue->value.str.val)) {
				zvalue->value.str.val = (char *) estrndup_rel(zvalue->value.str.val, zvalue->value.str.len);
			}
			break;
		case IS_ARRAY:
		case IS_CONSTANT_ARRAY: {
				zval *tmp;
				HashTable *original_ht = zvalue->value.ht;
				HashTable *tmp_ht = NULL;
				TSRMLS_FETCH();

				if (zvalue->value.ht == &EG(symbol_table)) {
					return; /* do nothing */
				}
				ALLOC_HASHTABLE_REL(tmp_ht);
				zend_hash_init(tmp_ht, zend_hash_num_elements(original_ht), NULL, ZVAL_PTR_DTOR, 0);
				zend_hash_copy(tmp_ht, original_ht, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));
				zvalue->value.ht = tmp_ht;
			}
			break;
		case IS_CONSTANT_AST:
			Z_AST_P(zvalue) = zend_ast_copy(Z_AST_P(zvalue));
			break;
		case IS_OBJECT:
			{
				TSRMLS_FETCH();
				Z_OBJ_HT_P(zvalue)->add_ref(zvalue TSRMLS_CC);
			}
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


ZEND_API void _zval_ptr_dtor_wrapper(zval **zval_ptr)
{
	zval_ptr_dtor(zval_ptr);
}


ZEND_API void _zval_internal_ptr_dtor_wrapper(zval **zval_ptr)
{
	zval_internal_ptr_dtor(zval_ptr);
}
#endif

ZEND_API int zval_copy_static_var(zval **p TSRMLS_DC, int num_args, va_list args, zend_hash_key *key) /* {{{ */
{
	HashTable *target = va_arg(args, HashTable*);
	zend_bool is_ref;
	zval *tmp;
  
	if (Z_TYPE_PP(p) & (IS_LEXICAL_VAR|IS_LEXICAL_REF)) {
		is_ref = Z_TYPE_PP(p) & IS_LEXICAL_REF;
    
		if (!EG(active_symbol_table)) {
			zend_rebuild_symbol_table(TSRMLS_C);
		}
		if (zend_hash_quick_find(EG(active_symbol_table), key->arKey, key->nKeyLength, key->h, (void **) &p) == FAILURE) {
			if (is_ref) {        
				ALLOC_INIT_ZVAL(tmp);
				Z_SET_ISREF_P(tmp);
				zend_hash_quick_add(EG(active_symbol_table), key->arKey, key->nKeyLength, key->h, &tmp, sizeof(zval*), (void**)&p);
			} else {
				tmp = EG(uninitialized_zval_ptr);
				zend_error(E_NOTICE,"Undefined variable: %s", key->arKey);
			}
		} else {
			if (is_ref) {
				SEPARATE_ZVAL_TO_MAKE_IS_REF(p);
				tmp = *p;
			} else if (Z_ISREF_PP(p)) {
				ALLOC_INIT_ZVAL(tmp);
				ZVAL_COPY_VALUE(tmp, *p);
				zval_copy_ctor(tmp);
				Z_SET_REFCOUNT_P(tmp, 0);
				Z_UNSET_ISREF_P(tmp);
			} else {
				tmp = *p;
			}
		}
	} else {
		tmp = *p;
	}
	if (zend_hash_quick_add(target, key->arKey, key->nKeyLength, key->h, &tmp, sizeof(zval*), NULL) == SUCCESS) {
		Z_ADDREF_P(tmp);
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
