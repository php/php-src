--TEST--
Test uasort() function : usage variations - unexpected values for 'cmp_function' argument
--FILE--
<?php
/* Prototype  : bool uasort(array $array_arg, string $cmp_function)
 * Description: Sort an array with a user-defined comparison function and maintain index association 
 * Source code: ext/standard/array.c
*/

/*
* Testing uasort() function with different scalar and nonscalar values in place of 'cmp_function'
*/

echo "*** Testing uasort() : Unexpected values in place of comparison function ***\n";

// Class definition for object variable
class MyClass
{
  public function __toString()
  {
    return 'object';
  }
}

$array_arg = array(0 => 1, 1 => -1, 2 => 3, 3 => 10, 4 => 4, 5 => 2, 6 => 8, 7 => 5);

// Get an unset variable
$unset_var = 10;
unset ($unset_var);

// Get resource variable
$fp = fopen(__FILE__,'r');

// different values for 'cmp_function'
$cmp_values = array(

       // int data
/*1*/  0,
       1,
       12345,
       -2345,

       // float data
/*5*/  10.5,
       -10.5,
       10.1234567e8,
       10.7654321E-8,
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
/*17*/ true,
       false,
       TRUE,
       FALSE,

       // empty data
/*21*/ "",
       '',

       // string data
       "string",
       'string',

       // object data
/*25*/ new MyClass(),

       // resource data
       $fp,

       // undefined data
       @$undefined_var,

       // unset data
/*28*/ @$unset_var,
);

// loop through each element of the cmp_values for 'cmp_function'
for($count = 0; $count < count($cmp_values); $count++) {
  echo "-- Iteration ".($count + 1)." --\n";
  var_dump( uasort($array_arg, $cmp_values[$count]) );
};

//closing resource
fclose($fp);
echo "Done"
?>
--EXPECTF--
*** Testing uasort() : Unexpected values in place of comparison function ***
-- Iteration 1 --

Warning: uasort(): Invalid comparison function in %s on line %d
bool(false)
-- Iteration 2 --

Warning: uasort(): Invalid comparison function in %s on line %d
bool(false)
-- Iteration 3 --

Warning: uasort(): Invalid comparison function in %s on line %d
bool(false)
-- Iteration 4 --

Warning: uasort(): Invalid comparison function in %s on line %d
bool(false)
-- Iteration 5 --

Warning: uasort(): Invalid comparison function in %s on line %d
bool(false)
-- Iteration 6 --

Warning: uasort(): Invalid comparison function in %s on line %d
bool(false)
-- Iteration 7 --

Warning: uasort(): Invalid comparison function in %s on line %d
bool(false)
-- Iteration 8 --

Warning: uasort(): Invalid comparison function in %s on line %d
bool(false)
-- Iteration 9 --

Warning: uasort(): Invalid comparison function in %s on line %d
bool(false)
-- Iteration 10 --

Warning: uasort(): Invalid comparison function in %s on line %d
bool(false)
-- Iteration 11 --

Warning: uasort(): Invalid comparison function in %s on line %d
bool(false)
-- Iteration 12 --

Warning: uasort(): Invalid comparison function in %s on line %d
bool(false)
-- Iteration 13 --

Warning: uasort(): Invalid comparison function in %s on line %d
bool(false)
-- Iteration 14 --

Warning: uasort(): Invalid comparison function in %s on line %d
bool(false)
-- Iteration 15 --

Warning: uasort(): Invalid comparison function in %s on line %d
bool(false)
-- Iteration 16 --

Warning: uasort(): Invalid comparison function in %s on line %d
bool(false)
-- Iteration 17 --

Warning: uasort(): Invalid comparison function in %s on line %d
bool(false)
-- Iteration 18 --

Warning: uasort(): Invalid comparison function in %s on line %d
bool(false)
-- Iteration 19 --

Warning: uasort(): Invalid comparison function in %s on line %d
bool(false)
-- Iteration 20 --

Warning: uasort(): Invalid comparison function in %s on line %d
bool(false)
-- Iteration 21 --

Warning: uasort(): Invalid comparison function in %s on line %d
bool(false)
-- Iteration 22 --

Warning: uasort(): Invalid comparison function in %s on line %d
bool(false)
-- Iteration 23 --

Warning: uasort(): Invalid comparison function in %s on line %d
bool(false)
-- Iteration 24 --

Warning: uasort(): Invalid comparison function in %s on line %d
bool(false)
-- Iteration 25 --

Warning: uasort(): Invalid comparison function in %s on line %d
bool(false)
-- Iteration 26 --

Warning: uasort(): Invalid comparison function in %s on line %d
bool(false)
-- Iteration 27 --

Warning: uasort(): Invalid comparison function in %s on line %d
bool(false)
-- Iteration 28 --

Warning: uasort(): Invalid comparison function in %s on line %d
bool(false)
Done
