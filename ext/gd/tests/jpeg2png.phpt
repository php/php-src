--TEST--
jpeg <--> png conversion test
--SKIPIF--
<?php 
	if (!extension_loaded('gd')) {
		die("skip gd extension not avaliable.");
	}
	if (!GD_BUNDLED) {
		die("skip external GD libraries always fail");
	}	
	if (!function_exists("imagecreatefrompng") || !function_exists("imagepng")) {
		die("skip png support unavailable");
	}
?>
--FILE--
<?php
	$cwd = dirname(__FILE__);

	echo "PNG to JPEG conversion: ";
	echo imagejpeg(imagecreatefrompng($cwd . "/conv_test.png"), $cwd . "/test_jpeg.jpeg") ? 'ok' : 'failed';
	echo "\n";
	
	echo "Generated JPEG to PNG conversion: ";
	echo imagepng(imagecreatefromjpeg($cwd . "/test_jpeg.jpeg"), $cwd . "/test_jpng.png") ? 'ok' : 'failed';
	echo "\n";
	
	echo "JPEG to PNG conversion: ";
	echo imagepng(imagecreatefromjpeg($cwd . "/conv_test.jpeg"), $cwd . "/test_png.png") ? 'ok' : 'failed';
	echo "\n";
	
	echo "Generated PNG to JPEG conversion: ";
	echo imagejpeg(imagecreatefrompng($cwd . "/test_png.png"), $cwd . "/test_pjpeg.jpeg") ? 'ok' : 'failed';
	echo "\n";
	
	@unlink($cwd . "/test_jpeg.jpeg");
	@unlink($cwd . "/test_jpng.png");
	@unlink($cwd . "/test_png.png");
	@unlink($cwd . "/test_pjpeg.jpeg");
?>
--EXPECT--
PNG to JPEG conversion: ok
Generated JPEG to PNG conversion: ok
JPEG to PNG conversion: ok
Generated PNG to JPEG conversion: ok
