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

#include "../intl_cppshims.h"

#include <unicode/calendar.h>
#include <unicode/gregocal.h>
#include <unicode/datefmt.h>
#include <unicode/smpdtfmt.h>
#include <unicode/locid.h>

#include "../intl_convertcpp.h"

extern "C" {
#include "../php_intl.h"
#include "../locale/locale.h"
#define USE_CALENDAR_POINTER 1
#include "../calendar/calendar_class.h"
#include <ext/date/php_date.h>
#include "../common/common_date.h"
}

static const DateFormat::EStyle valid_styles[] = {
		DateFormat::kNone,
		DateFormat::kFull,
		DateFormat::kLong,
		DateFormat::kMedium,
		DateFormat::kShort,
		DateFormat::kFullRelative,
		DateFormat::kLongRelative,
		DateFormat::kMediumRelative,
		DateFormat::kShortRelative,
};

static bool valid_format(zval **z) {
	if (Z_TYPE_PP(z) == IS_LONG) {
		long lval = Z_LVAL_PP(z);
		for (int i = 0; i < sizeof(valid_styles) / sizeof(*valid_styles); i++) {
			if ((long)valid_styles[i] == lval) {
				return true;
			}
		}
	}

	return false;
}

U_CFUNC PHP_FUNCTION(datefmt_format_object)
{
	zval				*object,
						**format = NULL;
	const char			*locale_str	= NULL;
	int					locale_len;
	bool				pattern		= false;
	UDate				date;
	TimeZone			*timeZone	= NULL;
	UErrorCode			status		= U_ZERO_ERROR;
	DateFormat			*df			= NULL;
	Calendar			*cal		= NULL;
	DateFormat::EStyle	dateStyle = DateFormat::kDefault,
						timeStyle = DateFormat::kDefault;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o|Zs!",
			&object, &format, &locale_str, &locale_len) == FAILURE) {
		RETURN_FALSE;
	}

	if (!locale_str) {
		locale_str = intl_locale_get_default(TSRMLS_C);
	}

	if (format == NULL || Z_TYPE_PP(format) == IS_NULL) {
		//nothing
	} else if (Z_TYPE_PP(format) == IS_ARRAY) {
		HashTable		*ht	= Z_ARRVAL_PP(format);
		HashPosition	pos	= {0};
		zval			**z;
		if (zend_hash_num_elements(ht) != 2) {
			intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
					"datefmt_format_object: bad format; if array, it must have "
					"two elements", 0 TSRMLS_CC);
			RETURN_FALSE;
		}

		zend_hash_internal_pointer_reset_ex(ht, &pos);
		zend_hash_get_current_data_ex(ht, (void**)&z, &pos);
		if (!valid_format(z)) {
			intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
					"datefmt_format_object: bad format; the date format (first "
					"element of the array) is not valid", 0 TSRMLS_CC);
			RETURN_FALSE;
		}
		dateStyle = (DateFormat::EStyle)Z_LVAL_PP(z);

		zend_hash_move_forward_ex(ht, &pos);
		zend_hash_get_current_data_ex(ht, (void**)&z, &pos);
		if (!valid_format(z)) {
			intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
					"datefmt_format_object: bad format; the time format ("
					"second element of the array) is not valid", 0 TSRMLS_CC);
			RETURN_FALSE;
		}
		timeStyle = (DateFormat::EStyle)Z_LVAL_PP(z);
	} else if (Z_TYPE_PP(format) == IS_LONG) {
		if (!valid_format(format)) {
			intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
					"datefmt_format_object: the date/time format type is invalid",
					0 TSRMLS_CC);
			RETURN_FALSE;
		}
		dateStyle = timeStyle = (DateFormat::EStyle)Z_LVAL_PP(format);
	} else {
		convert_to_string_ex(format);
		if (Z_STRLEN_PP(format) == 0) {
			intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
					"datefmt_format_object: the format is empty", 0 TSRMLS_CC);
			RETURN_FALSE;
		}
		pattern = true;
	}

	//there's no support for relative time in ICU yet
	timeStyle = (DateFormat::EStyle)(timeStyle & ~DateFormat::kRelative);

	zend_class_entry *instance_ce = Z_OBJCE_P(object);
	if (instanceof_function(instance_ce, Calendar_ce_ptr TSRMLS_CC)) {
		Calendar *obj_cal = calendar_fetch_native_calendar(object TSRMLS_CC);
		if (obj_cal == NULL) {
			intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
					"datefmt_format_object: bad IntlCalendar instance: "
					"not initialized properly", 0 TSRMLS_CC);
			RETURN_FALSE;
		}
		timeZone = obj_cal->getTimeZone().clone();
		date = obj_cal->getTime(status);
		if (U_FAILURE(status)) {
			intl_error_set(NULL, status,
					"datefmt_format_object: error obtaining instant from "
					"IntlCalendar", 0 TSRMLS_CC);
			RETVAL_FALSE;
			goto cleanup;
		}
		cal = obj_cal->clone();
	} else if (instanceof_function(instance_ce, php_date_get_date_ce() TSRMLS_CC)) {
		if (intl_datetime_decompose(object, &date, &timeZone, NULL,
				"datefmt_format_object" TSRMLS_CC) == FAILURE) {
			RETURN_FALSE;
		}
		cal = new GregorianCalendar(Locale::createFromName(locale_str), status);
		if (U_FAILURE(status)) {
			intl_error_set(NULL, status,
					"datefmt_format_object: could not create GregorianCalendar",
					0 TSRMLS_CC);
			RETVAL_FALSE;
			goto cleanup;
		}
	} else {
		intl_error_set(NULL, status, "datefmt_format_object: the passed object "
				"must be an instance of either IntlCalendar or DateTime",
				0 TSRMLS_CC);
		RETURN_FALSE;
	}

	if (pattern) {
		 df = new SimpleDateFormat(
				UnicodeString(Z_STRVAL_PP(format), Z_STRLEN_PP(format),
						UnicodeString::kInvariant),
				Locale::createFromName(locale_str),
				status);

		if (U_FAILURE(status)) {
			intl_error_set(NULL, status,
					"datefmt_format_object: could not create SimpleDateFormat",
					0 TSRMLS_CC);
			RETVAL_FALSE;
			goto cleanup;
		}
	} else {
		df = DateFormat::createDateTimeInstance(dateStyle, timeStyle,
				Locale::createFromName(locale_str));

		if (df == NULL) { /* according to ICU sources, this should never happen */
			intl_error_set(NULL, status,
					"datefmt_format_object: could not create DateFormat",
					0 TSRMLS_CC);
			RETVAL_FALSE;
			goto cleanup;
		}
	}

	//must be in this order (or have the cal adopt the tz)
	df->adoptCalendar(cal);
	cal = NULL;
	df->adoptTimeZone(timeZone);
	timeZone = NULL;

	{
		UnicodeString result = UnicodeString();
		df->format(date, result);

		Z_TYPE_P(return_value) = IS_STRING;
		if (intl_charFromString(result, &Z_STRVAL_P(return_value),
				&Z_STRLEN_P(return_value), &status) == FAILURE) {
			intl_error_set(NULL, status,
					"datefmt_format_object: error converting result to UTF-8",
					0 TSRMLS_CC);
			RETVAL_FALSE;
			goto cleanup;
		}
	}


cleanup:
	delete df;
	delete timeZone;
	delete cal;
}
