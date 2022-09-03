--TEST--
openssl_cipher_iv_length() basic test
--EXTENSIONS--
openssl
--FILE--
<?php

var_dump(openssl_cipher_iv_length('AES-128-CBC'));

?>
--EXPECT--
int(16)
