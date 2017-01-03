--TEST--
Bug #73161 (imagecreatefromgd2() may leak memory)
--DESCRIPTION--
We're testing for a memory leak that might not even show up with valgrind.
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip gd extension not available');
?>
--FILE--
<?php
$im = imagecreatefromgd2(__DIR__ . DIRECTORY_SEPARATOR . 'bug73161.gd2');
var_dump($im);
?>
===DONE===
--EXPECTF--
Warning: imagecreatefromgd2(): '%s' is not a valid GD2 file in %s on line %d
bool(false)
===DONE===
