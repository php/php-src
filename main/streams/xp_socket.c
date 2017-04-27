/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2017 The PHP Group                                |
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

#ifndef MSG_DONTWAIT
# define MSG_DONTWAIT 0
#endif

#ifndef MSG_PEEK
# define MSG_PEEK 0
#endif

#ifdef PHP_WIN32
/* send/recv family on windows expects int */
# define XP_SOCK_BUF_SIZE(sz) (((sz) > INT_MAX) ? INT_MAX : (int)(sz))
#else
# define XP_SOCK_BUF_SIZE(sz) (sz)
#endif

php_stream_ops php_stream_generic_socket_ops;
PHPAPI php_stream_ops php_stream_socket_ops;
php_stream_ops php_stream_udp_socket_ops;
#ifdef AF_UNIX
php_stream_ops php_stream_unix_socket_ops;
php_stream_ops php_stream_unixdg_socket_ops;
#endif


static int php_tcp_sockop_set_option(php_stream *stream, int option, int value, void *ptrparam);

/* {{{ Generic socket stream operations */
static size_t php_sockop_write(php_stream *stream, const char *buf, size_t count)
{
	php_netstream_data_t *sock = (php_netstream_data_t*)stream->abstract;
	int didwrite;
	struct timeval *ptimeout;

	if (!sock || sock->socket == -1) {
		return 0;
	}

	if (sock->timeout.tv_sec == -1)
		ptimeout = NULL;
	else
		ptimeout = &sock->timeout;

retry:
	didwrite = send(sock->socket, buf, XP_SOCK_BUF_SIZE(count), (sock->is_blocked && ptimeout) ? MSG_DONTWAIT : 0);

	if (didwrite <= 0) {
		int err = php_socket_errno();
		char *estr;

		if (sock->is_blocked && (err == EWOULDBLOCK || err == EAGAIN)) {
			int retval;

			sock->timeout_event = 0;

			do {
				retval = php_pollfd_for(sock->socket, POLLOUT, ptimeout);

				if (retval == 0) {
					sock->timeout_event = 1;
					break;
				}

				if (retval > 0) {
					/* writable now; retry */
					goto retry;
				}

				err = php_socket_errno();
			} while (err == EINTR);
		}
		estr = php_socket_strerror(err, NULL, 0);
		php_error_docref(NULL, E_NOTICE, "send of " ZEND_LONG_FMT " bytes failed with errno=%ld %s",
				(zend_long)count, err, estr);
		efree(estr);
	}

	if (didwrite > 0) {
		php_stream_notify_progress_increment(PHP_STREAM_CONTEXT(stream), didwrite, 0);
	}

	if (didwrite < 0) {
		didwrite = 0;
	}

	return didwrite;
}

static void php_sock_stream_wait_for_data(php_stream *stream, php_netstream_data_t *sock)
{
	int retval;
	struct timeval *ptimeout;

	if (!sock || sock->socket == -1) {
		return;
	}

	sock->timeout_event = 0;

	if (sock->timeout.tv_sec == -1)
		ptimeout = NULL;
	else
		ptimeout = &sock->timeout;

	while(1) {
		retval = php_pollfd_for(sock->socket, PHP_POLLREADABLE, ptimeout);

		if (retval == 0)
			sock->timeout_event = 1;

		if (retval >= 0)
			break;

		if (php_socket_errno() != EINTR)
			break;
	}
}

static size_t php_sockop_read(php_stream *stream, char *buf, size_t count)
{
	php_netstream_data_t *sock = (php_netstream_data_t*)stream->abstract;
	ssize_t nr_bytes = 0;
	int err;

	if (!sock || sock->socket == -1) {
		return 0;
	}

	if (sock->is_blocked) {
		php_sock_stream_wait_for_data(stream, sock);
		if (sock->timeout_event)
			return 0;
	}

	nr_bytes = recv(sock->socket, buf, XP_SOCK_BUF_SIZE(count), (sock->is_blocked && sock->timeout.tv_sec != -1) ? MSG_DONTWAIT : 0);
	err = php_socket_errno();

	stream->eof = (nr_bytes == 0 || (nr_bytes == -1 && err != EWOULDBLOCK && err != EAGAIN));

	if (nr_bytes > 0) {
		php_stream_notify_progress_increment(PHP_STREAM_CONTEXT(stream), nr_bytes, 0);
	}

	if (nr_bytes < 0) {
		nr_bytes = 0;
	}

	return nr_bytes;
}


static int php_sockop_close(php_stream *stream, int close_handle)
{
	php_netstream_data_t *sock = (php_netstream_data_t*)stream->abstract;
#ifdef PHP_WIN32
	int n;
#endif

	if (!sock) {
		return 0;
	}

	if (close_handle) {

#ifdef PHP_WIN32
		if (sock->socket == -1)
			sock->socket = SOCK_ERR;
#endif
		if (sock->socket != SOCK_ERR) {
#ifdef PHP_WIN32
			/* prevent more data from coming in */
			shutdown(sock->socket, SHUT_RD);

			/* try to make sure that the OS sends all data before we close the connection.
			 * Essentially, we are waiting for the socket to become writeable, which means
			 * that all pending data has been sent.
			 * We use a small timeout which should encourage the OS to send the data,
			 * but at the same time avoid hanging indefinitely.
			 * */
			do {
				n = php_pollfd_for_ms(sock->socket, POLLOUT, 500);
			} while (n == -1 && php_socket_errno() == EINTR);
#endif
			closesocket(sock->socket);
			sock->socket = SOCK_ERR;
		}

	}

	pefree(sock, php_stream_is_persistent(stream));

	return 0;
}

static int php_sockop_flush(php_stream *stream)
{
#if 0
	php_netstream_data_t *sock = (php_netstream_data_t*)stream->abstract;
	return fsync(sock->socket);
#endif
	return 0;
}

static int php_sockop_stat(php_stream *stream, php_stream_statbuf *ssb)
{
#if ZEND_WIN32
	return 0;
#else
	php_netstream_data_t *sock = (php_netstream_data_t*)stream->abstract;

	return zend_fstat(sock->socket, &ssb->sb);
#endif
}

static inline int sock_sendto(php_netstream_data_t *sock, const char *buf, size_t buflen, int flags,
		struct sockaddr *addr, socklen_t addrlen
		)
{
	int ret;
	if (addr) {
		ret = sendto(sock->socket, buf, XP_SOCK_BUF_SIZE(buflen), flags, addr, XP_SOCK_BUF_SIZE(addrlen));

		return (ret == SOCK_CONN_ERR) ? -1 : ret;
	}
#ifdef PHP_WIN32
	return ((ret = send(sock->socket, buf, buflen > INT_MAX ? INT_MAX : (int)buflen, flags)) == SOCK_CONN_ERR) ? -1 : ret;
#else
	return ((ret = send(sock->socket, buf, buflen, flags)) == SOCK_CONN_ERR) ? -1 : ret;
#endif
}

static inline int sock_recvfrom(php_netstream_data_t *sock, char *buf, size_t buflen, int flags,
		zend_string **textaddr,
		struct sockaddr **addr, socklen_t *addrlen
		)
{
	int ret;
	int want_addr = textaddr || addr;

	if (want_addr) {
		php_sockaddr_storage sa;
		socklen_t sl = sizeof(sa);
		ret = recvfrom(sock->socket, buf, XP_SOCK_BUF_SIZE(buflen), flags, (struct sockaddr*)&sa, &sl);
		ret = (ret == SOCK_CONN_ERR) ? -1 : ret;
		if (sl) {
			php_network_populate_name_from_sockaddr((struct sockaddr*)&sa, sl,
					textaddr, addr, addrlen);
		} else {
			if (textaddr) {
				*textaddr = ZSTR_EMPTY_ALLOC();
			}
			if (addr) {
				*addr = NULL;
				*addrlen = 0;
			}
		}
	} else {
		ret = recv(sock->socket, buf, XP_SOCK_BUF_SIZE(buflen), flags);
		ret = (ret == SOCK_CONN_ERR) ? -1 : ret;
	}

	return ret;
}

