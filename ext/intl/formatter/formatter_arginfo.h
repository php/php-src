/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: de7625297bcdb8c7e4944a319c019ed68508ea2c */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_NumberFormatter___construct, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, style, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, pattern, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_NumberFormatter_create arginfo_class_NumberFormatter___construct

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_NumberFormatter_format, 0, 0, 1)
	ZEND_ARG_TYPE_MASK(0, num, MAY_BE_LONG|MAY_BE_DOUBLE, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, type, IS_LONG, 0, "NumberFormatter::TYPE_DEFAULT")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_NumberFormatter_parse, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, type, IS_LONG, 0, "NumberFormatter::TYPE_DOUBLE")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, offset, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_NumberFormatter_formatCurrency, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, amount, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, currency, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_NumberFormatter_parseCurrency, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_INFO(1, currency)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, offset, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_NumberFormatter_setAttribute, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, attribute, IS_LONG, 0)
	ZEND_ARG_TYPE_MASK(0, value, MAY_BE_LONG|MAY_BE_DOUBLE, NULL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_NumberFormatter_getAttribute, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, attribute, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_NumberFormatter_setTextAttribute, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, attribute, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_NumberFormatter_getTextAttribute arginfo_class_NumberFormatter_getAttribute

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_NumberFormatter_setSymbol, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, symbol, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_NumberFormatter_getSymbol, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, symbol, IS_LONG, 0)
ZEND_END_ARG_INFO()

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


ZEND_METHOD(NumberFormatter, __construct);
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


static const zend_function_entry class_NumberFormatter_methods[] = {
	ZEND_ME(NumberFormatter, __construct, arginfo_class_NumberFormatter___construct, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(create, numfmt_create, arginfo_class_NumberFormatter_create, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME_MAPPING(format, numfmt_format, arginfo_class_NumberFormatter_format, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(parse, numfmt_parse, arginfo_class_NumberFormatter_parse, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(formatCurrency, numfmt_format_currency, arginfo_class_NumberFormatter_formatCurrency, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(parseCurrency, numfmt_parse_currency, arginfo_class_NumberFormatter_parseCurrency, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(setAttribute, numfmt_set_attribute, arginfo_class_NumberFormatter_setAttribute, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getAttribute, numfmt_get_attribute, arginfo_class_NumberFormatter_getAttribute, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(setTextAttribute, numfmt_set_text_attribute, arginfo_class_NumberFormatter_setTextAttribute, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getTextAttribute, numfmt_get_text_attribute, arginfo_class_NumberFormatter_getTextAttribute, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(setSymbol, numfmt_set_symbol, arginfo_class_NumberFormatter_setSymbol, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getSymbol, numfmt_get_symbol, arginfo_class_NumberFormatter_getSymbol, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(setPattern, numfmt_set_pattern, arginfo_class_NumberFormatter_setPattern, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getPattern, numfmt_get_pattern, arginfo_class_NumberFormatter_getPattern, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getLocale, numfmt_get_locale, arginfo_class_NumberFormatter_getLocale, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getErrorCode, numfmt_get_error_code, arginfo_class_NumberFormatter_getErrorCode, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getErrorMessage, numfmt_get_error_message, arginfo_class_NumberFormatter_getErrorMessage, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};
