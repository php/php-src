/*
  +----------------------------------------------------------------------+
  | Copyright © The PHP Group and Contributors.                          |
  +----------------------------------------------------------------------+
  | This source file is subject to the Modified BSD License that is      |
  | bundled with this package in the file LICENSE, and is available      |
  | through the World Wide Web at <https://www.php.net/license/>.        |
  |                                                                      |
  | SPDX-License-Identifier: BSD-3-Clause                                |
  +----------------------------------------------------------------------+
  | Authors: Gianfrancesco Aurecchia <gianfri@aurecchia.com>             |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "php.h"
#include "ext/standard/file.h"
#include "streams/php_streams_int.h"
#include "php_openssl.h"
#include "php_network.h"
#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/err.h>

#ifndef OPENSSL_NO_DTLS

/* Stream data backing a dtls:// transport. The datagram BIO is owned by
 * ssl_handle (attached with SSL_set_bio) and freed with it. The socket stays
 * non-blocking for the DTLS timers, so is_blocked tracks the stream-level mode
 * and timeout is the read timeout, both honoured by the I/O loop. */
typedef struct _php_openssl_dtls_data_t {
	php_socket_t socket;
	SSL_CTX *ctx;
	SSL *ssl_handle;
	bool is_blocked;
	struct timeval timeout;
} php_openssl_dtls_data_t;

/* Datagram I/O: each SSL_read/SSL_write maps to one DTLS record. The socket is
 * non-blocking, so when OpenSSL wants more I/O we poll (up to the read timeout
 * for a blocking stream) and retry. */
static ssize_t php_openssl_dtls_io(bool read, php_stream *stream, char *buf, size_t count)
{
	php_openssl_dtls_data_t *dtlssock = (php_openssl_dtls_data_t *)stream->abstract;
	SSL *ssl = dtlssock->ssl_handle;

	if (ssl == NULL) {
		return -1;
	}

	/* OpenSSL takes an int length. */
	if (count > INT_MAX) {
		count = INT_MAX;
	}

	int timeout_ms = -1;
	if (dtlssock->is_blocked && (dtlssock->timeout.tv_sec > 0 || dtlssock->timeout.tv_usec > 0)) {
		timeout_ms = (int)(dtlssock->timeout.tv_sec * 1000 + dtlssock->timeout.tv_usec / 1000);
	}

	for (;;) {
		ERR_clear_error();
		int n = read ? SSL_read(ssl, buf, (int)count) : SSL_write(ssl, buf, (int)count);
		if (n > 0) {
			return n;
		}

		int events;
		switch (SSL_get_error(ssl, n)) {
			case SSL_ERROR_WANT_READ:
				events = POLLIN;
				break;
			case SSL_ERROR_WANT_WRITE:
				events = POLLOUT;
				break;
			case SSL_ERROR_ZERO_RETURN:
				/* The peer sent a close_notify alert. */
				stream->eof = 1;
				return 0;
			default:
				if (read) {
					stream->eof = 1;
				}
				return -1;
		}

		/* Non-blocking stream: report that no data is available right now. */
		if (!dtlssock->is_blocked) {
			return -1;
		}

		if (php_pollfd_for_ms(dtlssock->socket, events, timeout_ms) <= 0) {
			/* Timed out or poll error. */
			return -1;
		}
	}
}

static ssize_t php_openssl_dtls_sockop_write(php_stream *stream, const char *buf, size_t count)
{
	return php_openssl_dtls_io(false, stream, (char *)buf, count);
}

static ssize_t php_openssl_dtls_sockop_read(php_stream *stream, char *buf, size_t count)
{
	return php_openssl_dtls_io(true, stream, buf, count);
}

