--TEST--
Argument unpacking with internal functions
--FILE--
<?php

$arrays = [
    [1, 2, 3],
    [4, 5, 6],
    [7, 8, 9],
];
var_dump(array_map(null, ...$arrays));

?>
--EXPECT--
array(3) {
  [0]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(4)
    [2]=>
    int(7)
  }
  [1]=>
  array(3) {
    [0]=>
    int(2)
    [1]=>
    int(5)
    [2]=>
    int(8)
  }
  [2]=>
  array(3) {
    [0]=>
    int(3)
    [1]=>
    int(6)
    [2]=>
    int(9)
  }
}
