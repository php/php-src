--TEST--
Test sort() function : usage variations - unexpected values for 'array_arg' argument
--FILE--
<?php
/* Prototype  : bool sort(array &array_arg [, int $sort_flags])
 * Description: Sort an array 
 * Source code: ext/standard/array.c
*/

/*
 * testing sort() by providing different unexpected values for array argument
 * with following flag values.
 * 1. flag value as defualt
 * 2. SORT_REGULAR - compare items normally
 * 3. SORT_NUMERIC - compare items numerically
 * 4. SORT_STRING - compare items as strings
*/

echo "*** Testing sort() : usage variations ***\n";

//get an unset variable
$unset_var = 10;
unset ($unset_var);

// resource variable
$fp = fopen(__FILE__, "r");

//array of values to iterate over
$unexpected_values = array (

       // int data
/*1*/  0,
       1,
       12345,
       -2345,

       // float data
/*5*/  10.5,
       -10.5,
       10.5e3,
       10.6E-2,
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
/*20*/ new stdclass(),

       // undefined data
/*21*/ @undefined_var,

       // unset data
/*22*/ @unset_var,

       // resource variable
/*23*/ $fp
);

// loop though each element of the array and check the working of sort()
// when $array arugment is supplied with different values from $unexpected_values 
echo "\n-- Testing sort() by supplying different unexpected values for 'array' argument --\n";
echo "\n-- Flag values are defualt, SORT_REGULAR, SORT_NUMERIC, SORT_STRING --\n"; 

$counter = 1;
for($index = 0; $index < count($unexpected_values); $index ++) {
  echo "-- Iteration $counter --\n";
  $value = $unexpected_values [$index];
  var_dump( sort($value) ); // expecting : bool(false)
  var_dump( sort($value, SORT_REGULAR) ); // expecting : bool(false)
  var_dump( sort($value, SORT_NUMERIC) ); // expecting : bool(false)
  var_dump( sort($value, SORT_STRING) ); // expecting : bool(false)
  $counter++;
}

echo "Done";
?>
--EXPECTF--
*** Testing sort() : usage variations ***

-- Testing sort() by supplying different unexpected values for 'array' argument --

-- Flag values are defualt, SORT_REGULAR, SORT_NUMERIC, SORT_STRING --
-- Iteration 1 --

Warning: sort() expects parameter 1 to be array, integer given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, integer given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, integer given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, integer given in %s on line %d
bool(false)
-- Iteration 2 --

Warning: sort() expects parameter 1 to be array, integer given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, integer given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, integer given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, integer given in %s on line %d
bool(false)
-- Iteration 3 --

Warning: sort() expects parameter 1 to be array, integer given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, integer given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, integer given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, integer given in %s on line %d
bool(false)
-- Iteration 4 --

Warning: sort() expects parameter 1 to be array, integer given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, integer given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, integer given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, integer given in %s on line %d
bool(false)
-- Iteration 5 --

Warning: sort() expects parameter 1 to be array, double given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, double given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, double given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, double given in %s on line %d
bool(false)
-- Iteration 6 --

Warning: sort() expects parameter 1 to be array, double given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, double given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, double given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, double given in %s on line %d
bool(false)
-- Iteration 7 --

Warning: sort() expects parameter 1 to be array, double given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, double given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, double given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, double given in %s on line %d
bool(false)
-- Iteration 8 --

Warning: sort() expects parameter 1 to be array, double given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, double given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, double given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, double given in %s on line %d
bool(false)
-- Iteration 9 --

Warning: sort() expects parameter 1 to be array, double given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, double given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, double given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, double given in %s on line %d
bool(false)
-- Iteration 10 --

Warning: sort() expects parameter 1 to be array, null given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, null given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, null given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, null given in %s on line %d
bool(false)
-- Iteration 11 --

Warning: sort() expects parameter 1 to be array, null given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, null given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, null given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, null given in %s on line %d
bool(false)
-- Iteration 12 --

Warning: sort() expects parameter 1 to be array, boolean given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, boolean given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, boolean given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, boolean given in %s on line %d
bool(false)
-- Iteration 13 --

Warning: sort() expects parameter 1 to be array, boolean given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, boolean given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, boolean given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, boolean given in %s on line %d
bool(false)
-- Iteration 14 --

Warning: sort() expects parameter 1 to be array, boolean given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, boolean given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, boolean given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, boolean given in %s on line %d
bool(false)
-- Iteration 15 --

Warning: sort() expects parameter 1 to be array, boolean given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, boolean given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, boolean given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, boolean given in %s on line %d
bool(false)
-- Iteration 16 --

Warning: sort() expects parameter 1 to be array, string given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, string given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, string given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, string given in %s on line %d
bool(false)
-- Iteration 17 --

Warning: sort() expects parameter 1 to be array, string given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, string given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, string given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, string given in %s on line %d
bool(false)
-- Iteration 18 --

Warning: sort() expects parameter 1 to be array, string given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, string given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, string given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, string given in %s on line %d
bool(false)
-- Iteration 19 --

Warning: sort() expects parameter 1 to be array, string given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, string given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, string given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, string given in %s on line %d
bool(false)
-- Iteration 20 --

Warning: sort() expects parameter 1 to be array, object given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, object given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, object given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, object given in %s on line %d
bool(false)
-- Iteration 21 --

Warning: sort() expects parameter 1 to be array, string given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, string given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, string given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, string given in %s on line %d
bool(false)
-- Iteration 22 --

Warning: sort() expects parameter 1 to be array, string given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, string given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, string given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, string given in %s on line %d
bool(false)
-- Iteration 23 --

Warning: sort() expects parameter 1 to be array, resource given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, resource given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, resource given in %s on line %d
bool(false)

Warning: sort() expects parameter 1 to be array, resource given in %s on line %d
bool(false)
Done
