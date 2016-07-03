/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2016 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
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
#include "streamsfuncs.h"
#include "php_network.h"
#include "php_string.h"

#ifndef PHP_WIN32
#define php_select(m, r, w, e, t)	select(m, r, w, e, t)
typedef unsigned long long php_timeout_ull;
#else
#include "win32/select.h"
#include "win32/sockets.h"
typedef unsigned __int64 php_timeout_ull;
#endif

#define GET_CTX_OPT(stream, wrapper, name, val) (PHP_STREAM_CONTEXT(stream) && NULL != (val = php_stream_context_get_option(PHP_STREAM_CONTEXT(stream), wrapper, name)))

static php_stream_context *decode_context_param(zval *contextresource);

/* Streams based network functions */

#if HAVE_SOCKETPAIR
/* {{{ proto array stream_socket_pair(int domain, int type, int protocol)
   Creates a pair of connected, indistinguishable socket streams */
PHP_FUNCTION(stream_socket_pair)
{
	zend_long domain, type, protocol;
	php_stream *s1, *s2;
	php_socket_t pair[2];

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "lll",
			&domain, &type, &protocol)) {
		RETURN_FALSE;
	}

	if (0 != socketpair((int)domain, (int)type, (int)protocol, pair)) {
		char errbuf[256];
		php_error_docref(NULL, E_WARNING, "failed to create sockets: [%d]: %s",
			php_socket_errno(), php_socket_strerror(php_socket_errno(), errbuf, sizeof(errbuf)));
		RETURN_FALSE;
	}

	array_init(return_value);

	s1 = php_stream_sock_open_from_socket(pair[0], 0);
	s2 = php_stream_sock_open_from_socket(pair[1], 0);

	/* set the __exposed flag.
	 * php_stream_to_zval() does, add_next_index_resource() does not */
	php_stream_auto_cleanup(s1);
	php_stream_auto_cleanup(s2);

	add_next_index_resource(return_value, s1->res);
	add_next_index_resource(return_value, s2->res);
}
/* }}} */
#endif

/* {{{ proto resource stream_socket_client(string remoteaddress [, long &errcode [, string &errstring [, double timeout [, long flags [, resource context]]]]])
   Open a client connection to a remote address */
PHP_FUNCTION(stream_socket_client)
{
	zend_string *host;
	zval *zerrno = NULL, *zerrstr = NULL, *zcontext = NULL;
	double timeout = (double)FG(default_socket_timeout);
	php_timeout_ull conv;
	struct timeval tv;
	char *hashkey = NULL;
	php_stream *stream = NULL;
	int err;
	zend_long flags = PHP_STREAM_CLIENT_CONNECT;
	zend_string *errstr = NULL;
	php_stream_context *context = NULL;

	RETVAL_FALSE;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|z/z/dlr", &host, &zerrno, &zerrstr, &timeout, &flags, &zcontext) == FAILURE) {
		RETURN_FALSE;
	}

	context = php_stream_context_from_zval(zcontext, flags & PHP_FILE_NO_DEFAULT_CONTEXT);

	if (flags & PHP_STREAM_CLIENT_PERSISTENT) {
		spprintf(&hashkey, 0, "stream_socket_client__%s", ZSTR_VAL(host));
	}

	/* prepare the timeout value for use */
	conv = (php_timeout_ull) (timeout * 1000000.0);
#ifdef PHP_WIN32
	tv.tv_sec = (long)(conv / 1000000);
	tv.tv_usec =(long)(conv % 1000000);
#else
	tv.tv_sec = conv / 1000000;
	tv.tv_usec = conv % 1000000;
#endif
	if (zerrno)	{
		zval_dtor(zerrno);
		ZVAL_LONG(zerrno, 0);
	}
	if (zerrstr) {
		zval_dtor(zerrstr);
		ZVAL_EMPTY_STRING(zerrstr);
	}

	stream = php_stream_xport_create(ZSTR_VAL(host), ZSTR_LEN(host), REPORT_ERRORS,
			STREAM_XPORT_CLIENT | (flags & PHP_STREAM_CLIENT_CONNECT ? STREAM_XPORT_CONNECT : 0) |
			(flags & PHP_STREAM_CLIENT_ASYNC_CONNECT ? STREAM_XPORT_CONNECT_ASYNC : 0),
			hashkey, &tv, context, &errstr, &err);


	if (stream == NULL) {
		/* host might contain binary characters */
		zend_string *quoted_host = php_addslashes(host, 0);

		php_error_docref(NULL, E_WARNING, "unable to connect to %s (%s)", ZSTR_VAL(quoted_host), errstr == NULL ? "Unknown error" : ZSTR_VAL(errstr));
		zend_string_release(quoted_host);
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
			zval_dtor(zerrstr);
			ZVAL_STR(zerrstr, errstr);
		} else if (errstr) {
			zend_string_release(errstr);
		}
		RETURN_FALSE;
	}

	if (errstr) {
		zend_string_release(errstr);
	}

	php_stream_to_zval(stream, return_value);

}
/* }}} */

/* {{{ proto resource stream_socket_server(string localaddress [, long &errcode [, string &errstring [, long flags [, resource context]]]])
   Create a server socket bound to localaddress */
PHP_FUNCTION(stream_socket_server)
{
	char *host;
	size_t host_len;
	zval *zerrno = NULL, *zerrstr = NULL, *zcontext = NULL;
	php_stream *stream = NULL;
	int err = 0;
	zend_long flags = STREAM_XPORT_BIND | STREAM_XPORT_LISTEN;
	zend_string *errstr = NULL;
	php_stream_context *context = NULL;

	RETVAL_FALSE;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|z/z/lr", &host, &host_len, &zerrno, &zerrstr, &flags, &zcontext) == FAILURE) {
		RETURN_FALSE;
	}

	context = php_stream_context_from_zval(zcontext, flags & PHP_FILE_NO_DEFAULT_CONTEXT);

	if (context) {
		GC_REFCOUNT(context->res)++;
	}

	if (zerrno)	{
		zval_dtor(zerrno);
		ZVAL_LONG(zerrno, 0);
	}
	if (zerrstr) {
		zval_dtor(zerrstr);
		ZVAL_EMPTY_STRING(zerrstr);
	}

	stream = php_stream_xport_create(host, host_len, REPORT_ERRORS,
			STREAM_XPORT_SERVER | (int)flags,
			NULL, NULL, context, &errstr, &err);

	if (stream == NULL) {
		php_error_docref(NULL, E_WARNING, "unable to connect to %s (%s)", host, errstr == NULL ? "Unknown error" : ZSTR_VAL(errstr));
	}

	if (stream == NULL)	{
		if (zerrno) {
			zval_dtor(zerrno);
			ZVAL_LONG(zerrno, err);
		}
		if (zerrstr && errstr) {
			zval_dtor(zerrstr);
			ZVAL_STR(zerrstr, errstr);
		} else if (errstr) {
			zend_string_release(errstr);
		}
		RETURN_FALSE;
	}

	if (errstr) {
		zend_string_release(errstr);
	}

	php_stream_to_zval(stream, return_value);
}
/* }}} */

