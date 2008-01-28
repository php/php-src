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

#ifndef ZEND_EXECUTE_H
#define ZEND_EXECUTE_H

#include "zend_compile.h"
#include "zend_hash.h"
#include "zend_operators.h"
#include "zend_variables.h"

typedef union _temp_variable {
	zval tmp_var;
	struct {
		zval **ptr_ptr;
		zval *ptr;
		zend_bool fcall_returned_reference;
	} var;
	struct {
		zval **ptr_ptr;
		zval *ptr;
		zend_bool fcall_returned_reference;
		zval *str;
		zend_uint offset;
	} str_offset;
	struct {
		zval **ptr_ptr;
		zval *ptr;
		zend_bool fcall_returned_reference;
		HashPointer fe_pos;
	} fe;
	zend_class_entry *class_entry;
} temp_variable;


BEGIN_EXTERN_C()
ZEND_API extern UChar u_main[sizeof("main")];
ZEND_API extern UChar u_return[sizeof("return ")];
ZEND_API extern UChar u_semicolon[sizeof(" ;")];
ZEND_API extern UChar u_doublecolon[sizeof("::")];

ZEND_API extern void (*zend_execute)(zend_op_array *op_array TSRMLS_DC);
ZEND_API extern void (*zend_execute_internal)(zend_execute_data *execute_data_ptr, int return_value_used TSRMLS_DC);

void init_executor(TSRMLS_D);
void shutdown_executor(TSRMLS_D);
void shutdown_destructors(TSRMLS_D);
ZEND_API void execute(zend_op_array *op_array TSRMLS_DC);
ZEND_API void execute_internal(zend_execute_data *execute_data_ptr, int return_value_used TSRMLS_DC);
ZEND_API int zend_is_true(zval *op);
#define safe_free_zval_ptr(p) safe_free_zval_ptr_rel(p ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC)
static inline void safe_free_zval_ptr_rel(zval *p ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC)
{
	TSRMLS_FETCH();

	if (p!=EG(uninitialized_zval_ptr)) {
		FREE_ZVAL_REL(p);
	}
}
ZEND_API int zend_lookup_class(char *name, int name_length, zend_class_entry ***ce TSRMLS_DC);
ZEND_API int zend_lookup_class_ex(char *name, int name_length, char *autoload_name, zend_class_entry ***ce TSRMLS_DC);
ZEND_API int zend_u_lookup_class(zend_uchar type, zstr name, int name_length, zend_class_entry ***ce TSRMLS_DC);
ZEND_API int zend_u_lookup_class_ex(zend_uchar type, zstr name, int name_length, zstr autoload_name, int do_normalize, zend_class_entry ***ce TSRMLS_DC);
ZEND_API int zend_eval_string(char *str, zval *retval_ptr, char *string_name TSRMLS_DC);
ZEND_API int zend_eval_string_ex(char *str, zval *retval_ptr, char *string_name, int handle_exceptions TSRMLS_DC);
ZEND_API int zend_u_eval_string(zend_uchar type, zstr str, zval *retval_ptr, char *string_name TSRMLS_DC);
ZEND_API int zend_u_eval_string_ex(zend_uchar type, zstr str, zval *retval_ptr, char *string_name, int handle_exceptions TSRMLS_DC);

static inline int i_zend_is_true(zval *op)
{
	int result;

	switch (Z_TYPE_P(op)) {
		case IS_NULL:
			result = 0;
			break;
		case IS_LONG:
		case IS_BOOL:
		case IS_RESOURCE:
			result = (Z_LVAL_P(op)?1:0);
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
		case IS_UNICODE:
			if (Z_USTRLEN_P(op) == 0
				|| (Z_USTRLEN_P(op)==1 && Z_USTRVAL_P(op)[0]=='0')) {
				result = 0;
			} else {
				result = 1;
			}
			break;
		case IS_ARRAY:
			result = (zend_hash_num_elements(Z_ARRVAL_P(op))?1:0);
			break;
		case IS_OBJECT:
			if(IS_ZEND_STD_OBJECT(*op)) {
				TSRMLS_FETCH();

				if (Z_OBJ_HT_P(op)->cast_object) {
					zval tmp;
					if (Z_OBJ_HT_P(op)->cast_object(op, &tmp, IS_BOOL, NULL TSRMLS_CC) == SUCCESS) {
						result = Z_LVAL(tmp);
						break;
					}
				} else if (Z_OBJ_HT_P(op)->get) {
					zval *tmp = Z_OBJ_HT_P(op)->get(op TSRMLS_CC);
					if(Z_TYPE_P(tmp) != IS_OBJECT) {
						/* for safety - avoid loop */
						convert_to_boolean(tmp);
						result = Z_LVAL_P(tmp);
						zval_ptr_dtor(&tmp);
						break;
					}
				}
			}
			result = 1;
			break;
		default:
			result = 0;
			break;
	}
	return result;
}

