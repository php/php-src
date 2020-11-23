/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: a06da9ea0191ed78ee7af8f0d9eaccb17dfa4b20 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_token_get_all, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, code, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_token_name, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, id, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_PhpToken_tokenize arginfo_token_get_all

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PhpToken___construct, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, id, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, text, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, line, IS_LONG, 0, "-1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, pos, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_PhpToken_is, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, kind)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_PhpToken_isIgnorable, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_PhpToken_getTokenName, 0, 0, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_PhpToken___toString, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()


ZEND_FUNCTION(token_get_all);
ZEND_FUNCTION(token_name);
ZEND_METHOD(PhpToken, tokenize);
ZEND_METHOD(PhpToken, __construct);
ZEND_METHOD(PhpToken, is);
ZEND_METHOD(PhpToken, isIgnorable);
ZEND_METHOD(PhpToken, getTokenName);
ZEND_METHOD(PhpToken, __toString);


static const zend_function_entry ext_functions[] = {
	ZEND_FE(token_get_all, arginfo_token_get_all)
	ZEND_FE(token_name, arginfo_token_name)
	ZEND_FE_END
};


static const zend_function_entry class_PhpToken_methods[] = {
	ZEND_ME(PhpToken, tokenize, arginfo_class_PhpToken_tokenize, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(PhpToken, __construct, arginfo_class_PhpToken___construct, ZEND_ACC_PUBLIC|ZEND_ACC_FINAL)
	ZEND_ME(PhpToken, is, arginfo_class_PhpToken_is, ZEND_ACC_PUBLIC)
	ZEND_ME(PhpToken, isIgnorable, arginfo_class_PhpToken_isIgnorable, ZEND_ACC_PUBLIC)
	ZEND_ME(PhpToken, getTokenName, arginfo_class_PhpToken_getTokenName, ZEND_ACC_PUBLIC)
	ZEND_ME(PhpToken, __toString, arginfo_class_PhpToken___toString, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};
