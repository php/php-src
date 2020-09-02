--TEST--
Test array_filter() function : usage variations - anonymous callback functions
--FILE--
<?php
/*
* Passing different anonymous callback functions with passed by value and reference arguments
*/

echo "*** Testing array_filter() : usage variations - Anonymous callback functions ***\n";

$input = array(0, 1, -1, 10, 100, 1000, 'Hello', null);

// anonymous callback function
echo "Anonymous callback function with regular parameter and statement\n";
var_dump( array_filter($input, function($input) { return ($input > 1); }) );

// anonymous callback function with null argument
echo "Anonymous callback function with null argument\n";
var_dump( array_filter($input, function() { return true; }) );

// anonymous callback function with argument and null statement
echo "Anonymous callback function with regular argument and null statement\n";
var_dump( array_filter($input, function($input) { }) );

echo "Done"
?>
--EXPECT--
*** Testing array_filter() : usage variations - Anonymous callback functions ***
Anonymous callback function with regular parameter and statement
array(4) {
  [3]=>
  int(10)
  [4]=>
  int(100)
  [5]=>
  int(1000)
  [6]=>
  string(5) "Hello"
}
Anonymous callback function with null argument
array(8) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(-1)
  [3]=>
  int(10)
  [4]=>
  int(100)
  [5]=>
  int(1000)
  [6]=>
  string(5) "Hello"
  [7]=>
  NULL
}
Anonymous callback function with regular argument and null statement
array(0) {
}
Done
