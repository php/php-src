/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 880a93d8a3461635447318317869fa7d3d7762dd */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_strtotime, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, datetime, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, baseTimestamp, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_date, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, format, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timestamp, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_idate, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, format, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timestamp, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_gmdate arginfo_date

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_mktime, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, hour, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, minute, IS_LONG, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, second, IS_LONG, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, month, IS_LONG, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, day, IS_LONG, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, year, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_gmmktime arginfo_mktime

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_checkdate, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, month, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, day, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, year, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_strftime, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, format, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timestamp, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_gmstrftime arginfo_strftime

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_time, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_localtime, 0, 0, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timestamp, IS_LONG, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, associative, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_getdate, 0, 0, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timestamp, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_date_create, 0, 0, DateTime, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, datetime, IS_STRING, 0, "\"now\"")
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, timezone, DateTimeZone, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_date_create_immutable, 0, 0, DateTimeImmutable, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, datetime, IS_STRING, 0, "\"now\"")
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, timezone, DateTimeZone, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_date_create_from_format, 0, 2, DateTime, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, format, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, datetime, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, timezone, DateTimeZone, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_date_create_immutable_from_format, 0, 2, DateTimeImmutable, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, format, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, datetime, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, timezone, DateTimeZone, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_date_parse, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, datetime, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_date_parse_from_format, 0, 2, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, format, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, datetime, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_date_get_last_errors, 0, 0, MAY_BE_ARRAY|MAY_BE_FALSE)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_date_format, 0, 2, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO(0, object, DateTimeInterface, 0)
	ZEND_ARG_TYPE_INFO(0, format, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_date_modify, 0, 2, DateTime, MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, object, DateTime, 0)
	ZEND_ARG_TYPE_INFO(0, modifier, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_date_add, 0, 2, DateTime, 0)
	ZEND_ARG_OBJ_INFO(0, object, DateTime, 0)
	ZEND_ARG_OBJ_INFO(0, interval, DateInterval, 0)
ZEND_END_ARG_INFO()

#define arginfo_date_sub arginfo_date_add

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_date_timezone_get, 0, 1, DateTimeZone, MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, object, DateTimeInterface, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_date_timezone_set, 0, 2, DateTime, 0)
	ZEND_ARG_OBJ_INFO(0, object, DateTime, 0)
	ZEND_ARG_OBJ_INFO(0, timezone, DateTimeZone, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_date_offset_get, 0, 1, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, object, DateTimeInterface, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_date_diff, 0, 2, DateInterval, 0)
	ZEND_ARG_OBJ_INFO(0, baseObject, DateTimeInterface, 0)
	ZEND_ARG_OBJ_INFO(0, targetObject, DateTimeInterface, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, absolute, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_date_time_set, 0, 3, DateTime, 0)
	ZEND_ARG_OBJ_INFO(0, object, DateTime, 0)
	ZEND_ARG_TYPE_INFO(0, hour, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, minute, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, second, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, microsecond, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_date_date_set, 0, 4, DateTime, 0)
	ZEND_ARG_OBJ_INFO(0, object, DateTime, 0)
	ZEND_ARG_TYPE_INFO(0, year, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, month, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, day, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_date_isodate_set, 0, 3, DateTime, 0)
	ZEND_ARG_OBJ_INFO(0, object, DateTime, 0)
	ZEND_ARG_TYPE_INFO(0, year, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, week, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, dayOfWeek, IS_LONG, 0, "1")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_date_timestamp_set, 0, 2, DateTime, 0)
	ZEND_ARG_OBJ_INFO(0, object, DateTime, 0)
	ZEND_ARG_TYPE_INFO(0, timestamp, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_date_timestamp_get arginfo_date_offset_get

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_timezone_open, 0, 1, DateTimeZone, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, timezone, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_timezone_name_get, 0, 1, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO(0, object, DateTimeZone, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_timezone_name_from_abbr, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, abbr, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, utcOffset, IS_LONG, 0, "-1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, isDST, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_timezone_offset_get, 0, 2, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, object, DateTimeZone, 0)
	ZEND_ARG_OBJ_INFO(0, datetime, DateTimeInterface, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_timezone_transitions_get, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, object, DateTimeZone, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timestampBegin, IS_LONG, 0, "PHP_INT_MIN")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timestampEnd, IS_LONG, 0, "PHP_INT_MAX")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_timezone_location_get, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, object, DateTimeZone, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_timezone_identifiers_list, 0, 0, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timezoneGroup, IS_LONG, 0, "DateTimeZone::ALL")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, countryCode, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_timezone_abbreviations_list, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_timezone_version_get, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_date_interval_create_from_date_string, 0, 1, DateInterval, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, datetime, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_date_interval_format, 0, 2, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO(0, object, DateInterval, 0)
	ZEND_ARG_TYPE_INFO(0, format, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_date_default_timezone_set, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, timezoneId, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_date_default_timezone_get arginfo_timezone_version_get

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_date_sunrise, 0, 1, MAY_BE_STRING|MAY_BE_LONG|MAY_BE_DOUBLE|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, timestamp, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, returnFormat, IS_LONG, 0, "SUNFUNCS_RET_STRING")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, latitude, IS_DOUBLE, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, longitude, IS_DOUBLE, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, zenith, IS_DOUBLE, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, utcOffset, IS_DOUBLE, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_date_sunset arginfo_date_sunrise

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_date_sun_info, 0, 3, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, timestamp, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, latitude, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, longitude, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DateTimeInterface_format, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, format, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DateTimeInterface_getTimezone, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DateTimeInterface_getOffset arginfo_class_DateTimeInterface_getTimezone

