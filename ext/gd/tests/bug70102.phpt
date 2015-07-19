--TEST--
Bug #70102 (imagecreatefromwebm() shifts colors)
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip gd extension not available');
if (!function_exists('imagewebp') || !function_exists('imagecreatefromwebp'))
    die('skip WebP support not available');
?>
--FILE--
<?php
$filename = __DIR__ .  '/bug70102.webp';

$im = imagecreatetruecolor(8, 8);
$white = imagecolorallocate($im, 255, 255, 255);
var_dump($white & 0xffffff);
imagefilledrectangle($im, 0, 0, 7, 7, $white);
imagewebp($im, $filename);
imagedestroy($im);

$im = imagecreatefromwebp($filename);
$color = imagecolorat($im, 4, 4);
var_dump($color & 0xffffff);
?>
--CLEAN--
<?php
unlink(__DIR__ . '/bug70102.webp');
?>
--EXPECT--
int(16777215)
int(16777215)
