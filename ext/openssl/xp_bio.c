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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "php.h"
#include "php_network.h"
#include "php_streams.h"
#include "xp_bio.h"

#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/err.h>

#ifndef PHP_WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#endif

/* Deadlines */

void php_openssl_deadline_init(php_openssl_deadline *dl, const struct timeval *tv)
{
	if (tv == NULL || tv->tv_sec < 0 || (tv->tv_sec == 0 && tv->tv_usec <= 0)) {
		php_openssl_deadline_init_infinite(dl);
		return;
	}
	dl->infinite = false;
	dl->at = zend_hrtime() + (zend_hrtime_t) tv->tv_sec * ZEND_NANO_IN_SEC
			+ (zend_hrtime_t) tv->tv_usec * 1000;
}

void php_openssl_deadline_init_infinite(php_openssl_deadline *dl)
{
	dl->infinite = true;
	dl->at = 0;
}

void php_openssl_deadline_init_nonblock(php_openssl_deadline *dl)
{
	dl->infinite = false;
	dl->at = 0;
}

bool php_openssl_deadline_expired(const php_openssl_deadline *dl)
{
	return !dl->infinite && (dl->at == 0 || zend_hrtime() >= dl->at);
}

int php_openssl_deadline_remaining_ms(const php_openssl_deadline *dl)
{
	if (dl->infinite) {
		return -1;
	}
	if (dl->at == 0) {
		return 0;
	}
	zend_hrtime_t now = zend_hrtime();
	if (now >= dl->at) {
		return 0;
	}
	zend_hrtime_t left = (dl->at - now + 999999) / 1000000;
	return left > INT_MAX ? INT_MAX : (int) left;
}

struct timeval *php_openssl_deadline_remaining_tv(const php_openssl_deadline *dl, struct timeval *tv)
{
	if (dl->infinite) {
		return NULL;
	}
	zend_hrtime_t now = zend_hrtime();
	zend_hrtime_t left = (dl->at == 0 || now >= dl->at) ? 0 : dl->at - now;
	tv->tv_sec = (time_t) (left / ZEND_NANO_IN_SEC);
	tv->tv_usec = (long) ((left % ZEND_NANO_IN_SEC) / 1000);
	return tv;
}

void php_openssl_deadline_cap(php_openssl_deadline *dl, const php_openssl_deadline *other)
{
	if (other->infinite) {
		return;
	}
	if (dl->infinite || other->at < dl->at) {
		*dl = *other;
	}
}

/* Queues */

static void php_openssl_bytebuf_free(php_openssl_bytebuf *buf, bool persistent)
{
	if (buf->data) {
		pefree(buf->data, persistent);
	}
	memset(buf, 0, sizeof(*buf));
}

static void php_openssl_bytebuf_compact(php_openssl_bytebuf *buf)
{
	if (buf->pos == buf->len) {
		buf->pos = buf->len = 0;
	} else if (buf->pos > 0) {
		memmove(buf->data, buf->data + buf->pos, buf->len - buf->pos);
		buf->len -= buf->pos;
		buf->pos = 0;
	}
}

/* Room for n more bytes at the end, compacting first */
static char *php_openssl_bytebuf_reserve(php_openssl_bytebuf *buf, size_t n, bool persistent)
{
	php_openssl_bytebuf_compact(buf);
	if (buf->len + n > buf->cap) {
		size_t cap = buf->cap ? buf->cap : 4096;
		while (cap < buf->len + n) {
			cap *= 2;
		}
		buf->data = perealloc(buf->data, cap, persistent);
		buf->cap = cap;
	}
	return buf->data + buf->len;
}

static void php_openssl_bytebuf_append(php_openssl_bytebuf *buf, const char *data, size_t n, bool persistent)
{
	memcpy(php_openssl_bytebuf_reserve(buf, n, persistent), data, n);
	buf->len += n;
}

static size_t php_openssl_bytebuf_avail(const php_openssl_bytebuf *buf)
{
	return buf->len - buf->pos;
}

