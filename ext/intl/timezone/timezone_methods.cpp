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
		0 TSRMLS_CC );
}

U_CFUNC PHP_FUNCTION(intltz_create_time_zone)
{
	char	*str_id;
	int		str_id_len;
	intl_error_reset(NULL TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
			&str_id, &str_id_len) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intltz_create_time_zone: bad arguments", 0 TSRMLS_CC);
		RETURN_NULL();
	}

	UErrorCode status = UErrorCode();
	UnicodeString id = UnicodeString();
	if (intl_stringFromChar(id, str_id, str_id_len, &status) == FAILURE) {
		intl_error_set(NULL, status,
			"intltz_create_time_zone: could not convert time zone id to UTF-16", 0 TSRMLS_CC);
		RETURN_NULL();
	}

	//guaranteed non-null; GMT if timezone cannot be understood
	TimeZone *tz = TimeZone::createTimeZone(id);
	timezone_object_construct(tz, return_value, 1 TSRMLS_CC);
}

U_CFUNC PHP_FUNCTION(intltz_from_date_time_zone)
{
	zval				*zv_timezone;
	TimeZone			*tz;
	php_timezone_obj	*tzobj;
	intl_error_reset(NULL TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O",
			&zv_timezone, php_date_get_timezone_ce()) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intltz_from_date_time_zone: bad arguments", 0 TSRMLS_CC);
		RETURN_NULL();
	}

	tzobj = (php_timezone_obj *)zend_objects_get_address(zv_timezone TSRMLS_CC);
	if (!tzobj->initialized) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intltz_from_date_time_zone: DateTimeZone object is unconstructed",
			0 TSRMLS_CC);
		RETURN_NULL();
	}

	tz = timezone_convert_datetimezone(tzobj->type, tzobj, FALSE, NULL,
		"intltz_from_date_time_zone" TSRMLS_CC);
	if (tz == NULL) {
		RETURN_NULL();
	}

	timezone_object_construct(tz, return_value, 1 TSRMLS_CC);
}

U_CFUNC PHP_FUNCTION(intltz_create_default)
{
	intl_error_reset(NULL TSRMLS_CC);

	if (zend_parse_parameters_none() == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intltz_create_default: bad arguments", 0 TSRMLS_CC);
		RETURN_NULL();
	}

	TimeZone *tz = TimeZone::createDefault();
	timezone_object_construct(tz, return_value, 1 TSRMLS_CC);
}

U_CFUNC PHP_FUNCTION(intltz_get_gmt)
{
	intl_error_reset(NULL TSRMLS_CC);

	if (zend_parse_parameters_none() == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intltz_get_gmt: bad arguments", 0 TSRMLS_CC);
		RETURN_NULL();
	}

	timezone_object_construct(TimeZone::getGMT(), return_value, 0 TSRMLS_CC);
}

#if U_ICU_VERSION_MAJOR_NUM >= 49
U_CFUNC PHP_FUNCTION(intltz_get_unknown)
{
	intl_error_reset(NULL TSRMLS_CC);

	if (zend_parse_parameters_none() == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intltz_get_unknown: bad arguments", 0 TSRMLS_CC);
		RETURN_NULL();
	}

	timezone_object_construct(&TimeZone::getUnknown(), return_value, 0 TSRMLS_CC);
}
#endif

