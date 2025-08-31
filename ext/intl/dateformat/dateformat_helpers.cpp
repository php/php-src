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

#include "../intl_cppshims.h"

#include <unicode/calendar.h>
#include <unicode/gregocal.h>

#include "dateformat_helpers.h"

extern "C" {
#include "../php_intl.h"
#include <Zend/zend_operators.h>
#define USE_CALENDAR_POINTER 1
#include "../calendar/calendar_class.h"
}

using icu::GregorianCalendar;

zend_result datefmt_process_calendar_arg(
	zend_object *calendar_obj, zend_long calendar_long, bool calendar_is_null, Locale const& locale,
	intl_error *err, Calendar*& cal, zend_long& cal_int_type, bool& calendar_owned
) {
	UErrorCode status = UErrorCode();

	if (calendar_is_null) {
		// default requested
		cal = new GregorianCalendar(locale, status);
		calendar_owned = true;

		cal_int_type = UCAL_GREGORIAN;

	} else if (!calendar_obj) {
		zend_long v = calendar_long;
		if (v != (zend_long)UCAL_TRADITIONAL && v != (zend_long)UCAL_GREGORIAN) {
			intl_errors_set(err, U_ILLEGAL_ARGUMENT_ERROR,
				"Invalid value for calendar type; it must be one of "
				"IntlDateFormatter::TRADITIONAL (locale's default calendar) or"
				" IntlDateFormatter::GREGORIAN. Alternatively, it can be an "
				"IntlCalendar object");
			return FAILURE;
		} else if (v == (zend_long)UCAL_TRADITIONAL) {
			cal = Calendar::createInstance(locale, status);
		} else { //UCAL_GREGORIAN
			cal = new GregorianCalendar(locale, status);
		}
		calendar_owned = true;

		cal_int_type = calendar_long;

	} else if (calendar_obj) {
		cal = calendar_fetch_native_calendar(calendar_obj);
		if (cal == NULL) {
			intl_errors_set(err, U_ILLEGAL_ARGUMENT_ERROR, "Found unconstructed IntlCalendar object");
			return FAILURE;
		}
		calendar_owned = false;

		cal_int_type = -1;

	} else {
		intl_errors_set(err, U_ILLEGAL_ARGUMENT_ERROR,
			"Invalid calendar argument; should be an integer or an IntlCalendar instance");
		return FAILURE;
	}

	if (cal == NULL && !U_FAILURE(status)) {
		status = U_MEMORY_ALLOCATION_ERROR;
	}
	if (U_FAILURE(status)) {
		intl_errors_set(err, U_ILLEGAL_ARGUMENT_ERROR, "Failure instantiating calendar");
		return FAILURE;
	}

	return SUCCESS;
}
