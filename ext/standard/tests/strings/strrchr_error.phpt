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
Warning: strrchr() expects exactly 2 parameters, 0 given in %s on line %d
NULL

-- Testing strrchr() function with less than expected no. of arguments --
Warning: strrchr() expects exactly 2 parameters, 1 given in %s on line %d
NULL

-- Testing strrchr() function with more than expected no. of arguments --
Warning: strrchr() expects exactly 2 parameters, 3 given in %s on line %d
NULL
*** Done ***
