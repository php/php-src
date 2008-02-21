--TEST--
Test key() function : error conditions - Pass incorrect number of args
--FILE--
<?php
/* Prototype  : mixed key(array $array_arg)
 * Description: Return the key of the element currently pointed to by the internal array pointer
 * Source code: ext/standard/array.c
 */

/*
 * Pass incorrect number of arguments to key() to test behaviour
 */

echo "*** Testing key() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing key() function with Zero arguments --\n";
var_dump( key() );

//Test current with one more than the expected number of arguments
echo "\n-- Testing key() function with more than expected no. of arguments --\n";
$array_arg = array(1, 2);
$extra_arg = 10;
var_dump( key($array_arg, $extra_arg) );
?>
===DONE===
--EXPECTF--
*** Testing key() : error conditions ***

-- Testing key() function with Zero arguments --

Warning: Wrong parameter count for key() in %s on line %d
NULL

-- Testing key() function with more than expected no. of arguments --

Warning: Wrong parameter count for key() in %s on line %d
NULL
===DONE===
