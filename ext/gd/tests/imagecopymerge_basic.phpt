--TEST--
Testing imagecopymerge() of GD library
--CREDITS--
Cleston Viel Vieira de Sousa <cleston [dot] vs [at] gmail [dot] com>
#testfest PHPSP on 2009-06-20
--SKIPIF--
<?php 
if (!extension_loaded("gd")) die("skip GD not present");
?>
--FILE--
<?php

$des = imagecreate(120, 120);
$src = imagecreate(100, 100);

imagecolorallocate($des, 50, 50, 200);
$colorTXT_des = imagecolorallocate($des, 255, 255, 255);

imagecolorallocate($src, 255, 255, 255);
$colorTXT_src = imagecolorallocate($src, 0, 255, 255);

imagestring($src, 1, 5, 5,  "A Simple Text", $colorTXT_src);
imagestring($des, 1, 5, 5,  "Another Simple Text", $colorTXT_des);

var_dump(imagecopymerge($des, $src, 20, 20, 0, 0, 50, 50, 75));


?>
--EXPECTF--
bool(true)
