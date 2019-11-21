/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlCalendar___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlCalendar_createInstance, 0, 0, 0)
	ZEND_ARG_INFO(0, timeZone)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlCalendar_equals, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, calendar, IntlCalendar, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlCalendar_fieldDifference, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, when, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, field, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlCalendar_add, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, field, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, amount, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlCalendar_after arginfo_class_IntlCalendar_equals

#define arginfo_class_IntlCalendar_before arginfo_class_IntlCalendar_equals

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlCalendar_clear, 0, 0, 0)
	ZEND_ARG_TYPE_INFO(0, field, IS_LONG, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlCalendar_fromDateTime, 0, 0, 1)
	ZEND_ARG_INFO(0, dateTime)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 1)
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
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, commonlyUsed, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlCalendar_getLeastMaximum arginfo_class_IntlCalendar_get

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlCalendar_getLocale, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, localeType, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlCalendar_getMaximum arginfo_class_IntlCalendar_get

#define arginfo_class_IntlCalendar_getMinimalDaysInFirstWeek arginfo_class_IntlCalendar___construct

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlCalendar_setMinimalDaysInFirstWeek, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, numberOfDays, IS_LONG, 0)
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
	ZEND_ARG_TYPE_INFO(0, date, IS_DOUBLE, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlCalendar_roll, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, field, IS_LONG, 0)
	ZEND_ARG_INFO(0, amountOrUpOrDown)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlCalendar_isSet arginfo_class_IntlCalendar_get

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlCalendar_set, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, year, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, month, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, dayOfMonth, IS_LONG, 0)
	ZEND_ARG_INFO(0, hour)
	ZEND_ARG_TYPE_INFO(0, minute, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, second, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlCalendar_setFirstDayOfWeek arginfo_class_IntlCalendar_getDayOfWeekType

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlCalendar_setLenient, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, isLenient, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlCalendar_setRepeatedWallTimeOption, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, wallTimeOption, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlCalendar_setSkippedWallTimeOption arginfo_class_IntlCalendar_setRepeatedWallTimeOption

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlCalendar_setTime, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, date, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlCalendar_setTimeZone, 0, 0, 1)
	ZEND_ARG_INFO(0, timeZone)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlCalendar_toDateTime arginfo_class_IntlCalendar___construct

#define arginfo_class_IntlGregorianCalendar___construct arginfo_class_IntlCalendar___construct

#define arginfo_class_IntlGregorianCalendar_createInstance arginfo_class_IntlCalendar_createInstance

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlGregorianCalendar_setGregorianChange, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, change, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlGregorianCalendar_getGregorianChange arginfo_class_IntlCalendar___construct

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlGregorianCalendar_isLeapYear, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, year, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_intlcal_create_instance, 0, 0, IntlCalendar, 1)
	ZEND_ARG_INFO(0, timeZone)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_intlcal_get_keyword_values_for_locale, 0, 3, Iterator, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, commonlyUsed, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_intlcal_get_now, 0, 0, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_intlcal_get_available_locales, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_intlcal_get, 0, 2, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, calendar, IntlCalendar, 0)
	ZEND_ARG_TYPE_INFO(0, field, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_intlcal_get_time, 0, 1, MAY_BE_DOUBLE|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, calendar, IntlCalendar, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_intlcal_set_time, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, calendar, IntlCalendar, 0)
	ZEND_ARG_TYPE_INFO(0, date, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_intlcal_add, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, calendar, IntlCalendar, 0)
	ZEND_ARG_TYPE_INFO(0, field, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, amount, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_intlcal_set_time_zone, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, calendar, IntlCalendar, 0)
	ZEND_ARG_INFO(0, timeZone)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_intlcal_after, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, calendarObject, IntlCalendar, 0)
	ZEND_ARG_OBJ_INFO(0, calendar, IntlCalendar, 0)
ZEND_END_ARG_INFO()