static php_openssl_dgram *php_openssl_dgram_new(const char *data, size_t len, const struct sockaddr *peer,
		socklen_t peerlen, bool persistent)
{
	php_openssl_dgram *dg = pemalloc(sizeof(*dg) + len, persistent);
	dg->next = NULL;
	dg->len = len;
	dg->peerlen = 0;
	if (peer != NULL && peerlen > 0 && peerlen <= sizeof(dg->peer)) {
		memcpy(&dg->peer, peer, peerlen);
		dg->peerlen = peerlen;
	}
	if (len) {
		memcpy(dg->data, data, len);
	}
	return dg;
}

static void php_openssl_dgram_queue_push(php_openssl_dgram_queue *q, php_openssl_dgram *dg)
{
	if (q->tail) {
		q->tail->next = dg;
	} else {
		q->head = dg;
	}
	q->tail = dg;
	q->count++;
}

static php_openssl_dgram *php_openssl_dgram_queue_pop(php_openssl_dgram_queue *q)
{
	php_openssl_dgram *dg = q->head;
	if (dg) {
		q->head = dg->next;
		if (q->head == NULL) {
			q->tail = NULL;
		}
		q->count--;
	}
	return dg;
}

static void php_openssl_dgram_queue_free(php_openssl_dgram_queue *q, bool persistent)
{
	php_openssl_dgram *dg;
	while ((dg = php_openssl_dgram_queue_pop(q)) != NULL) {
		pefree(dg, persistent);
	}
}

/* Transports */

php_openssl_xport *php_openssl_xport_new_fd(php_socket_t fd, bool dgram, bool persistent)
{
	php_openssl_xport *xport = pecalloc(1, sizeof(*xport), persistent);
	xport->refcount = 1;
	xport->persistent = persistent;
	xport->dgram = dgram;
	xport->fd = fd;
	return xport;
}

php_openssl_xport *php_openssl_xport_new_stream(php_stream *inner, bool dgram)
{
	php_openssl_xport *xport = ecalloc(1, sizeof(*xport));
	xport->refcount = 1;
	xport->dgram = dgram;
	xport->fd = SOCK_ERR;
	xport->inner = inner;
	if (inner->res) {
		GC_ADDREF(inner->res);
	}
	if (dgram) {
		/* One read is one datagram */
		inner->flags |= PHP_STREAM_FLAG_NO_BUFFER;
	}
	return xport;
}

void php_openssl_xport_addref(php_openssl_xport *xport)
{
	xport->refcount++;
}

void php_openssl_xport_release(php_openssl_xport *xport)
{
	if (xport == NULL || --xport->refcount > 0) {
		return;
	}
	if (xport->inner) {
		if (xport->inner->res) {
			zend_list_delete(xport->inner->res);
		}
		xport->inner = NULL;
	}
	pefree(xport, xport->persistent);
}

int php_openssl_xport_wait(php_openssl_xport *xport, short events, php_openssl_deadline *dl)
{
	struct timeval tv;

	if (xport->fd == SOCK_ERR) {
		errno = EAGAIN;
		return -1;
	}
	for (;;) {
		struct timeval *ptv = php_openssl_deadline_remaining_tv(dl, &tv);
		int n = php_pollfd_for(xport->fd, events, ptv);
		if (n >= 0) {
			return n > 0 ? 1 : 0;
		}
		if (php_socket_errno() != EINTR) {
			return -1;
		}
	}
}

/* The last socket error as errno, EAGAIN for every would-block code */
static int php_openssl_xport_errno(void)
{
	int err = php_socket_errno();
	if (PHP_IS_TRANSIENT_ERROR(err)) {
		return EAGAIN;
	}
#ifdef PHP_WIN32
	if (err == WSAEMSGSIZE) {
		return EMSGSIZE;
	}
#endif
	return err;
}

