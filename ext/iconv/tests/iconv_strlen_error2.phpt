--TEST--
Test iconv_strlen() function : error conditions - pass an unknown encoding
--EXTENSIONS--
iconv
--FILE--
<?php
/*
 * Test iconv_strlen when passed an unknown encoding
 */

echo "*** Testing iconv_strlen() : error ***\n";

$string = 'abcdef';

$encoding = 'unknown-encoding';

var_dump(iconv_strlen($string, $encoding));

?>
--EXPECTF--
*** Testing iconv_strlen() : error ***

Warning: iconv_strlen(): Wrong encoding, conversion from "unknown-encoding" to "UCS-4LE" is not allowed in %s on line %d
bool(false)
