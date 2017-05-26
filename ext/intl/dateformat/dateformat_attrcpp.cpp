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

#include "../intl_cppshims.h"

#include <unicode/timezone.h>
#include <unicode/calendar.h>
#include <unicode/datefmt.h>

extern "C" {
#include "../php_intl.h"
#include "dateformat_class.h"
#include "dateformat_attrcpp.h"
#define USE_TIMEZONE_POINTER 1
#include "../timezone/timezone_class.h"
#define USE_CALENDAR_POINTER 1
#include "../calendar/calendar_class.h"
}

#include "../intl_convertcpp.h"
#include "dateformat_helpers.h"

static inline DateFormat *fetch_datefmt(IntlDateFormatter_object *dfo) {
	return (DateFormat *)dfo->datef_data.udatf;
}

/* {{{ proto string IntlDateFormatter::getTimeZoneId()
 * Get formatter timezone_id. }}} */
/* {{{ proto string datefmt_get_timezone_id(IntlDateFormatter $mf)
 * Get formatter timezone_id.
 */
U_CFUNC PHP_FUNCTION(datefmt_get_timezone_id)
{
	zend_string *u8str;
	DATE_FORMAT_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O",
			&object, IntlDateFormatter_ce_ptr ) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,	"datefmt_get_timezone_"
				"id: unable to parse input params", 0);
		RETURN_FALSE;
	}

	DATE_FORMAT_METHOD_FETCH_OBJECT;

	UnicodeString res = UnicodeString();
	fetch_datefmt(dfo)->getTimeZone().getID(res);
	u8str = intl_charFromString(res, &INTL_DATA_ERROR_CODE(dfo));
	INTL_METHOD_CHECK_STATUS(dfo, "Could not convert time zone id to UTF-8");

	RETVAL_STR(u8str);
}

/* {{{ proto IntlTimeZone IntlDateFormatter::getTimeZone()
 * Get formatter timezone. }}} */
/* {{{ proto IntlTimeZone datefmt_get_timezone(IntlDateFormatter $mf)
 * Get formatter timezone.
 */
U_CFUNC PHP_FUNCTION(datefmt_get_timezone)
{
	DATE_FORMAT_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O",
			&object, IntlDateFormatter_ce_ptr ) == FAILURE) {
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"datefmt_get_timezone: unable to parse input params", 0 );
		RETURN_FALSE;
	}

	DATE_FORMAT_METHOD_FETCH_OBJECT;

	const TimeZone& tz = fetch_datefmt(dfo)->getTimeZone();
	TimeZone *tz_clone = tz.clone();
	if (tz_clone == NULL) {
		intl_errors_set(INTL_DATA_ERROR_P(dfo), U_MEMORY_ALLOCATION_ERROR,
				"datefmt_get_timezone: Out of memory when cloning time zone",
				0);
		RETURN_FALSE;
	}

	timezone_object_construct(tz_clone, return_value, 1);
}

/* {{{ proto boolean IntlDateFormatter::setTimeZone(mixed $timezone)
 * Set formatter's timezone. */
U_CFUNC PHP_FUNCTION(datefmt_set_timezone)
{
	zval		*timezone_zv;
	TimeZone	*timezone;

	DATE_FORMAT_METHOD_INIT_VARS;

	if ( zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(),
			"Oz", &object, IntlDateFormatter_ce_ptr, &timezone_zv) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR, "datefmt_set_timezone: "
				"unable to parse input params", 0);
		RETURN_FALSE;
	}

	DATE_FORMAT_METHOD_FETCH_OBJECT;

	timezone = timezone_process_timezone_argument(timezone_zv,
			INTL_DATA_ERROR_P(dfo), "datefmt_set_timezone");
	if (timezone == NULL) {
		RETURN_FALSE;
	}

	fetch_datefmt(dfo)->adoptTimeZone(timezone);
}

/* {{{ proto int IntlDateFormatter::getCalendar( )
 * Get formatter calendar type. }}} */
/* {{{ proto int datefmt_get_calendar(IntlDateFormatter $mf)
 * Get formatter calendar type.
 */
