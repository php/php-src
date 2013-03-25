--TEST--
Testing wrong parameter passing of SMOOTH in imagefilter() of GD library
--CREDITS--
Guilherme Blanco <guilhermeblanco [at] hotmail [dot] com>
#testfest PHPSP on 2009-06-20
--SKIPIF--
<?php 
if (!extension_loaded("gd")) die("skip GD not present");
?>
--FILE--
<?php
$image = imagecreatetruecolor(180, 30);

var_dump(imagefilter($image, IMG_FILTER_SMOOTH, 'wrong parameter'));
?>
--EXPECTF--
Warning: imagefilter() expects parameter 3 to be double, %unicode_string_optional% given in %s on line %d
bool(false)
