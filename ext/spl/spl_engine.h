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

void spl_instantiate(zend_class_entry *pce, zval **object, int alloc TSRMLS_DC);
int spl_instantiate_arg_ex1(zend_class_entry *pce, zval **retval, int alloc, zval *arg1 TSRMLS_DC);
int spl_instantiate_arg_ex2(zend_class_entry *pce, zval **retval, int alloc, zval *arg1, zval *arg2 TSRMLS_DC);

int spl_is_instance_of(zval **obj, zend_class_entry *ce TSRMLS_DC);

#endif /* SPL_ENGINE_H */

/*
 * Local Variables:
 * c-basic-offset: 4
 * tab-width: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */
