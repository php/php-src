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
  | Author: Wez Furlong <wez@thebrainroom.com>                           |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "php.h"
#include "ext/standard/file.h"
#include "streams/php_streams_int.h"
#include "php_network.h"

#if defined(PHP_WIN32) || defined(__riscos__) || defined(NETWARE)
# undef AF_UNIX
#endif

#if defined(AF_UNIX)
#include <sys/un.h>
#endif

php_stream_ops php_stream_generic_socket_ops;
php_stream_ops php_stream_socket_ops;
php_stream_ops php_stream_udp_socket_ops;
#ifdef AF_UNIX
php_stream_ops php_stream_unix_socket_ops;
php_stream_ops php_stream_unixdg_socket_ops;
#endif


static int php_tcp_sockop_set_option(php_stream *stream, int option, int value, void *ptrparam TSRMLS_DC);

/* {{{ Generic socket stream operations */
static size_t php_sockop_write(php_stream *stream, const char *buf, size_t count TSRMLS_DC)
{
	php_netstream_data_t *sock = (php_netstream_data_t*)stream->abstract;
	size_t didwrite;

	if (sock->socket == -1) {
		return 0;
	}

	didwrite = send(sock->socket, buf, count, 0);

	if (didwrite <= 0) {
		char *estr = php_socket_strerror(php_socket_errno(), NULL, 0);

		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "send of %d bytes failed with errno=%d %s",
				count, php_socket_errno(), estr);
		efree(estr);
	}

	if (didwrite > 0) {
		php_stream_notify_progress_increment(stream->context, didwrite, 0);
	}

	return didwrite;
}

static void php_sock_stream_wait_for_data(php_stream *stream, php_netstream_data_t *sock TSRMLS_DC)
{
	fd_set fdr, tfdr;
	int retval;
	struct timeval timeout, *ptimeout;

	if (sock->socket == -1) {
		return;
	}
	
	FD_ZERO(&fdr);
	FD_SET(sock->socket, &fdr);
	sock->timeout_event = 0;

	if (sock->timeout.tv_sec == -1)
		ptimeout = NULL;
	else
		ptimeout = &timeout;


	while(1) {
		tfdr = fdr;
		timeout = sock->timeout;

		retval = select(sock->socket + 1, &tfdr, NULL, NULL, ptimeout);

		if (retval == 0)
			sock->timeout_event = 1;

		if (retval >= 0)
			break;
	}
}

static size_t php_sockop_read(php_stream *stream, char *buf, size_t count TSRMLS_DC)
{
	php_netstream_data_t *sock = (php_netstream_data_t*)stream->abstract;
	int nr_bytes = 0;

	if (sock->socket == -1) {
		return 0;
	}

	if (sock->is_blocked) {
		php_sock_stream_wait_for_data(stream, sock TSRMLS_CC);
		if (sock->timeout_event)
			return 0;
	}

	nr_bytes = recv(sock->socket, buf, count, 0);

	if (nr_bytes == 0 || (nr_bytes == -1 && php_socket_errno() != EWOULDBLOCK)) {
		stream->eof = 1;
	}

	if (nr_bytes > 0)
		php_stream_notify_progress_increment(stream->context, nr_bytes, 0);

	return nr_bytes;
}


static int php_sockop_close(php_stream *stream, int close_handle TSRMLS_DC)
{
	php_netstream_data_t *sock = (php_netstream_data_t*)stream->abstract;
	fd_set wrfds, efds;
	int n;
	struct timeval timeout;

	if (close_handle) {

		if (sock->socket != -1) {
			/* prevent more data from coming in */

#ifdef AF_UNIX
			if (stream->ops != &php_stream_unix_socket_ops && stream->ops != &php_stream_unixdg_socket_ops) {
#endif
				shutdown(sock->socket, SHUT_RD);
#ifdef AF_UNIX
			}
#endif

			/* try to make sure that the OS sends all data before we close the connection.
			 * Essentially, we are waiting for the socket to become writeable, which means
			 * that all pending data has been sent.
			 * We use a small timeout which should encourage the OS to send the data,
			 * but at the same time avoid hanging indefintely.
			 * */
			do {
				FD_ZERO(&wrfds);
				FD_SET(sock->socket, &wrfds);
				efds = wrfds;

				timeout.tv_sec = 0;
				timeout.tv_usec = 5000; /* arbitrary */

				n = select(sock->socket + 1, NULL, &wrfds, &efds, &timeout);
			} while (n == -1 && php_socket_errno() == EINTR);

			closesocket(sock->socket);
			sock->socket = -1;
		}

	}

	pefree(sock, php_stream_is_persistent(stream));
	
	return 0;
}

static int php_sockop_flush(php_stream *stream TSRMLS_DC)
{
	php_netstream_data_t *sock = (php_netstream_data_t*)stream->abstract;
	return fsync(sock->socket);
}

static int php_sockop_stat(php_stream *stream, php_stream_statbuf *ssb TSRMLS_DC)
{
	php_netstream_data_t *sock = (php_netstream_data_t*)stream->abstract;
	return fstat(sock->socket, &ssb->sb);
}

static int php_sockop_set_option(php_stream *stream, int option, int value, void *ptrparam TSRMLS_DC)
{
	int oldmode;
	php_netstream_data_t *sock = (php_netstream_data_t*)stream->abstract;
	php_stream_xport_param *xparam;
	
	switch(option) {
		case PHP_STREAM_OPTION_BLOCKING:
	
			oldmode = sock->is_blocked;
	
			/* no need to change anything */
			if (value == oldmode)
				return oldmode;
	
			if (SUCCESS == php_set_sock_blocking(sock->socket, value TSRMLS_CC)) {
				sock->is_blocked = value;
				return oldmode;
			}

			return PHP_STREAM_OPTION_RETURN_ERR;

		case PHP_STREAM_OPTION_READ_TIMEOUT:
			sock->timeout = *(struct timeval*)ptrparam;
			sock->timeout_event = 0;
			return PHP_STREAM_OPTION_RETURN_OK;

		case PHP_STREAM_OPTION_META_DATA_API:
			add_assoc_bool((zval *)ptrparam, "timed_out", sock->timeout_event);
			add_assoc_bool((zval *)ptrparam, "blocked", sock->is_blocked);
			add_assoc_bool((zval *)ptrparam, "eof", stream->eof);
			return PHP_STREAM_OPTION_RETURN_OK;
		
		case PHP_STREAM_OPTION_XPORT_API:
			xparam = (php_stream_xport_param *)ptrparam;

			switch (xparam->op) {
				case STREAM_XPORT_OP_LISTEN:
					xparam->outputs.returncode = listen(sock->socket, 5);
					return PHP_STREAM_OPTION_RETURN_OK;

				case STREAM_XPORT_OP_GET_NAME:
					xparam->outputs.returncode = php_network_get_sock_name(sock->socket,
							xparam->want_textaddr ? &xparam->outputs.textaddr : NULL,
							xparam->want_textaddr ? &xparam->outputs.textaddrlen : NULL,
							xparam->want_addr ? &xparam->outputs.addr : NULL,
							xparam->want_addr ? &xparam->outputs.addrlen : NULL
							TSRMLS_CC);
					return PHP_STREAM_OPTION_RETURN_OK;

				case STREAM_XPORT_OP_GET_PEER_NAME:
					xparam->outputs.returncode = php_network_get_peer_name(sock->socket,
							xparam->want_textaddr ? &xparam->outputs.textaddr : NULL,
							xparam->want_textaddr ? &xparam->outputs.textaddrlen : NULL,
							xparam->want_addr ? &xparam->outputs.addr : NULL,
							xparam->want_addr ? &xparam->outputs.addrlen : NULL
							TSRMLS_CC);
					return PHP_STREAM_OPTION_RETURN_OK;


				default:
					return PHP_STREAM_OPTION_RETURN_NOTIMPL;
			}

		default:
			return PHP_STREAM_OPTION_RETURN_NOTIMPL;
	}
}

