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
   | Author: Sterling Hughes <sterling@php.net>                           |
   | Wez Furlong <wez@thebrainroom.com>                                   |
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

#include <curl/curl.h>


extern zend_module_entry curl_module_entry;
#define curl_module_ptr &curl_module_entry

#define CURLOPT_RETURNTRANSFER 19913
#define CURLOPT_BINARYTRANSFER 19914

PHP_MINIT_FUNCTION(curl);
PHP_MSHUTDOWN_FUNCTION(curl);
PHP_MINFO_FUNCTION(curl);
PHP_FUNCTION(curl_version);
PHP_FUNCTION(curl_init);
PHP_FUNCTION(curl_setopt);
PHP_FUNCTION(curl_exec);
PHP_FUNCTION(curl_getinfo);
PHP_FUNCTION(curl_error);
PHP_FUNCTION(curl_errno);
PHP_FUNCTION(curl_close);

typedef struct {
	zval         *func;
	FILE         *fp;
	smart_str     buf;
	int           method;
	int           type;
} php_curl_write;

typedef struct {
	zval         *func;
	FILE         *fp;
	long          fd;
	int           method;
} php_curl_read;

typedef struct {
	php_curl_write *write;
	php_curl_write *write_header;
	php_curl_read  *read;
	zval           *passwd;
} php_curl_handlers;

struct _php_curl_error  {
	char str[CURL_ERROR_SIZE + 1];
	int  no;
};

struct _php_curl_free {
	zend_llist str;
	zend_llist post;
	zend_llist slist;
};

typedef struct {
	CURL                    *cp;
	php_curl_handlers       *handlers;
	struct _php_curl_error   err;
	struct _php_curl_free    to_free;
	long                     id;
} php_curl;

/* streams support */

PHPAPI extern php_stream_ops php_curl_stream_ops;
#define PHP_STREAM_IS_CURL	&php_curl_stream_ops

PHPAPI php_stream *php_curl_stream_opener(php_stream_wrapper *wrapper, char *filename, char *mode,
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
