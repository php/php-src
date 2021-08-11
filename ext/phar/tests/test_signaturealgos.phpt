--TEST--
Phar: verify signature parsing works
--EXTENSIONS--
phar
--SKIPIF--
<?php
$arr = Phar::getSupportedSignatures();
if (!in_array("OpenSSL", $arr)) die("skip openssl support required");
?>
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php
$a = new Phar(__DIR__ . '/files/sha1.phar');
$r = $a->getSignature();
var_dump($r['hash_type']);
$a = new Phar(__DIR__ . '/files/sha512.phar');
$r = $a->getSignature();
var_dump($r['hash_type']);
$a = new Phar(__DIR__ . '/files/sha256.phar');
$r = $a->getSignature();
var_dump($r['hash_type']);
$a = new Phar(__DIR__ . '/files/md5.phar');
$r = $a->getSignature();
var_dump($r['hash_type']);
$a = new Phar(__DIR__ . '/files/openssl.phar');
$r = $a->getSignature();
var_dump($r['hash_type']);
$a = new Phar(__DIR__ . '/files/openssl256.phar');
$r = $a->getSignature();
var_dump($r['hash_type']);
$a = new Phar(__DIR__ . '/files/openssl512.phar');
$r = $a->getSignature();
var_dump($r['hash_type']);
?>
--EXPECT--
string(5) "SHA-1"
string(7) "SHA-512"
string(7) "SHA-256"
string(3) "MD5"
string(7) "OpenSSL"
string(14) "OpenSSL_SHA256"
string(14) "OpenSSL_SHA512"
