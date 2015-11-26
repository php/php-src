--TEST--
Test sscanf() function : error conditions 
--FILE--
<?php

/* Prototype  : mixed sscanf  ( string $str  , string $format  [, mixed &$...  ] )
 * Description: Parses input from a string according to a format
 * Source code: ext/standard/string.c
*/
echo "*** Testing sscanf() : error conditions ***\n";

$str = "Hello World";
$format = "%s %s";

echo "\n-- Testing sscanf() function with no arguments --\n";
var_dump( sscanf() );

echo "\n-- Testing sscanf() function with one argument --\n";
var_dump( sscanf($str) );

echo "\n-- Testing sscanf() function with more than expected no. of arguments --\n";

var_dump( sscanf($str, $format, $str1, $str2, $extra_str) );

?>
===DONE===
--EXPECTF--
*** Testing sscanf() : error conditions ***

-- Testing sscanf() function with no arguments --

Warning: sscanf() expects at least 2 parameters, 0 given in %s on line %d
NULL

-- Testing sscanf() function with one argument --

Warning: sscanf() expects at least 2 parameters, 1 given in %s on line %d
NULL

-- Testing sscanf() function with more than expected no. of arguments --

Warning: sscanf(): Variable is not assigned by any conversion specifiers in %s on line %d
int(-1)
===DONE===