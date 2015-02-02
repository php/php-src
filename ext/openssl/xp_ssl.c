/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2015 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Wez Furlong <wez@thebrainroom.com>                          |
  |          Daniel Lowrey <rdlowrey@php.net>                            |
  |          Chris Wright <daverandom@php.net>                           |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

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
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/err.h>

#ifdef PHP_WIN32
#include "win32/winutil.h"
#include "win32/time.h"
#include <Wincrypt.h>
/* These are from Wincrypt.h, they conflict with OpenSSL */
#undef X509_NAME
#undef X509_CERT_PAIR
#undef X509_EXTENSIONS
#endif

#ifdef NETWARE
#include <sys/select.h>
#endif

#if !defined(OPENSSL_NO_ECDH) && OPENSSL_VERSION_NUMBER >= 0x0090800fL
#define HAVE_ECDH 1
#endif

#if OPENSSL_VERSION_NUMBER >= 0x00908070L && !defined(OPENSSL_NO_TLSEXT)
#define HAVE_SNI 1
#endif

/* Flags for determining allowed stream crypto methods */
#define STREAM_CRYPTO_IS_CLIENT            (1<<0)
#define STREAM_CRYPTO_METHOD_SSLv2         (1<<1)
#define STREAM_CRYPTO_METHOD_SSLv3         (1<<2)
#define STREAM_CRYPTO_METHOD_TLSv1_0       (1<<3)
#define STREAM_CRYPTO_METHOD_TLSv1_1       (1<<4)
#define STREAM_CRYPTO_METHOD_TLSv1_2       (1<<5)

/* Simplify ssl context option retrieval */
#define GET_VER_OPT(name)               (PHP_STREAM_CONTEXT(stream) && (val = php_stream_context_get_option(PHP_STREAM_CONTEXT(stream), "ssl", name)) != NULL)
#define GET_VER_OPT_STRING(name, str)   if (GET_VER_OPT(name)) { convert_to_string_ex(val); str = Z_STRVAL_P(val); }
#define GET_VER_OPT_LONG(name, num)     if (GET_VER_OPT(name)) { convert_to_long_ex(val); num = Z_LVAL_P(val); }

/* Used for peer verification in windows */
#define PHP_X509_NAME_ENTRY_TO_UTF8(ne, i, out) ASN1_STRING_to_UTF8(&out, X509_NAME_ENTRY_get_data(X509_NAME_get_entry(ne, i)))

extern php_stream* php_openssl_get_stream_from_ssl_handle(const SSL *ssl);
extern zend_string* php_openssl_x509_fingerprint(X509 *peer, const char *method, zend_bool raw);
extern int php_openssl_get_ssl_stream_data_index();
extern int php_openssl_get_x509_list_id(void);
static struct timeval subtract_timeval( struct timeval a, struct timeval b );
static int compare_timeval( struct timeval a, struct timeval b );
static size_t php_openssl_sockop_io(int read, php_stream *stream, char *buf, size_t count);

php_stream_ops php_openssl_socket_ops;

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
	char *url_name;
	unsigned state_set:1;
	unsigned _spare:31;
} php_openssl_netstream_data_t;

/* it doesn't matter that we do some hash traversal here, since it is done only
 * in an error condition arising from a network connection problem */
static int is_http_stream_talking_to_iis(php_stream *stream) /* {{{ */
{
	if (Z_TYPE(stream->wrapperdata) == IS_ARRAY && stream->wrapper && strcasecmp(stream->wrapper->wops->label, "HTTP") == 0) {
		/* the wrapperdata is an array zval containing the headers */
		zval *tmp;

#define SERVER_MICROSOFT_IIS	"Server: Microsoft-IIS"
#define SERVER_GOOGLE "Server: GFE/"

		ZEND_HASH_FOREACH_VAL(Z_ARRVAL(stream->wrapperdata), tmp) {
			if (strncasecmp(Z_STRVAL_P(tmp), SERVER_MICROSOFT_IIS, sizeof(SERVER_MICROSOFT_IIS)-1) == 0) {
				return 1;
			} else if (strncasecmp(Z_STRVAL_P(tmp), SERVER_GOOGLE, sizeof(SERVER_GOOGLE)-1) == 0) {
				return 1;
			}
		} ZEND_HASH_FOREACH_END();
	}
	return 0;
}
/* }}} */

static int handle_ssl_error(php_stream *stream, int nr_bytes, zend_bool is_init) /* {{{ */
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
					if (!is_http_stream_talking_to_iis(stream) && ERR_get_error() != 0) {
						php_error_docref(NULL, E_WARNING,
								"SSL: fatal protocol error");
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


			/* fall through */
		default:
			/* some other error */
			ecode = ERR_get_error();

			switch (ERR_GET_REASON(ecode)) {
				case SSL_R_NO_SHARED_CIPHER:
					php_error_docref(NULL, E_WARNING, "SSL_R_NO_SHARED_CIPHER: no suitable shared cipher could be used.  This could be because the server is missing an SSL certificate (local_cert context option)");
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
							ebuf.s ? ebuf.s->val : "");
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

static int php_x509_fingerprint_cmp(X509 *peer, const char *method, const char *expected)
{
	zend_string *fingerprint;
	int result = -1;

	fingerprint = php_openssl_x509_fingerprint(peer, method, 0);
	if (fingerprint) {
		result = strcmp(expected, fingerprint->val);
		zend_string_release(fingerprint);
	}

	return result;
}

static zend_bool php_x509_fingerprint_match(X509 *peer, zval *val)
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

		return method && php_x509_fingerprint_cmp(peer, method, Z_STRVAL_P(val)) == 0;
	} else if (Z_TYPE_P(val) == IS_ARRAY) {
		zval *current;
		zend_string *key;

		ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(val), key, current) {
			if (key && Z_TYPE_P(current) == IS_STRING
				&& php_x509_fingerprint_cmp(peer, key->val, Z_STRVAL_P(current)) != 0
			) {
				return 0;
			}
		} ZEND_HASH_FOREACH_END();
		return 1;
	}
	return 0;
}

static zend_bool matches_wildcard_name(const char *subjectname, const char *certname) /* {{{ */
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

static zend_bool matches_san_list(X509 *peer, const char *subject_name) /* {{{ */
{
	int i, san_name_len;
	zend_bool is_match = 0;
	unsigned char *cert_name = NULL;

	GENERAL_NAMES *alt_names = X509_get_ext_d2i(peer, NID_subject_alt_name, 0, 0);
	int alt_name_count = sk_GENERAL_NAME_num(alt_names);

	for (i = 0; i < alt_name_count; i++) {
		GENERAL_NAME *san = sk_GENERAL_NAME_value(alt_names, i);
		if (san->type != GEN_DNS) {
			/* we only care about DNS names */
			continue;
		}

		san_name_len = ASN1_STRING_length(san->d.dNSName);
		ASN1_STRING_to_UTF8(&cert_name, san->d.dNSName);

		/* prevent null byte poisoning */
		if (san_name_len != strlen((const char*)cert_name)) {
			php_error_docref(NULL, E_WARNING, "Peer SAN entry is malformed");
		} else {
			is_match = matches_wildcard_name(subject_name, (const char *)cert_name);
		}

		OPENSSL_free(cert_name);

		if (is_match) {
			break;
		}
	}

	return is_match;
}
/* }}} */

static zend_bool matches_common_name(X509 *peer, const char *subject_name) /* {{{ */
{
	char buf[1024];
	X509_NAME *cert_name;
	zend_bool is_match = 0;
	int cert_name_len;

	cert_name = X509_get_subject_name(peer);
	cert_name_len = X509_NAME_get_text_by_NID(cert_name, NID_commonName, buf, sizeof(buf));

	if (cert_name_len == -1) {
		php_error_docref(NULL, E_WARNING, "Unable to locate peer certificate CN");
	} else if (cert_name_len != strlen(buf)) {
		php_error_docref(NULL, E_WARNING, "Peer certificate CN=`%.*s' is malformed", cert_name_len, buf);
	} else if (matches_wildcard_name(subject_name, buf)) {
		is_match = 1;
	} else {
		php_error_docref(NULL, E_WARNING, "Peer certificate CN=`%.*s' did not match expected CN=`%s'", cert_name_len, buf, subject_name);
	}

	return is_match;
}
/* }}} */

static int apply_peer_verification_policy(SSL *ssl, X509 *peer, php_stream *stream) /* {{{ */
{
	zval *val = NULL;
	char *peer_name = NULL;
	int err,
		must_verify_peer,
		must_verify_peer_name,
		must_verify_fingerprint,
		has_cnmatch_ctx_opt;

	php_openssl_netstream_data_t *sslsock = (php_openssl_netstream_data_t*)stream->abstract;

	must_verify_peer = GET_VER_OPT("verify_peer")
		? zend_is_true(val)
		: sslsock->is_client;

	has_cnmatch_ctx_opt = GET_VER_OPT("CN_match");
	must_verify_peer_name = (has_cnmatch_ctx_opt || GET_VER_OPT("verify_peer_name"))
		? zend_is_true(val)
		: sslsock->is_client;

	must_verify_fingerprint = (GET_VER_OPT("peer_fingerprint") && zend_is_true(val));

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
		if (Z_TYPE_P(val) == IS_STRING || Z_TYPE_P(val) == IS_ARRAY) {
			if (!php_x509_fingerprint_match(peer, val)) {
				php_error_docref(NULL, E_WARNING,
					"Peer fingerprint doesn't match"
				);
				return FAILURE;
			}
		} else {
			php_error_docref(NULL, E_WARNING,
				"Expected peer fingerprint must be a string or an array"
			);
		}
	}

	/* verify the host name presented in the peer certificate */
	if (must_verify_peer_name) {
		GET_VER_OPT_STRING("peer_name", peer_name);

		if (has_cnmatch_ctx_opt) {
			GET_VER_OPT_STRING("CN_match", peer_name);
			php_error(E_DEPRECATED,
				"the 'CN_match' SSL context option is deprecated in favor of 'peer_name'"
			);
		}
		/* If no peer name was specified we use the autodetected url name in client environments */
		if (peer_name == NULL && sslsock->is_client) {
			peer_name = sslsock->url_name;
		}

		if (peer_name) {
			if (matches_san_list(peer, peer_name)) {
				return SUCCESS;
			} else if (matches_common_name(peer, peer_name)) {
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

static int passwd_callback(char *buf, int num, int verify, void *data) /* {{{ */
{
	php_stream *stream = (php_stream *)data;
	zval *val = NULL;
	char *passphrase = NULL;
	/* TODO: could expand this to make a callback into PHP user-space */

	GET_VER_OPT_STRING("passphrase", passphrase);

	if (passphrase) {
		if (Z_STRLEN_P(val) < num - 1) {
			memcpy(buf, Z_STRVAL_P(val), Z_STRLEN_P(val)+1);
			return (int)Z_STRLEN_P(val);
		}
	}
	return 0;
}
/* }}} */

#if defined(PHP_WIN32) && OPENSSL_VERSION_NUMBER >= 0x00907000L
#define RETURN_CERT_VERIFY_FAILURE(code) X509_STORE_CTX_set_error(x509_store_ctx, code); return 0;
static int win_cert_verify_callback(X509_STORE_CTX *x509_store_ctx, void *arg) /* {{{ */
{
	PCCERT_CONTEXT cert_ctx = NULL;
	PCCERT_CHAIN_CONTEXT cert_chain_ctx = NULL;

	php_stream *stream;
	php_openssl_netstream_data_t *sslsock;
	zval *val;
	zend_bool is_self_signed = 0;


	stream = (php_stream*)arg;
	sslsock = (php_openssl_netstream_data_t*)stream->abstract;

	{ /* First convert the x509 struct back to a DER encoded buffer and let Windows decode it into a form it can work with */
		unsigned char *der_buf = NULL;
		int der_len;

		der_len = i2d_X509(x509_store_ctx->cert, &der_buf);
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
			php_error_docref(NULL, E_WARNING, "Error creating certificate context: %s", php_win_err());
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
			php_error_docref(NULL, E_WARNING, "Error getting certificate chain: %s", php_win_err());
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
		LPWSTR server_name = NULL;
		BOOL verify_result;

		{ /* This looks ridiculous and it is - but we validate the name ourselves using the peer_name
		     ctx option, so just use the CN from the cert here */

			X509_NAME *cert_name;
			unsigned char *cert_name_utf8;
			int index, cert_name_utf8_len;
			DWORD num_wchars;

			cert_name = X509_get_subject_name(x509_store_ctx->cert);
			index = X509_NAME_get_index_by_NID(cert_name, NID_commonName, -1);
			if (index < 0) {
				php_error_docref(NULL, E_WARNING, "Unable to locate certificate CN");
				CertFreeCertificateChain(cert_chain_ctx);
				CertFreeCertificateContext(cert_ctx);
				RETURN_CERT_VERIFY_FAILURE(SSL_R_CERTIFICATE_VERIFY_FAILED);
			}

			cert_name_utf8_len = PHP_X509_NAME_ENTRY_TO_UTF8(cert_name, index, cert_name_utf8);

			num_wchars = MultiByteToWideChar(CP_UTF8, 0, (char*)cert_name_utf8, -1, NULL, 0);
			if (num_wchars == 0) {
				php_error_docref(NULL, E_WARNING, "Unable to convert %s to wide character string", cert_name_utf8);
				OPENSSL_free(cert_name_utf8);
				CertFreeCertificateChain(cert_chain_ctx);
				CertFreeCertificateContext(cert_ctx);
				RETURN_CERT_VERIFY_FAILURE(SSL_R_CERTIFICATE_VERIFY_FAILED);
			}

			server_name = emalloc((num_wchars * sizeof(WCHAR)) + sizeof(WCHAR));

			num_wchars = MultiByteToWideChar(CP_UTF8, 0, (char*)cert_name_utf8, -1, server_name, num_wchars);
			if (num_wchars == 0) {
				php_error_docref(NULL, E_WARNING, "Unable to convert %s to wide character string", cert_name_utf8);
				efree(server_name);
				OPENSSL_free(cert_name_utf8);
				CertFreeCertificateChain(cert_chain_ctx);
				CertFreeCertificateContext(cert_ctx);
				RETURN_CERT_VERIFY_FAILURE(SSL_R_CERTIFICATE_VERIFY_FAILED);
			}

			OPENSSL_free(cert_name_utf8);
		}

		ssl_policy_params.dwAuthType = (sslsock->is_client) ? AUTHTYPE_SERVER : AUTHTYPE_CLIENT;
		ssl_policy_params.pwszServerName = server_name;
		chain_policy_params.pvExtraPolicyPara = &ssl_policy_params;

		verify_result = CertVerifyCertificateChainPolicy(CERT_CHAIN_POLICY_SSL, cert_chain_ctx, &chain_policy_params, &chain_policy_status);

		efree(server_name);
		CertFreeCertificateChain(cert_chain_ctx);
		CertFreeCertificateContext(cert_ctx);

		if (!verify_result) {
			php_error_docref(NULL, E_WARNING, "Error verifying certificate chain policy: %s", php_win_err());
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

static long load_stream_cafile(X509_STORE *cert_store, const char *cafile) /* {{{ */
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

static int enable_peer_verification(SSL_CTX *ctx, php_stream *stream) /* {{{ */
{
	zval *val = NULL;
	char *cafile = NULL;
	char *capath = NULL;

	GET_VER_OPT_STRING("cafile", cafile);
	GET_VER_OPT_STRING("capath", capath);

	if (!cafile) {
		cafile = zend_ini_string("openssl.cafile", sizeof("openssl.cafile")-1, 0);
		cafile = strlen(cafile) ? cafile : NULL;
	}

	if (!capath) {
		capath = zend_ini_string("openssl.capath", sizeof("openssl.capath")-1, 0);
		capath = strlen(capath) ? capath : NULL;
	}

	if (cafile || capath) {
		if (!SSL_CTX_load_verify_locations(ctx, cafile, capath)) {
			if (cafile && !load_stream_cafile(SSL_CTX_get_cert_store(ctx), cafile)) {
				return FAILURE;
			}
		}
	} else {
#if defined(PHP_WIN32) && OPENSSL_VERSION_NUMBER >= 0x00907000L
		SSL_CTX_set_cert_verify_callback(ctx, win_cert_verify_callback, (void *)stream);
		SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);
#else
		php_openssl_netstream_data_t *sslsock;
		sslsock = (php_openssl_netstream_data_t*)stream->abstract;

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

static void disable_peer_verification(SSL_CTX *ctx, php_stream *stream) /* {{{ */
{
	SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, NULL);
}
/* }}} */

static int set_local_cert(SSL_CTX *ctx, php_stream *stream) /* {{{ */
{
	zval *val = NULL;
	char *certfile = NULL;

	GET_VER_OPT_STRING("local_cert", certfile);

	if (certfile) {
		char resolved_path_buff[MAXPATHLEN];
		const char * private_key = NULL;

		if (VCWD_REALPATH(certfile, resolved_path_buff)) {
			/* a certificate to use for authentication */
			if (SSL_CTX_use_certificate_chain_file(ctx, resolved_path_buff) != 1) {
				php_error_docref(NULL, E_WARNING, "Unable to set local cert chain file `%s'; Check that your cafile/capath settings include details of your certificate and its issuer", certfile);
				return FAILURE;
			}
			GET_VER_OPT_STRING("local_pk", private_key);

			if (private_key) {
				char resolved_path_buff_pk[MAXPATHLEN];
				if (VCWD_REALPATH(private_key, resolved_path_buff_pk)) {
					if (SSL_CTX_use_PrivateKey_file(ctx, resolved_path_buff_pk, SSL_FILETYPE_PEM) != 1) {
						php_error_docref(NULL, E_WARNING, "Unable to set private key file `%s'", resolved_path_buff_pk);
						return FAILURE;
					}
				}
			} else {
				if (SSL_CTX_use_PrivateKey_file(ctx, resolved_path_buff, SSL_FILETYPE_PEM) != 1) {
					php_error_docref(NULL, E_WARNING, "Unable to set private key file `%s'", resolved_path_buff);
					return FAILURE;
				}
			}

#if OPENSSL_VERSION_NUMBER < 0x10001001L
			do {
				/* Unnecessary as of OpenSSLv1.0.1 (will segfault if used with >= 10001001 ) */
				X509 *cert = NULL;
				EVP_PKEY *key = NULL;
				SSL *tmpssl = SSL_new(ctx);
				cert = SSL_get_certificate(tmpssl);

				if (cert) {
					key = X509_get_pubkey(cert);
					EVP_PKEY_copy_parameters(key, SSL_get_privatekey(tmpssl));
					EVP_PKEY_free(key);
				}
				SSL_free(tmpssl);
			} while (0);
#endif
			if (!SSL_CTX_check_private_key(ctx)) {
				php_error_docref(NULL, E_WARNING, "Private key does not match certificate!");
			}
		}
	}

	return SUCCESS;
}
/* }}} */

static const SSL_METHOD *php_select_crypto_method(zend_long method_value, int is_client) /* {{{ */
{
	if (method_value == STREAM_CRYPTO_METHOD_SSLv2) {
#ifndef OPENSSL_NO_SSL2
		return is_client ? SSLv2_client_method() : SSLv2_server_method();
#else
		php_error_docref(NULL, E_WARNING,
				"SSLv2 support is not compiled into the OpenSSL library PHP is linked against");
		return NULL;
#endif
	} else if (method_value == STREAM_CRYPTO_METHOD_SSLv3) {
#ifndef OPENSSL_NO_SSL3
		return is_client ? SSLv3_client_method() : SSLv3_server_method();
#else
		php_error_docref(NULL, E_WARNING,
				"SSLv3 support is not compiled into the OpenSSL library PHP is linked against");
		return NULL;
#endif
	} else if (method_value == STREAM_CRYPTO_METHOD_TLSv1_0) {
		return is_client ? TLSv1_client_method() : TLSv1_server_method();
	} else if (method_value == STREAM_CRYPTO_METHOD_TLSv1_1) {
#if OPENSSL_VERSION_NUMBER >= 0x10001001L
		return is_client ? TLSv1_1_client_method() : TLSv1_1_server_method();
#else
		php_error_docref(NULL, E_WARNING,
				"TLSv1.1 support is not compiled into the OpenSSL library PHP is linked against");
		return NULL;
#endif
	} else if (method_value == STREAM_CRYPTO_METHOD_TLSv1_2) {
#if OPENSSL_VERSION_NUMBER >= 0x10001001L
		return is_client ? TLSv1_2_client_method() : TLSv1_2_server_method();
#else
		php_error_docref(NULL, E_WARNING,
				"TLSv1.2 support is not compiled into the OpenSSL library PHP is linked against");
		return NULL;
#endif
	} else {
		php_error_docref(NULL, E_WARNING,
				"Invalid crypto method");
		return NULL;
	}
}
/* }}} */

static int php_get_crypto_method_ctx_flags(int method_flags) /* {{{ */
{
	int ssl_ctx_options = SSL_OP_ALL;

#ifndef OPENSSL_NO_SSL2
	if (!(method_flags & STREAM_CRYPTO_METHOD_SSLv2)) {
		ssl_ctx_options |= SSL_OP_NO_SSLv2;
	}
#endif
#ifndef OPENSSL_NO_SSL3
	if (!(method_flags & STREAM_CRYPTO_METHOD_SSLv3)) {
		ssl_ctx_options |= SSL_OP_NO_SSLv3;
	}
#endif
#ifndef OPENSSL_NO_TLS1
	if (!(method_flags & STREAM_CRYPTO_METHOD_TLSv1_0)) {
		ssl_ctx_options |= SSL_OP_NO_TLSv1;
	}
#endif
#if OPENSSL_VERSION_NUMBER >= 0x10001001L
	if (!(method_flags & STREAM_CRYPTO_METHOD_TLSv1_1)) {
		ssl_ctx_options |= SSL_OP_NO_TLSv1_1;
	}

	if (!(method_flags & STREAM_CRYPTO_METHOD_TLSv1_2)) {
		ssl_ctx_options |= SSL_OP_NO_TLSv1_2;
	}
#endif

	return ssl_ctx_options;
}
/* }}} */

static void limit_handshake_reneg(const SSL *ssl) /* {{{ */
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
			if (FAILURE == call_user_function_ex(EG(function_table), NULL, val, &retval, 1, &param, 0, NULL)) {
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

static void info_callback(const SSL *ssl, int where, int ret) /* {{{ */
{
	/* Rate-limit client-initiated handshake renegotiation to prevent DoS */
	if (where & SSL_CB_HANDSHAKE_START) {
		limit_handshake_reneg(ssl);
	}
}
/* }}} */

static void init_server_reneg_limit(php_stream *stream, php_openssl_netstream_data_t *sslsock) /* {{{ */
{
	zval *val;
	zend_long limit = OPENSSL_DEFAULT_RENEG_LIMIT;
	zend_long window = OPENSSL_DEFAULT_RENEG_WINDOW;

	if (PHP_STREAM_CONTEXT(stream) &&
		NULL != (val = php_stream_context_get_option(PHP_STREAM_CONTEXT(stream),
				"ssl", "reneg_limit"))
	) {
		convert_to_long(val);
		limit = Z_LVAL_P(val);
	}

	/* No renegotiation rate-limiting */
	if (limit < 0) {
		return;
	}

	if (PHP_STREAM_CONTEXT(stream) &&
		NULL != (val = php_stream_context_get_option(PHP_STREAM_CONTEXT(stream),
				"ssl", "reneg_window"))
	) {
		convert_to_long(val);
		window = Z_LVAL_P(val);
	}

	sslsock->reneg = (void*)pemalloc(sizeof(php_openssl_handshake_bucket_t),
		php_stream_is_persistent(stream)
	);

	sslsock->reneg->limit = limit;
	sslsock->reneg->window = window;
	sslsock->reneg->prev_handshake = 0;
	sslsock->reneg->tokens = 0;
	sslsock->reneg->should_close = 0;

	SSL_set_info_callback(sslsock->ssl_handle, info_callback);
}
/* }}} */

static int set_server_rsa_key(php_stream *stream, SSL_CTX *ctx) /* {{{ */
{
	zval *val;
	int rsa_key_size;
	RSA* rsa;

	if ((val = php_stream_context_get_option(PHP_STREAM_CONTEXT(stream), "ssl", "rsa_key_size")) != NULL) {
		rsa_key_size = (int) Z_LVAL_P(val);
		if ((rsa_key_size != 1) && (rsa_key_size & (rsa_key_size - 1))) {
			php_error_docref(NULL, E_WARNING, "RSA key size requires a power of 2: %d", rsa_key_size);
			rsa_key_size = 2048;
		}
	} else {
		rsa_key_size = 2048;
	}

	rsa = RSA_generate_key(rsa_key_size, RSA_F4, NULL, NULL);

	if (!SSL_CTX_set_tmp_rsa(ctx, rsa)) {
		php_error_docref(NULL, E_WARNING, "Failed setting RSA key");
		RSA_free(rsa);
		return FAILURE;
	}

	RSA_free(rsa);

	return SUCCESS;
}
/* }}} */

static int set_server_dh_param(SSL_CTX *ctx, char *dh_path) /* {{{ */
{
	DH *dh;
	BIO* bio;

	bio = BIO_new_file(dh_path, "r");

	if (bio == NULL) {
		php_error_docref(NULL, E_WARNING, "Invalid dh_param file: %s", dh_path);
		return FAILURE;
	}

	dh = PEM_read_bio_DHparams(bio, NULL, NULL, NULL);
	BIO_free(bio);

	if (dh == NULL) {
		php_error_docref(NULL, E_WARNING, "Failed reading DH params from file: %s", dh_path);
		return FAILURE;
	}

	if (SSL_CTX_set_tmp_dh(ctx, dh) < 0) {
		php_error_docref(NULL, E_WARNING, "DH param assignment failed");
		DH_free(dh);
		return FAILURE;
	}

	DH_free(dh);

	return SUCCESS;
}
/* }}} */

#ifdef HAVE_ECDH
static int set_server_ecdh_curve(php_stream *stream, SSL_CTX *ctx) /* {{{ */
{
	zval *val;
	int curve_nid;
	char *curve_str;
	EC_KEY *ecdh;

	if ((val = php_stream_context_get_option(PHP_STREAM_CONTEXT(stream), "ssl", "ecdh_curve")) != NULL) {
		convert_to_string_ex(val);
		curve_str = Z_STRVAL_P(val);
		curve_nid = OBJ_sn2nid(curve_str);
		if (curve_nid == NID_undef) {
			php_error_docref(NULL, E_WARNING, "Invalid ECDH curve: %s", curve_str);
			return FAILURE;
		}
	} else {
		curve_nid = NID_X9_62_prime256v1;
	}

	ecdh = EC_KEY_new_by_curve_name(curve_nid);
	if (ecdh == NULL) {
		php_error_docref(NULL, E_WARNING,
			"Failed generating ECDH curve");

		return FAILURE;
	}

	SSL_CTX_set_tmp_ecdh(ctx, ecdh);
	EC_KEY_free(ecdh);

	return SUCCESS;
}
/* }}} */
#endif

static int set_server_specific_opts(php_stream *stream, SSL_CTX *ctx) /* {{{ */
{
	zval *val;
	long ssl_ctx_options = SSL_CTX_get_options(ctx);

#ifdef HAVE_ECDH
	if (FAILURE == set_server_ecdh_curve(stream, ctx)) {
		return FAILURE;
	}
#else
	val = php_stream_context_get_option(PHP_STREAM_CONTEXT(stream), "ssl", "ecdh_curve");
	if (val != NULL) {
		php_error_docref(NULL, E_WARNING,
			"ECDH curve support not compiled into the OpenSSL lib against which PHP is linked");

		return FAILURE;
	}
#endif

	if ((val = php_stream_context_get_option(PHP_STREAM_CONTEXT(stream), "ssl", "dh_param")) != NULL) {
		convert_to_string_ex(val);
		if (FAILURE == set_server_dh_param(ctx,  Z_STRVAL_P(val))) {
			return FAILURE;
		}
	}

	if (FAILURE == set_server_rsa_key(stream, ctx)) {
		return FAILURE;
	}

	if (NULL != (val = php_stream_context_get_option(
				PHP_STREAM_CONTEXT(stream), "ssl", "honor_cipher_order")) &&
			zend_is_true(val)
	) {
		ssl_ctx_options |= SSL_OP_CIPHER_SERVER_PREFERENCE;
	}

	if (NULL != (val = php_stream_context_get_option(
				PHP_STREAM_CONTEXT(stream), "ssl", "single_dh_use")) &&
			zend_is_true(val)
	) {
		ssl_ctx_options |= SSL_OP_SINGLE_DH_USE;
	}

#ifdef HAVE_ECDH
	if (NULL != (val = php_stream_context_get_option(
				PHP_STREAM_CONTEXT(stream), "ssl", "single_ecdh_use")) &&
			zend_is_true(val)) {
		ssl_ctx_options |= SSL_OP_SINGLE_ECDH_USE;
	}
#endif

	SSL_CTX_set_options(ctx, ssl_ctx_options);

	return SUCCESS;
}
/* }}} */

#ifdef HAVE_SNI
static int server_sni_callback(SSL *ssl_handle, int *al, void *arg) /* {{{ */
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
		if (matches_wildcard_name(server_name, sslsock->sni_certs[i].name)) {
			SSL_set_SSL_CTX(ssl_handle, sslsock->sni_certs[i].ctx);
			return SSL_TLSEXT_ERR_OK;
		}
	}

	return SSL_TLSEXT_ERR_NOACK;
}
/* }}} */

static int enable_server_sni(php_stream *stream, php_openssl_netstream_data_t *sslsock)
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

	ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(val), key_index, key, current) {
		(void) key_index;

		if (!key) {
			php_error_docref(NULL, E_WARNING,
				"SNI_server_certs array requires string host name keys"
			);
			return FAILURE;
		}

		if (VCWD_REALPATH(Z_STRVAL_P(current), resolved_path_buff)) {
			/* The hello method is not inherited by SSL structs when assigning a new context
			 * inside the SNI callback, so the just use SSLv23 */
			ctx = SSL_CTX_new(SSLv23_server_method());

			if (SSL_CTX_use_certificate_chain_file(ctx, resolved_path_buff) != 1) {
				php_error_docref(NULL, E_WARNING,
					"failed setting local cert chain file `%s'; " \
					"check that your cafile/capath settings include " \
					"details of your certificate and its issuer",
					resolved_path_buff
				);
				SSL_CTX_free(ctx);
				return FAILURE;
			} else if (SSL_CTX_use_PrivateKey_file(ctx, resolved_path_buff, SSL_FILETYPE_PEM) != 1) {
				php_error_docref(NULL, E_WARNING,
					"failed setting private key from file `%s'",
					resolved_path_buff
				);
				SSL_CTX_free(ctx);
				return FAILURE;
			} else {
				sslsock->sni_certs[i].name = pestrdup(key->val, php_stream_is_persistent(stream));
				sslsock->sni_certs[i].ctx = ctx;
				++i;
			}
		} else {
			php_error_docref(NULL, E_WARNING,
				"failed setting local cert chain file `%s'; file not found",
				Z_STRVAL_P(current)
			);
			return FAILURE;
		}
	} ZEND_HASH_FOREACH_END();

	SSL_CTX_set_tlsext_servername_callback(sslsock->ctx, server_sni_callback);

	return SUCCESS;
}

static void enable_client_sni(php_stream *stream, php_openssl_netstream_data_t *sslsock) /* {{{ */
{
	zval *val;
	char *sni_server_name;

	/* If SNI is explicitly disabled we're finished here */
	if (GET_VER_OPT("SNI_enabled") && !zend_is_true(val)) {
		return;
	}

	sni_server_name = sslsock->url_name;

	GET_VER_OPT_STRING("peer_name", sni_server_name);

	if (GET_VER_OPT("SNI_server_name")) {
		GET_VER_OPT_STRING("SNI_server_name", sni_server_name);
		php_error(E_DEPRECATED, "SNI_server_name is deprecated in favor of peer_name");
	}

	if (sni_server_name) {
		SSL_set_tlsext_host_name(sslsock->ssl_handle, sni_server_name);
	}
}
/* }}} */
#endif

int php_openssl_setup_crypto(php_stream *stream,
		php_openssl_netstream_data_t *sslsock,
		php_stream_xport_crypto_param *cparam
		) /* {{{ */
{
	const SSL_METHOD *method;
	int ssl_ctx_options;
	int method_flags;
	char *cipherlist = NULL;
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
	method_flags = ((cparam->inputs.method >> 1) << 1);

	/* Should we use a specific crypto method or is generic SSLv23 okay? */
	if ((method_flags & (method_flags-1)) == 0) {
		ssl_ctx_options = SSL_OP_ALL;
		method = php_select_crypto_method(method_flags, sslsock->is_client);
		if (method == NULL) {
			return FAILURE;
		}
	} else {
		method = sslsock->is_client ? SSLv23_client_method() : SSLv23_server_method();
		ssl_ctx_options = php_get_crypto_method_ctx_flags(method_flags);
		if (ssl_ctx_options == -1) {
			return FAILURE;
		}
	}

#if OPENSSL_VERSION_NUMBER >= 0x10001001L
	sslsock->ctx = SSL_CTX_new(method);
#else
	/* Avoid const warning with old versions */
	sslsock->ctx = SSL_CTX_new((SSL_METHOD*)method);
#endif

	if (sslsock->ctx == NULL) {
		php_error_docref(NULL, E_WARNING, "SSL context creation failure");
		return FAILURE;
	}

#if OPENSSL_VERSION_NUMBER >= 0x0090806fL
	if (GET_VER_OPT("no_ticket") && zend_is_true(val)) {
		ssl_ctx_options |= SSL_OP_NO_TICKET;
	}
#endif

#if OPENSSL_VERSION_NUMBER >= 0x0090605fL
	ssl_ctx_options &= ~SSL_OP_DONT_INSERT_EMPTY_FRAGMENTS;
#endif

#if OPENSSL_VERSION_NUMBER >= 0x10000000L
	if (!GET_VER_OPT("disable_compression") || zend_is_true(val)) {
		ssl_ctx_options |= SSL_OP_NO_COMPRESSION;
	}
#endif

	if (GET_VER_OPT("verify_peer") && !zend_is_true(val)) {
		disable_peer_verification(sslsock->ctx, stream);
	} else if (FAILURE == enable_peer_verification(sslsock->ctx, stream)) {
		return FAILURE;
	}

	/* callback for the passphrase (for localcert) */
	if (GET_VER_OPT("passphrase")) {
		SSL_CTX_set_default_passwd_cb_userdata(sslsock->ctx, stream);
		SSL_CTX_set_default_passwd_cb(sslsock->ctx, passwd_callback);
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
	if (FAILURE == set_local_cert(sslsock->ctx, stream)) {
		return FAILURE;
	}

	SSL_CTX_set_options(sslsock->ctx, ssl_ctx_options);

	if (sslsock->is_client == 0 &&
		PHP_STREAM_CONTEXT(stream) &&
		FAILURE == set_server_specific_opts(stream, sslsock->ctx)
	) {
		return FAILURE;
	}

	sslsock->ssl_handle = SSL_new(sslsock->ctx);
	if (sslsock->ssl_handle == NULL) {
		php_error_docref(NULL, E_WARNING, "SSL handle creation failure");
		SSL_CTX_free(sslsock->ctx);
		sslsock->ctx = NULL;
		return FAILURE;
	} else {
		SSL_set_ex_data(sslsock->ssl_handle, php_openssl_get_ssl_stream_data_index(), stream);
	}

	if (!SSL_set_fd(sslsock->ssl_handle, sslsock->s.socket)) {
		handle_ssl_error(stream, 0, 1);
	}

#ifdef HAVE_SNI
	/* Enable server-side SNI */
	if (sslsock->is_client == 0 && enable_server_sni(stream, sslsock) == FAILURE) {
		return FAILURE;
	}
#endif

	/* Enable server-side handshake renegotiation rate-limiting */
	if (sslsock->is_client == 0) {
		init_server_reneg_limit(stream, sslsock);
	}

#ifdef SSL_MODE_RELEASE_BUFFERS
	do {
		long mode = SSL_get_mode(sslsock->ssl_handle);
		SSL_set_mode(sslsock->ssl_handle, mode | SSL_MODE_RELEASE_BUFFERS);
	} while (0);
#endif

	if (cparam->inputs.session) {
		if (cparam->inputs.session->ops != &php_openssl_socket_ops) {
			php_error_docref(NULL, E_WARNING, "supplied session stream must be an SSL enabled stream");
		} else if (((php_openssl_netstream_data_t*)cparam->inputs.session->abstract)->ssl_handle == NULL) {
			php_error_docref(NULL, E_WARNING, "supplied SSL session stream is not initialized");
		} else {
			SSL_copy_session_id(sslsock->ssl_handle, ((php_openssl_netstream_data_t*)cparam->inputs.session->abstract)->ssl_handle);
		}
	}

	return SUCCESS;
}
/* }}} */

static zend_array *capture_session_meta(SSL *ssl_handle) /* {{{ */
{
	zval meta_arr;
	char *proto_str;
	long proto = SSL_version(ssl_handle);
	const SSL_CIPHER *cipher = SSL_get_current_cipher(ssl_handle);

	switch (proto) {
#if OPENSSL_VERSION_NUMBER >= 0x10001001L
		case TLS1_2_VERSION: proto_str = "TLSv1.2"; break;
		case TLS1_1_VERSION: proto_str = "TLSv1.1"; break;
#endif
		case TLS1_VERSION: proto_str = "TLSv1"; break;
		case SSL3_VERSION: proto_str = "SSLv3"; break;
		case SSL2_VERSION: proto_str = "SSLv2"; break;
		default: proto_str = "UNKNOWN";
	}

	array_init(&meta_arr);
	add_assoc_string(&meta_arr, "protocol", proto_str);
	add_assoc_string(&meta_arr, "cipher_name", (char *) SSL_CIPHER_get_name(cipher));
	add_assoc_long(&meta_arr, "cipher_bits", SSL_CIPHER_get_bits(cipher, NULL));
	add_assoc_string(&meta_arr, "cipher_version", SSL_CIPHER_get_version(cipher));

	return Z_ARR(meta_arr);
}
/* }}} */

static int capture_peer_certs(php_stream *stream, php_openssl_netstream_data_t *sslsock, X509 *peer_cert) /* {{{ */
{
	zval *val, zcert;
	int cert_captured = 0;

	if (NULL != (val = php_stream_context_get_option(PHP_STREAM_CONTEXT(stream),
			"ssl", "capture_peer_cert")) &&
		zend_is_true(val)
	) {
		zend_register_resource(&zcert, peer_cert, php_openssl_get_x509_list_id());
		php_stream_context_set_option(PHP_STREAM_CONTEXT(stream), "ssl", "peer_certificate", &zcert);
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
				zend_register_resource(&zcert, mycert, php_openssl_get_x509_list_id());
				add_next_index_zval(&arr, &zcert);
			}

		} else {
			ZVAL_NULL(&arr);
		}

		php_stream_context_set_option(PHP_STREAM_CONTEXT(stream), "ssl", "peer_certificate_chain", &arr);
		zval_dtor(&arr);
	}

	return cert_captured;
}
/* }}} */

static int php_openssl_enable_crypto(php_stream *stream,
		php_openssl_netstream_data_t *sslsock,
		php_stream_xport_crypto_param *cparam
		)
{
	int n;
	int retry = 1;
	int cert_captured;
	X509 *peer_cert;

	if (cparam->inputs.activate && !sslsock->ssl_active) {
		struct timeval	start_time,
						*timeout;
		int				blocked		= sslsock->s.is_blocked,
						has_timeout = 0;

#ifdef HAVE_SNI
		if (sslsock->is_client) {
			enable_client_sni(stream, sslsock);
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
		}

		timeout = sslsock->is_client ? &sslsock->connect_timeout : &sslsock->s.timeout;
		has_timeout = !sslsock->s.is_blocked && (timeout->tv_sec || timeout->tv_usec);
		/* gettimeofday is not monotonic; using it here is not strictly correct */
		if (has_timeout) {
			gettimeofday(&start_time, NULL);
		}

		do {
			struct timeval	cur_time,
							elapsed_time;
			
			if (sslsock->is_client) {
				n = SSL_connect(sslsock->ssl_handle);
			} else {
				n = SSL_accept(sslsock->ssl_handle);
			}

			if (has_timeout) {
				gettimeofday(&cur_time, NULL);
				elapsed_time = subtract_timeval( cur_time, start_time );
			
				if (compare_timeval( elapsed_time, *timeout) > 0) {
					php_error_docref(NULL, E_WARNING, "SSL: Handshake timed out");
					return -1;
				}
			}

			if (n <= 0) {
				/* in case of SSL_ERROR_WANT_READ/WRITE, do not retry in non-blocking mode */
				retry = handle_ssl_error(stream, n, blocked);
				if (retry) {
					/* wait until something interesting happens in the socket. It may be a
					 * timeout. Also consider the unlikely of possibility of a write block  */
					int err = SSL_get_error(sslsock->ssl_handle, n);
					struct timeval left_time;

					if (has_timeout) {
						left_time = subtract_timeval( *timeout, elapsed_time );
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
				cert_captured = capture_peer_certs(stream, sslsock, peer_cert);
			}

			if (FAILURE == apply_peer_verification_policy(sslsock->ssl_handle, peer_cert, stream)) {
				SSL_shutdown(sslsock->ssl_handle);
				n = -1;
			} else {
				sslsock->ssl_active = 1;

				if (PHP_STREAM_CONTEXT(stream)) {
					zval *val;

					if (NULL != (val = php_stream_context_get_option(PHP_STREAM_CONTEXT(stream),
							"ssl", "capture_session_meta")) &&
						zend_is_true(val)
					) {
						zval meta_arr;
						ZVAL_ARR(&meta_arr, capture_session_meta(sslsock->ssl_handle));
						php_stream_context_set_option(PHP_STREAM_CONTEXT(stream), "ssl", "session_meta", &meta_arr);
						zval_dtor(&meta_arr);
					}
				}
			}
		} else if (errno == EAGAIN) {
			n = 0;
		} else {
			n = -1;
			peer_cert = SSL_get_peer_certificate(sslsock->ssl_handle);
			if (peer_cert && PHP_STREAM_CONTEXT(stream)) {
				cert_captured = capture_peer_certs(stream, sslsock, peer_cert);
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

static size_t php_openssl_sockop_write(php_stream *stream, const char *buf, size_t count) /* {{{ */
{
	return php_openssl_sockop_io( 0, stream, (char*)buf, count );
}
/* }}} */

static size_t php_openssl_sockop_read(php_stream *stream, char *buf, size_t count) /* {{{ */
{
	return php_openssl_sockop_io( 1, stream, buf, count );
}
/* }}} */

/**
 * Factored out common functionality (blocking, timeout, loop management) for read and write.
 * Perform IO (read or write) to an SSL socket. If we have a timeout, we switch to non-blocking mode
 * for the duration of the operation, using select to do our waits. If we time out, or we have an error
 * report that back to PHP
 *
 */
static size_t php_openssl_sockop_io(int read, php_stream *stream, char *buf, size_t count) /* {{{ */
{
	php_openssl_netstream_data_t *sslsock = (php_openssl_netstream_data_t*)stream->abstract;

	/* Only do this if SSL is active. */
	if (sslsock->ssl_active) {
		int retry = 1;
		struct timeval start_time,
		               *timeout;
		int    blocked     = sslsock->s.is_blocked,
		       has_timeout = 0;
		int nr_bytes = 0;

		/* prevent overflow in openssl */
		if (count > INT_MAX) {
			count = INT_MAX;
		}

		/* Begin by making the socket non-blocking. This allows us to check the timeout. */
		if (SUCCESS == php_set_sock_blocking(sslsock->s.socket, 0)) {
			sslsock->s.is_blocked = 0;
		}

		/* Get the timeout value (and make sure we are to check it. */
		timeout = sslsock->is_client ? &sslsock->connect_timeout : &sslsock->s.timeout;
		has_timeout = !sslsock->s.is_blocked && (timeout->tv_sec || timeout->tv_usec);

		/* gettimeofday is not monotonic; using it here is not strictly correct */
		if (has_timeout) {
			gettimeofday(&start_time, NULL);
		}

		/* Main IO loop. */
		do {
			struct timeval cur_time, elapsed_time, left_time;

			/* If we have a timeout to check, figure out how much time has elapsed since we started. */
			if (has_timeout) {
				gettimeofday(&cur_time, NULL);

				/* Determine how much time we've taken so far. */
				elapsed_time = subtract_timeval( cur_time, start_time );

				/* and return an error if we've taken too long. */
				if (compare_timeval( elapsed_time, *timeout) > 0 ) {
					/* If the socket was originally blocking, set it back. */
					if (blocked) {
						php_set_sock_blocking(sslsock->s.socket, 1);
						sslsock->s.is_blocked = 1;
					}
					return -1;
				}
			}

			/* Now, do the IO operation. Don't block if we can't complete... */
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
				left_time = subtract_timeval( *timeout, elapsed_time );
			}

			/* If we didn't do anything on the last loop (or an error) check to see if we should retry or exit. */
			if (nr_bytes <= 0) {

				/* Get the error code from SSL, and check to see if it's an error or not. */
				int err = SSL_get_error(sslsock->ssl_handle, nr_bytes );
				retry = handle_ssl_error(stream, nr_bytes, 0);

				/* If we get this (the above doesn't check) then we'll retry as well. */
				if (errno == EAGAIN && err == SSL_ERROR_WANT_READ && read) {
					retry = 1;
				}
				if (errno == EAGAIN && SSL_ERROR_WANT_WRITE && read == 0) {          
					retry = 1;
				}
				
				/* Also, on reads, we may get this condition on an EOF. We should check properly. */
				if (read) {
					stream->eof = (retry == 0 && errno != EAGAIN && !SSL_pending(sslsock->ssl_handle));
				}

				/* Now, if we have to wait some time, and we're supposed to be blocking, wait for the socket to become
				 * available. Now, php_pollfd_for uses select to wait up to our time_left value only...
				 */
				if (retry && blocked) {
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
				int err = SSL_get_error(sslsock->ssl_handle, nr_bytes );

				/* If we didn't get any error, then let's return it to PHP. */
				if (err == SSL_ERROR_NONE) {
					break;
				}

				/* Otherwise, we need to wait again (up to time_left or we get an error) */
				if (blocked) {
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
		if (blocked) {
			php_set_sock_blocking(sslsock->s.socket, 1);
			 sslsock->s.is_blocked = 1;
		}

		return 0 > nr_bytes ? 0 : nr_bytes;
	} else {
		size_t nr_bytes = 0;

		/*
	     	 * This block is if we had no timeout... We will just sit and wait forever on the IO operation.
		 */
		if (read) {
			nr_bytes = php_stream_socket_ops.read(stream, buf, count);
		} else {
			nr_bytes = php_stream_socket_ops.write(stream, buf, count);
		}

		return nr_bytes;
	}
}
/* }}} */

static struct timeval subtract_timeval( struct timeval a, struct timeval b )
{
	struct timeval difference;

	difference.tv_sec  = a.tv_sec  - b.tv_sec;
	difference.tv_usec = a.tv_usec - b.tv_usec;

	if (a.tv_usec < b.tv_usec) {
	  	b.tv_sec  -= 1L;
	   	b.tv_usec += 1000000L;
	}

	return difference;
}

static int compare_timeval( struct timeval a, struct timeval b )
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
		for (i=0; i<sslsock->sni_cert_count; i++) {
			SSL_CTX_free(sslsock->sni_certs[i].ctx);
			pefree(sslsock->sni_certs[i].name, php_stream_is_persistent(stream));
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
		php_stream_xport_param *xparam STREAMS_DC)
{
	int clisock;

	xparam->outputs.client = NULL;

	clisock = php_network_accept_incoming(sock->s.socket,
			xparam->want_textaddr ? &xparam->outputs.textaddr : NULL,
			xparam->want_addr ? &xparam->outputs.addr : NULL,
			xparam->want_addr ? &xparam->outputs.addrlen : NULL,
			xparam->inputs.timeout,
			xparam->want_errortext ? &xparam->outputs.error_text : NULL,
			&xparam->outputs.error_code
			);

	if (clisock >= 0) {
		php_openssl_netstream_data_t *clisockdata;

		clisockdata = emalloc(sizeof(*clisockdata));

		if (clisockdata == NULL) {
			closesocket(clisock);
			/* technically a fatal error */
		} else {
			/* copy underlying tcp fields */
			memset(clisockdata, 0, sizeof(*clisockdata));
			memcpy(clisockdata, sock, sizeof(clisockdata->s));

			clisockdata->s.socket = clisock;

			xparam->outputs.client = php_stream_alloc_rel(stream->ops, clisockdata, NULL, "r+");
			if (xparam->outputs.client) {
				xparam->outputs.client->ctx = stream->ctx;
				if (stream->ctx) {
					GC_REFCOUNT(stream->ctx)++;
				}
			}
		}

		if (xparam->outputs.client && sock->enable_on_connect) {
			/* remove the client bit */
			if (sock->method & STREAM_CRYPTO_IS_CLIENT) {
				sock->method = ((sock->method >> 1) << 1);
			}

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

static int php_openssl_sockop_set_option(php_stream *stream, int option, int value, void *ptrparam)
{
	php_openssl_netstream_data_t *sslsock = (php_openssl_netstream_data_t*)stream->abstract;
	php_stream_xport_crypto_param *cparam = (php_stream_xport_crypto_param *)ptrparam;
	php_stream_xport_param *xparam = (php_stream_xport_param *)ptrparam;

	switch (option) {
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
				} else if (php_pollfd_for(sslsock->s.socket, PHP_POLLREADABLE|POLLPRI, &tv) > 0) {
					if (sslsock->ssl_active) {
						int n;

						do {
							n = SSL_peek(sslsock->ssl_handle, &buf, sizeof(buf));
							if (n <= 0) {
								int err = SSL_get_error(sslsock->ssl_handle, n);

								if (err == SSL_ERROR_SYSCALL) {
									alive = php_socket_errno() == EAGAIN;
									break;
								}

								if (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE) {
									/* re-negotiate */
									continue;
								}

								/* any other problem is a fatal error */
								alive = 0;
							}
							/* either peek succeeded or there was an error; we
							 * have set the alive flag appropriately */
							break;
						} while (1);
					} else if (0 == recv(sslsock->s.socket, &buf, sizeof(buf), MSG_PEEK) && php_socket_errno() != EAGAIN) {
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

static int php_openssl_sockop_cast(php_stream *stream, int castas, void **ret)
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

php_stream_ops php_openssl_socket_ops = {
	php_openssl_sockop_write, php_openssl_sockop_read,
	php_openssl_sockop_close, php_openssl_sockop_flush,
	"tcp_socket/ssl",
	NULL, /* seek */
	php_openssl_sockop_cast,
	php_openssl_sockop_stat,
	php_openssl_sockop_set_option,
};

static zend_long get_crypto_method(php_stream_context *ctx, zend_long crypto_method)
{
	zval *val;

	if (ctx && (val = php_stream_context_get_option(ctx, "ssl", "crypto_method")) != NULL) {
		convert_to_long_ex(val);
		crypto_method = (zend_long)Z_LVAL_P(val);
	        crypto_method |= STREAM_CRYPTO_IS_CLIENT;
	}

	return crypto_method;
}

static char *get_url_name(const char *resourcename, size_t resourcenamelen, int is_persistent)
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
		const char * host = url->host;
		char * url_name = NULL;
		size_t len = strlen(host);

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

php_stream *php_openssl_ssl_socket_factory(const char *proto, size_t protolen,
		const char *resourcename, size_t resourcenamelen,
		const char *persistent_id, int options, int flags,
		struct timeval *timeout,
		php_stream_context *context STREAMS_DC)
{
	php_stream *stream = NULL;
	php_openssl_netstream_data_t *sslsock = NULL;

	sslsock = pemalloc(sizeof(php_openssl_netstream_data_t), persistent_id ? 1 : 0);
	memset(sslsock, 0, sizeof(*sslsock));

	sslsock->s.is_blocked = 1;
	/* this timeout is used by standard stream funcs, therefor it should use the default value */
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
		sslsock->method = get_crypto_method(context, STREAM_CRYPTO_METHOD_ANY_CLIENT);
	} else if (strncmp(proto, "sslv2", protolen) == 0) {
#ifdef OPENSSL_NO_SSL2
		php_error_docref(NULL, E_WARNING, "SSLv2 support is not compiled into the OpenSSL library PHP is linked against");
		return NULL;
#else
		sslsock->enable_on_connect = 1;
		sslsock->method = STREAM_CRYPTO_METHOD_SSLv2_CLIENT;
#endif
	} else if (strncmp(proto, "sslv3", protolen) == 0) {
#ifdef OPENSSL_NO_SSL3
		php_error_docref(NULL, E_WARNING, "SSLv3 support is not compiled into the OpenSSL library PHP is linked against");
		return NULL;
#else
		sslsock->enable_on_connect = 1;
		sslsock->method = STREAM_CRYPTO_METHOD_SSLv3_CLIENT;
#endif
	} else if (strncmp(proto, "tls", protolen) == 0) {
		sslsock->enable_on_connect = 1;
		sslsock->method = get_crypto_method(context, STREAM_CRYPTO_METHOD_TLS_CLIENT);
	} else if (strncmp(proto, "tlsv1.0", protolen) == 0) {
		sslsock->enable_on_connect = 1;
		sslsock->method = STREAM_CRYPTO_METHOD_TLSv1_0_CLIENT;
	} else if (strncmp(proto, "tlsv1.1", protolen) == 0) {
#if OPENSSL_VERSION_NUMBER >= 0x10001001L
		sslsock->enable_on_connect = 1;
		sslsock->method = STREAM_CRYPTO_METHOD_TLSv1_1_CLIENT;
#else
		php_error_docref(NULL, E_WARNING, "TLSv1.1 support is not compiled into the OpenSSL library PHP is linked against");
		return NULL;
#endif
	} else if (strncmp(proto, "tlsv1.2", protolen) == 0) {
#if OPENSSL_VERSION_NUMBER >= 0x10001001L
		sslsock->enable_on_connect = 1;
		sslsock->method = STREAM_CRYPTO_METHOD_TLSv1_2_CLIENT;
#else
		php_error_docref(NULL, E_WARNING, "TLSv1.2 support is not compiled into the OpenSSL library PHP is linked against");
		return NULL;
#endif
	}

	sslsock->url_name = get_url_name(resourcename, resourcenamelen, !!persistent_id);

	return stream;
}



/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
