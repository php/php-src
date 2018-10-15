--TEST--
Test utf8_decode() function : error conditions
--FILE--
<?php
/* Prototype  : proto string utf8_decode(string data)
 * Description: Converts a UTF-8 encoded string to ISO-8859-1
 * Source code: ext/standard/string.c
 * Alias to functions:
 */

echo "*** Testing utf8_decode() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing utf8_decode() function with Zero arguments --\n";
var_dump( utf8_decode() );

//Test utf8_decode with one more than the expected number of arguments
echo "\n-- Testing utf8_decode() function with more than expected no. of arguments --\n";
$data = 'string_val';
$extra_arg = 10;
var_dump( utf8_decode($data, $extra_arg) );

echo "Done";
?>
--EXPECTF--
*** Testing utf8_decode() : error conditions ***

-- Testing utf8_decode() function with Zero arguments --

Warning: utf8_decode() expects exactly 1 parameter, 0 given in %s on line %d
NULL

-- Testing utf8_decode() function with more than expected no. of arguments --

Warning: utf8_decode() expects exactly 1 parameter, 2 given in %s on line %d
NULL
Done
