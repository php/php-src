--TEST--
openssl_public_decrypt() tests
--SKIPIF--
<?php if (!extension_loaded("openssl")) print "skip"; ?>
--FILE--
<?php
$data = b"Testing openssl_public_decrypt()";
$privkey = "file://" . dirname(__FILE__) . "/private.key";
$pubkey = "file://" . dirname(__FILE__) . "/public.key";
$wrong = "wrong";
$invalid_ascii_string = "non-ascii-unicode\u0500";

openssl_private_encrypt($data, $encrypted, $privkey);
var_dump(openssl_public_decrypt($encrypted, $output, $pubkey));
var_dump($output);
var_dump(openssl_public_decrypt($encrypted, $output2, $wrong));
var_dump($output2);
var_dump(openssl_public_decrypt($wrong, $output3, $pubkey));
var_dump($output3);
var_dump(openssl_public_decrypt((binary)$wrong, $output4, $pubkey));
var_dump($output4);
var_dump(openssl_public_decrypt($data, $output5, $invalid_ascii_string));
var_dump($output5);
?>
--EXPECTF--
bool(true)
string(32) "Testing openssl_public_decrypt()"

Warning: openssl_public_decrypt(): Binary string expected, Unicode string received in %s on line %d

Warning: openssl_public_decrypt(): key parameter is not a valid public key in %s on line %d
bool(false)
NULL

Warning: openssl_public_decrypt() expects parameter 1 to be strictly a binary string, Unicode string given in %s on line %d
bool(false)
NULL
bool(false)
NULL

Warning: openssl_public_decrypt(): Binary string expected, Unicode string received in %s on line %d

Warning: openssl_public_decrypt(): key parameter is not a valid public key in %s on line %d
bool(false)
NULL

