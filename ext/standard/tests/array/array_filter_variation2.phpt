--TEST--
Test array_filter() function : usage variations - Unexpected values for 'callback' function argument
--FILE--
<?php
/* Prototype  : array array_filter(array $input [, callback $callback])
 * Description: Filters elements from the array via the callback. 
 * Source code: ext/standard/array.c
*/

/* Passing different scalar and nonscalar values in place of 'callback' argument
*/
echo "*** Testing array_filter() : usage variations - unexpected values for 'callback' function***\n";

// Initialise variables
$input = array('value1', 'value2', 'value3', 'value4');

//get an unset variable
$unset_var = 10;
unset ($unset_var);

// class definition for object variable
class MyClass
{
  public function __toString()
  {
    return 'object';
  }
}

// resource variable
$fp = fopen(__FILE__, 'r');

// different scalar and nonscalar values in place of callback function
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

        // array data
/*10*/  array(),
        array(0),
        array(1),
        array(1, 2),
        array('color' => 'red', 'item' => 'pen'),

        // null data
/*15*/  NULL,
        null,

        // boolean data
/*17*/  true,
        false,
        TRUE,
        FALSE,

        // empty data
/*21*/  "",
        '',

        // string data
/*23*/  "string",
        'string',

        // object data
/*25*/  new MyClass(),

        // resource data
        $fp,

        // undefined data
        @$undefined_var,

        // unset data
/*28*/  @$unset_var,
);

// loop through each element of the 'values' for callback
for($count = 0; $count < count($values); $count++) {
  echo "-- Iteration ".($count + 1)." --";
  var_dump( array_filter($input, $values[$count]) );
};

// closing resource
fclose($fp);

echo "Done"
?>
--EXPECTF--
*** Testing array_filter() : usage variations - unexpected values for 'callback' function***
-- Iteration 1 --
Warning: array_filter() expects parameter 2 to be a valid callback, no array or string given in %s on line %d
NULL
-- Iteration 2 --
Warning: array_filter() expects parameter 2 to be a valid callback, no array or string given in %s on line %d
NULL
-- Iteration 3 --
Warning: array_filter() expects parameter 2 to be a valid callback, no array or string given in %s on line %d
NULL
-- Iteration 4 --
Warning: array_filter() expects parameter 2 to be a valid callback, no array or string given in %s on line %d
NULL
-- Iteration 5 --
Warning: array_filter() expects parameter 2 to be a valid callback, no array or string given in %s on line %d
NULL
-- Iteration 6 --
Warning: array_filter() expects parameter 2 to be a valid callback, no array or string given in %s on line %d
NULL
-- Iteration 7 --
Warning: array_filter() expects parameter 2 to be a valid callback, no array or string given in %s on line %d
NULL
-- Iteration 8 --
Warning: array_filter() expects parameter 2 to be a valid callback, no array or string given in %s on line %d
NULL
-- Iteration 9 --
Warning: array_filter() expects parameter 2 to be a valid callback, no array or string given in %s on line %d
NULL
-- Iteration 10 --
Warning: array_filter() expects parameter 2 to be a valid callback, array must have exactly two members in %s on line %d
NULL
-- Iteration 11 --
Warning: array_filter() expects parameter 2 to be a valid callback, array must have exactly two members in %s on line %d
NULL
-- Iteration 12 --
Warning: array_filter() expects parameter 2 to be a valid callback, array must have exactly two members in %s on line %d
NULL
-- Iteration 13 --
Warning: array_filter() expects parameter 2 to be a valid callback, first array member is not a valid class name or object in %s on line %d
NULL
-- Iteration 14 --
Warning: array_filter() expects parameter 2 to be a valid callback, first array member is not a valid class name or object in %s on line %d
NULL
-- Iteration 15 --
Warning: array_filter() expects parameter 2 to be a valid callback, no array or string given in %s on line %d
NULL
-- Iteration 16 --
Warning: array_filter() expects parameter 2 to be a valid callback, no array or string given in %s on line %d
NULL
-- Iteration 17 --
Warning: array_filter() expects parameter 2 to be a valid callback, no array or string given in %s on line %d
NULL
-- Iteration 18 --
Warning: array_filter() expects parameter 2 to be a valid callback, no array or string given in %s on line %d
NULL
-- Iteration 19 --
Warning: array_filter() expects parameter 2 to be a valid callback, no array or string given in %s on line %d
NULL
-- Iteration 20 --
Warning: array_filter() expects parameter 2 to be a valid callback, no array or string given in %s on line %d
NULL
-- Iteration 21 --
Warning: array_filter() expects parameter 2 to be a valid callback, function '' not found or invalid function name in %s on line %d
NULL
-- Iteration 22 --
Warning: array_filter() expects parameter 2 to be a valid callback, function '' not found or invalid function name in %s on line %d
NULL
-- Iteration 23 --
Warning: array_filter() expects parameter 2 to be a valid callback, function 'string' not found or invalid function name in %s on line %d
NULL
-- Iteration 24 --
Warning: array_filter() expects parameter 2 to be a valid callback, function 'string' not found or invalid function name in %s on line %d
NULL
-- Iteration 25 --
Warning: array_filter() expects parameter 2 to be a valid callback, no array or string given in %s on line %d
NULL
-- Iteration 26 --
Warning: array_filter() expects parameter 2 to be a valid callback, no array or string given in %s on line %d
NULL
-- Iteration 27 --
Warning: array_filter() expects parameter 2 to be a valid callback, no array or string given in %s on line %d
NULL
-- Iteration 28 --
Warning: array_filter() expects parameter 2 to be a valid callback, no array or string given in %s on line %d
NULL
Done
