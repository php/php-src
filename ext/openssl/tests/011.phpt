--TEST--
openssl_encrypt() and openssl_decrypt() tests
--SKIPIF--
<?php if (!extension_loaded("openssl")) print "skip"; ?>
--FILE--
<?php
$data = b"openssl_encrypt() and openssl_decrypt() tests";
$method = "AES-128-CBC";
$password = "openssl";

$encrypted = openssl_encrypt($data, $method, $password);
$output = openssl_decrypt($encrypted, $method, $password);
var_dump($output);
$encrypted = openssl_encrypt($data, $method, $password, true);
$output = openssl_decrypt($encrypted, $method, $password, true);
var_dump($output);
?>
--EXPECT--
string(45) "openssl_encrypt() and openssl_decrypt() tests"
string(45) "openssl_encrypt() and openssl_decrypt() tests"

