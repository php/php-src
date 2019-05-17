--TEST--
openssl: get public key from generated private key
--SKIPIF--
<?php
if (!extension_loaded("openssl")) die("skip");
if (!@openssl_pkey_new()) die("skip cannot create private key");
?>
--FILE--
<?php
$priv = openssl_pkey_new();
$pub = openssl_pkey_get_public($priv);
?>
--EXPECTF--
Warning: openssl_pkey_get_public(): Don't know how to get public key from this private key %s
