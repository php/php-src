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
  | Authors: Wez Furlong <wez@thebrainroom.com>                          |
  |          Sara Golemon <pollita@php.net>                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "php.h"
#include "php_globals.h"
#include "ext/standard/flock_compat.h"
#include "ext/standard/file.h"
#include "ext/standard/php_filestat.h"
#include "php_open_temporary_file.h"
#include "ext/standard/basic_functions.h"
#include "php_ini.h"


#ifndef PHP_WIN32
#define php_select(m, r, w, e, t)	select(m, r, w, e, t)
#endif

/* {{{ proto resource stream_get_meta_data(resource fp)
    Retrieves header/meta data from streams/file pointers */
PHP_FUNCTION(stream_get_meta_data)
{
	zval **arg1;
	php_stream *stream;
	zval *newval;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	php_stream_from_zval(stream, arg1);

	array_init(return_value);
	
	if (stream->wrapperdata) {
		MAKE_STD_ZVAL(newval);
		*newval = *(stream->wrapperdata);
		zval_copy_ctor(newval);

		add_assoc_zval(return_value, "wrapper_data", newval);
	}
	if (stream->wrapper) {
		add_assoc_string(return_value, "wrapper_type", (char *)stream->wrapper->wops->label, 1);
	}
	add_assoc_string(return_value, "stream_type", (char *)stream->ops->label, 1);

#if 0	/* TODO: needs updating for new filter API */
	if (stream->filterhead) {
		php_stream_filter *filter;
		
		MAKE_STD_ZVAL(newval);
		array_init(newval);
		
		for (filter = stream->filterhead; filter != NULL; filter = filter->next) {
			add_next_index_string(newval, (char *)filter->fops->label, 1);
		}

		add_assoc_zval(return_value, "filters", newval);
	}
#endif
	
	add_assoc_long(return_value, "unread_bytes", stream->writepos - stream->readpos);

	if (!php_stream_populate_meta_data(stream, return_value)) {
		add_assoc_bool(return_value, "timed_out", 0);
		add_assoc_bool(return_value, "blocked", 1);
		add_assoc_bool(return_value, "eof", php_stream_eof(stream));
	}

}
/* }}} */

/* {{{ proto array stream_get_wrappers()
    Retrieves list of registered stream wrappers */
PHP_FUNCTION(stream_get_wrappers)
{
	HashTable *url_stream_wrappers_hash;
	char *stream_protocol;
	int key_flags, stream_protocol_len = 0;

	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	if ((url_stream_wrappers_hash = php_stream_get_url_stream_wrappers_hash())) {
		array_init(return_value);
		for(zend_hash_internal_pointer_reset(url_stream_wrappers_hash);
			(key_flags = zend_hash_get_current_key_ex(url_stream_wrappers_hash, &stream_protocol, &stream_protocol_len, NULL, 0, NULL)) != HASH_KEY_NON_EXISTANT;
			zend_hash_move_forward(url_stream_wrappers_hash)) {
				if (key_flags == HASH_KEY_IS_STRING) {
					add_next_index_stringl(return_value, stream_protocol, stream_protocol_len, 1);
				}
		}
	} else {
		RETURN_FALSE;
	}

}
/* }}} */

/* {{{ stream_select related functions */
static int stream_array_to_fd_set(zval *stream_array, fd_set *fds, int *max_fd TSRMLS_DC)
{
	zval **elem;
	php_stream *stream;
	int this_fd;

	if (Z_TYPE_P(stream_array) != IS_ARRAY) {
		return 0;
	}
	for (zend_hash_internal_pointer_reset(Z_ARRVAL_P(stream_array));
		 zend_hash_get_current_data(Z_ARRVAL_P(stream_array), (void **) &elem) == SUCCESS;
		 zend_hash_move_forward(Z_ARRVAL_P(stream_array))) {

		php_stream_from_zval_no_verify(stream, elem);
		if (stream == NULL) {
			continue;
		}
		/* get the fd.
		 * NB: Most other code will NOT use the PHP_STREAM_CAST_INTERNAL flag
		 * when casting.  It is only used here so that the buffered data warning
		 * is not displayed.
		 * */
		if (SUCCESS == php_stream_cast(stream, PHP_STREAM_AS_FD | PHP_STREAM_CAST_INTERNAL, (void*)&this_fd, 1)) {
			FD_SET(this_fd, fds);
			if (this_fd > *max_fd) {
				*max_fd = this_fd;
			}
		}
	}
	return 1;
}

