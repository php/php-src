/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2009 Zend Technologies Ltd. (http://www.zend.com) |
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
ZEND_API int zend_lookup_class_ex(char *name, int name_length, int use_autoload, zend_class_entry ***ce TSRMLS_DC);
ZEND_API int zend_eval_string(char *str, zval *retval_ptr, char *string_name TSRMLS_DC);
ZEND_API int zend_eval_string_ex(char *str, zval *retval_ptr, char *string_name, int handle_exceptions TSRMLS_DC);

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
		case IS_ARRAY:
			result = (zend_hash_num_elements(Z_ARRVAL_P(op))?1:0);
			break;
		case IS_OBJECT:
			if(IS_ZEND_STD_OBJECT(*op)) {
				TSRMLS_FETCH();

				if (Z_OBJ_HT_P(op)->cast_object) {
					zval tmp;
					if (Z_OBJ_HT_P(op)->cast_object(op, &tmp, IS_BOOL TSRMLS_CC) == SUCCESS) {
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
			
				if(EG(ze1_compatibility_mode)) {
					result = (zend_hash_num_elements(Z_OBJPROP_P(op))?1:0);
				} else {
					result = 1;
				}
			} else {
				result = 1;
			}
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
static inline void zend_ptr_stack_clear_multiple(TSRMLS_D)
{
	void **p = EG(argument_stack).top_element-2;
	int delete_count = (int)(zend_uintptr_t) *p;

	EG(argument_stack).top -= (delete_count+2);
	while (--delete_count>=0) {
		zval *q = *(zval **)(--p);
		*p = NULL;
		zval_ptr_dtor(&q);
	}
	EG(argument_stack).top_element = p;
}

static inline int zend_ptr_stack_get_arg(int requested_arg, void **data TSRMLS_DC)
{
	void **p = EG(argument_stack).top_element-2;
	int arg_count = (int)(zend_uintptr_t) *p;

	if (requested_arg>arg_count) {
		return FAILURE;
	}
	*data = (p-arg_count+requested_arg-1);
	return SUCCESS;
}

void execute_new_code(TSRMLS_D);


/* services */
ZEND_API char *get_active_class_name(char **space TSRMLS_DC);
ZEND_API char *get_active_function_name(TSRMLS_D);
ZEND_API char *zend_get_executed_filename(TSRMLS_D);
ZEND_API uint zend_get_executed_lineno(TSRMLS_D);
ZEND_API zend_bool zend_is_executing(TSRMLS_D);

ZEND_API void zend_set_timeout(long seconds);
ZEND_API void zend_unset_timeout(TSRMLS_D);
ZEND_API void zend_timeout(int dummy);
ZEND_API zend_class_entry *zend_fetch_class(char *class_name, uint class_name_len, int fetch_type TSRMLS_DC);
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
#define Z_OBJ_CLASS_NAME_P(zval) ((zval) && Z_TYPE_P(zval) == IS_OBJECT && Z_OBJ_HT_P(zval)->get_class_entry != NULL && Z_OBJ_HT_P(zval)->get_class_entry(zval TSRMLS_CC) ? Z_OBJ_HT_P(zval)->get_class_entry(zval TSRMLS_CC)->name : "")

ZEND_API zval** zend_get_compiled_variable_value(zend_execute_data *execute_data_ptr, zend_uint var);

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
