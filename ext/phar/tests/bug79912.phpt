--TEST--
Bug #79912 (Phar::decompressFiles not working)
--SKIPIF--
<?php
if (!extension_loaded('phar')) die('skip phar extension is not available');
if (!extension_loaded('zlib')) die('skip zlib extension not available');
?>
--INI--
phar.readonly=0
--FILE--
<?php
$phar = new Phar(__DIR__ . "/bug79912.phar");
$phar->addFromString("test.txt", "This is a test file.This is a test file.This is a test file.");
$file = $phar["test.txt"];
var_dump($file->compress(Phar::GZ)); //true (success)
var_dump($file->getContent());
var_dump($file->isCompressed()); //true (the file is compressed)
var_dump($phar->decompressFiles()); //true (success)
var_dump($file->isCompressed()); //false (the file should not be compressed anymore)
var_dump($phar->extractTo(__DIR__ . "/bug79912")); //true
var_dump(file_get_contents(__DIR__ . "/bug79912/test.txt")); //the extracted file in the folder should be decompressed
?>
--EXPECT--
bool(true)
string(60) "This is a test file.This is a test file.This is a test file."
bool(true)
bool(true)
bool(false)
bool(true)
string(60) "This is a test file.This is a test file.This is a test file."
--CLEAN--
<?php
@unlink(__DIR__ . "/bug79912/test.txt");
@rmdir(__DIR__ . "/bug79912");
@unlink(__DIR__ . "/bug79912.phar");
?>
