--TEST--
Cloning stdClass
--FILE--
<?php

$x[] = clone new stdclass;
$x[] = clone new stdclass;
$x[] = clone new stdclass;

$x[0]->a = 1;

var_dump($x);

?>
--EXPECT--
array(3) {
  [0]=>
  object(DynamicObject)#2 (1) {
    ["a"]=>
    int(1)
  }
  [1]=>
  object(DynamicObject)#3 (0) {
  }
  [2]=>
  object(DynamicObject)#4 (0) {
  }
}
