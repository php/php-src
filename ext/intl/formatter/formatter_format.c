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
   | Authors: Stanislav Malyshev <stas@zend.com>                          |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php_intl.h"

#include <unicode/ustring.h>

#include "formatter_class.h"
#include "formatter_format.h"
#include "intl_convert.h"

/* {{{ proto mixed NumberFormatter::format( mixed $num[, int $type] )
 * Format a number. }}} */
/* {{{ proto mixed numfmt_format( NumberFormatter $nf, mixed $num[, int type] )
 * Format a number.
 */
PHP_FUNCTION( numfmt_format )
{
	zval *number;
	zend_long type = FORMAT_TYPE_DEFAULT;
	UChar format_buf[32];
	UChar* formatted = format_buf;
	int32_t formatted_len = USIZE(format_buf);
	FORMATTER_METHOD_INIT_VARS;

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "Oz|l",
		&object, NumberFormatter_ce_ptr,  &number, &type ) == FAILURE )
	{
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"numfmt_format: unable to parse input params", 0 );

		RETURN_FALSE;
	}

	/* Fetch the object. */
	FORMATTER_METHOD_FETCH_OBJECT;

	if(type == FORMAT_TYPE_DEFAULT) {
		if(Z_TYPE_P(number) == IS_STRING) {
			convert_scalar_to_number_ex(number);
		}

		if(Z_TYPE_P(number) == IS_LONG) {
			/* take INT32 on 32-bit, int64 on 64-bit */
			type = (sizeof(zend_long) == 8)?FORMAT_TYPE_INT64:FORMAT_TYPE_INT32;
		} else if(Z_TYPE_P(number) == IS_DOUBLE) {
			type = FORMAT_TYPE_DOUBLE;
		} else {
			type = FORMAT_TYPE_INT32;
		}
	}

	if(Z_TYPE_P(number) != IS_DOUBLE && Z_TYPE_P(number) != IS_LONG) {
		convert_scalar_to_number(number );
	}

	switch(type) {
		case FORMAT_TYPE_INT32:
			convert_to_long_ex(number);
			formatted_len = unum_format(FORMATTER_OBJECT(nfo), (int32_t)Z_LVAL_P(number),
				formatted, formatted_len, NULL, &INTL_DATA_ERROR_CODE(nfo));
			if (INTL_DATA_ERROR_CODE(nfo) == U_BUFFER_OVERFLOW_ERROR) {
				intl_error_reset(INTL_DATA_ERROR_P(nfo));
				formatted = eumalloc(formatted_len);
				formatted_len = unum_format(FORMATTER_OBJECT(nfo), (int32_t)Z_LVAL_P(number),
					formatted, formatted_len, NULL, &INTL_DATA_ERROR_CODE(nfo));
				if (U_FAILURE( INTL_DATA_ERROR_CODE(nfo) ) ) {
					efree(formatted);
				}
			}
			INTL_METHOD_CHECK_STATUS( nfo, "Number formatting failed" );
			break;

		case FORMAT_TYPE_INT64:
		{
			int64_t value = (Z_TYPE_P(number) == IS_DOUBLE)?(int64_t)Z_DVAL_P(number):Z_LVAL_P(number);
			formatted_len = unum_formatInt64(FORMATTER_OBJECT(nfo), value, formatted, formatted_len, NULL, &INTL_DATA_ERROR_CODE(nfo));
			if (INTL_DATA_ERROR_CODE(nfo) == U_BUFFER_OVERFLOW_ERROR) {
				intl_error_reset(INTL_DATA_ERROR_P(nfo));
				formatted = eumalloc(formatted_len);
				formatted_len = unum_formatInt64(FORMATTER_OBJECT(nfo), value, formatted, formatted_len, NULL, &INTL_DATA_ERROR_CODE(nfo));
				if (U_FAILURE( INTL_DATA_ERROR_CODE(nfo) ) ) {
					efree(formatted);
				}
			}
			INTL_METHOD_CHECK_STATUS( nfo, "Number formatting failed" );
		}
			break;

		case FORMAT_TYPE_DOUBLE:
			convert_to_double_ex(number);
			formatted_len = unum_formatDouble(FORMATTER_OBJECT(nfo), Z_DVAL_P(number), formatted, formatted_len, NULL, &INTL_DATA_ERROR_CODE(nfo));
			if (INTL_DATA_ERROR_CODE(nfo) == U_BUFFER_OVERFLOW_ERROR) {
				intl_error_reset(INTL_DATA_ERROR_P(nfo));
				formatted = eumalloc(formatted_len);
				unum_formatDouble(FORMATTER_OBJECT(nfo), Z_DVAL_P(number), formatted, formatted_len, NULL, &INTL_DATA_ERROR_CODE(nfo));
				if (U_FAILURE( INTL_DATA_ERROR_CODE(nfo) ) ) {
					efree(formatted);
				}
			}
			INTL_METHOD_CHECK_STATUS( nfo, "Number formatting failed" );
			break;

		default:
			php_error_docref(NULL, E_WARNING, "Unsupported format type " ZEND_LONG_FMT, type);
			RETURN_FALSE;
			break;
	}

	INTL_METHOD_RETVAL_UTF8( nfo, formatted, formatted_len, ( formatted != format_buf ) );
}
/* }}} */

