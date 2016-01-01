/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2016 The PHP Group                                |
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
PHPAPI void spl_instantiate(zend_class_entry *pce, zval **object, int alloc TSRMLS_DC)
{
	if (alloc) {
		ALLOC_ZVAL(*object);
	}
	object_init_ex(*object, pce);
	Z_SET_REFCOUNT_PP(object, 1);
	Z_SET_ISREF_PP(object); /* check if this can be hold always */
}
/* }}} */

PHPAPI long spl_offset_convert_to_long(zval *offset TSRMLS_DC) /* {{{ */
{
	switch (Z_TYPE_P(offset)) {
	case IS_STRING:
		ZEND_HANDLE_NUMERIC(Z_STRVAL_P(offset), Z_STRLEN_P(offset)+1, idx);
		break;
	case IS_DOUBLE:
		return (long)Z_DVAL_P(offset);
	case IS_RESOURCE:
	case IS_BOOL:
	case IS_LONG:
		return Z_LVAL_P(offset);
	}
	return -1;
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
