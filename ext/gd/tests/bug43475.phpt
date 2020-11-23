--TEST--
Bug #43475 (Thick styled lines have scrambled patterns)
--SKIPIF--
<?php
    if (!extension_loaded('gd')) die("skip gd extension not available\n");
    if (!GD_BUNDLED && version_compare(GD_VERSION, '2.2.2', '<')) {
        die("skip test requires GD 2.2.2 or higher");
    }
?>
--FILE--
<?php
require_once __DIR__ . '/similarity.inc';

function setStyleAndThickness($im, $color, $thickness)
{
    $style = array();
    $i = 0;
    while ($i < 16 * $thickness) {
        $style[$i++] = $color;
    }
    while ($i < 20 * $thickness) {
        $style[$i++] = IMG_COLOR_TRANSPARENT;
    }
    while ($i < 28 * $thickness) {
        $style[$i++] = $color;
    }
    while ($i < 32 * $thickness) {
        $style[$i++] = IMG_COLOR_TRANSPARENT;
    }
    imagesetstyle($im, $style);
    imagesetthickness($im, $thickness);
}

$im = imagecreate(800, 800);
imagecolorallocate($im, 255, 255, 255);
$black = imagecolorallocate($im, 0, 0, 0);

setStyleAndThickness($im, $black, 1);
imageline($im,  50, 250, 550, 250, IMG_COLOR_STYLED);
imageline($im, 550, 250, 550, 750, IMG_COLOR_STYLED);
imageline($im, 550, 750,  50, 250, IMG_COLOR_STYLED);

setStyleAndThickness($im, $black, 2);
imageline($im, 100, 200, 600, 200, IMG_COLOR_STYLED);
imageline($im, 600, 200, 600, 700, IMG_COLOR_STYLED);
imageline($im, 600, 700, 100, 200, IMG_COLOR_STYLED);

setStyleAndThickness($im, $black, 4);
imageline($im, 150, 150, 650, 150, IMG_COLOR_STYLED);
imageline($im, 650, 150, 650, 650, IMG_COLOR_STYLED);
imageline($im, 650, 650, 150, 150, IMG_COLOR_STYLED);

setStyleAndThickness($im, $black, 6);
imageline($im, 200, 100, 700, 100, IMG_COLOR_STYLED);
imageline($im, 700, 100, 700, 600, IMG_COLOR_STYLED);
imageline($im, 700, 600, 200, 100, IMG_COLOR_STYLED);

$ex = imagecreatefrompng(__DIR__ . '/bug43475.png');
var_dump(calc_image_dissimilarity($ex, $im) < 1e-5);
?>
--EXPECT--
bool(true)
