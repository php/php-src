--TEST--
openssl_encrypt() CBC and its alias
--EXTENSIONS--
openssl
--FILE--
<?php
var_dump(bin2hex(openssl_encrypt('data', 'AES-128-CBC', 'foo', 0, '0123456789abcdef')));
var_dump(bin2hex(openssl_encrypt('data', 'aes128', 'foo', 0, '0123456789abcdef')));
?>
--EXPECTF--
string(48) "7a654459353452676f6c6b6a446b75455a6c4c6b4f513d3d"
string(48) "7a654459353452676f6c6b6a446b75455a6c4c6b4f513d3d"
