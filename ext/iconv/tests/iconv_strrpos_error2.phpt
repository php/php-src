--TEST--
Test iconv_strrpos() function : error conditions - pass an unknown encoding
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
 * Pass iconv_strrpos() an encoding that doesn't exist
 */

echo "*** Testing iconv_strrpos() : error conditions ***\n";

$haystack = 'This is an English string. 0123456789.';
$needle = '123';
$offset = 5;
$encoding = 'unknown-encoding';

var_dump(iconv_strrpos($haystack, $needle , $encoding));

echo "Done";
?>
--EXPECTF--
*** Testing iconv_strrpos() : error conditions ***

Notice: iconv_strrpos(): Wrong charset, conversion from `unknown-encoding' to `UCS-4LE' is not allowed in %s on line %d
bool(false)
Done