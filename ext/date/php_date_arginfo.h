/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_strtotime, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, time, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, now, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_date, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, format, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, timestamp, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_idate, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, format, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, timestamp, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_gmdate arginfo_date

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_mktime, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, hour, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, min, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, sec, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, mon, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, day, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, year, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_gmmktime arginfo_mktime

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_checkdate, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, m, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, d, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, y, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_strftime, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, format, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, timestamp, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_gmstrftime arginfo_strftime

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_time, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_localtime, 0, 0, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, timestamp, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, associative, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_getdate, 0, 0, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, timestamp, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_date_create, 0, 0, DateTime, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, time, IS_STRING, 0, "\"now\"")
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, timezone, DateTimeZone, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_date_create_immutable, 0, 0, DateTimeImmutable, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, time, IS_STRING, 0, "\"now\"")
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, timezone, DateTimeZone, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_date_create_from_format, 0, 2, DateTime, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, format, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, time, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, timezone, DateTimeZone, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_date_create_immutable_from_format, 0, 2, DateTimeImmutable, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, format, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, time, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, timezone, DateTimeZone, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_date_parse, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, date, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_date_parse_from_format, 0, 2, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, format, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, date, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_date_get_last_errors, 0, 0, MAY_BE_ARRAY|MAY_BE_FALSE)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_date_format, 0, 2, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO(0, object, DateTimeInterface, 0)
	ZEND_ARG_TYPE_INFO(0, format, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_date_modify, 0, 2, DateTime, MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, object, DateTime, 0)
	ZEND_ARG_TYPE_INFO(0, modify, IS_STRING, 0)
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
	ZEND_ARG_OBJ_INFO(0, object, DateTimeInterface, 0)
	ZEND_ARG_OBJ_INFO(0, timezone, DateTimeZone, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_date_offset_get, 0, 1, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, object, DateTimeInterface, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_date_diff, 0, 2, DateInterval, 0)
	ZEND_ARG_OBJ_INFO(0, object, DateTimeInterface, 0)
	ZEND_ARG_OBJ_INFO(0, object2, DateTimeInterface, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, absolute, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_date_time_set, 0, 3, DateTime, 0)
	ZEND_ARG_OBJ_INFO(0, object, DateTime, 0)
	ZEND_ARG_TYPE_INFO(0, hour, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, minute, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, second, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, microseconds, IS_LONG, 0, "0")
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
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, day, IS_LONG, 0, "1")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_date_timestamp_set, 0, 2, DateTime, 0)
	ZEND_ARG_OBJ_INFO(0, object, DateTime, 0)
	ZEND_ARG_TYPE_INFO(0, timestamp, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_date_timestamp_get, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, object, DateTimeInterface, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_timezone_open, 0, 1, DateTimeZone, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, timezone, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_timezone_name_get, 0, 1, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO(0, object, DateTimeZone, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_timezone_name_from_abbr, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, abbr, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, gmtoffset, IS_LONG, 0, "-1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, isdst, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_timezone_offset_get, 0, 2, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, object, DateTimeZone, 0)
	ZEND_ARG_OBJ_INFO(0, datetime, DateTimeInterface, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_timezone_transitions_get, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, object, DateTimeZone, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timestamp_begin, IS_LONG, 0, "PHP_INT_MIN")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timestamp_end, IS_LONG, 0, "PHP_INT_MAX")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_timezone_location_get, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, object, DateTimeZone, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_timezone_identifiers_list, 0, 0, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, what, IS_LONG, 0, "DateTimeZone::ALL")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, country, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_timezone_abbreviations_list, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_timezone_version_get, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_date_interval_create_from_date_string, 0, 1, DateInterval, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, time, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_date_interval_format, 0, 2, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO(0, object, DateInterval, 0)
	ZEND_ARG_TYPE_INFO(0, format, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_date_default_timezone_set, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, timezone_identifier, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_date_default_timezone_get arginfo_timezone_version_get

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_date_sunrise, 0, 1, MAY_BE_STRING|MAY_BE_LONG|MAY_BE_DOUBLE|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, time, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, retformat, IS_LONG, 0, "SUNFUNCS_RET_STRING")
	ZEND_ARG_TYPE_INFO(0, latitude, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, longitude, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, zenith, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, gmt_offset, IS_DOUBLE, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_date_sunset arginfo_date_sunrise

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_date_sun_info, 0, 3, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, time, IS_LONG, 0)
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
	ZEND_ARG_OBJ_INFO(0, object, DateTimeInterface, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, absolute, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

#define arginfo_class_DateTimeInterface___wakeup arginfo_class_DateTimeInterface_getTimezone

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DateTime___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, time, IS_STRING, 0, "\"now\"")
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, timezone, DateTimeZone, 1, "null")
ZEND_END_ARG_INFO()

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
	ZEND_ARG_TYPE_INFO(0, time, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, timezone, DateTimeZone, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_DateTime_getLastErrors arginfo_class_DateTimeInterface_getTimezone

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DateTime_modify, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, modify, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DateTime_add, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, interval, DateInterval, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DateTime_sub arginfo_class_DateTime_add

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DateTime_setTimezone, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, timezone, DateTimeZone, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DateTime_setTime, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, hour, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, minute, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, second, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, microseconds, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DateTime_setDate, 0, 0, 3)
	ZEND_ARG_TYPE_INFO(0, year, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, month, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, day, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DateTime_setISODate, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, year, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, week, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, day, IS_LONG, 0, "1")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DateTime_setTimestamp, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, timestamp, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DateTimeImmutable___construct arginfo_class_DateTime___construct

#define arginfo_class_DateTimeImmutable___set_state arginfo_class_DateTime___set_state

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DateTimeImmutable_createFromMutable, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, object, DateTime, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_DateTimeImmutable_createFromInterface, 0, 1, DateTimeImmutable, 0)
	ZEND_ARG_OBJ_INFO(0, object, DateTimeInterface, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DateTimeImmutable_createFromFormat arginfo_class_DateTime_createFromFormat

#define arginfo_class_DateTimeImmutable_getLastErrors arginfo_class_DateTimeInterface_getTimezone

#define arginfo_class_DateTimeImmutable_modify arginfo_class_DateTime_modify

#define arginfo_class_DateTimeImmutable_add arginfo_class_DateTime_add

#define arginfo_class_DateTimeImmutable_sub arginfo_class_DateTime_add

#define arginfo_class_DateTimeImmutable_setTimezone arginfo_class_DateTime_setTimezone

#define arginfo_class_DateTimeImmutable_setTime arginfo_class_DateTime_setTime

#define arginfo_class_DateTimeImmutable_setDate arginfo_class_DateTime_setDate

#define arginfo_class_DateTimeImmutable_setISODate arginfo_class_DateTime_setISODate

#define arginfo_class_DateTimeImmutable_setTimestamp arginfo_class_DateTime_setTimestamp

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DateTimeZone___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, timezone, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DateTimeZone_getName arginfo_class_DateTimeInterface_getTimezone

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DateTimeZone_getOffset, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, datetime, DateTimeInterface, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DateTimeZone_getTransitions, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timestamp_begin, IS_LONG, 0, "PHP_INT_MIN")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timestamp_end, IS_LONG, 0, "PHP_INT_MAX")
ZEND_END_ARG_INFO()

#define arginfo_class_DateTimeZone_getLocation arginfo_class_DateTimeInterface_getTimezone

#define arginfo_class_DateTimeZone_listAbbreviations arginfo_class_DateTimeInterface_getTimezone

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DateTimeZone_listIdentifiers, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, what, IS_LONG, 0, "DateTimeZone::ALL")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, country, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_DateTimeZone___wakeup arginfo_class_DateTimeInterface_getTimezone

#define arginfo_class_DateTimeZone___set_state arginfo_class_DateTime___set_state

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DateInterval___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, interval_spec, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DateInterval_createFromDateString, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, time, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DateInterval_format arginfo_class_DateTimeInterface_format

#define arginfo_class_DateInterval___wakeup arginfo_class_DateTimeInterface_getTimezone

#define arginfo_class_DateInterval___set_state arginfo_class_DateTime___set_state

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DatePeriod___construct, 0, 0, 1)
	ZEND_ARG_INFO(0, start)
	ZEND_ARG_INFO(0, interval)
	ZEND_ARG_INFO(0, end)
ZEND_END_ARG_INFO()

#define arginfo_class_DatePeriod_getStartDate arginfo_class_DateTimeInterface_getTimezone

#define arginfo_class_DatePeriod_getEndDate arginfo_class_DateTimeInterface_getTimezone

#define arginfo_class_DatePeriod_getDateInterval arginfo_class_DateTimeInterface_getTimezone

#define arginfo_class_DatePeriod_getRecurrences arginfo_class_DateTimeInterface_getTimezone

#define arginfo_class_DatePeriod___wakeup arginfo_class_DateTimeInterface_getTimezone

#define arginfo_class_DatePeriod___set_state arginfo_class_DateTime___set_state


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
