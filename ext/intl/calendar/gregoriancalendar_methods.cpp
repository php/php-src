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
   | Authors: Gustavo Lopes <cataphract@php.net>                          |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "../intl_cppshims.h"

#include <unicode/locid.h>
#include <unicode/calendar.h>
#include <unicode/gregocal.h>
#include <unicode/ustring.h>

extern "C" {
#include "../php_intl.h"
#include "../intl_common.h"
#define USE_TIMEZONE_POINTER 1
#include "../timezone/timezone_class.h"
#define USE_CALENDAR_POINTER 1
#include "calendar_class.h"
#include <ext/date/php_date.h>
#include "zend_exceptions.h"
}

using icu::GregorianCalendar;
using icu::Locale;
using icu::UnicodeString;
using icu::StringPiece;

static inline GregorianCalendar *fetch_greg(Calendar_object *co) {
	return (GregorianCalendar*)co->ucal;
}

static void _php_intlgregcal_constructor_body(INTERNAL_FUNCTION_PARAMETERS, bool is_constructor)
{
	UErrorCode status = U_ZERO_ERROR;
	zval *timezone_or_year = NULL;
	zend_string *locale = NULL;
	zend_long month;
	bool is_month_null = true;
	zend_long day_of_month = 0;
	bool is_dom_null = true;
	zend_long hour = 0;
	zend_long minutes = 0;
	zend_long seconds = 0;

	ZEND_PARSE_PARAMETERS_START(0, 6)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(timezone_or_year)
		Z_PARAM_STR_OR_LONG_OR_NULL(locale, month, is_month_null)
		Z_PARAM_LONG_OR_NULL(day_of_month, is_dom_null)
		Z_PARAM_LONG(hour)
		Z_PARAM_LONG(minutes)
		Z_PARAM_LONG(seconds)
	ZEND_PARSE_PARAMETERS_END();

	intl_error_reset(NULL);

	// instantiation of ICU object
	Calendar_object *co = Z_INTL_CALENDAR_P(return_value);
	GregorianCalendar *gcal = NULL;

	if (co->ucal) {
		zend_throw_error(NULL, "IntlGregorianCalendar object is already constructed");
		RETURN_THROWS();
	}

	if (!timezone_or_year || Z_TYPE_P(timezone_or_year) != IS_LONG) {
		if (!locale && !is_month_null) {
			zend_argument_value_error(2, "must be ?string if argument 1 is a timezone");
			RETURN_THROWS();
		}

		TimeZone *tz = timezone_process_timezone_argument(timezone_or_year, NULL,
			"intlgregcal_create_instance");
		if (tz == NULL) {
			if (!EG(exception)) {
				zend_throw_exception(IntlException_ce_ptr, "Constructor failed", 0);
			}
			if (!is_constructor) {
				zval_ptr_dtor(return_value);
				RETURN_NULL();
			}
			RETURN_THROWS();
		}
		char *intl_locale;
		if (locale) {
			intl_locale = ZSTR_VAL(locale);
		} else {
			intl_locale = const_cast<char*>(intl_locale_get_default());
		}

		gcal = new GregorianCalendar(tz, Locale::createFromName(intl_locale), status);
		// Should this always throw?
		if (U_FAILURE(status)) {
			intl_error_set(NULL, status, "intlgregcal_create_instance: error "
				"creating ICU GregorianCalendar from time zone and locale", 0);
			if (gcal) {
				delete gcal;
			}
			delete tz;
			if (!is_constructor) {
				zval_ptr_dtor(return_value);
				RETURN_NULL();
			}
			RETURN_THROWS();
		}
	} else {
		ZEND_ASSERT(Z_TYPE_P(timezone_or_year) == IS_LONG);
		if (locale || is_month_null) {
			zend_argument_value_error(2, "must be int if argument 1 is an int");
			RETURN_THROWS();
		}
		if (is_dom_null) {
			zend_argument_value_error(3, "must be provided");
			RETURN_THROWS();
		}

		zend_long year = Z_LVAL_P(timezone_or_year);
		if (year < INT32_MIN || year > INT32_MAX) {
			zend_argument_value_error(3, "must be between %d and %d", INT32_MIN, INT32_MAX);
			RETURN_THROWS();
		}
		if (month < INT32_MIN || month > INT32_MAX) {
			zend_argument_value_error(3, "must be between %d and %d", INT32_MIN, INT32_MAX);
			RETURN_THROWS();
		}
		if (day_of_month < INT32_MIN || day_of_month > INT32_MAX) {
			zend_argument_value_error(3, "must be between %d and %d", INT32_MIN, INT32_MAX);
			RETURN_THROWS();
		}
		if (hour < INT32_MIN || hour > INT32_MAX) {
			zend_argument_value_error(4, "must be between %d and %d", INT32_MIN, INT32_MAX);
			RETURN_THROWS();
		}
		if (minutes < INT32_MIN || minutes > INT32_MAX) {
			zend_argument_value_error(5, "must be between %d and %d", INT32_MIN, INT32_MAX);
			RETURN_THROWS();
		}
		if (seconds < INT32_MIN || seconds > INT32_MAX) {
			zend_argument_value_error(6, "must be between %d and %d", INT32_MIN, INT32_MAX);
			RETURN_THROWS();
		}

		gcal = new GregorianCalendar((int32_t)year, (int32_t)month, (int32_t)day_of_month,
			(int32_t)hour, (int32_t)minutes, (int32_t)seconds, status);

		if (U_FAILURE(status)) {
			intl_error_set(NULL, status, "intlgregcal_create_instance: error "
				"creating ICU GregorianCalendar from date", 0);
			if (gcal) {
				delete gcal;
			}
			if (!is_constructor) {
				zval_ptr_dtor(return_value);
				RETURN_NULL();
			}
			RETURN_THROWS();
		}

		timelib_tzinfo *tzinfo = get_timezone_info();
		UnicodeString tzstr = UnicodeString::fromUTF8(StringPiece(tzinfo->name));
		if (tzstr.isBogus()) {
			intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
				"intlgregcal_create_instance: could not create UTF-8 string "
				"from PHP's default timezone name (see date_default_timezone_get())",
				0);
			delete gcal;
			if (!is_constructor) {
				zval_ptr_dtor(return_value);
				RETURN_NULL();
			}
			RETURN_THROWS();
		}

		TimeZone *tz = TimeZone::createTimeZone(tzstr);
		gcal->adoptTimeZone(tz);
	}

	co->ucal = gcal;
}

