/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998, 1999 Andi Gutmans, Zeev Suraski                  |
   +----------------------------------------------------------------------+
   | This source file is subject to the Zend license, that is bundled     |
   | with this package in the file LICENSE.  If you did not receive a     |
   | copy of the Zend license, please mail us at zend@zend.com so we can  |
   | send you a copy immediately.                                         |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/

#ifndef _EXECUTE_H
#define _EXECUTE_H

#include "zend_compile.h"
#include "zend_hash.h"

typedef union {
	zval tmp_var;
	zval **var;
	struct {
		zval **var_dummy; /* a dummy */

		zval *str;
		int offset;
		unsigned char type;
	} EA;
} temp_variable;


ZEND_API extern void (*zend_execute)(zend_op_array *op_array ELS_DC);

void init_executor(CLS_D ELS_DC);
void shutdown_executor(ELS_D);
void execute(zend_op_array *op_array ELS_DC);
ZEND_API int zend_is_true(zval *op);
ZEND_API inline void safe_free_zval_ptr(zval *p);
ZEND_API void zend_eval_string(char *str, zval *retval CLS_DC ELS_DC);
ZEND_API inline int i_zend_is_true(zval *op);
ZEND_API void zval_update_constant(zval *p);
ZEND_API inline void zend_assign_to_variable_reference(znode *result, zval **variable_ptr_ptr, zval **value_ptr_ptr, temp_variable *Ts ELS_DC);

/* dedicated Zend executor functions - do not use! */
ZEND_API inline void zend_ptr_stack_clear_multiple(ELS_D);
ZEND_API inline int zend_ptr_stack_get_arg(int requested_arg, void **data ELS_DC);

#if SUPPORT_INTERACTIVE
void execute_new_code(CLS_D);
#endif


/* services */
ZEND_API char *get_active_function_name();
ZEND_API char *zend_get_executed_filename(ELS_D);
ZEND_API uint zend_get_executed_lineno(ELS_D);

#define zendi_zval_copy_ctor(p) zval_copy_ctor(&(p))
#define zendi_zval_dtor(p) zval_dtor(&(p))

#define active_opline (*EG(opline_ptr))


#define IS_OVERLOADED_OBJECT 1
#define IS_STRING_OFFSET 2

#endif /* _EXECUTE_H */
