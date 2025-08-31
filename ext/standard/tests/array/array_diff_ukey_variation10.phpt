--TEST--
Test array_diff_ukey() function : usage variation - Passing non-existing function name to callback
--FILE--
<?php
echo "*** Testing array_diff_ukey() : usage variation ***\n";

//Initialize variables
$array1 = array("a" => "green", "b" => "brown", "c" => "blue", "red");
$array2 = array("a" => "green", "yellow", "red");

//function name within double quotes
try {
    var_dump( array_diff_ukey($array1, $array1, "unknown_function") );
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

//function name within single quotes
try {
    var_dump( array_diff_ukey($array1, $array1, 'unknown_function') );
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
*** Testing array_diff_ukey() : usage variation ***
array_diff_ukey(): Argument #3 must be a valid callback, function "unknown_function" not found or invalid function name
array_diff_ukey(): Argument #3 must be a valid callback, function "unknown_function" not found or invalid function name