#define arginfo_class_DateTimeInterface_getTimestamp arginfo_class_DateTimeInterface_getTimezone

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DateTimeInterface_diff, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, targetObject, DateTimeInterface, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, absolute, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

#define arginfo_class_DateTimeInterface___wakeup arginfo_class_DateTimeInterface_getTimezone

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DateTime___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, datetime, IS_STRING, 0, "\"now\"")
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, timezone, DateTimeZone, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_DateTime___wakeup arginfo_class_DateTimeInterface_getTimezone

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DateTime___set_state, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DateTime_createFromImmutable, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, object, DateTimeImmutable, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_DateTime_createFromInterface, 0, 1, DateTime, 0)
	ZEND_ARG_OBJ_INFO(0, object, DateTimeInterface, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DateTime_createFromFormat, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, format, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, datetime, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, timezone, DateTimeZone, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_DateTime_getLastErrors arginfo_class_DateTimeInterface_getTimezone

#define arginfo_class_DateTime_format arginfo_class_DateTimeInterface_format

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DateTime_modify, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, modifier, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DateTime_add, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, interval, DateInterval, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DateTime_sub arginfo_class_DateTime_add

#define arginfo_class_DateTime_getTimezone arginfo_class_DateTimeInterface_getTimezone

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DateTime_setTimezone, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, timezone, DateTimeZone, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DateTime_getOffset arginfo_class_DateTimeInterface_getTimezone

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DateTime_setTime, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, hour, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, minute, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, second, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, microsecond, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DateTime_setDate, 0, 0, 3)
	ZEND_ARG_TYPE_INFO(0, year, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, month, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, day, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DateTime_setISODate, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, year, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, week, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, dayOfWeek, IS_LONG, 0, "1")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DateTime_setTimestamp, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, timestamp, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DateTime_getTimestamp arginfo_class_DateTimeInterface_getTimezone

#define arginfo_class_DateTime_diff arginfo_class_DateTimeInterface_diff

#define arginfo_class_DateTimeImmutable___construct arginfo_class_DateTime___construct

#define arginfo_class_DateTimeImmutable___wakeup arginfo_class_DateTimeInterface_getTimezone

#define arginfo_class_DateTimeImmutable___set_state arginfo_class_DateTime___set_state

