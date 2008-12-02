--TEST--
Test array_udiff_assoc() function : error conditions 
--FILE--
<?php
/* Prototype  : array array_udiff_assoc(array arr1, array arr2 [, array ...], callback key_comp_func)
 * Description: Returns the entries of arr1 that have values which are not present in any of the others arguments but do additional checks whether the keys are equal. Keys are compared by user supplied function. 
 * Source code: ext/standard/array.c
 * Alias to functions: 
 */

echo "*** Testing array_udiff_assoc() : error conditions ***\n";

$arr1 = array(1, 2);
$arr2 = array(1, 2);
$extra_arg = 10;
include('compare_function.inc');
$key_comp_func = 'compare_function';


//Test array_udiff_assoc with one more than the expected number of arguments
echo "\n-- Testing array_udiff_assoc() function with more than expected no. of arguments --\n";
var_dump( array_udiff_assoc($arr1, $arr2, $key_comp_func, $extra_arg) );

// Testing array_udiff_assoc with one less than the expected number of arguments
echo "\n-- Testing array_udiff_assoc() function with less than expected no. of arguments --\n";
var_dump( array_udiff_assoc($arr1, $arr2) );

?>
===DONE===
--EXPECTF--
*** Testing array_udiff_assoc() : error conditions ***

-- Testing array_udiff_assoc() function with more than expected no. of arguments --

Warning: array_udiff_assoc(): Not a valid callback 10 in %s on line %d
NULL

-- Testing array_udiff_assoc() function with less than expected no. of arguments --

Warning: Wrong parameter count for array_udiff_assoc() in %s on line %d
NULL
===DONE===

