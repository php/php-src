--TEST--
Test array_walk_recursive() function : usage variations - buit-in function as callback
--FILE--
<?php
/* Prototype  : bool array_walk_recursive(array $input, string $funcname [, mixed $userdata])
 * Description: Apply a user function to every member of an array 
 * Source code: ext/standard/array.c
*/

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
var_dump( array_walk_recursive($input, "echo"));

echo "Done"
?>
--EXPECTF--
*** Testing array_walk_recursive() : built-in function as callback ***
-- With 'pow' built-in function --
bool(true)
-- With 'min' built-in function --
bool(true)
-- With 'echo' language construct --

Warning: array_walk_recursive() expects parameter 2 to be a valid callback, function 'echo' not found or invalid function name in %s on line %d
NULL
Done
