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
