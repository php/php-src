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

#ifndef ZEND_EXECUTE_H
#define ZEND_EXECUTE_H

#include "zend_compile.h"
#include "zend_hash.h"
#include "zend_operators.h"
#include "zend_variables.h"

#include <stdint.h>

BEGIN_EXTERN_C()
struct _zend_fcall_info;
ZEND_API extern void (*zend_execute_ex)(zend_execute_data *execute_data);
ZEND_API extern void (*zend_execute_internal)(zend_execute_data *execute_data, zval *return_value);

/* The lc_name may be stack allocated! */
ZEND_API extern zend_class_entry *(*zend_autoload)(zend_string *name, zend_string *lc_name);

void init_executor(void);
void shutdown_executor(void);
void shutdown_destructors(void);
ZEND_API void zend_shutdown_executor_values(bool fast_shutdown);

ZEND_API void zend_init_execute_data(zend_execute_data *execute_data, zend_op_array *op_array, zval *return_value);
ZEND_API void zend_init_func_execute_data(zend_execute_data *execute_data, zend_op_array *op_array, zval *return_value);
ZEND_API void zend_init_code_execute_data(zend_execute_data *execute_data, zend_op_array *op_array, zval *return_value);
ZEND_API void zend_execute(zend_op_array *op_array, zval *return_value);
ZEND_API void execute_ex(zend_execute_data *execute_data);
ZEND_API void execute_internal(zend_execute_data *execute_data, zval *return_value);
ZEND_API bool zend_is_valid_class_name(zend_string *name);
ZEND_API zend_class_entry *zend_lookup_class(zend_string *name);
ZEND_API zend_class_entry *zend_lookup_class_ex(zend_string *name, zend_string *lcname, uint32_t flags);
ZEND_API zend_class_entry *zend_get_called_scope(zend_execute_data *ex);
ZEND_API zend_object *zend_get_this_object(zend_execute_data *ex);
ZEND_API zend_result zend_eval_string(const char *str, zval *retval_ptr, const char *string_name);
ZEND_API zend_result zend_eval_stringl(const char *str, size_t str_len, zval *retval_ptr, const char *string_name);
ZEND_API zend_result zend_eval_string_ex(const char *str, zval *retval_ptr, const char *string_name, bool handle_exceptions);
ZEND_API zend_result zend_eval_stringl_ex(const char *str, size_t str_len, zval *retval_ptr, const char *string_name, bool handle_exceptions);

/* export zend_pass_function to allow comparisons against it */
extern ZEND_API const zend_internal_function zend_pass_function;

ZEND_API ZEND_COLD void ZEND_FASTCALL zend_missing_arg_error(zend_execute_data *execute_data);
ZEND_API ZEND_COLD void ZEND_FASTCALL zend_deprecated_function(const zend_function *fbc);
ZEND_API ZEND_COLD void ZEND_FASTCALL zend_false_to_array_deprecated(void);
ZEND_COLD void ZEND_FASTCALL zend_param_must_be_ref(const zend_function *func, uint32_t arg_num);
ZEND_API ZEND_COLD void ZEND_FASTCALL zend_use_resource_as_offset(const zval *dim);

ZEND_API bool ZEND_FASTCALL zend_verify_ref_assignable_zval(zend_reference *ref, zval *zv, bool strict);

typedef enum {
	ZEND_VERIFY_PROP_ASSIGNABLE_BY_REF_CONTEXT_ASSIGNMENT,
	ZEND_VERIFY_PROP_ASSIGNABLE_BY_REF_CONTEXT_MAGIC_GET,
} zend_verify_prop_assignable_by_ref_context;
ZEND_API bool ZEND_FASTCALL zend_verify_prop_assignable_by_ref_ex(const zend_property_info *prop_info, zval *orig_val, bool strict, zend_verify_prop_assignable_by_ref_context context);
ZEND_API bool ZEND_FASTCALL zend_verify_prop_assignable_by_ref(const zend_property_info *prop_info, zval *orig_val, bool strict);

ZEND_API ZEND_COLD void zend_throw_ref_type_error_zval(const zend_property_info *prop, const zval *zv);
ZEND_API ZEND_COLD void zend_throw_ref_type_error_type(const zend_property_info *prop1, const zend_property_info *prop2, const zval *zv);
ZEND_API ZEND_COLD zval* ZEND_FASTCALL zend_undefined_offset_write(HashTable *ht, zend_long lval);
ZEND_API ZEND_COLD zval* ZEND_FASTCALL zend_undefined_index_write(HashTable *ht, zend_string *offset);
ZEND_API ZEND_COLD void zend_wrong_string_offset_error(void);

ZEND_API ZEND_COLD void ZEND_FASTCALL zend_readonly_property_modification_error(const zend_property_info *info);
ZEND_API ZEND_COLD void ZEND_FASTCALL zend_readonly_property_indirect_modification_error(const zend_property_info *info);

ZEND_API ZEND_COLD void ZEND_FASTCALL zend_invalid_class_constant_type_error(uint8_t type);

ZEND_API ZEND_COLD void ZEND_FASTCALL zend_object_released_while_assigning_to_property_error(const zend_property_info *info);

ZEND_API ZEND_COLD void ZEND_FASTCALL zend_cannot_add_element(void);

ZEND_API bool zend_verify_scalar_type_hint(uint32_t type_mask, zval *arg, bool strict, bool is_internal_arg);
ZEND_API ZEND_COLD void zend_verify_arg_error(
		const zend_function *zf, const zend_arg_info *arg_info, uint32_t arg_num, zval *value);
ZEND_API ZEND_COLD void zend_verify_return_error(
		const zend_function *zf, zval *value);
ZEND_API ZEND_COLD void zend_verify_never_error(
		const zend_function *zf);
ZEND_API bool zend_verify_ref_array_assignable(zend_reference *ref);
ZEND_API bool zend_check_user_type_slow(
		zend_type *type, zval *arg, zend_reference *ref, void **cache_slot, bool is_return_type);

#if ZEND_DEBUG
ZEND_API bool zend_internal_call_should_throw(zend_function *fbc, zend_execute_data *call);
ZEND_API ZEND_COLD void zend_internal_call_arginfo_violation(zend_function *fbc);
ZEND_API bool zend_verify_internal_return_type(zend_function *zf, zval *ret);
#endif

#define ZEND_REF_TYPE_SOURCES(ref) \
	(ref)->sources

#define ZEND_REF_HAS_TYPE_SOURCES(ref) \
	(ZEND_REF_TYPE_SOURCES(ref).ptr != NULL)

#define ZEND_REF_FIRST_SOURCE(ref) \
	(ZEND_PROPERTY_INFO_SOURCE_IS_LIST((ref)->sources.list) \
		? ZEND_PROPERTY_INFO_SOURCE_TO_LIST((ref)->sources.list)->ptr[0] \
		: (ref)->sources.ptr)


ZEND_API void ZEND_FASTCALL zend_ref_add_type_source(zend_property_info_source_list *source_list, zend_property_info *prop);
ZEND_API void ZEND_FASTCALL zend_ref_del_type_source(zend_property_info_source_list *source_list, const zend_property_info *prop);

ZEND_API zval* zend_assign_to_typed_ref(zval *variable_ptr, zval *value, uint8_t value_type, bool strict);
ZEND_API zval* zend_assign_to_typed_ref_ex(zval *variable_ptr, zval *value, uint8_t value_type, bool strict, zend_refcounted **garbage_ptr);

static zend_always_inline void zend_copy_to_variable(zval *variable_ptr, zval *value, uint8_t value_type)
{
	zend_refcounted *ref = NULL;

	if (ZEND_CONST_COND(value_type & (IS_VAR|IS_CV), 1) && Z_ISREF_P(value)) {
		ref = Z_COUNTED_P(value);
		value = Z_REFVAL_P(value);
	}

	ZVAL_COPY_VALUE(variable_ptr, value);
	if (ZEND_CONST_COND(value_type  == IS_CONST, 0)) {
		if (UNEXPECTED(Z_OPT_REFCOUNTED_P(variable_ptr))) {
			Z_ADDREF_P(variable_ptr);
		}
	} else if (value_type & (IS_CONST|IS_CV)) {
		if (Z_OPT_REFCOUNTED_P(variable_ptr)) {
			Z_ADDREF_P(variable_ptr);
		}
	} else if (ZEND_CONST_COND(value_type == IS_VAR, 1) && UNEXPECTED(ref)) {
		if (UNEXPECTED(GC_DELREF(ref) == 0)) {
			efree_size(ref, sizeof(zend_reference));
		} else if (Z_OPT_REFCOUNTED_P(variable_ptr)) {
			Z_ADDREF_P(variable_ptr);
		}
	}
}

static zend_always_inline zval* zend_assign_to_variable(zval *variable_ptr, zval *value, uint8_t value_type, bool strict)
{
	do {
		if (UNEXPECTED(Z_REFCOUNTED_P(variable_ptr))) {
			zend_refcounted *garbage;

			if (Z_ISREF_P(variable_ptr)) {
				if (UNEXPECTED(ZEND_REF_HAS_TYPE_SOURCES(Z_REF_P(variable_ptr)))) {
					return zend_assign_to_typed_ref(variable_ptr, value, value_type, strict);
				}

				variable_ptr = Z_REFVAL_P(variable_ptr);
				if (EXPECTED(!Z_REFCOUNTED_P(variable_ptr))) {
					break;
				}
			}
			garbage = Z_COUNTED_P(variable_ptr);
			zend_copy_to_variable(variable_ptr, value, value_type);
			GC_DTOR_NO_REF(garbage);
			return variable_ptr;
		}
	} while (0);

	zend_copy_to_variable(variable_ptr, value, value_type);
	return variable_ptr;
}

static zend_always_inline zval* zend_assign_to_variable_ex(zval *variable_ptr, zval *value, zend_uchar value_type, bool strict, zend_refcounted **garbage_ptr)
{
	do {
		if (UNEXPECTED(Z_REFCOUNTED_P(variable_ptr))) {
			if (Z_ISREF_P(variable_ptr)) {
				if (UNEXPECTED(ZEND_REF_HAS_TYPE_SOURCES(Z_REF_P(variable_ptr)))) {
					return zend_assign_to_typed_ref_ex(variable_ptr, value, value_type, strict, garbage_ptr);
				}

				variable_ptr = Z_REFVAL_P(variable_ptr);
				if (EXPECTED(!Z_REFCOUNTED_P(variable_ptr))) {
					break;
				}
			}
			*garbage_ptr = Z_COUNTED_P(variable_ptr);
		}
	} while (0);

	zend_copy_to_variable(variable_ptr, value, value_type);
	return variable_ptr;
}

ZEND_API zend_result ZEND_FASTCALL zval_update_constant(zval *pp);
ZEND_API zend_result ZEND_FASTCALL zval_update_constant_ex(zval *pp, zend_class_entry *scope);
ZEND_API zend_result ZEND_FASTCALL zval_update_constant_with_ctx(zval *pp, zend_class_entry *scope, zend_ast_evaluate_ctx *ctx);

/* dedicated Zend executor functions - do not use! */
struct _zend_vm_stack {
	zval *top;
	zval *end;
	zend_vm_stack prev;
};

/* Ensure the correct alignment before slots calculation */
ZEND_STATIC_ASSERT(ZEND_MM_ALIGNED_SIZE(sizeof(zval)) == sizeof(zval),
                   "zval must be aligned by ZEND_MM_ALIGNMENT");
/* A number of call frame slots (zvals) reserved for _zend_vm_stack. */
#define ZEND_VM_STACK_HEADER_SLOTS \
	((sizeof(struct _zend_vm_stack) + sizeof(zval) - 1) / sizeof(zval))

#define ZEND_VM_STACK_ELEMENTS(stack) \
	(((zval*)(stack)) + ZEND_VM_STACK_HEADER_SLOTS)

/*
 * In general in RELEASE build ZEND_ASSERT() must be zero-cost, but for some
 * reason, GCC generated worse code, performing CSE on assertion code and the
 * following "slow path" and moving memory read operations from slow path into
 * common header. This made a degradation for the fast path.
 * The following "#if ZEND_DEBUG" eliminates it.
 */
#if ZEND_DEBUG
# define ZEND_ASSERT_VM_STACK(stack) ZEND_ASSERT(stack->top > (zval *) stack && stack->end > (zval *) stack && stack->top <= stack->end)
# define ZEND_ASSERT_VM_STACK_GLOBAL ZEND_ASSERT(EG(vm_stack_top) > (zval *) EG(vm_stack) && EG(vm_stack_end) > (zval *) EG(vm_stack) && EG(vm_stack_top) <= EG(vm_stack_end))
#else
# define ZEND_ASSERT_VM_STACK(stack)
# define ZEND_ASSERT_VM_STACK_GLOBAL
#endif

ZEND_API void zend_vm_stack_init(void);
ZEND_API void zend_vm_stack_init_ex(size_t page_size);
ZEND_API void zend_vm_stack_destroy(void);
ZEND_API void* zend_vm_stack_extend(size_t size);

static zend_always_inline zend_vm_stack zend_vm_stack_new_page(size_t size, zend_vm_stack prev) {
	zend_vm_stack page = (zend_vm_stack)emalloc(size);

	page->top = ZEND_VM_STACK_ELEMENTS(page);
	page->end = (zval*)((char*)page + size);
	page->prev = prev;
	return page;
}

static zend_always_inline void zend_vm_init_call_frame(zend_execute_data *call, uint32_t call_info, zend_function *func, uint32_t num_args, void *object_or_called_scope)
{
	ZEND_ASSERT(!func->common.scope || object_or_called_scope);
	call->func = func;
	Z_PTR(call->This) = object_or_called_scope;
	ZEND_CALL_INFO(call) = call_info;
	ZEND_CALL_NUM_ARGS(call) = num_args;
}

static zend_always_inline zend_execute_data *zend_vm_stack_push_call_frame_ex(uint32_t used_stack, uint32_t call_info, zend_function *func, uint32_t num_args, void *object_or_called_scope)
{
	zend_execute_data *call = (zend_execute_data*)EG(vm_stack_top);

	ZEND_ASSERT_VM_STACK_GLOBAL;

	if (UNEXPECTED(used_stack > (size_t)(((char*)EG(vm_stack_end)) - (char*)call))) {
		call = (zend_execute_data*)zend_vm_stack_extend(used_stack);
		ZEND_ASSERT_VM_STACK_GLOBAL;
		zend_vm_init_call_frame(call, call_info | ZEND_CALL_ALLOCATED, func, num_args, object_or_called_scope);
		return call;
	} else {
		EG(vm_stack_top) = (zval*)((char*)call + used_stack);
		zend_vm_init_call_frame(call, call_info, func, num_args, object_or_called_scope);
		return call;
	}
}

static zend_always_inline uint32_t zend_vm_calc_used_stack(uint32_t num_args, zend_function *func)
{
	uint32_t used_stack = ZEND_CALL_FRAME_SLOT + num_args + func->common.T;

	if (EXPECTED(ZEND_USER_CODE(func->type))) {
		used_stack += func->op_array.last_var - MIN(func->op_array.num_args, num_args);
	}
	return used_stack * sizeof(zval);
}

static zend_always_inline zend_execute_data *zend_vm_stack_push_call_frame(uint32_t call_info, zend_function *func, uint32_t num_args, void *object_or_called_scope)
{
	uint32_t used_stack = zend_vm_calc_used_stack(num_args, func);

	return zend_vm_stack_push_call_frame_ex(used_stack, call_info,
		func, num_args, object_or_called_scope);
}

static zend_always_inline void zend_vm_stack_free_extra_args_ex(uint32_t call_info, zend_execute_data *call)
{
	if (UNEXPECTED(call_info & ZEND_CALL_FREE_EXTRA_ARGS)) {
		uint32_t count = ZEND_CALL_NUM_ARGS(call) - call->func->op_array.num_args;
		zval *p = ZEND_CALL_VAR_NUM(call, call->func->op_array.last_var + call->func->op_array.T);
		do {
			i_zval_ptr_dtor(p);
			p++;
		} while (--count);
 	}
}

static zend_always_inline void zend_vm_stack_free_extra_args(zend_execute_data *call)
{
	zend_vm_stack_free_extra_args_ex(ZEND_CALL_INFO(call), call);
}

static zend_always_inline void zend_vm_stack_free_args(zend_execute_data *call)
{
	uint32_t num_args = ZEND_CALL_NUM_ARGS(call);

	if (EXPECTED(num_args > 0)) {
		zval *p = ZEND_CALL_ARG(call, 1);

		do {
			zval_ptr_dtor_nogc(p);
			p++;
		} while (--num_args);
	}
}

static zend_always_inline void zend_vm_stack_free_call_frame_ex(uint32_t call_info, zend_execute_data *call)
{
	ZEND_ASSERT_VM_STACK_GLOBAL;

	if (UNEXPECTED(call_info & ZEND_CALL_ALLOCATED)) {
		zend_vm_stack p = EG(vm_stack);
		zend_vm_stack prev = p->prev;

		ZEND_ASSERT(call == (zend_execute_data*)ZEND_VM_STACK_ELEMENTS(EG(vm_stack)));
		EG(vm_stack_top) = prev->top;
		EG(vm_stack_end) = prev->end;
		EG(vm_stack) = prev;
		efree(p);
	} else {
		EG(vm_stack_top) = (zval*)call;
	}

	ZEND_ASSERT_VM_STACK_GLOBAL;
}

static zend_always_inline void zend_vm_stack_free_call_frame(zend_execute_data *call)
{
	zend_vm_stack_free_call_frame_ex(ZEND_CALL_INFO(call), call);
}

zend_execute_data *zend_vm_stack_copy_call_frame(
	zend_execute_data *call, uint32_t passed_args, uint32_t additional_args);

static zend_always_inline void zend_vm_stack_extend_call_frame(
	zend_execute_data **call, uint32_t passed_args, uint32_t additional_args)
{
	if (EXPECTED((uint32_t)(EG(vm_stack_end) - EG(vm_stack_top)) > additional_args)) {
		EG(vm_stack_top) += additional_args;
	} else {
		*call = zend_vm_stack_copy_call_frame(*call, passed_args, additional_args);
	}
}

ZEND_API void ZEND_FASTCALL zend_free_extra_named_params(zend_array *extra_named_params);

/* services */
ZEND_API const char *get_active_class_name(const char **space);
ZEND_API const char *get_active_function_name(void);
ZEND_API const char *get_active_function_arg_name(uint32_t arg_num);
ZEND_API const char *get_function_arg_name(const zend_function *func, uint32_t arg_num);
ZEND_API zend_function *zend_active_function_ex(zend_execute_data *execute_data);

static zend_always_inline zend_function *zend_active_function(void)
{
	zend_function *func = EG(current_execute_data)->func;
	if (ZEND_USER_CODE(func->type)) {
		return zend_active_function_ex(EG(current_execute_data));
	} else {
		return func;
	}
}

ZEND_API zend_string *get_active_function_or_method_name(void);
ZEND_API zend_string *get_function_or_method_name(const zend_function *func);
ZEND_API const char *zend_get_executed_filename(void);
ZEND_API zend_string *zend_get_executed_filename_ex(void);
ZEND_API uint32_t zend_get_executed_lineno(void);
ZEND_API zend_class_entry *zend_get_executed_scope(void);
ZEND_API bool zend_is_executing(void);
ZEND_API ZEND_COLD void ZEND_FASTCALL zend_cannot_pass_by_reference(uint32_t arg_num);

ZEND_API void zend_set_timeout(zend_long seconds, bool reset_signals);
ZEND_API void zend_unset_timeout(void);
ZEND_API ZEND_NORETURN void ZEND_FASTCALL zend_timeout(void);
ZEND_API zend_class_entry *zend_fetch_class(zend_string *class_name, uint32_t fetch_type);
ZEND_API zend_class_entry *zend_fetch_class_with_scope(zend_string *class_name, uint32_t fetch_type, zend_class_entry *scope);
ZEND_API zend_class_entry *zend_fetch_class_by_name(zend_string *class_name, zend_string *lcname, uint32_t fetch_type);

#if defined(__APPLE__) && !defined(ZTS)
void zend_timeout_handler(void);
#endif

ZEND_API zend_function * ZEND_FASTCALL zend_fetch_function(zend_string *name);
ZEND_API zend_function * ZEND_FASTCALL zend_fetch_function_str(const char *name, size_t len);
ZEND_API void ZEND_FASTCALL zend_init_func_run_time_cache(zend_op_array *op_array);

ZEND_API void zend_fetch_dimension_const(zval *result, zval *container, zval *dim, int type);

ZEND_API zval* zend_get_compiled_variable_value(const zend_execute_data *execute_data_ptr, uint32_t var);

ZEND_API bool zend_gcc_global_regs(void);

#define ZEND_USER_OPCODE_CONTINUE   0 /* execute next opcode */
#define ZEND_USER_OPCODE_RETURN     1 /* exit from executor (return from function) */
#define ZEND_USER_OPCODE_DISPATCH   2 /* call original opcode handler */
#define ZEND_USER_OPCODE_ENTER      3 /* enter into new op_array without recursion */
#define ZEND_USER_OPCODE_LEAVE      4 /* return to calling op_array within the same executor */

#define ZEND_USER_OPCODE_DISPATCH_TO 0x100 /* call original handler of returned opcode */

ZEND_API zend_result zend_set_user_opcode_handler(uint8_t opcode, user_opcode_handler_t handler);
ZEND_API user_opcode_handler_t zend_get_user_opcode_handler(uint8_t opcode);

ZEND_API zval *zend_get_zval_ptr(const zend_op *opline, int op_type, const znode_op *node, const zend_execute_data *execute_data);

ZEND_API void zend_clean_and_cache_symbol_table(zend_array *symbol_table);
ZEND_API void ZEND_FASTCALL zend_free_compiled_variables(zend_execute_data *execute_data);
ZEND_API void zend_unfinished_calls_gc(zend_execute_data *execute_data, zend_execute_data *call, uint32_t op_num, zend_get_gc_buffer *buf);
ZEND_API void zend_cleanup_unfinished_execution(zend_execute_data *execute_data, uint32_t op_num, uint32_t catch_op_num);
ZEND_API ZEND_ATTRIBUTE_DEPRECATED HashTable *zend_unfinished_execution_gc(zend_execute_data *execute_data, zend_execute_data *call, zend_get_gc_buffer *gc_buffer);
ZEND_API HashTable *zend_unfinished_execution_gc_ex(zend_execute_data *execute_data, zend_execute_data *call, zend_get_gc_buffer *gc_buffer, bool suspended_by_yield);

zval * ZEND_FASTCALL zend_handle_named_arg(
		zend_execute_data **call_ptr, zend_string *arg_name,
		uint32_t *arg_num_ptr, void **cache_slot);
ZEND_API zend_result ZEND_FASTCALL zend_handle_undef_args(zend_execute_data *call);

#define CACHE_ADDR(num) \
	((void**)((char*)EX(run_time_cache) + (num)))

#define CACHED_PTR(num) \
	((void**)((char*)EX(run_time_cache) + (num)))[0]

#define CACHE_PTR(num, ptr) do { \
		((void**)((char*)EX(run_time_cache) + (num)))[0] = (ptr); \
	} while (0)

#define CACHED_POLYMORPHIC_PTR(num, ce) \
	(EXPECTED(((void**)((char*)EX(run_time_cache) + (num)))[0] == (void*)(ce)) ? \
		((void**)((char*)EX(run_time_cache) + (num)))[1] : \
		NULL)

#define CACHE_POLYMORPHIC_PTR(num, ce, ptr) do { \
		void **slot = (void**)((char*)EX(run_time_cache) + (num)); \
		slot[0] = (ce); \
		slot[1] = (ptr); \
	} while (0)

#define CACHED_PTR_EX(slot) \
	(slot)[0]

#define CACHE_PTR_EX(slot, ptr) do { \
		(slot)[0] = (ptr); \
	} while (0)

#define CACHED_POLYMORPHIC_PTR_EX(slot, ce) \
	(EXPECTED((slot)[0] == (ce)) ? (slot)[1] : NULL)

#define CACHE_POLYMORPHIC_PTR_EX(slot, ce, ptr) do { \
		(slot)[0] = (ce); \
		(slot)[1] = (ptr); \
	} while (0)

#define CACHE_SPECIAL (1<<0)

#define IS_SPECIAL_CACHE_VAL(ptr) \
	(((uintptr_t)(ptr)) & CACHE_SPECIAL)

#define ENCODE_SPECIAL_CACHE_NUM(num) \
	((void*)((((uintptr_t)(num)) << 1) | CACHE_SPECIAL))

#define DECODE_SPECIAL_CACHE_NUM(ptr) \
	(((uintptr_t)(ptr)) >> 1)

#define ENCODE_SPECIAL_CACHE_PTR(ptr) \
	((void*)(((uintptr_t)(ptr)) | CACHE_SPECIAL))

#define DECODE_SPECIAL_CACHE_PTR(ptr) \
	((void*)(((uintptr_t)(ptr)) & ~CACHE_SPECIAL))

#define SKIP_EXT_OPLINE(opline) do { \
		while (UNEXPECTED((opline)->opcode >= ZEND_EXT_STMT \
			&& (opline)->opcode <= ZEND_TICKS)) {     \
			(opline)--;                                  \
		}                                                \
	} while (0)

#define ZEND_CLASS_HAS_TYPE_HINTS(ce) ((ce->ce_flags & ZEND_ACC_HAS_TYPE_HINTS) == ZEND_ACC_HAS_TYPE_HINTS)
#define ZEND_CLASS_HAS_READONLY_PROPS(ce) ((ce->ce_flags & ZEND_ACC_HAS_READONLY_PROPS) == ZEND_ACC_HAS_READONLY_PROPS)


ZEND_API bool zend_verify_class_constant_type(zend_class_constant *c, const zend_string *name, zval *constant);
ZEND_COLD void zend_verify_class_constant_type_error(const zend_class_constant *c, const zend_string *name, const zval *constant);

ZEND_API bool zend_verify_property_type(const zend_property_info *info, zval *property, bool strict);
ZEND_COLD void zend_verify_property_type_error(const zend_property_info *info, const zval *property);
ZEND_COLD void zend_magic_get_property_type_inconsistency_error(const zend_property_info *info, const zval *property);

#define ZEND_REF_ADD_TYPE_SOURCE(ref, source) \
	zend_ref_add_type_source(&ZEND_REF_TYPE_SOURCES(ref), source)

#define ZEND_REF_DEL_TYPE_SOURCE(ref, source) \
	zend_ref_del_type_source(&ZEND_REF_TYPE_SOURCES(ref), source)

#define ZEND_REF_FOREACH_TYPE_SOURCES(ref, prop) do { \
		zend_property_info_source_list *_source_list = &ZEND_REF_TYPE_SOURCES(ref); \
		zend_property_info **_prop, **_end; \
		zend_property_info_list *_list; \
		if (_source_list->ptr) { \
			if (ZEND_PROPERTY_INFO_SOURCE_IS_LIST(_source_list->list)) { \
				_list = ZEND_PROPERTY_INFO_SOURCE_TO_LIST(_source_list->list); \
				_prop = _list->ptr; \
				_end = _list->ptr + _list->num; \
			} else { \
				_prop = &_source_list->ptr; \
				_end = _prop + 1; \
			} \
			for (; _prop < _end; _prop++) { \
				prop = *_prop; \

#define ZEND_REF_FOREACH_TYPE_SOURCES_END() \
			} \
		} \
	} while (0)

ZEND_COLD void zend_match_unhandled_error(const zval *value);

static zend_always_inline void *zend_get_bad_ptr(void)
{
	ZEND_UNREACHABLE();
	return NULL;
}

END_EXTERN_C()

#endif /* ZEND_EXECUTE_H */
