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
#include "zend_smart_str.h"
#include "zend_observer.h"
#include "zend_system_id.h"
#include "zend_call_stack.h"
#include "Optimizer/zend_func_info.h"

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
#  define ZEND_VM_FP_GLOBAL_REG "r14"
#  define ZEND_VM_IP_GLOBAL_REG "r15"
# elif defined(__IBMC__) && ZEND_GCC_VERSION >= 4002 && defined(__powerpc64__)
#  define ZEND_VM_FP_GLOBAL_REG "r14"
#  define ZEND_VM_IP_GLOBAL_REG "r15"
# elif defined(__GNUC__) && ZEND_GCC_VERSION >= 4008 && defined(__aarch64__)
#  define ZEND_VM_FP_GLOBAL_REG "x27"
#  define ZEND_VM_IP_GLOBAL_REG "x28"
#elif defined(__GNUC__) && ZEND_GCC_VERSION >= 4008 && defined(__riscv) && __riscv_xlen == 64
#  define ZEND_VM_FP_GLOBAL_REG "x18"
#  define ZEND_VM_IP_GLOBAL_REG "x19"
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

#define get_zval_ptr(op_type, node, type) _get_zval_ptr(op_type, node, type EXECUTE_DATA_CC OPLINE_CC)
#define get_zval_ptr_deref(op_type, node, type) _get_zval_ptr_deref(op_type, node, type EXECUTE_DATA_CC OPLINE_CC)
#define get_zval_ptr_undef(op_type, node, type) _get_zval_ptr_undef(op_type, node, type EXECUTE_DATA_CC OPLINE_CC)
#define get_op_data_zval_ptr_r(op_type, node) _get_op_data_zval_ptr_r(op_type, node EXECUTE_DATA_CC OPLINE_CC)
#define get_op_data_zval_ptr_deref_r(op_type, node) _get_op_data_zval_ptr_deref_r(op_type, node EXECUTE_DATA_CC OPLINE_CC)
#define get_zval_ptr_ptr(op_type, node, type) _get_zval_ptr_ptr(op_type, node, type EXECUTE_DATA_CC)
#define get_zval_ptr_ptr_undef(op_type, node, type) _get_zval_ptr_ptr(op_type, node, type EXECUTE_DATA_CC)
#define get_obj_zval_ptr(op_type, node, type) _get_obj_zval_ptr(op_type, node, type EXECUTE_DATA_CC OPLINE_CC)
#define get_obj_zval_ptr_undef(op_type, node, type) _get_obj_zval_ptr_undef(op_type, node, type EXECUTE_DATA_CC OPLINE_CC)
#define get_obj_zval_ptr_ptr(op_type, node, type) _get_obj_zval_ptr_ptr(op_type, node, type EXECUTE_DATA_CC)

#define RETURN_VALUE_USED(opline) ((opline)->result_type != IS_UNUSED)

static ZEND_FUNCTION(pass)
{
}

ZEND_BEGIN_ARG_INFO_EX(zend_pass_function_arg_info, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_API const zend_internal_function zend_pass_function = {
	ZEND_INTERNAL_FUNCTION, /* type              */
	{0, 0, 0},              /* arg_flags         */
	0,                      /* fn_flags          */
	NULL,                   /* name              */
	NULL,                   /* scope             */
	NULL,                   /* prototype         */
	0,                      /* num_args          */
	0,                      /* required_num_args */
	(zend_internal_arg_info *) zend_pass_function_arg_info + 1, /* arg_info */
	NULL,                   /* attributes        */
	NULL,                   /* run_time_cache    */
	0,                      /* T                 */
	ZEND_FN(pass),          /* handler           */
	NULL,                   /* module            */
	{NULL,NULL,NULL,NULL}   /* reserved          */
};

#define FREE_VAR_PTR_AND_EXTRACT_RESULT_IF_NECESSARY(free_var) do {			\
	zval *__container_to_free = EX_VAR(free_var);							\
	if (UNEXPECTED(Z_REFCOUNTED_P(__container_to_free))) {					\
		zend_refcounted *__ref = Z_COUNTED_P(__container_to_free);			\
		if (UNEXPECTED(!GC_DELREF(__ref))) {								\
			zval *__zv = EX_VAR(opline->result.var);						\
			if (EXPECTED(Z_TYPE_P(__zv) == IS_INDIRECT)) {					\
				ZVAL_COPY(__zv, Z_INDIRECT_P(__zv));						\
			}																\
			rc_dtor_func(__ref);											\
		}																	\
	}																		\
} while (0)

#define FREE_OP(type, var) \
	if ((type) & (IS_TMP_VAR|IS_VAR)) { \
		zval_ptr_dtor_nogc(EX_VAR(var)); \
	}

#define CV_DEF_OF(i) (EX(func)->op_array.vars[i])

#define ZEND_VM_STACK_PAGE_SLOTS (16 * 1024) /* should be a power of 2 */

#define ZEND_VM_STACK_PAGE_SIZE  (ZEND_VM_STACK_PAGE_SLOTS * sizeof(zval))

#define ZEND_VM_STACK_PAGE_ALIGNED_SIZE(size, page_size) \
	(((size) + ZEND_VM_STACK_HEADER_SLOTS * sizeof(zval) \
	  + ((page_size) - 1)) & ~((page_size) - 1))

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

ZEND_API bool zend_gcc_global_regs(void)
{
  #if defined(HAVE_GCC_GLOBAL_REGS)
        return 1;
  #else
        return 0;
  #endif
}

static zend_always_inline zval *_get_zval_ptr_tmp(uint32_t var EXECUTE_DATA_DC)
{
	zval *ret = EX_VAR(var);

	ZEND_ASSERT(Z_TYPE_P(ret) != IS_REFERENCE);

	return ret;
}

static zend_always_inline zval *_get_zval_ptr_var(uint32_t var EXECUTE_DATA_DC)
{
	zval *ret = EX_VAR(var);

	return ret;
}

static zend_always_inline zval *_get_zval_ptr_var_deref(uint32_t var EXECUTE_DATA_DC)
{
	zval *ret = EX_VAR(var);

	ZVAL_DEREF(ret);
	return ret;
}

static zend_never_inline ZEND_COLD zval* zval_undefined_cv(uint32_t var EXECUTE_DATA_DC)
{
	if (EXPECTED(EG(exception) == NULL)) {
		zend_string *cv = CV_DEF_OF(EX_VAR_TO_NUM(var));
		zend_error(E_WARNING, "Undefined variable $%s", ZSTR_VAL(cv));
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
			ZEND_FALLTHROUGH;
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
		zval_undefined_cv(var EXECUTE_DATA_CC);
		ZVAL_NULL(ret);
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

static zend_always_inline zval *_get_zval_ptr_tmpvarcv(int op_type, znode_op node, int type EXECUTE_DATA_DC)
{
	if (op_type & (IS_TMP_VAR|IS_VAR)) {
		if (op_type == IS_TMP_VAR) {
			return _get_zval_ptr_tmp(node.var EXECUTE_DATA_CC);
		} else {
			ZEND_ASSERT(op_type == IS_VAR);
			return _get_zval_ptr_var_deref(node.var EXECUTE_DATA_CC);
		}
	} else {
		ZEND_ASSERT(op_type == IS_CV);
		return _get_zval_ptr_cv_deref(node.var, type EXECUTE_DATA_CC);
	}
}

static zend_always_inline zval *_get_zval_ptr(int op_type, znode_op node, int type EXECUTE_DATA_DC OPLINE_DC)
{
	if (op_type & (IS_TMP_VAR|IS_VAR)) {
		if (!ZEND_DEBUG || op_type == IS_VAR) {
			return _get_zval_ptr_var(node.var EXECUTE_DATA_CC);
		} else {
			ZEND_ASSERT(op_type == IS_TMP_VAR);
			return _get_zval_ptr_tmp(node.var EXECUTE_DATA_CC);
		}
	} else {
		if (op_type == IS_CONST) {
			return RT_CONSTANT(opline, node);
		} else if (op_type == IS_CV) {
			return _get_zval_ptr_cv(node.var, type EXECUTE_DATA_CC);
		} else {
			return NULL;
		}
	}
}

static zend_always_inline zval *_get_op_data_zval_ptr_r(int op_type, znode_op node EXECUTE_DATA_DC OPLINE_DC)
{
	if (op_type & (IS_TMP_VAR|IS_VAR)) {
		if (!ZEND_DEBUG || op_type == IS_VAR) {
			return _get_zval_ptr_var(node.var EXECUTE_DATA_CC);
		} else {
			ZEND_ASSERT(op_type == IS_TMP_VAR);
			return _get_zval_ptr_tmp(node.var EXECUTE_DATA_CC);
		}
	} else {
		if (op_type == IS_CONST) {
			return RT_CONSTANT(opline + 1, node);
		} else if (op_type == IS_CV) {
			return _get_zval_ptr_cv_BP_VAR_R(node.var EXECUTE_DATA_CC);
		} else {
			return NULL;
		}
	}
}

static zend_always_inline ZEND_ATTRIBUTE_UNUSED zval *_get_zval_ptr_deref(int op_type, znode_op node, int type EXECUTE_DATA_DC OPLINE_DC)
{
	if (op_type & (IS_TMP_VAR|IS_VAR)) {
		if (op_type == IS_TMP_VAR) {
			return _get_zval_ptr_tmp(node.var EXECUTE_DATA_CC);
		} else {
			ZEND_ASSERT(op_type == IS_VAR);
			return _get_zval_ptr_var_deref(node.var EXECUTE_DATA_CC);
		}
	} else {
		if (op_type == IS_CONST) {
			return RT_CONSTANT(opline, node);
		} else if (op_type == IS_CV) {
			return _get_zval_ptr_cv_deref(node.var, type EXECUTE_DATA_CC);
		} else {
			return NULL;
		}
	}
}

static zend_always_inline ZEND_ATTRIBUTE_UNUSED zval *_get_op_data_zval_ptr_deref_r(int op_type, znode_op node EXECUTE_DATA_DC OPLINE_DC)
{
	if (op_type & (IS_TMP_VAR|IS_VAR)) {
		if (op_type == IS_TMP_VAR) {
			return _get_zval_ptr_tmp(node.var EXECUTE_DATA_CC);
		} else {
			ZEND_ASSERT(op_type == IS_VAR);
			return _get_zval_ptr_var_deref(node.var EXECUTE_DATA_CC);
		}
	} else {
		if (op_type == IS_CONST) {
			return RT_CONSTANT(opline + 1, node);
		} else if (op_type == IS_CV) {
			return _get_zval_ptr_cv_deref_BP_VAR_R(node.var EXECUTE_DATA_CC);
		} else {
			return NULL;
		}
	}
}

static zend_always_inline zval *_get_zval_ptr_undef(int op_type, znode_op node, int type EXECUTE_DATA_DC OPLINE_DC)
{
	if (op_type & (IS_TMP_VAR|IS_VAR)) {
		if (!ZEND_DEBUG || op_type == IS_VAR) {
			return _get_zval_ptr_var(node.var EXECUTE_DATA_CC);
		} else {
			ZEND_ASSERT(op_type == IS_TMP_VAR);
			return _get_zval_ptr_tmp(node.var EXECUTE_DATA_CC);
		}
	} else {
		if (op_type == IS_CONST) {
			return RT_CONSTANT(opline, node);
		} else if (op_type == IS_CV) {
			return EX_VAR(node.var);
		} else {
			return NULL;
		}
	}
}

static zend_always_inline zval *_get_zval_ptr_ptr_var(uint32_t var EXECUTE_DATA_DC)
{
	zval *ret = EX_VAR(var);

	if (EXPECTED(Z_TYPE_P(ret) == IS_INDIRECT)) {
		ret = Z_INDIRECT_P(ret);
	}
	return ret;
}

static inline zval *_get_zval_ptr_ptr(int op_type, znode_op node, int type EXECUTE_DATA_DC)
{
	if (op_type == IS_CV) {
		return _get_zval_ptr_cv(node.var, type EXECUTE_DATA_CC);
	} else /* if (op_type == IS_VAR) */ {
		ZEND_ASSERT(op_type == IS_VAR);
		return _get_zval_ptr_ptr_var(node.var EXECUTE_DATA_CC);
	}
}

static inline ZEND_ATTRIBUTE_UNUSED zval *_get_obj_zval_ptr(int op_type, znode_op op, int type EXECUTE_DATA_DC OPLINE_DC)
{
	if (op_type == IS_UNUSED) {
		return &EX(This);
	}
	return get_zval_ptr(op_type, op, type);
}

static inline ZEND_ATTRIBUTE_UNUSED zval *_get_obj_zval_ptr_undef(int op_type, znode_op op, int type EXECUTE_DATA_DC OPLINE_DC)
{
	if (op_type == IS_UNUSED) {
		return &EX(This);
	}
	return get_zval_ptr_undef(op_type, op, type);
}

static inline ZEND_ATTRIBUTE_UNUSED zval *_get_obj_zval_ptr_ptr(int op_type, znode_op node, int type EXECUTE_DATA_DC)
{
	if (op_type == IS_UNUSED) {
		return &EX(This);
	}
	return get_zval_ptr_ptr(op_type, node, type);
}

static inline void zend_assign_to_variable_reference(zval *variable_ptr, zval *value_ptr, zend_refcounted **garbage_ptr)
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
		*garbage_ptr = Z_COUNTED_P(variable_ptr);
	}
	ZVAL_REF(variable_ptr, ref);
}

static zend_never_inline zval* zend_assign_to_typed_property_reference(zend_property_info *prop_info, zval *prop, zval *value_ptr, zend_refcounted **garbage_ptr EXECUTE_DATA_DC)
{
	if (!zend_verify_prop_assignable_by_ref(prop_info, value_ptr, EX_USES_STRICT_TYPES())) {
		return &EG(uninitialized_zval);
	}
	if (Z_ISREF_P(prop)) {
		ZEND_REF_DEL_TYPE_SOURCE(Z_REF_P(prop), prop_info);
	}
	zend_assign_to_variable_reference(prop, value_ptr, garbage_ptr);
	ZEND_REF_ADD_TYPE_SOURCE(Z_REF_P(prop), prop_info);
	return prop;
}

static zend_never_inline ZEND_COLD zval *zend_wrong_assign_to_variable_reference(zval *variable_ptr, zval *value_ptr, zend_refcounted **garbage_ptr OPLINE_DC EXECUTE_DATA_DC)
{
	zend_error(E_NOTICE, "Only variables should be assigned by reference");
	if (UNEXPECTED(EG(exception) != NULL)) {
		return &EG(uninitialized_zval);
	}

	/* Use IS_TMP_VAR instead of IS_VAR to avoid ISREF check */
	Z_TRY_ADDREF_P(value_ptr);
	return zend_assign_to_variable_ex(variable_ptr, value_ptr, IS_TMP_VAR, EX_USES_STRICT_TYPES(), garbage_ptr);
}

ZEND_API ZEND_COLD void ZEND_FASTCALL zend_cannot_pass_by_reference(uint32_t arg_num)
{
	const zend_execute_data *execute_data = EG(current_execute_data);
	zend_string *func_name = get_function_or_method_name(EX(call)->func);
	const char *param_name = get_function_arg_name(EX(call)->func, arg_num);

	zend_throw_error(NULL, "%s(): Argument #%d%s%s%s could not be passed by reference",
		ZSTR_VAL(func_name), arg_num, param_name ? " ($" : "", param_name ? param_name : "", param_name ? ")" : ""
	);

	zend_string_release(func_name);
}

static zend_never_inline ZEND_COLD void zend_throw_auto_init_in_prop_error(zend_property_info *prop) {
	zend_string *type_str = zend_type_to_string(prop->type);
	zend_type_error(
		"Cannot auto-initialize an array inside property %s::$%s of type %s",
		ZSTR_VAL(prop->ce->name), zend_get_unmangled_property_name(prop->name),
		ZSTR_VAL(type_str)
	);
	zend_string_release(type_str);
}

static zend_never_inline ZEND_COLD void zend_throw_auto_init_in_ref_error(zend_property_info *prop) {
	zend_string *type_str = zend_type_to_string(prop->type);
	zend_type_error(
		"Cannot auto-initialize an array inside a reference held by property %s::$%s of type %s",
		ZSTR_VAL(prop->ce->name), zend_get_unmangled_property_name(prop->name),
		ZSTR_VAL(type_str)
	);
	zend_string_release(type_str);
}

static zend_never_inline ZEND_COLD void zend_throw_access_uninit_prop_by_ref_error(
		zend_property_info *prop) {
	zend_throw_error(NULL,
		"Cannot access uninitialized non-nullable property %s::$%s by reference",
		ZSTR_VAL(prop->ce->name),
		zend_get_unmangled_property_name(prop->name));
}

/* this should modify object only if it's empty */
static zend_never_inline ZEND_COLD void ZEND_FASTCALL zend_throw_non_object_error(zval *object, zval *property OPLINE_DC EXECUTE_DATA_DC)
{
	zend_string *tmp_property_name;
	zend_string *property_name = zval_get_tmp_string(property, &tmp_property_name);

	if (opline->opcode == ZEND_PRE_INC_OBJ
	 || opline->opcode == ZEND_PRE_DEC_OBJ
	 || opline->opcode == ZEND_POST_INC_OBJ
	 || opline->opcode == ZEND_POST_DEC_OBJ) {
		zend_throw_error(NULL,
			"Attempt to increment/decrement property \"%s\" on %s",
			ZSTR_VAL(property_name), zend_zval_value_name(object)
		);
	} else if (opline->opcode == ZEND_FETCH_OBJ_W
			|| opline->opcode == ZEND_FETCH_OBJ_RW
			|| opline->opcode == ZEND_FETCH_OBJ_FUNC_ARG
			|| opline->opcode == ZEND_ASSIGN_OBJ_REF) {
		zend_throw_error(NULL,
			"Attempt to modify property \"%s\" on %s",
			ZSTR_VAL(property_name), zend_zval_value_name(object)
		);
	} else {
		zend_throw_error(NULL,
			"Attempt to assign property \"%s\" on %s",
			ZSTR_VAL(property_name), zend_zval_value_name(object)
		);
	}
	zend_tmp_string_release(tmp_property_name);

	if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
		ZVAL_NULL(EX_VAR(opline->result.var));
	}
}

static ZEND_COLD void zend_verify_type_error_common(
		const zend_function *zf, const zend_arg_info *arg_info, zval *value,
		const char **fname, const char **fsep, const char **fclass,
		zend_string **need_msg, const char **given_kind)
{
	*fname = ZSTR_VAL(zf->common.function_name);
	if (zf->common.scope) {
		*fsep =  "::";
		*fclass = ZSTR_VAL(zf->common.scope->name);
	} else {
		*fsep =  "";
		*fclass = "";
	}

	*need_msg = zend_type_to_string_resolved(arg_info->type, zf->common.scope);

	if (value) {
		*given_kind = zend_zval_value_name(value);
	} else {
		*given_kind = "none";
	}
}

