/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 1d75807de20b87cd274e6073ef9c9392a34f7279 */

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

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_generichash_update, 0, 2, _IS_BOOL, 0)
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
