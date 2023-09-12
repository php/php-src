--TEST--
ziparchive::addFile() for dynamic files reported empty
--EXTENSIONS--
zip
--SKIPIF--
<?php
if (!file_exists('/proc/cpuinfo')) die('skip no /proc');
if (!defined('ZipArchive::LENGTH_UNCHECKED')) die('skip libzip to old');
?>
--FILE--
<?php

include __DIR__ . '/utils.inc';
$file = __DIR__ . '/__tmp_oo_addfile_proc.zip';

$zip = new ZipArchive;
if (!$zip->open($file, ZIPARCHIVE::CREATE)) {
	exit('failed');
}
if (!$zip->addFile('/proc/cpuinfo', 'cpu.txt', 0, ZipArchive::LENGTH_UNCHECKED)) {
	echo "failed\n";
}
if (!$zip->addFile('/proc/meminfo', 'mem.txt', 0, ZipArchive::LENGTH_UNCHECKED)) {
	echo "failed\n";
}

if ($zip->status == ZIPARCHIVE::ER_OK) {
	dump_entries_name($zip);
	$zip->close();
} else {
	echo "failed\n";
}
var_dump($zip->status);

if (!$zip->open($file)) {
	exit('failed');
}
var_dump(strlen($zip->getFromName('cpu.txt')) > 0);
var_dump(strlen($zip->getFromName('mem.txt')) > 0);

@unlink($file);
?>
Done
--EXPECTF--
0 cpu.txt
1 mem.txt
int(0)
bool(true)
bool(true)
Done
