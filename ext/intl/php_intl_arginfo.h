/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 50cc9edef7f40e6885be38be25a71f66d7405a50 */

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_intlcal_create_instance, 0, 0, IntlCalendar, 1)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, timezone, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, locale, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_intlcal_get_keyword_values_for_locale, 0, 3, IntlIterator, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, keyword, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, onlyCommon, _IS_BOOL, 0)
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
	ZEND_ARG_TYPE_INFO(0, timestamp, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_intlcal_add, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, calendar, IntlCalendar, 0)
	ZEND_ARG_TYPE_INFO(0, field, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_intlcal_set_time_zone, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, calendar, IntlCalendar, 0)
	ZEND_ARG_INFO(0, timezone)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_intlcal_after, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, calendar, IntlCalendar, 0)
	ZEND_ARG_OBJ_INFO(0, other, IntlCalendar, 0)
ZEND_END_ARG_INFO()

#define arginfo_intlcal_before arginfo_intlcal_after

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_intlcal_set, 0, 3, IS_TRUE, 0)
	ZEND_ARG_OBJ_INFO(0, calendar, IntlCalendar, 0)
	ZEND_ARG_TYPE_INFO(0, year, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, month, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, dayOfMonth, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, hour, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, minute, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, second, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_intlcal_roll, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, calendar, IntlCalendar, 0)
	ZEND_ARG_TYPE_INFO(0, field, IS_LONG, 0)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_intlcal_clear, 0, 1, IS_TRUE, 0)
	ZEND_ARG_OBJ_INFO(0, calendar, IntlCalendar, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, field, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_intlcal_field_difference, 0, 3, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, calendar, IntlCalendar, 0)
	ZEND_ARG_TYPE_INFO(0, timestamp, IS_DOUBLE, 0)
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

#define arginfo_intlcal_get_least_maximum arginfo_intlcal_get

#define arginfo_intlcal_get_greatest_minimum arginfo_intlcal_get

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_intlcal_get_locale, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, calendar, IntlCalendar, 0)
	ZEND_ARG_TYPE_INFO(0, type, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_intlcal_get_maximum arginfo_intlcal_get

#define arginfo_intlcal_get_minimal_days_in_first_week arginfo_intlcal_get_first_day_of_week

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_intlcal_set_minimal_days_in_first_week, 0, 2, IS_TRUE, 0)
	ZEND_ARG_OBJ_INFO(0, calendar, IntlCalendar, 0)
	ZEND_ARG_TYPE_INFO(0, days, IS_LONG, 0)
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
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timestamp, IS_DOUBLE, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_intlcal_set_first_day_of_week, 0, 2, IS_TRUE, 0)
	ZEND_ARG_OBJ_INFO(0, calendar, IntlCalendar, 0)
	ZEND_ARG_TYPE_INFO(0, dayOfWeek, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_intlcal_set_lenient, 0, 2, IS_TRUE, 0)
	ZEND_ARG_OBJ_INFO(0, calendar, IntlCalendar, 0)
	ZEND_ARG_TYPE_INFO(0, lenient, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_intlcal_get_repeated_wall_time_option, 0, 1, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, calendar, IntlCalendar, 0)
ZEND_END_ARG_INFO()

#define arginfo_intlcal_equals arginfo_intlcal_after

#define arginfo_intlcal_get_skipped_wall_time_option arginfo_intlcal_get_repeated_wall_time_option

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_intlcal_set_repeated_wall_time_option, 0, 2, IS_TRUE, 0)
	ZEND_ARG_OBJ_INFO(0, calendar, IntlCalendar, 0)
	ZEND_ARG_TYPE_INFO(0, option, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_intlcal_set_skipped_wall_time_option arginfo_intlcal_set_repeated_wall_time_option

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_intlcal_from_date_time, 0, 1, IntlCalendar, 1)
	ZEND_ARG_OBJ_TYPE_MASK(0, datetime, DateTime, MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, locale, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_intlcal_to_date_time, 0, 1, DateTime, MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, calendar, IntlCalendar, 0)
ZEND_END_ARG_INFO()

#define arginfo_intlcal_get_error_code arginfo_intlcal_get_first_day_of_week

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_intlcal_get_error_message, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, calendar, IntlCalendar, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_intlgregcal_create_instance, 0, 0, IntlGregorianCalendar, 1)
	ZEND_ARG_INFO(0, timezoneOrYear)
	ZEND_ARG_INFO(0, localeOrMonth)
	ZEND_ARG_INFO(0, day)
	ZEND_ARG_INFO(0, hour)
	ZEND_ARG_INFO(0, minute)
	ZEND_ARG_INFO(0, second)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_intlgregcal_set_gregorian_change, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, calendar, IntlGregorianCalendar, 0)
	ZEND_ARG_TYPE_INFO(0, timestamp, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_intlgregcal_get_gregorian_change, 0, 1, IS_DOUBLE, 0)
	ZEND_ARG_OBJ_INFO(0, calendar, IntlGregorianCalendar, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_intlgregcal_is_leap_year, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, calendar, IntlGregorianCalendar, 0)
	ZEND_ARG_TYPE_INFO(0, year, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_collator_create, 0, 1, Collator, 1)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_collator_compare, 0, 3, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, object, Collator, 0)
	ZEND_ARG_TYPE_INFO(0, string1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string2, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_collator_get_attribute, 0, 2, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, object, Collator, 0)
	ZEND_ARG_TYPE_INFO(0, attribute, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_collator_set_attribute, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, object, Collator, 0)
	ZEND_ARG_TYPE_INFO(0, attribute, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_collator_get_strength, 0, 1, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, object, Collator, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_collator_set_strength, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, object, Collator, 0)
	ZEND_ARG_TYPE_INFO(0, strength, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_collator_sort, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, object, Collator, 0)
	ZEND_ARG_TYPE_INFO(1, array, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "Collator::SORT_REGULAR")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_collator_sort_with_sort_keys, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, object, Collator, 0)
	ZEND_ARG_TYPE_INFO(1, array, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_collator_asort arginfo_collator_sort

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_collator_get_locale, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, object, Collator, 0)
	ZEND_ARG_TYPE_INFO(0, type, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_collator_get_error_code, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, object, Collator, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_collator_get_error_message, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, object, Collator, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_collator_get_sort_key, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, object, Collator, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_intl_get_error_code, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_intl_get_error_message, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_intl_is_failure, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, errorCode, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_intl_error_name, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, errorCode, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_datefmt_create, 0, 1, IntlDateFormatter, 1)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, dateType, IS_LONG, 0, "IntlDateFormatter::FULL")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timeType, IS_LONG, 0, "IntlDateFormatter::FULL")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, timezone, "null")
	ZEND_ARG_OBJ_TYPE_MASK(0, calendar, IntlCalendar, MAY_BE_LONG|MAY_BE_NULL, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, pattern, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_datefmt_get_datetype, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, formatter, IntlDateFormatter, 0)
ZEND_END_ARG_INFO()

#define arginfo_datefmt_get_timetype arginfo_datefmt_get_datetype

#define arginfo_datefmt_get_calendar arginfo_datefmt_get_datetype

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_datefmt_set_calendar, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, formatter, IntlDateFormatter, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, calendar, IntlCalendar, MAY_BE_LONG|MAY_BE_NULL, NULL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_datefmt_get_timezone_id, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, formatter, IntlDateFormatter, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_datefmt_get_calendar_object, 0, 1, IntlCalendar, MAY_BE_FALSE|MAY_BE_NULL)
	ZEND_ARG_OBJ_INFO(0, formatter, IntlDateFormatter, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_datefmt_get_timezone, 0, 1, IntlTimeZone, MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, formatter, IntlDateFormatter, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_datefmt_set_timezone, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, formatter, IntlDateFormatter, 0)
	ZEND_ARG_INFO(0, timezone)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_datefmt_set_pattern, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, formatter, IntlDateFormatter, 0)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_datefmt_get_pattern arginfo_datefmt_get_timezone_id

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_datefmt_get_locale, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, formatter, IntlDateFormatter, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, type, IS_LONG, 0, "ULOC_ACTUAL_LOCALE")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_datefmt_set_lenient, 0, 2, IS_VOID, 0)
	ZEND_ARG_OBJ_INFO(0, formatter, IntlDateFormatter, 0)
	ZEND_ARG_TYPE_INFO(0, lenient, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_datefmt_is_lenient, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, formatter, IntlDateFormatter, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_datefmt_format, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, formatter, IntlDateFormatter, 0)
	ZEND_ARG_INFO(0, datetime)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_datefmt_format_object, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, datetime)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, format, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, locale, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_datefmt_parse, 0, 2, MAY_BE_LONG|MAY_BE_DOUBLE|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, formatter, IntlDateFormatter, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, offset, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_datefmt_localtime, 0, 2, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, formatter, IntlDateFormatter, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, offset, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_datefmt_get_error_code, 0, 1, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, formatter, IntlDateFormatter, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_datefmt_get_error_message, 0, 1, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO(0, formatter, IntlDateFormatter, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_numfmt_create, 0, 2, NumberFormatter, 1)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, style, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, pattern, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_numfmt_format, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, formatter, NumberFormatter, 0)
	ZEND_ARG_TYPE_MASK(0, num, MAY_BE_LONG|MAY_BE_DOUBLE, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, type, IS_LONG, 0, "NumberFormatter::TYPE_DEFAULT")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_numfmt_parse, 0, 2, MAY_BE_LONG|MAY_BE_DOUBLE|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, formatter, NumberFormatter, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, type, IS_LONG, 0, "NumberFormatter::TYPE_DOUBLE")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, offset, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_numfmt_format_currency, 0, 3, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, formatter, NumberFormatter, 0)
	ZEND_ARG_TYPE_INFO(0, amount, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, currency, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_numfmt_parse_currency, 0, 3, MAY_BE_DOUBLE|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, formatter, NumberFormatter, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_INFO(1, currency)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, offset, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_numfmt_set_attribute, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, formatter, NumberFormatter, 0)
	ZEND_ARG_TYPE_INFO(0, attribute, IS_LONG, 0)
	ZEND_ARG_TYPE_MASK(0, value, MAY_BE_LONG|MAY_BE_DOUBLE, NULL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_numfmt_get_attribute, 0, 2, MAY_BE_LONG|MAY_BE_DOUBLE|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, formatter, NumberFormatter, 0)
	ZEND_ARG_TYPE_INFO(0, attribute, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_numfmt_set_text_attribute, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, formatter, NumberFormatter, 0)
	ZEND_ARG_TYPE_INFO(0, attribute, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_numfmt_get_text_attribute, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, formatter, NumberFormatter, 0)
	ZEND_ARG_TYPE_INFO(0, attribute, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_numfmt_set_symbol, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, formatter, NumberFormatter, 0)
	ZEND_ARG_TYPE_INFO(0, symbol, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_numfmt_get_symbol, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, formatter, NumberFormatter, 0)
	ZEND_ARG_TYPE_INFO(0, symbol, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_numfmt_set_pattern, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, formatter, NumberFormatter, 0)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_numfmt_get_pattern, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, formatter, NumberFormatter, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_numfmt_get_locale, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, formatter, NumberFormatter, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, type, IS_LONG, 0, "ULOC_ACTUAL_LOCALE")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_numfmt_get_error_code, 0, 1, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, formatter, NumberFormatter, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_numfmt_get_error_message, 0, 1, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO(0, formatter, NumberFormatter, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_grapheme_strlen, 0, 1, MAY_BE_LONG|MAY_BE_FALSE|MAY_BE_NULL)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_grapheme_strpos, 0, 2, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, haystack, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, needle, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_grapheme_stripos arginfo_grapheme_strpos

#define arginfo_grapheme_strrpos arginfo_grapheme_strpos

#define arginfo_grapheme_strripos arginfo_grapheme_strpos

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_grapheme_substr, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_grapheme_strstr, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, haystack, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, needle, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, beforeNeedle, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

#define arginfo_grapheme_stristr arginfo_grapheme_strstr

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_grapheme_extract, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, haystack, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, size, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, type, IS_LONG, 0, "GRAPHEME_EXTR_COUNT")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 0, "0")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, next, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_idn_to_ascii, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, domain, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "IDNA_DEFAULT")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, variant, IS_LONG, 0, "INTL_IDNA_VARIANT_UTS46")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, idna_info, "null")
ZEND_END_ARG_INFO()

#define arginfo_idn_to_utf8 arginfo_idn_to_ascii

#define arginfo_locale_get_default arginfo_intl_get_error_message

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_locale_set_default, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_locale_get_primary_language, 0, 1, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_locale_get_script arginfo_locale_get_primary_language

#define arginfo_locale_get_region arginfo_locale_get_primary_language

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_locale_get_keywords, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE|MAY_BE_NULL)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_locale_get_display_script, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, displayLocale, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_locale_get_display_region arginfo_locale_get_display_script

#define arginfo_locale_get_display_name arginfo_locale_get_display_script

#define arginfo_locale_get_display_language arginfo_locale_get_display_script

#define arginfo_locale_get_display_variant arginfo_locale_get_display_script

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_locale_compose, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, subtags, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_locale_parse, 0, 1, IS_ARRAY, 1)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_locale_get_all_variants arginfo_locale_parse

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_locale_filter_matches, 0, 2, _IS_BOOL, 1)
	ZEND_ARG_TYPE_INFO(0, languageTag, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, canonicalize, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

#define arginfo_locale_canonicalize arginfo_locale_get_primary_language

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_locale_lookup, 0, 2, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, languageTag, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, canonicalize, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, defaultLocale, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_locale_accept_from_http, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, header, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_msgfmt_create, 0, 2, MessageFormatter, 1)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_msgfmt_format, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, formatter, MessageFormatter, 0)
	ZEND_ARG_TYPE_INFO(0, values, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_msgfmt_format_message, 0, 3, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, values, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_msgfmt_parse, 0, 2, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, formatter, MessageFormatter, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_msgfmt_parse_message, 0, 3, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, message, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_msgfmt_set_pattern, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, formatter, MessageFormatter, 0)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_msgfmt_get_pattern, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, formatter, MessageFormatter, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_msgfmt_get_locale, 0, 1, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO(0, formatter, MessageFormatter, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_msgfmt_get_error_code, 0, 1, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, formatter, MessageFormatter, 0)
ZEND_END_ARG_INFO()

#define arginfo_msgfmt_get_error_message arginfo_msgfmt_get_locale

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_normalizer_normalize, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, form, IS_LONG, 0, "Normalizer::FORM_C")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_normalizer_is_normalized, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, form, IS_LONG, 0, "Normalizer::FORM_C")
ZEND_END_ARG_INFO()

#if U_ICU_VERSION_MAJOR_NUM >= 56
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_normalizer_get_raw_decomposition, 0, 1, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, form, IS_LONG, 0, "Normalizer::FORM_C")
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_resourcebundle_create, 0, 2, ResourceBundle, 1)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, bundle, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, fallback, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_resourcebundle_get, 0, 2, IS_MIXED, 0)
	ZEND_ARG_OBJ_INFO(0, bundle, ResourceBundle, 0)
	ZEND_ARG_INFO(0, index)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, fallback, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_resourcebundle_count, 0, 1, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, bundle, ResourceBundle, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_resourcebundle_locales, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, bundle, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_resourcebundle_get_error_code arginfo_resourcebundle_count

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_resourcebundle_get_error_message, 0, 1, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO(0, bundle, ResourceBundle, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_intltz_count_equivalent_ids, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, timezoneId, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_intltz_create_default, 0, 0, IntlTimeZone, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_intltz_create_enumeration, 0, 0, IntlIterator, MAY_BE_FALSE)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, countryOrRawOffset, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_intltz_create_time_zone, 0, 1, IntlTimeZone, 1)
	ZEND_ARG_TYPE_INFO(0, timezoneId, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_intltz_create_time_zone_id_enumeration, 0, 1, IntlIterator, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, type, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, region, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, rawOffset, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_intltz_from_date_time_zone, 0, 1, IntlTimeZone, 1)
	ZEND_ARG_OBJ_INFO(0, timezone, DateTimeZone, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_intltz_get_canonical_id, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, timezoneId, IS_STRING, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, isSystemId, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_intltz_get_display_name, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, timezone, IntlTimeZone, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, dst, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, style, IS_LONG, 0, "IntlTimeZone::DISPLAY_LONG")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, locale, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_intltz_get_dst_savings, 0, 1, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, timezone, IntlTimeZone, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_intltz_get_equivalent_id, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, timezoneId, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_intltz_get_error_code, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, timezone, IntlTimeZone, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_intltz_get_error_message, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, timezone, IntlTimeZone, 0)
ZEND_END_ARG_INFO()

#define arginfo_intltz_get_gmt arginfo_intltz_create_default

#define arginfo_intltz_get_id arginfo_intltz_get_error_message

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_intltz_get_offset, 0, 5, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, timezone, IntlTimeZone, 0)
	ZEND_ARG_TYPE_INFO(0, timestamp, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, local, _IS_BOOL, 0)
	ZEND_ARG_INFO(1, rawOffset)
	ZEND_ARG_INFO(1, dstOffset)
ZEND_END_ARG_INFO()

#define arginfo_intltz_get_raw_offset arginfo_intltz_get_dst_savings

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_intltz_get_region, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, timezoneId, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_intltz_get_tz_data_version, 0, 0, MAY_BE_STRING|MAY_BE_FALSE)
ZEND_END_ARG_INFO()

#define arginfo_intltz_get_unknown arginfo_intltz_create_default

#if U_ICU_VERSION_MAJOR_NUM >= 52
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_intltz_get_windows_id, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, timezoneId, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if U_ICU_VERSION_MAJOR_NUM >= 52
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_intltz_get_id_for_windows_id, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, timezoneId, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, region, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_intltz_has_same_rules, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, timezone, IntlTimeZone, 0)
	ZEND_ARG_OBJ_INFO(0, other, IntlTimeZone, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_intltz_to_date_time_zone, 0, 1, DateTimeZone, MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, timezone, IntlTimeZone, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_intltz_use_daylight_time, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, timezone, IntlTimeZone, 0)
ZEND_END_ARG_INFO()

#if U_ICU_VERSION_MAJOR_NUM >= 74
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_intltz_get_iana_id, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, timezoneId, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_transliterator_create, 0, 1, Transliterator, 1)
	ZEND_ARG_TYPE_INFO(0, id, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, direction, IS_LONG, 0, "Transliterator::FORWARD")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_transliterator_create_from_rules, 0, 1, Transliterator, 1)
	ZEND_ARG_TYPE_INFO(0, rules, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, direction, IS_LONG, 0, "Transliterator::FORWARD")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_transliterator_list_ids, 0, 0, MAY_BE_ARRAY|MAY_BE_FALSE)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_transliterator_create_inverse, 0, 1, Transliterator, 1)
	ZEND_ARG_OBJ_INFO(0, transliterator, Transliterator, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_transliterator_transliterate, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_TYPE_MASK(0, transliterator, Transliterator, MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, start, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, end, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_transliterator_get_error_code, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, transliterator, Transliterator, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_transliterator_get_error_message, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, transliterator, Transliterator, 0)
ZEND_END_ARG_INFO()

ZEND_FUNCTION(intlcal_create_instance);
ZEND_FUNCTION(intlcal_get_keyword_values_for_locale);
ZEND_FUNCTION(intlcal_get_now);
ZEND_FUNCTION(intlcal_get_available_locales);
ZEND_FUNCTION(intlcal_get);
ZEND_FUNCTION(intlcal_get_time);
ZEND_FUNCTION(intlcal_set_time);
ZEND_FUNCTION(intlcal_add);
ZEND_FUNCTION(intlcal_set_time_zone);
ZEND_FUNCTION(intlcal_after);
ZEND_FUNCTION(intlcal_before);
ZEND_FUNCTION(intlcal_set);
ZEND_FUNCTION(intlcal_roll);
ZEND_FUNCTION(intlcal_clear);
ZEND_FUNCTION(intlcal_field_difference);
ZEND_FUNCTION(intlcal_get_actual_maximum);
ZEND_FUNCTION(intlcal_get_actual_minimum);
ZEND_FUNCTION(intlcal_get_day_of_week_type);
ZEND_FUNCTION(intlcal_get_first_day_of_week);
ZEND_FUNCTION(intlcal_get_least_maximum);
ZEND_FUNCTION(intlcal_get_greatest_minimum);
ZEND_FUNCTION(intlcal_get_locale);
ZEND_FUNCTION(intlcal_get_maximum);
ZEND_FUNCTION(intlcal_get_minimal_days_in_first_week);
ZEND_FUNCTION(intlcal_set_minimal_days_in_first_week);
ZEND_FUNCTION(intlcal_get_minimum);
ZEND_FUNCTION(intlcal_get_time_zone);
ZEND_FUNCTION(intlcal_get_type);
ZEND_FUNCTION(intlcal_get_weekend_transition);
ZEND_FUNCTION(intlcal_in_daylight_time);
ZEND_FUNCTION(intlcal_is_lenient);
ZEND_FUNCTION(intlcal_is_set);
ZEND_FUNCTION(intlcal_is_equivalent_to);
ZEND_FUNCTION(intlcal_is_weekend);
ZEND_FUNCTION(intlcal_set_first_day_of_week);
ZEND_FUNCTION(intlcal_set_lenient);
ZEND_FUNCTION(intlcal_get_repeated_wall_time_option);
ZEND_FUNCTION(intlcal_equals);
ZEND_FUNCTION(intlcal_get_skipped_wall_time_option);
ZEND_FUNCTION(intlcal_set_repeated_wall_time_option);
ZEND_FUNCTION(intlcal_set_skipped_wall_time_option);
ZEND_FUNCTION(intlcal_from_date_time);
ZEND_FUNCTION(intlcal_to_date_time);
ZEND_FUNCTION(intlcal_get_error_code);
ZEND_FUNCTION(intlcal_get_error_message);
ZEND_FUNCTION(intlgregcal_create_instance);
ZEND_FUNCTION(intlgregcal_set_gregorian_change);
ZEND_FUNCTION(intlgregcal_get_gregorian_change);
ZEND_FUNCTION(intlgregcal_is_leap_year);
ZEND_FUNCTION(collator_create);
ZEND_FUNCTION(collator_compare);
ZEND_FUNCTION(collator_get_attribute);
ZEND_FUNCTION(collator_set_attribute);
ZEND_FUNCTION(collator_get_strength);
ZEND_FUNCTION(collator_set_strength);
ZEND_FUNCTION(collator_sort);
ZEND_FUNCTION(collator_sort_with_sort_keys);
ZEND_FUNCTION(collator_asort);
ZEND_FUNCTION(collator_get_locale);
ZEND_FUNCTION(collator_get_error_code);
ZEND_FUNCTION(collator_get_error_message);
ZEND_FUNCTION(collator_get_sort_key);
ZEND_FUNCTION(intl_get_error_code);
ZEND_FUNCTION(intl_get_error_message);
ZEND_FUNCTION(intl_is_failure);
ZEND_FUNCTION(intl_error_name);
ZEND_FUNCTION(datefmt_create);
ZEND_FUNCTION(datefmt_get_datetype);
ZEND_FUNCTION(datefmt_get_timetype);
ZEND_FUNCTION(datefmt_get_calendar);
ZEND_FUNCTION(datefmt_set_calendar);
ZEND_FUNCTION(datefmt_get_timezone_id);
ZEND_FUNCTION(datefmt_get_calendar_object);
ZEND_FUNCTION(datefmt_get_timezone);
ZEND_FUNCTION(datefmt_set_timezone);
ZEND_FUNCTION(datefmt_set_pattern);
ZEND_FUNCTION(datefmt_get_pattern);
ZEND_FUNCTION(datefmt_get_locale);
ZEND_FUNCTION(datefmt_set_lenient);
ZEND_FUNCTION(datefmt_is_lenient);
ZEND_FUNCTION(datefmt_format);
ZEND_FUNCTION(datefmt_format_object);
ZEND_FUNCTION(datefmt_parse);
ZEND_FUNCTION(datefmt_localtime);
ZEND_FUNCTION(datefmt_get_error_code);
ZEND_FUNCTION(datefmt_get_error_message);
ZEND_FUNCTION(numfmt_create);
ZEND_FUNCTION(numfmt_format);
ZEND_FUNCTION(numfmt_parse);
ZEND_FUNCTION(numfmt_format_currency);
ZEND_FUNCTION(numfmt_parse_currency);
ZEND_FUNCTION(numfmt_set_attribute);
ZEND_FUNCTION(numfmt_get_attribute);
ZEND_FUNCTION(numfmt_set_text_attribute);
ZEND_FUNCTION(numfmt_get_text_attribute);
ZEND_FUNCTION(numfmt_set_symbol);
ZEND_FUNCTION(numfmt_get_symbol);
ZEND_FUNCTION(numfmt_set_pattern);
ZEND_FUNCTION(numfmt_get_pattern);
ZEND_FUNCTION(numfmt_get_locale);
ZEND_FUNCTION(numfmt_get_error_code);
ZEND_FUNCTION(numfmt_get_error_message);
ZEND_FUNCTION(grapheme_strlen);
ZEND_FUNCTION(grapheme_strpos);
ZEND_FUNCTION(grapheme_stripos);
ZEND_FUNCTION(grapheme_strrpos);
ZEND_FUNCTION(grapheme_strripos);
ZEND_FUNCTION(grapheme_substr);
ZEND_FUNCTION(grapheme_strstr);
ZEND_FUNCTION(grapheme_stristr);
ZEND_FUNCTION(grapheme_extract);
ZEND_FUNCTION(idn_to_ascii);
ZEND_FUNCTION(idn_to_utf8);
ZEND_FUNCTION(locale_get_default);
ZEND_FUNCTION(locale_set_default);
ZEND_FUNCTION(locale_get_primary_language);
ZEND_FUNCTION(locale_get_script);
ZEND_FUNCTION(locale_get_region);
ZEND_FUNCTION(locale_get_keywords);
ZEND_FUNCTION(locale_get_display_script);
ZEND_FUNCTION(locale_get_display_region);
ZEND_FUNCTION(locale_get_display_name);
ZEND_FUNCTION(locale_get_display_language);
ZEND_FUNCTION(locale_get_display_variant);
ZEND_FUNCTION(locale_compose);
ZEND_FUNCTION(locale_parse);
ZEND_FUNCTION(locale_get_all_variants);
ZEND_FUNCTION(locale_filter_matches);
ZEND_FUNCTION(locale_canonicalize);
ZEND_FUNCTION(locale_lookup);
ZEND_FUNCTION(locale_accept_from_http);
ZEND_FUNCTION(msgfmt_create);
ZEND_FUNCTION(msgfmt_format);
ZEND_FUNCTION(msgfmt_format_message);
ZEND_FUNCTION(msgfmt_parse);
ZEND_FUNCTION(msgfmt_parse_message);
ZEND_FUNCTION(msgfmt_set_pattern);
ZEND_FUNCTION(msgfmt_get_pattern);
ZEND_FUNCTION(msgfmt_get_locale);
ZEND_FUNCTION(msgfmt_get_error_code);
ZEND_FUNCTION(msgfmt_get_error_message);
ZEND_FUNCTION(normalizer_normalize);
ZEND_FUNCTION(normalizer_is_normalized);
#if U_ICU_VERSION_MAJOR_NUM >= 56
ZEND_FUNCTION(normalizer_get_raw_decomposition);
#endif
ZEND_FUNCTION(resourcebundle_create);
ZEND_FUNCTION(resourcebundle_get);
ZEND_FUNCTION(resourcebundle_count);
ZEND_FUNCTION(resourcebundle_locales);
ZEND_FUNCTION(resourcebundle_get_error_code);
ZEND_FUNCTION(resourcebundle_get_error_message);
ZEND_FUNCTION(intltz_count_equivalent_ids);
ZEND_FUNCTION(intltz_create_default);
ZEND_FUNCTION(intltz_create_enumeration);
ZEND_FUNCTION(intltz_create_time_zone);
ZEND_FUNCTION(intltz_create_time_zone_id_enumeration);
ZEND_FUNCTION(intltz_from_date_time_zone);
ZEND_FUNCTION(intltz_get_canonical_id);
ZEND_FUNCTION(intltz_get_display_name);
ZEND_FUNCTION(intltz_get_dst_savings);
ZEND_FUNCTION(intltz_get_equivalent_id);
ZEND_FUNCTION(intltz_get_error_code);
ZEND_FUNCTION(intltz_get_error_message);
ZEND_FUNCTION(intltz_get_gmt);
ZEND_FUNCTION(intltz_get_id);
ZEND_FUNCTION(intltz_get_offset);
ZEND_FUNCTION(intltz_get_raw_offset);
ZEND_FUNCTION(intltz_get_region);
ZEND_FUNCTION(intltz_get_tz_data_version);
ZEND_FUNCTION(intltz_get_unknown);
#if U_ICU_VERSION_MAJOR_NUM >= 52
ZEND_FUNCTION(intltz_get_windows_id);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 52
ZEND_FUNCTION(intltz_get_id_for_windows_id);
#endif
ZEND_FUNCTION(intltz_has_same_rules);
ZEND_FUNCTION(intltz_to_date_time_zone);
ZEND_FUNCTION(intltz_use_daylight_time);
#if U_ICU_VERSION_MAJOR_NUM >= 74
ZEND_FUNCTION(intltz_get_iana_id);
#endif
ZEND_FUNCTION(transliterator_create);
ZEND_FUNCTION(transliterator_create_from_rules);
ZEND_FUNCTION(transliterator_list_ids);
ZEND_FUNCTION(transliterator_create_inverse);
ZEND_FUNCTION(transliterator_transliterate);
ZEND_FUNCTION(transliterator_get_error_code);
ZEND_FUNCTION(transliterator_get_error_message);

static const zend_function_entry ext_functions[] = {
	ZEND_FE(intlcal_create_instance, arginfo_intlcal_create_instance)
	ZEND_FE(intlcal_get_keyword_values_for_locale, arginfo_intlcal_get_keyword_values_for_locale)
	ZEND_FE(intlcal_get_now, arginfo_intlcal_get_now)
	ZEND_FE(intlcal_get_available_locales, arginfo_intlcal_get_available_locales)
	ZEND_FE(intlcal_get, arginfo_intlcal_get)
	ZEND_FE(intlcal_get_time, arginfo_intlcal_get_time)
	ZEND_FE(intlcal_set_time, arginfo_intlcal_set_time)
	ZEND_FE(intlcal_add, arginfo_intlcal_add)
	ZEND_FE(intlcal_set_time_zone, arginfo_intlcal_set_time_zone)
	ZEND_FE(intlcal_after, arginfo_intlcal_after)
	ZEND_FE(intlcal_before, arginfo_intlcal_before)
	ZEND_RAW_FENTRY("intlcal_set", zif_intlcal_set, arginfo_intlcal_set, ZEND_ACC_DEPRECATED, NULL, NULL)
	ZEND_FE(intlcal_roll, arginfo_intlcal_roll)
	ZEND_FE(intlcal_clear, arginfo_intlcal_clear)
	ZEND_FE(intlcal_field_difference, arginfo_intlcal_field_difference)
	ZEND_FE(intlcal_get_actual_maximum, arginfo_intlcal_get_actual_maximum)
	ZEND_FE(intlcal_get_actual_minimum, arginfo_intlcal_get_actual_minimum)
	ZEND_FE(intlcal_get_day_of_week_type, arginfo_intlcal_get_day_of_week_type)
	ZEND_FE(intlcal_get_first_day_of_week, arginfo_intlcal_get_first_day_of_week)
	ZEND_FE(intlcal_get_least_maximum, arginfo_intlcal_get_least_maximum)
	ZEND_FE(intlcal_get_greatest_minimum, arginfo_intlcal_get_greatest_minimum)
	ZEND_FE(intlcal_get_locale, arginfo_intlcal_get_locale)
	ZEND_FE(intlcal_get_maximum, arginfo_intlcal_get_maximum)
	ZEND_FE(intlcal_get_minimal_days_in_first_week, arginfo_intlcal_get_minimal_days_in_first_week)
	ZEND_FE(intlcal_set_minimal_days_in_first_week, arginfo_intlcal_set_minimal_days_in_first_week)
	ZEND_FE(intlcal_get_minimum, arginfo_intlcal_get_minimum)
	ZEND_FE(intlcal_get_time_zone, arginfo_intlcal_get_time_zone)
	ZEND_FE(intlcal_get_type, arginfo_intlcal_get_type)
	ZEND_FE(intlcal_get_weekend_transition, arginfo_intlcal_get_weekend_transition)
	ZEND_FE(intlcal_in_daylight_time, arginfo_intlcal_in_daylight_time)
	ZEND_FE(intlcal_is_lenient, arginfo_intlcal_is_lenient)
	ZEND_FE(intlcal_is_set, arginfo_intlcal_is_set)
	ZEND_FE(intlcal_is_equivalent_to, arginfo_intlcal_is_equivalent_to)
	ZEND_FE(intlcal_is_weekend, arginfo_intlcal_is_weekend)
	ZEND_FE(intlcal_set_first_day_of_week, arginfo_intlcal_set_first_day_of_week)
	ZEND_FE(intlcal_set_lenient, arginfo_intlcal_set_lenient)
	ZEND_FE(intlcal_get_repeated_wall_time_option, arginfo_intlcal_get_repeated_wall_time_option)
	ZEND_FE(intlcal_equals, arginfo_intlcal_equals)
	ZEND_FE(intlcal_get_skipped_wall_time_option, arginfo_intlcal_get_skipped_wall_time_option)
	ZEND_FE(intlcal_set_repeated_wall_time_option, arginfo_intlcal_set_repeated_wall_time_option)
	ZEND_FE(intlcal_set_skipped_wall_time_option, arginfo_intlcal_set_skipped_wall_time_option)
	ZEND_FE(intlcal_from_date_time, arginfo_intlcal_from_date_time)
	ZEND_FE(intlcal_to_date_time, arginfo_intlcal_to_date_time)
	ZEND_FE(intlcal_get_error_code, arginfo_intlcal_get_error_code)
	ZEND_FE(intlcal_get_error_message, arginfo_intlcal_get_error_message)
	ZEND_RAW_FENTRY("intlgregcal_create_instance", zif_intlgregcal_create_instance, arginfo_intlgregcal_create_instance, ZEND_ACC_DEPRECATED, NULL, NULL)
	ZEND_FE(intlgregcal_set_gregorian_change, arginfo_intlgregcal_set_gregorian_change)
	ZEND_FE(intlgregcal_get_gregorian_change, arginfo_intlgregcal_get_gregorian_change)
	ZEND_FE(intlgregcal_is_leap_year, arginfo_intlgregcal_is_leap_year)
	ZEND_FE(collator_create, arginfo_collator_create)
	ZEND_FE(collator_compare, arginfo_collator_compare)
	ZEND_FE(collator_get_attribute, arginfo_collator_get_attribute)
	ZEND_FE(collator_set_attribute, arginfo_collator_set_attribute)
	ZEND_FE(collator_get_strength, arginfo_collator_get_strength)
	ZEND_FE(collator_set_strength, arginfo_collator_set_strength)
	ZEND_FE(collator_sort, arginfo_collator_sort)
	ZEND_FE(collator_sort_with_sort_keys, arginfo_collator_sort_with_sort_keys)
	ZEND_FE(collator_asort, arginfo_collator_asort)
	ZEND_FE(collator_get_locale, arginfo_collator_get_locale)
	ZEND_FE(collator_get_error_code, arginfo_collator_get_error_code)
	ZEND_FE(collator_get_error_message, arginfo_collator_get_error_message)
	ZEND_FE(collator_get_sort_key, arginfo_collator_get_sort_key)
	ZEND_FE(intl_get_error_code, arginfo_intl_get_error_code)
	ZEND_FE(intl_get_error_message, arginfo_intl_get_error_message)
	ZEND_FE(intl_is_failure, arginfo_intl_is_failure)
	ZEND_FE(intl_error_name, arginfo_intl_error_name)
	ZEND_FE(datefmt_create, arginfo_datefmt_create)
	ZEND_FE(datefmt_get_datetype, arginfo_datefmt_get_datetype)
	ZEND_FE(datefmt_get_timetype, arginfo_datefmt_get_timetype)
	ZEND_FE(datefmt_get_calendar, arginfo_datefmt_get_calendar)
	ZEND_FE(datefmt_set_calendar, arginfo_datefmt_set_calendar)
	ZEND_FE(datefmt_get_timezone_id, arginfo_datefmt_get_timezone_id)
	ZEND_FE(datefmt_get_calendar_object, arginfo_datefmt_get_calendar_object)
	ZEND_FE(datefmt_get_timezone, arginfo_datefmt_get_timezone)
	ZEND_FE(datefmt_set_timezone, arginfo_datefmt_set_timezone)
	ZEND_FE(datefmt_set_pattern, arginfo_datefmt_set_pattern)
	ZEND_FE(datefmt_get_pattern, arginfo_datefmt_get_pattern)
	ZEND_FE(datefmt_get_locale, arginfo_datefmt_get_locale)
	ZEND_FE(datefmt_set_lenient, arginfo_datefmt_set_lenient)
	ZEND_FE(datefmt_is_lenient, arginfo_datefmt_is_lenient)
	ZEND_FE(datefmt_format, arginfo_datefmt_format)
	ZEND_FE(datefmt_format_object, arginfo_datefmt_format_object)
	ZEND_FE(datefmt_parse, arginfo_datefmt_parse)
	ZEND_FE(datefmt_localtime, arginfo_datefmt_localtime)
	ZEND_FE(datefmt_get_error_code, arginfo_datefmt_get_error_code)
	ZEND_FE(datefmt_get_error_message, arginfo_datefmt_get_error_message)
	ZEND_FE(numfmt_create, arginfo_numfmt_create)
	ZEND_FE(numfmt_format, arginfo_numfmt_format)
	ZEND_FE(numfmt_parse, arginfo_numfmt_parse)
	ZEND_FE(numfmt_format_currency, arginfo_numfmt_format_currency)
	ZEND_FE(numfmt_parse_currency, arginfo_numfmt_parse_currency)
	ZEND_FE(numfmt_set_attribute, arginfo_numfmt_set_attribute)
	ZEND_FE(numfmt_get_attribute, arginfo_numfmt_get_attribute)
	ZEND_FE(numfmt_set_text_attribute, arginfo_numfmt_set_text_attribute)
	ZEND_FE(numfmt_get_text_attribute, arginfo_numfmt_get_text_attribute)
	ZEND_FE(numfmt_set_symbol, arginfo_numfmt_set_symbol)
	ZEND_FE(numfmt_get_symbol, arginfo_numfmt_get_symbol)
	ZEND_FE(numfmt_set_pattern, arginfo_numfmt_set_pattern)
	ZEND_FE(numfmt_get_pattern, arginfo_numfmt_get_pattern)
	ZEND_FE(numfmt_get_locale, arginfo_numfmt_get_locale)
	ZEND_FE(numfmt_get_error_code, arginfo_numfmt_get_error_code)
	ZEND_FE(numfmt_get_error_message, arginfo_numfmt_get_error_message)
	ZEND_FE(grapheme_strlen, arginfo_grapheme_strlen)
	ZEND_FE(grapheme_strpos, arginfo_grapheme_strpos)
	ZEND_FE(grapheme_stripos, arginfo_grapheme_stripos)
	ZEND_FE(grapheme_strrpos, arginfo_grapheme_strrpos)
	ZEND_FE(grapheme_strripos, arginfo_grapheme_strripos)
	ZEND_FE(grapheme_substr, arginfo_grapheme_substr)
	ZEND_FE(grapheme_strstr, arginfo_grapheme_strstr)
	ZEND_FE(grapheme_stristr, arginfo_grapheme_stristr)
	ZEND_FE(grapheme_extract, arginfo_grapheme_extract)
	ZEND_FE(idn_to_ascii, arginfo_idn_to_ascii)
	ZEND_FE(idn_to_utf8, arginfo_idn_to_utf8)
	ZEND_FE(locale_get_default, arginfo_locale_get_default)
	ZEND_FE(locale_set_default, arginfo_locale_set_default)
	ZEND_FE(locale_get_primary_language, arginfo_locale_get_primary_language)
	ZEND_FE(locale_get_script, arginfo_locale_get_script)
	ZEND_FE(locale_get_region, arginfo_locale_get_region)
	ZEND_FE(locale_get_keywords, arginfo_locale_get_keywords)
	ZEND_FE(locale_get_display_script, arginfo_locale_get_display_script)
	ZEND_FE(locale_get_display_region, arginfo_locale_get_display_region)
	ZEND_FE(locale_get_display_name, arginfo_locale_get_display_name)
	ZEND_FE(locale_get_display_language, arginfo_locale_get_display_language)
	ZEND_FE(locale_get_display_variant, arginfo_locale_get_display_variant)
	ZEND_FE(locale_compose, arginfo_locale_compose)
	ZEND_FE(locale_parse, arginfo_locale_parse)
	ZEND_FE(locale_get_all_variants, arginfo_locale_get_all_variants)
	ZEND_FE(locale_filter_matches, arginfo_locale_filter_matches)
	ZEND_FE(locale_canonicalize, arginfo_locale_canonicalize)
	ZEND_FE(locale_lookup, arginfo_locale_lookup)
	ZEND_FE(locale_accept_from_http, arginfo_locale_accept_from_http)
	ZEND_FE(msgfmt_create, arginfo_msgfmt_create)
	ZEND_FE(msgfmt_format, arginfo_msgfmt_format)
	ZEND_FE(msgfmt_format_message, arginfo_msgfmt_format_message)
	ZEND_FE(msgfmt_parse, arginfo_msgfmt_parse)
	ZEND_FE(msgfmt_parse_message, arginfo_msgfmt_parse_message)
	ZEND_FE(msgfmt_set_pattern, arginfo_msgfmt_set_pattern)
	ZEND_FE(msgfmt_get_pattern, arginfo_msgfmt_get_pattern)
	ZEND_FE(msgfmt_get_locale, arginfo_msgfmt_get_locale)
	ZEND_FE(msgfmt_get_error_code, arginfo_msgfmt_get_error_code)
	ZEND_FE(msgfmt_get_error_message, arginfo_msgfmt_get_error_message)
	ZEND_FE(normalizer_normalize, arginfo_normalizer_normalize)
	ZEND_FE(normalizer_is_normalized, arginfo_normalizer_is_normalized)
#if U_ICU_VERSION_MAJOR_NUM >= 56
	ZEND_FE(normalizer_get_raw_decomposition, arginfo_normalizer_get_raw_decomposition)
#endif
	ZEND_FE(resourcebundle_create, arginfo_resourcebundle_create)
	ZEND_FE(resourcebundle_get, arginfo_resourcebundle_get)
	ZEND_FE(resourcebundle_count, arginfo_resourcebundle_count)
	ZEND_FE(resourcebundle_locales, arginfo_resourcebundle_locales)
	ZEND_FE(resourcebundle_get_error_code, arginfo_resourcebundle_get_error_code)
	ZEND_FE(resourcebundle_get_error_message, arginfo_resourcebundle_get_error_message)
	ZEND_FE(intltz_count_equivalent_ids, arginfo_intltz_count_equivalent_ids)
	ZEND_FE(intltz_create_default, arginfo_intltz_create_default)
	ZEND_FE(intltz_create_enumeration, arginfo_intltz_create_enumeration)
	ZEND_FE(intltz_create_time_zone, arginfo_intltz_create_time_zone)
	ZEND_FE(intltz_create_time_zone_id_enumeration, arginfo_intltz_create_time_zone_id_enumeration)
	ZEND_FE(intltz_from_date_time_zone, arginfo_intltz_from_date_time_zone)
	ZEND_FE(intltz_get_canonical_id, arginfo_intltz_get_canonical_id)
	ZEND_FE(intltz_get_display_name, arginfo_intltz_get_display_name)
	ZEND_FE(intltz_get_dst_savings, arginfo_intltz_get_dst_savings)
	ZEND_FE(intltz_get_equivalent_id, arginfo_intltz_get_equivalent_id)
	ZEND_FE(intltz_get_error_code, arginfo_intltz_get_error_code)
	ZEND_FE(intltz_get_error_message, arginfo_intltz_get_error_message)
	ZEND_FE(intltz_get_gmt, arginfo_intltz_get_gmt)
	ZEND_FE(intltz_get_id, arginfo_intltz_get_id)
	ZEND_FE(intltz_get_offset, arginfo_intltz_get_offset)
	ZEND_FE(intltz_get_raw_offset, arginfo_intltz_get_raw_offset)
	ZEND_FE(intltz_get_region, arginfo_intltz_get_region)
	ZEND_FE(intltz_get_tz_data_version, arginfo_intltz_get_tz_data_version)
	ZEND_FE(intltz_get_unknown, arginfo_intltz_get_unknown)
#if U_ICU_VERSION_MAJOR_NUM >= 52
	ZEND_FE(intltz_get_windows_id, arginfo_intltz_get_windows_id)
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 52
	ZEND_FE(intltz_get_id_for_windows_id, arginfo_intltz_get_id_for_windows_id)
#endif
	ZEND_FE(intltz_has_same_rules, arginfo_intltz_has_same_rules)
	ZEND_FE(intltz_to_date_time_zone, arginfo_intltz_to_date_time_zone)
	ZEND_FE(intltz_use_daylight_time, arginfo_intltz_use_daylight_time)
#if U_ICU_VERSION_MAJOR_NUM >= 74
	ZEND_FE(intltz_get_iana_id, arginfo_intltz_get_iana_id)
#endif
	ZEND_FE(transliterator_create, arginfo_transliterator_create)
	ZEND_FE(transliterator_create_from_rules, arginfo_transliterator_create_from_rules)
	ZEND_FE(transliterator_list_ids, arginfo_transliterator_list_ids)
	ZEND_FE(transliterator_create_inverse, arginfo_transliterator_create_inverse)
	ZEND_FE(transliterator_transliterate, arginfo_transliterator_transliterate)
	ZEND_FE(transliterator_get_error_code, arginfo_transliterator_get_error_code)
	ZEND_FE(transliterator_get_error_message, arginfo_transliterator_get_error_message)
	ZEND_FE_END
};

static const zend_function_entry class_IntlException_methods[] = {
	ZEND_FE_END
};

static void register_php_intl_symbols(int module_number)
{
	REGISTER_LONG_CONSTANT("INTL_MAX_LOCALE_LEN", INTL_MAX_LOCALE_LEN, CONST_PERSISTENT);
	REGISTER_STRING_CONSTANT("INTL_ICU_VERSION", U_ICU_VERSION, CONST_PERSISTENT);
#if defined(U_ICU_DATA_VERSION)
	REGISTER_STRING_CONSTANT("INTL_ICU_DATA_VERSION", U_ICU_DATA_VERSION, CONST_PERSISTENT);
#endif
	REGISTER_LONG_CONSTANT("GRAPHEME_EXTR_COUNT", GRAPHEME_EXTRACT_TYPE_COUNT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GRAPHEME_EXTR_MAXBYTES", GRAPHEME_EXTRACT_TYPE_MAXBYTES, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GRAPHEME_EXTR_MAXCHARS", GRAPHEME_EXTRACT_TYPE_MAXCHARS, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IDNA_DEFAULT", UIDNA_DEFAULT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IDNA_ALLOW_UNASSIGNED", UIDNA_ALLOW_UNASSIGNED, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IDNA_USE_STD3_RULES", UIDNA_USE_STD3_RULES, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IDNA_CHECK_BIDI", UIDNA_CHECK_BIDI, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IDNA_CHECK_CONTEXTJ", UIDNA_CHECK_CONTEXTJ, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IDNA_NONTRANSITIONAL_TO_ASCII", UIDNA_NONTRANSITIONAL_TO_ASCII, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IDNA_NONTRANSITIONAL_TO_UNICODE", UIDNA_NONTRANSITIONAL_TO_UNICODE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("INTL_IDNA_VARIANT_UTS46", INTL_IDN_VARIANT_UTS46, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IDNA_ERROR_EMPTY_LABEL", UIDNA_ERROR_EMPTY_LABEL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IDNA_ERROR_LABEL_TOO_LONG", UIDNA_ERROR_LABEL_TOO_LONG, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IDNA_ERROR_DOMAIN_NAME_TOO_LONG", UIDNA_ERROR_DOMAIN_NAME_TOO_LONG, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IDNA_ERROR_LEADING_HYPHEN", UIDNA_ERROR_LEADING_HYPHEN, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IDNA_ERROR_TRAILING_HYPHEN", UIDNA_ERROR_TRAILING_HYPHEN, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IDNA_ERROR_HYPHEN_3_4", UIDNA_ERROR_HYPHEN_3_4, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IDNA_ERROR_LEADING_COMBINING_MARK", UIDNA_ERROR_LEADING_COMBINING_MARK, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IDNA_ERROR_DISALLOWED", UIDNA_ERROR_DISALLOWED, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IDNA_ERROR_PUNYCODE", UIDNA_ERROR_PUNYCODE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IDNA_ERROR_LABEL_HAS_DOT", UIDNA_ERROR_LABEL_HAS_DOT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IDNA_ERROR_INVALID_ACE_LABEL", UIDNA_ERROR_INVALID_ACE_LABEL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IDNA_ERROR_BIDI", UIDNA_ERROR_BIDI, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IDNA_ERROR_CONTEXTJ", UIDNA_ERROR_CONTEXTJ, CONST_PERSISTENT);
}

static zend_class_entry *register_class_IntlException(zend_class_entry *class_entry_Exception)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "IntlException", class_IntlException_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_Exception);

	return class_entry;
}
