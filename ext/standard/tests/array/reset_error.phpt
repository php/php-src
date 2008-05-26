--TEST--
Test reset() function : error conditions - Pass incorrect number of args
--FILE--
<?php
/* Prototype  : mixed reset(array $array_arg)
 * Description: Set array argument's internal pointer to the first element and return it 
 * Source code: ext/standard/array.c
 */

/*
 * Pass incorrect number of arguments to reset() to test behaviour
 */

echo "*** Testing reset() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing reset() function with Zero arguments --\n";
var_dump( reset() );

//Test reset with one more than the expected number of arguments
echo "\n-- Testing reset() function with more than expected no. of arguments --\n";
$array_arg = array(1, 2);
$extra_arg = 10;
var_dump( reset($array_arg, $extra_arg) );
?>
===DONE===
--EXPECTF--
*** Testing reset() : error conditions ***

-- Testing reset() function with Zero arguments --

Warning: reset() expects exactly 1 parameter, 0 given in %s on line %d
NULL

-- Testing reset() function with more than expected no. of arguments --

Warning: reset() expects exactly 1 parameter, 2 given in %s on line %d
NULL
===DONE===
