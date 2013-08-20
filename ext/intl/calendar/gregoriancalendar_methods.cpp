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
#define USE_TIMEZONE_POINTER 1
#include "../timezone/timezone_class.h"
#define USE_CALENDAR_POINTER 1
#include "calendar_class.h"
#include <ext/date/php_date.h>
}

static inline GregorianCalendar *fetch_greg(Calendar_object *co) {
	return (GregorianCalendar*)co->ucal;
}

static void _php_intlgregcal_constructor_body(INTERNAL_FUNCTION_PARAMETERS)
{
	zval		**tz_object	= NULL;
	zval		**args_a[6] = {0},
				***args		= &args_a[0];
	char		*locale		= NULL;
	int			locale_len;
	long		largs[6];
	UErrorCode	status		= U_ZERO_ERROR;
	int			variant;
	intl_error_reset(NULL TSRMLS_CC);
	
	// parameter number validation / variant determination
	if (ZEND_NUM_ARGS() > 6 ||
			zend_get_parameters_array_ex(ZEND_NUM_ARGS(), args) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlgregcal_create_instance: too many arguments", 0 TSRMLS_CC);
		RETURN_NULL();
	}
	for (variant = ZEND_NUM_ARGS();
		variant > 0 && Z_TYPE_PP(args[variant - 1]) == IS_NULL;
		variant--) {}
	if (variant == 4) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlgregcal_create_instance: no variant with 4 arguments "
			"(excluding trailing NULLs)", 0 TSRMLS_CC);
		RETURN_NULL();
	}

	// argument parsing
	if (variant <= 2) {
		if (zend_parse_parameters(MIN(ZEND_NUM_ARGS(), 2) TSRMLS_CC,
				"|Z!s!", &tz_object, &locale, &locale_len) == FAILURE) {
			intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
				"intlgregcal_create_instance: bad arguments", 0 TSRMLS_CC);
			RETURN_NULL();
		}
	}
	if (variant > 2 && zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
			"lll|lll", &largs[0], &largs[1], &largs[2], &largs[3], &largs[4],
			&largs[5]) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlgregcal_create_instance: bad arguments", 0 TSRMLS_CC);
		RETURN_NULL();
	}
	
	// instantion of ICU object
	GregorianCalendar *gcal = NULL;

	if (variant <= 2) {
		// From timezone and locale (0 to 2 arguments)
		TimeZone *tz = timezone_process_timezone_argument(tz_object, NULL,
			"intlgregcal_create_instance" TSRMLS_CC);
		if (tz == NULL) {
			RETURN_NULL();
		}
		if (!locale) {
			locale = const_cast<char*>(intl_locale_get_default(TSRMLS_C));
		}
		
		gcal = new GregorianCalendar(tz, Locale::createFromName(locale),
			status);
		if (U_FAILURE(status)) {
			intl_error_set(NULL, status, "intlgregcal_create_instance: error "
				"creating ICU GregorianCalendar from time zone and locale", 0 TSRMLS_CC);
			if (gcal) {
				delete gcal;
			}
			delete tz;
            RETURN_NULL();
		}
	} else {
		// From date/time (3, 5 or 6 arguments)
		for (int i = 0; i < variant; i++) {
			if (largs[i] < INT32_MIN || largs[i] > INT32_MAX) {
				intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
					"intlgregcal_create_instance: at least one of the arguments"
					" has an absolute value that is too large", 0 TSRMLS_CC);
				RETURN_NULL();
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
				"creating ICU GregorianCalendar from date", 0 TSRMLS_CC);
			if (gcal) {
				delete gcal;
			}
			RETURN_NULL();
		}

		timelib_tzinfo *tzinfo = get_timezone_info(TSRMLS_C);
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
				0 TSRMLS_CC);
			delete gcal;
			RETURN_NULL();
		}

		TimeZone *tz = TimeZone::createTimeZone(tzstr);
		gcal->adoptTimeZone(tz);
	}
    
    Calendar_object *co = (Calendar_object*)zend_object_store_get_object(
            return_value TSRMLS_CC);
    co->ucal = gcal;
}

U_CFUNC PHP_FUNCTION(intlgregcal_create_instance)
{
	zval orig;
	intl_error_reset(NULL TSRMLS_CC);

	object_init_ex(return_value, GregorianCalendar_ce_ptr);
	orig = *return_value;

	_php_intlgregcal_constructor_body(INTERNAL_FUNCTION_PARAM_PASSTHRU);

	if (Z_TYPE_P(return_value) == IS_NULL) {
		zend_object_store_ctor_failed(&orig TSRMLS_CC);
		zval_dtor(&orig);
	}
}

U_CFUNC PHP_METHOD(IntlGregorianCalendar, __construct)
{
	zval	orig_this		= *getThis();
	intl_error_reset(NULL TSRMLS_CC);

	return_value = getThis();
	//changes this to IS_NULL (without first destroying) if there's an error
	_php_intlgregcal_constructor_body(INTERNAL_FUNCTION_PARAM_PASSTHRU);

	if (Z_TYPE_P(return_value) == IS_NULL) {
		zend_object_store_ctor_failed(&orig_this TSRMLS_CC);
		zval_dtor(&orig_this);
	}
}

U_CFUNC PHP_FUNCTION(intlgregcal_set_gregorian_change)
{
	double date;
	CALENDAR_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(),
			"Od", &object, GregorianCalendar_ce_ptr, &date) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlgregcal_set_gregorian_change: bad arguments", 0 TSRMLS_CC);
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

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(),
			"O", &object, GregorianCalendar_ce_ptr) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlgregcal_get_gregorian_change: bad arguments", 0 TSRMLS_CC);
		RETURN_FALSE;
	}

	CALENDAR_METHOD_FETCH_OBJECT;
	
	RETURN_DOUBLE((double)fetch_greg(co)->getGregorianChange());
}

U_CFUNC PHP_FUNCTION(intlgregcal_is_leap_year)
{
	long year;
	CALENDAR_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(),
			"Ol", &object, GregorianCalendar_ce_ptr, &year) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlgregcal_is_leap_year: bad arguments", 0 TSRMLS_CC);
		RETURN_FALSE;
	}

	if (year < INT32_MIN || year > INT32_MAX) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlgregcal_is_leap_year: year out of bounds", 0 TSRMLS_CC);
		RETURN_FALSE;
	}

	CALENDAR_METHOD_FETCH_OBJECT;
	
	RETURN_BOOL((int)fetch_greg(co)->isLeapYear((int32_t)year));
}
