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

#include <unicode/timezone.h>
#include <unicode/calendar.h>
#include "../intl_convertcpp.h"

#include "../common/common_date.h"

extern "C" {
#include "../intl_convert.h"
#define USE_TIMEZONE_POINTER 1
#include "timezone_class.h"
#include "timezone_methods.h"
#include <zend_exceptions.h>
#include <zend_interfaces.h>
#include <ext/date/php_date.h>
}

using icu::Calendar;

/* {{{ Global variables */
U_CDECL_BEGIN
zend_class_entry *TimeZone_ce_ptr = NULL;
zend_object_handlers TimeZone_handlers;
U_CDECL_END
/* }}} */

/* {{{ timezone_object_construct */
U_CFUNC void timezone_object_construct(const TimeZone *zone, zval *object, int owned)
{
	TimeZone_object	*to;

	object_init_ex(object, TimeZone_ce_ptr);
	TIMEZONE_METHOD_FETCH_OBJECT_NO_CHECK; /* fetch zend object from zval "object" into "to" */
	to->utimezone = zone;
	to->should_delete = owned;
}
/* }}} */

/* {{{ timezone_convert_to_datetimezone
 *	   Convert from TimeZone to DateTimeZone object */
U_CFUNC zval *timezone_convert_to_datetimezone(const TimeZone *timeZone,
											   intl_error *outside_error,
											   const char *func, zval *ret)
{
	UnicodeString		id;
	char				*message = NULL;
	php_timezone_obj	*tzobj;
	zval				arg;

	timeZone->getID(id);
	if (id.isBogus()) {
		spprintf(&message, 0, "%s: could not obtain TimeZone id", func);
		intl_errors_set(outside_error, U_ILLEGAL_ARGUMENT_ERROR,
			message, 1);
		goto error;
	}

	object_init_ex(ret, php_date_get_timezone_ce());
	tzobj = Z_PHPTIMEZONE_P(ret);

	if (id.compare(0, 3, UnicodeString("GMT", sizeof("GMT")-1, US_INV)) == 0) {
		/* The DateTimeZone constructor doesn't support offset time zones,
		 * so we must mess with DateTimeZone structure ourselves */
		tzobj->initialized	  = 1;
		tzobj->type			  = TIMELIB_ZONETYPE_OFFSET;
		//convert offset from milliseconds to seconds
		tzobj->tzi.utc_offset = timeZone->getRawOffset() / 1000;
	} else {
		zend_string *u8str;
		/* Call the constructor! */
		u8str = intl_charFromString(id, &INTL_ERROR_CODE(*outside_error));
		if (!u8str) {
			spprintf(&message, 0, "%s: could not convert id to UTF-8", func);
			intl_errors_set(outside_error, INTL_ERROR_CODE(*outside_error),
				message, 1);
			goto error;
		}
		ZVAL_STR(&arg, u8str);
		zend_call_method_with_1_params(ret, NULL, &Z_OBJCE_P(ret)->constructor, "__construct", NULL, &arg);
		if (EG(exception)) {
			spprintf(&message, 0,
				"%s: DateTimeZone constructor threw exception", func);
			intl_errors_set(outside_error, U_ILLEGAL_ARGUMENT_ERROR,
				message, 1);
			zend_object_store_ctor_failed(Z_OBJ_P(ret));
			zval_ptr_dtor(&arg);
			goto error;
		}
		zval_ptr_dtor(&arg);
	}

	if (0) {
error:
		if (ret) {
			zval_ptr_dtor(ret);
		}
		ret = NULL;
	}

	if (message) {
		efree(message);
	}
	return ret;
}
/* }}} */

/* {{{ timezone_process_timezone_argument
 * TimeZone argument processor. outside_error may be NULL (for static functions/constructors) */
