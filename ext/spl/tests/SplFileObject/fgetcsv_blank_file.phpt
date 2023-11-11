--TEST--
SplFileObject: fgetcsv() on a blank line
--FILE--
<?php

$file_path = __DIR__ . '/SplFileObject_fgetcsv_empty.csv';
$file = new SplFileObject($file_path, 'w');
$file = new SplTempFileObject();

// write to file
$file->fwrite("");

// read from file
$file->rewind();
var_dump($file->fgetcsv());

$file->setFlags(SplFileObject::SKIP_EMPTY);
$file->rewind();
var_dump($file->fgetcsv());
?>
--CLEAN--
<?php
$file_path = __DIR__ . '/SplFileObject_fgetcsv_empty.csv';
unlink($file_path);
?>
--EXPECT--
array(1) {
  [0]=>
  NULL
}
bool(false)
