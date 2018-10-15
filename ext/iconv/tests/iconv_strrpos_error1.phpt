--TEST--
Test iconv_strrpos() function : error conditions - pass incorrect number of args
--SKIPIF--
<?php
extension_loaded('iconv') or die('skip');
function_exists('iconv_strrpos') or die("skip iconv_strrpos() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : proto int iconv_strrpos(string haystack, string needle [, string charset])
 * Description: Find position of last occurrence of a string within another
 * Source code: ext/iconv/iconv.c
 */

/*
 * Pass iconv_strrpos() an incorrect number of arguments
 */

echo "*** Testing iconv_strrpos() : error conditions ***\n";


//Test iconv_strrpos with one more than the expected number of arguments
echo "\n-- Testing iconv_strrpos() function with more than expected no. of arguments --\n";
$haystack = 'string_val';
$needle = 'string_val';
$encoding = 'string_val';
$extra_arg = 10;
var_dump( iconv_strrpos($haystack, $needle, $encoding, $extra_arg) );

// Testing iconv_strrpos with one less than the expected number of arguments
echo "\n-- Testing iconv_strrpos() function with less than expected no. of arguments --\n";
$haystack = 'string_val';
var_dump( iconv_strrpos($haystack) );

echo "Done";
?>
--EXPECTF--
*** Testing iconv_strrpos() : error conditions ***

-- Testing iconv_strrpos() function with more than expected no. of arguments --

Warning: iconv_strrpos() expects at most 3 parameters, 4 given in %s on line %d
bool(false)

-- Testing iconv_strrpos() function with less than expected no. of arguments --

Warning: iconv_strrpos() expects at least 2 parameters, 1 given in %s on line %d
bool(false)
Done
