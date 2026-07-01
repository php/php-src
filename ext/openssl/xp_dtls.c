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
#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/hmac.h>
#include <openssl/rand.h>

#ifndef OPENSSL_NO_DTLS

/* The base socket data is embedded first so the generic socket option handlers
 * can be reused; the datagram BIO is owned by ssl_handle (freed with it). */
typedef struct _php_openssl_dtls_data_t {
	php_netstream_data_t s;
	SSL_CTX *ctx;
	SSL *ssl_handle;
	int method;             /* STREAM_CRYPTO_METHOD_DTLS_* */
	int enable_on_connect;  /* dtls:// scheme: run the handshake on connect */
} php_openssl_dtls_data_t;

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

/* The socket is non-blocking, so on WANT_* we poll (up to the read timeout when
 * blocking) and retry. */
static ssize_t php_openssl_dtls_io(bool read, php_stream *stream, char *buf, size_t count)
{
	php_openssl_dtls_data_t *dtlssock = (php_openssl_dtls_data_t *)stream->abstract;
	SSL *ssl = dtlssock->ssl_handle;

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

		if (ssl != NULL) {
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
		} else {
			/* Plain UDP: crypto not enabled (e.g. a udp:// stream before
			 * stream_socket_enable_crypto()). */
			ssize_t n = read
					? recv(dtlssock->s.socket, buf, count, 0)
					: send(dtlssock->s.socket, buf, count, 0);
			if (n > 0) {
				php_stream_notify_progress_increment(PHP_STREAM_CONTEXT(stream), (size_t)n, 0);
				return n;
			}
			if (n == 0) {
				return 0;
			}
			if (errno != EAGAIN
#if EAGAIN != EWOULDBLOCK
					&& errno != EWOULDBLOCK
#endif
			) {
				return -1;
			}
			events = read ? POLLIN : POLLOUT;
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

	/* Peer verification, on by default. A peer_fingerprint authenticates the peer
	 * by itself (checked after the handshake), so it overrides CA verification. */
	bool verify_peer = !(GET_VER_OPT("verify_peer") && !zend_is_true(val));
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
		SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);
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

/* Create the DTLS context, SSL object and datagram BIO. */
static int php_openssl_dtls_setup_crypto(php_stream *stream, php_openssl_dtls_data_t *dtlssock,
		const char *peer_host)
{
	BIO *bio;
	zval *val;

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

	/* Hostname verification needs the SSL object; the verify mode is inherited
	 * from the context. */
	bool verify_peer = !(GET_VER_OPT("verify_peer") && !zend_is_true(val));
	bool verify_name = !(GET_VER_OPT("verify_peer_name") && !zend_is_true(val));
	if (verify_peer && verify_name) {
		const char *name = peer_host;
		GET_VER_OPT_STRING("peer_name", name);
		if (name != NULL) {
			/* An IP literal needs IP-address matching, not DNS-name matching. */
			X509_VERIFY_PARAM *param = SSL_get0_param(dtlssock->ssl_handle);
			if (X509_VERIFY_PARAM_set1_ip_asc(param, name) != 1) {
				SSL_set1_host(dtlssock->ssl_handle, name);
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

	/* Always bound the handshake so a silent peer can't make OpenSSL's DTLS timer
	 * retransmit forever: use the connect timeout, else the default socket timeout. */
	struct timeval *tmo = xparam->inputs.timeout;
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
				if (xparam->want_errortext) {
					xparam->outputs.error_text = strpprintf(0, "DTLS handshake failed: %s",
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
			if (xparam->want_errortext) {
				xparam->outputs.error_text = ZSTR_INIT_LITERAL("DTLS handshake timed out", 0);
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

static bool php_openssl_dtls_fingerprint_is_equal(php_stream *stream, X509 *peer, const char *method,
		zend_string *expected)
{
	zend_string *fingerprint = php_openssl_x509_fingerprint(peer, method, false, stream);
	bool is_equal = false;

	if (fingerprint != NULL) {
		is_equal = zend_string_equals_ci(fingerprint, expected);
		zend_string_release_ex(fingerprint, false);
	}

	return is_equal;
}

/* Match the peer certificate against a md5/sha1 hash (by length) or an
 * [algo => hash] map. */
static bool php_openssl_dtls_fingerprint_match(php_stream *stream, X509 *peer, zval *val)
{
	if (Z_TYPE_P(val) == IS_STRING) {
		const char *method = NULL;
		switch (Z_STRLEN_P(val)) {
			case 32: method = "md5"; break;
			case 40: method = "sha1"; break;
		}
		if (method == NULL) {
			php_stream_warn(stream, AuthFailed, "peer_fingerprint length doesn't match a md5 or sha1 hash");
			return false;
		}
		return php_openssl_dtls_fingerprint_is_equal(stream, peer, method, Z_STR_P(val));
	}

	if (Z_TYPE_P(val) == IS_ARRAY) {
		zval *current;
		zend_string *key;

		if (zend_hash_num_elements(Z_ARRVAL_P(val)) == 0) {
			php_stream_warn(stream, Generic, "Invalid peer_fingerprint array; [algo => fingerprint] form required");
			return false;
		}
		ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(val), key, current) {
			if (key == NULL || Z_TYPE_P(current) != IS_STRING) {
				php_stream_warn(stream, Generic, "Invalid peer_fingerprint array; [algo => fingerprint] form required");
				return false;
			}
			if (!php_openssl_dtls_fingerprint_is_equal(stream, peer, ZSTR_VAL(key), Z_STR_P(current))) {
				return false;
			}
		} ZEND_HASH_FOREACH_END();
		return true;
	}

	php_stream_warn(stream, Generic, "Invalid peer_fingerprint; a string or [algo => fingerprint] array required");
	return false;
}

/* Verify the peer certificate against the peer_fingerprint option, if set. This
 * lets the caller authenticate the peer by fingerprint instead of a CA chain. */
static int php_openssl_dtls_check_fingerprint(php_stream *stream, php_openssl_dtls_data_t *dtlssock,
		php_stream_xport_param *xparam)
{
	zval *val;
	if (!GET_VER_OPT("peer_fingerprint")) {
		return 0;
	}

	X509 *peer = SSL_get_peer_certificate(dtlssock->ssl_handle);
	bool match = peer != NULL && php_openssl_dtls_fingerprint_match(stream, peer, val);
	if (peer != NULL) {
		X509_free(peer);
	}

	if (!match) {
		if (xparam->want_errortext) {
			xparam->outputs.error_text = ZSTR_INIT_LITERAL("peer_fingerprint match failure", 0);
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
	return (cookie_len == len && memcmp(cookie, expected, len) == 0) ? 1 : 0;
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
	return 0;
}

/* Accept one peer: run the cookie exchange with DTLSv1_listen, move to a socket
 * connected to that peer (sharing the local port), and finish the handshake. */
static int php_openssl_dtls_accept(php_stream *stream, php_openssl_dtls_data_t *listen,
		php_stream_xport_param *xparam)
{
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

	int timeout_ms = xparam->inputs.timeout != NULL
			? (int)(xparam->inputs.timeout->tv_sec * 1000 + xparam->inputs.timeout->tv_usec / 1000)
			: -1;

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
		if (ret < 0 || php_pollfd_for_ms(listen->s.socket, POLLIN, timeout_ms) <= 0) {
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
	}
#ifdef HAVE_IPV6
	else if (family == AF_INET6) {
		struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)&peer;
		size_t addrlen = sizeof(sin6->sin6_addr);
		sin6->sin6_family = AF_INET6;
		sin6->sin6_port = BIO_ADDR_rawport(client_addr);
		BIO_ADDR_rawaddress(client_addr, &sin6->sin6_addr, &addrlen);
		peerlen = sizeof(struct sockaddr_in6);
	}
#endif
	BIO_ADDR_free(client_addr);
	if (peerlen == 0) {
		SSL_free(ssl);
		return -1;
	}

	/* A fresh socket bound to the same local address and connected to the peer
	 * keeps the listening socket free for the next accept. */
	struct sockaddr_storage local;
	socklen_t locallen = sizeof(local);
	php_socket_t clifd;
	int on = 1;

	if (getsockname(listen->s.socket, (struct sockaddr *)&local, &locallen) != 0
			|| (clifd = socket(peer.ss_family, SOCK_DGRAM, 0)) == SOCK_ERR) {
		SSL_free(ssl);
		return -1;
	}
	setsockopt(clifd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
#ifdef SO_REUSEPORT
	setsockopt(clifd, SOL_SOCKET, SO_REUSEPORT, (char *)&on, sizeof(on));
#endif
	if (bind(clifd, (struct sockaddr *)&local, locallen) != 0
			|| connect(clifd, (struct sockaddr *)&peer, peerlen) != 0) {
		closesocket(clifd);
		SSL_free(ssl);
		return -1;
	}
	php_set_sock_blocking(clifd, 0);

	BIO_set_fd(bio, clifd, BIO_NOCLOSE);
	BIO_ctrl(bio, BIO_CTRL_DGRAM_SET_CONNECTED, 0, &peer);

	php_openssl_dtls_data_t *clisock = pemalloc(sizeof(*clisock), 0);
	memset(clisock, 0, sizeof(*clisock));
	clisock->s.socket = clifd;
	clisock->s.is_blocked = true;
	clisock->s.timeout.tv_sec = (time_t)FG(default_socket_timeout);
	clisock->ssl_handle = ssl;
	clisock->method = STREAM_CRYPTO_METHOD_DTLS_SERVER;

	php_stream *clistream = php_stream_alloc_rel(&php_openssl_dtls_socket_ops, clisock, NULL, "r+");
	if (clistream == NULL) {
		SSL_free(ssl);
		closesocket(clifd);
		pefree(clisock, 0);
		return -1;
	}

	/* Finish the handshake (SSL_do_handshake performs the accept). */
	if (php_openssl_dtls_handshake(clistream, clisock, xparam) != 0) {
		php_stream_close(clistream);
		return -1;
	}

	xparam->outputs.client = clistream;
	return 0;
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

	if (dtlssock->s.socket == SOCK_ERR) {
		efree(host);
		return -1;
	}

	/* The I/O loop emulates blocking with poll, so the fd stays non-blocking. */
	php_set_sock_blocking(dtlssock->s.socket, 0);

	if (!dtlssock->enable_on_connect) {
		/* Plain udp:// stream; DTLS is enabled later via stream_socket_enable_crypto(). */
		efree(host);
		return 0;
	}

	if (php_openssl_dtls_setup_crypto(stream, dtlssock, host) != 0) {
		efree(host);
		return -1;
	}

	efree(host);

	if (php_openssl_dtls_handshake(stream, dtlssock, xparam) != 0) {
		return -1;
	}

	return php_openssl_dtls_check_fingerprint(stream, dtlssock, xparam);
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
				add_assoc_zval((zval *)ptrparam, "crypto", &crypto);
			}
			add_assoc_bool((zval *)ptrparam, "timed_out", dtlssock->s.timeout_event);
			add_assoc_bool((zval *)ptrparam, "blocked", dtlssock->s.is_blocked);
			add_assoc_bool((zval *)ptrparam, "eof", stream->eof);
			return PHP_STREAM_OPTION_RETURN_OK;
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
					int portno = 0, err = 0;
					char *host = php_openssl_dtls_parse_ip_address(xparam, &portno);
					if (host == NULL) {
						xparam->outputs.returncode = -1;
						return PHP_STREAM_OPTION_RETURN_OK;
					}
					/* REUSEADDR+REUSEPORT so each accepted peer can bind a fresh
					 * socket on this same local address. */
					dtlssock->s.socket = php_network_bind_socket_to_local_addr(host, portno, SOCK_DGRAM,
							STREAM_SOCKOP_SO_REUSEADDR | STREAM_SOCKOP_SO_REUSEPORT,
							xparam->want_errortext ? &xparam->outputs.error_text : NULL, &err);
					efree(host);
					xparam->outputs.error_code = err;
					if (dtlssock->s.socket == SOCK_ERR) {
						xparam->outputs.returncode = -1;
						return PHP_STREAM_OPTION_RETURN_OK;
					}
					php_set_sock_blocking(dtlssock->s.socket, 0);
					dtlssock->method = STREAM_CRYPTO_METHOD_DTLS_SERVER;
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
					xparam->outputs.returncode = php_openssl_dtls_accept(stream, dtlssock, xparam);
					return PHP_STREAM_OPTION_RETURN_OK;

				default:
					/* Local/peer name, shutdown, etc. operate on the socket. */
					return php_stream_socket_ops.set_option(stream, option, value, ptrparam);
			}

		case PHP_STREAM_OPTION_BLOCKING: {
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
	dtlssock->method = STREAM_CRYPTO_METHOD_DTLS_CLIENT;

	/* dtls:// runs the handshake on connect; a plain udp:// enables it on demand
	 * (stream_socket_enable_crypto). */
	if (protolen != strlen("udp") || strncmp(proto, "udp", protolen) != 0) {
		dtlssock->enable_on_connect = 1;
	}

	stream = php_stream_alloc_rel(&php_openssl_dtls_socket_ops, dtlssock, persistent_id, "r+");
	if (stream == NULL) {
		pefree(dtlssock, persistent_id ? 1 : 0);
	}

	return stream;
}

#endif /* OPENSSL_NO_DTLS */
