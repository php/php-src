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
   | Authors: Stanislav Malyshev <stas@zend.com>                          |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <unicode/ustring.h>

#include "php_intl.h"
#include "formatter_class.h"
#include "intl_convert.h"

/* {{{ */
static void numfmt_ctor(INTERNAL_FUNCTION_PARAMETERS)
{
	const char* locale;
	char*       pattern = NULL;
	int         locale_len = 0, pattern_len = 0;
	long        style;
	UChar*      spattern     = NULL;
	int         spattern_len = 0;
	FORMATTER_METHOD_INIT_VARS;

	/* Parse parameters. */
	if( zend_parse_parameters( ZEND_NUM_ARGS() TSRMLS_CC, "sl|s",
		&locale, &locale_len, &style, &pattern, &pattern_len ) == FAILURE )
	{
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"numfmt_create: unable to parse input parameters", 0 TSRMLS_CC );
		zval_dtor(return_value);
		RETURN_NULL();
	}

	INTL_CHECK_LOCALE_LEN_OBJ(locale_len, return_value);
	object = return_value;
	FORMATTER_METHOD_FETCH_OBJECT_NO_CHECK;

	/* Convert pattern (if specified) to UTF-16. */
	if(pattern && pattern_len) {
		intl_convert_utf8_to_utf16(&spattern, &spattern_len, pattern, pattern_len, &INTL_DATA_ERROR_CODE(nfo));
		INTL_CTOR_CHECK_STATUS(nfo, "numfmt_create: error converting pattern to UTF-16");
	}

	if(locale_len == 0) {
		locale = intl_locale_get_default(TSRMLS_C);
	}

	/* Create an ICU number formatter. */
	FORMATTER_OBJECT(nfo) = unum_open(style, spattern, spattern_len, locale, NULL, &INTL_DATA_ERROR_CODE(nfo));

	if(spattern) {
		efree(spattern);
	}

	INTL_CTOR_CHECK_STATUS(nfo, "numfmt_create: number formatter creation failed");
}
/* }}} */

/* {{{ proto NumberFormatter NumberFormatter::create( string $locale, int style[, string $pattern ] )
 * Create number formatter. }}} */
/* {{{ proto NumberFormatter numfmt_create( string $locale, int style[, string $pattern ] )
 * Create number formatter.
 */
PHP_FUNCTION( numfmt_create )
{
	object_init_ex( return_value, NumberFormatter_ce_ptr );
	numfmt_ctor(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto void NumberFormatter::__construct( string $locale, int style[, string $pattern ] )
 * NumberFormatter object constructor.
 */
PHP_METHOD( NumberFormatter, __construct )
{
	return_value = getThis();
	numfmt_ctor(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto int NumberFormatter::getErrorCode()
 * Get formatter's last error code. }}} */
/* {{{ proto int numfmt_get_error_code( NumberFormatter $nf )
 * Get formatter's last error code.
 */
PHP_FUNCTION( numfmt_get_error_code )
{
	FORMATTER_METHOD_INIT_VARS

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O",
		&object, NumberFormatter_ce_ptr ) == FAILURE )
	{
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"numfmt_get_error_code: unable to parse input params", 0 TSRMLS_CC );

		RETURN_FALSE;
	}

	nfo = (NumberFormatter_object *) zend_object_store_get_object( object TSRMLS_CC );

	/* Return formatter's last error code. */
	RETURN_LONG( INTL_DATA_ERROR_CODE(nfo) );
}
/* }}} */

/* {{{ proto string NumberFormatter::getErrorMessage( )
 * Get text description for formatter's last error code. }}} */
/* {{{ proto string numfmt_get_error_message( NumberFormatter $nf )
 * Get text description for formatter's last error code.
 */
PHP_FUNCTION( numfmt_get_error_message )
{
	char*                    message = NULL;
	FORMATTER_METHOD_INIT_VARS

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O",
		&object, NumberFormatter_ce_ptr ) == FAILURE )
	{
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"numfmt_get_error_message: unable to parse input params", 0 TSRMLS_CC );

		RETURN_FALSE;
	}

	nfo = (NumberFormatter_object *) zend_object_store_get_object( object TSRMLS_CC );

	/* Return last error message. */
	message = intl_error_get_message( INTL_DATA_ERROR_P(nfo) TSRMLS_CC );
	RETURN_STRING( message, 0);
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
