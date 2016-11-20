--TEST--
Test str_rot13() function : error conditions
--FILE--
<?php
/* Prototype  : string str_rot13  ( string $str  )
 * Description: Perform the rot13 transform on a string
 * Source code: ext/standard/string.c
*/
echo "*** Testing str_rot13() : error conditions ***\n";

echo "-- Testing str_rot13() function with Zero arguments --\n";
var_dump( str_rot13() );

echo "\n\n-- Testing str_rot13() function with more than expected no. of arguments --\n";
$str = "str_rot13() tests starting";
$extra_arg = 10;
var_dump( str_rot13( $str, $extra_arg) );
?>
===DONE===
--EXPECTF--
*** Testing str_rot13() : error conditions ***
-- Testing str_rot13() function with Zero arguments --

Warning: str_rot13() expects exactly 1 parameter, 0 given in %s on line %d
NULL


-- Testing str_rot13() function with more than expected no. of arguments --

Warning: str_rot13() expects exactly 1 parameter, 2 given in %s on line %d
NULL
===DONE===