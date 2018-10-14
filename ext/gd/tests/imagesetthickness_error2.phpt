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

imagesetthickness($image, 's');
imagesetthickness($image, array());
imagesetthickness($image, $image);

?>
--EXPECTF--
Warning: imagesetthickness() expects parameter 2 to be integer, string given in %s on line %d

Warning: imagesetthickness() expects parameter 2 to be integer, array given in %s on line %d

Warning: imagesetthickness() expects parameter 2 to be integer, resource given in %s on line %d
