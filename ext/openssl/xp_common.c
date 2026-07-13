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
