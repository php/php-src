/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Pierrick Charron <pierrick@php.net>                          |
   +----------------------------------------------------------------------+
*/

#define ZEND_INCLUDE_FULL_WINDOWS_HEADERS

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "Zend/zend_exceptions.h"
#include "zend_smart_str.h"

#include "curl_private.h"

#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */

/* CurlUrl class */
zend_class_entry *curl_CURLUrl_ce;
zend_class_entry *curl_CURLUrlException_ce;

static void php_curl_url_throw_exception(int code, CURLUPart part, const char *action)
{
	const char *part_name = "(unknown)";
	switch (part) {
		case CURLUPART_URL:
			part_name = "URL";
			break;
		case CURLUPART_SCHEME:
			part_name = "scheme";
			break;
		case CURLUPART_HOST:
			part_name = "host";
			break;
		case CURLUPART_PORT:
			part_name = "port";
			break;
		case CURLUPART_USER:
			part_name = "user";
			break;
		case CURLUPART_PASSWORD:
			part_name = "password";
			break;
		case CURLUPART_PATH:
			part_name = "path";
			break;
		case CURLUPART_QUERY:
			part_name = "query";
			break;
		case CURLUPART_FRAGMENT:
			part_name = "fragment";
			break;
		case CURLUPART_OPTIONS:
			part_name = "options";
			break;
#if LIBCURL_VERSION_NUM >= 0x074100 /* Available since 7.65.0 */
		case CURLUPART_ZONEID:
			part_name = "zoneid";
			break;
#endif
	}
#if LIBCURL_VERSION_NUM >= 0x075000 /* Available since 7.80.0 */
	zend_throw_exception_ex(curl_CURLUrlException_ce, code, "Unable to %s %s: %s", action, part_name, curl_url_strerror(code));
#else
	zend_throw_exception_ex(curl_CURLUrlException_ce, code, "Unable to %s %s (Code: %d)", action, part_name, code);
#endif
}

static zend_result _php_curl_url_set(php_curlurl *uh, zend_long option, const char *str, const size_t len, unsigned int flags)
{
	CURLUcode error = curl_url_set(uh->url, option, str, flags);
	SAVE_CURL_ERROR(uh, error);

	return error == CURLUE_OK ? SUCCESS : FAILURE;
}

static void php_curl_url_set_nullable_string(INTERNAL_FUNCTION_PARAMETERS, zend_long part, zend_bool has_flags)
{
	zval *zid;
	zend_long flags = 0;
	php_curlurl *uh;
	zend_string *content = NULL;
	zend_result res;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), has_flags ? "OS!|l": "OS!", &zid, curl_CURLUrl_ce, &content, &flags) == FAILURE) {
		RETURN_THROWS();
	}

	if (content && strlen(ZSTR_VAL(content)) != ZSTR_LEN(content)) {
		zend_argument_value_error(1, "must not contain any null bytes");
		RETURN_THROWS();
	}

	uh = Z_CURL_URL_P(zid);

	if (content) {
		res = _php_curl_url_set(uh, part, ZSTR_VAL(content), ZSTR_LEN(content), flags);
	} else {
		res = _php_curl_url_set(uh, part, NULL, 0, flags);
	}

	if (res == FAILURE) {
		php_curl_url_throw_exception(uh->err.no, part, "set");
	}
}

static void php_curl_url_set_nullable_int(INTERNAL_FUNCTION_PARAMETERS, zend_long part, zend_bool has_flags)
{
	zval *zid;
	zend_long flags = 0;
	php_curlurl *uh;
	zend_long content = 0;
	bool content_is_null = 1;
	zend_result res;
	smart_str str = {0};

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), has_flags ? "Ol!|l" : "Ol!", &zid, curl_CURLUrl_ce, &content, &content_is_null, &flags) == FAILURE) {
		RETURN_THROWS();
	}

	uh = Z_CURL_URL_P(zid);

	if (content_is_null) {
		res = _php_curl_url_set(uh, part, NULL, 0, flags);
	} else {
		smart_str_append_long(&str, content);
		smart_str_0(&str);

		res = _php_curl_url_set(uh, part, ZSTR_VAL(str.s) , ZSTR_LEN(str.s), flags);
	}

	if (res == FAILURE) {
		php_curl_url_throw_exception(uh->err.no, part, "set");
	}

	if (!content_is_null) {
		smart_str_free(&str);
	}
}

static void php_curl_url_get_nullable_string(INTERNAL_FUNCTION_PARAMETERS, zend_long part, zend_bool has_flags, long no_value)
{
	zval *zid;
	zend_long flags = 0;
	php_curlurl *uh;
	char *value;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), has_flags ? "O|l" : "O", &zid, curl_CURLUrl_ce, &flags) == FAILURE) {
		RETURN_THROWS();
	}

	uh = Z_CURL_URL_P(zid);

 	CURLUcode res = curl_url_get(uh->url, part, &value, flags);
	if (res == no_value && res != CURLUE_OK) {
		RETURN_NULL();
	} if (res == CURLUE_OK) {
		RETVAL_STRING(value);
		curl_free(value);
	} else {
		SAVE_CURL_ERROR(uh, res);
		php_curl_url_throw_exception(uh->err.no, part, "get");
	}
}

static void php_curl_url_get_nullable_int(INTERNAL_FUNCTION_PARAMETERS, zend_long part, zend_bool has_flags, long no_value)
{
	zval *zid;
	zend_long flags = 0;
	php_curlurl *uh;
	char *value;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), has_flags ? "O|l" : "O", &zid, curl_CURLUrl_ce, &flags) == FAILURE) {
		RETURN_THROWS();
	}

	uh = Z_CURL_URL_P(zid);

 	CURLUcode res = curl_url_get(uh->url, part, &value, flags);
	if (res == no_value && res != CURLUE_OK) {
		RETURN_NULL();
	} if (res == CURLUE_OK) {
		RETURN_LONG(ZEND_STRTOL(value, NULL, 10));
		curl_free(value);
	} else {
		SAVE_CURL_ERROR(uh, res);
		php_curl_url_throw_exception(uh->err.no, part, "get");
	}
}

PHP_METHOD(CurlUrl, __construct)
{
	php_curlurl *uh;
	zend_string *url = NULL;
	zend_long flags = 0;

	ZEND_PARSE_PARAMETERS_START(0,2)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR_OR_NULL(url)
		Z_PARAM_LONG(flags)
	ZEND_PARSE_PARAMETERS_END();

	if (NULL != url && strlen(ZSTR_VAL(url)) != ZSTR_LEN(url)) {
		zend_argument_value_error(1, "must not contain any null bytes");
		RETURN_THROWS();
	}

	return_value = ZEND_THIS;
	uh = Z_CURL_URL_P(return_value);
	uh->url = curl_url();

	if (url) {
		if (_php_curl_url_set(uh, CURLUPART_URL, ZSTR_VAL(url), ZSTR_LEN(url), flags) == FAILURE) {
#if LIBCURL_VERSION_NUM >= 0x075000 /* Available since 7.80.0 */
			zend_throw_exception(curl_CURLUrlException_ce, curl_url_strerror(uh->err.no), uh->err.no);
#else
			zend_throw_exception(curl_CURLUrlException_ce, "Unable to create CurlUrl", uh->err.no);
#endif
			RETURN_THROWS();
		}
	}
}

PHP_METHOD(CurlUrl, set)
{
	php_curl_url_set_nullable_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, CURLUPART_URL, true /* has flags */);
	RETURN_OBJ_COPY(Z_OBJ_P(ZEND_THIS));
}

PHP_METHOD(CurlUrl, get)
{
	php_curl_url_get_nullable_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, CURLUPART_URL, true /* has flags */, CURLUE_OK);
}

PHP_METHOD(CurlUrl, setScheme)
{
	php_curl_url_set_nullable_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, CURLUPART_SCHEME, true /* has flags */);
	RETURN_OBJ_COPY(Z_OBJ_P(ZEND_THIS));
}

PHP_METHOD(CurlUrl, getScheme)
{
	php_curl_url_get_nullable_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, CURLUPART_SCHEME, false /* has flags */, CURLUE_NO_SCHEME);
}

PHP_METHOD(CurlUrl, setHost)
{
	php_curl_url_set_nullable_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, CURLUPART_HOST, false /* has flags */);
	RETURN_OBJ_COPY(Z_OBJ_P(ZEND_THIS));
}

PHP_METHOD(CurlUrl, getHost)
{
	php_curl_url_get_nullable_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, CURLUPART_HOST, false /* has flags */, CURLUE_NO_HOST);
}

PHP_METHOD(CurlUrl, setPort)
{
	php_curl_url_set_nullable_int(INTERNAL_FUNCTION_PARAM_PASSTHRU, CURLUPART_PORT, false /* has flags */);
	RETURN_OBJ_COPY(Z_OBJ_P(ZEND_THIS));
}

PHP_METHOD(CurlUrl, getPort)
{
	php_curl_url_get_nullable_int(INTERNAL_FUNCTION_PARAM_PASSTHRU, CURLUPART_PORT, true /* has flags */, CURLUE_NO_PORT);
}

PHP_METHOD(CurlUrl, setPath)
{
	php_curl_url_set_nullable_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, CURLUPART_PATH, true /* has flags */);
	RETURN_OBJ_COPY(Z_OBJ_P(ZEND_THIS));
}

PHP_METHOD(CurlUrl, getPath)
{
	php_curl_url_get_nullable_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, CURLUPART_PATH, true /* has flags */, CURLUE_OK);
}

PHP_METHOD(CurlUrl, setQuery)
{
	php_curl_url_set_nullable_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, CURLUPART_QUERY, true /* has flags */);
	RETURN_OBJ_COPY(Z_OBJ_P(ZEND_THIS));
}

PHP_METHOD(CurlUrl, getQuery)
{
	php_curl_url_get_nullable_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, CURLUPART_QUERY, true /* has flags */, CURLUE_NO_QUERY);
}

