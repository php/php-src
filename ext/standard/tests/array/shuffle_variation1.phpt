--TEST--
Test shuffle() function : usage variations  - unexpected values for 'array_arg' argument
--FILE--
<?php
/* Prototype  : bool shuffle(array $array_arg)
 * Description: Randomly shuffle the contents of an array 
 * Source code: ext/standard/array.c
*/

/*
* Test behaviour of shuffle() when unexpected values are passed for 'array_arg'
* argument and verify that function outputs required warning messages wherever applicable
*/

echo "*** Testing shuffle() : with unexpected values for 'array_arg' argument ***\n";


//get an unset variable
$unset_var = 10;
unset ($unset_var);

//get a resource variable
$fp = fopen(__FILE__, "r");

//define a class
class test
{
  var $t = 10;
  function __toString()
  {
    return "object";
  }
}

//array of values to iterate over
$values = array(

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

       // object data
/*20*/ new test(),

       // undefined data
/*21*/ @$undefined_var,

       // unset data
/*22*/ @$unset_var,

/*23*/ // resource data
       $fp
);

// loop through the array to test shuffle() function
// with each element of the array
$count = 1;
foreach($values as $value) {
  echo "\n-- Iteration $count --\n";
  var_dump( shuffle($value) );
  $count++;
};

// closing the resource
fclose($fp);

echo "Done";
?>
--EXPECTF--
*** Testing shuffle() : with unexpected values for 'array_arg' argument ***

-- Iteration 1 --

Warning: shuffle() expects parameter 1 to be array, integer given in %s on line %d
bool(false)

-- Iteration 2 --

Warning: shuffle() expects parameter 1 to be array, integer given in %s on line %d
bool(false)

-- Iteration 3 --

Warning: shuffle() expects parameter 1 to be array, integer given in %s on line %d
bool(false)

-- Iteration 4 --

Warning: shuffle() expects parameter 1 to be array, integer given in %s on line %d
bool(false)

-- Iteration 5 --

Warning: shuffle() expects parameter 1 to be array, double given in %s on line %d
bool(false)

-- Iteration 6 --

Warning: shuffle() expects parameter 1 to be array, double given in %s on line %d
bool(false)

-- Iteration 7 --

Warning: shuffle() expects parameter 1 to be array, double given in %s on line %d
bool(false)

-- Iteration 8 --

Warning: shuffle() expects parameter 1 to be array, double given in %s on line %d
bool(false)

-- Iteration 9 --

Warning: shuffle() expects parameter 1 to be array, double given in %s on line %d
bool(false)

-- Iteration 10 --

Warning: shuffle() expects parameter 1 to be array, null given in %s on line %d
bool(false)

-- Iteration 11 --

Warning: shuffle() expects parameter 1 to be array, null given in %s on line %d
bool(false)

-- Iteration 12 --

Warning: shuffle() expects parameter 1 to be array, boolean given in %s on line %d
bool(false)

-- Iteration 13 --

Warning: shuffle() expects parameter 1 to be array, boolean given in %s on line %d
bool(false)

-- Iteration 14 --

Warning: shuffle() expects parameter 1 to be array, boolean given in %s on line %d
bool(false)

-- Iteration 15 --

Warning: shuffle() expects parameter 1 to be array, boolean given in %s on line %d
bool(false)

-- Iteration 16 --

Warning: shuffle() expects parameter 1 to be array, string given in %s on line %d
bool(false)

-- Iteration 17 --

Warning: shuffle() expects parameter 1 to be array, string given in %s on line %d
bool(false)

-- Iteration 18 --

Warning: shuffle() expects parameter 1 to be array, string given in %s on line %d
bool(false)

-- Iteration 19 --

Warning: shuffle() expects parameter 1 to be array, string given in %s on line %d
bool(false)

-- Iteration 20 --

Warning: shuffle() expects parameter 1 to be array, object given in %s on line %d
bool(false)

-- Iteration 21 --

Warning: shuffle() expects parameter 1 to be array, null given in %s on line %d
bool(false)

-- Iteration 22 --

Warning: shuffle() expects parameter 1 to be array, null given in %s on line %d
bool(false)

-- Iteration 23 --

Warning: shuffle() expects parameter 1 to be array, resource given in %s on line %d
bool(false)
Done

