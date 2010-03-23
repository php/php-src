--TEST--
gif --> png conversion test
--SKIPIF--
<?php
	if (!extension_loaded('gd')) {
		die("skip gd extension not available.");
	}

	if (!function_exists("imagepng")) {
		die("skip png support unavailable");
	}
	if (!function_exists("imagecreatefromgif")) {
		die("skip gif read support unavailable");
	}
?>
--FILE--
<?php
	$cwd = dirname(__FILE__);

	echo "GIF to PNG conversion: ";
	echo imagepng(imagecreatefromgif($cwd . "/conv_test.gif"), $cwd . "/test_gif.png") ? 'ok' : 'failed';
	echo "\n";

	@unlink($cwd . "/test_gif.png");
?>
--EXPECT--
GIF to PNG conversion: ok
