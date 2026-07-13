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

/* Code shared between the tls:// (xp_ssl.c) and dtls:// (xp_dtls.c) transports. */

#ifndef PHP_OPENSSL_XP_COMMON_H
#define PHP_OPENSSL_XP_COMMON_H

#include "php.h"
#include "php_network.h"

#include <openssl/x509.h>

/* Match the peer certificate against the peer_fingerprint context option. */
bool php_openssl_x509_fingerprint_is_equal(php_stream *stream, X509 *peer,
		const char *method, const zend_string *expected);
bool php_openssl_x509_fingerprint_match(php_stream *stream, X509 *peer, const zval *val);

/* Host from the stream URL, used as the default peer name for verification. */
char *php_openssl_get_url_name(const char *resourcename, size_t resourcenamelen,
		int is_persistent, php_stream_context *context);

/* Turn on crypto right after the base transport connected (enable_on_connect). */
int php_openssl_setup_crypto_on_connect(php_stream *stream,
		php_stream_xport_crypt_method_t method);

#endif /* PHP_OPENSSL_XP_COMMON_H */
