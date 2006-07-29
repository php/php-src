--TEST--
openssl key from zval leaks 
--SKIPIF--
<?php 
if (!extension_loaded("openssl")) die("skip"); 
?>
--FILE--
<?php
$pub_key_id = false; 
$signature = '';
$ok = openssl_verify("foo", $signature, $pub_key_id, OPENSSL_ALGO_MD5);
?>
--EXPECTF--
Warning: openssl_verify(): supplied key param cannot be coerced into a public key in %s/bug38255.php on line %d 
