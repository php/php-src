--TEST--
Test array_intersect() function : error conditions
--FILE--
<?php
/* Prototype  : array array_intersect(array $arr1, array $arr2 [, array $...])
 * Description: Returns the entries of arr1 that have values which are present in all the other arguments
 * Source code: ext/standard/array.c
*/

echo "*** Testing array_intersect() : error conditions ***\n";

// Testing array_intersect() with zero arguments
echo "\n-- Testing array_intersect() function with Zero arguments --\n";
try {
    var_dump( array_intersect() );
} catch (ArgumentCountError $e) {
    echo $e->getMessage(), "\n";
}

// Testing array_intersect() with one less than the expected number of arguments
echo "\n-- Testing array_intersect() function with less than expected no. of arguments --\n";
$arr1 = array(1, 2);
try {
    var_dump( array_intersect($arr1) );
} catch (ArgumentCountError $e) {
    echo $e->getMessage(), "\n";
}

echo "Done";
?>
--EXPECT--
*** Testing array_intersect() : error conditions ***

-- Testing array_intersect() function with Zero arguments --
At least 2 parameters are required, 0 given

-- Testing array_intersect() function with less than expected no. of arguments --
At least 2 parameters are required, 1 given
Done
