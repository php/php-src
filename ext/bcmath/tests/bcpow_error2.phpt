--TEST--
bcpow() does not support exponents >= 2**63
--SKIPIF--
<?php
if (!extension_loaded('bcmath')) die('skip bcmath extension is not available');
?>
--FILE--
<?php
var_dump(bcpow('0', '9223372036854775808', 2));
?>
===DONE===
--EXPECTF--
Warning: bcpow(): exponent too large in %s on line %d
string(4) "1.00"
===DONE===
