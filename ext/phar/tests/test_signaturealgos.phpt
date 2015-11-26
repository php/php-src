--TEST--
Phar: verify signature parsing works
--SKIPIF--
<?php
if (!extension_loaded("phar")) die("skip");
if (!extension_loaded("hash")) die("skip extension hash conflicts");
$arr = Phar::getSupportedSignatures();
if (!in_array("OpenSSL", $arr)) die("skip openssl support required");
if (!in_array('SHA-256', $arr)) die("skip hash extension loaded shared");
?>
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php
$a = new Phar(dirname(__FILE__) . '/files/sha1.phar');
$r = $a->getSignature();
var_dump($r['hash_type']);
$a = new Phar(dirname(__FILE__) . '/files/sha512.phar');
$r = $a->getSignature();
var_dump($r['hash_type']);
$a = new Phar(dirname(__FILE__) . '/files/sha256.phar');
$r = $a->getSignature();
var_dump($r['hash_type']);
$a = new Phar(dirname(__FILE__) . '/files/md5.phar');
$r = $a->getSignature();
var_dump($r['hash_type']);
$a = new Phar(dirname(__FILE__) . '/files/openssl.phar');
$r = $a->getSignature();
var_dump($r['hash_type']);
?>
===DONE===
--EXPECT--
string(5) "SHA-1"
string(7) "SHA-512"
string(7) "SHA-256"
string(3) "MD5"
string(7) "OpenSSL"
===DONE===
