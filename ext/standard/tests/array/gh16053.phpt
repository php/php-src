--TEST--
GH-16053: Assertion failure in Zend/zend_hash.c
--FILE--
<?php

class test
{
}
$x = new test;
$arr1 = array("string" => $x);
$arr2 = array("string" => "hello");
var_dump($arr1);
var_dump(array_merge_recursive($arr1, $arr2));

?>
--EXPECTF--
array(1) {
  ["string"]=>
  object(test)#%d (0) {
  }
}
array(1) {
  ["string"]=>
  array(1) {
    [0]=>
    string(5) "hello"
  }
}
