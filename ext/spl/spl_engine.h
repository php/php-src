/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2014 The PHP Group                                |
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

#ifndef SPL_ENGINE_H
#define SPL_ENGINE_H

#include "php.h"
#include "php_spl.h"
#include "zend_interfaces.h"

PHPAPI void spl_instantiate(zend_class_entry *pce, zval **object, int alloc TSRMLS_DC);

PHPAPI long spl_offset_convert_to_long(zval *offset TSRMLS_DC);

/* {{{ spl_instantiate_arg_ex1 */
static inline int spl_instantiate_arg_ex1(zend_class_entry *pce, zval **retval, int alloc, zval *arg1 TSRMLS_DC)
{
	spl_instantiate(pce, retval, alloc TSRMLS_CC);
	
	zend_call_method(retval, pce, &pce->constructor, pce->constructor->common.function_name, strlen(pce->constructor->common.function_name), NULL, 1, arg1, NULL TSRMLS_CC);
	return 0;
}
/* }}} */

/* {{{ spl_instantiate_arg_ex2 */
static inline int spl_instantiate_arg_ex2(zend_class_entry *pce, zval **retval, int alloc, zval *arg1, zval *arg2 TSRMLS_DC)
{
	spl_instantiate(pce, retval, alloc TSRMLS_CC);
	
	zend_call_method(retval, pce, &pce->constructor, pce->constructor->common.function_name, strlen(pce->constructor->common.function_name), NULL, 2, arg1, arg2 TSRMLS_CC);
	return 0;
}
/* }}} */

#endif /* SPL_ENGINE_H */

/*
 * Local Variables:
 * c-basic-offset: 4
 * tab-width: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */
