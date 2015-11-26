--TEST--
Bug #61124: Segmentation fault with openssl_decrypt
--SKIPIF--
<?php 
if (!extension_loaded("openssl")) die("skip");
--FILE--
<?php
var_dump(openssl_decrypt('kzo w2RMExUTYQXW2Xzxmg==', 'aes-128-cbc', 'pass', false, 'pass'));

--EXPECTF--
Warning: openssl_decrypt(): Failed to base64 decode the input in %s on line %s
bool(false)