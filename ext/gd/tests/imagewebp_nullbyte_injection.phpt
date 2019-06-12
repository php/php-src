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

$tempdir = sprintf("%s/%s", sys_get_temp_dir(), preg_replace("~\.php$~", null, __FILE__));
if (!is_dir($tempdir)) {
	mkdir ($tempdir, 0777, true);
}

$userinput = "1\0"; // from post or get data
$temp = $tempdir. "/test" . $userinput .".tmp";

echo "\nimagewebp TEST\n";
imagewebp($image, $temp);
var_dump(file_exists($tempdir. "/test1"));
var_dump(file_exists($tempdir. "/test1.tmp"));
?>
--EXPECTF--
imagewebp TEST

Warning: imagewebp(): Invalid 2nd parameter, filename must not contain null bytes in %s on line %d
bool(false)
bool(false)
