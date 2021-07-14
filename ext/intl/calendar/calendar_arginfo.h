/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 9c4f78dc00000876a28250515573a34b44f082dc */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlCalendar___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlCalendar_createInstance, 0, 0, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, timezone, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, locale, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlCalendar_equals, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, other, IntlCalendar, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlCalendar_fieldDifference, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, timestamp, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, field, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlCalendar_add, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, field, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlCalendar_after arginfo_class_IntlCalendar_equals

#define arginfo_class_IntlCalendar_before arginfo_class_IntlCalendar_equals

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlCalendar_clear, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, field, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlCalendar_fromDateTime, 0, 0, 1)
	ZEND_ARG_OBJ_TYPE_MASK(0, datetime, DateTime, MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, locale, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlCalendar_get, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, field, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlCalendar_getActualMaximum arginfo_class_IntlCalendar_get

#define arginfo_class_IntlCalendar_getActualMinimum arginfo_class_IntlCalendar_get

#define arginfo_class_IntlCalendar_getAvailableLocales arginfo_class_IntlCalendar___construct

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlCalendar_getDayOfWeekType, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, dayOfWeek, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlCalendar_getErrorCode arginfo_class_IntlCalendar___construct

#define arginfo_class_IntlCalendar_getErrorMessage arginfo_class_IntlCalendar___construct

#define arginfo_class_IntlCalendar_getFirstDayOfWeek arginfo_class_IntlCalendar___construct

#define arginfo_class_IntlCalendar_getGreatestMinimum arginfo_class_IntlCalendar_get

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlCalendar_getKeywordValuesForLocale, 0, 0, 3)
	ZEND_ARG_TYPE_INFO(0, keyword, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, onlyCommon, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlCalendar_getLeastMaximum arginfo_class_IntlCalendar_get

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlCalendar_getLocale, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, type, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlCalendar_getMaximum arginfo_class_IntlCalendar_get

#define arginfo_class_IntlCalendar_getMinimalDaysInFirstWeek arginfo_class_IntlCalendar___construct

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlCalendar_setMinimalDaysInFirstWeek, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, days, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlCalendar_getMinimum arginfo_class_IntlCalendar_get

#define arginfo_class_IntlCalendar_getNow arginfo_class_IntlCalendar___construct

#define arginfo_class_IntlCalendar_getRepeatedWallTimeOption arginfo_class_IntlCalendar___construct

#define arginfo_class_IntlCalendar_getSkippedWallTimeOption arginfo_class_IntlCalendar___construct

#define arginfo_class_IntlCalendar_getTime arginfo_class_IntlCalendar___construct

#define arginfo_class_IntlCalendar_getTimeZone arginfo_class_IntlCalendar___construct

#define arginfo_class_IntlCalendar_getType arginfo_class_IntlCalendar___construct

#define arginfo_class_IntlCalendar_getWeekendTransition arginfo_class_IntlCalendar_getDayOfWeekType

#define arginfo_class_IntlCalendar_inDaylightTime arginfo_class_IntlCalendar___construct

#define arginfo_class_IntlCalendar_isEquivalentTo arginfo_class_IntlCalendar_equals

#define arginfo_class_IntlCalendar_isLenient arginfo_class_IntlCalendar___construct

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlCalendar_isWeekend, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timestamp, IS_DOUBLE, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlCalendar_roll, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, field, IS_LONG, 0)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlCalendar_isSet arginfo_class_IntlCalendar_get

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlCalendar_set, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, year, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, month, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, dayOfMonth, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, hour, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, minute, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, second, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlCalendar_setFirstDayOfWeek arginfo_class_IntlCalendar_getDayOfWeekType

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlCalendar_setLenient, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, lenient, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlCalendar_setRepeatedWallTimeOption, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, option, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlCalendar_setSkippedWallTimeOption arginfo_class_IntlCalendar_setRepeatedWallTimeOption

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlCalendar_setTime, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, timestamp, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlCalendar_setTimeZone, 0, 0, 1)
	ZEND_ARG_INFO(0, timezone)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlCalendar_toDateTime arginfo_class_IntlCalendar___construct

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlGregorianCalendar___construct, 0, 0, 0)
	ZEND_ARG_INFO(0, timezoneOrYear)
	ZEND_ARG_INFO(0, localeOrMonth)
	ZEND_ARG_INFO(0, day)
	ZEND_ARG_INFO(0, hour)
	ZEND_ARG_INFO(0, minute)
	ZEND_ARG_INFO(0, second)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlGregorianCalendar_setGregorianChange arginfo_class_IntlCalendar_setTime

