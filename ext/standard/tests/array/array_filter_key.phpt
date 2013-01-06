--TEST--
Test array_filter() function : key functionality 
--FILE--
<?php
/* Prototype  : array array_filter(array $input [, callback $callback])
 * Description: Filters elements from the array via the callback. 
 * Source code: ext/standard/array.c
*/

echo "*** Testing array_filter() : key functionality ***\n";

function vd($value)
{
    var_dump($value);
    return true;
}

function vd_key($value, $key)
{
    var_dump($value, $key);
    return true;
}

// different types of 'input' array
$input_values = array(
  array(0, 1, 2, 3),  // integer keys
  array(1 => 'one', 'zero' => 0, -2 => "value"), //associative array
  array("one" => 1, null => 'null', 5.2 => "float", true => 1, "" => 'empty'),  // associative array with different keys
  array(1 => 'one', 2, "key" => 'value')  // combinition of associative and non-associative array
);

// loop through each element of 'input' with default callback
for($count = 0; $count < count($input_values); $count++)
{
  echo "-- Iteration ".($count + 1). " --\n";
  var_dump( array_filter($input_values[$count], 'vd', true) );
  var_dump( array_filter($input_values[$count], 'vd_key', true) );
}

echo "Done";
?>
--EXPECTF--
*** Testing array_filter() : key functionality ***
-- Iteration 1 --
int(0)
int(1)
int(2)
int(3)
array(4) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
  [3]=>
  int(3)
}
int(0)
int(0)
int(1)
int(1)
int(2)
int(2)
int(3)
int(3)
array(4) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
  [3]=>
  int(3)
}
-- Iteration 2 --
string(3) "one"
int(0)
string(5) "value"
array(3) {
  [1]=>
  string(3) "one"
  ["zero"]=>
  int(0)
  [-2]=>
  string(5) "value"
}
string(3) "one"
int(1)
int(0)
string(4) "zero"
string(5) "value"
int(-2)
array(3) {
  [1]=>
  string(3) "one"
  ["zero"]=>
  int(0)
  [-2]=>
  string(5) "value"
}
-- Iteration 3 --
int(1)
string(5) "empty"
string(5) "float"
int(1)
array(4) {
  ["one"]=>
  int(1)
  [""]=>
  string(5) "empty"
  [5]=>
  string(5) "float"
  [1]=>
  int(1)
}
int(1)
string(3) "one"
string(5) "empty"
NULL
string(5) "float"
int(5)
int(1)
int(1)
array(4) {
  ["one"]=>
  int(1)
  [""]=>
  string(5) "empty"
  [5]=>
  string(5) "float"
  [1]=>
  int(1)
}
-- Iteration 4 --
string(3) "one"
int(2)
string(5) "value"
array(3) {
  [1]=>
  string(3) "one"
  [2]=>
  int(2)
  ["key"]=>
  string(5) "value"
}
string(3) "one"
int(1)
int(2)
int(2)
string(5) "value"
string(3) "key"
array(3) {
  [1]=>
  string(3) "one"
  [2]=>
  int(2)
  ["key"]=>
  string(5) "value"
}
Done
