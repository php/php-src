/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000, 2001 The PHP Group             |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
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

int spl_begin_method_call_arg(zval **ce, char *function_name, zval *retval, zval *arg1 TSRMLS_DC);
int spl_begin_method_call_this(zval **ce, char *function_name, zval *retval TSRMLS_DC);
int spl_begin_method_call_arg_ex1(zval **ce, char *function_name, zval **retval, zval **arg1, int no_separation, HashTable *symbol_table TSRMLS_DC);
int spl_begin_method_call_arg_ex2(zval **ce, char *function_name, zval **retval, zval **arg1, zval **arg2, int no_separation, HashTable *symbol_table TSRMLS_DC);

void spl_instanciate(zend_class_entry *pce, zval **object TSRMLS_DC);
int spl_instanciate_arg_ex2(zend_class_entry *pce, zval **retval, zval **arg1, zval **arg2, int no_separation, HashTable *symbol_table TSRMLS_DC);

zval ** spl_get_zval_ptr_ptr(znode *node, temp_variable *Ts TSRMLS_DC);
void spl_unlock_zval_ptr_ptr(znode *node, temp_variable *Ts TSRMLS_DC);
zval * spl_get_zval_ptr(znode *node, temp_variable *Ts, zval **should_free TSRMLS_DC);

int spl_is_instance_of(zval **obj, zend_class_entry *ce TSRMLS_DC);
int spl_implements(zval **obj, zend_class_entry *ce TSRMLS_DC);

#endif /* SPL_ENGINE_H */

/*
 * Local Variables:
 * c-basic-offset: 4
 * tab-width: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */
