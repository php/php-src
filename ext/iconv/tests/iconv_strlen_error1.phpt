--TEST--
Test iconv_strlen() function : error conditions - pass incorrect number of args
--SKIPIF--
<?php
extension_loaded('iconv') or die('skip');
function_exists('iconv_strlen') or die("skip iconv_strlen() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : int iconv_strlen(string str [, string charset])
 * Description: Get character numbers of a string 
 * Source code: ext/iconv/iconv.c
 */

/*
 * Pass iconv_strlen an incorrect number of arguments to test behaviour
 */

echo "*** Testing iconv_strlen() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing iconv_strlen() function with Zero arguments --\n";
var_dump( iconv_strlen() );

//Test iconv_strlen with one more than the expected number of arguments
echo "\n-- Testing iconv_strlen() function with more than expected no. of arguments --\n";
$str = 'string_val';
$encoding = 'string_val';
$extra_arg = 10;
var_dump( iconv_strlen($str, $encoding, $extra_arg) );
?>
===DONE===
--EXPECTF--
*** Testing iconv_strlen() : error conditions ***

-- Testing iconv_strlen() function with Zero arguments --

Warning: iconv_strlen() expects at least 1 parameter, 0 given in %s on line %d
bool(false)

-- Testing iconv_strlen() function with more than expected no. of arguments --

Warning: iconv_strlen() expects at most 2 parameters, 3 given in %s on line %d
bool(false)
===DONE===
