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
#ifdef PHP_WIN32
#include <winsock.h>
#include <sys/types.h>
#define fstat(handle, buff) _fstat(handle, buff)
#define stat _stat
#endif
#include <curl/curl.h>
#include <curl/easy.h>
#include <sys/stat.h>
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#include "ext/standard/info.h"
#include "php_curl.h"

#ifdef ZTS
int curl_globals_id;
#else
php_curl_globals curl_globals;
#endif

#ifdef PHP_WIN32
static void win32_cleanup();
static void win32_init();

static void win32_cleanup()
{
	WSACleanup();
}

static CURLcode win32_init()
{
	WORD wVersionRequested;  
	WSADATA wsaData; 
	int err; 
	wVersionRequested = MAKEWORD(1, 1); 
    
	err = WSAStartup(wVersionRequested, &wsaData); 
    
	if (err != 0) return CURLE_FAILED_INIT;

	if ( LOBYTE(wsaData.wVersion) != 1 || 
	     HIBYTE(wsaData.wVersion) != 1 ) { 
		WSACleanup(); 
		return CURLE_FAILED_INIT; 
	}
	return CURLE_OK;
}
#else
static CURLcode win32_init(void) { return CURLE_OK; }
#define win32_cleanup()
#endif


function_entry curl_functions[] = {
	PHP_FE(curl_init,     NULL)
	PHP_FE(curl_version,  NULL)
	PHP_FE(curl_setopt,   NULL)
	PHP_FE(curl_exec,     NULL)
	PHP_FE(curl_close,    NULL)
	{NULL, NULL, NULL}
};

zend_module_entry curl_module_entry = {
	"curl",
	curl_functions,
	PHP_MINIT(curl),
	PHP_MSHUTDOWN(curl),
	NULL,
	NULL,
	PHP_MINFO(curl),
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_CURL
ZEND_GET_MODULE (curl)
#endif

static void php_curl_close(php_curl *curl_handle);
static int php_curl_error_translator(CURLcode err);

PHP_MINFO_FUNCTION(curl)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "CURL support", "enabled");
	php_info_print_table_row(2, "CURL Information", curl_version());
	php_info_print_table_end();
}

