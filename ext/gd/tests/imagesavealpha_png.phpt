--TEST--
Testing imagesavealpha() with PNG
--EXTENSIONS--
gd
--SKIPIF--
<?php
$support = gd_info();
if (!isset($support['PNG Support']) || $support['PNG Support'] === false) {
    die("skip PNG support not available");
}
?>
--FILE--
<?php
$filename = __DIR__ . "/imagesavealpha_png.png";
$im = imagecreatetruecolor(8, 8);
imagealphablending($im, false);
imagefilledrectangle($im, 0, 0, 7, 7, 0x7f000000);

echo "without alpha\n";
imagesavealpha($im, false);
imagepng($im, $filename);
$im1 = imagecreatefrompng($filename);
checkColors($im1, 0x000000);
imagedestroy($im1);

echo "with alpha\n";
imagesavealpha($im, true);
imagepng($im, $filename);
$im1 = imagecreatefrompng($filename);
checkColors($im1, 0x7f000000);
imagedestroy($im1);

function checkColors(GdImage $im, int $expected) {
    for ($i = 0; $i < 8; $i++) {
        for ($j = 0; $j < 8; $j++) {
            $actual = imagecolorat($im, $i, $j);
            if ($actual !== $expected) {
                echo "($i, $j) is $actual, but $expected expected\n";
            }
        }
    }
}
?>
--CLEAN--
<?php
@unlink(__DIR__ . "/imagesavealpha_png.png");
?>
--EXPECT--
without alpha
with alpha
