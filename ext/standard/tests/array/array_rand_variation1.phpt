--TEST--
Test array_rand() function : usage variations - unexpected values for 'input' parameter
--FILE--
<?php
/* Prototype  : mixed array_rand(array input [, int num_req])
 * Description: Return key/keys for random entry/entries in the array
 * Source code: ext/standard/array.c
*/

/*
* Test array_rand() with different types of values other than arrays passed to the 'input' parameter
* to see that function works with unexpeced data and generates warning message as required.
*/

echo "*** Testing array_rand() : unexpected values for 'input' parameter ***\n";

// Initialise function arguments
$num_req = 10;

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

//array of different values for 'input' parameter
$values = array(

        // int data
/*1*/   0,
        1,
        12345,
        -2345,

        // float data
/*5*/   10.5,
        -10.5,
        12.3456789000e10,
        12.3456789000E-10,
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
/*20*/  new test(),

        // resource data
/*21*/  $fp,

        // undefined data
/*22*/  @$undefined_var,

        // unset data
/*23*/  @$unset_var,
);

/* loop through each element of the array to test array_rand() function
 * for different values for 'input' argument
*/
$count = 1;
foreach($values as $value) {
  echo "\n-- Iteration $count --\n";
  var_dump( array_rand($value,$num_req) );
  $count++;
};

// closing the resource
fclose($fp);

echo "Done";
?>
--EXPECTF--
*** Testing array_rand() : unexpected values for 'input' parameter ***

-- Iteration 1 --

Warning: array_rand() expects parameter 1 to be array, integer given in %s on line %d
NULL

-- Iteration 2 --

Warning: array_rand() expects parameter 1 to be array, integer given in %s on line %d
NULL

-- Iteration 3 --

Warning: array_rand() expects parameter 1 to be array, integer given in %s on line %d
NULL

-- Iteration 4 --

Warning: array_rand() expects parameter 1 to be array, integer given in %s on line %d
NULL

-- Iteration 5 --

Warning: array_rand() expects parameter 1 to be array, float given in %s on line %d
NULL

-- Iteration 6 --

Warning: array_rand() expects parameter 1 to be array, float given in %s on line %d
NULL

-- Iteration 7 --

Warning: array_rand() expects parameter 1 to be array, float given in %s on line %d
NULL

-- Iteration 8 --

Warning: array_rand() expects parameter 1 to be array, float given in %s on line %d
NULL

-- Iteration 9 --

Warning: array_rand() expects parameter 1 to be array, float given in %s on line %d
NULL

-- Iteration 10 --

Warning: array_rand() expects parameter 1 to be array, null given in %s on line %d
NULL

-- Iteration 11 --

Warning: array_rand() expects parameter 1 to be array, null given in %s on line %d
NULL

-- Iteration 12 --

Warning: array_rand() expects parameter 1 to be array, boolean given in %s on line %d
NULL

-- Iteration 13 --

Warning: array_rand() expects parameter 1 to be array, boolean given in %s on line %d
NULL

-- Iteration 14 --

Warning: array_rand() expects parameter 1 to be array, boolean given in %s on line %d
NULL

-- Iteration 15 --

Warning: array_rand() expects parameter 1 to be array, boolean given in %s on line %d
NULL

-- Iteration 16 --

Warning: array_rand() expects parameter 1 to be array, string given in %s on line %d
NULL

-- Iteration 17 --

Warning: array_rand() expects parameter 1 to be array, string given in %s on line %d
NULL

-- Iteration 18 --

Warning: array_rand() expects parameter 1 to be array, string given in %s on line %d
NULL

-- Iteration 19 --

Warning: array_rand() expects parameter 1 to be array, string given in %s on line %d
NULL

-- Iteration 20 --

Warning: array_rand() expects parameter 1 to be array, object given in %s on line %d
NULL

-- Iteration 21 --

Warning: array_rand() expects parameter 1 to be array, resource given in %s on line %d
NULL

-- Iteration 22 --

Warning: array_rand() expects parameter 1 to be array, null given in %s on line %d
NULL

-- Iteration 23 --

Warning: array_rand() expects parameter 1 to be array, null given in %s on line %d
NULL
Done