U_CFUNC PHP_FUNCTION(intltz_create_enumeration)
{
	zval				**arg = NULL;
	StringEnumeration	*se	  = NULL;
	intl_error_reset(NULL TSRMLS_CC);

	/* double indirection to have the zend engine destroy the new zval that
	 * results from separation */
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|Z", &arg) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intltz_create_enumeration: bad arguments", 0 TSRMLS_CC);
		RETURN_FALSE;
	}

	if (arg == NULL || Z_TYPE_PP(arg) == IS_NULL) {
		se = TimeZone::createEnumeration();
	} else if (Z_TYPE_PP(arg) == IS_LONG) {
int_offset:
		if (Z_LVAL_PP(arg) < (long)INT32_MIN ||
				Z_LVAL_PP(arg) > (long)INT32_MAX) {
			intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
				"intltz_create_enumeration: value is out of range", 0 TSRMLS_CC);
			RETURN_FALSE;
		} else {
			se = TimeZone::createEnumeration((int32_t) Z_LVAL_PP(arg));
		}
	} else if (Z_TYPE_PP(arg) == IS_DOUBLE) {
double_offset:
		convert_to_long_ex(arg);
		goto int_offset;
	} else if (Z_TYPE_PP(arg) == IS_OBJECT || Z_TYPE_PP(arg) == IS_STRING) {
		long lval;
		double dval;
		convert_to_string_ex(arg);
		switch (is_numeric_string(Z_STRVAL_PP(arg), Z_STRLEN_PP(arg), &lval, &dval, 0)) {
		case IS_DOUBLE:
			SEPARATE_ZVAL(arg);
			zval_dtor(*arg);
			Z_TYPE_PP(arg) = IS_DOUBLE;
			Z_DVAL_PP(arg) = dval;
			goto double_offset;
		case IS_LONG:
			SEPARATE_ZVAL(arg);
			zval_dtor(*arg);
			Z_TYPE_PP(arg) = IS_LONG;
			Z_LVAL_PP(arg) = lval;
			goto int_offset;
		}
		/* else call string version */
		se = TimeZone::createEnumeration(Z_STRVAL_PP(arg));
	} else {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intltz_create_enumeration: invalid argument type", 0 TSRMLS_CC);
		RETURN_FALSE;
	}

	if (se) {
		IntlIterator_from_StringEnumeration(se, return_value TSRMLS_CC);
	} else {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intltz_create_enumeration: error obtaining enumeration", 0 TSRMLS_CC);
		RETVAL_FALSE;
	}
}

U_CFUNC PHP_FUNCTION(intltz_count_equivalent_ids)
{
	char	*str_id;
	int		str_id_len;
	intl_error_reset(NULL TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
			&str_id, &str_id_len) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intltz_count_equivalent_ids: bad arguments", 0 TSRMLS_CC);
		RETURN_FALSE;
	}

	UErrorCode status = UErrorCode();
	UnicodeString id = UnicodeString();
	if (intl_stringFromChar(id, str_id, str_id_len, &status) == FAILURE) {
		intl_error_set(NULL, status,
			"intltz_count_equivalent_ids: could not convert time zone id to UTF-16", 0 TSRMLS_CC);
		RETURN_FALSE;
	}

	int32_t result = TimeZone::countEquivalentIDs(id);
	RETURN_LONG((long)result);
}

#if U_ICU_VERSION_MAJOR_NUM * 10 + U_ICU_VERSION_MINOR_NUM >= 48
U_CFUNC PHP_FUNCTION(intltz_create_time_zone_id_enumeration)
{
	long	zoneType,
			offset_arg;
	char	*region		= NULL;
	int		region_len	= 0;
	int32_t	offset,
			*offsetp	= NULL;
	int		arg3isnull	= 0;
	intl_error_reset(NULL TSRMLS_CC);

	/* must come before zpp because zpp would convert the arg in the stack to 0 */
	if (ZEND_NUM_ARGS() == 3) {
		zval **dummy, **zvoffset;
		arg3isnull = zend_get_parameters_ex(3, &dummy, &dummy, &zvoffset)
				!= FAILURE && Z_TYPE_PP(zvoffset) == IS_NULL;
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|s!l",
			&zoneType, &region, &region_len, &offset_arg) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intltz_create_time_zone_id_enumeration: bad arguments", 0 TSRMLS_CC);
		RETURN_FALSE;
	}

	if (zoneType != UCAL_ZONE_TYPE_ANY && zoneType != UCAL_ZONE_TYPE_CANONICAL
			&& zoneType != UCAL_ZONE_TYPE_CANONICAL_LOCATION) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intltz_create_time_zone_id_enumeration: bad zone type", 0 TSRMLS_CC);
		RETURN_FALSE;
	}

	if (ZEND_NUM_ARGS() == 3) {
		if (offset_arg < (long)INT32_MIN || offset_arg > (long)INT32_MAX) {
			intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
				"intltz_create_time_zone_id_enumeration: offset out of bounds", 0 TSRMLS_CC);
			RETURN_FALSE;
		}
		
		if (!arg3isnull) {
			offset = (int32_t)offset_arg;
			offsetp = &offset;
		} //else leave offsetp NULL
	}

	StringEnumeration *se;
	UErrorCode uec = UErrorCode();
	se = TimeZone::createTimeZoneIDEnumeration((USystemTimeZoneType)zoneType,
		region, offsetp, uec);
	INTL_CHECK_STATUS(uec, "intltz_create_time_zone_id_enumeration: "
		"Error obtaining time zone id enumeration")

	IntlIterator_from_StringEnumeration(se, return_value TSRMLS_CC);
}
#endif

