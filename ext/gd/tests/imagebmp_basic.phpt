--TEST--
imagebmp() - basic functionality
--EXTENSIONS--
gd
--FILE--
<?php
// create an image
$im = imagecreate(100, 100);
imagecolorallocate($im, 0, 0, 0);
$white = imagecolorallocate($im, 255, 255, 255);
imageline($im, 10,10, 89,89, $white);
imagepalettetotruecolor($im);
require __DIR__ . "/func.inc";
test_image_equals_file(__DIR__ . "/imagebmp_basic.png", $im);
?>
--EXPECT--
The images are equal.
