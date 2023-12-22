<?php

/** @generate-class-entries */

/**
 * @var string
 * @cvalue OPENSSL_VERSION_TEXT
 */
const OPENSSL_VERSION_TEXT = UNKNOWN;
/**
 * @var int
 * @cvalue OPENSSL_VERSION_NUMBER
 */
const OPENSSL_VERSION_NUMBER = UNKNOWN;

/**
 * @var int
 * @cvalue X509_PURPOSE_SSL_CLIENT
 */
const X509_PURPOSE_SSL_CLIENT = UNKNOWN;
/**
 * @var int
 * @cvalue X509_PURPOSE_SSL_SERVER
 */
const X509_PURPOSE_SSL_SERVER = UNKNOWN;

/**
 * @var int
 * @cvalue X509_PURPOSE_NS_SSL_SERVER
 */
const X509_PURPOSE_NS_SSL_SERVER = UNKNOWN;
/**
 * @var int
 * @cvalue X509_PURPOSE_SMIME_SIGN
 */
const X509_PURPOSE_SMIME_SIGN = UNKNOWN;
/**
 * @var int
 * @cvalue X509_PURPOSE_SMIME_ENCRYPT
 */
const X509_PURPOSE_SMIME_ENCRYPT = UNKNOWN;
/**
 * @var int
 * @cvalue X509_PURPOSE_CRL_SIGN
 */
const X509_PURPOSE_CRL_SIGN = UNKNOWN;
#ifdef X509_PURPOSE_ANY
/**
 * @var int
 * @cvalue X509_PURPOSE_ANY
 */
const X509_PURPOSE_ANY = UNKNOWN;
#endif

/* digest algorithm constants */

/**
 * @var int
 * @cvalue OPENSSL_ALGO_SHA1
 */
const OPENSSL_ALGO_SHA1 = UNKNOWN;
/**
 * @var int
 * @cvalue OPENSSL_ALGO_MD5
 */
const OPENSSL_ALGO_MD5 = UNKNOWN;
#ifndef OPENSSL_NO_MD4
/**
 * @var int
 * @cvalue OPENSSL_ALGO_MD4
 */
const OPENSSL_ALGO_MD4 = UNKNOWN;
#endif
#ifndef OPENSSL_NO_MD2
/**
 * @var int
 * @cvalue OPENSSL_ALGO_MD2
 */
const OPENSSL_ALGO_MD2 = UNKNOWN;
#endif
#if PHP_OPENSSL_API_VERSION < 0x10100
/**
 * @var int
 * @cvalue OPENSSL_ALGO_DSS1
 */
const OPENSSL_ALGO_DSS1 = UNKNOWN;
#endif

/**
 * @var int
 * @cvalue OPENSSL_ALGO_SHA224
 */
const OPENSSL_ALGO_SHA224 = UNKNOWN;
/**
 * @var int
 * @cvalue OPENSSL_ALGO_SHA256
 */
const OPENSSL_ALGO_SHA256 = UNKNOWN;
/**
 * @var int
 * @cvalue OPENSSL_ALGO_SHA384
 */
const OPENSSL_ALGO_SHA384 = UNKNOWN;
/**
 * @var int
 * @cvalue OPENSSL_ALGO_SHA512
 */
const OPENSSL_ALGO_SHA512 = UNKNOWN;
#ifndef OPENSSL_NO_RMD160
/**
 * @var int
 * @cvalue OPENSSL_ALGO_RMD160
 */
const OPENSSL_ALGO_RMD160 = UNKNOWN;
#endif

/* flags for S/MIME */

/**
 * @var int
 * @cvalue PKCS7_DETACHED
 */
const PKCS7_DETACHED = UNKNOWN;
/**
 * @var int
 * @cvalue PKCS7_TEXT
 */
const PKCS7_TEXT = UNKNOWN;
/**
 * @var int
 * @cvalue PKCS7_NOINTERN
 */
const PKCS7_NOINTERN = UNKNOWN;
/**
 * @var int
 * @cvalue PKCS7_NOVERIFY
 */
const PKCS7_NOVERIFY = UNKNOWN;
/**
 * @var int
 * @cvalue PKCS7_NOCHAIN
 */
const PKCS7_NOCHAIN = UNKNOWN;
/**
 * @var int
 * @cvalue PKCS7_NOCERTS
 */
const PKCS7_NOCERTS = UNKNOWN;
/**
 * @var int
 * @cvalue PKCS7_NOATTR
 */
const PKCS7_NOATTR = UNKNOWN;
/**
 * @var int
 * @cvalue PKCS7_BINARY
 */
const PKCS7_BINARY = UNKNOWN;
/**
 * @var int
 * @cvalue PKCS7_NOSIGS
 */
const PKCS7_NOSIGS = UNKNOWN;
/**
 * @var int
 * @cvalue PKCS7_NOOLDMIMETYPE
 */
const PKCS7_NOOLDMIMETYPE = UNKNOWN;

/**
 * @var int
 * @cvalue CMS_DETACHED
 */
const OPENSSL_CMS_DETACHED = UNKNOWN;
/**
 * @var int
 * @cvalue CMS_TEXT
 */
const OPENSSL_CMS_TEXT = UNKNOWN;
/**
 * @var int
 * @cvalue CMS_NOINTERN
 */
const OPENSSL_CMS_NOINTERN = UNKNOWN;
/**
 * @var int
 * @cvalue CMS_NOVERIFY
 */
const OPENSSL_CMS_NOVERIFY = UNKNOWN;
/**
 * @var int
 * @cvalue CMS_NOCERTS
 */
const OPENSSL_CMS_NOCERTS = UNKNOWN;
/**
 * @var int
 * @cvalue CMS_NOATTR
 */
const OPENSSL_CMS_NOATTR = UNKNOWN;
/**
 * @var int
 * @cvalue CMS_BINARY
 */
const OPENSSL_CMS_BINARY = UNKNOWN;
/**
 * @var int
 * @cvalue CMS_NOSIGS
 */
const OPENSSL_CMS_NOSIGS = UNKNOWN;
/**
 * @var int
 * @cvalue CMS_NOOLDMIMETYPE
 */
const OPENSSL_CMS_OLDMIMETYPE = UNKNOWN;

/**
 * @var int
 * @cvalue RSA_PKCS1_PADDING
 */
const OPENSSL_PKCS1_PADDING = UNKNOWN;
#ifdef RSA_SSLV23_PADDING
/**
 * @var int
 * @cvalue RSA_SSLV23_PADDING
 */
const OPENSSL_SSLV23_PADDING = UNKNOWN;
#endif

/**
 * @var int
 * @cvalue RSA_NO_PADDING
 */
const OPENSSL_NO_PADDING = UNKNOWN;
/**
 * @var int
 * @cvalue RSA_PKCS1_OAEP_PADDING
 */
const OPENSSL_PKCS1_OAEP_PADDING = UNKNOWN;

/* Informational stream wrapper constants */

/**
 * @var string
 * @cvalue OPENSSL_DEFAULT_STREAM_CIPHERS
 */
const OPENSSL_DEFAULT_STREAM_CIPHERS = UNKNOWN;

/* Ciphers */

#ifndef OPENSSL_NO_RC2
/**
 * @var int
 * @cvalue PHP_OPENSSL_CIPHER_RC2_40
 */
const OPENSSL_CIPHER_RC2_40 = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_OPENSSL_CIPHER_RC2_128
 */
const OPENSSL_CIPHER_RC2_128 = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_OPENSSL_CIPHER_RC2_64
 */
const OPENSSL_CIPHER_RC2_64 = UNKNOWN;
#endif

#ifndef OPENSSL_NO_DES
/**
 * @var int
 * @cvalue PHP_OPENSSL_CIPHER_DES
 */
const OPENSSL_CIPHER_DES = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_OPENSSL_CIPHER_3DES
 */
const OPENSSL_CIPHER_3DES = UNKNOWN;
#endif

#ifndef OPENSSL_NO_AES
/**
 * @var int
 * @cvalue PHP_OPENSSL_CIPHER_AES_128_CBC
 */
const OPENSSL_CIPHER_AES_128_CBC = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_OPENSSL_CIPHER_AES_192_CBC
 */
const OPENSSL_CIPHER_AES_192_CBC = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_OPENSSL_CIPHER_AES_256_CBC
 */
const OPENSSL_CIPHER_AES_256_CBC = UNKNOWN;
#endif

/* Values for key types */

/**
 * @var int
 * @cvalue OPENSSL_KEYTYPE_RSA
 */
const OPENSSL_KEYTYPE_RSA = UNKNOWN;
#ifndef OPENSSL_NO_DSA
/**
 * @var int
 * @cvalue OPENSSL_KEYTYPE_DSA
 */
const OPENSSL_KEYTYPE_DSA = UNKNOWN;
#endif

/**
 * @var int
 * @cvalue OPENSSL_KEYTYPE_DH
 */
const OPENSSL_KEYTYPE_DH = UNKNOWN;
#ifdef HAVE_EVP_PKEY_EC
/**
 * @var int
 * @cvalue OPENSSL_KEYTYPE_EC
 */
const OPENSSL_KEYTYPE_EC = UNKNOWN;
#endif

/**
 * @var int
 * @cvalue OPENSSL_RAW_DATA
 */
const OPENSSL_RAW_DATA = UNKNOWN;
/**
 * @var int
 * @cvalue OPENSSL_ZERO_PADDING
 */
const OPENSSL_ZERO_PADDING = UNKNOWN;
/**
 * @var int
 * @cvalue OPENSSL_DONT_ZERO_PAD_KEY
 */
const OPENSSL_DONT_ZERO_PAD_KEY = UNKNOWN;

#ifndef OPENSSL_NO_TLSEXT
/** @var int */
const OPENSSL_TLSEXT_SERVER_NAME = 1;
#endif

/* Register encodings */

/**
 * @var int
 * @cvalue ENCODING_DER
 */
const OPENSSL_ENCODING_DER = UNKNOWN;
/**
 * @var int
 * @cvalue ENCODING_SMIME
 */
const OPENSSL_ENCODING_SMIME = UNKNOWN;
/**
 * @var int
 * @cvalue ENCODING_PEM
 */
const OPENSSL_ENCODING_PEM = UNKNOWN;


/**
 * @strict-properties
 * @not-serializable
 */
final class OpenSSLCertificate
{
}

/**
 * @strict-properties
 * @not-serializable
 */
final class OpenSSLCertificateSigningRequest
{
}

/**
 * @strict-properties
 * @not-serializable
 */
final class OpenSSLAsymmetricKey
{
}

function openssl_x509_export_to_file(OpenSSLCertificate|string $certificate, string $output_filename, bool $no_text = true): bool {}

/** @param string $output */
function openssl_x509_export(OpenSSLCertificate|string $certificate, &$output, bool $no_text = true): bool {}

function openssl_x509_fingerprint(OpenSSLCertificate|string $certificate, string $digest_algo = "sha1", bool $binary = false): string|false {}

/**
 * @param OpenSSLAsymmetricKey|OpenSSLCertificate|array|string $private_key
 */
