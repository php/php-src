/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2004 Zend Technologies Ltd. (http://www.zend.com) |
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

#define ZEND_INTENSIVE_DEBUGGING 0

#include <stdio.h>
#include <signal.h>

#include "zend.h"
#include "zend_compile.h"
#include "zend_execute.h"
#include "zend_API.h"
#include "zend_ptr_stack.h"
#include "zend_constants.h"
#include "zend_extensions.h"
#include "zend_fast_cache.h"
#include "zend_ini.h"
#include "zend_exceptions.h"

#define get_zval_ptr(node, Ts, should_free, type) _get_zval_ptr(node, Ts, should_free TSRMLS_CC)
#define get_zval_ptr_ptr(node, Ts, type) _get_zval_ptr_ptr(node, Ts TSRMLS_CC)

/* Prototypes */
static void zend_fetch_var_address(zend_op *opline, temp_variable *Ts, int type TSRMLS_DC);
static void zend_fetch_dimension_address(znode *result, znode *op1, znode *op2, temp_variable *Ts, int type TSRMLS_DC);
static void zend_fetch_property_address(znode *result, znode *op1, znode *op2, temp_variable *Ts, int type TSRMLS_DC);
static void zend_fetch_dimension_address_from_tmp_var(znode *result, znode *op1, znode *op2, temp_variable *Ts TSRMLS_DC);
static void zend_extension_statement_handler(zend_extension *extension, zend_op_array *op_array TSRMLS_DC);
static void zend_extension_fcall_begin_handler(zend_extension *extension, zend_op_array *op_array TSRMLS_DC);
static void zend_extension_fcall_end_handler(zend_extension *extension, zend_op_array *op_array TSRMLS_DC);

#define RETURN_VALUE_USED(opline) (!((opline)->result.u.EA.type & EXT_TYPE_UNUSED))

#define EX_T(offset) (*(temp_variable *)((char *) EX(Ts) + offset))
#define T(offset) (*(temp_variable *)((char *) Ts + offset))

#define TEMP_VAR_STACK_LIMIT 2000

#define MAKE_REAL_ZVAL_PTR(val) \
	do { \
		zval *_tmp; \
		ALLOC_ZVAL(_tmp); \
		_tmp->value = (val)->value; \
		_tmp->type = (val)->type; \
		_tmp->refcount = 1; \
		_tmp->is_ref = 0; \
		val = _tmp; \
	} while (0)

/* former zend_execute_locks.h */
static inline void zend_pzval_lock_func(zval *z)
{
	z->refcount++;
}


static inline void zend_pzval_unlock_func(zval *z TSRMLS_DC)
{
	z->refcount--;
	if (!z->refcount) {
		z->refcount = 1;
		z->is_ref = 0;
		EG(garbage)[EG(garbage_ptr)++] = z;
	}
}

static inline void zend_clean_garbage(TSRMLS_D)
{
	while (EG(garbage_ptr)) {
		zval_ptr_dtor(&EG(garbage)[--EG(garbage_ptr)]);
	}
}

#define PZVAL_UNLOCK(z) zend_pzval_unlock_func(z TSRMLS_CC)
#define PZVAL_LOCK(z) zend_pzval_lock_func(z)
#define RETURN_VALUE_UNUSED(pzn)	(((pzn)->u.EA.type & EXT_TYPE_UNUSED))
#define SELECTIVE_PZVAL_LOCK(pzv, pzn)	if (!RETURN_VALUE_UNUSED(pzn)) { PZVAL_LOCK(pzv); }


/* End of zend_execute_locks.h */

static inline zval *_get_zval_ptr(znode *node, temp_variable *Ts, zval **should_free TSRMLS_DC)
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
				temp_variable *T = &T(node->u.var);
				zval *str = T->str_offset.str;

				/* string offset */
				*should_free = &T(node->u.var).tmp_var;

				if (T->str_offset.str->type != IS_STRING
					|| ((int)T->str_offset.offset<0)
					|| (T->str_offset.str->value.str.len <= T->str_offset.offset)) {
					zend_error(E_NOTICE, "Uninitialized string offset:  %d", T->str_offset.offset);
					T->tmp_var.value.str.val = empty_string;
					T->tmp_var.value.str.len = 0;
				} else {
					char c = str->value.str.val[T->str_offset.offset];

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
		case IS_UNUSED:
			*should_free = 0;
			return NULL;
			break;
		EMPTY_SWITCH_DEFAULT_CASE()
	}
	return NULL;
}

static inline zval **_get_zval_ptr_ptr(znode *node, temp_variable *Ts TSRMLS_DC)
{
	if (node->op_type==IS_VAR) {
		if (T(node->u.var).var.ptr_ptr) {
			PZVAL_UNLOCK(*T(node->u.var).var.ptr_ptr);
		} else {
			/* string offset */
			PZVAL_UNLOCK(T(node->u.var).str_offset.str);
		}
		return T(node->u.var).var.ptr_ptr;
	} else {
		return NULL;
	}
}

static inline void zend_fetch_property_address_inner(zval *object, znode *op2, znode *result, temp_variable *Ts, int type TSRMLS_DC)
{
	zval *prop_ptr = get_zval_ptr(op2, Ts, &EG(free_op2), BP_VAR_R);

	if (EG(free_op2)) {
		MAKE_REAL_ZVAL_PTR(prop_ptr);
	}

	if (Z_OBJ_HT_P(object)->get_property_ptr_ptr) {
		zval **ptr_ptr = Z_OBJ_HT_P(object)->get_property_ptr_ptr(object, prop_ptr TSRMLS_CC);
		if(NULL == ptr_ptr) {
			if (Z_OBJ_HT_P(object)->read_property &&
			    (T(result->u.var).var.ptr = Z_OBJ_HT_P(object)->read_property(object, prop_ptr, BP_VAR_W TSRMLS_CC)) != NULL) {
				T(result->u.var).var.ptr_ptr = &T(result->u.var).var.ptr;
			} else {
				zend_error(E_ERROR, "Cannot access undefined property for object with overloaded property access");
			}
		} else {
			T(result->u.var).var.ptr_ptr = ptr_ptr;
		}
	} else if (Z_OBJ_HT_P(object)->read_property) {
		T(result->u.var).var.ptr = Z_OBJ_HT_P(object)->read_property(object, prop_ptr, BP_VAR_W TSRMLS_CC);
		T(result->u.var).var.ptr_ptr = &T(result->u.var).var.ptr;
	} else {
		zend_error(E_WARNING, "This object doesn't support property references");
		T(result->u.var).var.ptr_ptr = &EG(error_zval_ptr);
	}
	
	if (EG(free_op2)) {
		zval_ptr_dtor(&prop_ptr);
	}
}



static inline void zend_switch_free(zend_op *opline, temp_variable *Ts TSRMLS_DC)
{
	switch (opline->op1.op_type) {
		case IS_VAR:
			if (!T(opline->op1.u.var).var.ptr_ptr) {
				temp_variable *T = &T(opline->op1.u.var);
				/* perform the equivalent of equivalent of a
				 * quick & silent get_zval_ptr, and FREE_OP
				 */
				PZVAL_UNLOCK(T->str_offset.str);
			} else {
				zval_ptr_dtor(&T(opline->op1.u.var).var.ptr);
				if (opline->extended_value) { /* foreach() free */
					zval_ptr_dtor(&T(opline->op1.u.var).var.ptr);
				}
			}
			break;
		case IS_TMP_VAR:
			zendi_zval_dtor(T(opline->op1.u.var).tmp_var);
			break;
		EMPTY_SWITCH_DEFAULT_CASE()
	}
}

void zend_assign_to_variable_reference(znode *result, zval **variable_ptr_ptr, zval **value_ptr_ptr, temp_variable *Ts TSRMLS_DC)
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
	} else if (variable_ptr != value_ptr) {
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
		
		variable_ptr->refcount--;
		if (variable_ptr->refcount==0) {
			zendi_zval_dtor(*variable_ptr);
			FREE_ZVAL(variable_ptr);
		}
	} else if (!variable_ptr->is_ref) {
		if (variable_ptr_ptr == value_ptr_ptr) {
			SEPARATE_ZVAL(variable_ptr_ptr);
		} else if (variable_ptr==EG(uninitialized_zval_ptr)
			|| variable_ptr->refcount>2) {
			/* we need to separate */
			variable_ptr->refcount -= 2;
			ALLOC_ZVAL(*variable_ptr_ptr);
			**variable_ptr_ptr = *variable_ptr;
			zval_copy_ctor(*variable_ptr_ptr);
			*value_ptr_ptr = *variable_ptr_ptr;
			(*variable_ptr_ptr)->refcount = 2;
		}
		(*variable_ptr_ptr)->is_ref = 1;
	}

	if (result && !(result->u.EA.type & EXT_TYPE_UNUSED)) {
		T(result->u.var).var.ptr_ptr = variable_ptr_ptr;
		SELECTIVE_PZVAL_LOCK(*variable_ptr_ptr, result);
		AI_USE_PTR(T(result->u.var).var);
	}
}

static inline void make_real_object(zval **object_ptr TSRMLS_DC)
{
/* this should modify object only if it's empty */
	if ((*object_ptr)->type == IS_NULL
		|| ((*object_ptr)->type == IS_BOOL && (*object_ptr)->value.lval==0)
		|| ((*object_ptr)->type == IS_STRING && (*object_ptr)->value.str.len == 0)) {
		if (!PZVAL_IS_REF(*object_ptr)) {
			SEPARATE_ZVAL(object_ptr);
		}
		zend_error(E_STRICT, "Creating default object from empty value");
		object_init(*object_ptr);
	}
}

static inline zval **get_obj_zval_ptr_ptr(znode *op, temp_variable *Ts, int type TSRMLS_DC)
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
	return get_zval_ptr_ptr(op, Ts, type);
}

static inline zval *get_obj_zval_ptr(znode *op, temp_variable *Ts, zval **freeop, int type TSRMLS_DC)
{
	if (op->op_type == IS_UNUSED) {
		if (EG(This)) {
			return EG(This);
		} else {
			zend_error(E_ERROR, "Using $this when not in object context");
		}
	}
	return get_zval_ptr(op, Ts, freeop, type);
}


static inline void zend_verify_arg_type(zend_function *zf, zend_uint arg_num, zval *arg TSRMLS_DC)
{
	zend_arg_info *cur_arg_info;

	if (!zf->common.arg_info
		|| arg_num>zf->common.num_args) {
		return;
	}

	cur_arg_info = &zf->common.arg_info[arg_num-1];

	if (cur_arg_info->class_name) {
		if (!arg) {
			zend_error(E_ERROR, "Argument %d must be an object of class %s", arg_num, cur_arg_info->class_name);
		}
		switch (Z_TYPE_P(arg)) {
			case IS_NULL:
				if (!cur_arg_info->allow_null) {
					zend_error(E_ERROR, "Argument %d must not be null", arg_num);
				}
				break;
			case IS_OBJECT: {
					zend_class_entry *ce = zend_fetch_class(cur_arg_info->class_name, cur_arg_info->class_name_len, ZEND_FETCH_CLASS_AUTO TSRMLS_CC);
					if (!instanceof_function(Z_OBJCE_P(arg), ce TSRMLS_CC)) {
						char *error_msg;

						if (ce->ce_flags & ZEND_ACC_INTERFACE) {
							error_msg = "implement interface";
						} else {
							error_msg = "be an instance of";
						}
						zend_error(E_ERROR, "Argument %d must %s %s", arg_num, error_msg, ce->name);
					}
				}
				break;
			default:
				zend_error(E_ERROR, "Argument %d must be an object of class %s", arg_num, cur_arg_info->class_name);
				break;
		}
	}	
}

static inline void zend_assign_to_object(znode *result, zval **object_ptr, znode *op2, znode *value_op, temp_variable *Ts, int opcode TSRMLS_DC)
{
	zval *object;
	zval *property_name = get_zval_ptr(op2, Ts, &EG(free_op2), BP_VAR_R);
	zval *free_value;
	zval *value = get_zval_ptr(value_op, Ts, &free_value, BP_VAR_R);
	zval **retval = &T(result->u.var).var.ptr;

	make_real_object(object_ptr TSRMLS_CC); /* this should modify object only if it's empty */
	object = *object_ptr;
	
	if (object->type != IS_OBJECT) {
		zend_error(E_WARNING, "Attempt to assign property of non-object");
		FREE_OP(Ts, op2, EG(free_op2));
		*retval = EG(uninitialized_zval_ptr);

		SELECTIVE_PZVAL_LOCK(*retval, result);
		FREE_OP(Ts, value_op, free_value);
		return;
	}
	
	/* here we are sure we are dealing with an object */

	/* separate our value if necessary */
	if (EG(ze1_compatibility_mode) && Z_TYPE_P(value) == IS_OBJECT) {
		zval *orig_value = value;
		char *class_name;
		zend_uint class_name_len;
		int dup;
	
		ALLOC_ZVAL(value);
		*value = *orig_value;
	 	value->is_ref = 0;
		value->refcount = 0;

		dup = zend_get_object_classname(orig_value, &class_name, &class_name_len TSRMLS_CC);

		if (Z_OBJ_HANDLER_P(value, clone_obj) == NULL) {
			zend_error(E_ERROR, "Trying to clone an uncloneable object of class %s",  class_name);
		}

		zend_error(E_STRICT, "Implicit cloning object of class '%s' because of 'zend.ze1_compatibility_mode'", class_name);
		
		if(!dup)	{
			efree(class_name);
		}
		
		value->value.obj = Z_OBJ_HANDLER_P(orig_value, clone_obj)(orig_value TSRMLS_CC);
	} else if (value_op->op_type == IS_TMP_VAR) {
		zval *orig_value = value;

		ALLOC_ZVAL(value);
		*value = *orig_value;
		value->is_ref = 0;
		value->refcount = 0;
	} else if (value_op->op_type == IS_CONST) {
		zval *orig_value = value;

		ALLOC_ZVAL(value);
		*value = *orig_value;
		value->is_ref = 0;
		value->refcount = 0;
		zval_copy_ctor(value);
	}
		

	value->refcount++;
	if (opcode == ZEND_ASSIGN_OBJ) {
		if (EG(free_op2)) {
			MAKE_REAL_ZVAL_PTR(property_name);
		}
		Z_OBJ_HT_P(object)->write_property(object, property_name, value TSRMLS_CC);
	} else {
		/* Note:  property_name in this case is really the array index! */
		if (!Z_OBJ_HT_P(object)->write_dimension) {
			zend_error(E_ERROR, "Cannot use object as array");
		}
		if (EG(free_op2)) {
			MAKE_REAL_ZVAL_PTR(property_name);
		}
		Z_OBJ_HT_P(object)->write_dimension(object, property_name, value TSRMLS_CC);
	}
	
	if (result) {
		T(result->u.var).var.ptr = value;
		T(result->u.var).var.ptr_ptr = &T(result->u.var).var.ptr; /* this is so that we could use it in FETCH_DIM_R, etc. - see bug #27876 */
		SELECTIVE_PZVAL_LOCK(value, result);
	}
	if (EG(free_op2)) {
		zval_ptr_dtor(&property_name);
	}
	zval_ptr_dtor(&value);
}


static inline void zend_assign_to_variable(znode *result, znode *op1, znode *op2, zval *value, int type, temp_variable *Ts TSRMLS_DC)
{
	zval **variable_ptr_ptr = get_zval_ptr_ptr(op1, Ts, BP_VAR_W);
	zval *variable_ptr;
	
	if (!variable_ptr_ptr) {
		temp_variable *T = &T(op1->u.var);

		if (T->str_offset.str->type == IS_STRING) do {
			zval tmp;
			zval *final_value = value;

			if (((int)T->str_offset.offset < 0)) {
				zend_error(E_WARNING, "Illegal string offset:  %d", T->str_offset.offset);
				break;
			}
			if (T->str_offset.offset >= T->str_offset.str->value.str.len) {
				zend_uint i;

				if (T->str_offset.str->value.str.len==0) {
					STR_FREE(T->str_offset.str->value.str.val);
					T->str_offset.str->value.str.val = (char *) emalloc(T->str_offset.offset+1+1);
				} else {
					T->str_offset.str->value.str.val = (char *) erealloc(T->str_offset.str->value.str.val, T->str_offset.offset+1+1);
				}
				for (i=T->str_offset.str->value.str.len; i<T->str_offset.offset; i++) {
					T->str_offset.str->value.str.val[i] = ' ';
				}
				T->str_offset.str->value.str.val[T->str_offset.offset+1] = 0;
				T->str_offset.str->value.str.len = T->str_offset.offset+1;
			}

			if (value->type!=IS_STRING) {
				tmp = *value;
				if (op2 && op2->op_type == IS_VAR) {
					zval_copy_ctor(&tmp);
				}
				convert_to_string(&tmp);
				final_value = &tmp;
			}

			T->str_offset.str->value.str.val[T->str_offset.offset] = final_value->value.str.val[0];
			
			if (op2) {
				if (op2->op_type == IS_VAR) {
					if (value == &T(op2->u.var).tmp_var) {
						if (result->u.EA.type & EXT_TYPE_UNUSED) {
							/* We are not going to use return value, drop it */
							STR_FREE(value->value.str.val);
						} else {
							/* We are going to use return value, make it real zval */
							ALLOC_ZVAL(value);
							*value = T(op2->u.var).tmp_var;
							value->is_ref = 0;
							value->refcount = 0; /* LOCK will increase it */
						}
					}
				} else {
					if (final_value == &T(op2->u.var).tmp_var) {
						/* we can safely free final_value here
						 * because separation is done only
						 * in case op2->op_type == IS_VAR */
						STR_FREE(final_value->value.str.val);
					}
				}
			}
			if (final_value == &tmp) {
				zval_dtor(final_value);
			}
			/*
			 * the value of an assignment to a string offset is undefined
			T(result->u.var).var = &T->str_offset.str;
			*/
		} while (0);
		/* zval_ptr_dtor(&T->str_offset.str); Nuke this line if it doesn't cause a leak */
		T->tmp_var.type = IS_STRING;
		
/*		T(result->u.var).var.ptr_ptr = &EG(uninitialized_zval_ptr); */
		T(result->u.var).var.ptr_ptr = &value;
		SELECTIVE_PZVAL_LOCK(*T(result->u.var).var.ptr_ptr, result);
		AI_USE_PTR(T(result->u.var).var);
		return;
	}

	variable_ptr = *variable_ptr_ptr;

	if (variable_ptr == EG(error_zval_ptr)) {
		if (result) {
			T(result->u.var).var.ptr_ptr = &EG(uninitialized_zval_ptr);
			SELECTIVE_PZVAL_LOCK(*T(result->u.var).var.ptr_ptr, result);
			AI_USE_PTR(T(result->u.var).var);
		}
		if (type==IS_TMP_VAR) {
			zval_dtor(value);
		}
		return;
	}

	if(Z_TYPE_P(variable_ptr) == IS_OBJECT && Z_OBJ_HANDLER_P(variable_ptr, set)) {
		/* TODO? ze1_compatibility_mode support */
		Z_OBJ_HANDLER_P(variable_ptr, set)(variable_ptr_ptr, value TSRMLS_CC);
		goto done_setting_var;
	}
	
	if (EG(ze1_compatibility_mode) && Z_TYPE_P(value) == IS_OBJECT) {
		char *class_name;
		zend_uint class_name_len;
		int dup;

		dup = zend_get_object_classname(value, &class_name, &class_name_len TSRMLS_CC);
 
		if (Z_OBJ_HANDLER_P(value, clone_obj) == NULL) {
			zend_error(E_ERROR, "Trying to clone an uncloneable object of class %s",  class_name);
		} else if (PZVAL_IS_REF(variable_ptr)) {
			if (variable_ptr != value) {
				zend_uint refcount = variable_ptr->refcount;
				zval garbage;
				
				if (type != IS_TMP_VAR) {
					value->refcount++;
				}
			
				garbage = *variable_ptr;
				*variable_ptr = *value;
				variable_ptr->refcount = refcount;
				variable_ptr->is_ref = 1;
				
				zend_error(E_STRICT, "Implicit cloning object of class '%s' because of 'zend.ze1_compatibility_mode'", class_name);

				variable_ptr->value.obj = Z_OBJ_HANDLER_P(value, clone_obj)(value TSRMLS_CC);

				if (type != IS_TMP_VAR) {
					value->refcount--;
				}

				zendi_zval_dtor(garbage);
			}
		} else {
			if (variable_ptr != value) {
				value->refcount++;
				variable_ptr->refcount--;
				if (variable_ptr->refcount == 0) {
					zendi_zval_dtor(*variable_ptr);
				} else {
					ALLOC_ZVAL(variable_ptr);
					*variable_ptr_ptr = variable_ptr;
				}
				*variable_ptr = *value;
				INIT_PZVAL(variable_ptr);
				zend_error(E_STRICT, "Implicit cloning object of class '%s' because of 'zend.ze1_compatibility_mode'", class_name);
				variable_ptr->value.obj = Z_OBJ_HANDLER_P(value, clone_obj)(value TSRMLS_CC);
				zval_ptr_dtor(&value);
			}
		}

		if(!dup) {
			efree(class_name);
		}

	} else if (PZVAL_IS_REF(variable_ptr)) {
		if (variable_ptr!=value) {
			zend_uint refcount = variable_ptr->refcount;
			zval garbage;
			
			if (type!=IS_TMP_VAR) {
				value->refcount++;
			}
			garbage = *variable_ptr;
			*variable_ptr = *value;
			variable_ptr->refcount = refcount;
			variable_ptr->is_ref = 1;
			if (type!=IS_TMP_VAR) {
				zendi_zval_copy_ctor(*variable_ptr);
				value->refcount--;
			}
			zendi_zval_dtor(garbage);
		}
	} else {
		variable_ptr->refcount--;
		if (variable_ptr->refcount==0) {
			switch (type) {
				case IS_VAR:
					/* break missing intentionally */
				case IS_CONST:
					if (variable_ptr==value) {
						variable_ptr->refcount++;
					} else if (PZVAL_IS_REF(value)) {
						zval tmp;
						
						tmp = *value;
						zval_copy_ctor(&tmp);
						tmp.refcount=1;
						zendi_zval_dtor(*variable_ptr);
						*variable_ptr = tmp;
					} else {
						value->refcount++;
						zendi_zval_dtor(*variable_ptr);
						safe_free_zval_ptr(variable_ptr);
						*variable_ptr_ptr = value;
					}
					break;
				case IS_TMP_VAR:
					zendi_zval_dtor(*variable_ptr);
					value->refcount=1;
					*variable_ptr = *value;
					break;
					EMPTY_SWITCH_DEFAULT_CASE()
						}
		} else { /* we need to split */
			switch (type) {
				case IS_VAR:
					/* break missing intentionally */
				case IS_CONST:
					if (PZVAL_IS_REF(value) && value->refcount > 0) {
						ALLOC_ZVAL(variable_ptr);
						*variable_ptr_ptr = variable_ptr;
						*variable_ptr = *value;
						zval_copy_ctor(variable_ptr);
						variable_ptr->refcount=1;
						break;
					}
					*variable_ptr_ptr = value;
					value->refcount++;
					break;
				case IS_TMP_VAR:
					ALLOC_ZVAL(*variable_ptr_ptr);
					value->refcount=1;
					**variable_ptr_ptr = *value;
					break;
					EMPTY_SWITCH_DEFAULT_CASE()
						}
		}
		(*variable_ptr_ptr)->is_ref=0;
	}
	
done_setting_var:
	if (result) {
		T(result->u.var).var.ptr_ptr = variable_ptr_ptr;
		SELECTIVE_PZVAL_LOCK(*variable_ptr_ptr, result);
		AI_USE_PTR(T(result->u.var).var);
	} 
}


/* Utility Functions for Extensions */
static void zend_extension_statement_handler(zend_extension *extension, zend_op_array *op_array TSRMLS_DC)
{
	if (extension->statement_handler) {
		extension->statement_handler(op_array);
	}
}


static void zend_extension_fcall_begin_handler(zend_extension *extension, zend_op_array *op_array TSRMLS_DC)
{
	if (extension->fcall_begin_handler) {
		extension->fcall_begin_handler(op_array);
	}
}


static void zend_extension_fcall_end_handler(zend_extension *extension, zend_op_array *op_array TSRMLS_DC)
{
	if (extension->fcall_end_handler) {
		extension->fcall_end_handler(op_array);
	}
}


static void print_refcount(zval *p, char *str)
{
	print_refcount(NULL, NULL);
}


static inline HashTable *zend_get_target_symbol_table(zend_op *opline, temp_variable *Ts, int type, zval *variable TSRMLS_DC)
{
	switch (opline->op2.u.EA.type) {
		case ZEND_FETCH_LOCAL:
			return EG(active_symbol_table);
			break;
		case ZEND_FETCH_GLOBAL:
			return &EG(symbol_table);
			break;
		case ZEND_FETCH_STATIC:
			if (!EG(active_op_array)->static_variables) {
				ALLOC_HASHTABLE(EG(active_op_array)->static_variables);
				zend_hash_init(EG(active_op_array)->static_variables, 2, NULL, ZVAL_PTR_DTOR, 0);
			}
			return EG(active_op_array)->static_variables;
			break;
		EMPTY_SWITCH_DEFAULT_CASE()
	}
	return NULL;
}


static void zend_fetch_var_address(zend_op *opline, temp_variable *Ts, int type TSRMLS_DC)
{
	zval *free_op1;
	zval *varname = get_zval_ptr(&opline->op1, Ts, &free_op1, BP_VAR_R);
	zval **retval;
	zval tmp_varname;
	HashTable *target_symbol_table;
	zend_bool free_tmp = 0;

 	if (varname->type != IS_STRING) {
		tmp_varname = *varname;
		zval_copy_ctor(&tmp_varname);
		convert_to_string(&tmp_varname);
		varname = &tmp_varname;
		free_tmp = 1;
	}

	if (opline->op2.u.EA.type == ZEND_FETCH_STATIC_MEMBER) {
		target_symbol_table = NULL;
		retval = zend_std_get_static_property(T(opline->op2.u.var).class_entry, Z_STRVAL_P(varname), Z_STRLEN_P(varname), 0 TSRMLS_CC);
	} else {
		if (opline->op2.u.EA.type == ZEND_FETCH_GLOBAL && opline->op1.op_type == IS_VAR) {
			varname->refcount++;
		}
		target_symbol_table = zend_get_target_symbol_table(opline, Ts, type, varname TSRMLS_CC);
		if (!target_symbol_table) {
			return;
		}
		if (zend_hash_find(target_symbol_table, varname->value.str.val, varname->value.str.len+1, (void **) &retval) == FAILURE) {
			switch (type) {
				case BP_VAR_R: 
					zend_error(E_NOTICE,"Undefined variable: %s", varname->value.str.val);
					/* break missing intentionally */
				case BP_VAR_IS:
					retval = &EG(uninitialized_zval_ptr);
					break;
				case BP_VAR_RW:
					zend_error(E_NOTICE,"Undefined variable: %s", varname->value.str.val);
					/* break missing intentionally */
				case BP_VAR_W: {					
						zval *new_zval = &EG(uninitialized_zval);

						new_zval->refcount++;
						zend_hash_update(target_symbol_table, varname->value.str.val, varname->value.str.len+1, &new_zval, sizeof(zval *), (void **) &retval);
					}
					break;
				EMPTY_SWITCH_DEFAULT_CASE()
			}
		}
		switch (opline->op2.u.EA.type) {
			case ZEND_FETCH_LOCAL:
				FREE_OP(Ts, &opline->op1, free_op1);
				break;
			case ZEND_FETCH_STATIC:
				zval_update_constant(retval, (void*) 1 TSRMLS_CC);
				break;
		}
	}


	if (free_tmp) {
		zval_dtor(varname);
	}
	T(opline->result.u.var).var.ptr_ptr = retval;
	SELECTIVE_PZVAL_LOCK(*retval, &opline->result);
}


static inline zval **zend_fetch_dimension_address_inner(HashTable *ht, znode *op2, temp_variable *Ts, int type TSRMLS_DC)
{
	zval *dim = get_zval_ptr(op2, Ts, &EG(free_op2), BP_VAR_R);
	zval **retval;
	char *offset_key;
	int offset_key_length;

	switch (dim->type) {
		case IS_NULL:
			offset_key = "";
			offset_key_length = 0;
			goto fetch_string_dim;
		case IS_STRING:
			
			offset_key = dim->value.str.val;
			offset_key_length = dim->value.str.len;
			
fetch_string_dim:
			if (zend_symtable_find(ht, offset_key, offset_key_length+1, (void **) &retval) == FAILURE) {
				switch (type) {
					case BP_VAR_R:
						zend_error(E_NOTICE, "Undefined index:  %s", offset_key);
						/* break missing intentionally */
					case BP_VAR_UNSET:
					case BP_VAR_IS:
						retval = &EG(uninitialized_zval_ptr);
						break;
					case BP_VAR_RW:
						zend_error(E_NOTICE,"Undefined index:  %s", offset_key);
						/* break missing intentionally */
					case BP_VAR_W: {
							zval *new_zval = &EG(uninitialized_zval);

							new_zval->refcount++;
							zend_symtable_update(ht, offset_key, offset_key_length+1, &new_zval, sizeof(zval *), (void **) &retval);
						}
						break;
				}
			}
			break;
		case IS_RESOURCE:
			zend_error(E_STRICT, "Resource ID#%ld used as offset, casting to integer (%ld)", dim->value.lval, dim->value.lval);
			/* Fall Through */
		case IS_DOUBLE:
		case IS_BOOL: 
		case IS_LONG: {
				long index;

				if (dim->type == IS_DOUBLE) {
					index = (long)dim->value.dval;
				} else {
					index = dim->value.lval;
				}
				if (zend_hash_index_find(ht, index, (void **) &retval) == FAILURE) {
					switch (type) {
						case BP_VAR_R:
							zend_error(E_NOTICE,"Undefined offset:  %ld", index);
							/* break missing intentionally */
						case BP_VAR_UNSET:
						case BP_VAR_IS:
							retval = &EG(uninitialized_zval_ptr);
							break;
						case BP_VAR_RW:
							zend_error(E_NOTICE,"Undefined offset:  %ld", index);
							/* break missing intentionally */
						case BP_VAR_W: {
							zval *new_zval = &EG(uninitialized_zval);

							new_zval->refcount++;
							zend_hash_index_update(ht, index, &new_zval, sizeof(zval *), (void **) &retval);
						}
						break;
					}
				}
			}
			break;
		default: 
			zend_error(E_WARNING, "Illegal offset type");
			switch (type) {
				case BP_VAR_R:
				case BP_VAR_IS:
				case BP_VAR_UNSET:
					retval = &EG(uninitialized_zval_ptr);
					break;
				default:
					retval = &EG(error_zval_ptr);
					break;
			}
			break;
	}
	FREE_OP(Ts, op2, EG(free_op2));
	return retval;
}

static void zend_fetch_dimension_address(znode *result, znode *op1, znode *op2, temp_variable *Ts, int type TSRMLS_DC)
{
	zval **container_ptr = get_zval_ptr_ptr(op1, Ts, type);
	zval *container;
	zval ***retval = &T(result->u.var).var.ptr_ptr;

	if (!container_ptr) {
		zend_error(E_ERROR, "Cannot use string offset as an array");
	}
	
	container = *container_ptr;

	if (container == EG(error_zval_ptr)) {
		*retval = &EG(error_zval_ptr);
		SELECTIVE_PZVAL_LOCK(**retval, result);
		return;
	}

	if (container->type==IS_NULL
		|| (container->type==IS_BOOL && container->value.lval==0)
		|| (container->type==IS_STRING && container->value.str.len==0)) {
		switch (type) {
			case BP_VAR_RW:
			case BP_VAR_W:
				if (!PZVAL_IS_REF(container)) {
					SEPARATE_ZVAL(container_ptr);
					container = *container_ptr;
				}
				zval_dtor(container);
				array_init(container);
				break;
		}
	}

	switch (container->type) {
		case IS_ARRAY:
			if ((type==BP_VAR_W || type==BP_VAR_RW) && container->refcount>1 && !PZVAL_IS_REF(container)) {
				SEPARATE_ZVAL(container_ptr);
				container = *container_ptr;
			}
			if (op2->op_type == IS_UNUSED) {
				zval *new_zval = &EG(uninitialized_zval);

				new_zval->refcount++;
				if (zend_hash_next_index_insert(container->value.ht, &new_zval, sizeof(zval *), (void **) retval) == FAILURE) {
					zend_error(E_WARNING, "Cannot add element to the array as the next element is already occupied");
					*retval = &EG(error_zval_ptr);
					new_zval->refcount--; 
				}
			} else {
				*retval = zend_fetch_dimension_address_inner(container->value.ht, op2, Ts, type TSRMLS_CC);
			}
			SELECTIVE_PZVAL_LOCK(**retval, result);
			break;
		case IS_NULL:
			/* for read-mode only */
			get_zval_ptr(op2, Ts, &EG(free_op2), BP_VAR_R);
			*retval = &EG(uninitialized_zval_ptr);
			SELECTIVE_PZVAL_LOCK(**retval, result);
			FREE_OP(Ts, op2, EG(free_op2));
			if (type==BP_VAR_W || type==BP_VAR_RW) {
				zend_error(E_WARNING, "Cannot use a NULL value as an array");
			}
			break;
		case IS_STRING: {
				zval *offset;
				zval tmp;

				if (op2->op_type==IS_UNUSED) {
					zend_error(E_ERROR, "[] operator not supported for strings");
				}

				offset = get_zval_ptr(op2, Ts, &EG(free_op2), BP_VAR_R);

				if (offset->type != IS_LONG) {
					tmp = *offset;
					zval_copy_ctor(&tmp);
					convert_to_long(&tmp);
					offset = &tmp;
				}
				switch (type) {
					case BP_VAR_R:
					case BP_VAR_IS:
					case BP_VAR_UNSET:
						/* do nothing... */
						break;
					default:
						SEPARATE_ZVAL_IF_NOT_REF(container_ptr);
						break;
				}
				container = *container_ptr;
				T(result->u.var).str_offset.str = container;
				PZVAL_LOCK(container);
				T(result->u.var).str_offset.offset = offset->value.lval;
				FREE_OP(Ts, op2, EG(free_op2));
				*retval = NULL;
				return;
			}
			break;
		case IS_OBJECT:
			if (!Z_OBJ_HT_P(container)->read_dimension) {
				zend_error(E_ERROR, "Cannot use object as array");
			} else {
				zval *dim = get_zval_ptr(op2, Ts, &EG(free_op2), BP_VAR_R);
				zval *overloaded_result;

				if (EG(free_op2)) {
					MAKE_REAL_ZVAL_PTR(dim);
				}
				overloaded_result = Z_OBJ_HT_P(container)->read_dimension(container, dim, type TSRMLS_CC);
				if (overloaded_result) {
					switch (type) {
						case BP_VAR_RW:
						case BP_VAR_W:
							if (overloaded_result->type != IS_OBJECT
								&& !overloaded_result->is_ref) {
								zend_error(E_ERROR, "Objects used as arrays in post/pre increment/decrement must return values by reference");
							}
							break;
					}

					*retval = &overloaded_result;
				} else {
					*retval = &EG(error_zval_ptr);
				}
				AI_USE_PTR(T(result->u.var).var);
				SELECTIVE_PZVAL_LOCK(**retval, result);
				if (EG(free_op2)) {
					zval_ptr_dtor(&dim);
				}
			}
			break;
		default: {
				get_zval_ptr(op2, Ts, &EG(free_op2), BP_VAR_R);

				switch (type) {
					case BP_VAR_UNSET:
						zend_error(E_WARNING, "Cannot unset offset in a non-array variable");
						/* break missing intentionally */
					case BP_VAR_R:
					case BP_VAR_IS:
						*retval = &EG(uninitialized_zval_ptr);
						break;
					default:
						*retval = &EG(error_zval_ptr);
						break;
				}
				FREE_OP(Ts, op2, EG(free_op2));
				SELECTIVE_PZVAL_LOCK(**retval, result);
				if (type==BP_VAR_W || type==BP_VAR_RW) {
					zend_error(E_WARNING, "Cannot use a scalar value as an array");
				}
			}
			break;
	}
}


static void zend_fetch_dimension_address_from_tmp_var(znode *result, znode *op1, znode *op2, temp_variable *Ts TSRMLS_DC)
{
	zval *free_op1;
	zval *container = get_zval_ptr(op1, Ts, &free_op1, BP_VAR_R);

	if (container->type != IS_ARRAY) {
		T(result->u.var).var.ptr_ptr = &EG(uninitialized_zval_ptr);
		SELECTIVE_PZVAL_LOCK(*T(result->u.var).var.ptr_ptr, result);
		return;
	}

	T(result->u.var).var.ptr_ptr = zend_fetch_dimension_address_inner(container->value.ht, op2, Ts, BP_VAR_R TSRMLS_CC);
	SELECTIVE_PZVAL_LOCK(*T(result->u.var).var.ptr_ptr, result);
}

static void zend_fetch_property_address(znode *result, znode *op1, znode *op2, temp_variable *Ts, int type TSRMLS_DC)
{
	zval **container_ptr = get_obj_zval_ptr_ptr(op1, Ts, type TSRMLS_CC);
	zval *container;
	zval ***retval = &T(result->u.var).var.ptr_ptr;
	
	container = *container_ptr;
	if (container == EG(error_zval_ptr)) {
		*retval = &EG(error_zval_ptr);
		SELECTIVE_PZVAL_LOCK(**retval, result);
		return;
	}
	/* this should modify object only if it's empty */
	if (container->type == IS_NULL
		|| (container->type == IS_BOOL && container->value.lval==0)
		|| (container->type == IS_STRING && container->value.str.len == 0)) {
		switch (type) {
			case BP_VAR_RW:
			case BP_VAR_W:
				if (!PZVAL_IS_REF(container)) {
					SEPARATE_ZVAL(container_ptr);
					container = *container_ptr;
				}
				object_init(container);
				break;
		}
	}
	
	if (container->type != IS_OBJECT) {
		get_zval_ptr(op2, Ts, &EG(free_op2), BP_VAR_R);
		FREE_OP(Ts, op2, EG(free_op2));
		if (type == BP_VAR_R || type == BP_VAR_IS) {
			*retval = &EG(uninitialized_zval_ptr);
		} else {
			*retval = &EG(error_zval_ptr);
		}
		SELECTIVE_PZVAL_LOCK(**retval, result);
		return;
	}
	
	
	if ((type==BP_VAR_W || type==BP_VAR_RW) && container->refcount>1 && !PZVAL_IS_REF(container)) {
		SEPARATE_ZVAL(container_ptr);
		container = *container_ptr;
	}
	zend_fetch_property_address_inner(container, op2, result, Ts, type TSRMLS_CC);
	SELECTIVE_PZVAL_LOCK(**retval, result);
}

static void zend_fetch_property_address_read(znode *result, znode *op1, znode *op2, temp_variable *Ts, int type TSRMLS_DC)
{
	zval *container;
	zval **retval;
	
	retval = &T(result->u.var).var.ptr;
	T(result->u.var).var.ptr_ptr = retval;

	container = get_obj_zval_ptr(op1, Ts, &EG(free_op1), type TSRMLS_CC);

	if (container == EG(error_zval_ptr)) {
		*retval = EG(error_zval_ptr);
		SELECTIVE_PZVAL_LOCK(*retval, result);
		return;
	}


	if (container->type != IS_OBJECT) {
		zend_error(E_NOTICE, "Trying to get property of non-object");

		if (type==BP_VAR_R || type==BP_VAR_IS) {
			*retval = EG(uninitialized_zval_ptr);
		} else {
			*retval = EG(error_zval_ptr);
		}
		SELECTIVE_PZVAL_LOCK(*retval, result);
	} else {
		zval *offset;

		offset = get_zval_ptr(op2, Ts, &EG(free_op2), BP_VAR_R);
		if (EG(free_op2)) {
			MAKE_REAL_ZVAL_PTR(offset);
		}

		/* here we are sure we are dealing with an object */
		*retval = Z_OBJ_HT_P(container)->read_property(container, offset, type TSRMLS_CC);

		if (RETURN_VALUE_UNUSED(result) && ((*retval)->refcount == 0)) {
			zval_dtor(*retval);
			FREE_ZVAL(*retval);
		} else {
			SELECTIVE_PZVAL_LOCK(*retval, result);
		}

		if (EG(free_op2)) {
			zval_ptr_dtor(&offset);
		}
	}
}

static void zend_pre_incdec_property(znode *result, znode *op1, znode *op2, temp_variable * Ts, int (*incdec_op)(zval *) TSRMLS_DC)
{
	zval **object_ptr = get_obj_zval_ptr_ptr(op1, Ts, BP_VAR_W TSRMLS_CC);
	zval *object;
	zval *property = get_zval_ptr(op2, Ts, &EG(free_op2), BP_VAR_R);
	zval **retval = &T(result->u.var).var.ptr;
	int have_get_ptr = 0;

	make_real_object(object_ptr TSRMLS_CC); /* this should modify object only if it's empty */
	object = *object_ptr;
	
	if (object->type != IS_OBJECT) {
		zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");
		FREE_OP(Ts, op2, EG(free_op2));
		*retval = EG(uninitialized_zval_ptr);

		SELECTIVE_PZVAL_LOCK(*retval, result);
		return;
	}
	
	if (EG(free_op2)) {
		MAKE_REAL_ZVAL_PTR(property);
	}

	/* here we are sure we are dealing with an object */

	if (Z_OBJ_HT_P(object)->get_property_ptr_ptr) {
		zval **zptr = Z_OBJ_HT_P(object)->get_property_ptr_ptr(object, property TSRMLS_CC);
		if (zptr != NULL) { 			/* NULL means no success in getting PTR */
			SEPARATE_ZVAL_IF_NOT_REF(zptr);

			have_get_ptr = 1;
			incdec_op(*zptr);
			*retval = *zptr;
			SELECTIVE_PZVAL_LOCK(*retval, result);
		}
	}

	if (!have_get_ptr) {
		zval *z = Z_OBJ_HT_P(object)->read_property(object, property, BP_VAR_RW TSRMLS_CC);

		if (z->type == IS_OBJECT && Z_OBJ_HT_P(z)->get) {
			zval *value = Z_OBJ_HT_P(z)->get(z TSRMLS_CC);

			if (z->refcount == 0) {
				zval_dtor(z);
				FREE_ZVAL(z);
			}
			z = value;
		}
		z->refcount++;
		SEPARATE_ZVAL_IF_NOT_REF(&z);
		incdec_op(z);
		*retval = z;
		Z_OBJ_HT_P(object)->write_property(object, property, z TSRMLS_CC);
		SELECTIVE_PZVAL_LOCK(*retval, result);
		zval_ptr_dtor(&z);
	}
	
	if (EG(free_op2)) {
		zval_ptr_dtor(&property);
	}
}

static void zend_post_incdec_property(znode *result, znode *op1, znode *op2, temp_variable * Ts, int (*incdec_op)(zval *) TSRMLS_DC)
{
	zval **object_ptr = get_obj_zval_ptr_ptr(op1, Ts, BP_VAR_W TSRMLS_CC);
	zval *object;
	zval *property = get_zval_ptr(op2, Ts, &EG(free_op2), BP_VAR_R);
	zval *retval = &T(result->u.var).tmp_var;
	int have_get_ptr = 0;

	make_real_object(object_ptr TSRMLS_CC); /* this should modify object only if it's empty */
	object = *object_ptr;
	
	if (object->type != IS_OBJECT) {
		zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");
		FREE_OP(Ts, op2, EG(free_op2));
		*retval = *EG(uninitialized_zval_ptr);
		return;
	}
	
	if (EG(free_op2)) {
		MAKE_REAL_ZVAL_PTR(property);
	}

	/* here we are sure we are dealing with an object */

	if (Z_OBJ_HT_P(object)->get_property_ptr_ptr) {
		zval **zptr = Z_OBJ_HT_P(object)->get_property_ptr_ptr(object, property TSRMLS_CC);
		if (zptr != NULL) { 			/* NULL means no success in getting PTR */
			have_get_ptr = 1;
			SEPARATE_ZVAL_IF_NOT_REF(zptr);
			
			*retval = **zptr;
			zendi_zval_copy_ctor(*retval);
			
			incdec_op(*zptr);

		}
	}

	if (!have_get_ptr) {
		zval *z = Z_OBJ_HT_P(object)->read_property(object, property, BP_VAR_RW TSRMLS_CC);
		zval *z_copy;

		if (z->type == IS_OBJECT && Z_OBJ_HT_P(z)->get) {
			zval *value = Z_OBJ_HT_P(z)->get(z TSRMLS_CC);

			if (z->refcount == 0) {
				zval_dtor(z);
				FREE_ZVAL(z);
			}
			z = value;
		}
		*retval = *z;
		zendi_zval_copy_ctor(*retval);
		ALLOC_ZVAL(z_copy);
		*z_copy = *z;
		zendi_zval_copy_ctor(*z_copy);
		INIT_PZVAL(z_copy);
		incdec_op(z_copy);
		z->refcount++;
		Z_OBJ_HT_P(object)->write_property(object, property, z_copy TSRMLS_CC);
		zval_ptr_dtor(&z_copy);
		/* this would destroy z if it was returned with refcount == 0 and undo
		   recount++ above otherwise */
		zval_ptr_dtor(&z);
	}
	
	if (EG(free_op2)) {
		zval_ptr_dtor(&property);
	}
}


#if ZEND_INTENSIVE_DEBUGGING

#define CHECK_SYMBOL_TABLES()														\
	zend_hash_apply(&EG(symbol_table), (apply_func_t) zend_check_symbol TSRMLS_CC);	\
	if (&EG(symbol_table)!=EG(active_symbol_table)) {								\
		zend_hash_apply(EG(active_symbol_table), (apply_func_t) zend_check_symbol TSRMLS_CC);	\
	}

static int zend_check_symbol(zval **pz TSRMLS_DC)
{
	if (Z_TYPE_PP(pz) > 9) {
		fprintf(stderr, "Warning!  %x has invalid type!\n", *pz);
	} else if (Z_TYPE_PP(pz) == IS_ARRAY) {
		zend_hash_apply(Z_ARRVAL_PP(pz), (apply_func_t) zend_check_symbol TSRMLS_CC);
	} else if (Z_TYPE_PP(pz) == IS_OBJECT) {
		
		/* OBJ-TBI - doesn't support new object model! */
		zend_hash_apply(Z_OBJPROP_PP(pz), (apply_func_t) zend_check_symbol TSRMLS_CC);
	}

	return 0;
}


#else
#define CHECK_SYMBOL_TABLES()
#endif

#define NEXT_OPCODE()		\
	CHECK_SYMBOL_TABLES()	\
	EX(opline)++;			\
	return 0; /* CHECK_ME */

#define SET_OPCODE(new_op)	\
	CHECK_SYMBOL_TABLES()	\
	EX(opline) = new_op; \

#define INC_OPCODE()			\
	if (!EG(exception)) {		\
		CHECK_SYMBOL_TABLES()	\
		EX(opline)++;			\
	}

#define RETURN_FROM_EXECUTE_LOOP(execute_data)								\
	if (EX(op_array)->T < TEMP_VAR_STACK_LIMIT) {						\
 		free_alloca(EX(Ts));													\
 	} else {								\
 		efree(EX(Ts));							\
 	}									\
	EG(in_execution) = EX(original_in_execution);							\
	EG(current_execute_data) = EX(prev_execute_data);						\
	return 1; /* CHECK_ME */

ZEND_API opcode_handler_t zend_opcode_handlers[512];

ZEND_API void execute_internal(zend_execute_data *execute_data_ptr, int return_value_used TSRMLS_DC)
{
	((zend_internal_function *) execute_data_ptr->function_state.function)->handler(execute_data_ptr->opline->extended_value, (*(temp_variable *)((char *) execute_data_ptr->Ts + execute_data_ptr->opline->result.u.var)).var.ptr, execute_data_ptr->object, return_value_used TSRMLS_CC);
}

ZEND_API void execute(zend_op_array *op_array TSRMLS_DC)
{
	zend_execute_data execute_data;

	/* Initialize execute_data */
	EX(fbc) = NULL;
	EX(object) = NULL;
 	if (op_array->T < TEMP_VAR_STACK_LIMIT) {
 		EX(Ts) = (temp_variable *) do_alloca(sizeof(temp_variable) * op_array->T);
 	} else {
 		EX(Ts) = (temp_variable *) safe_emalloc(sizeof(temp_variable), op_array->T, 0);
 	}
	EX(op_array) = op_array;
	EX(original_in_execution) = EG(in_execution);
	EX(prev_execute_data) = EG(current_execute_data);
	EG(current_execute_data) = &execute_data;

	EG(in_execution) = 1;
	if (op_array->start_op) {
		SET_OPCODE(op_array->start_op);
	} else {
		SET_OPCODE(op_array->opcodes);
	}

	if (op_array->uses_this && EG(This)) {
		EG(This)->refcount++; /* For $this pointer */
		if (zend_hash_add(EG(active_symbol_table), "this", sizeof("this"), &EG(This), sizeof(zval *), NULL)==FAILURE) {
			EG(This)->refcount--;
		}
	}

	EG(opline_ptr) = &EX(opline);

	EX(function_state).function = (zend_function *) op_array;
	EG(function_state_ptr) = &EX(function_state);
#if ZEND_DEBUG
	/* function_state.function_symbol_table is saved as-is to a stack,
	 * which is an intentional UMR.  Shut it up if we're in DEBUG.
	 */
	EX(function_state).function_symbol_table = NULL;
#endif
	
	while (1) {
#ifdef ZEND_WIN32
		if (EG(timed_out)) {
			zend_timeout(0);
		}
#endif

		zend_clean_garbage(TSRMLS_C);
		if (EX(opline)->handler(&execute_data, EX(opline), op_array TSRMLS_CC)) {
			return;
		}
	}
	zend_error(E_ERROR, "Arrived at end of main loop which shouldn't happen");
}

/* CHECK_ME */
#undef EX
#define EX(element) execute_data->element

int zend_add_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	add_function(&EX_T(opline->result.u.var).tmp_var, 
		get_zval_ptr(&opline->op1, EX(Ts), &EG(free_op1), BP_VAR_R),
		get_zval_ptr(&opline->op2, EX(Ts), &EG(free_op2), BP_VAR_R) TSRMLS_CC);
	FREE_OP(EX(Ts), &opline->op1, EG(free_op1));
	FREE_OP(EX(Ts), &opline->op2, EG(free_op2));
	NEXT_OPCODE();
}


int zend_sub_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	sub_function(&EX_T(opline->result.u.var).tmp_var, 
		get_zval_ptr(&opline->op1, EX(Ts), &EG(free_op1), BP_VAR_R),
		get_zval_ptr(&opline->op2, EX(Ts), &EG(free_op2), BP_VAR_R) TSRMLS_CC);
	FREE_OP(EX(Ts), &opline->op1, EG(free_op1));
	FREE_OP(EX(Ts), &opline->op2, EG(free_op2));
	NEXT_OPCODE();
}


int zend_mul_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	mul_function(&EX_T(opline->result.u.var).tmp_var, 
		get_zval_ptr(&opline->op1, EX(Ts), &EG(free_op1), BP_VAR_R),
		get_zval_ptr(&opline->op2, EX(Ts), &EG(free_op2), BP_VAR_R) TSRMLS_CC);
	FREE_OP(EX(Ts), &opline->op1, EG(free_op1));
	FREE_OP(EX(Ts), &opline->op2, EG(free_op2));
	NEXT_OPCODE();
}


int zend_div_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	div_function(&EX_T(opline->result.u.var).tmp_var, 
		get_zval_ptr(&opline->op1, EX(Ts), &EG(free_op1), BP_VAR_R),
		get_zval_ptr(&opline->op2, EX(Ts), &EG(free_op2), BP_VAR_R) TSRMLS_CC);
	FREE_OP(EX(Ts), &opline->op1, EG(free_op1));
	FREE_OP(EX(Ts), &opline->op2, EG(free_op2));
	NEXT_OPCODE();
}


int zend_mod_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	mod_function(&EX_T(opline->result.u.var).tmp_var, 
		get_zval_ptr(&opline->op1, EX(Ts), &EG(free_op1), BP_VAR_R),
		get_zval_ptr(&opline->op2, EX(Ts), &EG(free_op2), BP_VAR_R) TSRMLS_CC);
	FREE_OP(EX(Ts), &opline->op1, EG(free_op1));
	FREE_OP(EX(Ts), &opline->op2, EG(free_op2));
	NEXT_OPCODE();
}


int zend_sl_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	shift_left_function(&EX_T(opline->result.u.var).tmp_var, 
		get_zval_ptr(&opline->op1, EX(Ts), &EG(free_op1), BP_VAR_R),
		get_zval_ptr(&opline->op2, EX(Ts), &EG(free_op2), BP_VAR_R) TSRMLS_CC);
	FREE_OP(EX(Ts), &opline->op1, EG(free_op1));
	FREE_OP(EX(Ts), &opline->op2, EG(free_op2));
	NEXT_OPCODE();
}


int zend_sr_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	shift_right_function(&EX_T(opline->result.u.var).tmp_var, 
		get_zval_ptr(&opline->op1, EX(Ts), &EG(free_op1), BP_VAR_R),
		get_zval_ptr(&opline->op2, EX(Ts), &EG(free_op2), BP_VAR_R) TSRMLS_CC);
	FREE_OP(EX(Ts), &opline->op1, EG(free_op1));
	FREE_OP(EX(Ts), &opline->op2, EG(free_op2));
	NEXT_OPCODE();
}


int zend_concat_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	concat_function(&EX_T(opline->result.u.var).tmp_var, 
		get_zval_ptr(&opline->op1, EX(Ts), &EG(free_op1), BP_VAR_R),
		get_zval_ptr(&opline->op2, EX(Ts), &EG(free_op2), BP_VAR_R) TSRMLS_CC);
	FREE_OP(EX(Ts), &opline->op1, EG(free_op1));
	FREE_OP(EX(Ts), &opline->op2, EG(free_op2));
	NEXT_OPCODE();
}


int zend_is_identical_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	is_identical_function(&EX_T(opline->result.u.var).tmp_var, 
		get_zval_ptr(&opline->op1, EX(Ts), &EG(free_op1), BP_VAR_R),
		get_zval_ptr(&opline->op2, EX(Ts), &EG(free_op2), BP_VAR_R) TSRMLS_CC);
	FREE_OP(EX(Ts), &opline->op1, EG(free_op1));
	FREE_OP(EX(Ts), &opline->op2, EG(free_op2));
	NEXT_OPCODE();
}


int zend_is_not_identical_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	is_not_identical_function(&EX_T(opline->result.u.var).tmp_var, 
		get_zval_ptr(&opline->op1, EX(Ts), &EG(free_op1), BP_VAR_R),
		get_zval_ptr(&opline->op2, EX(Ts), &EG(free_op2), BP_VAR_R) TSRMLS_CC);
	FREE_OP(EX(Ts), &opline->op1, EG(free_op1));
	FREE_OP(EX(Ts), &opline->op2, EG(free_op2));
	NEXT_OPCODE();
}


int zend_is_equal_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	is_equal_function(&EX_T(opline->result.u.var).tmp_var, 
		get_zval_ptr(&opline->op1, EX(Ts), &EG(free_op1), BP_VAR_R),
		get_zval_ptr(&opline->op2, EX(Ts), &EG(free_op2), BP_VAR_R) TSRMLS_CC);
	FREE_OP(EX(Ts), &opline->op1, EG(free_op1));
	FREE_OP(EX(Ts), &opline->op2, EG(free_op2));
	NEXT_OPCODE();
}


int zend_is_not_equal_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	is_not_equal_function(&EX_T(opline->result.u.var).tmp_var, 
		get_zval_ptr(&opline->op1, EX(Ts), &EG(free_op1), BP_VAR_R),
		get_zval_ptr(&opline->op2, EX(Ts), &EG(free_op2), BP_VAR_R) TSRMLS_CC);
	FREE_OP(EX(Ts), &opline->op1, EG(free_op1));
	FREE_OP(EX(Ts), &opline->op2, EG(free_op2));
	NEXT_OPCODE();
}


int zend_is_smaller_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	is_smaller_function(&EX_T(opline->result.u.var).tmp_var, 
		get_zval_ptr(&opline->op1, EX(Ts), &EG(free_op1), BP_VAR_R),
		get_zval_ptr(&opline->op2, EX(Ts), &EG(free_op2), BP_VAR_R) TSRMLS_CC);
	FREE_OP(EX(Ts), &opline->op1, EG(free_op1));
	FREE_OP(EX(Ts), &opline->op2, EG(free_op2));
	NEXT_OPCODE();
}


int zend_is_smaller_or_equal_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	is_smaller_or_equal_function(&EX_T(opline->result.u.var).tmp_var, 
		get_zval_ptr(&opline->op1, EX(Ts), &EG(free_op1), BP_VAR_R),
		get_zval_ptr(&opline->op2, EX(Ts), &EG(free_op2), BP_VAR_R) TSRMLS_CC);
	FREE_OP(EX(Ts), &opline->op1, EG(free_op1));
	FREE_OP(EX(Ts), &opline->op2, EG(free_op2));
	NEXT_OPCODE();
}


int zend_bw_or_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	bitwise_or_function(&EX_T(opline->result.u.var).tmp_var, 
		get_zval_ptr(&opline->op1, EX(Ts), &EG(free_op1), BP_VAR_R),
		get_zval_ptr(&opline->op2, EX(Ts), &EG(free_op2), BP_VAR_R) TSRMLS_CC);
	FREE_OP(EX(Ts), &opline->op1, EG(free_op1));
	FREE_OP(EX(Ts), &opline->op2, EG(free_op2));
	NEXT_OPCODE();
}


int zend_bw_and_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	bitwise_and_function(&EX_T(opline->result.u.var).tmp_var, 
		get_zval_ptr(&opline->op1, EX(Ts), &EG(free_op1), BP_VAR_R),
		get_zval_ptr(&opline->op2, EX(Ts), &EG(free_op2), BP_VAR_R) TSRMLS_CC);
	FREE_OP(EX(Ts), &opline->op1, EG(free_op1));
	FREE_OP(EX(Ts), &opline->op2, EG(free_op2));
	NEXT_OPCODE();
}


int zend_bw_xor_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	bitwise_xor_function(&EX_T(opline->result.u.var).tmp_var, 
		get_zval_ptr(&opline->op1, EX(Ts), &EG(free_op1), BP_VAR_R),
		get_zval_ptr(&opline->op2, EX(Ts), &EG(free_op2), BP_VAR_R) TSRMLS_CC);
	FREE_OP(EX(Ts), &opline->op1, EG(free_op1));
	FREE_OP(EX(Ts), &opline->op2, EG(free_op2));
	NEXT_OPCODE();
}


int zend_bool_xor_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	boolean_xor_function(&EX_T(opline->result.u.var).tmp_var, 
		get_zval_ptr(&opline->op1, EX(Ts), &EG(free_op1), BP_VAR_R),
		get_zval_ptr(&opline->op2, EX(Ts), &EG(free_op2), BP_VAR_R) TSRMLS_CC);
	FREE_OP(EX(Ts), &opline->op1, EG(free_op1));
	FREE_OP(EX(Ts), &opline->op2, EG(free_op2));
	NEXT_OPCODE();
}


int zend_bw_not_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	bitwise_not_function(&EX_T(opline->result.u.var).tmp_var,
		get_zval_ptr(&opline->op1, EX(Ts), &EG(free_op1), BP_VAR_R) TSRMLS_CC);
	FREE_OP(EX(Ts), &opline->op1, EG(free_op1));
	NEXT_OPCODE();
}


int zend_bool_not_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	boolean_not_function(&EX_T(opline->result.u.var).tmp_var,
		get_zval_ptr(&opline->op1, EX(Ts), &EG(free_op1), BP_VAR_R) TSRMLS_CC);
	FREE_OP(EX(Ts), &opline->op1, EG(free_op1));
	NEXT_OPCODE();
}


static inline int zend_binary_assign_op_obj_helper(int (*binary_op)(zval *result, zval *op1, zval *op2 TSRMLS_DC), ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *op_data = opline+1;
	zval **object_ptr = get_obj_zval_ptr_ptr(&opline->op1, EX(Ts), BP_VAR_W TSRMLS_CC);
	zval *object;
	zval *property = get_zval_ptr(&opline->op2, EX(Ts), &EG(free_op2), BP_VAR_R);
	zval *free_value;
	zval *value = get_zval_ptr(&op_data->op1, EX(Ts), &free_value, BP_VAR_R);
	znode *result = &opline->result;
	zval **retval = &EX_T(result->u.var).var.ptr;
	int have_get_ptr = 0;

	EX_T(result->u.var).var.ptr_ptr = NULL;
	make_real_object(object_ptr TSRMLS_CC);
	object = *object_ptr;
	
	if (object->type != IS_OBJECT) {
		zend_error(E_WARNING, "Attempt to assign property of non-object");
		FREE_OP(Ts, op2, EG(free_op2));
		FREE_OP(Ts, value, free_value);

		*retval = EG(uninitialized_zval_ptr);

		SELECTIVE_PZVAL_LOCK(*retval, result);
	} else {
		/* here we are sure we are dealing with an object */
		if (EG(free_op2)) {
			MAKE_REAL_ZVAL_PTR(property);
		}

		/* here property is a string */
		if (opline->extended_value == ZEND_ASSIGN_OBJ
			&& Z_OBJ_HT_P(object)->get_property_ptr_ptr) {
			zval **zptr = Z_OBJ_HT_P(object)->get_property_ptr_ptr(object, property TSRMLS_CC);
			if (zptr != NULL) { 			/* NULL means no success in getting PTR */
				SEPARATE_ZVAL_IF_NOT_REF(zptr);

				have_get_ptr = 1;
				binary_op(*zptr, *zptr, value TSRMLS_CC);
				*retval = *zptr;
				SELECTIVE_PZVAL_LOCK(*retval, result);
			}
		}

		if (!have_get_ptr) {
			zval *z;
			
			switch (opline->extended_value) {
				case ZEND_ASSIGN_OBJ:
					z = Z_OBJ_HT_P(object)->read_property(object, property, BP_VAR_RW TSRMLS_CC);
					break;
				case ZEND_ASSIGN_DIM:
					z = Z_OBJ_HT_P(object)->read_dimension(object, property, BP_VAR_RW TSRMLS_CC);
					break;
			}
			if (z->type == IS_OBJECT && Z_OBJ_HT_P(z)->get) {
				zval *value = Z_OBJ_HT_P(z)->get(z TSRMLS_CC);

				if (z->refcount == 0) {
					zval_dtor(z);
					FREE_ZVAL(z);
				}
				z = value;
			}
			z->refcount++;
			SEPARATE_ZVAL_IF_NOT_REF(&z);
			binary_op(z, z, value TSRMLS_CC);
			switch (opline->extended_value) {
				case ZEND_ASSIGN_OBJ:
					Z_OBJ_HT_P(object)->write_property(object, property, z TSRMLS_CC);
					break;
				case ZEND_ASSIGN_DIM:
					Z_OBJ_HT_P(object)->write_dimension(object, property, z TSRMLS_CC);
					break;
			}
			*retval = z;
			SELECTIVE_PZVAL_LOCK(*retval, result);
			zval_ptr_dtor(&z);
		}

		if (EG(free_op2)) {
			zval_ptr_dtor(&property);
		}
		FREE_OP(Ts, value, free_value);
	}

	/* assign_obj has two opcodes! */
	INC_OPCODE();
	NEXT_OPCODE();
}


