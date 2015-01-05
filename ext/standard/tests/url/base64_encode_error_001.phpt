--TEST--
Test base64_encode() function : error conditions - wrong number of args
--FILE--
<?php
/* Prototype  : proto string base64_encode(string str)
 * Description: Encodes string using MIME base64 algorithm 
 * Source code: ext/standard/base64.c
 * Alias to functions: 
 */

echo "*** Testing base64_encode() : error conditions - wrong number of args ***\n";

// Zero arguments
echo "\n-- Testing base64_encode() function with Zero arguments --\n";
var_dump( base64_encode() );

//Test base64_encode with one more than the expected number of arguments
echo "\n-- Testing base64_encode() function with more than expected no. of arguments --\n";
$str = 'string_val';
$extra_arg = 10;
var_dump( base64_encode($str, $extra_arg) );

echo "Done";
?>
--EXPECTF--
*** Testing base64_encode() : error conditions - wrong number of args ***

-- Testing base64_encode() function with Zero arguments --

Warning: base64_encode() expects exactly 1 parameter, 0 given in %s on line 12
NULL

-- Testing base64_encode() function with more than expected no. of arguments --

Warning: base64_encode() expects exactly 1 parameter, 2 given in %s on line 18
NULL
Done