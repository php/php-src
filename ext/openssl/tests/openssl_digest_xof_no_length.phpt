--TEST--
openssl_digest() and openssl_x509_fingerprint() with XOF digests that have no default length
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (OPENSSL_VERSION_NUMBER < 0x30400000) die("skip OpenSSL 3.4 or later required: SHAKE has a default length before");
?>
--FILE--
<?php

$cert = "file://" . __DIR__ . "/cert.crt";

foreach (['shake128', 'shake256'] as $algo) {
    var_dump(openssl_digest('abc', $algo));
    var_dump(openssl_digest('abc', $algo, true));
    var_dump(openssl_x509_fingerprint($cert, $algo));
}

echo "Fixed length digests are not affected\n";
var_dump(openssl_digest('abc', 'sha3-256'));
var_dump(strlen(openssl_x509_fingerprint($cert, 'sha3-256')));

?>
--EXPECTF--
Warning: openssl_digest(): Unsupported digest algorithm: output length must be specified in %s on line %d
bool(false)

Warning: openssl_digest(): Unsupported digest algorithm: output length must be specified in %s on line %d
bool(false)

Warning: openssl_x509_fingerprint(): Unsupported digest algorithm: output length must be specified in %s on line %d
bool(false)

Warning: openssl_digest(): Unsupported digest algorithm: output length must be specified in %s on line %d
bool(false)

Warning: openssl_digest(): Unsupported digest algorithm: output length must be specified in %s on line %d
bool(false)

Warning: openssl_x509_fingerprint(): Unsupported digest algorithm: output length must be specified in %s on line %d
bool(false)
Fixed length digests are not affected
string(64) "3a985da74fe225b2045c172d6bd390bd855f086e3e9d525b46bfe24511431532"
int(64)
