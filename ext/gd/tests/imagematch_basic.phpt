--TEST--
imagematch() function test
--EXTENSIONS--
gd
--SKIPIF--
<?php
if (!(imagetypes() & IMG_PNG)) die("skip No PNG support");
?>
--FILE--
<?php

/* sample palette image, red/green */
$palette_1 = imagecreate(2, 1);
imagesetpixel($palette_1, 0, 0, imagecolorallocate($palette_1, 255, 0, 0));
imagesetpixel($palette_1, 1, 0, imagecolorallocate($palette_1, 0, 255, 0));

/* same image but with inverted colors allocations */
$palette_1b = imagecreate(2, 1);
imagesetpixel($palette_1b, 1, 0, imagecolorallocate($palette_1b, 0, 255, 0));
imagesetpixel($palette_1b, 0, 0, imagecolorallocate($palette_1b, 255, 0, 0));

/* a different palette image, red/blue */
$palette_2 = imagecreate(2, 1);
imagesetpixel($palette_2, 0, 0, imagecolorallocate($palette_2, 255, 0, 0));
imagesetpixel($palette_2, 1, 0, imagecolorallocate($palette_2, 0, 0, 255));

/* a truecolor image, matching the first one */
$truecolor_1 = imagecreatetruecolor(2, 1);
imagesetpixel($truecolor_1, 0, 0, imagecolorallocate($truecolor_1, 255, 0, 0));
imagesetpixel($truecolor_1, 1, 0, imagecolorallocate($truecolor_1, 0, 255, 0));

/* a truecolor image, identical to the previous one */
$truecolor_1a = imagecreatetruecolor(2, 1);
imagesetpixel($truecolor_1a, 0, 0, imagecolorallocate($truecolor_1a, 255, 0, 0));
imagesetpixel($truecolor_1a, 1, 0, imagecolorallocate($truecolor_1a, 0, 255, 0));

/* a truecolor image, differing only by color */
$truecolor_2 = imagecreatetruecolor(2, 1);
imagesetpixel($truecolor_2, 0, 0, imagecolorallocate($truecolor_2, 255, 0, 0));
imagesetpixel($truecolor_2, 1, 0, imagecolorallocate($truecolor_2, 0, 0, 255));

/* a truecolor image, differing only by alpha */
$truecolor_3 = imagecreatetruecolor(2, 1);
imagealphablending($truecolor_3, false);
imagesetpixel($truecolor_3, 0, 0, imagecolorallocatealpha($truecolor_3, 255, 0, 0, 0));
imagesetpixel($truecolor_3, 1, 0, imagecolorallocatealpha($truecolor_3, 0, 255, 0, 1));

print   'imagematch($palette_1, $palette_1b) = ';
var_dump(imagematch($palette_1, $palette_1b));

print   'imagematch($palette_1, $palette_2) = ';
var_dump(imagematch($palette_1, $palette_2));

print   'imagematch($palette_1, $truecolor_1) = ';
var_dump(imagematch($palette_1, $truecolor_1));

print   'imagematch($truecolor_1, $truecolor_1a) = ';
var_dump(imagematch($truecolor_1, $truecolor_1a));

print   'imagematch($truecolor_1, $truecolor_2) = ';
var_dump(imagematch($truecolor_1, $truecolor_2));

print   'imagematch($truecolor_1, $truecolor_3) = ';
var_dump(imagematch($truecolor_1, $truecolor_3));
--EXPECT--
imagematch($palette_1, $palette_1b) = bool(true)
imagematch($palette_1, $palette_2) = bool(false)
imagematch($palette_1, $truecolor_1) = bool(true)
imagematch($truecolor_1, $truecolor_1a) = bool(true)
imagematch($truecolor_1, $truecolor_2) = bool(false)
imagematch($truecolor_1, $truecolor_3) = bool(false)
