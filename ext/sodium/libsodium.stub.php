<?php

/** @generate-class-entries */

/**
 * @var string
 * @cvalue SODIUM_LIBRARY_VERSION()
 */
const SODIUM_LIBRARY_VERSION = UNKNOWN;
/**
 * @var int
 * @cvalue sodium_library_version_major()
 */
const SODIUM_LIBRARY_MAJOR_VERSION = UNKNOWN;
/**
 * @var int
 * @cvalue sodium_library_version_minor()
 */
const SODIUM_LIBRARY_MINOR_VERSION = UNKNOWN;
#ifdef HAVE_AESGCM
/**
 * @var int
 * @cvalue crypto_aead_aes256gcm_KEYBYTES
 */
const SODIUM_CRYPTO_AEAD_AES256GCM_KEYBYTES = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_aead_aes256gcm_NSECBYTES
 */
const SODIUM_CRYPTO_AEAD_AES256GCM_NSECBYTES = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_aead_aes256gcm_NPUBBYTES
 */
const SODIUM_CRYPTO_AEAD_AES256GCM_NPUBBYTES = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_aead_aes256gcm_ABYTES
 */
const SODIUM_CRYPTO_AEAD_AES256GCM_ABYTES = UNKNOWN;
#endif

/**
 * @var int
 * @cvalue crypto_aead_chacha20poly1305_KEYBYTES
 */
const SODIUM_CRYPTO_AEAD_CHACHA20POLY1305_KEYBYTES = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_aead_chacha20poly1305_NSECBYTES
 */
const SODIUM_CRYPTO_AEAD_CHACHA20POLY1305_NSECBYTES = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_aead_chacha20poly1305_NPUBBYTES
 */
const SODIUM_CRYPTO_AEAD_CHACHA20POLY1305_NPUBBYTES = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_aead_chacha20poly1305_ABYTES
 */
const SODIUM_CRYPTO_AEAD_CHACHA20POLY1305_ABYTES = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_aead_chacha20poly1305_IETF_KEYBYTES
 */
const SODIUM_CRYPTO_AEAD_CHACHA20POLY1305_IETF_KEYBYTES = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_aead_chacha20poly1305_IETF_NSECBYTES
 */
const SODIUM_CRYPTO_AEAD_CHACHA20POLY1305_IETF_NSECBYTES = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_aead_chacha20poly1305_IETF_NPUBBYTES
 */
const SODIUM_CRYPTO_AEAD_CHACHA20POLY1305_IETF_NPUBBYTES = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_aead_chacha20poly1305_IETF_ABYTES
 */
const SODIUM_CRYPTO_AEAD_CHACHA20POLY1305_IETF_ABYTES = UNKNOWN;
#ifdef crypto_aead_xchacha20poly1305_IETF_NPUBBYTES
/**
 * @var int
 * @cvalue crypto_aead_xchacha20poly1305_IETF_KEYBYTES
 */
const SODIUM_CRYPTO_AEAD_XCHACHA20POLY1305_IETF_KEYBYTES = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_aead_xchacha20poly1305_IETF_NSECBYTES
 */
const SODIUM_CRYPTO_AEAD_XCHACHA20POLY1305_IETF_NSECBYTES = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_aead_xchacha20poly1305_IETF_NPUBBYTES
 */
const SODIUM_CRYPTO_AEAD_XCHACHA20POLY1305_IETF_NPUBBYTES = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_aead_xchacha20poly1305_IETF_ABYTES
 */
const SODIUM_CRYPTO_AEAD_XCHACHA20POLY1305_IETF_ABYTES = UNKNOWN;
#endif
/**
 * @var int
 * @cvalue crypto_auth_BYTES
 */
const SODIUM_CRYPTO_AUTH_BYTES = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_auth_KEYBYTES
 */
const SODIUM_CRYPTO_AUTH_KEYBYTES = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_box_SEALBYTES
 */
const SODIUM_CRYPTO_BOX_SEALBYTES = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_box_SECRETKEYBYTES
 */
