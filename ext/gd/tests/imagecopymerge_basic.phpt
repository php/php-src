--TEST--
Testing imagecopymerge() of GD library
--CREDITS--
Cleston Viel Vieira de Sousa <cleston [dot] vs [at] gmail [dot] com>
#testfest PHPSP on 2009-06-20

Updated by Sean Fraaser <frasmage [at] gmail [dot] com>
PHP[tek] 2017
--SKIPIF--
<?php
if (!extension_loaded("gd")) die("skip GD not present");
?>
--FILE--
<?php

$des = imagecreate(120, 120);
$src = imagecreate(100, 100);

$color_des = imagecolorallocate($des, 50, 50, 200);
$color_src = imagecolorallocate($src, 255, 255, 255);

imagefill($des, 0, 0, $color_des);
imagefill($src, 0, 0, $color_src);

var_dump(imagecopymerge($des, $src, 20, 20, 0, 0, 50, 50, 75));

$color = imagecolorat($des, 30, 30);
$rgb = imagecolorsforindex($des, $color);
echo $rgb['red'], ", ", $rgb['green'], ", ", $rgb['blue'], "\n";

?>
--EXPECT--
bool(true)
203, 203, 241
