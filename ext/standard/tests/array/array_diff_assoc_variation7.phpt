--TEST--
Test array_diff_assoc() function : usage variations - arrays containing referenced variables
--FILE--

<?php
/* Prototype  : array array_diff_assoc(array $arr1, array $arr2 [, array ...])
 * Description: Returns the entries of $arr1 that have values which are not 
 * present in any of the others arguments but do additional checks whether the keys are equal 
 * Source code: ext/standard/array.c
 */

/*
 * Tests how array_diff_assoc compares
 * 1. Referenced variables
 * 2. Arrays that have been referenced to each other
 */

echo "*** Testing array_diff_assoc() : usage variations ***\n";

$a = 'a';

$arr1 = array('a', 'b', 'c', $a);
$arr2 = array('a' => 1, 'b' => 2, 'c' => 3, &$a);

echo "-- Results when \$a = $a: --\n";
var_dump(array_diff_assoc($arr1, $arr2));
var_dump(array_diff_assoc($arr2, $arr1));

$a = 4;

echo "-- Results when \$a has been changed to $a: --\n";
var_dump(array_diff_assoc($arr1, $arr2));
var_dump(array_diff_assoc($arr2, $arr1));

$arr2 = &$arr1;

echo "-- Results when \$arr2 is referenced to \$arr1 --\n";
var_dump(array_diff_assoc($arr1, $arr2));
var_dump(array_diff_assoc($arr2, $arr1));

$arr1 = array('zero' => 'x', 'one' => 'y', 'two' => 'z');

echo "-- Results when \$arr1 is changed --\n";
var_dump(array_diff_assoc($arr1, $arr2));
var_dump(array_diff_assoc($arr2, $arr1));

echo "Done";
?>

--EXPECTF--

*** Testing array_diff_assoc() : usage variations ***
-- Results when $a = a: --
array(3) {
  [1]=>
  string(1) "b"
  [2]=>
  string(1) "c"
  [3]=>
  string(1) "a"
}
array(3) {
  ["a"]=>
  int(1)
  ["b"]=>
  int(2)
  ["c"]=>
  int(3)
}
-- Results when $a has been changed to 4: --
array(4) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
  [2]=>
  string(1) "c"
  [3]=>
  string(1) "a"
}
array(4) {
  ["a"]=>
  int(1)
  ["b"]=>
  int(2)
  ["c"]=>
  int(3)
  [0]=>
  &int(4)
}
-- Results when $arr2 is referenced to $arr1 --
array(0) {
}
array(0) {
}
-- Results when $arr1 is changed --
array(0) {
}
array(0) {
}
Done