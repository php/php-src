/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
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

#define ZEND_INCLUDE_FULL_WINDOWS_HEADERS

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#if HAVE_CURL

#include <stdio.h>
#include <string.h>

#ifdef PHP_WIN32
#include <winsock2.h>
#include <sys/types.h>
#endif

#include <curl/curl.h>
#include <curl/easy.h>

#define SMART_STR_PREALLOC 4096

#include "ext/standard/php_smart_str.h"
#include "ext/standard/info.h"
#include "ext/standard/file.h"
#include "php_curl.h"

static void _php_curl_close(zend_rsrc_list_entry *rsrc TSRMLS_DC);

#define SAVE_CURL_ERROR(__handle, __err) (__handle)->err.no = (int) __err;

#define CAAL(s, v) add_assoc_long_ex(return_value, s, sizeof(s), (long) v);
#define CAAD(s, v) add_assoc_double_ex(return_value, s, sizeof(s), (double) v);
#define CAAS(s, v) add_assoc_string_ex(return_value, s, sizeof(s), (char *) v, 1);
#define CAAZ(s, v) add_assoc_zval_ex(return_value, s, sizeof(s), (zval *) v);

/* {{{ curl_functions[]
 */
function_entry curl_functions[] = {
	PHP_FE(curl_init,                NULL)
	PHP_FE(curl_version,             NULL)
	PHP_FE(curl_setopt,              NULL)
	PHP_FE(curl_exec,                NULL)
	PHP_FE(curl_getinfo,             NULL)
	PHP_FE(curl_error,               NULL)
	PHP_FE(curl_errno,               NULL)
	PHP_FE(curl_close,               NULL)
	PHP_FE(curl_multi_init,          NULL)
	PHP_FE(curl_multi_add_handle,    NULL)
	PHP_FE(curl_multi_remove_handle, NULL)
	PHP_FE(curl_multi_select,        NULL)
	PHP_FE(curl_multi_exec,          second_arg_force_ref)
	PHP_FE(curl_multi_getcontent,    NULL)
	PHP_FE(curl_multi_info_read,     NULL)
	PHP_FE(curl_multi_close,         NULL)
	{NULL, NULL, NULL}
};
/* }}} */

/* {{{ curl_module_entry
 */
zend_module_entry curl_module_entry = {
	STANDARD_MODULE_HEADER,
	"curl",
	curl_functions,
	PHP_MINIT(curl),
	PHP_MSHUTDOWN(curl),
	NULL,
	NULL,
	PHP_MINFO(curl),
	NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_CURL
ZEND_GET_MODULE (curl)
#endif

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(curl)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "CURL support",    "enabled");
	php_info_print_table_row(2, "CURL Information", curl_version());
	php_info_print_table_end();
}
/* }}} */

