--TEST--
Phar: verify signature parsing works
--SKIPIF--
<?php if (!extension_loaded("phar")) print "skip"; ?>
<?php if ( extension_loaded("hash")) print "skip extension hash conflicts"; ?>
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php
$a = new Phar('sha1.phar');
$r = $a->getSignature();
var_dump($r['hash_type']);
$a = new Phar('sha512.phar');
$r = $a->getSignature();
var_dump($r['hash_type']);
$a = new Phar('sha256.phar');
$r = $a->getSignature();
var_dump($r['hash_type']);
$a = new Phar('md5.phar');
$r = $a->getSignature();
var_dump($r['hash_type']);
?>
===DONE===
--EXPECT--
string(5) "SHA-1"
string(7) "SHA-512"
string(7) "SHA-256"
string(3) "MD5"
===DONE===
