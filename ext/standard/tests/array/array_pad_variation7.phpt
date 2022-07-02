--TEST--
Test array_pad() function : usage variations - two dimensional array for 'input' argument
--FILE--
<?php
/*
* Passing two dimensional array to $input argument and testing whether
* array_pad() behaves in an expected way with the other arguments passed to the function.
* The $pad_size and $pad_value arguments passed are fixed values.
*/

echo "*** Testing array_pad() : Passing 2-D array to \$input argument ***\n";

// initialize the 2-d array
$input = array (
  array(1, 2, 3),
  array("hello", 'world'),
  array("one" => 1, "two" => 2)
);

// initialize the $pad_size and $pad_value arguments
$pad_size = 5;
$pad_value = "HELLO";

// entire 2-d array
echo "-- Entire 2-d array for \$input argument --\n";
var_dump( array_pad($input, $pad_size, $pad_value) );  // positive 'pad_size'
var_dump( array_pad($input, -$pad_size, $pad_value) );  // negative 'pad_size'

// sub array
echo "-- Sub array for \$input argument --\n";
var_dump( array_pad($input[1], $pad_size, $pad_value) );  // positive 'pad_size'
var_dump( array_pad($input[1], -$pad_size, $pad_value) );  // negative 'pad_size'

echo "Done";
?>
--EXPECT--
*** Testing array_pad() : Passing 2-D array to $input argument ***
-- Entire 2-d array for $input argument --
array(5) {
  [0]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
  [1]=>
  array(2) {
    [0]=>
    string(5) "hello"
    [1]=>
    string(5) "world"
  }
  [2]=>
  array(2) {
    ["one"]=>
    int(1)
    ["two"]=>
    int(2)
  }
  [3]=>
  string(5) "HELLO"
  [4]=>
  string(5) "HELLO"
}
array(5) {
  [0]=>
  string(5) "HELLO"
  [1]=>
  string(5) "HELLO"
  [2]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
  [3]=>
  array(2) {
    [0]=>
    string(5) "hello"
    [1]=>
    string(5) "world"
  }
  [4]=>
  array(2) {
    ["one"]=>
    int(1)
    ["two"]=>
    int(2)
  }
}
-- Sub array for $input argument --
array(5) {
  [0]=>
  string(5) "hello"
  [1]=>
  string(5) "world"
  [2]=>
  string(5) "HELLO"
  [3]=>
  string(5) "HELLO"
  [4]=>
  string(5) "HELLO"
}
array(5) {
  [0]=>
  string(5) "HELLO"
  [1]=>
  string(5) "HELLO"
  [2]=>
  string(5) "HELLO"
  [3]=>
  string(5) "hello"
  [4]=>
  string(5) "world"
}
Done
