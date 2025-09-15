/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: f3c524798d1933a400cc9377cfbfdcbaf77b87f0 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_Rfc3986_Uri_parse, 0, 1, IS_STATIC, 1)
	ZEND_ARG_TYPE_INFO(0, uri, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, baseUrl, Uri\\Rfc3986\\\125ri, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Uri_Rfc3986_Uri___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, uri, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, baseUrl, Uri\\Rfc3986\\\125ri, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_Rfc3986_Uri_getScheme, 0, 0, IS_STRING, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_Uri_Rfc3986_Uri_getRawScheme arginfo_class_Uri_Rfc3986_Uri_getScheme

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_Rfc3986_Uri_withScheme, 0, 1, IS_STATIC, 0)
	ZEND_ARG_TYPE_INFO(0, scheme, IS_STRING, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_Uri_Rfc3986_Uri_getUserInfo arginfo_class_Uri_Rfc3986_Uri_getScheme

#define arginfo_class_Uri_Rfc3986_Uri_getRawUserInfo arginfo_class_Uri_Rfc3986_Uri_getScheme

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_Rfc3986_Uri_withUserInfo, 0, 1, IS_STATIC, 0)
	ZEND_ARG_TYPE_INFO(0, userinfo, IS_STRING, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_Uri_Rfc3986_Uri_getUsername arginfo_class_Uri_Rfc3986_Uri_getScheme

#define arginfo_class_Uri_Rfc3986_Uri_getRawUsername arginfo_class_Uri_Rfc3986_Uri_getScheme

#define arginfo_class_Uri_Rfc3986_Uri_getPassword arginfo_class_Uri_Rfc3986_Uri_getScheme

#define arginfo_class_Uri_Rfc3986_Uri_getRawPassword arginfo_class_Uri_Rfc3986_Uri_getScheme

#define arginfo_class_Uri_Rfc3986_Uri_getHost arginfo_class_Uri_Rfc3986_Uri_getScheme

#define arginfo_class_Uri_Rfc3986_Uri_getRawHost arginfo_class_Uri_Rfc3986_Uri_getScheme

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_Rfc3986_Uri_withHost, 0, 1, IS_STATIC, 0)
	ZEND_ARG_TYPE_INFO(0, host, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_Rfc3986_Uri_getPort, 0, 0, IS_LONG, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_Rfc3986_Uri_withPort, 0, 1, IS_STATIC, 0)
	ZEND_ARG_TYPE_INFO(0, port, IS_LONG, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_Rfc3986_Uri_getPath, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Uri_Rfc3986_Uri_getRawPath arginfo_class_Uri_Rfc3986_Uri_getPath

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_Rfc3986_Uri_withPath, 0, 1, IS_STATIC, 0)
	ZEND_ARG_TYPE_INFO(0, path, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Uri_Rfc3986_Uri_getQuery arginfo_class_Uri_Rfc3986_Uri_getScheme

#define arginfo_class_Uri_Rfc3986_Uri_getRawQuery arginfo_class_Uri_Rfc3986_Uri_getScheme

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_Rfc3986_Uri_withQuery, 0, 1, IS_STATIC, 0)
	ZEND_ARG_TYPE_INFO(0, query, IS_STRING, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_Uri_Rfc3986_Uri_getFragment arginfo_class_Uri_Rfc3986_Uri_getScheme

#define arginfo_class_Uri_Rfc3986_Uri_getRawFragment arginfo_class_Uri_Rfc3986_Uri_getScheme

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_Rfc3986_Uri_withFragment, 0, 1, IS_STATIC, 0)
	ZEND_ARG_TYPE_INFO(0, fragment, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_Rfc3986_Uri_equals, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, uri, Uri\\Rfc3986\\\125ri, 0)
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, comparisonMode, Uri\\\125riComparisonMode, 0, "Uri\\UriComparisonMode::ExcludeFragment")
ZEND_END_ARG_INFO()

#define arginfo_class_Uri_Rfc3986_Uri_toString arginfo_class_Uri_Rfc3986_Uri_getPath

#define arginfo_class_Uri_Rfc3986_Uri_toRawString arginfo_class_Uri_Rfc3986_Uri_getPath

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_Rfc3986_Uri_resolve, 0, 1, IS_STATIC, 0)
	ZEND_ARG_TYPE_INFO(0, uri, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_Rfc3986_Uri___serialize, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_Rfc3986_Uri___unserialize, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Uri_Rfc3986_Uri___debugInfo arginfo_class_Uri_Rfc3986_Uri___serialize

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Uri_WhatWg_InvalidUrlException___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, message, IS_STRING, 0, "\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, errors, IS_ARRAY, 0, "[]")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, code, IS_LONG, 0, "0")
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, previous, Throwable, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Uri_WhatWg_UrlValidationError___construct, 0, 0, 3)
	ZEND_ARG_TYPE_INFO(0, context, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO(0, type, Uri\\WhatWg\\\125rlValidationErrorType, 0)
	ZEND_ARG_TYPE_INFO(0, failure, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_WhatWg_Url_parse, 0, 1, IS_STATIC, 1)
	ZEND_ARG_TYPE_INFO(0, uri, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, baseUrl, Uri\\WhatWg\\\125rl, 1, "null")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, errors, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Uri_WhatWg_Url___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, uri, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, baseUrl, Uri\\WhatWg\\\125rl, 1, "null")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, softErrors, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_Uri_WhatWg_Url_getScheme arginfo_class_Uri_Rfc3986_Uri_getPath

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_WhatWg_Url_withScheme, 0, 1, IS_STATIC, 0)
	ZEND_ARG_TYPE_INFO(0, scheme, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Uri_WhatWg_Url_getUsername arginfo_class_Uri_Rfc3986_Uri_getScheme

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_WhatWg_Url_withUsername, 0, 1, IS_STATIC, 0)
	ZEND_ARG_TYPE_INFO(0, username, IS_STRING, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_Uri_WhatWg_Url_getPassword arginfo_class_Uri_Rfc3986_Uri_getScheme

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_WhatWg_Url_withPassword, 0, 1, IS_STATIC, 0)
	ZEND_ARG_TYPE_INFO(0, password, IS_STRING, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_Uri_WhatWg_Url_getAsciiHost arginfo_class_Uri_Rfc3986_Uri_getScheme

#define arginfo_class_Uri_WhatWg_Url_getUnicodeHost arginfo_class_Uri_Rfc3986_Uri_getScheme

#define arginfo_class_Uri_WhatWg_Url_withHost arginfo_class_Uri_Rfc3986_Uri_withHost

#define arginfo_class_Uri_WhatWg_Url_getPort arginfo_class_Uri_Rfc3986_Uri_getPort

#define arginfo_class_Uri_WhatWg_Url_withPort arginfo_class_Uri_Rfc3986_Uri_withPort

#define arginfo_class_Uri_WhatWg_Url_getPath arginfo_class_Uri_Rfc3986_Uri_getPath

#define arginfo_class_Uri_WhatWg_Url_withPath arginfo_class_Uri_Rfc3986_Uri_withPath

#define arginfo_class_Uri_WhatWg_Url_getQuery arginfo_class_Uri_Rfc3986_Uri_getScheme

#define arginfo_class_Uri_WhatWg_Url_withQuery arginfo_class_Uri_Rfc3986_Uri_withQuery

#define arginfo_class_Uri_WhatWg_Url_getFragment arginfo_class_Uri_Rfc3986_Uri_getScheme

#define arginfo_class_Uri_WhatWg_Url_withFragment arginfo_class_Uri_Rfc3986_Uri_withFragment

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_WhatWg_Url_equals, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, url, Uri\\WhatWg\\\125rl, 0)
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, comparisonMode, Uri\\\125riComparisonMode, 0, "Uri\\UriComparisonMode::ExcludeFragment")
ZEND_END_ARG_INFO()

