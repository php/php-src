--TEST--
Test array_diff() function : error conditions - too few arguments passed to function
--FILE--
<?php
/* Prototype  : array array_diff(array $arr1, array $arr2 [, array ...])
 * Description: Returns the entries of $arr1 that have values which are
 * not present in any of the others arguments.
 * Source code: ext/standard/array.c
 */

/*
 * Test array_diff with less than the expected number of arguments
 */

echo "*** Testing array_diff() : error conditions ***\n";
// Zero arguments
echo "\n-- Testing array_diff() function with zero arguments --\n";
var_dump( array_diff() );


// Testing array_diff with one less than the expected number of arguments
echo "\n-- Testing array_diff() function with less than expected no. of arguments --\n";
$arr1 = array(1, 2);
var_dump( array_diff($arr1) );

echo "Done";
?>
--EXPECTF--
*** Testing array_diff() : error conditions ***

-- Testing array_diff() function with zero arguments --

Warning: array_diff(): at least 2 parameters are required, 0 given in %s on line %d
NULL

-- Testing array_diff() function with less than expected no. of arguments --

Warning: array_diff(): at least 2 parameters are required, 1 given in %s on line %d
NULL
Done
