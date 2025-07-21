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

using icu::Locale;
using icu::DateFormat;
using icu::GregorianCalendar;
using icu::StringPiece;
using icu::SimpleDateFormat;

static constexpr DateFormat::EStyle valid_styles[] = {
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

static bool valid_format(zval *z) {
	if (Z_TYPE_P(z) == IS_LONG) {
		zend_long lval = Z_LVAL_P(z);
		for (int i = 0; i < sizeof(valid_styles) / sizeof(*valid_styles); i++) {
			if ((zend_long)valid_styles[i] == lval) {
				return true;
			}
		}
	}

	return false;
}

U_CFUNC PHP_FUNCTION(datefmt_format_object)
{
	zend_object			*object;
	zval				*format = NULL;
	char			       *locale_str	= NULL;
	size_t				locale_len;
	bool				pattern		= false;
	UDate				date;
	std::unique_ptr<TimeZone>	timeZone;
	UErrorCode			status		= U_ZERO_ERROR;
	std::unique_ptr<DateFormat>	df;
	std::unique_ptr<Calendar>       cal;
	DateFormat::EStyle	dateStyle = DateFormat::kDefault,
						timeStyle = DateFormat::kDefault;

	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_OBJ(object)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(format)
		Z_PARAM_STRING_OR_NULL(locale_str, locale_len)
	ZEND_PARSE_PARAMETERS_END();

	if (!locale_str) {
		locale_str = (char *)intl_locale_get_default();
	}

	if (format == NULL || Z_TYPE_P(format) == IS_NULL) {
		//nothing
	} else if (Z_TYPE_P(format) == IS_ARRAY) {
		HashTable *ht = Z_ARRVAL_P(format);
		if (zend_hash_num_elements(ht) != 2) {
			intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
					"datefmt_format_object: bad format; if array, it must have "
					"two elements", 0);
			RETURN_FALSE;
		}

		uint32_t idx = 0;
		zval *z;
		ZEND_HASH_FOREACH_VAL(ht, z) {
			if (!valid_format(z)) {
				if (idx == 0) {
					intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
						"datefmt_format_object: bad format; the date format (first "
						"element of the array) is not valid", 0);
				} else {
					ZEND_ASSERT(idx == 1 && "We checked that there are two elements above");
					intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
						"datefmt_format_object: bad format; the time format (second "
						"element of the array) is not valid", 0);
				}
				RETURN_FALSE;
			}
			if (idx == 0) {
				dateStyle = (DateFormat::EStyle)Z_LVAL_P(z);
			} else {
				ZEND_ASSERT(idx == 1 && "We checked that there are two elements above");
				timeStyle = (DateFormat::EStyle)Z_LVAL_P(z);
			}
			idx++;
		} ZEND_HASH_FOREACH_END();
		ZEND_ASSERT(idx == 2 && "We checked that there are two elements above");
	} else if (Z_TYPE_P(format) == IS_LONG) {
		if (!valid_format(format)) {
			intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
					"datefmt_format_object: the date/time format type is invalid",
					0);
			RETURN_FALSE;
		}
		dateStyle = timeStyle = (DateFormat::EStyle)Z_LVAL_P(format);
	} else {
		if (!try_convert_to_string(format)) {
			RETURN_THROWS();
		}
		if (Z_STRLEN_P(format) == 0) {
			intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
					"datefmt_format_object: the format is empty", 0);
			RETURN_FALSE;
		}
		pattern = true;
	}

	//there's no support for relative time in ICU yet
	if (timeStyle != DateFormat::NONE) {
		timeStyle = (DateFormat::EStyle)(timeStyle & ~DateFormat::kRelative);
	}

	zend_class_entry *instance_ce = object->ce;
	if (instanceof_function(instance_ce, Calendar_ce_ptr)) {
		Calendar *obj_cal = calendar_fetch_native_calendar(object);
		if (obj_cal == NULL) {
			intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
					"datefmt_format_object: bad IntlCalendar instance: "
					"not initialized properly", 0);
			RETURN_FALSE;
		}
		timeZone = std::unique_ptr<TimeZone>(obj_cal->getTimeZone().clone());
		date = obj_cal->getTime(status);
		if (U_FAILURE(status)) {
			intl_error_set(NULL, status,
					"datefmt_format_object: error obtaining instant from "
					"IntlCalendar", 0);
			RETURN_FALSE;
		}
		cal = std::unique_ptr<Calendar>(obj_cal->clone());
	} else if (instanceof_function(instance_ce, php_date_get_interface_ce())) {
		TimeZone *tz;
		if (intl_datetime_decompose(object, &date, &tz, NULL,
				"datefmt_format_object") == FAILURE) {
			RETURN_FALSE;
		}
		timeZone = std::unique_ptr<TimeZone>(tz);
		cal = std::unique_ptr<Calendar>(new GregorianCalendar(Locale::createFromName(locale_str), status));
		if (U_FAILURE(status)) {
			intl_error_set(NULL, status,
					"datefmt_format_object: could not create GregorianCalendar",
					0);
			RETURN_FALSE;
		}
	} else {
		intl_error_set(NULL, status, "datefmt_format_object: the passed object "
				"must be an instance of either IntlCalendar or DateTimeInterface",
				0);
		RETURN_FALSE;
	}

	if (pattern) {
		StringPiece sp(Z_STRVAL_P(format));
		df = std::unique_ptr<DateFormat>(new SimpleDateFormat(
			UnicodeString::fromUTF8(sp),
			Locale::createFromName(locale_str),
			status));

		if (U_FAILURE(status)) {
			intl_error_set(NULL, status,
					"datefmt_format_object: could not create SimpleDateFormat",
					0);
			RETURN_FALSE;
		}
	} else {
		df = std::unique_ptr<DateFormat>(DateFormat::createDateTimeInstance(dateStyle, timeStyle,
				Locale::createFromName(locale_str)));

		if (df == NULL) { /* according to ICU sources, this should never happen */
			intl_error_set(NULL, status,
					"datefmt_format_object: could not create DateFormat",
					0);
			RETURN_FALSE;
		}
	}

	//must be in this order (or have the cal adopt the tz)
	df->adoptCalendar(cal.release());
	df->adoptTimeZone(timeZone.release());

	{
		zend_string *u8str;
		UnicodeString result = UnicodeString();
		df->format(date, result);

		u8str = intl_charFromString(result, &status);
		if (!u8str) {
			intl_error_set(NULL, status,
					"datefmt_format_object: error converting result to UTF-8",
					0);
			RETURN_FALSE;
		}
		RETVAL_STR(u8str);
	}
}
