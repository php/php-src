--TEST--
GH-16957 (Assertion failure in array_shift with self-referencing array)
--FILE--
<?php
$new_array = array(&$new_array, 1, 'two');
var_dump($shifted = array_shift($new_array));
var_dump($new_array);
var_dump($new_array === $shifted);

$new_array2 = array(&$new_array2, 2 => 1, 300 => 'two');
var_dump($shifted = array_shift($new_array2));
var_dump($new_array2);
var_dump($new_array2 === $shifted);
?>
--EXPECT--
array(2) {
  [0]=>
  int(1)
  [1]=>
  string(3) "two"
}
array(2) {
  [0]=>
  int(1)
  [1]=>
  string(3) "two"
}
bool(true)
array(2) {
  [0]=>
  int(1)
  [1]=>
  string(3) "two"
}
array(2) {
  [0]=>
  int(1)
  [1]=>
  string(3) "two"
}
bool(true)
