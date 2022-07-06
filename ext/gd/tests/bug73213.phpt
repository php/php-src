--TEST--
Bug #73213 (Integer overflow in imageline() with antialiasing)
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip gd extension not available');
?>
--FILE--
<?php
require_once __DIR__ . DIRECTORY_SEPARATOR . 'func.inc';

$im = imagecreatetruecolor(32768, 1);
$black = imagecolorallocate($im, 0, 0, 0);
imageantialias($im, true);

imageline($im, 0,0, 32767,0, $black);

test_image_equals_file(__DIR__ . DIRECTORY_SEPARATOR . 'bug73213.png', $im);
?>
--EXPECT--
The images are equal.
