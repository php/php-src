--TEST--
imagewebp() and imagecreatefromwebp() - basic test
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip gd extension not available');
if (!function_exists('imagewebp') || !function_exists('imagecreatefromwebp'))
    die('skip WebP support not available');
?>
--FILE--
<?php
require_once __DIR__ . '/similarity.inc';

$filename = __DIR__ . '/webp_basic.webp';

$im1 = imagecreatetruecolor(75, 75);
$white = imagecolorallocate($im1, 255, 255, 255);
$red = imagecolorallocate($im1, 255, 0, 0);
$green = imagecolorallocate($im1, 0, 255, 0);
$blue = imagecolorallocate($im1, 0, 0, 255);
imagefilledrectangle($im1, 0, 0, 74, 74, $white);
imageline($im1, 3, 3, 71, 71, $red);
imageellipse($im1, 18, 54, 36, 36, $green);
imagerectangle($im1, 41, 3, 71, 33, $blue);
imagewebp($im1, $filename);

$im2 = imagecreatefromwebp($filename);
imagewebp($im2, $filename);
var_dump(calc_image_dissimilarity($im1, $im2) < 10e5);
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/webp_basic.webp');
?>
--EXPECT--
bool(true)
