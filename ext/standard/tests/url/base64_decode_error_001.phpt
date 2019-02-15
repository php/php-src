--TEST--
Test base64_decode() function : error conditions - wrong number of args
--FILE--
<?php
/* Prototype  : proto string base64_decode(string str[, bool strict])
 * Description: Decodes string using MIME base64 algorithm
 * Source code: ext/standard/base64.c
 * Alias to functions:
 */

echo "*** Testing base64_decode() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing base64_decode() function with Zero arguments --\n";
var_dump( base64_decode() );

//Test base64_decode with one more than the expected number of arguments
echo "\n-- Testing base64_decode() function with more than expected no. of arguments --\n";
$str = 'string_val';
$strict = true;
$extra_arg = 10;
var_dump( base64_decode($str, $strict, $extra_arg) );

echo "Done";
?>
--EXPECTF--
*** Testing base64_decode() : error conditions ***

-- Testing base64_decode() function with Zero arguments --

Warning: base64_decode() expects at least 1 parameter, 0 given in %s on line 12
NULL

-- Testing base64_decode() function with more than expected no. of arguments --

Warning: base64_decode() expects at most 2 parameters, 3 given in %s on line 19
NULL
Done
