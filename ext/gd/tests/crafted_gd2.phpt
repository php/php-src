--TEST--
Test max colors for a gd image.
--SKIPIF--
<?php
	if (!extension_loaded('gd')) {
		die("skip gd extension not available\n");
	}
?>
--FILE--
<?php
imagecreatefromgd(__DIR__ . '/crafted.gd2');
?>
--EXPECTF--
Warning: imagecreatefromgd(): '%scrafted.gd2' is not a valid GD file in %s on line %d