ZEND_API int zval_update_constant(zval **pp, void *arg TSRMLS_DC);
ZEND_API int zval_update_constant_ex(zval **pp, void *arg, zend_class_entry *scope TSRMLS_DC);

/* dedicated Zend executor functions - do not use! */
#define ZEND_VM_STACK_PAGE_SIZE (64 * 1024)

struct _zend_vm_stack {
	void **top;
	void **end;
	zend_vm_stack prev;
	void *elements[1];
};

#define ZEND_VM_STACK_GROW_IF_NEEDED(count)							\
	do {															\
		if (UNEXPECTED(count >                                      \
		    EG(argument_stack)->end - EG(argument_stack)->top)) {	\
			zend_vm_stack_extend(count TSRMLS_CC);					\
		}															\
	} while (0)

static inline zend_vm_stack zend_vm_stack_new_page(int count) {
	zend_vm_stack page = (zend_vm_stack)emalloc(sizeof(*page)+sizeof(page->elements[0])*(count-1));

	page->top = page->elements;
	page->end = page->elements + count;
	page->prev = NULL;
	return page;
}

static inline void zend_vm_stack_init(TSRMLS_D)
{
	EG(argument_stack) = zend_vm_stack_new_page(ZEND_VM_STACK_PAGE_SIZE);
}

static inline void zend_vm_stack_destroy(TSRMLS_D)
{
	zend_vm_stack stack = EG(argument_stack);

	while (stack != NULL) {
		zend_vm_stack p = stack->prev;
		efree(stack);
		stack = p;
	}
}

static inline void zend_vm_stack_extend(int count TSRMLS_DC)
{
	zend_vm_stack p = zend_vm_stack_new_page(count >= ZEND_VM_STACK_PAGE_SIZE ? count : ZEND_VM_STACK_PAGE_SIZE);
	p->prev = EG(argument_stack);
	EG(argument_stack) = p;
}

static inline void **zend_vm_stack_top(TSRMLS_D)
{
	return EG(argument_stack)->top;
}

static inline void zend_vm_stack_push(void *ptr TSRMLS_DC)
{
	ZEND_VM_STACK_GROW_IF_NEEDED(1);
	*(EG(argument_stack)->top++) = ptr;
}

static inline void zend_vm_stack_push_nocheck(void *ptr TSRMLS_DC)
{
	*(EG(argument_stack)->top++) = ptr;
}

static inline void *zend_vm_stack_pop(TSRMLS_D)
{
	void *el = *(--EG(argument_stack)->top);

	if (UNEXPECTED(EG(argument_stack)->top == EG(argument_stack)->elements)) {
		zend_vm_stack p = EG(argument_stack);
		EG(argument_stack) = p->prev;
		efree(p);
 	}
	return el;
}

static inline void *zend_vm_stack_alloc(size_t size TSRMLS_DC)
{
	void *ret;

	size = (size + (sizeof(void*) - 1)) / sizeof(void*);

	ZEND_VM_STACK_GROW_IF_NEEDED(size);
	ret = EG(argument_stack)->top;
	EG(argument_stack)->top += size;
	return ret;
}

static inline void zend_vm_stack_free(void *ptr TSRMLS_DC)
{	
	if (UNEXPECTED(EG(argument_stack)->elements == ptr)) {
		zend_vm_stack p = EG(argument_stack);

		EG(argument_stack) = p->prev;
		efree(p);
	} else {
		EG(argument_stack)->top = ptr;
	}
}

static inline void** zend_vm_stack_push_args(int count TSRMLS_DC)
{

	if (UNEXPECTED(EG(argument_stack)->top - EG(argument_stack)->elements < count)  || 
		UNEXPECTED(EG(argument_stack)->top == EG(argument_stack)->end)) {
		zend_vm_stack p = EG(argument_stack);

		zend_vm_stack_extend(count + 1 TSRMLS_CC);

		EG(argument_stack)->top += count;
		*(EG(argument_stack)->top) = (void*)(zend_uintptr_t)count;
		while (count-- > 0) {
			void *data = *(--p->top);

			if (UNEXPECTED(p->top == p->elements)) {
				zend_vm_stack r = p;

				EG(argument_stack)->prev = p->prev;
				p = p->prev;
				efree(r);
			}
			*(EG(argument_stack)->elements + count) = data;
		}
		return EG(argument_stack)->top++;
	}
	*(EG(argument_stack)->top) = (void*)(zend_uintptr_t)count;
	return EG(argument_stack)->top++;
}

