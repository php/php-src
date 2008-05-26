--TEST--
Test current() function : error conditions - Pass incorrect number of args
--FILE--
<?php
/* Prototype  : mixed current(array $array_arg)
 * Description: Return the element currently pointed to by the internal array pointer
 * Source code: ext/standard/array.c
 * Alias to functions: pos
 */

/*
 * Pass incorrect number of arguments to current() to test behaviour
 */

echo "*** Testing current() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing current() function with Zero arguments --\n";
var_dump( current() );

//Test current with one more than the expected number of arguments
echo "\n-- Testing current() function with more than expected no. of arguments --\n";
$array_arg = array(1, 2);
$extra_arg = 10;
var_dump( current($array_arg, $extra_arg) );
?>
===DONE===
--EXPECTF--
*** Testing current() : error conditions ***

-- Testing current() function with Zero arguments --

Warning: current() expects exactly 1 parameter, 0 given in %s on line %d
NULL

-- Testing current() function with more than expected no. of arguments --

Warning: current() expects exactly 1 parameter, 2 given in %s on line %d
NULL
===DONE===
