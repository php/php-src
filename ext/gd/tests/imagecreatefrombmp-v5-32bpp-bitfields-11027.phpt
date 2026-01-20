--TEST--
imagecreatefrombmp() - test image with BITMAPV5HEADER, 32bpp and (unnecessary) bitfields
--EXTENSIONS--
gd
--SKIPIF--
<?php
if (!(imagetypes() & IMG_BMP)) die('skip BMP support required');
?>
--FILE--
<?php
// create an image from a BMP file
$im = imagecreatefrombmp(__DIR__ . '/imagecreatefrombmp-v5-32bpp-bitfields-11027-input.bmp');

include_once __DIR__ . '/func.inc';
test_image_equals_file(__DIR__ . '/imagecreatefrombmp-v3info-32bpp-bitfields-11027-output.png', $im);
?>
--EXPECT--
The images are equal.
