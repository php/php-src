--TEST--
Test ltrim() function : error conditions
--FILE--
<?php

/* Prototype  : string ltrim  ( string $str  [, string $charlist  ] )
 * Description: Strip whitespace (or other characters) from the beginning of a string.
 * Source code: ext/standard/string.c
*/


echo "*** Testing ltrim() : error conditions ***\n";

echo "\n-- Testing ltrim() function with no arguments --\n";
var_dump( ltrim() );

echo "\n-- Testing ltrim() function with more than expected no. of arguments --\n";
$extra_arg = 10;
var_dump( ltrim("Hello World",  "Heo", $extra_arg) );


$hello = "  Hello World\n";
echo "\n-- Test ltrim function with various invalid charlists\n";
var_dump(ltrim($hello, "..a"));
var_dump(ltrim($hello, "a.."));
var_dump(ltrim($hello, "z..a"));
var_dump(ltrim($hello, "a..b..c"));

?>
===DONE===
--EXPECTF--
*** Testing ltrim() : error conditions ***

-- Testing ltrim() function with no arguments --

Warning: ltrim() expects at least 1 parameter, 0 given in %s on line %d
NULL

-- Testing ltrim() function with more than expected no. of arguments --

Warning: ltrim() expects at most 2 parameters, 3 given in %s on line %d
NULL

-- Test ltrim function with various invalid charlists

Warning: ltrim(): Invalid '..'-range, no character to the left of '..' in %s on line %d
string(14) "  Hello World
"

Warning: ltrim(): Invalid '..'-range, no character to the right of '..' in %s on line %d
string(14) "  Hello World
"

Warning: ltrim(): Invalid '..'-range, '..'-range needs to be incrementing in %s on line %d
string(14) "  Hello World
"

Warning: ltrim(): Invalid '..'-range in %s on line %d
string(14) "  Hello World
"
===DONE===