static int stream_array_from_fd_set(zval *stream_array, fd_set *fds TSRMLS_DC)
{
	zval **elem, **dest_elem;
	php_stream *stream;
	HashTable *new_hash;
	int this_fd, ret = 0;

	if (Z_TYPE_P(stream_array) != IS_ARRAY) {
		return 0;
	}
	ALLOC_HASHTABLE(new_hash);
	zend_hash_init(new_hash, 0, NULL, ZVAL_PTR_DTOR, 0);
	
	for (zend_hash_internal_pointer_reset(Z_ARRVAL_P(stream_array));
		 zend_hash_get_current_data(Z_ARRVAL_P(stream_array), (void **) &elem) == SUCCESS;
		 zend_hash_move_forward(Z_ARRVAL_P(stream_array))) {

		php_stream_from_zval_no_verify(stream, elem);
		if (stream == NULL) {
			continue;
		}
		/* get the fd 
		 * NB: Most other code will NOT use the PHP_STREAM_CAST_INTERNAL flag
		 * when casting.  It is only used here so that the buffered data warning
		 * is not displayed.
		 */
		if (SUCCESS == php_stream_cast(stream, PHP_STREAM_AS_FD | PHP_STREAM_CAST_INTERNAL, (void*)&this_fd, 1)) {
			if (FD_ISSET(this_fd, fds)) {
				zend_hash_next_index_insert(new_hash, (void *)elem, sizeof(zval *), (void **)&dest_elem);
				if (dest_elem) {
					zval_add_ref(dest_elem);
				}
				ret++;
				continue;
			}
		}
	}

	/* destroy old array and add new one */
	zend_hash_destroy(Z_ARRVAL_P(stream_array));
	efree(Z_ARRVAL_P(stream_array));

	zend_hash_internal_pointer_reset(new_hash);
	Z_ARRVAL_P(stream_array) = new_hash;
	
	return ret;
}

static int stream_array_emulate_read_fd_set(zval *stream_array TSRMLS_DC)
{
	zval **elem, **dest_elem;
	php_stream *stream;
	HashTable *new_hash;
	int ret = 0;

	if (Z_TYPE_P(stream_array) != IS_ARRAY) {
		return 0;
	}
	ALLOC_HASHTABLE(new_hash);
	zend_hash_init(new_hash, 0, NULL, ZVAL_PTR_DTOR, 0);
	
	for (zend_hash_internal_pointer_reset(Z_ARRVAL_P(stream_array));
		 zend_hash_get_current_data(Z_ARRVAL_P(stream_array), (void **) &elem) == SUCCESS;
		 zend_hash_move_forward(Z_ARRVAL_P(stream_array))) {

		php_stream_from_zval_no_verify(stream, elem);
		if (stream == NULL) {
			continue;
		}
		if ((stream->writepos - stream->readpos) > 0) {
			/* allow readable non-descriptor based streams to participate in stream_select.
			 * Non-descriptor streams will only "work" if they have previously buffered the
			 * data.  Not ideal, but better than nothing.
			 * This branch of code also allows blocking streams with buffered data to
			 * operate correctly in stream_select.
			 * */
			zend_hash_next_index_insert(new_hash, (void *)elem, sizeof(zval *), (void **)&dest_elem);
			if (dest_elem) {
				zval_add_ref(dest_elem);
			}
			ret++;
			continue;
		}
	}

	if (ret > 0) {
		/* destroy old array and add new one */
		zend_hash_destroy(Z_ARRVAL_P(stream_array));
		efree(Z_ARRVAL_P(stream_array));

		zend_hash_internal_pointer_reset(new_hash);
		Z_ARRVAL_P(stream_array) = new_hash;
	} else {
		zend_hash_destroy(new_hash);
		FREE_HASHTABLE(new_hash);
	}
	
	return ret;
}
/* }}} */

