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
Warning: imagecreatetruecolor() expects parameter 1 to be long, %s given in %s on line %d

Warning: imagecreatetruecolor() expects parameter 2 to be long, %s given in %s on line %d
