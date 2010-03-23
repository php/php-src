--TEST--
xpm --> jpeg conversion test
--SKIPIF--
<?php
	if (!extension_loaded('gd')) {
		die("skip gd extension not available.");
	}

	if (!function_exists("imagejpeg")) {
		die("skip jpeg support unavailable");
	}
	if (!function_exists("imagecreatefromxpm")) {
		die("skip xpm read support unavailable");
	}
?>
--FILE--
<?php
	$cwd = dirname(__FILE__);

	echo "XPM to JPEG conversion: ";
	echo imagejpeg(imagecreatefromxpm($cwd . "/conv_test.xpm"), $cwd . "/test_xpm.jpeg") ? 'ok' : 'failed';
	echo "\n";

	@unlink($cwd . "/test_xpm.jpeg");
?>
--EXPECT--
XPM to JPEG conversion: ok
