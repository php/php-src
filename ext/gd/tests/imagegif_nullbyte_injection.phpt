--TEST--
Testing null byte injection in imagegif
--SKIPIF--
<?php
if(!extension_loaded('gd')){ die('skip gd extension not available'); }
?>
--FILE--
<?php
$image = imagecreate(1,1);// 1px image
var_dump(imagegif($image, "./foo\0bar"));
?>
--EXPECTF--
Warning: imagegif(): Invalid 2nd parameter, filename must not contain null bytes in %s on line %d
bool(false)
