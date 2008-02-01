--TEST--
Test mb_strtolower() function : error conditions - pass incorrect number of arguments
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_strtolower') or die("skip mb_strtolower() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : string mb_strtolower(string $sourcestring [, string $encoding])
 * Description: Returns a lowercased version of $sourcestring
 * Source code: ext/mbstring/mbstring.c
 */

/*
 * Pass an incorrect number of arguments to mb_strtolower() to test behaviour
 */

echo "*** Testing mb_strtolower() : error conditions***\n";

//Test mb_strtolower with one more than the expected number of arguments
echo "\n-- Testing mb_strtolower() function with more than expected no. of arguments --\n";
$sourcestring = 'string_value';
$encoding = 'UTF-8';
$extra_arg = 10;
var_dump( mb_strtolower($sourcestring, $encoding, $extra_arg) );

//Test mb_strtolower with zero arguments
echo "\n-- Testing mb_strtolower() function with zero arguments --\n";
var_dump( mb_strtolower() );

echo "Done";
?>
--EXPECTF--
*** Testing mb_strtolower() : error conditions***

-- Testing mb_strtolower() function with more than expected no. of arguments --

Warning: mb_strtolower() expects at most 2 parameters, 3 given in %s on line %d
NULL

-- Testing mb_strtolower() function with zero arguments --

Warning: mb_strtolower() expects at least 1 parameter, 0 given in %s on line %d
NULL
Done