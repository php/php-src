--TEST--
Bug #54598 (bcpowmod() may return 1 if modulus is 1)
--SKIPIF--
<?php
if (!extension_loaded('bcmath')) die('skip bcmath extension is not available');
?>
--FILE--
<?php
var_dump(bcpowmod(5, 0, 1));
var_dump(bcpowmod(5, 0, 1, 3));
?>
===DONE===
--EXPECT--
string(1) "0"
string(5) "0.000"
===DONE===