#define REGISTER_CURL_CONSTANT(__c) REGISTER_LONG_CONSTANT(#__c, __c, CONST_CS | CONST_PERSISTENT)

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(curl)
{
	le_curl = zend_register_list_destructors_ex(_php_curl_close, NULL, "curl", module_number);
	le_curl_multi_handle = zend_register_list_destructors_ex(_php_curl_multi_close, NULL, "curl", module_number);
	
	/* Constants for curl_setopt() */
	REGISTER_CURL_CONSTANT(CURLOPT_DNS_USE_GLOBAL_CACHE);
	REGISTER_CURL_CONSTANT(CURLOPT_DNS_CACHE_TIMEOUT);
	REGISTER_CURL_CONSTANT(CURLOPT_PORT);
	REGISTER_CURL_CONSTANT(CURLOPT_FILE);
	REGISTER_CURL_CONSTANT(CURLOPT_INFILE);
	REGISTER_CURL_CONSTANT(CURLOPT_INFILESIZE);
	REGISTER_CURL_CONSTANT(CURLOPT_URL);
	REGISTER_CURL_CONSTANT(CURLOPT_PROXY);
	REGISTER_CURL_CONSTANT(CURLOPT_VERBOSE);
	REGISTER_CURL_CONSTANT(CURLOPT_HEADER);
	REGISTER_CURL_CONSTANT(CURLOPT_HTTPHEADER);
	REGISTER_CURL_CONSTANT(CURLOPT_NOPROGRESS);
	REGISTER_CURL_CONSTANT(CURLOPT_NOBODY);
	REGISTER_CURL_CONSTANT(CURLOPT_FAILONERROR);
	REGISTER_CURL_CONSTANT(CURLOPT_UPLOAD);
	REGISTER_CURL_CONSTANT(CURLOPT_POST);
	REGISTER_CURL_CONSTANT(CURLOPT_FTPLISTONLY);
	REGISTER_CURL_CONSTANT(CURLOPT_FTPAPPEND);
	REGISTER_CURL_CONSTANT(CURLOPT_NETRC);
	REGISTER_CURL_CONSTANT(CURLOPT_FOLLOWLOCATION);
	REGISTER_CURL_CONSTANT(CURLOPT_FTPASCII);
	REGISTER_CURL_CONSTANT(CURLOPT_PUT);
	REGISTER_CURL_CONSTANT(CURLOPT_MUTE);
	REGISTER_CURL_CONSTANT(CURLOPT_USERPWD);
	REGISTER_CURL_CONSTANT(CURLOPT_PROXYUSERPWD);
	REGISTER_CURL_CONSTANT(CURLOPT_RANGE);
	REGISTER_CURL_CONSTANT(CURLOPT_TIMEOUT);
	REGISTER_CURL_CONSTANT(CURLOPT_POSTFIELDS);
	REGISTER_CURL_CONSTANT(CURLOPT_REFERER);
	REGISTER_CURL_CONSTANT(CURLOPT_USERAGENT);
	REGISTER_CURL_CONSTANT(CURLOPT_FTPPORT);
	REGISTER_CURL_CONSTANT(CURLOPT_FTP_USE_EPSV);
	REGISTER_CURL_CONSTANT(CURLOPT_LOW_SPEED_LIMIT);
	REGISTER_CURL_CONSTANT(CURLOPT_LOW_SPEED_TIME);
	REGISTER_CURL_CONSTANT(CURLOPT_RESUME_FROM);
	REGISTER_CURL_CONSTANT(CURLOPT_COOKIE);
	REGISTER_CURL_CONSTANT(CURLOPT_SSLCERT);
	REGISTER_CURL_CONSTANT(CURLOPT_SSLCERTPASSWD);
	REGISTER_CURL_CONSTANT(CURLOPT_WRITEHEADER);
	REGISTER_CURL_CONSTANT(CURLOPT_SSL_VERIFYHOST);
	REGISTER_CURL_CONSTANT(CURLOPT_COOKIEFILE);
	REGISTER_CURL_CONSTANT(CURLOPT_SSLVERSION);
	REGISTER_CURL_CONSTANT(CURLOPT_TIMECONDITION);
	REGISTER_CURL_CONSTANT(CURLOPT_TIMEVALUE);
	REGISTER_CURL_CONSTANT(CURLOPT_CUSTOMREQUEST);
	REGISTER_CURL_CONSTANT(CURLOPT_STDERR);
	REGISTER_CURL_CONSTANT(CURLOPT_TRANSFERTEXT);
	REGISTER_CURL_CONSTANT(CURLOPT_RETURNTRANSFER);
	REGISTER_CURL_CONSTANT(CURLOPT_QUOTE);
	REGISTER_CURL_CONSTANT(CURLOPT_POSTQUOTE);
	REGISTER_CURL_CONSTANT(CURLOPT_INTERFACE);
	REGISTER_CURL_CONSTANT(CURLOPT_KRB4LEVEL);
	REGISTER_CURL_CONSTANT(CURLOPT_HTTPPROXYTUNNEL);
	REGISTER_CURL_CONSTANT(CURLOPT_FILETIME);
	REGISTER_CURL_CONSTANT(CURLOPT_WRITEFUNCTION);
	REGISTER_CURL_CONSTANT(CURLOPT_READFUNCTION);
	REGISTER_CURL_CONSTANT(CURLOPT_PASSWDFUNCTION);
	REGISTER_CURL_CONSTANT(CURLOPT_HEADERFUNCTION);
	REGISTER_CURL_CONSTANT(CURLOPT_MAXREDIRS);
	REGISTER_CURL_CONSTANT(CURLOPT_MAXCONNECTS);
	REGISTER_CURL_CONSTANT(CURLOPT_CLOSEPOLICY);
	REGISTER_CURL_CONSTANT(CURLOPT_FRESH_CONNECT);
	REGISTER_CURL_CONSTANT(CURLOPT_FORBID_REUSE);
	REGISTER_CURL_CONSTANT(CURLOPT_RANDOM_FILE);
	REGISTER_CURL_CONSTANT(CURLOPT_EGDSOCKET);
	REGISTER_CURL_CONSTANT(CURLOPT_CONNECTTIMEOUT);
	REGISTER_CURL_CONSTANT(CURLOPT_SSL_VERIFYPEER);
	REGISTER_CURL_CONSTANT(CURLOPT_CAINFO);
	REGISTER_CURL_CONSTANT(CURLOPT_CAPATH);
	REGISTER_CURL_CONSTANT(CURLOPT_COOKIEJAR);
	REGISTER_CURL_CONSTANT(CURLOPT_SSL_CIPHER_LIST);
	REGISTER_CURL_CONSTANT(CURLOPT_BINARYTRANSFER);
	REGISTER_CURL_CONSTANT(CURLOPT_NOSIGNAL);
	REGISTER_CURL_CONSTANT(CURLOPT_PROXYTYPE);
	REGISTER_CURL_CONSTANT(CURLOPT_BUFFERSIZE);
	REGISTER_CURL_CONSTANT(CURLOPT_HTTPGET);
	REGISTER_CURL_CONSTANT(CURLOPT_HTTP_VERSION);
	REGISTER_CURL_CONSTANT(CURLOPT_SSLKEY);
	REGISTER_CURL_CONSTANT(CURLOPT_SSLKEYTYPE);
	REGISTER_CURL_CONSTANT(CURLOPT_SSLKEYPASSWD);
	REGISTER_CURL_CONSTANT(CURLOPT_SSLENGINE);
	REGISTER_CURL_CONSTANT(CURLOPT_SSLENGINE_DEFAULT);
	REGISTER_CURL_CONSTANT(CURLOPT_CRLF);
	REGISTER_CURL_CONSTANT(CURLOPT_ENCODING);
	
	/* Constants effecting the way CURLOPT_CLOSEPOLICY works */
	REGISTER_CURL_CONSTANT(CURLCLOSEPOLICY_LEAST_RECENTLY_USED);
	REGISTER_CURL_CONSTANT(CURLCLOSEPOLICY_LEAST_TRAFFIC);
	REGISTER_CURL_CONSTANT(CURLCLOSEPOLICY_SLOWEST);
	REGISTER_CURL_CONSTANT(CURLCLOSEPOLICY_CALLBACK);
	REGISTER_CURL_CONSTANT(CURLCLOSEPOLICY_OLDEST);

	/* Info constants */
	REGISTER_CURL_CONSTANT(CURLINFO_EFFECTIVE_URL);
	REGISTER_CURL_CONSTANT(CURLINFO_HTTP_CODE);
	REGISTER_CURL_CONSTANT(CURLINFO_HEADER_SIZE);
	REGISTER_CURL_CONSTANT(CURLINFO_REQUEST_SIZE);
	REGISTER_CURL_CONSTANT(CURLINFO_TOTAL_TIME);
	REGISTER_CURL_CONSTANT(CURLINFO_NAMELOOKUP_TIME);
	REGISTER_CURL_CONSTANT(CURLINFO_CONNECT_TIME);
	REGISTER_CURL_CONSTANT(CURLINFO_PRETRANSFER_TIME);
	REGISTER_CURL_CONSTANT(CURLINFO_SIZE_UPLOAD);
	REGISTER_CURL_CONSTANT(CURLINFO_SIZE_DOWNLOAD);
	REGISTER_CURL_CONSTANT(CURLINFO_SPEED_DOWNLOAD);
	REGISTER_CURL_CONSTANT(CURLINFO_SPEED_UPLOAD);
	REGISTER_CURL_CONSTANT(CURLINFO_FILETIME);
	REGISTER_CURL_CONSTANT(CURLINFO_SSL_VERIFYRESULT);
	REGISTER_CURL_CONSTANT(CURLINFO_CONTENT_LENGTH_DOWNLOAD);
	REGISTER_CURL_CONSTANT(CURLINFO_CONTENT_LENGTH_UPLOAD);
	REGISTER_CURL_CONSTANT(CURLINFO_STARTTRANSFER_TIME);
	REGISTER_CURL_CONSTANT(CURLINFO_CONTENT_TYPE);
	REGISTER_CURL_CONSTANT(CURLINFO_REDIRECT_TIME);
	REGISTER_CURL_CONSTANT(CURLINFO_REDIRECT_COUNT);

	/* cURL protocol constants (curl_version) */
	REGISTER_CURL_CONSTANT(CURL_VERSION_IPV6);
	REGISTER_CURL_CONSTANT(CURL_VERSION_KERBEROS4);
	REGISTER_CURL_CONSTANT(CURL_VERSION_SSL);
	REGISTER_CURL_CONSTANT(CURL_VERSION_LIBZ);
	
	/* version constants */
	REGISTER_CURL_CONSTANT(CURLVERSION_NOW);

	/* Error Constants */
	REGISTER_CURL_CONSTANT(CURLE_OK);
	REGISTER_CURL_CONSTANT(CURLE_UNSUPPORTED_PROTOCOL);
	REGISTER_CURL_CONSTANT(CURLE_FAILED_INIT);
	REGISTER_CURL_CONSTANT(CURLE_URL_MALFORMAT);
	REGISTER_CURL_CONSTANT(CURLE_URL_MALFORMAT_USER);
	REGISTER_CURL_CONSTANT(CURLE_COULDNT_RESOLVE_PROXY);
	REGISTER_CURL_CONSTANT(CURLE_COULDNT_RESOLVE_HOST);
	REGISTER_CURL_CONSTANT(CURLE_COULDNT_CONNECT);
	REGISTER_CURL_CONSTANT(CURLE_FTP_WEIRD_SERVER_REPLY);
	REGISTER_CURL_CONSTANT(CURLE_FTP_ACCESS_DENIED);
	REGISTER_CURL_CONSTANT(CURLE_FTP_USER_PASSWORD_INCORRECT);
	REGISTER_CURL_CONSTANT(CURLE_FTP_WEIRD_PASS_REPLY);
	REGISTER_CURL_CONSTANT(CURLE_FTP_WEIRD_USER_REPLY);
	REGISTER_CURL_CONSTANT(CURLE_FTP_WEIRD_PASV_REPLY);
	REGISTER_CURL_CONSTANT(CURLE_FTP_WEIRD_227_FORMAT);
	REGISTER_CURL_CONSTANT(CURLE_FTP_CANT_GET_HOST);
	REGISTER_CURL_CONSTANT(CURLE_FTP_CANT_RECONNECT);
	REGISTER_CURL_CONSTANT(CURLE_FTP_COULDNT_SET_BINARY);
	REGISTER_CURL_CONSTANT(CURLE_PARTIAL_FILE);
	REGISTER_CURL_CONSTANT(CURLE_FTP_COULDNT_RETR_FILE);
	REGISTER_CURL_CONSTANT(CURLE_FTP_WRITE_ERROR);
	REGISTER_CURL_CONSTANT(CURLE_FTP_QUOTE_ERROR);
	REGISTER_CURL_CONSTANT(CURLE_HTTP_NOT_FOUND);
	REGISTER_CURL_CONSTANT(CURLE_WRITE_ERROR);
	REGISTER_CURL_CONSTANT(CURLE_MALFORMAT_USER);
	REGISTER_CURL_CONSTANT(CURLE_FTP_COULDNT_STOR_FILE);
	REGISTER_CURL_CONSTANT(CURLE_READ_ERROR);
	REGISTER_CURL_CONSTANT(CURLE_OUT_OF_MEMORY);
	REGISTER_CURL_CONSTANT(CURLE_OPERATION_TIMEOUTED);
	REGISTER_CURL_CONSTANT(CURLE_FTP_COULDNT_SET_ASCII);
	REGISTER_CURL_CONSTANT(CURLE_FTP_PORT_FAILED);
	REGISTER_CURL_CONSTANT(CURLE_FTP_COULDNT_USE_REST);
	REGISTER_CURL_CONSTANT(CURLE_FTP_COULDNT_GET_SIZE);
	REGISTER_CURL_CONSTANT(CURLE_HTTP_RANGE_ERROR);
	REGISTER_CURL_CONSTANT(CURLE_HTTP_POST_ERROR);
	REGISTER_CURL_CONSTANT(CURLE_SSL_CONNECT_ERROR);
	REGISTER_CURL_CONSTANT(CURLE_FTP_BAD_DOWNLOAD_RESUME);
	REGISTER_CURL_CONSTANT(CURLE_FILE_COULDNT_READ_FILE);
	REGISTER_CURL_CONSTANT(CURLE_LDAP_CANNOT_BIND);
	REGISTER_CURL_CONSTANT(CURLE_LDAP_SEARCH_FAILED);
	REGISTER_CURL_CONSTANT(CURLE_LIBRARY_NOT_FOUND);
	REGISTER_CURL_CONSTANT(CURLE_FUNCTION_NOT_FOUND);
	REGISTER_CURL_CONSTANT(CURLE_ABORTED_BY_CALLBACK);
	REGISTER_CURL_CONSTANT(CURLE_BAD_FUNCTION_ARGUMENT);
	REGISTER_CURL_CONSTANT(CURLE_BAD_CALLING_ORDER);
	REGISTER_CURL_CONSTANT(CURLE_HTTP_PORT_FAILED);
	REGISTER_CURL_CONSTANT(CURLE_BAD_PASSWORD_ENTERED);
	REGISTER_CURL_CONSTANT(CURLE_TOO_MANY_REDIRECTS);
	REGISTER_CURL_CONSTANT(CURLE_UNKNOWN_TELNET_OPTION);
	REGISTER_CURL_CONSTANT(CURLE_TELNET_OPTION_SYNTAX);
	REGISTER_CURL_CONSTANT(CURLE_OBSOLETE);
	REGISTER_CURL_CONSTANT(CURLE_SSL_PEER_CERTIFICATE);

	REGISTER_CURL_CONSTANT(CURLPROXY_HTTP);
	REGISTER_CURL_CONSTANT(CURLPROXY_SOCKS5);

	REGISTER_CURL_CONSTANT(CURL_NETRC_OPTIONAL);
	REGISTER_CURL_CONSTANT(CURL_NETRC_IGNORED);
	REGISTER_CURL_CONSTANT(CURL_NETRC_REQUIRED);

	REGISTER_CURL_CONSTANT(CURL_HTTP_VERSION_NONE);
	REGISTER_CURL_CONSTANT(CURL_HTTP_VERSION_1_0);
	REGISTER_CURL_CONSTANT(CURL_HTTP_VERSION_1_1);
	
	REGISTER_CURL_CONSTANT(CURLM_CALL_MULTI_PERFORM);
	REGISTER_CURL_CONSTANT(CURLM_OK);
	REGISTER_CURL_CONSTANT(CURLM_BAD_HANDLE);
	REGISTER_CURL_CONSTANT(CURLM_BAD_EASY_HANDLE);
	REGISTER_CURL_CONSTANT(CURLM_OUT_OF_MEMORY);
	REGISTER_CURL_CONSTANT(CURLM_INTERNAL_ERROR);

	REGISTER_CURL_CONSTANT(CURLMSG_DONE);
	
	if (curl_global_init(CURL_GLOBAL_SSL) != CURLE_OK) {
		return FAILURE;
	}

#ifdef PHP_CURL_URL_WRAPPERS
# if HAVE_CURL_VERSION_INFO
	{
		curl_version_info_data *info = curl_version_info(CURLVERSION_NOW);
		char **p = (char **)info->protocols;

		while (*p != NULL) {
			php_register_url_stream_wrapper(*p++, &php_curl_wrapper TSRMLS_CC);
		}
	}
# else
	php_register_url_stream_wrapper("http", &php_curl_wrapper TSRMLS_CC);
	php_register_url_stream_wrapper("https", &php_curl_wrapper TSRMLS_CC);
	php_register_url_stream_wrapper("ftp", &php_curl_wrapper TSRMLS_CC);
	php_register_url_stream_wrapper("ldap", &php_curl_wrapper TSRMLS_CC);
# endif
#endif
	
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(curl)
{
#ifdef PHP_CURL_URL_WRAPPERS
	php_unregister_url_stream_wrapper("http" TSRMLS_CC);
	php_unregister_url_stream_wrapper("https" TSRMLS_CC);
	php_unregister_url_stream_wrapper("ftp" TSRMLS_CC);
	php_unregister_url_stream_wrapper("ldap" TSRMLS_CC);
#endif
	curl_global_cleanup();

	return SUCCESS;
}
/* }}} */

/* {{{ curl_write
 */
static size_t curl_write(char *data, size_t size, size_t nmemb, void *ctx)
{
	php_curl       *ch     = (php_curl *) ctx;
	php_curl_write *t      = ch->handlers->write;
	size_t          length = size * nmemb;
	TSRMLS_FETCH();

#if PHP_CURL_DEBUG
	fprintf(stderr, "curl_write() called\n");
	fprintf(stderr, "data = %s, size = %d, nmemb = %d, ctx = %x\n", 
			data, size, nmemb, ctx);
#endif
	
	switch (t->method) {
	case PHP_CURL_STDOUT:
		PHPWRITE(data, size);
		break;
	case PHP_CURL_FILE:
		return fwrite(data, size, nmemb, t->fp);
	case PHP_CURL_RETURN:
		smart_str_appendl(&t->buf, data, (int) length);
		break;
	case PHP_CURL_USER: {
		zval *argv[2];
		zval *retval;
		int   error;
		TSRMLS_FETCH();

		MAKE_STD_ZVAL(argv[0]);
		MAKE_STD_ZVAL(argv[1]);
		MAKE_STD_ZVAL(retval);

		ZVAL_RESOURCE(argv[0], ch->id);
		zend_list_addref(ch->id);
		ZVAL_STRINGL(argv[1], data, length, 1);

		error = call_user_function(EG(function_table),
		                           NULL,
		                           t->func,
		                           retval, 2, argv TSRMLS_CC);
		if (error == FAILURE) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Couldn't call the CURLOPT_WRITEFUNCTION");
			length = -1;
		}
		else {
			length = Z_LVAL_P(retval);
		}

		zval_ptr_dtor(&argv[0]);
		zval_ptr_dtor(&argv[1]);
		zval_ptr_dtor(&retval);

		break;
	}
	}

	return length;
}
/* }}} */