static int php_sockop_set_option(php_stream *stream, int option, int value, void *ptrparam)
{
	int oldmode, flags;
	php_netstream_data_t *sock = (php_netstream_data_t*)stream->abstract;
	php_stream_xport_param *xparam;

	if (!sock) {
		return PHP_STREAM_OPTION_RETURN_NOTIMPL;
	}

	switch(option) {
		case PHP_STREAM_OPTION_CHECK_LIVENESS:
			{
				struct timeval tv;
				char buf;
				int alive = 1;

				if (value == -1) {
					if (sock->timeout.tv_sec == -1) {
						tv.tv_sec = FG(default_socket_timeout);
						tv.tv_usec = 0;
					} else {
						tv = sock->timeout;
					}
				} else {
					tv.tv_sec = value;
					tv.tv_usec = 0;
				}

				if (sock->socket == -1) {
					alive = 0;
				} else if (php_pollfd_for(sock->socket, PHP_POLLREADABLE|POLLPRI, &tv) > 0) {
#ifdef PHP_WIN32
					int ret;
#else
					ssize_t ret;
#endif
					int err;

					ret = recv(sock->socket, &buf, sizeof(buf), MSG_PEEK);
					err = php_socket_errno();
					if (0 == ret || /* the counterpart did properly shutdown*/
						(0 > ret && err != EWOULDBLOCK && err != EAGAIN && err != EMSGSIZE)) { /* there was an unrecoverable error */
						alive = 0;
					}
				}
				return alive ? PHP_STREAM_OPTION_RETURN_OK : PHP_STREAM_OPTION_RETURN_ERR;
			}

		case PHP_STREAM_OPTION_BLOCKING:
			oldmode = sock->is_blocked;
			if (SUCCESS == php_set_sock_blocking(sock->socket, value)) {
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
					xparam->outputs.returncode = (listen(sock->socket, xparam->inputs.backlog) == 0) ?  0: -1;
					return PHP_STREAM_OPTION_RETURN_OK;

				case STREAM_XPORT_OP_GET_NAME:
					xparam->outputs.returncode = php_network_get_sock_name(sock->socket,
							xparam->want_textaddr ? &xparam->outputs.textaddr : NULL,
							xparam->want_addr ? &xparam->outputs.addr : NULL,
							xparam->want_addr ? &xparam->outputs.addrlen : NULL
							);
					return PHP_STREAM_OPTION_RETURN_OK;

				case STREAM_XPORT_OP_GET_PEER_NAME:
					xparam->outputs.returncode = php_network_get_peer_name(sock->socket,
							xparam->want_textaddr ? &xparam->outputs.textaddr : NULL,
							xparam->want_addr ? &xparam->outputs.addr : NULL,
							xparam->want_addr ? &xparam->outputs.addrlen : NULL
							);
					return PHP_STREAM_OPTION_RETURN_OK;

				case STREAM_XPORT_OP_SEND:
					flags = 0;
					if ((xparam->inputs.flags & STREAM_OOB) == STREAM_OOB) {
						flags |= MSG_OOB;
					}
					xparam->outputs.returncode = sock_sendto(sock,
							xparam->inputs.buf, xparam->inputs.buflen,
							flags,
							xparam->inputs.addr,
							xparam->inputs.addrlen);
					if (xparam->outputs.returncode == -1) {
						char *err = php_socket_strerror(php_socket_errno(), NULL, 0);
						php_error_docref(NULL, E_WARNING,
						   	"%s\n", err);
						efree(err);
					}
					return PHP_STREAM_OPTION_RETURN_OK;

				case STREAM_XPORT_OP_RECV:
					flags = 0;
					if ((xparam->inputs.flags & STREAM_OOB) == STREAM_OOB) {
						flags |= MSG_OOB;
					}
					if ((xparam->inputs.flags & STREAM_PEEK) == STREAM_PEEK) {
						flags |= MSG_PEEK;
					}
					xparam->outputs.returncode = sock_recvfrom(sock,
							xparam->inputs.buf, xparam->inputs.buflen,
							flags,
							xparam->want_textaddr ? &xparam->outputs.textaddr : NULL,
							xparam->want_addr ? &xparam->outputs.addr : NULL,
							xparam->want_addr ? &xparam->outputs.addrlen : NULL
							);
					return PHP_STREAM_OPTION_RETURN_OK;


#ifdef HAVE_SHUTDOWN
# ifndef SHUT_RD
#  define SHUT_RD 0
# endif
# ifndef SHUT_WR
#  define SHUT_WR 1
# endif
# ifndef SHUT_RDWR
#  define SHUT_RDWR 2
# endif
				case STREAM_XPORT_OP_SHUTDOWN: {
					static const int shutdown_how[] = {SHUT_RD, SHUT_WR, SHUT_RDWR};

					xparam->outputs.returncode = shutdown(sock->socket, shutdown_how[xparam->how]);
					return PHP_STREAM_OPTION_RETURN_OK;
				}
#endif

				default:
					return PHP_STREAM_OPTION_RETURN_NOTIMPL;
			}

		default:
			return PHP_STREAM_OPTION_RETURN_NOTIMPL;
	}
}

