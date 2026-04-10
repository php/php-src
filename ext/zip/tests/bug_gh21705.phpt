--TEST--
ZipArchive::getFromIndex() honors FL_UNCHANGED for deleted entries
--EXTENSIONS--
zip
--FILE--
<?php
$name = __DIR__ . '/test.zip';
@unlink($name);
$zip = new ZipArchive;
$zip->open($name, ZipArchive::CREATE);
$zip->addFromString('foo.txt', 'foo');
$zip->addFromString('bar.txt', 'bar');
$zip->close();

$zip = new ZipArchive;
$zip->open($name);

$index = $zip->locateName('bar.txt');
$zip->deleteName('bar.txt');

var_dump($zip->getFromName('bar.txt', 0, ZipArchive::FL_UNCHANGED));
var_dump($zip->getFromIndex($index, 0, ZipArchive::FL_UNCHANGED));

$zip->close();
@unlink($name);
?>
--EXPECT--
string(3) "bar"
string(3) "bar"
