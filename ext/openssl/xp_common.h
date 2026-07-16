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

/* Userland session-resumption callbacks, shared (refcounted) by a listener and
 * its accepted connections. */
typedef struct _php_openssl_session_callbacks_t {
	int refcount;
	zend_fcall_info_cache new_cb;
	zend_fcall_info_cache get_cb;
	zend_fcall_info_cache remove_cb;
} php_openssl_session_callbacks_t;

enum php_openssl_session_callback_type {
	PHP_OPENSSL_NEW_CB,
	PHP_OPENSSL_GET_CB,
	PHP_OPENSSL_REMOVE_CB,
};

/* ex-data indices on the SSL_CTX so the shared session callbacks can reach the
 * stream and the callbacks struct without knowing the transport's netstream type. */
int php_openssl_get_ctx_session_callbacks_index(void);
int php_openssl_get_ctx_stream_index(void);

/* The OpenSSL session cache callbacks (data comes from the CTX ex-data). */
int php_openssl_session_new_cb(SSL *ssl, SSL_SESSION *session);
SSL_SESSION *php_openssl_session_get_cb(SSL *ssl, const unsigned char *session_id, int session_id_len, int *copy);
void php_openssl_session_remove_cb(SSL_CTX *ctx, SSL_SESSION *session);

/* Validate a session_*_cb callable and store it in *callbacks (allocating it). */
zend_result php_openssl_validate_and_allocate_session_callback(php_stream *stream,
		php_openssl_session_callbacks_t **callbacks, const zval *callable,
		enum php_openssl_session_callback_type cb_type, bool is_persistent);

#endif /* PHP_OPENSSL_XP_COMMON_H */
