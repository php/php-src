/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_0.txt.                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Marcus Boerger <helly@php.net>                              |
   +----------------------------------------------------------------------+
 */

#ifndef SPL_ENGINE_H
#define SPL_ENGINE_H

#include "php.h"
#include "php_spl.h"

#include "zend_compile.h"
#include "zend_execute_locks.h"

#undef EX
#define EX(element) execute_data->element
#define EX_T(offset) (*(temp_variable *)((char *) EX(Ts) + offset))
#define T(offset) (*(temp_variable *)((char *) Ts + offset))

#define NEXT_OPCODE()		\
	EX(opline)++;			\
	return 0;

int zend_do_fcall_common_helper(ZEND_OPCODE_HANDLER_ARGS);

int spl_call_method(zval **object_pp, zend_class_entry *obj_ce, zend_function **fn_proxy, char *function_name, int function_name_len, zval **retval_ptr, HashTable *symbol_table TSRMLS_DC, int param_count, ...);

/* {{{ zend_class_entry */
static inline zend_class_entry *spl_get_class_entry(zval *obj TSRMLS_DC)
{
	if (obj && Z_TYPE_P(obj) == IS_OBJECT && Z_OBJ_HT_P(obj)->get_class_entry) {
		return Z_OBJ_HT_P(obj)->get_class_entry(obj TSRMLS_CC);
	} else {
		return NULL;
	}
}
/* }}} */

/* {{{ spl_begin_method_call_arg */
static inline int spl_begin_method_call_arg(zval **ce, zend_class_entry *obj_ce, zend_function **fn_proxy, char *function_name, int fname_len, zval *retval, zval *arg1 TSRMLS_DC)
{
	zval *local_retval;
	int ret = spl_call_method(ce, obj_ce, fn_proxy, function_name, fname_len, &local_retval, NULL TSRMLS_CC, 1, arg1);
	if (local_retval) {
		COPY_PZVAL_TO_ZVAL(*retval, local_retval);
	} else {
		INIT_ZVAL(*retval);
	}
	return ret;
}
/* }}} */

/* {{{ spl_begin_method_call_this */
static inline int spl_begin_method_call_this(zval **ce, zend_class_entry *obj_ce, zend_function **fn_proxy, char *function_name, int fname_len, zval *retval TSRMLS_DC)
{
	zval *local_retval;
	int ret = spl_call_method(ce, obj_ce, fn_proxy, function_name, fname_len, &local_retval, NULL TSRMLS_CC, 0);
	if (local_retval) {
		COPY_PZVAL_TO_ZVAL(*retval, local_retval);
	} else {
		INIT_ZVAL(*retval);
	}
	return ret;
}
/* }}} */

/* {{{ spl_begin_method_call_ex */
static inline int spl_begin_method_call_ex(zval **ce, zend_class_entry *obj_ce, zend_function **fn_proxy, char *function_name, int fname_len, zval **retval TSRMLS_DC)
{
	return spl_call_method(ce, obj_ce, fn_proxy, function_name, fname_len, retval, NULL TSRMLS_CC, 0);
}
/* }}} */

/* {{{ spl_begin_method_call_arg_ex1 */
static inline int spl_begin_method_call_arg_ex1(zval **ce, zend_class_entry *obj_ce, zend_function **fn_proxy, char *function_name, int fname_len, zval **retval, zval *arg1 TSRMLS_DC)
{
	return spl_call_method(ce, obj_ce, fn_proxy, function_name, fname_len, retval, NULL TSRMLS_CC, 1, arg1);
}
/* }}} */

/* {{{ spl_begin_method_call_arg_ex2 */
static inline int spl_begin_method_call_arg_ex2(zval **ce, zend_class_entry *obj_ce, zend_function **fn_proxy, char *function_name, int fname_len, zval **retval, zval *arg1, zval *arg2 TSRMLS_DC)
{
	return spl_call_method(ce, obj_ce, fn_proxy, function_name, fname_len, retval, NULL TSRMLS_CC, 2, arg1, arg2);
}
/* }}} */

void spl_instanciate(zend_class_entry *pce, zval **object TSRMLS_DC);
int spl_instanciate_arg_ex2(zend_class_entry *pce, zval **retval, zval *arg1, zval *arg2, HashTable *symbol_table TSRMLS_DC);

zval ** spl_get_zval_ptr_ptr(znode *node, temp_variable *Ts TSRMLS_DC);
void spl_unlock_zval_ptr_ptr(znode *node, temp_variable *Ts TSRMLS_DC);
zval * spl_get_zval_ptr(znode *node, temp_variable *Ts, zval **should_free TSRMLS_DC);

int spl_is_instance_of(zval **obj, zend_class_entry *ce TSRMLS_DC);

typedef enum {
	SPL_IS_A_ITERATOR = 1,
	SPL_IS_A_FORWARD  = 2,
	SPL_IS_A_ASSOC    = 4,
	SPL_IS_A_SEQUENCE = 8
} spl_is_a;

spl_is_a spl_implements(zend_class_entry *ce);

#endif /* SPL_ENGINE_H */

/*
 * Local Variables:
 * c-basic-offset: 4
 * tab-width: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */
