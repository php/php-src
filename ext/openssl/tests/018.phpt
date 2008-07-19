--TEST--
openssl_sign() tests
--SKIPIF--
<?php if (!extension_loaded("openssl")) print "skip"; ?>
--FILE--
<?php
$data = b"Testing openssl_sign()";
$privkey = "file://" . dirname(__FILE__) . "/private.key";
$wrong = "wrong";
$invalid_ascii_string = "non-ascii-unicode\u0500";

var_dump(openssl_sign($data, $sign, $privkey));                 // no output
var_dump(openssl_sign($data, $sign, $wrong));
var_dump(openssl_sign($data, $sign, $invalid_ascii_string));
var_dump(openssl_sign($invalid_ascii_string, $sign, $privkey));
var_dump(openssl_sign($data, $sign, (binary)$privkey));
var_dump(openssl_sign((unicode)$data, $sign, $privkey));
?>
--EXPECTF--	
bool(true)

Warning: openssl_sign(): Binary string expected, Unicode string received in %s on line %d

Warning: openssl_sign(): supplied key param cannot be coerced into a private key in %s on line %d
bool(false)

Warning: openssl_sign(): Binary string expected, Unicode string received in %s on line %d

Warning: openssl_sign(): supplied key param cannot be coerced into a private key in %s on line %d
bool(false)

Warning: openssl_sign() expects parameter 1 to be strictly a binary string, Unicode string given in %s on line %d
bool(false)
bool(true)

Warning: openssl_sign() expects parameter 1 to be strictly a binary string, Unicode string given in %s on line %d
bool(false)
