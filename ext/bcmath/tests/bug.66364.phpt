--TEST--
Bug #66364 (BCMath bcmul ignores scale parameter)
--SKIPIF--
<?php
if (!extension_loaded('bcmath')) die('skip bcmath extension not available');
?>
--FILE--
<?php
var_dump(bcmul('0.3', '0.2', 4));
?>
===DONE===
--EXPECT--
string(6) "0.0600"
===DONE===
