/*
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | https://www.php.net/license/3_01.txt                                 |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Wez Furlong <wez@thebrainroom.com>                          |
  |          Daniel Lowrey <rdlowrey@php.net>                            |
  |          Chris Wright <daverandom@php.net>                           |
  |          Jakub Zelenka <bukka@php.net>                               |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "ext/standard/file.h"
#include "ext/standard/url.h"
#include "streams/php_streams_int.h"
#include "zend_smart_str.h"
#include "php_openssl.h"
#include "php_network.h"
#include <openssl/ssl.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/err.h>
#include <openssl/bn.h>
#include <openssl/dh.h>

#ifdef PHP_WIN32
#include "win32/winutil.h"
#include "win32/time.h"
#include <Ws2tcpip.h>
#include <Wincrypt.h>
/* These are from Wincrypt.h, they conflict with OpenSSL */
#undef X509_NAME
#undef X509_CERT_PAIR
#undef X509_EXTENSIONS
#endif

#ifndef MSG_DONTWAIT
# define MSG_DONTWAIT 0
#endif

#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif

/* Flags for determining allowed stream crypto methods */
#define STREAM_CRYPTO_IS_CLIENT            (1<<0)
#define STREAM_CRYPTO_METHOD_SSLv2         (1<<1)
#define STREAM_CRYPTO_METHOD_SSLv3         (1<<2)
#define STREAM_CRYPTO_METHOD_TLSv1_0       (1<<3)
#define STREAM_CRYPTO_METHOD_TLSv1_1       (1<<4)
#define STREAM_CRYPTO_METHOD_TLSv1_2       (1<<5)
#define STREAM_CRYPTO_METHOD_TLSv1_3       (1<<6)

#ifndef OPENSSL_NO_TLS1_METHOD
#define HAVE_TLS1 1
#endif

#ifndef OPENSSL_NO_TLS1_1_METHOD
#define HAVE_TLS11 1
#endif

#ifndef OPENSSL_NO_TLS1_2_METHOD
#define HAVE_TLS12 1
#endif

#if OPENSSL_VERSION_NUMBER >= 0x10101000 && !defined(OPENSSL_NO_TLS1_3)
#define HAVE_TLS13 1
#endif

#ifndef OPENSSL_NO_ECDH
#define HAVE_ECDH 1
#endif

#ifndef OPENSSL_NO_TLSEXT
#define HAVE_TLS_SNI 1
#define HAVE_TLS_ALPN 1
#endif

#if OPENSSL_VERSION_NUMBER >= 0x10100000L && !defined(LIBRESSL_VERSION_NUMBER)
#define HAVE_SEC_LEVEL 1
#endif

#ifndef OPENSSL_NO_SSL3
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
	if (GET_VER_OPT(_name)) _num = zval_get_long(val)

/* Used for peer verification in windows */
#define PHP_X509_NAME_ENTRY_TO_UTF8(ne, i, out) \
	ASN1_STRING_to_UTF8(&out, X509_NAME_ENTRY_get_data(X509_NAME_get_entry(ne, i)))

#if defined(HAVE_IPV6) && defined(HAVE_INET_PTON)
/* Used for IPv6 Address peer verification */
#define EXPAND_IPV6_ADDRESS(_str, _bytes) \
	do { \
		snprintf(_str, 40, "%X:%X:%X:%X:%X:%X:%X:%X", \
			_bytes[0] << 8 | _bytes[1], \
			_bytes[2] << 8 | _bytes[3], \
			_bytes[4] << 8 | _bytes[5], \
			_bytes[6] << 8 | _bytes[7], \
			_bytes[8] << 8 | _bytes[9], \
			_bytes[10] << 8 | _bytes[11], \
			_bytes[12] << 8 | _bytes[13], \
			_bytes[14] << 8 | _bytes[15] \
		); \
	} while(0)
#define HAVE_IPV6_SAN 1
#endif

#if PHP_OPENSSL_API_VERSION < 0x10100
static RSA *php_openssl_tmp_rsa_cb(SSL *s, int is_export, int keylength);
#endif

extern php_stream* php_openssl_get_stream_from_ssl_handle(const SSL *ssl);
extern zend_string* php_openssl_x509_fingerprint(X509 *peer, const char *method, bool raw);
extern int php_openssl_get_ssl_stream_data_index(void);
static struct timeval php_openssl_subtract_timeval(struct timeval a, struct timeval b);
static int php_openssl_compare_timeval(struct timeval a, struct timeval b);
static ssize_t php_openssl_sockop_io(int read, php_stream *stream, char *buf, size_t count);

const php_stream_ops php_openssl_socket_ops;

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

/* This implementation is very closely tied to the that of the native
 * sockets implemented in the core.
 * Don't try this technique in other extensions!
 * */
typedef struct _php_openssl_netstream_data_t {
	php_netstream_data_t s;
	SSL *ssl_handle;
	SSL_CTX *ctx;
	struct timeval connect_timeout;
	int enable_on_connect;
	int is_client;
	int ssl_active;
	php_stream_xport_crypt_method_t method;
	php_openssl_handshake_bucket_t *reneg;
	php_openssl_sni_cert_t *sni_certs;
	unsigned sni_cert_count;
#ifdef HAVE_TLS_ALPN
	php_openssl_alpn_ctx alpn_ctx;
#endif
	char *url_name;
	unsigned state_set:1;
	unsigned _spare:31;
} php_openssl_netstream_data_t;

/* it doesn't matter that we do some hash traversal here, since it is done only
 * in an error condition arising from a network connection problem */
static int php_openssl_is_http_stream_talking_to_iis(php_stream *stream) /* {{{ */
{
	if (Z_TYPE(stream->wrapperdata) == IS_ARRAY &&
		stream->wrapper &&
		strcasecmp(stream->wrapper->wops->label, "HTTP") == 0
	) {
		/* the wrapperdata is an array zval containing the headers */
		zval *tmp;

#define SERVER_MICROSOFT_IIS	"Server: Microsoft-IIS"
#define SERVER_GOOGLE "Server: GFE/"

		ZEND_HASH_FOREACH_VAL(Z_ARRVAL(stream->wrapperdata), tmp) {
			if (zend_string_equals_literal_ci(Z_STR_P(tmp), SERVER_MICROSOFT_IIS)) {
				return 1;
			} else if (zend_string_equals_literal_ci(Z_STR_P(tmp), SERVER_GOOGLE)) {
				return 1;
			}
		} ZEND_HASH_FOREACH_END();
	}
	return 0;
}
/* }}} */

static int php_openssl_handle_ssl_error(php_stream *stream, int nr_bytes, bool is_init) /* {{{ */
{
	php_openssl_netstream_data_t *sslsock = (php_openssl_netstream_data_t*)stream->abstract;
	int err = SSL_get_error(sslsock->ssl_handle, nr_bytes);
	char esbuf[512];
	smart_str ebuf = {0};
	unsigned long ecode;
	int retry = 1;

	switch(err) {
		case SSL_ERROR_ZERO_RETURN:
			/* SSL terminated (but socket may still be active) */
			retry = 0;
			break;
		case SSL_ERROR_WANT_READ:
		case SSL_ERROR_WANT_WRITE:
			/* re-negotiation, or perhaps the SSL layer needs more
			 * packets: retry in next iteration */
			errno = EAGAIN;
			retry = is_init ? 1 : sslsock->s.is_blocked;
			break;
		case SSL_ERROR_SYSCALL:
			if (ERR_peek_error() == 0) {
				if (nr_bytes == 0) {
					if (!php_openssl_is_http_stream_talking_to_iis(stream) && ERR_get_error() != 0) {
						php_error_docref(NULL, E_WARNING, "SSL: fatal protocol error");
					}
					SSL_set_shutdown(sslsock->ssl_handle, SSL_SENT_SHUTDOWN|SSL_RECEIVED_SHUTDOWN);
					stream->eof = 1;
					retry = 0;
				} else {
					char *estr = php_socket_strerror(php_socket_errno(), NULL, 0);

					php_error_docref(NULL, E_WARNING,
							"SSL: %s", estr);

					efree(estr);
					retry = 0;
				}
				break;
			}

			ZEND_FALLTHROUGH;
		default:
			/* some other error */
			ecode = ERR_get_error();

			switch (ERR_GET_REASON(ecode)) {
				case SSL_R_NO_SHARED_CIPHER:
					php_error_docref(NULL, E_WARNING,
							"SSL_R_NO_SHARED_CIPHER: no suitable shared cipher could be used.  "
							"This could be because the server is missing an SSL certificate "
							"(local_cert context option)");
					retry = 0;
					break;

				default:
					do {
						/* NULL is automatically added */
						ERR_error_string_n(ecode, esbuf, sizeof(esbuf));
						if (ebuf.s) {
							smart_str_appendc(&ebuf, '\n');
						}
						smart_str_appends(&ebuf, esbuf);
					} while ((ecode = ERR_get_error()) != 0);

					smart_str_0(&ebuf);

					php_error_docref(NULL, E_WARNING,
							"SSL operation failed with code %d. %s%s",
							err,
							ebuf.s ? "OpenSSL Error messages:\n" : "",
							ebuf.s ? ZSTR_VAL(ebuf.s) : "");
					if (ebuf.s) {
						smart_str_free(&ebuf);
					}
			}

			retry = 0;
			errno = 0;
	}
	return retry;
}
/* }}} */

static int verify_callback(int preverify_ok, X509_STORE_CTX *ctx) /* {{{ */
{
	php_stream *stream;
	SSL *ssl;
	int err, depth, ret;
	zval *val;
	zend_ulong allowed_depth = OPENSSL_DEFAULT_STREAM_VERIFY_DEPTH;


	ret = preverify_ok;

	/* determine the status for the current cert */
	err = X509_STORE_CTX_get_error(ctx);
	depth = X509_STORE_CTX_get_error_depth(ctx);

	/* conjure the stream & context to use */
	ssl = X509_STORE_CTX_get_ex_data(ctx, SSL_get_ex_data_X509_STORE_CTX_idx());
	stream = (php_stream*)SSL_get_ex_data(ssl, php_openssl_get_ssl_stream_data_index());

	/* if allow_self_signed is set, make sure that verification succeeds */
	if (err == X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT &&
		GET_VER_OPT("allow_self_signed") &&
		zend_is_true(val)
	) {
		ret = 1;
	}

	/* check the depth */
	GET_VER_OPT_LONG("verify_depth", allowed_depth);
	if ((zend_ulong)depth > allowed_depth) {
		ret = 0;
		X509_STORE_CTX_set_error(ctx, X509_V_ERR_CERT_CHAIN_TOO_LONG);
	}

	return ret;
}
/* }}} */

static int php_openssl_x509_fingerprint_cmp(X509 *peer, const char *method, const char *expected)
{
	zend_string *fingerprint;
	int result = -1;

	fingerprint = php_openssl_x509_fingerprint(peer, method, 0);
	if (fingerprint) {
		result = strcasecmp(expected, ZSTR_VAL(fingerprint));
		zend_string_release_ex(fingerprint, 0);
	}

	return result;
}

static bool php_openssl_x509_fingerprint_match(X509 *peer, zval *val)
{
	if (Z_TYPE_P(val) == IS_STRING) {
		const char *method = NULL;

		switch (Z_STRLEN_P(val)) {
			case 32:
				method = "md5";
				break;

			case 40:
				method = "sha1";
				break;
		}

		return method && php_openssl_x509_fingerprint_cmp(peer, method, Z_STRVAL_P(val)) == 0;
	} else if (Z_TYPE_P(val) == IS_ARRAY) {
		zval *current;
		zend_string *key;

		if (!zend_hash_num_elements(Z_ARRVAL_P(val))) {
			php_error_docref(NULL, E_WARNING, "Invalid peer_fingerprint array; [algo => fingerprint] form required");
			return 0;
		}

		ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(val), key, current) {
			if (key == NULL || Z_TYPE_P(current) != IS_STRING) {
				php_error_docref(NULL, E_WARNING, "Invalid peer_fingerprint array; [algo => fingerprint] form required");
				return 0;
			}
			if (php_openssl_x509_fingerprint_cmp(peer, ZSTR_VAL(key), Z_STRVAL_P(current)) != 0) {
				return 0;
			}
		} ZEND_HASH_FOREACH_END();

		return 1;
	} else {
		php_error_docref(NULL, E_WARNING,
			"Invalid peer_fingerprint value; fingerprint string or array of the form [algo => fingerprint] required");
	}

	return 0;
}

