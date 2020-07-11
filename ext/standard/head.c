/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                        |
   +----------------------------------------------------------------------+
 */

#include <stdio.h>
#include "php.h"
#include "ext/standard/php_standard.h"
#include "ext/date/php_date.h"
#include "SAPI.h"
#include "php_main.h"
#include "head.h"
#include <time.h>

#include "php_globals.h"
#include "zend_smart_str.h"


/* Implementation of the language Header() function */
/* {{{ Sends a raw HTTP header */
PHP_FUNCTION(header)
{
	zend_bool rep = 1;
	sapi_header_line ctr = {0};
	char *line;
	size_t len;

	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_STRING(line, len)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(rep)
		Z_PARAM_LONG(ctr.response_code)
	ZEND_PARSE_PARAMETERS_END();

	ctr.line = line;
	ctr.line_len = (uint32_t)len;
	sapi_header_op(rep ? SAPI_HEADER_REPLACE:SAPI_HEADER_ADD, &ctr);
}
/* }}} */

/* {{{ Removes an HTTP header previously set using header() */
PHP_FUNCTION(header_remove)
{
	sapi_header_line ctr = {0};
	char *line = NULL;
	size_t len = 0;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING(line, len)
	ZEND_PARSE_PARAMETERS_END();

	ctr.line = line;
	ctr.line_len = (uint32_t)len;
	sapi_header_op(ZEND_NUM_ARGS() == 0 ? SAPI_HEADER_DELETE_ALL : SAPI_HEADER_DELETE, &ctr);
}
/* }}} */

PHPAPI int php_header(void)
{
	if (sapi_send_headers()==FAILURE || SG(request_info).headers_only) {
		return 0; /* don't allow output */
	} else {
		return 1; /* allow output */
	}
}

PHPAPI int php_setcookie(zend_string *name, zend_string *value, time_t expires, zend_string *path, zend_string *domain, int secure, int httponly, zend_string *samesite, int url_encode)
{
	zend_string *dt;
	sapi_header_line ctr = {0};
	int result;
	smart_str buf = {0};

	if (!ZSTR_LEN(name)) {
		zend_error( E_WARNING, "Cookie names must not be empty" );
		return FAILURE;
	} else if (strpbrk(ZSTR_VAL(name), "=,; \t\r\n\013\014") != NULL) {   /* man isspace for \013 and \014 */
		zend_error(E_WARNING, "Cookie names cannot contain any of the following '=,; \\t\\r\\n\\013\\014'" );
		return FAILURE;
	}

	if (!url_encode && value &&
			strpbrk(ZSTR_VAL(value), ",; \t\r\n\013\014") != NULL) { /* man isspace for \013 and \014 */
		zend_error(E_WARNING, "Cookie values cannot contain any of the following ',; \\t\\r\\n\\013\\014'" );
		return FAILURE;
	}

	if (path && strpbrk(ZSTR_VAL(path), ",; \t\r\n\013\014") != NULL) { /* man isspace for \013 and \014 */
		zend_error(E_WARNING, "Cookie paths cannot contain any of the following ',; \\t\\r\\n\\013\\014'" );
		return FAILURE;
	}

	if (domain && strpbrk(ZSTR_VAL(domain), ",; \t\r\n\013\014") != NULL) { /* man isspace for \013 and \014 */
		zend_error(E_WARNING, "Cookie domains cannot contain any of the following ',; \\t\\r\\n\\013\\014'" );
		return FAILURE;
	}

	if (value == NULL || ZSTR_LEN(value) == 0) {
		/*
		 * MSIE doesn't delete a cookie when you set it to a null value
		 * so in order to force cookies to be deleted, even on MSIE, we
		 * pick an expiry date in the past
		 */
		dt = php_format_date("D, d-M-Y H:i:s T", sizeof("D, d-M-Y H:i:s T")-1, 1, 0);
		smart_str_appends(&buf, "Set-Cookie: ");
		smart_str_append(&buf, name);
		smart_str_appends(&buf, "=deleted; expires=");
		smart_str_append(&buf, dt);
		smart_str_appends(&buf, "; Max-Age=0");
		zend_string_free(dt);
	} else {
		smart_str_appends(&buf, "Set-Cookie: ");
		smart_str_append(&buf, name);
		smart_str_appendc(&buf, '=');
		if (url_encode) {
			zend_string *encoded_value = php_raw_url_encode(ZSTR_VAL(value), ZSTR_LEN(value));
			smart_str_append(&buf, encoded_value);
			zend_string_release_ex(encoded_value, 0);
		} else {
			smart_str_append(&buf, value);
		}
		if (expires > 0) {
			const char *p;
			double diff;

			smart_str_appends(&buf, COOKIE_EXPIRES);
			dt = php_format_date("D, d-M-Y H:i:s T", sizeof("D, d-M-Y H:i:s T")-1, expires, 0);
			/* check to make sure that the year does not exceed 4 digits in length */
			p = zend_memrchr(ZSTR_VAL(dt), '-', ZSTR_LEN(dt));
			if (!p || *(p + 5) != ' ') {
				zend_string_free(dt);
				smart_str_free(&buf);
				zend_error(E_WARNING, "Expiry date cannot have a year greater than 9999");
				return FAILURE;
			}

			smart_str_append(&buf, dt);
			zend_string_free(dt);

			diff = difftime(expires, php_time());
			if (diff < 0) {
				diff = 0;
			}

			smart_str_appends(&buf, COOKIE_MAX_AGE);
			smart_str_append_long(&buf, (zend_long) diff);
		}
	}

	if (path && ZSTR_LEN(path)) {
		smart_str_appends(&buf, COOKIE_PATH);
		smart_str_append(&buf, path);
	}
	if (domain && ZSTR_LEN(domain)) {
		smart_str_appends(&buf, COOKIE_DOMAIN);
		smart_str_append(&buf, domain);
	}
	if (secure) {
		smart_str_appends(&buf, COOKIE_SECURE);
	}
	if (httponly) {
		smart_str_appends(&buf, COOKIE_HTTPONLY);
	}
	if (samesite && ZSTR_LEN(samesite)) {
		smart_str_appends(&buf, COOKIE_SAMESITE);
		smart_str_append(&buf, samesite);
	}

	ctr.line = ZSTR_VAL(buf.s);
	ctr.line_len = (uint32_t) ZSTR_LEN(buf.s);

	result = sapi_header_op(SAPI_HEADER_ADD, &ctr);
	zend_string_release(buf.s);
	return result;
}

