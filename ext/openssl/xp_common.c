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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "php.h"
#include "php_openssl.h"
#include "php_openssl_backend.h"
#include "xp_common.h"
#include "ext/uri/php_uri.h"

/* Stores the php_stream on the SSL, so callbacks can recover it (defined in openssl.c). */
extern int php_openssl_get_ssl_stream_data_index(void);
#ifdef PHP_WIN32
/* Defined in xp_ssl.c (needs the Windows cert-store headers). */
extern int php_openssl_win_cert_verify_callback(X509_STORE_CTX *x509_store_ctx, void *arg);
#endif

/* Read an option from the "ssl" stream context (same helpers as xp_ssl.c/xp_dtls.c). */
#define GET_VER_OPT(_name) \
	(PHP_STREAM_CONTEXT(stream) && (val = php_stream_context_get_option(PHP_STREAM_CONTEXT(stream), "ssl", _name)) != NULL)
#define GET_VER_OPT_STRING(_name, _str) \
	do { if (GET_VER_OPT(_name)) { if (try_convert_to_string(val)) _str = Z_STRVAL_P(val); } } while (0)
#define GET_VER_OPT_LONG(_name, _num) \
	do { if (GET_VER_OPT(_name)) { _num = zval_get_long(val); } } while (0)

bool php_openssl_x509_fingerprint_is_equal(php_stream *stream, X509 *peer,
		const char *method, const zend_string *expected)
{
	bool is_equal = false;
	zend_string *fingerprint = php_openssl_x509_fingerprint(peer, method, false, stream);
	if (fingerprint) {
		is_equal = zend_string_equals_ci(fingerprint, expected);
		zend_string_release_ex(fingerprint, false);
	}

	return is_equal;
}

bool php_openssl_x509_fingerprint_match(php_stream *stream, X509 *peer, const zval *val)
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

		if (UNEXPECTED(method == NULL)) {
			php_stream_warn(stream, AuthFailed, "peer_fingerprint length doesn't match a md5 or sha1 hash");
			return false;
		}
		if (!php_openssl_x509_fingerprint_is_equal(stream, peer, method, Z_STR_P(val))) {
			php_stream_warn(stream, AuthFailed, "peer_fingerprint match failure");
			return false;
		}
		return true;
	} else if (Z_TYPE_P(val) == IS_ARRAY) {
		zval *current;
		zend_string *key;

		if (!zend_hash_num_elements(Z_ARRVAL_P(val))) {
			php_stream_warn(stream, Generic, "Invalid peer_fingerprint array; [algo => fingerprint] form required");
			return false;
		}

		ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(val), key, current) {
			if (key == NULL || Z_TYPE_P(current) != IS_STRING) {
				php_stream_warn(stream, Generic, "Invalid peer_fingerprint array; [algo => fingerprint] form required");
				return false;
			}
			if (!php_openssl_x509_fingerprint_is_equal(stream, peer, ZSTR_VAL(key), Z_STR_P(current))) {
				php_stream_warn(stream, AuthFailed, "peer_fingerprint match failure");
				return false;
			}
		} ZEND_HASH_FOREACH_END();

		return true;
	} else {
		php_stream_warn(stream, Generic,
			"Invalid peer_fingerprint value; fingerprint string or array of the form [algo => fingerprint] required");
	}

	return false;
}

char *php_openssl_get_url_name(const char *resourcename, size_t resourcenamelen,
		int is_persistent, php_stream_context *context)
{
	if (!resourcename) {
		return NULL;
	}

	const php_uri_parser *uri_parser = php_stream_context_get_uri_parser("ssl", context);
	if (uri_parser == NULL) {
		zend_value_error("%s(): Provided stream context has invalid value for the \"uri_parser_class\" option", get_active_function_name());
		return NULL;
	}

	php_uri_internal *internal_uri = php_uri_parse(uri_parser, resourcename, resourcenamelen, true);
	if (internal_uri == NULL) {
		return NULL;
	}

	char * url_name = NULL;
	zval host_zv;
	zend_result result = php_uri_get_host(internal_uri, PHP_URI_COMPONENT_READ_MODE_RAW, &host_zv);
	if (result == SUCCESS && Z_TYPE(host_zv) == IS_STRING) {
		const char * host = Z_STRVAL(host_zv);
		size_t len = Z_STRLEN(host_zv);

		/* skip trailing dots */
		while (len && host[len-1] == '.') {
			--len;
		}

		if (len) {
			url_name = pestrndup(host, len, is_persistent);
		}
	}

	php_uri_free(internal_uri);
	zval_ptr_dtor(&host_zv);

	return url_name;
}

