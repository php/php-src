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
   | Authors: Kirti Velankar <kirtig@yahoo-inc.com>                       |
   |          Gustavo Lopes <cataphract@php.net>                          |
   +----------------------------------------------------------------------+
*/

#include "../intl_cppshims.h"

#include <unicode/timezone.h>
#include <unicode/calendar.h>
#include <unicode/datefmt.h>

extern "C" {
#include <unicode/ustring.h>
#include <unicode/udat.h>

#include "php_intl.h"
#include "dateformat_create.h"
#include "dateformat_class.h"
#define USE_TIMEZONE_POINTER 1
#include "../timezone/timezone_class.h"
#include "../intl_convert.h"
}

#include "dateformat_helpers.h"

/* {{{ */
static void datefmt_ctor(INTERNAL_FUNCTION_PARAMETERS)
{
	zval		*object;

	const char	*locale_str;
	int			locale_len		= 0;
	Locale		locale;
    long		date_type		= 0;
    long		time_type		= 0;
	zval		*calendar_zv	= NULL;
	Calendar	*calendar		= NULL;
	long		calendar_type;
	bool		calendar_owned;
	zval		**timezone_zv	= NULL;
	TimeZone	*timezone		= NULL;
	bool		explicit_tz;
    char*       pattern_str		= NULL;
    int         pattern_str_len	= 0;
    UChar*      svalue			= NULL;		/* UTF-16 pattern_str */
    int         slength			= 0;
	IntlDateFormatter_object* dfo;

	intl_error_reset(NULL TSRMLS_CC);
	object = return_value;
	/* Parse parameters. */
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sll|Zzs",
			&locale_str, &locale_len, &date_type, &time_type, &timezone_zv,
			&calendar_zv, &pattern_str, &pattern_str_len) == FAILURE) {
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,	"datefmt_create: "
				"unable to parse input parameters", 0 TSRMLS_CC);
		zval_dtor(return_value);
		RETURN_NULL();
    }

	INTL_CHECK_LOCALE_LEN_OBJ(locale_len, return_value);
	if (locale_len == 0) {
		locale_str = intl_locale_get_default(TSRMLS_C);
	}
	locale = Locale::createFromName(locale_str);

	DATE_FORMAT_METHOD_FETCH_OBJECT_NO_CHECK;

	if (DATE_FORMAT_OBJECT(dfo) != NULL) {
		intl_errors_set(INTL_DATA_ERROR_P(dfo), U_ILLEGAL_ARGUMENT_ERROR,
				"datefmt_create: cannot call constructor twice", 0 TSRMLS_CC);
		return;
	}

	/* process calendar */
	if (datefmt_process_calendar_arg(calendar_zv, locale, "datefmt_create",
			INTL_DATA_ERROR_P(dfo), calendar, calendar_type,
			calendar_owned TSRMLS_CC)
			== FAILURE) {
		goto error;
	}

	/* process timezone */
	explicit_tz = timezone_zv != NULL && Z_TYPE_PP(timezone_zv) != IS_NULL;

	if (explicit_tz || calendar_owned ) {
		//we have an explicit time zone or a non-object calendar
		timezone = timezone_process_timezone_argument(timezone_zv,
				INTL_DATA_ERROR_P(dfo), "datefmt_create" TSRMLS_CC);
		if (timezone == NULL) {
			goto error;
		}
	}

	/* Convert pattern (if specified) to UTF-16. */
	if (pattern_str && pattern_str_len > 0) {
		intl_convert_utf8_to_utf16(&svalue, &slength,
				pattern_str, pattern_str_len, &INTL_DATA_ERROR_CODE(dfo));
		if (U_FAILURE(INTL_DATA_ERROR_CODE(dfo))) {
			/* object construction -> only set global error */
			intl_error_set(NULL, INTL_DATA_ERROR_CODE(dfo), "datefmt_create: "
					"error converting pattern to UTF-16", 0 TSRMLS_CC);
			goto error;
		}
	}

	if (pattern_str && pattern_str_len > 0) {
		DATE_FORMAT_OBJECT(dfo) = udat_open(UDAT_IGNORE, UDAT_IGNORE,
				locale_str, NULL, 0, svalue, slength,
				&INTL_DATA_ERROR_CODE(dfo));
	} else {
		DATE_FORMAT_OBJECT(dfo) = udat_open((UDateFormatStyle)time_type,
				(UDateFormatStyle)date_type, locale_str, NULL, 0, svalue,
				slength, &INTL_DATA_ERROR_CODE(dfo));
	}

    if (!U_FAILURE(INTL_DATA_ERROR_CODE(dfo))) {
		DateFormat *df = (DateFormat*)DATE_FORMAT_OBJECT(dfo);
		if (calendar_owned) {
			df->adoptCalendar(calendar);
			calendar_owned = false;
		} else {
			df->setCalendar(*calendar);
		}

		if (timezone != NULL) {
			df->adoptTimeZone(timezone);
		}
    } else {
		intl_error_set(NULL, INTL_DATA_ERROR_CODE(dfo),	"datefmt_create: date "
				"formatter creation failed", 0 TSRMLS_CC);
		goto error;
	}

	/* Set the class variables */
	dfo->date_type			= date_type;
	dfo->time_type			= time_type;
	dfo->calendar			= calendar_type;
	dfo->requested_locale	= estrdup(locale_str);

error:
	if (svalue) {
		efree(svalue);
	}
	if (timezone != NULL && DATE_FORMAT_OBJECT(dfo) == NULL) {
		delete timezone;
	}
	if (calendar != NULL && calendar_owned) {
		delete calendar;
	}
	if (U_FAILURE(intl_error_get_code(NULL TSRMLS_CC))) {
		/* free_object handles partially constructed instances fine */
		zval_dtor(return_value);
		RETVAL_NULL();
	}
}
/* }}} */

/* {{{ proto IntlDateFormatter IntlDateFormatter::create(string $locale, long date_type, long time_type[, string $timezone_str, long $calendar, string $pattern] )
 * Create formatter. }}} */
/* {{{ proto IntlDateFormatter datefmt_create(string $locale, long date_type, long time_type[, string $timezone_str, long $calendar, string $pattern)
 * Create formatter.
 */
U_CFUNC PHP_FUNCTION( datefmt_create )
{
    object_init_ex( return_value, IntlDateFormatter_ce_ptr );
	datefmt_ctor(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto void IntlDateFormatter::__construct(string $locale, long date_type, long time_type[, string $timezone_str, long $calendar, string $pattern])
 * IntlDateFormatter object constructor.
 */
U_CFUNC PHP_METHOD( IntlDateFormatter, __construct )
{
	/* return_value param is being changed, therefore we will always return
	 * NULL here */
	return_value = getThis();
	datefmt_ctor(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */
