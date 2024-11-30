/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: dd07ef16c13b1cf2e32cb92b4e69710656b5fae0 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_Uri_getScheme, 0, 0, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_Uri_withScheme, 0, 1, IS_STATIC, 0)
	ZEND_ARG_TYPE_INFO(0, encodedScheme, IS_STRING, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_Uri_Uri_getUser arginfo_class_Uri_Uri_getScheme

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_Uri_withUser, 0, 1, IS_STATIC, 0)
	ZEND_ARG_TYPE_INFO(0, encodedUser, IS_STRING, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_Uri_Uri_getPassword arginfo_class_Uri_Uri_getScheme

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_Uri_withPassword, 0, 1, IS_STATIC, 0)
	ZEND_ARG_TYPE_INFO(0, encodedPassword, IS_STRING, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_Uri_Uri_getHost arginfo_class_Uri_Uri_getScheme

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_Uri_withHost, 0, 1, IS_STATIC, 0)
	ZEND_ARG_TYPE_INFO(0, encodedHost, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_Uri_getPort, 0, 0, IS_LONG, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_Uri_withPort, 0, 1, IS_STATIC, 0)
	ZEND_ARG_TYPE_INFO(0, encodedPort, IS_LONG, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_Uri_Uri_getPath arginfo_class_Uri_Uri_getScheme

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_Uri_withPath, 0, 1, IS_STATIC, 0)
	ZEND_ARG_TYPE_INFO(0, encodedPath, IS_STRING, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_Uri_Uri_getQuery arginfo_class_Uri_Uri_getScheme

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_Uri_withQuery, 0, 1, IS_STATIC, 0)
	ZEND_ARG_TYPE_INFO(0, encodedQuery, IS_STRING, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_Uri_Uri_getFragment arginfo_class_Uri_Uri_getScheme

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_Uri_withFragment, 0, 1, IS_STATIC, 0)
	ZEND_ARG_TYPE_INFO(0, encodedFragment, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_Uri_equals, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, uri, Uri\\\125ri, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, excludeFragment, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_Uri_normalize, 0, 0, IS_STATIC, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_Uri_toNormalizedString, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Uri_Uri_toString arginfo_class_Uri_Uri_toNormalizedString

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_Uri_resolve, 0, 1, IS_STATIC, 0)
	ZEND_ARG_TYPE_INFO(0, uri, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_Rfc3986Uri_parse, 0, 1, IS_STATIC, 1)
	ZEND_ARG_TYPE_INFO(0, uri, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, baseUrl, IS_STRING, 1, "null")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, errors, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Uri_Rfc3986Uri___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, uri, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, baseUrl, IS_STRING, 1, "null")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, errors, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_Uri_Rfc3986Uri_getScheme arginfo_class_Uri_Uri_getScheme

#define arginfo_class_Uri_Rfc3986Uri_withScheme arginfo_class_Uri_Uri_withScheme

#define arginfo_class_Uri_Rfc3986Uri_getUser arginfo_class_Uri_Uri_getScheme

#define arginfo_class_Uri_Rfc3986Uri_withUser arginfo_class_Uri_Uri_withUser

#define arginfo_class_Uri_Rfc3986Uri_getPassword arginfo_class_Uri_Uri_getScheme

#define arginfo_class_Uri_Rfc3986Uri_withPassword arginfo_class_Uri_Uri_withPassword

#define arginfo_class_Uri_Rfc3986Uri_getHost arginfo_class_Uri_Uri_getScheme

#define arginfo_class_Uri_Rfc3986Uri_withHost arginfo_class_Uri_Uri_withHost

#define arginfo_class_Uri_Rfc3986Uri_getPort arginfo_class_Uri_Uri_getPort

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_Rfc3986Uri_withPort, 0, 1, IS_STATIC, 0)
	ZEND_ARG_TYPE_INFO(0, port, IS_LONG, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_Uri_Rfc3986Uri_getPath arginfo_class_Uri_Uri_getScheme

#define arginfo_class_Uri_Rfc3986Uri_withPath arginfo_class_Uri_Uri_withPath

#define arginfo_class_Uri_Rfc3986Uri_getQuery arginfo_class_Uri_Uri_getScheme

#define arginfo_class_Uri_Rfc3986Uri_withQuery arginfo_class_Uri_Uri_withQuery

#define arginfo_class_Uri_Rfc3986Uri_getFragment arginfo_class_Uri_Uri_getScheme

#define arginfo_class_Uri_Rfc3986Uri_withFragment arginfo_class_Uri_Uri_withFragment

#define arginfo_class_Uri_Rfc3986Uri_equals arginfo_class_Uri_Uri_equals

#define arginfo_class_Uri_Rfc3986Uri_normalize arginfo_class_Uri_Uri_normalize

#define arginfo_class_Uri_Rfc3986Uri_toNormalizedString arginfo_class_Uri_Uri_toNormalizedString

#define arginfo_class_Uri_Rfc3986Uri_toString arginfo_class_Uri_Uri_toNormalizedString

#define arginfo_class_Uri_Rfc3986Uri_resolve arginfo_class_Uri_Uri_resolve

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_Rfc3986Uri___serialize, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_Rfc3986Uri___unserialize, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Uri_WhatWgUri_parse arginfo_class_Uri_Rfc3986Uri_parse

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Uri_WhatWgUri___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, uri, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, baseUrl, IS_STRING, 1, "null")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, softErrors, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_Uri_WhatWgUri_getScheme arginfo_class_Uri_Uri_getScheme

#define arginfo_class_Uri_WhatWgUri_withScheme arginfo_class_Uri_Uri_withScheme

#define arginfo_class_Uri_WhatWgUri_getUser arginfo_class_Uri_Uri_getScheme

#define arginfo_class_Uri_WhatWgUri_withUser arginfo_class_Uri_Uri_withUser

#define arginfo_class_Uri_WhatWgUri_getPassword arginfo_class_Uri_Uri_getScheme

#define arginfo_class_Uri_WhatWgUri_withPassword arginfo_class_Uri_Uri_withPassword

#define arginfo_class_Uri_WhatWgUri_getHost arginfo_class_Uri_Uri_getScheme

#define arginfo_class_Uri_WhatWgUri_withHost arginfo_class_Uri_Uri_withHost

#define arginfo_class_Uri_WhatWgUri_getPort arginfo_class_Uri_Uri_getPort

#define arginfo_class_Uri_WhatWgUri_withPort arginfo_class_Uri_Uri_withPort

#define arginfo_class_Uri_WhatWgUri_getPath arginfo_class_Uri_Uri_getScheme

#define arginfo_class_Uri_WhatWgUri_withPath arginfo_class_Uri_Uri_withPath

#define arginfo_class_Uri_WhatWgUri_getQuery arginfo_class_Uri_Uri_getScheme

#define arginfo_class_Uri_WhatWgUri_withQuery arginfo_class_Uri_Uri_withQuery

#define arginfo_class_Uri_WhatWgUri_getFragment arginfo_class_Uri_Uri_getScheme

#define arginfo_class_Uri_WhatWgUri_withFragment arginfo_class_Uri_Uri_withFragment

#define arginfo_class_Uri_WhatWgUri_equals arginfo_class_Uri_Uri_equals

#define arginfo_class_Uri_WhatWgUri_normalize arginfo_class_Uri_Uri_normalize

#define arginfo_class_Uri_WhatWgUri_toNormalizedString arginfo_class_Uri_Uri_toNormalizedString

#define arginfo_class_Uri_WhatWgUri_toString arginfo_class_Uri_Uri_toNormalizedString

#define arginfo_class_Uri_WhatWgUri_resolve arginfo_class_Uri_Uri_resolve

