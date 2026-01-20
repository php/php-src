--TEST--
GH-9422: check for SM2 public key
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (OPENSSL_VERSION_NUMBER < 0x10101000) die("skip OpenSSL v1.1.1 required");
if (!defined("OPENSSL_KEYTYPE_EC")) die("skip EC disabled");
?>
--FILE--
<?php
$pemPubKey = file_get_contents(__DIR__ . '/sm2pubkey.pem');
$pubKey = openssl_pkey_get_public($pemPubKey);
$pubKeyDetails = openssl_pkey_get_details($pubKey);
$expectedPubKeyType = (OPENSSL_VERSION_NUMBER < 0x30000000) ? 3 : -1;
var_dump($pubKeyDetails['type'] === $expectedPubKeyType);
?>
--EXPECT--
bool(true)
