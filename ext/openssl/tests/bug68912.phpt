--TEST--
Bug #68912 (Segmentation fault at openssl_spki_new)
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!@openssl_pkey_new()) die("skip cannot create private key");
?>
--FILE--
<?php

$var1=fopen(__FILE__, 'r');
$var2=2;
$var3=3;

try {
    openssl_spki_new($var1, $var2, $var3);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
openssl_spki_new(): Argument #1 ($private_key) must be of type OpenSSLAsymmetricKey, resource given