U_CFUNC PHP_FUNCTION(intltz_get_canonical_id)
{
	char	*str_id;
	int		str_id_len;
	zval	*is_systemid = NULL;
	intl_error_reset(NULL TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|z",
			&str_id, &str_id_len, &is_systemid) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intltz_get_canonical_id: bad arguments", 0 TSRMLS_CC);
		RETURN_FALSE;
	}

	UErrorCode status = UErrorCode();
	UnicodeString id;
	if (intl_stringFromChar(id, str_id, str_id_len, &status) == FAILURE) {
		intl_error_set(NULL, status,
			"intltz_get_canonical_id: could not convert time zone id to UTF-16", 0 TSRMLS_CC);
		RETURN_FALSE;
	}

	UnicodeString result;
	UBool isSystemID;
	TimeZone::getCanonicalID(id, result, isSystemID, status);
	INTL_CHECK_STATUS(status, "intltz_get_canonical_id: error obtaining canonical ID");
	
	intl_convert_utf16_to_utf8(&Z_STRVAL_P(return_value), &Z_STRLEN_P(return_value),
		result.getBuffer(), result.length(), &status);
	INTL_CHECK_STATUS(status,
		"intltz_get_canonical_id: could not convert time zone id to UTF-16");
	Z_TYPE_P(return_value) = IS_STRING;
	
	if (is_systemid) { /* by-ref argument passed */
		zval_dtor(is_systemid);
		ZVAL_BOOL(is_systemid, isSystemID);
	}
}

#if U_ICU_VERSION_MAJOR_NUM * 10 + U_ICU_VERSION_MINOR_NUM >= 48
U_CFUNC PHP_FUNCTION(intltz_get_region)
{
	char	*str_id;
	int		str_id_len;
	char	outbuf[3];
	intl_error_reset(NULL TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
			&str_id, &str_id_len) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intltz_get_region: bad arguments", 0 TSRMLS_CC);
		RETURN_FALSE;
	}

	UErrorCode status = UErrorCode();
	UnicodeString id;
	if (intl_stringFromChar(id, str_id, str_id_len, &status) == FAILURE) {
		intl_error_set(NULL, status,
			"intltz_get_region: could not convert time zone id to UTF-16", 0 TSRMLS_CC);
		RETURN_FALSE;
	}

	int32_t region_len = TimeZone::getRegion(id, outbuf, sizeof(outbuf), status);
	INTL_CHECK_STATUS(status, "intltz_get_region: Error obtaining region");

	RETURN_STRINGL(outbuf, region_len, 1);
}
#endif

U_CFUNC PHP_FUNCTION(intltz_get_tz_data_version)
{
	intl_error_reset(NULL TSRMLS_CC);

	if (zend_parse_parameters_none() == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intltz_get_tz_data_version: bad arguments", 0 TSRMLS_CC);
		RETURN_FALSE;
	}

	UErrorCode status = UErrorCode();
	const char *res = TimeZone::getTZDataVersion(status);
	INTL_CHECK_STATUS(status, "intltz_get_tz_data_version: "
		"Error obtaining time zone data version");

	RETURN_STRING(res, 1);
}