U_CFUNC TimeZone *timezone_process_timezone_argument(zval *zv_timezone,
													 intl_error *outside_error,
													 const char *func)
{
	zval		local_zv_tz;
	char		*message = NULL;
	TimeZone	*timeZone;

	if (zv_timezone == NULL || Z_TYPE_P(zv_timezone) == IS_NULL) {
		timelib_tzinfo *tzinfo = get_timezone_info();
		ZVAL_STRING(&local_zv_tz, tzinfo->name);
		zv_timezone = &local_zv_tz;
	} else {
		ZVAL_NULL(&local_zv_tz);
	}

	if (Z_TYPE_P(zv_timezone) == IS_OBJECT &&
			instanceof_function(Z_OBJCE_P(zv_timezone), TimeZone_ce_ptr)) {
		TimeZone_object *to = Z_INTL_TIMEZONE_P(zv_timezone);
		if (to->utimezone == NULL) {
			spprintf(&message, 0, "%s: passed IntlTimeZone is not "
				"properly constructed", func);
			if (message) {
				intl_errors_set(outside_error, U_ILLEGAL_ARGUMENT_ERROR, message, 1);
				efree(message);
			}
			zval_ptr_dtor_str(&local_zv_tz);
			return NULL;
		}
		timeZone = to->utimezone->clone();
		if (timeZone == NULL) {
			spprintf(&message, 0, "%s: could not clone TimeZone", func);
			if (message) {
				intl_errors_set(outside_error, U_MEMORY_ALLOCATION_ERROR, message, 1);
				efree(message);
			}
			zval_ptr_dtor_str(&local_zv_tz);
			return NULL;
		}
	} else if (Z_TYPE_P(zv_timezone) == IS_OBJECT &&
			instanceof_function(Z_OBJCE_P(zv_timezone), php_date_get_timezone_ce())) {

		php_timezone_obj *tzobj = Z_PHPTIMEZONE_P(zv_timezone);

		zval_ptr_dtor_str(&local_zv_tz);
		return timezone_convert_datetimezone(tzobj->type, tzobj, 0,
			outside_error, func);
	} else {
		UnicodeString	id,
						gottenId;
		UErrorCode		status = U_ZERO_ERROR; /* outside_error may be NULL */
		convert_to_string_ex(zv_timezone);
		if (intl_stringFromChar(id, Z_STRVAL_P(zv_timezone), Z_STRLEN_P(zv_timezone),
				&status) == FAILURE) {
			spprintf(&message, 0, "%s: Time zone identifier given is not a "
				"valid UTF-8 string", func);
			if (message) {
				intl_errors_set(outside_error, status, message, 1);
				efree(message);
			}
			zval_ptr_dtor_str(&local_zv_tz);
			return NULL;
		}
		timeZone = TimeZone::createTimeZone(id);
		if (timeZone == NULL) {
			spprintf(&message, 0, "%s: could not create time zone", func);
			if (message) {
				intl_errors_set(outside_error, U_MEMORY_ALLOCATION_ERROR, message, 1);
				efree(message);
			}
			zval_ptr_dtor_str(&local_zv_tz);
			return NULL;
		}
		if (timeZone->getID(gottenId) != id) {
			spprintf(&message, 0, "%s: no such time zone: '%s'",
				func, Z_STRVAL_P(zv_timezone));
			if (message) {
				intl_errors_set(outside_error, U_ILLEGAL_ARGUMENT_ERROR, message, 1);
				efree(message);
			}
			zval_ptr_dtor_str(&local_zv_tz);
			delete timeZone;
			return NULL;
		}
	}

	zval_ptr_dtor_str(&local_zv_tz);

	return timeZone;
}
/* }}} */

/* {{{ clone handler for TimeZone */
static zend_object *TimeZone_clone_obj(zval *object)
{
	TimeZone_object		*to_orig,
						*to_new;
	zend_object			*ret_val;
	intl_error_reset(NULL);

	to_orig = Z_INTL_TIMEZONE_P(object);
	intl_error_reset(TIMEZONE_ERROR_P(to_orig));

	ret_val = TimeZone_ce_ptr->create_object(Z_OBJCE_P(object));
	to_new  = php_intl_timezone_fetch_object(ret_val);

	zend_objects_clone_members(&to_new->zo, &to_orig->zo);

	if (to_orig->utimezone != NULL) {
		TimeZone	*newTimeZone;

		newTimeZone = to_orig->utimezone->clone();
		to_new->should_delete = 1;
		if (!newTimeZone) {
			zend_string *err_msg;
			intl_errors_set_code(TIMEZONE_ERROR_P(to_orig),
				U_MEMORY_ALLOCATION_ERROR);
			intl_errors_set_custom_msg(TIMEZONE_ERROR_P(to_orig),
				"Could not clone IntlTimeZone", 0);
			err_msg = intl_error_get_message(TIMEZONE_ERROR_P(to_orig));
			zend_throw_exception(NULL, ZSTR_VAL(err_msg), 0);
			zend_string_free(err_msg);
		} else {
			to_new->utimezone = newTimeZone;
		}
	} else {
		zend_throw_exception(NULL, "Cannot clone unconstructed IntlTimeZone", 0);
	}

	return ret_val;
}
/* }}} */

