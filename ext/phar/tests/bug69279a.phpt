--TEST--
Bug #69279 (Compressed ZIP Phar extractTo() creates garbage files)
--SKIPIF--
<?php
if (!extension_loaded('phar')) die('skip phar extension not available');
if (!extension_loaded('zlib')) die('skip zlib extension not available');
?>
--FILE--
<?php
$phar = new PharData(__DIR__ . '/bug69279a.zip');
mkdir(__DIR__ . '/bug69279a');
var_dump($phar->extractTo(__DIR__ . '/bug69279a', null, true));
var_dump(strncmp(file_get_contents(__DIR__ . '/bug69279a/1.txt'), 'Lorem ipsum', 11));
var_dump(strncmp(file_get_contents(__DIR__ . '/bug69279a/2.txt'), 'foo', 3));
var_dump(strncmp(file_get_contents(__DIR__ . '/bug69279a/3.txt'), 'Lorem ipsum', 11));
?>
--EXPECT--
bool(true)
int(0)
int(0)
int(0)
--CLEAN--
<?php
@unlink(__DIR__ . '/bug69279a/1.txt');
@unlink(__DIR__ . '/bug69279a/2.txt');
@unlink(__DIR__ . '/bug69279a/3.txt');
@rmdir(__DIR__ . '/bug69279a');
