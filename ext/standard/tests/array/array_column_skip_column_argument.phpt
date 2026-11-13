--TEST--
parameter $column_key of array_column() is optional
--FILE--
<?php

$array = [['a'], ['b']];
var_dump(array_column($array, index_key: 0));

?>
--EXPECT--
array(2) {
  ["a"]=>
  array(1) {
    [0]=>
    string(1) "a"
  }
  ["b"]=>
  array(1) {
    [0]=>
    string(1) "b"
  }
}
