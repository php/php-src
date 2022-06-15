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

#include "curl_private.h"

#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */

/* CurlUrl class */
zend_class_entry *curl_CURLUrl_ce;
zend_class_entry *curl_CURLUrlException_ce;

static zend_result php_curl_url_set(php_curlurl *uh, zend_long option, const char *str, const size_t len, unsigned int flags)
{
	CURLUcode error = curl_url_set(uh->url, option, str, flags);
	SAVE_CURL_ERROR(uh, error);

	return error == CURLUE_OK ? SUCCESS : FAILURE;
}

static void php_curl_url_ctor(INTERNAL_FUNCTION_PARAMETERS)
{
	php_curlurl *uh;
	zend_string *url = NULL;

	ZEND_PARSE_PARAMETERS_START(0,1)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR_OR_NULL(url)
	ZEND_PARSE_PARAMETERS_END();

	if (NULL != url && strlen(ZSTR_VAL(url)) != ZSTR_LEN(url)) {
		zend_argument_error(curl_CURLUrlException_ce, 1, "must not contain any null bytes");
		RETURN_THROWS();
	}

	uh = Z_CURL_URL_P(return_value);
	uh->url = curl_url();

	if (url) {
		if (php_curl_url_set(uh, CURLUPART_URL, ZSTR_VAL(url), ZSTR_LEN(url), 0) == FAILURE) {
#if LIBCURL_VERSION_NUM >= 0x075000 /* Available since 7.80.0 */
			zend_throw_exception(curl_CURLUrlException_ce, curl_url_strerror(uh->err.no), uh->err.no);
#else
			zend_throw_exception(curl_CURLUrlException_ce, "Unable to create CurlUrl", uh->err.no);
#endif
			RETURN_THROWS();
		}
	}
}

PHP_METHOD(CurlUrl, __construct)
{
	return_value = ZEND_THIS;
	php_curl_url_ctor(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

PHP_FUNCTION(curl_url)
{
	object_init_ex(return_value, curl_CURLUrl_ce);
	php_curl_url_ctor(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

PHP_FUNCTION(curl_url_set)
{
	zval *zid;
	zend_long part, flags = 0;
	php_curlurl *uh;
	zend_string *content;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "OlS|l", &zid, curl_CURLUrl_ce, &part, &content, &flags) == FAILURE) {
		RETURN_THROWS();
	}

	if (content && strlen(ZSTR_VAL(content)) != ZSTR_LEN(content)) {
		zend_argument_error(curl_CURLUrlException_ce, getThis() ? 2 : 3, "must not contain any null bytes");
		RETURN_THROWS();
	}

	uh = Z_CURL_URL_P(zid);

	if (php_curl_url_set(uh, part, ZSTR_VAL(content), ZSTR_LEN(content), flags) == FAILURE) {
#if LIBCURL_VERSION_NUM >= 0x075000 /* Available since 7.80.0 */
		zend_throw_exception(curl_CURLUrlException_ce, curl_url_strerror(uh->err.no), uh->err.no);
#else
		zend_throw_exception(curl_CURLUrlException_ce, "Unable to set CurlUrl part", uh->err.no);
#endif
	}
}

PHP_FUNCTION(curl_url_get)
{
	zval *zid;
	zend_long part, flags = 0;
	php_curlurl *uh;
	char *value;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Ol|l", &zid, curl_CURLUrl_ce, &part, &flags) == FAILURE) {
		RETURN_THROWS();
	}

	uh = Z_CURL_URL_P(zid);

 	CURLUcode res = curl_url_get(uh->url, part, &value, flags);
	if (res != CURLUE_OK) {
		SAVE_CURL_ERROR(uh, res);
#if LIBCURL_VERSION_NUM >= 0x075000 /* Available since 7.80.0 */
		zend_throw_exception(curl_CURLUrlException_ce, curl_url_strerror(uh->err.no), uh->err.no);
#else
		zend_throw_exception(curl_CURLUrlException_ce, "Unable to get CurlUrl part", uh->err.no);
#endif
	} else {
		RETVAL_STRING(value);
		curl_free(value);
	}
}

PHP_FUNCTION(curl_url_errno)
{
	zval *zid;
	php_curlurl  *uh;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &zid, curl_CURLUrl_ce) == FAILURE) {
		RETURN_THROWS();
	}

	uh = Z_CURL_URL_P(zid);

	RETURN_LONG(uh->err.no);
}

PHP_METHOD(CurlUrl, __toString)
{
	zval *object = ZEND_THIS;
	php_curlurl *uh;
	char *value;

	uh = Z_CURL_URL_P(object);

	curl_url_get(uh->url, CURLUPART_URL, &value, 0L);

	RETVAL_STRING(value);

	curl_free(value);
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

#if LIBCURL_VERSION_NUM >= 0x075000 /* Available since 7.80.0 */
PHP_FUNCTION(curl_url_strerror)
{
	zend_long code;
	const char *str;

	ZEND_PARSE_PARAMETERS_START(1,1)
		Z_PARAM_LONG(code)
	ZEND_PARSE_PARAMETERS_END();

	str = curl_url_strerror(code);
	if (str) {
		RETURN_STRING(str);
	} else {
		RETURN_NULL();
	}
}
#endif