static bool php_openssl_matches_wildcard_name(const char *subjectname, const char *certname) /* {{{ */
{
	char *wildcard = NULL;
	ptrdiff_t prefix_len;
	size_t suffix_len, subject_len;

	if (strcasecmp(subjectname, certname) == 0) {
		return 1;
	}

	/* wildcard, if present, must only be present in the left-most component */
	if (!(wildcard = strchr(certname, '*')) || memchr(certname, '.', wildcard - certname)) {
		return 0;
	}

	/* 1) prefix, if not empty, must match subject */
	prefix_len = wildcard - certname;
	if (prefix_len && strncasecmp(subjectname, certname, prefix_len) != 0) {
		return 0;
	}

	suffix_len = strlen(wildcard + 1);
	subject_len = strlen(subjectname);
	if (suffix_len <= subject_len) {
		/* 2) suffix must match
		 * 3) no . between prefix and suffix
		 **/
		return strcasecmp(wildcard + 1, subjectname + subject_len - suffix_len) == 0 &&
			memchr(subjectname + prefix_len, '.', subject_len - suffix_len - prefix_len) == NULL;
	}

	return 0;
}
/* }}} */

static bool php_openssl_matches_san_list(X509 *peer, const char *subject_name) /* {{{ */
{
	int i, len;
	unsigned char *cert_name = NULL;
	char ipbuffer[64];

	GENERAL_NAMES *alt_names = X509_get_ext_d2i(peer, NID_subject_alt_name, 0, 0);
	int alt_name_count = sk_GENERAL_NAME_num(alt_names);

#ifdef HAVE_IPV6_SAN
	/* detect if subject name is an IPv6 address and expand once if required */
	char subject_name_ipv6_expanded[40];
	unsigned char ipv6[16];
	bool subject_name_is_ipv6 = false;
	subject_name_ipv6_expanded[0] = 0;

	if (inet_pton(AF_INET6, subject_name, &ipv6)) {
		EXPAND_IPV6_ADDRESS(subject_name_ipv6_expanded, ipv6);
		subject_name_is_ipv6 = true;
	}
#endif

	for (i = 0; i < alt_name_count; i++) {
		GENERAL_NAME *san = sk_GENERAL_NAME_value(alt_names, i);

		if (san->type == GEN_DNS) {
			ASN1_STRING_to_UTF8(&cert_name, san->d.dNSName);
			if ((size_t)ASN1_STRING_length(san->d.dNSName) != strlen((const char*)cert_name)) {
				OPENSSL_free(cert_name);
				/* prevent null-byte poisoning*/
				continue;
			}

			/* accommodate valid FQDN entries ending in "." */
			len = strlen((const char*)cert_name);
			if (len && strcmp((const char *)&cert_name[len-1], ".") == 0) {
				cert_name[len-1] = '\0';
			}

			if (php_openssl_matches_wildcard_name(subject_name, (const char *)cert_name)) {
				OPENSSL_free(cert_name);
				sk_GENERAL_NAME_pop_free(alt_names, GENERAL_NAME_free);

				return 1;
			}
			OPENSSL_free(cert_name);
		} else if (san->type == GEN_IPADD) {
			if (san->d.iPAddress->length == 4) {
				sprintf(ipbuffer, "%d.%d.%d.%d",
					san->d.iPAddress->data[0],
					san->d.iPAddress->data[1],
					san->d.iPAddress->data[2],
					san->d.iPAddress->data[3]
				);
				if (strcasecmp(subject_name, (const char*)ipbuffer) == 0) {
					sk_GENERAL_NAME_pop_free(alt_names, GENERAL_NAME_free);

					return 1;
				}
			}
#ifdef HAVE_IPV6_SAN
			else if (san->d.ip->length == 16 && subject_name_is_ipv6) {
				ipbuffer[0] = 0;
				EXPAND_IPV6_ADDRESS(ipbuffer, san->d.iPAddress->data);
				if (strcasecmp((const char*)subject_name_ipv6_expanded, (const char*)ipbuffer) == 0) {
					sk_GENERAL_NAME_pop_free(alt_names, GENERAL_NAME_free);

					return 1;
				}
			}
#endif
		}
	}

	sk_GENERAL_NAME_pop_free(alt_names, GENERAL_NAME_free);

	return 0;
}
/* }}} */

static bool php_openssl_matches_common_name(X509 *peer, const char *subject_name) /* {{{ */
{
	char buf[1024];
	X509_NAME *cert_name;
	bool is_match = 0;
	int cert_name_len;

	cert_name = X509_get_subject_name(peer);
	cert_name_len = X509_NAME_get_text_by_NID(cert_name, NID_commonName, buf, sizeof(buf));

	if (cert_name_len == -1) {
		php_error_docref(NULL, E_WARNING, "Unable to locate peer certificate CN");
	} else if ((size_t)cert_name_len != strlen(buf)) {
		php_error_docref(NULL, E_WARNING, "Peer certificate CN=`%.*s' is malformed", cert_name_len, buf);
	} else if (php_openssl_matches_wildcard_name(subject_name, buf)) {
		is_match = 1;
	} else {
		php_error_docref(NULL, E_WARNING,
			"Peer certificate CN=`%.*s' did not match expected CN=`%s'",
			cert_name_len, buf, subject_name);
	}

	return is_match;
}
/* }}} */

static int php_openssl_apply_peer_verification_policy(SSL *ssl, X509 *peer, php_stream *stream) /* {{{ */
{
	zval *val = NULL;
	zval *peer_fingerprint;
	char *peer_name = NULL;
	int err,
		must_verify_peer,
		must_verify_peer_name,
		must_verify_fingerprint;

	php_openssl_netstream_data_t *sslsock = (php_openssl_netstream_data_t*)stream->abstract;

	must_verify_peer = GET_VER_OPT("verify_peer")
		? zend_is_true(val)
		: sslsock->is_client;

	must_verify_peer_name = GET_VER_OPT("verify_peer_name")
		? zend_is_true(val)
		: sslsock->is_client;

	must_verify_fingerprint = GET_VER_OPT("peer_fingerprint");
	peer_fingerprint = val;

	if ((must_verify_peer || must_verify_peer_name || must_verify_fingerprint) && peer == NULL) {
		php_error_docref(NULL, E_WARNING, "Could not get peer certificate");
		return FAILURE;
	}

	/* Verify the peer against using CA file/path settings */
	if (must_verify_peer) {
		err = SSL_get_verify_result(ssl);
		switch (err) {
			case X509_V_OK:
				/* fine */
				break;
			case X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT:
				if (GET_VER_OPT("allow_self_signed") && zend_is_true(val)) {
					/* allowed */
					break;
				}
				/* not allowed, so fall through */
				ZEND_FALLTHROUGH;
			default:
				php_error_docref(NULL, E_WARNING,
						"Could not verify peer: code:%d %s",
						err,
						X509_verify_cert_error_string(err)
				);
				return FAILURE;
		}
	}

	/* If a peer_fingerprint match is required this trumps peer and peer_name verification */
	if (must_verify_fingerprint) {
		if (Z_TYPE_P(peer_fingerprint) == IS_STRING || Z_TYPE_P(peer_fingerprint) == IS_ARRAY) {
			if (!php_openssl_x509_fingerprint_match(peer, peer_fingerprint)) {
				php_error_docref(NULL, E_WARNING,
					"peer_fingerprint match failure"
				);
				return FAILURE;
			}
		} else {
			php_error_docref(NULL, E_WARNING,
				"Expected peer fingerprint must be a string or an array"
			);
			return FAILURE;
		}
	}

	/* verify the host name presented in the peer certificate */
	if (must_verify_peer_name) {
		GET_VER_OPT_STRING("peer_name", peer_name);

		/* If no peer name was specified we use the autodetected url name in client environments */
		if (peer_name == NULL && sslsock->is_client) {
			peer_name = sslsock->url_name;
		}

		if (peer_name) {
			if (php_openssl_matches_san_list(peer, peer_name)) {
				return SUCCESS;
			} else if (php_openssl_matches_common_name(peer, peer_name)) {
				return SUCCESS;
			} else {
				return FAILURE;
			}
		} else {
			return FAILURE;
		}
	}

	return SUCCESS;
}
/* }}} */

static int php_openssl_passwd_callback(char *buf, int num, int verify, void *data) /* {{{ */
{
	php_stream *stream = (php_stream *)data;
	zval *val = NULL;
	char *passphrase = NULL;
	/* TODO: could expand this to make a callback into PHP user-space */

	GET_VER_OPT_STRING("passphrase", passphrase);

	if (passphrase) {
		if (Z_STRLEN_P(val) < (size_t)num - 1) {
			memcpy(buf, Z_STRVAL_P(val), Z_STRLEN_P(val)+1);
			return (int)Z_STRLEN_P(val);
		}
	}
	return 0;
}
/* }}} */

#ifdef PHP_WIN32
#define RETURN_CERT_VERIFY_FAILURE(code) X509_STORE_CTX_set_error(x509_store_ctx, code); return 0;
static int php_openssl_win_cert_verify_callback(X509_STORE_CTX *x509_store_ctx, void *arg) /* {{{ */
{
	PCCERT_CONTEXT cert_ctx = NULL;
	PCCERT_CHAIN_CONTEXT cert_chain_ctx = NULL;
#if OPENSSL_VERSION_NUMBER < 0x10100000L
	X509 *cert = x509_store_ctx->cert;
#else
	X509 *cert = X509_STORE_CTX_get0_cert(x509_store_ctx);
#endif

	php_stream *stream;
	php_openssl_netstream_data_t *sslsock;
	zval *val;
	bool is_self_signed = 0;


	stream = (php_stream*)arg;
	sslsock = (php_openssl_netstream_data_t*)stream->abstract;

	{ /* First convert the x509 struct back to a DER encoded buffer and let Windows decode it into a form it can work with */
		unsigned char *der_buf = NULL;
		int der_len;

		der_len = i2d_X509(cert, &der_buf);
		if (der_len < 0) {
			unsigned long err_code, e;
			char err_buf[512];

			while ((e = ERR_get_error()) != 0) {
				err_code = e;
			}

			php_error_docref(NULL, E_WARNING, "Error encoding X509 certificate: %d: %s", err_code, ERR_error_string(err_code, err_buf));
			RETURN_CERT_VERIFY_FAILURE(SSL_R_CERTIFICATE_VERIFY_FAILED);
		}

		cert_ctx = CertCreateCertificateContext(X509_ASN_ENCODING, der_buf, der_len);
		OPENSSL_free(der_buf);

		if (cert_ctx == NULL) {
			char *err = php_win_err();
			php_error_docref(NULL, E_WARNING, "Error creating certificate context: %s", err);
			php_win_err_free(err);
			RETURN_CERT_VERIFY_FAILURE(SSL_R_CERTIFICATE_VERIFY_FAILED);
		}
	}

	{ /* Next fetch the relevant cert chain from the store */
		CERT_ENHKEY_USAGE enhkey_usage = {0};
		CERT_USAGE_MATCH cert_usage = {0};
		CERT_CHAIN_PARA chain_params = {sizeof(CERT_CHAIN_PARA)};
		LPSTR usages[] = {szOID_PKIX_KP_SERVER_AUTH, szOID_SERVER_GATED_CRYPTO, szOID_SGC_NETSCAPE};
		DWORD chain_flags = 0;
		unsigned long allowed_depth = OPENSSL_DEFAULT_STREAM_VERIFY_DEPTH;
		unsigned int i;

		enhkey_usage.cUsageIdentifier = 3;
		enhkey_usage.rgpszUsageIdentifier = usages;
		cert_usage.dwType = USAGE_MATCH_TYPE_OR;
		cert_usage.Usage = enhkey_usage;
		chain_params.RequestedUsage = cert_usage;
		chain_flags = CERT_CHAIN_CACHE_END_CERT | CERT_CHAIN_REVOCATION_CHECK_CHAIN_EXCLUDE_ROOT;

		if (!CertGetCertificateChain(NULL, cert_ctx, NULL, NULL, &chain_params, chain_flags, NULL, &cert_chain_ctx)) {
			char *err = php_win_err();
			php_error_docref(NULL, E_WARNING, "Error getting certificate chain: %s", err);
			php_win_err_free(err);
			CertFreeCertificateContext(cert_ctx);
			RETURN_CERT_VERIFY_FAILURE(SSL_R_CERTIFICATE_VERIFY_FAILED);
		}

		/* check if the cert is self-signed */
		if (cert_chain_ctx->cChain > 0 && cert_chain_ctx->rgpChain[0]->cElement > 0
			&& (cert_chain_ctx->rgpChain[0]->rgpElement[0]->TrustStatus.dwInfoStatus & CERT_TRUST_IS_SELF_SIGNED) != 0) {
			is_self_signed = 1;
		}

		/* check the depth */
		GET_VER_OPT_LONG("verify_depth", allowed_depth);

		for (i = 0; i < cert_chain_ctx->cChain; i++) {
			if (cert_chain_ctx->rgpChain[i]->cElement > allowed_depth) {
				CertFreeCertificateChain(cert_chain_ctx);
				CertFreeCertificateContext(cert_ctx);
				RETURN_CERT_VERIFY_FAILURE(X509_V_ERR_CERT_CHAIN_TOO_LONG);
			}
		}
	}

	{ /* Then verify it against a policy */
		SSL_EXTRA_CERT_CHAIN_POLICY_PARA ssl_policy_params = {sizeof(SSL_EXTRA_CERT_CHAIN_POLICY_PARA)};
		CERT_CHAIN_POLICY_PARA chain_policy_params = {sizeof(CERT_CHAIN_POLICY_PARA)};
		CERT_CHAIN_POLICY_STATUS chain_policy_status = {sizeof(CERT_CHAIN_POLICY_STATUS)};
		BOOL verify_result;

		ssl_policy_params.dwAuthType = (sslsock->is_client) ? AUTHTYPE_SERVER : AUTHTYPE_CLIENT;
		/* we validate the name ourselves using the peer_name
		   ctx option, so no need to use a server name here */
		ssl_policy_params.pwszServerName = NULL;
		chain_policy_params.pvExtraPolicyPara = &ssl_policy_params;

		verify_result = CertVerifyCertificateChainPolicy(CERT_CHAIN_POLICY_SSL, cert_chain_ctx, &chain_policy_params, &chain_policy_status);

		CertFreeCertificateChain(cert_chain_ctx);
		CertFreeCertificateContext(cert_ctx);

		if (!verify_result) {
			char *err = php_win_err();
			php_error_docref(NULL, E_WARNING, "Error verifying certificate chain policy: %s", err);
			php_win_err_free(err);
			RETURN_CERT_VERIFY_FAILURE(SSL_R_CERTIFICATE_VERIFY_FAILED);
		}

		if (chain_policy_status.dwError != 0) {
			/* The chain does not match the policy */
			if (is_self_signed && chain_policy_status.dwError == CERT_E_UNTRUSTEDROOT
				&& GET_VER_OPT("allow_self_signed") && zend_is_true(val)) {
				/* allow self-signed certs */
				X509_STORE_CTX_set_error(x509_store_ctx, X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT);
			} else {
				RETURN_CERT_VERIFY_FAILURE(SSL_R_CERTIFICATE_VERIFY_FAILED);
			}
		}
	}

	return 1;
}
/* }}} */
#endif

static long php_openssl_load_stream_cafile(X509_STORE *cert_store, const char *cafile) /* {{{ */
{
	php_stream *stream;
	X509 *cert;
	BIO *buffer;
	int buffer_active = 0;
	char *line = NULL;
	size_t line_len;
	long certs_added = 0;

	stream = php_stream_open_wrapper(cafile, "rb", 0, NULL);

	if (stream == NULL) {
		php_error(E_WARNING, "failed loading cafile stream: `%s'", cafile);
		return 0;
	} else if (stream->wrapper->is_url) {
		php_stream_close(stream);
		php_error(E_WARNING, "remote cafile streams are disabled for security purposes");
		return 0;
	}

	cert_start: {
		line = php_stream_get_line(stream, NULL, 0, &line_len);
		if (line == NULL) {
			goto stream_complete;
		} else if (!strcmp(line, "-----BEGIN CERTIFICATE-----\n") ||
			!strcmp(line, "-----BEGIN CERTIFICATE-----\r\n")
		) {
			buffer = BIO_new(BIO_s_mem());
			buffer_active = 1;
			goto cert_line;
		} else {
			efree(line);
			goto cert_start;
		}
	}

	cert_line: {
		BIO_puts(buffer, line);
		efree(line);
		line = php_stream_get_line(stream, NULL, 0, &line_len);
		if (line == NULL) {
			goto stream_complete;
		} else if (!strcmp(line, "-----END CERTIFICATE-----") ||
			!strcmp(line, "-----END CERTIFICATE-----\n") ||
			!strcmp(line, "-----END CERTIFICATE-----\r\n")
		) {
			goto add_cert;
		} else {
			goto cert_line;
		}
	}

	add_cert: {
		BIO_puts(buffer, line);
		efree(line);
		cert = PEM_read_bio_X509(buffer, NULL, 0, NULL);
		BIO_free(buffer);
		buffer_active = 0;
		if (cert && X509_STORE_add_cert(cert_store, cert)) {
			++certs_added;
			X509_free(cert);
		}
		goto cert_start;
	}

	stream_complete: {
		php_stream_close(stream);
		if (buffer_active == 1) {
			BIO_free(buffer);
		}
	}

	if (certs_added == 0) {
		php_error(E_WARNING, "no valid certs found cafile stream: `%s'", cafile);
	}

	return certs_added;
}
/* }}} */

static int php_openssl_enable_peer_verification(SSL_CTX *ctx, php_stream *stream) /* {{{ */
{
	zval *val = NULL;
	char *cafile = NULL;
	char *capath = NULL;
	php_openssl_netstream_data_t *sslsock = (php_openssl_netstream_data_t*)stream->abstract;

	GET_VER_OPT_STRING("cafile", cafile);
	GET_VER_OPT_STRING("capath", capath);

	if (cafile == NULL) {
		cafile = zend_ini_string("openssl.cafile", sizeof("openssl.cafile")-1, 0);
		cafile = strlen(cafile) ? cafile : NULL;
	} else if (!sslsock->is_client) {
		/* Servers need to load and assign CA names from the cafile */
		STACK_OF(X509_NAME) *cert_names = SSL_load_client_CA_file(cafile);
		if (cert_names != NULL) {
			SSL_CTX_set_client_CA_list(ctx, cert_names);
		} else {
			php_error(E_WARNING, "SSL: failed loading CA names from cafile");
			return FAILURE;
		}
	}

	if (capath == NULL) {
		capath = zend_ini_string("openssl.capath", sizeof("openssl.capath")-1, 0);
		capath = strlen(capath) ? capath : NULL;
	}

	if (cafile || capath) {
		if (!SSL_CTX_load_verify_locations(ctx, cafile, capath)) {
			ERR_clear_error();
			if (cafile && !php_openssl_load_stream_cafile(SSL_CTX_get_cert_store(ctx), cafile)) {
				return FAILURE;
			}
		}
	} else {
#ifdef PHP_WIN32
		SSL_CTX_set_cert_verify_callback(ctx, php_openssl_win_cert_verify_callback, (void *)stream);
#else
		if (sslsock->is_client && !SSL_CTX_set_default_verify_paths(ctx)) {
			php_error_docref(NULL, E_WARNING,
				"Unable to set default verify locations and no CA settings specified");
			return FAILURE;
		}
#endif
	}

	SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, verify_callback);

	return SUCCESS;
}
/* }}} */

static void php_openssl_disable_peer_verification(SSL_CTX *ctx, php_stream *stream) /* {{{ */
{
	SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, NULL);
}
/* }}} */

static int php_openssl_set_local_cert(SSL_CTX *ctx, php_stream *stream) /* {{{ */
{
	zval *val = NULL;
	char *certfile = NULL;
	size_t certfile_len;

	GET_VER_OPT_STRINGL("local_cert", certfile, certfile_len);

	if (certfile) {
		char resolved_path_buff[MAXPATHLEN];
		const char *private_key = NULL;
		size_t private_key_len;

		if (!php_openssl_check_path_ex(
				certfile, certfile_len, resolved_path_buff, 0, false, false,
				"local_cert in ssl stream context")) {
			php_error_docref(NULL, E_WARNING, "Unable to get real path of certificate file `%s'", certfile);
			return FAILURE;
		}
		/* a certificate to use for authentication */
		if (SSL_CTX_use_certificate_chain_file(ctx, resolved_path_buff) != 1) {
			php_error_docref(NULL, E_WARNING,
				"Unable to set local cert chain file `%s'; Check that your cafile/capath "
				"settings include details of your certificate and its issuer",
				certfile);
			return FAILURE;
		}

		GET_VER_OPT_STRINGL("local_pk", private_key, private_key_len);
		if (private_key && !php_openssl_check_path_ex(
				private_key, private_key_len, resolved_path_buff, 0, false, false,
				"local_pk in ssl stream context")) {
			php_error_docref(NULL, E_WARNING, "Unable to get real path of private key file `%s'", private_key);
			return FAILURE;
		}
		if (SSL_CTX_use_PrivateKey_file(ctx, resolved_path_buff, SSL_FILETYPE_PEM) != 1) {
			php_error_docref(NULL, E_WARNING, "Unable to set private key file `%s'", resolved_path_buff);
			return FAILURE;
		}
		if (!SSL_CTX_check_private_key(ctx)) {
			php_error_docref(NULL, E_WARNING, "Private key does not match certificate!");
		}
	}

	return SUCCESS;
}
/* }}} */

#if PHP_OPENSSL_API_VERSION < 0x10100
static int php_openssl_get_crypto_method_ctx_flags(int method_flags) /* {{{ */
{
	int ssl_ctx_options = SSL_OP_ALL;

#ifdef SSL_OP_NO_SSLv2
	ssl_ctx_options |= SSL_OP_NO_SSLv2;
#endif
#ifdef HAVE_SSL3
	if (!(method_flags & STREAM_CRYPTO_METHOD_SSLv3)) {
		ssl_ctx_options |= SSL_OP_NO_SSLv3;
	}
#endif
#ifdef HAVE_TLS1
	if (!(method_flags & STREAM_CRYPTO_METHOD_TLSv1_0)) {
		ssl_ctx_options |= SSL_OP_NO_TLSv1;
	}
#endif
#ifdef HAVE_TLS11
	if (!(method_flags & STREAM_CRYPTO_METHOD_TLSv1_1)) {
		ssl_ctx_options |= SSL_OP_NO_TLSv1_1;
	}
#endif
#ifdef HAVE_TLS12
	if (!(method_flags & STREAM_CRYPTO_METHOD_TLSv1_2)) {
		ssl_ctx_options |= SSL_OP_NO_TLSv1_2;
	}
#endif
#ifdef HAVE_TLS13
	if (!(method_flags & STREAM_CRYPTO_METHOD_TLSv1_3)) {
		ssl_ctx_options |= SSL_OP_NO_TLSv1_3;
	}
#endif

	return ssl_ctx_options;
}
/* }}} */
#endif

static inline int php_openssl_get_min_proto_version_flag(int flags) /* {{{ */
{
	int ver;
	for (ver = PHP_OPENSSL_MIN_PROTO_VERSION; ver <= PHP_OPENSSL_MAX_PROTO_VERSION; ver <<= 1) {
		if (flags & ver) {
			return ver;
		}
	}
	return PHP_OPENSSL_MAX_PROTO_VERSION;
}
/* }}} */

static inline int php_openssl_get_max_proto_version_flag(int flags) /* {{{ */
{
	int ver;
	for (ver = PHP_OPENSSL_MAX_PROTO_VERSION; ver >= PHP_OPENSSL_MIN_PROTO_VERSION; ver >>= 1) {
		if (flags & ver) {
			return ver;
		}
	}
	return STREAM_CRYPTO_METHOD_TLSv1_3;
}
/* }}} */

#if PHP_OPENSSL_API_VERSION >= 0x10100
static inline int php_openssl_map_proto_version(int flag) /* {{{ */
{
	switch (flag) {
#ifdef HAVE_TLS13
		case STREAM_CRYPTO_METHOD_TLSv1_3:
			return TLS1_3_VERSION;
#endif
		case STREAM_CRYPTO_METHOD_TLSv1_2:
			return TLS1_2_VERSION;
		case STREAM_CRYPTO_METHOD_TLSv1_1:
			return TLS1_1_VERSION;
		case STREAM_CRYPTO_METHOD_TLSv1_0:
			return TLS1_VERSION;
#ifdef HAVE_SSL3
		case STREAM_CRYPTO_METHOD_SSLv3:
			return SSL3_VERSION;
#endif
		default:
			return TLS1_2_VERSION;
	}
}
/* }}} */

