--TEST--
Test strrstr() function : with before_needle param set to TRUE
--FILE--
<?php
/* Prototype  : string strrstr(string $haystack, string $needle, [bool $before_needle = FALSE] );
 * Description: Finds the last occurrence of a character in a string.
 * Source code: ext/standard/string.c
*/

echo "*** Testing strrstr() function : with before_needle param set to TRUE ***\n";
var_dump( strrstr("Hello, World", ",", true) ); // Single byte needle
var_dump( strrstr("Hello, World", "Wo", true) ); // Multiple byte needle
var_dump( strrstr('Hello, World', 'H', true) ); // Get text before the first byte

echo "*** Done ***";
?>
--EXPECT--
*** Testing strrstr() function : with before_needle param set to TRUE ***
string(5) "Hello"
string(7) "Hello, "
string(0) ""
*** Done ***