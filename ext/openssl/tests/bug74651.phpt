--TEST--
Bug #74651: negative-size-param (-1) in memcpy in zif_openssl_seal()
--SKIPIF--
<?php
if (!extension_loaded("openssl")) die("skip openssl not loaded");
?>
--FILE--
<?php

$inputstr = file_get_contents(__DIR__ . "/74651.pem");
$pub_key_id = openssl_get_publickey($inputstr);
var_dump($pub_key_id);
var_dump(openssl_seal($inputstr, $sealed, $ekeys, array($pub_key_id, $pub_key_id), 'AES-128-ECB'));
?>
--EXPECTF--
object(OpenSSLAsymmetricKey)#%d (0) {
}
bool(false)
