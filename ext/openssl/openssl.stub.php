<?php

/** @generate-function-entries */

final class OpenSSLCertificate
{
}

final class OpenSSLCertificateSigningRequest
{
}

final class OpenSSLAsymmetricKey
{
}

function openssl_x509_export_to_file(OpenSSLCertificate|string $x509, string $outfilename, bool $notext = true): bool {}

/** @param string $out */
function openssl_x509_export(OpenSSLCertificate|string $x509, &$out, bool $notext = true): bool {}

function openssl_x509_fingerprint(OpenSSLCertificate|string $x509, string $method = "sha1", bool $raw_output = false): string|false {}

/** @param OpenSSLAsymmetricKey|OpenSSLCertificate|array|string $key */
function openssl_x509_check_private_key(OpenSSLCertificate|string $x509, $key): bool {}

/** @param OpenSSLAsymmetricKey|OpenSSLCertificate|array|string $key */
function openssl_x509_verify(OpenSSLCertificate|string $x509, $key): int {}

function openssl_x509_parse(OpenSSLCertificate|string $x509, bool $shortname = true): array|false {}

function openssl_x509_checkpurpose(OpenSSLCertificate|string $x509, int $purpose, ?array $cainfo = [], ?string $untrustedfile = null): bool|int {}

function openssl_x509_read(OpenSSLCertificate|string $x509): OpenSSLCertificate|false {}

/** @deprecated */
function openssl_x509_free(OpenSSLCertificate $x509): void {}

/** @param OpenSSLAsymmetricKey|OpenSSLCertificate|array|string $priv_key */
function openssl_pkcs12_export_to_file(OpenSSLCertificate|string $x509cert, string $filename, $priv_key, string $pass, array $args = []): bool {}

/**
 * @param string $out
 * @param OpenSSLAsymmetricKey|OpenSSLCertificate|array|string $priv_key
 */
function openssl_pkcs12_export(OpenSSLCertificate|string $x509, &$out, $priv_key, string $pass, array $args = []): bool {}

/** @param array $certs */
function openssl_pkcs12_read(string $pkcs12, &$certs, string $pass): bool {}

function openssl_csr_export_to_file(OpenSSLCertificateSigningRequest|string $csr, string $outfilename, bool $notext = true): bool {}

/** @param OpenSSLAsymmetricKey $out */
function openssl_csr_export(OpenSSLCertificateSigningRequest|string $csr, &$out, bool $notext = true): bool {}

/** @param OpenSSLAsymmetricKey|OpenSSLCertificate|array|string $priv_key */
function openssl_csr_sign(OpenSSLCertificateSigningRequest|string $csr, OpenSSLCertificate|string|null $cacert, $priv_key, int $days, ?array $config_args = null, int $serial = 0): OpenSSLCertificate|false {}

/** @param OpenSSLAsymmetricKey $privkey */
function openssl_csr_new(array $dn, &$privkey, ?array $configargs = null, ?array $extraattribs = null): OpenSSLCertificateSigningRequest|false {}

function openssl_csr_get_subject(OpenSSLCertificateSigningRequest|string $csr, bool $use_shortnames = true): array|false {}

function openssl_csr_get_public_key(OpenSSLCertificateSigningRequest|string $csr, bool $use_shortnames = true): OpenSSLAsymmetricKey|false {}

function openssl_pkey_new(?array $configargs = null): OpenSSLAsymmetricKey|false {}

/**
 * @param OpenSSLAsymmetricKey|OpenSSLCertificate|array|string $key
 * @param string $out
 */
function openssl_pkey_export_to_file($key, string $outfilename, ?string $passphrase = null, ?array $configargs = null): bool {}

/**
 * @param OpenSSLAsymmetricKey|OpenSSLCertificate|array|string $key
 * @param string $out
 */
function openssl_pkey_export($key, &$out, ?string $passphrase = null, ?array $configargs = null): bool {}

/** @param OpenSSLAsymmetricKey|OpenSSLCertificate|array|string $cert */
function openssl_pkey_get_public($cert): OpenSSLAsymmetricKey|false {}

/**
 * @param OpenSSLAsymmetricKey|OpenSSLCertificate|array|string $cert
 * @alias openssl_pkey_get_public
 */
function openssl_get_publickey($cert): OpenSSLAsymmetricKey|false {}

/** @deprecated */
function openssl_pkey_free(OpenSSLAsymmetricKey $key): void {}

/**
 * @alias openssl_pkey_free
 * @deprecated
 */
function openssl_free_key(OpenSSLAsymmetricKey $key): void {}

/** @param OpenSSLAsymmetricKey|OpenSSLCertificate|array|string $key */
function openssl_pkey_get_private($key, string $passphrase = UNKNOWN): OpenSSLAsymmetricKey|false {}

/**
 * @param OpenSSLAsymmetricKey|OpenSSLCertificate|array|string $key
 * @alias openssl_pkey_get_private
 */
function openssl_get_privatekey($key, string $passphrase = UNKNOWN): OpenSSLAsymmetricKey|false {}

function openssl_pkey_get_details(OpenSSLAsymmetricKey $key): array|false {}

function openssl_pbkdf2(string $password, string $salt, int $key_length, int $iterations, string $digest_algorithm = 'sha1'): string|false {}

function openssl_pkcs7_verify(string $filename, int $flags, string $signerscerts = UNKNOWN, array $cainfo = UNKNOWN, string $extracerts = UNKNOWN, string $content = UNKNOWN, string $pk7 = UNKNOWN): bool|int {}

