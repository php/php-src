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

#ifndef PHP_FUNCTIONS_H
#define PHP_FUNCTIONS_H

#include "php.h"

typedef zend_object_value (*create_object_func_t)(zend_class_entry *class_type TSRMLS_DC);

#define REGISTER_SPL_STD_CLASS(class_name, obj_ctor) \
	spl_register_std_class(&spl_ce_ ## class_name, # class_name, obj_ctor, NULL TSRMLS_CC);

#define REGISTER_SPL_STD_CLASS_EX(class_name, obj_ctor, funcs) \
	spl_register_std_class(&spl_ce_ ## class_name, # class_name, obj_ctor, funcs TSRMLS_CC);

#define REGISTER_SPL_SUB_CLASS_EX(class_name, parent_class_name, obj_ctor, funcs) \
	spl_register_sub_class(&spl_ce_ ## class_name, spl_ce_ ## parent_class_name, # class_name, obj_ctor, funcs TSRMLS_CC);

#define REGISTER_SPL_INTERFACE(class_name) \
	spl_register_interface(&spl_ce_ ## class_name, # class_name, spl_funcs_ ## class_name TSRMLS_CC);

#define REGISTER_SPL_PARENT_CE(class_name, parent_class) \
	spl_register_parent_ce(spl_ce_ ## class_name, spl_ce_ ## parent_class TSRMLS_CC);

#define REGISTER_SPL_IMPLEMENTS(class_name, interface_name) \
	zend_class_implements(spl_ce_ ## class_name TSRMLS_CC, 1, spl_ce_ ## interface_name);

#define REGISTER_SPL_FUNCTIONS(class_name, function_list) \
	spl_register_functions(spl_ce_ ## class_name, function_list TSRMLS_CC);

#define REGISTER_SPL_PROPERTY(class_name, prop_name) \
	spl_register_property(spl_ce_ ## class_name, prop_name, prop_val, prop_flags TSRMLS_CC);

void spl_destroy_class(zend_class_entry ** ppce);

void spl_register_std_class(zend_class_entry ** ppce, char * class_name, create_object_func_t ctor, function_entry * function_list TSRMLS_DC);
void spl_register_sub_class(zend_class_entry ** ppce, zend_class_entry * parent_ce, char * class_name, create_object_func_t ctor, function_entry * function_list TSRMLS_DC);

void spl_register_interface(zend_class_entry ** ppce, char * class_name, zend_function_entry *functions TSRMLS_DC);

void spl_register_parent_ce(zend_class_entry * class_entry, zend_class_entry * parent_class TSRMLS_DC);
void spl_register_functions(zend_class_entry * class_entry, function_entry * function_list TSRMLS_DC);
void spl_register_property( zend_class_entry * class_entry, char *prop_name, zval *prop_val, int prop_flags TSRMLS_DC);

void spl_add_class_name(zval * list, zend_class_entry * pce TSRMLS_DC);
void spl_add_interfaces(zval * list, zend_class_entry * pce TSRMLS_DC);
int spl_add_classes(zend_class_entry ** ppce, zval *list TSRMLS_DC);

#define SPL_ME(class_name, function_name, arg_info, flags) \
	PHP_ME( spl_ ## class_name, function_name, arg_info, flags)
	
#define SPL_ABSTRACT_ME(class_name, arg_info, flags) \
	ZEND_ABSTRACT_ME( spl_ ## class_name, arg_info, flags)

#define SPL_METHOD(class_name, function_name) \
	PHP_METHOD(spl_ ## class_name, function_name)

#endif /* PHP_FUNCTIONS_H */

/*
 * Local Variables:
 * c-basic-offset: 4
 * tab-width: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */
