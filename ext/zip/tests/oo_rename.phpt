--TEST--
Rename entries
--SKIPIF--
<?php
if(!extension_loaded('zip')) die('skip');
?>
--FILE--
<?php
$dirname = __DIR__ . '/';
include $dirname . 'utils.inc';
$file = $dirname . 'oo_rename.zip';

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
	echo "failed2\n";
}

$zip->close();

if (!$zip->open($file)) {
	exit('failed3');
}

if (!verify_entries($zip, [
	"entry1.txt",
    "entry2.txt",
    "dir/entry2.txt"
])) {
	exit("failed4");
} else {
	echo "OK\n";
}

if (!$zip->renameIndex(0, 'ren_entry1.txt')) {
	echo "failed index 0\n";
}

if (!$zip->renameName('dir/entry2.txt', 'dir3/ren_entry2.txt')) {
	echo "failed name dir/entry2.txt\n";
}

if (!verify_entries($zip, [
	"ren_entry1.txt",
    "entry2.txt",
    "dir3/ren_entry2.txt"
])) {
	exit("failed5");
} else {
	echo "OK\n";
}
$zip->close();

@unlink($file);
?>
--EXPECT--
OK
OK
