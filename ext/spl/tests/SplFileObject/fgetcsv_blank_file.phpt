--TEST--
SplFileObject: fgetcsv() on a blank line
--FILE--
<?php

$file = new SplTempFileObject();
$file->setCsvControl(escape: "");

// write to file
$file->fwrite("");

// read from file
$file->rewind();
var_dump($file->fgetcsv());

$file->setFlags(SplFileObject::SKIP_EMPTY);
$file->rewind();
var_dump($file->fgetcsv());
?>
--EXPECT--
array(1) {
  [0]=>
  NULL
}
bool(false)
