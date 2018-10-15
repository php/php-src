--TEST--
Bug #71475: openssl_seal() uninitialized memory usage
--SKIPIF--
<?php
if (!extension_loaded("openssl")) die("skip openssl not loaded");
?>
--FILE--
<?php
$_ = str_repeat("A", 512);
openssl_seal($_, $_, $_, array_fill(0,64,0));
?>
DONE
--EXPECTF--
Warning: openssl_seal(): not a public key (1th member of pubkeys) in %s%ebug71475.php on line %d
DONE