U_CFUNC PHP_FUNCTION(intlgregcal_create_instance)
{
	intl_error_reset(NULL);

	object_init_ex(return_value, GregorianCalendar_ce_ptr);
	_php_intlgregcal_constructor_body(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}

U_CFUNC PHP_METHOD(IntlGregorianCalendar, __construct)
{
	zend_error_handling error_handling;

	zend_replace_error_handling(EH_THROW, IntlException_ce_ptr, &error_handling);
	return_value = ZEND_THIS;
	_php_intlgregcal_constructor_body(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
	zend_restore_error_handling(&error_handling);
}

U_CFUNC PHP_FUNCTION(intlgregcal_set_gregorian_change)
{
	double date;
	CALENDAR_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(),
			"Od", &object, GregorianCalendar_ce_ptr, &date) == FAILURE) {
		RETURN_THROWS();
	}

	CALENDAR_METHOD_FETCH_OBJECT;

	fetch_greg(co)->setGregorianChange(date, CALENDAR_ERROR_CODE(co));
	INTL_METHOD_CHECK_STATUS(co, "intlgregcal_set_gregorian_change: error "
		"calling ICU method");

	RETURN_TRUE;
}

U_CFUNC PHP_FUNCTION(intlgregcal_get_gregorian_change)
{
	CALENDAR_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(),
			"O", &object, GregorianCalendar_ce_ptr) == FAILURE) {
		RETURN_THROWS();
	}

	CALENDAR_METHOD_FETCH_OBJECT;

	RETURN_DOUBLE((double)fetch_greg(co)->getGregorianChange());
}

U_CFUNC PHP_FUNCTION(intlgregcal_is_leap_year)
{
	zend_long year;
	CALENDAR_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(),
			"Ol", &object, GregorianCalendar_ce_ptr, &year) == FAILURE) {
		RETURN_THROWS();
	}

	if (year < INT32_MIN || year > INT32_MAX) {
		zend_argument_value_error(getThis() ? 1 : 2, "must be between %d and %d", INT32_MIN, INT32_MAX);
		RETURN_THROWS();
	}

	CALENDAR_METHOD_FETCH_OBJECT;

	RETURN_BOOL((int)fetch_greg(co)->isLeapYear((int32_t)year));
}
