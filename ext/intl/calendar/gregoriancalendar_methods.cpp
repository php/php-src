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
#include <memory>

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

#define ZEND_VALUE_ERROR_OUT_OF_BOUND_VALUE(argument, zpp_arg_position) \
	if (UNEXPECTED(argument < INT32_MIN || argument > INT32_MAX)) { \
		zend_argument_value_error(zpp_arg_position, "must be between %d and %d", INT32_MIN, INT32_MAX); \
		RETURN_THROWS(); \
	}

static inline GregorianCalendar *fetch_greg(Calendar_object *co) {
	return static_cast<GregorianCalendar *>(co->ucal);
}

static bool set_gregorian_calendar_time_zone(GregorianCalendar *gcal, UErrorCode status)
{
	if (U_FAILURE(status)) {
		intl_error_set(NULL, status,
			"Error creating ICU GregorianCalendar from date");

		return false;
	}

	timelib_tzinfo *tzinfo = get_timezone_info();
	UnicodeString tzstr = UnicodeString::fromUTF8(StringPiece(tzinfo->name));
	if (tzstr.isBogus()) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
		   "Could not create UTF-8 string from PHP's default timezone "
		   "name (see date_default_timezone_get())");

		return false;
	}

	TimeZone *tz = TimeZone::createTimeZone(tzstr);
	gcal->adoptTimeZone(tz);

	return true;
}

static void _php_intlgregcal_constructor_body(INTERNAL_FUNCTION_PARAMETERS, bool is_constructor)
{
	zend_object *timezone_object = nullptr;
	zend_string *timezone_string = nullptr;
	zval		args_a[6],
				*args		= &args_a[0];
	char		*locale		= NULL;
	size_t			locale_len;
	zend_long		largs[6];
	UErrorCode	status		= U_ZERO_ERROR;
	int			variant;
	intl_error_reset(NULL);

	if (is_constructor && ZEND_NUM_ARGS() > 2) {
		zend_error(E_DEPRECATED, "Calling IntlGregorianCalendar::__construct() with more than 2 arguments is deprecated, "
			"use either IntlGregorianCalendar::createFromDate() or IntlGregorianCalendar::createFromDateTime() instead");
		if (UNEXPECTED(EG(exception))) {
			RETURN_THROWS();
		}
	}

	// parameter number validation / variant determination
	if (ZEND_NUM_ARGS() > 6 ||
			zend_get_parameters_array_ex(ZEND_NUM_ARGS(), args) == FAILURE) {
		zend_argument_count_error("Too many arguments");
		RETURN_THROWS();
	}

	for (variant = ZEND_NUM_ARGS();
		variant > 0 && Z_TYPE(args[variant - 1]) == IS_NULL;
		variant--) {}
	if (variant == 4) {
		zend_argument_count_error("No variant with 4 arguments (excluding trailing NULLs)");
		RETURN_THROWS();
	}

	// argument parsing
	if (variant <= 2) {
		/* These dummy variables are needed because the 2 param constructor allows trailing nulls... */
		zval *dummy1, *dummy2, *dummy3, *dummy4;
		ZEND_PARSE_PARAMETERS_START(0, 6)
			Z_PARAM_OPTIONAL
			Z_PARAM_OBJ_OR_STR_OR_NULL(timezone_object, timezone_string)
			Z_PARAM_STRING_OR_NULL(locale, locale_len)
			Z_PARAM_ZVAL(dummy1)
			Z_PARAM_ZVAL(dummy2)
			Z_PARAM_ZVAL(dummy3)
			Z_PARAM_ZVAL(dummy4)
		ZEND_PARSE_PARAMETERS_END();
	}
	if (variant > 2
		&& zend_parse_parameters(ZEND_NUM_ARGS(), "lll|lll",
			&largs[0], &largs[1], &largs[2], &largs[3], &largs[4], &largs[5]) == FAILURE
	) {
		RETURN_THROWS();
	}

	// instantion of ICU object
	Calendar_object *co = Z_INTL_CALENDAR_P(return_value);
	std::unique_ptr<GregorianCalendar> gcal;

	if (co->ucal) {
		zend_throw_error(NULL, "IntlGregorianCalendar object is already constructed");
		RETURN_THROWS();
	}

	if (variant <= 2) {
		// From timezone and locale (0 to 2 arguments)
		TimeZone *tz = timezone_process_timezone_argument(timezone_object, timezone_string, nullptr);
		if (tz == nullptr) {
			// TODO: Exception should always occur already?
			if (!EG(exception)) {
				zend_throw_exception(IntlException_ce_ptr, "Constructor failed", 0);
			}
			if (!is_constructor) {
				zval_ptr_dtor(return_value);
				RETVAL_NULL();
			}
			return;
		}
		if (!locale) {
			locale = const_cast<char*>(intl_locale_get_default());
		}

		gcal = std::unique_ptr<GregorianCalendar>(new GregorianCalendar(tz, Locale::createFromName(locale),
			status));
			// Should this throw?
		if (U_FAILURE(status)) {
			intl_error_set(NULL, status, "error creating ICU "
				"GregorianCalendar from time zone and locale");
			delete tz;
			if (!is_constructor) {
				zval_ptr_dtor(return_value);
				RETVAL_NULL();
			}
			return;
		}
	} else {
		// From date/time (3, 5 or 6 arguments)
		GregorianCalendar *tmp;
		for (int i = 0; i < variant; i++) {
			ZEND_VALUE_ERROR_OUT_OF_BOUND_VALUE(largs[i], hasThis() ? (i-1) : i);
		}

		if (variant == 3) {
			tmp = new GregorianCalendar((int32_t)largs[0], (int32_t)largs[1],
				(int32_t)largs[2], status);
		} else if (variant == 5) {
			tmp = new GregorianCalendar((int32_t)largs[0], (int32_t)largs[1],
				(int32_t)largs[2], (int32_t)largs[3], (int32_t)largs[4], status);
		} else if (variant == 6) {
			tmp = new GregorianCalendar((int32_t)largs[0], (int32_t)largs[1],
				(int32_t)largs[2], (int32_t)largs[3], (int32_t)largs[4], (int32_t)largs[5],
				status);
		} else {
			ZEND_UNREACHABLE();
		}

		gcal = std::unique_ptr<GregorianCalendar>(tmp);

		if (!set_gregorian_calendar_time_zone(gcal.get(), status)) {
			if (!is_constructor) {
				zval_ptr_dtor(return_value);
				RETVAL_NULL();
			}
			return;
		}
	}

	co->ucal = gcal.release();
}

U_CFUNC PHP_FUNCTION(intlgregcal_create_instance)
{
	object_init_ex(return_value, GregorianCalendar_ce_ptr);
	_php_intlgregcal_constructor_body(INTERNAL_FUNCTION_PARAM_PASSTHRU, /* is_constructor */ false);
}

U_CFUNC PHP_METHOD(IntlGregorianCalendar, __construct)
{
	const bool old_use_exception = INTL_G(use_exceptions);
	const zend_long old_error_level = INTL_G(error_level);
	INTL_G(use_exceptions) = true;
	INTL_G(error_level) = 0;

	return_value = ZEND_THIS;
	_php_intlgregcal_constructor_body(INTERNAL_FUNCTION_PARAM_PASSTHRU, /* is_constructor */ true);
	INTL_G(use_exceptions) = old_use_exception;
	INTL_G(error_level) = old_error_level;
}

U_CFUNC PHP_METHOD(IntlGregorianCalendar, createFromDate)
{
	zend_long year, month, day;
	UErrorCode status = U_ZERO_ERROR;
	Calendar_object *co;
	std::unique_ptr<GregorianCalendar> gcal;

	intl_error_reset(NULL);

	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_LONG(year)
		Z_PARAM_LONG(month)
		Z_PARAM_LONG(day)
	ZEND_PARSE_PARAMETERS_END();

	ZEND_VALUE_ERROR_OUT_OF_BOUND_VALUE(year, 1);
	ZEND_VALUE_ERROR_OUT_OF_BOUND_VALUE(month, 2);
	ZEND_VALUE_ERROR_OUT_OF_BOUND_VALUE(day, 3);

	const bool old_use_exception = INTL_G(use_exceptions);
	const zend_long old_error_level = INTL_G(error_level);

	gcal = std::unique_ptr<GregorianCalendar>(new GregorianCalendar((int32_t) year, (int32_t) month, (int32_t) day, status));
	if (!set_gregorian_calendar_time_zone(gcal.get(), status)) {
		ZEND_ASSERT(EG(exception));
		goto cleanup;
	}

	object_init_ex(return_value, GregorianCalendar_ce_ptr);
	co = Z_INTL_CALENDAR_P(return_value);
	co->ucal = gcal.release();

cleanup:
	INTL_G(use_exceptions) = old_use_exception;
	INTL_G(error_level) = old_error_level;
}

U_CFUNC PHP_METHOD(IntlGregorianCalendar, createFromDateTime)
{
	zend_long year, month, day, hour, minute, second;
	bool second_is_null = 1;
	UErrorCode status = U_ZERO_ERROR;
	Calendar_object *co;
	GregorianCalendar *tmp;

	intl_error_reset(NULL);

	ZEND_PARSE_PARAMETERS_START(5, 6)
		Z_PARAM_LONG(year)
		Z_PARAM_LONG(month)
		Z_PARAM_LONG(day)
		Z_PARAM_LONG(hour)
		Z_PARAM_LONG(minute)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(second, second_is_null)
	ZEND_PARSE_PARAMETERS_END();

	ZEND_VALUE_ERROR_OUT_OF_BOUND_VALUE(year, 1);
	ZEND_VALUE_ERROR_OUT_OF_BOUND_VALUE(month, 2);
	ZEND_VALUE_ERROR_OUT_OF_BOUND_VALUE(day, 3);
	ZEND_VALUE_ERROR_OUT_OF_BOUND_VALUE(hour, 4);
	ZEND_VALUE_ERROR_OUT_OF_BOUND_VALUE(minute, 5);

	if (second_is_null) {
		tmp = new GregorianCalendar((int32_t) year, (int32_t) month, (int32_t) day, (int32_t) hour, (int32_t) minute, status);
	} else {
		ZEND_VALUE_ERROR_OUT_OF_BOUND_VALUE(second, 6);
		tmp = new GregorianCalendar((int32_t) year, (int32_t) month, (int32_t) day, (int32_t) hour, (int32_t) minute, (int32_t) second, status);
	}
	auto gcal = std::unique_ptr<GregorianCalendar>(tmp);
	const bool old_use_exception = INTL_G(use_exceptions);
	const zend_long old_error_level = INTL_G(error_level);
	if (!set_gregorian_calendar_time_zone(gcal.get(), status)) {
		ZEND_ASSERT(EG(exception));
		goto cleanup;
	}

	object_init_ex(return_value, GregorianCalendar_ce_ptr);
	co = Z_INTL_CALENDAR_P(return_value);
	// TODO: trying to get passed the ownership change step
	co->ucal = gcal.release();

cleanup:
	INTL_G(use_exceptions) = old_use_exception;
	INTL_G(error_level) = old_error_level;
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
	INTL_METHOD_CHECK_STATUS(co, "error calling ICU method");

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


	if (UNEXPECTED(year < INT32_MIN || year > INT32_MAX)) {
		zend_argument_value_error(hasThis() ? 1 : 2, "must be between %d and %d", INT32_MIN, INT32_MAX);
		RETURN_THROWS();
	}

	CALENDAR_METHOD_FETCH_OBJECT;

	RETURN_BOOL((int)fetch_greg(co)->isLeapYear((int32_t)year));
}
