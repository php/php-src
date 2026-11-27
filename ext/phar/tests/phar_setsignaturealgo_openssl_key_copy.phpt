--TEST--
Phar::setSignatureAlgorithm() with Phar::OPENSSL keeps its own copy of the private key
--EXTENSIONS--
phar
openssl
--SKIPIF--
<?php
if (!in_array("OpenSSL", Phar::getSupportedSignatures())) die("skip openssl support required");
?>
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php
$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar';
$keyfile = __DIR__ . '/files/private.pem';

$p = new Phar($fname);
$p['file1.txt'] = 'hi';
// The key string is freed when the call returns
$p->setSignatureAlgorithm(Phar::OPENSSL, file_get_contents($keyfile));
// Reuse the freed memory
$other = str_repeat('x', filesize($keyfile));
// Signs the phar again
$p['file2.txt'] = 'hello';
var_dump($p->getSignature()['hash_type']);
?>
--CLEAN--
<?php
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar');
?>
--EXPECT--
string(7) "OpenSSL"
