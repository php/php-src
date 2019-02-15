--TEST--
Test array_unshift() function : error conditions
--FILE--
<?php
/* Prototype  : int array_unshift(array $array[, mixed ...])
 * Description: Pushes elements onto the beginning of the array
 * Source code: ext/standard/array.c
*/

echo "*** Testing array_unshift() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing array_unshift() function with Zero arguments --\n";
var_dump( array_unshift() );
echo "Done";
?>
--EXPECTF--
*** Testing array_unshift() : error conditions ***

-- Testing array_unshift() function with Zero arguments --

Warning: array_unshift() expects at least 1 parameter, 0 given in %s on line %d
NULL
Done
