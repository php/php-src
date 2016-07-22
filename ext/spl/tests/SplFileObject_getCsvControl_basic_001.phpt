--TEST--
SplFileObject::getCsvControl function - basic test 
--FILE--
<?php
$obj = New SplFileObject(dirname(__FILE__).'/SplFileObject_testinput.csv');
var_dump($obj->getCsvControl());

?>
--EXPECT--
array(3) {
  [0]=>
  string(1) ","
  [1]=>
  string(1) """
  [2]=>
  string(1) "\"
}
