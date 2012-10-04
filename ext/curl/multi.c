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
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	mh = ecalloc(1, sizeof(php_curlm));
	mh->multi = curl_multi_init();

	zend_llist_init(&mh->easyh, sizeof(zval), _php_curl_multi_cleanup_list, 0);

	ZEND_REGISTER_RESOURCE(return_value, mh, le_curl_multi_handle);
}
/* }}} */

/* {{{ proto int curl_multi_add_handle(resource mh, resource ch)
   Add a normal cURL handle to a cURL multi handle */
PHP_FUNCTION(curl_multi_add_handle)
{
	zval      *z_mh;
	zval      *z_ch;
	php_curlm *mh;
	php_curl  *ch;
	zval tmp_val;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rr", &z_mh, &z_ch) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(mh, php_curlm *, &z_mh, -1, le_curl_multi_handle_name, le_curl_multi_handle);
	ZEND_FETCH_RESOURCE(ch, php_curl *, &z_ch, -1, le_curl_name, le_curl);

	_php_curl_cleanup_handle(ch);
	ch->uses++;

	/* we want to create a copy of this zval that we store in the multihandle structure element "easyh" */
	tmp_val = *z_ch;
	zval_copy_ctor(&tmp_val);

	zend_llist_add_element(&mh->easyh, &tmp_val);

	RETURN_LONG((long) curl_multi_add_handle(mh->multi, ch->cp));	
}
/* }}} */

void _php_curl_multi_cleanup_list(void *data) /* {{{ */
{
	zval *z_ch = (zval *)data;
	php_curl *ch;
	TSRMLS_FETCH();

	if (!z_ch) {
		return;
	}
	
	ch = (php_curl *) zend_fetch_resource(&z_ch TSRMLS_CC, -1, le_curl_name, NULL, 1, le_curl);
	if (!ch) {
		return;
	}

	if (ch->uses) {	
		ch->uses--;
	} else {
		zend_list_delete(Z_LVAL_P(z_ch));
	}
}
/* }}} */

/* Used internally as comparison routine passed to zend_list_del_element */
static int curl_compare_resources( zval *z1, zval **z2 ) /* {{{ */
{
	return (Z_TYPE_P( z1 ) == Z_TYPE_PP( z2 ) && 
            Z_TYPE_P( z1 ) == IS_RESOURCE     &&
            Z_LVAL_P( z1 ) == Z_LVAL_PP( z2 ) );
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

	zend_llist_del_element( &mh->easyh, &z_ch, 
							(int (*)(void *, void *)) curl_compare_resources );
	
	RETURN_LONG((long) curl_multi_remove_handle(mh->multi, ch->cp));
}
/* }}} */

static void _make_timeval_struct(struct timeval *to, double timeout) /* {{{ */
{
	unsigned long conv;

	conv = (unsigned long) (timeout * 1000000.0);
	to->tv_sec = conv / 1000000;
	to->tv_usec = conv % 1000000;
}
/* }}} */

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
	if (maxfd == -1) {
		RETURN_LONG(-1);
	}
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

	{
		zend_llist_position pos;
		php_curl *ch;
		zval	*pz_ch;

		for(pz_ch = (zval *)zend_llist_get_first_ex(&mh->easyh, &pos); pz_ch;
			pz_ch = (zval *)zend_llist_get_next_ex(&mh->easyh, &pos)) {

			ZEND_FETCH_RESOURCE(ch, php_curl *, &pz_ch, -1, le_curl_name, le_curl);
			_php_curl_verify_handlers(ch, 1 TSRMLS_CC);
		}
	}

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
/* }}} */

/* {{{ proto array curl_multi_info_read(resource mh [, long msgs_in_queue])
   Get information about the current transfers */
PHP_FUNCTION(curl_multi_info_read)
{
	zval      *z_mh;
	php_curlm *mh;
	CURLMsg	  *tmp_msg;
	int        queued_msgs;
	zval      *zmsgs_in_queue = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|z", &z_mh, &zmsgs_in_queue) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(mh, php_curlm *, &z_mh, -1, le_curl_multi_handle_name, le_curl_multi_handle);

	tmp_msg = curl_multi_info_read(mh->multi, &queued_msgs);
	if (tmp_msg == NULL) {
		RETURN_FALSE;
	}
	if (zmsgs_in_queue) {
		zval_dtor(zmsgs_in_queue);
		ZVAL_LONG(zmsgs_in_queue, queued_msgs);
	}

	array_init(return_value);
	add_assoc_long(return_value, "msg", tmp_msg->msg);
	add_assoc_long(return_value, "result", tmp_msg->data.result);

	/* find the original easy curl handle */
	{
		zend_llist_position pos;
		php_curl *ch;
		zval	*pz_ch;

		/* search the list of easy handles hanging off the multi-handle */
		for(pz_ch = (zval *)zend_llist_get_first_ex(&mh->easyh, &pos); pz_ch;
			pz_ch = (zval *)zend_llist_get_next_ex(&mh->easyh, &pos)) {
			ZEND_FETCH_RESOURCE(ch, php_curl *, &pz_ch, -1, le_curl_name, le_curl);
			if (ch->cp == tmp_msg->easy_handle) {

				/* we are adding a reference to the underlying php_curl
				   resource, so we need to add one to the resource's refcount 
				   in order to ensure it doesn't get destroyed when the 
				   underlying curl easy handle goes out of scope.
				   Normally you would call zval_copy_ctor( pz_ch ), or
				   SEPARATE_ZVAL, but those create new zvals, which is already
				   being done in add_assoc_resource */

				zend_list_addref( Z_RESVAL_P( pz_ch ) );

				/* add_assoc_resource automatically creates a new zval to 
				   wrap the "resource" represented by the current pz_ch */

				add_assoc_resource(return_value, "handle", Z_RESVAL_P(pz_ch));

				break;
			}
		}
	}
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

void _php_curl_multi_close(zend_rsrc_list_entry *rsrc TSRMLS_DC) /* {{{ */
{
	php_curlm *mh = (php_curlm *) rsrc->ptr;
	if (mh) {
		zend_llist_position pos;
		php_curl *ch;
		zval	*pz_ch;

		for(pz_ch = (zval *)zend_llist_get_first_ex(&mh->easyh, &pos); pz_ch;
			pz_ch = (zval *)zend_llist_get_next_ex(&mh->easyh, &pos)) {

			ch = (php_curl *) zend_fetch_resource(&pz_ch TSRMLS_CC, -1, le_curl_name, NULL, 1, le_curl);
			_php_curl_verify_handlers(ch, 0 TSRMLS_CC);
		}

		curl_multi_cleanup(mh->multi);
		zend_llist_clean(&mh->easyh);
		efree(mh);
		rsrc->ptr = NULL;
	}
}
/* }}} */

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
