--TEST--
imageopenpolygon(): basic test
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip gd extension not available');
?>
--FILE--
<?php
require_once __DIR__ . DIRECTORY_SEPARATOR . 'func.inc';

$im = imagecreatetruecolor(100, 100);
$white = imagecolorallocate($im, 255, 255, 255);
$black = imagecolorallocate($im, 0, 0, 0);
$red = imagecolorallocate($im, 255, 0, 0);
$green = imagecolorallocate($im, 0, 128, 0);
$blue = imagecolorallocate($im, 0, 0, 255);
imagefilledrectangle($im, 0,0, 99,99, $white);

imageopenpolygon($im, [10,10, 49,89, 89,10], $black);
imageopenpolygon($im, [10,89, 35,10, 60,89, 85,10], $red);
imageopenpolygon($im, [10,49, 30,89, 50,10, 70,89, 90,10], $green);
imageopenpolygon($im, [10,50, 25,10, 40,89, 55,10, 80,89, 90,50], $blue);

test_image_equals_file(__DIR__ . DIRECTORY_SEPARATOR . 'imageopenpolygon.png', $im);
?>
--EXPECT--
The images are equal.