#define arginfo_class_Uri_WhatWg_Url_toAsciiString arginfo_class_Uri_Rfc3986_Uri_getPath

#define arginfo_class_Uri_WhatWg_Url_toUnicodeString arginfo_class_Uri_Rfc3986_Uri_getPath

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_WhatWg_Url_resolve, 0, 1, IS_STATIC, 0)
	ZEND_ARG_TYPE_INFO(0, uri, IS_STRING, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, softErrors, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_Uri_WhatWg_Url___serialize arginfo_class_Uri_Rfc3986_Uri___serialize

#define arginfo_class_Uri_WhatWg_Url___unserialize arginfo_class_Uri_Rfc3986_Uri___unserialize

#define arginfo_class_Uri_WhatWg_Url___debugInfo arginfo_class_Uri_Rfc3986_Uri___serialize

ZEND_METHOD(Uri_Rfc3986_Uri, parse);
ZEND_METHOD(Uri_Rfc3986_Uri, __construct);
ZEND_METHOD(Uri_Rfc3986_Uri, getScheme);
ZEND_METHOD(Uri_Rfc3986_Uri, getRawScheme);
ZEND_METHOD(Uri_Rfc3986_Uri, withScheme);
ZEND_METHOD(Uri_Rfc3986_Uri, getUserInfo);
ZEND_METHOD(Uri_Rfc3986_Uri, getRawUserInfo);
ZEND_METHOD(Uri_Rfc3986_Uri, withUserInfo);
ZEND_METHOD(Uri_Rfc3986_Uri, getUsername);
ZEND_METHOD(Uri_Rfc3986_Uri, getRawUsername);
ZEND_METHOD(Uri_Rfc3986_Uri, getPassword);
ZEND_METHOD(Uri_Rfc3986_Uri, getRawPassword);
ZEND_METHOD(Uri_Rfc3986_Uri, getHost);
ZEND_METHOD(Uri_Rfc3986_Uri, getRawHost);
ZEND_METHOD(Uri_Rfc3986_Uri, withHost);
ZEND_METHOD(Uri_Rfc3986_Uri, getPort);
ZEND_METHOD(Uri_Rfc3986_Uri, withPort);
ZEND_METHOD(Uri_Rfc3986_Uri, getPath);
ZEND_METHOD(Uri_Rfc3986_Uri, getRawPath);
ZEND_METHOD(Uri_Rfc3986_Uri, withPath);
ZEND_METHOD(Uri_Rfc3986_Uri, getQuery);
ZEND_METHOD(Uri_Rfc3986_Uri, getRawQuery);
ZEND_METHOD(Uri_Rfc3986_Uri, withQuery);
ZEND_METHOD(Uri_Rfc3986_Uri, getFragment);
ZEND_METHOD(Uri_Rfc3986_Uri, getRawFragment);
ZEND_METHOD(Uri_Rfc3986_Uri, withFragment);
ZEND_METHOD(Uri_Rfc3986_Uri, equals);
ZEND_METHOD(Uri_Rfc3986_Uri, toString);
ZEND_METHOD(Uri_Rfc3986_Uri, toRawString);
ZEND_METHOD(Uri_Rfc3986_Uri, resolve);
ZEND_METHOD(Uri_Rfc3986_Uri, __serialize);
ZEND_METHOD(Uri_Rfc3986_Uri, __unserialize);
ZEND_METHOD(Uri_Rfc3986_Uri, __debugInfo);
ZEND_METHOD(Uri_WhatWg_InvalidUrlException, __construct);
ZEND_METHOD(Uri_WhatWg_UrlValidationError, __construct);
ZEND_METHOD(Uri_WhatWg_Url, parse);
ZEND_METHOD(Uri_WhatWg_Url, __construct);
ZEND_METHOD(Uri_WhatWg_Url, getScheme);
ZEND_METHOD(Uri_WhatWg_Url, withScheme);
ZEND_METHOD(Uri_WhatWg_Url, withUsername);
ZEND_METHOD(Uri_WhatWg_Url, withPassword);
ZEND_METHOD(Uri_WhatWg_Url, getAsciiHost);
ZEND_METHOD(Uri_WhatWg_Url, getUnicodeHost);
ZEND_METHOD(Uri_WhatWg_Url, equals);
ZEND_METHOD(Uri_WhatWg_Url, toAsciiString);
ZEND_METHOD(Uri_WhatWg_Url, toUnicodeString);
ZEND_METHOD(Uri_WhatWg_Url, resolve);
ZEND_METHOD(Uri_WhatWg_Url, __serialize);
ZEND_METHOD(Uri_WhatWg_Url, __unserialize);
ZEND_METHOD(Uri_WhatWg_Url, __debugInfo);

static const zend_function_entry class_Uri_Rfc3986_Uri_methods[] = {
	ZEND_ME(Uri_Rfc3986_Uri, parse, arginfo_class_Uri_Rfc3986_Uri_parse, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Uri_Rfc3986_Uri, __construct, arginfo_class_Uri_Rfc3986_Uri___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Rfc3986_Uri, getScheme, arginfo_class_Uri_Rfc3986_Uri_getScheme, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Rfc3986_Uri, getRawScheme, arginfo_class_Uri_Rfc3986_Uri_getRawScheme, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Rfc3986_Uri, withScheme, arginfo_class_Uri_Rfc3986_Uri_withScheme, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Rfc3986_Uri, getUserInfo, arginfo_class_Uri_Rfc3986_Uri_getUserInfo, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Rfc3986_Uri, getRawUserInfo, arginfo_class_Uri_Rfc3986_Uri_getRawUserInfo, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Rfc3986_Uri, withUserInfo, arginfo_class_Uri_Rfc3986_Uri_withUserInfo, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Rfc3986_Uri, getUsername, arginfo_class_Uri_Rfc3986_Uri_getUsername, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Rfc3986_Uri, getRawUsername, arginfo_class_Uri_Rfc3986_Uri_getRawUsername, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Rfc3986_Uri, getPassword, arginfo_class_Uri_Rfc3986_Uri_getPassword, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Rfc3986_Uri, getRawPassword, arginfo_class_Uri_Rfc3986_Uri_getRawPassword, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Rfc3986_Uri, getHost, arginfo_class_Uri_Rfc3986_Uri_getHost, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Rfc3986_Uri, getRawHost, arginfo_class_Uri_Rfc3986_Uri_getRawHost, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Rfc3986_Uri, withHost, arginfo_class_Uri_Rfc3986_Uri_withHost, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Rfc3986_Uri, getPort, arginfo_class_Uri_Rfc3986_Uri_getPort, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Rfc3986_Uri, withPort, arginfo_class_Uri_Rfc3986_Uri_withPort, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Rfc3986_Uri, getPath, arginfo_class_Uri_Rfc3986_Uri_getPath, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Rfc3986_Uri, getRawPath, arginfo_class_Uri_Rfc3986_Uri_getRawPath, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Rfc3986_Uri, withPath, arginfo_class_Uri_Rfc3986_Uri_withPath, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Rfc3986_Uri, getQuery, arginfo_class_Uri_Rfc3986_Uri_getQuery, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Rfc3986_Uri, getRawQuery, arginfo_class_Uri_Rfc3986_Uri_getRawQuery, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Rfc3986_Uri, withQuery, arginfo_class_Uri_Rfc3986_Uri_withQuery, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Rfc3986_Uri, getFragment, arginfo_class_Uri_Rfc3986_Uri_getFragment, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Rfc3986_Uri, getRawFragment, arginfo_class_Uri_Rfc3986_Uri_getRawFragment, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Rfc3986_Uri, withFragment, arginfo_class_Uri_Rfc3986_Uri_withFragment, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Rfc3986_Uri, equals, arginfo_class_Uri_Rfc3986_Uri_equals, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Rfc3986_Uri, toString, arginfo_class_Uri_Rfc3986_Uri_toString, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Rfc3986_Uri, toRawString, arginfo_class_Uri_Rfc3986_Uri_toRawString, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Rfc3986_Uri, resolve, arginfo_class_Uri_Rfc3986_Uri_resolve, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Rfc3986_Uri, __serialize, arginfo_class_Uri_Rfc3986_Uri___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Rfc3986_Uri, __unserialize, arginfo_class_Uri_Rfc3986_Uri___unserialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Rfc3986_Uri, __debugInfo, arginfo_class_Uri_Rfc3986_Uri___debugInfo, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_Uri_WhatWg_InvalidUrlException_methods[] = {
	ZEND_ME(Uri_WhatWg_InvalidUrlException, __construct, arginfo_class_Uri_WhatWg_InvalidUrlException___construct, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_Uri_WhatWg_UrlValidationError_methods[] = {
	ZEND_ME(Uri_WhatWg_UrlValidationError, __construct, arginfo_class_Uri_WhatWg_UrlValidationError___construct, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_Uri_WhatWg_Url_methods[] = {
	ZEND_ME(Uri_WhatWg_Url, parse, arginfo_class_Uri_WhatWg_Url_parse, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Uri_WhatWg_Url, __construct, arginfo_class_Uri_WhatWg_Url___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_WhatWg_Url, getScheme, arginfo_class_Uri_WhatWg_Url_getScheme, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_WhatWg_Url, withScheme, arginfo_class_Uri_WhatWg_Url_withScheme, ZEND_ACC_PUBLIC)
	ZEND_RAW_FENTRY("getUsername", zim_Uri_Rfc3986_Uri_getUsername, arginfo_class_Uri_WhatWg_Url_getUsername, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_ME(Uri_WhatWg_Url, withUsername, arginfo_class_Uri_WhatWg_Url_withUsername, ZEND_ACC_PUBLIC)
	ZEND_RAW_FENTRY("getPassword", zim_Uri_Rfc3986_Uri_getPassword, arginfo_class_Uri_WhatWg_Url_getPassword, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_ME(Uri_WhatWg_Url, withPassword, arginfo_class_Uri_WhatWg_Url_withPassword, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_WhatWg_Url, getAsciiHost, arginfo_class_Uri_WhatWg_Url_getAsciiHost, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_WhatWg_Url, getUnicodeHost, arginfo_class_Uri_WhatWg_Url_getUnicodeHost, ZEND_ACC_PUBLIC)
	ZEND_RAW_FENTRY("withHost", zim_Uri_Rfc3986_Uri_withHost, arginfo_class_Uri_WhatWg_Url_withHost, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("getPort", zim_Uri_Rfc3986_Uri_getPort, arginfo_class_Uri_WhatWg_Url_getPort, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("withPort", zim_Uri_Rfc3986_Uri_withPort, arginfo_class_Uri_WhatWg_Url_withPort, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("getPath", zim_Uri_Rfc3986_Uri_getPath, arginfo_class_Uri_WhatWg_Url_getPath, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("withPath", zim_Uri_Rfc3986_Uri_withPath, arginfo_class_Uri_WhatWg_Url_withPath, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("getQuery", zim_Uri_Rfc3986_Uri_getQuery, arginfo_class_Uri_WhatWg_Url_getQuery, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("withQuery", zim_Uri_Rfc3986_Uri_withQuery, arginfo_class_Uri_WhatWg_Url_withQuery, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("getFragment", zim_Uri_Rfc3986_Uri_getFragment, arginfo_class_Uri_WhatWg_Url_getFragment, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("withFragment", zim_Uri_Rfc3986_Uri_withFragment, arginfo_class_Uri_WhatWg_Url_withFragment, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_ME(Uri_WhatWg_Url, equals, arginfo_class_Uri_WhatWg_Url_equals, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_WhatWg_Url, toAsciiString, arginfo_class_Uri_WhatWg_Url_toAsciiString, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_WhatWg_Url, toUnicodeString, arginfo_class_Uri_WhatWg_Url_toUnicodeString, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_WhatWg_Url, resolve, arginfo_class_Uri_WhatWg_Url_resolve, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_WhatWg_Url, __serialize, arginfo_class_Uri_WhatWg_Url___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_WhatWg_Url, __unserialize, arginfo_class_Uri_WhatWg_Url___unserialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_WhatWg_Url, __debugInfo, arginfo_class_Uri_WhatWg_Url___debugInfo, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Uri_UriException(zend_class_entry *class_entry_Exception)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Uri", "UriException", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_Exception, ZEND_ACC_NO_DYNAMIC_PROPERTIES);

	return class_entry;
}

static zend_class_entry *register_class_Uri_UriError(zend_class_entry *class_entry_Error)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Uri", "UriError", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_Error, ZEND_ACC_NO_DYNAMIC_PROPERTIES);

	return class_entry;
}

static zend_class_entry *register_class_Uri_InvalidUriException(zend_class_entry *class_entry_Uri_UriException)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Uri", "InvalidUriException", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_Uri_UriException, ZEND_ACC_NO_DYNAMIC_PROPERTIES);

	return class_entry;
}

static zend_class_entry *register_class_Uri_UriComparisonMode(void)
{
	zend_class_entry *class_entry = zend_register_internal_enum("Uri\\UriComparisonMode", IS_UNDEF, NULL);

	zend_enum_add_case_cstr(class_entry, "IncludeFragment", NULL);

	zend_enum_add_case_cstr(class_entry, "ExcludeFragment", NULL);

	return class_entry;
}

static zend_class_entry *register_class_Uri_Rfc3986_Uri(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Uri\\Rfc3986", "Uri", class_Uri_Rfc3986_Uri_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_READONLY_CLASS);


	zend_add_parameter_attribute(zend_hash_str_find_ptr(&class_entry->function_table, "withuserinfo", sizeof("withuserinfo") - 1), 0, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	return class_entry;
}

static zend_class_entry *register_class_Uri_WhatWg_InvalidUrlException(zend_class_entry *class_entry_Uri_InvalidUriException)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Uri\\WhatWg", "InvalidUrlException", class_Uri_WhatWg_InvalidUrlException_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_Uri_InvalidUriException, ZEND_ACC_NO_DYNAMIC_PROPERTIES);

	zval property_errors_default_value;
	ZVAL_UNDEF(&property_errors_default_value);
	zend_string *property_errors_name = zend_string_init("errors", sizeof("errors") - 1, 1);
	zend_declare_typed_property(class_entry, property_errors_name, &property_errors_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_ARRAY));
	zend_string_release(property_errors_name);

	return class_entry;
}

static zend_class_entry *register_class_Uri_WhatWg_UrlValidationErrorType(void)
{
	zend_class_entry *class_entry = zend_register_internal_enum("Uri\\WhatWg\\UrlValidationErrorType", IS_UNDEF, NULL);

	zend_enum_add_case_cstr(class_entry, "DomainToAscii", NULL);

	zend_enum_add_case_cstr(class_entry, "DomainToUnicode", NULL);

	zend_enum_add_case_cstr(class_entry, "DomainInvalidCodePoint", NULL);

	zend_enum_add_case_cstr(class_entry, "HostInvalidCodePoint", NULL);

	zend_enum_add_case_cstr(class_entry, "Ipv4EmptyPart", NULL);

	zend_enum_add_case_cstr(class_entry, "Ipv4TooManyParts", NULL);

	zend_enum_add_case_cstr(class_entry, "Ipv4NonNumericPart", NULL);

	zend_enum_add_case_cstr(class_entry, "Ipv4NonDecimalPart", NULL);

	zend_enum_add_case_cstr(class_entry, "Ipv4OutOfRangePart", NULL);

	zend_enum_add_case_cstr(class_entry, "Ipv6Unclosed", NULL);

	zend_enum_add_case_cstr(class_entry, "Ipv6InvalidCompression", NULL);

	zend_enum_add_case_cstr(class_entry, "Ipv6TooManyPieces", NULL);

	zend_enum_add_case_cstr(class_entry, "Ipv6MultipleCompression", NULL);

	zend_enum_add_case_cstr(class_entry, "Ipv6InvalidCodePoint", NULL);

	zend_enum_add_case_cstr(class_entry, "Ipv6TooFewPieces", NULL);

	zend_enum_add_case_cstr(class_entry, "Ipv4InIpv6TooManyPieces", NULL);

	zend_enum_add_case_cstr(class_entry, "Ipv4InIpv6InvalidCodePoint", NULL);

	zend_enum_add_case_cstr(class_entry, "Ipv4InIpv6OutOfRangePart", NULL);

	zend_enum_add_case_cstr(class_entry, "Ipv4InIpv6TooFewParts", NULL);

	zend_enum_add_case_cstr(class_entry, "InvalidUrlUnit", NULL);

	zend_enum_add_case_cstr(class_entry, "SpecialSchemeMissingFollowingSolidus", NULL);

	zend_enum_add_case_cstr(class_entry, "MissingSchemeNonRelativeUrl", NULL);

	zend_enum_add_case_cstr(class_entry, "InvalidReverseSoldius", NULL);

	zend_enum_add_case_cstr(class_entry, "InvalidCredentials", NULL);

	zend_enum_add_case_cstr(class_entry, "HostMissing", NULL);

	zend_enum_add_case_cstr(class_entry, "PortOutOfRange", NULL);

	zend_enum_add_case_cstr(class_entry, "PortInvalid", NULL);

	zend_enum_add_case_cstr(class_entry, "FileInvalidWindowsDriveLetter", NULL);

	zend_enum_add_case_cstr(class_entry, "FileInvalidWindowsDriveLetterHost", NULL);

	return class_entry;
}

static zend_class_entry *register_class_Uri_WhatWg_UrlValidationError(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Uri\\WhatWg", "UrlValidationError", class_Uri_WhatWg_UrlValidationError_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_READONLY_CLASS);

	zval property_context_default_value;
	ZVAL_UNDEF(&property_context_default_value);
	zend_string *property_context_name = zend_string_init("context", sizeof("context") - 1, 1);
	zend_declare_typed_property(class_entry, property_context_name, &property_context_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_context_name);

	zval property_type_default_value;
	ZVAL_UNDEF(&property_type_default_value);
	zend_string *property_type_class_Uri_WhatWg_UrlValidationErrorType = zend_string_init("Uri\\WhatWg\\\125rlValidationErrorType", sizeof("Uri\\WhatWg\\\125rlValidationErrorType")-1, 1);
	zend_declare_typed_property(class_entry, ZSTR_KNOWN(ZEND_STR_TYPE), &property_type_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_type_class_Uri_WhatWg_UrlValidationErrorType, 0, 0));

	zval property_failure_default_value;
	ZVAL_UNDEF(&property_failure_default_value);
	zend_string *property_failure_name = zend_string_init("failure", sizeof("failure") - 1, 1);
	zend_declare_typed_property(class_entry, property_failure_name, &property_failure_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_BOOL));
	zend_string_release(property_failure_name);

	return class_entry;
}

static zend_class_entry *register_class_Uri_WhatWg_Url(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Uri\\WhatWg", "Url", class_Uri_WhatWg_Url_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_READONLY_CLASS);


	zend_add_parameter_attribute(zend_hash_str_find_ptr(&class_entry->function_table, "withpassword", sizeof("withpassword") - 1), 0, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	return class_entry;
}
