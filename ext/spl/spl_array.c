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
#include "spl_array.h"

#define DELETE_ZVAL(z) \
	if ((z)->refcount < 2) { \
		zval_dtor(z); \
		FREE_ZVAL(z); /* maybe safe_free_zval_ptr is needed for the uninitialised things */ \
	}

#define DELETE_RET_ZVAL(z) \
	if ((z)->refcount < 3) { \
		zval_dtor(z); \
		FREE_ZVAL(z); /* maybe safe_free_zval_ptr is needed for the uninitialised things */ \
	}

#define AI_PTR_2_PTR_PTR(ai) \
	(ai).ptr_ptr = &((ai).ptr)

/* {{{ spl_fetch_dimension_address */
int spl_fetch_dimension_address(znode *result, znode *op1, znode *op2, temp_variable *Ts, int type TSRMLS_DC)
{
	zval **obj;
	zend_class_entry *obj_ce;
	spl_is_a is_a;

	obj = spl_get_zval_ptr_ptr(op1, Ts TSRMLS_CC);

	if (!obj || (obj_ce = spl_get_class_entry(*obj TSRMLS_CC)) == NULL) {
		return 1;
	}

	is_a = spl_implements(obj_ce);

	if (is_a & SPL_IS_A_ARRAY_READ) {
		zval **retval = &(T(result->u.var).var.ptr);
		zval *dim = spl_get_zval_ptr(op2, Ts, &EG(free_op2) TSRMLS_CC);
		zval *exists;

		spl_begin_method_call_arg_ex1(obj, obj_ce, NULL, "exists", sizeof("exists")-1, &exists, dim TSRMLS_CC);
		if (!i_zend_is_true(exists)) {
			if (type == BP_VAR_R || type == BP_VAR_RW) {
				SEPARATE_ZVAL(&dim);
				convert_to_string_ex(&dim);
				zend_error(E_NOTICE, "Undefined index:  %s", Z_STRVAL_P(dim));
				DELETE_ZVAL(dim);
			}
			if (type == BP_VAR_R || type == BP_VAR_IS) {
				DELETE_RET_ZVAL(exists);
				*retval = &EG(error_zval);
				(*retval)->refcount++;
				FREE_OP(Ts, op2, EG(free_op2));
				SELECTIVE_PZVAL_LOCK(*retval, result);
				return 0;
			}
		}
		DELETE_RET_ZVAL(exists);
		if (type == BP_VAR_R || type == BP_VAR_IS) {
			spl_begin_method_call_arg_ex1(obj, obj_ce, NULL, "get", sizeof("get")-1, retval, dim TSRMLS_CC);
		}
		FREE_OP(Ts, op2, EG(free_op2));
		return 0;
	}
	return 1;
}
/* }}} */

/* {{{ ZEND_EXECUTE_HOOK_FUNCTION(ZEND_FETCH_DIM_R) */
#ifdef SPL_ARRAY_READ
ZEND_EXECUTE_HOOK_FUNCTION(ZEND_FETCH_DIM_R)
{
	if (!spl_fetch_dimension_address(&EX(opline)->result, &EX(opline)->op1, &EX(opline)->op2, EX(Ts), BP_VAR_R TSRMLS_CC))
	{
		if (EX(opline)->extended_value == ZEND_FETCH_ADD_LOCK) {
			PZVAL_LOCK(*EX_T(EX(opline)->op1.u.var).var.ptr_ptr);
		}
		spl_unlock_zval_ptr_ptr(&EX(opline)->op1, EX(Ts) TSRMLS_CC);

		AI_PTR_2_PTR_PTR(EX_T(EX(opline)->result.u.var).var);
		NEXT_OPCODE();
	}
	ZEND_EXECUTE_HOOK_ORIGINAL(ZEND_FETCH_DIM_R);
}
#endif
/* }}} */

/* {{{ ZEND_EXECUTE_HOOK_FUNCTION(ZEND_FETCH_DIM_W) */
#ifdef SPL_ARRAY_READ
ZEND_EXECUTE_HOOK_FUNCTION(ZEND_FETCH_DIM_W)
{
	if (!spl_fetch_dimension_address(&EX(opline)->result, &EX(opline)->op1, &EX(opline)->op2, EX(Ts), BP_VAR_W TSRMLS_CC))
	{
		spl_unlock_zval_ptr_ptr(&EX(opline)->op1, EX(Ts) TSRMLS_CC);

		NEXT_OPCODE();
	}
	ZEND_EXECUTE_HOOK_ORIGINAL(ZEND_FETCH_DIM_W);
}
#endif
/* }}} */

