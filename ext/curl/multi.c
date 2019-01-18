/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2018 The PHP Group                                |
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

#define SAVE_CURLM_ERROR(__handle, __err) (__handle)->err.no = (int) __err;

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
	mh->handlers = ecalloc(1, sizeof(php_curlm_handlers));

	zend_llist_init(&mh->easyh, sizeof(zval), _php_curl_multi_cleanup_list, 0);

	RETURN_RES(zend_register_resource(mh, le_curl_multi_handle));
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
	CURLMcode error = CURLM_OK;

	ZEND_PARSE_PARAMETERS_START(2,2)
		Z_PARAM_RESOURCE(z_mh)
		Z_PARAM_RESOURCE(z_ch)
	ZEND_PARSE_PARAMETERS_END();

	if ((mh = (php_curlm *)zend_fetch_resource(Z_RES_P(z_mh), le_curl_multi_handle_name, le_curl_multi_handle)) == NULL) {
		RETURN_FALSE;
	}

	if ((ch = (php_curl *)zend_fetch_resource(Z_RES_P(z_ch), le_curl_name, le_curl)) == NULL) {
		RETURN_FALSE;
	}

	_php_curl_cleanup_handle(ch);

	GC_ADDREF(Z_RES_P(z_ch));
	zend_llist_add_element(&mh->easyh, z_ch);

	error = curl_multi_add_handle(mh->multi, ch->cp);
	SAVE_CURLM_ERROR(mh, error);

	RETURN_LONG((zend_long) error);
}
/* }}} */

void _php_curl_multi_cleanup_list(void *data) /* {{{ */
{
	zval *z_ch = (zval *)data;
	php_curl *ch;

	if (!z_ch) {
		return;
	}
	if (!Z_RES_P(z_ch)->ptr) {
		return;
	}
	if ((ch = (php_curl *)zend_fetch_resource(Z_RES_P(z_ch), le_curl_name, le_curl)) == NULL) {
		return;
	}

	zend_list_delete(Z_RES_P(z_ch));
}
/* }}} */

/* Used internally as comparison routine passed to zend_list_del_element */
static int curl_compare_resources( zval *z1, zval *z2 ) /* {{{ */
{
	return (Z_TYPE_P(z1) == Z_TYPE_P(z2) &&
			Z_TYPE_P(z1) == IS_RESOURCE &&
			Z_RES_P(z1) == Z_RES_P(z2));
}
/* }}} */

/* Used to find the php_curl resource for a given curl easy handle */
static zval *_php_curl_multi_find_easy_handle(php_curlm *mh, CURL *easy) /* {{{ */
{
	php_curl 			*tmp_ch;
	zend_llist_position pos;
	zval				*pz_ch_temp;

	for(pz_ch_temp = (zval *)zend_llist_get_first_ex(&mh->easyh, &pos); pz_ch_temp;
		pz_ch_temp = (zval *)zend_llist_get_next_ex(&mh->easyh, &pos)) {

		if ((tmp_ch = (php_curl *)zend_fetch_resource(Z_RES_P(pz_ch_temp), le_curl_name, le_curl)) == NULL) {
			return NULL;
		}

		if (tmp_ch->cp == easy) {
			return pz_ch_temp;
		}
	}

	return NULL;
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
	CURLMcode error = CURLM_OK;

	ZEND_PARSE_PARAMETERS_START(2,2)
		Z_PARAM_RESOURCE(z_mh)
		Z_PARAM_RESOURCE(z_ch)
	ZEND_PARSE_PARAMETERS_END();

	if ((mh = (php_curlm *)zend_fetch_resource(Z_RES_P(z_mh), le_curl_multi_handle_name, le_curl_multi_handle)) == NULL) {
		RETURN_FALSE;
	}

	if ((ch = (php_curl *)zend_fetch_resource(Z_RES_P(z_ch), le_curl_name, le_curl)) == NULL) {
		RETURN_FALSE;
	}

	error = curl_multi_remove_handle(mh->multi, ch->cp);
	SAVE_CURLM_ERROR(mh, error);

	RETVAL_LONG((zend_long) error);
	zend_llist_del_element(&mh->easyh, z_ch, (int (*)(void *, void *))curl_compare_resources);

}
/* }}} */

