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
   | Authors: Stanislav Malyshev <stas@zend.com>                          |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <unicode/curramt.h>
#include <unicode/fmtable.h>
#include "../intl_convertcpp.h"
#include "formatter_class.h"
#include "formatter_format.h"

extern "C" {
#include "php_intl.h"
}

/* {{{ Format a number. */
U_CFUNC PHP_FUNCTION( numfmt_format )
{
	zval *number;
	zend_long type = FORMAT_TYPE_DEFAULT;
	FORMATTER_METHOD_INIT_VARS;

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "On|l",
		&object, NumberFormatter_ce_ptr,  &number, &type ) == FAILURE )
	{
		RETURN_THROWS();
	}

	/* Fetch the object. */
	FORMATTER_METHOD_FETCH_OBJECT;

	if(type == FORMAT_TYPE_DEFAULT) {
		switch(Z_TYPE_P(number)) {
			case IS_LONG:
				/* take INT32 on 32-bit, int64 on 64-bit */
				type = (sizeof(zend_long) == 8)?FORMAT_TYPE_INT64:FORMAT_TYPE_INT32;
				break;
			case IS_DOUBLE:
				type = FORMAT_TYPE_DOUBLE;
				break;
			EMPTY_SWITCH_DEFAULT_CASE();
		}
	}

	icu::UnicodeString result;
	icu::FieldPosition pos;

	switch(type) {
		case FORMAT_TYPE_INT32:
			convert_to_long(number);
			FORMATTER_OBJECT(nfo)->format((int32_t)Z_LVAL_P(number), result, pos, INTL_DATA_ERROR_CODE(nfo));
			INTL_METHOD_CHECK_STATUS( nfo, "Number formatting failed" );
			break;

		case FORMAT_TYPE_INT64:
		{
			int64_t value = (Z_TYPE_P(number) == IS_DOUBLE)?(int64_t)Z_DVAL_P(number):Z_LVAL_P(number);
			FORMATTER_OBJECT(nfo)->format(value, result, pos, INTL_DATA_ERROR_CODE(nfo));
			INTL_METHOD_CHECK_STATUS( nfo, "Number formatting failed" );
		}
			break;

		case FORMAT_TYPE_DOUBLE:
			convert_to_double(number);
			FORMATTER_OBJECT(nfo)->format(Z_DVAL_P(number), result, pos, INTL_DATA_ERROR_CODE(nfo));
			INTL_METHOD_CHECK_STATUS( nfo, "Number formatting failed" );
			break;
		case FORMAT_TYPE_CURRENCY:
			if (hasThis()) {
				const char *space;
				const char *class_name = get_active_class_name(&space);
				zend_argument_value_error(2, "cannot be NumberFormatter::TYPE_CURRENCY constant, "
					"use %s%sformatCurrency() method instead", class_name, space);
			} else {
				zend_argument_value_error(3, "cannot be NumberFormatter::TYPE_CURRENCY constant, use numfmt_format_currency() function instead");
			}
			RETURN_THROWS();
		default:
			zend_argument_value_error(hasThis() ? 2 : 3, "must be a NumberFormatter::TYPE_* constant");
			RETURN_THROWS();
	}

	zend_string *u8str = intl_charFromString(result, &INTL_DATA_ERROR_CODE(nfo));
	INTL_METHOD_CHECK_STATUS(nfo, "Error converting result to UTF-8");
	RETVAL_STR(u8str);
}
/* }}} */

/* {{{ Format a number as currency. */
U_CFUNC PHP_FUNCTION( numfmt_format_currency )
{
	double     number;
	char*      currency      = NULL;
	size_t     currency_len  = 0;
	FORMATTER_METHOD_INIT_VARS;

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "Ods",
		&object, NumberFormatter_ce_ptr,  &number, &currency, &currency_len ) == FAILURE )
	{
		RETURN_THROWS();
	}

	/* Fetch the object. */
	FORMATTER_METHOD_FETCH_OBJECT;

	/* Convert currency to UTF-16. */
	icu::UnicodeString ucurrency;
	intl_stringFromChar(ucurrency, currency, currency_len, &INTL_DATA_ERROR_CODE(nfo));
	INTL_METHOD_CHECK_STATUS( nfo, "Currency conversion to UTF-16 failed" );

	/* Format using CurrencyAmount. */
	icu::CurrencyAmount *currAmt = new icu::CurrencyAmount(number, ucurrency.getTerminatedBuffer(), INTL_DATA_ERROR_CODE(nfo));
	if (U_FAILURE(INTL_DATA_ERROR_CODE(nfo))) {
		delete currAmt;
		intl_errors_set_custom_msg(INTL_DATA_ERROR_P(nfo), "Number formatting failed");
		RETURN_FALSE;
	}
	icu::Formattable fmt;
	fmt.adoptObject(currAmt);
	icu::UnicodeString result;
	icu::FieldPosition pos;
	FORMATTER_OBJECT(nfo)->format(fmt, result, pos, INTL_DATA_ERROR_CODE(nfo));

	if (U_FAILURE(INTL_DATA_ERROR_CODE(nfo))) {
		intl_errors_set_custom_msg(INTL_DATA_ERROR_P(nfo), "Number formatting failed");
		RETURN_FALSE;
	}

	zend_string *u8str = intl_charFromString(result, &INTL_DATA_ERROR_CODE(nfo));
	INTL_METHOD_CHECK_STATUS(nfo, "Error converting result to UTF-8");
	RETVAL_STR(u8str);
}

/* }}} */
