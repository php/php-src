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
#include "zend_closures.h"
#include "zend_generators.h"
#include "zend_vm.h"
#include "zend_dtrace.h"

/* Virtual current working directory support */
#include "tsrm_virtual_cwd.h"

#define _CONST_CODE  0
#define _TMP_CODE    1
#define _VAR_CODE    2
#define _UNUSED_CODE 3
#define _CV_CODE     4

typedef int (*incdec_t)(zval *);

#define get_zval_ptr(op_type, node, ex, should_free, type) _get_zval_ptr(op_type, node, ex, should_free, type TSRMLS_CC)
#define get_zval_ptr_ptr(op_type, node, ex, should_free, type) _get_zval_ptr_ptr(op_type, node, ex, should_free, type TSRMLS_CC)
#define get_obj_zval_ptr(op_type, node, ex, should_free, type) _get_obj_zval_ptr(op_type, node, ex, should_free, type TSRMLS_CC)
#define get_obj_zval_ptr_ptr(op_type, node, ex, should_free, type) _get_obj_zval_ptr_ptr(op_type, node, ex, should_free, type TSRMLS_CC)

/* Prototypes */
static void zend_extension_statement_handler(const zend_extension *extension, zend_op_array *op_array TSRMLS_DC);
static void zend_extension_fcall_begin_handler(const zend_extension *extension, zend_op_array *op_array TSRMLS_DC);
static void zend_extension_fcall_end_handler(const zend_extension *extension, zend_op_array *op_array TSRMLS_DC);

#define RETURN_VALUE_USED(opline) (!((opline)->result_type & EXT_TYPE_UNUSED))

#define EX_T(offset) (*EX_TMP_VAR(execute_data, offset))
#define EX_CV(var)   (*EX_CV_NUM(execute_data, var))

#define TEMP_VAR_STACK_LIMIT 2000

static zend_always_inline void zend_pzval_unlock_func(zval *z, zend_free_op *should_free, int unref TSRMLS_DC)
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

static zend_always_inline void zend_pzval_unlock_free_func(zval *z TSRMLS_DC)
{
	if (!Z_DELREF_P(z)) {
		ZEND_ASSERT(z != &EG(uninitialized_zval));
		GC_REMOVE_ZVAL_FROM_BUFFER(z);
		zval_dtor(z);
		efree(z);
	}
}

#undef zval_ptr_dtor
#define zval_ptr_dtor(pzv) i_zval_ptr_dtor(*(pzv)  ZEND_FILE_LINE_CC)

#define PZVAL_UNLOCK(z, f) zend_pzval_unlock_func(z, f, 1 TSRMLS_CC)
#define PZVAL_UNLOCK_EX(z, f, u) zend_pzval_unlock_func(z, f, u TSRMLS_CC)
#define PZVAL_UNLOCK_FREE(z) zend_pzval_unlock_free_func(z TSRMLS_CC)
#define PZVAL_LOCK(z) Z_ADDREF_P((z))
#define SELECTIVE_PZVAL_LOCK(pzv, opline)	if (RETURN_VALUE_USED(opline)) { PZVAL_LOCK(pzv); }

#define EXTRACT_ZVAL_PTR(t) do {						\
		temp_variable *__t = (t);					\
		if (__t->var.ptr_ptr) {						\
			__t->var.ptr = *__t->var.ptr_ptr;		\
			__t->var.ptr_ptr = &__t->var.ptr;		\
			if (!PZVAL_IS_REF(__t->var.ptr) && 		\
			    Z_REFCOUNT_P(__t->var.ptr) > 2) {	\
				SEPARATE_ZVAL(__t->var.ptr_ptr);	\
			}										\
		}											\
	} while (0)

#define AI_SET_PTR(t, val) do {				\
		temp_variable *__t = (t);			\
		__t->var.ptr = (val);				\
		__t->var.ptr_ptr = &__t->var.ptr;	\
	} while (0)

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

#define MAKE_REAL_ZVAL_PTR(val) \
	do { \
		zval *_tmp; \
		ALLOC_ZVAL(_tmp); \
		INIT_PZVAL_COPY(_tmp, (val)); \
		(val) = _tmp; \
	} while (0)

/* End of zend_execute_locks.h */

#define CV_DEF_OF(i) (EG(active_op_array)->vars[i])

#define CTOR_CALL_BIT    0x1
#define CTOR_USED_BIT    0x2

#define IS_CTOR_CALL(ce) (((zend_uintptr_t)(ce)) & CTOR_CALL_BIT)
#define IS_CTOR_USED(ce) (((zend_uintptr_t)(ce)) & CTOR_USED_BIT)

#define ENCODE_CTOR(ce, used) \
	((zend_class_entry*)(((zend_uintptr_t)(ce)) | CTOR_CALL_BIT | ((used) ? CTOR_USED_BIT : 0)))
#define DECODE_CTOR(ce) \
	((zend_class_entry*)(((zend_uintptr_t)(ce)) & ~(CTOR_CALL_BIT|CTOR_USED_BIT)))

#undef EX
#define EX(element) execute_data->element

ZEND_API zval** zend_get_compiled_variable_value(const zend_execute_data *execute_data, zend_uint var)
{
	return EX_CV(var);
}

static zend_always_inline zval *_get_zval_ptr_tmp(zend_uint var, const zend_execute_data *execute_data, zend_free_op *should_free TSRMLS_DC)
{
	return should_free->var = &EX_T(var).tmp_var;
}

static zend_always_inline zval *_get_zval_ptr_var(zend_uint var, const zend_execute_data *execute_data, zend_free_op *should_free TSRMLS_DC)
{
	zval *ptr = EX_T(var).var.ptr;

	PZVAL_UNLOCK(ptr, should_free);
	return ptr;
}

static zend_never_inline zval **_get_zval_cv_lookup(zval ***ptr, zend_uint var, int type TSRMLS_DC)
{
	zend_compiled_variable *cv = &CV_DEF_OF(var);

	if (!EG(active_symbol_table) ||
	    zend_hash_quick_find(EG(active_symbol_table), cv->name, cv->name_len+1, cv->hash_value, (void **)ptr)==FAILURE) {
		switch (type) {
			case BP_VAR_R:
			case BP_VAR_UNSET:
				zend_error(E_NOTICE, "Undefined variable: %s", cv->name);
				/* break missing intentionally */
			case BP_VAR_IS:
				return &EG(uninitialized_zval_ptr);
				break;
			case BP_VAR_RW:
				zend_error(E_NOTICE, "Undefined variable: %s", cv->name);
				/* break missing intentionally */
			case BP_VAR_W:
				Z_ADDREF(EG(uninitialized_zval));
				if (!EG(active_symbol_table)) {
					*ptr = (zval**)EX_CV_NUM(EG(current_execute_data), EG(active_op_array)->last_var + var);
					**ptr = &EG(uninitialized_zval);
				} else {
					zend_hash_quick_update(EG(active_symbol_table), cv->name, cv->name_len+1, cv->hash_value, &EG(uninitialized_zval_ptr), sizeof(zval *), (void **)ptr);
				}
				break;
		}
	}
	return *ptr;
}

static zend_never_inline zval **_get_zval_cv_lookup_BP_VAR_R(zval ***ptr, zend_uint var TSRMLS_DC)
{
	zend_compiled_variable *cv = &CV_DEF_OF(var);

	if (!EG(active_symbol_table) ||
	    zend_hash_quick_find(EG(active_symbol_table), cv->name, cv->name_len+1, cv->hash_value, (void **)ptr)==FAILURE) {
		zend_error(E_NOTICE, "Undefined variable: %s", cv->name);
		return &EG(uninitialized_zval_ptr);
	}
	return *ptr;
}

static zend_never_inline zval **_get_zval_cv_lookup_BP_VAR_UNSET(zval ***ptr, zend_uint var TSRMLS_DC)
{
	zend_compiled_variable *cv = &CV_DEF_OF(var);

	if (!EG(active_symbol_table) ||
	    zend_hash_quick_find(EG(active_symbol_table), cv->name, cv->name_len+1, cv->hash_value, (void **)ptr)==FAILURE) {
		zend_error(E_NOTICE, "Undefined variable: %s", cv->name);
		return &EG(uninitialized_zval_ptr);
	}
	return *ptr;
}

static zend_never_inline zval **_get_zval_cv_lookup_BP_VAR_IS(zval ***ptr, zend_uint var TSRMLS_DC)
{
	zend_compiled_variable *cv = &CV_DEF_OF(var);

	if (!EG(active_symbol_table) ||
	    zend_hash_quick_find(EG(active_symbol_table), cv->name, cv->name_len+1, cv->hash_value, (void **)ptr)==FAILURE) {
		return &EG(uninitialized_zval_ptr);
	}
	return *ptr;
}

static zend_never_inline zval **_get_zval_cv_lookup_BP_VAR_RW(zval ***ptr, zend_uint var TSRMLS_DC)
{
	zend_compiled_variable *cv = &CV_DEF_OF(var);

	if (!EG(active_symbol_table)) {
		Z_ADDREF(EG(uninitialized_zval));
		*ptr = (zval**)EX_CV_NUM(EG(current_execute_data), EG(active_op_array)->last_var + var);
		**ptr = &EG(uninitialized_zval);
		zend_error(E_NOTICE, "Undefined variable: %s", cv->name);
	} else if (zend_hash_quick_find(EG(active_symbol_table), cv->name, cv->name_len+1, cv->hash_value, (void **)ptr)==FAILURE) {
		Z_ADDREF(EG(uninitialized_zval));
		zend_hash_quick_update(EG(active_symbol_table), cv->name, cv->name_len+1, cv->hash_value, &EG(uninitialized_zval_ptr), sizeof(zval *), (void **)ptr);
		zend_error(E_NOTICE, "Undefined variable: %s", cv->name);
	}
	return *ptr;
}

static zend_never_inline zval **_get_zval_cv_lookup_BP_VAR_W(zval ***ptr, zend_uint var TSRMLS_DC)
{
	zend_compiled_variable *cv = &CV_DEF_OF(var);

	if (!EG(active_symbol_table)) {
		Z_ADDREF(EG(uninitialized_zval));
		*ptr = (zval**)EX_CV_NUM(EG(current_execute_data), EG(active_op_array)->last_var + var);
		**ptr = &EG(uninitialized_zval);
	} else if (zend_hash_quick_find(EG(active_symbol_table), cv->name, cv->name_len+1, cv->hash_value, (void **)ptr)==FAILURE) {
		Z_ADDREF(EG(uninitialized_zval));
		zend_hash_quick_update(EG(active_symbol_table), cv->name, cv->name_len+1, cv->hash_value, &EG(uninitialized_zval_ptr), sizeof(zval *), (void **)ptr);
	}
	return *ptr;
}

static zend_always_inline zval *_get_zval_ptr_cv(zend_uint var, int type TSRMLS_DC)
{
	zval ***ptr = EX_CV_NUM(EG(current_execute_data), var);

	if (UNEXPECTED(*ptr == NULL)) {
		return *_get_zval_cv_lookup(ptr, var, type TSRMLS_CC);
	}
	return **ptr;
}

static zend_always_inline zval *_get_zval_ptr_cv_BP_VAR_R(const zend_execute_data *execute_data, zend_uint var TSRMLS_DC)
{
	zval ***ptr = EX_CV_NUM(execute_data, var);

	if (UNEXPECTED(*ptr == NULL)) {
		return *_get_zval_cv_lookup_BP_VAR_R(ptr, var TSRMLS_CC);
	}
	return **ptr;
}

static zend_always_inline zval *_get_zval_ptr_cv_BP_VAR_UNSET(const zend_execute_data *execute_data, zend_uint var TSRMLS_DC)
{
	zval ***ptr = EX_CV_NUM(execute_data, var);

	if (UNEXPECTED(*ptr == NULL)) {
		return *_get_zval_cv_lookup_BP_VAR_UNSET(ptr, var TSRMLS_CC);
	}
	return **ptr;
}

static zend_always_inline zval *_get_zval_ptr_cv_BP_VAR_IS(const zend_execute_data *execute_data, zend_uint var TSRMLS_DC)
{
	zval ***ptr = EX_CV_NUM(execute_data, var);

	if (UNEXPECTED(*ptr == NULL)) {
		return *_get_zval_cv_lookup_BP_VAR_IS(ptr, var TSRMLS_CC);
	}
	return **ptr;
}

static zend_always_inline zval *_get_zval_ptr_cv_BP_VAR_RW(const zend_execute_data *execute_data, zend_uint var TSRMLS_DC)
{
	zval ***ptr = EX_CV_NUM(execute_data, var);

	if (UNEXPECTED(*ptr == NULL)) {
		return *_get_zval_cv_lookup_BP_VAR_RW(ptr, var TSRMLS_CC);
	}
	return **ptr;
}

static zend_always_inline zval *_get_zval_ptr_cv_BP_VAR_W(const zend_execute_data *execute_data, zend_uint var TSRMLS_DC)
{
	zval ***ptr = EX_CV_NUM(execute_data, var);

	if (UNEXPECTED(*ptr == NULL)) {
		return *_get_zval_cv_lookup_BP_VAR_W(ptr, var TSRMLS_CC);
	}
	return **ptr;
}

static inline zval *_get_zval_ptr(int op_type, const znode_op *node, const zend_execute_data *execute_data, zend_free_op *should_free, int type TSRMLS_DC)
{
/*	should_free->is_var = 0; */
	switch (op_type) {
		case IS_CONST:
			should_free->var = 0;
			return node->zv;
			break;
		case IS_TMP_VAR:
			should_free->var = TMP_FREE(&EX_T(node->var).tmp_var);
			return &EX_T(node->var).tmp_var;
			break;
		case IS_VAR:
			return _get_zval_ptr_var(node->var, execute_data, should_free TSRMLS_CC);
			break;
		case IS_UNUSED:
			should_free->var = 0;
			return NULL;
			break;
		case IS_CV:
			should_free->var = 0;
			return _get_zval_ptr_cv(node->var, type TSRMLS_CC);
			break;
		EMPTY_SWITCH_DEFAULT_CASE()
	}
	return NULL;
}

static zend_always_inline zval **_get_zval_ptr_ptr_var(zend_uint var, const zend_execute_data *execute_data, zend_free_op *should_free TSRMLS_DC)
{
	zval** ptr_ptr = EX_T(var).var.ptr_ptr;

	if (EXPECTED(ptr_ptr != NULL)) {
		PZVAL_UNLOCK(*ptr_ptr, should_free);
	} else {
		/* string offset */
		PZVAL_UNLOCK(EX_T(var).str_offset.str, should_free);
	}
	return ptr_ptr;
}

static zend_always_inline zval **_get_zval_ptr_ptr_cv(zend_uint var, int type TSRMLS_DC)
{
	zval ***ptr = EX_CV_NUM(EG(current_execute_data), var);

	if (UNEXPECTED(*ptr == NULL)) {
		return _get_zval_cv_lookup(ptr, var, type TSRMLS_CC);
	}
	return *ptr;
}

static zend_always_inline zval **_get_zval_ptr_ptr_cv_BP_VAR_R(const zend_execute_data *execute_data, zend_uint var TSRMLS_DC)
{
	zval ***ptr = EX_CV_NUM(execute_data, var);

	if (UNEXPECTED(*ptr == NULL)) {
		return _get_zval_cv_lookup_BP_VAR_R(ptr, var TSRMLS_CC);
	}
	return *ptr;
}

static zend_always_inline zval **_get_zval_ptr_ptr_cv_BP_VAR_UNSET(const zend_execute_data *execute_data, zend_uint var TSRMLS_DC)
{
	zval ***ptr = EX_CV_NUM(execute_data, var);

	if (UNEXPECTED(*ptr == NULL)) {
		return _get_zval_cv_lookup_BP_VAR_UNSET(ptr, var TSRMLS_CC);
	}
	return *ptr;
}

static zend_always_inline zval **_get_zval_ptr_ptr_cv_BP_VAR_IS(const zend_execute_data *execute_data, zend_uint var TSRMLS_DC)
{
	zval ***ptr = EX_CV_NUM(execute_data, var);

	if (UNEXPECTED(*ptr == NULL)) {
		return _get_zval_cv_lookup_BP_VAR_IS(ptr, var TSRMLS_CC);
	}
	return *ptr;
}

static zend_always_inline zval **_get_zval_ptr_ptr_cv_BP_VAR_RW(const zend_execute_data *execute_data, zend_uint var TSRMLS_DC)
{
	zval ***ptr = EX_CV_NUM(execute_data, var);

	if (UNEXPECTED(*ptr == NULL)) {
		return _get_zval_cv_lookup_BP_VAR_RW(ptr, var TSRMLS_CC);
	}
	return *ptr;
}

static zend_always_inline zval **_get_zval_ptr_ptr_cv_BP_VAR_W(const zend_execute_data *execute_data, zend_uint var TSRMLS_DC)
{
	zval ***ptr = EX_CV_NUM(execute_data, var);

	if (UNEXPECTED(*ptr == NULL)) {
		return _get_zval_cv_lookup_BP_VAR_W(ptr, var TSRMLS_CC);
	}
	return *ptr;
}

static inline zval **_get_zval_ptr_ptr(int op_type, const znode_op *node, const zend_execute_data *execute_data, zend_free_op *should_free, int type TSRMLS_DC)
{
	if (op_type == IS_CV) {
		should_free->var = 0;
		return _get_zval_ptr_ptr_cv(node->var, type TSRMLS_CC);
	} else if (op_type == IS_VAR) {
		return _get_zval_ptr_ptr_var(node->var, execute_data, should_free TSRMLS_CC);
	} else {
		should_free->var = 0;
		return NULL;
	}
}

static zend_always_inline zval *_get_obj_zval_ptr_unused(TSRMLS_D)
{
	if (EXPECTED(EG(This) != NULL)) {
		return EG(This);
	} else {
		zend_error_noreturn(E_ERROR, "Using $this when not in object context");
		return NULL;
	}
}

static inline zval **_get_obj_zval_ptr_ptr(int op_type, const znode_op *op, const zend_execute_data *execute_data, zend_free_op *should_free, int type TSRMLS_DC)
{
	if (op_type == IS_UNUSED) {
		if (EXPECTED(EG(This) != NULL)) {
			/* this should actually never be modified, _ptr_ptr is modified only when
			   the object is empty */
			should_free->var = 0;
			return &EG(This);
		} else {
			zend_error_noreturn(E_ERROR, "Using $this when not in object context");
		}
	}
	return get_zval_ptr_ptr(op_type, op, execute_data, should_free, type);
}

static zend_always_inline zval **_get_obj_zval_ptr_ptr_unused(TSRMLS_D)
{
	if (EXPECTED(EG(This) != NULL)) {
		return &EG(This);
	} else {
		zend_error_noreturn(E_ERROR, "Using $this when not in object context");
		return NULL;
	}
}

static inline zval *_get_obj_zval_ptr(int op_type, znode_op *op, const zend_execute_data *execute_data, zend_free_op *should_free, int type TSRMLS_DC)
{
	if (op_type == IS_UNUSED) {
		if (EXPECTED(EG(This) != NULL)) {
			should_free->var = 0;
			return EG(This);
		} else {
			zend_error_noreturn(E_ERROR, "Using $this when not in object context");
		}
	}
	return get_zval_ptr(op_type, op, execute_data, should_free, type);
}

static void zend_assign_to_variable_reference(zval **variable_ptr_ptr, zval **value_ptr_ptr TSRMLS_DC)
{
	zval *variable_ptr = *variable_ptr_ptr;
	zval *value_ptr = *value_ptr_ptr;

	if (variable_ptr == &EG(error_zval) || value_ptr == &EG(error_zval)) {
		variable_ptr_ptr = &EG(uninitialized_zval_ptr);
	} else if (variable_ptr != value_ptr) {
		if (!PZVAL_IS_REF(value_ptr)) {
			/* break it away */
			Z_DELREF_P(value_ptr);
			if (Z_REFCOUNT_P(value_ptr)>0) {
				ALLOC_ZVAL(*value_ptr_ptr);
				ZVAL_COPY_VALUE(*value_ptr_ptr, value_ptr);
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
		} else if (variable_ptr==&EG(uninitialized_zval)
			|| Z_REFCOUNT_P(variable_ptr)>2) {
			/* we need to separate */
			Z_SET_REFCOUNT_P(variable_ptr, Z_REFCOUNT_P(variable_ptr) - 2);
			ALLOC_ZVAL(*variable_ptr_ptr);
			ZVAL_COPY_VALUE(*variable_ptr_ptr, variable_ptr);
			zval_copy_ctor(*variable_ptr_ptr);
			*value_ptr_ptr = *variable_ptr_ptr;
			Z_SET_REFCOUNT_PP(variable_ptr_ptr, 2);
		}
		Z_SET_ISREF_PP(variable_ptr_ptr);
	}
}

/* this should modify object only if it's empty */
static inline void make_real_object(zval **object_ptr TSRMLS_DC)
{
	if (Z_TYPE_PP(object_ptr) == IS_NULL
		|| (Z_TYPE_PP(object_ptr) == IS_BOOL && Z_LVAL_PP(object_ptr) == 0)
		|| (Z_TYPE_PP(object_ptr) == IS_STRING && Z_STRLEN_PP(object_ptr) == 0)
	) {
		SEPARATE_ZVAL_IF_NOT_REF(object_ptr);
		zval_dtor(*object_ptr);
		object_init(*object_ptr);
		zend_error(E_WARNING, "Creating default object from empty value");
	}
}

ZEND_API char * zend_verify_arg_class_kind(const zend_arg_info *cur_arg_info, ulong fetch_type, const char **class_name, zend_class_entry **pce TSRMLS_DC)
{
	*pce = zend_fetch_class(cur_arg_info->class_name, cur_arg_info->class_name_len, (fetch_type | ZEND_FETCH_CLASS_AUTO | ZEND_FETCH_CLASS_NO_AUTOLOAD) TSRMLS_CC);

	*class_name = (*pce) ? (*pce)->name: cur_arg_info->class_name;
	if (*pce && (*pce)->ce_flags & ZEND_ACC_INTERFACE) {
		return "implement interface ";
	} else {
		return "be an instance of ";
	}
}

ZEND_API int zend_verify_arg_error(int error_type, const zend_function *zf, zend_uint arg_num, const char *need_msg, const char *need_kind, const char *given_msg, const char *given_kind TSRMLS_DC)
{
	zend_execute_data *ptr = EG(current_execute_data)->prev_execute_data;
	const char *fname = zf->common.function_name;
	char *fsep;
	const char *fclass;

	if (zf->common.scope) {
		fsep =  "::";
		fclass = zf->common.scope->name;
	} else {
		fsep =  "";
		fclass = "";
	}

	if (ptr && ptr->op_array) {
		zend_error(error_type, "Argument %d passed to %s%s%s() must %s%s, %s%s given, called in %s on line %d and defined", arg_num, fclass, fsep, fname, need_msg, need_kind, given_msg, given_kind, ptr->op_array->filename, ptr->opline->lineno);
	} else {
		zend_error(error_type, "Argument %d passed to %s%s%s() must %s%s, %s%s given", arg_num, fclass, fsep, fname, need_msg, need_kind, given_msg, given_kind);
	}
	return 0;
}

static inline int zend_verify_arg_type(zend_function *zf, zend_uint arg_num, zval *arg, ulong fetch_type TSRMLS_DC)
{
	zend_arg_info *cur_arg_info;
	char *need_msg;
	zend_class_entry *ce;

	if (!zf->common.arg_info
		|| arg_num>zf->common.num_args) {
		return 1;
	}

	cur_arg_info = &zf->common.arg_info[arg_num-1];

	if (cur_arg_info->class_name) {
		const char *class_name;

		if (!arg) {
			need_msg = zend_verify_arg_class_kind(cur_arg_info, fetch_type, &class_name, &ce TSRMLS_CC);
			return zend_verify_arg_error(E_RECOVERABLE_ERROR, zf, arg_num, need_msg, class_name, "none", "" TSRMLS_CC);
		}
		if (Z_TYPE_P(arg) == IS_OBJECT) {
			need_msg = zend_verify_arg_class_kind(cur_arg_info, fetch_type, &class_name, &ce TSRMLS_CC);
			if (!ce || !instanceof_function(Z_OBJCE_P(arg), ce TSRMLS_CC)) {
				return zend_verify_arg_error(E_RECOVERABLE_ERROR, zf, arg_num, need_msg, class_name, "instance of ", Z_OBJCE_P(arg)->name TSRMLS_CC);
			}
		} else if (Z_TYPE_P(arg) != IS_NULL || !cur_arg_info->allow_null) {
			need_msg = zend_verify_arg_class_kind(cur_arg_info, fetch_type, &class_name, &ce TSRMLS_CC);
			return zend_verify_arg_error(E_RECOVERABLE_ERROR, zf, arg_num, need_msg, class_name, zend_zval_type_name(arg), "" TSRMLS_CC);
		}
	} else if (cur_arg_info->type_hint) {
		switch(cur_arg_info->type_hint) {
			case IS_ARRAY:
				if (!arg) {
					return zend_verify_arg_error(E_RECOVERABLE_ERROR, zf, arg_num, "be of the type array", "", "none", "" TSRMLS_CC);
				}

				if (Z_TYPE_P(arg) != IS_ARRAY && (Z_TYPE_P(arg) != IS_NULL || !cur_arg_info->allow_null)) {
					return zend_verify_arg_error(E_RECOVERABLE_ERROR, zf, arg_num, "be of the type array", "", zend_zval_type_name(arg), "" TSRMLS_CC);
				}
				break;

			case IS_CALLABLE:
				if (!arg) {
					return zend_verify_arg_error(E_RECOVERABLE_ERROR, zf, arg_num, "be callable", "", "none", "" TSRMLS_CC);
				}
				if (!zend_is_callable(arg, IS_CALLABLE_CHECK_SILENT, NULL TSRMLS_CC) && (Z_TYPE_P(arg) != IS_NULL || !cur_arg_info->allow_null)) {
					return zend_verify_arg_error(E_RECOVERABLE_ERROR, zf, arg_num, "be callable", "", zend_zval_type_name(arg), "" TSRMLS_CC);
				}
				break;

			default:
				zend_error(E_ERROR, "Unknown typehint");
		}
	}
	return 1;
}

static inline void zend_assign_to_object(zval **retval, zval **object_ptr, zval *property_name, int value_type, znode_op *value_op, const zend_execute_data *execute_data, int opcode, const zend_literal *key TSRMLS_DC)
{
	zval *object = *object_ptr;
	zend_free_op free_value;
 	zval *value = get_zval_ptr(value_type, value_op, execute_data, &free_value, BP_VAR_R);

	if (Z_TYPE_P(object) != IS_OBJECT) {
		if (object == &EG(error_zval)) {
 			if (retval) {
				*retval = &EG(uninitialized_zval);
				PZVAL_LOCK(*retval);
			}
			FREE_OP(free_value);
			return;
		}
		if (Z_TYPE_P(object) == IS_NULL ||
		    (Z_TYPE_P(object) == IS_BOOL && Z_LVAL_P(object) == 0) ||
		    (Z_TYPE_P(object) == IS_STRING && Z_STRLEN_P(object) == 0)) {
			SEPARATE_ZVAL_IF_NOT_REF(object_ptr);
			object = *object_ptr;
			Z_ADDREF_P(object);
			zend_error(E_WARNING, "Creating default object from empty value");
			if (Z_REFCOUNT_P(object) == 1) {
				/* object was removed by error handler, nothing to assign to */
				zval_ptr_dtor(&object);
				if (retval) {
					*retval = &EG(uninitialized_zval);
					PZVAL_LOCK(*retval);
				}
				FREE_OP(free_value);
				return;
			}
			Z_DELREF_P(object);
			zval_dtor(object);
			object_init(object);
		} else {
			zend_error(E_WARNING, "Attempt to assign property of non-object");
			if (retval) {
				*retval = &EG(uninitialized_zval);
				PZVAL_LOCK(*retval);
			}
			FREE_OP(free_value);
			return;
		}
	}

	/* separate our value if necessary */
	if (value_type == IS_TMP_VAR) {
		zval *orig_value = value;

		ALLOC_ZVAL(value);
		ZVAL_COPY_VALUE(value, orig_value);
		Z_UNSET_ISREF_P(value);
		Z_SET_REFCOUNT_P(value, 0);
	} else if (value_type == IS_CONST) {
		zval *orig_value = value;

		ALLOC_ZVAL(value);
		ZVAL_COPY_VALUE(value, orig_value);
		Z_UNSET_ISREF_P(value);
		Z_SET_REFCOUNT_P(value, 0);
		zval_copy_ctor(value);
	}


	Z_ADDREF_P(value);
	if (opcode == ZEND_ASSIGN_OBJ) {
		if (!Z_OBJ_HT_P(object)->write_property) {
			zend_error(E_WARNING, "Attempt to assign property of non-object");
			if (retval) {
				*retval = &EG(uninitialized_zval);
				PZVAL_LOCK(&EG(uninitialized_zval));
			}
			if (value_type == IS_TMP_VAR) {
				FREE_ZVAL(value);
			} else if (value_type == IS_CONST) {
				zval_ptr_dtor(&value);
			}
			FREE_OP(free_value);
			return;
		}
		Z_OBJ_HT_P(object)->write_property(object, property_name, value, key TSRMLS_CC);
	} else {
		/* Note:  property_name in this case is really the array index! */
		if (!Z_OBJ_HT_P(object)->write_dimension) {
			zend_error_noreturn(E_ERROR, "Cannot use object as array");
		}
		Z_OBJ_HT_P(object)->write_dimension(object, property_name, value TSRMLS_CC);
	}

	if (retval && !EG(exception)) {
		*retval = value;
		PZVAL_LOCK(value);
	}
	zval_ptr_dtor(&value);
	FREE_OP_IF_VAR(free_value);
}

static inline int zend_assign_to_string_offset(const temp_variable *T, const zval *value, int value_type TSRMLS_DC)
{
	if (Z_TYPE_P(T->str_offset.str) == IS_STRING) {

		if (((int)T->str_offset.offset < 0)) {
			zend_error(E_WARNING, "Illegal string offset:  %d", T->str_offset.offset);
			return 0;
		}

		if (T->str_offset.offset >= Z_STRLEN_P(T->str_offset.str)) {
			if (IS_INTERNED(Z_STRVAL_P(T->str_offset.str))) {
				char *tmp = (char *) emalloc(T->str_offset.offset+1+1);

				memcpy(tmp, Z_STRVAL_P(T->str_offset.str), Z_STRLEN_P(T->str_offset.str)+1);
				Z_STRVAL_P(T->str_offset.str) = tmp;
			} else {
				Z_STRVAL_P(T->str_offset.str) = (char *) erealloc(Z_STRVAL_P(T->str_offset.str), T->str_offset.offset+1+1);
			}
			memset(Z_STRVAL_P(T->str_offset.str) + Z_STRLEN_P(T->str_offset.str),
			       ' ',
			       T->str_offset.offset - Z_STRLEN_P(T->str_offset.str));
			Z_STRVAL_P(T->str_offset.str)[T->str_offset.offset+1] = 0;
			Z_STRLEN_P(T->str_offset.str) = T->str_offset.offset+1;
		} else if (IS_INTERNED(Z_STRVAL_P(T->str_offset.str))) {
			char *tmp = (char *) emalloc(Z_STRLEN_P(T->str_offset.str) + 1);

			memcpy(tmp, Z_STRVAL_P(T->str_offset.str), Z_STRLEN_P(T->str_offset.str) + 1);
			Z_STRVAL_P(T->str_offset.str) = tmp;
		}

		if (Z_TYPE_P(value) != IS_STRING) {
			zval tmp;

			ZVAL_COPY_VALUE(&tmp, value);
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
	}
	return 1;
}


static inline zval* zend_assign_tmp_to_variable(zval **variable_ptr_ptr, zval *value TSRMLS_DC)
{
	zval *variable_ptr = *variable_ptr_ptr;
	zval garbage;

	if (Z_TYPE_P(variable_ptr) == IS_OBJECT &&
	    UNEXPECTED(Z_OBJ_HANDLER_P(variable_ptr, set) != NULL)) {
		Z_OBJ_HANDLER_P(variable_ptr, set)(variable_ptr_ptr, value TSRMLS_CC);
		return variable_ptr;
	}

 	if (UNEXPECTED(Z_REFCOUNT_P(variable_ptr) > 1) &&
 	    EXPECTED(!PZVAL_IS_REF(variable_ptr))) {
 	    /* we need to split */
		Z_DELREF_P(variable_ptr);
		GC_ZVAL_CHECK_POSSIBLE_ROOT(variable_ptr);
		ALLOC_ZVAL(variable_ptr);
		INIT_PZVAL_COPY(variable_ptr, value);
		*variable_ptr_ptr = variable_ptr;
		return variable_ptr;
	} else {
		if (EXPECTED(Z_TYPE_P(variable_ptr) <= IS_BOOL)) {
			/* nothing to destroy */
			ZVAL_COPY_VALUE(variable_ptr, value);
		} else {
			ZVAL_COPY_VALUE(&garbage, variable_ptr);
			ZVAL_COPY_VALUE(variable_ptr, value);
			_zval_dtor_func(&garbage ZEND_FILE_LINE_CC);
		}
		return variable_ptr;
	}
}

static inline zval* zend_assign_const_to_variable(zval **variable_ptr_ptr, zval *value TSRMLS_DC)
{
	zval *variable_ptr = *variable_ptr_ptr;
	zval garbage;

	if (Z_TYPE_P(variable_ptr) == IS_OBJECT &&
	    UNEXPECTED(Z_OBJ_HANDLER_P(variable_ptr, set) != NULL)) {
		Z_OBJ_HANDLER_P(variable_ptr, set)(variable_ptr_ptr, value TSRMLS_CC);
		return variable_ptr;
	}

 	if (UNEXPECTED(Z_REFCOUNT_P(variable_ptr) > 1) &&
 	    EXPECTED(!PZVAL_IS_REF(variable_ptr))) {
		/* we need to split */
		Z_DELREF_P(variable_ptr);
		GC_ZVAL_CHECK_POSSIBLE_ROOT(variable_ptr);
		ALLOC_ZVAL(variable_ptr);
		INIT_PZVAL_COPY(variable_ptr, value);
		zval_copy_ctor(variable_ptr);
		*variable_ptr_ptr = variable_ptr;
		return variable_ptr;
 	} else {
		if (EXPECTED(Z_TYPE_P(variable_ptr) <= IS_BOOL)) {
			/* nothing to destroy */
			ZVAL_COPY_VALUE(variable_ptr, value);
			zendi_zval_copy_ctor(*variable_ptr);
		} else {
			ZVAL_COPY_VALUE(&garbage, variable_ptr);
			ZVAL_COPY_VALUE(variable_ptr, value);
			zendi_zval_copy_ctor(*variable_ptr);
			_zval_dtor_func(&garbage ZEND_FILE_LINE_CC);
		}
		return variable_ptr;
	}
}

static inline zval* zend_assign_to_variable(zval **variable_ptr_ptr, zval *value TSRMLS_DC)
{
	zval *variable_ptr = *variable_ptr_ptr;
	zval garbage;

	if (Z_TYPE_P(variable_ptr) == IS_OBJECT &&
	    UNEXPECTED(Z_OBJ_HANDLER_P(variable_ptr, set) != NULL)) {
		Z_OBJ_HANDLER_P(variable_ptr, set)(variable_ptr_ptr, value TSRMLS_CC);
		return variable_ptr;
	}

 	if (EXPECTED(!PZVAL_IS_REF(variable_ptr))) {
		if (Z_REFCOUNT_P(variable_ptr)==1) {
			if (UNEXPECTED(variable_ptr == value)) {
				return variable_ptr;
			} else if (EXPECTED(!PZVAL_IS_REF(value))) {
				Z_ADDREF_P(value);
				*variable_ptr_ptr = value;
				ZEND_ASSERT(variable_ptr != &EG(uninitialized_zval));
				GC_REMOVE_ZVAL_FROM_BUFFER(variable_ptr);
				zval_dtor(variable_ptr);
				efree(variable_ptr);
				return value;
			} else {
				goto copy_value;
			}
		} else { /* we need to split */
			Z_DELREF_P(variable_ptr);
			GC_ZVAL_CHECK_POSSIBLE_ROOT(variable_ptr);
			if (PZVAL_IS_REF(value) && Z_REFCOUNT_P(value) > 0) {
				ALLOC_ZVAL(variable_ptr);
				*variable_ptr_ptr = variable_ptr;
				INIT_PZVAL_COPY(variable_ptr, value);
				zval_copy_ctor(variable_ptr);
				return variable_ptr;
			} else {
				*variable_ptr_ptr = value;
				Z_ADDREF_P(value);
				Z_UNSET_ISREF_P(value);
				return value;
			}
		}
 	} else {
		if (EXPECTED(variable_ptr != value)) {
copy_value:
			if (EXPECTED(Z_TYPE_P(variable_ptr) <= IS_BOOL)) {
				/* nothing to destroy */
				ZVAL_COPY_VALUE(variable_ptr, value);
				zendi_zval_copy_ctor(*variable_ptr);
			} else {
				ZVAL_COPY_VALUE(&garbage, variable_ptr);
				ZVAL_COPY_VALUE(variable_ptr, value);
				zendi_zval_copy_ctor(*variable_ptr);
				_zval_dtor_func(&garbage ZEND_FILE_LINE_CC);
			}
		}
		return variable_ptr;
	}
}

/* Utility Functions for Extensions */
static void zend_extension_statement_handler(const zend_extension *extension, zend_op_array *op_array TSRMLS_DC)
{
	if (extension->statement_handler) {
		extension->statement_handler(op_array);
	}
}


static void zend_extension_fcall_begin_handler(const zend_extension *extension, zend_op_array *op_array TSRMLS_DC)
{
	if (extension->fcall_begin_handler) {
		extension->fcall_begin_handler(op_array);
	}
}


static void zend_extension_fcall_end_handler(const zend_extension *extension, zend_op_array *op_array TSRMLS_DC)
{
	if (extension->fcall_end_handler) {
		extension->fcall_end_handler(op_array);
	}
}


static inline HashTable *zend_get_target_symbol_table(int fetch_type TSRMLS_DC)
{
	switch (fetch_type) {
		case ZEND_FETCH_LOCAL:
			if (!EG(active_symbol_table)) {
				zend_rebuild_symbol_table(TSRMLS_C);
			}
			return EG(active_symbol_table);
			break;
		case ZEND_FETCH_GLOBAL:
		case ZEND_FETCH_GLOBAL_LOCK:
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

static inline zval **zend_fetch_dimension_address_inner(HashTable *ht, const zval *dim, int dim_type, int type TSRMLS_DC)
{
	zval **retval;
	char *offset_key;
	int offset_key_length;
	ulong hval;

	switch (dim->type) {
		case IS_NULL:
			offset_key = "";
			offset_key_length = 0;
			hval = zend_inline_hash_func("", 1);
			goto fetch_string_dim;

		case IS_STRING:

			offset_key = dim->value.str.val;
			offset_key_length = dim->value.str.len;

			if (dim_type == IS_CONST) {
				hval = Z_HASH_P(dim);
			} else {
				ZEND_HANDLE_NUMERIC_EX(offset_key, offset_key_length+1, hval, goto num_index);
				if (IS_INTERNED(offset_key)) {
					hval = INTERNED_HASH(offset_key);
				} else {
					hval = zend_hash_func(offset_key, offset_key_length+1);
				}
			}
fetch_string_dim:
			if (zend_hash_quick_find(ht, offset_key, offset_key_length+1, hval, (void **) &retval) == FAILURE) {
				switch (type) {
					case BP_VAR_R:
						zend_error(E_NOTICE, "Undefined index: %s", offset_key);
						/* break missing intentionally */
					case BP_VAR_UNSET:
					case BP_VAR_IS:
						retval = &EG(uninitialized_zval_ptr);
						break;
					case BP_VAR_RW:
						zend_error(E_NOTICE,"Undefined index: %s", offset_key);
						/* break missing intentionally */
					case BP_VAR_W: {
							zval *new_zval = &EG(uninitialized_zval);

							Z_ADDREF_P(new_zval);
							zend_hash_quick_update(ht, offset_key, offset_key_length+1, hval, &new_zval, sizeof(zval *), (void **) &retval);
						}
						break;
				}
			}
			break;
		case IS_DOUBLE:
			hval = zend_dval_to_lval(Z_DVAL_P(dim));
			goto num_index;
		case IS_RESOURCE:
			zend_error(E_STRICT, "Resource ID#%ld used as offset, casting to integer (%ld)", Z_LVAL_P(dim), Z_LVAL_P(dim));
			/* Fall Through */
		case IS_BOOL:
		case IS_LONG:
			hval = Z_LVAL_P(dim);
num_index:
			if (zend_hash_index_find(ht, hval, (void **) &retval) == FAILURE) {
				switch (type) {
					case BP_VAR_R:
						zend_error(E_NOTICE,"Undefined offset: %ld", hval);
						/* break missing intentionally */
					case BP_VAR_UNSET:
					case BP_VAR_IS:
						retval = &EG(uninitialized_zval_ptr);
						break;
					case BP_VAR_RW:
						zend_error(E_NOTICE,"Undefined offset: %ld", hval);
						/* break missing intentionally */
					case BP_VAR_W: {
						zval *new_zval = &EG(uninitialized_zval);

						Z_ADDREF_P(new_zval);
						zend_hash_index_update(ht, hval, &new_zval, sizeof(zval *), (void **) &retval);
					}
					break;
				}
			}
			break;

		default:
			zend_error(E_WARNING, "Illegal offset type");
			return (type == BP_VAR_W || type == BP_VAR_RW) ?
				&EG(error_zval_ptr) : &EG(uninitialized_zval_ptr);
	}
	return retval;
}

static void zend_fetch_dimension_address(temp_variable *result, zval **container_ptr, zval *dim, int dim_type, int type TSRMLS_DC)
{
	zval *container = *container_ptr;
	zval **retval;

	switch (Z_TYPE_P(container)) {

		case IS_ARRAY:
			if (type != BP_VAR_UNSET && Z_REFCOUNT_P(container)>1 && !PZVAL_IS_REF(container)) {
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
				retval = zend_fetch_dimension_address_inner(Z_ARRVAL_P(container), dim, dim_type, type TSRMLS_CC);
			}
			result->var.ptr_ptr = retval;
			PZVAL_LOCK(*retval);
			return;
			break;

		case IS_NULL:
			if (container == &EG(error_zval)) {
				result->var.ptr_ptr = &EG(error_zval_ptr);
				PZVAL_LOCK(EG(error_zval_ptr));
			} else if (type != BP_VAR_UNSET) {
convert_to_array:
				if (!PZVAL_IS_REF(container)) {
					SEPARATE_ZVAL(container_ptr);
					container = *container_ptr;
				}
				zval_dtor(container);
				array_init(container);
				goto fetch_from_array;
			} else {
				/* for read-mode only */
				result->var.ptr_ptr = &EG(uninitialized_zval_ptr);
				PZVAL_LOCK(EG(uninitialized_zval_ptr));
			}
			return;
			break;

		case IS_STRING: {
				zval tmp;

				if (type != BP_VAR_UNSET && Z_STRLEN_P(container)==0) {
					goto convert_to_array;
				}
				if (dim == NULL) {
					zend_error_noreturn(E_ERROR, "[] operator not supported for strings");
				}

				if (type != BP_VAR_UNSET) {
					SEPARATE_ZVAL_IF_NOT_REF(container_ptr);
				}

				if (Z_TYPE_P(dim) != IS_LONG) {

					switch(Z_TYPE_P(dim)) {
						/* case IS_LONG: */
						case IS_STRING:
							if (IS_LONG == is_numeric_string(Z_STRVAL_P(dim), Z_STRLEN_P(dim), NULL, NULL, -1)) {
								break;
							}
							if (type != BP_VAR_UNSET) {
								zend_error(E_WARNING, "Illegal string offset '%s'", dim->value.str.val);
							}

							break;
						case IS_DOUBLE:
						case IS_NULL:
						case IS_BOOL:
							zend_error(E_NOTICE, "String offset cast occurred");
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
				container = *container_ptr;
				result->str_offset.str = container;
				PZVAL_LOCK(container);
				result->str_offset.offset = Z_LVAL_P(dim);
				result->str_offset.ptr_ptr = NULL;
				return;
			}
			break;

		case IS_OBJECT:
			if (!Z_OBJ_HT_P(container)->read_dimension) {
				zend_error_noreturn(E_ERROR, "Cannot use object as array");
			} else {
				zval *overloaded_result;

				if (dim_type == IS_TMP_VAR) {
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
							ZVAL_COPY_VALUE(overloaded_result, tmp);
							zval_copy_ctor(overloaded_result);
							Z_UNSET_ISREF_P(overloaded_result);
							Z_SET_REFCOUNT_P(overloaded_result, 0);
						}
						if (Z_TYPE_P(overloaded_result) != IS_OBJECT) {
							zend_class_entry *ce = Z_OBJCE_P(container);
							zend_error(E_NOTICE, "Indirect modification of overloaded element of %s has no effect", ce->name);
						}
					}
					retval = &overloaded_result;
				} else {
					retval = &EG(error_zval_ptr);
				}
				AI_SET_PTR(result, *retval);
				PZVAL_LOCK(*retval);
				if (dim_type == IS_TMP_VAR) {
					zval_ptr_dtor(&dim);
				}
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
				AI_SET_PTR(result, &EG(uninitialized_zval));
				PZVAL_LOCK(&EG(uninitialized_zval));
			} else {
				zend_error(E_WARNING, "Cannot use a scalar value as an array");
				result->var.ptr_ptr = &EG(error_zval_ptr);
				PZVAL_LOCK(EG(error_zval_ptr));
			}
			break;
	}
}

static void zend_fetch_dimension_address_read(temp_variable *result, zval *container, zval *dim, int dim_type, int type TSRMLS_DC)
{
	zval **retval;

	switch (Z_TYPE_P(container)) {

		case IS_ARRAY:
			retval = zend_fetch_dimension_address_inner(Z_ARRVAL_P(container), dim, dim_type, type TSRMLS_CC);
			AI_SET_PTR(result, *retval);
			PZVAL_LOCK(*retval);
			return;

		case IS_NULL:
			AI_SET_PTR(result, &EG(uninitialized_zval));
			PZVAL_LOCK(&EG(uninitialized_zval));
			return;

		case IS_STRING: {
				zval tmp;
				zval *ptr;

				if (Z_TYPE_P(dim) != IS_LONG) {
					switch(Z_TYPE_P(dim)) {
						/* case IS_LONG: */
						case IS_STRING:
							if (IS_LONG == is_numeric_string(Z_STRVAL_P(dim), Z_STRLEN_P(dim), NULL, NULL, -1)) {
								break;
							}
							if (type != BP_VAR_IS) {
								zend_error(E_WARNING, "Illegal string offset '%s'", dim->value.str.val);
							}
							break;
						case IS_DOUBLE:
						case IS_NULL:
						case IS_BOOL:
							if (type != BP_VAR_IS) {
								zend_error(E_NOTICE, "String offset cast occurred");
							}
							break;
						default:
							zend_error(E_WARNING, "Illegal offset type");
							break;
					}

					ZVAL_COPY_VALUE(&tmp, dim);
					zval_copy_ctor(&tmp);
					convert_to_long(&tmp);
					dim = &tmp;
				}

				ALLOC_ZVAL(ptr);
				INIT_PZVAL(ptr);
				Z_TYPE_P(ptr) = IS_STRING;

				if (Z_LVAL_P(dim) < 0 || Z_STRLEN_P(container) <= Z_LVAL_P(dim)) {
					if (type != BP_VAR_IS) {
						zend_error(E_NOTICE, "Uninitialized string offset: %ld", Z_LVAL_P(dim));
					}
					Z_STRVAL_P(ptr) = STR_EMPTY_ALLOC();
					Z_STRLEN_P(ptr) = 0;
				} else {
					Z_STRVAL_P(ptr) = (char*)emalloc(2);
					Z_STRVAL_P(ptr)[0] = Z_STRVAL_P(container)[Z_LVAL_P(dim)];
					Z_STRVAL_P(ptr)[1] = 0;
					Z_STRLEN_P(ptr) = 1;
				}
				AI_SET_PTR(result, ptr);
				return;
			}
			break;

		case IS_OBJECT:
			if (!Z_OBJ_HT_P(container)->read_dimension) {
				zend_error_noreturn(E_ERROR, "Cannot use object as array");
			} else {
				zval *overloaded_result;

				if (dim_type == IS_TMP_VAR) {
					zval *orig = dim;
					MAKE_REAL_ZVAL_PTR(dim);
					ZVAL_NULL(orig);
				}
				overloaded_result = Z_OBJ_HT_P(container)->read_dimension(container, dim, type TSRMLS_CC);

				if (overloaded_result) {
					AI_SET_PTR(result, overloaded_result);
					PZVAL_LOCK(overloaded_result);
				} else if (result) {
					AI_SET_PTR(result, &EG(uninitialized_zval));
					PZVAL_LOCK(&EG(uninitialized_zval));
				}
				if (dim_type == IS_TMP_VAR) {
					zval_ptr_dtor(&dim);
				}
			}
			return;

		default:
			AI_SET_PTR(result, &EG(uninitialized_zval));
			PZVAL_LOCK(&EG(uninitialized_zval));
			return;
	}
}

static void zend_fetch_property_address(temp_variable *result, zval **container_ptr, zval *prop_ptr, const zend_literal *key, int type TSRMLS_DC)
{
	zval *container = *container_ptr;;

	if (Z_TYPE_P(container) != IS_OBJECT) {
		if (container == &EG(error_zval)) {
			result->var.ptr_ptr = &EG(error_zval_ptr);
			PZVAL_LOCK(EG(error_zval_ptr));
			return;
		}

		/* this should modify object only if it's empty */
		if (type != BP_VAR_UNSET &&
		    ((Z_TYPE_P(container) == IS_NULL ||
		     (Z_TYPE_P(container) == IS_BOOL && Z_LVAL_P(container)==0) ||
		     (Z_TYPE_P(container) == IS_STRING && Z_STRLEN_P(container)==0)))) {
			if (!PZVAL_IS_REF(container)) {
				SEPARATE_ZVAL(container_ptr);
				container = *container_ptr;
			}
			object_init(container);
		} else {
			zend_error(E_WARNING, "Attempt to modify property of non-object");
			result->var.ptr_ptr = &EG(error_zval_ptr);
			PZVAL_LOCK(EG(error_zval_ptr));
			return;
		}
	}

	if (Z_OBJ_HT_P(container)->get_property_ptr_ptr) {
		zval **ptr_ptr = Z_OBJ_HT_P(container)->get_property_ptr_ptr(container, prop_ptr, type, key TSRMLS_CC);
		if (NULL == ptr_ptr) {
			zval *ptr;

			if (Z_OBJ_HT_P(container)->read_property &&
				(ptr = Z_OBJ_HT_P(container)->read_property(container, prop_ptr, type, key TSRMLS_CC)) != NULL) {
				AI_SET_PTR(result, ptr);
				PZVAL_LOCK(ptr);
			} else {
				zend_error_noreturn(E_ERROR, "Cannot access undefined property for object with overloaded property access");
			}
		} else {
			result->var.ptr_ptr = ptr_ptr;
			PZVAL_LOCK(*ptr_ptr);
		}
	} else if (Z_OBJ_HT_P(container)->read_property) {
		zval *ptr = Z_OBJ_HT_P(container)->read_property(container, prop_ptr, type, key TSRMLS_CC);

		AI_SET_PTR(result, ptr);
		PZVAL_LOCK(ptr);
	} else {
		zend_error(E_WARNING, "This object doesn't support property references");
		result->var.ptr_ptr = &EG(error_zval_ptr);
		PZVAL_LOCK(EG(error_zval_ptr));
	}
}

static inline zend_brk_cont_element* zend_brk_cont(int nest_levels, int array_offset, const zend_op_array *op_array, const zend_execute_data *execute_data TSRMLS_DC)
{
	int original_nest_levels = nest_levels;
	zend_brk_cont_element *jmp_to;

	do {
		if (array_offset==-1) {
			zend_error_noreturn(E_ERROR, "Cannot break/continue %d level%s", original_nest_levels, (original_nest_levels == 1) ? "" : "s");
		}
		jmp_to = &op_array->brk_cont_array[array_offset];
		if (nest_levels>1) {
			zend_op *brk_opline = &op_array->opcodes[jmp_to->brk];

			switch (brk_opline->opcode) {
				case ZEND_SWITCH_FREE:
					if (!(brk_opline->extended_value & EXT_TYPE_FREE_ON_RETURN)) {
						zval_ptr_dtor(&EX_T(brk_opline->op1.var).var.ptr);
					}
					break;
				case ZEND_FREE:
					if (!(brk_opline->extended_value & EXT_TYPE_FREE_ON_RETURN)) {
						zendi_zval_dtor(EX_T(brk_opline->op1.var).tmp_var);
					}
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
/* See http://support.microsoft.com/kb/190351 */
#ifdef PHP_WIN32
		fflush(stderr);
#endif
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

ZEND_API void execute_internal(zend_execute_data *execute_data_ptr, zend_fcall_info *fci, int return_value_used TSRMLS_DC)
{
	if(fci != NULL) {
		((zend_internal_function *) execute_data_ptr->function_state.function)->handler(fci->param_count,
				*fci->retval_ptr_ptr, fci->retval_ptr_ptr, fci->object_ptr, 1 TSRMLS_CC);

	} else {
		zval **return_value_ptr = &EX_TMP_VAR(execute_data_ptr, execute_data_ptr->opline->result.var)->var.ptr;
		((zend_internal_function *) execute_data_ptr->function_state.function)->handler(execute_data_ptr->opline->extended_value, *return_value_ptr,
					(execute_data_ptr->function_state.function->common.fn_flags & ZEND_ACC_RETURN_REFERENCE)?return_value_ptr:NULL,
					execute_data_ptr->object, return_value_used TSRMLS_CC);
	}
}

void zend_clean_and_cache_symbol_table(HashTable *symbol_table TSRMLS_DC) /* {{{ */
{
	if (EG(symtable_cache_ptr) >= EG(symtable_cache_limit)) {
		zend_hash_destroy(symbol_table);
		FREE_HASHTABLE(symbol_table);
	} else {
		/* clean before putting into the cache, since clean
		   could call dtors, which could use cached hash */
		zend_hash_clean(symbol_table);
		*(++EG(symtable_cache_ptr)) = symbol_table;
	}
}
/* }}} */

static zend_always_inline void i_free_compiled_variables(zend_execute_data *execute_data) /* {{{ */
{
	zval ***cv = EX_CV_NUM(execute_data, 0);
	zval ***end = cv + EX(op_array)->last_var;
	while (cv != end) {
		if (*cv) {
			zval_ptr_dtor(*cv);
		}
		cv++;
 	}
}
/* }}} */

void zend_free_compiled_variables(zend_execute_data *execute_data) /* {{{ */
{
	i_free_compiled_variables(execute_data);
}
/* }}} */

/*
 * Stack Frame Layout (the whole stack frame is allocated at once)
 * ==================
 *
 *                             +========================================+
 *                             | zend_execute_data                      |<---+
 *                             |     EX(function_state).arguments       |--+ |
 *                             |  ...                                   |  | |
 *                             | ARGUMENT [1]                           |  | |
 *                             | ...                                    |  | |
 *                             | ARGUMENT [ARGS_NUMBER]                 |  | |
 *                             | ARGS_NUMBER                            |<-+ |
 *                             +========================================+    |
 *                                                                           |
 *                             +========================================+    |
 *                             | TMP_VAR[op_arrat->T-1]                 |    |
 *                             | ...                                    |    |
 *     EX_TMP_VAR_NUM(0) ----> | TMP_VAR[0]                             |    |
 *                             +----------------------------------------+    |
 * EG(current_execute_data) -> | zend_execute_data                      |    |
 *                             |     EX(prev_execute_data)              |----+
 *                             +----------------------------------------+
 *     EX_CV_NUM(0) ---------> | CV[0]                                  |--+
 *                             | ...                                    |  |
 *                             | CV[op_array->last_var-1]               |  |
 *                             +----------------------------------------+  |
 *                             | Optional slot for CV[0] zval*          |<-+
 *                             | ...                                    |
 *                             | ...for CV [op_array->last_var-1] zval* |
 *                             +----------------------------------------+
 *           EX(call_slots) -> | CALL_SLOT[0]                           |
 *                             | ...                                    |
 *                             | CALL_SLOT[op_array->nested_calls-1]    |
 *                             +----------------------------------------+
 * zend_vm_stack_frame_base -> | ARGUMENTS STACK [0]                    |
 *                             | ...                                    |
 * zend_vm_stack_top --------> | ...                                    |
 *                             | ...                                    |
 *                             | ARGUMENTS STACK [op_array->used_stack] |
 *                             +----------------------------------------+
 */

static zend_always_inline zend_execute_data *i_create_execute_data_from_op_array(zend_op_array *op_array, zend_bool nested TSRMLS_DC) /* {{{ */
{
	zend_execute_data *execute_data;

	/*
	 * When allocating the execute_data, memory for compiled variables and
	 * temporary variables is also allocated before and after the actual
	 * zend_execute_data struct. In addition we also allocate space to store
	 * information about syntactically nested called functions and actual
	 * parameters. op_array->last_var specifies the number of compiled
	 * variables and op_array->T is the number of temporary variables. If there
	 * is no symbol table, then twice as much memory is allocated for compiled
	 * variables. In that case the first half contains zval**s and the second
	 * half the actual zval*s (which would otherwise be in the symbol table).
	 */
	size_t execute_data_size = ZEND_MM_ALIGNED_SIZE(sizeof(zend_execute_data));
	size_t CVs_size = ZEND_MM_ALIGNED_SIZE(sizeof(zval **) * op_array->last_var * (EG(active_symbol_table) ? 1 : 2));
	size_t Ts_size = ZEND_MM_ALIGNED_SIZE(sizeof(temp_variable)) * op_array->T;
	size_t call_slots_size = ZEND_MM_ALIGNED_SIZE(sizeof(call_slot)) * op_array->nested_calls;
	size_t stack_size = ZEND_MM_ALIGNED_SIZE(sizeof(zval*)) * op_array->used_stack;
	size_t total_size = execute_data_size + Ts_size + CVs_size + call_slots_size + stack_size;

	/*
	 * Normally the execute_data is allocated on the VM stack (because it does
	 * not actually do any allocation and thus is faster). For generators
	 * though this behavior would be suboptimal, because the (rather large)
	 * structure would have to be copied back and forth every time execution is
	 * suspended or resumed. That's why for generators the execution context
	 * is allocated using a separate VM stack, thus allowing to save and
	 * restore it simply by replacing a pointer. The same segment also keeps
	 * a copy of previous execute_data and passed parameters.
	 */
	if (UNEXPECTED((op_array->fn_flags & ZEND_ACC_GENERATOR) != 0)) {
		/* Prepend the regular stack frame with a copy of prev_execute_data
		 * and the passed arguments
		 */
		int args_count = zend_vm_stack_get_args_count_ex(EG(current_execute_data));
		size_t args_size = ZEND_MM_ALIGNED_SIZE(sizeof(zval*)) * (args_count + 1);

		total_size += args_size + execute_data_size;

		EG(argument_stack) = zend_vm_stack_new_page((total_size + (sizeof(void*) - 1)) / sizeof(void*));
		EG(argument_stack)->prev = NULL;
		execute_data = (zend_execute_data*)((char*)ZEND_VM_STACK_ELEMETS(EG(argument_stack)) + args_size + execute_data_size + Ts_size);

		/* copy prev_execute_data */
		EX(prev_execute_data) = (zend_execute_data*)((char*)ZEND_VM_STACK_ELEMETS(EG(argument_stack)) + args_size);
		memset(EX(prev_execute_data), 0, sizeof(zend_execute_data));
		EX(prev_execute_data)->function_state.function = (zend_function*)op_array;
		EX(prev_execute_data)->function_state.arguments = (void**)((char*)ZEND_VM_STACK_ELEMETS(EG(argument_stack)) + ZEND_MM_ALIGNED_SIZE(sizeof(zval*)) * args_count);

		/* copy arguments */
		*EX(prev_execute_data)->function_state.arguments = (void*)(zend_uintptr_t)args_count;
		if (args_count > 0) {
			zval **arg_src = (zval**)zend_vm_stack_get_arg_ex(EG(current_execute_data), 1);
			zval **arg_dst = (zval**)zend_vm_stack_get_arg_ex(EX(prev_execute_data), 1);
			int i;

			for (i = 0; i < args_count; i++) {
				arg_dst[i] = arg_src[i];
				Z_ADDREF_P(arg_dst[i]);
			}
		}
	} else {
		execute_data = zend_vm_stack_alloc(total_size TSRMLS_CC);
		execute_data = (zend_execute_data*)((char*)execute_data + Ts_size);
		EX(prev_execute_data) = EG(current_execute_data);
	}

	memset(EX_CV_NUM(execute_data, 0), 0, sizeof(zval **) * op_array->last_var);

	EX(call_slots) = (call_slot*)((char *)execute_data + execute_data_size + CVs_size);


	EX(op_array) = op_array;

	EG(argument_stack)->top = zend_vm_stack_frame_base(execute_data);

	EX(object) = NULL;
	EX(current_this) = NULL;
	EX(old_error_reporting) = NULL;
	EX(symbol_table) = EG(active_symbol_table);
	EX(call) = NULL;
	EG(current_execute_data) = execute_data;
	EX(nested) = nested;

	if (!op_array->run_time_cache && op_array->last_cache_slot) {
		op_array->run_time_cache = ecalloc(op_array->last_cache_slot, sizeof(void*));
	}

	if (op_array->this_var != -1 && EG(This)) {
 		Z_ADDREF_P(EG(This)); /* For $this pointer */
		if (!EG(active_symbol_table)) {
			EX_CV(op_array->this_var) = (zval **) EX_CV_NUM(execute_data, op_array->last_var + op_array->this_var);
			*EX_CV(op_array->this_var) = EG(This);
		} else {
			if (zend_hash_add(EG(active_symbol_table), "this", sizeof("this"), &EG(This), sizeof(zval *), (void **) EX_CV_NUM(execute_data, op_array->this_var))==FAILURE) {
				Z_DELREF_P(EG(This));
			}
		}
	}

	EX(opline) = UNEXPECTED((op_array->fn_flags & ZEND_ACC_INTERACTIVE) != 0) && EG(start_op) ? EG(start_op) : op_array->opcodes;
	EG(opline_ptr) = &EX(opline);

	EX(function_state).function = (zend_function *) op_array;
	EX(function_state).arguments = NULL;

	return execute_data;
}
/* }}} */

zend_execute_data *zend_create_execute_data_from_op_array(zend_op_array *op_array, zend_bool nested TSRMLS_DC) /* {{{ */
{
	return i_create_execute_data_from_op_array(op_array, nested TSRMLS_CC);
}
/* }}} */

#define ZEND_VM_NEXT_OPCODE() \
	CHECK_SYMBOL_TABLES() \
	ZEND_VM_INC_OPCODE(); \
	ZEND_VM_CONTINUE()

#define ZEND_VM_SET_OPCODE(new_op) \
	CHECK_SYMBOL_TABLES() \
	OPLINE = new_op

#define ZEND_VM_JMP(new_op) \
	if (EXPECTED(!EG(exception))) { \
		ZEND_VM_SET_OPCODE(new_op); \
	} else { \
		LOAD_OPLINE(); \
	} \
	ZEND_VM_CONTINUE()

#define ZEND_VM_INC_OPCODE() \
	OPLINE++

#ifdef __GNUC__
# define ZEND_VM_GUARD(name) __asm__("#" #name)
#else
# define ZEND_VM_GUARD(name)
#endif

#include "zend_vm_execute.h"

ZEND_API int zend_set_user_opcode_handler(zend_uchar opcode, user_opcode_handler_t handler)
{
	if (opcode != ZEND_USER_OPCODE) {
		if (handler == NULL) {
			/* restore the original handler */
			zend_user_opcodes[opcode] = opcode;
		} else {
			zend_user_opcodes[opcode] = ZEND_USER_OPCODE;
		}
		zend_user_opcode_handlers[opcode] = handler;
		return SUCCESS;
	}
	return FAILURE;
}

ZEND_API user_opcode_handler_t zend_get_user_opcode_handler(zend_uchar opcode)
{
	return zend_user_opcode_handlers[opcode];
}

ZEND_API zval *zend_get_zval_ptr(int op_type, const znode_op *node, const zend_execute_data *execute_data, zend_free_op *should_free, int type TSRMLS_DC) {
	return get_zval_ptr(op_type, node, execute_data, should_free, type);
}

ZEND_API zval **zend_get_zval_ptr_ptr(int op_type, const znode_op *node, const zend_execute_data *execute_data, zend_free_op *should_free, int type TSRMLS_DC) {
	return get_zval_ptr_ptr(op_type, node, execute_data, should_free, type);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