#if LIBCURL_VERSION_NUM < 0x071c00
static void _make_timeval_struct(struct timeval *to, double timeout) /* {{{ */
{
	unsigned long conv;

	conv = (unsigned long) (timeout * 1000000.0);
	to->tv_sec = conv / 1000000;
	to->tv_usec = conv % 1000000;
}
/* }}} */
#endif

/* {{{ proto int curl_multi_select(resource mh[, double timeout])
   Get all the sockets associated with the cURL extension, which can then be "selected" */
PHP_FUNCTION(curl_multi_select)
{
	zval           *z_mh;
	php_curlm      *mh;
	double          timeout = 1.0;
#if LIBCURL_VERSION_NUM >= 0x071c00 /* Available since 7.28.0 */
	int             numfds = 0;
#else
	fd_set          readfds;
	fd_set          writefds;
	fd_set          exceptfds;
	int             maxfd;
	struct timeval  to;
#endif
	CURLMcode error = CURLM_OK;

	ZEND_PARSE_PARAMETERS_START(1,2)
		Z_PARAM_RESOURCE(z_mh)
		Z_PARAM_OPTIONAL
		Z_PARAM_DOUBLE(timeout)
	ZEND_PARSE_PARAMETERS_END();

	if ((mh = (php_curlm *)zend_fetch_resource(Z_RES_P(z_mh), le_curl_multi_handle_name, le_curl_multi_handle)) == NULL) {
		RETURN_FALSE;
	}

#if LIBCURL_VERSION_NUM >= 0x071c00 /* Available since 7.28.0 */
	error = curl_multi_wait(mh->multi, NULL, 0, (unsigned long) timeout * 1000.0, &numfds);
	if (CURLM_OK != error) {
		SAVE_CURLM_ERROR(mh, error);
		RETURN_LONG(-1);
	}

	RETURN_LONG(numfds);
#else
	_make_timeval_struct(&to, timeout);

	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
	FD_ZERO(&exceptfds);

	error = curl_multi_fdset(mh->multi, &readfds, &writefds, &exceptfds, &maxfd);
	SAVE_CURLM_ERROR(mh, error);

	if (maxfd == -1) {
		RETURN_LONG(-1);
	}
	RETURN_LONG(select(maxfd + 1, &readfds, &writefds, &exceptfds, &to));
#endif
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
	CURLMcode error = CURLM_OK;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_RESOURCE(z_mh)
		Z_PARAM_ZVAL_DEREF(z_still_running)
	ZEND_PARSE_PARAMETERS_END();

	if ((mh = (php_curlm *)zend_fetch_resource(Z_RES_P(z_mh), le_curl_multi_handle_name, le_curl_multi_handle)) == NULL) {
		RETURN_FALSE;
	}

	{
		zend_llist_position pos;
		php_curl *ch;
		zval	*pz_ch;

		for (pz_ch = (zval *)zend_llist_get_first_ex(&mh->easyh, &pos); pz_ch;
			pz_ch = (zval *)zend_llist_get_next_ex(&mh->easyh, &pos)) {

			if ((ch = (php_curl *)zend_fetch_resource(Z_RES_P(pz_ch), le_curl_name, le_curl)) == NULL) {
				RETURN_FALSE;
			}

			_php_curl_verify_handlers(ch, 1);
		}
	}

	still_running = zval_get_long(z_still_running);
	error = curl_multi_perform(mh->multi, &still_running);
	zval_ptr_dtor(z_still_running);
	ZVAL_LONG(z_still_running, still_running);

	SAVE_CURLM_ERROR(mh, error);
	RETURN_LONG((zend_long) error);
}
/* }}} */