/** @param OpenSSLCertificate|array|string $recipcerts */
function openssl_pkcs7_encrypt(string $infile, string $outfile, $recipcerts, ?array $headers, int $flags = 0, int $cipher = OPENSSL_CIPHER_RC2_40): bool {}

/** @param OpenSSLAsymmetricKey|OpenSSLCertificate|array|string $signkey */
function openssl_pkcs7_sign(string $infile, string $outfile, OpenSSLCertificate|string $signcert, $signkey, ?array $headers, int $flags = PKCS7_DETACHED, ?string $extracertsfilename = null): bool {}

/** @param OpenSSLAsymmetricKey|OpenSSLCertificate|array|string|null $recipkey */
function openssl_pkcs7_decrypt(string $infilename, string $outfilename, OpenSSLCertificate|string $recipcert, $recipkey = null): bool {}

/** @param array $certs */
function openssl_pkcs7_read(string $infilename, &$certs): bool {}

function openssl_cms_verify(string $filename, int $flags = 0, ?string $signerscerts = null, ?array $cainfo = null, ?string $extracerts = null, ?string $content = null, ?string $pk7 = null, ?string $sigfile = null, int $encoding = OPENSSL_ENCODING_SMIME): bool {}

/** @param OpenSSLCertificate|array|string $recipcerts */
function openssl_cms_encrypt(string $infile, string $outfile, $recipcerts, ?array $headers, int $flags = 0, int $encoding = OPENSSL_ENCODING_SMIME,  int $cipher = OPENSSL_CIPHER_RC2_40): bool {}

/** @param OpenSSLAsymmetricKey|OpenSSLCertificate|array|string $signkey */
function openssl_cms_sign(string $infile, string $outfile, OpenSSLCertificate|string $signcert, $signkey, ?array $headers, int $flags = 0, int $encoding = OPENSSL_ENCODING_SMIME, ?string $extracertsfilename = null): bool {}

/** @param OpenSSLAsymmetricKey|OpenSSLCertificate|array|string $recipkey */
function openssl_cms_decrypt(string $infilename, string $outfilename, OpenSSLCertificate|string $recipcert, $recipkey, int $encoding = OPENSSL_ENCODING_SMIME): bool {}

/** @param array $certs */
function openssl_cms_read(string $infilename, &$certs): bool {}

/**
 * @param string $crypted
 * @param OpenSSLAsymmetricKey|OpenSSLCertificate|array|string $key
 */
function openssl_private_encrypt(string $data, &$crypted, $key, int $padding = OPENSSL_PKCS1_PADDING): bool {}

/**
 * @param string $crypted
 * @param OpenSSLAsymmetricKey|OpenSSLCertificate|array|string $key
 */
function openssl_private_decrypt(string $data, &$crypted, $key, int $padding = OPENSSL_PKCS1_PADDING): bool {}

/**
 * @param string $crypted
 * @param OpenSSLAsymmetricKey|OpenSSLCertificate|array|string $key
 */
function openssl_public_encrypt(string $data, &$crypted, $key, int $padding = OPENSSL_PKCS1_PADDING): bool {}

/**
 * @param string $crypted
 * @param OpenSSLAsymmetricKey|OpenSSLCertificate|array|string $key
 */
function openssl_public_decrypt(string $data, &$crypted, $key, int $padding = OPENSSL_PKCS1_PADDING): bool {}

function openssl_error_string(): string|false {}

/**
 * @param string $signature
 * @param OpenSSLAsymmetricKey|OpenSSLCertificate|array|string $key
 * @param int|string $method
 */
function openssl_sign(string $data, &$signature, $key, $method = OPENSSL_ALGO_SHA1): bool {}

/**
 * @param OpenSSLAsymmetricKey|OpenSSLCertificate|array|string $key
 * @param int|string $method
 */
function openssl_verify(string $data, string $signature, $key, $method = OPENSSL_ALGO_SHA1): int|false {}

/**
 * @param string $sealdata
 * @param array $ekeys
 * @param OpenSSLAsymmetricKey|OpenSSLCertificate|array|string $pubkeys
 * @param string $iv
 */
function openssl_seal(string $data, &$sealdata, &$ekeys, array $pubkeys, string $method = UNKNOWN, &$iv = UNKNOWN): int|false {}

/**
 * @param string $opendata
 * @param OpenSSLAsymmetricKey|OpenSSLCertificate|array|string $privkey
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

function openssl_dh_compute_key(string $pub_key, OpenSSLAsymmetricKey $dh_key): string|false {}

/**
 * @param OpenSSLAsymmetricKey|OpenSSLCertificate|array|string $peer_pub_key
 * @param OpenSSLAsymmetricKey|OpenSSLCertificate|array|string $priv_key
 */
function openssl_pkey_derive($peer_pub_key, $priv_key, int $keylen = 0): string|false {}

/** @param bool $result_is_strong */
function openssl_random_pseudo_bytes(int $length, &$result_is_strong = UNKNOWN): string {}

function openssl_spki_new(OpenSSLAsymmetricKey $privkey, string $challenge, int $algo = OPENSSL_ALGO_MD5): string|false {}

function openssl_spki_verify(string $spki): bool {}

function openssl_spki_export(string $spki): string|false {}

function openssl_spki_export_challenge(string $spki): string|false {}

function openssl_get_cert_locations(): array {}
