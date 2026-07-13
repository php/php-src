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
#include "php_openssl_backend.h"
#include "php_network.h"
#include "xp_common.h"
#ifdef PHP_WIN32
# include "win32/time.h"
#endif
#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/hmac.h>
#include <openssl/rand.h>
#include <openssl/crypto.h>

#ifndef OPENSSL_NO_DTLS

/* Index of the php_stream pointer stored on an SSL, so a callback can recover it. */
extern int php_openssl_get_ssl_stream_data_index(void);

/* Holds the session cache callbacks */
typedef struct _php_openssl_dtls_session_callbacks_t {
	int refcount;
	zend_fcall_info_cache new_cb;
	zend_fcall_info_cache get_cb;
	zend_fcall_info_cache remove_cb;
} php_openssl_dtls_session_callbacks_t;

/* The base socket data is embedded first so the generic socket option handlers
 * can be reused; the datagram BIO is owned by ssl_handle (freed with it). */
typedef struct _php_openssl_dtls_data_t {
	php_netstream_data_t s;
	SSL_CTX *ctx;
	SSL *ssl_handle;
	bool is_server;
	bool ssl_active;
	bool enable_on_connect;
	php_stream_xport_crypt_method_t method;
	struct timeval connect_timeout;
	char *url_name;
	php_openssl_dtls_session_callbacks_t *session_callbacks;
} php_openssl_dtls_data_t;

/* Index of the php_stream pointer stored on an SSL_CTX, for the remove callback
 * which is only passed the context. */
static int php_openssl_dtls_ctx_stream_index = -1;
static int php_openssl_dtls_get_ctx_stream_index(void)
{
	if (php_openssl_dtls_ctx_stream_index < 0) {
		php_openssl_dtls_ctx_stream_index =
				SSL_CTX_get_ex_new_index(0, "PHP dtls ctx stream index", NULL, NULL, NULL);
	}
	return php_openssl_dtls_ctx_stream_index;
}

static const php_stream_ops php_openssl_dtls_socket_ops;

/* Read an option from the "ssl" stream context into the local `val`. */
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

/* Datagram semantics: like udp:// (and tls://) the stream is buffered, so a read
 * yields the data of a single DTLS record and a write emits one record (atomic,
 * no partial writes). EOF is the peer's close_notify, which UDP need not deliver,
 * so it is best-effort.
 *
 * The socket is non-blocking, so on WANT_* we poll (up to the read timeout when
 * blocking) and retry. */
static ssize_t php_openssl_dtls_io(bool read, php_stream *stream, char *buf, size_t count)
{
	php_openssl_dtls_data_t *dtlssock = (php_openssl_dtls_data_t *)stream->abstract;

	/* Plain udp:// (no crypto): read/write the datagram socket directly. */
	if (!dtlssock->ssl_active) {
		return read
			? php_stream_socket_ops.read(stream, buf, count)
			: php_stream_socket_ops.write(stream, buf, count);
	}

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
		int events;

		ERR_clear_error();
		int n = read ? SSL_read(ssl, buf, (int)count) : SSL_write(ssl, buf, (int)count);
		if (n > 0) {
			php_stream_notify_progress_increment(PHP_STREAM_CONTEXT(stream), n, 0);
			return n;
		}
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

		/* Non-blocking, or a zero read timeout: don't wait, report would-block. */
		if (!dtlssock->s.is_blocked
				|| (dtlssock->s.timeout.tv_sec == 0 && dtlssock->s.timeout.tv_usec == 0)) {
			return 0;
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
		/* Shut down cleanly so a session captured for resumption stays usable;
		 * an SSL_free on an unfinished exchange marks the session non-resumable. */
		if (SSL_is_init_finished(dtlssock->ssl_handle)) {
			SSL_shutdown(dtlssock->ssl_handle);
		}
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

	/* Shared between the listener and its connections; free on the last one. */
	if (dtlssock->session_callbacks && --dtlssock->session_callbacks->refcount == 0) {
		if (ZEND_FCC_INITIALIZED(dtlssock->session_callbacks->new_cb)) {
			zend_fcc_dtor(&dtlssock->session_callbacks->new_cb);
		}
		if (ZEND_FCC_INITIALIZED(dtlssock->session_callbacks->get_cb)) {
			zend_fcc_dtor(&dtlssock->session_callbacks->get_cb);
		}
		if (ZEND_FCC_INITIALIZED(dtlssock->session_callbacks->remove_cb)) {
			zend_fcc_dtor(&dtlssock->session_callbacks->remove_cb);
		}
		pefree(dtlssock->session_callbacks, 0);
	}
	dtlssock->session_callbacks = NULL;

	if (dtlssock->url_name != NULL) {
		pefree(dtlssock->url_name, php_stream_is_persistent(stream));
	}

	pefree(dtlssock, php_stream_is_persistent(stream));
	stream->abstract = NULL;

	return 0;
}

/* Supply the passphrase for an encrypted local_pk from the "passphrase" option. */
static int php_openssl_dtls_passwd_callback(char *buf, int num, int verify, void *data)
{
	php_stream *stream = (php_stream *)data;
	zval *val = NULL;
	char *passphrase = NULL;

	GET_VER_OPT_STRING("passphrase", passphrase);

	if (passphrase != NULL && Z_STRLEN_P(val) < (size_t)num - 1) {
		memcpy(buf, Z_STRVAL_P(val), Z_STRLEN_P(val) + 1);
		return (int)Z_STRLEN_P(val);
	}
	return 0;
}

/* Apply the "ssl" context options to the SSL_CTX: peer verification, ciphers and
 * the local certificate. Set on the context so SSL_new() inherits them. */
static int php_openssl_dtls_apply_context(php_stream *stream, php_openssl_dtls_data_t *dtlssock)
{
	SSL_CTX *ctx = dtlssock->ctx;
	char *cafile = NULL, *capath = NULL, *cipherlist = NULL, *certfile = NULL;
	size_t certfile_len = 0;
	zval *val;

	/* DTLS 1.0 is deprecated; require DTLS 1.2 or higher. */
	SSL_CTX_set_min_proto_version(ctx, DTLS1_2_VERSION);

	/* Clients verify the server by default; a server does not request a client
	 * certificate unless verify_peer is set explicitly. A peer_fingerprint
	 * authenticates the peer by itself (checked after the handshake), so it
	 * overrides CA verification. */
	bool is_server = dtlssock->is_server;
	bool verify_peer = GET_VER_OPT("verify_peer") ? zend_is_true(val) : !is_server;
	bool has_fingerprint = GET_VER_OPT("peer_fingerprint");
	if (!verify_peer || has_fingerprint) {
		SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, NULL);
	} else {
		GET_VER_OPT_STRING("cafile", cafile);
		GET_VER_OPT_STRING("capath", capath);
		if (cafile != NULL || capath != NULL) {
			if (SSL_CTX_load_verify_locations(ctx, cafile, capath) != 1) {
				php_stream_warn(stream, CreateFailed, "Failed to load the CA verify locations");
				return -1;
			}
		} else if (SSL_CTX_set_default_verify_paths(ctx) != 1) {
			php_stream_warn(stream, CreateFailed, "Failed to set the default CA verify paths");
			return -1;
		}
		int verify_mode = SSL_VERIFY_PEER;
		if (is_server) {
			/* A server that verifies peers must require the client certificate. */
			verify_mode |= SSL_VERIFY_FAIL_IF_NO_PEER_CERT;
		}
		SSL_CTX_set_verify(ctx, verify_mode, NULL);
	}

	GET_VER_OPT_STRING("ciphers", cipherlist);
	if (cipherlist != NULL && SSL_CTX_set_cipher_list(ctx, cipherlist) != 1) {
		php_stream_warn(stream, CreateFailed, "Failed to set the cipher list");
		return -1;
	}

	/* Local certificate and private key as file paths. */
	GET_VER_OPT_STRINGL("local_cert", certfile, certfile_len);
	if (certfile != NULL) {
		char resolved[MAXPATHLEN];
		char *private_key = NULL;
		size_t private_key_len = 0;

		if (!php_openssl_check_path_ex(certfile, certfile_len, resolved, 0, false, false,
				"local_cert in ssl stream context", stream)) {
			return -1;
		}
		if (SSL_CTX_use_certificate_chain_file(ctx, resolved) != 1) {
			php_stream_warn(stream, WriteFailed, "Unable to set local cert chain file `%s'", certfile);
			return -1;
		}

		/* A passphrase for an encrypted private key. */
		if (GET_VER_OPT("passphrase")) {
			SSL_CTX_set_default_passwd_cb_userdata(ctx, stream);
			SSL_CTX_set_default_passwd_cb(ctx, php_openssl_dtls_passwd_callback);
		}

		/* local_pk defaults to the certificate file (combined PEM). */
		GET_VER_OPT_STRINGL("local_pk", private_key, private_key_len);
		if (private_key != NULL && !php_openssl_check_path_ex(private_key, private_key_len, resolved, 0,
				false, false, "local_pk in ssl stream context", stream)) {
			return -1;
		}
		if (SSL_CTX_use_PrivateKey_file(ctx, resolved, SSL_FILETYPE_PEM) != 1) {
			php_stream_warn(stream, WriteFailed, "Unable to set private key file `%s'", resolved);
			return -1;
		}
		if (SSL_CTX_check_private_key(ctx) != 1) {
			php_stream_warn(stream, PermissionDenied, "Private key does not match certificate!");
		}
	}

	return 0;
}

/* Keep the handshake within the path MTU: enable path-MTU discovery so the
 * kernel drops-and-signals oversized datagrams (OpenSSL then shrinks its DTLS
 * MTU and retransmits), and honour an explicit dtls_link_mtu context option. */
static void php_openssl_dtls_configure_mtu(php_stream *stream, SSL *ssl, php_socket_t fd, int family)
{
	zval *val;

#if defined(IP_MTU_DISCOVER) && defined(IP_PMTUDISC_DO)
	if (family == AF_INET) {
		int mode = IP_PMTUDISC_DO;
		setsockopt(fd, IPPROTO_IP, IP_MTU_DISCOVER, (char *)&mode, sizeof(mode));
	}
#endif
#if defined(IPV6_MTU_DISCOVER) && defined(IPV6_PMTUDISC_DO)
	if (family == AF_INET6) {
		int mode = IPV6_PMTUDISC_DO;
		setsockopt(fd, IPPROTO_IPV6, IPV6_MTU_DISCOVER, (char *)&mode, sizeof(mode));
	}
#endif

	if (GET_VER_OPT("dtls_link_mtu")) {
		zend_long mtu = zval_get_long(val);
		if (mtu > 0) {
			DTLS_set_link_mtu(ssl, mtu);
			SSL_set_options(ssl, SSL_OP_NO_QUERY_MTU);
		}
	}
}

