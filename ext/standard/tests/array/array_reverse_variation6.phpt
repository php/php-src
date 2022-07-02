--TEST--
Test array_reverse() function : usage variations - two dimensional arrays for 'array' argument
--FILE--
<?php
/*
 * testing the functionality of array_reverse() by giving 2-D arrays for $array argument
*/

echo "*** Testing array_reverse() : usage variations ***\n";

// Initializing the 2-d arrays
$two_dimensional_array = array(

  // associative array
  array('color' => 'red', 'item' => 'pen', 'place' => 'LA'),

   // numeric array
   array(1, 2, 3, 4, 5),

   // combination of numeric and associative arrays
   array('a' => 'green', 'red', 'brown', 33, 88, 'orange', 'item' => 'ball')
);

// calling array_reverse() with various types of 2-d arrays
// with default arguments
echo "-- with default argument --\n";
var_dump( array_reverse($two_dimensional_array) );  // whole array
var_dump( array_reverse($two_dimensional_array[1]) );  // sub array

// with $preserve_keys argument
echo "-- with all possible arguments --\n";
// whole array
var_dump( array_reverse($two_dimensional_array, true) );
var_dump( array_reverse($two_dimensional_array, false) );
// sub array
var_dump( array_reverse($two_dimensional_array[1], true) );
var_dump( array_reverse($two_dimensional_array[1], false) );

echo "Done";
?>
--EXPECT--
*** Testing array_reverse() : usage variations ***
-- with default argument --
array(3) {
  [0]=>
  array(7) {
    ["a"]=>
    string(5) "green"
    [0]=>
    string(3) "red"
    [1]=>
    string(5) "brown"
    [2]=>
    int(33)
    [3]=>
    int(88)
    [4]=>
    string(6) "orange"
    ["item"]=>
    string(4) "ball"
  }
  [1]=>
  array(5) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
    [3]=>
    int(4)
    [4]=>
    int(5)
  }
  [2]=>
  array(3) {
    ["color"]=>
    string(3) "red"
    ["item"]=>
    string(3) "pen"
    ["place"]=>
    string(2) "LA"
  }
}
array(5) {
  [0]=>
  int(5)
  [1]=>
  int(4)
  [2]=>
  int(3)
  [3]=>
  int(2)
  [4]=>
  int(1)
}
-- with all possible arguments --
array(3) {
  [2]=>
  array(7) {
    ["a"]=>
    string(5) "green"
    [0]=>
    string(3) "red"
    [1]=>
    string(5) "brown"
    [2]=>
    int(33)
    [3]=>
    int(88)
    [4]=>
    string(6) "orange"
    ["item"]=>
    string(4) "ball"
  }
  [1]=>
  array(5) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
    [3]=>
    int(4)
    [4]=>
    int(5)
  }
  [0]=>
  array(3) {
    ["color"]=>
    string(3) "red"
    ["item"]=>
    string(3) "pen"
    ["place"]=>
    string(2) "LA"
  }
}
array(3) {
  [0]=>
  array(7) {
    ["a"]=>
    string(5) "green"
    [0]=>
    string(3) "red"
    [1]=>
    string(5) "brown"
    [2]=>
    int(33)
    [3]=>
    int(88)
    [4]=>
    string(6) "orange"
    ["item"]=>
    string(4) "ball"
  }
  [1]=>
  array(5) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
    [3]=>
    int(4)
    [4]=>
    int(5)
  }
  [2]=>
  array(3) {
    ["color"]=>
    string(3) "red"
    ["item"]=>
    string(3) "pen"
    ["place"]=>
    string(2) "LA"
  }
}
array(5) {
  [4]=>
  int(5)
  [3]=>
  int(4)
  [2]=>
  int(3)
  [1]=>
  int(2)
  [0]=>
  int(1)
}
array(5) {
  [0]=>
  int(5)
  [1]=>
  int(4)
  [2]=>
  int(3)
  [3]=>
  int(2)
  [4]=>
  int(1)
}
Done
