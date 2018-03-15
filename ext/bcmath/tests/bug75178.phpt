--TEST--
Bug #75178 (bcpowmod() misbehaves for non-integer base or modulus)
--SKIPIF--
<?php
if (!extension_loaded('bcmath')) die('skip bcmath extension is not available');
?>
--FILE--
<?php
var_dump(bcpowmod('4.1', '4', '3', 3));
var_dump(bcpowmod('4', '4', '3.1', 3));
?>
===DONE===
--EXPECTF--
Warning: bcpowmod(): non-zero scale in base in %s on line %d
string(5) "1.000"

Warning: bcpowmod(): non-zero scale in modulus in %s on line %d
string(5) "1.000"
===DONE===
