--TEST--
openssl_private_decrypt() tests
--SKIPIF--
<?php if (!extension_loaded("openssl")) print "skip"; ?>
--FILE--
<?php
$data = b"Testing openssl_public_decrypt()";
$privkey = "file://" . dirname(__FILE__) . "/private.key";
$pubkey = "file://" . dirname(__FILE__) . "/public.key";
$wrong = "wrong";
$invalid_ascii_string = "non-ascii-unicode\u0500";

openssl_public_encrypt($data, $encrypted, $pubkey);
var_dump(openssl_private_decrypt($encrypted, $output, $privkey));
var_dump($output);
var_dump(openssl_private_decrypt($encrypted, $output2, $wrong));
var_dump($output2);
var_dump(openssl_private_decrypt($wrong, $output3, $privkey));
var_dump($output3);
var_dump(openssl_private_decrypt((binary)$wrong, $output4, $privkey));
var_dump($output4);
var_dump(openssl_private_decrypt($data, $output5, $invalid_ascii_string));
var_dump($output5);
?>
--EXPECTF--
bool(true)
string(32) "Testing openssl_public_decrypt()"

Warning: openssl_private_decrypt(): Binary string expected, Unicode string received in %s on line %d

Warning: openssl_private_decrypt(): key parameter is not a valid private key in %s on line %d
bool(false)
NULL

Warning: openssl_private_decrypt() expects parameter 1 to be strictly a binary string, Unicode string given in %s on line %d
bool(false)
NULL
bool(false)
NULL

Warning: openssl_private_decrypt(): Binary string expected, Unicode string received in %s on line %d

Warning: openssl_private_decrypt(): key parameter is not a valid private key in %s on line %d
bool(false)
NULL
