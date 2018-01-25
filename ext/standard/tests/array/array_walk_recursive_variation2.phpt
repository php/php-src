--TEST--
Test array_walk_recursive() function : usage variations - unexpected values in place of 'funcname' argument
--FILE--
<?php
/* Prototype  : bool array_walk_recursive(array $input, string $funcname [, mixed $userdata])
 * Description: Apply a user function to every member of an array
 * Source code: ext/standard/array.c
*/

/*
 * Passing different scalar/nonscalar values in place of 'funcname' argument
*/

echo "*** Testing array_walk_recursive() : unexpected values for 'funcname' argument ***\n";

$input = array(1, array(2, 3));

//get an unset variable
$unset_var = 10;
unset ($unset_var);

$user_data = 20;

// class definition
class MyClass
{
  public function __toString()
  {
    return 'object';
  }
}

// different scalar/nonscalar values to be used in place of callback function
$funcname_values = array(

         // int data
/* 1*/   0,
         1,
         12345,
         -2345,

         // float data
/* 5*/   10.5,
         -10.5,
         10.1234567e8,
         10.7654321E-8,
         .5,

         // array data
/* 10*/  array(),
         array(0),
         array(1),
         array('color' => 'red', 'item' => 'pen'),

         // null data
/* 14*/  NULL,
         null,

         // boolean data
/* 16*/  true,
         false,
         TRUE,
         FALSE,

         // empty data
/* 20*/  "",
         '',

         // object data
         new MyClass(),

         // resource data
/* 23*/  $fp = fopen(__FILE__, 'r'),

         // undefined data
         @$undefined_var,

         // unset data
/* 25*/  @$unset_var,
);

for($count = 0; $count < count($funcname_values); $count++) {
  echo "-- Iteration ".($count + 1)." --\n";
  var_dump( array_walk_recursive($input, $funcname_values[$count]) );
  var_dump( array_walk_recursive($input, $funcname_values[$count], $user_data ));
}

fclose($fp);
echo "Done"
?>
--EXPECTF--
*** Testing array_walk_recursive() : unexpected values for 'funcname' argument ***
-- Iteration 1 --

Warning: array_walk_recursive() expects parameter 2 to be a valid callback, no array or string given in %s on line %d
NULL

Warning: array_walk_recursive() expects parameter 2 to be a valid callback, no array or string given in %s on line %d
NULL
-- Iteration 2 --

Warning: array_walk_recursive() expects parameter 2 to be a valid callback, no array or string given in %s on line %d
NULL

Warning: array_walk_recursive() expects parameter 2 to be a valid callback, no array or string given in %s on line %d
NULL
-- Iteration 3 --

Warning: array_walk_recursive() expects parameter 2 to be a valid callback, no array or string given in %s on line %d
NULL

Warning: array_walk_recursive() expects parameter 2 to be a valid callback, no array or string given in %s on line %d
NULL
-- Iteration 4 --

Warning: array_walk_recursive() expects parameter 2 to be a valid callback, no array or string given in %s on line %d
NULL

Warning: array_walk_recursive() expects parameter 2 to be a valid callback, no array or string given in %s on line %d
NULL
-- Iteration 5 --

Warning: array_walk_recursive() expects parameter 2 to be a valid callback, no array or string given in %s on line %d
NULL

Warning: array_walk_recursive() expects parameter 2 to be a valid callback, no array or string given in %s on line %d
NULL
-- Iteration 6 --

Warning: array_walk_recursive() expects parameter 2 to be a valid callback, no array or string given in %s on line %d
NULL

Warning: array_walk_recursive() expects parameter 2 to be a valid callback, no array or string given in %s on line %d
NULL
-- Iteration 7 --

Warning: array_walk_recursive() expects parameter 2 to be a valid callback, no array or string given in %s on line %d
NULL

Warning: array_walk_recursive() expects parameter 2 to be a valid callback, no array or string given in %s on line %d
NULL
-- Iteration 8 --

Warning: array_walk_recursive() expects parameter 2 to be a valid callback, no array or string given in %s on line %d
NULL

Warning: array_walk_recursive() expects parameter 2 to be a valid callback, no array or string given in %s on line %d
NULL
-- Iteration 9 --

Warning: array_walk_recursive() expects parameter 2 to be a valid callback, no array or string given in %s on line %d
NULL

Warning: array_walk_recursive() expects parameter 2 to be a valid callback, no array or string given in %s on line %d
NULL
-- Iteration 10 --

Warning: array_walk_recursive() expects parameter 2 to be a valid callback, array must have exactly two members in %s on line %d
NULL

Warning: array_walk_recursive() expects parameter 2 to be a valid callback, array must have exactly two members in %s on line %d
NULL
-- Iteration 11 --

Warning: array_walk_recursive() expects parameter 2 to be a valid callback, array must have exactly two members in %s on line %d
NULL

Warning: array_walk_recursive() expects parameter 2 to be a valid callback, array must have exactly two members in %s on line %d
NULL
-- Iteration 12 --

Warning: array_walk_recursive() expects parameter 2 to be a valid callback, array must have exactly two members in %s on line %d
NULL

Warning: array_walk_recursive() expects parameter 2 to be a valid callback, array must have exactly two members in %s on line %d
NULL
-- Iteration 13 --

Warning: array_walk_recursive() expects parameter 2 to be a valid callback, first array member is not a valid class name or object in %s on line %d
NULL

Warning: array_walk_recursive() expects parameter 2 to be a valid callback, first array member is not a valid class name or object in %s on line %d
NULL
-- Iteration 14 --

Warning: array_walk_recursive() expects parameter 2 to be a valid callback, no array or string given in %s on line %d
NULL

Warning: array_walk_recursive() expects parameter 2 to be a valid callback, no array or string given in %s on line %d
NULL
-- Iteration 15 --

Warning: array_walk_recursive() expects parameter 2 to be a valid callback, no array or string given in %s on line %d
NULL

Warning: array_walk_recursive() expects parameter 2 to be a valid callback, no array or string given in %s on line %d
NULL
-- Iteration 16 --

Warning: array_walk_recursive() expects parameter 2 to be a valid callback, no array or string given in %s on line %d
NULL

Warning: array_walk_recursive() expects parameter 2 to be a valid callback, no array or string given in %s on line %d
NULL
-- Iteration 17 --

Warning: array_walk_recursive() expects parameter 2 to be a valid callback, no array or string given in %s on line %d
NULL

Warning: array_walk_recursive() expects parameter 2 to be a valid callback, no array or string given in %s on line %d
NULL
-- Iteration 18 --

Warning: array_walk_recursive() expects parameter 2 to be a valid callback, no array or string given in %s on line %d
NULL

Warning: array_walk_recursive() expects parameter 2 to be a valid callback, no array or string given in %s on line %d
NULL
-- Iteration 19 --

Warning: array_walk_recursive() expects parameter 2 to be a valid callback, no array or string given in %s on line %d
NULL

Warning: array_walk_recursive() expects parameter 2 to be a valid callback, no array or string given in %s on line %d
NULL
-- Iteration 20 --

Warning: array_walk_recursive() expects parameter 2 to be a valid callback, function '' not found or invalid function name in %s on line %d
NULL

Warning: array_walk_recursive() expects parameter 2 to be a valid callback, function '' not found or invalid function name in %s on line %d
NULL
-- Iteration 21 --

Warning: array_walk_recursive() expects parameter 2 to be a valid callback, function '' not found or invalid function name in %s on line %d
NULL

Warning: array_walk_recursive() expects parameter 2 to be a valid callback, function '' not found or invalid function name in %s on line %d
NULL
-- Iteration 22 --

Warning: array_walk_recursive() expects parameter 2 to be a valid callback, no array or string given in %s on line %d
NULL

Warning: array_walk_recursive() expects parameter 2 to be a valid callback, no array or string given in %s on line %d
NULL
-- Iteration 23 --

Warning: array_walk_recursive() expects parameter 2 to be a valid callback, no array or string given in %s on line %d
NULL

Warning: array_walk_recursive() expects parameter 2 to be a valid callback, no array or string given in %s on line %d
NULL
-- Iteration 24 --

Warning: array_walk_recursive() expects parameter 2 to be a valid callback, no array or string given in %s on line %d
NULL

Warning: array_walk_recursive() expects parameter 2 to be a valid callback, no array or string given in %s on line %d
NULL
-- Iteration 25 --

Warning: array_walk_recursive() expects parameter 2 to be a valid callback, no array or string given in %s on line %d
NULL

Warning: array_walk_recursive() expects parameter 2 to be a valid callback, no array or string given in %s on line %d
NULL
Done
