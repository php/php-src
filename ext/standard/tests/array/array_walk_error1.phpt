--TEST--
Test array_walk() function : error conditions
--FILE--
<?php
/* Prototype  : bool array_walk(array $input, string $funcname [, mixed $userdata])
 * Description: Apply a user function to every member of an array 
 * Source code: ext/standard/array.c
*/

$input = array(1, 2);

/* Prototype : callback(mixed value, mixed key, mixed user_data)
 * Parameters : value - value in key/value pair
 *              key - key in key/value pair
 *              user_data - extra parameter
 */
function callback ($value, $key, $user_data) {
  echo "\ncallback() invoked \n";
}

echo "*** Testing array_walk() : error conditions ***\n";

echo "-- Testing array_walk() function with zero arguments --\n";
var_dump( array_walk() );

echo "-- Testing array_walk() function with one argument --\n";
var_dump( array_walk($input) );

echo "-- Testing array_walk() function with non existent callback function  --\n";
var_dump( array_walk($input, "non_existent") );

echo "Done";
?>
--EXPECTF--
*** Testing array_walk() : error conditions ***
-- Testing array_walk() function with zero arguments --

Warning: array_walk() expects at least 2 parameters, 0 given in %s on line %d
NULL
-- Testing array_walk() function with one argument --

Warning: array_walk() expects at least 2 parameters, 1 given in %s on line %d
NULL
-- Testing array_walk() function with non existent callback function  --

Warning: array_walk(): Unable to call non_existent() - function does not exist in %s on line %d
bool(true)
Done
