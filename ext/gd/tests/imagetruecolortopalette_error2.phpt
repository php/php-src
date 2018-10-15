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

imagetruecolortopalette($image, $resource, 2);
imagetruecolortopalette($image, array(), 2);

?>
--EXPECTF--
Warning: imagetruecolortopalette() expects parameter 2 to be bool, resource given in %s on line %d

Warning: imagetruecolortopalette() expects parameter 2 to be bool, array given in %s on line %d
