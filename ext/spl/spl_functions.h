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

#ifndef PHP_FUNCTIONS_H
#define PHP_FUNCTIONS_H

#include "php.h"

typedef zend_object_value (*create_object_func_t)(zend_class_entry *class_type TSRMLS_DC);

#define REGISTER_SPL_STD_CLASS(namespace_name, class_name, obj_ctor) \
	spl_register_std_class(&spl_ce_ ## class_name, "spl_" # class_name, obj_ctor TSRMLS_CC);

#define REGISTER_SPL_INTERFACE(namespace_name, class_name) \
	spl_register_interface(&spl_ce_ ## class_name, "spl_" # class_name TSRMLS_CC);

#define REGISTER_SPL_INTF_FUNC(namespace_name, class_name, function_name) \
	spl_register_interface_function(spl_ce_ ## class_name, # function_name TSRMLS_CC);

#define REGISTER_SPL_PARENT_CE(namespace_name, class_name, parent_class) \
	spl_register_parent_ce(spl_ce_ ## class_name, spl_ce_ ## parent_class TSRMLS_CC);

#define REGISTER_SPL_IMPLEMENT(namespace_name, class_name, interface_name) \
	spl_register_implement(spl_ce_ ## class_name, spl_ce_ ## interface_name TSRMLS_CC);

#define REGISTER_SPL_FUNCTIONS(namespace_name, class_name, function_list) \
	spl_register_functions(spl_ce_ ## class_name, function_list TSRMLS_CC);

void spl_destroy_class(zend_class_entry ** ppce);

void spl_register_std_class(zend_class_entry ** ppce, char * class_name, create_object_func_t ctor TSRMLS_DC);

void spl_register_interface(zend_class_entry ** ppce, char * class_name TSRMLS_DC);

void spl_register_interface_function(zend_class_entry * class_entry, char * fn_name TSRMLS_DC);
void spl_register_parent_ce(zend_class_entry * class_entry, zend_class_entry * parent_class TSRMLS_DC);
void spl_register_implement(zend_class_entry * class_entry, zend_class_entry * interface_entry TSRMLS_DC);
void spl_register_functions(zend_class_entry * class_entry, function_entry * function_list TSRMLS_DC);

char * spl_make_fully_qualyfied_name(zend_class_entry * pce TSRMLS_DC);
void spl_add_class_name(zval * list, zend_class_entry * pce TSRMLS_DC);
void spl_add_interfaces(zval * list, zend_class_entry * pce TSRMLS_DC);
int spl_add_classes(zend_class_entry ** ppce, zval *list TSRMLS_DC);

#define SPL_CLASS_FE(class_name, function_name, arg_types) \
	PHP_NAMED_FE( function_name, spl_ ## class_name ## function_name, arg_types)

#define SPL_CLASS_FUNCTION(class_name, function_name) \
	PHP_NAMED_FUNCTION(spl_ ## class_name ## function_name)

#endif /* PHP_FUNCTIONS_H */

/*
 * Local Variables:
 * c-basic-offset: 4
 * tab-width: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */
