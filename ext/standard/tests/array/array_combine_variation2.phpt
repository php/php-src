--TEST--
Test array_combine() function : usage variations - unexpected values for 'values' argument
--FILE--
<?php
/* Prototype  : array array_combine(array $keys, array $values)
 * Description: Creates an array by using the elements of the first parameter as keys 
 *              and the elements of the second as the corresponding values 
 * Source code: ext/standard/array.c
*/

/*
* Testing array_combine() function by passing values to $values argument other than arrays
* and see that function emits proper warning messages wherever expected.
* The $keys argument passed is a fixed array.
*/

echo "*** Testing array_combine() : Passing non-array values to \$values argument ***\n";

// Initialize $keys array
$keys = array(1, 2);

//get an unset variable
$unset_var = 10;
unset($unset_var);

// get a class
class classA
{
  public function __toString() {
    return "Class A object";
  }
}

// heredoc string
$heredoc = <<<EOT
hello world
EOT;

// get a resource variable
$fp = fopen(__FILE__, "r");

// unexpected values to be passed to $values argument
$values_passed = array(

       // int data
/*1*/  0,
       1,
       12345,
       -2345,

       // float data
/*5*/  10.5,
       -10.5,
       12.3456789000e10,
       12.3456789000E-10,
       .5,

       // null data
/*10*/ NULL,
       null,

       // boolean data
/*12*/ true,
       false,
       TRUE,
       FALSE,

       // empty data
/*16*/ "",
       '',

       // string data
/*18*/ "string",
       'string',
       $heredoc,

       // object data
/*21*/ new classA(),

       // undefined data
/*22*/ @$undefined_var,

       // unset data
/*23*/ @$unset_var,

       // resource variable
/*24*/ $fp
);

// loop through each element within $values_passed to check the behavior of array_combine()
$iterator = 1;
foreach($values_passed as $values) {
  echo "-- Iteration $iterator --\n";
  var_dump( array_combine($keys,$values) );
  $iterator++;
};

echo "Done";
?>
--EXPECTF--
*** Testing array_combine() : Passing non-array values to $values argument ***
-- Iteration 1 --

Warning: array_combine() expects parameter 2 to be array, integer given in %s on line %d
NULL
-- Iteration 2 --

Warning: array_combine() expects parameter 2 to be array, integer given in %s on line %d
NULL
-- Iteration 3 --

Warning: array_combine() expects parameter 2 to be array, integer given in %s on line %d
NULL
-- Iteration 4 --

Warning: array_combine() expects parameter 2 to be array, integer given in %s on line %d
NULL
-- Iteration 5 --

Warning: array_combine() expects parameter 2 to be array, double given in %s on line %d
NULL
-- Iteration 6 --

Warning: array_combine() expects parameter 2 to be array, double given in %s on line %d
NULL
-- Iteration 7 --

Warning: array_combine() expects parameter 2 to be array, double given in %s on line %d
NULL
-- Iteration 8 --

Warning: array_combine() expects parameter 2 to be array, double given in %s on line %d
NULL
-- Iteration 9 --

Warning: array_combine() expects parameter 2 to be array, double given in %s on line %d
NULL
-- Iteration 10 --

Warning: array_combine() expects parameter 2 to be array, null given in %s on line %d
NULL
-- Iteration 11 --

Warning: array_combine() expects parameter 2 to be array, null given in %s on line %d
NULL
-- Iteration 12 --

Warning: array_combine() expects parameter 2 to be array, boolean given in %s on line %d
NULL
-- Iteration 13 --

Warning: array_combine() expects parameter 2 to be array, boolean given in %s on line %d
NULL
-- Iteration 14 --

Warning: array_combine() expects parameter 2 to be array, boolean given in %s on line %d
NULL
-- Iteration 15 --

Warning: array_combine() expects parameter 2 to be array, boolean given in %s on line %d
NULL
-- Iteration 16 --

Warning: array_combine() expects parameter 2 to be array, string given in %s on line %d
NULL
-- Iteration 17 --

Warning: array_combine() expects parameter 2 to be array, string given in %s on line %d
NULL
-- Iteration 18 --

Warning: array_combine() expects parameter 2 to be array, string given in %s on line %d
NULL
-- Iteration 19 --

Warning: array_combine() expects parameter 2 to be array, string given in %s on line %d
NULL
-- Iteration 20 --

Warning: array_combine() expects parameter 2 to be array, string given in %s on line %d
NULL
-- Iteration 21 --

Warning: array_combine() expects parameter 2 to be array, object given in %s on line %d
NULL
-- Iteration 22 --

Warning: array_combine() expects parameter 2 to be array, null given in %s on line %d
NULL
-- Iteration 23 --

Warning: array_combine() expects parameter 2 to be array, null given in %s on line %d
NULL
-- Iteration 24 --

Warning: array_combine() expects parameter 2 to be array, resource given in %s on line %d
NULL
Done
