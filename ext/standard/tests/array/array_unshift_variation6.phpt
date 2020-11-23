--TEST--
Test array_unshift() function : usage variations - two dimensional arrays for 'array' argument
--FILE--
<?php
/*
 * Testing the functionality of array_unshift() by giving two-dimensional
 * arrays and also sub-arrays within the two-dimensional array for $array argument.
 * The $var argument passed is a fixed value
*/

echo "*** Testing array_unshift() : two dimensional arrays for \$array argument ***\n";

// initializing $var argument
$var = 10;

// two-dimensional array to be passed to $array argument
$two_dimensional_array = array(

   // associative array
   array('color' => 'red', 'item' => 'pen', 'place' => 'LA'),

   // numeric array
   array(1, 2, 3, 4, 5),

   // combination of numeric and associative arrays
   array('a' => 'green', 'red', 'brown', 33, 88, 'orange', 'item' => 'ball')
);

/* Passing the entire $two_dimensional_array to $array */

/* With default argument */
// returns element count in the resulting array after arguments are pushed to
// beginning of the given array
$temp_array = $two_dimensional_array;
var_dump( array_unshift($temp_array, $var) );  // whole 2-d array

// dumps the resulting array
var_dump($temp_array);

/* With optional arguments */
// returns element count in the resulting array after arguments are pushed to
// beginning of the given array
$temp_array = $two_dimensional_array;
var_dump( array_unshift($temp_array, $var, "hello", 'world') );  // whole 2-d array

// dumps the resulting array
var_dump($temp_array);

/* Passing the sub-array within the $two_dimensional_array to $array argument */

/* With default argument */
// returns element count in the resulting array after arguments are pushed to
// beginning of the given array
$temp_array = $two_dimensional_array[0];
var_dump( array_unshift($temp_array, $var) );  // sub array

// dumps the resulting array
var_dump($temp_array);

/* With optional arguments */
// returns element count in the resulting array after arguments are pushed to
// beginning of the given array
$temp_array = $two_dimensional_array[0];
var_dump( array_unshift($temp_array, $var, "hello", 'world') );  // sub array

// dumps the resulting array
var_dump($temp_array);

echo "Done";
?>
--EXPECT--
*** Testing array_unshift() : two dimensional arrays for $array argument ***
int(4)
array(4) {
  [0]=>
  int(10)
  [1]=>
  array(3) {
    ["color"]=>
    string(3) "red"
    ["item"]=>
    string(3) "pen"
    ["place"]=>
    string(2) "LA"
  }
  [2]=>
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
  [3]=>
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
}
int(6)
array(6) {
  [0]=>
  int(10)
  [1]=>
  string(5) "hello"
  [2]=>
  string(5) "world"
  [3]=>
  array(3) {
    ["color"]=>
    string(3) "red"
    ["item"]=>
    string(3) "pen"
    ["place"]=>
    string(2) "LA"
  }
  [4]=>
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
  [5]=>
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
}
int(4)
array(4) {
  [0]=>
  int(10)
  ["color"]=>
  string(3) "red"
  ["item"]=>
  string(3) "pen"
  ["place"]=>
  string(2) "LA"
}
int(6)
array(6) {
  [0]=>
  int(10)
  [1]=>
  string(5) "hello"
  [2]=>
  string(5) "world"
  ["color"]=>
  string(3) "red"
  ["item"]=>
  string(3) "pen"
  ["place"]=>
  string(2) "LA"
}
Done
