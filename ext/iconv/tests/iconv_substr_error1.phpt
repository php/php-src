--TEST--
Test iconv_substr() function : error conditions - Pass incorrect number of args
--SKIPIF--
<?php
extension_loaded('iconv') or die('skip');
function_exists('iconv_substr') or die("skip iconv_substr() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : string iconv_substr(string str, int offset, [int length, string charset])
 * Description: Returns part of a string
 * Source code: ext/iconv/iconv.c
 */

/*
 * Pass incorrect number of arguments to iconv_substr() to test behaviour
 */

echo "*** Testing iconv_substr() : error conditions ***\n";

//Test iconv_substr with one more than the expected number of arguments
echo "\n-- Testing iconv_substr() function with more than expected no. of arguments --\n";
$str = 'string_val';
$start = 10;
$length = 10;
$encoding = 'string_val';
$extra_arg = 10;
var_dump( iconv_substr($str, $start, $length, $encoding, $extra_arg) );

// Testing iconv_substr with one less than the expected number of arguments
echo "\n-- Testing iconv_substr() function with less than expected no. of arguments --\n";
$str = 'string_val';
var_dump( iconv_substr($str) );

echo "Done";
?>
--EXPECTF--
*** Testing iconv_substr() : error conditions ***

-- Testing iconv_substr() function with more than expected no. of arguments --

Warning: iconv_substr() expects at most 4 parameters, 5 given in %s on line %d
bool(false)

-- Testing iconv_substr() function with less than expected no. of arguments --

Warning: iconv_substr() expects at least 2 parameters, 1 given in %s on line %d
bool(false)
Done
