/*
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Kirti Velankar <kirtig@yahoo-inc.com>                       |
   +----------------------------------------------------------------------+
*/
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <unicode/udat.h>

#include "php_intl.h"
#include "dateformat_class.h"
#include "dateformat.h"

/* {{{ Get formatter's last error code. */
PHP_FUNCTION( datefmt_get_error_code )
{
	DATE_FORMAT_METHOD_INIT_VARS;

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "O",
		&object, IntlDateFormatter_ce_ptr ) == FAILURE )
	{
		RETURN_THROWS();
	}

	dfo = Z_INTL_DATEFORMATTER_P( object );

	/* Return formatter's last error code. */
	RETURN_LONG( INTL_DATA_ERROR_CODE(dfo) );
}
/* }}} */

/* {{{ Get text description for formatter's last error code. */
PHP_FUNCTION( datefmt_get_error_message )
{
	zend_string *message = NULL;
	DATE_FORMAT_METHOD_INIT_VARS;

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "O",
		&object, IntlDateFormatter_ce_ptr ) == FAILURE )
	{
		RETURN_THROWS();
	}

	dfo = Z_INTL_DATEFORMATTER_P( object );

	/* Return last error message. */
	message = intl_error_get_message( INTL_DATA_ERROR_P(dfo) );
	RETURN_STR( message);
}
/* }}} */
