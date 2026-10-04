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
  | Authors: Wez Furlong <wez@thebrainroom.com>                          |
  |          Daniel Lowrey <rdlowrey@php.net>                            |
  |          Chris Wright <daverandom@php.net>                           |
  |          Jakub Zelenka <bukka@php.net>                               |
  +----------------------------------------------------------------------+
*/

/* Shared between the TLS stream implementation (xp_ssl.c) and the DTLS server port (xp_dtls.c) */

#ifndef PHP_OPENSSL_XP_SSL_H
#define PHP_OPENSSL_XP_SSL_H

#include "php.h"
#include "php_network.h"
#include "php_streams.h"
#include "ext/standard/file.h"
#include "streams/php_streams_int.h"
#include "xp_bio.h"

#include <openssl/ssl.h>

/* Flags for determining allowed stream crypto methods */
#define STREAM_CRYPTO_IS_CLIENT            (1<<0)
#define STREAM_CRYPTO_METHOD_SSLv2         (1<<1)
#define STREAM_CRYPTO_METHOD_SSLv3         (1<<2)
#define STREAM_CRYPTO_METHOD_TLSv1_0       (1<<3)
#define STREAM_CRYPTO_METHOD_TLSv1_1       (1<<4)
#define STREAM_CRYPTO_METHOD_TLSv1_2       (1<<5)
#define STREAM_CRYPTO_METHOD_TLSv1_3       (1<<6)
#define STREAM_CRYPTO_METHOD_DTLSv1_2      (1<<7)
#define STREAM_CRYPTO_METHOD_DTLSv1_3      (1<<8)

#ifndef OPENSSL_NO_TLS1_METHOD
#define HAVE_TLS1 1
#endif

#ifndef OPENSSL_NO_TLS1_1_METHOD
#define HAVE_TLS11 1
#endif

#ifndef OPENSSL_NO_TLS1_2_METHOD
#define HAVE_TLS12 1
#endif

#ifndef OPENSSL_NO_TLS1_3
#define HAVE_TLS13 1
#endif

#ifndef OPENSSL_NO_DTLS
#define HAVE_DTLS 1
#endif

#if defined(HAVE_DTLS) && defined(DTLS1_3_VERSION)
#define HAVE_DTLS13 1
#endif

#ifndef OPENSSL_NO_ECDH
#define HAVE_ECDH 1
#endif

#ifndef OPENSSL_NO_TLSEXT
#define HAVE_TLS_SNI 1
#define HAVE_TLS_ALPN 1
#endif

#ifndef LIBRESSL_VERSION_NUMBER
#define HAVE_SEC_LEVEL 1
#endif

#if OPENSSL_VERSION_NUMBER < 0x40000000L && !defined(OPENSSL_NO_SSL3)
#define HAVE_SSL3 1
#define PHP_OPENSSL_MIN_PROTO_VERSION STREAM_CRYPTO_METHOD_SSLv3
#else
#define PHP_OPENSSL_MIN_PROTO_VERSION STREAM_CRYPTO_METHOD_TLSv1_0
#endif
#ifdef HAVE_TLS13
#define PHP_OPENSSL_MAX_PROTO_VERSION STREAM_CRYPTO_METHOD_TLSv1_3
#else
#define PHP_OPENSSL_MAX_PROTO_VERSION STREAM_CRYPTO_METHOD_TLSv1_2
#endif

#define PHP_OPENSSL_MIN_DTLS_PROTO_VERSION STREAM_CRYPTO_METHOD_DTLSv1_2
#ifdef HAVE_DTLS13
#define PHP_OPENSSL_MAX_DTLS_PROTO_VERSION STREAM_CRYPTO_METHOD_DTLSv1_3
#else
#define PHP_OPENSSL_MAX_DTLS_PROTO_VERSION STREAM_CRYPTO_METHOD_DTLSv1_2
#endif

/* Simplify ssl context option retrieval */
#define GET_VER_OPT(_name) \
	(PHP_STREAM_CONTEXT(stream) && (val = php_stream_context_get_option(PHP_STREAM_CONTEXT(stream), "ssl", _name)) != NULL)
#define GET_VER_OPT_STRING(_name, _str) \
	do { \
		if (GET_VER_OPT(_name)) { \
			if (try_convert_to_string(val)) _str = Z_STRVAL_P(val); \
		} \
	} while (0)
#define GET_VER_OPT_STRINGL(_name, _str, _len) \
	do { \
		if (GET_VER_OPT(_name)) { \
			if (try_convert_to_string(val)) { \
				_str = Z_STRVAL_P(val); \
				_len = Z_STRLEN_P(val); \
			} \
		} \
	} while (0)
#define GET_VER_OPT_LONG(_name, _num) \
	do { \
		if (GET_VER_OPT(_name)) { \
			_num = zval_get_long(val); \
		} \
	} while (0)

/* Certificate contexts used for server-side SNI selection */
typedef struct _php_openssl_sni_cert_t {
	char *name;
	SSL_CTX *ctx;
} php_openssl_sni_cert_t;

/* Provides leaky bucket handhsake renegotiation rate-limiting  */
typedef struct _php_openssl_handshake_bucket_t {
	zend_long prev_handshake;
	zend_long limit;
	zend_long window;
	float tokens;
	unsigned should_close;
} php_openssl_handshake_bucket_t;

#ifdef HAVE_TLS_ALPN
/* Holds the available server ALPN protocols for negotiation */
typedef struct _php_openssl_alpn_ctx_t {
	unsigned char *data;
	unsigned short len;
} php_openssl_alpn_ctx;
#endif

/* Holds PSK callbacks */
typedef struct _php_openssl_psk_callbacks_t {
	int refcount;
	zend_fcall_info_cache client_cb;
	zend_fcall_info_cache server_cb;
} php_openssl_psk_callbacks_t;

#ifdef HAVE_TLS13
/* TLS 1.3 early data (0-RTT) handshake phase */
typedef enum {
	PHP_OPENSSL_EARLY_DATA_NONE = 0,
	PHP_OPENSSL_EARLY_DATA_ACTIVE,
	PHP_OPENSSL_EARLY_DATA_DONE,
} php_openssl_early_data_state_t;

/* Size of the buffer used to drain server-side early data chunk by chunk */
#define PHP_OPENSSL_EARLY_DATA_CHUNK 16384

/* Holds the server early data callback */
typedef struct _php_openssl_early_data_callbacks_t {
	int refcount;
	zend_fcall_info_cache read_cb;
} php_openssl_early_data_callbacks_t;
#endif

/* Holds session callback */
typedef struct _php_openssl_session_callbacks_t {
	int refcount;
	zend_fcall_info_cache new_cb;
	zend_fcall_info_cache get_cb;
	zend_fcall_info_cache remove_cb;
} php_openssl_session_callbacks_t;

/* The DTLS server port: the socket, the demultiplexer and the connections of a dtls:// server */
typedef struct _php_openssl_port php_openssl_port;

/* This implementation is very closely tied to the that of the native
 * sockets implemented in the core.
 * Don't try this technique in other extensions!
 * */
typedef struct _php_openssl_netstream_data_t {
	php_netstream_data_t s;
	SSL *ssl_handle;
	SSL_CTX *ctx;
	/* The ciphertext queues and transport, once crypto is set up */
	php_openssl_conn *conn;
	/* The inner stream carrying the ciphertext instead of a socket of our own */
	php_stream *inner;
	/* The DTLS server port of a listener or of a connection it accepted */
	php_openssl_port *port;
	struct timeval connect_timeout;
	int enable_on_connect;
	int is_client;
	int ssl_active;
	int last_status;
	php_stream_xport_crypt_method_t method;
	php_openssl_handshake_bucket_t *reneg;
	php_openssl_sni_cert_t *sni_certs;
	unsigned sni_cert_count;
#ifdef HAVE_TLS_ALPN
	php_openssl_alpn_ctx alpn_ctx;
#endif
	php_openssl_session_callbacks_t *session_callbacks;
	php_openssl_psk_callbacks_t *psk_callbacks;
	/* Identity buffer for TLS 1.3 client PSK whose lifetime outlives the
	 * psk_use_session_cb call but OpenSSL doesn't free it, so we own it. */
	unsigned char *psk_identity_buf;
	size_t psk_identity_len;
#ifdef HAVE_TLS13
	/* TLS 1.3 early data (0-RTT) */
	php_openssl_early_data_callbacks_t *early_data_callbacks;
	/* Client payload to send as early data, borrowed for the handshake */
	zend_string *early_data_send;
	size_t early_data_offset;
	php_openssl_early_data_state_t early_data_state;
#endif
	char *url_name;
	unsigned state_set:1;
	/* The next IO call must not wait: it only moves what is already there */
	unsigned peek_only:1;
	unsigned _spare:30;
} php_openssl_netstream_data_t;

/* xp_ssl.c, for the DTLS port */

extern const php_stream_ops php_openssl_dgram_socket_ops;

int php_openssl_get_ssl_stream_data_index(void);
/* Builds the SSL_CTX of the stream from its context options */
zend_result php_openssl_create_server_ctx(php_stream *stream, php_openssl_netstream_data_t *sslsock,
		int method_flags);
/* Path MTU discovery on a datagram socket */
void php_openssl_dgram_socket_setup(php_socket_t fd);
/* Shares the context and the callbacks of src with dst, counting the references */
void php_openssl_netstream_share_ctx(php_openssl_netstream_data_t *dst, const php_openssl_netstream_data_t *src);
/* Captures and verifies the peer certificate after a handshake; 1 when the connection may be
 * used, -1 when it was rejected */
int php_openssl_handshake_complete(php_stream *stream, php_openssl_netstream_data_t *sslsock);
/* Logs an SSL error of the connection; true when the call should be retried */
bool php_openssl_handle_ssl_error(php_stream *stream, php_openssl_netstream_data_t *sslsock, int nr_bytes);

/* xp_dtls.c */

#ifdef HAVE_DTLS
/* Sets up the port of a bound dtls:// server stream; -1 on failure */
int php_openssl_dtls_listen(php_stream *stream, php_openssl_netstream_data_t *sslsock);
/* Accepts the next connection of the port as a new stream in xparam->outputs.client */
int php_openssl_dtls_accept(php_stream *stream, php_openssl_netstream_data_t *sslsock,
		php_stream_xport_param *xparam STREAMS_DC);
/* Detaches a listener or an accepted connection from its port; true when the socket is the
 * port's and must not be closed by the stream */
bool php_openssl_dtls_detach(php_stream *stream, php_openssl_netstream_data_t *sslsock);
#endif

#endif /* PHP_OPENSSL_XP_SSL_H */
