--TEST--
Test mb_strrichr() function : error conditions 
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_strrichr') or die("skip mb_strrichr() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : string mb_strrichr(string haystack, string needle[, bool part[, string encoding]])
 * Description: Finds the last occurrence of a character in a string within another, case insensitive 
 * Source code: ext/mbstring/mbstring.c
 * Alias to functions: 
 */

echo "*** Testing mb_strrichr() : error conditions ***\n";


echo "\n-- Testing mb_strrichr() with unknown encoding --\n";
$haystack = b'Hello, world';
$needle = b'world';
$encoding = 'unknown-encoding';
$part = true;
var_dump( mb_strrichr($haystack, $needle, $part, $encoding) );

?>
===DONE===
--EXPECTF--
*** Testing mb_strrichr() : error conditions ***

-- Testing mb_strrichr() with unknown encoding --

Warning: mb_strrichr(): Unknown encoding "unknown-encoding" in %s on line %d
bool(false)
===DONE===