/* {{{ compare_objects handler for TimeZone
 *     Can't be used for >, >=, <, <= comparisons */
static int TimeZone_compare_objects(zval *object1, zval *object2)
{
	TimeZone_object		*to1,
						*to2;
	to1 = Z_INTL_TIMEZONE_P(object1);
	to2 = Z_INTL_TIMEZONE_P(object2);

	if (to1->utimezone == NULL || to2->utimezone == NULL) {
		zend_throw_exception(NULL, "Comparison with at least one unconstructed "
				"IntlTimeZone operand", 0);
		/* intentionally not returning */
	} else {
		if (*to1->utimezone == *to2->utimezone) {
			return 0;
		}
	}

	return 1;
}
/* }}} */

/* {{{ get_debug_info handler for TimeZone */
static HashTable *TimeZone_get_debug_info(zval *object, int *is_temp)
{
	zval			zv;
	TimeZone_object	*to;
	const TimeZone	*tz;
	UnicodeString	ustr;
	zend_string     *u8str;
	HashTable 		*debug_info;
	UErrorCode		uec = U_ZERO_ERROR;

	*is_temp = 1;

	debug_info = zend_new_array(8);

	to = Z_INTL_TIMEZONE_P(object);
	tz = to->utimezone;

	if (tz == NULL) {
		ZVAL_FALSE(&zv);
		zend_hash_str_update(debug_info, "valid", sizeof("valid") - 1, &zv);
		return debug_info;
	}

	ZVAL_TRUE(&zv);
	zend_hash_str_update(debug_info, "valid", sizeof("valid") - 1, &zv);

	tz->getID(ustr);
	u8str = intl_convert_utf16_to_utf8(
		ustr.getBuffer(), ustr.length(), &uec);
	if (!u8str) {
		return debug_info;
	}
	ZVAL_NEW_STR(&zv, u8str);
	zend_hash_str_update(debug_info, "id", sizeof("id") - 1, &zv);

	int32_t rawOffset, dstOffset;
	UDate now = Calendar::getNow();
	tz->getOffset(now, FALSE, rawOffset, dstOffset, uec);
	if (U_FAILURE(uec)) {
		return debug_info;
	}

	ZVAL_LONG(&zv, (zend_long)rawOffset);
	zend_hash_str_update(debug_info,"rawOffset", sizeof("rawOffset") - 1, &zv);
	ZVAL_LONG(&zv, (zend_long)(rawOffset + dstOffset));
	zend_hash_str_update(debug_info,"currentOffset", sizeof("currentOffset") - 1, &zv);

	return debug_info;
}
/* }}} */

/* {{{ void TimeZone_object_init(TimeZone_object* to)
 * Initialize internals of TImeZone_object not specific to zend standard objects.
 */
static void TimeZone_object_init(TimeZone_object *to)
{
	intl_error_init(TIMEZONE_ERROR_P(to));
	to->utimezone = NULL;
	to->should_delete = 0;
}
/* }}} */

/* {{{ TimeZone_objects_dtor */
static void TimeZone_objects_dtor(zend_object *object)
{
	zend_objects_destroy_object(object);
}
/* }}} */

/* {{{ TimeZone_objects_free */
static void TimeZone_objects_free(zend_object *object)
{
	TimeZone_object* to = php_intl_timezone_fetch_object(object);

	if (to->utimezone && to->should_delete) {
		delete to->utimezone;
		to->utimezone = NULL;
	}
	intl_error_reset(TIMEZONE_ERROR_P(to));

	zend_object_std_dtor(&to->zo);
}
/* }}} */

/* {{{ TimeZone_object_create */
static zend_object *TimeZone_object_create(zend_class_entry *ce)
{
	TimeZone_object*	intern;

	intern = (TimeZone_object*)ecalloc(1, sizeof(TimeZone_object) + sizeof(zval) * (ce->default_properties_count - 1));

	zend_object_std_init(&intern->zo, ce);
    object_properties_init(&intern->zo, ce);
	TimeZone_object_init(intern);

	intern->zo.handlers = &TimeZone_handlers;

	return &intern->zo;
}
/* }}} */

/* {{{ TimeZone methods arguments info */

