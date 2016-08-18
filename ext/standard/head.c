/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2016 The PHP Group                                |
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
/* $Id$ */

#include <stdio.h>
#include "php.h"
#include "ext/standard/php_standard.h"
#include "ext/date/php_date.h"
#include "SAPI.h"
#include "php_main.h"
#include "head.h"
#ifdef TM_IN_SYS_TIME
#include <sys/time.h>
#else
#include <time.h>
#endif

#include "php_globals.h"


/* Implementation of the language Header() function */
/* {{{ proto void header(string header [, bool replace, [int http_response_code]])
   Sends a raw HTTP header */
PHP_FUNCTION(header)
{
	zend_bool rep = 1;
	sapi_header_line ctr = {0};
	size_t len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|bl", &ctr.line,
				&len, &rep, &ctr.response_code) == FAILURE)
		return;

	ctr.line_len = (uint)len;
	sapi_header_op(rep ? SAPI_HEADER_REPLACE:SAPI_HEADER_ADD, &ctr);
}
/* }}} */

/* {{{ proto void header_remove([string name])
   Removes an HTTP header previously set using header() */
PHP_FUNCTION(header_remove)
{
	sapi_header_line ctr = {0};
	size_t len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|s", &ctr.line,
	                          &len) == FAILURE)
		return;

	ctr.line_len = (uint)len;
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


PHPAPI int php_setcookie(zend_string *name, zend_string *value, time_t expires, zend_string *path, zend_string *domain, int secure, int url_encode, int httponly)
{
	char *cookie;
	size_t len = sizeof("Set-Cookie: ");
	zend_string *dt;
	sapi_header_line ctr = {0};
	int result;
	zend_string *encoded_value = NULL;

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

	len += ZSTR_LEN(name);
	if (value) {
		if (url_encode) {
			encoded_value = php_url_encode(ZSTR_VAL(value), ZSTR_LEN(value));
			len += ZSTR_LEN(encoded_value);
		} else {
			encoded_value = zend_string_copy(value);
			len += ZSTR_LEN(encoded_value);
		}
	}

	if (path) {
		len += ZSTR_LEN(path);
	}
	if (domain) {
		len += ZSTR_LEN(domain);
	}

	cookie = emalloc(len + 100);

	if (value == NULL || ZSTR_LEN(value) == 0) {
		/*
		 * MSIE doesn't delete a cookie when you set it to a null value
		 * so in order to force cookies to be deleted, even on MSIE, we
		 * pick an expiry date in the past
		 */
		dt = php_format_date("D, d-M-Y H:i:s T", sizeof("D, d-M-Y H:i:s T")-1, 1, 0);
		snprintf(cookie, len + 100, "Set-Cookie: %s=deleted; expires=%s; Max-Age=0", ZSTR_VAL(name), ZSTR_VAL(dt));
		zend_string_free(dt);
	} else {
		snprintf(cookie, len + 100, "Set-Cookie: %s=%s", ZSTR_VAL(name), value ? ZSTR_VAL(encoded_value) : "");
		if (expires > 0) {
			const char *p;
			char tsdelta[13];
			strlcat(cookie, COOKIE_EXPIRES, len + 100);
			dt = php_format_date("D, d-M-Y H:i:s T", sizeof("D, d-M-Y H:i:s T")-1, expires, 0);
			/* check to make sure that the year does not exceed 4 digits in length */
			p = zend_memrchr(ZSTR_VAL(dt), '-', ZSTR_LEN(dt));
			if (!p || *(p + 5) != ' ') {
				zend_string_free(dt);
				efree(cookie);
				zend_string_release(encoded_value);
				zend_error(E_WARNING, "Expiry date cannot have a year greater than 9999");
				return FAILURE;
			}
			strlcat(cookie, ZSTR_VAL(dt), len + 100);
			zend_string_free(dt);

			snprintf(tsdelta, sizeof(tsdelta), ZEND_LONG_FMT, (zend_long) difftime(expires, time(NULL)));
			strlcat(cookie, COOKIE_MAX_AGE, len + 100);
			strlcat(cookie, tsdelta, len + 100);
		}
	}

	if (encoded_value) {
		zend_string_release(encoded_value);
	}

	if (path && ZSTR_LEN(path)) {
		strlcat(cookie, COOKIE_PATH, len + 100);
		strlcat(cookie, ZSTR_VAL(path), len + 100);
	}
	if (domain && ZSTR_LEN(domain)) {
		strlcat(cookie, COOKIE_DOMAIN, len + 100);
		strlcat(cookie, ZSTR_VAL(domain), len + 100);
	}
	if (secure) {
		strlcat(cookie, COOKIE_SECURE, len + 100);
	}
	if (httponly) {
		strlcat(cookie, COOKIE_HTTPONLY, len + 100);
	}

	ctr.line = cookie;
	ctr.line_len = (uint)strlen(cookie);

	result = sapi_header_op(SAPI_HEADER_ADD, &ctr);
	efree(cookie);
	return result;
}


