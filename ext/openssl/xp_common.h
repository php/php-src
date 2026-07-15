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

#include <openssl/ssl.h>
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

/* SSL_CTX default passphrase callback: supplies the "passphrase" context option. */
int php_openssl_passwd_callback(char *buf, int num, int verify, void *data);

/* Load the local_cert chain and local_pk private key onto the SSL_CTX. The caller
 * must have installed the passphrase callback for an encrypted key. */
zend_result php_openssl_set_local_cert(SSL_CTX *ctx, php_stream *stream);

/* SSL_CTX verify callback: honours allow_self_signed and verify_depth. */
int php_openssl_verify_callback(int preverify_ok, X509_STORE_CTX *ctx);

/* Configure peer certificate verification on the SSL_CTX from the cafile/capath
 * context options (is_client selects client- vs server-side behaviour). */
zend_result php_openssl_enable_peer_verification(SSL_CTX *ctx, php_stream *stream, bool is_client);
void php_openssl_disable_peer_verification(SSL_CTX *ctx, php_stream *stream);

/* Add the negotiated cipher_name/bits/version to a crypto metadata array. */
void php_openssl_add_crypto_cipher(zval *crypto, SSL *ssl);

#endif /* PHP_OPENSSL_XP_COMMON_H */