static inline int zend_binary_assign_op_helper(int (*binary_op)(zval *result, zval *op1, zval *op2 TSRMLS_DC), ZEND_OPCODE_HANDLER_ARGS)
{
	zval **var_ptr;
	zval *value;
	zend_bool increment_opline = 0;

	switch (opline->extended_value) {
		case ZEND_ASSIGN_OBJ:
			return zend_binary_assign_op_obj_helper(binary_op, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
			break;
		case ZEND_ASSIGN_DIM: {
				zval **object_ptr = get_obj_zval_ptr_ptr(&opline->op1, EX(Ts), BP_VAR_W TSRMLS_CC);

				(*object_ptr)->refcount++;  /* undo the effect of get_obj_zval_ptr_ptr() */

				if ((*object_ptr)->type == IS_OBJECT) {
					return zend_binary_assign_op_obj_helper(binary_op, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
				} else {
					zend_op *op_data = opline+1;

					zend_fetch_dimension_address(&op_data->op2, &opline->op1, &opline->op2, EX(Ts), BP_VAR_RW TSRMLS_CC);

					value = get_zval_ptr(&op_data->op1, EX(Ts), &EG(free_op1), BP_VAR_R);
					var_ptr = get_zval_ptr_ptr(&op_data->op2, EX(Ts), BP_VAR_RW);
					EG(free_op2) = 0;
					increment_opline = 1;
				}
			}
			break;
		default:
			value = get_zval_ptr(&opline->op2, EX(Ts), &EG(free_op2), BP_VAR_R);
			var_ptr = get_zval_ptr_ptr(&opline->op1, EX(Ts), BP_VAR_RW);
			EG(free_op1) = 0;
			/* do nothing */
			break;
	}

	if (!var_ptr) {
		zend_error(E_ERROR, "Cannot use assign-op operators with overloaded objects nor string offsets");
	}

	if (*var_ptr == EG(error_zval_ptr)) {
		EX_T(opline->result.u.var).var.ptr_ptr = &EG(uninitialized_zval_ptr);
		SELECTIVE_PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr, &opline->result);
		AI_USE_PTR(EX_T(opline->result.u.var).var);
	    if (increment_opline) {
    	    INC_OPCODE();
    	}
		NEXT_OPCODE();
	}
	
	SEPARATE_ZVAL_IF_NOT_REF(var_ptr);

	if(Z_TYPE_PP(var_ptr) == IS_OBJECT && Z_OBJ_HANDLER_PP(var_ptr, get)
	   && Z_OBJ_HANDLER_PP(var_ptr, set)) {
		/* proxy object */
		zval *objval = Z_OBJ_HANDLER_PP(var_ptr, get)(*var_ptr TSRMLS_CC);
		objval->refcount++;
		binary_op(objval, objval, value TSRMLS_CC);
		Z_OBJ_HANDLER_PP(var_ptr, set)(var_ptr, objval TSRMLS_CC);
		zval_ptr_dtor(&objval);
	} else {
		binary_op(*var_ptr, *var_ptr, value TSRMLS_CC);
	}
	
	EX_T(opline->result.u.var).var.ptr_ptr = var_ptr;
	SELECTIVE_PZVAL_LOCK(*var_ptr, &opline->result);
	FREE_OP(Ex(Ts), &opline->op1, EG(free_op1));
	FREE_OP(EX(Ts), &opline->op2, EG(free_op2));
	AI_USE_PTR(EX_T(opline->result.u.var).var);

	if (increment_opline) {
		INC_OPCODE();
	}
	NEXT_OPCODE();
}


int zend_assign_add_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper(add_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}


int zend_assign_sub_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper(sub_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}


int zend_assign_mul_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper(mul_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}


int zend_assign_div_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper(div_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}


int zend_assign_mod_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper(mod_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}


int zend_assign_sl_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper(shift_left_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}


int zend_assign_sr_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper(shift_right_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}


int zend_assign_concat_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper(concat_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}


int zend_assign_bw_or_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper(bitwise_or_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}


int zend_assign_bw_and_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper(bitwise_and_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}


int zend_assign_bw_xor_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_binary_assign_op_helper(bitwise_xor_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}

int zend_pre_inc_obj_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_pre_incdec_property(&opline->result, &opline->op1, &opline->op2, EX(Ts), increment_function TSRMLS_CC);
	NEXT_OPCODE();
}


int zend_pre_dec_obj_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_pre_incdec_property(&opline->result, &opline->op1, &opline->op2, EX(Ts), decrement_function TSRMLS_CC);
	NEXT_OPCODE();
}


int zend_post_inc_obj_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_post_incdec_property(&opline->result, &opline->op1, &opline->op2, EX(Ts), increment_function TSRMLS_CC);
	NEXT_OPCODE();
}


int zend_post_dec_obj_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_post_incdec_property(&opline->result, &opline->op1, &opline->op2, EX(Ts), decrement_function TSRMLS_CC);
	NEXT_OPCODE();
}

typedef int (*incdec_t)(zval *);

static inline int zend_incdec_op_helper(incdec_t incdec_op_arg, ZEND_OPCODE_HANDLER_ARGS)
{
	zval **var_ptr = get_zval_ptr_ptr(&opline->op1, EX(Ts), BP_VAR_RW);
	int (*incdec_op)(zval *op1) = incdec_op_arg;

	if (!var_ptr) {
		zend_error(E_ERROR, "Cannot increment/decrement overloaded objects nor string offsets");
	}
	if (*var_ptr == EG(error_zval_ptr)) {
		switch (opline->opcode) {
			case ZEND_PRE_INC:
			case ZEND_PRE_DEC:
				EX_T(opline->result.u.var).var.ptr_ptr = &EG(uninitialized_zval_ptr);
				SELECTIVE_PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr, &opline->result);
				AI_USE_PTR(EX_T(opline->result.u.var).var);
				NEXT_OPCODE();
				break;
			case ZEND_POST_INC:
			case ZEND_POST_DEC:
				EX_T(opline->result.u.var).tmp_var = *EG(uninitialized_zval_ptr);
				NEXT_OPCODE();
				break;
		}
	}

	switch (opline->opcode) {
		case ZEND_POST_INC:
		case ZEND_POST_DEC:
			EX_T(opline->result.u.var).tmp_var = **var_ptr;
			zendi_zval_copy_ctor(EX_T(opline->result.u.var).tmp_var);
			break;
	}
	
	SEPARATE_ZVAL_IF_NOT_REF(var_ptr);

	if(Z_TYPE_PP(var_ptr) == IS_OBJECT && Z_OBJ_HANDLER_PP(var_ptr, get)
	   && Z_OBJ_HANDLER_PP(var_ptr, set)) {
		/* proxy object */
		zval *val = Z_OBJ_HANDLER_PP(var_ptr, get)(*var_ptr TSRMLS_CC);
		val->refcount++;
		incdec_op(val);
		Z_OBJ_HANDLER_PP(var_ptr, set)(var_ptr, val TSRMLS_CC);
		zval_ptr_dtor(&val);
	} else {
		incdec_op(*var_ptr);
	}

	switch (opline->opcode) {
		case ZEND_PRE_INC:
		case ZEND_PRE_DEC:
			EX_T(opline->result.u.var).var.ptr_ptr = var_ptr;
			SELECTIVE_PZVAL_LOCK(*var_ptr, &opline->result);
			AI_USE_PTR(EX_T(opline->result.u.var).var);
			break;
	}
	NEXT_OPCODE();
}


int zend_pre_inc_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_incdec_op_helper(increment_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}


int zend_pre_dec_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_incdec_op_helper(decrement_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}


int zend_post_inc_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_incdec_op_helper(increment_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}


int zend_post_dec_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_incdec_op_helper(decrement_function, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}


int zend_echo_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	zval *free_op1;
	zval z_copy;
	zval *z = get_zval_ptr(&opline->op1, EX(Ts), &free_op1, BP_VAR_R);

	if (Z_TYPE_P(z) == IS_OBJECT && Z_OBJ_HT_P(z)->get_method != NULL && 
		zend_std_cast_object_tostring(z, &z_copy, IS_STRING, 0 TSRMLS_CC) == SUCCESS) {
		zend_print_variable(&z_copy);
		zval_dtor(&z_copy);
	} else {
		zend_print_variable(z);
	}

	FREE_OP(EX(Ts), &opline->op1, free_op1);
	NEXT_OPCODE();
}


int zend_print_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	EX_T(opline->result.u.var).tmp_var.value.lval = 1;
	EX_T(opline->result.u.var).tmp_var.type = IS_LONG;

	return zend_echo_handler(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}


int zend_fetch_r_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_fetch_var_address(opline, EX(Ts), BP_VAR_R TSRMLS_CC);
	AI_USE_PTR(EX_T(opline->result.u.var).var);
	NEXT_OPCODE();
}


int zend_fetch_w_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_fetch_var_address(opline, EX(Ts), BP_VAR_W TSRMLS_CC);
	NEXT_OPCODE();
}


int zend_fetch_rw_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_fetch_var_address(opline, EX(Ts), BP_VAR_RW TSRMLS_CC);
	NEXT_OPCODE();
}


int zend_fetch_func_arg_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	if (ARG_SHOULD_BE_SENT_BY_REF(EX(fbc), opline->extended_value)) {
		/* Behave like FETCH_W */
		zend_fetch_var_address(opline, EX(Ts), BP_VAR_W TSRMLS_CC);
	} else {
		/* Behave like FETCH_R */
		zend_fetch_var_address(opline, EX(Ts), BP_VAR_R TSRMLS_CC);
		AI_USE_PTR(EX_T(opline->result.u.var).var);
	}
	NEXT_OPCODE();
}


int zend_fetch_unset_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_fetch_var_address(opline, EX(Ts), BP_VAR_R TSRMLS_CC);
	PZVAL_UNLOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
	if (EX_T(opline->result.u.var).var.ptr_ptr != &EG(uninitialized_zval_ptr)) {
		SEPARATE_ZVAL_IF_NOT_REF(EX_T(opline->result.u.var).var.ptr_ptr);
	}
	PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
	NEXT_OPCODE();
}


int zend_fetch_is_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_fetch_var_address(opline, EX(Ts), BP_VAR_IS TSRMLS_CC);
	AI_USE_PTR(EX_T(opline->result.u.var).var);
	NEXT_OPCODE();
}


int zend_fetch_dim_r_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	if (opline->extended_value == ZEND_FETCH_ADD_LOCK) {
		PZVAL_LOCK(*EX_T(opline->op1.u.var).var.ptr_ptr);
	}
	zend_fetch_dimension_address(&opline->result, &opline->op1, &opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);
	AI_USE_PTR(EX_T(opline->result.u.var).var);
	NEXT_OPCODE();
}


int zend_fetch_dim_w_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_fetch_dimension_address(&opline->result, &opline->op1, &opline->op2, EX(Ts), BP_VAR_W TSRMLS_CC);
	NEXT_OPCODE();
}


int zend_fetch_dim_rw_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_fetch_dimension_address(&opline->result, &opline->op1, &opline->op2, EX(Ts), BP_VAR_RW TSRMLS_CC);
	NEXT_OPCODE();
}


int zend_fetch_dim_is_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_fetch_dimension_address(&opline->result, &opline->op1, &opline->op2, EX(Ts), BP_VAR_IS TSRMLS_CC);
	AI_USE_PTR(EX_T(opline->result.u.var).var);
	NEXT_OPCODE();
}


int zend_fetch_dim_func_arg_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	if (ARG_SHOULD_BE_SENT_BY_REF(EX(fbc), opline->extended_value)) {
		/* Behave like FETCH_DIM_W */
		zend_fetch_dimension_address(&opline->result, &opline->op1, &opline->op2, EX(Ts), BP_VAR_W TSRMLS_CC);
	} else {
		if (opline->op2.op_type == IS_UNUSED) {
			zend_error(E_ERROR, "Cannot use [] for reading");
		}
		/* Behave like FETCH_DIM_R, except for locking used for list() */
		zend_fetch_dimension_address(&opline->result, &opline->op1, &opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);
		AI_USE_PTR(EX_T(opline->result.u.var).var);
	}
	NEXT_OPCODE();
}


int zend_fetch_dim_unset_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	/* Not needed in DIM_UNSET
	if (opline->extended_value == ZEND_FETCH_ADD_LOCK) {
		PZVAL_LOCK(*EX_T(opline->op1.u.var).var.ptr_ptr);
	}
	*/
	zend_fetch_dimension_address(&opline->result, &opline->op1, &opline->op2, EX(Ts), BP_VAR_UNSET TSRMLS_CC);
	if (EX_T(opline->result.u.var).var.ptr_ptr == NULL) {
		zend_error(E_ERROR, "Cannot unset string offsets");
	} else {
		PZVAL_UNLOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
		if (EX_T(opline->result.u.var).var.ptr_ptr != &EG(uninitialized_zval_ptr)) {
			SEPARATE_ZVAL_IF_NOT_REF(EX_T(opline->result.u.var).var.ptr_ptr);
		}
		PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
	}
	NEXT_OPCODE();
}


int zend_fetch_obj_r_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_fetch_property_address_read(&opline->result, &opline->op1, &opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);
	AI_USE_PTR(EX_T(opline->result.u.var).var);
	NEXT_OPCODE();
}


int zend_fetch_obj_w_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	if (opline->extended_value == ZEND_FETCH_ADD_LOCK) {
		PZVAL_LOCK(*EX_T(opline->op1.u.var).var.ptr_ptr);
		EX_T(opline->op1.u.var).var.ptr = *EX_T(opline->op1.u.var).var.ptr_ptr;
	}
	zend_fetch_property_address(&opline->result, &opline->op1, &opline->op2, EX(Ts), BP_VAR_W TSRMLS_CC);
	NEXT_OPCODE();
}


int zend_fetch_obj_rw_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_fetch_property_address(&opline->result, &opline->op1, &opline->op2, EX(Ts), BP_VAR_RW TSRMLS_CC);
	NEXT_OPCODE();
}


int zend_fetch_obj_is_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_fetch_property_address_read(&opline->result, &opline->op1, &opline->op2, EX(Ts), BP_VAR_IS TSRMLS_CC);
	AI_USE_PTR(EX_T(opline->result.u.var).var);
	NEXT_OPCODE();
}


int zend_fetch_obj_func_arg_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	if (ARG_SHOULD_BE_SENT_BY_REF(EX(fbc), opline->extended_value)) {
		/* Behave like FETCH_OBJ_W */
		zend_fetch_property_address(&opline->result, &opline->op1, &opline->op2, EX(Ts), BP_VAR_W TSRMLS_CC);
	} else {
		zend_fetch_property_address_read(&opline->result, &opline->op1, &opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);
		AI_USE_PTR(EX_T(opline->result.u.var).var);
	}
	NEXT_OPCODE();
}


int zend_fetch_obj_unset_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_fetch_property_address(&opline->result, &opline->op1, &opline->op2, EX(Ts), BP_VAR_R TSRMLS_CC);

	PZVAL_UNLOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
	if (EX_T(opline->result.u.var).var.ptr_ptr != &EG(uninitialized_zval_ptr)) {
		SEPARATE_ZVAL_IF_NOT_REF(EX_T(opline->result.u.var).var.ptr_ptr);
	}
	PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
	NEXT_OPCODE();
}


int zend_fetch_dim_tmp_var_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_fetch_dimension_address_from_tmp_var(&opline->result, &opline->op1, &opline->op2, EX(Ts) TSRMLS_CC);
	AI_USE_PTR(EX_T(opline->result.u.var).var);
	NEXT_OPCODE();
}


int zend_assign_obj_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *op_data = opline+1;
	zval **object_ptr = get_obj_zval_ptr_ptr(&opline->op1, EX(Ts), BP_VAR_W TSRMLS_CC);

	zend_assign_to_object(&opline->result, object_ptr, &opline->op2, &op_data->op1, EX(Ts), ZEND_ASSIGN_OBJ TSRMLS_CC);
	/* assign_obj has two opcodes! */
	INC_OPCODE();
	NEXT_OPCODE();
}


int zend_assign_dim_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *op_data = opline+1;
	zval **object_ptr;
	
	if (EX_T(opline->op1.u.var).var.ptr_ptr) {
		/* not an array offset */
		object_ptr = get_obj_zval_ptr_ptr(&opline->op1, EX(Ts), BP_VAR_W TSRMLS_CC);
	} else {
		object_ptr = NULL;
	}

	if (object_ptr && (*object_ptr)->type == IS_OBJECT) {
		zend_assign_to_object(&opline->result, object_ptr, &opline->op2, &op_data->op1, EX(Ts), ZEND_ASSIGN_DIM TSRMLS_CC);
	} else {
		zval *value;

		if (object_ptr) {
			(*object_ptr)->refcount++;  /* undo the effect of get_obj_zval_ptr_ptr() */
		}
		zend_fetch_dimension_address(&op_data->op2, &opline->op1, &opline->op2, EX(Ts), BP_VAR_W TSRMLS_CC);

		value = get_zval_ptr(&op_data->op1, EX(Ts), &EG(free_op1), BP_VAR_R);
	 	zend_assign_to_variable(&opline->result, &op_data->op2, &op_data->op1, value, (EG(free_op1)?IS_TMP_VAR:op_data->op1.op_type), EX(Ts) TSRMLS_CC);
	}
	/* assign_dim has two opcodes! */
	INC_OPCODE();
	NEXT_OPCODE();
}


int zend_assign_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	zval *value;
	value = get_zval_ptr(&opline->op2, EX(Ts), &EG(free_op2), BP_VAR_R);

 	zend_assign_to_variable(&opline->result, &opline->op1, &opline->op2, value, (EG(free_op2)?IS_TMP_VAR:opline->op2.op_type), EX(Ts) TSRMLS_CC);
	/* zend_assign_to_variable() always takes care of op2, never free it! */

	NEXT_OPCODE();
}


int zend_assign_ref_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	zval **value_ptr_ptr = get_zval_ptr_ptr(&opline->op2, EX(Ts), BP_VAR_W);

	if (opline->op2.op_type == IS_VAR &&
	    value_ptr_ptr &&
	    !(*value_ptr_ptr)->is_ref &&
	     opline->extended_value == ZEND_RETURNS_FUNCTION && 
	     !EX_T(opline->op2.u.var).var.fcall_returned_reference) {
		PZVAL_LOCK(*value_ptr_ptr); /* undo the effect of get_zval_ptr_ptr() */
		zend_error(E_STRICT, "Only variables should be assigned by reference");
		return zend_assign_handler(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
	}
	if (opline->op1.op_type == IS_VAR && EX_T(opline->op1.u.var).var.ptr_ptr == &EX_T(opline->op1.u.var).var.ptr) {
		zend_error(E_ERROR, "Cannot assign by reference to overloaded object");
	}

	zend_assign_to_variable_reference(&opline->result, get_zval_ptr_ptr(&opline->op1, EX(Ts), BP_VAR_W), value_ptr_ptr, EX(Ts) TSRMLS_CC);

	NEXT_OPCODE();
}


int zend_jmp_handler(ZEND_OPCODE_HANDLER_ARGS)
{
#if DEBUG_ZEND>=2
	printf("Jumping to %d\n", opline->op1.u.opline_num);
#endif
	SET_OPCODE(opline->op1.u.jmp_addr);
	return 0; /* CHECK_ME */
}


int zend_jmpz_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	znode *op1 = &opline->op1;
					
	if (!i_zend_is_true(get_zval_ptr(op1, EX(Ts), &EG(free_op1), BP_VAR_R))) {
#if DEBUG_ZEND>=2
		printf("Conditional jmp to %d\n", opline->op2.u.opline_num);
#endif
		SET_OPCODE(opline->op2.u.jmp_addr);
		FREE_OP(EX(Ts), op1, EG(free_op1));
		return 0; /* CHECK_ME */
	}
	FREE_OP(EX(Ts), op1, EG(free_op1));

	NEXT_OPCODE();
}


int zend_jmpnz_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	znode *op1 = &opline->op1;
	
	if (zend_is_true(get_zval_ptr(op1, EX(Ts), &EG(free_op1), BP_VAR_R))) {
#if DEBUG_ZEND>=2
		printf("Conditional jmp to %d\n", opline->op2.u.opline_num);
#endif
		SET_OPCODE(opline->op2.u.jmp_addr);
		FREE_OP(EX(Ts), op1, EG(free_op1));
		return 0; /* CHECK_ME */
	}
	FREE_OP(EX(Ts), op1, EG(free_op1));

	NEXT_OPCODE();
}


int zend_jmpznz_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	znode *res = &opline->op1;
	
	if (zend_is_true(get_zval_ptr(res, EX(Ts), &EG(free_op1), BP_VAR_R))) {
#if DEBUG_ZEND>=2
		printf("Conditional jmp on true to %d\n", opline->extended_value);
#endif
		SET_OPCODE(&op_array->opcodes[opline->extended_value]);
	} else {
#if DEBUG_ZEND>=2
		printf("Conditional jmp on false to %d\n", opline->op2.u.opline_num);
#endif
		SET_OPCODE(&op_array->opcodes[opline->op2.u.opline_num]);
	}
	FREE_OP(EX(Ts), res, EG(free_op1));

	return 0; /* CHECK_ME */
}


int zend_jmpz_ex_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *original_opline = opline;
	int retval = zend_is_true(get_zval_ptr(&original_opline->op1, EX(Ts), &EG(free_op1), BP_VAR_R));
	
	FREE_OP(EX(Ts), &original_opline->op1, EG(free_op1));
	EX_T(original_opline->result.u.var).tmp_var.value.lval = retval;
	EX_T(original_opline->result.u.var).tmp_var.type = IS_BOOL;
	if (!retval) {
#if DEBUG_ZEND>=2
		printf("Conditional jmp to %d\n", opline->op2.u.opline_num);
#endif
		SET_OPCODE(opline->op2.u.jmp_addr);
		return 0; /* CHECK_ME */
	}
	NEXT_OPCODE();
}


int zend_jmpnz_ex_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *original_opline = opline;
	int retval = zend_is_true(get_zval_ptr(&original_opline->op1, EX(Ts), &EG(free_op1), BP_VAR_R));
	
	FREE_OP(EX(Ts), &original_opline->op1, EG(free_op1));
	EX_T(original_opline->result.u.var).tmp_var.value.lval = retval;
	EX_T(original_opline->result.u.var).tmp_var.type = IS_BOOL;
	if (retval) {
#if DEBUG_ZEND>=2
		printf("Conditional jmp to %d\n", opline->op2.u.opline_num);
#endif
		SET_OPCODE(opline->op2.u.jmp_addr);
		return 0; /* CHECK_ME */
	}
	NEXT_OPCODE();
}


int zend_free_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	zendi_zval_dtor(EX_T(opline->op1.u.var).tmp_var);
	NEXT_OPCODE();
}