static int php_openssl_dtls_sockop_close(php_stream *stream, int close_handle)
{
	php_openssl_dtls_data_t *dtlssock = (php_openssl_dtls_data_t *)stream->abstract;

	if (dtlssock == NULL) {
		return 0;
	}

	/* SSL_free also frees the datagram BIO attached with SSL_set_bio; the BIO was
	 * created BIO_NOCLOSE, so the socket itself is closed separately below. */
	if (dtlssock->ssl_handle != NULL) {
		SSL_free(dtlssock->ssl_handle);
		dtlssock->ssl_handle = NULL;
	}
	if (dtlssock->ctx != NULL) {
		SSL_CTX_free(dtlssock->ctx);
		dtlssock->ctx = NULL;
	}

	if (close_handle && dtlssock->socket != SOCK_ERR) {
		closesocket(dtlssock->socket);
		dtlssock->socket = SOCK_ERR;
	}

	pefree(dtlssock, php_stream_is_persistent(stream));
	stream->abstract = NULL;

	return 0;
}

/* Split "host:port" (or "[ipv6]:port") from the resource name into host and
 * port. Returns an emalloc'd host the caller must efree, or NULL on a parse
 * error, with xparam->outputs.error_text set when requested. */
static char *php_openssl_dtls_parse_ip_address(php_stream_xport_param *xparam, int *portno)
{
	const char *str = xparam->inputs.name;
	size_t str_len = xparam->inputs.namelen;
	const char *colon;

	if (str == NULL || memchr(str, '\0', str_len)) {
		if (xparam->want_errortext) {
			xparam->outputs.error_text = ZSTR_INIT_LITERAL("The hostname must not contain null bytes", 0);
		}
		return NULL;
	}

#ifdef HAVE_IPV6
	if (*str == '[' && str_len > 1) {
		/* [ipv6]:port notation, e.g. [fe80::1]:443 */
		const char *p = memchr(str + 1, ']', str_len - 2);
		if (p == NULL || *(p + 1) != ':') {
			if (xparam->want_errortext) {
				xparam->outputs.error_text = strpprintf(0, "Failed to parse IPv6 address \"%s\"", str);
			}
			return NULL;
		}
		*portno = atoi(p + 2);
		return estrndup(str + 1, p - str - 1);
	}
#endif

	colon = str_len ? memchr(str, ':', str_len - 1) : NULL;
	if (colon == NULL) {
		if (xparam->want_errortext) {
			xparam->outputs.error_text = strpprintf(0, "Failed to parse address \"%s\"", str);
		}
		return NULL;
	}

	*portno = atoi(colon + 1);
	return estrndup(str, colon - str);
}

/* Create the DTLS context and SSL object and bind a datagram BIO to the
 * connected UDP socket. */
static int php_openssl_dtls_setup_crypto(php_stream *stream, php_openssl_dtls_data_t *dtlssock)
{
	BIO *bio;

	dtlssock->ctx = SSL_CTX_new(DTLS_client_method());
	if (dtlssock->ctx == NULL) {
		php_stream_warn(stream, CreateFailed, "DTLS context creation failure");
		return -1;
	}

	dtlssock->ssl_handle = SSL_new(dtlssock->ctx);
	if (dtlssock->ssl_handle == NULL) {
		php_stream_warn(stream, CreateFailed, "DTLS handle creation failure");
		SSL_CTX_free(dtlssock->ctx);
		dtlssock->ctx = NULL;
		return -1;
	}

	bio = BIO_new_dgram(dtlssock->socket, BIO_NOCLOSE);
	if (bio == NULL) {
		php_stream_warn(stream, CreateFailed, "DTLS datagram BIO creation failure");
		SSL_free(dtlssock->ssl_handle);
		dtlssock->ssl_handle = NULL;
		SSL_CTX_free(dtlssock->ctx);
		dtlssock->ctx = NULL;
		return -1;
	}

	/* A datagram BIO defaults to sendto() with an empty peer address, which
	 * fails with EINVAL on our connected socket. Tell it the socket is connected
	 * (and to whom) so it uses send()/recv() instead. */
	{
		struct sockaddr_storage peer;
		socklen_t peerlen = sizeof(peer);
		if (getpeername(dtlssock->socket, (struct sockaddr *)&peer, &peerlen) == 0) {
			BIO_ctrl(bio, BIO_CTRL_DGRAM_SET_CONNECTED, 0, &peer);
		}
	}

	/* The UDP socket is already connected, so one datagram BIO serves both
	 * reads and writes. */
	SSL_set_bio(dtlssock->ssl_handle, bio, bio);
	SSL_set_connect_state(dtlssock->ssl_handle);

	return 0;
}

/* Drive the DTLS handshake to completion. The socket is non-blocking, so the
 * loop waits for readability or for the next retransmission timeout, after
 * which OpenSSL resends the last flight. */
static int php_openssl_dtls_handshake(php_stream *stream, php_openssl_dtls_data_t *dtlssock)
{
	SSL *ssl = dtlssock->ssl_handle;

	for (;;) {
		int n = SSL_do_handshake(ssl);
		if (n == 1) {
			return 0;
		}

		int events;
		switch (SSL_get_error(ssl, n)) {
			case SSL_ERROR_WANT_READ:
				events = POLLIN;
				break;
			case SSL_ERROR_WANT_WRITE:
				events = POLLOUT;
				break;
			default: {
				char buf[256] = "";
				unsigned long ecode = ERR_get_error();
				if (ecode != 0) {
					ERR_error_string_n(ecode, buf, sizeof(buf));
				}
				php_stream_warn(stream, ProtocolError, "DTLS handshake failed: %s",
						buf[0] != '\0' ? buf : "unexpected error");
				return -1;
			}
		}

		struct timeval tv;
		int timeout = DTLSv1_get_timeout(ssl, &tv)
				? (int)(tv.tv_sec * 1000 + tv.tv_usec / 1000)
				: -1;

		int ready = php_pollfd_for_ms(dtlssock->socket, events, timeout);
		if (ready == 0) {
			/* The retransmission timer fired: let OpenSSL resend the last flight. */
			if (DTLSv1_handle_timeout(ssl) < 0) {
				php_stream_warn(stream, ProtocolError, "DTLS handshake timed out");
				return -1;
			}
		} else if (ready < 0) {
			php_stream_warn(stream, ProtocolError, "DTLS handshake failed while waiting for the socket");
			return -1;
		}
	}
}

/* Create the UDP socket, connect it to the peer, attach the DTLS objects and
 * run the handshake. */
static int php_openssl_dtls_connect(php_stream *stream, php_openssl_dtls_data_t *dtlssock,
		php_stream_xport_param *xparam)
{
	char *host;
	int portno = 0;
	int err = 0;

	host = php_openssl_dtls_parse_ip_address(xparam, &portno);
	if (host == NULL) {
		return -1;
	}

	dtlssock->socket = php_network_connect_socket_to_host(host, (unsigned short)portno,
			SOCK_DGRAM,
			xparam->op == STREAM_XPORT_OP_CONNECT_ASYNC,
			xparam->inputs.timeout,
			xparam->want_errortext ? &xparam->outputs.error_text : NULL,
			&err, NULL, 0, STREAM_SOCKOP_NONE);

	xparam->outputs.error_code = err;
	efree(host);

	if (dtlssock->socket == SOCK_ERR) {
		return -1;
	}

	/* DTLS drives its own retransmission timers, so the socket must be
	 * non-blocking for the handshake and datagram I/O loops. */
	php_set_sock_blocking(dtlssock->socket, 0);

	if (php_openssl_dtls_setup_crypto(stream, dtlssock) != 0) {
		return -1;
	}

	return php_openssl_dtls_handshake(stream, dtlssock);
}

/* Expose the underlying fd so stream_select() and friends can wait on the
 * dtls:// stream. The raw fd/FILE* is never handed out otherwise, since DTLS is
 * always encrypted. */
static int php_openssl_dtls_sockop_cast(php_stream *stream, int castas, void **ret)
{
	php_openssl_dtls_data_t *dtlssock = (php_openssl_dtls_data_t *)stream->abstract;

	switch (castas) {
		case PHP_STREAM_AS_FD_FOR_SELECT:
			if (ret != NULL) {
				/* Data already decrypted and buffered in OpenSSL would not show up
				 * in a select() on the socket, so surface it into the read buffer. */
				size_t pending;
				if (stream->writepos == stream->readpos
						&& dtlssock->ssl_handle != NULL
						&& (pending = (size_t)SSL_pending(dtlssock->ssl_handle)) > 0) {
					php_stream_fill_read_buffer(stream, pending < stream->chunk_size
							? pending
							: stream->chunk_size);
				}
				*(php_socket_t *)ret = dtlssock->socket;
			}
			return SUCCESS;

		default:
			return FAILURE;
	}
}

static int php_openssl_dtls_sockop_set_option(php_stream *stream, int option, int value, void *ptrparam)
{
	php_openssl_dtls_data_t *dtlssock = (php_openssl_dtls_data_t *)stream->abstract;
	php_stream_xport_param *xparam;

	switch (option) {
		case PHP_STREAM_OPTION_XPORT_API:
			xparam = (php_stream_xport_param *)ptrparam;

			switch (xparam->op) {
				case STREAM_XPORT_OP_CONNECT:
				case STREAM_XPORT_OP_CONNECT_ASYNC:
					xparam->outputs.returncode =
							php_openssl_dtls_connect(stream, dtlssock, xparam);
					return PHP_STREAM_OPTION_RETURN_OK;

				default:
					return PHP_STREAM_OPTION_RETURN_NOTIMPL;
			}

		case PHP_STREAM_OPTION_BLOCKING: {
			/* The fd stays non-blocking for the DTLS timers; the stream-level
			 * blocking mode is emulated with poll in the I/O loop. */
			int old = dtlssock->is_blocked;
			dtlssock->is_blocked = value;
			return old;
		}

		case PHP_STREAM_OPTION_READ_TIMEOUT:
			dtlssock->timeout = *(struct timeval *)ptrparam;
			return PHP_STREAM_OPTION_RETURN_OK;

		default:
			return PHP_STREAM_OPTION_RETURN_NOTIMPL;
	}
}

static const php_stream_ops php_openssl_dtls_socket_ops = {
	php_openssl_dtls_sockop_write, php_openssl_dtls_sockop_read,
	php_openssl_dtls_sockop_close, NULL, /* flush */
	"udp_socket/dtls",
	NULL, /* seek */
	php_openssl_dtls_sockop_cast,
	NULL, /* stat */
	php_openssl_dtls_sockop_set_option,
};

php_stream *php_openssl_dtls_socket_factory(const char *proto, size_t protolen,
		const char *resourcename, size_t resourcenamelen,
		const char *persistent_id, int options, int flags,
		struct timeval *timeout,
		php_stream_context *context STREAMS_DC)
{
	php_openssl_dtls_data_t *dtlssock;
	php_stream *stream;

	dtlssock = pemalloc(sizeof(*dtlssock), persistent_id ? 1 : 0);
	memset(dtlssock, 0, sizeof(*dtlssock));
	dtlssock->socket = -1;
	dtlssock->is_blocked = true;
	dtlssock->timeout.tv_sec = (time_t)FG(default_socket_timeout);
	dtlssock->timeout.tv_usec = 0;

	stream = php_stream_alloc_rel(&php_openssl_dtls_socket_ops, dtlssock, persistent_id, "r+");
	if (stream == NULL) {
		pefree(dtlssock, persistent_id ? 1 : 0);
	}

	return stream;
}

#endif /* OPENSSL_NO_DTLS */
