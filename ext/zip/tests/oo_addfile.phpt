--TEST--
ziparchive::addFile() function
--SKIPIF--
<?php
if(!extension_loaded('zip')) die('skip');
?>
--FILE--
<?php

$dirname = dirname(__FILE__) . '/';
include $dirname . 'utils.inc';
$file = $dirname . '__tmp_oo_addfile.zip';

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
	dump_entries_name($zip);
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
0 bar
1 foobar/
2 foobar/baz
3 entry1.txt
4 test.php
5 mini.txt
bool(true)
bool(true)
