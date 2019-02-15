--TEST--
Test mb_strripos() function : error conditions - Pass incorrect number of args
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_strripos') or die("skip mb_strripos() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : int mb_strripos(string haystack, string needle [, int offset [, string encoding]])
 * Description: Finds position of last occurrence of a string within another, case insensitive
 * Source code: ext/mbstring/mbstring.c
 * Alias to functions:
 */

/*
 * Test how mb_strripos behaves when passed an incorrect number of arguments
 */

echo "*** Testing mb_strripos() : error conditions ***\n";


//Test mb_strripos with one more than the expected number of arguments
echo "\n-- Testing mb_strripos() function with more than expected no. of arguments --\n";
$haystack = 'string_val';
$needle = 'string_val';
$offset = 10;
$encoding = 'string_val';
$extra_arg = 10;
var_dump( mb_strripos($haystack, $needle, $offset, $encoding, $extra_arg) );

// Testing mb_strripos with one less than the expected number of arguments
echo "\n-- Testing mb_strripos() function with less than expected no. of arguments --\n";
$haystack = 'string_val';
var_dump( mb_strripos($haystack) );

echo "Done";
?>
--EXPECTF--
*** Testing mb_strripos() : error conditions ***

-- Testing mb_strripos() function with more than expected no. of arguments --

Warning: mb_strripos() expects at most 4 parameters, 5 given in %s on line %d
NULL

-- Testing mb_strripos() function with less than expected no. of arguments --

Warning: mb_strripos() expects at least 2 parameters, 1 given in %s on line %d
NULL
Done
