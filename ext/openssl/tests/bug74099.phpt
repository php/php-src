--TEST--
Bug #74099 Memory leak with openssl_encrypt()
--EXTENSIONS--
openssl
--FILE--
<?php
$aad = random_bytes(32);
$iv = random_bytes(16);
$key = random_bytes(32);

$plaintext = '';
$tag = null;

$ciphertext = openssl_encrypt($plaintext, 'aes-256-gcm', $key, \OPENSSL_RAW_DATA, $iv, $tag, $aad);
var_dump($ciphertext);
?>
--EXPECT--
string(0) ""