static void php_head_parse_cookie_options_array(zval *options, zend_long *expires, zend_string **path, zend_string **domain, zend_bool *secure, zend_bool *httponly, zend_string **samesite) {
	int found = 0;
	zend_string *key;
	zval *value;

	ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(options), key, value) {
		if (key) {
			if (zend_string_equals_literal_ci(key, "expires")) {
				*expires = zval_get_long(value);
				found++;
			} else if (zend_string_equals_literal_ci(key, "path")) {
				*path = zval_get_string(value);
				found++;
			} else if (zend_string_equals_literal_ci(key, "domain")) {
				*domain = zval_get_string(value);
				found++;
			} else if (zend_string_equals_literal_ci(key, "secure")) {
				*secure = zval_is_true(value);
				found++;
			} else if (zend_string_equals_literal_ci(key, "httponly")) {
				*httponly = zval_is_true(value);
				found++;
			} else if (zend_string_equals_literal_ci(key, "samesite")) {
				*samesite = zval_get_string(value);
				found++;
			} else {
				php_error_docref(NULL, E_WARNING, "Unrecognized key '%s' found in the options array", ZSTR_VAL(key));
			}
		} else {
			php_error_docref(NULL, E_WARNING, "Numeric key found in the options array");
		}
	} ZEND_HASH_FOREACH_END();

	/* Array is not empty but no valid keys were found */
	if (found == 0 && zend_hash_num_elements(Z_ARRVAL_P(options)) > 0) {
		php_error_docref(NULL, E_WARNING, "No valid options were found in the given array");
	}
}

/* {{{ setcookie(string name [, string value [, array options]])
   Send a cookie */
PHP_FUNCTION(setcookie)
{
	zval *expires_or_options = NULL;
	zend_string *name, *value = NULL, *path = NULL, *domain = NULL, *samesite = NULL;
	zend_long expires = 0;
	zend_bool secure = 0, httponly = 0;

	ZEND_PARSE_PARAMETERS_START(1, 7)
		Z_PARAM_STR(name)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR(value)
		Z_PARAM_ZVAL(expires_or_options)
		Z_PARAM_STR(path)
		Z_PARAM_STR(domain)
		Z_PARAM_BOOL(secure)
		Z_PARAM_BOOL(httponly)
	ZEND_PARSE_PARAMETERS_END();

	if (expires_or_options) {
		if (Z_TYPE_P(expires_or_options) == IS_ARRAY) {
			if (UNEXPECTED(ZEND_NUM_ARGS() > 3)) {
				php_error_docref(NULL, E_WARNING, "Cannot pass arguments after the options array");
				RETURN_FALSE;
			}
			php_head_parse_cookie_options_array(expires_or_options, &expires, &path, &domain, &secure, &httponly, &samesite);
		} else {
			expires = zval_get_long(expires_or_options);
		}
	}

	if (!EG(exception)) {
		if (php_setcookie(name, value, expires, path, domain, secure, httponly, samesite, 1) == SUCCESS) {
			RETVAL_TRUE;
		} else {
			RETVAL_FALSE;
		}
	}

	if (expires_or_options && Z_TYPE_P(expires_or_options) == IS_ARRAY) {
		if (path) {
			zend_string_release(path);
		}
		if (domain) {
			zend_string_release(domain);
		}
		if (samesite) {
			zend_string_release(samesite);
		}
	}
}
/* }}} */

/* {{{ setrawcookie(string name [, string value [, array options]])
   Send a cookie with no url encoding of the value */
PHP_FUNCTION(setrawcookie)
{
	zval *expires_or_options = NULL;
	zend_string *name, *value = NULL, *path = NULL, *domain = NULL, *samesite = NULL;
	zend_long expires = 0;
	zend_bool secure = 0, httponly = 0;

	ZEND_PARSE_PARAMETERS_START(1, 7)
		Z_PARAM_STR(name)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR(value)
		Z_PARAM_ZVAL(expires_or_options)
		Z_PARAM_STR(path)
		Z_PARAM_STR(domain)
		Z_PARAM_BOOL(secure)
		Z_PARAM_BOOL(httponly)
	ZEND_PARSE_PARAMETERS_END();

	if (expires_or_options) {
		if (Z_TYPE_P(expires_or_options) == IS_ARRAY) {
			if (UNEXPECTED(ZEND_NUM_ARGS() > 3)) {
				php_error_docref(NULL, E_WARNING, "Cannot pass arguments after the options array");
				RETURN_FALSE;
			}
			php_head_parse_cookie_options_array(expires_or_options, &expires, &path, &domain, &secure, &httponly, &samesite);
		} else {
			expires = zval_get_long(expires_or_options);
		}
	}

	if (!EG(exception)) {
		if (php_setcookie(name, value, expires, path, domain, secure, httponly, samesite, 0) == SUCCESS) {
			RETVAL_TRUE;
		} else {
			RETVAL_FALSE;
		}
	}

	if (expires_or_options && Z_TYPE_P(expires_or_options) == IS_ARRAY) {
		if (path) {
			zend_string_release(path);
		}
		if (domain) {
			zend_string_release(domain);
		}
		if (samesite) {
			zend_string_release(samesite);
		}
	}
}
/* }}} */


/* {{{ Returns true if headers have already been sent, false otherwise */
PHP_FUNCTION(headers_sent)
{
	zval *arg1 = NULL, *arg2 = NULL;
	const char *file="";
	int line=0;

	ZEND_PARSE_PARAMETERS_START(0, 2)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(arg1)
		Z_PARAM_ZVAL(arg2)
	ZEND_PARSE_PARAMETERS_END();

	if (SG(headers_sent)) {
		line = php_output_get_start_lineno();
		file = php_output_get_start_filename();
	}

	switch(ZEND_NUM_ARGS()) {
	case 2:
		ZEND_TRY_ASSIGN_REF_LONG(arg2, line);
	case 1:
		if (file) {
			ZEND_TRY_ASSIGN_REF_STRING(arg1, file);
		} else {
			ZEND_TRY_ASSIGN_REF_EMPTY_STRING(arg1);
		}
		break;
	}

	if (SG(headers_sent)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ php_head_apply_header_list_to_hash
   Turn an llist of sapi_header_struct headers into a numerically indexed zval hash */
static void php_head_apply_header_list_to_hash(void *data, void *arg)
{
	sapi_header_struct *sapi_header = (sapi_header_struct *)data;

	if (arg && sapi_header) {
		add_next_index_string((zval *)arg, (char *)(sapi_header->header));
	}
}

/* {{{ Return list of headers to be sent / already sent */
PHP_FUNCTION(headers_list)
{
	ZEND_PARSE_PARAMETERS_NONE();

	array_init(return_value);
	zend_llist_apply_with_argument(&SG(sapi_headers).headers, php_head_apply_header_list_to_hash, return_value);
}
/* }}} */

/* {{{ Sets a response code, or returns the current HTTP response code */
PHP_FUNCTION(http_response_code)
{
	zend_long response_code = 0;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(response_code)
	ZEND_PARSE_PARAMETERS_END();

	if (response_code)
	{
		zend_long old_response_code;

		old_response_code = SG(sapi_headers).http_response_code;
		SG(sapi_headers).http_response_code = (int)response_code;

		if (old_response_code) {
			RETURN_LONG(old_response_code);
		}

		RETURN_TRUE;
	}

	if (!SG(sapi_headers).http_response_code) {
		RETURN_FALSE;
	}

	RETURN_LONG(SG(sapi_headers).http_response_code);
}
/* }}} */
