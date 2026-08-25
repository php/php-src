--TEST--
Bug #46569 (SplFileObject: fgetcsv after seek returns wrong line)
--FILE--
<?php
$file = new SplFileObject(__DIR__ . '/bug46569.csv');
$file->setCsvControl(escape: "");
$file->seek(1);
var_dump($file->fgetcsv());
?>
--EXPECTF--
Deprecated: Method SplFileObject::setCsvControl() is deprecated since 8.6 in %s on line %d

Deprecated: Method SplFileObject::fgetcsv() is deprecated since 8.6 in %s on line %d
array(2) {
  [0]=>
  string(6) "second"
  [1]=>
  string(4) "line"
}
