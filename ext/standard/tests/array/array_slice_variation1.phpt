--TEST--
Test array_slice() - Third parameter (NULL vs 0)
--FILE--
<?php

var_dump(array_slice(range(1, 3), 0, NULL, true));
var_dump(array_slice(range(1, 3), 0, 0, true));
var_dump(array_slice(range(1, 3), 0, NULL));
var_dump(array_slice(range(1, 3), 0, 0));

var_dump(array_slice(range(1, 3), -1, 0));
var_dump(array_slice(range(1, 3), -1, 0, true));
var_dump(array_slice(range(1, 3), -1, NULL));
var_dump(array_slice(range(1, 3), -1, NULL, true));

?>
--EXPECT--
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
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
array(0) {
}
array(0) {
}
array(0) {
}
array(1) {
  [0]=>
  int(3)
}
array(1) {
  [2]=>
  int(3)
}
