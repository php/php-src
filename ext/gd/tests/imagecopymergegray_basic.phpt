--TEST--
Testing imagecopymergegray() of GD library
--CREDITS--
Sean Fraser <frasmage [at] gmail [dot] com>
PHP[tek] 2017
--EXTENSIONS--
gd
--FILE--
<?php

$des = imagecreate(120, 120);
$src = imagecreate(100, 100);

$color_des = imagecolorallocate($des, 50, 50, 200);
$color_src = imagecolorallocate($src, 255, 255, 0);

imagefill($des, 0, 0, $color_des);
imagefill($src, 0, 0, $color_src);

var_dump(imagecopymergegray($des, $src, 20, 20, 0, 0, 50, 50, 75));

$color = imagecolorat($des, 30, 30);
$rgb = imagecolorsforindex($des, $color);
echo $rgb['red'], ", ", $rgb['green'], ", ", $rgb['blue'], "\n";

?>
--EXPECT--
bool(true)
208, 208, 16
