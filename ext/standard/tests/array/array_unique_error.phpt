--TEST--
Test array_unique() function : error conditions
--FILE--
<?php
/* Prototype  : array array_unique(array $input)
 * Description: Removes duplicate values from array
 * Source code: ext/standard/array.c
*/

echo "*** Testing array_unique() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing array_unique() function with zero arguments --\n";
var_dump( array_unique() );

//Test array_unique with one more than the expected number of arguments
echo "\n-- Testing array_unique() function with more than expected no. of arguments --\n";
$input = array(1, 2);
$extra_arg = 10;
var_dump( array_unique($input, SORT_NUMERIC, $extra_arg) );

echo "Done";
?>
--EXPECTF--
*** Testing array_unique() : error conditions ***

-- Testing array_unique() function with zero arguments --

Warning: array_unique() expects at least 1 parameter, 0 given in %s on line %d
NULL

-- Testing array_unique() function with more than expected no. of arguments --

Warning: array_unique() expects at most 2 parameters, 3 given in %s on line %d
NULL
Done
