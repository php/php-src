--TEST--
openssl_private_decrypt() tests
--SKIPIF--
<?php if (!extension_loaded("openssl")) print "skip"; ?>
--FILE--
<?php
$data = "Testing openssl_public_decrypt()";
$privkey = "file://" . __DIR__ . "/private_rsa_1024.key";
$pubkey = "file://" . __DIR__ . "/public.key";
$wrong = "wrong";

openssl_public_encrypt($data, $encrypted, $pubkey);
var_dump(openssl_private_decrypt($encrypted, $output, $privkey));
var_dump($output);
var_dump(openssl_private_decrypt($encrypted, $output2, $wrong));
var_dump($output2);
var_dump(openssl_private_decrypt($wrong, $output3, $privkey));
var_dump($output3);
var_dump(openssl_private_decrypt($encrypted, $output4, array($privkey)));
var_dump($output4);
var_dump(openssl_private_decrypt($encrypted, $output5, array($privkey, "")));
var_dump($output5);
?>
--EXPECTF--
bool(true)
string(32) "Testing openssl_public_decrypt()"

Warning: openssl_private_decrypt(): key parameter is not a valid private key in %s on line %d
bool(false)
NULL
bool(false)
NULL

Warning: openssl_private_decrypt(): key array must be of the form array(0 => key, 1 => phrase) in %s on line %d

Warning: openssl_private_decrypt(): key parameter is not a valid private key in %s on line %d
bool(false)
NULL
bool(true)
string(32) "Testing openssl_public_decrypt()"
