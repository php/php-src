/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                        |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#include <stdio.h>

#if defined(NETWARE) && !defined(NEW_LIBC)
#include <sys/socket.h>
#endif

#include "php.h"
#include "ext/standard/php_standard.h"
#include "SAPI.h"
#include "php_main.h"
#include "head.h"
#include "SAPI.h"
#ifdef TM_IN_SYS_TIME
#include <sys/time.h>
#else
#include <time.h>
#endif

#include "php_globals.h"
#include "safe_mode.h"


/* Implementation of the language Header() function */
/* {{{ proto void header(string header [, bool replace])
   Sends a raw HTTP header */
PHP_FUNCTION(header)
{
	char *header;
	int header_len;
	zend_bool replace = 1;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|b", &header,
							  &header_len, &replace) == FAILURE) {
		return;
	}
	sapi_add_header_ex(header, header_len, 1, replace TSRMLS_CC);
}
/* }}} */

PHPAPI int php_header()
{
	TSRMLS_FETCH();

	if (sapi_send_headers(TSRMLS_C)==FAILURE || SG(request_info).headers_only) {
		return 0; /* don't allow output */
	} else {
		return 1; /* allow output */
	}
}


PHPAPI int php_setcookie(char *name, int name_len, char *value, int value_len, time_t expires, char *path, int path_len, char *domain, int domain_len, int secure TSRMLS_DC)
{
	char *cookie, *encoded_value = NULL;
	int len=sizeof("Set-Cookie: ");
	time_t t;
	char *dt;

	len += name_len;
	if (value) {
		int encoded_value_len;

		encoded_value = php_url_encode(value, value_len, &encoded_value_len);
		len += encoded_value_len;
	}
	if (path) {
		len += path_len;
	}
	if (domain) {
		len += domain_len;
	}
	cookie = emalloc(len + 100);

	if (value && value_len == 0) {
		/* 
		 * MSIE doesn't delete a cookie when you set it to a null value
		 * so in order to force cookies to be deleted, even on MSIE, we
		 * pick an expiry date 1 year and 1 second in the past
		 */
		t = time(NULL) - 31536001;
		dt = php_std_date(t);
		sprintf(cookie, "Set-Cookie: %s=deleted; expires=%s", name, dt);
		efree(dt);
	} else {
		sprintf(cookie, "Set-Cookie: %s=%s", name, value ? encoded_value : "");
		if (expires > 0) {
			strcat(cookie, "; expires=");
			dt = php_std_date(expires);
			strcat(cookie, dt);
			efree(dt);
		}
	}

	if (encoded_value) {
		efree(encoded_value);
	}

	if (path && path_len > 0) {
		strcat(cookie, "; path=");
		strcat(cookie, path);
	}
	if (domain && domain_len > 0) {
		strcat(cookie, "; domain=");
		strcat(cookie, domain);
	}
	if (secure) {
		strcat(cookie, "; secure");
	}

	return sapi_add_header_ex(cookie, strlen(cookie), 0, 0 TSRMLS_CC);
}


/* php_set_cookie(name, value, expires, path, domain, secure) */
/* {{{ proto bool setcookie(string name [, string value [, int expires [, string path [, string domain [, bool secure]]]]])
   Send a cookie */
PHP_FUNCTION(setcookie)
{
	char *name, *value = NULL, *path = NULL, *domain = NULL;
	time_t expires = 0;
	zend_bool secure = 0;
	int name_len, value_len, path_len, domain_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|slssb", &name,
							  &name_len, &value, &value_len, &expires, &path,
							  &path_len, &domain, &domain_len, &secure) == FAILURE) {
		return;
	}

	if (php_setcookie(name, name_len, value, value_len, expires, path, path_len, domain, domain_len, secure TSRMLS_CC) == SUCCESS) {
		RETVAL_TRUE;
	} else {
		RETVAL_FALSE;
	}
}
/* }}} */


/* {{{ proto int headers_sent(void)
   Returns true if headers have already been sent, false otherwise */
PHP_FUNCTION(headers_sent)
{
	if (ZEND_NUM_ARGS() != 0) {
		php_error(E_WARNING, "%s() expects no parameters, %d given",
				  get_active_function_name(TSRMLS_C), ZEND_NUM_ARGS());
		return;
	}

	if (SG(headers_sent)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4 * End:
 */
