/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
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
	UChar*           str1      = NULL;
	UChar*           str2      = NULL;
	int              str1_len  = 0;
	int              str2_len  = 0;

	UCollationResult result;

	COLLATOR_METHOD_INIT_VARS

	// Parse parameters.
	if( zend_parse_method_parameters( ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Ouu",
		&object, Collator_ce_ptr, &str1, &str1_len, &str2, &str2_len ) == FAILURE )
	{
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			 "collator_compare: unable to parse input params", 0 TSRMLS_CC );

		RETURN_FALSE;
	}

	// Fetch the object.
	COLLATOR_METHOD_FETCH_OBJECT;


	/*
	 * Compare given strings (converting them to UTF-16 first).
	 */

	// Compare given strings.
	result = ucol_strcoll(
		co->ucoll,
		str1, str1_len,
		str2, str2_len );

	// Return result of the comparison.
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
