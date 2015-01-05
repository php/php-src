--TEST--
Test iconv_strpos() function : error conditions - Pass unknown encoding
--SKIPIF--
<?php
extension_loaded('iconv') or die('skip');
function_exists('iconv_strpos') or die("skip iconv_strpos() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : int iconv_strpos(string haystack, string needle [, int offset [, string charset]])
 * Description: Find position of first occurrence of a string within another 
 * Source code: ext/iconv/iconv.c
 */

/*
 * Pass an unknown encoding to iconv_strpos() to test behaviour
 */

echo "*** Testing iconv_strpos() : error conditions ***\n";
$haystack = 'Hello, world';
$needle = 'world';
$offset = 2;
$encoding = 'unknown-encoding';

var_dump( iconv_strpos($haystack, $needle, $offset, $encoding) );

echo "Done";
?>
--EXPECTF--
*** Testing iconv_strpos() : error conditions ***

Notice: iconv_strpos(): Wrong charset, conversion from `unknown-encoding' to `UCS-4LE' is not allowed in %s on line %d
bool(false)
Done
