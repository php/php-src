/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2008 Zend Technologies Ltd. (http://www.zend.com) |
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

static inline void zend_pzval_unlock_func(zval *z, zend_free_op *should_free, int unref TSRMLS_DC) /* {{{ */
{
	if (!Z_DELREF_P(z)) {
		Z_SET_REFCOUNT_P(z, 1);
		Z_UNSET_ISREF_P(z);
		should_free->var = z;
/*		should_free->is_var = 1; */
	} else {
		should_free->var = 0;
		if (unref && Z_ISREF_P(z) && Z_REFCOUNT_P(z) == 1) {
			Z_UNSET_ISREF_P(z);
		}
		GC_ZVAL_CHECK_POSSIBLE_ROOT(z);
	}
}
/* }}} */

static inline void zend_pzval_unlock_free_func(zval *z) /* {{{ */
{
	if (!Z_DELREF_P(z)) {
		GC_REMOVE_ZVAL_FROM_BUFFER(z);
		zval_dtor(z);
		safe_free_zval_ptr(z);
	}
}
/* }}} */

#define PZVAL_UNLOCK(z, f) zend_pzval_unlock_func(z, f, 1 TSRMLS_CC)
#define PZVAL_UNLOCK_EX(z, f, u) zend_pzval_unlock_func(z, f, u TSRMLS_CC)
#define PZVAL_UNLOCK_FREE(z) zend_pzval_unlock_free_func(z)
#define PZVAL_LOCK(z) Z_ADDREF_P((z))
#define RETURN_VALUE_UNUSED(pzn)	(((pzn)->u.EA.type & EXT_TYPE_UNUSED))
#define SELECTIVE_PZVAL_LOCK(pzv, pzn)	if (!RETURN_VALUE_UNUSED(pzn)) { PZVAL_LOCK(pzv); }

#define AI_USE_PTR(ai) \
	if ((ai).ptr_ptr) { \
		(ai).ptr = *((ai).ptr_ptr); \
		(ai).ptr_ptr = &((ai).ptr); \
	} else { \
		(ai).ptr = NULL; \
	}

#define AI_SET_PTR(ai, val)		\
	(ai).ptr = (val);			\
	(ai).ptr_ptr = &((ai).ptr);

#define FREE_OP(should_free) \
	if (should_free.var) { \
		if ((zend_uintptr_t)should_free.var & 1L) { \
			zval_dtor((zval*)((zend_uintptr_t)should_free.var & ~1L)); \
		} else { \
			zval_ptr_dtor(&should_free.var); \
		} \
	}

#define FREE_OP_IF_VAR(should_free) \
	if (should_free.var != NULL && (((zend_uintptr_t)should_free.var & 1L) == 0)) { \
		zval_ptr_dtor(&should_free.var); \
	}

#define FREE_OP_VAR_PTR(should_free) \
	if (should_free.var) { \
		zval_ptr_dtor(&should_free.var); \
	}

#define TMP_FREE(z) (zval*)(((zend_uintptr_t)(z)) | 1L)

#define IS_TMP_FREE(should_free) ((zend_uintptr_t)should_free.var & 1L)

#define INIT_PZVAL_COPY(z,v) \
	(z)->value = (v)->value; \
	Z_TYPE_P(z) = Z_TYPE_P(v); \
	Z_SET_REFCOUNT_P((z), 1); \
	Z_UNSET_ISREF_P(z);

#define MAKE_REAL_ZVAL_PTR(val) \
	do { \
		zval *_tmp; \
		ALLOC_ZVAL(_tmp); \
		_tmp->value = (val)->value; \
		Z_TYPE_P(_tmp) = Z_TYPE_P(val); \
		Z_SET_REFCOUNT_P(_tmp, 1); \
		Z_UNSET_ISREF_P(_tmp); \
		val = _tmp; \
	} while (0)

/* End of zend_execute_locks.h */

#define CV_OF(i)     (EG(current_execute_data)->CVs[i])
#define CV_DEF_OF(i) (EG(active_op_array)->vars[i])

#define CTOR_CALL_BIT    0x1
#define CTOR_USED_BIT    0x2

#define IS_CTOR_CALL(ce) (((zend_uintptr_t)(ce)) & CTOR_CALL_BIT)
#define IS_CTOR_USED(ce) (((zend_uintptr_t)(ce)) & CTOR_USED_BIT)

#define ENCODE_CTOR(ce, used) \
	((zend_class_entry*)(((zend_uintptr_t)(ce)) | CTOR_CALL_BIT | ((used) ? CTOR_USED_BIT : 0)))
#define DECODE_CTOR(ce) \
	((zend_class_entry*)(((zend_uintptr_t)(ce)) & ~(CTOR_CALL_BIT|CTOR_USED_BIT)))

ZEND_API zval** zend_get_compiled_variable_value(zend_execute_data *execute_data_ptr, zend_uint var) /* {{{ */
{
	return execute_data_ptr->CVs[var];
}
/* }}} */

static inline void zend_get_cv_address(zend_uchar utype, zend_compiled_variable *cv, zval ***ptr, temp_variable *Ts TSRMLS_DC) /* {{{ */
{
	zval *new_zval = &EG(uninitialized_zval);

	Z_ADDREF_P(new_zval);
	zend_u_hash_quick_update(EG(active_symbol_table), utype, cv->name, cv->name_len+1, cv->hash_value, &new_zval, sizeof(zval *), (void **)ptr);
}
/* }}} */

static inline zval *_get_zval_ptr_tmp(znode *node, temp_variable *Ts, zend_free_op *should_free TSRMLS_DC) /* {{{ */
{
	return should_free->var = &T(node->u.var).tmp_var;
}
/* }}} */

static inline zval *_get_zval_ptr_var(znode *node, temp_variable *Ts, zend_free_op *should_free TSRMLS_DC) /* {{{ */
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
				Z_STRVAL_P(ptr) = estrndup( Z_STRVAL_P(str) + T->str_offset.offset, 1);
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
		Z_SET_REFCOUNT_P(ptr, 1);
		Z_SET_ISREF_P(ptr);
		return ptr;
	}
}
/* }}} */

static inline zval *_get_zval_ptr_cv(znode *node, temp_variable *Ts, int type TSRMLS_DC) /* {{{ */
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
/* }}} */

static inline zval *_get_zval_ptr(znode *node, temp_variable *Ts, zend_free_op *should_free, int type TSRMLS_DC) /* {{{ */
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
/* }}} */

static inline zval **_get_zval_ptr_ptr_var(znode *node, temp_variable *Ts, zend_free_op *should_free TSRMLS_DC) /* {{{ */
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
/* }}} */

static inline zval **_get_zval_ptr_ptr_cv(znode *node, temp_variable *Ts, int type TSRMLS_DC) /* {{{ */
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
/* }}} */

static inline zval **_get_zval_ptr_ptr(znode *node, temp_variable *Ts, zend_free_op *should_free, int type TSRMLS_DC) /* {{{ */
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
/* }}} */

static inline zval *_get_obj_zval_ptr_unused(TSRMLS_D) /* {{{ */
{
	if (EG(This)) {
		return EG(This);
	} else {
		zend_error_noreturn(E_ERROR, "Using $this when not in object context");
		return NULL;
	}
}
/* }}} */

static inline zval **_get_obj_zval_ptr_ptr(znode *op, temp_variable *Ts, zend_free_op *should_free, int type TSRMLS_DC) /* {{{ */
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
/* }}} */

static inline zval **_get_obj_zval_ptr_ptr_unused(TSRMLS_D) /* {{{ */
{
	if (EG(This)) {
		return &EG(This);
	} else {
		zend_error_noreturn(E_ERROR, "Using $this when not in object context");
		return NULL;
	}
}
/* }}} */

static inline zval *_get_obj_zval_ptr(znode *op, temp_variable *Ts, zend_free_op *should_free, int type TSRMLS_DC) /* {{{ */
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
/* }}} */

static inline void zend_switch_free(temp_variable *T, int type, int extended_value TSRMLS_DC) /* {{{ */
{
	if (type == IS_VAR) {
		if (T->var.ptr) {
			if (extended_value & ZEND_FE_RESET_VARIABLE) { /* foreach() free */
				Z_DELREF_P(T->var.ptr);
			}
			zval_ptr_dtor(&T->var.ptr);
		} else if (!T->var.ptr_ptr) {
			/* perform the equivalent of equivalent of a
			 * quick & silent get_zval_ptr, and FREE_OP
			 */
			PZVAL_UNLOCK_FREE(T->str_offset.str);
		}
	} else { /* IS_TMP_VAR */
		zendi_zval_dtor(T->tmp_var);
	}
}
/* }}} */

static void zend_assign_to_variable_reference(zval **variable_ptr_ptr, zval **value_ptr_ptr TSRMLS_DC) /* {{{ */
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
			Z_DELREF_P(value_ptr);
			if (Z_REFCOUNT_P(value_ptr) > 0) {
				ALLOC_ZVAL(*value_ptr_ptr);
				**value_ptr_ptr = *value_ptr;
				value_ptr = *value_ptr_ptr;
				zendi_zval_copy_ctor(*value_ptr);
			}
			Z_SET_REFCOUNT_P(value_ptr, 1);
			Z_SET_ISREF_P(value_ptr);
		}

		*variable_ptr_ptr = value_ptr;
		Z_ADDREF_P(value_ptr);

		zval_ptr_dtor(&variable_ptr);
	} else if (!Z_ISREF_P(variable_ptr)) {
		if (variable_ptr_ptr == value_ptr_ptr) {
			SEPARATE_ZVAL(variable_ptr_ptr);
		} else if (variable_ptr==EG(uninitialized_zval_ptr)
			|| Z_REFCOUNT_P(variable_ptr) > 2) {
			/* we need to separate */
			Z_DELREF_P(variable_ptr);
			Z_DELREF_P(variable_ptr);
			ALLOC_ZVAL(*variable_ptr_ptr);
			**variable_ptr_ptr = *variable_ptr;
			zval_copy_ctor(*variable_ptr_ptr);
			*value_ptr_ptr = *variable_ptr_ptr;
			Z_SET_REFCOUNT_P((*variable_ptr_ptr), 2);
		}
		Z_SET_ISREF_PP(variable_ptr_ptr);
	}
}
/* }}} */

/* this should modify object only if it's empty */
static inline void make_real_object(zval **object_ptr TSRMLS_DC) /* {{{ */
{
	if (Z_TYPE_PP(object_ptr) == IS_NULL
		|| (Z_TYPE_PP(object_ptr) == IS_BOOL && Z_LVAL_PP(object_ptr) == 0)
		|| (Z_TYPE_PP(object_ptr) == IS_STRING && Z_STRLEN_PP(object_ptr) == 0)
		|| (Z_TYPE_PP(object_ptr) == IS_UNICODE && Z_USTRLEN_PP(object_ptr) == 0)
	) {
		zend_error(E_STRICT, "Creating default object from empty value");
		SEPARATE_ZVAL_IF_NOT_REF(object_ptr);
		zval_dtor(*object_ptr);
		object_init(*object_ptr);
	}
}
/* }}} */

static inline char * zend_verify_arg_class_kind(zend_arg_info *cur_arg_info, ulong fetch_type, zstr *class_name, zend_class_entry **pce TSRMLS_DC) /* {{{ */
{
	*pce = zend_u_fetch_class(UG(unicode) ? IS_UNICODE : IS_STRING, cur_arg_info->class_name, cur_arg_info->class_name_len, (fetch_type | ZEND_FETCH_CLASS_AUTO | ZEND_FETCH_CLASS_NO_AUTOLOAD) TSRMLS_CC);

	*class_name = (*pce) ? (*pce)->name: cur_arg_info->class_name;
	if (*pce && (*pce)->ce_flags & ZEND_ACC_INTERFACE) {
		return "implement interface ";
	} else {
		return "be an instance of ";
	}
}
/* }}} */

static inline int zend_verify_arg_error(zend_function *zf, zend_uint arg_num, zend_arg_info *cur_arg_info, char *need_msg, zstr need_kind, char *given_msg, zstr given_kind TSRMLS_DC) /* {{{ */
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
/* }}} */

