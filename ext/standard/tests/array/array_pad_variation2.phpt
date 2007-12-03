--TEST--
Test array_pad() function : usage variations - unexpected values for 'pad_size' argument(Bug#43482)
--FILE--
<?php
/* Prototype  : array array_pad(array $input, int $pad_size, mixed $pad_value)
 * Description: Returns a copy of input array padded with pad_value to size pad_size 
 * Source code: ext/standard/array.c
*/

/*
* Testing array_pad() function by passing values to $pad_size argument other than integers
* and see that function outputs proper warning messages wherever expected.
* The $input and $pad_value arguments passed are fixed values.
*/

echo "*** Testing array_pad() : passing non integer values to \$pad_size argument ***\n";

// Initialise $input and $pad_value arguments
$input = array(1, 2);
$pad_value = 1;

//get an unset variable
$unset_var = 10;
unset ($unset_var);

// get a class
class classA
{
  public function __toString() {
    return "Class A object";
  }
}

//array of values to iterate over
$pad_sizes = array(

      // float data
/*1*/  10.5,
       -10.5,
       12.3456789000e10,
       -12.3456789000e10,
       12.3456789000E-10,
       .5,

       // array data
/*6*/  array(),
       array(0),
       array(1),
       array(1, 2),
       array('color' => 'red', 'item' => 'pen'),

       // null data
/*11*/ NULL,
       null,

       // boolean data
/*13*/ true,
       false,
       TRUE,
       FALSE,

       // empty data
/*17*/ "",
       '',

       // string data
/*19*/ "string",
       'string',

       // object data
/*21*/ new classA(),
 
       // undefined data
/*22*/ @$undefined_var,

       // unset data
/*23*/ @$unset_var,
);

// loop through each element of $pad_sizes to check the behavior of array_pad()
$iterator = 1;
foreach($pad_sizes as $pad_size) {
  echo "-- Iteration $iterator --\n";
  var_dump( array_pad($input, $pad_size, $pad_value) );
  $iterator++;
};

echo "Done";
?>
--EXPECTF--
*** Testing array_pad() : passing non integer values to $pad_size argument ***
-- Iteration 1 --
array(10) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(1)
  [3]=>
  int(1)
  [4]=>
  int(1)
  [5]=>
  int(1)
  [6]=>
  int(1)
  [7]=>
  int(1)
  [8]=>
  int(1)
  [9]=>
  int(1)
}
-- Iteration 2 --
array(10) {
  [0]=>
  int(1)
  [1]=>
  int(1)
  [2]=>
  int(1)
  [3]=>
  int(1)
  [4]=>
  int(1)
  [5]=>
  int(1)
  [6]=>
  int(1)
  [7]=>
  int(1)
  [8]=>
  int(1)
  [9]=>
  int(2)
}
-- Iteration 3 --

Warning: array_pad(): You may only pad up to 1048576 elements at a time in %s on line %d
bool(false)
-- Iteration 4 --

Warning: array_pad(): You may only pad up to 1048576 elements at a time in %s on line %d
bool(false)
-- Iteration 5 --
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
-- Iteration 6 --
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
-- Iteration 7 --
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
-- Iteration 8 --
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
-- Iteration 9 --
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
-- Iteration 10 --
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
-- Iteration 11 --
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
-- Iteration 12 --
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
-- Iteration 13 --
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
-- Iteration 14 --
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
-- Iteration 15 --
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
-- Iteration 16 --
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
-- Iteration 17 --
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
-- Iteration 18 --
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
-- Iteration 19 --
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
-- Iteration 20 --
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
-- Iteration 21 --
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
-- Iteration 22 --

Notice: Object of class classA could not be converted to int in %s on line %d
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
-- Iteration 23 --
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
-- Iteration 24 --
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
Done