U_CFUNC PHP_FUNCTION(intltz_get_equivalent_id)
{
	char	*str_id;
	int		str_id_len;
	long	index;
	intl_error_reset(NULL TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl",
			&str_id, &str_id_len, &index) == FAILURE ||
			index < (long)INT32_MIN || index > (long)INT32_MAX) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intltz_get_equivalent_id: bad arguments", 0 TSRMLS_CC);
		RETURN_FALSE;
	}

	UErrorCode status = UErrorCode();
	UnicodeString id;
	if (intl_stringFromChar(id, str_id, str_id_len, &status) == FAILURE) {
		intl_error_set(NULL, status,
			"intltz_get_equivalent_id: could not convert time zone id to UTF-16", 0 TSRMLS_CC);
		RETURN_FALSE;
	}

	const UnicodeString result = TimeZone::getEquivalentID(id, (int32_t)index);
	intl_convert_utf16_to_utf8(&Z_STRVAL_P(return_value), &Z_STRLEN_P(return_value),
		result.getBuffer(), result.length(), &status);
	INTL_CHECK_STATUS(status, "intltz_get_equivalent_id: "
		"could not convert resulting time zone id to UTF-16");
	Z_TYPE_P(return_value) = IS_STRING;
}

U_CFUNC PHP_FUNCTION(intltz_get_id)
{
	TIMEZONE_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O",
			&object, TimeZone_ce_ptr) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intltz_get_id: bad arguments", 0 TSRMLS_CC);
		RETURN_FALSE;
	}

	TIMEZONE_METHOD_FETCH_OBJECT;

	UnicodeString id_us;
	to->utimezone->getID(id_us);

	char *id = NULL;
	int  id_len   = 0;

	intl_convert_utf16_to_utf8(&id, &id_len,
		id_us.getBuffer(), id_us.length(), TIMEZONE_ERROR_CODE_P(to));
	INTL_METHOD_CHECK_STATUS(to, "intltz_get_id: Could not convert id to UTF-8");

	RETURN_STRINGL(id, id_len, 0);
}

U_CFUNC PHP_FUNCTION(intltz_use_daylight_time)
{
	TIMEZONE_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O",
			&object, TimeZone_ce_ptr) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intltz_use_daylight_time: bad arguments", 0 TSRMLS_CC);
		RETURN_FALSE;
	}

	TIMEZONE_METHOD_FETCH_OBJECT;

	RETURN_BOOL(to->utimezone->useDaylightTime());
}

U_CFUNC PHP_FUNCTION(intltz_get_offset)
{
	UDate		date;
	zend_bool	local;
	zval		*rawOffsetArg,
				*dstOffsetArg;
	int32_t		rawOffset,
				dstOffset;
	TIMEZONE_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(),
			"Odbzz", &object, TimeZone_ce_ptr, &date, &local, &rawOffsetArg,
			&dstOffsetArg) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intltz_get_offset: bad arguments", 0 TSRMLS_CC);
		RETURN_FALSE;
	}

	TIMEZONE_METHOD_FETCH_OBJECT;

	to->utimezone->getOffset(date, (UBool) local, rawOffset, dstOffset,
		TIMEZONE_ERROR_CODE(to));

	INTL_METHOD_CHECK_STATUS(to, "intltz_get_offset: error obtaining offset");

	zval_dtor(rawOffsetArg);
	ZVAL_LONG(rawOffsetArg, rawOffset);
	zval_dtor(dstOffsetArg);
	ZVAL_LONG(dstOffsetArg, dstOffset);

	RETURN_TRUE;
}

