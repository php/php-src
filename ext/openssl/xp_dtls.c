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
  |          Jakub Zelenka <bukka@php.net>                               |
  +----------------------------------------------------------------------+
*/

/* The dtls:// server port. One UDP socket carries every peer of a server: the port receives the
 * datagrams, routes each to its connection by the peer address, runs the handshakes of new
 * peers with a cookie exchange, and hands the connections that completed to accept() as
 * streams. The listener stream and every accepted stream share the port and its socket. */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "php.h"
#include "php_openssl.h"
#include "xp_ssl.h"

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/hmac.h>
#include <openssl/rand.h>
#include <openssl/crypto.h>

#ifdef HAVE_DTLS

#define PHP_OPENSSL_DTLS_MAX_PENDING 256
#define PHP_OPENSSL_DTLS_PENDING_TIMEOUT 30
/* Datagrams queued for a connection nobody is reading before the port drops the next */
#define PHP_OPENSSL_DTLS_RX_LIMIT 256
/* A cookie is a timestamp and an HMAC of it with the peer address */
#define PHP_OPENSSL_DTLS_COOKIE_TS_LEN 8
#define PHP_OPENSSL_DTLS_COOKIE_MAC_LEN 32
#define PHP_OPENSSL_DTLS_COOKIE_LEN (PHP_OPENSSL_DTLS_COOKIE_TS_LEN + PHP_OPENSSL_DTLS_COOKIE_MAC_LEN)
#define PHP_OPENSSL_DTLS_COOKIE_MAX_AGE 60

typedef enum {
	PHP_OPENSSL_DTLS_PEER_PENDING,
	PHP_OPENSSL_DTLS_PEER_READY,
	PHP_OPENSSL_DTLS_PEER_ACCEPTED,
} php_openssl_dtls_peer_state;

typedef struct _php_openssl_dtls_peer {
	php_openssl_port *port;
	php_openssl_conn *conn;
	SSL *ssl;
	zend_string *key;
	zend_hrtime_t created;
	php_openssl_dtls_peer_state state;
	/* The accepted stream's data, owner of ssl and conn from then on */
	php_openssl_netstream_data_t *sslsock;
	struct _php_openssl_dtls_peer *next;
} php_openssl_dtls_peer;

struct _php_openssl_port {
	int refcount;
	/* The listener while it is open */
	php_stream *stream;
	php_openssl_netstream_data_t *listener;
	php_openssl_xport *xport;
	php_socket_t fd;
	SSL_CTX *ctx;
	/* Every peer by address */
	HashTable peers;
	php_openssl_dtls_peer *pending;
	size_t pending_count;
	/* Handshakes completed, oldest first */
	php_openssl_dtls_peer *ready;
	php_openssl_dtls_peer *ready_tail;
	zend_long max_pending;
	zend_long pending_timeout;
	unsigned link_mtu;
	unsigned char cookie_secret[32];
	bool closed;
};

static int php_openssl_port_fill(php_openssl_conn *conn, php_openssl_deadline *dl);

/* Peers */

static zend_string *php_openssl_dtls_peer_key(const struct sockaddr *peer, socklen_t peerlen)
{
	return zend_string_init((const char *) peer, peerlen, 0);
}

static void php_openssl_dtls_peer_unlink(php_openssl_dtls_peer **list, php_openssl_dtls_peer *peer)
{
	while (*list != NULL && *list != peer) {
		list = &(*list)->next;
	}
	if (*list == peer) {
		*list = peer->next;
	}
	peer->next = NULL;
}

/* Frees a peer the port still owns: not accepted, or accepted and now closed by its stream */
static void php_openssl_dtls_peer_free(php_openssl_port *port, php_openssl_dtls_peer *peer)
{
	switch (peer->state) {
		case PHP_OPENSSL_DTLS_PEER_PENDING:
			php_openssl_dtls_peer_unlink(&port->pending, peer);
			port->pending_count--;
			break;
		case PHP_OPENSSL_DTLS_PEER_READY:
			php_openssl_dtls_peer_unlink(&port->ready, peer);
			if (port->ready == NULL) {
				port->ready_tail = NULL;
			} else if (port->ready_tail == peer) {
				port->ready_tail = port->ready;
				while (port->ready_tail->next) {
					port->ready_tail = port->ready_tail->next;
				}
			}
			break;
		case PHP_OPENSSL_DTLS_PEER_ACCEPTED:
			/* The stream owns the SSL and the connection */
			peer->ssl = NULL;
			peer->conn = NULL;
			break;
	}
	if (peer->key) {
		zend_hash_del(&port->peers, peer->key);
		zend_string_release(peer->key);
	}
	if (peer->ssl) {
		SSL_free(peer->ssl);
	}
	if (peer->conn) {
		php_openssl_conn_free(peer->conn);
	}
	efree(peer);
}

