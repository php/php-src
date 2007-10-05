--TEST--
Test str_split() function : usage variations - unexpected values for 'str' argument 
--FILE--
<?php
/* Prototype  : array str_split(string $str [, int $split_length])
 * Description: Convert a string to an array. If split_length is 
                specified, break the string down into chunks each 
                split_length characters long. 
 * Source code: ext/standard/string.c
 * Alias to functions: none
*/

echo "*** Testing str_split() : unexpected values for 'str' ***\n";

// Initialise function arguments
$split_length = 3;

//get an unset variable
$unset_var = 10;
unset ($unset_var);

//defining class for object variable
class MyClass
{
  public function __toString()
  {
    return "object";
  }
}

//resource variable
$fp = fopen(__FILE__, 'r');

//different values for 'str' argument
$values = array(

  // int data
  0,
  1,
  12345,
  -2345,

  // float data
  10.5,
  -10.5,
  10.5e10,
  10.6E-10,
  .5,

  // array data
  array(),
  array(0),
  array(1),
  array(1, 2),
  array('color' => 'red', 'item' => 'pen'),

  // null data
  NULL,
  null,

  // boolean data
  true,
  false,
  TRUE,
  FALSE,

  // empty data
  "",
  '',

  // object data
  new MyClass(),

  // undefined data
  @$undefined_var,

  // unset data
  @$unset_var,

  //resource data
  $fp
);

// loop through each element of $values for 'str' argument
for($count = 0; $count < count($values); $count++) {
  echo "-- Iteration ".($count+1)." --\n";
  var_dump( str_split($values[$count], $split_length) );
}

//closing resource
fclose($fp);

echo "Done";
?>
--EXPECTF--
*** Testing str_split() : unexpected values for 'str' ***
-- Iteration 1 --
array(1) {
  [0]=>
  string(1) "0"
}
-- Iteration 2 --
array(1) {
  [0]=>
  string(1) "1"
}
-- Iteration 3 --
array(2) {
  [0]=>
  string(3) "123"
  [1]=>
  string(2) "45"
}
-- Iteration 4 --
array(2) {
  [0]=>
  string(3) "-23"
  [1]=>
  string(2) "45"
}
-- Iteration 5 --
array(2) {
  [0]=>
  string(3) "10."
  [1]=>
  string(1) "5"
}
-- Iteration 6 --
array(2) {
  [0]=>
  string(3) "-10"
  [1]=>
  string(2) ".5"
}
-- Iteration 7 --
array(4) {
  [0]=>
  string(3) "105"
  [1]=>
  string(3) "000"
  [2]=>
  string(3) "000"
  [3]=>
  string(3) "000"
}
-- Iteration 8 --
array(3) {
  [0]=>
  string(3) "1.0"
  [1]=>
  string(3) "6E-"
  [2]=>
  string(1) "9"
}
-- Iteration 9 --
array(1) {
  [0]=>
  string(3) "0.5"
}
-- Iteration 10 --

Warning: str_split() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 11 --

Warning: str_split() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 12 --

Warning: str_split() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 13 --

Warning: str_split() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 14 --

Warning: str_split() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 15 --
array(1) {
  [0]=>
  string(0) ""
}
-- Iteration 16 --
array(1) {
  [0]=>
  string(0) ""
}
-- Iteration 17 --
array(1) {
  [0]=>
  string(1) "1"
}
-- Iteration 18 --
array(1) {
  [0]=>
  string(0) ""
}
-- Iteration 19 --
array(1) {
  [0]=>
  string(1) "1"
}
-- Iteration 20 --
array(1) {
  [0]=>
  string(0) ""
}
-- Iteration 21 --
array(1) {
  [0]=>
  string(0) ""
}
-- Iteration 22 --
array(1) {
  [0]=>
  string(0) ""
}
-- Iteration 23 --
array(2) {
  [0]=>
  string(3) "obj"
  [1]=>
  string(3) "ect"
}
-- Iteration 24 --
array(1) {
  [0]=>
  string(0) ""
}
-- Iteration 25 --
array(1) {
  [0]=>
  string(0) ""
}
-- Iteration 26 --

Warning: str_split() expects parameter 1 to be string, resource given in %s on line %d
NULL
Done
