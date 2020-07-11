--TEST--
Test array_diff_assoc() function : error conditions - pass array_diff_assoc() too few/zero arguments
--FILE--
<?php
/*
 * Test errors for array_diff with too few\zero arguments
 */

echo "*** Testing array_diff_assoc() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing array_diff_assoc() function with zero arguments --\n";
try {
    var_dump( array_diff_assoc() );
} catch (ArgumentCountError $e) {
    echo $e->getMessage(), "\n";
}

// Testing array_diff_assoc with one less than the expected number of arguments
echo "\n-- Testing array_diff_assoc() function with less than expected no. of arguments --\n";
$arr1 = array(1, 2);
try {
    var_dump( array_diff_assoc($arr1) );
} catch (ArgumentCountError $e) {
    echo $e->getMessage(), "\n";
}

echo "Done";
?>
--EXPECT--
*** Testing array_diff_assoc() : error conditions ***

-- Testing array_diff_assoc() function with zero arguments --
At least 2 parameters are required, 0 given

-- Testing array_diff_assoc() function with less than expected no. of arguments --
At least 2 parameters are required, 1 given
Done
