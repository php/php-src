--TEST--
jpeg <--> gd1/gd2 conversion test
--SKIPIF--
<?php
	if (!extension_loaded('gd')) {
		die("skip gd extension not available.");
	}

	if (!function_exists("imagecreatefromjpeg") || !function_exists("imagejpeg")) {
		die("skip jpeg support unavailable");
	}
?>
--FILE--
<?php
	$cwd = __DIR__;

	echo "JPEG to GD1 conversion: ";
	echo imagegd(imagecreatefromjpeg($cwd . "/conv_test私はガラスを食べられます.jpg"), $cwd . "/test私はガラスを食べられます.gd1") ? 'ok' : 'failed';
	echo "\n";

	echo "JPEG to GD2 conversion: ";
	echo imagegd2(imagecreatefromjpeg($cwd . "/conv_test私はガラスを食べられます.jpg"), $cwd . "/test私はガラスを食べられます.gd2") ? 'ok' : 'failed';
	echo "\n";

	echo "GD1 to JPEG conversion: ";
	echo imagejpeg(imagecreatefromgd($cwd . "/test私はガラスを食べられます.gd1"), $cwd . "/test_gd1.jpeg") ? 'ok' : 'failed';
	echo "\n";

	echo "GD2 to JPEG conversion: ";
	echo imagejpeg(imagecreatefromgd2($cwd . "/test私はガラスを食べられます.gd2"), $cwd . "/test_gd2.jpeg") ? 'ok' : 'failed';
	echo "\n";

	@unlink($cwd . "/test私はガラスを食べられます.gd1");
	@unlink($cwd . "/test私はガラスを食べられます.gd2");
	@unlink($cwd . "/test_gd1.jpeg");
	@unlink($cwd . "/test_gd2.jpeg");
?>
--EXPECT--
JPEG to GD1 conversion: ok
JPEG to GD2 conversion: ok
GD1 to JPEG conversion: ok
GD2 to JPEG conversion: ok
