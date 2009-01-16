--TEST--
Test rtrim() function : error conditions 
--FILE--
<?php

/* Prototype  : string rtrim  ( string $str  [, string $charlist  ] )
 * Description: Strip whitespace (or other characters) from the end of a string.
 * Source code: ext/standard/string.c
*/


echo "*** Testing rtrim() : error conditions ***\n";

echo "\n-- Testing rtrim() function with no arguments --\n";
var_dump( rtrim() );

echo "\n-- Testing rtrim() function with more than expected no. of arguments --\n";
$extra_arg = 10;
var_dump( rtrim("Hello World",  "Heo", $extra_arg) );


$hello = "  Hello World\n";
echo "\n-- Test rtrim function with various invalid charlists\n";
var_dump(rtrim($hello, "..a"));
var_dump(rtrim($hello, "a.."));
var_dump(rtrim($hello, "z..a"));
var_dump(rtrim($hello, "a..b..c"));

?>
===DONE===
--EXPECTF--
*** Testing rtrim() : error conditions ***

-- Testing rtrim() function with no arguments --

Warning: Wrong parameter count for rtrim() in %s on line %d
NULL

-- Testing rtrim() function with more than expected no. of arguments --

Warning: Wrong parameter count for rtrim() in %s on line %d
NULL

-- Test rtrim function with various invalid charlists

Warning: rtrim(): Invalid '..'-range, no character to the left of '..' in %s on line %d
string(14) "  Hello World
"

Warning: rtrim(): Invalid '..'-range, no character to the right of '..' in %s on line %d
string(14) "  Hello World
"

Warning: rtrim(): Invalid '..'-range, '..'-range needs to be incrementing in %s on line %d
string(14) "  Hello World
"

Warning: rtrim(): Invalid '..'-range in %s on line %d
string(14) "  Hello World
"
===DONE===