--TEST--
Testing imagecreatetruecolor(): error on non-long parameters
--CREDITS--
Rafael Dohms <rdohms [at] gmail [dot] com>
--SKIPIF--
<?php 
	if (!extension_loaded("gd")) die("skip GD not present");
	if (!function_exists("imagecreatetruecolor")) die("skip GD Version not compatible");
?>
--FILE--
<?php
$image = imagecreatetruecolor('s', 30);
$image = imagecreatetruecolor(30, 's');
?>
--EXPECTF--
Warning: imagecreatetruecolor(): Invalid image dimensions in %s on line %d

Warning: imagecreatetruecolor(): Invalid image dimensions in %s on line %d