static int php_sockop_cast(php_stream *stream, int castas, void **ret TSRMLS_DC)
{
	php_netstream_data_t *sock = (php_netstream_data_t*)stream->abstract;

	switch(castas)	{
		case PHP_STREAM_AS_STDIO:
			if (ret)	{
				*ret = fdopen(sock->socket, stream->mode);
				if (*ret)
					return SUCCESS;
				return FAILURE;
			}
			return SUCCESS;
		case PHP_STREAM_AS_FD:
		case PHP_STREAM_AS_SOCKETD:
			if (ret)
				*ret = (void*)sock->socket;
			return SUCCESS;
		default:
			return FAILURE;
	}
}
/* }}} */

/* These may look identical, but we need them this way so that
 * we can determine which type of socket we are dealing with
 * by inspecting stream->ops.
 * A "useful" side-effect is that the user's scripts can then
 * make similar decisions using stream_get_meta_data.
 * */
php_stream_ops php_stream_generic_socket_ops = {
	php_sockop_write, php_sockop_read,
	php_sockop_close, php_sockop_flush,
	"generic_socket",
	NULL, /* seek */
	php_sockop_cast,
	php_sockop_stat,
	php_sockop_set_option,
};


php_stream_ops php_stream_socket_ops = {
	php_sockop_write, php_sockop_read,
	php_sockop_close, php_sockop_flush,
	"tcp_socket",
	NULL, /* seek */
	php_sockop_cast,
	php_sockop_stat,
	php_tcp_sockop_set_option,
};

php_stream_ops php_stream_udp_socket_ops = {
	php_sockop_write, php_sockop_read,
	php_sockop_close, php_sockop_flush,
	"udp_socket",
	NULL, /* seek */
	php_sockop_cast,
	php_sockop_stat,
	php_tcp_sockop_set_option,
};

#ifdef AF_UNIX
php_stream_ops php_stream_unix_socket_ops = {
	php_sockop_write, php_sockop_read,
	php_sockop_close, php_sockop_flush,
	"unix_socket",
	NULL, /* seek */
	php_sockop_cast,
	php_sockop_stat,
	php_tcp_sockop_set_option,
};
php_stream_ops php_stream_unixdg_socket_ops = {
	php_sockop_write, php_sockop_read,
	php_sockop_close, php_sockop_flush,
	"udg_socket",
	NULL, /* seek */
	php_sockop_cast,
	php_sockop_stat,
	php_tcp_sockop_set_option,
};
#endif


/* network socket operations */

#ifdef AF_UNIX
static inline int parse_unix_address(php_stream_xport_param *xparam, struct sockaddr_un *unix_addr TSRMLS_DC)
{
	memset(unix_addr, 0, sizeof(*unix_addr));
	unix_addr->sun_family = AF_UNIX;

	/* we need to be binary safe on systems that support an abstract
	 * namespace */
	if (xparam->inputs.namelen >= sizeof(unix_addr->sun_path)) {
		/* On linux, when the path begins with a NUL byte we are
		 * referring to an abstract namespace.  In theory we should
		 * allow an extra byte below, since we don't need the NULL.
		 * BUT, to get into this branch of code, the name is too long,
		 * so we don't care. */
		xparam->inputs.namelen = sizeof(unix_addr->sun_path) - 1;
	}

	memcpy(unix_addr->sun_path, xparam->inputs.name, xparam->inputs.namelen);

	return 1;
}
#endif

static inline char *parse_ip_address(php_stream_xport_param *xparam, int *portno TSRMLS_DC)
{
	char *colon;
	char *host = NULL;

	colon = memchr(xparam->inputs.name, ':', xparam->inputs.namelen);
	if (colon) {
		*portno = atoi(colon + 1);
		host = estrndup(xparam->inputs.name, colon - xparam->inputs.name);
	} else {
		if (xparam->want_errortext) {
			spprintf(&xparam->outputs.error_text, 0, "Failed to parse address \"%s\"", xparam->inputs.name);
		}
		return NULL;
	}

	return host;
}