const SODIUM_CRYPTO_BOX_SECRETKEYBYTES = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_box_PUBLICKEYBYTES
 */
const SODIUM_CRYPTO_BOX_PUBLICKEYBYTES = UNKNOWN;
/**
 * @var int
 * @cvalue SODIUM_CRYPTO_BOX_KEYPAIRBYTES()
 */
const SODIUM_CRYPTO_BOX_KEYPAIRBYTES = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_box_MACBYTES
 */
const SODIUM_CRYPTO_BOX_MACBYTES = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_box_NONCEBYTES
 */
const SODIUM_CRYPTO_BOX_NONCEBYTES = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_box_SEEDBYTES
 */
const SODIUM_CRYPTO_BOX_SEEDBYTES = UNKNOWN;

/**
 * @var int
 * @cvalue crypto_kdf_BYTES_MIN
 */
const SODIUM_CRYPTO_KDF_BYTES_MIN = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_kdf_BYTES_MAX
 */
const SODIUM_CRYPTO_KDF_BYTES_MAX = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_kdf_CONTEXTBYTES
 */
const SODIUM_CRYPTO_KDF_CONTEXTBYTES = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_kdf_KEYBYTES
 */
const SODIUM_CRYPTO_KDF_KEYBYTES = UNKNOWN;

/**
 * @var int
 * @cvalue crypto_kx_SEEDBYTES
 */
const SODIUM_CRYPTO_KX_SEEDBYTES = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_kx_SESSIONKEYBYTES
 */
const SODIUM_CRYPTO_KX_SESSIONKEYBYTES = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_kx_PUBLICKEYBYTES
 */
const SODIUM_CRYPTO_KX_PUBLICKEYBYTES = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_kx_SECRETKEYBYTES
 */
const SODIUM_CRYPTO_KX_SECRETKEYBYTES = UNKNOWN;
/**
 * @var int
 * @cvalue SODIUM_CRYPTO_KX_KEYPAIRBYTES()
 */
const SODIUM_CRYPTO_KX_KEYPAIRBYTES = UNKNOWN;
#ifdef crypto_secretstream_xchacha20poly1305_ABYTES
/**
 * @var int
 * @cvalue crypto_secretstream_xchacha20poly1305_ABYTES
 */
const SODIUM_CRYPTO_SECRETSTREAM_XCHACHA20POLY1305_ABYTES = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_secretstream_xchacha20poly1305_HEADERBYTES
 */
const SODIUM_CRYPTO_SECRETSTREAM_XCHACHA20POLY1305_HEADERBYTES = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_secretstream_xchacha20poly1305_KEYBYTES
 */
const SODIUM_CRYPTO_SECRETSTREAM_XCHACHA20POLY1305_KEYBYTES = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_secretstream_xchacha20poly1305_MESSAGEBYTES_MAX
 */
const SODIUM_CRYPTO_SECRETSTREAM_XCHACHA20POLY1305_MESSAGEBYTES_MAX = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_secretstream_xchacha20poly1305_TAG_MESSAGE
 */
const SODIUM_CRYPTO_SECRETSTREAM_XCHACHA20POLY1305_TAG_MESSAGE = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_secretstream_xchacha20poly1305_TAG_PUSH
 */
const SODIUM_CRYPTO_SECRETSTREAM_XCHACHA20POLY1305_TAG_PUSH = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_secretstream_xchacha20poly1305_TAG_REKEY
 */
const SODIUM_CRYPTO_SECRETSTREAM_XCHACHA20POLY1305_TAG_REKEY = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_secretstream_xchacha20poly1305_TAG_FINAL
 */
const SODIUM_CRYPTO_SECRETSTREAM_XCHACHA20POLY1305_TAG_FINAL = UNKNOWN;
#endif

/**
 * @var int
 * @cvalue crypto_generichash_BYTES
 */
const SODIUM_CRYPTO_GENERICHASH_BYTES = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_generichash_BYTES_MIN
 */
const SODIUM_CRYPTO_GENERICHASH_BYTES_MIN = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_generichash_BYTES_MAX
 */