U_CFUNC PHP_FUNCTION(intltz_get_raw_offset)
{
	TIMEZONE_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(),
			"O", &object, TimeZone_ce_ptr) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intltz_get_raw_offset: bad arguments", 0 TSRMLS_CC);
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

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(),
			"OO", &object, TimeZone_ce_ptr, &other_object, TimeZone_ce_ptr)
			== FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intltz_has_same_rules: bad arguments", 0 TSRMLS_CC);
		RETURN_FALSE;
	}
	TIMEZONE_METHOD_FETCH_OBJECT;
	other_to = (TimeZone_object *) zend_object_store_get_object(other_object TSRMLS_CC);
	if (other_to->utimezone == NULL) {
		intl_errors_set(&to->err, U_ILLEGAL_ARGUMENT_ERROR,
			"intltz_has_same_rules: The second IntlTimeZone is unconstructed", 0 TSRMLS_CC);
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
	long		display_type	= TimeZone::LONG;
	const char	*locale_str		= NULL;
	int			dummy			= 0;
	TIMEZONE_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(),
			"O|bls!", &object, TimeZone_ce_ptr, &daylight, &display_type,
			&locale_str, &dummy) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intltz_get_display_name: bad arguments", 0 TSRMLS_CC);
		RETURN_FALSE;
	}

	bool found = false;
	for (int i = 0; !found && i < sizeof(display_types)/sizeof(*display_types); i++) {
		if (display_types[i] == display_type)
			found = true;
	}
	if (!found) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intltz_get_display_name: wrong display type", 0 TSRMLS_CC);
		RETURN_FALSE;
	}

	if (!locale_str) {
		locale_str = intl_locale_get_default(TSRMLS_C);
	}

	TIMEZONE_METHOD_FETCH_OBJECT;

	UnicodeString result;
	to->utimezone->getDisplayName((UBool)daylight, (TimeZone::EDisplayType)display_type,
		Locale::createFromName(locale_str), result);

	intl_convert_utf16_to_utf8(&Z_STRVAL_P(return_value), &Z_STRLEN_P(return_value),
		result.getBuffer(), result.length(), TIMEZONE_ERROR_CODE_P(to));
	INTL_METHOD_CHECK_STATUS(to, "intltz_get_display_name: "
		"could not convert resulting time zone id to UTF-16");

	Z_TYPE_P(return_value) = IS_STRING;
}

U_CFUNC PHP_FUNCTION(intltz_get_dst_savings)
{
	TIMEZONE_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(),
			"O", &object, TimeZone_ce_ptr) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intltz_get_dst_savings: bad arguments", 0 TSRMLS_CC);
		RETURN_FALSE;
	}

	TIMEZONE_METHOD_FETCH_OBJECT;

	RETURN_LONG((long)to->utimezone->getDSTSavings());
}

U_CFUNC PHP_FUNCTION(intltz_to_date_time_zone)
{
	TIMEZONE_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(),
			"O", &object, TimeZone_ce_ptr) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intltz_to_date_time_zone: bad arguments", 0 TSRMLS_CC);
		RETURN_FALSE;
	}

	TIMEZONE_METHOD_FETCH_OBJECT;

	zval *ret = timezone_convert_to_datetimezone(to->utimezone,
		&TIMEZONE_ERROR(to), "intltz_to_date_time_zone" TSRMLS_CC);

	if (ret) {
		RETURN_ZVAL(ret, 1, 1);
	} else {
		RETURN_FALSE;
	}
}

U_CFUNC PHP_FUNCTION(intltz_get_error_code)
{
	TIMEZONE_METHOD_INIT_VARS

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O",
			&object, TimeZone_ce_ptr) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intltz_get_error_code: bad arguments", 0 TSRMLS_CC);
		RETURN_FALSE;
	}

	/* Fetch the object (without resetting its last error code ). */
	to = (TimeZone_object*)zend_object_store_get_object(object TSRMLS_CC);
	if (to == NULL)
		RETURN_FALSE;

	RETURN_LONG((long)TIMEZONE_ERROR_CODE(to));
}

U_CFUNC PHP_FUNCTION(intltz_get_error_message)
{
	const char* message = NULL;
	TIMEZONE_METHOD_INIT_VARS

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O",
			&object, TimeZone_ce_ptr) == FAILURE) {
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intltz_get_error_message: bad arguments", 0 TSRMLS_CC );
		RETURN_FALSE;
	}


	/* Fetch the object (without resetting its last error code ). */
	to = (TimeZone_object*)zend_object_store_get_object(object TSRMLS_CC);
	if (to == NULL)
		RETURN_FALSE;

	/* Return last error message. */
	message = intl_error_get_message(TIMEZONE_ERROR_P(to) TSRMLS_CC);
	RETURN_STRING(message, 0);
}
