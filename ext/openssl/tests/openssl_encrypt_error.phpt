--TEST--
openssl_encrypt() error tests
--SKIPIF--
<?php if (!extension_loaded("openssl")) print "skip"; ?>
--FILE--
<?php
$data = "openssl_encrypt() tests";
$method = "AES-128-CBC";
$password = "openssl";
$iv = str_repeat("\0", openssl_cipher_iv_length($method));
$wrong = "wrong";
$object = new stdclass;
$arr = array(1);

// wrong parameters tests
var_dump(openssl_encrypt($data, $wrong, $password));
var_dump(openssl_encrypt($object, $method, $password));
var_dump(openssl_encrypt($data, $object, $password));
var_dump(openssl_encrypt($data, $method, $object));
var_dump(openssl_encrypt($arr, $method, $object));
var_dump(openssl_encrypt($data, $arr, $object));
var_dump(openssl_encrypt($data, $method, $arr));

// padding of the key is disabled
var_dump(openssl_encrypt($data, $method, $password, OPENSSL_DONT_ZERO_PAD_KEY, $iv));
?>
--EXPECTF--
Warning: openssl_encrypt(): Unknown cipher algorithm in %s on line %d
bool(false)

Warning: openssl_encrypt() expects parameter 1 to be string, object given in %s on line %d
NULL

Warning: openssl_encrypt() expects parameter 2 to be string, object given in %s on line %d
NULL

Warning: openssl_encrypt() expects parameter 3 to be string, object given in %s on line %d
NULL

Warning: openssl_encrypt() expects parameter 1 to be string, array given in %s on line %d
NULL

Warning: openssl_encrypt() expects parameter 2 to be string, array given in %s on line %d
NULL

Warning: openssl_encrypt() expects parameter 3 to be string, array given in %s on line %d
NULL

Warning: openssl_encrypt(): Key length cannot be set for the cipher method in %s on line %d
bool(false)
