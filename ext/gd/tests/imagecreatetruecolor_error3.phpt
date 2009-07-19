--TEST--
Testing imagecreatetruecolor(): error on wrong parameter count
--CREDITS--
Rafael Dohms <rdohms [at] gmail [dot] com>
--SKIPIF--
<?php 
	if (!extension_loaded("gd")) die("skip GD not present");
	if (!function_exists("imagecreatetruecolor")) die("skip GD Version not compatible");
?>
--FILE--
<?php
$image = imagecreatetruecolor();
$image = imagecreatetruecolor(30);
$image = imagecreatetruecolor(1,1,1);
?>
--EXPECTF--
Warning: Wrong parameter count for imagecreatetruecolor() in %s on line %d

Warning: Wrong parameter count for imagecreatetruecolor() in %s on line %d

Warning: Wrong parameter count for imagecreatetruecolor() in %s on line %d