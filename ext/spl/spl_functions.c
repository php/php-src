/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Marcus Boerger <helly@php.net>                              |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif

#include "php.h"

/* {{{ spl_add_class_name */
void spl_add_class_name(zval *list, zend_class_entry *pce, int allow, int ce_flags)
{
	if (!allow || (allow > 0 && (pce->ce_flags & ce_flags)) || (allow < 0 && !(pce->ce_flags & ce_flags))) {
		zval *tmp;

		if ((tmp = zend_hash_find(Z_ARRVAL_P(list), pce->name)) == NULL) {
			zval t;
			ZVAL_STR_COPY(&t, pce->name);
			zend_hash_add(Z_ARRVAL_P(list), pce->name, &t);
		}
	}
}
/* }}} */

/* {{{ spl_add_interfaces */
void spl_add_interfaces(zval *list, zend_class_entry * pce, int allow, int ce_flags)
{
	if (pce->num_interfaces) {
		ZEND_ASSERT(pce->ce_flags & ZEND_ACC_LINKED);
		for (uint32_t num_interfaces = 0; num_interfaces < pce->num_interfaces; num_interfaces++) {
			spl_add_class_name(list, pce->interfaces[num_interfaces], allow, ce_flags);
		}
	}
}
/* }}} */

/* {{{ spl_add_traits */
void spl_add_traits(zval *list, zend_class_entry * pce, int allow, int ce_flags)
{
	zend_class_entry *trait;

	for (uint32_t num_traits = 0; num_traits < pce->num_traits; num_traits++) {
		trait = zend_fetch_class_by_name(pce->trait_names[num_traits].name,
			pce->trait_names[num_traits].lc_name, ZEND_FETCH_CLASS_TRAIT);
		ZEND_ASSERT(trait);
		spl_add_class_name(list, trait, allow, ce_flags);
	}
}
/* }}} */


/* {{{ spl_add_classes */
void spl_add_classes(zend_class_entry *pce, zval *list, bool sub, int allow, int ce_flags)
{
	ZEND_ASSERT(pce);
	spl_add_class_name(list, pce, allow, ce_flags);
	if (sub) {
		spl_add_interfaces(list, pce, allow, ce_flags);
		while (pce->parent) {
			pce = pce->parent;
			spl_add_classes(pce, list, sub, allow, ce_flags);
		}
	}
}
/* }}} */

void spl_set_private_debug_info_property(
	const zend_class_entry *ce,
	const char *property,
	size_t property_len,
	HashTable *debug_info,
	zval *value
)
{
	zend_string *mangled_named = zend_mangle_property_name(
		ZSTR_VAL(ce->name),
		ZSTR_LEN(ce->name),
		property,
		property_len,
		/* persistent */ false
	);
	zend_hash_update(debug_info, mangled_named, value);
	zend_string_release_ex(mangled_named, /* persistent */ false);
}
