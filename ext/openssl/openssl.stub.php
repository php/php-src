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
