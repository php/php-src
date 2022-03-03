--TEST--
Test imagecolorset() function : basic functionality
--CREDITS--
Erick Belluci Tedeschi <erickbt86 [at] gmail [dot] com>
#testfest PHPSP on 2009-06-20
--EXTENSIONS--
gd
--FILE--
<?php
// Create a 300x100 image
$im = imagecreate(300, 100);

// Set the background to be red
imagecolorallocate($im, 255, 0, 0);

// Get the color index for the background
$bg = imagecolorat($im, 0, 0);

// Set the background to be blue
imagecolorset($im, $bg, 0, 0, 255);

include_once __DIR__ . '/func.inc';
test_image_equals_file(__DIR__ . '/imagecolorset_basic.png', $im);
imagedestroy($im);
?>
--EXPECT--
The images are equal.