static php_openssl_dtls_peer *php_openssl_dtls_peer_of(php_openssl_conn *conn)
{
	return (php_openssl_dtls_peer *) conn->owner;
}

/* Cookies: HMAC of a timestamp and the peer address, keyed by a secret of the port */

static php_openssl_dtls_peer *php_openssl_dtls_peer_from_ssl(SSL *ssl)
{
	BIO *bio = SSL_get_rbio(ssl);
	php_openssl_conn *conn = bio ? BIO_get_data(bio) : NULL;
	return conn ? php_openssl_dtls_peer_of(conn) : NULL;
}

static void php_openssl_dtls_cookie_mac(php_openssl_port *port, const php_openssl_conn *conn,
		const unsigned char *ts, unsigned char *mac)
{
	unsigned char msg[PHP_OPENSSL_DTLS_COOKIE_TS_LEN + sizeof(conn->peer)];
	unsigned int len = 0;

	memcpy(msg, ts, PHP_OPENSSL_DTLS_COOKIE_TS_LEN);
	memcpy(msg + PHP_OPENSSL_DTLS_COOKIE_TS_LEN, &conn->peer, conn->peerlen);
	HMAC(EVP_sha256(), port->cookie_secret, sizeof(port->cookie_secret), msg,
			PHP_OPENSSL_DTLS_COOKIE_TS_LEN + conn->peerlen, mac, &len);
}

static int php_openssl_dtls_cookie_generate(SSL *ssl, unsigned char *cookie, unsigned int *cookie_len)
{
	php_openssl_dtls_peer *peer = php_openssl_dtls_peer_from_ssl(ssl);
	if (peer == NULL || peer->conn->peerlen == 0) {
		return 0;
	}

	uint64_t now = (uint64_t) time(NULL);
	for (int i = 0; i < PHP_OPENSSL_DTLS_COOKIE_TS_LEN; i++) {
		cookie[i] = (unsigned char) (now >> (8 * (PHP_OPENSSL_DTLS_COOKIE_TS_LEN - 1 - i)));
	}
	php_openssl_dtls_cookie_mac(peer->port, peer->conn, cookie, cookie + PHP_OPENSSL_DTLS_COOKIE_TS_LEN);
	*cookie_len = PHP_OPENSSL_DTLS_COOKIE_LEN;
	return 1;
}

static int php_openssl_dtls_cookie_verify(SSL *ssl, const unsigned char *cookie, unsigned int cookie_len)
{
	php_openssl_dtls_peer *peer = php_openssl_dtls_peer_from_ssl(ssl);
	if (peer == NULL || peer->conn->peerlen == 0 || cookie_len != PHP_OPENSSL_DTLS_COOKIE_LEN) {
		return 0;
	}

	uint64_t ts = 0;
	for (int i = 0; i < PHP_OPENSSL_DTLS_COOKIE_TS_LEN; i++) {
		ts = (ts << 8) | cookie[i];
	}
	uint64_t now = (uint64_t) time(NULL);
	if (ts > now || now - ts > PHP_OPENSSL_DTLS_COOKIE_MAX_AGE) {
		return 0;
	}

	unsigned char mac[PHP_OPENSSL_DTLS_COOKIE_MAC_LEN];
	php_openssl_dtls_cookie_mac(peer->port, peer->conn, cookie, mac);
	return CRYPTO_memcmp(mac, cookie + PHP_OPENSSL_DTLS_COOKIE_TS_LEN, sizeof(mac)) == 0;
}

static int php_openssl_dtls_stateless_cookie_generate(SSL *ssl, unsigned char *cookie, size_t *cookie_len)
{
	unsigned int len = 0;
	int ret = php_openssl_dtls_cookie_generate(ssl, cookie, &len);
	*cookie_len = len;
	return ret;
}

static int php_openssl_dtls_stateless_cookie_verify(SSL *ssl, const unsigned char *cookie, size_t cookie_len)
{
	return php_openssl_dtls_cookie_verify(ssl, cookie, (unsigned int) cookie_len);
}

/* The port */

static void php_openssl_port_release(php_openssl_port *port)
{
	if (--port->refcount > 0) {
		return;
	}
	ZEND_ASSERT(port->pending == NULL && port->ready == NULL);
	zend_hash_destroy(&port->peers);
	if (port->ctx) {
		SSL_CTX_free(port->ctx);
	}
	php_openssl_xport_release(port->xport);
	if (port->fd != SOCK_ERR) {
		closesocket(port->fd);
	}
	efree(port);
}

/* Sends what a pending handshake produced; what the socket refuses waits for the next tick */
static void php_openssl_dtls_peer_flush(php_openssl_dtls_peer *peer)
{
	php_openssl_conn_flush(peer->conn, NULL);
}

/* Moves a pending handshake as far as the datagrams received let it */
static void php_openssl_dtls_peer_drive(php_openssl_port *port, php_openssl_dtls_peer *peer)
{
	for (;;) {
		ERR_clear_error();
		int n = SSL_accept(peer->ssl);
		int err = n > 0 ? SSL_ERROR_NONE : SSL_get_error(peer->ssl, n);
		php_openssl_dtls_peer_flush(peer);

		if (n > 0) {
			php_openssl_dtls_peer_unlink(&port->pending, peer);
			port->pending_count--;
			peer->state = PHP_OPENSSL_DTLS_PEER_READY;
			if (port->ready_tail) {
				port->ready_tail->next = peer;
			} else {
				port->ready = peer;
			}
			port->ready_tail = peer;
			return;
		}
		if (err == SSL_ERROR_WANT_READ) {
			return;
		}
		if (err == SSL_ERROR_WANT_WRITE) {
			if (php_openssl_conn_tx_empty(peer->conn)) {
				continue;
			}
			return;
		}
		/* A failed handshake: the peer is forgotten, its next ClientHello starts over */
		ERR_clear_error();
		php_openssl_dtls_peer_free(port, peer);
		return;
	}
}

static php_openssl_dtls_peer *php_openssl_dtls_peer_new(php_openssl_port *port,
		const struct sockaddr *addr, socklen_t addrlen, zend_string *key)
{
	php_openssl_dtls_peer *peer = ecalloc(1, sizeof(*peer));
	peer->port = port;
	peer->state = PHP_OPENSSL_DTLS_PEER_PENDING;
	peer->created = zend_hrtime();

	peer->ssl = SSL_new(port->ctx);
	if (peer->ssl == NULL) {
		efree(peer);
		return NULL;
	}
	SSL_set_accept_state(peer->ssl);
	/* A cookie exchange before any state is kept for the peer: HelloVerifyRequest for DTLS 1.2 */
	SSL_set_options(peer->ssl, SSL_OP_COOKIE_EXCHANGE);
	SSL_set_mode(peer->ssl, SSL_MODE_RELEASE_BUFFERS | SSL_MODE_ENABLE_PARTIAL_WRITE
			| SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER);
	SSL_set_ex_data(peer->ssl, php_openssl_get_ssl_stream_data_index(), port->stream);

	peer->conn = php_openssl_conn_new(port->xport, port->stream, false);
	php_openssl_conn_set_peer(peer->conn, addr, addrlen);
	peer->conn->fill = php_openssl_port_fill;
	peer->conn->owner = peer;
	if (php_openssl_conn_set_ssl(peer->conn, peer->ssl) == FAILURE) {
		SSL_free(peer->ssl);
		php_openssl_conn_free(peer->conn);
		efree(peer);
		return NULL;
	}
	if (port->link_mtu) {
		DTLS_set_link_mtu(peer->ssl, (long) port->link_mtu);
		SSL_set_options(peer->ssl, SSL_OP_NO_QUERY_MTU);
		peer->conn->link_mtu = port->link_mtu;
	}

	peer->key = zend_string_copy(key);
	zend_hash_add_new_ptr(&port->peers, key, peer);
	peer->next = port->pending;
	port->pending = peer;
	port->pending_count++;
	return peer;
}