/* {{{ curl_read
 */
static size_t curl_read(char *data, size_t size, size_t nmemb, void *ctx)
{
	php_curl       *ch = (php_curl *) ctx;
	php_curl_read  *t  = ch->handlers->read;
	int             length = -1;

	switch (t->method) {
	case PHP_CURL_DIRECT:
		length = fread(data, size, nmemb, t->fp);
		break;
	case PHP_CURL_USER: {
		zval *argv[3];
		zval *retval;
		int   length;
		int   error;
		TSRMLS_FETCH();

		MAKE_STD_ZVAL(argv[0]);
		MAKE_STD_ZVAL(argv[1]);
		MAKE_STD_ZVAL(argv[2]);
		MAKE_STD_ZVAL(retval);

		ZVAL_RESOURCE(argv[0], ch->id);
		zend_list_addref(ch->id);
		ZVAL_RESOURCE(argv[1], t->fd);
		zend_list_addref(t->fd);
		ZVAL_LONG(argv[2], (int) size * nmemb);

		error = call_user_function(EG(function_table),
		                           NULL,
		                           t->func,
		                           retval, 3, argv TSRMLS_CC);
		if (error == FAILURE) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot call the CURLOPT_READFUNCTION"); 
			length = -1;
		}
		else {
			memcpy(data, Z_STRVAL_P(retval), size * nmemb);
			length = Z_STRLEN_P(retval);
		}

		zval_ptr_dtor(&argv[0]);
		zval_ptr_dtor(&argv[1]);
		zval_ptr_dtor(&argv[2]);
		zval_ptr_dtor(&retval);
		break;
	}
	}

	return length;
}
/* }}} */