/* Create the DTLS context, SSL object and datagram BIO. */
static int php_openssl_dtls_setup_crypto(php_stream *stream, php_openssl_dtls_data_t *dtlssock,
		const char *peer_host)
{
	BIO *bio;
	zval *val;

	/* The DTLS I/O loop emulates blocking with poll, so the fd stays non-blocking. */
	php_set_sock_blocking(dtlssock->s.socket, 0);

	dtlssock->ctx = SSL_CTX_new(DTLS_client_method());
	if (dtlssock->ctx == NULL) {
		php_stream_warn(stream, CreateFailed, "DTLS context creation failure");
		return -1;
	}

	if (php_openssl_dtls_apply_context(stream, dtlssock) != 0) {
		SSL_CTX_free(dtlssock->ctx);
		dtlssock->ctx = NULL;
		return -1;
	}

	dtlssock->ssl_handle = SSL_new(dtlssock->ctx);
	if (dtlssock->ssl_handle == NULL) {
		php_stream_warn(stream, CreateFailed, "DTLS handle creation failure");
		SSL_CTX_free(dtlssock->ctx);
		dtlssock->ctx = NULL;
		return -1;
	}

	/* Resume a previous session (abbreviated handshake) if session_data holds an
	 * Openssl\Session; SSL_set_session takes its own reference. The client cache
	 * must be enabled for the session to be offered in the ClientHello. */
	if (GET_VER_OPT("session_data") && php_openssl_is_session_ce(val)) {
		SSL_SESSION *session = php_openssl_session_from_zval(val);
		if (session != NULL) {
			SSL_CTX_set_session_cache_mode(dtlssock->ctx,
					SSL_SESS_CACHE_CLIENT | SSL_SESS_CACHE_NO_INTERNAL);
			SSL_set_session(dtlssock->ssl_handle, session);
		}
	}

	/* Hostname verification needs the SSL object; the verify mode is inherited
	 * from the context. */
	bool verify_peer = !(GET_VER_OPT("verify_peer") && !zend_is_true(val));
	bool verify_name = !(GET_VER_OPT("verify_peer_name") && !zend_is_true(val));
	if (verify_peer && verify_name) {
		const char *name = peer_host ? peer_host : dtlssock->url_name;
		GET_VER_OPT_STRING("peer_name", name);
		if (name != NULL) {
			/* An IP literal needs IP-address matching, not DNS-name matching. */
			X509_VERIFY_PARAM *param = SSL_get0_param(dtlssock->ssl_handle);
			if (X509_VERIFY_PARAM_set1_ip_asc(param, name) != 1) {
				X509_VERIFY_PARAM_set1_host(param, name, 0);
			}
		}
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
			php_openssl_dtls_configure_mtu(stream, dtlssock->ssl_handle, dtlssock->s.socket,
					peer.ss_family);
		}
	}

	SSL_set_bio(dtlssock->ssl_handle, bio, bio);
	SSL_set_connect_state(dtlssock->ssl_handle);

	return 0;
}

/* The socket is non-blocking, so poll between flights and resend on the
 * retransmission timeout. */
static int php_openssl_dtls_handshake(php_stream *stream, php_openssl_dtls_data_t *dtlssock,
		struct timeval *timeout, zend_string **error_text)
{
	SSL *ssl = dtlssock->ssl_handle;

	/* Always bound the handshake so a silent peer can't make OpenSSL's DTLS timer
	 * retransmit forever: use the connect timeout, else the default socket timeout. */
	struct timeval *tmo = timeout;
	struct timeval deadline;
	gettimeofday(&deadline, NULL);
	if (tmo != NULL && (tmo->tv_sec > 0 || tmo->tv_usec > 0)) {
		deadline.tv_sec += tmo->tv_sec;
		deadline.tv_usec += tmo->tv_usec;
		if (deadline.tv_usec >= 1000000) {
			deadline.tv_sec++;
			deadline.tv_usec -= 1000000;
		}
	} else {
		deadline.tv_sec += (time_t) FG(default_socket_timeout);
	}

	for (;;) {
		ERR_clear_error();
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
				if (error_text != NULL) {
					*error_text = strpprintf(0, "DTLS handshake failed: %s",
							buf[0] != '\0' ? buf : "unexpected error");
				}
				return -1;
			}
		}

		struct timeval tv;
		int wait_ms = DTLSv1_get_timeout(ssl, &tv)
				? (int)(tv.tv_sec * 1000 + tv.tv_usec / 1000)
				: -1;

		struct timeval now;
		gettimeofday(&now, NULL);
		long remaining = (deadline.tv_sec - now.tv_sec) * 1000L
				+ (deadline.tv_usec - now.tv_usec) / 1000;
		if (remaining <= 0) {
			if (error_text != NULL) {
				*error_text = ZSTR_INIT_LITERAL("DTLS handshake timed out", 0);
			}
			return -1;
		}
		if (wait_ms < 0 || wait_ms > remaining) {
			wait_ms = (int)remaining;
		}

		int ready = php_pollfd_for_ms(dtlssock->s.socket, events, wait_ms);
		if (ready == 0) {
			/* Timer fired: let OpenSSL resend the last flight. */
			if (DTLSv1_handle_timeout(ssl) < 0) {
				if (error_text != NULL) {
					*error_text = ZSTR_INIT_LITERAL("DTLS handshake timed out", 0);
				}
				return -1;
			}
		} else if (ready < 0) {
			if (error_text != NULL) {
				*error_text =
						ZSTR_INIT_LITERAL("DTLS handshake failed while waiting for the socket", 0);
			}
			return -1;
		}
	}
}

/* Verify the peer certificate against the peer_fingerprint option, if set. */
static int php_openssl_dtls_check_fingerprint(php_stream *stream, php_openssl_dtls_data_t *dtlssock,
		zend_string **error_text)
{
	zval *val;
	if (!GET_VER_OPT("peer_fingerprint")) {
		return 0;
	}

	X509 *peer = SSL_get_peer_certificate(dtlssock->ssl_handle);
	bool match = peer != NULL && php_openssl_x509_fingerprint_match(stream, peer, val);
	if (peer != NULL) {
		X509_free(peer);
	}

	if (!match) {
		if (error_text != NULL) {
			*error_text = ZSTR_INIT_LITERAL("peer_fingerprint match failure", 0);
		}
		return -1;
	}

	return 0;
}

/* Per-process secret for the DTLSv1_listen cookie (an HMAC of the peer address,
 * so the server stays stateless during the HelloVerifyRequest exchange). */
#define PHP_OPENSSL_DTLS_COOKIE_SECRET_LEN 16
static unsigned char php_openssl_dtls_cookie_secret[PHP_OPENSSL_DTLS_COOKIE_SECRET_LEN];
static bool php_openssl_dtls_cookie_secret_ready = false;

static bool php_openssl_dtls_peer_addr(SSL *ssl, struct sockaddr_storage *peer, socklen_t *peerlen)
{
	memset(peer, 0, sizeof(*peer));
	if (BIO_dgram_get_peer(SSL_get_rbio(ssl), peer) <= 0) {
		return false;
	}
	*peerlen = (peer->ss_family == AF_INET6) ? sizeof(struct sockaddr_in6) : sizeof(struct sockaddr_in);
	return true;
}