ZEND_API ZEND_COLD void zend_verify_arg_error(
		const zend_function *zf, const zend_arg_info *arg_info, uint32_t arg_num, zval *value)
{
	zend_execute_data *ptr = EG(current_execute_data)->prev_execute_data;
	const char *fname, *fsep, *fclass;
	zend_string *need_msg;
	const char *given_msg;

	zend_verify_type_error_common(
		zf, arg_info, value, &fname, &fsep, &fclass, &need_msg, &given_msg);

	ZEND_ASSERT(zf->common.type == ZEND_USER_FUNCTION
		&& "Arginfo verification is not performed for internal functions");
	if (ptr && ptr->func && ZEND_USER_CODE(ptr->func->common.type)) {
		zend_argument_type_error(arg_num, "must be of type %s, %s given, called in %s on line %d",
			ZSTR_VAL(need_msg), given_msg,
			ZSTR_VAL(ptr->func->op_array.filename), ptr->opline->lineno
		);
	} else {
		zend_argument_type_error(arg_num,
			"must be of type %s, %s given", ZSTR_VAL(need_msg), given_msg);
	}

	zend_string_release(need_msg);
}

static bool zend_verify_weak_scalar_type_hint(uint32_t type_mask, zval *arg)
{
	zend_long lval;
	double dval;
	zend_string *str;
	bool bval;

	/* Type preference order: int -> float -> string -> bool */
	if (type_mask & MAY_BE_LONG) {
		/* For an int|float union type and string value,
		 * determine chosen type by is_numeric_string() semantics. */
		if ((type_mask & MAY_BE_DOUBLE) && Z_TYPE_P(arg) == IS_STRING) {
			uint8_t type = is_numeric_str_function(Z_STR_P(arg), &lval, &dval);
			if (type == IS_LONG) {
				zend_string_release(Z_STR_P(arg));
				ZVAL_LONG(arg, lval);
				return 1;
			}
			if (type == IS_DOUBLE) {
				zend_string_release(Z_STR_P(arg));
				ZVAL_DOUBLE(arg, dval);
				return 1;
			}
		} else if (zend_parse_arg_long_weak(arg, &lval, 0)) {
			zval_ptr_dtor(arg);
			ZVAL_LONG(arg, lval);
			return 1;
		} else if (UNEXPECTED(EG(exception))) {
			return 0;
		}
	}
	if ((type_mask & MAY_BE_DOUBLE) && zend_parse_arg_double_weak(arg, &dval, 0)) {
		zval_ptr_dtor(arg);
		ZVAL_DOUBLE(arg, dval);
		return 1;
	}
	if ((type_mask & MAY_BE_STRING) && zend_parse_arg_str_weak(arg, &str, 0)) {
		/* on success "arg" is converted to IS_STRING */
		return 1;
	}
	if ((type_mask & MAY_BE_BOOL) == MAY_BE_BOOL && zend_parse_arg_bool_weak(arg, &bval, 0)) {
		zval_ptr_dtor(arg);
		ZVAL_BOOL(arg, bval);
		return 1;
	}
	return 0;
}

#if ZEND_DEBUG
static bool can_convert_to_string(const zval *zv) {
	/* We don't call cast_object here, because this check must be side-effect free. As this
	 * is only used for a sanity check of arginfo/zpp consistency, it's okay if we accept
	 * more than actually allowed here. */
	if (Z_TYPE_P(zv) == IS_OBJECT) {
		return Z_OBJ_HT_P(zv)->cast_object != zend_std_cast_object_tostring
			|| Z_OBJCE_P(zv)->__tostring;
	}
	return Z_TYPE_P(zv) <= IS_STRING;
}

/* Used to sanity-check internal arginfo types without performing any actual type conversions. */
static bool zend_verify_weak_scalar_type_hint_no_sideeffect(uint32_t type_mask, const zval *arg)
{
	zend_long lval;
	double dval;
	bool bval;

	/* Pass (uint32_t)-1 as arg_num to indicate to ZPP not to emit any deprecation notice,
	 * this is needed because the version with side effects also uses 0 (e.g. for typed properties) */
	if ((type_mask & MAY_BE_LONG) && zend_parse_arg_long_weak(arg, &lval, (uint32_t)-1)) {
		return 1;
	}
	if ((type_mask & MAY_BE_DOUBLE) && zend_parse_arg_double_weak(arg, &dval, (uint32_t)-1)) {
		return 1;
	}
	if ((type_mask & MAY_BE_STRING) && can_convert_to_string(arg)) {
		return 1;
	}
	if ((type_mask & MAY_BE_BOOL) == MAY_BE_BOOL && zend_parse_arg_bool_weak(arg, &bval, (uint32_t)-1)) {
		return 1;
	}
	return 0;
}
#endif

ZEND_API bool zend_verify_scalar_type_hint(uint32_t type_mask, zval *arg, bool strict, bool is_internal_arg)
{
	if (UNEXPECTED(strict)) {
		/* SSTH Exception: IS_LONG may be accepted as IS_DOUBLE (converted) */
		if (!(type_mask & MAY_BE_DOUBLE) || Z_TYPE_P(arg) != IS_LONG) {
			return 0;
		}
	} else if (UNEXPECTED(Z_TYPE_P(arg) == IS_NULL)) {
		/* NULL may be accepted only by nullable hints (this is already checked).
		 * As an exception for internal functions, null is allowed for scalar types in weak mode. */
		return is_internal_arg
			&& (type_mask & (MAY_BE_TRUE|MAY_BE_FALSE|MAY_BE_LONG|MAY_BE_DOUBLE|MAY_BE_STRING));
	}
#if ZEND_DEBUG
	if (is_internal_arg) {
		return zend_verify_weak_scalar_type_hint_no_sideeffect(type_mask, arg);
	}
#endif
	return zend_verify_weak_scalar_type_hint(type_mask, arg);
}

ZEND_COLD zend_never_inline void zend_verify_class_constant_type_error(const zend_class_constant *c, const zend_string *name, const zval *constant)
{
	zend_string *type_str = zend_type_to_string(c->type);

	zend_type_error("Cannot assign %s to class constant %s::%s of type %s",
		zend_zval_type_name(constant), ZSTR_VAL(c->ce->name), ZSTR_VAL(name), ZSTR_VAL(type_str));

	zend_string_release(type_str);
}

ZEND_COLD zend_never_inline void zend_verify_property_type_error(const zend_property_info *info, const zval *property)
{
	zend_string *type_str;

	/* we _may_ land here in case reading already errored and runtime cache thus has not been updated (i.e. it contains a valid but unrelated info) */
	if (EG(exception)) {
		return;
	}

	type_str = zend_type_to_string(info->type);
	zend_type_error("Cannot assign %s to property %s::$%s of type %s",
		zend_zval_value_name(property),
		ZSTR_VAL(info->ce->name),
		zend_get_unmangled_property_name(info->name),
		ZSTR_VAL(type_str));
	zend_string_release(type_str);
}

ZEND_COLD zend_never_inline void zend_magic_get_property_type_inconsistency_error(const zend_property_info *info, const zval *property)
{
	/* we _may_ land here in case reading already errored and runtime cache thus has not been updated (i.e. it contains a valid but unrelated info) */
	if (EG(exception)) {
		return;
	}

	zend_string *type_str = zend_type_to_string(info->type);
	zend_type_error("Value of type %s returned from %s::__get() must be compatible with unset property %s::$%s of type %s",
		zend_zval_type_name(property),
		ZSTR_VAL(info->ce->name),
		ZSTR_VAL(info->ce->name),
		zend_get_unmangled_property_name(info->name),
		ZSTR_VAL(type_str));
	zend_string_release(type_str);
}

ZEND_COLD void zend_match_unhandled_error(const zval *value)
{
	smart_str msg = {0};

	if (Z_TYPE_P(value) <= IS_STRING) {
		smart_str_append_scalar(&msg, value, EG(exception_string_param_max_len));
	} else {
		smart_str_appendl(&msg, "of type ", sizeof("of type ")-1);
		smart_str_appends(&msg, zend_zval_type_name(value));
	}

	smart_str_0(&msg);

	zend_throw_exception_ex(
		zend_ce_unhandled_match_error, 0, "Unhandled match case %s", ZSTR_VAL(msg.s));

	smart_str_free(&msg);
}

ZEND_API ZEND_COLD void ZEND_FASTCALL zend_readonly_property_modification_error(
		const zend_property_info *info) {
	zend_throw_error(NULL, "Cannot modify readonly property %s::$%s",
		ZSTR_VAL(info->ce->name), zend_get_unmangled_property_name(info->name));
}

ZEND_API ZEND_COLD void ZEND_FASTCALL zend_readonly_property_indirect_modification_error(const zend_property_info *info)
{
	zend_throw_error(NULL, "Cannot indirectly modify readonly property %s::$%s",
		ZSTR_VAL(info->ce->name), zend_get_unmangled_property_name(info->name));
}

ZEND_API ZEND_COLD void ZEND_FASTCALL zend_invalid_class_constant_type_error(uint8_t type)
{
	zend_type_error("Cannot use value of type %s as class constant name", zend_get_type_by_const(type));
}

ZEND_API ZEND_COLD void ZEND_FASTCALL zend_object_released_while_assigning_to_property_error(const zend_property_info *info)
{
	zend_throw_error(NULL, "Object was released while assigning to property %s::$%s",
		ZSTR_VAL(info->ce->name), zend_get_unmangled_property_name(info->name));
}

static const zend_class_entry *resolve_single_class_type(zend_string *name, const zend_class_entry *self_ce) {
	if (zend_string_equals_literal_ci(name, "self")) {
		return self_ce;
	} else if (zend_string_equals_literal_ci(name, "parent")) {
		return self_ce->parent;
	} else {
		return zend_lookup_class_ex(name, NULL, ZEND_FETCH_CLASS_NO_AUTOLOAD);
	}
}

static zend_always_inline const zend_class_entry *zend_ce_from_type(
		const zend_class_entry *scope, const zend_type *type) {
	ZEND_ASSERT(ZEND_TYPE_HAS_NAME(*type));
	zend_string *name = ZEND_TYPE_NAME(*type);
	if (ZSTR_HAS_CE_CACHE(name)) {
		zend_class_entry *ce = ZSTR_GET_CE_CACHE(name);
		if (!ce) {
			ce = zend_lookup_class_ex(name, NULL, ZEND_FETCH_CLASS_NO_AUTOLOAD);
		}
		return ce;
	}
	return resolve_single_class_type(name, scope);
}

static bool zend_check_intersection_for_property_or_class_constant_class_type(
	const zend_class_entry *scope, zend_type_list *intersection_type_list, const zend_class_entry *value_ce)
{
	zend_type *list_type;

	ZEND_TYPE_LIST_FOREACH(intersection_type_list, list_type) {
		ZEND_ASSERT(!ZEND_TYPE_HAS_LIST(*list_type));
		const zend_class_entry *ce = zend_ce_from_type(scope, list_type);
		if (!ce || !instanceof_function(value_ce, ce)) {
			return false;
		}
	} ZEND_TYPE_LIST_FOREACH_END();
	return true;
}

static bool zend_check_and_resolve_property_or_class_constant_class_type(
	const zend_class_entry *scope, zend_type member_type, const zend_class_entry *value_ce) {
	if (ZEND_TYPE_HAS_LIST(member_type)) {
		zend_type *list_type;
		if (ZEND_TYPE_IS_INTERSECTION(member_type)) {
			return zend_check_intersection_for_property_or_class_constant_class_type(
				scope, ZEND_TYPE_LIST(member_type), value_ce);
		} else {
			ZEND_TYPE_LIST_FOREACH(ZEND_TYPE_LIST(member_type), list_type) {
				if (ZEND_TYPE_IS_INTERSECTION(*list_type)) {
					if (zend_check_intersection_for_property_or_class_constant_class_type(
							scope, ZEND_TYPE_LIST(*list_type), value_ce)) {
						return true;
					}
					continue;
				}
				ZEND_ASSERT(!ZEND_TYPE_HAS_LIST(*list_type));
				const zend_class_entry *ce = zend_ce_from_type(scope, list_type);
				if (ce && instanceof_function(value_ce, ce)) {
					return true;
				}
			} ZEND_TYPE_LIST_FOREACH_END();

			if ((ZEND_TYPE_PURE_MASK(member_type) & MAY_BE_STATIC)) {
				return value_ce == scope;
			}

			return false;
		}
	} else if ((ZEND_TYPE_PURE_MASK(member_type) & MAY_BE_STATIC) && value_ce == scope) {
		return true;
	} else if (ZEND_TYPE_HAS_NAME(member_type)) {
		const zend_class_entry *ce = zend_ce_from_type(scope, &member_type);
		return ce && instanceof_function(value_ce, ce);
	}

	return false;
}

static zend_always_inline bool i_zend_check_property_type(const zend_property_info *info, zval *property, bool strict)
{
	ZEND_ASSERT(!Z_ISREF_P(property));
	if (EXPECTED(ZEND_TYPE_CONTAINS_CODE(info->type, Z_TYPE_P(property)))) {
		return 1;
	}

	if (ZEND_TYPE_IS_COMPLEX(info->type) && Z_TYPE_P(property) == IS_OBJECT
			&& zend_check_and_resolve_property_or_class_constant_class_type(info->ce, info->type, Z_OBJCE_P(property))) {
		return 1;
	}

	uint32_t type_mask = ZEND_TYPE_FULL_MASK(info->type);
	ZEND_ASSERT(!(type_mask & (MAY_BE_CALLABLE|MAY_BE_STATIC|MAY_BE_NEVER|MAY_BE_VOID)));
	return zend_verify_scalar_type_hint(type_mask, property, strict, 0);
}

static zend_always_inline bool i_zend_verify_property_type(const zend_property_info *info, zval *property, bool strict)
{
	if (i_zend_check_property_type(info, property, strict)) {
		return 1;
	}

	zend_verify_property_type_error(info, property);
	return 0;
}

ZEND_API bool zend_never_inline zend_verify_property_type(const zend_property_info *info, zval *property, bool strict) {
	return i_zend_verify_property_type(info, property, strict);
}

static zend_never_inline zval* zend_assign_to_typed_prop(zend_property_info *info, zval *property_val, zval *value, zend_refcounted **garbage_ptr EXECUTE_DATA_DC)
{
	zval tmp;

	if (UNEXPECTED((info->flags & ZEND_ACC_READONLY) && !(Z_PROP_FLAG_P(property_val) & IS_PROP_REINITABLE))) {
		zend_readonly_property_modification_error(info);
		return &EG(uninitialized_zval);
	}

	ZVAL_DEREF(value);
	ZVAL_COPY(&tmp, value);

	if (UNEXPECTED(!i_zend_verify_property_type(info, &tmp, EX_USES_STRICT_TYPES()))) {
		zval_ptr_dtor(&tmp);
		return &EG(uninitialized_zval);
	}

	Z_PROP_FLAG_P(property_val) &= ~IS_PROP_REINITABLE;

	return zend_assign_to_variable_ex(property_val, &tmp, IS_TMP_VAR, EX_USES_STRICT_TYPES(), garbage_ptr);
}

static zend_always_inline bool zend_value_instanceof_static(zval *zv) {
	if (Z_TYPE_P(zv) != IS_OBJECT) {
		return 0;
	}

	zend_class_entry *called_scope = zend_get_called_scope(EG(current_execute_data));
	if (!called_scope) {
		return 0;
	}
	return instanceof_function(Z_OBJCE_P(zv), called_scope);
}

/* The cache_slot may only be NULL in debug builds, where arginfo verification of
 * internal functions is enabled. Avoid unnecessary checks in release builds. */
#if ZEND_DEBUG
# define HAVE_CACHE_SLOT (cache_slot != NULL)
#else
# define HAVE_CACHE_SLOT 1
#endif

#define PROGRESS_CACHE_SLOT() if (HAVE_CACHE_SLOT) {cache_slot++;}

static zend_always_inline zend_class_entry *zend_fetch_ce_from_cache_slot(
		void **cache_slot, zend_type *type)
{
	if (EXPECTED(HAVE_CACHE_SLOT && *cache_slot)) {
		return (zend_class_entry *) *cache_slot;
	}

	zend_string *name = ZEND_TYPE_NAME(*type);
	zend_class_entry *ce;
	if (ZSTR_HAS_CE_CACHE(name)) {
		ce = ZSTR_GET_CE_CACHE(name);
		if (!ce) {
			ce = zend_lookup_class_ex(name, NULL, ZEND_FETCH_CLASS_NO_AUTOLOAD);
			if (UNEXPECTED(!ce)) {
				/* Cannot resolve */
				return NULL;
			}
		}
	} else {
		ce = zend_fetch_class(name,
			ZEND_FETCH_CLASS_AUTO | ZEND_FETCH_CLASS_NO_AUTOLOAD | ZEND_FETCH_CLASS_SILENT);
		if (UNEXPECTED(!ce)) {
			return NULL;
		}
	}
	if (HAVE_CACHE_SLOT) {
		*cache_slot = (void *) ce;
	}
	return ce;
}

static bool zend_check_intersection_type_from_cache_slot(zend_type_list *intersection_type_list,
	zend_class_entry *arg_ce, void ***cache_slot_ptr)
{
	void **cache_slot = *cache_slot_ptr;
	zend_class_entry *ce;
	zend_type *list_type;
	bool status = true;
	ZEND_TYPE_LIST_FOREACH(intersection_type_list, list_type) {
		/* Only check classes if the type might be valid */
		if (status) {
			ce = zend_fetch_ce_from_cache_slot(cache_slot, list_type);
			/* If type is not an instance of one of the types taking part in the
			 * intersection it cannot be a valid instance of the whole intersection type. */
			if (!ce || !instanceof_function(arg_ce, ce)) {
				status = false;
			}
		}
		PROGRESS_CACHE_SLOT();
	} ZEND_TYPE_LIST_FOREACH_END();
	if (HAVE_CACHE_SLOT) {
		*cache_slot_ptr = cache_slot;
	}
	return status;
}

