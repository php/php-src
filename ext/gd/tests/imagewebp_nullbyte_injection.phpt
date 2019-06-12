--TEST--
Testing null byte injection in imagewebp
--SKIPIF--
<?php
if(!extension_loaded('gd')){ die('skip gd extension not available'); }
$support = gd_info();
if (!isset($support['WebP Support']) || $support['WebP Support'] === false) {
	print 'skip webp support not available';
}
?>
--CLEAN--
$tempdir = sprintf("%s/%s", sys_get_temp_dir(), preg_replace("~\.php$~", null, __FILE__));
foreach (glob($tempdir . "/test*") as $file ) { unlink($file); }
rmdir($tempdir);
--FILE--
<?php
$image = imagecreate(1,1);// 1px image
var_dump(imagewebp($image, "./foo\0bar"));
?>
--EXPECTF--
Warning: imagewebp(): Invalid 2nd parameter, filename must not contain null bytes in %s on line %d
bool(false)
