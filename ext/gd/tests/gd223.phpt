--TEST--
libgd bug 223 (gdImageRotateGeneric() does not properly interpolate)
--EXTENSIONS--
gd
--SKIPIF--
<?php
if (!GD_BUNDLED) die("skip only for bundled libgd");
?>
--FILE--
<?php
require_once __DIR__ . "/func.inc";

$im = imagecreatetruecolor(64, 64);
for ($j = 0; $j < 64; $j++) {
    for ($i = 0; $i < 64; $i++) {
        imagesetpixel($im, $i, $j, ($i % 2 || $j % 2) ? 0x000000 : 0xffffff);
    }
}

imagesetinterpolation($im, IMG_BICUBIC);
$im = imagerotate($im, 45, 0xff0000);

test_image_equals_file(__DIR__ . "/gd223.png", $im);
?>
--EXPECT--
The images are equal.
