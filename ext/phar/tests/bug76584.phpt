--TEST--
Bug #76584 (PharFileInfo::decompress not working)
--SKIPIF--
<?php
if (!extension_loaded('phar')) die('skip phar extension not available');
if (!extension_loaded('zlib')) die('skip zlib extension not available');
?>
--INI--
phar.readonly=0
--FILE--
<?php
$phar = new Phar(__DIR__ . '/76584.phar');
$phar->addFromString('76584.txt', 'This is a test file.');
$file = $phar['76584.txt'];
var_dump($file->compress(Phar::GZ));
var_dump($file->isCompressed());
var_dump($file->decompress());
var_dump($file->isCompressed());
mkdir(__DIR__ . '/76584');
var_dump($phar->extractTo(__DIR__ . '/76584'));
echo file_get_contents(__DIR__ . '/76584/76584.txt');
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(false)
bool(true)
This is a test file.
--CLEAN--
<?php
unlink(__DIR__ . '/76584/76584.txt');
rmdir(__DIR__ . '/76584');
unlink(__DIR__ . '/76584.phar');
?>
