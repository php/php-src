--TEST--
Test array_diff_key() function : error conditions
--FILE--
<?php
echo "*** Testing array_diff_key() : error conditions ***\n";

// Initialise the variables
$array1 = array('blue' => 1, 'red' => 2, 'green' => 3, 'purple' => 4);

// Testing array_diff_key with one less than the expected number of arguments
echo "\n-- Testing array_diff_key() function with less than expected no. of arguments --\n";
try {
    var_dump( array_diff_key($array1) );
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

// Testing array_diff_key with no arguments
echo "\n-- Testing array_diff_key() function with no arguments --\n";
try {
    var_dump( array_diff_key() );
} catch (ArgumentCountError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
*** Testing array_diff_key() : error conditions ***

-- Testing array_diff_key() function with less than expected no. of arguments --
At least 2 parameters are required, 1 given

-- Testing array_diff_key() function with no arguments --
At least 2 parameters are required, 0 given
