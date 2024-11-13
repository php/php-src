--TEST--
Bug #72165 Null pointer dereference - openssl_csr_new
--EXTENSIONS--
openssl
--FILE--
<?php
$var0 = [0 => "hello", 1 => "world"];
$options = ['config' => __DIR__ . DIRECTORY_SEPARATOR . 'openssl.cnf'];
$var2 = openssl_csr_new([0], $var0, $options, [0]);
?>
--EXPECTF--
Warning: openssl_csr_new(): attributes: numeric fild names are not supported in %sbug72165.php on line %d

Warning: openssl_csr_new(): add1_attr_by_txt challengePassword_min -> 4 (failed; check error queue and value of string_mask OpenSSL option if illegal characters are reported) in %sbug72165.php on line %d