/* {{{ proto resource stream_socket_accept(resource serverstream, [ double timeout [, string &peername ]])
   Accept a client connection from a server socket */
PHP_FUNCTION(stream_socket_accept)
{
	double timeout = (double)FG(default_socket_timeout);
	zval *zpeername = NULL;
	zend_string *peername = NULL;
	php_timeout_ull conv;
	struct timeval tv;
	php_stream *stream = NULL, *clistream = NULL;
	zval *zstream;
	zend_string *errstr = NULL;

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r|dz/", &zstream, &timeout, &zpeername) == FAILURE) {
		RETURN_FALSE;
	}
#else
	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_RESOURCE(zstream)
		Z_PARAM_OPTIONAL
		Z_PARAM_DOUBLE(timeout)
		Z_PARAM_ZVAL_EX(zpeername, 0, 1)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
#endif

	php_stream_from_zval(stream, zstream);

	/* prepare the timeout value for use */
	conv = (php_timeout_ull) (timeout * 1000000.0);
#ifdef PHP_WIN32
	tv.tv_sec = (long)(conv / 1000000);
	tv.tv_usec = (long)(conv % 1000000);
#else
	tv.tv_sec = conv / 1000000;
	tv.tv_usec = conv % 1000000;
#endif
	if (zpeername) {
		zval_dtor(zpeername);
		ZVAL_NULL(zpeername);
	}

	if (0 == php_stream_xport_accept(stream, &clistream,
				zpeername ? &peername : NULL,
				NULL, NULL,
				&tv, &errstr
				) && clistream) {

		if (peername) {
			ZVAL_STR(zpeername, peername);
		}
		php_stream_to_zval(clistream, return_value);
	} else {
		php_error_docref(NULL, E_WARNING, "accept failed: %s", errstr ? ZSTR_VAL(errstr) : "Unknown error");
		RETVAL_FALSE;
	}

	if (errstr) {
		zend_string_release(errstr);
	}
}
/* }}} */

/* {{{ proto string stream_socket_get_name(resource stream, bool want_peer)
   Returns either the locally bound or remote name for a socket stream */
PHP_FUNCTION(stream_socket_get_name)
{
	php_stream *stream;
	zval *zstream;
	zend_bool want_peer;
	zend_string *name = NULL;

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rb", &zstream, &want_peer) == FAILURE) {
		RETURN_FALSE;
	}
#else
	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_RESOURCE(zstream)
		Z_PARAM_BOOL(want_peer)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
#endif

	php_stream_from_zval(stream, zstream);

	if (0 != php_stream_xport_get_name(stream, want_peer,
				&name,
				NULL, NULL
				) || !name) {
		RETURN_FALSE;
	}

	RETVAL_STR(name);
}
/* }}} */

/* {{{ proto long stream_socket_sendto(resouce stream, string data [, long flags [, string target_addr]])
   Send data to a socket stream.  If target_addr is specified it must be in dotted quad (or [ipv6]) format */
PHP_FUNCTION(stream_socket_sendto)
{
	php_stream *stream;
	zval *zstream;
	zend_long flags = 0;
	char *data, *target_addr = NULL;
	size_t datalen, target_addr_len = 0;
	php_sockaddr_storage sa;
	socklen_t sl = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rs|ls", &zstream, &data, &datalen, &flags, &target_addr, &target_addr_len) == FAILURE) {
		RETURN_FALSE;
	}
	php_stream_from_zval(stream, zstream);

	if (target_addr_len) {
		/* parse the address */
		if (FAILURE == php_network_parse_network_address_with_port(target_addr, target_addr_len, (struct sockaddr*)&sa, &sl)) {
			php_error_docref(NULL, E_WARNING, "Failed to parse `%s' into a valid network address", target_addr);
			RETURN_FALSE;
		}
	}

	RETURN_LONG(php_stream_xport_sendto(stream, data, datalen, (int)flags, target_addr ? &sa : NULL, sl));
}
/* }}} */

/* {{{ proto string stream_socket_recvfrom(resource stream, long amount [, long flags [, string &remote_addr]])
   Receives data from a socket stream */
PHP_FUNCTION(stream_socket_recvfrom)
{
	php_stream *stream;
	zval *zstream, *zremote = NULL;
	zend_string *remote_addr = NULL;
	zend_long to_read = 0;
	zend_string *read_buf;
	zend_long flags = 0;
	int recvd;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rl|lz/", &zstream, &to_read, &flags, &zremote) == FAILURE) {
		RETURN_FALSE;
	}

	php_stream_from_zval(stream, zstream);

	if (zremote) {
		zval_dtor(zremote);
		ZVAL_NULL(zremote);
	}

	if (to_read <= 0) {
		php_error_docref(NULL, E_WARNING, "Length parameter must be greater than 0");
		RETURN_FALSE;
	}

	read_buf = zend_string_alloc(to_read, 0);

	recvd = php_stream_xport_recvfrom(stream, ZSTR_VAL(read_buf), to_read, (int)flags, NULL, NULL,
			zremote ? &remote_addr : NULL
			);

	if (recvd >= 0) {
		if (zremote) {
			ZVAL_STR(zremote, remote_addr);
		}
		ZSTR_VAL(read_buf)[recvd] = '\0';
		ZSTR_LEN(read_buf) = recvd;
		RETURN_NEW_STR(read_buf);
	}

	zend_string_free(read_buf);
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto string stream_get_contents(resource source [, long maxlen [, long offset]])
   Reads all remaining bytes (or up to maxlen bytes) from a stream and returns them as a string. */