/* {{{ proto int stream_select(array &read_streams, array &write_streams, array &except_streams, int tv_sec[, int tv_usec])
   Runs the select() system call on the sets of streams with a timeout specified by tv_sec and tv_usec */
PHP_FUNCTION(stream_select)
{
	zval			*r_array, *w_array, *e_array, *sec;
	struct timeval	tv;
	struct timeval *tv_p = NULL;
	fd_set			rfds, wfds, efds;
	int				max_fd = 0;
	int				retval, sets = 0, usec = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a!a!a!z!|l", &r_array, &w_array, &e_array, &sec, &usec) == FAILURE)
		return;

	FD_ZERO(&rfds);
	FD_ZERO(&wfds);
	FD_ZERO(&efds);

	if (r_array != NULL) sets += stream_array_to_fd_set(r_array, &rfds, &max_fd TSRMLS_CC);
	if (w_array != NULL) sets += stream_array_to_fd_set(w_array, &wfds, &max_fd TSRMLS_CC);
	if (e_array != NULL) sets += stream_array_to_fd_set(e_array, &efds, &max_fd TSRMLS_CC);

	if (!sets) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "No stream arrays were passed");
		RETURN_FALSE;
	}

	/* If seconds is not set to null, build the timeval, else we wait indefinitely */
	if (sec != NULL) {
		convert_to_long_ex(&sec);
		tv.tv_sec = Z_LVAL_P(sec);
		tv.tv_usec = usec;
		tv_p = &tv;
	}

	/* slight hack to support buffered data; if there is data sitting in the
	 * read buffer of any of the streams in the read array, let's pretend
	 * that we selected, but return only the readable sockets */
	if (r_array != NULL) {

		retval = stream_array_emulate_read_fd_set(r_array TSRMLS_CC);
		if (retval > 0) {
			RETURN_LONG(retval);
		}
	}
	
	retval = php_select(max_fd+1, &rfds, &wfds, &efds, tv_p);

	if (retval == -1) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "unable to select [%d]: %s (max_fd=%d)",
				errno, strerror(errno), max_fd);
		RETURN_FALSE;
	}

	if (r_array != NULL) stream_array_from_fd_set(r_array, &rfds TSRMLS_CC);
	if (w_array != NULL) stream_array_from_fd_set(w_array, &wfds TSRMLS_CC);
	if (e_array != NULL) stream_array_from_fd_set(e_array, &efds TSRMLS_CC);

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ stream_context related functions */
static void user_space_stream_notifier(php_stream_context *context, int notifycode, int severity,
		char *xmsg, int xcode, size_t bytes_sofar, size_t bytes_max, void * ptr TSRMLS_DC)
{
	zval *callback = (zval*)context->notifier->ptr;
	zval *retval = NULL;
	zval zvs[6];
	zval *ps[6];
	zval **ptps[6];
	int i;
	
	for (i = 0; i < 6; i++) {
		INIT_ZVAL(zvs[i]);
		ps[i] = &zvs[i];
		ptps[i] = &ps[i];
	}
		
	ZVAL_LONG(ps[0], notifycode);
	ZVAL_LONG(ps[1], severity);
	if (xmsg) {
		ZVAL_STRING(ps[2], xmsg, 0);
	} else {
		ZVAL_NULL(ps[2]);
	}
	ZVAL_LONG(ps[3], xcode);
	ZVAL_LONG(ps[4], bytes_sofar);
	ZVAL_LONG(ps[5], bytes_max);

	if (FAILURE == call_user_function_ex(EG(function_table), NULL, callback, &retval, 6, ptps, 0, NULL TSRMLS_CC)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "failed to call user notifier");
	}
	if (retval) {
		zval_ptr_dtor(&retval);
	}
}

