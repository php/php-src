--TEST--
Bug #75178 (bcpowmod() misbehaves for non-integer base or modulus)
--SKIPIF--
<?php
if (!extension_loaded('bcmath')) die('skip bcmath extension is not available');
if (substr(PHP_OS, 0, 3) != 'WIN') {
    die('skip valid only for windows');
}
?>
--FILE--
<?php
var_dump(bcpowmod('4.1', '4', '3', 3));
var_dump(bcpowmod('4', '4', '3.1', 3));
?>
===DONE===
--EXPECT--
string(5) "1.000"
string(5) "1.000"
===DONE===
bc math warning: non-zero scale in base
bc math warning: non-zero scale in modulus
