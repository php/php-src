--TEST--
Bug #39082 (Output image to stdout segfaults).
--SKIPIF--
<?php 
	if (!extension_loaded('gd')) {	
		die("skip gd extension not available\n");
	}
	if (!GD_BUNDLED) {
		die('skip external GD libraries may fail');
	}
?>
--FILE--
<?php
$im = imagecreatetruecolor(1,1);
imagegif($im);
?>
--EXPECTF--
GIF87a%s
