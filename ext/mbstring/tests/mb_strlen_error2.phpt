--TEST--
Test mb_strlen() function : error conditions - pass an unknown encoding
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_strlen') or die("skip mb_strlen() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : int mb_strlen(string $str [, string $encoding])
 * Description: Get character numbers of a string
 * Source code: ext/mbstring/mbstring.c
 */

/*
 * Test mb_strlen when passed an unknown encoding
 */

echo "*** Testing mb_strlen() : error ***\n";

$string = 'abcdef';

$encoding = 'unknown-encoding';

var_dump(mb_strlen($string, $encoding));

echo "Done";
?>
--EXPECTF--
*** Testing mb_strlen() : error ***

Warning: mb_strlen(): Unknown encoding "unknown-encoding" in %s on line %d
bool(false)
Done
