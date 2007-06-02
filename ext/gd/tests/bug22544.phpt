--TEST--
Bug #22544 (TrueColor transparency in PNG images).
--SKIPIF--
<?php
	if (!extension_loaded('gd')) {
		die("skip gd extension not available\n");
	}
?>
--FILE--
<?php
	$dest = dirname(realpath(__FILE__)) . '/bug22544.png';
	@unlink($dest);
	$image = imageCreateTruecolor(640, 100);
	$transparent = imageColorAllocate($image, 0, 0, 0);
	$red = imageColorAllocate($image, 255, 50, 50);
	imageColorTransparent($image, $transparent);
	imageFilledRectangle($image, 0, 0, 640-1, 100-1, $transparent);
	imagePng($image, $dest);
	echo md5_file($dest) . "\n";
	@unlink($dest);
?>
--EXPECT--
10a57d09a2c63fad87b85b38d6b258d6