#define arginfo_class_IntlGregorianCalendar_getGregorianChange arginfo_class_IntlCalendar___construct

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlGregorianCalendar_isLeapYear, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, year, IS_LONG, 0)
ZEND_END_ARG_INFO()


ZEND_METHOD(IntlCalendar, __construct);
ZEND_FUNCTION(intlcal_create_instance);
ZEND_FUNCTION(intlcal_equals);
ZEND_FUNCTION(intlcal_field_difference);
ZEND_FUNCTION(intlcal_add);
ZEND_FUNCTION(intlcal_after);
ZEND_FUNCTION(intlcal_before);
ZEND_FUNCTION(intlcal_clear);
ZEND_FUNCTION(intlcal_from_date_time);
ZEND_FUNCTION(intlcal_get);
ZEND_FUNCTION(intlcal_get_actual_maximum);
ZEND_FUNCTION(intlcal_get_actual_minimum);
ZEND_FUNCTION(intlcal_get_available_locales);
ZEND_FUNCTION(intlcal_get_day_of_week_type);
ZEND_FUNCTION(intlcal_get_error_code);
ZEND_FUNCTION(intlcal_get_error_message);
ZEND_FUNCTION(intlcal_get_first_day_of_week);
ZEND_FUNCTION(intlcal_get_greatest_minimum);
ZEND_FUNCTION(intlcal_get_keyword_values_for_locale);
ZEND_FUNCTION(intlcal_get_least_maximum);
ZEND_FUNCTION(intlcal_get_locale);
ZEND_FUNCTION(intlcal_get_maximum);
ZEND_FUNCTION(intlcal_get_minimal_days_in_first_week);
ZEND_FUNCTION(intlcal_set_minimal_days_in_first_week);
ZEND_FUNCTION(intlcal_get_minimum);
ZEND_FUNCTION(intlcal_get_now);
ZEND_FUNCTION(intlcal_get_repeated_wall_time_option);
ZEND_FUNCTION(intlcal_get_skipped_wall_time_option);
ZEND_FUNCTION(intlcal_get_time);
ZEND_FUNCTION(intlcal_get_time_zone);
ZEND_FUNCTION(intlcal_get_type);
ZEND_FUNCTION(intlcal_get_weekend_transition);
ZEND_FUNCTION(intlcal_in_daylight_time);
ZEND_FUNCTION(intlcal_is_equivalent_to);
ZEND_FUNCTION(intlcal_is_lenient);
ZEND_FUNCTION(intlcal_is_weekend);
ZEND_FUNCTION(intlcal_roll);
ZEND_FUNCTION(intlcal_is_set);
ZEND_FUNCTION(intlcal_set);
ZEND_FUNCTION(intlcal_set_first_day_of_week);
ZEND_FUNCTION(intlcal_set_lenient);
ZEND_FUNCTION(intlcal_set_repeated_wall_time_option);
ZEND_FUNCTION(intlcal_set_skipped_wall_time_option);
ZEND_FUNCTION(intlcal_set_time);
ZEND_FUNCTION(intlcal_set_time_zone);
ZEND_FUNCTION(intlcal_to_date_time);
ZEND_METHOD(IntlGregorianCalendar, __construct);
ZEND_FUNCTION(intlgregcal_set_gregorian_change);
ZEND_FUNCTION(intlgregcal_get_gregorian_change);
ZEND_FUNCTION(intlgregcal_is_leap_year);


