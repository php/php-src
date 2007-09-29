--TEST--
Test strrchr() function : error conditions 
--FILE--
<?php
/* Prototype  : string strrchr(string $haystack, string $needle);
 * Description: Finds the last occurrence of a character in a string. 
 * Source code: ext/standard/string.c
*/

echo "*** Testing strrchr() function: error conditions ***\n";
$haystack = "Hello";
$needle = "Hello";
$extra_arg = "Hello";

echo "\n-- Testing strrchr() function with Zero arguments --";
var_dump( strrchr() );

echo "\n-- Testing strrchr() function with less than expected no. of arguments --";
var_dump( strrchr($haystack) );

echo "\n-- Testing strrchr() function with more than expected no. of arguments --";
var_dump( strrchr($haystack, $needle, $extra_arg) );

echo "*** Done ***";
?>
--EXPECTF--
*** Testing strrchr() function: error conditions ***

-- Testing strrchr() function with Zero arguments --
Warning: Wrong parameter count for strrchr() in %s on line %d
NULL

-- Testing strrchr() function with less than expected no. of arguments --
Warning: Wrong parameter count for strrchr() in %s on line %d
NULL

-- Testing strrchr() function with more than expected no. of arguments --
Warning: Wrong parameter count for strrchr() in %s on line %d
NULL
*** Done ***
