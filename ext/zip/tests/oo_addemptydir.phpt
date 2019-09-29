--TEST--
ziparchive::addEmptyDir
--SKIPIF--
<?php
if(!extension_loaded('zip')) die('skip');
?>
--FILE--
<?php

$dirname = __DIR__ . '/';
include $dirname . 'utils.inc';
$file = $dirname . 'oo_addemptydir.zip';

copy($dirname . 'test.zip', $file);

$zip = new ZipArchive;
if (!$zip->open($file)) {
	exit('failed');
}

$zip->addEmptyDir('emptydir');
if ($zip->status == ZIPARCHIVE::ER_OK) {
	if (!verify_entries($zip, [
		"bar",
		"foobar/",
		"foobar/baz",
		"entry1.txt",
		"emptydir/"
	])) {
		echo "failed\n";
	} else {
		echo "OK";
	}
	$zip->close();
} else {
	echo "failed3\n";
}
@unlink($file);
?>
--EXPECT--
OK
