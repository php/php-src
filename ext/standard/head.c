/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_0.txt.                                  |
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
#include "main.h"
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


#if 0
/* Adds header information */
void php4i_add_header_information(char *header_information, uint header_length)
{
	char *r;
#if APACHE
	char *rr = NULL;
	char *temp = NULL;
	long myuid = 0L;
	char temp2[32];
	request_rec *req;
	SLS_FETCH();
	
	req = ((request_rec *) SG(server_context));
#endif

	if (php_header_printed == 1) {
#if DEBUG
		php_error(E_WARNING, "Cannot add more header information - the header was already sent "
							  "(header information may be added only before any output is generated from the script - "
							  "check for text or whitespace outside PHP tags, or calls to functions that output text)");
#endif
		return;					/* too late, already sent */
	}
#if APACHE
	/*
	 * Not entirely sure this is the right way to support the header
	 * command in the Apache module.  Comments?
	 */
	r = strchr(header_information, ':');
	if (r) {
		*r = '\0';
		if (!strcasecmp(header_information, "Content-type")) {
			if (*(r + 1) == ' ')
				req->content_type = pstrdup(req->pool,r + 2);
			else
				req->content_type = pstrdup(req->pool,r + 1);
			cont_type = (char *) req->content_type;
		} else {
			if (*(r + 1) == ' ') {
				rr = r + 2;
			} else {
				rr = r + 1;
			}
			if (PG(safe_mode) && (!strcasecmp(header_information, "WWW-authenticate"))) {
				myuid = php_getuid();
				sprintf(temp2, "realm=\"%ld ", myuid);  /* SAFE */
				temp = php_reg_replace("realm=\"", temp2, rr, 1, 0);
				if (!strcmp(temp, rr)) {
					sprintf(temp2, "realm=%ld", myuid);  /* SAFE */
					temp = php_reg_replace("realm=", temp2, rr, 1, 0);
					if (!strcmp(temp, rr)) {
						sprintf(temp2, " realm=%ld", myuid);  /* SAFE */
						temp = php_reg_replace("$", temp2, rr, 0, 0);
					}
				}
				table_set(req->headers_out, header_information, temp);
			} else
				table_set(req->headers_out, header_information, rr);
		}
		if (!strcasecmp(header_information, "location")) {
			req->status = REDIRECT;
		}
		*r = ':';
		php_header_printed = 2;
	}
	if (!strncasecmp(header_information, "http/", 5)) {
		if (strlen(header_information) > 9) {
			req->status = atoi(&((header_information)[9]));
		}
		/* Use a pstrdup here to get the memory straight from Apache's per-request pool to
		 * avoid having our own memory manager complain about this memory not being freed
		 * because it really shouldn't be freed until the end of the request and it isn't
		 * easy for us to figure out when we allocated it vs. when something else might have.
		 */
		req->status_line = pstrdup(req->pool,&((header_information)[9]));
	}
#else
	r = strchr(header_information, ':');
	if (r) {
		*r = '\0';
		if (!strcasecmp(header_information, "Content-type")) {
			if (cont_type) efree(cont_type);
			cont_type = estrdup(r + 1);
#if 0 /*WIN32|WINNT / *M$ does us again*/
			if (!strcmp(cont_type," text/html")){
				*r=':';
				PUTS_H(header_information);
				PUTS_H("\015\012");
			}
#endif
		} else {
			*r = ':';
#if USE_SAPI
			{
				char *tempstr=emalloc(strlen(header_information)+2);
				
				sprintf(tempstr,"%s\015\012",tempstr);
				sapi_rqst->header(sapi_rqst->scid,tempstr);
				efree(tempstr);
			}
#else
			PUTS_H(header_information);
			PUTS_H("\015\012");
#endif
		}
	} else {
#if USE_SAPI
		{
		char *tempstr=emalloc(strlen(header_information)+2);
		sprintf(tempstr,"%s\015\012",tempstr);
		sapi_rqst->header(sapi_rqst->scid,tempstr);
		efree(tempstr);
		}
#else
		PUTS_H(header_information);
		PUTS_H("\015\012");
#endif
	}
#endif
}
#else
void php4i_add_header_information(char *header_information, uint header_length)
{
	sapi_add_header(header_information, header_length);
}
#endif


/* Implementation of the language Header() function */
PHP_FUNCTION(Header)
{
	pval **arg1;

	if (zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(arg1);
	php4i_add_header_information((*arg1)->value.str.val,(*arg1)->value.str.len);
	(*arg1)->type = IS_LONG; /* change arg1's type so that it doesn't get freed */
	(*arg1)->value.lval = 0;
}


PHPAPI int php_header()
{
	SLS_FETCH();

	if (sapi_send_headers()==FAILURE || SG(request_info).headers_only) {
		return 0; /* don't allow output */
	} else {
		return 1; /* allow output */
	}
}


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

CookieList *php_pop_cookie_list(void)
{
	CookieList *ret;

	ret = top;
	if (top)
		top = top->next;
	return (ret);
}

/* php_set_cookie(name,value,expires,path,domain,secure) */
PHP_FUNCTION(setcookie)
{
	char *cookie;
	int len=sizeof("Set-Cookie: ");
	time_t t;
	char *r, *dt;
	char *name = NULL, *value = NULL, *path = NULL, *domain = NULL;
	time_t expires = 0;
	int secure = 0;
	pval **arg[6];
	int arg_count;

	arg_count = ARG_COUNT(ht);
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
		len += strlen(value);
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
		r = php_url_encode(value, strlen (value));
		sprintf(cookie, "Set-Cookie: %s=%s", name, value ? r : "");
		if (r) efree(r);
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


	if (sapi_add_header(cookie, strlen(cookie))==SUCCESS) {
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


int php_headers_unsent(void)
{
	if (php_header_printed!=1 || !php_print_header) {
		return 1;
	} else {
		return 0;
	}
}

PHP_FUNCTION(headers_sent)
{
	SLS_FETCH();
	
	if (SG(headers_sent)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}

function_entry php_header_functions[] = {
	{NULL, NULL, NULL}
};


zend_module_entry php_header_module_entry = {
	"PHP_head", php_header_functions, NULL, NULL, PHP_RINIT(head), NULL, NULL, STANDARD_MODULE_PROPERTIES
};

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
