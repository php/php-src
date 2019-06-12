--TEST--
Testing null byte injection in imagegd
--SKIPIF--
<?php
        if(!extension_loaded('gd')){ die('skip gd extension not available'); }
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

echo "\nimagegd TEST\n";
imagegd($image, $temp);
var_dump(file_exists($tempdir. "/test1"));
var_dump(file_exists($tempdir. "/test1.tmp"));
?>
--EXPECTF--
imagegd TEST

Warning: imagegd() expects parameter 2 to be a valid path, string given in %s on line %d
bool(false)
bool(false)
