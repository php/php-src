<?php

/** @generate-function-entries */

final class X509Certificate
{
}

final class X509CertificateSigningRequest
{
}

final class OpenSSLKey
{
}

function openssl_x509_export_to_file(X509Certificate|string $x509, string $outfilename, bool $notext = true): bool {}

function openssl_x509_export(X509Certificate|string $x509, &$out, bool $notext = true): bool {}

function openssl_x509_fingerprint(X509Certificate|string $x509, string $method = "sha1", bool $raw_output = false): string|false {}

/** @param OpenSSLKey|X509Certificate|string|array $key */
function openssl_x509_check_private_key(X509Certificate|string $x509, $key): bool {}

/** @param OpenSSLKey|X509Certificate|string|array $key */
function openssl_x509_verify(X509Certificate|string $x509, $key): int {}

function openssl_x509_parse(X509Certificate|string $x509, bool $shortname = true): array|false {}

function openssl_x509_checkpurpose(X509Certificate|string $x509, int $purpose, ?array $cainfo = [], ?string $untrustedfile = null): bool|int {}

function openssl_x509_read(X509Certificate|string $x509): X509Certificate|false {}

/** @deprecated */
function openssl_x509_free(X509Certificate $x509): void {}

/** @param OpenSSLKey|X509Certificate|string|array $priv_key */
function openssl_pkcs12_export_to_file(X509Certificate|string $x509cert, string $filename, $priv_key, string $pass, array $args = []): bool {}

/** @param OpenSSLKey|X509Certificate|string|array $priv_key */
function openssl_pkcs12_export(X509Certificate|string $x509, &$out, $priv_key, string $pass, array $args = []): bool {}

/** @param array $certs */
function openssl_pkcs12_read(string $pkcs12, &$certs, string $pass): bool {}

function openssl_csr_export_to_file(X509CertificateSigningRequest|string $csr, string $outfilename, bool $notext = true): bool {}

/** @param OpenSSLKey $out */
function openssl_csr_export(X509CertificateSigningRequest|string $csr, &$out, bool $notext = true): bool {}

/** @param OpenSSLKey|X509Certificate|string|array $priv_key */
function openssl_csr_sign(X509CertificateSigningRequest|string $csr, X509Certificate|string|null $cacert, $priv_key, int $days, ?array $config_args = null, int $serial = 0): X509Certificate|false {}

/** @param OpenSSLKey $privkey */
function openssl_csr_new(array $dn, &$privkey, ?array $configargs = null, ?array $extraattribs = null): X509CertificateSigningRequest|false {}

function openssl_csr_get_subject(X509CertificateSigningRequest|string $csr, bool $use_shortnames = true): array|false {}

function openssl_csr_get_public_key(X509CertificateSigningRequest|string $csr, bool $use_shortnames = true): OpenSSLKey|false {}

function openssl_pkey_new(?array $configargs = null): OpenSSLKey|false {}

/**
 * @param OpenSSLKey|X509Certificate|string|array $key
 * @param string $out
 */
function openssl_pkey_export_to_file($key, string $outfilename, ?string $passphrase = null, ?array $configargs = null): bool {}

/**
 * @param OpenSSLKey|X509Certificate|string|array $key
 * @param string $out
 */
function openssl_pkey_export($key, &$out, ?string $passphrase = null, ?array $configargs = null): bool {}

/** @param OpenSSLKey|X509Certificate|string|array $cert */
function openssl_pkey_get_public($cert): OpenSSLKey|false {}

/**
 * @param OpenSSLKey|X509Certificate|string|array $cert
 * @alias openssl_pkey_get_public
 */
function openssl_get_publickey($cert): OpenSSLKey|false {}

/** @deprecated */
function openssl_pkey_free(OpenSSLKey $key): void {}

/**
 * @alias openssl_pkey_free
 * @deprecated
 */
function openssl_free_key(OpenSSLKey $key): void {}

/** @param OpenSSLKey|X509Certificate|string|array $key */
function openssl_pkey_get_private($key, string $passphrase = UNKNOWN): OpenSSLKey|false {}

/**
 * @param OpenSSLKey|X509Certificate|string|array $key
 * @alias openssl_pkey_get_private
 */
function openssl_get_privatekey($key, string $passphrase = UNKNOWN): OpenSSLKey|false {}

function openssl_pkey_get_details(OpenSSLKey $key): array|false {}

function openssl_pbkdf2(string $password, string $salt, int $key_length, int $iterations, string $digest_algorithm = 'sha1'): string|false {}

function openssl_pkcs7_verify(string $filename, int $flags, string $signerscerts = UNKNOWN, array $cainfo = UNKNOWN, string $extracerts = UNKNOWN, string $content = UNKNOWN, string $pk7 = UNKNOWN): bool|int {}

/** @param X509Certificate|string|array $recipcerts */
function openssl_pkcs7_encrypt(string $infile, string $outfile, $recipcerts, ?array $headers, int $flags = 0, int $cipher = OPENSSL_CIPHER_RC2_40): bool {}

/** @param OpenSSLKey|X509Certificate|string|array $signkey */
function openssl_pkcs7_sign(string $infile, string $outfile, X509Certificate|string $signcert, $signkey, ?array $headers, int $flags = PKCS7_DETACHED, ?string $extracertsfilename = null): bool {}

/** @param OpenSSLKey|X509Certificate|string|array|null $recipkey */
function openssl_pkcs7_decrypt(string $infilename, string $outfilename, X509Certificate|string $recipcert, $recipkey = null): bool {}

function openssl_pkcs7_read(string $infilename, &$certs): bool {}

function openssl_cms_verify(string $filename, int $flags = 0, ?string $signerscerts = null, ?array $cainfo = null, ?string $extracerts = null, ?string $content = null, ?string $pk7 = null, ?string $sigfile = null, $encoding = OPENSSL_ENCODING_SMIME): bool {}

/** @param X509Certificate|string|array $recipcerts */
function openssl_cms_encrypt(string $infile, string $outfile, $recipcerts, ?array $headers, int $flags = 0, int $encoding = OPENSSL_ENCODING_SMIME,  int $cipher = OPENSSL_CIPHER_RC2_40): bool {}

/** @param OpenSSLKey|X509Certificate|string|array $signkey */
function openssl_cms_sign(string $infile, string $outfile, X509Certificate|string $signcert, $signkey, ?array $headers, int $flags = 0, int $encoding = OPENSSL_ENCODING_SMIME, ?string $extracertsfilename = null): bool {}

/** @param OpenSSLKey|X509Certificate|string|array $recipkey */
function openssl_cms_decrypt(string $infilename, string $outfilename, X509Certificate|string $recipcert, $recipkey, int $encoding = OPENSSL_ENCODING_SMIME): bool {}

/** @param array $certs */
function openssl_cms_read(string $infilename, &$certs): bool {}

/**
 * @param string $crypted
 * @param OpenSSLKey|X509Certificate|string|array $key
 */
function openssl_private_encrypt(string $data, &$crypted, $key, int $padding = OPENSSL_PKCS1_PADDING): bool {}

/**
 * @param string $crypted
 * @param OpenSSLKey|X509Certificate|string|array $key
 */
function openssl_private_decrypt(string $data, &$crypted, $key, int $padding = OPENSSL_PKCS1_PADDING): bool {}

/**
 * @param string $crypted
 * @param OpenSSLKey|X509Certificate|string|array $key
 */
function openssl_public_encrypt(string $data, &$crypted, $key, int $padding = OPENSSL_PKCS1_PADDING): bool {}

/**
 * @param string $crypted
 * @param OpenSSLKey|X509Certificate|string|array $key
 */
function openssl_public_decrypt(string $data, &$crypted, $key, int $padding = OPENSSL_PKCS1_PADDING): bool {}

function openssl_error_string(): string|false {}

/**
 * @param string $signature
 * @param OpenSSLKey|X509Certificate|string|array $key
 * @param int|string $method
 */
function openssl_sign(string $data, &$signature, $key, $method = OPENSSL_ALGO_SHA1): bool {}

/**
 * @param OpenSSLKey|X509Certificate|string|array $key
 * @param int|string $method
 */
function openssl_verify(string $data, string $signature, $key, $method = OPENSSL_ALGO_SHA1): int|false {}

/**
 * @param string $sealdata
 * @param array $ekeys
 * @param OpenSSLKey|X509Certificate|string|array $pubkeys
 * @param string $iv
 */
function openssl_seal(string $data, &$sealdata, &$ekeys, array $pubkeys, string $method = UNKNOWN, &$iv = UNKNOWN): int|false {}

/**
 * @param string $opendata
 * @param OpenSSLKey|X509Certificate|string|array $privkey
 */
function openssl_open(string $data, &$opendata, string $ekey, $privkey, string $method = UNKNOWN, string $iv = UNKNOWN): bool {}

function openssl_get_md_methods(bool $aliases = false): array {}

function openssl_get_cipher_methods(bool $aliases = false): array {}

#ifdef HAVE_EVP_PKEY_EC
function openssl_get_curve_names(): array|false {}
#endif

function openssl_digest(string $data, string $method, bool $raw_output = false): string|false {}

/** @param string $tag */
function openssl_encrypt(string $data, string $method, string $password, int $options = 0, string $iv = '', &$tag = UNKNOWN, string $aad = '', int $tag_length = 16): string|false {}

function openssl_decrypt(string $data, string $method, string $password, int $options = 0, string $iv = '', string $tag = UNKNOWN, string $aad = ''): string|false {}

function openssl_cipher_iv_length(string $method): int|false {}

function openssl_dh_compute_key(string $pub_key, OpenSSLKey $dh_key): string|false {}

/**
 * @param OpenSSLKey|X509Certificate|string|array $peer_pub_key
 * @param OpenSSLKey|X509Certificate|string|array $priv_key
 */
function openssl_pkey_derive($peer_pub_key, $priv_key, int $keylen = 0): string|false {}

/** @param bool $result_is_strong */
function openssl_random_pseudo_bytes(int $length, &$result_is_strong = UNKNOWN): string {}

function openssl_spki_new(OpenSSLKey $privkey, string $challenge, int $algo = OPENSSL_ALGO_MD5): string|false {}

function openssl_spki_verify(string $spki): bool {}

function openssl_spki_export(string $spki): string|false {}

function openssl_spki_export_challenge(string $spki): string|false {}

function openssl_get_cert_locations(): array {}
