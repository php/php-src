--TEST--
bcinit() – error conditions
--SKIPIF--
<?php
if (!extension_loaded('bcmath')) die('skip bcmath extension is not available');
?>
--FILE--
<?php
var_dump(bcinit(true));
var_dump(bcinit(array()));
var_dump(bcinit(new stdClass));
?>
===DONE===
--EXPECTF--
Warning: bcinit(): Invalid type for num in %s on line %d
bool(false)

Warning: bcinit(): Invalid type for num in %s on line %d
bool(false)

Warning: bcinit(): Invalid type for num in %s on line %d
bool(false)
===DONE===
