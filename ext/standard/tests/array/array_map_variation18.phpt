--TEST--
Test array_map() function : usage variations - unexpected values for 'arr1' argument
--FILE--
<?php
/* Prototype  : array array_map  ( callback $callback  , array $arr1  [, array $...  ] )
 * Description: Applies the callback to the elements of the given arrays
 * Source code: ext/standard/array.c
 */

/*
 * Test array_map() by passing non array values in place of $arr1
 */

echo "*** Testing array_map() : unexpected values for 'arr1' ***\n";

function callback($a)
{
  return $a;
}

//get an unset array variable
$unset_var1 = array(1, 2);
unset ($unset_var1);

// get an unset variable
$unset_var2 = 10;
unset ($unset_var2);

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

// different scalar/non-scalar values for array input
$unexpected_inputs = array(

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
/*23*/ @$unset_var1,
       @$unset_var2,

       // resource variable
/*25*/ $fp
);

// loop through each element of $unexpected_inputs to check the behavior of array_map
for($count = 0; $count < count($unexpected_inputs); $count++) {
  echo "-- Iteration ".($count + 1)." --\n";
  var_dump( array_map('callback', $unexpected_inputs[$count]));
};

fclose($fp);
echo "Done";
?>
--EXPECTF--
*** Testing array_map() : unexpected values for 'arr1' ***
-- Iteration 1 --

Warning: array_map(): Argument #2 should be an array in %s on line %d
NULL
-- Iteration 2 --

Warning: array_map(): Argument #2 should be an array in %s on line %d
NULL
-- Iteration 3 --

Warning: array_map(): Argument #2 should be an array in %s on line %d
NULL
-- Iteration 4 --

Warning: array_map(): Argument #2 should be an array in %s on line %d
NULL
-- Iteration 5 --

Warning: array_map(): Argument #2 should be an array in %s on line %d
NULL
-- Iteration 6 --

Warning: array_map(): Argument #2 should be an array in %s on line %d
NULL
-- Iteration 7 --

Warning: array_map(): Argument #2 should be an array in %s on line %d
NULL
-- Iteration 8 --

Warning: array_map(): Argument #2 should be an array in %s on line %d
NULL
-- Iteration 9 --

Warning: array_map(): Argument #2 should be an array in %s on line %d
NULL
-- Iteration 10 --

Warning: array_map(): Argument #2 should be an array in %s on line %d
NULL
-- Iteration 11 --

Warning: array_map(): Argument #2 should be an array in %s on line %d
NULL
-- Iteration 12 --

Warning: array_map(): Argument #2 should be an array in %s on line %d
NULL
-- Iteration 13 --

Warning: array_map(): Argument #2 should be an array in %s on line %d
NULL
-- Iteration 14 --

Warning: array_map(): Argument #2 should be an array in %s on line %d
NULL
-- Iteration 15 --

Warning: array_map(): Argument #2 should be an array in %s on line %d
NULL
-- Iteration 16 --

Warning: array_map(): Argument #2 should be an array in %s on line %d
NULL
-- Iteration 17 --

Warning: array_map(): Argument #2 should be an array in %s on line %d
NULL
-- Iteration 18 --

Warning: array_map(): Argument #2 should be an array in %s on line %d
NULL
-- Iteration 19 --

Warning: array_map(): Argument #2 should be an array in %s on line %d
NULL
-- Iteration 20 --

Warning: array_map(): Argument #2 should be an array in %s on line %d
NULL
-- Iteration 21 --

Warning: array_map(): Argument #2 should be an array in %s on line %d
NULL
-- Iteration 22 --

Warning: array_map(): Argument #2 should be an array in %s on line %d
NULL
-- Iteration 23 --

Warning: array_map(): Argument #2 should be an array in %s on line %d
NULL
-- Iteration 24 --

Warning: array_map(): Argument #2 should be an array in %s on line %d
NULL
-- Iteration 25 --

Warning: array_map(): Argument #2 should be an array in %s on line %d
NULL
Done
