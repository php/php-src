/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_MessageFormatter___construct, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_MessageFormatter_create arginfo_class_MessageFormatter___construct

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_MessageFormatter_format, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, args, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_MessageFormatter_formatMessage, 0, 0, 3)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, args, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_MessageFormatter_parse, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_MessageFormatter_parseMessage, 0, 0, 3)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, source, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_MessageFormatter_setPattern, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_MessageFormatter_getPattern, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_MessageFormatter_getLocale arginfo_class_MessageFormatter_getPattern

#define arginfo_class_MessageFormatter_getErrorCode arginfo_class_MessageFormatter_getPattern

#define arginfo_class_MessageFormatter_getErrorMessage arginfo_class_MessageFormatter_getPattern
