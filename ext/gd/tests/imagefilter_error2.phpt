--TEST--
Testing wrong parameter passing in imagefilter() of GD library
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

var_dump(imagefilter($image, 'wrong parameter'));
?>
--EXPECTF--
Warning: imagefilter() expects parameter 2 to be long, %unicode_string_optional% given in %s on line %d
NULL
