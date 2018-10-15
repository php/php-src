--TEST--
Testing imagecolorallocatealpha(): Wrong types for parameter 3
--CREDITS--
Rafael Dohms <rdohms [at] gmail [dot] com>
--SKIPIF--
<?php
	if (!extension_loaded("gd")) die("skip GD not present");
?>
--FILE--
<?php
$img = imagecreatetruecolor(200, 200);

imagecolorallocatealpha($img, 255, 'string-non-numeric', 255, 50);
imagecolorallocatealpha($img, 255, array(), 255, 50);
imagecolorallocatealpha($img, 255, tmpfile(), 255, 50);
?>
--EXPECTF--
Warning: imagecolorallocatealpha() expects parameter 3 to be integer, %s given in %s on line %d

Warning: imagecolorallocatealpha() expects parameter 3 to be integer, array given in %s on line %d

Warning: imagecolorallocatealpha() expects parameter 3 to be integer, resource given in %s on line %d
