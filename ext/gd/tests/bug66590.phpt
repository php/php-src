--TEST--
Bug #66590 (imagewebp() doesn't pad to even length)
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip gd extension not available');
if (!function_exists('imagewebp')) die('skip WebP support not available');
?>
--FILE--
<?php
$filename = __DIR__ . '/bug66590.webp';
$im = imagecreatetruecolor(75, 75);
$red = imagecolorallocate($im, 255, 0, 0);
imagefilledrectangle($im, 0, 0, 74, 74, $red);
imagewebp($im, $filename);
$stream = fopen($filename, 'rb');
fread($stream, 4); // skip "RIFF"
$length = fread($stream, 4);
fclose($stream);
$length = unpack('V', $length)[1] + 8;
var_dump($length === filesize($filename));
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/bug66590.webp');
?>
--EXPECT--
bool(true)
