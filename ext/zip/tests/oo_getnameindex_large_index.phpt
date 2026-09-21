--TEST--
ZipArchive::getNameIndex() with an index that does not fit in an int
--EXTENSIONS--
zip
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die('skip 64-bit only');
?>
--FILE--
<?php
$file = __DIR__ . '/oo_getnameindex_large_index.zip';

@unlink($file);

$zip = new ZipArchive;
if (!$zip->open($file, ZipArchive::CREATE)) {
    exit('failed');
}

$zip->addFromString('entry1.txt', 'entry #1');
$zip->close();

if (!$zip->open($file)) {
    exit('failed');
}

var_dump($zip->getNameIndex(0));
var_dump($zip->getNameIndex(1 << 32));
var_dump($zip->getNameIndex((1 << 32) + 1));
var_dump($zip->getNameIndex(PHP_INT_MAX));
var_dump($zip->getNameIndex(-1));

$zip->close();
?>
--EXPECT--
string(10) "entry1.txt"
bool(false)
bool(false)
bool(false)
bool(false)
--CLEAN--
<?php
unlink(__DIR__ . '/oo_getnameindex_large_index.zip');
?>
