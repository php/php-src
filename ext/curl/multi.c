/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
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
#include "Zend/zend_interfaces.h"
#include "Zend/zend_smart_str.h"

#include "curl_private.h"

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

/* CurlMultiHandle class */

zend_class_entry *curl_multi_ce;

static inline php_curlm *curl_multi_from_obj(zend_object *obj) {
	return (php_curlm *)((char *)(obj) - XtOffsetOf(php_curlm, std));
}

#define Z_CURL_MULTI_P(zv) curl_multi_from_obj(Z_OBJ_P(zv))

/* {{{ Returns a new cURL multi handle */
PHP_FUNCTION(curl_multi_init)
{
	php_curlm *mh;

	ZEND_PARSE_PARAMETERS_NONE();

	object_init_ex(return_value, curl_multi_ce);
	mh = Z_CURL_MULTI_P(return_value);
	mh->multi = curl_multi_init();
	mh->handlers = ecalloc(1, sizeof(php_curlm_handlers));

	zend_llist_init(&mh->easyh, sizeof(zval), _php_curl_multi_cleanup_list, 0);
}
/* }}} */

/* {{{ Add a normal cURL handle to a cURL multi handle */
PHP_FUNCTION(curl_multi_add_handle)
{
	zval      *z_mh;
	zval      *z_ch;
	php_curlm *mh;
	php_curl  *ch;
	CURLMcode error = CURLM_OK;

	ZEND_PARSE_PARAMETERS_START(2,2)
		Z_PARAM_OBJECT_OF_CLASS(z_mh, curl_multi_ce)
		Z_PARAM_OBJECT_OF_CLASS(z_ch, curl_ce)
	ZEND_PARSE_PARAMETERS_END();

	mh = Z_CURL_MULTI_P(z_mh);
	ch = Z_CURL_P(z_ch);

	_php_curl_verify_handlers(ch, 1);

	_php_curl_cleanup_handle(ch);

	Z_ADDREF_P(z_ch);
	zend_llist_add_element(&mh->easyh, z_ch);

	error = curl_multi_add_handle(mh->multi, ch->cp);
	SAVE_CURLM_ERROR(mh, error);

	RETURN_LONG((zend_long) error);
}
/* }}} */

void _php_curl_multi_cleanup_list(void *data) /* {{{ */
{
	zval *z_ch = (zval *)data;

	zval_ptr_dtor(z_ch);
}
/* }}} */

/* Used internally as comparison routine passed to zend_list_del_element */
static int curl_compare_objects( zval *z1, zval *z2 ) /* {{{ */
{
	return (Z_TYPE_P(z1) == Z_TYPE_P(z2) &&
			Z_TYPE_P(z1) == IS_OBJECT &&
			Z_OBJ_P(z1) == Z_OBJ_P(z2));
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
		tmp_ch = Z_CURL_P(pz_ch_temp);

		if (tmp_ch->cp == easy) {
			return pz_ch_temp;
		}
	}

	return NULL;
}
/* }}} */

/* {{{ Remove a multi handle from a set of cURL handles */
PHP_FUNCTION(curl_multi_remove_handle)
{
	zval      *z_mh;
	zval      *z_ch;
	php_curlm *mh;
	php_curl  *ch;
	CURLMcode error = CURLM_OK;

	ZEND_PARSE_PARAMETERS_START(2,2)
		Z_PARAM_OBJECT_OF_CLASS(z_mh, curl_multi_ce)
		Z_PARAM_OBJECT_OF_CLASS(z_ch, curl_ce)
	ZEND_PARSE_PARAMETERS_END();

	mh = Z_CURL_MULTI_P(z_mh);
	ch = Z_CURL_P(z_ch);

	error = curl_multi_remove_handle(mh->multi, ch->cp);
	SAVE_CURLM_ERROR(mh, error);

	RETVAL_LONG((zend_long) error);
	zend_llist_del_element(&mh->easyh, z_ch, (int (*)(void *, void *))curl_compare_objects);

}
/* }}} */

/* {{{ Get all the sockets associated with the cURL extension, which can then be "selected" */
PHP_FUNCTION(curl_multi_select)
{
	zval           *z_mh;
	php_curlm      *mh;
	double          timeout = 1.0;
	int             numfds = 0;
	CURLMcode error = CURLM_OK;

	ZEND_PARSE_PARAMETERS_START(1,2)
		Z_PARAM_OBJECT_OF_CLASS(z_mh, curl_multi_ce)
		Z_PARAM_OPTIONAL
		Z_PARAM_DOUBLE(timeout)
	ZEND_PARSE_PARAMETERS_END();

	mh = Z_CURL_MULTI_P(z_mh);

	error = curl_multi_wait(mh->multi, NULL, 0, (unsigned long) (timeout * 1000.0), &numfds);
	if (CURLM_OK != error) {
		SAVE_CURLM_ERROR(mh, error);
		RETURN_LONG(-1);
	}

	RETURN_LONG(numfds);
}
/* }}} */

/* {{{ Run the sub-connections of the current cURL handle */
PHP_FUNCTION(curl_multi_exec)
{
	zval      *z_mh;
	zval      *z_still_running;
	php_curlm *mh;
	int        still_running;
	CURLMcode error = CURLM_OK;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_OBJECT_OF_CLASS(z_mh, curl_multi_ce)
		Z_PARAM_ZVAL(z_still_running)
	ZEND_PARSE_PARAMETERS_END();

	mh = Z_CURL_MULTI_P(z_mh);

	{
		zend_llist_position pos;
		php_curl *ch;
		zval	*pz_ch;

		for (pz_ch = (zval *)zend_llist_get_first_ex(&mh->easyh, &pos); pz_ch;
			pz_ch = (zval *)zend_llist_get_next_ex(&mh->easyh, &pos)) {
			ch = Z_CURL_P(pz_ch);

			_php_curl_verify_handlers(ch, 1);
		}
	}

	still_running = zval_get_long(z_still_running);
	error = curl_multi_perform(mh->multi, &still_running);
	ZEND_TRY_ASSIGN_REF_LONG(z_still_running, still_running);

	SAVE_CURLM_ERROR(mh, error);
	RETURN_LONG((zend_long) error);
}
/* }}} */

