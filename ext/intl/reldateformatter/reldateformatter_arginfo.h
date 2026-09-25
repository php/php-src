/* This is a generated file, edit reldateformatter.stub.php instead.
 * Stub hash: af29fb1710109d93ee24e216619e601ff7c72f81
 * Has decl header: yes */

#include "zend_enum.h"

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlRelativeDateTimeFormatter___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, locale, IS_STRING, 1, "null")
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, style, IntlRelativeDateTimeFormatterStyle, 0, "IntlRelativeDateTimeFormatterStyle::Long")
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, capitalizationContext, IntlRelativeDateTimeFormatterCapitalization, 0, "IntlRelativeDateTimeFormatterCapitalization::None")
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, numberFormatter, NumberFormatter, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_IntlRelativeDateTimeFormatter_format, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_MASK(0, offset, MAY_BE_LONG|MAY_BE_DOUBLE, NULL)
	ZEND_ARG_OBJ_INFO(0, unit, IntlRelativeDateTimeFormatterUnit, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlRelativeDateTimeFormatter_formatNumeric arginfo_class_IntlRelativeDateTimeFormatter_format

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_IntlRelativeDateTimeFormatter_combineDateAndTime, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, relativeDate, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, time, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_IntlRelativeDateTimeFormatter_getErrorCode, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_IntlRelativeDateTimeFormatter_getErrorMessage, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_METHOD(IntlRelativeDateTimeFormatter, __construct);
ZEND_METHOD(IntlRelativeDateTimeFormatter, format);
ZEND_METHOD(IntlRelativeDateTimeFormatter, formatNumeric);
ZEND_METHOD(IntlRelativeDateTimeFormatter, combineDateAndTime);
ZEND_METHOD(IntlRelativeDateTimeFormatter, getErrorCode);
ZEND_METHOD(IntlRelativeDateTimeFormatter, getErrorMessage);

static const zend_function_entry class_IntlRelativeDateTimeFormatter_methods[] = {
	ZEND_ME(IntlRelativeDateTimeFormatter, __construct, arginfo_class_IntlRelativeDateTimeFormatter___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(IntlRelativeDateTimeFormatter, format, arginfo_class_IntlRelativeDateTimeFormatter_format, ZEND_ACC_PUBLIC)
	ZEND_ME(IntlRelativeDateTimeFormatter, formatNumeric, arginfo_class_IntlRelativeDateTimeFormatter_formatNumeric, ZEND_ACC_PUBLIC)
	ZEND_ME(IntlRelativeDateTimeFormatter, combineDateAndTime, arginfo_class_IntlRelativeDateTimeFormatter_combineDateAndTime, ZEND_ACC_PUBLIC)
	ZEND_ME(IntlRelativeDateTimeFormatter, getErrorCode, arginfo_class_IntlRelativeDateTimeFormatter_getErrorCode, ZEND_ACC_PUBLIC)
	ZEND_ME(IntlRelativeDateTimeFormatter, getErrorMessage, arginfo_class_IntlRelativeDateTimeFormatter_getErrorMessage, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_IntlRelativeDateTimeFormatterStyle(void)
{
	zend_class_entry *class_entry = zend_register_internal_enum("IntlRelativeDateTimeFormatterStyle", IS_UNDEF, NULL);

	zend_enum_add_case_cstr(class_entry, "Long", NULL);

	zend_enum_add_case_cstr(class_entry, "Short", NULL);

	zend_enum_add_case_cstr(class_entry, "Narrow", NULL);

	return class_entry;
}

static zend_class_entry *register_class_IntlRelativeDateTimeFormatterCapitalization(void)
{
	zend_class_entry *class_entry = zend_register_internal_enum("IntlRelativeDateTimeFormatterCapitalization", IS_UNDEF, NULL);

	zend_enum_add_case_cstr(class_entry, "None", NULL);

	zend_enum_add_case_cstr(class_entry, "MiddleOfSentence", NULL);

	zend_enum_add_case_cstr(class_entry, "BeginningOfSentence", NULL);

	zend_enum_add_case_cstr(class_entry, "UiListAndMenu", NULL);

	zend_enum_add_case_cstr(class_entry, "Standalone", NULL);

	return class_entry;
}

static zend_class_entry *register_class_IntlRelativeDateTimeFormatterUnit(void)
{
	zend_class_entry *class_entry = zend_register_internal_enum("IntlRelativeDateTimeFormatterUnit", IS_UNDEF, NULL);

	zend_enum_add_case_cstr(class_entry, "Year", NULL);

	zend_enum_add_case_cstr(class_entry, "Quarter", NULL);

	zend_enum_add_case_cstr(class_entry, "Month", NULL);

	zend_enum_add_case_cstr(class_entry, "Week", NULL);

	zend_enum_add_case_cstr(class_entry, "Day", NULL);

	zend_enum_add_case_cstr(class_entry, "Hour", NULL);

	zend_enum_add_case_cstr(class_entry, "Minute", NULL);

	zend_enum_add_case_cstr(class_entry, "Second", NULL);

	zend_enum_add_case_cstr(class_entry, "Sunday", NULL);

	zend_enum_add_case_cstr(class_entry, "Monday", NULL);

	zend_enum_add_case_cstr(class_entry, "Tuesday", NULL);

	zend_enum_add_case_cstr(class_entry, "Wednesday", NULL);

	zend_enum_add_case_cstr(class_entry, "Thursday", NULL);

	zend_enum_add_case_cstr(class_entry, "Friday", NULL);

	zend_enum_add_case_cstr(class_entry, "Saturday", NULL);

	return class_entry;
}

static zend_class_entry *register_class_IntlRelativeDateTimeFormatter(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "IntlRelativeDateTimeFormatter", class_IntlRelativeDateTimeFormatter_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE);

	return class_entry;
}
