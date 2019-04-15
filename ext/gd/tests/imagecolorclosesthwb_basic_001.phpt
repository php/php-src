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

	$image = imagecreatefromjpeg($cwd . "/conv_test.jpeg");
	var_dump(imagecolorclosesthwb($image, 12, 14, 25));

	$image = imagecreatefromjpeg($cwd . "/conv_test.jpeg");
	var_dump(imagecolorclosesthwb($image, 1, 1, 1));

	$image = imagecreatefrompng($cwd . "/test.png");
	var_dump(imagecolorclosesthwb($image, 1, 12, 12));

	$image = imagecreatefrompng($cwd . "/test.png");
	var_dump(imagecolorclosesthwb($image, 116, 120, 115));

	imagedestroy($image);
?>
--EXPECT--
int(7631768)
int(790041)
int(65793)
int(68620)
int(7633011)
int(87)
