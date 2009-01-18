--TEST--
Test hebrev() function : error conditions 
--FILE--
<?php

/* Prototype  : string hebrev  ( string $hebrew_text  [, int $max_chars_per_line  ] )
 * Description: Convert logical Hebrew text to visual text
 * Source code: ext/standard/string.c
*/

echo "*** Testing hebrev() : error conditions ***\n";

echo "\n-- Testing hebrev() function with no arguments --\n";
var_dump( hebrev() );

echo "\n-- Testing hebrev() function with more than expected no. of arguments --\n";
$extra_arg = 10;
var_dump( hebrev("Hello World", 5, $extra_arg) );

?> 
===DONE===
--EXPECTF--
*** Testing hebrev() : error conditions ***

-- Testing hebrev() function with no arguments --

Warning: hebrev() expects at least 1 parameter, 0 given in %s on line %d
NULL

-- Testing hebrev() function with more than expected no. of arguments --

Warning: hebrev() expects at most 2 parameters, 3 given in %s on line %d
NULL
 
===DONE===