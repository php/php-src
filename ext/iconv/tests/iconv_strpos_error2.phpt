--TEST--
Test iconv_strpos() function : error conditions - Pass unknown encoding
--SKIPIF--
<?php
extension_loaded('iconv') or die('skip');
function_exists('iconv_strpos') or die("skip iconv_strpos() is not available in this build");
?>
--FILE--
<?php
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

Warning: iconv_strpos(): Wrong encoding, conversion from "unknown-encoding" to "UCS-4LE" is not allowed in %s on line %d
bool(false)
Done
