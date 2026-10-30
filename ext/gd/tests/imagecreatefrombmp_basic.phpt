--TEST--
imagecreatefrombmp() - basic functionality
--EXTENSIONS--
gd
--SKIPIF--
<?php
if (!(imagetypes() & IMG_BMP)) die('skip BMP support required');
if (!(imagetypes() & IMG_PNG)) {
    die("skip No PNG support");
}
?>
--FILE--
<?php
// create an image from a BMP file
$im = imagecreatefrombmp(__DIR__ . '/imagecreatefrombmp_basic.bmp');

include_once __DIR__ . '/func.inc';
test_image_equals_file(__DIR__ . '/imagecreatefrombmp_basic.png', $im);
?>
--EXPECT--
The images are equal.