/* Stream transport send: sends what it can, returns bytes sent, 0 when it would block, -1 on error */
static ssize_t php_openssl_xport_send(php_openssl_xport *xport, const char *data, size_t len)
{
	if (xport->inner) {
		ssize_t n = php_stream_write(xport->inner, data, len);
		if (n < 0) {
			errno = EIO;
			return -1;
		}
		return n;
	}

	ssize_t n;
#ifdef PHP_WIN32
	n = send(xport->fd, data, len > INT_MAX ? INT_MAX : (int) len, 0);
#else
	n = send(xport->fd, data, len, 0);
#endif
	if (n < 0) {
		errno = php_openssl_xport_errno();
		return errno == EAGAIN ? 0 : -1;
	}
	return n;
}

/* Datagram transport send of one datagram; 1 sent, 0 would block, -1 error (EMSGSIZE included) */
static int php_openssl_xport_send_dgram(php_openssl_xport *xport, const php_openssl_dgram *dg)
{
	if (xport->inner) {
		ssize_t n = php_stream_write(xport->inner, dg->data, dg->len);
		if (n < 0) {
			errno = EIO;
			return -1;
		}
		if (n == 0) {
			return 0;
		}
		return 1;
	}

	ssize_t n;
#ifdef PHP_WIN32
	int len = dg->len > INT_MAX ? INT_MAX : (int) dg->len;
#else
	size_t len = dg->len;
#endif
	if (dg->peerlen > 0) {
		n = sendto(xport->fd, dg->data, len, 0, (const struct sockaddr *) &dg->peer, dg->peerlen);
	} else {
		n = send(xport->fd, dg->data, len, 0);
	}
	if (n < 0) {
		errno = php_openssl_xport_errno();
		return errno == EAGAIN ? 0 : -1;
	}
	return 1;
}

/* Stream transport receive; returns bytes, 0 on EOF, -1 with errno (EAGAIN when it would block) */
static ssize_t php_openssl_xport_recv(php_openssl_xport *xport, char *buf, size_t len)
{
	if (xport->inner) {
		ssize_t n = php_stream_read(xport->inner, buf, len);
		if (n < 0) {
			errno = EIO;
			return -1;
		}
		if (n == 0) {
			if (xport->inner->eof) {
				return 0;
			}
			errno = EAGAIN;
			return -1;
		}
		return n;
	}

	ssize_t n;
#ifdef PHP_WIN32
	n = recv(xport->fd, buf, len > INT_MAX ? INT_MAX : (int) len, 0);
#else
	n = recv(xport->fd, buf, len, 0);
#endif
	if (n < 0) {
		errno = php_openssl_xport_errno();
	}
	return n;
}

ssize_t php_openssl_xport_recv_dgram(php_openssl_xport *xport, char *buf, size_t len,
		php_sockaddr_storage *peer, socklen_t *peerlen)
{
	*peerlen = 0;
	if (xport->inner) {
		ssize_t n = php_stream_read(xport->inner, buf, len);
		if (n < 0) {
			errno = EIO;
			return -1;
		}
		if (n == 0) {
			errno = EAGAIN;
			return -1;
		}
		return n;
	}

	socklen_t sl = sizeof(*peer);
	ssize_t n;
#ifdef PHP_WIN32
	n = recvfrom(xport->fd, buf, len > INT_MAX ? INT_MAX : (int) len, 0, (struct sockaddr *) peer, &sl);
#else
	n = recvfrom(xport->fd, buf, len, 0, (struct sockaddr *) peer, &sl);
#endif
	if (n < 0) {
		errno = php_openssl_xport_errno();
#ifdef PHP_WIN32
		/* A datagram larger than the buffer is truncated, as POSIX does */
		if (errno == EMSGSIZE) {
			*peerlen = sl;
			return len;
		}
#endif
		return -1;
	}
	*peerlen = sl;
	return n;
}

/* The BIO */

static BIO_METHOD *php_openssl_bio_method = NULL;

static int php_openssl_bio_read(BIO *bio, char *buf, int len)
{
	php_openssl_conn *conn = BIO_get_data(bio);

	BIO_clear_retry_flags(bio);
	if (conn == NULL || buf == NULL || len <= 0) {
		return -1;
	}

	if (conn->dgram) {
		php_openssl_dgram *dg = php_openssl_dgram_queue_pop(&conn->rxq);
		if (dg != NULL) {
			size_t n = dg->len > (size_t) len ? (size_t) len : dg->len;
			memcpy(buf, dg->data, n);
			pefree(dg, conn->persistent);
			return (int) n;
		}
	} else {
		size_t avail = php_openssl_bytebuf_avail(&conn->rx);
		if (avail > 0) {
			size_t n = avail > (size_t) len ? (size_t) len : avail;
			memcpy(buf, conn->rx.data + conn->rx.pos, n);
			conn->rx.pos += n;
			return (int) n;
		}
	}

	if (conn->rx_eof) {
		return 0;
	}
	conn->rx_want = (size_t) len;
	BIO_set_retry_read(bio);
	return -1;
}

static int php_openssl_bio_write(BIO *bio, const char *buf, int len)
{
	php_openssl_conn *conn = BIO_get_data(bio);

	BIO_clear_retry_flags(bio);
	if (conn == NULL || buf == NULL || len < 0) {
		return -1;
	}

	if (conn->dgram) {
		if (conn->txq.count >= PHP_OPENSSL_TX_DGRAM_LIMIT) {
			conn->tx_full = true;
			BIO_set_retry_write(bio);
			return -1;
		}
		php_openssl_dgram_queue_push(&conn->txq, php_openssl_dgram_new(buf, (size_t) len,
				(const struct sockaddr *) &conn->peer, conn->peerlen, conn->persistent));
	} else {
		if (php_openssl_bytebuf_avail(&conn->tx) >= PHP_OPENSSL_TX_LIMIT) {
			conn->tx_full = true;
			BIO_set_retry_write(bio);
			return -1;
		}
		php_openssl_bytebuf_append(&conn->tx, buf, (size_t) len, conn->persistent);
	}
	return len;
}

static unsigned php_openssl_bio_mtu_overhead(const php_openssl_conn *conn)
{
#ifdef HAVE_IPV6
	if (conn->peerlen > 0 && conn->peer.ss_family == AF_INET6) {
		return 48;
	}
	if (conn->peerlen == 0 && conn->xport && conn->xport->fd != SOCK_ERR) {
		php_sockaddr_storage sa;
		socklen_t sl = sizeof(sa);
		if (getsockname(conn->xport->fd, (struct sockaddr *) &sa, &sl) == 0 && sa.ss_family == AF_INET6) {
			return 48;
		}
	}
#endif
	return 28;
}

/* The path MTU of a connected socket when the system knows it, else the Ethernet default */
static long php_openssl_bio_query_mtu(php_openssl_conn *conn)
{
	unsigned overhead = php_openssl_bio_mtu_overhead(conn);

	if (conn->link_mtu) {
		return conn->link_mtu > overhead ? (long) (conn->link_mtu - overhead) : 0;
	}
#if defined(__linux__) && defined(IP_MTU)
	if (conn->peerlen == 0 && conn->xport && conn->xport->fd != SOCK_ERR) {
		int mtu = 0;
		socklen_t len = sizeof(mtu);
		int level = overhead == 48 ? IPPROTO_IPV6 : IPPROTO_IP;
#ifdef IPV6_MTU
		int name = overhead == 48 ? IPV6_MTU : IP_MTU;
#else
		int name = IP_MTU;
#endif
		if (getsockopt(conn->xport->fd, level, name, &mtu, &len) == 0 && mtu > (int) overhead) {
			return mtu - (long) overhead;
		}
	}
#endif
	return 1500 - (long) overhead;
}

