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

#ifndef ZEND_EXECUTE_H
#define ZEND_EXECUTE_H

#include "zend_compile.h"
#include "zend_hash.h"
#include "zend_operators.h"
#include "zend_variables.h"

BEGIN_EXTERN_C()
struct _zend_fcall_info;
ZEND_API extern void (*zend_execute_ex)(zend_execute_data *execute_data TSRMLS_DC);
ZEND_API extern void (*zend_execute_internal)(zend_execute_data *execute_data_ptr, struct _zend_fcall_info *fci TSRMLS_DC);

void init_executor(TSRMLS_D);
void shutdown_executor(TSRMLS_D);
void shutdown_destructors(TSRMLS_D);
ZEND_API zend_execute_data *zend_create_execute_data_from_op_array(zend_op_array *op_array, zval *return_value, vm_frame_kind frame_kind TSRMLS_DC);
ZEND_API void zend_execute(zend_op_array *op_array, zval *return_value TSRMLS_DC);
ZEND_API void execute_ex(zend_execute_data *execute_data TSRMLS_DC);
ZEND_API void execute_internal(zend_execute_data *execute_data_ptr, struct _zend_fcall_info *fci TSRMLS_DC);
ZEND_API int zend_is_true(zval *op TSRMLS_DC);
ZEND_API zend_class_entry *zend_lookup_class(zend_string *name TSRMLS_DC);
ZEND_API zend_class_entry *zend_lookup_class_ex(zend_string *name, const zval *key, int use_autoload TSRMLS_DC);
ZEND_API int zend_eval_string(char *str, zval *retval_ptr, char *string_name TSRMLS_DC);
ZEND_API int zend_eval_stringl(char *str, int str_len, zval *retval_ptr, char *string_name TSRMLS_DC);
ZEND_API int zend_eval_string_ex(char *str, zval *retval_ptr, char *string_name, int handle_exceptions TSRMLS_DC);
ZEND_API int zend_eval_stringl_ex(char *str, int str_len, zval *retval_ptr, char *string_name, int handle_exceptions TSRMLS_DC);

ZEND_API char * zend_verify_arg_class_kind(const zend_arg_info *cur_arg_info, ulong fetch_type, char **class_name, zend_class_entry **pce TSRMLS_DC);
ZEND_API void zend_verify_arg_error(int error_type, const zend_function *zf, zend_uint arg_num, const char *need_msg, const char *need_kind, const char *given_msg, const char *given_kind TSRMLS_DC);

static zend_always_inline void i_zval_ptr_dtor(zval *zval_ptr ZEND_FILE_LINE_DC TSRMLS_DC)
{
	if (Z_REFCOUNTED_P(zval_ptr)) {
		if (!Z_DELREF_P(zval_ptr)) {
			ZEND_ASSERT(zval_ptr != &EG(uninitialized_zval));
			_zval_dtor_func_for_ptr(Z_COUNTED_P(zval_ptr) ZEND_FILE_LINE_CC);
		} else {
			GC_ZVAL_CHECK_POSSIBLE_ROOT(zval_ptr);
		}
	}
}

static zend_always_inline void i_zval_ptr_dtor_nogc(zval *zval_ptr ZEND_FILE_LINE_DC TSRMLS_DC)
{
	if (Z_REFCOUNTED_P(zval_ptr)) {
		if (!Z_DELREF_P(zval_ptr)) {
			ZEND_ASSERT(zval_ptr != &EG(uninitialized_zval));
			_zval_dtor_func_for_ptr(Z_COUNTED_P(zval_ptr) ZEND_FILE_LINE_CC);
		}
	}
}

static zend_always_inline int i_zend_is_true(zval *op TSRMLS_DC)
{
	int result;

again:
	switch (Z_TYPE_P(op)) {
		case IS_UNDEF:
		case IS_NULL:
		case IS_FALSE:
			result = 0;
			break;
		case IS_TRUE:
			result = 1;
			break;
		case IS_LONG:
			result = (Z_LVAL_P(op)?1:0);
			break;
		case IS_RESOURCE:
			result = (Z_RES_HANDLE_P(op)?1:0);
			break;
		case IS_DOUBLE:
			result = (Z_DVAL_P(op) ? 1 : 0);
			break;
		case IS_STRING:
			if (Z_STRLEN_P(op) == 0
				|| (Z_STRLEN_P(op)==1 && Z_STRVAL_P(op)[0]=='0')) {
				result = 0;
			} else {
				result = 1;
			}
			break;
		case IS_ARRAY:
			result = (zend_hash_num_elements(Z_ARRVAL_P(op))?1:0);
			break;
		case IS_OBJECT:
			if (IS_ZEND_STD_OBJECT(*op)) {
				if (Z_OBJ_HT_P(op)->cast_object) {
					zval tmp;
					if (Z_OBJ_HT_P(op)->cast_object(op, &tmp, _IS_BOOL TSRMLS_CC) == SUCCESS) {
						result = Z_TYPE(tmp) == IS_TRUE;
						break;
					}

					// TODO: The following line breakes phar ???
					// zend_error(E_RECOVERABLE_ERROR, "Object of class %s could not be converted to boolean", Z_OBJ_P(op)->ce->name->val);
				} else if (Z_OBJ_HT_P(op)->get) {
					zval rv;
					zval *tmp = Z_OBJ_HT_P(op)->get(op, &rv TSRMLS_CC);
					if (Z_TYPE_P(tmp) != IS_OBJECT) {
						/* for safety - avoid loop */
						convert_to_boolean(tmp);
						result = Z_TYPE_P(tmp) == IS_TRUE;
						zval_ptr_dtor(tmp);
						break;
					}
				}
			}
			result = 1;
			break;
		case IS_REFERENCE:
			op = Z_REFVAL_P(op);
			goto again;
			break;
		default:
			result = 0;
			break;
	}
	return result;
}

ZEND_API int zval_update_constant(zval *pp, zend_bool inline_change TSRMLS_DC);
ZEND_API int zval_update_constant_inline_change(zval *pp, zend_class_entry *scope TSRMLS_DC);
ZEND_API int zval_update_constant_no_inline_change(zval *pp, zend_class_entry *scope TSRMLS_DC);
ZEND_API int zval_update_constant_ex(zval *pp, zend_bool inline_change, zend_class_entry *scope TSRMLS_DC);

/* dedicated Zend executor functions - do not use! */
#define ZEND_VM_STACK_PAGE_SIZE ((16 * 1024) - 16)

struct _zend_vm_stack {
	zval *top;
	zval *end;
	zend_vm_stack prev;
};

#define ZEND_VM_STACK_ELEMETS(stack) \
	((zval*)(((char*)(stack)) + ZEND_MM_ALIGNED_SIZE(sizeof(struct _zend_vm_stack))))

#define ZEND_VM_STACK_GROW_IF_NEEDED(count)							\
	do {															\
		if (UNEXPECTED((count) >									\
		    EG(argument_stack)->end - EG(argument_stack)->top)) {	\
			zend_vm_stack_extend((count) TSRMLS_CC);				\
		}															\
	} while (0)

static zend_always_inline zend_vm_stack zend_vm_stack_new_page(int count) {
	zend_vm_stack page = (zend_vm_stack)emalloc(ZEND_MM_ALIGNED_SIZE(sizeof(*page)) + sizeof(zval) * count);

	page->top = ZEND_VM_STACK_ELEMETS(page);
	page->end = page->top + count;
	page->prev = NULL;
	return page;
}

static zend_always_inline void zend_vm_stack_init(TSRMLS_D)
{
	EG(argument_stack) = zend_vm_stack_new_page(ZEND_VM_STACK_PAGE_SIZE);
}

static zend_always_inline void zend_vm_stack_destroy(TSRMLS_D)
{
	zend_vm_stack stack = EG(argument_stack);

	while (stack != NULL) {
		zend_vm_stack p = stack->prev;
		efree(stack);
		stack = p;
	}
}

static zend_always_inline void zend_vm_stack_extend(int count TSRMLS_DC)
{
	zend_vm_stack p = zend_vm_stack_new_page(count >= ZEND_VM_STACK_PAGE_SIZE ? count : ZEND_VM_STACK_PAGE_SIZE);
	p->prev = EG(argument_stack);
	EG(argument_stack) = p;
}

static zend_always_inline zval *zend_vm_stack_top(TSRMLS_D)
{
	return EG(argument_stack)->top;
}

static zend_always_inline zval *zend_vm_stack_top_inc(TSRMLS_D)
{
	return EG(argument_stack)->top++;
}

static zend_always_inline void zend_vm_stack_push(zval *ptr TSRMLS_DC)
{
	ZVAL_COPY_VALUE(EG(argument_stack)->top, ptr);
	EG(argument_stack)->top++;
}

static zend_always_inline zval *zend_vm_stack_pop(TSRMLS_D)
{
	return --EG(argument_stack)->top;
}

static zend_always_inline void *zend_vm_stack_alloc(size_t size TSRMLS_DC)
{
	zval *ret;
	int count = (size + (sizeof(zval) - 1)) / sizeof(zval);

	ZEND_VM_STACK_GROW_IF_NEEDED(count);
	ret = (void*)EG(argument_stack)->top;
	EG(argument_stack)->top += count;
	return ret;
}

static zend_always_inline zval* zend_vm_stack_frame_base(zend_execute_data *ex)
{
	return (zval*)((char*)ex->call_slots +
		ZEND_MM_ALIGNED_SIZE(sizeof(call_slot)) * ex->op_array->nested_calls);
}

static zend_always_inline void zend_vm_stack_free(void *ptr TSRMLS_DC)
{
	if (UNEXPECTED((void*)ZEND_VM_STACK_ELEMETS(EG(argument_stack)) == ptr)) {
		zend_vm_stack p = EG(argument_stack);

		EG(argument_stack) = p->prev;
		efree(p);
	} else {
		EG(argument_stack)->top = (zval*)ptr;
	}
}

static zend_always_inline void zend_vm_stack_clear_multiple(int nested TSRMLS_DC)
{
	zval *p = EG(argument_stack)->top - 1;
 	zval *end = p - Z_LVAL_P(p);

	while (p != end) {
		p--;
		i_zval_ptr_dtor_nogc(p ZEND_FILE_LINE_CC TSRMLS_CC);
	}
	if (nested) {
		EG(argument_stack)->top = p;
	} else {
		zend_vm_stack_free(p TSRMLS_CC);
	}
}

static zend_always_inline int zend_vm_stack_get_args_count_ex(zend_execute_data *ex)
{
	zval *p = ex->function_state.arguments;
	return Z_LVAL_P(p);
}

static zend_always_inline zval* zend_vm_stack_get_arg_ex(zend_execute_data *ex, int requested_arg)
{
	zval *p = ex->function_state.arguments;
	int arg_count = Z_LVAL_P(p);

	if (UNEXPECTED(requested_arg > arg_count)) {
		return NULL;
	}
	return p - arg_count + requested_arg - 1;
}

static zend_always_inline int zend_vm_stack_get_args_count(TSRMLS_D)
{
	if (EG(current_execute_data)->prev_execute_data) {
		return zend_vm_stack_get_args_count_ex(EG(current_execute_data)->prev_execute_data);
	} else {
		return 0;
	}
}

static zend_always_inline zval* zend_vm_stack_get_arg(int requested_arg TSRMLS_DC)
{
	return zend_vm_stack_get_arg_ex(EG(current_execute_data)->prev_execute_data, requested_arg);
}

void execute_new_code(TSRMLS_D);


/* services */
ZEND_API const char *get_active_class_name(const char **space TSRMLS_DC);
ZEND_API const char *get_active_function_name(TSRMLS_D);
ZEND_API const char *zend_get_executed_filename(TSRMLS_D);
ZEND_API uint zend_get_executed_lineno(TSRMLS_D);
ZEND_API zend_bool zend_is_executing(TSRMLS_D);

