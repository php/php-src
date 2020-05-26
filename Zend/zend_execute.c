/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) Zend Technologies Ltd. (http://www.zend.com)           |
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
# elif defined(__GNUC__) && ZEND_GCC_VERSION >= 4008 && defined(__aarch64__)
#  define ZEND_VM_FP_GLOBAL_REG "x27"
#  define ZEND_VM_IP_GLOBAL_REG "x28"
# endif
#endif

#if defined(ZEND_VM_FP_GLOBAL_REG) && ((ZEND_VM_KIND == ZEND_VM_KIND_CALL) || (ZEND_VM_KIND == ZEND_VM_KIND_HYBRID))
# pragma GCC diagnostic ignored "-Wvolatile-register-var"
  register zend_execute_data* volatile execute_data __asm__(ZEND_VM_FP_GLOBAL_REG);
# pragma GCC diagnostic warning "-Wvolatile-register-var"
#endif

#if defined(ZEND_VM_FP_GLOBAL_REG) && ((ZEND_VM_KIND == ZEND_VM_KIND_CALL) || (ZEND_VM_KIND == ZEND_VM_KIND_HYBRID))
# define EXECUTE_DATA_D     void
# define EXECUTE_DATA_C
# define EXECUTE_DATA_DC
# define EXECUTE_DATA_CC
# define NO_EXECUTE_DATA_CC
#else
# define EXECUTE_DATA_D     zend_execute_data* execute_data
# define EXECUTE_DATA_C     execute_data
# define EXECUTE_DATA_DC    , EXECUTE_DATA_D
# define EXECUTE_DATA_CC    , EXECUTE_DATA_C
# define NO_EXECUTE_DATA_CC , NULL
#endif

#if defined(ZEND_VM_FP_GLOBAL_REG) && ((ZEND_VM_KIND == ZEND_VM_KIND_CALL) || (ZEND_VM_KIND == ZEND_VM_KIND_HYBRID))
# define OPLINE_D           void
# define OPLINE_C
# define OPLINE_DC
# define OPLINE_CC
#else
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

static zend_never_inline ZEND_COLD zval* zval_undefined_cv(uint32_t var EXECUTE_DATA_DC)
{
	if (EXPECTED(EG(exception) == NULL)) {
		zend_string *cv = CV_DEF_OF(EX_VAR_TO_NUM(var));
		zend_error(E_NOTICE, "Undefined variable: %s", ZSTR_VAL(cv));
	}
	return &EG(uninitialized_zval);
}

static zend_never_inline ZEND_COLD zval* ZEND_FASTCALL _zval_undefined_op1(EXECUTE_DATA_D)
{
	return zval_undefined_cv(EX(opline)->op1.var EXECUTE_DATA_CC);
}

static zend_never_inline ZEND_COLD zval* ZEND_FASTCALL _zval_undefined_op2(EXECUTE_DATA_D)
{
	return zval_undefined_cv(EX(opline)->op2.var EXECUTE_DATA_CC);
}

#define ZVAL_UNDEFINED_OP1() _zval_undefined_op1(EXECUTE_DATA_C)
#define ZVAL_UNDEFINED_OP2() _zval_undefined_op2(EXECUTE_DATA_C)

static zend_never_inline ZEND_COLD zval *_get_zval_cv_lookup(zval *ptr, uint32_t var, int type EXECUTE_DATA_DC)
{
	switch (type) {
		case BP_VAR_R:
		case BP_VAR_UNSET:
			ptr = zval_undefined_cv(var EXECUTE_DATA_CC);
			break;
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
		return zval_undefined_cv(var EXECUTE_DATA_CC);
	}
	return ret;
}

static zend_always_inline zval *_get_zval_ptr_cv_deref_BP_VAR_R(uint32_t var EXECUTE_DATA_DC)
{
	zval *ret = EX_VAR(var);

	if (UNEXPECTED(Z_TYPE_P(ret) == IS_UNDEF)) {
		return zval_undefined_cv(var EXECUTE_DATA_CC);
	}
	ZVAL_DEREF(ret);
	return ret;
}

static zend_always_inline zval *_get_zval_ptr_cv_BP_VAR_IS(uint32_t var EXECUTE_DATA_DC)
{
	zval *ret = EX_VAR(var);

	return ret;
}

static zend_always_inline zval *_get_zval_ptr_cv_BP_VAR_RW(uint32_t var EXECUTE_DATA_DC)
{
	zval *ret = EX_VAR(var);

	if (UNEXPECTED(Z_TYPE_P(ret) == IS_UNDEF)) {
		ZVAL_NULL(ret);
		zval_undefined_cv(var EXECUTE_DATA_CC);
		return ret;
	}
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

static zend_always_inline zval *_get_zval_ptr(int op_type, znode_op node, zend_free_op *should_free, int type EXECUTE_DATA_DC OPLINE_DC)
{
	if (op_type & (IS_TMP_VAR|IS_VAR)) {
		if (!ZEND_DEBUG || op_type == IS_VAR) {
			return _get_zval_ptr_var(node.var, should_free EXECUTE_DATA_CC);
		} else {
			ZEND_ASSERT(op_type == IS_TMP_VAR);
			return _get_zval_ptr_tmp(node.var, should_free EXECUTE_DATA_CC);
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
		if (!ZEND_DEBUG || op_type == IS_VAR) {
			return _get_zval_ptr_var(node.var, should_free EXECUTE_DATA_CC);
		} else {
			ZEND_ASSERT(op_type == IS_TMP_VAR);
			return _get_zval_ptr_tmp(node.var, should_free EXECUTE_DATA_CC);
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

static zend_always_inline ZEND_ATTRIBUTE_UNUSED zval *_get_zval_ptr_deref(int op_type, znode_op node, zend_free_op *should_free, int type EXECUTE_DATA_DC OPLINE_DC)
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

static zend_always_inline ZEND_ATTRIBUTE_UNUSED zval *_get_op_data_zval_ptr_deref_r(int op_type, znode_op node, zend_free_op *should_free EXECUTE_DATA_DC OPLINE_DC)
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
		if (!ZEND_DEBUG || op_type == IS_VAR) {
			return _get_zval_ptr_var(node.var, should_free EXECUTE_DATA_CC);
		} else {
			ZEND_ASSERT(op_type == IS_TMP_VAR);
			return _get_zval_ptr_tmp(node.var, should_free EXECUTE_DATA_CC);
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

static inline ZEND_ATTRIBUTE_UNUSED zval *_get_obj_zval_ptr(int op_type, znode_op op, zend_free_op *should_free, int type EXECUTE_DATA_DC OPLINE_DC)
{
	if (op_type == IS_UNUSED) {
		*should_free = NULL;
		return &EX(This);
	}
	return get_zval_ptr(op_type, op, should_free, type);
}

static inline ZEND_ATTRIBUTE_UNUSED zval *_get_obj_zval_ptr_undef(int op_type, znode_op op, zend_free_op *should_free, int type EXECUTE_DATA_DC OPLINE_DC)
{
	if (op_type == IS_UNUSED) {
		*should_free = NULL;
		return &EX(This);
	}
	return get_zval_ptr_undef(op_type, op, should_free, type);
}

static inline ZEND_ATTRIBUTE_UNUSED zval *_get_obj_zval_ptr_ptr(int op_type, znode_op node, zend_free_op *should_free, int type EXECUTE_DATA_DC)
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

static zend_never_inline zval* zend_assign_to_typed_property_reference(zend_property_info *prop_info, zval *prop, zval *value_ptr EXECUTE_DATA_DC)
{
	if (!zend_verify_prop_assignable_by_ref(prop_info, value_ptr, EX_USES_STRICT_TYPES())) {
		return &EG(uninitialized_zval);
	}
	if (Z_ISREF_P(prop)) {
		ZEND_REF_DEL_TYPE_SOURCE(Z_REF_P(prop), prop_info);
	}
	zend_assign_to_variable_reference(prop, value_ptr);
	ZEND_REF_ADD_TYPE_SOURCE(Z_REF_P(prop), prop_info);
	return prop;
}

static zend_never_inline ZEND_COLD int zend_wrong_assign_to_variable_reference(zval *variable_ptr, zval *value_ptr OPLINE_DC EXECUTE_DATA_DC)
{
	zend_error(E_NOTICE, "Only variables should be assigned by reference");
	if (UNEXPECTED(EG(exception) != NULL)) {
		return 0;
	}

	/* Use IS_TMP_VAR instead of IS_VAR to avoid ISREF check */
	Z_TRY_ADDREF_P(value_ptr);
	value_ptr = zend_assign_to_variable(variable_ptr, value_ptr, IS_TMP_VAR, EX_USES_STRICT_TYPES());

	return 1;
}

static void zend_format_type(zend_type type, const char **part1, const char **part2) {
	*part1 = ZEND_TYPE_ALLOW_NULL(type) ? "?" : "";
	if (ZEND_TYPE_IS_CLASS(type)) {
		if (ZEND_TYPE_IS_CE(type)) {
			*part2 = ZSTR_VAL(ZEND_TYPE_CE(type)->name);
		} else {
			*part2 = ZSTR_VAL(ZEND_TYPE_NAME(type));
		}
	} else {
		*part2 = zend_get_type_by_const(ZEND_TYPE_CODE(type));
	}
}

static zend_never_inline ZEND_COLD void zend_throw_auto_init_in_prop_error(zend_property_info *prop, const char *type) {
	const char *prop_type1, *prop_type2;
	zend_format_type(prop->type, &prop_type1, &prop_type2);
	zend_type_error(
		"Cannot auto-initialize an %s inside property %s::$%s of type %s%s",
		type,
		ZSTR_VAL(prop->ce->name), zend_get_unmangled_property_name(prop->name),
		prop_type1, prop_type2
	);
}

static zend_never_inline ZEND_COLD void zend_throw_auto_init_in_ref_error(zend_property_info *prop, const char *type) {
	const char *prop_type1, *prop_type2;
	zend_format_type(prop->type, &prop_type1, &prop_type2);
	zend_type_error(
		"Cannot auto-initialize an %s inside a reference held by property %s::$%s of type %s%s",
		type,
		ZSTR_VAL(prop->ce->name), zend_get_unmangled_property_name(prop->name),
		prop_type1, prop_type2
	);
}

static zend_never_inline ZEND_COLD void zend_throw_access_uninit_prop_by_ref_error(
		zend_property_info *prop) {
	zend_throw_error(NULL,
		"Cannot access uninitialized non-nullable property %s::$%s by reference",
		ZSTR_VAL(prop->ce->name),
		zend_get_unmangled_property_name(prop->name));
}

static zend_never_inline zend_bool zend_verify_ref_stdClass_assignable(zend_reference *ref);
static zend_never_inline zend_bool zend_verify_ref_array_assignable(zend_reference *ref);

/* this should modify object only if it's empty */
static zend_never_inline ZEND_COLD zval* ZEND_FASTCALL make_real_object(zval *object, zval *property OPLINE_DC EXECUTE_DATA_DC)
{
	zend_object *obj;
	zval *ref = NULL;
	if (Z_ISREF_P(object)) {
		ref = object;
		object = Z_REFVAL_P(object);
	}

	if (UNEXPECTED(Z_TYPE_P(object) > IS_FALSE &&
			(Z_TYPE_P(object) != IS_STRING || Z_STRLEN_P(object) != 0))) {
		if (opline->op1_type != IS_VAR || EXPECTED(!Z_ISERROR_P(object))) {
			zend_string *tmp_property_name;
			zend_string *property_name = zval_get_tmp_string(property, &tmp_property_name);

			if (opline->opcode == ZEND_PRE_INC_OBJ
			 || opline->opcode == ZEND_PRE_DEC_OBJ
			 || opline->opcode == ZEND_POST_INC_OBJ
			 || opline->opcode == ZEND_POST_DEC_OBJ) {
				zend_error(E_WARNING, "Attempt to increment/decrement property '%s' of non-object", ZSTR_VAL(property_name));
			} else if (opline->opcode == ZEND_FETCH_OBJ_W
					|| opline->opcode == ZEND_FETCH_OBJ_RW
					|| opline->opcode == ZEND_FETCH_OBJ_FUNC_ARG
					|| opline->opcode == ZEND_ASSIGN_OBJ_REF) {
				zend_error(E_WARNING, "Attempt to modify property '%s' of non-object", ZSTR_VAL(property_name));
			} else {
				zend_error(E_WARNING, "Attempt to assign property '%s' of non-object", ZSTR_VAL(property_name));
			}
			zend_tmp_string_release(tmp_property_name);
		}
		if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
			ZVAL_NULL(EX_VAR(opline->result.var));
		}
		return NULL;
	}

	if (ref && ZEND_REF_HAS_TYPE_SOURCES(Z_REF_P(ref))) {
		if (UNEXPECTED(!zend_verify_ref_stdClass_assignable(Z_REF_P(ref)))) {
			if (RETURN_VALUE_USED(opline)) {
				ZVAL_UNDEF(EX_VAR(opline->result.var));
			}
			return NULL;
		}
	}

	zval_ptr_dtor_nogc(object);
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
		return NULL;
	}
	Z_DELREF_P(object);
	return object;
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

	if (EG(exception)) {
		/* The type verification itself might have already thrown an exception
		 * through a promoted warning. */
		return;
	}

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
			return zend_parse_arg_str_weak(arg, &dest);
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

ZEND_COLD zend_never_inline void zend_verify_property_type_error(zend_property_info *info, zval *property)
{
	const char *prop_type1, *prop_type2;

	/* we _may_ land here in case reading already errored and runtime cache thus has not been updated (i.e. it contains a valid but unrelated info) */
	if (EG(exception)) {
		return;
	}

	// TODO Switch to a more standard error message?
	zend_format_type(info->type, &prop_type1, &prop_type2);
	(void) prop_type1;
	if (ZEND_TYPE_IS_CLASS(info->type)) {
		zend_type_error("Typed property %s::$%s must be an instance of %s%s, %s used",
			ZSTR_VAL(info->ce->name),
			zend_get_unmangled_property_name(info->name),
			prop_type2,
			ZEND_TYPE_ALLOW_NULL(info->type) ? " or null" : "",
			Z_TYPE_P(property) == IS_OBJECT ? ZSTR_VAL(Z_OBJCE_P(property)->name) : zend_get_type_by_const(Z_TYPE_P(property)));
	} else {
		zend_type_error("Typed property %s::$%s must be %s%s, %s used",
			ZSTR_VAL(info->ce->name),
			zend_get_unmangled_property_name(info->name),
			prop_type2,
			ZEND_TYPE_ALLOW_NULL(info->type) ? " or null" : "",
			Z_TYPE_P(property) == IS_OBJECT ? ZSTR_VAL(Z_OBJCE_P(property)->name) : zend_get_type_by_const(Z_TYPE_P(property)));
	}
}

static zend_bool zend_resolve_class_type(zend_type *type, zend_class_entry *self_ce) {
	zend_class_entry *ce;
	zend_string *name = ZEND_TYPE_NAME(*type);
	if (zend_string_equals_literal_ci(name, "self")) {
		/* We need to explicitly check for this here, to avoid updating the type in the trait and
		 * later using the wrong "self" when the trait is used in a class. */
		if (UNEXPECTED((self_ce->ce_flags & ZEND_ACC_TRAIT) != 0)) {
			zend_throw_error(NULL, "Cannot write a%s value to a 'self' typed static property of a trait", ZEND_TYPE_ALLOW_NULL(*type) ? " non-null" : "");
			return 0;
		}
		ce = self_ce;
	} else if (zend_string_equals_literal_ci(name, "parent")) {
		if (UNEXPECTED(!self_ce->parent)) {
			zend_throw_error(NULL, "Cannot access parent:: when current class scope has no parent");
			return 0;
		}
		ce = self_ce->parent;
	} else {
		ce = zend_lookup_class_ex(name, NULL, ZEND_FETCH_CLASS_NO_AUTOLOAD);
		if (UNEXPECTED(!ce)) {
			return 0;
		}
	}

	zend_string_release(name);
	*type = ZEND_TYPE_ENCODE_CE(ce, ZEND_TYPE_ALLOW_NULL(*type));
	return 1;
}


static zend_always_inline zend_bool i_zend_check_property_type(zend_property_info *info, zval *property, zend_bool strict)
{
	ZEND_ASSERT(!Z_ISREF_P(property));
	if (ZEND_TYPE_IS_CLASS(info->type)) {
		if (UNEXPECTED(Z_TYPE_P(property) != IS_OBJECT)) {
			return Z_TYPE_P(property) == IS_NULL && ZEND_TYPE_ALLOW_NULL(info->type);
		}

		if (UNEXPECTED(!ZEND_TYPE_IS_CE(info->type)) && UNEXPECTED(!zend_resolve_class_type(&info->type, info->ce))) {
			return 0;
		}

		return instanceof_function(Z_OBJCE_P(property), ZEND_TYPE_CE(info->type));
	}

	ZEND_ASSERT(ZEND_TYPE_CODE(info->type) != IS_CALLABLE);
	if (EXPECTED(ZEND_TYPE_CODE(info->type) == Z_TYPE_P(property))) {
		return 1;
	} else if (EXPECTED(Z_TYPE_P(property) == IS_NULL)) {
		return ZEND_TYPE_ALLOW_NULL(info->type);
	} else if (ZEND_TYPE_CODE(info->type) == _IS_BOOL && EXPECTED(Z_TYPE_P(property) == IS_FALSE || Z_TYPE_P(property) == IS_TRUE)) {
		return 1;
	} else if (ZEND_TYPE_CODE(info->type) == IS_ITERABLE) {
		return zend_is_iterable(property);
	} else {
		return zend_verify_scalar_type_hint(ZEND_TYPE_CODE(info->type), property, strict);
	}
}

static zend_bool zend_always_inline i_zend_verify_property_type(zend_property_info *info, zval *property, zend_bool strict)
{
	if (i_zend_check_property_type(info, property, strict)) {
		return 1;
	}

	zend_verify_property_type_error(info, property);
	return 0;
}

zend_bool zend_never_inline zend_verify_property_type(zend_property_info *info, zval *property, zend_bool strict) {
	return i_zend_verify_property_type(info, property, strict);
}

static zend_never_inline zval* zend_assign_to_typed_prop(zend_property_info *info, zval *property_val, zval *value EXECUTE_DATA_DC)
{
	zval tmp;

	ZVAL_DEREF(value);
	ZVAL_COPY(&tmp, value);

	if (UNEXPECTED(!i_zend_verify_property_type(info, &tmp, EX_USES_STRICT_TYPES()))) {
		zval_ptr_dtor(&tmp);
		return &EG(uninitialized_zval);
	}

	return zend_assign_to_variable(property_val, &tmp, IS_TMP_VAR, EX_USES_STRICT_TYPES());
}


static zend_always_inline zend_bool zend_check_type(
		zend_type type,
		zval *arg, zend_class_entry **ce, void **cache_slot,
		zval *default_value, zend_class_entry *scope,
		zend_bool is_return_type)
{
	zend_reference *ref = NULL;

	if (!ZEND_TYPE_IS_SET(type)) {
		return 1;
	}

	if (UNEXPECTED(Z_ISREF_P(arg))) {
		ref = Z_REF_P(arg);
		arg = Z_REFVAL_P(arg);
	}

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
	} else if (ref && ZEND_REF_HAS_TYPE_SOURCES(ref)) {
		return 0; /* we cannot have conversions for typed refs */
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

static zend_always_inline int zend_verify_recv_arg_type(zend_function *zf, uint32_t arg_num, zval *arg, zval *default_value, void **cache_slot)
{
	zend_arg_info *cur_arg_info = &zf->common.arg_info[arg_num-1];
	zend_class_entry *ce;

	ZEND_ASSERT(arg_num <= zf->common.num_args);
	cur_arg_info = &zf->common.arg_info[arg_num-1];

	ce = NULL;
	if (UNEXPECTED(!zend_check_type(cur_arg_info->type, arg, &ce, cache_slot, default_value, zf->common.scope, 0))) {
		zend_verify_arg_error(zf, cur_arg_info, arg_num, ce, arg);
		return 0;
	}

	return 1;
}

static zend_always_inline int zend_verify_variadic_arg_type(zend_function *zf, uint32_t arg_num, zval *arg, zval *default_value, void **cache_slot)
{
	zend_arg_info *cur_arg_info;
	zend_class_entry *ce;

	ZEND_ASSERT(arg_num > zf->common.num_args);
	ZEND_ASSERT(zf->common.fn_flags & ZEND_ACC_VARIADIC);
	cur_arg_info = &zf->common.arg_info[zf->common.num_args];

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
	Z_OBJ_HT_P(object)->write_dimension(object, dim, value);

	if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
		ZVAL_COPY(EX_VAR(opline->result.var), value);
	}
}

static zend_always_inline int zend_binary_op(zval *ret, zval *op1, zval *op2 OPLINE_DC)
{
	static const binary_op_type zend_binary_ops[] = {
		add_function,
		sub_function,
		mul_function,
		div_function,
		mod_function,
		shift_left_function,
		shift_right_function,
		concat_function,
		bitwise_or_function,
		bitwise_and_function,
		bitwise_xor_function,
		pow_function
	};
	/* size_t cast makes GCC to better optimize 64-bit PIC code */
	size_t opcode = (size_t)opline->extended_value;

	return zend_binary_ops[opcode - ZEND_ADD](ret, op1, op2);
}

static zend_never_inline void zend_binary_assign_op_obj_dim(zval *object, zval *property OPLINE_DC EXECUTE_DATA_DC)
{
	zend_free_op free_op_data1;
	zval *value;
	zval *z;
	zval rv, res;

	value = get_op_data_zval_ptr_r((opline+1)->op1_type, (opline+1)->op1, &free_op_data1);
	if ((z = Z_OBJ_HT_P(object)->read_dimension(object, property, BP_VAR_R, &rv)) != NULL) {

		if (Z_TYPE_P(z) == IS_OBJECT && Z_OBJ_HT_P(z)->get) {
			zval rv2;
			zval *value = Z_OBJ_HT_P(z)->get(z, &rv2);

			if (z == &rv) {
				zval_ptr_dtor(&rv);
			}
			ZVAL_COPY_VALUE(z, value);
		}
		if (zend_binary_op(&res, z, value OPLINE_CC) == SUCCESS) {
			Z_OBJ_HT_P(object)->write_dimension(object, property, &res);
		}
		if (z == &rv) {
			zval_ptr_dtor(&rv);
		}
		if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
			ZVAL_COPY(EX_VAR(opline->result.var), &res);
		}
		zval_ptr_dtor(&res);
	} else {
		zend_use_object_as_array();
		if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
			ZVAL_NULL(EX_VAR(opline->result.var));
		}
	}
	FREE_OP(free_op_data1);
}

static zend_never_inline void zend_binary_assign_op_typed_ref(zend_reference *ref, zval *value OPLINE_DC EXECUTE_DATA_DC)
{
	zval z_copy;

	zend_binary_op(&z_copy, &ref->val, value OPLINE_CC);
	if (EXPECTED(zend_verify_ref_assignable_zval(ref, &z_copy, EX_USES_STRICT_TYPES()))) {
		zval_ptr_dtor(&ref->val);
		ZVAL_COPY_VALUE(&ref->val, &z_copy);
	} else {
		zval_ptr_dtor(&z_copy);
	}
}

static zend_never_inline void zend_binary_assign_op_typed_prop(zend_property_info *prop_info, zval *zptr, zval *value OPLINE_DC EXECUTE_DATA_DC)
{
	zval z_copy;

	zend_binary_op(&z_copy, zptr, value OPLINE_CC);
	if (EXPECTED(zend_verify_property_type(prop_info, &z_copy, EX_USES_STRICT_TYPES()))) {
		zval_ptr_dtor(zptr);
		ZVAL_COPY_VALUE(zptr, &z_copy);
	} else {
		zval_ptr_dtor(&z_copy);
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
				ZVAL_UNDEFINED_OP2();
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
		case ZEND_ASSIGN_OP:
		case ZEND_ASSIGN_DIM_OP:
		case ZEND_ASSIGN_OBJ_OP:
		case ZEND_ASSIGN_STATIC_PROP_OP:
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
						case ZEND_ASSIGN_OBJ_OP:
							msg = "Cannot use string offset as an object";
							break;
						case ZEND_ASSIGN_DIM_OP:
							msg = "Cannot use string offset as an array";
							break;
						case ZEND_ASSIGN_STATIC_PROP_OP:
						case ZEND_ASSIGN_OP:
							msg = "Cannot use assign-op operators with string offsets";
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

static zend_never_inline ZEND_COLD void ZEND_FASTCALL zend_deprecated_function(const zend_function *fbc)
{
	zend_error(E_DEPRECATED, "Function %s%s%s() is deprecated",
		fbc->common.scope ? ZSTR_VAL(fbc->common.scope->name) : "",
		fbc->common.scope ? "::" : "",
		ZSTR_VAL(fbc->common.function_name));
}

static zend_never_inline ZEND_COLD void ZEND_FASTCALL zend_abstract_method(const zend_function *fbc)
{
	zend_throw_error(NULL, "Cannot call abstract method %s::%s()",
		ZSTR_VAL(fbc->common.scope->name), ZSTR_VAL(fbc->common.function_name));
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
		zend_string *tmp = zval_try_get_string_func(value);
		if (UNEXPECTED(!tmp)) {
			if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
				ZVAL_UNDEF(EX_VAR(opline->result.var));
			}
			return;
		}

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

static zend_property_info *zend_get_prop_not_accepting_double(zend_reference *ref)
{
	zend_property_info *prop;
	ZEND_REF_FOREACH_TYPE_SOURCES(ref, prop) {
		if (ZEND_TYPE_CODE(prop->type) != IS_DOUBLE) {
			return prop;
		}
	} ZEND_REF_FOREACH_TYPE_SOURCES_END();
	return NULL;
}

static ZEND_COLD zend_long zend_throw_incdec_ref_error(zend_reference *ref OPLINE_DC)
{
	zend_property_info *error_prop = zend_get_prop_not_accepting_double(ref);
	/* Currently there should be no way for a typed reference to accept both int and double.
	 * Generalize this and the related property code once this becomes possible. */
	ZEND_ASSERT(error_prop);
	if (ZEND_IS_INCREMENT(opline->opcode)) {
		zend_type_error(
			"Cannot increment a reference held by property %s::$%s of type %sint past its maximal value",
			ZSTR_VAL(error_prop->ce->name),
			zend_get_unmangled_property_name(error_prop->name),
			ZEND_TYPE_ALLOW_NULL(error_prop->type) ? "?" : "");
		return ZEND_LONG_MAX;
	} else {
		zend_type_error(
			"Cannot decrement a reference held by property %s::$%s of type %sint past its minimal value",
			ZSTR_VAL(error_prop->ce->name),
			zend_get_unmangled_property_name(error_prop->name),
			ZEND_TYPE_ALLOW_NULL(error_prop->type) ? "?" : "");
		return ZEND_LONG_MIN;
	}
}

static ZEND_COLD zend_long zend_throw_incdec_prop_error(zend_property_info *prop OPLINE_DC) {
	const char *prop_type1, *prop_type2;
	zend_format_type(prop->type, &prop_type1, &prop_type2);
	if (ZEND_IS_INCREMENT(opline->opcode)) {
		zend_type_error("Cannot increment property %s::$%s of type %s%s past its maximal value",
			ZSTR_VAL(prop->ce->name),
			zend_get_unmangled_property_name(prop->name),
			prop_type1, prop_type2);
		return ZEND_LONG_MAX;
	} else {
		zend_type_error("Cannot decrement property %s::$%s of type %s%s past its minimal value",
			ZSTR_VAL(prop->ce->name),
			zend_get_unmangled_property_name(prop->name),
			prop_type1, prop_type2);
		return ZEND_LONG_MIN;
	}
}

static void zend_incdec_typed_ref(zend_reference *ref, zval *copy OPLINE_DC EXECUTE_DATA_DC)
{
	zval tmp;
	zval *var_ptr = &ref->val;

	if (!copy) {
		copy = &tmp;
	}

	ZVAL_COPY(copy, var_ptr);

	if (ZEND_IS_INCREMENT(opline->opcode)) {
		increment_function(var_ptr);
	} else {
		decrement_function(var_ptr);
	}

	if (UNEXPECTED(Z_TYPE_P(var_ptr) == IS_DOUBLE) && Z_TYPE_P(copy) == IS_LONG) {
		zend_long val = zend_throw_incdec_ref_error(ref OPLINE_CC);
		ZVAL_LONG(var_ptr, val);
	} else if (UNEXPECTED(!zend_verify_ref_assignable_zval(ref, var_ptr, EX_USES_STRICT_TYPES()))) {
		zval_ptr_dtor(var_ptr);
		ZVAL_COPY_VALUE(var_ptr, copy);
		ZVAL_UNDEF(copy);
	} else if (copy == &tmp) {
		zval_ptr_dtor(&tmp);
	}
}

static void zend_incdec_typed_prop(zend_property_info *prop_info, zval *var_ptr, zval *copy OPLINE_DC EXECUTE_DATA_DC)
{
	zval tmp;

	if (!copy) {
		copy = &tmp;
	}

	ZVAL_COPY(copy, var_ptr);

	if (ZEND_IS_INCREMENT(opline->opcode)) {
		increment_function(var_ptr);
	} else {
		decrement_function(var_ptr);
	}

	if (UNEXPECTED(Z_TYPE_P(var_ptr) == IS_DOUBLE) && Z_TYPE_P(copy) == IS_LONG) {
		zend_long val = zend_throw_incdec_prop_error(prop_info OPLINE_CC);
		ZVAL_LONG(var_ptr, val);
	} else if (UNEXPECTED(!zend_verify_property_type(prop_info, var_ptr, EX_USES_STRICT_TYPES()))) {
		zval_ptr_dtor(var_ptr);
		ZVAL_COPY_VALUE(var_ptr, copy);
		ZVAL_UNDEF(copy);
	} else if (copy == &tmp) {
		zval_ptr_dtor(&tmp);
	}
}

static void zend_pre_incdec_property_zval(zval *prop, zend_property_info *prop_info OPLINE_DC EXECUTE_DATA_DC)
{
	if (EXPECTED(Z_TYPE_P(prop) == IS_LONG)) {
		if (ZEND_IS_INCREMENT(opline->opcode)) {
			fast_long_increment_function(prop);
		} else {
			fast_long_decrement_function(prop);
		}
		if (UNEXPECTED(Z_TYPE_P(prop) != IS_LONG) && UNEXPECTED(prop_info)) {
			zend_long val = zend_throw_incdec_prop_error(prop_info OPLINE_CC);
			ZVAL_LONG(prop, val);
		}
	} else {
		do {
			if (Z_ISREF_P(prop)) {
				zend_reference *ref = Z_REF_P(prop);
				prop = Z_REFVAL_P(prop);
				if (UNEXPECTED(ZEND_REF_HAS_TYPE_SOURCES(ref))) {
					zend_incdec_typed_ref(ref, NULL OPLINE_CC EXECUTE_DATA_CC);
					break;
				}
			}

			if (UNEXPECTED(prop_info)) {
				zend_incdec_typed_prop(prop_info, prop, NULL OPLINE_CC EXECUTE_DATA_CC);
			} else if (ZEND_IS_INCREMENT(opline->opcode)) {
				increment_function(prop);
			} else {
				decrement_function(prop);
			}
		} while (0);
	}
	if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
		ZVAL_COPY(EX_VAR(opline->result.var), prop);
	}
}

static void zend_post_incdec_property_zval(zval *prop, zend_property_info *prop_info OPLINE_DC EXECUTE_DATA_DC)
{
	if (EXPECTED(Z_TYPE_P(prop) == IS_LONG)) {
		ZVAL_LONG(EX_VAR(opline->result.var), Z_LVAL_P(prop));
		if (ZEND_IS_INCREMENT(opline->opcode)) {
			fast_long_increment_function(prop);
		} else {
			fast_long_decrement_function(prop);
		}
		if (UNEXPECTED(Z_TYPE_P(prop) != IS_LONG) && UNEXPECTED(prop_info)) {
			zend_long val = zend_throw_incdec_prop_error(prop_info OPLINE_CC);
			ZVAL_LONG(prop, val);
		}
	} else {
		if (Z_ISREF_P(prop)) {
			zend_reference *ref = Z_REF_P(prop);
			prop = Z_REFVAL_P(prop);
			if (ZEND_REF_HAS_TYPE_SOURCES(ref)) {
				zend_incdec_typed_ref(ref, EX_VAR(opline->result.var) OPLINE_CC EXECUTE_DATA_CC);
				return;
			}
		}

		if (UNEXPECTED(prop_info)) {
			zend_incdec_typed_prop(prop_info, prop, EX_VAR(opline->result.var) OPLINE_CC EXECUTE_DATA_CC);
		} else {
			ZVAL_COPY(EX_VAR(opline->result.var), prop);
			if (ZEND_IS_INCREMENT(opline->opcode)) {
				increment_function(prop);
			} else {
				decrement_function(prop);
			}
		}
	}
}

static zend_never_inline void zend_post_incdec_overloaded_property(zval *object, zval *property, void **cache_slot OPLINE_DC EXECUTE_DATA_DC)
{
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
	if (ZEND_IS_INCREMENT(opline->opcode)) {
		increment_function(&z_copy);
	} else {
		decrement_function(&z_copy);
	}
	Z_OBJ_HT(obj)->write_property(&obj, property, &z_copy, cache_slot);
	OBJ_RELEASE(Z_OBJ(obj));
	zval_ptr_dtor(&z_copy);
	zval_ptr_dtor(z);
}

static zend_never_inline void zend_pre_incdec_overloaded_property(zval *object, zval *property, void **cache_slot OPLINE_DC EXECUTE_DATA_DC)
{
	zval rv;
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
	if (ZEND_IS_INCREMENT(opline->opcode)) {
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
}

static zend_never_inline void zend_assign_op_overloaded_property(zval *object, zval *property, void **cache_slot, zval *value OPLINE_DC EXECUTE_DATA_DC)
{
	zval *z;
	zval rv, obj, res;

	ZVAL_OBJ(&obj, Z_OBJ_P(object));
	Z_ADDREF(obj);
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
	if (zend_binary_op(&res, z, value OPLINE_CC) == SUCCESS) {
		Z_OBJ_HT(obj)->write_property(&obj, property, &res, cache_slot);
	}
	if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
		ZVAL_COPY(EX_VAR(opline->result.var), &res);
	}
	zval_ptr_dtor(z);
	zval_ptr_dtor(&res);
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

static ZEND_COLD void zend_binary_assign_op_dim_slow(zval *container, zval *dim OPLINE_DC EXECUTE_DATA_DC)
{
	zend_free_op free_op_data1;

	if (UNEXPECTED(Z_TYPE_P(container) == IS_STRING)) {
		if (opline->op2_type == IS_UNUSED) {
			zend_use_new_element_for_string();
		} else {
			zend_check_string_offset(dim, BP_VAR_RW EXECUTE_DATA_CC);
			zend_wrong_string_offset(EXECUTE_DATA_C);
		}
	} else if (EXPECTED(!Z_ISERROR_P(container))) {
		zend_use_scalar_as_array();
	}
	get_op_data_zval_ptr_r((opline+1)->op1_type, (opline+1)->op1, &free_op_data1);
	FREE_OP(free_op_data1);
}

static zend_never_inline zend_uchar slow_index_convert(const zval *dim, zend_value *value EXECUTE_DATA_DC)
{
	switch (Z_TYPE_P(dim)) {
		case IS_UNDEF:
			ZVAL_UNDEFINED_OP2();
			/* break missing intentionally */
		case IS_NULL:
			value->str = ZSTR_EMPTY_ALLOC();
			return IS_STRING;
		case IS_DOUBLE:
			value->lval = zend_dval_to_lval(Z_DVAL_P(dim));
			return IS_LONG;
		case IS_RESOURCE:
			zend_use_resource_as_offset(dim);
			value->lval = Z_RES_HANDLE_P(dim);
			return IS_LONG;
		case IS_FALSE:
			value->lval = 0;
			return IS_LONG;
		case IS_TRUE:
			value->lval = 1;
			return IS_LONG;
		default:
			zend_illegal_offset();
			return IS_NULL;
	}
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
		if (ZEND_CONST_COND(dim_type != IS_CONST, 1)) {
			if (ZEND_HANDLE_NUMERIC(offset_key, hval)) {
				goto num_index;
			}
		}
str_index:
		retval = zend_hash_find_ex(ht, offset_key, ZEND_CONST_COND(dim_type == IS_CONST, 0));
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
	} else if (EXPECTED(Z_TYPE_P(dim) == IS_REFERENCE)) {
		dim = Z_REFVAL_P(dim);
		goto try_again;
	} else {
		zend_value val;
		zend_uchar t = slow_index_convert(dim, &val EXECUTE_DATA_CC);

		if (t == IS_STRING) {
			offset_key = val.str;
			goto str_index;
		} else if (t == IS_LONG) {
			hval = val.lval;
			goto num_index;
		} else {
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
		zend_reference *ref = Z_REF_P(container);
		container = Z_REFVAL_P(container);
		if (EXPECTED(Z_TYPE_P(container) == IS_ARRAY)) {
			goto try_array;
		} else if (EXPECTED(Z_TYPE_P(container) <= IS_FALSE)) {
			if (type != BP_VAR_UNSET) {
				if (ZEND_REF_HAS_TYPE_SOURCES(ref)) {
					if (UNEXPECTED(!zend_verify_ref_array_assignable(ref))) {
						ZVAL_ERROR(result);
						return;
					}
				}
				array_init(container);
				goto fetch_from_array;
			} else {
				goto return_null;
			}
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
		if (ZEND_CONST_COND(dim_type == IS_CV, dim != NULL) && UNEXPECTED(Z_TYPE_P(dim) == IS_UNDEF)) {
			dim = ZVAL_UNDEFINED_OP2();
		}
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
	} else {
		if (EXPECTED(Z_TYPE_P(container) <= IS_FALSE)) {
			if (type != BP_VAR_W && UNEXPECTED(Z_TYPE_P(container) == IS_UNDEF)) {
				ZVAL_UNDEFINED_OP1();
			}
			if (type != BP_VAR_UNSET) {
				array_init(container);
				goto fetch_from_array;
			} else {
return_null:
				/* for read-mode only */
				if (ZEND_CONST_COND(dim_type == IS_CV, dim != NULL) && UNEXPECTED(Z_TYPE_P(dim) == IS_UNDEF)) {
					ZVAL_UNDEFINED_OP2();
				}
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

static zend_always_inline void zend_fetch_dimension_address_read(zval *result, zval *container, zval *dim, int dim_type, int type, int is_list, int slow EXECUTE_DATA_DC)
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
	if (!is_list && EXPECTED(Z_TYPE_P(container) == IS_STRING)) {
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
					ZVAL_UNDEFINED_OP2();
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

		if (UNEXPECTED(Z_STRLEN_P(container) < ((offset < 0) ? -(size_t)offset : ((size_t)offset + 1)))) {
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
		if (ZEND_CONST_COND(dim_type == IS_CV, 1) && UNEXPECTED(Z_TYPE_P(dim) == IS_UNDEF)) {
			dim = ZVAL_UNDEFINED_OP2();
		}
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
	} else {
		if (type != BP_VAR_IS && UNEXPECTED(Z_TYPE_P(container) == IS_UNDEF)) {
			container = ZVAL_UNDEFINED_OP1();
		}
		if (ZEND_CONST_COND(dim_type == IS_CV, 1) && UNEXPECTED(Z_TYPE_P(dim) == IS_UNDEF)) {
			ZVAL_UNDEFINED_OP2();
		}
		if (!is_list && type != BP_VAR_IS) {
			zend_error(E_NOTICE, "Trying to access array offset on value of type %s",
				zend_zval_type_name(container));
		}
		ZVAL_NULL(result);
	}
}

static zend_never_inline void ZEND_FASTCALL zend_fetch_dimension_address_read_R(zval *container, zval *dim, int dim_type OPLINE_DC EXECUTE_DATA_DC)
{
	zval *result = EX_VAR(opline->result.var);
	zend_fetch_dimension_address_read(result, container, dim, dim_type, BP_VAR_R, 0, 0 EXECUTE_DATA_CC);
}

static zend_never_inline void zend_fetch_dimension_address_read_R_slow(zval *container, zval *dim OPLINE_DC EXECUTE_DATA_DC)
{
	zval *result = EX_VAR(opline->result.var);
	zend_fetch_dimension_address_read(result, container, dim, IS_CV, BP_VAR_R, 0, 1 EXECUTE_DATA_CC);
}

static zend_never_inline void ZEND_FASTCALL zend_fetch_dimension_address_read_IS(zval *container, zval *dim, int dim_type OPLINE_DC EXECUTE_DATA_DC)
{
	zval *result = EX_VAR(opline->result.var);
	zend_fetch_dimension_address_read(result, container, dim, dim_type, BP_VAR_IS, 0, 0 EXECUTE_DATA_CC);
}

static zend_never_inline void ZEND_FASTCALL zend_fetch_dimension_address_LIST_r(zval *container, zval *dim, int dim_type OPLINE_DC EXECUTE_DATA_DC)
{
	zval *result = EX_VAR(opline->result.var);
	zend_fetch_dimension_address_read(result, container, dim, dim_type, BP_VAR_R, 1, 0 EXECUTE_DATA_CC);
}

ZEND_API void zend_fetch_dimension_const(zval *result, zval *container, zval *dim, int type)
{
	zend_fetch_dimension_address_read(result, container, dim, IS_TMP_VAR, type, 0, 0 NO_EXECUTE_DATA_CC);
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
		ZVAL_UNDEFINED_OP2();
		goto str_idx;
	} else {
		zend_error(E_WARNING, "Illegal offset type in isset or empty");
		return NULL;
	}
}

static zend_never_inline int ZEND_FASTCALL zend_isset_dim_slow(zval *container, zval *offset EXECUTE_DATA_DC)
{
	if (/*OP2_TYPE == IS_CV &&*/ UNEXPECTED(Z_TYPE_P(offset) == IS_UNDEF)) {
		offset = ZVAL_UNDEFINED_OP2();
	}

	if (/*OP1_TYPE != IS_CONST &&*/ EXPECTED(Z_TYPE_P(container) == IS_OBJECT)) {
		return Z_OBJ_HT_P(container)->has_dimension(container, offset, 0);
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
		offset = ZVAL_UNDEFINED_OP2();
	}

	if (/*OP1_TYPE != IS_CONST &&*/ EXPECTED(Z_TYPE_P(container) == IS_OBJECT)) {
		return !Z_OBJ_HT_P(container)->has_dimension(container, offset, 1);
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

static zend_never_inline uint32_t ZEND_FASTCALL zend_array_key_exists_fast(HashTable *ht, zval *key OPLINE_DC EXECUTE_DATA_DC)
{
	zend_string *str;
	zend_ulong hval;

try_again:
	if (EXPECTED(Z_TYPE_P(key) == IS_STRING)) {
		str = Z_STR_P(key);
		if (ZEND_HANDLE_NUMERIC(str, hval)) {
			goto num_key;
		}
str_key:
		return zend_hash_find_ind(ht, str) != NULL ? IS_TRUE : IS_FALSE;
	} else if (EXPECTED(Z_TYPE_P(key) == IS_LONG)) {
		hval = Z_LVAL_P(key);
num_key:
		return zend_hash_index_find(ht, hval) != NULL ? IS_TRUE : IS_FALSE;
	} else if (EXPECTED(Z_ISREF_P(key))) {
		key = Z_REFVAL_P(key);
		goto try_again;
	} else if (Z_TYPE_P(key) <= IS_NULL) {
		if (UNEXPECTED(Z_TYPE_P(key) == IS_UNDEF)) {
			ZVAL_UNDEFINED_OP1();
		}
		str = ZSTR_EMPTY_ALLOC();
		goto str_key;
	} else {
		zend_error(E_WARNING, "array_key_exists(): The first argument should be either a string or an integer");
		return IS_FALSE;
	}
}

static zend_never_inline uint32_t ZEND_FASTCALL zend_array_key_exists_slow(zval *subject, zval *key OPLINE_DC EXECUTE_DATA_DC)
{
	if (EXPECTED(Z_TYPE_P(subject) == IS_OBJECT)) {
		zend_error(E_DEPRECATED, "array_key_exists(): "
			"Using array_key_exists() on objects is deprecated. "
			"Use isset() or property_exists() instead");

		HashTable *ht = zend_get_properties_for(subject, ZEND_PROP_PURPOSE_ARRAY_CAST);
		uint32_t result = zend_array_key_exists_fast(ht, key OPLINE_CC EXECUTE_DATA_CC);
		zend_release_properties(ht);
		return result;
	} else {
		if (UNEXPECTED(Z_TYPE_P(key) == IS_UNDEF)) {
			ZVAL_UNDEFINED_OP1();
		}
		if (UNEXPECTED(Z_TYPE_INFO_P(subject) == IS_UNDEF)) {
			ZVAL_UNDEFINED_OP2();
		}
		zend_internal_type_error(EX_USES_STRICT_TYPES(), "array_key_exists() expects parameter 2 to be array, %s given", zend_get_type_by_const(Z_TYPE_P(subject)));
		return IS_NULL;
	}
}

static zend_always_inline zend_bool promotes_to_array(zval *val) {
	return Z_TYPE_P(val) <= IS_FALSE
		|| (Z_ISREF_P(val) && Z_TYPE_P(Z_REFVAL_P(val)) <= IS_FALSE);
}

static zend_always_inline zend_bool promotes_to_object(zval *val) {
	ZVAL_DEREF(val);
	return Z_TYPE_P(val) <= IS_FALSE
		|| (Z_TYPE_P(val) == IS_STRING && Z_STRLEN_P(val) == 0);
}

static zend_always_inline zend_bool check_type_array_assignable(zend_type type) {
	if (!type) {
		return 1;
	}
	return ZEND_TYPE_IS_CODE(type)
		&& (ZEND_TYPE_CODE(type) == IS_ARRAY || ZEND_TYPE_CODE(type) == IS_ITERABLE);
}

static zend_always_inline zend_bool check_type_stdClass_assignable(zend_type type) {
	if (!type) {
		return 1;
	}
	if (ZEND_TYPE_IS_CLASS(type)) {
		if (ZEND_TYPE_IS_CE(type)) {
			return ZEND_TYPE_CE(type) == zend_standard_class_def;
		} else {
			return zend_string_equals_literal_ci(ZEND_TYPE_NAME(type), "stdclass");
		}
	} else {
		return ZEND_TYPE_CODE(type) == IS_OBJECT;
	}
}

/* Checks whether an array can be assigned to the reference. Returns conflicting property if
 * assignment is not possible, NULL otherwise. */
static zend_never_inline zend_bool zend_verify_ref_array_assignable(zend_reference *ref) {
	zend_property_info *prop;
	ZEND_ASSERT(ZEND_REF_HAS_TYPE_SOURCES(ref));
	ZEND_REF_FOREACH_TYPE_SOURCES(ref, prop) {
		if (!check_type_array_assignable(prop->type)) {
			zend_throw_auto_init_in_ref_error(prop, "array");
			return 0;
		}
	} ZEND_REF_FOREACH_TYPE_SOURCES_END();
	return 1;
}

/* Checks whether an stdClass can be assigned to the reference. Returns conflicting property if
 * assignment is not possible, NULL otherwise. */
static zend_never_inline zend_bool zend_verify_ref_stdClass_assignable(zend_reference *ref) {
	zend_property_info *prop;
	ZEND_ASSERT(ZEND_REF_HAS_TYPE_SOURCES(ref));
	ZEND_REF_FOREACH_TYPE_SOURCES(ref, prop) {
		if (!check_type_stdClass_assignable(prop->type)) {
			zend_throw_auto_init_in_ref_error(prop, "stdClass");
			return 0;
		}
	} ZEND_REF_FOREACH_TYPE_SOURCES_END();
	return 1;
}

static zend_property_info *zend_object_fetch_property_type_info(
		zend_object *obj, zval *slot)
{
	if (EXPECTED(!ZEND_CLASS_HAS_TYPE_HINTS(obj->ce))) {
		return NULL;
	}

	/* Not a declared property */
	if (UNEXPECTED(slot < obj->properties_table ||
			slot >= obj->properties_table + obj->ce->default_properties_count)) {
		return NULL;
	}

	return zend_get_typed_property_info_for_slot(obj, slot);
}

static zend_never_inline zend_bool zend_handle_fetch_obj_flags(
		zval *result, zval *ptr, zend_object *obj, zend_property_info *prop_info, uint32_t flags)
{
	switch (flags) {
		case ZEND_FETCH_DIM_WRITE:
			if (promotes_to_array(ptr)) {
				if (!prop_info) {
					prop_info = zend_object_fetch_property_type_info(obj, ptr);
					if (!prop_info) {
						break;
					}
				}
				if (!check_type_array_assignable(prop_info->type)) {
					zend_throw_auto_init_in_prop_error(prop_info, "array");
					if (result) ZVAL_ERROR(result);
					return 0;
				}
			}
			break;
		case ZEND_FETCH_OBJ_WRITE:
			if (promotes_to_object(ptr)) {
				if (!prop_info) {
					prop_info = zend_object_fetch_property_type_info(obj, ptr);
					if (!prop_info) {
						break;
					}
				}
				if (!check_type_stdClass_assignable(prop_info->type)) {
					zend_throw_auto_init_in_prop_error(prop_info, "stdClass");
					if (result) ZVAL_ERROR(result);
					return 0;
				}
	        }
			break;
		case ZEND_FETCH_REF:
			if (Z_TYPE_P(ptr) != IS_REFERENCE) {
				if (!prop_info) {
					prop_info = zend_object_fetch_property_type_info(obj, ptr);
					if (!prop_info) {
						break;
					}
				}
				if (Z_TYPE_P(ptr) == IS_UNDEF) {
					if (!ZEND_TYPE_ALLOW_NULL(prop_info->type)) {
						zend_throw_access_uninit_prop_by_ref_error(prop_info);
						if (result) ZVAL_ERROR(result);
						return 0;
					}
					ZVAL_NULL(ptr);
				}

				ZVAL_NEW_REF(ptr, ptr);
				ZEND_REF_ADD_TYPE_SOURCE(Z_REF_P(ptr), prop_info);
			}
			break;
		EMPTY_SWITCH_DEFAULT_CASE()
	}
	return 1;
}

static zend_always_inline void zend_fetch_property_address(zval *result, zval *container, uint32_t container_op_type, zval *prop_ptr, uint32_t prop_op_type, void **cache_slot, int type, uint32_t flags, zend_bool init_undef OPLINE_DC EXECUTE_DATA_DC)
{
	zval *ptr;

	if (container_op_type != IS_UNUSED && UNEXPECTED(Z_TYPE_P(container) != IS_OBJECT)) {
		do {
			if (Z_ISREF_P(container) && Z_TYPE_P(Z_REFVAL_P(container)) == IS_OBJECT) {
				container = Z_REFVAL_P(container);
				break;
			}

			if (container_op_type == IS_CV
			 && type != BP_VAR_W
			 && UNEXPECTED(Z_TYPE_P(container) == IS_UNDEF)) {
				ZVAL_UNDEFINED_OP1();
			}

			/* this should modify object only if it's empty */
			if (type == BP_VAR_UNSET) {
				ZVAL_NULL(result);
				return;
			}

			container = make_real_object(container, prop_ptr OPLINE_CC EXECUTE_DATA_CC);
			if (UNEXPECTED(!container)) {
				ZVAL_ERROR(result);
				return;
			}
		} while (0);
	}
	if (prop_op_type == IS_CONST &&
	    EXPECTED(Z_OBJCE_P(container) == CACHED_PTR_EX(cache_slot))) {
		uintptr_t prop_offset = (uintptr_t)CACHED_PTR_EX(cache_slot + 1);
		zend_object *zobj = Z_OBJ_P(container);

		if (EXPECTED(IS_VALID_PROPERTY_OFFSET(prop_offset))) {
			ptr = OBJ_PROP(zobj, prop_offset);
			if (EXPECTED(Z_TYPE_P(ptr) != IS_UNDEF)) {
				ZVAL_INDIRECT(result, ptr);
				if (flags) {
					zend_property_info *prop_info = CACHED_PTR_EX(cache_slot + 2);
					if (prop_info) {
						zend_handle_fetch_obj_flags(result, ptr, NULL, prop_info, flags);
					}
				}
				return;
			}
		} else if (EXPECTED(zobj->properties != NULL)) {
			if (UNEXPECTED(GC_REFCOUNT(zobj->properties) > 1)) {
				if (EXPECTED(!(GC_FLAGS(zobj->properties) & IS_ARRAY_IMMUTABLE))) {
					GC_DELREF(zobj->properties);
				}
				zobj->properties = zend_array_dup(zobj->properties);
			}
			ptr = zend_hash_find_ex(zobj->properties, Z_STR_P(prop_ptr), 1);
			if (EXPECTED(ptr)) {
				ZVAL_INDIRECT(result, ptr);
				return;
			}
		}
	}
	ptr = Z_OBJ_HT_P(container)->get_property_ptr_ptr(container, prop_ptr, type, cache_slot);
	if (NULL == ptr) {
		ptr = Z_OBJ_HT_P(container)->read_property(container, prop_ptr, type, cache_slot, result);
		if (ptr == result) {
			if (UNEXPECTED(Z_ISREF_P(ptr) && Z_REFCOUNT_P(ptr) == 1)) {
				ZVAL_UNREF(ptr);
			}
			return;
		}
	} else if (UNEXPECTED(Z_ISERROR_P(ptr))) {
		ZVAL_ERROR(result);
		return;
	}

	ZVAL_INDIRECT(result, ptr);
	if (flags) {
		zend_property_info *prop_info;

		if (prop_op_type == IS_CONST) {
			prop_info = CACHED_PTR_EX(cache_slot + 2);
			if (prop_info) {
				if (UNEXPECTED(!zend_handle_fetch_obj_flags(result, ptr, NULL, prop_info, flags))) {
					return;
				}
			}
		} else {
			if (UNEXPECTED(!zend_handle_fetch_obj_flags(result, ptr, Z_OBJ_P(container), NULL, flags))) {
				return;
			}
		}
	}
	if (init_undef && UNEXPECTED(Z_TYPE_P(ptr) == IS_UNDEF)) {
		ZVAL_NULL(ptr);
	}
}

static zend_always_inline void zend_assign_to_property_reference(zval *container, uint32_t container_op_type, zval *prop_ptr, uint32_t prop_op_type, zval *value_ptr OPLINE_DC EXECUTE_DATA_DC)
{
	zval variable, *variable_ptr = &variable;
	void **cache_addr = (prop_op_type == IS_CONST) ? CACHE_ADDR(opline->extended_value & ~ZEND_RETURNS_FUNCTION) : NULL;

	zend_fetch_property_address(variable_ptr, container, container_op_type, prop_ptr, prop_op_type,
		cache_addr, BP_VAR_W, 0, 0 OPLINE_CC EXECUTE_DATA_CC);

	if (Z_TYPE_P(variable_ptr) == IS_INDIRECT) {
		variable_ptr = Z_INDIRECT_P(variable_ptr);
	}

	if (UNEXPECTED(Z_ISERROR_P(variable_ptr))) {
		variable_ptr = &EG(uninitialized_zval);
	} else if (UNEXPECTED(Z_TYPE(variable) != IS_INDIRECT)) {
		zend_throw_error(NULL, "Cannot assign by reference to overloaded object");
		zval_ptr_dtor(&variable);
		variable_ptr = &EG(uninitialized_zval);
	} else if (/*OP_DATA_TYPE == IS_VAR &&*/ UNEXPECTED(Z_ISERROR_P(value_ptr))) {
		variable_ptr = &EG(uninitialized_zval);
	} else if (/*OP_DATA_TYPE == IS_VAR &&*/
	           (opline->extended_value & ZEND_RETURNS_FUNCTION) &&
			   UNEXPECTED(!Z_ISREF_P(value_ptr))) {

		if (UNEXPECTED(!zend_wrong_assign_to_variable_reference(
				variable_ptr, value_ptr OPLINE_CC EXECUTE_DATA_CC))) {
			variable_ptr = &EG(uninitialized_zval);
		}
	} else {
		zend_property_info *prop_info = NULL;

		if (prop_op_type == IS_CONST) {
			prop_info = (zend_property_info *) CACHED_PTR_EX(cache_addr + 2);
		} else {
			ZVAL_DEREF(container);
			prop_info = zend_object_fetch_property_type_info(Z_OBJ_P(container), variable_ptr);
		}

		if (UNEXPECTED(prop_info)) {
			variable_ptr = zend_assign_to_typed_property_reference(prop_info, variable_ptr, value_ptr EXECUTE_DATA_CC);
		} else {
			zend_assign_to_variable_reference(variable_ptr, value_ptr);
		}
	}

	if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
		ZVAL_COPY(EX_VAR(opline->result.var), variable_ptr);
	}
}

static zend_never_inline void zend_assign_to_property_reference_this_const(zval *container, zval *prop_ptr, zval *value_ptr OPLINE_DC EXECUTE_DATA_DC)
{
	zend_assign_to_property_reference(container, IS_UNUSED, prop_ptr, IS_CONST, value_ptr
		OPLINE_CC EXECUTE_DATA_CC);
}

static zend_never_inline void zend_assign_to_property_reference_var_const(zval *container, zval *prop_ptr, zval *value_ptr OPLINE_DC EXECUTE_DATA_DC)
{
	zend_assign_to_property_reference(container, IS_VAR, prop_ptr, IS_CONST, value_ptr
		OPLINE_CC EXECUTE_DATA_CC);
}

static zend_never_inline void zend_assign_to_property_reference_this_var(zval *container, zval *prop_ptr, zval *value_ptr OPLINE_DC EXECUTE_DATA_DC)
{
	zend_assign_to_property_reference(container, IS_UNUSED, prop_ptr, IS_VAR, value_ptr
		OPLINE_CC EXECUTE_DATA_CC);
}

static zend_never_inline void zend_assign_to_property_reference_var_var(zval *container, zval *prop_ptr, zval *value_ptr OPLINE_DC EXECUTE_DATA_DC)
{
	zend_assign_to_property_reference(container, IS_VAR, prop_ptr, IS_VAR, value_ptr
		OPLINE_CC EXECUTE_DATA_CC);
}

static zend_never_inline int zend_fetch_static_property_address_ex(zval **retval, zend_property_info **prop_info, uint32_t cache_slot, int fetch_type OPLINE_DC EXECUTE_DATA_DC) {
	zend_free_op free_op1;
	zend_string *name, *tmp_name;
	zend_class_entry *ce;
	zend_property_info *property_info;

	zend_uchar op1_type = opline->op1_type, op2_type = opline->op2_type;

	if (EXPECTED(op2_type == IS_CONST)) {
		zval *class_name = RT_CONSTANT(opline, opline->op2);

		ZEND_ASSERT(op1_type != IS_CONST || CACHED_PTR(cache_slot) == NULL);

		if (EXPECTED((ce = CACHED_PTR(cache_slot)) == NULL)) {
			ce = zend_fetch_class_by_name(Z_STR_P(class_name), Z_STR_P(class_name + 1), ZEND_FETCH_CLASS_DEFAULT | ZEND_FETCH_CLASS_EXCEPTION);
			if (UNEXPECTED(ce == NULL)) {
				FREE_UNFETCHED_OP(op1_type, opline->op1.var);
				return FAILURE;
			}
			if (UNEXPECTED(op1_type != IS_CONST)) {
				CACHE_PTR(cache_slot, ce);
			}
		}
	} else {
		if (EXPECTED(op2_type == IS_UNUSED)) {
			ce = zend_fetch_class(NULL, opline->op2.num);
			if (UNEXPECTED(ce == NULL)) {
				FREE_UNFETCHED_OP(op1_type, opline->op1.var);
				return FAILURE;
			}
		} else {
			ce = Z_CE_P(EX_VAR(opline->op2.var));
		}
		if (EXPECTED(op1_type == IS_CONST) && EXPECTED(CACHED_PTR(cache_slot) == ce)) {
			*retval = CACHED_PTR(cache_slot + sizeof(void *));
			*prop_info = CACHED_PTR(cache_slot + sizeof(void *) * 2);
			return SUCCESS;
		}
	}

	if (EXPECTED(op1_type == IS_CONST)) {
		name = Z_STR_P(RT_CONSTANT(opline, opline->op1));
	} else {
		zval *varname = get_zval_ptr_undef(opline->op1_type, opline->op1, &free_op1, BP_VAR_R);
		if (EXPECTED(Z_TYPE_P(varname) == IS_STRING)) {
			name = Z_STR_P(varname);
			tmp_name = NULL;
		} else {
			if (op1_type == IS_CV && UNEXPECTED(Z_TYPE_P(varname) == IS_UNDEF)) {
				zval_undefined_cv(opline->op1.var EXECUTE_DATA_CC);
			}
			name = zval_get_tmp_string(varname, &tmp_name);
		}
	}

	*retval = zend_std_get_static_property_with_info(ce, name, fetch_type, &property_info);

	if (UNEXPECTED(op1_type != IS_CONST)) {
		zend_tmp_string_release(tmp_name);

		if (op1_type != IS_CV) {
			zval_ptr_dtor_nogc(free_op1);
		}
	}

	if (UNEXPECTED(*retval == NULL)) {
		return FAILURE;
	}

	*prop_info = property_info;

	if (EXPECTED(op1_type == IS_CONST)) {
		CACHE_POLYMORPHIC_PTR(cache_slot, ce, *retval);
		CACHE_PTR(cache_slot + sizeof(void *) * 2, property_info);
	}

	return SUCCESS;
}


static zend_always_inline int zend_fetch_static_property_address(zval **retval, zend_property_info **prop_info, uint32_t cache_slot, int fetch_type, int flags OPLINE_DC EXECUTE_DATA_DC) {
	int success;
	zend_property_info *property_info;

	if (opline->op1_type == IS_CONST && (opline->op2_type == IS_CONST || (opline->op2_type == IS_UNUSED && (opline->op2.num == ZEND_FETCH_CLASS_SELF || opline->op2.num == ZEND_FETCH_CLASS_PARENT))) && EXPECTED(CACHED_PTR(cache_slot) != NULL)) {
		*retval = CACHED_PTR(cache_slot + sizeof(void *));
		property_info = CACHED_PTR(cache_slot + sizeof(void *) * 2);

		if ((fetch_type == BP_VAR_R || fetch_type == BP_VAR_RW)
				&& UNEXPECTED(Z_TYPE_P(*retval) == IS_UNDEF) && UNEXPECTED(property_info->type != 0)) {
			zend_throw_error(NULL, "Typed static property %s::$%s must not be accessed before initialization",
				ZSTR_VAL(property_info->ce->name),
				zend_get_unmangled_property_name(property_info->name));
			return FAILURE;
		}
	} else {
		success = zend_fetch_static_property_address_ex(retval, &property_info, cache_slot, fetch_type OPLINE_CC EXECUTE_DATA_CC);
		if (UNEXPECTED(success != SUCCESS)) {
			return FAILURE;
		}
	}

	if (flags && property_info->type) {
		zend_handle_fetch_obj_flags(NULL, *retval, NULL, property_info, flags);
	}

	if (prop_info) {
		*prop_info = property_info;
	}

	return SUCCESS;
}

ZEND_API ZEND_COLD void zend_throw_ref_type_error_type(zend_property_info *prop1, zend_property_info *prop2, zval *zv) {
	const char *prop1_type1, *prop1_type2, *prop2_type1, *prop2_type2;
	zend_format_type(prop1->type, &prop1_type1, &prop1_type2);
	zend_format_type(prop2->type, &prop2_type1, &prop2_type2);
	zend_type_error("Reference with value of type %s held by property %s::$%s of type %s%s is not compatible with property %s::$%s of type %s%s",
		Z_TYPE_P(zv) == IS_OBJECT ? ZSTR_VAL(Z_OBJCE_P(zv)->name) : zend_get_type_by_const(Z_TYPE_P(zv)),
		ZSTR_VAL(prop1->ce->name),
		zend_get_unmangled_property_name(prop1->name),
		prop1_type1, prop1_type2,
		ZSTR_VAL(prop2->ce->name),
		zend_get_unmangled_property_name(prop2->name),
		prop2_type1, prop2_type2
	);
}

ZEND_API ZEND_COLD void zend_throw_ref_type_error_zval(zend_property_info *prop, zval *zv) {
	const char *prop_type1, *prop_type2;
	zend_format_type(prop->type, &prop_type1, &prop_type2);
	zend_type_error("Cannot assign %s to reference held by property %s::$%s of type %s%s",
		Z_TYPE_P(zv) == IS_OBJECT ? ZSTR_VAL(Z_OBJCE_P(zv)->name) : zend_get_type_by_const(Z_TYPE_P(zv)),
		ZSTR_VAL(prop->ce->name),
		zend_get_unmangled_property_name(prop->name),
		prop_type1, prop_type2
	);
}

ZEND_API ZEND_COLD void zend_throw_conflicting_coercion_error(zend_property_info *prop1, zend_property_info *prop2, zval *zv) {
	const char *prop1_type1, *prop1_type2, *prop2_type1, *prop2_type2;
	zend_format_type(prop1->type, &prop1_type1, &prop1_type2);
	zend_format_type(prop2->type, &prop2_type1, &prop2_type2);
	zend_type_error("Cannot assign %s to reference held by property %s::$%s of type %s%s and property %s::$%s of type %s%s, as this would result in an inconsistent type conversion",
		Z_TYPE_P(zv) == IS_OBJECT ? ZSTR_VAL(Z_OBJCE_P(zv)->name) : zend_get_type_by_const(Z_TYPE_P(zv)),
		ZSTR_VAL(prop1->ce->name),
		zend_get_unmangled_property_name(prop1->name),
		prop1_type1, prop1_type2,
		ZSTR_VAL(prop2->ce->name),
		zend_get_unmangled_property_name(prop2->name),
		prop2_type1, prop2_type2
	);
}

/* 1: valid, 0: invalid, -1: may be valid after type coercion */
static zend_always_inline int i_zend_verify_type_assignable_zval(
		zend_type *type_ptr, zend_class_entry *self_ce, zval *zv, zend_bool strict) {
	zend_type type = *type_ptr;
	zend_uchar type_code;
	zend_uchar zv_type = Z_TYPE_P(zv);

	if (ZEND_TYPE_ALLOW_NULL(type) && zv_type == IS_NULL) {
		return 1;
	}

	if (ZEND_TYPE_IS_CLASS(type)) {
		if (!ZEND_TYPE_IS_CE(type)) {
			if (!zend_resolve_class_type(type_ptr, self_ce)) {
				return 0;
			}
			type = *type_ptr;
		}
		return zv_type == IS_OBJECT && instanceof_function(Z_OBJCE_P(zv), ZEND_TYPE_CE(type));
	}

	type_code = ZEND_TYPE_CODE(type);
	if (type_code == zv_type ||
			(type_code == _IS_BOOL && (zv_type == IS_FALSE || zv_type == IS_TRUE))) {
		return 1;
	}

	if (type_code == IS_ITERABLE) {
		return zend_is_iterable(zv);
	}

	/* SSTH Exception: IS_LONG may be accepted as IS_DOUBLE (converted) */
	if (strict) {
		if (type_code == IS_DOUBLE && zv_type == IS_LONG) {
			return -1;
		}
		return 0;
	}

	/* No weak conversions for arrays and objects */
	if (type_code == IS_ARRAY || type_code == IS_OBJECT) {
		return 0;
	}

	/* NULL may be accepted only by nullable hints (this is already checked) */
	if (zv_type == IS_NULL) {
		return 0;
	}

	/* Coercion may be necessary, check separately */
	return -1;
}

ZEND_API zend_bool ZEND_FASTCALL zend_verify_ref_assignable_zval(zend_reference *ref, zval *zv, zend_bool strict)
{
	zend_property_info *prop;

	/* The value must satisfy each property type, and coerce to the same value for each property
	 * type. Right now, the latter rule means that *if* coercion is necessary, then all types
	 * must be the same (modulo nullability). To handle this, remember the first type we see and
	 * compare against it when coercion becomes necessary. */
	zend_property_info *seen_prop = NULL;
	zend_uchar seen_type;
	zend_bool needs_coercion = 0;

	ZEND_ASSERT(Z_TYPE_P(zv) != IS_REFERENCE);
	ZEND_REF_FOREACH_TYPE_SOURCES(ref, prop) {
		int result = i_zend_verify_type_assignable_zval(&prop->type, prop->ce, zv, strict);
		if (result == 0) {
			zend_throw_ref_type_error_zval(prop, zv);
			return 0;
		}

		if (result < 0) {
			needs_coercion = 1;
		}

		if (!seen_prop) {
			seen_prop = prop;
			seen_type = ZEND_TYPE_IS_CLASS(prop->type) ? IS_OBJECT : ZEND_TYPE_CODE(prop->type);
		} else if (needs_coercion && seen_type != ZEND_TYPE_CODE(prop->type)) {
			zend_throw_conflicting_coercion_error(seen_prop, prop, zv);
			return 0;
		}
	} ZEND_REF_FOREACH_TYPE_SOURCES_END();

	if (UNEXPECTED(needs_coercion && !zend_verify_weak_scalar_type_hint(seen_type, zv))) {
		zend_throw_ref_type_error_zval(seen_prop, zv);
		return 0;
	}

	return 1;
}

static zend_always_inline void i_zval_ptr_dtor_noref(zval *zval_ptr) {
	if (Z_REFCOUNTED_P(zval_ptr)) {
		zend_refcounted *ref = Z_COUNTED_P(zval_ptr);
		ZEND_ASSERT(Z_TYPE_P(zval_ptr) != IS_REFERENCE);
		if (!GC_DELREF(ref)) {
			rc_dtor_func(ref);
		} else if (UNEXPECTED(GC_MAY_LEAK(ref))) {
			gc_possible_root(ref);
		}
	}
}

ZEND_API zval* zend_assign_to_typed_ref(zval *variable_ptr, zval *orig_value, zend_uchar value_type, zend_bool strict, zend_refcounted *ref)
{
	zend_bool ret;
	zval value;
	ZVAL_COPY(&value, orig_value);
	ret = zend_verify_ref_assignable_zval(Z_REF_P(variable_ptr), &value, strict);
	variable_ptr = Z_REFVAL_P(variable_ptr);
	if (EXPECTED(ret)) {
		i_zval_ptr_dtor_noref(variable_ptr);
		ZVAL_COPY_VALUE(variable_ptr, &value);
	} else {
		zval_ptr_dtor_nogc(&value);
	}
	if (value_type & (IS_VAR|IS_TMP_VAR)) {
		if (UNEXPECTED(ref)) {
			if (UNEXPECTED(GC_DELREF(ref) == 0)) {
				zval_ptr_dtor(orig_value);
				efree_size(ref, sizeof(zend_reference));
			}
		} else {
			i_zval_ptr_dtor_noref(orig_value);
		}
	}
	return variable_ptr;
}

ZEND_API zend_bool ZEND_FASTCALL zend_verify_prop_assignable_by_ref(zend_property_info *prop_info, zval *orig_val, zend_bool strict) {
	zval *val = orig_val;
	if (Z_ISREF_P(val) && ZEND_REF_HAS_TYPE_SOURCES(Z_REF_P(val))) {
		int result;

		val = Z_REFVAL_P(val);
		result = i_zend_verify_type_assignable_zval(&prop_info->type, prop_info->ce, val, strict);
		if (result > 0) {
			return 1;
		}

		if (result < 0) {
			zend_property_info *ref_prop = ZEND_REF_FIRST_SOURCE(Z_REF_P(orig_val));
			if (ZEND_TYPE_CODE(prop_info->type) != ZEND_TYPE_CODE(ref_prop->type)) {
				/* Invalid due to conflicting coercion */
				zend_throw_ref_type_error_type(ref_prop, prop_info, val);
				return 0;
			}
			if (zend_verify_weak_scalar_type_hint(ZEND_TYPE_CODE(prop_info->type), val)) {
				return 1;
			}
		}
	} else {
		ZVAL_DEREF(val);
		if (i_zend_check_property_type(prop_info, val, strict)) {
			return 1;
		}
	}

	zend_verify_property_type_error(prop_info, val);
	return 0;
}

ZEND_API void ZEND_FASTCALL zend_ref_add_type_source(zend_property_info_source_list *source_list, zend_property_info *prop)
{
	zend_property_info_list *list;
	if (source_list->ptr == NULL) {
		source_list->ptr = prop;
		return;
	}

	list = ZEND_PROPERTY_INFO_SOURCE_TO_LIST(source_list->list);
	if (!ZEND_PROPERTY_INFO_SOURCE_IS_LIST(source_list->list)) {
		list = emalloc(sizeof(zend_property_info_list) + (4 - 1) * sizeof(zend_property_info *));
		list->ptr[0] = source_list->ptr;
		list->num_allocated = 4;
		list->num = 1;
	} else if (list->num_allocated == list->num) {
		list->num_allocated = list->num * 2;
		list = erealloc(list, sizeof(zend_property_info_list) + (list->num_allocated - 1) * sizeof(zend_property_info *));
	}

	list->ptr[list->num++] = prop;
	source_list->list = ZEND_PROPERTY_INFO_SOURCE_FROM_LIST(list);
}

ZEND_API void ZEND_FASTCALL zend_ref_del_type_source(zend_property_info_source_list *source_list, zend_property_info *prop)
{
	zend_property_info_list *list = ZEND_PROPERTY_INFO_SOURCE_TO_LIST(source_list->list);
	zend_property_info **ptr, **end;

	if (!ZEND_PROPERTY_INFO_SOURCE_IS_LIST(source_list->list)) {
		ZEND_ASSERT(source_list->ptr == prop);
		source_list->ptr = NULL;
		return;
	}

	if (list->num == 1) {
		ZEND_ASSERT(*list->ptr == prop);
		efree(list);
		source_list->ptr = NULL;
		return;
	}

	/* Checking against end here to get a more graceful failure mode if we missed adding a type
	 * source at some point. */
	ptr = list->ptr;
	end = ptr + list->num;
	while (ptr < end && *ptr != prop) {
		ptr++;
	}
	ZEND_ASSERT(*ptr == prop);

	/* Copy the last list element into the deleted slot. */
	*ptr = list->ptr[--list->num];

	if (list->num >= 4 && list->num * 4 == list->num_allocated) {
		list->num_allocated = list->num * 2;
		source_list->list = ZEND_PROPERTY_INFO_SOURCE_FROM_LIST(erealloc(list, sizeof(zend_property_info_list) + (list->num_allocated - 1) * sizeof(zend_property_info *)));
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

static zend_never_inline ZEND_COLD void ZEND_FASTCALL zend_wrong_clone_call(zend_function *clone, zend_class_entry *scope)
{
	zend_throw_error(NULL, "Call to %s %s::__clone() from context '%s'", zend_visibility_string(clone->common.fn_flags), ZSTR_VAL(clone->common.scope->name), scope ? ZSTR_VAL(scope->name) : "");
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
		*(EG(symtable_cache_ptr)++) = symbol_table;
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
 *     EX_VAR_NUM(0) --------> | VAR[0] = ARG[1]                        |
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

/* zend_copy_extra_args is used when the actually passed number of arguments
 * (EX_NUM_ARGS) is greater than what the function defined (op_array->num_args).
 *
 * The extra arguments will be copied into the call frame after all the compiled variables.
 *
 * If there are extra arguments copied, a flag "ZEND_CALL_FREE_EXTRA_ARGS" will be set
 * on the zend_execute_data, and when the executor leaves the function, the
 * args will be freed in zend_leave_helper.
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
#if defined(ZEND_VM_IP_GLOBAL_REG) && ((ZEND_VM_KIND == ZEND_VM_KIND_CALL) || (ZEND_VM_KIND == ZEND_VM_KIND_HYBRID))
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

#if defined(ZEND_VM_IP_GLOBAL_REG) && ((ZEND_VM_KIND == ZEND_VM_KIND_CALL) || (ZEND_VM_KIND == ZEND_VM_KIND_HYBRID))
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
#if defined(ZEND_VM_IP_GLOBAL_REG) && ((ZEND_VM_KIND == ZEND_VM_KIND_CALL) || (ZEND_VM_KIND == ZEND_VM_KIND_HYBRID))
		opline += num_args;
#else
		EX(opline) += num_args;
#endif
	}

	/* Initialize CV variables (skip arguments) */
	zend_init_cvs(num_args, op_array->last_var EXECUTE_DATA_CC);

	EX(run_time_cache) = RUN_TIME_CACHE(op_array);

	EG(current_execute_data) = execute_data;
}
/* }}} */

static zend_always_inline void init_func_run_time_cache_i(zend_op_array *op_array) /* {{{ */
{
	void **run_time_cache;

	ZEND_ASSERT(RUN_TIME_CACHE(op_array) == NULL);
	run_time_cache = zend_arena_alloc(&CG(arena), op_array->cache_size);
	memset(run_time_cache, 0, op_array->cache_size);
	ZEND_MAP_PTR_SET(op_array->run_time_cache, run_time_cache);
}
/* }}} */

static zend_never_inline void ZEND_FASTCALL init_func_run_time_cache(zend_op_array *op_array) /* {{{ */
{
	init_func_run_time_cache_i(op_array);
}
/* }}} */

ZEND_API zend_function * ZEND_FASTCALL zend_fetch_function(zend_string *name) /* {{{ */
{
	zval *zv = zend_hash_find(EG(function_table), name);

	if (EXPECTED(zv != NULL)) {
		zend_function *fbc = Z_FUNC_P(zv);

		if (EXPECTED(fbc->type == ZEND_USER_FUNCTION) && UNEXPECTED(!RUN_TIME_CACHE(&fbc->op_array))) {
			init_func_run_time_cache_i(&fbc->op_array);
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

		if (EXPECTED(fbc->type == ZEND_USER_FUNCTION) && UNEXPECTED(!RUN_TIME_CACHE(&fbc->op_array))) {
			init_func_run_time_cache_i(&fbc->op_array);
		}
		return fbc;
	}
	return NULL;
} /* }}} */

ZEND_API void ZEND_FASTCALL zend_init_func_run_time_cache(zend_op_array *op_array) /* {{{ */
{
	if (!RUN_TIME_CACHE(op_array)) {
		init_func_run_time_cache_i(op_array);
	}
} /* }}} */

static zend_always_inline void i_init_code_execute_data(zend_execute_data *execute_data, zend_op_array *op_array, zval *return_value) /* {{{ */
{
	ZEND_ASSERT(EX(func) == (zend_function*)op_array);

	EX(opline) = op_array->opcodes;
	EX(call) = NULL;
	EX(return_value) = return_value;

	zend_attach_symbol_table(execute_data);

	if (!ZEND_MAP_PTR(op_array->run_time_cache)) {
		void *ptr;

		ZEND_ASSERT(op_array->fn_flags & ZEND_ACC_HEAP_RT_CACHE);
		ptr = emalloc(op_array->cache_size + sizeof(void*));
		ZEND_MAP_PTR_INIT(op_array->run_time_cache, ptr);
		ptr = (char*)ptr + sizeof(void*);
		ZEND_MAP_PTR_SET(op_array->run_time_cache, ptr);
		memset(ptr, 0, op_array->cache_size);
	}
	EX(run_time_cache) = RUN_TIME_CACHE(op_array);

	EG(current_execute_data) = execute_data;
}
/* }}} */

ZEND_API void zend_init_func_execute_data(zend_execute_data *ex, zend_op_array *op_array, zval *return_value) /* {{{ */
{
#if defined(ZEND_VM_FP_GLOBAL_REG) && ((ZEND_VM_KIND == ZEND_VM_KIND_CALL) || (ZEND_VM_KIND == ZEND_VM_KIND_HYBRID))
	zend_execute_data *orig_execute_data = execute_data;
#endif
#if defined(ZEND_VM_IP_GLOBAL_REG) && ((ZEND_VM_KIND == ZEND_VM_KIND_CALL) || (ZEND_VM_KIND == ZEND_VM_KIND_HYBRID))
	const zend_op *orig_opline = opline;
#endif
#if defined(ZEND_VM_FP_GLOBAL_REG) && ((ZEND_VM_KIND == ZEND_VM_KIND_CALL) || (ZEND_VM_KIND == ZEND_VM_KIND_HYBRID))
	execute_data = ex;
#else
	zend_execute_data *execute_data = ex;
#endif

	EX(prev_execute_data) = EG(current_execute_data);
	if (!RUN_TIME_CACHE(op_array)) {
		init_func_run_time_cache(op_array);
	}
	i_init_func_execute_data(op_array, return_value, 1 EXECUTE_DATA_CC);

#if defined(ZEND_VM_IP_GLOBAL_REG) && ((ZEND_VM_KIND == ZEND_VM_KIND_CALL) || (ZEND_VM_KIND == ZEND_VM_KIND_HYBRID))
    EX(opline) = opline;
	opline = orig_opline;
#endif
#if defined(ZEND_VM_FP_GLOBAL_REG) && ((ZEND_VM_KIND == ZEND_VM_KIND_CALL) || (ZEND_VM_KIND == ZEND_VM_KIND_HYBRID))
	execute_data = orig_execute_data;
#endif
}
/* }}} */

ZEND_API void zend_init_code_execute_data(zend_execute_data *execute_data, zend_op_array *op_array, zval *return_value) /* {{{ */
{
	EX(prev_execute_data) = EG(current_execute_data);
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

	/* delete previous stack segment if it became empty */
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
				} else if (kind == ZEND_LIVE_NEW) {
					zend_object *obj;
					ZEND_ASSERT(Z_TYPE_P(var) == IS_OBJECT);
					obj = Z_OBJ_P(var);
					zend_object_store_ctor_failed(obj);
					OBJ_RELEASE(obj);
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

#if ZEND_VM_SPEC
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
#endif

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
		if (EXPECTED(fbc->type == ZEND_USER_FUNCTION) && UNEXPECTED(!RUN_TIME_CACHE(&fbc->op_array))) {
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
		if (EXPECTED(fbc->type == ZEND_USER_FUNCTION) && UNEXPECTED(!RUN_TIME_CACHE(&fbc->op_array))) {
			init_func_run_time_cache(&fbc->op_array);
		}
		called_scope = NULL;
	}

	return zend_vm_stack_push_call_frame(ZEND_CALL_NESTED_FUNCTION | ZEND_CALL_DYNAMIC,
		fbc, num_args, called_scope);
}
/* }}} */

static zend_never_inline zend_execute_data *zend_init_dynamic_call_object(zval *function, uint32_t num_args) /* {{{ */
{
	zend_function *fbc;
	void *object_or_called_scope;
	zend_class_entry *called_scope;
	zend_object *object;
	uint32_t call_info = ZEND_CALL_NESTED_FUNCTION | ZEND_CALL_DYNAMIC;

	if (EXPECTED(Z_OBJ_HANDLER_P(function, get_closure)) &&
	    EXPECTED(Z_OBJ_HANDLER_P(function, get_closure)(function, &called_scope, &fbc, &object) == SUCCESS)) {

	    object_or_called_scope = called_scope;
		if (fbc->common.fn_flags & ZEND_ACC_CLOSURE) {
			/* Delay closure destruction until its invocation */
			GC_ADDREF(ZEND_CLOSURE_OBJECT(fbc));
			call_info |= ZEND_CALL_CLOSURE;
			if (fbc->common.fn_flags & ZEND_ACC_FAKE_CLOSURE) {
				call_info |= ZEND_CALL_FAKE_CLOSURE;
			}
			if (object) {
				call_info |= ZEND_CALL_HAS_THIS;
				object_or_called_scope = object;
			}
		} else if (object) {
			call_info |= ZEND_CALL_RELEASE_THIS | ZEND_CALL_HAS_THIS;
			GC_ADDREF(object); /* For $this pointer */
			object_or_called_scope = object;
		}
	} else {
		zend_throw_error(NULL, "Function name must be a string");
		return NULL;
	}

	if (EXPECTED(fbc->type == ZEND_USER_FUNCTION) && UNEXPECTED(!RUN_TIME_CACHE(&fbc->op_array))) {
		init_func_run_time_cache(&fbc->op_array);
	}

	return zend_vm_stack_push_call_frame(call_info,
		fbc, num_args, object_or_called_scope);
}
/* }}} */

static zend_never_inline zend_execute_data *zend_init_dynamic_call_array(zend_array *function, uint32_t num_args) /* {{{ */
{
	zend_function *fbc;
	void *object_or_called_scope;
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
			zend_class_entry *called_scope = zend_fetch_class_by_name(Z_STR_P(obj), NULL, ZEND_FETCH_CLASS_DEFAULT | ZEND_FETCH_CLASS_EXCEPTION);

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
			object_or_called_scope = called_scope;
		} else {
			zend_object *object = Z_OBJ_P(obj);

			fbc = Z_OBJ_HT_P(obj)->get_method(&object, Z_STR_P(method), NULL);
			if (UNEXPECTED(fbc == NULL)) {
				if (EXPECTED(!EG(exception))) {
					zend_undefined_method(object->ce, Z_STR_P(method));
				}
				return NULL;
			}

			if ((fbc->common.fn_flags & ZEND_ACC_STATIC) != 0) {
				object_or_called_scope = object->ce;
			} else {
				call_info |= ZEND_CALL_RELEASE_THIS | ZEND_CALL_HAS_THIS;
				GC_ADDREF(object); /* For $this pointer */
				object_or_called_scope = object;
			}
		}
	} else {
		zend_throw_error(NULL, "Function name must be a string");
		return NULL;
	}

	if (EXPECTED(fbc->type == ZEND_USER_FUNCTION) && UNEXPECTED(!RUN_TIME_CACHE(&fbc->op_array))) {
		init_func_run_time_cache(&fbc->op_array);
	}

	return zend_vm_stack_push_call_frame(call_info,
		fbc, num_args, object_or_called_scope);
}
/* }}} */

#define ZEND_FAKE_OP_ARRAY ((zend_op_array*)(zend_intptr_t)-1)

static zend_never_inline zend_op_array* ZEND_FASTCALL zend_include_or_eval(zval *inc_filename, int type) /* {{{ */
{
	zend_op_array *new_op_array = NULL;
	zval tmp_inc_filename;

	ZVAL_UNDEF(&tmp_inc_filename);
	if (Z_TYPE_P(inc_filename) != IS_STRING) {
		zend_string *tmp = zval_try_get_string_func(inc_filename);

		if (UNEXPECTED(!tmp)) {
			return NULL;
		}
		ZVAL_STR(&tmp_inc_filename, tmp);
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
				} else if (UNEXPECTED(EG(exception))) {
					break;
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

static ZEND_COLD int ZEND_FASTCALL zend_do_fcall_overloaded(zend_execute_data *call, zval *ret) /* {{{ */
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
				/* Namespaces are always case-insensitive. Only compare shortname. */
				ns_sep = zend_memrchr(ZSTR_VAL(c->name), '\\', ZSTR_LEN(c->name));
				if (ns_sep) {
					shortname_offset = ns_sep - ZSTR_VAL(c->name) + 1;
					shortname_len = ZSTR_LEN(c->name) - shortname_offset;
				} else {
					shortname_offset = 0;
					shortname_len = ZSTR_LEN(c->name);
				}

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

#if defined(ZEND_VM_IP_GLOBAL_REG) && ((ZEND_VM_KIND == ZEND_VM_KIND_CALL) || (ZEND_VM_KIND == ZEND_VM_KIND_HYBRID))
/* Special versions of functions that sets EX(opline) before calling zend_vm_stack_extend() */
static zend_always_inline zend_execute_data *_zend_vm_stack_push_call_frame_ex(uint32_t used_stack, uint32_t call_info, zend_function *func, uint32_t num_args, void *object_or_called_scope) /* {{{ */
{
	zend_execute_data *call = (zend_execute_data*)EG(vm_stack_top);

	ZEND_ASSERT_VM_STACK_GLOBAL;

	if (UNEXPECTED(used_stack > (size_t)(((char*)EG(vm_stack_end)) - (char*)call))) {
		EX(opline) = opline; /* this is the only difference */
		call = (zend_execute_data*)zend_vm_stack_extend(used_stack);
		ZEND_ASSERT_VM_STACK_GLOBAL;
		zend_vm_init_call_frame(call, call_info | ZEND_CALL_ALLOCATED, func, num_args, object_or_called_scope);
		return call;
	} else {
		EG(vm_stack_top) = (zval*)((char*)call + used_stack);
		zend_vm_init_call_frame(call, call_info, func, num_args, object_or_called_scope);
		return call;
	}
} /* }}} */

static zend_always_inline zend_execute_data *_zend_vm_stack_push_call_frame(uint32_t call_info, zend_function *func, uint32_t num_args, void *object_or_called_scope) /* {{{ */
{
	uint32_t used_stack = zend_vm_calc_used_stack(num_args, func);

	return _zend_vm_stack_push_call_frame_ex(used_stack, call_info,
		func, num_args, object_or_called_scope);
} /* }}} */
#else
# define _zend_vm_stack_push_call_frame_ex zend_vm_stack_push_call_frame_ex
# define _zend_vm_stack_push_call_frame    zend_vm_stack_push_call_frame
#endif

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
		if ((_check) && UNEXPECTED(EG(exception))) { \
			break; \
		} \
		if (EXPECTED((opline+1)->opcode == ZEND_JMPZ)) { \
			if (_result) { \
				ZEND_VM_SET_NEXT_OPCODE(opline + 2); \
			} else { \
				ZEND_VM_SET_OPCODE(OP_JMP_ADDR(opline + 1, (opline+1)->op2)); \
			} \
		} else if (EXPECTED((opline+1)->opcode == ZEND_JMPNZ)) { \
			if (!(_result)) { \
				ZEND_VM_SET_NEXT_OPCODE(opline + 2); \
			} else { \
				ZEND_VM_SET_OPCODE(OP_JMP_ADDR(opline + 1, (opline+1)->op2)); \
			} \
		} else { \
			break; \
		} \
		ZEND_VM_CONTINUE(); \
	} while (0)
# define ZEND_VM_SMART_BRANCH_JMPZ(_result, _check) do { \
		if ((_check) && UNEXPECTED(EG(exception))) { \
			break; \
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
			break; \
		} \
		if (!(_result)) { \
			ZEND_VM_SET_NEXT_OPCODE(opline + 2); \
		} else { \
			ZEND_VM_SET_OPCODE(OP_JMP_ADDR(opline + 1, (opline+1)->op2)); \
		} \
		ZEND_VM_CONTINUE(); \
	} while (0)
#define ZEND_VM_SMART_BRANCH_TRUE() do { \
		if (EXPECTED((opline+1)->opcode == ZEND_JMPNZ)) { \
			ZEND_VM_SET_OPCODE(OP_JMP_ADDR(opline + 1, (opline+1)->op2)); \
			ZEND_VM_CONTINUE(); \
		} else if (EXPECTED((opline+1)->opcode == ZEND_JMPZ)) { \
			ZEND_VM_SET_NEXT_OPCODE(opline + 2); \
			ZEND_VM_CONTINUE(); \
		} \
	} while (0)
#define ZEND_VM_SMART_BRANCH_TRUE_JMPZ() do { \
		ZEND_VM_SET_NEXT_OPCODE(opline + 2); \
		ZEND_VM_CONTINUE(); \
	} while (0)
#define ZEND_VM_SMART_BRANCH_TRUE_JMPNZ() do { \
		ZEND_VM_SET_OPCODE(OP_JMP_ADDR(opline + 1, (opline+1)->op2)); \
		ZEND_VM_CONTINUE(); \
	} while (0)
#define ZEND_VM_SMART_BRANCH_FALSE() do { \
		if (EXPECTED((opline+1)->opcode == ZEND_JMPNZ)) { \
			ZEND_VM_SET_NEXT_OPCODE(opline + 2); \
			ZEND_VM_CONTINUE(); \
		} else if (EXPECTED((opline+1)->opcode == ZEND_JMPZ)) { \
			ZEND_VM_SET_OPCODE(OP_JMP_ADDR(opline + 1, (opline+1)->op2)); \
			ZEND_VM_CONTINUE(); \
		} \
	} while (0)
#define ZEND_VM_SMART_BRANCH_FALSE_JMPZ() do { \
		ZEND_VM_SET_OPCODE(OP_JMP_ADDR(opline + 1, (opline+1)->op2)); \
		ZEND_VM_CONTINUE(); \
	} while (0)
#define ZEND_VM_SMART_BRANCH_FALSE_JMPNZ() do { \
		ZEND_VM_SET_NEXT_OPCODE(opline + 2); \
		ZEND_VM_CONTINUE(); \
	} while (0)
#else
# define ZEND_VM_REPEATABLE_OPCODE
# define ZEND_VM_REPEAT_OPCODE(_opcode)
# define ZEND_VM_SMART_BRANCH(_result, _check)
# define ZEND_VM_SMART_BRANCH_JMPZ(_result, _check)
# define ZEND_VM_SMART_BRANCH_JMPNZ(_result, _check)
# define ZEND_VM_SMART_BRANCH_TRUE()
# define ZEND_VM_SMART_BRANCH_FALSE()
#endif

#ifdef __GNUC__
# define ZEND_VM_GUARD(name) __asm__("#" #name)
#else
# define ZEND_VM_GUARD(name)
#endif

#define UNDEF_RESULT() do { \
		if (opline->result_type & (IS_VAR | IS_TMP_VAR)) { \
			ZVAL_UNDEF(EX_VAR(opline->result.var)); \
		} \
	} while (0)

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