static int php_openssl_dtls_cookie_generate(SSL *ssl, unsigned char *cookie, unsigned int *cookie_len)
{
	struct sockaddr_storage peer;
	socklen_t peerlen;

	if (!php_openssl_dtls_cookie_secret_ready) {
		if (RAND_bytes(php_openssl_dtls_cookie_secret, sizeof(php_openssl_dtls_cookie_secret)) != 1) {
			return 0;
		}
		php_openssl_dtls_cookie_secret_ready = true;
	}
	if (!php_openssl_dtls_peer_addr(ssl, &peer, &peerlen)) {
		return 0;
	}

	unsigned int len = 0;
	HMAC(EVP_sha256(), php_openssl_dtls_cookie_secret, sizeof(php_openssl_dtls_cookie_secret),
			(const unsigned char *)&peer, peerlen, cookie, &len);
	*cookie_len = len;
	return 1;
}

static int php_openssl_dtls_cookie_verify(SSL *ssl, const unsigned char *cookie, unsigned int cookie_len)
{
	unsigned char expected[EVP_MAX_MD_SIZE];
	unsigned int len = 0;
	struct sockaddr_storage peer;
	socklen_t peerlen;

	if (!php_openssl_dtls_cookie_secret_ready || !php_openssl_dtls_peer_addr(ssl, &peer, &peerlen)) {
		return 0;
	}
	HMAC(EVP_sha256(), php_openssl_dtls_cookie_secret, sizeof(php_openssl_dtls_cookie_secret),
			(const unsigned char *)&peer, peerlen, expected, &len);
	/* Constant-time compare: the cookie is derived from a secret HMAC. */
	return (cookie_len == len && CRYPTO_memcmp(cookie, expected, len) == 0) ? 1 : 0;
}

/* The session_new_cb / session_get_cb / session_remove_cb options let userland
 * keep the session cache. A single-peer server uses a new context per accepted
 * connection, so the built-in cache cannot resume across connections. */

enum php_openssl_dtls_session_cb_type {
	PHP_OPENSSL_DTLS_NEW_CB,
	PHP_OPENSSL_DTLS_GET_CB,
	PHP_OPENSSL_DTLS_REMOVE_CB,
};

/* Called when a new session is established. */
static int php_openssl_dtls_session_new_cb(SSL *ssl, SSL_SESSION *session)
{
	php_stream *stream = (php_stream *)SSL_get_ex_data(ssl, php_openssl_get_ssl_stream_data_index());
	if (!stream) {
		return 0;
	}
	php_openssl_dtls_data_t *dtlssock = (php_openssl_dtls_data_t *)stream->abstract;
	if (!dtlssock || !dtlssock->session_callbacks) {
		return 0;
	}

	/* The PHP object takes its own reference. */
	SSL_SESSION_up_ref(session);

	zval args[2];
	ZVAL_RES(&args[0], stream->res);
	php_openssl_session_object_init(&args[1], session);
	zend_call_known_fcc(&dtlssock->session_callbacks->new_cb, NULL, 2, args, NULL);
	zval_ptr_dtor(&args[1]);

	return 0;
}

/* Called when the server looks up a session by id. */
static SSL_SESSION *php_openssl_dtls_session_get_cb(SSL *ssl, const unsigned char *session_id,
		int session_id_len, int *copy)
{
	*copy = 0;
	php_stream *stream = (php_stream *)SSL_get_ex_data(ssl, php_openssl_get_ssl_stream_data_index());
	if (!stream) {
		return NULL;
	}
	php_openssl_dtls_data_t *dtlssock = (php_openssl_dtls_data_t *)stream->abstract;
	if (!dtlssock || !dtlssock->session_callbacks) {
		return NULL;
	}

	zval args[2];
	zval retval;
	ZVAL_RES(&args[0], stream->res);
	ZVAL_STRINGL(&args[1], (char *)session_id, session_id_len);

	SSL_SESSION *session = NULL;
	zend_call_known_fcc(&dtlssock->session_callbacks->get_cb, &retval, 2, args, NULL);
	zval_ptr_dtor(&args[1]);

	if (php_openssl_is_session_ce(&retval)) {
		SSL_SESSION *found = php_openssl_session_from_zval(&retval);
		if (found != NULL) {
			/* OpenSSL takes ownership of the returned session. */
			SSL_SESSION_up_ref(found);
			session = found;
		}
	} else if (Z_TYPE(retval) != IS_NULL) {
		zend_type_error("session_get_cb return type must be null or Openssl\\Session");
	}
	zval_ptr_dtor(&retval);

	return session;
}

/* Called when a session is removed from the cache. */
static void php_openssl_dtls_session_remove_cb(SSL_CTX *ctx, SSL_SESSION *session)
{
	php_stream *stream = (php_stream *)SSL_CTX_get_ex_data(ctx, php_openssl_dtls_get_ctx_stream_index());
	if (!stream) {
		return;
	}
	php_openssl_dtls_data_t *dtlssock = (php_openssl_dtls_data_t *)stream->abstract;
	if (!dtlssock || !dtlssock->session_callbacks) {
		return;
	}

	unsigned int session_id_len = 0;
	const unsigned char *session_id = SSL_SESSION_get_id(session, &session_id_len);

	zval args[2];
	ZVAL_RES(&args[0], stream->res);
	ZVAL_STRINGL(&args[1], (char *)session_id, session_id_len);
	zend_call_known_fcc(&dtlssock->session_callbacks->remove_cb, NULL, 2, args, NULL);
	zval_ptr_dtor(&args[1]);
}

/* Validate a session callback option and store it, allocating the struct once. */
static zend_result php_openssl_dtls_store_session_cb(php_stream *stream,
		php_openssl_dtls_data_t *dtlssock, const zval *callable,
		enum php_openssl_dtls_session_cb_type cb_type)
{
	const char *name = cb_type == PHP_OPENSSL_DTLS_NEW_CB ? "session_new_cb"
			: cb_type == PHP_OPENSSL_DTLS_GET_CB ? "session_get_cb" : "session_remove_cb";

	char *is_callable_error = NULL;
	zend_fcall_info_cache fcc;
	if (!zend_is_callable_ex(callable, NULL, 0, NULL, &fcc, &is_callable_error)) {
		if (is_callable_error) {
			zend_type_error("%s must be a valid callback, %s", name, is_callable_error);
			efree(is_callable_error);
		} else {
			zend_type_error("%s must be a valid callback", name);
		}
		return FAILURE;
	}

	if (!dtlssock->session_callbacks) {
		dtlssock->session_callbacks = pecalloc(1, sizeof(*dtlssock->session_callbacks), 0);
		dtlssock->session_callbacks->refcount = 1;
	}

	zend_fcc_addref(&fcc);
	switch (cb_type) {
		case PHP_OPENSSL_DTLS_NEW_CB: dtlssock->session_callbacks->new_cb = fcc; break;
		case PHP_OPENSSL_DTLS_GET_CB: dtlssock->session_callbacks->get_cb = fcc; break;
		case PHP_OPENSSL_DTLS_REMOVE_CB: dtlssock->session_callbacks->remove_cb = fcc; break;
	}
	return SUCCESS;
}

/* Configure server-side session resumption from the "ssl" context options. */
static zend_result php_openssl_dtls_setup_server_session(php_stream *stream,
		php_openssl_dtls_data_t *dtlssock)
{
	zval *val;
	bool has_get_cb = false, has_new_cb = false, has_session_id_context = false;

	if (php_stream_is_persistent(stream) &&
			(GET_VER_OPT("session_new_cb") || GET_VER_OPT("session_get_cb")
					|| GET_VER_OPT("session_remove_cb"))) {
		php_stream_warn(stream, PersistentNotSupported,
				"session callbacks are not supported for persistent dtls:// streams");
		return FAILURE;
	}

	if (GET_VER_OPT("session_get_cb")) {
		if (php_openssl_dtls_store_session_cb(stream, dtlssock, val, PHP_OPENSSL_DTLS_GET_CB) == FAILURE) {
			return FAILURE;
		}
		has_get_cb = true;
	}

	if (GET_VER_OPT("session_id_context")) {
		if (Z_TYPE_P(val) != IS_STRING || Z_STRLEN_P(val) == 0) {
			zend_type_error("session_id_context must be a non empty string");
			return FAILURE;
		}
		SSL_CTX_set_session_id_context(dtlssock->ctx,
				(const unsigned char *)Z_STRVAL_P(val), Z_STRLEN_P(val));
		has_session_id_context = true;
	}

	if (GET_VER_OPT("session_new_cb")) {
		if (php_openssl_dtls_store_session_cb(stream, dtlssock, val, PHP_OPENSSL_DTLS_NEW_CB) == FAILURE) {
			return FAILURE;
		}
		has_new_cb = true;
	}

	if (has_get_cb && !has_new_cb) {
		zend_value_error("session_new_cb is required when session_get_cb is provided");
		return FAILURE;
	}
	/* Server-side resumption needs a session id context. */
	if (has_get_cb && !has_session_id_context) {
		zend_value_error("session_id_context must be set when session_get_cb is provided");
		return FAILURE;
	}

	if (GET_VER_OPT("session_remove_cb")) {
		if (php_openssl_dtls_store_session_cb(stream, dtlssock, val, PHP_OPENSSL_DTLS_REMOVE_CB) == FAILURE) {
			return FAILURE;
		}
	}

	if (has_get_cb) {
		/* External cache mode - the callbacks hold the sessions. */
		SSL_CTX_set_ex_data(dtlssock->ctx, php_openssl_dtls_get_ctx_stream_index(), stream);
		SSL_CTX_set_session_cache_mode(dtlssock->ctx,
				SSL_SESS_CACHE_SERVER | SSL_SESS_CACHE_NO_INTERNAL);
		SSL_CTX_sess_set_new_cb(dtlssock->ctx, php_openssl_dtls_session_new_cb);
		SSL_CTX_sess_set_get_cb(dtlssock->ctx, php_openssl_dtls_session_get_cb);
		if (dtlssock->session_callbacks
				&& ZEND_FCC_INITIALIZED(dtlssock->session_callbacks->remove_cb)) {
			SSL_CTX_sess_set_remove_cb(dtlssock->ctx, php_openssl_dtls_session_remove_cb);
		}
		/* Tickets bypass the id-based cache, so disable them here. */
		SSL_CTX_set_options(dtlssock->ctx, SSL_OP_NO_TICKET);
		if (GET_VER_OPT("no_ticket") && !zend_is_true(val)) {
			zend_value_error("Session tickets cannot be enabled when session_get_cb is set");
			return FAILURE;
		}

		if (GET_VER_OPT("session_timeout")) {
			zend_long timeout = zval_get_long(val);
			if (timeout <= 0) {
				zend_value_error("session_timeout must be positive");
				return FAILURE;
			}
			SSL_CTX_set_timeout(dtlssock->ctx, timeout);
		}
	} else {
		SSL_CTX_set_session_cache_mode(dtlssock->ctx, SSL_SESS_CACHE_OFF);
	}

	return SUCCESS;
}

/* Set up the server SSL_CTX (cert/verify options plus the cookie callbacks that
 * DTLSv1_listen needs for the stateless HelloVerifyRequest exchange). */
static int php_openssl_dtls_server_ctx(php_stream *stream, php_openssl_dtls_data_t *dtlssock)
{
	dtlssock->ctx = SSL_CTX_new(DTLS_server_method());
	if (dtlssock->ctx == NULL) {
		php_stream_warn(stream, CreateFailed, "DTLS context creation failure");
		return -1;
	}
	if (php_openssl_dtls_apply_context(stream, dtlssock) != 0) {
		SSL_CTX_free(dtlssock->ctx);
		dtlssock->ctx = NULL;
		return -1;
	}
	SSL_CTX_set_cookie_generate_cb(dtlssock->ctx, php_openssl_dtls_cookie_generate);
	SSL_CTX_set_cookie_verify_cb(dtlssock->ctx, php_openssl_dtls_cookie_verify);
	if (php_openssl_dtls_setup_server_session(stream, dtlssock) == FAILURE) {
		SSL_CTX_free(dtlssock->ctx);
		dtlssock->ctx = NULL;
		return -1;
	}
	return 0;
}