static const zend_function_entry class_IntlCalendar_methods[] = {
	ZEND_ME(IntlCalendar, __construct, arginfo_class_IntlCalendar___construct, ZEND_ACC_PRIVATE)
	ZEND_ME_MAPPING(createInstance, intlcal_create_instance, arginfo_class_IntlCalendar_createInstance, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME_MAPPING(equals, intlcal_equals, arginfo_class_IntlCalendar_equals, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(fieldDifference, intlcal_field_difference, arginfo_class_IntlCalendar_fieldDifference, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(add, intlcal_add, arginfo_class_IntlCalendar_add, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(after, intlcal_after, arginfo_class_IntlCalendar_after, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(before, intlcal_before, arginfo_class_IntlCalendar_before, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(clear, intlcal_clear, arginfo_class_IntlCalendar_clear, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(fromDateTime, intlcal_from_date_time, arginfo_class_IntlCalendar_fromDateTime, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME_MAPPING(get, intlcal_get, arginfo_class_IntlCalendar_get, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getActualMaximum, intlcal_get_actual_maximum, arginfo_class_IntlCalendar_getActualMaximum, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getActualMinimum, intlcal_get_actual_minimum, arginfo_class_IntlCalendar_getActualMinimum, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getAvailableLocales, intlcal_get_available_locales, arginfo_class_IntlCalendar_getAvailableLocales, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME_MAPPING(getDayOfWeekType, intlcal_get_day_of_week_type, arginfo_class_IntlCalendar_getDayOfWeekType, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getErrorCode, intlcal_get_error_code, arginfo_class_IntlCalendar_getErrorCode, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getErrorMessage, intlcal_get_error_message, arginfo_class_IntlCalendar_getErrorMessage, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getFirstDayOfWeek, intlcal_get_first_day_of_week, arginfo_class_IntlCalendar_getFirstDayOfWeek, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getGreatestMinimum, intlcal_get_greatest_minimum, arginfo_class_IntlCalendar_getGreatestMinimum, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getKeywordValuesForLocale, intlcal_get_keyword_values_for_locale, arginfo_class_IntlCalendar_getKeywordValuesForLocale, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME_MAPPING(getLeastMaximum, intlcal_get_least_maximum, arginfo_class_IntlCalendar_getLeastMaximum, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getLocale, intlcal_get_locale, arginfo_class_IntlCalendar_getLocale, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getMaximum, intlcal_get_maximum, arginfo_class_IntlCalendar_getMaximum, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getMinimalDaysInFirstWeek, intlcal_get_minimal_days_in_first_week, arginfo_class_IntlCalendar_getMinimalDaysInFirstWeek, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(setMinimalDaysInFirstWeek, intlcal_set_minimal_days_in_first_week, arginfo_class_IntlCalendar_setMinimalDaysInFirstWeek, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getMinimum, intlcal_get_minimum, arginfo_class_IntlCalendar_getMinimum, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getNow, intlcal_get_now, arginfo_class_IntlCalendar_getNow, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME_MAPPING(getRepeatedWallTimeOption, intlcal_get_repeated_wall_time_option, arginfo_class_IntlCalendar_getRepeatedWallTimeOption, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getSkippedWallTimeOption, intlcal_get_skipped_wall_time_option, arginfo_class_IntlCalendar_getSkippedWallTimeOption, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getTime, intlcal_get_time, arginfo_class_IntlCalendar_getTime, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getTimeZone, intlcal_get_time_zone, arginfo_class_IntlCalendar_getTimeZone, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getType, intlcal_get_type, arginfo_class_IntlCalendar_getType, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getWeekendTransition, intlcal_get_weekend_transition, arginfo_class_IntlCalendar_getWeekendTransition, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(inDaylightTime, intlcal_in_daylight_time, arginfo_class_IntlCalendar_inDaylightTime, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(isEquivalentTo, intlcal_is_equivalent_to, arginfo_class_IntlCalendar_isEquivalentTo, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(isLenient, intlcal_is_lenient, arginfo_class_IntlCalendar_isLenient, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(isWeekend, intlcal_is_weekend, arginfo_class_IntlCalendar_isWeekend, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(roll, intlcal_roll, arginfo_class_IntlCalendar_roll, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(isSet, intlcal_is_set, arginfo_class_IntlCalendar_isSet, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(set, intlcal_set, arginfo_class_IntlCalendar_set, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(setFirstDayOfWeek, intlcal_set_first_day_of_week, arginfo_class_IntlCalendar_setFirstDayOfWeek, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(setLenient, intlcal_set_lenient, arginfo_class_IntlCalendar_setLenient, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(setRepeatedWallTimeOption, intlcal_set_repeated_wall_time_option, arginfo_class_IntlCalendar_setRepeatedWallTimeOption, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(setSkippedWallTimeOption, intlcal_set_skipped_wall_time_option, arginfo_class_IntlCalendar_setSkippedWallTimeOption, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(setTime, intlcal_set_time, arginfo_class_IntlCalendar_setTime, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(setTimeZone, intlcal_set_time_zone, arginfo_class_IntlCalendar_setTimeZone, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(toDateTime, intlcal_to_date_time, arginfo_class_IntlCalendar_toDateTime, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_IntlGregorianCalendar_methods[] = {
	ZEND_ME(IntlGregorianCalendar, __construct, arginfo_class_IntlGregorianCalendar___construct, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(setGregorianChange, intlgregcal_set_gregorian_change, arginfo_class_IntlGregorianCalendar_setGregorianChange, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getGregorianChange, intlgregcal_get_gregorian_change, arginfo_class_IntlGregorianCalendar_getGregorianChange, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(isLeapYear, intlgregcal_is_leap_year, arginfo_class_IntlGregorianCalendar_isLeapYear, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};
