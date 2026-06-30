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

/* The base socket data is embedded first so the generic socket option handlers
 * can be reused; the datagram BIO is owned by ssl_handle (freed with it). */
typedef struct _php_openssl_dtls_data_t {
	php_netstream_data_t s;
	SSL_CTX *ctx;
	SSL *ssl_handle;
} php_openssl_dtls_data_t;

/* The socket is non-blocking, so on WANT_* we poll (up to the read timeout when
 * blocking) and retry. */
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

	dtlssock->s.timeout_event = false;

	/* Bound the total time across retries, not each individual poll. */
	struct timeval deadline;
	bool has_deadline = false;
	if (dtlssock->s.is_blocked && (dtlssock->s.timeout.tv_sec > 0 || dtlssock->s.timeout.tv_usec > 0)) {
		gettimeofday(&deadline, NULL);
		deadline.tv_sec += dtlssock->s.timeout.tv_sec;
		deadline.tv_usec += dtlssock->s.timeout.tv_usec;
		if (deadline.tv_usec >= 1000000) {
			deadline.tv_sec++;
			deadline.tv_usec -= 1000000;
		}
		has_deadline = true;
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
				/* Peer sent close_notify. */
				stream->eof = 1;
				return 0;
			default:
				if (read) {
					stream->eof = 1;
				}
				return -1;
		}

		/* Non-blocking: nothing available right now. */
		if (!dtlssock->s.is_blocked) {
			return -1;
		}

		int wait_ms = -1;
		if (has_deadline) {
			struct timeval now;
			gettimeofday(&now, NULL);
			long remaining = (deadline.tv_sec - now.tv_sec) * 1000L
					+ (deadline.tv_usec - now.tv_usec) / 1000;
			if (remaining <= 0) {
				dtlssock->s.timeout_event = true;
				return -1;
			}
			wait_ms = remaining > INT_MAX ? INT_MAX : (int)remaining;
		}

		int ready = php_pollfd_for_ms(dtlssock->s.socket, events, wait_ms);
		if (ready == 0) {
			dtlssock->s.timeout_event = true;
			return -1;
		}
		if (ready < 0) {
			return -1;
		}
	}
}

/* Send one datagram of application data. */
static ssize_t php_openssl_dtls_sockop_write(php_stream *stream, const char *buf, size_t count)
{
	return php_openssl_dtls_io(false, stream, (char *)buf, count);
}

/* Receive one datagram of application data. */
static ssize_t php_openssl_dtls_sockop_read(php_stream *stream, char *buf, size_t count)
{
	return php_openssl_dtls_io(true, stream, buf, count);
}

/* Free the DTLS objects and close the socket. */
static int php_openssl_dtls_sockop_close(php_stream *stream, int close_handle)
{
	php_openssl_dtls_data_t *dtlssock = (php_openssl_dtls_data_t *)stream->abstract;

	if (dtlssock == NULL) {
		return 0;
	}

	/* SSL_free also frees the BIO (created BIO_NOCLOSE), so the socket is closed
	 * separately below. */
	if (dtlssock->ssl_handle != NULL) {
		SSL_free(dtlssock->ssl_handle);
		dtlssock->ssl_handle = NULL;
	}
	if (dtlssock->ctx != NULL) {
		SSL_CTX_free(dtlssock->ctx);
		dtlssock->ctx = NULL;
	}

	if (close_handle && dtlssock->s.socket != SOCK_ERR) {
		closesocket(dtlssock->s.socket);
		dtlssock->s.socket = SOCK_ERR;
	}

	pefree(dtlssock, php_stream_is_persistent(stream));
	stream->abstract = NULL;

	return 0;
}

/* Returns an emalloc'd host (caller frees) and a port, or NULL on error.
 *
 * TODO: duplicates the static parse_ip_address_ex() in xp_socket.c. */
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

/* Create the DTLS context, SSL object and datagram BIO. */
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

	bio = BIO_new_dgram(dtlssock->s.socket, BIO_NOCLOSE);
	if (bio == NULL) {
		php_stream_warn(stream, CreateFailed, "DTLS datagram BIO creation failure");
		SSL_free(dtlssock->ssl_handle);
		dtlssock->ssl_handle = NULL;
		SSL_CTX_free(dtlssock->ctx);
		dtlssock->ctx = NULL;
		return -1;
	}

	/* A datagram BIO defaults to sendto() with an empty peer, which fails with
	 * EINVAL on a connected socket; mark it connected so it uses send()/recv(). */
	{
		struct sockaddr_storage peer;
		socklen_t peerlen = sizeof(peer);
		if (getpeername(dtlssock->s.socket, (struct sockaddr *)&peer, &peerlen) == 0) {
			BIO_ctrl(bio, BIO_CTRL_DGRAM_SET_CONNECTED, 0, &peer);
		}
	}

	SSL_set_bio(dtlssock->ssl_handle, bio, bio);
	SSL_set_connect_state(dtlssock->ssl_handle);

	return 0;
}

