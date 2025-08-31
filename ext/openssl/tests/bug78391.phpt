--TEST--
Bug #78391: Assertion failure in openssl_random_pseudo_bytes
--EXTENSIONS--
openssl
--FILE--
<?php

$isStrongCrypto = false;
var_dump(strlen(openssl_random_pseudo_bytes(16, $isStrongCrypto)));
var_dump($isStrongCrypto);

?>
--EXPECT--
int(16)
bool(true)
