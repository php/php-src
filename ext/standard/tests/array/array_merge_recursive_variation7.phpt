--TEST--
Test array_merge_recursive() function : usage variations - array with reference variables
--FILE--
<?php
/* Prototype  : array array_merge_recursive(array $arr1[, array $...])
 * Description: Recursively merges elements from passed arrays into one array
 * Source code: ext/standard/array.c
*/

/*
 * Testing the functionality of array_merge_recursive() by passing 
 * array having reference variables.
*/

echo "*** Testing array_merge_recursive() : array with reference variables for \$arr1 argument ***\n";

$value1 = 10;
$value2 = "hello";
$value3 = 0;
$value4 = &$value2;

// input array containing elements as reference variables
$arr1 = array(
  0 => 0,
  1 => &$value4,
  2 => &$value2,
  3 => "hello",
  4 => &$value3,
  $value4 => &$value2
);

// initialize the second argument
$arr2 = array($value4 => "hello", &$value2);

echo "-- With default argument --\n";
var_dump( array_merge_recursive($arr1) );

echo "-- With more arguments --\n";
var_dump( array_merge_recursive($arr1, $arr2) );

echo "Done";
?>
--EXPECTF--
*** Testing array_merge_recursive() : array with reference variables for $arr1 argument ***
-- With default argument --
array(6) {
  [0]=>
  int(0)
  [1]=>
  &string(5) "hello"
  [2]=>
  &string(5) "hello"
  [3]=>
  string(5) "hello"
  [4]=>
  &int(0)
  ["hello"]=>
  &string(5) "hello"
}
-- With more arguments --
array(7) {
  [0]=>
  int(0)
  [1]=>
  &string(5) "hello"
  [2]=>
  &string(5) "hello"
  [3]=>
  string(5) "hello"
  [4]=>
  &int(0)
  ["hello"]=>
  array(2) {
    [0]=>
    string(5) "hello"
    [1]=>
    string(5) "hello"
  }
  [5]=>
  &string(5) "hello"
}
Done
