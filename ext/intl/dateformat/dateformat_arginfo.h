/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlDateFormatter___construct, 0, 0, 3)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, datetype, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, timetype, IS_LONG, 0)
	ZEND_ARG_INFO(0, timezone)
	ZEND_ARG_INFO(0, calendar)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlDateFormatter_create arginfo_class_IntlDateFormatter___construct

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlDateFormatter_getDateType, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlDateFormatter_getTimeType arginfo_class_IntlDateFormatter_getDateType

#define arginfo_class_IntlDateFormatter_getCalendar arginfo_class_IntlDateFormatter_getDateType

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlDateFormatter_setCalendar, 0, 0, 1)
	ZEND_ARG_INFO(0, which)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlDateFormatter_getTimeZoneId arginfo_class_IntlDateFormatter_getDateType

#define arginfo_class_IntlDateFormatter_getCalendarObject arginfo_class_IntlDateFormatter_getDateType

#define arginfo_class_IntlDateFormatter_getTimeZone arginfo_class_IntlDateFormatter_getDateType

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlDateFormatter_setTimeZone, 0, 0, 1)
	ZEND_ARG_INFO(0, zone)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlDateFormatter_setPattern, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlDateFormatter_getPattern arginfo_class_IntlDateFormatter_getDateType

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlDateFormatter_getLocale, 0, 0, 0)
	ZEND_ARG_TYPE_INFO(0, which, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlDateFormatter_setLenient, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, lenient, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlDateFormatter_isLenient arginfo_class_IntlDateFormatter_getDateType

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlDateFormatter_format, 0, 0, 1)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlDateFormatter_formatObject, 0, 0, 1)
	ZEND_ARG_INFO(0, object)
	ZEND_ARG_INFO(0, format)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlDateFormatter_parse, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(1, position, IS_LONG, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlDateFormatter_localtime arginfo_class_IntlDateFormatter_parse

#define arginfo_class_IntlDateFormatter_getErrorCode arginfo_class_IntlDateFormatter_getDateType

#define arginfo_class_IntlDateFormatter_getErrorMessage arginfo_class_IntlDateFormatter_getDateType

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_datefmt_create, 0, 3, IntlDateFormatter, 1)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, datetype, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, timetype, IS_LONG, 0)
	ZEND_ARG_INFO(0, timezone)
	ZEND_ARG_INFO(0, calendar)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_datefmt_get_datetype, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, df, IntlDateFormatter, 0)
ZEND_END_ARG_INFO()

#define arginfo_datefmt_get_timetype arginfo_datefmt_get_datetype

#define arginfo_datefmt_get_calendar arginfo_datefmt_get_datetype

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_datefmt_set_calendar, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, df, IntlDateFormatter, 0)
	ZEND_ARG_INFO(0, which)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_datefmt_get_timezone_id, 0, 1, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO(0, df, IntlDateFormatter, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_datefmt_get_calendar_object, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, df, IntlDateFormatter, 0)
ZEND_END_ARG_INFO()

#define arginfo_datefmt_get_timezone arginfo_datefmt_get_calendar_object

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_datefmt_set_timezone, 0, 2, _IS_BOOL, 1)
	ZEND_ARG_OBJ_INFO(0, df, IntlDateFormatter, 0)
	ZEND_ARG_INFO(0, zone)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_datefmt_set_pattern, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, df, IntlDateFormatter, 0)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_datefmt_get_pattern, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, df, IntlDateFormatter, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_datefmt_get_locale, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, df, IntlDateFormatter, 0)
	ZEND_ARG_TYPE_INFO(0, which, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_datefmt_set_lenient, 0, 2, MAY_BE_NULL|MAY_BE_BOOL)
	ZEND_ARG_OBJ_INFO(0, df, IntlDateFormatter, 0)
	ZEND_ARG_TYPE_INFO(0, lenient, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_datefmt_is_lenient, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, df, IntlDateFormatter, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_datefmt_format, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, df, IntlDateFormatter, 0)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_datefmt_format_object, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, object)
	ZEND_ARG_INFO(0, format)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_datefmt_parse, 0, 2, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, df, IntlDateFormatter, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(1, position, IS_LONG, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_datefmt_localtime, 0, 2, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, df, IntlDateFormatter, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(1, position, IS_LONG, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_datefmt_get_error_code, 0, 1, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, df, IntlDateFormatter, 0)
ZEND_END_ARG_INFO()

#define arginfo_datefmt_get_error_message arginfo_datefmt_get_timezone_id
