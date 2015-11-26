--TEST--
Test nl2br() function : error conditions 
--FILE--
<?php
/* Prototype  : string nl2br(string $str)
 * Description: Inserts HTML line breaks before all newlines in a string.
 * Source code: ext/standard/string.c
*/

echo "*** Testing nl2br() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing nl2br() function with Zero arguments --";
var_dump( nl2br() );

//Test nl2br with one more than the expected number of arguments
echo "\n-- Testing nl2br() function with more than expected no. of arguments --";
$str = 'string_val';
$extra_arg = 10;
var_dump( nl2br($str, true, $extra_arg) );

echo "Done";
?>
--EXPECTF--
*** Testing nl2br() : error conditions ***

-- Testing nl2br() function with Zero arguments --
Warning: nl2br() expects at least 1 parameter, 0 given in %s on line %d
NULL

-- Testing nl2br() function with more than expected no. of arguments --
Warning: nl2br() expects at most 2 parameters, 3 given in %s on line %d
NULL
Done
