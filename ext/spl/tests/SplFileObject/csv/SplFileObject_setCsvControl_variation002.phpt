--TEST--
SplFileObject::setCsvControl() and ::getCsvControl() with empty $escape
--FILE--
<?php
$file = new SplTempFileObject;
$file->setCsvControl(',', '"', '');
var_dump($file->getCsvControl());
?>
--EXPECTF--
Deprecated: Method SplFileObject::setCsvControl() is deprecated since 8.6 in %s on line %d

Deprecated: Method SplFileObject::getCsvControl() is deprecated since 8.6 in %s on line %d
array(3) {
  [0]=>
  string(1) ","
  [1]=>
  string(1) """
  [2]=>
  string(0) ""
}
