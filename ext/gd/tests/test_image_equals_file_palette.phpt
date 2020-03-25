--TEST--
test_image_equals_file(): comparing palette images
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip gd extension not available');
?>
--FILE--
<?php
require_once __DIR__ . DIRECTORY_SEPARATOR . 'func.inc';

$im = imagecreate(10, 10);
imagecolorallocate($im, 255, 255, 255);
$red = imagecolorallocate($im, 255, 0, 0);
imagefilledrectangle($im, 3,3, 7,7, $red);

$filename = __DIR__ . DIRECTORY_SEPARATOR . 'test_image_equals_file_palette.png';
imagepng($im, $filename);

$im = imagecreate(10, 10);
imagecolorallocate($im, 255, 255, 255);
$blue = imagecolorallocate($im, 0, 0, 255);
imagefilledrectangle($im, 3,3, 7,7, $blue);

test_image_equals_file($filename, $im);

$im = imagecreate(10, 10);
imagecolorallocate($im, 255, 255, 255);
imagecolorallocate($im, 0, 0, 0);
$red = imagecolorallocate($im, 255, 0, 0);
imagefilledrectangle($im, 3,3, 7,7, $red);

test_image_equals_file($filename, $im);
?>
--EXPECT--
The images differ in 25 pixels.
The images are equal.
--CLEAN--
<?php
unlink(__DIR__ . DIRECTORY_SEPARATOR . 'test_image_equals_file_palette.png');
?>
