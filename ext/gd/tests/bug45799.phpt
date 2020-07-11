--TEST--
Bug #45799 (imagepng() crashes on empty image).
--SKIPIF--
<?php
	if (!extension_loaded('gd')) die("skip gd extension not available\n");
?>
--FILE--
<?php
$img = imagecreate(500,500);
imagepng($img);
imagedestroy($img);
?>
--EXPECTF--
Warning: imagepng(): gd-png error: no colors in palette in %s on line %d