PHP_FUNCTION(stream_get_contents)
{
	php_stream	*stream;
	zval		*zsrc;
	zend_long		maxlen		= (ssize_t) PHP_STREAM_COPY_ALL,
				desiredpos	= -1L;
	zend_string *contents;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r|ll", &zsrc, &maxlen, &desiredpos) == FAILURE) {
		RETURN_FALSE;
	}

	php_stream_from_zval(stream, zsrc);

	if (desiredpos >= 0) {
		int		seek_res = 0;
		zend_off_t	position;

		position = php_stream_tell(stream);
		if (position >= 0 && desiredpos > position) {
			/* use SEEK_CUR to allow emulation in streams that don't support seeking */
			seek_res = php_stream_seek(stream, desiredpos - position, SEEK_CUR);
		} else if (desiredpos < position)  {
			/* desired position before position or error on tell */
			seek_res = php_stream_seek(stream, desiredpos, SEEK_SET);
		}

		if (seek_res != 0) {
			php_error_docref(NULL, E_WARNING,
				"Failed to seek to position %pd in the stream", desiredpos);
			RETURN_FALSE;
		}
	}

	if (maxlen > INT_MAX) {
		php_error_docref(NULL, E_WARNING, "maxlen truncated from %pd to %d bytes", maxlen, INT_MAX);
		maxlen = INT_MAX;
	}
	if ((contents = php_stream_copy_to_mem(stream, maxlen, 0))) {
		RETURN_STR(contents);
	} else {
		RETURN_EMPTY_STRING();
	}
}
/* }}} */

/* {{{ proto long stream_copy_to_stream(resource source, resource dest [, long maxlen [, long pos]])
   Reads up to maxlen bytes from source stream and writes them to dest stream. */
PHP_FUNCTION(stream_copy_to_stream)
{
	php_stream *src, *dest;
	zval *zsrc, *zdest;
	zend_long maxlen = PHP_STREAM_COPY_ALL, pos = 0;
	size_t len;
	int ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rr|ll", &zsrc, &zdest, &maxlen, &pos) == FAILURE) {
		RETURN_FALSE;
	}

	php_stream_from_zval(src, zsrc);
	php_stream_from_zval(dest, zdest);

	if (pos > 0 && php_stream_seek(src, pos, SEEK_SET) < 0) {
		php_error_docref(NULL, E_WARNING, "Failed to seek to position " ZEND_LONG_FMT " in the stream", pos);
		RETURN_FALSE;
	}

	ret = php_stream_copy_to_stream_ex(src, dest, maxlen, &len);

	if (ret != SUCCESS) {
		RETURN_FALSE;
	}
	RETURN_LONG(len);
}
/* }}} */

/* {{{ proto array stream_get_meta_data(resource fp)
    Retrieves header/meta data from streams/file pointers */
PHP_FUNCTION(stream_get_meta_data)
{
	zval *zstream;
	php_stream *stream;

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &zstream) == FAILURE) {
		return;
	}
#else
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_RESOURCE(zstream)
	ZEND_PARSE_PARAMETERS_END();
#endif

	php_stream_from_zval(stream, zstream);

	array_init(return_value);

	if (!php_stream_populate_meta_data(stream, return_value)) {
		add_assoc_bool(return_value, "timed_out", 0);
		add_assoc_bool(return_value, "blocked", 1);
		add_assoc_bool(return_value, "eof", php_stream_eof(stream));
	}

	if (!Z_ISUNDEF(stream->wrapperdata)) {
		Z_ADDREF_P(&stream->wrapperdata);
		add_assoc_zval(return_value, "wrapper_data", &stream->wrapperdata);
	}
	if (stream->wrapper) {
		add_assoc_string(return_value, "wrapper_type", (char *)stream->wrapper->wops->label);
	}
	add_assoc_string(return_value, "stream_type", (char *)stream->ops->label);

	add_assoc_string(return_value, "mode", stream->mode);

#if 0	/* TODO: needs updating for new filter API */
	if (stream->filterhead) {
		php_stream_filter *filter;

		MAKE_STD_ZVAL(newval);
		array_init(newval);

		for (filter = stream->filterhead; filter != NULL; filter = filter->next) {
			add_next_index_string(newval, (char *)filter->fops->label);
		}

		add_assoc_zval(return_value, "filters", newval);
	}
#endif

	add_assoc_long(return_value, "unread_bytes", stream->writepos - stream->readpos);

	add_assoc_bool(return_value, "seekable", (stream->ops->seek) && (stream->flags & PHP_STREAM_FLAG_NO_SEEK) == 0);
	if (stream->orig_path) {
		add_assoc_string(return_value, "uri", stream->orig_path);
	}

}
/* }}} */

/* {{{ proto array stream_get_transports()
   Retrieves list of registered socket transports */
