/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2008 The PHP Group                                |
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
typedef unsigned __int64 php_timeout_ull;
#endif

static php_stream_context *decode_context_param(zval *contextresource TSRMLS_DC);

/* Streams based network functions */

#if HAVE_SOCKETPAIR
/* {{{ proto array stream_socket_pair(int domain, int type, int protocol) U
   Creates a pair of connected, indistinguishable socket streams */
PHP_FUNCTION(stream_socket_pair)
{
	long domain, type, protocol;
	php_stream *s1, *s2;
	int pair[2];

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lll",
			&domain, &type, &protocol)) {
		RETURN_FALSE;
	}

	if (0 != socketpair(domain, type, protocol, pair)) {
		char errbuf[256];
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "failed to create sockets: [%d]: %s",
			php_socket_errno(), php_socket_strerror(php_socket_errno(), errbuf, sizeof(errbuf)));
		RETURN_FALSE;
	}

	array_init(return_value);

	s1 = php_stream_sock_open_from_socket(pair[0], 0);
	s2 = php_stream_sock_open_from_socket(pair[1], 0);

	add_next_index_resource(return_value, php_stream_get_resource_id(s1));
	add_next_index_resource(return_value, php_stream_get_resource_id(s2));
}
/* }}} */
#endif

/* {{{ proto resource stream_socket_client(string remoteaddress [, long &errcode [, string &errstring [, double timeout [, long flags [, resource context]]]]]) U
   Open a client connection to a remote address */
PHP_FUNCTION(stream_socket_client)
{
	char *host;
	int host_len;
	zval *zerrno = NULL, *zerrstr = NULL, *zcontext = NULL;
	double timeout = FG(default_socket_timeout);
	php_timeout_ull conv;
	struct timeval tv;
	char *hashkey = NULL;
	php_stream *stream = NULL;
	int err;
	long flags = PHP_STREAM_CLIENT_CONNECT;
	char *errstr = NULL;
	php_stream_context *context = NULL;

	RETVAL_FALSE;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|zzd!lr", &host, &host_len, &zerrno, &zerrstr, &timeout, &flags, &zcontext) == FAILURE) {
		RETURN_FALSE;
	}
	
	context = php_stream_context_from_zval(zcontext, flags & PHP_FILE_NO_DEFAULT_CONTEXT);

	if (context) {
		zend_list_addref(context->rsrc_id);
	}

	if (flags & PHP_STREAM_CLIENT_PERSISTENT) {
		spprintf(&hashkey, 0, "stream_socket_client__%s", host);
	}
	
	/* prepare the timeout value for use */
	conv = (php_timeout_ull) (timeout * 1000000.0);
	tv.tv_sec = conv / 1000000;
	tv.tv_usec = conv % 1000000;

	if (zerrno)	{
		zval_dtor(zerrno);
		ZVAL_LONG(zerrno, 0);
	}
	if (zerrstr) {
		zval_dtor(zerrstr);
		ZVAL_EMPTY_TEXT(zerrstr);
	}

	stream = php_stream_xport_create(host, host_len, REPORT_ERRORS,
			STREAM_XPORT_CLIENT | (flags & PHP_STREAM_CLIENT_CONNECT ? STREAM_XPORT_CONNECT : 0) |
			(flags & PHP_STREAM_CLIENT_ASYNC_CONNECT ? STREAM_XPORT_CONNECT_ASYNC : 0),
			hashkey, &tv, context, &errstr, &err);
		

	if (stream == NULL) {
		/* host might contain binary characters */
		char *quoted_host = php_addslashes(host, host_len, NULL, 0 TSRMLS_CC);
		
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "unable to connect to %s (%s)", quoted_host, errstr == NULL ? "Unknown error" : errstr);
		efree(quoted_host);
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
			/* errstr is emalloc'd, either assign it to STRVAL if !UG(unicode)
			 * Or free it post-conversion if UG(unicode) */
			zval_dtor(zerrstr);
			ZVAL_RT_STRING(zerrstr, errstr, ZSTR_AUTOFREE);
		} else if (errstr) {
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

/* {{{ proto resource stream_socket_server(string localaddress [, long &errcode [, string &errstring [, long flags [, resource context]]]]) U
   Create a server socket bound to localaddress */
PHP_FUNCTION(stream_socket_server)
{
	char *host;
	int host_len;
	zval *zerrno = NULL, *zerrstr = NULL, *zcontext = NULL;
	php_stream *stream = NULL;
	int err = 0;
	long flags = STREAM_XPORT_BIND | STREAM_XPORT_LISTEN;
	char *errstr = NULL;
	php_stream_context *context = NULL;

	RETVAL_FALSE;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|zzlr", &host, &host_len, &zerrno, &zerrstr, &flags, &zcontext) == FAILURE) {
		RETURN_FALSE;
	}
	
	context = php_stream_context_from_zval(zcontext, flags & PHP_FILE_NO_DEFAULT_CONTEXT);
	
	if (context) {
		zend_list_addref(context->rsrc_id);
	}

	if (zerrno)	{
		zval_dtor(zerrno);
		ZVAL_LONG(zerrno, 0);
	}
	if (zerrstr) {
		zval_dtor(zerrstr);
		ZVAL_EMPTY_TEXT(zerrstr);
	}

	stream = php_stream_xport_create(host, host_len, REPORT_ERRORS,
			STREAM_XPORT_SERVER | flags,
			NULL, NULL, context, &errstr, &err);
			
	if (stream == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "unable to connect to %s (%s)", host, errstr == NULL ? "Unknown error" : errstr);
	}
	
	if (stream == NULL)	{
		if (zerrno) {
			zval_dtor(zerrno);
			ZVAL_LONG(zerrno, err);
		}
		if (zerrstr && errstr) {
			/* errstr is emalloc'd, either assign it to STRVAL if !UG(unicode)
			 * Or free it post-conversion if UG(unicode) */
			zval_dtor(zerrstr);
			ZVAL_RT_STRING(zerrstr, errstr, ZSTR_AUTOFREE);
		} else if (errstr) {
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

/* {{{ proto resource stream_socket_accept(resource serverstream, [ double timeout [, string &peername ]]) U
   Accept a client connection from a server socket */
PHP_FUNCTION(stream_socket_accept)
{
	double timeout = FG(default_socket_timeout);
	zval *zpeername = NULL;
	char *peername = NULL;
	int peername_len;
	php_timeout_ull conv;
	struct timeval tv;
	php_stream *stream = NULL, *clistream = NULL;
	zval *zstream;

	char *errstr = NULL;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|dz", &zstream, &timeout, &zpeername) == FAILURE) {
		RETURN_FALSE;
	}
	
	php_stream_from_zval(stream, &zstream);
	
	/* prepare the timeout value for use */
	conv = (php_timeout_ull) (timeout * 1000000.0);
	tv.tv_sec = conv / 1000000;
	tv.tv_usec = conv % 1000000;

	if (zpeername) {
		zval_dtor(zpeername);
		ZVAL_NULL(zpeername);
	}

	if (0 == php_stream_xport_accept(stream, &clistream,
				zpeername ? &peername : NULL,
				zpeername ? &peername_len : NULL,
				NULL, NULL,
				&tv, &errstr
				TSRMLS_CC) && clistream) {
		
		if (peername) {
			ZVAL_RT_STRINGL(zpeername, peername, peername_len, ZSTR_AUTOFREE);
		}
		php_stream_to_zval(clistream, return_value);
	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "accept failed: %s", errstr ? errstr : "Unknown error");
		RETVAL_FALSE;
	}

	if (errstr) {
		efree(errstr);
	}
}
/* }}} */

