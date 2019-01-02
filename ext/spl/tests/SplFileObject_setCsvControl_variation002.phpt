--TEST--
SplFileObject::setCsvControl() and ::getCsvControl() with empty $escape
--FILE--
<?php
$file = new SplTempFileObject;
$file->setCsvControl(',', '"', '');
var_dump($file->getCsvControl());
?>
===DONE===
--EXPECT--
array(3) {
  [0]=>
  string(1) ","
  [1]=>
  string(1) """
  [2]=>
  string(0) ""
}
===DONE===
