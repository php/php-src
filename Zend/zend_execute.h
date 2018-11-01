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

#ifndef ZEND_EXECUTE_H
#define ZEND_EXECUTE_H

#include "zend_compile.h"
#include "zend_hash.h"
#include "zend_operators.h"
#include "zend_variables.h"

BEGIN_EXTERN_C()
struct _zend_fcall_info;
ZEND_API extern void (*zend_execute_ex)(zend_execute_data *execute_data);
ZEND_API extern void (*zend_execute_internal)(zend_execute_data *execute_data, zval *return_value);

void init_executor(void);
void shutdown_executor(void);
void shutdown_destructors(void);
ZEND_API void zend_init_execute_data(zend_execute_data *execute_data, zend_op_array *op_array, zval *return_value);
ZEND_API void zend_init_func_execute_data(zend_execute_data *execute_data, zend_op_array *op_array, zval *return_value);
ZEND_API void zend_init_code_execute_data(zend_execute_data *execute_data, zend_op_array *op_array, zval *return_value);
ZEND_API void zend_execute(zend_op_array *op_array, zval *return_value);
ZEND_API void execute_ex(zend_execute_data *execute_data);
ZEND_API void execute_internal(zend_execute_data *execute_data, zval *return_value);
ZEND_API zend_class_entry *zend_lookup_class(zend_string *name);
ZEND_API zend_class_entry *zend_lookup_class_ex(zend_string *name, const zval *key, int use_autoload);
ZEND_API zend_class_entry *zend_get_called_scope(zend_execute_data *ex);
ZEND_API zend_object *zend_get_this_object(zend_execute_data *ex);
ZEND_API int zend_eval_string(char *str, zval *retval_ptr, char *string_name);
ZEND_API int zend_eval_stringl(char *str, size_t str_len, zval *retval_ptr, char *string_name);
ZEND_API int zend_eval_string_ex(char *str, zval *retval_ptr, char *string_name, int handle_exceptions);
ZEND_API int zend_eval_stringl_ex(char *str, size_t str_len, zval *retval_ptr, char *string_name, int handle_exceptions);

/* export zend_pass_function to allow comparisons against it */
extern ZEND_API const zend_internal_function zend_pass_function;

ZEND_API void ZEND_FASTCALL zend_check_internal_arg_type(zend_function *zf, uint32_t arg_num, zval *arg);
ZEND_API int  ZEND_FASTCALL zend_check_arg_type(zend_function *zf, uint32_t arg_num, zval *arg, zval *default_value, void **cache_slot);
ZEND_API ZEND_COLD void ZEND_FASTCALL zend_missing_arg_error(zend_execute_data *execute_data);

