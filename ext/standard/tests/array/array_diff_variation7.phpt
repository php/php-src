--TEST--
Test array_diff() function : usage variations - arrays containing referenced variables
--FILE--
<?php
/*
 * Test how array_diff compares arrays that
 * 1. Contain referenced variables
 * 2. Have been referenced to each other
 */

echo "*** Testing array_diff() : usage variations ***\n";
$a = 'a';

$arr1 = array ("&$a", 'b', 'c');
$arr2 = array (1, 2, 3);
echo "-- Basic Comparison --\n";
var_dump(array_diff($arr1, $arr2));
var_dump(array_diff($arr2, $arr1));

$a = 1;

echo "-- \$a changed --\n";
var_dump(array_diff($arr1, $arr2));
var_dump(array_diff($arr2, $arr1));


$arr2 = &$arr1;
echo "-- Arrays referenced to each other --\n";
var_dump(array_diff($arr1, $arr2));
var_dump(array_diff($arr2, $arr1));


$arr1 = array('x', 'y', 'z');
echo "-- \$arr1 changed --\n";
var_dump(array_diff($arr1, $arr2));
var_dump(array_diff($arr2, $arr1));


echo "Done";
?>
--EXPECT--
*** Testing array_diff() : usage variations ***
-- Basic Comparison --
array(3) {
  [0]=>
  string(2) "&a"
  [1]=>
  string(1) "b"
  [2]=>
  string(1) "c"
}
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
-- $a changed --
array(3) {
  [0]=>
  string(2) "&a"
  [1]=>
  string(1) "b"
  [2]=>
  string(1) "c"
}
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
-- Arrays referenced to each other --
array(0) {
}
array(0) {
}
-- $arr1 changed --
array(0) {
}
array(0) {
}
Done