static int php_openssl_get_min_proto_version(int flags) /* {{{ */
{
	return php_openssl_map_proto_version(php_openssl_get_min_proto_version_flag(flags));
}
/* }}} */

static int php_openssl_get_max_proto_version(int flags) /* {{{ */
{
	return php_openssl_map_proto_version(php_openssl_get_max_proto_version_flag(flags));
}
/* }}} */
#endif

static int php_openssl_get_proto_version_flags(int flags, int min, int max) /* {{{ */
{
	int ver;

	if (!min) {
		min = php_openssl_get_min_proto_version_flag(flags);
	}
	if (!max) {
		max = php_openssl_get_max_proto_version_flag(flags);
	}

	for (ver = PHP_OPENSSL_MIN_PROTO_VERSION; ver <= PHP_OPENSSL_MAX_PROTO_VERSION; ver <<= 1) {
		if (ver >= min && ver <= max) {
			if (!(flags & ver)) {
				flags |= ver;
			}
		} else if (flags & ver) {
			flags &= ~ver;
		}
	}

	return flags;
}
/* }}} */

static void php_openssl_limit_handshake_reneg(const SSL *ssl) /* {{{ */
{
	php_stream *stream;
	php_openssl_netstream_data_t *sslsock;
	struct timeval now;
	zend_long elapsed_time;

	stream = php_openssl_get_stream_from_ssl_handle(ssl);
	sslsock = (php_openssl_netstream_data_t*)stream->abstract;
	gettimeofday(&now, NULL);

	/* The initial handshake is never rate-limited */
	if (sslsock->reneg->prev_handshake == 0) {
		sslsock->reneg->prev_handshake = now.tv_sec;
		return;
	}

	elapsed_time = (now.tv_sec - sslsock->reneg->prev_handshake);
	sslsock->reneg->prev_handshake = now.tv_sec;
	sslsock->reneg->tokens -= (elapsed_time * (sslsock->reneg->limit / sslsock->reneg->window));

	if (sslsock->reneg->tokens < 0) {
		sslsock->reneg->tokens = 0;
	}
	++sslsock->reneg->tokens;

	/* The token level exceeds our allowed limit */
	if (sslsock->reneg->tokens > sslsock->reneg->limit) {
		zval *val;


		sslsock->reneg->should_close = 1;

		if (PHP_STREAM_CONTEXT(stream) && (val = php_stream_context_get_option(PHP_STREAM_CONTEXT(stream),
				"ssl", "reneg_limit_callback")) != NULL
		) {
			zval param, retval;

			php_stream_to_zval(stream, &param);

			/* Closing the stream inside this callback would segfault! */
			stream->flags |= PHP_STREAM_FLAG_NO_FCLOSE;
			if (FAILURE == call_user_function(NULL, NULL, val, &retval, 1, &param)) {
				php_error(E_WARNING, "SSL: failed invoking reneg limit notification callback");
			}
			stream->flags ^= PHP_STREAM_FLAG_NO_FCLOSE;

			/* If the reneg_limit_callback returned true don't auto-close */
			if (Z_TYPE(retval) == IS_TRUE) {
				sslsock->reneg->should_close = 0;
			}

			zval_ptr_dtor(&retval);
		} else {
			php_error_docref(NULL, E_WARNING,
				"SSL: client-initiated handshake rate limit exceeded by peer");
		}
	}
}
/* }}} */

static void php_openssl_info_callback(const SSL *ssl, int where, int ret) /* {{{ */
{
	/* Rate-limit client-initiated handshake renegotiation to prevent DoS */
	if (where & SSL_CB_HANDSHAKE_START) {
		php_openssl_limit_handshake_reneg(ssl);
	}
}
/* }}} */

static void php_openssl_init_server_reneg_limit(php_stream *stream, php_openssl_netstream_data_t *sslsock) /* {{{ */
{
	zval *val;
	zend_long limit = OPENSSL_DEFAULT_RENEG_LIMIT;
	zend_long window = OPENSSL_DEFAULT_RENEG_WINDOW;

	if (PHP_STREAM_CONTEXT(stream) &&
		NULL != (val = php_stream_context_get_option(PHP_STREAM_CONTEXT(stream), "ssl", "reneg_limit"))
	) {
		limit = zval_get_long(val);
	}

	/* No renegotiation rate-limiting */
	if (limit < 0) {
		return;
	}

	if (PHP_STREAM_CONTEXT(stream) &&
		NULL != (val = php_stream_context_get_option(PHP_STREAM_CONTEXT(stream), "ssl", "reneg_window"))
	) {
		window = zval_get_long(val);
	}

	sslsock->reneg = (void*)pemalloc(sizeof(php_openssl_handshake_bucket_t),
		php_stream_is_persistent(stream)
	);

	sslsock->reneg->limit = limit;
	sslsock->reneg->window = window;
	sslsock->reneg->prev_handshake = 0;
	sslsock->reneg->tokens = 0;
	sslsock->reneg->should_close = 0;

	SSL_set_info_callback(sslsock->ssl_handle, php_openssl_info_callback);
}
/* }}} */

#if PHP_OPENSSL_API_VERSION < 0x10100
static RSA *php_openssl_tmp_rsa_cb(SSL *s, int is_export, int keylength)
{
	BIGNUM *bn = NULL;
	static RSA *rsa_tmp = NULL;

	if (!rsa_tmp && ((bn = BN_new()) == NULL)) {
		php_error_docref(NULL, E_WARNING, "allocation error generating RSA key");
	}
	if (!rsa_tmp && bn) {
		if (!BN_set_word(bn, RSA_F4) || ((rsa_tmp = RSA_new()) == NULL) ||
			!RSA_generate_key_ex(rsa_tmp, keylength, bn, NULL)) {
			if (rsa_tmp) {
				RSA_free(rsa_tmp);
			}
			rsa_tmp = NULL;
		}
		BN_free(bn);
	}

	return (rsa_tmp);
}
#endif

static int php_openssl_set_server_dh_param(php_stream * stream, SSL_CTX *ctx) /* {{{ */
{
	zval *zdhpath = php_stream_context_get_option(PHP_STREAM_CONTEXT(stream), "ssl", "dh_param");
	if (zdhpath == NULL) {
#if 0
	/* Coming in OpenSSL 1.1 ... eventually we'll want to enable this
	 * in the absence of an explicit dh_param.
	 */
	SSL_CTX_set_dh_auto(ctx, 1);
#endif
		return SUCCESS;
	}

	if (!try_convert_to_string(zdhpath)) {
		return FAILURE;
	}

	BIO *bio = BIO_new_file(Z_STRVAL_P(zdhpath), PHP_OPENSSL_BIO_MODE_R(PKCS7_BINARY));

	if (bio == NULL) {
		php_error_docref(NULL, E_WARNING, "Invalid dh_param");
		return FAILURE;
	}

#if PHP_OPENSSL_API_VERSION >= 0x30000
	EVP_PKEY *pkey = PEM_read_bio_Parameters(bio, NULL);
	BIO_free(bio);

	if (pkey == NULL) {
		php_error_docref(NULL, E_WARNING, "Failed reading DH params");
		return FAILURE;
	}

	if (SSL_CTX_set0_tmp_dh_pkey(ctx, pkey) == 0) {
		php_error_docref(NULL, E_WARNING, "Failed assigning DH params");
		EVP_PKEY_free(pkey);
		return FAILURE;
	}
#else
	DH *dh = PEM_read_bio_DHparams(bio, NULL, NULL, NULL);
	BIO_free(bio);

	if (dh == NULL) {
		php_error_docref(NULL, E_WARNING, "Failed reading DH params");
		return FAILURE;
	}

	if (SSL_CTX_set_tmp_dh(ctx, dh) == 0) {
		php_error_docref(NULL, E_WARNING, "Failed assigning DH params");
		DH_free(dh);
		return FAILURE;
	}

	DH_free(dh);
#endif

	return SUCCESS;
}
/* }}} */

#if defined(HAVE_ECDH) && PHP_OPENSSL_API_VERSION < 0x10100
static int php_openssl_set_server_ecdh_curve(php_stream *stream, SSL_CTX *ctx) /* {{{ */
{
	zval *zvcurve;
	int curve_nid;
	EC_KEY *ecdh;

	zvcurve = php_stream_context_get_option(PHP_STREAM_CONTEXT(stream), "ssl", "ecdh_curve");
	if (zvcurve == NULL) {
		SSL_CTX_set_ecdh_auto(ctx, 1);
		return SUCCESS;
	} else {
		if (!try_convert_to_string(zvcurve)) {
			return FAILURE;
		}

		curve_nid = OBJ_sn2nid(Z_STRVAL_P(zvcurve));
		if (curve_nid == NID_undef) {
			php_error_docref(NULL, E_WARNING, "Invalid ecdh_curve specified");
			return FAILURE;
		}
	}

	ecdh = EC_KEY_new_by_curve_name(curve_nid);
	if (ecdh == NULL) {
		php_error_docref(NULL, E_WARNING, "Failed generating ECDH curve");
		return FAILURE;
	}

	SSL_CTX_set_tmp_ecdh(ctx, ecdh);
	EC_KEY_free(ecdh);

	return SUCCESS;
}
/* }}} */
#endif

static int php_openssl_set_server_specific_opts(php_stream *stream, SSL_CTX *ctx) /* {{{ */
{
	zval *zv;
	long ssl_ctx_options = SSL_CTX_get_options(ctx);

#if defined(HAVE_ECDH) && PHP_OPENSSL_API_VERSION < 0x10100
	if (php_openssl_set_server_ecdh_curve(stream, ctx) == FAILURE) {
		return FAILURE;
	}
#endif

#if PHP_OPENSSL_API_VERSION < 0x10100
	SSL_CTX_set_tmp_rsa_callback(ctx, php_openssl_tmp_rsa_cb);
#endif
	/* We now use php_openssl_tmp_rsa_cb to generate a key of appropriate size whenever necessary */
	if (php_stream_context_get_option(PHP_STREAM_CONTEXT(stream), "ssl", "rsa_key_size") != NULL) {
		php_error_docref(NULL, E_WARNING, "rsa_key_size context option has been removed");
	}

	if (php_openssl_set_server_dh_param(stream, ctx) == FAILURE) {
		return FAILURE;
	}

	zv = php_stream_context_get_option(PHP_STREAM_CONTEXT(stream), "ssl", "single_dh_use");
	if (zv == NULL || zend_is_true(zv)) {
		ssl_ctx_options |= SSL_OP_SINGLE_DH_USE;
	}

	zv = php_stream_context_get_option(PHP_STREAM_CONTEXT(stream), "ssl", "honor_cipher_order");
	if (zv == NULL || zend_is_true(zv)) {
		ssl_ctx_options |= SSL_OP_CIPHER_SERVER_PREFERENCE;
	}

	SSL_CTX_set_options(ctx, ssl_ctx_options);

	return SUCCESS;
}
/* }}} */

#ifdef HAVE_TLS_SNI
static int php_openssl_server_sni_callback(SSL *ssl_handle, int *al, void *arg) /* {{{ */
{
	php_stream *stream;
	php_openssl_netstream_data_t *sslsock;
	unsigned i;
	const char *server_name;

	server_name = SSL_get_servername(ssl_handle, TLSEXT_NAMETYPE_host_name);

	if (!server_name) {
		return SSL_TLSEXT_ERR_NOACK;
	}

	stream = (php_stream*)SSL_get_ex_data(ssl_handle, php_openssl_get_ssl_stream_data_index());
	sslsock = (php_openssl_netstream_data_t*)stream->abstract;

	if (!(sslsock->sni_cert_count && sslsock->sni_certs)) {
		return SSL_TLSEXT_ERR_NOACK;
	}

	for (i=0; i < sslsock->sni_cert_count; i++) {
		if (php_openssl_matches_wildcard_name(server_name, sslsock->sni_certs[i].name)) {
			SSL_set_SSL_CTX(ssl_handle, sslsock->sni_certs[i].ctx);
			return SSL_TLSEXT_ERR_OK;
		}
	}

	return SSL_TLSEXT_ERR_NOACK;
}
/* }}} */

static SSL_CTX *php_openssl_create_sni_server_ctx(char *cert_path, char *key_path)  /* {{{ */
{
	/* The hello method is not inherited by SSL structs when assigning a new context
	 * inside the SNI callback, so the just use SSLv23 */
	SSL_CTX *ctx = SSL_CTX_new(SSLv23_server_method());

	if (SSL_CTX_use_certificate_chain_file(ctx, cert_path) != 1) {
		php_error_docref(NULL, E_WARNING,
			"Failed setting local cert chain file `%s'; " \
			"check that your cafile/capath settings include " \
			"details of your certificate and its issuer",
			cert_path
		);
		SSL_CTX_free(ctx);
		return NULL;
	} else if (SSL_CTX_use_PrivateKey_file(ctx, key_path, SSL_FILETYPE_PEM) != 1) {
		php_error_docref(NULL, E_WARNING,
			"Failed setting private key from file `%s'",
			key_path
		);
		SSL_CTX_free(ctx);
		return NULL;
	}

	return ctx;
}
/* }}} */

static int php_openssl_enable_server_sni(php_stream *stream, php_openssl_netstream_data_t *sslsock)  /* {{{ */
{
	zval *val;
	zval *current;
	zend_string *key;
	zend_ulong key_index;
	int i = 0;
	char resolved_path_buff[MAXPATHLEN];
	SSL_CTX *ctx;

	/* If the stream ctx disables SNI we're finished here */
	if (GET_VER_OPT("SNI_enabled") && !zend_is_true(val)) {
		return SUCCESS;
	}

	/* If no SNI cert array is specified we're finished here */
	if (!GET_VER_OPT("SNI_server_certs")) {
		return SUCCESS;
	}

	if (Z_TYPE_P(val) != IS_ARRAY) {
		php_error_docref(NULL, E_WARNING,
			"SNI_server_certs requires an array mapping host names to cert paths"
		);
		return FAILURE;
	}

	sslsock->sni_cert_count = zend_hash_num_elements(Z_ARRVAL_P(val));
	if (sslsock->sni_cert_count == 0) {
		php_error_docref(NULL, E_WARNING,
			"SNI_server_certs host cert array must not be empty"
		);
		return FAILURE;
	}

	sslsock->sni_certs = (php_openssl_sni_cert_t*)safe_pemalloc(sslsock->sni_cert_count,
		sizeof(php_openssl_sni_cert_t), 0, php_stream_is_persistent(stream)
	);
	memset(sslsock->sni_certs, 0, sslsock->sni_cert_count * sizeof(php_openssl_sni_cert_t));

	ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(val), key_index, key, current) {
		(void) key_index;

		if (!key) {
			php_error_docref(NULL, E_WARNING,
				"SNI_server_certs array requires string host name keys"
			);
			return FAILURE;
		}

		if (Z_TYPE_P(current) == IS_ARRAY) {
			zval *local_pk, *local_cert;
			zend_string *local_pk_str, *local_cert_str;
			char resolved_cert_path_buff[MAXPATHLEN], resolved_pk_path_buff[MAXPATHLEN];

			local_cert = zend_hash_str_find(Z_ARRVAL_P(current), "local_cert", sizeof("local_cert")-1);
			if (local_cert == NULL) {
				php_error_docref(NULL, E_WARNING,
					"local_cert not present in the array"
				);
				return FAILURE;
			}

			local_cert_str = zval_try_get_string(local_cert);
			if (UNEXPECTED(!local_cert_str)) {
				return FAILURE;
			}
			if (!php_openssl_check_path_str_ex(
					local_cert_str, resolved_cert_path_buff, 0, false, false,
					"SNI_server_certs local_cert in ssl stream context")) {
				php_error_docref(NULL, E_WARNING,
					"Failed setting local cert chain file `%s'; could not open file",
					ZSTR_VAL(local_cert_str)
				);
				zend_string_release(local_cert_str);
				return FAILURE;
			}
			zend_string_release(local_cert_str);

			local_pk = zend_hash_str_find(Z_ARRVAL_P(current), "local_pk", sizeof("local_pk")-1);
			if (local_pk == NULL) {
				php_error_docref(NULL, E_WARNING,
					"local_pk not present in the array"
				);
				return FAILURE;
			}

			local_pk_str = zval_try_get_string(local_pk);
			if (UNEXPECTED(!local_pk_str)) {
				return FAILURE;
			}
			if (!php_openssl_check_path_str_ex(
					local_pk_str, resolved_pk_path_buff, 0, false, false,
					"SNI_server_certs local_pk in ssl stream context")) {
				php_error_docref(NULL, E_WARNING,
					"Failed setting local private key file `%s';  could not open file",
					ZSTR_VAL(local_pk_str)
				);
				zend_string_release(local_pk_str);
				return FAILURE;
			}
			zend_string_release(local_pk_str);

			ctx = php_openssl_create_sni_server_ctx(resolved_cert_path_buff, resolved_pk_path_buff);

		} else if (php_openssl_check_path_str_ex(
				Z_STR_P(current), resolved_path_buff, 0, false, false,
				"SNI_server_certs in ssl stream context")) {
			ctx = php_openssl_create_sni_server_ctx(resolved_path_buff, resolved_path_buff);
		} else {
			php_error_docref(NULL, E_WARNING,
				"Failed setting local cert chain file `%s'; file not found",
				Z_STRVAL_P(current)
			);
			return FAILURE;
		}

		if (ctx == NULL) {
			return FAILURE;
		}

		sslsock->sni_certs[i].name = pestrdup(ZSTR_VAL(key), php_stream_is_persistent(stream));
		sslsock->sni_certs[i].ctx = ctx;
		++i;

	} ZEND_HASH_FOREACH_END();

	SSL_CTX_set_tlsext_servername_callback(sslsock->ctx, php_openssl_server_sni_callback);

	return SUCCESS;
}
/* }}} */

static void php_openssl_enable_client_sni(php_stream *stream, php_openssl_netstream_data_t *sslsock) /* {{{ */
{
	zval *val;
	char *sni_server_name;

	/* If SNI is explicitly disabled we're finished here */
	if (GET_VER_OPT("SNI_enabled") && !zend_is_true(val)) {
		return;
	}

	sni_server_name = sslsock->url_name;

	GET_VER_OPT_STRING("peer_name", sni_server_name);

	if (sni_server_name) {
		SSL_set_tlsext_host_name(sslsock->ssl_handle, sni_server_name);
	}
}
/* }}} */
#endif

#ifdef HAVE_TLS_ALPN
/**
 * Parses a comma-separated list of strings into a string suitable for SSL_CTX_set_next_protos_advertised
 *   outlen: (output) set to the length of the resulting buffer on success.
 *   err: (maybe NULL) on failure, an error message line is written to this BIO.
 *   in: a NULL terminated string like "abc,def,ghi"
 *
 *   returns: an emalloced buffer or NULL on failure.
 */
static unsigned char *php_openssl_alpn_protos_parse(unsigned short *outlen, const char *in) /* {{{ */
{
	size_t len;
	unsigned char *out;
	size_t i, start = 0;

	len = strlen(in);
	if (len >= 65535) {
		return NULL;
	}

	out = emalloc(strlen(in) + 1);

	for (i = 0; i <= len; ++i) {
		if (i == len || in[i] == ',') {
			if (i - start > 255) {
				efree(out);
				return NULL;
			}
			out[start] = i - start;
			start = i + 1;
		} else {
			out[i + 1] = in[i];
		}
	}

	*outlen = len + 1;

	return out;
}
/* }}} */

static int php_openssl_server_alpn_callback(SSL *ssl_handle,
		const unsigned char **out, unsigned char *outlen,
		const unsigned char *in, unsigned int inlen, void *arg) /* {{{ */
{
	php_openssl_netstream_data_t *sslsock = arg;

	if (SSL_select_next_proto((unsigned char **)out, outlen, sslsock->alpn_ctx.data, sslsock->alpn_ctx.len, in, inlen) != OPENSSL_NPN_NEGOTIATED) {
		return SSL_TLSEXT_ERR_NOACK;
	}

	return SSL_TLSEXT_ERR_OK;
}
/* }}} */

#endif

int php_openssl_setup_crypto(php_stream *stream,
		php_openssl_netstream_data_t *sslsock,
		php_stream_xport_crypto_param *cparam) /* {{{ */
{
	const SSL_METHOD *method;
	int ssl_ctx_options;
	int method_flags;
	zend_long min_version = 0;
	zend_long max_version = 0;
	char *cipherlist = NULL;
	char *alpn_protocols = NULL;
	zval *val;

	if (sslsock->ssl_handle) {
		if (sslsock->s.is_blocked) {
			php_error_docref(NULL, E_WARNING, "SSL/TLS already set-up for this stream");
			return FAILURE;
		} else {
			return SUCCESS;
		}
	}

	ERR_clear_error();

	/* We need to do slightly different things based on client/server method
	 * so lets remember which method was selected */
	sslsock->is_client = cparam->inputs.method & STREAM_CRYPTO_IS_CLIENT;
	method_flags = cparam->inputs.method & ~STREAM_CRYPTO_IS_CLIENT;

	method = sslsock->is_client ? SSLv23_client_method() : SSLv23_server_method();
	sslsock->ctx = SSL_CTX_new(method);

	if (sslsock->ctx == NULL) {
		php_error_docref(NULL, E_WARNING, "SSL context creation failure");
		return FAILURE;
	}

	GET_VER_OPT_LONG("min_proto_version", min_version);
	GET_VER_OPT_LONG("max_proto_version", max_version);
	method_flags = php_openssl_get_proto_version_flags(method_flags, min_version, max_version);
#if PHP_OPENSSL_API_VERSION < 0x10100
	ssl_ctx_options = php_openssl_get_crypto_method_ctx_flags(method_flags);
#else
	ssl_ctx_options = SSL_OP_ALL;
#endif

	if (GET_VER_OPT("no_ticket") && zend_is_true(val)) {
		ssl_ctx_options |= SSL_OP_NO_TICKET;
	}

	ssl_ctx_options &= ~SSL_OP_DONT_INSERT_EMPTY_FRAGMENTS;

#ifdef SSL_OP_IGNORE_UNEXPECTED_EOF
	/* Only for OpenSSL 3+ to keep OpenSSL 1.1.1 behavior */
	ssl_ctx_options |= SSL_OP_IGNORE_UNEXPECTED_EOF;
#endif

	if (!GET_VER_OPT("disable_compression") || zend_is_true(val)) {
		ssl_ctx_options |= SSL_OP_NO_COMPRESSION;
	}

	if (GET_VER_OPT("verify_peer") && !zend_is_true(val)) {
		php_openssl_disable_peer_verification(sslsock->ctx, stream);
	} else if (FAILURE == php_openssl_enable_peer_verification(sslsock->ctx, stream)) {
		return FAILURE;
	}

	/* callback for the passphrase (for localcert) */
	if (GET_VER_OPT("passphrase")) {
		SSL_CTX_set_default_passwd_cb_userdata(sslsock->ctx, stream);
		SSL_CTX_set_default_passwd_cb(sslsock->ctx, php_openssl_passwd_callback);
	}

	GET_VER_OPT_STRING("ciphers", cipherlist);
#ifndef USE_OPENSSL_SYSTEM_CIPHERS
	if (!cipherlist) {
		cipherlist = OPENSSL_DEFAULT_STREAM_CIPHERS;
	}
#endif
	if (cipherlist) {
		if (SSL_CTX_set_cipher_list(sslsock->ctx, cipherlist) != 1) {
			return FAILURE;
		}
	}

	if (GET_VER_OPT("security_level")) {
		zend_long lval = zval_get_long(val);
		if (lval < 0 || lval > 5) {
			php_error_docref(NULL, E_WARNING, "Security level must be between 0 and 5");
		}
#ifdef HAVE_SEC_LEVEL
		SSL_CTX_set_security_level(sslsock->ctx, lval);
#endif
	}

	GET_VER_OPT_STRING("alpn_protocols", alpn_protocols);
	if (alpn_protocols) {
#ifdef HAVE_TLS_ALPN
		{
			unsigned short alpn_len;
			unsigned char *alpn = php_openssl_alpn_protos_parse(&alpn_len, alpn_protocols);

			if (alpn == NULL) {
				php_error_docref(NULL, E_WARNING, "Failed parsing comma-separated TLS ALPN protocol string");
				SSL_CTX_free(sslsock->ctx);
				sslsock->ctx = NULL;
				return FAILURE;
			}
			if (sslsock->is_client) {
				SSL_CTX_set_alpn_protos(sslsock->ctx, alpn, alpn_len);
			} else {
				sslsock->alpn_ctx.data = (unsigned char *) pestrndup((const char*)alpn, alpn_len, php_stream_is_persistent(stream));
				sslsock->alpn_ctx.len = alpn_len;
				SSL_CTX_set_alpn_select_cb(sslsock->ctx, php_openssl_server_alpn_callback, sslsock);
			}

			efree(alpn);
		}
#else
		php_error_docref(NULL, E_WARNING,
			"alpn_protocols support is not compiled into the OpenSSL library against which PHP is linked");
#endif
	}

	if (FAILURE == php_openssl_set_local_cert(sslsock->ctx, stream)) {
		return FAILURE;
	}

	SSL_CTX_set_options(sslsock->ctx, ssl_ctx_options);

#if PHP_OPENSSL_API_VERSION >= 0x10100
	SSL_CTX_set_min_proto_version(sslsock->ctx, php_openssl_get_min_proto_version(method_flags));
	SSL_CTX_set_max_proto_version(sslsock->ctx, php_openssl_get_max_proto_version(method_flags));
#endif

	if (sslsock->is_client == 0 &&
		PHP_STREAM_CONTEXT(stream) &&
		FAILURE == php_openssl_set_server_specific_opts(stream, sslsock->ctx)
	) {
		return FAILURE;
	}

	sslsock->ssl_handle = SSL_new(sslsock->ctx);

	if (sslsock->ssl_handle == NULL) {
		php_error_docref(NULL, E_WARNING, "SSL handle creation failure");
		SSL_CTX_free(sslsock->ctx);
		sslsock->ctx = NULL;
#ifdef HAVE_TLS_ALPN
		if (sslsock->alpn_ctx.data) {
			pefree(sslsock->alpn_ctx.data, php_stream_is_persistent(stream));
			sslsock->alpn_ctx.data = NULL;
		}
#endif
		return FAILURE;
	} else {
		SSL_set_ex_data(sslsock->ssl_handle, php_openssl_get_ssl_stream_data_index(), stream);
	}

	if (!SSL_set_fd(sslsock->ssl_handle, sslsock->s.socket)) {
		php_openssl_handle_ssl_error(stream, 0, 1);
	}

#ifdef HAVE_TLS_SNI
	/* Enable server-side SNI */
	if (!sslsock->is_client && php_openssl_enable_server_sni(stream, sslsock) == FAILURE) {
		return FAILURE;
	}
#endif

	/* Enable server-side handshake renegotiation rate-limiting */
	if (!sslsock->is_client) {
		php_openssl_init_server_reneg_limit(stream, sslsock);
	}

#ifdef SSL_MODE_RELEASE_BUFFERS
	SSL_set_mode(sslsock->ssl_handle, SSL_MODE_RELEASE_BUFFERS);
#endif

	if (cparam->inputs.session) {
		if (cparam->inputs.session->ops != &php_openssl_socket_ops) {
			php_error_docref(NULL, E_WARNING, "Supplied session stream must be an SSL enabled stream");
		} else if (((php_openssl_netstream_data_t*)cparam->inputs.session->abstract)->ssl_handle == NULL) {
			php_error_docref(NULL, E_WARNING, "Supplied SSL session stream is not initialized");
		} else {
			SSL_copy_session_id(sslsock->ssl_handle, ((php_openssl_netstream_data_t*)cparam->inputs.session->abstract)->ssl_handle);
		}
	}

	return SUCCESS;
}
/* }}} */

static int php_openssl_capture_peer_certs(php_stream *stream,
		php_openssl_netstream_data_t *sslsock, X509 *peer_cert) /* {{{ */
{
	zval *val, zcert;
	php_openssl_certificate_object *cert_object;
	int cert_captured = 0;

	if (NULL != (val = php_stream_context_get_option(PHP_STREAM_CONTEXT(stream),
			"ssl", "capture_peer_cert")) &&
		zend_is_true(val)
	) {
		object_init_ex(&zcert, php_openssl_certificate_ce);
		cert_object = Z_OPENSSL_CERTIFICATE_P(&zcert);
		cert_object->x509 = peer_cert;

		php_stream_context_set_option(PHP_STREAM_CONTEXT(stream), "ssl", "peer_certificate", &zcert);
		zval_ptr_dtor(&zcert);
		cert_captured = 1;
	}

	if (NULL != (val = php_stream_context_get_option(PHP_STREAM_CONTEXT(stream),
			"ssl", "capture_peer_cert_chain")) &&
		zend_is_true(val)
	) {
		zval arr;
		STACK_OF(X509) *chain;

		chain = SSL_get_peer_cert_chain(sslsock->ssl_handle);

		if (chain && sk_X509_num(chain) > 0) {
			int i;
			array_init(&arr);

			for (i = 0; i < sk_X509_num(chain); i++) {
				X509 *mycert = X509_dup(sk_X509_value(chain, i));

				object_init_ex(&zcert, php_openssl_certificate_ce);
				cert_object = Z_OPENSSL_CERTIFICATE_P(&zcert);
				cert_object->x509 = mycert;
				add_next_index_zval(&arr, &zcert);
			}

		} else {
			ZVAL_NULL(&arr);
		}

		php_stream_context_set_option(PHP_STREAM_CONTEXT(stream), "ssl", "peer_certificate_chain", &arr);
		zval_ptr_dtor(&arr);
	}

	return cert_captured;
}
/* }}} */

static int php_openssl_enable_crypto(php_stream *stream,
		php_openssl_netstream_data_t *sslsock,
		php_stream_xport_crypto_param *cparam) /* {{{ */
{
	int n;
	int retry = 1;
	int cert_captured = 0;
	X509 *peer_cert;

	if (cparam->inputs.activate && !sslsock->ssl_active) {
		struct timeval start_time, *timeout;
		int	blocked = sslsock->s.is_blocked, has_timeout = 0;

#ifdef HAVE_TLS_SNI
		if (sslsock->is_client) {
			php_openssl_enable_client_sni(stream, sslsock);
		}
#endif

		if (!sslsock->state_set) {
			if (sslsock->is_client) {
				SSL_set_connect_state(sslsock->ssl_handle);
			} else {
				SSL_set_accept_state(sslsock->ssl_handle);
			}
			sslsock->state_set = 1;
		}

		if (SUCCESS == php_set_sock_blocking(sslsock->s.socket, 0)) {
			sslsock->s.is_blocked = 0;
			/* The following mode are added only if we are able to change socket
			 * to non blocking mode which is also used for read and write */
			SSL_set_mode(sslsock->ssl_handle, SSL_MODE_ENABLE_PARTIAL_WRITE | SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER);
		}

		timeout = sslsock->is_client ? &sslsock->connect_timeout : &sslsock->s.timeout;
		has_timeout = !sslsock->s.is_blocked && (timeout->tv_sec > 0 || (timeout->tv_sec == 0 && timeout->tv_usec));
		/* gettimeofday is not monotonic; using it here is not strictly correct */
		if (has_timeout) {
			gettimeofday(&start_time, NULL);
		}

		do {
			struct timeval cur_time, elapsed_time;

			ERR_clear_error();
			if (sslsock->is_client) {
				n = SSL_connect(sslsock->ssl_handle);
			} else {
				n = SSL_accept(sslsock->ssl_handle);
			}

			if (has_timeout) {
				gettimeofday(&cur_time, NULL);
				elapsed_time = php_openssl_subtract_timeval(cur_time, start_time);

				if (php_openssl_compare_timeval( elapsed_time, *timeout) > 0) {
					php_error_docref(NULL, E_WARNING, "SSL: Handshake timed out");
					return -1;
				}
			}

			if (n <= 0) {
				/* in case of SSL_ERROR_WANT_READ/WRITE, do not retry in non-blocking mode */
				retry = php_openssl_handle_ssl_error(stream, n, blocked);
				if (retry) {
					/* wait until something interesting happens in the socket. It may be a
					 * timeout. Also consider the unlikely of possibility of a write block  */
					int err = SSL_get_error(sslsock->ssl_handle, n);
					struct timeval left_time;

					if (has_timeout) {
						left_time = php_openssl_subtract_timeval(*timeout, elapsed_time);
					}
					php_pollfd_for(sslsock->s.socket, (err == SSL_ERROR_WANT_READ) ?
						(POLLIN|POLLPRI) : POLLOUT, has_timeout ? &left_time : NULL);
				}
			} else {
				retry = 0;
			}
		} while (retry);

		if (sslsock->s.is_blocked != blocked && SUCCESS == php_set_sock_blocking(sslsock->s.socket, blocked)) {
			sslsock->s.is_blocked = blocked;
		}

		if (n == 1) {
			peer_cert = SSL_get_peer_certificate(sslsock->ssl_handle);
			if (peer_cert && PHP_STREAM_CONTEXT(stream)) {
				cert_captured = php_openssl_capture_peer_certs(stream, sslsock, peer_cert);
			}

			if (FAILURE == php_openssl_apply_peer_verification_policy(sslsock->ssl_handle, peer_cert, stream)) {
				SSL_shutdown(sslsock->ssl_handle);
				n = -1;
			} else {
				sslsock->ssl_active = 1;
			}
		} else if (errno == EAGAIN) {
			n = 0;
		} else {
			n = -1;
			/* We want to capture the peer cert even if verification fails*/
			peer_cert = SSL_get_peer_certificate(sslsock->ssl_handle);
			if (peer_cert && PHP_STREAM_CONTEXT(stream)) {
				cert_captured = php_openssl_capture_peer_certs(stream, sslsock, peer_cert);
			}
		}

		if (n && peer_cert && cert_captured == 0) {
			X509_free(peer_cert);
		}

		return n;

	} else if (!cparam->inputs.activate && sslsock->ssl_active) {
		/* deactivate - common for server/client */
		SSL_shutdown(sslsock->ssl_handle);
		sslsock->ssl_active = 0;
	}

	return -1;
}
/* }}} */

static ssize_t php_openssl_sockop_read(php_stream *stream, char *buf, size_t count) /* {{{ */
{
	return php_openssl_sockop_io( 1, stream, buf, count );
}
/* }}} */

static ssize_t php_openssl_sockop_write(php_stream *stream, const char *buf, size_t count) /* {{{ */
{
	return php_openssl_sockop_io( 0, stream, (char*)buf, count );
}
/* }}} */

/**
 * Factored out common functionality (blocking, timeout, loop management) for read and write.
 * Perform IO (read or write) to an SSL socket. If we have a timeout, we switch to non-blocking mode
 * for the duration of the operation, using select to do our waits. If we time out, or we have an error
 * report that back to PHP
 */
static ssize_t php_openssl_sockop_io(int read, php_stream *stream, char *buf, size_t count) /* {{{ */
{
	php_openssl_netstream_data_t *sslsock = (php_openssl_netstream_data_t*)stream->abstract;

	/* Only do this if SSL is active. */
	if (sslsock->ssl_active) {
		int retry = 1;
		struct timeval start_time;
		struct timeval *timeout = NULL;
		int began_blocked = sslsock->s.is_blocked;
		int has_timeout = 0;
		int nr_bytes = 0;

		/* prevent overflow in openssl */
		if (count > INT_MAX) {
			count = INT_MAX;
		}

		/* never use a timeout with non-blocking sockets */
		if (began_blocked) {
			timeout = &sslsock->s.timeout;
		}

		if (timeout && php_set_sock_blocking(sslsock->s.socket, 0) == SUCCESS) {
			sslsock->s.is_blocked = 0;
		}

		if (!sslsock->s.is_blocked && timeout && (timeout->tv_sec > 0 || (timeout->tv_sec == 0 && timeout->tv_usec))) {
			has_timeout = 1;
			/* gettimeofday is not monotonic; using it here is not strictly correct */
			gettimeofday(&start_time, NULL);
		}

		/* Main IO loop. */
		do {
			struct timeval cur_time, elapsed_time, left_time;

			/* If we have a timeout to check, figure out how much time has elapsed since we started. */
			if (has_timeout) {
				gettimeofday(&cur_time, NULL);

				/* Determine how much time we've taken so far. */
				elapsed_time = php_openssl_subtract_timeval(cur_time, start_time);

				/* and return an error if we've taken too long. */
				if (php_openssl_compare_timeval(elapsed_time, *timeout) > 0 ) {
					/* If the socket was originally blocking, set it back. */
					if (began_blocked) {
						php_set_sock_blocking(sslsock->s.socket, 1);
						sslsock->s.is_blocked = 1;
					}
					sslsock->s.timeout_event = 1;
					return -1;
				}
			}

			/* Now, do the IO operation. Don't block if we can't complete... */
			ERR_clear_error();
			if (read) {
				nr_bytes = SSL_read(sslsock->ssl_handle, buf, (int)count);

				if (sslsock->reneg && sslsock->reneg->should_close) {
					/* renegotiation rate limiting triggered */
					php_stream_xport_shutdown(stream, (stream_shutdown_t)SHUT_RDWR);
					nr_bytes = 0;
					stream->eof = 1;
					 break;
				}
			} else {
				nr_bytes = SSL_write(sslsock->ssl_handle, buf, (int)count);
			}

			/* Now, how much time until we time out? */
			if (has_timeout) {
				left_time = php_openssl_subtract_timeval( *timeout, elapsed_time );
			}

			/* If we didn't do anything on the last loop (or an error) check to see if we should retry or exit. */
			if (nr_bytes <= 0) {

				/* Get the error code from SSL, and check to see if it's an error or not. */
				int err = SSL_get_error(sslsock->ssl_handle, nr_bytes );
				retry = php_openssl_handle_ssl_error(stream, nr_bytes, 0);

				/* If we get this (the above doesn't check) then we'll retry as well. */
				if (errno == EAGAIN && err == SSL_ERROR_WANT_READ && read) {
					retry = 1;
				}
				if (errno == EAGAIN && err == SSL_ERROR_WANT_WRITE && read == 0) {
					retry = 1;
				}

				/* Also, on reads, we may get this condition on an EOF. We should check properly. */
				if (read) {
					stream->eof = (retry == 0 && errno != EAGAIN && !SSL_pending(sslsock->ssl_handle));
				}

				/* Don't loop indefinitely in non-blocking mode if no data is available */
				if (began_blocked == 0) {
					break;
				}

				/* Now, if we have to wait some time, and we're supposed to be blocking, wait for the socket to become
				 * available. Now, php_pollfd_for uses select to wait up to our time_left value only...
				 */
				if (retry) {
					if (read) {
						php_pollfd_for(sslsock->s.socket, (err == SSL_ERROR_WANT_WRITE) ?
							(POLLOUT|POLLPRI) : (POLLIN|POLLPRI), has_timeout ? &left_time : NULL);
					} else {
						php_pollfd_for(sslsock->s.socket, (err == SSL_ERROR_WANT_READ) ?
							(POLLIN|POLLPRI) : (POLLOUT|POLLPRI), has_timeout ? &left_time : NULL);
					}
				}
			} else {
				/* Else, if we got bytes back, check for possible errors. */
				int err = SSL_get_error(sslsock->ssl_handle, nr_bytes);

				/* If we didn't get any error, then let's return it to PHP. */
				if (err == SSL_ERROR_NONE) {
					break;
				}

				/* Otherwise, we need to wait again (up to time_left or we get an error) */
				if (began_blocked) {
					if (read) {
						php_pollfd_for(sslsock->s.socket, (err == SSL_ERROR_WANT_WRITE) ?
							(POLLOUT|POLLPRI) : (POLLIN|POLLPRI), has_timeout ? &left_time : NULL);
					} else {
						php_pollfd_for(sslsock->s.socket, (err == SSL_ERROR_WANT_READ) ?
							(POLLIN|POLLPRI) : (POLLOUT|POLLPRI), has_timeout ? &left_time : NULL);
					}
				}
			}

			/* Finally, we keep going until we got data, and an SSL_ERROR_NONE, unless we had an error. */
		} while (retry);

		/* Tell PHP if we read / wrote bytes. */
		if (nr_bytes > 0) {
			php_stream_notify_progress_increment(PHP_STREAM_CONTEXT(stream), nr_bytes, 0);
		}

		/* And if we were originally supposed to be blocking, let's reset the socket to that. */
		if (began_blocked && php_set_sock_blocking(sslsock->s.socket, 1) == SUCCESS) {
			sslsock->s.is_blocked = 1;
		}

		return 0 > nr_bytes ? 0 : nr_bytes;
	} else {
		size_t nr_bytes = 0;

		/* This block is if we had no timeout... We will just sit and wait forever on the IO operation. */
		if (read) {
			nr_bytes = php_stream_socket_ops.read(stream, buf, count);
		} else {
			nr_bytes = php_stream_socket_ops.write(stream, buf, count);
		}

		return nr_bytes;
	}
}
/* }}} */

static struct timeval php_openssl_subtract_timeval(struct timeval a, struct timeval b) /* {{{ */
{
	struct timeval difference;

	difference.tv_sec  = a.tv_sec  - b.tv_sec;
	difference.tv_usec = a.tv_usec - b.tv_usec;

	if (a.tv_usec < b.tv_usec) {
		difference.tv_sec  -= 1L;
		difference.tv_usec += 1000000L;
	}

	return difference;
}
/* }}} */

static int php_openssl_compare_timeval( struct timeval a, struct timeval b )
{
	if (a.tv_sec > b.tv_sec || (a.tv_sec == b.tv_sec && a.tv_usec > b.tv_usec) ) {
		return 1;
	} else if( a.tv_sec == b.tv_sec && a.tv_usec == b.tv_usec ) {
		return 0;
	} else {
		return -1;
	}
}

static int php_openssl_sockop_close(php_stream *stream, int close_handle) /* {{{ */
{
	php_openssl_netstream_data_t *sslsock = (php_openssl_netstream_data_t*)stream->abstract;
#ifdef PHP_WIN32
	int n;
#endif
	unsigned i;

	if (close_handle) {
		if (sslsock->ssl_active) {
			SSL_shutdown(sslsock->ssl_handle);
			sslsock->ssl_active = 0;
		}
		if (sslsock->ssl_handle) {
			SSL_free(sslsock->ssl_handle);
			sslsock->ssl_handle = NULL;
		}
		if (sslsock->ctx) {
			SSL_CTX_free(sslsock->ctx);
			sslsock->ctx = NULL;
		}
#ifdef HAVE_TLS_ALPN
		if (sslsock->alpn_ctx.data) {
			pefree(sslsock->alpn_ctx.data, php_stream_is_persistent(stream));
		}
#endif
#ifdef PHP_WIN32
		if (sslsock->s.socket == -1)
			sslsock->s.socket = SOCK_ERR;
#endif
		if (sslsock->s.socket != SOCK_ERR) {
#ifdef PHP_WIN32
			/* prevent more data from coming in */
			shutdown(sslsock->s.socket, SHUT_RD);

			/* try to make sure that the OS sends all data before we close the connection.
			 * Essentially, we are waiting for the socket to become writeable, which means
			 * that all pending data has been sent.
			 * We use a small timeout which should encourage the OS to send the data,
			 * but at the same time avoid hanging indefinitely.
			 * */
			do {
				n = php_pollfd_for_ms(sslsock->s.socket, POLLOUT, 500);
			} while (n == -1 && php_socket_errno() == EINTR);
#endif
			closesocket(sslsock->s.socket);
			sslsock->s.socket = SOCK_ERR;
		}
	}

	if (sslsock->sni_certs) {
		for (i = 0; i < sslsock->sni_cert_count; i++) {
			if (sslsock->sni_certs[i].ctx) {
				SSL_CTX_free(sslsock->sni_certs[i].ctx);
				pefree(sslsock->sni_certs[i].name, php_stream_is_persistent(stream));
			}
		}
		pefree(sslsock->sni_certs, php_stream_is_persistent(stream));
		sslsock->sni_certs = NULL;
	}

	if (sslsock->url_name) {
		pefree(sslsock->url_name, php_stream_is_persistent(stream));
	}

	if (sslsock->reneg) {
		pefree(sslsock->reneg, php_stream_is_persistent(stream));
	}

	pefree(sslsock, php_stream_is_persistent(stream));

	return 0;
}
/* }}} */

static int php_openssl_sockop_flush(php_stream *stream) /* {{{ */
{
	return php_stream_socket_ops.flush(stream);
}
/* }}} */

static int php_openssl_sockop_stat(php_stream *stream, php_stream_statbuf *ssb) /* {{{ */
{
	return php_stream_socket_ops.stat(stream, ssb);
}
/* }}} */

static inline int php_openssl_tcp_sockop_accept(php_stream *stream, php_openssl_netstream_data_t *sock,
		php_stream_xport_param *xparam STREAMS_DC)  /* {{{ */
{
	int clisock;
	bool nodelay = 0;
	zval *tmpzval = NULL;

	xparam->outputs.client = NULL;

	if (PHP_STREAM_CONTEXT(stream) &&
		(tmpzval = php_stream_context_get_option(PHP_STREAM_CONTEXT(stream), "socket", "tcp_nodelay")) != NULL &&
		zend_is_true(tmpzval)) {
		nodelay = 1;
	}

	clisock = php_network_accept_incoming(sock->s.socket,
		xparam->want_textaddr ? &xparam->outputs.textaddr : NULL,
		xparam->want_addr ? &xparam->outputs.addr : NULL,
		xparam->want_addr ? &xparam->outputs.addrlen : NULL,
		xparam->inputs.timeout,
		xparam->want_errortext ? &xparam->outputs.error_text : NULL,
		&xparam->outputs.error_code,
		nodelay);

	if (clisock >= 0) {
		php_openssl_netstream_data_t *clisockdata = (php_openssl_netstream_data_t*) emalloc(sizeof(*clisockdata));

		/* copy underlying tcp fields */
		memset(clisockdata, 0, sizeof(*clisockdata));
		memcpy(clisockdata, sock, sizeof(clisockdata->s));

		clisockdata->s.socket = clisock;
#ifdef __linux__
		/* O_NONBLOCK is not inherited on Linux */
		clisockdata->s.is_blocked = 1;
#endif

		xparam->outputs.client = php_stream_alloc_rel(stream->ops, clisockdata, NULL, "r+");
		if (xparam->outputs.client) {
			xparam->outputs.client->ctx = stream->ctx;
			if (stream->ctx) {
				GC_ADDREF(stream->ctx);
			}
		}

		if (xparam->outputs.client && sock->enable_on_connect) {
			/* remove the client bit */
			sock->method &= ~STREAM_CRYPTO_IS_CLIENT;

			clisockdata->method = sock->method;

			if (php_stream_xport_crypto_setup(xparam->outputs.client, clisockdata->method,
					NULL) < 0 || php_stream_xport_crypto_enable(
					xparam->outputs.client, 1) < 0) {
				php_error_docref(NULL, E_WARNING, "Failed to enable crypto");

				php_stream_close(xparam->outputs.client);
				xparam->outputs.client = NULL;
				xparam->outputs.returncode = -1;
			}
		}
	}

	return xparam->outputs.client == NULL ? -1 : 0;
}
/* }}} */

