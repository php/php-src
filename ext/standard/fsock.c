/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2013 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Paul Panotzki - Bunyip Information Systems                  |
   |          Jim Winstead <jimw@php.net>                                 |
   |          Sascha Schumann <sascha@schumann.cx>                        |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "php.h"
#include "php_globals.h"
#include <stdlib.h>
#include <stddef.h>
#include "php_network.h"
#include "file.h"

/* {{{ php_fsockopen() */

static void php_fsockopen_stream(INTERNAL_FUNCTION_PARAMETERS, int persistent)
{
	char *host;
	int host_len;
	long port = -1;
	zval *zerrno = NULL, *zerrstr = NULL;
	double timeout = FG(default_socket_timeout);
	unsigned long conv;
	struct timeval tv;
	char *hashkey = NULL;
	php_stream *stream = NULL;
	int err;
	char *hostname = NULL;
	long hostname_len;
	char *errstr = NULL;

	RETVAL_FALSE;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|lzzd", &host, &host_len, &port, &zerrno, &zerrstr, &timeout) == FAILURE) {
		RETURN_FALSE;
	}

	if (persistent) {
		spprintf(&hashkey, 0, "pfsockopen__%s:%ld", host, port);
	}

	if (port > 0) {
		hostname_len = spprintf(&hostname, 0, "%s:%ld", host, port);
	} else {
		hostname_len = host_len;
		hostname = host;
	}
	
	/* prepare the timeout value for use */
	conv = (unsigned long) (timeout * 1000000.0);
	tv.tv_sec = conv / 1000000;
	tv.tv_usec = conv % 1000000;

	if (zerrno)	{
		zval_dtor(zerrno);
		ZVAL_LONG(zerrno, 0);
	}
	if (zerrstr) {
		zval_dtor(zerrstr);
		ZVAL_STRING(zerrstr, "", 1);
	}

	stream = php_stream_xport_create(hostname, hostname_len, REPORT_ERRORS,
			STREAM_XPORT_CLIENT | STREAM_XPORT_CONNECT, hashkey, &tv, NULL, &errstr, &err);

	if (port > 0) {
		efree(hostname);
	}
	if (stream == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "unable to connect to %s:%ld (%s)", host, port, errstr == NULL ? "Unknown error" : errstr);
	}

	if (hashkey) {
		efree(hashkey);
	}
	
	if (stream == NULL)	{
		if (zerrno) {
			zval_dtor(zerrno);
			ZVAL_LONG(zerrno, err);
		}
		if (zerrstr && errstr) {
			/* no need to dup; we need to efree buf anyway */
			zval_dtor(zerrstr);
			ZVAL_STRING(zerrstr, errstr, 0);
		}
		else if (!zerrstr && errstr) {
			efree(errstr);
		} 

		RETURN_FALSE;
	}

	if (errstr) {
		efree(errstr);
	}
		
	php_stream_to_zval(stream, return_value);
}

/* }}} */

/* {{{ proto resource fsockopen(string hostname, int port [, int errno [, string errstr [, float timeout]]])
   Open Internet or Unix domain socket connection */
PHP_FUNCTION(fsockopen)
{
	php_fsockopen_stream(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */
/* {{{ proto resource pfsockopen(string hostname, int port [, int errno [, string errstr [, float timeout]]])
   Open persistent Internet or Unix domain socket connection */
PHP_FUNCTION(pfsockopen)
{
	php_fsockopen_stream(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
