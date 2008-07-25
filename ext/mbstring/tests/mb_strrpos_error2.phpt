--TEST--
Test mb_strrpos() function : error conditions - pass an unknown encoding
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_strrpos') or die("skip mb_strrpos() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : int mb_strrpos(string $haystack, string $needle [, int $offset [, string $encoding]])
 * Description: Find position of last occurrence of a string within another 
 * Source code: ext/mbstring/mbstring.c
 */

/*
 * Pass mb_strrpos() an encoding that doesn't exist
 */

echo "*** Testing mb_strrpos() : error conditions ***\n";

$haystack = 'This is an English string. 0123456789.';
$needle = '123';
$offset = 5;
$encoding = 'unknown-encoding';

var_dump(mb_strrpos($haystack, $needle , $offset, $encoding));

echo "Done";
?>
--EXPECTF--
*** Testing mb_strrpos() : error conditions ***

Warning: mb_strrpos(): Unknown encoding "unknown-encoding" in %s on line %d
bool(false)
Done