/* Accept one peer: run the cookie exchange with DTLSv1_listen, connect the
 * listening socket to that peer, and finish the handshake on it. */
static int php_openssl_dtls_accept(php_stream *stream, php_openssl_dtls_data_t *listen,
		php_stream_xport_param *xparam STREAMS_DC)
{
	if (listen->s.socket == SOCK_ERR) {
		/* The listening socket is handed to the first accepted peer. */
		php_error_docref(NULL, E_WARNING, "This dtls:// server has already accepted its peer");
		return -1;
	}

	SSL *ssl = SSL_new(listen->ctx);
	if (ssl == NULL) {
		php_stream_warn(stream, CreateFailed, "DTLS handle creation failure");
		return -1;
	}
	BIO *bio = BIO_new_dgram(listen->s.socket, BIO_NOCLOSE);
	if (bio == NULL) {
		SSL_free(ssl);
		return -1;
	}
	SSL_set_bio(ssl, bio, bio);

	/* Bound the whole accept by a deadline (not each poll), so a bogus-ClientHello
	 * flood can't keep DTLSv1_listen() spinning past the timeout; with no timeout
	 * we block. */
	struct timeval *tmo = xparam->inputs.timeout;
	struct timeval deadline;
	bool has_deadline = tmo != NULL && (tmo->tv_sec > 0 || tmo->tv_usec > 0);
	if (has_deadline) {
		gettimeofday(&deadline, NULL);
		deadline.tv_sec += tmo->tv_sec;
		deadline.tv_usec += tmo->tv_usec;
		if (deadline.tv_usec >= 1000000) {
			deadline.tv_sec++;
			deadline.tv_usec -= 1000000;
		}
	}

	BIO_ADDR *client_addr = BIO_ADDR_new();
	if (client_addr == NULL) {
		SSL_free(ssl);
		return -1;
	}
	for (;;) {
		int ret = DTLSv1_listen(ssl, client_addr);
		if (ret > 0) {
			break;
		}
		if (ret < 0) {
			BIO_ADDR_free(client_addr);
			SSL_free(ssl);
			return -1;
		}

		int wait_ms = -1;
		if (has_deadline) {
			struct timeval now;
			gettimeofday(&now, NULL);
			long remaining = (deadline.tv_sec - now.tv_sec) * 1000L
					+ (deadline.tv_usec - now.tv_usec) / 1000;
			if (remaining <= 0) {
				BIO_ADDR_free(client_addr);
				SSL_free(ssl);
				return -1;
			}
			wait_ms = remaining > INT_MAX ? INT_MAX : (int)remaining;
		}
		if (php_pollfd_for_ms(listen->s.socket, POLLIN, wait_ms) <= 0) {
			BIO_ADDR_free(client_addr);
			SSL_free(ssl);
			return -1;
		}
	}

	/* Turn the BIO_ADDR the cookie exchange gave us into a sockaddr. */
	struct sockaddr_storage peer;
	socklen_t peerlen = 0;
	int family = BIO_ADDR_family(client_addr);
	memset(&peer, 0, sizeof(peer));
	if (family == AF_INET) {
		struct sockaddr_in *sin = (struct sockaddr_in *)&peer;
		size_t addrlen = sizeof(sin->sin_addr);
		sin->sin_family = AF_INET;
		sin->sin_port = BIO_ADDR_rawport(client_addr);
		BIO_ADDR_rawaddress(client_addr, &sin->sin_addr, &addrlen);
		peerlen = sizeof(struct sockaddr_in);
#ifdef HAVE_STRUCT_SOCKADDR_SA_LEN
		sin->sin_len = sizeof(struct sockaddr_in);
#endif
	}
#ifdef HAVE_IPV6
	else if (family == AF_INET6) {
		struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)&peer;
		size_t addrlen = sizeof(sin6->sin6_addr);
		sin6->sin6_family = AF_INET6;
		sin6->sin6_port = BIO_ADDR_rawport(client_addr);
		BIO_ADDR_rawaddress(client_addr, &sin6->sin6_addr, &addrlen);
		peerlen = sizeof(struct sockaddr_in6);
#ifdef HAVE_STRUCT_SOCKADDR_SA_LEN
		sin6->sin6_len = sizeof(struct sockaddr_in6);
#endif
	}
#endif
	BIO_ADDR_free(client_addr);
	if (peerlen == 0) {
		SSL_free(ssl);
		return -1;
	}

	/* Connect the listening socket to this peer and hand it to the accepted
	 * stream. Serving one peer per server stream avoids SO_REUSEPORT (which
	 * would let another process bind the same port and steal datagrams). */
	if (connect(listen->s.socket, (struct sockaddr *)&peer, peerlen) != 0) {
		SSL_free(ssl);
		return -1;
	}
	BIO_ctrl(bio, BIO_CTRL_DGRAM_SET_CONNECTED, 0, &peer);
	php_openssl_dtls_configure_mtu(stream, ssl, listen->s.socket, peer.ss_family);

	php_openssl_dtls_data_t *clisock = pemalloc(sizeof(*clisock), 0);
	memset(clisock, 0, sizeof(*clisock));
	clisock->s.socket = listen->s.socket;
	clisock->s.is_blocked = true;
	clisock->s.timeout.tv_sec = (time_t)FG(default_socket_timeout);
	clisock->ssl_handle = ssl;
	clisock->is_server = true;

	/* The socket now belongs to the accepted stream. */
	listen->s.socket = SOCK_ERR;

	php_stream *clistream = php_stream_alloc_rel(&php_openssl_dtls_socket_ops, clisock, NULL, "r+");
	if (clistream == NULL) {
		SSL_free(ssl);
		closesocket(clisock->s.socket);
		pefree(clisock, 0);
		return -1;
	}

	/* The accepted stream inherits the listener's context (cert options,
	 * keying material requests, ...). */
	clistream->ctx = stream->ctx;
	if (stream->ctx) {
		GC_ADDREF(stream->ctx);
	}

	/* The connection shares the listener's session callbacks, and the callbacks
	 * find its stream through the SSL during the handshake. */
	clisock->session_callbacks = listen->session_callbacks;
	if (clisock->session_callbacks) {
		clisock->session_callbacks->refcount++;
	}
	SSL_set_ex_data(ssl, php_openssl_get_ssl_stream_data_index(), clistream);

	/* Finish the handshake (SSL_do_handshake performs the accept). */
	if (php_openssl_dtls_handshake(clistream, clisock,
			xparam->inputs.timeout,
			xparam->want_errortext ? &xparam->outputs.error_text : NULL) != 0) {
		php_stream_close(clistream);
		return -1;
	}
	clisock->ssl_active = true;

	xparam->outputs.client = clistream;
	return 0;
}