/* {{{ ZEND_EXECUTE_HOOK_FUNCTION(ZEND_FETCH_DIM_RW) */
#ifdef SPL_ARRAY_READ
ZEND_EXECUTE_HOOK_FUNCTION(ZEND_FETCH_DIM_RW)
{
	if (!spl_fetch_dimension_address(&EX(opline)->result, &EX(opline)->op1, &EX(opline)->op2, EX(Ts), BP_VAR_RW TSRMLS_CC))
	{
		spl_unlock_zval_ptr_ptr(&EX(opline)->op1, EX(Ts) TSRMLS_CC);

		NEXT_OPCODE();
	}
	ZEND_EXECUTE_HOOK_ORIGINAL(ZEND_FETCH_DIM_RW);
}
#endif
/* }}} */

static inline zval **spl_get_obj_zval_ptr_ptr(znode *op, temp_variable *Ts, int type TSRMLS_DC)
{
	if (op->op_type == IS_UNUSED) {
		if (EG(This)) {
			/* this should actually never be modified, _ptr_ptr is modified only when
			   the object is empty */
			return &EG(This);
		} else {
			zend_error(E_ERROR, "Using $this when not in object context");
		}
	}
	return spl_get_zval_ptr_ptr(op, Ts TSRMLS_CC);
}

/* {{{ ZEND_EXECUTE_HOOK_FUNCTION(ZEND_ASSIGN_DIM) */
#ifdef SPL_ARRAY_WRITE
ZEND_EXECUTE_HOOK_FUNCTION(ZEND_ASSIGN_DIM)
{
	zval **obj;
	zend_class_entry *obj_ce;
	spl_is_a is_a;

	obj = spl_get_obj_zval_ptr_ptr(&EX(opline)->op1, EX(Ts), 0 TSRMLS_CC);

	if (!obj || (obj_ce = spl_get_class_entry(*obj TSRMLS_CC)) == NULL) {
		ZEND_EXECUTE_HOOK_ORIGINAL(ZEND_ASSIGN_DIM);
	}

	is_a = spl_implements(obj_ce);

	if (is_a & SPL_IS_A_ARRAY_ACCESS) {
		znode *op2 = &EX(opline)->op2;
		zval *index = spl_get_zval_ptr(op2, EX(Ts), &EG(free_op2), BP_VAR_R);
		zval *free_value;
		zend_op *value_op = EX(opline)+1;
		zval *value = spl_get_zval_ptr(&value_op->op1, EX(Ts), &free_value, BP_VAR_R);
		zval tmp;
		zval *retval;

		spl_unlock_zval_ptr_ptr(&EX(opline)->op1, EX(Ts) TSRMLS_CC);

		/* here we are sure we are dealing with an object */
		switch (op2->op_type) {
			case IS_CONST:
				/* already a constant string */
				break;
			case IS_VAR:
				tmp = *index;
				zval_copy_ctor(&tmp);
				convert_to_string(&tmp);
				index = &tmp;
				break;
			case IS_TMP_VAR:
				convert_to_string(index);
				break;
		}

		/* separate our value if necessary */
		if (value_op->op1.op_type == IS_TMP_VAR) {
			zval *orig_value = value;
	
			ALLOC_ZVAL(value);
			*value = *orig_value;
			value->is_ref = 0;
			value->refcount = 0;
		}

		spl_begin_method_call_arg_ex2(obj, obj_ce, NULL, "set", sizeof("set")-1, &retval, index, value TSRMLS_CC);

		if (index == &tmp) {
			zval_dtor(index);
		}

		FREE_OP(Ts, op2, EG(free_op2));
		if (&EX(opline)->result) {
			EX_T(EX(opline)->result.u.var).var.ptr = retval;
			EX_T(EX(opline)->result.u.var).var.ptr_ptr = NULL;/*&EX_T(EX(opline)->result.u.var).var.ptr;*/
			SELECTIVE_PZVAL_LOCK(retval, &EX(opline)->result);
		}

		EX(opline)++;
		NEXT_OPCODE();
	}
	ZEND_EXECUTE_HOOK_ORIGINAL(ZEND_ASSIGN_DIM);
}
#endif
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */
