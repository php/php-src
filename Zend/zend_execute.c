/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2018 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <zeev@php.net>                                 |
   |          Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

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
#include "zend_inheritance.h"
#include "zend_type_info.h"

/* Virtual current working directory support */
#include "zend_virtual_cwd.h"

#ifdef HAVE_GCC_GLOBAL_REGS
# if defined(__GNUC__) && ZEND_GCC_VERSION >= 4008 && defined(i386)
#  define ZEND_VM_FP_GLOBAL_REG "%esi"
#  define ZEND_VM_IP_GLOBAL_REG "%edi"
# elif defined(__GNUC__) && ZEND_GCC_VERSION >= 4008 && defined(__x86_64__)
#  define ZEND_VM_FP_GLOBAL_REG "%r14"
#  define ZEND_VM_IP_GLOBAL_REG "%r15"
# elif defined(__GNUC__) && ZEND_GCC_VERSION >= 4008 && defined(__powerpc64__)
#  define ZEND_VM_FP_GLOBAL_REG "r28"
#  define ZEND_VM_IP_GLOBAL_REG "r29"
# elif defined(__IBMC__) && ZEND_GCC_VERSION >= 4002 && defined(__powerpc64__)
#  define ZEND_VM_FP_GLOBAL_REG "r28"
#  define ZEND_VM_IP_GLOBAL_REG "r29"
# endif
#endif

#if defined(ZEND_VM_FP_GLOBAL_REG) && ((ZEND_VM_KIND == ZEND_VM_KIND_CALL) || (ZEND_VM_KIND == ZEND_VM_KIND_HYBRID))
# pragma GCC diagnostic ignored "-Wvolatile-register-var"
  register zend_execute_data* volatile execute_data __asm__(ZEND_VM_FP_GLOBAL_REG);
# pragma GCC diagnostic warning "-Wvolatile-register-var"
# define EXECUTE_DATA_D     void
# define EXECUTE_DATA_C
# define EXECUTE_DATA_DC
# define EXECUTE_DATA_CC
# define NO_EXECUTE_DATA_CC
# define OPLINE_D           void
# define OPLINE_C
# define OPLINE_DC
# define OPLINE_CC
#else
# define EXECUTE_DATA_D     zend_execute_data* execute_data
# define EXECUTE_DATA_C     execute_data
# define EXECUTE_DATA_DC    , EXECUTE_DATA_D
# define EXECUTE_DATA_CC    , EXECUTE_DATA_C
# define NO_EXECUTE_DATA_CC , NULL
# define OPLINE_D           const zend_op* opline
# define OPLINE_C           opline
# define OPLINE_DC          , OPLINE_D
# define OPLINE_CC          , OPLINE_C
#endif

#if defined(ZEND_VM_IP_GLOBAL_REG) && ((ZEND_VM_KIND == ZEND_VM_KIND_CALL) || (ZEND_VM_KIND == ZEND_VM_KIND_HYBRID))
# pragma GCC diagnostic ignored "-Wvolatile-register-var"
  register const zend_op* volatile opline __asm__(ZEND_VM_IP_GLOBAL_REG);
# pragma GCC diagnostic warning "-Wvolatile-register-var"
#else
#endif

#define _CONST_CODE  0
#define _TMP_CODE    1
#define _VAR_CODE    2
#define _UNUSED_CODE 3
#define _CV_CODE     4

typedef int (ZEND_FASTCALL *incdec_t)(zval *);

#define get_zval_ptr(op_type, node, should_free, type) _get_zval_ptr(op_type, node, should_free, type EXECUTE_DATA_CC OPLINE_CC)
#define get_zval_ptr_deref(op_type, node, should_free, type) _get_zval_ptr_deref(op_type, node, should_free, type EXECUTE_DATA_CC OPLINE_CC)
#define get_zval_ptr_undef(op_type, node, should_free, type) _get_zval_ptr_undef(op_type, node, should_free, type EXECUTE_DATA_CC OPLINE_CC)
#define get_op_data_zval_ptr_r(op_type, node, should_free) _get_op_data_zval_ptr_r(op_type, node, should_free EXECUTE_DATA_CC OPLINE_CC)
#define get_op_data_zval_ptr_deref_r(op_type, node, should_free) _get_op_data_zval_ptr_deref_r(op_type, node, should_free EXECUTE_DATA_CC OPLINE_CC)
#define get_zval_ptr_ptr(op_type, node, should_free, type) _get_zval_ptr_ptr(op_type, node, should_free, type EXECUTE_DATA_CC)
#define get_zval_ptr_ptr_undef(op_type, node, should_free, type) _get_zval_ptr_ptr(op_type, node, should_free, type EXECUTE_DATA_CC)
#define get_obj_zval_ptr(op_type, node, should_free, type) _get_obj_zval_ptr(op_type, node, should_free, type EXECUTE_DATA_CC OPLINE_CC)
#define get_obj_zval_ptr_undef(op_type, node, should_free, type) _get_obj_zval_ptr_undef(op_type, node, should_free, type EXECUTE_DATA_CC OPLINE_CC)
#define get_obj_zval_ptr_ptr(op_type, node, should_free, type) _get_obj_zval_ptr_ptr(op_type, node, should_free, type EXECUTE_DATA_CC)

#define RETURN_VALUE_USED(opline) ((opline)->result_type != IS_UNUSED)

static ZEND_FUNCTION(pass)
{
}

ZEND_API const zend_internal_function zend_pass_function = {
	ZEND_INTERNAL_FUNCTION, /* type              */
	{0, 0, 0},              /* arg_flags         */
	0,                      /* fn_flags          */
	NULL,                   /* name              */
	NULL,                   /* scope             */
	NULL,                   /* prototype         */
	0,                      /* num_args          */
	0,                      /* required_num_args */
	NULL,                   /* arg_info          */
	ZEND_FN(pass),          /* handler           */
	NULL,                   /* module            */
	{NULL,NULL,NULL,NULL}   /* reserved          */
};

#undef zval_ptr_dtor
#define zval_ptr_dtor(zv) i_zval_ptr_dtor(zv ZEND_FILE_LINE_CC)

#define FREE_VAR_PTR_AND_EXTRACT_RESULT_IF_NECESSARY(free_op, result) do {	\
	zval *__container_to_free = (free_op);									\
	if (UNEXPECTED(__container_to_free)										\
	 && EXPECTED(Z_REFCOUNTED_P(__container_to_free))) {					\
		zend_refcounted *__ref = Z_COUNTED_P(__container_to_free);			\
		if (UNEXPECTED(!GC_DELREF(__ref))) {								\
			zval *__zv = (result);											\
			if (EXPECTED(Z_TYPE_P(__zv) == IS_INDIRECT)) {					\
				ZVAL_COPY(__zv, Z_INDIRECT_P(__zv));						\
			}																\
			rc_dtor_func(__ref);											\
		}																	\
	}																		\
} while (0)

#define FREE_OP(should_free) \
	if (should_free) { \
		zval_ptr_dtor_nogc(should_free); \
	}

#define FREE_UNFETCHED_OP(type, var) \
	if ((type) & (IS_TMP_VAR|IS_VAR)) { \
		zval_ptr_dtor_nogc(EX_VAR(var)); \
	}

#define FREE_OP_VAR_PTR(should_free) \
	if (should_free) { \
		zval_ptr_dtor_nogc(should_free); \
	}

#define CV_DEF_OF(i) (EX(func)->op_array.vars[i])

#define ZEND_VM_STACK_PAGE_SLOTS (16 * 1024) /* should be a power of 2 */

#define ZEND_VM_STACK_PAGE_SIZE  (ZEND_VM_STACK_PAGE_SLOTS * sizeof(zval))

#define ZEND_VM_STACK_PAGE_ALIGNED_SIZE(size, page_size) \
	(((size) + ZEND_VM_STACK_HEADER_SLOTS * sizeof(zval) \
	  + ((page_size) - 1)) & ~((page_size) - 1))

static zend_always_inline zend_vm_stack zend_vm_stack_new_page(size_t size, zend_vm_stack prev) {
	zend_vm_stack page = (zend_vm_stack)emalloc(size);

	page->top = ZEND_VM_STACK_ELEMENTS(page);
	page->end = (zval*)((char*)page + size);
	page->prev = prev;
	return page;
}

ZEND_API void zend_vm_stack_init(void)
{
	EG(vm_stack_page_size) = ZEND_VM_STACK_PAGE_SIZE;
	EG(vm_stack) = zend_vm_stack_new_page(ZEND_VM_STACK_PAGE_SIZE, NULL);
	EG(vm_stack)->top++;
	EG(vm_stack_top) = EG(vm_stack)->top;
	EG(vm_stack_end) = EG(vm_stack)->end;
}

ZEND_API void zend_vm_stack_init_ex(size_t page_size)
{
	/* page_size must be a power of 2 */
	ZEND_ASSERT(page_size > 0 && (page_size & (page_size - 1)) == 0);
	EG(vm_stack_page_size) = page_size;
	EG(vm_stack) = zend_vm_stack_new_page(page_size, NULL);
	EG(vm_stack_top) = EG(vm_stack)->top;
	EG(vm_stack_end) = EG(vm_stack)->end;
}

ZEND_API void zend_vm_stack_destroy(void)
{
	zend_vm_stack stack = EG(vm_stack);

	while (stack != NULL) {
		zend_vm_stack p = stack->prev;
		efree(stack);
		stack = p;
	}
}

ZEND_API void* zend_vm_stack_extend(size_t size)
{
	zend_vm_stack stack;
	void *ptr;

	stack = EG(vm_stack);
	stack->top = EG(vm_stack_top);
	EG(vm_stack) = stack = zend_vm_stack_new_page(
		EXPECTED(size < EG(vm_stack_page_size) - (ZEND_VM_STACK_HEADER_SLOTS * sizeof(zval))) ?
			EG(vm_stack_page_size) : ZEND_VM_STACK_PAGE_ALIGNED_SIZE(size, EG(vm_stack_page_size)),
		stack);
	ptr = stack->top;
	EG(vm_stack_top) = (void*)(((char*)ptr) + size);
	EG(vm_stack_end) = stack->end;
	return ptr;
}

ZEND_API zval* zend_get_compiled_variable_value(const zend_execute_data *execute_data, uint32_t var)
{
	return EX_VAR(var);
}

static zend_always_inline zval *_get_zval_ptr_tmp(uint32_t var, zend_free_op *should_free EXECUTE_DATA_DC)
{
	zval *ret = EX_VAR(var);
	*should_free = ret;

	ZEND_ASSERT(Z_TYPE_P(ret) != IS_REFERENCE);

	return ret;
}

static zend_always_inline zval *_get_zval_ptr_var(uint32_t var, zend_free_op *should_free EXECUTE_DATA_DC)
{
	zval *ret = EX_VAR(var);

	*should_free = ret;
	return ret;
}

static zend_always_inline zval *_get_zval_ptr_var_deref(uint32_t var, zend_free_op *should_free EXECUTE_DATA_DC)
{
	zval *ret = EX_VAR(var);

	*should_free = ret;
	ZVAL_DEREF(ret);
	return ret;
}

static zend_never_inline ZEND_COLD void zval_undefined_cv(uint32_t var EXECUTE_DATA_DC)
{
	if (EXPECTED(EG(exception) == NULL)) {
		zend_string *cv = CV_DEF_OF(EX_VAR_TO_NUM(var));
		zend_error(E_NOTICE, "Undefined variable: %s", ZSTR_VAL(cv));
	}
}

static zend_never_inline ZEND_COLD zval *_get_zval_cv_lookup(zval *ptr, uint32_t var, int type EXECUTE_DATA_DC)
{
	switch (type) {
		case BP_VAR_R:
		case BP_VAR_UNSET:
			zval_undefined_cv(var EXECUTE_DATA_CC);
			/* break missing intentionally */
		case BP_VAR_IS:
			ptr = &EG(uninitialized_zval);
			break;
		case BP_VAR_RW:
			zval_undefined_cv(var EXECUTE_DATA_CC);
			/* break missing intentionally */
		case BP_VAR_W:
			ZVAL_NULL(ptr);
			break;
	}
	return ptr;
}

static zend_never_inline ZEND_COLD zval *_get_zval_cv_lookup_BP_VAR_R(zval *ptr, uint32_t var EXECUTE_DATA_DC)
{
	zval_undefined_cv(var EXECUTE_DATA_CC);
	return &EG(uninitialized_zval);
}

static zend_never_inline ZEND_COLD zval *_get_zval_cv_lookup_BP_VAR_UNSET(zval *ptr, uint32_t var EXECUTE_DATA_DC)
{
	zval_undefined_cv(var EXECUTE_DATA_CC);
	return &EG(uninitialized_zval);
}

static zend_never_inline ZEND_COLD zval *_get_zval_cv_lookup_BP_VAR_RW(zval *ptr, uint32_t var EXECUTE_DATA_DC)
{
	ZVAL_NULL(ptr);
	zval_undefined_cv(var EXECUTE_DATA_CC);
	return ptr;
}

static zend_always_inline zval *_get_zval_ptr_cv(uint32_t var, int type EXECUTE_DATA_DC)
{
	zval *ret = EX_VAR(var);

	if (UNEXPECTED(Z_TYPE_P(ret) == IS_UNDEF)) {
		if (type == BP_VAR_W) {
			ZVAL_NULL(ret);
		} else {
			return _get_zval_cv_lookup(ret, var, type EXECUTE_DATA_CC);
		}
	}
	return ret;
}

static zend_always_inline zval *_get_zval_ptr_cv_deref(uint32_t var, int type EXECUTE_DATA_DC)
{
	zval *ret = EX_VAR(var);

	if (UNEXPECTED(Z_TYPE_P(ret) == IS_UNDEF)) {
		if (type == BP_VAR_W) {
			ZVAL_NULL(ret);
			return ret;
		} else {
			return _get_zval_cv_lookup(ret, var, type EXECUTE_DATA_CC);
		}
	}
	ZVAL_DEREF(ret);
	return ret;
}

static zend_always_inline zval *_get_zval_ptr_cv_BP_VAR_R(uint32_t var EXECUTE_DATA_DC)
{
	zval *ret = EX_VAR(var);

	if (UNEXPECTED(Z_TYPE_P(ret) == IS_UNDEF)) {
		return _get_zval_cv_lookup_BP_VAR_R(ret, var EXECUTE_DATA_CC);
	}
	return ret;
}

static zend_always_inline zval *_get_zval_ptr_cv_deref_BP_VAR_R(uint32_t var EXECUTE_DATA_DC)
{
	zval *ret = EX_VAR(var);

	if (UNEXPECTED(Z_TYPE_P(ret) == IS_UNDEF)) {
		return _get_zval_cv_lookup_BP_VAR_R(ret, var EXECUTE_DATA_CC);
	}
	ZVAL_DEREF(ret);
	return ret;
}

static zend_always_inline zval *_get_zval_ptr_cv_BP_VAR_UNSET(uint32_t var EXECUTE_DATA_DC)
{
	zval *ret = EX_VAR(var);

	if (UNEXPECTED(Z_TYPE_P(ret) == IS_UNDEF)) {
		return _get_zval_cv_lookup_BP_VAR_UNSET(ret, var EXECUTE_DATA_CC);
	}
	return ret;
}

static zend_always_inline zval *_get_zval_ptr_cv_deref_BP_VAR_UNSET(uint32_t var EXECUTE_DATA_DC)
{
	zval *ret = EX_VAR(var);

	if (UNEXPECTED(Z_TYPE_P(ret) == IS_UNDEF)) {
		return _get_zval_cv_lookup_BP_VAR_UNSET(ret, var EXECUTE_DATA_CC);
	}
	ZVAL_DEREF(ret);
	return ret;
}

static zend_always_inline zval *_get_zval_ptr_cv_BP_VAR_IS(uint32_t var EXECUTE_DATA_DC)
{
	zval *ret = EX_VAR(var);

	return ret;
}

static zend_always_inline zval *_get_zval_ptr_cv_deref_BP_VAR_IS(uint32_t var EXECUTE_DATA_DC)
{
	zval *ret = EX_VAR(var);

	ZVAL_DEREF(ret);
	return ret;
}

static zend_always_inline zval *_get_zval_ptr_cv_BP_VAR_RW(uint32_t var EXECUTE_DATA_DC)
{
	zval *ret = EX_VAR(var);

	if (UNEXPECTED(Z_TYPE_P(ret) == IS_UNDEF)) {
		return _get_zval_cv_lookup_BP_VAR_RW(ret, var EXECUTE_DATA_CC);
	}
	return ret;
}

static zend_always_inline zval *_get_zval_ptr_cv_deref_BP_VAR_RW(uint32_t var EXECUTE_DATA_DC)
{
	zval *ret = EX_VAR(var);

	if (UNEXPECTED(Z_TYPE_P(ret) == IS_UNDEF)) {
		return _get_zval_cv_lookup_BP_VAR_RW(ret, var EXECUTE_DATA_CC);
	}
	ZVAL_DEREF(ret);
	return ret;
}

static zend_always_inline zval *_get_zval_ptr_cv_BP_VAR_W(uint32_t var EXECUTE_DATA_DC)
{
	zval *ret = EX_VAR(var);

	if (Z_TYPE_P(ret) == IS_UNDEF) {
		ZVAL_NULL(ret);
	}
	return ret;
}

static zend_always_inline zval *_get_zval_ptr_cv_deref_BP_VAR_W(uint32_t var EXECUTE_DATA_DC)
{
	zval *ret = EX_VAR(var);

	if (Z_TYPE_P(ret) == IS_UNDEF) {
		ZVAL_NULL(ret);
		return ret;
	}
	ZVAL_DEREF(ret);
	return ret;
}

static zend_always_inline zval *_get_zval_ptr(int op_type, znode_op node, zend_free_op *should_free, int type EXECUTE_DATA_DC OPLINE_DC)
{
	if (op_type & (IS_TMP_VAR|IS_VAR)) {
		if (op_type == IS_TMP_VAR) {
			return _get_zval_ptr_tmp(node.var, should_free EXECUTE_DATA_CC);
		} else {
			ZEND_ASSERT(op_type == IS_VAR);
			return _get_zval_ptr_var(node.var, should_free EXECUTE_DATA_CC);
		}
	} else {
		*should_free = NULL;
		if (op_type == IS_CONST) {
			return RT_CONSTANT(opline, node);
		} else if (op_type == IS_CV) {
			return _get_zval_ptr_cv(node.var, type EXECUTE_DATA_CC);
		} else {
			return NULL;
		}
	}
}

static zend_always_inline zval *_get_op_data_zval_ptr_r(int op_type, znode_op node, zend_free_op *should_free EXECUTE_DATA_DC OPLINE_DC)
{
	if (op_type & (IS_TMP_VAR|IS_VAR)) {
		if (op_type == IS_TMP_VAR) {
			return _get_zval_ptr_tmp(node.var, should_free EXECUTE_DATA_CC);
		} else {
			ZEND_ASSERT(op_type == IS_VAR);
			return _get_zval_ptr_var(node.var, should_free EXECUTE_DATA_CC);
		}
	} else {
		*should_free = NULL;
		if (op_type == IS_CONST) {
			return RT_CONSTANT(opline + 1, node);
		} else if (op_type == IS_CV) {
			return _get_zval_ptr_cv_BP_VAR_R(node.var EXECUTE_DATA_CC);
		} else {
			return NULL;
		}
	}
}

static zend_always_inline zval *_get_zval_ptr_deref(int op_type, znode_op node, zend_free_op *should_free, int type EXECUTE_DATA_DC OPLINE_DC)
{
	if (op_type & (IS_TMP_VAR|IS_VAR)) {
		if (op_type == IS_TMP_VAR) {
			return _get_zval_ptr_tmp(node.var, should_free EXECUTE_DATA_CC);
		} else {
			ZEND_ASSERT(op_type == IS_VAR);
			return _get_zval_ptr_var_deref(node.var, should_free EXECUTE_DATA_CC);
		}
	} else {
		*should_free = NULL;
		if (op_type == IS_CONST) {
			return RT_CONSTANT(opline, node);
		} else if (op_type == IS_CV) {
			return _get_zval_ptr_cv_deref(node.var, type EXECUTE_DATA_CC);
		} else {
			return NULL;
		}
	}
}

static zend_always_inline zval *_get_op_data_zval_ptr_deref_r(int op_type, znode_op node, zend_free_op *should_free EXECUTE_DATA_DC OPLINE_DC)
{
	if (op_type & (IS_TMP_VAR|IS_VAR)) {
		if (op_type == IS_TMP_VAR) {
			return _get_zval_ptr_tmp(node.var, should_free EXECUTE_DATA_CC);
		} else {
			ZEND_ASSERT(op_type == IS_VAR);
			return _get_zval_ptr_var_deref(node.var, should_free EXECUTE_DATA_CC);
		}
	} else {
		*should_free = NULL;
		if (op_type == IS_CONST) {
			return RT_CONSTANT(opline + 1, node);
		} else if (op_type == IS_CV) {
			return _get_zval_ptr_cv_deref_BP_VAR_R(node.var EXECUTE_DATA_CC);
		} else {
			return NULL;
		}
	}
}

static zend_always_inline zval *_get_zval_ptr_undef(int op_type, znode_op node, zend_free_op *should_free, int type EXECUTE_DATA_DC OPLINE_DC)
{
	if (op_type & (IS_TMP_VAR|IS_VAR)) {
		if (op_type == IS_TMP_VAR) {
			return _get_zval_ptr_tmp(node.var, should_free EXECUTE_DATA_CC);
		} else {
			ZEND_ASSERT(op_type == IS_VAR);
			return _get_zval_ptr_var(node.var, should_free EXECUTE_DATA_CC);
		}
	} else {
		*should_free = NULL;
		if (op_type == IS_CONST) {
			return RT_CONSTANT(opline, node);
		} else if (op_type == IS_CV) {
			return EX_VAR(node.var);
		} else {
			return NULL;
		}
	}
}

static zend_always_inline zval *_get_zval_ptr_ptr_var(uint32_t var, zend_free_op *should_free EXECUTE_DATA_DC)
{
	zval *ret = EX_VAR(var);

	if (EXPECTED(Z_TYPE_P(ret) == IS_INDIRECT)) {
		*should_free = NULL;
		ret = Z_INDIRECT_P(ret);
	} else {
		*should_free = ret;
	}
	return ret;
}

static inline zval *_get_zval_ptr_ptr(int op_type, znode_op node, zend_free_op *should_free, int type EXECUTE_DATA_DC)
{
	if (op_type == IS_CV) {
		*should_free = NULL;
		return _get_zval_ptr_cv(node.var, type EXECUTE_DATA_CC);
	} else /* if (op_type == IS_VAR) */ {
		ZEND_ASSERT(op_type == IS_VAR);
		return _get_zval_ptr_ptr_var(node.var, should_free EXECUTE_DATA_CC);
	}
}

static inline zval *_get_obj_zval_ptr(int op_type, znode_op op, zend_free_op *should_free, int type EXECUTE_DATA_DC OPLINE_DC)
{
	if (op_type == IS_UNUSED) {
		*should_free = NULL;
		return &EX(This);
	}
	return get_zval_ptr(op_type, op, should_free, type);
}

static inline zval *_get_obj_zval_ptr_undef(int op_type, znode_op op, zend_free_op *should_free, int type EXECUTE_DATA_DC OPLINE_DC)
{
	if (op_type == IS_UNUSED) {
		*should_free = NULL;
		return &EX(This);
	}
	return get_zval_ptr_undef(op_type, op, should_free, type);
}

static inline zval *_get_obj_zval_ptr_ptr(int op_type, znode_op node, zend_free_op *should_free, int type EXECUTE_DATA_DC)
{
	if (op_type == IS_UNUSED) {
		*should_free = NULL;
		return &EX(This);
	}
	return get_zval_ptr_ptr(op_type, node, should_free, type);
}

static inline void zend_assign_to_variable_reference(zval *variable_ptr, zval *value_ptr)
{
	zend_reference *ref;

	if (EXPECTED(!Z_ISREF_P(value_ptr))) {
		ZVAL_NEW_REF(value_ptr, value_ptr);
	} else if (UNEXPECTED(variable_ptr == value_ptr)) {
		return;
	}

	ref = Z_REF_P(value_ptr);
	GC_ADDREF(ref);
	if (Z_REFCOUNTED_P(variable_ptr)) {
		zend_refcounted *garbage = Z_COUNTED_P(variable_ptr);

		if (GC_DELREF(garbage) == 0) {
			ZVAL_REF(variable_ptr, ref);
			rc_dtor_func(garbage);
			return;
		} else {
			gc_check_possible_root(garbage);
		}
	}
	ZVAL_REF(variable_ptr, ref);
}

static zend_never_inline ZEND_COLD int zend_wrong_assign_to_variable_reference(zval *variable_ptr, zval *value_ptr, zend_uchar value_type OPLINE_DC EXECUTE_DATA_DC)
{
	zend_error(E_NOTICE, "Only variables should be assigned by reference");
	if (UNEXPECTED(EG(exception) != NULL)) {
		return 0;
	}

	value_ptr = zend_assign_to_variable(variable_ptr, value_ptr, value_type);

	if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
		ZVAL_COPY(EX_VAR(opline->result.var), value_ptr);
	}
	return 1;
}

static zend_never_inline ZEND_COLD void ZEND_FASTCALL zend_wrong_property_assignment(zval *property OPLINE_DC EXECUTE_DATA_DC)
{
	zend_string *tmp_property_name;
	zend_string *property_name = zval_get_tmp_string(property, &tmp_property_name);
	zend_error(E_WARNING, "Attempt to assign property '%s' of non-object", ZSTR_VAL(property_name));
	zend_tmp_string_release(tmp_property_name);
	if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
		ZVAL_NULL(EX_VAR(opline->result.var));
	}
}

/* this should modify object only if it's empty */
static zend_never_inline ZEND_COLD int ZEND_FASTCALL make_real_object(zval *object, zval *property OPLINE_DC EXECUTE_DATA_DC)
{
	zend_object *obj;

	if (EXPECTED(Z_TYPE_P(object) <= IS_FALSE)) {
		/* nothing to destroy */
	} else if (EXPECTED((Z_TYPE_P(object) == IS_STRING && Z_STRLEN_P(object) == 0))) {
		zval_ptr_dtor_nogc(object);
	} else {
		if (opline->op1_type != IS_VAR || EXPECTED(!Z_ISERROR_P(object))) {
			zend_string *tmp_property_name;
			zend_string *property_name = zval_get_tmp_string(property, &tmp_property_name);

			if (opline->opcode == ZEND_PRE_INC_OBJ
			 || opline->opcode == ZEND_PRE_DEC_OBJ
			 || opline->opcode == ZEND_POST_INC_OBJ
			 || opline->opcode == ZEND_POST_DEC_OBJ) {
				zend_error(E_WARNING, "Attempt to increment/decrement property '%s' of non-object", ZSTR_VAL(property_name));
			} else {
				zend_error(E_WARNING, "Attempt to assign property '%s' of non-object", ZSTR_VAL(property_name));
			}
			zend_tmp_string_release(tmp_property_name);
		}
		if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
			ZVAL_NULL(EX_VAR(opline->result.var));
		}
		return 0;
	}
	object_init(object);
	Z_ADDREF_P(object);
	obj = Z_OBJ_P(object);
	zend_error(E_WARNING, "Creating default object from empty value");
	if (GC_REFCOUNT(obj) == 1) {
		/* the enclosing container was deleted, obj is unreferenced */
		OBJ_RELEASE(obj);
		if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
			ZVAL_NULL(EX_VAR(opline->result.var));
		}
		return 0;
	}
	Z_DELREF_P(object);
	return 1;
}

static zend_never_inline ZEND_COLD int ZEND_FASTCALL make_real_object_rw(zval *object, zval *property OPLINE_DC)
{
	if (EXPECTED(Z_TYPE_P(object) <= IS_FALSE)) {
		/* nothing to destroy */
	} else if (EXPECTED((Z_TYPE_P(object) == IS_STRING && Z_STRLEN_P(object) == 0))) {
		zval_ptr_dtor_nogc(object);
	} else {
		if (opline->op1_type != IS_VAR || EXPECTED(!Z_ISERROR_P(object))) {
			zend_string *tmp_property_name;
			zend_string *property_name = zval_get_tmp_string(property, &tmp_property_name);
			zend_error(E_WARNING, "Attempt to modify property '%s' of non-object", ZSTR_VAL(property_name));
			zend_tmp_string_release(tmp_property_name);
		}
		return 0;
	}
	object_init(object);
	return 1;
}

static ZEND_COLD void zend_verify_type_error_common(
		const zend_function *zf, const zend_arg_info *arg_info,
		const zend_class_entry *ce, zval *value,
		const char **fname, const char **fsep, const char **fclass,
		const char **need_msg, const char **need_kind, const char **need_or_null,
		const char **given_msg, const char **given_kind)
{
	zend_bool is_interface = 0;
	*fname = ZSTR_VAL(zf->common.function_name);
	if (zf->common.scope) {
		*fsep =  "::";
		*fclass = ZSTR_VAL(zf->common.scope->name);
	} else {
		*fsep =  "";
		*fclass = "";
	}

	if (ZEND_TYPE_IS_CLASS(arg_info->type)) {
		if (ce) {
			if (ce->ce_flags & ZEND_ACC_INTERFACE) {
				*need_msg = "implement interface ";
				is_interface = 1;
			} else {
				*need_msg = "be an instance of ";
			}
			*need_kind = ZSTR_VAL(ce->name);
		} else {
			/* We don't know whether it's a class or interface, assume it's a class */

			*need_msg = "be an instance of ";
			*need_kind = ZSTR_VAL(ZEND_TYPE_NAME(arg_info->type));
		}
	} else {
		switch (ZEND_TYPE_CODE(arg_info->type)) {
			case IS_OBJECT:
				*need_msg = "be an ";
				*need_kind = "object";
				break;
			case IS_CALLABLE:
				*need_msg = "be callable";
				*need_kind = "";
				break;
			case IS_ITERABLE:
				*need_msg = "be iterable";
				*need_kind = "";
				break;
			default:
				*need_msg = "be of the type ";
				*need_kind = zend_get_type_by_const(ZEND_TYPE_CODE(arg_info->type));
				break;
		}
	}

	if (ZEND_TYPE_ALLOW_NULL(arg_info->type)) {
		*need_or_null = is_interface ? " or be null" : " or null";
	} else {
		*need_or_null = "";
	}

	if (value) {
		if (ZEND_TYPE_IS_CLASS(arg_info->type) && Z_TYPE_P(value) == IS_OBJECT) {
			*given_msg = "instance of ";
			*given_kind = ZSTR_VAL(Z_OBJCE_P(value)->name);
		} else {
			*given_msg = zend_zval_type_name(value);
			*given_kind = "";
		}
	} else {
		*given_msg = "none";
		*given_kind = "";
	}
}

static ZEND_COLD void zend_verify_arg_error(
		const zend_function *zf, const zend_arg_info *arg_info,
		int arg_num, const zend_class_entry *ce, zval *value)
{
	zend_execute_data *ptr = EG(current_execute_data)->prev_execute_data;
	const char *fname, *fsep, *fclass;
	const char *need_msg, *need_kind, *need_or_null, *given_msg, *given_kind;

	if (value) {
		zend_verify_type_error_common(
			zf, arg_info, ce, value,
			&fname, &fsep, &fclass, &need_msg, &need_kind, &need_or_null, &given_msg, &given_kind);

		if (zf->common.type == ZEND_USER_FUNCTION) {
			if (ptr && ptr->func && ZEND_USER_CODE(ptr->func->common.type)) {
				zend_type_error("Argument %d passed to %s%s%s() must %s%s%s, %s%s given, called in %s on line %d",
						arg_num, fclass, fsep, fname, need_msg, need_kind, need_or_null, given_msg, given_kind,
						ZSTR_VAL(ptr->func->op_array.filename), ptr->opline->lineno);
			} else {
				zend_type_error("Argument %d passed to %s%s%s() must %s%s%s, %s%s given", arg_num, fclass, fsep, fname, need_msg, need_kind, need_or_null, given_msg, given_kind);
			}
		} else {
			zend_type_error("Argument %d passed to %s%s%s() must %s%s%s, %s%s given", arg_num, fclass, fsep, fname, need_msg, need_kind, need_or_null, given_msg, given_kind);
		}
	} else {
		zend_missing_arg_error(ptr);
	}
}

static int is_null_constant(zend_class_entry *scope, zval *default_value)
{
	if (Z_TYPE_P(default_value) == IS_CONSTANT_AST) {
		zval constant;

		ZVAL_COPY(&constant, default_value);
		if (UNEXPECTED(zval_update_constant_ex(&constant, scope) != SUCCESS)) {
			return 0;
		}
		if (Z_TYPE(constant) == IS_NULL) {
			return 1;
		}
		zval_ptr_dtor_nogc(&constant);
	}
	return 0;
}

static zend_bool zend_verify_weak_scalar_type_hint(zend_uchar type_hint, zval *arg)
{
	switch (type_hint) {
		case _IS_BOOL: {
			zend_bool dest;

			if (!zend_parse_arg_bool_weak(arg, &dest)) {
				return 0;
			}
			zval_ptr_dtor(arg);
			ZVAL_BOOL(arg, dest);
			return 1;
		}
		case IS_LONG: {
			zend_long dest;

			if (!zend_parse_arg_long_weak(arg, &dest)) {
				return 0;
			}
			zval_ptr_dtor(arg);
			ZVAL_LONG(arg, dest);
			return 1;
		}
		case IS_DOUBLE: {
			double dest;

			if (!zend_parse_arg_double_weak(arg, &dest)) {
				return 0;
			}
			zval_ptr_dtor(arg);
			ZVAL_DOUBLE(arg, dest);
			return 1;
		}
		case IS_STRING: {
			zend_string *dest;

			/* on success "arg" is converted to IS_STRING */
			if (!zend_parse_arg_str_weak(arg, &dest)) {
				return 0;
			}
			return 1;
		}
		default:
			return 0;
	}
}

static zend_bool zend_verify_scalar_type_hint(zend_uchar type_hint, zval *arg, zend_bool strict)
{
	if (UNEXPECTED(strict)) {
		/* SSTH Exception: IS_LONG may be accepted as IS_DOUBLE (converted) */
		if (type_hint != IS_DOUBLE || Z_TYPE_P(arg) != IS_LONG) {
			return 0;
		}
	} else if (UNEXPECTED(Z_TYPE_P(arg) == IS_NULL)) {
		/* NULL may be accepted only by nullable hints (this is already checked) */
		return 0;
	}
	return zend_verify_weak_scalar_type_hint(type_hint, arg);
}

static zend_always_inline zend_bool zend_check_type(
		zend_type type,
		zval *arg, zend_class_entry **ce, void **cache_slot,
		zval *default_value, zend_class_entry *scope,
		zend_bool is_return_type)
{
	if (!ZEND_TYPE_IS_SET(type)) {
		return 1;
	}

	ZVAL_DEREF(arg);
	if (ZEND_TYPE_IS_CLASS(type)) {
		if (EXPECTED(*cache_slot)) {
			*ce = (zend_class_entry *) *cache_slot;
		} else {
			*ce = zend_fetch_class(ZEND_TYPE_NAME(type), (ZEND_FETCH_CLASS_AUTO | ZEND_FETCH_CLASS_NO_AUTOLOAD));
			if (UNEXPECTED(!*ce)) {
				return Z_TYPE_P(arg) == IS_NULL && (ZEND_TYPE_ALLOW_NULL(type) || (default_value && is_null_constant(scope, default_value)));
			}
			*cache_slot = (void *) *ce;
		}
		if (EXPECTED(Z_TYPE_P(arg) == IS_OBJECT)) {
			return instanceof_function(Z_OBJCE_P(arg), *ce);
		}
		return Z_TYPE_P(arg) == IS_NULL && (ZEND_TYPE_ALLOW_NULL(type) || (default_value && is_null_constant(scope, default_value)));
	} else if (EXPECTED(ZEND_TYPE_CODE(type) == Z_TYPE_P(arg))) {
		return 1;
	}

	if (Z_TYPE_P(arg) == IS_NULL && (ZEND_TYPE_ALLOW_NULL(type) || (default_value && is_null_constant(scope, default_value)))) {
		/* Null passed to nullable type */
		return 1;
	}

	if (ZEND_TYPE_CODE(type) == IS_CALLABLE) {
		return zend_is_callable(arg, IS_CALLABLE_CHECK_SILENT, NULL);
	} else if (ZEND_TYPE_CODE(type) == IS_ITERABLE) {
		return zend_is_iterable(arg);
	} else if (ZEND_TYPE_CODE(type) == _IS_BOOL &&
			   EXPECTED(Z_TYPE_P(arg) == IS_FALSE || Z_TYPE_P(arg) == IS_TRUE)) {
		return 1;
	} else {
		return zend_verify_scalar_type_hint(ZEND_TYPE_CODE(type), arg,
			is_return_type ? ZEND_RET_USES_STRICT_TYPES() : ZEND_ARG_USES_STRICT_TYPES());
	}

	/* Special handling for IS_VOID is not necessary (for return types),
	 * because this case is already checked at compile-time. */
}

static zend_always_inline int zend_verify_arg_type(zend_function *zf, uint32_t arg_num, zval *arg, zval *default_value, void **cache_slot)
{
	zend_arg_info *cur_arg_info;
	zend_class_entry *ce;

	if (EXPECTED(arg_num <= zf->common.num_args)) {
		cur_arg_info = &zf->common.arg_info[arg_num-1];
	} else if (UNEXPECTED(zf->common.fn_flags & ZEND_ACC_VARIADIC)) {
		cur_arg_info = &zf->common.arg_info[zf->common.num_args];
	} else {
		return 1;
	}

	ce = NULL;
	if (UNEXPECTED(!zend_check_type(cur_arg_info->type, arg, &ce, cache_slot, default_value, zf->common.scope, 0))) {
		zend_verify_arg_error(zf, cur_arg_info, arg_num, ce, arg);
		return 0;
	}

	return 1;
}

static zend_never_inline int zend_verify_internal_arg_types(zend_function *fbc, zend_execute_data *call)
{
	uint32_t i;
	uint32_t num_args = ZEND_CALL_NUM_ARGS(call);
	zval *p = ZEND_CALL_ARG(call, 1);
	void *dummy_cache_slot;

	for (i = 0; i < num_args; ++i) {
		dummy_cache_slot = NULL;
		if (UNEXPECTED(!zend_verify_arg_type(fbc, i + 1, p, NULL, &dummy_cache_slot))) {
			EG(current_execute_data) = call->prev_execute_data;
			zend_vm_stack_free_args(call);
			return 0;
		}
		p++;
	}
	return 1;
}

ZEND_API ZEND_COLD void ZEND_FASTCALL zend_missing_arg_error(zend_execute_data *execute_data)
{
	zend_execute_data *ptr = EX(prev_execute_data);

	if (ptr && ptr->func && ZEND_USER_CODE(ptr->func->common.type)) {
		zend_throw_error(zend_ce_argument_count_error, "Too few arguments to function %s%s%s(), %d passed in %s on line %d and %s %d expected",
			EX(func)->common.scope ? ZSTR_VAL(EX(func)->common.scope->name) : "",
			EX(func)->common.scope ? "::" : "",
			ZSTR_VAL(EX(func)->common.function_name),
			EX_NUM_ARGS(),
			ZSTR_VAL(ptr->func->op_array.filename),
			ptr->opline->lineno,
			EX(func)->common.required_num_args == EX(func)->common.num_args ? "exactly" : "at least",
			EX(func)->common.required_num_args);
	} else {
		zend_throw_error(zend_ce_argument_count_error, "Too few arguments to function %s%s%s(), %d passed and %s %d expected",
			EX(func)->common.scope ? ZSTR_VAL(EX(func)->common.scope->name) : "",
			EX(func)->common.scope ? "::" : "",
			ZSTR_VAL(EX(func)->common.function_name),
			EX_NUM_ARGS(),
			EX(func)->common.required_num_args == EX(func)->common.num_args ? "exactly" : "at least",
			EX(func)->common.required_num_args);
	}
}

static ZEND_COLD void zend_verify_return_error(
		const zend_function *zf, const zend_class_entry *ce, zval *value)
{
	const zend_arg_info *arg_info = &zf->common.arg_info[-1];
	const char *fname, *fsep, *fclass;
	const char *need_msg, *need_kind, *need_or_null, *given_msg, *given_kind;

	zend_verify_type_error_common(
		zf, arg_info, ce, value,
		&fname, &fsep, &fclass, &need_msg, &need_kind, &need_or_null, &given_msg, &given_kind);

	zend_type_error("Return value of %s%s%s() must %s%s%s, %s%s returned",
		fclass, fsep, fname, need_msg, need_kind, need_or_null, given_msg, given_kind);
}

#if ZEND_DEBUG
static ZEND_COLD void zend_verify_internal_return_error(
		const zend_function *zf, const zend_class_entry *ce, zval *value)
{
	const zend_arg_info *arg_info = &zf->common.arg_info[-1];
	const char *fname, *fsep, *fclass;
	const char *need_msg, *need_kind, *need_or_null, *given_msg, *given_kind;

	zend_verify_type_error_common(
		zf, arg_info, ce, value,
		&fname, &fsep, &fclass, &need_msg, &need_kind, &need_or_null, &given_msg, &given_kind);

	zend_error_noreturn(E_CORE_ERROR, "Return value of %s%s%s() must %s%s%s, %s%s returned",
		fclass, fsep, fname, need_msg, need_kind, need_or_null, given_msg, given_kind);
}

static ZEND_COLD void zend_verify_void_return_error(const zend_function *zf, const char *returned_msg, const char *returned_kind)
{
	const char *fname = ZSTR_VAL(zf->common.function_name);
	const char *fsep;
	const char *fclass;

	if (zf->common.scope) {
		fsep =  "::";
		fclass = ZSTR_VAL(zf->common.scope->name);
	} else {
		fsep =  "";
		fclass = "";
	}

	zend_type_error("%s%s%s() must not return a value, %s%s returned",
		fclass, fsep, fname, returned_msg, returned_kind);
}

static int zend_verify_internal_return_type(zend_function *zf, zval *ret)
{
	zend_internal_arg_info *ret_info = zf->internal_function.arg_info - 1;
	zend_class_entry *ce = NULL;
	void *dummy_cache_slot = NULL;

	if (ZEND_TYPE_CODE(ret_info->type) == IS_VOID) {
		if (UNEXPECTED(Z_TYPE_P(ret) != IS_NULL)) {
			zend_verify_void_return_error(zf, zend_zval_type_name(ret), "");
			return 0;
		}
		return 1;
	}

	if (UNEXPECTED(!zend_check_type(ret_info->type, ret, &ce, &dummy_cache_slot, NULL, NULL, 1))) {
		zend_verify_internal_return_error(zf, ce, ret);
		return 0;
	}

	return 1;
}
#endif

static zend_always_inline void zend_verify_return_type(zend_function *zf, zval *ret, void **cache_slot)
{
	zend_arg_info *ret_info = zf->common.arg_info - 1;
	zend_class_entry *ce = NULL;

	if (UNEXPECTED(!zend_check_type(ret_info->type, ret, &ce, cache_slot, NULL, NULL, 1))) {
		zend_verify_return_error(zf, ce, ret);
	}
}

static ZEND_COLD int zend_verify_missing_return_type(const zend_function *zf, void **cache_slot)
{
	zend_arg_info *ret_info = zf->common.arg_info - 1;

	if (ZEND_TYPE_IS_SET(ret_info->type) && UNEXPECTED(ZEND_TYPE_CODE(ret_info->type) != IS_VOID)) {
		zend_class_entry *ce = NULL;
		if (ZEND_TYPE_IS_CLASS(ret_info->type)) {
			if (EXPECTED(*cache_slot)) {
				ce = (zend_class_entry*) *cache_slot;
			} else {
				ce = zend_fetch_class(ZEND_TYPE_NAME(ret_info->type), (ZEND_FETCH_CLASS_AUTO | ZEND_FETCH_CLASS_NO_AUTOLOAD));
				if (ce) {
					*cache_slot = (void*)ce;
				}
			}
		}
		zend_verify_return_error(zf, ce, NULL);
		return 0;
	}
	return 1;
}

static zend_never_inline ZEND_COLD void ZEND_FASTCALL zend_use_object_as_array(void)
{
	zend_throw_error(NULL, "Cannot use object as array");
}

static zend_never_inline ZEND_COLD void ZEND_FASTCALL zend_illegal_offset(void)
{
	zend_error(E_WARNING, "Illegal offset type");
}

static zend_never_inline void zend_assign_to_object_dim(zval *object, zval *dim, zval *value OPLINE_DC EXECUTE_DATA_DC)
{
	if (UNEXPECTED(!Z_OBJ_HT_P(object)->write_dimension)) {
		zend_use_object_as_array();
		if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
			ZVAL_UNDEF(EX_VAR(opline->result.var));
		}
		return;
	}

	Z_OBJ_HT_P(object)->write_dimension(object, dim, value);

	if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
		ZVAL_COPY(EX_VAR(opline->result.var), value);
	}
}

static zend_never_inline void zend_binary_assign_op_obj_dim(zval *object, zval *property, zval *value, zval *retval, binary_op_type binary_op EXECUTE_DATA_DC)
{
	zval *z;
	zval rv, res;

	if (Z_OBJ_HT_P(object)->read_dimension &&
		(z = Z_OBJ_HT_P(object)->read_dimension(object, property, BP_VAR_R, &rv)) != NULL) {

		if (Z_TYPE_P(z) == IS_OBJECT && Z_OBJ_HT_P(z)->get) {
			zval rv2;
			zval *value = Z_OBJ_HT_P(z)->get(z, &rv2);

			if (z == &rv) {
				zval_ptr_dtor(&rv);
			}
			ZVAL_COPY_VALUE(z, value);
		}
		binary_op(&res, Z_ISREF_P(z) ? Z_REFVAL_P(z) : z, value);
		Z_OBJ_HT_P(object)->write_dimension(object, property, &res);
		if (z == &rv) {
			zval_ptr_dtor(&rv);
		}
		if (retval) {
			ZVAL_COPY(retval, &res);
		}
		zval_ptr_dtor(&res);
	} else {
		zend_use_object_as_array();
		if (retval) {
			ZVAL_NULL(retval);
		}
	}
}

static zend_never_inline zend_long zend_check_string_offset(zval *dim, int type EXECUTE_DATA_DC)
{
	zend_long offset;

try_again:
	if (UNEXPECTED(Z_TYPE_P(dim) != IS_LONG)) {
		switch(Z_TYPE_P(dim)) {
			case IS_STRING:
				if (IS_LONG == is_numeric_string(Z_STRVAL_P(dim), Z_STRLEN_P(dim), NULL, NULL, -1)) {
					break;
				}
				if (type != BP_VAR_UNSET) {
					zend_error(E_WARNING, "Illegal string offset '%s'", Z_STRVAL_P(dim));
				}
				break;
			case IS_UNDEF:
				zval_undefined_cv(EX(opline)->op2.var EXECUTE_DATA_CC);
			case IS_DOUBLE:
			case IS_NULL:
			case IS_FALSE:
			case IS_TRUE:
				zend_error(E_NOTICE, "String offset cast occurred");
				break;
			case IS_REFERENCE:
				dim = Z_REFVAL_P(dim);
				goto try_again;
			default:
				zend_illegal_offset();
				break;
		}

		offset = zval_get_long_func(dim);
	} else {
		offset = Z_LVAL_P(dim);
	}

	return offset;
}

static zend_never_inline ZEND_COLD void zend_wrong_string_offset(EXECUTE_DATA_D)
{
	const char *msg = NULL;
	const zend_op *opline = EX(opline);
	const zend_op *end;
	uint32_t var;

	if (UNEXPECTED(EG(exception) != NULL)) {
		return;
	}

	switch (opline->opcode) {
		case ZEND_ASSIGN_ADD:
		case ZEND_ASSIGN_SUB:
		case ZEND_ASSIGN_MUL:
		case ZEND_ASSIGN_DIV:
		case ZEND_ASSIGN_MOD:
		case ZEND_ASSIGN_SL:
		case ZEND_ASSIGN_SR:
		case ZEND_ASSIGN_CONCAT:
		case ZEND_ASSIGN_BW_OR:
		case ZEND_ASSIGN_BW_AND:
		case ZEND_ASSIGN_BW_XOR:
		case ZEND_ASSIGN_POW:
			msg = "Cannot use assign-op operators with string offsets";
			break;
		case ZEND_FETCH_DIM_W:
		case ZEND_FETCH_DIM_RW:
		case ZEND_FETCH_DIM_FUNC_ARG:
		case ZEND_FETCH_DIM_UNSET:
		case ZEND_FETCH_LIST_W:
			/* TODO: Encode the "reason" into opline->extended_value??? */
			var = opline->result.var;
			opline++;
			end = EG(current_execute_data)->func->op_array.opcodes +
				EG(current_execute_data)->func->op_array.last;
			while (opline < end) {
				if (opline->op1_type == IS_VAR && opline->op1.var == var) {
					switch (opline->opcode) {
						case ZEND_ASSIGN_ADD:
						case ZEND_ASSIGN_SUB:
						case ZEND_ASSIGN_MUL:
						case ZEND_ASSIGN_DIV:
						case ZEND_ASSIGN_MOD:
						case ZEND_ASSIGN_SL:
						case ZEND_ASSIGN_SR:
						case ZEND_ASSIGN_CONCAT:
						case ZEND_ASSIGN_BW_OR:
						case ZEND_ASSIGN_BW_AND:
						case ZEND_ASSIGN_BW_XOR:
						case ZEND_ASSIGN_POW:
							if (opline->extended_value == ZEND_ASSIGN_OBJ) {
								msg = "Cannot use string offset as an object";
							} else if (opline->extended_value == ZEND_ASSIGN_DIM) {
								msg = "Cannot use string offset as an array";
							} else {
								msg = "Cannot use assign-op operators with string offsets";
							}
							break;
						case ZEND_PRE_INC_OBJ:
						case ZEND_PRE_DEC_OBJ:
						case ZEND_POST_INC_OBJ:
						case ZEND_POST_DEC_OBJ:
						case ZEND_PRE_INC:
						case ZEND_PRE_DEC:
						case ZEND_POST_INC:
						case ZEND_POST_DEC:
							msg = "Cannot increment/decrement string offsets";
							break;
						case ZEND_FETCH_DIM_W:
						case ZEND_FETCH_DIM_RW:
						case ZEND_FETCH_DIM_FUNC_ARG:
						case ZEND_FETCH_DIM_UNSET:
						case ZEND_FETCH_LIST_W:
						case ZEND_ASSIGN_DIM:
							msg = "Cannot use string offset as an array";
							break;
						case ZEND_FETCH_OBJ_W:
						case ZEND_FETCH_OBJ_RW:
						case ZEND_FETCH_OBJ_FUNC_ARG:
						case ZEND_FETCH_OBJ_UNSET:
						case ZEND_ASSIGN_OBJ:
							msg = "Cannot use string offset as an object";
							break;
						case ZEND_ASSIGN_REF:
						case ZEND_ADD_ARRAY_ELEMENT:
						case ZEND_INIT_ARRAY:
						case ZEND_MAKE_REF:
							msg = "Cannot create references to/from string offsets";
							break;
						case ZEND_RETURN_BY_REF:
						case ZEND_VERIFY_RETURN_TYPE:
							msg = "Cannot return string offsets by reference";
							break;
						case ZEND_UNSET_DIM:
						case ZEND_UNSET_OBJ:
							msg = "Cannot unset string offsets";
							break;
						case ZEND_YIELD:
							msg = "Cannot yield string offsets by reference";
							break;
						case ZEND_SEND_REF:
						case ZEND_SEND_VAR_EX:
						case ZEND_SEND_FUNC_ARG:
							msg = "Only variables can be passed by reference";
							break;
						case ZEND_FE_RESET_RW:
							msg = "Cannot iterate on string offsets by reference";
							break;
						EMPTY_SWITCH_DEFAULT_CASE();
					}
					break;
				}
				if (opline->op2_type == IS_VAR && opline->op2.var == var) {
					ZEND_ASSERT(opline->opcode == ZEND_ASSIGN_REF);
					msg = "Cannot create references to/from string offsets";
					break;
				}
			}
			break;
		EMPTY_SWITCH_DEFAULT_CASE();
	}
	ZEND_ASSERT(msg != NULL);
	zend_throw_error(NULL, "%s", msg);
}

static zend_never_inline ZEND_COLD void ZEND_FASTCALL zend_wrong_property_read(zval *property)
{
	zend_string *tmp_property_name;
	zend_string *property_name = zval_get_tmp_string(property, &tmp_property_name);
	zend_error(E_NOTICE, "Trying to get property '%s' of non-object", ZSTR_VAL(property_name));
	zend_tmp_string_release(tmp_property_name);
}

static zend_never_inline ZEND_COLD void ZEND_FASTCALL zend_wrong_property_unset(zval *property)
{
	zend_string *tmp_property_name;
	zend_string *property_name = zval_get_tmp_string(property, &tmp_property_name);
	zend_error(E_NOTICE, "Trying to unset property '%s' of non-object", ZSTR_VAL(property_name));
	zend_tmp_string_release(tmp_property_name);
}

static zend_never_inline ZEND_COLD void ZEND_FASTCALL zend_wrong_property_check(zval *property)
{
	zend_string *tmp_property_name;
	zend_string *property_name = zval_get_tmp_string(property, &tmp_property_name);
	zend_error(E_NOTICE, "Trying to check property '%s' of non-object", ZSTR_VAL(property_name));
	zend_tmp_string_release(tmp_property_name);
}

static zend_never_inline ZEND_COLD void ZEND_FASTCALL zend_deprecated_function(const zend_function *fbc)
{
	zend_error(E_DEPRECATED, "Function %s%s%s() is deprecated",
		fbc->common.scope ? ZSTR_VAL(fbc->common.scope->name) : "",
		fbc->common.scope ? "::" : "",
		ZSTR_VAL(fbc->common.function_name));
}

static zend_never_inline void zend_assign_to_string_offset(zval *str, zval *dim, zval *value OPLINE_DC EXECUTE_DATA_DC)
{
	zend_uchar c;
	size_t string_len;
	zend_long offset;

	offset = zend_check_string_offset(dim, BP_VAR_W EXECUTE_DATA_CC);
	if (offset < -(zend_long)Z_STRLEN_P(str)) {
		/* Error on negative offset */
		zend_error(E_WARNING, "Illegal string offset:  " ZEND_LONG_FMT, offset);
		if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
			ZVAL_NULL(EX_VAR(opline->result.var));
		}
		return;
	}

	if (Z_TYPE_P(value) != IS_STRING) {
		/* Convert to string, just the time to pick the 1st byte */
		zend_string *tmp = zval_get_string_func(value);

		string_len = ZSTR_LEN(tmp);
		c = (zend_uchar)ZSTR_VAL(tmp)[0];
		zend_string_release_ex(tmp, 0);
	} else {
		string_len = Z_STRLEN_P(value);
		c = (zend_uchar)Z_STRVAL_P(value)[0];
	}

	if (string_len == 0) {
		/* Error on empty input string */
		zend_error(E_WARNING, "Cannot assign an empty string to a string offset");
		if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
			ZVAL_NULL(EX_VAR(opline->result.var));
		}
		return;
	}

	if (offset < 0) { /* Handle negative offset */
		offset += (zend_long)Z_STRLEN_P(str);
	}

	if ((size_t)offset >= Z_STRLEN_P(str)) {
		/* Extend string if needed */
		zend_long old_len = Z_STRLEN_P(str);
		ZVAL_NEW_STR(str, zend_string_extend(Z_STR_P(str), offset + 1, 0));
		memset(Z_STRVAL_P(str) + old_len, ' ', offset - old_len);
		Z_STRVAL_P(str)[offset+1] = 0;
	} else if (!Z_REFCOUNTED_P(str)) {
		ZVAL_NEW_STR(str, zend_string_init(Z_STRVAL_P(str), Z_STRLEN_P(str), 0));
	} else if (Z_REFCOUNT_P(str) > 1) {
		Z_DELREF_P(str);
		ZVAL_NEW_STR(str, zend_string_init(Z_STRVAL_P(str), Z_STRLEN_P(str), 0));
	} else {
		zend_string_forget_hash_val(Z_STR_P(str));
	}

	Z_STRVAL_P(str)[offset] = c;

	if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
		/* Return the new character */
		ZVAL_INTERNED_STR(EX_VAR(opline->result.var), ZSTR_CHAR(c));
	}
}

static zend_never_inline void zend_post_incdec_overloaded_property(zval *object, zval *property, void **cache_slot, int inc OPLINE_DC EXECUTE_DATA_DC)
{
	if (Z_OBJ_HT_P(object)->read_property && Z_OBJ_HT_P(object)->write_property) {
		zval rv, obj;
		zval *z;
		zval z_copy;

		ZVAL_OBJ(&obj, Z_OBJ_P(object));
		Z_ADDREF(obj);
		z = Z_OBJ_HT(obj)->read_property(&obj, property, BP_VAR_R, cache_slot, &rv);
		if (UNEXPECTED(EG(exception))) {
			OBJ_RELEASE(Z_OBJ(obj));
			ZVAL_UNDEF(EX_VAR(opline->result.var));
			return;
		}

		if (UNEXPECTED(Z_TYPE_P(z) == IS_OBJECT) && Z_OBJ_HT_P(z)->get) {
			zval rv2;
			zval *value = Z_OBJ_HT_P(z)->get(z, &rv2);
			if (z == &rv) {
				zval_ptr_dtor(&rv);
			}
			ZVAL_COPY_VALUE(z, value);
		}

		ZVAL_COPY_DEREF(&z_copy, z);
		ZVAL_COPY(EX_VAR(opline->result.var), &z_copy);
		if (inc) {
			increment_function(&z_copy);
		} else {
			decrement_function(&z_copy);
		}
		Z_OBJ_HT(obj)->write_property(&obj, property, &z_copy, cache_slot);
		OBJ_RELEASE(Z_OBJ(obj));
		zval_ptr_dtor(&z_copy);
		zval_ptr_dtor(z);
	} else {
		zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");
		ZVAL_NULL(EX_VAR(opline->result.var));
	}
}

static zend_never_inline void zend_pre_incdec_overloaded_property(zval *object, zval *property, void **cache_slot, int inc OPLINE_DC EXECUTE_DATA_DC)
{
	zval rv;

	if (Z_OBJ_HT_P(object)->read_property && Z_OBJ_HT_P(object)->write_property) {
		zval *z, obj;
		zval z_copy;

		ZVAL_OBJ(&obj, Z_OBJ_P(object));
		Z_ADDREF(obj);
		z = Z_OBJ_HT(obj)->read_property(&obj, property, BP_VAR_R, cache_slot, &rv);
		if (UNEXPECTED(EG(exception))) {
			OBJ_RELEASE(Z_OBJ(obj));
			if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
				ZVAL_NULL(EX_VAR(opline->result.var));
			}
			return;
		}

		if (UNEXPECTED(Z_TYPE_P(z) == IS_OBJECT) && Z_OBJ_HT_P(z)->get) {
			zval rv2;
			zval *value = Z_OBJ_HT_P(z)->get(z, &rv2);

			if (z == &rv) {
				zval_ptr_dtor(&rv);
			}
			ZVAL_COPY_VALUE(z, value);
		}
		ZVAL_COPY_DEREF(&z_copy, z);
		if (inc) {
			increment_function(&z_copy);
		} else {
			decrement_function(&z_copy);
		}
		if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
			ZVAL_COPY(EX_VAR(opline->result.var), &z_copy);
		}
		Z_OBJ_HT(obj)->write_property(&obj, property, &z_copy, cache_slot);
		OBJ_RELEASE(Z_OBJ(obj));
		zval_ptr_dtor(&z_copy);
		zval_ptr_dtor(z);
	} else {
		zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");
		if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
			ZVAL_NULL(EX_VAR(opline->result.var));
		}
	}
}

static zend_never_inline void zend_assign_op_overloaded_property(zval *object, zval *property, void **cache_slot, zval *value, binary_op_type binary_op OPLINE_DC EXECUTE_DATA_DC)
{
	zval *z;
	zval rv, obj, res;

	ZVAL_OBJ(&obj, Z_OBJ_P(object));
	Z_ADDREF(obj);
	if (EXPECTED(Z_OBJ_HT(obj)->read_property)) {
		z = Z_OBJ_HT(obj)->read_property(&obj, property, BP_VAR_R, cache_slot, &rv);
		if (UNEXPECTED(EG(exception))) {
			OBJ_RELEASE(Z_OBJ(obj));
			if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
				ZVAL_UNDEF(EX_VAR(opline->result.var));
			}
			return;
		}
		if (Z_TYPE_P(z) == IS_OBJECT && Z_OBJ_HT_P(z)->get) {
			zval rv2;
			zval *value = Z_OBJ_HT_P(z)->get(z, &rv2);

			if (z == &rv) {
				zval_ptr_dtor(&rv);
			}
			ZVAL_COPY_VALUE(z, value);
		}
		binary_op(&res, z, value);
		Z_OBJ_HT(obj)->write_property(&obj, property, &res, cache_slot);
		if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
			ZVAL_COPY(EX_VAR(opline->result.var), &res);
		}
		zval_ptr_dtor(z);
		zval_ptr_dtor(&res);
	} else {
		zend_error(E_WARNING, "Attempt to assign property of non-object");
		if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
			ZVAL_NULL(EX_VAR(opline->result.var));
		}
	}
	OBJ_RELEASE(Z_OBJ(obj));
}

/* Utility Functions for Extensions */
static void zend_extension_statement_handler(const zend_extension *extension, zend_execute_data *frame)
{
	if (extension->statement_handler) {
		extension->statement_handler(frame);
	}
}


static void zend_extension_fcall_begin_handler(const zend_extension *extension, zend_execute_data *frame)
{
	if (extension->fcall_begin_handler) {
		extension->fcall_begin_handler(frame);
	}
}


static void zend_extension_fcall_end_handler(const zend_extension *extension, zend_execute_data *frame)
{
	if (extension->fcall_end_handler) {
		extension->fcall_end_handler(frame);
	}
}


static zend_always_inline HashTable *zend_get_target_symbol_table(int fetch_type EXECUTE_DATA_DC)
{
	HashTable *ht;

	if (EXPECTED(fetch_type & (ZEND_FETCH_GLOBAL_LOCK | ZEND_FETCH_GLOBAL))) {
		ht = &EG(symbol_table);
	} else {
		ZEND_ASSERT(fetch_type & ZEND_FETCH_LOCAL);
		if (!(EX_CALL_INFO() & ZEND_CALL_HAS_SYMBOL_TABLE)) {
			zend_rebuild_symbol_table();
		}
		ht = EX(symbol_table);
	}
	return ht;
}

static zend_never_inline ZEND_COLD void ZEND_FASTCALL zend_undefined_offset(zend_long lval)
{
	zend_error(E_NOTICE, "Undefined offset: " ZEND_LONG_FMT, lval);
}

static zend_never_inline ZEND_COLD void ZEND_FASTCALL zend_undefined_index(const zend_string *offset)
{
	zend_error(E_NOTICE, "Undefined index: %s", ZSTR_VAL(offset));
}

static zend_never_inline ZEND_COLD void ZEND_FASTCALL zend_undefined_method(const zend_class_entry *ce, const zend_string *method)
{
	zend_throw_error(NULL, "Call to undefined method %s::%s()", ZSTR_VAL(ce->name), ZSTR_VAL(method));
}

static zend_never_inline ZEND_COLD void ZEND_FASTCALL zend_invalid_method_call(zval *object, zval *function_name)
{
	zend_throw_error(NULL, "Call to a member function %s() on %s", Z_STRVAL_P(function_name), zend_get_type_by_const(Z_TYPE_P(object)));
}

static zend_never_inline ZEND_COLD void ZEND_FASTCALL zend_non_static_method_call(const zend_function *fbc)
{
	if (fbc->common.fn_flags & ZEND_ACC_ALLOW_STATIC) {
		zend_error(E_DEPRECATED,
			"Non-static method %s::%s() should not be called statically",
			ZSTR_VAL(fbc->common.scope->name), ZSTR_VAL(fbc->common.function_name));
	} else {
		zend_throw_error(
			zend_ce_error,
			"Non-static method %s::%s() cannot be called statically",
			ZSTR_VAL(fbc->common.scope->name), ZSTR_VAL(fbc->common.function_name));
	}
}

static zend_never_inline ZEND_COLD void ZEND_FASTCALL zend_param_must_be_ref(const zend_function *func, uint32_t arg_num)
{
	zend_error(E_WARNING, "Parameter %d to %s%s%s() expected to be a reference, value given",
		arg_num,
		func->common.scope ? ZSTR_VAL(func->common.scope->name) : "",
		func->common.scope ? "::" : "",
		ZSTR_VAL(func->common.function_name));
}

static zend_never_inline ZEND_COLD void ZEND_FASTCALL zend_use_scalar_as_array(void)
{
	zend_error(E_WARNING, "Cannot use a scalar value as an array");
}

static zend_never_inline ZEND_COLD void ZEND_FASTCALL zend_cannot_add_element(void)
{
	zend_error(E_WARNING, "Cannot add element to the array as the next element is already occupied");
}

static zend_never_inline ZEND_COLD void ZEND_FASTCALL zend_use_resource_as_offset(const zval *dim)
{
	zend_error(E_NOTICE, "Resource ID#%d used as offset, casting to integer (%d)", Z_RES_HANDLE_P(dim), Z_RES_HANDLE_P(dim));
}

static zend_never_inline ZEND_COLD void ZEND_FASTCALL zend_use_new_element_for_string(void)
{
	zend_throw_error(NULL, "[] operator not supported for strings");
}

static zend_never_inline ZEND_COLD void ZEND_FASTCALL zend_access_undefined_propery_in_overloaded_object(void)
{
	zend_throw_error(NULL, "Cannot access undefined property for object with overloaded property access");
}

static zend_never_inline ZEND_COLD void ZEND_FASTCALL zend_unsupported_property_reference(void)
{
	zend_error(E_WARNING, "This object doesn't support property references");
}

static zend_always_inline zval *zend_fetch_dimension_address_inner(HashTable *ht, const zval *dim, int dim_type, int type EXECUTE_DATA_DC)
{
	zval *retval = NULL;
	zend_string *offset_key;
	zend_ulong hval;

try_again:
	if (EXPECTED(Z_TYPE_P(dim) == IS_LONG)) {
		hval = Z_LVAL_P(dim);
num_index:
		ZEND_HASH_INDEX_FIND(ht, hval, retval, num_undef);
		return retval;
num_undef:
		switch (type) {
			case BP_VAR_R:
				zend_undefined_offset(hval);
				/* break missing intentionally */
			case BP_VAR_UNSET:
			case BP_VAR_IS:
				retval = &EG(uninitialized_zval);
				break;
			case BP_VAR_RW:
				zend_undefined_offset(hval);
				retval = zend_hash_index_update(ht, hval, &EG(uninitialized_zval));
				break;
			case BP_VAR_W:
				retval = zend_hash_index_add_new(ht, hval, &EG(uninitialized_zval));
				break;
		}
	} else if (EXPECTED(Z_TYPE_P(dim) == IS_STRING)) {
		offset_key = Z_STR_P(dim);
		if (dim_type != IS_CONST) {
			if (ZEND_HANDLE_NUMERIC(offset_key, hval)) {
				goto num_index;
			}
		}
str_index:
		retval = zend_hash_find_ex(ht, offset_key, dim_type == IS_CONST);
		if (retval) {
			/* support for $GLOBALS[...] */
			if (UNEXPECTED(Z_TYPE_P(retval) == IS_INDIRECT)) {
				retval = Z_INDIRECT_P(retval);
				if (UNEXPECTED(Z_TYPE_P(retval) == IS_UNDEF)) {
					switch (type) {
						case BP_VAR_R:
							zend_undefined_index(offset_key);
							/* break missing intentionally */
						case BP_VAR_UNSET:
						case BP_VAR_IS:
							retval = &EG(uninitialized_zval);
							break;
						case BP_VAR_RW:
							zend_undefined_index(offset_key);
							/* break missing intentionally */
						case BP_VAR_W:
							ZVAL_NULL(retval);
							break;
					}
				}
			}
		} else {
			switch (type) {
				case BP_VAR_R:
					zend_undefined_index(offset_key);
					/* break missing intentionally */
				case BP_VAR_UNSET:
				case BP_VAR_IS:
					retval = &EG(uninitialized_zval);
					break;
				case BP_VAR_RW:
					zend_undefined_index(offset_key);
					retval = zend_hash_update(ht, offset_key, &EG(uninitialized_zval));
					break;
				case BP_VAR_W:
					retval = zend_hash_add_new(ht, offset_key, &EG(uninitialized_zval));
					break;
			}
		}
	} else {
		switch (Z_TYPE_P(dim)) {
			case IS_UNDEF:
				zval_undefined_cv(EX(opline)->op2.var EXECUTE_DATA_CC);
				/* break missing intentionally */
			case IS_NULL:
				offset_key = ZSTR_EMPTY_ALLOC();
				goto str_index;
			case IS_DOUBLE:
				hval = zend_dval_to_lval(Z_DVAL_P(dim));
				goto num_index;
			case IS_RESOURCE:
				zend_use_resource_as_offset(dim);
				hval = Z_RES_HANDLE_P(dim);
				goto num_index;
			case IS_FALSE:
				hval = 0;
				goto num_index;
			case IS_TRUE:
				hval = 1;
				goto num_index;
			case IS_REFERENCE:
				dim = Z_REFVAL_P(dim);
				goto try_again;
			default:
				zend_illegal_offset();
				retval = (type == BP_VAR_W || type == BP_VAR_RW) ?
					NULL : &EG(uninitialized_zval);
		}
	}
	return retval;
}

static zend_never_inline zval* ZEND_FASTCALL zend_fetch_dimension_address_inner_W(HashTable *ht, const zval *dim EXECUTE_DATA_DC)
{
	return zend_fetch_dimension_address_inner(ht, dim, IS_TMP_VAR, BP_VAR_W EXECUTE_DATA_CC);
}

static zend_never_inline zval* ZEND_FASTCALL zend_fetch_dimension_address_inner_W_CONST(HashTable *ht, const zval *dim EXECUTE_DATA_DC)
{
	return zend_fetch_dimension_address_inner(ht, dim, IS_CONST, BP_VAR_W EXECUTE_DATA_CC);
}

static zend_never_inline zval* ZEND_FASTCALL zend_fetch_dimension_address_inner_RW(HashTable *ht, const zval *dim EXECUTE_DATA_DC)
{
	return zend_fetch_dimension_address_inner(ht, dim, IS_TMP_VAR, BP_VAR_RW EXECUTE_DATA_CC);
}

static zend_never_inline zval* ZEND_FASTCALL zend_fetch_dimension_address_inner_RW_CONST(HashTable *ht, const zval *dim EXECUTE_DATA_DC)
{
	return zend_fetch_dimension_address_inner(ht, dim, IS_CONST, BP_VAR_RW EXECUTE_DATA_CC);
}

static zend_always_inline void zend_fetch_dimension_address(zval *result, zval *container, zval *dim, int dim_type, int type EXECUTE_DATA_DC)
{
    zval *retval;

	if (EXPECTED(Z_TYPE_P(container) == IS_ARRAY)) {
try_array:
		SEPARATE_ARRAY(container);
fetch_from_array:
		if (dim == NULL) {
			retval = zend_hash_next_index_insert(Z_ARRVAL_P(container), &EG(uninitialized_zval));
			if (UNEXPECTED(retval == NULL)) {
				zend_cannot_add_element();
				ZVAL_ERROR(result);
				return;
			}
		} else {
			retval = zend_fetch_dimension_address_inner(Z_ARRVAL_P(container), dim, dim_type, type EXECUTE_DATA_CC);
			if (UNEXPECTED(!retval)) {
				ZVAL_ERROR(result);
				return;
			}
		}
		ZVAL_INDIRECT(result, retval);
		return;
	} else if (EXPECTED(Z_TYPE_P(container) == IS_REFERENCE)) {
		container = Z_REFVAL_P(container);
		if (EXPECTED(Z_TYPE_P(container) == IS_ARRAY)) {
			goto try_array;
		}
	}
	if (UNEXPECTED(Z_TYPE_P(container) == IS_STRING)) {
		if (dim == NULL) {
			zend_use_new_element_for_string();
		} else {
			zend_check_string_offset(dim, type EXECUTE_DATA_CC);
			zend_wrong_string_offset(EXECUTE_DATA_C);
		}
		ZVAL_ERROR(result);
	} else if (EXPECTED(Z_TYPE_P(container) == IS_OBJECT)) {
		if (/*dim_type == IS_CV &&*/ dim && UNEXPECTED(Z_TYPE_P(dim) == IS_UNDEF)) {
			zval_undefined_cv(EX(opline)->op2.var EXECUTE_DATA_CC);
			dim = &EG(uninitialized_zval);
		}
		if (!Z_OBJ_HT_P(container)->read_dimension) {
			zend_use_object_as_array();
			ZVAL_ERROR(result);
		} else {
			if (dim_type == IS_CONST && Z_EXTRA_P(dim) == ZEND_EXTRA_VALUE) {
				dim++;
			}
			retval = Z_OBJ_HT_P(container)->read_dimension(container, dim, type, result);

			if (UNEXPECTED(retval == &EG(uninitialized_zval))) {
				zend_class_entry *ce = Z_OBJCE_P(container);

				ZVAL_NULL(result);
				zend_error(E_NOTICE, "Indirect modification of overloaded element of %s has no effect", ZSTR_VAL(ce->name));
			} else if (EXPECTED(retval && Z_TYPE_P(retval) != IS_UNDEF)) {
				if (!Z_ISREF_P(retval)) {
					if (result != retval) {
						ZVAL_COPY(result, retval);
						retval = result;
					}
					if (Z_TYPE_P(retval) != IS_OBJECT) {
						zend_class_entry *ce = Z_OBJCE_P(container);
						zend_error(E_NOTICE, "Indirect modification of overloaded element of %s has no effect", ZSTR_VAL(ce->name));
					}
				} else if (UNEXPECTED(Z_REFCOUNT_P(retval) == 1)) {
					ZVAL_UNREF(retval);
				}
				if (result != retval) {
					ZVAL_INDIRECT(result, retval);
				}
			} else {
				ZVAL_ERROR(result);
			}
		}
	} else {
		if (type != BP_VAR_W && UNEXPECTED(Z_TYPE_P(container) == IS_UNDEF)) {
			zval_undefined_cv(EX(opline)->op1.var EXECUTE_DATA_CC);
		}
		if (/*dim_type == IS_CV &&*/ dim && UNEXPECTED(Z_TYPE_P(dim) == IS_UNDEF)) {
			zval_undefined_cv(EX(opline)->op2.var EXECUTE_DATA_CC);
		}
		if (EXPECTED(Z_TYPE_P(container) <= IS_FALSE)) {
			if (type != BP_VAR_UNSET) {
				array_init(container);
				goto fetch_from_array;
			} else {
				/* for read-mode only */
				ZVAL_NULL(result);
			}
		} else if (EXPECTED(Z_ISERROR_P(container))) {
			ZVAL_ERROR(result);
		} else {
			if (type == BP_VAR_UNSET) {
				zend_error(E_WARNING, "Cannot unset offset in a non-array variable");
				ZVAL_NULL(result);
			} else {
				zend_use_scalar_as_array();
				ZVAL_ERROR(result);
			}
		}
	}
}

static zend_never_inline void ZEND_FASTCALL zend_fetch_dimension_address_W(zval *container_ptr, zval *dim, int dim_type OPLINE_DC EXECUTE_DATA_DC)
{
	zval *result = EX_VAR(opline->result.var);
	zend_fetch_dimension_address(result, container_ptr, dim, dim_type, BP_VAR_W EXECUTE_DATA_CC);
}

static zend_never_inline void ZEND_FASTCALL zend_fetch_dimension_address_RW(zval *container_ptr, zval *dim, int dim_type OPLINE_DC EXECUTE_DATA_DC)
{
	zval *result = EX_VAR(opline->result.var);
	zend_fetch_dimension_address(result, container_ptr, dim, dim_type, BP_VAR_RW EXECUTE_DATA_CC);
}

static zend_never_inline void ZEND_FASTCALL zend_fetch_dimension_address_UNSET(zval *container_ptr, zval *dim, int dim_type OPLINE_DC EXECUTE_DATA_DC)
{
	zval *result = EX_VAR(opline->result.var);
	zend_fetch_dimension_address(result, container_ptr, dim, dim_type, BP_VAR_UNSET EXECUTE_DATA_CC);
}

static zend_always_inline void zend_fetch_dimension_address_read(zval *result, zval *container, zval *dim, int dim_type, int type, int support_strings, int slow EXECUTE_DATA_DC)
{
	zval *retval;

	if (!slow) {
		if (EXPECTED(Z_TYPE_P(container) == IS_ARRAY)) {
try_array:
			retval = zend_fetch_dimension_address_inner(Z_ARRVAL_P(container), dim, dim_type, type EXECUTE_DATA_CC);
			ZVAL_COPY_DEREF(result, retval);
			return;
		} else if (EXPECTED(Z_TYPE_P(container) == IS_REFERENCE)) {
			container = Z_REFVAL_P(container);
			if (EXPECTED(Z_TYPE_P(container) == IS_ARRAY)) {
				goto try_array;
			}
		}
	}
	if (support_strings && EXPECTED(Z_TYPE_P(container) == IS_STRING)) {
		zend_long offset;

try_string_offset:
		if (UNEXPECTED(Z_TYPE_P(dim) != IS_LONG)) {
			switch (Z_TYPE_P(dim)) {
				/* case IS_LONG: */
				case IS_STRING:
					if (IS_LONG == is_numeric_string(Z_STRVAL_P(dim), Z_STRLEN_P(dim), NULL, NULL, -1)) {
						break;
					}
					if (type == BP_VAR_IS) {
						ZVAL_NULL(result);
						return;
					}
					zend_error(E_WARNING, "Illegal string offset '%s'", Z_STRVAL_P(dim));
					break;
				case IS_UNDEF:
					zval_undefined_cv(EX(opline)->op2.var EXECUTE_DATA_CC);
				case IS_DOUBLE:
				case IS_NULL:
				case IS_FALSE:
				case IS_TRUE:
					if (type != BP_VAR_IS) {
						zend_error(E_NOTICE, "String offset cast occurred");
					}
					break;
				case IS_REFERENCE:
					dim = Z_REFVAL_P(dim);
					goto try_string_offset;
				default:
					zend_illegal_offset();
					break;
			}

			offset = zval_get_long_func(dim);
		} else {
			offset = Z_LVAL_P(dim);
		}

		if (UNEXPECTED(Z_STRLEN_P(container) < (size_t)((offset < 0) ? -offset : (offset + 1)))) {
			if (type != BP_VAR_IS) {
				zend_error(E_NOTICE, "Uninitialized string offset: " ZEND_LONG_FMT, offset);
				ZVAL_EMPTY_STRING(result);
			} else {
				ZVAL_NULL(result);
			}
		} else {
			zend_uchar c;
			zend_long real_offset;

			real_offset = (UNEXPECTED(offset < 0)) /* Handle negative offset */
				? (zend_long)Z_STRLEN_P(container) + offset : offset;
			c = (zend_uchar)Z_STRVAL_P(container)[real_offset];

			ZVAL_INTERNED_STR(result, ZSTR_CHAR(c));
		}
	} else if (EXPECTED(Z_TYPE_P(container) == IS_OBJECT)) {
		if (/*dim_type == IS_CV &&*/ UNEXPECTED(Z_TYPE_P(dim) == IS_UNDEF)) {
			zval_undefined_cv(EX(opline)->op2.var EXECUTE_DATA_CC);
			dim = &EG(uninitialized_zval);
		}
		if (!Z_OBJ_HT_P(container)->read_dimension) {
			zend_use_object_as_array();
			ZVAL_NULL(result);
		} else {
			if (dim_type == IS_CONST && Z_EXTRA_P(dim) == ZEND_EXTRA_VALUE) {
				dim++;
			}
			retval = Z_OBJ_HT_P(container)->read_dimension(container, dim, type, result);

			ZEND_ASSERT(result != NULL);
			if (retval) {
				if (result != retval) {
					ZVAL_COPY_DEREF(result, retval);
				} else if (UNEXPECTED(Z_ISREF_P(retval))) {
					zend_unwrap_reference(result);
				}
			} else {
				ZVAL_NULL(result);
			}
		}
	} else {
		if (type != BP_VAR_IS && UNEXPECTED(Z_TYPE_P(container) == IS_UNDEF)) {
			zval_undefined_cv(EX(opline)->op1.var EXECUTE_DATA_CC);
		}
		if (/*dim_type == IS_CV &&*/ UNEXPECTED(Z_TYPE_P(dim) == IS_UNDEF)) {
			zval_undefined_cv(EX(opline)->op2.var EXECUTE_DATA_CC);
		}
		ZVAL_NULL(result);
	}
}

static zend_never_inline void ZEND_FASTCALL zend_fetch_dimension_address_read_R(zval *container, zval *dim, int dim_type OPLINE_DC EXECUTE_DATA_DC)
{
	zval *result = EX_VAR(opline->result.var);
	zend_fetch_dimension_address_read(result, container, dim, dim_type, BP_VAR_R, 1, 0 EXECUTE_DATA_CC);
}

static zend_never_inline void zend_fetch_dimension_address_read_R_slow(zval *container, zval *dim OPLINE_DC EXECUTE_DATA_DC)
{
	zval *result = EX_VAR(opline->result.var);
	zend_fetch_dimension_address_read(result, container, dim, IS_CV, BP_VAR_R, 1, 1 EXECUTE_DATA_CC);
}

static zend_never_inline void ZEND_FASTCALL zend_fetch_dimension_address_read_IS(zval *container, zval *dim, int dim_type OPLINE_DC EXECUTE_DATA_DC)
{
	zval *result = EX_VAR(opline->result.var);
	zend_fetch_dimension_address_read(result, container, dim, dim_type, BP_VAR_IS, 1, 0 EXECUTE_DATA_CC);
}

static zend_never_inline void ZEND_FASTCALL zend_fetch_dimension_address_LIST_r(zval *container, zval *dim, int dim_type OPLINE_DC EXECUTE_DATA_DC)
{
	zval *result = EX_VAR(opline->result.var);
	zend_fetch_dimension_address_read(result, container, dim, dim_type, BP_VAR_R, 0, 0 EXECUTE_DATA_CC);
}

ZEND_API void zend_fetch_dimension_const(zval *result, zval *container, zval *dim, int type)
{
	zend_fetch_dimension_address_read(result, container, dim, IS_TMP_VAR, type, 1, 0 NO_EXECUTE_DATA_CC);
}

static zend_never_inline zval* ZEND_FASTCALL zend_find_array_dim_slow(HashTable *ht, zval *offset EXECUTE_DATA_DC)
{
	zend_ulong hval;

	if (Z_TYPE_P(offset) == IS_DOUBLE) {
		hval = zend_dval_to_lval(Z_DVAL_P(offset));
num_idx:
		return zend_hash_index_find(ht, hval);
	} else if (Z_TYPE_P(offset) == IS_NULL) {
str_idx:
		return zend_hash_find_ex_ind(ht, ZSTR_EMPTY_ALLOC(), 1);
	} else if (Z_TYPE_P(offset) == IS_FALSE) {
		hval = 0;
		goto num_idx;
	} else if (Z_TYPE_P(offset) == IS_TRUE) {
		hval = 1;
		goto num_idx;
	} else if (Z_TYPE_P(offset) == IS_RESOURCE) {
		hval = Z_RES_HANDLE_P(offset);
		goto num_idx;
	} else if (/*OP2_TYPE == IS_CV &&*/ Z_TYPE_P(offset) == IS_UNDEF) {
		zval_undefined_cv(EX(opline)->op2.var EXECUTE_DATA_CC);
		goto str_idx;
	} else {
		zend_error(E_WARNING, "Illegal offset type in isset or empty");
		return NULL;
	}
}

static zend_never_inline int ZEND_FASTCALL zend_isset_dim_slow(zval *container, zval *offset EXECUTE_DATA_DC)
{
	if (/*OP2_TYPE == IS_CV &&*/ UNEXPECTED(Z_TYPE_P(offset) == IS_UNDEF)) {
		zval_undefined_cv(EX(opline)->op2.var EXECUTE_DATA_CC);
		offset = &EG(uninitialized_zval);
	}

	if (/*OP1_TYPE != IS_CONST &&*/ EXPECTED(Z_TYPE_P(container) == IS_OBJECT)) {
		if (EXPECTED(Z_OBJ_HT_P(container)->has_dimension)) {
			return Z_OBJ_HT_P(container)->has_dimension(container, offset, 0);
		} else {
			zend_use_object_as_array();
			return 0;
		}
	} else if (EXPECTED(Z_TYPE_P(container) == IS_STRING)) { /* string offsets */
		zend_long lval;

		if (EXPECTED(Z_TYPE_P(offset) == IS_LONG)) {
			lval = Z_LVAL_P(offset);
str_offset:
			if (UNEXPECTED(lval < 0)) { /* Handle negative offset */
				lval += (zend_long)Z_STRLEN_P(container);
			}
			if (EXPECTED(lval >= 0) && (size_t)lval < Z_STRLEN_P(container)) {
				return 1;
			} else {
				return 0;
			}
		} else {
			/*if (OP2_TYPE & (IS_CV|IS_VAR)) {*/
				ZVAL_DEREF(offset);
			/*}*/
			if (Z_TYPE_P(offset) < IS_STRING /* simple scalar types */
					|| (Z_TYPE_P(offset) == IS_STRING /* or numeric string */
						&& IS_LONG == is_numeric_string(Z_STRVAL_P(offset), Z_STRLEN_P(offset), NULL, NULL, 0))) {
				lval = zval_get_long(offset);
				goto str_offset;
			}
			return 0;
		}
	} else {
		return 0;
	}
}

static zend_never_inline int ZEND_FASTCALL zend_isempty_dim_slow(zval *container, zval *offset EXECUTE_DATA_DC)
{
	if (/*OP2_TYPE == IS_CV &&*/ UNEXPECTED(Z_TYPE_P(offset) == IS_UNDEF)) {
		zval_undefined_cv(EX(opline)->op2.var EXECUTE_DATA_CC);
		offset = &EG(uninitialized_zval);
	}

	if (/*OP1_TYPE != IS_CONST &&*/ EXPECTED(Z_TYPE_P(container) == IS_OBJECT)) {
		if (EXPECTED(Z_OBJ_HT_P(container)->has_dimension)) {
			return !Z_OBJ_HT_P(container)->has_dimension(container, offset, 1);
		} else {
			zend_use_object_as_array();
			return 1;
		}
	} else if (EXPECTED(Z_TYPE_P(container) == IS_STRING)) { /* string offsets */
		zend_long lval;

		if (EXPECTED(Z_TYPE_P(offset) == IS_LONG)) {
			lval = Z_LVAL_P(offset);
str_offset:
			if (UNEXPECTED(lval < 0)) { /* Handle negative offset */
				lval += (zend_long)Z_STRLEN_P(container);
			}
			if (EXPECTED(lval >= 0) && (size_t)lval < Z_STRLEN_P(container)) {
				return (Z_STRVAL_P(container)[lval] == '0');
			} else {
				return 1;
			}
		} else {
			/*if (OP2_TYPE & (IS_CV|IS_VAR)) {*/
				ZVAL_DEREF(offset);
			/*}*/
			if (Z_TYPE_P(offset) < IS_STRING /* simple scalar types */
					|| (Z_TYPE_P(offset) == IS_STRING /* or numeric string */
						&& IS_LONG == is_numeric_string(Z_STRVAL_P(offset), Z_STRLEN_P(offset), NULL, NULL, 0))) {
				lval = zval_get_long(offset);
				goto str_offset;
			}
			return 1;
		}
	} else {
		return 1;
	}
}

static zend_always_inline void zend_fetch_property_address(zval *result, zval *container, uint32_t container_op_type, zval *prop_ptr, uint32_t prop_op_type, void **cache_slot, int type OPLINE_DC)
{
    if (container_op_type != IS_UNUSED && UNEXPECTED(Z_TYPE_P(container) != IS_OBJECT)) {
		do {
			if (Z_ISREF_P(container)) {
				container = Z_REFVAL_P(container);
				if (EXPECTED(Z_TYPE_P(container) == IS_OBJECT)) {
					break;
				}
			}

			/* this should modify object only if it's empty */
			if (type == BP_VAR_UNSET ||
				UNEXPECTED(!make_real_object_rw(container, prop_ptr OPLINE_CC))) {
				ZVAL_ERROR(result);
				return;
			}
		} while (0);
	}
	if (prop_op_type == IS_CONST &&
	    EXPECTED(Z_OBJCE_P(container) == CACHED_PTR_EX(cache_slot))) {
		uintptr_t prop_offset = (uintptr_t)CACHED_PTR_EX(cache_slot + 1);
		zend_object *zobj = Z_OBJ_P(container);
		zval *retval;

		if (EXPECTED(IS_VALID_PROPERTY_OFFSET(prop_offset))) {
			retval = OBJ_PROP(zobj, prop_offset);
			if (EXPECTED(Z_TYPE_P(retval) != IS_UNDEF)) {
				ZVAL_INDIRECT(result, retval);
				return;
			}
		} else if (EXPECTED(zobj->properties != NULL)) {
			if (UNEXPECTED(GC_REFCOUNT(zobj->properties) > 1)) {
				if (EXPECTED(!(GC_FLAGS(zobj->properties) & IS_ARRAY_IMMUTABLE))) {
					GC_DELREF(zobj->properties);
				}
				zobj->properties = zend_array_dup(zobj->properties);
			}
			retval = zend_hash_find_ex(zobj->properties, Z_STR_P(prop_ptr), 1);
			if (EXPECTED(retval)) {
				ZVAL_INDIRECT(result, retval);
				return;
			}
		}
	}
	if (EXPECTED(Z_OBJ_HT_P(container)->get_property_ptr_ptr)) {
		zval *ptr = Z_OBJ_HT_P(container)->get_property_ptr_ptr(container, prop_ptr, type, cache_slot);
		if (NULL == ptr) {
			if (EXPECTED(Z_OBJ_HT_P(container)->read_property)) {
use_read_property:
				ptr = Z_OBJ_HT_P(container)->read_property(container, prop_ptr, type, cache_slot, result);
				if (ptr != result) {
					ZVAL_INDIRECT(result, ptr);
				} else if (UNEXPECTED(Z_ISREF_P(ptr) && Z_REFCOUNT_P(ptr) == 1)) {
					ZVAL_UNREF(ptr);
				}
			} else {
				zend_access_undefined_propery_in_overloaded_object();
				ZVAL_ERROR(result);
			}
		} else {
			ZVAL_INDIRECT(result, ptr);
		}
	} else if (EXPECTED(Z_OBJ_HT_P(container)->read_property)) {
		goto use_read_property;
	} else {
		zend_unsupported_property_reference();
		ZVAL_ERROR(result);
	}
}

static zend_never_inline void zend_fetch_this_var(int type OPLINE_DC EXECUTE_DATA_DC)
{
	zval *result = EX_VAR(opline->result.var);

	switch (type) {
		case BP_VAR_R:
			if (EXPECTED(Z_TYPE(EX(This)) == IS_OBJECT)) {
				ZVAL_OBJ(result, Z_OBJ(EX(This)));
				Z_ADDREF_P(result);
			} else {
				ZVAL_NULL(result);
				zend_error(E_NOTICE,"Undefined variable: this");
			}
			break;
		case BP_VAR_IS:
			if (EXPECTED(Z_TYPE(EX(This)) == IS_OBJECT)) {
				ZVAL_OBJ(result, Z_OBJ(EX(This)));
				Z_ADDREF_P(result);
			} else {
				ZVAL_NULL(result);
			}
			break;
		case BP_VAR_RW:
		case BP_VAR_W:
			ZVAL_UNDEF(result);
			zend_throw_error(NULL, "Cannot re-assign $this");
			break;
		case BP_VAR_UNSET:
			ZVAL_UNDEF(result);
			zend_throw_error(NULL, "Cannot unset $this");
			break;
		EMPTY_SWITCH_DEFAULT_CASE()
	}
}


#if ZEND_INTENSIVE_DEBUGGING

#define CHECK_SYMBOL_TABLES()													\
	zend_hash_apply(&EG(symbol_table), zend_check_symbol);			\
	if (&EG(symbol_table)!=EX(symbol_table)) {							\
		zend_hash_apply(EX(symbol_table), zend_check_symbol);	\
	}

static int zend_check_symbol(zval *pz)
{
	if (Z_TYPE_P(pz) == IS_INDIRECT) {
		pz = Z_INDIRECT_P(pz);
	}
	if (Z_TYPE_P(pz) > 10) {
		fprintf(stderr, "Warning!  %x has invalid type!\n", *pz);
/* See http://support.microsoft.com/kb/190351 */
#ifdef ZEND_WIN32
		fflush(stderr);
#endif
	} else if (Z_TYPE_P(pz) == IS_ARRAY) {
		zend_hash_apply(Z_ARRVAL_P(pz), zend_check_symbol);
	} else if (Z_TYPE_P(pz) == IS_OBJECT) {
		/* OBJ-TBI - doesn't support new object model! */
		zend_hash_apply(Z_OBJPROP_P(pz), zend_check_symbol);
	}

	return 0;
}


#else
#define CHECK_SYMBOL_TABLES()
#endif

ZEND_API void execute_internal(zend_execute_data *execute_data, zval *return_value)
{
	execute_data->func->internal_function.handler(execute_data, return_value);
}

ZEND_API void zend_clean_and_cache_symbol_table(zend_array *symbol_table) /* {{{ */
{
	if (EG(symtable_cache_ptr) >= EG(symtable_cache_limit)) {
		zend_array_destroy(symbol_table);
	} else {
		/* clean before putting into the cache, since clean
		   could call dtors, which could use cached hash */
		zend_symtable_clean(symbol_table);
		*(++EG(symtable_cache_ptr)) = symbol_table;
	}
}
/* }}} */

static zend_always_inline void i_free_compiled_variables(zend_execute_data *execute_data) /* {{{ */
{
	zval *cv = EX_VAR_NUM(0);
	int count = EX(func)->op_array.last_var;
	while (EXPECTED(count != 0)) {
		if (Z_REFCOUNTED_P(cv)) {
			zend_refcounted *r = Z_COUNTED_P(cv);
			if (!GC_DELREF(r)) {
				ZVAL_NULL(cv);
				rc_dtor_func(r);
			} else {
				gc_check_possible_root(r);
			}
		}
		cv++;
		count--;
	}
}
/* }}} */

ZEND_API void zend_free_compiled_variables(zend_execute_data *execute_data) /* {{{ */
{
	i_free_compiled_variables(execute_data);
}
/* }}} */

#define ZEND_VM_INTERRUPT_CHECK() do { \
		if (UNEXPECTED(EG(vm_interrupt))) { \
			ZEND_VM_INTERRUPT(); \
		} \
	} while (0)

#define ZEND_VM_LOOP_INTERRUPT_CHECK() do { \
		if (UNEXPECTED(EG(vm_interrupt))) { \
			ZEND_VM_LOOP_INTERRUPT(); \
		} \
	} while (0)

/*
 * Stack Frame Layout (the whole stack frame is allocated at once)
 * ==================
 *
 *                             +========================================+
 * EG(current_execute_data) -> | zend_execute_data                      |
 *                             +----------------------------------------+
 *     EX_CV_NUM(0) ---------> | VAR[0] = ARG[1]                        |
 *                             | ...                                    |
 *                             | VAR[op_array->num_args-1] = ARG[N]     |
 *                             | ...                                    |
 *                             | VAR[op_array->last_var-1]              |
 *                             | VAR[op_array->last_var] = TMP[0]       |
 *                             | ...                                    |
 *                             | VAR[op_array->last_var+op_array->T-1]  |
 *                             | ARG[N+1] (extra_args)                  |
 *                             | ...                                    |
 *                             +----------------------------------------+
 */

static zend_never_inline void zend_copy_extra_args(EXECUTE_DATA_D)
{
	zend_op_array *op_array = &EX(func)->op_array;
	uint32_t first_extra_arg = op_array->num_args;
	uint32_t num_args = EX_NUM_ARGS();
	zval *src;
	size_t delta;
	uint32_t count;
	uint32_t type_flags = 0;

	if (EXPECTED((op_array->fn_flags & ZEND_ACC_HAS_TYPE_HINTS) == 0)) {
		/* Skip useless ZEND_RECV and ZEND_RECV_INIT opcodes */
#if defined(ZEND_VM_FP_GLOBAL_REG) && ((ZEND_VM_KIND == ZEND_VM_KIND_CALL) || (ZEND_VM_KIND == ZEND_VM_KIND_HYBRID))
		opline += first_extra_arg;
#else
		EX(opline) += first_extra_arg;
#endif

	}

	/* move extra args into separate array after all CV and TMP vars */
	src = EX_VAR_NUM(num_args - 1);
	delta = op_array->last_var + op_array->T - first_extra_arg;
	count = num_args - first_extra_arg;
	if (EXPECTED(delta != 0)) {
		delta *= sizeof(zval);
		do {
			type_flags |= Z_TYPE_INFO_P(src);
			ZVAL_COPY_VALUE((zval*)(((char*)src) + delta), src);
			ZVAL_UNDEF(src);
			src--;
		} while (--count);
		if (Z_TYPE_INFO_REFCOUNTED(type_flags)) {
			ZEND_ADD_CALL_FLAG(execute_data, ZEND_CALL_FREE_EXTRA_ARGS);
		}
	} else {
		do {
			if (Z_REFCOUNTED_P(src)) {
				ZEND_ADD_CALL_FLAG(execute_data, ZEND_CALL_FREE_EXTRA_ARGS);
				break;
			}
			src--;
		} while (--count);
	}
}

static zend_always_inline void zend_init_cvs(uint32_t first, uint32_t last EXECUTE_DATA_DC)
{
	if (EXPECTED(first < last)) {
		uint32_t count = last - first;
		zval *var = EX_VAR_NUM(first);

		do {
			ZVAL_UNDEF(var);
			var++;
		} while (--count);
	}
}

static zend_always_inline void i_init_func_execute_data(zend_op_array *op_array, zval *return_value, zend_bool may_be_trampoline EXECUTE_DATA_DC) /* {{{ */
{
	uint32_t first_extra_arg, num_args;
	ZEND_ASSERT(EX(func) == (zend_function*)op_array);

#if defined(ZEND_VM_FP_GLOBAL_REG) && ((ZEND_VM_KIND == ZEND_VM_KIND_CALL) || (ZEND_VM_KIND == ZEND_VM_KIND_HYBRID))
	opline = op_array->opcodes;
#else
	EX(opline) = op_array->opcodes;
#endif
	EX(call) = NULL;
	EX(return_value) = return_value;

	/* Handle arguments */
	first_extra_arg = op_array->num_args;
	num_args = EX_NUM_ARGS();
	if (UNEXPECTED(num_args > first_extra_arg)) {
		if (!may_be_trampoline || EXPECTED(!(op_array->fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE))) {
			zend_copy_extra_args(EXECUTE_DATA_C);
		}
	} else if (EXPECTED((op_array->fn_flags & ZEND_ACC_HAS_TYPE_HINTS) == 0)) {
		/* Skip useless ZEND_RECV and ZEND_RECV_INIT opcodes */
#if defined(ZEND_VM_FP_GLOBAL_REG) && ((ZEND_VM_KIND == ZEND_VM_KIND_CALL) || (ZEND_VM_KIND == ZEND_VM_KIND_HYBRID))
		opline += num_args;
#else
		EX(opline) += num_args;
#endif
	}

	/* Initialize CV variables (skip arguments) */
	zend_init_cvs(num_args, op_array->last_var EXECUTE_DATA_CC);

	EX_LOAD_RUN_TIME_CACHE(op_array);

	EG(current_execute_data) = execute_data;
#if defined(ZEND_VM_FP_GLOBAL_REG) && ((ZEND_VM_KIND == ZEND_VM_KIND_CALL) || (ZEND_VM_KIND == ZEND_VM_KIND_HYBRID))
	EX(opline) = opline;
#endif
}
/* }}} */

static zend_never_inline void ZEND_FASTCALL init_func_run_time_cache(zend_op_array *op_array) /* {{{ */
{
	ZEND_ASSERT(op_array->run_time_cache == NULL);
	op_array->run_time_cache = zend_arena_alloc(&CG(arena), op_array->cache_size);
	memset(op_array->run_time_cache, 0, op_array->cache_size);
}
/* }}} */

static zend_always_inline zend_function* ZEND_FASTCALL init_func_run_time_cache_i(zval *zv) /* {{{ */
{
	zend_op_array *op_array = Z_PTR_P(zv);

	ZEND_ASSERT(op_array->run_time_cache == NULL);
	if (op_array->fn_flags & ZEND_ACC_IMMUTABLE) {
		zend_op_array *new_op_array = zend_arena_alloc(&CG(arena), sizeof(zend_op_array) + op_array->cache_size);

		Z_PTR_P(zv) = new_op_array;
		memcpy(new_op_array, op_array, sizeof(zend_op_array));
		new_op_array->fn_flags &= ~ZEND_ACC_IMMUTABLE;
		new_op_array->run_time_cache = (void**)(new_op_array + 1);
		memset(new_op_array->run_time_cache, 0, new_op_array->cache_size);
		return (zend_function*)new_op_array;
	} else {
		op_array->run_time_cache = zend_arena_alloc(&CG(arena), op_array->cache_size);
		memset(op_array->run_time_cache, 0, op_array->cache_size);
		return (zend_function*)op_array;
	}
}
/* }}} */

static zend_never_inline zend_function* init_func_run_time_cache_ex(zval *zv) /* {{{ */
{
	return init_func_run_time_cache_i(zv);
}
/* }}} */

ZEND_API zend_function * ZEND_FASTCALL zend_fetch_function(zend_string *name) /* {{{ */
{
	zval *zv = zend_hash_find(EG(function_table), name);

	if (EXPECTED(zv != NULL)) {
		zend_function *fbc = Z_FUNC_P(zv);

		if (EXPECTED(fbc->type == ZEND_USER_FUNCTION) && UNEXPECTED(!fbc->op_array.run_time_cache)) {
			fbc = (zend_function*)init_func_run_time_cache_i(zv);
		}
		return fbc;
	}
	return NULL;
} /* }}} */

ZEND_API zend_function * ZEND_FASTCALL zend_fetch_function_str(const char *name, size_t len) /* {{{ */
{
	zval *zv = zend_hash_str_find(EG(function_table), name, len);

	if (EXPECTED(zv != NULL)) {
		zend_function *fbc = Z_FUNC_P(zv);

		if (EXPECTED(fbc->type == ZEND_USER_FUNCTION) && UNEXPECTED(!fbc->op_array.run_time_cache)) {
			fbc = (zend_function*)init_func_run_time_cache_i(zv);
		}
		return fbc;
	}
	return NULL;
} /* }}} */

static zend_always_inline void i_init_code_execute_data(zend_execute_data *execute_data, zend_op_array *op_array, zval *return_value) /* {{{ */
{
	ZEND_ASSERT(EX(func) == (zend_function*)op_array);

	EX(opline) = op_array->opcodes;
	EX(call) = NULL;
	EX(return_value) = return_value;

	zend_attach_symbol_table(execute_data);

	if (!op_array->run_time_cache) {
		op_array->run_time_cache = emalloc(op_array->cache_size);
		memset(op_array->run_time_cache, 0, op_array->cache_size);
	}
	EX_LOAD_RUN_TIME_CACHE(op_array);

	EG(current_execute_data) = execute_data;
}
/* }}} */

ZEND_API void zend_init_func_execute_data(zend_execute_data *ex, zend_op_array *op_array, zval *return_value) /* {{{ */
{
#if defined(ZEND_VM_FP_GLOBAL_REG) && ((ZEND_VM_KIND == ZEND_VM_KIND_CALL) || (ZEND_VM_KIND == ZEND_VM_KIND_HYBRID))
	zend_execute_data *orig_execute_data = execute_data;
	const zend_op *orig_opline = opline;
	execute_data = ex;
#else
	zend_execute_data *execute_data = ex;
#endif

	EX(prev_execute_data) = EG(current_execute_data);
	if (!op_array->run_time_cache) {
		init_func_run_time_cache(op_array);
	}
	i_init_func_execute_data(op_array, return_value, 1 EXECUTE_DATA_CC);

#if defined(ZEND_VM_FP_GLOBAL_REG) && ((ZEND_VM_KIND == ZEND_VM_KIND_CALL) || (ZEND_VM_KIND == ZEND_VM_KIND_HYBRID))
    EX(opline) = opline;
	opline = orig_opline;
	execute_data = orig_execute_data;
#endif
}
/* }}} */

ZEND_API void zend_init_code_execute_data(zend_execute_data *execute_data, zend_op_array *op_array, zval *return_value) /* {{{ */
{
	EX(prev_execute_data) = EG(current_execute_data);
	if (!op_array->run_time_cache) {
		op_array->run_time_cache = emalloc(op_array->cache_size);
		memset(op_array->run_time_cache, 0, op_array->cache_size);
	}
	i_init_code_execute_data(execute_data, op_array, return_value);
}
/* }}} */

ZEND_API void zend_init_execute_data(zend_execute_data *execute_data, zend_op_array *op_array, zval *return_value) /* {{{ */
{
	if (EX_CALL_INFO() & ZEND_CALL_HAS_SYMBOL_TABLE) {
		zend_init_code_execute_data(execute_data, op_array, return_value);
	} else {
		zend_init_func_execute_data(execute_data, op_array, return_value);
	}
}
/* }}} */

static zend_always_inline zend_bool zend_is_by_ref_func_arg_fetch(uint32_t arg_num, zend_function *func) /* {{{ */
{
	if (EXPECTED(arg_num <= MAX_ARG_FLAG_NUM)) {
		return QUICK_ARG_SHOULD_BE_SENT_BY_REF(func, arg_num);
	}
	return ARG_SHOULD_BE_SENT_BY_REF(func, arg_num);
}
/* }}} */

static zend_execute_data *zend_vm_stack_copy_call_frame(zend_execute_data *call, uint32_t passed_args, uint32_t additional_args) /* {{{ */
{
	zend_execute_data *new_call;
	int used_stack = (EG(vm_stack_top) - (zval*)call) + additional_args;

	/* copy call frame into new stack segment */
	new_call = zend_vm_stack_extend(used_stack * sizeof(zval));
	*new_call = *call;
	ZEND_ADD_CALL_FLAG(new_call, ZEND_CALL_ALLOCATED);

	if (passed_args) {
		zval *src = ZEND_CALL_ARG(call, 1);
		zval *dst = ZEND_CALL_ARG(new_call, 1);
		do {
			ZVAL_COPY_VALUE(dst, src);
			passed_args--;
			src++;
			dst++;
		} while (passed_args);
	}

	/* delete old call_frame from previous stack segment */
	EG(vm_stack)->prev->top = (zval*)call;

	/* delete previous stack segment if it becames empty */
	if (UNEXPECTED(EG(vm_stack)->prev->top == ZEND_VM_STACK_ELEMENTS(EG(vm_stack)->prev))) {
		zend_vm_stack r = EG(vm_stack)->prev;

		EG(vm_stack)->prev = r->prev;
		efree(r);
	}

	return new_call;
}
/* }}} */

static zend_always_inline void zend_vm_stack_extend_call_frame(zend_execute_data **call, uint32_t passed_args, uint32_t additional_args) /* {{{ */
{
	if (EXPECTED((uint32_t)(EG(vm_stack_end) - EG(vm_stack_top)) > additional_args)) {
		EG(vm_stack_top) += additional_args;
	} else {
		*call = zend_vm_stack_copy_call_frame(*call, passed_args, additional_args);
	}
}
/* }}} */

static zend_always_inline zend_generator *zend_get_running_generator(EXECUTE_DATA_D) /* {{{ */
{
	/* The generator object is stored in EX(return_value) */
	zend_generator *generator = (zend_generator *) EX(return_value);
	/* However control may currently be delegated to another generator.
	 * That's the one we're interested in. */
	return generator;
}
/* }}} */

static void cleanup_unfinished_calls(zend_execute_data *execute_data, uint32_t op_num) /* {{{ */
{
	if (UNEXPECTED(EX(call))) {
		zend_execute_data *call = EX(call);
		zend_op *opline = EX(func)->op_array.opcodes + op_num;
		int level;
		int do_exit;

		if (UNEXPECTED(opline->opcode == ZEND_INIT_FCALL ||
			opline->opcode == ZEND_INIT_FCALL_BY_NAME ||
			opline->opcode == ZEND_INIT_NS_FCALL_BY_NAME ||
			opline->opcode == ZEND_INIT_DYNAMIC_CALL ||
			opline->opcode == ZEND_INIT_USER_CALL ||
			opline->opcode == ZEND_INIT_METHOD_CALL ||
			opline->opcode == ZEND_INIT_STATIC_METHOD_CALL ||
			opline->opcode == ZEND_NEW)) {
			ZEND_ASSERT(op_num);
			opline--;
		}

		do {
			/* If the exception was thrown during a function call there might be
			 * arguments pushed to the stack that have to be dtor'ed. */

			/* find the number of actually passed arguments */
			level = 0;
			do_exit = 0;
			do {
				switch (opline->opcode) {
					case ZEND_DO_FCALL:
					case ZEND_DO_ICALL:
					case ZEND_DO_UCALL:
					case ZEND_DO_FCALL_BY_NAME:
						level++;
						break;
					case ZEND_INIT_FCALL:
					case ZEND_INIT_FCALL_BY_NAME:
					case ZEND_INIT_NS_FCALL_BY_NAME:
					case ZEND_INIT_DYNAMIC_CALL:
					case ZEND_INIT_USER_CALL:
					case ZEND_INIT_METHOD_CALL:
					case ZEND_INIT_STATIC_METHOD_CALL:
					case ZEND_NEW:
						if (level == 0) {
							ZEND_CALL_NUM_ARGS(call) = 0;
							do_exit = 1;
						}
						level--;
						break;
					case ZEND_SEND_VAL:
					case ZEND_SEND_VAL_EX:
					case ZEND_SEND_VAR:
					case ZEND_SEND_VAR_EX:
					case ZEND_SEND_FUNC_ARG:
					case ZEND_SEND_REF:
					case ZEND_SEND_VAR_NO_REF:
					case ZEND_SEND_VAR_NO_REF_EX:
					case ZEND_SEND_USER:
						if (level == 0) {
							ZEND_CALL_NUM_ARGS(call) = opline->op2.num;
							do_exit = 1;
						}
						break;
					case ZEND_SEND_ARRAY:
					case ZEND_SEND_UNPACK:
						if (level == 0) {
							do_exit = 1;
						}
						break;
				}
				if (!do_exit) {
					opline--;
				}
			} while (!do_exit);
			if (call->prev_execute_data) {
				/* skip current call region */
				level = 0;
				do_exit = 0;
				do {
					switch (opline->opcode) {
						case ZEND_DO_FCALL:
						case ZEND_DO_ICALL:
						case ZEND_DO_UCALL:
						case ZEND_DO_FCALL_BY_NAME:
							level++;
							break;
						case ZEND_INIT_FCALL:
						case ZEND_INIT_FCALL_BY_NAME:
						case ZEND_INIT_NS_FCALL_BY_NAME:
						case ZEND_INIT_DYNAMIC_CALL:
						case ZEND_INIT_USER_CALL:
						case ZEND_INIT_METHOD_CALL:
						case ZEND_INIT_STATIC_METHOD_CALL:
						case ZEND_NEW:
							if (level == 0) {
								do_exit = 1;
							}
							level--;
							break;
					}
					opline--;
				} while (!do_exit);
			}

			zend_vm_stack_free_args(EX(call));

			if (ZEND_CALL_INFO(call) & ZEND_CALL_RELEASE_THIS) {
				if (ZEND_CALL_INFO(call) & ZEND_CALL_CTOR) {
					GC_DELREF(Z_OBJ(call->This));
					if (GC_REFCOUNT(Z_OBJ(call->This)) == 1) {
						zend_object_store_ctor_failed(Z_OBJ(call->This));
					}
				}
				OBJ_RELEASE(Z_OBJ(call->This));
			}
			if (call->func->common.fn_flags & ZEND_ACC_CLOSURE) {
				zend_object_release(ZEND_CLOSURE_OBJECT(call->func));
			} else if (call->func->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE) {
				zend_string_release_ex(call->func->common.function_name, 0);
				zend_free_trampoline(call->func);
			}

			EX(call) = call->prev_execute_data;
			zend_vm_stack_free_call_frame(call);
			call = EX(call);
		} while (call);
	}
}
/* }}} */

static const zend_live_range *find_live_range(const zend_op_array *op_array, uint32_t op_num, uint32_t var_num) /* {{{ */
{
	int i;
	for (i = 0; i < op_array->last_live_range; i++) {
		const zend_live_range *range = &op_array->live_range[i];
		if (op_num >= range->start && op_num < range->end
				&& var_num == (range->var & ~ZEND_LIVE_MASK)) {
			return range;
		}
	}
	return NULL;
}
/* }}} */

static void cleanup_live_vars(zend_execute_data *execute_data, uint32_t op_num, uint32_t catch_op_num) /* {{{ */
{
	int i;

	for (i = 0; i < EX(func)->op_array.last_live_range; i++) {
		const zend_live_range *range = &EX(func)->op_array.live_range[i];
		if (range->start > op_num) {
			/* further blocks will not be relevant... */
			break;
		} else if (op_num < range->end) {
			if (!catch_op_num || catch_op_num >= range->end) {
				uint32_t kind = range->var & ZEND_LIVE_MASK;
				uint32_t var_num = range->var & ~ZEND_LIVE_MASK;
				zval *var = EX_VAR(var_num);

				if (kind == ZEND_LIVE_TMPVAR) {
					zval_ptr_dtor_nogc(var);
				} else if (kind == ZEND_LIVE_LOOP) {
					if (Z_TYPE_P(var) != IS_ARRAY && Z_FE_ITER_P(var) != (uint32_t)-1) {
						zend_hash_iterator_del(Z_FE_ITER_P(var));
					}
					zval_ptr_dtor_nogc(var);
				} else if (kind == ZEND_LIVE_ROPE) {
					zend_string **rope = (zend_string **)var;
					zend_op *last = EX(func)->op_array.opcodes + op_num;
					while ((last->opcode != ZEND_ROPE_ADD && last->opcode != ZEND_ROPE_INIT)
							|| last->result.var != var_num) {
						ZEND_ASSERT(last >= EX(func)->op_array.opcodes);
						last--;
					}
					if (last->opcode == ZEND_ROPE_INIT) {
						zend_string_release_ex(*rope, 0);
					} else {
						int j = last->extended_value;
						do {
							zend_string_release_ex(rope[j], 0);
						} while (j--);
					}
				} else if (kind == ZEND_LIVE_SILENCE) {
					/* restore previous error_reporting value */
					if (!EG(error_reporting) && Z_LVAL_P(var) != 0) {
						EG(error_reporting) = Z_LVAL_P(var);
					}
				}
			}
		}
	}
}
/* }}} */

ZEND_API void zend_cleanup_unfinished_execution(zend_execute_data *execute_data, uint32_t op_num, uint32_t catch_op_num) {
	cleanup_unfinished_calls(execute_data, op_num);
	cleanup_live_vars(execute_data, op_num, catch_op_num);
}

static void zend_swap_operands(zend_op *op) /* {{{ */
{
	znode_op     tmp;
	zend_uchar   tmp_type;

	tmp          = op->op1;
	tmp_type     = op->op1_type;
	op->op1      = op->op2;
	op->op1_type = op->op2_type;
	op->op2      = tmp;
	op->op2_type = tmp_type;
}
/* }}} */

static zend_never_inline zend_execute_data *zend_init_dynamic_call_string(zend_string *function, uint32_t num_args) /* {{{ */
{
	zend_function *fbc;
	zval *func;
	zend_class_entry *called_scope;
	zend_string *lcname;
	const char *colon;

	if ((colon = zend_memrchr(ZSTR_VAL(function), ':', ZSTR_LEN(function))) != NULL &&
		colon > ZSTR_VAL(function) &&
		*(colon-1) == ':'
	) {
		zend_string *mname;
		size_t cname_length = colon - ZSTR_VAL(function) - 1;
		size_t mname_length = ZSTR_LEN(function) - cname_length - (sizeof("::") - 1);

		lcname = zend_string_init(ZSTR_VAL(function), cname_length, 0);

		called_scope = zend_fetch_class_by_name(lcname, NULL, ZEND_FETCH_CLASS_DEFAULT | ZEND_FETCH_CLASS_EXCEPTION);
		if (UNEXPECTED(called_scope == NULL)) {
			zend_string_release_ex(lcname, 0);
			return NULL;
		}

		mname = zend_string_init(ZSTR_VAL(function) + (cname_length + sizeof("::") - 1), mname_length, 0);

		if (called_scope->get_static_method) {
			fbc = called_scope->get_static_method(called_scope, mname);
		} else {
			fbc = zend_std_get_static_method(called_scope, mname, NULL);
		}
		if (UNEXPECTED(fbc == NULL)) {
			if (EXPECTED(!EG(exception))) {
				zend_undefined_method(called_scope, mname);
			}
			zend_string_release_ex(lcname, 0);
			zend_string_release_ex(mname, 0);
			return NULL;
		}

		zend_string_release_ex(lcname, 0);
		zend_string_release_ex(mname, 0);

		if (UNEXPECTED(!(fbc->common.fn_flags & ZEND_ACC_STATIC))) {
			zend_non_static_method_call(fbc);
			if (UNEXPECTED(EG(exception) != NULL)) {
				return NULL;
			}
		}
		if (EXPECTED(fbc->type == ZEND_USER_FUNCTION) && UNEXPECTED(!fbc->op_array.run_time_cache)) {
			init_func_run_time_cache(&fbc->op_array);
		}
	} else {
		if (ZSTR_VAL(function)[0] == '\\') {
			lcname = zend_string_alloc(ZSTR_LEN(function) - 1, 0);
			zend_str_tolower_copy(ZSTR_VAL(lcname), ZSTR_VAL(function) + 1, ZSTR_LEN(function) - 1);
		} else {
			lcname = zend_string_tolower(function);
		}
		if (UNEXPECTED((func = zend_hash_find(EG(function_table), lcname)) == NULL)) {
			zend_throw_error(NULL, "Call to undefined function %s()", ZSTR_VAL(function));
			zend_string_release_ex(lcname, 0);
			return NULL;
		}
		zend_string_release_ex(lcname, 0);

		fbc = Z_FUNC_P(func);
		if (EXPECTED(fbc->type == ZEND_USER_FUNCTION) && UNEXPECTED(!fbc->op_array.run_time_cache)) {
			fbc = init_func_run_time_cache_ex(func);
		}
		called_scope = NULL;
	}

	return zend_vm_stack_push_call_frame(ZEND_CALL_NESTED_FUNCTION | ZEND_CALL_DYNAMIC,
		fbc, num_args, called_scope, NULL);
}
/* }}} */

static zend_never_inline zend_execute_data *zend_init_dynamic_call_object(zval *function, uint32_t num_args) /* {{{ */
{
	zend_function *fbc;
	zend_class_entry *called_scope;
	zend_object *object;
	uint32_t call_info = ZEND_CALL_NESTED_FUNCTION | ZEND_CALL_DYNAMIC;

	if (EXPECTED(Z_OBJ_HANDLER_P(function, get_closure)) &&
	    EXPECTED(Z_OBJ_HANDLER_P(function, get_closure)(function, &called_scope, &fbc, &object) == SUCCESS)) {

		if (fbc->common.fn_flags & ZEND_ACC_CLOSURE) {
			/* Delay closure destruction until its invocation */
			GC_ADDREF(ZEND_CLOSURE_OBJECT(fbc));
			call_info |= ZEND_CALL_CLOSURE;
			if (fbc->common.fn_flags & ZEND_ACC_FAKE_CLOSURE) {
				call_info |= ZEND_CALL_FAKE_CLOSURE;
			}
		} else if (object) {
			call_info |= ZEND_CALL_RELEASE_THIS;
			GC_ADDREF(object); /* For $this pointer */
		}
	} else {
		zend_throw_error(NULL, "Function name must be a string");
		return NULL;
	}

	if (EXPECTED(fbc->type == ZEND_USER_FUNCTION) && UNEXPECTED(!fbc->op_array.run_time_cache)) {
		init_func_run_time_cache(&fbc->op_array);
	}

	return zend_vm_stack_push_call_frame(call_info,
		fbc, num_args, called_scope, object);
}
/* }}} */

static zend_never_inline zend_execute_data *zend_init_dynamic_call_array(zend_array *function, uint32_t num_args) /* {{{ */
{
	zend_function *fbc;
	zend_class_entry *called_scope;
	zend_object *object;
	uint32_t call_info = ZEND_CALL_NESTED_FUNCTION | ZEND_CALL_DYNAMIC;

	if (zend_hash_num_elements(function) == 2) {
		zval *obj;
		zval *method;
		obj = zend_hash_index_find(function, 0);
		method = zend_hash_index_find(function, 1);

		if (UNEXPECTED(!obj) || UNEXPECTED(!method)) {
			zend_throw_error(NULL, "Array callback has to contain indices 0 and 1");
			return NULL;
		}

		ZVAL_DEREF(obj);
		if (UNEXPECTED(Z_TYPE_P(obj) != IS_STRING) && UNEXPECTED(Z_TYPE_P(obj) != IS_OBJECT)) {
			zend_throw_error(NULL, "First array member is not a valid class name or object");
			return NULL;
		}

		ZVAL_DEREF(method);
		if (UNEXPECTED(Z_TYPE_P(method) != IS_STRING)) {
			zend_throw_error(NULL, "Second array member is not a valid method");
			return NULL;
		}

		if (Z_TYPE_P(obj) == IS_STRING) {
			object = NULL;
			called_scope = zend_fetch_class_by_name(Z_STR_P(obj), NULL, ZEND_FETCH_CLASS_DEFAULT | ZEND_FETCH_CLASS_EXCEPTION);
			if (UNEXPECTED(called_scope == NULL)) {
				return NULL;
			}

			if (called_scope->get_static_method) {
				fbc = called_scope->get_static_method(called_scope, Z_STR_P(method));
			} else {
				fbc = zend_std_get_static_method(called_scope, Z_STR_P(method), NULL);
			}
			if (UNEXPECTED(fbc == NULL)) {
				if (EXPECTED(!EG(exception))) {
					zend_undefined_method(called_scope, Z_STR_P(method));
				}
				return NULL;
			}
			if (!(fbc->common.fn_flags & ZEND_ACC_STATIC)) {
				zend_non_static_method_call(fbc);
				if (UNEXPECTED(EG(exception) != NULL)) {
					return NULL;
				}
			}
		} else {
			called_scope = Z_OBJCE_P(obj);
			object = Z_OBJ_P(obj);

			fbc = Z_OBJ_HT_P(obj)->get_method(&object, Z_STR_P(method), NULL);
			if (UNEXPECTED(fbc == NULL)) {
				if (EXPECTED(!EG(exception))) {
					zend_undefined_method(object->ce, Z_STR_P(method));
				}
				return NULL;
			}

			if ((fbc->common.fn_flags & ZEND_ACC_STATIC) != 0) {
				object = NULL;
			} else {
				call_info |= ZEND_CALL_RELEASE_THIS;
				GC_ADDREF(object); /* For $this pointer */
			}
		}
	} else {
		zend_throw_error(NULL, "Function name must be a string");
		return NULL;
	}

	if (EXPECTED(fbc->type == ZEND_USER_FUNCTION) && UNEXPECTED(!fbc->op_array.run_time_cache)) {
		init_func_run_time_cache(&fbc->op_array);
	}

	return zend_vm_stack_push_call_frame(call_info,
		fbc, num_args, called_scope, object);
}
/* }}} */

#define ZEND_FAKE_OP_ARRAY ((zend_op_array*)(zend_intptr_t)-1)

static zend_never_inline zend_op_array* ZEND_FASTCALL zend_include_or_eval(zval *inc_filename, int type) /* {{{ */
{
	zend_op_array *new_op_array = NULL;
	zval tmp_inc_filename;

	ZVAL_UNDEF(&tmp_inc_filename);
	if (Z_TYPE_P(inc_filename) != IS_STRING) {
		ZVAL_STR(&tmp_inc_filename, zval_get_string_func(inc_filename));
		inc_filename = &tmp_inc_filename;
	}

	switch (type) {
		case ZEND_INCLUDE_ONCE:
		case ZEND_REQUIRE_ONCE: {
				zend_file_handle file_handle;
				zend_string *resolved_path;

				resolved_path = zend_resolve_path(Z_STRVAL_P(inc_filename), Z_STRLEN_P(inc_filename));
				if (EXPECTED(resolved_path)) {
					if (zend_hash_exists(&EG(included_files), resolved_path)) {
						goto already_compiled;
					}
				} else if (UNEXPECTED(strlen(Z_STRVAL_P(inc_filename)) != Z_STRLEN_P(inc_filename))) {
					zend_message_dispatcher(
						(type == ZEND_INCLUDE_ONCE) ?
							ZMSG_FAILED_INCLUDE_FOPEN : ZMSG_FAILED_REQUIRE_FOPEN,
							Z_STRVAL_P(inc_filename));
					break;
				} else {
					resolved_path = zend_string_copy(Z_STR_P(inc_filename));
				}

				if (SUCCESS == zend_stream_open(ZSTR_VAL(resolved_path), &file_handle)) {

					if (!file_handle.opened_path) {
						file_handle.opened_path = zend_string_copy(resolved_path);
					}

					if (zend_hash_add_empty_element(&EG(included_files), file_handle.opened_path)) {
						zend_op_array *op_array = zend_compile_file(&file_handle, (type==ZEND_INCLUDE_ONCE?ZEND_INCLUDE:ZEND_REQUIRE));
						zend_destroy_file_handle(&file_handle);
						zend_string_release_ex(resolved_path, 0);
						if (Z_TYPE(tmp_inc_filename) != IS_UNDEF) {
							zval_ptr_dtor_str(&tmp_inc_filename);
						}
						return op_array;
					} else {
						zend_file_handle_dtor(&file_handle);
already_compiled:
						new_op_array = ZEND_FAKE_OP_ARRAY;
					}
				} else {
					zend_message_dispatcher(
						(type == ZEND_INCLUDE_ONCE) ?
							ZMSG_FAILED_INCLUDE_FOPEN : ZMSG_FAILED_REQUIRE_FOPEN,
							Z_STRVAL_P(inc_filename));
				}
				zend_string_release_ex(resolved_path, 0);
			}
			break;
		case ZEND_INCLUDE:
		case ZEND_REQUIRE:
			if (UNEXPECTED(strlen(Z_STRVAL_P(inc_filename)) != Z_STRLEN_P(inc_filename))) {
				zend_message_dispatcher(
					(type == ZEND_INCLUDE) ?
						ZMSG_FAILED_INCLUDE_FOPEN : ZMSG_FAILED_REQUIRE_FOPEN,
						Z_STRVAL_P(inc_filename));
				break;
			}
			new_op_array = compile_filename(type, inc_filename);
			break;
		case ZEND_EVAL: {
				char *eval_desc = zend_make_compiled_string_description("eval()'d code");
				new_op_array = zend_compile_string(inc_filename, eval_desc);
				efree(eval_desc);
			}
			break;
		EMPTY_SWITCH_DEFAULT_CASE()
	}

	if (Z_TYPE(tmp_inc_filename) != IS_UNDEF) {
		zval_ptr_dtor_str(&tmp_inc_filename);
	}
	return new_op_array;
}
/* }}} */

ZEND_API int ZEND_FASTCALL zend_do_fcall_overloaded(zend_execute_data *call, zval *ret) /* {{{ */
{
	zend_function *fbc = call->func;
	zend_object *object;

	/* Not sure what should be done here if it's a static method */
	if (UNEXPECTED(Z_TYPE(call->This) != IS_OBJECT)) {
		zend_vm_stack_free_args(call);
		if (fbc->type == ZEND_OVERLOADED_FUNCTION_TEMPORARY) {
			zend_string_release_ex(fbc->common.function_name, 0);
		}
		efree(fbc);
		zend_vm_stack_free_call_frame(call);

		zend_throw_error(NULL, "Cannot call overloaded function for non-object");
		return 0;
	}

	object = Z_OBJ(call->This);

	ZVAL_NULL(ret);

	EG(current_execute_data) = call;
	object->handlers->call_method(fbc->common.function_name, object, call, ret);
	EG(current_execute_data) = call->prev_execute_data;

	zend_vm_stack_free_args(call);

	if (fbc->type == ZEND_OVERLOADED_FUNCTION_TEMPORARY) {
		zend_string_release_ex(fbc->common.function_name, 0);
	}
	efree(fbc);

	return 1;
}
/* }}} */

static zend_never_inline zend_bool ZEND_FASTCALL zend_fe_reset_iterator(zval *array_ptr, int by_ref OPLINE_DC EXECUTE_DATA_DC) /* {{{ */
{
	zend_class_entry *ce = Z_OBJCE_P(array_ptr);
	zend_object_iterator *iter = ce->get_iterator(ce, array_ptr, by_ref);
	zend_bool is_empty;

	if (UNEXPECTED(!iter) || UNEXPECTED(EG(exception))) {
		if (iter) {
			OBJ_RELEASE(&iter->std);
		}
		if (!EG(exception)) {
			zend_throw_exception_ex(NULL, 0, "Object of type %s did not create an Iterator", ZSTR_VAL(ce->name));
		}
		ZVAL_UNDEF(EX_VAR(opline->result.var));
		return 1;
	}

	iter->index = 0;
	if (iter->funcs->rewind) {
		iter->funcs->rewind(iter);
		if (UNEXPECTED(EG(exception) != NULL)) {
			OBJ_RELEASE(&iter->std);
			ZVAL_UNDEF(EX_VAR(opline->result.var));
			return 1;
		}
	}

	is_empty = iter->funcs->valid(iter) != SUCCESS;

	if (UNEXPECTED(EG(exception) != NULL)) {
		OBJ_RELEASE(&iter->std);
		ZVAL_UNDEF(EX_VAR(opline->result.var));
		return 1;
	}
	iter->index = -1; /* will be set to 0 before using next handler */

	ZVAL_OBJ(EX_VAR(opline->result.var), &iter->std);
	Z_FE_ITER_P(EX_VAR(opline->result.var)) = (uint32_t)-1;

	return is_empty;
}
/* }}} */

static zend_always_inline int _zend_quick_get_constant(
		const zval *key, uint32_t flags, int check_defined_only OPLINE_DC EXECUTE_DATA_DC) /* {{{ */
{
	zval *zv;
	const zval *orig_key = key;
	zend_constant *c = NULL;

	zv = zend_hash_find_ex(EG(zend_constants), Z_STR_P(key), 1);
	if (zv) {
		c = (zend_constant*)Z_PTR_P(zv);
	} else {
		key++;
		zv = zend_hash_find_ex(EG(zend_constants), Z_STR_P(key), 1);
		if (zv && (ZEND_CONSTANT_FLAGS((zend_constant*)Z_PTR_P(zv)) & CONST_CS) == 0) {
			c = (zend_constant*)Z_PTR_P(zv);
		} else {
			if ((flags & (IS_CONSTANT_IN_NAMESPACE|IS_CONSTANT_UNQUALIFIED)) == (IS_CONSTANT_IN_NAMESPACE|IS_CONSTANT_UNQUALIFIED)) {
				key++;
				zv = zend_hash_find_ex(EG(zend_constants), Z_STR_P(key), 1);
				if (zv) {
					c = (zend_constant*)Z_PTR_P(zv);
				} else {
				    key++;
					zv = zend_hash_find_ex(EG(zend_constants), Z_STR_P(key), 1);
					if (zv && (ZEND_CONSTANT_FLAGS((zend_constant*)Z_PTR_P(zv)) & CONST_CS) == 0) {
						c = (zend_constant*)Z_PTR_P(zv);
					}
				}
			}
		}
	}

	if (!c) {
		if (!check_defined_only) {
			if ((opline->op1.num & IS_CONSTANT_UNQUALIFIED) != 0) {
				char *actual = (char *)zend_memrchr(Z_STRVAL_P(RT_CONSTANT(opline, opline->op2)), '\\', Z_STRLEN_P(RT_CONSTANT(opline, opline->op2)));
				if (!actual) {
					ZVAL_STR_COPY(EX_VAR(opline->result.var), Z_STR_P(RT_CONSTANT(opline, opline->op2)));
				} else {
					actual++;
					ZVAL_STRINGL(EX_VAR(opline->result.var),
							actual, Z_STRLEN_P(RT_CONSTANT(opline, opline->op2)) - (actual - Z_STRVAL_P(RT_CONSTANT(opline, opline->op2))));
				}
				/* non-qualified constant - allow text substitution */
				zend_error(E_WARNING, "Use of undefined constant %s - assumed '%s' (this will throw an Error in a future version of PHP)",
						Z_STRVAL_P(EX_VAR(opline->result.var)), Z_STRVAL_P(EX_VAR(opline->result.var)));
			} else {
				zend_throw_error(NULL, "Undefined constant '%s'", Z_STRVAL_P(RT_CONSTANT(opline, opline->op2)));
				ZVAL_UNDEF(EX_VAR(opline->result.var));
			}
		}
		return FAILURE;
	}

	if (!check_defined_only) {
		ZVAL_COPY_OR_DUP(EX_VAR(opline->result.var), &c->value);
		if (!(ZEND_CONSTANT_FLAGS(c) & (CONST_CS|CONST_CT_SUBST))) {
			const char *ns_sep;
			size_t shortname_offset;
			size_t shortname_len;
			zend_bool is_deprecated;

			if (flags & IS_CONSTANT_UNQUALIFIED) {
				const zval *access_key;

				if (!(flags & IS_CONSTANT_IN_NAMESPACE)) {
					access_key = orig_key - 1;
				} else {
					if (key < orig_key + 2) {
						goto check_short_name;
					} else {
						access_key = orig_key + 2;
					}
				}
				is_deprecated = !zend_string_equals(c->name, Z_STR_P(access_key));
			} else {
check_short_name:
				ns_sep = zend_memrchr(ZSTR_VAL(c->name), '\\', ZSTR_LEN(c->name));
				ZEND_ASSERT(ns_sep);
				/* Namespaces are always case-insensitive. Only compare shortname. */
				shortname_offset = ns_sep - ZSTR_VAL(c->name) + 1;
				shortname_len = ZSTR_LEN(c->name) - shortname_offset;

				is_deprecated = memcmp(ZSTR_VAL(c->name) + shortname_offset, Z_STRVAL_P(orig_key - 1) + shortname_offset, shortname_len) != 0;
			}

			if (is_deprecated) {
				zend_error(E_DEPRECATED,
					"Case-insensitive constants are deprecated. "
					"The correct casing for this constant is \"%s\"",
					ZSTR_VAL(c->name));
				return SUCCESS;
			}
		}
	}

	CACHE_PTR(opline->extended_value, c);
	return SUCCESS;
}
/* }}} */

static zend_never_inline void ZEND_FASTCALL zend_quick_get_constant(
		const zval *key, uint32_t flags OPLINE_DC EXECUTE_DATA_DC) /* {{{ */
{
	_zend_quick_get_constant(key, flags, 0 OPLINE_CC EXECUTE_DATA_CC);
} /* }}} */

static zend_never_inline int ZEND_FASTCALL zend_quick_check_constant(
		const zval *key OPLINE_DC EXECUTE_DATA_DC) /* {{{ */
{
	return _zend_quick_get_constant(key, 0, 1 OPLINE_CC EXECUTE_DATA_CC);
} /* }}} */

#ifdef ZEND_VM_TRACE_HANDLERS
# include "zend_vm_trace_handlers.h"
#elif defined(ZEND_VM_TRACE_MAP)
# include "zend_vm_trace_map.h"
#endif

#define ZEND_VM_NEXT_OPCODE_EX(check_exception, skip) \
	CHECK_SYMBOL_TABLES() \
	if (check_exception) { \
		OPLINE = EX(opline) + (skip); \
	} else { \
		OPLINE = opline + (skip); \
	} \
	ZEND_VM_CONTINUE()

#define ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION() \
	ZEND_VM_NEXT_OPCODE_EX(1, 1)

#define ZEND_VM_NEXT_OPCODE() \
	ZEND_VM_NEXT_OPCODE_EX(0, 1)

#define ZEND_VM_SET_NEXT_OPCODE(new_op) \
	CHECK_SYMBOL_TABLES() \
	OPLINE = new_op

#define ZEND_VM_SET_OPCODE(new_op) \
	CHECK_SYMBOL_TABLES() \
	OPLINE = new_op; \
	ZEND_VM_INTERRUPT_CHECK()

#define ZEND_VM_SET_RELATIVE_OPCODE(opline, offset) \
	ZEND_VM_SET_OPCODE(ZEND_OFFSET_TO_OPLINE(opline, offset))

#define ZEND_VM_JMP_EX(new_op, check_exception) do { \
		if (check_exception && UNEXPECTED(EG(exception))) { \
			HANDLE_EXCEPTION(); \
		} \
		ZEND_VM_SET_OPCODE(new_op); \
		ZEND_VM_CONTINUE(); \
	} while (0)

#define ZEND_VM_JMP(new_op) \
	ZEND_VM_JMP_EX(new_op, 1)

#define ZEND_VM_INC_OPCODE() \
	OPLINE++


#ifndef VM_SMART_OPCODES
# define VM_SMART_OPCODES 1
#endif

#if VM_SMART_OPCODES
# define ZEND_VM_REPEATABLE_OPCODE \
	do {
# define ZEND_VM_REPEAT_OPCODE(_opcode) \
	} while (UNEXPECTED((++opline)->opcode == _opcode)); \
	OPLINE = opline; \
	ZEND_VM_CONTINUE()
# define ZEND_VM_SMART_BRANCH(_result, _check) do { \
		int __result; \
		if (EXPECTED((opline+1)->opcode == ZEND_JMPZ)) { \
			__result = (_result); \
		} else if (EXPECTED((opline+1)->opcode == ZEND_JMPNZ)) { \
			__result = !(_result); \
		} else { \
			break; \
		} \
		if ((_check) && UNEXPECTED(EG(exception))) { \
			ZVAL_UNDEF(EX_VAR(opline->result.var)); \
			HANDLE_EXCEPTION(); \
		} \
		if (__result) { \
			ZEND_VM_SET_NEXT_OPCODE(opline + 2); \
		} else { \
			ZEND_VM_SET_OPCODE(OP_JMP_ADDR(opline + 1, (opline+1)->op2)); \
		} \
		ZEND_VM_CONTINUE(); \
	} while (0)
# define ZEND_VM_SMART_BRANCH_JMPZ(_result, _check) do { \
		if ((_check) && UNEXPECTED(EG(exception))) { \
			ZVAL_UNDEF(EX_VAR(opline->result.var)); \
			HANDLE_EXCEPTION(); \
		} \
		if (_result) { \
			ZEND_VM_SET_NEXT_OPCODE(opline + 2); \
		} else { \
			ZEND_VM_SET_OPCODE(OP_JMP_ADDR(opline + 1, (opline+1)->op2)); \
		} \
		ZEND_VM_CONTINUE(); \
	} while (0)
# define ZEND_VM_SMART_BRANCH_JMPNZ(_result, _check) do { \
		if ((_check) && UNEXPECTED(EG(exception))) { \
			ZVAL_UNDEF(EX_VAR(opline->result.var)); \
			HANDLE_EXCEPTION(); \
		} \
		if (!(_result)) { \
			ZEND_VM_SET_NEXT_OPCODE(opline + 2); \
		} else { \
			ZEND_VM_SET_OPCODE(OP_JMP_ADDR(opline + 1, (opline+1)->op2)); \
		} \
		ZEND_VM_CONTINUE(); \
	} while (0)
#else
# define ZEND_VM_REPEATABLE_OPCODE
# define ZEND_VM_REPEAT_OPCODE(_opcode)
# define ZEND_VM_SMART_BRANCH(_result, _check)
# define ZEND_VM_SMART_BRANCH_JMPZ(_result, _check)
# define ZEND_VM_SMART_BRANCH_JMPNZ(_result, _check)
#endif

#ifdef __GNUC__
# define ZEND_VM_GUARD(name) __asm__("#" #name)
#else
# define ZEND_VM_GUARD(name)
#endif

#define GET_OP1_UNDEF_CV(ptr, type) \
	_get_zval_cv_lookup_ ## type(ptr, opline->op1.var EXECUTE_DATA_CC)
#define GET_OP2_UNDEF_CV(ptr, type) \
	_get_zval_cv_lookup_ ## type(ptr, opline->op2.var EXECUTE_DATA_CC)

#define UNDEF_RESULT() do { \
		if (opline->result_type & (IS_VAR | IS_TMP_VAR)) { \
			ZVAL_UNDEF(EX_VAR(opline->result.var)); \
		} \
	} while (0)

#if ZEND_GCC_VERSION >= 4000 && !defined(__clang__)
# pragma GCC push_options
# pragma GCC optimize("no-gcse")
# pragma GCC optimize("no-ivopts")
#endif

#include "zend_vm_execute.h"

#if ZEND_GCC_VERSION >= 4000 && !defined(__clang__)
# pragma GCC pop_options
#endif

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

ZEND_API zval *zend_get_zval_ptr(const zend_op *opline, int op_type, const znode_op *node, const zend_execute_data *execute_data, zend_free_op *should_free, int type)
{
	zval *ret;

	switch (op_type) {
		case IS_CONST:
			ret = RT_CONSTANT(opline, *node);
			*should_free = NULL;
			break;
		case IS_TMP_VAR:
		case IS_VAR:
			ret = EX_VAR(node->var);
			*should_free = ret;
			break;
		case IS_CV:
			ret = EX_VAR(node->var);
			*should_free = NULL;
			break;
		default:
			ret = NULL;
			*should_free = ret;
			break;
	}
	return ret;
}

ZEND_API void ZEND_FASTCALL zend_check_internal_arg_type(zend_function *zf, uint32_t arg_num, zval *arg)
{
	void *dummy_cache_slot = NULL;

	zend_verify_arg_type(zf, arg_num, arg, NULL, &dummy_cache_slot);
}

ZEND_API int ZEND_FASTCALL zend_check_arg_type(zend_function *zf, uint32_t arg_num, zval *arg, zval *default_value, void **cache_slot)
{
	return zend_verify_arg_type(zf, arg_num, arg, default_value, cache_slot);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