/* {{{ curl_write_header
 */
static size_t curl_write_header(char *data, size_t size, size_t nmemb, void *ctx)
{
	php_curl       *ch  = (php_curl *) ctx;
	php_curl_write *t   = ch->handlers->write_header;
	size_t          length = size * nmemb;
	TSRMLS_FETCH();
	
	switch (t->method) {
		case PHP_CURL_STDOUT:
			/* Handle special case write when we're returning the entire transfer
			 */
			if (ch->handlers->write->method == PHP_CURL_RETURN)
				smart_str_appendl(&ch->handlers->write->buf, data, (int) length);
			else
				PHPWRITE(data, size);
			break;
		case PHP_CURL_FILE:
			return fwrite(data, size, nmemb, t->fp);
		case PHP_CURL_USER: {
			zval *argv[2];
			zval *retval;
			int   error;
			TSRMLS_FETCH();

			MAKE_STD_ZVAL(argv[0]);
			MAKE_STD_ZVAL(argv[1]);
			MAKE_STD_ZVAL(retval);

			ZVAL_RESOURCE(argv[0], ch->id);
			zend_list_addref(ch->id);
			ZVAL_STRINGL(argv[1], data, length, 1);

			error = call_user_function(EG(function_table),
									   NULL,
									   t->func,
									   retval, 2, argv TSRMLS_CC);
			if (error == FAILURE) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Couldn't call the CURLOPT_HEADERFUNCTION");
				length = -1;
			}
			else {
				length = Z_LVAL_P(retval);
			}
			zval_ptr_dtor(&argv[0]);
			zval_ptr_dtor(&argv[1]);
			zval_ptr_dtor(&retval);
			break;
		}
		case PHP_CURL_IGNORE:
			return length;
	}
	return length;
}
/* }}} */

/* {{{ curl_passwd
 */
static size_t curl_passwd(void *ctx, char *prompt, char *buf, int buflen)
{
	php_curl    *ch   = (php_curl *) ctx;
	zval        *func = ch->handlers->passwd;
	zval        *argv[3];
	zval        *retval = NULL;
	int          error;
	int          ret = 0;
	TSRMLS_FETCH();

	MAKE_STD_ZVAL(argv[0]);
	MAKE_STD_ZVAL(argv[1]);
	MAKE_STD_ZVAL(argv[2]);

	ZVAL_RESOURCE(argv[0], ch->id);
	zend_list_addref(ch->id);
	ZVAL_STRING(argv[1], prompt, 1);
	ZVAL_LONG(argv[2], buflen);

	error = call_user_function(EG(function_table),
	                           NULL,
	                           func,
	                           retval, 2, argv TSRMLS_CC);
	if (error == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Couldn't call the CURLOPT_PASSWDFUNCTION");
		ret = -1;
	}
	else {
		if (Z_STRLEN_P(retval) > buflen) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Returned password is too long for libcurl to handle");
			ret = -1;
		}
		else {
			strlcpy(buf, Z_STRVAL_P(retval), buflen);
		}
	}
	
	zval_ptr_dtor(&argv[0]);
	zval_ptr_dtor(&argv[1]);
	zval_ptr_dtor(&argv[2]);
	zval_ptr_dtor(&retval);

	return ret;
}
/* }}} */

/* {{{ curl_free_string
 */
static void curl_free_string(void **string)
{
	efree(*string);
}	
/* }}} */

/* {{{ curl_free_post
 */
static void curl_free_post(void **post)
{
	curl_formfree((struct HttpPost *) *post);
}
/* }}} */

/* {{{ curl_free_slist
 */
static void curl_free_slist(void **slist)
{
	curl_slist_free_all((struct curl_slist *) *slist);
}
/* }}} */


/* {{{ proto array curl_version([int version])
   Return cURL version information. */
PHP_FUNCTION(curl_version)
{
	curl_version_info_data *d;
	long                    uversion = CURLVERSION_NOW;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &uversion) == FAILURE) {
		return;
	}

	d = curl_version_info(uversion);
	if (d == NULL) {
		RETURN_FALSE;
	}

	array_init(return_value);

	CAAL("version_number", d->version_num);
	CAAL("age", d->age);
	CAAL("features", d->features);
	CAAL("ssl_version_number", d->ssl_version_num);
	CAAS("version", d->version);
	CAAS("host", d->host);
	CAAS("ssl_version", d->ssl_version);
	CAAS("libz_version", d->libz_version);
	/* Add an array of protocols */
	{
		char **p = (char **) d->protocols;
		zval  *protocol_list = NULL;

		MAKE_STD_ZVAL(protocol_list);
		array_init(protocol_list);

		while (*p != NULL) {
			add_next_index_string(protocol_list,  *p++, 1);
		}
		CAAZ("protocols", protocol_list);
	}
}
/* }}} */

/* {{{ alloc_curl_handle
 */
static void alloc_curl_handle(php_curl **ch)
{
	*ch                    = emalloc(sizeof(php_curl));
	(*ch)->handlers        = ecalloc(1, sizeof(php_curl_handlers));
	(*ch)->handlers->write = ecalloc(1, sizeof(php_curl_write));
	(*ch)->handlers->write_header = ecalloc(1, sizeof(php_curl_write));
	(*ch)->handlers->read  = ecalloc(1, sizeof(php_curl_read));
	memset(&(*ch)->err, 0, sizeof((*ch)->err));
	
	zend_llist_init(&(*ch)->to_free.str, sizeof(char *), 
	                (void(*)(void *)) curl_free_string, 0);
	zend_llist_init(&(*ch)->to_free.slist, sizeof(struct curl_slist),
	                (void(*)(void *)) curl_free_slist, 0);
	zend_llist_init(&(*ch)->to_free.post, sizeof(struct HttpPost),
	                (void(*)(void *)) curl_free_post, 0);
}
/* }}} */

/* {{{ proto resource curl_init([string url])
   Initialize a CURL session */
PHP_FUNCTION(curl_init)
{
	zval       **url;
	php_curl    *ch;
	int          argc = ZEND_NUM_ARGS();

	if (argc < 0 || argc > 1 ||
	    zend_get_parameters_ex(argc, &url) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	alloc_curl_handle(&ch);

	ch->cp = curl_easy_init();
	if (! ch->cp) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot initialize a new cURL handle");
		RETURN_FALSE;
	}

	ch->handlers->write->method = PHP_CURL_STDOUT;
	ch->handlers->write->type   = PHP_CURL_ASCII;
	ch->handlers->read->method  = PHP_CURL_DIRECT;
	ch->handlers->write_header->method = PHP_CURL_IGNORE;

	curl_easy_setopt(ch->cp, CURLOPT_NOPROGRESS,        1);
	curl_easy_setopt(ch->cp, CURLOPT_VERBOSE,           0);
	curl_easy_setopt(ch->cp, CURLOPT_ERRORBUFFER,       ch->err.str);
	curl_easy_setopt(ch->cp, CURLOPT_WRITEFUNCTION,     curl_write);
	curl_easy_setopt(ch->cp, CURLOPT_FILE,              (void *) ch);
	curl_easy_setopt(ch->cp, CURLOPT_READFUNCTION,      curl_read);
	curl_easy_setopt(ch->cp, CURLOPT_INFILE,            (void *) ch);
	curl_easy_setopt(ch->cp, CURLOPT_HEADERFUNCTION,    curl_write_header);
	curl_easy_setopt(ch->cp, CURLOPT_WRITEHEADER,       (void *) ch);
	curl_easy_setopt(ch->cp, CURLOPT_DNS_USE_GLOBAL_CACHE, 1);
	curl_easy_setopt(ch->cp, CURLOPT_DNS_CACHE_TIMEOUT, 120);

	if (argc > 0) {
		char *urlcopy;
		convert_to_string_ex(url);

		urlcopy = estrndup(Z_STRVAL_PP(url), Z_STRLEN_PP(url));
		curl_easy_setopt(ch->cp, CURLOPT_URL, urlcopy);
		zend_llist_add_element(&ch->to_free.str, &urlcopy);
	}

	ZEND_REGISTER_RESOURCE(return_value, ch, le_curl);
	ch->id = Z_LVAL_P(return_value);
}
/* }}} */

/* {{{ proto bool curl_setopt(resource ch, string option, mixed value)
   Set an option for a CURL transfer */
PHP_FUNCTION(curl_setopt)
{
	zval       **zid, 
		**zoption, 
		**zvalue;
	php_curl    *ch;
	CURLcode     error=CURLE_OK;
	int          option;

	if (ZEND_NUM_ARGS() != 3 ||
	    zend_get_parameters_ex(3, &zid, &zoption, &zvalue) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	ZEND_FETCH_RESOURCE(ch, php_curl *, zid, -1, le_curl_name, le_curl);
	convert_to_long_ex(zoption);

	option = Z_LVAL_PP(zoption);
	switch (option) {
		case CURLOPT_INFILESIZE:
		case CURLOPT_VERBOSE:
		case CURLOPT_HEADER:
		case CURLOPT_NOPROGRESS:
		case CURLOPT_NOBODY:
		case CURLOPT_FAILONERROR:
		case CURLOPT_UPLOAD:
		case CURLOPT_POST:
		case CURLOPT_FTPLISTONLY:
		case CURLOPT_FTPAPPEND:
		case CURLOPT_NETRC:
		case CURLOPT_FOLLOWLOCATION:
		case CURLOPT_PUT:
		case CURLOPT_MUTE:
		case CURLOPT_TIMEOUT:
		case CURLOPT_FTP_USE_EPSV:
		case CURLOPT_LOW_SPEED_LIMIT:
		case CURLOPT_SSLVERSION:
		case CURLOPT_LOW_SPEED_TIME:
		case CURLOPT_RESUME_FROM:
		case CURLOPT_TIMEVALUE:
		case CURLOPT_TIMECONDITION:
		case CURLOPT_TRANSFERTEXT:
		case CURLOPT_HTTPPROXYTUNNEL:
		case CURLOPT_FILETIME:
		case CURLOPT_MAXREDIRS:
		case CURLOPT_MAXCONNECTS:
		case CURLOPT_CLOSEPOLICY:
		case CURLOPT_FRESH_CONNECT:
		case CURLOPT_FORBID_REUSE:
		case CURLOPT_CONNECTTIMEOUT:
		case CURLOPT_SSL_VERIFYHOST:
		case CURLOPT_SSL_VERIFYPEER:
		case CURLOPT_DNS_USE_GLOBAL_CACHE:
		case CURLOPT_NOSIGNAL:
		case CURLOPT_PROXYTYPE:
		case CURLOPT_BUFFERSIZE:
		case CURLOPT_HTTPGET:
		case CURLOPT_HTTP_VERSION:
		case CURLOPT_CRLF:
			convert_to_long_ex(zvalue);
			error = curl_easy_setopt(ch->cp, option, Z_LVAL_PP(zvalue));
			break;
		case CURLOPT_URL:
		case CURLOPT_PROXY:
		case CURLOPT_USERPWD:
		case CURLOPT_PROXYUSERPWD:
		case CURLOPT_RANGE:
		case CURLOPT_CUSTOMREQUEST:
		case CURLOPT_USERAGENT:
		case CURLOPT_FTPPORT:
		case CURLOPT_COOKIE:
		case CURLOPT_COOKIEFILE:
		case CURLOPT_REFERER:
		case CURLOPT_INTERFACE:
		case CURLOPT_KRB4LEVEL: 
		case CURLOPT_RANDOM_FILE:
		case CURLOPT_EGDSOCKET:
		case CURLOPT_CAINFO: 
		case CURLOPT_CAPATH:
		case CURLOPT_COOKIEJAR:
		case CURLOPT_SSL_CIPHER_LIST: 
		case CURLOPT_SSLKEY:
		case CURLOPT_SSLCERT:
		case CURLOPT_SSLKEYTYPE: 
		case CURLOPT_SSLKEYPASSWD: 
		case CURLOPT_SSLENGINE: 
		case CURLOPT_SSLENGINE_DEFAULT:
		case CURLOPT_ENCODING: {
			char *copystr = NULL;
	
			convert_to_string_ex(zvalue);
			copystr = estrndup(Z_STRVAL_PP(zvalue), Z_STRLEN_PP(zvalue));

			error = curl_easy_setopt(ch->cp, option, copystr);
			zend_llist_add_element(&ch->to_free.str, &copystr);

			break;
		}
		case CURLOPT_FILE:
		case CURLOPT_INFILE: 
		case CURLOPT_WRITEHEADER:
		case CURLOPT_STDERR: {
			FILE *fp = NULL;
			int type;
			void * what;
		
			what = zend_fetch_resource(zvalue TSRMLS_CC, -1, "File-Handle", &type, 1, php_file_le_stream());
			ZEND_VERIFY_RESOURCE(what);

			if (FAILURE == php_stream_cast((php_stream *) what, 
										   PHP_STREAM_AS_STDIO, 
										   (void *) &fp, 
										   REPORT_ERRORS)) {
				RETURN_FALSE;
			}

			if (!fp) {
				RETURN_FALSE;
			}

			error = CURLE_OK;
			switch (option) {
				case CURLOPT_FILE:
					ch->handlers->write->fp = fp;
					ch->handlers->write->method = PHP_CURL_FILE;
					break;
				case CURLOPT_WRITEHEADER:
					ch->handlers->write_header->fp = fp;
					ch->handlers->write_header->method = PHP_CURL_FILE;
					break;
				case CURLOPT_INFILE:
					zend_list_addref(Z_LVAL_PP(zvalue));
					ch->handlers->read->fp = fp;
					ch->handlers->read->fd = Z_LVAL_PP(zvalue);
					break;
				default:
					error = curl_easy_setopt(ch->cp, option, fp);
					break;
			}

			break;
		}
		case CURLOPT_RETURNTRANSFER:
			convert_to_long_ex(zvalue);

			if (Z_LVAL_PP(zvalue)) {
				ch->handlers->write->method = PHP_CURL_RETURN;
			}
			break;
		case CURLOPT_BINARYTRANSFER:
			convert_to_long_ex(zvalue);	

			if (Z_LVAL_PP(zvalue)) {
				ch->handlers->write->type = PHP_CURL_BINARY;
			}
			break;
		case CURLOPT_WRITEFUNCTION:
			if (ch->handlers->write->func) {
				zval_ptr_dtor(&ch->handlers->write->func);
			}
			zval_add_ref(zvalue);
			ch->handlers->write->func   = *zvalue;
			ch->handlers->write->method = PHP_CURL_USER;
			break;
		case CURLOPT_READFUNCTION:
			if (ch->handlers->read->func) {
				zval_ptr_dtor(&ch->handlers->read->func);
			}
			zval_add_ref(zvalue);
			ch->handlers->read->func   = *zvalue;
			ch->handlers->read->method = PHP_CURL_USER;
			break;
		case CURLOPT_HEADERFUNCTION:
			if (ch->handlers->write_header->func) {
				zval_ptr_dtor(&ch->handlers->write_header->func);
			}
			zval_add_ref(zvalue);
			ch->handlers->write_header->func   = *zvalue;
			ch->handlers->write_header->method = PHP_CURL_USER;
			break;
		case CURLOPT_PASSWDFUNCTION:
			if (ch->handlers->passwd) {
				zval_ptr_dtor(&ch->handlers->passwd);
			}
			zval_add_ref(zvalue);
			ch->handlers->passwd = *zvalue;
			error = curl_easy_setopt(ch->cp, CURLOPT_PASSWDFUNCTION, curl_passwd);
			error = curl_easy_setopt(ch->cp, CURLOPT_PASSWDDATA,     (void *) ch);
			break;
		case CURLOPT_POSTFIELDS:
			if (Z_TYPE_PP(zvalue) == IS_ARRAY || Z_TYPE_PP(zvalue) == IS_OBJECT) {
				zval            **current;
				HashTable        *postfields;
				struct HttpPost  *first = NULL;
				struct HttpPost  *last  = NULL;
				char             *postval;
				char             *string_key = NULL;
				ulong             num_key;
				uint              string_key_len;

				postfields = HASH_OF(*zvalue);
				if (! postfields) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Couldn't get HashTable in CURLOPT_POSTFIELDS"); 
					RETURN_FALSE;
				}

				for (zend_hash_internal_pointer_reset(postfields);
					 zend_hash_get_current_data(postfields, 
						 						(void **) &current) == SUCCESS;
					 zend_hash_move_forward(postfields)) {

					SEPARATE_ZVAL(current);
					convert_to_string_ex(current);

					zend_hash_get_current_key_ex(postfields, 
							&string_key, &string_key_len, &num_key, 0, NULL);
				
					postval = Z_STRVAL_PP(current);
					if (*postval == '@') {
						error = curl_formadd(&first, &last, 
											 CURLFORM_COPYNAME, string_key,
											 CURLFORM_NAMELENGTH, string_key_len - 1,
											 CURLFORM_FILE, ++postval, 
											 CURLFORM_END);
					}
					else {
						error = curl_formadd(&first, &last, 
											 CURLFORM_COPYNAME, string_key,
											 CURLFORM_NAMELENGTH, string_key_len - 1,
											 CURLFORM_PTRCONTENTS, postval, 
											 CURLFORM_CONTENTSLENGTH, Z_STRLEN_PP(current),
											 CURLFORM_END);
					}
				}

				SAVE_CURL_ERROR(ch, error);
				if (error != CURLE_OK) {
					RETURN_FALSE;
				}

				zend_llist_add_element(&ch->to_free.post, &first);
				error = curl_easy_setopt(ch->cp, CURLOPT_HTTPPOST, first);
			}
			else {
				char *post = NULL;

				convert_to_string_ex(zvalue);
				post = estrndup(Z_STRVAL_PP(zvalue), Z_STRLEN_PP(zvalue));
				zend_llist_add_element(&ch->to_free.str, &post);

				error = curl_easy_setopt(ch->cp, CURLOPT_POSTFIELDS, post);
				error = curl_easy_setopt(ch->cp, CURLOPT_POSTFIELDSIZE, Z_STRLEN_PP(zvalue));
			}

			break;
		case CURLOPT_HTTPHEADER: 
		case CURLOPT_QUOTE:
		case CURLOPT_POSTQUOTE: {
			zval              **current;
			HashTable          *ph;
			struct curl_slist  *slist = NULL;

			ph = HASH_OF(*zvalue);
			if (!ph) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "You must pass either an object or an array with the CURLOPT_HTTPHEADER, CURLOPT_QUOTE and CURLOPT_POSTQUOTE arguments");
				RETURN_FALSE;
			}

			for (zend_hash_internal_pointer_reset(ph);
				 zend_hash_get_current_data(ph, (void **) &current) == SUCCESS;
				 zend_hash_move_forward(ph)) {
				char *indiv = NULL;

				SEPARATE_ZVAL(current);
				convert_to_string_ex(current);

				indiv = estrndup(Z_STRVAL_PP(current), Z_STRLEN_PP(current) + 1);
				slist = curl_slist_append(slist, indiv);
				if (! slist) {
					efree(indiv);
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Couldn't build curl_slist"); 
					RETURN_FALSE;
				}
				zend_llist_add_element(&ch->to_free.str, &indiv);
			}
			zend_llist_add_element(&ch->to_free.slist, &slist);

			error = curl_easy_setopt(ch->cp, option, slist);

			break;
		}
	}

	SAVE_CURL_ERROR(ch, error);
	if (error != CURLE_OK) {
		RETURN_FALSE;
	} else {
		RETURN_TRUE;
	}
}
/* }}} */

