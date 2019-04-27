--TEST--
Testing null byte injection in imagegif
--CLEAN--
$tempdir = sys_get_temp_dir(). '/php-gdtest';
foreach (glob($tempdir . "/test*") as $file ) { unlink($file); }
rmdir($tempdir);
--SKIPIF--
<?php
if(!extension_loaded('gd')){ die('skip gd extension not available'); }
$support = gd_info();
if (!isset($support['GIF Create Support']) || $support['GIF Create Support'] === false) {
	print 'skip gif support not available';
}
?>
--FILE--
<?php
$image = imagecreate(1,1);// 1px image


$tempdir = sys_get_temp_dir(). '/php-gdtest';
if (!file_exists($tempdir) && !is_dir($tempdir)) {
	mkdir ($tempdir, 0777, true);
}

$userinput = "1\0"; // from post or get data
$temp = $tempdir. "/test" . $userinput .".tmp";

echo "\nimagegif TEST\n";
imagegif($image, $temp);
var_dump(file_exists($tempdir. "/test1"));
var_dump(file_exists($tempdir. "/test1.tmp"));
foreach (glob($tempdir . "/test*") as $file ) { unlink($file); }
--EXPECTF--
imagegif TEST

Warning: imagegif(): Invalid 2nd parameter, filename must not contain null bytes in %s on line %d
bool(false)
bool(false)
