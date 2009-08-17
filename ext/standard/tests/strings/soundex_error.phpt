--TEST--
Test soundex() function : error conditions
--FILE--
<?php
/* Prototype  : string soundex  ( string $str  )
 * Description: Calculate the soundex key of a string
 * Source code: ext/standard/string.c
*/
		
echo "\n*** Testing soundex error conditions ***";

echo "-- Testing soundex() function with Zero arguments --\n";
var_dump( soundex() );

echo "\n\n-- Testing soundex() function with more than expected no. of arguments --\n";
$str = "Euler";
$extra_arg = 10;
var_dump( soundex( $str, $extra_arg) );

?> 
===DONE===
--EXPECTF--
*** Testing soundex error conditions ***-- Testing soundex() function with Zero arguments --

Warning: soundex() expects exactly 1 parameter, 0 given in %s on line %d
NULL


-- Testing soundex() function with more than expected no. of arguments --

Warning: soundex() expects exactly 1 parameter, 2 given in %s on line %d
NULL
 
===DONE===