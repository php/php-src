/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2004 The PHP Group                                |
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

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "zend_interfaces.h"

#include "php_spl.h"
#include "spl_functions.h"
#include "spl_engine.h"

#include "spl_array.h"

/* {{{ spl_instantiate */
void spl_instantiate(zend_class_entry *pce, zval **object, int alloc TSRMLS_DC)
{
	if (alloc) {
		ALLOC_ZVAL(*object);
	}
	object_init_ex(*object, pce);
	(*object)->refcount = 1;
	(*object)->is_ref = 1; /* check if this can be hold always */
}
/* }}} */

/* {{{ spl_is_instance_of */
int spl_is_instance_of(zval **obj, zend_class_entry *ce TSRMLS_DC)
{
	/* Ensure everything needed is available before checking for the type.
	 */
	zend_class_entry *instance_ce;

	if (obj && (instance_ce = spl_get_class_entry(*obj TSRMLS_CC)) != NULL) {
		return instanceof_function(instance_ce, ce TSRMLS_CC);
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
