--TEST--
SPL: iterator_to_array() supports arrays.
--FILE--
<?php

var_dump(iterator_to_array([]));
var_dump(iterator_to_array([1]));
var_dump(iterator_to_array(['a' => 1, 'b' => 2, 5 => 3]));
var_dump(iterator_to_array([], false));
var_dump(iterator_to_array([1], false));
var_dump(iterator_to_array(['a' => 1, 'b' => 2, 5 => 3], false));

?>
--EXPECT--
array(0) {
}
array(1) {
  [0]=>
  int(1)
}
array(3) {
  ["a"]=>
  int(1)
  ["b"]=>
  int(2)
  [5]=>
  int(3)
}
array(0) {
}
array(1) {
  [0]=>
  int(1)
}
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
