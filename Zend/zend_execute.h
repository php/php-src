/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2000 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 0.92 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        | 
   | available at through the world-wide-web at                           |
   | http://www.zend.com/license/0_92.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/


#ifndef ZEND_EXECUTE_H
#define ZEND_EXECUTE_H

#include "zend_compile.h"
#include "zend_hash.h"
#include "zend_variables.h"
#include "zend_execute_locks.h"

typedef union _temp_variable {
	zval tmp_var;
	struct {
		zval **ptr_ptr;
		zval *ptr;
	} var;
	struct {
		zval tmp_var; /* a dummy */

		union {
			struct {
				zval *str;
				int offset;
			} str_offset;
			zend_property_reference overloaded_element;
		} data;
			
		unsigned char type;
	} EA;
} temp_variable;


ZEND_API extern void (*zend_execute)(zend_op_array *op_array ELS_DC);

void init_executor(CLS_D ELS_DC);
void shutdown_executor(ELS_D);
void execute(zend_op_array *op_array ELS_DC);
ZEND_API int zend_is_true(zval *op);
static inline void safe_free_zval_ptr(zval *p)
{
	ELS_FETCH();

	if (p!=EG(uninitialized_zval_ptr)) {
		FREE_ZVAL(p);
	}
}

ZEND_API int zend_eval_string(char *str, zval *retval_ptr, char *string_name CLS_DC ELS_DC);
static inline int i_zend_is_true(zval *op)
{
	int result;

	switch (op->type) {
		case IS_NULL:
			result = 0;
			break;
		case IS_LONG:
		case IS_BOOL:
		case IS_RESOURCE:
			result = (op->value.lval?1:0);
			break;
		case IS_DOUBLE:
			result = (op->value.dval ? 1 : 0);
			break;
		case IS_STRING:
			if (op->value.str.len == 0
				|| (op->value.str.len==1 && op->value.str.val[0]=='0')) {
				result = 0;
			} else {
				result = 1;
			}
			break;
		case IS_ARRAY:
			result = (zend_hash_num_elements(op->value.ht)?1:0);
			break;
		case IS_OBJECT:
			result = (zend_hash_num_elements(op->value.obj.properties)?1:0);
			break;
		default:
			result = 0;
			break;
	}
	return result;
}

ZEND_API int zval_update_constant(zval **pp, void *arg);

/* dedicated Zend executor functions - do not use! */
static inline void zend_ptr_stack_clear_multiple(ELS_D)
{
	void **p = EG(argument_stack).top_element-2;
	int delete_count = (ulong) *p;

	EG(argument_stack).top -= (delete_count+2);
	while (--delete_count>=0) {
		zval_ptr_dtor((zval **) --p);
	}
	EG(argument_stack).top_element = p;
}

static inline int zend_ptr_stack_get_arg(int requested_arg, void **data ELS_DC)
{
	void **p = EG(argument_stack).top_element-2;
	int arg_count = (ulong) *p;

	if (requested_arg>arg_count) {
		return FAILURE;
	}
	*data = (p-arg_count+requested_arg-1);
	return SUCCESS;
}

#if SUPPORT_INTERACTIVE
void execute_new_code(CLS_D);
#endif


/* services */
ZEND_API char *get_active_function_name(void);
ZEND_API char *zend_get_executed_filename(ELS_D);
ZEND_API uint zend_get_executed_lineno(ELS_D);
ZEND_API zend_bool zend_is_executing(void);

ZEND_API void zend_set_timeout(long seconds);
ZEND_API void zend_unset_timeout(void);
ZEND_API void zend_timeout(int dummy);

#ifdef ZEND_WIN32
void zend_init_timeout_thread();
void zend_shutdown_timeout_thread();
#define WM_REGISTER_ZEND_TIMEOUT		(WM_USER+1)
#define WM_UNREGISTER_ZEND_TIMEOUT		(WM_USER+2)
#endif

#define zendi_zval_copy_ctor(p) zval_copy_ctor(&(p))
#define zendi_zval_dtor(p) zval_dtor(&(p))

#define active_opline (*EG(opline_ptr))

static inline void zend_assign_to_variable_reference(znode *result, zval **variable_ptr_ptr, zval **value_ptr_ptr, temp_variable *Ts ELS_DC)
{
	zval *variable_ptr;
	zval *value_ptr;

	if (!value_ptr_ptr || !variable_ptr_ptr) {
		zend_error(E_ERROR, "Cannot create references to/from string offsets nor overloaded objects");
		return;
	}

	variable_ptr = *variable_ptr_ptr;
	value_ptr = *value_ptr_ptr;

	if (variable_ptr == EG(error_zval_ptr) || value_ptr==EG(error_zval_ptr)) {
		variable_ptr_ptr = &EG(uninitialized_zval_ptr);
/*	} else if (variable_ptr==&EG(uninitialized_zval) || variable_ptr!=value_ptr) { */
	} else if (variable_ptr_ptr != value_ptr_ptr) {
		variable_ptr->refcount--;
		if (variable_ptr->refcount==0) {
			zendi_zval_dtor(*variable_ptr);
			FREE_ZVAL(variable_ptr);
		}

		if (!PZVAL_IS_REF(value_ptr)) {
			/* break it away */
			value_ptr->refcount--;
			if (value_ptr->refcount>0) {
				ALLOC_ZVAL(*value_ptr_ptr);
				**value_ptr_ptr = *value_ptr;
				value_ptr = *value_ptr_ptr;
				zendi_zval_copy_ctor(*value_ptr);
			}
			value_ptr->refcount = 1;
			value_ptr->is_ref = 1;
		}

		*variable_ptr_ptr = value_ptr;
		value_ptr->refcount++;
	} else {
		if (variable_ptr->refcount>1) { /* we need to break away */
			SEPARATE_ZVAL(variable_ptr_ptr);
		}
		(*variable_ptr_ptr)->is_ref = 1;
	}

	if (result && !(result->u.EA.type & EXT_TYPE_UNUSED)) {
		Ts[result->u.var].var.ptr_ptr = variable_ptr_ptr;
		SELECTIVE_PZVAL_LOCK(*variable_ptr_ptr, result);
		AI_USE_PTR(Ts[result->u.var].var);
	}
}

#define IS_OVERLOADED_OBJECT 1
#define IS_STRING_OFFSET 2

#endif /* ZEND_EXECUTE_H */
