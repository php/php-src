--TEST--
Bug #79068 (gdTransformAffineCopy() changes interpolation method)
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip gd extension not available');
?>
--FILE--
<?php
require_once __DIR__ . '/func.inc';

$src = imagecreatetruecolor(100, 100);
imagefilledrectangle($src, 0, 0, 99, 99, 0xffffff);
imageline($src, 10, 10, 90, 90, 0x000000);
imagesetinterpolation($src, IMG_BSPLINE);
imageaffine($src, [1, 1, 1, 1, 1, 1]);
$dst = imagerotate($src, 80, 0xffffff);

test_image_equals_file(__DIR__ . '/bug79068.png', $dst);
?>
--EXPECT--
The images are equal.
