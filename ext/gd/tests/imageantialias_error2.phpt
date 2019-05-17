--TEST--
Testing wrong parameter passing in imageantialias() of GD library
--CREDITS--
Guilherme Blanco <guilhermeblanco [at] hotmail [dot] com>
#testfest PHPSP on 2009-06-20
--SKIPIF--
<?php
if (!extension_loaded("gd")) die("skip GD not present");
?>
--FILE--
<?php
/*

It seems the second argument passing is not being correclty checked.
This test is failing due to this wrogn check

*/
$image = imagecreatetruecolor(180, 30);

var_dump(imageantialias($image, 'wrong param')); // 'wrogn param' is converted to true
?>
--EXPECT--
bool(true)