static inline void zend_vm_stack_clear_multiple(TSRMLS_D)
{
	void **p = EG(argument_stack)->top - 1;
	int delete_count = (int)(zend_uintptr_t) *p;

	while (--delete_count>=0) {
		zval *q = *(zval **)(--p);
		*p = NULL;
		zval_ptr_dtor(&q);
	}
	zend_vm_stack_free(p TSRMLS_CC);
}

static inline zval** zend_vm_stack_get_arg(int requested_arg TSRMLS_DC)
{
	void **p = EG(current_execute_data)->prev_execute_data->function_state.arguments;
	int arg_count = (int)(zend_uintptr_t) *p;

	if (UNEXPECTED(requested_arg > arg_count)) {
		return NULL;
	}
	return (zval**)p - arg_count + requested_arg - 1;
}

void execute_new_code(TSRMLS_D);


/* services */
ZEND_API zstr get_active_class_name(char **space TSRMLS_DC);
ZEND_API zstr get_active_function_name(TSRMLS_D);
ZEND_API char *zend_get_executed_filename(TSRMLS_D);
ZEND_API uint zend_get_executed_lineno(TSRMLS_D);
ZEND_API zend_bool zend_is_executing(TSRMLS_D);

ZEND_API void zend_set_timeout(long seconds);
ZEND_API void zend_unset_timeout(TSRMLS_D);
ZEND_API void zend_timeout(int dummy);
ZEND_API zend_class_entry *zend_fetch_class(char *class_name, uint class_name_len, int fetch_type TSRMLS_DC);
ZEND_API zend_class_entry *zend_u_fetch_class(zend_uchar type, zstr class_name, uint class_name_len, int fetch_type TSRMLS_DC);
void zend_verify_abstract_class(zend_class_entry *ce TSRMLS_DC);

#ifdef ZEND_WIN32
void zend_init_timeout_thread(void);
void zend_shutdown_timeout_thread(void);
#define WM_REGISTER_ZEND_TIMEOUT		(WM_USER+1)
#define WM_UNREGISTER_ZEND_TIMEOUT		(WM_USER+2)
#endif

#define zendi_zval_copy_ctor(p) zval_copy_ctor(&(p))
#define zendi_zval_dtor(p) zval_dtor(&(p))

#define active_opline (*EG(opline_ptr))

/* The following tries to resolve the classname of a zval of type object.
 * Since it is slow it should be only used in error messages.
 */
#define Z_OBJ_CLASS_NAME_P(zval) ((zval) && Z_TYPE_P(zval) == IS_OBJECT && Z_OBJ_HT_P(zval)->get_class_entry != NULL && Z_OBJ_HT_P(zval)->get_class_entry(zval TSRMLS_CC) ? Z_OBJ_HT_P(zval)->get_class_entry(zval TSRMLS_CC)->name : EMPTY_ZSTR)

ZEND_API zval** zend_get_compiled_variable_value(zend_execute_data *execute_data_ptr, zend_uint var);

void init_unicode_strings(void);

#define ZEND_USER_OPCODE_CONTINUE   0 /* execute next opcode */
#define ZEND_USER_OPCODE_RETURN     1 /* exit from executor (return from function) */
#define ZEND_USER_OPCODE_DISPATCH   2 /* call original opcode handler */

#define ZEND_USER_OPCODE_DISPATCH_TO 0x100 /* call original handler of returned opcode */

ZEND_API int zend_set_user_opcode_handler(zend_uchar opcode, opcode_handler_t handler);
ZEND_API opcode_handler_t zend_get_user_opcode_handler(zend_uchar opcode);

/* former zend_execute_locks.h */
typedef struct _zend_free_op {
	zval* var;
/*	int   is_var; */
} zend_free_op;

ZEND_API zval *zend_get_zval_ptr(znode *node, temp_variable *Ts, zend_free_op *should_free, int type TSRMLS_DC);
ZEND_API zval **zend_get_zval_ptr_ptr(znode *node, temp_variable *Ts, zend_free_op *should_free, int type TSRMLS_DC);

ZEND_API int zend_do_fcall(ZEND_OPCODE_HANDLER_ARGS);

END_EXTERN_C()

#endif /* ZEND_EXECUTE_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