PHP_FUNCTION(stream_get_transports)
{
	HashTable *stream_xport_hash;
	zend_string *stream_xport;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if ((stream_xport_hash = php_stream_xport_get_hash())) {
		array_init(return_value);
		ZEND_HASH_FOREACH_STR_KEY(stream_xport_hash, stream_xport) {
			add_next_index_str(return_value, zend_string_copy(stream_xport));
		} ZEND_HASH_FOREACH_END();
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto array stream_get_wrappers()
    Retrieves list of registered stream wrappers */
PHP_FUNCTION(stream_get_wrappers)
{
	HashTable *url_stream_wrappers_hash;
	zend_string *stream_protocol;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if ((url_stream_wrappers_hash = php_stream_get_url_stream_wrappers_hash())) {
		array_init(return_value);
		ZEND_HASH_FOREACH_STR_KEY(url_stream_wrappers_hash, stream_protocol) {
			if (stream_protocol) {
				add_next_index_str(return_value, zend_string_copy(stream_protocol));
			}
		} ZEND_HASH_FOREACH_END();
	} else {
		RETURN_FALSE;
	}

}
/* }}} */

/* {{{ stream_select related functions */
static int stream_array_to_fd_set(zval *stream_array, fd_set *fds, php_socket_t *max_fd)
{
	zval *elem;
	php_stream *stream;
	int cnt = 0;

	if (Z_TYPE_P(stream_array) != IS_ARRAY) {
		return 0;
	}

	ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(stream_array), elem) {
		/* Temporary int fd is needed for the STREAM data type on windows, passing this_fd directly to php_stream_cast()
			would eventually bring a wrong result on x64. php_stream_cast() casts to int internally, and this will leave
			the higher bits of a SOCKET variable uninitialized on systems with little endian. */
		php_socket_t this_fd;

		ZVAL_DEREF(elem);
		php_stream_from_zval_no_verify(stream, elem);
		if (stream == NULL) {
			continue;
		}
		/* get the fd.
		 * NB: Most other code will NOT use the PHP_STREAM_CAST_INTERNAL flag
		 * when casting.  It is only used here so that the buffered data warning
		 * is not displayed.
		 * */
		if (SUCCESS == php_stream_cast(stream, PHP_STREAM_AS_FD_FOR_SELECT | PHP_STREAM_CAST_INTERNAL, (void*)&this_fd, 1) && this_fd != -1) {

			PHP_SAFE_FD_SET(this_fd, fds);

			if (this_fd > *max_fd) {
				*max_fd = this_fd;
			}
			cnt++;
		}
	} ZEND_HASH_FOREACH_END();
	return cnt ? 1 : 0;
}

static int stream_array_from_fd_set(zval *stream_array, fd_set *fds)
{
	zval *elem, *dest_elem, new_array;
	php_stream *stream;
	int ret = 0;
	zend_string *key;
	zend_ulong num_ind;

	if (Z_TYPE_P(stream_array) != IS_ARRAY) {
		return 0;
	}
	ZVAL_NEW_ARR(&new_array);
	zend_hash_init(Z_ARRVAL(new_array), zend_hash_num_elements(Z_ARRVAL_P(stream_array)), NULL, ZVAL_PTR_DTOR, 0);

	ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(stream_array), num_ind, key, elem) {
		php_socket_t this_fd;

		ZVAL_DEREF(elem);
		php_stream_from_zval_no_verify(stream, elem);
		if (stream == NULL) {
			continue;
		}
		/* get the fd
		 * NB: Most other code will NOT use the PHP_STREAM_CAST_INTERNAL flag
		 * when casting.  It is only used here so that the buffered data warning
		 * is not displayed.
		 */
		if (SUCCESS == php_stream_cast(stream, PHP_STREAM_AS_FD_FOR_SELECT | PHP_STREAM_CAST_INTERNAL, (void*)&this_fd, 1) && this_fd != SOCK_ERR) {
			if (PHP_SAFE_FD_ISSET(this_fd, fds)) {
				if (!key) {
					dest_elem = zend_hash_index_update(Z_ARRVAL(new_array), num_ind, elem);
				} else {
					dest_elem = zend_hash_update(Z_ARRVAL(new_array), key, elem);
				}

				if (dest_elem) {
					zval_add_ref(dest_elem);
				}
				ret++;
				continue;
			}
		}
	} ZEND_HASH_FOREACH_END();

	/* destroy old array and add new one */
	zend_array_destroy(Z_ARR_P(stream_array));
	Z_ARR_P(stream_array) = Z_ARR(new_array);

	return ret;
}

static int stream_array_emulate_read_fd_set(zval *stream_array)
{
	zval *elem, *dest_elem, new_array;
	php_stream *stream;
	int ret = 0;

	if (Z_TYPE_P(stream_array) != IS_ARRAY) {
		return 0;
	}
	ZVAL_NEW_ARR(&new_array);
	zend_hash_init(Z_ARRVAL(new_array), zend_hash_num_elements(Z_ARRVAL_P(stream_array)), NULL, ZVAL_PTR_DTOR, 0);

	ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(stream_array), elem) {
		ZVAL_DEREF(elem);
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
			dest_elem = zend_hash_next_index_insert(Z_ARRVAL(new_array), elem);
			if (dest_elem) {
				zval_add_ref(dest_elem);
			}
			ret++;
			continue;
		}
	} ZEND_HASH_FOREACH_END();

	if (ret > 0) {
		/* destroy old array and add new one */
		zend_array_destroy(Z_ARR_P(stream_array));
		Z_ARR_P(stream_array) = Z_ARR(new_array);
	} else {
		zend_array_destroy(Z_ARR(new_array));
	}

	return ret;
}
/* }}} */

/* {{{ proto int stream_select(array &read_streams, array &write_streams, array &except_streams, int tv_sec[, int tv_usec])
   Runs the select() system call on the sets of streams with a timeout specified by tv_sec and tv_usec */
PHP_FUNCTION(stream_select)
{
	zval *r_array, *w_array, *e_array;
	struct timeval tv, *tv_p = NULL;
	fd_set rfds, wfds, efds;
	php_socket_t max_fd = 0;
	int retval, sets = 0;
	zend_long sec, usec = 0;
	zend_bool secnull;
	int set_count, max_set_count = 0;

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "a/!a/!a/!l!|l", &r_array, &w_array, &e_array, &sec, &secnull, &usec) == FAILURE) {
		return;
	}
#else
	ZEND_PARSE_PARAMETERS_START(4, 5)
		Z_PARAM_ARRAY_EX(r_array, 1, 1)
		Z_PARAM_ARRAY_EX(w_array, 1, 1)
		Z_PARAM_ARRAY_EX(e_array, 1, 1)
		Z_PARAM_LONG_EX(sec, secnull, 1, 0)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(usec)
	ZEND_PARSE_PARAMETERS_END();
