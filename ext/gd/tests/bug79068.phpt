--TEST--
Bug #79068 (gdTransformAffineCopy() changes interpolation method)
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip gd extension not available');
?>
--FILE--
<?php
$src = imagecreatetruecolor(100, 100);
imagefilledrectangle($src, 0, 0, 99, 99, 0xffffff);
imageline($src, 10, 10, 90, 90, 0x000000);
imagesetinterpolation($src, IMG_BSPLINE);
imageaffine($src, [1, 1, 1, 1, 1, 1]);
var_dump(imagegetinterpolation($src) === IMG_BSPLINE);
?>
--EXPECT--
bool(true)