/* {{{ Return the content of a cURL handle if CURLOPT_RETURNTRANSFER is set */
PHP_FUNCTION(curl_multi_getcontent)
{
	zval     *z_ch;
	php_curl *ch;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(z_ch, curl_ce)
	ZEND_PARSE_PARAMETERS_END();

	ch = Z_CURL_P(z_ch);

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

/* {{{ Get information about the current transfers */
PHP_FUNCTION(curl_multi_info_read)
{
	zval      *z_mh;
	php_curlm *mh;
	CURLMsg	  *tmp_msg;
	int        queued_msgs;
	zval      *zmsgs_in_queue = NULL;
	php_curl  *ch;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_OBJECT_OF_CLASS(z_mh, curl_multi_ce)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(zmsgs_in_queue)
	ZEND_PARSE_PARAMETERS_END();

	mh = Z_CURL_MULTI_P(z_mh);

	tmp_msg = curl_multi_info_read(mh->multi, &queued_msgs);
	if (tmp_msg == NULL) {
		RETURN_FALSE;
	}

	if (zmsgs_in_queue) {
		ZEND_TRY_ASSIGN_REF_LONG(zmsgs_in_queue, queued_msgs);
	}

	array_init(return_value);
	add_assoc_long(return_value, "msg", tmp_msg->msg);
	add_assoc_long(return_value, "result", tmp_msg->data.result);

	/* find the original easy curl handle */
	{
		zval *pz_ch = _php_curl_multi_find_easy_handle(mh, tmp_msg->easy_handle);
		if (pz_ch != NULL) {
			/* we must save result to be able to read error message */
			ch = Z_CURL_P(pz_ch);
			SAVE_CURL_ERROR(ch, tmp_msg->data.result);

			Z_ADDREF_P(pz_ch);
			add_assoc_zval(return_value, "handle", pz_ch);
		}
	}
}
/* }}} */

/* {{{ Close a set of cURL handles */
PHP_FUNCTION(curl_multi_close)
{
	php_curlm *mh;
	zval *z_mh;

	zend_llist_position pos;
	zval *pz_ch;

	ZEND_PARSE_PARAMETERS_START(1,1)
		Z_PARAM_OBJECT_OF_CLASS(z_mh, curl_multi_ce)
	ZEND_PARSE_PARAMETERS_END();

	mh = Z_CURL_MULTI_P(z_mh);

	for (pz_ch = (zval *)zend_llist_get_first_ex(&mh->easyh, &pos); pz_ch;
		pz_ch = (zval *)zend_llist_get_next_ex(&mh->easyh, &pos)) {
		php_curl *ch = Z_CURL_P(pz_ch);
		_php_curl_verify_handlers(ch, 1);
		curl_multi_remove_handle(mh->multi, ch->cp);
	}
	zend_llist_clean(&mh->easyh);
}
/* }}} */

/* {{{ Return an integer containing the last multi curl error number */
PHP_FUNCTION(curl_multi_errno)
{
	zval        *z_mh;
	php_curlm   *mh;

	ZEND_PARSE_PARAMETERS_START(1,1)
		Z_PARAM_OBJECT_OF_CLASS(z_mh, curl_multi_ce)
	ZEND_PARSE_PARAMETERS_END();

	mh = Z_CURL_MULTI_P(z_mh);

	RETURN_LONG(mh->err.no);
}
/* }}} */

/* {{{ return string describing error code */
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
	char 					*header;
	int  					error;
	zend_fcall_info 		fci 			= empty_fcall_info;

	pz_parent_ch = _php_curl_multi_find_easy_handle(mh, parent_ch);
	if (pz_parent_ch == NULL) {
		return rval;
	}

	parent = Z_CURL_P(pz_parent_ch);

	ch = init_curl_handle_into_zval(&pz_ch);
	ch->cp = easy;
	_php_setup_easy_copy_handlers(ch, parent);

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
		case CURLMOPT_PIPELINING:
		case CURLMOPT_MAXCONNECTS:
#if LIBCURL_VERSION_NUM >= 0x071e00 /* 7.30.0 */
		case CURLMOPT_CHUNK_LENGTH_PENALTY_SIZE:
		case CURLMOPT_CONTENT_LENGTH_PENALTY_SIZE:
		case CURLMOPT_MAX_HOST_CONNECTIONS:
		case CURLMOPT_MAX_PIPELINE_LENGTH:
		case CURLMOPT_MAX_TOTAL_CONNECTIONS:
#endif
		{
			zend_long lval = zval_get_long(zvalue);

			if (option == CURLMOPT_PIPELINING && (lval & 1)) {
#if LIBCURL_VERSION_NUM >= 0x073e00 /* 7.62.0 */
				php_error_docref(NULL, E_WARNING, "CURLPIPE_HTTP1 is no longer supported");
#else
				php_error_docref(NULL, E_DEPRECATED, "CURLPIPE_HTTP1 is deprecated");
#endif
			}
			error = curl_multi_setopt(mh->multi, option, lval);
			break;
		}
#if LIBCURL_VERSION_NUM > 0x072D00 /* Available since 7.45.0 */
		case CURLMOPT_PUSHFUNCTION:
			if (mh->handlers->server_push == NULL) {
				mh->handlers->server_push = ecalloc(1, sizeof(php_curlm_server_push));
			} else if (!Z_ISUNDEF(mh->handlers->server_push->func_name)) {
				zval_ptr_dtor(&mh->handlers->server_push->func_name);
				mh->handlers->server_push->fci_cache = empty_fcall_info_cache;
			}

			ZVAL_COPY(&mh->handlers->server_push->func_name, zvalue);
			mh->handlers->server_push->method = PHP_CURL_USER;
			error = curl_multi_setopt(mh->multi, option, _php_server_push_callback);
			if (error != CURLM_OK) {
				return 0;
			}
			error = curl_multi_setopt(mh->multi, CURLMOPT_PUSHDATA, mh);
			break;
#endif
		default:
			zend_argument_value_error(2, "is not a valid cURL multi option");
			error = CURLM_UNKNOWN_OPTION;
			break;
	}

	SAVE_CURLM_ERROR(mh, error);

	return error != CURLM_OK;
}
/* }}} */

/* {{{ Set an option for the curl multi handle */
PHP_FUNCTION(curl_multi_setopt)
{
	zval       *z_mh, *zvalue;
	zend_long        options;
	php_curlm *mh;

	ZEND_PARSE_PARAMETERS_START(3,3)
		Z_PARAM_OBJECT_OF_CLASS(z_mh, curl_multi_ce)
		Z_PARAM_LONG(options)
		Z_PARAM_ZVAL(zvalue)
	ZEND_PARSE_PARAMETERS_END();

	mh = Z_CURL_MULTI_P(z_mh);

	if (!_php_curl_multi_setopt(mh, options, zvalue, return_value)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* CurlMultiHandle class */

static zend_object_handlers curl_multi_handlers;

static zend_object *curl_multi_create_object(zend_class_entry *class_type) {
	php_curlm *intern = zend_object_alloc(sizeof(php_curlm), class_type);

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);
	intern->std.handlers = &curl_multi_handlers;

	return &intern->std;
}

static zend_function *curl_multi_get_constructor(zend_object *object) {
	zend_throw_error(NULL, "Cannot directly construct CurlMultiHandle, use curl_multi_init() instead");
	return NULL;
}

void curl_multi_free_obj(zend_object *object)
{
	php_curlm *mh = curl_multi_from_obj(object);

	zend_llist_position pos;
	php_curl *ch;
	zval	*pz_ch;

	if (!mh->multi) {
		/* Can happen if constructor throws. */
		zend_object_std_dtor(&mh->std);
		return;
	}

	for (pz_ch = (zval *)zend_llist_get_first_ex(&mh->easyh, &pos); pz_ch;
		pz_ch = (zval *)zend_llist_get_next_ex(&mh->easyh, &pos)) {
		if (!(OBJ_FLAGS(Z_OBJ_P(pz_ch)) & IS_OBJ_FREE_CALLED)) {
			ch = Z_CURL_P(pz_ch);
			_php_curl_verify_handlers(ch, 0);
		}
	}

	curl_multi_cleanup(mh->multi);
	zend_llist_clean(&mh->easyh);
	if (mh->handlers->server_push) {
		zval_ptr_dtor(&mh->handlers->server_push->func_name);
		efree(mh->handlers->server_push);
	}
	if (mh->handlers) {
		efree(mh->handlers);
	}

	zend_object_std_dtor(&mh->std);
}

static HashTable *curl_multi_get_gc(zend_object *object, zval **table, int *n)
{
	php_curlm *curl_multi = curl_multi_from_obj(object);

	zend_get_gc_buffer *gc_buffer = zend_get_gc_buffer_create();

	if (curl_multi->handlers) {
		if (curl_multi->handlers->server_push) {
			zend_get_gc_buffer_add_zval(gc_buffer, &curl_multi->handlers->server_push->func_name);
		}
	}

	zend_llist_position pos;
	for (zval *pz_ch = (zval *) zend_llist_get_first_ex(&curl_multi->easyh, &pos); pz_ch;
		pz_ch = (zval *) zend_llist_get_next_ex(&curl_multi->easyh, &pos)) {
		zend_get_gc_buffer_add_zval(gc_buffer, pz_ch);
	}

	zend_get_gc_buffer_use(gc_buffer, table, n);

	return zend_std_get_properties(object);
}

void curl_multi_register_class(const zend_function_entry *method_entries) {
	zend_class_entry ce_multi;
	INIT_CLASS_ENTRY(ce_multi, "CurlMultiHandle", method_entries);
	curl_multi_ce = zend_register_internal_class(&ce_multi);
	curl_multi_ce->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;
	curl_multi_ce->create_object = curl_multi_create_object;
	curl_multi_ce->serialize = zend_class_serialize_deny;
	curl_multi_ce->unserialize = zend_class_unserialize_deny;

	memcpy(&curl_multi_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	curl_multi_handlers.offset = XtOffsetOf(php_curlm, std);
	curl_multi_handlers.free_obj = curl_multi_free_obj;
	curl_multi_handlers.get_gc = curl_multi_get_gc;
	curl_multi_handlers.get_constructor = curl_multi_get_constructor;
	curl_multi_handlers.clone_obj = NULL;
	curl_multi_handlers.cast_object = curl_cast_object;
	curl_multi_handlers.compare = zend_objects_not_comparable;
}
