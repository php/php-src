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

#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_spl.h"
#include "spl_foreach.h"

/* {{{ spl_destroy_class */
void spl_destroy_class(zend_class_entry ** ppce)
{                           
	SPL_DEBUG(fprintf(stderr, "Destroy(%s): %s\n", (*ppce)->type == ZEND_USER_CLASS ? "user" : "other", (*ppce)->name);)
	destroy_zend_class(ppce);
}
/* }}} */

/* {{{ spl_register_namespace */
void spl_register_namespace(zend_namespace ** ppns, char * namespace_name TSRMLS_DC)
{
	zend_namespace ns;

	INIT_NAMESPACE(ns, namespace_name);
	ns.name_length = strlen(namespace_name);
	*ppns = zend_register_internal_namespace(&ns TSRMLS_CC);
}
/* }}} */

/* {{{ spl_register_interface */
void spl_register_interface(zend_class_entry ** ppce, zend_namespace * namespace_entry, char * class_name TSRMLS_DC)
{
	zend_class_entry ce;
	
	INIT_CLASS_ENTRY(ce, class_name, NULL);
	ce.name_length = strlen(class_name);
	*ppce = zend_register_internal_ns_class(&ce, NULL, namespace_entry, NULL TSRMLS_CC);

	/* entries changed by initialize */
	(*ppce)->ce_flags = ZEND_ACC_ABSTRACT | ZEND_ACC_INTERFACE;
}
/* }}} */

/* {{{ spl_register_std_class */
void spl_register_std_class(zend_class_entry ** ppce, zend_namespace * namespace_entry, char * class_name, void * obj_ctor TSRMLS_DC)
{
	zend_class_entry ce;
	memset(&ce, 0, sizeof(zend_class_entry));
	
	INIT_CLASS_ENTRY(ce, class_name, NULL);
	ce.name_length = strlen(class_name);
	*ppce = zend_register_internal_ns_class(&ce, NULL, namespace_entry, NULL TSRMLS_CC);

	/* entries changed by initialize */
	(*ppce)->ce_flags = ZEND_ACC_ABSTRACT | ZEND_ACC_INTERFACE;
	(*ppce)->create_object = obj_ctor;
}
/* }}} */

/* {{{ spl_register_interface_function */
void spl_register_interface_function(zend_class_entry * class_entry, char * fn_name TSRMLS_DC)
{
	zend_function function, *reg_function;
	zend_internal_function *pfunction = (zend_internal_function *)&function;

	pfunction->type = ZEND_INTERNAL_FUNCTION;
	pfunction->handler = NULL;
	pfunction->arg_types = NULL;
	pfunction->function_name = fn_name;
	pfunction->scope = class_entry;
	pfunction->fn_flags = ZEND_ACC_ABSTRACT | ZEND_ACC_PUBLIC;
	pfunction->ns = class_entry->ns;
	pfunction->prototype = NULL;
	zend_hash_add(&class_entry->function_table, fn_name, strlen(fn_name)+1, &function, sizeof(zend_function), (void**)&reg_function);
}
/* }}} */

/* {{{ spl_register_parent_ce */
void spl_register_parent_ce(zend_class_entry * class_entry, zend_class_entry * parent_class TSRMLS_DC)
{
	class_entry->parent = parent_class;
}
/* }}} */

/* {{{ spl_register_implement */
void spl_register_implement(zend_class_entry * class_entry, zend_class_entry * interface_entry TSRMLS_DC)
{
	class_entry->interfaces = realloc(class_entry->interfaces, sizeof(zend_class_entry*) * (class_entry->num_interfaces+1));
	class_entry->interfaces[class_entry->num_interfaces++] = interface_entry;
	zend_do_implement_interface(class_entry, interface_entry);
}
/* }}} */

/* {{{ spl_register_functions */
void spl_register_functions(zend_class_entry * class_entry, function_entry * function_list TSRMLS_DC)
{
	zend_register_functions(class_entry, function_list, &class_entry->function_table, MODULE_PERSISTENT TSRMLS_CC);
}
/* }}} */

/* {{ spl_make_fully_qualyfied_name */
char * spl_make_fully_qualyfied_name(zend_class_entry * pce TSRMLS_DC)
{
	if (pce->ns && (pce->ns != &CG(global_namespace))) {
		char *retval;

		spprintf(&retval, 0, "%s::%s", pce->ns->name, pce->name);
		return retval;
	} else {
		return estrdup(pce->name);
	}
}
/* }}} */

/* {{{ spl_add_class_name */
void spl_add_class_name(zval * list, zend_class_entry * pce TSRMLS_DC)
{
	char * str = spl_make_fully_qualyfied_name(pce TSRMLS_CC); 
	size_t len = strlen(str);
	zval *tmp;

	if (zend_hash_find(Z_ARRVAL_P(list), str, len+1, (void*)&tmp) == FAILURE) {
		MAKE_STD_ZVAL(tmp);
		ZVAL_STRING(tmp, str, 0);
		zend_hash_add(Z_ARRVAL_P(list), str, len+1, &tmp, sizeof(zval *), NULL);
	} else {
		efree(str);
	}
}
/* }}} */

/* {{{ spl_add_interfaces */
void spl_add_interfaces(zval *list, zend_class_entry * pce TSRMLS_DC)
{
	zend_uint num_interfaces;

	for (num_interfaces = 0; num_interfaces < pce->num_interfaces; num_interfaces++) {
		spl_add_class_name(list, pce->interfaces[num_interfaces] TSRMLS_CC);
	}
}
/* }}} */

/* {{{ spl_add_classes */
int spl_add_classes(zend_class_entry ** ppce, zval *list TSRMLS_DC)
{
	zend_class_entry *pce = *ppce;
	spl_add_class_name(list, pce TSRMLS_CC);
	spl_add_interfaces(list, pce TSRMLS_CC);
	while (pce->parent) {
		pce = pce->parent;
		spl_add_classes(&pce, list TSRMLS_CC);
	}
	return 0;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */
