--TEST--
imagefilledpolygon() with a negative num of points
--SKIPIF--
<?php 
	if (!function_exists('imagefilledpolygon')) die('skip imagefilledpolygon() not available'); 
?>
--FILE--
<?php
$im = imagecreate(100, 100);
$black = imagecolorallocate($im, 0, 0, 0);
if (imagefilledpolygon($im, array(0, 0, 0, 0, 0, 0), -1, $black)) echo "should be false";
imagedestroy($im);
?>
--EXPECTF--
Warning: imagefilledpolygon(): You must give a positive number of points in %s on line %d
