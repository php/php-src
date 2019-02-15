--TEST--
Bug #77198 (threshold cropping has insufficient precision)
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip gd extension not available');
if (!GD_BUNDLED) die('upstream bugfix has not been released');
?>
--FILE--
<?php

function createWhiteImageWithBlackPixelAt($x, $y)
{
    $im = imagecreatetruecolor(8, 8);
    imagefilledrectangle($im, 0, 0, 7, 7, 0xffffff);
    imagesetpixel($im, $x, $y, 0x000000);
    return $im;
}

for ($y = 0; $y < 8; $y++) {
    for ($x = 0; $x < 8; $x++) {
        $orig = createWhiteImageWithBlackPixelAt($x, $y);
        $cropped = imagecropauto($orig, IMG_CROP_THRESHOLD, 1, 0xffffff);
        if (!$cropped) {
            printf("Pixel at %d, %d: unexpected NULL crop\n", $x, $y);
        } else {
            $width = imagesx($cropped);
            if ($width !== 1) {
                printf("Pixel at %d, %d: unexpected width (%d)\n", $x, $y, $width);
            }
            $height = imagesy($cropped);
            if ($height !== 1) {
                printf("Pixel at %d, %d: unexpected height (%d)\n", $x, $y, $height);
            }
            $color = imagecolorat($cropped, 0, 0);
            if ($color !== 0x000000) {
                printf("Pixel at %d, %d: unexpected color (%d)\n", $x, $y, $color);
            }
            imagedestroy($cropped);
        }
        imagedestroy($orig);
    }
}

?>
===DONE===
--EXPECT--
===DONE===
