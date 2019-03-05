--TEST--
Testing imagetruecolortopalette(): wrong parameters for parameter 3
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
imagetruecolortopalette($image, true, null);
?>
--EXPECTF--
Warning: imagetruecolortopalette(): Number of colors has to be greater than zero and no more than %d in %s on line %d