static long php_openssl_bio_ctrl(BIO *bio, int cmd, long num, void *ptr)
{
	php_openssl_conn *conn = BIO_get_data(bio);

	if (conn == NULL) {
		return 0;
	}
	switch (cmd) {
		case BIO_CTRL_FLUSH:
		case BIO_CTRL_DUP:
			return 1;
		case BIO_CTRL_EOF:
			return conn->rx_eof && php_openssl_conn_rx_empty(conn);
		case BIO_CTRL_PENDING:
			if (conn->dgram) {
				return conn->rxq.head ? (long) conn->rxq.head->len : 0;
			}
			return (long) php_openssl_bytebuf_avail(&conn->rx);
		case BIO_CTRL_WPENDING:
			if (conn->dgram) {
				return (long) conn->txq.count;
			}
			return (long) php_openssl_bytebuf_avail(&conn->tx);
#ifdef BIO_CTRL_DGRAM_QUERY_MTU
		case BIO_CTRL_DGRAM_QUERY_MTU:
			return conn->dgram ? php_openssl_bio_query_mtu(conn) : 0;
		case BIO_CTRL_DGRAM_GET_FALLBACK_MTU:
			return php_openssl_bio_mtu_overhead(conn) == 48 ? 1280 - 48 : 576 - 28;
		case BIO_CTRL_DGRAM_GET_MTU_OVERHEAD:
			return (long) php_openssl_bio_mtu_overhead(conn);
		case BIO_CTRL_DGRAM_SET_MTU:
			conn->link_mtu = (unsigned) num + php_openssl_bio_mtu_overhead(conn);
			return num;
		case BIO_CTRL_DGRAM_GET_MTU:
			return conn->link_mtu ? (long) (conn->link_mtu - php_openssl_bio_mtu_overhead(conn)) : 0;
		case BIO_CTRL_DGRAM_MTU_EXCEEDED: {
			long ret = conn->mtu_exceeded;
			conn->mtu_exceeded = false;
			return ret;
		}
		case BIO_CTRL_DGRAM_SET_NEXT_TIMEOUT:
		case BIO_CTRL_DGRAM_SET_CONNECTED:
			return 1;
		case BIO_CTRL_DGRAM_GET_PEER:
			if (ptr != NULL && conn->peerlen > 0) {
				memcpy(ptr, &conn->peer, conn->peerlen);
				return (long) conn->peerlen;
			}
			return 0;
		case BIO_CTRL_DGRAM_SET_PEER:
			if (ptr != NULL) {
				const struct sockaddr *sa = ptr;
				socklen_t sl = 0;
				if (sa->sa_family == AF_INET) {
					sl = sizeof(struct sockaddr_in);
				}
#ifdef HAVE_IPV6
				else if (sa->sa_family == AF_INET6) {
					sl = sizeof(struct sockaddr_in6);
				}
#endif
				php_openssl_conn_set_peer(conn, sa, sl);
				return 1;
			}
			return 0;
#endif
		default:
			return 0;
	}
}

static int php_openssl_bio_create(BIO *bio)
{
	BIO_set_init(bio, 0);
	return 1;
}

static int php_openssl_bio_destroy(BIO *bio)
{
	if (bio == NULL) {
		return 0;
	}
	BIO_set_data(bio, NULL);
	BIO_set_init(bio, 0);
	return 1;
}

void php_openssl_bio_minit(void)
{
	BIO_METHOD *m = BIO_meth_new(BIO_get_new_index() | BIO_TYPE_SOURCE_SINK, "PHP stream");
	if (m == NULL) {
		return;
	}
	BIO_meth_set_write(m, php_openssl_bio_write);
	BIO_meth_set_read(m, php_openssl_bio_read);
	BIO_meth_set_ctrl(m, php_openssl_bio_ctrl);
	BIO_meth_set_create(m, php_openssl_bio_create);
	BIO_meth_set_destroy(m, php_openssl_bio_destroy);
	php_openssl_bio_method = m;
}

void php_openssl_bio_mshutdown(void)
{
	if (php_openssl_bio_method != NULL) {
		BIO_meth_free(php_openssl_bio_method);
		php_openssl_bio_method = NULL;
	}
}

/* Connections */

