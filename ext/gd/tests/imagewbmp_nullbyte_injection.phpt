--TEST--
Testing null byte injection in imagewbmp
--SKIPIF--
<?php
if(!extension_loaded('gd')){ die('skip gd extension not available'); }
$support = gd_info();
if (!isset($support['WBMP Support']) || $support['WBMP Support'] === false) {
	print 'skip wbmp support not available';
}
?>
--FILE--
<?php
$image = imagecreate(1,1);// 1px image
var_dump(imagewbmp($image, "./foo\0bar"));
?>
--EXPECTF--
Warning: imagewbmp(): Invalid 2nd parameter, filename must not contain null bytes in %s on line %d
bool(false)