#define arginfo_class_DateTimeImmutable_createFromFormat arginfo_class_DateTime_createFromFormat

#define arginfo_class_DateTimeImmutable_getLastErrors arginfo_class_DateTimeInterface_getTimezone

#define arginfo_class_DateTimeImmutable_format arginfo_class_DateTimeInterface_format

#define arginfo_class_DateTimeImmutable_getTimezone arginfo_class_DateTimeInterface_getTimezone

#define arginfo_class_DateTimeImmutable_getOffset arginfo_class_DateTimeInterface_getTimezone

#define arginfo_class_DateTimeImmutable_getTimestamp arginfo_class_DateTimeInterface_getTimezone

#define arginfo_class_DateTimeImmutable_diff arginfo_class_DateTimeInterface_diff

#define arginfo_class_DateTimeImmutable_modify arginfo_class_DateTime_modify

#define arginfo_class_DateTimeImmutable_add arginfo_class_DateTime_add

#define arginfo_class_DateTimeImmutable_sub arginfo_class_DateTime_add

#define arginfo_class_DateTimeImmutable_setTimezone arginfo_class_DateTime_setTimezone

#define arginfo_class_DateTimeImmutable_setTime arginfo_class_DateTime_setTime

#define arginfo_class_DateTimeImmutable_setDate arginfo_class_DateTime_setDate

#define arginfo_class_DateTimeImmutable_setISODate arginfo_class_DateTime_setISODate

#define arginfo_class_DateTimeImmutable_setTimestamp arginfo_class_DateTime_setTimestamp

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DateTimeImmutable_createFromMutable, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, object, DateTime, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_DateTimeImmutable_createFromInterface, 0, 1, DateTimeImmutable, 0)
	ZEND_ARG_OBJ_INFO(0, object, DateTimeInterface, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DateTimeZone___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, timezone, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DateTimeZone_getName arginfo_class_DateTimeInterface_getTimezone

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DateTimeZone_getOffset, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, datetime, DateTimeInterface, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DateTimeZone_getTransitions, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timestampBegin, IS_LONG, 0, "PHP_INT_MIN")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timestampEnd, IS_LONG, 0, "PHP_INT_MAX")
ZEND_END_ARG_INFO()

#define arginfo_class_DateTimeZone_getLocation arginfo_class_DateTimeInterface_getTimezone

#define arginfo_class_DateTimeZone_listAbbreviations arginfo_class_DateTimeInterface_getTimezone

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DateTimeZone_listIdentifiers, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timezoneGroup, IS_LONG, 0, "DateTimeZone::ALL")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, countryCode, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_DateTimeZone___wakeup arginfo_class_DateTimeInterface_getTimezone

#define arginfo_class_DateTimeZone___set_state arginfo_class_DateTime___set_state

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DateInterval___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, duration, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DateInterval_createFromDateString, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, datetime, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DateInterval_format arginfo_class_DateTimeInterface_format

#define arginfo_class_DateInterval___wakeup arginfo_class_DateTimeInterface_getTimezone

#define arginfo_class_DateInterval___set_state arginfo_class_DateTime___set_state

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DatePeriod___construct, 0, 0, 1)
	ZEND_ARG_INFO(0, start)
	ZEND_ARG_INFO(0, interval)
	ZEND_ARG_INFO(0, end)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

#define arginfo_class_DatePeriod_getStartDate arginfo_class_DateTimeInterface_getTimezone

#define arginfo_class_DatePeriod_getEndDate arginfo_class_DateTimeInterface_getTimezone

#define arginfo_class_DatePeriod_getDateInterval arginfo_class_DateTimeInterface_getTimezone

#define arginfo_class_DatePeriod_getRecurrences arginfo_class_DateTimeInterface_getTimezone

#define arginfo_class_DatePeriod___wakeup arginfo_class_DateTimeInterface_getTimezone

#define arginfo_class_DatePeriod___set_state arginfo_class_DateTime___set_state

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_DatePeriod_getIterator, 0, 0, Iterator, 0)
ZEND_END_ARG_INFO()


