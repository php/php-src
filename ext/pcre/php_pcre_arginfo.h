/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 8b3d31a0fc1542bcb432f1200a7b9f49afbf0f1f */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_preg_match, 0, 2, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, subject, IS_STRING, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, matches, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_preg_match_all arginfo_preg_match

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_preg_replace, 0, 3, MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_NULL)
	ZEND_ARG_TYPE_MASK(0, pattern, MAY_BE_STRING|MAY_BE_ARRAY, NULL)
	ZEND_ARG_TYPE_MASK(0, replacement, MAY_BE_STRING|MAY_BE_ARRAY, NULL)
	ZEND_ARG_TYPE_MASK(0, subject, MAY_BE_STRING|MAY_BE_ARRAY, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, limit, IS_LONG, 0, "-1")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, count, "null")
ZEND_END_ARG_INFO()

#define arginfo_preg_filter arginfo_preg_replace

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_preg_replace_callback, 0, 3, MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_NULL)
	ZEND_ARG_TYPE_MASK(0, pattern, MAY_BE_STRING|MAY_BE_ARRAY, NULL)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
	ZEND_ARG_TYPE_MASK(0, subject, MAY_BE_STRING|MAY_BE_ARRAY, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, limit, IS_LONG, 0, "-1")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, count, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_preg_replace_callback_array, 0, 2, MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_NULL)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_ARRAY, 0)
	ZEND_ARG_TYPE_MASK(0, subject, MAY_BE_STRING|MAY_BE_ARRAY, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, limit, IS_LONG, 0, "-1")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, count, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_preg_split, 0, 2, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, subject, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, limit, IS_LONG, 0, "-1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_preg_quote, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, delimiter, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_preg_grep, 0, 2, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_preg_last_error, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_preg_last_error_msg, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()


ZEND_FUNCTION(preg_match);
ZEND_FUNCTION(preg_match_all);
ZEND_FUNCTION(preg_replace);
ZEND_FUNCTION(preg_filter);
ZEND_FUNCTION(preg_replace_callback);
ZEND_FUNCTION(preg_replace_callback_array);
ZEND_FUNCTION(preg_split);
ZEND_FUNCTION(preg_quote);
ZEND_FUNCTION(preg_grep);
ZEND_FUNCTION(preg_last_error);
ZEND_FUNCTION(preg_last_error_msg);


static const zend_function_entry ext_functions[] = {
	ZEND_FE(preg_match, arginfo_preg_match)
	ZEND_FE(preg_match_all, arginfo_preg_match_all)
	ZEND_FE(preg_replace, arginfo_preg_replace)
	ZEND_FE(preg_filter, arginfo_preg_filter)
	ZEND_FE(preg_replace_callback, arginfo_preg_replace_callback)
	ZEND_FE(preg_replace_callback_array, arginfo_preg_replace_callback_array)
	ZEND_FE(preg_split, arginfo_preg_split)
	ZEND_FE(preg_quote, arginfo_preg_quote)
	ZEND_FE(preg_grep, arginfo_preg_grep)
	ZEND_FE(preg_last_error, arginfo_preg_last_error)
	ZEND_FE(preg_last_error_msg, arginfo_preg_last_error_msg)
	ZEND_FE_END
};
