--TEST--
Bug #68912 (Segmentation fault at openssl_spki_new)
--SKIPIF--
<?php
if (!extension_loaded("openssl")) die("skip");
if (!extension_loaded("xml")) die("skip xml extension not loaded");
if (!@openssl_pkey_new()) die("skip cannot create private key");
?>
--FILE--
<?php

$var1=xml_parser_create_ns();
$var2=2;
$var3=3;

openssl_spki_new($var1, $var2, $var3);
?>
--EXPECTF--
Warning: openssl_spki_new(): supplied resource is not a valid OpenSSL X.509/key resource in %sbug68912.php on line %d

Warning: openssl_spki_new(): Unable to use supplied private key in %sbug68912.php on line %d
