--TEST--
Test array_filter() function : usage variations - Different types of array for 'input' argument
--FILE--
<?php
/*
* Passing different types of array as 'input' argument.
*/

function always_false($input)
{
  return false;
}

// callback function returning always true
function always_true($input)
{
  return true;
}

echo "*** Testing array_filter() : usage variations - different types of array for 'input' argument***\n";

// different types of 'input' array
$input_values = array(
  array(0, 1, 2, -1, 034, 0X4A),  // integer values
  array(0.0, 1.2, 1.2e3, 1.2e-3),  // float values
  array('value1', "value2", '', " ", ""),  // string values
  array(true, false),  // bool values
  array(1 => 'one', 'zero' => 0, -2 => "value"), //associative array
  array("one" => 1, 5 => "float", true => 1, "" => 'empty'),  // associative array with different keys
  array(1 => 'one', 2, "key" => 'value')  // combination of associative and non-associative array

);

// loop through each element of 'input' with default callback
for($count = 0; $count < count($input_values); $count++)
{
  echo "-- Iteration ".($count + 1). " --\n";
  var_dump( array_filter($input_values[$count]) );
  var_dump( array_filter($input_values[$count], 'always_true') );
  var_dump( array_filter($input_values[$count], 'always_false') );
}

echo "Done"
?>
--EXPECT--
*** Testing array_filter() : usage variations - different types of array for 'input' argument***
-- Iteration 1 --
array(5) {
  [1]=>
  int(1)
  [2]=>
  int(2)
  [3]=>
  int(-1)
  [4]=>
  int(28)
  [5]=>
  int(74)
}
array(6) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
  [3]=>
  int(-1)
  [4]=>
  int(28)
  [5]=>
  int(74)
}
array(0) {
}
-- Iteration 2 --
array(3) {
  [1]=>
  float(1.2)
  [2]=>
  float(1200)
  [3]=>
  float(0.0012)
}
array(4) {
  [0]=>
  float(0)
  [1]=>
  float(1.2)
  [2]=>
  float(1200)
  [3]=>
  float(0.0012)
}
array(0) {
}
-- Iteration 3 --
array(3) {
  [0]=>
  string(6) "value1"
  [1]=>
  string(6) "value2"
  [3]=>
  string(1) " "
}
array(5) {
  [0]=>
  string(6) "value1"
  [1]=>
  string(6) "value2"
  [2]=>
  string(0) ""
  [3]=>
  string(1) " "
  [4]=>
  string(0) ""
}
array(0) {
}
-- Iteration 4 --
array(1) {
  [0]=>
  bool(true)
}
array(2) {
  [0]=>
  bool(true)
  [1]=>
  bool(false)
}
array(0) {
}
-- Iteration 5 --
array(2) {
  [1]=>
  string(3) "one"
  [-2]=>
  string(5) "value"
}
array(3) {
  [1]=>
  string(3) "one"
  ["zero"]=>
  int(0)
  [-2]=>
  string(5) "value"
}
array(0) {
}
-- Iteration 6 --
array(4) {
  ["one"]=>
  int(1)
  [5]=>
  string(5) "float"
  [1]=>
  int(1)
  [""]=>
  string(5) "empty"
}
array(4) {
  ["one"]=>
  int(1)
  [5]=>
  string(5) "float"
  [1]=>
  int(1)
  [""]=>
  string(5) "empty"
}
array(0) {
}
-- Iteration 7 --
array(3) {
  [1]=>
  string(3) "one"
  [2]=>
  int(2)
  ["key"]=>
  string(5) "value"
}
array(3) {
  [1]=>
  string(3) "one"
  [2]=>
  int(2)
  ["key"]=>
  string(5) "value"
}
array(0) {
}
Done