static zend_always_inline bool zend_check_type_slow(
		zend_type *type, zval *arg, zend_reference *ref, void **cache_slot,
		bool is_return_type, bool is_internal)
{
	uint32_t type_mask;
	if (ZEND_TYPE_IS_COMPLEX(*type) && EXPECTED(Z_TYPE_P(arg) == IS_OBJECT)) {
		zend_class_entry *ce;
		if (UNEXPECTED(ZEND_TYPE_HAS_LIST(*type))) {
			zend_type *list_type;
			if (ZEND_TYPE_IS_INTERSECTION(*type)) {
				return zend_check_intersection_type_from_cache_slot(ZEND_TYPE_LIST(*type), Z_OBJCE_P(arg), &cache_slot);
			} else {
				ZEND_TYPE_LIST_FOREACH(ZEND_TYPE_LIST(*type), list_type) {
					if (ZEND_TYPE_IS_INTERSECTION(*list_type)) {
						if (zend_check_intersection_type_from_cache_slot(ZEND_TYPE_LIST(*list_type), Z_OBJCE_P(arg), &cache_slot)) {
							return true;
						}
						/* The cache_slot is progressed in zend_check_intersection_type_from_cache_slot() */
					} else {
						ZEND_ASSERT(!ZEND_TYPE_HAS_LIST(*list_type));
						ce = zend_fetch_ce_from_cache_slot(cache_slot, list_type);
						/* Instance of a single type part of a union is sufficient to pass the type check */
						if (ce && instanceof_function(Z_OBJCE_P(arg), ce)) {
							return true;
						}
						PROGRESS_CACHE_SLOT();
					}
				} ZEND_TYPE_LIST_FOREACH_END();
			}
		} else {
			ce = zend_fetch_ce_from_cache_slot(cache_slot, type);
			/* If we have a CE we check if it satisfies the type constraint,
			 * otherwise it will check if a standard type satisfies it. */
			if (ce && instanceof_function(Z_OBJCE_P(arg), ce)) {
				return true;
			}
		}
	}

	type_mask = ZEND_TYPE_FULL_MASK(*type);
	if ((type_mask & MAY_BE_CALLABLE) &&
		zend_is_callable(arg, is_internal ? IS_CALLABLE_SUPPRESS_DEPRECATIONS : 0, NULL)) {
		return 1;
	}
	if ((type_mask & MAY_BE_STATIC) && zend_value_instanceof_static(arg)) {
		return 1;
	}
	if (ref && ZEND_REF_HAS_TYPE_SOURCES(ref)) {
		/* We cannot have conversions for typed refs. */
		return 0;
	}
	if (is_internal && is_return_type) {
		/* For internal returns, the type has to match exactly, because we're not
		 * going to check it for non-debug builds, and there will be no chance to
		 * apply coercions. */
		return 0;
	}

	return zend_verify_scalar_type_hint(type_mask, arg,
		is_return_type ? ZEND_RET_USES_STRICT_TYPES() : ZEND_ARG_USES_STRICT_TYPES(),
		is_internal);

	/* Special handling for IS_VOID is not necessary (for return types),
	 * because this case is already checked at compile-time. */
}

static zend_always_inline bool zend_check_type(
		zend_type *type, zval *arg, void **cache_slot, zend_class_entry *scope,
		bool is_return_type, bool is_internal)
{
	zend_reference *ref = NULL;
	ZEND_ASSERT(ZEND_TYPE_IS_SET(*type));

	if (UNEXPECTED(Z_ISREF_P(arg))) {
		ref = Z_REF_P(arg);
		arg = Z_REFVAL_P(arg);
	}

	if (EXPECTED(ZEND_TYPE_CONTAINS_CODE(*type, Z_TYPE_P(arg)))) {
		return 1;
	}

	return zend_check_type_slow(type, arg, ref, cache_slot, is_return_type, is_internal);
}

ZEND_API bool zend_check_user_type_slow(
		zend_type *type, zval *arg, zend_reference *ref, void **cache_slot, bool is_return_type)
{
	return zend_check_type_slow(
		type, arg, ref, cache_slot, is_return_type, /* is_internal */ false);
}

static zend_always_inline bool zend_verify_recv_arg_type(zend_function *zf, uint32_t arg_num, zval *arg, void **cache_slot)
{
	zend_arg_info *cur_arg_info;

	ZEND_ASSERT(arg_num <= zf->common.num_args);
	cur_arg_info = &zf->common.arg_info[arg_num-1];

	if (ZEND_TYPE_IS_SET(cur_arg_info->type)
			&& UNEXPECTED(!zend_check_type(&cur_arg_info->type, arg, cache_slot, zf->common.scope, 0, 0))) {
		zend_verify_arg_error(zf, cur_arg_info, arg_num, arg);
		return 0;
	}

	return 1;
}

static zend_always_inline bool zend_verify_variadic_arg_type(
		zend_function *zf, zend_arg_info *arg_info, uint32_t arg_num, zval *arg, void **cache_slot)
{
	ZEND_ASSERT(ZEND_TYPE_IS_SET(arg_info->type));
	if (UNEXPECTED(!zend_check_type(&arg_info->type, arg, cache_slot, zf->common.scope, 0, 0))) {
		zend_verify_arg_error(zf, arg_info, arg_num, arg);
		return 0;
	}

	return 1;
}

static zend_never_inline ZEND_ATTRIBUTE_UNUSED bool zend_verify_internal_arg_types(zend_function *fbc, zend_execute_data *call)
{
	uint32_t i;
	uint32_t num_args = ZEND_CALL_NUM_ARGS(call);
	zval *arg = ZEND_CALL_ARG(call, 1);

	for (i = 0; i < num_args; ++i) {
		zend_arg_info *cur_arg_info;
		if (EXPECTED(i < fbc->common.num_args)) {
			cur_arg_info = &fbc->common.arg_info[i];
		} else if (UNEXPECTED(fbc->common.fn_flags & ZEND_ACC_VARIADIC)) {
			cur_arg_info = &fbc->common.arg_info[fbc->common.num_args];
		} else {
			break;
		}

		if (ZEND_TYPE_IS_SET(cur_arg_info->type)
				&& UNEXPECTED(!zend_check_type(&cur_arg_info->type, arg, /* cache_slot */ NULL, fbc->common.scope, 0, /* is_internal */ 1))) {
			return 0;
		}
		arg++;
	}
	return 1;
}

#if ZEND_DEBUG
/* Determine whether an internal call should throw, because the passed arguments violate
 * an arginfo constraint. This is only checked in debug builds. In release builds, we
 * trust that arginfo matches what is enforced by zend_parse_parameters. */
ZEND_API bool zend_internal_call_should_throw(zend_function *fbc, zend_execute_data *call)
{
	if (fbc->internal_function.handler == ZEND_FN(pass) || (fbc->internal_function.fn_flags & ZEND_ACC_FAKE_CLOSURE)) {
		/* Be lenient about the special pass function and about fake closures. */
		return 0;
	}

	if (fbc->common.required_num_args > ZEND_CALL_NUM_ARGS(call)) {
		/* Required argument not passed. */
		return 1;
	}

	if (fbc->common.num_args < ZEND_CALL_NUM_ARGS(call)
			&& !(fbc->common.fn_flags & ZEND_ACC_VARIADIC)) {
		/* Too many arguments passed. For internal functions (unlike userland functions),
		 * this should always throw. */
		return 1;
	}

	if ((fbc->common.fn_flags & ZEND_ACC_HAS_TYPE_HINTS) &&
			!zend_verify_internal_arg_types(fbc, call)) {
		return 1;
	}

	return 0;
}

ZEND_API ZEND_COLD void zend_internal_call_arginfo_violation(zend_function *fbc)
{
	zend_error_noreturn(E_ERROR, "Arginfo / zpp mismatch during call of %s%s%s()",
		fbc->common.scope ? ZSTR_VAL(fbc->common.scope->name) : "",
		fbc->common.scope ? "::" : "",
		ZSTR_VAL(fbc->common.function_name));
}

#ifndef ZEND_VERIFY_FUNC_INFO
# define ZEND_VERIFY_FUNC_INFO 0
#endif

static void zend_verify_internal_func_info(zend_function *fn, zval *retval) {
#if ZEND_VERIFY_FUNC_INFO
	zend_string *name = fn->common.function_name;
	uint32_t type_mask = zend_get_internal_func_info(fn, NULL, NULL);
	if (!type_mask) {
		return;
	}

	/* Always check refcount of arrays, as immutable arrays are RCN. */
	if (Z_REFCOUNTED_P(retval) || Z_TYPE_P(retval) == IS_ARRAY) {
		if (!(type_mask & MAY_BE_RC1)) {
			zend_error_noreturn(E_CORE_ERROR, "%s() missing rc1", ZSTR_VAL(name));
		}
		if (Z_REFCOUNT_P(retval) > 1 && !(type_mask & MAY_BE_RCN)) {
			zend_error_noreturn(E_CORE_ERROR, "%s() missing rcn", ZSTR_VAL(name));
		}
	}

	uint32_t type = 1u << Z_TYPE_P(retval);
	if (!(type_mask & type)) {
		zend_error_noreturn(E_CORE_ERROR, "%s() missing type %s",
			ZSTR_VAL(name), zend_get_type_by_const(Z_TYPE_P(retval)));
	}

	if (Z_TYPE_P(retval) == IS_ARRAY) {
		HashTable *ht = Z_ARRVAL_P(retval);
		uint32_t num_checked = 0;
		zend_string *str;
		zval *val;
		ZEND_HASH_FOREACH_STR_KEY_VAL(ht, str, val) {
			if (str) {
				if (!(type_mask & MAY_BE_ARRAY_KEY_STRING)) {
					zend_error_noreturn(E_CORE_ERROR,
						"%s() missing array_key_string", ZSTR_VAL(name));
				}
			} else {
				if (!(type_mask & MAY_BE_ARRAY_KEY_LONG)) {
					zend_error_noreturn(E_CORE_ERROR,
						"%s() missing array_key_long", ZSTR_VAL(name));
				}
			}

			uint32_t array_type = 1u << (Z_TYPE_P(val) + MAY_BE_ARRAY_SHIFT);
			if (!(type_mask & array_type)) {
				zend_error_noreturn(E_CORE_ERROR,
					"%s() missing array element type %s",
					ZSTR_VAL(name), zend_get_type_by_const(Z_TYPE_P(retval)));
			}

			/* Don't check all elements of large arrays. */
			if (++num_checked > 16) {
				break;
			}
		} ZEND_HASH_FOREACH_END();
	}
#endif
}
#endif

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

ZEND_API ZEND_COLD void zend_verify_return_error(const zend_function *zf, zval *value)
{
	const zend_arg_info *arg_info = &zf->common.arg_info[-1];
	const char *fname, *fsep, *fclass;
	zend_string *need_msg;
	const char *given_msg;

	zend_verify_type_error_common(
		zf, arg_info, value, &fname, &fsep, &fclass, &need_msg, &given_msg);

	zend_type_error("%s%s%s(): Return value must be of type %s, %s returned",
		fclass, fsep, fname, ZSTR_VAL(need_msg), given_msg);

	zend_string_release(need_msg);
}

ZEND_API ZEND_COLD void zend_verify_never_error(const zend_function *zf)
{
	zend_string *func_name = get_function_or_method_name(zf);

	zend_type_error("%s(): never-returning %s must not implicitly return",
		ZSTR_VAL(func_name), zf->common.scope ? "method" : "function");

	zend_string_release(func_name);
}

#if ZEND_DEBUG
static ZEND_COLD void zend_verify_internal_return_error(const zend_function *zf, zval *value)
{
	const zend_arg_info *arg_info = &zf->common.arg_info[-1];
	const char *fname, *fsep, *fclass;
	zend_string *need_msg;
	const char *given_msg;

	zend_verify_type_error_common(
		zf, arg_info, value, &fname, &fsep, &fclass, &need_msg, &given_msg);

	zend_error_noreturn(E_CORE_ERROR, "%s%s%s(): Return value must be of type %s, %s returned",
		fclass, fsep, fname, ZSTR_VAL(need_msg), given_msg);
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

ZEND_API bool zend_verify_internal_return_type(zend_function *zf, zval *ret)
{
	zend_internal_arg_info *ret_info = zf->internal_function.arg_info - 1;

	if (ZEND_TYPE_FULL_MASK(ret_info->type) & MAY_BE_VOID) {
		if (UNEXPECTED(Z_TYPE_P(ret) != IS_NULL)) {
			zend_verify_void_return_error(zf, zend_zval_value_name(ret), "");
			return 0;
		}
		return 1;
	}

	if (UNEXPECTED(!zend_check_type(&ret_info->type, ret, /* cache_slot */ NULL, NULL, 1, /* is_internal */ 1))) {
		zend_verify_internal_return_error(zf, ret);
		return 0;
	}

	return 1;
}
#endif

static ZEND_COLD void zend_verify_missing_return_type(const zend_function *zf)
{
	/* VERIFY_RETURN_TYPE is not emitted for "void" functions, so this is always an error. */
	zend_verify_return_error(zf, NULL);
}

static zend_always_inline bool zend_check_class_constant_type(zend_class_constant *c, zval *constant)
{
	ZEND_ASSERT(!Z_ISREF_P(constant));
	if (EXPECTED(ZEND_TYPE_CONTAINS_CODE(c->type, Z_TYPE_P(constant)))) {
		return 1;
	}

	if (((ZEND_TYPE_PURE_MASK(c->type) & MAY_BE_STATIC) || ZEND_TYPE_IS_COMPLEX(c->type)) && Z_TYPE_P(constant) == IS_OBJECT
		&& zend_check_and_resolve_property_or_class_constant_class_type(c->ce, c->type, Z_OBJCE_P(constant))) {
		return 1;
	}

	uint32_t type_mask = ZEND_TYPE_FULL_MASK(c->type);
	ZEND_ASSERT(!(type_mask & (MAY_BE_CALLABLE|MAY_BE_NEVER|MAY_BE_VOID)));
	return zend_verify_scalar_type_hint(type_mask, constant, true, false);
}

ZEND_API bool zend_never_inline zend_verify_class_constant_type(zend_class_constant *c, const zend_string *name, zval *constant)
{
	if (!zend_check_class_constant_type(c, constant)) {
		zend_verify_class_constant_type_error(c, name, constant);
		return 0;
	}

	return 1;
}

static zend_never_inline ZEND_COLD void ZEND_FASTCALL zend_use_object_as_array(const zend_object *object)
{
	zend_throw_error(NULL, "Cannot use object of type %s as array", ZSTR_VAL(object->ce->name));
}

static zend_never_inline ZEND_COLD void ZEND_FASTCALL zend_illegal_array_offset_access(const zval *offset)
{
	zend_illegal_container_offset(ZSTR_KNOWN(ZEND_STR_ARRAY), offset, BP_VAR_RW);
}

static zend_never_inline ZEND_COLD void ZEND_FASTCALL zend_illegal_array_offset_isset(const zval *offset)
{
	zend_illegal_container_offset(ZSTR_KNOWN(ZEND_STR_ARRAY), offset, BP_VAR_IS);
}

static zend_never_inline ZEND_COLD void ZEND_FASTCALL zend_illegal_array_offset_unset(const zval *offset)
{
	zend_illegal_container_offset(ZSTR_KNOWN(ZEND_STR_ARRAY), offset, BP_VAR_UNSET);
}

static zend_never_inline ZEND_COLD void ZEND_FASTCALL zend_illegal_string_offset(const zval *offset, int type)
{
	zend_illegal_container_offset(ZSTR_KNOWN(ZEND_STR_STRING), offset, type);
}

static zend_never_inline void zend_assign_to_object_dim(zend_object *obj, zval *dim, zval *value OPLINE_DC EXECUTE_DATA_DC)
{
	obj->handlers->write_dimension(obj, dim, value);

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

static zend_never_inline void zend_binary_assign_op_obj_dim(zend_object *obj, zval *property OPLINE_DC EXECUTE_DATA_DC)
{
	zval *value;
	zval *z;
	zval rv, res;

	GC_ADDREF(obj);
	if (property && UNEXPECTED(Z_ISUNDEF_P(property))) {
		property = ZVAL_UNDEFINED_OP2();
	}
	value = get_op_data_zval_ptr_r((opline+1)->op1_type, (opline+1)->op1);
	if ((z = obj->handlers->read_dimension(obj, property, BP_VAR_R, &rv)) != NULL) {

		if (zend_binary_op(&res, z, value OPLINE_CC) == SUCCESS) {
			obj->handlers->write_dimension(obj, property, &res);
		}
		if (z == &rv) {
			zval_ptr_dtor(&rv);
		}
		if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
			ZVAL_COPY(EX_VAR(opline->result.var), &res);
		}
		zval_ptr_dtor(&res);
	} else {
		zend_use_object_as_array(obj);
		if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
			ZVAL_NULL(EX_VAR(opline->result.var));
		}
	}
	FREE_OP((opline+1)->op1_type, (opline+1)->op1.var);
	if (UNEXPECTED(GC_DELREF(obj) == 0)) {
		zend_objects_store_del(obj);
	}
}

static zend_never_inline void zend_binary_assign_op_typed_ref(zend_reference *ref, zval *value OPLINE_DC EXECUTE_DATA_DC)
{
	zval z_copy;

	/* Make sure that in-place concatenation is used if the LHS is a string. */
	if (opline->extended_value == ZEND_CONCAT && Z_TYPE(ref->val) == IS_STRING) {
		concat_function(&ref->val, &ref->val, value);
		ZEND_ASSERT(Z_TYPE(ref->val) == IS_STRING && "Concat should return string");
		return;
	}

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

	/* Make sure that in-place concatenation is used if the LHS is a string. */
	if (opline->extended_value == ZEND_CONCAT && Z_TYPE_P(zptr) == IS_STRING) {
		concat_function(zptr, zptr, value);
		ZEND_ASSERT(Z_TYPE_P(zptr) == IS_STRING && "Concat should return string");
		return;
	}

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
	switch(Z_TYPE_P(dim)) {
		case IS_LONG:
			return Z_LVAL_P(dim);
		case IS_STRING:
		{
			bool trailing_data = false;
			/* For BC reasons we allow errors so that we can warn on leading numeric string */
			if (IS_LONG == is_numeric_string_ex(Z_STRVAL_P(dim), Z_STRLEN_P(dim), &offset, NULL,
					/* allow errors */ true, NULL, &trailing_data)) {
				if (UNEXPECTED(trailing_data) && type != BP_VAR_UNSET) {
					zend_error(E_WARNING, "Illegal string offset \"%s\"", Z_STRVAL_P(dim));
				}
				return offset;
			}
			zend_illegal_string_offset(dim, type);
			return 0;
		}
		case IS_UNDEF:
			ZVAL_UNDEFINED_OP2();
			ZEND_FALLTHROUGH;
		case IS_DOUBLE:
		case IS_NULL:
		case IS_FALSE:
		case IS_TRUE:
			zend_error(E_WARNING, "String offset cast occurred");
			break;
		case IS_REFERENCE:
			dim = Z_REFVAL_P(dim);
			goto try_again;
		default:
			zend_illegal_string_offset(dim, type);
			return 0;
	}

	return zval_get_long_func(dim, /* is_strict */ false);
}

ZEND_API ZEND_COLD void zend_wrong_string_offset_error(void)
{
	const char *msg = NULL;
	const zend_execute_data *execute_data = EG(current_execute_data);
	const zend_op *opline = execute_data->opline;

	if (UNEXPECTED(EG(exception) != NULL)) {
		return;
	}

	switch (opline->opcode) {
		case ZEND_ASSIGN_DIM_OP:
			msg = "Cannot use assign-op operators with string offsets";
			break;
		case ZEND_FETCH_LIST_W:
			msg = "Cannot create references to/from string offsets";
			break;
		case ZEND_FETCH_DIM_W:
		case ZEND_FETCH_DIM_RW:
		case ZEND_FETCH_DIM_FUNC_ARG:
		case ZEND_FETCH_DIM_UNSET:
			switch (opline->extended_value) {
				case ZEND_FETCH_DIM_REF:
					msg = "Cannot create references to/from string offsets";
					break;
				case ZEND_FETCH_DIM_DIM:
					msg = "Cannot use string offset as an array";
					break;
				case ZEND_FETCH_DIM_OBJ:
					msg = "Cannot use string offset as an object";
					break;
				case ZEND_FETCH_DIM_INCDEC:
					msg = "Cannot increment/decrement string offsets";
					break;
				EMPTY_SWITCH_DEFAULT_CASE();
			}
			break;
		EMPTY_SWITCH_DEFAULT_CASE();
	}
	ZEND_ASSERT(msg != NULL);
	zend_throw_error(NULL, "%s", msg);
}

