--TEST--
Testing imagesavealpha() with WebP
--EXTENSIONS--
gd
--SKIPIF--
<?php
$support = gd_info();
if (!isset($support['WebP Support']) || $support['WebP Support'] === false) {
    die("skip WebP support not available");
}
?>
--FILE--
<?php
$filename = __DIR__ . "/imagesavealpha_webp.webp";
$im = imagecreatetruecolor(8, 8);
imagealphablending($im, false);
imagefilledrectangle($im, 0, 0, 7, 7, 0x7f000000);

echo "without alpha\n";
imagesavealpha($im, false);
imagewebp($im, $filename, 101); // lossless
$im1 = imagecreatefromwebp($filename);
checkColors($im1, 0x000000);
imagedestroy($im1);

echo "with alpha\n";
imagesavealpha($im, true);
imagewebp($im, $filename, 101); // lossless
$im1 = imagecreatefromwebp($filename);
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
@unlink(__DIR__ . "/imagesavealpha_webp.webp");
?>
--EXPECT--
without alpha
with alpha