ZEND_FUNCTION(strtotime);
ZEND_FUNCTION(date);
ZEND_FUNCTION(idate);
ZEND_FUNCTION(gmdate);
ZEND_FUNCTION(mktime);
ZEND_FUNCTION(gmmktime);
ZEND_FUNCTION(checkdate);
ZEND_FUNCTION(strftime);
ZEND_FUNCTION(gmstrftime);
ZEND_FUNCTION(time);
ZEND_FUNCTION(localtime);
ZEND_FUNCTION(getdate);
ZEND_FUNCTION(date_create);
ZEND_FUNCTION(date_create_immutable);
ZEND_FUNCTION(date_create_from_format);
ZEND_FUNCTION(date_create_immutable_from_format);
ZEND_FUNCTION(date_parse);
ZEND_FUNCTION(date_parse_from_format);
ZEND_FUNCTION(date_get_last_errors);
ZEND_FUNCTION(date_format);
ZEND_FUNCTION(date_modify);
ZEND_FUNCTION(date_add);
ZEND_FUNCTION(date_sub);
ZEND_FUNCTION(date_timezone_get);
ZEND_FUNCTION(date_timezone_set);
ZEND_FUNCTION(date_offset_get);
ZEND_FUNCTION(date_diff);
ZEND_FUNCTION(date_time_set);
ZEND_FUNCTION(date_date_set);
ZEND_FUNCTION(date_isodate_set);
ZEND_FUNCTION(date_timestamp_set);
ZEND_FUNCTION(date_timestamp_get);
ZEND_FUNCTION(timezone_open);
ZEND_FUNCTION(timezone_name_get);
ZEND_FUNCTION(timezone_name_from_abbr);
ZEND_FUNCTION(timezone_offset_get);
ZEND_FUNCTION(timezone_transitions_get);
ZEND_FUNCTION(timezone_location_get);
ZEND_FUNCTION(timezone_identifiers_list);
ZEND_FUNCTION(timezone_abbreviations_list);
ZEND_FUNCTION(timezone_version_get);
ZEND_FUNCTION(date_interval_create_from_date_string);
ZEND_FUNCTION(date_interval_format);
ZEND_FUNCTION(date_default_timezone_set);
ZEND_FUNCTION(date_default_timezone_get);
ZEND_FUNCTION(date_sunrise);
ZEND_FUNCTION(date_sunset);
ZEND_FUNCTION(date_sun_info);
ZEND_METHOD(DateTime, __construct);
ZEND_METHOD(DateTime, __wakeup);
ZEND_METHOD(DateTime, __set_state);
ZEND_METHOD(DateTime, createFromImmutable);
ZEND_METHOD(DateTime, createFromInterface);
ZEND_METHOD(DateTimeImmutable, __construct);
ZEND_METHOD(DateTimeImmutable, __wakeup);
ZEND_METHOD(DateTimeImmutable, __set_state);
ZEND_METHOD(DateTimeImmutable, modify);
ZEND_METHOD(DateTimeImmutable, add);
ZEND_METHOD(DateTimeImmutable, sub);
ZEND_METHOD(DateTimeImmutable, setTimezone);
ZEND_METHOD(DateTimeImmutable, setTime);
ZEND_METHOD(DateTimeImmutable, setDate);
ZEND_METHOD(DateTimeImmutable, setISODate);
ZEND_METHOD(DateTimeImmutable, setTimestamp);
ZEND_METHOD(DateTimeImmutable, createFromMutable);
ZEND_METHOD(DateTimeImmutable, createFromInterface);
ZEND_METHOD(DateTimeZone, __construct);
ZEND_METHOD(DateTimeZone, __wakeup);
ZEND_METHOD(DateTimeZone, __set_state);
ZEND_METHOD(DateInterval, __construct);
ZEND_METHOD(DateInterval, __wakeup);
ZEND_METHOD(DateInterval, __set_state);
ZEND_METHOD(DatePeriod, __construct);
ZEND_METHOD(DatePeriod, getStartDate);
ZEND_METHOD(DatePeriod, getEndDate);
ZEND_METHOD(DatePeriod, getDateInterval);
ZEND_METHOD(DatePeriod, getRecurrences);
ZEND_METHOD(DatePeriod, __wakeup);
ZEND_METHOD(DatePeriod, __set_state);
ZEND_METHOD(DatePeriod, getIterator);


static const zend_function_entry ext_functions[] = {
	ZEND_FE(strtotime, arginfo_strtotime)
	ZEND_FE(date, arginfo_date)
	ZEND_FE(idate, arginfo_idate)
	ZEND_FE(gmdate, arginfo_gmdate)
	ZEND_FE(mktime, arginfo_mktime)
	ZEND_FE(gmmktime, arginfo_gmmktime)
	ZEND_FE(checkdate, arginfo_checkdate)
	ZEND_FE(strftime, arginfo_strftime)
	ZEND_FE(gmstrftime, arginfo_gmstrftime)
	ZEND_FE(time, arginfo_time)
	ZEND_FE(localtime, arginfo_localtime)
	ZEND_FE(getdate, arginfo_getdate)
	ZEND_FE(date_create, arginfo_date_create)
	ZEND_FE(date_create_immutable, arginfo_date_create_immutable)
	ZEND_FE(date_create_from_format, arginfo_date_create_from_format)
	ZEND_FE(date_create_immutable_from_format, arginfo_date_create_immutable_from_format)
	ZEND_FE(date_parse, arginfo_date_parse)
	ZEND_FE(date_parse_from_format, arginfo_date_parse_from_format)
	ZEND_FE(date_get_last_errors, arginfo_date_get_last_errors)
	ZEND_FE(date_format, arginfo_date_format)
	ZEND_FE(date_modify, arginfo_date_modify)
	ZEND_FE(date_add, arginfo_date_add)
	ZEND_FE(date_sub, arginfo_date_sub)
	ZEND_FE(date_timezone_get, arginfo_date_timezone_get)
	ZEND_FE(date_timezone_set, arginfo_date_timezone_set)
	ZEND_FE(date_offset_get, arginfo_date_offset_get)
	ZEND_FE(date_diff, arginfo_date_diff)
	ZEND_FE(date_time_set, arginfo_date_time_set)
	ZEND_FE(date_date_set, arginfo_date_date_set)
	ZEND_FE(date_isodate_set, arginfo_date_isodate_set)
	ZEND_FE(date_timestamp_set, arginfo_date_timestamp_set)
	ZEND_FE(date_timestamp_get, arginfo_date_timestamp_get)
	ZEND_FE(timezone_open, arginfo_timezone_open)
	ZEND_FE(timezone_name_get, arginfo_timezone_name_get)
	ZEND_FE(timezone_name_from_abbr, arginfo_timezone_name_from_abbr)
	ZEND_FE(timezone_offset_get, arginfo_timezone_offset_get)
	ZEND_FE(timezone_transitions_get, arginfo_timezone_transitions_get)
	ZEND_FE(timezone_location_get, arginfo_timezone_location_get)
	ZEND_FE(timezone_identifiers_list, arginfo_timezone_identifiers_list)
	ZEND_FE(timezone_abbreviations_list, arginfo_timezone_abbreviations_list)
	ZEND_FE(timezone_version_get, arginfo_timezone_version_get)
	ZEND_FE(date_interval_create_from_date_string, arginfo_date_interval_create_from_date_string)
	ZEND_FE(date_interval_format, arginfo_date_interval_format)
	ZEND_FE(date_default_timezone_set, arginfo_date_default_timezone_set)
	ZEND_FE(date_default_timezone_get, arginfo_date_default_timezone_get)
	ZEND_FE(date_sunrise, arginfo_date_sunrise)
	ZEND_FE(date_sunset, arginfo_date_sunset)
	ZEND_FE(date_sun_info, arginfo_date_sun_info)
	ZEND_FE_END
};


static const zend_function_entry class_DateTimeInterface_methods[] = {
	ZEND_ABSTRACT_ME_WITH_FLAGS(DateTimeInterface, format, arginfo_class_DateTimeInterface_format, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(DateTimeInterface, getTimezone, arginfo_class_DateTimeInterface_getTimezone, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(DateTimeInterface, getOffset, arginfo_class_DateTimeInterface_getOffset, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(DateTimeInterface, getTimestamp, arginfo_class_DateTimeInterface_getTimestamp, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(DateTimeInterface, diff, arginfo_class_DateTimeInterface_diff, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(DateTimeInterface, __wakeup, arginfo_class_DateTimeInterface___wakeup, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_FE_END
};


static const zend_function_entry class_DateTime_methods[] = {
	ZEND_ME(DateTime, __construct, arginfo_class_DateTime___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(DateTime, __wakeup, arginfo_class_DateTime___wakeup, ZEND_ACC_PUBLIC)
	ZEND_ME(DateTime, __set_state, arginfo_class_DateTime___set_state, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(DateTime, createFromImmutable, arginfo_class_DateTime_createFromImmutable, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(DateTime, createFromInterface, arginfo_class_DateTime_createFromInterface, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME_MAPPING(createFromFormat, date_create_from_format, arginfo_class_DateTime_createFromFormat, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME_MAPPING(getLastErrors, date_get_last_errors, arginfo_class_DateTime_getLastErrors, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME_MAPPING(format, date_format, arginfo_class_DateTime_format, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(modify, date_modify, arginfo_class_DateTime_modify, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(add, date_add, arginfo_class_DateTime_add, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(sub, date_sub, arginfo_class_DateTime_sub, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getTimezone, date_timezone_get, arginfo_class_DateTime_getTimezone, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(setTimezone, date_timezone_set, arginfo_class_DateTime_setTimezone, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getOffset, date_offset_get, arginfo_class_DateTime_getOffset, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(setTime, date_time_set, arginfo_class_DateTime_setTime, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(setDate, date_date_set, arginfo_class_DateTime_setDate, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(setISODate, date_isodate_set, arginfo_class_DateTime_setISODate, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(setTimestamp, date_timestamp_set, arginfo_class_DateTime_setTimestamp, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getTimestamp, date_timestamp_get, arginfo_class_DateTime_getTimestamp, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(diff, date_diff, arginfo_class_DateTime_diff, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_DateTimeImmutable_methods[] = {
	ZEND_ME(DateTimeImmutable, __construct, arginfo_class_DateTimeImmutable___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(DateTimeImmutable, __wakeup, arginfo_class_DateTimeImmutable___wakeup, ZEND_ACC_PUBLIC)
	ZEND_ME(DateTimeImmutable, __set_state, arginfo_class_DateTimeImmutable___set_state, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME_MAPPING(createFromFormat, date_create_immutable_from_format, arginfo_class_DateTimeImmutable_createFromFormat, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME_MAPPING(getLastErrors, date_get_last_errors, arginfo_class_DateTimeImmutable_getLastErrors, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME_MAPPING(format, date_format, arginfo_class_DateTimeImmutable_format, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getTimezone, date_timezone_get, arginfo_class_DateTimeImmutable_getTimezone, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getOffset, date_offset_get, arginfo_class_DateTimeImmutable_getOffset, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getTimestamp, date_timestamp_get, arginfo_class_DateTimeImmutable_getTimestamp, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(diff, date_diff, arginfo_class_DateTimeImmutable_diff, ZEND_ACC_PUBLIC)
	ZEND_ME(DateTimeImmutable, modify, arginfo_class_DateTimeImmutable_modify, ZEND_ACC_PUBLIC)
	ZEND_ME(DateTimeImmutable, add, arginfo_class_DateTimeImmutable_add, ZEND_ACC_PUBLIC)
	ZEND_ME(DateTimeImmutable, sub, arginfo_class_DateTimeImmutable_sub, ZEND_ACC_PUBLIC)
	ZEND_ME(DateTimeImmutable, setTimezone, arginfo_class_DateTimeImmutable_setTimezone, ZEND_ACC_PUBLIC)
	ZEND_ME(DateTimeImmutable, setTime, arginfo_class_DateTimeImmutable_setTime, ZEND_ACC_PUBLIC)
	ZEND_ME(DateTimeImmutable, setDate, arginfo_class_DateTimeImmutable_setDate, ZEND_ACC_PUBLIC)
	ZEND_ME(DateTimeImmutable, setISODate, arginfo_class_DateTimeImmutable_setISODate, ZEND_ACC_PUBLIC)
	ZEND_ME(DateTimeImmutable, setTimestamp, arginfo_class_DateTimeImmutable_setTimestamp, ZEND_ACC_PUBLIC)
	ZEND_ME(DateTimeImmutable, createFromMutable, arginfo_class_DateTimeImmutable_createFromMutable, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(DateTimeImmutable, createFromInterface, arginfo_class_DateTimeImmutable_createFromInterface, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_FE_END
};


static const zend_function_entry class_DateTimeZone_methods[] = {
	ZEND_ME(DateTimeZone, __construct, arginfo_class_DateTimeZone___construct, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getName, timezone_name_get, arginfo_class_DateTimeZone_getName, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getOffset, timezone_offset_get, arginfo_class_DateTimeZone_getOffset, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getTransitions, timezone_transitions_get, arginfo_class_DateTimeZone_getTransitions, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getLocation, timezone_location_get, arginfo_class_DateTimeZone_getLocation, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(listAbbreviations, timezone_abbreviations_list, arginfo_class_DateTimeZone_listAbbreviations, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME_MAPPING(listIdentifiers, timezone_identifiers_list, arginfo_class_DateTimeZone_listIdentifiers, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(DateTimeZone, __wakeup, arginfo_class_DateTimeZone___wakeup, ZEND_ACC_PUBLIC)
	ZEND_ME(DateTimeZone, __set_state, arginfo_class_DateTimeZone___set_state, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_FE_END
};


static const zend_function_entry class_DateInterval_methods[] = {
	ZEND_ME(DateInterval, __construct, arginfo_class_DateInterval___construct, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(createFromDateString, date_interval_create_from_date_string, arginfo_class_DateInterval_createFromDateString, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME_MAPPING(format, date_interval_format, arginfo_class_DateInterval_format, ZEND_ACC_PUBLIC)
	ZEND_ME(DateInterval, __wakeup, arginfo_class_DateInterval___wakeup, ZEND_ACC_PUBLIC)
	ZEND_ME(DateInterval, __set_state, arginfo_class_DateInterval___set_state, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_FE_END
};


static const zend_function_entry class_DatePeriod_methods[] = {
	ZEND_ME(DatePeriod, __construct, arginfo_class_DatePeriod___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(DatePeriod, getStartDate, arginfo_class_DatePeriod_getStartDate, ZEND_ACC_PUBLIC)
	ZEND_ME(DatePeriod, getEndDate, arginfo_class_DatePeriod_getEndDate, ZEND_ACC_PUBLIC)
	ZEND_ME(DatePeriod, getDateInterval, arginfo_class_DatePeriod_getDateInterval, ZEND_ACC_PUBLIC)
	ZEND_ME(DatePeriod, getRecurrences, arginfo_class_DatePeriod_getRecurrences, ZEND_ACC_PUBLIC)
	ZEND_ME(DatePeriod, __wakeup, arginfo_class_DatePeriod___wakeup, ZEND_ACC_PUBLIC)
	ZEND_ME(DatePeriod, __set_state, arginfo_class_DatePeriod___set_state, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(DatePeriod, getIterator, arginfo_class_DatePeriod_getIterator, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};
