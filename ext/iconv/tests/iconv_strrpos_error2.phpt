--TEST--
Test iconv_strrpos() function : error conditions - pass an unknown encoding
--EXTENSIONS--
iconv
--FILE--
<?php
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

Warning: iconv_strrpos(): Wrong encoding, conversion from "unknown-encoding" to "UCS-4LE" is not allowed in %s on line %d
bool(false)
Done
