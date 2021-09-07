--TEST--
Testing assign to property of an object in an array
--FILE--
<?php

$arr = array(new stdClass);

$arr[0]->a = clone $arr[0];
var_dump($arr);

$arr[0]->b = new $arr[0];
var_dump($arr);

$arr[0]->c = $arr[0]->a;
var_dump($arr);

?>
--EXPECT--
array(1) {
  [0]=>
  object(DynamicObject)#1 (1) {
    ["a"]=>
    object(DynamicObject)#2 (0) {
    }
  }
}
array(1) {
  [0]=>
  object(DynamicObject)#1 (2) {
    ["a"]=>
    object(DynamicObject)#2 (0) {
    }
    ["b"]=>
    object(DynamicObject)#3 (0) {
    }
  }
}
array(1) {
  [0]=>
  object(DynamicObject)#1 (3) {
    ["a"]=>
    object(DynamicObject)#2 (0) {
    }
    ["b"]=>
    object(DynamicObject)#3 (0) {
    }
    ["c"]=>
    object(DynamicObject)#2 (0) {
    }
  }
}
