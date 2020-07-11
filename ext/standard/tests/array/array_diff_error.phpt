--TEST--
Test array_diff() function : error conditions - too few arguments passed to function
--FILE--
<?php
/*
 * Test array_diff with less than the expected number of arguments
 */

echo "*** Testing array_diff() : error conditions ***\n";
// Zero arguments
echo "\n-- Testing array_diff() function with zero arguments --\n";
try {
    var_dump( array_diff() );
} catch (ArgumentCountError $e) {
    echo $e->getMessage(), "\n";
}


// Testing array_diff with one less than the expected number of arguments
echo "\n-- Testing array_diff() function with less than expected no. of arguments --\n";
$arr1 = array(1, 2);
try {
    var_dump( array_diff($arr1) );
} catch (ArgumentCountError $e) {
    echo $e->getMessage(), "\n";
}

echo "Done";
?>
--EXPECT--
*** Testing array_diff() : error conditions ***

-- Testing array_diff() function with zero arguments --
At least 2 parameters are required, 0 given

-- Testing array_diff() function with less than expected no. of arguments --
At least 2 parameters are required, 1 given
Done
