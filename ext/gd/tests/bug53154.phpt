--TEST--
Bug #53154 (Zero-height rectangle has whiskers)
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip gd extension not available');
?>
--FILE--
<?php
$im = imagecreatetruecolor(100, 10);
$red = imagecolorallocate($im, 255, 0, 0);
imagerectangle($im, 5, 5, 95, 5, $red);
var_dump(imagecolorat($im, 5, 4) !== $red);
var_dump(imagecolorat($im, 5, 6) !== $red);
var_dump(imagecolorat($im, 95, 4) !== $red);
var_dump(imagecolorat($im, 95, 6) !== $red);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
