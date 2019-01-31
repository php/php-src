--TEST--
Test krsort() function : usage variations - unexpected values for 'array' argument
--FILE--
<?php
/* Prototype  : bool krsort(array &array_arg [, int sort_flags])
 * Description: Sort an array by key in reverse order, maintaining key to data correlation
 * Source code: ext/standard/array.c
*/

/*
 * testing krsort() by providing different unexpected values for array argument
 * with following flag values:
 *  1.flag value as defualt
 *  2.SORT_REGULAR - compare items normally
 *  3.SORT_NUMERIC - compare items numerically
 *  4.SORT_STRING - compare items as strings
*/

echo "*** Testing krsort() : usage variations ***\n";

// get an unset variable
$unset_var = 10;
unset ($unset_var);

// resource variable
$fp = fopen(__FILE__, "r");

$unexpected_values = array (

        // int data
/*1*/   0,
        1,
        12345,
        -2345,

        // float data
/*5*/   10.5,
        -10.5,
        10.5e3,
        10.6E-2,
        0.5,

        // null data
/*10*/  NULL,
        null,

        // boolean data
/*11*/  true,
        false,
        TRUE,
        FALSE,

        // empty data
/*15*/  "",
        '',

        // string data
/*17*/  "string",
        'string',

        // object data
/*19*/  new stdclass(),

        // undefined data
/*20*/  @$undefined_var,

        // unset data
/*21*/  @$unset_var,

        // resource variable
/*22*/  $fp
);

// loop though each element of the array and check the working of krsort()
// when $array argument is supplied with different values from $unexpected_values
echo "\n-- Testing krsort() by supplying different unexpected values for 'array' argument --\n";
echo "\n-- Flag values are defualt, SORT_REGULAR, SORT_NUMERIC, SORT_STRING --\n";

$counter = 1;
for($index = 0; $index < count($unexpected_values); $index ++) {
  echo "-- Iteration $counter --\n";
  $value = $unexpected_values [$index];
  var_dump( krsort($value) ); // expecting : bool(false)
  var_dump( krsort($value, SORT_REGULAR) ); // expecting : bool(false)
  var_dump( krsort($value, SORT_NUMERIC) ); // expecting : bool(false)
  var_dump( krsort($value, SORT_STRING) ); // expecting : bool(false)
  $counter++;
}

echo "Done";
?>
--EXPECTF--
*** Testing krsort() : usage variations ***

-- Testing krsort() by supplying different unexpected values for 'array' argument --

-- Flag values are defualt, SORT_REGULAR, SORT_NUMERIC, SORT_STRING --
-- Iteration 1 --

Warning: krsort() expects parameter 1 to be array, int given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, int given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, int given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, int given in %s on line %d
bool(false)
-- Iteration 2 --

Warning: krsort() expects parameter 1 to be array, int given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, int given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, int given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, int given in %s on line %d
bool(false)
-- Iteration 3 --

Warning: krsort() expects parameter 1 to be array, int given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, int given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, int given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, int given in %s on line %d
bool(false)
-- Iteration 4 --

Warning: krsort() expects parameter 1 to be array, int given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, int given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, int given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, int given in %s on line %d
bool(false)
-- Iteration 5 --

Warning: krsort() expects parameter 1 to be array, float given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, float given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, float given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, float given in %s on line %d
bool(false)
-- Iteration 6 --

Warning: krsort() expects parameter 1 to be array, float given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, float given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, float given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, float given in %s on line %d
bool(false)
-- Iteration 7 --

Warning: krsort() expects parameter 1 to be array, float given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, float given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, float given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, float given in %s on line %d
bool(false)
-- Iteration 8 --

Warning: krsort() expects parameter 1 to be array, float given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, float given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, float given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, float given in %s on line %d
bool(false)
-- Iteration 9 --

Warning: krsort() expects parameter 1 to be array, float given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, float given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, float given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, float given in %s on line %d
bool(false)
-- Iteration 10 --

Warning: krsort() expects parameter 1 to be array, null given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, null given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, null given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, null given in %s on line %d
bool(false)
-- Iteration 11 --

Warning: krsort() expects parameter 1 to be array, null given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, null given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, null given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, null given in %s on line %d
bool(false)
-- Iteration 12 --

Warning: krsort() expects parameter 1 to be array, bool given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, bool given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, bool given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, bool given in %s on line %d
bool(false)
-- Iteration 13 --

Warning: krsort() expects parameter 1 to be array, bool given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, bool given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, bool given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, bool given in %s on line %d
bool(false)
-- Iteration 14 --

Warning: krsort() expects parameter 1 to be array, bool given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, bool given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, bool given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, bool given in %s on line %d
bool(false)
-- Iteration 15 --

Warning: krsort() expects parameter 1 to be array, bool given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, bool given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, bool given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, bool given in %s on line %d
bool(false)
-- Iteration 16 --

Warning: krsort() expects parameter 1 to be array, string given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, string given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, string given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, string given in %s on line %d
bool(false)
-- Iteration 17 --

Warning: krsort() expects parameter 1 to be array, string given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, string given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, string given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, string given in %s on line %d
bool(false)
-- Iteration 18 --

Warning: krsort() expects parameter 1 to be array, string given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, string given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, string given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, string given in %s on line %d
bool(false)
-- Iteration 19 --

Warning: krsort() expects parameter 1 to be array, string given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, string given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, string given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, string given in %s on line %d
bool(false)
-- Iteration 20 --

Warning: krsort() expects parameter 1 to be array, object given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, object given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, object given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, object given in %s on line %d
bool(false)
-- Iteration 21 --

Warning: krsort() expects parameter 1 to be array, null given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, null given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, null given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, null given in %s on line %d
bool(false)
-- Iteration 22 --

Warning: krsort() expects parameter 1 to be array, null given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, null given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, null given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, null given in %s on line %d
bool(false)
-- Iteration 23 --

Warning: krsort() expects parameter 1 to be array, resource given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, resource given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, resource given in %s on line %d
bool(false)

Warning: krsort() expects parameter 1 to be array, resource given in %s on line %d
bool(false)
Done
