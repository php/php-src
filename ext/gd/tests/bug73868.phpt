--TEST--
Bug 73868 (DOS vulnerability in gdImageCreateFromGd2Ctx())
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip gd extension not available');
?>
--FILE--
<?php
var_dump(imagecreatefromgd2(__DIR__ . DIRECTORY_SEPARATOR . 'bug73868.gd2'));
?>
===DONE===
--EXPECTF--
Warning: imagecreatefromgd2(): gd2: EOF while reading
 in %s on line %d

Warning: imagecreatefromgd2(): '%s' is not a valid GD2 file in %s on line %d
bool(false)
===DONE===
