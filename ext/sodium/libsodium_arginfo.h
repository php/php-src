/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 89cbb449ee6146dc8d50ba4bb1e76f83444a2db2 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_aead_aes256gcm_is_available, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#if defined(HAVE_AESGCM)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_sodium_crypto_aead_aes256gcm_decrypt, 0, 4, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, ciphertext, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, additional_data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, nonce, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_AESGCM)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_aead_aes256gcm_encrypt, 0, 4, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, message, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, additional_data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, nonce, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_AESGCM)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_aead_aes256gcm_keygen, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(crypto_aead_aegis128l_KEYBYTES)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_sodium_crypto_aead_aegis128l_decrypt, 0, 4, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, ciphertext, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, additional_data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, nonce, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(crypto_aead_aegis128l_KEYBYTES)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_aead_aegis128l_encrypt, 0, 4, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, message, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, additional_data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, nonce, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(crypto_aead_aegis128l_KEYBYTES)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_aead_aegis128l_keygen, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(crypto_aead_aegis256_KEYBYTES)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_sodium_crypto_aead_aegis256_decrypt, 0, 4, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, ciphertext, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, additional_data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, nonce, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(crypto_aead_aegis256_KEYBYTES)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_aead_aegis256_encrypt, 0, 4, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, message, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, additional_data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, nonce, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(crypto_aead_aegis256_KEYBYTES)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_aead_aegis256_keygen, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_sodium_crypto_aead_chacha20poly1305_decrypt, 0, 4, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, ciphertext, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, additional_data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, nonce, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_aead_chacha20poly1305_encrypt, 0, 4, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, message, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, additional_data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, nonce, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_aead_chacha20poly1305_keygen, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_sodium_crypto_aead_chacha20poly1305_ietf_decrypt arginfo_sodium_crypto_aead_chacha20poly1305_decrypt

#define arginfo_sodium_crypto_aead_chacha20poly1305_ietf_encrypt arginfo_sodium_crypto_aead_chacha20poly1305_encrypt

#define arginfo_sodium_crypto_aead_chacha20poly1305_ietf_keygen arginfo_sodium_crypto_aead_chacha20poly1305_keygen

#if defined(crypto_aead_xchacha20poly1305_IETF_NPUBBYTES)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_sodium_crypto_aead_xchacha20poly1305_ietf_decrypt, 0, 4, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, ciphertext, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, additional_data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, nonce, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(crypto_aead_xchacha20poly1305_IETF_NPUBBYTES)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_aead_xchacha20poly1305_ietf_keygen, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(crypto_aead_xchacha20poly1305_IETF_NPUBBYTES)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_aead_xchacha20poly1305_ietf_encrypt, 0, 4, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, message, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, additional_data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, nonce, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_auth, 0, 2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, message, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_sodium_crypto_auth_keygen arginfo_sodium_crypto_aead_chacha20poly1305_keygen

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_auth_verify, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, mac, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, message, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_box, 0, 3, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, message, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, nonce, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, key_pair, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_sodium_crypto_box_keypair arginfo_sodium_crypto_aead_chacha20poly1305_keygen

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_box_seed_keypair, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, seed, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_box_keypair_from_secretkey_and_publickey, 0, 2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, secret_key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, public_key, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_sodium_crypto_box_open, 0, 3, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, ciphertext, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, nonce, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, key_pair, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_box_publickey, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, key_pair, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_box_publickey_from_secretkey, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, secret_key, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_box_seal, 0, 2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, message, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, public_key, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_sodium_crypto_box_seal_open, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, ciphertext, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, key_pair, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_sodium_crypto_box_secretkey arginfo_sodium_crypto_box_publickey

#if defined(crypto_core_ristretto255_HASHBYTES)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_core_ristretto255_add, 0, 2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, p, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, q, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(crypto_core_ristretto255_HASHBYTES)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_core_ristretto255_from_hash, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, s, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(crypto_core_ristretto255_HASHBYTES)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_core_ristretto255_is_valid_point, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, s, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(crypto_core_ristretto255_HASHBYTES)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_core_ristretto255_random, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(crypto_core_ristretto255_HASHBYTES)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_core_ristretto255_scalar_add, 0, 2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, x, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, y, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(crypto_core_ristretto255_HASHBYTES)
#define arginfo_sodium_crypto_core_ristretto255_scalar_complement arginfo_sodium_crypto_core_ristretto255_from_hash
#endif

#if defined(crypto_core_ristretto255_HASHBYTES)
#define arginfo_sodium_crypto_core_ristretto255_scalar_invert arginfo_sodium_crypto_core_ristretto255_from_hash
#endif

#if defined(crypto_core_ristretto255_HASHBYTES)
#define arginfo_sodium_crypto_core_ristretto255_scalar_mul arginfo_sodium_crypto_core_ristretto255_scalar_add
#endif

#if defined(crypto_core_ristretto255_HASHBYTES)
#define arginfo_sodium_crypto_core_ristretto255_scalar_negate arginfo_sodium_crypto_core_ristretto255_from_hash
#endif

#if defined(crypto_core_ristretto255_HASHBYTES)
#define arginfo_sodium_crypto_core_ristretto255_scalar_random arginfo_sodium_crypto_core_ristretto255_random
#endif

#if defined(crypto_core_ristretto255_HASHBYTES)
#define arginfo_sodium_crypto_core_ristretto255_scalar_reduce arginfo_sodium_crypto_core_ristretto255_from_hash
#endif

#if defined(crypto_core_ristretto255_HASHBYTES)
#define arginfo_sodium_crypto_core_ristretto255_scalar_sub arginfo_sodium_crypto_core_ristretto255_scalar_add
#endif

#if defined(crypto_core_ristretto255_HASHBYTES)
#define arginfo_sodium_crypto_core_ristretto255_sub arginfo_sodium_crypto_core_ristretto255_add
#endif

#define arginfo_sodium_crypto_kx_keypair arginfo_sodium_crypto_aead_chacha20poly1305_keygen

#define arginfo_sodium_crypto_kx_publickey arginfo_sodium_crypto_box_publickey

#define arginfo_sodium_crypto_kx_secretkey arginfo_sodium_crypto_box_publickey

#define arginfo_sodium_crypto_kx_seed_keypair arginfo_sodium_crypto_box_seed_keypair

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_kx_client_session_keys, 0, 2, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, client_key_pair, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, server_key, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_kx_server_session_keys, 0, 2, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, server_key_pair, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, client_key, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_generichash, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, message, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, key, IS_STRING, 0, "\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 0, "SODIUM_CRYPTO_GENERICHASH_BYTES")
ZEND_END_ARG_INFO()

