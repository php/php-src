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
  | Author: Wez Furlong <wez@thebrainroom.com>                           |
  +----------------------------------------------------------------------+
*/

#include "php.h"
#include "php_streams_int.h"
#include "ext/standard/file.h"

static HashTable xport_hash;

PHPAPI HashTable *php_stream_xport_get_hash(void)
{
	return &xport_hash;
}

PHPAPI int php_stream_xport_register(const char *protocol, php_stream_transport_factory factory)
{
	zend_string *str = zend_string_init_interned(protocol, strlen(protocol), 1);

	zend_hash_update_ptr(&xport_hash, str, factory);
	zend_string_release_ex(str, 1);
	return SUCCESS;
}

PHPAPI int php_stream_xport_unregister(const char *protocol)
{
	return zend_hash_str_del(&xport_hash, protocol, strlen(protocol));
}

#define ERR_REPORT(out_err, fmt, arg) \
	if (out_err) { *out_err = strpprintf(0, fmt, arg); } \
	else { php_error_docref(NULL, E_WARNING, fmt, arg); }

#define ERR_RETURN(out_err, local_err, fmt) \
	if (out_err) { *out_err = local_err; } \
	else { php_error_docref(NULL, E_WARNING, fmt, local_err ? ZSTR_VAL(local_err) : "Unspecified error"); \
		if (local_err) { zend_string_release_ex(local_err, 0); local_err = NULL; } \
	}

PHPAPI php_stream *_php_stream_xport_create(const char *name, size_t namelen, int options,
		int flags, const char *persistent_id,
		struct timeval *timeout,
		php_stream_context *context,
		zend_string **error_string,
		int *error_code
		STREAMS_DC)
{
	php_stream *stream = NULL;
	php_stream_transport_factory factory = NULL;
	const char *p, *protocol = NULL;
	size_t n = 0;
	int failed = 0;
	zend_string *error_text = NULL;
	struct timeval default_timeout = { 0, 0 };

	default_timeout.tv_sec = FG(default_socket_timeout);

	if (timeout == NULL) {
		timeout = &default_timeout;
	}

	/* check for a cached persistent socket */
	if (persistent_id) {
		switch(php_stream_from_persistent_id(persistent_id, &stream)) {
			case PHP_STREAM_PERSISTENT_SUCCESS:
				/* use a 0 second timeout when checking if the socket
				 * has already died */
				if (PHP_STREAM_OPTION_RETURN_OK == php_stream_set_option(stream, PHP_STREAM_OPTION_CHECK_LIVENESS, 0, NULL)) {
					return stream;
				}
				/* dead - kill it */
				php_stream_pclose(stream);
				stream = NULL;

				/* fall through */

			case PHP_STREAM_PERSISTENT_FAILURE:
			default:
				/* failed; get a new one */
				;
		}
	}

	for (p = name; isalnum((int)*p) || *p == '+' || *p == '-' || *p == '.'; p++) {
		n++;
	}

	if ((*p == ':') && (n > 1) && !strncmp("://", p, 3)) {
		protocol = name;
		name = p + 3;
		namelen -= n + 3;
	} else {
		protocol = "tcp";
		n = 3;
	}

	if (protocol) {
		if (NULL == (factory = zend_hash_str_find_ptr(&xport_hash, protocol, n))) {
			char wrapper_name[32];

			if (n >= sizeof(wrapper_name))
				n = sizeof(wrapper_name) - 1;
			PHP_STRLCPY(wrapper_name, protocol, sizeof(wrapper_name), n);

			ERR_REPORT(error_string, "Unable to find the socket transport \"%s\" - did you forget to enable it when you configured PHP?",
					wrapper_name);

			return NULL;
		}
	}

	if (factory == NULL) {
		/* should never happen */
		php_error_docref(NULL, E_WARNING, "Could not find a factory !?");
		return NULL;
	}

	stream = (factory)(protocol, n,
			(char*)name, namelen, persistent_id, options, flags, timeout,
			context STREAMS_REL_CC);

	if (stream) {
		php_stream_context_set(stream, context);

		if ((flags & STREAM_XPORT_SERVER) == 0) {
			/* client */

			if (flags & (STREAM_XPORT_CONNECT|STREAM_XPORT_CONNECT_ASYNC)) {
				if (-1 == php_stream_xport_connect(stream, name, namelen,
							flags & STREAM_XPORT_CONNECT_ASYNC ? 1 : 0,
							timeout, &error_text, error_code)) {

					ERR_RETURN(error_string, error_text, "connect() failed: %s");

					failed = 1;
				}
			}

		} else {
			/* server */
			if (flags & STREAM_XPORT_BIND) {
				if (0 != php_stream_xport_bind(stream, name, namelen, &error_text)) {
					ERR_RETURN(error_string, error_text, "bind() failed: %s");
					failed = 1;
				} else if (flags & STREAM_XPORT_LISTEN) {
					zval *zbacklog = NULL;
					int backlog = 32;

					if (PHP_STREAM_CONTEXT(stream) && (zbacklog = php_stream_context_get_option(PHP_STREAM_CONTEXT(stream), "socket", "backlog")) != NULL) {
						backlog = zval_get_long(zbacklog);
					}

					if (0 != php_stream_xport_listen(stream, backlog, &error_text)) {
						ERR_RETURN(error_string, error_text, "listen() failed: %s");
						failed = 1;
					}
				}
			}
		}
	}

	if (failed) {
		/* failure means that they don't get a stream to play with */
		if (persistent_id) {
			php_stream_pclose(stream);
		} else {
			php_stream_close(stream);
		}
		stream = NULL;
	}

	return stream;
}

