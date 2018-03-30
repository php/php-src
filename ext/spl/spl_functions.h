/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2018 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Marcus Boerger <helly@php.net>                              |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef PHP_FUNCTIONS_H
#define PHP_FUNCTIONS_H

#include "php.h"

typedef zend_object* (*create_object_func_t)(zend_class_entry *class_type);

#define REGISTER_SPL_STD_CLASS(class_name, obj_ctor) \
	spl_register_std_class(&spl_ce_ ## class_name, # class_name, obj_ctor, NULL);

#define REGISTER_SPL_STD_CLASS_EX(class_name, obj_ctor, funcs) \
	spl_register_std_class(&spl_ce_ ## class_name, # class_name, obj_ctor, funcs);

#define REGISTER_SPL_SUB_CLASS_EX(class_name, parent_class_name, obj_ctor, funcs) \
	spl_register_sub_class(&spl_ce_ ## class_name, spl_ce_ ## parent_class_name, # class_name, obj_ctor, funcs);

#define REGISTER_SPL_INTERFACE(class_name) \
	spl_register_interface(&spl_ce_ ## class_name, # class_name, spl_funcs_ ## class_name);

#define REGISTER_SPL_IMPLEMENTS(class_name, interface_name) \
	zend_class_implements(spl_ce_ ## class_name, 1, spl_ce_ ## interface_name);

#define REGISTER_SPL_ITERATOR(class_name) \
	zend_class_implements(spl_ce_ ## class_name, 1, zend_ce_iterator);

#define REGISTER_SPL_PROPERTY(class_name, prop_name, prop_flags) \
	spl_register_property(spl_ce_ ## class_name, prop_name, sizeof(prop_name)-1, prop_flags);

#define REGISTER_SPL_CLASS_CONST_LONG(class_name, const_name, value) \
	zend_declare_class_constant_long(spl_ce_ ## class_name, const_name, sizeof(const_name)-1, (zend_long)value);

void spl_register_std_class(zend_class_entry ** ppce, char * class_name, create_object_func_t ctor, const zend_function_entry * function_list);
void spl_register_sub_class(zend_class_entry ** ppce, zend_class_entry * parent_ce, char * class_name, create_object_func_t ctor, const zend_function_entry * function_list);
void spl_register_interface(zend_class_entry ** ppce, char * class_name, const zend_function_entry *functions);

void spl_register_property( zend_class_entry * class_entry, char *prop_name, int prop_name_len, int prop_flags);

/* sub: whether to allow subclasses/interfaces
   allow = 0: allow all classes and interfaces
   allow > 0: allow all that match and mask ce_flags
   allow < 0: disallow all that match and mask ce_flags
 */
void spl_add_class_name(zval * list, zend_class_entry * pce, int allow, int ce_flags);
void spl_add_interfaces(zval * list, zend_class_entry * pce, int allow, int ce_flags);
void spl_add_traits(zval * list, zend_class_entry * pce, int allow, int ce_flags);
int spl_add_classes(zend_class_entry *pce, zval *list, int sub, int allow, int ce_flags);

/* caller must efree(return) */
zend_string *spl_gen_private_prop_name(zend_class_entry *ce, char *prop_name, int prop_len);

#define SPL_ME(class_name, function_name, arg_info, flags) \
	PHP_ME( spl_ ## class_name, function_name, arg_info, flags)

#define SPL_ABSTRACT_ME(class_name, function_name, arg_info) \
	ZEND_ABSTRACT_ME( spl_ ## class_name, function_name, arg_info)

#define SPL_METHOD(class_name, function_name) \
	PHP_METHOD(spl_ ## class_name, function_name)

#define SPL_MA(class_name, function_name, alias_class, alias_function, arg_info, flags) \
	PHP_MALIAS(spl_ ## alias_class, function_name, alias_function, arg_info, flags)
#endif /* PHP_FUNCTIONS_H */

/*
 * Local Variables:
 * c-basic-offset: 4
 * tab-width: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */
