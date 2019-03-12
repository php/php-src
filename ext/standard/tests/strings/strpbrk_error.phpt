--TEST--
Test strpbrk() function : error conditions
--FILE--
<?php
/* Prototype  : array strpbrk(string haystack, string char_list)
 * Description: Search a string for any of a set of characters
 * Source code: ext/standard/string.c
 * Alias to functions:
 */

echo "*** Testing strpbrk() : error conditions ***\n";

$haystack = 'This is a Simple text.';

echo "\n-- Testing strpbrk() function with empty second argument --\n";
var_dump( strpbrk($haystack, '') );

?>
===DONE===
--EXPECTF--
*** Testing strpbrk() : error conditions ***

-- Testing strpbrk() function with empty second argument --

Warning: strpbrk(): The character list cannot be empty in %s on line %d
bool(false)
===DONE===