/* {{{ proto string curl_multi_getcontent(resource ch)
   Return the content of a cURL handle if CURLOPT_RETURNTRANSFER is set */
PHP_FUNCTION(curl_multi_getcontent)
{
	zval     *z_ch;
	php_curl *ch;

	ZEND_PARSE_PARAMETERS_START(1,1)
		Z_PARAM_RESOURCE(z_ch)
	ZEND_PARSE_PARAMETERS_END();

	if ((ch = (php_curl *)zend_fetch_resource(Z_RES_P(z_ch), le_curl_name, le_curl)) == NULL) {
		RETURN_FALSE;
	}

	if (ch->handlers->write->method == PHP_CURL_RETURN) {
		if (!ch->handlers->write->buf.s) {
			RETURN_EMPTY_STRING();
		}
		smart_str_0(&ch->handlers->write->buf);
		RETURN_STR_COPY(ch->handlers->write->buf.s);
	}

	RETURN_NULL();
}
/* }}} */

/* {{{ proto array curl_multi_info_read(resource mh [, int &msgs_in_queue])
   Get information about the current transfers */
PHP_FUNCTION(curl_multi_info_read)
{
	zval      *z_mh;
	php_curlm *mh;
	CURLMsg	  *tmp_msg;
	int        queued_msgs;
	zval      *zmsgs_in_queue = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_RESOURCE(z_mh)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL_DEREF(zmsgs_in_queue)
	ZEND_PARSE_PARAMETERS_END();

	if ((mh = (php_curlm *)zend_fetch_resource(Z_RES_P(z_mh), le_curl_multi_handle_name, le_curl_multi_handle)) == NULL) {
		RETURN_FALSE;
	}

	tmp_msg = curl_multi_info_read(mh->multi, &queued_msgs);
	if (tmp_msg == NULL) {
		RETURN_FALSE;
	}
	if (zmsgs_in_queue) {
		zval_ptr_dtor(zmsgs_in_queue);
		ZVAL_LONG(zmsgs_in_queue, queued_msgs);
	}

	array_init(return_value);
	add_assoc_long(return_value, "msg", tmp_msg->msg);
	add_assoc_long(return_value, "result", tmp_msg->data.result);

	/* find the original easy curl handle */
	{
		zval	*pz_ch = _php_curl_multi_find_easy_handle(mh, tmp_msg->easy_handle);
		if (pz_ch != NULL) {
			/* we are adding a reference to the underlying php_curl
			   resource, so we need to add one to the resource's refcount
			   in order to ensure it doesn't get destroyed when the
			   underlying curl easy handle goes out of scope.
			   Normally you would call zval_copy_ctor( pz_ch ), or
			   SEPARATE_ZVAL, but those create new zvals, which is already
			   being done in add_assoc_resource */
			Z_ADDREF_P(pz_ch);

			/* add_assoc_resource automatically creates a new zval to
			   wrap the "resource" represented by the current pz_ch */

			add_assoc_zval(return_value, "handle", pz_ch);
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

	ZEND_PARSE_PARAMETERS_START(1,1)
		Z_PARAM_RESOURCE(z_mh)
	ZEND_PARSE_PARAMETERS_END();

	if ((mh = (php_curlm *)zend_fetch_resource(Z_RES_P(z_mh), le_curl_multi_handle_name, le_curl_multi_handle)) == NULL) {
		RETURN_FALSE;
	}

	zend_list_close(Z_RES_P(z_mh));
}
/* }}} */

void _php_curl_multi_close(zend_resource *rsrc) /* {{{ */
{
	php_curlm *mh = (php_curlm *)rsrc->ptr;
	if (mh) {
		zend_llist_position pos;
		php_curl *ch;
		zval	*pz_ch;

		for (pz_ch = (zval *)zend_llist_get_first_ex(&mh->easyh, &pos); pz_ch;
			pz_ch = (zval *)zend_llist_get_next_ex(&mh->easyh, &pos)) {
			/* ptr is NULL means it already be freed */
			if (Z_RES_P(pz_ch)->ptr) {
				if ((ch = (php_curl *) zend_fetch_resource(Z_RES_P(pz_ch), le_curl_name, le_curl))) {
					_php_curl_verify_handlers(ch, 0);
				}
			}
		}

		curl_multi_cleanup(mh->multi);
		zend_llist_clean(&mh->easyh);
		if (mh->handlers->server_push) {
			efree(mh->handlers->server_push);
		}
		if (mh->handlers) {
			efree(mh->handlers);
		}
		efree(mh);
		rsrc->ptr = NULL;
	}
}
/* }}} */

/* {{{ proto int curl_multi_errno(resource mh)
         Return an integer containing the last multi curl error number */
PHP_FUNCTION(curl_multi_errno)
{
	zval        *z_mh;
	php_curlm   *mh;

	ZEND_PARSE_PARAMETERS_START(1,1)
		Z_PARAM_RESOURCE(z_mh)
	ZEND_PARSE_PARAMETERS_END();

	if ((mh = (php_curlm *)zend_fetch_resource(Z_RES_P(z_mh), le_curl_multi_handle_name, le_curl_multi_handle)) == NULL) {
		RETURN_FALSE;
	}

	RETURN_LONG(mh->err.no);
}
/* }}} */

/* {{{ proto bool curl_multi_strerror(int code)
         return string describing error code */
PHP_FUNCTION(curl_multi_strerror)
{
	zend_long code;
	const char *str;

	ZEND_PARSE_PARAMETERS_START(1,1)
		Z_PARAM_LONG(code)
	ZEND_PARSE_PARAMETERS_END();

	str = curl_multi_strerror(code);
	if (str) {
		RETURN_STRING(str);
	} else {
		RETURN_NULL();
	}
}
/* }}} */

#if LIBCURL_VERSION_NUM >= 0x072C00 /* Available since 7.44.0 */

static int _php_server_push_callback(CURL *parent_ch, CURL *easy, size_t num_headers, struct curl_pushheaders *push_headers, void *userp) /* {{{ */
{
	php_curl 				*ch;
	php_curl 				*parent;
	php_curlm 				*mh 			= (php_curlm *)userp;
	size_t 					rval 			= CURL_PUSH_DENY;
	php_curlm_server_push 	*t 				= mh->handlers->server_push;
	zval					*pz_parent_ch 	= NULL;
	zval 					pz_ch;
	zval 					headers;
	zval 					retval;
	zend_resource 			*res;
	char 					*header;
	int  					error;
	zend_fcall_info 		fci 			= empty_fcall_info;

	pz_parent_ch = _php_curl_multi_find_easy_handle(mh, parent_ch);
	if (pz_parent_ch == NULL) {
		return rval;
	}

	parent = (php_curl*)zend_fetch_resource(Z_RES_P(pz_parent_ch), le_curl_name, le_curl);

	ch = alloc_curl_handle();
	ch->cp = easy;
	_php_setup_easy_copy_handlers(ch, parent);

	Z_ADDREF_P(pz_parent_ch);

	res = zend_register_resource(ch, le_curl);
	ch->res = res;
	ZVAL_RES(&pz_ch, res);

	size_t i;
	array_init(&headers);
	for(i=0; i<num_headers; i++) {
		header = curl_pushheader_bynum(push_headers, i);
		add_next_index_string(&headers, header);
  	}

	zend_fcall_info_init(&t->func_name, 0, &fci, &t->fci_cache, NULL, NULL);

	zend_fcall_info_argn(
		&fci, 3,
		pz_parent_ch,
		&pz_ch,
		&headers
	);

	fci.retval = &retval;

	error = zend_call_function(&fci, &t->fci_cache);
	zend_fcall_info_args_clear(&fci, 1);
	zval_ptr_dtor_nogc(&headers);

	if (error == FAILURE) {
		php_error_docref(NULL, E_WARNING, "Cannot call the CURLMOPT_PUSHFUNCTION");
	} else if (!Z_ISUNDEF(retval)) {
		if (CURL_PUSH_DENY != zval_get_long(&retval)) {
		    rval = CURL_PUSH_OK;
			GC_ADDREF(Z_RES(pz_ch));
			zend_llist_add_element(&mh->easyh, &pz_ch);
		} else {
			/* libcurl will free this easy handle, avoid double free */
			ch->cp = NULL;
		}
	}

	return rval;
}
/* }}} */

#endif

static int _php_curl_multi_setopt(php_curlm *mh, zend_long option, zval *zvalue, zval *return_value) /* {{{ */
{
	CURLMcode error = CURLM_OK;

	switch (option) {
#if LIBCURL_VERSION_NUM >= 0x071000 /* 7.16.0 */
		case CURLMOPT_PIPELINING:
#endif
#if LIBCURL_VERSION_NUM >= 0x071003 /* 7.16.3 */
		case CURLMOPT_MAXCONNECTS:
#endif
#if LIBCURL_VERSION_NUM >= 0x071e00 /* 7.30.0 */
		case CURLMOPT_CHUNK_LENGTH_PENALTY_SIZE:
		case CURLMOPT_CONTENT_LENGTH_PENALTY_SIZE:
		case CURLMOPT_MAX_HOST_CONNECTIONS:
		case CURLMOPT_MAX_PIPELINE_LENGTH:
		case CURLMOPT_MAX_TOTAL_CONNECTIONS:
#endif
			error = curl_multi_setopt(mh->multi, option, zval_get_long(zvalue));
			break;
#if LIBCURL_VERSION_NUM > 0x072D00 /* Available since 7.46.0 */
		case CURLMOPT_PUSHFUNCTION:
			if (mh->handlers->server_push == NULL) {
				mh->handlers->server_push = ecalloc(1, sizeof(php_curlm_server_push));
			} else if (!Z_ISUNDEF(mh->handlers->server_push->func_name)) {
				zval_ptr_dtor(&mh->handlers->server_push->func_name);
				mh->handlers->server_push->fci_cache = empty_fcall_info_cache;
			}

			ZVAL_COPY(&mh->handlers->server_push->func_name, zvalue);
			mh->handlers->server_push->method = PHP_CURL_USER;
			if (!Z_ISUNDEF(mh->handlers->server_push->func_name)) {
				zval_ptr_dtor(&mh->handlers->server_push->func_name);
				mh->handlers->server_push->fci_cache = empty_fcall_info_cache;

			}
			error = curl_multi_setopt(mh->multi, option, _php_server_push_callback);
			if (error != CURLM_OK) {
				return 0;
			}
			error = curl_multi_setopt(mh->multi, CURLMOPT_PUSHDATA, mh);
			break;
#endif
		default:
			php_error_docref(NULL, E_WARNING, "Invalid curl multi configuration option");
			error = CURLM_UNKNOWN_OPTION;
			break;
	}

	SAVE_CURLM_ERROR(mh, error);
	if (error != CURLM_OK) {
		return 1;
	} else {
		return 0;
	}
}
/* }}} */

/* {{{ proto int curl_multi_setopt(resource mh, int option, mixed value)
       Set an option for the curl multi handle */
PHP_FUNCTION(curl_multi_setopt)
{
	zval       *z_mh, *zvalue;
	zend_long        options;
	php_curlm *mh;

	ZEND_PARSE_PARAMETERS_START(3,3)
		Z_PARAM_RESOURCE(z_mh)
		Z_PARAM_LONG(options)
		Z_PARAM_ZVAL(zvalue)
	ZEND_PARSE_PARAMETERS_END();

	if ((mh = (php_curlm *)zend_fetch_resource(Z_RES_P(z_mh), le_curl_multi_handle_name, le_curl_multi_handle)) == NULL) {
		RETURN_FALSE;
	}

	if (!_php_curl_multi_setopt(mh, options, zvalue, return_value)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
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
