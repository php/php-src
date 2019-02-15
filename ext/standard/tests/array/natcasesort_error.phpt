--TEST--
Test natcasesort() function : error conditions - Pass incorrect number of args
--FILE--
<?php
/* Prototype  : bool natcasesort(array &$array_arg)
 * Description: Sort an array using case-insensitive natural sort
 * Source code: ext/standard/array.c
 */

/*
 * Pass incorrect number of arguments to natcasesort() to test behaviour
 */

echo "*** Testing natcasesort() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing natcasesort() function with Zero arguments --\n";
var_dump( natcasesort() );

// Test natcasesort with one more than the expected number of arguments
echo "\n-- Testing natcasesort() function with more than expected no. of arguments --\n";
$array_arg = array(1, 2);
$extra_arg = 10;
var_dump( natcasesort($array_arg, $extra_arg) );

echo "Done";
?>
--EXPECTF--
*** Testing natcasesort() : error conditions ***

-- Testing natcasesort() function with Zero arguments --

Warning: natcasesort() expects exactly 1 parameter, 0 given in %s on line %d
NULL

-- Testing natcasesort() function with more than expected no. of arguments --

Warning: natcasesort() expects exactly 1 parameter, 2 given in %s on line %d
NULL
Done
