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
  | Authors: Jakub Zelenka <bukka@php.net>                               |
  +----------------------------------------------------------------------+
*/

/* The ciphertext transport of the TLS and DTLS streams.
 *
 * OpenSSL never does IO itself: its BIO serves records from a receive queue and appends to a
 * send queue, both owned by a connection. The stream moves ciphertext between the queues and
 * the transport around each SSL call, so no call into OpenSSL ever waits. The transport is a
 * socket, or any other stream (a user wrapper included). A DTLS connection queues datagrams
 * with their peer, so one socket can carry the connections of a server. */

#ifndef PHP_OPENSSL_XP_BIO_H
#define PHP_OPENSSL_XP_BIO_H

#include "php.h"
#include "php_network.h"
#include "zend_hrtime.h"

#include <openssl/ssl.h>
#include <openssl/bio.h>

/* A monotonic deadline; infinite when no timeout applies */
typedef struct _php_openssl_deadline {
	zend_hrtime_t at;
	bool infinite;
} php_openssl_deadline;

/* NULL or a non-positive timeval means no timeout */
void php_openssl_deadline_init(php_openssl_deadline *dl, const struct timeval *tv);
void php_openssl_deadline_init_infinite(php_openssl_deadline *dl);
/* A deadline already passed: one attempt, no wait */
void php_openssl_deadline_init_nonblock(php_openssl_deadline *dl);
bool php_openssl_deadline_expired(const php_openssl_deadline *dl);
/* -1 for infinite */
int php_openssl_deadline_remaining_ms(const php_openssl_deadline *dl);
/* Fills tv with the time left and returns it, NULL for infinite */
struct timeval *php_openssl_deadline_remaining_tv(const php_openssl_deadline *dl, struct timeval *tv);
/* The earlier of the two */
void php_openssl_deadline_cap(php_openssl_deadline *dl, const php_openssl_deadline *other);

/* Bytes of a stream transport */
typedef struct _php_openssl_bytebuf {
	char *data;
	size_t pos;
	size_t len;
	size_t cap;
} php_openssl_bytebuf;

/* One datagram with its peer; the peer is empty on a connected socket */
typedef struct _php_openssl_dgram {
	struct _php_openssl_dgram *next;
	size_t len;
	socklen_t peerlen;
	php_sockaddr_storage peer;
	char data[1];
} php_openssl_dgram;

typedef struct _php_openssl_dgram_queue {
	php_openssl_dgram *head;
	php_openssl_dgram *tail;
	size_t count;
} php_openssl_dgram_queue;

typedef struct _php_openssl_conn php_openssl_conn;

/* Where the ciphertext goes: a socket, or an inner stream. Shared by the connections of a DTLS
 * server through its port, hence counted. The owner closes the socket; the transport only keeps
 * the inner stream alive. */
typedef struct _php_openssl_xport {
	int refcount;
	bool persistent;
	bool dgram;
	php_socket_t fd;
	php_stream *inner;
} php_openssl_xport;

php_openssl_xport *php_openssl_xport_new_fd(php_socket_t fd, bool dgram, bool persistent);
php_openssl_xport *php_openssl_xport_new_stream(php_stream *inner, bool dgram);
void php_openssl_xport_addref(php_openssl_xport *xport);
void php_openssl_xport_release(php_openssl_xport *xport);
/* Waits for readiness of the socket; 1 ready, 0 timed out, -1 error. Not for an inner stream. */
int php_openssl_xport_wait(php_openssl_xport *xport, short events, php_openssl_deadline *dl);

/* Receives ciphertext for a connection. Installed by a DTLS port, whose socket carries every
 * connection of the server; the default receives for the connection itself. Returns like
 * php_openssl_conn_fill(). */
typedef int (*php_openssl_conn_fill_fn)(php_openssl_conn *conn, php_openssl_deadline *dl);

/* One SSL connection's ciphertext queues: the data of its BIO */
struct _php_openssl_conn {
	php_openssl_xport *xport;
	SSL *ssl;
	/* The stream OpenSSL callbacks reach through ex_data; NULL while a DTLS server connection
	 * has no stream yet */
	php_stream *stream;
	bool dgram;
	bool persistent;
	/* Stream transport queues */
	php_openssl_bytebuf rx;
	php_openssl_bytebuf tx;
	/* Datagram transport queues */
	php_openssl_dgram_queue rxq;
	php_openssl_dgram_queue txq;
	/* Bytes the last read of OpenSSL asked for, the size of the next receive */
	size_t rx_want;
	bool rx_eof;
	/* The queue is as full as a non-blocking transport lets it be: the BIO reports a retry */
	bool tx_full;
	/* The peer of a datagram connection; empty on a connected socket */
	php_sockaddr_storage peer;
	socklen_t peerlen;
	/* DTLS path MTU state the BIO answers */
	unsigned link_mtu;
	bool mtu_exceeded;
	php_openssl_conn_fill_fn fill;
	void *owner;
};

void php_openssl_bio_minit(void);
void php_openssl_bio_mshutdown(void);

php_openssl_conn *php_openssl_conn_new(php_openssl_xport *xport, php_stream *stream, bool persistent);
/* Frees the queues and the transport reference; the SSL is the caller's */
void php_openssl_conn_free(php_openssl_conn *conn);
/* Installs the connection as the BIO of ssl */
zend_result php_openssl_conn_set_ssl(php_openssl_conn *conn, SSL *ssl);
void php_openssl_conn_set_peer(php_openssl_conn *conn, const struct sockaddr *peer, socklen_t peerlen);
/* Queues a received datagram for the connection */
zend_result php_openssl_conn_push_dgram(php_openssl_conn *conn, const char *data, size_t len,
		const struct sockaddr *peer, socklen_t peerlen);
bool php_openssl_conn_rx_empty(const php_openssl_conn *conn);
bool php_openssl_conn_tx_empty(const php_openssl_conn *conn);
/* The retransmit timer of a DTLS connection as a deadline; infinite when none is armed */
void php_openssl_conn_timer_deadline(php_openssl_conn *conn, php_openssl_deadline *dl);
/* Services an expired DTLS retransmit timer; 1 when it did something */
int php_openssl_conn_handle_timeout(php_openssl_conn *conn);

/* Sends everything queued. A NULL deadline makes one attempt without waiting. Returns 1 when
 * the queue is empty, 0 when the transport would block, -1 on an error with errno set. */
int php_openssl_conn_flush(php_openssl_conn *conn, php_openssl_deadline *dl);
/* Receives more ciphertext, waiting up to the deadline, or not at all when it is NULL. The DTLS
 * retransmit timer bounds the wait and is serviced when it fires. Returns 1 when something was
 * received or a timer fired, 0 on EOF, -1 with errno: EAGAIN when the transport would block,
 * ETIMEDOUT when the deadline passed, otherwise the transport error. */
int php_openssl_conn_fill(php_openssl_conn *conn, php_openssl_deadline *dl);
/* The default fill: receives for this connection alone */
int php_openssl_conn_fill_own(php_openssl_conn *conn, php_openssl_deadline *dl);
/* Receives one datagram from a datagram transport into buf; -1 with errno EAGAIN when none */
ssize_t php_openssl_xport_recv_dgram(php_openssl_xport *xport, char *buf, size_t len,
		php_sockaddr_storage *peer, socklen_t *peerlen);

/* The largest datagram a DTLS peer may send */
#define PHP_OPENSSL_MAX_DGRAM 18432
/* Bytes a non-blocking stream may leave unsent before the BIO reports a retry */
#define PHP_OPENSSL_TX_LIMIT 65536
/* Datagrams a non-blocking connection may leave unsent */
#define PHP_OPENSSL_TX_DGRAM_LIMIT 64

#endif /* PHP_OPENSSL_XP_BIO_H */