#define arginfo_class_Uri_WhatWgUri___serialize arginfo_class_Uri_Rfc3986Uri___serialize

#define arginfo_class_Uri_WhatWgUri___unserialize arginfo_class_Uri_Rfc3986Uri___unserialize

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Uri_WhatWg_WhatWgError___construct, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, context, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO(0, type, Uri\\WhatWg\\WhatWgErrorType, 0)
ZEND_END_ARG_INFO()

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
ZEND_METHOD(Uri_Rfc3986Uri, equals);
ZEND_METHOD(Uri_Rfc3986Uri, normalize);
ZEND_METHOD(Uri_Rfc3986Uri, toNormalizedString);
ZEND_METHOD(Uri_Rfc3986Uri, toString);
ZEND_METHOD(Uri_Rfc3986Uri, resolve);
ZEND_METHOD(Uri_Rfc3986Uri, __serialize);
ZEND_METHOD(Uri_Rfc3986Uri, __unserialize);
ZEND_METHOD(Uri_WhatWgUri, parse);
ZEND_METHOD(Uri_WhatWgUri, __construct);
ZEND_METHOD(Uri_WhatWgUri, __unserialize);
ZEND_METHOD(Uri_WhatWg_WhatWgError, __construct);

static const zend_function_entry class_Uri_Uri_methods[] = {
	ZEND_RAW_FENTRY("getScheme", NULL, arginfo_class_Uri_Uri_getScheme, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("withScheme", NULL, arginfo_class_Uri_Uri_withScheme, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("getUser", NULL, arginfo_class_Uri_Uri_getUser, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("withUser", NULL, arginfo_class_Uri_Uri_withUser, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("getPassword", NULL, arginfo_class_Uri_Uri_getPassword, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("withPassword", NULL, arginfo_class_Uri_Uri_withPassword, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("getHost", NULL, arginfo_class_Uri_Uri_getHost, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("withHost", NULL, arginfo_class_Uri_Uri_withHost, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("getPort", NULL, arginfo_class_Uri_Uri_getPort, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("withPort", NULL, arginfo_class_Uri_Uri_withPort, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("getPath", NULL, arginfo_class_Uri_Uri_getPath, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("withPath", NULL, arginfo_class_Uri_Uri_withPath, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("getQuery", NULL, arginfo_class_Uri_Uri_getQuery, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("withQuery", NULL, arginfo_class_Uri_Uri_withQuery, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("getFragment", NULL, arginfo_class_Uri_Uri_getFragment, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("withFragment", NULL, arginfo_class_Uri_Uri_withFragment, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("equals", NULL, arginfo_class_Uri_Uri_equals, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("normalize", NULL, arginfo_class_Uri_Uri_normalize, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("toNormalizedString", NULL, arginfo_class_Uri_Uri_toNormalizedString, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("toString", NULL, arginfo_class_Uri_Uri_toString, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("resolve", NULL, arginfo_class_Uri_Uri_resolve, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
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
	ZEND_ME(Uri_Rfc3986Uri, equals, arginfo_class_Uri_Rfc3986Uri_equals, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Rfc3986Uri, normalize, arginfo_class_Uri_Rfc3986Uri_normalize, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Rfc3986Uri, toNormalizedString, arginfo_class_Uri_Rfc3986Uri_toNormalizedString, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Rfc3986Uri, toString, arginfo_class_Uri_Rfc3986Uri_toString, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Rfc3986Uri, resolve, arginfo_class_Uri_Rfc3986Uri_resolve, ZEND_ACC_PUBLIC)
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
	ZEND_RAW_FENTRY("equals", zim_Uri_Rfc3986Uri_equals, arginfo_class_Uri_WhatWgUri_equals, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("normalize", zim_Uri_Rfc3986Uri_normalize, arginfo_class_Uri_WhatWgUri_normalize, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("toNormalizedString", zim_Uri_Rfc3986Uri_toNormalizedString, arginfo_class_Uri_WhatWgUri_toNormalizedString, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("toString", zim_Uri_Rfc3986Uri_toString, arginfo_class_Uri_WhatWgUri_toString, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("resolve", zim_Uri_Rfc3986Uri_resolve, arginfo_class_Uri_WhatWgUri_resolve, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("__serialize", zim_Uri_Rfc3986Uri___serialize, arginfo_class_Uri_WhatWgUri___serialize, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_ME(Uri_WhatWgUri, __unserialize, arginfo_class_Uri_WhatWgUri___unserialize, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_Uri_WhatWg_WhatWgError_methods[] = {
	ZEND_ME(Uri_WhatWg_WhatWgError, __construct, arginfo_class_Uri_WhatWg_WhatWgError___construct, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Uri_UriException(zend_class_entry *class_entry_Exception)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Uri", "UriException", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_Exception, ZEND_ACC_NO_DYNAMIC_PROPERTIES);

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

static zend_class_entry *register_class_Uri_Uri(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Uri", "Uri", class_Uri_Uri_methods);
	class_entry = zend_register_internal_interface(&ce);

	return class_entry;
}

static zend_class_entry *register_class_Uri_Rfc3986Uri(zend_class_entry *class_entry_Uri_Uri)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Uri", "Rfc3986Uri", class_Uri_Rfc3986Uri_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_READONLY_CLASS);
	zend_class_implements(class_entry, 1, class_entry_Uri_Uri);

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

static zend_class_entry *register_class_Uri_WhatWgUri(zend_class_entry *class_entry_Uri_Uri)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Uri", "WhatWgUri", class_Uri_WhatWgUri_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_READONLY_CLASS);
	zend_class_implements(class_entry, 1, class_entry_Uri_Uri);

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

static zend_class_entry *register_class_Uri_WhatWg_WhatWgErrorType(void)
{
	zend_class_entry *class_entry = zend_register_internal_enum("Uri\\WhatWg\\WhatWgErrorType", IS_LONG, NULL);

	zval enum_case_DomainToAscii_value;
	ZVAL_LONG(&enum_case_DomainToAscii_value, 0);
	zend_enum_add_case_cstr(class_entry, "DomainToAscii", &enum_case_DomainToAscii_value);

	zval enum_case_DomainToUnicode_value;
	ZVAL_LONG(&enum_case_DomainToUnicode_value, 1);
	zend_enum_add_case_cstr(class_entry, "DomainToUnicode", &enum_case_DomainToUnicode_value);

	zval enum_case_DomainInvalidCodePoint_value;
	ZVAL_LONG(&enum_case_DomainInvalidCodePoint_value, 2);
	zend_enum_add_case_cstr(class_entry, "DomainInvalidCodePoint", &enum_case_DomainInvalidCodePoint_value);

	zval enum_case_HostInvalidCodePoint_value;
	ZVAL_LONG(&enum_case_HostInvalidCodePoint_value, 3);
	zend_enum_add_case_cstr(class_entry, "HostInvalidCodePoint", &enum_case_HostInvalidCodePoint_value);

	zval enum_case_Ipv4EmptyPart_value;
	ZVAL_LONG(&enum_case_Ipv4EmptyPart_value, 4);
	zend_enum_add_case_cstr(class_entry, "Ipv4EmptyPart", &enum_case_Ipv4EmptyPart_value);

	zval enum_case_Ipv4TooManyParts_value;
	ZVAL_LONG(&enum_case_Ipv4TooManyParts_value, 5);
	zend_enum_add_case_cstr(class_entry, "Ipv4TooManyParts", &enum_case_Ipv4TooManyParts_value);

	zval enum_case_Ipv4NonNumericPart_value;
	ZVAL_LONG(&enum_case_Ipv4NonNumericPart_value, 6);
	zend_enum_add_case_cstr(class_entry, "Ipv4NonNumericPart", &enum_case_Ipv4NonNumericPart_value);

	zval enum_case_Ipv4NonDecimalPart_value;
	ZVAL_LONG(&enum_case_Ipv4NonDecimalPart_value, 7);
	zend_enum_add_case_cstr(class_entry, "Ipv4NonDecimalPart", &enum_case_Ipv4NonDecimalPart_value);

	zval enum_case_Ipv4OutOfRangePart_value;
	ZVAL_LONG(&enum_case_Ipv4OutOfRangePart_value, 8);
	zend_enum_add_case_cstr(class_entry, "Ipv4OutOfRangePart", &enum_case_Ipv4OutOfRangePart_value);

	zval enum_case_Ipv6Unclosed_value;
	ZVAL_LONG(&enum_case_Ipv6Unclosed_value, 9);
	zend_enum_add_case_cstr(class_entry, "Ipv6Unclosed", &enum_case_Ipv6Unclosed_value);

	zval enum_case_Ipv6InvalidCompression_value;
	ZVAL_LONG(&enum_case_Ipv6InvalidCompression_value, 10);
	zend_enum_add_case_cstr(class_entry, "Ipv6InvalidCompression", &enum_case_Ipv6InvalidCompression_value);

	zval enum_case_Ipv6TooManyPieces_value;
	ZVAL_LONG(&enum_case_Ipv6TooManyPieces_value, 11);
	zend_enum_add_case_cstr(class_entry, "Ipv6TooManyPieces", &enum_case_Ipv6TooManyPieces_value);

	zval enum_case_Ipv6MultipleCompression_value;
	ZVAL_LONG(&enum_case_Ipv6MultipleCompression_value, 12);
	zend_enum_add_case_cstr(class_entry, "Ipv6MultipleCompression", &enum_case_Ipv6MultipleCompression_value);

	zval enum_case_Ipv6InvalidCodePoint_value;
	ZVAL_LONG(&enum_case_Ipv6InvalidCodePoint_value, 13);
	zend_enum_add_case_cstr(class_entry, "Ipv6InvalidCodePoint", &enum_case_Ipv6InvalidCodePoint_value);

	zval enum_case_Ipv6TooFewPieces_value;
	ZVAL_LONG(&enum_case_Ipv6TooFewPieces_value, 14);
	zend_enum_add_case_cstr(class_entry, "Ipv6TooFewPieces", &enum_case_Ipv6TooFewPieces_value);

	zval enum_case_Ipv4InIpv6TooManyPieces_value;
	ZVAL_LONG(&enum_case_Ipv4InIpv6TooManyPieces_value, 15);
	zend_enum_add_case_cstr(class_entry, "Ipv4InIpv6TooManyPieces", &enum_case_Ipv4InIpv6TooManyPieces_value);

	zval enum_case_Ipv4InIpv6InvalidCodePoint_value;
	ZVAL_LONG(&enum_case_Ipv4InIpv6InvalidCodePoint_value, 16);
	zend_enum_add_case_cstr(class_entry, "Ipv4InIpv6InvalidCodePoint", &enum_case_Ipv4InIpv6InvalidCodePoint_value);

	zval enum_case_Ipv4InIpv6OutOfRangePart_value;
	ZVAL_LONG(&enum_case_Ipv4InIpv6OutOfRangePart_value, 17);
	zend_enum_add_case_cstr(class_entry, "Ipv4InIpv6OutOfRangePart", &enum_case_Ipv4InIpv6OutOfRangePart_value);

	zval enum_case_Ipv4InIpv6TooFewParts_value;
	ZVAL_LONG(&enum_case_Ipv4InIpv6TooFewParts_value, 18);
	zend_enum_add_case_cstr(class_entry, "Ipv4InIpv6TooFewParts", &enum_case_Ipv4InIpv6TooFewParts_value);

	zval enum_case_InvalidUrlUnit_value;
	ZVAL_LONG(&enum_case_InvalidUrlUnit_value, 19);
	zend_enum_add_case_cstr(class_entry, "InvalidUrlUnit", &enum_case_InvalidUrlUnit_value);

	zval enum_case_SpecialSchemeMissingFollowingSolidus_value;
	ZVAL_LONG(&enum_case_SpecialSchemeMissingFollowingSolidus_value, 20);
	zend_enum_add_case_cstr(class_entry, "SpecialSchemeMissingFollowingSolidus", &enum_case_SpecialSchemeMissingFollowingSolidus_value);

	zval enum_case_MissingSchemeNonRelativeUrl_value;
	ZVAL_LONG(&enum_case_MissingSchemeNonRelativeUrl_value, 21);
	zend_enum_add_case_cstr(class_entry, "MissingSchemeNonRelativeUrl", &enum_case_MissingSchemeNonRelativeUrl_value);

	zval enum_case_InvalidReverseSoldius_value;
	ZVAL_LONG(&enum_case_InvalidReverseSoldius_value, 22);
	zend_enum_add_case_cstr(class_entry, "InvalidReverseSoldius", &enum_case_InvalidReverseSoldius_value);

	zval enum_case_InvalidCredentials_value;
	ZVAL_LONG(&enum_case_InvalidCredentials_value, 23);
	zend_enum_add_case_cstr(class_entry, "InvalidCredentials", &enum_case_InvalidCredentials_value);

	zval enum_case_HostMissing_value;
	ZVAL_LONG(&enum_case_HostMissing_value, 24);
	zend_enum_add_case_cstr(class_entry, "HostMissing", &enum_case_HostMissing_value);

	zval enum_case_PortOfOfRange_value;
	ZVAL_LONG(&enum_case_PortOfOfRange_value, 25);
	zend_enum_add_case_cstr(class_entry, "PortOfOfRange", &enum_case_PortOfOfRange_value);

	zval enum_case_PortInvalid_value;
	ZVAL_LONG(&enum_case_PortInvalid_value, 26);
	zend_enum_add_case_cstr(class_entry, "PortInvalid", &enum_case_PortInvalid_value);

	zval enum_case_FileInvalidWindowsDriveLetter_value;
	ZVAL_LONG(&enum_case_FileInvalidWindowsDriveLetter_value, 27);
	zend_enum_add_case_cstr(class_entry, "FileInvalidWindowsDriveLetter", &enum_case_FileInvalidWindowsDriveLetter_value);

	zval enum_case_FileInvalidWindowsDriveLetterHost_value;
	ZVAL_LONG(&enum_case_FileInvalidWindowsDriveLetterHost_value, 28);
	zend_enum_add_case_cstr(class_entry, "FileInvalidWindowsDriveLetterHost", &enum_case_FileInvalidWindowsDriveLetterHost_value);

	return class_entry;
}

static zend_class_entry *register_class_Uri_WhatWg_WhatWgError(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Uri\\WhatWg", "WhatWgError", class_Uri_WhatWg_WhatWgError_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_READONLY_CLASS);

	zval property_context_default_value;
	ZVAL_UNDEF(&property_context_default_value);
	zend_string *property_context_name = zend_string_init("context", sizeof("context") - 1, 1);
	zend_declare_typed_property(class_entry, property_context_name, &property_context_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_context_name);

	zval property_type_default_value;
	ZVAL_UNDEF(&property_type_default_value);
	zend_string *property_type_class_Uri_WhatWg_WhatWgErrorType = zend_string_init("Uri\\WhatWg\\WhatWgErrorType", sizeof("Uri\\WhatWg\\WhatWgErrorType")-1, 1);
	zend_declare_typed_property(class_entry, ZSTR_KNOWN(ZEND_STR_TYPE), &property_type_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_type_class_Uri_WhatWg_WhatWgErrorType, 0, 0));

	return class_entry;
}
