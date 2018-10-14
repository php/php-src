--TEST--
Test mb_strpos() function : error conditions - Pass incorrect number of args
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_strpos') or die("skip mb_strpos() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : int mb_strpos(string $haystack, string $needle [, int $offset [, string $encoding]])
 * Description: Find position of first occurrence of a string within another
 * Source code: ext/mbstring/mbstring.c
 */

/*
 * Test how mb_strpos behaves when passed an incorrect number of arguments
 */

echo "*** Testing mb_strpos() : error conditions ***\n";


//Test mb_strpos with one more than the expected number of arguments
echo "\n-- Testing mb_strpos() function with more than expected no. of arguments --\n";
$haystack = 'string_val';
$needle = 'string_val';
$offset = 10;
$encoding = 'string_val';
$extra_arg = 10;
var_dump( mb_strpos($haystack, $needle, $offset, $encoding, $extra_arg) );

// Testing mb_strpos with one less than the expected number of arguments
echo "\n-- Testing mb_strpos() function with less than expected no. of arguments --\n";
$haystack = 'string_val';
var_dump( mb_strpos($haystack) );

echo "Done";
?>
--EXPECTF--
*** Testing mb_strpos() : error conditions ***

-- Testing mb_strpos() function with more than expected no. of arguments --

Warning: mb_strpos() expects at most 4 parameters, 5 given in %s on line %d
NULL

-- Testing mb_strpos() function with less than expected no. of arguments --

Warning: mb_strpos() expects at least 2 parameters, 1 given in %s on line %d
NULL
Done
