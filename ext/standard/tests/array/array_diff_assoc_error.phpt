--TEST--
Test array_diff_assoc() function : error conditions - pass array_diff_assoc() too few/zero arguments
--FILE--
<?php
/* Prototype  : array array_diff_assoc(array $arr1, array $arr2 [, array ...])
 * Description: Returns the entries of arr1 that have values which are not present 
 * in any of the others arguments but do additional checks whether the keys are equal 
 * Source code: ext/standard/array.c
 */

/*
 * Test errors for array_diff with too few\zero arguments
 */

echo "*** Testing array_diff_assoc() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing array_diff_assoc() function with zero arguments --\n";
var_dump( array_diff_assoc() );

// Testing array_diff_assoc with one less than the expected number of arguments
echo "\n-- Testing array_diff_assoc() function with less than expected no. of arguments --\n";
$arr1 = array(1, 2);
var_dump( array_diff_assoc($arr1) );


echo "Done";
?>
--EXPECTF--
*** Testing array_diff_assoc() : error conditions ***

-- Testing array_diff_assoc() function with zero arguments --

Warning: Wrong parameter count for array_diff_assoc() in %s on line %d
NULL

-- Testing array_diff_assoc() function with less than expected no. of arguments --

Warning: Wrong parameter count for array_diff_assoc() in %s on line %d
NULL
Done

