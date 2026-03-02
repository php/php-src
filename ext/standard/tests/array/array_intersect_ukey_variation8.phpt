--TEST--
Test array_intersect_ukey() function : usage variation - Passing non-existing function name to callback
--FILE--
<?php
echo "*** Testing array_intersect_ukey() : usage variation ***\n";

//Initialise arguments
$array1 = array('blue'  => 1, 'red'  => 2, 'green'  => 3, 'purple' => 4);
$array2 = array('green' => 5, 'blue' => 6, 'yellow' => 7, 'cyan'   => 8);

//function name within double quotes
try {
    var_dump( array_intersect_ukey($array1, $array2, "unknown_function") );
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

//function name within single quotes
try {
    var_dump( array_intersect_ukey($array1, $array2, 'unknown_function') );
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
*** Testing array_intersect_ukey() : usage variation ***
array_intersect_ukey(): Argument #3 must be a valid callback, function "unknown_function" not found or invalid function name
array_intersect_ukey(): Argument #3 must be a valid callback, function "unknown_function" not found or invalid function name
