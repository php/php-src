--TEST--
Test utf8_encode() function : error conditions
--FILE--
<?php
/* Prototype  : proto string utf8_encode(string data)
 * Description: Encodes an ISO-8859-1 string to UTF-8
 * Source code: ext/standard/string.c
 * Alias to functions:
 */

echo "*** Testing utf8_encode() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing utf8_encode() function with Zero arguments --\n";
var_dump( utf8_encode() );

//Test utf8_encode with one more than the expected number of arguments
echo "\n-- Testing utf8_encode() function with more than expected no. of arguments --\n";
$data = 'string_val';
$extra_arg = 10;
var_dump( utf8_encode($data, $extra_arg) );

echo "Done";
?>
--EXPECTF--
*** Testing utf8_encode() : error conditions ***

-- Testing utf8_encode() function with Zero arguments --

Warning: utf8_encode() expects exactly 1 parameter, 0 given in %s on line %d
NULL

-- Testing utf8_encode() function with more than expected no. of arguments --

Warning: utf8_encode() expects exactly 1 parameter, 2 given in %s on line %d
NULL
Done
