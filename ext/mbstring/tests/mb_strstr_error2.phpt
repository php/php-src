--TEST--
Test mb_strstr() function : error conditions
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_strstr') or die("skip mb_strstr() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : string mb_strstr(string haystack, string needle[, bool part[, string encoding]])
 * Description: Finds first occurrence of a string within another
 * Source code: ext/mbstring/mbstring.c
 * Alias to functions:
 */

echo "*** Testing mb_strstr() : error conditions ***\n";


echo "\n-- Testing mb_strstr() with unknown encoding --\n";
$haystack = 'Hello, world';
$needle = 'world';
$encoding = 'unknown-encoding';
$part = true;
var_dump( mb_strstr($haystack, $needle, $part, $encoding) );

?>
===DONE===
--EXPECTF--
*** Testing mb_strstr() : error conditions ***

-- Testing mb_strstr() with unknown encoding --

Warning: mb_strstr(): Unknown encoding "unknown-encoding" in %s on line %d
bool(false)
===DONE===
