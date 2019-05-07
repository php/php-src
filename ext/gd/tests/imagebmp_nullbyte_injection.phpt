--TEST--
Testing null byte injection in imagebmp
--SKIPIF--
<?php
if(!extension_loaded('gd')) die('skip gd extension not available');
if (!gd_info()['BMP Support']) die('skip BMP support not available');
?>
--FILE--
<?php
$image = imagecreate(1,1);// 1px image
var_dump(imagebmp($image, "./foo\0bar"));
?>
===DONE===
--EXPECTF--
Warning: imagebmp(): Invalid 2nd parameter, filename must not contain null bytes in %s on line %d
bool(false)
===DONE===