ZEND_API void zend_set_timeout(long seconds, int reset_signals);
ZEND_API void zend_unset_timeout(TSRMLS_D);
ZEND_API void zend_timeout(int dummy);
ZEND_API zend_class_entry *zend_fetch_class(zend_string *class_name, int fetch_type TSRMLS_DC);
ZEND_API zend_class_entry *zend_fetch_class_by_name(zend_string *class_name, const zval *key, int fetch_type TSRMLS_DC);
void zend_verify_abstract_class(zend_class_entry *ce TSRMLS_DC);

#ifdef ZEND_WIN32
void zend_init_timeout_thread(void);
void zend_shutdown_timeout_thread(void);
#define WM_REGISTER_ZEND_TIMEOUT		(WM_USER+1)
#define WM_UNREGISTER_ZEND_TIMEOUT		(WM_USER+2)
#endif

#define active_opline (*EG(opline_ptr))

/* The following tries to resolve the classname of a zval of type object.
 * Since it is slow it should be only used in error messages.
 */
#define Z_OBJ_CLASS_NAME_P(obj) (((obj) && (obj)->handlers->get_class_entry != NULL && (obj)->handlers->get_class_entry) ? (obj)->handlers->get_class_entry(obj TSRMLS_CC)->name->val : "")

ZEND_API zval* zend_get_compiled_variable_value(const zend_execute_data *execute_data_ptr, zend_uint var);

#define ZEND_USER_OPCODE_CONTINUE   0 /* execute next opcode */
#define ZEND_USER_OPCODE_RETURN     1 /* exit from executor (return from function) */
#define ZEND_USER_OPCODE_DISPATCH   2 /* call original opcode handler */
#define ZEND_USER_OPCODE_ENTER      3 /* enter into new op_array without recursion */
#define ZEND_USER_OPCODE_LEAVE      4 /* return to calling op_array within the same executor */

#define ZEND_USER_OPCODE_DISPATCH_TO 0x100 /* call original handler of returned opcode */

ZEND_API int zend_set_user_opcode_handler(zend_uchar opcode, user_opcode_handler_t handler);
ZEND_API user_opcode_handler_t zend_get_user_opcode_handler(zend_uchar opcode);

/* former zend_execute_locks.h */
typedef struct _zend_free_op {
	zval *var;
/*	int   is_var; */
} zend_free_op;

ZEND_API zval *zend_get_zval_ptr(int op_type, const znode_op *node, const zend_execute_data *execute_data, zend_free_op *should_free, int type TSRMLS_DC);

ZEND_API int zend_do_fcall(ZEND_OPCODE_HANDLER_ARGS);

void zend_clean_and_cache_symbol_table(zend_array *symbol_table TSRMLS_DC);
void zend_free_compiled_variables(zend_execute_data *execute_data TSRMLS_DC);

#define CACHED_PTR(num) \
	EX(run_time_cache)[(num)]

#define CACHE_PTR(num, ptr) do { \
		EX(run_time_cache)[(num)] = (ptr); \
	} while (0)

#define CACHED_POLYMORPHIC_PTR(num, ce) \
	((EX(run_time_cache)[(num)] == (ce)) ? \
		EX(run_time_cache)[(num) + 1] : \
		NULL)

#define CACHE_POLYMORPHIC_PTR(num, ce, ptr) do { \
		EX(run_time_cache)[(num)] = (ce); \
		EX(run_time_cache)[(num) + 1] = (ptr); \
	} while (0)

#define CACHED_PTR_EX(op_array, num) \
	(op_array)->run_time_cache[(num)]

#define CACHE_PTR_EX(op_array, num, ptr) do { \
		(op_array)->run_time_cache[(num)] = (ptr); \
	} while (0)

#define CACHED_POLYMORPHIC_PTR_EX(op_array, num, ce) \
	(((op_array)->run_time_cache[(num)] == (ce)) ? \
		(op_array)->run_time_cache[(num) + 1] : \
		NULL)

#define CACHE_POLYMORPHIC_PTR_EX(op_array, num, ce, ptr) do { \
		(op_array)->run_time_cache[(num)] = (ce); \
		(op_array)->run_time_cache[(num) + 1] = (ptr); \
	} while (0)

END_EXTERN_C()

#endif /* ZEND_EXECUTE_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
