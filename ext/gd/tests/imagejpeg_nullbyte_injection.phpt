--TEST--
Testing null byte injection in imagejpeg
--SKIPIF--
<?php
if(!extension_loaded('gd')){ die('skip gd extension not available'); }
$support = gd_info();
if (!isset($support['JPEG Support']) || $support['JPEG Support'] === false) {
	print 'skip jpeg support not available';
}
?>
--FILE--
<?php
$image = imagecreate(1,1);// 1px image
var_dump(imagejpeg($image, "./foo\0bar"));
?>
--EXPECTF--
Warning: imagejpeg(): Invalid 2nd parameter, filename must not contain null bytes in %s on line %d
bool(false)
