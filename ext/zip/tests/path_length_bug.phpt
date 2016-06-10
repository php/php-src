--TEST--
ZipArchive::addGlob() method
--SKIPIF--
<?php
/* $Id$ */
if(!extension_loaded('zip')) die('skip');
if(!defined("GLOB_BRACE")) die ('skip');
?>
--FILE--
<?php
$dirname = dirname(__FILE__) . '/';
include $dirname . 'utils.inc';
$file = $dirname . '__tmp_oo_addglob.zip';
$subdir = $dirname . 'foo/';

copy($dirname . 'test.zip', $file);
mkdir($subdir);
touch($subdir . 'baz');

$zip = new ZipArchive();
if (!$zip->open($file)) {
	exit('failed');
}
$options = array('add_path' => 'prefix/', 'remove_path' => $subdir);
if (!$zip->addGlob($subdir . 'baz', 0, $options)) {
	echo "failed1\n";
}
if ($zip->status == ZIPARCHIVE::ER_OK) {
	dump_entries_name($zip);
	$zip->close();
} else {
	echo "failed2\n";
}
?>
--CLEAN--
<?php
$dirname = dirname(__FILE__) . '/';
$subdir = $dirname . 'foo/';
unlink($dirname . '__tmp_oo_addglob.zip');
unlink($subdir . 'baz');
rmdir($subdir);
?>
--EXPECTF--
0 bar
1 foobar/
2 foobar/baz
3 entry1.txt
4 prefix/baz
