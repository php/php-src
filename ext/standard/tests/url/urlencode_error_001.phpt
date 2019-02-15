--TEST--
Test urlencode() function : error conditions
--FILE--
<?php
/* Prototype  : proto string urlencode(string str)
 * Description: URL-encodes string
 * Source code: ext/standard/url.c
 * Alias to functions:
 */

// NB: basic functionality tested in tests/strings/001.phpt

echo "*** Testing urlencode() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing urlencode() function with Zero arguments --\n";
var_dump( urlencode() );

//Test urlencode with one more than the expected number of arguments
echo "\n-- Testing urlencode() function with more than expected no. of arguments --\n";
$str = 'string_val';
$extra_arg = 10;
var_dump( urlencode($str, $extra_arg) );

echo "Done";
?>
--EXPECTF--
*** Testing urlencode() : error conditions ***

-- Testing urlencode() function with Zero arguments --

Warning: urlencode() expects exactly 1 parameter, 0 given in %s on line 14
NULL

-- Testing urlencode() function with more than expected no. of arguments --

Warning: urlencode() expects exactly 1 parameter, 2 given in %s on line 20
NULL
Done
