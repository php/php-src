--TEST--
openssl_encrypt() and openssl_decrypt() tests
--SKIPIF--
<?php if (!extension_loaded("openssl")) print "skip"; ?>
--FILE--
<?php
$data = "openssl_encrypt() and openssl_decrypt() tests";
$method = "AES-128-CBC";
$password = "openssl";

$ivlen = openssl_cipher_iv_length($method);
$iv    = '';
srand(time() + ((microtime(true) * 1000000) % 1000000));
while(strlen($iv) < $ivlen) $iv .= chr(rand(0,255));

$encrypted = openssl_encrypt($data, $method, $password, false, $iv);
$output = openssl_decrypt($encrypted, $method, $password, false, $iv);
var_dump($output);
$encrypted = openssl_encrypt($data, $method, $password, true, $iv);
$output = openssl_decrypt($encrypted, $method, $password, true, $iv);
var_dump($output);
?>
--EXPECT--
string(45) "openssl_encrypt() and openssl_decrypt() tests"
string(45) "openssl_encrypt() and openssl_decrypt() tests"

