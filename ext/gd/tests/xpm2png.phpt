--TEST--
xpm --> png conversion test
--SKIPIF--
<?php
	if (!extension_loaded('gd')) {
		die("skip gd extension not available.");
	}

	if (!function_exists("imagepng")) {
		die("skip png support unavailable");
	}
	if (!function_exists("imagecreatefromxpm")) {
		die("skip xpm read support unavailable");
	}
?>
--FILE--
<?php
	$cwd = dirname(__FILE__);

	echo "XPM to PNG conversion: ";
	echo imagepng(imagecreatefromxpm($cwd . "/conv_test.xpm"), $cwd . "/test_xpm.png") ? 'ok' : 'failed';
	echo "\n";

	@unlink($cwd . "/test_xpm.png");
?>
--EXPECT--
XPM to PNG conversion: ok
