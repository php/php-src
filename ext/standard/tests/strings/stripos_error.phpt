--TEST--
Test stripos() function : error conditions
--FILE--
<?php
/* Prototype  : int stripos ( string $haystack, string $needle [, int $offset] );
 * Description: Find position of first occurrence of a case-insensitive string
 * Source code: ext/standard/string.c
*/

echo "*** Testing stripos() function: error conditions ***\n";

echo "\n-- Offset beyond the end of the string --\n";
try {
    stripos("Hello World", "o", 12);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

echo "\n-- Offset before the start of the string --\n";
try {
    stripos("Hello World", "o", -12);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

echo "*** Done ***";
?>
--EXPECT--
*** Testing stripos() function: error conditions ***

-- Offset beyond the end of the string --
stripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)

-- Offset before the start of the string --
stripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
*** Done ***
