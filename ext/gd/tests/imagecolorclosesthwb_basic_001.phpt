--TEST--
Test imagecolorclosesthwb() basic functionality
--CREDITS--
Rodrigo Prado de Jesus <royopa [at] gmail [dot] com>
#PHPSP User Group Brazil
--SKIPIF--
<?php
	if (!extension_loaded('gd')) {
		die("skip gd extension not available.");
	}
?>
--FILE--
<?php
	$cwd = dirname(__FILE__);
	$image = imagecreatefromjpeg($cwd . "/conv_test.jpeg");
	var_dump(imagecolorclosesthwb($image, 116, 115, 152));
	imagedestroy($image);
?>
--EXPECTF--
int(%i)
