--TEST--
Bug #75961: array_walk() converts items into references
--FILE--
<?php

$arr = [[1]];

array_walk($arr, function(){});
array_map('array_shift', $arr);
var_dump($arr);

?>
--EXPECT--
array(1) {
  [0]=>
  array(1) {
    [0]=>
    int(1)
  }
}
