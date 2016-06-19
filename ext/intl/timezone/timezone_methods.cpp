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
#include <unicode/timezone.h>
#include <unicode/ustring.h>
#include "intl_convertcpp.h"

#include "../common/common_date.h"

extern "C" {
#include "../php_intl.h"
#define USE_TIMEZONE_POINTER 1
#include "timezone_class.h"
#include "intl_convert.h"
#include <zend_exceptions.h>
#include <ext/date/php_date.h>
}
#include "common/common_enum.h"

U_CFUNC PHP_METHOD(IntlTimeZone, __construct)
{
	zend_throw_exception( NULL,
		"An object of this type cannot be created with the new operator",
		0 );
}

U_CFUNC PHP_FUNCTION(intltz_create_time_zone)
{
	char	*str_id;
	size_t	 str_id_len;
	intl_error_reset(NULL);

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &str_id, &str_id_len) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intltz_create_time_zone: bad arguments", 0);
		RETURN_NULL();
	}

	UErrorCode status = UErrorCode();
	UnicodeString id = UnicodeString();
	if (intl_stringFromChar(id, str_id, str_id_len, &status) == FAILURE) {
		intl_error_set(NULL, status,
			"intltz_create_time_zone: could not convert time zone id to UTF-16", 0);
		RETURN_NULL();
	}

	//guaranteed non-null; GMT if timezone cannot be understood
	TimeZone *tz = TimeZone::createTimeZone(id);
	timezone_object_construct(tz, return_value, 1);
}

U_CFUNC PHP_FUNCTION(intltz_from_date_time_zone)
{
	zval				*zv_timezone;
	TimeZone			*tz;
	php_timezone_obj	*tzobj;
	intl_error_reset(NULL);

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O",
			&zv_timezone, php_date_get_timezone_ce()) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intltz_from_date_time_zone: bad arguments", 0);
		RETURN_NULL();
	}

	tzobj = Z_PHPTIMEZONE_P(zv_timezone);
	if (!tzobj->initialized) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intltz_from_date_time_zone: DateTimeZone object is unconstructed",
			0);
		RETURN_NULL();
	}

	tz = timezone_convert_datetimezone(tzobj->type, tzobj, FALSE, NULL,
		"intltz_from_date_time_zone");
	if (tz == NULL) {
		RETURN_NULL();
	}

	timezone_object_construct(tz, return_value, 1);
}

U_CFUNC PHP_FUNCTION(intltz_create_default)
{
	intl_error_reset(NULL);

	if (zend_parse_parameters_none() == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intltz_create_default: bad arguments", 0);
		RETURN_NULL();
	}

	TimeZone *tz = TimeZone::createDefault();
	timezone_object_construct(tz, return_value, 1);
}

U_CFUNC PHP_FUNCTION(intltz_get_gmt)
{
	intl_error_reset(NULL);

	if (zend_parse_parameters_none() == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intltz_get_gmt: bad arguments", 0);
		RETURN_NULL();
	}

	timezone_object_construct(TimeZone::getGMT(), return_value, 0);
}

#if U_ICU_VERSION_MAJOR_NUM >= 49
U_CFUNC PHP_FUNCTION(intltz_get_unknown)
{
	intl_error_reset(NULL);

	if (zend_parse_parameters_none() == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intltz_get_unknown: bad arguments", 0);
		RETURN_NULL();
	}

	timezone_object_construct(&TimeZone::getUnknown(), return_value, 0);
}
#endif