int php_openssl_passwd_callback(char *buf, int num, int verify, void *data)
{
	php_stream *stream = (php_stream *)data;
	zval *val;

	/* TODO: could expand this to make a callback into PHP user-space */
	if (PHP_STREAM_CONTEXT(stream) == NULL) {
		return 0;
	}

	val = php_stream_context_get_option(PHP_STREAM_CONTEXT(stream), "ssl", "passphrase");
	if (val == NULL || !try_convert_to_string(val)) {
		return 0;
	}

	if (Z_STRLEN_P(val) < (size_t)num - 1) {
		memcpy(buf, Z_STRVAL_P(val), Z_STRLEN_P(val) + 1);
		return (int)Z_STRLEN_P(val);
	}

	return 0;
}

int php_openssl_verify_callback(int preverify_ok, X509_STORE_CTX *ctx)
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
	stream = (php_stream *)SSL_get_ex_data(ssl, php_openssl_get_ssl_stream_data_index());

	/* if allow_self_signed is set, make sure that verification succeeds */
	if (err == X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT
			&& PHP_STREAM_CONTEXT(stream) != NULL
			&& (val = php_stream_context_get_option(PHP_STREAM_CONTEXT(stream), "ssl", "allow_self_signed")) != NULL
			&& zend_is_true(val)) {
		ret = 1;
	}

	/* check the depth */
	if (PHP_STREAM_CONTEXT(stream) != NULL
			&& (val = php_stream_context_get_option(PHP_STREAM_CONTEXT(stream), "ssl", "verify_depth")) != NULL) {
		allowed_depth = zval_get_long(val);
	}
	if ((zend_ulong)depth > allowed_depth) {
		ret = 0;
		X509_STORE_CTX_set_error(ctx, X509_V_ERR_CERT_CHAIN_TOO_LONG);
	}

	return ret;
}

void php_openssl_add_crypto_cipher(zval *crypto, SSL *ssl)
{
	const SSL_CIPHER *cipher = SSL_get_current_cipher(ssl);
	if (cipher != NULL) {
		add_assoc_string(crypto, "cipher_name", (char *)SSL_CIPHER_get_name(cipher));
		add_assoc_long(crypto, "cipher_bits", SSL_CIPHER_get_bits(cipher, NULL));
		add_assoc_string(crypto, "cipher_version", (char *)SSL_CIPHER_get_version(cipher));
	}
}

