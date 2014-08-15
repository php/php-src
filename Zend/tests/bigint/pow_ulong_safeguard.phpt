--TEST--
Bigint ** unsigned long exponent safeguard
--DESCRIPTION--
This makes sure that the pow operator is properly checking if a bigint power is within the range of an unsigned long
(The reason it needs to check that specifically is due to the zend_bigint API not accepting larger powers)
--FILE--
<?php
/* This value should exceed the size of an unsigned C long by one */
$exp = 2 ** (PHP_INT_SIZE * 8);
var_dump(2 ** $exp);
--EXPECTF--
Warning: Exponent too large in %s on line %d
bool(false)