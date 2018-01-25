--TEST--
Test strrev() function : error conditions
--FILE--
<?php
/* Prototype  : string strrev(string $str);
 * Description: Reverse a string
 * Source code: ext/standard/string.c
*/

echo "*** Testing strrev() : error conditions ***\n";
echo "-- Testing strrev() function with Zero arguments --";
var_dump( strrev() );

echo "\n-- Testing strrev() function with more than expected no. of arguments --";
var_dump( strrev("string", 'extra_arg') );
echo "*** Done ***";
?>
--EXPECTF--
*** Testing strrev() : error conditions ***
-- Testing strrev() function with Zero arguments --
Warning: strrev() expects exactly 1 parameter, 0 given in %s on line %d
NULL

-- Testing strrev() function with more than expected no. of arguments --
Warning: strrev() expects exactly 1 parameter, 2 given in %s on line %d
NULL
*** Done ***