/* Connect the datagram socket through the generic transport, then run DTLS on
 * top, the way xp_ssl.c layers TLS on a connected tcp:// stream. */
static int php_openssl_dtls_connect(php_stream *stream, php_openssl_dtls_data_t *dtlssock,
		php_stream_xport_param *xparam)
{
	/* The generic ops open and connect the datagram socket (recognised as UDP by
	 * the "udp_socket" ops label). */
	php_stream_socket_ops.set_option(stream, PHP_STREAM_OPTION_XPORT_API, 0, xparam);
	if (xparam->outputs.returncode != 0 || !dtlssock->enable_on_connect) {
		return xparam->outputs.returncode;
	}

	/* Run the handshake here rather than via the crypto ops so the connect timeout
	 * and the OpenSSL error text still reach the caller. */
	zend_string **err_text = xparam->want_errortext ? &xparam->outputs.error_text : NULL;
	if (php_openssl_dtls_setup_crypto(stream, dtlssock, NULL) != 0
			|| php_openssl_dtls_handshake(stream, dtlssock, &dtlssock->connect_timeout, err_text) != 0
			|| php_openssl_dtls_check_fingerprint(stream, dtlssock, err_text) != 0) {
		xparam->outputs.returncode = -1;
		return -1;
	}

	dtlssock->ssl_active = true;
	return 0;
}

/* Run or tear down the DTLS handshake for stream_socket_enable_crypto(). */
static int php_openssl_dtls_enable_crypto(php_stream *stream, php_openssl_dtls_data_t *dtlssock,
		php_stream_xport_crypto_param *cparam)
{
	if (cparam->inputs.activate && !dtlssock->ssl_active) {
		if (dtlssock->ssl_handle == NULL) {
			php_stream_warn(stream, Generic, "Crypto has not been set up; the setup op must run first");
			return -1;
		}
		if (php_openssl_dtls_handshake(stream, dtlssock, &dtlssock->s.timeout, NULL) != 0) {
			return -1;
		}
		if (php_openssl_dtls_check_fingerprint(stream, dtlssock, NULL) != 0) {
			return -1;
		}
		dtlssock->ssl_active = true;
		/* 1 (not 0) so stream_socket_enable_crypto() reports success, as xp_ssl.c does. */
		return 1;
	} else if (!cparam->inputs.activate && dtlssock->ssl_active) {
		SSL_shutdown(dtlssock->ssl_handle);
		dtlssock->ssl_active = false;
		return 1;
	}

	return -1;
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
				 * it into the read buffer.
				 * TODO: same idiom as php_openssl_sockop_cast() in xp_ssl.c. */
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
			/* Plain udp:// (no crypto): let the base hand out the fd/stdio. */
			if (!dtlssock->ssl_active) {
				return php_stream_socket_ops.cast(stream, castas, ret);
			}
			return FAILURE;
	}
}

/* Handle transport and stream options. */
static int php_openssl_dtls_sockop_set_option(php_stream *stream, int option, int value, void *ptrparam)
{
	php_openssl_dtls_data_t *dtlssock = (php_openssl_dtls_data_t *)stream->abstract;
	php_stream_xport_param *xparam;

	switch (option) {
		case PHP_STREAM_OPTION_META_DATA_API: {
			if (dtlssock->ssl_handle != NULL) {
				zval crypto;
				char *proto_str;
				const SSL_CIPHER *cipher;
				zval *val;

				array_init(&crypto);
				switch (SSL_version(dtlssock->ssl_handle)) {
					case DTLS1_2_VERSION: proto_str = "DTLSv1.2"; break;
					case DTLS1_VERSION:   proto_str = "DTLSv1.0"; break;
					default:              proto_str = "UNKNOWN"; break;
				}
				add_assoc_string(&crypto, "protocol", proto_str);

				cipher = SSL_get_current_cipher(dtlssock->ssl_handle);
				if (cipher != NULL) {
					add_assoc_string(&crypto, "cipher_name", (char *) SSL_CIPHER_get_name(cipher));
					add_assoc_long(&crypto, "cipher_bits", SSL_CIPHER_get_bits(cipher, NULL));
					add_assoc_string(&crypto, "cipher_version", (char *) SSL_CIPHER_get_version(cipher));
				}

				/* RFC 5705 exported keying material (e.g. DTLS-SRTP keys),
				 * requested via the keying_material_label/length context options. */
				char *km_label = NULL;
				size_t km_label_len = 0;
				GET_VER_OPT_STRINGL("keying_material_label", km_label, km_label_len);
				if (km_label != NULL && GET_VER_OPT("keying_material_length")) {
					zend_long km_len = zval_get_long(val);
					if (km_len > 0 && km_len <= 1024) {
						zend_string *km = zend_string_alloc((size_t)km_len, 0);
						if (SSL_export_keying_material(dtlssock->ssl_handle,
								(unsigned char *)ZSTR_VAL(km), (size_t)km_len,
								km_label, km_label_len, NULL, 0, 0) == 1) {
							ZSTR_VAL(km)[km_len] = '\0';
							add_assoc_str(&crypto, "keying_material", km);
						} else {
							zend_string_release(km);
						}
					}
				}

				/* Expose the negotiated session so the caller can resume it later
				 * (session_data), and whether this handshake was resumed. */
				SSL_SESSION *session = SSL_get1_session(dtlssock->ssl_handle);
				if (session != NULL) {
					zval zsession;
					php_openssl_session_object_init(&zsession, session);
					add_assoc_zval(&crypto, "session", &zsession);
				}
				add_assoc_bool(&crypto, "session_reused",
						SSL_session_reused(dtlssock->ssl_handle) == 1);

				add_assoc_zval((zval *)ptrparam, "crypto", &crypto);
			}
			add_assoc_bool((zval *)ptrparam, "timed_out", dtlssock->s.timeout_event);
			add_assoc_bool((zval *)ptrparam, "blocked", dtlssock->s.is_blocked);
			add_assoc_bool((zval *)ptrparam, "eof", stream->eof);
			return PHP_STREAM_OPTION_RETURN_OK;
		}

		case PHP_STREAM_OPTION_CRYPTO_API: {
			php_stream_xport_crypto_param *cparam = (php_stream_xport_crypto_param *)ptrparam;

			switch (cparam->op) {
				case STREAM_XPORT_CRYPTO_OP_SETUP:
					/* DTLS 1.3 is a defined method but not implemented yet. */
					if (cparam->inputs.method & STREAM_CRYPTO_METHOD_DTLSv1_3_SERVER) {
						php_stream_warn(stream, Generic, "DTLS 1.3 is not supported yet");
						cparam->outputs.returncode = -1;
						return PHP_STREAM_OPTION_RETURN_OK;
					}
					cparam->outputs.returncode =
							php_openssl_dtls_setup_crypto(stream, dtlssock, NULL);
					return PHP_STREAM_OPTION_RETURN_OK;

				case STREAM_XPORT_CRYPTO_OP_ENABLE:
					cparam->outputs.returncode =
							php_openssl_dtls_enable_crypto(stream, dtlssock, cparam);
					return PHP_STREAM_OPTION_RETURN_OK;

				case STREAM_XPORT_CRYPTO_OP_GET_STATUS:
					cparam->outputs.returncode = dtlssock->ssl_active;
					return PHP_STREAM_OPTION_RETURN_OK;

				default:
					return PHP_STREAM_OPTION_RETURN_ERR;
			}
		}

		case PHP_STREAM_OPTION_XPORT_API:
			xparam = (php_stream_xport_param *)ptrparam;

			switch (xparam->op) {
				case STREAM_XPORT_OP_CONNECT:
				case STREAM_XPORT_OP_CONNECT_ASYNC:
					xparam->outputs.returncode =
							php_openssl_dtls_connect(stream, dtlssock, xparam);
					return PHP_STREAM_OPTION_RETURN_OK;

				case STREAM_XPORT_OP_BIND: {
					/* s.socktype makes the generic ops bind a SOCK_DGRAM socket. */
					php_stream_socket_ops.set_option(stream, option, value, ptrparam);
					if (xparam->outputs.returncode != 0 || !dtlssock->enable_on_connect) {
						return PHP_STREAM_OPTION_RETURN_OK;
					}
					/* dtls:// server: keep the fd non-blocking for the accept timers
					 * and set up the cookie/handshake context. */
					php_set_sock_blocking(dtlssock->s.socket, 0);
					dtlssock->is_server = true;
					xparam->outputs.returncode = php_openssl_dtls_server_ctx(stream, dtlssock);
					if (xparam->outputs.returncode != 0) {
						closesocket(dtlssock->s.socket);
						dtlssock->s.socket = SOCK_ERR;
					}
					return PHP_STREAM_OPTION_RETURN_OK;
				}

				case STREAM_XPORT_OP_LISTEN:
					/* DTLS has no socket-level listen; accept uses DTLSv1_listen. */
					xparam->outputs.returncode = 0;
					return PHP_STREAM_OPTION_RETURN_OK;

				case STREAM_XPORT_OP_ACCEPT:
					xparam->outputs.returncode = php_openssl_dtls_accept(stream, dtlssock, xparam STREAMS_CC);
					return PHP_STREAM_OPTION_RETURN_OK;

				default:
					/* Local/peer name, shutdown, etc. operate on the socket. */
					return php_stream_socket_ops.set_option(stream, option, value, ptrparam);
			}

		case PHP_STREAM_OPTION_BLOCKING: {
			if (!dtlssock->ssl_active) {
				/* Plain udp:// socket: let the base handler flip the fd. */
				return php_stream_socket_ops.set_option(stream, option, value, ptrparam);
			}
			/* The fd must stay non-blocking for the DTLS timers, so only track the
			 * stream-level mode (the base handler would flip the fd too). */
			int old = dtlssock->s.is_blocked;
			dtlssock->s.is_blocked = value;
			return old;
		}
	}

	/* Read timeout, liveness check and the rest operate on the embedded base
	 * socket data, so defer to the generic socket handler. */
	return php_stream_socket_ops.set_option(stream, option, value, ptrparam);
}

static const php_stream_ops php_openssl_dtls_socket_ops = {
	php_openssl_dtls_sockop_write, php_openssl_dtls_sockop_read,
	php_openssl_dtls_sockop_close, NULL, /* flush */
	"udp_socket/dtls",
	NULL, /* seek */
	php_openssl_dtls_sockop_cast,
	NULL, /* stat */
	php_openssl_dtls_sockop_set_option,
	true, /* is_dgram */
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
	if (timeout != NULL) {
		dtlssock->connect_timeout = *timeout;
	} else {
		dtlssock->connect_timeout = dtlssock->s.timeout;
	}

	if (strncmp(proto, "udp", protolen) == 0) {
		/* Plain udp://: stays a datagram socket until stream_socket_enable_crypto(). */
		dtlssock->enable_on_connect = 0;
	} else {
		/* dtls:// / dtlsv1.2://: run DTLS on connect. */
		dtlssock->enable_on_connect = 1;
		dtlssock->method = STREAM_CRYPTO_METHOD_DTLSv1_2_CLIENT;
	}

	stream = php_stream_alloc_rel(&php_openssl_dtls_socket_ops, dtlssock, persistent_id, "r+");
	if (stream == NULL) {
		pefree(dtlssock, persistent_id ? 1 : 0);
		return NULL;
	}

	dtlssock->url_name = php_openssl_get_url_name(resourcename, resourcenamelen, persistent_id != NULL, context);

	return stream;
}

#endif /* OPENSSL_NO_DTLS */
