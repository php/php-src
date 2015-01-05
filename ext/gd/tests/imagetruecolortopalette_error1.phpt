--TEST--
Testing imagetruecolortopalette(): wrong parameters for parameter 1
--CREDITS--
Rafael Dohms <rdohms [at] gmail [dot] com>
--SKIPIF--
<?php 
	if (!extension_loaded("gd")) die("skip GD not present");
	if (!function_exists("imagecreatetruecolor")) die("skip GD Version not compatible");
?>
--FILE--
<?php
$resource = tmpfile();

imagetruecolortopalette($resource, true, 2);
imagetruecolortopalette('string', true, 2);
imagetruecolortopalette(array(), true, 2);
imagetruecolortopalette(null, true, 2);
?>
--EXPECTF--
Warning: imagetruecolortopalette(): supplied resource is not a valid Image resource in %s on line %d

Warning: imagetruecolortopalette() expects parameter 1 to be resource, %s given in %s on line %d

Warning: imagetruecolortopalette() expects parameter 1 to be resource, array given in %s on line %d

Warning: imagetruecolortopalette() expects parameter 1 to be resource, null given in %s on line %d