PHP_METHOD(CurlUrl, setFragment)
{
	php_curl_url_set_nullable_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, CURLUPART_FRAGMENT, true /* has flags */);
	RETURN_OBJ_COPY(Z_OBJ_P(ZEND_THIS));
}

PHP_METHOD(CurlUrl, getFragment)
{
	php_curl_url_get_nullable_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, CURLUPART_FRAGMENT, true /* has flags */, CURLUE_NO_FRAGMENT);
}

PHP_METHOD(CurlUrl, setUser)
{
	php_curl_url_set_nullable_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, CURLUPART_USER, true /* has flags */);
	RETURN_OBJ_COPY(Z_OBJ_P(ZEND_THIS));
}

PHP_METHOD(CurlUrl, getUser)
{
	php_curl_url_get_nullable_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, CURLUPART_USER, true /* has flags */, CURLUE_NO_USER);
}

PHP_METHOD(CurlUrl, setPassword)
{
	php_curl_url_set_nullable_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, CURLUPART_PASSWORD, true /* has flags */);
	RETURN_OBJ_COPY(Z_OBJ_P(ZEND_THIS));
}

PHP_METHOD(CurlUrl, getPassword)
{
	php_curl_url_get_nullable_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, CURLUPART_PASSWORD, true /* has flags */, CURLUE_NO_PASSWORD);
}

PHP_METHOD(CurlUrl, setOptions)
{
	php_curl_url_set_nullable_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, CURLUPART_OPTIONS, true /* has flags */);
	RETURN_OBJ_COPY(Z_OBJ_P(ZEND_THIS));
}

PHP_METHOD(CurlUrl, getOptions)
{
	php_curl_url_get_nullable_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, CURLUPART_OPTIONS, true /* has flags */, CURLUE_NO_OPTIONS);
}

#if LIBCURL_VERSION_NUM >= 0x074100 /* Available since 7.65.0 */
PHP_METHOD(CurlUrl, setZoneId)
{
	php_curl_url_set_nullable_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, CURLUPART_ZONEID, true /* has flags */);
	RETURN_OBJ_COPY(Z_OBJ_P(ZEND_THIS));
}

PHP_METHOD(CurlUrl, getZoneId)
{
#if LIBCURL_VERSION_NUM >= 0x075100 /* Available since 7.81.0 */
	php_curl_url_get_nullable_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, CURLUPART_ZONEID, true /* has flags */, CURLUE_NO_ZONEID);
#else
	php_curl_url_get_nullable_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, CURLUPART_ZONEID, true /* has flags */, CURLUE_UNKNOWN_PART);
#endif
}
#endif

PHP_METHOD(CurlUrl, __toString)
{
	zval *object = ZEND_THIS;
	php_curlurl *uh;
	char *value;
	CURLUcode res;

	uh = Z_CURL_URL_P(object);

 	res = curl_url_get(uh->url, CURLUPART_URL, &value, 0L);
	if (res != CURLUE_OK) {
		SAVE_CURL_ERROR(uh, res);
#if LIBCURL_VERSION_NUM >= 0x075000 /* Available since 7.80.0 */
		zend_throw_exception_ex(curl_CURLUrlException_ce, uh->err.no, "Unable to convert CurlUrl to string: %s", curl_url_strerror(uh->err.no));
#else
		zend_throw_exception(curl_CURLUrlException_ce, "Unable to convert CurlUrl to string", uh->err.no);
#endif
	} else {
		RETVAL_STRING(value);
		curl_free(value);
	}
}

static zend_object_handlers curl_url_handlers;

static zend_object *curl_url_create_object(zend_class_entry *class_type) {
	php_curlurl *intern = zend_object_alloc(sizeof(php_curlurl), class_type);

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);
	intern->std.handlers = &curl_url_handlers;

	return &intern->std;
}

static zend_object *curl_url_clone_obj(zend_object *object) {
	php_curlurl *uh;
	CURLU *url;
	zend_object *clone_object;
	php_curlurl *clone_uh;

	clone_object = curl_url_create_object(curl_CURLUrl_ce);
	clone_uh = curl_url_from_obj(clone_object);

	uh = curl_url_from_obj(object);
	url = curl_url_dup(uh->url);
	if (!url) {
		zend_throw_exception(NULL, "Failed to clone CurlUrl", 0);
		return &clone_uh->std;
	}

	clone_uh->url = url;

	return &clone_uh->std;
}

void curl_url_free_obj(zend_object *object)
{
	php_curlurl *uh = curl_url_from_obj(object);
	curl_url_cleanup(uh->url);
	zend_object_std_dtor(&uh->std);
}

void curl_url_register_handlers(void) {
	curl_CURLUrl_ce->create_object = curl_url_create_object;

	memcpy(&curl_url_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	curl_url_handlers.offset = XtOffsetOf(php_curlurl, std);
	curl_url_handlers.free_obj = curl_url_free_obj;
	curl_url_handlers.clone_obj = curl_url_clone_obj;
	curl_url_handlers.compare = zend_objects_not_comparable;
}
#endif
