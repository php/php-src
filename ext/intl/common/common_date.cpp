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

extern "C" {
#include "../php_intl.h"
#define USE_CALENDAR_POINTER 1
#include "../calendar/calendar_class.h"
#include <ext/date/php_date.h>
}

using icu::TimeZone;
using icu::UnicodeString;

#include "zend_portability.h"

/* {{{ timezone_convert_datetimezone
 *      The timezone in DateTime and DateTimeZone is not unified. */
U_CFUNC TimeZone *timezone_convert_datetimezone(
	int type, void *object, bool is_datetime,
	intl_error *outside_error)
{
	char		*id = NULL,
				offset_id[] = "GMT+00:00";
	int32_t		id_len = 0;
	char		*message;
	TimeZone	*timeZone;

	switch (type) {
		case TIMELIB_ZONETYPE_ID:
			id = is_datetime
				? ((php_date_obj*)object)->time->tz_info->name
				: ((php_timezone_obj*)object)->tzi.tz->name;
			id_len = strlen(id);
			break;
		case TIMELIB_ZONETYPE_OFFSET: {
			int offset_mins = is_datetime
				? ((php_date_obj*)object)->time->z / 60
				: (int)((php_timezone_obj*)object)->tzi.utc_offset / 60,
				hours = offset_mins / 60,
				minutes = offset_mins - hours * 60;
			minutes *= minutes > 0 ? 1 : -1;

			if (offset_mins <= -24 * 60 || offset_mins >= 24 * 60) {
				intl_errors_set(outside_error, U_ILLEGAL_ARGUMENT_ERROR,
					"object has an time zone offset that's too large");
				return NULL;
			}

			id = offset_id;
			id_len = slprintf(id, sizeof(offset_id), "GMT%+03d:%02d",
				hours, minutes);
			break;
		}
		case TIMELIB_ZONETYPE_ABBR:
			id = is_datetime
				? ((php_date_obj*)object)->time->tz_abbr
				: ((php_timezone_obj*)object)->tzi.z.abbr;
			id_len = strlen(id);
			break;
	}

	UnicodeString s = UnicodeString(id, id_len, US_INV);
	timeZone = TimeZone::createTimeZone(s);
	if (*timeZone == TimeZone::getUnknown()) {
		spprintf(&message, 0, "time zone id '%s' "
			"extracted from ext/date DateTimeZone not recognized", id);
		intl_errors_set(outside_error, U_ILLEGAL_ARGUMENT_ERROR, message);
		efree(message);
		delete timeZone;
		return NULL;
	}
	return timeZone;
}
/* }}} */

U_CFUNC zend_result intl_datetime_decompose(zend_object *obj, double *millis, TimeZone **tz,
		intl_error *err)
{
	char	*message;
	php_date_obj *datetime = php_date_obj_from_obj(obj);

	if (err && U_FAILURE(err->code)) {
		return FAILURE;
	}

	if (millis) {
		*millis = ZEND_NAN;
	}
	if (tz) {
		*tz = nullptr;
	}

	if (millis) {
		auto getTimestampMethod = static_cast<zend_function *>(zend_hash_str_find_ptr(&obj->ce->function_table, ZEND_STRL("gettimestamp")));
		zval retval;

		ZEND_ASSERT(getTimestampMethod && "DateTimeInterface is sealed and thus must have this method");
		zend_call_known_function(getTimestampMethod, obj, obj->ce, &retval, 0, nullptr, nullptr);

		/* An exception has occurred */
		if (Z_TYPE(retval) == IS_UNDEF) {
			return FAILURE;
		}
		// TODO: Remove this when DateTimeInterface::getTimestamp() no longer has a tentative return type
		if (Z_TYPE(retval) != IS_LONG) {
			zval_ptr_dtor(&retval);
			spprintf(&message, 0, "%s::getTimestamp() did not return an int", ZSTR_VAL(obj->ce->name));
			intl_errors_set(err, U_INTERNAL_PROGRAM_ERROR, message);
			efree(message);
			return FAILURE;
		}

		*millis = U_MILLIS_PER_SECOND * (double)Z_LVAL(retval) + (datetime->time->us / 1000);
	}

	if (tz) {
		if (!datetime->time) {
			spprintf(&message, 0, "the %s object is not properly "
					"initialized", ZSTR_VAL(obj->ce->name));
			intl_errors_set(err, U_ILLEGAL_ARGUMENT_ERROR, message);
			efree(message);
			return FAILURE;
		}
		if (!datetime->time->is_localtime) {
			*tz = TimeZone::getGMT()->clone();
		} else {
			*tz = timezone_convert_datetimezone(datetime->time->zone_type,
				datetime, 1, NULL);
			if (*tz == NULL) {
				intl_errors_set(err, U_ILLEGAL_ARGUMENT_ERROR,
					"could not convert DateTime's time zone");
				return FAILURE;
			}
		}
	}

	return SUCCESS;
}

U_CFUNC double intl_zval_to_millis(zval *z, intl_error *err)
{
	double	rv = ZEND_NAN;
	zend_long	lv;
	int		type;

	if (err && U_FAILURE(err->code)) {
		return ZEND_NAN;
	}

try_again:
	switch (Z_TYPE_P(z)) {
	case IS_STRING:
		type = is_numeric_string(Z_STRVAL_P(z), Z_STRLEN_P(z), &lv, &rv, 0);
		if (type == IS_DOUBLE) {
			rv *= U_MILLIS_PER_SECOND;
		} else if (type == IS_LONG) {
			rv = U_MILLIS_PER_SECOND * (double)lv;
		} else {
			char *message;
			spprintf(&message, 0, "string '%s' is not numeric, "
					"which would be required for it to be a valid date",
					Z_STRVAL_P(z));
			intl_errors_set(err, U_ILLEGAL_ARGUMENT_ERROR,
				message);
			efree(message);
		}
		break;
	case IS_LONG:
		rv = U_MILLIS_PER_SECOND * (double)Z_LVAL_P(z);
		break;
	case IS_DOUBLE:
		rv = U_MILLIS_PER_SECOND * Z_DVAL_P(z);
		break;
	case IS_OBJECT:
		if (instanceof_function(Z_OBJCE_P(z), php_date_get_interface_ce())) {
			intl_datetime_decompose(Z_OBJ_P(z), &rv, nullptr, err);
		} else if (instanceof_function(Z_OBJCE_P(z), Calendar_ce_ptr)) {
			Calendar_object *co = Z_INTL_CALENDAR_P(z);
			if (co->ucal == NULL) {
				intl_errors_set(err, U_ILLEGAL_ARGUMENT_ERROR,
					"IntlCalendar object is not properly constructed");
			} else {
				UErrorCode status = UErrorCode();
				rv = (double)co->ucal->getTime(status);
				if (U_FAILURE(status)) {
					intl_errors_set(err, status, "call to internal Calendar::getTime() has failed");
				}
			}
		} else {
			/* TODO: try with cast(), get() to obtain a number */
			intl_errors_set(err, U_ILLEGAL_ARGUMENT_ERROR,
				"invalid object type for date/time "
				"(only IntlCalendar and DateTimeInterface permitted)");
		}
		break;
	case IS_REFERENCE:
		z = Z_REFVAL_P(z);
		goto try_again;
	default:
		intl_errors_set(err, U_ILLEGAL_ARGUMENT_ERROR,
			"invalid PHP type for date");
		break;
	}

	return rv;
}
