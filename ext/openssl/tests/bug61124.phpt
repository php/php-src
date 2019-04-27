--TEST--
Bug #61124: Segmentation fault with openssl_decrypt
--SKIPIF--
<?php
if (!extension_loaded("openssl")) die("skip");
--FILE--
<?php
var_dump(openssl_decrypt('kzo w2RMExUTYQXW2Xzxmg==', 'aes-128-cbc', 'pass', false, 'pass'));
--EXPECTF--
Warning: openssl_decrypt(): IV passed is only 4 bytes long, cipher expects an IV of precisely 16 bytes, padding with \0 in %s on line %d
bool(false)
