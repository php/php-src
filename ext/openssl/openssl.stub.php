<?php

function openssl_x509_export_to_file($x509, string $outfilename, bool $notext = true): bool {}

function openssl_x509_export($x509 , &$out, bool $notext = true): bool {}

/** @return string|false */
function openssl_x509_fingerprint($x509, string $method = 'sha1', bool $raw_output = false) {}

function openssl_x509_check_private_key($cert, $key): bool {}

function openssl_x509_verify($cert, $key): int {}

/** @return array|false */
function openssl_x509_parse($x509, bool $shortname = true) {}

/** @return bool|int */
function openssl_x509_checkpurpose($x509cert, int $purpose, ?array $cainfo = [], string $untrustedfile = UNKNOWN) {}

/** @return resource|false */
function openssl_x509_read($cert) {}

/** @return false|void */
function openssl_x509_free($x509) {}

function openssl_pkcs12_export_to_file($x509cert, string $filename, $priv_key, string $pass, array $args = UNKNOWN): bool {}

function openssl_pkcs12_export($x509 , &$out, $priv_key, string $pass, array $args = UNKNOWN): bool {}

function openssl_pkcs12_read(string $pkcs12, &$certs, string $pass): bool {}

function openssl_csr_export_to_file($csr, string $outfilename, bool $notext = true): bool {}

function openssl_csr_export($csr, &$out, bool $notext = true): bool {}

/** @return resource|false */
function openssl_csr_sign($csr, $cacert, $priv_key, int $days, array $config_args = UNKNOWN, int $serial = 0) {}

/** @return resource|false */
function openssl_csr_new($dn, &$privkey, array $configargs = UNKNOWN, array $extraattribs = UNKNOWN) {}

/** @return array|false */
function openssl_csr_get_subject($csr, bool $use_shortnames = true) {}

/** @return resource|false */
function openssl_csr_get_public_key($csr, bool $use_shortnames = true) {}

/** @return resource|false */
function openssl_pkey_new(array $configargs = UNKNOWN) {}

function openssl_pkey_export_to_file($key, string $outfilename, string $passphrase = UNKNOWN, array $configargs = UNKNOWN): bool {}

function openssl_pkey_export($key, &$out, string $passphrase = UNKNOWN, array $configargs = UNKNOWN): bool {}

/** @return resource|false */
function openssl_pkey_get_public($cert) {}

function openssl_pkey_free($key) {}

/** @return resource|false */
function openssl_pkey_get_private($key, string $passphrase = UNKNOWN) {}

/** @return array|false */
function openssl_pkey_get_details($key) {}

/** @return string|false */
function openssl_pbkdf2(string $password, string $salt, int $key_length, int $iterations, string $digest_algorithm = 'sha1') {}

/** @return bool|int */
function openssl_pkcs7_verify(string $filename, int $flags, string $signerscerts = UNKNOWN, array $cainfo = UNKNOWN, string $extracerts = UNKNOWN, string $content = UNKNOWN, string $pk7 = UNKNOWN) {}
