--TEST--
SplFileObject::getCsvControl function - basic test 
--FILE--
<?php
$obj = New SplFileObject(dirname(__FILE__).'/SplFileObject_testinput.csv');
var_dump($obj->getCsvControl());

?>
--EXPECTF--
array(2) {
  [0]=>
  %unicode|string%(1) ","
  [1]=>
  %unicode|string%(1) """
}