PHP_MINIT_FUNCTION(curl)
{
	CURLLS_FETCH();
	CURLG(le_curl) = register_list_destructors(php_curl_close, NULL);
	
	/* Constants for curl_setopt() */
	REGISTER_LONG_CONSTANT("CURLOPT_PORT", CURLOPT_PORT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLOPT_FILE", CURLOPT_FILE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLOPT_INFILE", CURLOPT_INFILE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLOPT_INFILESIZE", CURLOPT_INFILESIZE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLOPT_URL", CURLOPT_URL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLOPT_PROXY", CURLOPT_PROXY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLOPT_VERBOSE", CURLOPT_VERBOSE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLOPT_HEADER", CURLOPT_HEADER, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLOPT_NOPROGRESS", CURLOPT_NOPROGRESS, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLOPT_NOBODY", CURLOPT_NOBODY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLOPT_FAILONERROR", CURLOPT_FAILONERROR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLOPT_UPLOAD", CURLOPT_UPLOAD, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLOPT_POST", CURLOPT_POST, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLOPT_FTPLISTONLY", CURLOPT_FTPLISTONLY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLOPT_FTPAPPEND", CURLOPT_FTPAPPEND, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLOPT_NETRC", CURLOPT_NETRC, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLOPT_FOLLOWLOCATION", CURLOPT_FOLLOWLOCATION, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLOPT_FTPASCII", CURLOPT_FTPASCII, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLOPT_PUT", CURLOPT_PUT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLOPT_MUTE", CURLOPT_MUTE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLOPT_USERPWD", CURLOPT_USERPWD, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLOPT_PROXYUSERPWD", CURLOPT_PROXYUSERPWD, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLOPT_RANGE", CURLOPT_RANGE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLOPT_TIMEOUT", CURLOPT_TIMEOUT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLOPT_POSTFIELDS", CURLOPT_POSTFIELDS, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLOPT_REFERER", CURLOPT_REFERER, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLOPT_USERAGENT", CURLOPT_USERAGENT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLOPT_FTPPORT", CURLOPT_FTPPORT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLOPT_LOW_SPEED_LIMIT", CURLOPT_LOW_SPEED_LIMIT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLOPT_LOW_SPEED_TIME", CURLOPT_LOW_SPEED_TIME, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLOPT_RESUME_FROM", CURLOPT_RESUME_FROM, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLOPT_COOKIE", CURLOPT_COOKIE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLOPT_SSLCERT", CURLOPT_SSLCERT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLOPT_SSLCERTPASSWD", CURLOPT_SSLCERTPASSWD, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLOPT_WRITEHEADER", CURLOPT_WRITEHEADER, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLOPT_COOKIEFILE", CURLOPT_COOKIEFILE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLOPT_SSLVERSION", CURLOPT_SSLVERSION, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLOPT_TIMECONDITION", CURLOPT_TIMECONDITION, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLOPT_TIMEVALUE", CURLOPT_TIMEVALUE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLOPT_CUSTOMREQUEST", CURLOPT_CUSTOMREQUEST, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLOPT_STDERR", CURLOPT_STDERR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLOPT_WRITEINFO", CURLOPT_WRITEINFO, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLOPT_TRANSFERTEXT", CURLOPT_TRANSFERTEXT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLOPT_RETURNTRANSFER", CURLOPT_RETURNTRANSFER, CONST_CS | CONST_PERSISTENT);
	
	/* Error Constants */
	REGISTER_LONG_CONSTANT("CURLE_OK", CE_OK, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_UNSUPPORTED_PROTOCOL", CE_UNSUPPORTED_PROTOCOL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_FAILED_INIT", CE_FAILED_INIT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_URL_MALFORMAT", CE_URL_MALFORMAT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_URL_MALFORMAT_USER", CE_URL_MALFORMAT_USER, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_COULDNT_RESOLVE_PROXY", CE_COULDNT_RESOLVE_PROXY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_COULDNT_RESOLVE_HOST", CE_COULDNT_RESOLVE_HOST, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_COULDNT_CONNECT", CE_COULDNT_CONNECT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_FTP_WEIRD_SERVER_REPLY", CE_FTP_WEIRD_SERVER_REPLY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_FTP_ACCESS_DENIED", CE_FTP_ACCESS_DENIED, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_FTP_USER_PASSWORD_INCORRECT", CE_FTP_USER_PASSWORD_INCORRECT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_FTP_WEIRD_PASS_REPLY", CE_FTP_WEIRD_PASS_REPLY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_FTP_WEIRD_USER_REPLY", CE_FTP_WEIRD_USER_REPLY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_FTP_WEIRD_PASV_REPLY", CE_FTP_WEIRD_PASV_REPLY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_FTP_WEIRD_227_FORMAT", CE_FTP_WEIRD_227_FORMAT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_FTP_CANT_GET_HOST", CE_FTP_CANT_GET_HOST, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_FTP_CANT_RECONNECT", CE_FTP_CANT_RECONNECT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_FTP_COULDNT_SET_BINARY", CE_FTP_COULDNT_SET_BINARY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_PARTIAL_FILE", CE_PARTIAL_FILE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_FTP_COULDNT_RETR_FILE", CE_FTP_COULDNT_RETR_FILE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_FTP_WRITE_ERROR", CE_FTP_WRITE_ERROR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_FTP_QUOTE_ERROR", CE_FTP_QUOTE_ERROR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_HTTP_NOT_FOUND", CE_HTTP_NOT_FOUND, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_WRITE_ERROR", CE_WRITE_ERROR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_MALFORMAT_USER", CE_MALFORMAT_USER, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_FTP_COULDNT_STOR_FILE", CE_FTP_COULDNT_STOR_FILE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_READ_ERROR", CE_READ_ERROR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_OUT_OF_MEMORY", CE_OUT_OF_MEMORY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_OPERATION_TIMEOUTED", CE_OPERATION_TIMEOUTED, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_FTP_COULDNT_SET_ASCII", CE_FTP_COULDNT_SET_ASCII, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_FTP_PORT_FAILED", CE_FTP_PORT_FAILED, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_FTP_COULDNT_USE_REST", CE_FTP_COULDNT_USE_REST, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_FTP_COULDNT_GET_SIZE", CE_FTP_COULDNT_GET_SIZE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_HTTP_RANGE_ERROR", CE_HTTP_RANGE_ERROR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_HTTP_POST_ERROR", CE_HTTP_POST_ERROR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_SSL_CONNECT_ERROR", CE_SSL_CONNECT_ERROR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_FTP_BAD_DOWNLOAD_RESUME", CE_FTP_BAD_DOWNLOAD_RESUME, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_FILE_COULDNT_READ_FILE", CE_FILE_COULDNT_READ_FILE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_LDAP_CANNOT_BIND", CE_LDAP_CANNOT_BIND, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_LDAP_SEARCH_FAILED", CE_LDAP_SEARCH_FAILED, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_LIBRARY_NOT_FOUND", CE_LIBRARY_NOT_FOUND, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_FUNCTION_NOT_FOUND", CE_FUNCTION_NOT_FOUND, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_ABORTED_BY_CALLBACK", CE_ABORTED_BY_CALLBACK, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_BAD_FUNCTION_ARGUMENT", CE_BAD_FUNCTION_ARGUMENT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_BAD_CALLING_ORDER", CE_BAD_CALLING_ORDER, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURL_LAST", C_LAST, CONST_CS | CONST_PERSISTENT);
	
	win32_init();

	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(curl)
{
	win32_cleanup();
}


/* {{{ proto string curl_version (void)
   Return the CURL version string. */
PHP_FUNCTION (curl_version)
{
	RETURN_STRING(curl_version(), 1);
}
/* }}} */

/* {{{ proto int curl_init ([string url])
   Initialize a CURL session */
PHP_FUNCTION(curl_init)
{
	zval **u_url;
	php_curl *curl_handle;
	int argcount = ZEND_NUM_ARGS();
	CURLLS_FETCH();
	
	if (argcount < 0 || argcount > 1 ||
	    zend_get_parameters_ex(argcount, &u_url) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	curl_handle->cp = curl_easy_init();
	
	if (!curl_handle->cp) {
		php_error(E_ERROR, "Cannot initialize CURL Handle");
		RETURN_FALSE;
	}
	
	if (argcount > 0) {
		convert_to_string_ex(u_url);
		curl_easy_setopt(curl_handle->cp, CURLOPT_URL, Z_STRVAL_PP(u_url));
	}
	
	curl_easy_setopt(curl_handle->cp, CURLOPT_NOPROGRESS, 1);
	curl_easy_setopt(curl_handle->cp, CURLOPT_VERBOSE,    0);
	
	curl_handle->output_file = 0;
	curl_handle->php_stdout  = 1;
	
	ZEND_REGISTER_RESOURCE(return_value, curl_handle, CURLG(le_curl));
}
/* }}} */

/* {{{ proto bool curl_setopt (int ch, string option, mixed value)
   Set an option for a CURL transfer */
PHP_FUNCTION(curl_setopt)
{
	zval **u_curl_id, **u_curl_option, **u_curl_value;
	php_curl *curl_handle;
	CURLcode ret;
	int option;
	CURLLS_FETCH();
	
	if (ZEND_NUM_ARGS() != 3 ||
	    zend_get_parameters_ex(3, &u_curl_id, &u_curl_option, &u_curl_value) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(curl_handle, php_curl *, u_curl_id, -1, "CURL Handle", CURLG(le_curl));
	
	convert_to_long_ex(u_curl_option);
	option = Z_LVAL_PP(u_curl_option);
	
	if (option == CURLOPT_INFILESIZE      || option == CURLOPT_VERBOSE         ||
	    option == CURLOPT_HEADER          || option == CURLOPT_NOPROGRESS      ||
	    option == CURLOPT_NOBODY          || option == CURLOPT_FAILONERROR     ||
	    option == CURLOPT_UPLOAD          || option == CURLOPT_POST            ||
	    option == CURLOPT_FTPLISTONLY     || option == CURLOPT_FTPAPPEND       ||
	    option == CURLOPT_NETRC           || option == CURLOPT_FOLLOWLOCATION  ||
	    option == CURLOPT_PUT             || option == CURLOPT_MUTE            || 
	    option == CURLOPT_TIMEOUT         || option == CURLOPT_LOW_SPEED_LIMIT || 
	    option == CURLOPT_LOW_SPEED_TIME  || option == CURLOPT_RESUME_FROM     ||
	    option == CURLOPT_SSLVERSION      || option == CURLOPT_TIMECONDITION   ||
	    option == CURLOPT_TIMEVALUE       || option == CURLOPT_TRANSFERTEXT) {

		convert_to_long_ex(u_curl_value);
		ret = curl_easy_setopt(curl_handle->cp, option, Z_LVAL_PP(u_curl_value));

	} else if (option == CURLOPT_URL           || option == CURLOPT_PROXY        ||
	           option == CURLOPT_USERPWD       || option == CURLOPT_PROXYUSERPWD ||
	           option == CURLOPT_RANGE         || option == CURLOPT_POSTFIELDS   ||
	           option == CURLOPT_USERAGENT     || option == CURLOPT_FTPPORT      ||
	           option == CURLOPT_COOKIE        || option == CURLOPT_SSLCERT      ||
	           option == CURLOPT_SSLCERTPASSWD || option == CURLOPT_COOKIEFILE   ||
	           option == CURLOPT_CUSTOMREQUEST) {

		convert_to_string_ex(u_curl_value);
		ret = curl_easy_setopt(curl_handle->cp, option, Z_STRVAL_PP(u_curl_value));

	} else if (option == CURLOPT_FILE        || option == CURLOPT_INFILE ||
	           option == CURLOPT_WRITEHEADER || option == CURLOPT_STDERR) {

		FILE *fp;
		ZEND_FETCH_RESOURCE(fp, FILE *, u_curl_value, -1, "File-handle", php_file_le_fopen());
		ret = curl_easy_setopt(curl_handle->cp, option, fp);
		
		if (option == CURLOPT_FILE) {
			curl_handle->output_file = Z_LVAL_PP(u_curl_value);
			curl_handle->php_stdout  = 0;
		}
		
	} else if (option == CURLOPT_RETURNTRANSFER) {
	
		convert_to_long_ex(u_curl_value);

		curl_handle->return_transfer = Z_LVAL_PP(u_curl_value);
		curl_handle->php_stdout      = 0;
	}

	RETURN_LONG(php_curl_error_translator(ret));
}
/* }}} */

/* {{{ proto bool curl_exec (int ch)
   Perform a CURL session */
PHP_FUNCTION(curl_exec)
{
	zval **u_curl_id;
	php_curl *curl_handle;
	CURLcode ret;

	FILE *fp;
	char buf[4096];
	int b;
	unsigned long pos = 0;

	CURLLS_FETCH();
	

	if (ZEND_NUM_ARGS() != 1 ||
	    zend_get_parameters_ex(1, &u_curl_id) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(curl_handle, php_curl *, u_curl_id, -1, "CURL Handle", CURLG(le_curl));
	
	if ((curl_handle->return_transfer &&
	    !curl_handle->output_file) || curl_handle->php_stdout) {
		
		if ((fp = tmpfile()) == NULL) {
			php_error(E_WARNING, "Cannot initialize temporary file to save output from curl_exec()");
			RETURN_FALSE;
		}
		
		curl_easy_setopt(curl_handle->cp, CURLOPT_FILE, fp);
	
	} else if (curl_handle->return_transfer &&
	           curl_handle->output_file) {
		
		zval **id;
	
		Z_LVAL_PP(id) = curl_handle->output_file;
		Z_TYPE_PP(id) = IS_RESOURCE;
		
		ZEND_FETCH_RESOURCE(fp, FILE *, id, -1, "File-Handle", php_file_le_fopen());

	}
	
	ret = curl_easy_perform(curl_handle->cp);
	
	if ((!curl_handle->return_transfer && !curl_handle->php_stdout) ||
	    (ret != CURLE_OK)) {
		RETURN_LONG(php_curl_error_translator(ret));
	}
	
	fseek(fp, 0, SEEK_SET);
		
	if (curl_handle->php_stdout) {
		
		while ((b = fread(buf, 1, sizeof(buf), fp)) > 0) {
			php_write(buf, b);
		}
		
	} else {
		
		char *ret_data;
		struct stat stat_sb;
		
		if (fstat(fileno(fp), &stat_sb)) {
			RETURN_FALSE;
		}
				
		ret_data = emalloc((stat_sb.st_size+1)*sizeof(char));
		
		while ((b = fread(buf, 1, sizeof(buf), fp)) > 0) {
			memcpy(&(ret_data[pos]), buf, b);
			pos += b;
		}
		ret_data[stat_sb.st_size - 1] = '\0';
		
		RETURN_STRINGL(ret_data, stat_sb.st_size, 0);
	
	}

}
/* }}} */

/* {{{ proto void curl_close (int ch)
   Close a CURL session */
PHP_FUNCTION (curl_close)
{
	zval **u_curl_id;
	php_curl *curl_handle;
	CURLLS_FETCH();
	
	if (ZEND_NUM_ARGS() != 1 ||
	    zend_get_parameters_ex(1, &u_curl_id) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(curl_handle, php_curl *, u_curl_id, -1, "CURL Handle", CURLG(le_curl));
	
	zend_list_delete(Z_LVAL_PP(u_curl_id));
}
/* }}} */


static int php_curl_error_translator(CURLcode err)
{
	switch (err)
	{
		case CURLE_OK:
			return(CE_OK);
		case CURLE_UNSUPPORTED_PROTOCOL:
			return(CE_UNSUPPORTED_PROTOCOL);
		case CURLE_FAILED_INIT:
			return(CE_FAILED_INIT);
		case CURLE_URL_MALFORMAT:
			return(CE_URL_MALFORMAT);
		case CURLE_URL_MALFORMAT_USER:
			return(CE_URL_MALFORMAT_USER);
		case CURLE_COULDNT_RESOLVE_PROXY:
			return(CE_COULDNT_RESOLVE_PROXY);
		case CURLE_COULDNT_RESOLVE_HOST:
			return(CE_COULDNT_RESOLVE_HOST);
		case CURLE_COULDNT_CONNECT:
			return(CE_COULDNT_CONNECT);
		case CURLE_FTP_WEIRD_SERVER_REPLY:
			return(CE_FTP_WEIRD_SERVER_REPLY);
		case CURLE_FTP_ACCESS_DENIED:
			return(CE_FTP_ACCESS_DENIED);
		case CURLE_FTP_USER_PASSWORD_INCORRECT:
			return(CE_FTP_USER_PASSWORD_INCORRECT);
		case CURLE_FTP_WEIRD_PASS_REPLY:
			return(CE_FTP_WEIRD_PASS_REPLY);
		case CURLE_FTP_WEIRD_USER_REPLY:
			return(CE_FTP_WEIRD_USER_REPLY);
		case CURLE_FTP_WEIRD_PASV_REPLY:
			return(CE_FTP_WEIRD_PASV_REPLY);
		case CURLE_FTP_WEIRD_227_FORMAT:
			return(CE_FTP_WEIRD_227_FORMAT);
		case CURLE_FTP_CANT_GET_HOST:
			return(CE_FTP_CANT_GET_HOST);
		case CURLE_FTP_CANT_RECONNECT:
			return(CE_FTP_CANT_RECONNECT);
		case CURLE_FTP_COULDNT_SET_BINARY:
			return(CE_FTP_COULDNT_SET_BINARY);
		case CURLE_PARTIAL_FILE:
			return(CE_PARTIAL_FILE);
		case CURLE_FTP_COULDNT_RETR_FILE:
			return(CE_FTP_COULDNT_RETR_FILE);
		case CURLE_FTP_WRITE_ERROR:
			return(CE_FTP_WRITE_ERROR);
		case CURLE_FTP_QUOTE_ERROR:
			return(CE_FTP_QUOTE_ERROR);
		case CURLE_HTTP_NOT_FOUND:
			return(CE_HTTP_NOT_FOUND);
		case CURLE_WRITE_ERROR:
			return(CE_WRITE_ERROR);
		case CURLE_MALFORMAT_USER:
			return(CE_MALFORMAT_USER);
		case CURLE_FTP_COULDNT_STOR_FILE:
			return(CE_FTP_COULDNT_STOR_FILE);
		case CURLE_READ_ERROR:
			return(CE_READ_ERROR);
		case CURLE_OUT_OF_MEMORY:
			return(CE_OUT_OF_MEMORY);
		case CURLE_OPERATION_TIMEOUTED:
			return(CE_OPERATION_TIMEOUTED);
		case CURLE_FTP_COULDNT_SET_ASCII:
			return(CE_FTP_COULDNT_SET_ASCII);
		case CURLE_FTP_PORT_FAILED:
			return(CE_FTP_PORT_FAILED);
		case CURLE_FTP_COULDNT_USE_REST:
			return(CE_FTP_COULDNT_USE_REST);
		case CURLE_FTP_COULDNT_GET_SIZE:
			return(CE_FTP_COULDNT_GET_SIZE);
		case CURLE_HTTP_RANGE_ERROR:
			return(CE_HTTP_RANGE_ERROR);
		case CURLE_HTTP_POST_ERROR:
			return(CE_HTTP_POST_ERROR);
		case CURLE_SSL_CONNECT_ERROR:
			return(CE_SSL_CONNECT_ERROR);
		case CURLE_FTP_BAD_DOWNLOAD_RESUME:
			return(CE_FTP_BAD_DOWNLOAD_RESUME);
		case CURLE_FILE_COULDNT_READ_FILE:
			return(CE_FILE_COULDNT_READ_FILE);
		case CURLE_LDAP_CANNOT_BIND:
			return(CE_LDAP_CANNOT_BIND);
		case CURLE_LDAP_SEARCH_FAILED:
			return(CE_LDAP_SEARCH_FAILED);
		case CURLE_LIBRARY_NOT_FOUND:
			return(CE_LIBRARY_NOT_FOUND);
		case CURLE_FUNCTION_NOT_FOUND:
			return(CE_FUNCTION_NOT_FOUND);
		case CURLE_ABORTED_BY_CALLBACK:
			return(CE_ABORTED_BY_CALLBACK);
		case CURLE_BAD_FUNCTION_ARGUMENT:
			return(CE_BAD_FUNCTION_ARGUMENT);
		case CURLE_BAD_CALLING_ORDER:
			return(CE_BAD_CALLING_ORDER);
		case CURL_LAST:
			return(C_LAST);
	}
}

static void php_curl_close(php_curl *curl_handle)
{
	curl_easy_cleanup(curl_handle->cp);
}

#endif
