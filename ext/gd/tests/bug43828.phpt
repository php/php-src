--TEST--
Bug #43828 (broken transparency of imagearc for truecolor in blendingmode)
--EXTENSIONS--
gd
--SKIPIF--
<?php
if (!GD_BUNDLED && version_compare(GD_VERSION, '2.2.2', '<')) {
    die("skip test requires GD 2.2.2 or higher");
}
if (!(imagetypes() & IMG_PNG)) {
    die("skip No PNG support");
}
?>
--FILE--
<?php

$im = imagecreatetruecolor(100,100);

$transparent = imagecolorallocatealpha($im, 255, 255, 255, 80);
imagefilledrectangle($im, 0,0, 99,99, $transparent);
$color = imagecolorallocatealpha($im, 0, 255, 0, 100);
imagefilledarc($im, 49, 49, 99,99, 0 , 360, $color, IMG_ARC_PIE);

include_once __DIR__ . '/func.inc';
test_image_equals_file(__DIR__ . '/bug43828.png', $im);

imagedestroy($im);
?>
--EXPECT--
The images are equal.
