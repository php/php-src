--TEST--
openssl_decrypt() error tests
--SKIPIF--
<?php if (!extension_loaded("openssl")) print "skip"; ?>
--FILE--
<?php
$data = b"openssl_decrypt() tests";
$method = "AES-128-CBC";
$password = "openssl";
$wrong = "wrong";
$invalid_ascii_string = "non-ascii-unicode\u0500";

$encrypted = openssl_encrypt($data, $method, $password);
var_dump(openssl_decrypt($encrypted, $method, $wrong));
var_dump(openssl_decrypt($encrypted, $wrong, $password));
var_dump(openssl_decrypt($wrong, $method, $password));
var_dump(openssl_decrypt($wrong, $wrong, $password));
var_dump(openssl_decrypt($wrong, $method, $wrong));
var_dump(openssl_decrypt($encrypted, $wrong, $wrong));
var_dump(openssl_decrypt($wrong, $wrong, $wrong));
echo "---\n";
var_dump(openssl_decrypt($encrypted, $invalid_ascii_string, $password));
var_dump(openssl_decrypt($encrypted, $method, $invalid_ascii_string));
var_dump(openssl_decrypt($invalid_ascii_string, $method, $password));
var_dump(openssl_decrypt($encrypted, $invalid_ascii_string, $invalid_ascii_string));
?>
--EXPECTF--
bool(false)

Warning: openssl_decrypt(): Unknown cipher algorithm in %s on line %d
bool(false)

Warning: openssl_decrypt() expects parameter 1 to be strictly a binary string, Unicode string given in %s on line %d
NULL

Warning: openssl_decrypt() expects parameter 1 to be strictly a binary string, Unicode string given in %s on line %d
NULL

Warning: openssl_decrypt() expects parameter 1 to be strictly a binary string, Unicode string given in %s on line %d
NULL

Warning: openssl_decrypt(): Unknown cipher algorithm in %s on line %d
bool(false)

Warning: openssl_decrypt() expects parameter 1 to be strictly a binary string, Unicode string given in %s on line %d
NULL
---

Warning: Could not convert Unicode string to binary string (converter US-ASCII failed on character {U+0500} at offset %d) in %s on line %d
NULL

Warning: Could not convert Unicode string to binary string (converter US-ASCII failed on character {U+0500} at offset %d) in %s on line %d
NULL

Warning: openssl_decrypt() expects parameter 1 to be strictly a binary string, Unicode string given in %s on line %d
NULL

Warning: Could not convert Unicode string to binary string (converter US-ASCII failed on character {U+0500} at offset %d) in %s on line %d
NULL

