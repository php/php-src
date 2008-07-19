--TEST--
openssl_verify() tests
--SKIPIF--
<?php if (!extension_loaded("openssl")) print "skip"; ?>
--FILE--
<?php
$data = b"Testing openssl_verify()";
$privkey = "file://" . dirname(__FILE__) . "/private.key";
$pubkey = "file://" . dirname(__FILE__) . "/public.key";
$wrong = "wrong";
$wrong2 = b"wrong";
$invalid_ascii_string = "non-ascii-unicode\u0500";

openssl_sign($data, $sign, $privkey);
var_dump(openssl_verify($data, $sign, $pubkey));
var_dump(openssl_verify($data, $sign, $privkey));
var_dump(openssl_verify($data, $sign, $wrong));
var_dump(openssl_verify($data, $wrong, $pubkey));
var_dump(openssl_verify($wrong, $sign, $pubkey));
var_dump(openssl_verify($wrong2, $sign, $pubkey));
var_dump(openssl_verify($invalid_ascii_string, $sign, $pubkey));
var_dump(openssl_verify($data, $sign, $invalid_ascii_string));
var_dump(openssl_verify($data, (unicode)$sign, $pubkey));
?>
--EXPECTF--
int(1)

Warning: openssl_verify(): supplied key param cannot be coerced into a public key in %s on line %d
bool(false)

Warning: openssl_verify(): Binary string expected, Unicode string received in %s on line %d

Warning: openssl_verify(): supplied key param cannot be coerced into a public key in %s on line %d
bool(false)

Warning: openssl_verify() expects parameter 2 to be strictly a binary string, Unicode string given in %s on line %d
bool(false)

Warning: openssl_verify() expects parameter 1 to be strictly a binary string, Unicode string given in %s on line %d
bool(false)
int(0)

Warning: openssl_verify() expects parameter 1 to be strictly a binary string, Unicode string given in %s on line %d
bool(false)

Warning: openssl_verify(): Binary string expected, Unicode string received in %s on line %d

Warning: openssl_verify(): supplied key param cannot be coerced into a public key in %s on line %d
bool(false)

Warning: openssl_verify() expects parameter 2 to be strictly a binary string, Unicode string given in %s on line %d
bool(false)