int zend_init_string_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	EX_T(opline->result.u.var).tmp_var.value.str.val = emalloc(1);
	EX_T(opline->result.u.var).tmp_var.value.str.val[0] = 0;
	EX_T(opline->result.u.var).tmp_var.value.str.len = 0;
	EX_T(opline->result.u.var).tmp_var.refcount = 1;
	EX_T(opline->result.u.var).tmp_var.type = IS_STRING;
	EX_T(opline->result.u.var).tmp_var.is_ref = 0;
	NEXT_OPCODE();
}


int zend_add_char_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	add_char_to_string(&EX_T(opline->result.u.var).tmp_var,
		get_zval_ptr(&opline->op1, EX(Ts), &EG(free_op1), BP_VAR_NA),
		&opline->op2.u.constant);
	/* FREE_OP is missing intentionally here - we're always working on the same temporary variable */
	NEXT_OPCODE();
}


int zend_add_string_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	add_string_to_string(&EX_T(opline->result.u.var).tmp_var,
		get_zval_ptr(&opline->op1, EX(Ts), &EG(free_op1), BP_VAR_NA),
		&opline->op2.u.constant);
	/* FREE_OP is missing intentionally here - we're always working on the same temporary variable */
	NEXT_OPCODE();
}


int zend_add_var_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	zval *var = get_zval_ptr(&opline->op2, EX(Ts), &EG(free_op2), BP_VAR_R);
	zval var_copy;
	int use_copy;

	zend_make_printable_zval(var, &var_copy, &use_copy);
	if (use_copy) {
		var = &var_copy;
	}
	add_string_to_string(	&EX_T(opline->result.u.var).tmp_var,
							get_zval_ptr(&opline->op1, EX(Ts), &EG(free_op1), BP_VAR_NA),
							var);
	if (use_copy) {
		zval_dtor(var);
	}
	/* original comment, possibly problematic:
	 * FREE_OP is missing intentionally here - we're always working on the same temporary variable
	 * (Zeev):  I don't think it's problematic, we only use variables
	 * which aren't affected by FREE_OP(Ts, )'s anyway, unless they're
	 * string offsets or overloaded objects
	 */
	FREE_OP(EX(Ts), &opline->op2, EG(free_op2));

	NEXT_OPCODE();
}


int zend_fetch_class_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	zval *class_name;
	

	if (opline->op2.op_type == IS_UNUSED) {
		EX_T(opline->result.u.var).class_entry = zend_fetch_class(NULL, 0, opline->extended_value TSRMLS_CC);
		NEXT_OPCODE();
	}

	class_name = get_zval_ptr(&opline->op2, EX(Ts), &EG(free_op2), BP_VAR_R);

	switch (class_name->type) {
		case IS_OBJECT:
			EX_T(opline->result.u.var).class_entry = Z_OBJCE_P(class_name);
			break;
		case IS_STRING:
			EX_T(opline->result.u.var).class_entry = zend_fetch_class(Z_STRVAL_P(class_name), Z_STRLEN_P(class_name), opline->extended_value TSRMLS_CC);
			break;
		default:
			zend_error(E_ERROR, "Class name must be a valid object or a string");
			break;
	}

	FREE_OP(EX(Ts), &opline->op2, EG(free_op2));
	NEXT_OPCODE();
}


int zend_init_ctor_call_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_ptr_stack_n_push(&EG(arg_types_stack), 3, EX(fbc), EX(object), EX(calling_scope));

	if (opline->op1.op_type == IS_VAR) {
		SELECTIVE_PZVAL_LOCK(*EX_T(opline->op1.u.var).var.ptr_ptr, &opline->op1);
	}

	/* We are not handling overloaded classes right now */
	EX(object) = get_zval_ptr(&opline->op1, EX(Ts), &EG(free_op1), BP_VAR_R);

	/* New always returns the object as is_ref=0, therefore, we can just increment the reference count */
	EX(object)->refcount++; /* For $this pointer */

	EX(fbc) = EX(fbc_constructor);

	if (EX(fbc)->type == ZEND_USER_FUNCTION) { /* HACK!! */
		EX(calling_scope) = EX(fbc)->common.scope;
	} else {
		EX(calling_scope) = NULL;
	}

	NEXT_OPCODE();
}


int zend_init_method_call_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	zval *function_name;
	char *function_name_strval;
	int function_name_strlen;
	
	zend_ptr_stack_n_push(&EG(arg_types_stack), 3, EX(fbc), EX(object), EX(calling_scope));

	function_name = get_zval_ptr(&opline->op2, EX(Ts), &EG(free_op2), BP_VAR_R);

	if (Z_TYPE_P(function_name)!=IS_STRING) {
		zend_error(E_ERROR, "Method name must be a string");
	}

	function_name_strval = function_name->value.str.val;
	function_name_strlen = function_name->value.str.len;

	EX(calling_scope) = EG(scope);

	EX(object) = get_obj_zval_ptr(&opline->op1, EX(Ts), &EG(free_op1), BP_VAR_R TSRMLS_CC);
			
	if (EX(object) && EX(object)->type == IS_OBJECT) {
		if (Z_OBJ_HT_P(EX(object))->get_method == NULL) {
			zend_error(E_ERROR, "Object does not support method calls");
		}

		/* First, locate the function. */
		EX(fbc) = Z_OBJ_HT_P(EX(object))->get_method(EX(object), function_name_strval, function_name_strlen TSRMLS_CC);
		if (!EX(fbc)) {
			zend_error(E_ERROR, "Call to undefined method %s::%s()", Z_OBJ_CLASS_NAME_P(EX(object)), function_name_strval);
		}
	} else {
		zend_error(E_ERROR, "Call to a member function %s() on a non-object", function_name_strval);
	}

	if (EX(fbc)->common.fn_flags & ZEND_ACC_STATIC) {
		EX(object) = NULL;
	} else {
		if (!PZVAL_IS_REF(EX(object))) {
			EX(object)->refcount++; /* For $this pointer */
		} else {
			zval *this_ptr;
			ALLOC_ZVAL(this_ptr);
			*this_ptr = *EX(object);
			INIT_PZVAL(this_ptr);
			zval_copy_ctor(this_ptr);
			EX(object) = this_ptr;
		}	
	}

	if (EX(fbc)->type == ZEND_USER_FUNCTION) {
		EX(calling_scope) = EX(fbc)->common.scope;
	} else {
		EX(calling_scope) = NULL;
	}

	FREE_OP(EX(Ts), &opline->op2, EG(free_op2));
	
	NEXT_OPCODE();
}


int zend_init_static_method_call_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	zval *function_name;
	zend_class_entry *ce;

	zend_ptr_stack_n_push(&EG(arg_types_stack), 3, EX(fbc), EX(object), EX(calling_scope));

	ce = EX_T(opline->op1.u.var).class_entry;
	if(opline->op2.op_type != IS_UNUSED) {
		char *function_name_strval;
		int function_name_strlen;
		zend_bool is_const = (opline->op2.op_type == IS_CONST);

		if (is_const) {
			function_name_strval = opline->op2.u.constant.value.str.val;
			function_name_strlen = opline->op2.u.constant.value.str.len;
		} else {
			function_name = get_zval_ptr(&opline->op2, EX(Ts), &EG(free_op2), BP_VAR_R);

			if (Z_TYPE_P(function_name) != IS_STRING) {
				zend_error(E_ERROR, "Function name must be a string");
			}
			function_name_strval = zend_str_tolower_dup(function_name->value.str.val, function_name->value.str.len);
			function_name_strlen = function_name->value.str.len;
		}

		EX(fbc) = zend_std_get_static_method(ce, function_name_strval, function_name_strlen TSRMLS_CC);

		if (!is_const) {
			efree(function_name_strval);
			FREE_OP(EX(Ts), &opline->op2, EG(free_op2));
		}	
	} else {
		if(!ce->constructor) {
			zend_error(E_ERROR, "Can not call constructor");
		}
		EX(fbc) = ce->constructor;
	}

	EX(calling_scope) = EX(fbc)->common.scope;

	if (EX(fbc)->common.fn_flags & ZEND_ACC_STATIC) {
		EX(object) = NULL;
	} else {
		if ((EX(object) = EG(This))) {
			EX(object)->refcount++;
		}
	}

	NEXT_OPCODE();
}

int zend_init_fcall_by_name_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	zval *function_name;
	zend_function *function;
	zend_bool is_const;
	char *function_name_strval, *lcname;
	int function_name_strlen;

	zend_ptr_stack_n_push(&EG(arg_types_stack), 3, EX(fbc), EX(object), EX(calling_scope));

	is_const = (opline->op2.op_type == IS_CONST);

	if (is_const) {
		function_name_strval = opline->op2.u.constant.value.str.val;
		function_name_strlen = opline->op2.u.constant.value.str.len;
	} else {
		function_name = get_zval_ptr(&opline->op2, EX(Ts), &EG(free_op2), BP_VAR_R);

		if (Z_TYPE_P(function_name) != IS_STRING) {
			zend_error(E_ERROR, "Function name must be a string");
		}
		function_name_strval = function_name->value.str.val;
		function_name_strlen = function_name->value.str.len;
	}

	lcname = zend_str_tolower_dup(function_name_strval, function_name_strlen);
	if (zend_hash_find(EG(function_table), lcname, function_name_strlen+1, (void **) &function)==FAILURE) {
		efree(lcname);
		zend_error(E_ERROR, "Call to undefined function %s()", function_name_strval);
	}

	efree(lcname);
	if (!is_const) {
		FREE_OP(EX(Ts), &opline->op2, EG(free_op2));
	} 

	EX(calling_scope) = function->common.scope;
	EX(object) = NULL;
	
	EX(fbc) = function;

	NEXT_OPCODE();
}

int zend_do_fcall_common_helper(ZEND_OPCODE_HANDLER_ARGS)
{
	zval **original_return_value;
	zend_class_entry *current_scope;
	zval *current_this;
	int return_value_used = RETURN_VALUE_USED(opline);
	zend_bool should_change_scope;

	if (EX(function_state).function->common.fn_flags & ZEND_ACC_ABSTRACT) {
		zend_error(E_ERROR, "Cannot call abstract method %s::%s()", EX(function_state).function->common.scope->name, EX(function_state).function->common.function_name);
		NEXT_OPCODE(); /* Never reached */
	}

	zend_ptr_stack_n_push(&EG(argument_stack), 2, (void *) opline->extended_value, NULL);

	EX_T(opline->result.u.var).var.ptr_ptr = &EX_T(opline->result.u.var).var.ptr;

	if (EX(function_state).function->type == ZEND_USER_FUNCTION
		|| EX(function_state).function->common.scope) {
		should_change_scope = 1;
		current_this = EG(This);
		EG(This) = EX(object);
		current_scope = EG(scope);
		EG(scope) = EX(calling_scope);
	} else {
		should_change_scope = 0;
	}

	EX_T(opline->result.u.var).var.fcall_returned_reference = 0;

	if (EX(function_state).function->common.scope) {
		if (!EG(This) && !(EX(function_state).function->common.fn_flags & ZEND_ACC_STATIC)) {
			int severity;
			char *severity_word;
			if (EX(function_state).function->common.fn_flags & ZEND_ACC_ALLOW_STATIC) {
				severity = E_STRICT;
				severity_word = "should not";
			} else {
				severity = E_ERROR;
				severity_word = "cannot";
			}
			zend_error(severity, "Non-static method %s::%s() %s be called statically", EX(function_state).function->common.scope->name, EX(function_state).function->common.function_name, severity_word);
		}
	}
	if (EX(function_state).function->type == ZEND_INTERNAL_FUNCTION) {	
		ALLOC_ZVAL(EX_T(opline->result.u.var).var.ptr);
		INIT_ZVAL(*(EX_T(opline->result.u.var).var.ptr));

		if (EX(function_state).function->common.arg_info) {
			zend_uint i=0;
			zval **p;
			ulong arg_count;

			p = (zval **) EG(argument_stack).top_element-2;
			arg_count = (ulong) *p;

			while (arg_count>0) {
				zend_verify_arg_type(EX(function_state).function, ++i, *(p-arg_count) TSRMLS_CC);
				arg_count--;
			}
		}
		if (!zend_execute_internal) {
			/* saves one function call if zend_execute_internal is not used */
			((zend_internal_function *) EX(function_state).function)->handler(opline->extended_value, EX_T(opline->result.u.var).var.ptr, EX(object), return_value_used TSRMLS_CC);
		} else {
			zend_execute_internal(execute_data, return_value_used TSRMLS_CC);
		}

		EG(current_execute_data) = execute_data;
		EX_T(opline->result.u.var).var.ptr->is_ref = 0;
		EX_T(opline->result.u.var).var.ptr->refcount = 1;
		if (!return_value_used) {
			zval_ptr_dtor(&EX_T(opline->result.u.var).var.ptr);
		}
	} else if (EX(function_state).function->type == ZEND_USER_FUNCTION) {
		HashTable *calling_symbol_table;

		EX_T(opline->result.u.var).var.ptr = NULL;
		if (EG(symtable_cache_ptr)>=EG(symtable_cache)) {
			/*printf("Cache hit!  Reusing %x\n", symtable_cache[symtable_cache_ptr]);*/
			EX(function_state).function_symbol_table = *(EG(symtable_cache_ptr)--);
		} else {
			ALLOC_HASHTABLE(EX(function_state).function_symbol_table);
			zend_hash_init(EX(function_state).function_symbol_table, 0, NULL, ZVAL_PTR_DTOR, 0);
			/*printf("Cache miss!  Initialized %x\n", function_state.function_symbol_table);*/
		}
		calling_symbol_table = EG(active_symbol_table);
		EG(active_symbol_table) = EX(function_state).function_symbol_table;
		original_return_value = EG(return_value_ptr_ptr);
		EG(return_value_ptr_ptr) = EX_T(opline->result.u.var).var.ptr_ptr;
		EG(active_op_array) = (zend_op_array *) EX(function_state).function;

		zend_execute(EG(active_op_array) TSRMLS_CC);
		EX_T(opline->result.u.var).var.fcall_returned_reference = EG(active_op_array)->return_reference;

		if (return_value_used && !EX_T(opline->result.u.var).var.ptr) {
			if (!EG(exception)) {
				ALLOC_ZVAL(EX_T(opline->result.u.var).var.ptr);
				INIT_ZVAL(*EX_T(opline->result.u.var).var.ptr);
			}
		} else if (!return_value_used && EX_T(opline->result.u.var).var.ptr) {
			zval_ptr_dtor(&EX_T(opline->result.u.var).var.ptr);
		}

		EG(opline_ptr) = &EX(opline);
		EG(active_op_array) = op_array;
		EG(return_value_ptr_ptr)=original_return_value;
		if (EG(symtable_cache_ptr)>=EG(symtable_cache_limit)) {
			zend_hash_destroy(EX(function_state).function_symbol_table);
			FREE_HASHTABLE(EX(function_state).function_symbol_table);
		} else {
			/* clean before putting into the cache, since clean
			   could call dtors, which could use cached hash */
			zend_hash_clean(EX(function_state).function_symbol_table);
			*(++EG(symtable_cache_ptr)) = EX(function_state).function_symbol_table;
		}
		EG(active_symbol_table) = calling_symbol_table;
	} else { /* ZEND_OVERLOADED_FUNCTION */
		ALLOC_ZVAL(EX_T(opline->result.u.var).var.ptr);
		INIT_ZVAL(*(EX_T(opline->result.u.var).var.ptr));

			/* Not sure what should be done here if it's a static method */
		if (EX(object)) {
			Z_OBJ_HT_P(EX(object))->call_method(EX(fbc)->common.function_name, opline->extended_value, EX_T(opline->result.u.var).var.ptr, EX(object), return_value_used TSRMLS_CC);
		} else {
			zend_error(E_ERROR, "Cannot call overloaded function for non-object");
		}
			
		if (EX(function_state).function->type == ZEND_OVERLOADED_FUNCTION_TEMPORARY) {
			efree(EX(function_state).function->common.function_name);
		}
		efree(EX(fbc));

		if (!return_value_used) {
			zval_ptr_dtor(&EX_T(opline->result.u.var).var.ptr);
		} else {
			EX_T(opline->result.u.var).var.ptr->is_ref = 0;
			EX_T(opline->result.u.var).var.ptr->refcount = 1;
		}
	}

	if (EG(This)) {
		if (EG(exception) && EX(fbc) && EX(fbc)->common.fn_flags&ZEND_ACC_CTOR) {
			EG(This)->refcount--;
			if (EG(This)->refcount == 1) {
				zend_object_store_ctor_failed(EG(This) TSRMLS_CC);
			}
			if (should_change_scope && EG(This) != current_this) {
				zval_ptr_dtor(&EG(This));
			}
		} else if (should_change_scope) {
			zval_ptr_dtor(&EG(This));
		}
	}

	if (should_change_scope) {
		EG(This) = current_this;
		EG(scope) = current_scope;
	}
	zend_ptr_stack_n_pop(&EG(arg_types_stack), 3, &EX(calling_scope), &EX(object), &EX(fbc));
	
	EX(function_state).function = (zend_function *) op_array;
	EG(function_state_ptr) = &EX(function_state);
	zend_ptr_stack_clear_multiple(TSRMLS_C);

	if (EG(exception)) {
		zend_throw_exception_internal(NULL TSRMLS_CC);
		if (return_value_used && EX_T(opline->result.u.var).var.ptr) {
			zval_ptr_dtor(&EX_T(opline->result.u.var).var.ptr);
		}
	}

	NEXT_OPCODE();
}


int zend_do_fcall_by_name_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	EX(function_state).function = EX(fbc);
	return zend_do_fcall_common_helper(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}


int zend_do_fcall_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	zval *fname = get_zval_ptr(&opline->op1, EX(Ts), &EG(free_op1), BP_VAR_R);
	
	zend_ptr_stack_n_push(&EG(arg_types_stack), 3, EX(fbc), EX(object), EX(calling_scope));

	if (zend_hash_find(EG(function_table), fname->value.str.val, fname->value.str.len+1, (void **) &EX(function_state).function)==FAILURE) {
		zend_error(E_ERROR, "Unknown function:  %s()\n", fname->value.str.val);
	}
	EX(object) = NULL;
	EX(calling_scope) = EX(function_state).function->common.scope;

	FREE_OP(EX(Ts), &opline->op1, EG(free_op1));
					
	return zend_do_fcall_common_helper(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}


int zend_return_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	zval *retval_ptr;
	zval **retval_ptr_ptr;
			
	if (EG(active_op_array)->return_reference == ZEND_RETURN_REF) {
		if (opline->op1.op_type == IS_CONST || opline->op1.op_type == IS_TMP_VAR) {
			/* Not supposed to happen, but we'll allow it */
			zend_error(E_STRICT, "Only variable references should be returned by reference");
			goto return_by_value;
		}

		retval_ptr_ptr = get_zval_ptr_ptr(&opline->op1, EX(Ts), BP_VAR_W);

		if (!retval_ptr_ptr) {
			zend_error(E_ERROR, "Cannot return string offsets by reference");
		}

		if (!(*retval_ptr_ptr)->is_ref) {
			if (opline->extended_value == ZEND_RETURNS_FUNCTION &&
			    EX_T(opline->op1.u.var).var.fcall_returned_reference) {
			} else if (EX_T(opline->op1.u.var).var.ptr_ptr == &EX_T(opline->op1.u.var).var.ptr) {
				PZVAL_LOCK(*retval_ptr_ptr); /* undo the effect of get_zval_ptr_ptr() */
				zend_error(E_STRICT, "Only variable references should be returned by reference");
				goto return_by_value;
			}
		}
		
		SEPARATE_ZVAL_TO_MAKE_IS_REF(retval_ptr_ptr);
		(*retval_ptr_ptr)->refcount++;
		
		(*EG(return_value_ptr_ptr)) = (*retval_ptr_ptr);
	} else {
return_by_value:
		retval_ptr = get_zval_ptr(&opline->op1, EX(Ts), &EG(free_op1), BP_VAR_R);
	
		if (EG(ze1_compatibility_mode) && Z_TYPE_P(retval_ptr) == IS_OBJECT) {
			char *class_name;
			zend_uint class_name_len;
			int dup;

			dup = zend_get_object_classname(retval_ptr, &class_name, &class_name_len TSRMLS_CC);

			ALLOC_ZVAL(*(EG(return_value_ptr_ptr)));
			**EG(return_value_ptr_ptr) = *retval_ptr;
			INIT_PZVAL(*EG(return_value_ptr_ptr));
			if (Z_OBJ_HT_P(retval_ptr)->clone_obj == NULL) {
				zend_error(E_ERROR, "Trying to clone an uncloneable object of class %s",  class_name);
			}
			zend_error(E_STRICT, "Implicit cloning object of class '%s' because of 'zend.ze1_compatibility_mode'", class_name);
			(*EG(return_value_ptr_ptr))->value.obj = Z_OBJ_HT_P(retval_ptr)->clone_obj(retval_ptr TSRMLS_CC);

			if(!dup) {
			   efree(class_name);
			}
		} else if (!EG(free_op1)) { /* Not a temp var */
			if (EG(active_op_array)->return_reference == ZEND_RETURN_REF ||
			    (PZVAL_IS_REF(retval_ptr) && retval_ptr->refcount > 0)) {
				ALLOC_ZVAL(*(EG(return_value_ptr_ptr)));
				**EG(return_value_ptr_ptr) = *retval_ptr;
				(*EG(return_value_ptr_ptr))->is_ref = 0;
				(*EG(return_value_ptr_ptr))->refcount = 1;
				zval_copy_ctor(*EG(return_value_ptr_ptr));
			} else {
				*EG(return_value_ptr_ptr) = retval_ptr;
				retval_ptr->refcount++;
			}
		} else {
			ALLOC_ZVAL(*(EG(return_value_ptr_ptr)));
			**EG(return_value_ptr_ptr) = *retval_ptr;
			(*EG(return_value_ptr_ptr))->refcount = 1;
			(*EG(return_value_ptr_ptr))->is_ref = 0;
		}
	}
	RETURN_FROM_EXECUTE_LOOP(execute_data);
}


