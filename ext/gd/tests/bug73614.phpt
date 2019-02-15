--TEST--
Bug #73614 (gdImageFilledArc() doesn't properly draw pies)
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip gd extension not available');
if (!GD_BUNDLED && version_compare(GD_VERSION, '2.2.5', '<=')) die('skip upstream bugfix not yet released');
?>
--FILE--
<?php
require_once __DIR__ . '/func.inc';

$image = imagecreatetruecolor(500, 500);

$white    = imagecolorallocate($image, 0xFF, 0xFF, 0xFF);
$navy     = imagecolorallocate($image, 0x00, 0x00, 0x80);
$red      = imagecolorallocate($image, 0xFF, 0x00, 0x00);

imagefilledarc($image, 250, 250, 500, 250, 0, 88, $white, IMG_ARC_PIE);
imagefilledarc($image, 250, 250, 500, 250, 88, 91 , $navy, IMG_ARC_PIE);
imagefilledarc($image, 250, 250, 500, 250, 91, 360 , $red, IMG_ARC_PIE);

test_image_equals_file(__DIR__ . '/bug73614.png', $image);
?>
===DONE===
--EXPECT--
The images are equal.
===DONE===