zend_result php_openssl_set_local_cert(SSL_CTX *ctx, php_stream *stream)
{
	zval *val;
	char *certfile = NULL;
	size_t certfile_len = 0;

	if (PHP_STREAM_CONTEXT(stream) != NULL
			&& (val = php_stream_context_get_option(PHP_STREAM_CONTEXT(stream), "ssl", "local_cert")) != NULL
			&& try_convert_to_string(val)) {
		certfile = Z_STRVAL_P(val);
		certfile_len = Z_STRLEN_P(val);
	}

	if (certfile) {
		char resolved_path_buff[MAXPATHLEN];
		char *private_key = NULL;
		size_t private_key_len = 0;

		if (!php_openssl_check_path_ex(certfile, certfile_len, resolved_path_buff, 0, false, false,
				"local_cert in ssl stream context", stream)) {
			php_stream_warn(stream, NotFound, "Unable to get real path of certificate file `%s'", certfile);
			return FAILURE;
		}
		/* a certificate to use for authentication */
		if (SSL_CTX_use_certificate_chain_file(ctx, resolved_path_buff) != 1) {
			php_stream_warn(stream, WriteFailed,
				"Unable to set local cert chain file `%s'; Check that your cafile/capath "
				"settings include details of your certificate and its issuer",
				certfile);
			return FAILURE;
		}

		if (PHP_STREAM_CONTEXT(stream) != NULL
				&& (val = php_stream_context_get_option(PHP_STREAM_CONTEXT(stream), "ssl", "local_pk")) != NULL
				&& try_convert_to_string(val)) {
			private_key = Z_STRVAL_P(val);
			private_key_len = Z_STRLEN_P(val);
		}
		if (private_key && !php_openssl_check_path_ex(private_key, private_key_len, resolved_path_buff, 0, false, false,
				"local_pk in ssl stream context", stream)) {
			php_stream_warn(stream, NotFound, "Unable to get real path of private key file `%s'", private_key);
			return FAILURE;
		}
		if (SSL_CTX_use_PrivateKey_file(ctx, resolved_path_buff, SSL_FILETYPE_PEM) != 1) {
			php_stream_warn(stream, WriteFailed, "Unable to set private key file `%s'", resolved_path_buff);
			return FAILURE;
		}
		if (!SSL_CTX_check_private_key(ctx)) {
			php_stream_warn(stream, PermissionDenied, "Private key does not match certificate!");
		}
	}

	return SUCCESS;
}

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
		// TODO: no stream and no wrapper, cannot use php_stream_warn(stream, ReadFailed, ...) nor php_stream_wrapper_log_error()
		php_error(E_WARNING, "failed loading cafile stream: `%s'", cafile);
		return 0;
	} else if (stream->wrapper->is_url) {
		php_stream_warn(stream, PermissionDenied, "remote cafile streams are disabled for security purposes");
		php_stream_close(stream);
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
		cert = php_openssl_pem_read_bio_x509(buffer);
		BIO_free(buffer);
		buffer_active = 0;
		if (cert && X509_STORE_add_cert(cert_store, cert)) {
			++certs_added;
		}
		/* TODO: notify user when adding certificate failed? */
		X509_free(cert);
		goto cert_start;
	}

	stream_complete: {
		php_stream_close(stream);
		if (buffer_active == 1) {
			BIO_free(buffer);
		}
	}

	if (certs_added == 0) {
		php_stream_warn(stream, DecodingFailed, "no valid certs found cafile stream: `%s'", cafile);
	}

	return certs_added;
}
/* }}} */

zend_result php_openssl_enable_peer_verification(SSL_CTX *ctx, php_stream *stream, bool is_client) /* {{{ */
{
	zval *val = NULL;
	const char *cafile = NULL;
	const char *capath = NULL;

	GET_VER_OPT_STRING("cafile", cafile);
	GET_VER_OPT_STRING("capath", capath);

	if (cafile == NULL) {
		const zend_string *cafile_str = zend_ini_str_literal("openssl.cafile");
		cafile = ZSTR_LEN(cafile_str) ? ZSTR_VAL(cafile_str) : NULL;
	} else if (!is_client) {
		/* Servers need to load and assign CA names from the cafile */
		STACK_OF(X509_NAME) *cert_names = SSL_load_client_CA_file(cafile);
		if (cert_names != NULL) {
			SSL_CTX_set_client_CA_list(ctx, cert_names);
		} else {
			php_stream_warn(stream, InvalidFormat, "SSL: failed loading CA names from cafile");
			return FAILURE;
		}
	}

	if (capath == NULL) {
		const zend_string *capath_str = zend_ini_str_literal("openssl.capath");
		capath = ZSTR_LEN(capath_str) ? ZSTR_VAL(capath_str) : NULL;
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
		if (is_client && !SSL_CTX_set_default_verify_paths(ctx)) {
			php_stream_warn(stream, ProtocolError,
				"Unable to set default verify locations and no CA settings specified");
			return FAILURE;
		}
#endif
	}

	SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, php_openssl_verify_callback);

	return SUCCESS;
}
/* }}} */

void php_openssl_disable_peer_verification(SSL_CTX *ctx, php_stream *stream) /* {{{ */
{
	SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, NULL);
}
/* }}} */

int php_openssl_setup_crypto_on_connect(php_stream *stream,
		php_stream_xport_crypt_method_t method)
{
	zval *val;
	php_stream *session_stream = NULL;

	if (PHP_STREAM_CONTEXT(stream) &&
			(val = php_stream_context_get_option(PHP_STREAM_CONTEXT(stream), "ssl", "session_stream")) != NULL) {
		php_stream_from_zval_no_verify(session_stream, val);
	}

	if (php_stream_xport_crypto_setup(stream, method, session_stream) < 0 ||
			php_stream_xport_crypto_enable(stream, 1) < 0) {
		php_stream_warn(stream, ProtocolError, "Failed to enable crypto");
		return -1;
	}

	return 0;
}
