--TEST--
Test stripos() function : error conditions
--FILE--
<?php
/* Prototype  : int stripos ( string $haystack, string $needle [, int $offset] );
 * Description: Find position of first occurrence of a case-insensitive string
 * Source code: ext/standard/string.c
*/

echo "*** Testing stripos() function: error conditions ***\n";

echo "\n-- Offset beyond the end of the string --";
var_dump( stripos("Hello World", "o", 12) );

echo "\n-- Offset before the start of the string --";
var_dump( stripos("Hello World", "o", -12) );

echo "*** Done ***";
?>
--EXPECTF--
*** Testing stripos() function: error conditions ***

-- Offset beyond the end of the string --
Warning: stripos(): Offset not contained in string in %s on line %d
bool(false)

-- Offset before the start of the string --
Warning: stripos(): Offset not contained in string in %s on line %d
bool(false)
*** Done ***
