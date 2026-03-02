--TEST--
Bug #64641 (imagefilledpolygon doesn't draw horizontal line)
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
require_once __DIR__ . '/func.inc';

$im = imagecreatetruecolor(640, 480);

$points = array(
    100, 100,
    100, 200,
    100, 300
);
imagefilledpolygon($im, $points, 0xFFFF00);

$points = array(
    300, 200,
    400, 200,
    500, 200
);
imagefilledpolygon($im, $points, 0xFFFF00);

test_image_equals_file(__DIR__ . '/bug64641.png', $im);
?>
--EXPECT--
The images are equal.