php_openssl_conn *php_openssl_conn_new(php_openssl_xport *xport, php_stream *stream, bool persistent)
{
	php_openssl_conn *conn = pecalloc(1, sizeof(*conn), persistent);
	php_openssl_xport_addref(xport);
	conn->xport = xport;
	conn->stream = stream;
	conn->dgram = xport->dgram;
	conn->persistent = persistent;
	conn->fill = php_openssl_conn_fill_own;
	return conn;
}

void php_openssl_conn_free(php_openssl_conn *conn)
{
	if (conn == NULL) {
		return;
	}
	php_openssl_bytebuf_free(&conn->rx, conn->persistent);
	php_openssl_bytebuf_free(&conn->tx, conn->persistent);
	php_openssl_dgram_queue_free(&conn->rxq, conn->persistent);
	php_openssl_dgram_queue_free(&conn->txq, conn->persistent);
	php_openssl_xport_release(conn->xport);
	pefree(conn, conn->persistent);
}

zend_result php_openssl_conn_set_ssl(php_openssl_conn *conn, SSL *ssl)
{
	BIO *bio = php_openssl_bio_method ? BIO_new(php_openssl_bio_method) : NULL;
	if (bio == NULL) {
		return FAILURE;
	}
	BIO_set_data(bio, conn);
	BIO_set_init(bio, 1);
	/* One BIO for both directions: SSL_set_bio() takes one reference, the second direction
	 * shares it */
	SSL_set_bio(ssl, bio, bio);
	conn->ssl = ssl;
	return SUCCESS;
}

void php_openssl_conn_set_peer(php_openssl_conn *conn, const struct sockaddr *peer, socklen_t peerlen)
{
	if (peer == NULL || peerlen == 0 || peerlen > sizeof(conn->peer)) {
		conn->peerlen = 0;
		return;
	}
	memcpy(&conn->peer, peer, peerlen);
	conn->peerlen = peerlen;
}

zend_result php_openssl_conn_push_dgram(php_openssl_conn *conn, const char *data, size_t len,
		const struct sockaddr *peer, socklen_t peerlen)
{
	if (!conn->dgram) {
		return FAILURE;
	}
	php_openssl_dgram_queue_push(&conn->rxq, php_openssl_dgram_new(data, len, peer, peerlen,
			conn->persistent));
	return SUCCESS;
}

bool php_openssl_conn_rx_empty(const php_openssl_conn *conn)
{
	return conn->dgram ? conn->rxq.count == 0 : php_openssl_bytebuf_avail(&conn->rx) == 0;
}

bool php_openssl_conn_tx_empty(const php_openssl_conn *conn)
{
	return conn->dgram ? conn->txq.count == 0 : php_openssl_bytebuf_avail(&conn->tx) == 0;
}

void php_openssl_conn_timer_deadline(php_openssl_conn *conn, php_openssl_deadline *dl)
{
	struct timeval tv;

	php_openssl_deadline_init_infinite(dl);
#ifndef OPENSSL_NO_DTLS
	if (conn->dgram && conn->ssl != NULL && DTLSv1_get_timeout(conn->ssl, &tv)) {
		if (tv.tv_sec == 0 && tv.tv_usec == 0) {
			php_openssl_deadline_init_nonblock(dl);
		} else {
			php_openssl_deadline_init(dl, &tv);
		}
	}
#endif
}

int php_openssl_conn_handle_timeout(php_openssl_conn *conn)
{
#ifndef OPENSSL_NO_DTLS
	if (conn->dgram && conn->ssl != NULL) {
		return DTLSv1_handle_timeout(conn->ssl) > 0;
	}
#endif
	return 0;
}

