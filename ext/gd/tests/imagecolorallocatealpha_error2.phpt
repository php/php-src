--TEST--
Testing imagecolorallocatealpha(): Wrong types for parameter 2
--CREDITS--
Rafael Dohms <rdohms [at] gmail [dot] com>
--SKIPIF--
<?php
	if (!extension_loaded("gd")) die("skip GD not present");
?>
--FILE--
<?php
$img = imagecreatetruecolor(200, 200);

imagecolorallocatealpha($img, 'string-non-numeric', 255, 255, 50);
imagecolorallocatealpha($img, array(), 255, 255, 50);
imagecolorallocatealpha($img, tmpfile(), 255, 255, 50);
?>
--EXPECTF--
Warning: imagecolorallocatealpha() expects parameter 2 to be integer, %s given in %s on line %d

Warning: imagecolorallocatealpha() expects parameter 2 to be integer, array given in %s on line %d

Warning: imagecolorallocatealpha() expects parameter 2 to be integer, resource given in %s on line %d