#define arginfo_intlcal_before arginfo_intlcal_after

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_intlcal_set, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, calendar, IntlCalendar, 0)
	ZEND_ARG_TYPE_INFO(0, year, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, month, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, dayOfMonth, IS_LONG, 0)
	ZEND_ARG_INFO(0, hour)
	ZEND_ARG_TYPE_INFO(0, minute, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, second, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_intlcal_roll, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, calendar, IntlCalendar, 0)
	ZEND_ARG_TYPE_INFO(0, field, IS_LONG, 0)
	ZEND_ARG_INFO(0, amountOrUpOrDown)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_intlcal_clear, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, calendar, IntlCalendar, 0)
	ZEND_ARG_TYPE_INFO(0, field, IS_LONG, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_intlcal_field_difference, 0, 3, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, calendar, IntlCalendar, 0)
	ZEND_ARG_TYPE_INFO(0, when, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, field, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_intlcal_get_actual_maximum arginfo_intlcal_get

#define arginfo_intlcal_get_actual_minimum arginfo_intlcal_get

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_intlcal_get_day_of_week_type, 0, 2, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, calendar, IntlCalendar, 0)
	ZEND_ARG_TYPE_INFO(0, dayOfWeek, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_intlcal_get_first_day_of_week, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, calendar, IntlCalendar, 0)
ZEND_END_ARG_INFO()

#define arginfo_intlcal_greatest_minimum arginfo_intlcal_get

#define arginfo_intlcal_get_least_maximum arginfo_intlcal_get

#define arginfo_intlcal_get_greatest_minimum arginfo_intlcal_get

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_intlcal_get_locale, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, calendar, IntlCalendar, 0)
	ZEND_ARG_TYPE_INFO(0, localeType, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_intlcal_get_maximum arginfo_intlcal_get

#define arginfo_intlcal_get_minimal_days_in_first_week arginfo_intlcal_get_first_day_of_week

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_intlcal_set_minimal_days_in_first_week, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, calendar, IntlCalendar, 0)
	ZEND_ARG_TYPE_INFO(0, numberOfDays, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_intlcal_get_minimum arginfo_intlcal_get

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_intlcal_get_time_zone, 0, 1, IntlTimeZone, MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, calendar, IntlCalendar, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_intlcal_get_type, 0, 1, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO(0, calendar, IntlCalendar, 0)
ZEND_END_ARG_INFO()

#define arginfo_intlcal_get_weekend_transition arginfo_intlcal_get_day_of_week_type

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_intlcal_in_daylight_time, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, calendar, IntlCalendar, 0)
ZEND_END_ARG_INFO()

#define arginfo_intlcal_is_lenient arginfo_intlcal_in_daylight_time

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_intlcal_is_set, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, calendar, IntlCalendar, 0)
	ZEND_ARG_TYPE_INFO(0, field, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_intlcal_is_equivalent_to arginfo_intlcal_after

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_intlcal_is_weekend, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, calendar, IntlCalendar, 0)
	ZEND_ARG_TYPE_INFO(0, date, IS_DOUBLE, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_intlcal_set_first_day_of_week, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, calendar, IntlCalendar, 0)
	ZEND_ARG_TYPE_INFO(0, dayOfWeek, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_intlcal_set_lenient, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, calendar, IntlCalendar, 0)
	ZEND_ARG_TYPE_INFO(0, isLenient, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_intlcal_get_repeated_wall_time_option, 0, 1, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, calendar, IntlCalendar, 0)
ZEND_END_ARG_INFO()

#define arginfo_intlcal_equals arginfo_intlcal_after

#define arginfo_intlcal_get_skipped_wall_time_option arginfo_intlcal_get_repeated_wall_time_option

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_intlcal_set_repeated_wall_time_option, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, calendar, IntlCalendar, 0)
	ZEND_ARG_TYPE_INFO(0, wallTimeOption, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_intlcal_set_skipped_wall_time_option arginfo_intlcal_set_repeated_wall_time_option

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_intlcal_from_date_time, 0, 1, IntlCalendar, 1)
	ZEND_ARG_INFO(0, dateTime)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_intlcal_to_date_time, 0, 1, DateTime, MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, calendar, IntlCalendar, 0)
ZEND_END_ARG_INFO()

#define arginfo_intlcal_get_error_code arginfo_intlcal_get_first_day_of_week

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_intlcal_get_error_message, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, calendar, IntlCalendar, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_intlgregcal_create_instance, 0, 0, IntlGregorianCalendar, 1)
	ZEND_ARG_INFO(0, timeZone)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_intlgregcal_set_gregorian_change, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, calendar, IntlGregorianCalendar, 0)
	ZEND_ARG_TYPE_INFO(0, change, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_intlgregcal_get_gregorian_change, 0, 1, MAY_BE_DOUBLE|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, calendar, IntlGregorianCalendar, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_intlgregcal_is_leap_year, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, calendar, IntlGregorianCalendar, 0)
	ZEND_ARG_TYPE_INFO(0, year, IS_LONG, 0)
ZEND_END_ARG_INFO()
