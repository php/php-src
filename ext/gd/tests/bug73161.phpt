--TEST--
Bug #73161 (imagecreatefromgd2() may leak memory)
--DESCRIPTION--
We're testing for a memory leak that might not even show up with valgrind.
--EXTENSIONS--
gd
--FILE--
<?php
$im = imagecreatefromgd2(__DIR__ . DIRECTORY_SEPARATOR . 'bug73161.gd2');
var_dump($im);
?>
--EXPECTF--
Warning: imagecreatefromgd2(): "%s" is not a valid GD2 file in %s on line %d
bool(false)
