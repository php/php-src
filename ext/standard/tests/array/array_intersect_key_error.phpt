--TEST--
Test array_intersect_key() function : error conditions
--FILE--
<?php
/* Prototype  : array array_intersect_key(array arr1, array arr2 [, array ...])
 * Description: Returns the entries of arr1 that have keys which are present in all the other arguments.
 * Source code: ext/standard/array.c
 */

echo "*** Testing array_intersect_key() : error conditions ***\n";

//Initialise function arguments
$array1 = array('blue' => 1, 'red' => 2, 'green' => 3, 'purple' => 4);

// Testing array_intersect_key with one less than the expected number of arguments
echo "\n-- Testing array_intersect_key() function with less than expected no. of arguments --\n";
try {
    var_dump( array_intersect_key($array1) );
} catch (ArgumentCountError $e) {
    echo $e->getMessage(), "\n";
}

// Testing array_intersect_key with one less than the expected number of arguments
echo "\n-- Testing array_intersect_key() function with no arguments --\n";
try {
    var_dump( array_intersect_key() );
} catch (ArgumentCountError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
*** Testing array_intersect_key() : error conditions ***

-- Testing array_intersect_key() function with less than expected no. of arguments --
At least 2 parameters are required, 1 given

-- Testing array_intersect_key() function with no arguments --
At least 2 parameters are required, 0 given