static int parse_context_options(php_stream_context *context, zval *options)
{
	HashPosition pos, opos;
	zval **wval, **oval;
	char *wkey, *okey;
	int wkey_len, okey_len;
	int ret = SUCCESS;
	
	zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(options), &pos);
	while (SUCCESS == zend_hash_get_current_data_ex(Z_ARRVAL_P(options), (void**)&wval, &pos)) {
		if (HASH_KEY_IS_STRING == zend_hash_get_current_key_ex(Z_ARRVAL_P(options), &wkey, &wkey_len, NULL, 0, &pos)
				&& Z_TYPE_PP(wval) == IS_ARRAY) {

			zend_hash_internal_pointer_reset_ex(Z_ARRVAL_PP(wval), &opos);
			while (SUCCESS == zend_hash_get_current_data_ex(Z_ARRVAL_PP(wval), (void**)&oval, &opos)) {

				if (HASH_KEY_IS_STRING == zend_hash_get_current_key_ex(Z_ARRVAL_PP(wval), &okey, &okey_len, NULL, 0, &opos)) {
					ZVAL_ADDREF(*oval);
					php_stream_context_set_option(context, wkey, okey, *oval);
				}
				zend_hash_move_forward_ex(Z_ARRVAL_PP(wval), &opos);
			}

		} else {
			zend_error(E_WARNING, "options should have the form [\"wrappername\"][\"optionname\"] = $value");
		}
		zend_hash_move_forward_ex(Z_ARRVAL_P(options), &pos);
	}

	return ret;
}

static int parse_context_params(php_stream_context *context, zval *params)
{
	int ret = SUCCESS;
	zval **tmp;

	if (SUCCESS == zend_hash_find(Z_ARRVAL_P(params), "notification", sizeof("notification"), (void**)&tmp)) {
		
		if (context->notifier) {
			php_stream_notification_free(context->notifier);
			context->notifier = NULL;
		}

		context->notifier = php_stream_notification_alloc();
		context->notifier->func = user_space_stream_notifier;
		context->notifier->ptr = *tmp;
		ZVAL_ADDREF(*tmp);
	}
	if (SUCCESS == zend_hash_find(Z_ARRVAL_P(params), "options", sizeof("options"), (void**)&tmp)) {
		parse_context_options(context, *tmp);
	}
	
	return ret;
}

/* given a zval which is either a stream or a context, return the underlying
 * stream_context.  If it is a stream that does not have a context assigned, it
 * will create and assign a context and return that.  */
static php_stream_context *decode_context_param(zval *contextresource TSRMLS_DC)
{
	php_stream_context *context = NULL;

	context = zend_fetch_resource(&contextresource TSRMLS_CC, -1, "Stream-Context", NULL, 1, php_le_stream_context());
	if (context == NULL) {
		php_stream *stream;

		php_stream_from_zval_no_verify(stream, &contextresource);

		if (stream) {
			context = stream->context;
			if (context == NULL) {
				context = stream->context = php_stream_context_alloc();
			}
		}
	}

	return context;
}
/* }}} */

/* {{{ proto array stream_context_get_options(resource context|resource stream)
   Retrieve options for a stream/wrapper/context */
PHP_FUNCTION(stream_context_get_options)
{
	zval *zcontext;
	php_stream_context *context;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zcontext) == FAILURE) {
		RETURN_FALSE;
	}
	context = decode_context_param(zcontext TSRMLS_CC);
	ZEND_VERIFY_RESOURCE(context);

	array_init(return_value);
	*return_value = *context->options;
	zval_copy_ctor(return_value);
		
}
/* }}} */

/* {{{ proto bool stream_context_set_option(resource context|resource stream, string wrappername, string optionname, mixed value)
   Set an option for a wrapper */
PHP_FUNCTION(stream_context_set_option)
{
	zval *options = NULL, *zcontext = NULL, *zvalue = NULL;
	php_stream_context *context;
	char *wrappername, *optionname;
	int wrapperlen, optionlen;

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC,
				"rssz", &zcontext, &wrappername, &wrapperlen,
				&optionname, &optionlen, &zvalue) == FAILURE) {
		if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC,
					"ra", &zcontext, &options) == FAILURE) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "called with wrong number or type of parameters; please RTM");
			RETURN_FALSE;
		}
	}

	/* figure out where the context is coming from exactly */
	context = decode_context_param(zcontext TSRMLS_CC);
	ZEND_VERIFY_RESOURCE(context);

	if (options) {
		/* handle the array syntax */
		RETVAL_BOOL(parse_context_options(context, options) == SUCCESS);
	} else {
		ZVAL_ADDREF(zvalue);
		php_stream_context_set_option(context, wrappername, optionname, zvalue);
		RETVAL_TRUE;
	}
}
/* }}} */

