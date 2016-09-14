--TEST--
Bug #50194 (imagettftext broken on transparent background w/o alphablending)
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip gd extension not available');
if (!function_exists('imagettftext')) die('skip imagettftext() not available');
?>
--FILE--
<?php
require_once __DIR__ . DIRECTORY_SEPARATOR . 'func.inc';

$im = imagecreatetruecolor(240, 55);
$background = imagecolorallocatealpha($im, 60, 60, 60, 0); // no tranparency
$black = imagecolorallocate($im, 0, 0, 0);
imagealphablending($im, false); 
imagefilledrectangle($im, 0, 0, 239, 54, $background);
$text = 'Testing ... ';
$font = __DIR__ . DIRECTORY_SEPARATOR . 'Tuffy.ttf';
imagettftext($im, 40, 0, 10, 40, $black, $font, $text);
imagesavealpha($im, true);

test_image_equals_file(__DIR__ . DIRECTORY_SEPARATOR . 'bug50194.png', $im);

imagedestroy($im);
?>
===DONE===
--EXPECT--
The images are equal.
===DONE===
