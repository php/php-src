/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_INFO_EX(arginfo_grapheme_strlen, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, input, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_grapheme_strpos, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, haystack, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, needle, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_grapheme_stripos arginfo_grapheme_strpos

#define arginfo_grapheme_strrpos arginfo_grapheme_strpos

#define arginfo_grapheme_strripos arginfo_grapheme_strpos

ZEND_BEGIN_ARG_INFO_EX(arginfo_grapheme_substr, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, start, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_grapheme_strstr, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, haystack, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, needle, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, before_needle, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_grapheme_stristr arginfo_grapheme_strstr

ZEND_BEGIN_ARG_INFO_EX(arginfo_grapheme_extract, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, haystack, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, size, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, extract_type, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, start, IS_LONG, 0)
	ZEND_ARG_INFO(1, next)
ZEND_END_ARG_INFO()