/* {{{ proto bool stream_context_set_params(resource context|resource stream, array options)
   Set parameters for a file context */
PHP_FUNCTION(stream_context_set_params)
{
	zval *params, *zcontext;
	php_stream_context *context;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ra", &zcontext, &params) == FAILURE) {
		RETURN_FALSE;
	}

	context = decode_context_param(zcontext TSRMLS_CC);
	ZEND_VERIFY_RESOURCE(context);

	RETVAL_BOOL(parse_context_params(context, params) == SUCCESS);
}
/* }}} */

/* {{{ proto resource stream_context_create([array options])
   Create a file context and optionally set parameters */
PHP_FUNCTION(stream_context_create)
{
	zval *params = NULL;
	php_stream_context *context;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|a", &params) == FAILURE) {
		RETURN_FALSE;
	}
	
	context = php_stream_context_alloc();
	
	if (params) {
		parse_context_options(context, params);
	}
	
	ZEND_REGISTER_RESOURCE(return_value, context, php_le_stream_context());
}
/* }}} */

/* {{{ streams filter functions */
static void apply_filter_to_stream(int append, INTERNAL_FUNCTION_PARAMETERS)
{
	zval *zstream;
	php_stream *stream;
	char *filtername, *filterparams = NULL;
	int filternamelen, filterparamslen = 0, read_write = 0;
	php_stream_filter *filter;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|ls", &zstream,
				&filtername, &filternamelen, &read_write, &filterparams, &filterparamslen) == FAILURE) {
		RETURN_FALSE;
	}

	php_stream_from_zval(stream, &zstream);

	if ((read_write & PHP_STREAM_FILTER_ALL) == 0) {
		/* Chain not specified.
		 * Examine stream->mode to determine which filters are needed
		 * There's no harm in attaching a filter to an unused chain,
		 * but why waste the memory and clock cycles? 
		 */
		if (strchr(stream->mode, 'r') || strchr(stream->mode, '+')) {
			read_write |= PHP_STREAM_FILTER_READ;
		}
		if (strchr(stream->mode, 'w') || strchr(stream->mode, '+') || strchr(stream->mode, 'a')) {
			read_write |= PHP_STREAM_FILTER_WRITE;
		}
	}

	if (read_write & PHP_STREAM_FILTER_READ) {
		filter = php_stream_filter_create(filtername, filterparams, filterparamslen, php_stream_is_persistent(stream) TSRMLS_CC);
		if (filter == NULL) {
			RETURN_FALSE;
		}

		if (append) { 
			php_stream_filter_append(&stream->readfilters, filter);
		} else {
			php_stream_filter_prepend(&stream->readfilters, filter);
		}
	}

	if (read_write & PHP_STREAM_FILTER_WRITE) {
		filter = php_stream_filter_create(filtername, filterparams, filterparamslen, php_stream_is_persistent(stream) TSRMLS_CC);
		if (filter == NULL) {
			RETURN_FALSE;
		}

		if (append) { 
			php_stream_filter_append(&stream->writefilters, filter);
		} else {
			php_stream_filter_prepend(&stream->writefilters, filter);
		}
	}

	RETURN_TRUE;
}

/* {{{ proto bool stream_filter_prepend(resource stream, string filtername[, int read_write[, string filterparams]])
   Prepend a filter to a stream */
PHP_FUNCTION(stream_filter_prepend)
{
	apply_filter_to_stream(0, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto bool stream_filter_append(resource stream, string filtername[, int read_write[, string filterparams]])
   Append a filter to a stream */
PHP_FUNCTION(stream_filter_append)
{
	apply_filter_to_stream(1, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* }}} */

/* {{{ proto string stream_get_line(resource stream, int maxlen, string ending)
   Read up to maxlen bytes from a stream or until the ending string is found */
PHP_FUNCTION(stream_get_line)
{
	char *str;
	int str_len;
	long max_length;
	zval *zstream;
	char *buf;
	size_t buf_size;
	php_stream *stream;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rls", &zstream, &max_length, &str, &str_len) == FAILURE) {
		RETURN_FALSE;
	}

	if (max_length < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "The maximum allowed length must be greater then or equal to zero.");
		RETURN_FALSE;
	}

	php_stream_from_zval(stream, &zstream);

	if ((buf = php_stream_get_record(stream, max_length, &buf_size, str, str_len TSRMLS_CC))) {
		RETURN_STRINGL(buf, buf_size, 0);
	} else {
		RETURN_FALSE;
	}
}

