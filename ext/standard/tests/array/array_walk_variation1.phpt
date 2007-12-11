--TEST--
Test array_walk() function : usage variations - unexpected values for 'input' argument 
--FILE--
<?php
/* Prototype  : bool array_walk(array $input, string $funcname [, mixed $userdata])
 * Description: Apply a user function to every member of an array 
 * Source code: ext/standard/array.c
*/

/*
 * Passing different scalar/nonscalar values in place of 'input' argument
*/

echo "*** Testing array_walk() : unexpected values for 'input' argument ***\n";

// callback function
/* Prototype : callback(mixed $value, mixed $key)
 * Parameters : $value - values given in input array
 *              $key - keys given in input array
 * Description : Function prints each element of an array with key
 */
function callback($value, $key)
{
  echo "key : ";
  var_dump($key);
  echo "value : ";
  var_dump($value);
}

// extra parameter passed to array_walk()
$user_data = 10;

//get an unset variable
$unset_var = 10;
unset ($unset_var);

// get resource variable
$fp = fopen(__FILE__, 'r');

// different scalar/nonscalar values to be used in place of an 'input' argument
$input_values = array(

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

         // null data
/* 10*/  NULL,
         null,

         // boolean data
/* 12*/  true,
         false,
         TRUE,
         FALSE,

         // empty data
/* 16*/  "",
         '', 

         // string data
/* 18*/  "string",
         'string',

         // resource data
         $fp, 

         // undefined data
         @$undefined_var,

         // unset data
/* 22*/  @$unset_var,
);


for($count = 0; $count < count($input_values); $count++) {
  echo "-- Iteration ".($count + 1)." --\n";
  var_dump( array_walk($input_values[$count], "callback") );
  var_dump( array_walk($input_values[$count], "callback", $user_data) );
}

fclose($fp);
echo "Done"
?>
--EXPECTF--
*** Testing array_walk() : unexpected values for 'input' argument ***
-- Iteration 1 --

Warning: array_walk(): The argument should be an array in %s on line %d
bool(false)

Warning: array_walk(): The argument should be an array in %s on line %d
bool(false)
-- Iteration 2 --

Warning: array_walk(): The argument should be an array in %s on line %d
bool(false)

Warning: array_walk(): The argument should be an array in %s on line %d
bool(false)
-- Iteration 3 --

Warning: array_walk(): The argument should be an array in %s on line %d
bool(false)

Warning: array_walk(): The argument should be an array in %s on line %d
bool(false)
-- Iteration 4 --

Warning: array_walk(): The argument should be an array in %s on line %d
bool(false)

Warning: array_walk(): The argument should be an array in %s on line %d
bool(false)
-- Iteration 5 --

Warning: array_walk(): The argument should be an array in %s on line %d
bool(false)

Warning: array_walk(): The argument should be an array in %s on line %d
bool(false)
-- Iteration 6 --

Warning: array_walk(): The argument should be an array in %s on line %d
bool(false)

Warning: array_walk(): The argument should be an array in %s on line %d
bool(false)
-- Iteration 7 --

Warning: array_walk(): The argument should be an array in %s on line %d
bool(false)

Warning: array_walk(): The argument should be an array in %s on line %d
bool(false)
-- Iteration 8 --

Warning: array_walk(): The argument should be an array in %s on line %d
bool(false)

Warning: array_walk(): The argument should be an array in %s on line %d
bool(false)
-- Iteration 9 --

Warning: array_walk(): The argument should be an array in %s on line %d
bool(false)

Warning: array_walk(): The argument should be an array in %s on line %d
bool(false)
-- Iteration 10 --

Warning: array_walk(): The argument should be an array in %s on line %d
bool(false)

Warning: array_walk(): The argument should be an array in %s on line %d
bool(false)
-- Iteration 11 --

Warning: array_walk(): The argument should be an array in %s on line %d
bool(false)

Warning: array_walk(): The argument should be an array in %s on line %d
bool(false)
-- Iteration 12 --

Warning: array_walk(): The argument should be an array in %s on line %d
bool(false)

Warning: array_walk(): The argument should be an array in %s on line %d
bool(false)
-- Iteration 13 --

Warning: array_walk(): The argument should be an array in %s on line %d
bool(false)

Warning: array_walk(): The argument should be an array in %s on line %d
bool(false)
-- Iteration 14 --

Warning: array_walk(): The argument should be an array in %s on line %d
bool(false)

Warning: array_walk(): The argument should be an array in %s on line %d
bool(false)
-- Iteration 15 --

Warning: array_walk(): The argument should be an array in %s on line %d
bool(false)

Warning: array_walk(): The argument should be an array in %s on line %d
bool(false)
-- Iteration 16 --

Warning: array_walk(): The argument should be an array in %s on line %d
bool(false)

Warning: array_walk(): The argument should be an array in %s on line %d
bool(false)
-- Iteration 17 --

Warning: array_walk(): The argument should be an array in %s on line %d
bool(false)

Warning: array_walk(): The argument should be an array in %s on line %d
bool(false)
-- Iteration 18 --

Warning: array_walk(): The argument should be an array in %s on line %d
bool(false)

Warning: array_walk(): The argument should be an array in %s on line %d
bool(false)
-- Iteration 19 --

Warning: array_walk(): The argument should be an array in %s on line %d
bool(false)

Warning: array_walk(): The argument should be an array in %s on line %d
bool(false)
-- Iteration 20 --

Warning: array_walk(): The argument should be an array in %s on line %d
bool(false)

Warning: array_walk(): The argument should be an array in %s on line %d
bool(false)
-- Iteration 21 --

Warning: array_walk(): The argument should be an array in %s on line %d
bool(false)

Warning: array_walk(): The argument should be an array in %s on line %d
bool(false)
-- Iteration 22 --

Warning: array_walk(): The argument should be an array in %s on line %d
bool(false)

Warning: array_walk(): The argument should be an array in %s on line %d
bool(false)
Done
