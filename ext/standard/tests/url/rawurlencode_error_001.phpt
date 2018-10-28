--TEST--
Test rawurlencode() function : error conditions
--FILE--
<?php
/* Prototype  : proto string rawurlencode(string str)
 * Description: URL-encodes string
 * Source code: ext/standard/url.c
 * Alias to functions:
 */

// NB: basic functionality tested in tests/strings/001.phpt

echo "*** Testing rawurlencode() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing rawurlencode() function with Zero arguments --\n";
var_dump( rawurlencode() );

//Test rawurlencode with one more than the expected number of arguments
echo "\n-- Testing rawurlencode() function with more than expected no. of arguments --\n";
$str = 'string_val';
$extra_arg = 10;
var_dump( rawurlencode($str, $extra_arg) );

echo "Done";
?>
--EXPECTF--
*** Testing rawurlencode() : error conditions ***

-- Testing rawurlencode() function with Zero arguments --

Warning: rawurlencode() expects exactly 1 parameter, 0 given in %s on line 14
NULL

-- Testing rawurlencode() function with more than expected no. of arguments --

Warning: rawurlencode() expects exactly 1 parameter, 2 given in %s on line 20
NULL
Done
