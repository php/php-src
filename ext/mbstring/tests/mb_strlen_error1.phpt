--TEST--
Test mb_strlen() function : error conditions - pass incorrect number of args
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_strlen') or die("skip mb_strlen() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : int mb_strlen(string $str [, string $encoding])
 * Description: Get character numbers of a string
 * Source code: ext/mbstring/mbstring.c
 */

/*
 * Pass mb_strlen an incorrect number of arguments to test behaviour
 */

echo "*** Testing mb_strlen() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing mb_strlen() function with Zero arguments --\n";
var_dump( mb_strlen() );

//Test mb_strlen with one more than the expected number of arguments
echo "\n-- Testing mb_strlen() function with more than expected no. of arguments --\n";
$str = 'string_val';
$encoding = 'string_val';
$extra_arg = 10;
var_dump( mb_strlen($str, $encoding, $extra_arg) );

echo "Done";
?>
--EXPECTF--
*** Testing mb_strlen() : error conditions ***

-- Testing mb_strlen() function with Zero arguments --

Warning: mb_strlen() expects at least 1 parameter, 0 given in %s on line %d
NULL

-- Testing mb_strlen() function with more than expected no. of arguments --

Warning: mb_strlen() expects at most 2 parameters, 3 given in %s on line %d
NULL
Done
