/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlCalendar___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlCalendar_createInstance, 0, 0, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, timeZone, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, locale, IS_STRING, 1, "null")
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
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, field, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlCalendar_fromDateTime, 0, 0, 1)
	ZEND_ARG_INFO(0, dateTime)
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
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, date, IS_DOUBLE, 1, "null")
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
