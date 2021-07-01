--TEST--
openssl_public_encrypt() tests
--EXTENSIONS--
openssl
--FILE--
<?php
$data = "Testing openssl_public_encrypt()";
$privkey = "file://" . __DIR__ . "/private_rsa_1024.key";
$pubkey = "file://" . __DIR__ . "/public.key";
$wrong = "wrong";

class test {
    function __toString() {
        return "test";
    }
}
$obj = new test;

var_dump(openssl_public_encrypt($data, $encrypted, $pubkey));
var_dump(openssl_public_encrypt($data, $encrypted, $privkey));
var_dump(openssl_public_encrypt($data, $encrypted, $wrong));
var_dump(openssl_public_encrypt($data, $encrypted, $obj));
var_dump(openssl_public_encrypt($obj, $encrypted, $pubkey));
openssl_private_decrypt($encrypted, $output, $privkey);
var_dump($output);
?>
--EXPECTF--
bool(true)

Warning: openssl_public_encrypt(): key parameter is not a valid public key in %s on line %d
bool(false)

Warning: openssl_public_encrypt(): key parameter is not a valid public key in %s on line %d
bool(false)

Warning: openssl_public_encrypt(): key parameter is not a valid public key in %s on line %d
bool(false)
bool(true)
string(4) "test"