/* Expired retransmit timers and stale handshakes of the pending peers; true when a timer fired */
static bool php_openssl_port_tick(php_openssl_port *port)
{
	bool progress = false;
	zend_hrtime_t now = zend_hrtime();
	zend_hrtime_t max_age = (zend_hrtime_t) port->pending_timeout * ZEND_NANO_IN_SEC;
	php_openssl_dtls_peer *peer = port->pending;

	while (peer != NULL) {
		php_openssl_dtls_peer *next = peer->next;
		if (now - peer->created > max_age) {
			php_openssl_dtls_peer_free(port, peer);
		} else {
			php_openssl_deadline timer;
			php_openssl_conn_timer_deadline(peer->conn, &timer);
			if (!timer.infinite && php_openssl_deadline_expired(&timer)
					&& php_openssl_conn_handle_timeout(peer->conn) > 0) {
				progress = true;
			}
			php_openssl_dtls_peer_flush(peer);
		}
		peer = next;
	}
	return progress;
}

/* The earliest retransmit timer of the pending peers */
static void php_openssl_port_timer_deadline(php_openssl_port *port, php_openssl_deadline *dl)
{
	php_openssl_deadline_init_infinite(dl);
	for (php_openssl_dtls_peer *peer = port->pending; peer != NULL; peer = peer->next) {
		php_openssl_deadline timer;
		php_openssl_conn_timer_deadline(peer->conn, &timer);
		php_openssl_deadline_cap(dl, &timer);
	}
	if (port->pending != NULL) {
		/* Stale handshakes are reaped at the latest when the oldest expires */
		php_openssl_dtls_peer *oldest = port->pending;
		while (oldest->next) {
			oldest = oldest->next;
		}
		php_openssl_deadline reap;
		reap.infinite = false;
		reap.at = oldest->created + (zend_hrtime_t) port->pending_timeout * ZEND_NANO_IN_SEC;
		php_openssl_deadline_cap(dl, &reap);
	}
}

/* Receives one datagram and gives it to its peer, a new one when unknown; 1 when it did,
 * -1 with errno EAGAIN when the socket has none */
static int php_openssl_port_recv(php_openssl_port *port)
{
	char buf[PHP_OPENSSL_MAX_DGRAM];
	php_sockaddr_storage addr;
	socklen_t addrlen;

	ssize_t n = php_openssl_xport_recv_dgram(port->xport, buf, sizeof(buf), &addr, &addrlen);
	if (n < 0) {
		return -1;
	}
	if (addrlen == 0) {
		/* No peer to route by: the transport is connected to one */
		memset(&addr, 0, sizeof(addr));
		addrlen = sizeof(addr.ss_family);
	}

	zend_string *key = php_openssl_dtls_peer_key((const struct sockaddr *) &addr, addrlen);
	php_openssl_dtls_peer *peer = zend_hash_find_ptr(&port->peers, key);
	if (peer == NULL) {
		if (port->closed || (zend_long) port->pending_count >= port->max_pending) {
			zend_string_release(key);
			return 1;
		}
		peer = php_openssl_dtls_peer_new(port, (const struct sockaddr *) &addr, addrlen, key);
		if (peer == NULL) {
			zend_string_release(key);
			return 1;
		}
	}
	zend_string_release(key);

	if (peer->conn->rxq.count >= PHP_OPENSSL_DTLS_RX_LIMIT) {
		return 1;
	}
	php_openssl_conn_push_dgram(peer->conn, buf, (size_t) n, (const struct sockaddr *) &addr, addrlen);
	if (peer->state == PHP_OPENSSL_DTLS_PEER_PENDING) {
		php_openssl_dtls_peer_drive(port, peer);
	}
	return 1;
}

/* Receives for the port, waiting up to the deadline, bounded by the retransmit timers of conn
 * (when given) and of the pending peers, which it services when they fire. Returns like
 * php_openssl_conn_fill(). */
static int php_openssl_port_pump(php_openssl_port *port, php_openssl_conn *conn, php_openssl_deadline *dl)
{
	for (;;) {
		int n = php_openssl_port_recv(port);
		if (n > 0) {
			return 1;
		}
		if (errno != EAGAIN) {
			return -1;
		}
		if (dl == NULL) {
			errno = EAGAIN;
			return -1;
		}

		php_openssl_deadline wait = *dl;
		php_openssl_deadline timer;
		if (conn != NULL) {
			php_openssl_conn_timer_deadline(conn, &timer);
			php_openssl_deadline_cap(&wait, &timer);
		}
		php_openssl_port_timer_deadline(port, &timer);
		php_openssl_deadline_cap(&wait, &timer);

		int w = php_openssl_xport_wait(port->xport, PHP_POLLREADABLE, &wait);
		if (w < 0) {
			errno = php_socket_errno();
			return -1;
		}
		if (w == 0) {
			bool progress = php_openssl_port_tick(port);
			if (conn != NULL) {
				php_openssl_conn_timer_deadline(conn, &timer);
				if (!timer.infinite && php_openssl_deadline_expired(&timer)
						&& php_openssl_conn_handle_timeout(conn) > 0) {
					progress = true;
				}
			}
			if (progress) {
				return 1;
			}
			if (php_openssl_deadline_expired(dl)) {
				errno = ETIMEDOUT;
				return -1;
			}
		}
	}
}

static int php_openssl_port_fill(php_openssl_conn *conn, php_openssl_deadline *dl)
{
	php_openssl_dtls_peer *peer = php_openssl_dtls_peer_of(conn);
	return php_openssl_port_pump(peer->port, conn, dl);
}

/* The stream API */

int php_openssl_dtls_listen(php_stream *stream, php_openssl_netstream_data_t *sslsock)
{
	zval *val;

	if (sslsock->port != NULL || sslsock->s.socket == SOCK_ERR) {
		return -1;
	}

	/* The server's method */
	sslsock->is_client = 0;
	sslsock->method &= ~STREAM_CRYPTO_IS_CLIENT;
	if (!GET_VER_OPT("crypto_method")) {
		/* Without a listener API of the library a DTLS 1.3 server cannot validate the peer
		 * address before its first flight, so it is opt-in */
		sslsock->method &= ~STREAM_CRYPTO_METHOD_DTLSv1_3;
	}
	if (sslsock->ctx == NULL && php_openssl_create_server_ctx(stream, sslsock, sslsock->method) == FAILURE) {
		return -1;
	}

	php_openssl_port *port = ecalloc(1, sizeof(*port));
	port->refcount = 1;
	port->stream = stream;
	port->listener = sslsock;
	port->fd = sslsock->s.socket;
	port->max_pending = PHP_OPENSSL_DTLS_MAX_PENDING;
	port->pending_timeout = PHP_OPENSSL_DTLS_PENDING_TIMEOUT;
	GET_VER_OPT_LONG("dtls_max_pending", port->max_pending);
	GET_VER_OPT_LONG("dtls_pending_timeout", port->pending_timeout);
	if (port->max_pending < 1) {
		port->max_pending = 1;
	}
	if (port->pending_timeout < 1) {
		port->pending_timeout = 1;
	}
	if (GET_VER_OPT("dtls_link_mtu")) {
		zend_long mtu = zval_get_long(val);
		if (mtu > 0) {
			port->link_mtu = (unsigned) mtu;
		}
	}
	if (RAND_bytes(port->cookie_secret, sizeof(port->cookie_secret)) != 1) {
		php_stream_warn(stream, CreateFailed, "DTLS cookie secret generation failure");
		efree(port);
		return -1;
	}
	zend_hash_init(&port->peers, 8, NULL, NULL, 0);

	SSL_CTX_up_ref(sslsock->ctx);
	port->ctx = sslsock->ctx;
	SSL_CTX_set_cookie_generate_cb(port->ctx, php_openssl_dtls_cookie_generate);
	SSL_CTX_set_cookie_verify_cb(port->ctx, php_openssl_dtls_cookie_verify);
	SSL_CTX_set_stateless_cookie_generate_cb(port->ctx, php_openssl_dtls_stateless_cookie_generate);
	SSL_CTX_set_stateless_cookie_verify_cb(port->ctx, php_openssl_dtls_stateless_cookie_verify);

	php_set_sock_blocking(port->fd, 0);
	php_openssl_dgram_socket_setup(port->fd);
	port->xport = php_openssl_xport_new_fd(port->fd, true, false);

	sslsock->port = port;
	return 0;
}

int php_openssl_dtls_accept(php_stream *stream, php_openssl_netstream_data_t *sslsock,
		php_stream_xport_param *xparam STREAMS_DC)
{
	php_openssl_port *port = sslsock->port;
	php_openssl_deadline dl;

	xparam->outputs.client = NULL;
	if (port == NULL) {
		if (xparam->want_errortext) {
			xparam->outputs.error_text = ZSTR_INIT_LITERAL("The dtls:// stream is not a server", 0);
		}
		return -1;
	}

	/* No timeout waits for good, a zero one does not wait at all */
	if (xparam->inputs.timeout != NULL && xparam->inputs.timeout->tv_sec == 0
			&& xparam->inputs.timeout->tv_usec == 0) {
		php_openssl_deadline_init_nonblock(&dl);
	} else {
		php_openssl_deadline_init(&dl, xparam->inputs.timeout);
	}
	while (port->ready == NULL) {
		if (php_openssl_port_pump(port, NULL, &dl) < 0) {
			if (errno == ETIMEDOUT) {
				xparam->outputs.error_code = ETIMEDOUT;
				if (xparam->want_errortext) {
					xparam->outputs.error_text = ZSTR_INIT_LITERAL("Accept timed out", 0);
				}
			} else {
				xparam->outputs.error_code = errno;
				if (xparam->want_errortext) {
					xparam->outputs.error_text = strpprintf(0, "%s", strerror(errno));
				}
			}
			return -1;
		}
	}

	/* The oldest completed handshake becomes a stream */
	php_openssl_dtls_peer *peer = port->ready;
	port->ready = peer->next;
	if (port->ready == NULL) {
		port->ready_tail = NULL;
	}
	peer->next = NULL;
	peer->state = PHP_OPENSSL_DTLS_PEER_ACCEPTED;

	php_openssl_netstream_data_t *clisockdata = emalloc(sizeof(*clisockdata));
	memset(clisockdata, 0, sizeof(*clisockdata));
	memcpy(&clisockdata->s, &sslsock->s, sizeof(clisockdata->s));
	clisockdata->s.is_blocked = true;
	clisockdata->connect_timeout = sslsock->connect_timeout;
	clisockdata->method = sslsock->method;
	clisockdata->is_client = 0;
	clisockdata->state_set = 1;
	clisockdata->ssl_handle = peer->ssl;
	clisockdata->conn = peer->conn;
	clisockdata->port = port;
	port->refcount++;
	php_openssl_netstream_share_ctx(clisockdata, sslsock);
	peer->sslsock = clisockdata;

	php_stream *clistream = php_stream_alloc_rel(&php_openssl_dgram_socket_ops, clisockdata, NULL, "r+");
	clistream->ctx = stream->ctx;
	if (stream->ctx) {
		GC_ADDREF(stream->ctx);
	}
	peer->conn->stream = clistream;
	SSL_set_ex_data(peer->ssl, php_openssl_get_ssl_stream_data_index(), clistream);

	if (php_openssl_handshake_complete(clistream, clisockdata) < 0) {
		php_stream_close(clistream);
		if (xparam->want_errortext) {
			xparam->outputs.error_text = ZSTR_INIT_LITERAL("Cannot enable crypto", 0);
		}
		return -1;
	}

	if (xparam->want_addr || xparam->want_textaddr) {
		php_network_populate_name_from_sockaddr((struct sockaddr *) &peer->conn->peer,
				peer->conn->peerlen,
				xparam->want_textaddr ? &xparam->outputs.textaddr : NULL,
				xparam->want_addr ? &xparam->outputs.addr : NULL,
				xparam->want_addr ? &xparam->outputs.addrlen : NULL);
	}
	xparam->outputs.client = clistream;
	return 0;
}

bool php_openssl_dtls_detach(php_stream *stream, php_openssl_netstream_data_t *sslsock)
{
	php_openssl_port *port = sslsock->port;

	if (port == NULL) {
		return false;
	}
	sslsock->port = NULL;

	if (port->listener == sslsock) {
		/* The listener is gone: no new peers, and the handshakes under way are dropped */
		port->closed = true;
		port->stream = NULL;
		port->listener = NULL;
		while (port->pending) {
			php_openssl_dtls_peer_free(port, port->pending);
		}
		while (port->ready) {
			php_openssl_dtls_peer_free(port, port->ready);
		}
	} else if (sslsock->conn != NULL) {
		php_openssl_dtls_peer *peer = php_openssl_dtls_peer_of(sslsock->conn);
		if (peer != NULL) {
			php_openssl_dtls_peer_free(port, peer);
		}
	}

	php_openssl_port_release(port);
	return true;
}

#endif /* HAVE_DTLS */
