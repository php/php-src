--TEST--
openssl_private_encrypt() tests
--SKIPIF--
<?php if (!extension_loaded("openssl")) print "skip"; ?>
--FILE--
<?php
$data = "Testing openssl_private_encrypt()";
$privkey = "file://" . dirname(__FILE__) . "/private_rsa_1024.key";
$pubkey = "file://" . dirname(__FILE__) . "/public.key";
$wrong = "wrong";

class test {
	function __toString() {
		return "test";
	}
}
$obj = new test;

var_dump(openssl_private_encrypt($data, $encrypted, $privkey));
var_dump(openssl_private_encrypt($data, $encrypted, $pubkey));
var_dump(openssl_private_encrypt($data, $encrypted, $wrong));
var_dump(openssl_private_encrypt($data, $encrypted, $obj));
var_dump(openssl_private_encrypt($obj, $encrypted, $privkey));
openssl_public_decrypt($encrypted, $output, $pubkey);
var_dump($output);
?>
--EXPECTF--
bool(true)

Warning: openssl_private_encrypt(): key param is not a valid private key in %s on line %d
bool(false)

Warning: openssl_private_encrypt(): key param is not a valid private key in %s on line %d
bool(false)

Warning: openssl_private_encrypt(): key param is not a valid private key in %s on line %d
bool(false)
bool(true)
string(4) "test"
