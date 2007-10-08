--TEST--
Test strtr() function : error conditions 
--FILE--
<?php
/* Prototype  : string strtr(string str, string from[, string to])
 * Description: Translates characters in str using given translation tables 
 * Source code: ext/standard/string.c
*/

echo "*** Testing strtr() : error conditions ***\n";
$str = "string";
$from = "string";
$to = "STRING";
$extra_arg = "extra_argument";

echo "\n-- Testing strtr() function with Zero arguments --";
var_dump( strtr() );

echo "\n-- Testing strtr() function with less than expected no. of arguments --";
var_dump( strtr($str) );

echo "\n-- Testing strtr() function with more than expected no. of arguments --";
var_dump( strtr($str, $from, $to, $extra_arg) );

echo "Done";
?>
--EXPECTF--
*** Testing strtr() : error conditions ***

-- Testing strtr() function with Zero arguments --
Warning: Wrong parameter count for strtr() in %s on line %d
NULL

-- Testing strtr() function with less than expected no. of arguments --
Warning: Wrong parameter count for strtr() in %s on line %d
NULL

-- Testing strtr() function with more than expected no. of arguments --
Warning: Wrong parameter count for strtr() in %s on line %d
NULL
Done
