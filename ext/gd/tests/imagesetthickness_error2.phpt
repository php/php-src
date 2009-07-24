--TEST--
Testing imagetruecolortopalette(): wrong types for second parameter
--CREDITS--
Rafael Dohms <rdohms [at] gmail [dot] com>
--SKIPIF--
<?php 
	if (!extension_loaded("gd")) die("skip GD not present");
	if (!function_exists("imagecreatetruecolor")) die("skip GD Version not compatible");
?>
--FILE--
<?php
$image = imagecreatetruecolor(200, 100);

$a = imagesetthickness($image, 's');
$b = imagesetthickness($image, array());
$c = imagesetthickness($image, $image);

var_dump($a, $b, $c);
//All should be true because 5,2 does not check parameters
?>
--EXPECTF--
bool(true)
bool(true)
bool(true)