U_CFUNC PHP_FUNCTION(datefmt_get_calendar)
{
	DATE_FORMAT_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O",
			&object, IntlDateFormatter_ce_ptr ) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"datefmt_get_calendar: unable to parse input params", 0);
		RETURN_FALSE;
	}

	DATE_FORMAT_METHOD_FETCH_OBJECT;

	if (dfo->calendar == -1) {
		/* an IntlCalendar was provided to the constructor */
		RETURN_FALSE;
	}

	RETURN_LONG(dfo->calendar);
}
/* }}} */

/* {{{ proto IntlCalendar IntlDateFormatter::getCalendarObject()
 * Get formatter calendar. }}} */
/* {{{ proto IntlCalendar datefmt_get_calendar_object(IntlDateFormatter $mf)
 * Get formatter calendar.
 */
U_CFUNC PHP_FUNCTION(datefmt_get_calendar_object)
{
	DATE_FORMAT_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O",
			&object, IntlDateFormatter_ce_ptr ) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
				"datefmt_get_calendar_object: unable to parse input params",
				0);
		RETURN_FALSE;
	}

	DATE_FORMAT_METHOD_FETCH_OBJECT;

	const Calendar *cal = fetch_datefmt(dfo)->getCalendar();
	if (cal == NULL) {
		RETURN_NULL();
	}

	Calendar *cal_clone = cal->clone();
	if (cal_clone == NULL) {
		intl_errors_set(INTL_DATA_ERROR_P(dfo), U_MEMORY_ALLOCATION_ERROR,
				"datefmt_get_calendar_object: Out of memory when cloning "
				"calendar", 0);
		RETURN_FALSE;
	}

	calendar_object_create(return_value, cal_clone);
}
/* }}} */

/* {{{ proto bool IntlDateFormatter::setCalendar(mixed $calendar)
 * Set formatter's calendar. }}} */
/* {{{ proto bool datefmt_set_calendar(IntlDateFormatter $mf, mixed $calendar)
 * Set formatter's calendar.
 */
U_CFUNC PHP_FUNCTION(datefmt_set_calendar)
{
	zval	*calendar_zv;
	DATE_FORMAT_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oz",
			&object, IntlDateFormatter_ce_ptr, &calendar_zv) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"datefmt_set_calendar: unable to parse input params", 0);
		RETURN_FALSE;
	}

	DATE_FORMAT_METHOD_FETCH_OBJECT;

	Calendar	*cal;
	zend_long		cal_type;
	bool		cal_owned;
	Locale		locale = Locale::createFromName(dfo->requested_locale);
	// getting the actual locale from the DateFormat is not enough
	// because we would have lost modifiers such as @calendar. We
	// must store the requested locale on object creation

	if (datefmt_process_calendar_arg(calendar_zv, locale,
			"datefmt_set_calendar",	INTL_DATA_ERROR_P(dfo), cal, cal_type,
			cal_owned) == FAILURE) {
		RETURN_FALSE;
	}

	if (cal_owned) {
		/* a non IntlCalendar was specified, we want to keep the timezone */
		TimeZone *old_timezone = fetch_datefmt(dfo)->getTimeZone().clone();
		if (old_timezone == NULL) {
			intl_errors_set(INTL_DATA_ERROR_P(dfo), U_MEMORY_ALLOCATION_ERROR,
					"datefmt_set_calendar: Out of memory when cloning calendar",
					0);
			delete cal;
			RETURN_FALSE;
		}
		cal->adoptTimeZone(old_timezone);
	} else {
		cal = cal->clone();
		if (cal == NULL) {
			intl_errors_set(INTL_DATA_ERROR_P(dfo), U_MEMORY_ALLOCATION_ERROR,
					"datefmt_set_calendar: Out of memory when cloning calendar",
					0);
			RETURN_FALSE;
		}
	}

	fetch_datefmt(dfo)->adoptCalendar(cal);

	dfo->calendar = cal_type;

	RETURN_TRUE;
}
/* }}} */

