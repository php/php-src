--TEST--
GH-17349 (Tiled truecolor filling looses single color transparency)
--EXTENSIONS--
gd
--FILE--
<?php
require_once __DIR__ . "/func.inc";

$tile = imagecreatetruecolor(8, 8);
$red = imagecolorallocate($tile, 255, 0, 0);
imagefilledrectangle($tile, 0, 0, 7, 7, $red);
imagecolortransparent($tile, $red);

$im = imagecreatetruecolor(64, 64);
$bg = imagecolorallocate($im, 255, 255, 255);
imagefilledrectangle($im, 0, 0, 63, 63, $bg);
imagecolortransparent($im, $bg);
$fg = imagecolorallocate($im, 0, 0, 0);
imageellipse($im, 31, 31, 50, 50, $fg);
imagesettile($im, $tile);
imagealphablending($im, false);
imagefill($im, 31, 31, IMG_COLOR_TILED);

test_image_equals_file(__DIR__ . "/gh17349.png", $im);
?>
--EXPECT--
The images are equal.
