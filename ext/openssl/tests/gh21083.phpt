--TEST--
GH-21083: openssl_pkey_new() should not require private_key_bits for EC keys
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!defined("OPENSSL_KEYTYPE_EC")) die("skip EC disabled");
?>
--FILE--
<?php
$config = __DIR__ . DIRECTORY_SEPARATOR . 'openssl.cnf';

$key = openssl_pkey_new([
    'curve_name' => 'prime256v1',
    'private_key_type' => OPENSSL_KEYTYPE_EC,
    'config' => $config,
]);
var_dump($key instanceof OpenSSLAsymmetricKey);

$details = openssl_pkey_get_details($key);
var_dump($details['ec']['curve_name']);
var_dump($details['type'] === OPENSSL_KEYTYPE_EC);

$key2 = openssl_pkey_new([
    'curve_name' => 'secp384r1',
    'private_key_type' => OPENSSL_KEYTYPE_EC,
    'config' => $config,
]);
var_dump($key2 instanceof OpenSSLAsymmetricKey);

$details2 = openssl_pkey_get_details($key2);
var_dump($details2['ec']['curve_name']);
?>
--EXPECT--
bool(true)
string(10) "prime256v1"
bool(true)
bool(true)
string(9) "secp384r1"
