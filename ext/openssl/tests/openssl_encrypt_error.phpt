--TEST--
openssl_encrypt() error tests
--SKIPIF--
<?php if (!extension_loaded("openssl")) print "skip"; ?>
--FILE--
<?php
$data = b"openssl_encrypt() tests";
$method = "AES-128-CBC";
$password = "openssl";
$wrong = "wrong";
$invalid_ascii_string = "non-ascii-unicode\u0500";
$object = new stdclass;
$arr = array(1);

var_dump(openssl_encrypt($data, $wrong, $password));
var_dump(openssl_encrypt($data, $invalid_ascii_string, $password));
var_dump(openssl_encrypt($data, $method, $invalid_ascii_string));
var_dump(openssl_encrypt($object, $method, $password));
var_dump(openssl_encrypt($data, $object, $password));
var_dump(openssl_encrypt($data, $method, $object));
var_dump(openssl_encrypt($arr, $method, $object));
var_dump(openssl_encrypt($data, $arr, $object));
var_dump(openssl_encrypt($data, $method, $arr));
?>
--EXPECTF--
Warning: openssl_encrypt(): Unknown cipher algorithm in %s on line %d
bool(false)

Warning: Could not convert Unicode string to binary string (converter US-ASCII failed on character {U+0500} at offset %d) in %s on line %d
NULL

Warning: Could not convert Unicode string to binary string (converter US-ASCII failed on character {U+0500} at offset %d) in %s on line %d
NULL

Warning: openssl_encrypt() expects parameter 1 to be binary string, object given in %s on line %d
NULL

Warning: openssl_encrypt() expects parameter 2 to be binary string, object given in %s on line %d
NULL

Warning: openssl_encrypt() expects parameter 3 to be binary string, object given in %s on line %d
NULL

Warning: openssl_encrypt() expects parameter 1 to be binary string, array given in %s on line %d
NULL

Warning: openssl_encrypt() expects parameter 2 to be binary string, array given in %s on line %d
NULL

Warning: openssl_encrypt() expects parameter 3 to be binary string, array given in %s on line %d
NULL

