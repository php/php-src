--TEST--
Test parse_str() function : non-default arg_separator.input specified
--INI--
arg_separator.input = "/"
--FILE--
<?php
/* Prototype  : void parse_str  ( string $str , array &$arr )
 * Description: Parses the string into variables
 * Source code: ext/standard/string.c
*/

echo "*** Testing parse_str() : error conditions ***\n";

echo "\n-- Testing parse_str() function with less than expected no. of arguments --\n";
parse_str();
echo "\n-- Testing parse_str() function with more than expected no. of arguments --\n";
$s1 = "first=val1&second=val2&third=val3";
parse_str($s1, $res_array, true);

?>
===DONE===
--EXPECTF--
*** Testing parse_str() : error conditions ***

-- Testing parse_str() function with less than expected no. of arguments --

Warning: parse_str() expects exactly 2 parameters, 0 given in %s on line %d

-- Testing parse_str() function with more than expected no. of arguments --

Warning: parse_str() expects exactly 2 parameters, 3 given in %s on line %d
===DONE===
