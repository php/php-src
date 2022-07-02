--TEST--
Bug #51498 (imagefilledellipse does not work for large circles)
--EXTENSIONS--
gd
--SKIPIF--
<?php
if (!GD_BUNDLED && version_compare(GD_VERSION, "2.3.0") < 0) {
    die("skip test requires GD 2.3.0 or higher");
}
?>
--FILE--
<?php
$img = imagecreatetruecolor(2200, 2200);
$white = imagecolorallocate($img, 255, 255, 255);
imagefilledellipse($img, 1100, 1100, 2200, 2200, $white);

require_once __DIR__ . '/func.inc';
test_image_equals_file(__DIR__ . '/bug51498_exp.png', $img);
?>
--EXPECT--
The images are equal.
