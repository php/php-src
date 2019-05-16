--TEST--
antialiased imagepolygon()
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip gd extension not available');
?>
--FILE--
<?php
require_once __DIR__ . DIRECTORY_SEPARATOR . 'func.inc';

$im = imagecreatetruecolor(100, 100);
$white = imagecolorallocate($im, 255, 255, 255);
$black = imagecolorallocate($im, 0, 0, 0);
imagefilledrectangle($im, 0,0, 99,99, $white);
imageantialias($im, true);

imagepolygon($im, [10,10, 49,89, 89,49], 3, $black);

test_image_equals_file(__DIR__ . DIRECTORY_SEPARATOR . 'imagepolygon_aa.png', $im);
?>
===DONE===
--EXPECT--
The images are equal.
===DONE===