/* The socket is non-blocking, so poll between flights and resend on the
 * retransmission timeout. */
static int php_openssl_dtls_handshake(php_stream *stream, php_openssl_dtls_data_t *dtlssock,
		php_stream_xport_param *xparam)
{
	SSL *ssl = dtlssock->ssl_handle;

	for (;;) {
		int n = SSL_do_handshake(ssl);
		if (n == 1) {
			return 0;
		}
		int saved_errno = errno;

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
				} else if (saved_errno != 0) {
					/* SSL_ERROR_SYSCALL with an empty queue: report the syscall. */
					snprintf(buf, sizeof(buf), "%s", strerror(saved_errno));
				}
				if (xparam->want_errortext) {
					xparam->outputs.error_text = strpprintf(0, "DTLS handshake failed: %s",
							buf[0] != '\0' ? buf : "unexpected error");
				}
				return -1;
			}
		}

		struct timeval tv;
		int timeout = DTLSv1_get_timeout(ssl, &tv)
				? (int)(tv.tv_sec * 1000 + tv.tv_usec / 1000)
				: -1;

		int ready = php_pollfd_for_ms(dtlssock->s.socket, events, timeout);
		if (ready == 0) {
			/* Timer fired: let OpenSSL resend the last flight. */
			if (DTLSv1_handle_timeout(ssl) < 0) {
				if (xparam->want_errortext) {
					xparam->outputs.error_text = ZSTR_INIT_LITERAL("DTLS handshake timed out", 0);
				}
				return -1;
			}
		} else if (ready < 0) {
			if (xparam->want_errortext) {
				xparam->outputs.error_text =
						ZSTR_INIT_LITERAL("DTLS handshake failed while waiting for the socket", 0);
			}
			return -1;
		}
	}
}

/* Connect the UDP socket and run the DTLS handshake. */
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

	dtlssock->s.socket = php_network_connect_socket_to_host(host, (unsigned short)portno,
			SOCK_DGRAM,
			xparam->op == STREAM_XPORT_OP_CONNECT_ASYNC,
			xparam->inputs.timeout,
			xparam->want_errortext ? &xparam->outputs.error_text : NULL,
			&err, NULL, 0, STREAM_SOCKOP_NONE);

	xparam->outputs.error_code = err;
	efree(host);

	if (dtlssock->s.socket == SOCK_ERR) {
		return -1;
	}

	/* DTLS drives its own retransmission timers, so the socket must be
	 * non-blocking for the handshake and I/O loops. */
	php_set_sock_blocking(dtlssock->s.socket, 0);

	if (php_openssl_dtls_setup_crypto(stream, dtlssock) != 0) {
		return -1;
	}

	return php_openssl_dtls_handshake(stream, dtlssock, xparam);
}

/* Expose the fd for stream_select(); the raw fd is not handed out otherwise,
 * since DTLS is always encrypted. */
static int php_openssl_dtls_sockop_cast(php_stream *stream, int castas, void **ret)
{
	php_openssl_dtls_data_t *dtlssock = (php_openssl_dtls_data_t *)stream->abstract;

	switch (castas) {
		case PHP_STREAM_AS_FD_FOR_SELECT:
			if (ret != NULL) {
				/* Decrypted data buffered in OpenSSL is invisible to select(), so push
				 * it into the read buffer. TODO: same idiom as xp_ssl.c. */
				size_t pending;
				if (stream->writepos == stream->readpos
						&& dtlssock->ssl_handle != NULL
						&& (pending = (size_t)SSL_pending(dtlssock->ssl_handle)) > 0) {
					php_stream_fill_read_buffer(stream, pending < stream->chunk_size
							? pending
							: stream->chunk_size);
				}
				*(php_socket_t *)ret = dtlssock->s.socket;
			}
			return SUCCESS;

		default:
			return FAILURE;
	}
}

/* Handle transport and stream options. */
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
			/* The fd must stay non-blocking for the DTLS timers, so only track the
			 * stream-level mode (the base handler would flip the fd too). */
			int old = dtlssock->s.is_blocked;
			dtlssock->s.is_blocked = value;
			return old;
		}

		case PHP_STREAM_OPTION_META_DATA_API:
		case PHP_STREAM_OPTION_READ_TIMEOUT:
			/* The embedded base socket data is what these operate on. */
			return php_stream_socket_ops.set_option(stream, option, value, ptrparam);

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

/* Allocate a dtls:// stream. */
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
	dtlssock->s.socket = -1;
	dtlssock->s.is_blocked = true;
	dtlssock->s.timeout.tv_sec = (time_t)FG(default_socket_timeout);
	dtlssock->s.timeout.tv_usec = 0;

	stream = php_stream_alloc_rel(&php_openssl_dtls_socket_ops, dtlssock, persistent_id, "r+");
	if (stream == NULL) {
		pefree(dtlssock, persistent_id ? 1 : 0);
	}

	return stream;
}

#endif /* OPENSSL_NO_DTLS */
