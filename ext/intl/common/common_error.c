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
   | Authors: Vadim Savchuk <vsavchuk@productengine.com>                  |
   |          Dmitry Lakhtyuk <dlakhtyuk@productengine.com>               |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "php_intl.h"
#include "intl_error.h"

/* {{{ Get code of the last occurred error. */
PHP_FUNCTION( intl_get_error_code )
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_LONG( intl_error_get_code( NULL ) );
}
/* }}} */

/* {{{ Get text description of the last occurred error. */
PHP_FUNCTION( intl_get_error_message )
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_STR(intl_error_get_message( NULL ));
}
/* }}} */

/* {{{ Check whether the given error code indicates a failure.
 * Returns true if it does, and false if the code
 * indicates success or a warning.
 */
PHP_FUNCTION( intl_is_failure )
{
	zend_long err_code;

	/* Parse parameters. */
	if( zend_parse_parameters( ZEND_NUM_ARGS(), "l",
		&err_code ) == FAILURE )
	{
		RETURN_THROWS();
	}

	RETURN_BOOL( U_FAILURE( err_code ) );
}
/* }}} */

/* {{{ Return a string for a given error code.
 * The string will be the same as the name of the error code constant.
 */
PHP_FUNCTION( intl_error_name )
{
	zend_long err_code;

	/* Parse parameters. */
	if( zend_parse_parameters( ZEND_NUM_ARGS(), "l",
		&err_code ) == FAILURE )
	{
		RETURN_THROWS();
	}

	RETURN_STRING( (char*)u_errorName( err_code ) );
}
/* }}} */