U_CFUNC PHP_FUNCTION(intltz_create_enumeration)
{
	zval				*arg = NULL;
	StringEnumeration	*se	  = NULL;
	intl_error_reset(NULL);

	/* double indirection to have the zend engine destroy the new zval that
	 * results from separation */
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|z", &arg) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intltz_create_enumeration: bad arguments", 0);
		RETURN_FALSE;
	}

	if (arg == NULL || Z_TYPE_P(arg) == IS_NULL) {
		se = TimeZone::createEnumeration();
	} else if (Z_TYPE_P(arg) == IS_LONG) {
int_offset:
		if (Z_LVAL_P(arg) < (zend_long)INT32_MIN ||
				Z_LVAL_P(arg) > (zend_long)INT32_MAX) {
			intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
				"intltz_create_enumeration: value is out of range", 0);
			RETURN_FALSE;
		} else {
			se = TimeZone::createEnumeration((int32_t) Z_LVAL_P(arg));
		}
	} else if (Z_TYPE_P(arg) == IS_DOUBLE) {
double_offset:
		convert_to_long_ex(arg);
		goto int_offset;
	} else if (Z_TYPE_P(arg) == IS_OBJECT || Z_TYPE_P(arg) == IS_STRING) {
		zend_long lval;
		double dval;
		convert_to_string_ex(arg);
		switch (is_numeric_string(Z_STRVAL_P(arg), Z_STRLEN_P(arg), &lval, &dval, 0)) {
		case IS_DOUBLE:
			SEPARATE_ZVAL(arg);
			zval_dtor(arg);
			ZVAL_DOUBLE(arg, dval);
			goto double_offset;
		case IS_LONG:
			SEPARATE_ZVAL(arg);
			zval_dtor(arg);
			ZVAL_LONG(arg, lval);
			goto int_offset;
		}
		/* else call string version */
		se = TimeZone::createEnumeration(Z_STRVAL_P(arg));
	} else {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intltz_create_enumeration: invalid argument type", 0);
		RETURN_FALSE;
	}

	if (se) {
		IntlIterator_from_StringEnumeration(se, return_value);
	} else {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intltz_create_enumeration: error obtaining enumeration", 0);
		RETVAL_FALSE;
	}
}

U_CFUNC PHP_FUNCTION(intltz_count_equivalent_ids)
{
	char	*str_id;
	size_t	 str_id_len;
	intl_error_reset(NULL);

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s",
			&str_id, &str_id_len) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intltz_count_equivalent_ids: bad arguments", 0);
		RETURN_FALSE;
	}

	UErrorCode status = UErrorCode();
	UnicodeString id = UnicodeString();
	if (intl_stringFromChar(id, str_id, str_id_len, &status) == FAILURE) {
		intl_error_set(NULL, status,
			"intltz_count_equivalent_ids: could not convert time zone id to UTF-16", 0);
		RETURN_FALSE;
	}

	int32_t result = TimeZone::countEquivalentIDs(id);
	RETURN_LONG((zend_long)result);
}

#if U_ICU_VERSION_MAJOR_NUM * 10 + U_ICU_VERSION_MINOR_NUM >= 48
U_CFUNC PHP_FUNCTION(intltz_create_time_zone_id_enumeration)
{
	zend_long zoneType,
			  offset_arg;
	char	 *region		= NULL;
	size_t	  region_len	= 0;
	int32_t	  offset,
			 *offsetp	= NULL;
	zend_bool arg3isnull = 1;

	intl_error_reset(NULL);

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l|s!l!",
			&zoneType, &region, &region_len, &offset_arg, &arg3isnull) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intltz_create_time_zone_id_enumeration: bad arguments", 0);
		RETURN_FALSE;
	}

	if (zoneType != UCAL_ZONE_TYPE_ANY && zoneType != UCAL_ZONE_TYPE_CANONICAL
			&& zoneType != UCAL_ZONE_TYPE_CANONICAL_LOCATION) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intltz_create_time_zone_id_enumeration: bad zone type", 0);
		RETURN_FALSE;
	}

	if (!arg3isnull) {
		if (offset_arg < (zend_long)INT32_MIN || offset_arg > (zend_long)INT32_MAX) {
			intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
				"intltz_create_time_zone_id_enumeration: offset out of bounds", 0);
			RETURN_FALSE;
		}
		offset = (int32_t)offset_arg;
		offsetp = &offset;
	} //else leave offsetp NULL

	StringEnumeration *se;
	UErrorCode uec = UErrorCode();
	se = TimeZone::createTimeZoneIDEnumeration((USystemTimeZoneType)zoneType,
		region, offsetp, uec);
	INTL_CHECK_STATUS(uec, "intltz_create_time_zone_id_enumeration: "
		"Error obtaining time zone id enumeration")

	IntlIterator_from_StringEnumeration(se, return_value);
}
#endif