/* {{{ proto mixed NumberFormatter::formatCurrency( double $num, string $currency )
 * Format a number as currency. }}} */
/* {{{ proto mixed numfmt_format_currency( NumberFormatter $nf, double $num, string $currency )
 * Format a number as currency.
 */
PHP_FUNCTION( numfmt_format_currency )
{
	double     number;
	UChar      format_buf[32];
	UChar*     formatted     = format_buf;
	int32_t    formatted_len = USIZE(format_buf);
	char*      currency      = NULL;
	size_t     currency_len  = 0;
	UChar*     scurrency     = NULL;
	int32_t    scurrency_len = 0;
	FORMATTER_METHOD_INIT_VARS;

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "Ods",
		&object, NumberFormatter_ce_ptr,  &number, &currency, &currency_len ) == FAILURE )
	{
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"numfmt_format_currency: unable to parse input params", 0 );

		RETURN_FALSE;
	}

	/* Fetch the object. */
	FORMATTER_METHOD_FETCH_OBJECT;

	/* Convert currency to UTF-16. */
	intl_convert_utf8_to_utf16(&scurrency, &scurrency_len, currency, currency_len, &INTL_DATA_ERROR_CODE(nfo));
	INTL_METHOD_CHECK_STATUS( nfo, "Currency conversion to UTF-16 failed" );

	/* Format the number using a fixed-length buffer. */
	formatted_len = unum_formatDoubleCurrency(FORMATTER_OBJECT(nfo), number, scurrency, formatted, formatted_len, NULL, &INTL_DATA_ERROR_CODE(nfo));

	/* If the buffer turned out to be too small
	 * then allocate another buffer dynamically
	 * and use it to format the number.
	 */
	if (INTL_DATA_ERROR_CODE(nfo) == U_BUFFER_OVERFLOW_ERROR) {
		intl_error_reset(INTL_DATA_ERROR_P(nfo));
		formatted = eumalloc(formatted_len);
		unum_formatDoubleCurrency(FORMATTER_OBJECT(nfo), number, scurrency, formatted, formatted_len, NULL, &INTL_DATA_ERROR_CODE(nfo));
	}

	if( U_FAILURE( INTL_DATA_ERROR_CODE((nfo)) ) ) {
		intl_error_set_code( NULL, INTL_DATA_ERROR_CODE((nfo)) );
		intl_errors_set_custom_msg( INTL_DATA_ERROR_P(nfo), "Number formatting failed", 0 );
		RETVAL_FALSE;
		if (formatted != format_buf) {
			efree(formatted);
		}
	} else {
		INTL_METHOD_RETVAL_UTF8( nfo, formatted, formatted_len, ( formatted != format_buf ) );
	}

	if(scurrency) {
		efree(scurrency);
	}
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
