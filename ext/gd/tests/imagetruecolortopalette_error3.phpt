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
$resource = tmpfile();

imagetruecolortopalette($image, true, 'string');
imagetruecolortopalette($image, true, $resource);
imagetruecolortopalette($image, true, array());
imagetruecolortopalette($image, true, null);

?>
--EXPECTF--
Warning: imagetruecolortopalette() expects parameter 3 to be long, string given in %s on line %d

Warning: imagetruecolortopalette() expects parameter 3 to be long, resource given in %s on line %d

Warning: imagetruecolortopalette() expects parameter 3 to be long, array given in %s on line %d

Warning: imagetruecolortopalette(): Number of colors has to be greater than zero in %s on line %d