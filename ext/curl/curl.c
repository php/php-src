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

/* Standard Includes */
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef PHP_WIN32
#include <winsock.h>
#include <sys/types.h>
#define fstat(handle, buff) _fstat(handle, buff)
#define stat _stat
#endif

/* CURL Includes */
#include <curl/curl.h>
#include <curl/easy.h>

/* PHP Includes */
#include "ext/standard/info.h"
#include "php_curl.h"

#ifdef ZTS
int curl_globals_id;
#else
php_curl_globals curl_globals;
#endif

static int le_curl;

static void _php_curl_close(zend_rsrc_list_entry *rsrc);
#define SAVE_CURL_ERROR(__handle, __err) \
	__handle->cerrno = (int)__err;

#ifdef PHP_WIN32
/* {{{ win32_cleanup()
   Clean-up allocated socket data on win32 systems */
static void win32_cleanup()
{
	WSACleanup();
}
/* }}} */

/* {{{ win32_init()
   Initialize WSA stuff on Win32 systems */
static CURLcode win32_init()
{
	WORD wVersionRequested;  
	WSADATA wsaData; 
	int err; 
	wVersionRequested = MAKEWORD(1, 1); 
    
	err = WSAStartup(wVersionRequested, &wsaData); 
    
	if (err != 0) return CURLE_FAILED_INIT;

	if (LOBYTE(wsaData.wVersion) != 1 || 
	    HIBYTE(wsaData.wVersion) != 1) { 
		WSACleanup(); 
		return CURLE_FAILED_INIT; 
	}
	return CURLE_OK;
}
/* }}} */
#else
static CURLcode win32_init(void) { return CURLE_OK; }
#define win32_cleanup()
#endif


function_entry curl_functions[] = {
	PHP_FE(curl_init,     NULL)
	PHP_FE(curl_version,  NULL)
	PHP_FE(curl_setopt,   NULL)
	PHP_FE(curl_exec,     NULL)
#if LIBCURL_VERSION_NUM >= 0x070401
	PHP_FE(curl_getinfo,  NULL)
#endif
	PHP_FE(curl_error,    NULL)
	PHP_FE(curl_errno,    NULL)
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

PHP_MINFO_FUNCTION(curl)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "CURL support", "enabled");
	php_info_print_table_row(2, "CURL Information", curl_version());
	php_info_print_table_end();
}

