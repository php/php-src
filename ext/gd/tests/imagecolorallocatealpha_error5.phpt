--TEST--
Testing imagecolorallocatealpha(): Wrong types for parameter 5
--CREDITS--
Rafael Dohms <rdohms [at] gmail [dot] com>
--SKIPIF--
<?php
	if (!extension_loaded("gd")) die("skip GD not present");
?>
--FILE--
<?php
$img = imagecreatetruecolor(200, 200);

imagecolorallocatealpha($img, 255, 255, 255, 'string-non-numeric');
imagecolorallocatealpha($img, 255, 255, 255, array());
imagecolorallocatealpha($img, 255, 255, 255, tmpfile());
?>
--EXPECTF--
Warning: imagecolorallocatealpha() expects parameter 5 to be int, %s given in %s on line %d

Warning: imagecolorallocatealpha() expects parameter 5 to be int, array given in %s on line %d

Warning: imagecolorallocatealpha() expects parameter 5 to be int, resource given in %s on line %d
