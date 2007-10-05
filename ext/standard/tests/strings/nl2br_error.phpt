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
var_dump( nl2br($str, $extra_arg) );

echo "Done";
?>
--EXPECTF--
*** Testing nl2br() : error conditions ***

-- Testing nl2br() function with Zero arguments --
Warning: Wrong parameter count for nl2br() in %s on line %d
NULL

-- Testing nl2br() function with more than expected no. of arguments --
Warning: Wrong parameter count for nl2br() in %s on line %d
NULL
Done