/* Bind the stream to a local address */
PHPAPI int php_stream_xport_bind(php_stream *stream,
		const char *name, size_t namelen,
		zend_string **error_text
		)
{
	php_stream_xport_param param;
	int ret;

	memset(&param, 0, sizeof(param));
	param.op = STREAM_XPORT_OP_BIND;
	param.inputs.name = (char*)name;
	param.inputs.namelen = namelen;
	param.want_errortext = error_text ? 1 : 0;

	ret = php_stream_set_option(stream, PHP_STREAM_OPTION_XPORT_API, 0, &param);

	if (ret == PHP_STREAM_OPTION_RETURN_OK) {
		if (error_text) {
			*error_text = param.outputs.error_text;
		}

		return param.outputs.returncode;
	}

	return ret;
}

/* Connect to a remote address */
PHPAPI int php_stream_xport_connect(php_stream *stream,
		const char *name, size_t namelen,
		int asynchronous,
		struct timeval *timeout,
		zend_string **error_text,
		int *error_code
		)
{
	php_stream_xport_param param;
	int ret;

	memset(&param, 0, sizeof(param));
	param.op = asynchronous ? STREAM_XPORT_OP_CONNECT_ASYNC: STREAM_XPORT_OP_CONNECT;
	param.inputs.name = (char*)name;
	param.inputs.namelen = namelen;
	param.inputs.timeout = timeout;

	param.want_errortext = error_text ? 1 : 0;

	ret = php_stream_set_option(stream, PHP_STREAM_OPTION_XPORT_API, 0, &param);

	if (ret == PHP_STREAM_OPTION_RETURN_OK) {
		if (error_text) {
			*error_text = param.outputs.error_text;
		}
		if (error_code) {
			*error_code = param.outputs.error_code;
		}
		return param.outputs.returncode;
	}

	return ret;

}

/* Prepare to listen */
PHPAPI int php_stream_xport_listen(php_stream *stream, int backlog, zend_string **error_text)
{
	php_stream_xport_param param;
	int ret;

	memset(&param, 0, sizeof(param));
	param.op = STREAM_XPORT_OP_LISTEN;
	param.inputs.backlog = backlog;
	param.want_errortext = error_text ? 1 : 0;

	ret = php_stream_set_option(stream, PHP_STREAM_OPTION_XPORT_API, 0, &param);

	if (ret == PHP_STREAM_OPTION_RETURN_OK) {
		if (error_text) {
			*error_text = param.outputs.error_text;
		}

		return param.outputs.returncode;
	}

	return ret;
}