static int php_sockop_cast(php_stream *stream, int castas, void **ret)
{
	php_netstream_data_t *sock = (php_netstream_data_t*)stream->abstract;

	if (!sock) {
		return FAILURE;
	}

	switch(castas)	{
		case PHP_STREAM_AS_STDIO:
			if (ret)	{
				*(FILE**)ret = fdopen(sock->socket, stream->mode);
				if (*ret)
					return SUCCESS;
				return FAILURE;
			}
			return SUCCESS;
		case PHP_STREAM_AS_FD_FOR_SELECT:
		case PHP_STREAM_AS_FD:
		case PHP_STREAM_AS_SOCKETD:
			if (ret)
				*(php_socket_t *)ret = sock->socket;
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
static inline int parse_unix_address(php_stream_xport_param *xparam, struct sockaddr_un *unix_addr)
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
		php_error_docref(NULL, E_NOTICE,
			"socket path exceeded the maximum allowed length of %lu bytes "
			"and was truncated", (unsigned long)sizeof(unix_addr->sun_path));
	}

	memcpy(unix_addr->sun_path, xparam->inputs.name, xparam->inputs.namelen);

	return 1;
}
#endif

static inline char *parse_ip_address_ex(const char *str, size_t str_len, int *portno, int get_err, zend_string **err)
{
	char *colon;
	char *host = NULL;

#ifdef HAVE_IPV6
	char *p;

	if (*(str) == '[' && str_len > 1) {
		/* IPV6 notation to specify raw address with port (i.e. [fe80::1]:80) */
		p = memchr(str + 1, ']', str_len - 2);
		if (!p || *(p + 1) != ':') {
			if (get_err) {
				*err = strpprintf(0, "Failed to parse IPv6 address \"%s\"", str);
			}
			return NULL;
		}
		*portno = atoi(p + 2);
		return estrndup(str + 1, p - str - 1);
	}
#endif
	if (str_len) {
		colon = memchr(str, ':', str_len - 1);
	} else {
		colon = NULL;
	}
	if (colon) {
		*portno = atoi(colon + 1);
		host = estrndup(str, colon - str);
	} else {
		if (get_err) {
			*err = strpprintf(0, "Failed to parse address \"%s\"", str);
		}
		return NULL;
	}

	return host;
}

static inline char *parse_ip_address(php_stream_xport_param *xparam, int *portno)
{
	return parse_ip_address_ex(xparam->inputs.name, xparam->inputs.namelen, portno, xparam->want_errortext, &xparam->outputs.error_text);
}