U_CFUNC PHP_FUNCTION(intltz_get_canonical_id)
{
	char	*str_id;
	size_t	 str_id_len;
	zval	*is_systemid = NULL;
	intl_error_reset(NULL);

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|z",
			&str_id, &str_id_len, &is_systemid) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intltz_get_canonical_id: bad arguments", 0);
		RETURN_FALSE;
	}

	UErrorCode status = UErrorCode();
	UnicodeString id;
	if (intl_stringFromChar(id, str_id, str_id_len, &status) == FAILURE) {
		intl_error_set(NULL, status,
			"intltz_get_canonical_id: could not convert time zone id to UTF-16", 0);
		RETURN_FALSE;
	}

	UnicodeString result;
	UBool isSystemID;
	TimeZone::getCanonicalID(id, result, isSystemID, status);
	INTL_CHECK_STATUS(status, "intltz_get_canonical_id: error obtaining canonical ID");

	zend_string *u8str =intl_convert_utf16_to_utf8(result.getBuffer(), result.length(), &status);
	INTL_CHECK_STATUS(status,
		"intltz_get_canonical_id: could not convert time zone id to UTF-16");
	RETVAL_NEW_STR(u8str);

	if (is_systemid) { /* by-ref argument passed */
		ZVAL_DEREF(is_systemid);
		zval_dtor(is_systemid);
		ZVAL_BOOL(is_systemid, isSystemID);
	}
}

#if U_ICU_VERSION_MAJOR_NUM * 10 + U_ICU_VERSION_MINOR_NUM >= 48
U_CFUNC PHP_FUNCTION(intltz_get_region)
{
	char	*str_id;
	size_t	 str_id_len;
	char	 outbuf[3];
	intl_error_reset(NULL);

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s",
			&str_id, &str_id_len) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intltz_get_region: bad arguments", 0);
		RETURN_FALSE;
	}

	UErrorCode status = UErrorCode();
	UnicodeString id;
	if (intl_stringFromChar(id, str_id, str_id_len, &status) == FAILURE) {
		intl_error_set(NULL, status,
			"intltz_get_region: could not convert time zone id to UTF-16", 0);
		RETURN_FALSE;
	}

	int32_t region_len = TimeZone::getRegion(id, outbuf, sizeof(outbuf), status);
	INTL_CHECK_STATUS(status, "intltz_get_region: Error obtaining region");

	RETURN_STRINGL(outbuf, region_len);
}
#endif

U_CFUNC PHP_FUNCTION(intltz_get_tz_data_version)
{
	intl_error_reset(NULL);

	if (zend_parse_parameters_none() == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intltz_get_tz_data_version: bad arguments", 0);
		RETURN_FALSE;
	}

	UErrorCode status = UErrorCode();
	const char *res = TimeZone::getTZDataVersion(status);
	INTL_CHECK_STATUS(status, "intltz_get_tz_data_version: "
		"Error obtaining time zone data version");

	RETURN_STRING(res);
}

