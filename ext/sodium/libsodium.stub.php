<?php

/** @generate-class-entries */

function sodium_crypto_aead_aes256gcm_is_available(): bool {}

#ifdef HAVE_AESGCM
function sodium_crypto_aead_aes256gcm_decrypt(string $ciphertext, string $additional_data, string $nonce, #[\SensitiveParameter] string $key): string|false {}

function sodium_crypto_aead_aes256gcm_encrypt(#[\SensitiveParameter] string $message, string $additional_data, string $nonce, #[\SensitiveParameter] string $key): string {}

function sodium_crypto_aead_aes256gcm_keygen(): string {}
#endif

function sodium_crypto_aead_chacha20poly1305_decrypt(string $ciphertext, string $additional_data, string $nonce, #[\SensitiveParameter] string $key): string|false {}

function sodium_crypto_aead_chacha20poly1305_encrypt(#[\SensitiveParameter] string $message, string $additional_data, string $nonce, #[\SensitiveParameter] string $key): string {}

function sodium_crypto_aead_chacha20poly1305_keygen(): string {}

function sodium_crypto_aead_chacha20poly1305_ietf_decrypt(string $ciphertext, string $additional_data, string $nonce, #[\SensitiveParameter] string $key): string|false {}

function sodium_crypto_aead_chacha20poly1305_ietf_encrypt(#[\SensitiveParameter] string $message, string $additional_data, string $nonce, #[\SensitiveParameter] string $key): string {}

function sodium_crypto_aead_chacha20poly1305_ietf_keygen(): string {}

#ifdef crypto_aead_xchacha20poly1305_IETF_NPUBBYTES
function sodium_crypto_aead_xchacha20poly1305_ietf_decrypt(string $ciphertext, string $additional_data, string $nonce, #[\SensitiveParameter] string $key): string|false {}

function sodium_crypto_aead_xchacha20poly1305_ietf_keygen(): string {}

function sodium_crypto_aead_xchacha20poly1305_ietf_encrypt(#[\SensitiveParameter] string $message, string $additional_data, string $nonce, #[\SensitiveParameter] string $key): string {}
#endif

function sodium_crypto_auth(string $message, #[\SensitiveParameter] string $key): string {}

function sodium_crypto_auth_keygen(): string {}

function sodium_crypto_auth_verify(string $mac, string $message, #[\SensitiveParameter] string $key): bool {}

function sodium_crypto_box(#[\SensitiveParameter] string $message, string $nonce, #[\SensitiveParameter] string $key_pair): string {}

function sodium_crypto_box_keypair(): string {}

function sodium_crypto_box_seed_keypair(#[\SensitiveParameter] string $seed): string {}

function sodium_crypto_box_keypair_from_secretkey_and_publickey(#[\SensitiveParameter] string $secret_key, string $public_key): string {}

function sodium_crypto_box_open(string $ciphertext, string $nonce, #[\SensitiveParameter] string $key_pair): string|false {}

function sodium_crypto_box_publickey(#[\SensitiveParameter] string $key_pair): string {}

function sodium_crypto_box_publickey_from_secretkey(#[\SensitiveParameter] string $secret_key): string {}

function sodium_crypto_box_seal(#[\SensitiveParameter] string $message, string $public_key): string {}

function sodium_crypto_box_seal_open(string $ciphertext, #[\SensitiveParameter] string $key_pair): string|false {}

function sodium_crypto_box_secretkey(#[\SensitiveParameter] string $key_pair): string {}

#ifdef crypto_core_ristretto255_HASHBYTES
function sodium_crypto_core_ristretto255_add(string $p, string $q): string {}

function sodium_crypto_core_ristretto255_from_hash(string $s): string {}

function sodium_crypto_core_ristretto255_is_valid_point(string $s): bool {}

function sodium_crypto_core_ristretto255_random(): string {}

function sodium_crypto_core_ristretto255_scalar_add(string $x, string $y): string {}

function sodium_crypto_core_ristretto255_scalar_complement(string $s): string {}

function sodium_crypto_core_ristretto255_scalar_invert(string $s): string {}

function sodium_crypto_core_ristretto255_scalar_mul(string $x, string $y): string {}

function sodium_crypto_core_ristretto255_scalar_negate(string $s): string {}

function sodium_crypto_core_ristretto255_scalar_random(): string {}

function sodium_crypto_core_ristretto255_scalar_reduce(string $s): string {}

function sodium_crypto_core_ristretto255_scalar_sub(string $x, string $y): string {}

function sodium_crypto_core_ristretto255_sub(string $p, string $q): string {}
#endif

function sodium_crypto_kx_keypair(): string {}

function sodium_crypto_kx_publickey(#[\SensitiveParameter] string $key_pair): string {}

function sodium_crypto_kx_secretkey(#[\SensitiveParameter] string $key_pair): string {}

function sodium_crypto_kx_seed_keypair(#[\SensitiveParameter] string $seed): string {}

/**
 * @return array<int, string>
 */
function sodium_crypto_kx_client_session_keys(#[\SensitiveParameter] string $client_key_pair, string $server_key): array {}

/**
 * @return array<int, string>
 *
 */
function sodium_crypto_kx_server_session_keys(#[\SensitiveParameter] string $server_key_pair, string $client_key): array {}

function sodium_crypto_generichash(string $message, #[\SensitiveParameter] string $key = "", int $length = SODIUM_CRYPTO_GENERICHASH_BYTES): string {}

function sodium_crypto_generichash_keygen(): string {}

function sodium_crypto_generichash_init(#[\SensitiveParameter] string $key = "", int $length = SODIUM_CRYPTO_GENERICHASH_BYTES): string {}

function sodium_crypto_generichash_update(string &$state, string $message): true {}

function sodium_crypto_generichash_final(string &$state, int $length = SODIUM_CRYPTO_GENERICHASH_BYTES): string {}

function sodium_crypto_kdf_derive_from_key(int $subkey_length, int $subkey_id, string $context, #[\SensitiveParameter] string $key): string {}

function sodium_crypto_kdf_keygen(): string {}

#ifdef crypto_pwhash_SALTBYTES
function sodium_crypto_pwhash(int $length, #[\SensitiveParameter] string $password, string $salt, int $opslimit, int $memlimit, int $algo = SODIUM_CRYPTO_PWHASH_ALG_DEFAULT): string {}

function sodium_crypto_pwhash_str(#[\SensitiveParameter] string $password, int $opslimit, int $memlimit): string {}

function sodium_crypto_pwhash_str_verify(string $hash, #[\SensitiveParameter] string $password): bool {}
#endif

#if SODIUM_LIBRARY_VERSION_MAJOR > 9 || (SODIUM_LIBRARY_VERSION_MAJOR == 9 && SODIUM_LIBRARY_VERSION_MINOR >= 6)
function sodium_crypto_pwhash_str_needs_rehash(string $password, int $opslimit, int $memlimit): bool {}
#endif

#ifdef crypto_pwhash_scryptsalsa208sha256_SALTBYTES
function sodium_crypto_pwhash_scryptsalsa208sha256(int $length, #[\SensitiveParameter] string $password, string $salt, int $opslimit, int $memlimit): string {}

function sodium_crypto_pwhash_scryptsalsa208sha256_str(#[\SensitiveParameter] string $password, int $opslimit, int $memlimit): string {}

function sodium_crypto_pwhash_scryptsalsa208sha256_str_verify(string $hash, #[\SensitiveParameter] string $password): bool {}
#endif

function sodium_crypto_scalarmult(string $n, string $p): string {}

#ifdef crypto_core_ristretto255_HASHBYTES
function sodium_crypto_scalarmult_ristretto255(string $n, string $p): string {}

function sodium_crypto_scalarmult_ristretto255_base(string $n): string {}
#endif

function sodium_crypto_secretbox(#[\SensitiveParameter] string $message, string $nonce, #[\SensitiveParameter] string $key): string {}

function sodium_crypto_secretbox_keygen(): string {}

function sodium_crypto_secretbox_open(string $ciphertext, string $nonce, #[\SensitiveParameter] string $key): string|false {}

#ifdef crypto_secretstream_xchacha20poly1305_ABYTES
function sodium_crypto_secretstream_xchacha20poly1305_keygen(): string {}

/**
 * @return array<int, string>
 */
function sodium_crypto_secretstream_xchacha20poly1305_init_push(#[\SensitiveParameter] string $key): array {}

function sodium_crypto_secretstream_xchacha20poly1305_push(string &$state, #[\SensitiveParameter] string $message, string $additional_data = "", int $tag = SODIUM_CRYPTO_SECRETSTREAM_XCHACHA20POLY1305_TAG_MESSAGE): string {}

function sodium_crypto_secretstream_xchacha20poly1305_init_pull(string $header, #[\SensitiveParameter] string $key): string {}

/** @return array<int, int|string>|false */
function sodium_crypto_secretstream_xchacha20poly1305_pull(string &$state, string $ciphertext, string $additional_data = ""): array|false {}

function sodium_crypto_secretstream_xchacha20poly1305_rekey(string &$state): void {}
#endif

function sodium_crypto_shorthash(string $message, #[\SensitiveParameter] string $key): string {}

function sodium_crypto_shorthash_keygen(): string {}

function sodium_crypto_sign(string $message, #[\SensitiveParameter] string $secret_key): string {}

function sodium_crypto_sign_detached(string $message, #[\SensitiveParameter] string $secret_key): string {}

function sodium_crypto_sign_ed25519_pk_to_curve25519(string $public_key): string {}

function sodium_crypto_sign_ed25519_sk_to_curve25519(#[\SensitiveParameter] string $secret_key): string {}

function sodium_crypto_sign_keypair(): string {}

function sodium_crypto_sign_keypair_from_secretkey_and_publickey(#[\SensitiveParameter] string $secret_key, string $public_key): string {}

function sodium_crypto_sign_open(string $signed_message, string $public_key): string|false {}

function sodium_crypto_sign_publickey(#[\SensitiveParameter] string $key_pair): string {}

function sodium_crypto_sign_secretkey(#[\SensitiveParameter] string $key_pair): string {}

function sodium_crypto_sign_publickey_from_secretkey(#[\SensitiveParameter] string $secret_key): string {}

function sodium_crypto_sign_seed_keypair(#[\SensitiveParameter] string $seed): string {}

function sodium_crypto_sign_verify_detached(string $signature, string $message, string $public_key): bool {}

function sodium_crypto_stream(int $length, string $nonce, #[\SensitiveParameter] string $key): string {}

function sodium_crypto_stream_keygen(): string {}

function sodium_crypto_stream_xor(#[\SensitiveParameter] string $message, string $nonce, #[\SensitiveParameter] string $key): string {}

#if defined(crypto_stream_xchacha20_KEYBYTES)
function sodium_crypto_stream_xchacha20(int $length, string $nonce, #[\SensitiveParameter] string $key): string {}

function sodium_crypto_stream_xchacha20_keygen(): string {}

function sodium_crypto_stream_xchacha20_xor(#[\SensitiveParameter] string $message, string $nonce, #[\SensitiveParameter] string $key): string {}

function sodium_crypto_stream_xchacha20_xor_ic(#[\SensitiveParameter] string $message, string $nonce, int $counter,#[\SensitiveParameter]  string $key): string {}
#endif

function sodium_add(string &$string1, string $string2): void {}

function sodium_compare(#[\SensitiveParameter] string $string1, #[\SensitiveParameter] string $string2): int {}

function sodium_increment(string &$string): void {}

function sodium_memcmp(#[\SensitiveParameter] string $string1, #[\SensitiveParameter] string $string2): int {}

function sodium_memzero(#[\SensitiveParameter] string &$string): void {}

function sodium_pad(#[\SensitiveParameter] string $string, int $block_size): string {}

function sodium_unpad(#[\SensitiveParameter] string $string, int $block_size): string {}

function sodium_bin2hex(#[\SensitiveParameter] string $string): string {}

function sodium_hex2bin(#[\SensitiveParameter] string $string, string $ignore = ""): string {}

#ifdef sodium_base64_VARIANT_ORIGINAL
function sodium_bin2base64(#[\SensitiveParameter] string $string, int $id): string {}

function sodium_base642bin(#[\SensitiveParameter] string $string, int $id, string $ignore = ""): string {}
#endif

/**
 * @alias sodium_crypto_box_publickey_from_secretkey
 */
function sodium_crypto_scalarmult_base(#[\SensitiveParameter] string $secret_key): string {}

class SodiumException extends Exception {}
