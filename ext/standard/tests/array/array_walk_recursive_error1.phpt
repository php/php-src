--TEST--
Test array_walk_recursive() function : error conditions
--FILE--
<?php
/* Prototype  : bool array_walk_recursive(array $input, string $funcname [, mixed $userdata])
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

echo "*** Testing array_walk_recursive() : error conditions ***\n";

echo "-- Testing array_walk_recursive() function with zero arguments --\n";
var_dump( array_walk_recursive() );

echo "-- Testing array_walk_recursive() function with one argument --\n";
var_dump( array_walk_recursive($input) );

$input = array( array(1, 2), array(3), array(4, 5));
echo "-- Testing array_walk_recursive() function with non existent callback function  --\n";
var_dump( array_walk_recursive($input, "non_existent") );

echo "Done";
?>
--EXPECTF--
*** Testing array_walk_recursive() : error conditions ***
-- Testing array_walk_recursive() function with zero arguments --

Warning: array_walk_recursive() expects at least 2 parameters, 0 given in %s on line %d
NULL
-- Testing array_walk_recursive() function with one argument --

Warning: array_walk_recursive() expects at least 2 parameters, 1 given in %s on line %d
NULL
-- Testing array_walk_recursive() function with non existent callback function  --

Warning: array_walk_recursive() expects parameter 2 to be a valid callback, function 'non_existent' not found or invalid function name in %s on line %d
NULL
Done