U_CFUNC PHP_FUNCTION(intltz_get_equivalent_id)
{
	char	   *str_id;
	size_t		str_id_len;
	zend_long	index;
	intl_error_reset(NULL);

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sl",
			&str_id, &str_id_len, &index) == FAILURE ||
			index < (zend_long)INT32_MIN || index > (zend_long)INT32_MAX) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intltz_get_equivalent_id: bad arguments", 0);
		RETURN_FALSE;
	}

	UErrorCode status = UErrorCode();
	UnicodeString id;
	if (intl_stringFromChar(id, str_id, str_id_len, &status) == FAILURE) {
		intl_error_set(NULL, status,
			"intltz_get_equivalent_id: could not convert time zone id to UTF-16", 0);
		RETURN_FALSE;
	}

	const UnicodeString result = TimeZone::getEquivalentID(id, (int32_t)index);
	zend_string *u8str;

	u8str = intl_convert_utf16_to_utf8(result.getBuffer(), result.length(), &status);
	INTL_CHECK_STATUS(status, "intltz_get_equivalent_id: "
		"could not convert resulting time zone id to UTF-16");
	RETVAL_NEW_STR(u8str);
}

U_CFUNC PHP_FUNCTION(intltz_get_id)
{
	TIMEZONE_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O",
			&object, TimeZone_ce_ptr) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intltz_get_id: bad arguments", 0);
		RETURN_FALSE;
	}

	TIMEZONE_METHOD_FETCH_OBJECT;

	UnicodeString id_us;
	to->utimezone->getID(id_us);

	zend_string *u8str;

	u8str = intl_convert_utf16_to_utf8(
		id_us.getBuffer(), id_us.length(), TIMEZONE_ERROR_CODE_P(to));
	INTL_METHOD_CHECK_STATUS(to, "intltz_get_id: Could not convert id to UTF-8");

	RETVAL_NEW_STR(u8str);
}

U_CFUNC PHP_FUNCTION(intltz_use_daylight_time)
{
	TIMEZONE_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O",
			&object, TimeZone_ce_ptr) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intltz_use_daylight_time: bad arguments", 0);
		RETURN_FALSE;
	}

	TIMEZONE_METHOD_FETCH_OBJECT;

	RETURN_BOOL(to->utimezone->useDaylightTime());
}

U_CFUNC PHP_FUNCTION(intltz_get_offset)
{
	double		date;
	zend_bool	local;
	zval		*rawOffsetArg,
				*dstOffsetArg;
	int32_t		rawOffset,
				dstOffset;
	TIMEZONE_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(),
			"Odbz/z/", &object, TimeZone_ce_ptr, &date, &local, &rawOffsetArg,
			&dstOffsetArg) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intltz_get_offset: bad arguments", 0);
		RETURN_FALSE;
	}

	TIMEZONE_METHOD_FETCH_OBJECT;

	to->utimezone->getOffset((UDate) date, (UBool) local, rawOffset, dstOffset,
		TIMEZONE_ERROR_CODE(to));

	INTL_METHOD_CHECK_STATUS(to, "intltz_get_offset: error obtaining offset");

	ZVAL_DEREF(rawOffsetArg);
	zval_dtor(rawOffsetArg);
	ZVAL_LONG(rawOffsetArg, rawOffset);
	ZVAL_DEREF(dstOffsetArg);
	zval_dtor(dstOffsetArg);
	ZVAL_LONG(dstOffsetArg, dstOffset);

	RETURN_TRUE;
}

U_CFUNC PHP_FUNCTION(intltz_get_raw_offset)
{
	TIMEZONE_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(),
			"O", &object, TimeZone_ce_ptr) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intltz_get_raw_offset: bad arguments", 0);
		RETURN_FALSE;
	}

	TIMEZONE_METHOD_FETCH_OBJECT;

	RETURN_LONG(to->utimezone->getRawOffset());
}

U_CFUNC PHP_FUNCTION(intltz_has_same_rules)
{
	zval			*other_object;
	TimeZone_object	*other_to;
	TIMEZONE_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(),
			"OO", &object, TimeZone_ce_ptr, &other_object, TimeZone_ce_ptr)
			== FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intltz_has_same_rules: bad arguments", 0);
		RETURN_FALSE;
	}
	TIMEZONE_METHOD_FETCH_OBJECT;
	other_to = Z_INTL_TIMEZONE_P(other_object);
	if (other_to->utimezone == NULL) {
		intl_errors_set(&to->err, U_ILLEGAL_ARGUMENT_ERROR,
			"intltz_has_same_rules: The second IntlTimeZone is unconstructed", 0);
		RETURN_FALSE;
	}

	RETURN_BOOL(to->utimezone->hasSameRules(*other_to->utimezone));
}

static const TimeZone::EDisplayType display_types[] = {
	TimeZone::SHORT,				TimeZone::LONG,
#if U_ICU_VERSION_MAJOR_NUM * 10 + U_ICU_VERSION_MINOR_NUM >= 44
	TimeZone::SHORT_GENERIC,		TimeZone::LONG_GENERIC,
	TimeZone::SHORT_GMT,			TimeZone::LONG_GMT,
	TimeZone::SHORT_COMMONLY_USED,	TimeZone::GENERIC_LOCATION
#endif
};

U_CFUNC PHP_FUNCTION(intltz_get_display_name)
{
	zend_bool	daylight		= 0;
	zend_long	display_type	= TimeZone::LONG;
	const char *locale_str		= NULL;
	size_t		dummy			= 0;
	TIMEZONE_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(),
			"O|bls!", &object, TimeZone_ce_ptr, &daylight, &display_type,
			&locale_str, &dummy) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intltz_get_display_name: bad arguments", 0);
		RETURN_FALSE;
	}

	bool found = false;
	for (int i = 0; !found && i < sizeof(display_types)/sizeof(*display_types); i++) {
		if (display_types[i] == display_type)
			found = true;
	}
	if (!found) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intltz_get_display_name: wrong display type", 0);
		RETURN_FALSE;
	}

	if (!locale_str) {
		locale_str = intl_locale_get_default();
	}

	TIMEZONE_METHOD_FETCH_OBJECT;

	UnicodeString result;
	to->utimezone->getDisplayName((UBool)daylight, (TimeZone::EDisplayType)display_type,
		Locale::createFromName(locale_str), result);

	zend_string *u8str = intl_convert_utf16_to_utf8(result.getBuffer(), result.length(), TIMEZONE_ERROR_CODE_P(to));
	INTL_METHOD_CHECK_STATUS(to, "intltz_get_display_name: "
		"could not convert resulting time zone id to UTF-16");

	RETVAL_NEW_STR(u8str);
}

U_CFUNC PHP_FUNCTION(intltz_get_dst_savings)
{
	TIMEZONE_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(),
			"O", &object, TimeZone_ce_ptr) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intltz_get_dst_savings: bad arguments", 0);
		RETURN_FALSE;
	}

	TIMEZONE_METHOD_FETCH_OBJECT;

	RETURN_LONG((zend_long)to->utimezone->getDSTSavings());
}

U_CFUNC PHP_FUNCTION(intltz_to_date_time_zone)
{
	zval tmp;
	TIMEZONE_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(),
			"O", &object, TimeZone_ce_ptr) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intltz_to_date_time_zone: bad arguments", 0);
		RETURN_FALSE;
	}

	TIMEZONE_METHOD_FETCH_OBJECT;

	zval *ret = timezone_convert_to_datetimezone(to->utimezone,
		&TIMEZONE_ERROR(to), "intltz_to_date_time_zone", &tmp);

	if (ret) {
		ZVAL_COPY_VALUE(return_value, ret);
	} else {
		RETURN_FALSE;
	}
}

U_CFUNC PHP_FUNCTION(intltz_get_error_code)
{
	TIMEZONE_METHOD_INIT_VARS

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O",
			&object, TimeZone_ce_ptr) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intltz_get_error_code: bad arguments", 0);
		RETURN_FALSE;
	}

	/* Fetch the object (without resetting its last error code ). */
	to = Z_INTL_TIMEZONE_P(object);
	if (to == NULL)
		RETURN_FALSE;

	RETURN_LONG((zend_long)TIMEZONE_ERROR_CODE(to));
}

U_CFUNC PHP_FUNCTION(intltz_get_error_message)
{
	zend_string* message = NULL;
	TIMEZONE_METHOD_INIT_VARS

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O",
			&object, TimeZone_ce_ptr) == FAILURE) {
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intltz_get_error_message: bad arguments", 0 );
		RETURN_FALSE;
	}


	/* Fetch the object (without resetting its last error code ). */
	to = Z_INTL_TIMEZONE_P(object);
	if (to == NULL)
		RETURN_FALSE;

	/* Return last error message. */
	message = intl_error_get_message(TIMEZONE_ERROR_P(to));
	RETURN_STR(message);
}

#if U_ICU_VERSION_MAJOR_NUM >= 52
/* {{{ proto string IntlTimeZone::getWindowsID(string $timezone)
       proto string intltz_get_windows_id(string $timezone)
Translate a system timezone (e.g. "America/Los_Angeles" into a
Windows Timezone (e.g. "Pacific Standard Time")
 */
U_CFUNC PHP_FUNCTION(intltz_get_windows_id)
{
	zend_string *id, *winID;
	UnicodeString uID, uWinID;
	UErrorCode error;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &id) == FAILURE) {
		return;
	}

	error = U_ZERO_ERROR;
	if (intl_stringFromChar(uID, id->val, id->len, &error) == FAILURE) {
		intl_error_set(NULL, error,
		               "intltz_get_windows_id: could not convert time zone id to UTF-16", 0);
		RETURN_FALSE;
	}

	error = U_ZERO_ERROR;
	TimeZone::getWindowsID(uID, uWinID, error);
	INTL_CHECK_STATUS(error, "intltz_get_windows_id: Unable to get timezone from windows ID");
	if (uWinID.length() == 0) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
		               "intltz_get_windows_id: Unknown system timezone", 0);
		RETURN_FALSE;
	}

	error = U_ZERO_ERROR;
	winID = intl_convert_utf16_to_utf8(uWinID.getBuffer(), uWinID.length(), &error);
	INTL_CHECK_STATUS(error, "intltz_get_windows_id: could not convert time zone id to UTF-8");
	RETURN_STR(winID);
}
/* }}} */

/* {{{ proto string IntlTimeZone::getIDForWindowsID(string $timezone[, string $region = NULL])
       proto string intltz_get_id_for_windows_id(string $timezone[, string $region = NULL])
Translate a windows timezone (e.g. "Pacific Time Zone" into a
System Timezone (e.g. "America/Los_Angeles")
 */
U_CFUNC PHP_FUNCTION(intltz_get_id_for_windows_id)
{
	zend_string *winID, *region = NULL, *id;
	UnicodeString uWinID, uID;
	UErrorCode error;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|S", &winID, &region) == FAILURE) {
		return;
	}

	error = U_ZERO_ERROR;
	if (intl_stringFromChar(uWinID, winID->val, winID->len, &error) == FAILURE) {
		intl_error_set(NULL, error,
		               "intltz_get_id_for_windows_id: could not convert time zone id to UTF-16", 0);
		RETURN_FALSE;
	}

	error = U_ZERO_ERROR;
	TimeZone::getIDForWindowsID(uWinID, region ? region->val : NULL, uID, error);
	INTL_CHECK_STATUS(error, "intltz_get_id_for_windows_id: Unable to get windows ID for timezone");
	if (uID.length() == 0) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
		               "intltz_get_windows_id: Unknown windows timezone", 0);
		RETURN_FALSE;
	}

	error = U_ZERO_ERROR;
	id = intl_convert_utf16_to_utf8(uID.getBuffer(), uID.length(), &error);
	INTL_CHECK_STATUS(error, "intltz_get_id_for_windows_id: could not convert time zone id to UTF-8");
	RETURN_STR(id);
}
/* }}} */
#endif
