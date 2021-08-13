--TEST--
Testing imagefilledellipse() of GD library
--EXTENSIONS--
gd
--FILE--
<?php

$image = imagecreatetruecolor(100, 100);

$white = imagecolorallocate($image, 0xFF, 0xFF, 0xFF);

//create an ellipse and fill it with white color
imagefilledellipse($image, 50, 50, 40, 30, $white);

include_once __DIR__ . '/func.inc';
test_image_equals_file(__DIR__ . '/imagefilledellipse_basic.png', $image);
?>
--EXPECT--
The images are equal.
