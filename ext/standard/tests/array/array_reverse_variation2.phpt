--TEST--
Test array_reverse() function : usage variations - unexpected values for 'preserve_keys' argument
--FILE--
<?php
/* Prototype  : array array_reverse(array $array [, bool $preserve_keys])
 * Description: Return input as a new array with the order of the entries reversed
 * Source code: ext/standard/array.c
*/

/*
 * testing the functionality of array_reverse() by giving unexpected values for $preserve_keys argument
*/

echo "*** Testing array_reverse() : usage variations ***\n";

// Initialise the array
$array = array("a" => "green", "red", "blue", "red", "orange", "pink");

//get an unset variable
$unset_var = 10;
unset ($unset_var);

//get a resource variable
$fp = fopen(__FILE__, "r");

//get a class
class classA
{
  public function __toString(){
    return "Class A object";
  }
}

//array of values to iterate over
$preserve_keys = array (

       // int data
/*1*/  0,
       1,
       12345,
       -2345,

       // float data
/*5*/  10.5,
       -10.5,
       10.5e10,
       10.6E-10,
       .5,

       // array data
/*10*/ array(),
       array(0),
       array(1),
       array(1, 2),
       array('color' => 'red', 'item' => 'pen'),

       // null data
/*15*/ NULL,
       null,

       // boolean data
       true,
       false,
       TRUE,
       FALSE,

       // empty data
/*21*/
       "",
       '',

       // object data
       new classA(),

       // undefined data
       @$undefined_var,

       // unset data
       @$unset_var,

       // resource variable
/*26*/ $fp

);

// loop through each element of the array $preserve_keys to check the behavior of array_reverse()
$iterator = 1;
foreach($preserve_keys as $preserve_key) {
  echo "-- Iteration $iterator --\n";
  var_dump( array_reverse($array, $preserve_key) );
  $iterator++;
};

// close the file resouce used
fclose($fp);

echo "Done";
?>
--EXPECTF--
*** Testing array_reverse() : usage variations ***
-- Iteration 1 --
array(6) {
  [0]=>
  string(4) "pink"
  [1]=>
  string(6) "orange"
  [2]=>
  string(3) "red"
  [3]=>
  string(4) "blue"
  [4]=>
  string(3) "red"
  ["a"]=>
  string(5) "green"
}
-- Iteration 2 --
array(6) {
  [4]=>
  string(4) "pink"
  [3]=>
  string(6) "orange"
  [2]=>
  string(3) "red"
  [1]=>
  string(4) "blue"
  [0]=>
  string(3) "red"
  ["a"]=>
  string(5) "green"
}
-- Iteration 3 --
array(6) {
  [4]=>
  string(4) "pink"
  [3]=>
  string(6) "orange"
  [2]=>
  string(3) "red"
  [1]=>
  string(4) "blue"
  [0]=>
  string(3) "red"
  ["a"]=>
  string(5) "green"
}
-- Iteration 4 --
array(6) {
  [4]=>
  string(4) "pink"
  [3]=>
  string(6) "orange"
  [2]=>
  string(3) "red"
  [1]=>
  string(4) "blue"
  [0]=>
  string(3) "red"
  ["a"]=>
  string(5) "green"
}
-- Iteration 5 --
array(6) {
  [4]=>
  string(4) "pink"
  [3]=>
  string(6) "orange"
  [2]=>
  string(3) "red"
  [1]=>
  string(4) "blue"
  [0]=>
  string(3) "red"
  ["a"]=>
  string(5) "green"
}
-- Iteration 6 --
array(6) {
  [4]=>
  string(4) "pink"
  [3]=>
  string(6) "orange"
  [2]=>
  string(3) "red"
  [1]=>
  string(4) "blue"
  [0]=>
  string(3) "red"
  ["a"]=>
  string(5) "green"
}
-- Iteration 7 --
array(6) {
  [4]=>
  string(4) "pink"
  [3]=>
  string(6) "orange"
  [2]=>
  string(3) "red"
  [1]=>
  string(4) "blue"
  [0]=>
  string(3) "red"
  ["a"]=>
  string(5) "green"
}
-- Iteration 8 --
array(6) {
  [4]=>
  string(4) "pink"
  [3]=>
  string(6) "orange"
  [2]=>
  string(3) "red"
  [1]=>
  string(4) "blue"
  [0]=>
  string(3) "red"
  ["a"]=>
  string(5) "green"
}
-- Iteration 9 --
array(6) {
  [4]=>
  string(4) "pink"
  [3]=>
  string(6) "orange"
  [2]=>
  string(3) "red"
  [1]=>
  string(4) "blue"
  [0]=>
  string(3) "red"
  ["a"]=>
  string(5) "green"
}
-- Iteration 10 --

