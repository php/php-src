/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_0.txt.                                  |
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
int spl_instanciate_arg_ex2(zend_class_entry *pce, zval **retval, zval *arg1, zval *arg2 TSRMLS_DC)
{
	zval *object;
	
	spl_instanciate(pce, &object TSRMLS_CC);
	
	retval = &EG(uninitialized_zval_ptr);
	
	spl_call_method(&object, pce, &pce->constructor, pce->constructor->common.function_name, strlen(pce->constructor->common.function_name), retval, 2, arg1, arg2 TSRMLS_CC);
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
	 */
	zend_class_entry *instance_ce;

	if (obj && (instance_ce = spl_get_class_entry(*obj TSRMLS_CC)) != NULL) {
		return instanceof_function(instance_ce, ce TSRMLS_CC);
	}
	return 0;
}
/* }}} */

/* {{{ spl_implements */
spl_is_a spl_implements(zend_class_entry *ce)
{
	register spl_is_a is_a = 0;
	register int i = ce->num_interfaces;
	register zend_class_entry **pce = ce->interfaces;

	while (i--) {
		if (*pce == spl_ce_iterator)          is_a |= SPL_IS_A_ITERATOR;
		else if (*pce == spl_ce_forward)      is_a |= SPL_IS_A_FORWARD;
		else if (*pce == spl_ce_assoc)        is_a |= SPL_IS_A_ASSOC;
		else if (*pce == spl_ce_sequence)     is_a |= SPL_IS_A_SEQUENCE;
		else if (*pce == spl_ce_array_read)   is_a |= SPL_IS_A_ARRAY_READ;
		else if (*pce == spl_ce_array_access) is_a |= SPL_IS_A_ARRAY_ACCESS;
		pce++;
	}
	return is_a;
}
/* }}} */

/* {{{ spl_call_method */
int spl_call_method(zval **object_pp, zend_class_entry *obj_ce, zend_function **fn_proxy, char *function_name, int function_name_len, zval **retval_ptr, int param_count, zval* arg1, zval* arg2 TSRMLS_DC)
{
	int result;
	zend_fcall_info fci;
	zval z_fname;
	zval *retval;

	zval **params[2];

	params[0] = &arg1;
	params[1] = &arg2;

	fci.size = sizeof(fci);
	/*fci.function_table = NULL; will be read form zend_class_entry of object if needed */
	fci.object_pp = object_pp;
	fci.function_name = &z_fname;
	fci.retval_ptr_ptr = retval_ptr ? retval_ptr : &retval;
	fci.param_count = param_count;
	fci.params = params;
	fci.no_separation = 1;
	fci.symbol_table = NULL;
	
	if (!fn_proxy && !obj_ce) {
		ZVAL_STRINGL(&z_fname, function_name, function_name_len, 0);
		result = zend_call_function(&fci, NULL TSRMLS_CC);
	} else {
		zend_fcall_info_cache fcic;

		fcic.initialized = 1;
		if (!obj_ce) {
			obj_ce = Z_OBJCE_PP(object_pp);
		}
		if (!fn_proxy || !*fn_proxy) {
			if (zend_hash_find(&obj_ce->function_table, function_name, function_name_len+1, (void **) &fcic.function_handler) == FAILURE) {
				zend_error(E_CORE_ERROR, "Couldn't find implementation for method %s::%s\n", obj_ce->name, function_name);
			}
			if (fn_proxy) {
				*fn_proxy = fcic.function_handler;
			}
		} else {
			fcic.function_handler = *fn_proxy;
		}
		fcic.calling_scope = obj_ce;
		fcic.object_pp = object_pp;
		result = zend_call_function(&fci, &fcic TSRMLS_CC);
	}
	if (!retval_ptr && retval) {
		zval_dtor(retval);
		FREE_ZVAL(retval);
	}
	return result;
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
