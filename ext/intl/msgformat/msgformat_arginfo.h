/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: d595f5c582996ebb96ab39df8cb56c4cf6c8dfcf */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_MessageFormatter___construct, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_INFO_EX(arginfo_class_MessageFormatter_create, 0, 2, MessageFormatter, 1)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_MessageFormatter_format, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, values, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_MessageFormatter_formatMessage, 0, 3, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, values, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_MessageFormatter_parse, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_MessageFormatter_parseMessage, 0, 3, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, message, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_MessageFormatter_setPattern, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_MessageFormatter_getPattern, 0, 0, MAY_BE_STRING|MAY_BE_FALSE)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_MessageFormatter_getLocale, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_MessageFormatter_getErrorCode, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_MessageFormatter_getErrorMessage arginfo_class_MessageFormatter_getLocale

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
	ZEND_RAW_FENTRY("create", zif_msgfmt_create, arginfo_class_MessageFormatter_create, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC, NULL, NULL)
	ZEND_RAW_FENTRY("format", zif_msgfmt_format, arginfo_class_MessageFormatter_format, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("formatMessage", zif_msgfmt_format_message, arginfo_class_MessageFormatter_formatMessage, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC, NULL, NULL)
	ZEND_RAW_FENTRY("parse", zif_msgfmt_parse, arginfo_class_MessageFormatter_parse, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("parseMessage", zif_msgfmt_parse_message, arginfo_class_MessageFormatter_parseMessage, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC, NULL, NULL)
	ZEND_RAW_FENTRY("setPattern", zif_msgfmt_set_pattern, arginfo_class_MessageFormatter_setPattern, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("getPattern", zif_msgfmt_get_pattern, arginfo_class_MessageFormatter_getPattern, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("getLocale", zif_msgfmt_get_locale, arginfo_class_MessageFormatter_getLocale, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("getErrorCode", zif_msgfmt_get_error_code, arginfo_class_MessageFormatter_getErrorCode, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("getErrorMessage", zif_msgfmt_get_error_message, arginfo_class_MessageFormatter_getErrorMessage, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_FE_END
};

static zend_class_entry *register_class_MessageFormatter(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "MessageFormatter", class_MessageFormatter_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_NOT_SERIALIZABLE;

	return class_entry;
}
