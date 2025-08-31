--TEST--
Test array_walk_recursive() function : usage variations - buit-in function as callback
--FILE--
<?php
/*
 * Passing different buit-in functionns as callback function
 *    pow function
 *    min function
 *    echo language construct
*/

echo "*** Testing array_walk_recursive() : built-in function as callback ***\n";

$input = array(array(2 => 1, 65), array(98, 100), array(6 => -4));

echo "-- With 'pow' built-in function --\n";
var_dump( array_walk_recursive($input, 'pow'));

echo "-- With 'min' built-in function --\n";
var_dump( array_walk_recursive($input, "min"));

echo "-- With 'echo' language construct --\n";
try {
    var_dump( array_walk_recursive($input, "echo"));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

echo "Done"
?>
--EXPECT--
*** Testing array_walk_recursive() : built-in function as callback ***
-- With 'pow' built-in function --
bool(true)
-- With 'min' built-in function --
bool(true)
-- With 'echo' language construct --
array_walk_recursive(): Argument #2 ($callback) must be a valid callback, function "echo" not found or invalid function name
Done
