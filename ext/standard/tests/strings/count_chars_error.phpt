--TEST--
Test count_chars() function : error conditions
--FILE--
<?php

/* Prototype  : mixed count_chars  ( string $string  [, int $mode  ] )
 * Description: Return information about characters used in a string
 * Source code: ext/standard/string.c
*/

echo "*** Testing count_chars() : error conditions ***\n";

echo "\n-- Testing count_chars() function with no arguments --\n";
var_dump( count_chars() );

echo "\n-- Testing count_chars() function with more than expected no. of arguments --\n";
$string = "Hello World\n"; 
$mode = 1;
$extra_arg = 10;
var_dump( count_chars($string, $mode, $extra_arg) );

?>
===DONE===
--EXPECTF--
*** Testing count_chars() : error conditions ***

-- Testing count_chars() function with no arguments --

Warning: count_chars() expects at least 1 parameter, 0 given in %s on line %d
NULL

-- Testing count_chars() function with more than expected no. of arguments --

Warning: count_chars() expects at most 2 parameters, 3 given in %s on line %d
NULL
===DONE===
