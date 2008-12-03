--TEST--
Test mb_stripos() function : error conditions - Pass unknown encoding
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_stripos') or die("skip mb_stripos() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : int mb_stripos(string haystack, string needle [, int offset [, string encoding]])
 * Description: Finds position of first occurrence of a string within another, case insensitive 
 * Source code: ext/mbstring/mbstring.c
 * Alias to functions: 
 */

/*
 * Pass an unknown encoding to mb_stripos() to test behaviour
 */

echo "*** Testing mb_stripos() : error conditions ***\n";
$haystack = b'Hello, world';
$needle = b'world';
$offset = 2;
$encoding = 'unknown-encoding';

var_dump( mb_stripos($haystack, $needle, $offset, $encoding) );

echo "Done";
?>
--EXPECTF--
*** Testing mb_stripos() : error conditions ***

Warning: mb_stripos(): Unknown encoding "unknown-encoding" in %s on line %d
bool(false)
Done