/* Get the next client and their address (as a string) */
PHPAPI int php_stream_xport_accept(php_stream *stream, php_stream **client,
		zend_string **textaddr,
		void **addr, socklen_t *addrlen,
		struct timeval *timeout,
		zend_string **error_text
		)
{
	php_stream_xport_param param;
	int ret;

	memset(&param, 0, sizeof(param));

	param.op = STREAM_XPORT_OP_ACCEPT;
	param.inputs.timeout = timeout;
	param.want_addr = addr ? 1 : 0;
	param.want_textaddr = textaddr ? 1 : 0;
	param.want_errortext = error_text ? 1 : 0;

	ret = php_stream_set_option(stream, PHP_STREAM_OPTION_XPORT_API, 0, &param);

	if (ret == PHP_STREAM_OPTION_RETURN_OK) {
		*client = param.outputs.client;
		if (addr) {
			*addr = param.outputs.addr;
			*addrlen = param.outputs.addrlen;
		}
		if (textaddr) {
			*textaddr = param.outputs.textaddr;
		}
		if (error_text) {
			*error_text = param.outputs.error_text;
		}

		return param.outputs.returncode;
	}
	return ret;
}

PHPAPI int php_stream_xport_get_name(php_stream *stream, int want_peer,
		zend_string **textaddr,
		void **addr, socklen_t *addrlen
		)
{
	php_stream_xport_param param;
	int ret;

	memset(&param, 0, sizeof(param));

	param.op = want_peer ? STREAM_XPORT_OP_GET_PEER_NAME : STREAM_XPORT_OP_GET_NAME;
	param.want_addr = addr ? 1 : 0;
	param.want_textaddr = textaddr ? 1 : 0;

	ret = php_stream_set_option(stream, PHP_STREAM_OPTION_XPORT_API, 0, &param);

	if (ret == PHP_STREAM_OPTION_RETURN_OK) {
		if (addr) {
			*addr = param.outputs.addr;
			*addrlen = param.outputs.addrlen;
		}
		if (textaddr) {
			*textaddr = param.outputs.textaddr;
		}

		return param.outputs.returncode;
	}
	return ret;
}

PHPAPI int php_stream_xport_crypto_setup(php_stream *stream, php_stream_xport_crypt_method_t crypto_method, php_stream *session_stream)
{
	php_stream_xport_crypto_param param;
	int ret;

	memset(&param, 0, sizeof(param));
	param.op = STREAM_XPORT_CRYPTO_OP_SETUP;
	param.inputs.method = crypto_method;
	param.inputs.session = session_stream;

	ret = php_stream_set_option(stream, PHP_STREAM_OPTION_CRYPTO_API, 0, &param);

	if (ret == PHP_STREAM_OPTION_RETURN_OK) {
		return param.outputs.returncode;
	}

	php_error_docref("streams.crypto", E_WARNING, "this stream does not support SSL/crypto");

	return ret;
}

PHPAPI int php_stream_xport_crypto_enable(php_stream *stream, int activate)
{
	php_stream_xport_crypto_param param;
	int ret;

	memset(&param, 0, sizeof(param));
	param.op = STREAM_XPORT_CRYPTO_OP_ENABLE;
	param.inputs.activate = activate;

	ret = php_stream_set_option(stream, PHP_STREAM_OPTION_CRYPTO_API, 0, &param);

	if (ret == PHP_STREAM_OPTION_RETURN_OK) {
		return param.outputs.returncode;
	}

	php_error_docref("streams.crypto", E_WARNING, "this stream does not support SSL/crypto");

	return ret;
}

/* Similar to recv() system call; read data from the stream, optionally
 * peeking, optionally retrieving OOB data */
