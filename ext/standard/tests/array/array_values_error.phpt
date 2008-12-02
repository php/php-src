--TEST--
Test array_values() function : error conditions - Pass incorrect number of functions
--FILE--
<?php
/* Prototype  : array array_values(array $input)
 * Description: Return just the values from the input array 
 * Source code: ext/standard/array.c
 */

/*
 * Pass incorrect number of arguments to array_values to test behaviour
 */

echo "*** Testing array_values() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing array_values() function with Zero arguments --\n";
var_dump( array_values() );

//Test array_values with one more than the expected number of arguments
echo "\n-- Testing array_values() function with more than expected no. of arguments --\n";
$input = array(1, 2);
$extra_arg = 10;
var_dump( array_values($input, $extra_arg) );

echo "Done";
?>

--EXPECTF--
*** Testing array_values() : error conditions ***

-- Testing array_values() function with Zero arguments --

Warning: Wrong parameter count for array_values() in %s on line %d
NULL

-- Testing array_values() function with more than expected no. of arguments --

Warning: Wrong parameter count for array_values() in %s on line %d
NULL
Done