#endif

	FD_ZERO(&rfds);
	FD_ZERO(&wfds);
	FD_ZERO(&efds);

	if (r_array != NULL) {
		set_count = stream_array_to_fd_set(r_array, &rfds, &max_fd);
		if (set_count > max_set_count)
			max_set_count = set_count;
		sets += set_count;
	}

	if (w_array != NULL) {
		set_count = stream_array_to_fd_set(w_array, &wfds, &max_fd);
		if (set_count > max_set_count)
			max_set_count = set_count;
		sets += set_count;
	}

	if (e_array != NULL) {
		set_count = stream_array_to_fd_set(e_array, &efds, &max_fd);
		if (set_count > max_set_count)
			max_set_count = set_count;
		sets += set_count;
	}

	if (!sets) {
		php_error_docref(NULL, E_WARNING, "No stream arrays were passed");
		RETURN_FALSE;
	}

	PHP_SAFE_MAX_FD(max_fd, max_set_count);

	/* If seconds is not set to null, build the timeval, else we wait indefinitely */
	if (!secnull) {
		if (sec < 0) {
			php_error_docref(NULL, E_WARNING, "The seconds parameter must be greater than 0");
			RETURN_FALSE;
		} else if (usec < 0) {
			php_error_docref(NULL, E_WARNING, "The microseconds parameter must be greater than 0");
			RETURN_FALSE;
		}

		/* Windows, Solaris and BSD do not like microsecond values which are >= 1 sec */
		tv.tv_sec = (long)(sec + (usec / 1000000));
		tv.tv_usec = (long)(usec % 1000000);
		tv_p = &tv;
	}

	/* slight hack to support buffered data; if there is data sitting in the
	 * read buffer of any of the streams in the read array, let's pretend
	 * that we selected, but return only the readable sockets */
	if (r_array != NULL) {
		retval = stream_array_emulate_read_fd_set(r_array);
		if (retval > 0) {
			if (w_array != NULL) {
				zend_hash_clean(Z_ARRVAL_P(w_array));
			}
			if (e_array != NULL) {
				zend_hash_clean(Z_ARRVAL_P(e_array));
			}
			RETURN_LONG(retval);
		}
	}

	retval = php_select(max_fd+1, &rfds, &wfds, &efds, tv_p);

	if (retval == -1) {
		php_error_docref(NULL, E_WARNING, "unable to select [%d]: %s (max_fd=%d)",
				errno, strerror(errno), max_fd);
		RETURN_FALSE;
	}

	if (r_array != NULL) stream_array_from_fd_set(r_array, &rfds);
	if (w_array != NULL) stream_array_from_fd_set(w_array, &wfds);
	if (e_array != NULL) stream_array_from_fd_set(e_array, &efds);

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ stream_context related functions */
static void user_space_stream_notifier(php_stream_context *context, int notifycode, int severity,
		char *xmsg, int xcode, size_t bytes_sofar, size_t bytes_max, void * ptr)
{
	zval *callback = &context->notifier->ptr;
	zval retval;
	zval zvs[6];
	int i;

	ZVAL_LONG(&zvs[0], notifycode);
	ZVAL_LONG(&zvs[1], severity);
	if (xmsg) {
		ZVAL_STRING(&zvs[2], xmsg);
	} else {
		ZVAL_NULL(&zvs[2]);
	}
	ZVAL_LONG(&zvs[3], xcode);
	ZVAL_LONG(&zvs[4], bytes_sofar);
	ZVAL_LONG(&zvs[5], bytes_max);

	if (FAILURE == call_user_function_ex(EG(function_table), NULL, callback, &retval, 6, zvs, 0, NULL)) {
		php_error_docref(NULL, E_WARNING, "failed to call user notifier");
	}
	for (i = 0; i < 6; i++) {
		zval_ptr_dtor(&zvs[i]);
	}
	zval_ptr_dtor(&retval);
}

static void user_space_stream_notifier_dtor(php_stream_notifier *notifier)
{
	if (notifier && Z_TYPE(notifier->ptr) != IS_UNDEF) {
		zval_ptr_dtor(&notifier->ptr);
		ZVAL_UNDEF(&notifier->ptr);
	}
}

static int parse_context_options(php_stream_context *context, zval *options)
{
	zval *wval, *oval;
	zend_string *wkey, *okey;
	int ret = SUCCESS;

	ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(options), wkey, wval) {
		ZVAL_DEREF(wval);
		if (wkey && Z_TYPE_P(wval) == IS_ARRAY) {
			ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(wval), okey, oval) {
				if (okey) {
					php_stream_context_set_option(context, ZSTR_VAL(wkey), ZSTR_VAL(okey), oval);
				}
			} ZEND_HASH_FOREACH_END();
		} else {
			php_error_docref(NULL, E_WARNING, "options should have the form [\"wrappername\"][\"optionname\"] = $value");
		}
	} ZEND_HASH_FOREACH_END();

	return ret;
}

static int parse_context_params(php_stream_context *context, zval *params)
{
	int ret = SUCCESS;
	zval *tmp;

	if (NULL != (tmp = zend_hash_str_find(Z_ARRVAL_P(params), "notification", sizeof("notification")-1))) {

		if (context->notifier) {
			php_stream_notification_free(context->notifier);
			context->notifier = NULL;
		}

		context->notifier = php_stream_notification_alloc();
		context->notifier->func = user_space_stream_notifier;
		ZVAL_COPY(&context->notifier->ptr, tmp);
		context->notifier->dtor = user_space_stream_notifier_dtor;
	}
	if (NULL != (tmp = zend_hash_str_find(Z_ARRVAL_P(params), "options", sizeof("options")-1))) {
		if (Z_TYPE_P(tmp) == IS_ARRAY) {
			parse_context_options(context, tmp);
		} else {
			php_error_docref(NULL, E_WARNING, "Invalid stream/context parameter");
		}
	}

	return ret;
}

/* given a zval which is either a stream or a context, return the underlying
 * stream_context.  If it is a stream that does not have a context assigned, it
 * will create and assign a context and return that.  */
