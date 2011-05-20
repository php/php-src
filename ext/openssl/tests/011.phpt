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

$encrypted = openssl_encrypt($data, $method, $password, 0, $iv);
$output = openssl_decrypt($encrypted, $method, $password, 0, $iv);
var_dump($output);
$encrypted = openssl_encrypt($data, $method, $password, OPENSSL_RAW_DATA, $iv);
$output = openssl_decrypt($encrypted, $method, $password, OPENSSL_RAW_DATA, $iv);
var_dump($output);
// if we want to manage our own padding
$padded_data = $data . str_repeat(' ', 16 - (strlen($data) % 16));
$encrypted = openssl_encrypt($padded_data, $method, $password, OPENSSL_RAW_DATA|OPENSSL_ZERO_PADDING, $iv);
$output = openssl_decrypt($encrypted, $method, $password, OPENSSL_RAW_DATA|OPENSSL_ZERO_PADDING, $iv);
var_dump(rtrim($output));
?>
--EXPECT--
string(45) "openssl_encrypt() and openssl_decrypt() tests"
string(45) "openssl_encrypt() and openssl_decrypt() tests"
string(45) "openssl_encrypt() and openssl_decrypt() tests"
