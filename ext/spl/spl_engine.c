/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
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
PHPAPI void spl_instantiate(zend_class_entry *pce, zval *object)
{
	object_init_ex(object, pce);
}
/* }}} */

PHPAPI zend_long spl_offset_convert_to_long(zval *offset) /* {{{ */
{
	zend_ulong idx;

try_again:
	switch (Z_TYPE_P(offset)) {
	case IS_STRING:
		if (ZEND_HANDLE_NUMERIC(Z_STR_P(offset), idx)) {
			return idx;
		}
		break;
	case IS_DOUBLE:
		return (zend_long)Z_DVAL_P(offset);
	case IS_LONG:
		return Z_LVAL_P(offset);
	case IS_FALSE:
		return 0;
	case IS_TRUE:
		return 1;
	case IS_REFERENCE:
		offset = Z_REFVAL_P(offset);
		goto try_again;
	case IS_RESOURCE:
		return Z_RES_HANDLE_P(offset);
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