static zend_always_inline zval* zend_assign_to_variable(zval *variable_ptr, zval *value, zend_uchar value_type)
{
	zend_refcounted *ref = NULL;

	if (ZEND_CONST_COND(value_type & (IS_VAR|IS_CV), 1) && Z_ISREF_P(value)) {
		ref = Z_COUNTED_P(value);
		value = Z_REFVAL_P(value);
	}

	do {
		if (UNEXPECTED(Z_REFCOUNTED_P(variable_ptr))) {
			zend_refcounted *garbage;

			if (Z_ISREF_P(variable_ptr)) {
				variable_ptr = Z_REFVAL_P(variable_ptr);
				if (EXPECTED(!Z_REFCOUNTED_P(variable_ptr))) {
					break;
				}
			}
			if (Z_TYPE_P(variable_ptr) == IS_OBJECT &&
	    		UNEXPECTED(Z_OBJ_HANDLER_P(variable_ptr, set) != NULL)) {
				Z_OBJ_HANDLER_P(variable_ptr, set)(variable_ptr, value);
				return variable_ptr;
			}
			if (ZEND_CONST_COND(value_type & (IS_VAR|IS_CV), 1) && variable_ptr == value) {
				if (value_type == IS_VAR && ref) {
					ZEND_ASSERT(GC_REFCOUNT(ref) > 1);
					GC_DELREF(ref);
				}
				return variable_ptr;
			}
			garbage = Z_COUNTED_P(variable_ptr);
			if (GC_DELREF(garbage) == 0) {
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
				rc_dtor_func(garbage);
				return variable_ptr;
			} else { /* we need to split */
				/* optimized version of GC_ZVAL_CHECK_POSSIBLE_ROOT(variable_ptr) */
				if (UNEXPECTED(GC_MAY_LEAK(garbage))) {
					gc_possible_root(garbage);
				}
			}
		}
	} while (0);

	ZVAL_COPY_VALUE(variable_ptr, value);
	if (ZEND_CONST_COND(value_type == IS_CONST, 0)) {
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
	return variable_ptr;
}

ZEND_API int zval_update_constant(zval *pp);
ZEND_API int zval_update_constant_ex(zval *pp, zend_class_entry *scope);
ZEND_API int zend_use_undefined_constant(zend_string *name, zend_ast_attr attr, zval *result);

/* dedicated Zend executor functions - do not use! */
struct _zend_vm_stack {
	zval *top;
	zval *end;
	zend_vm_stack prev;
};

#define ZEND_VM_STACK_HEADER_SLOTS \
	((ZEND_MM_ALIGNED_SIZE(sizeof(struct _zend_vm_stack)) + ZEND_MM_ALIGNED_SIZE(sizeof(zval)) - 1) / ZEND_MM_ALIGNED_SIZE(sizeof(zval)))

#define ZEND_VM_STACK_ELEMENTS(stack) \
	(((zval*)(stack)) + ZEND_VM_STACK_HEADER_SLOTS)

/*
 * In general in RELEASE build ZEND_ASSERT() must be zero-cost, but for some
 * reason, GCC generated worse code, performing CSE on assertion code and the
 * following "slow path" and moving memory read operatins from slow path into
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
ZEND_API void zend_vm_stack_destroy(void);
ZEND_API void* zend_vm_stack_extend(size_t size);

static zend_always_inline void zend_vm_init_call_frame(zend_execute_data *call, uint32_t call_info, zend_function *func, uint32_t num_args, zend_class_entry *called_scope, zend_object *object)
{
	call->func = func;
	if (object) {
		Z_OBJ(call->This) = object;
		ZEND_SET_CALL_INFO(call, 1, call_info);
	} else {
		Z_CE(call->This) = called_scope;
		ZEND_SET_CALL_INFO(call, 0, call_info);
	}
	ZEND_CALL_NUM_ARGS(call) = num_args;
}

static zend_always_inline zend_execute_data *zend_vm_stack_push_call_frame_ex(uint32_t used_stack, uint32_t call_info, zend_function *func, uint32_t num_args, zend_class_entry *called_scope, zend_object *object)
{
	zend_execute_data *call = (zend_execute_data*)EG(vm_stack_top);

	ZEND_ASSERT_VM_STACK_GLOBAL;

	if (UNEXPECTED(used_stack > (size_t)(((char*)EG(vm_stack_end)) - (char*)call))) {
		call = (zend_execute_data*)zend_vm_stack_extend(used_stack);
		ZEND_ASSERT_VM_STACK_GLOBAL;
		zend_vm_init_call_frame(call, call_info | ZEND_CALL_ALLOCATED, func, num_args, called_scope, object);
		return call;
	} else {
		EG(vm_stack_top) = (zval*)((char*)call + used_stack);
		zend_vm_init_call_frame(call, call_info, func, num_args, called_scope, object);
		return call;
	}
}

static zend_always_inline uint32_t zend_vm_calc_used_stack(uint32_t num_args, zend_function *func)
{
	uint32_t used_stack = ZEND_CALL_FRAME_SLOT + num_args;

	if (EXPECTED(ZEND_USER_CODE(func->type))) {
		used_stack += func->op_array.last_var + func->op_array.T - MIN(func->op_array.num_args, num_args);
	}
	return used_stack * sizeof(zval);
}

static zend_always_inline zend_execute_data *zend_vm_stack_push_call_frame(uint32_t call_info, zend_function *func, uint32_t num_args, zend_class_entry *called_scope, zend_object *object)
{
	uint32_t used_stack = zend_vm_calc_used_stack(num_args, func);

	return zend_vm_stack_push_call_frame_ex(used_stack, call_info,
		func, num_args, called_scope, object);
}

static zend_always_inline void zend_vm_stack_free_extra_args_ex(uint32_t call_info, zend_execute_data *call)
{
	if (UNEXPECTED(call_info & ZEND_CALL_FREE_EXTRA_ARGS)) {
		uint32_t count = ZEND_CALL_NUM_ARGS(call) - call->func->op_array.num_args;
		zval *p = ZEND_CALL_VAR_NUM(call, call->func->op_array.last_var + call->func->op_array.T);
		do {
			if (Z_REFCOUNTED_P(p)) {
				zend_refcounted *r = Z_COUNTED_P(p);
				if (!GC_DELREF(r)) {
					ZVAL_NULL(p);
					rc_dtor_func(r);
				} else {
					gc_check_possible_root(r);
				}
			}
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
			if (Z_REFCOUNTED_P(p)) {
				zend_refcounted *r = Z_COUNTED_P(p);
				if (!GC_DELREF(r)) {
					ZVAL_NULL(p);
					rc_dtor_func(r);
				}
			}
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

/* services */
ZEND_API const char *get_active_class_name(const char **space);
ZEND_API const char *get_active_function_name(void);
ZEND_API const char *zend_get_executed_filename(void);
ZEND_API zend_string *zend_get_executed_filename_ex(void);
ZEND_API uint32_t zend_get_executed_lineno(void);
ZEND_API zend_class_entry *zend_get_executed_scope(void);
ZEND_API zend_bool zend_is_executing(void);

ZEND_API void zend_set_timeout(zend_long seconds, int reset_signals);
ZEND_API void zend_unset_timeout(void);
ZEND_API ZEND_NORETURN void zend_timeout(int dummy);
ZEND_API zend_class_entry *zend_fetch_class(zend_string *class_name, int fetch_type);
ZEND_API zend_class_entry *zend_fetch_class_by_name(zend_string *class_name, const zval *key, int fetch_type);
void zend_verify_abstract_class(zend_class_entry *ce);

ZEND_API zend_function * ZEND_FASTCALL zend_fetch_function(zend_string *name);
ZEND_API zend_function * ZEND_FASTCALL zend_fetch_function_str(const char *name, size_t len);

ZEND_API void zend_fetch_dimension_const(zval *result, zval *container, zval *dim, int type);

ZEND_API zval* zend_get_compiled_variable_value(const zend_execute_data *execute_data_ptr, uint32_t var);

#define ZEND_USER_OPCODE_CONTINUE   0 /* execute next opcode */
#define ZEND_USER_OPCODE_RETURN     1 /* exit from executor (return from function) */
#define ZEND_USER_OPCODE_DISPATCH   2 /* call original opcode handler */
#define ZEND_USER_OPCODE_ENTER      3 /* enter into new op_array without recursion */
#define ZEND_USER_OPCODE_LEAVE      4 /* return to calling op_array within the same executor */

#define ZEND_USER_OPCODE_DISPATCH_TO 0x100 /* call original handler of returned opcode */

ZEND_API int zend_set_user_opcode_handler(zend_uchar opcode, user_opcode_handler_t handler);
ZEND_API user_opcode_handler_t zend_get_user_opcode_handler(zend_uchar opcode);

/* former zend_execute_locks.h */
typedef zval* zend_free_op;

ZEND_API zval *zend_get_zval_ptr(const zend_op *opline, int op_type, const znode_op *node, const zend_execute_data *execute_data, zend_free_op *should_free, int type);

ZEND_API void zend_clean_and_cache_symbol_table(zend_array *symbol_table);
ZEND_API void zend_free_compiled_variables(zend_execute_data *execute_data);
ZEND_API void zend_cleanup_unfinished_execution(zend_execute_data *execute_data, uint32_t op_num, uint32_t catch_op_num);

ZEND_API int ZEND_FASTCALL zend_do_fcall_overloaded(zend_execute_data *call, zval *ret);

#define CACHE_ADDR(num) \
	((void**)((char*)EX_RUN_TIME_CACHE() + (num)))

#define CACHED_PTR(num) \
	((void**)((char*)EX_RUN_TIME_CACHE() + (num)))[0]

#define CACHE_PTR(num, ptr) do { \
		((void**)((char*)EX_RUN_TIME_CACHE() + (num)))[0] = (ptr); \
	} while (0)

#define CACHED_POLYMORPHIC_PTR(num, ce) \
	(EXPECTED(((void**)((char*)EX_RUN_TIME_CACHE() + (num)))[0] == (void*)(ce)) ? \
		((void**)((char*)EX_RUN_TIME_CACHE() + (num)))[1] : \
		NULL)

#define CACHE_POLYMORPHIC_PTR(num, ce, ptr) do { \
		void **slot = (void**)((char*)EX_RUN_TIME_CACHE() + (num)); \
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

END_EXTERN_C()

#endif /* ZEND_EXECUTE_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
