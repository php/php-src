--TEST--
Test array_unshift() function : basic functionality - associative arrays for 'array' argument
--FILE--
<?php
/* Prototype  : int array_unshift(array $array, mixed $var [, mixed ...])
 * Description: Pushes elements onto the beginning of the array
 * Source code: ext/standard/array.c
*/

/*
 * Testing array_unshift() by giving associative arrays for $array argument
*/

echo "*** Testing array_unshift() : basic functionality with associative array ***\n";

// Initialise the array
$array = array('f' => "first", "s" => 'second', 1 => "one", 2 => 'two');

// Calling array_unshift() with default argument
$temp_array = $array;
// returns element count in the resulting array after arguments are pushed to
// beginning of the given array
var_dump( array_unshift($temp_array, 10) );

// dump the resulting array
var_dump($temp_array);

// Calling array_unshift() with optional arguments
$temp_array = $array;
// returns element count in the resulting array after arguments are pushed to
// beginning of the given array
var_dump( array_unshift($temp_array, 222, "hello", 12.33) );

// dump the resulting array
var_dump($temp_array);

echo "Done";
?>
--EXPECTF--
*** Testing array_unshift() : basic functionality with associative array ***
int(5)
array(5) {
  [0]=>
  int(10)
  ["f"]=>
  string(5) "first"
  ["s"]=>
  string(6) "second"
  [1]=>
  string(3) "one"
  [2]=>
  string(3) "two"
}
int(7)
array(7) {
  [0]=>
  int(222)
  [1]=>
  string(5) "hello"
  [2]=>
  float(12.33)
  ["f"]=>
  string(5) "first"
  ["s"]=>
  string(6) "second"
  [3]=>
  string(3) "one"
  [4]=>
  string(3) "two"
}
Done
