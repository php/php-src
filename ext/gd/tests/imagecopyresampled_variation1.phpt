--TEST--
Test for correct colors of imagecopyresampled() wrt. alpha
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip ext/gd required');
?>
--FILE--
<?php

const EXP_RED = 66;
const EXP_GREEN = 66;
const EXP_BLUE = 133;
const EXP_ALPHA = 32;

/* create the source image */
$im = imagecreatetruecolor(10, 10);
imagealphablending($im, false);
$solid = imagecolorallocate($im, 0, 100, 150);
$transparent = imagecolorallocatealpha($im, 200, 0, 100, 64);

/* draw a checker pattern */
for ($i = 0; $i < imagesx($im); $i++) {
    for ($j = 0; $j < imagesy($im); $j++) {
        imagesetpixel($im, $i, $j, ($i%2 != $j%2 ? $solid : $transparent));
    }
}

/* create the destination image */
$copy = imagecreatetruecolor(5, 5);
imagealphablending($copy, false);
imagesavealpha($copy, true);
imagecopyresampled($copy, $im, 0,0, 0,0, 5,5, 10, 10);

/* assert all pixels have the same color */
$color = imagecolorat($copy, 3, 3);
for ($i = 0; $i < imagesx($copy); $i++) {
    for ($j = 0; $j < imagesy($copy); $j++) {
        if (imagecolorat($copy, $i, $j) != $color) {
            echo 'different pixel values', PHP_EOL;
        }
    }
}

/* assign actual component values */
$red = ($color & 0xFF0000) >> 16;
$green = ($color & 0x00FF00) >> 8;
$blue = ($color & 0x0000FF);
$alpha = ($color & 0x7F000000) >> 24;

/* test for expected component values */
if (!($red >= EXP_RED - 1 && $red <= EXP_RED + 1)) {
    printf("red: expected roughly %d, got %d\n", EXP_RED, $red);
}
if (!($green >= EXP_GREEN - 1 && $green <= EXP_GREEN + 1)) {
    printf("green: expected roughly %d, got %d\n", EXP_GREEN, $green);
}
if (!($blue >= EXP_BLUE - 1 && $blue <= EXP_BLUE + 1)) {
    printf("blue: expected roughly %d, got %d\n", EXP_BLUE, $blue);
}
if (!($alpha >= EXP_ALPHA - 1 && $alpha <= EXP_ALPHA + 1)) {
    printf("alpha: expected roughly %d, got %d\n", EXP_ALPHA, $alpha);
}

imagedestroy($copy);
imagedestroy($im);

echo 'DONE';
?>
--EXPECT--
DONE
