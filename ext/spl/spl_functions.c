/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2009 The PHP Group                                |
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

#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_spl.h"

/* {{{ spl_destroy_class */
void spl_destroy_class(zend_class_entry ** ppce)
{                           
	SPL_DEBUG(fprintf(stderr, "Destroy(%s): %s\n", (*ppce)->type == ZEND_USER_CLASS ? "user" : "other", (*ppce)->name);)
	destroy_zend_class(ppce);
}
/* }}} */

/* {{{ spl_register_interface */
void spl_register_interface(zend_class_entry ** ppce, char * class_name, zend_function_entry * functions TSRMLS_DC)
{
	zend_class_entry ce;
	
	INIT_CLASS_ENTRY(ce, class_name, functions);
	ce.name_length = strlen(class_name);
	*ppce = zend_register_internal_interface(&ce TSRMLS_CC);
}
/* }}} */

/* {{{ spl_register_std_class */
PHPAPI void spl_register_std_class(zend_class_entry ** ppce, char * class_name, void * obj_ctor, zend_function_entry * function_list TSRMLS_DC)
{
	zend_class_entry ce;
	
	INIT_CLASS_ENTRY(ce, class_name, function_list);
	ce.name_length = strlen(class_name);
	*ppce = zend_register_internal_class(&ce TSRMLS_CC);

	/* entries changed by initialize */
	if (obj_ctor) {
		(*ppce)->create_object = obj_ctor;
	}
}
/* }}} */

/* {{{ spl_register_sub_class */
PHPAPI void spl_register_sub_class(zend_class_entry ** ppce, zend_class_entry * parent_ce, char * class_name, void *obj_ctor, zend_function_entry * function_list TSRMLS_DC)
{
	zend_class_entry ce;
	
	INIT_CLASS_ENTRY(ce, class_name, function_list);
	ce.name_length = strlen(class_name);
	*ppce = zend_register_internal_class_ex(&ce, parent_ce, NULL TSRMLS_CC);

	/* entries changed by initialize */
	if (obj_ctor) {
		(*ppce)->create_object = obj_ctor;
	} else {
		(*ppce)->create_object = parent_ce->create_object;
	}
}
/* }}} */

/* {{{ spl_register_parent_ce */
void spl_register_parent_ce(zend_class_entry * class_entry, zend_class_entry * parent_class TSRMLS_DC)
{
	class_entry->parent = parent_class;
}
/* }}} */

/* {{{ spl_register_functions */
void spl_register_functions(zend_class_entry * class_entry, zend_function_entry * function_list TSRMLS_DC)
{
	zend_register_functions(class_entry, function_list, &class_entry->function_table, MODULE_PERSISTENT TSRMLS_CC);
}
/* }}} */

/* {{{ spl_register_property */
void spl_register_property( zend_class_entry * class_entry, char *prop_name, int prop_name_len, int prop_flags TSRMLS_DC)
{
	zend_declare_property_null(class_entry, prop_name, prop_name_len, prop_flags TSRMLS_CC);
}
/* }}} */

/* {{{ spl_add_class_name */
void spl_add_class_name(zval *list, zend_class_entry * pce, int allow, int ce_flags TSRMLS_DC)
{
	if (!allow || (allow > 0 && pce->ce_flags & ce_flags) || (allow < 0 && !(pce->ce_flags & ce_flags))) {
		size_t len = strlen(pce->name);
		zval *tmp;

		if (zend_hash_find(Z_ARRVAL_P(list), pce->name, len+1, (void*)&tmp) == FAILURE) {
			MAKE_STD_ZVAL(tmp);
			ZVAL_STRING(tmp, pce->name, 1);
			zend_hash_add(Z_ARRVAL_P(list), pce->name, len+1, &tmp, sizeof(zval *), NULL);
		}
	}
}
/* }}} */

/* {{{ spl_add_interfaces */
void spl_add_interfaces(zval *list, zend_class_entry * pce, int allow, int ce_flags TSRMLS_DC)
{
	zend_uint num_interfaces;

	for (num_interfaces = 0; num_interfaces < pce->num_interfaces; num_interfaces++) {
		spl_add_class_name(list, pce->interfaces[num_interfaces], allow, ce_flags TSRMLS_CC);
	}
}
/* }}} */

/* {{{ spl_add_classes */
int spl_add_classes(zend_class_entry ** ppce, zval *list, int sub, int allow, int ce_flags TSRMLS_DC)
{
	zend_class_entry *pce = *ppce;

	if (!pce) {
		return 0;
	}
	spl_add_class_name(list, pce, allow, ce_flags TSRMLS_CC);
	if (sub) {
		spl_add_interfaces(list, pce, allow, ce_flags TSRMLS_CC);
		while (pce->parent) {
			pce = pce->parent;
			spl_add_classes(&pce, list, sub, allow, ce_flags TSRMLS_CC);
		}
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
