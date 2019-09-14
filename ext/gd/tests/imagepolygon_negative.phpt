--TEST--
imagepolygon() with a negative num of points
--SKIPIF--
<?php
	if (!function_exists('imagepolygon')) die('skip imagepolygon() not available');
?>
--FILE--
<?php
require __DIR__ . '/func.inc';

$im = imagecreate(100, 100);
$black = imagecolorallocate($im, 0, 0, 0);

trycatch_dump(
    fn() => imagepolygon($im, array(0, 0, 0, 0, 0, 0), -1, $black)
);

imagedestroy($im);
?>
--EXPECT--
!! [ValueError] You must give a positive number of points
