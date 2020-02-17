<?php

function openssl_x509_export_to_file($x509, string $outfilename, bool $notext = true): bool {}

function openssl_x509_export($x509 , &$out, bool $notext = true): bool {}

function openssl_x509_fingerprint($x509, string $method = 'sha1', bool $raw_output = false): string|false {}

function openssl_x509_check_private_key($cert, $key): bool {}

function openssl_x509_verify($cert, $key): int {}

function openssl_x509_parse($x509, bool $shortname = true): array|false {}

function openssl_x509_checkpurpose($x509cert, int $purpose, ?array $cainfo = [], string $untrustedfile = UNKNOWN): bool|int {}

/** @return resource|false */
function openssl_x509_read($cert) {}

function openssl_x509_free($x509): ?bool {}

/**
 * @param resource|string $x509cert
 * @param resource|string|array $priv_key
 */
function openssl_pkcs12_export_to_file($x509cert, string $filename, $priv_key, string $pass, array $args = UNKNOWN): bool {}

/**
 * @param resource|string $x509
 * @param resource|string|array $priv_key
 */
function openssl_pkcs12_export($x509 , &$out, $priv_key, string $pass, array $args = UNKNOWN): bool {}

function openssl_pkcs12_read(string $pkcs12, &$certs, string $pass): bool {}

/** @param resource $csr */
function openssl_csr_export_to_file($csr, string $outfilename, bool $notext = true): bool {}

/** @param resource $csr */
function openssl_csr_export($csr, &$out, bool $notext = true): bool {}

/**
 * @param resource|string $csr
 * @param resource|string $cacert
 * @param resource|string|array $priv_key
 * @return resource|false
 */
function openssl_csr_sign($csr, $cacert = null, $priv_key, int $days, ?array $config_args = null, int $serial = 0) {}

/** @return resource|false */
function openssl_csr_new(array $dn, &$privkey, ?array $configargs = null, ?array $extraattribs = null) {}

/** @param resource|string $csr */
function openssl_csr_get_subject($csr, bool $use_shortnames = true): array|false {}

/**
 * @param resource|string $csr
 * @return resource|false
 */
function openssl_csr_get_public_key($csr, bool $use_shortnames = true) {}

/** @return resource|false */
function openssl_pkey_new(?array $configargs = null) {}

/** @param resource|string|array $key */
function openssl_pkey_export_to_file($key, string $outfilename, ?string $passphrase = null, ?array $configargs = null): bool {}

/** @param resource|string|array $key */
function openssl_pkey_export($key, &$out, ?string $passphrase = null, ?array $configargs = null): bool {}

/**
 * @param resource|string|array $cert
 * @return resource|false
 */
function openssl_pkey_get_public($cert) {}

/** @param resource $key */
function openssl_pkey_free($key): void {}

/**
 * @param resource|string|array $key
 * @return resource|false
 */
function openssl_pkey_get_private($key, string $passphrase = UNKNOWN) {}

/** @param resource $key */
function openssl_pkey_get_details($key): array|false {}

function openssl_pbkdf2(string $password, string $salt, int $key_length, int $iterations, string $digest_algorithm = 'sha1'): string|false {}

function openssl_pkcs7_verify(string $filename, int $flags, string $signerscerts = UNKNOWN, array $cainfo = UNKNOWN, string $extracerts = UNKNOWN, string $content = UNKNOWN, string $pk7 = UNKNOWN): bool|int {}

/** @param resource|string|array $recipcerts */
function openssl_pkcs7_encrypt(string $infile, string $outfile, $recipcerts, ?array $headers, int $flags = 0, int $cipher = OPENSSL_CIPHER_RC2_40): bool {}

/**
 * @param resource|string $signcert
 * @param resource|string|array $signkey
 */
function openssl_pkcs7_sign(string $infile, string $outfile, $signcert, $signkey, ?array $headers, int $flags = PKCS7_DETACHED, ?string $extracertsfilename = null): bool {}

/**
 * @param resource|string $recipcert
 * @param resource|string|array $recipkey
 */
function openssl_pkcs7_decrypt(string $infilename, string $outfilename, $recipcert, $recipkey = UNKNOWN): bool {}

function openssl_pkcs7_read(string $infilename, &$certs): bool {}

/** @param resource|string|array $key */
function openssl_private_encrypt(string $data, &$crypted, $key, int $padding = OPENSSL_PKCS1_PADDING): bool {}

/** @param resource|string|array $key */
function openssl_private_decrypt(string $data, &$crypted, $key, int $padding = OPENSSL_PKCS1_PADDING): bool {}

/** @param resource|string|array $key */
function openssl_public_encrypt(string $data, &$crypted, $key, int $padding = OPENSSL_PKCS1_PADDING): bool {}

/** @param resource|string|array $key */
function openssl_public_decrypt(string $data, &$crypted, $key, int $padding = OPENSSL_PKCS1_PADDING): bool {}

function openssl_error_string(): string|false {}

/**
 * @param resource|string|array $key
 * @param int|string $method
 */
function openssl_sign(string $data, &$signature, $key, $method = OPENSSL_ALGO_SHA1): bool {}

/**
 * @param resource|string|array $key
 * @param int|string $method
 */
function openssl_verify(string $data, string $signature, $key, $method = OPENSSL_ALGO_SHA1): int|false {}

function openssl_seal(string $data, &$sealdata, &$ekeys, array $pubkeys, string $method = UNKNOWN, &$iv = UNKNOWN): int|false {}

/** @param resource|string|array $privkey */
function openssl_open(string $data, &$opendata, string $ekey, $privkey, string $method = UNKNOWN, string $iv = UNKNOWN): bool {}

function openssl_get_md_methods($aliases = false): array {}

function openssl_get_cipher_methods($aliases = false): array {}

#ifdef HAVE_EVP_PKEY_EC
function openssl_get_curve_names(): array|false {}
#endif

function openssl_digest(string $data, string $method, bool $raw_output = false): string|false {}

function openssl_encrypt(string $data, string $method, string $password, int $options = 0, string $iv = '', &$tag = UNKNOWN, string $aad = '', int $tag_length = 16): string|false {}

function openssl_decrypt(string $data, string $method, string $password, int $options = 0, string $iv = '', string $tag = UNKNOWN, string $aad = ''): string|false {}

function openssl_cipher_iv_length(string $method): int|false {}

/** @param resource $dh_key */
function openssl_dh_compute_key(string $pub_key, $dh_key): string|false {}

/**
 * @param resource|string|array $peer_pub_key
 * @param resource|string|array $priv_key
 */
function openssl_pkey_derive($peer_pub_key, $priv_key, int $keylen = 0): string|false {}

function openssl_random_pseudo_bytes(int $length, &$result_is_strong = UNKNOWN): string {}

/** @param resource $privkey */
function openssl_spki_new($privkey, string $challenge, int $algo = OPENSSL_ALGO_MD5): string|false {}

function openssl_spki_verify(string $spki): bool {}

function openssl_spki_export(string $spki): string|false {}

function openssl_spki_export_challenge(string $spki): string|false {}

function openssl_get_cert_locations(): array {}
