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
#include "zend_virtual_cwd.h"

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

#define TEMP_VAR_STACK_LIMIT 2000

static zend_always_inline void zend_pzval_unlock_func(zval *z, zend_free_op *should_free, int unref TSRMLS_DC)
{
	should_free->var = NULL;
	if (Z_REFCOUNTED_P(z)) {
		if (!Z_DELREF_P(z)) {
			Z_SET_REFCOUNT_P(z, 1);
//???			Z_UNSET_ISREF_P(z);
			if (Z_ISREF_P(z)) {
//???				zend_reference *ref = Z_REF_P(z);
//???				ZVAL_COPY_VALUE(z, &ref->val);
//???				efree(ref);
			}
			should_free->var = z;
/*		should_free->is_var = 1; */
		} else {
			if (unref && Z_ISREF_P(z) && Z_REFCOUNT_P(z) == 1) {
//???			Z_UNSET_ISREF_P(z);
				if (Z_ISREF_P(z)) {
//???					zend_reference *ref = Z_REF_P(z);
//???					ZVAL_COPY_VALUE(z, &ref->val);
//???					efree(ref);
				}
			}
		}
	}
}

static zend_always_inline void zend_pzval_unlock_free_func(zval *z TSRMLS_DC)
{
	if (Z_REFCOUNTED_P(z)) {
		if (!Z_DELREF_P(z)) {
			ZEND_ASSERT(z != &EG(uninitialized_zval));
			GC_REMOVE_ZVAL_FROM_BUFFER(z);
			zval_dtor(z);
			efree(z);
		}
	}
}

#undef zval_ptr_dtor
#define zval_ptr_dtor(zv) i_zval_ptr_dtor(zv ZEND_FILE_LINE_CC TSRMLS_CC)
#define zval_ptr_dtor_nogc(zv) i_zval_ptr_dtor_nogc(zv ZEND_FILE_LINE_CC TSRMLS_CC)

#define PZVAL_UNLOCK(z, f) zend_pzval_unlock_func(z, f, 1 TSRMLS_CC)
#define PZVAL_UNLOCK_EX(z, f, u) zend_pzval_unlock_func(z, f, u TSRMLS_CC)
#define PZVAL_UNLOCK_FREE(z) zend_pzval_unlock_free_func(z TSRMLS_CC)
#define PZVAL_LOCK(z) if (Z_REFCOUNTED_P(z)) Z_ADDREF_P((z))
#define SELECTIVE_PZVAL_LOCK(pzv, opline)	if (RETURN_VALUE_USED(opline)) { PZVAL_LOCK(pzv); }

#define EXTRACT_ZVAL_PTR(zv) do {						\
		zval *__zv = (zv);								\
		if (Z_TYPE_P(__zv) == IS_INDIRECT) {			\
			ZVAL_COPY_VALUE(__zv, Z_INDIRECT_P(__zv));	\
		}												\
	} while (0)

#define FREE_OP(should_free) \
	if (should_free.var) { \
		if ((zend_uintptr_t)should_free.var & 1L) { \
			zval_dtor((zval*)((zend_uintptr_t)should_free.var & ~1L)); \
		} else { \
			zval_ptr_dtor_nogc(should_free.var); \
		} \
	}

#define FREE_OP_IF_VAR(should_free) \
	if (should_free.var != NULL && (((zend_uintptr_t)should_free.var & 1L) == 0)) { \
		zval_ptr_dtor_nogc(should_free.var); \
	}

#define FREE_OP_VAR_PTR(should_free) \
	if (should_free.var) { \
		zval_ptr_dtor_nogc(should_free.var); \
	}

#define TMP_FREE(z) (zval*)(((zend_uintptr_t)(z)) | 1L)

#define IS_TMP_FREE(should_free) ((zend_uintptr_t)should_free.var & 1L)

#define MAKE_REAL_ZVAL_PTR(val) \
	do { \
		zval _tmp; \
		ZVAL_COPY_VALUE(&_tmp, (val)); \
		(val) = &_tmp; \
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

ZEND_API zval* zend_get_compiled_variable_value(const zend_execute_data *execute_data, zend_uint var)
{
	return EX_VAR_NUM(var);
}

static zend_always_inline zval *_get_zval_ptr_tmp(zend_uint var, const zend_execute_data *execute_data, zend_free_op *should_free TSRMLS_DC)
{
	zval *ret = EX_VAR(var);

	should_free->var = ret;
	return ret;
}

static zend_always_inline zval *_get_zval_ptr_var(zend_uint var, const zend_execute_data *execute_data, zend_free_op *should_free TSRMLS_DC)
{
	zval *ret = EX_VAR(var);

	if (UNEXPECTED(Z_TYPE_P(ret) == IS_INDIRECT)) {
		ret = Z_INDIRECT_P(ret);
	}
	should_free->var = ret;
	return ret;
}

static zend_always_inline zval *_get_zval_ptr_var_deref(zend_uint var, const zend_execute_data *execute_data, zend_free_op *should_free TSRMLS_DC)
{
	zval *ret = EX_VAR(var);

	if (UNEXPECTED(Z_TYPE_P(ret) == IS_INDIRECT)) {
		ret = Z_INDIRECT_P(ret);
	}
	should_free->var = ret;
	if (UNEXPECTED(Z_TYPE_P(ret) == IS_REFERENCE)) {
		ret = Z_REFVAL_P(ret);
	}
	return ret;
}

static zend_never_inline zval *_get_zval_cv_lookup(zval *ptr, zend_uint var, int type TSRMLS_DC)
{
	zend_string *cv = CV_DEF_OF(var);
	zval *ret = NULL;

	if (EG(active_symbol_table)) {
		ret = zend_hash_find(EG(active_symbol_table), cv);
		if (ret) {
			ZVAL_INDIRECT(ptr, ret);
			return ret;
		}
	}

	switch (type) {
		case BP_VAR_R:
		case BP_VAR_UNSET:
			zend_error(E_NOTICE, "Undefined variable: %s", cv->val);
			/* break missing intentionally */
		case BP_VAR_IS:
			return &EG(uninitialized_zval);
		case BP_VAR_RW:
			zend_error(E_NOTICE, "Undefined variable: %s", cv->val);
			/* break missing intentionally */
		case BP_VAR_W:
			if (EG(active_symbol_table)) {
				ret = zend_hash_update(EG(active_symbol_table), cv, ret);
				ZVAL_INDIRECT(ptr, ret);
			} else {
				ZVAL_NULL(ptr);
				ret = ptr;
			}
			break;
	}
	return ret;
}

static zend_never_inline zval *_get_zval_cv_lookup_BP_VAR_R(zval *ptr, zend_uint var TSRMLS_DC)
{
	zend_string *cv = CV_DEF_OF(var);
	zval *ret = NULL;

	if (EG(active_symbol_table)) {
		ret = zend_hash_find(EG(active_symbol_table), cv);
		if (ret) {
			ZVAL_INDIRECT(ptr, ret);
			return ret;
		}
	}

	zend_error(E_NOTICE, "Undefined variable: %s", cv->val);
	return &EG(uninitialized_zval);
}

static zend_never_inline zval *_get_zval_cv_lookup_BP_VAR_UNSET(zval *ptr, zend_uint var TSRMLS_DC)
{
	zend_string *cv = CV_DEF_OF(var);
	zval *ret;

	if (EG(active_symbol_table)) {
		ret = zend_hash_find(EG(active_symbol_table), cv);
		if (ret) {
			ZVAL_INDIRECT(ptr, ret);
			return ret;
		}
	}

	zend_error(E_NOTICE, "Undefined variable: %s", cv->val);
	return &EG(uninitialized_zval);
}

static zend_never_inline zval *_get_zval_cv_lookup_BP_VAR_IS(zval *ptr, zend_uint var TSRMLS_DC)
{
	zend_string *cv = CV_DEF_OF(var);
	zval *ret;

	if (EG(active_symbol_table)) {
		ret = zend_hash_find(EG(active_symbol_table), cv);
		if (ret) {
			ZVAL_INDIRECT(ptr, ret);
			return ret;
		}
	}

	return &EG(uninitialized_zval);
}

static zend_never_inline zval *_get_zval_cv_lookup_BP_VAR_RW(zval *ptr, zend_uint var TSRMLS_DC)
{
	zend_string *cv = CV_DEF_OF(var);
	zval *ret;

	if (EG(active_symbol_table)) {
		ret = zend_hash_find(EG(active_symbol_table), cv);
		if (ret) {
			ZVAL_INDIRECT(ptr, ret);
			return ret;
		}
		ret = zend_hash_update(EG(active_symbol_table), cv, &EG(uninitialized_zval));
		ZVAL_INDIRECT(ptr, ret);
		zend_error(E_NOTICE, "Undefined variable: %s", cv->val);
		return ret;
	} else {
		ZVAL_NULL(ptr);
		zend_error(E_NOTICE, "Undefined variable: %s", cv->val);
		return ptr;
	}
}

static zend_never_inline zval *_get_zval_cv_lookup_BP_VAR_W(zval *ptr, zend_uint var TSRMLS_DC)
{
	zend_string *cv = CV_DEF_OF(var);
	zval *ret;

	if (EG(active_symbol_table)) {
		ret = zend_hash_find(EG(active_symbol_table), cv);
		if (ret) {
			ZVAL_INDIRECT(ptr, ret);
			return ret;
		}
		ret = zend_hash_update(EG(active_symbol_table), cv, &EG(uninitialized_zval));
		ZVAL_INDIRECT(ptr, ret);
		return ret;
	} else {
		ZVAL_NULL(ptr);
		return ptr;
	}
}

static zend_always_inline zval *_get_zval_ptr_cv(zend_uint var, int type TSRMLS_DC)
{
	zval *ret = EX_VAR_NUM_2(EG(current_execute_data), var);

	if (UNEXPECTED(Z_TYPE_P(ret) == IS_UNDEF)) {
		return _get_zval_cv_lookup(ret, var, type TSRMLS_CC);
	} else if (UNEXPECTED(Z_TYPE_P(ret) == IS_INDIRECT)) {
		ret = Z_INDIRECT_P(ret);
	}
	return ret;
}

static zend_always_inline zval *_get_zval_ptr_cv_deref(zend_uint var, int type TSRMLS_DC)
{
	zval *ret = EX_VAR_NUM_2(EG(current_execute_data), var);

	if (UNEXPECTED(Z_TYPE_P(ret) == IS_UNDEF)) {
		return _get_zval_cv_lookup(ret, var, type TSRMLS_CC);
	} else if (UNEXPECTED(Z_TYPE_P(ret) == IS_INDIRECT)) {
		ret = Z_INDIRECT_P(ret);
	}
	if (UNEXPECTED(Z_TYPE_P(ret) == IS_REFERENCE)) {
		ret = Z_REFVAL_P(ret);
	}
	return ret;
}

static zend_always_inline zval *_get_zval_ptr_cv_BP_VAR_R(const zend_execute_data *execute_data, zend_uint var TSRMLS_DC)
{
	zval *ret = EX_VAR_NUM(var);

	if (UNEXPECTED(Z_TYPE_P(ret) == IS_UNDEF)) {
		return _get_zval_cv_lookup_BP_VAR_R(ret, var TSRMLS_CC);
	} else if (UNEXPECTED(Z_TYPE_P(ret) == IS_INDIRECT)) {
		ret = Z_INDIRECT_P(ret);
	}
	return ret;
}

static zend_always_inline zval *_get_zval_ptr_cv_deref_BP_VAR_R(const zend_execute_data *execute_data, zend_uint var TSRMLS_DC)
{
	zval *ret = EX_VAR_NUM(var);

	if (UNEXPECTED(Z_TYPE_P(ret) == IS_UNDEF)) {
		return _get_zval_cv_lookup_BP_VAR_R(ret, var TSRMLS_CC);
	} else if (UNEXPECTED(Z_TYPE_P(ret) == IS_INDIRECT)) {
		ret = Z_INDIRECT_P(ret);
	}
	if (UNEXPECTED(Z_TYPE_P(ret) == IS_REFERENCE)) {
		ret = Z_REFVAL_P(ret);
	}
	return ret;
}

static zend_always_inline zval *_get_zval_ptr_cv_BP_VAR_UNSET(const zend_execute_data *execute_data, zend_uint var TSRMLS_DC)
{
	zval *ret = EX_VAR_NUM(var);

	if (UNEXPECTED(Z_TYPE_P(ret) == IS_UNDEF)) {
		return _get_zval_cv_lookup_BP_VAR_UNSET(ret, var TSRMLS_CC);
	} else if (UNEXPECTED(Z_TYPE_P(ret) == IS_INDIRECT)) {
		ret = Z_INDIRECT_P(ret);
	}
	return ret;
}

static zend_always_inline zval *_get_zval_ptr_cv_deref_BP_VAR_UNSET(const zend_execute_data *execute_data, zend_uint var TSRMLS_DC)
{
	zval *ret = EX_VAR_NUM(var);

	if (UNEXPECTED(Z_TYPE_P(ret) == IS_UNDEF)) {
		return _get_zval_cv_lookup_BP_VAR_UNSET(ret, var TSRMLS_CC);
	} else if (UNEXPECTED(Z_TYPE_P(ret) == IS_INDIRECT)) {
		ret = Z_INDIRECT_P(ret);
	}
	if (UNEXPECTED(Z_TYPE_P(ret) == IS_REFERENCE)) {
		ret = Z_REFVAL_P(ret);
	}
	return ret;
}

static zend_always_inline zval *_get_zval_ptr_cv_BP_VAR_IS(const zend_execute_data *execute_data, zend_uint var TSRMLS_DC)
{
	zval *ret = EX_VAR_NUM(var);

	if (UNEXPECTED(Z_TYPE_P(ret) == IS_UNDEF)) {
		return _get_zval_cv_lookup_BP_VAR_IS(ret, var TSRMLS_CC);
	} else if (UNEXPECTED(Z_TYPE_P(ret) == IS_INDIRECT)) {
		ret = Z_INDIRECT_P(ret);
	}
	return ret;
}

static zend_always_inline zval *_get_zval_ptr_cv_deref_BP_VAR_IS(const zend_execute_data *execute_data, zend_uint var TSRMLS_DC)
{
	zval *ret = EX_VAR_NUM(var);

	if (UNEXPECTED(Z_TYPE_P(ret) == IS_UNDEF)) {
		return _get_zval_cv_lookup_BP_VAR_IS(ret, var TSRMLS_CC);
	} else if (UNEXPECTED(Z_TYPE_P(ret) == IS_INDIRECT)) {
		ret = Z_INDIRECT_P(ret);
	}
	if (UNEXPECTED(Z_TYPE_P(ret) == IS_REFERENCE)) {
		ret = Z_REFVAL_P(ret);
	}
	return ret;
}

static zend_always_inline zval *_get_zval_ptr_cv_BP_VAR_RW(const zend_execute_data *execute_data, zend_uint var TSRMLS_DC)
{
	zval *ret = EX_VAR_NUM(var);

	if (UNEXPECTED(Z_TYPE_P(ret) == IS_UNDEF)) {
		return _get_zval_cv_lookup_BP_VAR_RW(ret, var TSRMLS_CC);
	} else if (UNEXPECTED(Z_TYPE_P(ret) == IS_INDIRECT)) {
		ret = Z_INDIRECT_P(ret);
	}
	return ret;
}

static zend_always_inline zval *_get_zval_ptr_cv_deref_BP_VAR_RW(const zend_execute_data *execute_data, zend_uint var TSRMLS_DC)
{
	zval *ret = EX_VAR_NUM(var);

	if (UNEXPECTED(Z_TYPE_P(ret) == IS_UNDEF)) {
		return _get_zval_cv_lookup_BP_VAR_RW(ret, var TSRMLS_CC);
	} else if (UNEXPECTED(Z_TYPE_P(ret) == IS_INDIRECT)) {
		ret = Z_INDIRECT_P(ret);
	}
	if (UNEXPECTED(Z_TYPE_P(ret) == IS_REFERENCE)) {
		ret = Z_REFVAL_P(ret);
	}
	return ret;
}

static zend_always_inline zval *_get_zval_ptr_cv_BP_VAR_W(const zend_execute_data *execute_data, zend_uint var TSRMLS_DC)
{
	zval *ret = EX_VAR_NUM(var);

	if (UNEXPECTED(Z_TYPE_P(ret) == IS_UNDEF)) {
		return _get_zval_cv_lookup_BP_VAR_W(ret, var TSRMLS_CC);
	} else if (UNEXPECTED(Z_TYPE_P(ret) == IS_INDIRECT)) {
		ret = Z_INDIRECT_P(ret);
	}
	return ret;
}

static zend_always_inline zval *_get_zval_ptr_cv_deref_BP_VAR_W(const zend_execute_data *execute_data, zend_uint var TSRMLS_DC)
{
	zval *ret = EX_VAR_NUM(var);

	if (UNEXPECTED(Z_TYPE_P(ret) == IS_UNDEF)) {
		return _get_zval_cv_lookup_BP_VAR_W(ret, var TSRMLS_CC);
	} else if (UNEXPECTED(Z_TYPE_P(ret) == IS_INDIRECT)) {
		ret = Z_INDIRECT_P(ret);
	}
	if (UNEXPECTED(Z_TYPE_P(ret) == IS_REFERENCE)) {
		ret = Z_REFVAL_P(ret);
	}
	return ret;
}

static inline zval *_get_zval_ptr(int op_type, const znode_op *node, const zend_execute_data *execute_data, zend_free_op *should_free, int type TSRMLS_DC)
{
	zval *ret;

	switch (op_type) {
		case IS_CONST:
			should_free->var = NULL;
			return node->zv;
			break;
		case IS_TMP_VAR:
			ret = EX_VAR(node->var);
			should_free->var = TMP_FREE(ret);
			return ret;
			break;
		case IS_VAR:
			return _get_zval_ptr_var(node->var, execute_data, should_free TSRMLS_CC);
			break;
		case IS_UNUSED:
			should_free->var = NULL;
			return NULL;
			break;
		case IS_CV:
			should_free->var = NULL;
			return _get_zval_ptr_cv(node->var, type TSRMLS_CC);
			break;
		EMPTY_SWITCH_DEFAULT_CASE()
	}
	return NULL;
}

static inline zval *_get_zval_ptr_deref(int op_type, const znode_op *node, const zend_execute_data *execute_data, zend_free_op *should_free, int type TSRMLS_DC)
{
	zval *ret;

	switch (op_type) {
		case IS_CONST:
			should_free->var = NULL;
			return node->zv;
			break;
		case IS_TMP_VAR:
			ret = EX_VAR(node->var);
			should_free->var = TMP_FREE(ret);
			return ret;
			break;
		case IS_VAR:
			return _get_zval_ptr_var_deref(node->var, execute_data, should_free TSRMLS_CC);
			break;
		case IS_UNUSED:
			should_free->var = NULL;
			return NULL;
			break;
		case IS_CV:
			should_free->var = NULL;
			return _get_zval_ptr_cv_deref(node->var, type TSRMLS_CC);
			break;
		EMPTY_SWITCH_DEFAULT_CASE()
	}
	return NULL;
}

static zend_always_inline zval *_get_zval_ptr_ptr_var(zend_uint var, const zend_execute_data *execute_data, zend_free_op *should_free TSRMLS_DC)
{
	zval *ret = EX_VAR(var);

	if (EXPECTED(Z_TYPE_P(ret) == IS_INDIRECT)) {
		ret = Z_INDIRECT_P(ret);
	} else if (!Z_REFCOUNTED_P(ret)) {
		should_free->var = ret;
		return ret;
	}
	PZVAL_UNLOCK(ret, should_free);
	return ret;
}

//???
#if 0
static zend_always_inline zval *_get_zval_ptr_var_fast(zend_uint var, const zend_execute_data *execute_data, zend_free_op *should_free TSRMLS_DC)
{
	zval* ptr = EX_VAR(var);

	if (EXPECTED(Z_TYPE_P(ptr) != IS_STR_OFFSET)) {
		should_free->var = ptr;
	} else {
		/* string offset */
		should_free->var = Z_STR_OFFSET_P(EX_VAR(var))->str;
	}
	return ptr;
}
#endif

static zend_always_inline zval *_get_obj_zval_ptr_unused(TSRMLS_D)
{
	if (EXPECTED(Z_TYPE(EG(This)) != IS_UNDEF)) {
		return &EG(This);
	} else {
		zend_error_noreturn(E_ERROR, "Using $this when not in object context");
		return NULL;
	}
}

static inline zval *_get_obj_zval_ptr(int op_type, znode_op *op, const zend_execute_data *execute_data, zend_free_op *should_free, int type TSRMLS_DC)
{
	if (op_type == IS_UNUSED) {
		if (EXPECTED(Z_TYPE(EG(This)) != IS_UNDEF)) {
			should_free->var = NULL;
			return &EG(This);
		} else {
			zend_error_noreturn(E_ERROR, "Using $this when not in object context");
		}
	}
	return get_zval_ptr(op_type, op, execute_data, should_free, type);
}

static void zend_assign_to_variable_reference(zval *variable_ptr, zval *value_ptr TSRMLS_DC)
{
	if (variable_ptr == &EG(error_zval) || value_ptr == &EG(error_zval)) {
		ZVAL_NULL(variable_ptr);
	} else if (EXPECTED(variable_ptr != value_ptr)) {
		zval tmp;
		SEPARATE_ZVAL_TO_MAKE_IS_REF(value_ptr);
		ZVAL_COPY(&tmp, value_ptr);
		zval_ptr_dtor(variable_ptr);
		ZVAL_COPY_VALUE(variable_ptr, &tmp);
	} else if (!Z_ISREF_P(variable_ptr)) {
		ZVAL_NEW_REF(variable_ptr, variable_ptr);
	}
}

/* this should modify object only if it's empty */
static inline void make_real_object(zval *object_ptr TSRMLS_DC)
{
	zval *object = object_ptr;

	if (UNEXPECTED(Z_ISREF_P(object_ptr))) {
		object = Z_REFVAL_P(object);
	}
	if (Z_TYPE_P(object) == IS_NULL
		|| (Z_TYPE_P(object) == IS_BOOL && Z_LVAL_P(object) == 0)
		|| (Z_TYPE_P(object) == IS_STRING && Z_STRLEN_P(object) == 0)
	) {
		if (EXPECTED(!Z_ISREF_P(object_ptr))) {
			SEPARATE_ZVAL(object);
		}
		zval_dtor(object);
		object_init(object);
		zend_error(E_WARNING, "Creating default object from empty value");
	}
}

ZEND_API char * zend_verify_arg_class_kind(const zend_arg_info *cur_arg_info, ulong fetch_type, char **class_name, zend_class_entry **pce TSRMLS_DC)
{
	zend_string *key = STR_INIT(cur_arg_info->class_name, cur_arg_info->class_name_len, 0);
	*pce = zend_fetch_class(key, (fetch_type | ZEND_FETCH_CLASS_AUTO | ZEND_FETCH_CLASS_NO_AUTOLOAD) TSRMLS_CC);
	STR_FREE(key);
	*class_name = (*pce) ? (*pce)->name->val : (char*)cur_arg_info->class_name;
	if (*pce && (*pce)->ce_flags & ZEND_ACC_INTERFACE) {
		return "implement interface ";
	} else {
		return "be an instance of ";
	}
}

ZEND_API int zend_verify_arg_error(int error_type, const zend_function *zf, zend_uint arg_num, const char *need_msg, const char *need_kind, const char *given_msg, const char *given_kind TSRMLS_DC)
{
	zend_execute_data *ptr = EG(current_execute_data)->prev_execute_data;
	const char *fname = zf->common.function_name->val;
	const char *fsep;
	const char *fclass;

	if (zf->common.scope) {
		fsep =  "::";
		fclass = zf->common.scope->name->val;
	} else {
		fsep =  "";
		fclass = "";
	}

	if (ptr && ptr->op_array) {
		zend_error(error_type, "Argument %d passed to %s%s%s() must %s%s, %s%s given, called in %s on line %d and defined", arg_num, fclass, fsep, fname, need_msg, need_kind, given_msg, given_kind, ptr->op_array->filename->val, ptr->opline->lineno);
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

	if (!zf->common.arg_info) {
		return 1;
	}

	if (arg_num <= zf->common.num_args) {
		cur_arg_info = &zf->common.arg_info[arg_num-1];
	} else if (zf->common.fn_flags & ZEND_ACC_VARIADIC) {
		cur_arg_info = &zf->common.arg_info[zf->common.num_args-1];
	} else {
		return 1;
	}

	if (cur_arg_info->class_name) {
		char *class_name;

		if (!arg) {
			need_msg = zend_verify_arg_class_kind(cur_arg_info, fetch_type, &class_name, &ce TSRMLS_CC);
			return zend_verify_arg_error(E_RECOVERABLE_ERROR, zf, arg_num, need_msg, class_name, "none", "" TSRMLS_CC);
		}
		if (Z_TYPE_P(arg) == IS_REFERENCE) {
			arg = Z_REFVAL_P(arg);
		}
		if (Z_TYPE_P(arg) == IS_OBJECT) {
			need_msg = zend_verify_arg_class_kind(cur_arg_info, fetch_type, &class_name, &ce TSRMLS_CC);
			if (!ce || !instanceof_function(Z_OBJCE_P(arg), ce TSRMLS_CC)) {
				return zend_verify_arg_error(E_RECOVERABLE_ERROR, zf, arg_num, need_msg, class_name, "instance of ", Z_OBJCE_P(arg)->name->val TSRMLS_CC);
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

				if (Z_TYPE_P(arg) == IS_REFERENCE) {
					arg = Z_REFVAL_P(arg);
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

static inline void zend_assign_to_object(zval *retval, zval *object_ptr, zval *property_name, int value_type, znode_op *value_op, const zend_execute_data *execute_data, int opcode, const zend_literal *key TSRMLS_DC)
{
	zend_free_op free_value;
 	zval *value = get_zval_ptr(value_type, value_op, execute_data, &free_value, BP_VAR_R);
 	zval tmp;
 	zval *object = object_ptr;

 	if (Z_TYPE_P(object) == IS_REFERENCE) {
 		object = Z_REFVAL_P(object);
 	}
	if (Z_TYPE_P(object) != IS_OBJECT) {
		if (object == &EG(error_zval)) {
 			if (retval) {
 				ZVAL_NULL(retval);
			}
			FREE_OP(free_value);
			return;
		}
		if (Z_TYPE_P(object) == IS_NULL ||
		    (Z_TYPE_P(object) == IS_BOOL && Z_LVAL_P(object) == 0) ||
		    (Z_TYPE_P(object) == IS_STRING && Z_STRLEN_P(object) == 0)) {
			if (Z_REFCOUNTED_P(object)) {
				if (!Z_ISREF_P(object_ptr)) {
					SEPARATE_ZVAL(object);
				}
				Z_ADDREF_P(object);
				zend_error(E_WARNING, "Creating default object from empty value");
				if (Z_REFCOUNT_P(object) == 1) {
					/* object was removed by error handler, nothing to assign to */
					zval_ptr_dtor(object);
					if (retval) {
						ZVAL_NULL(retval);
					}
					FREE_OP(free_value);
					return;
				}
				Z_DELREF_P(object);
			} else {
				zend_error(E_WARNING, "Creating default object from empty value");
			}
			zval_dtor(object);
			object_init(object);
		} else {
			zend_error(E_WARNING, "Attempt to assign property of non-object");
			if (retval) {
				ZVAL_NULL(retval);
			}
			FREE_OP(free_value);
			return;
		}
	}

	/* separate our value if necessary */
	if (value_type == IS_TMP_VAR) {
		if (UNEXPECTED(Z_ISREF_P(value))) {
			ZVAL_COPY_VALUE(&tmp, Z_REFVAL_P(value));
        } else {
			ZVAL_COPY_VALUE(&tmp, value);
        }
		value = &tmp;
	} else if (value_type == IS_CONST) {
		if (UNEXPECTED(Z_ISREF_P(value))) {
			value = Z_REFVAL_P(value);
		}
		ZVAL_DUP(&tmp, value);
	} else if (Z_REFCOUNTED_P(value)) {
		Z_ADDREF_P(value);
	}

	if (opcode == ZEND_ASSIGN_OBJ) {
		if (!Z_OBJ_HT_P(object)->write_property) {
			zend_error(E_WARNING, "Attempt to assign property of non-object");
			if (retval) {
				ZVAL_NULL(retval);
			}
			if (value_type == IS_CONST) {
				zval_ptr_dtor(value);
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
		ZVAL_COPY(retval, value);
	}
	zval_ptr_dtor(value);
	FREE_OP_IF_VAR(free_value);
}

static inline int zend_assign_to_string_offset(zval *str_offset, zval *value, int value_type TSRMLS_DC)
{
	zval *str = Z_STR_OFFSET_P(str_offset)->str;
	zend_uint offset = Z_STR_OFFSET_P(str_offset)->offset;

	if ((int)offset < 0) {
		zend_error(E_WARNING, "Illegal string offset:  %d", offset);
		return 0;
	}

	if (offset >= Z_STRLEN_P(str)) {
		int old_len = Z_STRLEN_P(str);
		Z_STR_P(str) = STR_REALLOC(Z_STR_P(str), offset + 1, 0);
		memset(Z_STRVAL_P(str) + old_len, ' ', offset - old_len);
		Z_STRVAL_P(str)[offset+1] = 0;
	} else if (IS_INTERNED(Z_STR_P(str))) {
		Z_STR_P(str) = STR_DUP(Z_STR_P(str), 0);
	}

	if (Z_TYPE_P(value) != IS_STRING) {
		zval tmp;

		ZVAL_COPY_VALUE(&tmp, value);
		if (value_type != IS_TMP_VAR) {
			zval_copy_ctor(&tmp);
		}
		convert_to_string(&tmp);
		Z_STRVAL_P(str)[offset] = Z_STRVAL(tmp)[0];
		zval_dtor(&tmp);
	} else {
		Z_STRVAL_P(str)[offset] = Z_STRVAL_P(value)[0];
		if (value_type == IS_TMP_VAR) {
			/* we can safely free final_value here
			 * because separation is done only
			 * in case value_type == IS_VAR */
			zval_dtor(value);
		}
	}
	/*
	 * the value of an assignment to a string offset is undefined
	T(result->u.var).var = &T->str_offset.str;
	*/
	return 1;
}

static inline zval* zend_assign_tmp_to_variable(zval *variable_ptr, zval *value TSRMLS_DC)
{
	if (Z_ISREF_P(variable_ptr)) {
		variable_ptr = Z_REFVAL_P(variable_ptr);
	}

	if (Z_TYPE_P(variable_ptr) == IS_OBJECT &&
	    UNEXPECTED(Z_OBJ_HANDLER_P(variable_ptr, set) != NULL)) {
		Z_OBJ_HANDLER_P(variable_ptr, set)(variable_ptr, value TSRMLS_CC);
		return variable_ptr;
	}

	if (EXPECTED(!Z_REFCOUNTED_P(variable_ptr))) {
		ZVAL_COPY_VALUE(variable_ptr, value);
 	} else if (UNEXPECTED(Z_REFCOUNT_P(variable_ptr) > 1)) {
		/* we need to split */
		Z_DELREF_P(variable_ptr);
		GC_ZVAL_CHECK_POSSIBLE_ROOT(variable_ptr);
		ZVAL_COPY_VALUE(variable_ptr, value);
	} else {
		zval garbage;

		ZVAL_COPY_VALUE(&garbage, variable_ptr);
		ZVAL_COPY_VALUE(variable_ptr, value);
		_zval_dtor_func(&garbage ZEND_FILE_LINE_CC);
	}
	return variable_ptr;
}

static inline zval* zend_assign_const_to_variable(zval *variable_ptr, zval *value TSRMLS_DC)
{
	if (Z_ISREF_P(variable_ptr)) {
		variable_ptr = Z_REFVAL_P(variable_ptr);
	}

	if (Z_TYPE_P(variable_ptr) == IS_OBJECT &&
	    UNEXPECTED(Z_OBJ_HANDLER_P(variable_ptr, set) != NULL)) {
		Z_OBJ_HANDLER_P(variable_ptr, set)(variable_ptr, value TSRMLS_CC);
		return variable_ptr;
	}

	if (EXPECTED(!Z_REFCOUNTED_P(variable_ptr))) {
		ZVAL_COPY_VALUE(variable_ptr, value);
		if (Z_REFCOUNTED_P(value)) {
			zval_copy_ctor(variable_ptr);
		}
	} else if (UNEXPECTED(Z_REFCOUNT_P(variable_ptr) > 1)) {
		/* we need to split */
		Z_DELREF_P(variable_ptr);
		GC_ZVAL_CHECK_POSSIBLE_ROOT(variable_ptr);
		ZVAL_COPY_VALUE(variable_ptr, value);
		if (Z_REFCOUNTED_P(value)) {
			zval_copy_ctor(variable_ptr);
		}
 	} else {
		zval garbage;

		ZVAL_COPY_VALUE(&garbage, variable_ptr);
		ZVAL_COPY_VALUE(variable_ptr, value);
		if (Z_REFCOUNTED_P(value)) {
			zval_copy_ctor(variable_ptr);
		}
		_zval_dtor_func(&garbage ZEND_FILE_LINE_CC);
	}
	return variable_ptr;
}

static inline zval* zend_assign_to_variable(zval *variable_ptr, zval *value TSRMLS_DC)
{
	zval garbage;
	int is_ref = 0;

	if (EXPECTED(!Z_REFCOUNTED_P(variable_ptr))) {	
		if (EXPECTED(!Z_ISREF_P(value))) {
			ZVAL_COPY(variable_ptr, value);
		} else {
			goto assign_ref;
		}
		return variable_ptr;
	} else if (Z_ISREF_P(variable_ptr)) {
		variable_ptr = Z_REFVAL_P(variable_ptr);
		is_ref = 1;
	}

	if (EXPECTED(!Z_REFCOUNTED_P(variable_ptr))) {
		goto assign_simple;
	} else if (Z_TYPE_P(variable_ptr) == IS_OBJECT &&
	    UNEXPECTED(Z_OBJ_HANDLER_P(variable_ptr, set) != NULL)) {
		Z_OBJ_HANDLER_P(variable_ptr, set)(variable_ptr, value TSRMLS_CC);
	} else if (EXPECTED(variable_ptr != value)) {
		if (Z_REFCOUNT_P(variable_ptr)==1) {
			ZVAL_COPY_VALUE(&garbage, variable_ptr);
			if (EXPECTED(!Z_ISREF_P(value))) {
				if (!is_ref) {
					ZVAL_COPY(variable_ptr, value);
				} else {
					ZVAL_DUP(variable_ptr, value);
				}
			} else {
				if (Z_REFCOUNT_P(value) == 1) {
//??? auto dereferencing
					zend_reference *ref = Z_REF_P(value);
					ZVAL_COPY_VALUE(value, Z_REFVAL_P(value));
					ZVAL_COPY(variable_ptr, value);
					efree(ref);
				} else {
					ZVAL_DUP(variable_ptr, Z_REFVAL_P(value));
				}
			}				
			_zval_dtor_func(&garbage ZEND_FILE_LINE_CC);
		} else { /* we need to split */
			Z_DELREF_P(variable_ptr);
			GC_ZVAL_CHECK_POSSIBLE_ROOT(variable_ptr);
assign_simple:
			if (EXPECTED(!Z_ISREF_P(value))) {
				if (!is_ref) {
					ZVAL_COPY(variable_ptr, value);
				} else {
					ZVAL_DUP(variable_ptr, value);
				}
			} else {
assign_ref:
				if (Z_REFCOUNT_P(value) == 1) {
//??? auto dereferencing
					zend_reference *ref = Z_REF_P(value);
					ZVAL_COPY_VALUE(value, Z_REFVAL_P(value));
					ZVAL_COPY(variable_ptr, value);
					efree(ref);
				} else {
					ZVAL_DUP(variable_ptr, Z_REFVAL_P(value));
				}
			}
		}
	}
	return variable_ptr;
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
			return &EG(symbol_table).ht;
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

static inline zval *zend_fetch_dimension_address_inner(HashTable *ht, const zval *dim, int dim_type, int type TSRMLS_DC)
{
	zval *retval;
	zend_string *offset_key;
	ulong hval;

	switch (dim->type) {
		case IS_NULL:
			offset_key = STR_EMPTY_ALLOC();
			goto fetch_string_dim;

		case IS_STRING:
			offset_key = Z_STR_P(dim);
			if (dim_type != IS_CONST) {
				ZEND_HANDLE_NUMERIC_EX(offset_key->val, offset_key->len + 1, hval, goto num_index);
			}
fetch_string_dim:
			retval = zend_hash_find(ht, offset_key);
			if (retval == NULL) {
				switch (type) {
					case BP_VAR_R:
						zend_error(E_NOTICE, "Undefined index: %s", offset_key->val);
						/* break missing intentionally */
					case BP_VAR_UNSET:
					case BP_VAR_IS:
						retval = &EG(uninitialized_zval);
						break;
					case BP_VAR_RW:
						zend_error(E_NOTICE,"Undefined index: %s", offset_key->val);
						/* break missing intentionally */
					case BP_VAR_W:
						retval = zend_hash_update(ht, offset_key, &EG(uninitialized_zval));
						break;
				}
			}
			break;
		case IS_DOUBLE:
			hval = zend_dval_to_lval(Z_DVAL_P(dim));
			goto num_index;
		case IS_RESOURCE:
			zend_error(E_STRICT, "Resource ID#%ld used as offset, casting to integer (%ld)", Z_RES_HANDLE_P(dim), Z_RES_HANDLE_P(dim));
			hval = Z_RES_HANDLE_P(dim);
			goto num_index;
		case IS_BOOL:
		case IS_LONG:
			hval = Z_LVAL_P(dim);
num_index:
			retval = zend_hash_index_find(ht, hval);
			if (retval == NULL) {
				switch (type) {
					case BP_VAR_R:
						zend_error(E_NOTICE,"Undefined offset: %ld", hval);
						/* break missing intentionally */
					case BP_VAR_UNSET:
					case BP_VAR_IS:
						retval = &EG(uninitialized_zval);
						break;
					case BP_VAR_RW:
						zend_error(E_NOTICE,"Undefined offset: %ld", hval);
						/* break missing intentionally */
					case BP_VAR_W:
						retval = zend_hash_index_update(ht, hval, &EG(uninitialized_zval));
						break;
				}
			}
			break;

		default:
			zend_error(E_WARNING, "Illegal offset type");
			return (type == BP_VAR_W || type == BP_VAR_RW) ?
				&EG(error_zval) : &EG(uninitialized_zval);
	}
	return retval;
}

static void zend_fetch_dimension_address(zval *result, zval *container_ptr, zval *dim, int dim_type, int type TSRMLS_DC)
{
    zval *retval;
    zval *container = container_ptr;

	if (Z_ISREF_P(container)) {
		container = Z_REFVAL_P(container);
	}

	switch (Z_TYPE_P(container)) {
		case IS_ARRAY:
			if (type != BP_VAR_UNSET) {
				SEPARATE_ZVAL(container);
			}
fetch_from_array:
			if (dim == NULL) {
				retval = zend_hash_next_index_insert(Z_ARRVAL_P(container), &EG(uninitialized_zval));
				if (retval == NULL) {
					zend_error(E_WARNING, "Cannot add element to the array as the next element is already occupied");
					retval = &EG(error_zval);
				}
			} else {
				retval = zend_fetch_dimension_address_inner(Z_ARRVAL_P(container), dim, dim_type, type TSRMLS_CC);
			}
//???			ZVAL_COPY(result, retval);
			if (Z_REFCOUNTED_P(retval)) Z_ADDREF_P(retval);
			ZVAL_INDIRECT(result, retval);
			return;
			break;

		case IS_NULL:
			if (container == &EG(error_zval)) {
				ZVAL_INDIRECT(result, &EG(error_zval));
			} else if (type != BP_VAR_UNSET) {
convert_to_array:
				if (Z_TYPE_P(container_ptr) != IS_REFERENCE) {
					SEPARATE_ZVAL(container);
				}

				zval_dtor(container);
				array_init(container);
				goto fetch_from_array;
			} else {
				/* for read-mode only */
				ZVAL_NULL(result);
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
					if (Z_TYPE_P(container_ptr) != IS_REFERENCE) {
						SEPARATE_ZVAL(container);
					}
				}

				if (Z_TYPE_P(dim) != IS_LONG) {

					switch(Z_TYPE_P(dim)) {
						/* case IS_LONG: */
						case IS_STRING:
							if (IS_LONG == is_numeric_string(Z_STRVAL_P(dim), Z_STRLEN_P(dim), NULL, NULL, -1)) {
								break;
							}
							if (type != BP_VAR_UNSET) {
								zend_error(E_WARNING, "Illegal string offset '%s'", Z_STRVAL_P(dim));
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

					ZVAL_DUP(&tmp, dim);
					convert_to_long(&tmp);
					dim = &tmp;
				}
//??? container may became IS_INDIRECT because of symtable initialization in zend_error
				if (Z_TYPE_P(container) == IS_INDIRECT) {
					container = Z_INDIRECT_P(container);
				}
				ZVAL_STR_OFFSET(result, container, Z_LVAL_P(dim));
				if (!IS_INTERNED(Z_STR_P(container))) STR_ADDREF(Z_STR_P(container));
				return;
			}
			break;

		case IS_OBJECT:
			if (!Z_OBJ_HT_P(container)->read_dimension) {
				zend_error_noreturn(E_ERROR, "Cannot use object as array");
			} else {
				zval *overloaded_result;

//???				if (dim_type == IS_TMP_VAR) {
//???					zval *orig = dim;
//???					MAKE_REAL_ZVAL_PTR(dim);
//???					ZVAL_NULL(orig);
//???				}
				overloaded_result = Z_OBJ_HT_P(container)->read_dimension(container, dim, type, result TSRMLS_CC);

				if (overloaded_result && Z_TYPE_P(overloaded_result) != IS_UNDEF) {
					if (!Z_ISREF_P(overloaded_result)) {
						if (Z_REFCOUNTED_P(overloaded_result) &&
						    Z_REFCOUNT_P(overloaded_result) > 1) {
//???
#if 1
//							if (Z_TYPE_P(overloaded_result) != IS_OBJECT) {
								Z_DELREF_P(overloaded_result);
								ZVAL_DUP(result, overloaded_result);
								overloaded_result = result;
//							} else {
								ZVAL_COPY_VALUE(result, overloaded_result);
								overloaded_result = result;
//							}
#else
//???							zval *tmp = overloaded_result;
//???
//???							ALLOC_ZVAL(overloaded_result);
//???							ZVAL_DUP(overloaded_result, tmp);
//???							Z_UNSET_ISREF_P(overloaded_result);
//???							Z_SET_REFCOUNT_P(overloaded_result, 0);
#endif
						}
						if (Z_TYPE_P(overloaded_result) != IS_OBJECT) {
							zend_class_entry *ce = Z_OBJCE_P(container);
							zend_error(E_NOTICE, "Indirect modification of overloaded element of %s has no effect", ce->name->val);
						}
					}
//???					AI_SET_PTR(result, overloaded_result);
//???					PZVAL_LOCK(overloaded_result);
//???					ZVAL_COPY(result, overloaded_result);
					if (result != overloaded_result) {
						if (Z_REFCOUNTED_P(overloaded_result)) Z_ADDREF_P(overloaded_result);
						ZVAL_INDIRECT(result, overloaded_result);
					}
				} else {
					ZVAL_INDIRECT(result, &EG(error_zval));
				}
//???				if (dim_type == IS_TMP_VAR) {
//???					zval_ptr_dtor(dim);
//???				}
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
				ZVAL_NULL(result);
			} else {
				zend_error(E_WARNING, "Cannot use a scalar value as an array");
				ZVAL_INDIRECT(result, &EG(error_zval));
			}
			break;
	}
}

static void zend_fetch_dimension_address_read(zval *result, zval *container, zval *dim, int dim_type, int type TSRMLS_DC)
{
	zval *retval;

	if (UNEXPECTED(Z_TYPE_P(container) == IS_REFERENCE)) {
		container = Z_REFVAL_P(container);
	}
	switch (Z_TYPE_P(container)) {

		case IS_ARRAY:
			retval = zend_fetch_dimension_address_inner(Z_ARRVAL_P(container), dim, dim_type, type TSRMLS_CC);
			ZVAL_COPY(result, retval);
			return;

		case IS_NULL:
			ZVAL_NULL(result);
			return;

		case IS_STRING: {
				zval tmp;
				zend_string *str;

				if (Z_TYPE_P(dim) != IS_LONG) {
					switch(Z_TYPE_P(dim)) {
						/* case IS_LONG: */
						case IS_STRING:
							if (IS_LONG == is_numeric_string(Z_STRVAL_P(dim), Z_STRLEN_P(dim), NULL, NULL, -1)) {
								break;
							}
							if (type != BP_VAR_IS) {
								zend_error(E_WARNING, "Illegal string offset '%s'", Z_STRVAL_P(dim));
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

					ZVAL_DUP(&tmp, dim);
					convert_to_long(&tmp);
					dim = &tmp;
				}

				if (Z_LVAL_P(dim) < 0 || Z_STRLEN_P(container) <= Z_LVAL_P(dim)) {
					if (type != BP_VAR_IS) {
						zend_error(E_NOTICE, "Uninitialized string offset: %ld", Z_LVAL_P(dim));
					}
					str = STR_EMPTY_ALLOC();
				} else {
					str = STR_INIT(Z_STRVAL_P(container) + Z_LVAL_P(dim), 1, 0);
				}
				ZVAL_STR(result, str);
				return;
			}
			break;

		case IS_OBJECT:
			if (!Z_OBJ_HT_P(container)->read_dimension) {
				zend_error_noreturn(E_ERROR, "Cannot use object as array");
			} else {
				zval *overloaded_result;

//???				if (dim_type == IS_TMP_VAR) {
//???					zval *orig = dim;
//???					MAKE_REAL_ZVAL_PTR(dim);
//???					ZVAL_NULL(orig);
//???				}
				overloaded_result = Z_OBJ_HT_P(container)->read_dimension(container, dim, type, result TSRMLS_CC);

				if (result) {
					if (overloaded_result) {
						if (result != overloaded_result) {
							ZVAL_COPY(result, overloaded_result);
						}
					} else {
						ZVAL_NULL(result);
					}
				}
//???				if (dim_type == IS_TMP_VAR) {
//???					zval_ptr_dtor(dim);
//???				}
			}
			return;

		default:
			ZVAL_NULL(result);
			return;
	}
}

static void zend_fetch_property_address(zval *result, zval *container_ptr, zval *prop_ptr, const zend_literal *key, int type TSRMLS_DC)
{
	zval *container = container_ptr;

	if (Z_TYPE_P(container) == IS_REFERENCE) {
		container = Z_REFVAL_P(container);
	}

	if (Z_TYPE_P(container) != IS_OBJECT) {
		if (container == &EG(error_zval)) {
			ZVAL_INDIRECT(result, &EG(error_zval));
			return;
		}

		/* this should modify object only if it's empty */
		if (type != BP_VAR_UNSET &&
		    ((Z_TYPE_P(container) == IS_NULL ||
		     (Z_TYPE_P(container) == IS_BOOL && Z_LVAL_P(container)==0) ||
		     (Z_TYPE_P(container) == IS_STRING && Z_STRLEN_P(container)==0)))) {
			if (!Z_ISREF_P(container_ptr)) {
				SEPARATE_ZVAL(container);
			}
			object_init(container);
		} else {
			zend_error(E_WARNING, "Attempt to modify property of non-object");
			ZVAL_INDIRECT(result, &EG(error_zval));
			return;
		}
	}

	if (Z_OBJ_HT_P(container)->get_property_ptr_ptr) {
		zval *ptr = Z_OBJ_HT_P(container)->get_property_ptr_ptr(container, prop_ptr, type, key TSRMLS_CC);
		if (NULL == ptr) {
			if (Z_OBJ_HT_P(container)->read_property &&
				(ptr = Z_OBJ_HT_P(container)->read_property(container, prop_ptr, type, key, result TSRMLS_CC)) != NULL) {
				if (ptr != result) {
//???				ZVAL_COPY(result, ptr);
					if (Z_REFCOUNTED_P(ptr)) Z_ADDREF_P(ptr);
					ZVAL_INDIRECT(result, ptr);
				}
			} else {
				zend_error_noreturn(E_ERROR, "Cannot access undefined property for object with overloaded property access");
			}
		} else {
//???			ZVAL_COPY(result, ptr);
			if (Z_REFCOUNTED_P(ptr)) Z_ADDREF_P(ptr);
			ZVAL_INDIRECT(result, ptr);
		}
	} else if (Z_OBJ_HT_P(container)->read_property) {
		zval *ptr = Z_OBJ_HT_P(container)->read_property(container, prop_ptr, type, key, result TSRMLS_CC);
		if (ptr != result) {
//???		ZVAL_COPY(result, ptr);
			if (Z_REFCOUNTED_P(ptr)) Z_ADDREF_P(ptr);
			ZVAL_INDIRECT(result, ptr);
		}
	} else {
		zend_error(E_WARNING, "This object doesn't support property references");
		ZVAL_INDIRECT(result, &EG(error_zval));
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
						zval_ptr_dtor(EX_VAR(brk_opline->op1.var));
					}
					break;
				case ZEND_FREE:
					if (!(brk_opline->extended_value & EXT_TYPE_FREE_ON_RETURN)) {
						zval_dtor(EX_VAR(brk_opline->op1.var));
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
	if (fci != NULL) {
		execute_data_ptr->function_state.function->internal_function.handler(
			fci->param_count, fci->retval,
			fci->object_ptr, 1 TSRMLS_CC
		);
	} else {
		zval *return_value = EX_VAR_2(execute_data_ptr, execute_data_ptr->opline->result.var);
		execute_data_ptr->function_state.function->internal_function.handler(
			execute_data_ptr->opline->extended_value, return_value,
			&execute_data_ptr->object, return_value_used TSRMLS_CC
		);
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

static zend_always_inline void i_free_compiled_variables(zend_execute_data *execute_data TSRMLS_DC) /* {{{ */
{
	zval *cv = EX_VAR_NUM_2(execute_data, 0);
	zval *end = cv + EX(op_array)->last_var;
	while (cv != end) {
		zval_ptr_dtor(cv);
		cv++;
 	}
}
/* }}} */

void zend_free_compiled_variables(zend_execute_data *execute_data TSRMLS_DC) /* {{{ */
{
	i_free_compiled_variables(execute_data TSRMLS_CC);
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
 * EG(current_execute_data) -> | zend_execute_data                      |    |
 *                             |     EX(prev_execute_data)              |----+
 *                             +----------------------------------------+
 *     EX_CV_NUM(0) ---------> | VAR[0]                                 |
 *                             | ...                                    |
 *                             | VAR[op_array->last_var-1]              |
 *                             | VAR[op_array->last_var]                |
 *                             | ...                                    |
 *                             | VAR[op_array->last_var+op_array->T-1]  |
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

static zend_always_inline zend_execute_data *i_create_execute_data_from_op_array(zend_op_array *op_array, zval *return_value, zend_bool nested TSRMLS_DC) /* {{{ */
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
	size_t vars_size = ZEND_MM_ALIGNED_SIZE(sizeof(zval)) * (op_array->last_var + op_array->T);
	size_t call_slots_size = ZEND_MM_ALIGNED_SIZE(sizeof(call_slot)) * op_array->nested_calls;
	size_t stack_size = ZEND_MM_ALIGNED_SIZE(sizeof(zval)) * op_array->used_stack;
	size_t total_size = execute_data_size + vars_size + call_slots_size + stack_size;

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
		size_t args_size = ZEND_MM_ALIGNED_SIZE(sizeof(zval)) * (args_count + 1);

		total_size += args_size + execute_data_size;

		EG(argument_stack) = zend_vm_stack_new_page((total_size + (sizeof(zval) - 1)) / sizeof(zval));
		EG(argument_stack)->prev = NULL;
		execute_data = (zend_execute_data*)((char*)ZEND_VM_STACK_ELEMETS(EG(argument_stack)) + execute_data_size + args_size);

		/* copy prev_execute_data */
		EX(prev_execute_data) = (zend_execute_data*)ZEND_VM_STACK_ELEMETS(EG(argument_stack));
		memset(EX(prev_execute_data), 0, sizeof(zend_execute_data));
		EX(prev_execute_data)->function_state.function = (zend_function*)op_array;
		EX(prev_execute_data)->function_state.arguments = (zval*)(((char*)ZEND_VM_STACK_ELEMETS(EG(argument_stack)) + execute_data_size + args_size - sizeof(zval)));

		/* copy arguments */
		ZVAL_LONG(EX(prev_execute_data)->function_state.arguments, args_count);
		if (args_count > 0) {
			zval *arg_src = zend_vm_stack_get_arg_ex(EG(current_execute_data), 1);
			zval *arg_dst = zend_vm_stack_get_arg_ex(EX(prev_execute_data), 1);
			int i;

			for (i = 0; i < args_count; i++) {
				ZVAL_COPY(arg_dst + i, arg_src + i);
			}
		}
	} else {
		execute_data = zend_vm_stack_alloc(total_size TSRMLS_CC);
		EX(prev_execute_data) = EG(current_execute_data);
	}

	memset(EX_VAR_NUM_2(execute_data, 0), 0, sizeof(zval) * op_array->last_var);

	EX(call_slots) = (call_slot*)((char *)execute_data + execute_data_size + vars_size);


	EX(op_array) = op_array;

	EG(argument_stack)->top = (zval*)zend_vm_stack_frame_base(execute_data);

	ZVAL_UNDEF(&EX(object));
	ZVAL_UNDEF(&EX(current_this));
	ZVAL_UNDEF(&EX(old_error_reporting));
	EX(symbol_table) = EG(active_symbol_table);
	EX(call) = NULL;
	EG(current_execute_data) = execute_data;
	EX(nested) = nested;
	EX(delayed_exception) = NULL;
	EX(return_value) = return_value;

	if (!op_array->run_time_cache && op_array->last_cache_slot) {
		op_array->run_time_cache = ecalloc(op_array->last_cache_slot, sizeof(void*));
	}

	if (op_array->this_var != -1 && Z_TYPE(EG(This)) != IS_UNDEF) {
		if (!EG(active_symbol_table)) {
			ZVAL_COPY(EX_VAR_NUM(op_array->this_var), &EG(This));
		} else {
			ZVAL_COPY(EX_VAR_NUM(op_array->this_var), &EG(This));
			zval *zv = zend_hash_str_add(EG(active_symbol_table), "this", sizeof("this")-1, EX_VAR(op_array->this_var));
			if (zv) {
				ZVAL_INDIRECT(EX_VAR_NUM(op_array->this_var), zv);
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

ZEND_API zend_execute_data *zend_create_execute_data_from_op_array(zend_op_array *op_array, zval *return_value, zend_bool nested TSRMLS_DC) /* {{{ */
{
	return i_create_execute_data_from_op_array(op_array, return_value, nested TSRMLS_CC);
}
/* }}} */

static zend_always_inline zend_bool zend_is_by_ref_func_arg_fetch(zend_op *opline, call_slot *call TSRMLS_DC) /* {{{ */
{
	zend_uint arg_num = (opline->extended_value & ZEND_FETCH_ARG_MASK) + call->num_additional_args;
	return ARG_SHOULD_BE_SENT_BY_REF(call->fbc, arg_num);
}
/* }}} */

static zval *zend_vm_stack_push_args_with_copy(int count TSRMLS_DC) /* {{{ */
{
	zend_vm_stack p = EG(argument_stack);

	zend_vm_stack_extend(count + 1 TSRMLS_CC);

	EG(argument_stack)->top += count;
	ZVAL_LONG(EG(argument_stack)->top, count);
	while (count-- > 0) {
		zval *data = --p->top;

		if (UNEXPECTED(p->top == ZEND_VM_STACK_ELEMETS(p))) {
			zend_vm_stack r = p;

			EG(argument_stack)->prev = p->prev;
			p = p->prev;
			efree(r);
		}
		ZVAL_COPY_VALUE(ZEND_VM_STACK_ELEMETS(EG(argument_stack)) + count, data);
	}
	return EG(argument_stack)->top++;
}
/* }}} */

static zend_always_inline zval *zend_vm_stack_push_args(int count TSRMLS_DC) /* {{{ */
{
	if (UNEXPECTED(EG(argument_stack)->top - ZEND_VM_STACK_ELEMETS(EG(argument_stack)) < count)
		|| UNEXPECTED(EG(argument_stack)->top == EG(argument_stack)->end)) {
		return zend_vm_stack_push_args_with_copy(count TSRMLS_CC);
	}
	ZVAL_LONG(EG(argument_stack)->top, count);
	return EG(argument_stack)->top++;
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

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
