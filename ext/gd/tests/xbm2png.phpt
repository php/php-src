--TEST--
xbm --> png conversion test
--SKIPIF--
<?php 
	if (!extension_loaded('gd')) {
		die("skip gd extension not avaliable.");
	}
	if (!function_exists("imagepng")) {
		die("skip png support unavailable");
	}
	if (!function_exists("imagecreatefromxbm")) {
		die("skip xbm read support unavailable");
	}
?>
--FILE--
<?php
	$cwd = dirname(__FILE__);

	echo "XBM to PNG conversion: ";
	echo imagepng(imagecreatefromxbm($cwd . "/conv_test.xbm"), $cwd . "/test_xbm.png") ? 'ok' : 'failed';
	echo "\n";
	
	@unlink($cwd . "/test_xbm.png");
?>
--EXPECT--
XBM to PNG conversion: ok
