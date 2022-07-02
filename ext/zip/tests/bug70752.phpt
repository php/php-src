--TEST--
Bug #70752 (Depacking with wrong password leaves 0 length files)
--EXTENSIONS--
zip
--FILE--
<?php
$filename = __DIR__ . DIRECTORY_SEPARATOR . 'bug70752.zip';
$zip = new ZipArchive();
$zip->open($filename);

$filename =  __DIR__ . DIRECTORY_SEPARATOR . 'bug70752.txt';
var_dump(file_exists($filename));

$zip->setPassword('bar'); // correct password would be 'foo'
$zip->extractTo(__DIR__);
$zip->close();

var_dump(file_exists($filename));
?>
--EXPECT--
bool(false)
bool(false)
