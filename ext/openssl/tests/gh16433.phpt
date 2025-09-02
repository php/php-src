--TEST--
GH-16433 (Large values for openssl_csr_sign() $days overflow)
--EXTENSIONS--
openssl
--FILE--
<?php
$privkey = openssl_pkey_new();
$csr = openssl_csr_new([], $privkey);
var_dump(openssl_csr_sign($csr, null, $privkey, PHP_INT_MAX));
var_dump(openssl_csr_sign($csr, null, $privkey, -1));
?>
--EXPECTF--
Warning: openssl_csr_sign(): Days must be between 0 and %d in %s on line %d
bool(false)

Warning: openssl_csr_sign(): Days must be between 0 and %d in %s on line %d
bool(false)
