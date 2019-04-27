--TEST--
Test array_merge_recursive() function : error conditions
--FILE--
<?php
/* Prototype  : array array_merge_recursive(array $arr1[, array $...])
 * Description: Recursively merges elements from passed arrays into one array
 * Source code: ext/standard/array.c
 */

echo "*** Testing array_merge_recursive() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing array_merge_recursive() function with Zero arguments --\n";
try {
    var_dump( array_merge_recursive() );
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

echo "Done";
?>
--EXPECTF--
*** Testing array_merge_recursive() : error conditions ***

-- Testing array_merge_recursive() function with Zero arguments --
array_merge_recursive() expects at least 1 parameter, 0 given
Done