int php_openssl_conn_flush(php_openssl_conn *conn, php_openssl_deadline *dl)
{
	php_openssl_xport *xport = conn->xport;

	for (;;) {
		if (conn->dgram) {
			php_openssl_dgram *dg = conn->txq.head;
			if (dg == NULL) {
				conn->tx_full = false;
				return 1;
			}
			int n = php_openssl_xport_send_dgram(xport, dg);
			if (n > 0) {
				php_openssl_dgram_queue_pop(&conn->txq);
				pefree(dg, conn->persistent);
				continue;
			}
			if (n < 0) {
				if (errno == EMSGSIZE) {
					/* Too large for the path: OpenSSL retransmits a smaller one */
					conn->mtu_exceeded = true;
					php_openssl_dgram_queue_pop(&conn->txq);
					pefree(dg, conn->persistent);
					continue;
				}
				return -1;
			}
		} else {
			size_t avail = php_openssl_bytebuf_avail(&conn->tx);
			if (avail == 0) {
				conn->tx_full = false;
				php_openssl_bytebuf_compact(&conn->tx);
				return 1;
			}
			ssize_t n = php_openssl_xport_send(xport, conn->tx.data + conn->tx.pos, avail);
			if (n > 0) {
				conn->tx.pos += (size_t) n;
				continue;
			}
			if (n < 0) {
				return -1;
			}
		}

		/* The transport would block */
		if (dl == NULL || xport->inner != NULL) {
			errno = EAGAIN;
			return 0;
		}
		int w = php_openssl_xport_wait(xport, POLLOUT, dl);
		if (w < 0) {
			errno = php_openssl_xport_errno();
			return -1;
		}
		if (w == 0) {
			errno = ETIMEDOUT;
			return 0;
		}
	}
}

/* The receive of a stream transport into the connection's queue */
static int php_openssl_conn_recv_stream(php_openssl_conn *conn)
{
	size_t want = conn->rx_want ? conn->rx_want : 16384;
	if (want > 65536) {
		want = 65536;
	}
	char *dst = php_openssl_bytebuf_reserve(&conn->rx, want, conn->persistent);
	ssize_t n = php_openssl_xport_recv(conn->xport, dst, want);
	if (n > 0) {
		conn->rx.len += (size_t) n;
		conn->rx_want = 0;
		return 1;
	}
	if (n == 0) {
		conn->rx_eof = true;
		return 0;
	}
	return -1;
}

/* The receive of a datagram transport into the connection's queue */
static int php_openssl_conn_recv_dgram(php_openssl_conn *conn)
{
	char buf[PHP_OPENSSL_MAX_DGRAM];
	php_sockaddr_storage peer;
	socklen_t peerlen;

	ssize_t n = php_openssl_xport_recv_dgram(conn->xport, buf, sizeof(buf), &peer, &peerlen);
	if (n < 0) {
		return -1;
	}
	php_openssl_conn_push_dgram(conn, buf, (size_t) n, (const struct sockaddr *) &peer, peerlen);
	return 1;
}

int php_openssl_conn_fill_own(php_openssl_conn *conn, php_openssl_deadline *dl)
{
	php_openssl_xport *xport = conn->xport;

	for (;;) {
		int n = conn->dgram ? php_openssl_conn_recv_dgram(conn) : php_openssl_conn_recv_stream(conn);
		if (n >= 0) {
			return n;
		}
		if (errno != EAGAIN) {
			return -1;
		}
		if (dl == NULL || xport->inner != NULL) {
			errno = EAGAIN;
			return -1;
		}

		/* Wait for the socket, no longer than the retransmit timer of a DTLS connection */
		php_openssl_deadline wait = *dl;
		php_openssl_deadline timer;
		php_openssl_conn_timer_deadline(conn, &timer);
		php_openssl_deadline_cap(&wait, &timer);

		int w = php_openssl_xport_wait(xport, PHP_POLLREADABLE, &wait);
		if (w < 0) {
			errno = php_openssl_xport_errno();
			return -1;
		}
		if (w == 0) {
			if (!timer.infinite && php_openssl_deadline_expired(&timer)) {
				if (php_openssl_conn_handle_timeout(conn) > 0) {
					return 1;
				}
			}
			if (php_openssl_deadline_expired(dl)) {
				errno = ETIMEDOUT;
				return -1;
			}
		}
	}
}

int php_openssl_conn_fill(php_openssl_conn *conn, php_openssl_deadline *dl)
{
	return conn->fill(conn, dl);
}
