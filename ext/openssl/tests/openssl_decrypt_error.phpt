--TEST--
openssl_decrypt() error tests
--SKIPIF--
<?php if (!extension_loaded("openssl")) print "skip"; ?>
--FILE--
<?php
$data = "openssl_decrypt() tests";
$method = "AES-128-CBC";
$password = "openssl";
$wrong = "wrong";

$encrypted = openssl_encrypt($data, $method, $password);
var_dump(openssl_decrypt($encrypted, $method, $wrong));
var_dump(openssl_decrypt($encrypted, $wrong, $password));
var_dump(openssl_decrypt($wrong, $method, $password));
var_dump(openssl_decrypt($wrong, $wrong, $password));
var_dump(openssl_decrypt($encrypted, $wrong, $wrong));
var_dump(openssl_decrypt($wrong, $wrong, $wrong));
var_dump(openssl_decrypt(array(), $method, $password));
var_dump(openssl_decrypt($encrypted, array(), $password));
var_dump(openssl_decrypt($encrypted, $method, array()));
?>
--EXPECTF--
bool(false)

Warning: openssl_decrypt(): Unknown cipher algorithm in %s on line %d
bool(false)
bool(false)

Warning: openssl_decrypt(): Unknown cipher algorithm in %s on line %d
bool(false)

Warning: openssl_decrypt(): Unknown cipher algorithm in %s on line %d
bool(false)

Warning: openssl_decrypt(): Unknown cipher algorithm in %s on line %d
bool(false)

Warning: openssl_decrypt() expects parameter 1 to be string, array given in %s on line %d
NULL

Warning: openssl_decrypt() expects parameter 2 to be string, array given in %s on line %d
NULL

Warning: openssl_decrypt() expects parameter 3 to be string, array given in %s on line %d
NULL
