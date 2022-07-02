--TEST--
Bug GH-8563: Different results for seek() on SplFileObject and SplTempFileObject
--FILE--
<?php

const LINES = 5;
const INDEX = 10;

$file_01 = new SplFileObject(__DIR__ . '/gh8563.txt', 'w+');
$file_02 = new SplTempFileObject(-1);
$file_03 = new SplTempFileObject(0);
$file_04 = new SplTempFileObject();

// write to files
for ($i = 0; $i < LINES; $i++) {
    $file_01->fwrite("line {$i}" . PHP_EOL);
    $file_02->fwrite("line {$i}" . PHP_EOL);
    $file_03->fwrite("line {$i}" . PHP_EOL);
    $file_04->fwrite("line {$i}" . PHP_EOL);
}

// reset
$file_01->rewind();
$file_02->rewind();
$file_03->rewind();
$file_04->rewind();

// seek
$file_01->seek(INDEX);
$file_02->seek(INDEX);
$file_03->seek(INDEX);
$file_04->seek(INDEX);

// show results
echo 'file_01: ' . $file_01->key(), PHP_EOL;
echo 'file_02: ' . $file_02->key(), PHP_EOL;
echo 'file_03: ' . $file_03->key(), PHP_EOL;
echo 'file_04: ' . $file_04->key(), PHP_EOL;
?>
--CLEAN--
<?php
unlink(__DIR__ . '/gh8563.txt');
?>
--EXPECT--
file_01: 4
file_02: 4
file_03: 4
file_04: 4
