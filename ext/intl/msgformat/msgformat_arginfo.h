/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: ece8c5157dc3a62ca2951d058b3c5497bb5cb836 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_MessageFormatter___construct, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_MessageFormatter_create arginfo_class_MessageFormatter___construct

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_MessageFormatter_format, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, values, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_MessageFormatter_formatMessage, 0, 0, 3)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, values, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_MessageFormatter_parse, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_MessageFormatter_parseMessage, 0, 0, 3)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, message, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_MessageFormatter_setPattern, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_MessageFormatter_getPattern, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_MessageFormatter_getLocale arginfo_class_MessageFormatter_getPattern

#define arginfo_class_MessageFormatter_getErrorCode arginfo_class_MessageFormatter_getPattern

#define arginfo_class_MessageFormatter_getErrorMessage arginfo_class_MessageFormatter_getPattern


ZEND_METHOD(MessageFormatter, __construct);
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


static const zend_function_entry class_MessageFormatter_methods[] = {
	ZEND_ME(MessageFormatter, __construct, arginfo_class_MessageFormatter___construct, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(create, msgfmt_create, arginfo_class_MessageFormatter_create, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME_MAPPING(format, msgfmt_format, arginfo_class_MessageFormatter_format, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(formatMessage, msgfmt_format_message, arginfo_class_MessageFormatter_formatMessage, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME_MAPPING(parse, msgfmt_parse, arginfo_class_MessageFormatter_parse, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(parseMessage, msgfmt_parse_message, arginfo_class_MessageFormatter_parseMessage, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME_MAPPING(setPattern, msgfmt_set_pattern, arginfo_class_MessageFormatter_setPattern, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getPattern, msgfmt_get_pattern, arginfo_class_MessageFormatter_getPattern, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getLocale, msgfmt_get_locale, arginfo_class_MessageFormatter_getLocale, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getErrorCode, msgfmt_get_error_code, arginfo_class_MessageFormatter_getErrorCode, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getErrorMessage, msgfmt_get_error_message, arginfo_class_MessageFormatter_getErrorMessage, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};
