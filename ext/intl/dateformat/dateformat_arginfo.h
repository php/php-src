/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 91f1dbe4843fd1d4dff7266e814a3c8f9aed882a */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlDateFormatter___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, dateType, IS_LONG, 0, "IntlDateFormatter::FULL")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timeType, IS_LONG, 0, "IntlDateFormatter::FULL")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, timezone, "null")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, calendar, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, pattern, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_INFO_EX(arginfo_class_IntlDateFormatter_create, 0, 1, IntlDateFormatter, 1)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, dateType, IS_LONG, 0, "IntlDateFormatter::FULL")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timeType, IS_LONG, 0, "IntlDateFormatter::FULL")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, timezone, "null")
	ZEND_ARG_OBJ_TYPE_MASK(0, calendar, IntlCalendar, MAY_BE_LONG|MAY_BE_NULL, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, pattern, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_IntlDateFormatter_getDateType, 0, 0, MAY_BE_LONG|MAY_BE_FALSE)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlDateFormatter_getTimeType arginfo_class_IntlDateFormatter_getDateType

#define arginfo_class_IntlDateFormatter_getCalendar arginfo_class_IntlDateFormatter_getDateType

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_IntlDateFormatter_setCalendar, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, calendar, IntlCalendar, MAY_BE_LONG|MAY_BE_NULL, NULL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_IntlDateFormatter_getTimeZoneId, 0, 0, MAY_BE_STRING|MAY_BE_FALSE)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_IntlDateFormatter_getCalendarObject, 0, 0, IntlCalendar, MAY_BE_FALSE|MAY_BE_NULL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_IntlDateFormatter_getTimeZone, 0, 0, IntlTimeZone, MAY_BE_FALSE)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_IntlDateFormatter_setTimeZone, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, timezone)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_IntlDateFormatter_setPattern, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlDateFormatter_getPattern arginfo_class_IntlDateFormatter_getTimeZoneId

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_IntlDateFormatter_getLocale, 0, 0, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, type, IS_LONG, 0, "ULOC_ACTUAL_LOCALE")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_IntlDateFormatter_setLenient, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, lenient, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_IntlDateFormatter_isLenient, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_IntlDateFormatter_format, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, datetime)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_IntlDateFormatter_formatObject, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, datetime)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, format, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, locale, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_IntlDateFormatter_parse, 0, 1, MAY_BE_LONG|MAY_BE_DOUBLE|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, offset, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_IntlDateFormatter_localtime, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, offset, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_IntlDateFormatter_getErrorCode, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_IntlDateFormatter_getErrorMessage, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()


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

static zend_class_entry *register_class_IntlDateFormatter(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "IntlDateFormatter", class_IntlDateFormatter_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_NOT_SERIALIZABLE;

	zval const_FULL_value;
	ZVAL_LONG(&const_FULL_value, UDAT_FULL);
	zend_string *const_FULL_name = zend_string_init_interned("FULL", sizeof("FULL") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FULL_name, &const_FULL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FULL_name);

	zval const_LONG_value;
	ZVAL_LONG(&const_LONG_value, UDAT_LONG);
	zend_string *const_LONG_name = zend_string_init_interned("LONG", sizeof("LONG") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_LONG_name, &const_LONG_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_LONG_name);

	zval const_MEDIUM_value;
	ZVAL_LONG(&const_MEDIUM_value, UDAT_MEDIUM);
	zend_string *const_MEDIUM_name = zend_string_init_interned("MEDIUM", sizeof("MEDIUM") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_MEDIUM_name, &const_MEDIUM_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_MEDIUM_name);

	zval const_SHORT_value;
	ZVAL_LONG(&const_SHORT_value, UDAT_SHORT);
	zend_string *const_SHORT_name = zend_string_init_interned("SHORT", sizeof("SHORT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_SHORT_name, &const_SHORT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_SHORT_name);

	zval const_NONE_value;
	ZVAL_LONG(&const_NONE_value, UDAT_NONE);
	zend_string *const_NONE_name = zend_string_init_interned("NONE", sizeof("NONE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_NONE_name, &const_NONE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_NONE_name);

	zval const_RELATIVE_FULL_value;
	ZVAL_LONG(&const_RELATIVE_FULL_value, UDAT_FULL_RELATIVE);
	zend_string *const_RELATIVE_FULL_name = zend_string_init_interned("RELATIVE_FULL", sizeof("RELATIVE_FULL") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_RELATIVE_FULL_name, &const_RELATIVE_FULL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_RELATIVE_FULL_name);

	zval const_RELATIVE_LONG_value;
	ZVAL_LONG(&const_RELATIVE_LONG_value, UDAT_LONG_RELATIVE);
	zend_string *const_RELATIVE_LONG_name = zend_string_init_interned("RELATIVE_LONG", sizeof("RELATIVE_LONG") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_RELATIVE_LONG_name, &const_RELATIVE_LONG_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_RELATIVE_LONG_name);

	zval const_RELATIVE_MEDIUM_value;
	ZVAL_LONG(&const_RELATIVE_MEDIUM_value, UDAT_MEDIUM_RELATIVE);
	zend_string *const_RELATIVE_MEDIUM_name = zend_string_init_interned("RELATIVE_MEDIUM", sizeof("RELATIVE_MEDIUM") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_RELATIVE_MEDIUM_name, &const_RELATIVE_MEDIUM_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_RELATIVE_MEDIUM_name);

	zval const_RELATIVE_SHORT_value;
	ZVAL_LONG(&const_RELATIVE_SHORT_value, UDAT_SHORT_RELATIVE);
	zend_string *const_RELATIVE_SHORT_name = zend_string_init_interned("RELATIVE_SHORT", sizeof("RELATIVE_SHORT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_RELATIVE_SHORT_name, &const_RELATIVE_SHORT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_RELATIVE_SHORT_name);

	zval const_PATTERN_value;
	ZVAL_LONG(&const_PATTERN_value, UDAT_PATTERN);
	zend_string *const_PATTERN_name = zend_string_init_interned("PATTERN", sizeof("PATTERN") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PATTERN_name, &const_PATTERN_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PATTERN_name);

	zval const_GREGORIAN_value;
	ZVAL_LONG(&const_GREGORIAN_value, UCAL_GREGORIAN);
	zend_string *const_GREGORIAN_name = zend_string_init_interned("GREGORIAN", sizeof("GREGORIAN") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_GREGORIAN_name, &const_GREGORIAN_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_GREGORIAN_name);

	zval const_TRADITIONAL_value;
	ZVAL_LONG(&const_TRADITIONAL_value, UCAL_TRADITIONAL);
	zend_string *const_TRADITIONAL_name = zend_string_init_interned("TRADITIONAL", sizeof("TRADITIONAL") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_TRADITIONAL_name, &const_TRADITIONAL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_TRADITIONAL_name);

	return class_entry;
}
