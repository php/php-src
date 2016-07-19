--TEST--
Bug #43828 (broken transparency of imagearc for truecolor in blendingmode)
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip ext/gd not available');
?>
--FILE--
<?php

$im = imagecreatetruecolor(100,100);

$transparent = imagecolorallocatealpha($im, 255, 255, 255, 80);
imagefilledrectangle($im, 0,0, 99,99, $transparent);
$color = imagecolorallocatealpha($im, 0, 255, 0, 100);
imagefilledarc($im, 49, 49, 99,99, 0 , 360, $color, IMG_ARC_PIE);

ob_start();
imagepng($im);
echo md5(ob_get_clean());
imagedestroy($im);
?>
--EXPECT--
3d82e4525f19790ae1055366e2a36917
