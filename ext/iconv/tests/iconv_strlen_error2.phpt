--TEST--
Test iconv_strlen() function : error conditions - pass an unknown encoding
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
 * Test iconv_strlen when passed an unknown encoding
 */

echo "*** Testing iconv_strlen() : error ***\n";

$string = 'abcdef';

$encoding = 'unknown-encoding';

var_dump(iconv_strlen($string, $encoding));

?>
===DONE===
--EXPECTF--
*** Testing iconv_strlen() : error ***

Notice: iconv_strlen(): Wrong charset, conversion from `unknown-encoding' to `UCS-4LE' is not allowed in %s on line %d
bool(false)
===DONE===
