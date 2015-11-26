/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
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

/* {{{ dateformat_register_constants
 * Register constants common for the both (OO and procedural)
 * APIs.
 */
void dateformat_register_constants( INIT_FUNC_ARGS )
{
	if( IntlDateFormatter_ce_ptr == NULL) {
		zend_error(E_ERROR, "DateFormat class not defined");
		return;
	}

	#define DATEFORMATTER_EXPOSE_CONST(x) REGISTER_LONG_CONSTANT(#x, x, CONST_PERSISTENT | CONST_CS)
	#define DATEFORMATTER_EXPOSE_CLASS_CONST(x) zend_declare_class_constant_long( IntlDateFormatter_ce_ptr, ZEND_STRS( #x ) - 1, UDAT_##x );
	#define DATEFORMATTER_EXPOSE_CUSTOM_CLASS_CONST(name, value) zend_declare_class_constant_long( IntlDateFormatter_ce_ptr, ZEND_STRS( name ) - 1, value );

	#define DATEFORMATTER_EXPOSE_UCAL_CLASS_CONST(x) zend_declare_class_constant_long( IntlDateFormatter_ce_ptr, ZEND_STRS( #x ) - 1, UCAL_##x );

	/* UDateFormatStyle constants */
	DATEFORMATTER_EXPOSE_CLASS_CONST( FULL );
	DATEFORMATTER_EXPOSE_CLASS_CONST( LONG );
	DATEFORMATTER_EXPOSE_CLASS_CONST( MEDIUM );
	DATEFORMATTER_EXPOSE_CLASS_CONST( SHORT );
	DATEFORMATTER_EXPOSE_CLASS_CONST( NONE );

/*
	DATEFORMATTER_EXPOSE_CUSTOM_CLASS_CONST( "GREGORIAN", DATEF_GREGORIAN );
	DATEFORMATTER_EXPOSE_CUSTOM_CLASS_CONST( "CUSTOMARY", DATEF_CUSTOMARY );
	DATEFORMATTER_EXPOSE_CUSTOM_CLASS_CONST( "BUDDHIST", DATEF_BUDDHIST );
	DATEFORMATTER_EXPOSE_CUSTOM_CLASS_CONST( "JAPANESE_IMPERIAL", DATEF_JAPANESE_IMPERIAL );
*/

	DATEFORMATTER_EXPOSE_UCAL_CLASS_CONST( GREGORIAN );
	DATEFORMATTER_EXPOSE_UCAL_CLASS_CONST( TRADITIONAL );

	#undef DATEFORMATTER_EXPOSE_UCAL_CLASS_CONST
	#undef DATEFORMATTER_EXPOSE_CUSTOM_CLASS_CONST
	#undef DATEFORMATTER_EXPOSE_CLASS_CONST
	#undef DATEFORMATTER_EXPOSE_CONST
}
/* }}} */

/* {{{ proto int IntlDateFormatter::getErrorCode()
 * Get formatter's last error code. }}} */
/* {{{ proto int datefmt_get_error_code( IntlDateFormatter $nf )
 * Get formatter's last error code.
 */
PHP_FUNCTION( datefmt_get_error_code )
{
	DATE_FORMAT_METHOD_INIT_VARS;

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "O",
		&object, IntlDateFormatter_ce_ptr ) == FAILURE )
	{
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"datefmt_get_error_code: unable to parse input params", 0 );
		RETURN_FALSE;
	}

	dfo = Z_INTL_DATEFORMATTER_P( object );

	/* Return formatter's last error code. */
	RETURN_LONG( INTL_DATA_ERROR_CODE(dfo) );
}
/* }}} */

/* {{{ proto string IntlDateFormatter::getErrorMessage( )
 * Get text description for formatter's last error code. }}} */
/* {{{ proto string datefmt_get_error_message( IntlDateFormatter $coll )
 * Get text description for formatter's last error code.
 */
PHP_FUNCTION( datefmt_get_error_message )
{
	zend_string *message = NULL;
	DATE_FORMAT_METHOD_INIT_VARS;

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "O",
		&object, IntlDateFormatter_ce_ptr ) == FAILURE )
	{
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"datefmt_get_error_message: unable to parse input params", 0 );

		RETURN_FALSE;
	}

	dfo = Z_INTL_DATEFORMATTER_P( object );

	/* Return last error message. */
	message = intl_error_get_message( INTL_DATA_ERROR_P(dfo) );
	RETURN_STR( message);
}
/* }}} */