const SODIUM_CRYPTO_GENERICHASH_BYTES_MAX = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_generichash_KEYBYTES
 */
const SODIUM_CRYPTO_GENERICHASH_KEYBYTES = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_generichash_KEYBYTES_MIN
 */
const SODIUM_CRYPTO_GENERICHASH_KEYBYTES_MIN = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_generichash_KEYBYTES_MAX
 */
const SODIUM_CRYPTO_GENERICHASH_KEYBYTES_MAX = UNKNOWN;
#ifdef crypto_pwhash_SALTBYTES
/**
 * @var int
 * @cvalue crypto_pwhash_ALG_ARGON2I13
 */
const SODIUM_CRYPTO_PWHASH_ALG_ARGON2I13 = UNKNOWN;
# ifdef crypto_pwhash_ALG_ARGON2ID13
/**
 * @var int
 * @cvalue crypto_pwhash_ALG_ARGON2ID13
 */
const SODIUM_CRYPTO_PWHASH_ALG_ARGON2ID13 = UNKNOWN;
# endif
/**
 * @var int
 * @cvalue crypto_pwhash_ALG_DEFAULT
 */
const SODIUM_CRYPTO_PWHASH_ALG_DEFAULT = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_pwhash_SALTBYTES
 */
const SODIUM_CRYPTO_PWHASH_SALTBYTES = UNKNOWN;
/**
 * @var string
 * @cvalue crypto_pwhash_STRPREFIX
 */
const SODIUM_CRYPTO_PWHASH_STRPREFIX = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_pwhash_opslimit_interactive()
 */
const SODIUM_CRYPTO_PWHASH_OPSLIMIT_INTERACTIVE = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_pwhash_memlimit_interactive()
 */
const SODIUM_CRYPTO_PWHASH_MEMLIMIT_INTERACTIVE = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_pwhash_opslimit_moderate()
 */
const SODIUM_CRYPTO_PWHASH_OPSLIMIT_MODERATE = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_pwhash_memlimit_moderate()
 */
const SODIUM_CRYPTO_PWHASH_MEMLIMIT_MODERATE = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_pwhash_opslimit_sensitive()
 */
const SODIUM_CRYPTO_PWHASH_OPSLIMIT_SENSITIVE = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_pwhash_memlimit_sensitive()
 */
const SODIUM_CRYPTO_PWHASH_MEMLIMIT_SENSITIVE = UNKNOWN;
#endif
#ifdef crypto_pwhash_scryptsalsa208sha256_SALTBYTES
/**
 * @var int
 * @cvalue crypto_pwhash_scryptsalsa208sha256_SALTBYTES
 */
const SODIUM_CRYPTO_PWHASH_SCRYPTSALSA208SHA256_SALTBYTES = UNKNOWN;
/**
 * @var string
 * @cvalue crypto_pwhash_scryptsalsa208sha256_STRPREFIX
 */
const SODIUM_CRYPTO_PWHASH_SCRYPTSALSA208SHA256_STRPREFIX = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_pwhash_scryptsalsa208sha256_opslimit_interactive()
 */
const SODIUM_CRYPTO_PWHASH_SCRYPTSALSA208SHA256_OPSLIMIT_INTERACTIVE = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_pwhash_scryptsalsa208sha256_memlimit_interactive()
 */
const SODIUM_CRYPTO_PWHASH_SCRYPTSALSA208SHA256_MEMLIMIT_INTERACTIVE = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_pwhash_scryptsalsa208sha256_opslimit_sensitive()
 */
const SODIUM_CRYPTO_PWHASH_SCRYPTSALSA208SHA256_OPSLIMIT_SENSITIVE = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_pwhash_scryptsalsa208sha256_memlimit_sensitive()
 */
const SODIUM_CRYPTO_PWHASH_SCRYPTSALSA208SHA256_MEMLIMIT_SENSITIVE = UNKNOWN;
#endif

/**
 * @var int
 * @cvalue crypto_scalarmult_BYTES
 */
const SODIUM_CRYPTO_SCALARMULT_BYTES = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_scalarmult_SCALARBYTES
 */
const SODIUM_CRYPTO_SCALARMULT_SCALARBYTES = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_shorthash_BYTES
 */
const SODIUM_CRYPTO_SHORTHASH_BYTES = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_shorthash_KEYBYTES
 */
const SODIUM_CRYPTO_SHORTHASH_KEYBYTES = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_secretbox_KEYBYTES
 */
const SODIUM_CRYPTO_SECRETBOX_KEYBYTES = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_secretbox_MACBYTES
 */
const SODIUM_CRYPTO_SECRETBOX_MACBYTES = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_secretbox_NONCEBYTES
 */
const SODIUM_CRYPTO_SECRETBOX_NONCEBYTES = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_sign_BYTES
 */
const SODIUM_CRYPTO_SIGN_BYTES = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_sign_SEEDBYTES
 */
const SODIUM_CRYPTO_SIGN_SEEDBYTES = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_sign_PUBLICKEYBYTES
 */
const SODIUM_CRYPTO_SIGN_PUBLICKEYBYTES = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_sign_SECRETKEYBYTES
 */
const SODIUM_CRYPTO_SIGN_SECRETKEYBYTES = UNKNOWN;
/**
 * @var int
 * @cvalue SODIUM_CRYPTO_SIGN_KEYPAIRBYTES()
 */
const SODIUM_CRYPTO_SIGN_KEYPAIRBYTES = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_stream_NONCEBYTES
 */
const SODIUM_CRYPTO_STREAM_NONCEBYTES = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_stream_KEYBYTES
 */
const SODIUM_CRYPTO_STREAM_KEYBYTES = UNKNOWN;

#ifdef crypto_stream_xchacha20_KEYBYTES
/**
 * @var int
 * @cvalue crypto_stream_xchacha20_NONCEBYTES
 */
const SODIUM_CRYPTO_STREAM_XCHACHA20_NONCEBYTES = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_stream_xchacha20_KEYBYTES
 */
const SODIUM_CRYPTO_STREAM_XCHACHA20_KEYBYTES = UNKNOWN;
#endif
#ifdef sodium_base64_VARIANT_ORIGINAL
/**
 * @var int
 * @cvalue sodium_base64_VARIANT_ORIGINAL
 */
const SODIUM_BASE64_VARIANT_ORIGINAL = UNKNOWN;
/**
 * @var int
 * @cvalue sodium_base64_VARIANT_ORIGINAL_NO_PADDING
 */
const SODIUM_BASE64_VARIANT_ORIGINAL_NO_PADDING = UNKNOWN;
/**
 * @var int
 * @cvalue sodium_base64_VARIANT_URLSAFE
 */
const SODIUM_BASE64_VARIANT_URLSAFE = UNKNOWN;
/**
 * @var int
 * @cvalue sodium_base64_VARIANT_URLSAFE_NO_PADDING
 */
const SODIUM_BASE64_VARIANT_URLSAFE_NO_PADDING = UNKNOWN;
#endif
#ifdef crypto_core_ristretto255_HASHBYTES
/**
 * @var int
 * @cvalue crypto_scalarmult_ristretto255_BYTES
 */
const SODIUM_CRYPTO_SCALARMULT_RISTRETTO255_BYTES = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_scalarmult_ristretto255_SCALARBYTES
 */
const SODIUM_CRYPTO_SCALARMULT_RISTRETTO255_SCALARBYTES = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_core_ristretto255_BYTES
 */
const SODIUM_CRYPTO_CORE_RISTRETTO255_BYTES = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_core_ristretto255_HASHBYTES
 */
const SODIUM_CRYPTO_CORE_RISTRETTO255_HASHBYTES = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_core_ristretto255_SCALARBYTES
 */
const SODIUM_CRYPTO_CORE_RISTRETTO255_SCALARBYTES = UNKNOWN;
/**
 * @var int
 * @cvalue crypto_core_ristretto255_NONREDUCEDSCALARBYTES
 */
const SODIUM_CRYPTO_CORE_RISTRETTO255_NONREDUCEDSCALARBYTES = UNKNOWN;
#endif

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
