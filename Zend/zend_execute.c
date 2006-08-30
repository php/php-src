/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2006 Zend Technologies Ltd. (http://www.zend.com) |
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
#include "zend_ini.h"
#include "zend_exceptions.h"
#include "zend_interfaces.h"
#include "zend_vm.h"
#include "zend_unicode.h"

/* Virtual current working directory support */
#include "tsrm_virtual_cwd.h"

#define _CONST_CODE  0
#define _TMP_CODE    1
#define _VAR_CODE    2
#define _UNUSED_CODE 3
#define _CV_CODE     4

typedef int (*incdec_t)(zval *);

#define get_zval_ptr(node, Ts, should_free, type) _get_zval_ptr(node, Ts, should_free, type TSRMLS_CC)
#define get_zval_ptr_ptr(node, Ts, should_free, type) _get_zval_ptr_ptr(node, Ts, should_free, type TSRMLS_CC)
#define get_obj_zval_ptr(node, Ts, should_free, type) _get_obj_zval_ptr(node, Ts, should_free, type TSRMLS_CC)
#define get_obj_zval_ptr_ptr(node, Ts, should_free, type) _get_obj_zval_ptr_ptr(node, Ts, should_free, type TSRMLS_CC)

/* Prototypes */
static void zend_extension_statement_handler(zend_extension *extension, zend_op_array *op_array TSRMLS_DC);
static void zend_extension_fcall_begin_handler(zend_extension *extension, zend_op_array *op_array TSRMLS_DC);
static void zend_extension_fcall_end_handler(zend_extension *extension, zend_op_array *op_array TSRMLS_DC);

#define RETURN_VALUE_USED(opline) (!((opline)->result.u.EA.type & EXT_TYPE_UNUSED))

#define EX_T(offset) (*(temp_variable *)((char *) EX(Ts) + offset))
#define T(offset) (*(temp_variable *)((char *) Ts + offset))

#define TEMP_VAR_STACK_LIMIT 2000

static inline void zend_pzval_unlock_func(zval *z, zend_free_op *should_free)
{
	if (!--z->refcount) {
		z->refcount = 1;
		z->is_ref = 0;
		should_free->var = z;
/*		should_free->is_var = 1; */
	} else {
		should_free->var = 0;
		if (z->is_ref && z->refcount == 1) {
			z->is_ref = 0;
		}
	}
}

static inline void zend_pzval_unlock_free_func(zval *z)
{
	if (!--z->refcount) {
		zval_dtor(z);
		safe_free_zval_ptr(z);
	}
}

#define PZVAL_UNLOCK(z, f) zend_pzval_unlock_func(z, f)
#define PZVAL_UNLOCK_FREE(z) zend_pzval_unlock_free_func(z)
#define PZVAL_LOCK(z) (z)->refcount++
#define RETURN_VALUE_UNUSED(pzn)	(((pzn)->u.EA.type & EXT_TYPE_UNUSED))
#define SELECTIVE_PZVAL_LOCK(pzv, pzn)	if (!RETURN_VALUE_UNUSED(pzn)) { PZVAL_LOCK(pzv); }

#define AI_USE_PTR(ai) \
	if ((ai).ptr_ptr) { \
		(ai).ptr = *((ai).ptr_ptr); \
		(ai).ptr_ptr = &((ai).ptr); \
	} else { \
		(ai).ptr = NULL; \
	}

#define FREE_OP(should_free) \
	if (should_free.var) { \
		if ((long)should_free.var & 1L) { \
			zval_dtor((zval*)((long)should_free.var & ~1L)); \
		} else { \
			zval_ptr_dtor(&should_free.var); \
		} \
	}

#define FREE_OP_IF_VAR(should_free) \
	if (should_free.var != NULL && (((long)should_free.var & 1L) == 0)) { \
		zval_ptr_dtor(&should_free.var); \
	}

#define FREE_OP_VAR_PTR(should_free) \
	if (should_free.var) { \
		zval_ptr_dtor(&should_free.var); \
	}

#define TMP_FREE(z) (zval*)(((long)(z)) | 1L)

#define IS_TMP_FREE(should_free) ((long)should_free.var & 1L)

#define INIT_PZVAL_COPY(z,v) \
	(z)->value = (v)->value; \
	Z_TYPE_P(z) = Z_TYPE_P(v); \
	(z)->refcount = 1; \
	(z)->is_ref = 0;

#define MAKE_REAL_ZVAL_PTR(val) \
	do { \
		zval *_tmp; \
		ALLOC_ZVAL(_tmp); \
		_tmp->value = (val)->value; \
		Z_TYPE_P(_tmp) = Z_TYPE_P(val); \
		_tmp->refcount = 1; \
		_tmp->is_ref = 0; \
		val = _tmp; \
	} while (0)

/* End of zend_execute_locks.h */

#define CV_OF(i)     (EG(current_execute_data)->CVs[i])
#define CV_DEF_OF(i) (EG(active_op_array)->vars[i])

ZEND_API zval** zend_get_compiled_variable_value(zend_execute_data *execute_data_ptr, zend_uint var)
{
	return execute_data_ptr->CVs[var];
}

static inline void zend_get_cv_address(zend_uchar utype, zend_compiled_variable *cv, zval ***ptr, temp_variable *Ts TSRMLS_DC)
{
	zval *new_zval = &EG(uninitialized_zval);

	new_zval->refcount++;
	zend_u_hash_quick_update(EG(active_symbol_table), utype, cv->name, cv->name_len+1, cv->hash_value, &new_zval, sizeof(zval *), (void **)ptr);
}

static inline zval *_get_zval_ptr_tmp(znode *node, temp_variable *Ts, zend_free_op *should_free TSRMLS_DC)
{
	return should_free->var = &T(node->u.var).tmp_var;
}

static inline zval *_get_zval_ptr_var(znode *node, temp_variable *Ts, zend_free_op *should_free TSRMLS_DC)
{
	if (T(node->u.var).var.ptr) {
		PZVAL_UNLOCK(T(node->u.var).var.ptr, should_free);
		return T(node->u.var).var.ptr;
	} else {
		temp_variable *T = &T(node->u.var);
		zval *str = T->str_offset.str;
		zval *ptr;

		/* string offset */
		ALLOC_ZVAL(ptr);
		T->str_offset.ptr = ptr;
		should_free->var = ptr;

		/* T->str_offset.str here is always IS_STRING or IS_UNICODE */
		if (Z_TYPE_P(T->str_offset.str) == IS_STRING) {
			if (((int)T->str_offset.offset<0)
				|| (Z_STRLEN_P(T->str_offset.str) <= T->str_offset.offset)) {
				zend_error(E_NOTICE, "Uninitialized string offset:  %d", T->str_offset.offset);
				Z_STRVAL_P(ptr) = STR_EMPTY_ALLOC();
				Z_STRLEN_P(ptr) = 0;
			} else {
				char c = Z_STRVAL_P(str)[T->str_offset.offset];

				Z_STRVAL_P(ptr) = estrndup(&c, 1);
				Z_STRLEN_P(ptr) = 1;
			}
			Z_TYPE_P(ptr) = IS_STRING;
		} else {
			if (((int)T->str_offset.offset<0)
				|| (Z_USTRCPLEN_P(T->str_offset.str) <= T->str_offset.offset)) {
				zend_error(E_NOTICE, "Uninitialized string offset:  %d", T->str_offset.offset);
				Z_USTRVAL_P(ptr) = USTR_MAKE("");
				Z_USTRLEN_P(ptr) = 0;
			} else {
				UChar32 c = zend_get_codepoint_at(Z_USTRVAL_P(str), Z_USTRLEN_P(str), T->str_offset.offset);
				int32_t i = 0;

				Z_USTRVAL_P(ptr) = eumalloc(3); /* potentially 2 code units + null */
				U16_APPEND_UNSAFE(Z_USTRVAL_P(ptr), i, c);
				Z_USTRVAL_P(ptr)[i] = 0;
				Z_USTRLEN_P(ptr) = i;
			}
			Z_TYPE_P(ptr) = IS_UNICODE;
		}
		PZVAL_UNLOCK_FREE(str);
		ptr->refcount=1;
		ptr->is_ref=1;
		return ptr;
	}
}

static inline zval *_get_zval_ptr_cv(znode *node, temp_variable *Ts, int type TSRMLS_DC)
{
	zval ***ptr = &CV_OF(node->u.var);

	if (!*ptr) {
		zend_compiled_variable *cv = &CV_DEF_OF(node->u.var);
		zend_uchar utype = UG(unicode)?IS_UNICODE:IS_STRING;

		if (zend_u_hash_quick_find(EG(active_symbol_table), utype, cv->name, cv->name_len+1, cv->hash_value, (void **)ptr)==FAILURE) {
			switch (type) {
				case BP_VAR_R:
				case BP_VAR_UNSET:
					zend_error(E_NOTICE, "Undefined variable: %v", cv->name);
					/* break missing intentionally */
				case BP_VAR_IS:
					return &EG(uninitialized_zval);
					break;
				case BP_VAR_RW:
					zend_error(E_NOTICE, "Undefined variable: %v", cv->name);
					/* break missing intentionally */
				case BP_VAR_W:
					zend_get_cv_address(utype, cv, ptr, Ts TSRMLS_CC);
					break;
			}
		}
	}
	return **ptr;
}

static inline zval *_get_zval_ptr(znode *node, temp_variable *Ts, zend_free_op *should_free, int type TSRMLS_DC)
{
/*	should_free->is_var = 0; */
	switch (node->op_type) {
		case IS_CONST:
			should_free->var = 0;
			return &node->u.constant;
			break;
		case IS_TMP_VAR:
			should_free->var = TMP_FREE(&T(node->u.var).tmp_var);
			return &T(node->u.var).tmp_var;
			break;
		case IS_VAR:
			return _get_zval_ptr_var(node, Ts, should_free TSRMLS_CC);
			break;
		case IS_UNUSED:
			should_free->var = 0;
			return NULL;
			break;
		case IS_CV:
			should_free->var = 0;
			return _get_zval_ptr_cv(node, Ts, type TSRMLS_CC);
			break;
		EMPTY_SWITCH_DEFAULT_CASE()
	}
	return NULL;
}

static inline zval **_get_zval_ptr_ptr_var(znode *node, temp_variable *Ts, zend_free_op *should_free TSRMLS_DC)
{
	zval** ptr_ptr = T(node->u.var).var.ptr_ptr;

	if (ptr_ptr) {
		PZVAL_UNLOCK(*ptr_ptr, should_free);
	} else {
		/* string offset */
		PZVAL_UNLOCK(T(node->u.var).str_offset.str, should_free);
	}
	return ptr_ptr;
}

static inline zval **_get_zval_ptr_ptr_cv(znode *node, temp_variable *Ts, int type TSRMLS_DC)
{
	zval ***ptr = &CV_OF(node->u.var);

	if (!*ptr) {
		zend_compiled_variable *cv = &CV_DEF_OF(node->u.var);
		zend_uchar utype = UG(unicode)?IS_UNICODE:IS_STRING;

		if (zend_u_hash_quick_find(EG(active_symbol_table), utype, cv->name, cv->name_len+1, cv->hash_value, (void **)ptr)==FAILURE) {
			switch (type) {
				case BP_VAR_R:
				case BP_VAR_UNSET:
					zend_error(E_NOTICE, "Undefined variable: %v", cv->name);
					/* break missing intentionally */
				case BP_VAR_IS:
					return &EG(uninitialized_zval_ptr);
					break;
				case BP_VAR_RW:
					zend_error(E_NOTICE, "Undefined variable: %v", cv->name);
					/* break missing intentionally */
				case BP_VAR_W:
					zend_get_cv_address(utype, cv, ptr, Ts TSRMLS_CC);
					break;
			}
		}
	}
	return *ptr;
}

static inline zval **_get_zval_ptr_ptr(znode *node, temp_variable *Ts, zend_free_op *should_free, int type TSRMLS_DC)
{
	if (node->op_type == IS_CV) {
		should_free->var = 0;
		return _get_zval_ptr_ptr_cv(node, Ts, type TSRMLS_CC);
	} else if (node->op_type == IS_VAR) {
		return _get_zval_ptr_ptr_var(node, Ts, should_free TSRMLS_CC);
	} else {
		should_free->var = 0;
		return NULL;
	}
}

static inline zval *_get_obj_zval_ptr_unused(TSRMLS_D)
{
	if (EG(This)) {
		return EG(This);
	} else {
		zend_error_noreturn(E_ERROR, "Using $this when not in object context");
		return NULL;
	}
}

static inline zval **_get_obj_zval_ptr_ptr(znode *op, temp_variable *Ts, zend_free_op *should_free, int type TSRMLS_DC)
{
	if (op->op_type == IS_UNUSED) {
		if (EG(This)) {
			/* this should actually never be modified, _ptr_ptr is modified only when
			   the object is empty */
			should_free->var = 0;
			return &EG(This);
		} else {
			zend_error_noreturn(E_ERROR, "Using $this when not in object context");
		}
	}
	return get_zval_ptr_ptr(op, Ts, should_free, type);
}

static inline zval **_get_obj_zval_ptr_ptr_unused(TSRMLS_D)
{
	if (EG(This)) {
		return &EG(This);
	} else {
		zend_error_noreturn(E_ERROR, "Using $this when not in object context");
		return NULL;
	}
}

static inline zval *_get_obj_zval_ptr(znode *op, temp_variable *Ts, zend_free_op *should_free, int type TSRMLS_DC)
{
	if (op->op_type == IS_UNUSED) {
		if (EG(This)) {
			should_free->var = 0;
			return EG(This);
		} else {
			zend_error_noreturn(E_ERROR, "Using $this when not in object context");
		}
	}
	return get_zval_ptr(op, Ts, should_free, type);
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
				PZVAL_UNLOCK_FREE(T->str_offset.str);
			} else {
				zval_ptr_dtor(&T(opline->op1.u.var).var.ptr);
				if (opline->extended_value & ZEND_FE_RESET_VARIABLE) { /* foreach() free */
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

static void zend_assign_to_variable_reference(zval **variable_ptr_ptr, zval **value_ptr_ptr TSRMLS_DC)
{
	zval *variable_ptr;
	zval *value_ptr;

	if (!value_ptr_ptr || !variable_ptr_ptr) {
		zend_error_noreturn(E_ERROR, "Cannot create references to/from string offsets nor overloaded objects");
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
}

static inline void make_real_object(zval **object_ptr TSRMLS_DC)
{
/* this should modify object only if it's empty */
	if (Z_TYPE_PP(object_ptr) == IS_NULL
		|| (Z_TYPE_PP(object_ptr) == IS_BOOL && Z_LVAL_PP(object_ptr)==0)
		|| (Z_TYPE_PP(object_ptr) == IS_STRING && Z_STRLEN_PP(object_ptr) == 0)
		|| (Z_TYPE_PP(object_ptr) == IS_UNICODE && Z_USTRLEN_PP(object_ptr) == 0)) {
		if (!PZVAL_IS_REF(*object_ptr)) {
			SEPARATE_ZVAL(object_ptr);
		}
		zend_error(E_STRICT, "Creating default object from empty value");
		zval_dtor(*object_ptr);
		object_init(*object_ptr);
	}
}

static inline char * zend_verify_arg_class_kind(zend_arg_info *cur_arg_info, zend_class_entry **pce TSRMLS_DC)
{
	*pce = zend_u_fetch_class(UG(unicode) ? IS_UNICODE : IS_STRING, cur_arg_info->class_name, cur_arg_info->class_name_len, ZEND_FETCH_CLASS_AUTO TSRMLS_CC);

	if ((*pce)->ce_flags & ZEND_ACC_INTERFACE) {
		return "implement interface ";
	} else {
		return "be an instance of ";
	}
}

static inline int zend_verify_arg_error(zend_function *zf, zend_uint arg_num, zend_arg_info *cur_arg_info, char *need_msg, zstr need_kind, char *given_msg, zstr given_kind TSRMLS_DC)
{
	zend_execute_data *ptr = EG(current_execute_data)->prev_execute_data;
	zstr fname = zf->common.function_name;
	char *fsep;
	zstr fclass;

	if (zf->common.scope) {
		fsep =  "::";
		fclass = zf->common.scope->name;
	} else {
		fsep =  "";
		fclass = EMPTY_ZSTR;
	}

	if (ptr && ptr->op_array) {
		zend_error(E_RECOVERABLE_ERROR, "Argument %d passed to %v%s%v() must %s%v, %s%v given, called in %s on line %d and defined", arg_num, fclass, fsep, fname, need_msg, need_kind, given_msg, given_kind, ptr->op_array->filename, ptr->opline->lineno);
	} else {
		zend_error(E_RECOVERABLE_ERROR, "Argument %d passed to %v%s%v() must %s%v, %s%v given", arg_num, fclass, fsep, fname, need_msg, need_kind, given_msg, given_kind);
	}
	return 0;
}

static inline int zend_verify_arg_type(zend_function *zf, zend_uint arg_num, zval *arg TSRMLS_DC)
{
	zend_arg_info *cur_arg_info;
	char *need_msg;
	zend_class_entry *ce;

	if (!zf->common.arg_info
		|| arg_num>zf->common.num_args) {
		return 1;
	}

	cur_arg_info = &zf->common.arg_info[arg_num-1];

	if (cur_arg_info->class_name.v) {
		if (!arg) {
			need_msg = zend_verify_arg_class_kind(cur_arg_info, &ce TSRMLS_CC);
			return zend_verify_arg_error(zf, arg_num, cur_arg_info, need_msg, ce->name, "none", EMPTY_ZSTR TSRMLS_CC);
		}
		if (Z_TYPE_P(arg) == IS_OBJECT) {
			need_msg = zend_verify_arg_class_kind(cur_arg_info, &ce TSRMLS_CC);
			if (!instanceof_function(Z_OBJCE_P(arg), ce TSRMLS_CC)) {
				return zend_verify_arg_error(zf, arg_num, cur_arg_info, need_msg, ce->name, "instance of ", Z_OBJCE_P(arg)->name TSRMLS_CC);
			}
		} else if (Z_TYPE_P(arg) != IS_NULL || !cur_arg_info->allow_null) {
			need_msg = zend_verify_arg_class_kind(cur_arg_info, &ce TSRMLS_CC);
			return zend_verify_arg_error(zf, arg_num, cur_arg_info, need_msg, ce->name, zend_zval_type_name(arg), EMPTY_ZSTR TSRMLS_CC);
		}
	} else if (cur_arg_info->array_type_hint) {
		if (!arg) {
			return zend_verify_arg_error(zf, arg_num, cur_arg_info, "be an array", EMPTY_ZSTR, "none", EMPTY_ZSTR TSRMLS_CC);
		}
		if (Z_TYPE_P(arg) != IS_ARRAY && (Z_TYPE_P(arg) != IS_NULL || !cur_arg_info->allow_null)) {
			return zend_verify_arg_error(zf, arg_num, cur_arg_info, "be an array", EMPTY_ZSTR, zend_zval_type_name(arg), EMPTY_ZSTR TSRMLS_CC);
		}
	}
	return 1;
}


static inline void zend_assign_to_object(znode *result, zval **object_ptr, znode *op2, znode *value_op, temp_variable *Ts, int opcode TSRMLS_DC)
{
	zval *object;
	zend_free_op free_op2, free_value;
	zval *property_name = get_zval_ptr(op2, Ts, &free_op2, BP_VAR_R);
	zval *value = get_zval_ptr(value_op, Ts, &free_value, BP_VAR_R);
	zval **retval = &T(result->u.var).var.ptr;

	if (*object_ptr == EG(error_zval_ptr)) {
		FREE_OP(free_op2);
		if (!RETURN_VALUE_UNUSED(result)) {
			*retval = EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*retval);
		}
		FREE_OP(free_value);
		return;
	}

	make_real_object(object_ptr TSRMLS_CC); /* this should modify object only if it's empty */
	object = *object_ptr;

	if (Z_TYPE_P(object) != IS_OBJECT || (opcode == ZEND_ASSIGN_OBJ && !Z_OBJ_HT_P(object)->write_property)) {
		zend_error(E_WARNING, "Attempt to assign property of non-object");
		FREE_OP(free_op2);
		if (!RETURN_VALUE_UNUSED(result)) {
			*retval = EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*retval);
		}
		FREE_OP(free_value);
		return;
	}

	/* here we are sure we are dealing with an object */

	/* separate our value if necessary */
	if (value_op->op_type == IS_TMP_VAR) {
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
		if (IS_TMP_FREE(free_op2)) {
			MAKE_REAL_ZVAL_PTR(property_name);
		}
		Z_OBJ_HT_P(object)->write_property(object, property_name, value TSRMLS_CC);
	} else {
		/* Note:  property_name in this case is really the array index! */
		if (!Z_OBJ_HT_P(object)->write_dimension) {
			zend_error_noreturn(E_ERROR, "Cannot use object as array");
		}
		if (IS_TMP_FREE(free_op2)) {
			MAKE_REAL_ZVAL_PTR(property_name);
		}
		Z_OBJ_HT_P(object)->write_dimension(object, property_name, value TSRMLS_CC);
	}

	if (result && !RETURN_VALUE_UNUSED(result)) {
		T(result->u.var).var.ptr = value;
		T(result->u.var).var.ptr_ptr = &T(result->u.var).var.ptr; /* this is so that we could use it in FETCH_DIM_R, etc. - see bug #27876 */
		PZVAL_LOCK(value);
	}
	if (IS_TMP_FREE(free_op2)) {
		zval_ptr_dtor(&property_name);
	} else {
		FREE_OP(free_op2);
	}
	zval_ptr_dtor(&value);
	FREE_OP_IF_VAR(free_value);
}


static inline void zend_assign_to_variable(znode *result, znode *op1, znode *op2, zval *value, int type, temp_variable *Ts TSRMLS_DC)
{
	zend_free_op free_op1;
	zval **variable_ptr_ptr = get_zval_ptr_ptr(op1, Ts, &free_op1, BP_VAR_W);
	zval *variable_ptr;

	if (!variable_ptr_ptr) {
		temp_variable *T = &T(op1->u.var);

		if (UG(unicode) && Z_TYPE_P(T->str_offset.str) == IS_STRING && Z_TYPE_P(value) != IS_STRING) {
			convert_to_unicode(T->str_offset.str);
		}

		if (Z_TYPE_P(T->str_offset.str) == IS_STRING)
		do {
			zval tmp;
			zval *final_value = value;

			if (((int)T->str_offset.offset < 0)) {
				zend_error(E_WARNING, "Illegal string offset:  %d", T->str_offset.offset);
				break;
			}
			if (T->str_offset.offset >= Z_STRLEN_P(T->str_offset.str)) {
				zend_uint i;

				if (Z_STRLEN_P(T->str_offset.str)==0) {
					STR_FREE(Z_STRVAL_P(T->str_offset.str));
					Z_STRVAL_P(T->str_offset.str) = (char *) emalloc(T->str_offset.offset+1+1);
				} else {
					Z_STRVAL_P(T->str_offset.str) = (char *) erealloc(Z_STRVAL_P(T->str_offset.str), T->str_offset.offset+1+1);
				}
				for (i=Z_STRLEN_P(T->str_offset.str); i<T->str_offset.offset; i++) {
					Z_STRVAL_P(T->str_offset.str)[i] = ' ';
				}
				Z_STRVAL_P(T->str_offset.str)[T->str_offset.offset+1] = 0;
				Z_STRLEN_P(T->str_offset.str) = T->str_offset.offset+1;
			}

			if (Z_TYPE_P(value)!=IS_STRING) {
				tmp = *value;
				if (op2->op_type & (IS_VAR|IS_CV|IS_CONST)) {
					zval_copy_ctor(&tmp);
				}
				convert_to_string(&tmp);
				final_value = &tmp;
			}

			Z_STRVAL_P(T->str_offset.str)[T->str_offset.offset] = Z_STRVAL_P(final_value)[0];

			if (op2->op_type == IS_TMP_VAR) {
				if (final_value == &T(op2->u.var).tmp_var) {
					/* we can safely free final_value here
					 * because separation is done only
					 * in case op2->op_type == IS_VAR */
					STR_FREE(Z_STRVAL_P(final_value));
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
		else if (Z_TYPE_P(T->str_offset.str) == IS_UNICODE) do {
			zval tmp;
			zval *final_value = value;

			if (((int)T->str_offset.offset < 0)) {
				zend_error(E_WARNING, "Illegal string offset:  %d", T->str_offset.offset);
				break;
			}
			if (T->str_offset.offset >= Z_USTRLEN_P(T->str_offset.str)) {
				zend_uint i;

				if (Z_USTRLEN_P(T->str_offset.str)==0) {
					USTR_FREE(Z_USTRVAL_P(T->str_offset.str));
					Z_USTRVAL_P(T->str_offset.str) = eumalloc(T->str_offset.offset+1+1);
				} else {
					Z_USTRVAL_P(T->str_offset.str) = eurealloc(Z_USTRVAL_P(T->str_offset.str), T->str_offset.offset+1+1);
				}
				for (i=Z_USTRLEN_P(T->str_offset.str); i<T->str_offset.offset; i++) {
					Z_USTRVAL_P(T->str_offset.str)[i] = 0x20; /* ' ' */
				}
				Z_USTRVAL_P(T->str_offset.str)[T->str_offset.offset+1] = 0;
				Z_USTRLEN_P(T->str_offset.str) = T->str_offset.offset+1;
			}

			if (Z_TYPE_P(value)!=IS_UNICODE) {
				tmp = *value;
				if (op2->op_type & (IS_VAR|IS_CV|IS_CONST)) {
					zval_copy_ctor(&tmp);
				}
				convert_to_unicode(&tmp);
				final_value = &tmp;
			}

			Z_USTRVAL_P(T->str_offset.str)[T->str_offset.offset] = Z_USTRVAL_P(final_value)[0];

			if (op2->op_type == IS_TMP_VAR) {
				if (final_value == &T(op2->u.var).tmp_var) {
					/* we can safely free final_value here
					 * because separation is done only
					 * in case op2->op_type == IS_VAR */
					USTR_FREE(Z_USTRVAL_P(final_value));
				}
			}
			if (final_value == &tmp) {
				zval_dtor(final_value);
			}
		} while (0);
		/* zval_ptr_dtor(&T->str_offset.str); Nuke this line if it doesn't cause a leak */

/*		T(result->u.var).var.ptr_ptr = &EG(uninitialized_zval_ptr); */
		if (!RETURN_VALUE_UNUSED(result)) {
			T(result->u.var).var.ptr_ptr = &value;
			PZVAL_LOCK(*T(result->u.var).var.ptr_ptr);
			AI_USE_PTR(T(result->u.var).var);
		}
		FREE_OP_VAR_PTR(free_op1);
		return;
	}

	variable_ptr = *variable_ptr_ptr;

	if (variable_ptr == EG(error_zval_ptr)) {
		if (result && !RETURN_VALUE_UNUSED(result)) {
			T(result->u.var).var.ptr_ptr = &EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*T(result->u.var).var.ptr_ptr);
			AI_USE_PTR(T(result->u.var).var);
		}
		if (type==IS_TMP_VAR) {
			zval_dtor(value);
		}
		FREE_OP_VAR_PTR(free_op1);
		return;
	}

	if (Z_TYPE_P(variable_ptr) == IS_OBJECT && Z_OBJ_HANDLER_P(variable_ptr, set)) {
		Z_OBJ_HANDLER_P(variable_ptr, set)(variable_ptr_ptr, value TSRMLS_CC);
		goto done_setting_var;
	}

	if (PZVAL_IS_REF(variable_ptr)) {
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
				case IS_CV:
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
				case IS_CV:
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
	if (result && !RETURN_VALUE_UNUSED(result)) {
		T(result->u.var).var.ptr_ptr = variable_ptr_ptr;
		PZVAL_LOCK(*variable_ptr_ptr);
		AI_USE_PTR(T(result->u.var).var);
	}
	FREE_OP_VAR_PTR(free_op1);
}


static inline void zend_receive(zval **variable_ptr_ptr, zval *value TSRMLS_DC)
{
	(*variable_ptr_ptr)->refcount--;
	*variable_ptr_ptr = value;
	value->refcount++;
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


static inline HashTable *zend_get_target_symbol_table(zend_op *opline, temp_variable *Ts, int type, zval *variable TSRMLS_DC)
{
	switch (opline->op2.u.EA.type) {
		case ZEND_FETCH_LOCAL:
			return EG(active_symbol_table);
			break;
		case ZEND_FETCH_GLOBAL:
		case ZEND_FETCH_GLOBAL_LOCK:
			return &EG(symbol_table);
			break;
		case ZEND_FETCH_STATIC:
			if (!EG(active_op_array)->static_variables) {
				ALLOC_HASHTABLE(EG(active_op_array)->static_variables);
				zend_u_hash_init(EG(active_op_array)->static_variables, 2, NULL, ZVAL_PTR_DTOR, 0, UG(unicode));
			}
			return EG(active_op_array)->static_variables;
			break;
		EMPTY_SWITCH_DEFAULT_CASE()
	}
	return NULL;
}

static inline zval **zend_fetch_dimension_address_inner(HashTable *ht, zval *dim, int type TSRMLS_DC)
{
	zval **retval;
	zstr offset_key;
	int offset_key_length;
	zend_uchar ztype = Z_TYPE_P(dim);
	int free_offset = 0;

	switch (ztype) {
		case IS_NULL:
			ztype = IS_STRING;
			offset_key.s = "";
			offset_key_length = 0;
			goto fetch_string_dim;

		case IS_STRING:
		case IS_UNICODE:

			offset_key = Z_UNIVAL_P(dim);
			offset_key_length = Z_UNILEN_P(dim);

fetch_string_dim:
			if (UG(unicode) && ht == &EG(symbol_table) && ztype == IS_UNICODE) {
				/* Identifier normalization */
				UChar *norm;
				int norm_len;

				if (zend_normalize_identifier(&norm, &norm_len, offset_key.u, offset_key_length, 0) == FAILURE) {
					zend_error(E_WARNING, "Could not normalize identifier: %r", offset_key);
				} else if (norm != offset_key.u) {
					offset_key.u = norm;
					offset_key_length = norm_len;
					free_offset = 1;
				}
			}
			if (zend_u_symtable_find(ht, ztype, offset_key, offset_key_length + 1, (void **) &retval) == FAILURE) {
				switch (type) {
					case BP_VAR_R:
						zend_error(E_NOTICE, "Undefined index:  %R", ztype, offset_key);
						/* break missing intentionally */
					case BP_VAR_UNSET:
					case BP_VAR_IS:
						retval = &EG(uninitialized_zval_ptr);
						break;
					case BP_VAR_RW:
						zend_error(E_NOTICE,"Undefined index:  %R", ztype, offset_key);
						/* break missing intentionally */
					case BP_VAR_W: {
							zval *new_zval = &EG(uninitialized_zval);

							new_zval->refcount++;
							zend_u_symtable_update(ht, ztype, offset_key, offset_key_length + 1, &new_zval, sizeof(zval *), (void **) &retval);
						}
						break;
				}
			}
			if (free_offset) {
				efree(offset_key.v);
			}
			break;
		case IS_RESOURCE:
			zend_error(E_STRICT, "Resource ID#%ld used as offset, casting to integer (%ld)", Z_LVAL_P(dim), Z_LVAL_P(dim));
			/* Fall Through */
		case IS_DOUBLE:
		case IS_BOOL:
		case IS_LONG: {
				long index;

				if (Z_TYPE_P(dim) == IS_DOUBLE) {
					index = (long)Z_DVAL_P(dim);
				} else {
					index = Z_LVAL_P(dim);
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
	return retval;
}

static void zend_fetch_dimension_address(temp_variable *result, zval **container_ptr, zval *dim, int dim_is_tmp_var, int type TSRMLS_DC)
{
	zval *container;

	if (!container_ptr) {
		zend_error_noreturn(E_ERROR, "Cannot use string offset as an array");
	}

	container = *container_ptr;

	if (container == EG(error_zval_ptr)) {
		if (result) {
			result->var.ptr_ptr = &EG(error_zval_ptr);
			PZVAL_LOCK(*result->var.ptr_ptr);
			if (type == BP_VAR_R || type == BP_VAR_IS) {
				AI_USE_PTR(result->var);
			}
		}
		return;
	}

	if (Z_TYPE_P(container)==IS_NULL
		|| (Z_TYPE_P(container)==IS_BOOL && Z_LVAL_P(container)==0)
		|| (Z_TYPE_P(container)==IS_STRING && Z_STRLEN_P(container)==0)
		|| (Z_TYPE_P(container)==IS_UNICODE && Z_USTRLEN_P(container)==0)) {
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

	switch (Z_TYPE_P(container)) {
		zval **retval;

		case IS_ARRAY:
			if ((type==BP_VAR_W || type==BP_VAR_RW) && container->refcount>1 && !PZVAL_IS_REF(container)) {
				SEPARATE_ZVAL(container_ptr);
				container = *container_ptr;
			}
			if (dim == NULL) {
				zval *new_zval = &EG(uninitialized_zval);

				new_zval->refcount++;
				if (zend_hash_next_index_insert(Z_ARRVAL_P(container), &new_zval, sizeof(zval *), (void **) &retval) == FAILURE) {
					zend_error(E_WARNING, "Cannot add element to the array as the next element is already occupied");
					retval = &EG(error_zval_ptr);
					new_zval->refcount--;
				}
			} else {
				retval = zend_fetch_dimension_address_inner(Z_ARRVAL_P(container), dim, type TSRMLS_CC);
			}
			if (result) {
				result->var.ptr_ptr = retval;
			  PZVAL_LOCK(*result->var.ptr_ptr);
			}
			break;
		case IS_NULL: {
			/* for read-mode only */
			if (result) {
				result->var.ptr_ptr = &EG(uninitialized_zval_ptr);
				PZVAL_LOCK(*result->var.ptr_ptr);
			}
			if (type==BP_VAR_W || type==BP_VAR_RW) {
				zend_error(E_WARNING, "Cannot use a NULL value as an array");
			}
			break;
		}
		case IS_UNICODE:
		case IS_STRING: {
				zval tmp;

				if (dim == NULL) {
					zend_error_noreturn(E_ERROR, "[] operator not supported for strings");
				}

				if (Z_TYPE_P(dim) != IS_LONG) {
					switch(Z_TYPE_P(dim)) {
						/* case IS_LONG: */
						case IS_STRING:
						case IS_UNICODE:
						case IS_DOUBLE:
						case IS_NULL:
						case IS_BOOL:
							/* do nothing */
							break;
						default:
							zend_error(E_WARNING, "Illegal offset type");
							break;
					}

					tmp = *dim;
					zval_copy_ctor(&tmp);
					convert_to_long(&tmp);
					dim = &tmp;
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
				if (result) {
					container = *container_ptr;
					result->str_offset.str = container;
					PZVAL_LOCK(container);
					result->str_offset.offset = Z_LVAL_P(dim);
					result->var.ptr_ptr = NULL;
					if (type == BP_VAR_R || type == BP_VAR_IS) {
						AI_USE_PTR(result->var);
					}
				}
				return;
			}
			break;
		case IS_OBJECT:
			if (!Z_OBJ_HT_P(container)->read_dimension) {
				zend_error_noreturn(E_ERROR, "Cannot use object as array");
			} else {
				zval *overloaded_result;

				if (dim_is_tmp_var) {
					zval *orig = dim;
					MAKE_REAL_ZVAL_PTR(dim);
					ZVAL_NULL(orig);
				}
				overloaded_result = Z_OBJ_HT_P(container)->read_dimension(container, dim, type TSRMLS_CC);

				if (overloaded_result) {
					switch (type) {
						case BP_VAR_RW:
						case BP_VAR_W:
							if (Z_TYPE_P(overloaded_result) != IS_OBJECT
								&& !overloaded_result->is_ref) {
								zend_error_noreturn(E_ERROR, "Objects used as arrays in post/pre increment/decrement must return values by reference");
							}
							break;
					}

					retval = &overloaded_result;
				} else {
					retval = &EG(error_zval_ptr);
				}
				if (result) {
					result->var.ptr_ptr = retval;
					AI_USE_PTR(result->var);
					PZVAL_LOCK(*result->var.ptr_ptr);
				} else if ((*retval)->refcount == 0) {
					/* Destroy unused result from offsetGet() magic method */
					(*retval)->refcount = 1;
					zval_ptr_dtor(retval);
				}
				if (dim_is_tmp_var) {
					zval_ptr_dtor(&dim);
				}
				return;
			}
			break;
		default: {
				switch (type) {
					case BP_VAR_UNSET:
						zend_error(E_WARNING, "Cannot unset offset in a non-array variable");
						/* break missing intentionally */
					case BP_VAR_R:
					case BP_VAR_IS:
						retval = &EG(uninitialized_zval_ptr);
						break;
					default:
						retval = &EG(error_zval_ptr);
						break;
				}
				if (result) {
					result->var.ptr_ptr = retval;
					PZVAL_LOCK(*result->var.ptr_ptr);
				}
				if (type==BP_VAR_W || type==BP_VAR_RW) {
					zend_error(E_WARNING, "Cannot use a scalar value as an array");
				}
			}
			break;
	}
	if (result && (type == BP_VAR_R || type == BP_VAR_IS)) {
		AI_USE_PTR(result->var);
	}
}

static void zend_fetch_property_address(temp_variable *result, zval **container_ptr, zval *prop_ptr, int type TSRMLS_DC)
{
	zval *container;

	container = *container_ptr;
	if (container == EG(error_zval_ptr)) {
		if (result) {
			result->var.ptr_ptr = &EG(error_zval_ptr);
			PZVAL_LOCK(*result->var.ptr_ptr);
		}
		return;
	}
	/* this should modify object only if it's empty */
	if (Z_TYPE_P(container) == IS_NULL
		|| (Z_TYPE_P(container) == IS_BOOL && Z_LVAL_P(container)==0)
		|| (Z_TYPE_P(container) == IS_STRING && Z_STRLEN_P(container)==0)
		|| (Z_TYPE_P(container) == IS_UNICODE && Z_USTRLEN_P(container)==0)) {
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

	if (Z_TYPE_P(container) != IS_OBJECT) {
		if (result) {
			if (type == BP_VAR_R || type == BP_VAR_IS) {
				result->var.ptr_ptr = &EG(uninitialized_zval_ptr);
			} else {
				result->var.ptr_ptr = &EG(error_zval_ptr);
			}
			PZVAL_LOCK(*result->var.ptr_ptr);
		}
		return;
	}

	if (Z_OBJ_HT_P(container)->get_property_ptr_ptr) {
		zval **ptr_ptr = Z_OBJ_HT_P(container)->get_property_ptr_ptr(container, prop_ptr TSRMLS_CC);
		if (NULL == ptr_ptr) {
			zval *ptr;

			if (Z_OBJ_HT_P(container)->read_property &&
				(ptr = Z_OBJ_HT_P(container)->read_property(container, prop_ptr, type TSRMLS_CC)) != NULL) {
				if (result) {
					result->var.ptr = ptr;
					result->var.ptr_ptr = &result->var.ptr;
				}
			} else {
				zend_error(E_ERROR, "Cannot access undefined property for object with overloaded property access");
			}
		} else if (result) {
			result->var.ptr_ptr = ptr_ptr;
		}
	} else if (Z_OBJ_HT_P(container)->read_property) {
		if (result) {
			result->var.ptr = Z_OBJ_HT_P(container)->read_property(container, prop_ptr, type TSRMLS_CC);
			result->var.ptr_ptr = &result->var.ptr;
		}
	} else {
		zend_error(E_WARNING, "This object doesn't support property references");
		if (result) {
			result->var.ptr_ptr = &EG(error_zval_ptr);
		}
	}

	if (result) {
		PZVAL_LOCK(*result->var.ptr_ptr);
	}
}

static inline zend_brk_cont_element* zend_brk_cont(int nest_levels, int array_offset, zend_op_array *op_array, temp_variable *Ts TSRMLS_DC)
{
	int original_nest_levels;
	zend_brk_cont_element *jmp_to;

	original_nest_levels = nest_levels;
	do {
		if (array_offset==-1) {
			zend_error_noreturn(E_ERROR, "Cannot break/continue %d level%s", original_nest_levels, (original_nest_levels == 1) ? "" : "s");
		}
		jmp_to = &op_array->brk_cont_array[array_offset];
		if (nest_levels>1) {
			zend_op *brk_opline = &op_array->opcodes[jmp_to->brk];

			switch (brk_opline->opcode) {
				case ZEND_SWITCH_FREE:
					zend_switch_free(brk_opline, Ts TSRMLS_CC);
					break;
				case ZEND_FREE:
					zendi_zval_dtor(T(brk_opline->op1.u.var).tmp_var);
					break;
			}
		}
		array_offset = jmp_to->parent;
	} while (--nest_levels > 0);
	return jmp_to;
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

ZEND_API opcode_handler_t *zend_opcode_handlers;

ZEND_API void execute_internal(zend_execute_data *execute_data_ptr, int return_value_used TSRMLS_DC)
{
	zval **return_value_ptr = &(*(temp_variable *)((char *) execute_data_ptr->Ts + execute_data_ptr->opline->result.u.var)).var.ptr;
	((zend_internal_function *) execute_data_ptr->function_state.function)->handler(execute_data_ptr->opline->extended_value, *return_value_ptr, execute_data_ptr->function_state.function->common.return_reference?return_value_ptr:NULL, execute_data_ptr->object, return_value_used TSRMLS_CC);
}

#define ZEND_VM_NEXT_OPCODE() \
	CHECK_SYMBOL_TABLES() \
	EX(opline)++; \
	ZEND_VM_CONTINUE()

#define ZEND_VM_SET_OPCODE(new_op) \
	CHECK_SYMBOL_TABLES() \
	EX(opline) = new_op

#define ZEND_VM_JMP(new_op) \
     CHECK_SYMBOL_TABLES() \
 	   EX(opline) = EG(exception)?EX(opline)+1:new_op; \
     ZEND_VM_CONTINUE()

#define ZEND_VM_INC_OPCODE() \
	if (!EG(exception)) { \
		CHECK_SYMBOL_TABLES() \
		EX(opline)++; \
	}

#define ZEND_VM_RETURN_FROM_EXECUTE_LOOP() \
	free_alloca(EX(CVs)); \
	if (EX(op_array)->T < TEMP_VAR_STACK_LIMIT) { \
		free_alloca(EX(Ts)); \
	} else { \
		efree(EX(Ts)); \
	} \
	EG(in_execution) = EX(original_in_execution); \
	EG(current_execute_data) = EX(prev_execute_data); \
	ZEND_VM_RETURN()

#include "zend_vm_execute.h"

ZEND_API int zend_set_user_opcode_handler(zend_uchar opcode, opcode_handler_t handler)
{
	if (opcode != ZEND_USER_OPCODE) {
		zend_user_opcodes[opcode] = ZEND_USER_OPCODE;
		zend_user_opcode_handlers[opcode] = handler;
		return SUCCESS;
	}
	return FAILURE;
}

ZEND_API opcode_handler_t zend_get_user_opcode_handler(zend_uchar opcode)
{
	return zend_user_opcode_handlers[opcode];
}

ZEND_API zval *zend_get_zval_ptr(znode *node, temp_variable *Ts, zend_free_op *should_free, int type TSRMLS_DC) {
	return get_zval_ptr(node, Ts, should_free, type);
}

ZEND_API zval **zend_get_zval_ptr_ptr(znode *node, temp_variable *Ts, zend_free_op *should_free, int type TSRMLS_DC) {
	return get_zval_ptr_ptr(node, Ts, should_free, type);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
