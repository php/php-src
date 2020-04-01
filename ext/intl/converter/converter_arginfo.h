/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_UConverter___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, destination_encoding, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, source_encoding, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_UConverter_convert, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, reverse, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_UConverter_fromUCallback, 0, 0, 4)
	ZEND_ARG_TYPE_INFO(0, reason, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, source, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, codePoint, IS_LONG, 0)
	ZEND_ARG_INFO(1, error)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_UConverter_getAliases, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_UConverter_getAvailable, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_UConverter_getDestinationEncoding arginfo_class_UConverter_getAvailable

#define arginfo_class_UConverter_getDestinationType arginfo_class_UConverter_getAvailable

#define arginfo_class_UConverter_getErrorCode arginfo_class_UConverter_getAvailable

#define arginfo_class_UConverter_getErrorMessage arginfo_class_UConverter_getAvailable

#define arginfo_class_UConverter_getSourceEncoding arginfo_class_UConverter_getAvailable

#define arginfo_class_UConverter_getSourceType arginfo_class_UConverter_getAvailable

#define arginfo_class_UConverter_getStandards arginfo_class_UConverter_getAvailable

#define arginfo_class_UConverter_getSubstChars arginfo_class_UConverter_getAvailable

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_UConverter_reasonText, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, reason, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_UConverter_setDestinationEncoding, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, encoding, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_UConverter_setSourceEncoding arginfo_class_UConverter_setDestinationEncoding

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_UConverter_setSubstChars, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, chars, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_UConverter_toUCallback, 0, 0, 4)
	ZEND_ARG_TYPE_INFO(0, reason, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, source, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, codeUnits, IS_STRING, 0)
	ZEND_ARG_INFO(1, error)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_UConverter_transcode, 0, 0, 3)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, toEncoding, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, fromEncoding, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 1, "null")
ZEND_END_ARG_INFO()
