/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: e2c069ed3efafce65c85e844b5be4a94dd4d9744 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Uri_WhatWgError___construct, 0, 0, 3)
	ZEND_ARG_TYPE_INFO(0, uri, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, position, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, errorCode, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_UriInterface_getScheme, 0, 0, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_UriInterface_withScheme, 0, 1, IS_STATIC, 0)
	ZEND_ARG_TYPE_INFO(0, scheme, IS_STRING, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_Uri_UriInterface_getUser arginfo_class_Uri_UriInterface_getScheme

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_UriInterface_withUser, 0, 1, IS_STATIC, 0)
	ZEND_ARG_TYPE_INFO(0, user, IS_STRING, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_Uri_UriInterface_getPassword arginfo_class_Uri_UriInterface_getScheme

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_UriInterface_withPassword, 0, 1, IS_STATIC, 0)
	ZEND_ARG_TYPE_INFO(0, password, IS_STRING, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_Uri_UriInterface_getHost arginfo_class_Uri_UriInterface_getScheme

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_UriInterface_withHost, 0, 1, IS_STATIC, 0)
	ZEND_ARG_TYPE_INFO(0, host, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_UriInterface_getPort, 0, 0, IS_LONG, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_UriInterface_withPort, 0, 1, IS_STATIC, 0)
	ZEND_ARG_TYPE_INFO(0, port, IS_LONG, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_Uri_UriInterface_getPath arginfo_class_Uri_UriInterface_getScheme

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_UriInterface_withPath, 0, 1, IS_STATIC, 0)
	ZEND_ARG_TYPE_INFO(0, path, IS_STRING, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_Uri_UriInterface_getQuery arginfo_class_Uri_UriInterface_getScheme

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_UriInterface_withQuery, 0, 1, IS_STATIC, 0)
	ZEND_ARG_TYPE_INFO(0, query, IS_STRING, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_Uri_UriInterface_getFragment arginfo_class_Uri_UriInterface_getScheme

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_UriInterface_withFragment, 0, 1, IS_STATIC, 0)
	ZEND_ARG_TYPE_INFO(0, fragment, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_UriInterface_equalsTo, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, uri, Uri\\\125riInterface, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, excludeFragment, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_UriInterface_normalize, 0, 0, IS_STATIC, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_UriInterface_toNormalizedString, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Uri_UriInterface___toString arginfo_class_Uri_UriInterface_toNormalizedString

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_Rfc3986Uri_parse, 0, 1, IS_STATIC, 1)
	ZEND_ARG_TYPE_INFO(0, uri, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, baseUrl, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Uri_Rfc3986Uri___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, uri, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, baseUrl, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_Uri_Rfc3986Uri_getScheme arginfo_class_Uri_UriInterface_getScheme

#define arginfo_class_Uri_Rfc3986Uri_withScheme arginfo_class_Uri_UriInterface_withScheme

#define arginfo_class_Uri_Rfc3986Uri_getUser arginfo_class_Uri_UriInterface_getScheme

#define arginfo_class_Uri_Rfc3986Uri_withUser arginfo_class_Uri_UriInterface_withUser

#define arginfo_class_Uri_Rfc3986Uri_getPassword arginfo_class_Uri_UriInterface_getScheme

#define arginfo_class_Uri_Rfc3986Uri_withPassword arginfo_class_Uri_UriInterface_withPassword

#define arginfo_class_Uri_Rfc3986Uri_getHost arginfo_class_Uri_UriInterface_getScheme

#define arginfo_class_Uri_Rfc3986Uri_withHost arginfo_class_Uri_UriInterface_withHost

#define arginfo_class_Uri_Rfc3986Uri_getPort arginfo_class_Uri_UriInterface_getPort

#define arginfo_class_Uri_Rfc3986Uri_withPort arginfo_class_Uri_UriInterface_withPort

#define arginfo_class_Uri_Rfc3986Uri_getPath arginfo_class_Uri_UriInterface_getScheme

#define arginfo_class_Uri_Rfc3986Uri_withPath arginfo_class_Uri_UriInterface_withPath

#define arginfo_class_Uri_Rfc3986Uri_getQuery arginfo_class_Uri_UriInterface_getScheme

#define arginfo_class_Uri_Rfc3986Uri_withQuery arginfo_class_Uri_UriInterface_withQuery

#define arginfo_class_Uri_Rfc3986Uri_getFragment arginfo_class_Uri_UriInterface_getScheme

#define arginfo_class_Uri_Rfc3986Uri_withFragment arginfo_class_Uri_UriInterface_withFragment

#define arginfo_class_Uri_Rfc3986Uri_equalsTo arginfo_class_Uri_UriInterface_equalsTo

#define arginfo_class_Uri_Rfc3986Uri_normalize arginfo_class_Uri_UriInterface_normalize

#define arginfo_class_Uri_Rfc3986Uri_toNormalizedString arginfo_class_Uri_UriInterface_toNormalizedString

#define arginfo_class_Uri_Rfc3986Uri___toString arginfo_class_Uri_UriInterface_toNormalizedString

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_Rfc3986Uri___serialize, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_Rfc3986Uri___unserialize, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Uri_WhatWgUri_parse, 0, 1, MAY_BE_STATIC|MAY_BE_ARRAY)
	ZEND_ARG_TYPE_INFO(0, uri, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, baseUrl, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_Uri_WhatWgUri___construct arginfo_class_Uri_Rfc3986Uri___construct

#define arginfo_class_Uri_WhatWgUri_getScheme arginfo_class_Uri_UriInterface_getScheme

#define arginfo_class_Uri_WhatWgUri_withScheme arginfo_class_Uri_UriInterface_withScheme

#define arginfo_class_Uri_WhatWgUri_getUser arginfo_class_Uri_UriInterface_getScheme

#define arginfo_class_Uri_WhatWgUri_withUser arginfo_class_Uri_UriInterface_withUser

#define arginfo_class_Uri_WhatWgUri_getPassword arginfo_class_Uri_UriInterface_getScheme

#define arginfo_class_Uri_WhatWgUri_withPassword arginfo_class_Uri_UriInterface_withPassword

#define arginfo_class_Uri_WhatWgUri_getHost arginfo_class_Uri_UriInterface_getScheme

#define arginfo_class_Uri_WhatWgUri_withHost arginfo_class_Uri_UriInterface_withHost

#define arginfo_class_Uri_WhatWgUri_getPort arginfo_class_Uri_UriInterface_getPort

#define arginfo_class_Uri_WhatWgUri_withPort arginfo_class_Uri_UriInterface_withPort

#define arginfo_class_Uri_WhatWgUri_getPath arginfo_class_Uri_UriInterface_getScheme

#define arginfo_class_Uri_WhatWgUri_withPath arginfo_class_Uri_UriInterface_withPath

#define arginfo_class_Uri_WhatWgUri_getQuery arginfo_class_Uri_UriInterface_getScheme

#define arginfo_class_Uri_WhatWgUri_withQuery arginfo_class_Uri_UriInterface_withQuery

#define arginfo_class_Uri_WhatWgUri_getFragment arginfo_class_Uri_UriInterface_getScheme

#define arginfo_class_Uri_WhatWgUri_withFragment arginfo_class_Uri_UriInterface_withFragment

#define arginfo_class_Uri_WhatWgUri_equalsTo arginfo_class_Uri_UriInterface_equalsTo

#define arginfo_class_Uri_WhatWgUri_normalize arginfo_class_Uri_UriInterface_normalize

#define arginfo_class_Uri_WhatWgUri_toNormalizedString arginfo_class_Uri_UriInterface_toNormalizedString

#define arginfo_class_Uri_WhatWgUri___toString arginfo_class_Uri_UriInterface_toNormalizedString

#define arginfo_class_Uri_WhatWgUri___serialize arginfo_class_Uri_Rfc3986Uri___serialize

#define arginfo_class_Uri_WhatWgUri___unserialize arginfo_class_Uri_Rfc3986Uri___unserialize

ZEND_METHOD(Uri_WhatWgError, __construct);
ZEND_METHOD(Uri_Rfc3986Uri, parse);
ZEND_METHOD(Uri_Rfc3986Uri, __construct);
ZEND_METHOD(Uri_Rfc3986Uri, getScheme);
ZEND_METHOD(Uri_Rfc3986Uri, withScheme);
ZEND_METHOD(Uri_Rfc3986Uri, getUser);
ZEND_METHOD(Uri_Rfc3986Uri, withUser);
ZEND_METHOD(Uri_Rfc3986Uri, getPassword);
ZEND_METHOD(Uri_Rfc3986Uri, withPassword);
ZEND_METHOD(Uri_Rfc3986Uri, getHost);
ZEND_METHOD(Uri_Rfc3986Uri, withHost);
ZEND_METHOD(Uri_Rfc3986Uri, getPort);
ZEND_METHOD(Uri_Rfc3986Uri, withPort);
ZEND_METHOD(Uri_Rfc3986Uri, getPath);
ZEND_METHOD(Uri_Rfc3986Uri, withPath);
ZEND_METHOD(Uri_Rfc3986Uri, getQuery);
ZEND_METHOD(Uri_Rfc3986Uri, withQuery);
ZEND_METHOD(Uri_Rfc3986Uri, getFragment);
ZEND_METHOD(Uri_Rfc3986Uri, withFragment);
ZEND_METHOD(Uri_Rfc3986Uri, equalsTo);
ZEND_METHOD(Uri_Rfc3986Uri, normalize);
ZEND_METHOD(Uri_Rfc3986Uri, toNormalizedString);
ZEND_METHOD(Uri_Rfc3986Uri, __toString);
ZEND_METHOD(Uri_Rfc3986Uri, __serialize);
ZEND_METHOD(Uri_Rfc3986Uri, __unserialize);
ZEND_METHOD(Uri_WhatWgUri, parse);
ZEND_METHOD(Uri_WhatWgUri, __construct);
ZEND_METHOD(Uri_WhatWgUri, __unserialize);

static const zend_function_entry class_Uri_WhatWgError_methods[] = {
	ZEND_ME(Uri_WhatWgError, __construct, arginfo_class_Uri_WhatWgError___construct, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_Uri_UriInterface_methods[] = {
	ZEND_RAW_FENTRY("getScheme", NULL, arginfo_class_Uri_UriInterface_getScheme, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("withScheme", NULL, arginfo_class_Uri_UriInterface_withScheme, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("getUser", NULL, arginfo_class_Uri_UriInterface_getUser, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("withUser", NULL, arginfo_class_Uri_UriInterface_withUser, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("getPassword", NULL, arginfo_class_Uri_UriInterface_getPassword, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("withPassword", NULL, arginfo_class_Uri_UriInterface_withPassword, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("getHost", NULL, arginfo_class_Uri_UriInterface_getHost, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("withHost", NULL, arginfo_class_Uri_UriInterface_withHost, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("getPort", NULL, arginfo_class_Uri_UriInterface_getPort, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("withPort", NULL, arginfo_class_Uri_UriInterface_withPort, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("getPath", NULL, arginfo_class_Uri_UriInterface_getPath, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("withPath", NULL, arginfo_class_Uri_UriInterface_withPath, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("getQuery", NULL, arginfo_class_Uri_UriInterface_getQuery, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("withQuery", NULL, arginfo_class_Uri_UriInterface_withQuery, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("getFragment", NULL, arginfo_class_Uri_UriInterface_getFragment, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("withFragment", NULL, arginfo_class_Uri_UriInterface_withFragment, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("equalsTo", NULL, arginfo_class_Uri_UriInterface_equalsTo, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("normalize", NULL, arginfo_class_Uri_UriInterface_normalize, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("toNormalizedString", NULL, arginfo_class_Uri_UriInterface_toNormalizedString, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("__toString", NULL, arginfo_class_Uri_UriInterface___toString, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_Uri_Rfc3986Uri_methods[] = {
	ZEND_ME(Uri_Rfc3986Uri, parse, arginfo_class_Uri_Rfc3986Uri_parse, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Uri_Rfc3986Uri, __construct, arginfo_class_Uri_Rfc3986Uri___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Rfc3986Uri, getScheme, arginfo_class_Uri_Rfc3986Uri_getScheme, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Rfc3986Uri, withScheme, arginfo_class_Uri_Rfc3986Uri_withScheme, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Rfc3986Uri, getUser, arginfo_class_Uri_Rfc3986Uri_getUser, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Rfc3986Uri, withUser, arginfo_class_Uri_Rfc3986Uri_withUser, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Rfc3986Uri, getPassword, arginfo_class_Uri_Rfc3986Uri_getPassword, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Rfc3986Uri, withPassword, arginfo_class_Uri_Rfc3986Uri_withPassword, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Rfc3986Uri, getHost, arginfo_class_Uri_Rfc3986Uri_getHost, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Rfc3986Uri, withHost, arginfo_class_Uri_Rfc3986Uri_withHost, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Rfc3986Uri, getPort, arginfo_class_Uri_Rfc3986Uri_getPort, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Rfc3986Uri, withPort, arginfo_class_Uri_Rfc3986Uri_withPort, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Rfc3986Uri, getPath, arginfo_class_Uri_Rfc3986Uri_getPath, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Rfc3986Uri, withPath, arginfo_class_Uri_Rfc3986Uri_withPath, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Rfc3986Uri, getQuery, arginfo_class_Uri_Rfc3986Uri_getQuery, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Rfc3986Uri, withQuery, arginfo_class_Uri_Rfc3986Uri_withQuery, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Rfc3986Uri, getFragment, arginfo_class_Uri_Rfc3986Uri_getFragment, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Rfc3986Uri, withFragment, arginfo_class_Uri_Rfc3986Uri_withFragment, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Rfc3986Uri, equalsTo, arginfo_class_Uri_Rfc3986Uri_equalsTo, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Rfc3986Uri, normalize, arginfo_class_Uri_Rfc3986Uri_normalize, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Rfc3986Uri, toNormalizedString, arginfo_class_Uri_Rfc3986Uri_toNormalizedString, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Rfc3986Uri, __toString, arginfo_class_Uri_Rfc3986Uri___toString, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Rfc3986Uri, __serialize, arginfo_class_Uri_Rfc3986Uri___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Rfc3986Uri, __unserialize, arginfo_class_Uri_Rfc3986Uri___unserialize, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_Uri_WhatWgUri_methods[] = {
	ZEND_ME(Uri_WhatWgUri, parse, arginfo_class_Uri_WhatWgUri_parse, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Uri_WhatWgUri, __construct, arginfo_class_Uri_WhatWgUri___construct, ZEND_ACC_PUBLIC)
	ZEND_RAW_FENTRY("getScheme", zim_Uri_Rfc3986Uri_getScheme, arginfo_class_Uri_WhatWgUri_getScheme, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("withScheme", zim_Uri_Rfc3986Uri_withScheme, arginfo_class_Uri_WhatWgUri_withScheme, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("getUser", zim_Uri_Rfc3986Uri_getUser, arginfo_class_Uri_WhatWgUri_getUser, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("withUser", zim_Uri_Rfc3986Uri_withUser, arginfo_class_Uri_WhatWgUri_withUser, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("getPassword", zim_Uri_Rfc3986Uri_getPassword, arginfo_class_Uri_WhatWgUri_getPassword, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("withPassword", zim_Uri_Rfc3986Uri_withPassword, arginfo_class_Uri_WhatWgUri_withPassword, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("getHost", zim_Uri_Rfc3986Uri_getHost, arginfo_class_Uri_WhatWgUri_getHost, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("withHost", zim_Uri_Rfc3986Uri_withHost, arginfo_class_Uri_WhatWgUri_withHost, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("getPort", zim_Uri_Rfc3986Uri_getPort, arginfo_class_Uri_WhatWgUri_getPort, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("withPort", zim_Uri_Rfc3986Uri_withPort, arginfo_class_Uri_WhatWgUri_withPort, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("getPath", zim_Uri_Rfc3986Uri_getPath, arginfo_class_Uri_WhatWgUri_getPath, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("withPath", zim_Uri_Rfc3986Uri_withPath, arginfo_class_Uri_WhatWgUri_withPath, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("getQuery", zim_Uri_Rfc3986Uri_getQuery, arginfo_class_Uri_WhatWgUri_getQuery, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("withQuery", zim_Uri_Rfc3986Uri_withQuery, arginfo_class_Uri_WhatWgUri_withQuery, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("getFragment", zim_Uri_Rfc3986Uri_getFragment, arginfo_class_Uri_WhatWgUri_getFragment, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("withFragment", zim_Uri_Rfc3986Uri_withFragment, arginfo_class_Uri_WhatWgUri_withFragment, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("equalsTo", zim_Uri_Rfc3986Uri_equalsTo, arginfo_class_Uri_WhatWgUri_equalsTo, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("normalize", zim_Uri_Rfc3986Uri_normalize, arginfo_class_Uri_WhatWgUri_normalize, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("toNormalizedString", zim_Uri_Rfc3986Uri_toNormalizedString, arginfo_class_Uri_WhatWgUri_toNormalizedString, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("__toString", zim_Uri_Rfc3986Uri___toString, arginfo_class_Uri_WhatWgUri___toString, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("__serialize", zim_Uri_Rfc3986Uri___serialize, arginfo_class_Uri_WhatWgUri___serialize, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_ME(Uri_WhatWgUri, __unserialize, arginfo_class_Uri_WhatWgUri___unserialize, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static void register_php_uri_symbols(int module_number)
{
	REGISTER_STRING_CONSTANT("Uri\\URI_PARSER_RFC3986", URI_PARSER_RFC3986, CONST_PERSISTENT);
	REGISTER_STRING_CONSTANT("Uri\\URI_PARSER_WHATWG", URI_PARSER_WHATWG, CONST_PERSISTENT);
}

static zend_class_entry *register_class_Uri_UriException(zend_class_entry *class_entry_Exception)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Uri", "UriException", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_Exception, ZEND_ACC_ABSTRACT|ZEND_ACC_NO_DYNAMIC_PROPERTIES);

	return class_entry;
}

static zend_class_entry *register_class_Uri_UninitializedUriException(zend_class_entry *class_entry_Uri_UriException)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Uri", "UninitializedUriException", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_Uri_UriException, ZEND_ACC_NO_DYNAMIC_PROPERTIES);

	return class_entry;
}

static zend_class_entry *register_class_Uri_UriOperationException(zend_class_entry *class_entry_Uri_UriException)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Uri", "UriOperationException", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_Uri_UriException, ZEND_ACC_NO_DYNAMIC_PROPERTIES);

	return class_entry;
}

static zend_class_entry *register_class_Uri_InvalidUriException(zend_class_entry *class_entry_Uri_UriException)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Uri", "InvalidUriException", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_Uri_UriException, ZEND_ACC_NO_DYNAMIC_PROPERTIES);

	zval property_errors_default_value;
	ZVAL_UNDEF(&property_errors_default_value);
	zend_string *property_errors_name = zend_string_init("errors", sizeof("errors") - 1, 1);
	zend_declare_typed_property(class_entry, property_errors_name, &property_errors_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_ARRAY));
	zend_string_release(property_errors_name);

	return class_entry;
}

static zend_class_entry *register_class_Uri_WhatWgError(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Uri", "WhatWgError", class_Uri_WhatWgError_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_READONLY_CLASS);

	zval const_ERROR_TYPE_DOMAIN_TO_ASCII_value;
	ZVAL_LONG(&const_ERROR_TYPE_DOMAIN_TO_ASCII_value, LXB_URL_ERROR_TYPE_DOMAIN_TO_ASCII);
	zend_string *const_ERROR_TYPE_DOMAIN_TO_ASCII_name = zend_string_init_interned("ERROR_TYPE_DOMAIN_TO_ASCII", sizeof("ERROR_TYPE_DOMAIN_TO_ASCII") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ERROR_TYPE_DOMAIN_TO_ASCII_name, &const_ERROR_TYPE_DOMAIN_TO_ASCII_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ERROR_TYPE_DOMAIN_TO_ASCII_name);

	zval const_ERROR_TYPE_DOMAIN_TO_UNICODE_value;
	ZVAL_LONG(&const_ERROR_TYPE_DOMAIN_TO_UNICODE_value, LXB_URL_ERROR_TYPE_DOMAIN_TO_UNICODE);
	zend_string *const_ERROR_TYPE_DOMAIN_TO_UNICODE_name = zend_string_init_interned("ERROR_TYPE_DOMAIN_TO_UNICODE", sizeof("ERROR_TYPE_DOMAIN_TO_UNICODE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ERROR_TYPE_DOMAIN_TO_UNICODE_name, &const_ERROR_TYPE_DOMAIN_TO_UNICODE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ERROR_TYPE_DOMAIN_TO_UNICODE_name);

	zval const_ERROR_TYPE_DOMAIN_INVALID_CODE_POINT_value;
	ZVAL_LONG(&const_ERROR_TYPE_DOMAIN_INVALID_CODE_POINT_value, LXB_URL_ERROR_TYPE_DOMAIN_INVALID_CODE_POINT);
	zend_string *const_ERROR_TYPE_DOMAIN_INVALID_CODE_POINT_name = zend_string_init_interned("ERROR_TYPE_DOMAIN_INVALID_CODE_POINT", sizeof("ERROR_TYPE_DOMAIN_INVALID_CODE_POINT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ERROR_TYPE_DOMAIN_INVALID_CODE_POINT_name, &const_ERROR_TYPE_DOMAIN_INVALID_CODE_POINT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ERROR_TYPE_DOMAIN_INVALID_CODE_POINT_name);

	zval const_ERROR_TYPE_HOST_INVALID_CODE_POINT_value;
	ZVAL_LONG(&const_ERROR_TYPE_HOST_INVALID_CODE_POINT_value, LXB_URL_ERROR_TYPE_HOST_INVALID_CODE_POINT);
	zend_string *const_ERROR_TYPE_HOST_INVALID_CODE_POINT_name = zend_string_init_interned("ERROR_TYPE_HOST_INVALID_CODE_POINT", sizeof("ERROR_TYPE_HOST_INVALID_CODE_POINT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ERROR_TYPE_HOST_INVALID_CODE_POINT_name, &const_ERROR_TYPE_HOST_INVALID_CODE_POINT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ERROR_TYPE_HOST_INVALID_CODE_POINT_name);

	zval const_ERROR_TYPE_IPV4_EMPTY_PART_value;
	ZVAL_LONG(&const_ERROR_TYPE_IPV4_EMPTY_PART_value, LXB_URL_ERROR_TYPE_IPV4_EMPTY_PART);
	zend_string *const_ERROR_TYPE_IPV4_EMPTY_PART_name = zend_string_init_interned("ERROR_TYPE_IPV4_EMPTY_PART", sizeof("ERROR_TYPE_IPV4_EMPTY_PART") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ERROR_TYPE_IPV4_EMPTY_PART_name, &const_ERROR_TYPE_IPV4_EMPTY_PART_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ERROR_TYPE_IPV4_EMPTY_PART_name);

	zval const_ERROR_TYPE_IPV4_TOO_MANY_PARTS_value;
	ZVAL_LONG(&const_ERROR_TYPE_IPV4_TOO_MANY_PARTS_value, LXB_URL_ERROR_TYPE_IPV4_TOO_MANY_PARTS);
	zend_string *const_ERROR_TYPE_IPV4_TOO_MANY_PARTS_name = zend_string_init_interned("ERROR_TYPE_IPV4_TOO_MANY_PARTS", sizeof("ERROR_TYPE_IPV4_TOO_MANY_PARTS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ERROR_TYPE_IPV4_TOO_MANY_PARTS_name, &const_ERROR_TYPE_IPV4_TOO_MANY_PARTS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ERROR_TYPE_IPV4_TOO_MANY_PARTS_name);

	zval const_ERROR_TYPE_IPV4_NON_NUMERIC_PART_value;
	ZVAL_LONG(&const_ERROR_TYPE_IPV4_NON_NUMERIC_PART_value, LXB_URL_ERROR_TYPE_IPV4_NON_NUMERIC_PART);
	zend_string *const_ERROR_TYPE_IPV4_NON_NUMERIC_PART_name = zend_string_init_interned("ERROR_TYPE_IPV4_NON_NUMERIC_PART", sizeof("ERROR_TYPE_IPV4_NON_NUMERIC_PART") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ERROR_TYPE_IPV4_NON_NUMERIC_PART_name, &const_ERROR_TYPE_IPV4_NON_NUMERIC_PART_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ERROR_TYPE_IPV4_NON_NUMERIC_PART_name);

	zval const_ERROR_TYPE_IPV4_NON_DECIMAL_PART_value;
	ZVAL_LONG(&const_ERROR_TYPE_IPV4_NON_DECIMAL_PART_value, LXB_URL_ERROR_TYPE_IPV4_NON_DECIMAL_PART);
	zend_string *const_ERROR_TYPE_IPV4_NON_DECIMAL_PART_name = zend_string_init_interned("ERROR_TYPE_IPV4_NON_DECIMAL_PART", sizeof("ERROR_TYPE_IPV4_NON_DECIMAL_PART") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ERROR_TYPE_IPV4_NON_DECIMAL_PART_name, &const_ERROR_TYPE_IPV4_NON_DECIMAL_PART_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ERROR_TYPE_IPV4_NON_DECIMAL_PART_name);

	zval const_ERROR_TYPE_IPV4_OUT_OF_RANGE_PART_value;
	ZVAL_LONG(&const_ERROR_TYPE_IPV4_OUT_OF_RANGE_PART_value, LXB_URL_ERROR_TYPE_IPV4_OUT_OF_RANGE_PART);
	zend_string *const_ERROR_TYPE_IPV4_OUT_OF_RANGE_PART_name = zend_string_init_interned("ERROR_TYPE_IPV4_OUT_OF_RANGE_PART", sizeof("ERROR_TYPE_IPV4_OUT_OF_RANGE_PART") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ERROR_TYPE_IPV4_OUT_OF_RANGE_PART_name, &const_ERROR_TYPE_IPV4_OUT_OF_RANGE_PART_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ERROR_TYPE_IPV4_OUT_OF_RANGE_PART_name);

	zval const_ERROR_TYPE_IPV6_UNCLOSED_value;
	ZVAL_LONG(&const_ERROR_TYPE_IPV6_UNCLOSED_value, LXB_URL_ERROR_TYPE_IPV6_UNCLOSED);
	zend_string *const_ERROR_TYPE_IPV6_UNCLOSED_name = zend_string_init_interned("ERROR_TYPE_IPV6_UNCLOSED", sizeof("ERROR_TYPE_IPV6_UNCLOSED") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ERROR_TYPE_IPV6_UNCLOSED_name, &const_ERROR_TYPE_IPV6_UNCLOSED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ERROR_TYPE_IPV6_UNCLOSED_name);

	zval const_ERROR_TYPE_IPV6_INVALID_COMPRESSION_value;
	ZVAL_LONG(&const_ERROR_TYPE_IPV6_INVALID_COMPRESSION_value, LXB_URL_ERROR_TYPE_IPV6_INVALID_COMPRESSION);
	zend_string *const_ERROR_TYPE_IPV6_INVALID_COMPRESSION_name = zend_string_init_interned("ERROR_TYPE_IPV6_INVALID_COMPRESSION", sizeof("ERROR_TYPE_IPV6_INVALID_COMPRESSION") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ERROR_TYPE_IPV6_INVALID_COMPRESSION_name, &const_ERROR_TYPE_IPV6_INVALID_COMPRESSION_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ERROR_TYPE_IPV6_INVALID_COMPRESSION_name);

	zval const_ERROR_TYPE_IPV6_TOO_MANY_PIECES_value;
	ZVAL_LONG(&const_ERROR_TYPE_IPV6_TOO_MANY_PIECES_value, LXB_URL_ERROR_TYPE_IPV6_TOO_MANY_PIECES);
	zend_string *const_ERROR_TYPE_IPV6_TOO_MANY_PIECES_name = zend_string_init_interned("ERROR_TYPE_IPV6_TOO_MANY_PIECES", sizeof("ERROR_TYPE_IPV6_TOO_MANY_PIECES") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ERROR_TYPE_IPV6_TOO_MANY_PIECES_name, &const_ERROR_TYPE_IPV6_TOO_MANY_PIECES_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ERROR_TYPE_IPV6_TOO_MANY_PIECES_name);

	zval const_ERROR_TYPE_IPV6_MULTIPLE_COMPRESSION_value;
	ZVAL_LONG(&const_ERROR_TYPE_IPV6_MULTIPLE_COMPRESSION_value, LXB_URL_ERROR_TYPE_IPV6_MULTIPLE_COMPRESSION);
	zend_string *const_ERROR_TYPE_IPV6_MULTIPLE_COMPRESSION_name = zend_string_init_interned("ERROR_TYPE_IPV6_MULTIPLE_COMPRESSION", sizeof("ERROR_TYPE_IPV6_MULTIPLE_COMPRESSION") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ERROR_TYPE_IPV6_MULTIPLE_COMPRESSION_name, &const_ERROR_TYPE_IPV6_MULTIPLE_COMPRESSION_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ERROR_TYPE_IPV6_MULTIPLE_COMPRESSION_name);

	zval const_ERROR_TYPE_IPV6_INVALID_CODE_POINT_value;
	ZVAL_LONG(&const_ERROR_TYPE_IPV6_INVALID_CODE_POINT_value, LXB_URL_ERROR_TYPE_IPV6_INVALID_CODE_POINT);
	zend_string *const_ERROR_TYPE_IPV6_INVALID_CODE_POINT_name = zend_string_init_interned("ERROR_TYPE_IPV6_INVALID_CODE_POINT", sizeof("ERROR_TYPE_IPV6_INVALID_CODE_POINT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ERROR_TYPE_IPV6_INVALID_CODE_POINT_name, &const_ERROR_TYPE_IPV6_INVALID_CODE_POINT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ERROR_TYPE_IPV6_INVALID_CODE_POINT_name);

	zval const_ERROR_TYPE_IPV6_TOO_FEW_PIECES_value;
	ZVAL_LONG(&const_ERROR_TYPE_IPV6_TOO_FEW_PIECES_value, LXB_URL_ERROR_TYPE_IPV6_TOO_FEW_PIECES);
	zend_string *const_ERROR_TYPE_IPV6_TOO_FEW_PIECES_name = zend_string_init_interned("ERROR_TYPE_IPV6_TOO_FEW_PIECES", sizeof("ERROR_TYPE_IPV6_TOO_FEW_PIECES") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ERROR_TYPE_IPV6_TOO_FEW_PIECES_name, &const_ERROR_TYPE_IPV6_TOO_FEW_PIECES_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ERROR_TYPE_IPV6_TOO_FEW_PIECES_name);

	zval const_ERROR_TYPE_IPV4_IN_IPV6_TOO_MANY_PIECES_value;
	ZVAL_LONG(&const_ERROR_TYPE_IPV4_IN_IPV6_TOO_MANY_PIECES_value, LXB_URL_ERROR_TYPE_IPV4_IN_IPV6_TOO_MANY_PIECES);
	zend_string *const_ERROR_TYPE_IPV4_IN_IPV6_TOO_MANY_PIECES_name = zend_string_init_interned("ERROR_TYPE_IPV4_IN_IPV6_TOO_MANY_PIECES", sizeof("ERROR_TYPE_IPV4_IN_IPV6_TOO_MANY_PIECES") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ERROR_TYPE_IPV4_IN_IPV6_TOO_MANY_PIECES_name, &const_ERROR_TYPE_IPV4_IN_IPV6_TOO_MANY_PIECES_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ERROR_TYPE_IPV4_IN_IPV6_TOO_MANY_PIECES_name);

	zval const_ERROR_TYPE_IPV4_IN_IPV6_INVALID_CODE_POINT_value;
	ZVAL_LONG(&const_ERROR_TYPE_IPV4_IN_IPV6_INVALID_CODE_POINT_value, LXB_URL_ERROR_TYPE_IPV4_IN_IPV6_INVALID_CODE_POINT);
	zend_string *const_ERROR_TYPE_IPV4_IN_IPV6_INVALID_CODE_POINT_name = zend_string_init_interned("ERROR_TYPE_IPV4_IN_IPV6_INVALID_CODE_POINT", sizeof("ERROR_TYPE_IPV4_IN_IPV6_INVALID_CODE_POINT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ERROR_TYPE_IPV4_IN_IPV6_INVALID_CODE_POINT_name, &const_ERROR_TYPE_IPV4_IN_IPV6_INVALID_CODE_POINT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ERROR_TYPE_IPV4_IN_IPV6_INVALID_CODE_POINT_name);

	zval const_ERROR_TYPE_IPV4_IN_IPV6_OUT_OF_RANGE_PART_value;
	ZVAL_LONG(&const_ERROR_TYPE_IPV4_IN_IPV6_OUT_OF_RANGE_PART_value, LXB_URL_ERROR_TYPE_IPV4_IN_IPV6_OUT_OF_RANGE_PART);
	zend_string *const_ERROR_TYPE_IPV4_IN_IPV6_OUT_OF_RANGE_PART_name = zend_string_init_interned("ERROR_TYPE_IPV4_IN_IPV6_OUT_OF_RANGE_PART", sizeof("ERROR_TYPE_IPV4_IN_IPV6_OUT_OF_RANGE_PART") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ERROR_TYPE_IPV4_IN_IPV6_OUT_OF_RANGE_PART_name, &const_ERROR_TYPE_IPV4_IN_IPV6_OUT_OF_RANGE_PART_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ERROR_TYPE_IPV4_IN_IPV6_OUT_OF_RANGE_PART_name);

	zval const_ERROR_TYPE_IPV4_IN_IPV6_TOO_FEW_PARTS_value;
	ZVAL_LONG(&const_ERROR_TYPE_IPV4_IN_IPV6_TOO_FEW_PARTS_value, LXB_URL_ERROR_TYPE_IPV4_IN_IPV6_TOO_FEW_PARTS);
	zend_string *const_ERROR_TYPE_IPV4_IN_IPV6_TOO_FEW_PARTS_name = zend_string_init_interned("ERROR_TYPE_IPV4_IN_IPV6_TOO_FEW_PARTS", sizeof("ERROR_TYPE_IPV4_IN_IPV6_TOO_FEW_PARTS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ERROR_TYPE_IPV4_IN_IPV6_TOO_FEW_PARTS_name, &const_ERROR_TYPE_IPV4_IN_IPV6_TOO_FEW_PARTS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ERROR_TYPE_IPV4_IN_IPV6_TOO_FEW_PARTS_name);

	zval const_ERROR_TYPE_INVALID_URL_UNIT_value;
	ZVAL_LONG(&const_ERROR_TYPE_INVALID_URL_UNIT_value, LXB_URL_ERROR_TYPE_INVALID_URL_UNIT);
	zend_string *const_ERROR_TYPE_INVALID_URL_UNIT_name = zend_string_init_interned("ERROR_TYPE_INVALID_URL_UNIT", sizeof("ERROR_TYPE_INVALID_URL_UNIT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ERROR_TYPE_INVALID_URL_UNIT_name, &const_ERROR_TYPE_INVALID_URL_UNIT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ERROR_TYPE_INVALID_URL_UNIT_name);

	zval const_ERROR_TYPE_SPECIAL_SCHEME_MISSING_FOLLOWING_SOLIDUS_value;
	ZVAL_LONG(&const_ERROR_TYPE_SPECIAL_SCHEME_MISSING_FOLLOWING_SOLIDUS_value, LXB_URL_ERROR_TYPE_SPECIAL_SCHEME_MISSING_FOLLOWING_SOLIDUS);
	zend_string *const_ERROR_TYPE_SPECIAL_SCHEME_MISSING_FOLLOWING_SOLIDUS_name = zend_string_init_interned("ERROR_TYPE_SPECIAL_SCHEME_MISSING_FOLLOWING_SOLIDUS", sizeof("ERROR_TYPE_SPECIAL_SCHEME_MISSING_FOLLOWING_SOLIDUS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ERROR_TYPE_SPECIAL_SCHEME_MISSING_FOLLOWING_SOLIDUS_name, &const_ERROR_TYPE_SPECIAL_SCHEME_MISSING_FOLLOWING_SOLIDUS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ERROR_TYPE_SPECIAL_SCHEME_MISSING_FOLLOWING_SOLIDUS_name);

	zval const_ERROR_TYPE_MISSING_SCHEME_NON_RELATIVE_URL_value;
	ZVAL_LONG(&const_ERROR_TYPE_MISSING_SCHEME_NON_RELATIVE_URL_value, LXB_URL_ERROR_TYPE_MISSING_SCHEME_NON_RELATIVE_URL);
	zend_string *const_ERROR_TYPE_MISSING_SCHEME_NON_RELATIVE_URL_name = zend_string_init_interned("ERROR_TYPE_MISSING_SCHEME_NON_RELATIVE_URL", sizeof("ERROR_TYPE_MISSING_SCHEME_NON_RELATIVE_URL") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ERROR_TYPE_MISSING_SCHEME_NON_RELATIVE_URL_name, &const_ERROR_TYPE_MISSING_SCHEME_NON_RELATIVE_URL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ERROR_TYPE_MISSING_SCHEME_NON_RELATIVE_URL_name);

	zval const_ERROR_TYPE_INVALID_REVERSE_SOLIDUS_value;
	ZVAL_LONG(&const_ERROR_TYPE_INVALID_REVERSE_SOLIDUS_value, LXB_URL_ERROR_TYPE_INVALID_REVERSE_SOLIDUS);
	zend_string *const_ERROR_TYPE_INVALID_REVERSE_SOLIDUS_name = zend_string_init_interned("ERROR_TYPE_INVALID_REVERSE_SOLIDUS", sizeof("ERROR_TYPE_INVALID_REVERSE_SOLIDUS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ERROR_TYPE_INVALID_REVERSE_SOLIDUS_name, &const_ERROR_TYPE_INVALID_REVERSE_SOLIDUS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ERROR_TYPE_INVALID_REVERSE_SOLIDUS_name);

	zval const_ERROR_TYPE_INVALID_CREDENTIALS_value;
	ZVAL_LONG(&const_ERROR_TYPE_INVALID_CREDENTIALS_value, LXB_URL_ERROR_TYPE_INVALID_CREDENTIALS);
	zend_string *const_ERROR_TYPE_INVALID_CREDENTIALS_name = zend_string_init_interned("ERROR_TYPE_INVALID_CREDENTIALS", sizeof("ERROR_TYPE_INVALID_CREDENTIALS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ERROR_TYPE_INVALID_CREDENTIALS_name, &const_ERROR_TYPE_INVALID_CREDENTIALS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ERROR_TYPE_INVALID_CREDENTIALS_name);

	zval const_ERROR_TYPE_HOST_MISSING_value;
	ZVAL_LONG(&const_ERROR_TYPE_HOST_MISSING_value, LXB_URL_ERROR_TYPE_HOST_MISSING);
	zend_string *const_ERROR_TYPE_HOST_MISSING_name = zend_string_init_interned("ERROR_TYPE_HOST_MISSING", sizeof("ERROR_TYPE_HOST_MISSING") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ERROR_TYPE_HOST_MISSING_name, &const_ERROR_TYPE_HOST_MISSING_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ERROR_TYPE_HOST_MISSING_name);

	zval const_ERROR_TYPE_PORT_OUT_OF_RANGE_value;
	ZVAL_LONG(&const_ERROR_TYPE_PORT_OUT_OF_RANGE_value, LXB_URL_ERROR_TYPE_PORT_OUT_OF_RANGE);
	zend_string *const_ERROR_TYPE_PORT_OUT_OF_RANGE_name = zend_string_init_interned("ERROR_TYPE_PORT_OUT_OF_RANGE", sizeof("ERROR_TYPE_PORT_OUT_OF_RANGE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ERROR_TYPE_PORT_OUT_OF_RANGE_name, &const_ERROR_TYPE_PORT_OUT_OF_RANGE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ERROR_TYPE_PORT_OUT_OF_RANGE_name);

	zval const_ERROR_TYPE_PORT_INVALID_value;
	ZVAL_LONG(&const_ERROR_TYPE_PORT_INVALID_value, LXB_URL_ERROR_TYPE_PORT_INVALID);
	zend_string *const_ERROR_TYPE_PORT_INVALID_name = zend_string_init_interned("ERROR_TYPE_PORT_INVALID", sizeof("ERROR_TYPE_PORT_INVALID") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ERROR_TYPE_PORT_INVALID_name, &const_ERROR_TYPE_PORT_INVALID_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ERROR_TYPE_PORT_INVALID_name);

	zval const_ERROR_TYPE_FILE_INVALID_WINDOWS_DRIVE_LETTER_value;
	ZVAL_LONG(&const_ERROR_TYPE_FILE_INVALID_WINDOWS_DRIVE_LETTER_value, LXB_URL_ERROR_TYPE_FILE_INVALID_WINDOWS_DRIVE_LETTER);
	zend_string *const_ERROR_TYPE_FILE_INVALID_WINDOWS_DRIVE_LETTER_name = zend_string_init_interned("ERROR_TYPE_FILE_INVALID_WINDOWS_DRIVE_LETTER", sizeof("ERROR_TYPE_FILE_INVALID_WINDOWS_DRIVE_LETTER") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ERROR_TYPE_FILE_INVALID_WINDOWS_DRIVE_LETTER_name, &const_ERROR_TYPE_FILE_INVALID_WINDOWS_DRIVE_LETTER_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ERROR_TYPE_FILE_INVALID_WINDOWS_DRIVE_LETTER_name);

	zval const_ERROR_TYPE_FILE_INVALID_WINDOWS_DRIVE_LETTER_HOST_value;
	ZVAL_LONG(&const_ERROR_TYPE_FILE_INVALID_WINDOWS_DRIVE_LETTER_HOST_value, LXB_URL_ERROR_TYPE_FILE_INVALID_WINDOWS_DRIVE_LETTER_HOST);
	zend_string *const_ERROR_TYPE_FILE_INVALID_WINDOWS_DRIVE_LETTER_HOST_name = zend_string_init_interned("ERROR_TYPE_FILE_INVALID_WINDOWS_DRIVE_LETTER_HOST", sizeof("ERROR_TYPE_FILE_INVALID_WINDOWS_DRIVE_LETTER_HOST") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ERROR_TYPE_FILE_INVALID_WINDOWS_DRIVE_LETTER_HOST_name, &const_ERROR_TYPE_FILE_INVALID_WINDOWS_DRIVE_LETTER_HOST_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ERROR_TYPE_FILE_INVALID_WINDOWS_DRIVE_LETTER_HOST_name);

	zval property_position_default_value;
	ZVAL_UNDEF(&property_position_default_value);
	zend_string *property_position_name = zend_string_init("position", sizeof("position") - 1, 1);
	zend_declare_typed_property(class_entry, property_position_name, &property_position_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_position_name);

	zval property_errorCode_default_value;
	ZVAL_UNDEF(&property_errorCode_default_value);
	zend_string *property_errorCode_name = zend_string_init("errorCode", sizeof("errorCode") - 1, 1);
	zend_declare_typed_property(class_entry, property_errorCode_name, &property_errorCode_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_errorCode_name);

	return class_entry;
}

static zend_class_entry *register_class_Uri_UriInterface(zend_class_entry *class_entry_Stringable)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Uri", "UriInterface", class_Uri_UriInterface_methods);
	class_entry = zend_register_internal_interface(&ce);
	zend_class_implements(class_entry, 1, class_entry_Stringable);

	return class_entry;
}

static zend_class_entry *register_class_Uri_Rfc3986Uri(zend_class_entry *class_entry_Uri_UriInterface)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Uri", "Rfc3986Uri", class_Uri_Rfc3986Uri_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_READONLY_CLASS);
	zend_class_implements(class_entry, 1, class_entry_Uri_UriInterface);

	zval property_scheme_default_value;
	ZVAL_UNDEF(&property_scheme_default_value);
	zend_declare_typed_property(class_entry, ZSTR_KNOWN(ZEND_STR_SCHEME), &property_scheme_default_value, ZEND_ACC_PRIVATE|ZEND_ACC_READONLY|ZEND_ACC_VIRTUAL, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));

	zval property_user_default_value;
	ZVAL_UNDEF(&property_user_default_value);
	zend_declare_typed_property(class_entry, ZSTR_KNOWN(ZEND_STR_USER), &property_user_default_value, ZEND_ACC_PRIVATE|ZEND_ACC_READONLY|ZEND_ACC_VIRTUAL, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));

	zval property_password_default_value;
	ZVAL_UNDEF(&property_password_default_value);
	zend_string *property_password_name = zend_string_init("password", sizeof("password") - 1, 1);
	zend_declare_typed_property(class_entry, property_password_name, &property_password_default_value, ZEND_ACC_PRIVATE|ZEND_ACC_READONLY|ZEND_ACC_VIRTUAL, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));
	zend_string_release(property_password_name);

	zval property_host_default_value;
	ZVAL_UNDEF(&property_host_default_value);
	zend_declare_typed_property(class_entry, ZSTR_KNOWN(ZEND_STR_HOST), &property_host_default_value, ZEND_ACC_PRIVATE|ZEND_ACC_READONLY|ZEND_ACC_VIRTUAL, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));

	zval property_port_default_value;
	ZVAL_UNDEF(&property_port_default_value);
	zend_declare_typed_property(class_entry, ZSTR_KNOWN(ZEND_STR_PORT), &property_port_default_value, ZEND_ACC_PRIVATE|ZEND_ACC_READONLY|ZEND_ACC_VIRTUAL, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG|MAY_BE_NULL));

	zval property_path_default_value;
	ZVAL_UNDEF(&property_path_default_value);
	zend_declare_typed_property(class_entry, ZSTR_KNOWN(ZEND_STR_PATH), &property_path_default_value, ZEND_ACC_PRIVATE|ZEND_ACC_READONLY|ZEND_ACC_VIRTUAL, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));

	zval property_query_default_value;
	ZVAL_UNDEF(&property_query_default_value);
	zend_declare_typed_property(class_entry, ZSTR_KNOWN(ZEND_STR_QUERY), &property_query_default_value, ZEND_ACC_PRIVATE|ZEND_ACC_READONLY|ZEND_ACC_VIRTUAL, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));

	zval property_fragment_default_value;
	ZVAL_UNDEF(&property_fragment_default_value);
	zend_declare_typed_property(class_entry, ZSTR_KNOWN(ZEND_STR_FRAGMENT), &property_fragment_default_value, ZEND_ACC_PRIVATE|ZEND_ACC_READONLY|ZEND_ACC_VIRTUAL, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));

	return class_entry;
}

static zend_class_entry *register_class_Uri_WhatWgUri(zend_class_entry *class_entry_Uri_UriInterface)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Uri", "WhatWgUri", class_Uri_WhatWgUri_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_READONLY_CLASS);
	zend_class_implements(class_entry, 1, class_entry_Uri_UriInterface);

	zval property_scheme_default_value;
	ZVAL_UNDEF(&property_scheme_default_value);
	zend_declare_typed_property(class_entry, ZSTR_KNOWN(ZEND_STR_SCHEME), &property_scheme_default_value, ZEND_ACC_PRIVATE|ZEND_ACC_READONLY|ZEND_ACC_VIRTUAL, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));

	zval property_user_default_value;
	ZVAL_UNDEF(&property_user_default_value);
	zend_declare_typed_property(class_entry, ZSTR_KNOWN(ZEND_STR_USER), &property_user_default_value, ZEND_ACC_PRIVATE|ZEND_ACC_READONLY|ZEND_ACC_VIRTUAL, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));

	zval property_password_default_value;
	ZVAL_UNDEF(&property_password_default_value);
	zend_string *property_password_name = zend_string_init("password", sizeof("password") - 1, 1);
	zend_declare_typed_property(class_entry, property_password_name, &property_password_default_value, ZEND_ACC_PRIVATE|ZEND_ACC_READONLY|ZEND_ACC_VIRTUAL, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));
	zend_string_release(property_password_name);

	zval property_host_default_value;
	ZVAL_UNDEF(&property_host_default_value);
	zend_declare_typed_property(class_entry, ZSTR_KNOWN(ZEND_STR_HOST), &property_host_default_value, ZEND_ACC_PRIVATE|ZEND_ACC_READONLY|ZEND_ACC_VIRTUAL, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));

	zval property_port_default_value;
	ZVAL_UNDEF(&property_port_default_value);
	zend_declare_typed_property(class_entry, ZSTR_KNOWN(ZEND_STR_PORT), &property_port_default_value, ZEND_ACC_PRIVATE|ZEND_ACC_READONLY|ZEND_ACC_VIRTUAL, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG|MAY_BE_NULL));

	zval property_path_default_value;
	ZVAL_UNDEF(&property_path_default_value);
	zend_declare_typed_property(class_entry, ZSTR_KNOWN(ZEND_STR_PATH), &property_path_default_value, ZEND_ACC_PRIVATE|ZEND_ACC_READONLY|ZEND_ACC_VIRTUAL, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));

	zval property_query_default_value;
	ZVAL_UNDEF(&property_query_default_value);
	zend_declare_typed_property(class_entry, ZSTR_KNOWN(ZEND_STR_QUERY), &property_query_default_value, ZEND_ACC_PRIVATE|ZEND_ACC_READONLY|ZEND_ACC_VIRTUAL, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));

	zval property_fragment_default_value;
	ZVAL_UNDEF(&property_fragment_default_value);
	zend_declare_typed_property(class_entry, ZSTR_KNOWN(ZEND_STR_FRAGMENT), &property_fragment_default_value, ZEND_ACC_PRIVATE|ZEND_ACC_READONLY|ZEND_ACC_VIRTUAL, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));

	return class_entry;
}
