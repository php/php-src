--TEST--
SplFileObject::fscanf function - basic functionality test
--FILE--
<?php
$obj = New SplFileObject(dirname(__FILE__).'/SplFileObject_testinput.csv');
var_dump($obj->fscanf('%s'));
?>
--EXPECT--
array(1) {
  [0]=>
  string(18) "first,second,third"
}
