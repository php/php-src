--TEST--
ZipArchive::getFromIndex() honors FL_UNCHANGED for deleted entries
--EXTENSIONS--
zip
--FILE--
<?php
$name = __DIR__ . '/ZipArchive_getFromIndex_unchanged_deleted.zip';

$zip = new ZipArchive;
var_dump($zip->open($name, ZipArchive::CREATE));
var_dump($zip->addFromString('foo.txt', 'foo'));
var_dump($zip->addFromString('bar.txt', 'bar'));
var_dump($zip->close());

$zip = new ZipArchive;
var_dump($zip->open($name));
$index = $zip->locateName('bar.txt');
var_dump($index);
var_dump($zip->deleteName('bar.txt'));

var_dump($zip->getFromName('bar.txt', 0, ZipArchive::FL_UNCHANGED));
var_dump($zip->getFromIndex($index, 0, ZipArchive::FL_UNCHANGED));

var_dump($zip->close());
?>
--CLEAN--
<?php
$name = __DIR__ . '/ZipArchive_getFromIndex_unchanged_deleted.zip';
@unlink($name);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
int(1)
bool(true)
string(3) "bar"
string(3) "bar"
bool(true)
