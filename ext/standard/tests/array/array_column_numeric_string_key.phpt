--TEST--
array_column() treats numeric string keys as usual
--FILE--
<?php

$array = [[42 => 'a'], [42 => 'b']];
var_dump(array_column($array, 42));
var_dump(array_column($array, "42"));

?>
--EXPECT--
array(2) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
}
array(2) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
}