ZEND_API ZEND_COLD void ZEND_FASTCALL zend_deprecated_function(const zend_function *fbc)
{
	if (fbc->common.scope) {
		zend_error(E_DEPRECATED, "Method %s::%s() is deprecated",
			ZSTR_VAL(fbc->common.scope->name),
			ZSTR_VAL(fbc->common.function_name)
		);
	} else {
		zend_error(E_DEPRECATED, "Function %s() is deprecated", ZSTR_VAL(fbc->common.function_name));
	}
}

ZEND_API ZEND_COLD void ZEND_FASTCALL zend_false_to_array_deprecated(void)
{
	zend_error(E_DEPRECATED, "Automatic conversion of false to array is deprecated");
}

static zend_never_inline void zend_assign_to_string_offset(zval *str, zval *dim, zval *value OPLINE_DC EXECUTE_DATA_DC)
{
	zend_uchar c;
	size_t string_len;
	zend_long offset;
	zend_string *s;

	/* separate string */
	if (Z_REFCOUNTED_P(str) && Z_REFCOUNT_P(str) == 1) {
		s = Z_STR_P(str);
	} else {
		s = zend_string_init(Z_STRVAL_P(str), Z_STRLEN_P(str), 0);
		ZSTR_H(s) = ZSTR_H(Z_STR_P(str));
		if (Z_REFCOUNTED_P(str)) {
			GC_DELREF(Z_STR_P(str));
		}
		ZVAL_NEW_STR(str, s);
	}

	if (EXPECTED(Z_TYPE_P(dim) == IS_LONG)) {
		offset = Z_LVAL_P(dim);
	} else {
		/* The string may be destroyed while throwing the notice.
		 * Temporarily increase the refcount to detect this situation. */
		GC_ADDREF(s);
		offset = zend_check_string_offset(dim, BP_VAR_W EXECUTE_DATA_CC);
		if (UNEXPECTED(GC_DELREF(s) == 0)) {
			zend_string_efree(s);
			if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
				ZVAL_NULL(EX_VAR(opline->result.var));
			}
			return;
		}
		/* Illegal offset assignment */
		if (UNEXPECTED(EG(exception) != NULL)) {
			if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
				ZVAL_UNDEF(EX_VAR(opline->result.var));
			}
			return;
		}
	}

	if (UNEXPECTED(offset < -(zend_long)ZSTR_LEN(s))) {
		/* Error on negative offset */
		zend_error(E_WARNING, "Illegal string offset " ZEND_LONG_FMT, offset);
		if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
			ZVAL_NULL(EX_VAR(opline->result.var));
		}
		return;
	}

	if (offset < 0) { /* Handle negative offset */
		offset += (zend_long)ZSTR_LEN(s);
	}

	if (UNEXPECTED(Z_TYPE_P(value) != IS_STRING)) {
		zend_string *tmp;

		/* The string may be destroyed while throwing the notice.
		 * Temporarily increase the refcount to detect this situation. */
		GC_ADDREF(s);
		if (UNEXPECTED(Z_TYPE_P(value) == IS_UNDEF)) {
			zval_undefined_cv((opline+1)->op1.var EXECUTE_DATA_CC);
		}
		/* Convert to string, just the time to pick the 1st byte */
		tmp = zval_try_get_string_func(value);
		if (UNEXPECTED(GC_DELREF(s) == 0)) {
			zend_string_efree(s);
			if (tmp) {
				zend_string_release_ex(tmp, 0);
			}
			if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
				ZVAL_NULL(EX_VAR(opline->result.var));
			}
			return;
		}
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

	if (UNEXPECTED(string_len != 1)) {
		if (string_len == 0) {
			/* Error on empty input string */
			zend_throw_error(NULL, "Cannot assign an empty string to a string offset");
			if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
				ZVAL_NULL(EX_VAR(opline->result.var));
			}
			return;
		}

		/* The string may be destroyed while throwing the notice.
		 * Temporarily increase the refcount to detect this situation. */
		GC_ADDREF(s);
		zend_error(E_WARNING, "Only the first byte will be assigned to the string offset");
		if (UNEXPECTED(GC_DELREF(s) == 0)) {
			zend_string_efree(s);
			if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
				ZVAL_NULL(EX_VAR(opline->result.var));
			}
			return;
		}
		/* Illegal offset assignment */
		if (UNEXPECTED(EG(exception) != NULL)) {
			if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
				ZVAL_UNDEF(EX_VAR(opline->result.var));
			}
			return;
		}
	}

	if ((size_t)offset >= ZSTR_LEN(s)) {
		/* Extend string if needed */
		zend_long old_len = ZSTR_LEN(s);
		ZVAL_NEW_STR(str, zend_string_extend(s, (size_t)offset + 1, 0));
		memset(Z_STRVAL_P(str) + old_len, ' ', offset - old_len);
		Z_STRVAL_P(str)[offset+1] = 0;
	} else {
		zend_string_forget_hash_val(Z_STR_P(str));
	}

	Z_STRVAL_P(str)[offset] = c;

	if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
		/* Return the new character */
		ZVAL_CHAR(EX_VAR(opline->result.var), c);
	}
}

static zend_property_info *zend_get_prop_not_accepting_double(zend_reference *ref)
{
	zend_property_info *prop;
	ZEND_REF_FOREACH_TYPE_SOURCES(ref, prop) {
		if (!(ZEND_TYPE_FULL_MASK(prop->type) & MAY_BE_DOUBLE)) {
			return prop;
		}
	} ZEND_REF_FOREACH_TYPE_SOURCES_END();
	return NULL;
}

static ZEND_COLD zend_long zend_throw_incdec_ref_error(
		zend_reference *ref, zend_property_info *error_prop OPLINE_DC)
{
	zend_string *type_str = zend_type_to_string(error_prop->type);
	if (ZEND_IS_INCREMENT(opline->opcode)) {
		zend_type_error(
			"Cannot increment a reference held by property %s::$%s of type %s past its maximal value",
			ZSTR_VAL(error_prop->ce->name),
			zend_get_unmangled_property_name(error_prop->name),
			ZSTR_VAL(type_str));
		zend_string_release(type_str);
		return ZEND_LONG_MAX;
	} else {
		zend_type_error(
			"Cannot decrement a reference held by property %s::$%s of type %s past its minimal value",
			ZSTR_VAL(error_prop->ce->name),
			zend_get_unmangled_property_name(error_prop->name),
			ZSTR_VAL(type_str));
		zend_string_release(type_str);
		return ZEND_LONG_MIN;
	}
}

static ZEND_COLD zend_long zend_throw_incdec_prop_error(zend_property_info *prop OPLINE_DC) {
	zend_string *type_str = zend_type_to_string(prop->type);
	if (ZEND_IS_INCREMENT(opline->opcode)) {
		zend_type_error("Cannot increment property %s::$%s of type %s past its maximal value",
			ZSTR_VAL(prop->ce->name),
			zend_get_unmangled_property_name(prop->name),
			ZSTR_VAL(type_str));
		zend_string_release(type_str);
		return ZEND_LONG_MAX;
	} else {
		zend_type_error("Cannot decrement property %s::$%s of type %s past its minimal value",
			ZSTR_VAL(prop->ce->name),
			zend_get_unmangled_property_name(prop->name),
			ZSTR_VAL(type_str));
		zend_string_release(type_str);
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
		zend_property_info *error_prop = zend_get_prop_not_accepting_double(ref);
		if (UNEXPECTED(error_prop)) {
			zend_long val = zend_throw_incdec_ref_error(ref, error_prop OPLINE_CC);
			ZVAL_LONG(var_ptr, val);
		}
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
		if (!(ZEND_TYPE_FULL_MASK(prop_info->type) & MAY_BE_DOUBLE)) {
			zend_long val = zend_throw_incdec_prop_error(prop_info OPLINE_CC);
			ZVAL_LONG(var_ptr, val);
		}
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
		if (UNEXPECTED(Z_TYPE_P(prop) != IS_LONG) && UNEXPECTED(prop_info)
				&& !(ZEND_TYPE_FULL_MASK(prop_info->type) & MAY_BE_DOUBLE)) {
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
		if (UNEXPECTED(Z_TYPE_P(prop) != IS_LONG) && UNEXPECTED(prop_info)
				&& !(ZEND_TYPE_FULL_MASK(prop_info->type) & MAY_BE_DOUBLE)) {
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

static zend_never_inline void zend_post_incdec_overloaded_property(zend_object *object, zend_string *name, void **cache_slot OPLINE_DC EXECUTE_DATA_DC)
{
	zval rv;
	zval *z;
	zval z_copy;

	GC_ADDREF(object);
	z =object->handlers->read_property(object, name, BP_VAR_R, cache_slot, &rv);
	if (UNEXPECTED(EG(exception))) {
		OBJ_RELEASE(object);
		ZVAL_UNDEF(EX_VAR(opline->result.var));
		return;
	}

	ZVAL_COPY_DEREF(&z_copy, z);
	ZVAL_COPY(EX_VAR(opline->result.var), &z_copy);
	if (ZEND_IS_INCREMENT(opline->opcode)) {
		increment_function(&z_copy);
	} else {
		decrement_function(&z_copy);
	}
	object->handlers->write_property(object, name, &z_copy, cache_slot);
	OBJ_RELEASE(object);
	zval_ptr_dtor(&z_copy);
	if (z == &rv) {
		zval_ptr_dtor(z);
	}
}

static zend_never_inline void zend_pre_incdec_overloaded_property(zend_object *object, zend_string *name, void **cache_slot OPLINE_DC EXECUTE_DATA_DC)
{
	zval rv;
	zval *z;
	zval z_copy;

	GC_ADDREF(object);
	z = object->handlers->read_property(object, name, BP_VAR_R, cache_slot, &rv);
	if (UNEXPECTED(EG(exception))) {
		OBJ_RELEASE(object);
		if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
			ZVAL_NULL(EX_VAR(opline->result.var));
		}
		return;
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
	object->handlers->write_property(object, name, &z_copy, cache_slot);
	OBJ_RELEASE(object);
	zval_ptr_dtor(&z_copy);
	if (z == &rv) {
		zval_ptr_dtor(z);
	}
}

static zend_never_inline void zend_assign_op_overloaded_property(zend_object *object, zend_string *name, void **cache_slot, zval *value OPLINE_DC EXECUTE_DATA_DC)
{
	zval *z;
	zval rv, res;

	GC_ADDREF(object);
	z = object->handlers->read_property(object, name, BP_VAR_R, cache_slot, &rv);
	if (UNEXPECTED(EG(exception))) {
		OBJ_RELEASE(object);
		if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
			ZVAL_UNDEF(EX_VAR(opline->result.var));
		}
		return;
	}
	if (zend_binary_op(&res, z, value OPLINE_CC) == SUCCESS) {
		object->handlers->write_property(object, name, &res, cache_slot);
	}
	if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
		ZVAL_COPY(EX_VAR(opline->result.var), &res);
	}
	if (z == &rv) {
		zval_ptr_dtor(z);
	}
	zval_ptr_dtor(&res);
	OBJ_RELEASE(object);
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
	zend_error(E_WARNING, "Undefined array key " ZEND_LONG_FMT, lval);
}

static zend_never_inline ZEND_COLD void ZEND_FASTCALL zend_undefined_index(const zend_string *offset)
{
	zend_error(E_WARNING, "Undefined array key \"%s\"", ZSTR_VAL(offset));
}

ZEND_API ZEND_COLD zval* ZEND_FASTCALL zend_undefined_offset_write(HashTable *ht, zend_long lval)
{
	/* The array may be destroyed while throwing the notice.
	 * Temporarily increase the refcount to detect this situation. */
	if (!(GC_FLAGS(ht) & IS_ARRAY_IMMUTABLE)) {
		GC_ADDREF(ht);
	}
	zend_undefined_offset(lval);
	if (!(GC_FLAGS(ht) & IS_ARRAY_IMMUTABLE) && GC_DELREF(ht) != 1) {
		if (!GC_REFCOUNT(ht)) {
			zend_array_destroy(ht);
		}
		return NULL;
	}
	if (EG(exception)) {
		return NULL;
	}
	return zend_hash_index_add_new(ht, lval, &EG(uninitialized_zval));
}

ZEND_API ZEND_COLD zval* ZEND_FASTCALL zend_undefined_index_write(HashTable *ht, zend_string *offset)
{
	zval *retval;

	/* The array may be destroyed while throwing the notice.
	 * Temporarily increase the refcount to detect this situation. */
	if (!(GC_FLAGS(ht) & IS_ARRAY_IMMUTABLE)) {
		GC_ADDREF(ht);
	}
	/* Key may be released while throwing the undefined index warning. */
	zend_string_addref(offset);
	zend_undefined_index(offset);
	if (!(GC_FLAGS(ht) & IS_ARRAY_IMMUTABLE) && GC_DELREF(ht) != 1) {
		if (!GC_REFCOUNT(ht)) {
			zend_array_destroy(ht);
		}
		retval = NULL;
	} else if (EG(exception)) {
		retval = NULL;
	} else {
		retval = zend_hash_add_new(ht, offset, &EG(uninitialized_zval));
	}
	zend_string_release(offset);
	return retval;
}

static zend_never_inline ZEND_COLD void ZEND_FASTCALL zend_undefined_method(const zend_class_entry *ce, const zend_string *method)
{
	zend_throw_error(NULL, "Call to undefined method %s::%s()", ZSTR_VAL(ce->name), ZSTR_VAL(method));
}

static zend_never_inline ZEND_COLD void ZEND_FASTCALL zend_invalid_method_call(zval *object, zval *function_name)
{
	zend_throw_error(NULL, "Call to a member function %s() on %s",
		Z_STRVAL_P(function_name), zend_zval_value_name(object));
}

static zend_never_inline ZEND_COLD void ZEND_FASTCALL zend_non_static_method_call(const zend_function *fbc)
{
	zend_throw_error(
		zend_ce_error,
		"Non-static method %s::%s() cannot be called statically",
		ZSTR_VAL(fbc->common.scope->name), ZSTR_VAL(fbc->common.function_name));
}

ZEND_COLD void ZEND_FASTCALL zend_param_must_be_ref(const zend_function *func, uint32_t arg_num)
{
	const char *arg_name = get_function_arg_name(func, arg_num);

	zend_error(E_WARNING, "%s%s%s(): Argument #%d%s%s%s must be passed by reference, value given",
		func->common.scope ? ZSTR_VAL(func->common.scope->name) : "",
		func->common.scope ? "::" : "",
		ZSTR_VAL(func->common.function_name),
		arg_num,
		arg_name ? " ($" : "",
		arg_name ? arg_name : "",
		arg_name ? ")" : ""
	);
}

static zend_never_inline ZEND_COLD void ZEND_FASTCALL zend_use_scalar_as_array(void)
{
	zend_throw_error(NULL, "Cannot use a scalar value as an array");
}

ZEND_API zend_never_inline ZEND_COLD void ZEND_FASTCALL zend_cannot_add_element(void)
{
	zend_throw_error(NULL, "Cannot add element to the array as the next element is already occupied");
}

ZEND_API ZEND_COLD void ZEND_FASTCALL zend_use_resource_as_offset(const zval *dim)
{
	zend_error(E_WARNING,
		"Resource ID#" ZEND_LONG_FMT " used as offset, casting to integer (" ZEND_LONG_FMT ")",
		Z_RES_HANDLE_P(dim), Z_RES_HANDLE_P(dim));
}

static zend_never_inline ZEND_COLD void ZEND_FASTCALL zend_use_new_element_for_string(void)
{
	zend_throw_error(NULL, "[] operator not supported for strings");
}

#ifdef ZEND_CHECK_STACK_LIMIT
static zend_never_inline ZEND_COLD void ZEND_FASTCALL zend_call_stack_size_error(void)
{
	zend_throw_error(NULL, "Maximum call stack size of %zu bytes (zend.max_allowed_stack_size - zend.reserved_stack_size) reached. Infinite recursion?",
		(size_t) ((uintptr_t) EG(stack_base) - (uintptr_t) EG(stack_limit)));
}
#endif /* ZEND_CHECK_STACK_LIMIT */

static ZEND_COLD void zend_binary_assign_op_dim_slow(zval *container, zval *dim OPLINE_DC EXECUTE_DATA_DC)
{
	if (UNEXPECTED(Z_TYPE_P(container) == IS_STRING)) {
		if (opline->op2_type == IS_UNUSED) {
			zend_use_new_element_for_string();
		} else {
			zend_check_string_offset(dim, BP_VAR_RW EXECUTE_DATA_CC);
			zend_wrong_string_offset_error();
		}
	} else {
		zend_use_scalar_as_array();
	}
}

static zend_never_inline uint8_t slow_index_convert(HashTable *ht, const zval *dim, zend_value *value EXECUTE_DATA_DC)
{
	switch (Z_TYPE_P(dim)) {
		case IS_UNDEF: {
			/* The array may be destroyed while throwing the notice.
			 * Temporarily increase the refcount to detect this situation. */
			if (!(GC_FLAGS(ht) & IS_ARRAY_IMMUTABLE)) {
				GC_ADDREF(ht);
			}
			ZVAL_UNDEFINED_OP2();
			if (!(GC_FLAGS(ht) & IS_ARRAY_IMMUTABLE) && !GC_DELREF(ht)) {
				zend_array_destroy(ht);
				return IS_NULL;
			}
			if (EG(exception)) {
				return IS_NULL;
			}
			ZEND_FALLTHROUGH;
		}
		case IS_NULL:
			value->str = ZSTR_EMPTY_ALLOC();
			return IS_STRING;
		case IS_DOUBLE:
			value->lval = zend_dval_to_lval(Z_DVAL_P(dim));
			if (!zend_is_long_compatible(Z_DVAL_P(dim), value->lval)) {
				/* The array may be destroyed while throwing the notice.
				 * Temporarily increase the refcount to detect this situation. */
				if (!(GC_FLAGS(ht) & IS_ARRAY_IMMUTABLE)) {
					GC_ADDREF(ht);
				}
				zend_incompatible_double_to_long_error(Z_DVAL_P(dim));
				if (!(GC_FLAGS(ht) & IS_ARRAY_IMMUTABLE) && !GC_DELREF(ht)) {
					zend_array_destroy(ht);
					return IS_NULL;
				}
				if (EG(exception)) {
					return IS_NULL;
				}
			}
			return IS_LONG;
		case IS_RESOURCE:
			/* The array may be destroyed while throwing the notice.
			 * Temporarily increase the refcount to detect this situation. */
			if (!(GC_FLAGS(ht) & IS_ARRAY_IMMUTABLE)) {
				GC_ADDREF(ht);
			}
			zend_use_resource_as_offset(dim);
			if (!(GC_FLAGS(ht) & IS_ARRAY_IMMUTABLE) && !GC_DELREF(ht)) {
				zend_array_destroy(ht);
				return IS_NULL;
			}
			if (EG(exception)) {
				return IS_NULL;
			}
			value->lval = Z_RES_HANDLE_P(dim);
			return IS_LONG;
		case IS_FALSE:
			value->lval = 0;
			return IS_LONG;
		case IS_TRUE:
			value->lval = 1;
			return IS_LONG;
		default:
			zend_illegal_array_offset_access(dim);
			return IS_NULL;
	}
}

static zend_never_inline uint8_t slow_index_convert_w(HashTable *ht, const zval *dim, zend_value *value EXECUTE_DATA_DC)
{
	switch (Z_TYPE_P(dim)) {
		case IS_UNDEF: {
			/* The array may be destroyed while throwing the notice.
			 * Temporarily increase the refcount to detect this situation. */
			if (!(GC_FLAGS(ht) & IS_ARRAY_IMMUTABLE)) {
				GC_ADDREF(ht);
			}
			ZVAL_UNDEFINED_OP2();
			if (!(GC_FLAGS(ht) & IS_ARRAY_IMMUTABLE) && GC_DELREF(ht) != 1) {
				if (!GC_REFCOUNT(ht)) {
					zend_array_destroy(ht);
				}
				return IS_NULL;
			}
			if (EG(exception)) {
				return IS_NULL;
			}
			ZEND_FALLTHROUGH;
		}
		case IS_NULL:
			value->str = ZSTR_EMPTY_ALLOC();
			return IS_STRING;
		case IS_DOUBLE:
			value->lval = zend_dval_to_lval(Z_DVAL_P(dim));
			if (!zend_is_long_compatible(Z_DVAL_P(dim), value->lval)) {
				/* The array may be destroyed while throwing the notice.
				 * Temporarily increase the refcount to detect this situation. */
				if (!(GC_FLAGS(ht) & IS_ARRAY_IMMUTABLE)) {
					GC_ADDREF(ht);
				}
				zend_incompatible_double_to_long_error(Z_DVAL_P(dim));
				if (!(GC_FLAGS(ht) & IS_ARRAY_IMMUTABLE) && GC_DELREF(ht) != 1) {
					if (!GC_REFCOUNT(ht)) {
						zend_array_destroy(ht);
					}
					return IS_NULL;
				}
				if (EG(exception)) {
					return IS_NULL;
				}
			}
			return IS_LONG;
		case IS_RESOURCE:
			/* The array may be destroyed while throwing the notice.
			 * Temporarily increase the refcount to detect this situation. */
			if (!(GC_FLAGS(ht) & IS_ARRAY_IMMUTABLE)) {
				GC_ADDREF(ht);
			}
			zend_use_resource_as_offset(dim);
			if (!(GC_FLAGS(ht) & IS_ARRAY_IMMUTABLE) && GC_DELREF(ht) != 1) {
				if (!GC_REFCOUNT(ht)) {
					zend_array_destroy(ht);
				}
				return IS_NULL;
			}
			if (EG(exception)) {
				return IS_NULL;
			}
			value->lval = Z_RES_HANDLE_P(dim);
			return IS_LONG;
		case IS_FALSE:
			value->lval = 0;
			return IS_LONG;
		case IS_TRUE:
			value->lval = 1;
			return IS_LONG;
		default:
			zend_illegal_array_offset_access(dim);
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
		if (type != BP_VAR_W) {
			ZEND_HASH_INDEX_FIND(ht, hval, retval, num_undef);
			return retval;
num_undef:
			switch (type) {
				case BP_VAR_R:
					zend_undefined_offset(hval);
					ZEND_FALLTHROUGH;
				case BP_VAR_UNSET:
				case BP_VAR_IS:
					retval = &EG(uninitialized_zval);
					break;
				case BP_VAR_RW:
					retval = zend_undefined_offset_write(ht, hval);
					break;
				}
		} else {
			ZEND_HASH_INDEX_LOOKUP(ht, hval, retval);
		}
	} else if (EXPECTED(Z_TYPE_P(dim) == IS_STRING)) {
		offset_key = Z_STR_P(dim);
		if (ZEND_CONST_COND(dim_type != IS_CONST, 1)) {
			if (ZEND_HANDLE_NUMERIC(offset_key, hval)) {
				goto num_index;
			}
		}
str_index:
		if (type != BP_VAR_W) {
			retval = zend_hash_find_ex(ht, offset_key, ZEND_CONST_COND(dim_type == IS_CONST, 0));
			if (!retval) {
				switch (type) {
					case BP_VAR_R:
						zend_undefined_index(offset_key);
						ZEND_FALLTHROUGH;
					case BP_VAR_UNSET:
					case BP_VAR_IS:
						retval = &EG(uninitialized_zval);
						break;
					case BP_VAR_RW:
						retval = zend_undefined_index_write(ht, offset_key);
						break;
				}
			}
		} else {
			retval = zend_hash_lookup(ht, offset_key);
		}
	} else if (EXPECTED(Z_TYPE_P(dim) == IS_REFERENCE)) {
		dim = Z_REFVAL_P(dim);
		goto try_again;
	} else {
		zend_value val;
		uint8_t t;

		if (type != BP_VAR_W && type != BP_VAR_RW) {
			t = slow_index_convert(ht, dim, &val EXECUTE_DATA_CC);
		} else {
			t = slow_index_convert_w(ht, dim, &val EXECUTE_DATA_CC);
		}
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
				ZVAL_UNDEF(result);
				return;
			}
		} else {
			retval = zend_fetch_dimension_address_inner(Z_ARRVAL_P(container), dim, dim_type, type EXECUTE_DATA_CC);
			if (UNEXPECTED(!retval)) {
				/* This may fail without throwing if the array was modified while throwing an
				 * undefined index error. */
				ZVAL_NULL(result);
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
						ZVAL_UNDEF(result);
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
			zend_wrong_string_offset_error();
		}
		ZVAL_UNDEF(result);
	} else if (EXPECTED(Z_TYPE_P(container) == IS_OBJECT)) {
		zend_object *obj = Z_OBJ_P(container);
		GC_ADDREF(obj);
		if (ZEND_CONST_COND(dim_type == IS_CV, dim != NULL) && UNEXPECTED(Z_TYPE_P(dim) == IS_UNDEF)) {
			dim = ZVAL_UNDEFINED_OP2();
		} else if (dim_type == IS_CONST && Z_EXTRA_P(dim) == ZEND_EXTRA_VALUE) {
			dim++;
		}
		retval = obj->handlers->read_dimension(obj, dim, type, result);

		if (UNEXPECTED(retval == &EG(uninitialized_zval))) {
			zend_class_entry *ce = obj->ce;

			ZVAL_NULL(result);
			zend_error(E_NOTICE, "Indirect modification of overloaded element of %s has no effect", ZSTR_VAL(ce->name));
		} else if (EXPECTED(retval && Z_TYPE_P(retval) != IS_UNDEF)) {
			if (!Z_ISREF_P(retval)) {
				if (result != retval) {
					ZVAL_COPY(result, retval);
					retval = result;
				}
				if (Z_TYPE_P(retval) != IS_OBJECT) {
					zend_class_entry *ce = obj->ce;
					zend_error(E_NOTICE, "Indirect modification of overloaded element of %s has no effect", ZSTR_VAL(ce->name));
				}
			} else if (UNEXPECTED(Z_REFCOUNT_P(retval) == 1)) {
				ZVAL_UNREF(retval);
			}
			if (result != retval) {
				ZVAL_INDIRECT(result, retval);
			}
		} else {
			ZEND_ASSERT(EG(exception) && "read_dimension() returned NULL without exception");
			ZVAL_UNDEF(result);
		}
		if (UNEXPECTED(GC_DELREF(obj) == 0)) {
			zend_objects_store_del(obj);
		}
	} else {
		if (EXPECTED(Z_TYPE_P(container) <= IS_FALSE)) {
			if (type != BP_VAR_W && UNEXPECTED(Z_TYPE_P(container) == IS_UNDEF)) {
				ZVAL_UNDEFINED_OP1();
			}
			if (type != BP_VAR_UNSET) {
				HashTable *ht = zend_new_array(0);
				uint8_t old_type = Z_TYPE_P(container);

				ZVAL_ARR(container, ht);
				if (UNEXPECTED(old_type == IS_FALSE)) {
					GC_ADDREF(ht);
					zend_false_to_array_deprecated();
					if (UNEXPECTED(GC_DELREF(ht) == 0)) {
						zend_array_destroy(ht);
						goto return_null;
					}
				}
				goto fetch_from_array;
			} else {
				if (UNEXPECTED(Z_TYPE_P(container) == IS_FALSE)) {
					zend_false_to_array_deprecated();
				}
return_null:
				/* for read-mode only */
				if (ZEND_CONST_COND(dim_type == IS_CV, dim != NULL) && UNEXPECTED(Z_TYPE_P(dim) == IS_UNDEF)) {
					ZVAL_UNDEFINED_OP2();
				}
				ZVAL_NULL(result);
			}
		} else {
			if (type == BP_VAR_UNSET) {
				zend_throw_error(NULL, "Cannot unset offset in a non-array variable");
				ZVAL_UNDEF(result);
			} else {
				zend_use_scalar_as_array();
				ZVAL_UNDEF(result);
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

static zend_always_inline void zend_fetch_dimension_address_read(zval *result, zval *container, zval *dim, int dim_type, int type, bool is_list, int slow EXECUTE_DATA_DC)
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
		zend_string *str = Z_STR_P(container);
		zend_long offset;

try_string_offset:
		if (UNEXPECTED(Z_TYPE_P(dim) != IS_LONG)) {
			switch (Z_TYPE_P(dim)) {
				case IS_STRING:
				{
					bool trailing_data = false;
					/* For BC reasons we allow errors so that we can warn on leading numeric string */
					if (IS_LONG == is_numeric_string_ex(Z_STRVAL_P(dim), Z_STRLEN_P(dim), &offset,
							NULL, /* allow errors */ true, NULL, &trailing_data)) {
						if (UNEXPECTED(trailing_data)) {
							zend_error(E_WARNING, "Illegal string offset \"%s\"", Z_STRVAL_P(dim));
						}
						goto out;
					}
					if (type == BP_VAR_IS) {
						ZVAL_NULL(result);
						return;
					}
					zend_illegal_string_offset(dim, BP_VAR_R);
					ZVAL_NULL(result);
					return;
				}
				case IS_UNDEF:
					/* The string may be destroyed while throwing the notice.
					 * Temporarily increase the refcount to detect this situation. */
					if (!(GC_FLAGS(str) & IS_STR_INTERNED)) {
						GC_ADDREF(str);
					}
					ZVAL_UNDEFINED_OP2();
					if (!(GC_FLAGS(str) & IS_STR_INTERNED) && UNEXPECTED(GC_DELREF(str) == 0)) {
						zend_string_efree(str);
						ZVAL_NULL(result);
						return;
					}
					ZEND_FALLTHROUGH;
				case IS_DOUBLE:
				case IS_NULL:
				case IS_FALSE:
				case IS_TRUE:
					if (type != BP_VAR_IS) {
						/* The string may be destroyed while throwing the notice.
						 * Temporarily increase the refcount to detect this situation. */
						if (!(GC_FLAGS(str) & IS_STR_INTERNED)) {
							GC_ADDREF(str);
						}
						zend_error(E_WARNING, "String offset cast occurred");
						if (!(GC_FLAGS(str) & IS_STR_INTERNED) && UNEXPECTED(GC_DELREF(str) == 0)) {
							zend_string_efree(str);
							ZVAL_NULL(result);
							return;
						}
					}
					break;
				case IS_REFERENCE:
					dim = Z_REFVAL_P(dim);
					goto try_string_offset;
				default:
					zend_illegal_string_offset(dim, BP_VAR_R);
					ZVAL_NULL(result);
					return;
			}

			offset = zval_get_long_func(dim, /* is_strict */ false);
		} else {
			offset = Z_LVAL_P(dim);
		}
		out:

		if (UNEXPECTED(ZSTR_LEN(str) < ((offset < 0) ? -(size_t)offset : ((size_t)offset + 1)))) {
			if (type != BP_VAR_IS) {
				zend_error(E_WARNING, "Uninitialized string offset " ZEND_LONG_FMT, offset);
				ZVAL_EMPTY_STRING(result);
			} else {
				ZVAL_NULL(result);
			}
		} else {
			zend_uchar c;
			zend_long real_offset;

			real_offset = (UNEXPECTED(offset < 0)) /* Handle negative offset */
				? (zend_long)ZSTR_LEN(str) + offset : offset;
			c = (zend_uchar)ZSTR_VAL(str)[real_offset];

			ZVAL_CHAR(result, c);
		}
	} else if (EXPECTED(Z_TYPE_P(container) == IS_OBJECT)) {
		zend_object *obj = Z_OBJ_P(container);

		GC_ADDREF(obj);
		if (ZEND_CONST_COND(dim_type == IS_CV, 1) && UNEXPECTED(Z_TYPE_P(dim) == IS_UNDEF)) {
			dim = ZVAL_UNDEFINED_OP2();
		}
		if (dim_type == IS_CONST && Z_EXTRA_P(dim) == ZEND_EXTRA_VALUE) {
			dim++;
		}
		retval = obj->handlers->read_dimension(obj, dim, type, result);

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
		if (UNEXPECTED(GC_DELREF(obj) == 0)) {
			zend_objects_store_del(obj);
		}
	} else {
		if (type != BP_VAR_IS && UNEXPECTED(Z_TYPE_P(container) == IS_UNDEF)) {
			container = ZVAL_UNDEFINED_OP1();
		}
		if (ZEND_CONST_COND(dim_type == IS_CV, 1) && UNEXPECTED(Z_TYPE_P(dim) == IS_UNDEF)) {
			ZVAL_UNDEFINED_OP2();
		}
		if (!is_list && type != BP_VAR_IS) {
			zend_error(E_WARNING, "Trying to access array offset on %s",
				zend_zval_value_name(container));
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
		hval = zend_dval_to_lval_safe(Z_DVAL_P(offset));
num_idx:
		return zend_hash_index_find(ht, hval);
	} else if (Z_TYPE_P(offset) == IS_NULL) {
str_idx:
		return zend_hash_find_known_hash(ht, ZSTR_EMPTY_ALLOC());
	} else if (Z_TYPE_P(offset) == IS_FALSE) {
		hval = 0;
		goto num_idx;
	} else if (Z_TYPE_P(offset) == IS_TRUE) {
		hval = 1;
		goto num_idx;
	} else if (Z_TYPE_P(offset) == IS_RESOURCE) {
		zend_use_resource_as_offset(offset);
		hval = Z_RES_HANDLE_P(offset);
		goto num_idx;
	} else if (/*OP2_TYPE == IS_CV &&*/ Z_TYPE_P(offset) == IS_UNDEF) {
		ZVAL_UNDEFINED_OP2();
		goto str_idx;
	} else {
		zend_illegal_array_offset_isset(offset);
		return NULL;
	}
}

static zend_never_inline bool ZEND_FASTCALL zend_isset_dim_slow(zval *container, zval *offset EXECUTE_DATA_DC)
{
	if (/*OP2_TYPE == IS_CV &&*/ UNEXPECTED(Z_TYPE_P(offset) == IS_UNDEF)) {
		offset = ZVAL_UNDEFINED_OP2();
	}

	if (/*OP1_TYPE != IS_CONST &&*/ EXPECTED(Z_TYPE_P(container) == IS_OBJECT)) {
		return Z_OBJ_HT_P(container)->has_dimension(Z_OBJ_P(container), offset, 0);
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
				lval = zval_get_long_ex(offset, /* is_strict */ true);
				goto str_offset;
			}
			return 0;
		}
	} else {
		return 0;
	}
}

static zend_never_inline bool ZEND_FASTCALL zend_isempty_dim_slow(zval *container, zval *offset EXECUTE_DATA_DC)
{
	if (/*OP2_TYPE == IS_CV &&*/ UNEXPECTED(Z_TYPE_P(offset) == IS_UNDEF)) {
		offset = ZVAL_UNDEFINED_OP2();
	}

	if (/*OP1_TYPE != IS_CONST &&*/ EXPECTED(Z_TYPE_P(container) == IS_OBJECT)) {
		return !Z_OBJ_HT_P(container)->has_dimension(Z_OBJ_P(container), offset, 1);
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
				lval = zval_get_long_ex(offset, /* is_strict */ true);
				goto str_offset;
			}
			return 1;
		}
	} else {
		return 1;
	}
}

static zend_never_inline bool ZEND_FASTCALL zend_array_key_exists_fast(HashTable *ht, zval *key OPLINE_DC EXECUTE_DATA_DC)
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
		return zend_hash_exists(ht, str);
	} else if (EXPECTED(Z_TYPE_P(key) == IS_LONG)) {
		hval = Z_LVAL_P(key);
num_key:
		return zend_hash_index_exists(ht, hval);
	} else if (EXPECTED(Z_ISREF_P(key))) {
		key = Z_REFVAL_P(key);
		goto try_again;
	} else if (Z_TYPE_P(key) == IS_DOUBLE) {
		hval = zend_dval_to_lval_safe(Z_DVAL_P(key));
		goto num_key;
	} else if (Z_TYPE_P(key) == IS_FALSE) {
		hval = 0;
		goto num_key;
	} else if (Z_TYPE_P(key) == IS_TRUE) {
		hval = 1;
		goto num_key;
	} else if (Z_TYPE_P(key) == IS_RESOURCE) {
		zend_use_resource_as_offset(key);
		hval = Z_RES_HANDLE_P(key);
		goto num_key;
	} else if (Z_TYPE_P(key) <= IS_NULL) {
		if (UNEXPECTED(Z_TYPE_P(key) == IS_UNDEF)) {
			ZVAL_UNDEFINED_OP1();
		}
		str = ZSTR_EMPTY_ALLOC();
		goto str_key;
	} else {
		zend_illegal_array_offset_access(key);
		return 0;
	}
}

static ZEND_COLD void ZEND_FASTCALL zend_array_key_exists_error(
		zval *subject, zval *key OPLINE_DC EXECUTE_DATA_DC)
{
	if (Z_TYPE_P(key) == IS_UNDEF) {
		ZVAL_UNDEFINED_OP1();
	}
	if (Z_TYPE_P(subject) == IS_UNDEF) {
		ZVAL_UNDEFINED_OP2();
	}
	if (!EG(exception)) {
		zend_type_error("array_key_exists(): Argument #2 ($array) must be of type array, %s given",
			zend_zval_value_name(subject));
	}
}

static zend_always_inline bool promotes_to_array(zval *val) {
	return Z_TYPE_P(val) <= IS_FALSE
		|| (Z_ISREF_P(val) && Z_TYPE_P(Z_REFVAL_P(val)) <= IS_FALSE);
}

static zend_always_inline bool check_type_array_assignable(zend_type type) {
	if (!ZEND_TYPE_IS_SET(type)) {
		return 1;
	}
	return (ZEND_TYPE_FULL_MASK(type) & MAY_BE_ARRAY) != 0;
}

/* Checks whether an array can be assigned to the reference. Throws error if not assignable. */
ZEND_API bool zend_verify_ref_array_assignable(zend_reference *ref) {
	zend_property_info *prop;
	ZEND_ASSERT(ZEND_REF_HAS_TYPE_SOURCES(ref));
	ZEND_REF_FOREACH_TYPE_SOURCES(ref, prop) {
		if (!check_type_array_assignable(prop->type)) {
			zend_throw_auto_init_in_ref_error(prop);
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

static zend_never_inline bool zend_handle_fetch_obj_flags(
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
					zend_throw_auto_init_in_prop_error(prop_info);
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

static zend_always_inline void zend_fetch_property_address(zval *result, zval *container, uint32_t container_op_type, zval *prop_ptr, uint32_t prop_op_type, void **cache_slot, int type, uint32_t flags OPLINE_DC EXECUTE_DATA_DC)
{
	zval *ptr;
	zend_object *zobj;
	zend_string *name, *tmp_name;

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

			zend_throw_non_object_error(container, prop_ptr OPLINE_CC EXECUTE_DATA_CC);
			ZVAL_ERROR(result);
			return;
		} while (0);
	}

	zobj = Z_OBJ_P(container);
	if (prop_op_type == IS_CONST &&
	    EXPECTED(zobj->ce == CACHED_PTR_EX(cache_slot))) {
		uintptr_t prop_offset = (uintptr_t)CACHED_PTR_EX(cache_slot + 1);

		if (EXPECTED(IS_VALID_PROPERTY_OFFSET(prop_offset))) {
			ptr = OBJ_PROP(zobj, prop_offset);
			if (EXPECTED(Z_TYPE_P(ptr) != IS_UNDEF)) {
				ZVAL_INDIRECT(result, ptr);
				zend_property_info *prop_info = CACHED_PTR_EX(cache_slot + 2);
				if (prop_info) {
					if (UNEXPECTED(prop_info->flags & ZEND_ACC_READONLY)) {
						/* For objects, W/RW/UNSET fetch modes might not actually modify object.
						 * Similar as with magic __get() allow them, but return the value as a copy
						 * to make sure no actual modification is possible. */
						ZEND_ASSERT(type == BP_VAR_W || type == BP_VAR_RW || type == BP_VAR_UNSET);
						if (Z_TYPE_P(ptr) == IS_OBJECT) {
							ZVAL_COPY(result, ptr);
						} else if (Z_PROP_FLAG_P(ptr) & IS_PROP_REINITABLE) {
							Z_PROP_FLAG_P(ptr) &= ~IS_PROP_REINITABLE;
						} else {
							zend_readonly_property_modification_error(prop_info);
							ZVAL_ERROR(result);
						}
						return;
					}
					flags &= ZEND_FETCH_OBJ_FLAGS;
					if (flags) {
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
			ptr = zend_hash_find_known_hash(zobj->properties, Z_STR_P(prop_ptr));
			if (EXPECTED(ptr)) {
				ZVAL_INDIRECT(result, ptr);
				return;
			}
		}
	}

	if (prop_op_type == IS_CONST) {
		name = Z_STR_P(prop_ptr);
	} else {
		name = zval_get_tmp_string(prop_ptr, &tmp_name);
	}
	ptr = zobj->handlers->get_property_ptr_ptr(zobj, name, type, cache_slot);
	if (NULL == ptr) {
		ptr = zobj->handlers->read_property(zobj, name, type, cache_slot, result);
		if (ptr == result) {
			if (UNEXPECTED(Z_ISREF_P(ptr) && Z_REFCOUNT_P(ptr) == 1)) {
				ZVAL_UNREF(ptr);
			}
			goto end;
		}
		if (UNEXPECTED(EG(exception))) {
			ZVAL_ERROR(result);
			goto end;
		}
	} else if (UNEXPECTED(Z_ISERROR_P(ptr))) {
		ZVAL_ERROR(result);
		goto end;
	}

	ZVAL_INDIRECT(result, ptr);
	flags &= ZEND_FETCH_OBJ_FLAGS;
	if (flags) {
		zend_property_info *prop_info;

		if (prop_op_type == IS_CONST) {
			prop_info = CACHED_PTR_EX(cache_slot + 2);
			if (prop_info) {
				if (UNEXPECTED(!zend_handle_fetch_obj_flags(result, ptr, NULL, prop_info, flags))) {
					goto end;
				}
			}
		} else {
			if (UNEXPECTED(!zend_handle_fetch_obj_flags(result, ptr, Z_OBJ_P(container), NULL, flags))) {
				goto end;
			}
		}
	}

end:
	if (prop_op_type != IS_CONST) {
		zend_tmp_string_release(tmp_name);
	}
}

static zend_always_inline void zend_assign_to_property_reference(zval *container, uint32_t container_op_type, zval *prop_ptr, uint32_t prop_op_type, zval *value_ptr OPLINE_DC EXECUTE_DATA_DC)
{
	zval variable, *variable_ptr = &variable;
	void **cache_addr = (prop_op_type == IS_CONST) ? CACHE_ADDR(opline->extended_value & ~ZEND_RETURNS_FUNCTION) : NULL;
	zend_refcounted *garbage = NULL;

	zend_fetch_property_address(variable_ptr, container, container_op_type, prop_ptr, prop_op_type,
		cache_addr, BP_VAR_W, 0 OPLINE_CC EXECUTE_DATA_CC);

	if (EXPECTED(Z_TYPE_P(variable_ptr) == IS_INDIRECT)) {
		variable_ptr = Z_INDIRECT_P(variable_ptr);
		if (/*OP_DATA_TYPE == IS_VAR &&*/
				   (opline->extended_value & ZEND_RETURNS_FUNCTION) &&
				   UNEXPECTED(!Z_ISREF_P(value_ptr))) {

			variable_ptr = zend_wrong_assign_to_variable_reference(
				variable_ptr, value_ptr, &garbage OPLINE_CC EXECUTE_DATA_CC);
		} else {
			zend_property_info *prop_info = NULL;

			if (prop_op_type == IS_CONST) {
				prop_info = (zend_property_info *) CACHED_PTR_EX(cache_addr + 2);
			} else {
				ZVAL_DEREF(container);
				prop_info = zend_object_fetch_property_type_info(Z_OBJ_P(container), variable_ptr);
			}

			if (UNEXPECTED(prop_info)) {
				variable_ptr = zend_assign_to_typed_property_reference(prop_info, variable_ptr, value_ptr, &garbage EXECUTE_DATA_CC);
			} else {
				zend_assign_to_variable_reference(variable_ptr, value_ptr, &garbage);
			}
		}
	} else if (Z_ISERROR_P(variable_ptr)) {
		variable_ptr = &EG(uninitialized_zval);
	} else {
		zend_throw_error(NULL, "Cannot assign by reference to overloaded object");
		zval_ptr_dtor(&variable);
		variable_ptr = &EG(uninitialized_zval);
	}

	if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
		ZVAL_COPY(EX_VAR(opline->result.var), variable_ptr);
	}
	if (garbage) {
		GC_DTOR(garbage);
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

static zend_never_inline zend_result zend_fetch_static_property_address_ex(zval **retval, zend_property_info **prop_info, uint32_t cache_slot, int fetch_type OPLINE_DC EXECUTE_DATA_DC) {
	zend_string *name;
	zend_class_entry *ce;
	zend_property_info *property_info;

	uint8_t op1_type = opline->op1_type, op2_type = opline->op2_type;

	if (EXPECTED(op2_type == IS_CONST)) {
		zval *class_name = RT_CONSTANT(opline, opline->op2);

		ZEND_ASSERT(op1_type != IS_CONST || CACHED_PTR(cache_slot) == NULL);

		if (EXPECTED((ce = CACHED_PTR(cache_slot)) == NULL)) {
			ce = zend_fetch_class_by_name(Z_STR_P(class_name), Z_STR_P(class_name + 1), ZEND_FETCH_CLASS_DEFAULT | ZEND_FETCH_CLASS_EXCEPTION);
			if (UNEXPECTED(ce == NULL)) {
				FREE_OP(op1_type, opline->op1.var);
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
				FREE_OP(op1_type, opline->op1.var);
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
		*retval = zend_std_get_static_property_with_info(ce, name, fetch_type, &property_info);
	} else {
		zend_string *tmp_name;
		zval *varname = get_zval_ptr_undef(opline->op1_type, opline->op1, BP_VAR_R);
		if (EXPECTED(Z_TYPE_P(varname) == IS_STRING)) {
			name = Z_STR_P(varname);
			tmp_name = NULL;
		} else {
			if (op1_type == IS_CV && UNEXPECTED(Z_TYPE_P(varname) == IS_UNDEF)) {
				zval_undefined_cv(opline->op1.var EXECUTE_DATA_CC);
			}
			name = zval_get_tmp_string(varname, &tmp_name);
		}
		*retval = zend_std_get_static_property_with_info(ce, name, fetch_type, &property_info);

		zend_tmp_string_release(tmp_name);

		FREE_OP(op1_type, opline->op1.var);
	}

	if (UNEXPECTED(*retval == NULL)) {
		return FAILURE;
	}

	*prop_info = property_info;

	if (EXPECTED(op1_type == IS_CONST)
			&& EXPECTED(!(property_info->ce->ce_flags & ZEND_ACC_TRAIT))) {
		CACHE_POLYMORPHIC_PTR(cache_slot, ce, *retval);
		CACHE_PTR(cache_slot + sizeof(void *) * 2, property_info);
	}

	return SUCCESS;
}


static zend_always_inline zend_result zend_fetch_static_property_address(zval **retval, zend_property_info **prop_info, uint32_t cache_slot, int fetch_type, int flags OPLINE_DC EXECUTE_DATA_DC) {
	zend_property_info *property_info;

	if (opline->op1_type == IS_CONST && (opline->op2_type == IS_CONST || (opline->op2_type == IS_UNUSED && (opline->op2.num == ZEND_FETCH_CLASS_SELF || opline->op2.num == ZEND_FETCH_CLASS_PARENT))) && EXPECTED(CACHED_PTR(cache_slot) != NULL)) {
		*retval = CACHED_PTR(cache_slot + sizeof(void *));
		property_info = CACHED_PTR(cache_slot + sizeof(void *) * 2);

		if ((fetch_type == BP_VAR_R || fetch_type == BP_VAR_RW)
				&& UNEXPECTED(Z_TYPE_P(*retval) == IS_UNDEF)
				&& UNEXPECTED(ZEND_TYPE_IS_SET(property_info->type))) {
			zend_throw_error(NULL, "Typed static property %s::$%s must not be accessed before initialization",
				ZSTR_VAL(property_info->ce->name),
				zend_get_unmangled_property_name(property_info->name));
			return FAILURE;
		}
	} else {
		zend_result success;
		success = zend_fetch_static_property_address_ex(retval, &property_info, cache_slot, fetch_type OPLINE_CC EXECUTE_DATA_CC);
		if (UNEXPECTED(success != SUCCESS)) {
			return FAILURE;
		}
	}

	flags &= ZEND_FETCH_OBJ_FLAGS;
	if (flags && ZEND_TYPE_IS_SET(property_info->type)) {
		zend_handle_fetch_obj_flags(NULL, *retval, NULL, property_info, flags);
	}

	if (prop_info) {
		*prop_info = property_info;
	}

	return SUCCESS;
}

ZEND_API ZEND_COLD void zend_throw_ref_type_error_type(const zend_property_info *prop1, const zend_property_info *prop2, const zval *zv) {
	zend_string *type1_str = zend_type_to_string(prop1->type);
	zend_string *type2_str = zend_type_to_string(prop2->type);
	zend_type_error("Reference with value of type %s held by property %s::$%s of type %s is not compatible with property %s::$%s of type %s",
		zend_zval_type_name(zv),
		ZSTR_VAL(prop1->ce->name),
		zend_get_unmangled_property_name(prop1->name),
		ZSTR_VAL(type1_str),
		ZSTR_VAL(prop2->ce->name),
		zend_get_unmangled_property_name(prop2->name),
		ZSTR_VAL(type2_str)
	);
	zend_string_release(type1_str);
	zend_string_release(type2_str);
}

ZEND_API ZEND_COLD void zend_throw_ref_type_error_zval(const zend_property_info *prop, const zval *zv) {
	zend_string *type_str = zend_type_to_string(prop->type);
	zend_type_error("Cannot assign %s to reference held by property %s::$%s of type %s",
		zend_zval_value_name(zv),
		ZSTR_VAL(prop->ce->name),
		zend_get_unmangled_property_name(prop->name),
		ZSTR_VAL(type_str)
	);
	zend_string_release(type_str);
}

ZEND_API ZEND_COLD void zend_throw_conflicting_coercion_error(const zend_property_info *prop1, const zend_property_info *prop2, const zval *zv) {
	zend_string *type1_str = zend_type_to_string(prop1->type);
	zend_string *type2_str = zend_type_to_string(prop2->type);
	zend_type_error("Cannot assign %s to reference held by property %s::$%s of type %s and property %s::$%s of type %s, as this would result in an inconsistent type conversion",
		zend_zval_value_name(zv),
		ZSTR_VAL(prop1->ce->name),
		zend_get_unmangled_property_name(prop1->name),
		ZSTR_VAL(type1_str),
		ZSTR_VAL(prop2->ce->name),
		zend_get_unmangled_property_name(prop2->name),
		ZSTR_VAL(type2_str)
	);
	zend_string_release(type1_str);
	zend_string_release(type2_str);
}

/* 1: valid, 0: invalid, -1: may be valid after type coercion */
static zend_always_inline int i_zend_verify_type_assignable_zval(
		const zend_property_info *info, const zval *zv, bool strict) {
	zend_type type = info->type;
	uint32_t type_mask;
	uint8_t zv_type = Z_TYPE_P(zv);

	if (EXPECTED(ZEND_TYPE_CONTAINS_CODE(type, zv_type))) {
		return 1;
	}

	if (ZEND_TYPE_IS_COMPLEX(type) && zv_type == IS_OBJECT
			&& zend_check_and_resolve_property_or_class_constant_class_type(info->ce, info->type, Z_OBJCE_P(zv))) {
		return 1;
	}

	type_mask = ZEND_TYPE_FULL_MASK(type);
	ZEND_ASSERT(!(type_mask & (MAY_BE_CALLABLE|MAY_BE_STATIC)));

	/* SSTH Exception: IS_LONG may be accepted as IS_DOUBLE (converted) */
	if (strict) {
		if ((type_mask & MAY_BE_DOUBLE) && zv_type == IS_LONG) {
			return -1;
		}
		return 0;
	}

	/* NULL may be accepted only by nullable hints (this is already checked) */
	if (zv_type == IS_NULL) {
		return 0;
	}

	/* Does not contain any type to which a coercion is possible */
	if (!(type_mask & (MAY_BE_LONG|MAY_BE_DOUBLE|MAY_BE_STRING))
			&& (type_mask & MAY_BE_BOOL) != MAY_BE_BOOL) {
		return 0;
	}

	/* Coercion may be necessary, check separately */
	return -1;
}

ZEND_API bool ZEND_FASTCALL zend_verify_ref_assignable_zval(zend_reference *ref, zval *zv, bool strict)
{
	const zend_property_info *prop;

	/* The value must satisfy each property type, and coerce to the same value for each property
	 * type. Remember the first coerced type and value we've seen for this purpose. */
	const zend_property_info *first_prop = NULL;
	zval coerced_value;
	ZVAL_UNDEF(&coerced_value);

	ZEND_ASSERT(Z_TYPE_P(zv) != IS_REFERENCE);
	ZEND_REF_FOREACH_TYPE_SOURCES(ref, prop) {
		int result = i_zend_verify_type_assignable_zval(prop, zv, strict);
		if (result == 0) {
type_error:
			zend_throw_ref_type_error_zval(prop, zv);
			zval_ptr_dtor(&coerced_value);
			return 0;
		}

		if (result < 0) {
			if (!first_prop) {
				first_prop = prop;
				ZVAL_COPY(&coerced_value, zv);
				if (!zend_verify_weak_scalar_type_hint(
						ZEND_TYPE_FULL_MASK(prop->type), &coerced_value)) {
					goto type_error;
				}
			} else if (Z_ISUNDEF(coerced_value)) {
				/* A previous property did not require coercion, but this one does,
				 * so they are incompatible. */
				goto conflicting_coercion_error;
			} else {
				zval tmp;
				ZVAL_COPY(&tmp, zv);
				if (!zend_verify_weak_scalar_type_hint(ZEND_TYPE_FULL_MASK(prop->type), &tmp)) {
					zval_ptr_dtor(&tmp);
					goto type_error;
				}
				if (!zend_is_identical(&coerced_value, &tmp)) {
					zval_ptr_dtor(&tmp);
					goto conflicting_coercion_error;
				}
				zval_ptr_dtor(&tmp);
			}
		} else {
			if (!first_prop) {
				first_prop = prop;
			} else if (!Z_ISUNDEF(coerced_value)) {
				/* A previous property required coercion, but this one doesn't,
				 * so they are incompatible. */
conflicting_coercion_error:
				zend_throw_conflicting_coercion_error(first_prop, prop, zv);
				zval_ptr_dtor(&coerced_value);
				return 0;
			}
		}
	} ZEND_REF_FOREACH_TYPE_SOURCES_END();

	if (!Z_ISUNDEF(coerced_value)) {
		zval_ptr_dtor(zv);
		ZVAL_COPY_VALUE(zv, &coerced_value);
	}

	return 1;
}

static zend_always_inline void i_zval_ptr_dtor_noref(zval *zval_ptr) {
	if (Z_REFCOUNTED_P(zval_ptr)) {
		zend_refcounted *ref = Z_COUNTED_P(zval_ptr);
		ZEND_ASSERT(Z_TYPE_P(zval_ptr) != IS_REFERENCE);
		GC_DTOR_NO_REF(ref);
	}
}

ZEND_API zval* zend_assign_to_typed_ref_ex(zval *variable_ptr, zval *orig_value, uint8_t value_type, bool strict, zend_refcounted **garbage_ptr)
{
	bool ret;
	zval value;
	zend_refcounted *ref = NULL;

	if (Z_ISREF_P(orig_value)) {
		ref = Z_COUNTED_P(orig_value);
		orig_value = Z_REFVAL_P(orig_value);
	}

	ZVAL_COPY(&value, orig_value);
	ret = zend_verify_ref_assignable_zval(Z_REF_P(variable_ptr), &value, strict);
	variable_ptr = Z_REFVAL_P(variable_ptr);
	if (EXPECTED(ret)) {
		if (Z_REFCOUNTED_P(variable_ptr)) {
			*garbage_ptr = Z_COUNTED_P(variable_ptr);
		}
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

ZEND_API zval* zend_assign_to_typed_ref(zval *variable_ptr, zval *orig_value, uint8_t value_type, bool strict)
{
	zend_refcounted *garbage = NULL;
	zval *result = zend_assign_to_typed_ref_ex(variable_ptr, orig_value, value_type, strict, &garbage);
	if (garbage) {
		GC_DTOR_NO_REF(garbage);
	}
	return result;
}

ZEND_API bool ZEND_FASTCALL zend_verify_prop_assignable_by_ref_ex(const zend_property_info *prop_info, zval *orig_val, bool strict, zend_verify_prop_assignable_by_ref_context context) {
	zval *val = orig_val;
	if (Z_ISREF_P(val) && ZEND_REF_HAS_TYPE_SOURCES(Z_REF_P(val))) {
		int result;

		val = Z_REFVAL_P(val);
		result = i_zend_verify_type_assignable_zval(prop_info, val, strict);
		if (result > 0) {
			return 1;
		}

		if (result < 0) {
			/* This is definitely an error, but we still need to determined why: Either because
			 * the value is simply illegal for the type, or because or a conflicting coercion. */
			zval tmp;
			ZVAL_COPY(&tmp, val);
			if (zend_verify_weak_scalar_type_hint(ZEND_TYPE_FULL_MASK(prop_info->type), &tmp)) {
				const zend_property_info *ref_prop = ZEND_REF_FIRST_SOURCE(Z_REF_P(orig_val));
				zend_throw_ref_type_error_type(ref_prop, prop_info, val);
				zval_ptr_dtor(&tmp);
				return 0;
			}
			zval_ptr_dtor(&tmp);
		}
	} else {
		ZVAL_DEREF(val);
		if (i_zend_check_property_type(prop_info, val, strict)) {
			return 1;
		}
	}

	if (EXPECTED(context == ZEND_VERIFY_PROP_ASSIGNABLE_BY_REF_CONTEXT_ASSIGNMENT)) {
		zend_verify_property_type_error(prop_info, val);
	} else {
		ZEND_ASSERT(context == ZEND_VERIFY_PROP_ASSIGNABLE_BY_REF_CONTEXT_MAGIC_GET);
		zend_magic_get_property_type_inconsistency_error(prop_info, val);
	}

	return 0;
}

ZEND_API bool ZEND_FASTCALL zend_verify_prop_assignable_by_ref(const zend_property_info *prop_info, zval *orig_val, bool strict) {
	return zend_verify_prop_assignable_by_ref_ex(prop_info, orig_val, strict, ZEND_VERIFY_PROP_ASSIGNABLE_BY_REF_CONTEXT_ASSIGNMENT);
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

ZEND_API void ZEND_FASTCALL zend_ref_del_type_source(zend_property_info_source_list *source_list, const zend_property_info *prop)
{
	zend_property_info_list *list = ZEND_PROPERTY_INFO_SOURCE_TO_LIST(source_list->list);
	zend_property_info **ptr, **end;

	ZEND_ASSERT(prop);
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
				zend_error(E_WARNING, "Undefined variable $this");
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
	zend_throw_error(NULL, "Call to %s %s::__clone() from %s%s",
		zend_visibility_string(clone->common.fn_flags), ZSTR_VAL(clone->common.scope->name),
		scope ? "scope " : "global scope",
		scope ? ZSTR_VAL(scope->name) : ""
	);
}

#if ZEND_INTENSIVE_DEBUGGING

#define CHECK_SYMBOL_TABLES()													\
	zend_hash_apply(&EG(symbol_table), zend_check_symbol);			\
	if (&EG(symbol_table)!=EX(symbol_table)) {							\
		zend_hash_apply(EX(symbol_table), zend_check_symbol);	\
	}

static void zend_check_symbol(zval *pz)
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
	/* Clean before putting into the cache, since clean could call dtors,
	 * which could use the cached hash. Also do this before the check for
	 * available cache slots, as those may be used by a dtor as well. */
	zend_symtable_clean(symbol_table);
	if (EG(symtable_cache_ptr) >= EG(symtable_cache_limit)) {
		zend_array_destroy(symbol_table);
	} else {
		*(EG(symtable_cache_ptr)++) = symbol_table;
	}
}
/* }}} */

static zend_always_inline void i_free_compiled_variables(zend_execute_data *execute_data) /* {{{ */
{
	zval *cv = EX_VAR_NUM(0);
	int count = EX(func)->op_array.last_var;
	while (EXPECTED(count != 0)) {
		i_zval_ptr_dtor(cv);
		cv++;
		count--;
	}
}
/* }}} */

ZEND_API void ZEND_FASTCALL zend_free_compiled_variables(zend_execute_data *execute_data) /* {{{ */
{
	i_free_compiled_variables(execute_data);
}
/* }}} */

#define ZEND_VM_INTERRUPT_CHECK() do { \
		if (UNEXPECTED(zend_atomic_bool_load_ex(&EG(vm_interrupt)))) { \
			ZEND_VM_INTERRUPT(); \
		} \
	} while (0)

#define ZEND_VM_LOOP_INTERRUPT_CHECK() do { \
		if (UNEXPECTED(zend_atomic_bool_load_ex(&EG(vm_interrupt)))) { \
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

static zend_always_inline void i_init_func_execute_data(zend_op_array *op_array, zval *return_value, bool may_be_trampoline EXECUTE_DATA_DC) /* {{{ */
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

	if (op_array->last_var) {
		zend_attach_symbol_table(execute_data);
	}

	if (!ZEND_MAP_PTR(op_array->run_time_cache)) {
		void *ptr;

		ZEND_ASSERT(op_array->fn_flags & ZEND_ACC_HEAP_RT_CACHE);
		ptr = emalloc(op_array->cache_size);
		ZEND_MAP_PTR_INIT(op_array->run_time_cache, ptr);
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

zend_execute_data *zend_vm_stack_copy_call_frame(zend_execute_data *call, uint32_t passed_args, uint32_t additional_args) /* {{{ */
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

static zend_always_inline zend_generator *zend_get_running_generator(EXECUTE_DATA_D) /* {{{ */
{
	/* The generator object is stored in EX(return_value) */
	zend_generator *generator = (zend_generator *) EX(return_value);
	/* However control may currently be delegated to another generator.
	 * That's the one we're interested in. */
	return generator;
}
/* }}} */

ZEND_API void zend_unfinished_calls_gc(zend_execute_data *execute_data, zend_execute_data *call, uint32_t op_num, zend_get_gc_buffer *buf) /* {{{ */
{
	zend_op *opline = EX(func)->op_array.opcodes + op_num;
	int level;
	int do_exit;
	uint32_t num_args;

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
		/* find the number of actually passed arguments */
		level = 0;
		do_exit = 0;
		num_args = ZEND_CALL_NUM_ARGS(call);
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
						num_args = 0;
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
						/* For named args, the number of arguments is up to date. */
						if (opline->op2_type != IS_CONST) {
							num_args = opline->op2.num;
						}
						do_exit = 1;
					}
					break;
				case ZEND_SEND_ARRAY:
				case ZEND_SEND_UNPACK:
				case ZEND_CHECK_UNDEF_ARGS:
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

		if (EXPECTED(num_args > 0)) {
			zval *p = ZEND_CALL_ARG(call, 1);
			do {
				zend_get_gc_buffer_add_zval(buf, p);
				p++;
			} while (--num_args);
		}
		if (ZEND_CALL_INFO(call) & ZEND_CALL_RELEASE_THIS) {
			zend_get_gc_buffer_add_obj(buf, Z_OBJ(call->This));
		}
		if (ZEND_CALL_INFO(call) & ZEND_CALL_HAS_EXTRA_NAMED_PARAMS) {
			zval *val;
			ZEND_HASH_FOREACH_VAL(call->extra_named_params, val) {
				zend_get_gc_buffer_add_zval(buf, val);
			} ZEND_HASH_FOREACH_END();
		}
		if (call->func->common.fn_flags & ZEND_ACC_CLOSURE) {
			zend_get_gc_buffer_add_obj(buf, ZEND_CLOSURE_OBJECT(call->func));
		}

		call = call->prev_execute_data;
	} while (call);
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
							/* For named args, the number of arguments is up to date. */
							if (opline->op2_type != IS_CONST) {
								ZEND_CALL_NUM_ARGS(call) = opline->op2.num;
							}
							do_exit = 1;
						}
						break;
					case ZEND_SEND_ARRAY:
					case ZEND_SEND_UNPACK:
					case ZEND_CHECK_UNDEF_ARGS:
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
			if (ZEND_CALL_INFO(call) & ZEND_CALL_HAS_EXTRA_NAMED_PARAMS) {
				zend_free_extra_named_params(call->extra_named_params);
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
					if (E_HAS_ONLY_FATAL_ERRORS(EG(error_reporting))
							&& !E_HAS_ONLY_FATAL_ERRORS(Z_LVAL_P(var))) {
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

ZEND_API ZEND_ATTRIBUTE_DEPRECATED HashTable *zend_unfinished_execution_gc(zend_execute_data *execute_data, zend_execute_data *call, zend_get_gc_buffer *gc_buffer)
{
	bool suspended_by_yield = false;

	if (Z_TYPE_INFO(EX(This)) & ZEND_CALL_GENERATOR) {
		ZEND_ASSERT(EX(return_value));

		/* The generator object is stored in EX(return_value) */
		zend_generator *generator = (zend_generator*) EX(return_value);
		ZEND_ASSERT(execute_data == generator->execute_data);

		suspended_by_yield = !(generator->flags & ZEND_GENERATOR_CURRENTLY_RUNNING);
	}

	return zend_unfinished_execution_gc_ex(execute_data, call, gc_buffer, suspended_by_yield);
}

ZEND_API HashTable *zend_unfinished_execution_gc_ex(zend_execute_data *execute_data, zend_execute_data *call, zend_get_gc_buffer *gc_buffer, bool suspended_by_yield)
{
	if (!EX(func) || !ZEND_USER_CODE(EX(func)->common.type)) {
		return NULL;
	}

	zend_op_array *op_array = &EX(func)->op_array;

	if (!(EX_CALL_INFO() & ZEND_CALL_HAS_SYMBOL_TABLE)) {
		uint32_t i, num_cvs = EX(func)->op_array.last_var;
		for (i = 0; i < num_cvs; i++) {
			zend_get_gc_buffer_add_zval(gc_buffer, EX_VAR_NUM(i));
		}
	}

	if (EX_CALL_INFO() & ZEND_CALL_FREE_EXTRA_ARGS) {
		zval *zv = EX_VAR_NUM(op_array->last_var + op_array->T);
		zval *end = zv + (EX_NUM_ARGS() - op_array->num_args);
		while (zv != end) {
			zend_get_gc_buffer_add_zval(gc_buffer, zv++);
		}
	}

	if (EX_CALL_INFO() & ZEND_CALL_RELEASE_THIS) {
		zend_get_gc_buffer_add_obj(gc_buffer, Z_OBJ(execute_data->This));
	}
	if (EX_CALL_INFO() & ZEND_CALL_CLOSURE) {
		zend_get_gc_buffer_add_obj(gc_buffer, ZEND_CLOSURE_OBJECT(EX(func)));
	}
	if (EX_CALL_INFO() & ZEND_CALL_HAS_EXTRA_NAMED_PARAMS) {
		zval extra_named_params;
		ZVAL_ARR(&extra_named_params, EX(extra_named_params));
		zend_get_gc_buffer_add_zval(gc_buffer, &extra_named_params);
	}

	if (call) {
		uint32_t op_num = execute_data->opline - op_array->opcodes;
		if (suspended_by_yield) {
			/* When the execution was suspended by yield, EX(opline) points to
			 * next opline to execute. Otherwise, it points to the opline that
			 * suspended execution. */
			op_num--;
			ZEND_ASSERT(EX(func)->op_array.opcodes[op_num].opcode == ZEND_YIELD
				|| EX(func)->op_array.opcodes[op_num].opcode == ZEND_YIELD_FROM);
		}
		zend_unfinished_calls_gc(execute_data, call, op_num, gc_buffer);
	}

	if (execute_data->opline != op_array->opcodes) {
		uint32_t i, op_num = execute_data->opline - op_array->opcodes - 1;
		for (i = 0; i < op_array->last_live_range; i++) {
			const zend_live_range *range = &op_array->live_range[i];
			if (range->start > op_num) {
				break;
			} else if (op_num < range->end) {
				uint32_t kind = range->var & ZEND_LIVE_MASK;
				uint32_t var_num = range->var & ~ZEND_LIVE_MASK;
				zval *var = EX_VAR(var_num);
				if (kind == ZEND_LIVE_TMPVAR || kind == ZEND_LIVE_LOOP) {
					zend_get_gc_buffer_add_zval(gc_buffer, var);
				}
			}
		}
	}

	if (EX_CALL_INFO() & ZEND_CALL_HAS_SYMBOL_TABLE) {
		return execute_data->symbol_table;
	} else {
		return NULL;
	}
}

#if ZEND_VM_SPEC
static void zend_swap_operands(zend_op *op) /* {{{ */
{
	znode_op     tmp;
	uint8_t   tmp_type;

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
			if (fbc->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE) {
				zend_string_release_ex(fbc->common.function_name, 0);
				zend_free_trampoline(fbc);
			}
			return NULL;
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

static zend_never_inline zend_execute_data *zend_init_dynamic_call_object(zend_object *function, uint32_t num_args) /* {{{ */
{
	zend_function *fbc;
	void *object_or_called_scope;
	zend_class_entry *called_scope;
	zend_object *object;
	uint32_t call_info;

	if (EXPECTED(function->handlers->get_closure) &&
	    EXPECTED(function->handlers->get_closure(function, &called_scope, &fbc, &object, 0) == SUCCESS)) {

		object_or_called_scope = called_scope;
		if (EXPECTED(fbc->common.fn_flags & ZEND_ACC_CLOSURE)) {
			/* Delay closure destruction until its invocation */
			GC_ADDREF(ZEND_CLOSURE_OBJECT(fbc));
			ZEND_ASSERT(ZEND_ACC_FAKE_CLOSURE == ZEND_CALL_FAKE_CLOSURE);
			call_info = ZEND_CALL_NESTED_FUNCTION | ZEND_CALL_DYNAMIC | ZEND_CALL_CLOSURE |
				(fbc->common.fn_flags & ZEND_ACC_FAKE_CLOSURE);
			if (object) {
				call_info |= ZEND_CALL_HAS_THIS;
				object_or_called_scope = object;
			}
		} else {
			call_info = ZEND_CALL_NESTED_FUNCTION | ZEND_CALL_DYNAMIC;
			if (object) {
				call_info |= ZEND_CALL_RELEASE_THIS | ZEND_CALL_HAS_THIS;
				GC_ADDREF(object); /* For $this pointer */
				object_or_called_scope = object;
			}
		}
	} else {
		zend_throw_error(NULL, "Object of type %s is not callable", ZSTR_VAL(function->ce->name));
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
				if (fbc->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE) {
					zend_string_release_ex(fbc->common.function_name, 0);
					zend_free_trampoline(fbc);
				}
				return NULL;
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
		zend_throw_error(NULL, "Array callback must have exactly two elements");
		return NULL;
	}

	if (EXPECTED(fbc->type == ZEND_USER_FUNCTION) && UNEXPECTED(!RUN_TIME_CACHE(&fbc->op_array))) {
		init_func_run_time_cache(&fbc->op_array);
	}

	return zend_vm_stack_push_call_frame(call_info,
		fbc, num_args, object_or_called_scope);
}
/* }}} */

#define ZEND_FAKE_OP_ARRAY ((zend_op_array*)(intptr_t)-1)

static zend_never_inline zend_op_array* ZEND_FASTCALL zend_include_or_eval(zval *inc_filename_zv, int type) /* {{{ */
{
	zend_op_array *new_op_array = NULL;
	zend_string *tmp_inc_filename;
	zend_string *inc_filename = zval_try_get_tmp_string(inc_filename_zv, &tmp_inc_filename);
	if (UNEXPECTED(!inc_filename)) {
		return NULL;
	}

	switch (type) {
		case ZEND_INCLUDE_ONCE:
		case ZEND_REQUIRE_ONCE: {
				zend_file_handle file_handle;
				zend_string *resolved_path;

				resolved_path = zend_resolve_path(inc_filename);
				if (EXPECTED(resolved_path)) {
					if (zend_hash_exists(&EG(included_files), resolved_path)) {
						new_op_array = ZEND_FAKE_OP_ARRAY;
						zend_string_release_ex(resolved_path, 0);
						break;
					}
				} else if (UNEXPECTED(EG(exception))) {
					break;
				} else if (UNEXPECTED(strlen(ZSTR_VAL(inc_filename)) != ZSTR_LEN(inc_filename))) {
					zend_message_dispatcher(
						(type == ZEND_INCLUDE_ONCE) ?
							ZMSG_FAILED_INCLUDE_FOPEN : ZMSG_FAILED_REQUIRE_FOPEN,
							ZSTR_VAL(inc_filename));
					break;
				} else {
					resolved_path = zend_string_copy(inc_filename);
				}

				zend_stream_init_filename_ex(&file_handle, resolved_path);
				if (SUCCESS == zend_stream_open(&file_handle)) {

					if (!file_handle.opened_path) {
						file_handle.opened_path = zend_string_copy(resolved_path);
					}

					if (zend_hash_add_empty_element(&EG(included_files), file_handle.opened_path)) {
						new_op_array = zend_compile_file(&file_handle, (type==ZEND_INCLUDE_ONCE?ZEND_INCLUDE:ZEND_REQUIRE));
					} else {
						new_op_array = ZEND_FAKE_OP_ARRAY;
					}
				} else if (!EG(exception)) {
					zend_message_dispatcher(
						(type == ZEND_INCLUDE_ONCE) ?
							ZMSG_FAILED_INCLUDE_FOPEN : ZMSG_FAILED_REQUIRE_FOPEN,
							ZSTR_VAL(inc_filename));
				}
				zend_destroy_file_handle(&file_handle);
				zend_string_release_ex(resolved_path, 0);
			}
			break;
		case ZEND_INCLUDE:
		case ZEND_REQUIRE:
			if (UNEXPECTED(strlen(ZSTR_VAL(inc_filename)) != ZSTR_LEN(inc_filename))) {
				zend_message_dispatcher(
					(type == ZEND_INCLUDE) ?
						ZMSG_FAILED_INCLUDE_FOPEN : ZMSG_FAILED_REQUIRE_FOPEN,
						ZSTR_VAL(inc_filename));
				break;
			}
			new_op_array = compile_filename(type, inc_filename);
			break;
		case ZEND_EVAL: {
				char *eval_desc = zend_make_compiled_string_description("eval()'d code");
				new_op_array = zend_compile_string(inc_filename, eval_desc, ZEND_COMPILE_POSITION_AFTER_OPEN_TAG);
				efree(eval_desc);
			}
			break;
		EMPTY_SWITCH_DEFAULT_CASE()
	}

	zend_tmp_string_release(tmp_inc_filename);
	return new_op_array;
}
/* }}} */

static zend_never_inline bool ZEND_FASTCALL zend_fe_reset_iterator(zval *array_ptr, int by_ref OPLINE_DC EXECUTE_DATA_DC) /* {{{ */
{
	zend_class_entry *ce = Z_OBJCE_P(array_ptr);
	zend_object_iterator *iter = ce->get_iterator(ce, array_ptr, by_ref);
	bool is_empty;

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

static zend_always_inline zend_result _zend_quick_get_constant(
		const zval *key, uint32_t flags, bool check_defined_only OPLINE_DC EXECUTE_DATA_DC) /* {{{ */
{
	zval *zv;
	zend_constant *c = NULL;

	/* null/true/false are resolved during compilation, so don't check for them here. */
	zv = zend_hash_find_known_hash(EG(zend_constants), Z_STR_P(key));
	if (zv) {
		c = (zend_constant*)Z_PTR_P(zv);
	} else if (flags & IS_CONSTANT_UNQUALIFIED_IN_NAMESPACE) {
		key++;
		zv = zend_hash_find_known_hash(EG(zend_constants), Z_STR_P(key));
		if (zv) {
			c = (zend_constant*)Z_PTR_P(zv);
		}
	}

	if (!c) {
		if (!check_defined_only) {
			zend_throw_error(NULL, "Undefined constant \"%s\"", Z_STRVAL_P(RT_CONSTANT(opline, opline->op2)));
			ZVAL_UNDEF(EX_VAR(opline->result.var));
		}
		return FAILURE;
	}

	if (!check_defined_only) {
		ZVAL_COPY_OR_DUP(EX_VAR(opline->result.var), &c->value);
		if (ZEND_CONSTANT_FLAGS(c) & CONST_DEPRECATED) {
			zend_error(E_DEPRECATED, "Constant %s is deprecated", ZSTR_VAL(c->name));
			return SUCCESS;
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

static zend_never_inline zend_result ZEND_FASTCALL zend_quick_check_constant(
		const zval *key OPLINE_DC EXECUTE_DATA_DC) /* {{{ */
{
	return _zend_quick_get_constant(key, 0, 1 OPLINE_CC EXECUTE_DATA_CC);
} /* }}} */

static zend_always_inline uint32_t zend_get_arg_offset_by_name(
		zend_function *fbc, zend_string *arg_name, void **cache_slot) {
	if (EXPECTED(*cache_slot == fbc)) {
		return *(uintptr_t *)(cache_slot + 1);
	}

	// TODO: Use a hash table?
	uint32_t num_args = fbc->common.num_args;
	if (EXPECTED(fbc->type == ZEND_USER_FUNCTION)
			|| EXPECTED(fbc->common.fn_flags & ZEND_ACC_USER_ARG_INFO)) {
		for (uint32_t i = 0; i < num_args; i++) {
			zend_arg_info *arg_info = &fbc->op_array.arg_info[i];
			if (zend_string_equals(arg_name, arg_info->name)) {
				*cache_slot = fbc;
				*(uintptr_t *)(cache_slot + 1) = i;
				return i;
			}
		}
	} else {
		for (uint32_t i = 0; i < num_args; i++) {
			zend_internal_arg_info *arg_info = &fbc->internal_function.arg_info[i];
			size_t len = strlen(arg_info->name);
			if (zend_string_equals_cstr(arg_name, arg_info->name, len)) {
				*cache_slot = fbc;
				*(uintptr_t *)(cache_slot + 1) = i;
				return i;
			}
		}
	}

	if (fbc->common.fn_flags & ZEND_ACC_VARIADIC) {
		*cache_slot = fbc;
		*(uintptr_t *)(cache_slot + 1) = fbc->common.num_args;
		return fbc->common.num_args;
	}

	return (uint32_t) -1;
}

zval * ZEND_FASTCALL zend_handle_named_arg(
		zend_execute_data **call_ptr, zend_string *arg_name,
		uint32_t *arg_num_ptr, void **cache_slot) {
	zend_execute_data *call = *call_ptr;
	zend_function *fbc = call->func;
	uint32_t arg_offset = zend_get_arg_offset_by_name(fbc, arg_name, cache_slot);
	if (UNEXPECTED(arg_offset == (uint32_t) -1)) {
		zend_throw_error(NULL, "Unknown named parameter $%s", ZSTR_VAL(arg_name));
		return NULL;
	}

	zval *arg;
	if (UNEXPECTED(arg_offset == fbc->common.num_args)) {
		/* Unknown named parameter that will be collected into a variadic. */
		if (!(ZEND_CALL_INFO(call) & ZEND_CALL_HAS_EXTRA_NAMED_PARAMS)) {
			ZEND_ADD_CALL_FLAG(call, ZEND_CALL_HAS_EXTRA_NAMED_PARAMS);
			call->extra_named_params = zend_new_array(0);
		}

		arg = zend_hash_add_empty_element(call->extra_named_params, arg_name);
		if (!arg) {
			zend_throw_error(NULL, "Named parameter $%s overwrites previous argument",
				ZSTR_VAL(arg_name));
			return NULL;
		}
		*arg_num_ptr = arg_offset + 1;
		return arg;
	}

	uint32_t current_num_args = ZEND_CALL_NUM_ARGS(call);
	// TODO: We may wish to optimize the arg_offset == current_num_args case,
	// which is probably common (if the named parameters are in order of declaration).
	if (arg_offset >= current_num_args) {
		uint32_t new_num_args = arg_offset + 1;
		ZEND_CALL_NUM_ARGS(call) = new_num_args;

		uint32_t num_extra_args = new_num_args - current_num_args;
		zend_vm_stack_extend_call_frame(call_ptr, current_num_args, num_extra_args);
		call = *call_ptr;

		arg = ZEND_CALL_VAR_NUM(call, arg_offset);
		if (num_extra_args > 1) {
			zval *zv = ZEND_CALL_VAR_NUM(call, current_num_args);
			do {
				ZVAL_UNDEF(zv);
				zv++;
			} while (zv != arg);
			ZEND_ADD_CALL_FLAG(call, ZEND_CALL_MAY_HAVE_UNDEF);
		}
	} else {
		arg = ZEND_CALL_VAR_NUM(call, arg_offset);
		if (UNEXPECTED(!Z_ISUNDEF_P(arg))) {
			zend_throw_error(NULL, "Named parameter $%s overwrites previous argument",
				ZSTR_VAL(arg_name));
			return NULL;
		}
	}

	*arg_num_ptr = arg_offset + 1;
	return arg;
}

static zend_execute_data *start_fake_frame(zend_execute_data *call, const zend_op *opline) {
	zend_execute_data *old_prev_execute_data = call->prev_execute_data;
	call->prev_execute_data = EG(current_execute_data);
	call->opline = opline;
	EG(current_execute_data) = call;
	return old_prev_execute_data;
}

static void end_fake_frame(zend_execute_data *call, zend_execute_data *old_prev_execute_data) {
	zend_execute_data *prev_execute_data = call->prev_execute_data;
	EG(current_execute_data) = prev_execute_data;
	call->prev_execute_data = old_prev_execute_data;
	if (UNEXPECTED(EG(exception)) && ZEND_USER_CODE(prev_execute_data->func->common.type)) {
		zend_rethrow_exception(prev_execute_data);
	}
}

ZEND_API zend_result ZEND_FASTCALL zend_handle_undef_args(zend_execute_data *call) {
	zend_function *fbc = call->func;
	if (fbc->type == ZEND_USER_FUNCTION) {
		zend_op_array *op_array = &fbc->op_array;
		uint32_t num_args = ZEND_CALL_NUM_ARGS(call);
		for (uint32_t i = 0; i < num_args; i++) {
			zval *arg = ZEND_CALL_VAR_NUM(call, i);
			if (!Z_ISUNDEF_P(arg)) {
				continue;
			}

			zend_op *opline = &op_array->opcodes[i];
			if (EXPECTED(opline->opcode == ZEND_RECV_INIT)) {
				zval *default_value = RT_CONSTANT(opline, opline->op2);
				if (Z_OPT_TYPE_P(default_value) == IS_CONSTANT_AST) {
					if (UNEXPECTED(!RUN_TIME_CACHE(op_array))) {
						init_func_run_time_cache(op_array);
					}

					void *run_time_cache = RUN_TIME_CACHE(op_array);
					zval *cache_val =
						(zval *) ((char *) run_time_cache + Z_CACHE_SLOT_P(default_value));

					if (Z_TYPE_P(cache_val) != IS_UNDEF) {
						/* We keep in cache only not refcounted values */
						ZVAL_COPY_VALUE(arg, cache_val);
					} else {
						/* Update constant inside a temporary zval, to make sure the CONSTANT_AST
						 * value is not accessible through back traces. */
						zval tmp;
						ZVAL_COPY(&tmp, default_value);
						zend_execute_data *old = start_fake_frame(call, opline);
						zend_result ret = zval_update_constant_ex(&tmp, fbc->op_array.scope);
						end_fake_frame(call, old);
						if (UNEXPECTED(ret == FAILURE)) {
							zval_ptr_dtor_nogc(&tmp);
							return FAILURE;
						}
						ZVAL_COPY_VALUE(arg, &tmp);
						if (!Z_REFCOUNTED(tmp)) {
							ZVAL_COPY_VALUE(cache_val, &tmp);
						}
					}
				} else {
					ZVAL_COPY(arg, default_value);
				}
			} else {
				ZEND_ASSERT(opline->opcode == ZEND_RECV);
				zend_execute_data *old = start_fake_frame(call, opline);
				zend_argument_error(zend_ce_argument_count_error, i + 1, "not passed");
				end_fake_frame(call, old);
				return FAILURE;
			}
		}

		return SUCCESS;
	} else {
		if (fbc->common.fn_flags & ZEND_ACC_USER_ARG_INFO) {
			/* Magic function, let it deal with it. */
			return SUCCESS;
		}

		uint32_t num_args = ZEND_CALL_NUM_ARGS(call);
		for (uint32_t i = 0; i < num_args; i++) {
			zval *arg = ZEND_CALL_VAR_NUM(call, i);
			if (!Z_ISUNDEF_P(arg)) {
				continue;
			}

			zend_internal_arg_info *arg_info = &fbc->internal_function.arg_info[i];
			if (i < fbc->common.required_num_args) {
				zend_execute_data *old = start_fake_frame(call, NULL);
				zend_argument_error(zend_ce_argument_count_error, i + 1, "not passed");
				end_fake_frame(call, old);
				return FAILURE;
			}

			zval default_value;
			if (zend_get_default_from_internal_arg_info(&default_value, arg_info) == FAILURE) {
				zend_execute_data *old = start_fake_frame(call, NULL);
				zend_argument_error(zend_ce_argument_count_error, i + 1,
					"must be passed explicitly, because the default value is not known");
				end_fake_frame(call, old);
				return FAILURE;
			}

			if (Z_TYPE(default_value) == IS_CONSTANT_AST) {
				zend_execute_data *old = start_fake_frame(call, NULL);
				zend_result ret = zval_update_constant_ex(&default_value, fbc->common.scope);
				end_fake_frame(call, old);
				if (ret == FAILURE) {
					return FAILURE;
				}
			}

			ZVAL_COPY_VALUE(arg, &default_value);
			if (ZEND_ARG_SEND_MODE(arg_info) & ZEND_SEND_BY_REF) {
				ZVAL_NEW_REF(arg, arg);
			}
		}
	}

	return SUCCESS;
}

ZEND_API void ZEND_FASTCALL zend_free_extra_named_params(zend_array *extra_named_params)
{
	/* Extra named params may be shared. */
	zend_array_release(extra_named_params);
}

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
#elif defined(ZEND_VM_TRACE_LINES)
# include "zend_vm_trace_lines.h"
#elif defined(ZEND_VM_TRACE_MAP)
# include "zend_vm_trace_map.h"
#endif

#define ZEND_VM_NEXT_OPCODE_EX(check_exception, skip) \
	CHECK_SYMBOL_TABLES() \
	if (check_exception) { \
		OPLINE = EX(opline) + (skip); \
	} else { \
		ZEND_ASSERT(!EG(exception)); \
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


#define ZEND_VM_REPEATABLE_OPCODE \
	do {
#define ZEND_VM_REPEAT_OPCODE(_opcode) \
	} while (UNEXPECTED((++opline)->opcode == _opcode)); \
	OPLINE = opline; \
	ZEND_VM_CONTINUE()
#define ZEND_VM_SMART_BRANCH(_result, _check) do { \
		if ((_check) && UNEXPECTED(EG(exception))) { \
			OPLINE = EX(opline); \
		} else if (EXPECTED(opline->result_type == (IS_SMART_BRANCH_JMPZ|IS_TMP_VAR))) { \
			if (_result) { \
				ZEND_VM_SET_NEXT_OPCODE(opline + 2); \
			} else { \
				ZEND_VM_SET_OPCODE(OP_JMP_ADDR(opline + 1, (opline+1)->op2)); \
			} \
		} else if (EXPECTED(opline->result_type == (IS_SMART_BRANCH_JMPNZ|IS_TMP_VAR))) { \
			if (!(_result)) { \
				ZEND_VM_SET_NEXT_OPCODE(opline + 2); \
			} else { \
				ZEND_VM_SET_OPCODE(OP_JMP_ADDR(opline + 1, (opline+1)->op2)); \
			} \
		} else { \
			ZVAL_BOOL(EX_VAR(opline->result.var), _result); \
			ZEND_VM_SET_NEXT_OPCODE(opline + 1); \
		} \
		ZEND_VM_CONTINUE(); \
	} while (0)
#define ZEND_VM_SMART_BRANCH_JMPZ(_result, _check) do { \
		if ((_check) && UNEXPECTED(EG(exception))) { \
			OPLINE = EX(opline); \
		} else if (_result) { \
			ZEND_VM_SET_NEXT_OPCODE(opline + 2); \
		} else { \
			ZEND_VM_SET_OPCODE(OP_JMP_ADDR(opline + 1, (opline+1)->op2)); \
		} \
		ZEND_VM_CONTINUE(); \
	} while (0)
#define ZEND_VM_SMART_BRANCH_JMPNZ(_result, _check) do { \
		if ((_check) && UNEXPECTED(EG(exception))) { \
			OPLINE = EX(opline); \
		} else if (!(_result)) { \
			ZEND_VM_SET_NEXT_OPCODE(opline + 2); \
		} else { \
			ZEND_VM_SET_OPCODE(OP_JMP_ADDR(opline + 1, (opline+1)->op2)); \
		} \
		ZEND_VM_CONTINUE(); \
	} while (0)
#define ZEND_VM_SMART_BRANCH_NONE(_result, _check) do { \
		ZVAL_BOOL(EX_VAR(opline->result.var), _result); \
		ZEND_VM_NEXT_OPCODE_EX(_check, 1); \
		ZEND_VM_CONTINUE(); \
	} while (0)
#define ZEND_VM_SMART_BRANCH_TRUE() do { \
		if (EXPECTED(opline->result_type == (IS_SMART_BRANCH_JMPNZ|IS_TMP_VAR))) { \
			ZEND_VM_SET_OPCODE(OP_JMP_ADDR(opline + 1, (opline+1)->op2)); \
		} else if (EXPECTED(opline->result_type == (IS_SMART_BRANCH_JMPZ|IS_TMP_VAR))) { \
			ZEND_VM_SET_NEXT_OPCODE(opline + 2); \
		} else { \
			ZVAL_TRUE(EX_VAR(opline->result.var)); \
			ZEND_VM_SET_NEXT_OPCODE(opline + 1); \
		} \
		ZEND_VM_CONTINUE(); \
	} while (0)
#define ZEND_VM_SMART_BRANCH_TRUE_JMPZ() do { \
		ZEND_VM_SET_NEXT_OPCODE(opline + 2); \
		ZEND_VM_CONTINUE(); \
	} while (0)
#define ZEND_VM_SMART_BRANCH_TRUE_JMPNZ() do { \
		ZEND_VM_SET_OPCODE(OP_JMP_ADDR(opline + 1, (opline+1)->op2)); \
		ZEND_VM_CONTINUE(); \
	} while (0)
#define ZEND_VM_SMART_BRANCH_TRUE_NONE() do { \
		ZVAL_TRUE(EX_VAR(opline->result.var)); \
		ZEND_VM_NEXT_OPCODE(); \
	} while (0)
#define ZEND_VM_SMART_BRANCH_FALSE() do { \
		if (EXPECTED(opline->result_type == (IS_SMART_BRANCH_JMPNZ|IS_TMP_VAR))) { \
			ZEND_VM_SET_NEXT_OPCODE(opline + 2); \
		} else if (EXPECTED(opline->result_type == (IS_SMART_BRANCH_JMPZ|IS_TMP_VAR))) { \
			ZEND_VM_SET_OPCODE(OP_JMP_ADDR(opline + 1, (opline+1)->op2)); \
		} else { \
			ZVAL_FALSE(EX_VAR(opline->result.var)); \
			ZEND_VM_SET_NEXT_OPCODE(opline + 1); \
		} \
		ZEND_VM_CONTINUE(); \
	} while (0)
#define ZEND_VM_SMART_BRANCH_FALSE_JMPZ() do { \
		ZEND_VM_SET_OPCODE(OP_JMP_ADDR(opline + 1, (opline+1)->op2)); \
		ZEND_VM_CONTINUE(); \
	} while (0)
#define ZEND_VM_SMART_BRANCH_FALSE_JMPNZ() do { \
		ZEND_VM_SET_NEXT_OPCODE(opline + 2); \
		ZEND_VM_CONTINUE(); \
	} while (0)
#define ZEND_VM_SMART_BRANCH_FALSE_NONE() do { \
		ZVAL_FALSE(EX_VAR(opline->result.var)); \
		ZEND_VM_NEXT_OPCODE(); \
	} while (0)

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

/* This callback disables optimization of "vm_stack_data" variable in VM */
ZEND_API void (ZEND_FASTCALL *zend_touch_vm_stack_data)(void *vm_stack_data) = NULL;

#include "zend_vm_execute.h"

ZEND_API zend_result zend_set_user_opcode_handler(zend_uchar opcode, user_opcode_handler_t handler)
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

ZEND_API zval *zend_get_zval_ptr(const zend_op *opline, int op_type, const znode_op *node, const zend_execute_data *execute_data)
{
	zval *ret;

	switch (op_type) {
		case IS_CONST:
			ret = RT_CONSTANT(opline, *node);
			break;
		case IS_TMP_VAR:
		case IS_VAR:
		case IS_CV:
			ret = EX_VAR(node->var);
			break;
		default:
			ret = NULL;
			break;
	}
	return ret;
}
