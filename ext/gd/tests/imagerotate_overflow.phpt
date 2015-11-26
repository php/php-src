--TEST--
imagerotate() overflow with negative numbers
--SKIPIF--
<?php
	if (!extension_loaded('gd')) {
		die("skip gd extension not available.");
	}

	if (!function_exists('imagerotate')) {
		die("skip imagerotate() not available.");
	}
?>
--FILE--
<?php

$im = imagecreate(10, 10);

$tmp = imagerotate ($im, 5, -9999999);

var_dump($tmp);

if ($tmp) {
        imagedestroy($tmp);
}

if ($im) {
        imagedestroy($im);
}

?>
--EXPECT--
bool(false)
