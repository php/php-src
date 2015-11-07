--TEST--
Testing list() assign with non-variable with default value
--FILE--
<?php

list($a[1], $b[3]=99) = [1];
var_dump($a, $b);


$obj = new stdclass;

list($a, $obj->prop1, $obj->prop2='default') = [];
var_dump($a, $obj);

$obj2 = new stdclass;

list($a, list($obj2->prop1, $obj2->prop2='default2')) = [1, [1]];
var_dump($a, $obj2);

?>
--EXPECTF--
array(1) {
  [1]=>
  int(1)
}
array(1) {
  [3]=>
  int(99)
}

Notice: Undefined offset: 0 in %s on line %d

Notice: Undefined offset: 1 in %s on line %d
NULL
object(stdClass)#1 (2) {
  ["prop1"]=>
  NULL
  ["prop2"]=>
  string(7) "default"
}
int(1)
object(stdClass)#2 (2) {
  ["prop1"]=>
  int(1)
  ["prop2"]=>
  string(8) "default2"
}