static php_stream_context *decode_context_param(zval *contextresource)
{
	php_stream_context *context = NULL;

	context = zend_fetch_resource_ex(contextresource, NULL, php_le_stream_context());
	if (context == NULL) {
		php_stream *stream;

		stream = zend_fetch_resource2_ex(contextresource, NULL, php_file_le_stream(), php_file_le_pstream());

		if (stream) {
			context = PHP_STREAM_CONTEXT(stream);
			if (context == NULL) {
				/* Only way this happens is if file is opened with NO_DEFAULT_CONTEXT
				   param, but then something is called which requires a context.
				   Don't give them the default one though since they already said they
	 			   didn't want it. */
				context = php_stream_context_alloc();
				stream->ctx = context->res;
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

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &zcontext) == FAILURE) {
		RETURN_FALSE;
	}
#else
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_RESOURCE(zcontext)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
#endif

	context = decode_context_param(zcontext);
	if (!context) {
		php_error_docref(NULL, E_WARNING, "Invalid stream/context parameter");
		RETURN_FALSE;
	}

	ZVAL_COPY(return_value, &context->options);
}
/* }}} */

/* {{{ proto bool stream_context_set_option(resource context|resource stream, string wrappername, string optionname, mixed value)
   Set an option for a wrapper */
PHP_FUNCTION(stream_context_set_option)
{
	zval *zcontext = NULL;
	php_stream_context *context;

	if (ZEND_NUM_ARGS() == 2) {
		zval *options;

#ifndef FAST_ZPP
		if (zend_parse_parameters(, ZEND_NUM_ARGS(), "ra", &zcontext, &options) == FAILURE) {
			RETURN_FALSE;
		}
#else
		ZEND_PARSE_PARAMETERS_START(2, 2)
			Z_PARAM_RESOURCE(zcontext)
			Z_PARAM_ARRAY(options)
		ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
#endif

		/* figure out where the context is coming from exactly */
		if (!(context = decode_context_param(zcontext))) {
			php_error_docref(NULL, E_WARNING, "Invalid stream/context parameter");
			RETURN_FALSE;
		}

		RETURN_BOOL(parse_context_options(context, options) == SUCCESS);
	} else {
		zval *zvalue;
		char *wrappername, *optionname;
		size_t wrapperlen, optionlen;

#ifndef FAST_ZPP
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "rssz", &zcontext, &wrappername, &wrapperlen, &optionname, &optionlen, &zvalue) == FAILURE) {
			RETURN_FALSE;
		}
#else
		ZEND_PARSE_PARAMETERS_START(4, 4)
			Z_PARAM_RESOURCE(zcontext)
			Z_PARAM_STRING(wrappername, wrapperlen)
			Z_PARAM_STRING(optionname, optionlen)
			Z_PARAM_ZVAL(zvalue)
		ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
#endif

		/* figure out where the context is coming from exactly */
		if (!(context = decode_context_param(zcontext))) {
			php_error_docref(NULL, E_WARNING, "Invalid stream/context parameter");
			RETURN_FALSE;
		}

		RETURN_BOOL(php_stream_context_set_option(context, wrappername, optionname, zvalue) == SUCCESS);
	}
}
/* }}} */

/* {{{ proto bool stream_context_set_params(resource context|resource stream, array options)
   Set parameters for a file context */
PHP_FUNCTION(stream_context_set_params)
{
	zval *params, *zcontext;
	php_stream_context *context;

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ra", &zcontext, &params) == FAILURE) {
		RETURN_FALSE;
	}
#else
	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_RESOURCE(zcontext)
		Z_PARAM_ARRAY(params)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
#endif

	context = decode_context_param(zcontext);
	if (!context) {
		php_error_docref(NULL, E_WARNING, "Invalid stream/context parameter");
		RETURN_FALSE;
	}

	RETVAL_BOOL(parse_context_params(context, params) == SUCCESS);
}
/* }}} */

/* {{{ proto array stream_context_get_params(resource context|resource stream)
   Get parameters of a file context */
PHP_FUNCTION(stream_context_get_params)
{
	zval *zcontext;
	php_stream_context *context;

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &zcontext) == FAILURE) {
		RETURN_FALSE;
	}
#else
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_RESOURCE(zcontext)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
#endif

	context = decode_context_param(zcontext);
	if (!context) {
		php_error_docref(NULL, E_WARNING, "Invalid stream/context parameter");
		RETURN_FALSE;
	}

	array_init(return_value);
	if (context->notifier && Z_TYPE(context->notifier->ptr) != IS_UNDEF && context->notifier->func == user_space_stream_notifier) {
		add_assoc_zval_ex(return_value, "notification", sizeof("notification")-1, &context->notifier->ptr);
		if (Z_REFCOUNTED(context->notifier->ptr)) Z_ADDREF(context->notifier->ptr);
	}
	if (Z_REFCOUNTED(context->options)) Z_ADDREF(context->options);
	add_assoc_zval_ex(return_value, "options", sizeof("options")-1, &context->options);
}
/* }}} */

/* {{{ proto resource stream_context_get_default([array options])
   Get a handle on the default file/stream context and optionally set parameters */
PHP_FUNCTION(stream_context_get_default)
{
	zval *params = NULL;
	php_stream_context *context;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|a", &params) == FAILURE) {
		RETURN_FALSE;
	}

	if (FG(default_context) == NULL) {
		FG(default_context) = php_stream_context_alloc();
	}
	context = FG(default_context);

	if (params) {
		parse_context_options(context, params);
	}

	php_stream_context_to_zval(context, return_value);
}
/* }}} */

/* {{{ proto resource stream_context_set_default(array options)
   Set default file/stream context, returns the context as a resource */
PHP_FUNCTION(stream_context_set_default)
{
	zval *options = NULL;
	php_stream_context *context;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "a", &options) == FAILURE) {
		return;
	}

	if (FG(default_context) == NULL) {
		FG(default_context) = php_stream_context_alloc();
	}
	context = FG(default_context);

	parse_context_options(context, options);

	php_stream_context_to_zval(context, return_value);
}
/* }}} */

/* {{{ proto resource stream_context_create([array options[, array params]])
   Create a file context and optionally set parameters */
PHP_FUNCTION(stream_context_create)
{
	zval *options = NULL, *params = NULL;
	php_stream_context *context;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|a!a!", &options, &params) == FAILURE) {
		RETURN_FALSE;
	}

	context = php_stream_context_alloc();

	if (options) {
		parse_context_options(context, options);
	}

	if (params) {
		parse_context_params(context, params);
	}

	RETURN_RES(context->res);
}
/* }}} */

