/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_NumberFormatter___construct, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, style, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, pattern, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()

#define arginfo_class_NumberFormatter_create arginfo_class_NumberFormatter___construct

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_NumberFormatter_format, 0, 0, 1)
	ZEND_ARG_TYPE_MASK(0, value, MAY_BE_LONG|MAY_BE_DOUBLE, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, type, IS_LONG, 0, "NumberFormatter::TYPE_DEFAULT")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_NumberFormatter_parse, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, type, IS_LONG, 0, "NumberFormatter::TYPE_DOUBLE")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, position, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_NumberFormatter_formatCurrency, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, value, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, currency, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_NumberFormatter_parseCurrency, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
	ZEND_ARG_INFO(1, currency)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, position, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_NumberFormatter_setAttribute, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, attr, IS_LONG, 0)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_NumberFormatter_getAttribute, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, attr, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_NumberFormatter_setTextAttribute, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, attr, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_NumberFormatter_getTextAttribute arginfo_class_NumberFormatter_getAttribute

#define arginfo_class_NumberFormatter_setSymbol arginfo_class_NumberFormatter_setTextAttribute

#define arginfo_class_NumberFormatter_getSymbol arginfo_class_NumberFormatter_getAttribute

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_NumberFormatter_setPattern, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_NumberFormatter_getPattern, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_NumberFormatter_getLocale, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, type, IS_LONG, 0, "ULOC_ACTUAL_LOCALE")
ZEND_END_ARG_INFO()

#define arginfo_class_NumberFormatter_getErrorCode arginfo_class_NumberFormatter_getPattern

#define arginfo_class_NumberFormatter_getErrorMessage arginfo_class_NumberFormatter_getPattern

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_numfmt_create, 0, 2, NumberFormatter, 1)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, style, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, pattern, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_numfmt_format, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, fmt, NumberFormatter, 0)
	ZEND_ARG_TYPE_MASK(0, value, MAY_BE_LONG|MAY_BE_DOUBLE, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, type, IS_LONG, 0, "NumberFormatter::TYPE_DEFAULT")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_numfmt_parse, 0, 2, MAY_BE_LONG|MAY_BE_DOUBLE|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, fmt, NumberFormatter, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, type, IS_LONG, 0, "NumberFormatter::TYPE_DOUBLE")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, position, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_numfmt_format_currency, 0, 3, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, fmt, NumberFormatter, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, currency, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_numfmt_parse_currency, 0, 3, MAY_BE_DOUBLE|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, fmt, NumberFormatter, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
	ZEND_ARG_INFO(1, currency)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, position, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_numfmt_set_attribute, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, fmt, NumberFormatter, 0)
	ZEND_ARG_TYPE_INFO(0, attr, IS_LONG, 0)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_numfmt_get_attribute, 0, 2, double, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, fmt, NumberFormatter, 0)
	ZEND_ARG_TYPE_INFO(0, attr, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_numfmt_set_text_attribute, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, fmt, NumberFormatter, 0)
	ZEND_ARG_TYPE_INFO(0, attr, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_numfmt_get_text_attribute, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, fmt, NumberFormatter, 0)
	ZEND_ARG_TYPE_INFO(0, attr, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_numfmt_set_symbol arginfo_numfmt_set_text_attribute

#define arginfo_numfmt_get_symbol arginfo_numfmt_get_text_attribute

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_numfmt_set_pattern, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, fmt, NumberFormatter, 0)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_numfmt_get_pattern, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, fmt, NumberFormatter, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_numfmt_get_locale, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, fmt, NumberFormatter, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, type, IS_LONG, 0, "ULOC_ACTUAL_LOCALE")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_numfmt_get_error_code, 0, 1, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, fmt, NumberFormatter, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_numfmt_get_error_message, 0, 1, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO(0, fmt, NumberFormatter, 0)
ZEND_END_ARG_INFO()
