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

#ifndef _PHP_CURL_H
#define _PHP_CURL_H

#ifdef COMPILE_DL_CURL
#undef HAVE_CURL
#define HAVE_CURL 1
#endif

#if HAVE_CURL

extern zend_module_entry curl_module_entry;
#define curl_module_ptr &curl_module_entry

PHP_MINIT_FUNCTION(curl);
PHP_MSHUTDOWN_FUNCTION(curl);
PHP_MINFO_FUNCTION(curl);
PHP_FUNCTION(curl_version);
PHP_FUNCTION(curl_init);
PHP_FUNCTION(curl_setopt);
PHP_FUNCTION(curl_exec);
PHP_FUNCTION(curl_close);

#define CE_OK 0
#define CE_UNSUPPORTED_PROTOCOL 1
#define CE_FAILED_INIT 2
#define CE_URL_MALFORMAT 3
#define CE_URL_MALFORMAT_USER 4
#define CE_COULDNT_RESOLVE_PROXY 5
#define CE_COULDNT_RESOLVE_HOST 6
#define CE_COULDNT_CONNECT 7
#define CE_FTP_WEIRD_SERVER_REPLY 8
#define CE_FTP_ACCESS_DENIED 9
#define CE_FTP_USER_PASSWORD_INCORRECT 10
#define CE_FTP_WEIRD_PASS_REPLY 11
#define CE_FTP_WEIRD_USER_REPLY 12
#define CE_FTP_WEIRD_PASV_REPLY 13
#define CE_FTP_WEIRD_227_FORMAT 14
#define CE_FTP_CANT_GET_HOST 15
#define CE_FTP_CANT_RECONNECT 16
#define CE_FTP_COULDNT_SET_BINARY 17
#define CE_PARTIAL_FILE 18
#define CE_FTP_COULDNT_RETR_FILE 19
#define CE_FTP_WRITE_ERROR 20
#define CE_FTP_QUOTE_ERROR 21
#define CE_HTTP_NOT_FOUND 22
#define CE_WRITE_ERROR 23
#define CE_MALFORMAT_USER 24
#define CE_FTP_COULDNT_STOR_FILE 25
#define CE_READ_ERROR 26
#define CE_OUT_OF_MEMORY 27
#define CE_OPERATION_TIMEOUTED 28
#define CE_FTP_COULDNT_SET_ASCII 29
#define CE_FTP_PORT_FAILED 30
#define CE_FTP_COULDNT_USE_REST 31
#define CE_FTP_COULDNT_GET_SIZE 32
#define CE_HTTP_RANGE_ERROR 33
#define CE_HTTP_POST_ERROR 34
#define CE_SSL_CONNECT_ERROR 35
#define CE_FTP_BAD_DOWNLOAD_RESUME 36
#define CE_FILE_COULDNT_READ_FILE 37
#define CE_LDAP_CANNOT_BIND 38
#define CE_LDAP_SEARCH_FAILED 39
#define CE_LIBRARY_NOT_FOUND 40
#define CE_FUNCTION_NOT_FOUND 41
#define CE_ABORTED_BY_CALLBACK 42
#define CE_BAD_FUNCTION_ARGUMENT 43
#define CE_BAD_CALLING_ORDER 44
#define C_LAST 45

struct curl_file_id_table {
	int id;
	struct curl_file_id_table *next;
};

typedef struct {
	int use_file;
	int le_curl;
	struct curl_file_id_table *output_node, output_start;
} php_curl_globals;

#ifdef ZTS
#define CURLG(v) (curl_globals->v)
#define CURLLS_FETCH() php_curl_globals *curl_globals = ts_resource(gd_curl_id)
#else
#define CURLG(v) (curl_globals.v)
#define CURLLS_FETCH()
#endif


#else
#define curl_module_ptr NULL
#endif /* HAVE_CURL */
#define phpext_curl_ptr curl_module_ptr
#endif  /* _PHP_CURL_H */
