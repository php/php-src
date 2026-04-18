--TEST--
phar: EVP_PKEY freed on EVP_MD_CTX_create/EVP_VerifyInit failure in phar_verify_signature
--EXTENSIONS--
phar
--SKIPIF--
<?php
if (!in_array('OpenSSL', Phar::getSupportedSignatures())) {
    die('skip OpenSSL signature support required');
}
?>
--INI--
phar.require_hash=1
phar.readonly=0
--FILE--
<?php
// Exercise phar_verify_signature for all three OpenSSL digest types.
// Under ASAN, a missing EVP_PKEY_free on the EVP_MD_CTX_create/EVP_VerifyInit
// failure branch would produce a leak report.

$dir = __DIR__ . '/files/';

$p = new Phar($dir . 'openssl.phar');
var_dump($p->getSignature()['hash_type']);

$p = new Phar($dir . 'openssl256.phar');
var_dump($p->getSignature()['hash_type']);

$p = new Phar($dir . 'openssl512.phar');
var_dump($p->getSignature()['hash_type']);
?>
--EXPECT--
string(7) "OpenSSL"
string(14) "OpenSSL_SHA256"
string(14) "OpenSSL_SHA512"
