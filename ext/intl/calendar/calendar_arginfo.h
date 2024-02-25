/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: dbd7f8dd82cfdca04988aa791523b29b564347e0 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlCalendar___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_INFO_EX(arginfo_class_IntlCalendar_createInstance, 0, 0, IntlCalendar, 1)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, timezone, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, locale, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_IntlCalendar_equals, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, other, IntlCalendar, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_IntlCalendar_fieldDifference, 0, 2, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, timestamp, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, field, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_IntlCalendar_add, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, field, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlCalendar_after arginfo_class_IntlCalendar_equals

#define arginfo_class_IntlCalendar_before arginfo_class_IntlCalendar_equals

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlCalendar_clear, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, field, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_INFO_EX(arginfo_class_IntlCalendar_fromDateTime, 0, 1, IntlCalendar, 1)
	ZEND_ARG_OBJ_TYPE_MASK(0, datetime, DateTime, MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, locale, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_IntlCalendar_get, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, field, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlCalendar_getActualMaximum arginfo_class_IntlCalendar_get

#define arginfo_class_IntlCalendar_getActualMinimum arginfo_class_IntlCalendar_get

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_IntlCalendar_getAvailableLocales, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_IntlCalendar_getDayOfWeekType, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, dayOfWeek, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_IntlCalendar_getErrorCode, 0, 0, MAY_BE_LONG|MAY_BE_FALSE)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_IntlCalendar_getErrorMessage, 0, 0, MAY_BE_STRING|MAY_BE_FALSE)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlCalendar_getFirstDayOfWeek arginfo_class_IntlCalendar_getErrorCode

#define arginfo_class_IntlCalendar_getGreatestMinimum arginfo_class_IntlCalendar_get

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_IntlCalendar_getKeywordValuesForLocale, 0, 3, IntlIterator, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, keyword, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, onlyCommon, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlCalendar_getLeastMaximum arginfo_class_IntlCalendar_get

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_IntlCalendar_getLocale, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, type, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlCalendar_getMaximum arginfo_class_IntlCalendar_get

#define arginfo_class_IntlCalendar_getMinimalDaysInFirstWeek arginfo_class_IntlCalendar_getErrorCode

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlCalendar_setMinimalDaysInFirstWeek, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, days, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlCalendar_getMinimum arginfo_class_IntlCalendar_get

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_IntlCalendar_getNow, 0, 0, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_IntlCalendar_getRepeatedWallTimeOption, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlCalendar_getSkippedWallTimeOption arginfo_class_IntlCalendar_getRepeatedWallTimeOption

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_IntlCalendar_getTime, 0, 0, MAY_BE_DOUBLE|MAY_BE_FALSE)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_IntlCalendar_getTimeZone, 0, 0, IntlTimeZone, MAY_BE_FALSE)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_IntlCalendar_getType, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlCalendar_getWeekendTransition arginfo_class_IntlCalendar_getDayOfWeekType

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_IntlCalendar_inDaylightTime, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlCalendar_isEquivalentTo arginfo_class_IntlCalendar_equals

#define arginfo_class_IntlCalendar_isLenient arginfo_class_IntlCalendar_inDaylightTime

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_IntlCalendar_isWeekend, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timestamp, IS_DOUBLE, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_IntlCalendar_roll, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, field, IS_LONG, 0)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_IntlCalendar_isSet, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, field, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlCalendar_set, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, year, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, month, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, dayOfMonth, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, hour, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, minute, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, second, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_IntlCalendar_setDate, 0, 3, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, year, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, month, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, dayOfMonth, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_IntlCalendar_setDateTime, 0, 5, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, year, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, month, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, dayOfMonth, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, hour, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, minute, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, second, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlCalendar_setFirstDayOfWeek, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, dayOfWeek, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlCalendar_setLenient, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, lenient, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlCalendar_setRepeatedWallTimeOption, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, option, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlCalendar_setSkippedWallTimeOption arginfo_class_IntlCalendar_setRepeatedWallTimeOption

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_IntlCalendar_setTime, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, timestamp, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_IntlCalendar_setTimeZone, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, timezone)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_IntlCalendar_toDateTime, 0, 0, DateTime, MAY_BE_FALSE)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_IntlGregorianCalendar_createFromDate, 0, 3, IS_STATIC, 0)
	ZEND_ARG_TYPE_INFO(0, year, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, month, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, dayOfMonth, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_IntlGregorianCalendar_createFromDateTime, 0, 5, IS_STATIC, 0)
	ZEND_ARG_TYPE_INFO(0, year, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, month, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, dayOfMonth, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, hour, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, minute, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, second, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlGregorianCalendar___construct, 0, 0, 0)
	ZEND_ARG_INFO(0, timezoneOrYear)
	ZEND_ARG_INFO(0, localeOrMonth)
	ZEND_ARG_INFO(0, day)
	ZEND_ARG_INFO(0, hour)
	ZEND_ARG_INFO(0, minute)
	ZEND_ARG_INFO(0, second)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlGregorianCalendar_setGregorianChange arginfo_class_IntlCalendar_setTime

#define arginfo_class_IntlGregorianCalendar_getGregorianChange arginfo_class_IntlCalendar_getNow

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_IntlGregorianCalendar_isLeapYear, 0, 1, _IS_BOOL, 0)
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
ZEND_METHOD(IntlCalendar, setDate);
ZEND_METHOD(IntlCalendar, setDateTime);
ZEND_FUNCTION(intlcal_set_first_day_of_week);
ZEND_FUNCTION(intlcal_set_lenient);
ZEND_FUNCTION(intlcal_set_repeated_wall_time_option);
ZEND_FUNCTION(intlcal_set_skipped_wall_time_option);
ZEND_FUNCTION(intlcal_set_time);
ZEND_FUNCTION(intlcal_set_time_zone);
ZEND_FUNCTION(intlcal_to_date_time);
ZEND_METHOD(IntlGregorianCalendar, createFromDate);
ZEND_METHOD(IntlGregorianCalendar, createFromDateTime);
ZEND_METHOD(IntlGregorianCalendar, __construct);
ZEND_FUNCTION(intlgregcal_set_gregorian_change);
ZEND_FUNCTION(intlgregcal_get_gregorian_change);
ZEND_FUNCTION(intlgregcal_is_leap_year);

static const zend_function_entry class_IntlCalendar_methods[] = {
	ZEND_ME(IntlCalendar, __construct, arginfo_class_IntlCalendar___construct, ZEND_ACC_PRIVATE)
	ZEND_RAW_FENTRY("createInstance", zif_intlcal_create_instance, arginfo_class_IntlCalendar_createInstance, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC, NULL, NULL)
	ZEND_RAW_FENTRY("equals", zif_intlcal_equals, arginfo_class_IntlCalendar_equals, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("fieldDifference", zif_intlcal_field_difference, arginfo_class_IntlCalendar_fieldDifference, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("add", zif_intlcal_add, arginfo_class_IntlCalendar_add, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("after", zif_intlcal_after, arginfo_class_IntlCalendar_after, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("before", zif_intlcal_before, arginfo_class_IntlCalendar_before, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("clear", zif_intlcal_clear, arginfo_class_IntlCalendar_clear, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("fromDateTime", zif_intlcal_from_date_time, arginfo_class_IntlCalendar_fromDateTime, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC, NULL, NULL)
	ZEND_RAW_FENTRY("get", zif_intlcal_get, arginfo_class_IntlCalendar_get, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("getActualMaximum", zif_intlcal_get_actual_maximum, arginfo_class_IntlCalendar_getActualMaximum, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("getActualMinimum", zif_intlcal_get_actual_minimum, arginfo_class_IntlCalendar_getActualMinimum, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("getAvailableLocales", zif_intlcal_get_available_locales, arginfo_class_IntlCalendar_getAvailableLocales, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC, NULL, NULL)
	ZEND_RAW_FENTRY("getDayOfWeekType", zif_intlcal_get_day_of_week_type, arginfo_class_IntlCalendar_getDayOfWeekType, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("getErrorCode", zif_intlcal_get_error_code, arginfo_class_IntlCalendar_getErrorCode, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("getErrorMessage", zif_intlcal_get_error_message, arginfo_class_IntlCalendar_getErrorMessage, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("getFirstDayOfWeek", zif_intlcal_get_first_day_of_week, arginfo_class_IntlCalendar_getFirstDayOfWeek, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("getGreatestMinimum", zif_intlcal_get_greatest_minimum, arginfo_class_IntlCalendar_getGreatestMinimum, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("getKeywordValuesForLocale", zif_intlcal_get_keyword_values_for_locale, arginfo_class_IntlCalendar_getKeywordValuesForLocale, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC, NULL, NULL)
	ZEND_RAW_FENTRY("getLeastMaximum", zif_intlcal_get_least_maximum, arginfo_class_IntlCalendar_getLeastMaximum, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("getLocale", zif_intlcal_get_locale, arginfo_class_IntlCalendar_getLocale, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("getMaximum", zif_intlcal_get_maximum, arginfo_class_IntlCalendar_getMaximum, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("getMinimalDaysInFirstWeek", zif_intlcal_get_minimal_days_in_first_week, arginfo_class_IntlCalendar_getMinimalDaysInFirstWeek, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("setMinimalDaysInFirstWeek", zif_intlcal_set_minimal_days_in_first_week, arginfo_class_IntlCalendar_setMinimalDaysInFirstWeek, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("getMinimum", zif_intlcal_get_minimum, arginfo_class_IntlCalendar_getMinimum, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("getNow", zif_intlcal_get_now, arginfo_class_IntlCalendar_getNow, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC, NULL, NULL)
	ZEND_RAW_FENTRY("getRepeatedWallTimeOption", zif_intlcal_get_repeated_wall_time_option, arginfo_class_IntlCalendar_getRepeatedWallTimeOption, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("getSkippedWallTimeOption", zif_intlcal_get_skipped_wall_time_option, arginfo_class_IntlCalendar_getSkippedWallTimeOption, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("getTime", zif_intlcal_get_time, arginfo_class_IntlCalendar_getTime, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("getTimeZone", zif_intlcal_get_time_zone, arginfo_class_IntlCalendar_getTimeZone, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("getType", zif_intlcal_get_type, arginfo_class_IntlCalendar_getType, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("getWeekendTransition", zif_intlcal_get_weekend_transition, arginfo_class_IntlCalendar_getWeekendTransition, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("inDaylightTime", zif_intlcal_in_daylight_time, arginfo_class_IntlCalendar_inDaylightTime, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("isEquivalentTo", zif_intlcal_is_equivalent_to, arginfo_class_IntlCalendar_isEquivalentTo, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("isLenient", zif_intlcal_is_lenient, arginfo_class_IntlCalendar_isLenient, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("isWeekend", zif_intlcal_is_weekend, arginfo_class_IntlCalendar_isWeekend, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("roll", zif_intlcal_roll, arginfo_class_IntlCalendar_roll, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("isSet", zif_intlcal_is_set, arginfo_class_IntlCalendar_isSet, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("set", zif_intlcal_set, arginfo_class_IntlCalendar_set, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_ME(IntlCalendar, setDate, arginfo_class_IntlCalendar_setDate, ZEND_ACC_PUBLIC)
	ZEND_ME(IntlCalendar, setDateTime, arginfo_class_IntlCalendar_setDateTime, ZEND_ACC_PUBLIC)
	ZEND_RAW_FENTRY("setFirstDayOfWeek", zif_intlcal_set_first_day_of_week, arginfo_class_IntlCalendar_setFirstDayOfWeek, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("setLenient", zif_intlcal_set_lenient, arginfo_class_IntlCalendar_setLenient, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("setRepeatedWallTimeOption", zif_intlcal_set_repeated_wall_time_option, arginfo_class_IntlCalendar_setRepeatedWallTimeOption, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("setSkippedWallTimeOption", zif_intlcal_set_skipped_wall_time_option, arginfo_class_IntlCalendar_setSkippedWallTimeOption, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("setTime", zif_intlcal_set_time, arginfo_class_IntlCalendar_setTime, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("setTimeZone", zif_intlcal_set_time_zone, arginfo_class_IntlCalendar_setTimeZone, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("toDateTime", zif_intlcal_to_date_time, arginfo_class_IntlCalendar_toDateTime, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_IntlGregorianCalendar_methods[] = {
	ZEND_ME(IntlGregorianCalendar, createFromDate, arginfo_class_IntlGregorianCalendar_createFromDate, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlGregorianCalendar, createFromDateTime, arginfo_class_IntlGregorianCalendar_createFromDateTime, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlGregorianCalendar, __construct, arginfo_class_IntlGregorianCalendar___construct, ZEND_ACC_PUBLIC)
	ZEND_RAW_FENTRY("setGregorianChange", zif_intlgregcal_set_gregorian_change, arginfo_class_IntlGregorianCalendar_setGregorianChange, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("getGregorianChange", zif_intlgregcal_get_gregorian_change, arginfo_class_IntlGregorianCalendar_getGregorianChange, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("isLeapYear", zif_intlgregcal_is_leap_year, arginfo_class_IntlGregorianCalendar_isLeapYear, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_FE_END
};

static zend_class_entry *register_class_IntlCalendar(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "IntlCalendar", class_IntlCalendar_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_NOT_SERIALIZABLE;

	zval const_FIELD_ERA_value;
	ZVAL_LONG(&const_FIELD_ERA_value, UCAL_ERA);
	zend_string *const_FIELD_ERA_name = zend_string_init_interned("FIELD_ERA", sizeof("FIELD_ERA") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FIELD_ERA_name, &const_FIELD_ERA_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FIELD_ERA_name);

	zval const_FIELD_YEAR_value;
	ZVAL_LONG(&const_FIELD_YEAR_value, UCAL_YEAR);
	zend_string *const_FIELD_YEAR_name = zend_string_init_interned("FIELD_YEAR", sizeof("FIELD_YEAR") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FIELD_YEAR_name, &const_FIELD_YEAR_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FIELD_YEAR_name);

	zval const_FIELD_MONTH_value;
	ZVAL_LONG(&const_FIELD_MONTH_value, UCAL_MONTH);
	zend_string *const_FIELD_MONTH_name = zend_string_init_interned("FIELD_MONTH", sizeof("FIELD_MONTH") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FIELD_MONTH_name, &const_FIELD_MONTH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FIELD_MONTH_name);

	zval const_FIELD_WEEK_OF_YEAR_value;
	ZVAL_LONG(&const_FIELD_WEEK_OF_YEAR_value, UCAL_WEEK_OF_YEAR);
	zend_string *const_FIELD_WEEK_OF_YEAR_name = zend_string_init_interned("FIELD_WEEK_OF_YEAR", sizeof("FIELD_WEEK_OF_YEAR") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FIELD_WEEK_OF_YEAR_name, &const_FIELD_WEEK_OF_YEAR_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FIELD_WEEK_OF_YEAR_name);

	zval const_FIELD_WEEK_OF_MONTH_value;
	ZVAL_LONG(&const_FIELD_WEEK_OF_MONTH_value, UCAL_WEEK_OF_MONTH);
	zend_string *const_FIELD_WEEK_OF_MONTH_name = zend_string_init_interned("FIELD_WEEK_OF_MONTH", sizeof("FIELD_WEEK_OF_MONTH") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FIELD_WEEK_OF_MONTH_name, &const_FIELD_WEEK_OF_MONTH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FIELD_WEEK_OF_MONTH_name);

	zval const_FIELD_DATE_value;
	ZVAL_LONG(&const_FIELD_DATE_value, UCAL_DATE);
	zend_string *const_FIELD_DATE_name = zend_string_init_interned("FIELD_DATE", sizeof("FIELD_DATE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FIELD_DATE_name, &const_FIELD_DATE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FIELD_DATE_name);

	zval const_FIELD_DAY_OF_YEAR_value;
	ZVAL_LONG(&const_FIELD_DAY_OF_YEAR_value, UCAL_DAY_OF_YEAR);
	zend_string *const_FIELD_DAY_OF_YEAR_name = zend_string_init_interned("FIELD_DAY_OF_YEAR", sizeof("FIELD_DAY_OF_YEAR") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FIELD_DAY_OF_YEAR_name, &const_FIELD_DAY_OF_YEAR_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FIELD_DAY_OF_YEAR_name);

	zval const_FIELD_DAY_OF_WEEK_value;
	ZVAL_LONG(&const_FIELD_DAY_OF_WEEK_value, UCAL_DAY_OF_WEEK);
	zend_string *const_FIELD_DAY_OF_WEEK_name = zend_string_init_interned("FIELD_DAY_OF_WEEK", sizeof("FIELD_DAY_OF_WEEK") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FIELD_DAY_OF_WEEK_name, &const_FIELD_DAY_OF_WEEK_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FIELD_DAY_OF_WEEK_name);

	zval const_FIELD_DAY_OF_WEEK_IN_MONTH_value;
	ZVAL_LONG(&const_FIELD_DAY_OF_WEEK_IN_MONTH_value, UCAL_DAY_OF_WEEK_IN_MONTH);
	zend_string *const_FIELD_DAY_OF_WEEK_IN_MONTH_name = zend_string_init_interned("FIELD_DAY_OF_WEEK_IN_MONTH", sizeof("FIELD_DAY_OF_WEEK_IN_MONTH") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FIELD_DAY_OF_WEEK_IN_MONTH_name, &const_FIELD_DAY_OF_WEEK_IN_MONTH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FIELD_DAY_OF_WEEK_IN_MONTH_name);

	zval const_FIELD_AM_PM_value;
	ZVAL_LONG(&const_FIELD_AM_PM_value, UCAL_AM_PM);
	zend_string *const_FIELD_AM_PM_name = zend_string_init_interned("FIELD_AM_PM", sizeof("FIELD_AM_PM") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FIELD_AM_PM_name, &const_FIELD_AM_PM_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FIELD_AM_PM_name);

	zval const_FIELD_HOUR_value;
	ZVAL_LONG(&const_FIELD_HOUR_value, UCAL_HOUR);
	zend_string *const_FIELD_HOUR_name = zend_string_init_interned("FIELD_HOUR", sizeof("FIELD_HOUR") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FIELD_HOUR_name, &const_FIELD_HOUR_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FIELD_HOUR_name);

	zval const_FIELD_HOUR_OF_DAY_value;
	ZVAL_LONG(&const_FIELD_HOUR_OF_DAY_value, UCAL_HOUR_OF_DAY);
	zend_string *const_FIELD_HOUR_OF_DAY_name = zend_string_init_interned("FIELD_HOUR_OF_DAY", sizeof("FIELD_HOUR_OF_DAY") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FIELD_HOUR_OF_DAY_name, &const_FIELD_HOUR_OF_DAY_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FIELD_HOUR_OF_DAY_name);

	zval const_FIELD_MINUTE_value;
	ZVAL_LONG(&const_FIELD_MINUTE_value, UCAL_MINUTE);
	zend_string *const_FIELD_MINUTE_name = zend_string_init_interned("FIELD_MINUTE", sizeof("FIELD_MINUTE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FIELD_MINUTE_name, &const_FIELD_MINUTE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FIELD_MINUTE_name);

	zval const_FIELD_SECOND_value;
	ZVAL_LONG(&const_FIELD_SECOND_value, UCAL_SECOND);
	zend_string *const_FIELD_SECOND_name = zend_string_init_interned("FIELD_SECOND", sizeof("FIELD_SECOND") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FIELD_SECOND_name, &const_FIELD_SECOND_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FIELD_SECOND_name);

	zval const_FIELD_MILLISECOND_value;
	ZVAL_LONG(&const_FIELD_MILLISECOND_value, UCAL_MILLISECOND);
	zend_string *const_FIELD_MILLISECOND_name = zend_string_init_interned("FIELD_MILLISECOND", sizeof("FIELD_MILLISECOND") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FIELD_MILLISECOND_name, &const_FIELD_MILLISECOND_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FIELD_MILLISECOND_name);

	zval const_FIELD_ZONE_OFFSET_value;
	ZVAL_LONG(&const_FIELD_ZONE_OFFSET_value, UCAL_ZONE_OFFSET);
	zend_string *const_FIELD_ZONE_OFFSET_name = zend_string_init_interned("FIELD_ZONE_OFFSET", sizeof("FIELD_ZONE_OFFSET") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FIELD_ZONE_OFFSET_name, &const_FIELD_ZONE_OFFSET_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FIELD_ZONE_OFFSET_name);

	zval const_FIELD_DST_OFFSET_value;
	ZVAL_LONG(&const_FIELD_DST_OFFSET_value, UCAL_DST_OFFSET);
	zend_string *const_FIELD_DST_OFFSET_name = zend_string_init_interned("FIELD_DST_OFFSET", sizeof("FIELD_DST_OFFSET") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FIELD_DST_OFFSET_name, &const_FIELD_DST_OFFSET_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FIELD_DST_OFFSET_name);

	zval const_FIELD_YEAR_WOY_value;
	ZVAL_LONG(&const_FIELD_YEAR_WOY_value, UCAL_YEAR_WOY);
	zend_string *const_FIELD_YEAR_WOY_name = zend_string_init_interned("FIELD_YEAR_WOY", sizeof("FIELD_YEAR_WOY") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FIELD_YEAR_WOY_name, &const_FIELD_YEAR_WOY_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FIELD_YEAR_WOY_name);

	zval const_FIELD_DOW_LOCAL_value;
	ZVAL_LONG(&const_FIELD_DOW_LOCAL_value, UCAL_DOW_LOCAL);
	zend_string *const_FIELD_DOW_LOCAL_name = zend_string_init_interned("FIELD_DOW_LOCAL", sizeof("FIELD_DOW_LOCAL") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FIELD_DOW_LOCAL_name, &const_FIELD_DOW_LOCAL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FIELD_DOW_LOCAL_name);

	zval const_FIELD_EXTENDED_YEAR_value;
	ZVAL_LONG(&const_FIELD_EXTENDED_YEAR_value, UCAL_EXTENDED_YEAR);
	zend_string *const_FIELD_EXTENDED_YEAR_name = zend_string_init_interned("FIELD_EXTENDED_YEAR", sizeof("FIELD_EXTENDED_YEAR") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FIELD_EXTENDED_YEAR_name, &const_FIELD_EXTENDED_YEAR_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FIELD_EXTENDED_YEAR_name);

	zval const_FIELD_JULIAN_DAY_value;
	ZVAL_LONG(&const_FIELD_JULIAN_DAY_value, UCAL_JULIAN_DAY);
	zend_string *const_FIELD_JULIAN_DAY_name = zend_string_init_interned("FIELD_JULIAN_DAY", sizeof("FIELD_JULIAN_DAY") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FIELD_JULIAN_DAY_name, &const_FIELD_JULIAN_DAY_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FIELD_JULIAN_DAY_name);

	zval const_FIELD_MILLISECONDS_IN_DAY_value;
	ZVAL_LONG(&const_FIELD_MILLISECONDS_IN_DAY_value, UCAL_MILLISECONDS_IN_DAY);
	zend_string *const_FIELD_MILLISECONDS_IN_DAY_name = zend_string_init_interned("FIELD_MILLISECONDS_IN_DAY", sizeof("FIELD_MILLISECONDS_IN_DAY") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FIELD_MILLISECONDS_IN_DAY_name, &const_FIELD_MILLISECONDS_IN_DAY_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FIELD_MILLISECONDS_IN_DAY_name);

	zval const_FIELD_IS_LEAP_MONTH_value;
	ZVAL_LONG(&const_FIELD_IS_LEAP_MONTH_value, UCAL_IS_LEAP_MONTH);
	zend_string *const_FIELD_IS_LEAP_MONTH_name = zend_string_init_interned("FIELD_IS_LEAP_MONTH", sizeof("FIELD_IS_LEAP_MONTH") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FIELD_IS_LEAP_MONTH_name, &const_FIELD_IS_LEAP_MONTH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FIELD_IS_LEAP_MONTH_name);

	zval const_FIELD_FIELD_COUNT_value;
	ZVAL_LONG(&const_FIELD_FIELD_COUNT_value, UCAL_FIELD_COUNT);
	zend_string *const_FIELD_FIELD_COUNT_name = zend_string_init_interned("FIELD_FIELD_COUNT", sizeof("FIELD_FIELD_COUNT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FIELD_FIELD_COUNT_name, &const_FIELD_FIELD_COUNT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FIELD_FIELD_COUNT_name);

	zval const_FIELD_DAY_OF_MONTH_value;
	ZVAL_LONG(&const_FIELD_DAY_OF_MONTH_value, UCAL_DAY_OF_MONTH);
	zend_string *const_FIELD_DAY_OF_MONTH_name = zend_string_init_interned("FIELD_DAY_OF_MONTH", sizeof("FIELD_DAY_OF_MONTH") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FIELD_DAY_OF_MONTH_name, &const_FIELD_DAY_OF_MONTH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FIELD_DAY_OF_MONTH_name);

	zval const_DOW_SUNDAY_value;
	ZVAL_LONG(&const_DOW_SUNDAY_value, UCAL_SUNDAY);
	zend_string *const_DOW_SUNDAY_name = zend_string_init_interned("DOW_SUNDAY", sizeof("DOW_SUNDAY") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_DOW_SUNDAY_name, &const_DOW_SUNDAY_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_DOW_SUNDAY_name);

	zval const_DOW_MONDAY_value;
	ZVAL_LONG(&const_DOW_MONDAY_value, UCAL_MONDAY);
	zend_string *const_DOW_MONDAY_name = zend_string_init_interned("DOW_MONDAY", sizeof("DOW_MONDAY") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_DOW_MONDAY_name, &const_DOW_MONDAY_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_DOW_MONDAY_name);

	zval const_DOW_TUESDAY_value;
	ZVAL_LONG(&const_DOW_TUESDAY_value, UCAL_TUESDAY);
	zend_string *const_DOW_TUESDAY_name = zend_string_init_interned("DOW_TUESDAY", sizeof("DOW_TUESDAY") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_DOW_TUESDAY_name, &const_DOW_TUESDAY_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_DOW_TUESDAY_name);

	zval const_DOW_WEDNESDAY_value;
	ZVAL_LONG(&const_DOW_WEDNESDAY_value, UCAL_WEDNESDAY);
	zend_string *const_DOW_WEDNESDAY_name = zend_string_init_interned("DOW_WEDNESDAY", sizeof("DOW_WEDNESDAY") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_DOW_WEDNESDAY_name, &const_DOW_WEDNESDAY_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_DOW_WEDNESDAY_name);

	zval const_DOW_THURSDAY_value;
	ZVAL_LONG(&const_DOW_THURSDAY_value, UCAL_THURSDAY);
	zend_string *const_DOW_THURSDAY_name = zend_string_init_interned("DOW_THURSDAY", sizeof("DOW_THURSDAY") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_DOW_THURSDAY_name, &const_DOW_THURSDAY_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_DOW_THURSDAY_name);

	zval const_DOW_FRIDAY_value;
	ZVAL_LONG(&const_DOW_FRIDAY_value, UCAL_FRIDAY);
	zend_string *const_DOW_FRIDAY_name = zend_string_init_interned("DOW_FRIDAY", sizeof("DOW_FRIDAY") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_DOW_FRIDAY_name, &const_DOW_FRIDAY_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_DOW_FRIDAY_name);

	zval const_DOW_SATURDAY_value;
	ZVAL_LONG(&const_DOW_SATURDAY_value, UCAL_SATURDAY);
	zend_string *const_DOW_SATURDAY_name = zend_string_init_interned("DOW_SATURDAY", sizeof("DOW_SATURDAY") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_DOW_SATURDAY_name, &const_DOW_SATURDAY_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_DOW_SATURDAY_name);

	zval const_DOW_TYPE_WEEKDAY_value;
	ZVAL_LONG(&const_DOW_TYPE_WEEKDAY_value, UCAL_WEEKDAY);
	zend_string *const_DOW_TYPE_WEEKDAY_name = zend_string_init_interned("DOW_TYPE_WEEKDAY", sizeof("DOW_TYPE_WEEKDAY") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_DOW_TYPE_WEEKDAY_name, &const_DOW_TYPE_WEEKDAY_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_DOW_TYPE_WEEKDAY_name);

	zval const_DOW_TYPE_WEEKEND_value;
	ZVAL_LONG(&const_DOW_TYPE_WEEKEND_value, UCAL_WEEKEND);
	zend_string *const_DOW_TYPE_WEEKEND_name = zend_string_init_interned("DOW_TYPE_WEEKEND", sizeof("DOW_TYPE_WEEKEND") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_DOW_TYPE_WEEKEND_name, &const_DOW_TYPE_WEEKEND_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_DOW_TYPE_WEEKEND_name);

	zval const_DOW_TYPE_WEEKEND_OFFSET_value;
	ZVAL_LONG(&const_DOW_TYPE_WEEKEND_OFFSET_value, UCAL_WEEKEND_ONSET);
	zend_string *const_DOW_TYPE_WEEKEND_OFFSET_name = zend_string_init_interned("DOW_TYPE_WEEKEND_OFFSET", sizeof("DOW_TYPE_WEEKEND_OFFSET") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_DOW_TYPE_WEEKEND_OFFSET_name, &const_DOW_TYPE_WEEKEND_OFFSET_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_DOW_TYPE_WEEKEND_OFFSET_name);

	zval const_DOW_TYPE_WEEKEND_CEASE_value;
	ZVAL_LONG(&const_DOW_TYPE_WEEKEND_CEASE_value, UCAL_WEEKEND_CEASE);
	zend_string *const_DOW_TYPE_WEEKEND_CEASE_name = zend_string_init_interned("DOW_TYPE_WEEKEND_CEASE", sizeof("DOW_TYPE_WEEKEND_CEASE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_DOW_TYPE_WEEKEND_CEASE_name, &const_DOW_TYPE_WEEKEND_CEASE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_DOW_TYPE_WEEKEND_CEASE_name);

	zval const_WALLTIME_FIRST_value;
	ZVAL_LONG(&const_WALLTIME_FIRST_value, UCAL_WALLTIME_FIRST);
	zend_string *const_WALLTIME_FIRST_name = zend_string_init_interned("WALLTIME_FIRST", sizeof("WALLTIME_FIRST") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_WALLTIME_FIRST_name, &const_WALLTIME_FIRST_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_WALLTIME_FIRST_name);

	zval const_WALLTIME_LAST_value;
	ZVAL_LONG(&const_WALLTIME_LAST_value, UCAL_WALLTIME_LAST);
	zend_string *const_WALLTIME_LAST_name = zend_string_init_interned("WALLTIME_LAST", sizeof("WALLTIME_LAST") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_WALLTIME_LAST_name, &const_WALLTIME_LAST_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_WALLTIME_LAST_name);

	zval const_WALLTIME_NEXT_VALID_value;
	ZVAL_LONG(&const_WALLTIME_NEXT_VALID_value, UCAL_WALLTIME_NEXT_VALID);
	zend_string *const_WALLTIME_NEXT_VALID_name = zend_string_init_interned("WALLTIME_NEXT_VALID", sizeof("WALLTIME_NEXT_VALID") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_WALLTIME_NEXT_VALID_name, &const_WALLTIME_NEXT_VALID_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_WALLTIME_NEXT_VALID_name);

	return class_entry;
}

static zend_class_entry *register_class_IntlGregorianCalendar(zend_class_entry *class_entry_IntlCalendar)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "IntlGregorianCalendar", class_IntlGregorianCalendar_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_IntlCalendar);
	class_entry->ce_flags |= ZEND_ACC_NOT_SERIALIZABLE;

	return class_entry;
}
