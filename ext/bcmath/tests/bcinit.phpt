--TEST--
bcinit() – basic functionality
--SKIPIF--
<?php
if (!extension_loaded('bcmath')) die('skip bcmath extension is not available');
?>
--INI--
bcmath.scale = 6
--FILE--
<?php
var_dump(bcinit(1.234));
var_dump(bcinit(0.00005));
var_dump(bcinit(42));
var_dump(bcinit('1.234'));
var_dump(bcinit(1.234, 5));
var_dump(bcinit(0.00005, 5));
var_dump(bcinit(42, 5));
var_dump(bcinit('1.234', 5));
?>
===DONE===
--EXPECT--
string(8) "1.234000"
string(8) "0.000050"
string(2) "42"
string(5) "1.234"
string(7) "1.23400"
string(7) "0.00005"
string(2) "42"
string(5) "1.234"
===DONE===
