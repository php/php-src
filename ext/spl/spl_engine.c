/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000, 2001 The PHP Group             |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Marcus Boerger <helly@php.net>                              |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "zend_compile.h"
#include "zend_execute_locks.h"

#include "php_spl.h"
#include "spl_functions.h"
#include "spl_engine.h"

/* {{{ spl_begin_method_call_arg */
int spl_begin_method_call_arg(zval **ce, char *function_name, zval *retval, zval *arg1 TSRMLS_DC)
{
	zval *args[1];
	zval fn_name;
	
	ZVAL_STRING(&fn_name, function_name, 0);
	
	args[0] = arg1;
	return call_user_function(&Z_OBJCE_PP(ce)->function_table, ce, &fn_name, retval, 1, args TSRMLS_CC);
}
/* }}} */

/* {{{ spl_begin_method_call_this */
int spl_begin_method_call_this(zval **ce, char *function_name, zval *retval TSRMLS_DC)
{
	zval fn_name;
	
	ZVAL_STRING(&fn_name, function_name, 0);
	
	return call_user_function(&Z_OBJCE_PP(ce)->function_table, ce, &fn_name, retval, 0, NULL TSRMLS_CC);
}
/* }}} */

/* {{{ spl_begin_method_call_arg_ex1 */
int spl_begin_method_call_arg_ex1(zval **ce, char *function_name, zval **retval, zval **arg1, int no_separation, HashTable *symbol_table TSRMLS_DC)
{
	zval **args[1];
	zval fn_name;
	
	ZVAL_STRING(&fn_name, function_name, 0);
	
	args[0] = arg1;
	return call_user_function_ex(&Z_OBJCE_PP(ce)->function_table, ce, &fn_name, retval, 1, args, no_separation, symbol_table TSRMLS_CC);
}
/* }}} */

/* {{{ spl_begin_method_call_arg_ex2 */
int spl_begin_method_call_arg_ex2(zval **ce, char *function_name, zval **retval, zval **arg1, zval **arg2, int no_separation, HashTable *symbol_table TSRMLS_DC)
{
	zval **args[2];
	zval fn_name;
	
	ZVAL_STRING(&fn_name, function_name, 0);
	
	args[0] = arg1;
	args[1] = arg2;
	return call_user_function_ex(&Z_OBJCE_PP(ce)->function_table, ce, &fn_name, retval, 2, args, no_separation, symbol_table TSRMLS_CC);
}
/* }}} */

/* {{{ spl_instanciate */
void spl_instanciate(zend_class_entry *pce, zval **object TSRMLS_DC)
{
	ALLOC_ZVAL(*object);
	object_init_ex(*object, pce);
	(*object)->refcount = 1;
	(*object)->is_ref = 1; /* check if this can be hold always */
}
/* }}} */

/* {{{ spl_instanciate_arg_ex2 */
int spl_instanciate_arg_ex2(zend_class_entry *pce, zval **retval, zval **arg1, zval **arg2, int no_separation, HashTable *symbol_table TSRMLS_DC)
{
	zval **args[2];
	zval fn_name;
	zval *object;
	
	spl_instanciate(pce, &object TSRMLS_CC);
	
	retval = &EG(uninitialized_zval_ptr);
	
	ZVAL_STRING(&fn_name, pce->constructor->common.function_name, 0);
	
	args[0] = arg1;
	args[1] = arg2;
	call_user_function_ex(&pce->function_table, &object, &fn_name, retval, 2, args, no_separation, symbol_table TSRMLS_CC);
	*retval = object;
	return 0;
}
/* }}} */

/* {{{ spl_get_zval_ptr_ptr 
   Remember to call spl_unlock_ptr_ptr when needed */
zval ** spl_get_zval_ptr_ptr(znode *node, temp_variable *Ts TSRMLS_DC)
{
	if (node->op_type==IS_VAR) {
		return T(node->u.var).var.ptr_ptr;
	} else {
		return NULL;
	}
}
/* }}} */

/* {{{ spl_unlock_zval_ptr_ptr */
void spl_unlock_zval_ptr_ptr(znode *node, temp_variable *Ts TSRMLS_DC)
{
	if (node->op_type==IS_VAR) {
		if (T(node->u.var).var.ptr_ptr) {
			PZVAL_UNLOCK(*T(node->u.var).var.ptr_ptr);
		} else if (T(node->u.var).EA.type==IS_STRING_OFFSET) {
			PZVAL_UNLOCK(T(node->u.var).EA.data.str_offset.str);
		}
	}
}
/* }}} */

/* {{{ spl_get_zval_ptr */
zval * spl_get_zval_ptr(znode *node, temp_variable *Ts, zval **should_free TSRMLS_DC)
{
	switch (node->op_type) {
		case IS_CONST:
			*should_free = 0;
			return &node->u.constant;
			break;
		case IS_TMP_VAR:
			return *should_free = &T(node->u.var).tmp_var;
			break;
		case IS_VAR:
			if (T(node->u.var).var.ptr) {
				PZVAL_UNLOCK(T(node->u.var).var.ptr);
				*should_free = 0;
				return T(node->u.var).var.ptr;
			} else {
				*should_free = &T(node->u.var).tmp_var;

				switch (T(node->u.var).EA.type) {
					case IS_STRING_OFFSET: {
							temp_variable *T = &T(node->u.var);
							zval *str = T->EA.data.str_offset.str;

							if (T->EA.data.str_offset.str->type != IS_STRING
								|| (T->EA.data.str_offset.offset<0)
								|| (T->EA.data.str_offset.str->value.str.len <= T->EA.data.str_offset.offset)) {
								zend_error(E_NOTICE, "Uninitialized string offset:  %d", T->EA.data.str_offset.offset);
								T->tmp_var.value.str.val = empty_string;
								T->tmp_var.value.str.len = 0;
							} else {
								char c = str->value.str.val[T->EA.data.str_offset.offset];

								T->tmp_var.value.str.val = estrndup(&c, 1);
								T->tmp_var.value.str.len = 1;
							}
							PZVAL_UNLOCK(str);
							T->tmp_var.refcount=1;
							T->tmp_var.is_ref=1;
							T->tmp_var.type = IS_STRING;
							return &T->tmp_var;
						}
						break;
				}
			}
			break;
		case IS_UNUSED:
			*should_free = 0;
			return NULL;
			break;
		EMPTY_SWITCH_DEFAULT_CASE()
	}
	return NULL;
}
/* }}} */

/* {{{ spl_is_instance_of */
int spl_is_instance_of(zval **obj, zend_class_entry *ce TSRMLS_DC)
{
	/* Ensure everything needed is available before checking for the type.
	 * HAS_CLASS_ENTRY is neededto ensure Z_OBJCE_PP will not throw an error.
	 */
	if (!obj || !*obj || Z_TYPE_PP(obj) != IS_OBJECT || !HAS_CLASS_ENTRY(**obj)) {
		return 0;
	} else {
		zend_class_entry *instance_ce = Z_OBJCE_PP(obj);
	
		if (instanceof_function(instance_ce, ce TSRMLS_CC)) {
			return 1;
		} else {
			return 0;
		}
	}
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */
