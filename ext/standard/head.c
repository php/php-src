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


/* need to figure out some nice way to get rid of these */
#ifndef THREAD_SAFE
static int php_header_printed = 0;
static int php_print_header = 1;
static CookieList *top = NULL;
static char *cont_type = NULL;
static int header_called = 0;
#endif

void php_push_cookie_list(char *, char *, time_t, char *, char *, int);
CookieList *php_pop_cookie_list(void);

PHP_RINIT_FUNCTION(head)
{
	php_header_printed = 0;
	if (header_called == 0)
		php_print_header = 1;
	top = NULL;
	cont_type = NULL;

	return SUCCESS;
}


/* Implementation of the language Header() function */
/* {{{ proto void header(string header[, bool replace])
   Send a raw HTTP header */
PHP_FUNCTION(Header)
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
	sapi_add_header_ex(Z_STRVAL_PP(arg1), Z_STRLEN_PP(arg1), 1, replace);
}
/* }}} */

/* {{{ php_header
 */
PHPAPI int php_header()
{
	SLS_FETCH();

	if (sapi_send_headers()==FAILURE || SG(request_info).headers_only) {
		return 0; /* don't allow output */
	} else {
		return 1; /* allow output */
	}
}
/* }}} */

/* {{{ php_push_cookie_list
 */
void php_push_cookie_list(char *name, char *value, time_t expires, char *path, char *domain, int secure)
{
	CookieList *new;

	new = emalloc(sizeof(CookieList));
	new->next = top;
	new->name = name;
	new->value = value;
	new->expires = expires;
	new->path = path;
	new->domain = domain;
	new->secure = secure;
	top = new;
}
/* }}} */

/* {{{ php_pop_cookie_list
 */
CookieList *php_pop_cookie_list(void)
{
	CookieList *ret;

	ret = top;
	if (top)
		top = top->next;
	return (ret);
}
/* }}} */

/* php_set_cookie(name,value,expires,path,domain,secure) */
/* {{{ proto void setcookie(string name [, string value [, int expires [, string path [, string domain [, string secure]]]]])
   Send a cookie */
PHP_FUNCTION(setcookie)
{
	char *cookie, *encoded_value = NULL;
	int len=sizeof("Set-Cookie: ");
	time_t t;
	char *dt;
	char *name = NULL, *value = NULL, *path = NULL, *domain = NULL;
	time_t expires = 0;
	int secure = 0;
	pval **arg[6];
	int arg_count;

	arg_count = ZEND_NUM_ARGS();
	if (arg_count < 1 || arg_count > 6 || zend_get_parameters_array_ex(arg_count, arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	if (php_header_printed == 1) {
		php_error(E_WARNING, "Oops, php_set_cookie called after header has been sent\n");
		return;
	}
	switch (arg_count) {
		case 6:
			convert_to_boolean_ex(arg[5]);
			secure = (*arg[5])->value.lval;
			/* break missing intentionally */
		case 5:
			convert_to_string_ex(arg[4]);
			domain = estrndup((*arg[4])->value.str.val,(*arg[4])->value.str.len);
			/* break missing intentionally */
		case 4:
			convert_to_string_ex(arg[3]);
			path = estrndup((*arg[3])->value.str.val,(*arg[3])->value.str.len);
			/* break missing intentionally */
		case 3:
			convert_to_long_ex(arg[2]);
			expires = (*arg[2])->value.lval;
			/* break missing intentionally */
		case 2:
			convert_to_string_ex(arg[1]);
			value = estrndup((*arg[1])->value.str.val,(*arg[1])->value.str.len);
			/* break missing intentionally */
		case 1:
			convert_to_string_ex(arg[0]);
			name = estrndup((*arg[0])->value.str.val,(*arg[0])->value.str.len);
			break;
	}
#if 0
	php_push_cookie_list(name, value, expires, path, domain, secure);
#else
	if (name) {
		len += strlen(name);
	}
	if (value) {
		encoded_value = php_url_encode(value, strlen (value));
		len += strlen(encoded_value);
	}
	if (path) {
		len += strlen(path);
	}
	if (domain) {
		len += strlen(domain);
	}
	cookie = emalloc(len + 100);
	if (!value || (value && !*value)) {
		/* 
		 * MSIE doesn't delete a cookie when you set it to a null value
		 * so in order to force cookies to be deleted, even on MSIE, we
		 * pick an expiry date 1 year and 1 second in the past
		 */
		sprintf(cookie, "Set-Cookie: %s=deleted", name);
		strcat(cookie, "; expires=");
		t = time(NULL) - 31536001;
		dt = php_std_date(t);
		strcat(cookie, dt);
		efree(dt);
	} else {
		/* FIXME: XXX: this is not binary data safe */
		sprintf(cookie, "Set-Cookie: %s=%s", name, value ? encoded_value : "");
		if (value) efree(value);
		value=NULL;
		if (name) efree(name);
		name=NULL;
		if (expires > 0) {
			strcat(cookie, "; expires=");
			dt = php_std_date(expires);
			strcat(cookie, dt);
			efree(dt);
		}
	}

	if (encoded_value) efree(encoded_value);

	if (path && strlen(path)) {
		strcat(cookie, "; path=");
		strcat(cookie, path);
		efree(path);
		path=NULL;
	}
	if (domain && strlen(domain)) {
		strcat(cookie, "; domain=");
		strcat(cookie, domain);
		efree(domain);
		domain=NULL;
	}
	if (secure) {
		strcat(cookie, "; secure");
	}

	if (sapi_add_header(cookie, strlen(cookie), 0)==SUCCESS) {
		RETVAL_TRUE;
	} else {
		RETVAL_FALSE;
	}

	if (domain) {
		efree(domain);
	}
	if (path) {
		efree(path);
	}
	if (name) {
		efree(name);
	}
	if (value) {
		efree(value);
	}
#endif
}
/* }}} */

/* {{{ php_headers_unsent
 */
int php_headers_unsent(void)
{
	if (php_header_printed!=1 || !php_print_header) {
		return 1;
	} else {
		return 0;
	}
}
/* }}} */

/* {{{ proto int headers_sent(void)
   Return true if headers have already been sent, false otherwise */
PHP_FUNCTION(headers_sent)
{
	SLS_FETCH();
	
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
 * End:
 * vim600: sw=4 ts=4 tw=78 fdm=marker
 * vim<600: sw=4 ts=4 tw=78
 */
