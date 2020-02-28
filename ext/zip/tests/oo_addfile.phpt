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
if (!$zip->addFile($dirname . 'utils.inc', 'mini.txt', 12, 34)) {
	echo "failed\n";
}
if ($zip->status == ZIPARCHIVE::ER_OK) {
	if (!verify_entries($zip, [
		"bar",
		"foobar/",
		"foobar/baz",
		"entry1.txt",
		"test.php",
		"mini.txt"
	])) {
		echo "failed\n";
	} else {
		echo "OK\n";
	}
	$zip->close();
} else {
	echo "failed\n";
}
if (!$zip->open($file)) {
	exit('failed');
}
var_dump(strlen($zip->getFromName('test.php')) == filesize($dirname . 'utils.inc'));
var_dump(strlen($zip->getFromName('mini.txt')) == 34);
@unlink($file);
?>
--EXPECT--
OK
bool(true)
bool(true)
