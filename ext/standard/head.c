/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2001 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#include <stdio.h>
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
/* {{{ proto void header(string header[, bool replace])
   Send a raw HTTP header */
PHP_FUNCTION(header)
{
	pval **arg1, **arg2;
	zend_bool replace = 1;
	
	if (ZEND_NUM_ARGS() < 1 || ZEND_NUM_ARGS() > 2 
			|| zend_get_parameters_ex(ZEND_NUM_ARGS(), &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	switch (ZEND_NUM_ARGS()) {
		case 2:
			convert_to_boolean_ex(arg2);
			replace = Z_BVAL_PP(arg2);
		case 1:
			convert_to_string_ex(arg1);
	}
	sapi_add_header_ex(Z_STRVAL_PP(arg1), Z_STRLEN_PP(arg1), 1, replace TSRMLS_CC);
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



/* php_set_cookie(name, value, expires, path, domain, secure) */
/* {{{ proto void setcookie(string name [, string value [, int expires [, string path [, string domain [, string secure]]]]])
   Send a cookie */
PHP_FUNCTION(setcookie)
{
	char *cookie, *encoded_value = NULL;
	int len=sizeof("Set-Cookie: ");
	time_t t;
	char *dt;
	time_t expires = 0;
	int secure = 0;
	pval **arg[6];
	int arg_count;
	zval **z_name=NULL, **z_value=NULL, **z_path=NULL, **z_domain=NULL;

	arg_count = ZEND_NUM_ARGS();
	if (arg_count < 1 || arg_count > 6 || zend_get_parameters_array_ex(arg_count, arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	switch (arg_count) {
		case 6:
			convert_to_boolean_ex(arg[5]);
			secure = Z_LVAL_PP(arg[5]);
			/* break missing intentionally */
		case 5:
			convert_to_string_ex(arg[4]);
			z_domain = arg[4];
			/* break missing intentionally */
		case 4:
			convert_to_string_ex(arg[3]);
			z_path = arg[3];
			/* break missing intentionally */
		case 3:
			convert_to_long_ex(arg[2]);
			expires = Z_LVAL_PP(arg[2]);
			/* break missing intentionally */
		case 2:
			convert_to_string_ex(arg[1]);
			z_value = arg[1];
			/* break missing intentionally */
		case 1:
			convert_to_string_ex(arg[0]);
			z_name = arg[0];
			break;
	}
	if (z_name) {
		len += Z_STRLEN_PP(z_name);
	}
	if (z_value) {
		int encoded_value_len;

		encoded_value = php_url_encode(Z_STRVAL_PP(z_value), Z_STRLEN_PP(z_value), &encoded_value_len);
		len += encoded_value_len;
	}
	if (z_path) {
		len += Z_STRLEN_PP(z_path);
	}
	if (z_domain) {
		len += Z_STRLEN_PP(z_domain);
	}
	cookie = emalloc(len + 100);
	if (z_value && Z_STRLEN_PP(z_value)==0) {
		/* 
		 * MSIE doesn't delete a cookie when you set it to a null value
		 * so in order to force cookies to be deleted, even on MSIE, we
		 * pick an expiry date 1 year and 1 second in the past
		 */
		t = time(NULL) - 31536001;
		dt = php_std_date(t);
		sprintf(cookie, "Set-Cookie: %s=deleted; expires=%s", Z_STRVAL_PP(z_name), dt);
		efree(dt);
	} else {
		sprintf(cookie, "Set-Cookie: %s=%s", Z_STRVAL_PP(z_name), (z_value && Z_STRVAL_PP(z_value)) ? encoded_value : "");
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

	if (z_path && Z_STRLEN_PP(z_path)>0) {
		strcat(cookie, "; path=");
		strcat(cookie, Z_STRVAL_PP(z_path));
	}
	if (z_domain && Z_STRLEN_PP(z_domain)>0) {
		strcat(cookie, "; domain=");
		strcat(cookie, Z_STRVAL_PP(z_domain));
	}
	if (secure) {
		strcat(cookie, "; secure");
	}

	if (sapi_add_header(cookie, strlen(cookie), 0)==SUCCESS) {
		RETVAL_TRUE;
	} else {
		RETVAL_FALSE;
	}
}
/* }}} */


/* {{{ proto int headers_sent(void)
   Return true if headers have already been sent, false otherwise */
PHP_FUNCTION(headers_sent)
{
	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
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
 * vim600: sw=4 ts=4 tw=78 fdm=marker
 * vim<600: sw=4 ts=4 tw=78 * End:
 */
