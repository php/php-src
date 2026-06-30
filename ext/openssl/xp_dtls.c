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
#include "streams/php_streams_int.h"
#include "php_openssl.h"
#include "php_network.h"
#include <openssl/ssl.h>
#include <openssl/bio.h>

#ifndef OPENSSL_NO_DTLS

/* Stream data backing a dtls:// transport. The datagram BIO is owned by
 * ssl_handle (attached with SSL_set_bio) and freed with it. */
typedef struct _php_openssl_dtls_data_t {
	php_socket_t socket;
	SSL_CTX *ctx;
	SSL *ssl_handle;
} php_openssl_dtls_data_t;

/* TODO: datagram read/write */
static ssize_t php_openssl_dtls_sockop_write(php_stream *stream, const char *buf, size_t count)
{
	return -1;
}

static ssize_t php_openssl_dtls_sockop_read(php_stream *stream, char *buf, size_t count)
{
	return -1;
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

	/* The UDP socket is already connected, so one datagram BIO serves both
	 * reads and writes. */
	SSL_set_bio(dtlssock->ssl_handle, bio, bio);
	SSL_set_connect_state(dtlssock->ssl_handle);

	return 0;
}

/* Create the UDP socket, connect it to the peer and attach the DTLS objects. */
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

	return php_openssl_dtls_setup_crypto(stream, dtlssock);
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

		default:
			return PHP_STREAM_OPTION_RETURN_NOTIMPL;
	}
}

static const php_stream_ops php_openssl_dtls_socket_ops = {
	php_openssl_dtls_sockop_write, php_openssl_dtls_sockop_read,
	php_openssl_dtls_sockop_close, NULL, /* flush */
	"udp_socket/dtls",
	NULL, /* seek */
	NULL, /* cast */
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

	stream = php_stream_alloc_rel(&php_openssl_dtls_socket_ops, dtlssock, persistent_id, "r+");
	if (stream == NULL) {
		pefree(dtlssock, persistent_id ? 1 : 0);
	}

	return stream;
}

#endif /* OPENSSL_NO_DTLS */
