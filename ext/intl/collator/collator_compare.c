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
   | Authors: Vadim Savchuk <vsavchuk@productengine.com>                  |
   |          Dmitry Lakhtyuk <dlakhtyuk@productengine.com>               |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php_intl.h"
#include "collator_class.h"
#include "collator_compare.h"
#include "intl_convert.h"

/* {{{ proto int Collator::compare( string $str1, string $str2 )
 * Compare two strings. }}} */
/* {{{ proto int collator_compare( Collator $coll, string $str1, string $str2 )
 * Compare two strings.
 */
PHP_FUNCTION( collator_compare )
{
	char*            str1      = NULL;
	char*            str2      = NULL;
	size_t              str1_len  = 0;
	size_t              str2_len  = 0;

	UChar*           ustr1     = NULL;
	UChar*           ustr2     = NULL;
	int              ustr1_len = 0;
	int              ustr2_len = 0;

	UCollationResult result;

	COLLATOR_METHOD_INIT_VARS

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "Oss",
		&object, Collator_ce_ptr, &str1, &str1_len, &str2, &str2_len ) == FAILURE )
	{
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			 "collator_compare: unable to parse input params", 0 );

		RETURN_FALSE;
	}

	/* Fetch the object. */
	COLLATOR_METHOD_FETCH_OBJECT;

	if (!co || !co->ucoll) {
		intl_error_set_code( NULL, COLLATOR_ERROR_CODE( co ) );
		intl_errors_set_custom_msg( COLLATOR_ERROR_P( co ),
			"Object not initialized", 0 );
		php_error_docref(NULL, E_RECOVERABLE_ERROR, "Object not initialized");

		RETURN_FALSE;
	}

	/*
	 * Compare given strings (converting them to UTF-16 first).
	 */

	/* First convert the strings to UTF-16. */
	intl_convert_utf8_to_utf16(
		&ustr1, &ustr1_len, str1, str1_len, COLLATOR_ERROR_CODE_P( co ) );
	if( U_FAILURE( COLLATOR_ERROR_CODE( co ) ) )
	{
		/* Set global error code. */
		intl_error_set_code( NULL, COLLATOR_ERROR_CODE( co ) );

		/* Set error messages. */
		intl_errors_set_custom_msg( COLLATOR_ERROR_P( co ),
			"Error converting first argument to UTF-16", 0 );
		if (ustr1) {
			efree( ustr1 );
		}
		RETURN_FALSE;
	}

	intl_convert_utf8_to_utf16(
		&ustr2, &ustr2_len, str2, str2_len, COLLATOR_ERROR_CODE_P( co ) );
	if( U_FAILURE( COLLATOR_ERROR_CODE( co ) ) )
	{
		/* Set global error code. */
		intl_error_set_code( NULL, COLLATOR_ERROR_CODE( co ) );

		/* Set error messages. */
		intl_errors_set_custom_msg( COLLATOR_ERROR_P( co ),
			"Error converting second argument to UTF-16", 0 );
		if (ustr1) {
			efree( ustr1 );
		}
		if (ustr2) {
			efree( ustr2 );
		}
		RETURN_FALSE;
	}

	/* Then compare them. */
	result = ucol_strcoll(
		co->ucoll,
		ustr1, ustr1_len,
		ustr2, ustr2_len );

	if( ustr1 )
		efree( ustr1 );
	if( ustr2 )
		efree( ustr2 );

	/* Return result of the comparison. */
	RETURN_LONG( result );
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