ZEND_BEGIN_ARG_INFO_EX(ainfo_tz_idarg, 0, 0, 1)
	ZEND_ARG_INFO(0, zoneId)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ainfo_tz_fromDateTimeZone, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, otherTimeZone, IntlTimeZone, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ainfo_tz_createEnumeration, 0, 0, 0)
	ZEND_ARG_INFO(0, countryOrRawOffset)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ainfo_tz_countEquivalentIDs, 0, 0, 1)
	ZEND_ARG_INFO(0, zoneId)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ainfo_tz_createTimeZoneIDEnumeration, 0, 0, 1)
	ZEND_ARG_INFO(0, zoneType)
	ZEND_ARG_INFO(0, region)
	ZEND_ARG_INFO(0, rawOffset)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ainfo_tz_getCanonicalID, 0, 0, 1)
	ZEND_ARG_INFO(0, zoneId)
	ZEND_ARG_INFO(1, isSystemID)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ainfo_tz_getEquivalentID, 0, 0, 2)
	ZEND_ARG_INFO(0, zoneId)
	ZEND_ARG_INFO(0, index)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ainfo_tz_getOffset, 0, 0, 4)
	ZEND_ARG_INFO(0, date)
	ZEND_ARG_INFO(0, local)
	ZEND_ARG_INFO(1, rawOffset)
	ZEND_ARG_INFO(1, dstOffset)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ainfo_tz_hasSameRules, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, otherTimeZone, IntlTimeZone, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ainfo_tz_getDisplayName, 0, 0, 0)
	ZEND_ARG_INFO(0, isDaylight)
	ZEND_ARG_INFO(0, style)
	ZEND_ARG_INFO(0, locale)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ainfo_tz_void, 0, 0, 0)
ZEND_END_ARG_INFO()

#if U_ICU_VERSION_MAJOR_NUM >= 52
ZEND_BEGIN_ARG_INFO_EX(ainfo_tz_getWindowsID, 0, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, timezone)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ainfo_tz_getIDForWindowsID, 0, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, timezone)
	ZEND_ARG_INFO(0, region)
ZEND_END_ARG_INFO()
#endif

/* }}} */

/* {{{ TimeZone_class_functions
 * Every 'IntlTimeZone' class method has an entry in this table
 */
