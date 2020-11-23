--TEST--
openssl_sign() tests
--SKIPIF--
<?php if (!extension_loaded("openssl")) print "skip"; ?>
--FILE--
<?php
$data = "Testing openssl_sign()";
$privkey = "file://" . __DIR__ . "/private_rsa_1024.key";
$wrong = "wrong";

var_dump(openssl_sign($data, $sign, $privkey));                 // no output
var_dump(openssl_sign($data, $sign, $wrong));
?>
--EXPECTF--
bool(true)

Warning: openssl_sign(): Supplied key param cannot be coerced into a private key in %s on line %d
bool(false)
