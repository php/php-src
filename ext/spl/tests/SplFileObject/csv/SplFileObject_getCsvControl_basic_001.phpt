--TEST--
SplFileObject::getCsvControl function - basic test
--FILE--
<?php
$obj = New SplFileObject(__DIR__.'/SplFileObject_testinput.csv');
var_dump($obj->getCsvControl());

?>
--EXPECTF--
Deprecated: Method SplFileObject::getCsvControl() is deprecated since 8.6 in %s on line %d
array(3) {
  [0]=>
  string(1) ","
  [1]=>
  string(1) """
  [2]=>
  string(1) "\"
}
