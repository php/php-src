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
--EXPECTF--
Deprecated: Method SplFileObject::setCsvControl() is deprecated since 8.6 in %s on line %d

Deprecated: Method SplFileObject::fgetcsv() is deprecated since 8.6 in %s on line %d
bool(false)

Deprecated: Method SplFileObject::fgetcsv() is deprecated since 8.6 in %s on line %d
bool(false)
