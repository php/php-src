--TEST--
Test mb_stristr() function : error conditions
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_stristr') or die("skip mb_stristr() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : string mb_stristr(string haystack, string needle[, bool part[, string encoding]])
 * Description: Finds first occurrence of a string within another, case insensitive
 * Source code: ext/mbstring/mbstring.c
 * Alias to functions:
 */

echo "*** Testing mb_stristr() : error conditions ***\n";


echo "\n-- Testing mb_stristr() with unknown encoding --\n";
$haystack = 'Hello, world';
$needle = 'world';
$encoding = 'unknown-encoding';
$part = true;
var_dump( mb_stristr($haystack, $needle, $part, $encoding) );

?>
===DONE===
--EXPECTF--
*** Testing mb_stristr() : error conditions ***

-- Testing mb_stristr() with unknown encoding --

Warning: mb_stristr(): Unknown encoding "unknown-encoding" in %s on line %d
bool(false)
===DONE===
