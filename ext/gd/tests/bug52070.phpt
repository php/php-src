--TEST--
Bug #52070 (imagedashedline() - dashed line sometimes is not visible)
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip gd extension not available');
?>
--FILE--
<?php
$im = imagecreate(1200, 800);
$background_color = imagecolorallocate($im, 40, 40, 40);
$color = imagecolorallocate($im, 255, 255, 255);
imagedashedline($im, 800, 400, 300, 400, $color);
imagedashedline($im, 800, 400, 300, 800, $color);
imagedashedline($im, 800, 400, 400, 800, $color);
imagedashedline($im, 800, 400, 500, 800, $color);
imagedashedline($im, 800, 400, 600, 800, $color);
imagedashedline($im, 800, 400, 700, 800, $color);
imagedashedline($im, 800, 400, 800, 800, $color);
include_once __DIR__ . '/func.inc';
test_image_equals_file(__DIR__ . '/bug52070.png', $im);
?>
===DONE===
--EXPECT--
The images are equal.
===DONE===