/* {{{ streams filter functions */
static void apply_filter_to_stream(int append, INTERNAL_FUNCTION_PARAMETERS)
{
	zval *zstream;
	php_stream *stream;
	char *filtername;
	size_t filternamelen;
	zend_long read_write = 0;
	zval *filterparams = NULL;
	php_stream_filter *filter = NULL;
	int ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rs|lz", &zstream,
				&filtername, &filternamelen, &read_write, &filterparams) == FAILURE) {
		RETURN_FALSE;
	}

	php_stream_from_zval(stream, zstream);

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
		filter = php_stream_filter_create(filtername, filterparams, php_stream_is_persistent(stream));
		if (filter == NULL) {
			RETURN_FALSE;
		}

		if (append) {
			ret = php_stream_filter_append_ex(&stream->readfilters, filter);
		} else {
			ret = php_stream_filter_prepend_ex(&stream->readfilters, filter);
		}
		if (ret != SUCCESS) {
			php_stream_filter_remove(filter, 1);
			RETURN_FALSE;
		}
	}

	if (read_write & PHP_STREAM_FILTER_WRITE) {
		filter = php_stream_filter_create(filtername, filterparams, php_stream_is_persistent(stream));
		if (filter == NULL) {
			RETURN_FALSE;
		}

		if (append) {
			ret = php_stream_filter_append_ex(&stream->writefilters, filter);
		} else {
			ret = php_stream_filter_prepend_ex(&stream->writefilters, filter);
		}
		if (ret != SUCCESS) {
			php_stream_filter_remove(filter, 1);
			RETURN_FALSE;
		}
	}

	if (filter) {
		filter->res = zend_register_resource(filter, php_file_le_stream_filter());
		GC_REFCOUNT(filter->res)++;
		RETURN_RES(filter->res);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto resource stream_filter_prepend(resource stream, string filtername[, int read_write[, string filterparams]])
   Prepend a filter to a stream */
PHP_FUNCTION(stream_filter_prepend)
{
	apply_filter_to_stream(0, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto resource stream_filter_append(resource stream, string filtername[, int read_write[, string filterparams]])
   Append a filter to a stream */
PHP_FUNCTION(stream_filter_append)
{
	apply_filter_to_stream(1, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto bool stream_filter_remove(resource stream_filter)
	Flushes any data in the filter's internal buffer, removes it from the chain, and frees the resource */
PHP_FUNCTION(stream_filter_remove)
{
	zval *zfilter;
	php_stream_filter *filter;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &zfilter) == FAILURE) {
		RETURN_FALSE;
	}

	filter = zend_fetch_resource(Z_RES_P(zfilter), NULL, php_file_le_stream_filter());
	if (!filter) {
		php_error_docref(NULL, E_WARNING, "Invalid resource given, not a stream filter");
		RETURN_FALSE;
	}

	if (php_stream_filter_flush(filter, 1) == FAILURE) {
		php_error_docref(NULL, E_WARNING, "Unable to flush filter, not removing");
		RETURN_FALSE;
	}

	if (zend_list_close(Z_RES_P(zfilter)) == FAILURE) {
		php_error_docref(NULL, E_WARNING, "Could not invalidate filter, not removing");
		RETURN_FALSE;
	} else {
		php_stream_filter_remove(filter, 1);
		RETURN_TRUE;
	}
}
/* }}} */

/* {{{ proto string stream_get_line(resource stream, int maxlen [, string ending])
   Read up to maxlen bytes from a stream or until the ending string is found */
PHP_FUNCTION(stream_get_line)
{
	char *str = NULL;
	size_t str_len = 0;
	zend_long max_length;
	zval *zstream;
	zend_string *buf;
	php_stream *stream;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rl|s", &zstream, &max_length, &str, &str_len) == FAILURE) {
		RETURN_FALSE;
	}

	if (max_length < 0) {
		php_error_docref(NULL, E_WARNING, "The maximum allowed length must be greater than or equal to zero");
		RETURN_FALSE;
	}
	if (!max_length) {
		max_length = PHP_SOCK_CHUNK_SIZE;
	}

	php_stream_from_zval(stream, zstream);

	if ((buf = php_stream_get_record(stream, max_length, str, str_len))) {
		RETURN_STR(buf);
	} else {
		RETURN_FALSE;
	}
}

/* }}} */

/* {{{ proto bool stream_set_blocking(resource socket, bool mode)
   Set blocking/non-blocking mode on a socket or stream */
PHP_FUNCTION(stream_set_blocking)
{
	zval *zstream;
	zend_bool block;
	php_stream *stream;

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rb", &zstream, &block) == FAILURE) {
		return;
	}
#else
	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_RESOURCE(zstream)
		Z_PARAM_BOOL(block)
	ZEND_PARSE_PARAMETERS_END();
#endif

	php_stream_from_zval(stream, zstream);

	if (php_stream_set_option(stream, PHP_STREAM_OPTION_BLOCKING, block, NULL) == -1) {
		RETURN_FALSE;
	}

	RETURN_TRUE;
}

/* }}} */

/* {{{ proto bool stream_set_timeout(resource stream, int seconds [, int microseconds])
   Set timeout on stream read to seconds + microseonds */
#if HAVE_SYS_TIME_H || defined(PHP_WIN32)
PHP_FUNCTION(stream_set_timeout)
{
	zval *socket;
	zend_long seconds, microseconds = 0;
	struct timeval t;
	php_stream *stream;
	int argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc, "rl|l", &socket, &seconds, &microseconds) == FAILURE) {
		return;
	}

	php_stream_from_zval(stream, socket);

#ifdef PHP_WIN32
	t.tv_sec = (long)seconds;

	if (argc == 3) {
		t.tv_usec = (long)(microseconds % 1000000);
		t.tv_sec +=(long)(microseconds / 1000000);
	} else {
		t.tv_usec = 0;
	}
#else
	t.tv_sec = seconds;

	if (argc == 3) {
		t.tv_usec = microseconds % 1000000;
		t.tv_sec += microseconds / 1000000;
	} else {
		t.tv_usec = 0;
	}
#endif

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
	zval *arg1;
	int ret;
	zend_long arg2;
	size_t buff;
	php_stream *stream;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rl", &arg1, &arg2) == FAILURE) {
		RETURN_FALSE;
	}

	php_stream_from_zval(stream, arg1);

	buff = arg2;

	/* if buff is 0 then set to non-buffered */
	if (buff == 0) {
		ret = php_stream_set_option(stream, PHP_STREAM_OPTION_WRITE_BUFFER, PHP_STREAM_BUFFER_NONE, NULL);
	} else {
		ret = php_stream_set_option(stream, PHP_STREAM_OPTION_WRITE_BUFFER, PHP_STREAM_BUFFER_FULL, &buff);
	}

	RETURN_LONG(ret == 0 ? 0 : EOF);
}
/* }}} */

/* {{{ proto int stream_set_chunk_size(resource fp, int chunk_size)
   Set the stream chunk size */
PHP_FUNCTION(stream_set_chunk_size)
{
	int			ret;
	zend_long		csize;
	zval		*zstream;
	php_stream	*stream;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rl", &zstream, &csize) == FAILURE) {
		RETURN_FALSE;
	}

	if (csize <= 0) {
		php_error_docref(NULL, E_WARNING, "The chunk size must be a positive integer, given " ZEND_LONG_FMT, csize);
		RETURN_FALSE;
	}
	/* stream.chunk_size is actually a size_t, but php_stream_set_option
	 * can only use an int to accept the new value and return the old one.
	 * In any case, values larger than INT_MAX for a chunk size make no sense.
	 */
	if (csize > INT_MAX) {
		php_error_docref(NULL, E_WARNING, "The chunk size cannot be larger than %d", INT_MAX);
		RETURN_FALSE;
	}

	php_stream_from_zval(stream, zstream);

	ret = php_stream_set_option(stream, PHP_STREAM_OPTION_SET_CHUNK_SIZE, (int)csize, NULL);

	RETURN_LONG(ret > 0 ? (zend_long)ret : (zend_long)EOF);
}
/* }}} */

/* {{{ proto int stream_set_read_buffer(resource fp, int buffer)
   Set file read buffer */
PHP_FUNCTION(stream_set_read_buffer)
{
	zval *arg1;
	int ret;
	zend_long arg2;
	size_t buff;
	php_stream *stream;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rl", &arg1, &arg2) == FAILURE) {
		RETURN_FALSE;
	}

	php_stream_from_zval(stream, arg1);

	buff = arg2;

	/* if buff is 0 then set to non-buffered */
	if (buff == 0) {
		ret = php_stream_set_option(stream, PHP_STREAM_OPTION_READ_BUFFER, PHP_STREAM_BUFFER_NONE, NULL);
	} else {
		ret = php_stream_set_option(stream, PHP_STREAM_OPTION_READ_BUFFER, PHP_STREAM_BUFFER_FULL, &buff);
	}

	RETURN_LONG(ret == 0 ? 0 : EOF);
}
/* }}} */

/* {{{ proto int stream_socket_enable_crypto(resource stream, bool enable [, int cryptokind [, resource sessionstream]])
   Enable or disable a specific kind of crypto on the stream */
PHP_FUNCTION(stream_socket_enable_crypto)
{
	zend_long cryptokind = 0;
	zval *zstream, *zsessstream = NULL;
	php_stream *stream, *sessstream = NULL;
	zend_bool enable, cryptokindnull;
	int ret;

#ifdef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rb|l!r", &zstream, &enable, &cryptokind, &cryptokindnull, &zsessstream) == FAILURE) {
		RETURN_FALSE;
	}
#else
	ZEND_PARSE_PARAMETERS_START(2, 4)
		Z_PARAM_RESOURCE(zstream)
		Z_PARAM_BOOL(enable)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_EX(cryptokind, cryptokindnull, 1, 0)
		Z_PARAM_RESOURCE(zsessstream)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
#endif

	php_stream_from_zval(stream, zstream);

	if (enable) {
		if (ZEND_NUM_ARGS() < 3 || cryptokindnull) {
			zval *val;

			if (!GET_CTX_OPT(stream, "ssl", "crypto_method", val)) {
				php_error_docref(NULL, E_WARNING, "When enabling encryption you must specify the crypto type");
				RETURN_FALSE;
			}

			cryptokind = Z_LVAL_P(val);
		}

		if (zsessstream) {
			php_stream_from_zval(sessstream, zsessstream);
		}

		if (php_stream_xport_crypto_setup(stream, cryptokind, sessstream) < 0) {
			RETURN_FALSE;
		}
	}

	ret = php_stream_xport_crypto_enable(stream, enable);
	switch (ret) {
		case -1:
			RETURN_FALSE;

		case 0:
			RETURN_LONG(0);

		default:
			RETURN_TRUE;
	}
}
/* }}} */

/* {{{ proto string stream_resolve_include_path(string filename)
Determine what file will be opened by calls to fopen() with a relative path */
PHP_FUNCTION(stream_resolve_include_path)
{
	char *filename;
	size_t filename_len;
	zend_string *resolved_path;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "p", &filename, &filename_len) == FAILURE) {
		return;
	}

	resolved_path = zend_resolve_path(filename, (int)filename_len);

	if (resolved_path) {
		RETURN_STR(resolved_path);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool stream_is_local(resource stream|string url) U
*/
PHP_FUNCTION(stream_is_local)
{
	zval *zstream;
	php_stream *stream = NULL;
	php_stream_wrapper *wrapper = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &zstream) == FAILURE) {
		RETURN_FALSE;
	}

	if (Z_TYPE_P(zstream) == IS_RESOURCE) {
		php_stream_from_zval(stream, zstream);
		if (stream == NULL) {
			RETURN_FALSE;
		}
		wrapper = stream->wrapper;
	} else {
		convert_to_string_ex(zstream);

		wrapper = php_stream_locate_url_wrapper(Z_STRVAL_P(zstream), NULL, 0);
	}

	if (!wrapper) {
		RETURN_FALSE;
	}

	RETURN_BOOL(wrapper->is_url==0);
}
/* }}} */

/* {{{ proto bool stream_supports_lock(resource stream)
   Tells whether the stream supports locking through flock(). */
PHP_FUNCTION(stream_supports_lock)
{
	php_stream *stream;
	zval *zsrc;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &zsrc) == FAILURE) {
		RETURN_FALSE;
	}

	php_stream_from_zval(stream, zsrc);

	if (!php_stream_supports_lock(stream)) {
		RETURN_FALSE;
	}

	RETURN_TRUE;
}

#ifdef HAVE_SHUTDOWN
/* {{{ proto int stream_socket_shutdown(resource stream, int how)
	causes all or part of a full-duplex connection on the socket associated
	with stream to be shut down.  If how is SHUT_RD,  further receptions will
	be disallowed. If how is SHUT_WR, further transmissions will be disallowed.
	If how is SHUT_RDWR,  further  receptions and transmissions will be
	disallowed. */
PHP_FUNCTION(stream_socket_shutdown)
{
	zend_long how;
	zval *zstream;
	php_stream *stream;

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rl", &zstream, &how) == FAILURE) {
		RETURN_FALSE;
	}
#else
	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_RESOURCE(zstream)
		Z_PARAM_LONG(how)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
#endif

	if (how != STREAM_SHUT_RD &&
	    how != STREAM_SHUT_WR &&
	    how != STREAM_SHUT_RDWR) {
		php_error_docref(NULL, E_WARNING, "Second parameter $how needs to be one of STREAM_SHUT_RD, STREAM_SHUT_WR or STREAM_SHUT_RDWR");
		RETURN_FALSE;
	}

	php_stream_from_zval(stream, zstream);

	RETURN_BOOL(php_stream_xport_shutdown(stream, (stream_shutdown_t)how) == 0);
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

