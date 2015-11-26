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
Warning: imagecreatetruecolor() expects exactly 2 parameters, 0 given in %s on line %d

Warning: imagecreatetruecolor() expects exactly 2 parameters, 1 given in %s on line %d

Warning: imagecreatetruecolor() expects exactly 2 parameters, 3 given in %s on line %d