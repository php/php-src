--TEST--
Test array_merge() function : usage variations - multi-dimensional arrays
--FILE--
<?php
/* Prototype  : array array_merge(array $arr1, array $arr2 [, array $...])
 * Description: Merges elements from passed arrays into one array
 * Source code: ext/standard/array.c
 */

/*
 * Test array_merge() with multi-dimensional arrays
 */

echo "*** Testing array_merge() : usage variations ***\n";

$arr1 = array('zero', 'one', 'two', array(0));
$arr2 = array(1, 2, 3);

echo "\n-- Merge a two-dimensional and a one-dimensional array --\n";
var_dump(array_merge($arr1, $arr2));

echo "\n-- Merge an array and a sub-array --\n";
var_dump(array_merge($arr1[3], $arr2));
var_dump(array_merge($arr2, $arr1[3]));

echo "Done";
?>
--EXPECT--
*** Testing array_merge() : usage variations ***

-- Merge a two-dimensional and a one-dimensional array --
array(7) {
  [0]=>
  string(4) "zero"
  [1]=>
  string(3) "one"
  [2]=>
  string(3) "two"
  [3]=>
  array(1) {
    [0]=>
    int(0)
  }
  [4]=>
  int(1)
  [5]=>
  int(2)
  [6]=>
  int(3)
}

-- Merge an array and a sub-array --
array(4) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
  [3]=>
  int(3)
}
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(0)
}
Done
