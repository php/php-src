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
int spl_instanciate_arg_ex2(zend_class_entry *pce, zval **retval, zval *arg1, zval *arg2, HashTable *symbol_table TSRMLS_DC)
{
	zval *object;
	
	spl_instanciate(pce, &object TSRMLS_CC);
	
	retval = &EG(uninitialized_zval_ptr);
	
	spl_call_method(&object, NULL, pce->constructor->common.function_name, strlen(pce->constructor->common.function_name), retval, NULL TSRMLS_CC, 2, arg1, arg2);
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

/* {{{ */
inline zend_class_entry *spl_get_class_entry(zval *obj TSRMLS_DC)
{
	if (obj && Z_TYPE_P(obj) == IS_OBJECT && Z_OBJ_HT_P(obj)->get_class_entry) {
		return Z_OBJ_HT_P(obj)->get_class_entry(obj TSRMLS_CC);
	} else {
		return NULL;
	}
}
/* }}} */

/* {{{ spl_is_instance_of */
int spl_is_instance_of(zval **obj, zend_class_entry *ce TSRMLS_DC)
{
	/* Ensure everything needed is available before checking for the type.
	 */
	zend_class_entry *instance_ce;

	if (obj && (instance_ce = spl_get_class_entry(*obj TSRMLS_CC)) != NULL) {
		if (instanceof_function(instance_ce, ce TSRMLS_CC)) {
			return 1;
		}
	}
	return 0;
}
/* }}} */

/* {{{ spl_implements */
int spl_implements(zval **obj, zend_class_entry *ce TSRMLS_DC)
{
	/* Ensure everything needed is available before checking for the type.
	 */
	zend_class_entry *instance_ce;

	if (obj && (instance_ce = spl_get_class_entry(*obj TSRMLS_CC)) != NULL) {
		int i;

		while (instance_ce) {
			for (i = 0; i < instance_ce->num_interfaces; i++) {
				if (instance_ce->interfaces[i] == ce) {
					return 1;
				}
			}
			instance_ce = instance_ce->parent;
		}
	}
	return spl_is_instance_of(obj, ce TSRMLS_CC);
}
/* }}} */

#undef EX
#define EX(element) execute_data.element

/* {{{ spl_call_method */
int spl_call_method(zval **object_pp, zend_function **fn_proxy, char *function_name, int fname_len, zval **retval, HashTable *symbol_table TSRMLS_DC, int param_count, ...)
{
	int i;
	zval **original_return_value;
	HashTable *calling_symbol_table;
	zend_function_state *original_function_state_ptr;
	zend_op_array *original_op_array;
	zend_op **original_opline_ptr;
	zval *orig_free_op1, *orig_free_op2;
	int (*orig_unary_op)(zval *result, zval *op1);
	int (*orig_binary_op)(zval *result, zval *op1, zval *op2 TSRMLS_DC);
	zend_class_entry *current_scope;
	zend_class_entry *calling_scope = NULL;
	zval *current_this;
	zend_namespace *current_namespace = EG(active_namespace);
	zend_execute_data execute_data;
	zend_class_entry *obj_ce;
	va_list args;

	if (!object_pp || (obj_ce = spl_get_class_entry(*object_pp TSRMLS_CC)) == NULL) {
		return FAILURE;
	}

	/* Initialize execute_data */
	EX(fbc) = NULL;
	EX(Ts) = NULL;
	EX(op_array) = NULL;
	EX(opline) = NULL;

	EX(object) = *object_pp;
	calling_scope = Z_OBJCE_PP(object_pp);

	original_function_state_ptr = EG(function_state_ptr);
	if (fn_proxy && *fn_proxy) {
		EX(function_state).function = *fn_proxy;
	} else {
		if (zend_hash_find(&obj_ce->function_table, function_name, fname_len+1, (void **) &EX(function_state).function)==FAILURE) {
			return FAILURE;
		}
		if (fn_proxy) {
			*fn_proxy = EX(function_state).function;
		}
	}

	va_start(args, param_count);
	for (i=0; i<param_count; i++) {
		zval *arg;
		zval *param;

		arg = va_arg(args, zval*);

		if (EX(function_state).function->common.arg_types
			&& i<EX(function_state).function->common.arg_types[0]
			&& EX(function_state).function->common.arg_types[i+1]==BYREF_FORCE
			&& !PZVAL_IS_REF(arg)) {
			if (arg->refcount > 1) {
				zval *new_zval;

				ALLOC_ZVAL(new_zval);
				*new_zval = *arg;
				zval_copy_ctor(new_zval);
				new_zval->refcount = 1;
				arg->refcount--;
				arg = new_zval;
			}
			arg->refcount++;
			arg->is_ref = 1;
			param = arg;
		} else if (arg != &EG(uninitialized_zval)) {
			arg->refcount++;
			param = arg;
		} else {
			ALLOC_ZVAL(param);
			*param = *arg;
			INIT_PZVAL(param);
		}
		zend_ptr_stack_push(&EG(argument_stack), param);
	}
	va_end(args);

	zend_ptr_stack_n_push(&EG(argument_stack), 2, (void *) (long) param_count, NULL);

	EG(function_state_ptr) = &EX(function_state);

	current_scope = EG(scope);
	EG(scope) = calling_scope;

	current_this = EG(This);

	EG(This) = *object_pp;

	if (!PZVAL_IS_REF(EG(This))) {
		EG(This)->refcount++; /* For $this pointer */
	} else {
		zval *this_ptr;

		ALLOC_ZVAL(this_ptr);
		*this_ptr = *EG(This);
		INIT_PZVAL(this_ptr);
		zval_copy_ctor(this_ptr);
		EG(This) = this_ptr;
	}

	EX(prev_execute_data) = EG(current_execute_data);
	EG(current_execute_data) = &execute_data;

	if (EX(function_state).function->type == ZEND_USER_FUNCTION) {
		calling_symbol_table = EG(active_symbol_table);
		if (symbol_table) {
			EG(active_symbol_table) = symbol_table;
		} else {
			ALLOC_HASHTABLE(EG(active_symbol_table));
			zend_hash_init(EG(active_symbol_table), 0, NULL, ZVAL_PTR_DTOR, 0);
		}

		original_return_value = EG(return_value_ptr_ptr);
		original_op_array = EG(active_op_array);
		EG(return_value_ptr_ptr) = retval;
		EG(active_op_array) = (zend_op_array *) EX(function_state).function;
		original_opline_ptr = EG(opline_ptr);
		orig_free_op1 = EG(free_op1);
		orig_free_op2 = EG(free_op2);
		orig_unary_op = EG(unary_op);
		orig_binary_op = EG(binary_op);
		zend_execute(EG(active_op_array) TSRMLS_CC);
		if (!symbol_table) {
			zend_hash_destroy(EG(active_symbol_table));
			FREE_HASHTABLE(EG(active_symbol_table));
		}
		EG(active_symbol_table) = calling_symbol_table;
		EG(active_op_array) = original_op_array;
		EG(return_value_ptr_ptr)=original_return_value;
		EG(opline_ptr) = original_opline_ptr;
		EG(free_op1) = orig_free_op1;
		EG(free_op2) = orig_free_op2;
		EG(unary_op) = orig_unary_op;
		EG(binary_op) = orig_binary_op;
	} else {
		ALLOC_INIT_ZVAL(*retval);
		((zend_internal_function *) EX(function_state).function)->handler(param_count, *retval, (object_pp?*object_pp:NULL), 1 TSRMLS_CC);
		INIT_PZVAL(*retval);
	}
	zend_ptr_stack_clear_multiple(TSRMLS_C);
	EG(function_state_ptr) = original_function_state_ptr;
	EG(active_namespace) = current_namespace;

	if (EG(This)) {
		zval_ptr_dtor(&EG(This));
	}
	EG(scope) = current_scope;
	EG(This) = current_this;
	EG(current_execute_data) = EX(prev_execute_data);                       \

	return SUCCESS;
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
