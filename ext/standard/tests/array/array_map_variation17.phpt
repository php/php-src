--TEST--
Test array_map() function : usage variations - unexpected values for 'callback' argument
--FILE--
<?php
/* Prototype  : array array_map  ( callback $callback  , array $arr1  [, array $...  ] )
 * Description: Applies the callback to the elements of the given arrays 
 * Source code: ext/standard/array.c
 */

/*
 * Test array_map() by passing different scalar/nonscalar values in place of $callback
 */

echo "*** Testing array_map() : unexpected values for 'callback' argument ***\n";

$arr1 = array(1, 2, 3);

// get a class
class classA
{
  public function __toString() {
    return "Class A object";
  }
}

// get a resource variable
$fp = fopen(__FILE__, "r");

// unexpected values to be passed to $input argument
$unexpected_callbacks = array(

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

       // boolean data
/*10*/ true,
       false,
       TRUE,
       FALSE,
       
       // empty data
/*14*/ "",
       '',

       // array data
/*16*/ array(),
       array(1, 2),
       array(1, array(2)),

       // object data
/*19*/ new classA(),

       // resource variable
/*20*/ $fp
);

// loop through each element of $inputs to check the behavior of array_map
for($count = 0; $count < count($unexpected_callbacks); $count++) {
  echo "\n-- Iteration ".($count + 1)." --";
  var_dump( array_map($unexpected_callbacks[$count], $arr1));
};

fclose($fp);
echo "Done";
?>
--EXPECTF--
*** Testing array_map() : unexpected values for 'callback' argument ***

-- Iteration 1 --
Warning: array_map() expects parameter 1 to be a valid callback, no array or string given in %s on line %d
NULL

-- Iteration 2 --
Warning: array_map() expects parameter 1 to be a valid callback, no array or string given in %s on line %d
NULL

-- Iteration 3 --
Warning: array_map() expects parameter 1 to be a valid callback, no array or string given in %s on line %d
NULL

-- Iteration 4 --
Warning: array_map() expects parameter 1 to be a valid callback, no array or string given in %s on line %d
NULL

-- Iteration 5 --
Warning: array_map() expects parameter 1 to be a valid callback, no array or string given in %s on line %d
NULL

-- Iteration 6 --
Warning: array_map() expects parameter 1 to be a valid callback, no array or string given in %s on line %d
NULL

-- Iteration 7 --
Warning: array_map() expects parameter 1 to be a valid callback, no array or string given in %s on line %d
NULL

-- Iteration 8 --
Warning: array_map() expects parameter 1 to be a valid callback, no array or string given in %s on line %d
NULL

-- Iteration 9 --
Warning: array_map() expects parameter 1 to be a valid callback, no array or string given in %s on line %d
NULL

-- Iteration 10 --
Warning: array_map() expects parameter 1 to be a valid callback, no array or string given in %s on line %d
NULL

-- Iteration 11 --
Warning: array_map() expects parameter 1 to be a valid callback, no array or string given in %s on line %d
NULL

-- Iteration 12 --
Warning: array_map() expects parameter 1 to be a valid callback, no array or string given in %s on line %d
NULL

-- Iteration 13 --
Warning: array_map() expects parameter 1 to be a valid callback, no array or string given in %s on line %d
NULL

-- Iteration 14 --
Warning: array_map() expects parameter 1 to be a valid callback, function '' not found or invalid function name in %s on line %d
NULL

-- Iteration 15 --
Warning: array_map() expects parameter 1 to be a valid callback, function '' not found or invalid function name in %s on line %d
NULL

-- Iteration 16 --
Warning: array_map() expects parameter 1 to be a valid callback, array must have exactly two members in %s on line %d
NULL

-- Iteration 17 --
Warning: array_map() expects parameter 1 to be a valid callback, first array member is not a valid class name or object in %s on line %d
NULL

-- Iteration 18 --
Warning: array_map() expects parameter 1 to be a valid callback, first array member is not a valid class name or object in %s on line %d
NULL

-- Iteration 19 --
Warning: array_map() expects parameter 1 to be a valid callback, no array or string given in %s on line %d
NULL

-- Iteration 20 --
Warning: array_map() expects parameter 1 to be a valid callback, no array or string given in %s on line %d
NULL
Done