static inline int zend_verify_arg_type(zend_function *zf, zend_uint arg_num, zval *arg, ulong fetch_type TSRMLS_DC) /* {{{ */
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
        zstr class_name;

        if (!arg) {
            need_msg = zend_verify_arg_class_kind(cur_arg_info, fetch_type, &class_name, &ce TSRMLS_CC);
            return zend_verify_arg_error(zf, arg_num, cur_arg_info, need_msg, class_name, "none", EMPTY_ZSTR TSRMLS_CC);
        }
        if (Z_TYPE_P(arg) == IS_OBJECT) {
            need_msg = zend_verify_arg_class_kind(cur_arg_info, fetch_type, &class_name, &ce TSRMLS_CC);
            if (!ce || !instanceof_function(Z_OBJCE_P(arg), ce TSRMLS_CC)) {
                return zend_verify_arg_error(zf, arg_num, cur_arg_info, need_msg, class_name, "instance of ", Z_OBJCE_P(arg)->name TSRMLS_CC);
            }
        } else if (Z_TYPE_P(arg) != IS_NULL || !cur_arg_info->allow_null) {
            need_msg = zend_verify_arg_class_kind(cur_arg_info, fetch_type, &class_name, &ce TSRMLS_CC);
            return zend_verify_arg_error(zf, arg_num, cur_arg_info, need_msg, class_name, zend_zval_type_name(arg), EMPTY_ZSTR TSRMLS_CC);
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
/* }}} */

static inline void zend_assign_to_object(znode *result, zval **object_ptr, zval *property_name, znode *value_op, temp_variable *Ts, int opcode TSRMLS_DC) /* {{{ */
{
	zval *object;
	zend_free_op free_value;
	zval *value = get_zval_ptr(value_op, Ts, &free_value, BP_VAR_R);
	zval **retval = &T(result->u.var).var.ptr;

	if (!object_ptr) {
		zend_error_noreturn(E_ERROR, "Cannot use string offset as an array");
	}

	if (Z_TYPE_P(*object_ptr) != IS_OBJECT) {
		if (*object_ptr == EG(error_zval_ptr)) {
			if (!RETURN_VALUE_UNUSED(result)) {
				*retval = EG(uninitialized_zval_ptr);
				PZVAL_LOCK(*retval);
			}
			FREE_OP(free_value);
			return;
		}
		if (Z_TYPE_PP(object_ptr) == IS_NULL ||
		    (Z_TYPE_PP(object_ptr) == IS_BOOL && Z_LVAL_PP(object_ptr) == 0) ||
		    (Z_TYPE_PP(object_ptr) == IS_STRING && Z_STRLEN_PP(object_ptr) == 0) ||
		    (Z_TYPE_PP(object_ptr) == IS_UNICODE && Z_USTRLEN_PP(object_ptr) == 0)) {
			zend_error(E_STRICT, "Creating default object from empty value");
			SEPARATE_ZVAL_IF_NOT_REF(object_ptr);
			zval_dtor(*object_ptr);
			object_init(*object_ptr);
		} else {
			zend_error(E_WARNING, "Attempt to assign property of non-object");
			if (!RETURN_VALUE_UNUSED(result)) {
				*retval = EG(uninitialized_zval_ptr);
				PZVAL_LOCK(*retval);
			}
			FREE_OP(free_value);
			return;
		}
	}

 	/* here we are sure we are dealing with an object */
 	object = *object_ptr;

	/* separate our value if necessary */
	if (value_op->op_type == IS_TMP_VAR) {
		zval *orig_value = value;

		ALLOC_ZVAL(value);
		*value = *orig_value;
		Z_UNSET_ISREF_P(value);
		Z_SET_REFCOUNT_P(value, 0);
	} else if (value_op->op_type == IS_CONST) {
		zval *orig_value = value;

		ALLOC_ZVAL(value);
		*value = *orig_value;
		Z_UNSET_ISREF_P(value);
		Z_SET_REFCOUNT_P(value, 0);
		zval_copy_ctor(value);
	}


	Z_ADDREF_P(value);
	if (opcode == ZEND_ASSIGN_OBJ) {
		if (!Z_OBJ_HT_P(object)->write_property) {
			zend_error(E_WARNING, "Attempt to assign property of non-object");
			if (!RETURN_VALUE_UNUSED(result)) {
				*retval = EG(uninitialized_zval_ptr);
				PZVAL_LOCK(*retval);
			}
			FREE_OP(free_value);
			return;
		}
		Z_OBJ_HT_P(object)->write_property(object, property_name, value TSRMLS_CC);
	} else {
		/* Note:  property_name in this case is really the array index! */
		if (!Z_OBJ_HT_P(object)->write_dimension) {
			zend_error_noreturn(E_ERROR, "Cannot use object as array");
		}
		Z_OBJ_HT_P(object)->write_dimension(object, property_name, value TSRMLS_CC);
	}

	if (!RETURN_VALUE_UNUSED(result) && !EG(exception)) {
		AI_SET_PTR(T(result->u.var).var, value);
		PZVAL_LOCK(value);
	}
	zval_ptr_dtor(&value);
	FREE_OP_IF_VAR(free_value);
}
/* }}} */

static inline void zend_assign_to_string_offset(temp_variable *T, zval *value, int value_type TSRMLS_DC) /* {{{ */
{
	if (UG(unicode) && Z_TYPE_P(T->str_offset.str) == IS_STRING && Z_TYPE_P(value) != IS_STRING) {
		convert_to_unicode(T->str_offset.str);
	}

	if (Z_TYPE_P(T->str_offset.str) == IS_STRING) {

		if (((int)T->str_offset.offset < 0)) {
			zend_error(E_WARNING, "Illegal string offset:  %d", T->str_offset.offset);
			return;
		}

		if (T->str_offset.offset >= Z_STRLEN_P(T->str_offset.str)) {
			Z_STRVAL_P(T->str_offset.str) = (char *) erealloc(Z_STRVAL_P(T->str_offset.str), T->str_offset.offset+1+1);
			memset(Z_STRVAL_P(T->str_offset.str) + Z_STRLEN_P(T->str_offset.str),
			       ' ',
			       T->str_offset.offset - Z_STRLEN_P(T->str_offset.str));
			Z_STRVAL_P(T->str_offset.str)[T->str_offset.offset+1] = 0;
			Z_STRLEN_P(T->str_offset.str) = T->str_offset.offset+1;
		}

		if (Z_TYPE_P(value)!=IS_STRING) {
			zval tmp = *value;
			if (value_type != IS_TMP_VAR) {
				zval_copy_ctor(&tmp);
			}
			convert_to_string(&tmp);
			Z_STRVAL_P(T->str_offset.str)[T->str_offset.offset] = Z_STRVAL(tmp)[0];
			STR_FREE(Z_STRVAL(tmp));
		} else {
			Z_STRVAL_P(T->str_offset.str)[T->str_offset.offset] = Z_STRVAL_P(value)[0];
			if (value_type == IS_TMP_VAR) {
				/* we can safely free final_value here
				 * because separation is done only
				 * in case value_type == IS_VAR */
				STR_FREE(Z_STRVAL_P(value));
			}
		}
		/*
		 * the value of an assignment to a string offset is undefined
		T(result->u.var).var = &T->str_offset.str;
		*/
	} else if (Z_TYPE_P(T->str_offset.str) == IS_UNICODE) {

		if (((int)T->str_offset.offset < 0)) {
			zend_error(E_WARNING, "Illegal string offset:  %d", T->str_offset.offset);
			return;
		}

		if (T->str_offset.offset >= Z_USTRLEN_P(T->str_offset.str)) {
			Z_USTRVAL_P(T->str_offset.str) = (UChar *) eurealloc(Z_USTRVAL_P(T->str_offset.str), T->str_offset.offset+1+1);
			u_memset(Z_USTRVAL_P(T->str_offset.str) + Z_USTRLEN_P(T->str_offset.str),
			       ' ',
			       T->str_offset.offset - Z_USTRLEN_P(T->str_offset.str));
			Z_USTRVAL_P(T->str_offset.str)[T->str_offset.offset+1] = 0;
			Z_USTRLEN_P(T->str_offset.str) = T->str_offset.offset+1;
		}

		if (Z_TYPE_P(value) != IS_UNICODE) {
			zval tmp = *value;
			if (value_type != IS_TMP_VAR) {
				zval_copy_ctor(&tmp);
			}
			convert_to_unicode(&tmp);
			Z_USTRVAL_P(T->str_offset.str)[T->str_offset.offset] = Z_USTRVAL(tmp)[0];
			USTR_FREE(Z_USTRVAL(tmp));
		} else {
			Z_USTRVAL_P(T->str_offset.str)[T->str_offset.offset] = Z_USTRVAL_P(value)[0];
			if (value_type == IS_TMP_VAR) {
				/* we can safely free final_value here
				 * because separation is done only
				 * in case value_type == IS_VAR */
				USTR_FREE(Z_USTRVAL_P(value));
			}
		}
	}
}
/* }}} */

static inline zval* zend_assign_to_variable(zval **variable_ptr_ptr, zval *value, int is_tmp_var TSRMLS_DC) /* {{{ */
{
	zval *variable_ptr = *variable_ptr_ptr;
	zval garbage;

	if (variable_ptr == EG(error_zval_ptr)) {
		if (is_tmp_var) {
			zval_dtor(value);
		}
		return EG(uninitialized_zval_ptr);
	}

	if (Z_TYPE_P(variable_ptr) == IS_OBJECT && Z_OBJ_HANDLER_P(variable_ptr, set)) {
		Z_OBJ_HANDLER_P(variable_ptr, set)(variable_ptr_ptr, value TSRMLS_CC);
		return variable_ptr;
	}

	if (PZVAL_IS_REF(variable_ptr)) {
		if (variable_ptr!=value) {
			zend_uint refcount = Z_REFCOUNT_P(variable_ptr);

			if (!is_tmp_var) {
				Z_ADDREF_P(value);
			}
			garbage = *variable_ptr;
			*variable_ptr = *value;
			Z_SET_REFCOUNT_P(variable_ptr, refcount);
			Z_SET_ISREF_P(variable_ptr);
			if (!is_tmp_var) {
				zendi_zval_copy_ctor(*variable_ptr);
				Z_DELREF_P(value);
			}
			zendi_zval_dtor(garbage);
			return variable_ptr;
		}
	} else {
		if (Z_DELREF_P(variable_ptr) == 0) {
			if (!is_tmp_var) {
				if (variable_ptr==value) {
					Z_ADDREF_P(variable_ptr);
				} else if (PZVAL_IS_REF(value)) {
					garbage = *variable_ptr;
					*variable_ptr = *value;
					INIT_PZVAL(variable_ptr);
					zval_copy_ctor(variable_ptr);
					zendi_zval_dtor(garbage);
					return variable_ptr;
				} else {
					Z_ADDREF_P(value);
					*variable_ptr_ptr = value;
					GC_REMOVE_ZVAL_FROM_BUFFER(variable_ptr);
					zendi_zval_dtor(*variable_ptr);
					safe_free_zval_ptr(variable_ptr);
					return value;
				}
			} else {
				garbage = *variable_ptr;
				*variable_ptr = *value;
				INIT_PZVAL(variable_ptr);
				zendi_zval_dtor(garbage);
				return variable_ptr;
			}
		} else { /* we need to split */
			if (!is_tmp_var) {
				if (PZVAL_IS_REF(value) && Z_REFCOUNT_P(value) > 0) {
					ALLOC_ZVAL(variable_ptr);
					*variable_ptr_ptr = variable_ptr;
					*variable_ptr = *value;
					zval_copy_ctor(variable_ptr);
					Z_SET_REFCOUNT_P(variable_ptr, 1);
				} else {
					*variable_ptr_ptr = value;
					Z_ADDREF_P(value);
				}
			} else {
				ALLOC_ZVAL(*variable_ptr_ptr);
				Z_SET_REFCOUNT_P(value, 1);
				**variable_ptr_ptr = *value;
			}
		}
		Z_UNSET_ISREF_PP(variable_ptr_ptr);
	}

	return *variable_ptr_ptr;
}
/* }}} */

static inline void zend_receive(zval **variable_ptr_ptr, zval *value TSRMLS_DC) /* {{{ */
{
	Z_DELREF_PP(variable_ptr_ptr);
	*variable_ptr_ptr = value;
	Z_ADDREF_P(value);
}
/* }}} */

/* Utility Functions for Extensions */
static void zend_extension_statement_handler(zend_extension *extension, zend_op_array *op_array TSRMLS_DC) /* {{{ */
{
	if (extension->statement_handler) {
		extension->statement_handler(op_array);
	}
}
/* }}} */

static void zend_extension_fcall_begin_handler(zend_extension *extension, zend_op_array *op_array TSRMLS_DC) /* {{{ */
{
	if (extension->fcall_begin_handler) {
		extension->fcall_begin_handler(op_array);
	}
}
/* }}} */

static void zend_extension_fcall_end_handler(zend_extension *extension, zend_op_array *op_array TSRMLS_DC) /* {{{ */
{
	if (extension->fcall_end_handler) {
		extension->fcall_end_handler(op_array);
	}
}
/* }}} */

static inline HashTable *zend_get_target_symbol_table(zend_op *opline, temp_variable *Ts, int type, zval *variable TSRMLS_DC) /* {{{ */
{
	switch (opline->op2.u.EA.type) {
		case ZEND_FETCH_LOCAL:
			return EG(active_symbol_table);
			break;
		case ZEND_FETCH_GLOBAL:
		case ZEND_FETCH_GLOBAL_LOCK:
		case ZEND_FETCH_AUTO_GLOBAL:
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
/* }}} */

static inline zval **zend_fetch_dimension_address_inner(HashTable *ht, zval *dim, int type TSRMLS_DC) /* {{{ */
{
	zval **retval;
	zstr offset_key;
	int offset_key_length;
	zend_uchar ztype = Z_TYPE_P(dim);
	int free_offset = 0;

	switch (ztype) {
		case IS_NULL:
			ztype = ZEND_STR_TYPE;
			offset_key = EMPTY_ZSTR;
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

							Z_ADDREF_P(new_zval);
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

							Z_ADDREF_P(new_zval);
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
/* }}} */

static void zend_fetch_dimension_address(temp_variable *result, zval **container_ptr, zval *dim, int dim_is_tmp_var, int type TSRMLS_DC) /* {{{ */
{
	zval *container;
	zval **retval;

	if (!container_ptr) {
		zend_error_noreturn(E_ERROR, "Cannot use string offset as an array");
	}

	container = *container_ptr;

	switch (Z_TYPE_P(container)) {

		case IS_ARRAY:
			if (type != BP_VAR_UNSET && Z_REFCOUNT_P(container) > 1 && !PZVAL_IS_REF(container)) {
				SEPARATE_ZVAL(container_ptr);
				container = *container_ptr;
			}
fetch_from_array:
			if (dim == NULL) {
				zval *new_zval = &EG(uninitialized_zval);

				Z_ADDREF_P(new_zval);
				if (zend_hash_next_index_insert(Z_ARRVAL_P(container), &new_zval, sizeof(zval *), (void **) &retval) == FAILURE) {
					zend_error(E_WARNING, "Cannot add element to the array as the next element is already occupied");
					retval = &EG(error_zval_ptr);
					Z_DELREF_P(new_zval);
				}
			} else {
				retval = zend_fetch_dimension_address_inner(Z_ARRVAL_P(container), dim, type TSRMLS_CC);
			}
			if (result) {
				result->var.ptr_ptr = retval;
				PZVAL_LOCK(*retval);
			}
			return;
			break;

		case IS_NULL:
			if (container == EG(error_zval_ptr)) {
				if (result) {
					result->var.ptr_ptr = &EG(error_zval_ptr);
					PZVAL_LOCK(EG(error_zval_ptr));
				}
			} else if (type != BP_VAR_UNSET) {
convert_to_array:
				if (!PZVAL_IS_REF(container)) {
					SEPARATE_ZVAL(container_ptr);
					container = *container_ptr;
				}
				zval_dtor(container);
				array_init(container);
				goto fetch_from_array;
			} else if (result) {
				/* for read-mode only */
				result->var.ptr_ptr = &EG(uninitialized_zval_ptr);
				PZVAL_LOCK(EG(uninitialized_zval_ptr));
			}
			return;
			break;

		case IS_UNICODE:
		case IS_STRING: {
				zval tmp;

				if (type != BP_VAR_UNSET && Z_UNILEN_P(container)==0) {
					goto convert_to_array;
				}
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
				if (type != BP_VAR_UNSET) {
					SEPARATE_ZVAL_IF_NOT_REF(container_ptr);
				}
				if (result) {
					container = *container_ptr;
					result->str_offset.str = container;
					PZVAL_LOCK(container);
					result->str_offset.offset = Z_LVAL_P(dim);
					result->var.ptr_ptr = NULL;
					result->var.ptr = NULL;
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
					if (!Z_ISREF_P(overloaded_result)) {
						if (Z_REFCOUNT_P(overloaded_result) > 0) {
							zval *tmp = overloaded_result;

							ALLOC_ZVAL(overloaded_result);
							*overloaded_result = *tmp;
							zval_copy_ctor(overloaded_result);
							Z_UNSET_ISREF_P(overloaded_result);
							Z_SET_REFCOUNT_P(overloaded_result, 0);
						}
						if (Z_TYPE_P(overloaded_result) != IS_OBJECT) {
							zend_class_entry *ce = Z_OBJCE_P(container);
							zend_error(E_NOTICE, "Indirect modification of overloaded element of %v has no effect", ce->name);
						}
					}
					retval = &overloaded_result;
				} else {
					retval = &EG(error_zval_ptr);
				}
				if (result) {
					AI_SET_PTR(result->var, *retval);
					PZVAL_LOCK(*retval);
				} else if (Z_REFCOUNT_PP(retval) == 0) {
					/* Destroy unused result from offsetGet() magic method */
					Z_SET_REFCOUNT_P((*retval), 1);
					zval_ptr_dtor(retval);
				}
				if (dim_is_tmp_var) {
					zval_ptr_dtor(&dim);
				}
				return;
			}
			return;
			break;

		case IS_BOOL:
			if (type != BP_VAR_UNSET && Z_LVAL_P(container)==0) {
				goto convert_to_array;
			}
			/* break missing intentionally */

		default:
			if (type == BP_VAR_UNSET) {
				zend_error(E_WARNING, "Cannot unset offset in a non-array variable");
				if (result) {
					AI_SET_PTR(result->var, EG(uninitialized_zval_ptr));
					PZVAL_LOCK(EG(uninitialized_zval_ptr));
				}
			} else {
				zend_error(E_WARNING, "Cannot use a scalar value as an array");
				if (result) {
					result->var.ptr_ptr = &EG(error_zval_ptr);
					PZVAL_LOCK(EG(error_zval_ptr));
				}
			}
			break;
	}
}
/* }}} */

static void zend_fetch_dimension_address_read(temp_variable *result, zval **container_ptr, zval *dim, int dim_is_tmp_var, int type TSRMLS_DC) /* {{{ */
{
	zval *container;
	zval **retval;

	if (!container_ptr) {
		zend_error_noreturn(E_ERROR, "Cannot use string offset as an array");
	}

	container = *container_ptr;

	switch (Z_TYPE_P(container)) {

		case IS_ARRAY:
			if (dim == NULL) {
				zval *new_zval = &EG(uninitialized_zval);

				Z_ADDREF_P(new_zval);
				if (zend_hash_next_index_insert(Z_ARRVAL_P(container), &new_zval, sizeof(zval *), (void **) &retval) == FAILURE) {
					zend_error(E_WARNING, "Cannot add element to the array as the next element is already occupied");
					retval = &EG(error_zval_ptr);
					Z_DELREF_P(new_zval);
				}
			} else {
				retval = zend_fetch_dimension_address_inner(Z_ARRVAL_P(container), dim, type TSRMLS_CC);
			}
			if (result) {
				AI_SET_PTR(result->var, *retval);
				PZVAL_LOCK(*retval);
			}
			return;
			break;

		case IS_NULL:
			if (container == EG(error_zval_ptr)) {
				if (result) {
					AI_SET_PTR(result->var, EG(error_zval_ptr));
					PZVAL_LOCK(EG(error_zval_ptr));
				}
			} else if (result) {
				/* for read-mode only */
				AI_SET_PTR(result->var, EG(uninitialized_zval_ptr));
				PZVAL_LOCK(EG(uninitialized_zval_ptr));
			}
			return;
			break;

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
				if (result) {
					container = *container_ptr;
					result->str_offset.str = container;
					PZVAL_LOCK(container);
					result->str_offset.offset = Z_LVAL_P(dim);
					result->var.ptr_ptr = NULL;
					result->var.ptr = NULL;
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
					if (result) {
						AI_SET_PTR(result->var, overloaded_result);
						PZVAL_LOCK(overloaded_result);
					} else if (Z_REFCOUNT_P(overloaded_result) == 0) {
						/* Destroy unused result from offsetGet() magic method */
						Z_SET_REFCOUNT_P(overloaded_result, 1);
						zval_ptr_dtor(&overloaded_result);
					}
				} else if (result) {
					AI_SET_PTR(result->var, EG(error_zval_ptr));
					PZVAL_LOCK(EG(error_zval_ptr));
				}
				if (dim_is_tmp_var) {
					zval_ptr_dtor(&dim);
				}
				return;
			}
			return;
			break;

		default:
			if (result) {
				AI_SET_PTR(result->var, EG(uninitialized_zval_ptr));
				PZVAL_LOCK(EG(uninitialized_zval_ptr));
			}
			return;
			break;
	}
}
/* }}} */

static void zend_fetch_property_address(temp_variable *result, zval **container_ptr, zval *prop_ptr, int type TSRMLS_DC) /* {{{ */
{
	zval *container;

	if (!container_ptr) {
		zend_error_noreturn(E_ERROR, "Cannot use string offset as an object");
	}

	container = *container_ptr;
	if (Z_TYPE_P(container) != IS_OBJECT) {
		if (container == EG(error_zval_ptr)) {
			if (result) {
				result->var.ptr_ptr = &EG(error_zval_ptr);
				PZVAL_LOCK(*result->var.ptr_ptr);
			}
			return;
		}
	
		/* this should modify object only if it's empty */
		if (type != BP_VAR_UNSET &&
		    (Z_TYPE_P(container) == IS_NULL ||
		     (Z_TYPE_P(container) == IS_BOOL && Z_LVAL_P(container)==0) ||
		     (Z_TYPE_P(container) == IS_STRING && Z_STRLEN_P(container)==0) ||
		     (Z_TYPE_P(container) == IS_UNICODE && Z_USTRLEN_P(container)==0))) {
			if (!PZVAL_IS_REF(container)) {
				SEPARATE_ZVAL(container_ptr);
				container = *container_ptr;
			}
			object_init(container);
		} else {
			if (result) {
 				result->var.ptr_ptr = &EG(error_zval_ptr);
				PZVAL_LOCK(EG(error_zval_ptr));
			}
 			return;
		}
	}

	if (Z_OBJ_HT_P(container)->get_property_ptr_ptr) {
		zval **ptr_ptr = Z_OBJ_HT_P(container)->get_property_ptr_ptr(container, prop_ptr TSRMLS_CC);
		if (NULL == ptr_ptr) {
			zval *ptr;

			if (Z_OBJ_HT_P(container)->read_property &&
				(ptr = Z_OBJ_HT_P(container)->read_property(container, prop_ptr, type TSRMLS_CC)) != NULL) {
				if (result) {
					AI_SET_PTR(result->var, ptr);
					PZVAL_LOCK(ptr);
				}
			} else {
				zend_error(E_ERROR, "Cannot access undefined property for object with overloaded property access");
			}
		} else if (result) {
			result->var.ptr_ptr = ptr_ptr;
			PZVAL_LOCK(*ptr_ptr);
		}
	} else if (Z_OBJ_HT_P(container)->read_property) {
		zval *ptr = Z_OBJ_HT_P(container)->read_property(container, prop_ptr, type TSRMLS_CC);

		if (result) {
			AI_SET_PTR(result->var, ptr);
			PZVAL_LOCK(ptr);
		}
	} else {
		zend_error(E_WARNING, "This object doesn't support property references");
		if (result) {
			result->var.ptr_ptr = &EG(error_zval_ptr);
			PZVAL_LOCK(EG(error_zval_ptr));
		}
	}
}
/* }}} */

static inline zend_brk_cont_element* zend_brk_cont(int nest_levels, int array_offset, zend_op_array *op_array, temp_variable *Ts TSRMLS_DC) /* {{{ */
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
					zend_switch_free(&T(brk_opline->op1.u.var), brk_opline->op1.op_type, brk_opline->extended_value TSRMLS_CC);
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
/* }}} */

#if ZEND_INTENSIVE_DEBUGGING

#define CHECK_SYMBOL_TABLES()														\
	zend_hash_apply(&EG(symbol_table), (apply_func_t) zend_check_symbol TSRMLS_CC);	\
	if (&EG(symbol_table)!=EG(active_symbol_table)) {								\
		zend_hash_apply(EG(active_symbol_table), (apply_func_t) zend_check_symbol TSRMLS_CC);	\
	}

static int zend_check_symbol(zval **pz TSRMLS_DC) /* {{{ */
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
/* }}} */

#else
#define CHECK_SYMBOL_TABLES()
#endif

ZEND_API opcode_handler_t *zend_opcode_handlers;

ZEND_API void execute_internal(zend_execute_data *execute_data_ptr, int return_value_used TSRMLS_DC) /* {{{ */
{
	zval **return_value_ptr = &(*(temp_variable *)((char *) execute_data_ptr->Ts + execute_data_ptr->opline->result.u.var)).var.ptr;
	((zend_internal_function *) execute_data_ptr->function_state.function)->handler(execute_data_ptr->opline->extended_value, *return_value_ptr, return_value_ptr, execute_data_ptr->object, return_value_used TSRMLS_CC);
}
/* }}} */

#define ZEND_VM_NEXT_OPCODE() \
	CHECK_SYMBOL_TABLES() \
	EX(opline)++; \
	ZEND_VM_CONTINUE()

#define ZEND_VM_SET_OPCODE(new_op) \
	CHECK_SYMBOL_TABLES() \
	EX(opline) = new_op

#define ZEND_VM_JMP(new_op) \
	CHECK_SYMBOL_TABLES() \
	if (EXPECTED(!EG(exception))) { \
		EX(opline) = new_op; \
	} \
	ZEND_VM_CONTINUE()

#define ZEND_VM_INC_OPCODE() \
	EX(opline)++

#define ZEND_VM_EXIT_FROM_EXECUTE_LOOP() \
	free_alloca(EX(CVs), EX(use_heap)); \
	EG(in_execution) = EX(original_in_execution); \
	EG(current_execute_data) = EX(prev_execute_data); \
	EG(opline_ptr) = NULL;

#define ZEND_VM_RETURN_FROM_EXECUTE_LOOP() \
	ZEND_VM_EXIT_FROM_EXECUTE_LOOP(); \
	ZEND_VM_RETURN()

#include "zend_vm_execute.h"

ZEND_API int zend_set_user_opcode_handler(zend_uchar opcode, opcode_handler_t handler) /* {{{ */
{
	if (opcode != ZEND_USER_OPCODE) {
		zend_user_opcodes[opcode] = ZEND_USER_OPCODE;
		zend_user_opcode_handlers[opcode] = handler;
		return SUCCESS;
	}
	return FAILURE;
}
/* }}} */

ZEND_API opcode_handler_t zend_get_user_opcode_handler(zend_uchar opcode) /* {{{ */
{
	return zend_user_opcode_handlers[opcode];
}
/* }}} */

ZEND_API zval *zend_get_zval_ptr(znode *node, temp_variable *Ts, zend_free_op *should_free, int type TSRMLS_DC) { /* {{{ */
	return get_zval_ptr(node, Ts, should_free, type);
}
/* }}} */

ZEND_API zval **zend_get_zval_ptr_ptr(znode *node, temp_variable *Ts, zend_free_op *should_free, int type TSRMLS_DC) { /* {{{ */
	return get_zval_ptr_ptr(node, Ts, should_free, type);
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
