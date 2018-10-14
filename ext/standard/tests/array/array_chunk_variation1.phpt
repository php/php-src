--TEST--
Test array_chunk() function : usage variations - unexpected values for 'array' argument
--FILE--
<?php
/* Prototype  : proto array array_chunk(array $array, int $size [, bool $preserve_keys])
 * Description: Split array into chunks
 *              Chunks an array into size  large chunks.
 * Source code: ext/standard/array.c
*/

/*
* Testing array_chunk() function with unexpected values for 'array' argument
*/

echo "*** Testing array_chunk() : usage variations ***\n";

// Initialise function arguments
$size = 10;

//get an unset variable
$unset_var = 10;
unset ($unset_var);

//array of values to iterate over
$values = array(

        // int data
/*1*/   0,
        1,
        12345,
        -2345,

        // float data
/*5*/   10.5,
        -10.5,
        10.5e10,
        10.6E-10,
        .5,

        // null data
/*10*/  NULL,
        null,

        // boolean data
/*12*/  true,
        false,
        TRUE,
        FALSE,

        // empty data
/*16*/  "",
        '',

        // string data
/*18*/  "string",
        'string',

        // object data
/*20*/  new stdclass(),

        // undefined data
/*21*/  @undefined_var,

        // unset data
/*22*/  @unset_var

);

$count = 1;
// loop through each element of the array for input
foreach($values as $value){
  echo "\n-- Iteration $count --\n";
  var_dump( array_chunk($value, $size) );
  var_dump( array_chunk($value, $size, true) );
  var_dump( array_chunk($value, $size, false) );
  $count++;
}

echo "Done";
?>
--EXPECTF--
*** Testing array_chunk() : usage variations ***

-- Iteration 1 --

Warning: array_chunk() expects parameter 1 to be array, integer given in %s on line %d
NULL

Warning: array_chunk() expects parameter 1 to be array, integer given in %s on line %d
NULL

Warning: array_chunk() expects parameter 1 to be array, integer given in %s on line %d
NULL

-- Iteration 2 --

Warning: array_chunk() expects parameter 1 to be array, integer given in %s on line %d
NULL

Warning: array_chunk() expects parameter 1 to be array, integer given in %s on line %d
NULL

Warning: array_chunk() expects parameter 1 to be array, integer given in %s on line %d
NULL

-- Iteration 3 --

Warning: array_chunk() expects parameter 1 to be array, integer given in %s on line %d
NULL

Warning: array_chunk() expects parameter 1 to be array, integer given in %s on line %d
NULL

Warning: array_chunk() expects parameter 1 to be array, integer given in %s on line %d
NULL

-- Iteration 4 --

Warning: array_chunk() expects parameter 1 to be array, integer given in %s on line %d
NULL

Warning: array_chunk() expects parameter 1 to be array, integer given in %s on line %d
NULL

Warning: array_chunk() expects parameter 1 to be array, integer given in %s on line %d
NULL

-- Iteration 5 --

Warning: array_chunk() expects parameter 1 to be array, float given in %s on line %d
NULL

Warning: array_chunk() expects parameter 1 to be array, float given in %s on line %d
NULL

Warning: array_chunk() expects parameter 1 to be array, float given in %s on line %d
NULL

-- Iteration 6 --

Warning: array_chunk() expects parameter 1 to be array, float given in %s on line %d
NULL

Warning: array_chunk() expects parameter 1 to be array, float given in %s on line %d
NULL

Warning: array_chunk() expects parameter 1 to be array, float given in %s on line %d
NULL

-- Iteration 7 --

Warning: array_chunk() expects parameter 1 to be array, float given in %s on line %d
NULL

Warning: array_chunk() expects parameter 1 to be array, float given in %s on line %d
NULL

Warning: array_chunk() expects parameter 1 to be array, float given in %s on line %d
NULL

-- Iteration 8 --

Warning: array_chunk() expects parameter 1 to be array, float given in %s on line %d
NULL

Warning: array_chunk() expects parameter 1 to be array, float given in %s on line %d
NULL

Warning: array_chunk() expects parameter 1 to be array, float given in %s on line %d
NULL

-- Iteration 9 --

Warning: array_chunk() expects parameter 1 to be array, float given in %s on line %d
NULL

Warning: array_chunk() expects parameter 1 to be array, float given in %s on line %d
NULL

Warning: array_chunk() expects parameter 1 to be array, float given in %s on line %d
NULL

-- Iteration 10 --

Warning: array_chunk() expects parameter 1 to be array, null given in %s on line %d
NULL

Warning: array_chunk() expects parameter 1 to be array, null given in %s on line %d
NULL

Warning: array_chunk() expects parameter 1 to be array, null given in %s on line %d
NULL

-- Iteration 11 --

Warning: array_chunk() expects parameter 1 to be array, null given in %s on line %d
NULL

Warning: array_chunk() expects parameter 1 to be array, null given in %s on line %d
NULL

Warning: array_chunk() expects parameter 1 to be array, null given in %s on line %d
NULL

-- Iteration 12 --

Warning: array_chunk() expects parameter 1 to be array, boolean given in %s on line %d
NULL

Warning: array_chunk() expects parameter 1 to be array, boolean given in %s on line %d
NULL

Warning: array_chunk() expects parameter 1 to be array, boolean given in %s on line %d
NULL

-- Iteration 13 --

Warning: array_chunk() expects parameter 1 to be array, boolean given in %s on line %d
NULL

Warning: array_chunk() expects parameter 1 to be array, boolean given in %s on line %d
NULL

Warning: array_chunk() expects parameter 1 to be array, boolean given in %s on line %d
NULL

-- Iteration 14 --

Warning: array_chunk() expects parameter 1 to be array, boolean given in %s on line %d
NULL

Warning: array_chunk() expects parameter 1 to be array, boolean given in %s on line %d
NULL

Warning: array_chunk() expects parameter 1 to be array, boolean given in %s on line %d
NULL

-- Iteration 15 --

Warning: array_chunk() expects parameter 1 to be array, boolean given in %s on line %d
NULL

Warning: array_chunk() expects parameter 1 to be array, boolean given in %s on line %d
NULL

Warning: array_chunk() expects parameter 1 to be array, boolean given in %s on line %d
NULL

-- Iteration 16 --

Warning: array_chunk() expects parameter 1 to be array, string given in %s on line %d
NULL

Warning: array_chunk() expects parameter 1 to be array, string given in %s on line %d
NULL

Warning: array_chunk() expects parameter 1 to be array, string given in %s on line %d
NULL

-- Iteration 17 --

Warning: array_chunk() expects parameter 1 to be array, string given in %s on line %d
NULL

Warning: array_chunk() expects parameter 1 to be array, string given in %s on line %d
NULL

Warning: array_chunk() expects parameter 1 to be array, string given in %s on line %d
NULL

-- Iteration 18 --

Warning: array_chunk() expects parameter 1 to be array, string given in %s on line %d
NULL

Warning: array_chunk() expects parameter 1 to be array, string given in %s on line %d
NULL

Warning: array_chunk() expects parameter 1 to be array, string given in %s on line %d
NULL

-- Iteration 19 --

Warning: array_chunk() expects parameter 1 to be array, string given in %s on line %d
NULL

Warning: array_chunk() expects parameter 1 to be array, string given in %s on line %d
NULL

Warning: array_chunk() expects parameter 1 to be array, string given in %s on line %d
NULL

-- Iteration 20 --

Warning: array_chunk() expects parameter 1 to be array, object given in %s on line %d
NULL

Warning: array_chunk() expects parameter 1 to be array, object given in %s on line %d
NULL

Warning: array_chunk() expects parameter 1 to be array, object given in %s on line %d
NULL

-- Iteration 21 --

Warning: array_chunk() expects parameter 1 to be array, string given in %s on line %d
NULL

Warning: array_chunk() expects parameter 1 to be array, string given in %s on line %d
NULL

Warning: array_chunk() expects parameter 1 to be array, string given in %s on line %d
NULL

-- Iteration 22 --

Warning: array_chunk() expects parameter 1 to be array, string given in %s on line %d
NULL

Warning: array_chunk() expects parameter 1 to be array, string given in %s on line %d
NULL

Warning: array_chunk() expects parameter 1 to be array, string given in %s on line %d
NULL
Done
