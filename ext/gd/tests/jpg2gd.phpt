--TEST--
jpeg <--> gd1/gd2 conversion test
--SKIPIF--
<?php 
	if (!extension_loaded('gd')) {
		die("skip gd extension not avaliable.");
	}
	if (!GD_BUNDLED) {
		die("skip external GD libraries always fail");
	}	
	if (!function_exists("imagecreatefromjpeg") || !function_exists("imagejpeg")) {
		die("skip jpeg support unavailable");
	}
?>
--FILE--
<?php
	$cwd = dirname(__FILE__);

	echo "JPEG to GD1 conversion: ";
	echo imagegd(imagecreatefromjpeg($cwd . "/conv_test.jpeg"), $cwd . "/test.gd1") ? 'ok' : 'failed';
	echo "\n";
	
	echo "JPEG to GD2 conversion: ";
	echo imagegd2(imagecreatefromjpeg($cwd . "/conv_test.jpeg"), $cwd . "/test.gd2") ? 'ok' : 'failed';
	echo "\n";
	
	echo "GD1 to JPEG conversion: ";
	echo imagejpeg(imagecreatefromgd($cwd . "/test.gd1"), $cwd . "/test_gd1.jpeg") ? 'ok' : 'failed';
	echo "\n";
	
	echo "GD2 to JPEG conversion: ";
	echo imagejpeg(imagecreatefromgd2($cwd . "/test.gd2"), $cwd . "/test_gd2.jpeg") ? 'ok' : 'failed';
	echo "\n";
	
	@unlink($cwd . "/test.gd1");
	@unlink($cwd . "/test.gd2");
	@unlink($cwd . "/test_gd1.jpeg");
	@unlink($cwd . "/test_gd2.jpeg");
?>
--EXPECT--
JPEG to GD1 conversion: ok
JPEG to GD2 conversion: ok
GD1 to JPEG conversion: ok
GD2 to JPEG conversion: ok
