/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Sterling Hughes <sterling@php.net>                           |
   +----------------------------------------------------------------------+
*/

/* $Id$ */


#include "php.h"

#if HAVE_CURL

#include <stdio.h>
#include <curl/curl.h>
#include <curl/easy.h>
#include "ext/standard/info.h"
#include "php_curl.h"

static void _curl_close (CURL *cp);
static int  le_curl;


function_entry curl_functions[] = {
	PHP_FE (curl_init,		NULL)
	PHP_FE (curl_setopt,	NULL)
	PHP_FE (curl_exec,		NULL)
	PHP_FE (curl_close,		NULL)
	{NULL, NULL, NULL}
};


zend_module_entry curl_module_entry = {
	"curl",
	curl_functions,
	PHP_MINIT(curl),
	NULL,
	NULL,
	NULL,
	PHP_MINFO(curl),
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_CURL
ZEND_GET_MODULE (curl)
#endif

PHP_MINFO_FUNCTION(curl)
{
	php_info_print_table_start ();
	php_info_print_table_row (2, "CURL support",     "enabled");
	php_info_print_table_row (2, "CURL Information", curl_version());
	php_info_print_table_end ();
}

PHP_MINIT_FUNCTION(curl)
{
	le_curl = register_list_destructors (_curl_close, NULL);
	REGISTER_LONG_CONSTANT ("CURLOPT_PORT", CURLOPT_PORT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT ("CURLOPT_FILE", CURLOPT_FILE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT ("CURLOPT_INFILE", CURLOPT_INFILE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT ("CURLOPT_INFILESIZE", CURLOPT_INFILESIZE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT ("CURLOPT_URL", CURLOPT_URL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT ("CURLOPT_PROXY", CURLOPT_PROXY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT ("CURLOPT_VERBOSE", CURLOPT_VERBOSE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT ("CURLOPT_HEADER", CURLOPT_HEADER, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT ("CURLOPT_NOPROGRESS", CURLOPT_NOPROGRESS, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT ("CURLOPT_NOBODY", CURLOPT_NOBODY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT ("CURLOPT_FAILONERROR", CURLOPT_FAILONERROR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT ("CURLOPT_UPLOAD", CURLOPT_UPLOAD, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT ("CURLOPT_POST", CURLOPT_POST, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT ("CURLOPT_FTPLISTONLY", CURLOPT_FTPLISTONLY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT ("CURLOPT_FTPAPPEND", CURLOPT_FTPAPPEND, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT ("CURLOPT_NETRC", CURLOPT_NETRC, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT ("CURLOPT_FOLLOWLOCATION", CURLOPT_FOLLOWLOCATION, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT ("CURLOPT_FTPASCII", CURLOPT_FTPASCII, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT ("CURLOPT_PUT", CURLOPT_PUT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT ("CURLOPT_MUTE", CURLOPT_MUTE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT ("CURLOPT_USERPWD", CURLOPT_USERPWD, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT ("CURLOPT_PROXYUSERPWD", CURLOPT_PROXYUSERPWD, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT ("CURLOPT_RANGE", CURLOPT_RANGE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT ("CURLOPT_TIMEOUT", CURLOPT_TIMEOUT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT ("CURLOPT_POSTFIELDS", CURLOPT_POSTFIELDS, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT ("CURLOPT_REFERER", CURLOPT_REFERER, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT ("CURLOPT_USERAGENT", CURLOPT_USERAGENT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT ("CURLOPT_FTPPORT", CURLOPT_FTPPORT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT ("CURLOPT_LOW_SPEED_LIMIT", CURLOPT_LOW_SPEED_LIMIT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT ("CURLOPT_LOW_SPEED_TIME", CURLOPT_LOW_SPEED_TIME, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT ("CURLOPT_RESUME_FROM", CURLOPT_RESUME_FROM, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT ("CURLOPT_COOKIE", CURLOPT_COOKIE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT ("CURLOPT_SSLCERT", CURLOPT_SSLCERT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT ("CURLOPT_SSLCERTPASSWD", CURLOPT_SSLCERTPASSWD, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT ("CURLOPT_WRITEHEADER", CURLOPT_WRITEHEADER, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT ("CURLOPT_COOKIEFILE", CURLOPT_COOKIEFILE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT ("CURLOPT_SSLVERSION", CURLOPT_SSLVERSION, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT ("CURLOPT_TIMECONDITION", CURLOPT_TIMECONDITION, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT ("CURLOPT_TIMEVALUE", CURLOPT_TIMEVALUE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT ("CURLOPT_CUSTOMREQUEST", CURLOPT_CUSTOMREQUEST, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT ("CURLOPT_STDERR", CURLOPT_STDERR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT ("CURLOPT_WRITEINFO", CURLOPT_WRITEINFO, CONST_CS | CONST_PERSISTENT);
	return SUCCESS;
}


static void _curl_close (CURL *cp)
{
	curl_easy_cleanup (cp);
}

/* {{{ proto int curl_init ([string url])
   Initialize a CURL session */
PHP_FUNCTION (curl_init)
{
	zval **uUrl;
	CURL *cp;
	int numArgs = ZEND_NUM_ARGS();
	
	if (numArgs < 0 || numArgs > 1 ||
	    zend_get_parameters_ex (numArgs, &uUrl) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	cp = curl_easy_init ();
	
	if (!cp) {
		php_error (E_ERROR, "Cannot initialize CURL Handle");
		RETURN_FALSE;
	}
	
	if (numArgs > 0) {
		convert_to_string_ex (uUrl);
		curl_easy_setopt (cp, CURLOPT_URL, Z_STRVAL_PP (uUrl));
	}
	
	ZEND_REGISTER_RESOURCE (return_value, cp, le_curl);
}
/* }}} */

/* {{{ proto bool curl_setopt (int ch, string option, mixed value)
   Set an option for a CURL transfer */
PHP_FUNCTION (curl_setopt)
{
	zval **uCurlId, **uCurlOption, **uCurlValue;
	CURL *cp;
	CURLcode ret;
	int option;
	
	if (ZEND_NUM_ARGS() != 3 ||
	    zend_get_parameters_ex (3, &uCurlId, &uCurlOption, &uCurlValue) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE (cp, CURL *, uCurlId, -1, "CURL Handle", le_curl);
	
	convert_to_long_ex (uCurlOption);
	option = Z_LVAL_PP (uCurlOption);
	
	if (option == CURLOPT_INFILESIZE      || option == CURLOPT_VERBOSE        ||
	    option == CURLOPT_HEADER          || option == CURLOPT_NOPROGRESS     ||
	    option == CURLOPT_NOBODY          || option == CURLOPT_FAILONERROR    ||
	    option == CURLOPT_UPLOAD          || option == CURLOPT_POST           ||
	    option == CURLOPT_FTPLISTONLY     || option == CURLOPT_FTPAPPEND      ||
	    option == CURLOPT_NETRC           || option == CURLOPT_FOLLOWLOCATION ||
	    option == CURLOPT_PUT             ||
	    option == CURLOPT_MUTE            || option == CURLOPT_TIMEOUT        ||
	    option == CURLOPT_LOW_SPEED_LIMIT || option == CURLOPT_LOW_SPEED_TIME ||
	    option == CURLOPT_RESUME_FROM     || option == CURLOPT_SSLVERSION     ||
	    option == CURLOPT_TIMECONDITION   || option == CURLOPT_TIMEVALUE) {

		convert_to_long_ex (uCurlValue);
		ret = curl_easy_setopt (cp, option, Z_LVAL_PP (uCurlValue));

	} else if (option == CURLOPT_URL           || option == CURLOPT_PROXY        ||
	           option == CURLOPT_USERPWD       || option == CURLOPT_PROXYUSERPWD ||
	           option == CURLOPT_RANGE         || option == CURLOPT_POSTFIELDS   ||
	           option == CURLOPT_USERAGENT     || option == CURLOPT_FTPPORT      ||
	           option == CURLOPT_COOKIE        || option == CURLOPT_SSLCERT      ||
	           option == CURLOPT_SSLCERTPASSWD || option == CURLOPT_COOKIEFILE   ||
	           option == CURLOPT_CUSTOMREQUEST) {

		convert_to_string_ex (uCurlValue);
		ret = curl_easy_setopt (cp, option, Z_STRVAL_PP (uCurlValue));

	} else if (option == CURLOPT_FILE        || option == CURLOPT_INFILE ||
	           option == CURLOPT_WRITEHEADER || option == CURLOPT_STDERR) {

		FILE *fp;
		ZEND_FETCH_RESOURCE (fp, FILE *, uCurlValue, -1, "File-handle", php_file_le_fopen ());
		ret = curl_easy_setopt (cp, option, fp);

	}
	
	if (ret == CURLE_OK) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool curl_exec (int ch)
   Perform a CURL session */
PHP_FUNCTION (curl_exec)
{
	zval **uCurlId;
	CURL *cp;
	CURLcode ret;
	
	if (ZEND_NUM_ARGS() != 1 ||
	    zend_get_parameters_ex (1, &uCurlId) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE (cp, CURL *, uCurlId, -1, "CURL Handle", le_curl);
	ret = curl_easy_perform (cp);

	if (ret == CURLE_OK) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto void curl_close (int ch)
   Close a CURL session */
PHP_FUNCTION (curl_close)
{
	zval **uCurlId;
	CURL *cp;
	
	if (ZEND_NUM_ARGS() != 1 ||
	    zend_get_parameters_ex (1, &uCurlId) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	ZEND_FETCH_RESOURCE (cp, CURL *, uCurlId, -1, "CURL Handle", le_curl);
	zend_list_delete (Z_LVAL_PP (uCurlId));
}
/* }}} */

#endif