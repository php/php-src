--TEST--
Testing imagetruecolortopalette(): wrong parameters for parameter 2
--CREDITS--
Rafael Dohms <rdohms [at] gmail [dot] com>
--SKIPIF--
<?php 
	if (!extension_loaded("gd")) die("skip GD not present");
	if (!function_exists("imagecreatetruecolor")) die("skip GD Version not compatible");
?>
--FILE--
<?php
$image = imagecreatetruecolor(50, 50);
$resource = tmpfile();

$a = imagetruecolortopalette($image, $resource, 2);
$b = imagetruecolortopalette($image, array(), 2);

var_dump($a, $b);
//Both will return true in 5.2.x due to lack of parameter validation
?>
--EXPECTF--
bool(true)
bool(true)