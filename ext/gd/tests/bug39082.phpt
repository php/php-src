--TEST--
Bug #39082 (Output image to stdout segfaults).
--SKIPIF--
<?php
	if (!extension_loaded('gd')) {
		die("skip gd extension not available\n");
	}
?>
--FILE--
<?php
$im = imagecreatetruecolor(1,1);
imagegif($im);
?>
--EXPECTF--
GIF87a%s
