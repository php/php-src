--TEST--
Test mb_substr_count() function : error conditions - pass incorrect number of arguments
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_substr_count') or die("skip mb_substr_count() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : int mb_substr_count(string $haystack, string $needle [, string $encoding])
 * Description: Count the number of substring occurrences
 * Source code: ext/mbstring/mbstring.c
 */

/*
 * Pass an incorrect number of arguments to mb_substr_count() to test behaviour
 */

echo "*** Testing mb_substr_count() : error conditions ***\n";


//Test mb_substr_count with one more than the expected number of arguments
echo "\n-- Testing mb_substr_count() function with more than expected no. of arguments --\n";
$haystack = 'string_val';
$needle = 'val';
$encoding = 'utf-8';
$extra_arg = 10;
var_dump( mb_substr_count($haystack, $needle, $encoding, $extra_arg) );

// Testing mb_substr_count with one less than the expected number of arguments
echo "\n-- Testing mb_substr_count() function with less than expected no. of arguments --\n";
$haystack = 'string_val';
var_dump( mb_substr_count($haystack) );

echo "Done";
?>
--EXPECTF--
*** Testing mb_substr_count() : error conditions ***

-- Testing mb_substr_count() function with more than expected no. of arguments --

Warning: mb_substr_count() expects at most 3 parameters, 4 given in %s on line %d
NULL

-- Testing mb_substr_count() function with less than expected no. of arguments --

Warning: mb_substr_count() expects at least 2 parameters, 1 given in %s on line %d
NULL
Done