int zend_throw_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	zval *value;
	zval *exception;

	value = get_zval_ptr(&opline->op1, EX(Ts), &EG(free_op1), BP_VAR_R);

	if (value->type != IS_OBJECT) {
		zend_error(E_ERROR, "Can only throw objects");
	}
	/* Not sure if a complete copy is what we want here */
	MAKE_STD_ZVAL(exception);
	*exception = *value;
	if (!EG(free_op1)) {
		zval_copy_ctor(exception);
	}
	INIT_PZVAL(exception);

	zend_throw_exception_object(exception TSRMLS_CC);
	NEXT_OPCODE();
}


int zend_catch_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_class_entry *ce;

	/* Check whether an exception has been thrown, if not, jump over code */
	if (EG(exception) == NULL) {
		SET_OPCODE(&op_array->opcodes[opline->extended_value]);
		return 0; /* CHECK_ME */
	}
	ce = Z_OBJCE_P(EG(exception));
	if (ce != EX_T(opline->op1.u.var).class_entry) {
		if (!instanceof_function(ce, EX_T(opline->op1.u.var).class_entry TSRMLS_CC)) {
			if (opline->op1.u.EA.type) {
				zend_throw_exception_internal(NULL TSRMLS_CC);
				NEXT_OPCODE();
			}
			SET_OPCODE(&op_array->opcodes[opline->extended_value]);
			return 0; /* CHECK_ME */
		}
	}

	zend_hash_update(EG(active_symbol_table), opline->op2.u.constant.value.str.val,
		opline->op2.u.constant.value.str.len+1, &EG(exception), sizeof(zval *), (void **) NULL);
	EG(exception) = NULL;
	NEXT_OPCODE();
}


int zend_send_val_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	if (opline->extended_value==ZEND_DO_FCALL_BY_NAME
		&& ARG_SHOULD_BE_SENT_BY_REF(EX(fbc), opline->op2.u.opline_num)) {
			zend_error(E_ERROR, "Cannot pass parameter %d by reference", opline->op2.u.opline_num);
	}
	{
		zval *valptr;
		zval *value;

		value = get_zval_ptr(&opline->op1, EX(Ts), &EG(free_op1), BP_VAR_R);

		ALLOC_ZVAL(valptr);
		*valptr = *value;
		if (!EG(free_op1)) {
			zval_copy_ctor(valptr);
		}
		INIT_PZVAL(valptr);
		zend_ptr_stack_push(&EG(argument_stack), valptr);
	}
	NEXT_OPCODE();
}


static inline int zend_send_by_var_helper(ZEND_OPCODE_HANDLER_ARGS)
{
	zval *varptr;
	varptr = get_zval_ptr(&opline->op1, EX(Ts), &EG(free_op1), BP_VAR_R);

	if (varptr == &EG(uninitialized_zval)) {
		ALLOC_ZVAL(varptr);
		INIT_ZVAL(*varptr);
		varptr->refcount = 0;
	} else if (PZVAL_IS_REF(varptr)) {
		zval *original_var = varptr;

		ALLOC_ZVAL(varptr);
		*varptr = *original_var;
		varptr->is_ref = 0;
		varptr->refcount = 0;
		zval_copy_ctor(varptr);
	}
	varptr->refcount++;
	zend_ptr_stack_push(&EG(argument_stack), varptr);
	FREE_OP(EX(Ts), &opline->op1, EG(free_op1));  /* for string offsets */

	NEXT_OPCODE();
}


int zend_send_var_no_ref_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	if (opline->extended_value & ZEND_ARG_COMPILE_TIME_BOUND) { /* Had function_ptr at compile_time */
		if (!(opline->extended_value & ZEND_ARG_SEND_BY_REF)) {
			return zend_send_by_var_helper(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
		}
	} else if (!ARG_SHOULD_BE_SENT_BY_REF(EX(fbc), opline->op2.u.opline_num)) {
		return zend_send_by_var_helper(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
	}
	if ((opline->extended_value & ZEND_ARG_SEND_FUNCTION) &&
	    !EX_T(opline->op1.u.var).var.fcall_returned_reference) {
		zend_error(E_ERROR, "Only variables can be passed by reference");
	} else {
		zval *varptr;
		varptr = get_zval_ptr(&opline->op1, EX(Ts), &EG(free_op1), BP_VAR_R);

		if (varptr != &EG(uninitialized_zval) && (PZVAL_IS_REF(varptr) || varptr->refcount == 1)) {
			varptr->is_ref = 1;
			varptr->refcount++;
			zend_ptr_stack_push(&EG(argument_stack), varptr);
			NEXT_OPCODE();
		}
		zend_error(E_ERROR, "Only variables can be passed by reference");
	}
	NEXT_OPCODE();
}


int zend_send_ref_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	zval **varptr_ptr;
	zval *varptr;
	varptr_ptr = get_zval_ptr_ptr(&opline->op1, EX(Ts), BP_VAR_W);

	if (!varptr_ptr) {
		zend_error(E_ERROR, "Only variables can be passed by reference");
	}

	SEPARATE_ZVAL_TO_MAKE_IS_REF(varptr_ptr);
	varptr = *varptr_ptr;
	varptr->refcount++;
	zend_ptr_stack_push(&EG(argument_stack), varptr);

	NEXT_OPCODE();
}


int zend_send_var_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	if ((opline->extended_value == ZEND_DO_FCALL_BY_NAME)
		&& ARG_SHOULD_BE_SENT_BY_REF(EX(fbc), opline->op2.u.opline_num)) {
		return zend_send_ref_handler(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
	}
	return zend_send_by_var_helper(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}


int zend_recv_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	zval **param;
	zend_uint arg_num = opline->op1.u.constant.value.lval;

	if (zend_ptr_stack_get_arg(arg_num, (void **) &param TSRMLS_CC)==FAILURE) {
		char *space;
		char *class_name = get_active_class_name(&space TSRMLS_CC);
		zend_verify_arg_type((zend_function *) EG(active_op_array), arg_num, NULL TSRMLS_CC);
		zend_error(E_WARNING, "Missing argument %ld for %s%s%s()", opline->op1.u.constant.value.lval, class_name, space, get_active_function_name(TSRMLS_C));
		if (opline->result.op_type == IS_VAR) {
			PZVAL_UNLOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
		}
	} else {
		zend_verify_arg_type((zend_function *) EG(active_op_array), arg_num, *param TSRMLS_CC);
		if (PZVAL_IS_REF(*param)) {
			zend_assign_to_variable_reference(NULL, get_zval_ptr_ptr(&opline->result, EX(Ts), BP_VAR_W), param, NULL TSRMLS_CC);
		} else {
			zend_assign_to_variable(NULL, &opline->result, NULL, *param, IS_VAR, EX(Ts) TSRMLS_CC);
		}
	}

	NEXT_OPCODE();
}


int zend_recv_init_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	zval **param, *assignment_value;
	zend_uint arg_num = opline->op1.u.constant.value.lval;

	if (zend_ptr_stack_get_arg(arg_num, (void **) &param TSRMLS_CC)==FAILURE) {
		if (opline->op2.u.constant.type == IS_CONSTANT || opline->op2.u.constant.type==IS_CONSTANT_ARRAY) {
			zval *default_value;

			ALLOC_ZVAL(default_value);
			*default_value = opline->op2.u.constant;
			if (opline->op2.u.constant.type==IS_CONSTANT_ARRAY) {
				zval_copy_ctor(default_value);
			}
			default_value->refcount=1;
			zval_update_constant(&default_value, 0 TSRMLS_CC);
			default_value->refcount=0;
			default_value->is_ref=0;
			param = &default_value;
			assignment_value = default_value;
		} else {
			param = NULL;
			assignment_value = &opline->op2.u.constant;
		}
		zend_verify_arg_type((zend_function *) EG(active_op_array), arg_num, assignment_value TSRMLS_CC);
		zend_assign_to_variable(NULL, &opline->result, NULL, assignment_value, IS_VAR, EX(Ts) TSRMLS_CC);
	} else {
		assignment_value = *param;
		zend_verify_arg_type((zend_function *) EG(active_op_array), arg_num, assignment_value TSRMLS_CC);
		if (PZVAL_IS_REF(assignment_value)) {
			zend_assign_to_variable_reference(NULL, get_zval_ptr_ptr(&opline->result, EX(Ts), BP_VAR_W), param, NULL TSRMLS_CC);
		} else {
			zend_assign_to_variable(NULL, &opline->result, NULL, assignment_value, IS_VAR, EX(Ts) TSRMLS_CC);
		}
	}

	NEXT_OPCODE();
}


int zend_bool_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	/* PHP 3.0 returned "" for false and 1 for true, here we use 0 and 1 for now */
	EX_T(opline->result.u.var).tmp_var.value.lval = zend_is_true(get_zval_ptr(&opline->op1, EX(Ts), &EG(free_op1), BP_VAR_R));
	EX_T(opline->result.u.var).tmp_var.type = IS_BOOL;
	FREE_OP(EX(Ts), &opline->op1, EG(free_op1));

	NEXT_OPCODE();
}


static inline int zend_brk_cont_helper(ZEND_OPCODE_HANDLER_ARGS)
{
	zval *nest_levels_zval = get_zval_ptr(&opline->op2, EX(Ts), &EG(free_op2), BP_VAR_R);
	zval tmp;
	int array_offset, nest_levels, original_nest_levels;
	zend_brk_cont_element *jmp_to;

	if (nest_levels_zval->type != IS_LONG) {
		tmp = *nest_levels_zval;
		zval_copy_ctor(&tmp);
		convert_to_long(&tmp);
		nest_levels = tmp.value.lval;
	} else {
		nest_levels = nest_levels_zval->value.lval;
	}
	original_nest_levels = nest_levels;
	array_offset = opline->op1.u.opline_num;
	do {
		if (array_offset==-1) {
			zend_error(E_ERROR, "Cannot break/continue %d level%s", original_nest_levels, (original_nest_levels == 1) ? "" : "s");
		}
		jmp_to = &op_array->brk_cont_array[array_offset];
		if (nest_levels>1) {
			zend_op *brk_opline = &op_array->opcodes[jmp_to->brk];

			switch (brk_opline->opcode) {
				case ZEND_SWITCH_FREE:
					zend_switch_free(brk_opline, EX(Ts) TSRMLS_CC);
					break;
				case ZEND_FREE:
					zendi_zval_dtor(EX_T(brk_opline->op1.u.var).tmp_var);
					break;
			}
		}
		array_offset = jmp_to->parent;
	} while (--nest_levels > 0);

	if (opline->opcode == ZEND_BRK) {
		SET_OPCODE(op_array->opcodes+jmp_to->brk);
	} else {
		SET_OPCODE(op_array->opcodes+jmp_to->cont);
	}
	FREE_OP(EX(Ts), &opline->op2, EG(free_op2));
	return 0; /* CHECK_ME */
}


int zend_brk_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_brk_cont_helper(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}


int zend_cont_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_brk_cont_helper(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}


int zend_case_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	int switch_expr_is_overloaded=0;

	if (opline->op1.op_type==IS_VAR) {
		if (EX_T(opline->op1.u.var).var.ptr_ptr) {
			PZVAL_LOCK(EX_T(opline->op1.u.var).var.ptr);
		} else {
			switch_expr_is_overloaded = 1;
			EX_T(opline->op1.u.var).str_offset.str->refcount++;
		}
	}
	is_equal_function(&EX_T(opline->result.u.var).tmp_var, 
				 get_zval_ptr(&opline->op1, EX(Ts), &EG(free_op1), BP_VAR_R),
				 get_zval_ptr(&opline->op2, EX(Ts), &EG(free_op2), BP_VAR_R) TSRMLS_CC);

	FREE_OP(EX(Ts), &opline->op2, EG(free_op2));
	if (switch_expr_is_overloaded) {
		/* We only free op1 if this is a string offset,
		 * Since if it is a TMP_VAR, it'll be reused by
		 * other CASE opcodes (whereas string offsets
		 * are allocated at each get_zval_ptr())
		 */
		FREE_OP(EX(Ts), &opline->op1, EG(free_op1));
		EX_T(opline->op1.u.var).var.ptr_ptr = NULL;
		AI_USE_PTR(EX_T(opline->op1.u.var).var);
	}
	NEXT_OPCODE();
}


int zend_switch_free_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_switch_free(opline, EX(Ts) TSRMLS_CC);
	NEXT_OPCODE();
}


int zend_new_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	if (EX_T(opline->op1.u.var).class_entry->ce_flags & (ZEND_ACC_INTERFACE|ZEND_ACC_IMPLICIT_ABSTRACT_CLASS|ZEND_ACC_EXPLICIT_ABSTRACT_CLASS)) {
		char *class_type;

		if (EX_T(opline->op1.u.var).class_entry->ce_flags & ZEND_ACC_INTERFACE) {
			class_type = "interface";
		} else {
			class_type = "abstract class";
		}
		zend_error(E_ERROR, "Cannot instantiate %s %s", class_type,  EX_T(opline->op1.u.var).class_entry->name);
	}
	EX_T(opline->result.u.var).var.ptr_ptr = &EX_T(opline->result.u.var).var.ptr;
	ALLOC_ZVAL(EX_T(opline->result.u.var).var.ptr);
	object_init_ex(EX_T(opline->result.u.var).var.ptr, EX_T(opline->op1.u.var).class_entry);
	EX_T(opline->result.u.var).var.ptr->refcount=1;
	EX_T(opline->result.u.var).var.ptr->is_ref=0;
	
	NEXT_OPCODE();
}


int zend_clone_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	zval *obj = get_obj_zval_ptr(&opline->op1, EX(Ts), &EG(free_op1), BP_VAR_R TSRMLS_CC);
	zend_class_entry *ce;
	zend_function *clone;
	zend_object_clone_obj_t clone_call;

	if (!obj || Z_TYPE_P(obj) != IS_OBJECT) {
		zend_error(E_WARNING, "__clone method called on non-object");
		EX_T(opline->result.u.var).var.ptr = EG(error_zval_ptr);
		EX_T(opline->result.u.var).var.ptr->refcount++;
		NEXT_OPCODE();
	} 

	ce = (Z_OBJ_HT_P(obj)->get_class_entry) ? Z_OBJCE_P(obj) : NULL;
	clone = ce ? ce->clone : NULL;
	clone_call =  Z_OBJ_HT_P(obj)->clone_obj;
	if (!clone_call) {
		zend_error(E_ERROR, "Trying to clone an uncloneable object of class %s", ce->name);
		EX_T(opline->result.u.var).var.ptr = EG(error_zval_ptr);
		EX_T(opline->result.u.var).var.ptr->refcount++;
	}

	if (ce && clone) {
		if (clone->op_array.fn_flags & ZEND_ACC_PRIVATE) {
			/* Ensure that if we're calling a private function, we're allowed to do so.
			 */
			if (ce != EG(scope)) {
				zend_error(E_ERROR, "Call to private %s::__clone() from context '%s'", ce->name, EG(scope) ? EG(scope)->name : "");
			}
		} else if ((clone->common.fn_flags & ZEND_ACC_PROTECTED)) {
			/* Ensure that if we're calling a protected function, we're allowed to do so.
			 */
			if (!zend_check_protected(clone->common.scope, EG(scope))) {
				zend_error(E_ERROR, "Call to protected %s::__clone() from context '%s'", ce->name, EG(scope) ? EG(scope)->name : "");
			}
		}
	}

	EX_T(opline->result.u.var).var.ptr_ptr = &EX_T(opline->result.u.var).var.ptr;
	ALLOC_ZVAL(EX_T(opline->result.u.var).var.ptr);
	EX_T(opline->result.u.var).var.ptr->value.obj = clone_call(obj TSRMLS_CC);
	if (EG(exception)) {
		FREE_ZVAL(EX_T(opline->result.u.var).var.ptr);
	} else {
		EX_T(opline->result.u.var).var.ptr->type = IS_OBJECT;
		EX_T(opline->result.u.var).var.ptr->refcount=1;
		EX_T(opline->result.u.var).var.ptr->is_ref=1;
	}
	NEXT_OPCODE();
}


int zend_fetch_constant_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_class_entry *ce = NULL;
	zval **value;

	if (opline->op1.op_type == IS_UNUSED) {
/* This seems to be a reminant of namespaces
		if (EG(scope)) {
			ce = EG(scope);
			if (zend_hash_find(&ce->constants_table, opline->op2.u.constant.value.str.val, opline->op2.u.constant.value.str.len+1, (void **) &value) == SUCCESS) {
				zval_update_constant(value, (void *) 1 TSRMLS_CC);
				EX_T(opline->result.u.var).tmp_var = **value;
				zval_copy_ctor(&EX_T(opline->result.u.var).tmp_var);
				NEXT_OPCODE();
			}
		}
*/
		if (!zend_get_constant(opline->op2.u.constant.value.str.val, opline->op2.u.constant.value.str.len, &EX_T(opline->result.u.var).tmp_var TSRMLS_CC)) {
			zend_error(E_NOTICE, "Use of undefined constant %s - assumed '%s'",
						opline->op2.u.constant.value.str.val,
						opline->op2.u.constant.value.str.val);
			EX_T(opline->result.u.var).tmp_var = opline->op2.u.constant;
			zval_copy_ctor(&EX_T(opline->result.u.var).tmp_var);
		}
		NEXT_OPCODE();
	}
	
	ce = EX_T(opline->op1.u.var).class_entry;
	
	if (zend_hash_find(&ce->constants_table, opline->op2.u.constant.value.str.val, opline->op2.u.constant.value.str.len+1, (void **) &value) == SUCCESS) {
		zval_update_constant(value, (void *) 1 TSRMLS_CC);
		EX_T(opline->result.u.var).tmp_var = **value;
		zval_copy_ctor(&EX_T(opline->result.u.var).tmp_var);
	} else {
		zend_error(E_ERROR, "Undefined class constant '%s'", opline->op2.u.constant.value.str.val);
	}

	NEXT_OPCODE();
}


