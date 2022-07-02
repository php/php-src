--TEST--
Test array_intersect() function : basic functionality
--FILE--
<?php
/*
* Testing the behavior of array_intersect() by passing different arrays for the arguments.
* Function is tested by passing associative array as well as array with default keys.
*/

echo "*** Testing array_intersect() : basic functionality ***\n";

// array with default keys
$arr_default_keys = array(1, 2, "hello", 'world');

// associative array
$arr_associative = array("one" => 1, "two" => 2);

// default key array for both $arr1 and $arr2 argument
var_dump( array_intersect($arr_default_keys, $arr_default_keys) );

// default key array for $arr1 and associative array for $arr2 argument
var_dump( array_intersect($arr_default_keys, $arr_associative) );

// associative array for $arr1 and default key array for $arr2
var_dump( array_intersect($arr_associative, $arr_default_keys) );

// associative array for both $arr1 and $arr2 argument
var_dump( array_intersect($arr_associative, $arr_associative) );

// more arrays to be intersected
$arr3 = array(2, 3, 4);
var_dump( array_intersect($arr_default_keys, $arr_associative, $arr3) );
var_dump( array_intersect($arr_associative, $arr_default_keys, $arr3, $arr_associative) );

echo "Done";
?>
--EXPECT--
*** Testing array_intersect() : basic functionality ***
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  string(5) "hello"
  [3]=>
  string(5) "world"
}
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
array(2) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
}
array(2) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
}
array(1) {
  [1]=>
  int(2)
}
array(1) {
  ["two"]=>
  int(2)
}
Done
