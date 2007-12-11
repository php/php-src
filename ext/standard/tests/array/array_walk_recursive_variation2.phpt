--TEST--
Test array_walk_recursive() function : usage variations - unexpected values in place of 'funcname' argument(Bug#43543)
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

// get resource variable
$fp = fopen(__FILE__, 'r');

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
/* 23*/  $fp,

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

Warning: array_walk_recursive(): Wrong syntax for function name in %s on line %d
bool(false)

Warning: array_walk_recursive(): Wrong syntax for function name in %s on line %d
bool(false)
-- Iteration 2 --

Warning: array_walk_recursive(): Wrong syntax for function name in %s on line %d
bool(false)

Warning: array_walk_recursive(): Wrong syntax for function name in %s on line %d
bool(false)
-- Iteration 3 --

Warning: array_walk_recursive(): Wrong syntax for function name in %s on line %d
bool(false)

Warning: array_walk_recursive(): Wrong syntax for function name in %s on line %d
bool(false)
-- Iteration 4 --

Warning: array_walk_recursive(): Wrong syntax for function name in %s on line %d
bool(false)

Warning: array_walk_recursive(): Wrong syntax for function name in %s on line %d
bool(false)
-- Iteration 5 --

Warning: array_walk_recursive(): Wrong syntax for function name in %s on line %d
bool(false)

Warning: array_walk_recursive(): Wrong syntax for function name in %s on line %d
bool(false)
-- Iteration 6 --

Warning: array_walk_recursive(): Wrong syntax for function name in %s on line %d
bool(false)

Warning: array_walk_recursive(): Wrong syntax for function name in %s on line %d
bool(false)
-- Iteration 7 --

Warning: array_walk_recursive(): Wrong syntax for function name in %s on line %d
bool(false)

Warning: array_walk_recursive(): Wrong syntax for function name in %s on line %d
bool(false)
-- Iteration 8 --

Warning: array_walk_recursive(): Wrong syntax for function name in %s on line %d
bool(false)

Warning: array_walk_recursive(): Wrong syntax for function name in %s on line %d
bool(false)
-- Iteration 9 --

Warning: array_walk_recursive(): Wrong syntax for function name in %s on line %d
bool(false)

Warning: array_walk_recursive(): Wrong syntax for function name in %s on line %d
bool(false)
-- Iteration 10 --

Warning: array_walk_recursive(): Unable to call Array() - function does not exist in %s on line %d
bool(true)

Warning: array_walk_recursive(): Unable to call Array() - function does not exist in %s on line %d
bool(true)
-- Iteration 11 --

Warning: array_walk_recursive(): Unable to call Array() - function does not exist in %s on line %d
bool(true)

Warning: array_walk_recursive(): Unable to call Array() - function does not exist in %s on line %d
bool(true)
-- Iteration 12 --

Warning: array_walk_recursive(): Unable to call Array() - function does not exist in %s on line %d
bool(true)

Warning: array_walk_recursive(): Unable to call Array() - function does not exist in %s on line %d
bool(true)
-- Iteration 13 --

Warning: array_walk_recursive(): Unable to call Array() - function does not exist in %s on line %d
bool(true)

Warning: array_walk_recursive(): Unable to call Array() - function does not exist in %s on line %d
bool(true)
-- Iteration 14 --

Warning: array_walk_recursive(): Wrong syntax for function name in %s on line %d
bool(false)

Warning: array_walk_recursive(): Wrong syntax for function name in %s on line %d
bool(false)
-- Iteration 15 --

Warning: array_walk_recursive(): Wrong syntax for function name in %s on line %d
bool(false)

Warning: array_walk_recursive(): Wrong syntax for function name in %s on line %d
bool(false)
-- Iteration 16 --

Warning: array_walk_recursive(): Wrong syntax for function name in %s on line %d
bool(false)

Warning: array_walk_recursive(): Wrong syntax for function name in %s on line %d
bool(false)
-- Iteration 17 --

Warning: array_walk_recursive(): Wrong syntax for function name in %s on line %d
bool(false)

Warning: array_walk_recursive(): Wrong syntax for function name in %s on line %d
bool(false)
-- Iteration 18 --

Warning: array_walk_recursive(): Wrong syntax for function name in %s on line %d
bool(false)

Warning: array_walk_recursive(): Wrong syntax for function name in %s on line %d
bool(false)
-- Iteration 19 --

Warning: array_walk_recursive(): Wrong syntax for function name in %s on line %d
bool(false)

Warning: array_walk_recursive(): Wrong syntax for function name in %s on line %d
bool(false)
-- Iteration 20 --

Warning: array_walk_recursive(): Unable to call () - function does not exist in %s on line %d
bool(true)

Warning: array_walk_recursive(): Unable to call () - function does not exist in %s on line %d
bool(true)
-- Iteration 21 --

Warning: array_walk_recursive(): Unable to call () - function does not exist in %s on line %d
bool(true)

Warning: array_walk_recursive(): Unable to call () - function does not exist in %s on line %d
bool(true)
-- Iteration 22 --

Warning: array_walk_recursive(): Wrong syntax for function name in %s on line %d
bool(false)

Warning: array_walk_recursive(): Wrong syntax for function name in %s on line %d
bool(false)
-- Iteration 23 --

Warning: array_walk_recursive(): Wrong syntax for function name in %s on line %d
bool(false)

Warning: array_walk_recursive(): Wrong syntax for function name in %s on line %d
bool(false)
-- Iteration 24 --

Warning: array_walk_recursive(): Wrong syntax for function name in %s on line %d
bool(false)

Warning: array_walk_recursive(): Wrong syntax for function name in %s on line %d
bool(false)
-- Iteration 25 --

Warning: array_walk_recursive(): Wrong syntax for function name in %s on line %d
bool(false)

Warning: array_walk_recursive(): Wrong syntax for function name in %s on line %d
bool(false)
Done