static inline int zend_init_add_array_helper(ZEND_OPCODE_HANDLER_ARGS)
{
	zval *array_ptr = &EX_T(opline->result.u.var).tmp_var;
	zval *expr_ptr, **expr_ptr_ptr = NULL;
	zval *offset=get_zval_ptr(&opline->op2, EX(Ts), &EG(free_op2), BP_VAR_R);

	if (opline->extended_value) {
		expr_ptr_ptr=get_zval_ptr_ptr(&opline->op1, EX(Ts), BP_VAR_R);
		expr_ptr = *expr_ptr_ptr;
	} else {
		expr_ptr=get_zval_ptr(&opline->op1, EX(Ts), &EG(free_op1), BP_VAR_R);
	}
	
	if (opline->opcode == ZEND_INIT_ARRAY) {
		array_init(array_ptr);
		if (!expr_ptr) {
			NEXT_OPCODE();
		}
	}
	if (!opline->extended_value && EG(free_op1)) { /* temporary variable */
		zval *new_expr;

		ALLOC_ZVAL(new_expr);
		*new_expr = *expr_ptr;
		expr_ptr = new_expr;
		INIT_PZVAL(expr_ptr);
	} else {
		if (opline->extended_value) {
			SEPARATE_ZVAL_TO_MAKE_IS_REF(expr_ptr_ptr);
			expr_ptr = *expr_ptr_ptr;
			expr_ptr->refcount++;
		} else if (PZVAL_IS_REF(expr_ptr)) {
			zval *new_expr;

			ALLOC_ZVAL(new_expr);
			*new_expr = *expr_ptr;
			expr_ptr = new_expr;
			zendi_zval_copy_ctor(*expr_ptr);
			INIT_PZVAL(expr_ptr);
		} else {
			expr_ptr->refcount++;
		}
	}
	if (offset) {
		switch (offset->type) {
			case IS_DOUBLE:
				zend_hash_index_update(array_ptr->value.ht, (long) offset->value.dval, &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_LONG:
			case IS_BOOL:
				zend_hash_index_update(array_ptr->value.ht, offset->value.lval, &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_STRING:
				zend_symtable_update(array_ptr->value.ht, offset->value.str.val, offset->value.str.len+1, &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_NULL:
				zend_hash_update(array_ptr->value.ht, "", sizeof(""), &expr_ptr, sizeof(zval *), NULL);
				break;
			default:
				zend_error(E_WARNING, "Illegal offset type");
				zval_ptr_dtor(&expr_ptr);
				/* do nothing */
				break;
		}
		FREE_OP(EX(Ts), &opline->op2, EG(free_op2));
	} else {
		zend_hash_next_index_insert(array_ptr->value.ht, &expr_ptr, sizeof(zval *), NULL);
	}
	NEXT_OPCODE();
}


int zend_init_array_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_init_add_array_helper(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}


int zend_add_array_element_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_init_add_array_helper(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}


int zend_cast_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	zval *expr = get_zval_ptr(&opline->op1, EX(Ts), &EG(free_op1), BP_VAR_R);
	zval *result = &EX_T(opline->result.u.var).tmp_var;

	*result = *expr;
	if (!EG(free_op1)) {
		zendi_zval_copy_ctor(*result);
	}					
	switch (opline->extended_value) {
		case IS_NULL:
			convert_to_null(result);
			break;
		case IS_BOOL:
			convert_to_boolean(result);
			break;
		case IS_LONG:
			convert_to_long(result);
			break;
		case IS_DOUBLE:
			convert_to_double(result);
			break;
		case IS_STRING: {
			zval var_copy;
			int use_copy;
		
			zend_make_printable_zval(result, &var_copy, &use_copy);
			if (use_copy) {
				zval_dtor(result);
				*result = var_copy;
			}
			break;
		}
		case IS_ARRAY:
			convert_to_array(result);
			break;
		case IS_OBJECT:
			convert_to_object(result);
			break;
	}
	NEXT_OPCODE();
}


int zend_include_or_eval_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op_array *new_op_array=NULL;
	zval **original_return_value = EG(return_value_ptr_ptr);
	int return_value_used;
	zval *inc_filename = get_zval_ptr(&opline->op1, EX(Ts), &EG(free_op1), BP_VAR_R);
	zval tmp_inc_filename;
	zend_bool failure_retval=0;

	if (inc_filename->type!=IS_STRING) {
		tmp_inc_filename = *inc_filename;
		zval_copy_ctor(&tmp_inc_filename);
		convert_to_string(&tmp_inc_filename);
		inc_filename = &tmp_inc_filename;
	}
	
	return_value_used = RETURN_VALUE_USED(opline);

	switch (opline->op2.u.constant.value.lval) {
		case ZEND_INCLUDE_ONCE:
		case ZEND_REQUIRE_ONCE: {
				int dummy = 1;
				zend_file_handle file_handle;

				if (SUCCESS == zend_stream_open(inc_filename->value.str.val, &file_handle TSRMLS_CC)) {

					if (!file_handle.opened_path) {
						file_handle.opened_path = estrndup(inc_filename->value.str.val, inc_filename->value.str.len);
					}	
				
					if (zend_hash_add(&EG(included_files), file_handle.opened_path, strlen(file_handle.opened_path)+1, (void *)&dummy, sizeof(int), NULL)==SUCCESS) {
						new_op_array = zend_compile_file(&file_handle, (opline->op2.u.constant.value.lval==ZEND_INCLUDE_ONCE?ZEND_INCLUDE:ZEND_REQUIRE) TSRMLS_CC);
						zend_destroy_file_handle(&file_handle TSRMLS_CC);
					} else {
						zend_file_handle_dtor(&file_handle);
						failure_retval=1;
					}
				} else {
					if (opline->op2.u.constant.value.lval==ZEND_INCLUDE_ONCE) {
						zend_message_dispatcher(ZMSG_FAILED_INCLUDE_FOPEN, inc_filename->value.str.val);
					} else {
						zend_message_dispatcher(ZMSG_FAILED_REQUIRE_FOPEN, inc_filename->value.str.val);
					}
				}
				break;
			}
			break;
		case ZEND_INCLUDE:
		case ZEND_REQUIRE:
			new_op_array = compile_filename(opline->op2.u.constant.value.lval, inc_filename TSRMLS_CC);
			break;
		case ZEND_EVAL: {
				char *eval_desc = zend_make_compiled_string_description("eval()'d code" TSRMLS_CC);

				new_op_array = compile_string(inc_filename, eval_desc TSRMLS_CC);
				efree(eval_desc);
			}
			break;
		EMPTY_SWITCH_DEFAULT_CASE()
	}
	if (inc_filename==&tmp_inc_filename) {
		zval_dtor(&tmp_inc_filename);
	}
	FREE_OP(EX(Ts), &opline->op1, EG(free_op1));
	EX_T(opline->result.u.var).var.ptr_ptr = &EX_T(opline->result.u.var).var.ptr;
	if (new_op_array) {
		zval *saved_object;
		zend_function *saved_function;

		EG(return_value_ptr_ptr) = EX_T(opline->result.u.var).var.ptr_ptr;
		EG(active_op_array) = new_op_array;
		EX_T(opline->result.u.var).var.ptr = NULL;

		saved_object = EX(object);
		saved_function = EX(function_state).function;

		EX(function_state).function = (zend_function *) new_op_array;
		EX(object) = NULL;
		
		zend_execute(new_op_array TSRMLS_CC);
		
		EX(function_state).function = saved_function;
		EX(object) = saved_object;
		
		if (!return_value_used) {
			if (EX_T(opline->result.u.var).var.ptr) {
				zval_ptr_dtor(&EX_T(opline->result.u.var).var.ptr);
			} 
		} else { /* return value is used */
			if (!EX_T(opline->result.u.var).var.ptr) { /* there was no return statement */
				ALLOC_ZVAL(EX_T(opline->result.u.var).var.ptr);
				INIT_PZVAL(EX_T(opline->result.u.var).var.ptr);
				EX_T(opline->result.u.var).var.ptr->value.lval = 1;
				EX_T(opline->result.u.var).var.ptr->type = IS_BOOL;
			}
		}

		EG(opline_ptr) = &EX(opline);
		EG(active_op_array) = op_array;
		EG(function_state_ptr) = &EX(function_state);
		destroy_op_array(new_op_array TSRMLS_CC);
		efree(new_op_array);
		if (EG(exception)) {
			zend_throw_exception_internal(NULL TSRMLS_CC);
		}
	} else {
		if (return_value_used) {
			ALLOC_ZVAL(EX_T(opline->result.u.var).var.ptr);
			INIT_ZVAL(*EX_T(opline->result.u.var).var.ptr);
			EX_T(opline->result.u.var).var.ptr->value.lval = failure_retval;
			EX_T(opline->result.u.var).var.ptr->type = IS_BOOL;
		}
	}
	EG(return_value_ptr_ptr) = original_return_value;
	NEXT_OPCODE();
}


int zend_unset_var_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	zval tmp, *varname;
	HashTable *target_symbol_table;

	varname = get_zval_ptr(&opline->op1, EX(Ts), &EG(free_op1), BP_VAR_R);

	if (varname->type != IS_STRING) {
		tmp = *varname;
		zval_copy_ctor(&tmp);
		convert_to_string(&tmp);
		varname = &tmp;
	}

	if (opline->op2.u.EA.type == ZEND_FETCH_STATIC_MEMBER) {
		zend_std_unset_static_property(EX_T(opline->op2.u.var).class_entry, Z_STRVAL_P(varname), Z_STRLEN_P(varname) TSRMLS_CC);
	} else {
		target_symbol_table = zend_get_target_symbol_table(opline, EX(Ts), BP_VAR_IS, varname TSRMLS_CC);
		zend_hash_del(target_symbol_table, varname->value.str.val, varname->value.str.len+1);
	}

	if (varname == &tmp) {
		zval_dtor(&tmp);
	}
	FREE_OP(EX(Ts), &opline->op1, EG(free_op1));
	NEXT_OPCODE();
}



int zend_unset_dim_obj_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	zval **container = get_obj_zval_ptr_ptr(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC);
	zval *offset = get_zval_ptr(&opline->op2, EX(Ts), &EG(free_op2), BP_VAR_R);
	long index;
	
	if (container) {
		if (opline->extended_value == ZEND_UNSET_DIM) {
			switch (Z_TYPE_PP(container)) {
				case IS_ARRAY: {
					HashTable *ht = Z_ARRVAL_PP(container);
					switch (offset->type) {
						case IS_DOUBLE:
						case IS_RESOURCE:
						case IS_BOOL: 
						case IS_LONG:
							if (offset->type == IS_DOUBLE) {
								index = (long) offset->value.dval;
							} else {
								index = offset->value.lval;
							}
	
							zend_hash_index_del(ht, index);
							break;
						case IS_STRING:
							zend_symtable_del(ht, offset->value.str.val, offset->value.str.len+1);
							break;
						case IS_NULL:
							zend_hash_del(ht, "", sizeof(""));
							break;
						default: 
							zend_error(E_WARNING, "Illegal offset type in unset");
							break;
					}
					FREE_OP(EX(Ts), &opline->op2, EG(free_op2));
					break;
				}
				case IS_OBJECT:
					if (!Z_OBJ_HT_P(*container)->unset_dimension) {
						zend_error(E_ERROR, "Cannot use object as array");
					}
					if (EG(free_op2)) {
						MAKE_REAL_ZVAL_PTR(offset);
					}
					Z_OBJ_HT_P(*container)->unset_dimension(*container, offset TSRMLS_CC);
					if (EG(free_op2)) {
						zval_ptr_dtor(&offset);
					}
					break;
				case IS_STRING:
					zend_error(E_ERROR, "Cannot unset string offsets");
					return 0; /* bailed out before */
				default:
					FREE_OP(EX(Ts), &opline->op2, EG(free_op2));
					break;
			}
		} else { /* ZEND_UNSET_OBJ */
			if (Z_TYPE_PP(container) == IS_OBJECT) {
				if (EG(free_op2)) {
					MAKE_REAL_ZVAL_PTR(offset);
				}
				Z_OBJ_HT_P(*container)->unset_property(*container, offset TSRMLS_CC);
				if (EG(free_op2)) {
					zval_ptr_dtor(&offset);
				}
			}
		}
	} else {
		/* overloaded element */
		FREE_OP(EX(Ts), &opline->op2, EG(free_op2));
	}

	NEXT_OPCODE();
}


int zend_fe_reset_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	zval *array_ptr, **array_ptr_ptr;
	HashTable *fe_ht;
	zend_object_iterator *iter = NULL;
	zend_class_entry *ce = NULL;
	
	if (opline->extended_value) {
		array_ptr_ptr = get_zval_ptr_ptr(&opline->op1, EX(Ts), BP_VAR_R);
		if (array_ptr_ptr == NULL) {
			ALLOC_INIT_ZVAL(array_ptr);
		} else if (Z_TYPE_PP(array_ptr_ptr) == IS_OBJECT) {
			if(Z_OBJ_HT_PP(array_ptr_ptr)->get_class_entry == NULL) {
				zend_error(E_WARNING, "foreach() can not iterate over objects without PHP class");
				opline++;
				SET_OPCODE(op_array->opcodes+opline->op2.u.opline_num);
				return 0;	
			}
			ce = Z_OBJCE_PP(array_ptr_ptr);
			if (!ce || ce->get_iterator == NULL) {
				SEPARATE_ZVAL_IF_NOT_REF(array_ptr_ptr);
				(*array_ptr_ptr)->refcount++;
			}
			array_ptr = *array_ptr_ptr;
		} else {
			SEPARATE_ZVAL_IF_NOT_REF(array_ptr_ptr);
			array_ptr = *array_ptr_ptr;
			array_ptr->refcount++;
		}
	} else {
		array_ptr = get_zval_ptr(&opline->op1, EX(Ts), &EG(free_op1), BP_VAR_R);
		if (EG(free_op1)) { /* IS_TMP_VAR */
			zval *tmp;

			ALLOC_ZVAL(tmp);
			*tmp = *array_ptr;
			INIT_PZVAL(tmp);
			array_ptr = tmp;
		} else if (Z_TYPE_P(array_ptr) == IS_OBJECT && Z_OBJ_HT_P(array_ptr)->get_class_entry) {
			ce = Z_OBJCE_P(array_ptr);
		} else {
			array_ptr->refcount++;
		}
	}

	if (ce && ce->get_iterator) {
		iter = ce->get_iterator(ce, array_ptr TSRMLS_CC);

		if (iter && !EG(exception)) {
			array_ptr = zend_iterator_wrap(iter TSRMLS_CC);
		} else {
			FREE_OP(Ts, op1, EG(free_op1));
			if (!EG(exception)) {
				zend_throw_exception_ex(NULL, 0 TSRMLS_CC, "Object of type %s did not create an Iterator", ce->name);
			}
			zend_throw_exception_internal(NULL TSRMLS_CC);
			NEXT_OPCODE();
			return 0;
		}
	}
	
	PZVAL_LOCK(array_ptr);
	EX_T(opline->result.u.var).var.ptr = array_ptr;
	EX_T(opline->result.u.var).var.ptr_ptr = &EX_T(opline->result.u.var).var.ptr;	

	if (iter) {
		iter->index = 0;
		if (iter->funcs->rewind) {
			iter->funcs->rewind(iter TSRMLS_CC);
			if (EG(exception)) {
				array_ptr->refcount--;
				zval_ptr_dtor(&array_ptr);
				NEXT_OPCODE();
			}
		}
	} else if ((fe_ht = HASH_OF(array_ptr)) != NULL) {
		/* probably redundant */
		zend_hash_internal_pointer_reset(fe_ht);
	} else {
		zend_error(E_WARNING, "Invalid argument supplied for foreach()");
		
		opline++;
		SET_OPCODE(op_array->opcodes+opline->op2.u.opline_num);
		return 0;
	}

	NEXT_OPCODE();
}


int zend_fe_fetch_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	zval *array = get_zval_ptr(&opline->op1, EX(Ts), &EG(free_op1), BP_VAR_R);
	zval *result = &EX_T(opline->result.u.var).tmp_var;
	zval **value, *key;
	char *str_key;
	uint str_key_len;
	ulong int_key;
	HashTable *fe_ht;
	zend_object_iterator *iter = NULL;
	int key_type;

	PZVAL_LOCK(array);

	switch (zend_iterator_unwrap(array, &iter TSRMLS_CC)) {
		default:
		case ZEND_ITER_INVALID:
			zend_error(E_WARNING, "Invalid argument supplied for foreach()");
			SET_OPCODE(op_array->opcodes+opline->op2.u.opline_num);
			return 0; /* CHECK_ME */
			
		case ZEND_ITER_PLAIN_OBJECT: {
			char *class_name, *prop_name;
			zend_object *zobj = zend_objects_get_address(array TSRMLS_CC);

			fe_ht = HASH_OF(array);
			do {
				if (zend_hash_get_current_data(fe_ht, (void **) &value)==FAILURE) {
					/* reached end of iteration */
					SET_OPCODE(op_array->opcodes+opline->op2.u.opline_num);
					return 0; /* CHECK_ME */
				}
				key_type = zend_hash_get_current_key_ex(fe_ht, &str_key, &str_key_len, &int_key, 0, NULL);

				zend_hash_move_forward(fe_ht);
			} while (key_type != HASH_KEY_IS_STRING || zend_check_property_access(zobj, str_key TSRMLS_CC) != SUCCESS);
			zend_unmangle_property_name(str_key, &class_name, &prop_name);
			str_key_len = strlen(prop_name);
			str_key = estrndup(prop_name, str_key_len);
			str_key_len++;
			break;
		}

		case ZEND_ITER_PLAIN_ARRAY:
			fe_ht = HASH_OF(array);
			if (zend_hash_get_current_data(fe_ht, (void **) &value)==FAILURE) {
				/* reached end of iteration */
				SET_OPCODE(op_array->opcodes+opline->op2.u.opline_num);
				return 0; /* CHECK_ME */
			}
			key_type = zend_hash_get_current_key_ex(fe_ht, &str_key, &str_key_len, &int_key, 1, NULL);
			zend_hash_move_forward(fe_ht);
			break;

		case ZEND_ITER_OBJECT:
			/* !iter happens from exception */
			if (iter && iter->index++) {
				/* This could cause an endless loop if index becomes zero again.
				 * In case that ever happens we need an additional flag. */
				iter->funcs->move_forward(iter TSRMLS_CC);
				if (EG(exception)) {
					array->refcount--;
					zval_ptr_dtor(&array);
					NEXT_OPCODE();
				}
			}
			if (!iter || iter->funcs->valid(iter TSRMLS_CC) == FAILURE) {
				/* reached end of iteration */
				if (EG(exception)) {
					array->refcount--;
					zval_ptr_dtor(&array);
					NEXT_OPCODE();
				}
				SET_OPCODE(op_array->opcodes+opline->op2.u.opline_num);
				return 0; /* CHECK_ME */
			}	
			iter->funcs->get_current_data(iter, &value TSRMLS_CC);
			if (EG(exception)) {
				array->refcount--;
				zval_ptr_dtor(&array);
				NEXT_OPCODE();
			}
			if (!value) {
				/* failure in get_current_data */
				SET_OPCODE(op_array->opcodes+opline->op2.u.opline_num);
				return 0; /* CHECK_ME */
			}
			if (iter->funcs->get_current_key) {
				key_type = iter->funcs->get_current_key(iter, &str_key, &str_key_len, &int_key TSRMLS_CC);
				if (EG(exception)) {
					array->refcount--;
					zval_ptr_dtor(&array);
					NEXT_OPCODE();
				}
			} else {
				key_type = HASH_KEY_IS_LONG;
				int_key = iter->index;
			}

			break;
	}

	array_init(result);

	if (opline->extended_value) {
		SEPARATE_ZVAL_IF_NOT_REF(value);
		(*value)->is_ref = 1;
	}
	(*value)->refcount++;
	zend_hash_index_update(result->value.ht, 0, value, sizeof(zval *), NULL);

	ALLOC_ZVAL(key);
	INIT_PZVAL(key);

	switch (key_type) {
		case HASH_KEY_IS_STRING:
			key->value.str.val = str_key;
			key->value.str.len = str_key_len-1;
			key->type = IS_STRING;
			break;
		case HASH_KEY_IS_LONG:
			key->value.lval = int_key;
			key->type = IS_LONG;
			break;
		EMPTY_SWITCH_DEFAULT_CASE()
	}
	zend_hash_index_update(result->value.ht, 1, &key, sizeof(zval *), NULL);

	NEXT_OPCODE();
}


int zend_jmp_no_ctor_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	zval *object_zval;
	zend_function *constructor;

	if (opline->op1.op_type == IS_VAR) {
		PZVAL_LOCK(*EX_T(opline->op1.u.var).var.ptr_ptr);
	}
	
	object_zval = get_zval_ptr(&opline->op1, EX(Ts), &EG(free_op1), BP_VAR_R);
	constructor = Z_OBJ_HT_P(object_zval)->get_constructor(object_zval TSRMLS_CC);

	EX(fbc_constructor) = NULL;
	if (constructor == NULL) {
		if(opline->op1.u.EA.type & EXT_TYPE_UNUSED) {
			zval_ptr_dtor(EX_T(opline->op1.u.var).var.ptr_ptr);
		}
		SET_OPCODE(op_array->opcodes + opline->op2.u.opline_num);
		return 0; /* CHECK_ME */
	} else {
		EX(fbc_constructor) = constructor;
	}

	NEXT_OPCODE();	
}


int zend_isset_isempty_var_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	zval tmp, *varname = get_zval_ptr(&opline->op1, EX(Ts), &EG(free_op1), BP_VAR_R);
	zval **value;
	zend_bool isset = 1;
	HashTable *target_symbol_table;

	if (varname->type != IS_STRING) {
		tmp = *varname;
		zval_copy_ctor(&tmp);
		convert_to_string(&tmp);
		varname = &tmp;
	}
	
	if (opline->op2.u.EA.type == ZEND_FETCH_STATIC_MEMBER) {
		value = zend_std_get_static_property(EX_T(opline->op2.u.var).class_entry, Z_STRVAL_P(varname), Z_STRLEN_P(varname), 1 TSRMLS_CC);
		if (!value) {
			isset = 0;
		}
	} else {
		target_symbol_table = zend_get_target_symbol_table(opline, EX(Ts), BP_VAR_IS, varname TSRMLS_CC);
		if (zend_hash_find(target_symbol_table, varname->value.str.val, varname->value.str.len+1, (void **) &value) == FAILURE) {
			isset = 0;
		}
	}
	
	EX_T(opline->result.u.var).tmp_var.type = IS_BOOL;

	switch (opline->extended_value) {
		case ZEND_ISSET:
			if (isset && Z_TYPE_PP(value) == IS_NULL) {
				EX_T(opline->result.u.var).tmp_var.value.lval = 0;
			} else {
				EX_T(opline->result.u.var).tmp_var.value.lval = isset;
			}
			break;
		case ZEND_ISEMPTY:
			if (!isset || !zend_is_true(*value)) {
				EX_T(opline->result.u.var).tmp_var.value.lval = 1;
			} else {
				EX_T(opline->result.u.var).tmp_var.value.lval = 0;
			}
			break;
	}

	if (varname == &tmp) {
		zval_dtor(&tmp);
	}
	FREE_OP(EX(Ts), &opline->op1, EG(free_op1));

	NEXT_OPCODE();
}


static int zend_isset_isempty_dim_prop_obj_handler(int prop_dim, ZEND_OPCODE_HANDLER_ARGS)
{
	zval **container = get_obj_zval_ptr_ptr(&opline->op1, EX(Ts), BP_VAR_R TSRMLS_CC);
	zval **value = NULL;
	int result = 0;
	long index;

	if (container) {
		zval *offset = get_zval_ptr(&opline->op2, EX(Ts), &EG(free_op2), BP_VAR_R);

		if ((*container)->type == IS_ARRAY) {
			HashTable *ht;
			int isset = 0;

			ht = (*container)->value.ht;

			switch (offset->type) {
				case IS_DOUBLE:
				case IS_RESOURCE:
				case IS_BOOL: 
				case IS_LONG:
					if (offset->type == IS_DOUBLE) {
						index = (long) offset->value.dval;
					} else {
						index = offset->value.lval;
					}
					if (zend_hash_index_find(ht, index, (void **) &value) == SUCCESS) {
						isset = 1;
					}
					break;
				case IS_STRING:
					if (zend_symtable_find(ht, offset->value.str.val, offset->value.str.len+1, (void **) &value) == SUCCESS) {
						isset = 1;
					}
					break;
				case IS_NULL:
					if (zend_hash_find(ht, "", sizeof(""), (void **) &value) == SUCCESS) {
						isset = 1;
					}
					break;
				default: 
					zend_error(E_WARNING, "Illegal offset type in unset");
					
					break;
			}
			
			switch (opline->extended_value) {
				case ZEND_ISSET:
					if (isset && Z_TYPE_PP(value) == IS_NULL) {
						result = 0;
					} else {
						result = isset;
					}
					break;
				case ZEND_ISEMPTY:
					if (!isset || !zend_is_true(*value)) {
						result = 0;
					} else {
						result = 1;
					}
					break;
			}
			FREE_OP(EX(Ts), &opline->op2, EG(free_op2));
		} else if ((*container)->type == IS_OBJECT) {
			if (EG(free_op2)) {
				MAKE_REAL_ZVAL_PTR(offset);
			}
			if (prop_dim) {
				result = Z_OBJ_HT_P(*container)->has_property(*container, offset, (opline->extended_value == ZEND_ISEMPTY) TSRMLS_CC);
			} else {
				result = Z_OBJ_HT_P(*container)->has_dimension(*container, offset, (opline->extended_value == ZEND_ISEMPTY) TSRMLS_CC);
			}
			if (EG(free_op2)) {
				zval_ptr_dtor(&offset);
			}	
		} else if ((*container)->type == IS_STRING && !prop_dim) { /* string offsets */
			zval tmp;

			if (offset->type != IS_LONG) {
				tmp = *offset;
				zval_copy_ctor(&tmp);
				convert_to_long(&tmp);
				offset = &tmp;
			}
			switch (opline->extended_value) {
				case ZEND_ISSET:
					if (offset->value.lval >= 0 && offset->value.lval < Z_STRLEN_PP(container)) {
						result = 1;
					}
					break;
				case ZEND_ISEMPTY:
					if (offset->value.lval >= 0 && offset->value.lval < Z_STRLEN_PP(container) && Z_STRVAL_PP(container)[offset->value.lval] != '0') {
						result = 1;
					}
					break;
			}
			FREE_OP(EX(Ts), &opline->op2, EG(free_op2));
		} else {
			FREE_OP(EX(Ts), &opline->op2, EG(free_op2));
		}
	}

	EX_T(opline->result.u.var).tmp_var.type = IS_BOOL;

	switch (opline->extended_value) {
		case ZEND_ISSET:
			EX_T(opline->result.u.var).tmp_var.value.lval = result;
			break;
		case ZEND_ISEMPTY:
			EX_T(opline->result.u.var).tmp_var.value.lval = !result;
			break;
	}

	NEXT_OPCODE();
}


