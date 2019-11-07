--TEST--
Bug #72165 Null pointer dereference - openssl_csr_new
--SKIPIF--
<?php
if (!extension_loaded("openssl")) die("skip");
?>
--FILE--
<?php
$var0 = array(0 => "hello", 1 => "world");
$var2 = openssl_csr_new(array(0),$var0,null,array(0));
?>
--EXPECTF--
Warning: openssl_csr_new(): dn: numeric fild names are not supported in %sbug72165.php on line %d

Warning: openssl_csr_new(): add1_attr_by_txt challengePassword_min -> 4 (failed; check error queue and value of string_mask OpenSSL option if illegal characters are reported) in %sbug72165.php on line %d
