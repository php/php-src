--TEST--
Bug #39508 (imagefill crashes with small images 3 pixels or less)
--SKIPIF--
<?php
	if (!extension_loaded('gd')) die("skip gd extension not available\n");
?>
--FILE--
<?php
$img =imagecreatefromgd2part("foo.png",0, 100, 0, 100);
?>
--EXPECTF--
Warning: imagecreatefromgd2part(): Zero width or height not allowed in %s on line %d
