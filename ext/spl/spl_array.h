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

#ifndef SPL_ARRAY_H
#define SPL_ARRAY_H

#include "php.h"
#include "php_spl.h"

#ifdef SPL_ARRAY_READ
ZEND_EXECUTE_HOOK_FUNCTION(ZEND_FETCH_DIM_R);
ZEND_EXECUTE_HOOK_FUNCTION(ZEND_FETCH_DIM_W);
ZEND_EXECUTE_HOOK_FUNCTION(ZEND_FETCH_DIM_RW);
#endif

#ifdef SPL_ARRAY_WRITE
ZEND_EXECUTE_HOOK_FUNCTION(ZEND_ASSIGN);
#endif

SPL_CLASS_FUNCTION(array_writer_default, __construct);
SPL_CLASS_FUNCTION(array_writer_default, set);

zend_object_value spl_array_writer_default_create(zend_class_entry *class_type TSRMLS_DC);

#endif /* SPL_ARRAY_H */

/*
 * Local Variables:
 * c-basic-offset: 4
 * tab-width: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */
