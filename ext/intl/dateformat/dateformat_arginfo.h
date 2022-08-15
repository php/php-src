/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 425a285b704e175ccd0313fe1be85c8abaf820b7 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlDateFormatter___construct, 0, 0, 3)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, dateType, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, timeType, IS_LONG, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, timezone, "null")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, calendar, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, pattern, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlDateFormatter_create, 0, 0, 3)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, dateType, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, timeType, IS_LONG, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, timezone, "null")
	ZEND_ARG_OBJ_TYPE_MASK(0, calendar, IntlCalendar, MAY_BE_LONG|MAY_BE_NULL, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, pattern, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlDateFormatter_getDateType, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlDateFormatter_getTimeType arginfo_class_IntlDateFormatter_getDateType

#define arginfo_class_IntlDateFormatter_getCalendar arginfo_class_IntlDateFormatter_getDateType

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlDateFormatter_setCalendar, 0, 0, 1)
	ZEND_ARG_OBJ_TYPE_MASK(0, calendar, IntlCalendar, MAY_BE_LONG|MAY_BE_NULL, NULL)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlDateFormatter_getTimeZoneId arginfo_class_IntlDateFormatter_getDateType

#define arginfo_class_IntlDateFormatter_getCalendarObject arginfo_class_IntlDateFormatter_getDateType

#define arginfo_class_IntlDateFormatter_getTimeZone arginfo_class_IntlDateFormatter_getDateType

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlDateFormatter_setTimeZone, 0, 0, 1)
	ZEND_ARG_INFO(0, timezone)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlDateFormatter_setPattern, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlDateFormatter_getPattern arginfo_class_IntlDateFormatter_getDateType

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlDateFormatter_getLocale, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, type, IS_LONG, 0, "ULOC_ACTUAL_LOCALE")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlDateFormatter_setLenient, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, lenient, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlDateFormatter_isLenient arginfo_class_IntlDateFormatter_getDateType

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlDateFormatter_format, 0, 0, 1)
	ZEND_ARG_INFO(0, datetime)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlDateFormatter_formatObject, 0, 0, 1)
	ZEND_ARG_INFO(0, datetime)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, format, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, locale, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlDateFormatter_parse, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, offset, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_IntlDateFormatter_localtime arginfo_class_IntlDateFormatter_parse

#define arginfo_class_IntlDateFormatter_getErrorCode arginfo_class_IntlDateFormatter_getDateType

#define arginfo_class_IntlDateFormatter_getErrorMessage arginfo_class_IntlDateFormatter_getDateType


ZEND_METHOD(IntlDateFormatter, __construct);
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


static const zend_function_entry class_IntlDateFormatter_methods[] = {
	ZEND_ME(IntlDateFormatter, __construct, arginfo_class_IntlDateFormatter___construct, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(create, datefmt_create, arginfo_class_IntlDateFormatter_create, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME_MAPPING(getDateType, datefmt_get_datetype, arginfo_class_IntlDateFormatter_getDateType, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getTimeType, datefmt_get_timetype, arginfo_class_IntlDateFormatter_getTimeType, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getCalendar, datefmt_get_calendar, arginfo_class_IntlDateFormatter_getCalendar, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(setCalendar, datefmt_set_calendar, arginfo_class_IntlDateFormatter_setCalendar, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getTimeZoneId, datefmt_get_timezone_id, arginfo_class_IntlDateFormatter_getTimeZoneId, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getCalendarObject, datefmt_get_calendar_object, arginfo_class_IntlDateFormatter_getCalendarObject, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getTimeZone, datefmt_get_timezone, arginfo_class_IntlDateFormatter_getTimeZone, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(setTimeZone, datefmt_set_timezone, arginfo_class_IntlDateFormatter_setTimeZone, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(setPattern, datefmt_set_pattern, arginfo_class_IntlDateFormatter_setPattern, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getPattern, datefmt_get_pattern, arginfo_class_IntlDateFormatter_getPattern, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getLocale, datefmt_get_locale, arginfo_class_IntlDateFormatter_getLocale, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(setLenient, datefmt_set_lenient, arginfo_class_IntlDateFormatter_setLenient, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(isLenient, datefmt_is_lenient, arginfo_class_IntlDateFormatter_isLenient, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(format, datefmt_format, arginfo_class_IntlDateFormatter_format, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(formatObject, datefmt_format_object, arginfo_class_IntlDateFormatter_formatObject, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME_MAPPING(parse, datefmt_parse, arginfo_class_IntlDateFormatter_parse, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(localtime, datefmt_localtime, arginfo_class_IntlDateFormatter_localtime, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getErrorCode, datefmt_get_error_code, arginfo_class_IntlDateFormatter_getErrorCode, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getErrorMessage, datefmt_get_error_message, arginfo_class_IntlDateFormatter_getErrorMessage, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};
