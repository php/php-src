/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2018 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Frank Denis <jedisct1@php.net>                              |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_LIBSODIUM_H
#define PHP_LIBSODIUM_H

extern zend_module_entry sodium_module_entry;
#define phpext_sodium_ptr &sodium_module_entry

#define PHP_SODIUM_VERSION PHP_VERSION

#ifdef ZTS
# include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(sodium);
PHP_MSHUTDOWN_FUNCTION(sodium);
PHP_RINIT_FUNCTION(sodium);
PHP_RSHUTDOWN_FUNCTION(sodium);
PHP_MINFO_FUNCTION(sodium);

PHP_FUNCTION(sodium_add);
PHP_FUNCTION(sodium_base642bin);
PHP_FUNCTION(sodium_bin2base64);
PHP_FUNCTION(sodium_bin2hex);
PHP_FUNCTION(sodium_compare);
PHP_FUNCTION(sodium_crypto_aead_aes256gcm_decrypt);
PHP_FUNCTION(sodium_crypto_aead_aes256gcm_encrypt);
PHP_FUNCTION(sodium_crypto_aead_aes256gcm_is_available);
PHP_FUNCTION(sodium_crypto_aead_aes256gcm_keygen);
PHP_FUNCTION(sodium_crypto_aead_chacha20poly1305_decrypt);
PHP_FUNCTION(sodium_crypto_aead_chacha20poly1305_encrypt);
PHP_FUNCTION(sodium_crypto_aead_chacha20poly1305_ietf_decrypt);
PHP_FUNCTION(sodium_crypto_aead_chacha20poly1305_ietf_encrypt);
PHP_FUNCTION(sodium_crypto_aead_chacha20poly1305_ietf_keygen);
PHP_FUNCTION(sodium_crypto_aead_chacha20poly1305_keygen);
PHP_FUNCTION(sodium_crypto_aead_xchacha20poly1305_ietf_decrypt);
PHP_FUNCTION(sodium_crypto_aead_xchacha20poly1305_ietf_encrypt);
PHP_FUNCTION(sodium_crypto_aead_xchacha20poly1305_ietf_keygen);
PHP_FUNCTION(sodium_crypto_auth);
PHP_FUNCTION(sodium_crypto_auth_keygen);
PHP_FUNCTION(sodium_crypto_auth_verify);
PHP_FUNCTION(sodium_crypto_box);
PHP_FUNCTION(sodium_crypto_box_keypair);
PHP_FUNCTION(sodium_crypto_box_keypair_from_secretkey_and_publickey);
PHP_FUNCTION(sodium_crypto_box_open);
PHP_FUNCTION(sodium_crypto_box_publickey);
PHP_FUNCTION(sodium_crypto_box_publickey_from_secretkey);
PHP_FUNCTION(sodium_crypto_box_seal);
PHP_FUNCTION(sodium_crypto_box_seal_open);
PHP_FUNCTION(sodium_crypto_box_secretkey);
PHP_FUNCTION(sodium_crypto_box_seed_keypair);
PHP_FUNCTION(sodium_crypto_generichash);
PHP_FUNCTION(sodium_crypto_generichash_final);
PHP_FUNCTION(sodium_crypto_generichash_init);
PHP_FUNCTION(sodium_crypto_generichash_keygen);
PHP_FUNCTION(sodium_crypto_generichash_update);
PHP_FUNCTION(sodium_crypto_kdf_derive_from_key);
PHP_FUNCTION(sodium_crypto_kdf_keygen);
PHP_FUNCTION(sodium_crypto_kx_client_session_keys);
PHP_FUNCTION(sodium_crypto_kx_keypair);
PHP_FUNCTION(sodium_crypto_kx_publickey);
PHP_FUNCTION(sodium_crypto_kx_secretkey);
PHP_FUNCTION(sodium_crypto_kx_seed_keypair);
PHP_FUNCTION(sodium_crypto_kx_server_session_keys);
PHP_FUNCTION(sodium_crypto_pwhash);
PHP_FUNCTION(sodium_crypto_pwhash_scryptsalsa208sha256);
PHP_FUNCTION(sodium_crypto_pwhash_scryptsalsa208sha256_str);
PHP_FUNCTION(sodium_crypto_pwhash_scryptsalsa208sha256_str_verify);
PHP_FUNCTION(sodium_crypto_pwhash_str);
PHP_FUNCTION(sodium_crypto_pwhash_str_needs_rehash);
PHP_FUNCTION(sodium_crypto_pwhash_str_verify);
PHP_FUNCTION(sodium_crypto_scalarmult);
PHP_FUNCTION(sodium_crypto_scalarmult_base);
PHP_FUNCTION(sodium_crypto_secretbox);
PHP_FUNCTION(sodium_crypto_secretbox_keygen);
PHP_FUNCTION(sodium_crypto_secretbox_open);
PHP_FUNCTION(sodium_crypto_secretstream_xchacha20poly1305_keygen);
PHP_FUNCTION(sodium_crypto_secretstream_xchacha20poly1305_init_push);
PHP_FUNCTION(sodium_crypto_secretstream_xchacha20poly1305_push);
PHP_FUNCTION(sodium_crypto_secretstream_xchacha20poly1305_init_pull);
PHP_FUNCTION(sodium_crypto_secretstream_xchacha20poly1305_pull);
PHP_FUNCTION(sodium_crypto_secretstream_xchacha20poly1305_rekey);
PHP_FUNCTION(sodium_crypto_shorthash);
PHP_FUNCTION(sodium_crypto_shorthash_keygen);
PHP_FUNCTION(sodium_crypto_sign);
PHP_FUNCTION(sodium_crypto_sign_detached);
PHP_FUNCTION(sodium_crypto_sign_ed25519_pk_to_curve25519);
PHP_FUNCTION(sodium_crypto_sign_ed25519_sk_to_curve25519);
PHP_FUNCTION(sodium_crypto_sign_keypair);
PHP_FUNCTION(sodium_crypto_sign_keypair_from_secretkey_and_publickey);
PHP_FUNCTION(sodium_crypto_sign_open);
PHP_FUNCTION(sodium_crypto_sign_publickey);
PHP_FUNCTION(sodium_crypto_sign_publickey_from_secretkey);
PHP_FUNCTION(sodium_crypto_sign_secretkey);
PHP_FUNCTION(sodium_crypto_sign_seed_keypair);
PHP_FUNCTION(sodium_crypto_sign_verify_detached);
PHP_FUNCTION(sodium_crypto_stream);
PHP_FUNCTION(sodium_crypto_stream_keygen);
PHP_FUNCTION(sodium_crypto_stream_xor);
PHP_FUNCTION(sodium_hex2bin);
PHP_FUNCTION(sodium_increment);
PHP_FUNCTION(sodium_memcmp);
PHP_FUNCTION(sodium_memzero);
PHP_FUNCTION(sodium_pad);
PHP_FUNCTION(sodium_unpad);

#endif	/* PHP_LIBSODIUM_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
