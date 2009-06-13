--TEST--
Test iconv_substr() function : error conditions - Pass an unknown encoding
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
 * Pass an unknown encoding to iconv_substr() to test behaviour
 */

echo "*** Testing iconv_substr() : error conditions ***\n";

$str = 'Hello, world';
$start = 1;
$length = 5;
$encoding = 'unknown-encoding';

var_dump( iconv_substr($str, $start, $length, $encoding));

echo "Done";
?>
--EXPECTF--
*** Testing iconv_substr() : error conditions ***

Notice: iconv_substr(): Wrong charset, conversion from `unknown-encoding' to `UCS-4LE' is not allowed in %s on line %d
bool(false)
Done

