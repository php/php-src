--TEST--
Bug #64342 ZipArchive::addFile() has to check file existence (variation 2)
--SKIPIF--
<?php
if(!extension_loaded('zip')) die('skip');
?>
--FILE--
<?php

$dirname = __DIR__ . '/';
include $dirname . 'utils.inc';
$file = $dirname . 'bug64342_1.zip';

copy($dirname . 'test.zip', $file);

$zip = new ZipArchive;
if (!$zip->open($file)) {
	exit('failed');
}
if (!$zip->addFile($dirname . 'cant_find_me.txt', 'test.php')) {
	echo "failed\n";
}
if ($zip->status == ZIPARCHIVE::ER_OK) {
	if (!verify_entries($zip, [
		"bar",
		"foobar/",
		"foobar/baz",
		"entry1.txt"
	])) {
		echo "failed\n";
	} else {
		echo "OK";
	}
	$zip->close();
} else {
	echo "failed\n";
}
@unlink($file);
?>
--EXPECT--
failed
OK
