/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Authors: Stanislav Malyshev <stas@zend.com>                          |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

extern "C" {
#include "php_intl.h"
#include "intl_convert.h"
}
#include "formatter_class.h"

#include <unicode/ustring.h>

/* {{{ Get formatter attribute value. */
U_CFUNC PHP_FUNCTION( numfmt_get_attribute )
{
	zend_long lattribute, value;
	FORMATTER_METHOD_INIT_VARS;

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "Ol",
		&object, NumberFormatter_ce_ptr, &lattribute ) == FAILURE )
	{
		RETURN_THROWS();
	}

	UNumberFormatAttribute attribute = static_cast<UNumberFormatAttribute>(lattribute);

	/* Fetch the object. */
	FORMATTER_METHOD_FETCH_OBJECT;

	switch(attribute) {
		case UNUM_PARSE_INT_ONLY:
		case UNUM_GROUPING_USED:
		case UNUM_DECIMAL_ALWAYS_SHOWN:
		case UNUM_MAX_INTEGER_DIGITS:
		case UNUM_MIN_INTEGER_DIGITS:
		case UNUM_INTEGER_DIGITS:
		case UNUM_MAX_FRACTION_DIGITS:
		case UNUM_MIN_FRACTION_DIGITS:
		case UNUM_FRACTION_DIGITS:
		case UNUM_MULTIPLIER:
		case UNUM_GROUPING_SIZE:
		case UNUM_ROUNDING_MODE:
		case UNUM_FORMAT_WIDTH:
		case UNUM_PADDING_POSITION:
		case UNUM_SECONDARY_GROUPING_SIZE:
		case UNUM_SIGNIFICANT_DIGITS_USED:
		case UNUM_MIN_SIGNIFICANT_DIGITS:
		case UNUM_MAX_SIGNIFICANT_DIGITS:
		case UNUM_LENIENT_PARSE:
			value = unum_getAttribute(FORMATTER_OBJECT(nfo), attribute);
			if(value == -1) {
				INTL_DATA_ERROR_CODE(nfo) = U_UNSUPPORTED_ERROR;
			} else {
				RETVAL_LONG(value);
			}
			break;
		case UNUM_ROUNDING_INCREMENT:
		{
			double value_double = unum_getDoubleAttribute(FORMATTER_OBJECT(nfo), attribute);
			if(value_double == -1) {
				INTL_DATA_ERROR_CODE(nfo) = U_UNSUPPORTED_ERROR;
			} else {
				RETVAL_DOUBLE(value_double);
			}
		}
			break;
		default:
			INTL_DATA_ERROR_CODE(nfo) = U_UNSUPPORTED_ERROR;
			break;
	}

	INTL_METHOD_CHECK_STATUS( nfo, "Error getting attribute value" );
}
/* }}} */

/* {{{ Get formatter attribute value. */
U_CFUNC PHP_FUNCTION( numfmt_get_text_attribute )
{
	zend_long   lattribute;
	UChar   value_buf[64];
	int32_t value_buf_size = USIZE( value_buf );
	UChar*  value  = value_buf;
	int32_t length = 0;
	FORMATTER_METHOD_INIT_VARS;

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "Ol",
		&object, NumberFormatter_ce_ptr, &lattribute ) == FAILURE )
	{
		RETURN_THROWS();
	}

	/* Fetch the object. */
	FORMATTER_METHOD_FETCH_OBJECT;

	UNumberFormatTextAttribute attribute = static_cast<UNumberFormatTextAttribute>(lattribute);

	length = unum_getTextAttribute( FORMATTER_OBJECT(nfo), attribute, value, value_buf_size, &INTL_DATA_ERROR_CODE(nfo) );
	if(INTL_DATA_ERROR_CODE(nfo) == U_BUFFER_OVERFLOW_ERROR && length >= value_buf_size) {
		++length; /* to avoid U_STRING_NOT_TERMINATED_WARNING */
		INTL_DATA_ERROR_CODE(nfo) = U_ZERO_ERROR;
		value = eumalloc(length);
		length = unum_getTextAttribute( FORMATTER_OBJECT(nfo), attribute, value, length, &INTL_DATA_ERROR_CODE(nfo) );
		if(U_FAILURE(INTL_DATA_ERROR_CODE(nfo))) {
			efree(value);
			value = value_buf;
		}
	}
	INTL_METHOD_CHECK_STATUS( nfo, "Error getting attribute value" );

	INTL_METHOD_RETVAL_UTF8( nfo, value, length, ( value != value_buf ) );
}
/* }}} */

