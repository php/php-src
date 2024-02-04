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
#define USE_CALENDAR_POINTER 1
#include "../calendar/calendar_class.h"
#define USE_TIMEZONE_POINTER 1
#include "../timezone/timezone_class.h"
#include "../intl_convert.h"
}

#include "dateformat_helpers.h"
#include "zend_exceptions.h"

#define INTL_UDATE_FMT_OK(i) \
	(UDAT_FULL == (i) || UDAT_LONG == (i) ||    \
	 UDAT_MEDIUM == (i) || UDAT_SHORT == (i) || \
	 UDAT_RELATIVE == (i) || UDAT_FULL_RELATIVE == (i) || \
	 UDAT_LONG_RELATIVE == (i) || UDAT_MEDIUM_RELATIVE == (i) || \
	 UDAT_SHORT_RELATIVE == (i) || UDAT_NONE == (i) || \
	 UDAT_PATTERN == (i))

/* {{{ */
static zend_result datefmt_ctor(INTERNAL_FUNCTION_PARAMETERS, zend_error_handling *error_handling, bool *error_handling_replaced)
{
	zval		*object;
	char	*locale_str;
	size_t		locale_len	= 0;
	Locale		locale;
	zend_long	date_type = UDAT_FULL;
	zend_long	time_type = UDAT_FULL;
	zend_object *calendar_obj = NULL;
	zend_long calendar_long = 0;
	bool calendar_is_null = 1;
	Calendar *cal = NULL;
	zend_long	calendar_type;
	bool		calendar_owned;
	zval		*timezone_zv	= NULL;
	TimeZone	*timezone	= NULL;
	bool		explicit_tz;
	char*       pattern_str		= NULL;
	size_t      pattern_str_len	= 0;
	UChar*      svalue		= NULL;		/* UTF-16 pattern_str */
	int32_t     slength		= 0;
	IntlDateFormatter_object* dfo;

	intl_error_reset(NULL);
	object = return_value;

	ZEND_PARSE_PARAMETERS_START(1, 6)
		Z_PARAM_STRING_OR_NULL(locale_str, locale_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(date_type)
		Z_PARAM_LONG(time_type)
		Z_PARAM_ZVAL(timezone_zv)
		Z_PARAM_OBJ_OF_CLASS_OR_LONG_OR_NULL(calendar_obj, Calendar_ce_ptr, calendar_long, calendar_is_null)
		Z_PARAM_STRING_OR_NULL(pattern_str, pattern_str_len)
	ZEND_PARSE_PARAMETERS_END_EX(return FAILURE);

	if (error_handling != NULL) {
		zend_replace_error_handling(EH_THROW, IntlException_ce_ptr, error_handling);
		*error_handling_replaced = 1;
	}

	DATE_FORMAT_METHOD_FETCH_OBJECT_NO_CHECK;

	if (DATE_FORMAT_OBJECT(dfo) != NULL) {
		intl_errors_set(INTL_DATA_ERROR_P(dfo), U_ILLEGAL_ARGUMENT_ERROR, "datefmt_create: cannot call constructor twice", 0);
		return FAILURE;
	}

	if (!INTL_UDATE_FMT_OK(date_type)) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR, "datefmt_create: invalid date format style", 0);
		return FAILURE;
	}
	if (!INTL_UDATE_FMT_OK(time_type)) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR, "datefmt_create: invalid time format style", 0);
		return FAILURE;
	}
	if (date_type == UDAT_PATTERN && time_type != UDAT_PATTERN) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR, "datefmt_create: time format must be UDAT_PATTERN if date format is UDAT_PATTERN", 0);
		return FAILURE;
	}

	INTL_CHECK_LOCALE_LEN_OR_FAILURE(locale_len);
	if (locale_len == 0) {
		locale_str = (char *) intl_locale_get_default();
	}
	locale = Locale::createFromName(locale_str);
	/* get*Name accessors being set does not preclude being bogus */
	if (locale.isBogus() || ((locale_len == 1 && locale_str[0] != 'C') || (locale_len > 1 && strlen(locale.getISO3Language()) == 0))) {
        zend_argument_value_error(1, "\"%s\" is invalid", locale_str);
		return FAILURE;
	}

	/* process calendar */
	if (datefmt_process_calendar_arg(calendar_obj, calendar_long, calendar_is_null, locale, "datefmt_create",
		INTL_DATA_ERROR_P(dfo), cal, calendar_type, calendar_owned) == FAILURE
	) {
		goto error;
	}

	/* process timezone */
	explicit_tz = timezone_zv != NULL && Z_TYPE_P(timezone_zv) != IS_NULL;

	if (explicit_tz || calendar_owned ) {
		//we have an explicit time zone or a non-object calendar
		timezone = timezone_process_timezone_argument(timezone_zv,
				INTL_DATA_ERROR_P(dfo), "datefmt_create");
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
					"error converting pattern to UTF-16", 0);
			goto error;
		}
	}

	DATE_FORMAT_OBJECT(dfo) = udat_open((UDateFormatStyle)time_type,
			(UDateFormatStyle)date_type, locale_str, NULL, 0, svalue,
			slength, &INTL_DATA_ERROR_CODE(dfo));

	if (pattern_str && pattern_str_len > 0) {
		udat_applyPattern(DATE_FORMAT_OBJECT(dfo), true, svalue, slength);
		if (U_FAILURE(INTL_DATA_ERROR_CODE(dfo))) {
			intl_error_set(NULL, INTL_DATA_ERROR_CODE(dfo), "datefmt_create: error applying pattern", 0);
			goto error;
		}
	}

	if (!U_FAILURE(INTL_DATA_ERROR_CODE(dfo))) {
		DateFormat *df = (DateFormat*)DATE_FORMAT_OBJECT(dfo);
		if (calendar_owned) {
			df->adoptCalendar(cal);
			calendar_owned = false;
		} else {
			df->setCalendar(*cal);
		}

		if (timezone != NULL) {
			df->adoptTimeZone(timezone);
		}
	} else {
		intl_error_set(NULL, INTL_DATA_ERROR_CODE(dfo),	"datefmt_create: date "
				"formatter creation failed", 0);
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
	if (cal != NULL && calendar_owned) {
		delete cal;
	}

	return U_FAILURE(intl_error_get_code(NULL)) ? FAILURE : SUCCESS;
}
/* }}} */

/* {{{ Create formatter. */
U_CFUNC PHP_FUNCTION( datefmt_create )
{
    object_init_ex( return_value, IntlDateFormatter_ce_ptr );
    if (datefmt_ctor(INTERNAL_FUNCTION_PARAM_PASSTHRU, NULL, NULL) == FAILURE) {
		zval_ptr_dtor(return_value);
		RETURN_NULL();
	}
}
/* }}} */

/* {{{ IntlDateFormatter object constructor. */
U_CFUNC PHP_METHOD( IntlDateFormatter, __construct )
{
	zend_error_handling error_handling;
	bool error_handling_replaced = 0;

	/* return_value param is being changed, therefore we will always return
	 * NULL here */
	return_value = ZEND_THIS;
	if (datefmt_ctor(INTERNAL_FUNCTION_PARAM_PASSTHRU, &error_handling, &error_handling_replaced) == FAILURE) {
		if (!EG(exception)) {
			zend_string *err = intl_error_get_message(NULL);
			zend_throw_exception(IntlException_ce_ptr, ZSTR_VAL(err), intl_error_get_code(NULL));
			zend_string_release_ex(err, 0);
		}
	}
	if (error_handling_replaced) {
		zend_restore_error_handling(&error_handling);
	}
}
/* }}} */