/* php_set_cookie(name, value, expires, path, domain, secure) */
/* {{{ proto bool setcookie(string name [, string value [, int expires [, string path [, string domain [, bool secure[, bool httponly]]]]]])
   Send a cookie */
PHP_FUNCTION(setcookie)
{
	zend_string *name, *value = NULL, *path = NULL, *domain = NULL;
	zend_long expires = 0;
	zend_bool secure = 0, httponly = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|SlSSbb",
				&name, &value, &expires, &path, &domain, &secure, &httponly) == FAILURE) {
		return;
	}

	if (php_setcookie(name, value, expires, path, domain, secure, 1, httponly) == SUCCESS) {
		RETVAL_TRUE;
	} else {
		RETVAL_FALSE;
	}
}
/* }}} */

/* {{{ proto bool setrawcookie(string name [, string value [, int expires [, string path [, string domain [, bool secure[, bool httponly]]]]]])
   Send a cookie with no url encoding of the value */
PHP_FUNCTION(setrawcookie)
{
	zend_string *name, *value = NULL, *path = NULL, *domain = NULL;
	zend_long expires = 0;
	zend_bool secure = 0, httponly = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|SlSSbb",
				&name, &value, &expires, &path, &domain, &secure, &httponly) == FAILURE) {
		return;
	}

	if (php_setcookie(name, value, expires, path, domain, secure, 0, httponly) == SUCCESS) {
		RETVAL_TRUE;
	} else {
		RETVAL_FALSE;
	}
}
/* }}} */


/* {{{ proto bool headers_sent([string &$file [, int &$line]])
   Returns true if headers have already been sent, false otherwise */
PHP_FUNCTION(headers_sent)
{
	zval *arg1 = NULL, *arg2 = NULL;
	const char *file="";
	int line=0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|z/z/", &arg1, &arg2) == FAILURE)
		return;

	if (SG(headers_sent)) {
		line = php_output_get_start_lineno();
		file = php_output_get_start_filename();
	}

	switch(ZEND_NUM_ARGS()) {
	case 2:
		zval_dtor(arg2);
		ZVAL_LONG(arg2, line);
	case 1:
		zval_dtor(arg1);
		if (file) {
			ZVAL_STRING(arg1, file);
		} else {
			ZVAL_EMPTY_STRING(arg1);
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

/* {{{ proto array headers_list(void)
   Return list of headers to be sent / already sent */
PHP_FUNCTION(headers_list)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	array_init(return_value);
	zend_llist_apply_with_argument(&SG(sapi_headers).headers, php_head_apply_header_list_to_hash, return_value);
}
/* }}} */

/* {{{ proto long http_response_code([int response_code])
   Sets a response code, or returns the current HTTP response code */
PHP_FUNCTION(http_response_code)
{
	zend_long response_code = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|l", &response_code) == FAILURE) {
		return;
	}

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

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4 * End:
 */
