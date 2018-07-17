--TEST--
SPL: iterable_to_array() test
--FILE--
<?php

$array = ['a' => 1, 'b' => 2, 'c' => 3];
$iterator = new ArrayIterator($array);
$generator = function () { yield 'a' => 1; yield 'a' => 2; yield 'a' => 3; };

iterable_to_array();
iterable_to_array([], 1, 2);

var_dump(iterable_to_array([]));
var_dump(iterable_to_array($array));
var_dump(iterable_to_array($iterator));
var_dump(iterable_to_array($generator()));

var_dump(iterable_to_array([], false));
var_dump(iterable_to_array($array, false));
var_dump(iterable_to_array($iterator, false));
var_dump(iterable_to_array($generator(), false));

iterable_to_array(new stdClass);

?>
--EXPECTF--
Warning: iterable_to_array() expects at least 1 parameter, 0 given in %s

Warning: iterable_to_array() expects at most 2 parameters, 3 given in %s
array(0) {
}
array(3) {
  ["a"]=>
  int(1)
  ["b"]=>
  int(2)
  ["c"]=>
  int(3)
}
array(3) {
  ["a"]=>
  int(1)
  ["b"]=>
  int(2)
  ["c"]=>
  int(3)
}
array(1) {
  ["a"]=>
  int(3)
}
array(0) {
}
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}

Fatal error: Uncaught TypeError: Argument 1 passed to iterable_to_array() must be iterable, object given in %s:%d
Stack trace:
#0 %s(%d): iterable_to_array(Object(stdClass))
#1 {main}
  thrown in %s on line %d