PHPAPI int php_stream_xport_recvfrom(php_stream *stream, char *buf, size_t buflen,
		int flags, void **addr, socklen_t *addrlen, zend_string **textaddr
		)
{
	php_stream_xport_param param;
	int ret = 0;
	int recvd_len = 0;
#if 0
	int oob;

	if (flags == 0 && addr == NULL) {
		return php_stream_read(stream, buf, buflen);
	}

	if (stream->readfilters.head) {
		php_error_docref(NULL, E_WARNING, "cannot peek or fetch OOB data from a filtered stream");
		return -1;
	}

	oob = (flags & STREAM_OOB) == STREAM_OOB;

	if (!oob && addr == NULL) {
		/* must be peeking at regular data; copy content from the buffer
		 * first, then adjust the pointer/len before handing off to the
		 * stream */
		recvd_len = stream->writepos - stream->readpos;
		if (recvd_len > buflen) {
			recvd_len = buflen;
		}
		if (recvd_len) {
			memcpy(buf, stream->readbuf, recvd_len);
			buf += recvd_len;
			buflen -= recvd_len;
		}
		/* if we filled their buffer, return */
		if (buflen == 0) {
			return recvd_len;
		}
	}
#endif

	/* otherwise, we are going to bypass the buffer */

	memset(&param, 0, sizeof(param));

	param.op = STREAM_XPORT_OP_RECV;
	param.want_addr = addr ? 1 : 0;
	param.want_textaddr = textaddr ? 1 : 0;
	param.inputs.buf = buf;
	param.inputs.buflen = buflen;
	param.inputs.flags = flags;

	ret = php_stream_set_option(stream, PHP_STREAM_OPTION_XPORT_API, 0, &param);

	if (ret == PHP_STREAM_OPTION_RETURN_OK) {
		if (addr) {
			*addr = param.outputs.addr;
			*addrlen = param.outputs.addrlen;
		}
		if (textaddr) {
			*textaddr = param.outputs.textaddr;
		}
		return recvd_len + param.outputs.returncode;
	}
	return recvd_len ? recvd_len : -1;
}

/* Similar to send() system call; send data to the stream, optionally
 * sending it as OOB data */
PHPAPI int php_stream_xport_sendto(php_stream *stream, const char *buf, size_t buflen,
		int flags, void *addr, socklen_t addrlen)
{
	php_stream_xport_param param;
	int ret = 0;
	int oob;

#if 0
	if (flags == 0 && addr == NULL) {
		return php_stream_write(stream, buf, buflen);
	}
#endif

	oob = (flags & STREAM_OOB) == STREAM_OOB;

	if ((oob || addr) && stream->writefilters.head) {
		php_error_docref(NULL, E_WARNING, "cannot write OOB data, or data to a targeted address on a filtered stream");
		return -1;
	}

	memset(&param, 0, sizeof(param));

	param.op = STREAM_XPORT_OP_SEND;
	param.want_addr = addr ? 1 : 0;
	param.inputs.buf = (char*)buf;
	param.inputs.buflen = buflen;
	param.inputs.flags = flags;
	param.inputs.addr = addr;
	param.inputs.addrlen = addrlen;

	ret = php_stream_set_option(stream, PHP_STREAM_OPTION_XPORT_API, 0, &param);

	if (ret == PHP_STREAM_OPTION_RETURN_OK) {
		return param.outputs.returncode;
	}
	return -1;
}

/* Similar to shutdown() system call; shut down part of a full-duplex
 * connection */
PHPAPI int php_stream_xport_shutdown(php_stream *stream, stream_shutdown_t how)
{
	php_stream_xport_param param;
	int ret = 0;

	memset(&param, 0, sizeof(param));

	param.op = STREAM_XPORT_OP_SHUTDOWN;
	param.how = how;

	ret = php_stream_set_option(stream, PHP_STREAM_OPTION_XPORT_API, 0, &param);

	if (ret == PHP_STREAM_OPTION_RETURN_OK) {
		return param.outputs.returncode;
	}
	return -1;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