function openssl_x509_check_private_key(OpenSSLCertificate|string $certificate, #[\SensitiveParameter] $private_key): bool {}

/** @param OpenSSLAsymmetricKey|OpenSSLCertificate|array|string $public_key */
function openssl_x509_verify(OpenSSLCertificate|string $certificate, $public_key): int {}

/**
 * @return array<string, int|string|array>|false
 * @refcount 1
 */
function openssl_x509_parse(OpenSSLCertificate|string $certificate, bool $short_names = true): array|false {}

function openssl_x509_checkpurpose(OpenSSLCertificate|string $certificate, int $purpose, array $ca_info = [], ?string $untrusted_certificates_file = null): bool|int {}

function openssl_x509_read(OpenSSLCertificate|string $certificate): OpenSSLCertificate|false {}

/** @deprecated */
function openssl_x509_free(OpenSSLCertificate $certificate): void {}

/**
 * @param OpenSSLAsymmetricKey|OpenSSLCertificate|array|string $private_key
 */
function openssl_pkcs12_export_to_file(OpenSSLCertificate|string $certificate, string $output_filename, #[\SensitiveParameter] $private_key, #[\SensitiveParameter] string $passphrase, array $options = []): bool {}

/**
 * @param string $output
 * @param OpenSSLAsymmetricKey|OpenSSLCertificate|array|string $private_key
 */
function openssl_pkcs12_export(OpenSSLCertificate|string $certificate, &$output, #[\SensitiveParameter] $private_key, #[\SensitiveParameter] string $passphrase, array $options = []): bool {}

/**
 * @param array $certificates
 */
function openssl_pkcs12_read(string $pkcs12, &$certificates, #[\SensitiveParameter] string $passphrase): bool {}

function openssl_csr_export_to_file(OpenSSLCertificateSigningRequest|string $csr, string $output_filename, bool $no_text = true): bool {}

/** @param string $output */
function openssl_csr_export(OpenSSLCertificateSigningRequest|string $csr, &$output, bool $no_text = true): bool {}

/**
 * @param OpenSSLAsymmetricKey|OpenSSLCertificate|array|string $private_key
 */
function openssl_csr_sign(OpenSSLCertificateSigningRequest|string $csr, OpenSSLCertificate|string|null $ca_certificate, #[\SensitiveParameter] $private_key, int $days, ?array $options = null, int $serial = 0, ?string $serial_hex = null): OpenSSLCertificate|false {}

/**
 * @param OpenSSLAsymmetricKey $private_key
 */
function openssl_csr_new(array $distinguished_names, #[\SensitiveParameter] &$private_key, ?array $options = null, ?array $extra_attributes = null): OpenSSLCertificateSigningRequest|false {}

/**
 * @return array<string, string|array>|false
 * @refcount 1
 */
function openssl_csr_get_subject(OpenSSLCertificateSigningRequest|string $csr, bool $short_names = true): array|false {}

function openssl_csr_get_public_key(OpenSSLCertificateSigningRequest|string $csr, bool $short_names = true): OpenSSLAsymmetricKey|false {}

function openssl_pkey_new(?array $options = null): OpenSSLAsymmetricKey|false {}

/**
 * @param OpenSSLAsymmetricKey|OpenSSLCertificate|array|string $key
 */
function openssl_pkey_export_to_file(#[\SensitiveParameter] $key, string $output_filename, #[\SensitiveParameter] ?string $passphrase = null, ?array $options = null): bool {}

/**
 * @param OpenSSLAsymmetricKey|OpenSSLCertificate|array|string $key
 * @param string $output
 */
function openssl_pkey_export(#[\SensitiveParameter] $key, &$output, #[\SensitiveParameter] ?string $passphrase = null, ?array $options = null): bool {}

/** @param OpenSSLAsymmetricKey|OpenSSLCertificate|array|string $public_key */
function openssl_pkey_get_public($public_key): OpenSSLAsymmetricKey|false {}

/**
 * @param OpenSSLAsymmetricKey|OpenSSLCertificate|array|string $public_key
 * @alias openssl_pkey_get_public
 */
function openssl_get_publickey($public_key): OpenSSLAsymmetricKey|false {}

/**
 * @deprecated
 */
function openssl_pkey_free(OpenSSLAsymmetricKey $key): void {}

/**
 * @alias openssl_pkey_free
 * @deprecated
 */
function openssl_free_key(OpenSSLAsymmetricKey $key): void {}

/**
 * @param OpenSSLAsymmetricKey|OpenSSLCertificate|array|string $private_key
 */
function openssl_pkey_get_private(#[\SensitiveParameter] $private_key, #[\SensitiveParameter] ?string $passphrase = null): OpenSSLAsymmetricKey|false {}

/**
 * @param OpenSSLAsymmetricKey|OpenSSLCertificate|array|string $private_key
 * @alias openssl_pkey_get_private
 */
function openssl_get_privatekey(#[\SensitiveParameter] $private_key, #[\SensitiveParameter] ?string $passphrase = null): OpenSSLAsymmetricKey|false {}

/**
 * @return array<string, int|string|array>|false
 * @refcount 1
 */
function openssl_pkey_get_details(OpenSSLAsymmetricKey $key): array|false {}

function openssl_pbkdf2(#[\SensitiveParameter] string $password, string $salt, int $key_length, int $iterations, string $digest_algo = "sha1"): string|false {}

function openssl_pkcs7_verify(string $input_filename, int $flags, ?string $signers_certificates_filename = null, array $ca_info = [], ?string $untrusted_certificates_filename = null, ?string $content = null, ?string $output_filename = null): bool|int {}

/** @param OpenSSLCertificate|array|string $certificate */
function openssl_pkcs7_encrypt(string $input_filename, string $output_filename, $certificate, ?array $headers, int $flags = 0, int $cipher_algo = OPENSSL_CIPHER_AES_128_CBC): bool {}

/**
 * @param OpenSSLAsymmetricKey|OpenSSLCertificate|array|string $private_key
 */
function openssl_pkcs7_sign(string $input_filename, string $output_filename, OpenSSLCertificate|string $certificate, #[\SensitiveParameter] $private_key, ?array $headers, int $flags = PKCS7_DETACHED, ?string $untrusted_certificates_filename = null): bool {}

/**
 * @param OpenSSLCertificate|string $certificate
 * @param OpenSSLAsymmetricKey|OpenSSLCertificate|array|string|null $private_key
 */
function openssl_pkcs7_decrypt(string $input_filename, string $output_filename, #[\SensitiveParameter] $certificate, #[\SensitiveParameter] $private_key = null): bool {}

/** @param array $certificates */
function openssl_pkcs7_read(string $data, &$certificates): bool {}

function openssl_cms_verify(string $input_filename, int $flags = 0, ?string $certificates = null, array $ca_info = [], ?string $untrusted_certificates_filename = null, ?string $content = null, ?string $pk7 = null, ?string $sigfile = null, int $encoding = OPENSSL_ENCODING_SMIME): bool {}

/** @param OpenSSLCertificate|array|string $certificate */
function openssl_cms_encrypt(string $input_filename, string $output_filename, $certificate, ?array $headers, int $flags = 0, int $encoding = OPENSSL_ENCODING_SMIME, int $cipher_algo = OPENSSL_CIPHER_AES_128_CBC): bool {}

/**
 * @param OpenSSLAsymmetricKey|OpenSSLCertificate|array|string $private_key
 */
function openssl_cms_sign(string $input_filename, string $output_filename, OpenSSLCertificate|string $certificate, #[\SensitiveParameter] $private_key, ?array $headers, int $flags = 0, int $encoding = OPENSSL_ENCODING_SMIME, ?string $untrusted_certificates_filename = null): bool {}

/**
 * @param OpenSSLCertificate|string $certificate
 * @param OpenSSLAsymmetricKey|OpenSSLCertificate|array|string|null $private_key
 */
function openssl_cms_decrypt(string $input_filename, string $output_filename, #[\SensitiveParameter] $certificate, #[\SensitiveParameter] $private_key = null, int $encoding = OPENSSL_ENCODING_SMIME): bool {}

/** @param array $certificates */
function openssl_cms_read(string $input_filename, &$certificates): bool {}

/**
 * @param string $encrypted_data
 * @param OpenSSLAsymmetricKey|OpenSSLCertificate|array|string $private_key
 */
function openssl_private_encrypt(#[\SensitiveParameter] string $data, &$encrypted_data, #[\SensitiveParameter] $private_key, int $padding = OPENSSL_PKCS1_PADDING): bool {}

/**
 * @param string $decrypted_data
 * @param OpenSSLAsymmetricKey|OpenSSLCertificate|array|string $private_key
 */
function openssl_private_decrypt(string $data, #[\SensitiveParameter] &$decrypted_data, #[\SensitiveParameter] $private_key, int $padding = OPENSSL_PKCS1_PADDING): bool {}

/**
 * @param string $encrypted_data
 * @param OpenSSLAsymmetricKey|OpenSSLCertificate|array|string $public_key
 */
function openssl_public_encrypt(#[\SensitiveParameter] string $data, &$encrypted_data, $public_key, int $padding = OPENSSL_PKCS1_PADDING): bool {}

/**
 * @param string $decrypted_data
 * @param OpenSSLAsymmetricKey|OpenSSLCertificate|array|string $public_key
 */
function openssl_public_decrypt(string $data, #[\SensitiveParameter] &$decrypted_data, $public_key, int $padding = OPENSSL_PKCS1_PADDING): bool {}

function openssl_error_string(): string|false {}

/**
 * @param string $signature
 * @param OpenSSLAsymmetricKey|OpenSSLCertificate|array|string $private_key
 */
function openssl_sign(string $data, &$signature, #[\SensitiveParameter] $private_key, string|int $algorithm = OPENSSL_ALGO_SHA1): bool {}

/** @param OpenSSLAsymmetricKey|OpenSSLCertificate|array|string $public_key */
function openssl_verify(string $data, string $signature, $public_key, string|int $algorithm = OPENSSL_ALGO_SHA1): int|false {}

/**
 * @param string $sealed_data
 * @param array $encrypted_keys
 * @param string $iv
 */
function openssl_seal(#[\SensitiveParameter] string $data, &$sealed_data, &$encrypted_keys, array $public_key, string $cipher_algo, &$iv = null): int|false {}

/**
 * @param string $output
 * @param OpenSSLAsymmetricKey|OpenSSLCertificate|array|string $private_key
 */
function openssl_open(string $data, #[\SensitiveParameter] &$output, string $encrypted_key, #[\SensitiveParameter] $private_key, string $cipher_algo, ?string $iv = null): bool {}

/**
 * @return array<int, string>
 * @refcount 1
 */
function openssl_get_md_methods(bool $aliases = false): array {}

/**
 * @return array<int, string>
 * @refcount 1
 */
function openssl_get_cipher_methods(bool $aliases = false): array {}

#ifdef HAVE_EVP_PKEY_EC
/**
 * @return array<int, string>|false
 * @refcount 1
 */
function openssl_get_curve_names(): array|false {}
#endif

function openssl_digest(string $data, string $digest_algo, bool $binary = false): string|false {}

/**
 * @param string $tag
 */
function openssl_encrypt(#[\SensitiveParameter] string $data, string $cipher_algo, #[\SensitiveParameter] string $passphrase, int $options = 0, string $iv = "", &$tag = null, string $aad = "", int $tag_length = 16): string|false {}

function openssl_decrypt(string $data, string $cipher_algo, #[\SensitiveParameter] string $passphrase, int $options = 0, string $iv = "", ?string $tag = null, string $aad = ""): string|false {}

function openssl_cipher_iv_length(string $cipher_algo): int|false {}

function openssl_cipher_key_length(string $cipher_algo): int|false {}

function openssl_dh_compute_key(string $public_key, #[\SensitiveParameter] OpenSSLAsymmetricKey $private_key): string|false {}

/**
 * @param OpenSSLAsymmetricKey|OpenSSLCertificate|array|string $public_key
 * @param OpenSSLAsymmetricKey|OpenSSLCertificate|array|string $private_key
 */
function openssl_pkey_derive($public_key, #[\SensitiveParameter] $private_key, int $key_length = 0): string|false {}

/** @param bool $strong_result */
function openssl_random_pseudo_bytes(int $length, &$strong_result = null): string {}

function openssl_spki_new(#[\SensitiveParameter] OpenSSLAsymmetricKey $private_key, string $challenge, int $digest_algo = OPENSSL_ALGO_MD5): string|false {}

function openssl_spki_verify(string $spki): bool {}

function openssl_spki_export(string $spki): string|false {}

function openssl_spki_export_challenge(string $spki): string|false {}

/**
 * @return array<string, string>
 * @refcount 1
 */
function openssl_get_cert_locations(): array {}