/* {{{ Get formatter attribute value. */
U_CFUNC PHP_FUNCTION( numfmt_set_attribute )
{
	zend_long lattribute;
	zval *value;
	FORMATTER_METHOD_INIT_VARS;

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "Oln",
		&object, NumberFormatter_ce_ptr, &lattribute, &value ) == FAILURE)
	{
		RETURN_THROWS();
	}

	/* Fetch the object. */
	FORMATTER_METHOD_FETCH_OBJECT;

	UNumberFormatAttribute attribute = static_cast<UNumberFormatAttribute>(lattribute);

	switch(attribute) {
		case UNUM_PARSE_INT_ONLY:
		case UNUM_GROUPING_USED:
		case UNUM_DECIMAL_ALWAYS_SHOWN:
		case UNUM_MAX_INTEGER_DIGITS:
		case UNUM_MIN_INTEGER_DIGITS:
		case UNUM_INTEGER_DIGITS:
		case UNUM_MAX_FRACTION_DIGITS:
		case UNUM_MIN_FRACTION_DIGITS:
		case UNUM_FRACTION_DIGITS:
		case UNUM_MULTIPLIER:
		case UNUM_GROUPING_SIZE:
		case UNUM_ROUNDING_MODE:
		case UNUM_FORMAT_WIDTH:
		case UNUM_PADDING_POSITION:
		case UNUM_SECONDARY_GROUPING_SIZE:
		case UNUM_SIGNIFICANT_DIGITS_USED:
		case UNUM_MIN_SIGNIFICANT_DIGITS:
		case UNUM_MAX_SIGNIFICANT_DIGITS:
		case UNUM_LENIENT_PARSE:
			unum_setAttribute(FORMATTER_OBJECT(nfo), attribute, zval_get_long(value));
			break;
		case UNUM_ROUNDING_INCREMENT:
			unum_setDoubleAttribute(FORMATTER_OBJECT(nfo), attribute, zval_get_double(value));
			break;
		default:
			INTL_DATA_ERROR_CODE(nfo) = U_UNSUPPORTED_ERROR;
			break;
	}

	INTL_METHOD_CHECK_STATUS( nfo, "Error setting attribute value" );

	RETURN_TRUE;
}
/* }}} */

/* {{{ Get formatter attribute value. */
U_CFUNC PHP_FUNCTION( numfmt_set_text_attribute )
{
	int32_t slength = 0;
	UChar *svalue = NULL;
	zend_long attribute;
	char *value;
	size_t len;
	FORMATTER_METHOD_INIT_VARS;

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "Ols",
		&object, NumberFormatter_ce_ptr, &attribute, &value, &len ) == FAILURE)
	{
		RETURN_THROWS();
	}

	/* Fetch the object. */
	FORMATTER_METHOD_FETCH_OBJECT;

	/* Convert given attribute value to UTF-16. */
	intl_convert_utf8_to_utf16(&svalue, &slength, value, len, &INTL_DATA_ERROR_CODE(nfo));
	INTL_METHOD_CHECK_STATUS( nfo, "Error converting attribute value to UTF-16" );

	/* Actually set new attribute value. */
	unum_setTextAttribute(FORMATTER_OBJECT(nfo), static_cast<UNumberFormatTextAttribute>(attribute), svalue, slength, &INTL_DATA_ERROR_CODE(nfo));
	if (svalue) {
		efree(svalue);
	}
	INTL_METHOD_CHECK_STATUS( nfo, "Error setting text attribute" );

	RETURN_TRUE;
}
/* }}} */

/* {{{ Get formatter symbol value. */
U_CFUNC PHP_FUNCTION( numfmt_get_symbol )
{
	zend_long lsymbol;
	UChar value_buf[4];
	UChar *value = value_buf;
	uint32_t length = USIZE(value_buf);
	FORMATTER_METHOD_INIT_VARS;

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "Ol",
		&object, NumberFormatter_ce_ptr, &lsymbol ) == FAILURE )
	{
		RETURN_THROWS();
	}

	UNumberFormatSymbol symbol = static_cast<UNumberFormatSymbol>(lsymbol);

	if(symbol >= UNUM_FORMAT_SYMBOL_COUNT || symbol < 0) {
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,	"invalid symbol value");
		RETURN_FALSE;
	}

	/* Fetch the object. */
	FORMATTER_METHOD_FETCH_OBJECT;

	length = unum_getSymbol(FORMATTER_OBJECT(nfo), symbol, value_buf, length, &INTL_DATA_ERROR_CODE(nfo));
	if(INTL_DATA_ERROR_CODE(nfo) == U_BUFFER_OVERFLOW_ERROR && length >= USIZE( value_buf )) {
		++length; /* to avoid U_STRING_NOT_TERMINATED_WARNING */
		INTL_DATA_ERROR_CODE(nfo) = U_ZERO_ERROR;
		value = eumalloc(length);
		length = unum_getSymbol(FORMATTER_OBJECT(nfo), symbol, value, length, &INTL_DATA_ERROR_CODE(nfo));
		if(U_FAILURE(INTL_DATA_ERROR_CODE(nfo))) {
			efree(value);
			value = value_buf;
		}
	}
	INTL_METHOD_CHECK_STATUS( nfo, "Error getting symbol value" );

	INTL_METHOD_RETVAL_UTF8( nfo, value, length, ( value_buf != value ) );
}
/* }}} */