Warning: array_reverse() expects parameter 2 to be boolean, array given in %s on line %d
NULL
-- Iteration 11 --

Warning: array_reverse() expects parameter 2 to be boolean, array given in %s on line %d
NULL
-- Iteration 12 --

Warning: array_reverse() expects parameter 2 to be boolean, array given in %s on line %d
NULL
-- Iteration 13 --

Warning: array_reverse() expects parameter 2 to be boolean, array given in %s on line %d
NULL
-- Iteration 14 --

Warning: array_reverse() expects parameter 2 to be boolean, array given in %s on line %d
NULL
-- Iteration 15 --
array(6) {
  [0]=>
  string(4) "pink"
  [1]=>
  string(6) "orange"
  [2]=>
  string(3) "red"
  [3]=>
  string(4) "blue"
  [4]=>
  string(3) "red"
  ["a"]=>
  string(5) "green"
}
-- Iteration 16 --
array(6) {
  [0]=>
  string(4) "pink"
  [1]=>
  string(6) "orange"
  [2]=>
  string(3) "red"
  [3]=>
  string(4) "blue"
  [4]=>
  string(3) "red"
  ["a"]=>
  string(5) "green"
}
-- Iteration 17 --
array(6) {
  [4]=>
  string(4) "pink"
  [3]=>
  string(6) "orange"
  [2]=>
  string(3) "red"
  [1]=>
  string(4) "blue"
  [0]=>
  string(3) "red"
  ["a"]=>
  string(5) "green"
}
-- Iteration 18 --
array(6) {
  [0]=>
  string(4) "pink"
  [1]=>
  string(6) "orange"
  [2]=>
  string(3) "red"
  [3]=>
  string(4) "blue"
  [4]=>
  string(3) "red"
  ["a"]=>
  string(5) "green"
}
-- Iteration 19 --
array(6) {
  [4]=>
  string(4) "pink"
  [3]=>
  string(6) "orange"
  [2]=>
  string(3) "red"
  [1]=>
  string(4) "blue"
  [0]=>
  string(3) "red"
  ["a"]=>
  string(5) "green"
}
-- Iteration 20 --
array(6) {
  [0]=>
  string(4) "pink"
  [1]=>
  string(6) "orange"
  [2]=>
  string(3) "red"
  [3]=>
  string(4) "blue"
  [4]=>
  string(3) "red"
  ["a"]=>
  string(5) "green"
}
-- Iteration 21 --
array(6) {
  [0]=>
  string(4) "pink"
  [1]=>
  string(6) "orange"
  [2]=>
  string(3) "red"
  [3]=>
  string(4) "blue"
  [4]=>
  string(3) "red"
  ["a"]=>
  string(5) "green"
}
-- Iteration 22 --
array(6) {
  [0]=>
  string(4) "pink"
  [1]=>
  string(6) "orange"
  [2]=>
  string(3) "red"
  [3]=>
  string(4) "blue"
  [4]=>
  string(3) "red"
  ["a"]=>
  string(5) "green"
}
-- Iteration 23 --

Warning: array_reverse() expects parameter 2 to be boolean, object given in %s on line %d
NULL
-- Iteration 24 --
array(6) {
  [0]=>
  string(4) "pink"
  [1]=>
  string(6) "orange"
  [2]=>
  string(3) "red"
  [3]=>
  string(4) "blue"
  [4]=>
  string(3) "red"
  ["a"]=>
  string(5) "green"
}
-- Iteration 25 --
array(6) {
  [0]=>
  string(4) "pink"
  [1]=>
  string(6) "orange"
  [2]=>
  string(3) "red"
  [3]=>
  string(4) "blue"
  [4]=>
  string(3) "red"
  ["a"]=>
  string(5) "green"
}
-- Iteration 26 --

Warning: array_reverse() expects parameter 2 to be boolean, resource given in %s on line %d
NULL
Done
