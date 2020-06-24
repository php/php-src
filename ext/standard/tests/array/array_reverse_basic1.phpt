--TEST--
Test array_reverse() function : basic functionality - simple array for 'array' argument
--FILE--
<?php
/*
 * Testing array_reverse() by giving a simple array for $array argument
*/

echo "*** Testing array_reverse() : basic functionality ***\n";

// Initialise the array
$array = array("a", "green", "red", 'blue', 10, 13.33);

// Calling array_reverse() with default arguments
var_dump( array_reverse($array) );

// Calling array_reverse() with all possible arguments
var_dump( array_reverse($array, true) );  // expects the keys to be preserved
var_dump( array_reverse($array, false) );  // expects the keys not to be preserved

echo "Done";
?>
--EXPECT--
*** Testing array_reverse() : basic functionality ***
array(6) {
  [0]=>
  float(13.33)
  [1]=>
  int(10)
  [2]=>
  string(4) "blue"
  [3]=>
  string(3) "red"
  [4]=>
  string(5) "green"
  [5]=>
  string(1) "a"
}
array(6) {
  [5]=>
  float(13.33)
  [4]=>
  int(10)
  [3]=>
  string(4) "blue"
  [2]=>
  string(3) "red"
  [1]=>
  string(5) "green"
  [0]=>
  string(1) "a"
}
array(6) {
  [0]=>
  float(13.33)
  [1]=>
  int(10)
  [2]=>
  string(4) "blue"
  [3]=>
  string(3) "red"
  [4]=>
  string(5) "green"
  [5]=>
  string(1) "a"
}
Done