PHP_MINIT_FUNCTION(curl)
{
	le_curl = zend_register_list_destructors_ex(_php_curl_close, NULL, "curl", module_number);
	
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
	REGISTER_LONG_CONSTANT("CURLOPT_TRANSFERTEXT", CURLOPT_TRANSFERTEXT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLOPT_RETURNTRANSFER", CURLOPT_RETURNTRANSFER, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLOPT_QUOTE", CURLOPT_QUOTE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLOPT_POSTQUOTE", CURLOPT_POSTQUOTE, CONST_CS | CONST_PERSISTENT);
#if LIBCURL_VERSION_NUM >= 0x070300
	REGISTER_LONG_CONSTANT("CURLOPT_INTERFACE", CURLOPT_INTERFACE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLOPT_KRB4LEVEL", CURLOPT_KRB4LEVEL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLOPT_HTTPPROXYTUNNEL", CURLOPT_HTTPPROXYTUNNEL, CONST_CS | CONST_PERSISTENT);
#endif


#if LIBCURL_VERSION_NUM >= 0x070401
	/* Info constants */
	REGISTER_LONG_CONSTANT("CURLINFO_EFFECTIVE_URL", CURLINFO_EFFECTIVE_URL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLINFO_HTTP_CODE", CURLINFO_HTTP_CODE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLINFO_HEADER_SIZE", CURLINFO_HEADER_SIZE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLINFO_REQUEST_SIZE", CURLINFO_REQUEST_SIZE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLINFO_TOTAL_TIME", CURLINFO_TOTAL_TIME, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLINFO_NAMELOOKUP_TIME", CURLINFO_NAMELOOKUP_TIME, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLINFO_CONNECT_TIME", CURLINFO_CONNECT_TIME, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLINFO_PRETRANSFER_TIME", CURLINFO_PRETRANSFER_TIME, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLINFO_SIZE_UPLOAD", CURLINFO_SIZE_UPLOAD, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLINFO_SIZE_DOWNLOAD", CURLINFO_SIZE_DOWNLOAD, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLINFO_SPEED_DOWNLOAD", CURLINFO_SPEED_DOWNLOAD, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLINFO_SPEED_UPLOAD", CURLINFO_SPEED_UPLOAD, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLINFO_HEADER_SIZE", CURLINFO_HEADER_SIZE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLINFO_REQUEST_SIZE", CURLINFO_REQUEST_SIZE, CONST_CS | CONST_PERSISTENT);

#endif

	/* Error Constants */
	REGISTER_LONG_CONSTANT("CURLE_OK", (int)CURLE_OK, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_UNSUPPORTED_PROTOCOL", (int)CURLE_UNSUPPORTED_PROTOCOL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_FAILED_INIT", (int)CURLE_FAILED_INIT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_URL_MALFORMAT", (int)CURLE_URL_MALFORMAT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_URL_MALFORMAT_USER", (int)CURLE_URL_MALFORMAT_USER, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_COULDNT_RESOLVE_PROXY", (int)CURLE_COULDNT_RESOLVE_PROXY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_COULDNT_RESOLVE_HOST", (int)CURLE_COULDNT_RESOLVE_HOST, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_COULDNT_CONNECT", (int)CURLE_COULDNT_CONNECT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_FTP_WEIRD_SERVER_REPLY", (int)CURLE_FTP_WEIRD_SERVER_REPLY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_FTP_ACCESS_DENIED", (int)CURLE_FTP_ACCESS_DENIED, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_FTP_USER_PASSWORD_INCORRECT", (int)CURLE_FTP_USER_PASSWORD_INCORRECT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_FTP_WEIRD_PASS_REPLY", (int)CURLE_FTP_WEIRD_PASS_REPLY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_FTP_WEIRD_USER_REPLY", (int)CURLE_FTP_WEIRD_USER_REPLY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_FTP_WEIRD_PASV_REPLY", (int)CURLE_FTP_WEIRD_PASV_REPLY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_FTP_WEIRD_227_FORMAT", (int)CURLE_FTP_WEIRD_227_FORMAT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_FTP_CANT_GET_HOST", (int)CURLE_FTP_CANT_GET_HOST, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_FTP_CANT_RECONNECT", (int)CURLE_FTP_CANT_RECONNECT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_FTP_COULDNT_SET_BINARY", (int)CURLE_FTP_COULDNT_SET_BINARY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_PARTIAL_FILE", (int)CURLE_PARTIAL_FILE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_FTP_COULDNT_RETR_FILE", (int)CURLE_FTP_COULDNT_RETR_FILE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_FTP_WRITE_ERROR", (int)CURLE_FTP_WRITE_ERROR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_FTP_QUOTE_ERROR", (int)CURLE_FTP_QUOTE_ERROR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_HTTP_NOT_FOUND", (int)CURLE_HTTP_NOT_FOUND, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_WRITE_ERROR", (int)CURLE_WRITE_ERROR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_MALFORMAT_USER", (int)CURLE_MALFORMAT_USER, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_FTP_COULDNT_STOR_FILE", (int)CURLE_FTP_COULDNT_STOR_FILE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_READ_ERROR", (int)CURLE_READ_ERROR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_OUT_OF_MEMORY", (int)CURLE_OUT_OF_MEMORY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_OPERATION_TIMEOUTED", (int)CURLE_OPERATION_TIMEOUTED, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_FTP_COULDNT_SET_ASCII", (int)CURLE_FTP_COULDNT_SET_ASCII, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_FTP_PORT_FAILED", (int)CURLE_FTP_PORT_FAILED, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_FTP_COULDNT_USE_REST", (int)CURLE_FTP_COULDNT_USE_REST, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_FTP_COULDNT_GET_SIZE", (int)CURLE_FTP_COULDNT_GET_SIZE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_HTTP_RANGE_ERROR", (int)CURLE_HTTP_RANGE_ERROR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_HTTP_POST_ERROR", (int)CURLE_HTTP_POST_ERROR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_SSL_CONNECT_ERROR", (int)CURLE_SSL_CONNECT_ERROR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_FTP_BAD_DOWNLOAD_RESUME", (int)CURLE_FTP_BAD_DOWNLOAD_RESUME, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_FILE_COULDNT_READ_FILE", (int)CURLE_FILE_COULDNT_READ_FILE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_LDAP_CANNOT_BIND", (int)CURLE_LDAP_CANNOT_BIND, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_LDAP_SEARCH_FAILED", (int)CURLE_LDAP_SEARCH_FAILED, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_LIBRARY_NOT_FOUND", (int)CURLE_LIBRARY_NOT_FOUND, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_FUNCTION_NOT_FOUND", (int)CURLE_FUNCTION_NOT_FOUND, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_ABORTED_BY_CALLBACK", (int)CURLE_ABORTED_BY_CALLBACK, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_BAD_FUNCTION_ARGUMENT", (int)CURLE_BAD_FUNCTION_ARGUMENT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURLE_BAD_CALLING_ORDER", (int)CURLE_BAD_CALLING_ORDER, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CURL_LAST", (int)CURL_LAST, CONST_CS | CONST_PERSISTENT);
		
	if (win32_init() != CURLE_OK) {
		return FAILURE;
	}

	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(curl)
{
	win32_cleanup();
	return SUCCESS;
}


/* {{{ proto string curl_version(void)
   Return the CURL version string. */
PHP_FUNCTION(curl_version)
{
	RETURN_STRING(curl_version(), 1);
}
/* }}} */

/* {{{ proto int curl_init([string url])
   Initialize a CURL session */
PHP_FUNCTION(curl_init)
{
	zval **url;
	php_curl *curl_handle = NULL;
	int argc = ZEND_NUM_ARGS();
	
	if (argc < 0 || argc > 1 ||
	    zend_get_parameters_ex(argc, &url) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	curl_handle = (php_curl *)emalloc(sizeof(php_curl));
	if (!curl_handle) {
		php_error(E_WARNING, "Couldn't allocate a CURL Handle");
		RETURN_FALSE;
	}
	memset(curl_handle, 0, sizeof(php_curl));

	curl_handle->cp = curl_easy_init();
	if (!curl_handle->cp) {
		php_error(E_ERROR, "Cannot initialize CURL Handle");
		RETURN_FALSE;
	}
	
	if (argc > 0) {
		char *urlstr;
		convert_to_string_ex(url);

		urlstr = estrndup(Z_STRVAL_PP(url), Z_STRLEN_PP(url));
		curl_easy_setopt(curl_handle->cp, CURLOPT_URL, urlstr);
		efree(urlstr);
	}

	curl_easy_setopt(curl_handle->cp, CURLOPT_NOPROGRESS, 1);
	curl_easy_setopt(curl_handle->cp, CURLOPT_VERBOSE,    0);
	curl_easy_setopt(curl_handle->cp, CURLOPT_ERRORBUFFER, curl_handle->error);

	curl_handle->output_file = 0;
	curl_handle->php_stdout  = 1;

	ZEND_REGISTER_RESOURCE(return_value, curl_handle, le_curl);
}
/* }}} */

/* {{{ proto bool curl_setopt(int ch, string option, mixed value)
   Set an option for a CURL transfer */
PHP_FUNCTION(curl_setopt)
{
	zval **curl_id, 
	     **curl_option, 
	     **curl_value;
	php_curl *curl_handle;
	CURLcode ret;
	int option;
	
	if (ZEND_NUM_ARGS() != 3 ||
	    zend_get_parameters_ex(3, &curl_id, &curl_option, &curl_value) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(curl_handle, php_curl *, curl_id, -1, "CURL Handle", le_curl);
	
	convert_to_long_ex(curl_option);
	option = Z_LVAL_PP(curl_option);
	
	switch (option) {
		case CURLOPT_INFILESIZE:      case CURLOPT_VERBOSE:    case CURLOPT_HEADER:
		case CURLOPT_NOPROGRESS:      case CURLOPT_NOBODY:     case CURLOPT_FAILONERROR:
		case CURLOPT_UPLOAD:          case CURLOPT_POST:       case CURLOPT_FTPLISTONLY:
		case CURLOPT_FTPAPPEND:       case CURLOPT_NETRC:      case CURLOPT_FOLLOWLOCATION:
		case CURLOPT_PUT:             case CURLOPT_MUTE:       case CURLOPT_TIMEOUT:
		case CURLOPT_LOW_SPEED_LIMIT: case CURLOPT_SSLVERSION: case CURLOPT_LOW_SPEED_TIME:
		case CURLOPT_RESUME_FROM:     case CURLOPT_TIMEVALUE:  case CURLOPT_TIMECONDITION:
		case CURLOPT_TRANSFERTEXT:
#if LIBCURL_VERSION_NUM >= 0x070300
		case CURLOPT_HTTPPROXYTUNNEL:
#endif
		
			convert_to_long_ex(curl_value);
			ret = curl_easy_setopt(curl_handle->cp, option, Z_LVAL_PP(curl_value));
			break;
		
		case CURLOPT_URL:           case CURLOPT_PROXY:         case CURLOPT_USERPWD:
		case CURLOPT_PROXYUSERPWD:  case CURLOPT_RANGE:         case CURLOPT_CUSTOMREQUEST:
		case CURLOPT_USERAGENT:     case CURLOPT_FTPPORT:       case CURLOPT_COOKIE:
		case CURLOPT_SSLCERT:       case CURLOPT_SSLCERTPASSWD: case CURLOPT_COOKIEFILE:
		case CURLOPT_REFERER:
#if LIBCURL_VERSION_NUM >= 0x070300
		case CURLOPT_INTERFACE: case CURLOPT_KRB4LEVEL:
#endif
		
			{
				char *copystr = NULL;
				
				convert_to_string_ex(curl_value);
				copystr = estrndup(Z_STRVAL_PP(curl_value), Z_STRLEN_PP(curl_value));
				
				ret = curl_easy_setopt(curl_handle->cp, option, copystr);
			}
			break;
			
		case CURLOPT_FILE:   case CURLOPT_INFILE: case CURLOPT_WRITEHEADER:
		case CURLOPT_STDERR: 

			{
				FILE *fp;
			    
				ZEND_FETCH_RESOURCE(fp, FILE *, curl_value, -1, "File-Handle", php_file_le_fopen());
				ret = curl_easy_setopt(curl_handle->cp, option, fp);
			    
				if (option & CURLOPT_FILE) {
					curl_handle->output_file = Z_LVAL_PP(curl_value);
					curl_handle->php_stdout  = 0;
				}
			}
			break;
		
		case CURLOPT_RETURNTRANSFER:
			
			convert_to_long_ex(curl_value);

			curl_handle->return_transfer = Z_LVAL_PP(curl_value);
			curl_handle->php_stdout      = !Z_LVAL_PP(curl_value);
			break;
		
		case CURLOPT_POSTFIELDS:
		
			if (Z_TYPE_PP(curl_value) == IS_ARRAY ||
			    Z_TYPE_PP(curl_value) == IS_OBJECT) {
				
				zval **current;
				HashTable *u_post = HASH_OF(*curl_value);
				struct HttpPost *first = NULL, 
				                *last  = NULL;
				
				for (zend_hash_internal_pointer_reset(u_post);
				     zend_hash_get_current_data(u_post, (void **)&current) == SUCCESS;
				     zend_hash_move_forward(u_post)) {

					char *string_key = NULL, 
					     *str        = NULL, 
						 *val_str    = NULL;
					ulong num_key;
					
					SEPARATE_ZVAL(current);
					convert_to_string_ex(current);
					
					if (zend_hash_get_current_key(u_post, &string_key, &num_key) == HASH_KEY_IS_LONG) {
						php_error(E_WARNING, "Array passed to %s() must be an associative array", get_active_function_name());
						RETURN_FALSE;
					}
					
					val_str = estrndup(Z_STRVAL_PP(current), Z_STRLEN_PP(current));
					
					str = emalloc(strlen(string_key) + strlen(val_str) + 1 + 2);
					if (!str) {
						php_error(E_WARNING, "Couldn't allocate a post field from %s()", get_active_function_name());
						RETURN_FALSE;
					}
					sprintf(str, "%s=%s", string_key, val_str);

					ret = curl_formparse(str, &first, &last);
					
					efree(string_key);
					efree(val_str);
				}
				
				if (ret != CURLE_OK) {
					SAVE_CURL_ERROR(curl_handle, ret);
					RETURN_FALSE;
				}
				
				ret = curl_easy_setopt(curl_handle->cp, CURLOPT_HTTPPOST, first);
			} else {
				
				char *post_str = NULL;

				convert_to_string_ex(curl_value);
				post_str = estrndup(Z_STRVAL_PP(curl_value), Z_STRLEN_PP(curl_value));

				ret = curl_easy_setopt(curl_handle->cp, CURLOPT_POSTFIELDS, post_str);
#if LIBCURL_VERSION_NUM >= 0x070300
				ret = curl_easy_setopt(curl_handle->cp, CURLOPT_POSTFIELDSIZE, Z_STRLEN_PP(curl_value));
#endif
				if (ret != CURLE_OK) {
					SAVE_CURL_ERROR(curl_handle, ret);
					RETURN_FALSE;
				}
				
				ret = curl_easy_setopt(curl_handle->cp, CURLOPT_POSTFIELDSIZE, Z_STRLEN_PP(curl_value));
				break;
			
			}
			break;
		
		case CURLOPT_HTTPHEADER:
		
			{
				zval **current;
				HashTable *headers = HASH_OF(*curl_value);
				struct curl_slist *header = NULL;
				
				header = (struct curl_slist *)emalloc(sizeof(struct curl_slist));
				if (!header) {
					php_error(E_WARNING, "Couldn't allocate header list from %s()", get_active_function_name());
					RETURN_FALSE;
				}
				memset(header, 0, sizeof(struct curl_slist));
				
				for (zend_hash_internal_pointer_reset(headers);
				     zend_hash_get_current_data(headers, (void **)&current) == SUCCESS;
					 zend_hash_move_forward(headers)) {
					char *indiv_header = NULL;
				
					SEPARATE_ZVAL(current);
					convert_to_string_ex(current);
					
					indiv_header = estrndup(Z_STRVAL_PP(current), Z_STRLEN_PP(current));
					header = curl_slist_append(header, indiv_header);
					if (!header) {
						php_error(E_WARNING, "Couldn't build header from %s()", get_active_function_name());
						efree(indiv_header);
						RETURN_FALSE;
					}

				}
				
				ret = curl_easy_setopt(curl_handle->cp, CURLOPT_HTTPHEADER, header);
			}
			break;
		
		case CURLOPT_QUOTE: 
		case CURLOPT_POSTQUOTE:
		
			{
				zval **current;
				HashTable *php_commands = HASH_OF(*curl_value);
				struct curl_slist *commands = NULL;
				
				commands = (struct curl_slist *)emalloc(sizeof(struct curl_slist));
				if (!commands) {
					php_error(E_WARNING, "Couldn't allocate command list from %s()", get_active_function_name());
					RETURN_FALSE;
				}
				memset(commands, 0, sizeof(struct curl_slist));
				
				for (zend_hash_internal_pointer_reset(php_commands);
				     zend_hash_get_current_data(php_commands, (void **)&current) == SUCCESS;
					 zend_hash_move_forward(php_commands)) {
					char *indiv_command = NULL;
					
					SEPARATE_ZVAL(current);
					convert_to_string_ex(current);
					
					indiv_command = estrndup(Z_STRVAL_PP(current), Z_STRLEN_PP(current));
					commands = curl_slist_append(commands, indiv_command);
					if (!commands) {
						php_error(E_WARNING, "Couldn't build command list from %s()", get_active_function_name());
						efree(indiv_command);
						RETURN_FALSE;
					}
					
				}
				
				ret = curl_easy_setopt(curl_handle->cp, option, commands); 
			}
			break;
	}
	
	if (ret != CURLE_OK) {
		SAVE_CURL_ERROR(curl_handle, ret);
		RETURN_FALSE;
	} else {
		RETURN_TRUE;
	}
}
/* }}} */

/* {{{ proto bool curl_exec(int ch)
   Perform a CURL session */
PHP_FUNCTION(curl_exec)
{
	zval **curl_id;
	php_curl *curl_handle;
	CURLcode ret;
	FILE *fp;
	char buf[4096];
	int b;
	unsigned long pos = 0;
	
	if (ZEND_NUM_ARGS() != 1 ||
	    zend_get_parameters_ex(1, &curl_id) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(curl_handle, php_curl *, curl_id, -1, "CURL Handle", le_curl);
	
	if ((curl_handle->return_transfer &&
	    !curl_handle->output_file) || curl_handle->php_stdout) {
		
		if ((fp = tmpfile()) == NULL) {
			php_error(E_WARNING, "Cannot initialize temporary file to save output from %s()", get_active_function_name());
			RETURN_FALSE;
		}
		
		curl_easy_setopt(curl_handle->cp, CURLOPT_FILE, fp);
	
	} else if (curl_handle->return_transfer &&
	           curl_handle->output_file) {

		ZEND_FETCH_RESOURCE(fp, FILE *, (zval **)NULL, curl_handle->output_file, "File-Handle", php_file_le_fopen());

	}
	
	ret = curl_easy_perform(curl_handle->cp);
	
	if ((!curl_handle->return_transfer && !curl_handle->php_stdout) ||
	    (ret != CURLE_OK)) {
	
		if (ret != CURLE_OK) {
			SAVE_CURL_ERROR(curl_handle, ret);
			RETURN_FALSE;
		} else {
			RETURN_TRUE;
		}

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
				
		ret_data = emalloc(stat_sb.st_size+1);
		
		while ((b = fread(buf, 1, sizeof(buf), fp)) > 0) {
			memcpy(ret_data + pos, buf, b);
			pos += b;
		}
		ret_data[stat_sb.st_size - 1] = '\0';
		
		RETURN_STRINGL(ret_data, stat_sb.st_size, 0);
	
	}

}
/* }}} */

#if LIBCURL_VERSION_NUM >= 0x070401
/* {{{ proto string curl_getinfo(int ch, int opt)
   Get information regarding a specific transfer */
PHP_FUNCTION(curl_getinfo)
{
	zval **curl_id, **curl_opt;
	php_curl *curl_handle;
	int opt,
	    argc = ZEND_NUM_ARGS();
	
	if (argc < 1 || argc > 2 ||
	    zend_get_parameters_ex(argc, &curl_id, &curl_opt) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	ZEND_FETCH_RESOURCE(curl_handle, php_curl *, curl_id, -1, "CURL Handle", le_curl);

	if (argc < 2) {
		char url[250];
		long l_code;
		double d_code;

		array_init(return_value);

		curl_easy_getinfo(curl_handle->cp, CURLINFO_EFFECTIVE_URL, url);
		add_assoc_string(return_value, "url", url, 1);
		
		curl_easy_getinfo(curl_handle->cp, CURLINFO_HTTP_CODE, &l_code);
		add_assoc_long(return_value, "http_code", l_code);
		
		curl_easy_getinfo(curl_handle->cp, CURLINFO_HEADER_SIZE, &l_code);
		add_assoc_long(return_value, "header_size", l_code);
		
		curl_easy_getinfo(curl_handle->cp, CURLINFO_REQUEST_SIZE, &l_code);
		add_assoc_long(return_value, "request_size", l_code);
		
		curl_easy_getinfo(curl_handle->cp, CURLINFO_TOTAL_TIME, &d_code);
		add_assoc_double(return_value, "total_time", d_code);
		
		curl_easy_getinfo(curl_handle->cp, CURLINFO_NAMELOOKUP_TIME, &d_code);
		add_assoc_double(return_value, "namelookup_time", d_code);
		
		curl_easy_getinfo(curl_handle->cp, CURLINFO_CONNECT_TIME, &d_code);
		add_assoc_double(return_value, "connect_time", d_code);
		
		curl_easy_getinfo(curl_handle->cp, CURLINFO_PRETRANSFER_TIME, &d_code);
		add_assoc_double(return_value, "pretransfer_time", d_code);
		
		curl_easy_getinfo(curl_handle->cp, CURLINFO_SIZE_UPLOAD, &d_code);
		add_assoc_double(return_value, "size_upload", d_code);
		
		curl_easy_getinfo(curl_handle->cp, CURLINFO_SIZE_DOWNLOAD, &d_code);
		add_assoc_double(return_value, "size_download", d_code);
		
		curl_easy_getinfo(curl_handle->cp, CURLINFO_SPEED_DOWNLOAD, &d_code);
		add_assoc_double(return_value, "speed_download", d_code);
		
		curl_easy_getinfo(curl_handle->cp, CURLINFO_SPEED_UPLOAD, &d_code);
		add_assoc_double(return_value, "speed_upload", d_code);
	} else {
		opt = Z_LVAL_PP(curl_opt);
		switch (opt) {
			case CURLINFO_EFFECTIVE_URL:
				{
					char url[250];
	
					curl_easy_getinfo(curl_handle->cp, opt, url);
	
					RETURN_STRING(url, 1);
				}
				break;
			case CURLINFO_HTTP_CODE: case CURLINFO_HEADER_SIZE: case CURLINFO_REQUEST_SIZE:
				{
					long code;
					
					curl_easy_getinfo(curl_handle->cp, opt, &code);
					RETURN_LONG(code);
				}
				break;
			case CURLINFO_TOTAL_TIME: case CURLINFO_NAMELOOKUP_TIME: case CURLINFO_CONNECT_TIME:
			case CURLINFO_PRETRANSFER_TIME: case CURLINFO_SIZE_UPLOAD: case CURLINFO_SIZE_DOWNLOAD:
			case CURLINFO_SPEED_DOWNLOAD: case CURLINFO_SPEED_UPLOAD:
				{
					double code;
	
					curl_easy_getinfo(curl_handle->cp, opt, &code);
					RETURN_DOUBLE(code);
				}
				break;
		}
	}			
}
/* }}} */
#endif

/* {{{ proto string curl_error(int ch)
   Return a string contain the last error for the current session */
PHP_FUNCTION(curl_error)
{
	zval **curl_id;
	php_curl *curl_handle;
	
	if (ZEND_NUM_ARGS() != 1 ||
	    zend_get_parameters_ex(1, &curl_id) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(curl_handle, php_curl *, curl_id, -1, "CURL Handle", le_curl);
	RETURN_STRING(curl_handle->error, 1);
}
/* }}} */

/* {{{ proto int curl_errno(int ch)
   Return an integer containing the last error number */
PHP_FUNCTION(curl_errno)
{
	zval **curl_id;
	php_curl *curl_handle;
	
	if (ZEND_NUM_ARGS() != 1 ||
	    zend_get_parameters_ex(1, &curl_id) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(curl_handle, php_curl *, curl_id, -1, "CURL Handle", le_curl);
	RETURN_LONG(curl_handle->cerrno);
}
/* }}} */

/* {{{ proto void curl_close(int ch)
   Close a CURL session */
PHP_FUNCTION(curl_close)
{
	zval **curl_id;
	php_curl *curl_handle;
	
	if (ZEND_NUM_ARGS() != 1 ||
	    zend_get_parameters_ex(1, &curl_id) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(curl_handle, php_curl *, curl_id, -1, "CURL Handle", le_curl);
	
	zend_list_delete(Z_LVAL_PP(curl_id));
}
/* }}} */

/* {{{ _php_curl_close()
   List destructor for curl handles */
static void _php_curl_close(zend_rsrc_list_entry *rsrc)
{
	php_curl *curl_handle = (php_curl *)rsrc->ptr;
	curl_easy_cleanup(curl_handle->cp);
	efree(curl_handle);
}
/* }}} */

#endif