/* {{{ Set formatter symbol value. */
U_CFUNC PHP_FUNCTION( numfmt_set_symbol )
{
	zend_long  lsymbol;
	char*      value     = NULL;
	size_t     value_len = 0;
	UChar*     svalue  = 0;
	int32_t    slength = 0;
	FORMATTER_METHOD_INIT_VARS;

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "Ols",
		&object, NumberFormatter_ce_ptr, &lsymbol, &value, &value_len ) == FAILURE )
	{
		RETURN_THROWS();
	}

	UNumberFormatSymbol symbol = static_cast<UNumberFormatSymbol>(lsymbol);

	if (symbol >= UNUM_FORMAT_SYMBOL_COUNT || symbol < 0) {
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,	"invalid symbol value");
		RETURN_FALSE;
	}

	/* Fetch the object. */
	FORMATTER_METHOD_FETCH_OBJECT;

	/* Convert given symbol to UTF-16. */
	intl_convert_utf8_to_utf16(&svalue, &slength, value, value_len, &INTL_DATA_ERROR_CODE(nfo));
	INTL_METHOD_CHECK_STATUS( nfo, "Error converting symbol value to UTF-16" );

	/* Actually set the symbol. */
	unum_setSymbol(FORMATTER_OBJECT(nfo), symbol, svalue, slength, &INTL_DATA_ERROR_CODE(nfo));
	if (svalue) {
		efree(svalue);
	}
	INTL_METHOD_CHECK_STATUS( nfo, "Error setting symbol value" );

	RETURN_TRUE;
}
/* }}} */

/* {{{ Get formatter pattern. */
U_CFUNC PHP_FUNCTION( numfmt_get_pattern )
{
	UChar   value_buf[64];
	uint32_t length = USIZE( value_buf );
	UChar*  value  = value_buf;
	FORMATTER_METHOD_INIT_VARS;

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "O",
		&object, NumberFormatter_ce_ptr ) == FAILURE )
	{
		RETURN_THROWS();
	}

	/* Fetch the object. */
	FORMATTER_METHOD_FETCH_OBJECT;

	length = unum_toPattern(FORMATTER_OBJECT(nfo), 0, value, length, &INTL_DATA_ERROR_CODE(nfo));
	if(INTL_DATA_ERROR_CODE(nfo) == U_BUFFER_OVERFLOW_ERROR && length >= USIZE( value_buf )) {
		++length; /* to avoid U_STRING_NOT_TERMINATED_WARNING */
		INTL_DATA_ERROR_CODE(nfo) = U_ZERO_ERROR;
		value = eumalloc(length);
		length = unum_toPattern( FORMATTER_OBJECT(nfo), 0, value, length, &INTL_DATA_ERROR_CODE(nfo) );
		if(U_FAILURE(INTL_DATA_ERROR_CODE(nfo))) {
			efree(value);
			value = value_buf;
		}
	}
	INTL_METHOD_CHECK_STATUS( nfo, "Error getting formatter pattern" );

	INTL_METHOD_RETVAL_UTF8( nfo, value, length, ( value != value_buf ) );
}
/* }}} */

/* {{{ Set formatter pattern. */
U_CFUNC PHP_FUNCTION( numfmt_set_pattern )
{
	char*       value = NULL;
	size_t      value_len = 0;
	int32_t     slength = 0;
	UChar*	    svalue  = NULL;
	UParseError spattern_error = {0};
	FORMATTER_METHOD_INIT_VARS;

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "Os",
		&object, NumberFormatter_ce_ptr, &value, &value_len ) == FAILURE )
	{
		RETURN_THROWS();
	}

	FORMATTER_METHOD_FETCH_OBJECT;

	/* Convert given pattern to UTF-16. */
	intl_convert_utf8_to_utf16(&svalue, &slength, value, value_len, &INTL_DATA_ERROR_CODE(nfo));
	INTL_METHOD_CHECK_STATUS( nfo, "Error converting pattern to UTF-16" );

	unum_applyPattern(FORMATTER_OBJECT(nfo), 0, svalue, slength, &spattern_error, &INTL_DATA_ERROR_CODE(nfo));
	if (svalue) {
		efree(svalue);
	}
	if (U_FAILURE(INTL_DATA_ERROR_CODE(nfo))) {
		char *msg;
		spprintf(&msg, 0, "Error setting pattern value at line %d, offset %d", spattern_error.line, spattern_error.offset);
		intl_errors_set_custom_msg(INTL_DATA_ERROR_P(nfo), msg);
		efree(msg);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ Get formatter locale. */
U_CFUNC PHP_FUNCTION( numfmt_get_locale )
{
	zend_long type = ULOC_ACTUAL_LOCALE;
	const char* loc;
	FORMATTER_METHOD_INIT_VARS;

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "O|l",
		&object, NumberFormatter_ce_ptr, &type ) == FAILURE )
	{
		RETURN_THROWS();
	}

	/* Fetch the object. */
	FORMATTER_METHOD_FETCH_OBJECT;

	loc = unum_getLocaleByType(FORMATTER_OBJECT(nfo), static_cast<ULocDataLocaleType>(type), &INTL_DATA_ERROR_CODE(nfo));
	INTL_METHOD_CHECK_STATUS( nfo, "Error getting locale" );
	RETURN_STRING(loc);
}
/* }}} */
