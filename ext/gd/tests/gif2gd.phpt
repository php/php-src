--TEST--
gif --> gd1/gd2 conversion test
--SKIPIF--
<?php
	if (!extension_loaded('gd')) {
		die("skip gd extension not available.");
	}

	if (!function_exists("imagecreatefromgif")) {
		die("skip gif read support unavailable");
	}
?>
--FILE--
<?php
	$cwd = dirname(__FILE__);

	echo "GIF to GD1 conversion: ";
	echo imagegd(imagecreatefromgif($cwd . "/conv_test.gif"), $cwd . "/test.gd1") ? 'ok' : 'failed';
	echo "\n";

	echo "GIF to GD2 conversion: ";
	echo imagegd2(imagecreatefromgif($cwd . "/conv_test.gif"), $cwd . "/test.gd2") ? 'ok' : 'failed';
	echo "\n";

	@unlink($cwd . "/test.gd1");
	@unlink($cwd . "/test.gd2");
?>
--EXPECT--
GIF to GD1 conversion: ok
GIF to GD2 conversion: ok
