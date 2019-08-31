--TEST--
ziparchive::addFile() function
--SKIPIF--
<?php
if(!extension_loaded('zip')) die('skip');
?>
--FILE--
<?php

$dirname = __DIR__ . '/';
include $dirname . 'utils.inc';
$file = $dirname . 'oo_addfile.zip';

copy($dirname . 'test.zip', $file);

$zip = new ZipArchive;
if (!$zip->open($file)) {
	exit('failed');
}
if (!$zip->addFile($dirname . 'utils.inc', 'test.php')) {
	echo "failed\n";
}
if ($zip->status == ZIPARCHIVE::ER_OK) {
	if (!verify_entries($zip, [
		"bar",
		"foobar/",
		"foobar/baz",
		"entry1.txt",
		"test.php"
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
OK
