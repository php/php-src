/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2012 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Sterling Hughes <sterling@php.net>                           |
   |         Wez Furlong <wez@thebrainroom.com>                           |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef _PHP_CURL_H
#define _PHP_CURL_H

#include "php.h"
#include "ext/standard/php_smart_str.h"

#ifdef COMPILE_DL_CURL
#undef HAVE_CURL
#define HAVE_CURL 1
#endif

#if HAVE_CURL

#define PHP_CURL_DEBUG 0

#include <curl/curl.h>
#include <curl/multi.h>

extern zend_module_entry curl_module_entry;
#define curl_module_ptr &curl_module_entry

#define CURLOPT_RETURNTRANSFER 19913
#define CURLOPT_BINARYTRANSFER 19914
#define PHP_CURL_STDOUT 0
#define PHP_CURL_FILE   1
#define PHP_CURL_USER   2
#define PHP_CURL_DIRECT 3
#define PHP_CURL_RETURN 4
#define PHP_CURL_ASCII  5
#define PHP_CURL_BINARY 6
#define PHP_CURL_IGNORE 7

extern int  le_curl;
#define le_curl_name "cURL handle"
extern int  le_curl_multi_handle;
#define le_curl_multi_handle_name "cURL Multi Handle"

PHP_MINIT_FUNCTION(curl);
PHP_MSHUTDOWN_FUNCTION(curl);
PHP_MINFO_FUNCTION(curl);
PHP_FUNCTION(curl_version);
PHP_FUNCTION(curl_init);
PHP_FUNCTION(curl_copy_handle);
PHP_FUNCTION(curl_setopt);
PHP_FUNCTION(curl_setopt_array);
PHP_FUNCTION(curl_exec);
PHP_FUNCTION(curl_getinfo);
PHP_FUNCTION(curl_error);
PHP_FUNCTION(curl_errno);
PHP_FUNCTION(curl_close);
PHP_FUNCTION(curl_multi_init);
PHP_FUNCTION(curl_multi_add_handle);
PHP_FUNCTION(curl_multi_remove_handle);
PHP_FUNCTION(curl_multi_select);
PHP_FUNCTION(curl_multi_exec);
PHP_FUNCTION(curl_multi_getcontent);
PHP_FUNCTION(curl_multi_info_read);
PHP_FUNCTION(curl_multi_close);
void _php_curl_multi_close(zend_rsrc_list_entry * TSRMLS_DC);

typedef struct {
	zval            *func_name;
	zend_fcall_info_cache fci_cache;
	FILE            *fp;
	smart_str       buf;
	int             method;
	int             type;
	zval		*stream;
} php_curl_write;

typedef struct {
	zval            *func_name;
	zend_fcall_info_cache fci_cache;
	FILE            *fp;
	long            fd;
	int             method;
	zval		*stream;
} php_curl_read;

typedef struct {
	zval 		*func_name;
	zend_fcall_info_cache fci_cache;
	int    	        method;
} php_curl_progress;

typedef struct {
	php_curl_write *write;
	php_curl_write *write_header;
	php_curl_read  *read;
	zval           *passwd;
	zval           *std_err;
	php_curl_progress *progress;
} php_curl_handlers;

struct _php_curl_error  {
	char str[CURL_ERROR_SIZE + 1];
	int  no;
};

struct _php_curl_send_headers {
	char *str;
	size_t str_len;
};

struct _php_curl_free {
	zend_llist str;
	zend_llist post;
	zend_llist slist;
};

typedef struct {
	struct _php_curl_error   err;
	struct _php_curl_free    *to_free;
	struct _php_curl_send_headers header;
	void ***thread_ctx;
	CURL                    *cp;
	php_curl_handlers       *handlers;
	long                     id;
	unsigned int             uses;
	zend_bool                in_callback;
	zval                     *clone;
} php_curl;

typedef struct {
	int    still_running;
	CURLM *multi;
	zend_llist easyh;
} php_curlm;

void _php_curl_cleanup_handle(php_curl *);
void _php_curl_multi_cleanup_list(void *data);
int  _php_curl_verify_handlers(php_curl *ch, int reporterror TSRMLS_DC);

/* streams support */

extern php_stream_ops php_curl_stream_ops;
#define PHP_STREAM_IS_CURL	&php_curl_stream_ops

php_stream *php_curl_stream_opener(php_stream_wrapper *wrapper, char *filename, char *mode,
		int options, char **opened_path, php_stream_context *context STREAMS_DC TSRMLS_DC);

extern php_stream_wrapper php_curl_wrapper;

struct php_curl_buffer {
	off_t readpos, writepos;
	php_stream *buf;
};

typedef struct {
	CURL	*curl;
	CURLM	*multi;
	char *url;
	struct php_curl_buffer readbuffer; /* holds downloaded data */
	struct php_curl_buffer writebuffer; /* holds data to upload */

	fd_set readfds, writefds, excfds;
	int maxfd;
	
	char errstr[CURL_ERROR_SIZE + 1];
	CURLMcode mcode;
	int pending;
	zval *headers;
} php_curl_stream;


#else
#define curl_module_ptr NULL
#endif /* HAVE_CURL */
#define phpext_curl_ptr curl_module_ptr
#endif  /* _PHP_CURL_H */
