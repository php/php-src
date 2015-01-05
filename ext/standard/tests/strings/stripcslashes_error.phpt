--TEST--
Test stripcslashes() function :  error conditions 
--FILE--
<?php

/* Prototype  : string stripcslashes  ( string $str  )
 * Description: Returns a string with backslashes stripped off. Recognizes C-like \n, \r ..., 
 *              octal and hexadecimal representation.
 * Source code: ext/standard/string.c
*/

echo "*** Testing stripcslashes() : unexpected number of arguments ***";


echo "\n-- Testing stripcslashes() function with no arguments --\n";
var_dump( stripcslashes() );

echo "\n-- Testing stripcslashes() function with more than expected no. of arguments --\n";
$extra_arg = 10;
var_dump( stripcslashes("abc def", $extra_arg) );
?>
===DONE===
--EXPECTF--
*** Testing stripcslashes() : unexpected number of arguments ***
-- Testing stripcslashes() function with no arguments --

Warning: stripcslashes() expects exactly 1 parameter, 0 given in %s on line %d
NULL

-- Testing stripcslashes() function with more than expected no. of arguments --

Warning: stripcslashes() expects exactly 1 parameter, 2 given in %s on line %d
NULL
===DONE===
