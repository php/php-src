--TEST--
Testing wrong parameter resource of SMOOTH in imagefilter() of GD library
--CREDITS--
Guilherme Blanco <guilhermeblanco [at] hotmail [dot] com>
#testfest PHPSP on 2009-06-20
--SKIPIF--
<?php 
if (!extension_loaded("gd")) die("skip GD not present");
?>
--FILE--
<?php
$image = tmpfile();

var_dump(imagefilter($image, IMG_FILTER_SMOOTH, 3.0));
?>
--EXPECTF--
Warning: imagefilter(): supplied resource is not a valid Image resource in %s on line %d
bool(false)
