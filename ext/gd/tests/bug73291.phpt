--TEST--
Bug #73291 (imagecropauto() $threshold differs from external libgd)
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip gd extension not available');
?>
--FILE--
<?php

$src = imagecreatetruecolor(255, 255);
$white = imagecolorallocate($src, 255, 255, 255);
imagefilledrectangle($src, 0, 0, 254, 254, $white);

for ($i = 254; $i > 0; $i--) {
    $color = imagecolorallocate($src, $i, $i, $i);
    imagefilledellipse($src, 127, 127, $i, $i, $color);
}

foreach ([0.1, 0.5, 1.0, 10.0] as $threshold) {
    $dst = imagecropauto($src, IMG_CROP_THRESHOLD, $threshold, $white);
    if ($dst !== false) {
        printf("size: %d*%d\n", imagesx($dst), imagesy($dst));
    } else {
        echo "cropped to zero size\n";
    }
}

?>
--EXPECT--
size: 247*247
size: 237*237
size: 229*229
size: 175*175