/* }}} */

/* {{{ proto bool stream_set_blocking(resource socket, int mode)
   Set blocking/non-blocking mode on a socket or stream */
PHP_FUNCTION(stream_set_blocking)
{
	zval **arg1, **arg2;
	int block;
	php_stream *stream;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	php_stream_from_zval(stream, arg1);

	convert_to_long_ex(arg2);
	block = Z_LVAL_PP(arg2);

	if (php_stream_set_option(stream, PHP_STREAM_OPTION_BLOCKING, block == 0 ? 0 : 1, NULL) == -1)
		RETURN_FALSE;
	RETURN_TRUE;
}

/* }}} */

/* {{{ proto bool set_socket_blocking(resource socket, int mode)
   Set blocking/non-blocking mode on a socket */
PHP_FUNCTION(set_socket_blocking)
{
	php_error_docref(NULL TSRMLS_CC, E_NOTICE, "This function is deprecated, use stream_set_blocking() instead");
	PHP_FN(stream_set_blocking)(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto bool stream_set_timeout(resource stream, int seconds, int microseconds)
   Set timeout on stream read to seconds + microseonds */
#if HAVE_SYS_TIME_H || defined(PHP_WIN32)
PHP_FUNCTION(stream_set_timeout)
{
	zval **socket, **seconds, **microseconds;
	struct timeval t;
	php_stream *stream;

	if (ZEND_NUM_ARGS() < 2 || ZEND_NUM_ARGS() > 3 ||
		zend_get_parameters_ex(ZEND_NUM_ARGS(), &socket, &seconds, &microseconds)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	php_stream_from_zval(stream, socket);

	convert_to_long_ex(seconds);
	t.tv_sec = Z_LVAL_PP(seconds);

	if (ZEND_NUM_ARGS() == 3) {
		convert_to_long_ex(microseconds);
		t.tv_usec = Z_LVAL_PP(microseconds) % 1000000;
		t.tv_sec += Z_LVAL_PP(microseconds) / 1000000;
	}
	else
		t.tv_usec = 0;

	if (PHP_STREAM_OPTION_RETURN_OK == php_stream_set_option(stream, PHP_STREAM_OPTION_READ_TIMEOUT, 0, &t)) {
		RETURN_TRUE;
	}

	RETURN_FALSE;
}
#endif /* HAVE_SYS_TIME_H || defined(PHP_WIN32) */
/* }}} */

/* {{{ proto int stream_set_write_buffer(resource fp, int buffer)
   Set file write buffer */
PHP_FUNCTION(stream_set_write_buffer)
{
	zval **arg1, **arg2;
	int ret;
	size_t buff;
	php_stream *stream;

	switch (ZEND_NUM_ARGS()) {
	case 2:
		if (zend_get_parameters_ex(2, &arg1, &arg2)==FAILURE) {
			RETURN_FALSE;
		}
		break;
	default:
		WRONG_PARAM_COUNT;
		/* NOTREACHED */
		break;
	}
	
	php_stream_from_zval(stream, arg1);
	
	convert_to_long_ex(arg2);
	buff = Z_LVAL_PP(arg2);

	/* if buff is 0 then set to non-buffered */
	if (buff == 0) {
		ret = php_stream_set_option(stream, PHP_STREAM_OPTION_WRITE_BUFFER, PHP_STREAM_BUFFER_NONE, NULL);
	} else {
		ret = php_stream_set_option(stream, PHP_STREAM_OPTION_WRITE_BUFFER, PHP_STREAM_BUFFER_FULL, &buff);
	}

	RETURN_LONG(ret == 0 ? 0 : EOF);
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */

