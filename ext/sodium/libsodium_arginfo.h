/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_aead_aes256gcm_is_available, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#if defined(HAVE_AESGCM)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_sodium_crypto_aead_aes256gcm_decrypt, 0, 4, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, ad, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, nonce, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_AESGCM)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_aead_aes256gcm_encrypt, 0, 4, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, ad, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, nonce, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_AESGCM)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_aead_aes256gcm_keygen, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_sodium_crypto_aead_chacha20poly1305_decrypt, 0, 4, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, ad, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, nonce, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_aead_chacha20poly1305_encrypt, 0, 4, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, ad, IS_STRING, 0)
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
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, ad, IS_STRING, 0)
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
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, ad, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, nonce, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_auth, 0, 2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_sodium_crypto_auth_keygen arginfo_sodium_crypto_aead_chacha20poly1305_keygen

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_auth_verify, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, signature, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_box, 0, 3, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, nonce, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_sodium_crypto_box_keypair arginfo_sodium_crypto_aead_chacha20poly1305_keygen

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_box_seed_keypair, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_box_keypair_from_secretkey_and_publickey, 0, 2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, secret_key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, public_key, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_sodium_crypto_box_open, 0, 3, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, nonce, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_sodium_crypto_box_publickey arginfo_sodium_crypto_box_seed_keypair

#define arginfo_sodium_crypto_box_publickey_from_secretkey arginfo_sodium_crypto_box_seed_keypair

#define arginfo_sodium_crypto_box_seal arginfo_sodium_crypto_auth

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_sodium_crypto_box_seal_open, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_sodium_crypto_box_secretkey arginfo_sodium_crypto_box_seed_keypair

#define arginfo_sodium_crypto_kx_keypair arginfo_sodium_crypto_aead_chacha20poly1305_keygen

#define arginfo_sodium_crypto_kx_publickey arginfo_sodium_crypto_box_seed_keypair

#define arginfo_sodium_crypto_kx_secretkey arginfo_sodium_crypto_box_seed_keypair

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_kx_seed_keypair, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_kx_client_session_keys, 0, 2, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, client_keypair, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, server_key, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_kx_server_session_keys, 0, 2, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, server_keypair, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, client_key, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_generichash, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_sodium_crypto_generichash_keygen arginfo_sodium_crypto_aead_chacha20poly1305_keygen

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_generichash_init, 0, 0, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_generichash_update, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(1, state, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_generichash_final, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(1, state, IS_STRING, 0)
	ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_kdf_derive_from_key, 0, 4, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, subkey_len, IS_LONG, 0)
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
	ZEND_ARG_TYPE_INFO(0, alg, IS_LONG, 0)
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
	ZEND_ARG_TYPE_INFO(0, string_1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string_2, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_sodium_crypto_secretbox arginfo_sodium_crypto_box

#define arginfo_sodium_crypto_secretbox_keygen arginfo_sodium_crypto_aead_chacha20poly1305_keygen

#define arginfo_sodium_crypto_secretbox_open arginfo_sodium_crypto_box_open

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
	ZEND_ARG_TYPE_INFO(0, msg, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, ad, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, tag, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(crypto_secretstream_xchacha20poly1305_ABYTES)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_secretstream_xchacha20poly1305_init_pull, 0, 2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(crypto_secretstream_xchacha20poly1305_ABYTES)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_sodium_crypto_secretstream_xchacha20poly1305_pull, 0, 2, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(1, state, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, c, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, ad, IS_STRING, 0)
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
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, keypair, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_sodium_crypto_sign_detached arginfo_sodium_crypto_sign

#define arginfo_sodium_crypto_sign_ed25519_pk_to_curve25519 arginfo_sodium_crypto_box_seed_keypair

#define arginfo_sodium_crypto_sign_ed25519_sk_to_curve25519 arginfo_sodium_crypto_box_seed_keypair

#define arginfo_sodium_crypto_sign_keypair arginfo_sodium_crypto_aead_chacha20poly1305_keygen

#define arginfo_sodium_crypto_sign_keypair_from_secretkey_and_publickey arginfo_sodium_crypto_box_keypair_from_secretkey_and_publickey

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_sodium_crypto_sign_open, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, keypair, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_sodium_crypto_sign_publickey arginfo_sodium_crypto_box_seed_keypair

#define arginfo_sodium_crypto_sign_secretkey arginfo_sodium_crypto_box_seed_keypair

#define arginfo_sodium_crypto_sign_publickey_from_secretkey arginfo_sodium_crypto_box_seed_keypair

#define arginfo_sodium_crypto_sign_seed_keypair arginfo_sodium_crypto_box_seed_keypair

#define arginfo_sodium_crypto_sign_verify_detached arginfo_sodium_crypto_auth_verify

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_crypto_stream, 0, 3, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, nonce, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_sodium_crypto_stream_keygen arginfo_sodium_crypto_aead_chacha20poly1305_keygen

#define arginfo_sodium_crypto_stream_xor arginfo_sodium_crypto_box

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_add, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(1, string_1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string_2, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_compare, 0, 2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, string_1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string_2, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_increment, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(1, string, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_sodium_memcmp arginfo_sodium_compare

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_memzero, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(1, reference, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_pad, 0, 2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_sodium_unpad arginfo_sodium_pad

#define arginfo_sodium_bin2hex arginfo_sodium_crypto_kx_seed_keypair

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sodium_hex2bin, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_INFO(0, ignore)
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
	ZEND_ARG_TYPE_INFO(0, ignore, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#define arginfo_sodium_crypto_scalarmult_base arginfo_sodium_crypto_box_seed_keypair
