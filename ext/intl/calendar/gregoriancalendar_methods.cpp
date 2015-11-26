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

static inline GregorianCalendar *fetch_greg(Calendar_object *co) {
	return (GregorianCalendar*)co->ucal;
}

static void _php_intlgregcal_constructor_body(
    INTERNAL_FUNCTION_PARAMETERS, zend_bool is_constructor)
{
	zval		*tz_object	= NULL;
	zval		args_a[6] = {0},
				*args		= &args_a[0];
	char		*locale		= NULL;
	size_t			locale_len;
	zend_long		largs[6];
	UErrorCode	status		= U_ZERO_ERROR;
	int			variant;
  int zpp_flags = is_constructor ? ZEND_PARSE_PARAMS_THROW : 0;
	intl_error_reset(NULL);

	// parameter number validation / variant determination
	if (ZEND_NUM_ARGS() > 6 ||
			zend_get_parameters_array_ex(ZEND_NUM_ARGS(), args) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlgregcal_create_instance: too many arguments", 0);
		if (!is_constructor) {		
			zval_dtor(return_value);
			RETVAL_NULL();
		}
		return;
	}
	for (variant = ZEND_NUM_ARGS();
		variant > 0 && Z_TYPE(args[variant - 1]) == IS_NULL;
		variant--) {}
	if (variant == 4) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlgregcal_create_instance: no variant with 4 arguments "
			"(excluding trailing NULLs)", 0);
		if (!is_constructor) {		
			zval_dtor(return_value);
			RETVAL_NULL();
		}
		return;
	}

	// argument parsing
	if (variant <= 2) {
		if (zend_parse_parameters_ex(zpp_flags, MIN(ZEND_NUM_ARGS(), 2),
				"|z!s!", &tz_object, &locale, &locale_len) == FAILURE) {
			intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
				"intlgregcal_create_instance: bad arguments", 0);
			if (!is_constructor) {		
				zval_dtor(return_value);
				RETVAL_NULL();
			}
			return;
		}
	}
	if (variant > 2 && zend_parse_parameters_ex(zpp_flags, ZEND_NUM_ARGS(),
			"lll|lll", &largs[0], &largs[1], &largs[2], &largs[3], &largs[4],
			&largs[5]) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlgregcal_create_instance: bad arguments", 0);
		if (!is_constructor) {		
			zval_dtor(return_value);
			RETVAL_NULL();
		}
		return;
	}

	// instantion of ICU object
	GregorianCalendar *gcal = NULL;

	if (variant <= 2) {
		// From timezone and locale (0 to 2 arguments)
		TimeZone *tz = timezone_process_timezone_argument(tz_object, NULL,
			"intlgregcal_create_instance");
		if (tz == NULL) {
			if (!EG(exception)) {
				zend_throw_exception(IntlException_ce_ptr, "Constructor failed", 0);
			}
			if (!is_constructor) {		
				zval_dtor(return_value);
				RETVAL_NULL();
			}
			return;
		}
		if (!locale) {
			locale = const_cast<char*>(intl_locale_get_default());
		}

		gcal = new GregorianCalendar(tz, Locale::createFromName(locale),
			status);
		if (U_FAILURE(status)) {
			intl_error_set(NULL, status, "intlgregcal_create_instance: error "
				"creating ICU GregorianCalendar from time zone and locale", 0);
			if (gcal) {
				delete gcal;
			}
			delete tz;
			if (!is_constructor) {		
				zval_dtor(return_value);
				RETVAL_NULL();
			}
			return;
		}
	} else {
		// From date/time (3, 5 or 6 arguments)
		for (int i = 0; i < variant; i++) {
			if (largs[i] < INT32_MIN || largs[i] > INT32_MAX) {
				intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
					"intlgregcal_create_instance: at least one of the arguments"
					" has an absolute value that is too large", 0);
				if (!is_constructor) {		
					zval_dtor(return_value);
					RETVAL_NULL();
				}
				return;
			}
		}

		if (variant == 3) {
			gcal = new GregorianCalendar((int32_t)largs[0], (int32_t)largs[1],
				(int32_t)largs[2], status);
		} else if (variant == 5) {
			gcal = new GregorianCalendar((int32_t)largs[0], (int32_t)largs[1],
				(int32_t)largs[2], (int32_t)largs[3], (int32_t)largs[4], status);
		} else if (variant == 6) {
			gcal = new GregorianCalendar((int32_t)largs[0], (int32_t)largs[1],
				(int32_t)largs[2], (int32_t)largs[3], (int32_t)largs[4], (int32_t)largs[5],
				status);
		}
		if (U_FAILURE(status)) {
			intl_error_set(NULL, status, "intlgregcal_create_instance: error "
				"creating ICU GregorianCalendar from date", 0);
			if (gcal) {
				delete gcal;
			}
			if (!is_constructor) {		
				zval_dtor(return_value);
				RETVAL_NULL();
			}
			return;
		}

		timelib_tzinfo *tzinfo = get_timezone_info();
#if U_ICU_VERSION_MAJOR_NUM * 10 + U_ICU_VERSION_MINOR_NUM >= 42
		UnicodeString tzstr = UnicodeString::fromUTF8(StringPiece(tzinfo->name));
#else
		UnicodeString tzstr = UnicodeString(tzinfo->name,
			strlen(tzinfo->name), US_INV);
#endif
		if (tzstr.isBogus()) {
			intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
				"intlgregcal_create_instance: could not create UTF-8 string "
				"from PHP's default timezone name (see date_default_timezone_get())",
				0);
			delete gcal;
			if (!is_constructor) {		
				zval_dtor(return_value);
				RETVAL_NULL();
			}
			return;
		}

		TimeZone *tz = TimeZone::createTimeZone(tzstr);
		gcal->adoptTimeZone(tz);
	}

    Calendar_object *co = Z_INTL_CALENDAR_P(return_value);
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
	return_value = getThis();
	_php_intlgregcal_constructor_body(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
	zend_restore_error_handling(&error_handling);
}

U_CFUNC PHP_FUNCTION(intlgregcal_set_gregorian_change)
{
	double date;
	CALENDAR_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(),
			"Od", &object, GregorianCalendar_ce_ptr, &date) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlgregcal_set_gregorian_change: bad arguments", 0);
		RETURN_FALSE;
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
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlgregcal_get_gregorian_change: bad arguments", 0);
		RETURN_FALSE;
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
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlgregcal_is_leap_year: bad arguments", 0);
		RETURN_FALSE;
	}

	if (year < INT32_MIN || year > INT32_MAX) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlgregcal_is_leap_year: year out of bounds", 0);
		RETURN_FALSE;
	}

	CALENDAR_METHOD_FETCH_OBJECT;

	RETURN_BOOL((int)fetch_greg(co)->isLeapYear((int32_t)year));
}
