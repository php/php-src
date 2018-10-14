--TEST--
openssl_sign() tests
--SKIPIF--
<?php if (!extension_loaded("openssl")) print "skip"; ?>
--FILE--
<?php
$data = "Testing openssl_sign()";
$privkey = "file://" . dirname(__FILE__) . "/private_rsa_1024.key";
$wrong = "wrong";

var_dump(openssl_sign($data, $sign, $privkey));                 // no output
var_dump(openssl_sign($data, $sign, $wrong));
var_dump(openssl_sign(array(), $sign, $privkey));
?>
--EXPECTF--
bool(true)

Warning: openssl_sign(): supplied key param cannot be coerced into a private key in %s on line %d
bool(false)

Warning: openssl_sign() expects parameter 1 to be string, array given in %s on line %d
NULL
