--TEST--
Bug #64641 (imagefilledpolygon doesn't draw horizontal line)
--EXTENSIONS--
gd
--SKIPIF--
<?php
if (!GD_BUNDLED && version_compare(GD_VERSION, '2.2.2', '<')) {
    die("skip test requires GD 2.2.2 or higher");
}
?>
--FILE--
<?php
require_once __DIR__ . '/similarity.inc';

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

$ex = imagecreatefrompng(__DIR__ . '/bug64641.png');
if (($diss = calc_image_dissimilarity($ex, $im)) < 1e-5) {
    echo "IDENTICAL";
} else {
    echo "DISSIMILARITY: $diss";
}
imagedestroy($ex);

imagedestroy($im);
?>
--EXPECT--
IDENTICAL
