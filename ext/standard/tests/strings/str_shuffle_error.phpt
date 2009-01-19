--TEST--
Test str_shuffle() function : error conditions 
--FILE--
<?php

/* Prototype  : string str_shuffle  ( string $str  )
 * Description: Randomly shuffles a string
 * Source code: ext/standard/string.c
*/
echo "*** Testing str_shuffle() : error conditions ***\n";

echo "\n-- Testing str_shuffle() function with no arguments --\n";
var_dump( str_shuffle() );

echo "\n-- Testing str_shuffle() function with more than expected no. of arguments --\n";
$extra_arg = 10;
var_dump( str_shuffle("Hello World", $extra_arg) );

?>
===DONE===
--EXPECTF--
*** Testing str_shuffle() : error conditions ***

-- Testing str_shuffle() function with no arguments --

Warning: str_shuffle() expects exactly 1 parameter, 0 given in %s on line %d
NULL

-- Testing str_shuffle() function with more than expected no. of arguments --

Warning: str_shuffle() expects exactly 1 parameter, 2 given in %s on line %d
NULL
===DONE===