static const zend_function_entry TimeZone_class_functions[] = {
	PHP_ME(IntlTimeZone,				__construct,					ainfo_tz_void,				ZEND_ACC_PRIVATE)
	PHP_ME_MAPPING(createTimeZone,		intltz_create_time_zone,		ainfo_tz_idarg,				ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME_MAPPING(fromDateTimeZone,	intltz_from_date_time_zone,		ainfo_tz_idarg,				ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME_MAPPING(createDefault,		intltz_create_default,			ainfo_tz_void,				ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME_MAPPING(getGMT,				intltz_get_gmt,					ainfo_tz_void,				ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
#if U_ICU_VERSION_MAJOR_NUM >= 49
	PHP_ME_MAPPING(getUnknown,			intltz_get_unknown,				ainfo_tz_void,				ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
#endif
	PHP_ME_MAPPING(createEnumeration,	intltz_create_enumeration,		ainfo_tz_createEnumeration,	ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME_MAPPING(countEquivalentIDs,	intltz_count_equivalent_ids,	ainfo_tz_idarg,				ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
#if U_ICU_VERSION_MAJOR_NUM * 10 + U_ICU_VERSION_MINOR_NUM >= 48
	PHP_ME_MAPPING(createTimeZoneIDEnumeration, intltz_create_time_zone_id_enumeration, ainfo_tz_createTimeZoneIDEnumeration, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
#endif
	PHP_ME_MAPPING(getCanonicalID,		intltz_get_canonical_id,		ainfo_tz_getCanonicalID,	ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
#if U_ICU_VERSION_MAJOR_NUM * 10 + U_ICU_VERSION_MINOR_NUM >= 48
	PHP_ME_MAPPING(getRegion,			intltz_get_region,				ainfo_tz_idarg,				ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
#endif
	PHP_ME_MAPPING(getTZDataVersion,	intltz_get_tz_data_version,		ainfo_tz_void,				ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME_MAPPING(getEquivalentID,		intltz_get_equivalent_id,		ainfo_tz_getEquivalentID,	ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)

	PHP_ME_MAPPING(getID,				intltz_get_id,					ainfo_tz_void,				ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(useDaylightTime,		intltz_use_daylight_time,		ainfo_tz_void,				ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(getOffset,			intltz_get_offset,				ainfo_tz_getOffset,			ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(getRawOffset,		intltz_get_raw_offset,			ainfo_tz_void,				ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(hasSameRules,		intltz_has_same_rules,			ainfo_tz_hasSameRules,		ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(getDisplayName,		intltz_get_display_name,		ainfo_tz_getDisplayName,	ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(getDSTSavings,		intltz_get_dst_savings,			ainfo_tz_void,				ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(toDateTimeZone,		intltz_to_date_time_zone,		ainfo_tz_void,				ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(getErrorCode,		intltz_get_error_code,			ainfo_tz_void,				ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(getErrorMessage,		intltz_get_error_message,		ainfo_tz_void,				ZEND_ACC_PUBLIC)
#if U_ICU_VERSION_MAJOR_NUM >= 52
	PHP_ME_MAPPING(getWindowsID,		intltz_get_windows_id,			ainfo_tz_getWindowsID,		ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME_MAPPING(getIDForWindowsID,	intltz_get_id_for_windows_id,		ainfo_tz_getIDForWindowsID,	ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
#endif
	PHP_FE_END
};
/* }}} */

/* {{{ timezone_register_IntlTimeZone_class
 * Initialize 'IntlTimeZone' class
 */
U_CFUNC void timezone_register_IntlTimeZone_class(void)
{
	zend_class_entry ce;

	/* Create and register 'IntlTimeZone' class. */
	INIT_CLASS_ENTRY(ce, "IntlTimeZone", TimeZone_class_functions);
	ce.create_object = TimeZone_object_create;
	TimeZone_ce_ptr = zend_register_internal_class(&ce);
	if (!TimeZone_ce_ptr) {
		//can't happen now without bigger problems before
		php_error_docref0(NULL, E_ERROR,
			"IntlTimeZone: class registration has failed.");
		return;
	}

	memcpy(&TimeZone_handlers, &std_object_handlers,
		sizeof TimeZone_handlers);
	TimeZone_handlers.offset = XtOffsetOf(TimeZone_object, zo);
	TimeZone_handlers.clone_obj = TimeZone_clone_obj;
	TimeZone_handlers.compare_objects = TimeZone_compare_objects;
	TimeZone_handlers.get_debug_info = TimeZone_get_debug_info;
	TimeZone_handlers.dtor_obj = TimeZone_objects_dtor;
	TimeZone_handlers.free_obj = TimeZone_objects_free;


	/* Declare 'IntlTimeZone' class constants */
#define TIMEZONE_DECL_LONG_CONST(name, val) \
	zend_declare_class_constant_long(TimeZone_ce_ptr, name, sizeof(name) - 1, \
		val)

	TIMEZONE_DECL_LONG_CONST("DISPLAY_SHORT", TimeZone::SHORT);
	TIMEZONE_DECL_LONG_CONST("DISPLAY_LONG", TimeZone::LONG);

#if U_ICU_VERSION_MAJOR_NUM * 10 + U_ICU_VERSION_MINOR_NUM >= 44
	TIMEZONE_DECL_LONG_CONST("DISPLAY_SHORT_GENERIC", TimeZone::SHORT_GENERIC);
	TIMEZONE_DECL_LONG_CONST("DISPLAY_LONG_GENERIC", TimeZone::LONG_GENERIC);
	TIMEZONE_DECL_LONG_CONST("DISPLAY_SHORT_GMT", TimeZone::SHORT_GMT);
	TIMEZONE_DECL_LONG_CONST("DISPLAY_LONG_GMT", TimeZone::LONG_GMT);
	TIMEZONE_DECL_LONG_CONST("DISPLAY_SHORT_COMMONLY_USED", TimeZone::SHORT_COMMONLY_USED);
	TIMEZONE_DECL_LONG_CONST("DISPLAY_GENERIC_LOCATION", TimeZone::GENERIC_LOCATION);
#endif

#if U_ICU_VERSION_MAJOR_NUM * 10 + U_ICU_VERSION_MINOR_NUM >= 48
	TIMEZONE_DECL_LONG_CONST("TYPE_ANY", UCAL_ZONE_TYPE_ANY);
	TIMEZONE_DECL_LONG_CONST("TYPE_CANONICAL", UCAL_ZONE_TYPE_CANONICAL);
	TIMEZONE_DECL_LONG_CONST("TYPE_CANONICAL_LOCATION", UCAL_ZONE_TYPE_CANONICAL_LOCATION);
#endif

	/* Declare 'IntlTimeZone' class properties */

}
/* }}} */
