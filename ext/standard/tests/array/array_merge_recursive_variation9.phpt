--TEST--
Test array_merge_recursive() function : usage variations - common key and value(Bug#43559)
--FILE--
<?php
/* Prototype  : array array_merge_recursive(array $arr1[, array $...])
 * Description: Recursively merges elements from passed arrays into one array
 * Source code: ext/standard/array.c
*/

/*
 * Testing the functionality of array_merge_recursive() by passing
 * arrays having common key and value.
*/

echo "*** Testing array_merge_recursive() : arrays with common key and value ***\n";

/* initialize the array having duplicate values */

// integer values
$arr1 = array("a" => 1, "b" => 2);
$arr2 = array("b" => 2, "c" => 4);
echo "-- Integer values --\n";
var_dump( array_merge_recursive($arr1, $arr2) );

// float values
$arr1 = array("a" => 1.1, "b" => 2.2);
$arr2 = array("b" => 2.2, "c" => 3.3);
echo "-- Float values --\n";
var_dump( array_merge_recursive($arr1, $arr2) );

// string values
$arr1 = array("a" => "hello", "b" => "world");
$arr2 = array("b" => "world", "c" => "string");
echo "-- String values --\n";
var_dump( array_merge_recursive($arr1, $arr2) );

// boolean values
$arr1 = array("a" => true, "b" => false);
$arr2 = array("b" => false);
echo "-- Boolean values --\n";
var_dump( array_merge_recursive($arr1, $arr2) );

// null values
$arr1 = array( "a" => NULL);
$arr2 = array( "a" => NULL);
echo "-- Null values --\n";
var_dump( array_merge_recursive($arr1, $arr2) );

echo "Done";
?>
--EXPECTF--
*** Testing array_merge_recursive() : arrays with common key and value ***
-- Integer values --
array(3) {
  ["a"]=>
  int(1)
  ["b"]=>
  array(2) {
    [0]=>
    int(2)
    [1]=>
    int(2)
  }
  ["c"]=>
  int(4)
}
-- Float values --
array(3) {
  ["a"]=>
  float(1.1)
  ["b"]=>
  array(2) {
    [0]=>
    float(2.2)
    [1]=>
    float(2.2)
  }
  ["c"]=>
  float(3.3)
}
-- String values --
array(3) {
  ["a"]=>
  string(5) "hello"
  ["b"]=>
  array(2) {
    [0]=>
    string(5) "world"
    [1]=>
    string(5) "world"
  }
  ["c"]=>
  string(6) "string"
}
-- Boolean values --
array(2) {
  ["a"]=>
  bool(true)
  ["b"]=>
  array(2) {
    [0]=>
    bool(false)
    [1]=>
    bool(false)
  }
}
-- Null values --
array(1) {
  ["a"]=>
  array(2) {
    [0]=>
    NULL
    [1]=>
    NULL
  }
}
Done