/* {{{ proto string stream_socket_get_name(resource stream, bool want_peer) U
   Returns either the locally bound or remote name for a socket stream */
PHP_FUNCTION(stream_socket_get_name)
{
	php_stream *stream;
	zval *zstream;
	zend_bool want_peer;
	char *name = NULL;
	int name_len;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rb", &zstream, &want_peer) == FAILURE) {
		RETURN_FALSE;
	}
	
	php_stream_from_zval(stream, &zstream);

	if (0 != php_stream_xport_get_name(stream, want_peer,
				&name,
				&name_len,
				NULL, NULL
				TSRMLS_CC)) {
		RETURN_FALSE;
	}

	RETURN_RT_STRINGL(name, name_len, ZSTR_AUTOFREE);
}
/* }}} */

/* {{{ proto long stream_socket_sendto(resouce stream, string data [, long flags [, string target_addr]]) U
   Send data to a socket stream.  If target_addr is specified it must be in dotted quad (or [ipv6]) format */
PHP_FUNCTION(stream_socket_sendto)
{
	php_stream *stream;
	zval *zstream;
	long flags = 0;
	char *data, *target_addr = NULL;
	int datalen, target_addr_len = 0;
	php_sockaddr_storage sa;
	socklen_t sl = 0;
	
	/* Typically we'll be sending binary data, but implicit RT conversion from unicode is acceptable */
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|ls", &zstream, &data, &datalen, &flags, &target_addr, &target_addr_len) == FAILURE) {
		RETURN_FALSE;
	}
	php_stream_from_zval(stream, &zstream);

	if (target_addr_len) {
		/* parse the address */
		if (FAILURE == php_network_parse_network_address_with_port(target_addr, target_addr_len, (struct sockaddr*)&sa, &sl TSRMLS_CC)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Failed to parse `%s' into a valid network address", target_addr);
			RETURN_FALSE;
		}
	}

	RETURN_LONG(php_stream_xport_sendto(stream, data, datalen, flags, target_addr ? &sa : NULL, sl TSRMLS_CC));
}
/* }}} */

/* {{{ proto string stream_socket_recvfrom(resource stream, long amount [, long flags [, string &remote_addr]]) U
   Receives data from a socket stream */
PHP_FUNCTION(stream_socket_recvfrom)
{
	php_stream *stream;
	zval *zstream, *zremote = NULL;
	char *remote_addr = NULL;
	int remote_addr_len;
	long to_read = 0;
	char *read_buf;
	long flags = 0;
	int recvd;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl|lz", &zstream, &to_read, &flags, &zremote) == FAILURE) {
		RETURN_FALSE;
	}
	
	php_stream_from_zval(stream, &zstream);

	if (zremote) {
		zval_dtor(zremote);
		ZVAL_NULL(zremote);
	}

	if (to_read <= 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Length parameter must be greater than 0");
		RETURN_FALSE;
	}
	
	read_buf = safe_emalloc(1, to_read, 1);
	
	recvd = php_stream_xport_recvfrom(stream, read_buf, to_read, flags, NULL, NULL,
			zremote ? &remote_addr : NULL,
			zremote ? &remote_addr_len : NULL
			TSRMLS_CC);

	if (recvd >= 0) {
		if (zremote) {
			ZVAL_RT_STRINGL(zremote, remote_addr, remote_addr_len, ZSTR_AUTOFREE);
		}
		read_buf[recvd] = '\0';

		RETURN_RT_STRINGL(read_buf, recvd, ZSTR_AUTOFREE);
	}

	efree(read_buf);
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto string stream_get_contents(resource source [, long maxlen [, long offset]]) U
   Reads all remaining bytes (or up to maxlen bytes) from a stream and returns them as a string. */
PHP_FUNCTION(stream_get_contents)
{
	php_stream *stream;
	zval *zsrc;
	long maxlen = PHP_STREAM_COPY_ALL, pos = 0, real_maxlen;
	int len;
	void *contents = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|ll", &zsrc, &maxlen, &pos) == FAILURE) {
		RETURN_FALSE;
	}

	php_stream_from_zval(stream, &zsrc);

	if (pos > 0 && php_stream_seek(stream, pos, SEEK_SET) < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Failed to seek to position %ld in the stream", pos);
		RETURN_FALSE;
	}

	if (maxlen <= 0 || stream->readbuf_type == IS_STRING) {
		real_maxlen = maxlen;
	} else {
		/* Allows worst case scenario of each input char being turned into two UChars
		 * UTODO: Have this take converter into account, since many never generate surrogate pairs */
		real_maxlen = maxlen * 2;
	}

	len = php_stream_copy_to_mem_ex(stream, stream->readbuf_type, &contents, real_maxlen, maxlen, 0);

	if (stream->readbuf_type == IS_STRING) {
		if (len > 0) {
			RETVAL_STRINGL(contents, len, 0);
		} else {
			if (contents) {
				efree(contents);
			}
			RETVAL_EMPTY_STRING();
		}
	} else {
		if (len > 0) {
			RETVAL_UNICODEL(contents, len, 0);
		} else {
			if (contents) {
				efree(contents);
			}
			RETVAL_EMPTY_UNICODE();
		}
	}
}
/* }}} */

/* {{{ proto long stream_copy_to_stream(resource source, resource dest [, long maxlen [, long pos]]) U
   Reads up to maxlen bytes from source stream and writes them to dest stream. */
PHP_FUNCTION(stream_copy_to_stream)
{
	php_stream *src, *dest;
	zval *zsrc, *zdest;
	long maxlen = PHP_STREAM_COPY_ALL, pos = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rr|ll", &zsrc, &zdest, &maxlen, &pos) == FAILURE) {
		RETURN_FALSE;
	}

	php_stream_from_zval(src, &zsrc);
	php_stream_from_zval(dest, &zdest);

	if (pos > 0 && php_stream_seek(src, pos, SEEK_SET) < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Failed to seek to position %ld in the stream", pos);
		RETURN_FALSE;
	}

	RETURN_LONG(php_stream_copy_to_stream(src, dest, maxlen));
}
/* }}} */

/* {{{ proto array stream_get_meta_data(resource fp) U
    Retrieves header/meta data from streams/file pointers */
PHP_FUNCTION(stream_get_meta_data)
{
	zval *arg1;
	php_stream *stream;
	zval *newval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &arg1) == FAILURE) {
		return;
	}
	php_stream_from_zval(stream, &arg1);

	array_init(return_value);
	
	if (stream->wrapperdata) {
		MAKE_STD_ZVAL(newval);
		*newval = *(stream->wrapperdata);
		zval_copy_ctor(newval);
		INIT_PZVAL(newval);

		add_ascii_assoc_zval(return_value, "wrapper_data", newval);
	}
	if (stream->wrapper) {
		add_ascii_assoc_rt_string(return_value, "wrapper_type", (char *)stream->wrapper->wops->label, ZSTR_DUPLICATE);
	}
	add_ascii_assoc_rt_string(return_value, "stream_type", (char *)stream->ops->label, ZSTR_DUPLICATE);

	add_ascii_assoc_rt_string(return_value, "mode", stream->mode, ZSTR_DUPLICATE);
	
	if (stream->readfilters.head) {
		php_stream_filter *filter;
		
		MAKE_STD_ZVAL(newval);
		array_init(newval);
		
		for (filter = stream->readfilters.head; filter != NULL; filter = filter->next) {
			add_next_index_rt_string(newval, filter->name, ZSTR_DUPLICATE);
		}

		add_ascii_assoc_zval(return_value, "read_filters", newval);
	}

	if (stream->writefilters.head) {
		php_stream_filter *filter;
		
		MAKE_STD_ZVAL(newval);
		array_init(newval);
		
		for (filter = stream->writefilters.head; filter != NULL; filter = filter->next) {
			add_next_index_rt_string(newval, filter->name, ZSTR_DUPLICATE);
		}

		add_ascii_assoc_zval(return_value, "write_filters", newval);
	}
	
	if (stream->readbuf_type == IS_UNICODE) {
		int readbuf_len = u_countChar32(stream->readbuf.u + stream->readpos, stream->writepos - stream->readpos);
		add_ascii_assoc_long(return_value, "unread_bytes", UBYTES(stream->writepos - stream->readpos));
		add_ascii_assoc_long(return_value, "unread_chars", readbuf_len);
	} else { /* IS_STRING */
		add_ascii_assoc_long(return_value, "unread_bytes", stream->writepos - stream->readpos);
		add_ascii_assoc_long(return_value, "unread_chars", stream->writepos - stream->readpos);
	}

	add_ascii_assoc_bool(return_value, "seekable", (stream->ops->seek) && (stream->flags & PHP_STREAM_FLAG_NO_SEEK) == 0);
	if (stream->orig_path) {
		if (UG(unicode)) {
			UChar *decoded_path = NULL;
			int decoded_path_len = 0;

			if (SUCCESS == php_stream_path_decode(stream->wrapper, &decoded_path, &decoded_path_len, stream->orig_path, strlen(stream->orig_path), REPORT_ERRORS, stream->context)) {
				add_ascii_assoc_unicodel(return_value, "uri", decoded_path, decoded_path_len, 0);
			} else {
				add_ascii_assoc_null(return_value, "uri");
			}
		} else {
			add_ascii_assoc_string(return_value, "uri", stream->orig_path, 1);
		}
	}

	if (!php_stream_populate_meta_data(stream, return_value)) {
		add_ascii_assoc_bool(return_value, "timed_out", 0);
		add_ascii_assoc_bool(return_value, "blocked", 1);
		add_ascii_assoc_bool(return_value, "eof", php_stream_eof(stream));
	}

}
/* }}} */

/* {{{ proto array stream_get_transports() U
   Retrieves list of registered socket transports */
PHP_FUNCTION(stream_get_transports)
{
	HashTable *stream_xport_hash;
	zstr stream_xport;
	uint stream_xport_len;
	ulong num_key;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if ((stream_xport_hash = php_stream_xport_get_hash())) {
		array_init(return_value);
		zend_hash_internal_pointer_reset(stream_xport_hash);
		while (zend_hash_get_current_key_ex(stream_xport_hash,
					&stream_xport, &stream_xport_len,
					&num_key, 0, NULL) == HASH_KEY_IS_STRING) {
			add_next_index_rt_stringl(return_value, stream_xport.s, stream_xport_len - 1, ZSTR_DUPLICATE);
			zend_hash_move_forward(stream_xport_hash);
		}
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto array stream_get_wrappers() U
    Retrieves list of registered stream wrappers */
PHP_FUNCTION(stream_get_wrappers)
{
	HashTable *url_stream_wrappers_hash;
	zstr stream_protocol;
	uint key_flags, stream_protocol_len = 0;
	ulong num_key;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if ((url_stream_wrappers_hash = php_stream_get_url_stream_wrappers_hash())) {
		array_init(return_value);
		for(zend_hash_internal_pointer_reset(url_stream_wrappers_hash);
			(key_flags = zend_hash_get_current_key_ex(url_stream_wrappers_hash, &stream_protocol, &stream_protocol_len, &num_key, 0, NULL)) != HASH_KEY_NON_EXISTANT;
			zend_hash_move_forward(url_stream_wrappers_hash)) {
				if (key_flags == HASH_KEY_IS_STRING) {
					add_next_index_rt_stringl(return_value, stream_protocol.s, stream_protocol_len - 1, ZSTR_DUPLICATE);
				}
		}
	} else {
		RETURN_FALSE;
	}

}
/* }}} */

/* {{{ stream_select related functions */
static int stream_array_to_fd_set(zval *stream_array, fd_set *fds, php_socket_t *max_fd TSRMLS_DC)
{
	zval **elem;
	php_stream *stream;
	php_socket_t this_fd;
	int cnt = 0;

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
		if (SUCCESS == php_stream_cast(stream, PHP_STREAM_AS_FD_FOR_SELECT | PHP_STREAM_CAST_INTERNAL, (void*)&this_fd, 1) && this_fd >= 0) {
			
			PHP_SAFE_FD_SET(this_fd, fds);

			if (this_fd > *max_fd) {
				*max_fd = this_fd;
			}
			cnt++;
		}
	}
	return cnt ? 1 : 0;
}

static int stream_array_from_fd_set(zval *stream_array, fd_set *fds TSRMLS_DC)
{
	zval **elem, **dest_elem;
	php_stream *stream;
	HashTable *new_hash;
	php_socket_t this_fd;
	int ret = 0;

	if (Z_TYPE_P(stream_array) != IS_ARRAY) {
		return 0;
	}
	ALLOC_HASHTABLE(new_hash);
	zend_hash_init(new_hash, zend_hash_num_elements(Z_ARRVAL_P(stream_array)), NULL, ZVAL_PTR_DTOR, 0);
	
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
		if (SUCCESS == php_stream_cast(stream, PHP_STREAM_AS_FD_FOR_SELECT | PHP_STREAM_CAST_INTERNAL, (void*)&this_fd, 1) && this_fd >= 0) {
			if (PHP_SAFE_FD_ISSET(this_fd, fds)) {
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
	zend_hash_init(new_hash, zend_hash_num_elements(Z_ARRVAL_P(stream_array)), NULL, ZVAL_PTR_DTOR, 0);
	
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

/* {{{ proto int stream_select(array &read_streams, array &write_streams, array &except_streams, int tv_sec[, int tv_usec]) U
   Runs the select() system call on the sets of streams with a timeout specified by tv_sec and tv_usec */
PHP_FUNCTION(stream_select)
{
	zval			*r_array, *w_array, *e_array, **sec = NULL;
	struct timeval	tv;
	struct timeval *tv_p = NULL;
	fd_set			rfds, wfds, efds;
	php_socket_t	max_fd = 0;
	int				retval, sets = 0;
	long			usec = 0;
	int				set_count, max_set_count = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a!a!a!Z!|l", &r_array, &w_array, &e_array, &sec, &usec) == FAILURE)
		return;

	FD_ZERO(&rfds);
	FD_ZERO(&wfds);
	FD_ZERO(&efds);

	if (r_array != NULL) {
		set_count = stream_array_to_fd_set(r_array, &rfds, &max_fd TSRMLS_CC);
		if (set_count > max_set_count)
			max_set_count = set_count;
		sets += set_count;
	}
	
	if (w_array != NULL) {
		set_count = stream_array_to_fd_set(w_array, &wfds, &max_fd TSRMLS_CC);
		if (set_count > max_set_count)
			max_set_count = set_count;
		sets += set_count;
	}

	if (e_array != NULL) {
		set_count = stream_array_to_fd_set(e_array, &efds, &max_fd TSRMLS_CC);
		if (set_count > max_set_count)
			max_set_count = set_count;
		sets += set_count;
	}

	if (!sets) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "No stream arrays were passed");
		RETURN_FALSE;
	}

	PHP_SAFE_MAX_FD(max_fd, max_set_count);

	/* If seconds is not set to null, build the timeval, else we wait indefinitely */
	if (sec != NULL) {
		convert_to_long_ex(sec);

		if (Z_LVAL_PP(sec) < 0) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "The seconds parameter must be greater than 0");
			RETURN_FALSE;
		} else if (usec < 0) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "The microseconds parameter must be greater than 0");
			RETURN_FALSE;
		}

		/* Solaris + BSD do not like microsecond values which are >= 1 sec */
		if (usec > 999999) {
			tv.tv_sec = Z_LVAL_PP(sec) + (usec / 1000000);
			tv.tv_usec = usec % 1000000;			
		} else {
			tv.tv_sec = Z_LVAL_PP(sec);
			tv.tv_usec = usec;
		}

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
		ZVAL_RT_STRING(ps[2], xmsg, ZSTR_AUTOFREE);
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

static void user_space_stream_notifier_dtor(php_stream_notifier *notifier)
{
	if (notifier && notifier->ptr) {
		zval_ptr_dtor((zval **)&(notifier->ptr));
		notifier->ptr = NULL;
	}
}

static int parse_context_options(php_stream_context *context, zval *options TSRMLS_DC)
{
	HashPosition pos, opos;
	zval **wval, **oval;
	zstr wkey, okey;
	uint wkey_len, okey_len;
	int ret = SUCCESS;
	ulong num_key;
	
	zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(options), &pos);
	while (SUCCESS == zend_hash_get_current_data_ex(Z_ARRVAL_P(options), (void**)&wval, &pos)) {
		int wtype = zend_hash_get_current_key_ex(Z_ARRVAL_P(options), &wkey, &wkey_len, &num_key, 0, &pos);
		if (((HASH_KEY_IS_STRING == wtype) || (HASH_KEY_IS_UNICODE == wtype))
				&& Z_TYPE_PP(wval) == IS_ARRAY) {
			if (HASH_KEY_IS_UNICODE == wtype) {
				/* fold to string */
				UErrorCode errCode = 0;
				char *tmp;
				int tmp_len;

				zend_unicode_to_string_ex(ZEND_U_CONVERTER(UG(runtime_encoding_conv)), &tmp, &tmp_len, wkey.u, wkey_len, &errCode);
				wkey.s = tmp;
				wkey_len = tmp_len;
			}

			zend_hash_internal_pointer_reset_ex(Z_ARRVAL_PP(wval), &opos);
			while (SUCCESS == zend_hash_get_current_data_ex(Z_ARRVAL_PP(wval), (void**)&oval, &opos)) {
				int otype = zend_hash_get_current_key_ex(Z_ARRVAL_PP(wval), &okey, &okey_len, &num_key, 0, &opos);
				if (HASH_KEY_IS_UNICODE == otype) {
					/* fold to string */
					UErrorCode errCode = 0;
					char *tmp;
					int tmp_len;
	
					zend_unicode_to_string_ex(ZEND_U_CONVERTER(UG(runtime_encoding_conv)), &tmp, &tmp_len, okey.u, okey_len, &errCode);
					okey.s = tmp;
					okey_len = tmp_len;
					php_stream_context_set_option(context, wkey.s, okey.s, *oval);
					efree(okey.v);
				}
				if (HASH_KEY_IS_STRING == otype) {
					php_stream_context_set_option(context, wkey.s, okey.s, *oval);
				}
				zend_hash_move_forward_ex(Z_ARRVAL_PP(wval), &opos);
			}
			if (wtype == HASH_KEY_IS_UNICODE) {
				efree(wkey.v);
			}
		} else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "options should have the form [\"wrappername\"][\"optionname\"] = $value");
		}
		zend_hash_move_forward_ex(Z_ARRVAL_P(options), &pos);
	}

	return ret;
}

static int parse_context_params(php_stream_context *context, zval *params TSRMLS_DC)
{
	int ret = SUCCESS;
	zval **tmp;

	if (SUCCESS == zend_ascii_hash_find(Z_ARRVAL_P(params), "notification", sizeof("notification"), (void**)&tmp)) {
		
		if (context->notifier) {
			php_stream_notification_free(context->notifier);
			context->notifier = NULL;
		}

		context->notifier = php_stream_notification_alloc();
		context->notifier->func = user_space_stream_notifier;
		context->notifier->ptr = *tmp;
		Z_ADDREF_P(*tmp);
		context->notifier->dtor = user_space_stream_notifier_dtor;
	}
	if (SUCCESS == zend_ascii_hash_find(Z_ARRVAL_P(params), "options", sizeof("options"), (void**)&tmp)) {
		if (Z_TYPE_PP(tmp) == IS_ARRAY) {
			parse_context_options(context, *tmp TSRMLS_CC);
		} else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid stream/context parameter");
		}
	}
	if (SUCCESS == zend_ascii_hash_find(Z_ARRVAL_P(params), "encoding", sizeof("encoding"), (void**)&tmp)) {
		zval strval = **tmp;

		if (context->input_encoding) {
			efree(context->input_encoding);
		}
		if (context->output_encoding) {
			efree(context->output_encoding);
		}
		zval_copy_ctor(&strval);
		convert_to_string(&strval);
		context->input_encoding = Z_STRVAL(strval);
		context->output_encoding = estrdup(Z_STRVAL(strval));
	}
	if (SUCCESS == zend_ascii_hash_find(Z_ARRVAL_P(params), "input_encoding", sizeof("input_encoding"), (void**)&tmp)) {
		zval strval = **tmp;

		if (context->input_encoding) {
			efree(context->input_encoding);
		}

		zval_copy_ctor(&strval);
		convert_to_string(&strval);
		context->input_encoding = Z_STRVAL(strval);
	}
	if (SUCCESS == zend_ascii_hash_find(Z_ARRVAL_P(params), "output_encoding", sizeof("output_encoding"), (void**)&tmp)) {
		zval strval = **tmp;

		if (context->output_encoding) {
			efree(context->output_encoding);
		}

		zval_copy_ctor(&strval);
		convert_to_string(&strval);
		context->output_encoding = Z_STRVAL(strval);
	}
	if (SUCCESS == zend_ascii_hash_find(Z_ARRVAL_P(params), "default_mode", sizeof("default_mode"), (void**)&tmp)) {
		zval longval = **tmp;

		zval_copy_ctor(&longval);
		convert_to_long(&longval);
		context->default_mode = Z_LVAL(longval);
		zval_dtor(&longval);
	}
	return ret;
}

/* given a zval which is either a stream or a context, return the underlying
 * stream_context.  If it is a stream that does not have a context assigned, it
 * will create and assign a context and return that.  */
static php_stream_context *decode_context_param(zval *contextresource TSRMLS_DC)
{
	php_stream_context *context = NULL;

	context = zend_fetch_resource(&contextresource TSRMLS_CC, -1, NULL, NULL, 1, php_le_stream_context());
	if (context == NULL) {
		php_stream *stream;

		stream = zend_fetch_resource(&contextresource TSRMLS_CC, -1, NULL, NULL, 2, php_file_le_stream(), php_file_le_pstream);

		if (stream) {
			context = stream->context;
			if (context == NULL) {
				/* Only way this happens is if file is opened with NO_DEFAULT_CONTEXT
				   param, but then something is called which requires a context.
				   Don't give them the default one though since they already said they
	 			   didn't want it. */
				context = stream->context = php_stream_context_alloc();
			}
		}
	}

	return context;
}
/* }}} */

/* {{{ proto array stream_context_get_options(resource context|resource stream) U
   Retrieve options for a stream/wrapper/context */
PHP_FUNCTION(stream_context_get_options)
{
	zval *zcontext;
	php_stream_context *context;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zcontext) == FAILURE) {
		RETURN_FALSE;
	}
	context = decode_context_param(zcontext TSRMLS_CC);
	if (!context) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid stream/context parameter");
		RETURN_FALSE;
	}

	RETURN_ZVAL(context->options, 1, 0);
}
/* }}} */

/* {{{ proto bool stream_context_set_option(resource context|resource stream, string wrappername, string optionname, mixed value) U
 * Overloaded form: stream_context_set_option(resource context|resource stream, array options)
 * Set an option (or several options) for a wrapper */
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
	if (!context) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid stream/context parameter");
		RETURN_FALSE;
	}

	if (options) {
		/* handle the array syntax */
		RETVAL_BOOL(parse_context_options(context, options TSRMLS_CC) == SUCCESS);
	} else {
		php_stream_context_set_option(context, wrappername, optionname, zvalue);
		RETVAL_TRUE;
	}
}
/* }}} */

/* {{{ proto bool stream_context_set_params(resource context|resource stream, array options) U
   Set parameters for a file context */
PHP_FUNCTION(stream_context_set_params)
{
	zval *params, *zcontext;
	php_stream_context *context;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ra", &zcontext, &params) == FAILURE) {
		RETURN_FALSE;
	}

	context = decode_context_param(zcontext TSRMLS_CC);
	if (!context) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid stream/context parameter");
		RETURN_FALSE;
	}

	RETVAL_BOOL(parse_context_params(context, params TSRMLS_CC) == SUCCESS);
}
/* }}} */

/* {{{ proto resource stream_context_get_default([array options]) U
   Get a handle on the default file/stream context and optionally set parameters */
PHP_FUNCTION(stream_context_get_default)
{
	zval *params = NULL;
	php_stream_context *context;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|a", &params) == FAILURE) {
		RETURN_FALSE;
	}
	
	if (FG(default_context) == NULL) {
		FG(default_context) = php_stream_context_alloc();
	}
	context = FG(default_context);

	if (params) {
		parse_context_options(context, params TSRMLS_CC);
	}
	
	php_stream_context_to_zval(context, return_value);
}
/* }}} */

/* {{{ proto resource stream_context_create([array options[, array params]]) U
   Create a file context and optionally set parameters */
PHP_FUNCTION(stream_context_create)
{
	zval *options = NULL, *params = NULL;
	php_stream_context *context;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|a!a!", &options, &params) == FAILURE) {
		RETURN_FALSE;
	}
	
	context = php_stream_context_alloc();
	
	if (options) {
		parse_context_options(context, options TSRMLS_CC);
	}

	if (params) {
		parse_context_params(context, params TSRMLS_CC);
	}
	
	RETURN_RESOURCE(context->rsrc_id);
}
/* }}} */

/* {{{ streams filter functions */
static void apply_filter_to_stream(int append, INTERNAL_FUNCTION_PARAMETERS)
{
	zval *zstream;
	php_stream *stream;
	char *filtername;
	int filternamelen;
	long read_write = 0;
	zval *filterparams = NULL;
	php_stream_filter *filter = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|lz", &zstream,
				&filtername, &filternamelen, &read_write, &filterparams) == FAILURE) {
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
		filter = php_stream_filter_create(filtername, filterparams, php_stream_is_persistent(stream) TSRMLS_CC);
		if (filter == NULL) {
			RETURN_FALSE;
		}

		if (append) {
			php_stream_filter_append(&stream->readfilters, filter);
		} else {
			php_stream_filter_prepend(&stream->readfilters, filter);
		}
		if (FAILURE == php_stream_filter_check_chain(&stream->readfilters)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Readfilter chain unstable -- unresolvable unicode/string conversion conflict");
		}
	}

	if (read_write & PHP_STREAM_FILTER_WRITE) {
		filter = php_stream_filter_create(filtername, filterparams, php_stream_is_persistent(stream) TSRMLS_CC);
		if (filter == NULL) {
			RETURN_FALSE;
		}

		if (append) { 
			php_stream_filter_append(&stream->writefilters, filter);
		} else {
			php_stream_filter_prepend(&stream->writefilters, filter);
		}
		if (FAILURE == php_stream_filter_check_chain(&stream->writefilters)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Writefilter chain unstable -- unresolvable unicode/string conversion conflict");
		}
	}

	if (filter) {
		RETURN_RESOURCE(filter->rsrc_id = ZEND_REGISTER_RESOURCE(NULL, filter, php_file_le_stream_filter()));
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto resource stream_filter_prepend(resource stream, string filtername[, int read_write[, mixed filterparams]]) U
   Prepend a filter to a stream */
PHP_FUNCTION(stream_filter_prepend)
{
	apply_filter_to_stream(0, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto resource stream_filter_append(resource stream, string filtername[, int read_write[, mixed filterparams]]) U
   Append a filter to a stream */
PHP_FUNCTION(stream_filter_append)
{
	apply_filter_to_stream(1, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto bool stream_filter_remove(resource stream_filter) U
	Flushes any data in the filter's internal buffer, removes it from the chain, and frees the resource */
PHP_FUNCTION(stream_filter_remove)
{
	zval *zfilter;
	php_stream_filter *filter;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zfilter) == FAILURE) {
		RETURN_FALSE;
	}

	filter = zend_fetch_resource(&zfilter TSRMLS_CC, -1, NULL, NULL, 1, php_file_le_stream_filter());
	if (!filter) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid resource given, not a stream filter");
		RETURN_FALSE;
	}

	if (php_stream_filter_flush(filter, 1) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to flush filter, not removing");
		RETURN_FALSE;
	}

	if (zend_list_delete(Z_LVAL_P(zfilter)) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not invalidate filter, not removing");
		RETURN_FALSE;
	} else {
		if (FAILURE == php_stream_filter_check_chain(filter->chain)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Filterchain unstable -- unresolvable unicode/string conversion conflict");
		}

		php_stream_filter_remove(filter, 1 TSRMLS_CC);
		RETURN_TRUE;
	}
}
/* }}} */

/* {{{ proto string stream_get_line(resource stream, int maxlen [, string ending]) U
   Read up to maxlen bytes from a stream or until the ending string is found */
PHP_FUNCTION(stream_get_line)
{
	long max_length;
	zval *zstream;
	size_t buf_size;
	php_stream *stream;
	zval **delim = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl|Z", &zstream, &max_length, &delim) == FAILURE) {
		RETURN_FALSE;
	}

	if (max_length < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "The maximum allowed length must be greater than or equal to zero");
		RETURN_FALSE;
	}
	if (!max_length) {
		max_length = PHP_SOCK_CHUNK_SIZE;
	}

	php_stream_from_zval(stream, &zstream);

	if (stream->readbuf_type == IS_UNICODE) {
		UChar *buf;
		UChar *d = NULL;
		int dlen = 0;

		if (delim) {
			convert_to_unicode_ex(delim);
			d = Z_USTRVAL_PP(delim);
			dlen = Z_USTRLEN_PP(delim);
		}

		/* maxchars == maxlength will prevent the otherwise generous maxlen == max_length * 2
		   from allocating beyond what's requested */
		buf = php_stream_get_record_unicode(stream, max_length * 2, max_length, &buf_size, d, dlen TSRMLS_CC);
		if (!buf) {
			RETURN_FALSE;
		}

		RETURN_UNICODEL(buf, buf_size, 0);
	} else { /* IS_STRING */
		char *buf;
		char *d = NULL;
		int dlen = 0;

		if (delim) {
			convert_to_string_ex(delim);
			d = Z_STRVAL_PP(delim);
			dlen = Z_STRLEN_PP(delim);
		}

		buf = php_stream_get_record(stream, max_length, &buf_size, d, dlen TSRMLS_CC);
		if (!buf) {
			RETURN_FALSE;
		}

		RETURN_STRINGL(buf, buf_size, 0);
	}
}

/* }}} */

/* {{{ proto bool stream_set_blocking(resource socket, int mode) U
   Set blocking/non-blocking mode on a socket or stream */
PHP_FUNCTION(stream_set_blocking)
{
	zval *arg1;
	long arg2;
	int block;
	php_stream *stream;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &arg1, &arg2) == FAILURE) {
		return;
	}

	php_stream_from_zval(stream, &arg1);

	block = arg2;

	if (php_stream_set_option(stream, PHP_STREAM_OPTION_BLOCKING, block == 0 ? 0 : 1, NULL) == -1) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}

/* }}} */

/* {{{ proto bool stream_set_timeout(resource stream, int seconds [, int microseconds]) U
   Set timeout on stream read to seconds + microseonds */
#if HAVE_SYS_TIME_H || defined(PHP_WIN32)
PHP_FUNCTION(stream_set_timeout)
{
	zval *socket;
	long seconds, microseconds;
	struct timeval t;
	php_stream *stream;
	int argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc TSRMLS_CC, "rl|l", &socket, &seconds, &microseconds) == FAILURE) {
		return;
	}

	php_stream_from_zval(stream, &socket);

	t.tv_sec = seconds;

	if (argc == 3) {
		t.tv_usec = microseconds % 1000000;
		t.tv_sec += microseconds / 1000000;
	} else {
		t.tv_usec = 0;
	}

	if (PHP_STREAM_OPTION_RETURN_OK == php_stream_set_option(stream, PHP_STREAM_OPTION_READ_TIMEOUT, 0, &t)) {
		RETURN_TRUE;
	}

	RETURN_FALSE;
}
#endif /* HAVE_SYS_TIME_H || defined(PHP_WIN32) */
/* }}} */

/* {{{ proto int stream_set_write_buffer(resource fp, int buffer) U
   Set file write buffer */
PHP_FUNCTION(stream_set_write_buffer)
{
	zval *arg1;
	long arg2;
	int ret;
	size_t buff;
	php_stream *stream;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &arg1, &arg2) == FAILURE) {
		return;
	}
	
	php_stream_from_zval(stream, &arg1);
	
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

/* {{{ proto int stream_socket_enable_crypto(resource stream, bool enable [, int cryptokind [, resource sessionstream]]) U
   Enable or disable a specific kind of crypto on the stream */
PHP_FUNCTION(stream_socket_enable_crypto)
{
	long cryptokind;
	zval *zstream, *zsessstream = NULL;
	php_stream *stream, *sessstream = NULL;
	zend_bool enable;
	int ret;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rb|lr", &zstream, &enable, &cryptokind, &zsessstream) == FAILURE) {
		RETURN_FALSE;
	}
	
	php_stream_from_zval(stream, &zstream);

	if (ZEND_NUM_ARGS() >= 3) {
		if (zsessstream) {
			php_stream_from_zval(sessstream, &zsessstream);
		}
		
		if (php_stream_xport_crypto_setup(stream, cryptokind, sessstream TSRMLS_CC) < 0) {
			RETURN_FALSE;
		}
	} else if (enable) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "When enabling encryption you must specify the crypto type");
		RETURN_FALSE;
	}

	ret = php_stream_xport_crypto_enable(stream, enable TSRMLS_CC);
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

/* {{{ proto bool stream_default_encoding(string encoding) U
Convenience wrapper for ini_set('unicode.stream_encoding', $encoding) */
PHP_FUNCTION(stream_default_encoding)
{
	char *encoding;
	int encoding_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &encoding, &encoding_len) == FAILURE) {
		return;
	}

	RETURN_BOOL(SUCCESS == zend_alter_ini_entry("unicode.stream_encoding", sizeof("unicode.stream_encoding"),
							encoding, encoding_len, PHP_INI_ALL, PHP_INI_STAGE_RUNTIME));
}
/* }}} */

/* {{{ proto void stream_encoding(resource stream[, string encoding]) U
Set character set for stream encoding
UTODO: Return current encoding charset
*/
PHP_FUNCTION(stream_encoding)
{
	zval *zstream;
	php_stream *stream;
	char *encoding = NULL;
	int encoding_len = 0;
	int remove_read_tail = 0, remove_write_tail = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|s", &zstream, &encoding, &encoding_len) == FAILURE) {
		return;
	}

	php_stream_from_zval(stream, &zstream);

	/* Double check that the target encoding is legal before attempting anything */

	if (stream->readfilters.tail) {
		if (stream->readfilters.tail->fops == &php_unicode_from_string_filter_ops) {
			/* Remove the current unicode.from.* filter, 
               the filter layer will transcode anything in the read buffer back to binary 
               or invalidate the read buffer */
			remove_read_tail = 1;
		} else if (stream->readbuf_type == IS_UNICODE) {
			/* There's an encoding on the stream already, but then there's filtering happening after that point
			   It's asking too much for PHP to figure out what the user wants, throw an error back in their face */
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot change encoding on filtered stream");
			RETURN_FALSE;
		}
	}

	if (stream->writefilters.tail) {
		if (stream->writefilters.tail->fops == &php_unicode_to_string_filter_ops) {
			/* Remove the current unicode.to.* filter */
			remove_write_tail = 1;
		} else if ((stream->writefilters.tail->fops->flags & PSFO_FLAG_OUTPUTS_UNICODE) == 0) {
			/* conversion to binary is happening, them another filter is doing something
			   bailout for same reason as read filters */
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot change encoding on filtered stream");
			RETURN_FALSE;
		}
	}

	if (remove_read_tail) {
		php_stream_filter_remove(stream->readfilters.tail, 1 TSRMLS_CC);
	}
	if (remove_write_tail) {
		php_stream_filter_remove(stream->writefilters.tail, 1 TSRMLS_CC);
	}
	
	if (encoding_len == 0) {
		if (UG(stream_encoding) == NULL) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "The stream_encoding must be defined");
			RETURN_FALSE;
		}
		encoding = UG(stream_encoding);
	}

	/* UTODO: Allow overriding error handling for converters */
	php_stream_encoding_apply(stream, 1, encoding, UG(from_error_mode), UG(from_subst_char));
	php_stream_encoding_apply(stream, 0, encoding, UG(to_error_mode), NULL);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string stream_resolve_include_path(string filename[, resource context]) U
Determine what file will be opened by calls to fopen() with a relative path */
PHP_FUNCTION(stream_resolve_include_path)
{
	zval **ppfilename, *zcontext = NULL;
	char *filename, *ptr = PG(include_path), *end = ptr + (ptr ? strlen(ptr) : 0), buffer[MAXPATHLEN];
	int filename_len;
	php_stream_context *context = NULL;
	struct stat sb;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z|r", &ppfilename, &zcontext) == FAILURE ||
		php_stream_path_param_encode(ppfilename, &filename, &filename_len, REPORT_ERRORS, context = php_stream_context_from_zval(zcontext, 0)) == FAILURE) {
		return;
	}

	while (ptr < end) {
		char *s = strchr(ptr, DEFAULT_DIR_SEPARATOR);

		if (!s) {
			s = end;
		}

		if (s == ptr) {
			ptr++;
			continue;
		}

		if ((s - ptr) + 1 + filename_len >= MAXPATHLEN) {
			/* Too long to try */
			ptr = s + 1;
			continue;
		}

		memcpy(buffer, ptr, s - ptr);
		buffer[s - ptr] = '/';
		memcpy(buffer + (s - ptr) + 1, filename, filename_len + 1);

		if (php_check_open_basedir_ex(buffer, 0 TSRMLS_CC)) {
			ptr = s + 1;
			continue;
		}

		if (VCWD_STAT(buffer, &sb)) {
			ptr = s + 1;
			continue;
		}

		if (UG(unicode)) {
			UChar *upath;
			int upath_len;

			if (SUCCESS == php_stream_path_decode(NULL, &upath, &upath_len, buffer, (s - ptr) + 1 + filename_len, REPORT_ERRORS, context)) {
				RETURN_UNICODEL(upath, upath_len, 0);
			} else {
				/* Fallback */
				RETURN_STRINGL(buffer, (s - ptr) + 1 + filename_len, 1);
			}
		} else {
			RETURN_STRINGL(buffer, (s - ptr) + 1 + filename_len, 1);
		}
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

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &zstream) == FAILURE) {
		RETURN_FALSE;
	}

	if(Z_TYPE_P(zstream) == IS_RESOURCE) {
		php_stream_from_zval(stream, &zstream);
		if(stream == NULL) {
			RETURN_FALSE;
		}
		wrapper = stream->wrapper;
	} else {
		convert_to_string_ex(&zstream);
		wrapper = php_stream_locate_url_wrapper(Z_STRVAL_P(zstream), NULL, 0 TSRMLS_CC);
	}

	if(!wrapper) {
		RETURN_FALSE;
	}

	RETURN_BOOL(wrapper->is_url==0);
}
/* }}} */

/* {{{ proto bool stream_supports_lock(resource stream)
   Tells wether the stream supports locking through flock(). */
PHP_FUNCTION(stream_supports_lock)
{
	php_stream *stream;
	zval *zsrc;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zsrc) == FAILURE) {
		RETURN_FALSE;
	}

	php_stream_from_zval(stream, &zsrc);

	if (!php_stream_supports_lock(stream)) {
		RETURN_FALSE;
	}

	RETURN_TRUE;
}

#ifdef HAVE_SHUTDOWN
/* {{{ proto int stream_socket_shutdown(resource stream, int how) U
	causes all or part of a full-duplex connection on the socket associated
	with stream to be shut down.  If how is SHUT_RD,  further receptions will
	be disallowed. If how is SHUT_WR, further transmissions will be disallowed.
	If how is SHUT_RDWR,  further  receptions and transmissions will be
	disallowed. */
PHP_FUNCTION(stream_socket_shutdown)
{
	long how;
	zval *zstream;
	php_stream *stream;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &zstream, &how) == FAILURE) {
		RETURN_FALSE;
	}
	
	if (how != STREAM_SHUT_RD &&
	    how != STREAM_SHUT_WR &&
	    how != STREAM_SHUT_RDWR) {
		RETURN_FALSE;
	}

	php_stream_from_zval(stream, &zstream);

	RETURN_BOOL(php_stream_xport_shutdown(stream, (stream_shutdown_t)how TSRMLS_CC) == 0);
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

