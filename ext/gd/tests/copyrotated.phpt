--TEST--
imagecopyrotated
--EXTENSIONS--
gd
--SKIPIF--
<?php
    if (!function_exists("imagecopyrotated")) die("skip requires imagecopyrotated");
?>
--FILE--
<?php

$src_tc = imagecreatetruecolor(5,5);
imagefill($src_tc, 0,0, 0x000000);
imagesetpixel($src_tc, 1, 2, 0x0000ff);
imagesetpixel($src_tc, 2, 2, 0xffffff);
imagesetpixel($src_tc, 3, 2, 0xff0000);


$dst_tc = imagecreatetruecolor(5,5);
imagecopyrotated($dst_tc, $src_tc, 3.0,3.0, 0,0, imagesx($src_tc), imagesy($src_tc), 180);
$p1 = imagecolorat($dst_tc, 1, 2) == imagecolorat($src_tc, 3, 2);
$p2 = imagecolorat($dst_tc, 2, 2) == imagecolorat($src_tc, 2, 2);
$p3 = imagecolorat($dst_tc, 3, 2) == imagecolorat($src_tc, 1, 2);

if ($p1 && $p2 && $p3) {
    echo "TC/TC: ok\n";
}

try {
	imagecopyrotated($dst_tc, $src_tc, 3.0,3.0, -10,0, imagesx($src_tc), imagesy($src_tc), 180);
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}

try {
	imagecopyrotated($dst_tc, $src_tc, 3.0,3.0, 0,PHP_INT_MAX, imagesx($src_tc), imagesy($src_tc), 180);
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}

try {
	imagecopyrotated($dst_tc, $src_tc, 3.0,3.0, 0,0, -5, imagesy($src_tc), 180);
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}

try {
	imagecopyrotated($dst_tc, $src_tc, 3.0,3.0, 0,0, imagesx($src_tc), PHP_INT_MAX, 180);
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}

try {
	imagecopyrotated($dst_tc, $src_tc, 3.0,3.0, 0,0, imagesx($src_tc), imagesy($src_tc), PHP_INT_MAX);
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}

imagedestroy($src_tc); imagedestroy($dst_tc);
?>
--EXPECT--
TC/TC: ok
imagecopyrotated(): Argument #5 ($src_x) must be positive
imagecopyrotated(): Argument #6 ($src_y) must be positive
imagecopyrotated(): Argument #7 ($src_width) must be greater than 0
imagecopyrotated(): Argument #8 ($src_height) must be greater than 0
imagecopyrotated(): Argument #9 ($angle) must be greater than 0
