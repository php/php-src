--TEST--
imagefill() infinite loop with wrong color index
--SKIPIF--
<?php 
	if (!extension_loaded('gd')) {
		die("skip gd extension not available.");
	}
	if (!function_exists("imagefttext")) {
		die("skip imagefttext() not available.");
	}
?>
--FILE--
<?php
$im = imagecreate(100,100);
$white = imagecolorallocate($im, 255,255,255);
$blue = imagecolorallocate($im, 0,0,255);
$green = imagecolorallocate($im, 0,255,0);

print_r(imagecolorat($im, 0,0));
imagefill($im, 0,0,$white + 3);
print_r(imagecolorat($im, 0,0));
imagedestroy($im);
?>
--EXPECT--
00
