--TEST--
Test strpbrk() function : error conditions
--FILE--
<?php
/* Prototype  : array strpbrk(string haystack, string char_list)
 * Description: Search a string for any of a set of characters
 * Source code: ext/standard/string.c
 * Alias to functions:
 */

$haystack = 'This is a Simple text.';

echo "-- Testing strpbrk() function with empty second argument --\n";

try {
    strpbrk($haystack, '');
} catch (\Error $e) {
    echo $e->getMessage() . "\n";
}

?>
===DONE===
--EXPECTF--
-- Testing strpbrk() function with empty second argument --
The character list cannot be empty
===DONE===
