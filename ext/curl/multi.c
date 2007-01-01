/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2007 The PHP Group                                |
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
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#define ZEND_INCLUDE_FULL_WINDOWS_HEADERS

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#if HAVE_CURL

#include "php_curl.h"

#include <curl/curl.h>
#include <curl/multi.h>

#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

/* {{{ proto resource curl_multi_init(void)
   Returns a new cURL multi handle */
PHP_FUNCTION(curl_multi_init)
{
	php_curlm *mh;
	
	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	mh = ecalloc(1, sizeof(php_curlm));
	mh->multi = curl_multi_init();

	zend_llist_init(&mh->easyh, sizeof(zval *), (llist_dtor_func_t) ZVAL_PTR_DTOR, 0);

	ZEND_REGISTER_RESOURCE(return_value, mh, le_curl_multi_handle);
}
/* }}} */

/* {{{ proto int curl_multi_add_handle(resource multi, resource ch)
   Add a normal cURL handle to a cURL multi handle */
PHP_FUNCTION(curl_multi_add_handle)
{
	zval      *z_mh;
	zval      *z_ch;
	php_curlm *mh;
	php_curl  *ch;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rr", &z_mh, &z_ch) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(mh, php_curlm *, &z_mh, -1, le_curl_multi_handle_name, le_curl_multi_handle);
	ZEND_FETCH_RESOURCE(ch, php_curl *, &z_ch, -1, le_curl_name, le_curl);

	zval_add_ref(&z_ch);

	_php_curl_cleanup_handle(ch);
	ch->uses++;

	zend_llist_add_element(&mh->easyh, &z_ch);

	RETURN_LONG((long) curl_multi_add_handle(mh->multi, ch->cp));	
}
/* }}} */

/* {{{ proto int curl_multi_remove_handle(resource mh, resource ch)
   Remove a multi handle from a set of cURL handles */
PHP_FUNCTION(curl_multi_remove_handle)
{
	zval      *z_mh;
	zval      *z_ch;
	php_curlm *mh;
	php_curl  *ch;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rr", &z_mh, &z_ch) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(mh, php_curlm *, &z_mh, -1, le_curl_multi_handle_name, le_curl_multi_handle);
	ZEND_FETCH_RESOURCE(ch, php_curl *, &z_ch, -1, le_curl_name, le_curl);

	--ch->uses;
	
	RETURN_LONG((long) curl_multi_remove_handle(mh->multi, ch->cp));
}
/* }}} */

static void _make_timeval_struct(struct timeval *to, double timeout)
{
	unsigned long conv;

	conv = (unsigned long) (timeout * 1000000.0);
	to->tv_sec = conv / 1000000;
	to->tv_usec = conv % 1000000;
}

/* {{{ proto int curl_multi_select(resource mh[, double timeout])
   Get all the sockets associated with the cURL extension, which can then be "selected" */
PHP_FUNCTION(curl_multi_select)
{
	zval           *z_mh;
	php_curlm      *mh;
	fd_set          readfds;
	fd_set          writefds;
	fd_set          exceptfds;
	int             maxfd;
	double          timeout = 1.0;
	struct timeval  to;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|d", &z_mh, &timeout) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(mh, php_curlm *, &z_mh, -1, le_curl_multi_handle_name, le_curl_multi_handle);

	_make_timeval_struct(&to, timeout);
	
	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
	FD_ZERO(&exceptfds);

	curl_multi_fdset(mh->multi, &readfds, &writefds, &exceptfds, &maxfd);
	RETURN_LONG(select(maxfd + 1, &readfds, &writefds, &exceptfds, &to));
}
/* }}} */

/* {{{ proto int curl_multi_exec(resource mh, int &still_running) 
   Run the sub-connections of the current cURL handle */
PHP_FUNCTION(curl_multi_exec)
{
	zval      *z_mh;
	zval      *z_still_running;
	php_curlm *mh;
	int        still_running;
	int        result;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rz", &z_mh, &z_still_running) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(mh, php_curlm *, &z_mh, -1, le_curl_multi_handle_name, le_curl_multi_handle);

	convert_to_long_ex(&z_still_running);
	still_running = Z_LVAL_P(z_still_running);
	result = curl_multi_perform(mh->multi, &still_running);
	ZVAL_LONG(z_still_running, still_running);

	RETURN_LONG(result);
}
/* }}} */

/* {{{ proto string curl_multi_getcontent(resource ch)
   Return the content of a cURL handle if CURLOPT_RETURNTRANSFER is set */
PHP_FUNCTION(curl_multi_getcontent)
{
	zval     *z_ch;
	php_curl *ch;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &z_ch) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(ch, php_curl *, &z_ch, -1, le_curl_name, le_curl);

	if (ch->handlers->write->method == PHP_CURL_RETURN && ch->handlers->write->buf.len > 0) {
		smart_str_0(&ch->handlers->write->buf);
		RETURN_STRINGL(ch->handlers->write->buf.c, ch->handlers->write->buf.len, 1);
	}
}

/* {{{ proto array curl_multi_info_read(resource mh)
   Get information about the current transfers */
PHP_FUNCTION(curl_multi_info_read)
{
	zval      *z_mh;
	php_curlm *mh;
	CURLMsg   *tmp_msg;
	int        queued_msgs;

	/* XXX: Not Implemented */
	return;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &z_mh) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(mh, php_curlm *, &z_mh, -1, le_curl_multi_handle_name, le_curl_multi_handle);

	tmp_msg = curl_multi_info_read(mh->multi, &queued_msgs);
	if (tmp_msg == NULL) {
		RETURN_FALSE;
	}

	array_init(return_value);
	add_assoc_long(return_value, "msg", tmp_msg->msg);
	add_assoc_long(return_value, "result", tmp_msg->data.result);
	/* add_assoc_resource(return_value, "handle", zend_list_id_by_pointer(tmp_msg->easy_handle, le_curl TSRMLS_CC)); */
	add_assoc_string(return_value, "whatever", (char *) tmp_msg->data.whatever, 1);
}
/* }}} */

/* {{{ proto void curl_multi_close(resource mh)
   Close a set of cURL handles */
PHP_FUNCTION(curl_multi_close)
{
	zval      *z_mh;
	php_curlm *mh;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &z_mh) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(mh, php_curlm *, &z_mh, -1, le_curl_multi_handle_name, le_curl_multi_handle);

	zend_list_delete(Z_LVAL_P(z_mh));
}
/* }}} */

void _php_curl_multi_close(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	php_curlm *mh = (php_curlm *) rsrc->ptr;
	if (mh) {
		curl_multi_cleanup(mh->multi);
		zend_llist_clean(&mh->easyh);
		efree(mh);
		rsrc->ptr = NULL;
	}
}

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