int zend_isset_isempty_dim_obj_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_isset_isempty_dim_prop_obj_handler(0, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}


int zend_isset_isempty_prop_obj_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	return zend_isset_isempty_dim_prop_obj_handler(1, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
}


int zend_exit_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	if (opline->op1.op_type != IS_UNUSED) {
		zval *ptr;

		ptr = get_zval_ptr(&opline->op1, EX(Ts), &EG(free_op1), BP_VAR_R);
		if (Z_TYPE_P(ptr) == IS_LONG) {
			EG(exit_status) = Z_LVAL_P(ptr);
		} else {
			zend_print_variable(ptr);
		}
		FREE_OP(EX(Ts), &opline->op1, EG(free_op1));
	}
	zend_bailout();
	NEXT_OPCODE();
}


int zend_begin_silence_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	EX_T(opline->result.u.var).tmp_var.value.lval = EG(error_reporting);
	EX_T(opline->result.u.var).tmp_var.type = IS_LONG;  /* shouldn't be necessary */
	zend_alter_ini_entry("error_reporting", sizeof("error_reporting"), "0", 1, ZEND_INI_USER, ZEND_INI_STAGE_RUNTIME);
	NEXT_OPCODE();
}


int zend_raise_abstract_error_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_error(E_ERROR, "Cannot call abstract method %s::%s()", EG(scope)->name, op_array->function_name);
	NEXT_OPCODE(); /* Never reached */
}


int zend_end_silence_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	zval restored_error_reporting;
	
	if (!EG(error_reporting)) {
		restored_error_reporting.type = IS_LONG;
		restored_error_reporting.value.lval = EX_T(opline->op1.u.var).tmp_var.value.lval;
		convert_to_string(&restored_error_reporting);
		zend_alter_ini_entry("error_reporting", sizeof("error_reporting"), Z_STRVAL(restored_error_reporting), Z_STRLEN(restored_error_reporting), ZEND_INI_USER, ZEND_INI_STAGE_RUNTIME);
		zendi_zval_dtor(restored_error_reporting);
	}
	NEXT_OPCODE();
}


int zend_qm_assign_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	zval *value = get_zval_ptr(&opline->op1, EX(Ts), &EG(free_op1), BP_VAR_R);

	EX_T(opline->result.u.var).tmp_var = *value;
	if (!EG(free_op1)) {
		zval_copy_ctor(&EX_T(opline->result.u.var).tmp_var);
	}
	NEXT_OPCODE();
}


int zend_ext_stmt_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	if (!EG(no_extensions)) {
		zend_llist_apply_with_argument(&zend_extensions, (llist_apply_with_arg_func_t) zend_extension_statement_handler, op_array TSRMLS_CC);
	}
	NEXT_OPCODE();
}


int zend_ext_fcall_begin_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	if (!EG(no_extensions)) {
		zend_llist_apply_with_argument(&zend_extensions, (llist_apply_with_arg_func_t) zend_extension_fcall_begin_handler, op_array TSRMLS_CC);
	}
	NEXT_OPCODE();
}


int zend_ext_fcall_end_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	if (!EG(no_extensions)) {
		zend_llist_apply_with_argument(&zend_extensions, (llist_apply_with_arg_func_t) zend_extension_fcall_end_handler, op_array TSRMLS_CC);
	}
	NEXT_OPCODE();
}


int zend_declare_class_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	EX_T(opline->result.u.var).class_entry = do_bind_class(opline, EG(class_table), 0 TSRMLS_CC);
	NEXT_OPCODE();
}


int zend_declare_inherited_class_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	EX_T(opline->result.u.var).class_entry = do_bind_inherited_class(opline, EG(class_table), EX_T(opline->extended_value).class_entry, 0 TSRMLS_CC);
	NEXT_OPCODE();
}


int zend_declare_function_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	do_bind_function(opline, EG(function_table), 0);
	NEXT_OPCODE();
}


int zend_ticks_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	if (++EG(ticks_count)>=opline->op1.u.constant.value.lval) {
		EG(ticks_count)=0;
		if (zend_ticks_function) {
			zend_ticks_function(opline->op1.u.constant.value.lval);
		}
	}
	NEXT_OPCODE();
}


int zend_instanceof_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	zval *expr = get_zval_ptr(&opline->op1, EX(Ts), &EG(free_op1), BP_VAR_R);
	zend_bool result;

	if (Z_TYPE_P(expr) == IS_OBJECT && Z_OBJ_HT_P(expr)->get_class_entry) {
		result = instanceof_function(Z_OBJCE_P(expr), EX_T(opline->op2.u.var).class_entry TSRMLS_CC);
	} else {
		result = 0;
	}
	ZVAL_BOOL(&EX_T(opline->result.u.var).tmp_var, result);
	FREE_OP(EX(Ts), &opline->op1, EG(free_op1));
	NEXT_OPCODE();
}


int zend_ext_nop_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	NEXT_OPCODE();
}


int zend_nop_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	NEXT_OPCODE();
}

int zend_add_interface_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_class_entry *ce = EX_T(opline->op1.u.var).class_entry;
	zend_class_entry *iface = EX_T(opline->op2.u.var).class_entry;

	if (!(iface->ce_flags & ZEND_ACC_INTERFACE)) {
		zend_error(E_ERROR, "%s cannot implement %s - it is not an interface", ce->name, iface->name);
	}

	ce->interfaces[opline->extended_value] = iface;

	zend_do_implement_interface(ce, iface TSRMLS_CC);

	NEXT_OPCODE();
}


int zend_handle_exception_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_uint op_num = EG(opline_before_exception)-EG(active_op_array)->opcodes;
	int i;
	int encapsulating_block=-1;
	zval **stack_zval_pp;
	
	stack_zval_pp = (zval **) EG(argument_stack).top_element - 1;
	while (*stack_zval_pp != NULL) {
		zval_ptr_dtor(stack_zval_pp);
		EG(argument_stack).top_element--;
		EG(argument_stack).top--;
		stack_zval_pp--;
	}

	for (i=0; i<EG(active_op_array)->last_try_catch; i++) {
		if (EG(active_op_array)->try_catch_array[i].try_op > op_num) {
			/* further blocks will not be relevant... */
			break;
		}
		if (op_num >= EG(active_op_array)->try_catch_array[i].try_op
			&& op_num < EG(active_op_array)->try_catch_array[i].catch_op) {
			encapsulating_block = i;
		}
	}

	while (EX(fbc)) {
		if (EX(object)) {
			zval_ptr_dtor(&EX(object));
		}
		zend_ptr_stack_n_pop(&EG(arg_types_stack), 3, &EX(calling_scope), &EX(object), &EX(fbc));
	}

	if (encapsulating_block == -1) {
		RETURN_FROM_EXECUTE_LOOP(execute_data);
	} else {
		SET_OPCODE(&op_array->opcodes[EG(active_op_array)->try_catch_array[encapsulating_block].catch_op]);
		return 0;
	}
}


int zend_verify_abstract_class_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_verify_abstract_class(EX_T(opline->op1.u.var).class_entry TSRMLS_CC);
	NEXT_OPCODE();
}


void zend_init_opcodes_handlers()
{
	memset(zend_opcode_handlers, 0, sizeof(zend_opcode_handlers));
	zend_opcode_handlers[ZEND_NOP] = zend_nop_handler;
	zend_opcode_handlers[ZEND_ADD] = zend_add_handler;
	zend_opcode_handlers[ZEND_SUB] = zend_sub_handler;
	zend_opcode_handlers[ZEND_MUL] = zend_mul_handler;
	zend_opcode_handlers[ZEND_DIV] = zend_div_handler;
	zend_opcode_handlers[ZEND_MOD] = zend_mod_handler;
	zend_opcode_handlers[ZEND_SL] = zend_sl_handler;
	zend_opcode_handlers[ZEND_SR] = zend_sr_handler;
	zend_opcode_handlers[ZEND_CONCAT] = zend_concat_handler;
	zend_opcode_handlers[ZEND_BW_OR] = zend_bw_or_handler;
	zend_opcode_handlers[ZEND_BW_AND] = zend_bw_and_handler;
	zend_opcode_handlers[ZEND_BW_XOR] = zend_bw_xor_handler;
	zend_opcode_handlers[ZEND_BW_NOT] = zend_bw_not_handler;
	zend_opcode_handlers[ZEND_BOOL_NOT] = zend_bool_not_handler;
	zend_opcode_handlers[ZEND_BOOL_XOR] = zend_bool_xor_handler;
	zend_opcode_handlers[ZEND_IS_IDENTICAL] = zend_is_identical_handler;
	zend_opcode_handlers[ZEND_IS_NOT_IDENTICAL] = zend_is_not_identical_handler;
	zend_opcode_handlers[ZEND_IS_EQUAL] = zend_is_equal_handler;
	zend_opcode_handlers[ZEND_IS_NOT_EQUAL] = zend_is_not_equal_handler;
	zend_opcode_handlers[ZEND_IS_SMALLER] = zend_is_smaller_handler;
	zend_opcode_handlers[ZEND_IS_SMALLER_OR_EQUAL] = zend_is_smaller_or_equal_handler;
	zend_opcode_handlers[ZEND_CAST] = zend_cast_handler;
	zend_opcode_handlers[ZEND_QM_ASSIGN] = zend_qm_assign_handler;

	zend_opcode_handlers[ZEND_ASSIGN_ADD] = zend_assign_add_handler;
	zend_opcode_handlers[ZEND_ASSIGN_SUB] = zend_assign_sub_handler;
	zend_opcode_handlers[ZEND_ASSIGN_MUL] = zend_assign_mul_handler;
	zend_opcode_handlers[ZEND_ASSIGN_DIV] = zend_assign_div_handler;
	zend_opcode_handlers[ZEND_ASSIGN_MOD] = zend_assign_mod_handler;
	zend_opcode_handlers[ZEND_ASSIGN_SL] = zend_assign_sl_handler;
	zend_opcode_handlers[ZEND_ASSIGN_SR] = zend_assign_sr_handler;
	zend_opcode_handlers[ZEND_ASSIGN_CONCAT] = zend_assign_concat_handler;
	zend_opcode_handlers[ZEND_ASSIGN_BW_OR] = zend_assign_bw_or_handler;
	zend_opcode_handlers[ZEND_ASSIGN_BW_AND] = zend_assign_bw_and_handler;
	zend_opcode_handlers[ZEND_ASSIGN_BW_XOR] = zend_assign_bw_xor_handler;

	zend_opcode_handlers[ZEND_PRE_INC] = zend_pre_inc_handler;
	zend_opcode_handlers[ZEND_PRE_DEC] = zend_pre_dec_handler;
	zend_opcode_handlers[ZEND_POST_INC] = zend_post_inc_handler;
	zend_opcode_handlers[ZEND_POST_DEC] = zend_post_dec_handler;

	zend_opcode_handlers[ZEND_ASSIGN] = zend_assign_handler;
	zend_opcode_handlers[ZEND_ASSIGN_REF] = zend_assign_ref_handler;

	zend_opcode_handlers[ZEND_ECHO] = zend_echo_handler;
	zend_opcode_handlers[ZEND_PRINT] = zend_print_handler;

	zend_opcode_handlers[ZEND_JMP] = zend_jmp_handler;
	zend_opcode_handlers[ZEND_JMPZ] = zend_jmpz_handler;
	zend_opcode_handlers[ZEND_JMPNZ] = zend_jmpnz_handler;
	zend_opcode_handlers[ZEND_JMPZNZ] = zend_jmpznz_handler;
	zend_opcode_handlers[ZEND_JMPZ_EX] = zend_jmpz_ex_handler;
	zend_opcode_handlers[ZEND_JMPNZ_EX] = zend_jmpnz_ex_handler;
	zend_opcode_handlers[ZEND_CASE] = zend_case_handler;
	zend_opcode_handlers[ZEND_SWITCH_FREE] = zend_switch_free_handler;
	zend_opcode_handlers[ZEND_BRK] = zend_brk_handler;
	zend_opcode_handlers[ZEND_CONT] = zend_cont_handler;
	zend_opcode_handlers[ZEND_BOOL] = zend_bool_handler;

	zend_opcode_handlers[ZEND_INIT_STRING] = zend_init_string_handler;
	zend_opcode_handlers[ZEND_ADD_CHAR] = zend_add_char_handler;
	zend_opcode_handlers[ZEND_ADD_STRING] = zend_add_string_handler;
	zend_opcode_handlers[ZEND_ADD_VAR] = zend_add_var_handler;

	zend_opcode_handlers[ZEND_BEGIN_SILENCE] = zend_begin_silence_handler;
	zend_opcode_handlers[ZEND_END_SILENCE] = zend_end_silence_handler;

	zend_opcode_handlers[ZEND_INIT_FCALL_BY_NAME] = zend_init_fcall_by_name_handler;
	zend_opcode_handlers[ZEND_DO_FCALL] = zend_do_fcall_handler;
	zend_opcode_handlers[ZEND_DO_FCALL_BY_NAME] = zend_do_fcall_by_name_handler;
	zend_opcode_handlers[ZEND_RETURN] = zend_return_handler;

	zend_opcode_handlers[ZEND_RECV] = zend_recv_handler;
	zend_opcode_handlers[ZEND_RECV_INIT] = zend_recv_init_handler;

	zend_opcode_handlers[ZEND_SEND_VAL] = zend_send_val_handler;
	zend_opcode_handlers[ZEND_SEND_VAR] = zend_send_var_handler;
	zend_opcode_handlers[ZEND_SEND_REF] = zend_send_ref_handler;

	zend_opcode_handlers[ZEND_NEW] = zend_new_handler;
	zend_opcode_handlers[ZEND_JMP_NO_CTOR] = zend_jmp_no_ctor_handler;
	zend_opcode_handlers[ZEND_FREE] = zend_free_handler;

	zend_opcode_handlers[ZEND_INIT_ARRAY] = zend_init_array_handler;
	zend_opcode_handlers[ZEND_ADD_ARRAY_ELEMENT] = zend_add_array_element_handler;

	zend_opcode_handlers[ZEND_INCLUDE_OR_EVAL] = zend_include_or_eval_handler;

	zend_opcode_handlers[ZEND_UNSET_VAR] = zend_unset_var_handler;
	zend_opcode_handlers[ZEND_UNSET_DIM_OBJ] = zend_unset_dim_obj_handler;

	zend_opcode_handlers[ZEND_FE_RESET] = zend_fe_reset_handler;
	zend_opcode_handlers[ZEND_FE_FETCH] = zend_fe_fetch_handler;

	zend_opcode_handlers[ZEND_EXIT] = zend_exit_handler;

	zend_opcode_handlers[ZEND_FETCH_R] = zend_fetch_r_handler;
	zend_opcode_handlers[ZEND_FETCH_DIM_R] = zend_fetch_dim_r_handler;
	zend_opcode_handlers[ZEND_FETCH_OBJ_R] = zend_fetch_obj_r_handler;
	zend_opcode_handlers[ZEND_FETCH_W] = zend_fetch_w_handler;
	zend_opcode_handlers[ZEND_FETCH_DIM_W] = zend_fetch_dim_w_handler;
	zend_opcode_handlers[ZEND_FETCH_OBJ_W] = zend_fetch_obj_w_handler;
	zend_opcode_handlers[ZEND_FETCH_RW] = zend_fetch_rw_handler;
	zend_opcode_handlers[ZEND_FETCH_DIM_RW] = zend_fetch_dim_rw_handler;
	zend_opcode_handlers[ZEND_FETCH_OBJ_RW] = zend_fetch_obj_rw_handler;
	zend_opcode_handlers[ZEND_FETCH_IS] = zend_fetch_is_handler;
	zend_opcode_handlers[ZEND_FETCH_DIM_IS] = zend_fetch_dim_is_handler;
	zend_opcode_handlers[ZEND_FETCH_OBJ_IS] = zend_fetch_obj_is_handler;
	zend_opcode_handlers[ZEND_FETCH_FUNC_ARG] = zend_fetch_func_arg_handler;
	zend_opcode_handlers[ZEND_FETCH_DIM_FUNC_ARG] = zend_fetch_dim_func_arg_handler;
	zend_opcode_handlers[ZEND_FETCH_OBJ_FUNC_ARG] = zend_fetch_obj_func_arg_handler;
	zend_opcode_handlers[ZEND_FETCH_UNSET] = zend_fetch_unset_handler;
	zend_opcode_handlers[ZEND_FETCH_DIM_UNSET] = zend_fetch_dim_unset_handler;
	zend_opcode_handlers[ZEND_FETCH_OBJ_UNSET] = zend_fetch_obj_unset_handler;

	zend_opcode_handlers[ZEND_FETCH_DIM_TMP_VAR] = zend_fetch_dim_tmp_var_handler;
	zend_opcode_handlers[ZEND_FETCH_CONSTANT] = zend_fetch_constant_handler;

	zend_opcode_handlers[ZEND_EXT_STMT] = zend_ext_stmt_handler;
	zend_opcode_handlers[ZEND_EXT_FCALL_BEGIN] = zend_ext_fcall_begin_handler;
	zend_opcode_handlers[ZEND_EXT_FCALL_END] = zend_ext_fcall_end_handler;
	zend_opcode_handlers[ZEND_EXT_NOP] = zend_ext_nop_handler;

	zend_opcode_handlers[ZEND_TICKS] = zend_ticks_handler;

	zend_opcode_handlers[ZEND_SEND_VAR_NO_REF] = zend_send_var_no_ref_handler;

	zend_opcode_handlers[ZEND_CATCH] = zend_catch_handler;
	zend_opcode_handlers[ZEND_THROW] = zend_throw_handler;

	zend_opcode_handlers[ZEND_FETCH_CLASS] = zend_fetch_class_handler;

	zend_opcode_handlers[ZEND_CLONE] = zend_clone_handler;

	zend_opcode_handlers[ZEND_INIT_CTOR_CALL] = zend_init_ctor_call_handler;
	zend_opcode_handlers[ZEND_INIT_METHOD_CALL] = zend_init_method_call_handler;
	zend_opcode_handlers[ZEND_INIT_STATIC_METHOD_CALL] = zend_init_static_method_call_handler;

	zend_opcode_handlers[ZEND_ISSET_ISEMPTY_VAR] = zend_isset_isempty_var_handler;
	zend_opcode_handlers[ZEND_ISSET_ISEMPTY_DIM_OBJ] = zend_isset_isempty_dim_obj_handler;
	zend_opcode_handlers[ZEND_ISSET_ISEMPTY_PROP_OBJ] = zend_isset_isempty_prop_obj_handler;

	zend_opcode_handlers[ZEND_PRE_INC_OBJ] = zend_pre_inc_obj_handler;
	zend_opcode_handlers[ZEND_PRE_DEC_OBJ] = zend_pre_dec_obj_handler;
	zend_opcode_handlers[ZEND_POST_INC_OBJ] = zend_post_inc_obj_handler;
	zend_opcode_handlers[ZEND_POST_DEC_OBJ] = zend_post_dec_obj_handler;

	zend_opcode_handlers[ZEND_ASSIGN_OBJ] = zend_assign_obj_handler;
	zend_opcode_handlers[ZEND_OP_DATA] = NULL;

	zend_opcode_handlers[ZEND_INSTANCEOF] = zend_instanceof_handler;

	zend_opcode_handlers[ZEND_DECLARE_CLASS] = zend_declare_class_handler;
	zend_opcode_handlers[ZEND_DECLARE_INHERITED_CLASS] = zend_declare_inherited_class_handler;
	zend_opcode_handlers[ZEND_DECLARE_FUNCTION] = zend_declare_function_handler;

	zend_opcode_handlers[ZEND_RAISE_ABSTRACT_ERROR] = zend_raise_abstract_error_handler;

	zend_opcode_handlers[ZEND_ADD_INTERFACE] = zend_add_interface_handler;
	zend_opcode_handlers[ZEND_VERIFY_ABSTRACT_CLASS] = zend_verify_abstract_class_handler;

	zend_opcode_handlers[ZEND_ASSIGN_DIM] = zend_assign_dim_handler;

	zend_opcode_handlers[ZEND_HANDLE_EXCEPTION] = zend_handle_exception_handler;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