static int php_openssl_sockop_set_option(php_stream *stream, int option, int value, void *ptrparam)  /* {{{ */
{
	php_openssl_netstream_data_t *sslsock = (php_openssl_netstream_data_t*)stream->abstract;
	php_stream_xport_crypto_param *cparam = (php_stream_xport_crypto_param *)ptrparam;
	php_stream_xport_param *xparam = (php_stream_xport_param *)ptrparam;

	switch (option) {
		case PHP_STREAM_OPTION_META_DATA_API:
			if (sslsock->ssl_active) {
				zval tmp;
				char *proto_str;
				const SSL_CIPHER *cipher;

				array_init(&tmp);

				switch (SSL_version(sslsock->ssl_handle)) {
#ifdef HAVE_TLS13
					case TLS1_3_VERSION: proto_str = "TLSv1.3"; break;
#endif
#ifdef HAVE_TLS12
					case TLS1_2_VERSION: proto_str = "TLSv1.2"; break;
#endif
#ifdef HAVE_TLS11
					case TLS1_1_VERSION: proto_str = "TLSv1.1"; break;
#endif
					case TLS1_VERSION: proto_str = "TLSv1"; break;
#ifdef HAVE_SSL3
					case SSL3_VERSION: proto_str = "SSLv3"; break;
#endif
					default: proto_str = "UNKNOWN";
				}

				cipher = SSL_get_current_cipher(sslsock->ssl_handle);

				add_assoc_string(&tmp, "protocol", proto_str);
				add_assoc_string(&tmp, "cipher_name", (char *) SSL_CIPHER_get_name(cipher));
				add_assoc_long(&tmp, "cipher_bits", SSL_CIPHER_get_bits(cipher, NULL));
				add_assoc_string(&tmp, "cipher_version", SSL_CIPHER_get_version(cipher));

#ifdef HAVE_TLS_ALPN
				{
					const unsigned char *alpn_proto = NULL;
					unsigned int alpn_proto_len = 0;

					SSL_get0_alpn_selected(sslsock->ssl_handle, &alpn_proto, &alpn_proto_len);
					if (alpn_proto) {
						add_assoc_stringl(&tmp, "alpn_protocol", (char *)alpn_proto, alpn_proto_len);
					}
				}
#endif
				add_assoc_zval((zval *)ptrparam, "crypto", &tmp);
			}

			add_assoc_bool((zval *)ptrparam, "timed_out", sslsock->s.timeout_event);
			add_assoc_bool((zval *)ptrparam, "blocked", sslsock->s.is_blocked);
			add_assoc_bool((zval *)ptrparam, "eof", stream->eof);

			return PHP_STREAM_OPTION_RETURN_OK;

		case PHP_STREAM_OPTION_CHECK_LIVENESS:
			{
				struct timeval tv;
				char buf;
				int alive = 1;

				if (value == -1) {
					if (sslsock->s.timeout.tv_sec == -1) {
#ifdef _WIN32
						tv.tv_sec = (long)FG(default_socket_timeout);
#else
						tv.tv_sec = (time_t)FG(default_socket_timeout);
#endif
						tv.tv_usec = 0;
					} else {
						tv = sslsock->connect_timeout;
					}
				} else {
					tv.tv_sec = value;
					tv.tv_usec = 0;
				}

				if (sslsock->s.socket == -1) {
					alive = 0;
				} else if (
					(
						!sslsock->ssl_active &&
						value == 0 &&
						!(stream->flags & PHP_STREAM_FLAG_NO_IO) &&
						((MSG_DONTWAIT != 0) || !sslsock->s.is_blocked)
					) ||
					php_pollfd_for(sslsock->s.socket, PHP_POLLREADABLE|POLLPRI, &tv) > 0
				) {
					/* the poll() call was skipped if the socket is non-blocking (or MSG_DONTWAIT is available) and if the timeout is zero */
					/* additionally, we don't use this optimization if SSL is active because in that case, we're not using MSG_DONTWAIT */
					if (sslsock->ssl_active) {
						int n = SSL_peek(sslsock->ssl_handle, &buf, sizeof(buf));
						if (n <= 0) {
							int err = SSL_get_error(sslsock->ssl_handle, n);
							switch (err) {
								case SSL_ERROR_SYSCALL:
									alive = php_socket_errno() == EAGAIN;
									break;
								case SSL_ERROR_WANT_READ:
								case SSL_ERROR_WANT_WRITE:
									alive = 1;
									break;
								default:
									/* any other problem is a fatal error */
									alive = 0;
							}
						}
					} else if (0 == recv(sslsock->s.socket, &buf, sizeof(buf), MSG_PEEK|MSG_DONTWAIT) && php_socket_errno() != EAGAIN) {
						alive = 0;
					}
				}
				return alive ? PHP_STREAM_OPTION_RETURN_OK : PHP_STREAM_OPTION_RETURN_ERR;
			}

		case PHP_STREAM_OPTION_CRYPTO_API:

			switch(cparam->op) {

				case STREAM_XPORT_CRYPTO_OP_SETUP:
					cparam->outputs.returncode = php_openssl_setup_crypto(stream, sslsock, cparam);
					return PHP_STREAM_OPTION_RETURN_OK;
					break;
				case STREAM_XPORT_CRYPTO_OP_ENABLE:
					cparam->outputs.returncode = php_openssl_enable_crypto(stream, sslsock, cparam);
					return PHP_STREAM_OPTION_RETURN_OK;
					break;
				default:
					/* fall through */
					break;
			}

			break;

		case PHP_STREAM_OPTION_XPORT_API:
			switch(xparam->op) {

				case STREAM_XPORT_OP_CONNECT:
				case STREAM_XPORT_OP_CONNECT_ASYNC:
					/* TODO: Async connects need to check the enable_on_connect option when
					 * we notice that the connect has actually been established */
					php_stream_socket_ops.set_option(stream, option, value, ptrparam);

					if ((sslsock->enable_on_connect) &&
						((xparam->outputs.returncode == 0) ||
						(xparam->op == STREAM_XPORT_OP_CONNECT_ASYNC &&
						xparam->outputs.returncode == 1 && xparam->outputs.error_code == EINPROGRESS)))
					{
						if (php_stream_xport_crypto_setup(stream, sslsock->method, NULL) < 0 ||
								php_stream_xport_crypto_enable(stream, 1) < 0) {
							php_error_docref(NULL, E_WARNING, "Failed to enable crypto");
							xparam->outputs.returncode = -1;
						}
					}
					return PHP_STREAM_OPTION_RETURN_OK;

				case STREAM_XPORT_OP_ACCEPT:
					/* we need to copy the additional fields that the underlying tcp transport
					 * doesn't know about */
					xparam->outputs.returncode = php_openssl_tcp_sockop_accept(stream, sslsock, xparam STREAMS_CC);


					return PHP_STREAM_OPTION_RETURN_OK;

				default:
					/* fall through */
					break;
			}
	}

	return php_stream_socket_ops.set_option(stream, option, value, ptrparam);
}
/* }}} */

static int php_openssl_sockop_cast(php_stream *stream, int castas, void **ret)  /* {{{ */
{
	php_openssl_netstream_data_t *sslsock = (php_openssl_netstream_data_t*)stream->abstract;

	switch(castas)	{
		case PHP_STREAM_AS_STDIO:
			if (sslsock->ssl_active) {
				return FAILURE;
			}
			if (ret)	{
				*ret = fdopen(sslsock->s.socket, stream->mode);
				if (*ret) {
					return SUCCESS;
				}
				return FAILURE;
			}
			return SUCCESS;

		case PHP_STREAM_AS_FD_FOR_SELECT:
			if (ret) {
				size_t pending;
				if (stream->writepos == stream->readpos
					&& sslsock->ssl_active
					&& (pending = (size_t)SSL_pending(sslsock->ssl_handle)) > 0) {
						php_stream_fill_read_buffer(stream, pending < stream->chunk_size
							? pending
							: stream->chunk_size);
				}

				*(php_socket_t *)ret = sslsock->s.socket;
			}
			return SUCCESS;

		case PHP_STREAM_AS_FD:
		case PHP_STREAM_AS_SOCKETD:
			if (sslsock->ssl_active) {
				return FAILURE;
			}
			if (ret) {
				*(php_socket_t *)ret = sslsock->s.socket;
			}
			return SUCCESS;
		default:
			return FAILURE;
	}
}
/* }}} */

const php_stream_ops php_openssl_socket_ops = {
	php_openssl_sockop_write, php_openssl_sockop_read,
	php_openssl_sockop_close, php_openssl_sockop_flush,
	"tcp_socket/ssl",
	NULL, /* seek */
	php_openssl_sockop_cast,
	php_openssl_sockop_stat,
	php_openssl_sockop_set_option,
};

static zend_long php_openssl_get_crypto_method(
		php_stream_context *ctx, zend_long crypto_method)  /* {{{ */
{
	zval *val;

	if (ctx && (val = php_stream_context_get_option(ctx, "ssl", "crypto_method")) != NULL) {
		crypto_method = zval_get_long(val);
		crypto_method |= STREAM_CRYPTO_IS_CLIENT;
	}

	return crypto_method;
}
/* }}} */

static char *php_openssl_get_url_name(const char *resourcename,
		size_t resourcenamelen, int is_persistent)  /* {{{ */
{
	php_url *url;

	if (!resourcename) {
		return NULL;
	}

	url = php_url_parse_ex(resourcename, resourcenamelen);
	if (!url) {
		return NULL;
	}

	if (url->host) {
		const char * host = ZSTR_VAL(url->host);
		char * url_name = NULL;
		size_t len = ZSTR_LEN(url->host);

		/* skip trailing dots */
		while (len && host[len-1] == '.') {
			--len;
		}

		if (len) {
			url_name = pestrndup(host, len, is_persistent);
		}

		php_url_free(url);
		return url_name;
	}

	php_url_free(url);
	return NULL;
}
/* }}} */

php_stream *php_openssl_ssl_socket_factory(const char *proto, size_t protolen,
		const char *resourcename, size_t resourcenamelen,
		const char *persistent_id, int options, int flags,
		struct timeval *timeout,
		php_stream_context *context STREAMS_DC) /* {{{ */
{
	php_stream *stream = NULL;
	php_openssl_netstream_data_t *sslsock = NULL;

	sslsock = pemalloc(sizeof(php_openssl_netstream_data_t), persistent_id ? 1 : 0);
	memset(sslsock, 0, sizeof(*sslsock));

	sslsock->s.is_blocked = 1;
	/* this timeout is used by standard stream funcs, therefore it should use the default value */
#ifdef _WIN32
	sslsock->s.timeout.tv_sec = (long)FG(default_socket_timeout);
#else
	sslsock->s.timeout.tv_sec = (time_t)FG(default_socket_timeout);
#endif
	sslsock->s.timeout.tv_usec = 0;

	/* use separate timeout for our private funcs */
	sslsock->connect_timeout.tv_sec = timeout->tv_sec;
	sslsock->connect_timeout.tv_usec = timeout->tv_usec;

	/* we don't know the socket until we have determined if we are binding or
	 * connecting */
	sslsock->s.socket = -1;

	/* Initialize context as NULL */
	sslsock->ctx = NULL;

	stream = php_stream_alloc_rel(&php_openssl_socket_ops, sslsock, persistent_id, "r+");

	if (stream == NULL)	{
		pefree(sslsock, persistent_id ? 1 : 0);
		return NULL;
	}

	if (strncmp(proto, "ssl", protolen) == 0) {
		sslsock->enable_on_connect = 1;
		sslsock->method = php_openssl_get_crypto_method(context, STREAM_CRYPTO_METHOD_TLS_ANY_CLIENT);
	} else if (strncmp(proto, "sslv2", protolen) == 0) {
		php_error_docref(NULL, E_WARNING, "SSLv2 unavailable in this PHP version");
		php_stream_close(stream);
		return NULL;
	} else if (strncmp(proto, "sslv3", protolen) == 0) {
#ifdef HAVE_SSL3
		sslsock->enable_on_connect = 1;
		sslsock->method = STREAM_CRYPTO_METHOD_SSLv3_CLIENT;
#else
		php_error_docref(NULL, E_WARNING,
			"SSLv3 support is not compiled into the OpenSSL library against which PHP is linked");
		php_stream_close(stream);
		return NULL;
#endif
	} else if (strncmp(proto, "tls", protolen) == 0) {
		sslsock->enable_on_connect = 1;
		sslsock->method = php_openssl_get_crypto_method(context, STREAM_CRYPTO_METHOD_TLS_ANY_CLIENT);
	} else if (strncmp(proto, "tlsv1.0", protolen) == 0) {
		sslsock->enable_on_connect = 1;
		sslsock->method = STREAM_CRYPTO_METHOD_TLSv1_0_CLIENT;
	} else if (strncmp(proto, "tlsv1.1", protolen) == 0) {
#ifdef HAVE_TLS11
		sslsock->enable_on_connect = 1;
		sslsock->method = STREAM_CRYPTO_METHOD_TLSv1_1_CLIENT;
#else
		php_error_docref(NULL, E_WARNING,
			"TLSv1.1 support is not compiled into the OpenSSL library against which PHP is linked");
		php_stream_close(stream);
		return NULL;
#endif
	} else if (strncmp(proto, "tlsv1.2", protolen) == 0) {
#ifdef HAVE_TLS12
		sslsock->enable_on_connect = 1;
		sslsock->method = STREAM_CRYPTO_METHOD_TLSv1_2_CLIENT;
#else
		php_error_docref(NULL, E_WARNING,
			"TLSv1.2 support is not compiled into the OpenSSL library against which PHP is linked");
		php_stream_close(stream);
		return NULL;
#endif
	} else if (strncmp(proto, "tlsv1.3", protolen) == 0) {
#ifdef HAVE_TLS13
		sslsock->enable_on_connect = 1;
		sslsock->method = STREAM_CRYPTO_METHOD_TLSv1_3_CLIENT;
#else
		php_error_docref(NULL, E_WARNING,
			"TLSv1.3 support is not compiled into the OpenSSL library against which PHP is linked");
		php_stream_close(stream);
		return NULL;
#endif
	}

	sslsock->url_name = php_openssl_get_url_name(resourcename, resourcenamelen, !!persistent_id);

	return stream;
}
/* }}} */
