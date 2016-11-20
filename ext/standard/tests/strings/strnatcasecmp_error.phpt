--TEST--
Test strnatcasecmp() function : error conditions 
--FILE--
<?php
/* Prototype  : int strnatcasecmp  ( string $str1  , string $str2  )
 * Description: Case insensitive string comparisons using a "natural order" algorithm
 * Source code: ext/standard/string.c
*/
echo "*** Testing strnatcasecmp() : error conditions ***\n";

echo "-- Testing strnatcmp() function with Zero arguments --\n";
var_dump( strnatcasecmp() );

echo "\n\n-- Testing strnatcasecmp() function with more than expected no. of arguments --\n";
$str1 = "abc1";
$str2 = "ABC1";
$extra_arg = 10;
var_dump( strnatcasecmp( $str1, $str2, $extra_arg) );
?>
===DONE===
--EXPECTF--
*** Testing strnatcasecmp() : error conditions ***
-- Testing strnatcmp() function with Zero arguments --

Warning: strnatcasecmp() expects exactly 2 parameters, 0 given in %s on line %d
NULL


-- Testing strnatcasecmp() function with more than expected no. of arguments --

Warning: strnatcasecmp() expects exactly 2 parameters, 3 given in %s on line %d
NULL
===DONE===