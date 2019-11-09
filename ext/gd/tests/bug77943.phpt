--TEST--
Bug #77943 (imageantialias($image, false); does not work)
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip gd extension not available');
?>
--FILE--
<?php
require_once __DIR__ . '/func.inc';

$width = 400;
$height = 300;
$im = imagecreatetruecolor($width, $height);
$white = imagecolorallocate($im, 255, 255, 255);
$blue = imagecolorallocate($im, 0, 0, 255);

imageantialias($im, false);
imagefilledrectangle($im, 0, 0, $width-1, $height-1, $white);

imageline($im, 0, 0, $width, $height, $blue);
imagesetthickness($im, 3);
imageline($im, 10, 0, $width, $height-10, $blue);

test_image_equals_file(__DIR__ . '/bug77943.png', $im);
?>
--EXPECT--
The images are equal.