/* {{{ _php_curl_cleanup_handle(ch) 
   Cleanup an execution phase */
void 
_php_curl_cleanup_handle(php_curl *ch)
{
	if (ch->uses < 1) {
		return;
	}

	if (ch->handlers->write->buf.len) {
		memset(&ch->handlers->write->buf, 0, sizeof(smart_str));
	}

	memset(ch->err.str, 0, CURL_ERROR_SIZE + 1);
	ch->err.no = 0;
}
/* }}} */

/* {{{ proto bool curl_exec(resource ch)
   Perform a CURL session */
PHP_FUNCTION(curl_exec)
{
	zval      **zid;
	php_curl   *ch;
	CURLcode    error;

	if (ZEND_NUM_ARGS() != 1 ||
	    zend_get_parameters_ex(1, &zid) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	ZEND_FETCH_RESOURCE(ch, php_curl *, zid, -1, le_curl_name, le_curl);

	_php_curl_cleanup_handle(ch);
	
	error = curl_easy_perform(ch->cp);
	SAVE_CURL_ERROR(ch, error);
	if (error != CURLE_OK) {
		if (ch->handlers->write->buf.len > 0) {
			smart_str_free(&ch->handlers->write->buf);
		}

		RETURN_FALSE;
	}

	ch->uses++;

	if (ch->handlers->write->method == PHP_CURL_RETURN && ch->handlers->write->buf.len > 0) {
		if (ch->handlers->write->type != PHP_CURL_BINARY) 
			smart_str_0(&ch->handlers->write->buf);
		RETURN_STRINGL(ch->handlers->write->buf.c, ch->handlers->write->buf.len, 0);
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string curl_getinfo(resource ch, int opt)
   Get information regarding a specific transfer */
PHP_FUNCTION(curl_getinfo)
{
	zval       **zid, 
	           **zoption;
	php_curl    *ch;
	int          option,
	             argc = ZEND_NUM_ARGS();

	if (argc < 1 || argc > 2 ||
	    zend_get_parameters_ex(argc, &zid, &zoption) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	ZEND_FETCH_RESOURCE(ch, php_curl *, zid, -1, le_curl_name, le_curl);

	if (argc < 2) {
		char   *s_code;
		long    l_code;
		double  d_code;

		array_init(return_value);

		curl_easy_getinfo(ch->cp, CURLINFO_EFFECTIVE_URL, &s_code);
		CAAS("url", s_code);
		curl_easy_getinfo(ch->cp, CURLINFO_CONTENT_TYPE, &s_code);
		CAAS("content_type", s_code);
		curl_easy_getinfo(ch->cp, CURLINFO_HTTP_CODE, &l_code);
		CAAL("http_code", l_code);
		curl_easy_getinfo(ch->cp, CURLINFO_HEADER_SIZE, &l_code);
		CAAL("header_size", l_code);
		curl_easy_getinfo(ch->cp, CURLINFO_REQUEST_SIZE, &l_code);
		CAAL("request_size", l_code);
		curl_easy_getinfo(ch->cp, CURLINFO_FILETIME, &l_code);
		CAAL("filetime", l_code);
		curl_easy_getinfo(ch->cp, CURLINFO_SSL_VERIFYRESULT, &l_code);
		CAAL("ssl_verify_result", l_code);
		curl_easy_getinfo(ch->cp, CURLINFO_REDIRECT_COUNT, &l_code);
		CAAL("redirect_count", l_code);
		curl_easy_getinfo(ch->cp, CURLINFO_TOTAL_TIME, &d_code);
		CAAD("total_time", d_code);
		curl_easy_getinfo(ch->cp, CURLINFO_NAMELOOKUP_TIME, &d_code);
		CAAD("namelookup_time", d_code);
		curl_easy_getinfo(ch->cp, CURLINFO_CONNECT_TIME, &d_code);
		CAAD("connect_time", d_code);
		curl_easy_getinfo(ch->cp, CURLINFO_PRETRANSFER_TIME, &d_code);
		CAAD("pretransfer_time", d_code);
		curl_easy_getinfo(ch->cp, CURLINFO_SIZE_UPLOAD, &d_code);
		CAAD("size_upload", d_code);
		curl_easy_getinfo(ch->cp, CURLINFO_SIZE_DOWNLOAD, &d_code);
		CAAD("size_download", d_code);
		curl_easy_getinfo(ch->cp, CURLINFO_SPEED_DOWNLOAD, &d_code);
		CAAD("speed_download", d_code);
		curl_easy_getinfo(ch->cp, CURLINFO_SPEED_UPLOAD, &d_code);
		CAAD("speed_upload", d_code);
		curl_easy_getinfo(ch->cp, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &d_code);
		CAAD("download_content_length", d_code);
		curl_easy_getinfo(ch->cp, CURLINFO_CONTENT_LENGTH_UPLOAD, &d_code);
		CAAD("upload_content_length", d_code);
		curl_easy_getinfo(ch->cp, CURLINFO_STARTTRANSFER_TIME, &d_code);
		CAAD("starttransfer_time", d_code);
		curl_easy_getinfo(ch->cp, CURLINFO_REDIRECT_TIME, &d_code);
		CAAD("redirect_time", d_code);
	} else {
		option = Z_LVAL_PP(zoption);
		switch (option) {
		case CURLINFO_EFFECTIVE_URL: 
		case CURLINFO_CONTENT_TYPE: {
			char *s_code;

			curl_easy_getinfo(ch->cp, option, &s_code);
			RETURN_STRING(s_code, 1);

			break;
		}
		case CURLINFO_HTTP_CODE: 
		case CURLINFO_HEADER_SIZE: 
		case CURLINFO_REQUEST_SIZE: 
		case CURLINFO_FILETIME: 
		case CURLINFO_SSL_VERIFYRESULT: 
		case CURLINFO_REDIRECT_COUNT: {
			long code;

			curl_easy_getinfo(ch->cp, option, &code);
			RETURN_LONG(code);
   
			break;
		}
		case CURLINFO_TOTAL_TIME: 
		case CURLINFO_NAMELOOKUP_TIME: 
		case CURLINFO_CONNECT_TIME:
		case CURLINFO_PRETRANSFER_TIME: 
		case CURLINFO_SIZE_UPLOAD: 
		case CURLINFO_SIZE_DOWNLOAD:
		case CURLINFO_SPEED_DOWNLOAD: 
		case CURLINFO_SPEED_UPLOAD: 
		case CURLINFO_CONTENT_LENGTH_DOWNLOAD:
		case CURLINFO_CONTENT_LENGTH_UPLOAD: 
		case CURLINFO_STARTTRANSFER_TIME:
		case CURLINFO_REDIRECT_TIME: {
			double code;
	
			curl_easy_getinfo(ch->cp, option, &code);
			RETURN_DOUBLE(code);

			break;
		}
		}
	}			
}
/* }}} */

/* {{{ proto string curl_error(resource ch)
   Return a string contain the last error for the current session */
PHP_FUNCTION(curl_error)
{
	zval      **zid;
	php_curl   *ch;
	
	if (ZEND_NUM_ARGS() != 1 ||
	    zend_get_parameters_ex(1, &zid) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	ZEND_FETCH_RESOURCE(ch, php_curl *, zid, -1, le_curl_name, le_curl);

	ch->err.str[CURL_ERROR_SIZE] = 0;
	RETURN_STRING(ch->err.str, 1);
}
/* }}} */

/* {{{ proto int curl_errno(resource ch)
   Return an integer containing the last error number */
PHP_FUNCTION(curl_errno)
{
	zval      **zid;
	php_curl   *ch;

	if (ZEND_NUM_ARGS() != 1 ||
	    zend_get_parameters_ex(1, &zid) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	ZEND_FETCH_RESOURCE(ch, php_curl *, zid, -1, le_curl_name, le_curl);

	RETURN_LONG(ch->err.no);
}
/* }}} */

/* {{{ proto void curl_close(resource ch)
   Close a CURL session */
PHP_FUNCTION(curl_close)
{
	zval      **zid;
	php_curl   *ch;

	if (ZEND_NUM_ARGS() != 1 ||
	    zend_get_parameters_ex(1, &zid) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	ZEND_FETCH_RESOURCE(ch, php_curl *, zid, -1, le_curl_name, le_curl);
	
	zend_list_delete(Z_LVAL_PP(zid));
}
/* }}} */

/* {{{ _php_curl_close()
   List destructor for curl handles */
static void _php_curl_close(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	php_curl *ch = (php_curl *) rsrc->ptr;

#if PHP_CURL_DEBUG
	fprintf(stderr, "DTOR CALLED, ch = %x\n", ch);
#endif
	
	curl_easy_cleanup(ch->cp);
	zend_llist_clean(&ch->to_free.str);
	zend_llist_clean(&ch->to_free.slist);
	zend_llist_clean(&ch->to_free.post);

	if (ch->handlers->write->func) 
		zval_ptr_dtor(&ch->handlers->write->func);
	if (ch->handlers->read->func)  
		zval_ptr_dtor(&ch->handlers->read->func);
	if (ch->handlers->write_header->func) 
		zval_ptr_dtor(&ch->handlers->write_header->func);
	if (ch->handlers->passwd) 
		zval_ptr_dtor(&ch->handlers->passwd);

	efree(ch->handlers->write);
	efree(ch->handlers->write_header);
	efree(ch->handlers->read);
	efree(ch->handlers);
	efree(ch);
}	
/* }}} */

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */
