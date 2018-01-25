--TEST--
Test array_intersect_assoc() function : error conditions(Bug#43197)
--FILE--
<?php
/* Prototype  : array array_intersect_assoc(array $arr1, array $arr2 [, array $...])
 * Description: Returns the entries of arr1 that have values which are present in all the other arguments.
 * Keys are used to do more restrictive check
 * Source code: ext/standard/array.c
*/

echo "*** Testing array_intersect_assoc() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing array_intersect_assoc() function with Zero arguments --\n";
var_dump( array_intersect_assoc() );

// Testing array_intersect_assoc with one less than the expected number of arguments
echo "\n-- Testing array_intersect_assoc() function with less than expected no. of arguments --\n";
$arr1 = array(1, 2);
var_dump( array_intersect_assoc($arr1) );

echo "Done";
?>
--EXPECTF--
*** Testing array_intersect_assoc() : error conditions ***

-- Testing array_intersect_assoc() function with Zero arguments --

Warning: array_intersect_assoc(): at least 2 parameters are required, 0 given in %s on line %d
NULL

-- Testing array_intersect_assoc() function with less than expected no. of arguments --

Warning: array_intersect_assoc(): at least 2 parameters are required, 1 given in %s on line %d
NULL
Done
