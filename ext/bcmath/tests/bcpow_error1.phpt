--TEST--
bcpow() does not support non-integral exponents
--SKIPIF--
<?php
if (!extension_loaded('bcmath')) die('skip bcmath extension is not available');
?>
--FILE--
<?php
var_dump(bcpow('1', '1.1', 2));
?>
===DONE===
--EXPECTF--
Warning: bcpow(): non-zero scale in exponent in %s on line %d
string(4) "1.00"
===DONE===
