--TEST--
ZipArchive::closeString() basic
--EXTENSIONS--
zip
--FILE--
<?php
$zip = new ZipArchive();
$zip->openString();
$zip->addFromString('test1', '1');
$zip->addFromString('test2', '2');
$contents = $zip->closeString();
echo $contents ? "OK\n" : "FAILED\n";

$zip = new ZipArchive();
$zip->openString($contents);
var_dump($zip->getFromName('test1'));
var_dump($zip->getFromName('test2'));
var_dump($zip->getFromName('nonexistent'));

?>
--EXPECT--
OK
string(1) "1"
string(1) "2"
bool(false)