static inline int php_tcp_sockop_bind(php_stream *stream, php_netstream_data_t *sock,
		php_stream_xport_param *xparam TSRMLS_DC)
{
	char *host = NULL;
	int portno, err;

#ifdef AF_UNIX
	if (stream->ops == &php_stream_unix_socket_ops || stream->ops == &php_stream_unixdg_socket_ops) {
		struct sockaddr_un unix_addr;

		sock->socket = socket(PF_UNIX, stream->ops == &php_stream_unix_socket_ops ? SOCK_STREAM : SOCK_DGRAM, 0);

		if (sock->socket == SOCK_ERR) {
			if (xparam->want_errortext) {
				spprintf(&xparam->outputs.error_text, 0, "Failed to create unix%s socket %s",
						stream->ops == &php_stream_unix_socket_ops ? "" : "datagram",
						strerror(errno));
			}
			return -1;
		}

		parse_unix_address(xparam, &unix_addr TSRMLS_CC);

		return bind(sock->socket, &unix_addr, sizeof(unix_addr));
	}
#endif

	host = parse_ip_address(xparam, &portno TSRMLS_CC);

	if (host == NULL) {
		return -1;
	}

	sock->socket = php_network_bind_socket_to_local_addr(host, portno,
			stream->ops == &php_stream_udp_socket_ops ? SOCK_DGRAM : SOCK_STREAM,
			xparam->want_errortext ? &xparam->outputs.error_text : NULL,
			&err
			TSRMLS_CC);
	
	if (host) {
		efree(host);
	}

	return sock->socket == -1 ? -1 : 0;
}

static inline int php_tcp_sockop_connect(php_stream *stream, php_netstream_data_t *sock,
		php_stream_xport_param *xparam TSRMLS_DC)
{
	char *host = NULL;
	int portno;
	int err;
	int ret;

#ifdef AF_UNIX
	if (stream->ops == &php_stream_unix_socket_ops || stream->ops == &php_stream_unixdg_socket_ops) {
		struct sockaddr_un unix_addr;

		sock->socket = socket(PF_UNIX, stream->ops == &php_stream_unix_socket_ops ? SOCK_STREAM : SOCK_DGRAM, 0);

		if (sock->socket == SOCK_ERR) {
			if (xparam->want_errortext) {
				spprintf(&xparam->outputs.error_text, 0, "Failed to create unix socket");
			}
			return -1;
		}

		parse_unix_address(xparam, &unix_addr TSRMLS_CC);

		ret = php_network_connect_socket(sock->socket,
				(const struct sockaddr *)&unix_addr, (socklen_t)sizeof(unix_addr),
				xparam->op == STREAM_XPORT_OP_CONNECT_ASYNC, xparam->inputs.timeout,
				xparam->want_errortext ? &xparam->outputs.error_text : NULL,
				&err);

		xparam->outputs.error_code = err;

		goto out;
	}
#endif

	host = parse_ip_address(xparam, &portno TSRMLS_CC);

	if (host == NULL) {
		return -1;
	}

	/* Note: the test here for php_stream_udp_socket_ops is important, because we
	 * want the default to be TCP sockets so that the openssl extension can
	 * re-use this code. */
	
	sock->socket = php_network_connect_socket_to_host(host, portno,
			stream->ops == &php_stream_udp_socket_ops ? SOCK_DGRAM : SOCK_STREAM,
			xparam->op == STREAM_XPORT_OP_CONNECT_ASYNC,
			xparam->inputs.timeout,
			xparam->want_errortext ? &xparam->outputs.error_text : NULL,
			&err
			TSRMLS_CC);
	
	ret = sock->socket == -1 ? -1 : 0;
	xparam->outputs.error_code = err;

	if (host) {
		efree(host);
	}

#ifdef AF_UNIX
out:
#endif

	if (ret >= 0 && xparam->op == STREAM_XPORT_OP_CONNECT_ASYNC && err == EINPROGRESS) {
		/* indicates pending connection */
		return 1;
	}
	
	return ret;
}

static inline int php_tcp_sockop_accept(php_stream *stream, php_netstream_data_t *sock,
		php_stream_xport_param *xparam STREAMS_DC TSRMLS_DC)
{
	int clisock;

	xparam->outputs.client = NULL;

	clisock = php_network_accept_incoming(sock->socket,
			xparam->want_textaddr ? &xparam->outputs.textaddr : NULL,
			xparam->want_textaddr ? &xparam->outputs.textaddrlen : NULL,
			xparam->want_addr ? &xparam->outputs.addr : NULL,
			xparam->want_addr ? &xparam->outputs.addrlen : NULL,
			xparam->inputs.timeout,
			xparam->want_errortext ? &xparam->outputs.error_text : NULL,
			&xparam->outputs.error_code
			TSRMLS_CC);

	if (clisock >= 0) {
		php_netstream_data_t *clisockdata;

		clisockdata = pemalloc(sizeof(*clisockdata), stream->is_persistent);

		if (clisockdata == NULL) {
			close(clisock);
			/* technically a fatal error */
		} else {
			memcpy(clisockdata, sock, sizeof(*clisockdata));
			clisockdata->socket = clisock;

			xparam->outputs.client = php_stream_alloc_rel(stream->ops, clisockdata, NULL, "r+");
			if (xparam->outputs.client) {
				xparam->outputs.client->context = stream->context;
			}
		}
	}
	
	return xparam->outputs.client == NULL ? -1 : 0;
}

static int php_tcp_sockop_set_option(php_stream *stream, int option, int value, void *ptrparam TSRMLS_DC)
{
	php_netstream_data_t *sock = (php_netstream_data_t*)stream->abstract;
	php_stream_xport_param *xparam;

	switch(option) {
		case PHP_STREAM_OPTION_XPORT_API:
			xparam = (php_stream_xport_param *)ptrparam;

			switch(xparam->op) {
				case STREAM_XPORT_OP_CONNECT:
				case STREAM_XPORT_OP_CONNECT_ASYNC:
					xparam->outputs.returncode = php_tcp_sockop_connect(stream, sock, xparam TSRMLS_CC);
					return PHP_STREAM_OPTION_RETURN_OK;

				case STREAM_XPORT_OP_BIND:
					xparam->outputs.returncode = php_tcp_sockop_bind(stream, sock, xparam TSRMLS_CC);
					return PHP_STREAM_OPTION_RETURN_OK;


				case STREAM_XPORT_OP_ACCEPT:
					xparam->outputs.returncode = php_tcp_sockop_accept(stream, sock, xparam STREAMS_CC TSRMLS_CC);
					return PHP_STREAM_OPTION_RETURN_OK;
				default:
					/* fall through */
					;
			}
			
			/* fall through */
		default:
			return php_sockop_set_option(stream, option, value, ptrparam TSRMLS_CC);
	}
}


PHPAPI php_stream *php_stream_generic_socket_factory(const char *proto, long protolen,
		char *resourcename, long resourcenamelen,
		const char *persistent_id, int options, int flags,
		struct timeval *timeout,
		php_stream_context *context STREAMS_DC TSRMLS_DC)
{
	php_stream *stream = NULL;
	php_netstream_data_t *sock;
	php_stream_ops *ops;

	/* which type of socket ? */
	if (strncmp(proto, "tcp", protolen) == 0) {
		ops = &php_stream_socket_ops;
	} else if (strncmp(proto, "udp", protolen) == 0) {
		ops = &php_stream_udp_socket_ops;
	}
#ifdef AF_UNIX
	else if (strncmp(proto, "unix", protolen) == 0) {
		ops = &php_stream_unix_socket_ops;
	} else if (strncmp(proto, "udg", protolen) == 0) {
		ops = &php_stream_unixdg_socket_ops;
	}
#endif
	else {
		/* should never happen */
		return NULL;
	}
	
	sock = pemalloc(sizeof(php_netstream_data_t), persistent_id ? 1 : 0);
	memset(sock, 0, sizeof(php_netstream_data_t));

	sock->is_blocked = 1;
	sock->timeout.tv_sec = FG(default_socket_timeout);
	sock->timeout.tv_usec = 0;

	/* we don't know the socket until we have determined if we are binding or
	 * connecting */
	sock->socket = -1;
	
	stream = php_stream_alloc_rel(ops, sock, persistent_id, "r+");

	if (stream == NULL)	{
		pefree(sock, persistent_id ? 1 : 0);
		return NULL;
	}

	if (flags == 0) {
		return stream;
	}

	return stream;
}


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
