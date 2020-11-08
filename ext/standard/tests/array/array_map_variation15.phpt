--TEST--
Test array_map() function : usage variations - non existent 'callback' function
--FILE--
<?php
/*
 * Test array_map() by passing non existent function for $callback argument
 */

echo "*** Testing array_map() : non existent 'callback' function ***\n";

// arrays to be passed as arguments
$arr1 = array(1, 2);
$arr2 = array("one", "two");
$arr3 = array(1.1, 2.2);

try {
    var_dump( array_map('non_existent', $arr1, $arr2, $arr3) );
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

echo "Done";
?>
--EXPECT--
*** Testing array_map() : non existent 'callback' function ***
array_map(): Argument #1 ($callback) must be a valid callback, function "non_existent" not found or invalid function name
Done
