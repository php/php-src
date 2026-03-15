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

#include <unicode/fmtable.h>
#include <unicode/curramt.h>
#include "../intl_convertcpp.h"
#include "formatter_class.h"
#include "formatter_format.h"

extern "C" {
#include "php_intl.h"
}

#include <locale.h>

#define ICU_LOCALE_BUG 1

/* {{{ Parse a number. */
U_CFUNC PHP_FUNCTION( numfmt_parse )
{
	zend_long type = FORMAT_TYPE_DOUBLE;
	char* str = NULL;
	size_t str_len;
	int32_t position = 0;
	zval *zposition = NULL;
	char *oldlocale;
	FORMATTER_METHOD_INIT_VARS;

	/* Parse parameters. */
	if (zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "Os|lz!",
		&object, NumberFormatter_ce_ptr,  &str, &str_len, &type, &zposition ) == FAILURE )
	{
		RETURN_THROWS();
	}

	if (zposition) {
		position = (int32_t) zval_get_long(zposition);
	}

	/* Fetch the object. */
	FORMATTER_METHOD_FETCH_OBJECT;

	/* Convert given string to UTF-16. */
	icu::UnicodeString ustr;
	intl_stringFromChar(ustr, str, str_len, &INTL_DATA_ERROR_CODE(nfo));
	INTL_METHOD_CHECK_STATUS( nfo, "String conversion to UTF-16 failed" );

#if ICU_LOCALE_BUG && defined(LC_NUMERIC)
	/* need to copy here since setlocale may change it later */
	oldlocale = estrdup(setlocale(LC_NUMERIC, NULL));
	setlocale(LC_NUMERIC, "C");
#endif

	switch(type) {
		case FORMAT_TYPE_INT32:
		case FORMAT_TYPE_INT64:
		case FORMAT_TYPE_DOUBLE:
		{
			icu::Formattable result;
			icu::ParsePosition pp(position);
			FORMATTER_OBJECT(nfo)->parse(ustr, result, pp);

			if (pp.getErrorIndex() >= 0) {
				INTL_DATA_ERROR_CODE(nfo) = U_PARSE_ERROR;
			} else {
				position = pp.getIndex();
				switch(type) {
					case FORMAT_TYPE_INT32:
						RETVAL_LONG(result.getLong(INTL_DATA_ERROR_CODE(nfo)));
						break;
					case FORMAT_TYPE_INT64:
					{
						int64_t val64 = result.getInt64(INTL_DATA_ERROR_CODE(nfo));
						if(val64 > ZEND_LONG_MAX || val64 < ZEND_LONG_MIN) {
							RETVAL_DOUBLE(val64);
						} else {
							RETVAL_LONG((zend_long)val64);
						}
						break;
					}
					case FORMAT_TYPE_DOUBLE:
						RETVAL_DOUBLE(result.getDouble(INTL_DATA_ERROR_CODE(nfo)));
						break;
				}
			}
			break;
		}
		case FORMAT_TYPE_CURRENCY:
			if (hasThis()) {
				const char *space;
				const char *class_name = get_active_class_name(&space);
				zend_argument_value_error(2, "cannot be NumberFormatter::TYPE_CURRENCY constant, "
					"use %s%sparseCurrency() method instead", class_name, space);
			} else {
				zend_argument_value_error(3, "cannot be NumberFormatter::TYPE_CURRENCY constant, use numfmt_parse_currency() function instead");
			}
			goto cleanup;
		default:
			zend_argument_value_error(hasThis() ? 2 : 3, "must be a NumberFormatter::TYPE_* constant");
			goto cleanup;
	}

	if (zposition) {
		ZEND_TRY_ASSIGN_REF_LONG(zposition, position);
	}

cleanup:

#if ICU_LOCALE_BUG && defined(LC_NUMERIC)
	setlocale(LC_NUMERIC, oldlocale);
	efree(oldlocale);
#endif

	INTL_METHOD_CHECK_STATUS( nfo, "Number parsing failed" );
}
/* }}} */

/* {{{ Parse a number as currency. */
U_CFUNC PHP_FUNCTION( numfmt_parse_currency )
{
	char *str;
	size_t str_len;
	int32_t position = 0;
	zval *zcurrency, *zposition = NULL;
	FORMATTER_METHOD_INIT_VARS;

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "Osz|z!",
		&object, NumberFormatter_ce_ptr,  &str, &str_len, &zcurrency, &zposition ) == FAILURE )
	{
		RETURN_THROWS();
	}

	/* Fetch the object. */
	FORMATTER_METHOD_FETCH_OBJECT;

	/* Convert given string to UTF-16. */
	icu::UnicodeString ustr;
	intl_stringFromChar(ustr, str, str_len, &INTL_DATA_ERROR_CODE(nfo));
	INTL_METHOD_CHECK_STATUS( nfo, "String conversion to UTF-16 failed" );

	if(zposition) {
		position = (int32_t) zval_get_long(zposition);
	}

	icu::ParsePosition pp(position);
	icu::CurrencyAmount *currAmt = FORMATTER_OBJECT(nfo)->parseCurrency(ustr, pp);

	if (currAmt == nullptr || pp.getErrorIndex() >= 0) {
		delete currAmt;
		INTL_DATA_ERROR_CODE(nfo) = U_PARSE_ERROR;
		INTL_METHOD_CHECK_STATUS( nfo, "Number parsing failed" );
	}

	if(zposition) {
		ZEND_TRY_ASSIGN_REF_LONG(zposition, pp.getIndex());
	}

	double number = currAmt->getNumber().getDouble(INTL_DATA_ERROR_CODE(nfo));

	/* Convert parsed currency to UTF-8 and pass it back to caller. */
	icu::UnicodeString ucurrency(currAmt->getISOCurrency());
	delete currAmt;

	zend_string *u8str = intl_charFromString(ucurrency, &INTL_DATA_ERROR_CODE(nfo));
	INTL_METHOD_CHECK_STATUS( nfo, "Currency conversion to UTF-8 failed" );
	ZEND_TRY_ASSIGN_REF_STR(zcurrency, u8str);

	RETVAL_DOUBLE( number );
}
/* }}} */
