--TEST--
Testing imagetruecolortopalette() of GD library
--CREDITS--
Rafael Dohms <rdohms [at] gmail [dot] com>
--EXTENSIONS--
gd
--SKIPIF--
<?php
    if (!function_exists("imagecreatetruecolor")) die("skip GD Version not compatible");
    if (!(imagetypes() & IMG_PNG)) {
        die("skip No PNG support");
    }
?>
--FILE--
<?php
// Create a 200x100 image
$image = imagecreatetruecolor(200, 100);
$white = imagecolorallocate($image, 0xFF, 0xFF, 0xFF);
$black = imagecolorallocate($image, 0x00, 0x00, 0x00);

// Set the background to be white
imagefilledrectangle($image, 0, 0, 299, 99, $white);

// Set the line thickness to 5
imagesetthickness($image, 5);

// Draw the rectangle
imagerectangle($image, 14, 14, 185, 85, $black);

include_once __DIR__ . '/func.inc';
test_image_equals_file(__DIR__ . '/imagesetthickness_basic.png', $image);
?>
--EXPECT--
The images are equal.
