--TEST--
openssl_verify() tests
--SKIPIF--
<?php if (!extension_loaded("openssl")) print "skip"; ?>
--FILE--
<?php
$data = "Testing openssl_verify()";
$privkey = "file://" . dirname(__FILE__) . "/private_rsa_1024.key";
$pubkey = "file://" . dirname(__FILE__) . "/public.key";
$wrong = "wrong";

openssl_sign($data, $sign, $privkey);
var_dump(openssl_verify($data, $sign, $pubkey));
var_dump(openssl_verify($data, $sign, $privkey));
var_dump(openssl_verify($data, $sign, $wrong));
var_dump(openssl_verify($data, $wrong, $pubkey));
var_dump(openssl_verify($wrong, $sign, $pubkey));
?>
--EXPECTF--
int(1)

Warning: openssl_verify(): supplied key param cannot be coerced into a public key in %s on line %d
bool(false)

Warning: openssl_verify(): supplied key param cannot be coerced into a public key in %s on line %d
bool(false)
int(0)
int(0)
