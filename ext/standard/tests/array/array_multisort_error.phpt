--TEST--
Test array_multisort() function : error conditions 
--FILE--
<?php
/* Prototype  : bool array_multisort(array ar1 [, SORT_ASC|SORT_DESC [, SORT_REGULAR|SORT_NUMERIC|SORT_STRING]] [, array ar2 [, SORT_ASC|SORT_DESC [, SORT_REGULAR|SORT_NUMERIC|SORT_STRING]], ...])
 * Description: Sort multiple arrays at once similar to how ORDER BY clause works in SQL 
 * Source code: ext/standard/array.c
 * Alias to functions: 
 */

echo "*** Testing array_multisort() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing array_multisort() function with Zero arguments --\n";
var_dump( array_multisort() );

echo "\n-- Testing array_multisort() function with repeated flags --\n";
$ar1 = array(1);
var_dump( array_multisort($ar1, SORT_ASC, SORT_ASC) );

echo "\n-- Testing array_multisort() function with repeated flags --\n";
$ar1 = array(1);
var_dump( array_multisort($ar1, SORT_STRING, SORT_NUMERIC) );

?>
===DONE===
--EXPECTF--
*** Testing array_multisort() : error conditions ***

-- Testing array_multisort() function with Zero arguments --

Warning: array_multisort() expects at least 1 parameter, 0 given in %sarray_multisort_error.php on line %d
NULL

-- Testing array_multisort() function with repeated flags --

Warning: array_multisort(): Argument #3 is expected to be an array or sorting flag that has not already been specified in %sarray_multisort_error.php on line %d
bool(false)

-- Testing array_multisort() function with repeated flags --

Warning: array_multisort(): Argument #3 is expected to be an array or sorting flag that has not already been specified in %sarray_multisort_error.php on line %d
bool(false)
===DONE===