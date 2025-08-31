--TEST--
Test iconv_substr() function : error conditions - Pass an unknown encoding
--EXTENSIONS--
iconv
--FILE--
<?php
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

Warning: iconv_substr(): Wrong encoding, conversion from "unknown-encoding" to "UCS-4LE" is not allowed in %s on line %d
bool(false)
Done