static inline int php_tcp_sockop_bind(php_stream *stream, php_netstream_data_t *sock,
		php_stream_xport_param *xparam)
{
	char *host = NULL;
	int portno, err;
	long sockopts = STREAM_SOCKOP_NONE;
	zval *tmpzval = NULL;

#ifdef AF_UNIX
	if (stream->ops == &php_stream_unix_socket_ops || stream->ops == &php_stream_unixdg_socket_ops) {
		struct sockaddr_un unix_addr;

		sock->socket = socket(PF_UNIX, stream->ops == &php_stream_unix_socket_ops ? SOCK_STREAM : SOCK_DGRAM, 0);

		if (sock->socket == SOCK_ERR) {
			if (xparam->want_errortext) {
				xparam->outputs.error_text = strpprintf(0, "Failed to create unix%s socket %s",
						stream->ops == &php_stream_unix_socket_ops ? "" : "datagram",
						strerror(errno));
			}
			return -1;
		}

		parse_unix_address(xparam, &unix_addr);

		return bind(sock->socket, (const struct sockaddr *)&unix_addr,
			(socklen_t) XtOffsetOf(struct sockaddr_un, sun_path) + xparam->inputs.namelen);
	}
#endif

	host = parse_ip_address(xparam, &portno);

	if (host == NULL) {
		return -1;
	}

#ifdef IPV6_V6ONLY
	if (PHP_STREAM_CONTEXT(stream)
		&& (tmpzval = php_stream_context_get_option(PHP_STREAM_CONTEXT(stream), "socket", "ipv6_v6only")) != NULL
		&& Z_TYPE_P(tmpzval) != IS_NULL
	) {
		sockopts |= STREAM_SOCKOP_IPV6_V6ONLY;
		sockopts |= STREAM_SOCKOP_IPV6_V6ONLY_ENABLED * zend_is_true(tmpzval);
	}
#endif

#ifdef SO_REUSEPORT
	if (PHP_STREAM_CONTEXT(stream)
		&& (tmpzval = php_stream_context_get_option(PHP_STREAM_CONTEXT(stream), "socket", "so_reuseport")) != NULL
		&& zend_is_true(tmpzval)
	) {
		sockopts |= STREAM_SOCKOP_SO_REUSEPORT;
	}
#endif

#ifdef SO_BROADCAST
	if (stream->ops == &php_stream_udp_socket_ops /* SO_BROADCAST is only applicable for UDP */
		&& PHP_STREAM_CONTEXT(stream)
		&& (tmpzval = php_stream_context_get_option(PHP_STREAM_CONTEXT(stream), "socket", "so_broadcast")) != NULL
		&& zend_is_true(tmpzval)
	) {
		sockopts |= STREAM_SOCKOP_SO_BROADCAST;
	}
#endif

	sock->socket = php_network_bind_socket_to_local_addr(host, portno,
			stream->ops == &php_stream_udp_socket_ops ? SOCK_DGRAM : SOCK_STREAM,
			sockopts,
			xparam->want_errortext ? &xparam->outputs.error_text : NULL,
			&err
			);

	if (host) {
		efree(host);
	}

	return sock->socket == -1 ? -1 : 0;
}

static inline int php_tcp_sockop_connect(php_stream *stream, php_netstream_data_t *sock,
		php_stream_xport_param *xparam)
{
	char *host = NULL, *bindto = NULL;
	int portno, bindport = 0;
	int err = 0;
	int ret;
	zval *tmpzval = NULL;
	long sockopts = STREAM_SOCKOP_NONE;

#ifdef AF_UNIX
	if (stream->ops == &php_stream_unix_socket_ops || stream->ops == &php_stream_unixdg_socket_ops) {
		struct sockaddr_un unix_addr;

		sock->socket = socket(PF_UNIX, stream->ops == &php_stream_unix_socket_ops ? SOCK_STREAM : SOCK_DGRAM, 0);

		if (sock->socket == SOCK_ERR) {
			if (xparam->want_errortext) {
				xparam->outputs.error_text = strpprintf(0, "Failed to create unix socket");
			}
			return -1;
		}

		parse_unix_address(xparam, &unix_addr);

		ret = php_network_connect_socket(sock->socket,
				(const struct sockaddr *)&unix_addr, (socklen_t) XtOffsetOf(struct sockaddr_un, sun_path) + xparam->inputs.namelen,
				xparam->op == STREAM_XPORT_OP_CONNECT_ASYNC, xparam->inputs.timeout,
				xparam->want_errortext ? &xparam->outputs.error_text : NULL,
				&err);

		xparam->outputs.error_code = err;

		goto out;
	}
#endif

	host = parse_ip_address(xparam, &portno);

	if (host == NULL) {
		return -1;
	}

	if (PHP_STREAM_CONTEXT(stream) && (tmpzval = php_stream_context_get_option(PHP_STREAM_CONTEXT(stream), "socket", "bindto")) != NULL) {
		if (Z_TYPE_P(tmpzval) != IS_STRING) {
			if (xparam->want_errortext) {
				xparam->outputs.error_text = strpprintf(0, "local_addr context option is not a string.");
			}
			efree(host);
			return -1;
		}
		bindto = parse_ip_address_ex(Z_STRVAL_P(tmpzval), Z_STRLEN_P(tmpzval), &bindport, xparam->want_errortext, &xparam->outputs.error_text);
	}

#ifdef SO_BROADCAST
	if (stream->ops == &php_stream_udp_socket_ops /* SO_BROADCAST is only applicable for UDP */
		&& PHP_STREAM_CONTEXT(stream)
		&& (tmpzval = php_stream_context_get_option(PHP_STREAM_CONTEXT(stream), "socket", "so_broadcast")) != NULL
		&& zend_is_true(tmpzval)
	) {
		sockopts |= STREAM_SOCKOP_SO_BROADCAST;
	}
#endif

	/* Note: the test here for php_stream_udp_socket_ops is important, because we
	 * want the default to be TCP sockets so that the openssl extension can
	 * re-use this code. */

	sock->socket = php_network_connect_socket_to_host(host, portno,
			stream->ops == &php_stream_udp_socket_ops ? SOCK_DGRAM : SOCK_STREAM,
			xparam->op == STREAM_XPORT_OP_CONNECT_ASYNC,
			xparam->inputs.timeout,
			xparam->want_errortext ? &xparam->outputs.error_text : NULL,
			&err,
			bindto,
			bindport,
			sockopts
			);

	ret = sock->socket == -1 ? -1 : 0;
	xparam->outputs.error_code = err;

	if (host) {
		efree(host);
	}
	if (bindto) {
		efree(bindto);
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
		php_stream_xport_param *xparam STREAMS_DC)
{
	int clisock;

	xparam->outputs.client = NULL;

	clisock = php_network_accept_incoming(sock->socket,
			xparam->want_textaddr ? &xparam->outputs.textaddr : NULL,
			xparam->want_addr ? &xparam->outputs.addr : NULL,
			xparam->want_addr ? &xparam->outputs.addrlen : NULL,
			xparam->inputs.timeout,
			xparam->want_errortext ? &xparam->outputs.error_text : NULL,
			&xparam->outputs.error_code
			);

	if (clisock >= 0) {
		php_netstream_data_t *clisockdata;

		clisockdata = emalloc(sizeof(*clisockdata));

		if (clisockdata == NULL) {
			close(clisock);
			/* technically a fatal error */
		} else {
			memcpy(clisockdata, sock, sizeof(*clisockdata));
			clisockdata->socket = clisock;

			xparam->outputs.client = php_stream_alloc_rel(stream->ops, clisockdata, NULL, "r+");
			if (xparam->outputs.client) {
				xparam->outputs.client->ctx = stream->ctx;
				if (stream->ctx) {
					GC_REFCOUNT(stream->ctx)++;
				}
			}
		}
	}

	return xparam->outputs.client == NULL ? -1 : 0;
}

static int php_tcp_sockop_set_option(php_stream *stream, int option, int value, void *ptrparam)
{
	php_netstream_data_t *sock = (php_netstream_data_t*)stream->abstract;
	php_stream_xport_param *xparam;

	switch(option) {
		case PHP_STREAM_OPTION_XPORT_API:
			xparam = (php_stream_xport_param *)ptrparam;

			switch(xparam->op) {
				case STREAM_XPORT_OP_CONNECT:
				case STREAM_XPORT_OP_CONNECT_ASYNC:
					xparam->outputs.returncode = php_tcp_sockop_connect(stream, sock, xparam);
					return PHP_STREAM_OPTION_RETURN_OK;

				case STREAM_XPORT_OP_BIND:
					xparam->outputs.returncode = php_tcp_sockop_bind(stream, sock, xparam);
					return PHP_STREAM_OPTION_RETURN_OK;


				case STREAM_XPORT_OP_ACCEPT:
					xparam->outputs.returncode = php_tcp_sockop_accept(stream, sock, xparam STREAMS_CC);
					return PHP_STREAM_OPTION_RETURN_OK;
				default:
					/* fall through */
					;
			}
	}
	return php_sockop_set_option(stream, option, value, ptrparam);
}


PHPAPI php_stream *php_stream_generic_socket_factory(const char *proto, size_t protolen,
		const char *resourcename, size_t resourcenamelen,
		const char *persistent_id, int options, int flags,
		struct timeval *timeout,
		php_stream_context *context STREAMS_DC)
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
