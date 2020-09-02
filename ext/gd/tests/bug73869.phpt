--TEST--
Bug #73869 (Signed Integer Overflow gd_io.c)
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip gd extension not available');
?>
--FILE--
<?php
var_dump(imagecreatefromgd2(__DIR__ . DIRECTORY_SEPARATOR . 'bug73869a.gd2'));
var_dump(imagecreatefromgd2(__DIR__ . DIRECTORY_SEPARATOR . 'bug73869b.gd2'));
?>
--EXPECTF--
Warning: imagecreatefromgd2(): "%s" is not a valid GD2 file in %s on line %d
bool(false)

Warning: imagecreatefromgd2(): "%s" is not a valid GD2 file in %s on line %d
bool(false)
