/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_INFO_EX(arginfo_preg_match, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, subject, IS_STRING, 0)
	ZEND_ARG_INFO(1, subpatterns)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_preg_match_all arginfo_preg_match

ZEND_BEGIN_ARG_INFO_EX(arginfo_preg_replace, 0, 0, 3)
	ZEND_ARG_INFO(0, regex)
	ZEND_ARG_INFO(0, replace)
	ZEND_ARG_INFO(0, subject)
	ZEND_ARG_TYPE_INFO(0, limit, IS_LONG, 0)
	ZEND_ARG_INFO(1, count)
ZEND_END_ARG_INFO()

#define arginfo_preg_filter arginfo_preg_replace

ZEND_BEGIN_ARG_INFO_EX(arginfo_preg_replace_callback, 0, 0, 3)
	ZEND_ARG_INFO(0, regex)
	ZEND_ARG_INFO(0, callback)
	ZEND_ARG_INFO(0, subject)
	ZEND_ARG_TYPE_INFO(0, limit, IS_LONG, 0)
	ZEND_ARG_INFO(1, count)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_preg_replace_callback_array, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_ARRAY, 0)
	ZEND_ARG_INFO(0, subject)
	ZEND_ARG_TYPE_INFO(0, limit, IS_LONG, 0)
	ZEND_ARG_INFO(1, count)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_preg_split, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, subject, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, limit, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_preg_quote, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, delim_char, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_preg_grep, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, regex, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, input, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_preg_last_error, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()
