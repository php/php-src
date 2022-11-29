--TEST--
openssl_cipher_key_length() basic test
--EXTENSIONS--
openssl
--FILE--
<?php

var_dump(openssl_cipher_key_length('AES-128-CBC'));
var_dump(openssl_cipher_key_length('AES-256-CBC'));

?>
--EXPECT--
int(16)
int(32)
