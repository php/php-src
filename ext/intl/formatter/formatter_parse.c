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
#include "formatter_format.h"
#include "formatter_parse.h"

/* {{{ proto mixed NumberFormatter::parse( string $str[, int $type, int &$position ])
 * Parse a number. }}} */
/* {{{ proto mixed numfmt_parse( NumberFormatter $nf, string $str[, int $type, int &$position ])
 * Parse a number.
 */
PHP_FUNCTION( numfmt_parse )
{
	long type = FORMAT_TYPE_DOUBLE;
	UChar* str = NULL;
	int str_len;
	int32_t val32, position = 0;
	int64_t val64;
	double val_double;
	int32_t* position_p = NULL;
	zval *zposition = NULL;
	FORMATTER_METHOD_INIT_VARS;

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Ou|lz!",
		&object, NumberFormatter_ce_ptr,  &str, &str_len, &type, &zposition ) == FAILURE )
	{
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"number_parse: unable to parse input params", 0 TSRMLS_CC );

		RETURN_FALSE;
	}

	/* Fetch the object. */
	FORMATTER_METHOD_FETCH_OBJECT;

	if(zposition) {
		convert_to_long(zposition);
		position = Z_LVAL_P( zposition );
		position_p = &position;
	}

	switch(type) {
		case FORMAT_TYPE_INT32:
			val32 = unum_parse(FORMATTER_OBJECT(nfo), str, str_len, position_p, &INTL_DATA_ERROR_CODE(nfo));
			RETVAL_LONG(val32);
			break;
		case FORMAT_TYPE_INT64:
			val64 = unum_parseInt64(FORMATTER_OBJECT(nfo), str, str_len, position_p, &INTL_DATA_ERROR_CODE(nfo));
			if(val64 > LONG_MAX || val64 < -LONG_MAX) {
				RETVAL_DOUBLE((double)val64);
			} else {
				val32 = (int32_t)val64;
				RETVAL_LONG(val32);
			}
			break;
		case FORMAT_TYPE_DOUBLE:
			val_double = unum_parseDouble(FORMATTER_OBJECT(nfo), str, str_len, position_p, &INTL_DATA_ERROR_CODE(nfo));
			RETVAL_DOUBLE(val_double);
			break;
		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unsupported format type %ld", type);
			RETVAL_FALSE;
			break;
	}
	if(zposition) {
		zval_dtor(zposition);
		ZVAL_LONG(zposition, position);
	}

	INTL_METHOD_CHECK_STATUS( nfo, "Number parsing failed" );
}
/* }}} */

/* {{{ proto double NumberFormatter::parseCurrency( string $str, string $&currency[, int $&position] )
 * Parse a number as currency. }}} */
/* {{{ proto double numfmt_parse_currency( NumberFormatter $nf, string $str, string $&currency[, int $&position] )
 * Parse a number as currency.
 */
PHP_FUNCTION( numfmt_parse_currency )
{
	double number = 0;
	UChar currency[5] = {0};
	UChar *str = NULL;
	int str_len = 0;
	int32_t* position_p = NULL;
	int32_t position = 0;
	zval *zcurrency = NULL, *zposition = NULL;
	FORMATTER_METHOD_INIT_VARS;

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Ouz|z!",
		&object, NumberFormatter_ce_ptr, &str, &str_len, &zcurrency, &zposition ) == FAILURE )
	{
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"number_parse_currency: unable to parse input params", 0 TSRMLS_CC );

		RETURN_FALSE;
	}

	/* Fetch the object. */
	FORMATTER_METHOD_FETCH_OBJECT;

	if(zposition) {
		convert_to_long(zposition);
		position = Z_LVAL_P( zposition );
		position_p = &position;
	}

	number = unum_parseDoubleCurrency(FORMATTER_OBJECT(nfo), str, str_len, position_p, currency, &INTL_DATA_ERROR_CODE(nfo));
	if(zposition) {
		zval_dtor(zposition);
		ZVAL_LONG(zposition, position);
	}
	INTL_METHOD_CHECK_STATUS( nfo, "Number parsing failed" );

	zval_dtor( zcurrency );
	ZVAL_UNICODE( zcurrency, currency, TRUE );

	RETVAL_DOUBLE( number );
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