#define arginfo_sodium_crypto_generichash_keygen arginfo_sodium_crypto_aead_chacha20poly1305_keygen

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_generichash_init, 0, 0, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, key, IS_STRING, 0, "\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 0, "SODIUM_CRYPTO_GENERICHASH_BYTES")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_generichash_update, 0, 2, IS_TRUE, 0)
	ZEND_ARG_TYPE_INFO(1, state, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, message, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_generichash_final, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(1, state, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 0, "SODIUM_CRYPTO_GENERICHASH_BYTES")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_kdf_derive_from_key, 0, 4, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, subkey_length, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, subkey_id, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, context, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_sodium_crypto_kdf_keygen arginfo_sodium_crypto_aead_chacha20poly1305_keygen

#if defined(crypto_pwhash_SALTBYTES)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_pwhash, 0, 5, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, password, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, salt, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, opslimit, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, memlimit, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, algo, IS_LONG, 0, "SODIUM_CRYPTO_PWHASH_ALG_DEFAULT")
ZEND_END_ARG_INFO()
#endif

#if defined(crypto_pwhash_SALTBYTES)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_pwhash_str, 0, 3, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, password, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, opslimit, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, memlimit, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(crypto_pwhash_SALTBYTES)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_pwhash_str_verify, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, hash, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, password, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if SODIUM_LIBRARY_VERSION_MAJOR > 9 || (SODIUM_LIBRARY_VERSION_MAJOR == 9 && SODIUM_LIBRARY_VERSION_MINOR >= 6)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_pwhash_str_needs_rehash, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, password, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, opslimit, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, memlimit, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(crypto_pwhash_scryptsalsa208sha256_SALTBYTES)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_pwhash_scryptsalsa208sha256, 0, 5, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, password, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, salt, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, opslimit, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, memlimit, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(crypto_pwhash_scryptsalsa208sha256_SALTBYTES)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_pwhash_scryptsalsa208sha256_str, 0, 3, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, password, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, opslimit, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, memlimit, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(crypto_pwhash_scryptsalsa208sha256_SALTBYTES)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_pwhash_scryptsalsa208sha256_str_verify, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, hash, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, password, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_scalarmult, 0, 2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, n, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, p, IS_STRING, 0)
ZEND_END_ARG_INFO()

#if defined(crypto_core_ristretto255_HASHBYTES)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_scalarmult_ristretto255, 0, 2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, n, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, p, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(crypto_core_ristretto255_HASHBYTES)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_scalarmult_ristretto255_base, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, n, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_secretbox, 0, 3, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, message, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, nonce, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_sodium_crypto_secretbox_keygen arginfo_sodium_crypto_aead_chacha20poly1305_keygen

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_sodium_crypto_secretbox_open, 0, 3, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, ciphertext, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, nonce, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()

#if defined(crypto_secretstream_xchacha20poly1305_ABYTES)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_secretstream_xchacha20poly1305_keygen, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(crypto_secretstream_xchacha20poly1305_ABYTES)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_secretstream_xchacha20poly1305_init_push, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(crypto_secretstream_xchacha20poly1305_ABYTES)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_secretstream_xchacha20poly1305_push, 0, 2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(1, state, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, message, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, additional_data, IS_STRING, 0, "\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, tag, IS_LONG, 0, "SODIUM_CRYPTO_SECRETSTREAM_XCHACHA20POLY1305_TAG_MESSAGE")
ZEND_END_ARG_INFO()
#endif

#if defined(crypto_secretstream_xchacha20poly1305_ABYTES)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_secretstream_xchacha20poly1305_init_pull, 0, 2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, header, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(crypto_secretstream_xchacha20poly1305_ABYTES)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_sodium_crypto_secretstream_xchacha20poly1305_pull, 0, 2, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(1, state, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, ciphertext, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, additional_data, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()
#endif

#if defined(crypto_secretstream_xchacha20poly1305_ABYTES)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_secretstream_xchacha20poly1305_rekey, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(1, state, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#define arginfo_sodium_crypto_shorthash arginfo_sodium_crypto_auth

#define arginfo_sodium_crypto_shorthash_keygen arginfo_sodium_crypto_aead_chacha20poly1305_keygen

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_sign, 0, 2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, message, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, secret_key, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_sodium_crypto_sign_detached arginfo_sodium_crypto_sign

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_sign_ed25519_pk_to_curve25519, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, public_key, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_sodium_crypto_sign_ed25519_sk_to_curve25519 arginfo_sodium_crypto_box_publickey_from_secretkey

#define arginfo_sodium_crypto_sign_keypair arginfo_sodium_crypto_aead_chacha20poly1305_keygen

#define arginfo_sodium_crypto_sign_keypair_from_secretkey_and_publickey arginfo_sodium_crypto_box_keypair_from_secretkey_and_publickey

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_sodium_crypto_sign_open, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, signed_message, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, public_key, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_sodium_crypto_sign_publickey arginfo_sodium_crypto_box_publickey

#define arginfo_sodium_crypto_sign_secretkey arginfo_sodium_crypto_box_publickey

#define arginfo_sodium_crypto_sign_publickey_from_secretkey arginfo_sodium_crypto_box_publickey_from_secretkey

#define arginfo_sodium_crypto_sign_seed_keypair arginfo_sodium_crypto_box_seed_keypair

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_sign_verify_detached, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, signature, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, message, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, public_key, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_stream, 0, 3, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, nonce, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_sodium_crypto_stream_keygen arginfo_sodium_crypto_aead_chacha20poly1305_keygen

#define arginfo_sodium_crypto_stream_xor arginfo_sodium_crypto_secretbox

#if defined(crypto_stream_xchacha20_KEYBYTES)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_stream_xchacha20, 0, 3, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, nonce, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(crypto_stream_xchacha20_KEYBYTES)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_stream_xchacha20_keygen, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(crypto_stream_xchacha20_KEYBYTES)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_stream_xchacha20_xor, 0, 3, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, message, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, nonce, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(crypto_stream_xchacha20_KEYBYTES)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_stream_xchacha20_xor_ic, 0, 4, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, message, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, nonce, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, counter, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_add, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(1, string1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string2, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_compare, 0, 2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, string1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string2, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_increment, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(1, string, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_sodium_memcmp arginfo_sodium_compare

#define arginfo_sodium_memzero arginfo_sodium_increment

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_pad, 0, 2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, block_size, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_sodium_unpad arginfo_sodium_pad

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_bin2hex, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_hex2bin, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, ignore, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()

#if defined(sodium_base64_VARIANT_ORIGINAL)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_bin2base64, 0, 2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, id, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(sodium_base64_VARIANT_ORIGINAL)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_base642bin, 0, 2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, id, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, ignore, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()
#endif

#define arginfo_sodium_crypto_scalarmult_base arginfo_sodium_crypto_box_publickey_from_secretkey


ZEND_FUNCTION(sodium_crypto_aead_aes256gcm_is_available);
#if defined(HAVE_AESGCM)
ZEND_FUNCTION(sodium_crypto_aead_aes256gcm_decrypt);
#endif
#if defined(HAVE_AESGCM)
ZEND_FUNCTION(sodium_crypto_aead_aes256gcm_encrypt);
#endif
#if defined(HAVE_AESGCM)
ZEND_FUNCTION(sodium_crypto_aead_aes256gcm_keygen);
#endif
#if defined(crypto_aead_aegis128l_KEYBYTES)
ZEND_FUNCTION(sodium_crypto_aead_aegis128l_decrypt);
#endif
#if defined(crypto_aead_aegis128l_KEYBYTES)
ZEND_FUNCTION(sodium_crypto_aead_aegis128l_encrypt);
#endif
#if defined(crypto_aead_aegis128l_KEYBYTES)
ZEND_FUNCTION(sodium_crypto_aead_aegis128l_keygen);
#endif
#if defined(crypto_aead_aegis256_KEYBYTES)
ZEND_FUNCTION(sodium_crypto_aead_aegis256_decrypt);
#endif
#if defined(crypto_aead_aegis256_KEYBYTES)
ZEND_FUNCTION(sodium_crypto_aead_aegis256_encrypt);
#endif
#if defined(crypto_aead_aegis256_KEYBYTES)
ZEND_FUNCTION(sodium_crypto_aead_aegis256_keygen);
#endif
ZEND_FUNCTION(sodium_crypto_aead_chacha20poly1305_decrypt);
ZEND_FUNCTION(sodium_crypto_aead_chacha20poly1305_encrypt);
ZEND_FUNCTION(sodium_crypto_aead_chacha20poly1305_keygen);
ZEND_FUNCTION(sodium_crypto_aead_chacha20poly1305_ietf_decrypt);
ZEND_FUNCTION(sodium_crypto_aead_chacha20poly1305_ietf_encrypt);
ZEND_FUNCTION(sodium_crypto_aead_chacha20poly1305_ietf_keygen);
#if defined(crypto_aead_xchacha20poly1305_IETF_NPUBBYTES)
ZEND_FUNCTION(sodium_crypto_aead_xchacha20poly1305_ietf_decrypt);
#endif
#if defined(crypto_aead_xchacha20poly1305_IETF_NPUBBYTES)
ZEND_FUNCTION(sodium_crypto_aead_xchacha20poly1305_ietf_keygen);
#endif
#if defined(crypto_aead_xchacha20poly1305_IETF_NPUBBYTES)
ZEND_FUNCTION(sodium_crypto_aead_xchacha20poly1305_ietf_encrypt);
#endif
ZEND_FUNCTION(sodium_crypto_auth);
ZEND_FUNCTION(sodium_crypto_auth_keygen);
ZEND_FUNCTION(sodium_crypto_auth_verify);
ZEND_FUNCTION(sodium_crypto_box);
ZEND_FUNCTION(sodium_crypto_box_keypair);
ZEND_FUNCTION(sodium_crypto_box_seed_keypair);
ZEND_FUNCTION(sodium_crypto_box_keypair_from_secretkey_and_publickey);
ZEND_FUNCTION(sodium_crypto_box_open);
ZEND_FUNCTION(sodium_crypto_box_publickey);
ZEND_FUNCTION(sodium_crypto_box_publickey_from_secretkey);
ZEND_FUNCTION(sodium_crypto_box_seal);
ZEND_FUNCTION(sodium_crypto_box_seal_open);
ZEND_FUNCTION(sodium_crypto_box_secretkey);
#if defined(crypto_core_ristretto255_HASHBYTES)
ZEND_FUNCTION(sodium_crypto_core_ristretto255_add);
#endif
#if defined(crypto_core_ristretto255_HASHBYTES)
ZEND_FUNCTION(sodium_crypto_core_ristretto255_from_hash);
#endif
#if defined(crypto_core_ristretto255_HASHBYTES)
ZEND_FUNCTION(sodium_crypto_core_ristretto255_is_valid_point);
#endif
#if defined(crypto_core_ristretto255_HASHBYTES)
ZEND_FUNCTION(sodium_crypto_core_ristretto255_random);
#endif
#if defined(crypto_core_ristretto255_HASHBYTES)
ZEND_FUNCTION(sodium_crypto_core_ristretto255_scalar_add);
#endif
#if defined(crypto_core_ristretto255_HASHBYTES)
ZEND_FUNCTION(sodium_crypto_core_ristretto255_scalar_complement);
#endif
#if defined(crypto_core_ristretto255_HASHBYTES)
ZEND_FUNCTION(sodium_crypto_core_ristretto255_scalar_invert);
#endif
#if defined(crypto_core_ristretto255_HASHBYTES)
ZEND_FUNCTION(sodium_crypto_core_ristretto255_scalar_mul);
#endif
#if defined(crypto_core_ristretto255_HASHBYTES)
ZEND_FUNCTION(sodium_crypto_core_ristretto255_scalar_negate);
#endif
#if defined(crypto_core_ristretto255_HASHBYTES)
ZEND_FUNCTION(sodium_crypto_core_ristretto255_scalar_random);
#endif
#if defined(crypto_core_ristretto255_HASHBYTES)
ZEND_FUNCTION(sodium_crypto_core_ristretto255_scalar_reduce);
#endif
#if defined(crypto_core_ristretto255_HASHBYTES)
ZEND_FUNCTION(sodium_crypto_core_ristretto255_scalar_sub);
#endif
#if defined(crypto_core_ristretto255_HASHBYTES)
ZEND_FUNCTION(sodium_crypto_core_ristretto255_sub);
#endif
ZEND_FUNCTION(sodium_crypto_kx_keypair);
ZEND_FUNCTION(sodium_crypto_kx_publickey);
ZEND_FUNCTION(sodium_crypto_kx_secretkey);
ZEND_FUNCTION(sodium_crypto_kx_seed_keypair);
ZEND_FUNCTION(sodium_crypto_kx_client_session_keys);
ZEND_FUNCTION(sodium_crypto_kx_server_session_keys);
ZEND_FUNCTION(sodium_crypto_generichash);
ZEND_FUNCTION(sodium_crypto_generichash_keygen);
ZEND_FUNCTION(sodium_crypto_generichash_init);
ZEND_FUNCTION(sodium_crypto_generichash_update);
ZEND_FUNCTION(sodium_crypto_generichash_final);
ZEND_FUNCTION(sodium_crypto_kdf_derive_from_key);
ZEND_FUNCTION(sodium_crypto_kdf_keygen);
#if defined(crypto_pwhash_SALTBYTES)
ZEND_FUNCTION(sodium_crypto_pwhash);
#endif
#if defined(crypto_pwhash_SALTBYTES)
ZEND_FUNCTION(sodium_crypto_pwhash_str);
#endif
#if defined(crypto_pwhash_SALTBYTES)
ZEND_FUNCTION(sodium_crypto_pwhash_str_verify);
#endif
#if SODIUM_LIBRARY_VERSION_MAJOR > 9 || (SODIUM_LIBRARY_VERSION_MAJOR == 9 && SODIUM_LIBRARY_VERSION_MINOR >= 6)
ZEND_FUNCTION(sodium_crypto_pwhash_str_needs_rehash);
#endif
#if defined(crypto_pwhash_scryptsalsa208sha256_SALTBYTES)
ZEND_FUNCTION(sodium_crypto_pwhash_scryptsalsa208sha256);
#endif
#if defined(crypto_pwhash_scryptsalsa208sha256_SALTBYTES)
ZEND_FUNCTION(sodium_crypto_pwhash_scryptsalsa208sha256_str);
#endif
#if defined(crypto_pwhash_scryptsalsa208sha256_SALTBYTES)
ZEND_FUNCTION(sodium_crypto_pwhash_scryptsalsa208sha256_str_verify);
#endif
ZEND_FUNCTION(sodium_crypto_scalarmult);
#if defined(crypto_core_ristretto255_HASHBYTES)
ZEND_FUNCTION(sodium_crypto_scalarmult_ristretto255);
#endif
#if defined(crypto_core_ristretto255_HASHBYTES)
ZEND_FUNCTION(sodium_crypto_scalarmult_ristretto255_base);
#endif
ZEND_FUNCTION(sodium_crypto_secretbox);
ZEND_FUNCTION(sodium_crypto_secretbox_keygen);
ZEND_FUNCTION(sodium_crypto_secretbox_open);
#if defined(crypto_secretstream_xchacha20poly1305_ABYTES)
ZEND_FUNCTION(sodium_crypto_secretstream_xchacha20poly1305_keygen);
#endif
#if defined(crypto_secretstream_xchacha20poly1305_ABYTES)
ZEND_FUNCTION(sodium_crypto_secretstream_xchacha20poly1305_init_push);
#endif
#if defined(crypto_secretstream_xchacha20poly1305_ABYTES)
ZEND_FUNCTION(sodium_crypto_secretstream_xchacha20poly1305_push);
#endif
#if defined(crypto_secretstream_xchacha20poly1305_ABYTES)
ZEND_FUNCTION(sodium_crypto_secretstream_xchacha20poly1305_init_pull);
#endif
#if defined(crypto_secretstream_xchacha20poly1305_ABYTES)
ZEND_FUNCTION(sodium_crypto_secretstream_xchacha20poly1305_pull);
#endif
#if defined(crypto_secretstream_xchacha20poly1305_ABYTES)
ZEND_FUNCTION(sodium_crypto_secretstream_xchacha20poly1305_rekey);
#endif
ZEND_FUNCTION(sodium_crypto_shorthash);
ZEND_FUNCTION(sodium_crypto_shorthash_keygen);
ZEND_FUNCTION(sodium_crypto_sign);
ZEND_FUNCTION(sodium_crypto_sign_detached);
ZEND_FUNCTION(sodium_crypto_sign_ed25519_pk_to_curve25519);
ZEND_FUNCTION(sodium_crypto_sign_ed25519_sk_to_curve25519);
ZEND_FUNCTION(sodium_crypto_sign_keypair);
ZEND_FUNCTION(sodium_crypto_sign_keypair_from_secretkey_and_publickey);
ZEND_FUNCTION(sodium_crypto_sign_open);
ZEND_FUNCTION(sodium_crypto_sign_publickey);
ZEND_FUNCTION(sodium_crypto_sign_secretkey);
ZEND_FUNCTION(sodium_crypto_sign_publickey_from_secretkey);
ZEND_FUNCTION(sodium_crypto_sign_seed_keypair);
ZEND_FUNCTION(sodium_crypto_sign_verify_detached);
ZEND_FUNCTION(sodium_crypto_stream);
ZEND_FUNCTION(sodium_crypto_stream_keygen);
ZEND_FUNCTION(sodium_crypto_stream_xor);
#if defined(crypto_stream_xchacha20_KEYBYTES)
ZEND_FUNCTION(sodium_crypto_stream_xchacha20);
#endif
#if defined(crypto_stream_xchacha20_KEYBYTES)
ZEND_FUNCTION(sodium_crypto_stream_xchacha20_keygen);
#endif
#if defined(crypto_stream_xchacha20_KEYBYTES)
ZEND_FUNCTION(sodium_crypto_stream_xchacha20_xor);
#endif
#if defined(crypto_stream_xchacha20_KEYBYTES)
ZEND_FUNCTION(sodium_crypto_stream_xchacha20_xor_ic);
#endif
ZEND_FUNCTION(sodium_add);
ZEND_FUNCTION(sodium_compare);
ZEND_FUNCTION(sodium_increment);
ZEND_FUNCTION(sodium_memcmp);
ZEND_FUNCTION(sodium_memzero);
ZEND_FUNCTION(sodium_pad);
ZEND_FUNCTION(sodium_unpad);
ZEND_FUNCTION(sodium_bin2hex);
ZEND_FUNCTION(sodium_hex2bin);
#if defined(sodium_base64_VARIANT_ORIGINAL)
ZEND_FUNCTION(sodium_bin2base64);
#endif
#if defined(sodium_base64_VARIANT_ORIGINAL)
ZEND_FUNCTION(sodium_base642bin);
#endif


static const zend_function_entry ext_functions[] = {
	ZEND_FE(sodium_crypto_aead_aes256gcm_is_available, arginfo_sodium_crypto_aead_aes256gcm_is_available)
#if defined(HAVE_AESGCM)
	ZEND_FE(sodium_crypto_aead_aes256gcm_decrypt, arginfo_sodium_crypto_aead_aes256gcm_decrypt)
#endif
#if defined(HAVE_AESGCM)
	ZEND_FE(sodium_crypto_aead_aes256gcm_encrypt, arginfo_sodium_crypto_aead_aes256gcm_encrypt)
#endif
#if defined(HAVE_AESGCM)
	ZEND_FE(sodium_crypto_aead_aes256gcm_keygen, arginfo_sodium_crypto_aead_aes256gcm_keygen)
#endif
#if defined(crypto_aead_aegis128l_KEYBYTES)
	ZEND_FE(sodium_crypto_aead_aegis128l_decrypt, arginfo_sodium_crypto_aead_aegis128l_decrypt)
#endif
#if defined(crypto_aead_aegis128l_KEYBYTES)
	ZEND_FE(sodium_crypto_aead_aegis128l_encrypt, arginfo_sodium_crypto_aead_aegis128l_encrypt)
#endif
#if defined(crypto_aead_aegis128l_KEYBYTES)
	ZEND_FE(sodium_crypto_aead_aegis128l_keygen, arginfo_sodium_crypto_aead_aegis128l_keygen)
#endif
#if defined(crypto_aead_aegis256_KEYBYTES)
	ZEND_FE(sodium_crypto_aead_aegis256_decrypt, arginfo_sodium_crypto_aead_aegis256_decrypt)
#endif
#if defined(crypto_aead_aegis256_KEYBYTES)
	ZEND_FE(sodium_crypto_aead_aegis256_encrypt, arginfo_sodium_crypto_aead_aegis256_encrypt)
#endif
#if defined(crypto_aead_aegis256_KEYBYTES)
	ZEND_FE(sodium_crypto_aead_aegis256_keygen, arginfo_sodium_crypto_aead_aegis256_keygen)
#endif
	ZEND_FE(sodium_crypto_aead_chacha20poly1305_decrypt, arginfo_sodium_crypto_aead_chacha20poly1305_decrypt)
	ZEND_FE(sodium_crypto_aead_chacha20poly1305_encrypt, arginfo_sodium_crypto_aead_chacha20poly1305_encrypt)
	ZEND_FE(sodium_crypto_aead_chacha20poly1305_keygen, arginfo_sodium_crypto_aead_chacha20poly1305_keygen)
	ZEND_FE(sodium_crypto_aead_chacha20poly1305_ietf_decrypt, arginfo_sodium_crypto_aead_chacha20poly1305_ietf_decrypt)
	ZEND_FE(sodium_crypto_aead_chacha20poly1305_ietf_encrypt, arginfo_sodium_crypto_aead_chacha20poly1305_ietf_encrypt)
	ZEND_FE(sodium_crypto_aead_chacha20poly1305_ietf_keygen, arginfo_sodium_crypto_aead_chacha20poly1305_ietf_keygen)
#if defined(crypto_aead_xchacha20poly1305_IETF_NPUBBYTES)
	ZEND_FE(sodium_crypto_aead_xchacha20poly1305_ietf_decrypt, arginfo_sodium_crypto_aead_xchacha20poly1305_ietf_decrypt)
#endif
#if defined(crypto_aead_xchacha20poly1305_IETF_NPUBBYTES)
	ZEND_FE(sodium_crypto_aead_xchacha20poly1305_ietf_keygen, arginfo_sodium_crypto_aead_xchacha20poly1305_ietf_keygen)
#endif
#if defined(crypto_aead_xchacha20poly1305_IETF_NPUBBYTES)
	ZEND_FE(sodium_crypto_aead_xchacha20poly1305_ietf_encrypt, arginfo_sodium_crypto_aead_xchacha20poly1305_ietf_encrypt)
#endif
	ZEND_FE(sodium_crypto_auth, arginfo_sodium_crypto_auth)
	ZEND_FE(sodium_crypto_auth_keygen, arginfo_sodium_crypto_auth_keygen)
	ZEND_FE(sodium_crypto_auth_verify, arginfo_sodium_crypto_auth_verify)
	ZEND_FE(sodium_crypto_box, arginfo_sodium_crypto_box)
	ZEND_FE(sodium_crypto_box_keypair, arginfo_sodium_crypto_box_keypair)
	ZEND_FE(sodium_crypto_box_seed_keypair, arginfo_sodium_crypto_box_seed_keypair)
	ZEND_FE(sodium_crypto_box_keypair_from_secretkey_and_publickey, arginfo_sodium_crypto_box_keypair_from_secretkey_and_publickey)
	ZEND_FE(sodium_crypto_box_open, arginfo_sodium_crypto_box_open)
	ZEND_FE(sodium_crypto_box_publickey, arginfo_sodium_crypto_box_publickey)
	ZEND_FE(sodium_crypto_box_publickey_from_secretkey, arginfo_sodium_crypto_box_publickey_from_secretkey)
	ZEND_FE(sodium_crypto_box_seal, arginfo_sodium_crypto_box_seal)
	ZEND_FE(sodium_crypto_box_seal_open, arginfo_sodium_crypto_box_seal_open)
	ZEND_FE(sodium_crypto_box_secretkey, arginfo_sodium_crypto_box_secretkey)
#if defined(crypto_core_ristretto255_HASHBYTES)
	ZEND_FE(sodium_crypto_core_ristretto255_add, arginfo_sodium_crypto_core_ristretto255_add)
#endif
#if defined(crypto_core_ristretto255_HASHBYTES)
	ZEND_FE(sodium_crypto_core_ristretto255_from_hash, arginfo_sodium_crypto_core_ristretto255_from_hash)
#endif
#if defined(crypto_core_ristretto255_HASHBYTES)
	ZEND_FE(sodium_crypto_core_ristretto255_is_valid_point, arginfo_sodium_crypto_core_ristretto255_is_valid_point)
#endif
#if defined(crypto_core_ristretto255_HASHBYTES)
	ZEND_FE(sodium_crypto_core_ristretto255_random, arginfo_sodium_crypto_core_ristretto255_random)
#endif
#if defined(crypto_core_ristretto255_HASHBYTES)
	ZEND_FE(sodium_crypto_core_ristretto255_scalar_add, arginfo_sodium_crypto_core_ristretto255_scalar_add)
#endif
#if defined(crypto_core_ristretto255_HASHBYTES)
	ZEND_FE(sodium_crypto_core_ristretto255_scalar_complement, arginfo_sodium_crypto_core_ristretto255_scalar_complement)
#endif
#if defined(crypto_core_ristretto255_HASHBYTES)
	ZEND_FE(sodium_crypto_core_ristretto255_scalar_invert, arginfo_sodium_crypto_core_ristretto255_scalar_invert)
#endif
#if defined(crypto_core_ristretto255_HASHBYTES)
	ZEND_FE(sodium_crypto_core_ristretto255_scalar_mul, arginfo_sodium_crypto_core_ristretto255_scalar_mul)
#endif
#if defined(crypto_core_ristretto255_HASHBYTES)
	ZEND_FE(sodium_crypto_core_ristretto255_scalar_negate, arginfo_sodium_crypto_core_ristretto255_scalar_negate)
#endif
#if defined(crypto_core_ristretto255_HASHBYTES)
	ZEND_FE(sodium_crypto_core_ristretto255_scalar_random, arginfo_sodium_crypto_core_ristretto255_scalar_random)
#endif
#if defined(crypto_core_ristretto255_HASHBYTES)
	ZEND_FE(sodium_crypto_core_ristretto255_scalar_reduce, arginfo_sodium_crypto_core_ristretto255_scalar_reduce)
#endif
#if defined(crypto_core_ristretto255_HASHBYTES)
	ZEND_FE(sodium_crypto_core_ristretto255_scalar_sub, arginfo_sodium_crypto_core_ristretto255_scalar_sub)
#endif
#if defined(crypto_core_ristretto255_HASHBYTES)
	ZEND_FE(sodium_crypto_core_ristretto255_sub, arginfo_sodium_crypto_core_ristretto255_sub)
#endif
	ZEND_FE(sodium_crypto_kx_keypair, arginfo_sodium_crypto_kx_keypair)
	ZEND_FE(sodium_crypto_kx_publickey, arginfo_sodium_crypto_kx_publickey)
	ZEND_FE(sodium_crypto_kx_secretkey, arginfo_sodium_crypto_kx_secretkey)
	ZEND_FE(sodium_crypto_kx_seed_keypair, arginfo_sodium_crypto_kx_seed_keypair)
	ZEND_FE(sodium_crypto_kx_client_session_keys, arginfo_sodium_crypto_kx_client_session_keys)
	ZEND_FE(sodium_crypto_kx_server_session_keys, arginfo_sodium_crypto_kx_server_session_keys)
	ZEND_FE(sodium_crypto_generichash, arginfo_sodium_crypto_generichash)
	ZEND_FE(sodium_crypto_generichash_keygen, arginfo_sodium_crypto_generichash_keygen)
	ZEND_FE(sodium_crypto_generichash_init, arginfo_sodium_crypto_generichash_init)
	ZEND_FE(sodium_crypto_generichash_update, arginfo_sodium_crypto_generichash_update)
	ZEND_FE(sodium_crypto_generichash_final, arginfo_sodium_crypto_generichash_final)
	ZEND_FE(sodium_crypto_kdf_derive_from_key, arginfo_sodium_crypto_kdf_derive_from_key)
	ZEND_FE(sodium_crypto_kdf_keygen, arginfo_sodium_crypto_kdf_keygen)
#if defined(crypto_pwhash_SALTBYTES)
	ZEND_FE(sodium_crypto_pwhash, arginfo_sodium_crypto_pwhash)
#endif
#if defined(crypto_pwhash_SALTBYTES)
	ZEND_FE(sodium_crypto_pwhash_str, arginfo_sodium_crypto_pwhash_str)
#endif
#if defined(crypto_pwhash_SALTBYTES)
	ZEND_FE(sodium_crypto_pwhash_str_verify, arginfo_sodium_crypto_pwhash_str_verify)
#endif
#if SODIUM_LIBRARY_VERSION_MAJOR > 9 || (SODIUM_LIBRARY_VERSION_MAJOR == 9 && SODIUM_LIBRARY_VERSION_MINOR >= 6)
	ZEND_FE(sodium_crypto_pwhash_str_needs_rehash, arginfo_sodium_crypto_pwhash_str_needs_rehash)
#endif
#if defined(crypto_pwhash_scryptsalsa208sha256_SALTBYTES)
	ZEND_FE(sodium_crypto_pwhash_scryptsalsa208sha256, arginfo_sodium_crypto_pwhash_scryptsalsa208sha256)
#endif
#if defined(crypto_pwhash_scryptsalsa208sha256_SALTBYTES)
	ZEND_FE(sodium_crypto_pwhash_scryptsalsa208sha256_str, arginfo_sodium_crypto_pwhash_scryptsalsa208sha256_str)
#endif
#if defined(crypto_pwhash_scryptsalsa208sha256_SALTBYTES)
	ZEND_FE(sodium_crypto_pwhash_scryptsalsa208sha256_str_verify, arginfo_sodium_crypto_pwhash_scryptsalsa208sha256_str_verify)
#endif
	ZEND_FE(sodium_crypto_scalarmult, arginfo_sodium_crypto_scalarmult)
#if defined(crypto_core_ristretto255_HASHBYTES)
	ZEND_FE(sodium_crypto_scalarmult_ristretto255, arginfo_sodium_crypto_scalarmult_ristretto255)
#endif
#if defined(crypto_core_ristretto255_HASHBYTES)
	ZEND_FE(sodium_crypto_scalarmult_ristretto255_base, arginfo_sodium_crypto_scalarmult_ristretto255_base)
#endif
	ZEND_FE(sodium_crypto_secretbox, arginfo_sodium_crypto_secretbox)
	ZEND_FE(sodium_crypto_secretbox_keygen, arginfo_sodium_crypto_secretbox_keygen)
	ZEND_FE(sodium_crypto_secretbox_open, arginfo_sodium_crypto_secretbox_open)
#if defined(crypto_secretstream_xchacha20poly1305_ABYTES)
	ZEND_FE(sodium_crypto_secretstream_xchacha20poly1305_keygen, arginfo_sodium_crypto_secretstream_xchacha20poly1305_keygen)
#endif
#if defined(crypto_secretstream_xchacha20poly1305_ABYTES)
	ZEND_FE(sodium_crypto_secretstream_xchacha20poly1305_init_push, arginfo_sodium_crypto_secretstream_xchacha20poly1305_init_push)
#endif
#if defined(crypto_secretstream_xchacha20poly1305_ABYTES)
	ZEND_FE(sodium_crypto_secretstream_xchacha20poly1305_push, arginfo_sodium_crypto_secretstream_xchacha20poly1305_push)
#endif
#if defined(crypto_secretstream_xchacha20poly1305_ABYTES)
	ZEND_FE(sodium_crypto_secretstream_xchacha20poly1305_init_pull, arginfo_sodium_crypto_secretstream_xchacha20poly1305_init_pull)
#endif
#if defined(crypto_secretstream_xchacha20poly1305_ABYTES)
	ZEND_FE(sodium_crypto_secretstream_xchacha20poly1305_pull, arginfo_sodium_crypto_secretstream_xchacha20poly1305_pull)
#endif
#if defined(crypto_secretstream_xchacha20poly1305_ABYTES)
	ZEND_FE(sodium_crypto_secretstream_xchacha20poly1305_rekey, arginfo_sodium_crypto_secretstream_xchacha20poly1305_rekey)
#endif
	ZEND_FE(sodium_crypto_shorthash, arginfo_sodium_crypto_shorthash)
	ZEND_FE(sodium_crypto_shorthash_keygen, arginfo_sodium_crypto_shorthash_keygen)
	ZEND_FE(sodium_crypto_sign, arginfo_sodium_crypto_sign)
	ZEND_FE(sodium_crypto_sign_detached, arginfo_sodium_crypto_sign_detached)
	ZEND_FE(sodium_crypto_sign_ed25519_pk_to_curve25519, arginfo_sodium_crypto_sign_ed25519_pk_to_curve25519)
	ZEND_FE(sodium_crypto_sign_ed25519_sk_to_curve25519, arginfo_sodium_crypto_sign_ed25519_sk_to_curve25519)
	ZEND_FE(sodium_crypto_sign_keypair, arginfo_sodium_crypto_sign_keypair)
	ZEND_FE(sodium_crypto_sign_keypair_from_secretkey_and_publickey, arginfo_sodium_crypto_sign_keypair_from_secretkey_and_publickey)
	ZEND_FE(sodium_crypto_sign_open, arginfo_sodium_crypto_sign_open)
	ZEND_FE(sodium_crypto_sign_publickey, arginfo_sodium_crypto_sign_publickey)
	ZEND_FE(sodium_crypto_sign_secretkey, arginfo_sodium_crypto_sign_secretkey)
	ZEND_FE(sodium_crypto_sign_publickey_from_secretkey, arginfo_sodium_crypto_sign_publickey_from_secretkey)
	ZEND_FE(sodium_crypto_sign_seed_keypair, arginfo_sodium_crypto_sign_seed_keypair)
	ZEND_FE(sodium_crypto_sign_verify_detached, arginfo_sodium_crypto_sign_verify_detached)
	ZEND_FE(sodium_crypto_stream, arginfo_sodium_crypto_stream)
	ZEND_FE(sodium_crypto_stream_keygen, arginfo_sodium_crypto_stream_keygen)
	ZEND_FE(sodium_crypto_stream_xor, arginfo_sodium_crypto_stream_xor)
#if defined(crypto_stream_xchacha20_KEYBYTES)
	ZEND_FE(sodium_crypto_stream_xchacha20, arginfo_sodium_crypto_stream_xchacha20)
#endif
#if defined(crypto_stream_xchacha20_KEYBYTES)
	ZEND_FE(sodium_crypto_stream_xchacha20_keygen, arginfo_sodium_crypto_stream_xchacha20_keygen)
#endif
#if defined(crypto_stream_xchacha20_KEYBYTES)
	ZEND_FE(sodium_crypto_stream_xchacha20_xor, arginfo_sodium_crypto_stream_xchacha20_xor)
#endif
#if defined(crypto_stream_xchacha20_KEYBYTES)
	ZEND_FE(sodium_crypto_stream_xchacha20_xor_ic, arginfo_sodium_crypto_stream_xchacha20_xor_ic)
#endif
	ZEND_FE(sodium_add, arginfo_sodium_add)
	ZEND_FE(sodium_compare, arginfo_sodium_compare)
	ZEND_FE(sodium_increment, arginfo_sodium_increment)
	ZEND_FE(sodium_memcmp, arginfo_sodium_memcmp)
	ZEND_FE(sodium_memzero, arginfo_sodium_memzero)
	ZEND_FE(sodium_pad, arginfo_sodium_pad)
	ZEND_FE(sodium_unpad, arginfo_sodium_unpad)
	ZEND_FE(sodium_bin2hex, arginfo_sodium_bin2hex)
	ZEND_FE(sodium_hex2bin, arginfo_sodium_hex2bin)
#if defined(sodium_base64_VARIANT_ORIGINAL)
	ZEND_FE(sodium_bin2base64, arginfo_sodium_bin2base64)
#endif
#if defined(sodium_base64_VARIANT_ORIGINAL)
	ZEND_FE(sodium_base642bin, arginfo_sodium_base642bin)
#endif
	ZEND_FALIAS(sodium_crypto_scalarmult_base, sodium_crypto_box_publickey_from_secretkey, arginfo_sodium_crypto_scalarmult_base)
	ZEND_FE_END
};


static const zend_function_entry class_SodiumException_methods[] = {
	ZEND_FE_END
};

static void register_libsodium_symbols(int module_number)
{
	REGISTER_STRING_CONSTANT("SODIUM_LIBRARY_VERSION", SODIUM_LIBRARY_VERSION(), CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SODIUM_LIBRARY_MAJOR_VERSION", sodium_library_version_major(), CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SODIUM_LIBRARY_MINOR_VERSION", sodium_library_version_minor(), CONST_PERSISTENT);
#if defined(HAVE_AESGCM)
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_AEAD_AES256GCM_KEYBYTES", crypto_aead_aes256gcm_KEYBYTES, CONST_PERSISTENT);
#endif
#if defined(HAVE_AESGCM)
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_AEAD_AES256GCM_NSECBYTES", crypto_aead_aes256gcm_NSECBYTES, CONST_PERSISTENT);
#endif
#if defined(HAVE_AESGCM)
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_AEAD_AES256GCM_NPUBBYTES", crypto_aead_aes256gcm_NPUBBYTES, CONST_PERSISTENT);
#endif
#if defined(HAVE_AESGCM)
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_AEAD_AES256GCM_ABYTES", crypto_aead_aes256gcm_ABYTES, CONST_PERSISTENT);
#endif
#if defined(crypto_aead_aegis128l_KEYBYTES)
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_AEAD_AEGIS128L_KEYBYTES", crypto_aead_aegis128l_KEYBYTES, CONST_PERSISTENT);
#endif
#if defined(crypto_aead_aegis128l_KEYBYTES)
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_AEAD_AEGIS128L_NSECBYTES", crypto_aead_aegis128l_NSECBYTES, CONST_PERSISTENT);
#endif
#if defined(crypto_aead_aegis128l_KEYBYTES)
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_AEAD_AEGIS128L_NPUBBYTES", crypto_aead_aegis128l_NPUBBYTES, CONST_PERSISTENT);
#endif
#if defined(crypto_aead_aegis128l_KEYBYTES)
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_AEAD_AEGIS128L_ABYTES", crypto_aead_aegis128l_ABYTES, CONST_PERSISTENT);
#endif
#if defined(crypto_aead_aegis256_KEYBYTES)
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_AEAD_AEGIS256_KEYBYTES", crypto_aead_aegis256_KEYBYTES, CONST_PERSISTENT);
#endif
#if defined(crypto_aead_aegis256_KEYBYTES)
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_AEAD_AEGIS256_NSECBYTES", crypto_aead_aegis256_NSECBYTES, CONST_PERSISTENT);
#endif
#if defined(crypto_aead_aegis256_KEYBYTES)
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_AEAD_AEGIS256_NPUBBYTES", crypto_aead_aegis256_NPUBBYTES, CONST_PERSISTENT);
#endif
#if defined(crypto_aead_aegis256_KEYBYTES)
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_AEAD_AEGIS256_ABYTES", crypto_aead_aegis256_ABYTES, CONST_PERSISTENT);
#endif
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_AEAD_CHACHA20POLY1305_KEYBYTES", crypto_aead_chacha20poly1305_KEYBYTES, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_AEAD_CHACHA20POLY1305_NSECBYTES", crypto_aead_chacha20poly1305_NSECBYTES, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_AEAD_CHACHA20POLY1305_NPUBBYTES", crypto_aead_chacha20poly1305_NPUBBYTES, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_AEAD_CHACHA20POLY1305_ABYTES", crypto_aead_chacha20poly1305_ABYTES, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_AEAD_CHACHA20POLY1305_IETF_KEYBYTES", crypto_aead_chacha20poly1305_IETF_KEYBYTES, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_AEAD_CHACHA20POLY1305_IETF_NSECBYTES", crypto_aead_chacha20poly1305_IETF_NSECBYTES, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_AEAD_CHACHA20POLY1305_IETF_NPUBBYTES", crypto_aead_chacha20poly1305_IETF_NPUBBYTES, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_AEAD_CHACHA20POLY1305_IETF_ABYTES", crypto_aead_chacha20poly1305_IETF_ABYTES, CONST_PERSISTENT);
#if defined(crypto_aead_xchacha20poly1305_IETF_NPUBBYTES)
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_AEAD_XCHACHA20POLY1305_IETF_KEYBYTES", crypto_aead_xchacha20poly1305_IETF_KEYBYTES, CONST_PERSISTENT);
#endif
#if defined(crypto_aead_xchacha20poly1305_IETF_NPUBBYTES)
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_AEAD_XCHACHA20POLY1305_IETF_NSECBYTES", crypto_aead_xchacha20poly1305_IETF_NSECBYTES, CONST_PERSISTENT);
#endif
#if defined(crypto_aead_xchacha20poly1305_IETF_NPUBBYTES)
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_AEAD_XCHACHA20POLY1305_IETF_NPUBBYTES", crypto_aead_xchacha20poly1305_IETF_NPUBBYTES, CONST_PERSISTENT);
#endif
#if defined(crypto_aead_xchacha20poly1305_IETF_NPUBBYTES)
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_AEAD_XCHACHA20POLY1305_IETF_ABYTES", crypto_aead_xchacha20poly1305_IETF_ABYTES, CONST_PERSISTENT);
#endif
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_AUTH_BYTES", crypto_auth_BYTES, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_AUTH_KEYBYTES", crypto_auth_KEYBYTES, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_BOX_SEALBYTES", crypto_box_SEALBYTES, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_BOX_SECRETKEYBYTES", crypto_box_SECRETKEYBYTES, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_BOX_PUBLICKEYBYTES", crypto_box_PUBLICKEYBYTES, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_BOX_KEYPAIRBYTES", SODIUM_CRYPTO_BOX_KEYPAIRBYTES(), CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_BOX_MACBYTES", crypto_box_MACBYTES, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_BOX_NONCEBYTES", crypto_box_NONCEBYTES, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_BOX_SEEDBYTES", crypto_box_SEEDBYTES, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_KDF_BYTES_MIN", crypto_kdf_BYTES_MIN, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_KDF_BYTES_MAX", crypto_kdf_BYTES_MAX, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_KDF_CONTEXTBYTES", crypto_kdf_CONTEXTBYTES, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_KDF_KEYBYTES", crypto_kdf_KEYBYTES, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_KX_SEEDBYTES", crypto_kx_SEEDBYTES, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_KX_SESSIONKEYBYTES", crypto_kx_SESSIONKEYBYTES, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_KX_PUBLICKEYBYTES", crypto_kx_PUBLICKEYBYTES, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_KX_SECRETKEYBYTES", crypto_kx_SECRETKEYBYTES, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_KX_KEYPAIRBYTES", SODIUM_CRYPTO_KX_KEYPAIRBYTES(), CONST_PERSISTENT);
#if defined(crypto_secretstream_xchacha20poly1305_ABYTES)
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_SECRETSTREAM_XCHACHA20POLY1305_ABYTES", crypto_secretstream_xchacha20poly1305_ABYTES, CONST_PERSISTENT);
#endif
#if defined(crypto_secretstream_xchacha20poly1305_ABYTES)
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_SECRETSTREAM_XCHACHA20POLY1305_HEADERBYTES", crypto_secretstream_xchacha20poly1305_HEADERBYTES, CONST_PERSISTENT);
#endif
#if defined(crypto_secretstream_xchacha20poly1305_ABYTES)
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_SECRETSTREAM_XCHACHA20POLY1305_KEYBYTES", crypto_secretstream_xchacha20poly1305_KEYBYTES, CONST_PERSISTENT);
#endif
#if defined(crypto_secretstream_xchacha20poly1305_ABYTES)
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_SECRETSTREAM_XCHACHA20POLY1305_MESSAGEBYTES_MAX", crypto_secretstream_xchacha20poly1305_MESSAGEBYTES_MAX, CONST_PERSISTENT);
#endif
#if defined(crypto_secretstream_xchacha20poly1305_ABYTES)
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_SECRETSTREAM_XCHACHA20POLY1305_TAG_MESSAGE", crypto_secretstream_xchacha20poly1305_TAG_MESSAGE, CONST_PERSISTENT);
#endif
#if defined(crypto_secretstream_xchacha20poly1305_ABYTES)
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_SECRETSTREAM_XCHACHA20POLY1305_TAG_PUSH", crypto_secretstream_xchacha20poly1305_TAG_PUSH, CONST_PERSISTENT);
#endif
#if defined(crypto_secretstream_xchacha20poly1305_ABYTES)
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_SECRETSTREAM_XCHACHA20POLY1305_TAG_REKEY", crypto_secretstream_xchacha20poly1305_TAG_REKEY, CONST_PERSISTENT);
#endif
#if defined(crypto_secretstream_xchacha20poly1305_ABYTES)
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_SECRETSTREAM_XCHACHA20POLY1305_TAG_FINAL", crypto_secretstream_xchacha20poly1305_TAG_FINAL, CONST_PERSISTENT);
#endif
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_GENERICHASH_BYTES", crypto_generichash_BYTES, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_GENERICHASH_BYTES_MIN", crypto_generichash_BYTES_MIN, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_GENERICHASH_BYTES_MAX", crypto_generichash_BYTES_MAX, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_GENERICHASH_KEYBYTES", crypto_generichash_KEYBYTES, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_GENERICHASH_KEYBYTES_MIN", crypto_generichash_KEYBYTES_MIN, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_GENERICHASH_KEYBYTES_MAX", crypto_generichash_KEYBYTES_MAX, CONST_PERSISTENT);
#if defined(crypto_pwhash_SALTBYTES)
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_PWHASH_ALG_ARGON2I13", crypto_pwhash_ALG_ARGON2I13, CONST_PERSISTENT);
#endif
#if defined(crypto_pwhash_SALTBYTES) && defined(crypto_pwhash_ALG_ARGON2ID13)
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_PWHASH_ALG_ARGON2ID13", crypto_pwhash_ALG_ARGON2ID13, CONST_PERSISTENT);
#endif
#if defined(crypto_pwhash_SALTBYTES)
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_PWHASH_ALG_DEFAULT", crypto_pwhash_ALG_DEFAULT, CONST_PERSISTENT);
#endif
#if defined(crypto_pwhash_SALTBYTES)
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_PWHASH_SALTBYTES", crypto_pwhash_SALTBYTES, CONST_PERSISTENT);
#endif
#if defined(crypto_pwhash_SALTBYTES)
	REGISTER_STRING_CONSTANT("SODIUM_CRYPTO_PWHASH_STRPREFIX", crypto_pwhash_STRPREFIX, CONST_PERSISTENT);
#endif
#if defined(crypto_pwhash_SALTBYTES)
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_PWHASH_OPSLIMIT_INTERACTIVE", crypto_pwhash_opslimit_interactive(), CONST_PERSISTENT);
#endif
#if defined(crypto_pwhash_SALTBYTES)
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_PWHASH_MEMLIMIT_INTERACTIVE", crypto_pwhash_memlimit_interactive(), CONST_PERSISTENT);
#endif
#if defined(crypto_pwhash_SALTBYTES)
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_PWHASH_OPSLIMIT_MODERATE", crypto_pwhash_opslimit_moderate(), CONST_PERSISTENT);
#endif
#if defined(crypto_pwhash_SALTBYTES)
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_PWHASH_MEMLIMIT_MODERATE", crypto_pwhash_memlimit_moderate(), CONST_PERSISTENT);
#endif
#if defined(crypto_pwhash_SALTBYTES)
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_PWHASH_OPSLIMIT_SENSITIVE", crypto_pwhash_opslimit_sensitive(), CONST_PERSISTENT);
#endif
#if defined(crypto_pwhash_SALTBYTES)
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_PWHASH_MEMLIMIT_SENSITIVE", crypto_pwhash_memlimit_sensitive(), CONST_PERSISTENT);
#endif
#if defined(crypto_pwhash_scryptsalsa208sha256_SALTBYTES)
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_PWHASH_SCRYPTSALSA208SHA256_SALTBYTES", crypto_pwhash_scryptsalsa208sha256_SALTBYTES, CONST_PERSISTENT);
#endif
#if defined(crypto_pwhash_scryptsalsa208sha256_SALTBYTES)
	REGISTER_STRING_CONSTANT("SODIUM_CRYPTO_PWHASH_SCRYPTSALSA208SHA256_STRPREFIX", crypto_pwhash_scryptsalsa208sha256_STRPREFIX, CONST_PERSISTENT);
#endif
#if defined(crypto_pwhash_scryptsalsa208sha256_SALTBYTES)
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_PWHASH_SCRYPTSALSA208SHA256_OPSLIMIT_INTERACTIVE", crypto_pwhash_scryptsalsa208sha256_opslimit_interactive(), CONST_PERSISTENT);
#endif
#if defined(crypto_pwhash_scryptsalsa208sha256_SALTBYTES)
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_PWHASH_SCRYPTSALSA208SHA256_MEMLIMIT_INTERACTIVE", crypto_pwhash_scryptsalsa208sha256_memlimit_interactive(), CONST_PERSISTENT);
#endif
#if defined(crypto_pwhash_scryptsalsa208sha256_SALTBYTES)
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_PWHASH_SCRYPTSALSA208SHA256_OPSLIMIT_SENSITIVE", crypto_pwhash_scryptsalsa208sha256_opslimit_sensitive(), CONST_PERSISTENT);
#endif
#if defined(crypto_pwhash_scryptsalsa208sha256_SALTBYTES)
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_PWHASH_SCRYPTSALSA208SHA256_MEMLIMIT_SENSITIVE", crypto_pwhash_scryptsalsa208sha256_memlimit_sensitive(), CONST_PERSISTENT);
#endif
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_SCALARMULT_BYTES", crypto_scalarmult_BYTES, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_SCALARMULT_SCALARBYTES", crypto_scalarmult_SCALARBYTES, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_SHORTHASH_BYTES", crypto_shorthash_BYTES, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_SHORTHASH_KEYBYTES", crypto_shorthash_KEYBYTES, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_SECRETBOX_KEYBYTES", crypto_secretbox_KEYBYTES, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_SECRETBOX_MACBYTES", crypto_secretbox_MACBYTES, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_SECRETBOX_NONCEBYTES", crypto_secretbox_NONCEBYTES, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_SIGN_BYTES", crypto_sign_BYTES, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_SIGN_SEEDBYTES", crypto_sign_SEEDBYTES, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_SIGN_PUBLICKEYBYTES", crypto_sign_PUBLICKEYBYTES, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_SIGN_SECRETKEYBYTES", crypto_sign_SECRETKEYBYTES, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_SIGN_KEYPAIRBYTES", SODIUM_CRYPTO_SIGN_KEYPAIRBYTES(), CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_STREAM_NONCEBYTES", crypto_stream_NONCEBYTES, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_STREAM_KEYBYTES", crypto_stream_KEYBYTES, CONST_PERSISTENT);
#if defined(crypto_stream_xchacha20_KEYBYTES)
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_STREAM_XCHACHA20_NONCEBYTES", crypto_stream_xchacha20_NONCEBYTES, CONST_PERSISTENT);
#endif
#if defined(crypto_stream_xchacha20_KEYBYTES)
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_STREAM_XCHACHA20_KEYBYTES", crypto_stream_xchacha20_KEYBYTES, CONST_PERSISTENT);
#endif
#if defined(sodium_base64_VARIANT_ORIGINAL)
	REGISTER_LONG_CONSTANT("SODIUM_BASE64_VARIANT_ORIGINAL", sodium_base64_VARIANT_ORIGINAL, CONST_PERSISTENT);
#endif
#if defined(sodium_base64_VARIANT_ORIGINAL)
	REGISTER_LONG_CONSTANT("SODIUM_BASE64_VARIANT_ORIGINAL_NO_PADDING", sodium_base64_VARIANT_ORIGINAL_NO_PADDING, CONST_PERSISTENT);
#endif
#if defined(sodium_base64_VARIANT_ORIGINAL)
	REGISTER_LONG_CONSTANT("SODIUM_BASE64_VARIANT_URLSAFE", sodium_base64_VARIANT_URLSAFE, CONST_PERSISTENT);
#endif
#if defined(sodium_base64_VARIANT_ORIGINAL)
	REGISTER_LONG_CONSTANT("SODIUM_BASE64_VARIANT_URLSAFE_NO_PADDING", sodium_base64_VARIANT_URLSAFE_NO_PADDING, CONST_PERSISTENT);
#endif
#if defined(crypto_core_ristretto255_HASHBYTES)
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_SCALARMULT_RISTRETTO255_BYTES", crypto_scalarmult_ristretto255_BYTES, CONST_PERSISTENT);
#endif
#if defined(crypto_core_ristretto255_HASHBYTES)
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_SCALARMULT_RISTRETTO255_SCALARBYTES", crypto_scalarmult_ristretto255_SCALARBYTES, CONST_PERSISTENT);
#endif
#if defined(crypto_core_ristretto255_HASHBYTES)
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_CORE_RISTRETTO255_BYTES", crypto_core_ristretto255_BYTES, CONST_PERSISTENT);
#endif
#if defined(crypto_core_ristretto255_HASHBYTES)
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_CORE_RISTRETTO255_HASHBYTES", crypto_core_ristretto255_HASHBYTES, CONST_PERSISTENT);
#endif
#if defined(crypto_core_ristretto255_HASHBYTES)
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_CORE_RISTRETTO255_SCALARBYTES", crypto_core_ristretto255_SCALARBYTES, CONST_PERSISTENT);
#endif
#if defined(crypto_core_ristretto255_HASHBYTES)
	REGISTER_LONG_CONSTANT("SODIUM_CRYPTO_CORE_RISTRETTO255_NONREDUCEDSCALARBYTES", crypto_core_ristretto255_NONREDUCEDSCALARBYTES, CONST_PERSISTENT);
#endif

#if defined(HAVE_AESGCM)

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_aead_aes256gcm_decrypt", sizeof("sodium_crypto_aead_aes256gcm_decrypt") - 1), 3, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);
#endif
#if defined(HAVE_AESGCM)

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_aead_aes256gcm_encrypt", sizeof("sodium_crypto_aead_aes256gcm_encrypt") - 1), 0, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_aead_aes256gcm_encrypt", sizeof("sodium_crypto_aead_aes256gcm_encrypt") - 1), 3, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);
#endif
#if defined(crypto_aead_aegis128l_KEYBYTES)

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_aead_aegis128l_decrypt", sizeof("sodium_crypto_aead_aegis128l_decrypt") - 1), 3, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);
#endif
#if defined(crypto_aead_aegis128l_KEYBYTES)

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_aead_aegis128l_encrypt", sizeof("sodium_crypto_aead_aegis128l_encrypt") - 1), 0, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_aead_aegis128l_encrypt", sizeof("sodium_crypto_aead_aegis128l_encrypt") - 1), 3, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);
#endif
#if defined(crypto_aead_aegis256_KEYBYTES)

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_aead_aegis256_decrypt", sizeof("sodium_crypto_aead_aegis256_decrypt") - 1), 3, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);
#endif
#if defined(crypto_aead_aegis256_KEYBYTES)

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_aead_aegis256_encrypt", sizeof("sodium_crypto_aead_aegis256_encrypt") - 1), 0, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_aead_aegis256_encrypt", sizeof("sodium_crypto_aead_aegis256_encrypt") - 1), 3, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);
#endif

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_aead_chacha20poly1305_decrypt", sizeof("sodium_crypto_aead_chacha20poly1305_decrypt") - 1), 3, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_aead_chacha20poly1305_encrypt", sizeof("sodium_crypto_aead_chacha20poly1305_encrypt") - 1), 0, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_aead_chacha20poly1305_encrypt", sizeof("sodium_crypto_aead_chacha20poly1305_encrypt") - 1), 3, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_aead_chacha20poly1305_ietf_decrypt", sizeof("sodium_crypto_aead_chacha20poly1305_ietf_decrypt") - 1), 3, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_aead_chacha20poly1305_ietf_encrypt", sizeof("sodium_crypto_aead_chacha20poly1305_ietf_encrypt") - 1), 0, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_aead_chacha20poly1305_ietf_encrypt", sizeof("sodium_crypto_aead_chacha20poly1305_ietf_encrypt") - 1), 3, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);
#if defined(crypto_aead_xchacha20poly1305_IETF_NPUBBYTES)

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_aead_xchacha20poly1305_ietf_decrypt", sizeof("sodium_crypto_aead_xchacha20poly1305_ietf_decrypt") - 1), 3, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);
#endif
#if defined(crypto_aead_xchacha20poly1305_IETF_NPUBBYTES)

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_aead_xchacha20poly1305_ietf_encrypt", sizeof("sodium_crypto_aead_xchacha20poly1305_ietf_encrypt") - 1), 0, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_aead_xchacha20poly1305_ietf_encrypt", sizeof("sodium_crypto_aead_xchacha20poly1305_ietf_encrypt") - 1), 3, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);
#endif

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_auth", sizeof("sodium_crypto_auth") - 1), 1, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_auth_verify", sizeof("sodium_crypto_auth_verify") - 1), 2, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_box", sizeof("sodium_crypto_box") - 1), 0, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_box", sizeof("sodium_crypto_box") - 1), 2, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_box_seed_keypair", sizeof("sodium_crypto_box_seed_keypair") - 1), 0, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_box_keypair_from_secretkey_and_publickey", sizeof("sodium_crypto_box_keypair_from_secretkey_and_publickey") - 1), 0, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_box_open", sizeof("sodium_crypto_box_open") - 1), 2, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_box_publickey", sizeof("sodium_crypto_box_publickey") - 1), 0, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_box_publickey_from_secretkey", sizeof("sodium_crypto_box_publickey_from_secretkey") - 1), 0, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_box_seal", sizeof("sodium_crypto_box_seal") - 1), 0, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_box_seal_open", sizeof("sodium_crypto_box_seal_open") - 1), 1, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_box_secretkey", sizeof("sodium_crypto_box_secretkey") - 1), 0, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_kx_publickey", sizeof("sodium_crypto_kx_publickey") - 1), 0, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_kx_secretkey", sizeof("sodium_crypto_kx_secretkey") - 1), 0, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_kx_seed_keypair", sizeof("sodium_crypto_kx_seed_keypair") - 1), 0, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_kx_client_session_keys", sizeof("sodium_crypto_kx_client_session_keys") - 1), 0, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_kx_server_session_keys", sizeof("sodium_crypto_kx_server_session_keys") - 1), 0, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_generichash", sizeof("sodium_crypto_generichash") - 1), 1, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_generichash_init", sizeof("sodium_crypto_generichash_init") - 1), 0, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_kdf_derive_from_key", sizeof("sodium_crypto_kdf_derive_from_key") - 1), 3, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);
#if defined(crypto_pwhash_SALTBYTES)

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_pwhash", sizeof("sodium_crypto_pwhash") - 1), 1, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);
#endif
#if defined(crypto_pwhash_SALTBYTES)

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_pwhash_str", sizeof("sodium_crypto_pwhash_str") - 1), 0, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);
#endif
#if defined(crypto_pwhash_SALTBYTES)

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_pwhash_str_verify", sizeof("sodium_crypto_pwhash_str_verify") - 1), 1, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);
#endif
#if defined(crypto_pwhash_scryptsalsa208sha256_SALTBYTES)

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_pwhash_scryptsalsa208sha256", sizeof("sodium_crypto_pwhash_scryptsalsa208sha256") - 1), 1, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);
#endif
#if defined(crypto_pwhash_scryptsalsa208sha256_SALTBYTES)

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_pwhash_scryptsalsa208sha256_str", sizeof("sodium_crypto_pwhash_scryptsalsa208sha256_str") - 1), 0, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);
#endif
#if defined(crypto_pwhash_scryptsalsa208sha256_SALTBYTES)

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_pwhash_scryptsalsa208sha256_str_verify", sizeof("sodium_crypto_pwhash_scryptsalsa208sha256_str_verify") - 1), 1, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);
#endif

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_secretbox", sizeof("sodium_crypto_secretbox") - 1), 0, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_secretbox", sizeof("sodium_crypto_secretbox") - 1), 2, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_secretbox_open", sizeof("sodium_crypto_secretbox_open") - 1), 2, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);
#if defined(crypto_secretstream_xchacha20poly1305_ABYTES)

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_secretstream_xchacha20poly1305_init_push", sizeof("sodium_crypto_secretstream_xchacha20poly1305_init_push") - 1), 0, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);
#endif
#if defined(crypto_secretstream_xchacha20poly1305_ABYTES)

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_secretstream_xchacha20poly1305_push", sizeof("sodium_crypto_secretstream_xchacha20poly1305_push") - 1), 1, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);
#endif
#if defined(crypto_secretstream_xchacha20poly1305_ABYTES)

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_secretstream_xchacha20poly1305_init_pull", sizeof("sodium_crypto_secretstream_xchacha20poly1305_init_pull") - 1), 1, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);
#endif

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_shorthash", sizeof("sodium_crypto_shorthash") - 1), 1, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_sign", sizeof("sodium_crypto_sign") - 1), 1, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_sign_detached", sizeof("sodium_crypto_sign_detached") - 1), 1, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_sign_ed25519_sk_to_curve25519", sizeof("sodium_crypto_sign_ed25519_sk_to_curve25519") - 1), 0, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_sign_keypair_from_secretkey_and_publickey", sizeof("sodium_crypto_sign_keypair_from_secretkey_and_publickey") - 1), 0, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_sign_publickey", sizeof("sodium_crypto_sign_publickey") - 1), 0, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_sign_secretkey", sizeof("sodium_crypto_sign_secretkey") - 1), 0, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_sign_publickey_from_secretkey", sizeof("sodium_crypto_sign_publickey_from_secretkey") - 1), 0, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_sign_seed_keypair", sizeof("sodium_crypto_sign_seed_keypair") - 1), 0, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_stream", sizeof("sodium_crypto_stream") - 1), 2, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_stream_xor", sizeof("sodium_crypto_stream_xor") - 1), 0, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_stream_xor", sizeof("sodium_crypto_stream_xor") - 1), 2, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);
#if defined(crypto_stream_xchacha20_KEYBYTES)

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_stream_xchacha20", sizeof("sodium_crypto_stream_xchacha20") - 1), 2, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);
#endif
#if defined(crypto_stream_xchacha20_KEYBYTES)

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_stream_xchacha20_xor", sizeof("sodium_crypto_stream_xchacha20_xor") - 1), 0, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_stream_xchacha20_xor", sizeof("sodium_crypto_stream_xchacha20_xor") - 1), 2, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);
#endif
#if defined(crypto_stream_xchacha20_KEYBYTES)

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_stream_xchacha20_xor_ic", sizeof("sodium_crypto_stream_xchacha20_xor_ic") - 1), 0, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_stream_xchacha20_xor_ic", sizeof("sodium_crypto_stream_xchacha20_xor_ic") - 1), 3, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);
#endif

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_compare", sizeof("sodium_compare") - 1), 0, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_compare", sizeof("sodium_compare") - 1), 1, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_memcmp", sizeof("sodium_memcmp") - 1), 0, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_memcmp", sizeof("sodium_memcmp") - 1), 1, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_memzero", sizeof("sodium_memzero") - 1), 0, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_pad", sizeof("sodium_pad") - 1), 0, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_unpad", sizeof("sodium_unpad") - 1), 0, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_bin2hex", sizeof("sodium_bin2hex") - 1), 0, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_hex2bin", sizeof("sodium_hex2bin") - 1), 0, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);
#if defined(sodium_base64_VARIANT_ORIGINAL)

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_bin2base64", sizeof("sodium_bin2base64") - 1), 0, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);
#endif
#if defined(sodium_base64_VARIANT_ORIGINAL)

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_base642bin", sizeof("sodium_base642bin") - 1), 0, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);
#endif

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "sodium_crypto_scalarmult_base", sizeof("sodium_crypto_scalarmult_base") - 1), 0, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);
}

static zend_class_entry *register_class_SodiumException(zend_class_entry *class_entry_Exception)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "SodiumException", class_SodiumException_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_Exception);

	return class_entry;
}
