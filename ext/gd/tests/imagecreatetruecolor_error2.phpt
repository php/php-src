--TEST--
Testing imagecreatetruecolor(): error on out of bound parameters
--CREDITS--
Rafael Dohms <rdohms [at] gmail [dot] com>
--SKIPIF--
<?php
	if (!extension_loaded("gd")) die("skip GD not present");
	if (!function_exists("imagecreatetruecolor")) die("skip GD Version not compatible");
?>
--FILE--
<?php
$image = imagecreatetruecolor(-1, 30);
$image = imagecreatetruecolor(30, -1);
$image = imagecreatetruecolor(999999999999999999999999999, 30);
$image = imagecreatetruecolor(30, 999999999999999999999999999);
?>
--EXPECTF--
Warning: imagecreatetruecolor(): Invalid image dimensions in %s on line %d

Warning: imagecreatetruecolor(): Invalid image dimensions in %s on line %d

Warning: imagecreatetruecolor() expects parameter 1 to be int, float given in %s on line %d

Warning: imagecreatetruecolor() expects parameter 2 to be int, float given in %s on line %d
