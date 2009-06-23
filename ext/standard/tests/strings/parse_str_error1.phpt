--TEST--
Test parse_str() function : non-default arg_separator.input specified
--INI--
arg_separator.input = "/" 
--FILE--
<?php
/* Prototype  : void parse_str  ( string $str  [, array &$arr  ] )
 * Description: Parses the string into variables
 * Source code: ext/standard/string.c
*/

echo "*** Testing parse_str() : error conditions ***\n";

echo "\n-- Testing htmlentities() function with less than expected no. of arguments --\n";
parse_str();
echo "\n-- Testing htmlentities() function with more than expected no. of arguments --\n";
$s1 = "first=val1&second=val2&third=val3";
parse_str($s1, $res_array, true); 

?>
===DONE===
--EXPECTF--
*** Testing parse_str() : error conditions ***

-- Testing htmlentities() function with less than expected no. of arguments --

Warning: Wrong parameter count for parse_str() in %s on line %d

-- Testing htmlentities() function with more than expected no. of arguments --

Warning: Wrong parameter count for parse_str() in %s on line %d
===DONE===