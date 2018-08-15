--TEST--
Rename entries
--SKIPIF--
<?php
if(!extension_loaded('zip')) die('skip');
?>
--FILE--
<?php
$dirname = dirname(__FILE__) . '/';
include $dirname . 'utils.inc';
$file = $dirname . '__tmp_oo_rename.zip';

@unlink($file);

$zip = new ZipArchive;
if (!$zip->open($file, ZIPARCHIVE::CREATE)) {
	exit('failed');
}

$zip->addFromString('entry1.txt', 'entry #1');
$zip->addFromString('entry2.txt', 'entry #2');
$zip->addFromString('dir/entry2.txt', 'entry #2');

if (!$zip->status == ZIPARCHIVE::ER_OK) {
	var_dump($zip);
	echo "failed\n";
}

$zip->close();

if (!$zip->open($file)) {
	exit('failed');
}

dump_entries_name($zip);
echo "\n";

if (!$zip->renameIndex(0, 'ren_entry1.txt')) {
	echo "failed index 0\n";
}

if (!$zip->renameName('dir/entry2.txt', 'dir3/ren_entry2.txt')) {
	echo "failed name dir/entry2.txt\n";
}
dump_entries_name($zip);
$zip->close();

@unlink($file);
?>
--EXPECT--
0 entry1.txt
1 entry2.txt
2 dir/entry2.txt

0 ren_entry1.txt
1 entry2.txt
2 dir3/ren_entry2.txt
